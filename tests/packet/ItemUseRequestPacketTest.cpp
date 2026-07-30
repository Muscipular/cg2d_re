#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::ItemUseRequest;

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
    const ItemUseRequest request(10, 20, 8, 1);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(ItemUseRequest::Code == "Ak", "Ak code");
    failures += Expect(
        ItemUseRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "Ak direction");
    failures += Expect(ItemUseRequest::Fields.size() == 4U, "Ak fields");
    failures += Expect(ItemUseRequest::Fields[0].name == "x", "Ak x name");
    failures += Expect(ItemUseRequest::Fields[1].name == "y", "Ak y name");
    failures += Expect(
        ItemUseRequest::Fields[2].name == "haveitemindex",
        "Ak haveitemindex name");
    failures += Expect(ItemUseRequest::Fields[3].name == "toindex", "Ak toindex name");
    for (const cg::packet::PacketFieldSpec& field : ItemUseRequest::Fields) {
        failures += Expect(field.type == cg::packet::PacketFieldType::Int62, "Ak field type");
    }
    failures += Expect(ItemUseRequest::Spec.code == ItemUseRequest::Code, "Ak Spec code");
    failures += Expect(
        ItemUseRequest::Spec.direction == ItemUseRequest::Direction,
        "Ak Spec direction");
    failures += Expect(ItemUseRequest::Spec.fieldCount == 4U, "Ak Spec field count");
    failures += Expect(request.x == 10, "Ak x member");
    failures += Expect(request.y == 20, "Ak y member");
    failures += Expect(request.haveitemindex == 8, "Ak haveitemindex member");
    failures += Expect(request.toindex == 1, "Ak toindex member");
    failures += Expect(packet.code == cg::packet::PacketCode("Ak"), "Ak packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "Ak packet direction");
    failures += Expect(packet.fields.size() == 4U, "Ak packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "Ak x field");
    failures += ExpectField(packet, 1U, "y", "20", "Ak y field");
    failures += ExpectField(packet, 2U, "haveitemindex", "8", "Ak haveitemindex field");
    failures += ExpectField(packet, 3U, "toindex", "1", "Ak toindex field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "Ak Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "Ak a k 8 1 ", "Ak Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("Ak a k 8 1", ItemUseRequest::Spec);

    int failures = Expect(parsed.HasValue(), "Ak generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "10", "Ak parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "20", "Ak parsed y");
    failures += ExpectField(
        parsed.Value(),
        2U,
        "haveitemindex",
        "8",
        "Ak parsed haveitemindex");
    failures += ExpectField(parsed.Value(), 3U, "toindex", "1", "Ak parsed toindex");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), ItemUseRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "Ak generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "Ak a k 8 1 ", "Ak generic canonical");
    }
    return failures;
}

int TestSignedAndExtremeValues()
{
    int failures = 0;
    const cg::packet::PacketResult<std::string> signedValues =
        ItemUseRequest(-1, -2, -3, -4).Encode();
    failures += Expect(signedValues.HasValue(), "Ak signed values");
    if (signedValues.HasValue()) {
        failures += Expect(
            signedValues.Value() == "Ak -1 -2 -3 -4 ",
            "Ak signed canonical");
    }

    const ItemUseRequest extremes(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max());
    const cg::packet::PacketResult<std::string> extremeEncoded = extremes.Encode();
    failures += Expect(extremeEncoded.HasValue(), "Ak cg3a int extreme values");
    if (extremeEncoded.HasValue()) {
        failures += Expect(
            extremeEncoded.Value() == "Ak -2lkCB2 2lkCB1 -2lkCB2 2lkCB1 ",
            "Ak cg3a int extreme canonical");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 4U>& tokens)
{
    return "Ak " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ' + tokens[3] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k 8 1 ", ItemUseRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Ak wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("Ak a k 8", ItemUseRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Ak",
        true,
        3U,
        "Ak missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("Ak a k 8 1 0 ", ItemUseRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Ak",
        true,
        4U,
        "Ak extra field");

    const std::array<std::string, 4U> validTokens = {{"a", "k", "8", "1"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 4U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(invalidTokens), ItemUseRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Ak",
            true,
            index,
            "Ak invalid i62");

        std::array<std::string, 4U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(overflowTokens), ItemUseRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Ak",
            true,
            index,
            "Ak i62 overflow");
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
        std::cerr << failures << " item use request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
