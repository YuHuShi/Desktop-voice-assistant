/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * MAX98357A I2S Audio Driver for ART-Pi2
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "drv_audio_max98357a.h"
#include "stm32h7rsxx_hal.h"

/* RT-Thread Audio 设备框架定义 */
#include <rtdevice.h>

/* Audio设备类型 */
#define AUDIO_TYPE_OUTPUT   (1)

/* Audio控制命令 */
#define AUDIO_DSP_PARAM        (1)
#define AUDIO_DSP_SAMPLERATE   (2)

/* Audio配置结构体 */
struct rt_audio_configure
{
    rt_uint32_t samplerate;
    rt_uint16_t channels;
    rt_uint16_t samplebits;
};

struct rt_audio_caps
{
    int main_type;
    int sub_type;
    union
    {
        rt_uint32_t value;
        struct rt_audio_configure config;
    } udata;
};

struct rt_audio_device;

struct rt_audio_ops
{
    rt_err_t (*init)(struct rt_audio_device *audio);
    rt_err_t (*start)(struct rt_audio_device *audio, rt_uint8_t stream);
    rt_err_t (*stop)(struct rt_audio_device *audio, rt_uint8_t stream);
    rt_err_t (*control)(struct rt_audio_device *audio, struct rt_audio_caps *caps);
    rt_size_t (*transmit)(struct rt_audio_device *audio, const void *writeBuf, rt_size_t size);
};

struct rt_audio_device
{
    struct rt_device parent;
    struct rt_audio_ops *ops;
};

/* 简化的audio注册函数 */
rt_err_t rt_audio_register(struct rt_audio_device *audio, const char *name, rt_uint32_t flag, void *data)
{
    rt_device_t device;
    
    device = &(audio->parent);
    device->type = RT_Device_Class_Sound;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;
    
    device->init    = RT_NULL;
    device->open    = RT_NULL;
    device->close   = RT_NULL;
    device->read    = RT_NULL;
    device->write   = RT_NULL;
    device->control = RT_NULL;
    
    device->user_data = data;
    
    return rt_device_register(device, name, flag);
}

#define DBG_TAG "drv.audio"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/* I2S 句柄（使用 STM32 HAL）*/
extern I2S_HandleTypeDef hi2s1;

/* 音频设备控制块 */
static struct rt_audio_device audio_dev = {0};

/* DMA 传输完成标志 */
static rt_sem_t dma_sem = RT_NULL;

/* DMA 传输完成回调 */
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s == &hi2s1)
    {
        rt_sem_release(dma_sem);
    }
}

/* 初始化 I2S 外设 */
static rt_err_t audio_init(struct rt_audio_device *audio)
{
    LOG_I("MAX98357A audio init");
    
    /* 创建 DMA 传输信号量 */
    if (dma_sem == RT_NULL)
    {
        dma_sem = rt_sem_create("audio_dma", 0, RT_IPC_FLAG_FIFO);
        if (dma_sem == RT_NULL)
        {
            LOG_E("Create DMA semaphore failed");
            return -RT_ERROR;
        }
    }
    
    /* 注意：I2S已经在board层初始化（cubemx_init.c），这里不需要再初始化 */
    LOG_I("I2S already initialized in board layer");
    
    LOG_I("I2S initialized: 16kHz, 16bit, mono");
    return RT_EOK;
}

/* 启动音频播放 */
static rt_err_t audio_start(struct rt_audio_device *audio, rt_uint8_t stream)
{
    LOG_D("Audio start");
    return RT_EOK;
}

/* 停止音频播放 */
static rt_err_t audio_stop(struct rt_audio_device *audio, rt_uint8_t stream)
{
    LOG_D("Audio stop");
    HAL_I2S_DMAStop(&hi2s1);
    return RT_EOK;
}

