// MIT License

// Copyright (c) 2019 Polidea

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef LAP_TIMER_BLE_ADVERTISING_MANAGER_H
#define LAP_TIMER_BLE_ADVERTISING_MANAGER_H

#include "ble_gap.h"
#include "ble_advdata.h"

#include <cstdint>
#include <array>

///
/// @brief Class representing advertisement manager sent to initiate connections
///        with central devices.
/// 
///
class AdvertisingManager {
    public:
        AdvertisingManager();
        AdvertisingManager(const AdvertisingManager&) = delete;
        AdvertisingManager(AdvertisingManager&&) = delete;
        AdvertisingManager& operator=(const AdvertisingManager&) = delete;
        AdvertisingManager& operator=(AdvertisingManager&&) = delete;

        ///
        /// @brief Initialize advertisement data.
        /// @note This should be done after BLE initialization.
        ///
        /// @param uuids List of incomplete UUIDs to advertise.
        /// @param uuids_count Length of UUIDs list.
        ///
        void initialize(const ble_uuid_t *uuids, uint16_t uuids_count);

        ///
        /// @brief Start advertising process.
        /// 
        /// @param conn_cfg_tag Connection configuration tag.
        /// @return uint32_t nRF specific error code.
        ///
        uint32_t start_advertising(uint8_t conn_cfg_tag);

        ///
        /// @brief Stop advertising process.
        /// 
        /// @return uint32_t nRF specific error code.
        ///
        uint32_t stop_advertising();

    private:
        std::array<uint8_t, BLE_GAP_ADV_SET_DATA_SIZE_MAX> advertisement_data_buffer;
        std::array<uint8_t, BLE_GAP_ADV_SET_DATA_SIZE_MAX> scan_response_buffer;
        ble_gap_adv_data_t advertisement_data;
        ble_gap_adv_params_t advertisement_params;
        uint8_t advertisement_handle;
};

#endif // LAP_TIMER_BLE_ADVERTISING_MANAGER_H
