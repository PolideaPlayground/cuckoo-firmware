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

#ifndef LAP_TIMER_COMMANDS_H
#define LAP_TIMER_COMMANDS_H

#define COMMAND_MASK           0x3F
#define COMMAND_RESPONSE_BIT   0x80
#define COMMAND_INDICATION_BIT 0x40

#include <array>
#include <cstdint>
#include <cstring>
#include <cstddef>

#include "utils/byte_utils.h"

typedef uint8_t command_id_t;

enum CommandRequestCode : command_id_t {
    START_COMMAND_CODE      = 0x00,
    STOP_COMMAND_CODE       = 0x01,
    CURRENT_LAP_TIME_CODE   = 0x02,
    BEST_LAP_TIME_CODE      = 0x03,
    LAST_LAP_TIME_CODE      = 0x04,
    LAST_SESSION_ID_CODE    = 0x05,
    LIST_SESSIONS_IDS_CODE  = 0x06,
    GET_SESSION_RECORD_CODE = 0x07,
    COMMAND_CODE_MAX
};

typedef uint16_t session_id_t;
typedef uint8_t lap_id_t;
typedef uint8_t lap_id_offset_t;
typedef uint32_t lap_time_t;

enum SessionState : uint8_t {
    SESSION_STATE_PENDING   = 0x00,
    SESSION_STATE_COMPLETED = 0x01,
    SESSION_STATE_MAX
};

enum SessionIDRelation : uint8_t {
    SESSION_ID_RELATION_EQUAL_TO            = 0x00,
    SESSION_ID_RELATION_GREATER_OR_EQUAL_TO = 0x01,
    SESSION_ID_RELATION_MAX
};

// COMMAND utilities ----------------------------------------------------------

template<typename T, size_t N>
class Command {
    public:
        static constexpr size_t max_length = N;
        virtual size_t length() const {
            return max_length;
        }
        virtual bool serialize(uint8_t* buffer, size_t length) const = 0;
        virtual bool deserialize(const uint8_t* buffer, size_t length) = 0;
};

// START COMMAND --------------------------------------------------------------

class StartCommand : public Command<StartCommand, 1> {
public:
    StartCommand() : id(START_COMMAND_CODE) {}
    command_id_t get_id() const {
        return id;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        return true;
    }

private:
    command_id_t id;
};

class StartCommandResponse : public Command<StartCommandResponse, 3> {
public:
    StartCommandResponse(session_id_t session_id) : 
        id(START_COMMAND_CODE | COMMAND_RESPONSE_BIT),
        session_id(session_id) {}

    command_id_t get_id() const {
        return id;
    }
    session_id_t get_session_id() const {
        return session_id;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        write_uint16_le(session_id, buffer + 1);
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        session_id = read_uint16_le(buffer + 1);
        return true;
    }

private:
    command_id_t id;
    session_id_t session_id;
};

class StartCommandIndication : public Command<StartCommandIndication, 3> {
public:
    StartCommandIndication(session_id_t session_id) : 
        id(START_COMMAND_CODE | COMMAND_INDICATION_BIT),
        session_id(session_id) {}
    command_id_t get_id() const {
        return id;
    }
    session_id_t get_session_id() const {
        return session_id;
    }
    
    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        write_uint16_le(session_id, buffer + 1);
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        session_id = read_uint16_le(buffer + 1);
        return true;
    }

private:
    command_id_t id;
    session_id_t session_id;
};

// STOP COMMAND --------------------------------------------------------------

class StopCommand : public Command<StopCommand, 1>{
public:
    StopCommand() : id(STOP_COMMAND_CODE) {}
    command_id_t get_id() const {
        return id;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        return true;
    }

private:
    command_id_t id;
};

class StopCommandResponse : public Command<StopCommandResponse, 3> {
public:
    StopCommandResponse(session_id_t session_id) : 
        id(STOP_COMMAND_CODE | COMMAND_RESPONSE_BIT),
        session_id(session_id) {}
    
    command_id_t get_id() const {
        return id;
    }

    session_id_t get_session_id() const {
        return session_id;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        write_uint16_le(session_id, buffer + 1);
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        session_id = read_uint16_le(buffer + 1);
        return true;
    }

private:
    command_id_t id;
    session_id_t session_id;
};

