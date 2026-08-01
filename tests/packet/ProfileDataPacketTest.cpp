#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::ProfileData::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ProfileData::Spec);
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
    std::size_t fieldIndex,
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
    failures += Expect(error.fieldIndex.has_value(), message);
    if (error.fieldIndex.has_value()) {
        failures += Expect(*error.fieldIndex == fieldIndex, message);
    }
    return failures;
}

using ProfileTextValues = std::array<std::string, 8U>;

constexpr std::array<std::size_t, 8U> ProfileTextFieldIndexes = {{
    0U, 2U, 3U, 4U, 7U, 9U, 11U, 12U,
}};

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string bytes;
    bytes.reserve(values.size());
    for (const unsigned int value : values) {
        bytes.push_back(static_cast<char>(value));
    }
    return bytes;
}

ProfileTextValues DefaultProfileTextValues()
{
    return {{"Hero", "Job", "Title", "Guild", "Sell", "Buy", "About", "Profile"}};
}

cg::packet::Packet BuildOuterProfilePacket(const ProfileTextValues& values)
{
    cg::packet::Packet packet(
        cg::packet::PacketCode(cg::packet::receive::ProfileData::Code),
        cg::packet::PacketDirection::ServerToClient);
    packet.fields = {
        {"name", cg::packet::PacketFieldType::EscapedString, values[0U]},
        {"lv", cg::packet::PacketFieldType::Int62, "10"},
        {"job", cg::packet::PacketFieldType::EscapedString, values[1U]},
        {"title", cg::packet::PacketFieldType::EscapedString, values[2U]},
        {"guildName", cg::packet::PacketFieldType::EscapedString, values[3U]},
        {"graphic", cg::packet::PacketFieldType::Int62, "1000"},
        {"sid", cg::packet::PacketFieldType::Int62, "1"},
        {"smsg", cg::packet::PacketFieldType::EscapedString, values[4U]},
        {"bid", cg::packet::PacketFieldType::Int62, "2"},
        {"bmsg", cg::packet::PacketFieldType::EscapedString, values[5U]},
        {"aid", cg::packet::PacketFieldType::Int62, "3"},
        {"amsg", cg::packet::PacketFieldType::EscapedString, values[6U]},
        {"pmsg", cg::packet::PacketFieldType::EscapedString, values[7U]},
    };
    return packet;
}

cg::packet::PacketResult<std::string> BuildOuterProfilePlain(const ProfileTextValues& values)
{
    return BuildPacket(BuildOuterProfilePacket(values));
}

cg::packet::receive::ProfileData BuildProfileDataValue(const ProfileTextValues& values)
{
    return cg::packet::receive::ProfileData(
        values[0U],
        10,
        values[1U],
        values[2U],
        values[3U],
        1000,
        1,
        values[4U],
        2,
        values[5U],
        3,
        values[6U],
        values[7U]);
}

std::string_view GetProfileTextValue(
    const cg::packet::receive::ProfileData& data,
    std::size_t index)
{
    const std::array<std::string_view, 8U> values = {{
        data.name,
        data.jobName,
        data.title,
        data.guildName,
        data.sellMessage,
        data.buyMessage,
        data.aboutMessage,
        data.profileMessage,
    }};
    return values[index];
}

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectProfileDataPacket(const cg::packet::Packet& packet, const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ProfileData::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 13U, message);
    if (packet.fields.size() != 13U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "name", cg::packet::PacketFieldType::EscapedString, "Hero", message);
    failures += ExpectField(packet, 1U, "lv", cg::packet::PacketFieldType::Int62, "10", message);
    failures += ExpectField(packet, 2U, "job", cg::packet::PacketFieldType::EscapedString, "Job", message);
    failures += ExpectField(packet, 3U, "title", cg::packet::PacketFieldType::EscapedString, "Title", message);
    failures += ExpectField(packet, 4U, "guildName", cg::packet::PacketFieldType::EscapedString, "Guild", message);
    failures += ExpectField(packet, 5U, "graphic", cg::packet::PacketFieldType::Int62, "1000", message);
    failures += ExpectField(packet, 6U, "sid", cg::packet::PacketFieldType::Int62, "1", message);
    failures += ExpectField(packet, 7U, "smsg", cg::packet::PacketFieldType::EscapedString, "Sell", message);
    failures += ExpectField(packet, 8U, "bid", cg::packet::PacketFieldType::Int62, "2", message);
    failures += ExpectField(packet, 9U, "bmsg", cg::packet::PacketFieldType::EscapedString, "Buy", message);
    failures += ExpectField(packet, 10U, "aid", cg::packet::PacketFieldType::Int62, "3", message);
    failures += ExpectField(packet, 11U, "amsg", cg::packet::PacketFieldType::EscapedString, "About", message);
    failures += ExpectField(packet, 12U, "pmsg", cg::packet::PacketFieldType::EscapedString, "Profile", message);
    return failures;
}

