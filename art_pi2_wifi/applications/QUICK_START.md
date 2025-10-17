# 语音助手快速入门指南

## 5分钟快速开始

### 第一步：配置AI服务

编辑 `voice_assistant_config.h` 文件：

```c
// 使用百度AI示例
#define AI_SERVICE_PROVIDER     0

#define BAIDU_API_KEY           "替换为你的API Key"
#define BAIDU_SECRET_KEY        "替换为你的Secret Key"
#define BAIDU_APP_ID            "替换为你的App ID"
```

**获取百度AI密钥：**
1. 访问 https://ai.baidu.com/
2. 登录并进入控制台
3. 创建应用（语音技术 -> 语音识别/合成）
4. 复制 API Key、Secret Key 和 App ID

### 第二步：连接WiFi

系统启动后，在MSH终端执行：

```bash
# 扫描WiFi网络
wifi scan

# 连接WiFi（替换为你的WiFi名称和密码）
wifi join YourWiFiName YourPassword

# 检查网络连接
ifconfig
ping baidu.com
```

### 第三步：使用语音助手

```bash
# 触发语音识别（开始录音5秒）
va_trigger

# 说话...（系统会自动录音、识别、处理、播放回复）

# 查看状态
va_status
```

## 完整使用流程

```
1. 开发板上电启动
   ↓
2. 系统初始化（自动）
   ↓
3. 连接WiFi网络（手动/自动）
   ↓
4. 语音助手启动（自动）
   ↓
5. 执行 va_trigger 命令
   ↓
6. 对着麦克风说话（5秒）
   ↓
7. 系统处理并播放回复
   ↓
8. 重复步骤5-7
```

## 主要命令速查

| 命令 | 功能 |
|------|------|
| `va_trigger` | 开始语音识别 |
| `va_status` | 查看状态 |
| `va_stop` | 停止助手 |
| `va_start` | 启动助手 |
| `wifi join SSID PASS` | 连接WiFi |
| `ifconfig` | 查看网络 |

## 状态指示

- **IDLE** - 空闲，等待触发
- **LISTENING** - 正在录音
- **PROCESSING** - 正在处理（网络请求）
- **SPEAKING** - 正在播放回复
- **ERROR** - 错误状态

## 示例对话

```
msh> va_trigger
[I/voice.assistant] Voice assistant triggered, start listening...
[I/audio.capture] Audio capture started
[I/voice.assistant] Recording for 5 seconds...

（对着麦克风说："今天天气怎么样"）

[I/voice.assistant] Recording completed, captured 160000 bytes
[I/voice.assistant] Processing audio data...
[I/ai.cloud] Starting speech to text
[I/ai.cloud] Recognized text: 今天天气怎么样
[Voice] You said: 今天天气怎么样
[I/voice.assistant] Playing AI response...
[I/audio.player] Audio playback started
[I/voice.assistant] Voice assistant interaction completed
```

## 调试技巧

### 1. 启用详细日志
在 `voice_assistant_config.h` 中：
```c
#define VOICE_ASSISTANT_LOG_LEVEL   3  // DEBUG级别
```

### 2. 保存录音文件
```c
#define VOICE_SAVE_AUDIO_FILE       1
```
录音会保存到 `/sdcard/voice_record.pcm`

### 3. 测试单个模块
```bash
# 测试音频采集
audio_test init
audio_test start

# 测试音频播放
audio_play init
audio_play start

# 测试AI服务
ai_test init 0 "api_key" "app_id" "url"
ai_test tts "测试文本"
```

## 常见问题速查

**没反应？**
- 检查是否已连接WiFi
- 检查API密钥是否配置正确
- 使用 `va_status` 查看状态

**识别不准确？**
- 靠近麦克风清晰说话
- 减少环境噪音
- 检查麦克风硬件连接

**网络错误？**
- 执行 `ping baidu.com` 测试网络
- 检查WiFi信号强度
- 重新连接WiFi

**没有声音输出？**
- 检查扬声器硬件连接
- 使用 `audio_play start` 测试播放
- 检查DAC设备初始化

## 下一步

- 阅读 `VOICE_ASSISTANT_README.md` 了解详细功能
- 修改配置文件自定义行为
- 添加自己的语音命令处理逻辑
- 集成更多AI服务功能

---

遇到问题？查看详细文档或在RT-Thread论坛求助！

