#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::ItemFrontCharList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ItemFrontCharList::Spec);
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

std::string BuildPlain(std::string_view code, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        std::string plain(code);
        plain.append("  ");
        return plain;
    }

    std::string plain(code);
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

std::string BuildPlainLy(std::string_view payload)
{
    return BuildPlain(cg::packet::receive::ItemFrontCharList::Code, payload);
}

int ExpectItemFrontCharListPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ItemFrontCharList::Code), message);
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

int ExpectItemFrontCharListPacket(
    const cg::packet::PacketResult<cg::packet::Packet>& packet,
    std::string_view value,
    const char* message)
{
    int failures = Expect(static_cast<bool>(packet), message);
    if (packet.HasValue()) {
        failures += ExpectItemFrontCharListPacket(packet.Value(), value, message);
    }
    return failures;
}

int ExpectBuildData(
    const cg::packet::receive::ItemFrontCharList& itemFrontCharList,
    std::string_view value,
    const char* message)
{
    const cg::packet::PacketResult<std::string> data = itemFrontCharList.BuildData();
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
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlainLy(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse LY outer");
    if (parsed.HasValue()) {
        failures += ExpectItemFrontCharListPacket(parsed.Value(), payload, "parse LY outer packet");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::PacketResult<cg::packet::receive::ItemFrontCharList> parsed =
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy(""));

    int failures = Expect(static_cast<bool>(parsed), "parse LY empty data");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "LY empty data record count");
        failures += ExpectBuildData(parsed.Value(), "", "LY empty data BuildData");
    }
    return failures;
}

int TestTwoRecords()
{
    const std::string payload = "Alice|0|Pet|1|";
    const cg::packet::PacketResult<cg::packet::receive::ItemFrontCharList> parsed =
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse LY two records");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 2U, "LY two record count");
    if (parsed.Value().records.size() == 2U) {
        failures += ExpectRecord(parsed.Value().records[0], "Alice", 0, "LY first record");
        failures += ExpectRecord(parsed.Value().records[1], "Pet", 1, "LY second record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "LY two records BuildData");
    return failures;
}

int TestOuterSpaceNameRoundtrip()
{
    const std::string payload = "  Alice Bob  |0|";
    const cg::packet::PacketResult<cg::packet::receive::ItemFrontCharList> parsed =
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse LY spaced name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "LY spaced name record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], "  Alice Bob  ", 0, "LY spaced name record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "LY spaced name BuildData");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += ExpectItemFrontCharListPacket(packet, payload, "LY spaced name ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build LY spaced name packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainLy(payload), "LY spaced name roundtrip");
    }
    return failures;
}

int TestRawNamePreserved()
{
    const std::string payload = "A\\zB\\cC\\yD\\nE|0|";
    const cg::packet::PacketResult<cg::packet::receive::ItemFrontCharList> parsed =
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse LY raw name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "LY raw name record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], "A\\zB\\cC\\yD\\nE", 0, "LY raw name record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "LY raw name BuildData");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += ExpectItemFrontCharListPacket(packet, payload, "LY raw name ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build LY raw name packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainLy(payload), "LY raw name roundtrip");
    }
    return failures;
}

int TestConstructRoundtrip()
{
    std::vector<cg::packet::receive::FrontCharRecord> records;
    records.push_back(cg::packet::receive::FrontCharRecord {"Alice", 0});
    records.push_back(cg::packet::receive::FrontCharRecord {"Pet", 1});
    const cg::packet::receive::ItemFrontCharList itemFrontCharList(std::move(records));
    const std::string payload = "Alice|0|Pet|1|";

    const cg::packet::PacketResult<cg::packet::Packet> packet = itemFrontCharList.ToPacket();
    int failures = ExpectItemFrontCharListPacket(packet, payload, "LY ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build LY packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::ItemFrontCharList> parsed =
        cg::packet::receive::ItemFrontCharList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip LY packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 2U, "roundtrip LY record count");
        failures += ExpectBuildData(parsed.Value(), payload, "roundtrip LY payload");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice|0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        "LY missing trailing delimiter");
    failures += ExpectError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice|0|Bob|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LY",
        "LY odd payload fields");
    failures += ExpectError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice||")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        "LY nonPlayerFlag rejects empty");
    failures += ExpectError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice|x|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        "LY nonPlayerFlag non-decimal");
    failures += ExpectError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice|2|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        "LY nonPlayerFlag rejects 2");
    failures += ExpectError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice|-1|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        "LY nonPlayerFlag rejects -1");
    failures += ExpectError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlain("ysVc", "Alice|0|")),
        cg::packet::PacketErrorCode::InvalidCode,
        "ysVc",
        "LY rejects wrong code");
    return failures;
}

int TestBuildRejectsInvalidNonPlayerFlag()
{
    int failures = 0;
    const cg::packet::receive::ItemFrontCharList itemFrontCharListWithTwo(
        {cg::packet::receive::FrontCharRecord {"Alice", 2}});
    failures += ExpectError(
        itemFrontCharListWithTwo.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LY",
        "LY BuildData rejects 2");
    failures += ExpectError(
        itemFrontCharListWithTwo.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LY",
        "LY ToPacket rejects 2");

    const cg::packet::receive::ItemFrontCharList itemFrontCharListWithNegative(
        {cg::packet::receive::FrontCharRecord {"Alice", -1}});
    failures += ExpectError(
        itemFrontCharListWithNegative.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LY",
        "LY BuildData rejects -1");
    failures += ExpectError(
        itemFrontCharListWithNegative.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LY",
        "LY ToPacket rejects -1");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyData();
    failures += TestTwoRecords();
    failures += TestOuterSpaceNameRoundtrip();
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
