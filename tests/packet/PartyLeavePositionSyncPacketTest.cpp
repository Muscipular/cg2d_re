#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::PartyLeavePositionSync;

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
    failures += Expect(packet.fields[index].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int TestSpecAndEncode()
{
    const PartyLeavePositionSync sync(10, 20, 7);
    const cg::packet::Packet packet = sync.ToPacket();

    int failures = 0;
    failures += Expect(PartyLeavePositionSync::Code == "cZt", "cZt code");
    failures += Expect(
        PartyLeavePositionSync::Direction == cg::packet::PacketDirection::ClientToServer,
        "cZt direction");
    failures += Expect(PartyLeavePositionSync::Fields.size() == 3U, "cZt fields");
    failures += Expect(PartyLeavePositionSync::Fields[0].name == "x", "cZt x name");
    failures += Expect(PartyLeavePositionSync::Fields[1].name == "y", "cZt y name");
    failures += Expect(PartyLeavePositionSync::Fields[2].name == "dir", "cZt dir name");
    for (const cg::packet::PacketFieldSpec& field : PartyLeavePositionSync::Fields) {
        failures += Expect(field.type == cg::packet::PacketFieldType::Int62, "cZt field type");
    }
    failures += Expect(
        PartyLeavePositionSync::Spec.code == PartyLeavePositionSync::Code,
        "cZt Spec code");
    failures += Expect(
        PartyLeavePositionSync::Spec.direction == PartyLeavePositionSync::Direction,
        "cZt Spec direction");
    failures += Expect(PartyLeavePositionSync::Spec.fieldCount == 3U, "cZt Spec field count");
    failures += Expect(sync.x == 10, "cZt x member");
    failures += Expect(sync.y == 20, "cZt y member");
    failures += Expect(sync.direction == 7, "cZt direction member");
    failures += Expect(packet.code == cg::packet::PacketCode("cZt"), "cZt packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "cZt packet direction");
    failures += Expect(packet.fields.size() == 3U, "cZt packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "cZt x field");
    failures += ExpectField(packet, 1U, "y", "20", "cZt y field");
    failures += ExpectField(packet, 2U, "dir", "7", "cZt dir field");

    const cg::packet::PacketResult<std::string> encoded = sync.Encode();
    failures += Expect(encoded.HasValue(), "cZt Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "cZt a k 7 ", "cZt Encode value");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("cZt a k 7", PartyLeavePositionSync::Spec);

    int failures = Expect(parsed.HasValue(), "cZt generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "10", "cZt parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "20", "cZt parsed y");
    failures += ExpectField(parsed.Value(), 2U, "dir", "7", "cZt parsed dir");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), PartyLeavePositionSync::Spec);
    failures += Expect(rebuilt.HasValue(), "cZt generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "cZt a k 7 ", "cZt generic canonical");
    }
    return failures;
}

int TestSignedAndUnrestrictedValues()
{
    int failures = 0;
    const cg::packet::PacketResult<std::string> signedValues =
        PartyLeavePositionSync(-1, -2, -3).Encode();
    failures += Expect(signedValues.HasValue(), "cZt signed values");
    if (signedValues.HasValue()) {
        failures += Expect(signedValues.Value() == "cZt -1 -2 -3 ", "cZt signed canonical");
    }

    const cg::packet::PacketResult<std::string> unrestrictedDirection =
        PartyLeavePositionSync(0, 0, 256).Encode();
    failures += Expect(unrestrictedDirection.HasValue(), "cZt unrestricted direction");
    if (unrestrictedDirection.HasValue()) {
        failures += Expect(
            unrestrictedDirection.Value() == "cZt 0 0 48 ",
            "cZt unrestricted direction canonical");
    }

    const PartyLeavePositionSync extremes(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min());
    const cg::packet::PacketResult<std::string> extremeEncoded = extremes.Encode();
    failures += Expect(extremeEncoded.HasValue(), "cZt extreme values");
    if (extremeEncoded.HasValue()) {
        failures += Expect(
            extremeEncoded.Value() == "cZt -2lkCB2 2lkCB1 -2lkCB2 ",
            "cZt extreme canonical");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 3U>& tokens)
{
    return "cZt " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k 7 ", PartyLeavePositionSync::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "cZt wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("cZt a k", PartyLeavePositionSync::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "cZt",
        true,
        2U,
        "cZt missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("cZt a k 7 0 ", PartyLeavePositionSync::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "cZt",
        true,
        3U,
        "cZt extra field");

    const std::array<std::string, 3U> validTokens = {{"a", "k", "7"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 3U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(invalidTokens), PartyLeavePositionSync::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "cZt",
            true,
            index,
            "cZt invalid i62");

        std::array<std::string, 3U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(overflowTokens), PartyLeavePositionSync::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "cZt",
            true,
            index,
            "cZt i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestGenericRoundtrip()
        + TestSignedAndUnrestrictedValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " party leave position sync packet test(s) failed\n";
        return 1;
    }
    return 0;
}
