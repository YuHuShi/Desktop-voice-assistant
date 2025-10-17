# AI云端测试指南

## 快速测试AI云服务

### 📋 前提条件

1. **WiFi已连接**
2. **已配置API密钥**（在 `voice_assistant_config.h` 中）

### 🚀 快速开始

#### 1. 连接WiFi（如果还没连接）

```bash
msh> wifi scan                          # 扫描WiFi网络
msh> wifi join YourWiFi YourPassword    # 连接WiFi
msh> ifconfig                           # 确认已获取IP
```

#### 2. 初始化AI测试工具（可选，首次使用会自动初始化）

```bash
msh> ai_init
```

#### 3. 测试AI云服务

##### 方式A：发送文本，AI回复语音（推荐）

```bash
msh> ai_say 你好世界

# 输出：
========================================
[You] 你好世界
========================================

[AI] Processing completed!
Audio data received: 12800 bytes
Playing audio...
Playback completed!
========================================
```

##### 方式B：文本对话测试

```bash
msh> ai_chat 今天天气怎么样

# 输出：
========================================
[You] 今天天气怎么样
[AI]  Processing: 今天天气怎么样
      (AI replied with audio)
========================================
```

## 📝 完整命令列表

### 核心命令

| 命令 | 功能 | 示例 |
|------|------|------|
| `ai_say <文本>` | 发送文本到AI，播放语音回复 | `ai_say 你好` |
| `ai_chat <文本>` | 文本对话测试（不播放） | `ai_chat 测试` |
| `ai_init` | 初始化AI测试工具 | `ai_init` |
| `ai_status` | 查看AI工具状态 | `ai_status` |
| `ai_network` | 显示网络测试命令 | `ai_network` |

### 网络测试命令

| 命令 | 功能 |
|------|------|
| `ifconfig` | 查看网络状态 |
| `ping baidu.com` | 测试网络连接 |
| `wifi scan` | 扫描WiFi |
| `wifi join SSID PWD` | 连接WiFi |

## 🎯 测试场景示例

### 场景1：基础测试

```bash
# 1. 检查网络
msh> ifconfig

# 2. 发送简单文本
msh> ai_say 你好

# 3. 等待AI处理和播放
```

### 场景2：多词测试

```bash
msh> ai_say 今天是个好日子

msh> ai_say 讲个笑话

msh> ai_say 报告当前时间
```

### 场景3：中英文混合

```bash
msh> ai_say Hi 小石

msh> ai_say Hello world 你好世界
```

### 场景4：长句测试

```bash
msh> ai_say 人工智能技术正在改变我们的生活
```

## 🔍 输出说明

### 成功输出

```bash
msh> ai_say 测试

========================================
[You] 测试
========================================

[I/ai.cloud] Starting text to speech: 测试
[I/ai.cloud] Text to speech successful (audio_len: 6400)
[AI] Processing completed!
Audio data received: 6400 bytes
[I/audio.player] Audio playback started (size: 6400 bytes)
Playing audio...
Playback completed!
========================================
```

### 错误输出示例

#### 网络错误

```bash
[Error] AI service failed
Error message: HTTP request failed

Troubleshooting:
  1. Check network: ping baidu.com
  2. Check API key configuration
  3. Check API quota/balance
```

#### API配置错误

```bash
[Error] AI service failed
Error message: Invalid API key

Please check voice_assistant_config.h
```

## ⚙️ 配置说明

### 编辑配置文件

打开 `applications/voice_assistant_config.h`：

```c
// 选择AI服务提供商
#define AI_SERVICE_PROVIDER     0  // 0=百度, 1=讯飞, 2=阿里云

// 百度AI配置
#define BAIDU_API_KEY           "your_api_key_here"
#define BAIDU_SECRET_KEY        "your_secret_key_here"
#define BAIDU_APP_ID            "your_app_id_here"
```

### 获取API密钥

#### 百度AI
1. 访问 https://ai.baidu.com/
2. 注册并登录
3. 创建应用（语音技术 → 语音合成）
4. 获取 API Key、Secret Key 和 App ID

#### 讯飞
1. 访问 https://www.xfyun.cn/
2. 注册并登录
3. 创建应用（语音合成）
4. 获取 APPID、API Key 和 API Secret