class StopCommandIndication : public Command<StopCommandIndication, 3> {
public:
    StopCommandIndication(session_id_t session_id) : 
        id(STOP_COMMAND_CODE | COMMAND_INDICATION_BIT),
        session_id(session_id) {}
    command_id_t get_id() const {
        return id;
    }
    session_id_t get_session_id() const {
        return session_id;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        write_uint16_le(session_id, buffer + 1);
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        session_id = read_uint16_le(buffer + 1);
        return true;
    }

private:
    command_id_t id;
    session_id_t session_id;
};

// CURRENT LAP TIME COMMAND --------------------------------------------------------------

class CurrentLapTimeCommand : public Command<CurrentLapTimeCommand, 1> {
public:
    CurrentLapTimeCommand() : id(CURRENT_LAP_TIME_CODE) {}
    
    command_id_t get_id() const {
        return id;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        return true;
    }

private:
    command_id_t id;
};

class CurrentLapTimeCommandResponse : public Command<CurrentLapTimeCommandResponse, 9> {
public:
    CurrentLapTimeCommandResponse() :
        id(CURRENT_LAP_TIME_CODE | COMMAND_RESPONSE_BIT),
        session_state(SESSION_STATE_COMPLETED),
        session_id(0),
        lap_id(0),
        lap_time(0) {}

    CurrentLapTimeCommandResponse(session_id_t session_id, 
                                  lap_id_t lap_id, 
                                  lap_time_t lap_time) : 
        id(CURRENT_LAP_TIME_CODE | COMMAND_RESPONSE_BIT),
        session_state(SESSION_STATE_PENDING),
        session_id(session_id),
        lap_id(lap_id),
        lap_time(lap_time) {}

    command_id_t get_id() const {
        return id;
    }
    SessionState get_session_state() const {
        return session_state;
    }
    session_id_t get_session_id() const {
        return session_id;
    }
    lap_id_t get_lap_id() const {
        return lap_id;
    }
    lap_time_t get_lap_time() const {
        return lap_time;
    }

    virtual size_t length() const override {
        switch (session_state) {
            case SESSION_STATE_PENDING:
                return max_length;
            case SESSION_STATE_COMPLETED:
                return 2;
            default:
                return max_length;
        }
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        buffer[1] = session_state;
        write_uint16_le(session_id, buffer + 2);
        buffer[4] = lap_id;
        write_uint32_le(lap_time, buffer + 5);
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < 2) return false;
        if (buffer[0] != id) return false;
        SessionState session_state = static_cast<SessionState>(buffer[1]);
        switch (session_state) {
            case SESSION_STATE_COMPLETED:
                this->session_state = session_state;
                return true;
            case SESSION_STATE_PENDING:
                if (length < max_length) return false;
                this->session_state = session_state;
                session_id = read_uint16_le(buffer + 2);
                lap_id = buffer[4];
                lap_time = read_uint32_le(buffer + 5);
                return true;
            default:
                return false;
        }
    }

private:
    command_id_t id;
    SessionState session_state;
    session_id_t session_id;
    lap_id_t lap_id;
    lap_time_t lap_time;
};

// BEST LAP TIME COMMAND --------------------------------------------------------------

class BestLapTimeCommand : public Command<BestLapTimeCommand, 1> {
public:
    BestLapTimeCommand() : id(BEST_LAP_TIME_CODE) {}
    command_id_t get_id() const {
        return id;
    }
    
    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        return true;
    }

private:
    command_id_t id;
};

class BestLapTimeCommandResponse : public Command<BestLapTimeCommandResponse, 9> {
public:
    BestLapTimeCommandResponse() :
        id(BEST_LAP_TIME_CODE | COMMAND_RESPONSE_BIT),
        session_state(SESSION_STATE_COMPLETED),
        session_id(0),
        lap_id(0),
        lap_time(0) {}

    BestLapTimeCommandResponse(session_id_t session_id, 
                                  lap_id_t lap_id, 
                                  lap_time_t lap_time) : 
        id(BEST_LAP_TIME_CODE | COMMAND_RESPONSE_BIT),
        session_state(SESSION_STATE_PENDING),
        session_id(session_id),
        lap_id(lap_id),
        lap_time(lap_time) {}

