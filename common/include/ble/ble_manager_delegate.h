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

#ifndef LAP_TIMER_BLE_MANAGER_DELEGATE_H
#define LAP_TIMER_BLE_MANAGER_DELEGATE_H

#include "ble/ble_manager_interface.h"
#include "ble/ble_central_connection_delegate.h"

#include <array>

template<uint8_t MAX_CENTRAL_CONNECTIONS>
class BleManagerDelegate : public BleManagerInterface<MAX_CENTRAL_CONNECTIONS>::Delegate {
public:
    virtual void on_initialized(BleManagerInterface<MAX_CENTRAL_CONNECTIONS>& manager) override {
        this->manager = &manager;
    }

    virtual void on_cleanup() override {

    }

    virtual void on_central_connected(BleCentralConnectionInterface& central_connection, uint8_t connection_id) override {
        BleCentralConnectionDelegate* delegate = &connections[connection_id];
        delegate->on_initialized(central_connection);
        central_connection.set_delegate(delegate);
    }

    virtual void on_central_disconnected(BleCentralConnectionInterface& central_connection, uint8_t connection_id) override {
        BleCentralConnectionDelegate* delegate = &connections[connection_id];
        central_connection.set_delegate(nullptr);
        delegate->on_cleanup();
    }

private:
    BleManagerInterface<MAX_CENTRAL_CONNECTIONS> *manager;
    std::array<BleCentralConnectionDelegate, MAX_CENTRAL_CONNECTIONS> connections;
};

#endif // LAP_TIMER_BLE_MANAGER_DELEGATE_H
