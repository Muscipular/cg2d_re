#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <initializer_list>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

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

std::string SemanticText(std::initializer_list<unsigned int> prefix, std::string_view suffix)
{
    std::string value = Bytes(prefix);
    value.append(suffix);
    return value;
}

std::string EncodedText(std::initializer_list<unsigned int> prefix, std::string_view suffix)
{
    std::string value = Bytes(prefix);
    value.append(suffix);
    return value;
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

std::string EncodeFlag(int flag)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeInt62(flag);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string JoinPayload(const std::vector<std::string>& parts)
{
    const cg::packet::PacketResult<std::string> payload = cg::packet::BuildPayload(
        std::span<const std::string>(parts.data(), parts.size()),
        GuildInfo::PayloadDelimiter);
    return payload.HasValue() ? payload.Value() : std::string();
}

int ExpectGuildInfoPacket(
    const cg::packet::Packet& packet,
    std::string_view payload,
    cg::packet::PacketFieldType payloadType,
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
    failures += Expect(packet.fields[0].type == payloadType, message);
    failures += Expect(packet.fields[0].value == payload, message);
    return failures;
}

int ExpectGuildInfoPacket(
    const cg::packet::PacketResult<cg::packet::Packet>& packet,
    std::string_view payload,
    cg::packet::PacketFieldType payloadType,
    const char* message)
{
    int failures = Expect(static_cast<bool>(packet), message);
    if (packet.HasValue()) {
        failures += ExpectGuildInfoPacket(packet.Value(), payload, payloadType, message);
    }
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
        failures += ExpectGuildInfoPacket(
            parsed.Value(),
            "0|",
            cg::packet::PacketFieldType::EscapedString,
            "parse aDn outer packet");
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
    failures += ExpectGuildInfoPacket(
        packet.Value(),
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "aDn full packet");

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

int TestLegacyTextRoundtrip()
{
    const std::string guildName = SemanticText({0xC3, 0xFB, 0x96, 0x7C}, "A|B,C\\D\nE");
    const std::string roomName = SemanticText({0xB7, 0xBF, 0x96, 0x5C}, "F\nG,H|I\\J");
    const std::string breeding0 = SemanticText({0xD1, 0xF8, 0x96, 0x7C}, "K|L,M\\N\nO");
    const std::string breeding1 = SemanticText({0xD6, 0xB3, 0x96, 0x5C}, "P\nQ,R|S\\T");
    const std::string breeding2 = SemanticText({0xC9, 0xFA, 0x96, 0x7C}, "U|V,W\\X\nY");
    const std::string encodedGuildName = EncodedText({0xC3, 0xFB, 0x96, 0x7C}, "A\\zB\\cC\\yD\\nE");
    const std::string encodedRoomName = EncodedText({0xB7, 0xBF, 0x96, 0x5C}, "F\\nG\\cH\\zI\\yJ");
    const std::string encodedBreeding0 = EncodedText({0xD1, 0xF8, 0x96, 0x7C}, "K\\zL\\cM\\yN\\nO");
    const std::string encodedBreeding1 = EncodedText({0xD6, 0xB3, 0x96, 0x5C}, "P\\nQ\\cR\\zS\\yT");
    const std::string encodedBreeding2 = EncodedText({0xC9, 0xFA, 0x96, 0x7C}, "U\\zV\\cW\\yX\\nY");

    std::string payload = "1|123|";
    payload.append(encodedGuildName);
    payload.push_back('|');
    payload.append(encodedRoomName);
    payload.append("|5|6|7|8|9|");
    payload.append(encodedBreeding0);
    payload.push_back('|');
    payload.append(encodedBreeding1);
    payload.push_back('|');
    payload.append(encodedBreeding2);
    payload.append("|10|");

    const cg::packet::PacketResult<GuildInfo> parsed = GuildInfo::Parse(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aDn five legacy text fields");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(
        parsed.Value().guildNameRaw.has_value() && parsed.Value().guildNameRaw.value() == guildName,
        "aDn guildName semantic value");
    failures += Expect(
        parsed.Value().roomNameRaw.has_value() && parsed.Value().roomNameRaw.value() == roomName,
        "aDn roomName semantic value");
    failures += Expect(
        parsed.Value().breeding0Raw.has_value() && parsed.Value().breeding0Raw.value() == breeding0,
        "aDn breeding0 semantic value");
    failures += Expect(
        parsed.Value().breeding1Raw.has_value() && parsed.Value().breeding1Raw.value() == breeding1,
        "aDn breeding1 semantic value");
    failures += Expect(
        parsed.Value().breeding2Raw.has_value() && parsed.Value().breeding2Raw.value() == breeding2,
        "aDn breeding2 semantic value");

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue(), "aDn five legacy BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "aDn five legacy canonical payload");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += ExpectGuildInfoPacket(
        packet,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "aDn five legacy ToPacket SubPayload");
    if (!packet.HasValue()) {
        return failures;
    }
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "aDn five legacy BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain(payload), "aDn five legacy BuildPacket value");
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

int TestLegacyTextErrors()
{
    int failures = 0;
    const std::vector<std::string> validParts = {
        "1", "123", "Guild", "Room", "5", "6", "7", "8", "9", "Breed0", "Breed1", "Breed2", "10", "",
    };
    const std::array<std::size_t, 5U> textFieldIndices = {2U, 3U, 9U, 10U, 11U};

    for (const std::size_t fieldIndex : textFieldIndices) {
        std::vector<std::string> parts = validParts;
        parts[fieldIndex] = "truncated\\";
        failures += ExpectFieldError(
            GuildInfo::Parse(BuildPlain(JoinPayload(parts))),
            cg::packet::PacketErrorCode::DecodeFailed,
            "aDn",
            fieldIndex,
            "aDn truncated inner legacy text");
    }

    for (const std::size_t fieldIndex : textFieldIndices) {
        GuildInfo invalid = MakeFullGuildInfo();
        if (fieldIndex == 2U) {
            invalid.guildNameRaw = Bytes({0x96});
        } else if (fieldIndex == 3U) {
            invalid.roomNameRaw = Bytes({0x96});
        } else if (fieldIndex == 9U) {
            invalid.breeding0Raw = Bytes({0x96});
        } else if (fieldIndex == 10U) {
            invalid.breeding1Raw = Bytes({0x96});
        } else {
            invalid.breeding2Raw = Bytes({0x96});
        }

        failures += ExpectFieldError(
            invalid.BuildData(),
            cg::packet::PacketErrorCode::EncodeFailed,
            "aDn",
            fieldIndex,
            "aDn truncated semantic GBK text");
    }

    GuildInfo invalidPacket = MakeFullGuildInfo();
    invalidPacket.guildNameRaw = Bytes({0x96});
    failures += ExpectFieldError(
        invalidPacket.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "aDn",
        2U,
        "aDn ToPacket propagates text encode error");
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
    failures += TestLegacyTextRoundtrip();
    failures += TestPayloadErrors();
    failures += TestLegacyTextErrors();
    failures += TestOuterAndBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
