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
#include "nrf_log.h"

#include "ble/ble_manager.h"
#include "ble/ble_central_connection.h"
#include "utils/byte_utils.h"

BleCentralConnection::BleCentralConnection(BleManager& manager, uint8_t connection_id) : 
    manager(manager),
    connection_id(connection_id),
    connection_handle(BLE_CONN_HANDLE_INVALID),
    mac_addess_string {'?', '?', ':', '?', '?', ':', '?', '?', ':', '?', '?', ':', '?', '?', ':', '?', '?', '\0'},
    delegate(nullptr),
    mtu(BLE_GATT_ATT_MTU_DEFAULT)
    {}

void BleCentralConnection::set_mac_address(const uint8_t* mac_address_data) {
    // Set MAC address.
    const int tmp_mac_address_len = BLE_GAP_ADDR_LEN * 2 + 1;
    char tmp_mac_address[tmp_mac_address_len];
    byte_buffer_to_hex(mac_address_data, BLE_GAP_ADDR_LEN, tmp_mac_address, tmp_mac_address_len);
    for (int i = 0; i < BLE_GAP_ADDR_LEN; i++) {
        int offset = mac_address_string_len - 3 * (i + 1);
        mac_addess_string[offset] = tmp_mac_address[2*i];
        mac_addess_string[offset+1] = tmp_mac_address[2*i+1];
    }
}

bool BleCentralConnection::send_to_rx(const uint8_t *data, uint16_t length) {
    const size_t debug_buffer_max_size = NRF_SDH_BLE_GATT_MAX_MTU_SIZE * 2;
    char debug_buffer[debug_buffer_max_size];
    byte_buffer_to_hex(data, length, debug_buffer, debug_buffer_max_size);
    NRF_LOG_INFO("[%s] Sending to rx: %s", get_mac_address(), debug_buffer);

    uint16_t sent_length = length;
    ble_gatts_hvx_params_t params = {
        .handle = manager.get_attribute_table().get_rx_characteristic_handle(),
        .type = BLE_GATT_HVX_INDICATION,
        .offset = 0,
        .p_len = &sent_length,
        .p_data = data
    };

    uint32_t error_code = sd_ble_gatts_hvx(connection_handle, &params);
    if (error_code == NRF_SUCCESS) {
        NRF_LOG_INFO("[%s] Successfully send %u bytes.", get_mac_address(), sent_length);
        return true;
    } else {
        NRF_LOG_ERROR("[%s] Got an error during send operation: %u", get_mac_address(), error_code);
        return false;
    }
}

bool BleCentralConnection::handle_common_event(uint16_t id, const ble_common_evt_t& common_event) {
    switch(id) {
        case BLE_EVT_USER_MEM_REQUEST: {
            NRF_LOG_INFO("[%s] BLE_EVT_USER_MEM_REQUEST", get_mac_address());
            APP_ERROR_CHECK(sd_ble_user_mem_reply(common_event.conn_handle, NULL));
            return true;
        }
        case BLE_EVT_USER_MEM_RELEASE: {
            NRF_LOG_INFO("[%s] BLE_EVT_USER_MEM_RELEASE", get_mac_address());
            return true;
        }
    }
    return false;
}

bool BleCentralConnection::handle_gap_event(uint16_t id, const ble_gap_evt_t& gap_event) {
    switch (id) {
        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST: {
            const ble_gap_conn_params_t& params = gap_event.params.conn_param_update_request.conn_params;
            NRF_LOG_INFO("[%s] BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST: conn_sup_timeout=%u, slave_latency=%u, min_conn_interval=%u, max_conn_interval=%u", 
                get_mac_address(),
                params.conn_sup_timeout,
                params.slave_latency,
                params.min_conn_interval,
                params.max_conn_interval
            );

            APP_ERROR_CHECK(sd_ble_gap_conn_param_update(gap_event.conn_handle, &params));
            return true;
        }
        case BLE_GAP_EVT_CONN_PARAM_UPDATE: {
            const ble_gap_conn_params_t &conn_params = gap_event.params.conn_param_update.conn_params;
            NRF_LOG_INFO("[%s] BLE_GAP_EVT_CONN_PARAM_UPDATE: conn_sup_timeout=%u, slave_latency=%u, min_conn_interval=%u, max_conn_interval=%u",
                get_mac_address(),
                conn_params.conn_sup_timeout,
                conn_params.slave_latency,
                conn_params.min_conn_interval,
                conn_params.max_conn_interval
            );
            return true;
        }
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST: {
            const ble_gap_phys_t &peer_preferred_phys = gap_event.params.phy_update_request.peer_preferred_phys;
            NRF_LOG_INFO("[%s] BLE_GAP_EVT_PHY_UPDATE_REQUEST: tx=%u, rx=%u",
                get_mac_address(),
                peer_preferred_phys.tx_phys,
                peer_preferred_phys.rx_phys
            );
            APP_ERROR_CHECK(sd_ble_gap_phy_update(gap_event.conn_handle, &peer_preferred_phys));
            return true;
        }
        case BLE_GAP_EVT_PHY_UPDATE: {
            const ble_gap_evt_phy_update_t &phy_update = gap_event.params.phy_update;
            NRF_LOG_INFO("[%s] BLE_GAP_EVT_PHY_UPDATE: status=%u, tx=%u, rx=%u",
                get_mac_address(),
                phy_update.status, 
                phy_update.tx_phy, 
                phy_update.rx_phy
            );
            return true;
        }
        case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST: {
            // Handled by GATT library.
            const ble_gap_data_length_params_t &peer_params = gap_event.params.data_length_update_request.peer_params;
            NRF_LOG_INFO("[%s] BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST: tx_octets=%u, rx_octets=%u, tx_time=%u, rx_time=%u",
                get_mac_address(),
                peer_params.max_tx_octets,
                peer_params.max_rx_octets,
                peer_params.max_tx_time_us,
                peer_params.max_rx_time_us
            );
            return true;
        }
        case BLE_GAP_EVT_DATA_LENGTH_UPDATE: {
            //  Handled by GATT library.
            const ble_gap_data_length_params_t& effective_params = gap_event.params.data_length_update.effective_params;
            NRF_LOG_INFO("[%s] BLE_GAP_EVT_DATA_LENGTH_UPDATE: tx_octets=%u, rx_octets=%u, tx_time=%u, rx_time=%u", 
                get_mac_address(),
                effective_params.max_tx_octets,
                effective_params.max_rx_octets,
                effective_params.max_tx_time_us,
                effective_params.max_rx_time_us
            );
            return true;
        }
    }

    return false;
}

