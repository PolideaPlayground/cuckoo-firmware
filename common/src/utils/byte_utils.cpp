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

#include "utils/byte_utils.h"

void byte_buffer_to_hex(const uint8_t *byte_buffer, size_t byte_buffer_len, char *hex_buffer, size_t hex_buffer_len) {
    size_t hex_offset = 0;
    for (size_t i = 0; i < byte_buffer_len && hex_offset + 2 < hex_buffer_len; i++, hex_offset += 2) {
        uint8_t hi_nibble = (byte_buffer[i] & 0xF0) >> 4;
        hex_buffer[hex_offset] = hi_nibble >= 10 ? 'A' - 10 + hi_nibble : '0' + hi_nibble;
        uint8_t lo_nibble = byte_buffer[i] & 0x0F;
        hex_buffer[hex_offset + 1] = lo_nibble >= 10 ? 'A' - 10 + lo_nibble : '0' + lo_nibble;
    }
    if (hex_offset < hex_buffer_len) {
        hex_buffer[hex_offset] = '\0';   
    }
}