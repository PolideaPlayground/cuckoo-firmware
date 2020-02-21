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

#include "catch.hpp"
#include "events/mock_event_dispatcher.h"
#include <algorithm>

std::optional<Event> MockEventDispatcher::process_next_event() {
    if (events_queue.empty()) {
        return std::nullopt;
    }

    EventState first_event_state = events_queue.front();
    for (EventState& event_state: events_queue) {
        event_state.substract_delay(first_event_state.get_delay());
    }
    events_queue.pop_front();

    for (EventObserver *observer : observers) {
        if (observer) {
            observer->on_event(first_event_state.get_event());
        }
    }

    return first_event_state.get_event();
}

bool MockEventDispatcher::register_observer(EventObserver* observer) {
    observers.insert(observer);
    return true;
}

bool MockEventDispatcher::unregister_observer(EventObserver* observer) {
    observers.erase(observer);
    return true;
}

void MockEventDispatcher::emit_event(const Event& event) {
    events_queue.push_back(EventState(event, 0));
    std::stable_sort(events_queue.begin(), events_queue.end());
}

void MockEventDispatcher::emit_event_delayed(const Event& event, uint32_t ms_delay) {
    events_queue.push_back(EventState(event, ms_delay));
    std::stable_sort(events_queue.begin(), events_queue.end());
}

// TESTS ----------------------------------------------------------------------

class MockEventObserver : public EventObserver {
public:
    MockEventObserver(EventDispatcherInterface& dispatcher) : dispatcher(dispatcher), last_event(std::nullopt) {}

    void on_event(const Event& event) override {
        last_event = event;    
    }    

    std::optional<Event> get_last_event() const {
        return last_event;
    }

private:
    EventDispatcherInterface& dispatcher;
    std::optional<Event> last_event;
};

std::optional<Event> make_event(const Event& event) {
    return event;
}

TEST_CASE("mock event dispatcher is implemented properly", "[event_dispatcher]") {
    MockEventDispatcher dispatcher;
    MockEventObserver observer(dispatcher);
    REQUIRE(dispatcher.register_observer(&observer));
    
    dispatcher.emit_event(FlashLED{0, 0});
    dispatcher.emit_event(FlashLED{0, 1});
    dispatcher.emit_event(FlashLED{0, 2});

    REQUIRE(observer.get_last_event() == std::nullopt);
    REQUIRE(dispatcher.process_next_event() == make_event(FlashLED{0, 0}));
    REQUIRE(observer.get_last_event() == make_event(FlashLED{0, 0}));
    REQUIRE(dispatcher.process_next_event() == make_event(FlashLED{0, 1}));
    REQUIRE(observer.get_last_event() == make_event(FlashLED{0, 1}));
    REQUIRE(dispatcher.process_next_event() == make_event(FlashLED{0, 2}));
    REQUIRE(observer.get_last_event() == make_event(FlashLED{0, 2}));
    REQUIRE(dispatcher.process_next_event() == std::nullopt);

    dispatcher.emit_event_delayed(FlashLED{0, 0}, 1000);
    dispatcher.emit_event_delayed(FlashLED{0, 1}, 200);
    dispatcher.emit_event_delayed(FlashLED{1, 3}, 1300);
    dispatcher.emit_event(FlashLED{0, 2});
    dispatcher.emit_event(FlashLED{2, 2});
    dispatcher.emit_event_delayed(FlashLED{0, 3}, 300);

    REQUIRE(dispatcher.process_next_event() == make_event(FlashLED{0, 2}));
    REQUIRE(dispatcher.process_next_event() == make_event(FlashLED{2, 2}));
    REQUIRE(dispatcher.process_next_event() == make_event(FlashLED{0, 1}));
    REQUIRE(dispatcher.process_next_event() == make_event(FlashLED{0, 3}));
    REQUIRE(dispatcher.process_next_event() == make_event(FlashLED{0, 0}));
    REQUIRE(dispatcher.process_next_event() == make_event(FlashLED{1, 3}));
    REQUIRE(dispatcher.process_next_event() == std::nullopt);
}