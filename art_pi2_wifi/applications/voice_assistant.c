/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-16     AI Assistant first version - Voice Assistant Implementation
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "voice_assistant.h"
#include "voice_assistant_config.h"
#include "audio_capture.h"
#include "audio_player.h"
#include "ai_cloud_service.h"
#include "wakeup_detector.h"

#define DBG_TAG "voice.assistant"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/* 语音助手控制结构 */
static struct {
    voice_assistant_state_t state;
    rt_thread_t main_thread;
    rt_sem_t trigger_sem;
    rt_bool_t running;
    rt_bool_t initialized;
} voice_assistant_ctrl = {
    .state = VOICE_ASSISTANT_IDLE,
    .main_thread = RT_NULL,
    .trigger_sem = RT_NULL,
    .running = RT_FALSE,
    .initialized = RT_FALSE
};

/* 前置声明 */
static void wakeup_callback_handler(void);

/* 语音助手主线程 */
static void voice_assistant_thread_entry(void *parameter)
{
    uint8_t *audio_buffer = RT_NULL;
    ai_response_t ai_response;
    int ret;
    
    LOG_I("Voice assistant thread started");
    
    /* 分配音频缓冲区 */
    audio_buffer = (uint8_t *)rt_malloc(VOICE_BUFFER_SIZE);
    if (audio_buffer == RT_NULL)
    {
        LOG_E("Failed to allocate audio buffer");
        voice_assistant_ctrl.state = VOICE_ASSISTANT_ERROR;
        return;
    }
    
    while (voice_assistant_ctrl.running)
    {
        /* 等待触发信号 */
        voice_assistant_ctrl.state = VOICE_ASSISTANT_IDLE;
        LOG_I("Waiting for trigger...");
        
        if (rt_sem_take(voice_assistant_ctrl.trigger_sem, RT_WAITING_FOREVER) != RT_EOK)
        {
            continue;
        }
        
        if (!voice_assistant_ctrl.running)
        {
            break;
        }
        
        LOG_I("Voice assistant triggered, start listening...");
        voice_assistant_ctrl.state = VOICE_ASSISTANT_LISTENING;
        
        /* 开始录音 */
        ret = audio_capture_start();
        if (ret != RT_EOK)
        {
            LOG_E("Failed to start audio capture");
            voice_assistant_ctrl.state = VOICE_ASSISTANT_ERROR;
            continue;
        }
        
        /* 读取音频数据 */
        rt_memset(audio_buffer, 0, VOICE_BUFFER_SIZE);
        uint32_t total_read = 0;
        uint32_t timeout_count = 0;
        
        LOG_I("Recording for %d seconds...", VOICE_RECORD_DURATION);
        
        while (total_read < VOICE_BUFFER_SIZE && timeout_count < 100)
        {
            int read_size = audio_capture_read(audio_buffer + total_read,
                                                VOICE_BUFFER_SIZE - total_read,
                                                1000);
            if (read_size > 0)
            {
                total_read += read_size;
                LOG_D("Read %d bytes, total: %d", read_size, total_read);
            }
            else
            {
                timeout_count++;
            }
            
            /* 检查是否达到录音时长 */
            if (total_read >= VOICE_SAMPLE_RATE * VOICE_CHANNELS * 
                               (VOICE_BITS_PER_SAMPLE / 8) * VOICE_RECORD_DURATION)
            {
                break;
            }
        }
        
        /* 停止录音 */
        audio_capture_stop();
        
        LOG_I("Recording completed, captured %d bytes", total_read);
        
#if VOICE_SAVE_AUDIO_FILE
        /* 保存音频文件到SD卡（调试用）*/
        int fd = open(VOICE_AUDIO_FILE_PATH, O_WRONLY | O_CREAT | O_TRUNC);
        if (fd >= 0)
        {
            write(fd, audio_buffer, total_read);
            close(fd);
            LOG_I("Audio saved to %s", VOICE_AUDIO_FILE_PATH);
        }
#endif
        
        if (total_read < 1000)
        {
            LOG_W("Audio data too short, skipping...");
            continue;
        }
        
        /* 处理语音数据 */
        voice_assistant_ctrl.state = VOICE_ASSISTANT_PROCESSING;
        LOG_I("Processing audio data...");
        
        rt_memset(&ai_response, 0, sizeof(ai_response_t));
        
#if VOICE_FULL_DUPLEX_ENABLE
        /* 使用全双工模式（语音识别+AI回复+语音合成）*/
        ret = ai_cloud_service_full_duplex(audio_buffer, total_read, &ai_response);
#elif VOICE_STT_ENABLE
        /* 只使用语音识别 */
        ret = ai_cloud_service_speech_to_text(audio_buffer, total_read, &ai_response);
#else
        LOG_W("No AI service enabled");
        ret = -RT_ERROR;
#endif
        
        if (ret != RT_EOK || ai_response.error_code != 0)
        {
            LOG_E("AI service failed: %s", 
                  ai_response.error_msg ? ai_response.error_msg : "Unknown error");
            voice_assistant_ctrl.state = VOICE_ASSISTANT_ERROR;
            ai_cloud_service_free_response(&ai_response);
            
            rt_thread_mdelay(1000);
            continue;
        }
        
        /* 显示识别结果 */
        if (ai_response.text_result)
        {
            LOG_I("Recognized: %s", ai_response.text_result);
            rt_kprintf("\n[Voice] You said: %s\n", ai_response.text_result);
        }
        
        /* 播放AI回复 */
        if (ai_response.audio_result && ai_response.audio_len > 0)
        {
            voice_assistant_ctrl.state = VOICE_ASSISTANT_SPEAKING;
            LOG_I("Playing AI response (%d bytes)...", ai_response.audio_len);
            
            ret = audio_player_play((uint8_t *)ai_response.audio_result, 
                                    ai_response.audio_len);
            if (ret != RT_EOK)
            {
                LOG_E("Failed to play audio");
            }
            else
            {
                /* 等待播放完成 */
                while (audio_player_get_state() == AUDIO_PLAYER_PLAYING)
                {
                    rt_thread_mdelay(100);
                }
            }
        }
        
        ai_cloud_service_free_response(&ai_response);
        
        LOG_I("Voice assistant interaction completed");
    }
    
    rt_free(audio_buffer);
    voice_assistant_ctrl.state = VOICE_ASSISTANT_IDLE;
    
    LOG_I("Voice assistant thread stopped");
}

