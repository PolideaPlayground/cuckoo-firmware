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
#include "storage/mock_flash_storage.h"

MockFlashStorage::MockFlashStorage(uint16_t record_capacity) : delegate(nullptr), total_records(0), record_capacity(record_capacity) {
}

void MockFlashStorage::set_delegate(Delegate *delegate) {
    this->delegate = delegate;
}

void MockFlashStorage::initialize() {
    if (delegate) {
        delegate->on_initialized(true, *this);
    }
}

bool MockFlashStorage::collect_garbage() {
    if (delegate) {
        delegate->on_garbage_collected(true);
    }
    return true;
}

bool MockFlashStorage::delete_all_files() {
    total_records = 0;
    file_map.clear();
    if (delegate) {
        delegate->on_all_files_deleted(true);
    }
    return true;
}

bool MockFlashStorage::delete_file(uint16_t file_id) {
    bool deleted = false;
    auto it = file_map.find(file_id);
    
    if (it != file_map.end()) {
        deleted = true;
        total_records -= it->second.size();
        file_map.erase(file_id);
    }

    if (delegate) {
        delegate->on_file_deleted(deleted, file_id);
    }

    return true;
}

bool MockFlashStorage::delete_record(uint16_t file_id, uint16_t record_id) {
    bool deleted = false;
    auto it = file_map.find(file_id);
    if (it != file_map.end()) {
        RecordMap& record_map = it->second;
        if (record_map.erase(record_id) > 0) {
            deleted = true;
            total_records -= 1;
        }
    }

    if (delegate) {
        delegate->on_record_deleted(deleted, file_id, record_id);
    }

    return true;
}

bool MockFlashStorage::read_record(uint16_t file_id, uint16_t record_id, uint32_t* data, uint16_t *words_count) {
    bool read = false;
    auto file_it = file_map.find(file_id);
    if (file_it != file_map.end()) {
        auto record_it = file_it->second.find(record_id);
        if (record_it != file_it->second.end()) {
            std::vector<uint32_t>& record_data = record_it->second;
            *words_count = std::min(*words_count, static_cast<uint16_t>(record_data.size()));
            std::memcpy(data, record_data.data(), *words_count * 4);
            read = true;
        }
    }

    return read;
}

bool MockFlashStorage::write_record(uint16_t file_id, uint16_t record_id, const uint32_t* data, uint16_t words_count) {
    bool wrote = true;
    RecordMap& record_map = file_map[file_id];
    if (record_map.find(record_id) == record_map.end()) {
        if (total_records >= record_capacity) {
            wrote = false;
        } else {
            total_records++;
        }
    }

    if (wrote) {
        std::vector<uint32_t>& record_data = record_map[record_id];
        record_data.resize(std::max(words_count, static_cast<uint16_t>(record_data.size())));
        std::memcpy(record_data.data(), data, words_count * 4);
    }

    if (delegate) {
        delegate->on_record_written(wrote, file_id, record_id);
    }

    return true;
}

bool MockFlashStorage::iterate_records(std::function<void(uint16_t file_id, uint16_t record_id, const uint32_t* record_data, uint16_t record_data_length)> callback) {
    for (auto file_it = file_map.begin(); file_it != file_map.end(); file_it++) {
        const RecordMap& record_map = file_it->second;
        for (auto record_it = record_map.begin(); record_it != record_map.end(); record_it++) {
            callback(file_it->first, record_it->first, record_it->second.data(), static_cast<uint16_t>(record_it->second.size()));
        }
    }
    return true;
}

// TESTS

class MockStorageDelegate : public FlashStorageInterface::Delegate {
public:
    MockStorageDelegate() :
    on_initialized_count(0),
    on_initialized_success(false),
    on_garbage_collected_count(0),
    on_garbage_collected_success(false),
    on_all_files_deleted_count(0),
    on_all_files_deleted_success(false),
    on_file_deleted_count(0),
    on_file_deleted_success(false),
    on_record_deleted_count(0),
    on_record_deleted_success(false),
    on_record_written_count(0),
    on_record_written_success(false)
    {}

    void on_initialized(bool successful, FlashStorageInterface& interface) override {
        on_initialized_count++;
        on_initialized_success = successful;
    }

    void on_garbage_collected(bool successful) override {
        on_garbage_collected_count++;
        on_garbage_collected_success = successful;
    }

    void on_all_files_deleted(bool successful) override {
        on_all_files_deleted_count++;
        on_all_files_deleted_success = successful;
    }

    void on_file_deleted(bool successful, uint16_t file_id) override {
        on_file_deleted_count++;
        on_file_deleted_success = successful;
    }

