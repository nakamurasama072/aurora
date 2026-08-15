# Aurora Drive

Aurora Drive 是一个正在开发中的轻量级个人云存储服务。其 C++ 后端提供 REST API，用于浏览已配置的文件系统根目录并报告所在文件系统的存储容量。Angular Web 客户端通过开发代理使用这些接口。

## 其他语言

- [English (US)](../README.md)
- [繁體中文](README-zh_TW.md)
- [日本語](README-ja_JP.md)

## 当前功能

- 浏览存储根目录或其子目录中的条目。
- 返回条目的名称、识别出的类型、大小、修改时间和链接元数据。
- 目录优先，再按名称排序目录列表。
- 阻止解析后位于存储根目录之外的文件系统路径（路径遍历攻击）。
- 报告存储根目录所在文件系统的总容量和可用容量。
- 提供带有文件浏览、存储用量、语言选择以及明暗主题的 Angular 界面。

## 当前限制

Aurora Drive 目前处于 alpha 阶段。后端尚未提供认证、上传、下载、创建目录、删除或移动文件、共享，以及持久化收藏和回收站的 API。因此，Web 客户端中相应的若干控件目前仅为展示功能或本地状态功能。

存储根目录目前为进程工作目录（`./`），由 `include/config.h` 中的 `kNetDiskRoot` 配置。请从需要暴露的目录启动服务器，并且不要在不受信任的网络上暴露包含敏感数据的目录。

## 未来计划

|名称|预计版本|
|--|--|
|初始自定义与默认配置|0.2.0|
|进入子目录浏览（真的能用）|0.2.0|
|创建、重命名和删除目录及文件|0.3.0|
|身份认证|0.4.0|
|链接分享|0.5.0|
|加速、可恢复的文件上传与下载|0.6.0|
|...|...|

## 具体要求

### 部署

Aurora Drive 可以不使用前端部署，但最低要求会有所不同。

仅后端：
- 待确定

全栈（即前端 + 后端）：
- 待确定

### 构建

- Linux（内核版本 >= 6.1）
- 支持 C++17 的编译器
- CMake 3.20 或更高版本
- 捆绑的 [Crow](https://crowcpp.org/) 头文件所需的构建依赖，包括 Boost
- Node.js 与 npm，仅在运行 Angular Web 客户端时需要

请放心，Angular 将在每个发布构建中更新至最新版本。

## 部署服务器

## 运行后端

在仓库根目录配置并构建 C++ 服务器：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

从选定的存储目录启动它。例如，在开发时使用构建目录作为存储的根目录：

```bash
./build/aurora
```

服务器监听 `http://localhost:12384`，并使用 Crow 的多线程服务器模式。

## 运行 Web 客户端

请在另一个终端中先启动后端，然后安装并启动 Angular 应用：

```bash
cd frontend/aurora-drive
npm install
npm start
```

打开 `http://localhost:4200`。Angular 开发服务器会通过 `proxy.conf.json` 将 `/api` 下的请求转发到 `http://localhost:12384`。

构建生产版本的客户端：

```bash
cd frontend/aurora-drive
npm run build
```

## 关于发布版本

每个正式版本和预发布版本（beta、rcN）只包含预构建的后端二进制包，命名格式如下：
- `aurora_PLATFORM_ARCH_VERSION`

其中：
- `PLATFORM`：C 标准库实现，主要为 glibc 和 musl-libc.
- `ARCH`：宿主机使用的架构，例如 i386、x86-64、aarch64、risc-v.
- `VERSION`：遵循 X.Y.Z（SemVer2 格式）版本号。对于预发布版本，将直接添加修饰后缀（`-beta`、`-rcN`）。

Docker 和 Podman 镜像正在规划中，预计自 1.0.0 起提供。

## REST API 接口文档

免责声明：本节中的所有用法仅为文档说明需要，请根据实际情况进行调整。

### `GET /api/files`

列出存储根目录中的条目。可选参数为相对路径 `path`，是查询参数，作用是列出子目录。

```bash
curl 'http://localhost:12384/api/files'
curl 'http://localhost:12384/api/files?path=docs'
```

一个 response 的完整结构如下：

```json
{
	"success": true,
	"path": "docs",
	"message": "Fetch success",
	"entries": [
		{
			"name": "README-zh_CN.md",
			"type": "Markdown File",
			"size": 0,
			"last_modified": "2026-08-15 12:00:00",
			"directory": false,
			"symlink": false,
			"hard_link": false
		}
	]
}
```

解析后的路径若超出存储根目录，将返回 `403 Forbidden` 和 `success: false`。不存在的路径或非目录路径目前仅会返回一个空的 `entries` 数组。

### `GET /api/system/storage`

返回存储根目录所在文件系统的字节数：

```bash
curl 'http://localhost:12384/api/system/storage'
```

```json
{
	"success": true,
	"total": 1000000000000,
	"available": 750000000000,
	"message": "Fetch success"
}
```

## 项目结构

```text
docs/                   本地化 README 文件
frontend/aurora-drive/  Angular Web 客户端
src/                    C++ 服务器入口
include/                REST API、文件系统和系统工具
include/external/crow   捆绑的 Crow 头文件
```

## 致谢

Aurora Drive 的诞生离不开下面的开源项目：
- [Crow](https://github.com/crowcpp/crow)
- [Angular](https://angular.dev/)
- [copyparty](https://github.com/9001/copyparty)（本项目的直接灵感来源）

同时，衷心感谢 JetBrains 为开源项目免费提供 CLion.

## 许可证

Aurora Drive 使用 [GNU General Public License v3.0](../LICENSE) 许可证。
