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

#include "storage/session_storage.h"
#include "utils/log.h"

#include <algorithm>

SessionStorage::SessionStorage(EventDispatcherInterface &event_dispatcher, FlashStorageInterface &flash_storage) 
    : event_dispatcher(event_dispatcher), 
      flash_storage(flash_storage),
      reset_pending(false),
      first_session_id(0),
      last_session_id(0),
      last_lap_id(0),
      last_session_completed(true) {
    flash_storage.set_delegate(this);
    event_dispatcher.register_observer(this);
}

void SessionStorage::on_event(const Event& event) {
    std::visit(overloaded{
        [this](const ResetStorage& reset_storage) { on_reset_storage(reset_storage); },
        [this](const StartSession& start_session) { on_start_session(start_session); },
        [this](const StopSession& stop_session) { on_stop_session(stop_session); },
        [this](const AddLapTime& add_lap_time) { on_add_lap_time(add_lap_time); },
        [](auto other) {}
    }, event);
}

void SessionStorage::on_initialized(bool successful, FlashStorageInterface& interface) {
    if (!successful) {
        LOG_ERROR("Initialization of Session Storage is unsuccessful.");
        return;
    }

    flash_storage.iterate_records([this](uint16_t file_id, uint16_t record_id, const uint32_t *record_data, uint16_t record_data_length) {
        if (file_id < MAX_FILE_ID_FOR_SESSION_ID) {
            first_session_id = std::min(first_session_id, file_id);
            last_session_id = std::max(last_session_id, file_id);
        }
    });

    LOG_INFO("Initialization of Session Storage successful: first session: %u, last session: %u",
        first_session_id,
        last_session_id
    );

    event_dispatcher.emit_event(SessionStorageInitialized());
}

void SessionStorage::on_garbage_collected(bool successful) {
    // Once garbage is callected our factory reset is complete.
    if (reset_pending) {
        send_reset_storage_result(successful);
    }
}

void SessionStorage::on_all_files_deleted(bool successful) {
    // Once delete is complete, collect garbage.
    if (reset_pending) {
        if (!successful || !flash_storage.collect_garbage()) {
            send_reset_storage_result(false);
        }
    }
}

void SessionStorage::on_file_deleted(bool successful, uint16_t file_id) {

}

void SessionStorage::on_record_deleted(bool successful, uint16_t file_id, uint16_t record_id) {

}

void SessionStorage::on_record_written(bool successful, uint16_t file_id, uint16_t record_id) {

}

void SessionStorage::on_reset_storage(const ResetStorage& reset_storage) {
    LOG_INFO("Deleting whole sessions and records history...");
    reset_pending = true;
    if (!flash_storage.delete_all_files()) {
        reset_pending = false;
        event_dispatcher.emit_event(StorageResponse(reset_storage, false));
    }
}

void SessionStorage::send_reset_storage_result(bool successful) {
    reset_pending = false;
    if (successful) {
        LOG_INFO("Successfully cleared sessions and records history...");
    } else {
        LOG_WARNING("Failed to clear sessions and records history...");
    }
    event_dispatcher.emit_event(StorageResponse(ResetStorage(), successful));
}

void SessionStorage::on_start_session(const StartSession& start_session) {
    LOG_INFO("Saving new session...");
    if (!last_session_completed) {
        last_session_completed = false;
        last_lap_id = 0;
    }
    if (first_session_id == 0) {
        first_session_id++;
    }
    last_session_id++;
}

void SessionStorage::on_stop_session(const StopSession& stop_session) {
    LOG_INFO("Stoping session...");
    last_session_completed = true;
}

void SessionStorage::on_add_lap_time(const AddLapTime& add_lap_time) {
    LOG_INFO("Adding lap time...");
}