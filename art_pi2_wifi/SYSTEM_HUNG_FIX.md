# 系统卡死/无响应 解决方案

## 🚨 症状

- 终端无法输入命令
- 系统挂起/冻结
- 串口无响应

## 💥 原因

**内存耗尽导致系统崩溃或挂起**

当系统尝试分配大块内存（如32KB HTTP缓冲区）失败时，可能导致：
- 线程阻塞
- 系统挂起
- 看门狗超时

## ✅ 立即解决步骤

### 步骤1：硬件重启

**方法A：按复位按钮**
- 找到开发板上的 RESET/RST 按钮
- 按下并释放

**方法B：重新上电**
- 拔掉USB线
- 等待3秒
- 重新插入USB线

### 步骤2：检查内存

系统重启后，**立即**运行：

```bash
msh> free
```

**期望输出：**
```
total    : 262144  (总内存，实际值看你的板子)
used     : xxxxxx
free     : xxxxxx  ← 这个值很重要！
```

**判断标准：**
- `free` > 100KB : ✅ 内存充足，可以测试
- `free` = 70-100KB : ⚠️ 内存紧张，可能失败
- `free` < 70KB : ❌ 内存不足，必须优化

### 步骤3：根据内存情况决定

#### 如果 free > 100KB

```bash
# 直接测试
msh> ai_say 你好
```

#### 如果 free = 70-100KB

```bash
# 不启动语音助手，只测试AI
msh> ai_say 你好
```

#### 如果 free < 70KB

**必须重新编译！** 我已经优化了代码。

## 🔧 重新编译（必须！）

### Windows:

```powershell
cd d:\RT-ThreadStudio\workspace\Desktop-voice-assistant\art_pi2_wifi

# 清理
scons -c

# 重新编译
scons -j8

# 下载到开发板
```

### Linux/Mac:

```bash
cd /path/to/art_pi2_wifi

# 清理
scons -c

# 重新编译
scons -j8

# 下载到开发板
```

## 📊 优化效果对比

| 项目 | 优化前 | 优化后 | 节省 |
|------|--------|--------|------|
| HTTP缓冲区 | 64 KB | 16 KB | 48 KB |
| 音频采集 | 16 KB | 8 KB | 8 KB |
| 音频播放 | 16 KB | 8 KB | 8 KB |
| 录音时长 | 5秒(160KB) | 3秒(96KB) | 64 KB |
| **总节省** | - | - | **~130 KB** 🎉 |

## 🎯 禁用自动启动（省内存）

我已经修改了配置，语音助手不会自动启动，这样可以节省内存。

**修改内容：**
```c
// main.c
#define VOICE_ASSISTANT_AUTO_START  0  // 从1改为0
```

**效果：**
- 系统启动时不自动初始化语音助手
- 只有在运行 `ai_say` 时才分配内存
- 节省约 20-30KB 启动时的内存占用

## 🔍 重启后诊断命令

```bash
# 1. 检查内存
msh> free

# 2. 检查线程（看哪些在运行）
msh> list_thread

# 3. 测试AI（会自动初始化）
msh> ai_say 测试
```

## 📝 预防措施

### 1. 定期重启

长时间运行后，内存可能碎片化，建议定期重启：

```bash
msh> reboot
```

### 2. 不用时停止服务

```bash
msh> va_stop           # 停止语音助手
msh> wakeup_stop       # 停止唤醒检测
```

### 3. 监控内存

定期检查：
```bash
msh> free
```

## ⚠️ 如果优化后仍然卡死

### 终极方案：最小配置

编辑 `voice_assistant_config.h`：

```c
// 禁用唤醒词
#define VOICE_WAKEUP_ENABLE     0

// 进一步减少录音时长
#define VOICE_RECORD_DURATION   2  // 改为2秒

// 禁用音频保存
#define VOICE_SAVE_AUDIO_FILE   0
```

编辑 `web_client.c`：

```c
// 如果16KB还不够，降到8KB
#define HTTP_RESPONSE_MAX   (8 * 1024)  // 8KB（最小值）
```

**重新编译！**

## 🆘 仍然无法解决？

可能是硬件限制，请检查：

### 1. 确认RAM大小

查看开发板规格：
- ART-Pi2 应该有 1MB SRAM
- 如果小于 256KB，功能可能受限

### 2. 检查是否启用外部RAM

某些开发板有外部PSRAM/SDRAM，确保已启用：
- 检查 `rtconfig.h` 中的配置
- 查看硬件文档

### 3. 减少其他功能

如果还运行了其他应用：
- 暂时禁用不必要的功能
- 减少线程数量
- 降低栈大小

## 📞 技术支持

如果问题仍然存在，请提供：

1. `free` 命令输出
2. `list_thread` 输出
3. 开发板型号
4. RAM配置信息
5. 完整的错误日志

## ✅ 成功标志

优化并重新编译后，应该看到：

```bash
msh> free
total    : 262144
used     : 150000
free     : 112144  ✅ 足够了！

msh> ai_say 你好

========================================
[You] 你好
========================================
[D/web.client] Allocated 16384 bytes for HTTP response  ✅
[I/ai.cloud] Text to speech successful
[AI] Processing completed!
Audio data received: 6400 bytes
Playing audio...
Playback completed!
========================================
```

---

## 🎯 现在就做

1. **硬件重启开发板** （按RESET或重新上电）
2. **运行 `free` 查看内存**
3. **重新编译项目**（使用优化后的代码）
4. **下载并测试**

**代码已经优化好了，重新编译即可解决问题！** 🚀

