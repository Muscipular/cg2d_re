#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::send::ProfileMailFriendDelete::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::send::ProfileMailFriendDelete::Spec);
}

using ProfileMailFriendDelete = cg::packet::send::ProfileMailFriendDelete;

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string text;
    text.reserve(values.size());
    for (const unsigned int value : values) {
        text.push_back(static_cast<char>(value));
    }
    return text;
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

cg::packet::PacketField Field(
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string value)
{
    cg::packet::PacketField field;
    field.name = std::string(name);
    field.type = type;
    field.value = std::move(value);
    return field;
}

cg::packet::Packet PacketOf(
    cg::packet::PacketDirection direction,
    std::initializer_list<cg::packet::PacketField> fields)
{
    cg::packet::Packet packet(
        cg::packet::PacketCode(ProfileMailFriendDelete::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlain(std::string_view profileIndexToken, std::string_view nameToken)
{
    std::string plain = "wD ";
    plain.append(profileIndexToken);
    plain.push_back(' ');
    plain.append(nameToken);
    plain.push_back(' ');
    return plain;
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

int ExpectProfileMailFriendDeletePacket(
    const cg::packet::Packet& packet,
    std::string_view profileIndex,
    std::string_view name,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(ProfileMailFriendDelete::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "profileIndex", cg::packet::PacketFieldType::Int62, profileIndex, message);
    failures += ExpectField(packet, 1U, "name", cg::packet::PacketFieldType::EscapedString, name, message);
    return failures;
}

int TestToPacketAndEncode()
{
    const ProfileMailFriendDelete request(12, "Alice");

    int failures = ExpectProfileMailFriendDeletePacket(request.ToPacket(), "12", "Alice", "wD ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "wD BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "wD c Alice ", "wD BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "wD Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "wD c Alice ", "wD Encode value");
    }
    return failures;
}

int TestEmptyName()
{
    const ProfileMailFriendDelete request(0, "");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "wD empty name encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "wD 0  ", "wD empty name value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "wD empty name parse");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailFriendDeletePacket(parsed.Value(), "0", "", "wD empty name packet");
    }
    return failures;
}

int TestEscapedNameRoundtrip()
{
    const std::string name = "A B\\C\nD\rE";
    const ProfileMailFriendDelete request(12, name);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "wD escaped name encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "wD c A\\SB\\\\C\\nD\\rE ", "wD escaped name value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "wD escaped name parse");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailFriendDeletePacket(parsed.Value(), "12", name, "wD escaped name packet");
    }
    return failures;
}

int TestPipeNameIsNotNested()
{
    const std::string name = "Alice|Bob|Carol";
    const ProfileMailFriendDelete request(12, name);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "wD pipe name encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "wD c Alice|Bob|Carol ", "wD pipe name value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildRawPlain("c", name));
    failures += Expect(static_cast<bool>(parsed), "wD pipe name parse");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailFriendDeletePacket(parsed.Value(), "12", name, "wD pipe name packet");
    }
    return failures;
}

int TestParsePacketRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket("wD c Alice ");

    int failures = Expect(static_cast<bool>(parsed), "wD parse packet");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailFriendDeletePacket(parsed.Value(), "12", "Alice", "wD parsed packet");
    }
    return failures;
}

int TestEmptyProfileIndexParse()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildRawPlain("", "Alice"));

    int failures = Expect(static_cast<bool>(parsed), "wD empty profileIndex parse");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailFriendDeletePacket(parsed.Value(), "0", "Alice", "wD empty profileIndex packet");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("wD c"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "wD",
        true,
        1U,
        "wD missing name");
    failures += ExpectError(
        ParsePacket("wD c Alice extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "wD",
        true,
        2U,
        "wD extra field");
    failures += ExpectError(
        ParsePacket("wD _ Alice "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wD",
        true,
        0U,
        "wD bad profileIndex");
    failures += ExpectError(
        ParsePacket(BuildRawPlain("c", Bytes({0x81U}))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wD",
        true,
        1U,
        "wD bad name es");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = ProfileMailFriendDelete(12, "Alice").ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "wD",
        false,
        0U,
        "wD wrong direction");

    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("profileIndex", cg::packet::PacketFieldType::Int62, "12"),
            })),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "wD",
        true,
        1U,
        "wD manual missing field");

    cg::packet::Packet wrongName = ProfileMailFriendDelete(12, "Alice").ToPacket();
    wrongName.fields[1].name = "data";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "wD",
        true,
        1U,
        "wD wrong field name");

    cg::packet::Packet wrongType = ProfileMailFriendDelete(12, "Alice").ToPacket();
    wrongType.fields[1].type = cg::packet::PacketFieldType::Payload;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "wD",
        true,
        1U,
        "wD wrong field type");

    cg::packet::Packet badProfileIndex = ProfileMailFriendDelete(12, "Alice").ToPacket();
    badProfileIndex.fields[0].value = "bad";
    failures += ExpectError(
        BuildPacket(badProfileIndex),
        cg::packet::PacketErrorCode::EncodeFailed,
        "wD",
        true,
        0U,
        "wD bad manual profileIndex");

    cg::packet::Packet badName = ProfileMailFriendDelete(12, "Alice").ToPacket();
    badName.fields[1].value = Bytes({0x81U});
    failures += ExpectError(
        BuildPacket(badName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "wD",
        true,
        1U,
        "wD bad manual name");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestEmptyName();
    failures += TestEscapedNameRoundtrip();
    failures += TestPipeNameIsNotNested();
    failures += TestParsePacketRoundtrip();
    failures += TestEmptyProfileIndexParse();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
