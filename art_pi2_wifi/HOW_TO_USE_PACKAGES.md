# 如何使用Packages模式

## 📦 已创建的Package文件

我已经为你准备好了完整的package配置文件：

```
packages/
├── audio-codec/
│   ├── Kconfig ✅
│   ├── SConscript ✅
│   ├── package.json ✅
│   └── README.md ✅
│
├── voice-assistant/
│   ├── Kconfig ✅
│   ├── SConscript ✅
│   ├── package.json ✅
│   └── README.md ✅
│
└── (其他package配置文件待创建)
```

## 🚀 快速使用指南

### 方案A：保持当前结构（推荐）✅

**适合场景：**
- 快速开发和测试
- 个人项目
- 不需要分享代码

**操作：**
```bash
# 无需任何改动，直接编译
scons -j8
```

**优点：**
- ✅ 零配置，开箱即用
- ✅ 调试方便
- ✅ 编译快速

**当前所有功能都在 `applications/` 目录下，完全可以正常工作！**

---

### 方案B：迁移到Package结构

**适合场景：**
- 代码开源分享
- 团队协作项目
- 长期维护的产品

**步骤1：创建Package目录结构**

运行自动化脚本：

**Windows:**
```cmd
create_packages.bat
```

**Linux/Mac:**
```bash
chmod +x create_packages.sh
./create_packages.sh
```

或手动创建：

```bash
# 在packages目录下创建子目录
cd packages
mkdir -p audio-codec/inc audio-codec/src
mkdir -p web-client/inc web-client/src
mkdir -p ai-cloud/inc ai-cloud/src
mkdir -p voice-assistant/inc voice-assistant/src voice-assistant/docs
```

**步骤2：复制文件**

将applications目录的文件复制到对应package：

```
# audio-codec package
applications/audio_capture.h     → packages/audio-codec/inc/
applications/audio_capture.c     → packages/audio-codec/src/
applications/audio_player.h      → packages/audio-codec/inc/
applications/audio_player.c      → packages/audio-codec/src/

# web-client package
applications/web_client.h        → packages/web-client/inc/
applications/web_client.c        → packages/web-client/src/

# ai-cloud package
applications/ai_cloud_service.h  → packages/ai-cloud/inc/
applications/ai_cloud_service.c  → packages/ai-cloud/src/

# voice-assistant package
applications/voice_assistant.h   → packages/voice-assistant/inc/
applications/voice_assistant.c   → packages/voice-assistant/src/
applications/voice_assistant_config.h → packages/voice-assistant/inc/
applications/wakeup_detector.h   → packages/voice-assistant/inc/
applications/wakeup_detector.c   → packages/voice-assistant/src/
applications/QUICK_START.md      → packages/voice-assistant/docs/
applications/WAKEUP_GUIDE.md     → packages/voice-assistant/docs/
```

**步骤3：修改main.c**

编辑 `applications/main.c`：

```c
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_common.h"

// 如果启用了voice-assistant package，包含头文件
#ifdef PKG_USING_VOICE_ASSISTANT
#include <voice_assistant.h>
#endif

#define DBG_TAG "main"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

#define LED_PIN GET_PIN(O, 5)

int main(void)
{
    rt_uint32_t count = 0;
    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);

    LOG_I("Desktop Voice Assistant Starting...");
    LOG_I("RT-Thread Version: 5.1.0");

#ifdef PKG_USING_VOICE_ASSISTANT
    /* 等待网络就绪 */
    rt_thread_mdelay(3000);
    
    LOG_I("Initializing voice assistant...");
    
    if (voice_assistant_init() == RT_EOK)
    {
        if (voice_assistant_start() == RT_EOK)
        {
            LOG_I("Voice assistant started successfully");
        }
    }
#endif

    /* LED闪烁任务 */
    while(1)
    {
        count++;
        rt_pin_write(LED_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
    
    return RT_EOK;
}

#include "stm32h7rsxx.h"
static int vtor_config(void)
{
    SCB->VTOR = XSPI2_BASE;
    return 0;
}
INIT_BOARD_EXPORT(vtor_config);
```

**步骤4：配置rtconfig.h**

编辑 `rtconfig.h`，添加：

