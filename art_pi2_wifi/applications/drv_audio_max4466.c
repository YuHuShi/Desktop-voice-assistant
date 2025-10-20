/*
 * MAX4466 麦克风音频采集驱动
 * 使用ADC + DMA进行连续音频采集
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "drv_audio_max4466.h"
#include "stm32h7rsxx_hal.h"

#define DBG_TAG "drv.mic"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/* ADC句柄（使用STM32 HAL）*/
extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef handle_GPDMA1_Channel0;

/* 音频采集缓冲区（双缓冲）*/
#define AUDIO_BUFFER_SIZE  1024  /* 每个缓冲区样本数 */
static uint16_t adc_buffer[AUDIO_BUFFER_SIZE * 2];  /* 双缓冲 */

/* 音频采集控制 */
static struct {
    rt_bool_t is_recording;
    audio_capture_callback callback;
    void *user_data;
} audio_capture_ctrl = {
    .is_recording = RT_FALSE,
    .callback = RT_NULL,
    .user_data = RT_NULL
};

/* ADC转换完成回调（半满）*/
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc == &hadc1)
    {
        /* 处理前半部分缓冲区数据 */
        if (audio_capture_ctrl.callback)
        {
            audio_capture_ctrl.callback(adc_buffer, 
                                       AUDIO_BUFFER_SIZE,
                                       audio_capture_ctrl.user_data);
        }
    }
}

/* ADC转换完成回调（全满）*/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc == &hadc1)
    {
        /* 处理后半部分缓冲区数据 */
        if (audio_capture_ctrl.callback)
        {
            audio_capture_ctrl.callback(&adc_buffer[AUDIO_BUFFER_SIZE], 
                                       AUDIO_BUFFER_SIZE,
                                       audio_capture_ctrl.user_data);
        }
    }
}

/* 初始化音频采集 */
rt_err_t audio_capture_init(void)
{
    LOG_I("MAX4466 audio capture init");
    
    /* 链接DMA到ADC */
    __HAL_LINKDMA(&hadc1, DMA_Handle, handle_GPDMA1_Channel0);
    
    /* 校准ADC（STM32H7RS系列只需要SingleDiff参数）*/
    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        LOG_E("ADC calibration failed");
        return -RT_ERROR;
    }
    
    LOG_I("ADC calibrated successfully");
    return RT_EOK;
}

/* 开始录音 */
rt_err_t audio_capture_start(audio_capture_callback callback, void *user_data)
{
    if (audio_capture_ctrl.is_recording)
    {
        LOG_W("Already recording");
        return -RT_ERROR;
    }
    
    audio_capture_ctrl.callback = callback;
    audio_capture_ctrl.user_data = user_data;
    audio_capture_ctrl.is_recording = RT_TRUE;
    
    /* 启动ADC DMA连续转换 */
    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, 
                          AUDIO_BUFFER_SIZE * 2) != HAL_OK)
    {
        LOG_E("Failed to start ADC DMA");
        audio_capture_ctrl.is_recording = RT_FALSE;
        return -RT_ERROR;
    }
    
    LOG_I("Audio capture started");
    return RT_EOK;
}

/* 停止录音 */
rt_err_t audio_capture_stop(void)
{
    if (!audio_capture_ctrl.is_recording)
    {
        return RT_EOK;
    }
    
    HAL_ADC_Stop_DMA(&hadc1);
    
    audio_capture_ctrl.is_recording = RT_FALSE;
    audio_capture_ctrl.callback = RT_NULL;
    audio_capture_ctrl.user_data = RT_NULL;
    
    LOG_I("Audio capture stopped");
    return RT_EOK;
}

/* 获取录音状态 */
rt_bool_t audio_capture_is_recording(void)
{
    return audio_capture_ctrl.is_recording;
}

/* 音频数据处理：ADC原始值 → PCM 16bit */
void audio_process_samples(uint16_t *adc_data, int16_t *pcm_data, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++)
    {
        /* ADC: 12bit, 0-4095, 中心点约2048 */
        /* 转换为 16bit PCM: -32768 ~ 32767 */
        int32_t value = (int32_t)adc_data[i] - 2048;  /* 去除直流偏置 */
        value = value * 16;  /* 放大到16bit范围 */
        
        /* 限幅 */
        if (value > 32767) value = 32767;
        if (value < -32768) value = -32768;
        
        pcm_data[i] = (int16_t)value;
    }
}

/* 读取音频数据（兼容旧API）*/
int audio_capture_read(uint8_t *buffer, uint32_t size, uint32_t timeout)
{
    if (!audio_capture_ctrl.is_recording)
    {
        return -RT_ERROR;
    }
    
    /* 简化实现：返回当前缓冲区数据 */
    /* 注意：这是一个简化版本，实际应该使用队列或环形缓冲 */
    uint32_t copy_size = size > AUDIO_BUFFER_SIZE * 2 ? AUDIO_BUFFER_SIZE * 2 : size;
    rt_memcpy(buffer, adc_buffer, copy_size);
    
    return copy_size;
}

