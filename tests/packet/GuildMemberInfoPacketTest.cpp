#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <array>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::GuildMemberInfo::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::GuildMemberInfo::Spec);
}

using GuildMemberInfo = cg::packet::receive::GuildMemberInfo;

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

std::string BuildPlain(int index, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedIndex = cg::packet::EncodeInt62(index);
    const cg::packet::PacketResult<std::string> encodedData = cg::packet::EncodeEscapedString(payload);
    if (!encodedIndex.HasValue() || !encodedData.HasValue()) {
        return "Kka ";
    }

    std::string plain(GuildMemberInfo::Code);
    plain.push_back(' ');
    plain.append(encodedIndex.Value());
    plain.push_back(' ');
    plain.append(encodedData.Value());
    plain.push_back(' ');
    return plain;
}

std::string BuildRawPlain(std::string_view indexToken, std::string_view payload)
{
    std::string plain(GuildMemberInfo::Code);
    plain.push_back(' ');
    plain.append(indexToken);
    plain.push_back(' ');
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

std::string JoinPayload(const std::vector<std::string>& fields)
{
    std::string payload;
    for (const std::string& field : fields) {
        payload.append(field);
        payload.push_back('|');
    }
    return payload;
}

int ExpectGuildMemberInfoPacket(
    const cg::packet::Packet& packet,
    int index,
    std::string_view payload,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(GuildMemberInfo::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "index", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(index), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].value == payload, message);
    return failures;
}

