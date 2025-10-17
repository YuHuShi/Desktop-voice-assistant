# 语音助手软件包组织方案

## 📦 Package结构设计

将语音助手功能分成4个独立的软件包，实现模块化和可复用：

```
packages/
├── audio-codec/              # Package 1: 音频编解码
│   ├── inc/
│   │   ├── audio_capture.h   # 音频采集接口
│   │   └── audio_player.h    # 音频播放接口
│   ├── src/
│   │   ├── audio_capture.c   # 音频采集实现
│   │   └── audio_player.c    # 音频播放实现
│   ├── Kconfig              # 配置选项
│   ├── SConscript           # 构建脚本
│   ├── package.json         # 包信息
│   └── README.md            # 说明文档
│
├── web-client/              # Package 2: HTTP客户端
│   ├── inc/
│   │   └── web_client.h     # HTTP客户端接口
│   ├── src/
│   │   └── web_client.c     # HTTP客户端实现
│   ├── Kconfig
│   ├── SConscript
│   ├── package.json
│   └── README.md
│
├── ai-cloud/                # Package 3: AI云服务
│   ├── inc/
│   │   └── ai_cloud_service.h    # AI服务接口
│   ├── src/
│   │   └── ai_cloud_service.c    # AI服务实现
│   ├── Kconfig
│   ├── SConscript
│   ├── package.json
│   └── README.md
│
└── voice-assistant/         # Package 4: 语音助手核心
    ├── inc/
    │   ├── voice_assistant.h         # 语音助手接口
    │   ├── voice_assistant_config.h  # 配置文件
    │   └── wakeup_detector.h         # 唤醒词检测
    ├── src/
    │   ├── voice_assistant.c         # 语音助手实现
    │   └── wakeup_detector.c         # 唤醒词检测实现
    ├── docs/
    │   ├── QUICK_START.md           # 快速开始
    │   └── WAKEUP_GUIDE.md          # 唤醒词指南
    ├── Kconfig
    ├── SConscript
    ├── package.json
    └── README.md
```

## 🔗 Package依赖关系

```
voice-assistant (顶层应用)
    ├── depends on: audio-codec
    ├── depends on: web-client
    └── depends on: ai-cloud
        └── depends on: web-client

audio-codec (独立)
web-client (独立)
```

## 📋 各Package功能说明

### 1. audio-codec（音频编解码包）

**功能：**
- 音频采集（麦克风输入）
- 音频播放（扬声器输出）
- 支持ADC/I2S/I2C接口
- 可配置采样率、通道数、位深度

**配置选项：**
```
PKG_USING_AUDIO_CODEC
├── PKG_AUDIO_CODEC_SAMPLE_RATE (16000)
├── PKG_AUDIO_CODEC_CHANNELS (1)
├── PKG_AUDIO_CODEC_BITS (16)
└── PKG_AUDIO_CODEC_BUFFER_SIZE (16)
```

**对外接口：**
```c
audio_capture_init()
audio_capture_start()
audio_capture_read()
audio_capture_stop()

audio_player_init()
audio_player_play()
audio_player_stop()
```

### 2. web-client（HTTP客户端包）

**功能：**
- HTTP GET/POST请求
- 文件上传（multipart/form-data）
- URL解析
- 超时控制

**配置选项：**
```
PKG_USING_WEB_CLIENT
├── PKG_WEB_CLIENT_BUFFER_SIZE (4096)
├── PKG_WEB_CLIENT_RESPONSE_MAX (64)
└── PKG_WEB_CLIENT_TIMEOUT (30)
```

**对外接口：**
```c
web_client_get()
web_client_post()
web_client_post_file()
web_client_free_response()
```

### 3. ai-cloud（AI云服务包）

**功能：**
- 语音识别（STT）
- 语音合成（TTS）
- 支持多平台（百度、讯飞、阿里云）
- Base64编解码

