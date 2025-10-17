# 桌面语音助手使用说明

## 项目简介

本项目在ART-Pi2开发板基础上，实现了一个完整的AI语音助手功能。支持本地语音采集、云端AI识别处理、语音合成播放等功能。

## 功能特性

- ✅ 音频采集（支持ADC/I2S麦克风）
- ✅ 音频播放（支持DAC/I2S扬声器）
- ✅ AI云服务对接（支持百度AI、讯飞、阿里云）
- ✅ 语音识别（Speech to Text）
- ✅ 语音合成（Text to Speech）
- ✅ 全双工语音交互
- ✅ HTTP/HTTPS网络通信
- ✅ Base64编码/解码
- ✅ JSON数据处理

## 系统架构

```
┌─────────────────────────────────────────────┐
│           用户语音输入                        │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼──────────────────────────────┐
│       音频采集模块 (audio_capture)            │
│   - ADC/I2S采集                              │
│   - 16kHz, 16bit, 单声道                     │
│   - 缓冲区管理                               │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼──────────────────────────────┐
│       AI云服务模块 (ai_cloud_service)         │
│   - 语音识别                                 │
│   - AI对话处理                               │
│   - 语音合成                                 │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼──────────────────────────────┐
│       音频播放模块 (audio_player)             │
│   - DAC/I2S播放                              │
│   - 音频缓冲                                 │
└──────────────┬──────────────────────────────┘
               │
┌──────────────▼──────────────────────────────┐
│           扬声器输出                          │
└─────────────────────────────────────────────┘
```

## 文件说明

### 核心模块

- `voice_assistant.c/h` - 语音助手主控制模块
- `audio_capture.c/h` - 音频采集模块
- `audio_player.c/h` - 音频播放模块
- `ai_cloud_service.c/h` - AI云服务接口模块
- `web_client.c/h` - HTTP客户端工具
- `voice_assistant_config.h` - 配置文件

### 配置说明

在 `voice_assistant_config.h` 中配置以下参数：

```c
// 选择AI服务提供商
#define AI_SERVICE_PROVIDER     0  // 0-百度, 1-讯飞, 2-阿里云, 3-自定义

// 配置API密钥
#define BAIDU_API_KEY           "your_api_key"
#define BAIDU_SECRET_KEY        "your_secret_key"
#define BAIDU_APP_ID            "your_app_id"

// WiFi配置（可选）
#define WIFI_SSID               "your_wifi_ssid"
#define WIFI_PASSWORD           "your_wifi_password"
```

## 编译和运行

### 1. 环境准备

- RT-Thread Studio 或 ENV工具
- ARM GCC工具链
- ART-Pi2开发板

### 2. 更新软件包

```bash
# 在项目根目录执行
pkgs --update
```

### 3. 编译项目

```bash
# 使用scons编译
scons -j8

# 或生成IDE工程
scons --target=mdk5
```

### 4. 下载固件

使用ST-Link将编译生成的固件下载到ART-Pi2开发板。

### 5. 运行

系统启动后，会自动初始化并启动语音助手。在MSH终端可以看到：

```
===========================================
   Welcome to Desktop Voice Assistant!
===========================================
Commands:
  va_trigger - Trigger voice recognition
  va_status  - Show status
  va_stop    - Stop assistant
===========================================
```

## 使用方法

### 基本命令

1. **触发语音识别**
   ```
   msh> va_trigger
   ```
   执行后，系统会开始录音5秒，然后将音频发送到AI云服务进行识别和处理。

2. **查看状态**
   ```
   msh> va_status
   ```

3. **停止语音助手**
   ```
   msh> va_stop
   ```

4. **重新启动**
   ```
   msh> va_start
   ```

### 测试命令

#### 音频采集测试
```
msh> audio_test init      # 初始化音频采集
msh> audio_test start     # 开始采集
msh> audio_test status    # 查看状态
msh> audio_test stop      # 停止采集
```

#### 音频播放测试
```
msh> audio_play init      # 初始化音频播放
msh> audio_play start     # 播放测试音频
msh> audio_play status    # 查看状态
msh> audio_play stop      # 停止播放
```

#### AI服务测试
```
msh> ai_test init 0 "api_key" "app_id" "api_url"  # 初始化AI服务
msh> ai_test tts "你好世界"                        # 测试语音合成
```

## AI服务配置指南

### 百度AI配置

