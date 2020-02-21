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

#ifndef LAP_TIMER_RSSI_READER_H
#define LAP_TIMER_RSSI_READER_H

#include "rssi/rssi_reader_interface.h"
#include <nrfx_saadc.h>
#include <nrfx_uart.h>
#include <nrfx_timer.h>
#include <nrf_ppi.h>

#include "time/real_time_clock.h"

class RssiReader : RssiReaderInterface {
public:
    RssiReader(const RssiReader&) = delete;
    RssiReader(RssiReader&&) = delete;
    RssiReader& operator=(const RssiReader&) = delete;
    RssiReader& operator=(RssiReader&&) = delete;
    virtual ~RssiReader() {}

    ///
    /// @brief Get global singleton instance
    ///
    /// @return RssiReader& singleton instance
    ///
    static RssiReader& get_instance() {
        static RssiReader reader;
        return reader;
    }

    ///
    /// @brief Initialize RssiReader with a delegate.
    ///
    /// @param delegate Delegate of RssiReader object.
    ///
    void initialize(RssiReaderInterface::Delegate& delegate) override;

private:
    RssiReader();

    void initialize_adc();
    void initialize_sampling_timer();
    void initalize_uart_logger();
    void enable_sampling();

    static void handle_adc_event(nrfx_saadc_evt_t const * p_event);
    bool handle_adc_event_impl(nrfx_saadc_evt_t const * p_event);
    void log_sample(uint16_t sample);

    Delegate* delegate;

    const nrfx_timer_t timer;
    const nrfx_uart_t uart;

    static constexpr size_t SAMPLES_IN_BUFFER = 1;
    nrf_saadc_value_t buffer_pool[2][SAMPLES_IN_BUFFER];
    nrf_ppi_channel_t ppi_channel;
    uint8_t sample_seq;
};

#endif //LAP_TIMER_RSSI_READER_H
