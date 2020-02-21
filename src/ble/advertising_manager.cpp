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

#include "app_error.h"
#include "ble/advertising_manager.h"

AdvertisingManager::AdvertisingManager() : 
    advertisement_data({
        .adv_data = {
            .p_data = advertisement_data_buffer.data(),
            .len = static_cast<uint8_t>(advertisement_data_buffer.size())
        },
        .scan_rsp_data = {
            .p_data = scan_response_buffer.data(),
            .len = static_cast<uint8_t>(scan_response_buffer.size())
        }
    }),
    advertisement_params({
        .properties = {
            .type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED
        },
        .p_peer_addr = nullptr,
        .interval = MSEC_TO_UNITS(64, UNIT_0_625_MS),
        .duration = MSEC_TO_UNITS(0, UNIT_10_MS),
        .max_adv_evts = 0,
        .channel_mask = {0, 0, 0, 0, 0},
        .filter_policy = BLE_GAP_ADV_FP_ANY,
        .primary_phy = BLE_GAP_PHY_1MBPS,
        .secondary_phy = BLE_GAP_PHY_AUTO,
        .set_id = 0,
        .scan_req_notification = 0
    }),
    advertisement_handle(BLE_GAP_ADV_SET_HANDLE_NOT_SET) {}

void AdvertisingManager::initialize(const ble_uuid_t *uuids, uint16_t uuids_count) {
    ble_advdata_uuid_list_t service_uuid_list = {
        .uuid_cnt = uuids_count,
        .p_uuids = const_cast<ble_uuid_t*>(uuids)
    };

    ble_advdata_t adv_data = {
        .name_type = BLE_ADVDATA_FULL_NAME,
        .include_appearance = true,
        .flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE,
    };

    ble_advdata_t scan_response_data = {
        .uuids_more_available = service_uuid_list,
    };

    APP_ERROR_CHECK(ble_advdata_encode(
        &adv_data, 
        advertisement_data.adv_data.p_data, 
        &advertisement_data.adv_data.len
    ));
    APP_ERROR_CHECK(ble_advdata_encode(
        &scan_response_data, 
        advertisement_data.scan_rsp_data.p_data, 
        &advertisement_data.scan_rsp_data.len
    ));
    APP_ERROR_CHECK(sd_ble_gap_adv_set_configure(
        &advertisement_handle, 
        &advertisement_data, 
        &advertisement_params
    ));
}

uint32_t AdvertisingManager::start_advertising(uint8_t conn_cfg_tag) {
    return sd_ble_gap_adv_start(advertisement_handle, conn_cfg_tag);
}

uint32_t AdvertisingManager::stop_advertising() {
    return sd_ble_gap_adv_stop(advertisement_handle);
}