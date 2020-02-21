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

#ifndef LAP_TIMER_BLE_MANAGER_H
#define LAP_TIMER_BLE_MANAGER_H

#include <array>

#include "ble/ble_central_connection.h"
#include "ble/ble_manager_interface.h"
#include "ble/advertising_manager.h"
#include "ble/attribute_table.h"

#include "nrf_ble_gatt.h"

///
/// @brief nRF BleManager imnplementation
///
class BleManager final : public BleManagerInterface<NRF_SDH_BLE_PERIPHERAL_LINK_COUNT> {
public:
    BleManager(const BleManager&) = delete;
    BleManager(BleManager&&) = delete;
    BleManager& operator=(const BleManager&) = delete;
    BleManager& operator=(BleManager&&) = delete;
    virtual ~BleManager() {}

    ///
    /// @brief Get global singleton instance
    /// 
    /// @return BleManager& singleton instance
    ///
    static BleManager& get_instance();

    ///
    /// @brief Initialize BleManager with a delegate.
    /// 
    /// @param delegate Delegate of BleManager object.
    ///
    void initialize(BleManager::Delegate &delegate);

    ///
    /// @brief Get the attribute table instance.
    /// 
    /// @return const AttributeTable& Attribute table.
    ///
    const AttributeTable& get_attribute_table() const {
        return attribute_table;
    }

private:
    BleManager();

    void initialize_ble();
    void initialize_gap();
    void initialize_gatt();

    static void handle_ble_event(const ble_evt_t* ble_event, void* context);
    static void handle_nrf_ble_gatt_evt(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt);

    bool handle_ble_event(const ble_evt_t* ble_event);
    bool handle_common_event(uint16_t id, const ble_common_evt_t& common_event);
    bool handle_gap_event(uint16_t id, const ble_gap_evt_t& gap_event);
    bool handle_gatts_event(uint16_t id, const ble_gatts_evt_t& gatts_event);

    BleCentralConnection* get_free_central_connection();
    BleCentralConnection* get_central_connection_by_handle(uint16_t handle);
    
    void start_advertisement_if_needed();

public:
    virtual void set_delegate(Delegate* delegate) override { 
        this->delegate = delegate;
    }

private:
    Delegate* delegate;
    uint8_t conn_conf_tag;
    uint8_t ble_observer_priority;
    AdvertisingManager advertisement_data;
    AttributeTable attribute_table;
    std::array<BleCentralConnection, NRF_SDH_BLE_PERIPHERAL_LINK_COUNT> central_connections;
    bool is_advertising;
};

#endif // LAP_TIMER_BLE_MANAGER_H