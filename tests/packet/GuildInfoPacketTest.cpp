#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::GuildInfo::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::GuildInfo::Spec);
}

using GuildInfo = cg::packet::receive::GuildInfo;

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
        return "aDn ";
    }

    std::string plain(GuildInfo::Code);
    plain.push_back(' ');
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

std::string BuildRawPlain(std::string_view payload)
{
    std::string plain(GuildInfo::Code);
    plain.push_back(' ');
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

std::string EncodeFlag(int flag)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeInt62(flag);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

int ExpectGuildInfoPacket(
    const cg::packet::Packet& packet,
    std::string_view payload,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(GuildInfo::Code), message);
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

int ExpectFullGuildInfo(const GuildInfo& guildInfo, const char* message)
{
    int failures = 0;
    failures += Expect(guildInfo.flag == GuildInfo::AllFieldsFlag, message);
    failures += Expect(guildInfo.guildId.has_value() && guildInfo.guildId.value() == 123, message);
    failures += Expect(guildInfo.guildNameRaw.has_value() && guildInfo.guildNameRaw.value() == "Guild", message);
    failures += Expect(guildInfo.roomNameRaw.has_value() && guildInfo.roomNameRaw.value() == "Room", message);
    failures += Expect(guildInfo.titleId.has_value() && guildInfo.titleId.value() == 5, message);
    failures += Expect(guildInfo.lastUpdate.has_value() && guildInfo.lastUpdate.value() == 6, message);
    failures += Expect(guildInfo.sortType.has_value() && guildInfo.sortType.value() == 7, message);
    failures += Expect(guildInfo.serverNo.has_value() && guildInfo.serverNo.value() == 8, message);
    failures += Expect(guildInfo.commition.has_value() && guildInfo.commition.value() == 9, message);
    failures += Expect(guildInfo.breeding0Raw.has_value() && guildInfo.breeding0Raw.value() == "Breed0", message);
    failures += Expect(guildInfo.breeding1Raw.has_value() && guildInfo.breeding1Raw.value() == "Breed1", message);
    failures += Expect(guildInfo.breeding2Raw.has_value() && guildInfo.breeding2Raw.value() == "Breed2", message);
    failures += Expect(guildInfo.registSeqNumber.has_value() && guildInfo.registSeqNumber.value() == 10, message);
    return failures;
}

GuildInfo MakeFullGuildInfo()
{
    GuildInfo guildInfo(GuildInfo::AllFieldsFlag);
    guildInfo.guildId = 123;
    guildInfo.guildNameRaw = "Guild";
    guildInfo.roomNameRaw = "Room";
    guildInfo.titleId = 5;
    guildInfo.lastUpdate = 6;
    guildInfo.sortType = 7;
    guildInfo.serverNo = 8;
    guildInfo.commition = 9;
    guildInfo.breeding0Raw = "Breed0";
    guildInfo.breeding1Raw = "Breed1";
    guildInfo.breeding2Raw = "Breed2";
    guildInfo.registSeqNumber = 10;
    return guildInfo;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlain("0|"));

    int failures = Expect(static_cast<bool>(parsed), "parse aDn outer");
    if (parsed.HasValue()) {
        failures += ExpectGuildInfoPacket(parsed.Value(), "0|", "parse aDn outer packet");
    }
    return failures;
}

