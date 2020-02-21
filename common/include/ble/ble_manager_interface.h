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

#ifndef LAP_TIMER_BLE_MANAGER_INTERFACE_H
#define LAP_TIMER_BLE_MANAGER_INTERFACE_H

#include <cstdint>

class BleCentralConnectionInterface;

///
/// @brief BleManager interface which implement device specific functionality.
/// 
/// @tparam MAX_CENTRAL_CONNECTIONS Predetermined number of maxium number of connections
///         used to allow static allocations of connection instances.
///
template <uint8_t MAX_CENTRAL_CONNECTIONS>
class BleManagerInterface {
public:

    ///
    /// @brief  BleManager delegate class.
    /// 
    /// Instance of this class should implement platform independent functionality
    /// of the BLE handling.
    ///
    class Delegate {
    public:
        virtual ~Delegate() {}
    public:
        ///
        /// @brief BleManager is initialized.
        ///
        /// You can initialize your delegate instance at this point. This function
        /// should be called by BleManager's delegate.
        ///
        /// @note Called from main thread.
        /// 
        /// @param manager Manager which registered that delegate.
        ///
        virtual void on_initialized(BleManagerInterface& manager) = 0;

        ///
        /// @brief Called before BleManager is deinitialized. This should be called
        ///        by BleManager's delegate.
        /// 
        ///
        virtual void on_cleanup() = 0;

        ///
        /// @brief New connection to the central device was established.
        ///
        /// You can add your custom delegate to the new BleCentralConnectionInterface
        /// instance.
        ///
        /// @note Called from interrupt.
        /// @param central_connection new instance of central device which connected.
        /// @param connection_id New connection id in range 0 <= connection_id <= MAX_CENTRAL_CONNECTIONS.
        ///
        virtual void on_central_connected(BleCentralConnectionInterface& central_connection, uint8_t connection_id) = 0;

        ///
        /// @brief Central device disconnected from us.
        /// 
        /// You can unregister your delegate here. After a call disconnected device's
        /// instance is no longer valid.
        ///
        /// @param central_connection instance of central device which disconnected.
        /// @param connection_id Old connection id in range 0 <= connection_id <= MAX_CENTRAL_CONNECTIONS. 
        ///                      It is invalidated after the call and can be reused in further connections.
        ///
        virtual void on_central_disconnected(BleCentralConnectionInterface& central_connection, uint8_t connection_id) = 0;
    };

public:
    ///
    /// @brief Set the delegate object.
    /// 
    /// @param delegate Delegate object
    ///
    virtual void set_delegate(Delegate* delegate) = 0;

    virtual ~BleManagerInterface() {}
};

#endif // LAP_TIMER_BLE_MANAGER_INTERFACE_H