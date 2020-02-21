# MIT License

# Copyright (c) 2019 Polidea

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Make sure that all SDKs and tools paths are properly specified.
set(NRF5_SDK_PATH "" CACHE STRING "Path to the nRF5 SDK.")
if (NRF5_SDK_PATH STREQUAL "")
    message(FATAL_ERROR "The path to the nRF5 SDK (NRF5_SDK_PATH) must be set.")
endif ()

set(NRFJPROG "" CACHE STRING "Path to the nrfjprog binary.")
if (NRFJPROG STREQUAL "")
    message(FATAL_ERROR "The path to the nrfjprog utility (NRFJPROG) must be set.")
endif ()

set(NRF_TARGET "" CACHE STRING "nRF target name ex. pca10040")
if (NRF_TARGET STREQUAL "")
    message(FATAL_ERROR "nRF target (NRF_TARGET) must be set. Example: pca10040")
endif()

#  This is a core of the nRF5, which is included in every nRF5 build.
#  All of the source files are dependent on each other.
add_library(nRF5 OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5 PUBLIC
    # Config and components includes
    "config/"
    "${NRF5_SDK_PATH}/components"
    "${NRF5_SDK_PATH}/components/toolchain/cmsis/include"
    # Delay
    "${NRF5_SDK_PATH}/components/libraries/delay"
    # NRFX
    "${NRF5_SDK_PATH}/integration/nrfx"
    "${NRF5_SDK_PATH}/modules/nrfx"
    "${NRF5_SDK_PATH}/modules/nrfx/hal"
    "${NRF5_SDK_PATH}/modules/nrfx/mdk"
    # Utilities
    "${NRF5_SDK_PATH}/components/libraries/util"
    # Logging
    "${NRF5_SDK_PATH}/components/libraries/log"
    "${NRF5_SDK_PATH}/components/libraries/log/src"
    # Section Variables
    "${NRF5_SDK_PATH}/components/libraries/experimental_section_vars"
    # strerror
    "${NRF5_SDK_PATH}/components/libraries/strerror"
    # Other includes based on CONFIG variants (user should link libraries by itself)
    $<TARGET_PROPERTY:nRF5_MemoryObject,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:nRF5_fprintf,INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:nRF5_RingBuffer,INCLUDE_DIRECTORIES>
)

target_sources(nRF5 PRIVATE
    # NRFX 
    "${NRF5_SDK_PATH}/modules/nrfx/soc/nrfx_atomic.c"
    # Utilities
    "${NRF5_SDK_PATH}/components/libraries/util/app_error_handler_gcc.c"
    "${NRF5_SDK_PATH}/components/libraries/util/app_error_weak.c"
    "${NRF5_SDK_PATH}/components/libraries/util/app_error.c"
    "${NRF5_SDK_PATH}/components/libraries/util/app_util_platform.c"
    "${NRF5_SDK_PATH}/components/libraries/util/nrf_assert.c"
    # Logging
    "${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_frontend.c"
    "${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_str_formatter.c"
    # Section Variables
    "${NRF5_SDK_PATH}/components/libraries/experimental_section_vars/nrf_section_iter.c"
    # strerror
    "${NRF5_SDK_PATH}/components/libraries/strerror/nrf_strerror.c"
)

target_compile_definitions(nRF5 PUBLIC
    BSP_DEFINES_ONLY
    CONFIG_GPIO_AS_PINRESET
    FLOAT_ABI_HARD
    NRF_SD_BLE_API_VERSION=6
    SOFTDEVICE_PRESENT
    __HEAP_SIZE=8192
    __STACK_SIZE=8192
)

# Device specific settings: headers, softdevice blob etc.
if (NRF_TARGET STREQUAL "pca10059")
    target_sources(nRF5 PRIVATE
        "${NRF5_SDK_PATH}/modules/nrfx/mdk/gcc_startup_nrf52840.S"
        "${NRF5_SDK_PATH}/modules/nrfx/mdk/system_nrf52840.c"
    )
    target_include_directories(nRF5 PUBLIC
        "${NRF5_SDK_PATH}/components/softdevice/s140/headers"
        "${NRF5_SDK_PATH}/components/softdevice/s140/headers/nrf52"
    )
    target_compile_definitions(nRF5 PUBLIC
        BOARD_PCA10059
        NRF52840_XXAA
        S140
    )
    set(SOFTDEVICE_PATH "${NRF5_SDK_PATH}/components/softdevice/s140/hex/s140_nrf52_6.1.1_softdevice.hex")
