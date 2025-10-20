/*
 * MAX4466 麦克风测试程序
 */

#include <rtthread.h>
#include "drv_audio_max4466.h"
#include "audio_player.h"

#define DBG_TAG "audio.test"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

/* 录音测试：录制并立即播放（回音测试）*/
static rt_bool_t echo_test_running = RT_FALSE;

/* 回音测试回调 */
static void echo_test_callback(uint16_t *adc_data, uint32_t size, void *user_data)
{
    int16_t pcm_buffer[1024];
    
    /* 转换ADC数据为PCM */
    audio_process_samples(adc_data, pcm_buffer, size);
    
    /* 立即播放（回音效果）*/
    audio_player_play((uint8_t *)pcm_buffer, size * 2);
}

/* 回音测试命令 */
static int cmd_mic_echo_test(int argc, char **argv)
{
    if (echo_test_running)
    {
        /* 停止测试 */
        audio_capture_stop();
        echo_test_running = RT_FALSE;
        rt_kprintf("Echo test stopped\n");
    }
    else
    {
        /* 开始测试 */
        if (audio_capture_init() != RT_EOK)
        {
            rt_kprintf("Failed to init audio capture\n");
            return -1;
        }
        
        if (audio_capture_start(echo_test_callback, RT_NULL) != RT_EOK)
        {
            rt_kprintf("Failed to start recording\n");
            return -1;
        }
        
        echo_test_running = RT_TRUE;
        rt_kprintf("Echo test started (speak into microphone)\n");
        rt_kprintf("Run 'mic_echo_test' again to stop\n");
    }
    
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_mic_echo_test, mic_echo_test, Microphone echo test);

/* 音量测试：显示麦克风音量 */
static rt_bool_t volume_test_running = RT_FALSE;
static uint32_t volume_sum = 0;
static uint32_t volume_count = 0;

static void volume_test_callback(uint16_t *adc_data, uint32_t size, void *user_data)
{
    uint32_t sum = 0;
    
    /* 计算音量（平均值）*/
    for (uint32_t i = 0; i < size; i++)
    {
        int32_t value = (int32_t)adc_data[i] - 2048;
        if (value < 0) value = -value;
        sum += value;
    }
    
    volume_sum += sum;
    volume_count += size;
}

static int cmd_mic_volume(int argc, char **argv)
{
    if (volume_test_running)
    {
        /* 停止并显示结果 */
        audio_capture_stop();
        
        uint32_t avg_volume = volume_sum / volume_count;
        rt_kprintf("Average volume: %d (0-2048)\n", avg_volume);
        rt_kprintf("Volume level: ");
        
        if (avg_volume < 50)
            rt_kprintf("Too quiet\n");
        else if (avg_volume < 200)
            rt_kprintf("Normal\n");
        else if (avg_volume < 500)
            rt_kprintf("Loud\n");
        else
            rt_kprintf("Very loud!\n");
        
        volume_test_running = RT_FALSE;
        volume_sum = 0;
        volume_count = 0;
    }
    else
    {
        /* 开始测试 */
        audio_capture_init();
        
        if (audio_capture_start(volume_test_callback, RT_NULL) != RT_EOK)
        {
            rt_kprintf("Failed to start recording\n");
            return -1;
        }
        
        volume_test_running = RT_TRUE;
        rt_kprintf("Recording volume for 3 seconds...\n");
        rt_kprintf("Speak into the microphone!\n");
        
        /* 3秒后自动停止 */
        rt_thread_mdelay(3000);
        cmd_mic_volume(0, NULL);
    }
    
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_mic_volume, mic_volume, Test microphone volume);

