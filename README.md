# nativeRAG

## 概述
这是一个由C++编写的MCP系统，包括MCP核心架构的host、client和server

*server可用于其他支持MCP的软件，如Cursor*
*client也支持MCP官方的servers，和Cursor同样的配置即可*

## 基本组成
- **Qt6.8**：字符串、Json处理，元对象机制（用于动态调用任意函数）
- **cpp-httplib**：用于请求Ollama
- **Ollama**：本地部署大模型的后端程序
	- LLM 任意的大语言模型，用于自然语言的问答，例如DeepSeek、Qwen等

## 编译依赖
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
修改[config.json](./host/config.json)，可简单配置RAG服务端的功能
- model
	- host：ollama的地址
	- chat：生成对话的模型，推荐deepseek和qwen
- mcpServers
	- server名称，可以随便填
		- command：在windows系统上必须都是`cmd`
		- args：["/c"]第一个参数必须是`/c`，第二个参数看server的类型选择，exe直接是`exe的路径`，python程序则填`python`，Node.js则填`npx`，后面的参数再根据server的说明来填写