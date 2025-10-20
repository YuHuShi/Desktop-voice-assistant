/*
 * MAX98357A 音频测试
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DBG_TAG "audio.test"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#define SAMPLE_RATE 16000
#define TONE_FREQ   440    /* A4音符 */
#define DURATION_MS 1000   /* 1秒 */

/* 生成正弦波测试音 */
static void generate_sine_wave(int16_t *buffer, uint32_t samples, uint16_t freq)
{
    for (uint32_t i = 0; i < samples; i++)
    {
        float t = (float)i / SAMPLE_RATE;
        float value = sin(2.0f * M_PI * freq * t);
        buffer[i] = (int16_t)(value * 16000);  /* 幅度约50% */
    }
}

/* 测试命令 */
static int cmd_audio_test(int argc, char **argv)
{
    rt_device_t audio_dev;
    int16_t *buffer;
    uint32_t samples = SAMPLE_RATE * DURATION_MS / 1000;
    uint32_t buffer_size = samples * sizeof(int16_t);
    
    LOG_I("MAX98357A Audio Test");
    LOG_I("Generating %dHz sine wave, %dms", TONE_FREQ, DURATION_MS);
    
    /* 查找音频设备 */
    audio_dev = rt_device_find("audio0");
    if (audio_dev == RT_NULL)
    {
        LOG_E("Audio device not found");
        LOG_I("Please check:");
        LOG_I("1. MAX98357A is connected correctly");
        LOG_I("2. I2S driver is compiled");
        LOG_I("3. Run 'list_device' to check devices");
        return -1;
    }
    
    /* 打开设备 */
    if (rt_device_open(audio_dev, RT_DEVICE_OFLAG_WRONLY) != RT_EOK)
    {
        LOG_E("Failed to open audio device");
        return -1;
    }
    
    /* 分配缓冲区 */
    buffer = (int16_t *)rt_malloc(buffer_size);
    if (buffer == RT_NULL)
    {
        LOG_E("Failed to allocate buffer");
        rt_device_close(audio_dev);
        return -1;
    }
    
    /* 生成测试音 */
    generate_sine_wave(buffer, samples, TONE_FREQ);
    
    LOG_I("Playing audio...");
    
    /* 播放音频 */
    rt_size_t written = rt_device_write(audio_dev, 0, buffer, buffer_size);
    
    if (written == buffer_size)
    {
        LOG_I("Audio test completed successfully!");
        LOG_I("You should hear a %dHz tone from the speaker", TONE_FREQ);
    }
    else
    {
        LOG_E("Audio write failed: %d/%d bytes", written, buffer_size);
    }
    
    /* 清理 */
    rt_free(buffer);
    rt_device_close(audio_dev);
    
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_audio_test, audio_test_i2s, Test MAX98357A I2S audio);

/* 简单的嘀嘀声测试 */
static int cmd_audio_beep(int argc, char **argv)
{
    rt_device_t audio_dev;
    int16_t buffer[160];  /* 10ms @ 16kHz */
    uint16_t freq = 1000; /* 1kHz */
    
    if (argc > 1)
    {
        freq = atoi(argv[1]);
    }
    
    audio_dev = rt_device_find("audio0");
    if (audio_dev == RT_NULL)
    {
        rt_kprintf("Audio device not found\n");
        return -1;
    }
    
    rt_device_open(audio_dev, RT_DEVICE_OFLAG_WRONLY);
    
    /* 生成短促的嘀声 */
    generate_sine_wave(buffer, 160, freq);
    
    rt_device_write(audio_dev, 0, buffer, sizeof(buffer));
    
    rt_device_close(audio_dev);
    
    rt_kprintf("Beep %dHz\n", freq);
    
    return 0;
}
MSH_CMD_EXPORT_ALIAS(cmd_audio_beep, audio_beep, Beep test (audio_beep [freq]));

