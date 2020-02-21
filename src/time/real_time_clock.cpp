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

#include "time/real_time_clock.h"
#include <nrf_rtc.h>
#include <nrfx_rtc.h>

//Dumb handler, needed because nrfx API requires one
static void rtc_handler(nrfx_rtc_int_type_t int_type) {}

RealTimeClock::RealTimeClock() :
rtc_counter(NRFX_RTC_INSTANCE(2)) {
    initalize_rtc();
}

RealTimeClock& RealTimeClock::get_instance() {
    static RealTimeClock clock;
    return clock;
}

uint32_t RealTimeClock::get_current_timestamp_ms() const {
    uint64_t counter_value = nrf_rtc_counter_get(rtc_counter.p_reg);
    return 1000 * counter_value / RTC_COUNTER_FREQUENCY;
}

void RealTimeClock::initalize_rtc() {
    nrfx_rtc_config_t rtc_config = {
        .prescaler          = RTC_FREQ_TO_PRESCALER(RTC_COUNTER_FREQUENCY),
        .interrupt_priority = NRFX_RTC_DEFAULT_CONFIG_IRQ_PRIORITY,
        .tick_latency       = NRFX_RTC_US_TO_TICKS(NRFX_RTC_MAXIMUM_LATENCY_US, RTC_COUNTER_FREQUENCY),
        .reliable           = false,
    };

    APP_ERROR_CHECK(nrfx_rtc_init(&rtc_counter, &rtc_config, rtc_handler));
    nrfx_rtc_enable(&rtc_counter);
}