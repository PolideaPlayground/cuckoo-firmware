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

#include "events/event_dispatcher.h"
#include "app_scheduler.h"
#include "nrf_soc.h"
#include "nrf_log.h"

EventDispatcher::EventDispatcher() {
    for (size_t i = 0; i < MAX_OBSERVERS_COUNT; i++) {
        observers[i] = nullptr;
    }
}

void EventDispatcher::initialize() {
    APP_SCHED_INIT(sizeof(Event), MAX_EVENTS_COUNT);
    APP_ERROR_CHECK(app_timer_init());

    for (size_t i = 0; i < MAX_TIMERS_COUNT; i++) {
        timers[i].timer_id = &timers[i].timer;
        APP_ERROR_CHECK(app_timer_create(&timers[i].timer_id, APP_TIMER_MODE_SINGLE_SHOT, EventDispatcher::handle_timer_event));
    }
}

void EventDispatcher::handle_events() {
    app_sched_execute();
}

void EventDispatcher::wait_for_event() {
    auto error_code = sd_app_evt_wait();
    APP_ERROR_CHECK(error_code);
}

void EventDispatcher::handle_timer_event(void *context) {
    TimerState* timer_state = reinterpret_cast<TimerState*>(context);
    timer_state->used.store(false);
    APP_ERROR_CHECK(app_sched_event_put(&timer_state->event, sizeof(Event), EventDispatcher::handle_app_event));
}

void EventDispatcher::handle_app_event(void *event_data, uint16_t event_size) {
    const Event* event = reinterpret_cast<const Event*>(event_data);
    EventDispatcher& event_dispatcher = EventDispatcher::get_instance();
    for (size_t i = 0; i < MAX_OBSERVERS_COUNT; i++) {
        EventObserver* observer = event_dispatcher.observers[i];
        if (observer) {
            observer->on_event(*event);
        }
    }
}

bool EventDispatcher::register_observer(EventObserver* observer) {
    bool registered = false;
    for (size_t i = 0; i < MAX_OBSERVERS_COUNT; i++) {
        if (!observers[i]) {
            observers[i] = observer;
            registered = true;
            break;
        }
    }
    return registered;
}

bool EventDispatcher::unregister_observer(EventObserver* observer) {
    bool unregistered = false;
    for (size_t i = 0; i < MAX_OBSERVERS_COUNT; i++) {
        if (observers[i] == observer) {
            observers[i] = nullptr;
            unregistered = true;
            break;
        }
    }
    return unregistered;
}

void EventDispatcher::emit_event(const Event& event) {
    APP_ERROR_CHECK(app_sched_event_put(&event, sizeof(event), EventDispatcher::handle_app_event));
}

void EventDispatcher::emit_event_delayed(const Event& event, uint32_t ms_delay) {
    TimerState* timer_state = nullptr;
    size_t timer_id = 0;
    for (; timer_id < MAX_TIMERS_COUNT; timer_id++) {
        bool expected = false;
        if (timers[timer_id].used.compare_exchange_strong(expected, true)) {
            timer_state = &timers[timer_id];
            timer_state->event = event;
            break;
        }
    }
    
    // Make sure that we have a timer.
    APP_ERROR_CHECK_BOOL(timer_state != nullptr);
    APP_ERROR_CHECK(app_timer_start(timer_state->timer_id, APP_TIMER_TICKS(ms_delay), timer_state));
}