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

#include "sdk_common.h"
#include "app_error.h"

#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_log.h"
#include "nrf_ble_gatt.h"

#include <algorithm>

#include "ble/ble_manager.h"
#include "utils/byte_utils.h"

// Statically allocated data due to SoftDevice architecture

NRF_BLE_GATT_DEF(m_gatt);

BleManager& BleManager::get_instance() {
    static BleManager manager;
    return manager;
}

void BleManager::handle_nrf_ble_gatt_evt(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt) {
    switch(p_evt->evt_id) {
        case NRF_BLE_GATT_EVT_ATT_MTU_UPDATED: {
            uint16_t new_mtu = p_evt->params.att_mtu_effective;
            BleCentralConnection* connection = BleManager::get_instance().get_central_connection_by_handle(p_evt->conn_handle);
            if (connection) {
                connection->set_mtu(new_mtu);
            }
        }
        case NRF_BLE_GATT_EVT_DATA_LENGTH_UPDATED: {
            break;
        }
    }
}

void BleManager::handle_ble_event(const ble_evt_t* ble_event, void* context) {
    UNUSED_PARAMETER(context);
    if(!BleManager::get_instance().handle_ble_event(ble_event)) {
        NRF_LOG_WARNING("Unhandled BLE event: %u", ble_event->header.evt_id);
    }
}

// BleManager initialization

BleManager::BleManager() : 
    delegate(nullptr), 
    conn_conf_tag(1), 
    central_connections { BleCentralConnection(*this, 0), BleCentralConnection(*this, 1) }, 
    is_advertising(false) {}

void BleManager::initialize(BleManager::Delegate &delegate) {
    NRF_LOG_INFO("Initializing BleManager...");
    initialize_ble();
    
    NRF_LOG_INFO("Initializing GAP...");
    initialize_gap();
    
    NRF_LOG_INFO("Initializing GATT...");
    initialize_gatt();
    
    NRF_LOG_INFO("Initializing attribute table...");
    attribute_table.initialize();
    
    NRF_LOG_INFO("Initializing advertisement data...");
    ble_uuid_t service_uuid = attribute_table.get_service_uuid();
    advertisement_data.initialize(&service_uuid, 1);
    
    NRF_LOG_INFO("Ble initialization complete!");
    this->delegate = &delegate;
    delegate.on_initialized(*this);

    NRF_LOG_INFO("Starting initial advertisement!");
    start_advertisement_if_needed();
}

void BleManager::initialize_ble() {
    NRF_LOG_INFO("Enabling SoftDevice...");
    APP_ERROR_CHECK(nrf_sdh_enable_request());
    while(!nrf_sdh_is_enabled());

    NRF_LOG_INFO("Setting default SoftDevice configuration for connection...");
    uint32_t ram_start = 0;
    APP_ERROR_CHECK(nrf_sdh_ble_default_cfg_set(conn_conf_tag, &ram_start));

    NRF_LOG_INFO("Enabling BLE stack...");
    APP_ERROR_CHECK(nrf_sdh_ble_enable(&ram_start));

    // This is statically allocated. Placed here to make handle_ble_event private.
    const auto ble_observer_priority = 3;
    NRF_SDH_BLE_OBSERVER(m_ble_observer, ble_observer_priority, BleManager::handle_ble_event, nullptr);
}

void BleManager::initialize_gap() {
    NRF_LOG_INFO("Setting device name...");
    ble_gap_conn_sec_mode_t sec_mode;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    static char device_name[] = "LapTimer";
    APP_ERROR_CHECK(sd_ble_gap_device_name_set(
        &sec_mode,
        (const uint8_t *)device_name,
        sizeof(device_name) / sizeof(device_name[0]
    )));
    
    NRF_LOG_INFO("Setting appearance...");
    APP_ERROR_CHECK(sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_TAG));

    NRF_LOG_INFO("Setting connection parameters...");
    ble_gap_conn_params_t gap_conn_params = {
        .min_conn_interval = MSEC_TO_UNITS(100, UNIT_1_25_MS),
        .max_conn_interval = MSEC_TO_UNITS(200, UNIT_1_25_MS),
        .slave_latency = 0,
        .conn_sup_timeout  = MSEC_TO_UNITS(4000, UNIT_10_MS) 
    };
    APP_ERROR_CHECK(sd_ble_gap_ppcp_set(&gap_conn_params));
}

void BleManager::initialize_gatt() {
    NRF_LOG_INFO("Initializing GATT library...");
    APP_ERROR_CHECK(nrf_ble_gatt_init(&m_gatt, handle_nrf_ble_gatt_evt));
}

// BleManager private functions

BleCentralConnection* BleManager::get_central_connection_by_handle(uint16_t handle) {
    return std::find_if(central_connections.begin(), central_connections.end(), [handle] (const auto &connection) {
        return connection.get_connection_handle() == handle;
    });
}

