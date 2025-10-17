/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-16     AI Assistant first version - Audio Capture Implementation
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "audio_capture.h"

#define DBG_TAG "audio.capture"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/* 音频采集控制结构 */
static struct {
    audio_capture_state_t state;
    rt_device_t adc_dev;
    rt_thread_t capture_thread;
    rt_mutex_t lock;
    rt_sem_t sem;
    audio_capture_callback callback;
    uint8_t buffer[AUDIO_BUFFER_SIZE];
    uint32_t buffer_pos;
} audio_capture_ctrl = {
    .state = AUDIO_CAPTURE_IDLE,
    .adc_dev = RT_NULL,
    .capture_thread = RT_NULL,
    .lock = RT_NULL,
    .sem = RT_NULL,
    .callback = RT_NULL,
    .buffer_pos = 0
};

/* 音频采集线程 */
static void audio_capture_thread_entry(void *parameter)
{
    uint16_t adc_value;
    uint8_t *buf_ptr;
    
    LOG_I("Audio capture thread started");
    
    while (audio_capture_ctrl.state == AUDIO_CAPTURE_RECORDING)
    {
        /* 模拟从ADC读取音频数据 */
        /* 实际应用中，这里应该从真实的音频设备读取 */
        if (audio_capture_ctrl.adc_dev)
        {
            /* 读取ADC值并转换为音频数据 */
            rt_device_read(audio_capture_ctrl.adc_dev, 0, &adc_value, sizeof(adc_value));
            
            rt_mutex_take(audio_capture_ctrl.lock, RT_WAITING_FOREVER);
            
            buf_ptr = &audio_capture_ctrl.buffer[audio_capture_ctrl.buffer_pos];
            /* 将16位ADC值存入缓冲区 */
            buf_ptr[0] = (adc_value >> 8) & 0xFF;
            buf_ptr[1] = adc_value & 0xFF;
            audio_capture_ctrl.buffer_pos += 2;
            
            /* 缓冲区满时触发回调或信号量 */
            if (audio_capture_ctrl.buffer_pos >= AUDIO_BUFFER_SIZE)
            {
                if (audio_capture_ctrl.callback)
                {
                    audio_capture_ctrl.callback(audio_capture_ctrl.buffer, 
                                               audio_capture_ctrl.buffer_pos);
                }
                audio_capture_ctrl.buffer_pos = 0;
                rt_sem_release(audio_capture_ctrl.sem);
            }
            
            rt_mutex_release(audio_capture_ctrl.lock);
        }
        
        /* 根据采样率延时 */
        rt_thread_mdelay(1000 / AUDIO_SAMPLE_RATE);
    }
    
    LOG_I("Audio capture thread stopped");
}

/* 初始化音频采集 */
int audio_capture_init(void)
{
    if (audio_capture_ctrl.lock != RT_NULL)
    {
        LOG_W("Audio capture already initialized");
        return RT_EOK;
    }
    
    /* 创建互斥锁 */
    audio_capture_ctrl.lock = rt_mutex_create("audio_lock", RT_IPC_FLAG_PRIO);
    if (audio_capture_ctrl.lock == RT_NULL)
    {
        LOG_E("Failed to create audio mutex");
        return -RT_ERROR;
    }
    
    /* 创建信号量 */
    audio_capture_ctrl.sem = rt_sem_create("audio_sem", 0, RT_IPC_FLAG_FIFO);
    if (audio_capture_ctrl.sem == RT_NULL)
    {
        LOG_E("Failed to create audio semaphore");
        rt_mutex_delete(audio_capture_ctrl.lock);
        return -RT_ERROR;
    }
    
    /* 查找并打开ADC设备（如果存在）*/
    /* 注意：需要根据实际硬件配置ADC设备名称 */
    audio_capture_ctrl.adc_dev = rt_device_find("adc1");
    if (audio_capture_ctrl.adc_dev)
    {
        if (rt_device_open(audio_capture_ctrl.adc_dev, RT_DEVICE_OFLAG_RDONLY) != RT_EOK)
        {
            LOG_W("Failed to open ADC device, will use simulated data");
            audio_capture_ctrl.adc_dev = RT_NULL;
        }
    }
    
    audio_capture_ctrl.state = AUDIO_CAPTURE_IDLE;
    
    LOG_I("Audio capture initialized (Sample rate: %d Hz, Channels: %d, Bits: %d)",
          AUDIO_SAMPLE_RATE, AUDIO_CHANNELS, AUDIO_BITS_PER_SAMPLE);
    
    return RT_EOK;
}

