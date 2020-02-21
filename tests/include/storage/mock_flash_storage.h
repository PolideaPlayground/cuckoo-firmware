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

#ifndef LAP_TIMER_MOCK_FLASH_STORAGE_H
#define LAP_TIMER_MOCK_FLASH_STORAGE_H

#include "storage/flash_storage_interface.h"

#include <cstdint>
#include <unordered_map>
#include <vector>

class MockFlashStorage : public FlashStorageInterface {
public:
    MockFlashStorage(uint16_t record_capacity);

public:
    void initialize();

    uint16_t get_total_records() {
        return total_records;
    }

    uint16_t get_record_capacity() {
        return record_capacity;
    }

public:
    void set_delegate(Delegate *delegate) override;

    bool collect_garbage() override;
    bool delete_all_files() override;

    bool delete_file(uint16_t file_id) override;
    bool delete_record(uint16_t file_id, uint16_t record_id) override;

    bool read_record(uint16_t file_id, uint16_t record_id, uint32_t* data, uint16_t *words_count) override;
    bool write_record(uint16_t file_id, uint16_t record_id, const uint32_t* data, uint16_t words_count) override;

    bool iterate_records(std::function<void(uint16_t file_id, uint16_t record_id, const uint32_t* record_data, uint16_t record_data_length)> callback) override;

private:
    using RecordMap = std::unordered_map<uint16_t, std::vector<uint32_t>>;
    using FileMap = std::unordered_map<uint16_t, RecordMap>;
    
    FileMap file_map;
    Delegate* delegate;
    uint16_t total_records;
    uint16_t record_capacity;
};

#endif // LAP_TIMER_MOCK_FLASH_STORAGE_H