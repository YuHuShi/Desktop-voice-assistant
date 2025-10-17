# HTTP代理使用指南

## 🎯 方案说明

使用HTTP代理服务器，将设备的HTTP请求转发为HTTPS请求到百度API。

```
┌──────────┐  HTTP   ┌──────────┐  HTTPS  ┌──────────┐
│  设备    │ ------> │ PC代理   │ ------> │ 百度API  │
│(RT-Thread)│         │ (Python) │         │  (云端)  │
└──────────┘         └──────────┘         └──────────┘
   低内存              TLS加密              文心一言
   HTTP连接            Python处理          V2协议
```

**优点：**
- ✅ 设备无需TLS支持
- ✅ 低内存占用
- ✅ 配置简单
- ✅ 易于调试

## 📋 完整配置步骤

### 步骤1：安装Python依赖

在你的Windows PC上打开PowerShell或CMD：

```bash
# 安装Python（如果还没有）
# 下载: https://www.python.org/downloads/

# 安装依赖库
pip install flask requests
```

### 步骤2：启动代理服务器

```bash
# 进入项目目录
cd D:\RT-ThreadStudio\workspace\Desktop-voice-assistant\art_pi2_wifi

# 运行代理
python xfyun_proxy.py
```

你会看到：

```
========================================================================
🚀 百度文心一言HTTP代理服务器已启动！
========================================================================

📍 本机IP地址: 192.168.1.100
📍 监听端口: 8080

🔗 设备访问地址:
   http://192.168.1.100:8080/v2/app/conversation

🔗 浏览器访问:
   http://192.168.1.100:8080/

💡 设备配置:
   修改 applications/ai_dialog_tool.c 中的 api_url 为:
   "http://192.168.1.100:8080/v2/app/conversation"

🧪 测试命令:
   curl http://192.168.1.100:8080/test

========================================================================
按 Ctrl+C 停止服务器

 * Serving Flask app 'xfyun_proxy'
 * Running on all addresses (0.0.0.0)
 * Running on http://127.0.0.1:8080
 * Running on http://192.168.1.100:8080
```

**记下显示的IP地址！**（例如：192.168.1.100）

### 步骤3：测试代理服务器

在浏览器访问：
```
http://192.168.1.100:8080/
```

或使用curl测试：
```bash
curl http://192.168.1.100:8080/test
```

应该返回：
```json
{
  "status": "ok",
  "message": "代理服务器工作正常"
}
```

### 步骤4：修改设备代码

编辑 `applications/ai_dialog_tool.c` 第66行：

```c
/* 将IP地址改为你的PC实际IP */
strncpy(chat_config.api_url, 
        "http://192.168.1.100:8080/v2/app/conversation",  /* 改这里！ */
        sizeof(chat_config.api_url) - 1);
```

**⚠️ 重要：** 将 `192.168.1.100` 改为你PC实际显示的IP地址！

### 步骤5：编译并下载

```bash
# 清理
scons -c

# 编译
scons -j8

# 使用RT-Thread Studio下载固件
```

### 步骤6：测试

在设备串口终端：

```bash
# 确保WiFi已连接
msh> ifconfig

# 测试代理连接
msh> ping 192.168.1.100

# 测试AI对话
msh> ai_ask 你是谁

# 预期输出：
========================================
[You] 你是谁
========================================
[I/ai.chat] Using Baidu Wenxin V2 protocol
AI is thinking...

[AI] 我是文心一言，百度研发的知识增强大语言模型...
========================================
```

## 🔍 故障排除

### 问题1：设备无法连接到代理

**检查1：PC和设备在同一网络？**
```bash
# 在设备上ping PC
msh> ping 192.168.1.100
```

如果ping不通：
- 检查WiFi是否连接
- 确认PC和设备在同一局域网
- 检查PC防火墙设置

**检查2：防火墙阻止？**

Windows防火墙设置：
1. 打开"Windows Defender 防火墙"
2. 点击"允许应用通过防火墙"
3. 找到"Python"，勾选"专用"和"公用"
4. 或暂时关闭防火墙测试

**检查3：端口被占用？**
```bash
# 检查8080端口
netstat -ano | findstr :8080
```

如果被占用，修改 `baidu_proxy.py` 的端口号。

### 问题2：代理返回错误

查看代理服务器终端输出，会显示详细错误信息：

```
📥 收到设备请求
请求数据: {...}
🔄 尝试API端点: https://...
📤 百度响应状态: 403
❌ API返回错误: 403
```

常见错误：
- **403**: 认证失败，检查Access Key
- **404**: API端点不对，会自动尝试其他端点
- **500**: 服务器错误，查看详细信息

### 问题3：设备报错

**错误：**
```
[E/web.client] Failed to resolve host: 192.168.1.100
```

**解决：** DNS问题，直接使用IP地址即可（已使用）。