int TestFullFieldsRoundtrip()
{
    const std::string payload = "1|123|Guild|Room|5|6|7|8|9|Breed0|Breed1|Breed2|10|";
    const cg::packet::PacketResult<GuildInfo> parsed = GuildInfo::Parse(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aDn full fields");
    if (parsed.HasValue()) {
        failures += ExpectFullGuildInfo(parsed.Value(), "aDn full values");
        const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
        failures += Expect(data.HasValue(), "aDn full BuildData");
        if (data.HasValue()) {
            failures += Expect(data.Value() == payload, "aDn full BuildData value");
        }
    }

    const GuildInfo guildInfo = MakeFullGuildInfo();
    const cg::packet::PacketResult<std::string> data = guildInfo.BuildData();
    failures += Expect(data.HasValue(), "aDn constructed full BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "aDn constructed full payload");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = guildInfo.ToPacket();
    failures += Expect(static_cast<bool>(packet), "aDn full ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectGuildInfoPacket(packet.Value(), payload, "aDn full packet");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build aDn full packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain(payload), "build aDn full plain");
    }
    return failures;
}

int TestSingleFieldsAndEmpty()
{
    int failures = 0;
    const cg::packet::PacketResult<GuildInfo> guildIdOnly = GuildInfo::Parse(BuildPlain("2|123|"));
    failures += Expect(static_cast<bool>(guildIdOnly), "parse aDn guildId only");
    if (guildIdOnly.HasValue()) {
        failures += Expect(guildIdOnly.Value().flag == GuildInfo::GuildIdFlag, "aDn guildId flag");
        failures += Expect(guildIdOnly.Value().guildId.has_value() && guildIdOnly.Value().guildId.value() == 123, "aDn guildId value");
        failures += Expect(!guildIdOnly.Value().guildNameRaw.has_value(), "aDn guildName absent");
        const cg::packet::PacketResult<std::string> data = guildIdOnly.Value().BuildData();
        failures += Expect(data.HasValue() && data.Value() == "2|123|", "aDn guildId BuildData");
    }

    const cg::packet::PacketResult<GuildInfo> guildNameOnly = GuildInfo::Parse(BuildPlain("4|Guild|"));
    failures += Expect(static_cast<bool>(guildNameOnly), "parse aDn guildName only");
    if (guildNameOnly.HasValue()) {
        failures += Expect(guildNameOnly.Value().flag == GuildInfo::GuildNameFlag, "aDn guildName flag");
        failures += Expect(guildNameOnly.Value().guildNameRaw.has_value() && guildNameOnly.Value().guildNameRaw.value() == "Guild", "aDn guildName value");
        const cg::packet::PacketResult<std::string> data = guildNameOnly.Value().BuildData();
        failures += Expect(data.HasValue() && data.Value() == "4|Guild|", "aDn guildName BuildData");
    }

    const cg::packet::PacketResult<GuildInfo> empty = GuildInfo::Parse(BuildPlain("0|"));
    failures += Expect(static_cast<bool>(empty), "parse aDn empty fields");
    if (empty.HasValue()) {
        failures += Expect(empty.Value().flag == 0, "aDn empty flag");
        failures += Expect(!empty.Value().guildId.has_value(), "aDn empty guildId absent");
        const cg::packet::PacketResult<std::string> data = empty.Value().BuildData();
        failures += Expect(data.HasValue() && data.Value() == "0|", "aDn empty BuildData");
    }
    return failures;
}

int TestGuildIdNegativeOne()
{
    const cg::packet::PacketResult<GuildInfo> parsed = GuildInfo::Parse(BuildPlain("2|-1|"));

    int failures = Expect(static_cast<bool>(parsed), "parse aDn guildId -1");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().guildId.has_value(), "aDn guildId -1 present");
        if (parsed.Value().guildId.has_value()) {
            failures += Expect(parsed.Value().guildId.value() == -1, "aDn guildId -1 value");
        }
        const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
        failures += Expect(data.HasValue() && data.Value() == "2|-1|", "aDn guildId -1 BuildData");
    }
    return failures;
}

int TestRawEscapePreserved()
{
    const std::string payload = "4|A\\zB\\cC\\yD\\nE|";
    const cg::packet::PacketResult<GuildInfo> parsed = GuildInfo::Parse(BuildRawPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aDn raw escape");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().guildNameRaw.has_value(), "aDn raw escape guildName present");
    if (parsed.Value().guildNameRaw.has_value()) {
        failures += Expect(parsed.Value().guildNameRaw.value() == "A\\zB\\cC\\yD\\nE", "aDn raw escape value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "aDn raw escape ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "aDn raw escape BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "aDn 4|A\\\\zB\\\\cC\\\\yD\\\\nE| ", "aDn raw escape BuildPacket value");
    }
    return failures;
}

int TestPayloadErrors()
{
    int failures = 0;
    failures += ExpectError(
        GuildInfo::Parse(BuildPlain("2|123")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aDn",
        "aDn missing tail");
    failures += ExpectError(
        GuildInfo::Parse(BuildPlain("_|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "aDn",
        "aDn bad flag");
    failures += ExpectError(
        GuildInfo::Parse(BuildPlain("2|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aDn",
        "aDn missing field");
    failures += ExpectError(
        GuildInfo::Parse(BuildPlain("2|123|extra|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aDn",
        "aDn extra field");
    failures += ExpectError(
        GuildInfo::Parse(BuildPlain("2|abc|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "aDn",
        "aDn non decimal");
    failures += ExpectError(
        GuildInfo::Parse(BuildPlain(EncodeFlag(0x2000) + "|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "aDn",
        "aDn unknown flag bit");
    return failures;
}

int TestOuterAndBuildErrors()
{
    int failures = 0;
    failures += ExpectError(
        GuildInfo::Parse("aDn"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aDn",
        "aDn missing outer data");
    failures += ExpectError(
        GuildInfo::Parse("vI 1 0| "),
        cg::packet::PacketErrorCode::InvalidCode,
        "vI",
        "aDn wrong code");

    GuildInfo missing(GuildInfo::GuildIdFlag);
    failures += ExpectError(
        missing.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "aDn",
        "aDn build missing selected field");

    GuildInfo unknown(0x2000);
    failures += ExpectError(
        unknown.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "aDn",
        "aDn build unknown flag");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestFullFieldsRoundtrip();
    failures += TestSingleFieldsAndEmpty();
    failures += TestGuildIdNegativeOne();
    failures += TestRawEscapePreserved();
    failures += TestPayloadErrors();
    failures += TestOuterAndBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
