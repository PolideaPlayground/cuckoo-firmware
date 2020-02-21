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

#ifndef LAP_TIMER_RSSI_READER_DELEGATE_H
#define LAP_TIMER_RSSI_READER_DELEGATE_H

#include <nrf_log.h>

#include "rssi/rssi_reader_interface.h"
#include "time/real_time_clock_interface.h"
#include "events/event_dispatcher_interface.h"
#include "events/events.h"

class RssiReaderDelegate : public RssiReaderInterface::Delegate {
public:
    explicit RssiReaderDelegate(RealTimeClockInterface& clock, EventDispatcherInterface& event_dispatcher);
    void on_initialized(RssiReaderInterface& rssi_reader) override;
    void on_sample_captured(uint16_t sample) override;

private:
    static constexpr size_t MEDIAN_FILTER_ORDER = 5;
    RssiReaderInterface* reader;
    RealTimeClockInterface& clock;
    EventDispatcherInterface& event_dispatcher;
    uint16_t buffer[MEDIAN_FILTER_ORDER];
    size_t current_index;
    bool in_checkpoint;
    uint32_t checkpoint_threshold_counter;
    uint32_t track_threshold_counter;
};

#endif // LAP_TIMER_RSSI_READER_DELEGATE_H
