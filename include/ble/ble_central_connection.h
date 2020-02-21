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

#ifndef LAP_TIMER_BLE_CENTRAL_CONNECTION_H
#define LAP_TIMER_BLE_CENTRAL_CONNECTION_H

#include "ble.h"
#include "ble_types.h"
#include "ble_gap.h"
#include "ble_gatts.h"

#include "ble/ble_manager_interface.h"
#include "ble/ble_central_connection_interface.h"

#include <cstdint>

class BleManager;

///
/// @brief nRF implementation of BleCentralConnection
/// 
///
class BleCentralConnection final : public BleCentralConnectionInterface {
public:

    /// @brief Construct a new Ble Central Connection object
    /// 
    /// @param manager Manager instance.
    /// @param connection_id Connection id assigned by a manager.
    ///
    BleCentralConnection(BleManager& manager, uint8_t connection_id);
    
    virtual ~BleCentralConnection() {}

    ///
    /// @brief Inform connection about the MTU change.
    /// 
    /// @param mtu New MTU value.
    ///
    void set_mtu(uint16_t mtu) {
        this->mtu = mtu;
        if (delegate) {
            delegate->on_mtu_changed(mtu);
        }
    }

    ///
    /// @brief Set the connection handle.
    /// 
    /// @param connection_handle connection handle.
    ///
    void set_connection_handle(uint16_t connection_handle) {
        this->connection_handle = connection_handle;
    }

    ///
    /// @brief Set the MAC address.
    /// 
    /// @param mac_address_data MAC address in little endian order.
    ///
    void set_mac_address(const uint8_t* mac_address_data);

    ///
    /// @brief Get the connection id.
    /// 
    /// @return uint8_t Connection id.
    ///
    uint8_t get_connection_id() const {
        return connection_id;
    }

    ///
    /// @brief Get the connection handle.
    /// 
    /// @return uint16_t Connection handle.
    ///
    uint16_t get_connection_handle() const {
        return connection_handle;
    }

    /// @brief Handle Common event.
    /// 
    /// @param id nRF Common event id.
    /// @param common_event Common event.
    /// @return true Common event was handled by subroutine.
    /// @return false Common event wan't handled by subroutine.
    ///
    bool handle_common_event(uint16_t id, const ble_common_evt_t& common_event);
    
    ///
    /// @brief Handle GAP event.
    /// 
    /// @param id nRF GAP event id.
    /// @param gap_event GAP event.
    /// @return true GAP event was handled by subroutine.
    /// @return false GAP event wan't handled by subroutine.
    ///
    bool handle_gap_event(uint16_t id, const ble_gap_evt_t& gap_event);

    ///
    /// @brief Handle GATT Server event.
    /// 
    /// @param id nRF GATT Server event id.
    /// @param gatts_event GATT Server event.
    /// @return true GATT Server event was handled by subroutine.
    /// @return false GATT Server event wan't handled by subroutine.
    ///
    bool handle_gatts_event(uint16_t id, const ble_gatts_evt_t& gatts_event);

public:

    virtual uint16_t get_mtu() const override {
        return mtu;
    }

    virtual const char* get_mac_address() const override {
        return mac_addess_string;
    }

    virtual void set_delegate(Delegate *delegate) override {
        this->delegate = delegate;
    }

    virtual bool send_to_rx(const uint8_t* data, uint16_t length) override;

private:
    constexpr static int mac_address_string_len = BLE_GAP_ADDR_LEN * 3;

    BleManager& manager;
    uint8_t connection_id;
    uint16_t connection_handle;
    char mac_addess_string[mac_address_string_len];
    Delegate *delegate;
    uint16_t mtu;
};

#endif // LAP_TIMER_BLE_CENTRAL_CONNECTION_H