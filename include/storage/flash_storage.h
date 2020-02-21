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

#ifndef LAP_TIMER_FLASH_STORAGE_H
#define LAP_TIMER_FLASH_STORAGE_H

#include "fds.h"
#include "storage/flash_storage_interface.h"
#include <functional>

class FlashStorage : public FlashStorageInterface {
public:
    static FlashStorage& get_instance() {
        static FlashStorage flash_storage;
        return flash_storage;
    }

    FlashStorage(const FlashStorage&) = delete;
    FlashStorage(FlashStorage&&) = delete;
    FlashStorage& operator=(const FlashStorage&) = delete;
    FlashStorage& operator=(FlashStorage&&) = delete;

    void initialize();

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
    FlashStorage();

    static void handle_flash_storage_event(fds_evt_t const * p_evt);
    void handle_flash_storage_event(const fds_evt_t &event);

    void delete_next_record();

    Delegate *delegate;
    bool delete_all_files_pending;
};

#endif // LAP_TIMER_FLASH_STORAGE_H