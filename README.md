# Note from author

I am quite busy with academic work and FYP right now. There may not be any commits within 1 month, but be rest assure that strictly NO AI-generated code will be allowed for backends, and AI Agents are NOT ALLOWED to create Issues or Pull Requests either. This project is also a Rust-free one, you can use C/C++/Zig for the backend.

# Aurora Drive

Aurora Drive is an in-progress, lightweight personal cloud storage service. Its C++ backend exposes a small REST API for browsing a configured filesystem root and reporting the storage capacity of its containing filesystem. An Angular web client consumes those endpoints through a development proxy.

## Translations

- [简体中文](docs/README-zh_CN.md)
- [繁體中文](docs/README-zh_TW.md)
- [日本語](docs/README-ja_JP.md)

## Current capabilities

- Browse entries in the storage root or a child directory.
- Return each entry's name, detected type, size, modification time, and link metadata.
- Sort directory listings with directories first, then by name.
- Reject filesystem paths that resolve outside the storage root.
- Report total and available space for the filesystem that contains the storage root.
- Provide an Angular interface with file browsing, storage usage, locale selection, and light/dark themes.

## Current limitations

Aurora Drive is still in alpha. The backend currently has no API for authentication, upload, download, creating directories, deleting or moving files, sharing, or persistent favorites and trash. Several corresponding controls in the web client are therefore presentation or local-state features only.

The storage root is currently the process working directory (`./`), as configured by `kNetDiskRoot` in `include/config.h`. Run the server from the directory you intend to expose, and do not expose sensitive directories on an untrusted network.

## Future Plans

|Name|Expected Version|
|--|--|
|Custom and default configurations|0.2.0|
|Browse into child directories (for real)|0.2.0|
|Create, rename and delete directories and files|0.3.0|
|Authentication|0.4.0|
|Link sharing|0.5.0|
|Accelerated, resumable uploads and downloads|0.6.0|
|...|...|

## Requirements

### Deployment

Aurora Drive can be deployed without frontend. However, minimum requirements would vary.

For pure backend:
- To Be Decided

For full stack (i.e. frontend + backend):
- To Be Decided

### Building

- Linux (kernel version >= 6.1)
- A C++17-compatible compiler
- CMake 3.20 or later
- Build dependencies required by the bundled [Crow](https://crowcpp.org/) header, including Boost
- Node.js and npm, only when running the Angular web client

Please rest assured that the version of Angular will be updated to latest in every release.

## Deploying the server

### Run the backend

Configure and build the C++ server from the repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Start it from the chosen storage directory. For example, to expose the root of build directory during development:

```bash
./build/aurora
```

The server listens on `http://localhost:12384` and uses Crow's multithreaded server mode.

### Run the web client

In a separate terminal, start the backend first. Then install and serve the Angular application:

```bash
cd frontend/aurora-drive
npm install
npm start
```

Open `http://localhost:4200`. The Angular development server forwards requests under `/api` to `http://localhost:12384` through `proxy.conf.json`.

To produce a production client build:

```bash
cd frontend/aurora-drive
npm run build
```

## About the releases...

Each release and pre-release (beta, rcN) only contains the pre-built backend binary package. They would follow such naming:
- aurora_PLATFORM_ARCH_VERSION

In this case:
- PLATFORM: C library implementations. Mostly glibc and musl-libc.
- ARCH: The architecture used by the host, such as i386, x86-64, aarch64, risc-v.
- VERSION: The version number, following X.Y.Z SemVer2 format. For pre-releases, the suffixes (`-beta`, `-rcN`) will be simply appended.

Docker and podman images are on the way and expected to be available from 1.0.0 afterwards.

## REST API Endpoints Documentation

Disclaimer: All responses in this section are examples only.

### `GET /api/files`

Lists the entries in the storage root. Supply an optional relative `path` query parameter to list a child directory.

```bash
curl 'http://localhost:12384/api/files'
curl 'http://localhost:12384/api/files?path=docs'
```

A successful response has the following shape:

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

Requests whose resolved path escapes the storage root receive `403 Forbidden` with `success: false`. A missing or non-directory path currently returns an empty `entries` array.

### `GET /api/system/storage`

Returns byte counts for the filesystem containing the storage root:

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

## Project layout

```text
docs/                   Localized README files
frontend/aurora-drive/  Angular web client
src/                    C++ server entry point
include/                REST API, filesystem, and system utilities
include/external/crow   Bundled Crow header
```

## Acknowledgments

Aurora Drive will not be present without the following external open-source projects:
- [Crow](https://github.com/crowcpp/crow)
- [Angular](https://angular.dev/)
- [copyparty](https://github.com/9001/copyparty) (This was the direct inspiration for the project)

And I have to express my sincere thanks to Jetbrains, which had provided CLion free of charge for open-source projects.

## License

Aurora Drive is licensed under the [GNU General Public License v3.0](LICENSE).