/* 配置音频参数 */
static rt_err_t audio_config(struct rt_audio_device *audio, struct rt_audio_caps *caps)
{
    rt_err_t result = RT_EOK;
    
    switch (caps->main_type)
    {
    case AUDIO_TYPE_OUTPUT:
        switch (caps->sub_type)
        {
        case AUDIO_DSP_PARAM:
            LOG_I("Set samplerate: %d, channels: %d", 
                  caps->udata.config.samplerate,
                  caps->udata.config.channels);
            
            /* 根据采样率重新配置 I2S */
            switch (caps->udata.config.samplerate)
            {
            case 8000:
                hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_8K;
                break;
            case 16000:
                hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_16K;
                break;
            case 22050:
                hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_22K;
                break;
            case 44100:
                hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_44K;
                break;
            case 48000:
                hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_48K;
                break;
            default:
                LOG_W("Unsupported samplerate: %d, use 16kHz", 
                      caps->udata.config.samplerate);
                hi2s1.Init.AudioFreq = I2S_AUDIOFREQ_16K;
                break;
            }
            
            /* 注意：运行时改变采样率需要重新初始化I2S */
            /* 当前版本暂不支持动态改变采样率，使用固定16KHz */
            LOG_W("Dynamic samplerate change not supported, using 16KHz");
            break;
            
        case AUDIO_DSP_SAMPLERATE:
            /* 设置采样率 */
            break;
            
        default:
            result = -RT_ERROR;
            break;
        }
        break;
        
    default:
        result = -RT_ERROR;
        break;
    }
    
    return result;
}

/* 获取音频参数 */
__attribute__((unused)) static rt_err_t audio_getcaps(struct rt_audio_device *audio, struct rt_audio_caps *caps)
{
    rt_err_t result = RT_EOK;
    
    switch (caps->main_type)
    {
    case AUDIO_TYPE_OUTPUT:
        switch (caps->sub_type)
        {
        case AUDIO_DSP_PARAM:
            caps->udata.config.samplerate = 16000;
            caps->udata.config.channels = 1;
            caps->udata.config.samplebits = 16;
            break;
            
        case AUDIO_DSP_SAMPLERATE:
            caps->udata.value = 16000;
            break;
            
        default:
            result = -RT_ERROR;
            break;
        }
        break;
        
    default:
        result = -RT_ERROR;
        break;
    }
    
    return result;
}

/* 发送音频数据（通过 DMA）*/
static rt_size_t audio_transmit(struct rt_audio_device *audio, 
                                const void *writeBuf, 
                                rt_size_t size)
{
    const uint16_t *buf = (const uint16_t *)writeBuf;
    rt_size_t count = size / 2;  /* 16bit 数据 */
    
    LOG_D("Transmit %d bytes (%d samples)", size, count);
    
    /* 通过 DMA 发送数据 */
    if (HAL_I2S_Transmit_DMA(&hi2s1, (uint16_t *)buf, count) != HAL_OK)
    {
        LOG_E("I2S DMA transmit failed");
        return 0;
    }
    
    /* 等待 DMA 传输完成 */
    if (rt_sem_take(dma_sem, rt_tick_from_millisecond(1000)) != RT_EOK)
    {
        LOG_W("DMA transmit timeout");
        HAL_I2S_DMAStop(&hi2s1);
        return 0;
    }
    
    return size;
}

/* Audio 设备操作接口 */
static struct rt_audio_ops audio_ops =
{
    .init = audio_init,
    .start = audio_start,
    .stop = audio_stop,
    .control = audio_config,
    .transmit = audio_transmit,
};

/* 注册 MAX98357A 音频设备 */
int rt_hw_audio_max98357a_init(void)
{
    rt_err_t result;
    
    /* 注册 Audio 设备 */
    audio_dev.ops = &audio_ops;
    
    result = rt_audio_register(&audio_dev, "audio0", 
                               RT_DEVICE_FLAG_WRONLY, RT_NULL);
    
    if (result == RT_EOK)
    {
        LOG_I("MAX98357A audio device registered");
    }
    else
    {
        LOG_E("MAX98357A audio device register failed");
    }
    
    return result;
}
INIT_DEVICE_EXPORT(rt_hw_audio_max98357a_init);

