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
    return cg::packet::ParsePacket(plainTokens, cg::packet::send::ProfileMiniMailSend::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::send::ProfileMiniMailSend::Spec);
}

using ProfileMiniMailSend = cg::packet::send::ProfileMiniMailSend;

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
        cg::packet::PacketCode(ProfileMiniMailSend::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string EncodeColor(int color)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeInt62(color);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string BuildPlainGh(std::string_view payload, std::string_view colorToken)
{
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        return "GH ";
    }

    std::string plain = "GH ";
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    plain.append(colorToken);
    plain.push_back(' ');
    return plain;
}

std::string BuildPlainGh(std::string_view payload, int color)
{
    return BuildPlainGh(payload, EncodeColor(color));
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

int ExpectProfileMiniMailSendPacket(
    const cg::packet::Packet& packet,
    std::string_view data,
    std::string_view color,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(ProfileMiniMailSend::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "data", cg::packet::PacketFieldType::EscapedString, data, message);
    failures += ExpectField(packet, 1U, "color", cg::packet::PacketFieldType::Int62, color, message);
    return failures;
}

int ExpectProfileMiniMailSendValue(
    const ProfileMiniMailSend& packet,
    int profileIndex,
    std::string_view name,
    std::string_view messageText,
    int color,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.profileIndex == profileIndex, message);
    failures += Expect(packet.name == name, message);
    failures += Expect(packet.message == messageText, message);
    failures += Expect(packet.color == color, message);
    return failures;
}

int TestToPacketAndEncode()
{
    const ProfileMiniMailSend request(12, "Alice", "hello", 12);
    int failures = 0;

    const cg::packet::PacketResult<std::string> data = request.BuildData();
    failures += Expect(static_cast<bool>(data), "GH BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == "12|Alice|hello|", "GH BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = request.ToPacket();
    failures += Expect(static_cast<bool>(packet), "GH ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += ExpectProfileMiniMailSendPacket(packet.Value(), "12|Alice|hello|", "12", "GH ToPacket value");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "GH BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "GH 12|Alice|hello| c ", "GH BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "GH Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "GH 12|Alice|hello| c ", "GH Encode value");
    }
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainGh("12|Alice|hello|", 3));

    int failures = Expect(static_cast<bool>(parsed), "GH parse outer");
    if (parsed.HasValue()) {
        failures += ExpectProfileMiniMailSendPacket(parsed.Value(), "12|Alice|hello|", "3", "GH parse outer packet");
    }
    return failures;
}

int TestTypedParse()
{
    const cg::packet::PacketResult<ProfileMiniMailSend> parsed =
        ProfileMiniMailSend::Parse(BuildPlainGh("12|Alice|hello|", 3));

    int failures = Expect(static_cast<bool>(parsed), "GH typed parse");
    if (parsed.HasValue()) {
        failures += ExpectProfileMiniMailSendValue(parsed.Value(), 12, "Alice", "hello", 3, "GH typed parse value");
    }
    return failures;
}

int TestMessageWithSpace()
{
    const ProfileMiniMailSend request(12, "Alice", "hello world", 3);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "GH space message encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "GH 12|Alice|hello\\Sworld| 3 ", "GH space message encoded value");

    const cg::packet::PacketResult<ProfileMiniMailSend> parsed = ProfileMiniMailSend::Parse(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "GH space message parse");
    if (parsed.HasValue()) {
        failures += ExpectProfileMiniMailSendValue(
            parsed.Value(),
            12,
            "Alice",
            "hello world",
            3,
            "GH space message value");
    }
    return failures;
}

int TestRawEscapesPreserved()
{
    const std::string message = "line\\nred\\cblue\\y";
    const ProfileMiniMailSend request(12, "Alice", message, 3);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "GH raw escape encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(
        encoded.Value() == "GH 12|Alice|line\\\\nred\\\\cblue\\\\y| 3 ",
        "GH raw escape encoded value");

    const cg::packet::PacketResult<ProfileMiniMailSend> parsed = ProfileMiniMailSend::Parse(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "GH raw escape parse");
    if (parsed.HasValue()) {
        failures += ExpectProfileMiniMailSendValue(parsed.Value(), 12, "Alice", message, 3, "GH raw escape value");
    }
    return failures;
}