**错误：**
```
[E/web.client] Failed to connect to server
```

**解决：** 
1. 确认代理服务器正在运行
2. 检查IP地址是否正确
3. 测试ping连接

### 问题4：响应解析失败

如果看到：
```
[W/ai.chat] Failed to parse Wenxin response
```

查看代理日志，找到实际响应格式，然后修改 `ai_chat_service.c` 的解析逻辑。

## 🎛️ 高级配置

### 修改端口

编辑 `baidu_proxy.py` 最后一行：

```python
app.run(
    host='0.0.0.0',
    port=8888,  # 改为其他端口
    debug=False
)
```

同时修改设备代码中的URL。

### 添加日志记录

代理已内置详细日志，可以看到：
- 设备请求内容
- 百度响应内容
- 错误信息

### 多设备支持

代理支持多个设备同时连接，无需修改。

### 使用https（可选）

如果PC和设备之间也需要加密：

```python
# 使用自签名证书
app.run(
    host='0.0.0.0',
    port=8443,
    ssl_context='adhoc'
)
```

需要安装：
```bash
pip install pyopenssl
```

## 📊 代理服务器功能

### 自动端点探测

代理会自动尝试多个可能的API端点：
1. `https://qianfan.baidubce.com/v2/app/conversation`
2. `https://qianfan.baidubce.com/v2/app/app-PrhUxYCW/conversation`
3. `https://aip.baidubce.com/rpc/2.0/ai_custom/v2/app/conversation`

找到可用的会自动使用。

### 详细日志

每次请求都会显示：
- 📥 设备请求数据
- 🔄 尝试的API端点
- 📤 百度响应状态
- ✅/❌ 成功或失败

### 错误处理

- 自动重试不同端点
- 详细错误信息返回给设备
- 支持调试

## 🚀 性能优化

### 缓存响应（可选）

如果想缓存相同问题的答案：

```python
from functools import lru_cache

@lru_cache(maxsize=100)
def cached_request(question):
    # 实现缓存逻辑
    pass
```

### 并发处理

Flask默认支持多线程，多设备无压力。

### 速率限制

如果担心API调用过快：

```python
from flask_limiter import Limiter

limiter = Limiter(app, default_limits=["100 per hour"])
```

## 💡 使用技巧

### 1. 开机自启动（Windows）

创建批处理文件 `start_proxy.bat`：

```batch
@echo off
cd /d D:\RT-ThreadStudio\workspace\Desktop-voice-assistant\art_pi2_wifi
python xfyun_proxy.py
pause
```

放到启动文件夹：
`C:\Users\你的用户名\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup`

### 2. 后台运行

使用 `pythonw` 代替 `python`：
```bash
pythonw baidu_proxy.py
```

### 3. 查看实时日志

代理会在终端显示所有请求详情，方便调试。

### 4. 远程访问

如果想从外网访问：
1. 配置路由器端口转发
2. 使用DDNS
3. 修改设备URL为公网IP

但**不推荐**，有安全风险！

## 📱 移动调试

可以用手机测试代理：

```bash
# 手机浏览器访问
http://192.168.1.100:8080/

# 使用手机APP测试API
POST http://192.168.1.100:8080/v2/app/conversation
{
  "messages": [
    {"role": "user", "content": "你好"}
  ],
  "stream": false
}
```

## 🎉 成功的标志

当一切正确时，你会看到：

**代理终端：**
```
📥 收到设备请求
请求数据: {"messages": [{"role": "user", "content": "你是谁"}], ...}
🔄 尝试API端点: https://qianfan.baidubce.com/v2/app/conversation
📤 百度响应状态: 200
✅ 请求成功！
```

**设备终端：**
```
[You] 你是谁
========================================
AI is thinking...

[AI] 我是文心一言，百度研发的知识增强大语言模型...
========================================
Converting to speech...
Playing audio reply...
Done!
```

## 🔄 停止服务器

在代理终端按 `Ctrl+C` 即可停止。

## 📞 获取帮助

如果遇到问题：

1. **查看代理日志** - 最详细的错误信息
2. **查看设备日志** - HTTP请求状态
3. **网络测试** - ping, telnet 等
4. **防火墙检查** - 临时关闭测试

---

## 🎯 快速开始清单

- [ ] 安装Python和依赖（pip install flask requests）
- [ ] 运行代理（python xfyun_proxy.py）
- [ ] 记下IP地址（如 192.168.1.100）
- [ ] 修改设备代码中的IP地址
- [ ] 编译并下载固件（scons -j8）
- [ ] 测试连接（msh> ping 192.168.1.100）
- [ ] 测试对话（msh> ai_ask 你是谁）
- [ ] 享受AI对话！🎉

---

**现在就开始吧！运行 `python xfyun_proxy.py` 启动代理服务器！** 🚀