bool BleCentralConnection::handle_gatts_event(uint16_t id, const ble_gatts_evt_t& gatts_event) {
    switch (id) {
        case BLE_GATTS_EVT_WRITE: {
            const size_t debug_buffer_max_size = NRF_SDH_BLE_GATT_MAX_MTU_SIZE * 2;
            char debug_buffer[debug_buffer_max_size];
            const ble_gatts_evt_write_t &write = gatts_event.params.write;
            byte_buffer_to_hex(write.data, write.len, debug_buffer, debug_buffer_max_size);
            NRF_LOG_INFO("[%s] BLE_GATTS_EVT_WRITE: handle=%u auth_required=%u length=%u offset=%u data=%s",
                get_mac_address(),
                write.handle, 
                write.auth_required,
                write.len,
                write.offset,
                debug_buffer
            );
            if (delegate && manager.get_attribute_table().get_tx_characteristic_handle() == write.handle) {
                delegate->on_tx_write_request(write.data, write.len);
            }
            return true;
         }
        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST: {
            const ble_gatts_evt_rw_authorize_request_t& request = gatts_event.params.authorize_request;
            uint8_t operation = request.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE ? request.request.write.op : 0;
            NRF_LOG_INFO("[%s] BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST: type=%u, op=%u",
                get_mac_address(),
                request.type,
                operation
            );
            if ((request.request.write.op != BLE_GATTS_OP_PREP_WRITE_REQ) &&
                (request.request.write.op != BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) &&
                (request.request.write.op != BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL)) {
                return true;
            }
            
            ble_gatts_rw_authorize_reply_params_t auth_reply = {0};
            auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
            auth_reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_APP_BEGIN;
            if (request.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL) {
                auth_reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
            }
            APP_ERROR_CHECK(sd_ble_gatts_rw_authorize_reply(gatts_event.conn_handle, &auth_reply));
            return true;
        }
        case BLE_GATTS_EVT_SYS_ATTR_MISSING: {
            NRF_LOG_INFO("[%s] BLE_GATTS_EVT_SYS_ATTR_MISSING: hint=%u", 
                get_mac_address(),
                gatts_event.params.sys_attr_missing.hint
            );
            APP_ERROR_CHECK(sd_ble_gatts_sys_attr_set(gatts_event.conn_handle, nullptr, 0, 0));
            return true;
        }
        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST: {
            // Implemented by the GATT library.
            NRF_LOG_INFO("[%s] BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST: mtu=%u", 
                get_mac_address(), 
                gatts_event.params.exchange_mtu_request.client_rx_mtu
            );
            return true;
        }
        case BLE_GATTS_EVT_HVC: {
            NRF_LOG_INFO("[%s] BLE_GATTS_EVT_HVC: handle=%u", 
                get_mac_address(), 
                gatts_event.params.hvc.handle
            );
            if (gatts_event.params.hvc.handle == manager.get_attribute_table().get_rx_characteristic_handle() && delegate) {
                delegate->on_rx_completed();
            }
            return true;
        }
        case BLE_GATTS_EVT_HVN_TX_COMPLETE: {
            NRF_LOG_INFO("[%s] BLE_GATTS_EVT_HVN_TX_COMPLETE: count=%u", 
                get_mac_address(),
                gatts_event.params.hvn_tx_complete.count
            );
            return true;
        }
    }

    return false;
}