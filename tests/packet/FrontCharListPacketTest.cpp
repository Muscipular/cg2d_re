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
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::FrontCharList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::FrontCharList::Spec);
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
int ExpectFieldError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::string_view packetCode,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, packetCode, message);
    if (!result.HasError()) {
        return failures;
    }

    failures += Expect(result.Error().fieldIndex.has_value(), message);
    if (result.Error().fieldIndex.has_value()) {
        failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
    }
    return failures;
}

std::string EncodeLegacy(std::string_view value)
{
    const cg::packet::PacketResult<std::string> encoded =
        cg::packet::EncodeLegacyPayloadText(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string WithGbkTrailPipe(std::string_view prefix)
{
    std::string value(prefix);
    value.push_back(static_cast<char>(0x96));
    value.push_back(static_cast<char>(0x7C));
    return value;
}

std::string BuildRecordsData(
    const std::vector<cg::packet::receive::FrontCharRecord>& records)
{
    std::string data;
    for (const cg::packet::receive::FrontCharRecord& record : records) {
        data.append(EncodeLegacy(record.name));
        data.push_back('|');
        data.append(std::to_string(record.nonPlayerFlag));
        data.push_back('|');
    }
    return data;
}

std::string BuildPlainYsVc(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        return "ysVc  ";
    }

    std::string plain = "ysVc ";
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectFrontCharListPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    cg::packet::PacketFieldType dataType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::FrontCharList::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == dataType, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectFrontCharListPacket(
    const cg::packet::PacketResult<cg::packet::Packet>& packet,
    std::string_view value,
    cg::packet::PacketFieldType dataType,
    const char* message)
{
    int failures = Expect(static_cast<bool>(packet), message);
    if (packet.HasValue()) {
        failures += ExpectFrontCharListPacket(packet.Value(), value, dataType, message);
    }
    return failures;
}

int ExpectBuildData(
    const cg::packet::receive::FrontCharList& frontCharList,
    std::string_view value,
    const char* message)
{
    const cg::packet::PacketResult<std::string> data = frontCharList.BuildData();
    int failures = Expect(static_cast<bool>(data), message);
    if (data.HasValue()) {
        failures += Expect(data.Value() == value, message);
    }
    return failures;
}

int ExpectRecord(
    const cg::packet::receive::FrontCharRecord& record,
    std::string_view name,
    int nonPlayerFlag,
    const char* message)
{
    int failures = 0;
    failures += Expect(record.name == name, message);
    failures += Expect(record.nonPlayerFlag == nonPlayerFlag, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "Alice|0|Pet|1|";
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlainYsVc(payload));

    int failures = Expect(
        cg::packet::receive::FrontCharList::Fields[0U].type ==
            cg::packet::PacketFieldType::SubPayload,
        "ysVc data spec SubPayload");
    failures += Expect(static_cast<bool>(parsed), "parse ysVc outer");
    if (parsed.HasValue()) {
        failures += ExpectFrontCharListPacket(
            parsed.Value(),
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse ysVc outer packet");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::PacketResult<cg::packet::receive::FrontCharList> parsed =
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc(""));

    int failures = Expect(static_cast<bool>(parsed), "parse ysVc empty data");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "ysVc empty data record count");
        failures += ExpectBuildData(parsed.Value(), "", "ysVc empty data BuildData");
        failures += ExpectFrontCharListPacket(
            parsed.Value().ToPacket(),
            "",
            cg::packet::PacketFieldType::SubPayload,
            "ysVc empty data ToPacket");
    }
    return failures;
}

int TestTwoRecords()
{
    const std::string payload = "Alice|0|Pet|1|";
    const cg::packet::PacketResult<cg::packet::receive::FrontCharList> parsed =
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse ysVc two records");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 2U, "ysVc two record count");
    if (parsed.Value().records.size() == 2U) {
        failures += ExpectRecord(parsed.Value().records[0], "Alice", 0, "ysVc first record");
        failures += ExpectRecord(parsed.Value().records[1], "Pet", 1, "ysVc second record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "ysVc two records BuildData");
    return failures;
}

int TestTenRecords()
{
    std::vector<cg::packet::receive::FrontCharRecord> records;
    records.reserve(10U);
    for (int index = 0; index < 10; ++index) {
        records.push_back(cg::packet::receive::FrontCharRecord {
            index == 9 ? WithGbkTrailPipe("Name9") : "Name" + std::to_string(index),
            index % 2,
        });
    }
    const std::string lastName = records.back().name;
    const std::string payload = BuildRecordsData(records);
    const cg::packet::receive::FrontCharList value(std::move(records));

    int failures = ExpectBuildData(value, payload, "ysVc ten records BuildData");
    const cg::packet::PacketResult<cg::packet::Packet> packet = value.ToPacket();
    failures += ExpectFrontCharListPacket(
        packet,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "ysVc ten records ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build ysVc ten records packet");
    if (!built.HasValue()) {
        return failures;
    }
    const cg::packet::PacketResult<cg::packet::receive::FrontCharList> parsed =
        cg::packet::receive::FrontCharList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse ysVc ten records");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 10U, "ysVc ten-record upper bound");
        if (parsed.Value().records.size() == 10U) {
            failures += ExpectRecord(parsed.Value().records[9], lastName, 1, "ysVc tenth semantic record");
        }
    }
    return failures;
}

int TestSemanticNameRoundtrip()
{
    std::string name = "A|B,C\\D\nE";
    name.push_back(static_cast<char>(0x96));
    name.push_back(static_cast<char>(0x7C));
    std::string payload = "A\\zB\\cC\\yD\\nE";
    payload.push_back(static_cast<char>(0x96));
    payload.push_back(static_cast<char>(0x7C));
    payload.append("|0|");
    const cg::packet::PacketResult<cg::packet::receive::FrontCharList> parsed =
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse ysVc semantic GBK/legacy name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "ysVc one record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], name, 0, "ysVc semantic name record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "ysVc semantic name BuildData");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += ExpectFrontCharListPacket(
        packet,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "ysVc semantic name ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build ysVc semantic name packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainYsVc(payload), "ysVc semantic name roundtrip");
    }
    return failures;
}

int TestConstructRoundtrip()
{
    std::vector<cg::packet::receive::FrontCharRecord> records;
    records.push_back(cg::packet::receive::FrontCharRecord {"Alice", 0});
    records.push_back(cg::packet::receive::FrontCharRecord {"Pet", 1});
    const cg::packet::receive::FrontCharList frontCharList(std::move(records));
    const std::string payload = "Alice|0|Pet|1|";

    const cg::packet::PacketResult<cg::packet::Packet> packet = frontCharList.ToPacket();
    int failures = ExpectFrontCharListPacket(
        packet,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "ysVc ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build ysVc packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::FrontCharList> parsed =
        cg::packet::receive::FrontCharList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip ysVc packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 2U, "roundtrip ysVc record count");
        failures += ExpectBuildData(parsed.Value(), payload, "roundtrip ysVc payload");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectFieldError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice|0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        0U,
        "ysVc missing trailing delimiter");
    failures += ExpectFieldError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice|0|Pet|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ysVc",
        0U,
        "ysVc odd payload fields");
    failures += ExpectFieldError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice|x|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        1U,
        "ysVc nonPlayerFlag non-decimal");
    failures += ExpectFieldError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice||")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        1U,
        "ysVc nonPlayerFlag rejects empty");
    failures += ExpectFieldError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice|2|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        1U,
        "ysVc nonPlayerFlag rejects 2");
    failures += ExpectFieldError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice|-1|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        1U,
        "ysVc nonPlayerFlag rejects -1");
    failures += ExpectFieldError(
        cg::packet::receive::FrontCharList::Parse("ysVc"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ysVc",
        0U,
        "ysVc outer missing data index");
    failures += ExpectFieldError(
        cg::packet::receive::FrontCharList::Parse("ysVc data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ysVc",
        1U,
        "ysVc outer extra field index");
    failures += ExpectFieldError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice|0|Pet|2|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        3U,
        "ysVc second flag field index");
    failures += ExpectFieldError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Name\\|0|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        0U,
        "ysVc first name legacy truncated index");
    failures += ExpectFieldError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice|0|Name\\|1|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        2U,
        "ysVc second name legacy truncated index");

    const std::vector<cg::packet::receive::FrontCharRecord> elevenRecords(
        11U,
        cg::packet::receive::FrontCharRecord {"Name", 0});
    failures += ExpectFieldError(
        cg::packet::receive::FrontCharList::Parse(
            BuildPlainYsVc(BuildRecordsData(elevenRecords))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ysVc",
        0U,
        "ysVc parse eleven records rejected");
    return failures;
}

