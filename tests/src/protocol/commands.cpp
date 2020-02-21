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
#include "protocol/commands.h"

#include <cstring>
#include <vector>
#include <array>

template<typename T>
bool serialize_command(const T &command, const std::vector<uint8_t> &expected_result) {
    uint8_t serialized_data[T::max_length];
    REQUIRE(command.serialize(serialized_data, T::max_length));
    return std::memcmp(serialized_data, expected_result.data(), command.length()) == 0;
}

template<typename T>
bool deserialize_command(T& command, const std::vector<uint8_t> &data) {
    return command.deserialize(data.data(), data.size());
}

TEST_CASE("Start command should serialize properly", "[commands]") {
    REQUIRE(serialize_command<StartCommand>(StartCommand(), { 0x00 }));
    REQUIRE(serialize_command<StartCommandResponse>(StartCommandResponse(0x3012), { 0x80, 0x12, 0x30 }));
    REQUIRE(serialize_command<StartCommandIndication>(StartCommandIndication(0xFFDD), { 0x40, 0xDD, 0xFF }));
}

TEST_CASE("Start command should deserialize properly", "[commands]") {
    StartCommand start_command;
    REQUIRE(deserialize_command(start_command, { 0x00 }));
    REQUIRE(start_command.get_id() == START_COMMAND_CODE);
    REQUIRE(start_command.length() == 1);

    StartCommandResponse start_command_response(0xFFFF);
    REQUIRE(deserialize_command(start_command_response, { 0x80, 0x12, 0x30 }));
    REQUIRE(start_command_response.get_id() == (START_COMMAND_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(start_command_response.get_session_id() == 0x3012);
    REQUIRE(start_command_response.length() == 3);

    StartCommandIndication start_command_indication(0xFFFF);
    REQUIRE(deserialize_command(start_command_indication, { 0x40, 0xDD, 0xFF }));
    REQUIRE(start_command_indication.get_id() == (START_COMMAND_CODE | COMMAND_INDICATION_BIT));
    REQUIRE(start_command_indication.get_session_id() == 0xffdd);
    REQUIRE(start_command_indication.length() == 3);
}

TEST_CASE("Stop command should serialize properly", "[commands]") {
    REQUIRE(serialize_command<StopCommand>(StopCommand(), { 0x01 }));
    REQUIRE(serialize_command<StopCommandResponse>(StopCommandResponse(0x3012), { 0x81, 0x12, 0x30 }));
    REQUIRE(serialize_command<StopCommandIndication>(StopCommandIndication(0xFFDD), { 0x41, 0xDD, 0xFF }));
}

TEST_CASE("Stop command should deserialize properly", "[commands]") {
    StopCommand stop_command;
    REQUIRE(deserialize_command(stop_command, { 0x01 }));
    REQUIRE(stop_command.get_id() == STOP_COMMAND_CODE);
    REQUIRE(stop_command.length() == 1);

    StopCommandResponse stop_command_response(0xFFFF);
    REQUIRE(deserialize_command(stop_command_response, { 0x81, 0x12, 0x30 }));
    REQUIRE(stop_command_response.get_id() == (STOP_COMMAND_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(stop_command_response.get_session_id() == 0x3012);
    REQUIRE(stop_command_response.length() == 3);

    StopCommandIndication stop_command_indication(0xFFFF);
    REQUIRE(deserialize_command(stop_command_indication, { 0x41, 0xDD, 0xFF }));
    REQUIRE(stop_command_indication.get_id() == (STOP_COMMAND_CODE | COMMAND_INDICATION_BIT));
    REQUIRE(stop_command_indication.get_session_id() == 0xFFDD);
    REQUIRE(stop_command_indication.length() == 3);
}

TEST_CASE("Current lap time command should serialize properly", "[commands]") {
    REQUIRE(serialize_command<CurrentLapTimeCommand>(CurrentLapTimeCommand(), { 0x02 }));
    REQUIRE(serialize_command<CurrentLapTimeCommandResponse>(CurrentLapTimeCommandResponse(0x3012, 0xdd, 0x80),
        { 0x82, 0x00, 0x12, 0x30, 0xdd, 0x80, 0x00, 0x00, 0x00 }));
}

TEST_CASE("Current lap time command should deserialize properly", "[commands]") {
    CurrentLapTimeCommand command;
    REQUIRE(deserialize_command(command, { 0x02 }));
    REQUIRE(command.get_id() == CURRENT_LAP_TIME_CODE);
    REQUIRE(command.length() == 1);

    CurrentLapTimeCommandResponse response;
    REQUIRE(deserialize_command(response, { 0x82, 0x00, 0x12, 0x30, 0xdd, 0x80, 0x00, 0x00, 0x00 }));
    REQUIRE(response.get_id() == (CURRENT_LAP_TIME_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(response.get_session_state() == SESSION_STATE_PENDING);
    REQUIRE(response.get_session_id() == 0x3012);
    REQUIRE(response.get_lap_id() == 0xdd);
    REQUIRE(response.get_lap_time() == 0x80);
    REQUIRE(response.length() == 9);

    CurrentLapTimeCommandResponse response2;
    REQUIRE(deserialize_command(response2, { 0x82, 0x01 }));
    REQUIRE(response2.get_id() == (CURRENT_LAP_TIME_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(response2.get_session_state() == SESSION_STATE_COMPLETED);
    REQUIRE(response2.length() == 2);
}

TEST_CASE("Best lap time command should serialize properly", "[commands]") {
    REQUIRE(serialize_command<BestLapTimeCommand>(BestLapTimeCommand(), { 0x03 }));
    REQUIRE(serialize_command<BestLapTimeCommandResponse>(BestLapTimeCommandResponse(0x3012, 0xdd, 0x80),
        { 0x83, 0x00, 0x12, 0x30, 0xdd, 0x80, 0x00, 0x00, 0x00 }));
    REQUIRE(serialize_command<BestLapTimeCommandIndication>(BestLapTimeCommandIndication(0x3012, 0xdd, 0x80),
        { 0x43, 0x00, 0x12, 0x30, 0xdd, 0x80, 0x00, 0x00, 0x00 }));
}

TEST_CASE("Best lap time command should deserialize properly", "[commands]") {
    BestLapTimeCommand command;
    REQUIRE(deserialize_command(command, { 0x03 }));
    REQUIRE(command.get_id() == BEST_LAP_TIME_CODE);
    REQUIRE(command.length() == 1);

    BestLapTimeCommandResponse response;
    REQUIRE(deserialize_command(response, { 0x83, 0x00, 0x12, 0x30, 0xdd, 0x80, 0x00, 0x00, 0x00 }));
    REQUIRE(response.get_id() == (BEST_LAP_TIME_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(response.get_session_state() == SESSION_STATE_PENDING);
    REQUIRE(response.get_session_id() == 0x3012);
    REQUIRE(response.get_lap_id() == 0xdd);
    REQUIRE(response.get_lap_time() == 0x80);
    REQUIRE(response.length() == 9);

    BestLapTimeCommandResponse response2;
    REQUIRE(deserialize_command(response2, { 0x83, 0x01 }));
    REQUIRE(response2.get_id() == (BEST_LAP_TIME_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(response2.get_session_state() == SESSION_STATE_COMPLETED);
    REQUIRE(response2.length() == 2);

    BestLapTimeCommandIndication indication(0xFFFF, 0xFF, 0xFFFFFFFF);
    REQUIRE(deserialize_command(indication, { 0x43, 0x00, 0x12, 0x30, 0xdd, 0x80, 0x00, 0x00, 0x00 }));
    REQUIRE(indication.get_id() == (BEST_LAP_TIME_CODE | COMMAND_INDICATION_BIT));
    REQUIRE(indication.get_session_state() == SESSION_STATE_PENDING);
    REQUIRE(indication.get_session_id() == 0x3012);
    REQUIRE(indication.get_lap_id() == 0xdd);
    REQUIRE(indication.get_lap_time() == 0x80);
    REQUIRE(indication.length() == 9);
}

TEST_CASE("Last lap time command should serialize properly", "[commands]") {
    REQUIRE(serialize_command<LastLapTimeCommand>(LastLapTimeCommand(), { 0x04 }));
    REQUIRE(serialize_command<LastLapTimeCommandResponse>(LastLapTimeCommandResponse(0x3012, 0xdd, 0x80),
        { 0x84, 0x00, 0x12, 0x30, 0xdd, 0x80, 0x00, 0x00, 0x00 }));
    REQUIRE(serialize_command<LastLapTimeCommandIndication>(LastLapTimeCommandIndication(0x3012, 0xdd, 0x80),
        { 0x44, 0x00, 0x12, 0x30, 0xdd, 0x80, 0x00, 0x00, 0x00 }));
}

TEST_CASE("Last lap time command should deserialize properly", "[commands]") {
    LastLapTimeCommand command;
    REQUIRE(deserialize_command(command, { 0x04 }));
    REQUIRE(command.get_id() == LAST_LAP_TIME_CODE);
    REQUIRE(command.length() == 1);

    LastLapTimeCommandResponse response;
    REQUIRE(deserialize_command(response, { 0x84, 0x00, 0x12, 0x30, 0xdd, 0x80, 0x00, 0x00, 0x00 }));
    REQUIRE(response.get_id() == (LAST_LAP_TIME_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(response.get_session_state() == SESSION_STATE_PENDING);
    REQUIRE(response.get_session_id() == 0x3012);
    REQUIRE(response.get_lap_id() == 0xdd);
    REQUIRE(response.get_lap_time() == 0x80);
    REQUIRE(response.length() == 9);

    LastLapTimeCommandResponse response2;
    REQUIRE(deserialize_command(response2, { 0x84, 0x01 }));
    REQUIRE(response2.get_id() == (LAST_LAP_TIME_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(response2.get_session_state() == SESSION_STATE_COMPLETED);
    REQUIRE(response2.length() == 2);

    LastLapTimeCommandIndication indication(0xFFFF, 0xFF, 0xFFFFFFFF);
    REQUIRE(deserialize_command(indication, { 0x44, 0x00, 0x12, 0x30, 0xdd, 0x80, 0x00, 0x00, 0x00 }));
    REQUIRE(indication.get_id() == (LAST_LAP_TIME_CODE | COMMAND_INDICATION_BIT));
    REQUIRE(indication.get_session_state() == SESSION_STATE_PENDING);
    REQUIRE(indication.get_session_id() == 0x3012);
    REQUIRE(indication.get_lap_id() == 0xdd);
    REQUIRE(indication.get_lap_time() == 0x80);
    REQUIRE(indication.length() == 9);
}

TEST_CASE("Last Session ID command should serialize properly", "[commands]") {
    REQUIRE(serialize_command<LastSessionIDCommand>(LastSessionIDCommand(), { 0x05 }));
    REQUIRE(serialize_command<LastSessionIDCommandResponse>(LastSessionIDCommandResponse(0x3012, SESSION_STATE_COMPLETED), { 0x85, 0x12, 0x30, 0x01 }));
    REQUIRE(serialize_command<LastSessionIDCommandIndication>(LastSessionIDCommandIndication(0x3012), { 0x45, 0x12, 0x30, 0x00  }));
}

TEST_CASE("Last Session ID command should deserialize properly", "[commands]") {
    LastSessionIDCommand command;
    REQUIRE(deserialize_command(command, { 0x05 }));
    REQUIRE(command.get_id() == LAST_SESSION_ID_CODE);
    REQUIRE(command.length() == 1);

    LastSessionIDCommandResponse response(0xFFFF, SESSION_STATE_PENDING);
    REQUIRE(deserialize_command(response, { 0x85, 0x12, 0x30, 0x01 }));
    REQUIRE(response.get_id() == (LAST_SESSION_ID_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(response.get_session_id() == 0x3012);
    REQUIRE(response.get_session_state() == SESSION_STATE_COMPLETED);
    REQUIRE(response.length() == 4);

    LastSessionIDCommandIndication indication(0xFFFF);
    REQUIRE(deserialize_command(indication, { 0x45, 0x12, 0x30, 0x00 }));
    REQUIRE(indication.get_id() == (LAST_SESSION_ID_CODE | COMMAND_INDICATION_BIT));
    REQUIRE(indication.get_session_id() == 0x3012);
    REQUIRE(indication.get_session_state() == SESSION_STATE_PENDING);
    REQUIRE(indication.length() == 4);
}

TEST_CASE("List Sessions IDs command should serialize properly", "[commands]") {
    REQUIRE(serialize_command<ListSessionsIDsCommand>(ListSessionsIDsCommand(SESSION_ID_RELATION_EQUAL_TO), { 0x06, 0x00 }));
    session_id_t sessions[] = { 0x0102, 0x0304 };
    REQUIRE(serialize_command<ListSessionsIDsCommandResponse<6>>(ListSessionsIDsCommandResponse<6>(2, sessions, 2), { 0x86, 0x02, 0x02, 0x01, 0x04, 0x03 }));
}

TEST_CASE("List Sessions IDs command should deserialize properly", "[commands]") {
    ListSessionsIDsCommand command(SESSION_ID_RELATION_GREATER_OR_EQUAL_TO);
    REQUIRE(deserialize_command(command, { 0x06, 0x00 }));
    REQUIRE(command.get_id() == LIST_SESSIONS_IDS_CODE);
    REQUIRE(command.get_session_id_relation() == SESSION_ID_RELATION_EQUAL_TO);
    REQUIRE(command.length() == 2);

    uint16_t sessions[] = { 0xFFFF, 0xFFFF };
    ListSessionsIDsCommandResponse<2> response(0xFF, sessions, 2);
    REQUIRE(deserialize_command(response, { 0x86, 0x02, 0x02, 0x01, 0x04, 0x03 }));
    REQUIRE(response.get_id() == (LIST_SESSIONS_IDS_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(response.get_remaining_session_ids() == 2);
    REQUIRE(response.get_session_ids()[0] == 0x0102);
    REQUIRE(response.get_session_ids()[1] == 0x0304);
    REQUIRE(response.length() == 6);
}

TEST_CASE("Get Session Record command should serialize properly", "[commands]") {
    REQUIRE(serialize_command<GetSessionRecordCommand>(GetSessionRecordCommand(0x2010, SESSION_ID_RELATION_GREATER_OR_EQUAL_TO, 0xFF), 
        { 0x07, 0x10, 0x20, 0x01, 0xFF }));
    REQUIRE(serialize_command<GetSessionRecordEmptyCommandResponse>(GetSessionRecordEmptyCommandResponse(), { 0x87, 0xFF }));
    REQUIRE(serialize_command<GetSessionRecordHeaderCommandResponse>(GetSessionRecordHeaderCommandResponse(0x4050, SESSION_STATE_COMPLETED, 0x40), 
        { 0x87, 0x00, 0x50, 0x40, 0x01, 0x40 }));

    lap_time_t lap_times[] = { 0x01020304, 0x05060708 };
    REQUIRE(serialize_command<GetSessionRecordPayloadCommandResponse<6>>(GetSessionRecordPayloadCommandResponse<6>(lap_times, 2), 
        { 0x87, 0x01, 0x04, 0x03, 0x02, 0x01, 0x08, 0x07, 0x06, 0x05 }));
}

TEST_CASE("Get Session Record command should deserialize properly", "[commands]") {
    GetSessionRecordCommand command(0xFFFF, SESSION_ID_RELATION_EQUAL_TO, 0xFF);
    REQUIRE(deserialize_command(command, { 0x07, 0x10, 0x20, 0x01, 0xFF }));
    REQUIRE(command.get_id() == GET_SESSION_RECORD_CODE);
    REQUIRE(command.get_session_id() == 0x2010);
    REQUIRE(command.get_session_id_relation() == SESSION_ID_RELATION_GREATER_OR_EQUAL_TO);
    REQUIRE(command.get_lap_offset() == 0xFF);
    REQUIRE(command.length() == 5);

    GetSessionRecordEmptyCommandResponse empty;
    REQUIRE(deserialize_command(empty, { 0x87, 0xFF }));
    REQUIRE(empty.get_id() == (GET_SESSION_RECORD_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(empty.length() == 2);

    GetSessionRecordHeaderCommandResponse header(0xFFFF, SESSION_STATE_PENDING, 0xFF);
    REQUIRE(deserialize_command(header, { 0x87, 0x00, 0x50, 0x40, 0x01, 0x40 }));
    REQUIRE(header.get_id() == (GET_SESSION_RECORD_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(header.get_session_id() == 0x4050);
    REQUIRE(header.get_session_state() == SESSION_STATE_COMPLETED);
    REQUIRE(header.get_lap_count() == 0x40);
    REQUIRE(header.length() == 6);

    GetSessionRecordPayloadCommandResponse<6> payload(nullptr, 0);
    REQUIRE(deserialize_command(payload, { 0x87, 0x01, 0x04, 0x03, 0x02, 0x01, 0x08, 0x07, 0x06, 0x05 }));
    REQUIRE(payload.get_id() == (GET_SESSION_RECORD_CODE | COMMAND_RESPONSE_BIT));
    REQUIRE(payload.get_lap_times()[0] == 0x01020304);
    REQUIRE(payload.get_lap_times()[1] == 0x05060708);
    REQUIRE(payload.length() == 10);
}
