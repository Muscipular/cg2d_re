#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::ItemMoveRequest;

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
    const ItemMoveRequest request(10, 20, 3);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(ItemMoveRequest::Code == "yi", "yi code");
    failures += Expect(
        ItemMoveRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "yi direction");
    failures += Expect(ItemMoveRequest::Fields.size() == 3U, "yi fields");
    failures += Expect(ItemMoveRequest::Fields[0].name == "fromindex", "yi fromindex name");
    failures += Expect(ItemMoveRequest::Fields[1].name == "toindex", "yi toindex name");
    failures += Expect(ItemMoveRequest::Fields[2].name == "num", "yi num name");
    for (const cg::packet::PacketFieldSpec& field : ItemMoveRequest::Fields) {
        failures += Expect(field.type == cg::packet::PacketFieldType::Int62, "yi field type");
    }
    failures += Expect(ItemMoveRequest::Spec.code == ItemMoveRequest::Code, "yi Spec code");
    failures += Expect(
        ItemMoveRequest::Spec.direction == ItemMoveRequest::Direction,
        "yi Spec direction");
    failures += Expect(ItemMoveRequest::Spec.fieldCount == 3U, "yi Spec field count");
    failures += Expect(request.fromindex == 10, "yi fromindex member");
    failures += Expect(request.toindex == 20, "yi toindex member");
    failures += Expect(request.num == 3, "yi num member");
    failures += Expect(packet.code == cg::packet::PacketCode("yi"), "yi packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "yi packet direction");
    failures += Expect(packet.fields.size() == 3U, "yi packet fields");
    failures += ExpectField(packet, 0U, "fromindex", "10", "yi fromindex field");
    failures += ExpectField(packet, 1U, "toindex", "20", "yi toindex field");
    failures += ExpectField(packet, 2U, "num", "3", "yi num field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "yi Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "yi a k 3 ", "yi Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("yi a k 3", ItemMoveRequest::Spec);

    int failures = Expect(parsed.HasValue(), "yi generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "fromindex", "10", "yi parsed fromindex");
    failures += ExpectField(parsed.Value(), 1U, "toindex", "20", "yi parsed toindex");
    failures += ExpectField(parsed.Value(), 2U, "num", "3", "yi parsed num");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), ItemMoveRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "yi generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "yi a k 3 ", "yi generic canonical");
    }
    return failures;
}

int TestSignedAndExtremeValues()
{
    int failures = 0;
    const cg::packet::PacketResult<std::string> signedValues =
        ItemMoveRequest(-1, -2, -3).Encode();
    failures += Expect(signedValues.HasValue(), "yi signed values");
    if (signedValues.HasValue()) {
        failures += Expect(signedValues.Value() == "yi -1 -2 -3 ", "yi signed canonical");
    }

    const ItemMoveRequest extremes(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min());
    const cg::packet::PacketResult<std::string> extremeEncoded = extremes.Encode();
    failures += Expect(extremeEncoded.HasValue(), "yi cg3a int extreme values");
    if (extremeEncoded.HasValue()) {
        failures += Expect(
            extremeEncoded.Value() == "yi -2lkCB2 2lkCB1 -2lkCB2 ",
            "yi cg3a int extreme canonical");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 3U>& tokens)
{
    return "yi " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k 3 ", ItemMoveRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "yi wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("yi a k", ItemMoveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "yi",
        true,
        2U,
        "yi missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("yi a k 3 0 ", ItemMoveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "yi",
        true,
        3U,
        "yi extra field");

    const std::array<std::string, 3U> validTokens = {{"a", "k", "3"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 3U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(invalidTokens), ItemMoveRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "yi",
            true,
            index,
            "yi invalid i62");

        std::array<std::string, 3U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(overflowTokens), ItemMoveRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "yi",
            true,
            index,
            "yi i62 overflow");
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
        std::cerr << failures << " item move request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