int TestRawSignedDecimalProfileIndex()
{
    const ProfileMiniMailSend request(-12, "Alice", "hello", 3);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "GH signed profileIndex encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "GH -12|Alice|hello| 3 ", "GH signed profileIndex encoded value");

    const cg::packet::PacketResult<ProfileMiniMailSend> parsed = ProfileMiniMailSend::Parse(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "GH signed profileIndex parse");
    if (parsed.HasValue()) {
        failures += ExpectProfileMiniMailSendValue(parsed.Value(), -12, "Alice", "hello", 3, "GH signed value");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ProfileMiniMailSend::Parse("GH 12|Alice|hello| _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GH",
        "GH bad color");
    failures += ExpectError(
        ProfileMiniMailSend::Parse("GH 12|Alice|hello|"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "GH",
        "GH missing color");
    failures += ExpectError(
        ProfileMiniMailSend::Parse("GH 12|Alice|hello| 3 extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "GH",
        "GH extra outer field");
    failures += ExpectError(
        ProfileMiniMailSend::Parse(BuildPlainGh("", 3)),
        cg::packet::PacketErrorCode::EmptyInput,
        "GH",
        "GH empty data");
    failures += ExpectError(
        ProfileMiniMailSend::Parse(BuildPlainGh("12|Alice|hello", 3)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GH",
        "GH missing payload tail");
    failures += ExpectError(
        ProfileMiniMailSend::Parse(BuildPlainGh("12|Alice|hello||", 3)),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "GH",
        "GH extra trailing payload");
    failures += ExpectError(
        ProfileMiniMailSend::Parse(BuildPlainGh("|Alice|hello|", 3)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GH",
        "GH empty profileIndex");
    failures += ExpectError(
        ProfileMiniMailSend::Parse(BuildPlainGh("12||hello|", 3)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GH",
        "GH empty name");
    failures += ExpectError(
        ProfileMiniMailSend::Parse(BuildPlainGh("12|Alice||", 3)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GH",
        "GH empty message");
    failures += ExpectError(
        ProfileMiniMailSend::Parse(BuildPlainGh("c|Alice|hello|", 3)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GH",
        "GH bad profileIndex raw decimal");
    failures += ExpectError(
        ProfileMiniMailSend::Parse(BuildPlainGh("2147483648|Alice|hello|", 3)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GH",
        "GH profileIndex overflow");
    failures += ExpectError(
        ProfileMiniMailSend::Parse(BuildPlainGh("12|Alice|hello\\zbad|", 3)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GH",
        "GH message literal z delimiter");
    failures += ExpectError(
        ProfileMiniMailSend::Parse(BuildPlainGh("12|Alice|Bob|hello|", 3)),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "GH",
        "GH name literal pipe");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    failures += ExpectError(
        ProfileMiniMailSend(12, "", "hello", 3).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "GH",
        "GH build empty name");
    failures += ExpectError(
        ProfileMiniMailSend(12, "Alice", "", 3).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "GH",
        "GH build empty message");
    failures += ExpectError(
        ProfileMiniMailSend(12, "Alice|Bob", "hello", 3).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "GH",
        "GH build name pipe");
    failures += ExpectError(
        ProfileMiniMailSend(12, "Alice", "hello\\zbad", 3).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "GH",
        "GH build message z delimiter");
    failures += ExpectError(
        ProfileMiniMailSend(12, "Alice", Bytes({0x81U}), 3).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "GH",
        "GH build truncated legacy byte");

    cg::packet::Packet wrongDirection = PacketOf(
        cg::packet::PacketDirection::ServerToClient,
        {
            Field("data", cg::packet::PacketFieldType::EscapedString, "12|Alice|hello|"),
            Field("color", cg::packet::PacketFieldType::Int62, "3"),
        });
    failures += ExpectError(
        ProfileMiniMailSend::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GH",
        "GH wrong direction");

    failures += ExpectError(
        ProfileMiniMailSend::FromPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("data", cg::packet::PacketFieldType::EscapedString, "12|Alice|hello|"),
            })),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "GH",
        "GH manual missing field");

    failures += ExpectError(
        ProfileMiniMailSend::FromPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("payload", cg::packet::PacketFieldType::EscapedString, "12|Alice|hello|"),
                Field("color", cg::packet::PacketFieldType::Int62, "3"),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GH",
        "GH wrong data name");

    failures += ExpectError(
        ProfileMiniMailSend::FromPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("data", cg::packet::PacketFieldType::Unknown, "12|Alice|hello|"),
                Field("color", cg::packet::PacketFieldType::Int62, "3"),
            })),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "GH",
        "GH wrong data type");

    failures += ExpectError(
        ProfileMiniMailSend::FromPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("data", cg::packet::PacketFieldType::EscapedString, "12|Alice|hello|"),
                Field("color", cg::packet::PacketFieldType::Int62, "bad"),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GH",
        "GH manual bad color");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestParsePacketOuter();
    failures += TestTypedParse();
    failures += TestMessageWithSpace();
    failures += TestRawEscapesPreserved();
    failures += TestRawSignedDecimalProfileIndex();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
