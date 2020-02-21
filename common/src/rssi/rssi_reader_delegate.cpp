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

#include "rssi/rssi_reader_delegate.h"
#include <algorithm>
#include <iterator>

#include <nrf_log.h>

static constexpr uint32_t CHECKPOINT_RSSI_THRESHOLD_COUNT = 200;
static constexpr uint32_t TRACK_RSSI_THRESHOLD_COUNT = 20000;
static constexpr uint32_t RSSI_THRESHOLD_VALUE = 36100;

RssiReaderDelegate::RssiReaderDelegate(RealTimeClockInterface& clock, EventDispatcherInterface& event_dispatcher):
    reader(nullptr),
    clock(clock),
    event_dispatcher(event_dispatcher),
    buffer{},
    current_index(0),
    in_checkpoint(false),
    checkpoint_threshold_counter(0),
    track_threshold_counter(0) {}

void RssiReaderDelegate::on_initialized(RssiReaderInterface &rssi_reader) {
    this->reader = &rssi_reader;
}

void RssiReaderDelegate::on_sample_captured(uint16_t sample) {
    static uint16_t sorted_buffer[MEDIAN_FILTER_ORDER];

    buffer[current_index] = sample;
    ++current_index %= MEDIAN_FILTER_ORDER;

    //for greater orders of median filter smarter median calculation should be implemented
    std::copy(std::begin(buffer), std::end(buffer), std::begin(sorted_buffer));
    std::sort(std::begin(sorted_buffer), std::end(sorted_buffer));

    uint16_t filtered_sample = sorted_buffer[MEDIAN_FILTER_ORDER/2];

    if (filtered_sample > RSSI_THRESHOLD_VALUE && !in_checkpoint && ++checkpoint_threshold_counter > CHECKPOINT_RSSI_THRESHOLD_COUNT) {
        in_checkpoint = true;
        uint32_t timestamp = clock.get_current_timestamp_ms();
        event_dispatcher.emit_event(NewLap(timestamp));
        NRF_LOG_INFO("NEW LAP EVENT: %u", timestamp);
        checkpoint_threshold_counter = 0;
        track_threshold_counter = 0;
    }
    else if (filtered_sample < RSSI_THRESHOLD_VALUE && in_checkpoint && ++track_threshold_counter > TRACK_RSSI_THRESHOLD_COUNT) {
        in_checkpoint = false;
        checkpoint_threshold_counter = 0;
        track_threshold_counter = 0;
    }
}

