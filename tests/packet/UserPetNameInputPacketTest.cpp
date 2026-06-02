#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::send::UserPetNameInput::Spec;
}

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, PacketSpec());
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, PacketSpec());
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

int ExpectUserPetNameInputPacket(const cg::packet::Packet& packet, const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::UserPetNameInput::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "petSlot", cg::packet::PacketFieldType::Int62, "10", message);
    failures += ExpectField(packet, 1U, "name", cg::packet::PacketFieldType::EscapedString, "Pochi", message);
    return failures;
}

int TestToPacketAndEncode()
{
    const cg::packet::send::UserPetNameInput request(10, "Pochi");

    int failures = ExpectUserPetNameInputPacket(request.ToPacket(), "ycX ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "ycX BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "ycX a Pochi ", "ycX BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "ycX Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "ycX a Pochi ", "ycX Encode value");
    }
    return failures;
}

int TestEmptyName()
{
    const cg::packet::send::UserPetNameInput request(0, "");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "ycX Encode empty name");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "ycX 0  ", "ycX empty name value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "ycX parse empty name");
    if (parsed.HasValue()) {
        failures += ExpectField(
            parsed.Value(),
            1U,
            "name",
            cg::packet::PacketFieldType::EscapedString,
            "",
            "ycX parsed empty name");
    }
    return failures;
}

int TestEscapedNameRoundtrip()
{
    const cg::packet::send::UserPetNameInput request(10, "A B\\C\nD\rE");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "ycX Encode escaped name");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "ycX a A\\SB\\\\C\\nD\\rE ", "ycX escaped value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "ycX parse escaped name");
    if (parsed.HasValue()) {
        failures += ExpectField(
            parsed.Value(),
            1U,
            "name",
            cg::packet::PacketFieldType::EscapedString,
            request.name,
            "ycX roundtrip escaped name");
    }
    return failures;
}

int TestPipeName()
{
    const cg::packet::send::UserPetNameInput request(1, "A|B");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "ycX Encode pipe name");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "ycX 1 A|B ", "ycX pipe unchanged");
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("ycX a Pet\\SName ");

    int failures = Expect(static_cast<bool>(parsed), "parse ycX");
    if (parsed.HasValue()) {
        failures += ExpectField(
            parsed.Value(),
            0U,
            "petSlot",
            cg::packet::PacketFieldType::Int62,
            "10",
            "ycX parsed petSlot");
        failures += ExpectField(
            parsed.Value(),
            1U,
            "name",
            cg::packet::PacketFieldType::EscapedString,
            "Pet Name",
            "ycX parsed name");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("ycX a"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ycX",
        true,
        1U,
        "ycX missing name");
    failures += ExpectError(
        ParsePacket("ycX a Pet extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ycX",
        true,
        2U,
        "ycX extra field");
    failures += ExpectError(
        ParsePacket("ycX _ Pet "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ycX",
        true,
        0U,
        "ycX bad petSlot");

    std::string truncatedName = "ycX a ";
    truncatedName.push_back(static_cast<char>(0x80U));
    truncatedName.push_back(' ');
    failures += ExpectError(
        ParsePacket(truncatedName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ycX",
        true,
        1U,
        "ycX truncated name");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = cg::packet::send::UserPetNameInput(10, "Pochi").ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ycX",
        false,
        0U,
        "ycX direction mismatch");

    cg::packet::Packet wrongName = cg::packet::send::UserPetNameInput(10, "Pochi").ToPacket();
    wrongName.fields[1].name = "wrong";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ycX",
        true,
        1U,
        "ycX field name mismatch");

    cg::packet::Packet wrongType = cg::packet::send::UserPetNameInput(10, "Pochi").ToPacket();
    wrongType.fields[1].type = cg::packet::PacketFieldType::Int62;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ycX",
        true,
        1U,
        "ycX field type mismatch");

    cg::packet::Packet emptyValue = cg::packet::send::UserPetNameInput(10, "Pochi").ToPacket();
    emptyValue.fields[0].value.clear();
    failures += ExpectError(
        BuildPacket(emptyValue),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ycX",
        true,
        0U,
        "ycX empty i62 value");

    cg::packet::Packet badValue = cg::packet::send::UserPetNameInput(10, "Pochi").ToPacket();
    badValue.fields[0].value = "abc";
    failures += ExpectError(
        BuildPacket(badValue),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ycX",
        true,
        0U,
        "ycX bad i62 value");

    cg::packet::Packet truncatedValue = cg::packet::send::UserPetNameInput(10, "Pochi").ToPacket();
    truncatedValue.fields[1].value.assign(1U, static_cast<char>(0x80U));
    failures += ExpectError(
        BuildPacket(truncatedValue),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ycX",
        true,
        1U,
        "ycX truncated es value");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestEmptyName();
    failures += TestEscapedNameRoundtrip();
    failures += TestPipeName();
    failures += TestParsePacket();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
