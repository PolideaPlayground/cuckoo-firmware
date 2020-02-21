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

#include "catch.hpp"
#include "utils/byte_utils.h"

TEST_CASE("Byte utils properly parses to the binary form", "[byte_utils]") {
    const size_t data_len = 4;
    const uint8_t data[data_len] = { 0xAA, 0x3D, 0x02, 0x80 };
    const size_t hex_len = 20;
    char hex[hex_len] = { '\0' };
    byte_buffer_to_hex(data, data_len, hex, hex_len);
    REQUIRE(strcmp(hex, "AA3D0280") == 0);
}

TEST_CASE("Byte utils properly parses truncated binary form", "[byte_utils]") {
    const size_t data_len = 4;
    const uint8_t data[data_len] = { 0xAA, 0x3D, 0x02, 0x80 };
    const size_t hex_len = 3;
    char hex[hex_len] = { '\0' };
    byte_buffer_to_hex(data, data_len, hex, hex_len);
    REQUIRE(strcmp(hex, "AA") == 0);
}

TEST_CASE("Byte utils properly parses truncated not even binary form", "[byte_utils]") {
    const size_t data_len = 4;
    const uint8_t data[data_len] = { 0xAA, 0x3D, 0x02, 0x80 };
    const size_t hex_len = 4;
    char hex[hex_len] = { '\0' };
    byte_buffer_to_hex(data, data_len, hex, hex_len);
    REQUIRE(strcmp(hex, "AA") == 0);
}

TEST_CASE("Byte utils properly parses 2 byte truncated binary form", "[byte_utils]") {
    const size_t data_len = 4;
    const uint8_t data[data_len] = { 0xAA, 0x3D, 0x02, 0x80 };
    const size_t hex_len = 5;
    char hex[hex_len] = { '\0' };
    byte_buffer_to_hex(data, data_len, hex, hex_len);
    REQUIRE(strcmp(hex, "AA3D") == 0);
}

TEST_CASE("Byte utils properly parses 2 bytes of data", "[byte_utils]") {
    const size_t data_len = 2;
    const uint8_t data[data_len] = { 0x02, 0x00 };
    const size_t hex_len = 5;
    char hex[hex_len] = { '\0' };
    byte_buffer_to_hex(data, data_len, hex, hex_len);
    REQUIRE(strcmp(hex, "0200") == 0);
}

TEST_CASE("Byte utils properly parses 4 bytes of data", "[byte_utils]") {
    const size_t data_len = 4;
    const uint8_t data[data_len] = { 0xff, 0xee, 0xdd, 0x33 };
    const size_t hex_len = 27;
    char hex[hex_len] = { '\0' };
    byte_buffer_to_hex(data, data_len, hex, hex_len);
    REQUIRE(strcmp(hex, "FFEEDD33") == 0);
}

