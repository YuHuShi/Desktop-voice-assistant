# AI智能对话功能指南

## 🎯 功能说明

现在你可以和AI进行**真正的对话**了！

### 之前（仅TTS）
```
msh> ai_say 你好世界
→ AI播放"你好世界"（原样复述）
```

### 现在（智能对话）
```
msh> ai_ask 你是谁？
→ AI回复："我是小石，一个友好的智能助手"
→ 并播放语音
```

## 🚀 快速开始

### 步骤1：配置API密钥

编辑 `ai_dialog_tool.c`，找到这行：

```c
strncpy(chat_config.api_key, "your_openai_api_key", ...);
```

替换为你的API Key。

### 步骤2：选择AI服务

#### 选项A：OpenAI ChatGPT（推荐）

```c
chat_config.provider = AI_CHAT_OPENAI;
strncpy(chat_config.api_key, "sk-xxxxxxxxxxxx", ...);
strncpy(chat_config.model, "gpt-3.5-turbo", ...);
strncpy(chat_config.api_url, "https://api.openai.com/v1/chat/completions", ...);
```

**获取API Key：**
1. 访问 https://platform.openai.com/
2. 注册并创建API Key
3. 复制Key到代码中

#### 选项B：百度文心一言

```c
chat_config.provider = AI_CHAT_BAIDU_WENXIN;
// 配置百度API密钥
```

#### 选项C：讯飞星火

```c
chat_config.provider = AI_CHAT_XFYUN_XINGHUO;
// 配置讯飞API密钥
```

### 步骤3：编译运行

```bash
scons -j8
# 下载到开发板
```

### 步骤4：使用

```bash
# 完整对话（有语音回复）
msh> ai_ask 你是谁？

# 输出：
[You] 你是谁？
========================================
AI is thinking...

[AI] 我是小石，一个友好的智能助手，很高兴为您服务！
========================================
Converting to speech...
Playing audio reply...
Done!
```

## 📝 可用命令

| 命令 | 功能 | 示例 |
|------|------|------|
| **`ai_ask <问题>`** | 🎯 智能对话+语音回复 | `ai_ask 你是谁` |
| `ai_text <问题>` | 文本对话（不播放语音）| `ai_text 今天天气` |
| `ai_dialog_init` | 初始化对话工具 | `ai_dialog_init` |

## 🎭 对话示例

### 示例1：自我介绍

```bash
msh> ai_ask 你是谁？

[You] 你是谁？
[AI] 我是小石，一个友好的智能助手，很高兴为您服务！
（播放语音）
```

### 示例2：天气查询

```bash
msh> ai_ask 今天天气怎么样？

[You] 今天天气怎么样？
[AI] 抱歉，我无法获取实时天气信息。建议您查看天气应用或网站。
（播放语音）
```

### 示例3：知识问答

```bash
msh> ai_ask 什么是物联网？

[You] 什么是物联网？
[AI] 物联网是指通过互联网连接各种设备，实现设备间通信和数据交换的技术。
（播放语音）
```

### 示例4：闲聊

```bash
msh> ai_ask 讲个笑话

[You] 讲个笑话
[AI] 为什么程序员总是分不清万圣节和圣诞节？因为Oct 31 == Dec 25！
（播放语音）
```

## ⚙️ 自定义AI角色

编辑系统提示词来定制AI的行为：

```c
// 在 ai_dialog_tool.c 中修改
strncpy(chat_config.system_prompt, 
        "你是小石，一个友好的智能助手。"
        "请用简短的中文回答问题，不超过50字。"
        "你的回答要简洁、友好、有帮助。",
        sizeof(chat_config.system_prompt) - 1);
```

**示例角色：**

### 技术助手
```c
"你是一个专业的技术助手，擅长解答编程和物联网问题。"
```

### 生活管家
```c
"你是一个生活助手，帮助用户安排日程、提醒事项、回答生活问题。"
```

