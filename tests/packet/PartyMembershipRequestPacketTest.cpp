#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::PartyMembershipRequest;

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

template <typename T>
int ExpectError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::string_view packetCode,
    bool hasFieldIndex,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    const cg::packet::PacketError& error = result.Error();
    failures += Expect(error.code == code, message);
    failures += Expect(error.packetCode.has_value(), message);
    if (error.packetCode.has_value()) {
        failures += Expect(error.packetCode->View() == packetCode, message);
    }
    failures += Expect(error.fieldIndex.has_value() == hasFieldIndex, message);
    if (hasFieldIndex && error.fieldIndex.has_value()) {
        failures += Expect(*error.fieldIndex == fieldIndex, message);
    }
    return failures;
}

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(
        packet.fields[index].type == cg::packet::PacketFieldType::Int62,
        message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view x,
    std::string_view y,
    std::string_view request,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode(PartyMembershipRequest::Code),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "x", x, message);
    failures += ExpectField(packet, 1U, "y", y, message);
    failures += ExpectField(packet, 2U, "join", request, message);
    return failures;
}

int TestSpecAndEncode()
{
    const PartyMembershipRequest request(10, 20, 1);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(PartyMembershipRequest::Code == "zn", "zn code");
    failures += Expect(
        PartyMembershipRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "zn direction");
    failures += Expect(
        PartyMembershipRequest::Fields.size() == 3U,
        "zn field count");
    constexpr std::array<std::string_view, 3U> fieldNames = {{
        "x",
        "y",
        "join",
    }};
    for (std::size_t index = 0U; index < fieldNames.size(); ++index) {
        failures += Expect(
            PartyMembershipRequest::Fields[index].name == fieldNames[index],
            "zn field name");
        failures += Expect(
            PartyMembershipRequest::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "zn field type");
    }
    failures += Expect(
        PartyMembershipRequest::Spec.code == PartyMembershipRequest::Code,
        "zn Spec code");
    failures += Expect(
        PartyMembershipRequest::Spec.direction ==
            PartyMembershipRequest::Direction,
        "zn Spec direction");
    failures += Expect(
        PartyMembershipRequest::Spec.fieldCount == 3U,
        "zn Spec field count");
    failures += Expect(
        request.x == 10 && request.y == 20 && request.request == 1,
        "zn members");
    failures += ExpectPacket(packet, "10", "20", "1", "zn packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "zn Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "zn a k 1 ",
            "zn Encode canonical");
    }
    return failures;
}

int TestRequestValues()
{
    constexpr struct
    {
        int request;
        std::string_view expected;
    } Cases[] = {
        {0, "zn a k 0 "},
        {1, "zn a k 1 "},
        {2, "zn a k 2 "},
        {-1, "zn a k -1 "},
    };

    int failures = 0;
    for (const auto& testCase : Cases) {
        const PartyMembershipRequest request(10, 20, testCase.request);
        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "zn request Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "zn request canonical");
        }
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "zn a k 2",
            PartyMembershipRequest::Spec);

    int failures = Expect(parsed.HasValue(), "zn generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "20", "2", "zn generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            PartyMembershipRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "zn generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "zn a k 2 ", "zn generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    constexpr struct
    {
        int x;
        int y;
        int request;
        std::string_view expected;
    } Cases[] = {
        {0, 0, 0, "zn 0 0 0 "},
        {-10, -20, -2, "zn -a -k -2 "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            -1,
            "zn -2lkCB2 2lkCB1 -1 ",
        },
        {
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            "zn 2lkCB1 -2lkCB2 2lkCB1 ",
        },
    };

    int failures = 0;
    for (const auto& testCase : Cases) {
        const PartyMembershipRequest request(
            testCase.x,
            testCase.y,
            testCase.request);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.x),
            std::to_string(testCase.y),
            std::to_string(testCase.request),
            "zn signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "zn signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "zn signed canonical");
        }
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "zn -2lkCB2 2lkCB1 -1 ",
            PartyMembershipRequest::Spec);
    failures += Expect(parsed.HasValue(), "zn int boundary parse");
    if (parsed.HasValue()) {
        failures += ExpectPacket(
            parsed.Value(),
            std::to_string(std::numeric_limits<int>::min()),
            std::to_string(std::numeric_limits<int>::max()),
            "-1",
            "zn int boundary packet");
    }
    return failures;
}

int TestArityErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k 1 ", PartyMembershipRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "zn wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("zn", PartyMembershipRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zn",
        true,
        0U,
        "zn missing all fields");
    failures += ExpectError(
        cg::packet::ParsePacket("zn a", PartyMembershipRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zn",
        true,
        1U,
        "zn missing y and request");
    failures += ExpectError(
        cg::packet::ParsePacket("zn a k", PartyMembershipRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zn",
        true,
        2U,
        "zn missing request");
    failures += ExpectError(
        cg::packet::ParsePacket("zn a k 1 2 ", PartyMembershipRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zn",
        true,
        3U,
        "zn extra field");
    return failures;
}

int TestFieldErrors()
{
    constexpr struct
    {
        std::string_view wire;
        std::size_t fieldIndex;
        const char* message;
    } Cases[] = {
        {"zn _ k 1 ", 0U, "zn invalid x"},
        {"zn a _ 1 ", 1U, "zn invalid y"},
        {"zn a k _ ", 2U, "zn invalid request"},
        {"zn 2lkCB2 k 1 ", 0U, "zn positive x overflow"},
        {"zn a 2lkCB2 1 ", 1U, "zn positive y overflow"},
        {"zn a k 2lkCB2 ", 2U, "zn positive request overflow"},
        {"zn -2lkCB3 k 1 ", 0U, "zn negative x overflow"},
        {"zn a -2lkCB3 1 ", 1U, "zn negative y overflow"},
        {"zn a k -2lkCB3 ", 2U, "zn negative request overflow"},
    };

    int failures = 0;
    for (const auto& testCase : Cases) {
        failures += ExpectError(
            cg::packet::ParsePacket(
                testCase.wire,
                PartyMembershipRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "zn",
            true,
            testCase.fieldIndex,
            testCase.message);
    }

    const std::string pollutedCode = "zn\x17" "a";
    const std::string pollutedWire = pollutedCode + " k 1";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedWire,
            PartyMembershipRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "zn polluted code");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestRequestValues()
        + TestGenericRoundtrip()
        + TestSignedAndBoundaryValues()
        + TestArityErrors()
        + TestFieldErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " party membership request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
