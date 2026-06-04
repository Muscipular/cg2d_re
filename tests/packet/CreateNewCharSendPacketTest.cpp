#include "packet/Packet.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::send::CreateNewChar::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::send::CreateNewChar::Spec);
}

using CreateNewChar = cg::packet::send::CreateNewChar;

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

int ExpectCreateNewCharPacket(
    const cg::packet::Packet& packet,
    std::string_view dataPlace,
    std::string_view name,
    std::string_view image,
    std::string_view face,
    std::string_view vital,
    std::string_view strength,
    std::string_view tough,
    std::string_view quick,
    std::string_view magic,
    std::string_view earth,
    std::string_view water,
    std::string_view fire,
    std::string_view wind,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(CreateNewChar::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 13U, message);
    if (packet.fields.size() != 13U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "dataPlace", cg::packet::PacketFieldType::Int62, dataPlace, message);
    failures += ExpectField(packet, 1U, "name", cg::packet::PacketFieldType::EscapedString, name, message);
    failures += ExpectField(packet, 2U, "image", cg::packet::PacketFieldType::Int62, image, message);
    failures += ExpectField(packet, 3U, "face", cg::packet::PacketFieldType::Int62, face, message);
    failures += ExpectField(packet, 4U, "vital", cg::packet::PacketFieldType::Int62, vital, message);
    failures += ExpectField(packet, 5U, "strength", cg::packet::PacketFieldType::Int62, strength, message);
    failures += ExpectField(packet, 6U, "tough", cg::packet::PacketFieldType::Int62, tough, message);
    failures += ExpectField(packet, 7U, "quick", cg::packet::PacketFieldType::Int62, quick, message);
    failures += ExpectField(packet, 8U, "magic", cg::packet::PacketFieldType::Int62, magic, message);
    failures += ExpectField(packet, 9U, "earth", cg::packet::PacketFieldType::Int62, earth, message);
    failures += ExpectField(packet, 10U, "water", cg::packet::PacketFieldType::Int62, water, message);
    failures += ExpectField(packet, 11U, "fire", cg::packet::PacketFieldType::Int62, fire, message);
    failures += ExpectField(packet, 12U, "wind", cg::packet::PacketFieldType::Int62, wind, message);
    return failures;
}

std::string BuildPlain(const std::vector<std::string>& tokens)
{
    std::string plain("aSEl");
    for (const std::string& token : tokens) {
        plain.push_back(' ');
        plain.append(token);
    }
    plain.push_back(' ');
    return plain;
}

std::vector<std::string> BasicEncodedTokens()
{
    return {"1", "Alice", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c"};
}

int TestToPacketAndEncode()
{
    const CreateNewChar request(1, "Alice", 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);

    int failures = ExpectCreateNewCharPacket(
        request.ToPacket(),
        "1",
        "Alice",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "8",
        "9",
        "10",
        "11",
        "12",
        "aSEl ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "aSEl BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "aSEl 1 Alice 2 3 4 5 6 7 8 9 a b c ", "aSEl BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "aSEl Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "aSEl 1 Alice 2 3 4 5 6 7 8 9 a b c ", "aSEl Encode value");
    }
    return failures;
}

int TestNameRoundtripValues()
{
    int failures = 0;
    const std::vector<std::string> names = {
        "",
        " A B ",
        "A\\B",
        "A\nB",
        "A|B",
    };

    for (const std::string& name : names) {
        const CreateNewChar request(1, name, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(static_cast<bool>(encoded), "aSEl name Encode");
        if (!encoded.HasValue()) {
            continue;
        }

        const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
        failures += Expect(static_cast<bool>(parsed), "aSEl name ParsePacket");
        if (parsed.HasValue()) {
            failures += ExpectCreateNewCharPacket(
                parsed.Value(),
                "1",
                name,
                "2",
                "3",
                "4",
                "5",
                "6",
                "7",
                "8",
                "9",
                "10",
                "11",
                "12",
                "aSEl name roundtrip");
        }
    }
    return failures;
}

int TestParsePacketRoundtrip()
{
    const CreateNewChar request(10, "Round Trip", 11, 12, -1, 0, 31, 32, 33, 34, 35, 36, 37);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "aSEl roundtrip Encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "aSEl roundtrip ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectCreateNewCharPacket(
        parsed.Value(),
        "10",
        "Round Trip",
        "11",
        "12",
        "-1",
        "0",
        "31",
        "32",
        "33",
        "34",
        "35",
        "36",
        "37",
        "aSEl roundtrip packet");

    const cg::packet::PacketResult<std::string> rebuilt = BuildPacket(parsed.Value());
    failures += Expect(static_cast<bool>(rebuilt), "aSEl roundtrip BuildPacket");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == encoded.Value(), "aSEl roundtrip rebuilt");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("aSEl 1 Alice 2 3 4 5 6 7 8 9 a b"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aSEl",
        true,
        12U,
        "aSEl missing wind");
    failures += ExpectError(
        ParsePacket("aSEl 1 Alice 2 3 4 5 6 7 8 9 a b c d "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aSEl",
        true,
        13U,
        "aSEl extra field");

    const std::array<std::size_t, 12U> intFieldIndexes = {0U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U, 11U, 12U};
    for (const std::size_t fieldIndex : intFieldIndexes) {
        std::vector<std::string> tokens = BasicEncodedTokens();
        tokens[fieldIndex] = "_";
        failures += ExpectError(
            ParsePacket(BuildPlain(tokens)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "aSEl",
            true,
            fieldIndex,
            "aSEl invalid i62");
    }

    std::string invalidName("aSEl 1 ");
    invalidName.push_back(static_cast<char>(0x80));
    invalidName.append(" 2 3 4 5 6 7 8 9 a b c ");
    failures += ExpectError(
        ParsePacket(invalidName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "aSEl",
        true,
        1U,
        "aSEl invalid es");
    return failures;
}

int TestManualPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = CreateNewChar(1, "Alice", 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "aSEl",
        false,
        0U,
        "aSEl wrong direction");

    cg::packet::Packet wrongCode = CreateNewChar(1, "Alice", 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        BuildPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "aSEl wrong code");

    cg::packet::Packet wrongName = CreateNewChar(1, "Alice", 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12).ToPacket();
    wrongName.fields[1].name = "nickname";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "aSEl",
        true,
        1U,
        "aSEl wrong field name");

    cg::packet::Packet wrongType = CreateNewChar(1, "Alice", 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12).ToPacket();
    wrongType.fields[1].type = cg::packet::PacketFieldType::Unknown;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "aSEl",
        true,
        1U,
        "aSEl wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestNameRoundtripValues();
    failures += TestParsePacketRoundtrip();
    failures += TestParseErrors();
    failures += TestManualPacketErrors();

    if (failures != 0) {
        std::cerr << failures << " packet create new char send test(s) failed\n";
        return 1;
    }

    return 0;
}
