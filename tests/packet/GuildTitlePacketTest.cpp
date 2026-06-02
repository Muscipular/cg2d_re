#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::GuildTitle::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::GuildTitle::Spec);
}

using GuildTitle = cg::packet::receive::GuildTitle;
using GuildTitleRecord = cg::packet::receive::GuildTitleRecord;

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

std::string BuildPlain(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "znv ";
    }

    std::string plain(GuildTitle::Code);
    plain.push_back(' ');
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

std::string BuildRawPlain(std::string_view payload)
{
    std::string plain(GuildTitle::Code);
    plain.push_back(' ');
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

int ExpectGuildTitlePacket(
    const cg::packet::Packet& packet,
    std::string_view payload,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(GuildTitle::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == payload, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildRawPlain("0"));

    int failures = Expect(static_cast<bool>(parsed), "parse znv outer");
    if (parsed.HasValue()) {
        failures += ExpectGuildTitlePacket(parsed.Value(), "0", "parse znv outer packet");
    }
    return failures;
}

int TestEmptyList()
{
    const cg::packet::PacketResult<GuildTitle> parsed = GuildTitle::Parse(BuildPlain("0"));

    int failures = Expect(static_cast<bool>(parsed), "parse znv empty list");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "znv empty records");
        failures += Expect(parsed.Value().BuildData() == "0", "znv empty BuildData");
        const cg::packet::Packet packet = parsed.Value().ToPacket();
        failures += ExpectGuildTitlePacket(packet, "0", "znv empty packet");
    }

    const cg::packet::PacketResult<GuildTitle> tail = GuildTitle::Parse(BuildRawPlain("0|"));
    failures += Expect(static_cast<bool>(tail), "parse znv empty list final empty tail");
    if (tail.HasValue()) {
        failures += Expect(tail.Value().records.empty(), "znv empty tail records");
    }
    return failures;
}

int TestOneAndTwoRecords()
{
    int failures = 0;
    const cg::packet::PacketResult<GuildTitle> one = GuildTitle::Parse(BuildPlain("1|10|2|Leader"));
    failures += Expect(static_cast<bool>(one), "parse znv one record");
    if (one.HasValue()) {
        failures += Expect(one.Value().records.size() == 1U, "znv one count");
        if (one.Value().records.size() == 1U) {
            failures += Expect(one.Value().records[0].seq == 10, "znv one seq");
            failures += Expect(one.Value().records[0].commition == 2, "znv one commition");
            failures += Expect(one.Value().records[0].titleNameRaw == "Leader", "znv one title");
        }
        failures += Expect(one.Value().BuildData() == "1|10|2|Leader", "znv one BuildData");
    }

    const std::vector<GuildTitleRecord> records {
        {1, 2, "Leader"},
        {3, 4, "Member"},
    };
    const GuildTitle guildTitle(records);
    failures += Expect(guildTitle.BuildData() == "2|1|2|Leader|3|4|Member", "znv two BuildData");

    const cg::packet::Packet packet = guildTitle.ToPacket();
    failures += ExpectGuildTitlePacket(packet, "2|1|2|Leader|3|4|Member", "znv two packet");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build znv two packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain("2|1|2|Leader|3|4|Member"), "build znv two value");
    }
    return failures;
}

int TestClearCommition()
{
    const cg::packet::PacketResult<GuildTitle> parsed = GuildTitle::Parse(BuildRawPlain("1|8|-1|"));

    int failures = Expect(static_cast<bool>(parsed), "parse znv commition -1");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "znv commition -1 count");
    if (parsed.Value().records.size() == 1U) {
        failures += Expect(parsed.Value().records[0].seq == 8, "znv commition -1 seq");
        failures += Expect(parsed.Value().records[0].commition == -1, "znv commition -1 value");
        failures += Expect(parsed.Value().records[0].titleNameRaw.empty(), "znv commition -1 empty title");
    }
    failures += Expect(parsed.Value().BuildData() == "1|8|-1|", "znv commition -1 BuildData");
    return failures;
}

int TestRawEscapePreserved()
{
    const std::string payload = "1|1|2|A\\zB\\cC\\yD\\nE";
    const cg::packet::PacketResult<GuildTitle> parsed = GuildTitle::Parse(BuildRawPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse znv raw escape");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "znv raw escape count");
    if (parsed.Value().records.size() == 1U) {
        failures += Expect(
            parsed.Value().records[0].titleNameRaw == "A\\zB\\cC\\yD\\nE",
            "znv raw escape title");
    }

    const cg::packet::Packet packet = parsed.Value().ToPacket();
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build znv raw escape");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == "znv 1|1|2|A\\\\zB\\\\cC\\\\yD\\\\nE ",
            "build znv raw escaped value");
    }
    return failures;
}

int TestFinalEmptyTail()
{
    const cg::packet::PacketResult<GuildTitle> parsed = GuildTitle::Parse(BuildRawPlain("1|1|2|Title|"));

    int failures = Expect(static_cast<bool>(parsed), "parse znv final empty tail");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 1U, "znv final empty tail count");
        failures += Expect(parsed.Value().BuildData() == "1|1|2|Title", "znv final empty tail BuildData");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        GuildTitle::Parse("znv  "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "znv",
        "znv empty data invalid");
    failures += ExpectError(
        GuildTitle::Parse(BuildPlain("x")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "znv",
        "znv cnt nondecimal");
    failures += ExpectError(
        GuildTitle::Parse(BuildPlain("-1")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "znv",
        "znv cnt negative");
    failures += ExpectError(
        GuildTitle::Parse(BuildPlain("1|1|2")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "znv",
        "znv missing title segment");
    failures += ExpectError(
        GuildTitle::Parse(BuildPlain("2|1|2|Title")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "znv",
        "znv count mismatch");
    failures += ExpectError(
        GuildTitle::Parse(BuildPlain("1|1|2|Title|extra")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "znv",
        "znv final nonempty extra");
    failures += ExpectError(
        GuildTitle::Parse(BuildPlain("1|x|2|Title")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "znv",
        "znv seq nondecimal");
    failures += ExpectError(
        GuildTitle::Parse(BuildPlain("1|1|x|Title")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "znv",
        "znv commition nondecimal");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyList();
    failures += TestOneAndTwoRecords();
    failures += TestClearCommition();
    failures += TestRawEscapePreserved();
    failures += TestFinalEmptyTail();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
