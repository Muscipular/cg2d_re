#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::ProfileListsRequest;

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

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode("HoW"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "flg", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const ProfileListsRequest request(3);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(ProfileListsRequest::Code == "HoW", "HoW code");
    failures += Expect(
        ProfileListsRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "HoW direction");
    failures += Expect(ProfileListsRequest::Fields.size() == 1U, "HoW fields");
    failures += Expect(
        ProfileListsRequest::Fields[0].name == "flg",
        "HoW field name");
    failures += Expect(
        ProfileListsRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "HoW field type");
    failures += Expect(
        ProfileListsRequest::Spec.code == ProfileListsRequest::Code,
        "HoW Spec code");
    failures += Expect(
        ProfileListsRequest::Spec.direction == ProfileListsRequest::Direction,
        "HoW Spec direction");
    failures += Expect(
        ProfileListsRequest::Spec.fieldCount == 1U,
        "HoW Spec field count");
    failures += Expect(request.flags == 3, "HoW member");
    failures += ExpectPacket(packet, "3", "HoW packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "HoW Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "HoW 3 ", "HoW Encode canonical");
    }

    const cg::packet::PacketResult<std::string> profileOnly =
        ProfileListsRequest(1).Encode();
    failures += Expect(profileOnly.HasValue(), "HoW flags 1 Encode");
    if (profileOnly.HasValue()) {
        failures += Expect(profileOnly.Value() == "HoW 1 ", "HoW flags 1");
    }

    const cg::packet::PacketResult<std::string> aboutOnly =
        ProfileListsRequest(2).Encode();
    failures += Expect(aboutOnly.HasValue(), "HoW flags 2 Encode");
    if (aboutOnly.HasValue()) {
        failures += Expect(aboutOnly.Value() == "HoW 2 ", "HoW flags 2");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("HoW 3", ProfileListsRequest::Spec);

    int failures = Expect(parsed.HasValue(), "HoW generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "3", "HoW generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), ProfileListsRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "HoW generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "HoW 3 ", "HoW generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int flags;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, "HoW 0 "},
        {4, "HoW 4 "},
        {-1, "HoW -1 "},
        {std::numeric_limits<int>::min(), "HoW -2lkCB2 "},
        {std::numeric_limits<int>::max(), "HoW 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const ProfileListsRequest request(testCase.flags);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.flags),
            "HoW signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "HoW signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "HoW signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 3 ", ProfileListsRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "HoW wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("HoW", ProfileListsRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "HoW",
        true,
        0U,
        "HoW missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("HoW 3 1 ", ProfileListsRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "HoW",
        true,
        1U,
        "HoW extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("HoW _ ", ProfileListsRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "HoW",
        true,
        0U,
        "HoW invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("HoW 2lkCB2 ", ProfileListsRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "HoW",
        true,
        0U,
        "HoW positive i62 overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("HoW -2lkCB3 ", ProfileListsRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "HoW",
        true,
        0U,
        "HoW negative i62 overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestGenericRoundtrip()
        + TestSignedAndBoundaryValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " profile lists request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
