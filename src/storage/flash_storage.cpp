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

#include "storage/flash_storage.h"

#include "nrf_log.h"
#include "app_error.h"
#include "fds.h"

FlashStorage::FlashStorage() : delegate(nullptr), delete_all_files_pending(false) {
}

void FlashStorage::set_delegate(Delegate *delegate) {
    this->delegate = delegate;
}

void FlashStorage::initialize() {
    NRF_LOG_INFO("Registering Flash Storage handler...");
    APP_ERROR_CHECK(fds_register(FlashStorage::handle_flash_storage_event));

    NRF_LOG_INFO("Initializing Flash Storage handler...");
    APP_ERROR_CHECK(fds_init());
}

void FlashStorage::handle_flash_storage_event(fds_evt_t const * p_evt) {
    FlashStorage::get_instance().handle_flash_storage_event(*p_evt);
}

void FlashStorage::handle_flash_storage_event(const fds_evt_t &event) {
    switch (event.id) {
        case FDS_EVT_INIT: {
            NRF_LOG_INFO("FDS_EVT_INIT: result=%u", event.result);
            APP_ERROR_CHECK(event.result);
            if (delegate) {
                delegate->on_initialized(event.result == FDS_SUCCESS, *this);
            }
            break;
        }
        case FDS_EVT_WRITE: {
            NRF_LOG_INFO("FDS_EVT_WRITE: result=%u, file_id=%u, record_id=%u", event.result, event.write.file_id, event.write.record_key);
            if (delegate) {
                delegate->on_record_written(event.result == FDS_SUCCESS, event.write.file_id, event.write.record_key);
            }
            break;
        }
        case FDS_EVT_UPDATE: {
            NRF_LOG_INFO("FDS_EVT_UPDATE: result=%u, file_id=%u, record_id=%u", event.result, event.write.file_id, event.write.record_key);
            if (delegate) {
                delegate->on_record_written(event.result == FDS_SUCCESS, event.write.file_id, event.write.record_key);
            }
            break;
        }
        case FDS_EVT_DEL_RECORD: {
            NRF_LOG_INFO("FDS_EVT_DEL_RECORD: result=%u, file_id=%u, record_id=%u", event.result, event.del.file_id, event.del.record_key);
            if (delegate) {
                delegate->on_record_deleted(event.result == FDS_SUCCESS, event.del.file_id, event.del.record_key);
            }
            if (delete_all_files_pending) {
                delete_next_record();
            }
            break;
        }
        case FDS_EVT_DEL_FILE: {
            NRF_LOG_INFO("FDS_EVT_DEL_FILE: result=%u, file_id=%u, record_id=%u", event.result, event.del.file_id, event.del.record_key);
            if (delegate) {
                delegate->on_file_deleted(event.result == FDS_SUCCESS, event.del.file_id);
            }
            break;
        }
        case FDS_EVT_GC: {
            NRF_LOG_INFO("FDS_EVT_GC: result=%u", event.result);
            if (delegate) {
                delegate->on_garbage_collected(event.result == FDS_SUCCESS);
            }
            break;
        }
    }
}

bool FlashStorage::collect_garbage() {
    uint32_t error_code = fds_gc();
    if (error_code != NRF_SUCCESS) {
        NRF_LOG_WARNING("FlashStorage collect garbage failed: error_code=%u", error_code);
        return false;
    }

    return true;
}

bool FlashStorage::delete_all_files() {
    if (delete_all_files_pending) {
        NRF_LOG_WARNING("Delete all files operation is already in progress.");
        return false;
    }
    
    delete_all_files_pending = true;
    delete_next_record();
    return true;
}

void FlashStorage::delete_next_record() {
    fds_record_desc_t descriptor = {0};
    fds_find_token_t token = {0};

    uint32_t error_code = fds_record_iterate(&descriptor, &token);
    if (error_code == FDS_ERR_NOT_FOUND) {
        delete_all_files_pending = false;
        if (delegate) {
            delegate->on_all_files_deleted(true);
        }
        return;
    }

    if (error_code == FDS_SUCCESS) {
        error_code = fds_record_delete(&descriptor);
        if (error_code != FDS_SUCCESS) {
            NRF_LOG_WARNING("delete_next_record: fds_record_delete returned code %u", error_code);
            delete_all_files_pending = false;
            if (delegate) {
                delegate->on_all_files_deleted(false);
            }
        }
        return;
    }

    NRF_LOG_WARNING("delete_next_record: fds_record_iterate returned code %u", error_code);
    delete_all_files_pending = false;
    if (delegate) {
        delegate->on_all_files_deleted(false);
    }
}

bool FlashStorage::delete_file(uint16_t file_id) {
    if (delete_all_files_pending) {
        NRF_LOG_WARNING("Called delete_file(file_id=%u) during erasure", file_id);
        return false;
    }
    uint32_t error_code = fds_file_delete(file_id);
    if (error_code != FDS_SUCCESS) {
        NRF_LOG_WARNING("fds_file_delete(file_id=%u) failed with code %u", file_id, error_code);
        return false;
    }
    return false;
}