    command_id_t get_id() const {
        return id;
    }
    SessionState get_session_state() const {
        return session_state;
    }
    session_id_t get_session_id() const {
        return session_id;
    }
    lap_id_t get_lap_id() const {
        return lap_id;
    }
    lap_time_t get_lap_time() const {
        return lap_time;
    }

    virtual size_t length() const override {
        switch (session_state) {
            case SESSION_STATE_PENDING:
                return max_length;
            case SESSION_STATE_COMPLETED:
                return 2;
            default:
                return max_length;
        }
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        buffer[1] = session_state;
        write_uint16_le(session_id, buffer + 2);
        buffer[4] = lap_id;
        write_uint32_le(lap_time, buffer + 5);
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < 2) return false;
        if (buffer[0] != id) return false;
        SessionState session_state = static_cast<SessionState>(buffer[1]);
        switch (session_state) {
            case SESSION_STATE_COMPLETED:
                this->session_state = SESSION_STATE_COMPLETED;
                return true;
            case SESSION_STATE_PENDING:
                if (length < max_length) return false;
                this->session_state = SESSION_STATE_PENDING;
                session_id = read_uint16_le(buffer + 2);
                lap_id = buffer[4];
                lap_time = read_uint32_le(buffer + 5);
                return true;
            default:
                return false;
        }
    }

private:
    command_id_t id;
    SessionState session_state;
    session_id_t session_id;
    lap_id_t lap_id;
    lap_time_t lap_time;
};

class BestLapTimeCommandIndication : public Command<BestLapTimeCommandIndication, 9> {
public:
    BestLapTimeCommandIndication(session_id_t session_id, 
                                  lap_id_t lap_id, 
                                  lap_time_t lap_time) : 
        id(BEST_LAP_TIME_CODE | COMMAND_INDICATION_BIT),
        session_state(SESSION_STATE_PENDING),
        session_id(session_id),
        lap_id(lap_id),
        lap_time(lap_time) {}

    command_id_t get_id() const {
        return id;
    }
    SessionState get_session_state() const {
        return session_state;
    }
    session_id_t get_session_id() const {
        return session_id;
    }
    lap_id_t get_lap_id() const {
        return lap_id;
    }
    lap_time_t get_lap_time() const {
        return lap_time;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        buffer[1] = session_state;
        write_uint16_le(session_id, buffer + 2);
        buffer[4] = lap_id;
        write_uint32_le(lap_time, buffer + 5);
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        if (buffer[1] != SESSION_STATE_PENDING) return false;
        session_id = read_uint16_le(buffer + 2);
        lap_id = buffer[4];
        lap_time = read_uint32_le(buffer + 5);
        return true;
    }

private:
    command_id_t id;
    SessionState session_state;
    session_id_t session_id;
    lap_id_t lap_id;
    lap_time_t lap_time;
};

// LAST LAP TIME COMMAND --------------------------------------------------------------

class LastLapTimeCommand : public Command<LastLapTimeCommand, 1> {
public:
    LastLapTimeCommand() : id(LAST_LAP_TIME_CODE) {}
    command_id_t get_id() const {
        return id;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        return true;
    }

private:
    command_id_t id;
};

class LastLapTimeCommandResponse : public Command<LastLapTimeCommandResponse, 9> {
public:
    LastLapTimeCommandResponse() :
        id(LAST_LAP_TIME_CODE | COMMAND_RESPONSE_BIT),
        session_state(SESSION_STATE_COMPLETED),
        session_id(0),
        lap_id(0),
        lap_time(0) {}

    LastLapTimeCommandResponse(session_id_t session_id, 
                                  lap_id_t lap_id, 
                                  lap_time_t lap_time) : 
        id(LAST_LAP_TIME_CODE | COMMAND_RESPONSE_BIT),
        session_state(SESSION_STATE_PENDING),
        session_id(session_id),
        lap_id(lap_id),
        lap_time(lap_time) {}

    command_id_t get_id() const {
        return id;
    }
    SessionState get_session_state() const {
        return session_state;
    }
    session_id_t get_session_id() const {
        return session_id;
    }
    lap_id_t get_lap_id() const {
        return lap_id;
    }
    lap_time_t get_lap_time() const {
        return lap_time;
    }

