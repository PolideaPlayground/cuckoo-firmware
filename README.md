# Cuckoo Firmware

This repo contains code for Cuckoo - an embedded lap timer.

## Hardware requirements

Follow the instructions in our [DIY instruction](https://polidea.com) to create hardware compatible with the firmware.

## Build requirements

Install following dependencies:
* [arm-none-eabi-gcc toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads)
* [nRF52 SDK](https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v15.x.x/nRF5_SDK_15.3.0_59ac345.zip)
* [J-Link by SEGGER](https://www.segger.com/downloads/jlink/)
* [nrfjprog](https://www.nordicsemi.com/?sc_itemid=%7B56868165-9553-444D-AA57-15BDE1BF6B49%7D)
* cmake

## Building firmware

After that point you can build firmware with following script (change paths and binaries if needed):

```bash
#!/bin/bash

set -eo pipefail

rm -rf build

cmake  \
    -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake \
    -DTOOLCHAIN_PREFIX=/Users/przemyslawlenart/toolchains/gcc-arm-none-eabi/ \
    -DNRF5_SDK_PATH=/Users/przemyslawlenart/toolchains/nRF5_SDK_15.3.0/ \
    -DNRFJPROG=/usr/local/bin/nrfjprog \
    -DNRF_TARGET=pca10059 \
    -S . -B build/

cmake --build build
```

## Building unit tests

Without specifying toolchain tests will be built:

```bash
#!/bin/bash

set -eo pipefail

rm -rf build

cmake  \
    -S . -B build/

cmake --build build
```

## VSCode integration:

Download following plugins:
- CMake Tools (`vector-of-bool.cmake-tools`)
- CMake (`twxs.cmake`)
- C/C++ (`ms-vscode.cpptools`)
- Cortex Debug (`marus25.cortex-debug`)

### Config files

Please add following config files to `.vscode` directory and update paths to directories and binaries.

#### `cmake-kits.json` - add ARM toolchain

```json
[{
    "name": "ARM-GCC-NONE-EABI",
    "toolchainFile": "cmake/arm-none-eabi.cmake",
    "cmakeSettings": {
        "TOOLCHAIN_PREFIX": "/Users/przemyslawlenart/toolchains/gcc-arm-none-eabi/"
    }
}]
```

#### `launch.json` - configure debugging

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug nRF52840_xxAA",
            "cwd": "${workspaceRoot}",
            "executable": "${workspaceRoot}/build/lap_timer.out",
            "request": "launch",
            "type": "cortex-debug",
            "servertype": "jlink",
            "device": "nRF52840_xxAA",
            "svdFile": "${config:cmake.configureSettings.NRF5_SDK_PATH}/modules/nrfx/mdk/nrf52840.svd"
        },
        {
            "name": "Debug tests",
            "cwd": "${workspaceRoot}",
            "program": "${workspaceRoot}/build/tests/test_lap_timer",
            "request": "launch",
            "type": "cppdbg",
            "MIMode": "lldb"
        }
    ]
}
```

#### `settings.json` - local settings with variables

```json
{
    "C_Cpp.default.configurationProvider": "vector-of-bool.cmake-tools",
    "cortex-debug.armToolchainPath": "/Users/przemyslawlenart/toolchains/gcc-arm-none-eabi/bin",
    "cmake.configureSettings": {
        "NRF5_SDK_PATH": "/Users/przemyslawlenart/toolchains/nRF5_SDK_15.3.0/",
        "NRFJPROG": "/usr/local/bin/nrfjprog",
        "NRF_TARGET": "pca10059",
    },
}
```
