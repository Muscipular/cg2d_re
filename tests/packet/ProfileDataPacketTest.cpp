#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::receive::ProfileData::Spec;
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
    const cg::packet::Packet packet = parsed.Value().ToPacket();
    failures += ExpectProfileDataPacket(packet, "typed DpSY ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build DpSY");
    if (built.HasValue()) {
        failures += Expect(built.Value() == plain, "build DpSY value");
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

    const cg::packet::PacketResult<std::string> built = BuildPacket(data.ToPacket());
    int failures = Expect(static_cast<bool>(built), "build DpSY escaped fields");
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
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