    virtual size_t length() const override {
        switch (session_state) {
            case SESSION_STATE_PENDING:
                return max_length;
            case SESSION_STATE_COMPLETED:
                return 2;
            default:
                return max_length;
        }
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        buffer[1] = session_state;
        write_uint16_le(session_id, buffer + 2);
        buffer[4] = lap_id;
        write_uint32_le(lap_time, buffer + 5);
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < 2) return false;
        if (buffer[0] != id) return false;
        SessionState session_state = static_cast<SessionState>(buffer[1]);
        switch (session_state) {
            case SESSION_STATE_COMPLETED:
                this->session_state = SESSION_STATE_COMPLETED;
                return true;
            case SESSION_STATE_PENDING:
                if (length < max_length) return false;
                this->session_state = SESSION_STATE_PENDING;
                session_id = read_uint16_le(buffer + 2);
                lap_id = buffer[4];
                lap_time = read_uint32_le(buffer + 5);
                return true;
            default:
                return false;
        }
    }

private:
    command_id_t id;
    SessionState session_state;
    session_id_t session_id;
    lap_id_t lap_id;
    lap_time_t lap_time;
};

class LastLapTimeCommandIndication  : public Command<LastLapTimeCommandIndication, 9>  {
public:
    LastLapTimeCommandIndication(session_id_t session_id, 
                                  lap_id_t lap_id, 
                                  lap_time_t lap_time) : 
        id(LAST_LAP_TIME_CODE | COMMAND_INDICATION_BIT),
        session_state(SESSION_STATE_PENDING),
        session_id(session_id),
        lap_id(lap_id),
        lap_time(lap_time) {}

    command_id_t get_id() const {
        return id;
    }
    SessionState get_session_state() const {
        return session_state;
    }
    session_id_t get_session_id() const {
        return session_id;
    }
    lap_id_t get_lap_id() const {
        return lap_id;
    }
    lap_time_t get_lap_time() const {
        return lap_time;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        buffer[1] = session_state;
        write_uint16_le(session_id, buffer + 2);
        buffer[4] = lap_id;
        write_uint32_le(lap_time, buffer + 5);
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        if (buffer[1] != SESSION_STATE_PENDING) return false;
        session_id = read_uint16_le(buffer + 2);
        lap_id = buffer[4];
        lap_time = read_uint32_le(buffer + 5);
        return true;
    }

private:
    command_id_t id;
    SessionState session_state;
    session_id_t session_id;
    lap_id_t lap_id;
    lap_time_t lap_time;
};

// LAST SESSION ID --------------------------------------------------------------

class LastSessionIDCommand : public Command<LastSessionIDCommand, 1> {
public:
    LastSessionIDCommand() : id(LAST_SESSION_ID_CODE) {}
    command_id_t get_id() const {
        return id;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        return true;
    }

private:
    command_id_t id;
};

class LastSessionIDCommandResponse : public Command<LastSessionIDCommandResponse, 4> {
public:
    LastSessionIDCommandResponse(session_id_t session_id, SessionState session_state) : 
        id(LAST_SESSION_ID_CODE | COMMAND_RESPONSE_BIT),
        session_id(session_id),
        session_state(session_state) {}

    command_id_t get_id() const {
        return id;
    }
    session_id_t get_session_id() const {
        return session_id;
    }
    SessionState get_session_state() const {
        return session_state;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        write_uint16_le(session_id, buffer + 1);
        buffer[3] = session_state;
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        if (buffer[3] >= SESSION_STATE_MAX) return false;
        session_id = read_uint16_le(buffer + 1);
        session_state = static_cast<SessionState>(buffer[3]);
        return true;
    }

private:
    command_id_t id;
    session_id_t session_id;
    SessionState session_state;
};

class LastSessionIDCommandIndication : public Command<LastSessionIDCommandIndication, 4> {
public:
    LastSessionIDCommandIndication(session_id_t session_id) : 
        id(LAST_SESSION_ID_CODE | COMMAND_INDICATION_BIT),
        session_id(session_id),
        session_state(SESSION_STATE_PENDING) {}
        