BleCentralConnection* BleManager::get_free_central_connection() {
    return std::find_if(central_connections.begin(), central_connections.end(), [] (const auto &connection) {
        return connection.get_connection_handle() == BLE_CONN_HANDLE_INVALID;
    });
}

void BleManager::start_advertisement_if_needed() {
    if (is_advertising) return;

    uint8_t count = 0;
    for (auto connection = central_connections.begin(); connection != central_connections.end(); connection++) {
        if (connection->get_connection_handle() == BLE_CONN_HANDLE_INVALID) {
            count++;
        }
    }
    if (count > 0) {
        APP_ERROR_CHECK(advertisement_data.start_advertising(conn_conf_tag));
        is_advertising = true;
    }
}

bool BleManager::handle_ble_event(const ble_evt_t* ble_event) {
    auto id = ble_event->header.evt_id;
    if (id >= BLE_EVT_BASE && id <= BLE_EVT_LAST) {
        return handle_common_event(id, ble_event->evt.common_evt);
    }

    if (id >= BLE_GAP_EVT_BASE && id <= BLE_GAP_EVT_LAST) {
        return handle_gap_event(id, ble_event->evt.gap_evt);
    }

    if (id >= BLE_GATTC_EVT_BASE && id <= BLE_GATTC_EVT_LAST) {
        // GATT library handle those.
        return true;
    }

    if (id >= BLE_GATTS_EVT_BASE && id <= BLE_GATTS_EVT_LAST) {
        return handle_gatts_event(id, ble_event->evt.gatts_evt);
    }

    return false;
}

bool BleManager::handle_common_event(uint16_t id, const ble_common_evt_t& common_event) {
    BleCentralConnection *connection = get_central_connection_by_handle(common_event.conn_handle);
    if (connection) {
        return connection->handle_common_event(id, common_event);
    }
    return false;
}

bool BleManager::handle_gap_event(uint16_t id, const ble_gap_evt_t& gap_event) {
    // Handle events directed to the manager only.
    switch (id) {
        case BLE_GAP_EVT_CONNECTED: {
            is_advertising = false;
            if (gap_event.params.connected.role != BLE_GAP_ROLE_PERIPH) {
                NRF_LOG_ERROR("BLE_GAP_EVT_CONNECTED: peripheral role is only supported!");
                APP_ERROR_CHECK(sd_ble_gap_disconnect(gap_event.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION));
                start_advertisement_if_needed();
                return true;
            }
            BleCentralConnection* new_connection = get_free_central_connection();
            if (!new_connection) {
                NRF_LOG_ERROR("BLE_GAP_EVT_CONNECTED: no more free central connections!");
                APP_ERROR_CHECK(sd_ble_gap_disconnect(gap_event.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION));
                start_advertisement_if_needed();
                return true;
            }
            new_connection->set_mtu(nrf_ble_gatt_eff_mtu_get(&m_gatt, gap_event.conn_handle));
            new_connection->set_connection_handle(gap_event.conn_handle);
            new_connection->set_mac_address(gap_event.params.connected.peer_addr.addr);
            NRF_LOG_INFO("BLE_GAP_EVT_CONNECTED: %s", new_connection->get_mac_address());
            if (delegate) {
                delegate->on_central_connected(*new_connection, new_connection->get_connection_id());
            }
            start_advertisement_if_needed();
            return true;
        }
        case BLE_GAP_EVT_DISCONNECTED: {
            BleCentralConnection* connection = get_central_connection_by_handle(gap_event.conn_handle);
            if (connection) {
                NRF_LOG_INFO("BLE_GAP_EVT_DISCONNECTED: mac=%s, reason=%u", 
                    connection->get_mac_address(),
                    gap_event.params.disconnected.reason
                );
                if (delegate) {
                    delegate->on_central_disconnected(*connection, connection->get_connection_id());
                }
                connection->set_connection_handle(BLE_CONN_HANDLE_INVALID);
            } else {
                NRF_LOG_INFO("BLE_GAP_EVT_DISCONNECTED: unexpected device, reason=%u", gap_event.params.disconnected.reason);
            }
            start_advertisement_if_needed();
            return true;
        }
        case BLE_GAP_EVT_ADV_SET_TERMINATED: {
            // This should not happen as we always advertise indefinitely
            NRF_LOG_INFO("BLE_GAP_EVT_ADV_SET_TERMINATED: reason=%u",
                gap_event.params.adv_set_terminated.reason
            );
            is_advertising = false;
            return true;
        }
    }

    // Handle other gap events via connection.
    BleCentralConnection *connection = get_central_connection_by_handle(gap_event.conn_handle);
    if (connection) {
        return connection->handle_gap_event(id, gap_event);
    }

    return false;
}

bool BleManager::handle_gatts_event(uint16_t id, const ble_gatts_evt_t& gatts_event) {
    BleCentralConnection *connection = get_central_connection_by_handle(gatts_event.conn_handle);
    if (connection) {
        return connection->handle_gatts_event(id, gatts_event);
    }

    return false;
}