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

#include "ble/attribute_table.h"

#include "app_error.h"
#include "ble.h"

AttributeTable::AttributeTable() : 
    base_uuid128({
        .uuid128 = { 0xb4, 0x78, 0xff, 0xea, 0x72, 0x2a, 0x43, 0x06, 
                     0xb1, 0x9d, 0x53, 0x3a, 0x00, 0x00, 0x00, 0x00 }
    }),
    base_uuid_type(BLE_UUID_TYPE_UNKNOWN),
    
    // Service
    service_uuid({
        .uuid = 0x1000,
        .type = base_uuid_type
    }),
    service_handle(BLE_GATT_HANDLE_INVALID),

    // TX Characteristic
    tx_characteristic_uuid({
        .uuid = 0x1001,
        .type = base_uuid_type
    }),
    tx_characteristic_md({
        .char_props = {
            .write_wo_resp = 1,
            .write = 1
        },
        .char_ext_props = {
        },
        .p_char_user_desc = reinterpret_cast<const uint8_t*>(u8"Lap Timer TX"),
        .char_user_desc_max_size = 12,
        .char_user_desc_size = 12,
        .p_char_pf = nullptr,
        .p_user_desc_md = nullptr,
        .p_cccd_md = nullptr,
        .p_sccd_md = nullptr
    }),
    tx_characteristic_attr_md({
        .read_perm = {
            .sm = 1,
            .lv = 1
        },
        .write_perm = {
            .sm = 1,
            .lv = 1
        },
        .vlen = 1,
        .vloc = BLE_GATTS_VLOC_STACK,
        .rd_auth = 0,
        .wr_auth = 0
    }),
    tx_characteristic_attr({
        .p_uuid = &tx_characteristic_uuid,
        .p_attr_md = &tx_characteristic_attr_md,
        .init_len = 0,
        .init_offs = 0,
        .max_len = BLE_GATTS_VAR_ATTR_LEN_MAX,
        .p_value = nullptr
    }),
    
    // RX Characteristic
    rx_characteristic_uuid({
        .uuid = 0x1002,
        .type = base_uuid_type
    }),
    rx_characteristic_md({
        .char_props = {
            .indicate = 1
        },
        .char_ext_props = {
        },
        .p_char_user_desc = reinterpret_cast<const uint8_t*>(u8"Lap Timer RX"),
        .char_user_desc_max_size = 12,
        .char_user_desc_size = 12,
        .p_char_pf = nullptr,
        .p_user_desc_md = nullptr,
        .p_cccd_md = nullptr,
        .p_sccd_md = nullptr
    }),
    rx_characteristic_attr_md({
        .read_perm = {
            .sm = 1,
            .lv = 1
        },
        .write_perm = {
            .sm = 1,
            .lv = 1
        },
        .vlen = 1,
        .vloc = BLE_GATTS_VLOC_STACK,
        .rd_auth = 0,
        .wr_auth = 0
    }),
    rx_characteristic_attr({
        .p_uuid = &rx_characteristic_uuid,
        .p_attr_md = &rx_characteristic_attr_md,
        .init_len = 0,
        .init_offs = 0,
        .max_len = BLE_GATTS_VAR_ATTR_LEN_MAX,
        .p_value = nullptr
    }) {}

void AttributeTable::initialize() {
        APP_ERROR_CHECK(sd_ble_uuid_vs_add(&base_uuid128, &base_uuid_type));

        service_uuid.type = base_uuid_type;
        tx_characteristic_uuid.type = base_uuid_type;
        rx_characteristic_uuid.type = base_uuid_type;

        APP_ERROR_CHECK(sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &service_uuid, &service_handle));
        
        APP_ERROR_CHECK(sd_ble_gatts_characteristic_add(
            service_handle, 
            &tx_characteristic_md, 
            &tx_characteristic_attr, 
            &tx_characteristic_handles
        ));

        APP_ERROR_CHECK(sd_ble_gatts_characteristic_add(
            service_handle, 
            &rx_characteristic_md, 
            &rx_characteristic_attr, 
            &rx_characteristic_handles
        ));
}