    command_id_t get_id() const {
        return id;
    }
    session_id_t get_session_id() const {
        return session_id;
    }
    SessionState get_session_state() const {
        return session_state;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        write_uint16_le(session_id, buffer + 1);
        buffer[3] = session_state;
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        if (buffer[3] != SESSION_STATE_PENDING) return false;
        session_id = read_uint16_le(buffer + 1);
        session_state = static_cast<SessionState>(buffer[3]);
        return true;
    }

private:
    command_id_t id;
    session_id_t session_id;
    SessionState session_state;
};

// LIST SESSIONS IDS --------------------------------------------------------------

class ListSessionsIDsCommand : public Command<ListSessionsIDsCommand, 2> {
public:
    ListSessionsIDsCommand(SessionIDRelation session_id_relation) : 
        id(LIST_SESSIONS_IDS_CODE),
        session_id_relation(session_id_relation) {}

    command_id_t get_id() const {
        return id;
    }
    SessionIDRelation get_session_id_relation() const {
        return session_id_relation;
    }
    
    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        buffer[1] = session_id_relation;
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        if (buffer[1] >= SESSION_STATE_MAX) return false;
        session_id_relation = static_cast<SessionIDRelation>(buffer[1]);
        return true;
    }

private:
    command_id_t id;
    SessionIDRelation session_id_relation;
};

template<uint8_t MAX_SESSION_IDS>
class ListSessionsIDsCommandResponse : public Command<ListSessionsIDsCommandResponse<MAX_SESSION_IDS>, MAX_SESSION_IDS * sizeof(session_id_t) + 2> {
public:
    ListSessionsIDsCommandResponse(uint8_t remaining_session_ids, const session_id_t* session_ids, uint8_t session_ids_count) : 
        id(LIST_SESSIONS_IDS_CODE | COMMAND_RESPONSE_BIT),
        remaining_session_ids(remaining_session_ids),
        session_ids { 0 },
        session_ids_count(session_ids_count) {
            if (session_ids) {
                std::memcpy(this->session_ids, session_ids, session_ids_count * sizeof(session_id_t));
            }
        }

    command_id_t get_id() const {
        return id;
    }
    uint8_t get_remaining_session_ids() const {
        return remaining_session_ids;
    }
    
    const session_id_t* get_session_ids() const {
        return session_ids;
    }

    uint8_t get_session_ids_count() const {
        return session_ids_count;
    }

    virtual size_t length() const override {
        return 2 + session_ids_count * sizeof(session_id_t);
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < 2 + session_ids_count * sizeof(session_id_t)) return false;
        buffer[0] = id;
        buffer[1] = remaining_session_ids;
        for (uint8_t i = 0; i < session_ids_count; i++) {
            write_uint16_le(session_ids[i], buffer + 2 + i * sizeof(session_id_t));
        }
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < 2) return false;
        if (buffer[0] != id) return false;
        size_t count = (length - 2) / sizeof(session_id_t);
        if (count > MAX_SESSION_IDS) return false;

        remaining_session_ids = buffer[1];
        session_ids_count = count;
        for (size_t i = 0; i < session_ids_count; i++) {
            session_ids[i] = read_uint16_le(buffer + 2 + i * sizeof(session_id_t));
        }
        return true;
    }

private:
    command_id_t id;
    uint8_t remaining_session_ids;
    session_id_t session_ids[MAX_SESSION_IDS];
    uint8_t session_ids_count;
};

// GET SESSION RECORD --------------------------------------------------------------

class GetSessionRecordCommand : public Command<GetSessionRecordCommand, 5> {
public:
    GetSessionRecordCommand (session_id_t session_id, SessionIDRelation session_id_relation, lap_id_offset_t lap_offset) : 
        id(GET_SESSION_RECORD_CODE),
        session_id(session_id),
        session_id_relation(session_id_relation),
        lap_offset(lap_offset) {}

    command_id_t get_id() const {
        return id;
    }
    session_id_t get_session_id() const {
        return session_id;
    }
    SessionIDRelation get_session_id_relation() const {
        return session_id_relation;
    }
    lap_id_offset_t get_lap_offset() const {
        return lap_offset;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        write_uint16_le(session_id, buffer + 1);
        buffer[3] = session_id_relation;
        buffer[4] = lap_offset;
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        session_id = read_uint16_le(buffer + 1);
        session_id_relation = static_cast<SessionIDRelation>(buffer[3]);
        lap_offset = buffer[4];
        return true;
    }

private:
    command_id_t id;
    session_id_t session_id;
    SessionIDRelation session_id_relation;
    lap_id_offset_t lap_offset;
};

