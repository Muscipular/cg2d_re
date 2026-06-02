#include "packet/packet.hpp"

#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::receive::CharacterDeleteResult::Spec;
}

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, PacketSpec());
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, PacketSpec());
}

using CharacterDeleteResult = cg::packet::receive::CharacterDeleteResult;

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
        cg::packet::PacketCode(CharacterDeleteResult::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildPlain(std::string_view result, std::string_view data)
{
    const cg::packet::PacketResult<std::string> encodedResult = cg::packet::EncodeEscapedString(result);
    const cg::packet::PacketResult<std::string> encodedData = cg::packet::EncodeEscapedString(data);
    if (!encodedResult.HasValue() || !encodedData.HasValue()) {
        return "oy ";
    }

    std::string plain("oy ");
    plain.append(encodedResult.Value());
    plain.push_back(' ');
    plain.append(encodedData.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view result,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(CharacterDeleteResult::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "result", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == result, message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].value == data, message);
    return failures;
}

int ExpectValue(
    const CharacterDeleteResult& value,
    std::string_view result,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.result == result, message);
    failures += Expect(value.data == data, message);
    return failures;
}

int TestSuccessfulEmptyData()
{
    const cg::packet::PacketResult<CharacterDeleteResult> parsed =
        CharacterDeleteResult::Parse("oy successful  ");

    int failures = Expect(static_cast<bool>(parsed), "oy successful parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectValue(parsed.Value(), "successful", "", "oy successful value");
    failures += ExpectPacket(parsed.Value().ToPacket(), "successful", "", "oy successful packet");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "oy successful build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "oy successful  ", "oy successful built text");
    }
    return failures;
}

int TestFailedMessage()
{
    const char* plain = "oy failed Already\\SLogged\\Sin\\n ";
    const std::string data = "Already Logged in\n";
    const cg::packet::PacketResult<CharacterDeleteResult> parsed = CharacterDeleteResult::Parse(plain);

    int failures = Expect(static_cast<bool>(parsed), "oy failed parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectValue(parsed.Value(), "failed", data, "oy failed value");
    failures += ExpectPacket(parsed.Value().ToPacket(), "failed", data, "oy failed packet");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "oy failed build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == plain, "oy failed built text");
    }
    return failures;
}

int TestDataRoundtrip()
{
    int failures = 0;
    const std::vector<std::string> dataValues = {
        "",
        " ",
        "\\",
        "\n",
        "A|B",
        "A\nB",
    };

    for (const std::string& data : dataValues) {
        const CharacterDeleteResult value("failed", data);
        const cg::packet::PacketResult<std::string> built = BuildPacket(value.ToPacket());
        failures += Expect(static_cast<bool>(built), "oy data roundtrip build");
        if (!built.HasValue()) {
            continue;
        }

        const cg::packet::PacketResult<CharacterDeleteResult> parsed =
            CharacterDeleteResult::Parse(built.Value());
        failures += Expect(static_cast<bool>(parsed), "oy data roundtrip parse");
        if (parsed.HasValue()) {
            failures += ExpectValue(parsed.Value(), "failed", data, "oy data roundtrip value");
        }
    }

    const cg::packet::PacketResult<CharacterDeleteResult> custom =
        CharacterDeleteResult::Parse(BuildPlain("custom result", "db result"));
    failures += Expect(static_cast<bool>(custom), "oy custom result parse");
    if (custom.HasValue()) {
        failures += ExpectValue(custom.Value(), "custom result", "db result", "oy custom result value");
    }
    return failures;
}

int TestDataNotNestedPayload()
{
    const cg::packet::PacketResult<CharacterDeleteResult> parsed =
        CharacterDeleteResult::Parse("oy failed A\\nB ");

    int failures = Expect(static_cast<bool>(parsed), "oy normal es newline parse");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().data == "A\nB", "oy data is normal es newline");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        CharacterDeleteResult::Parse("DZ successful 1  "),
        cg::packet::PacketErrorCode::InvalidCode,
        "DZ",
        "oy wrong code");
    failures += ExpectError(
        CharacterDeleteResult::Parse("oy successful"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "oy",
        "oy missing data");
    failures += ExpectError(
        CharacterDeleteResult::Parse("oy successful ok extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "oy",
        "oy extra field");

    std::string invalidEs("oy successful ");
    invalidEs.push_back(static_cast<char>(0x80));
    invalidEs.push_back(' ');
    failures += ExpectError(
        CharacterDeleteResult::Parse(invalidEs),
        cg::packet::PacketErrorCode::DecodeFailed,
        "oy",
        "oy invalid es");
    return failures;
}

int TestManualPacketErrors()
{
    int failures = 0;
    failures += ExpectError(
        CharacterDeleteResult::FromPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {Field("result", cg::packet::PacketFieldType::EscapedString, "successful"),
             Field("data", cg::packet::PacketFieldType::EscapedString, "")})),
        cg::packet::PacketErrorCode::DecodeFailed,
        "oy",
        "oy wrong direction");
    failures += ExpectError(
        CharacterDeleteResult::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("status", cg::packet::PacketFieldType::EscapedString, "successful"),
             Field("data", cg::packet::PacketFieldType::EscapedString, "")})),
        cg::packet::PacketErrorCode::DecodeFailed,
        "oy",
        "oy wrong result field name");
    failures += ExpectError(
        CharacterDeleteResult::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("result", cg::packet::PacketFieldType::EscapedString, "successful"),
             Field("message", cg::packet::PacketFieldType::EscapedString, "")})),
        cg::packet::PacketErrorCode::DecodeFailed,
        "oy",
        "oy wrong data field name");
    failures += ExpectError(
        CharacterDeleteResult::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("result", cg::packet::PacketFieldType::Int62, "1"),
             Field("data", cg::packet::PacketFieldType::EscapedString, "")})),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "oy",
        "oy wrong result field type");
    failures += ExpectError(
        CharacterDeleteResult::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("result", cg::packet::PacketFieldType::EscapedString, "successful"),
             Field("data", cg::packet::PacketFieldType::RawString, "")})),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "oy",
        "oy wrong data field type");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    cg::packet::Packet wrongName = CharacterDeleteResult("successful", "").ToPacket();
    wrongName.fields[0].name = "status";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "oy",
        "oy build wrong field name");

    cg::packet::Packet wrongType = CharacterDeleteResult("successful", "").ToPacket();
    wrongType.fields[1].type = cg::packet::PacketFieldType::RawString;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "oy",
        "oy build wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSuccessfulEmptyData();
    failures += TestFailedMessage();
    failures += TestDataRoundtrip();
    failures += TestDataNotNestedPayload();
    failures += TestParseErrors();
    failures += TestManualPacketErrors();
    failures += TestBuildErrors();

    if (failures != 0) {
        std::cerr << failures << " packet character delete result test(s) failed\n";
        return 1;
    }

    return 0;
}
