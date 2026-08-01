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

template <typename T>
int ExpectFieldError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, GuildMemberInfo::Code, message);
    if (!result.HasError()) {
        return failures;
    }

    failures += Expect(result.Error().fieldIndex.has_value(), message);
    if (result.Error().fieldIndex.has_value()) {
        failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
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

std::string SemanticCharName()
{
    return Bytes({
        0x81U, 0x7CU, 0x81U, 0x5CU,
        0x41U, 0x7CU, 0x42U, 0x2CU, 0x43U, 0x5CU, 0x44U, 0x0AU, 0x45U,
    });
}

std::string SemanticOwnTitle()
{
    return Bytes({
        0x82U, 0x7CU, 0x82U, 0x5CU,
        0x4CU, 0x65U, 0x61U, 0x64U, 0x65U, 0x72U,
    });
}

std::string EncodedNameTitle()
{
    return Bytes({
        0x81U, 0x7CU, 0x81U, 0x5CU,
        0x41U, 0x5CU, 0x79U, 0x7AU, 0x42U, 0x5CU, 0x79U, 0x63U,
        0x43U, 0x5CU, 0x79U, 0x79U, 0x44U, 0x5CU, 0x79U, 0x6EU, 0x45U,
        0x5CU, 0x7AU,
        0x82U, 0x7CU, 0x82U, 0x5CU,
        0x4CU, 0x65U, 0x61U, 0x64U, 0x65U, 0x72U,
    });
}

std::string SemanticPayload()
{
    std::string payload = "1|123|";
    payload.append(EncodedNameTitle());
    payload.append("|999|-45|1|987|2|");
    return payload;
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
    cg::packet::PacketFieldType dataType,
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
    failures += Expect(packet.fields[1].type == dataType, message);
    failures += Expect(packet.fields[1].value == payload, message);
    return failures;
}

int ExpectGuildMemberInfoValue(
    const GuildMemberInfo& info,
    int index,
    std::string_view charNameRaw,
    std::string_view ownTitleRaw,
    const char* message)
{
    int failures = 0;
    failures += Expect(info.index == index, message);
    failures += Expect(info.flag == 1, message);
    failures += Expect(info.registSeqNumber == 123, message);
    failures += Expect(info.charNameRaw == charNameRaw, message);
    failures += Expect(info.ownTitleRaw == ownTitleRaw, message);
    failures += Expect(info.registSeqNumberAgain == 999, message);
    failures += Expect(info.level == -45, message);
    failures += Expect(info.online == 1, message);
    failures += Expect(info.graphicsNo == 987, message);
    failures += Expect(info.toGuildTitleId == 2, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = SemanticPayload();
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlain(10, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Kka outer");
    if (parsed.HasValue()) {
        failures += ExpectGuildMemberInfoPacket(
            parsed.Value(),
            10,
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse Kka outer packet");
    }
    return failures;
}

int TestNestedLegacyRoundtrip()
{
    const std::string payload = SemanticPayload();
    const GuildMemberInfo info(
        10,
        1,
        123,
        SemanticCharName(),
        SemanticOwnTitle(),
        999,
        -45,
        1,
        987,
        2);

    const cg::packet::PacketResult<std::string> data = info.BuildData();
    int failures = Expect(data.HasValue(), "Kka nested BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "Kka nested BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = info.ToPacket();
    failures += Expect(static_cast<bool>(packet), "Kka ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectGuildMemberInfoPacket(
        packet.Value(),
        10,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "Kka ToPacket value");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build Kka packet");
    if (!built.HasValue()) {
        return failures;
    }
    failures += Expect(built.Value() == BuildPlain(10, payload), "build Kka plain");

    const cg::packet::PacketResult<GuildMemberInfo> parsed = GuildMemberInfo::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse Kka nested roundtrip");
    if (parsed.HasValue()) {
        failures += ExpectGuildMemberInfoValue(
            parsed.Value(),
            10,
            SemanticCharName(),
            SemanticOwnTitle(),
            "Kka nested roundtrip value");
    }
    return failures;
}

int TestEmptyTexts()
{
    const GuildMemberInfo info(10, 1, 123, "", "", 123, 45, 1, 987, 2);
    const cg::packet::PacketResult<std::string> data = info.BuildData();
    int failures = Expect(data.HasValue(), "Kka empty texts BuildData");
    if (!data.HasValue()) {
        return failures;
    }

    failures += Expect(data.Value() == "1|123|\\z|123|45|1|987|2|", "Kka empty texts payload");
    const cg::packet::PacketResult<GuildMemberInfo> parsed =
        GuildMemberInfo::Parse(BuildPlain(10, data.Value()));
    failures += Expect(parsed.HasValue(), "Kka empty texts Parse");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().charNameRaw.empty(), "Kka empty char name");
        failures += Expect(parsed.Value().ownTitleRaw.empty(), "Kka empty own title");
    }
    return failures;
}

int TestServerTitleAsymmetry()
{
    int failures = 0;

    const GuildMemberInfo escapedTitle(10, 1, 123, "Alice", "A\\zB", 123, 45, 1, 987, 2);
    const cg::packet::PacketResult<std::string> escapedData = escapedTitle.BuildData();
    failures += Expect(escapedData.HasValue(), "Kka server raw escaped title BuildData");
    if (escapedData.HasValue()) {
        const cg::packet::PacketResult<GuildMemberInfo> parsed =
            GuildMemberInfo::Parse(BuildPlain(10, escapedData.Value()));
        failures += Expect(parsed.HasValue(), "Kka server raw escaped title Parse");
        if (parsed.HasValue()) {
            failures += Expect(parsed.Value().ownTitleRaw == "A|B", "Kka title cg2d inner decode");
        }
    }

    const GuildMemberInfo ordinaryTitle(10, 1, 123, "Alice", "A/,B\nC", 123, 45, 1, 987, 2);
    const cg::packet::PacketResult<std::string> ordinaryData = ordinaryTitle.BuildData();
    failures += Expect(ordinaryData.HasValue(), "Kka ordinary raw title BuildData");
    if (ordinaryData.HasValue()) {
        const cg::packet::PacketResult<GuildMemberInfo> parsed =
            GuildMemberInfo::Parse(BuildPlain(10, ordinaryData.Value()));
        failures += Expect(parsed.HasValue(), "Kka ordinary raw title Parse");
        if (parsed.HasValue()) {
            failures += Expect(parsed.Value().ownTitleRaw == "A/,B\nC", "Kka ordinary title semantic");
        }
    }

    const GuildMemberInfo pipeTitle(10, 1, 123, "Alice", "A|B", 123, 45, 1, 987, 2);
    const cg::packet::PacketResult<std::string> pipeData = pipeTitle.BuildData();
    failures += Expect(pipeData.HasValue(), "Kka server raw pipe title BuildData");
    if (pipeData.HasValue()) {
        failures += ExpectFieldError(
            GuildMemberInfo::Parse(BuildPlain(10, pipeData.Value())),
            cg::packet::PacketErrorCode::FieldCountMismatch,
            2U,
            "Kka server raw pipe title is asymmetric");
    }

    const GuildMemberInfo backslashTitle(10, 1, 123, "Alice", "A\\", 123, 45, 1, 987, 2);
    const cg::packet::PacketResult<std::string> backslashData = backslashTitle.BuildData();
    failures += Expect(backslashData.HasValue(), "Kka server raw backslash title BuildData");
    if (backslashData.HasValue()) {
        failures += ExpectFieldError(
            GuildMemberInfo::Parse(BuildPlain(10, backslashData.Value())),
            cg::packet::PacketErrorCode::DecodeFailed,
            2U,
            "Kka server raw trailing backslash title is asymmetric");
    }
    return failures;
}

int TestFlagZeroAndNoTextLengthLimit()
{
    const std::string longName(17U, 'A');
    const GuildMemberInfo info(10, 0, 123, longName, "", 123, 45, 1, 987, 2);
    const cg::packet::PacketResult<std::string> data = info.BuildData();

    int failures = Expect(data.HasValue(), "Kka flag zero long name BuildData");
    if (!data.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<GuildMemberInfo> parsed =
        GuildMemberInfo::Parse(BuildPlain(10, data.Value()));
    failures += Expect(parsed.HasValue(), "Kka flag zero long name Parse");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().flag == 0, "Kka flag zero preserved");
        failures += Expect(parsed.Value().charNameRaw == longName, "Kka no text length limit");
    }
    return failures;
}

int TestSeqMismatchAllowed()
{
    const std::string payload = "1|123|Alice\\zLeader|999|45|1|987|2|";
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
        std::vector<std::string> fields = {
            "1", "123", "Alice\\zLeader", "123", "45", "1", "987", "2",
        };
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
    const std::string payload = "1|123|Alice\\zLeader|123|45|1|987|2|";
    const GuildMemberInfo valid(10, 1, 123, "Alice", "Leader", 123, 45, 1, 987, 2);
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
        cg::packet::PacketFieldType::Unknown,
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
    packet.fields[1].type = cg::packet::PacketFieldType::Unknown;
    failures += ExpectError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "Kka",
        "Kka manual bad data type");

    packet = builtPacket.Value();
    packet.fields[0].value = "";
    packet.fields[1].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Kka",
        "Kka manual empty index value");

    packet = builtPacket.Value();
    packet.fields[1].type = cg::packet::PacketFieldType::EscapedString;
    packet.fields[1].value = payload.substr(0, payload.size() - 1U);
    failures += ExpectError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Kka",
        "Kka manual bad payload");
    return failures;
}

