#include "packet/packet.hpp"

#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using ProcessStatus = cg::packet::receive::ProcessStatus;

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
        cg::packet::PacketCode(ProcessStatus::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(ProcessStatus::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == data, message);
    return failures;
}

int TestMultilineRoundtrip()
{
    const std::string data = "Proc: online,2|accept\nworker: idle\n";
    const ProcessStatus value(data);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = ExpectPacket(packet, data, "cJzI multiline packet");
    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "cJzI multiline build");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<ProcessStatus> parsed = ProcessStatus::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "cJzI multiline parse");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().data == data, "cJzI multiline value");
        failures += ExpectPacket(parsed.Value().ToPacket(), data, "cJzI multiline parsed packet");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::PacketResult<ProcessStatus> parsed = ProcessStatus::Parse("cJzI  ");

    int failures = Expect(static_cast<bool>(parsed), "cJzI empty parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().data.empty(), "cJzI empty value");
    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "cJzI empty build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "cJzI  ", "cJzI empty built text");
    }
    return failures;
}

int TestOpaqueDelimiters()
{
    int failures = 0;
    const std::vector<std::string> dataValues = {
        "left|right",
        "a,b,c",
        "line1|field,line\nline2,tail|done",
    };

    for (const std::string& data : dataValues) {
        const ProcessStatus value(data);
        const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(value.ToPacket());
        failures += Expect(static_cast<bool>(built), "cJzI delimiter build");
        if (!built.HasValue()) {
            continue;
        }

        const cg::packet::PacketResult<ProcessStatus> parsed = ProcessStatus::Parse(built.Value());
        failures += Expect(static_cast<bool>(parsed), "cJzI delimiter parse");
        if (parsed.HasValue()) {
            failures += Expect(parsed.Value().data == data, "cJzI delimiter value");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ProcessStatus::Parse("Zd status "),
        cg::packet::PacketErrorCode::InvalidCode,
        "Zd",
        "cJzI wrong code");
    failures += ExpectError(
        ProcessStatus::Parse("cJzI"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "cJzI",
        "cJzI missing data");
    failures += ExpectError(
        ProcessStatus::Parse("cJzI status extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "cJzI",
        "cJzI extra field");

    std::string invalidEs("cJzI ");
    invalidEs.push_back(static_cast<char>(0x80));
    invalidEs.push_back(' ');
    failures += ExpectError(
        ProcessStatus::Parse(invalidEs),
        cg::packet::PacketErrorCode::DecodeFailed,
        "cJzI",
        "cJzI invalid es");
    return failures;
}

int TestManualPacketErrors()
{
    int failures = 0;
    failures += ExpectError(
        ProcessStatus::FromPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {Field("data", cg::packet::PacketFieldType::EscapedString, "")})),
        cg::packet::PacketErrorCode::DecodeFailed,
        "cJzI",
        "cJzI wrong direction");
    failures += ExpectError(
        ProcessStatus::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("message", cg::packet::PacketFieldType::EscapedString, "")})),
        cg::packet::PacketErrorCode::DecodeFailed,
        "cJzI",
        "cJzI wrong field name");
    failures += ExpectError(
        ProcessStatus::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("data", cg::packet::PacketFieldType::RawString, "")})),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "cJzI",
        "cJzI wrong field type");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    cg::packet::Packet wrongName = ProcessStatus("").ToPacket();
    wrongName.fields[0].name = "message";
    failures += ExpectError(
        cg::packet::BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "cJzI",
        "cJzI build wrong field name");

    cg::packet::Packet wrongType = ProcessStatus("").ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::RawString;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "cJzI",
        "cJzI build wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestMultilineRoundtrip();
    failures += TestEmptyData();
    failures += TestOpaqueDelimiters();
    failures += TestParseErrors();
    failures += TestManualPacketErrors();
    failures += TestBuildErrors();

    if (failures != 0) {
        std::cerr << failures << " packet process status test(s) failed\n";
        return 1;
    }

    return 0;
}
