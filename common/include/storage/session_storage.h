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

#ifndef LAP_TIMER_SESSION_STORAGE_H
#define LAP_TIMER_SESSION_STORAGE_H

#include "storage/flash_storage_interface.h"
#include "storage/session_storage_events.h"
#include "events/event_observer.h"
#include "events/event_dispatcher_interface.h"

class SessionStorage : public EventObserver, public FlashStorageInterface::Delegate {
public:
    SessionStorage(EventDispatcherInterface &event_dispatcher, FlashStorageInterface &flash_storage);

    void on_event(const Event& event) override;

    void send_reset_storage_result(bool successful);
    void on_reset_storage(const ResetStorage& reset_storage);

    void on_start_session(const StartSession& start_session);
    void on_stop_session(const StopSession& stop_session);
    void on_add_lap_time(const AddLapTime& add_lap_time);

    void on_initialized(bool successful, FlashStorageInterface& interface) override;
    void on_garbage_collected(bool successful) override;
    void on_all_files_deleted(bool successful) override;
    void on_file_deleted(bool successful, uint16_t file_id) override;
    void on_record_deleted(bool successful, uint16_t file_id, uint16_t record_id) override;
    void on_record_written(bool successful, uint16_t file_id, uint16_t record_id) override;

private:
    EventDispatcherInterface& event_dispatcher;
    FlashStorageInterface &flash_storage;

    struct LapRecordData {
        uint32_t lap_time;
    } __attribute__ ((aligned (32)));
    static_assert(sizeof(LapRecordData) % 4 == 0);

    constexpr static uint16_t MAX_FILE_ID_FOR_SESSION_ID = 0xFFF0;

    bool reset_pending;
    uint16_t first_session_id;
    uint16_t last_session_id;
    uint16_t last_lap_id;
    bool last_session_completed;
};

#endif // LAP_TIMER_SESSION_STORAGE_H