#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::PickUpItem;

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
    const PickUpItem item(10, 20, 3);
    const cg::packet::Packet packet = item.ToPacket();

    int failures = 0;
    failures += Expect(PickUpItem::Code == "fIM", "fIM code");
    failures += Expect(
        PickUpItem::Direction == cg::packet::PacketDirection::ClientToServer,
        "fIM direction");
    failures += Expect(PickUpItem::Fields.size() == 3U, "fIM fields");
    failures += Expect(PickUpItem::Fields[0].name == "x", "fIM x name");
    failures += Expect(PickUpItem::Fields[1].name == "y", "fIM y name");
    failures += Expect(PickUpItem::Fields[2].name == "dir", "fIM dir name");
    for (const cg::packet::PacketFieldSpec& field : PickUpItem::Fields) {
        failures += Expect(field.type == cg::packet::PacketFieldType::Int62, "fIM field type");
    }
    failures += Expect(PickUpItem::Spec.code == PickUpItem::Code, "fIM Spec code");
    failures += Expect(
        PickUpItem::Spec.direction == PickUpItem::Direction,
        "fIM Spec direction");
    failures += Expect(PickUpItem::Spec.fieldCount == 3U, "fIM Spec field count");
    failures += Expect(item.x == 10, "fIM x member");
    failures += Expect(item.y == 20, "fIM y member");
    failures += Expect(item.direction == 3, "fIM direction member");
    failures += Expect(packet.code == cg::packet::PacketCode("fIM"), "fIM packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "fIM packet direction");
    failures += Expect(packet.fields.size() == 3U, "fIM packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "fIM x field");
    failures += ExpectField(packet, 1U, "y", "20", "fIM y field");
    failures += ExpectField(packet, 2U, "dir", "3", "fIM dir field");

    const cg::packet::PacketResult<std::string> encoded = item.Encode();
    failures += Expect(encoded.HasValue(), "fIM Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "fIM a k 3 ", "fIM Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("fIM a k 3", PickUpItem::Spec);

    int failures = Expect(parsed.HasValue(), "fIM generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "10", "fIM parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "20", "fIM parsed y");
    failures += ExpectField(parsed.Value(), 2U, "dir", "3", "fIM parsed dir");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), PickUpItem::Spec);
    failures += Expect(rebuilt.HasValue(), "fIM generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "fIM a k 3 ", "fIM generic canonical");
    }
    return failures;
}

int TestSignedAndUnrestrictedValues()
{
    int failures = 0;
    const cg::packet::PacketResult<std::string> signedValues =
        PickUpItem(-1, -2, -3).Encode();
    failures += Expect(signedValues.HasValue(), "fIM signed values");
    if (signedValues.HasValue()) {
        failures += Expect(signedValues.Value() == "fIM -1 -2 -3 ", "fIM signed canonical");
    }

    const cg::packet::PacketResult<std::string> unrestrictedDirection =
        PickUpItem(0, 0, 256).Encode();
    failures += Expect(unrestrictedDirection.HasValue(), "fIM unrestricted direction");
    if (unrestrictedDirection.HasValue()) {
        failures += Expect(
            unrestrictedDirection.Value() == "fIM 0 0 48 ",
            "fIM unrestricted direction canonical");
    }

    const PickUpItem extremes(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min());
    const cg::packet::PacketResult<std::string> extremeEncoded = extremes.Encode();
    failures += Expect(extremeEncoded.HasValue(), "fIM extreme values");
    if (extremeEncoded.HasValue()) {
        failures += Expect(
            extremeEncoded.Value() == "fIM -2lkCB2 2lkCB1 -2lkCB2 ",
            "fIM extreme canonical");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 3U>& tokens)
{
    return "fIM " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ';
}

int TestLegacySeparatorsRejected()
{
    std::string betweenXAndY = "fIM a";
    betweenXAndY.push_back('\x17');
    betweenXAndY.append("k 3 ");

    std::string betweenYAndDirection = "fIM a k";
    betweenYAndDirection.push_back('\x17');
    betweenYAndDirection.append("3 ");

    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket(betweenXAndY, PickUpItem::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "fIM",
        true,
        0U,
        "fIM legacy x/y separator rejected at x");
    failures += ExpectError(
        cg::packet::ParsePacket(betweenYAndDirection, PickUpItem::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "fIM",
        true,
        1U,
        "fIM legacy y/dir separator rejected at y");
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k 3 ", PickUpItem::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "fIM wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("fIM a k", PickUpItem::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "fIM",
        true,
        2U,
        "fIM missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("fIM a k 3 0 ", PickUpItem::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "fIM",
        true,
        3U,
        "fIM extra field");

    const std::array<std::string, 3U> validTokens = {{"a", "k", "3"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 3U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(invalidTokens), PickUpItem::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "fIM",
            true,
            index,
            "fIM invalid i62");

        std::array<std::string, 3U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(overflowTokens), PickUpItem::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "fIM",
            true,
            index,
            "fIM i62 overflow");
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
        + TestLegacySeparatorsRejected()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " pick up item packet test(s) failed\n";
        return 1;
    }
    return 0;
}
