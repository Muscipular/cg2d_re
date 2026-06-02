#include "packet/packet.hpp"

#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

using CreateNewCharResult = cg::packet::receive::CreateNewCharResult;

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
        cg::packet::PacketCode(CreateNewCharResult::Code),
        direction,
        cg::packet::PacketSourceTag::ServerC);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildPlain(std::string_view result, int registNumber, std::string_view data)
{
    const cg::packet::PacketResult<std::string> encodedResult = cg::packet::EncodeEscapedString(result);
    const cg::packet::PacketResult<std::string> encodedRegistNumber = cg::packet::EncodeInt62(registNumber);
    const cg::packet::PacketResult<std::string> encodedData = cg::packet::EncodeEscapedString(data);
    if (!encodedResult.HasValue() || !encodedRegistNumber.HasValue() || !encodedData.HasValue()) {
        return "DZ ";
    }

    std::string plain("DZ ");
    plain.append(encodedResult.Value());
    plain.push_back(' ');
    plain.append(encodedRegistNumber.Value());
    plain.push_back(' ');
    plain.append(encodedData.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view result,
    int registNumber,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(CreateNewCharResult::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "result", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == result, message);
    failures += Expect(packet.fields[1].name == "registnumber", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[1].value == std::to_string(registNumber), message);
    failures += Expect(packet.fields[2].name == "data", message);
    failures += Expect(packet.fields[2].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[2].value == data, message);
    return failures;
}

int ExpectValue(
    const CreateNewCharResult& value,
    std::string_view result,
    int registNumber,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.result == result, message);
    failures += Expect(value.registNumber == registNumber, message);
    failures += Expect(value.data == data, message);
    return failures;
}

int TestSuccessfulEmptyData()
{
    const cg::packet::PacketResult<CreateNewCharResult> parsed =
        CreateNewCharResult::Parse("DZ successful 1  ");

    int failures = Expect(static_cast<bool>(parsed), "DZ successful parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectValue(parsed.Value(), "successful", 1, "", "DZ successful value");
    failures += ExpectPacket(parsed.Value().ToPacket(), "successful", 1, "", "DZ successful packet");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "DZ successful build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "DZ successful 1  ", "DZ successful built text");
    }
    return failures;
}

int TestFailedMessage()
{
    const char* plain = "DZ failed -1 Too\\Slong\\Scharname\\n ";
    const std::string data = "Too long charname\n";
    const cg::packet::PacketResult<CreateNewCharResult> parsed = CreateNewCharResult::Parse(plain);

    int failures = Expect(static_cast<bool>(parsed), "DZ failed parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectValue(parsed.Value(), "failed", -1, data, "DZ failed value");
    failures += ExpectPacket(parsed.Value().ToPacket(), "failed", -1, data, "DZ failed packet");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "DZ failed build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == plain, "DZ failed built text");
    }
    return failures;
}

