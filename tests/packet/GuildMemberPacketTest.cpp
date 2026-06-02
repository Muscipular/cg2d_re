#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::GuildMember::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::GuildMember::Spec);
}

using GuildMember = cg::packet::receive::GuildMember;
using GuildMemberRecord = cg::packet::receive::GuildMemberRecord;

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
        return "yuD ";
    }

    std::string plain(GuildMember::Code);
    plain.push_back(' ');
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

std::string BuildRawPlain(std::string_view payload)
{
    std::string plain(GuildMember::Code);
    plain.push_back(' ');
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

int ExpectGuildMemberPacket(
    const cg::packet::Packet& packet,
    std::string_view payload,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(GuildMember::Code), message);
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
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildRawPlain("1|100|Alice|101|30|1|2000|2"));

    int failures = Expect(static_cast<bool>(parsed), "parse yuD outer");
    if (parsed.HasValue()) {
        failures += ExpectGuildMemberPacket(parsed.Value(), "1|100|Alice|101|30|1|2000|2", "parse yuD outer packet");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::PacketResult<GuildMember> parsed = GuildMember::Parse("yuD  ");

    int failures = Expect(static_cast<bool>(parsed), "parse yuD empty data");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.empty(), "yuD empty records");
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue(), "yuD empty BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value().empty(), "yuD empty BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "yuD empty ToPacket");
    if (packet.HasValue()) {
        failures += ExpectGuildMemberPacket(packet.Value(), "", "yuD empty packet");
    }
    return failures;
}

int TestOneAndTwoRecords()
{
    int failures = 0;
    const std::string onePayload = "1|100|Alice|101|30|1|2000|2";
    const cg::packet::PacketResult<GuildMember> one = GuildMember::Parse(BuildPlain(onePayload));
    failures += Expect(static_cast<bool>(one), "parse yuD one record");
    if (one.HasValue()) {
        failures += Expect(one.Value().records.size() == 1U, "yuD one count");
        if (one.Value().records.size() == 1U) {
            const GuildMemberRecord& record = one.Value().records[0];
            failures += Expect(record.flag == 1, "yuD one flag");
            failures += Expect(record.registSeqNumber == 100, "yuD one seq");
            failures += Expect(record.nameTitleRaw == "Alice", "yuD one nameTitleRaw");
            failures += Expect(record.registSeqNumber2 == 101, "yuD one seq2");
            failures += Expect(record.level == 30, "yuD one level");
            failures += Expect(record.online == 1, "yuD one online");
            failures += Expect(record.graphicsNo == 2000, "yuD one graphicsNo");
            failures += Expect(record.toGuildTitleId == 2, "yuD one toGuildTitleId");
        }
        const cg::packet::PacketResult<std::string> data = one.Value().BuildData();
        failures += Expect(data.HasValue() && data.Value() == onePayload, "yuD one BuildData");
    }

    const std::vector<GuildMemberRecord> records {
        {1, 100, "Alice", 101, 30, 1, 2000, 2},
        {2, 200, "Bob", 201, 40, 0, 3000, 3},
    };
    const GuildMember guildMember(records);
    const std::string twoPayload = "1|100|Alice|101|30|1|2000|2|2|200|Bob|201|40|0|3000|3";
    const cg::packet::PacketResult<std::string> data = guildMember.BuildData();
    failures += Expect(data.HasValue() && data.Value() == twoPayload, "yuD two BuildData");

    const cg::packet::PacketResult<cg::packet::Packet> packet = guildMember.ToPacket();
    failures += Expect(static_cast<bool>(packet), "yuD two ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectGuildMemberPacket(packet.Value(), twoPayload, "yuD two packet");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build yuD two packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain(twoPayload), "build yuD two value");
    }
    return failures;
}

int TestFinalEmptyTail()
{
    const cg::packet::PacketResult<GuildMember> parsed =
        GuildMember::Parse(BuildRawPlain("1|100|Alice|101|30|1|2000|2|"));

    int failures = Expect(static_cast<bool>(parsed), "parse yuD final empty tail");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "yuD final empty tail count");
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue() && data.Value() == "1|100|Alice|101|30|1|2000|2", "yuD final empty tail BuildData");
    return failures;
}

int TestRawEscapePreserved()
{
    const std::string payload = "1|100|A\\zB\\cC\\yD\\nE|101|30|1|2000|2";
    const cg::packet::PacketResult<GuildMember> parsed = GuildMember::Parse(BuildRawPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse yuD raw escape");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "yuD raw escape count");
    if (parsed.Value().records.size() == 1U) {
        failures += Expect(
            parsed.Value().records[0].nameTitleRaw == "A\\zB\\cC\\yD\\nE",
            "yuD raw escape nameTitleRaw");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "yuD raw escape ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build yuD raw escape");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == "yuD 1|100|A\\\\zB\\\\cC\\\\yD\\\\nE|101|30|1|2000|2 ",
            "build yuD raw escaped value");
    }
    return failures;
}

int TestSeqMismatchAllowed()
{
    const cg::packet::PacketResult<GuildMember> parsed =
        GuildMember::Parse(BuildPlain("1|100|Alice|999|30|1|2000|2"));

    int failures = Expect(static_cast<bool>(parsed), "parse yuD seq mismatch");
    if (parsed.HasValue() && parsed.Value().records.size() == 1U) {
        failures += Expect(parsed.Value().records[0].registSeqNumber == 100, "yuD seq mismatch seq1");
        failures += Expect(parsed.Value().records[0].registSeqNumber2 == 999, "yuD seq mismatch seq2");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        GuildMember::Parse(BuildPlain("0|100|Alice|101|30|1|2000|2")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "yuD",
        "yuD flag zero invalid");
    failures += ExpectError(
        GuildMember::Parse(BuildPlain("1|100|Alice|101|30|1|2000")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "yuD",
        "yuD missing field");
    failures += ExpectError(
        GuildMember::Parse(BuildPlain("1|100|Alice|101|30|1|2000|2|extra")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "yuD",
        "yuD extra field");
    failures += ExpectError(
        GuildMember::Parse(BuildPlain("1|100|Alice|101|x|1|2000|2")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "yuD",
        "yuD nondecimal number");
    failures += ExpectError(
        GuildMember::Parse("znv 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "znv",
        "yuD wrong code");

    const GuildMember invalid(std::vector<GuildMemberRecord> {{0, 100, "Alice", 101, 30, 1, 2000, 2}});
    failures += ExpectError(
        invalid.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "yuD",
        "yuD build flag zero invalid");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyData();
    failures += TestOneAndTwoRecords();
    failures += TestFinalEmptyTail();
    failures += TestRawEscapePreserved();
    failures += TestSeqMismatchAllowed();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
