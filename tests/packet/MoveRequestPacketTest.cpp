#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::MoveRequest;

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
    cg::packet::PacketFieldType type,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }
    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int TestSpecAndEncode()
{
    const MoveRequest request(10, 20, "a", 256);
    const cg::packet::PacketResult<cg::packet::Packet> packet = request.ToPacket();

    int failures = 0;
    failures += Expect(MoveRequest::Code == "zA", "zA code");
    failures += Expect(
        MoveRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "zA direction");
    failures += Expect(MoveRequest::Fields.size() == 4U, "zA fields");
    failures += Expect(MoveRequest::Fields[0].name == "x", "zA x name");
    failures += Expect(MoveRequest::Fields[1].name == "y", "zA y name");
    failures += Expect(MoveRequest::Fields[2].name == "direction", "zA direction name");
    failures += Expect(MoveRequest::Fields[3].name == "warpnum", "zA warpnum name");
    failures += Expect(
        MoveRequest::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "zA x type");
    failures += Expect(
        MoveRequest::Fields[1].type == cg::packet::PacketFieldType::Int62,
        "zA y type");
    failures += Expect(
        MoveRequest::Fields[2].type == cg::packet::PacketFieldType::EscapedString,
        "zA direction type");
    failures += Expect(
        MoveRequest::Fields[3].type == cg::packet::PacketFieldType::Int62,
        "zA warpnum type");
    failures += Expect(MoveRequest::Spec.code == MoveRequest::Code, "zA Spec code");
    failures += Expect(MoveRequest::Spec.direction == MoveRequest::Direction, "zA Spec direction");
    failures += Expect(MoveRequest::Spec.fieldCount == 4U, "zA Spec field count");
    failures += Expect(request.x == 10, "zA x member");
    failures += Expect(request.y == 20, "zA y member");
    failures += Expect(request.direction == "a", "zA direction member");
    failures += Expect(request.warpNumber == 256, "zA warp number member");
    failures += Expect(packet.HasValue(), "zA ToPacket");
    if (packet.HasValue()) {
        failures += Expect(packet.Value().code == cg::packet::PacketCode("zA"), "zA packet code");
        failures += Expect(
            packet.Value().direction == cg::packet::PacketDirection::ClientToServer,
            "zA packet direction");
        failures += Expect(packet.Value().fields.size() == 4U, "zA packet fields");
        failures += ExpectField(
            packet.Value(),
            0U,
            "x",
            cg::packet::PacketFieldType::Int62,
            "10",
            "zA x field");
        failures += ExpectField(
            packet.Value(),
            1U,
            "y",
            cg::packet::PacketFieldType::Int62,
            "20",
            "zA y field");
        failures += ExpectField(
            packet.Value(),
            2U,
            "direction",
            cg::packet::PacketFieldType::EscapedString,
            "a",
            "zA direction field");
        failures += ExpectField(
            packet.Value(),
            3U,
            "warpnum",
            cg::packet::PacketFieldType::Int62,
            "256",
            "zA warpnum field");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "zA Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "zA a k a 48 ", "zA Encode value");
    }
    return failures;
}

int TestGenericRoundtripAndSignedValues()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("zA a k a 48", MoveRequest::Spec);

    int failures = Expect(parsed.HasValue(), "zA generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(
        parsed.Value(),
        0U,
        "x",
        cg::packet::PacketFieldType::Int62,
        "10",
        "zA parsed x");
    failures += ExpectField(
        parsed.Value(),
        1U,
        "y",
        cg::packet::PacketFieldType::Int62,
        "20",
        "zA parsed y");
    failures += ExpectField(
        parsed.Value(),
        2U,
        "direction",
        cg::packet::PacketFieldType::EscapedString,
        "a",
        "zA parsed direction");
    failures += ExpectField(
        parsed.Value(),
        3U,
        "warpnum",
        cg::packet::PacketFieldType::Int62,
        "256",
        "zA parsed warpnum");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), MoveRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "zA generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "zA a k a 48 ", "zA generic canonical");
    }

    const cg::packet::PacketResult<std::string> signedValues =
        MoveRequest(-1, -2, "H", -3).Encode();
    failures += Expect(signedValues.HasValue(), "zA signed values");
    if (signedValues.HasValue()) {
        failures += Expect(signedValues.Value() == "zA -1 -2 H -3 ", "zA signed canonical");
    }
    return failures;
}

int TestDirectionDomain()
{
    int failures = 0;
    constexpr std::string_view validDirections = "abcdefghABCDEFGH";
    for (const char direction : validDirections) {
        const cg::packet::PacketResult<std::string> encoded =
            MoveRequest(0, 0, std::string(1U, direction), 0).Encode();
        failures += Expect(encoded.HasValue(), "zA valid direction");
    }

    std::string highByteDirection(1U, static_cast<char>(0x81U));
    const std::array<std::string, 6U> invalidDirections = {{
        "",
        "i",
        "I",
        "aa",
        "0",
        highByteDirection,
    }};
    for (const std::string& direction : invalidDirections) {
        const MoveRequest request(0, 0, direction, 0);
        const cg::packet::PacketResult<cg::packet::Packet> packet = request.ToPacket();
        failures += ExpectError(
            packet,
            cg::packet::PacketErrorCode::EncodeFailed,
            "zA",
            true,
            2U,
            "zA invalid direction ToPacket");
        if (packet.HasError()) {
            failures += Expect(!packet.Error().message.empty(), "zA invalid direction message");
        }
        failures += ExpectError(
            request.Encode(),
            cg::packet::PacketErrorCode::EncodeFailed,
            "zA",
            true,
            2U,
            "zA invalid direction Encode");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 4U>& tokens)
{
    return "zA " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ' + tokens[3] + ' ';
}

int TestGenericErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k a 48 ", MoveRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "zA wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("zA a k a", MoveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zA",
        true,
        3U,
        "zA missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("zA a k a 48 0 ", MoveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zA",
        true,
        4U,
        "zA extra field");

    constexpr std::array<std::size_t, 3U> intFieldIndices = {{0U, 1U, 3U}};
    const std::array<std::string, 4U> validTokens = {{"a", "k", "a", "48"}};
    for (const std::size_t index : intFieldIndices) {
        std::array<std::string, 4U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(invalidTokens), MoveRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "zA",
            true,
            index,
            "zA invalid i62");

        std::array<std::string, 4U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(overflowTokens), MoveRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "zA",
            true,
            index,
            "zA i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestGenericRoundtripAndSignedValues()
        + TestDirectionDomain()
        + TestGenericErrors();
    if (failures != 0) {
        std::cerr << failures << " move request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
