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
#include "utils/queue.h"

TEST_CASE("Queue properly pushes and pops elements", "[queue]") {
    Queue<int, 4> queue; 
    REQUIRE(queue.is_empty());
    REQUIRE(!queue.is_full());

    REQUIRE(queue.push(1));
    REQUIRE(queue.size() == 1);
    REQUIRE(queue.get_first() == 1);
    REQUIRE(queue.get_last() == 1);

    REQUIRE(queue.push(2));
    REQUIRE(queue.size() == 2);
    REQUIRE(queue.get_first() == 1);
    REQUIRE(queue.get_last() == 2);

    REQUIRE(queue.push(3));
    REQUIRE(queue.size() == 3);
    REQUIRE(queue.get_first() == 1);
    REQUIRE(queue.get_last() == 3);

    REQUIRE(queue.push(4));
    REQUIRE(queue.size() == 4);
    REQUIRE(queue.get_first() == 1);
    REQUIRE(queue.get_last() == 4);

    REQUIRE(!queue.push(5));
    REQUIRE(queue.size() == 4);
    REQUIRE(queue.get_first() == 1);
    REQUIRE(queue.get_last() == 4);
    REQUIRE(!queue.is_empty());
    REQUIRE(queue.is_full());

    REQUIRE(queue.pop());
    REQUIRE(queue.size() == 3);
    REQUIRE(queue.get_first() == 2);
    REQUIRE(queue.get_last() == 4);
    REQUIRE(!queue.is_empty());
    REQUIRE(!queue.is_full());

    REQUIRE(queue.pop());
    REQUIRE(queue.size() == 2);
    REQUIRE(queue.get_first() == 3);
    REQUIRE(queue.get_last() == 4);

    REQUIRE(queue.pop());
    REQUIRE(queue.size() == 1);
    REQUIRE(queue.get_first() == 4);
    REQUIRE(queue.get_last() == 4);

    REQUIRE(queue.pop());
    REQUIRE(queue.size() == 0);
    REQUIRE(queue.is_empty());
    REQUIRE(!queue.is_full());

    REQUIRE(!queue.pop());
    REQUIRE(queue.size() == 0);
    REQUIRE(queue.is_empty());
    REQUIRE(!queue.is_full());
}

TEST_CASE("Queue properly pushes and pops elements with sliding window", "[queue]") {
    Queue<int, 255> queue;

    // Fill half of the queue.
    for (uint8_t i = 0; i < 128; i++) {
        REQUIRE(queue.push(i));
    }
    REQUIRE(queue.size() == 128);

    // Push and pop elements.
    for (uint8_t i = 128; i < 255; i++) {
        REQUIRE(queue.get_first() == i - 128);
        REQUIRE(queue.get_last() == i - 1);
        REQUIRE(queue.push(i));
        REQUIRE(queue.pop());
        REQUIRE(queue.size() == 128);
    }

    // Empty queue.
    for (uint8_t i = 127; i < 255; i++) {
         REQUIRE(queue.get_first() == i);
         REQUIRE(queue.pop());
    }
    
    REQUIRE(queue.is_empty());
}