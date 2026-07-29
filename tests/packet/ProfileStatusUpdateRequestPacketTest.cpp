#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::ProfileStatusUpdateRequest;

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

int ExpectEncode(int profileStatus, std::string_view expected, const char* message)
{
    const cg::packet::PacketResult<std::string> encoded =
        ProfileStatusUpdateRequest(profileStatus).Encode();
    int failures = Expect(encoded.HasValue(), message);
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == expected, message);
    }
    return failures;
}

int TestSpecToPacketAndEncode()
{
    const ProfileStatusUpdateRequest request(1);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(ProfileStatusUpdateRequest::Code == "lz", "lz code");
    failures += Expect(
        ProfileStatusUpdateRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "lz direction");
    failures += Expect(ProfileStatusUpdateRequest::Fields.size() == 1U, "lz fields");
    failures += Expect(
        ProfileStatusUpdateRequest::Fields[0].name == "profileStatus",
        "lz field name");
    failures += Expect(
        ProfileStatusUpdateRequest::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "lz field type");
    failures += Expect(
        ProfileStatusUpdateRequest::Spec.code == ProfileStatusUpdateRequest::Code,
        "lz Spec code");
    failures += Expect(
        ProfileStatusUpdateRequest::Spec.direction == ProfileStatusUpdateRequest::Direction,
        "lz Spec direction");
    failures += Expect(ProfileStatusUpdateRequest::Spec.fieldCount == 1U, "lz Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("lz"), "lz packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "lz packet direction");
    failures += Expect(packet.fields.size() == 1U, "lz packet fields");
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "profileStatus", "lz packet field name");
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            "lz packet field type");
        failures += Expect(packet.fields[0].value == "1", "lz packet field value");
    }

    failures += ExpectEncode(0, "lz 0 ", "lz Encode zero");
    failures += ExpectEncode(1, "lz 1 ", "lz Encode one");
    failures += ExpectEncode(2, "lz 2 ", "lz Encode non-boolean");
    return failures;
}

int TestGenericCanonicalRoundtrip()
{
    constexpr std::array<std::string_view, 3U> PlainValues = {{
        "lz 0 ",
        "lz 1 ",
        "lz 2 ",
    }};

    int failures = 0;
    for (const std::string_view plain : PlainValues) {
        const cg::packet::PacketResult<cg::packet::Packet> parsed =
            cg::packet::ParsePacket(plain, ProfileStatusUpdateRequest::Spec);
        failures += Expect(parsed.HasValue(), "lz ParsePacket");
        if (!parsed.HasValue()) {
            continue;
        }

        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(parsed.Value(), ProfileStatusUpdateRequest::Spec);
        failures += Expect(rebuilt.HasValue(), "lz BuildPacket");
        if (!rebuilt.HasValue()) {
            continue;
        }
        failures += Expect(rebuilt.Value() == plain, "lz canonical");

        const cg::packet::PacketResult<cg::packet::Packet> reparsed =
            cg::packet::ParsePacket(rebuilt.Value(), ProfileStatusUpdateRequest::Spec);
        failures += Expect(reparsed.HasValue(), "lz reparse canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 1 ", ProfileStatusUpdateRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "lz wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("lz", ProfileStatusUpdateRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "lz",
        true,
        0U,
        "lz missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("lz 1 2 ", ProfileStatusUpdateRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "lz",
        true,
        1U,
        "lz extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("lz _ ", ProfileStatusUpdateRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lz",
        true,
        0U,
        "lz invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("lz 2lkCB2 ", ProfileStatusUpdateRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lz",
        true,
        0U,
        "lz i62 overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecToPacketAndEncode() + TestGenericCanonicalRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " profile status update request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
