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

#ifndef LAP_TIMER_SESSION_STORAGE_EVENTS_H
#define LAP_TIMER_SESSION_STORAGE_EVENTS_H

#include <cstdint>

template<typename T>
class StorageResponse {
public:
    StorageResponse(T value, bool successful) : value(value), successful(successful) {}

    T get_value() const {
        return value;
    }

    bool is_successful() const {
        return successful;
    }

    bool operator==(const StorageResponse& response) const {
        return value == response.value && successful == response.successful;
    }

private:
    T value;
    bool successful;
};

class SessionStorageInitialized {
public:
    bool operator==(const SessionStorageInitialized& event) const {
        return true;
    }
};

class ResetStorage {
public:
    bool operator==(const ResetStorage& event) const {
        return true;
    }
};

class LoadSessionIDsEvent {
public:
    LoadSessionIDsEvent(
        uint16_t session_id_offset,
        uint16_t* session_ids_data,
        uint16_t session_ids_length) :
        session_id_offset(session_id_offset),
        session_ids_data(session_ids_data),
        session_ids_length(session_ids_length)
        {}

    uint16_t get_session_id_offset() const {
        return session_id_offset;
    }
    uint16_t* get_session_ids_data() const {
        return session_ids_data;
    }
    uint16_t get_session_ids_length() const {
        return session_ids_length;
    }

    bool operator==(const LoadSessionIDsEvent& event) const {
        return session_id_offset == event.session_id_offset &&
               session_ids_data == event.session_ids_data &&
               session_ids_length == event.session_ids_length;
    }

private:
    uint16_t session_id_offset;
    uint16_t* session_ids_data;
    uint16_t session_ids_length;
};

class LoadSessionRecordEvent {
public:
    LoadSessionRecordEvent(
        uint16_t session_id, 
        uint8_t lap_offset, 
        uint32_t *lap_time_data, 
        uint8_t lap_time_data_length) :
        session_id(session_id),
        lap_offset(lap_offset),
        lap_time_data(lap_time_data),
        lap_time_data_length(lap_time_data_length) {}

    uint16_t get_session_id() const {
        return session_id;
    }

    uint8_t get_lap_offset() const {
        return lap_offset;
    }

    uint32_t *get_lap_time_data() const {
        return lap_time_data;
    }

    uint8_t get_lap_time_data_length() const {
        return lap_time_data_length;
    }

    bool operator==(const LoadSessionRecordEvent& event) const {
        return session_id == event.session_id &&
               lap_offset == event.lap_offset &&
               lap_time_data == event.lap_time_data &&
               lap_time_data_length == event.lap_time_data_length;
    }

private:
    uint16_t session_id;
    uint8_t lap_offset;
    uint32_t *lap_time_data;
    uint8_t lap_time_data_length;
};

#endif // LAP_TIMER_SESSION_STORAGE_EVENTS_H