### 儿童伙伴
```c
"你是一个儿童AI伙伴，用简单有趣的语言和小朋友交流。"
```

## 🔧 工作流程

```
你的问题
    ↓
对话AI理解 (ChatGPT/文心一言)
    ↓
生成回复文本
    ↓
文本转语音 (TTS)
    ↓
播放语音
    ↓
完成！
```

## 📊 对比

| 功能 | ai_say | ai_ask |
|------|--------|--------|
| **输入** | 文本 | 问题 |
| **AI处理** | 无 | ✅ 智能理解 |
| **输出** | 原样播放 | 智能回复+播放 |
| **适用场景** | 播报信息 | 对话交互 |

**示例对比：**

```bash
# ai_say - 简单播放
msh> ai_say 你是谁
→ 播放："你是谁"（原文）

# ai_ask - 智能对话
msh> ai_ask 你是谁
→ AI理解问题
→ 生成回复："我是小石..."
→ 播放回复
```

## 🌐 API服务选择

### OpenAI ChatGPT
- ✅ 效果最好
- ✅ 回复质量高
- ❌ 需要国际网络
- ❌ 需要付费（但便宜）

**费用：** 约$0.002/次对话

### 百度文心一言
- ✅ 国内可用
- ✅ 中文效果好
- ✅ 有免费额度
- ⚠️ 需要申请

**费用：** 有免费额度

### 讯飞星火
- ✅ 国内可用
- ✅ 语音相关功能好
- ✅ 有免费额度

## ⚠️ 注意事项

### 1. API密钥安全

不要将API密钥提交到公开仓库！

### 2. 网络要求

- OpenAI: 需要能访问国际网络
- 国内服务: 普通网络即可

### 3. 费用控制

设置使用限额，避免意外费用：
- OpenAI: 在网站设置月度限额
- 国内服务: 查看各平台的免费额度

### 4. 回复长度

系统提示词中限制回复长度（如50字），因为：
- 播放时间更短
- 节省流量和费用
- 更适合语音交互

## 🐛 故障排除

### 问题1：API请求失败

```
[Error] AI chat failed
Error: HTTP request failed
```

**解决：**
1. 检查网络连接
2. 检查API Key是否正确
3. 检查API余额

### 问题2：无法访问OpenAI

**解决：**
- 使用国内AI服务（文心一言、星火）
- 或使用OpenAI代理服务

### 问题3：回复太长导致内存不足

**解决：**
在系统提示词中限制回复长度：
```c
"回答要简短，不超过30字"
```

## 🎯 最佳实践

### 1. 简短提问

```bash
✅ Good: ai_ask 你是谁
❌ Bad:  ai_ask 能不能详细介绍一下你自己包括你的功能特点优势...
```

### 2. 合理的系统提示词

```c
✅ "回答简短，不超过50字"
❌ "回答要详细，包含所有细节"
```

### 3. 控制对话次数

不要频繁调用，避免费用过高。

## 📚 完整示例

### 配置文件

```c
// ai_dialog_tool.c

ai_chat_config_t chat_config = {
    .provider = AI_CHAT_OPENAI,
    .api_key = "sk-your-key-here",
    .model = "gpt-3.5-turbo",
    .api_url = "https://api.openai.com/v1/chat/completions",
    .system_prompt = "你是小石，一个友好的智能助手。用简短的中文回答，不超过50字。"
};
```

### 使用流程

```bash
# 1. 连接WiFi
msh> wifi join YourWiFi Password

# 2. 测试对话
msh> ai_ask 你是谁

# 3. 各种问题
msh> ai_ask 什么是RT-Thread
msh> ai_ask 今天星期几
msh> ai_ask 讲个笑话
```

## 🚀 下一步

1. **配置API密钥**（必须）
2. **重新编译**
3. **测试对话功能**
4. **自定义AI角色**

---

**现在你有了真正的智能对话AI助手！** 🎉

问它任何问题，它都能理解并智能回复！

