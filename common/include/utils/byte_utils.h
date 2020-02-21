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

#ifndef LAP_TIMER_BYTE_UTILS_H
#define LAP_TIMER_BYTE_UTILS_H

#include <cstddef>
#include <cstdint>

void byte_buffer_to_hex(const uint8_t *byte_buffer, size_t byte_buffer_len, char *hex_buffer, size_t hex_buffer_len);

inline uint16_t read_uint16_le(const uint8_t *data) {
    return (static_cast<uint16_t>(data[0]) << 0) | 
           (static_cast<uint16_t>(data[1]) << 8);
}

inline void write_uint16_le(uint16_t value, uint8_t* data) {
    data[0] = (value >> 0) & 0xFF;
    data[1] = (value >> 8) & 0xFF;
}

inline uint32_t read_uint32_le(const uint8_t *data) {
    return (static_cast<uint32_t>(data[0]) << 0)  | 
           (static_cast<uint32_t>(data[1]) << 8)  | 
           (static_cast<uint32_t>(data[2]) << 16) |
           (static_cast<uint32_t>(data[3]) << 24);
}

inline void write_uint32_le(uint32_t value, uint8_t *data) {
    data[0] = (value >> 0) & 0xFF;
    data[1] = (value >> 8) & 0xFF;
    data[2] = (value >> 16) & 0xFF;
    data[3] = (value >> 24) & 0xFF;
}

#endif // LAP_TIMER_BYTE_UTILS_H