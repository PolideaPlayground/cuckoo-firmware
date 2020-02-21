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

#ifndef LAP_TIMER_BLE_CENTRAL_CONNECTION_DELEGATE_H
#define LAP_TIMER_BLE_CENTRAL_CONNECTION_DELEGATE_H

#include "ble/ble_central_connection_interface.h"
#include "protocol/commands.h"

class BleCentralConnectionDelegate : public BleCentralConnectionInterface::Delegate {
public:
    virtual void on_initialized(BleCentralConnectionInterface& ble_central_connection) override;
    virtual void on_cleanup() override;
    virtual void on_mtu_changed(uint16_t mtu) override;
    virtual void on_tx_write_request(const uint8_t* data, size_t length) override;
    virtual void on_rx_completed() override;

private:

    void handle_start_command(const StartCommand& command);
    void handle_stop_command(const StopCommand& command);
    void handle_current_lap_time_command(const CurrentLapTimeCommand& command);
    void handle_best_lap_time_command(const BestLapTimeCommand& command);
    void handle_last_lap_time_command(const LastLapTimeCommand& command);
    void handle_last_session_id_command(const LastSessionIDCommand& command);
    void handle_list_sessions_ids_command(const ListSessionsIDsCommand& command);
    void handle_get_session_record_command(const GetSessionRecordCommand& command);

    BleCentralConnectionInterface* connection;
};

#endif // LAP_TIMER_BLE_CENTRAL_CONNECTION_DELEGATE_H