1. 访问 [百度AI开放平台](https://ai.baidu.com/)
2. 创建应用，获取 API Key、Secret Key 和 App ID
3. 在 `voice_assistant_config.h` 中填入：
   ```c
   #define BAIDU_API_KEY    "your_api_key"
   #define BAIDU_SECRET_KEY "your_secret_key"
   #define BAIDU_APP_ID     "your_app_id"
   ```

### 讯飞配置

1. 访问 [讯飞开放平台](https://www.xfyun.cn/)
2. 创建应用，获取 APPID、API Key 和 API Secret
3. 在配置文件中设置：
   ```c
   #define AI_SERVICE_PROVIDER  1
   #define XFYUN_APP_ID         "your_app_id"
   #define XFYUN_API_KEY        "your_api_key"
   #define XFYUN_API_SECRET     "your_api_secret"
   ```

### 阿里云配置

1. 访问 [阿里云](https://www.aliyun.com/)
2. 开通智能语音服务
3. 配置相应的API密钥

## 硬件连接

### 麦克风（音频输入）

推荐使用I2S数字麦克风或模拟麦克风+ADC：

| 信号 | ART-Pi2引脚 | 说明 |
|------|-------------|------|
| ADC_IN | PA0 | 模拟麦克风输入 |
| I2S_SD | PB15 | I2S数据输入 |
| I2S_WS | PB12 | I2S字选择 |
| I2S_CK | PB13 | I2S时钟 |

### 扬声器（音频输出）

推荐使用I2S音频DAC或PWM音频输出：

| 信号 | ART-Pi2引脚 | 说明 |
|------|-------------|------|
| DAC_OUT | PA4 | 模拟音频输出 |
| I2S_SD | PB15 | I2S数据输出 |
| I2S_WS | PB12 | I2S字选择 |
| I2S_CK | PB13 | I2S时钟 |

## 常见问题

### Q1: 编译时找不到头文件？
A: 确保所有新创建的文件都在 `applications` 目录下，SConscript会自动包含。

### Q2: 网络连接失败？
A: 
- 检查WiFi是否已连接：`wifi scan` 和 `wifi join SSID PASSWORD`
- 检查网络配置：`ifconfig`
- 测试网络连通性：`ping baidu.com`

### Q3: AI服务返回错误？
A:
- 检查API密钥配置是否正确
- 检查网络连接是否正常
- 查看日志输出的详细错误信息
- 确认API服务余额充足

### Q4: 没有声音输出？
A:
- 检查DAC设备是否正确初始化
- 检查硬件连接是否正确
- 使用 `audio_play test` 测试播放功能
- 检查音量设置

### Q5: 录音效果不好？
A:
- 调整麦克风增益
- 检查采样率配置（默认16kHz）
- 确保环境噪音较小
- 使用质量更好的麦克风模块

## 性能优化建议

1. **内存优化**
   - 调整音频缓冲区大小（VOICE_BUFFER_SIZE）
   - 使用流式传输代替一次性传输大文件

2. **网络优化**
   - 增大TCP缓冲区
   - 使用更快的WiFi连接
   - 考虑使用WebSocket替代HTTP轮询

3. **音频质量优化**
   - 提高采样率（如从16kHz提升到48kHz）
   - 添加音频滤波器
   - 实现回声消除（AEC）

## 扩展功能

### 离线唤醒词检测
集成本地唤醒词检测算法，实现语音唤醒功能。

### 本地命令识别
添加简单的本地命令识别，无需联网即可控制设备。

### 多轮对话
集成对话管理系统，支持上下文相关的多轮对话。

### 情感识别
分析语音情感，提供更智能的交互体验。

## 技术支持

- RT-Thread官方论坛：https://club.rt-thread.org/
- ART-Pi2相关资料：https://art-pi.gitee.io/
- 项目问题反馈：提交Issue到项目仓库

## 许可证

本项目采用 Apache-2.0 许可证。

## 更新日志

### v1.0.0 (2024-10-16)
- ✅ 初始版本发布
- ✅ 实现基础语音采集和播放功能
- ✅ 集成百度AI、讯飞、阿里云接口
- ✅ 支持语音识别和语音合成
- ✅ 完整的MSH命令行接口

## 贡献指南

欢迎提交PR改进本项目！

---

**注意**：本项目需要有效的AI服务API密钥才能正常工作。请在使用前在 `voice_assistant_config.h` 中配置相应的密钥。