/* 初始化语音助手 */
int voice_assistant_init(void)
{
    ai_service_config_t ai_config;
    int ret;
    
    if (voice_assistant_ctrl.initialized)
    {
        LOG_W("Voice assistant already initialized");
        return RT_EOK;
    }
    
    LOG_I("Initializing voice assistant...");
    
    /* 初始化音频采集 */
    ret = audio_capture_init();
    if (ret != RT_EOK)
    {
        LOG_E("Failed to initialize audio capture");
        return ret;
    }
    
    /* 初始化音频播放 */
    ret = audio_player_init();
    if (ret != RT_EOK)
    {
        LOG_E("Failed to initialize audio player");
        return ret;
    }
    
    /* 配置AI服务 */
    rt_memset(&ai_config, 0, sizeof(ai_config));
    ai_config.provider = AI_SERVICE_PROVIDER;
    
#if AI_SERVICE_PROVIDER == 0  /* 百度AI */
    strncpy(ai_config.api_key, BAIDU_API_KEY, sizeof(ai_config.api_key) - 1);
    strncpy(ai_config.api_secret, BAIDU_SECRET_KEY, sizeof(ai_config.api_secret) - 1);
    strncpy(ai_config.app_id, BAIDU_APP_ID, sizeof(ai_config.app_id) - 1);
    strncpy(ai_config.api_url, BAIDU_STT_URL, sizeof(ai_config.api_url) - 1);
#elif AI_SERVICE_PROVIDER == 1  /* 讯飞 */
    strncpy(ai_config.api_key, XFYUN_API_KEY, sizeof(ai_config.api_key) - 1);
    strncpy(ai_config.api_secret, XFYUN_API_SECRET, sizeof(ai_config.api_secret) - 1);
    strncpy(ai_config.app_id, XFYUN_APP_ID, sizeof(ai_config.app_id) - 1);
    strncpy(ai_config.api_url, XFYUN_STT_URL, sizeof(ai_config.api_url) - 1);
#elif AI_SERVICE_PROVIDER == 2  /* 阿里云 */
    strncpy(ai_config.api_key, ALIYUN_API_KEY, sizeof(ai_config.api_key) - 1);
    strncpy(ai_config.api_secret, ALIYUN_API_SECRET, sizeof(ai_config.api_secret) - 1);
    strncpy(ai_config.app_id, ALIYUN_APP_ID, sizeof(ai_config.app_id) - 1);
    strncpy(ai_config.api_url, ALIYUN_STT_URL, sizeof(ai_config.api_url) - 1);
#else  /* 自定义 */
    strncpy(ai_config.api_key, CUSTOM_API_KEY, sizeof(ai_config.api_key) - 1);
    strncpy(ai_config.api_secret, CUSTOM_API_SECRET, sizeof(ai_config.api_secret) - 1);
    strncpy(ai_config.app_id, CUSTOM_APP_ID, sizeof(ai_config.app_id) - 1);
    strncpy(ai_config.api_url, CUSTOM_API_URL, sizeof(ai_config.api_url) - 1);
#endif
    
    /* 初始化AI云服务 */
    ret = ai_cloud_service_init(&ai_config);
    if (ret != RT_EOK)
    {
        LOG_E("Failed to initialize AI cloud service");
        return ret;
    }
    
    /* 创建触发信号量 */
    voice_assistant_ctrl.trigger_sem = rt_sem_create("va_trigger", 0, RT_IPC_FLAG_FIFO);
    if (voice_assistant_ctrl.trigger_sem == RT_NULL)
    {
        LOG_E("Failed to create trigger semaphore");
        return -RT_ERROR;
    }
    
    voice_assistant_ctrl.initialized = RT_TRUE;
    voice_assistant_ctrl.state = VOICE_ASSISTANT_IDLE;
    
    LOG_I("Voice assistant initialized successfully");
    
    return RT_EOK;
}