class GetSessionRecordHeaderCommandResponse : public Command<GetSessionRecordHeaderCommandResponse, 6> {
public:
    GetSessionRecordHeaderCommandResponse(session_id_t session_id, SessionState session_state, lap_id_t lap_count) : 
        id(GET_SESSION_RECORD_CODE | COMMAND_RESPONSE_BIT),
        phase(0),
        session_id(session_id),
        session_state(session_state),
        lap_count(lap_count) {}

    command_id_t get_id() const {
        return id;
    }
    session_id_t get_session_id() const {
        return session_id;
    }
    SessionState get_session_state() const {
        return session_state;
    }
    lap_id_t get_lap_count() const {
        return lap_count;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        buffer[1] = phase;
        write_uint16_le(session_id, buffer + 2);
        buffer[4] = session_state;
        buffer[5] = lap_count;
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        if (buffer[1] != phase) return false;
        session_id = read_uint16_le(buffer + 2);
        session_state = static_cast<SessionState>(buffer[4]);
        lap_count = buffer[5];
        return true;
    }

private:
    command_id_t id;
    uint8_t phase;
    session_id_t session_id;
    SessionState session_state;
    lap_id_t lap_count;
};

template<uint8_t MAX_LAP_TIMES>
class GetSessionRecordPayloadCommandResponse : public Command<GetSessionRecordPayloadCommandResponse<MAX_LAP_TIMES>, 2 + MAX_LAP_TIMES * sizeof(lap_time_t)> {
public:
    GetSessionRecordPayloadCommandResponse(const lap_time_t *lap_times, uint8_t lap_time_count) : 
        id(GET_SESSION_RECORD_CODE | COMMAND_RESPONSE_BIT),
        phase(1),
        lap_times { 0 },
        lap_time_count(lap_time_count) {
            if (lap_times) {
                std::memcpy(this->lap_times, lap_times, lap_time_count * sizeof(lap_time_t));
            }
        }

    command_id_t get_id() const {
        return id;
    }

    const lap_time_t* get_lap_times() const {
        return lap_times;
    }

    uint8_t get_lap_time_count() const {
        return lap_time_count;
    }

    virtual size_t length() const override {
        return 2 + lap_time_count * sizeof(lap_time_t);
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < 2 + lap_time_count * sizeof(lap_time_t)) return false;
        buffer[0] = id;
        buffer[1] = phase;
        for (size_t i = 0; i < lap_time_count; i++) {
            write_uint32_le(lap_times[i], buffer + 2 + i * sizeof(lap_time_t));
        }
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < 2) return false;
        if (buffer[0] != id) return false;
        if (buffer[1] != phase) return false;
        size_t count = (length - 2) / sizeof(lap_time_t);
        if (count > MAX_LAP_TIMES ) return false;
        lap_time_count = count;
        for (uint8_t i = 0; i < lap_time_count; i++) {
            lap_times[i] = read_uint32_le(buffer + 2 + i * sizeof(lap_time_t));
        }
        return true;
    }

private:
    command_id_t id;
    uint8_t phase;
    lap_time_t lap_times[MAX_LAP_TIMES];
    uint8_t lap_time_count;
};

class GetSessionRecordEmptyCommandResponse : public Command<GetSessionRecordEmptyCommandResponse, 2> {
public:
    GetSessionRecordEmptyCommandResponse() : 
        id(GET_SESSION_RECORD_CODE | COMMAND_RESPONSE_BIT),
        phase(0xFF) {}

    command_id_t get_id() const {
        return id;
    }

    virtual bool serialize(uint8_t* buffer, size_t length) const override {
        if (length < max_length) return false;
        buffer[0] = id;
        buffer[1] = phase;
        return true;
    }

    virtual bool deserialize(const uint8_t* buffer, size_t length) override {
        if (length < max_length) return false;
        if (buffer[0] != id) return false;
        if (buffer[1] != phase) return false;
        return true;
    }

private:
    command_id_t id;
    uint8_t phase;
};


#endif // LAP_TIMER_COMMANDS_H