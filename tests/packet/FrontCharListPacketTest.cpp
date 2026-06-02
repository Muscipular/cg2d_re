#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::receive::FrontCharList::Spec;
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
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectFrontCharListPacket(
    const cg::packet::PacketResult<cg::packet::Packet>& packet,
    std::string_view value,
    const char* message)
{
    int failures = Expect(static_cast<bool>(packet), message);
    if (packet.HasValue()) {
        failures += ExpectFrontCharListPacket(packet.Value(), value, message);
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

    int failures = Expect(static_cast<bool>(parsed), "parse ysVc outer");
    if (parsed.HasValue()) {
        failures += ExpectFrontCharListPacket(parsed.Value(), payload, "parse ysVc outer packet");
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

int TestRawNamePreserved()
{
    const std::string payload = "A\\zB\\cC\\yD\\nE|0|";
    const cg::packet::PacketResult<cg::packet::receive::FrontCharList> parsed =
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse ysVc raw name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "ysVc raw name record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], "A\\zB\\cC\\yD\\nE", 0, "ysVc raw name record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "ysVc raw name BuildData");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += ExpectFrontCharListPacket(packet, payload, "ysVc raw name ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build ysVc raw name packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainYsVc(payload), "ysVc raw name roundtrip");
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
    int failures = ExpectFrontCharListPacket(packet, payload, "ysVc ToPacket");
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
    failures += ExpectError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice|0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        "ysVc missing trailing delimiter");
    failures += ExpectError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice|0|Pet|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ysVc",
        "ysVc odd payload fields");
    failures += ExpectError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice|x|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        "ysVc nonPlayerFlag non-decimal");
    failures += ExpectError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice||")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        "ysVc nonPlayerFlag rejects empty");
    failures += ExpectError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice|2|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        "ysVc nonPlayerFlag rejects 2");
    failures += ExpectError(
        cg::packet::receive::FrontCharList::Parse(BuildPlainYsVc("Alice|-1|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ysVc",
        "ysVc nonPlayerFlag rejects -1");
    return failures;
}

int TestBuildRejectsInvalidNonPlayerFlag()
{
    int failures = 0;
    const cg::packet::receive::FrontCharList frontCharListWithTwo({cg::packet::receive::FrontCharRecord {"Alice", 2}});
    failures += ExpectError(
        frontCharListWithTwo.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ysVc",
        "ysVc BuildData rejects 2");
    failures += ExpectError(
        frontCharListWithTwo.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ysVc",
        "ysVc ToPacket rejects 2");

    const cg::packet::receive::FrontCharList frontCharListWithNegative(
        {cg::packet::receive::FrontCharRecord {"Alice", -1}});
    failures += ExpectError(
        frontCharListWithNegative.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ysVc",
        "ysVc BuildData rejects -1");
    failures += ExpectError(
        frontCharListWithNegative.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ysVc",
        "ysVc ToPacket rejects -1");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyData();
    failures += TestTwoRecords();
    failures += TestRawNamePreserved();
    failures += TestConstructRoundtrip();
    failures += TestErrors();
    failures += TestBuildRejectsInvalidNonPlayerFlag();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
