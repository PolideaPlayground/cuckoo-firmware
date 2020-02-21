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

#include "ble/ble_central_connection_delegate.h"
#include "utils/log.h"

void BleCentralConnectionDelegate::on_initialized(BleCentralConnectionInterface& ble_central_connection) {
    connection = &ble_central_connection;
    LOG_INFO("[%s] on_initialized", connection->get_mac_address());
}

void BleCentralConnectionDelegate::on_cleanup() {
    LOG_INFO("[%s] on_cleanup", connection->get_mac_address());
    connection = nullptr;
}

void BleCentralConnectionDelegate::on_mtu_changed(uint16_t mtu) {
    LOG_INFO("[%s] on_mtu_changed: mtu=%u", connection->get_mac_address(), mtu);
}

void BleCentralConnectionDelegate::on_tx_write_request(const uint8_t* data, size_t length) {
    LOG_INFO("[%s] on_tx_write_request: length=%u", connection->get_mac_address(), length);
    if (length == 0) return;

    switch (static_cast<CommandRequestCode>(data[0])) {
        case START_COMMAND_CODE: {
            StartCommand command;
            if (!command.deserialize(data, length)) {
                LOG_WARNING("[%s] Invalid start command payload", connection->get_mac_address());
                return;
            }
            handle_start_command(command);
            break;
        }
        case STOP_COMMAND_CODE: {
            StopCommand command;
            if (!command.deserialize(data, length)) {
                LOG_WARNING("[%s] Invalid stop command payload", connection->get_mac_address());
                return;
            }
            handle_stop_command(command);
            break;
        }
        case CURRENT_LAP_TIME_CODE: {
            CurrentLapTimeCommand command;
            if (!command.deserialize(data, length)) {
                LOG_WARNING("[%s] Invalid current lap time command", connection->get_mac_address());
                return;
            }
            handle_current_lap_time_command(command);
            break;
        }
        case BEST_LAP_TIME_CODE: {
            BestLapTimeCommand command;
            if (!command.deserialize(data, length)) {
                LOG_WARNING("[%s] Invalid best lap time command", connection->get_mac_address());
                return;
            }
            handle_best_lap_time_command(command);
            break;
        }
        case LAST_LAP_TIME_CODE: {
            LastLapTimeCommand command;
            if (!command.deserialize(data, length)) {
                LOG_WARNING("[%s] Invalid last lap time command", connection->get_mac_address());
                return;
            }
            handle_last_lap_time_command(command);
            break;
        }
        case LAST_SESSION_ID_CODE: {
            LastSessionIDCommand command;
            if (!command.deserialize(data, length)) {
                LOG_WARNING("[%s] Invalid last session id command", connection->get_mac_address());
                return;
            }
            handle_last_session_id_command(command);
            break;
        }
        case LIST_SESSIONS_IDS_CODE: {
            ListSessionsIDsCommand command(SESSION_ID_RELATION_MAX);
            if (!command.deserialize(data, length)) {
                LOG_WARNING("[%s] Invalid list sessions id command", connection->get_mac_address());
                return;
            }
            handle_list_sessions_ids_command(command);
            break;
        }
        case GET_SESSION_RECORD_CODE: {
            GetSessionRecordCommand command(0xFF, SESSION_ID_RELATION_MAX, 0xFF);
            if (!command.deserialize(data, length)) {
                LOG_WARNING("[%s] Invalid get session record command", connection->get_mac_address());
                return;
            }
            handle_get_session_record_command(command);
            break;
        }
        default: {
            LOG_WARNING("[%s] Command is not supported: id=%u", connection->get_mac_address(), data[0]);
        }
    }
}

void BleCentralConnectionDelegate::on_rx_completed() {
    LOG_INFO("[%s] on_rx_completed", connection->get_mac_address());
}

