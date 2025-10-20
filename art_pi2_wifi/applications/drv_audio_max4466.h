/*
 * MAX4466 麦克风音频采集驱动头文件
 */

#ifndef __DRV_AUDIO_MAX4466_H__
#define __DRV_AUDIO_MAX4466_H__

#include <rtthread.h>

/* 音频采集参数 */
#define AUDIO_SAMPLE_RATE    16000  /* 16kHz采样率 */
#define AUDIO_BITS           12     /* ADC 12bit */
#define AUDIO_CHANNELS       1      /* 单声道 */

/* 音频采集回调函数类型 */
typedef void (*audio_capture_callback)(uint16_t *data, uint32_t size, void *user_data);

/* 初始化音频采集 */
rt_err_t audio_capture_init(void);

/* 开始录音 */
rt_err_t audio_capture_start(audio_capture_callback callback, void *user_data);

/* 停止录音 */
rt_err_t audio_capture_stop(void);

/* 获取录音状态 */
rt_bool_t audio_capture_is_recording(void);

/* 音频数据处理 */
void audio_process_samples(uint16_t *adc_data, int16_t *pcm_data, uint32_t count);

#endif /* __DRV_AUDIO_MAX4466_H__ */

