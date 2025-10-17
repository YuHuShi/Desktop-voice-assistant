/*
 * Copyright (c) 2006-2024, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-10-16     AI Assistant first version - Voice Assistant Configuration
 */

#ifndef __VOICE_ASSISTANT_CONFIG_H__
#define __VOICE_ASSISTANT_CONFIG_H__

/* ==================== AI服务配置 ==================== */

/* 选择AI服务提供商 
 * 0 - 百度AI (https://ai.baidu.com/)
 * 1 - 讯飞 (https://www.xfyun.cn/)
 * 2 - 阿里云 (https://www.aliyun.com/)
 * 3 - 自定义
 */
#define AI_SERVICE_PROVIDER     0  /* 默认使用百度AI */

/* 百度AI配置 */
#define BAIDU_API_KEY           "your_baidu_api_key_here"
#define BAIDU_SECRET_KEY        "your_baidu_secret_key_here"
#define BAIDU_APP_ID            "your_baidu_app_id_here"
#define BAIDU_STT_URL           "http://vop.baidu.com/server_api"
#define BAIDU_TTS_URL           "http://tsn.baidu.com/text2audio"

/* 讯飞配置 */
#define XFYUN_API_KEY           "your_xfyun_api_key_here"
#define XFYUN_API_SECRET        "your_xfyun_api_secret_here"
#define XFYUN_APP_ID            "your_xfyun_app_id_here"
#define XFYUN_STT_URL           "http://api.xfyun.cn/v1/service/v1/iat"
#define XFYUN_TTS_URL           "http://api.xfyun.cn/v1/service/v1/tts"

/* 阿里云配置 */
#define ALIYUN_API_KEY          "your_aliyun_api_key_here"
#define ALIYUN_API_SECRET       "your_aliyun_api_secret_here"
#define ALIYUN_APP_ID           "your_aliyun_app_id_here"
#define ALIYUN_STT_URL          "http://nls-gateway.cn-shanghai.aliyuncs.com/stream/v1/asr"
#define ALIYUN_TTS_URL          "http://nls-gateway.cn-shanghai.aliyuncs.com/stream/v1/tts"

/* 自定义API配置 */
#define CUSTOM_API_KEY          "your_custom_api_key_here"
#define CUSTOM_API_SECRET       "your_custom_api_secret_here"
#define CUSTOM_APP_ID           "your_custom_app_id_here"
#define CUSTOM_API_URL          "http://your-custom-api-server.com/api/voice"

/* ==================== 音频配置 ==================== */

/* 音频采样率 (Hz) */
#define VOICE_SAMPLE_RATE       16000

/* 音频通道数 */
#define VOICE_CHANNELS          1

/* 音频位深度 (bits) */
#define VOICE_BITS_PER_SAMPLE   16

/* 录音持续时间 (秒) - 减少以节省内存 */
#define VOICE_RECORD_DURATION   3

/* 音频缓冲区大小 (字节) */
#define VOICE_BUFFER_SIZE       (VOICE_SAMPLE_RATE * VOICE_CHANNELS * (VOICE_BITS_PER_SAMPLE / 8) * VOICE_RECORD_DURATION)

/* ==================== 语音助手配置 ==================== */

/* 唤醒词检测使能 (如果为0，则手动触发) */
#define VOICE_WAKEUP_ENABLE     1

/* 唤醒词 */
#define VOICE_WAKEUP_WORD       "Hi小石"

/* VAD (Voice Activity Detection) 使能 */
#define VOICE_VAD_ENABLE        0

/* 静音检测阈值 (用于自动停止录音) */
#define VOICE_SILENCE_THRESHOLD 100

/* 静音持续时间 (毫秒，超过此时间认为说话结束) */
#define VOICE_SILENCE_DURATION  1000

/* ==================== 网络配置 ==================== */

/* WiFi SSID (如果需要自动连接) */
#define WIFI_SSID               "your_wifi_ssid"

/* WiFi密码 */
#define WIFI_PASSWORD           "your_wifi_password"

/* 网络超时时间 (秒) */
#define NETWORK_TIMEOUT         30

/* ==================== 调试配置 ==================== */

/* 调试日志级别 
 * 0 - ERROR
 * 1 - WARNING
 * 2 - INFO
 * 3 - DEBUG
 */
#define VOICE_ASSISTANT_LOG_LEVEL   2

/* 保存音频文件到SD卡 (用于调试) */
#define VOICE_SAVE_AUDIO_FILE       0

/* 音频文件保存路径 */
#define VOICE_AUDIO_FILE_PATH       "/sdcard/voice_record.pcm"

/* ==================== 功能开关 ==================== */

/* 启用语音识别功能 */
#define VOICE_STT_ENABLE        1

/* 启用语音合成功能 */
#define VOICE_TTS_ENABLE        1

/* 启用全双工交互 */
#define VOICE_FULL_DUPLEX_ENABLE    1

/* 启用本地命令识别（不需要联网）*/
#define VOICE_LOCAL_CMD_ENABLE  0

#endif /* __VOICE_ASSISTANT_CONFIG_H__ */

