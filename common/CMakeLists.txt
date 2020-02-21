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

add_library(common OBJECT)

target_sources(common PUBLIC
    "include/ble/ble_central_connection_delegate.h"
    "include/ble/ble_central_connection_interface.h"
    "include/ble/ble_manager_delegate.h"
    "include/ble/ble_manager_interface.h"
    "include/events/event_dispatcher_interface.h"
    "include/events/event_observer.h"
    "include/events/events.h"
    "include/protocol/commands.h"
    "include/storage/flash_storage_interface.h"
    "include/storage/session_storage_events.h"
    "include/storage/session_storage.h"
    "include/utils/byte_utils.h"
    "include/utils/log.h"
    "include/utils/queue.h"
    "include/time/real_time_clock_interface.h"
    "include/rssi/rssi_reader_delegate.h"
    "include/rssi/rssi_reader_interface.h"
)

target_sources(common PRIVATE
    "src/ble/ble_central_connection_delegate.cpp"
    "src/rssi/rssi_reader_delegate.cpp"
    "src/storage/session_storage.cpp"
    "src/utils/byte_utils.cpp"
)

target_include_directories(common PUBLIC
    "include"
)

target_compile_features(common PRIVATE cxx_std_17)
