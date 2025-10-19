# RT-Thread 智能语音助手

基于 ART-Pi2 开发板和讯飞星火大模型的智能语音助手项目。

[![RT-Thread](https://img.shields.io/badge/RT--Thread-5.1.0-blue)](https://www.rt-thread.org/)
[![XFyun Spark](https://img.shields.io/badge/XFyun-Spark%20Ultra--32K-orange)](https://www.xfyun.cn/)
[![License](https://img.shields.io/badge/License-Apache--2.0-green)](LICENSE)

## ✨ 项目特性

- ✅ **WiFi 网络连接** - 支持 2.4GHz WiFi 热点连接
- ✅ **AI 对话功能** - 集成讯飞星火 Ultra-32K 大模型
- ✅ **HTTP 代理方案** - 解决嵌入式设备 HTTPS 访问问题
- ✅ **中文完美支持** - Unicode 自动解码，支持中文对话
- ✅ **内存优化** - 稳定的内存管理，无内存泄漏
- ✅ **简洁回复** - AI 回复只包含中英文和标点符号，无表情符号
- ✅ **易于部署** - 完整文档和部署指南

## 📋 目录

- [快速开始](#快速开始)
- [硬件要求](#硬件要求)
- [功能说明](#功能说明)
- [使用方法](#使用方法)
- [项目结构](#项目结构)
- [常见问题](#常见问题)
- [更新日志](#更新日志)
- [参与贡献](#参与贡献)

## 🚀 快速开始

### 1. 准备工作

**硬件：**
- ART-Pi2 开发板（STM32H7R7）
- WiFi 模块（板载）
- USB 线缆（供电和调试）

**软件：**
- Python 3.6+ （运行代理服务器）
- RT-Thread Studio 或 Keil MDK（编译固件）

### 2. 配置讯飞星火 API

1. 访问 [讯飞开放平台](https://www.xfyun.cn/)
2. 注册并创建应用
3. 获取 APIPassword（在"HTTP服务接口认证信息"中）
4. 编辑 `xfyun_proxy.py`，填入你的 APIPassword：

```python
XFYUN_API_PASSWORD = "你的APIPassword"
```

**详细步骤请查看：[XFYUN_SPARK_CONFIG.md](XFYUN_SPARK_CONFIG.md)**

### 3. 启动代理服务器

```bash
# 安装依赖
pip install flask requests

# 启动代理
cd art_pi2_wifi
python xfyun_proxy.py
```

代理服务器会显示本机 IP 地址，记下这个地址。

### 4. 配置网络

**方案 A：使用 PC 热点（推荐）**

1. 在 PC 上创建移动热点（2.4GHz）
2. 开发板会自动连接到 `192.168.137.1:8080`

**方案 B：使用路由器**

1. 修改 `applications/ai_dialog_tool.c` 第 56 行的 IP 地址为你的 PC IP
2. 重新编译固件

### 5. 编译和下载固件

```bash
# 更新软件包
pkgs --update

# 编译固件
scons -j8

# 下载到开发板（使用 RT-Thread Studio 或 ST-Link）
```

### 6. 测试 AI 对话

```bash
msh> wifi join 你的WiFi名 密码
msh> ai_ask 你是谁
```

**🎉 完成！你已经成功运行了智能语音助手！**

详细步骤请查看：**[快速启动.md](快速启动.md)**

## 🔧 硬件要求

### 开发板规格

- **主控芯片**: STM32H7R7VIT6
- **CPU**: ARM Cortex-M7 @ 600MHz
- **RAM**: 128KB SRAM (预留) + 64MB PSRAM
- **Flash**: 2MB 内部 Flash + 64MB 外部 Flash
- **WiFi**: CYW43438 2.4GHz WiFi 模块
- **USB**: USB OTG、USB Type-C

### 最低配置

- RAM 使用: ~50KB
- Flash 使用: ~500KB
- WiFi: 仅支持 2.4GHz

## 💡 功能说明

### 已完成功能

#### 1. WiFi 连接 ✅
- 支持扫描 WiFi 网络
- 连接 2.4GHz WiFi 热点
- 自动获取 IP 地址（DHCP）

```bash
msh> wifi scan                    # 扫描网络
msh> wifi join SSID PASSWORD      # 连接 WiFi
msh> ifconfig                     # 查看 IP
```

#### 2. AI 对话功能 ✅
- 集成讯飞星火 Ultra-32K 大模型
- 支持中英文对话
- 简洁回复（无表情符号）
- 稳定的内存管理

```bash
msh> ai_ask 你是谁               # AI 对话（带提示）
msh> ai_text 今天天气怎么样      # AI 对话（纯文本）
```

#### 3. HTTP 代理服务器 ✅
- 将 HTTP 请求转换为 HTTPS
- 支持多设备连接
- 实时日志显示
- 跨平台运行（Windows/Linux/macOS）

#### 4. 中文支持 ✅
- Unicode 自动解码
- 完美显示中文回复
- 支持中英文混合输入

### 计划中功能

#### 语音识别（ASR）⏳
- 本地或云端语音识别
- 支持实时识别

#### 语音合成（TTS）⏳
- 文字转语音输出
- 多种音色选择

#### 语音唤醒⏳
- 本地唤醒词检测
- 低功耗待机

#### 多轮对话⏳
- 上下文记忆
- 连续对话

## 📖 使用方法

### AI 对话命令

#### ai_ask - AI 对话
```bash
msh> ai_ask 你是谁
[You] 你是谁
========================================
AI is thinking...

[AI] 我是小石，一个友好的智能助手。
========================================
```

#### ai_text - 纯文本对话
```bash
msh> ai_text 什么是 RT-Thread
[You] 什么是 RT-Thread
AI is thinking...
[AI] RT-Thread 是一个开源的实时操作系统...
```

### WiFi 命令

```bash
# 扫描网络
msh> wifi scan

# 连接网络
msh> wifi join SSID PASSWORD

# 查看状态
msh> wifi status

# 断开连接
msh> wifi disconnect

# 查看 IP
msh> ifconfig
```

### 网络测试

```bash
# Ping 测试
msh> ping www.baidu.com

# DNS 查询
msh> dns www.baidu.com
```

## 📁 项目结构

```
art_pi2_wifi/
├── applications/              # 应用代码
│   ├── ai_chat_service.c      # AI 对话服务
│   ├── ai_chat_service.h
│   ├── ai_dialog_tool.c       # AI 对话工具（命令行）
│   ├── main.c                 # 主程序
│   └── ...
│
├── xfyun_proxy.py            # 讯飞星火 HTTP 代理服务器 ⭐
│
├── 文档/
│   ├── 项目说明.md            # 项目总览 ⭐
│   ├── 快速启动.md            # 快速上手指南 ⭐
│   ├── XFYUN_SPARK_CONFIG.md # 讯飞星火配置详解 ⭐
│   ├── HTTP_PROXY_GUIDE.md   # HTTP 代理完整指南
│   └── ...
│
├── board/                    # 板级支持包
├── libraries/                # 驱动库
├── packages/                 # 第三方软件包
├── rt-thread/                # RT-Thread 内核
│
├── .gitignore               # Git 忽略配置
├── README.md                # 本文件
├── rtconfig.h               # RT-Thread 配置
└── SConstruct               # SCons 构建脚本
```

## ❓ 常见问题

### 1. WiFi 连接失败？

**解决方法：**
- 确认 WiFi 是 **2.4GHz** 频段（不支持 5GHz）
- 检查密码是否正确
- 尝试使用 PC 热点
- 查看详细错误：`wifi status`

### 2. AI 回复是乱码？

**解决方法：**
- 已在代码中实现 Unicode 解码
- 确保使用最新固件
- 重新编译并下载固件

### 3. 代理服务器连接失败？

**解决方法：**
- 确认 PC 和开发板在同一网络
- 检查防火墙设置（允许 8080 端口）
- 确认代理服务器已启动
- ping 测试 PC 地址

### 4. 内存不足或系统崩溃？

**解决方法：**
- 已修复内存泄漏问题
- 使用最新版本代码
- 检查堆大小配置

### 5. 不想用 PC 代理服务器？

**解决方案：**
- **云服务器部署**（推荐）：部署到 VPS，约 ￥10/月
- **树莓派部署**：局域网内 24 小时运行
- **移植 mbedTLS**：实现设备端 HTTPS（难度高）

详见：[项目说明.md - 不用代理的解决方案](项目说明.md#-不用代理的解决方案)

## 📊 性能指标

| 指标 | 数值 |
|------|------|
| RAM 使用 | ~50KB |
| Flash 使用 | ~500KB |
| AI 响应时间 | 1-3秒 |
| WiFi 速度 | ~13Mbps |
| 对话稳定性 | ✅ 稳定 |

## 📚 详细文档

- **[项目说明.md](项目说明.md)** - 完整项目介绍和架构说明
- **[快速启动.md](快速启动.md)** - 3 步快速开始
- **[XFYUN_SPARK_CONFIG.md](XFYUN_SPARK_CONFIG.md)** - 讯飞星火详细配置
- **[HTTP_PROXY_GUIDE.md](HTTP_PROXY_GUIDE.md)** - 代理服务器使用指南
- **[HTTPS_SOLUTION.md](HTTPS_SOLUTION.md)** - HTTPS 解决方案对比

## 🔄 更新日志

### v1.0.0 (2025-10-19)
- ✅ 集成讯飞星火 Ultra-32K 大模型
- ✅ 实现 HTTP 代理方案
- ✅ 完善 Unicode 中文解码
- ✅ 优化内存管理，修复内存泄漏
- ✅ 系统提示词优化（无表情符号）
- ✅ 完整的文档体系
- ✅ 项目代码重构和整理

## 🤝 参与贡献

欢迎提交 Issue 和 Pull Request！

### 贡献指南

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 提交 Pull Request

## 📄 开源协议

本项目基于 Apache-2.0 协议开源。

## 🙏 致谢

- [RT-Thread](https://www.rt-thread.org/) - 实时操作系统
- [讯飞开放平台](https://www.xfyun.cn/) - AI 服务支持
- ART-Pi2 开发板社区

## 📞 联系方式

- 提交 Issue：[GitHub Issues](../../issues)
- 参考文档：查看 `docs/` 目录

---

**🎉 开始你的智能语音助手之旅吧！**

如有问题，请查看详细文档或提交 Issue。