int ExpectProfileDataValue(const cg::packet::receive::ProfileData& data, const char* message)
{
    int failures = 0;
    failures += Expect(data.name == "Hero", message);
    failures += Expect(data.level == 10, message);
    failures += Expect(data.jobName == "Job", message);
    failures += Expect(data.title == "Title", message);
    failures += Expect(data.guildName == "Guild", message);
    failures += Expect(data.graphic == 1000, message);
    failures += Expect(data.sellFlag == 1, message);
    failures += Expect(data.sellMessage == "Sell", message);
    failures += Expect(data.buyFlag == 2, message);
    failures += Expect(data.buyMessage == "Buy", message);
    failures += Expect(data.aboutFlag == 3, message);
    failures += Expect(data.aboutMessage == "About", message);
    failures += Expect(data.profileMessage == "Profile", message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("DpSY Hero a Job Title Guild g8 1 Sell 2 Buy 3 About Profile ");

    int failures = Expect(static_cast<bool>(parsed), "parse DpSY outer");
    if (parsed.HasValue()) {
        failures += ExpectProfileDataPacket(parsed.Value(), "parse DpSY outer packet");
    }
    return failures;
}

int TestTypedParseAndBuild()
{
    const std::string plain = "DpSY Hero a Job Title Guild g8 1 Sell 2 Buy 3 About Profile ";
    const cg::packet::PacketResult<cg::packet::receive::ProfileData> parsed =
        cg::packet::receive::ProfileData::Parse(plain);

    int failures = Expect(static_cast<bool>(parsed), "typed parse DpSY");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectProfileDataValue(parsed.Value(), "typed DpSY value");
    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "typed DpSY ToPacket");
    if (packet.HasValue()) {
        failures += ExpectProfileDataPacket(packet.Value(), "typed DpSY ToPacket value");

        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "build DpSY");
        if (built.HasValue()) {
            failures += Expect(built.Value() == plain, "build DpSY value");
        }
    }
    return failures;
}

int TestEscapedFieldsRoundtrip()
{
    const cg::packet::receive::ProfileData data(
        "Hero Name",
        10,
        "Job\nName",
        "Title\\A",
        "Guild Name",
        1000,
        1,
        "Sell \\ message",
        2,
        "Buy\nmessage",
        3,
        "About message",
        "Profile \\ line\nnext");

    const cg::packet::PacketResult<cg::packet::Packet> packet = data.ToPacket();
    int failures = Expect(static_cast<bool>(packet), "DpSY escaped ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build DpSY escaped fields");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value().find('\n') == std::string::npos, "build DpSY escaped no newline");

    const cg::packet::PacketResult<cg::packet::receive::ProfileData> parsed =
        cg::packet::receive::ProfileData::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip DpSY escaped fields");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().name == data.name, "roundtrip DpSY name");
    failures += Expect(parsed.Value().jobName == data.jobName, "roundtrip DpSY job");
    failures += Expect(parsed.Value().title == data.title, "roundtrip DpSY title");
    failures += Expect(parsed.Value().guildName == data.guildName, "roundtrip DpSY guildName");
    failures += Expect(parsed.Value().sellMessage == data.sellMessage, "roundtrip DpSY smsg");
    failures += Expect(parsed.Value().buyMessage == data.buyMessage, "roundtrip DpSY bmsg");
    failures += Expect(parsed.Value().aboutMessage == data.aboutMessage, "roundtrip DpSY amsg");
    failures += Expect(parsed.Value().profileMessage == data.profileMessage, "roundtrip DpSY pmsg");
    return failures;
}

int TestLegacyTextFields()
{
    const std::string gbkEast = Bytes({0x96U, 0x7cU});
    std::string legacy = gbkEast;
    legacy.append("\\nline\\ccomma\\zpipe\\yslash\\qunknown outer space");
    std::string semantic = gbkEast;
    semantic.append("\nline,comma|pipe\\slashqunknown outer space");
    std::string canonical = gbkEast;
    canonical.append("\\nline\\ccomma\\zpipe\\yslashqunknown outer space");

    int failures = 0;
    for (std::size_t textIndex = 0; textIndex < ProfileTextFieldIndexes.size(); ++textIndex) {
        ProfileTextValues inputValues = DefaultProfileTextValues();
        inputValues[textIndex] = legacy;
        const cg::packet::PacketResult<std::string> plain = BuildOuterProfilePlain(inputValues);
        failures += Expect(static_cast<bool>(plain), "build DpSY legacy outer input");
        if (!plain.HasValue()) {
            continue;
        }

        failures += Expect(
            plain.Value().find("\\S") != std::string::npos,
            "DpSY legacy outer space encoded");
        const cg::packet::PacketResult<cg::packet::receive::ProfileData> parsed =
            cg::packet::receive::ProfileData::Parse(plain.Value());
        failures += Expect(static_cast<bool>(parsed), "parse DpSY legacy text field");
        if (!parsed.HasValue()) {
            continue;
        }

        failures += Expect(
            GetProfileTextValue(parsed.Value(), textIndex) == semantic,
            "DpSY legacy semantic text field");
        const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
        failures += Expect(static_cast<bool>(packet), "DpSY legacy canonical ToPacket");
        if (!packet.HasValue()) {
            continue;
        }

        failures += Expect(
            packet.Value().fields[ProfileTextFieldIndexes[textIndex]].value == canonical,
            "DpSY legacy canonical text field");
        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "build DpSY legacy canonical packet");

        ProfileTextValues canonicalValues = DefaultProfileTextValues();
        canonicalValues[textIndex] = canonical;
        const cg::packet::PacketResult<std::string> expected = BuildOuterProfilePlain(canonicalValues);
        failures += Expect(static_cast<bool>(expected), "build expected DpSY legacy canonical packet");
        if (built.HasValue() && expected.HasValue()) {
            failures += Expect(
                built.Value() == expected.Value(),
                "DpSY unknown legacy escape canonicalized");
        }
    }
    return failures;
}

