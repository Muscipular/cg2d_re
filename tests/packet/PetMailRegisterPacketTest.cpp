#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::send::PetMailRegister::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::send::PetMailRegister::Spec);
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

int ExpectPetMailRegisterPacket(
    const cg::packet::Packet& packet,
    std::string_view text,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::PetMailRegister::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 5U, message);
    if (packet.fields.size() != 5U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "toregist", cg::packet::PacketFieldType::Int62, "1", message);
    failures += ExpectField(packet, 1U, "petSlot", cg::packet::PacketFieldType::Int62, "2", message);
    failures += ExpectField(packet, 2U, "itemSlot", cg::packet::PacketFieldType::Int62, "3", message);
    failures += ExpectField(packet, 3U, "text", cg::packet::PacketFieldType::EscapedString, text, message);
    failures += ExpectField(packet, 4U, "color", cg::packet::PacketFieldType::Int62, "4", message);
    return failures;
}

int TestToPacketAndEncode()
{
    const cg::packet::send::PetMailRegister request(1, 2, 3, "Hello", 4);

    int failures = ExpectPetMailRegisterPacket(request.ToPacket(), "Hello", "PfqV ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "PfqV BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "PfqV 1 2 3 Hello 4 ", "PfqV BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "PfqV Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "PfqV 1 2 3 Hello 4 ", "PfqV Encode value");
    }
    return failures;
}

int TestEmptyText()
{
    const cg::packet::send::PetMailRegister request(1, 2, 3, "", 4);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "PfqV Encode empty text");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "PfqV 1 2 3  4 ", "PfqV empty text value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "PfqV parse empty text");
    if (parsed.HasValue()) {
        failures += ExpectPetMailRegisterPacket(parsed.Value(), "", "PfqV parsed empty text");
    }
    return failures;
}

int TestEscapedTextRoundtrip()
{
    const cg::packet::send::PetMailRegister request(1, 2, 3, "A B\\C\nD\rE", 4);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "PfqV Encode escaped text");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "PfqV 1 2 3 A\\SB\\\\C\\nD\\rE 4 ", "PfqV escaped value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "PfqV parse escaped text");
    if (parsed.HasValue()) {
        failures += ExpectPetMailRegisterPacket(parsed.Value(), request.text, "PfqV roundtrip escaped text");
    }
    return failures;
}

int TestPipeTextRoundtrip()
{
    // 这里只验证普通外层 es 可保留 raw pipe；legacy UI caller 会先将 pipe 预转义为 \z，
    // packet 层不负责该内容转换，color | 0x100 由 Server handler 处理。
    const cg::packet::send::PetMailRegister request(1, 2, 3, "A|B|C", 4);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "PfqV Encode pipe text");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "PfqV 1 2 3 A|B|C 4 ", "PfqV pipe unchanged");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "PfqV parse pipe text");
    if (parsed.HasValue()) {
        failures += ExpectPetMailRegisterPacket(parsed.Value(), request.text, "PfqV roundtrip pipe text");
    }
    return failures;
}

int TestColorNoServerMask()
{
    const cg::packet::send::PetMailRegister request(1, 2, 3, "Color", 1);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "PfqV Encode color one");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "PfqV 1 2 3 Color 1 ", "PfqV color remains one");
    }
    return failures;
}

int TestParsePacketRoundtrip()
{
    const cg::packet::send::PetMailRegister request(10, -1, 62, "Pet Register", 6);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "PfqV Encode i62 values");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "PfqV a -1 10 Pet\\SRegister 6 ", "PfqV i62 encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "PfqV parse i62 values");
    if (parsed.HasValue()) {
        failures += ExpectField(
            parsed.Value(),
            0U,
            "toregist",
            cg::packet::PacketFieldType::Int62,
            "10",
            "PfqV parsed toregist");
        failures += ExpectField(
            parsed.Value(),
            1U,
            "petSlot",
            cg::packet::PacketFieldType::Int62,
            "-1",
            "PfqV parsed petSlot");
        failures += ExpectField(
            parsed.Value(),
            2U,
            "itemSlot",
            cg::packet::PacketFieldType::Int62,
            "62",
            "PfqV parsed itemSlot");
        failures += ExpectField(
            parsed.Value(),
            3U,
            "text",
            cg::packet::PacketFieldType::EscapedString,
            "Pet Register",
            "PfqV parsed text");
        failures += ExpectField(
            parsed.Value(),
            4U,
            "color",
            cg::packet::PacketFieldType::Int62,
            "6",
            "PfqV parsed color");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("PfqV 1 2 3 text"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PfqV",
        true,
        4U,
        "PfqV missing color");
    failures += ExpectError(
        ParsePacket("PfqV 1 2 3 text 4 extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PfqV",
        true,
        5U,
        "PfqV extra field");
    failures += ExpectError(
        ParsePacket("PfqV _ 2 3 text 4 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "PfqV",
        true,
        0U,
        "PfqV bad toregist");
    failures += ExpectError(
        ParsePacket("PfqV 1 2 3 text _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "PfqV",
        true,
        4U,
        "PfqV bad color");

    std::string truncatedText = "PfqV 1 2 3 ";
    truncatedText.push_back(static_cast<char>(0x80U));
    truncatedText.append(" 4 ");
    failures += ExpectError(
        ParsePacket(truncatedText),
        cg::packet::PacketErrorCode::DecodeFailed,
        "PfqV",
        true,
        3U,
        "PfqV truncated text");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = cg::packet::send::PetMailRegister(1, 2, 3, "text", 4).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PfqV",
        false,
        0U,
        "PfqV direction mismatch");

    cg::packet::Packet wrongName = cg::packet::send::PetMailRegister(1, 2, 3, "text", 4).ToPacket();
    wrongName.fields[3].name = "message";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PfqV",
        true,
        3U,
        "PfqV field name mismatch");

    cg::packet::Packet wrongType = cg::packet::send::PetMailRegister(1, 2, 3, "text", 4).ToPacket();
    wrongType.fields[3].type = cg::packet::PacketFieldType::Unknown;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "PfqV",
        true,
        3U,
        "PfqV field type mismatch");

    cg::packet::Packet badInt = cg::packet::send::PetMailRegister(1, 2, 3, "text", 4).ToPacket();
    badInt.fields[1].value = "abc";
    failures += ExpectError(
        BuildPacket(badInt),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PfqV",
        true,
        1U,
        "PfqV bad i62 value");

    cg::packet::Packet badText = cg::packet::send::PetMailRegister(1, 2, 3, "text", 4).ToPacket();
    badText.fields[3].value.assign(1U, static_cast<char>(0x80U));
    failures += ExpectError(
        BuildPacket(badText),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PfqV",
        true,
        3U,
        "PfqV bad es value");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestEmptyText();
    failures += TestEscapedTextRoundtrip();
    failures += TestPipeTextRoundtrip();
    failures += TestColorNoServerMask();
    failures += TestParsePacketRoundtrip();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
