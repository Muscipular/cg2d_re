#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

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

template <typename T>
int ExpectErrorAt(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, "Rspn", message);
    if (result.HasError()) {
        failures += Expect(result.Error().fieldIndex.has_value(), message);
        if (result.Error().fieldIndex.has_value()) {
            failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
        }
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
    cg::packet::PacketFieldType dataFieldType,
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
    failures += Expect(packet.fields[1].type == dataFieldType, message);
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
        failures += ExpectTechListPacket(
            parsed.Value(),
            62,
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse Rspn outer packet");
    }
    failures += Expect(
        cg::packet::receive::TechList::Fields[1].type == cg::packet::PacketFieldType::SubPayload,
        "Rspn data spec is SubPayload");
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
    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "Rspn empty ToPacket");
    if (packet.HasValue()) {
        failures += ExpectTechListPacket(
            packet.Value(),
            3,
            "",
            cg::packet::PacketFieldType::SubPayload,
            "Rspn empty ToPacket value");
        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "build Rspn empty data");
        if (built.HasValue()) {
            failures += Expect(built.Value() == BuildPlainRspn(3, ""), "build Rspn empty value");
        }
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
    const cg::packet::PacketResult<std::string> built = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(built), "build Rspn payload");
    if (built.HasValue()) {
        failures += Expect(built.Value() == payload, "build Rspn payload value");
    }
    return failures;
}

int TestLegacyEscapeCanonical()
{
    const std::string legacy =
        "Tech\\zName|line\\ncomma\\cslash\\yunknown\\q|12|3|7|1|";
    const std::string canonical =
        "Tech\\zName|line\\ncomma\\cslash\\yunknownq|12|3|7|1|";
    const cg::packet::PacketResult<cg::packet::receive::TechList> parsed =
        cg::packet::receive::TechList::Parse(BuildPlainRspn(4, legacy));

    int failures = Expect(static_cast<bool>(parsed), "parse Rspn legacy escape payload");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "parse Rspn legacy escape record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            "Tech|Name",
            "line\ncomma,slash\\unknownq",
            12,
            3,
            7,
            1,
            "decode Rspn legacy escapes");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "build Rspn canonical legacy payload");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonical, "canonicalize Rspn legacy escapes");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "Rspn legacy ToPacket");
    if (packet.HasValue()) {
        failures += ExpectTechListPacket(
            packet.Value(),
            4,
            canonical,
            cg::packet::PacketFieldType::SubPayload,
            "Rspn legacy ToPacket field");
        const cg::packet::PacketResult<std::string> wire = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(wire), "build Rspn canonical legacy wire");
        if (wire.HasValue()) {
            failures += Expect(wire.Value() == BuildPlainRspn(4, canonical), "Rspn canonical legacy wire value");
        }
    }
    return failures;
}

int TestGbkTrailDelimiter()
{
    std::string techName;
    techName.push_back(static_cast<char>(0x96));
    techName.push_back(static_cast<char>(0x7c));

    std::string payload = techName;
    payload.append("|Comment|12|3|7|1|");
    const cg::packet::PacketResult<cg::packet::receive::TechList> parsed =
        cg::packet::receive::TechList::Parse(BuildPlainRspn(4, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Rspn GBK 96 7C");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "parse Rspn GBK record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            techName,
            "Comment",
            12,
            3,
            7,
            1,
            "parse Rspn GBK techName");
    }

    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build Rspn GBK 96 7C");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip Rspn GBK 96 7C");
    }
    return failures;
}

int TestPacketLogSample()
{
    const std::string techName {
        static_cast<char>(0xd4), static_cast<char>(0xc9),
        static_cast<char>(0xca), static_cast<char>(0xaf),
        static_cast<char>(0xc4), static_cast<char>(0xa7),
        static_cast<char>(0xb7), static_cast<char>(0xa8),
        'L', 'V', '1',
    };
    const std::string comment {
        static_cast<char>(0xcf), static_cast<char>(0xf2),
        static_cast<char>(0xb5), static_cast<char>(0xa5),
        static_cast<char>(0xcc), static_cast<char>(0xe5),
        static_cast<char>(0xb6), static_cast<char>(0xd4),
        static_cast<char>(0xcf), static_cast<char>(0xf3),
        static_cast<char>(0xca), static_cast<char>(0xa9),
        static_cast<char>(0xd5), static_cast<char>(0xb9),
        static_cast<char>(0xb5), static_cast<char>(0xd8),
        static_cast<char>(0xca), static_cast<char>(0xf4),
        static_cast<char>(0xd0), static_cast<char>(0xd4),
        static_cast<char>(0xb5), static_cast<char>(0xc4),
        static_cast<char>(0xc4), static_cast<char>(0xa7),
        static_cast<char>(0xb7), static_cast<char>(0xa8),
        static_cast<char>(0xb9), static_cast<char>(0xa5),
        static_cast<char>(0xbb), static_cast<char>(0xf7),
    };
    std::string payload = techName;
    payload.push_back('|');
    payload.append(comment);
    payload.append("|10|1|4221|0|");
    const cg::packet::PacketResult<cg::packet::receive::TechList> parsed =
        cg::packet::receive::TechList::Parse(BuildPlainRspn(10, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Rspn packet log sample");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().skillIndex == 10, "Rspn packet log skillIndex");
    failures += Expect(parsed.Value().records.size() == 1U, "Rspn packet log record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            techName,
            comment,
            10,
            1,
            4221,
            0,
            "Rspn packet log record");
    }
    const cg::packet::PacketResult<std::string> built = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(built), "Rspn packet log BuildData");
    if (built.HasValue()) {
        failures += Expect(built.Value() == payload, "Rspn packet log BuildData value");
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
    failures += ExpectErrorAt(
        cg::packet::receive::TechList::Parse(
            BuildPlainRspn(1, "Name\\|Comment|10|2|3|1|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "typed Rspn truncated techName escape");
    failures += ExpectErrorAt(
        cg::packet::receive::TechList::Parse(
            BuildPlainRspn(1, "Name|Comment\\|10|2|3|1|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "typed Rspn truncated comment escape");

    std::string truncatedGbk = "Rspn 1 Name|Comment|10|2|3|1|";
    truncatedGbk.push_back(static_cast<char>(0x81));
    failures += ExpectErrorAt(
        cg::packet::receive::TechList::Parse(truncatedGbk),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "typed Rspn truncated GBK payload");
    return failures;
}

int TestBuildAndToPacketErrors()
{
    std::string truncatedGbk(1U, static_cast<char>(0x81));
    const cg::packet::receive::TechList techList(
        1,
        {
            {"Valid", "Comment", 1, 2, 3, 4},
            {"Second", std::move(truncatedGbk), 5, 6, 7, 8},
        });

    int failures = 0;
    failures += ExpectErrorAt(
        techList.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        7U,
        "Rspn BuildData truncated GBK context");
    failures += ExpectErrorAt(
        techList.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        7U,
        "Rspn ToPacket truncated GBK context");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyData();
    failures += TestSingleRecordWithTrailingDelimiter();
    failures += TestLegacyEscapeCanonical();
    failures += TestGbkTrailDelimiter();
    failures += TestPacketLogSample();
    failures += TestErrors();
    failures += TestBuildAndToPacketErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
