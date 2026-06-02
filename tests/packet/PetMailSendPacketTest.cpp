#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::send::PetMailSend::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::send::PetMailSend::Spec);
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

int ExpectPetMailSendPacket(
    const cg::packet::Packet& packet,
    std::string_view messageText,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::PetMailSend::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 5U, message);
    if (packet.fields.size() != 5U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "index", cg::packet::PacketFieldType::Int62, "1", message);
    failures += ExpectField(packet, 1U, "petindex", cg::packet::PacketFieldType::Int62, "2", message);
    failures += ExpectField(packet, 2U, "itemindex", cg::packet::PacketFieldType::Int62, "3", message);
    failures += ExpectField(packet, 3U, "message", cg::packet::PacketFieldType::EscapedString, messageText, message);
    failures += ExpectField(packet, 4U, "color", cg::packet::PacketFieldType::Int62, "4", message);
    return failures;
}

int TestToPacketAndEncode()
{
    const cg::packet::send::PetMailSend request(1, 2, 3, "Hello", 4);

    int failures = ExpectPetMailSendPacket(request.ToPacket(), "Hello", "rMsv ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "rMsv BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "rMsv 1 2 3 Hello 4 ", "rMsv BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "rMsv Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "rMsv 1 2 3 Hello 4 ", "rMsv Encode value");
    }
    return failures;
}

int TestEmptyMessage()
{
    const cg::packet::send::PetMailSend request(1, 2, 3, "", 4);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "rMsv Encode empty message");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "rMsv 1 2 3  4 ", "rMsv empty message value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "rMsv parse empty message");
    if (parsed.HasValue()) {
        failures += ExpectPetMailSendPacket(parsed.Value(), "", "rMsv parsed empty message");
    }
    return failures;
}

int TestEscapedMessageRoundtrip()
{
    const cg::packet::send::PetMailSend request(1, 2, 3, "A B\\C\nD\rE", 4);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "rMsv Encode escaped message");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "rMsv 1 2 3 A\\SB\\\\C\\nD\\rE 4 ", "rMsv escaped value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "rMsv parse escaped message");
    if (parsed.HasValue()) {
        failures += ExpectPetMailSendPacket(parsed.Value(), request.message, "rMsv roundtrip escaped message");
    }
    return failures;
}

int TestPipeMessageRoundtrip()
{
    const cg::packet::send::PetMailSend request(1, 2, 3, "A|B|C", 4);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "rMsv Encode pipe message");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "rMsv 1 2 3 A|B|C 4 ", "rMsv pipe unchanged");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "rMsv parse pipe message");
    if (parsed.HasValue()) {
        failures += ExpectPetMailSendPacket(parsed.Value(), request.message, "rMsv roundtrip pipe message");
    }
    return failures;
}

int TestParsePacketRoundtrip()
{
    const cg::packet::send::PetMailSend request(10, -1, 62, "Pet Mail", 6);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "rMsv Encode i62 values");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "rMsv a -1 10 Pet\\SMail 6 ", "rMsv i62 encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "rMsv parse i62 values");
    if (parsed.HasValue()) {
        failures += ExpectField(
            parsed.Value(),
            0U,
            "index",
            cg::packet::PacketFieldType::Int62,
            "10",
            "rMsv parsed index");
        failures += ExpectField(
            parsed.Value(),
            1U,
            "petindex",
            cg::packet::PacketFieldType::Int62,
            "-1",
            "rMsv parsed petindex");
        failures += ExpectField(
            parsed.Value(),
            2U,
            "itemindex",
            cg::packet::PacketFieldType::Int62,
            "62",
            "rMsv parsed itemindex");
        failures += ExpectField(
            parsed.Value(),
            3U,
            "message",
            cg::packet::PacketFieldType::EscapedString,
            "Pet Mail",
            "rMsv parsed message");
        failures += ExpectField(
            parsed.Value(),
            4U,
            "color",
            cg::packet::PacketFieldType::Int62,
            "6",
            "rMsv parsed color");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("rMsv 1 2 3 msg"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "rMsv",
        true,
        4U,
        "rMsv missing color");
    failures += ExpectError(
        ParsePacket("rMsv 1 2 3 msg 4 extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "rMsv",
        true,
        5U,
        "rMsv extra field");
    failures += ExpectError(
        ParsePacket("rMsv _ 2 3 msg 4 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "rMsv",
        true,
        0U,
        "rMsv bad index");
    failures += ExpectError(
        ParsePacket("rMsv 1 2 3 msg _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "rMsv",
        true,
        4U,
        "rMsv bad color");

    std::string truncatedMessage = "rMsv 1 2 3 ";
    truncatedMessage.push_back(static_cast<char>(0x80U));
    truncatedMessage.append(" 4 ");
    failures += ExpectError(
        ParsePacket(truncatedMessage),
        cg::packet::PacketErrorCode::DecodeFailed,
        "rMsv",
        true,
        3U,
        "rMsv truncated message");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = cg::packet::send::PetMailSend(1, 2, 3, "msg", 4).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "rMsv",
        false,
        0U,
        "rMsv direction mismatch");

    cg::packet::Packet wrongName = cg::packet::send::PetMailSend(1, 2, 3, "msg", 4).ToPacket();
    wrongName.fields[3].name = "data";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "rMsv",
        true,
        3U,
        "rMsv field name mismatch");

    cg::packet::Packet wrongType = cg::packet::send::PetMailSend(1, 2, 3, "msg", 4).ToPacket();
    wrongType.fields[3].type = cg::packet::PacketFieldType::Unknown;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "rMsv",
        true,
        3U,
        "rMsv field type mismatch");

    cg::packet::Packet badInt = cg::packet::send::PetMailSend(1, 2, 3, "msg", 4).ToPacket();
    badInt.fields[1].value = "abc";
    failures += ExpectError(
        BuildPacket(badInt),
        cg::packet::PacketErrorCode::EncodeFailed,
        "rMsv",
        true,
        1U,
        "rMsv bad i62 value");

    cg::packet::Packet badMessage = cg::packet::send::PetMailSend(1, 2, 3, "msg", 4).ToPacket();
    badMessage.fields[3].value.assign(1U, static_cast<char>(0x80U));
    failures += ExpectError(
        BuildPacket(badMessage),
        cg::packet::PacketErrorCode::EncodeFailed,
        "rMsv",
        true,
        3U,
        "rMsv bad es value");
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
