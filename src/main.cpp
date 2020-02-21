/* MIT License

Copyright (c) 2019 Polidea

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#include "nrf_delay.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include <stdbool.h>
#include <stdint.h>

#include "ble/ble_manager.h"
#include "ble/ble_manager_delegate.h"
#include "events/event_dispatcher.h"
#include "events/event_observer.h"

#include "storage/session_storage.h"
#include "storage/flash_storage.h"

#include "rssi/rssi_reader.h"
#include "rssi/rssi_reader_delegate.h"

static void initialize_logger() {
    APP_ERROR_CHECK(NRF_LOG_INIT(app_timer_cnt_get));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

// Event Observer Demo.
class LEDObserver : public EventObserver {
public:
    LEDObserver(EventDispatcher& event_dispatcher) : event_dispatcher(event_dispatcher) {
        event_dispatcher.register_observer(this);
        event_dispatcher.emit_event(FlashLED(0, 1));
    }

    void on_event(const Event& event) override {
        std::visit(overloaded{
            [this](const FlashLED& data) {
                bsp_board_led_invert(data.get_led_id());
                uint8_t new_led_id = data.get_led_id();
                uint8_t new_ms_delay = data.get_ms_delay();
                if (new_ms_delay == 64) {
                    new_ms_delay = 1;
                    new_led_id = (new_led_id + 1) % LEDS_NUMBER;
                } else {
                    new_ms_delay++;
                }
                event_dispatcher.emit_event_delayed(FlashLED(new_led_id, new_ms_delay), new_ms_delay);
                ASSERT(false);
            },
            [](auto other) {

            }
        }, event);
    }

private:
    EventDispatcher &event_dispatcher;
};

int main(void) {
    initialize_logger();
    bsp_board_init(BSP_INIT_LEDS);

    EventDispatcher& event_dispatcher = EventDispatcher::get_instance();
    event_dispatcher.initialize();

    LEDObserver observer(event_dispatcher);

    BleManager &ble_manager = BleManager::get_instance();
    BleManagerDelegate<NRF_SDH_BLE_PERIPHERAL_LINK_COUNT> ble_delegate;
    ble_manager.initialize(ble_delegate);

    FlashStorage &flash_storage = FlashStorage::get_instance();
    SessionStorage session_storage(event_dispatcher, flash_storage);
    flash_storage.initialize();

    RssiReader &rssi_reader = RssiReader::get_instance();
    RssiReaderDelegate rssi_delegate(RealTimeClock::get_instance(), event_dispatcher);
    rssi_reader.initialize(rssi_delegate);

    while (true) {
        while(NRF_LOG_PROCESS());
        event_dispatcher.handle_events();
        event_dispatcher.wait_for_event();
    }
}