# Aurora Drive

Aurora Drive 是一個仍在開發中的輕量級個人雲端儲存服務。其 C++ 後端提供 REST API，用於瀏覽已設定的檔案系統根目錄並回報所在檔案系統的儲存容量。Angular 網頁用戶端透過開發代理使用這些端點。

## 其他語言

- [English (US)](../README.md)
- [简体中文](README-zh_CN.md)
- [日本語](README-ja_JP.md)

## 目前功能

- 瀏覽儲存根目錄或其子目錄中的項目。
- 回傳項目的名稱、識別出的類型、大小、修改時間和連結中繼資料。
- 目錄優先，再按名稱排序目錄列表。
- 阻擋解析後位於儲存根目錄之外的檔案系統路徑（路徑遍歷攻擊）。
- 回報儲存根目錄所在檔案系統的總容量和可用容量。
- 提供具備檔案瀏覽、儲存用量、語言選擇及明暗主題的 Angular 介面。

## 目前限制

Aurora Drive 目前處於 alpha 階段。後端尚未提供驗證、上傳、下載、建立目錄、刪除或移動檔案、分享，以及持久化收藏與回收桶的 API。因此，網頁用戶端中對應的數個控制項目前僅為展示功能或本機狀態功能。

儲存根目錄目前為行程工作目錄（`./`），由 `include/config.h` 中的 `kNetDiskRoot` 設定。請從想要公開的目錄啟動伺服器，且不要在不受信任的網路上公開包含敏感資料的目錄。

## 未來計畫

|名稱|預計版本|
|--|--|
|初步自訂與預設設定|0.2.0|
|進入子目錄瀏覽（真正可用）|0.2.0|
|建立、重新命名與刪除目錄及檔案|0.3.0|
|身分驗證|0.4.0|
|連結分享|0.5.0|
|加速、可恢復的檔案上傳與下載|0.6.0|
|...|...|

## 具體需求

### 部署

Aurora Drive 可以不使用前端部署，但最低需求會有所不同。

僅後端：
- 待決定

全端（即前端 + 後端）：
- 待決定

### 建置

- Linux（核心版本 >= 6.1）
- 支援 C++17 的編譯器
- CMake 3.20 或更新版本
- 捆綁的 [Crow](https://crowcpp.org/) 標頭檔所需的建置相依項目，包括 Boost
- Node.js 與 npm，僅在執行 Angular 網頁用戶端時需要

請放心，Angular 會在每個發布建置中更新至最新版本。

## 伺服器部署

### 執行後端

在儲存庫根目錄設定並建置 C++ 伺服器：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

從選定的儲存目錄啟動。例如，在開發時使用建置目錄作為儲存根目錄：

```bash
./build/aurora
```

伺服器監聽 `http://localhost:12384`，並使用 Crow 的多執行緒伺服器模式。

### 執行網頁用戶端

請在另一個終端機中先啟動後端，再安裝並啟動 Angular 應用程式：

```bash
cd frontend/aurora-drive
npm install
npm start
```

開啟 `http://localhost:4200`。Angular 開發伺服器會透過 `proxy.conf.json` 將 `/api` 下的請求轉送至 `http://localhost:12384`。

建置正式版用戶端：

```bash
cd frontend/aurora-drive
npm run build
```

## 關於發布版本

每個正式版本與預發布版本（beta、rcN）只包含預先建置的後端二進位套件，命名格式如下：
- `aurora_PLATFORM_ARCH_VERSION`

其中：
- `PLATFORM`：C 標準函式庫實作，主要為 glibc 和 musl-libc。
- `ARCH`：主機使用的架構，例如 i386、x86-64、aarch64、risc-v。
- `VERSION`：遵循 X.Y.Z（SemVer2 格式）的版本號。對於預發布版本，將直接加上修飾後綴（`-beta`、`-rcN`）。

Docker 與 Podman 映像檔正在規劃中，預計自 1.0.0 起提供。

## REST API 端點文件

免責聲明：本節中的所有用法僅為文件說明所需，請依實際情況調整。

### `GET /api/files`

列出儲存根目錄中的項目。可選參數為相對路徑 `path`，作為查詢參數以列出子目錄。

```bash
curl 'http://localhost:12384/api/files'
curl 'http://localhost:12384/api/files?path=docs'
```

一個回應的完整結構如下：

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

解析後的路徑若超出儲存根目錄，將回傳 `403 Forbidden` 與 `success: false`。不存在的路徑或非目錄路徑目前僅會回傳空的 `entries` 陣列。

### `GET /api/system/storage`

回傳儲存根目錄所在檔案系統的位元組數：

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

## 專案結構

```text
docs/                   本地化 README 檔案
frontend/aurora-drive/  Angular 網頁用戶端
src/                    C++ 伺服器進入點
include/                REST API、檔案系統與系統工具
include/external/crow   捆綁的 Crow 標頭檔
```

## 致謝

沒有以下開源專案，就不會有 Aurora Drive：
- [Crow](https://github.com/crowcpp/crow)
- [Angular](https://angular.dev/)
- [copyparty](https://github.com/9001/copyparty)（本專案的直接靈感來源）

同時，衷心感謝 JetBrains 為開源專案免費提供 CLion。

## 授權條款

Aurora Drive 採用 [GNU General Public License v3.0](../LICENSE) 授權。