**配置选项：**
```
PKG_USING_AI_CLOUD
├── PKG_AI_CLOUD_PROVIDER (0=百度,1=讯飞,2=阿里云)
├── PKG_AI_CLOUD_API_KEY
├── PKG_AI_CLOUD_APP_ID
└── PKG_AI_CLOUD_API_URL
```

**对外接口：**
```c
ai_cloud_service_init()
ai_cloud_service_speech_to_text()
ai_cloud_service_text_to_speech()
ai_cloud_service_full_duplex()
```

### 4. voice-assistant（语音助手包）

**功能：**
- 语音助手主控制
- 唤醒词检测
- 状态机管理
- 自动化工作流

**配置选项：**
```
PKG_USING_VOICE_ASSISTANT
├── PKG_VOICE_WAKEUP_ENABLE (0/1)
├── PKG_VOICE_WAKEUP_WORD ("Hi小石")
├── PKG_VOICE_RECORD_DURATION (5)
└── PKG_VOICE_AUTO_START (1)
```

**对外接口：**
```c
voice_assistant_init()
voice_assistant_start()
voice_assistant_trigger()
voice_assistant_stop()

wakeup_detector_init()
wakeup_detector_start()
wakeup_detector_stop()
```

## 🛠️ 如何使用Package模式

### 方式一：使用本地Packages（推荐）

#### 1. 创建Package目录结构

```bash
# Windows PowerShell
cd packages
mkdir audio-codec\inc, audio-codec\src
mkdir web-client\inc, web-client\src
mkdir ai-cloud\inc, ai-cloud\src
mkdir voice-assistant\inc, voice-assistant\src, voice-assistant\docs
```

#### 2. 复制文件到对应Package

**audio-codec:**
```
applications/audio_capture.h     → packages/audio-codec/inc/
applications/audio_capture.c     → packages/audio-codec/src/
applications/audio_player.h      → packages/audio-codec/inc/
applications/audio_player.c      → packages/audio-codec/src/
```

**web-client:**
```
applications/web_client.h        → packages/web-client/inc/
applications/web_client.c        → packages/web-client/src/
```

**ai-cloud:**
```
applications/ai_cloud_service.h  → packages/ai-cloud/inc/
applications/ai_cloud_service.c  → packages/ai-cloud/src/
```

**voice-assistant:**
```
applications/voice_assistant.h   → packages/voice-assistant/inc/
applications/voice_assistant.c   → packages/voice-assistant/src/
applications/voice_assistant_config.h → packages/voice-assistant/inc/
applications/wakeup_detector.h   → packages/voice-assistant/inc/
applications/wakeup_detector.c   → packages/voice-assistant/src/
applications/QUICK_START.md      → packages/voice-assistant/docs/
applications/WAKEUP_GUIDE.md     → packages/voice-assistant/docs/
```

#### 3. 创建Package配置文件

为每个package创建：
- `Kconfig` - menuconfig配置
- `SConscript` - SCons构建脚本
- `package.json` - 包信息
- `README.md` - 说明文档

#### 4. 在Kconfig中启用Packages

编辑主Kconfig，添加：

```kconfig
menu "Voice Assistant Packages"
    source "$PKGS_DIR/audio-codec/Kconfig"
    source "$PKGS_DIR/web-client/Kconfig"
    source "$PKGS_DIR/ai-cloud/Kconfig"
    source "$PKGS_DIR/voice-assistant/Kconfig"
endmenu
```

#### 5. 在主SConscript中包含Packages

编辑 `packages/SConscript`，添加：

```python
objs = []

# 添加本地packages
if GetDepend(['PKG_USING_AUDIO_CODEC']):
    objs = objs + SConscript(os.path.join('audio-codec', 'SConscript'))

if GetDepend(['PKG_USING_WEB_CLIENT']):
    objs = objs + SConscript(os.path.join('web-client', 'SConscript'))

if GetDepend(['PKG_USING_AI_CLOUD']):
    objs = objs + SConscript(os.path.join('ai-cloud', 'SConscript'))

if GetDepend(['PKG_USING_VOICE_ASSISTANT']):
    objs = objs + SConscript(os.path.join('voice-assistant', 'SConscript'))

Return('objs')
```