/* 启动语音助手 */
int voice_assistant_start(void)
{
    if (!voice_assistant_ctrl.initialized)
    {
        LOG_E("Voice assistant not initialized");
        return -RT_ERROR;
    }
    
    if (voice_assistant_ctrl.running)
    {
        LOG_W("Voice assistant already running");
        return RT_EOK;
    }
    
    voice_assistant_ctrl.running = RT_TRUE;
    
    /* 创建语音助手主线程 */
    voice_assistant_ctrl.main_thread = rt_thread_create("voice_asst",
                                                         voice_assistant_thread_entry,
                                                         RT_NULL,
                                                         4096,
                                                         10,
                                                         10);
    if (voice_assistant_ctrl.main_thread == RT_NULL)
    {
        LOG_E("Failed to create voice assistant thread");
        voice_assistant_ctrl.running = RT_FALSE;
        return -RT_ERROR;
    }
    
    rt_thread_startup(voice_assistant_ctrl.main_thread);
    
    LOG_I("Voice assistant started");
    
#if VOICE_WAKEUP_ENABLE
    /* 启动唤醒词检测 */
    wakeup_detector_init(VOICE_WAKEUP_WORD);
    wakeup_detector_set_callback(wakeup_callback_handler);
    wakeup_detector_start();
    
    rt_kprintf("\n======================================\n");
    rt_kprintf("  Voice Assistant Started!\n");
    rt_kprintf("  Wakeup word: %s\n", VOICE_WAKEUP_WORD);
    rt_kprintf("  Say the wakeup word to activate!\n");
    rt_kprintf("======================================\n\n");
#else
    rt_kprintf("\n======================================\n");
    rt_kprintf("  Voice Assistant Started!\n");
    rt_kprintf("  Use 'va_trigger' command to start\n");
    rt_kprintf("======================================\n\n");
#endif
    
    return RT_EOK;
}

/* 停止语音助手 */
int voice_assistant_stop(void)
{
    if (!voice_assistant_ctrl.running)
    {
        return RT_EOK;
    }
    
#if VOICE_WAKEUP_ENABLE
    /* 停止唤醒词检测 */
    wakeup_detector_stop();
#endif
    
    voice_assistant_ctrl.running = RT_FALSE;
    
    /* 释放信号量以退出线程 */
    rt_sem_release(voice_assistant_ctrl.trigger_sem);
    
    /* 等待线程结束 */
    rt_thread_mdelay(500);
    
    LOG_I("Voice assistant stopped");
    
    return RT_EOK;
}

/* 获取语音助手状态 */
voice_assistant_state_t voice_assistant_get_state(void)
{
    return voice_assistant_ctrl.state;
}

/* 手动触发语音识别 */
int voice_assistant_trigger(void)
{
    if (!voice_assistant_ctrl.initialized || !voice_assistant_ctrl.running)
    {
        LOG_E("Voice assistant not ready");
        return -RT_ERROR;
    }
    
    if (voice_assistant_ctrl.state != VOICE_ASSISTANT_IDLE)
    {
        LOG_W("Voice assistant is busy (state: %d)", voice_assistant_ctrl.state);
        return -RT_ERROR;
    }
    
    LOG_I("Voice assistant triggered");
    rt_sem_release(voice_assistant_ctrl.trigger_sem);
    
    return RT_EOK;
}

/* 唤醒词检测回调实现 */
static void wakeup_callback_handler(void)
{
    LOG_I("Wakeup callback triggered");
    voice_assistant_trigger();
}

/* 导出MSH命令 */
#ifdef FINSH_USING_MSH

static int cmd_va_init(int argc, char **argv)
{
    return voice_assistant_init();
}
MSH_CMD_EXPORT_ALIAS(cmd_va_init, va_init, Initialize voice assistant);

static int cmd_va_start(int argc, char **argv)
{
    return voice_assistant_start();
}
MSH_CMD_EXPORT_ALIAS(cmd_va_start, va_start, Start voice assistant);

static int cmd_va_stop(int argc, char **argv)
{
    return voice_assistant_stop();
}
MSH_CMD_EXPORT_ALIAS(cmd_va_stop, va_stop, Stop voice assistant);

static int cmd_va_trigger(int argc, char **argv)
{
    return voice_assistant_trigger();
}
MSH_CMD_EXPORT_ALIAS(cmd_va_trigger, va_trigger, Trigger voice recognition);

static int cmd_va_status(int argc, char **argv)
{
    const char *state_str[] = {
        "IDLE",
        "LISTENING",
        "PROCESSING",
        "SPEAKING",
        "ERROR"
    };
    
    rt_kprintf("Voice Assistant Status:\n");
    rt_kprintf("  Initialized: %s\n", voice_assistant_ctrl.initialized ? "Yes" : "No");
    rt_kprintf("  Running: %s\n", voice_assistant_ctrl.running ? "Yes" : "No");
    rt_kprintf("  State: %s\n", state_str[voice_assistant_ctrl.state]);
    
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_va_status, va_status, Show voice assistant status);

#endif