int TestLegacyLayerErrors()
{
    int failures = 0;
    failures += ExpectFieldError(
        GuildMemberInfo(10, 1, 123, Bytes({0x81U}), "Leader", 123, 45, 1, 987, 2).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        2U,
        "Kka truncated GBK char name encode");
    failures += ExpectFieldError(
        GuildMemberInfo(10, 1, 123, "Alice", Bytes({0x81U}), 123, 45, 1, 987, 2).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        2U,
        "Kka truncated GBK raw title whole encode");

    const GuildMemberInfo valid(10, 1, 123, "Alice", "Leader", 123, 45, 1, 987, 2);
    const cg::packet::PacketResult<cg::packet::Packet> builtPacket = valid.ToPacket();
    failures += Expect(builtPacket.HasValue(), "Kka legacy errors valid packet");
    if (!builtPacket.HasValue()) {
        return failures;
    }

    cg::packet::Packet packet = builtPacket.Value();
    packet.fields[1].type = cg::packet::PacketFieldType::EscapedString;
    packet.fields[1].value = Bytes({0x81U});
    failures += ExpectFieldError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "Kka truncated GBK outer payload decode");

    packet = builtPacket.Value();
    packet.fields[1].type = cg::packet::PacketFieldType::EscapedString;
    packet.fields[1].value = "1|123|Alice\\|123|45|1|987|2|";
    failures += ExpectFieldError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::DecodeFailed,
        2U,
        "Kka truncated whole legacy escape");

    packet.fields[1].value = "1|123|Alice\\y\\zLeader|123|45|1|987|2|";
    failures += ExpectFieldError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::DecodeFailed,
        2U,
        "Kka truncated char name inner escape");

    packet.fields[1].value = "1|123|Alice\\zLeader\\y|123|45|1|987|2|";
    failures += ExpectFieldError(
        GuildMemberInfo::FromPacket(packet),
        cg::packet::PacketErrorCode::DecodeFailed,
        2U,
        "Kka truncated title inner escape");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestNestedLegacyRoundtrip();
    failures += TestEmptyTexts();
    failures += TestServerTitleAsymmetry();
    failures += TestFlagZeroAndNoTextLengthLimit();
    failures += TestSeqMismatchAllowed();
    failures += TestPayloadErrors();
    failures += TestOuterErrors();
    failures += TestManualPacketErrors();
    failures += TestLegacyLayerErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
