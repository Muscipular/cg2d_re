#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::send::DropGold::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::send::DropGold::Spec);
}

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
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    const cg::packet::PacketError& error = result.Error();
    failures += Expect(error.code == code, message);
    failures += Expect(error.packetCode.has_value(), message);
    if (error.packetCode.has_value()) {
        failures += Expect(error.packetCode->View() == packetCode, message);
    }

    if (hasFieldIndex) {
        failures += Expect(error.fieldIndex.has_value(), message);
        if (error.fieldIndex.has_value()) {
            failures += Expect(*error.fieldIndex == fieldIndex, message);
        }
    } else {
        failures += Expect(!error.fieldIndex.has_value(), message);
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
    int failures = 0;
    failures += Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectDropGoldPacket(
    const cg::packet::Packet& packet,
    std::string_view x,
    std::string_view y,
    std::string_view amount,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::DropGold::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "x", x, message);
    failures += ExpectField(packet, 1U, "y", y, message);
    failures += ExpectField(packet, 2U, "amount", amount, message);
    return failures;
}

int TestToPacketAndEncode()
{
    const cg::packet::send::DropGold request(1, 2, 3);

    int failures = ExpectDropGoldPacket(request.ToPacket(), "1", "2", "3", "woz ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "woz BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "woz 1 2 3 ", "woz BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "woz Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "woz 1 2 3 ", "woz Encode value");
    }
    return failures;
}

int TestInt62Values()
{
    const cg::packet::send::DropGold request(10, -1, 62);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "woz Encode i62 values");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "woz a -1 10 ", "woz i62 encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "woz parse i62 values");
    if (parsed.HasValue()) {
        failures += ExpectDropGoldPacket(parsed.Value(), "10", "-1", "62", "woz parsed i62 values");
    }
    return failures;
}

int TestParsePacketRoundtrip()
{
    const cg::packet::send::DropGold request(1, 2, 3);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "woz Encode roundtrip");
    if (!encoded.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "woz ParsePacket roundtrip");
    if (parsed.HasValue()) {
        failures += ExpectDropGoldPacket(parsed.Value(), "1", "2", "3", "woz roundtrip packet");
    }
    return failures;
}

int TestParseNoTrailingSpace()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket("woz 1 2 3");

    int failures = Expect(static_cast<bool>(parsed), "woz parse no trailing space");
    if (parsed.HasValue()) {
        failures += ExpectDropGoldPacket(parsed.Value(), "1", "2", "3", "woz no trailing space packet");
    }
    return failures;
}

int TestParseEmptyAmountAsZero()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket("woz 1 2  ");

    int failures = Expect(static_cast<bool>(parsed), "woz parse empty amount");
    if (parsed.HasValue()) {
        failures += ExpectDropGoldPacket(parsed.Value(), "1", "2", "0", "woz empty amount packet");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("woz 1 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "woz",
        true,
        2U,
        "woz missing field");
    failures += ExpectError(
        ParsePacket("woz 1 2 3 4 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "woz",
        true,
        3U,
        "woz extra field");
    failures += ExpectError(
        ParsePacket("woz _ 2 3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "woz",
        true,
        0U,
        "woz bad x");
    failures += ExpectError(
        ParsePacket("woz 1 _ 3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "woz",
        true,
        1U,
        "woz bad y");
    failures += ExpectError(
        ParsePacket("woz 1 2 _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "woz",
        true,
        2U,
        "woz bad amount");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = cg::packet::send::DropGold(1, 2, 3).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "woz",
        false,
        0U,
        "woz direction mismatch");

    cg::packet::Packet wrongName = cg::packet::send::DropGold(1, 2, 3).ToPacket();
    wrongName.fields[1].name = "wrong";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "woz",
        true,
        1U,
        "woz field name mismatch");

    cg::packet::Packet wrongType = cg::packet::send::DropGold(1, 2, 3).ToPacket();
    wrongType.fields[2].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "woz",
        true,
        2U,
        "woz field type mismatch");

    cg::packet::Packet emptyValue = cg::packet::send::DropGold(1, 2, 3).ToPacket();
    emptyValue.fields[2].value.clear();
    failures += ExpectError(
        BuildPacket(emptyValue),
        cg::packet::PacketErrorCode::EncodeFailed,
        "woz",
        true,
        2U,
        "woz empty i62 value");

    cg::packet::Packet badValue = cg::packet::send::DropGold(1, 2, 3).ToPacket();
    badValue.fields[2].value = "abc";
    failures += ExpectError(
        BuildPacket(badValue),
        cg::packet::PacketErrorCode::EncodeFailed,
        "woz",
        true,
        2U,
        "woz bad i62 value");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestInt62Values();
    failures += TestParsePacketRoundtrip();
    failures += TestParseNoTrailingSpace();
    failures += TestParseEmptyAmountAsZero();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
