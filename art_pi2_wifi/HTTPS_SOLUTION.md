# HTTPS支持解决方案

## 🔴 当前问题

百度文心一言V2 API使用HTTPS，但项目未启用TLS支持。

错误信息：
```
[E/web.client] HTTPS not supported yet
[E/web.client] Failed to parse URL: https://qianfan.baidubce.com/v2/app/conversation
```

## ✅ 临时方案（已应用）

我已修改 `web_client.c`，允许解析HTTPS URL并连接443端口。

**注意：** 这是不安全的临时方案！仅用于测试API格式是否正确。

### 现在请测试：

```bash
# 编译
scons -j8

# 下载到开发板后测试
msh> ai_ask 你是谁
```

### 可能的结果：

#### 结果1：连接被拒绝
```
[E/web.client] Failed to connect to server
```
**原因：** 443端口需要TLS握手，普通socket无法连接。

#### 结果2：连接成功但返回错误
```
HTTP status: 400/403/...
```
**原因：** 服务器要求HTTPS加密。

#### 结果3：奇迹般成功！
```
[AI] 我是文心一言...
```
**原因：** 服务器允许非加密连接（不太可能）。

## 🎯 完整解决方案

### 方案A：启用TLS支持（推荐）

#### 步骤1：启用TLS配置

编辑 `rtconfig.h`，添加：

```c
#define RT_USING_SAL
#define SAL_USING_TLS
#define SAL_USING_POSIX
#define RT_USING_MBEDTLS
```

或使用menuconfig：

```bash
menuconfig
# 进入配置菜单
# RT-Thread Components → Network → Socket abstraction layer
# 选中 [*] Enable TLS for socket
# 保存退出
```

#### 步骤2：检查mbedtls包

```bash
# 查看是否有mbedtls包
ls packages/mbedtls*

# 如果没有，需要添加
```

#### 步骤3：重新编译

```bash
scons --target=mdk5  # 或其他目标
scons -j8
```

#### 缺点：
- **内存占用大**（TLS需要30-50KB额外RAM）
- **可能导致内存不足**（你之前已经有内存问题）
- **Flash占用增加**（100-200KB）

### 方案B：使用HTTP替代服务

寻找支持HTTP的AI服务：

1. **自建AI代理服务器**
   - 在PC/云服务器上搭建HTTP代理
   - 代理将HTTP请求转发到HTTPS API
   - 设备只需连接HTTP代理

2. **使用其他AI服务**
   - 查找支持HTTP的AI API
   - 或使用本地AI模型

### 方案C：简化的HTTPS实现（中等复杂度）

实现最小化TLS支持：
- 仅支持客户端模式
- 跳过证书验证（不安全但可用）
- 使用轻量级TLS库（如tinydtls）

## 🚀 推荐方案：使用HTTP代理

这是最practical的方案！

### 在PC上运行HTTP代理

创建文件 `xfyun_proxy.py`：

```python
from flask import Flask, request, jsonify
import requests

app = Flask(__name__)

BAIDU_API_URL = "https://qianfan.baidubce.com/v2/app/conversation"
BAIDU_ACCESS_KEY = "1497dd3e12eb41e9be2936a77f46795e"

@app.route('/v2/app/conversation', methods=['POST'])
def proxy():
    try:
        # 获取设备发送的数据
        data = request.get_json()
        
        # 转发到百度API（HTTPS）
        headers = {
            'Content-Type': 'application/json',
            'X-Appbuilder-Authorization': f'Bearer {BAIDU_ACCESS_KEY}'
        }
        
        response = requests.post(BAIDU_API_URL, json=data, headers=headers)
        
        # 返回结果给设备
        return jsonify(response.json()), response.status_code
    
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    # 在局域网中运行，允许设备访问
    app.run(host='0.0.0.0', port=8080)
```

运行代理：

```bash
pip install flask requests
python xfyun_proxy.py
```

### 修改设备配置

编辑 `applications/ai_dialog_tool.c`：

```c
/* 将HTTPS URL改为本地HTTP代理 */
chat_config.use_v2 = RT_FALSE;  /* 不使用V2协议的Header，让代理处理 */

/* 使用PC的IP地址 */
strncpy(chat_config.api_url, 
        "http://192.168.1.100:8080/v2/app/conversation",  // 改为你PC的IP
        sizeof(chat_config.api_url) - 1);
```

## 💡 方案D：等待百度HTTP支持

询问百度是否提供HTTP端点：

1. 访问控制台API文档
2. 查找是否有HTTP选项
3. 提交工单询问

## 📊 方案对比

| 方案 | 难度 | 内存占用 | 安全性 | 推荐度 |
|------|------|---------|--------|--------|
| 启用TLS | ⭐⭐⭐ | 😱高 | ✅安全 | ⭐⭐ |
| HTTP代理 | ⭐⭐ | ✅低 | ⚠️中等 | ⭐⭐⭐⭐⭐ |
| 当前临时方案 | ⭐ | ✅低 | ❌不安全 | ⭐（仅测试）|
| 自建服务 | ⭐⭐⭐⭐ | ✅低 | ✅可控 | ⭐⭐⭐ |

## 🎯 立即行动

### 选项1：测试当前临时方案

```bash
scons -j8
# 下载测试，看看错误信息
```

### 选项2：使用HTTP代理（最快）

1. 在PC上运行上面的Python代理脚本
2. 修改设备的API URL为代理地址
3. 编译测试

### 选项3：启用TLS（完整方案）

需要配置Kconfig，可能会遇到内存问题。

## 🔧 需要我做什么？

请告诉我你的选择：

1. **先测试临时方案** - 看看具体错误是什么
2. **使用HTTP代理** - 我帮你准备完整的代理脚本
3. **启用TLS支持** - 我帮你配置mbedtls（可能有内存问题）
4. **切换到旧版V1** - 使用access_token方式（仍需HTTPS）

---

**建议：先测试临时方案，看错误信息，然后再决定下一步！**

