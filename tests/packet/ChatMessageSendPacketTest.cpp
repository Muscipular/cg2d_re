#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

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
    cg::packet::PacketFieldType type,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].source == cg::packet::PacketSourceTag::Xlsx, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectChatMessageSendPacket(
    const cg::packet::Packet& packet,
    std::string_view x,
    std::string_view y,
    std::string_view msg,
    std::string_view color,
    std::string_view area,
    std::string_view fontSize,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::ChatMessageSend::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.source == cg::packet::PacketSourceTag::Xlsx, message);
    failures += Expect(packet.fields.size() == 6U, message);
    if (packet.fields.size() != 6U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "x", cg::packet::PacketFieldType::Int62, x, message);
    failures += ExpectField(packet, 1U, "y", cg::packet::PacketFieldType::Int62, y, message);
    failures += ExpectField(packet, 2U, "msg", cg::packet::PacketFieldType::EscapedString, msg, message);
    failures += ExpectField(packet, 3U, "color", cg::packet::PacketFieldType::Int62, color, message);
    failures += ExpectField(packet, 4U, "area", cg::packet::PacketFieldType::Int62, area, message);
    failures += ExpectField(packet, 5U, "fontSize", cg::packet::PacketFieldType::Int62, fontSize, message);
    return failures;
}

int TestToPacketAndEncode()
{
    const cg::packet::send::ChatMessageSend request(1, 2, "P|hello", 4, 5, 16);

    int failures = ExpectChatMessageSendPacket(request.ToPacket(), "1", "2", "P|hello", "4", "5", "16", "uSr ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "uSr BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "uSr 1 2 P|hello 4 5 g ", "uSr BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "uSr Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "uSr 1 2 P|hello 4 5 g ", "uSr Encode value");
    }
    return failures;
}

int TestEmptyMessage()
{
    const cg::packet::send::ChatMessageSend request(1, 2, "", 4, 5, 16);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "uSr Encode empty msg");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "uSr 1 2  4 5 g ", "uSr empty msg value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "uSr parse empty msg");
    if (parsed.HasValue()) {
        failures += ExpectChatMessageSendPacket(parsed.Value(), "1", "2", "", "4", "5", "16", "uSr parsed empty msg");
    }
    return failures;
}

int TestEscapedMessageRoundtrip()
{
    const cg::packet::send::ChatMessageSend request(1, 2, "P|A B\\C\nD\rE", 4, 5, 16);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "uSr Encode escaped msg");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "uSr 1 2 P|A\\SB\\\\C\\nD\\rE 4 5 g ", "uSr escaped value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "uSr parse escaped msg");
    if (parsed.HasValue()) {
        failures += ExpectChatMessageSendPacket(parsed.Value(), "1", "2", request.msg, "4", "5", "16", "uSr escaped roundtrip");
    }
    return failures;
}

int TestPipeAndMarkerNotValidated()
{
    const cg::packet::send::ChatMessageSend request(1, 2, "Q|hello|tail", 4, 5, 16);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "uSr Encode pipe msg");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "uSr 1 2 Q|hello|tail 4 5 g ", "uSr pipe unchanged");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "uSr parse pipe msg");
    if (parsed.HasValue()) {
        failures += ExpectChatMessageSendPacket(parsed.Value(), "1", "2", request.msg, "4", "5", "16", "uSr pipe roundtrip");
    }
    return failures;
}

int TestParsePacketRoundtrip()
{
    const cg::packet::send::ChatMessageSend request(10, 20, "P|hello world", 6, 7, 16);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "uSr Encode i62 values");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "uSr a k P|hello\\Sworld 6 7 g ", "uSr i62 encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "uSr parse i62 values");
    if (parsed.HasValue()) {
        failures += ExpectChatMessageSendPacket(parsed.Value(), "10", "20", request.msg, "6", "7", "16", "uSr parsed i62 values");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("uSr 1 2 P|hello 4 5"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "uSr",
        true,
        5U,
        "uSr missing fontSize");
    failures += ExpectError(
        cg::packet::ParsePacket("uSr 1 2 P|hello 4 5 g extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "uSr",
        true,
        6U,
        "uSr extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("uSr _ 2 P|hello 4 5 g "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "uSr",
        true,
        0U,
        "uSr bad x");
    failures += ExpectError(
        cg::packet::ParsePacket("uSr 1 _ P|hello 4 5 g "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "uSr",
        true,
        1U,
        "uSr bad y");
    failures += ExpectError(
        cg::packet::ParsePacket("uSr 1 2 P|hello _ 5 g "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "uSr",
        true,
        3U,
        "uSr bad color");
    failures += ExpectError(
        cg::packet::ParsePacket("uSr 1 2 P|hello 4 _ g "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "uSr",
        true,
        4U,
        "uSr bad area");
    failures += ExpectError(
        cg::packet::ParsePacket("uSr 1 2 P|hello 4 5 _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "uSr",
        true,
        5U,
        "uSr bad fontSize");

    std::string truncatedMessage = "uSr 1 2 ";
    truncatedMessage.push_back(static_cast<char>(0x80U));
    truncatedMessage.append(" 4 5 g ");
    failures += ExpectError(
        cg::packet::ParsePacket(truncatedMessage),
        cg::packet::PacketErrorCode::DecodeFailed,
        "uSr",
        true,
        2U,
        "uSr truncated msg");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = cg::packet::send::ChatMessageSend(1, 2, "P|hello", 4, 5, 16).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "uSr",
        false,
        0U,
        "uSr direction mismatch");

    cg::packet::Packet wrongName = cg::packet::send::ChatMessageSend(1, 2, "P|hello", 4, 5, 16).ToPacket();
    wrongName.fields[2].name = "message";
    failures += ExpectError(
        cg::packet::BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "uSr",
        true,
        2U,
        "uSr field name mismatch");

    cg::packet::Packet wrongType = cg::packet::send::ChatMessageSend(1, 2, "P|hello", 4, 5, 16).ToPacket();
    wrongType.fields[2].type = cg::packet::PacketFieldType::RawString;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "uSr",
        true,
        2U,
        "uSr field type mismatch");

    cg::packet::Packet badInt = cg::packet::send::ChatMessageSend(1, 2, "P|hello", 4, 5, 16).ToPacket();
    badInt.fields[0].value = "abc";
    failures += ExpectError(
        cg::packet::BuildPacket(badInt),
        cg::packet::PacketErrorCode::EncodeFailed,
        "uSr",
        true,
        0U,
        "uSr bad i62 value");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestEmptyMessage();
    failures += TestEscapedMessageRoundtrip();
    failures += TestPipeAndMarkerNotValidated();
    failures += TestParsePacketRoundtrip();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