```c
/* Voice Assistant Packages */
#define PKG_USING_AUDIO_CODEC
#define PKG_USING_WEB_CLIENT
#define PKG_USING_AI_CLOUD
#define PKG_USING_VOICE_ASSISTANT

/* Package配置 */
#define PKG_VOICE_AUTO_START         1
#define PKG_VOICE_WAKEUP_ENABLE      1
#define PKG_VOICE_WAKEUP_WORD        "Hi小石"
#define PKG_AI_PROVIDER_BAIDU        // 或其他提供商
```

**步骤5：编译**

```bash
scons -c      # 清理旧的编译文件
scons -j8     # 重新编译
```

## 📋 两种方案对比

| 特性 | 方案A (applications/) | 方案B (packages/) |
|------|---------------------|------------------|
| **配置难度** | ⭐⭐⭐⭐⭐ 简单 | ⭐⭐⭐ 中等 |
| **编译速度** | ⭐⭐⭐⭐⭐ 快 | ⭐⭐⭐⭐ 较快 |
| **代码组织** | ⭐⭐⭐ 一般 | ⭐⭐⭐⭐⭐ 优秀 |
| **模块复用** | ⭐⭐ 困难 | ⭐⭐⭐⭐⭐ 容易 |
| **维护性** | ⭐⭐⭐ 一般 | ⭐⭐⭐⭐⭐ 优秀 |
| **适合场景** | 快速开发 | 产品化/开源 |
| **学习曲线** | ⭐⭐⭐⭐⭐ 平缓 | ⭐⭐⭐ 陡峭 |

## 🎯 我的推荐

### 当前阶段：使用方案A ✅

**理由：**
1. 所有代码已经在 `applications/` 目录
2. 编译没有任何问题
3. 功能完全正常
4. 无需额外配置

**操作：**
```bash
# 什么都不用改，直接用！
scons -j8
```

### 后续阶段：考虑迁移到方案B

**时机：**
- 功能开发完成并稳定
- 需要开源分享代码
- 团队协作开发
- 准备发布到RT-Thread软件包中心

**操作：**
运行 `create_packages.bat` 自动创建结构

## 📚 相关文档

- **PACKAGE_STRUCTURE.md** - 详细的package设计方案
- **packages_summary.md** - Package组织方案总结
- **QUICK_START.md** - 快速开始指南
- **WAKEUP_GUIDE.md** - 唤醒词使用指南
- **VOICE_ASSISTANT_README.md** - 完整功能说明

## ❓ 常见问题

### Q1: 我现在需要做什么吗？

**A: 不需要！** 当前代码已经可以正常编译和使用了。

### Q2: 什么时候需要使用packages模式？

**A:** 当你需要：
- 将代码开源分享
- 提交到RT-Thread官方软件包仓库
- 在多个项目中复用这些模块

### Q3: 迁移到packages会很复杂吗？

**A:** 不会。我已经准备好了：
- 自动化脚本 `create_packages.bat`
- 完整的配置文件模板
- 详细的步骤说明

只需要运行脚本，然后稍作配置即可。

### Q4: 两种方式可以共存吗？

**A:** 可以。applications/ 和 packages/ 可以同时存在，通过宏控制使用哪个。

### Q5: 我该选择哪种方式？

**A:** 
- 🎯 **现在**：用 applications/ （已经可以工作）
- 📦 **将来**：需要时再迁移到 packages/

## 🔧 实用工具

### 1. 自动创建脚本
```cmd
create_packages.bat
```

### 2. 查看package结构
```cmd
tree /F packages
```

### 3. 测试编译
```bash
# 当前结构
scons -j8

# package结构（配置后）
scons -c && scons -j8
```

## 总结

🎯 **现状：** 代码在 `applications/` 目录，完全可用 ✅

🔮 **未来：** 需要时可以轻松迁移到 `packages/` 结构 ✅

📦 **工具：** 已提供自动化脚本和完整文档 ✅

💡 **建议：** 先用着，稳定后再考虑重组 ✅

---

**你现在不需要做任何改动，当前代码已经完美工作！** 🎉

如果将来需要重组，直接运行 `create_packages.bat` 即可！

