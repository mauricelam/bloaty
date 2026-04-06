# Bloaty WebAssembly Build Instructions

This document provides detailed steps to build Bloaty for WebAssembly and run it in a web browser.

## Changes for WebAssembly

To support WebAssembly, the following changes were made:
-   **Single-threaded Execution**: The `__EMSCRIPTEN__` macro is used in `src/bloaty.cc` to disable multi-threading (`std::thread`). This ensures compatibility with standard browser environments without requiring special headers (COOP/COEP).
-   **JavaScript Entry Point**: A new file `src/wasm_main.cc` was added to expose Bloaty's functionality through Emscripten's `embind`. It mimics the CLI interface.
-   **Virtual File System**: The WASM build uses Emscripten's MEMFS to handle input files.
-   **Mocked Protobuf**: To reduce the WASM binary size and complexity, the full protobuf library is replaced with a minimal mock (`src/wasm_protobuf_mock.h`) for the WebAssembly build. This removes the dependency on the host-side `protoc` tool during the WASM build.
-   **Dedicated Build System**: `Makefile.wasm` was created to manage the build process.

## Prerequisites

1.  **Emscripten SDK (emsdk)**: You must have the Emscripten SDK installed and activated.
    ```bash
    git clone https://github.com/emscripten-core/emsdk.git
    cd emsdk
    ./emsdk install latest
    ./emsdk activate latest
    source ./emsdk_env.sh
    ```
2.  **CMake**: Version 3.10 or higher.

## Build Steps

Use the provided `Makefile.wasm` to build the project. It automates the following steps:
1.  Building `libbloaty.a` and all dependencies (except protobuf) for WASM.
2.  Linking everything into `build/web/bloaty.js` and `build/web/bloaty.wasm`.

```bash
make -f Makefile.wasm
```

## Running the Example

1.  Build the project as described above.
2.  Navigate to the root directory.
3.  Serve the root using a local web server (e.g., `python3 -m http.server`).
4.  Open the browser and navigate to `http://localhost:8000/build/web/`.
5.  Drag and drop a binary file into the application to see its size profile.