int TestBuildRejectsInvalidNonPlayerFlag()
{
    int failures = 0;
    const cg::packet::receive::FrontCharList frontCharListWithTwo({cg::packet::receive::FrontCharRecord {"Alice", 2}});
    failures += ExpectFieldError(
        frontCharListWithTwo.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ysVc",
        1U,
        "ysVc BuildData rejects 2");
    failures += ExpectFieldError(
        frontCharListWithTwo.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ysVc",
        1U,
        "ysVc ToPacket rejects 2");

    const cg::packet::receive::FrontCharList frontCharListWithNegative(
        {cg::packet::receive::FrontCharRecord {"Alice", -1}});
    failures += ExpectFieldError(
        frontCharListWithNegative.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ysVc",
        1U,
        "ysVc BuildData rejects -1");
    failures += ExpectFieldError(
        frontCharListWithNegative.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ysVc",
        1U,
        "ysVc ToPacket rejects -1");

    std::string truncatedName(1U, static_cast<char>(0x96));
    const cg::packet::receive::FrontCharList frontCharListWithTruncatedName({
        cg::packet::receive::FrontCharRecord {"Alice", 0},
        cg::packet::receive::FrontCharRecord {std::move(truncatedName), 1},
    });
    failures += ExpectFieldError(
        frontCharListWithTruncatedName.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ysVc",
        2U,
        "ysVc second name BuildData encoding failure index");
    failures += ExpectFieldError(
        frontCharListWithTruncatedName.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ysVc",
        2U,
        "ysVc second name ToPacket encoding failure index");

    const std::vector<cg::packet::receive::FrontCharRecord> elevenRecords(
        11U,
        cg::packet::receive::FrontCharRecord {"Name", 0});
    const cg::packet::receive::FrontCharList frontCharListWithEleven(elevenRecords);
    failures += ExpectFieldError(
        frontCharListWithEleven.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ysVc",
        0U,
        "ysVc BuildData rejects eleven records");
    failures += ExpectFieldError(
        frontCharListWithEleven.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ysVc",
        0U,
        "ysVc ToPacket rejects eleven records");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyData();
    failures += TestTwoRecords();
    failures += TestTenRecords();
    failures += TestSemanticNameRoundtrip();
    failures += TestConstructRoundtrip();
    failures += TestErrors();
    failures += TestBuildRejectsInvalidNonPlayerFlag();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
