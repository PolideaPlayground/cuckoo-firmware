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

#ifndef LAPTIMER_COMMON_CONFIG_H
#define LAPTIMER_COMMON_CONFIG_H

#include "nrf_log.h"

// Common library logs
#ifndef LOG_ERROR
#define LOG_ERROR(...) NRF_LOG_ERROR(__VA_ARGS__) 
#endif

#ifndef LOG_WARNING
#define LOG_WARNING(...) NRF_LOG_WARNING(__VA_ARGS__) 
#endif

#ifndef LOG_INFO
#define LOG_INFO(...) NRF_LOG_INFO(__VA_ARGS__) 
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG(...) NRF_LOG_DEBUG(__VA_ARGS__) 
#endif

#endif // LAPTIMER_COMMON_CONFIG_H

