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

#ifndef LAP_TIMER_BLE_CENTRAL_CONNECTION_INTERFACE_H
#define LAP_TIMER_BLE_CENTRAL_CONNECTION_INTERFACE_H

#include <cstdint>
#include <cstddef>

#include "ble/ble_manager_interface.h"

///
/// @brief This is an implementation of specific connection to the central device.
///
/// BleManager is responsible for creating and destorying connections. It should call
/// initialize before usage and cleanup afterward. User can register custom delegate
/// to handle events.
///
class BleCentralConnectionInterface {
public:

    ///
    /// @brief Custom delegate implementation, which can handle connection specifc events.
    /// 
    ///
    class Delegate {
    public:

        ///
        /// @brief Callback called when delegate is initialized.
        /// 
        /// @param ble_central_connection Connection initializing this delegate.
        ///
        virtual void on_initialized(BleCentralConnectionInterface &ble_central_connection) = 0;

        ///
        /// @brief Callback called when delegate is cleaned up.
        ///
        virtual void on_cleanup() = 0;

        ///
        /// @brief Callback called when negotiated mtu has changed.
        /// @note Called from interrupt context.
        /// 
        /// @param mtu New ATT_MTU value
        ///
        virtual void on_mtu_changed(uint16_t att_mtu) = 0;

        ///
        /// @brief Callback called when data was received on TX characteristic
        /// @note Called from interrupt context.
        ///
        /// @param data Pointer to the data buffer.
        /// @param length Length of the data.
        ///
        virtual void on_tx_write_request(const uint8_t* data, size_t length) = 0;

        ///
        /// @brief Callback called when packet was successfully sent via RX channel.
        /// @note Called from interrupt context.
        ///
        virtual void on_rx_completed() = 0;

    public:
        virtual ~Delegate() {}
    };

public:

    ///
    /// @brief Set the delegate object.
    /// 
    /// @param delegate Delegate object.
    ///
    virtual void set_delegate(Delegate *delegate) = 0;

    ///
    /// @brief Get MTU for current connection.
    /// 
    /// @return uint16_t MTU value.
    ///
    virtual uint16_t get_mtu() const = 0;

    ///
    /// @brief Get the MAC address of current connection.
    /// 
    /// @return const char* MAC address of current connection.
    ///
    virtual const char* get_mac_address() const = 0;

    ///
    /// @brief Send packet to the RX channel.
    /// 
    /// @param data Pointer to the data.
    /// @param length Length of the packet.
    /// @return true Packet was sent successfully.
    /// @return false Packet cannot be sent. Please retry later.
    ///
    virtual bool send_to_rx(const uint8_t* data, uint16_t length) = 0;

public:
    virtual ~BleCentralConnectionInterface() {}
};

#endif // LAP_TIMER_BLE_CENTRAL_CONNECTION_INTERFACE_H