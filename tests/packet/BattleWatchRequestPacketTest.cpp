#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::BattleWatchRequest;

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
    const BattleWatchRequest request(10, 20);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(BattleWatchRequest::Code == "IMv", "IMv code");
    failures += Expect(
        BattleWatchRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "IMv direction");
    failures += Expect(BattleWatchRequest::Fields.size() == 2U, "IMv fields");
    failures += Expect(BattleWatchRequest::Fields[0].name == "x", "IMv x name");
    failures += Expect(BattleWatchRequest::Fields[1].name == "y", "IMv y name");
    for (const cg::packet::PacketFieldSpec& field : BattleWatchRequest::Fields) {
        failures += Expect(field.type == cg::packet::PacketFieldType::Int62, "IMv field type");
    }
    failures += Expect(BattleWatchRequest::Spec.code == BattleWatchRequest::Code, "IMv Spec code");
    failures += Expect(
        BattleWatchRequest::Spec.direction == BattleWatchRequest::Direction,
        "IMv Spec direction");
    failures += Expect(BattleWatchRequest::Spec.fieldCount == 2U, "IMv Spec field count");
    failures += Expect(request.x == 10, "IMv x member");
    failures += Expect(request.y == 20, "IMv y member");
    failures += Expect(packet.code == cg::packet::PacketCode("IMv"), "IMv packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "IMv packet direction");
    failures += Expect(packet.fields.size() == 2U, "IMv packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "IMv x field");
    failures += ExpectField(packet, 1U, "y", "20", "IMv y field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "IMv Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "IMv a k ", "IMv Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("IMv a k", BattleWatchRequest::Spec);

    int failures = Expect(parsed.HasValue(), "IMv generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "10", "IMv parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "20", "IMv parsed y");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), BattleWatchRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "IMv generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "IMv a k ", "IMv generic canonical");
    }
    return failures;
}

int TestSignedAndExtremeValues()
{
    int failures = 0;
    const cg::packet::PacketResult<std::string> signedValues =
        BattleWatchRequest(-1, -2).Encode();
    failures += Expect(signedValues.HasValue(), "IMv signed values");
    if (signedValues.HasValue()) {
        failures += Expect(signedValues.Value() == "IMv -1 -2 ", "IMv signed canonical");
    }

    const BattleWatchRequest extremes(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max());
    const cg::packet::PacketResult<std::string> extremeEncoded = extremes.Encode();
    failures += Expect(extremeEncoded.HasValue(), "IMv extreme values");
    if (extremeEncoded.HasValue()) {
        failures += Expect(
            extremeEncoded.Value() == "IMv -2lkCB2 2lkCB1 ",
            "IMv extreme canonical");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 2U>& tokens)
{
    return "IMv " + tokens[0] + ' ' + tokens[1] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k ", BattleWatchRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "IMv wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("IMv a", BattleWatchRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IMv",
        true,
        1U,
        "IMv missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("IMv a k 3 ", BattleWatchRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IMv",
        true,
        2U,
        "IMv extra field");

    const std::array<std::string, 2U> validTokens = {{"a", "k"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 2U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(invalidTokens), BattleWatchRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "IMv",
            true,
            index,
            "IMv invalid i62");

        std::array<std::string, 2U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(overflowTokens), BattleWatchRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "IMv",
            true,
            index,
            "IMv i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestGenericRoundtrip()
        + TestSignedAndExtremeValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " battle watch request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
