#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::send::MailMessageSend::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::send::MailMessageSend::Spec);
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
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectMailMessageSendPacket(
    const cg::packet::Packet& packet,
    std::string_view messageText,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::MailMessageSend::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "index", cg::packet::PacketFieldType::Int62, "1", message);
    failures += ExpectField(packet, 1U, "message", cg::packet::PacketFieldType::EscapedString, messageText, message);
    failures += ExpectField(packet, 2U, "color", cg::packet::PacketFieldType::Int62, "4", message);
    return failures;
}

int TestToPacketAndEncode()
{
    const cg::packet::send::MailMessageSend request(1, "Hello", 4);

    int failures = ExpectMailMessageSendPacket(request.ToPacket(), "Hello", "CV ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "CV BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "CV 1 Hello 4 ", "CV BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "CV Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "CV 1 Hello 4 ", "CV Encode value");
    }
    return failures;
}

int TestEmptyMessage()
{
    const cg::packet::send::MailMessageSend request(1, "", 4);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "CV Encode empty message");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "CV 1  4 ", "CV empty message value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "CV parse empty message");
    if (parsed.HasValue()) {
        failures += ExpectMailMessageSendPacket(parsed.Value(), "", "CV parsed empty message");
    }
    return failures;
}

int TestEscapedMessageRoundtrip()
{
    const cg::packet::send::MailMessageSend request(1, "A B\\C\nD\rE", 4);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "CV Encode escaped message");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "CV 1 A\\SB\\\\C\\nD\\rE 4 ", "CV escaped value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "CV parse escaped message");
    if (parsed.HasValue()) {
        failures += ExpectMailMessageSendPacket(parsed.Value(), request.message, "CV roundtrip escaped message");
    }
    return failures;
}

int TestPipeMessageRoundtrip()
{
    const cg::packet::send::MailMessageSend request(1, "A|B|C", 4);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "CV Encode pipe message");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "CV 1 A|B|C 4 ", "CV pipe unchanged");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "CV parse pipe message");
    if (parsed.HasValue()) {
        failures += ExpectMailMessageSendPacket(parsed.Value(), request.message, "CV roundtrip pipe message");
    }
    return failures;
}

int TestParsePacketRoundtrip()
{
    const cg::packet::send::MailMessageSend request(10, "Mail Message", 6);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "CV Encode i62 values");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "CV a Mail\\SMessage 6 ", "CV i62 encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "CV parse i62 values");
    if (parsed.HasValue()) {
        failures += ExpectField(
            parsed.Value(),
            0U,
            "index",
            cg::packet::PacketFieldType::Int62,
            "10",
            "CV parsed index");
        failures += ExpectField(
            parsed.Value(),
            1U,
            "message",
            cg::packet::PacketFieldType::EscapedString,
            "Mail Message",
            "CV parsed message");
        failures += ExpectField(
            parsed.Value(),
            2U,
            "color",
            cg::packet::PacketFieldType::Int62,
            "6",
            "CV parsed color");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("CV 1 msg"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "CV",
        true,
        2U,
        "CV missing color");
    failures += ExpectError(
        ParsePacket("CV 1 msg 4 extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "CV",
        true,
        3U,
        "CV extra field");
    failures += ExpectError(
        ParsePacket("CV _ msg 4 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "CV",
        true,
        0U,
        "CV bad index");
    failures += ExpectError(
        ParsePacket("CV 1 msg _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "CV",
        true,
        2U,
        "CV bad color");

    std::string truncatedMessage = "CV 1 ";
    truncatedMessage.push_back(static_cast<char>(0x80U));
    truncatedMessage.append(" 4 ");
    failures += ExpectError(
        ParsePacket(truncatedMessage),
        cg::packet::PacketErrorCode::DecodeFailed,
        "CV",
        true,
        1U,
        "CV truncated message");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = cg::packet::send::MailMessageSend(1, "msg", 4).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "CV",
        false,
        0U,
        "CV direction mismatch");

    cg::packet::Packet wrongName = cg::packet::send::MailMessageSend(1, "msg", 4).ToPacket();
    wrongName.fields[1].name = "data";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "CV",
        true,
        1U,
        "CV field name mismatch");

    cg::packet::Packet wrongType = cg::packet::send::MailMessageSend(1, "msg", 4).ToPacket();
    wrongType.fields[1].type = cg::packet::PacketFieldType::Payload;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "CV",
        true,
        1U,
        "CV field type mismatch");

    cg::packet::Packet badInt = cg::packet::send::MailMessageSend(1, "msg", 4).ToPacket();
    badInt.fields[0].value = "abc";
    failures += ExpectError(
        BuildPacket(badInt),
        cg::packet::PacketErrorCode::EncodeFailed,
        "CV",
        true,
        0U,
        "CV bad i62 value");

    cg::packet::Packet badMessage = cg::packet::send::MailMessageSend(1, "msg", 4).ToPacket();
    badMessage.fields[1].value.assign(1U, static_cast<char>(0x80U));
    failures += ExpectError(
        BuildPacket(badMessage),
        cg::packet::PacketErrorCode::EncodeFailed,
        "CV",
        true,
        1U,
        "CV bad es value");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestEmptyMessage();
    failures += TestEscapedMessageRoundtrip();
    failures += TestPipeMessageRoundtrip();
    failures += TestParsePacketRoundtrip();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