    void on_record_deleted(bool successful, uint16_t file_id, uint16_t record_id) override {
        on_record_deleted_count++;
        on_record_deleted_success = successful;
    }

    void on_record_written(bool successful, uint16_t file_id, uint16_t record_id) override {
        on_record_written_count++;
        on_record_written_success = successful;
    }
    
    int on_initialized_count;
    bool on_initialized_success;
    int on_garbage_collected_count;
    bool on_garbage_collected_success;
    int on_all_files_deleted_count;
    bool on_all_files_deleted_success;
    int on_file_deleted_count;
    bool on_file_deleted_success;
    int on_record_deleted_count;
    bool on_record_deleted_success;
    int on_record_written_count;
    bool on_record_written_success;
};

TEST_CASE("Mock flash storage is properly implemented", "[flash_storage]") {
    MockFlashStorage flash_storage(3);
    MockStorageDelegate delegate;
    flash_storage.set_delegate(&delegate);
    flash_storage.initialize();
    REQUIRE(delegate.on_initialized_count == 1);
    REQUIRE(delegate.on_initialized_success);

    REQUIRE(flash_storage.collect_garbage());
    REQUIRE(delegate.on_garbage_collected_count == 1);
    REQUIRE(delegate.on_garbage_collected_success);
    REQUIRE(flash_storage.get_total_records() == 0);

    uint32_t data = 0xFFAABB01;
    REQUIRE(flash_storage.write_record(1, 1, &data, 1));
    REQUIRE(delegate.on_record_written_count == 1);
    REQUIRE(delegate.on_record_written_success);
    REQUIRE(flash_storage.get_total_records() == 1);

    data = 0xFFAABB02;
    REQUIRE(flash_storage.write_record(1, 2, &data, 1));
    REQUIRE(delegate.on_record_written_count == 2);
    REQUIRE(delegate.on_record_written_success);
    REQUIRE(flash_storage.get_total_records() == 2);

    data = 0xFFAABBFF;
    REQUIRE(flash_storage.write_record(2, 1, &data, 1));
    REQUIRE(delegate.on_record_written_count == 3);
    REQUIRE(delegate.on_record_written_success);
    REQUIRE(flash_storage.get_total_records() == 3);

    data = 0xFFAABB03;
    REQUIRE(flash_storage.write_record(2, 1, &data, 1));
    REQUIRE(delegate.on_record_written_count == 4);
    REQUIRE(delegate.on_record_written_success);
    REQUIRE(flash_storage.get_total_records() == 3);

    data = 0xFFAABB04;
    REQUIRE(flash_storage.write_record(2, 2, &data, 1));
    REQUIRE(delegate.on_record_written_count == 5);
    REQUIRE(!delegate.on_record_written_success);
    REQUIRE(flash_storage.get_total_records() == 3);

    uint16_t size = 1;
    REQUIRE(!flash_storage.read_record(2, 10, &data, &size));
    REQUIRE(flash_storage.read_record(2, 1, &data, &size));
    REQUIRE(size == 1);
    REQUIRE(data == 0xFFAABB03);

    REQUIRE(flash_storage.delete_record(2, 1));
    REQUIRE(delegate.on_record_deleted_count == 1);
    REQUIRE(delegate.on_record_deleted_success);
    REQUIRE(flash_storage.get_total_records() == 2);

    REQUIRE(flash_storage.delete_record(2, 1));
    REQUIRE(delegate.on_record_deleted_count == 2);
    REQUIRE(!delegate.on_record_deleted_success);
    REQUIRE(flash_storage.get_total_records() == 2);

    uint16_t expected_count = 0;
    REQUIRE(flash_storage.iterate_records([&expected_count](uint16_t file_id, uint16_t record_id, const uint32_t* data, uint16_t length) {
        expected_count++;
        REQUIRE(file_id == 1);
        REQUIRE(*data == (0xFFAABB00 | record_id));
        REQUIRE(length == 1);
    }));
    REQUIRE(expected_count == 2);

    REQUIRE(flash_storage.delete_file(1));
    REQUIRE(delegate.on_file_deleted_count == 1);
    REQUIRE(delegate.on_file_deleted_success);
    REQUIRE(flash_storage.get_total_records() == 0);

    REQUIRE(flash_storage.delete_file(1));
    REQUIRE(delegate.on_file_deleted_count == 2);
    REQUIRE(!delegate.on_file_deleted_success);
    REQUIRE(flash_storage.get_total_records() == 0);

    expected_count = 0;
    REQUIRE(flash_storage.iterate_records([&expected_count](uint16_t file_id, uint16_t record_id, const uint32_t* data, uint16_t length) {
        expected_count++;
    }));
    REQUIRE(expected_count == 0);
}