#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::TechList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::TechList::Spec);
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

std::string BuildPlainRspn(int skillIndex, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedSkillIndex = cg::packet::EncodeInt62(skillIndex);
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedSkillIndex.HasValue() || !encodedPayload.HasValue()) {
        return "Rspn  ";
    }

    std::string plain = "Rspn ";
    plain.append(encodedSkillIndex.Value());
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectTechListPacket(
    const cg::packet::Packet& packet,
    int skillIndex,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::TechList::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "skillIndex", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(skillIndex), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].value == value, message);
    return failures;
}

int ExpectRecord(
    const cg::packet::receive::TechRecord& record,
    std::string_view techName,
    std::string_view comment,
    int forcePoint,
    int field,
    int targetFlag,
    int canUse,
    const char* message)
{
    int failures = 0;
    failures += Expect(record.techName == techName, message);
    failures += Expect(record.comment == comment, message);
    failures += Expect(record.forcePoint == forcePoint, message);
    failures += Expect(record.field == field, message);
    failures += Expect(record.targetFlag == targetFlag, message);
    failures += Expect(record.canUse == canUse, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "Fire|Burn|10|2|3|1|";
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainRspn(62, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Rspn outer");
    if (parsed.HasValue()) {
        failures += ExpectTechListPacket(parsed.Value(), 62, payload, "parse Rspn outer packet");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::PacketResult<cg::packet::receive::TechList> parsed =
        cg::packet::receive::TechList::Parse(BuildPlainRspn(3, ""));

    int failures = Expect(static_cast<bool>(parsed), "parse Rspn empty data");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().skillIndex == 3, "parse Rspn empty skillIndex");
    failures += Expect(parsed.Value().records.empty(), "parse Rspn empty records");
    failures += ExpectTechListPacket(parsed.Value().ToPacket(), 3, "", "Rspn empty ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build Rspn empty data");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainRspn(3, ""), "build Rspn empty value");
    }
    return failures;
}

int TestSingleRecordWithTrailingDelimiter()
{
    const std::string payload = "Fire|Burn target|10|2|3|1|";
    const cg::packet::PacketResult<cg::packet::receive::TechList> parsed =
        cg::packet::receive::TechList::Parse(BuildPlainRspn(5, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Rspn single record");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().skillIndex == 5, "parse Rspn skillIndex");
    failures += Expect(parsed.Value().records.size() == 1U, "parse Rspn record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], "Fire", "Burn target", 10, 2, 3, 1, "parse Rspn record");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "build Rspn payload");
    return failures;
}

int TestRawEscapeRoundtrip()
{
    const std::string payload = "Tech\\zName|Comment\\cGreen\\yEnd|12|3|7|1|";
    const cg::packet::PacketResult<cg::packet::receive::TechList> parsed =
        cg::packet::receive::TechList::Parse(BuildPlainRspn(4, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Rspn raw escape payload");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "parse Rspn raw escape record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            "Tech\\zName",
            "Comment\\cGreen\\yEnd",
            12,
            3,
            7,
            1,
            "parse Rspn raw escape record");
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build Rspn raw escape packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::TechList> parsedAgain =
        cg::packet::receive::TechList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsedAgain), "parse rebuilt Rspn raw escape packet");
    if (parsedAgain.HasValue()) {
        failures += Expect(parsedAgain.Value().BuildData() == payload, "roundtrip Rspn raw escape payload");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::TechList::Parse(BuildPlainRspn(1, "Fire|Burn|10|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rspn",
        "typed Rspn payload field count");
    failures += ExpectError(
        cg::packet::receive::TechList::Parse(BuildPlainRspn(1, "Fire|Burn|10|2|3|1")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rspn",
        "typed Rspn missing trailing delimiter");
    failures += ExpectError(
        cg::packet::receive::TechList::Parse(BuildPlainRspn(1, "Fire|Burn|x|2|3|1|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rspn",
        "typed Rspn bad forcePoint decimal");
    failures += ExpectError(
        cg::packet::receive::TechList::Parse(BuildPlainRspn(1, "Fire|Burn|10|x|3|1|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rspn",
        "typed Rspn bad field decimal");
    failures += ExpectError(
        cg::packet::receive::TechList::Parse(BuildPlainRspn(1, "Fire|Burn|10|2|x|1|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rspn",
        "typed Rspn bad targetFlag decimal");
    failures += ExpectError(
        cg::packet::receive::TechList::Parse(BuildPlainRspn(1, "Fire|Burn|10|2|3|x|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rspn",
        "typed Rspn bad canUse decimal");
    failures += ExpectError(
        cg::packet::receive::TechList::Parse("Rspn _ data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rspn",
        "typed Rspn bad skillIndex i62");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyData();
    failures += TestSingleRecordWithTrailingDelimiter();
    failures += TestRawEscapeRoundtrip();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