else()
    message(FATAL_ERROR "nRF target \"${NRF_TARGET}\" is not supported.")
endif()

# Program softdevice
add_custom_target(flash_softdevice
    COMMAND ${NRFJPROG} --program ${SOFTDEVICE_PATH} -f nrf52 --sectorerase
    COMMAND ${NRFJPROG} --reset -f nrf52
    COMMENT "flashing SoftDevice"
)

# Flash memory
add_custom_target(flash_erase
    COMMAND ${NRFJPROG} --eraseall -f nrf52
    COMMENT "erasing flash"
)

# Function which links to the nRF5 objects and sets proper linker options 
# and creates targets for flashing.
function(link_nrf52)
    cmake_parse_arguments(
        LINK_NRF52
        ""
        "BINARY"
        ""
        ${ARGN}
    )
    target_link_libraries(${LINK_NRF52_BINARY} PUBLIC nRF5)
    set_target_properties(${LINK_NRF52_BINARY} PROPERTIES PREFIX "")
    set_target_properties(${LINK_NRF52_BINARY} PROPERTIES SUFFIX ".out")
    target_link_options(${LINK_NRF52_BINARY} PUBLIC
        "-L${NRF5_SDK_PATH}/modules/nrfx/mdk"
        "-T${PROJECT_SOURCE_DIR}/config/${NRF_TARGET}.ld"
        "-Wl,-Map=${LINK_NRF52_BINARY}.map"
    )
    add_custom_command(TARGET ${LINK_NRF52_BINARY}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_BIN} "${LINK_NRF52_BINARY}.out"
        COMMAND ${CMAKE_OBJCOPY_BIN} -O binary "${LINK_NRF52_BINARY}.out" "${LINK_NRF52_BINARY}.bin"
        COMMAND ${CMAKE_OBJCOPY_BIN} -O ihex "${LINK_NRF52_BINARY}.out" "${LINK_NRF52_BINARY}.hex"
        COMMENT "post build steps for ${LINK_NRF52_BINARY}"
    )
    add_custom_target(flash_${LINK_NRF52_BINARY}
        COMMAND ${NRFJPROG} --program ${LINK_NRF52_BINARY}.hex -f nrf52 --sectorerase
        COMMAND ${NRFJPROG} --reset -f nrf52
        COMMENT "flashing SoftDevice"
    )
    add_dependencies(flash_${LINK_NRF52_BINARY} ${LINK_NRF52_BINARY})
endfunction()

# nRF Board Support Package library

add_library(nRF5_BoardSupportPackage OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_BoardSupportPackage PUBLIC
    "${NRF5_SDK_PATH}/components/libraries/bsp"
    "${NRF5_SDK_PATH}/components/boards"
)

target_sources(nRF5_BoardSupportPackage PRIVATE
    "${NRF5_SDK_PATH}/components/boards/boards.c"
)

target_link_libraries(nRF5_BoardSupportPackage nRF5)

# nRF Atomic library

add_library(nRF5_Atomic OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_Atomic PUBLIC
    "${NRF5_SDK_PATH}/components/libraries/atomic"
)

target_sources(nRF5_Atomic PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/atomic/nrf_atomic.c"
)

target_link_libraries(nRF5_Atomic nRF5)

# nRF Block allocator library

add_library(nRF5_Balloc OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_Balloc PUBLIC
    "${NRF5_SDK_PATH}/components/libraries/balloc"
)

target_sources(nRF5_Balloc PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/balloc/nrf_balloc.c"
)

target_link_libraries(nRF5_Balloc nRF5)

# nRF SoftDevice Handler library

add_library(nRF5_SoftDeviceHandler OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_SoftDeviceHandler PUBLIC
    "${NRF5_SDK_PATH}/components/softdevice/common"
)

target_sources(nRF5_SoftDeviceHandler PRIVATE
    "${NRF5_SDK_PATH}/components/softdevice/common/nrf_sdh.c"
)

target_link_libraries(nRF5_SoftDeviceHandler nRF5)

# nRF SoftDevice SoC library

add_library(nRF5_SoftDeviceSoC OBJECT EXCLUDE_FROM_ALL)

target_sources(nRF5_SoftDeviceSoC PRIVATE
    "${NRF5_SDK_PATH}/components/softdevice/common/nrf_sdh_soc.c"
)