void BleCentralConnectionDelegate::handle_start_command(const StartCommand& start_command) {
    StartCommandResponse response(0x0001);
    const size_t data_length = StartCommandResponse::max_length;
    uint8_t data[data_length];
    if (!response.serialize(data, data_length)) {
        LOG_WARNING("[%s] Cannot construct start response", connection->get_mac_address());
        return;
    }
    if (!connection->send_to_rx(data, data_length)) {
        LOG_WARNING("[%s] Cannot send start response", connection->get_mac_address());
        return;
    }
}

void BleCentralConnectionDelegate::handle_stop_command(const StopCommand& stop_command) {
    StopCommandResponse response(1);
    const size_t data_length = StopCommandResponse::max_length;
    uint8_t data[data_length];
    if (!response.serialize(data, data_length)) {
        LOG_WARNING("[%s] Cannot construct stop response", connection->get_mac_address());
        return;
    }
    if (!connection->send_to_rx(data, data_length)) {
        LOG_WARNING("[%s] Cannot send stop response", connection->get_mac_address());
        return;
    }
}

void BleCentralConnectionDelegate::handle_current_lap_time_command(const CurrentLapTimeCommand& command) {
   CurrentLapTimeCommandResponse response(1, 1, 60 * 1000 * 3 + 52 * 1000 + 657);
   const size_t data_length = CurrentLapTimeCommandResponse::max_length;
   uint8_t data[data_length];
   if (!response.serialize(data, data_length)) {
       LOG_WARNING("[%s] Cannot construct current lap time response", connection->get_mac_address());
       return;
   }
   if (!connection->send_to_rx(data, data_length)) {
       LOG_WARNING("[%s] Cannot send current lap time response", connection->get_mac_address());
       return;
   }
}

void BleCentralConnectionDelegate::handle_best_lap_time_command(const BestLapTimeCommand& command) {
    BestLapTimeCommandResponse response(1, 1, 60 * 1000 * 3 + 48 * 1000 + 657);
    const size_t data_length = BestLapTimeCommandResponse::max_length;
    uint8_t data[data_length];
    if (!response.serialize(data, data_length)) {
        LOG_WARNING("[%s] Cannot construct best lap time response", connection->get_mac_address());
        return;
    }
    if (!connection->send_to_rx(data, data_length)) {
        LOG_WARNING("[%s] Cannot send best lap time response", connection->get_mac_address());
        return;
    }
}

void BleCentralConnectionDelegate::handle_last_lap_time_command(const LastLapTimeCommand& command) {
    LastLapTimeCommandResponse response(1, 1, 60 * 1000 * 4 + 1 * 1000 + 657);
    const size_t data_length = LastLapTimeCommandResponse::max_length;
    uint8_t data[data_length];
    if (!response.serialize(data, data_length)) {
        LOG_WARNING("[%s] Cannot construct last lap time response", connection->get_mac_address());
        return;
    }
    if (!connection->send_to_rx(data, data_length)) {
        LOG_WARNING("[%s] Cannot send last lap time response", connection->get_mac_address());
        return;
    }
}

void BleCentralConnectionDelegate::handle_last_session_id_command(const LastSessionIDCommand& command) {
    LastSessionIDCommandResponse response(1, SESSION_STATE_PENDING);
    const size_t data_length = LastSessionIDCommandResponse::max_length;
    uint8_t data[data_length];
    if (!response.serialize(data, data_length)) {
        LOG_WARNING("[%s] Cannot construct last session ID response", connection->get_mac_address());
        return;
    }
    if (!connection->send_to_rx(data, data_length)) {
        LOG_WARNING("[%s] Cannot send last session ID response", connection->get_mac_address());
        return;
    }
}

void BleCentralConnectionDelegate::handle_list_sessions_ids_command(const ListSessionsIDsCommand& command) {
    LOG_WARNING("[%s] List Sessions ID command not implemented.")
}

void BleCentralConnectionDelegate::handle_get_session_record_command(const GetSessionRecordCommand& command) {
    LOG_WARNING("[%s] Get Session record command not implemented.")
}
