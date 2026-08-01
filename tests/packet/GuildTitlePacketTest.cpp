#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <initializer_list>
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

template <typename T>
int ExpectFieldError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::string_view packetCode,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, packetCode, message);
    if (result.HasError()) {
        failures += Expect(result.Error().fieldIndex.has_value(), message);
        if (result.Error().fieldIndex.has_value()) {
            failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
        }
    }
    return failures;
}

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string output;
    output.reserve(values.size());
    for (const unsigned int value : values) {
        output.push_back(static_cast<char>(value));
    }
    return output;
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
    cg::packet::PacketFieldType payloadType,
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
    failures += Expect(packet.fields[0].type == payloadType, message);
    failures += Expect(packet.fields[0].value == payload, message);
    return failures;
}

int ExpectGuildTitlePacket(
    const cg::packet::PacketResult<cg::packet::Packet>& packet,
    std::string_view payload,
    cg::packet::PacketFieldType payloadType,
    const char* message)
{
    int failures = Expect(static_cast<bool>(packet), message);
    if (packet.HasValue()) {
        failures += ExpectGuildTitlePacket(packet.Value(), payload, payloadType, message);
    }
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildRawPlain("0"));

    int failures = Expect(static_cast<bool>(parsed), "parse znv outer");
    if (parsed.HasValue()) {
        failures += ExpectGuildTitlePacket(
            parsed.Value(),
            "0",
            cg::packet::PacketFieldType::EscapedString,
            "parse znv outer packet");
    }
    return failures;
}

int TestEmptyList()
{
    const cg::packet::PacketResult<GuildTitle> parsed = GuildTitle::Parse(BuildPlain("0"));

    int failures = Expect(static_cast<bool>(parsed), "parse znv empty list");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "znv empty records");
        const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
        failures += Expect(data.HasValue() && data.Value() == "0", "znv empty BuildData");
        failures += ExpectGuildTitlePacket(
            parsed.Value().ToPacket(),
            "0",
            cg::packet::PacketFieldType::SubPayload,
            "znv empty packet");
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
    const std::string onePayload = "1|10|2|Leader|";
    const cg::packet::PacketResult<GuildTitle> one = GuildTitle::Parse(BuildPlain(onePayload));
    failures += Expect(static_cast<bool>(one), "parse znv one record");
    if (one.HasValue()) {
        failures += Expect(one.Value().records.size() == 1U, "znv one count");
        if (one.Value().records.size() == 1U) {
            failures += Expect(one.Value().records[0].seq == 10, "znv one seq");
            failures += Expect(one.Value().records[0].commition == 2, "znv one commition");
            failures += Expect(one.Value().records[0].titleNameRaw == "Leader", "znv one title");
        }
        const cg::packet::PacketResult<std::string> data = one.Value().BuildData();
        failures += Expect(data.HasValue() && data.Value() == onePayload, "znv one BuildData");
    }

    const std::vector<GuildTitleRecord> records {
        {1, 2, "Leader"},
        {3, 4, "Member"},
    };
    const GuildTitle guildTitle(records);
    const std::string twoPayload = "2|1|2|Leader|3|4|Member|";
    const cg::packet::PacketResult<std::string> data = guildTitle.BuildData();
    failures += Expect(data.HasValue() && data.Value() == twoPayload, "znv two BuildData");

    const cg::packet::PacketResult<cg::packet::Packet> packet = guildTitle.ToPacket();
    failures += ExpectGuildTitlePacket(
        packet,
        twoPayload,
        cg::packet::PacketFieldType::SubPayload,
        "znv two packet");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build znv two packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain(twoPayload), "build znv two value");
    }
    return failures;
}