target_link_libraries(nRF5_SoftDeviceSoC nRF5_SoftDeviceHandler)

# nRF SoftDevice BLE library

add_library(nRF5_SoftDeviceBLE OBJECT EXCLUDE_FROM_ALL)

target_sources(nRF5_SoftDeviceBLE PRIVATE
    "${NRF5_SDK_PATH}/components/softdevice/common/nrf_sdh_ble.c"
)

target_link_libraries(nRF5_SoftDeviceBLE nRF5_SoftDeviceHandler)

# nRF fprintf library

add_library(nRF5_fprintf OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_fprintf PUBLIC
    "${NRF5_SDK_PATH}/external/fprintf"
)

target_sources(nRF5_fprintf PRIVATE
    "${NRF5_SDK_PATH}/external/fprintf/nrf_fprintf_format.c"
    "${NRF5_SDK_PATH}/external/fprintf/nrf_fprintf.c"
)

target_link_libraries(nRF5_fprintf nRF5)

# nRF Memory object library

add_library(nRF5_MemoryObject OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_MemoryObject PUBLIC
    "${NRF5_SDK_PATH}/components/libraries/memobj"
)

target_sources(nRF5_MemoryObject PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/memobj/nrf_memobj.c"
)

target_link_libraries(nRF5_MemoryObject nRF5_Balloc nRF5_Atomic)

# nRF Ring Buffer library

add_library(nRF5_RingBuffer OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_RingBuffer PUBLIC
    "${NRF5_SDK_PATH}/components/libraries/ringbuf"
)

target_sources(nRF5_RingBuffer PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/ringbuf/nrf_ringbuf.c"
)

target_link_libraries(nRF5_RingBuffer nRF5_Atomic)

# Segger RTT external library

add_library(nRF5_Segger_RTT OBJECT EXCLUDE_FROM_ALL)


target_include_directories(nRF5_Segger_RTT PUBLIC
    "${NRF5_SDK_PATH}/external/segger_rtt"
)

target_sources(nRF5_Segger_RTT PRIVATE
    "${NRF5_SDK_PATH}/external/segger_rtt/SEGGER_RTT.c"
)

target_link_libraries(nRF5_Segger_RTT nRF5)

# nRF Logger RTT

add_library(nRF5_Logger_RTT OBJECT EXCLUDE_FROM_ALL)

target_sources(nRF5_Logger_RTT PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_backend_rtt.c"
)

target_link_libraries(nRF5_Logger_RTT nRF5_Segger_RTT nRF5_Logger_Serial)

# nRF Logger Flash

add_library(nRF5_Logger_Flash OBJECT EXCLUDE_FROM_ALL)

target_sources(nRF5_Logger_Flash PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_backend_flash.c"
)

target_link_libraries(nRF5_Logger_Flash nRF5)

# nRF Logger UART

add_library(nRF5_Logger_UART OBJECT EXCLUDE_FROM_ALL)

target_sources(nRF5_Logger_UART PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_backend_uart.c"
)

target_link_libraries(nRF5_Logger_UART nRF5)

# nRF Logger Serial

add_library(nRF5_Logger_Serial OBJECT EXCLUDE_FROM_ALL)

target_sources(nRF5_Logger_Serial PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_backend_serial.c"
)

target_link_libraries(nRF5_Logger_Serial nRF5)

# nRF Logger Default

add_library(nRF5_Logger_DefaultBackends OBJECT EXCLUDE_FROM_ALL)

target_sources(nRF5_Logger_DefaultBackends PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/log/src/nrf_log_default_backends.c"
)

target_link_libraries(nRF5_Logger_DefaultBackends nRF5_Logger_UART nRF5_Logger_Serial)

# nRF App Timer library

add_library(nRF5_AppTimer OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_AppTimer PUBLIC
    "${NRF5_SDK_PATH}/components/libraries/timer/"
    $<TARGET_PROPERTY:nRF5_AppScheduler,INCLUDE_DIRECTORIES>
)

target_sources(nRF5_AppTimer PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/timer/app_timer.c"
)

target_link_libraries(nRF5_AppTimer nRF5)

# nRF Scheduler library

add_library(nRF5_AppScheduler OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_AppScheduler PUBLIC
    "${NRF5_SDK_PATH}/components/libraries/scheduler/"
)

