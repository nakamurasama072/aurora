# Aurora Drive

Aurora Drive は、開発中の軽量な個人向けクラウドストレージサービスです。C++ バックエンドは、設定済みのファイルシステムルートを参照し、そのファイルシステムのストレージ容量を返す小規模な REST API を提供します。Angular Web クライアントは開発プロキシを介してこれらのエンドポイントを利用します。

## 翻訳

- [English (US)](../README.md)
- [简体中文](README-zh_CN.md)
- [繁體中文](README-zh_TW.md)

## 現在の機能

- ストレージルートまたはその子ディレクトリ内のエントリを参照します。
- 各エントリの名前、判定された種類、サイズ、更新時刻、およびリンクのメタデータを返します。
- ディレクトリを先にし、その後に名前でディレクトリ一覧をソートします。
- 解決後のパスがストレージルート外になるファイルシステムパスを遮断します（パストラバーサル攻撃）。
- ストレージルートを含むファイルシステムの総容量と空き容量を返します。
- ファイル参照、ストレージ使用量、言語選択、ライト/ダークテーマを備えた Angular インターフェースを提供します。

## 現在の制限

Aurora Drive は現在 alpha 段階です。バックエンドには、認証、アップロード、ダウンロード、ディレクトリ作成、ファイルの削除または移動、共有、永続的なお気に入りとごみ箱の API はまだありません。そのため、Web クライアント内の対応する一部のコントロールは、現在は表示機能またはローカル状態のみの機能です。

ストレージルートは現在、`include/config.h` の `kNetDiskRoot` で設定されているプロセスの作業ディレクトリ（`./`）です。公開するディレクトリからサーバーを実行し、信頼できないネットワークで機密データを含むディレクトリを公開しないでください。

## 今後の計画

|項目|予定バージョン|
|--|--|
|初期のカスタムおよび既定の設定|0.2.0|
|子ディレクトリの参照（実用可能なもの）|0.2.0|
|ディレクトリとファイルの作成、名前変更、削除|0.3.0|
|本人認証|0.4.0|
|リンク共有|0.5.0|
|高速で復元可能なファイルのアップロードとダウンロード|0.6.0|
|...|...|

## 具体的な要件

### デプロイ

Aurora Drive はフロントエンドなしでもデプロイできますが、最低要件は異なります。

バックエンドのみ：
- 未定

フルスタック（フロントエンド + バックエンド）：
- 未定

### ビルド

- Linux（カーネルバージョン >= 6.1）
- C++17 対応コンパイラ
- CMake 3.20 以降
- バンドルされた [Crow](https://crowcpp.org/) ヘッダーに必要な Boost を含むビルド依存関係
- Angular Web クライアントを実行する場合のみ Node.js と npm

Angular は各リリースビルドで最新バージョンに更新されます。

## サーバーの配置

### バックエンドの実行

リポジトリのルートから C++ サーバーを構成してビルドします。

```bash
cmake -S . -B build
cmake --build build
```

選択したストレージディレクトリから起動します。たとえば、開発時にビルドディレクトリをストレージルートとして使用する場合は次のとおりです。

```bash
./build/aurora
```

サーバーは `http://localhost:12384` で待ち受け、Crow のマルチスレッドサーバーモードを使用します。

### Web クライアントの実行

別のターミナルで先にバックエンドを起動してから、Angular アプリケーションをインストールして起動します。

```bash
cd frontend/aurora-drive
npm install
npm start
```

`http://localhost:4200` を開きます。Angular 開発サーバーは、`proxy.conf.json` を通じて `/api` 配下のリクエストを `http://localhost:12384` に転送します。

本番用クライアントをビルドするには、次を実行します。

```bash
cd frontend/aurora-drive
npm run build
```

## リリースについて

各リリースおよびプレリリース（beta、rcN）には、ビルド済みのバックエンドバイナリパッケージのみが含まれます。命名形式は次のとおりです。
- `aurora_PLATFORM_ARCH_VERSION`

この形式では：
- `PLATFORM`：C 標準ライブラリの実装です。主に glibc と musl-libc です。
- `ARCH`：i386、x86-64、aarch64、risc-v など、ホストで使用されるアーキテクチャです。
- `VERSION`：X.Y.Z（SemVer2 形式）に従うバージョン番号です。プレリリースでは、修飾サフィックス（`-beta`、`-rcN`）をそのまま追加します。

Docker および Podman イメージは準備中で、1.0.0 以降での提供を予定しています。

## REST API エンドポイント

免責事項：この節のすべての使用例はドキュメントの説明用です。実際の状況に応じて調整してください。

### `GET /api/files`

ストレージルート内のエントリを一覧表示します。任意の相対パス `path` をクエリパラメーターとして指定すると、子ディレクトリを一覧表示できます。

```bash
curl 'http://localhost:12384/api/files'
curl 'http://localhost:12384/api/files?path=docs'
```

レスポンスの完全な構造は次のとおりです。

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

解決後のパスがストレージルート外になるリクエストは、`success: false` を含む `403 Forbidden` を受け取ります。存在しないパスまたはディレクトリでないパスは、現在空の `entries` 配列のみを返します。

### `GET /api/system/storage`

ストレージルートを含むファイルシステムのバイト数を返します。

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

## プロジェクト構成

```text
docs/                   ローカライズされた README ファイル
frontend/aurora-drive/  Angular Web クライアント
src/                    C++ サーバーのエントリポイント
include/                REST API、ファイルシステム、およびシステムユーティリティ
include/external/crow   バンドルされた Crow ヘッダー
```

## 謝辞

以下の外部オープンソースプロジェクトなしに Aurora Drive は存在しません。
- [Crow](https://github.com/crowcpp/crow)
- [Angular](https://angular.dev/)
- [copyparty](https://github.com/9001/copyparty)（本プロジェクトの直接の着想元）

また、オープンソースプロジェクト向けに CLion を無償提供している JetBrains に心より感謝します。

## ライセンス

Aurora Drive は [GNU General Public License v3.0](../LICENSE) の下でライセンスされています。
