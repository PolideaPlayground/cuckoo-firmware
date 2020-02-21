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


#include "rssi/rssi_reader.h"
#include <algorithm>
#include <nrf.h>
#include <nrf_saadc.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <nrf_log_default_backends.h>
#include <nrfx_ppi.h>
#include <nrf_ppi.h>
#include <nrfx_timer.h>
#include <nrf_uart.h>
#include <nrfx_uart.h>
#include <nrf_gpio.h>
#include <libraries/timer/app_timer.h>

// Dumb handlers just to force asynchronous peripheral mode
static void uart_handler(const nrfx_uart_event_t* event_type, void * p_context) {}
static void timer_handler(nrf_timer_event_t event_type, void * p_context) {}


RssiReader::RssiReader() :
delegate(nullptr),
timer(NRFX_TIMER_INSTANCE(1)),
uart(NRFX_UART_INSTANCE(0)) {}


void RssiReader::initialize(RssiReaderInterface::Delegate& delegate) {
    this->delegate = &delegate;

    initialize_adc();
    initialize_sampling_timer();
    initalize_uart_logger();
    delegate.on_initialized(*this);
    enable_sampling();
}


void RssiReader::initialize_sampling_timer() {
    nrfx_timer_config_t timer_cfg = {
            .frequency = NRF_TIMER_FREQ_16MHz,
            .mode = NRF_TIMER_MODE_TIMER,
            .bit_width = NRF_TIMER_BIT_WIDTH_8,
            .interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,
            .p_context = NULL,
    };

    APP_ERROR_CHECK(nrfx_timer_init(&timer, &timer_cfg, timer_handler));

    nrf_timer_shorts_enable(timer.p_reg, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK);

    nrfx_timer_compare(&timer, NRF_TIMER_CC_CHANNEL0, 100, false);

    nrfx_timer_enable(&timer);

    APP_ERROR_CHECK(nrfx_ppi_channel_alloc(&ppi_channel));

    APP_ERROR_CHECK(nrfx_ppi_channel_assign(
            ppi_channel,
            nrfx_timer_compare_event_address_get(&timer, NRF_TIMER_CC_CHANNEL0),
            nrfx_saadc_sample_task_get()
    ));
}

void RssiReader::enable_sampling() {
    APP_ERROR_CHECK(nrfx_ppi_channel_enable(ppi_channel));
}

void RssiReader::handle_adc_event(nrfx_saadc_evt_t const * p_event) {
    if (!get_instance().handle_adc_event_impl(p_event)) {
        NRF_LOG_WARNING("Unhandled ADC event: %u", p_event->type);
    }
}


bool RssiReader::handle_adc_event_impl(nrfx_saadc_evt_t const * p_event) {
    if (p_event->type == NRFX_SAADC_EVT_DONE) {
        APP_ERROR_CHECK(nrfx_saadc_buffer_convert(p_event->data.done.p_buffer, SAMPLES_IN_BUFFER));
        uint16_t sample = std::max(static_cast<int16_t>(0), p_event->data.done.p_buffer[0]);
        log_sample(sample);

        // The sample is shifted by two bits to normalize the value to 16bits precision.
        delegate->on_sample_captured(sample << 2u);
        return true;
    }
    return false;
}


void RssiReader::log_sample(uint16_t sample) {
    // The sample data is 14 bits long - two most significant bits of sample parameter should be equal to zero.
    // Therefore we can concatenate two lsb bits of sequence number with the 14bits of sample data. The result is saved in
    // uint8_t array (big-endian) and transmited over uart. The aim of the SEQ bits is to allow validation on the receiver side.

    ++sample_seq;
    uint8_t data[2] = {
            static_cast<uint8_t>(((sample_seq << 6u) & 0xc0u)|((sample >> 8u) & 0x3fu)), // 2 bits of seq and 6 msb of sample
            static_cast<uint8_t>(sample & 0xffu) // 8 lsb of sample
    };
    nrfx_uart_tx(&uart, data, 2);
}

void RssiReader::initialize_adc()
{
    nrfx_saadc_config_t saadc_config = {
            .resolution         = NRF_SAADC_RESOLUTION_14BIT,
            .oversample         = NRF_SAADC_OVERSAMPLE_64X,
            .interrupt_priority = NRFX_SAADC_CONFIG_IRQ_PRIORITY,
            .low_power_mode     = NRFX_SAADC_CONFIG_LP_MODE,
    };

    nrf_saadc_channel_config_t channel_config = {
            .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
            .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
            .gain       = NRF_SAADC_GAIN1_2,
            .reference  = NRF_SAADC_REFERENCE_INTERNAL,
            .acq_time   = NRF_SAADC_ACQTIME_5US,
            .mode       = NRF_SAADC_MODE_SINGLE_ENDED,
            .burst      = NRF_SAADC_BURST_DISABLED,
            .pin_p      = NRF_SAADC_INPUT_AIN7,
            .pin_n      = NRF_SAADC_INPUT_DISABLED,
    };

    APP_ERROR_CHECK(nrfx_saadc_init(&saadc_config, handle_adc_event));
    APP_ERROR_CHECK(nrfx_saadc_channel_init(0, &channel_config));
    APP_ERROR_CHECK(nrfx_saadc_buffer_convert(buffer_pool[0], SAMPLES_IN_BUFFER));
    APP_ERROR_CHECK(nrfx_saadc_buffer_convert(buffer_pool[1], SAMPLES_IN_BUFFER));
}


void RssiReader::initalize_uart_logger() {
    nrfx_uart_config_t uart_config = {
            .pseltxd            = NRF_GPIO_PIN_MAP(1, 10),
            .pselrxd            = NRF_UART_PSEL_DISCONNECTED,
            .pselcts            = NRF_UART_PSEL_DISCONNECTED,
            .pselrts            = NRF_UART_PSEL_DISCONNECTED,
            .p_context          = nullptr,
            .hwfc               = (nrf_uart_hwfc_t)NRFX_UART_DEFAULT_CONFIG_HWFC,
            .parity             = (nrf_uart_parity_t)NRFX_UART_DEFAULT_CONFIG_PARITY,
            .baudrate           = (nrf_uart_baudrate_t)NRFX_UART_DEFAULT_CONFIG_BAUDRATE,
            .interrupt_priority = NRFX_UART_DEFAULT_CONFIG_IRQ_PRIORITY
    };

    APP_ERROR_CHECK(nrfx_uart_init(&uart, &uart_config, uart_handler));
    nrf_uart_enable(uart.p_reg);
}