#### 6. 配置和编译

```bash
# 配置（如果使用menuconfig）
menuconfig

# 或直接编辑rtconfig.h添加：
# #define PKG_USING_AUDIO_CODEC
# #define PKG_USING_WEB_CLIENT
# #define PKG_USING_AI_CLOUD
# #define PKG_USING_VOICE_ASSISTANT

# 编译
scons -j8
```

### 方式二：保持现有结构（简单）

如果不想重组代码，保持当前在 `applications/` 目录的结构也可以正常工作。

优点：
- ✅ 简单直接，无需移动文件
- ✅ 适合快速开发和测试

缺点：
- ❌ 代码耦合度高
- ❌ 不利于代码复用
- ❌ 不符合RT-Thread package规范

## 📊 两种方式对比

| 特性 | Package模式 | Applications模式 |
|------|------------|-----------------|
| 模块化 | ⭐⭐⭐⭐⭐ | ⭐⭐ |
| 可复用性 | ⭐⭐⭐⭐⭐ | ⭐ |
| 维护性 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ |
| 配置灵活性 | ⭐⭐⭐⭐⭐ | ⭐⭐ |
| 开发速度 | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| 代码复杂度 | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |

## 🎯 推荐方案

### 当前阶段（开发测试）
保持 `applications/` 结构，快速迭代开发

### 稳定后（发布共享）
重构为 `packages/` 结构，便于维护和分享

### 长期目标
发布到RT-Thread官方packages仓库

## 📝 迁移步骤（详细）

### 步骤1：创建Package框架

在 `packages/` 目录创建4个package的完整结构（已提供模板）

### 步骤2：移动源文件

使用文件管理器或命令行移动文件到对应package的 `inc/` 和 `src/` 目录

### 步骤3：修改include路径

所有包含这些头文件的地方，从：
```c
#include "audio_capture.h"
```
改为：
```c
#include <audio_capture.h>  // 使用<>而不是""
```

SCons会自动处理路径。

### 步骤4：更新main.c

```c
// applications/main.c
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_common.h"

#ifdef PKG_USING_VOICE_ASSISTANT
#include <voice_assistant.h>
#endif

int main(void)
{
    // ... LED初始化 ...

#ifdef PKG_USING_VOICE_ASSISTANT
    voice_assistant_init();
    voice_assistant_start();
#endif

    // ... 主循环 ...
    return RT_EOK;
}
```

### 步骤5：测试编译

```bash
scons -c       # 清理
scons -j8      # 重新编译
```

### 步骤6：测试功能

确保所有功能正常工作。

## 🔧 故障排除

**问题1：找不到头文件**
- 检查 `SConscript` 中的 `CPPPATH` 设置
- 确保使用 `<>` 包含头文件

**问题2：链接错误**
- 检查 `SConscript` 中的 `src` 设置
- 确保所有 `.c` 文件都被包含

**问题3：Package未生效**
- 检查 Kconfig 配置
- 确保定义了相应的宏（如 `PKG_USING_AUDIO_CODEC`）

## 📚 参考资料

- [RT-Thread软件包开发指南](https://www.rt-thread.org/document/site/development-guide/package/package/)
- [SCons构建系统](https://www.rt-thread.org/document/site/programming-manual/scons/scons/)
- [Kconfig配置系统](https://www.rt-thread.org/document/site/development-guide/kconfig/kconfig/)

## 总结

**当前推荐：** 先保持 `applications/` 结构完成开发和测试

**后续优化：** 产品化时再迁移到 `packages/` 结构

两种方式都能正常工作，根据项目需求选择！