int TestEmptyTitle()
{
    ProfileTextValues values = DefaultProfileTextValues();
    values[2U].clear();
    const cg::packet::receive::ProfileData data = BuildProfileDataValue(values);
    const cg::packet::PacketResult<cg::packet::Packet> packet = data.ToPacket();

    int failures = Expect(static_cast<bool>(packet), "DpSY empty title ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += Expect(packet.Value().fields[3U].value.empty(), "DpSY empty title packet field");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build DpSY empty title");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::ProfileData> parsed =
        cg::packet::receive::ProfileData::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse DpSY empty title");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().title.empty(), "DpSY empty title semantic value");
    }
    return failures;
}

int TestLegacyTextErrors()
{
    int failures = 0;
    for (std::size_t textIndex = 0; textIndex < ProfileTextFieldIndexes.size(); ++textIndex) {
        ProfileTextValues values = DefaultProfileTextValues();
        values[textIndex] = "truncated\\";
        const cg::packet::PacketResult<std::string> plain = BuildOuterProfilePlain(values);
        failures += Expect(static_cast<bool>(plain), "build DpSY truncated legacy input");
        if (plain.HasValue()) {
            failures += ExpectError(
                cg::packet::receive::ProfileData::Parse(plain.Value()),
                cg::packet::PacketErrorCode::DecodeFailed,
                "DpSY",
                ProfileTextFieldIndexes[textIndex],
                "DpSY trailing legacy escape context");
        }
    }

    const std::string truncatedGbk(1U, static_cast<char>(0x81));
    for (std::size_t textIndex = 0; textIndex < ProfileTextFieldIndexes.size(); ++textIndex) {
        ProfileTextValues values = DefaultProfileTextValues();
        values[textIndex] = truncatedGbk;
        failures += ExpectError(
            BuildProfileDataValue(values).ToPacket(),
            cg::packet::PacketErrorCode::EncodeFailed,
            "DpSY",
            ProfileTextFieldIndexes[textIndex],
            "DpSY truncated GBK encode context");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::ProfileData::Parse("DpSY Hero a Job Title Guild g8 1 Sell 2 Buy 3 About"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "DpSY",
        12U,
        "typed DpSY missing pmsg");
    failures += ExpectError(
        cg::packet::receive::ProfileData::Parse("DpSY Hero a Job Title Guild g8 1 Sell 2 Buy 3 About Profile extra"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "DpSY",
        13U,
        "typed DpSY extra field");
    failures += ExpectError(
        cg::packet::receive::ProfileData::Parse("DpSY Hero _ Job Title Guild g8 1 Sell 2 Buy 3 About Profile "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "DpSY",
        1U,
        "typed DpSY bad lv");
    failures += ExpectError(
        cg::packet::receive::ProfileData::Parse("DpSY Hero a Job Title Guild _ 1 Sell 2 Buy 3 About Profile "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "DpSY",
        5U,
        "typed DpSY bad graphic");
    failures += ExpectError(
        cg::packet::receive::ProfileData::Parse("DpSY Hero a Job Title Guild g8 _ Sell 2 Buy 3 About Profile "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "DpSY",
        6U,
        "typed DpSY bad sid");
    failures += ExpectError(
        cg::packet::receive::ProfileData::Parse("DpSY Hero a Job Title Guild g8 1 Sell _ Buy 3 About Profile "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "DpSY",
        8U,
        "typed DpSY bad bid");
    failures += ExpectError(
        cg::packet::receive::ProfileData::Parse("DpSY Hero a Job Title Guild g8 1 Sell 2 Buy _ About Profile "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "DpSY",
        10U,
        "typed DpSY bad aid");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedParseAndBuild();
    failures += TestEscapedFieldsRoundtrip();
    failures += TestLegacyTextFields();
    failures += TestEmptyTitle();
    failures += TestLegacyTextErrors();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
