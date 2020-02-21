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

#ifndef LAP_TIMER_EVENT_DISPATCHER_H
#define LAP_TIMER_EVENT_DISPATCHER_H

#include "app_timer.h"

#include "events/event_dispatcher_interface.h"
#include "events/event_observer.h"

#include <array>
#include <atomic>

class EventDispatcher : public EventDispatcherInterface {
public:
    static EventDispatcher& get_instance() {
        static EventDispatcher event_dispatcher;
        return event_dispatcher;
    }

    EventDispatcher(const EventDispatcher&) = delete;
    EventDispatcher(EventDispatcher&&) = delete;
    EventDispatcher& operator=(const EventDispatcher&) = delete;
    EventDispatcher& operator=(EventDispatcher&&) = delete;

    void initialize();
    void wait_for_event();
    void handle_events();

public:
    bool register_observer(EventObserver* observer) override;
    bool unregister_observer(EventObserver* observer) override;
    void emit_event(const Event& event) override;
    void emit_event_delayed(const Event& event, uint32_t ms_delay) override;

private:
    EventDispatcher();

    static void handle_timer_event(void *context);
    static void handle_app_event(void *event_data, uint16_t event_size);

    static constexpr size_t MAX_EVENTS_COUNT = 16;
    static constexpr size_t MAX_OBSERVERS_COUNT = 4;
    static constexpr size_t MAX_TIMERS_COUNT = 8;
    
    struct TimerState {
        TimerState() : timer {0}, timer_id {nullptr} {}
        app_timer_t timer;
        app_timer_id_t timer_id;
        Event event;
        std::atomic<bool> used;
    };

    std::array<EventObserver*, MAX_OBSERVERS_COUNT> observers;
    std::array<TimerState, MAX_TIMERS_COUNT> timers;
};

#endif // LAP_TIMER_EVENT_DISPATCHER_H