int TestOpaqueDataRoundtrip()
{
    const std::string data = "name raw|db\\payload\\zinner value\nline2";
    const std::string plain = BuildPlain("custom result", 42, data);
    const cg::packet::PacketResult<CreateNewCharResult> parsed = CreateNewCharResult::Parse(plain);

    int failures = Expect(static_cast<bool>(parsed), "DZ opaque parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectValue(parsed.Value(), "custom result", 42, data, "DZ opaque value");
    failures += Expect(parsed.Value().data == data, "DZ opaque data not split");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "DZ opaque build");
    if (built.HasValue()) {
        const cg::packet::PacketResult<CreateNewCharResult> roundtrip =
            CreateNewCharResult::Parse(built.Value());
        failures += Expect(static_cast<bool>(roundtrip), "DZ opaque roundtrip parse");
        if (roundtrip.HasValue()) {
            failures += ExpectValue(roundtrip.Value(), "custom result", 42, data, "DZ opaque roundtrip value");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        CreateNewCharResult::Parse("whV 0 ok "),
        cg::packet::PacketErrorCode::InvalidCode,
        "whV",
        "DZ wrong code");
    failures += ExpectError(
        CreateNewCharResult::Parse("DZ successful 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "DZ",
        "DZ missing data");
    failures += ExpectError(
        CreateNewCharResult::Parse("DZ successful 1 data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "DZ",
        "DZ extra field");
    failures += ExpectError(
        CreateNewCharResult::Parse("DZ failed _ error "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "DZ",
        "DZ invalid i62");
    return failures;
}

int TestManualPacketErrors()
{
    int failures = 0;
    failures += ExpectError(
        CreateNewCharResult::FromPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {Field("result", cg::packet::PacketFieldType::EscapedString, "successful"),
             Field("registnumber", cg::packet::PacketFieldType::Int62, "1"),
             Field("data", cg::packet::PacketFieldType::EscapedString, "")})),
        cg::packet::PacketErrorCode::DecodeFailed,
        "DZ",
        "DZ wrong direction");
    failures += ExpectError(
        CreateNewCharResult::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("status", cg::packet::PacketFieldType::EscapedString, "successful"),
             Field("registnumber", cg::packet::PacketFieldType::Int62, "1"),
             Field("data", cg::packet::PacketFieldType::EscapedString, "")})),
        cg::packet::PacketErrorCode::DecodeFailed,
        "DZ",
        "DZ wrong result field name");
    failures += ExpectError(
        CreateNewCharResult::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("result", cg::packet::PacketFieldType::EscapedString, "successful"),
             Field("registNumber", cg::packet::PacketFieldType::Int62, "1"),
             Field("data", cg::packet::PacketFieldType::EscapedString, "")})),
        cg::packet::PacketErrorCode::DecodeFailed,
        "DZ",
        "DZ wrong registnumber field name");
    failures += ExpectError(
        CreateNewCharResult::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("result", cg::packet::PacketFieldType::EscapedString, "successful"),
             Field("registnumber", cg::packet::PacketFieldType::Int62, "1"),
             Field("message", cg::packet::PacketFieldType::EscapedString, "")})),
        cg::packet::PacketErrorCode::DecodeFailed,
        "DZ",
        "DZ wrong data field name");
    failures += ExpectError(
        CreateNewCharResult::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("result", cg::packet::PacketFieldType::Int62, "1"),
             Field("registnumber", cg::packet::PacketFieldType::Int62, "1"),
             Field("data", cg::packet::PacketFieldType::EscapedString, "")})),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "DZ",
        "DZ wrong result field type");
    failures += ExpectError(
        CreateNewCharResult::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("result", cg::packet::PacketFieldType::EscapedString, "successful"),
             Field("registnumber", cg::packet::PacketFieldType::EscapedString, "1"),
             Field("data", cg::packet::PacketFieldType::EscapedString, "")})),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "DZ",
        "DZ wrong registnumber field type");
    failures += ExpectError(
        CreateNewCharResult::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("result", cg::packet::PacketFieldType::EscapedString, "successful"),
             Field("registnumber", cg::packet::PacketFieldType::Int62, "1"),
             Field("data", cg::packet::PacketFieldType::RawString, "")})),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "DZ",
        "DZ wrong data field type");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    cg::packet::Packet wrongName = CreateNewCharResult("successful", 1, "").ToPacket();
    wrongName.fields[0].name = "status";
    failures += ExpectError(
        cg::packet::BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "DZ",
        "DZ build wrong field name");

    cg::packet::Packet wrongType = CreateNewCharResult("successful", 1, "").ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::Int62;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "DZ",
        "DZ build wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSuccessfulEmptyData();
    failures += TestFailedMessage();
    failures += TestOpaqueDataRoundtrip();
    failures += TestParseErrors();
    failures += TestManualPacketErrors();
    failures += TestBuildErrors();

    if (failures != 0) {
        std::cerr << failures << " packet create new char result test(s) failed\n";
        return 1;
    }

    return 0;
}
