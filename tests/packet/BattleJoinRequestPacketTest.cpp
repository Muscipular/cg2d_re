#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::BattleJoinRequest;

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
    const BattleJoinRequest request(10, 20);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(BattleJoinRequest::Code == "cEj", "cEj code");
    failures += Expect(
        BattleJoinRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "cEj direction");
    failures += Expect(BattleJoinRequest::Fields.size() == 2U, "cEj fields");
    failures += Expect(BattleJoinRequest::Fields[0].name == "x", "cEj x name");
    failures += Expect(BattleJoinRequest::Fields[1].name == "y", "cEj y name");
    for (const cg::packet::PacketFieldSpec& field : BattleJoinRequest::Fields) {
        failures += Expect(field.type == cg::packet::PacketFieldType::Int62, "cEj field type");
    }
    failures += Expect(BattleJoinRequest::Spec.code == BattleJoinRequest::Code, "cEj Spec code");
    failures += Expect(
        BattleJoinRequest::Spec.direction == BattleJoinRequest::Direction,
        "cEj Spec direction");
    failures += Expect(BattleJoinRequest::Spec.fieldCount == 2U, "cEj Spec field count");
    failures += Expect(request.x == 10, "cEj x member");
    failures += Expect(request.y == 20, "cEj y member");
    failures += Expect(packet.code == cg::packet::PacketCode("cEj"), "cEj packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "cEj packet direction");
    failures += Expect(packet.fields.size() == 2U, "cEj packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "cEj x field");
    failures += ExpectField(packet, 1U, "y", "20", "cEj y field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "cEj Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "cEj a k ", "cEj Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("cEj a k", BattleJoinRequest::Spec);

    int failures = Expect(parsed.HasValue(), "cEj generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "10", "cEj parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "20", "cEj parsed y");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), BattleJoinRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "cEj generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "cEj a k ", "cEj generic canonical");
    }
    return failures;
}

int TestSignedAndExtremeValues()
{
    int failures = 0;
    const cg::packet::PacketResult<std::string> signedValues =
        BattleJoinRequest(-1, -2).Encode();
    failures += Expect(signedValues.HasValue(), "cEj signed values");
    if (signedValues.HasValue()) {
        failures += Expect(signedValues.Value() == "cEj -1 -2 ", "cEj signed canonical");
    }

    const BattleJoinRequest extremes(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max());
    const cg::packet::PacketResult<std::string> extremeEncoded = extremes.Encode();
    failures += Expect(extremeEncoded.HasValue(), "cEj extreme values");
    if (extremeEncoded.HasValue()) {
        failures += Expect(
            extremeEncoded.Value() == "cEj -2lkCB2 2lkCB1 ",
            "cEj extreme canonical");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 2U>& tokens)
{
    return "cEj " + tokens[0] + ' ' + tokens[1] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k ", BattleJoinRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "cEj wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("cEj a", BattleJoinRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "cEj",
        true,
        1U,
        "cEj missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("cEj a k 3 ", BattleJoinRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "cEj",
        true,
        2U,
        "cEj extra field");

    const std::array<std::string, 2U> validTokens = {{"a", "k"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 2U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(invalidTokens), BattleJoinRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "cEj",
            true,
            index,
            "cEj invalid i62");

        std::array<std::string, 2U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(overflowTokens), BattleJoinRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "cEj",
            true,
            index,
            "cEj i62 overflow");
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
        std::cerr << failures << " battle join request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
