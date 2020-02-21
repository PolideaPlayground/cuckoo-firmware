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

#ifndef LAP_TIMER_EVENTS_H
#define LAP_TIMER_EVENTS_H

#include <variant>

#include "storage/session_storage_events.h"

class StartSession {
public:
    bool operator==(const StartSession& event) const {
        return true;
    }
};

class StopSession {
public:
    bool operator==(const StopSession& event) const {
        return true;
    }
};

class AddLapTime {
public:
    AddLapTime(uint32_t lap_time) : lap_time(lap_time) {}

    uint32_t get_lap_time() const {
        return lap_time;
    }

    bool operator==(const AddLapTime& event) const {
        return lap_time == event.lap_time;
    }

private:
    uint32_t lap_time;
};

class FlashLED {
public:
    FlashLED(uint8_t led_id, uint8_t ms_delay) : led_id(led_id), ms_delay(ms_delay) {}
    uint8_t get_ms_delay() const {
        return ms_delay;
    }
    uint8_t get_led_id() const {
        return led_id;
    }
    bool operator==(const FlashLED& other) const {
        return other.led_id == led_id && other.ms_delay == ms_delay;
    }
private:
    uint8_t led_id;
    uint8_t ms_delay;
};

class NewLap {
public:
    NewLap(uint32_t timestamp) : timestamp(timestamp) {}

    uint32_t get_timestamp() const {
        return timestamp;
    }
private:
    uint32_t timestamp;
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

typedef std::variant<
    std::monostate,

    StartSession,
    StopSession,
    AddLapTime,

    SessionStorageInitialized,
    ResetStorage,
    StorageResponse<ResetStorage>,
    LoadSessionIDsEvent,
    StorageResponse<LoadSessionIDsEvent>,
    LoadSessionRecordEvent,
    StorageResponse<LoadSessionRecordEvent>,

    FlashLED,
    NewLap

> Event;

// Increase if there is a need. Added here to monitor if event sizes are sane.
static_assert(sizeof(Event) < 64);

#endif // LAP_TIMER_EVENTS_H