/* 开始音频采集 */
int audio_capture_start(void)
{
    if (audio_capture_ctrl.lock == RT_NULL)
    {
        LOG_E("Audio capture not initialized");
        return -RT_ERROR;
    }
    
    if (audio_capture_ctrl.state == AUDIO_CAPTURE_RECORDING)
    {
        LOG_W("Audio capture already running");
        return RT_EOK;
    }
    
    audio_capture_ctrl.buffer_pos = 0;
    audio_capture_ctrl.state = AUDIO_CAPTURE_RECORDING;
    
    /* 创建采集线程 */
    audio_capture_ctrl.capture_thread = rt_thread_create("audio_cap",
                                                          audio_capture_thread_entry,
                                                          RT_NULL,
                                                          2048,
                                                          15,
                                                          10);
    if (audio_capture_ctrl.capture_thread == RT_NULL)
    {
        LOG_E("Failed to create audio capture thread");
        audio_capture_ctrl.state = AUDIO_CAPTURE_IDLE;
        return -RT_ERROR;
    }
    
    rt_thread_startup(audio_capture_ctrl.capture_thread);
    
    LOG_I("Audio capture started");
    
    return RT_EOK;
}

/* 停止音频采集 */
int audio_capture_stop(void)
{
    if (audio_capture_ctrl.state != AUDIO_CAPTURE_RECORDING)
    {
        return RT_EOK;
    }
    
    audio_capture_ctrl.state = AUDIO_CAPTURE_STOPPED;
    
    /* 等待线程结束 */
    if (audio_capture_ctrl.capture_thread)
    {
        rt_thread_mdelay(100);
        audio_capture_ctrl.capture_thread = RT_NULL;
    }
    
    audio_capture_ctrl.state = AUDIO_CAPTURE_IDLE;
    
    LOG_I("Audio capture stopped");
    
    return RT_EOK;
}

/* 设置音频数据回调 */
int audio_capture_set_callback(audio_capture_callback callback)
{
    audio_capture_ctrl.callback = callback;
    return RT_EOK;
}

/* 获取采集状态 */
audio_capture_state_t audio_capture_get_state(void)
{
    return audio_capture_ctrl.state;
}

/* 读取音频数据 */
int audio_capture_read(uint8_t *buffer, uint32_t size, uint32_t timeout)
{
    int result;
    
    if (buffer == RT_NULL || size == 0)
    {
        return -RT_EINVAL;
    }
    
    /* 等待数据就绪 */
    result = rt_sem_take(audio_capture_ctrl.sem, timeout);
    if (result != RT_EOK)
    {
        return 0;
    }
    
    rt_mutex_take(audio_capture_ctrl.lock, RT_WAITING_FOREVER);
    
    uint32_t copy_size = size < AUDIO_BUFFER_SIZE ? size : AUDIO_BUFFER_SIZE;
    rt_memcpy(buffer, audio_capture_ctrl.buffer, copy_size);
    
    rt_mutex_release(audio_capture_ctrl.lock);
    
    return copy_size;
}

/* 导出MSH命令 */
#ifdef FINSH_USING_MSH
static int cmd_audio_capture_test(int argc, char **argv)
{
    if (argc < 2)
    {
        rt_kprintf("Usage: audio_test [init|start|stop|status]\n");
        return -1;
    }
    
    if (strcmp(argv[1], "init") == 0)
    {
        return audio_capture_init();
    }
    else if (strcmp(argv[1], "start") == 0)
    {
        return audio_capture_start();
    }
    else if (strcmp(argv[1], "stop") == 0)
    {
        return audio_capture_stop();
    }
    else if (strcmp(argv[1], "status") == 0)
    {
        rt_kprintf("Audio capture state: %d\n", audio_capture_get_state());
        return 0;
    }
    else
    {
        rt_kprintf("Unknown command: %s\n", argv[1]);
        return -1;
    }
}
MSH_CMD_EXPORT_ALIAS(cmd_audio_capture_test, audio_test, Audio capture test commands);
#endif

