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

# Lap timer
cmake_minimum_required(VERSION 3.15)

project(lap_timer VERSION 1.0)
enable_language(C ASM CXX)

# Include common functionality
add_subdirectory(common)

if (CMAKE_CROSSCOMPILING)
    # Include nRF definitions
    include(cmake/nrf52.cmake)

    # Create main executable
    add_executable(${CMAKE_PROJECT_NAME} 
        "include/ble/advertising_manager.h"
        "include/ble/attribute_table.h"
        "include/ble/ble_central_connection.h"
        "include/ble/ble_manager.h"
        "include/events/event_dispatcher.h"
        "include/storage/flash_storage.h"
        "include/time/real_time_clock.h"
        "include/rssi/rssi_reader.h"
        "src/ble/advertising_manager.cpp"
        "src/ble/attribute_table.cpp"
        "src/ble/ble_central_connection.cpp"
        "src/ble/ble_manager.cpp"
        "src/events/event_dispatcher.cpp"
        "src/storage/flash_storage.cpp"
        "src/time/real_time_clock.cpp"
        "src/rssi/rssi_reader.cpp"
        "src/main.cpp"
    )

    # Add local include folder
    target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE include)

    # Make sure to link to the base of nRF5
    link_nrf52(BINARY ${CMAKE_PROJECT_NAME})

    # Add information to propagate sdk_config to common library.
    target_compile_definitions(common PUBLIC
        USE_COMMON_CONFIG
    )
    target_link_libraries(common PRIVATE
        nRF5
    )

    # Link common functionality
    target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
        nRF5_PeriphDrivers
        nRF5_fprintf
        nRF5_Atomic
        nRF5_Balloc
        nRF5_RingBuffer
        nRF5_MemoryObject
        nRF5_BoardSupportPackage
        nRF5_Logger_DefaultBackends
        nRF5_Segger_RTT
        nRF5_Logger_RTT
        nRF5_Logger_Serial
        nRF5_AppTimer
        nRF5_AppScheduler
        nRF5_SoftDeviceHandler
        nRF5_SoftDeviceSoC
        nRF5_SoftDeviceBLE
        nRF5_BLE_GATT
        nRF5_BLE_Common_AdvData_Encoder
        nRF5_AtomicFIFO
        nRF5_FlashStorage
        nRF5_FlashDataStorage
        common
    )
else()
    # Build tests
    add_subdirectory(tests)
endif()
