# 内存优化指南

## 问题描述

如果你遇到类似错误：
```
[E/web.client] Failed to allocate receive buffer
```

这说明系统可用内存不足，无法分配HTTP响应缓冲区。

## ✅ 已应用的优化

我已经进行了以下优化：

### 1. 减小缓冲区大小

**修改前：**
```c
#define HTTP_RESPONSE_MAX   (64 * 1024)  // 64KB
```

**修改后：**
```c
#define HTTP_RESPONSE_MAX   (32 * 1024)  // 32KB
```

### 2. 添加内存检查工具

新增了 `memory_helper.c` 提供 `meminfo` 命令。

## 🔧 解决方案

### 方案1：重新编译（推荐）

```bash
# 清理旧文件
scons -c

# 重新编译（使用优化后的代码）
scons -j8

# 下载到开发板
```

### 方案2：检查内存使用

```bash
# 查看内存使用情况
msh> free

# 或使用详细信息
msh> meminfo
```

### 方案3：释放内存

如果内存仍然不足，尝试：

```bash
# 1. 停止不必要的服务
msh> va_stop              # 停止语音助手

# 2. 重启系统（最有效）
msh> reboot

# 3. 重试AI测试
msh> ai_say 你好
```

## 📊 内存需求

各功能的典型内存需求：

| 功能 | 内存需求 | 说明 |
|------|---------|------|
| HTTP缓冲区 | 32 KB | AI通信必需 |
| 音频缓冲区 | 16 KB | 录音/播放 |
| 语音助手 | 4 KB | 控制结构 |
| 栈空间 | 8-16 KB | 线程栈 |
| **总计** | **~60 KB** | 最低要求 |

建议系统至少有 **128 KB** 可用RAM。

## 🎯 进一步优化选项

### 选项1：减小音频缓冲区

编辑 `audio_capture.h`：

```c
// 从 16KB 减小到 8KB
#define AUDIO_BUFFER_SIZE  (1024 * 8)  
```

### 选项2：减小HTTP缓冲区（如果32KB还不够）

编辑 `web_client.c`：

```c
// 从 32KB 减小到 16KB
#define HTTP_RESPONSE_MAX   (16 * 1024)
```

⚠️ **注意**：太小的缓冲区可能导致AI响应被截断。

### 选项3：禁用部分功能

编辑 `voice_assistant_config.h`：

```c
// 禁用唤醒词检测（节省内存）
#define VOICE_WAKEUP_ENABLE     0

// 禁用音频文件保存
#define VOICE_SAVE_AUDIO_FILE   0
```

### 选项4：优化编译选项

在 `rtconfig.py` 中：

```python
# 启用编译优化
CFLAGS = ' -Os'  # 优化代码大小
```

## 🔍 诊断步骤

### 1. 检查当前内存

```bash
msh> free

# 输出示例：
total    : 262144
used     : 102400
free     : 159744
```

如果 `free` 小于 64KB，内存不足！

### 2. 查看线程占用

```bash
msh> list_thread

# 检查各线程的栈大小
```

### 3. 测试分配

```bash
# 重新编译后测试
msh> ai_say 测试
```

## 🚨 常见错误及解决

### 错误1：内存分配失败

**症状：**
```
[E/web.client] Failed to allocate receive buffer (32768 bytes)
Available memory insufficient. Run 'free' command to check
```

**解决：**
1. 运行 `free` 检查可用内存
2. 如果不足，重启系统：`reboot`
3. 或减小缓冲区大小

### 错误2：内存碎片

**症状：**
`free` 显示有足够内存，但仍然分配失败

**原因：**
内存碎片化，没有连续的大块内存

**解决：**
```bash
# 重启系统清理内存碎片
msh> reboot
```

### 错误3：内存泄漏

**症状：**
运行一段时间后可用内存持续减少

**解决：**
```bash
# 1. 停止所有服务
msh> va_stop

# 2. 检查内存
msh> free

# 3. 如果内存未释放，重启
msh> reboot
```

## 💡 最佳实践

### 1. 监控内存使用

定期运行：
```bash
msh> free
```

### 2. 及时清理

不使用时停止服务：
```bash
msh> va_stop
msh> wakeup_stop
```

### 3. 避免长时间运行

定期重启系统以清理内存：
```bash
# 每运行数小时后重启
msh> reboot
```

### 4. 优先级设置

重要功能优先保证内存：
- AI通信 > 音频采集 > 唤醒词检测

## 📝 配置建议

### 最小配置（低内存系统）

```c
// voice_assistant_config.h
#define VOICE_WAKEUP_ENABLE     0    // 禁用唤醒词
#define VOICE_RECORD_DURATION   3    // 减少录音时长
#define VOICE_BUFFER_SIZE       (16000 * 2 * 3)  // 3秒音频

// web_client.c
#define HTTP_RESPONSE_MAX   (16 * 1024)  // 16KB

// audio_capture.h
#define AUDIO_BUFFER_SIZE   (1024 * 8)   // 8KB
```

### 标准配置（当前设置）

```c
// voice_assistant_config.h
#define VOICE_WAKEUP_ENABLE     1    // 启用唤醒词
#define VOICE_RECORD_DURATION   5    // 5秒录音
#define VOICE_BUFFER_SIZE       (16000 * 2 * 5)

// web_client.c
#define HTTP_RESPONSE_MAX   (32 * 1024)  // 32KB ✅ 已优化

// audio_capture.h
#define AUDIO_BUFFER_SIZE   (1024 * 16)  // 16KB
```

### 完整配置（高内存系统）

```c
// voice_assistant_config.h
#define VOICE_WAKEUP_ENABLE     1
#define VOICE_RECORD_DURATION   10   // 10秒录音

// web_client.c
#define HTTP_RESPONSE_MAX   (64 * 1024)  // 64KB

// audio_capture.h
#define AUDIO_BUFFER_SIZE   (1024 * 32)  // 32KB
```

## 🔄 现在就试试

### 步骤1：重新编译

```bash
cd /path/to/project
scons -c
scons -j8
```

### 步骤2：下载并运行

下载固件到开发板

### 步骤3：测试

```bash
# 检查内存
msh> free

# 测试AI服务
msh> ai_say 你好世界
```

## ✅ 预期结果

优化后，你应该看到：

```bash
msh> ai_say 你好世界

========================================
[You] 你好世界
========================================
[I/ai.test] Sending text to AI: 你好世界
[D/web.client] Allocated 32768 bytes for HTTP response  ✅
[I/ai.cloud] Starting text to speech: 你好世界
[I/ai.cloud] Text to speech successful (audio_len: 12800)
[AI] Processing completed!
Audio data received: 12800 bytes
Playing audio...
Playback completed!
========================================
```

## 📞 仍然有问题？

如果优化后仍然失败：

1. **检查系统RAM大小**
   - 确认开发板至少有 128KB RAM
   - 查看产品手册

2. **检查其他服务**
   ```bash
   list_thread    # 查看所有线程
   ```

3. **联系支持**
   - 提供 `free` 命令输出
   - 提供完整的错误日志
   - 说明开发板型号

---

**重新编译后，内存问题应该得到改善！** 🚀