bool FlashStorage::delete_record(uint16_t file_id, uint16_t record_id) {
    if (delete_all_files_pending) {
        NRF_LOG_WARNING("Called delete_record(file_id=%u, record_id=%u) during erasure", file_id, record_id);
        return false;
    }

    fds_record_desc_t descriptor = {0};
    fds_find_token_t token = {0};

    uint32_t error_code = fds_record_find(file_id, record_id, &descriptor, &token);
    if (error_code != FDS_SUCCESS) {
        NRF_LOG_WARNING("fds_record_find(file_id=%u, record_id=%u) failed with code %u", file_id, record_id, error_code);
        return false;
    }

    error_code = fds_record_delete(&descriptor);
    if (error_code != FDS_SUCCESS) {
        NRF_LOG_WARNING("fds_record_delete(file_id=%u, record_id=%u) failed with code %u", file_id, record_id, error_code);
        return false;
    }

    return true;
}

bool FlashStorage::read_record(uint16_t file_id, uint16_t record_id, uint32_t* data, uint16_t *words_count) {
    if (delete_all_files_pending) {
        NRF_LOG_WARNING("Called read_record(file_id=%u, record_id=%u) during erasure", file_id, record_id);
        return false;
    }

    fds_record_desc_t descriptor = {0};
    fds_find_token_t token = {0};

    uint32_t error_code = fds_record_find(file_id, record_id, &descriptor, &token);
    if (error_code != FDS_SUCCESS) {
        NRF_LOG_WARNING("fds_record_find(file_id=%u, record_id=%u) failed with code %u", file_id, record_id, error_code);
        return false;
    }

    fds_flash_record_t record = {0};
    error_code = fds_record_open(&descriptor, &record);
    if (error_code != FDS_SUCCESS) {
        NRF_LOG_WARNING("fds_record_open(file_id=%u, record_id=%u) failed with code %u", file_id, record_id, error_code);
        return false;
    }

    *words_count = MIN(record.p_header->length_words, *words_count);
    memcpy(data, record.p_data, *words_count * 4);

    error_code = fds_record_close(&descriptor);
    if (error_code != FDS_SUCCESS) {
        NRF_LOG_WARNING("fds_record_close(file_id=%u, record_id=%u) failed with code %u", file_id, record_id, error_code);
        return false;
    }

    return true;
}

bool FlashStorage::write_record(uint16_t file_id, uint16_t record_id, const uint32_t* data, uint16_t words_count) {
    if (delete_all_files_pending) {
        NRF_LOG_WARNING("Called write_record(file_id=%u, record_id=%u) during erasure", file_id, record_id);
        return false;
    }

    fds_record_desc_t descriptor = {0};
    fds_find_token_t token = {0};
    fds_record_t record = {
        .file_id = file_id,
        .key = record_id,
        .data = {
            .p_data = data,
            .length_words = words_count
        }
    };

    uint32_t error_code = fds_record_find(file_id, record_id, &descriptor, &token);
    if (error_code == FDS_ERR_NOT_FOUND) {
        error_code = fds_record_write(&descriptor, &record);
        if (error_code != FDS_SUCCESS) {
            NRF_LOG_WARNING("fds_record_write(file_id=%u, record_id=%u) failed with code %u", file_id, record_id, error_code);
            return false;
        }
        return true;
    }

    if (error_code == FDS_SUCCESS) {
        error_code = fds_record_update(&descriptor, &record);
        if (error_code != FDS_SUCCESS) {
            NRF_LOG_WARNING("fds_record_update(file_id=%u, record_id=%u) failed with code %u", file_id, record_id, error_code);
            return false;
        }
        return true;
    }

    NRF_LOG_WARNING("fds_record_find(file_id=%u, record_id=%u) failed with code %u", file_id, record_id, error_code);
    return false;
}

bool FlashStorage::iterate_records(std::function<void(uint16_t, uint16_t, const uint32_t*, uint16_t)> callback) {
    fds_record_desc_t descriptor = {0};
    fds_find_token_t token = {0};
    uint32_t error_code;

    while(true) {
        error_code = fds_record_iterate(&descriptor, &token);
        if (error_code == FDS_ERR_NOT_FOUND) {
            return true;
        }

        if (error_code != FDS_SUCCESS) {
            NRF_LOG_WARNING("iterate_records(fds_record_iterate) returned code: %u", error_code);
            return false;
        }

        fds_flash_record_t record = {0};
        error_code = fds_record_open(&descriptor, &record);
        if (error_code != FDS_SUCCESS) {
            NRF_LOG_WARNING("iterate_records(fds_record_open) returned code: %u", error_code);
            return false;
        }
        
        callback(
            record.p_header->file_id, 
            record.p_header->record_key, 
            static_cast<const uint32_t*>(record.p_data), 
            record.p_header->length_words
        );

        error_code = fds_record_close(&descriptor);
        if (error_code != FDS_SUCCESS) {
            NRF_LOG_WARNING("iterate_records(fds_record_close) returned code: %u", error_code);
            return false;
        }
    }

    // Not reachable
    return false;
}