#### 阿里云
1. 访问 https://www.aliyun.com/
2. 开通智能语音服务
3. 获取相应密钥

## 🐛 故障排除

### 问题1：命令找不到

**症状：**
```bash
msh> ai_say
command not found: ai_say
```

**解决：**
- 确认已编译包含 `ai_test_tool.c`
- 重新编译固件

### 问题2：网络连接失败

**症状：**
```bash
[Error] Failed to resolve host
```

**解决步骤：**
```bash
# 1. 检查WiFi连接
msh> ifconfig

# 2. 如果未连接，重新连接
msh> wifi join YourSSID YourPassword

# 3. 测试网络
msh> ping baidu.com

# 4. 重试AI命令
msh> ai_say 测试
```

### 问题3：API认证失败

**症状：**
```bash
[Error] AI service failed
Error message: Invalid API key
```

**解决：**
1. 检查 `voice_assistant_config.h` 中的API密钥
2. 确认API密钥有效且未过期
3. 检查API服务余额
4. 重新编译固件

### 问题4：没有声音输出

**症状：**
AI返回成功，但听不到声音

**原因：**
- 没有连接扬声器
- DAC设备未初始化

**确认：**
```bash
msh> ai_say 测试

# 会显示：
Audio data received: 6400 bytes
Warning: Audio playback failed (no audio device?)
But AI service is working correctly!
```

**说明：**
AI服务本身工作正常，只是音频输出硬件未配置。

### 问题5：初始化失败

**症状：**
```bash
[Error] Failed to initialize AI test tool
```

**检查清单：**
- [ ] WiFi是否连接（`ifconfig`）
- [ ] API密钥是否配置
- [ ] 网络是否通畅（`ping baidu.com`）
- [ ] API服务是否可用

## 📊 性能测试

### 延迟测试

```bash
# 记录发送到接收的时间
msh> ai_say 测试

# 典型延迟：
# - 网络请求：500-2000ms
# - 音频合成：200-500ms
# - 总延迟：1-3秒
```

### 压力测试

```bash
# 连续发送多个请求
msh> ai_say 测试1
msh> ai_say 测试2
msh> ai_say 测试3

# 观察系统稳定性和响应时间
```

## 🎓 高级用法

### 1. 自定义测试脚本

创建测试序列：
```bash
# test_ai.sh
ai_say 你好
rt_thread_mdelay 3000
ai_say 测试完成
```

### 2. 监控日志

启用详细日志：
```c
// 在代码中设置
#define DBG_LVL DBG_DEBUG
```

### 3. 性能监控

```bash
# 查看线程状态
msh> list_thread

# 查看内存使用
msh> free
```

## 📈 测试结果记录

### 测试表格模板

| 时间 | 命令 | 响应时间 | 音频大小 | 结果 | 备注 |
|------|------|---------|---------|------|------|
| 14:30 | ai_say 你好 | 1.2s | 6.4KB | ✅ | 正常 |
| 14:31 | ai_say 测试长句... | 2.1s | 12.8KB | ✅ | 正常 |
| 14:32 | ai_say 特殊字符@# | 1.8s | 8.0KB | ⚠️ | 识别异常 |

## 💡 最佳实践

1. **先测试网络**：使用 `ping` 确认网络通畅
2. **短句测试**：先用简单短句测试，确认服务可用
3. **逐步增加复杂度**：从简单到复杂，逐步测试
4. **记录问题**：遇到问题记录完整的错误信息
5. **定期重启**：长时间运行后重启系统

## 🔗 相关文档

- **QUICK_START.md** - 快速开始指南
- **VOICE_ASSISTANT_README.md** - 完整功能说明
- **HOW_TO_USE_PACKAGES.md** - Package使用说明

## 📞 技术支持

如果遇到问题：
1. 查看本文档的故障排除部分
2. 检查API服务商的文档
3. 在RT-Thread论坛提问
4. 提交Issue到项目仓库

---

**快速测试命令：**
```bash
ai_say 你好      # 最简单的测试
ai_status       # 查看状态
ai_network      # 网络帮助
```

祝测试顺利！🎉