int ExpectGuildMemberInfoValue(
    const GuildMemberInfo& info,
    int index,
    std::string_view nameTitleRaw,
    const char* message)
{
    int failures = 0;
    failures += Expect(info.index == index, message);
    failures += Expect(info.flag == 1, message);
    failures += Expect(info.registSeqNumber == 123, message);
    failures += Expect(info.nameTitleRaw == nameTitleRaw, message);
    failures += Expect(info.registSeqNumberAgain == 123, message);
    failures += Expect(info.level == 45, message);
    failures += Expect(info.online == 1, message);
    failures += Expect(info.graphicsNo == 987, message);
    failures += Expect(info.toGuildTitleId == 2, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "1|123|Alice\\zLeader|123|45|1|987|2|";
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(BuildRawPlain("a", payload), cg::packet::receive::GuildMemberInfo::Spec);

    int failures = Expect(static_cast<bool>(parsed), "parse Kka outer");
    if (parsed.HasValue()) {
        failures += ExpectGuildMemberInfoPacket(parsed.Value(), 10, payload, "parse Kka outer packet");
    }
    return failures;
}

int TestRoundtrip()
{
    const std::string payload = "1|123|Alice\\zLeader|123|45|1|987|2|";
    const cg::packet::PacketResult<GuildMemberInfo> parsed = GuildMemberInfo::Parse(BuildPlain(10, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Kka roundtrip");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectGuildMemberInfoValue(parsed.Value(), 10, "Alice\\zLeader", "Kka roundtrip value");

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue(), "Kka BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "Kka BuildData keeps tail");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "Kka ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectGuildMemberInfoPacket(packet.Value(), 10, payload, "Kka ToPacket value");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build Kka packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain(10, payload), "build Kka plain");
    }
    return failures;
}

int TestRawEscapePreserved()
{
    const std::string payload = "1|123|A\\zB\\cC\\yD\\nE|123|45|1|987|2|";
    const cg::packet::PacketResult<GuildMemberInfo> parsed = GuildMemberInfo::Parse(BuildRawPlain("a", payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Kka raw escape");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectGuildMemberInfoValue(parsed.Value(), 10, "A\\zB\\cC\\yD\\nE", "Kka raw escape value");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "Kka raw escape ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "Kka raw escape BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain(10, payload), "Kka raw escape BuildPacket value");
    }
    return failures;
}

int TestSeqMismatchAllowed()
{
    const std::string payload = "1|123|Alice|999|45|1|987|2|";
    const cg::packet::PacketResult<GuildMemberInfo> parsed = GuildMemberInfo::Parse(BuildPlain(10, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Kka seq mismatch");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().index == 10, "Kka seq mismatch index");
        failures += Expect(parsed.Value().registSeqNumber == 123, "Kka seq mismatch seq");
        failures += Expect(parsed.Value().registSeqNumberAgain == 999, "Kka seq mismatch again");
    }
    return failures;
}

int TestPayloadErrors()
{
    int failures = 0;
    failures += ExpectError(
        GuildMemberInfo::Parse(BuildRawPlain("a", "")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Kka",
        "Kka empty data invalid");
    failures += ExpectError(
        GuildMemberInfo::Parse(BuildPlain(10, "1|123|Alice|123|45|1|987|2")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Kka",
        "Kka missing final tail");
    failures += ExpectError(
        GuildMemberInfo::Parse(BuildPlain(10, "1|123|Alice|123|45|1|987|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Kka",
        "Kka seven fields invalid");
    failures += ExpectError(
        GuildMemberInfo::Parse(BuildPlain(10, "1|123|Alice|123|45|1|987|2|extra|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Kka",
        "Kka extra fields invalid");

    const std::array<std::size_t, 7U> numericIndexes = {0U, 1U, 3U, 4U, 5U, 6U, 7U};
    for (const std::size_t numericIndex : numericIndexes) {
        std::vector<std::string> fields = {"1", "123", "Alice", "123", "45", "1", "987", "2"};
        fields[numericIndex] = "x";
        failures += ExpectError(
            GuildMemberInfo::Parse(BuildPlain(10, JoinPayload(fields))),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Kka",
            "Kka payload nondecimal");
    }
    return failures;
}

int TestOuterErrors()
{
    int failures = 0;
    const std::string payload = "1|123|Alice|123|45|1|987|2|";
    failures += ExpectError(
        GuildMemberInfo::Parse("Kka a"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Kka",
        "Kka missing data field");
    failures += ExpectError(
        GuildMemberInfo::Parse("Kka a 1|123|Alice|123|45|1|987|2| extra"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Kka",
        "Kka extra outer field");
    failures += ExpectError(
        GuildMemberInfo::Parse(BuildRawPlain("!", payload)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Kka",
        "Kka invalid index i62");
    failures += ExpectError(
        GuildMemberInfo::Parse("yuD 1|123|Alice|123|45|1|987|2| "),
        cg::packet::PacketErrorCode::InvalidCode,
        "yuD",
        "Kka wrong code");
    return failures;
}

int TestManualPacketErrors()
{
    int failures = 0;
    const std::string payload = "1|123|Alice|123|45|1|987|2|";
    const GuildMemberInfo valid(10, 1, 123, "Alice", 123, 45, 1, 987, 2);
    const cg::packet::PacketResult<cg::packet::Packet> builtPacket = valid.ToPacket();
    failures += Expect(builtPacket.HasValue(), "Kka valid manual packet");
    if (!builtPacket.HasValue()) {
        return failures;
    }

    cg::packet::Packet packet = builtPacket.Value();
    packet.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Kka",
        "Kka manual wrong direction");

    packet = builtPacket.Value();
    packet.fields.pop_back();
    failures += ExpectError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Kka",
        "Kka manual missing field");

    packet = builtPacket.Value();
    packet.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::RawString,
        "",
    });
    failures += ExpectError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Kka",
        "Kka manual extra field");

    packet = builtPacket.Value();
    packet.fields[0].name = "registry";
    failures += ExpectError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Kka",
        "Kka manual bad index name");

    packet = builtPacket.Value();
    packet.fields[1].type = cg::packet::PacketFieldType::RawString;
    failures += ExpectError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "Kka",
        "Kka manual bad data type");

    packet = builtPacket.Value();
    packet.fields[0].value = "";
    failures += ExpectError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Kka",
        "Kka manual empty index value");

    packet = builtPacket.Value();
    packet.fields[1].value = payload.substr(0, payload.size() - 1U);
    failures += ExpectError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Kka",
        "Kka manual bad payload");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestRoundtrip();
    failures += TestRawEscapePreserved();
    failures += TestSeqMismatchAllowed();
    failures += TestPayloadErrors();
    failures += TestOuterErrors();
    failures += TestManualPacketErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
