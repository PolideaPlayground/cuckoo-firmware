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

#ifndef LAP_TIMER_QUEUE_H
#define LAP_TIMER_QUEUE_H

#include <cstdint>

///
/// @brief Statically allocated queue.
/// 
/// @tparam T Object type.
/// @tparam N Number of maximum objects in the queue.
///
template<typename T, uint8_t N>
class Queue {
public:
    Queue() : head(0), tail(0), full(false) {}

    ///
    /// @brief Returns maximum number of elements.
    /// 
    /// @return uint8_t Number of elements.
    ///
    uint8_t capacity() const {
        return N;
    }

    ///
    /// @brief Checks if queue is full.
    /// 
    /// @return true Queue is full.
    /// @return false Queue is not full.
    ///
    bool is_full() const {
        return full;
    }

    ///
    /// @brief Checks if queue is empty.
    /// 
    /// @return true Queue is empty
    /// @return false Queue is not empty.
    ///
    bool is_empty() const {
        return !full && head == tail;
    }

    ///
    /// @brief Get the first object, which is about to be popped. Make sure to check if
    ///        queue is not empty before that operation.
    /// 
    /// @return T& First object.
    ///
    T& get_first() {
        return data[tail];
    }

    ///
    /// @brief Get the last object, which was most recently pushed. Make sure to check if
    ///        queue is not empty before that operation.
    /// 
    /// @return T& 
    ///
    T& get_last() {
        uint8_t index = head == 0 ? N - 1 : head - 1;
        return data[index];
    }

    ///
    /// @brief Push new object to the queue.
    /// 
    /// @param value Object to be pushed.
    /// @return true Object was successfuly pushed.
    /// @return false There is no space in the queue to push the object.
    ///
    bool push(T value) {
        if (is_full()) {
            return false;
        }
        data[head] = value;
        head = (head + 1) % N;
        full = head == tail;
        return true;
    }

    ///
    /// @brief Pops object from the queue.
    /// 
    /// @return true Object was successfully popped.
    /// @return false Queue was already empty.
    ///
    bool pop() {
        if (is_empty()) {
            return false;
        }
        full = false;
        tail = (tail + 1) % N;
        return true;
    }

    ///
    /// @brief Returns current size of a queue.
    /// 
    /// @return uint8_t 
    ///
    uint8_t size() const {
        if (is_full()) {
            return N;
        }

        if (head >= tail) {
            return head - tail;
        } else {
            return (N - tail) + head;
        }
    }

private:
    T data[N];
    uint8_t head;
    uint8_t tail;
    bool full;
};

#endif // LAP_TIMER_QUEUE_H