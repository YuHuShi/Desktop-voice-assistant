# 紧急内存修复方案

## ⚠️ 当前问题

系统无法分配32KB内存用于HTTP响应，说明可用内存严重不足。

## 🚨 立即尝试的方法

### 方法1：重启系统（最快）

```bash
msh> reboot
```

然后重试：
```bash
msh> ai_say 你好
```

### 方法2：检查内存

```bash
msh> free
```

查看输出中的 `free` 值，如果小于 50KB，内存严重不足。

### 方法3：停止其他服务

```bash
# 停止语音助手
msh> va_stop

# 停止唤醒词检测
msh> wakeup_stop

# 再次检查内存
msh> free

# 重试
msh> ai_say 测试
```

## 🔧 已应用的极限优化

我刚刚进行了更激进的优化：

| 项目 | 原值 | 优化后 | 节省 |
|------|------|--------|------|
| HTTP缓冲区 | 32KB | **16KB** | 16KB |
| 音频采集缓冲 | 16KB | **8KB** | 8KB |
| 音频播放缓冲 | 16KB | **8KB** | 8KB |
| 录音时长 | 5秒 | **3秒** | ~32KB |
| **总节省** | - | - | **~64KB** |

## 🚀 重新编译

```bash
# 1. 清理
scons -c

# 2. 重新编译（使用极限优化）
scons -j8

# 3. 下载到开发板
```

## 📊 最小内存需求

优化后的最小内存需求：

```
HTTP缓冲区:      16 KB
音频采集:         8 KB  
音频播放:         8 KB
语音助手:         4 KB
栈空间:          8 KB
系统开销:        10 KB
----------------------------
总计:           ~54 KB
```

**建议系统至少有 80KB 可用RAM。**

## 🔍 诊断命令

### 1. 查看内存
```bash
msh> free
```

**期望输出：**
```
total    : 262144  (256KB)
used     : 180000
free     : 82144   ← 应该 > 60KB
```

### 2. 查看线程
```bash
msh> list_thread
```

查找占用内存多的线程。

### 3. 查看对象
```bash
msh> list_mem
```

## ⚡ 如果仍然失败

### 选项A：禁用唤醒词（节省更多内存）

编辑 `voice_assistant_config.h`：

```c
// 禁用唤醒词检测
#define VOICE_WAKEUP_ENABLE     0
```

重新编译。

### 选项B：使用最小配置

只启用AI测试功能，禁用其他：

在 `main.c` 中：

```c
int main(void)
{
    // ... LED初始化 ...

    // 注释掉自动启动语音助手
    // #if VOICE_ASSISTANT_AUTO_START
    //     voice_assistant_init();
    //     voice_assistant_start();
    // #endif

    // 主循环
    while(1) {
        // ...
    }
}
```

这样只有在手动运行 `ai_say` 时才分配内存。

### 选项C：增加系统内存（硬件方案）

如果是内存真的不够用：

1. 检查硬件配置是否启用了所有可用RAM
2. 查看是否可以扩展外部RAM
3. 考虑使用内存更大的开发板型号

## 📝 临时测试方法

如果无法立即重新编译，可以尝试：

```bash
# 1. 重启清理内存
msh> reboot

# 2. 不启动语音助手，直接测试AI
msh> ai_say 你好

# 3. 如果失败，再次重启后立即测试
msh> reboot
msh> ai_say 测试
```

## 🎯 最终解决方案

### 标准方案：重新编译

```bash
scons -c && scons -j8
```

### 应急方案：重启系统

```bash
reboot
```

### 长期方案：优化配置

根据实际内存情况调整所有缓冲区大小。

## 📞 获取帮助

如果问题依然存在，请提供以下信息：

1. `free` 命令的完整输出
2. `list_thread` 的输出
3. 开发板型号和RAM大小
4. 系统配置（是否启用了PSRAM等）

## ✅ 预期成功输出

重新编译并重启后，应该看到：

```bash
msh> free
total    : 262144
used     : 160000
free     : 102144  ✅ 足够了

msh> ai_say 你好

========================================
[You] 你好
========================================
[D/web.client] Allocated 16384 bytes for HTTP response  ✅
[I/ai.cloud] Text to speech successful
[AI] Processing completed!
========================================
```

---

**现在请先运行 `free` 命令，告诉我可用内存情况！** 📊