int TestClearCommition()
{
    const std::string serverClear = "1|8|||";
    const cg::packet::PacketResult<GuildTitle> parsed = GuildTitle::Parse(BuildPlain(serverClear));

    int failures = Expect(static_cast<bool>(parsed), "parse znv server clear");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "znv commition -1 count");
    if (parsed.Value().records.size() == 1U) {
        failures += Expect(parsed.Value().records[0].seq == 8, "znv commition -1 seq");
        failures += Expect(parsed.Value().records[0].commition == -1, "znv commition -1 value");
        failures += Expect(parsed.Value().records[0].titleNameRaw.empty(), "znv commition -1 empty title");
    }
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue() && data.Value() == serverClear, "znv server clear canonical BuildData");

    const cg::packet::PacketResult<GuildTitle> literal = GuildTitle::Parse(BuildPlain("1|8|-1||"));
    failures += Expect(static_cast<bool>(literal), "parse znv literal -1 compatibility");
    if (literal.HasValue()) {
        const cg::packet::PacketResult<std::string> literalData = literal.Value().BuildData();
        failures += Expect(
            literalData.HasValue() && literalData.Value() == serverClear,
            "znv literal -1 normalizes to server clear");
    }

    const GuildTitle constructed({GuildTitleRecord {8, -1, "ignored"}});
    const cg::packet::PacketResult<std::string> constructedData = constructed.BuildData();
    failures += Expect(
        constructedData.HasValue() && constructedData.Value() == serverClear,
        "znv constructed clear normalizes empty title");
    return failures;
}

int TestLegacyTextRoundtrip()
{
    std::string semantic = Bytes({0xC3, 0xFB, 0x96, 0x7C, 0x96, 0x5C});
    semantic.append("A|B,C\\D\nE");
    std::string encoded = Bytes({0xC3, 0xFB, 0x96, 0x7C, 0x96, 0x5C});
    encoded.append("A\\zB\\cC\\yD\\nE");
    const std::string payload = "1|1|2|" + encoded + "|";
    const cg::packet::PacketResult<GuildTitle> parsed = GuildTitle::Parse(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse znv legacy title");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "znv legacy title count");
    if (parsed.Value().records.size() == 1U) {
        failures += Expect(parsed.Value().records[0].titleNameRaw == semantic, "znv legacy semantic title");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue() && data.Value() == payload, "znv legacy canonical BuildData");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += ExpectGuildTitlePacket(
        packet,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "znv legacy ToPacket SubPayload");
    if (!packet.HasValue()) {
        return failures;
    }
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build znv legacy title");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain(payload), "build znv legacy title value");
    }
    return failures;
}

int TestFinalEmptyTail()
{
    const cg::packet::PacketResult<GuildTitle> parsed = GuildTitle::Parse(BuildRawPlain("1|1|2|Title|"));

    int failures = Expect(static_cast<bool>(parsed), "parse znv final empty tail");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 1U, "znv final empty tail count");
        const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
        failures += Expect(data.HasValue() && data.Value() == "1|1|2|Title|", "znv final tail BuildData");
    }
    failures += ExpectError(
        GuildTitle::Parse(BuildPlain("1|1|2|Title")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "znv",
        "znv nonempty records require final tail");
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
        GuildTitle::Parse(BuildPlain("1|x|2|Title|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "znv",
        "znv seq nondecimal");
    failures += ExpectError(
        GuildTitle::Parse(BuildPlain("1|1|x|Title|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "znv",
        "znv commition nondecimal");
    failures += ExpectFieldError(
        GuildTitle::Parse(BuildPlain("1|1||Title|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "znv",
        3U,
        "znv server clear title must be empty");
    failures += ExpectFieldError(
        GuildTitle::Parse(BuildPlain("1|1|-1|Title|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "znv",
        3U,
        "znv literal clear title must be empty");
    failures += ExpectFieldError(
        GuildTitle::Parse(BuildPlain("1|1|2|truncated\\|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "znv",
        3U,
        "znv truncated title legacy");

    const GuildTitle invalid({GuildTitleRecord {1, 2, Bytes({0x96})}});
    failures += ExpectFieldError(
        invalid.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "znv",
        3U,
        "znv truncated semantic GBK title");
    failures += ExpectFieldError(
        invalid.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "znv",
        3U,
        "znv ToPacket propagates title encode error");
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
    failures += TestLegacyTextRoundtrip();
    failures += TestFinalEmptyTail();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
