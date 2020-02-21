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

#ifndef LAP_TIMER_REAL_TIME_CLOCK_H
#define LAP_TIMER_REAL_TIME_CLOCK_H

#include <nrf.h>
#include <nrfx_timer.h>
#include <nrfx_rtc.h>
#include "time/real_time_clock_interface.h"

class RealTimeClock : public RealTimeClockInterface {
public:
    RealTimeClock(const RealTimeClock&) = delete;
    RealTimeClock(RealTimeClock&&) = delete;
    RealTimeClock& operator=(const RealTimeClock&) = delete;
    RealTimeClock& operator=(RealTimeClock&&) = delete;

    ///
    /// @brief Get global singleton instance
    ///
    /// @return RealTimeClock& singleton instance
    ///
    static RealTimeClock& get_instance(); /* */

    ///
    /// @brief Provides current timestamp in milliseconds since the Clock was initialized. Timestamp overflows
    /// after approximately 4.5 hours.
    ///
    /// @return uint32_t current timestamp
    ///
    uint32_t get_current_timestamp_ms() const override;

private:
    RealTimeClock();
    void initalize_rtc();

    const nrfx_rtc_t rtc_counter;
    static const uint16_t RTC_COUNTER_FREQUENCY = 1024;
};


#endif //LAP_TIMER_REAL_TIME_CLOCK_H
