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

#ifndef LAP_TIMER_MOCK_EVENT_DISPATCHER_H
#define LAP_TIMER_MOCK_EVENT_DISPATCHER_H

#include "events/event_dispatcher_interface.h"
#include "events/event_observer.h"

#include <unordered_set>
#include <optional>
#include <deque>

class MockEventDispatcher : public EventDispatcherInterface {
public:
    std::optional<Event> process_next_event();

    bool register_observer(EventObserver* observer) override;
    bool unregister_observer(EventObserver* observer) override;

public:
    void emit_event(const Event& event) override;
    void emit_event_delayed(const Event& event, uint32_t ms_delay) override;

private:

    class EventState {
    public:
        EventState(const Event& event, size_t delay) : event(event), delay(delay) {}
        
        Event get_event() const {
            return event;
        }

        size_t get_delay() const {
            return delay;
        }

        void substract_delay(size_t delay) {
            this->delay -= delay;
        }

        bool operator<(const EventState& other) const {
            return delay < other.delay;
        }

    private:
        Event event;
        size_t delay;
    };

    std::unordered_set<EventObserver*> observers;
    std::deque<EventState> events_queue;
};

#endif // LAP_TIMER_MOCK_EVENT_DISPATCHER_H