target_sources(nRF5_AppScheduler PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/scheduler/app_scheduler.c"
)

target_link_libraries(nRF5_AppScheduler nRF5)

# nRF BLE Adv Data Encoder library

add_library(nRF5_BLE_Common_AdvData_Encoder OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_BLE_Common_AdvData_Encoder PUBLIC
    "${NRF5_SDK_PATH}/components/ble/common/"
)

target_sources(nRF5_BLE_Common_AdvData_Encoder PRIVATE
    "${NRF5_SDK_PATH}/components/ble/common/ble_advdata.c"
)

target_link_libraries(nRF5_BLE_Common_AdvData_Encoder nRF5)

# nRF BLE Common service library

add_library(nRF5_BLE_Common_Service OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_BLE_Common_Service PUBLIC
    "${NRF5_SDK_PATH}/components/ble/common/"
)

target_sources(nRF5_BLE_Common_Service PRIVATE
    "${NRF5_SDK_PATH}/components/ble/common/ble_srv_common.c"
)

target_link_libraries(nRF5_BLE_Common_Service nRF5)

# nRF BLE GATT library

add_library(nRF5_BLE_GATT OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_BLE_GATT PUBLIC
    "${NRF5_SDK_PATH}/components/ble/nrf_ble_gatt/"
)

target_sources(nRF5_BLE_GATT PRIVATE
    "${NRF5_SDK_PATH}/components/ble/nrf_ble_gatt/nrf_ble_gatt.c"
)

target_link_libraries(nRF5_BLE_GATT nRF5_SoftDeviceBLE)

# nRF BLE Queued Writes library

add_library(nRF5_BLE_QueuedWrites OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_BLE_QueuedWrites PUBLIC
    "${NRF5_SDK_PATH}/components/ble/nrf_ble_qwr/"
)

target_sources(nRF5_BLE_QueuedWrites PRIVATE
    "${NRF5_SDK_PATH}/components/ble/nrf_ble_qwr/nrf_ble_qwr.c"
)

target_link_libraries(nRF5_BLE_QueuedWrites nRF5_SoftDeviceBLE)

# Atomic FIFO

add_library(nRF5_AtomicFIFO OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_AtomicFIFO PUBLIC
    "${NRF5_SDK_PATH}/components/libraries/atomic_fifo/"
)

target_sources(nRF5_AtomicFIFO PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/atomic_fifo/nrf_atfifo.c"
)

target_link_libraries(nRF5_AtomicFIFO nRF5)

# nRF Flash Storage

add_library(nRF5_FlashStorage OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_FlashStorage PUBLIC
    "${NRF5_SDK_PATH}/components/libraries/fstorage/"
)

target_sources(nRF5_FlashStorage PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/fstorage/nrf_fstorage.c"
    "${NRF5_SDK_PATH}/components/libraries/fstorage/nrf_fstorage_sd.c"
)

target_link_libraries(nRF5_FlashStorage nRF5_AtomicFIFO nRF5_SoftDeviceHandler)

# nRF Flash Data Storage

add_library(nRF5_FlashDataStorage OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_FlashDataStorage PUBLIC
    "${NRF5_SDK_PATH}/components/libraries/fds/"
)

target_sources(nRF5_FlashDataStorage PRIVATE
    "${NRF5_SDK_PATH}/components/libraries/fds/fds.c"
)

target_link_libraries(nRF5_FlashDataStorage nRF5_FlashStorage)


# nRF Periph Drivers

add_library(nRF5_PeriphDrivers OBJECT EXCLUDE_FROM_ALL)

target_include_directories(nRF5_PeriphDrivers PUBLIC
    "${NRF5_SDK_PATH}/modules/nrfx/drivers/include/"
)

target_sources(nRF5_PeriphDrivers PRIVATE
    "${NRF5_SDK_PATH}/modules/nrfx/drivers/src/nrfx_saadc.c"
    "${NRF5_SDK_PATH}/modules/nrfx/drivers/src/nrfx_timer.c"
    "${NRF5_SDK_PATH}/modules/nrfx/drivers/src/nrfx_ppi.c"
    "${NRF5_SDK_PATH}/modules/nrfx/drivers/src/nrfx_uart.c"
    "${NRF5_SDK_PATH}/modules/nrfx/drivers/src/nrfx_rtc.c"
)

target_link_libraries(nRF5_PeriphDrivers nRF5)
