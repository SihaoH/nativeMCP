# nativeMCP

## 概述
这是一个由C++编写的MCP系统，包括MCP核心架构的host、client和server；而host本身已经相当于是AI Agent，虽然目前只有命令行（但我觉得也不需要花哨界面）

参考：https://modelcontextprotocol.io/introduction

*server可用于其他支持MCP的软件，如Cursor*

*client也支持MCP官方的servers，和Cursor同样的配置即可*

## 基本组成
- MCPServer：遵循MCP的Server父类，相当于SDK，子类只需要继承以及按要求新增所需的方法即可；目前通信方式只支持stdio
- servers：继承MCPServer后实现的一些具体server，可以直接使用；后续分支再更新更多的工具
- host：宿主应用程序，连接LLM，根据配置加载MCPClient
	- MCPClient：在host应用程序内部与服务器保持1:1连接，在LLM需要的时候调用MCPServer提供的工具
	- ModelAdapter：连接LLM的适配层，目前实现是使用cpp-httplib对Ollama发起请求

## 依赖库&外部程序
- **Qt6.8**：字符串、Json处理，元对象机制（用于动态调用任意函数）
- **cpp-httplib**：用于请求Ollama或是其他在线大模型的api
- **Ollama**：本地部署大模型的后端程序
	- LLM 任意的大语言模型，用于自然语言的问答，例如DeepSeek、Qwen等

## 编译环境
### 编译工具链
- CMake >= 3.30
- Visual Studio 2022（勾选`使用C++进行桌面开发`）

### vcpkg
用于导入第三方库cpp-httplib、spdlog
```
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg && bootstrap-vcpkg.bat
```
然后将vcpkg.exe的路径添加到系统环境变量PATH里

### Qt6.8.2
可以直接使用在线安装包，不过还是建议自己下载源码来构建。目前只需编译qt-base模块即可。

#### 参考
https://doc.qt.io/qt-6/getting-sources-from-git.html

https://doc.qt.io/qt-6/windows-building.html

https://doc.qt.io/qt-6/configure-options.html

#### 下载源码
`git clone --branch v6.8.2 git://code.qt.io/qt/qt5.git .\src`

#### 生成
```
mkdir build
cd build
..\src\configure.bat -init-submodules -submodules qtbase
..\src\configure.bat -debug-and-release -prefix <path-to-qt>
cmake --build . --parallel
cmake --install .
cmake --install . --config debug
```
*上述步骤有两次install，第一次默认是只安装release*
最后将`<path-to-qt>\bin`添加到系统环境变量PATH中里

## 构建
clone源码并进入到源码目录下
```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
```

最后打开build/nativeMCP.sln进行编译和调试

## 配置
修改[config.json](./host/config.json)，可简单配置MCP Host的功能
- model
	- host：Ollama的地址
	- chat：生成对话的模型，推荐deepseek或qwen
- mcpServers
	- server名称（因为代码会判断冒号，所以不要使用冒号）
		- command：在windows系统上必须都是`cmd`
		- args：["/c"]第一个参数必须是`/c`，第二个参数看server的类型选择，exe直接是`exe的路径`，python程序则填`python`，Node.js则填`npx`，后面的参数再根据server的说明来填写

## 示例
```text
[2025-03-26 12:27:30.296][info][Host.cpp::18] MCP Host初始化
[2025-03-26 12:27:30.299][info][ModelAdapter.cpp::22] ModelAdapter初始化：http://localhost:11434, qwen2.5:7b
[2025-03-26 12:27:30.397][info][Host.cpp::67]
可用工具列表：
cpp-time:
  getCurrentTime 获取当前时间
  waitTime 等待指定的时间
server-test:
  getAvailableIP 获取可用的IP列表
  sendToIP 将内容发送到指定的ip地址
  testMultiParams 测试多参数的工具调用

>>> 获取当前时间，并发送给所有可用的ip
[2025-03-26 12:29:08.349][info][Host.cpp::123] 调用MCP工具[cpp-time::getCurrentTime]: {
    "datetime": "2025-03-26T12:29:08.346",
    "timezone": "Asia/Shanghai"
}

[2025-03-26 12:29:20.160][info][Host.cpp::123] 调用MCP工具[server-test::getAvailableIP]: {
    "ip_list": [
        "192.168.1.201",
        "192.168.1.202",
        "192.168.1.203"
    ]
}

[2025-03-26 12:30:07.784][info][Host.cpp::123] 调用MCP工具[server-test::sendToIP]: {
    "content": "当前时间为：2025-03-26T12:29:08.346 (Asia/Shanghai)",
    "ip": "192.168.1.201",
    "status": "发送成功"
}

[2025-03-26 12:30:07.788][info][Host.cpp::123] 调用MCP工具[server-test::sendToIP]: {
    "content": "当前时间为：2025-03-26T12:29:08.346 (Asia/Shanghai)",
    "ip": "192.168.1.202",
    "status": "发送成功"
}

[2025-03-26 12:30:07.789][info][Host.cpp::123] 调用MCP工具[server-test::sendToIP]: {
    "content": "当前时间为：2025-03-26T12:29:08.346 (Asia/Shanghai)",
    "ip": "192.168.1.203",
    "status": "发送成功"
}

当前时间为：2025-03-26T12:29:08.346 (Asia/Shanghai)

已经将当前时间发送给所有可用的IP地址，发送状态均为成功。

```