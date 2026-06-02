#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace {

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

std::string JoinZ(const std::vector<std::string>& parts)
{
    const cg::packet::PacketResult<std::string> payload =
        cg::packet::BuildPayload(std::span<const std::string>(parts.data(), parts.size()), "\\z");
    return payload.HasValue() ? payload.Value() : std::string();
}

std::string EncodeI62(int value)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeInt62(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string BuildPlainDyag(int windowType, int buttonType, int seqNo, int objectIndex, std::string_view data)
{
    const cg::packet::PacketResult<std::string> encodedData = cg::packet::EncodeEscapedString(data);
    std::string plain = "DyAG ";
    plain.append(EncodeI62(windowType));
    plain.push_back(' ');
    plain.append(EncodeI62(buttonType));
    plain.push_back(' ');
    plain.append(EncodeI62(seqNo));
    plain.push_back(' ');
    plain.append(EncodeI62(objectIndex));
    plain.push_back(' ');
    plain.append(encodedData.HasValue() ? encodedData.Value() : std::string());
    plain.push_back(' ');
    return plain;
}

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectProfileWindowPacket(const cg::packet::Packet& packet, std::string_view data, const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ProfileWindow::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 5U, message);
    if (packet.fields.size() != 5U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "windowType", cg::packet::PacketFieldType::Int62, "62", message);
    failures += ExpectField(packet, 1U, "buttonType", cg::packet::PacketFieldType::Int62, "2", message);
    failures += ExpectField(packet, 2U, "seqNo", cg::packet::PacketFieldType::Int62, "3", message);
    failures += ExpectField(packet, 3U, "objectIndex", cg::packet::PacketFieldType::Int62, "-1", message);
    failures += ExpectField(packet, 4U, "data", cg::packet::PacketFieldType::EscapedString, data, message);
    return failures;
}

std::string BuildProfileListData()
{
    std::vector<std::string> parts;
    parts.reserve(83U);
    parts.push_back("7");
    parts.push_back("2");
    for (int index = 0; index < 20; ++index) {
        if (index == 5) {
            parts.push_back("-1");
            parts.emplace_back();
            parts.emplace_back();
            parts.emplace_back();
            continue;
        }

        parts.push_back(std::to_string(100 + index));
        parts.push_back(std::to_string(200 + index));
        parts.push_back(std::string("Name") + std::to_string(index));
        parts.push_back(index == 3 ? "Message With Space" : std::string("Message") + std::to_string(index));
    }
    parts.emplace_back();
    return JoinZ(parts);
}

std::string BuildProfileMailData()
{
    return JoinZ({
        "10",
        "Hero",
        "25",
        "Warrior",
        "Title",
        "",
        "1001",
        "1",
        "",
        "2",
        "Buy text",
        "3",
        "",
        "Profile text",
    });
}

int TestParsePacketOuter()
{
    const std::string data = "0\\z3\\z1\\z";
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(BuildPlainDyag(62, 2, 3, -1, data));

    int failures = Expect(static_cast<bool>(parsed), "parse DyAG outer");
    if (parsed.HasValue()) {
        failures += ExpectProfileWindowPacket(parsed.Value(), data, "parse DyAG outer packet");
    }
    return failures;
}

int TestStartPayload()
{
    const std::string data = "0\\z3\\z1\\z";
    const cg::packet::PacketResult<cg::packet::receive::ProfileWindow> parsed =
        cg::packet::receive::ProfileWindow::Parse(BuildPlainDyag(62, 2, 3, -1, data));

    int failures = Expect(static_cast<bool>(parsed), "parse DyAG START");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().windowType == 62, "DyAG START windowType");
    failures += Expect(parsed.Value().payloadKind == cg::packet::receive::ProfileWindowPayloadKind::Start, "DyAG START kind");
    failures += Expect(parsed.Value().flags.size() == 3U, "DyAG START flags count");
    if (parsed.Value().flags.size() == 3U) {
        failures += Expect(parsed.Value().flags[0] == 0, "DyAG START flag 0");
        failures += Expect(parsed.Value().flags[1] == 3, "DyAG START flag 1");
        failures += Expect(parsed.Value().flags[2] == 1, "DyAG START flag 2");
    }
    failures += Expect(parsed.Value().BuildData() == data, "DyAG START BuildData");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build DyAG START");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainDyag(62, 2, 3, -1, data), "roundtrip DyAG START");
    }
    return failures;
}

int TestProfileListPayload()
{
    const std::string data = BuildProfileListData();
    const cg::packet::PacketResult<cg::packet::receive::ProfileWindow> parsed =
        cg::packet::receive::ProfileWindow::Parse(BuildPlainDyag(62, 2, 3, -1, data));

    int failures = Expect(static_cast<bool>(parsed), "parse DyAG PROFILELIST");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(
        parsed.Value().payloadKind == cg::packet::receive::ProfileWindowPayloadKind::ProfileList,
        "DyAG PROFILELIST kind");
    failures += Expect(parsed.Value().profileList.categoryId == 7, "DyAG PROFILELIST categoryId");
    failures += Expect(parsed.Value().profileList.categoryIndex == 2, "DyAG PROFILELIST categoryIndex");
    failures += Expect(parsed.Value().profileList.records[3].seekPos == 103, "DyAG PROFILELIST seekPos");
    failures += Expect(parsed.Value().profileList.records[3].graphics == 203, "DyAG PROFILELIST graphics");
    failures += Expect(parsed.Value().profileList.records[3].name == "Name3", "DyAG PROFILELIST name");
    failures += Expect(parsed.Value().profileList.records[3].message == "Message With Space", "DyAG PROFILELIST message");
    failures += Expect(parsed.Value().profileList.records[5].empty, "DyAG PROFILELIST empty slot");
    failures += Expect(parsed.Value().profileList.records[5].seekPos == -1, "DyAG PROFILELIST empty seekPos");
    failures += Expect(parsed.Value().profileList.records[5].name.empty(), "DyAG PROFILELIST empty name");
    failures += Expect(parsed.Value().profileList.records[5].message.empty(), "DyAG PROFILELIST empty message");
    failures += Expect(parsed.Value().BuildData() == data, "DyAG PROFILELIST BuildData");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build DyAG PROFILELIST");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainDyag(62, 2, 3, -1, data), "roundtrip DyAG PROFILELIST");
    }
    return failures;
}

int TestProfileMailPayload()
{
    const std::string data = BuildProfileMailData();
    const cg::packet::PacketResult<cg::packet::receive::ProfileWindow> parsed =
        cg::packet::receive::ProfileWindow::Parse(BuildPlainDyag(62, 2, 3, -1, data));

    int failures = Expect(static_cast<bool>(parsed), "parse DyAG PROFILEMAIL");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(
        parsed.Value().payloadKind == cg::packet::receive::ProfileWindowPayloadKind::ProfileMail,
        "DyAG PROFILEMAIL kind");
    failures += Expect(parsed.Value().profileMail.seekPos == 10, "DyAG PROFILEMAIL seekPos");
    failures += Expect(parsed.Value().profileMail.name == "Hero", "DyAG PROFILEMAIL name");
    failures += Expect(parsed.Value().profileMail.level == 25, "DyAG PROFILEMAIL level");
    failures += Expect(parsed.Value().profileMail.guildName.empty(), "DyAG PROFILEMAIL empty guild");
    failures += Expect(parsed.Value().profileMail.sellMessage.empty(), "DyAG PROFILEMAIL empty sell message");
    failures += Expect(parsed.Value().profileMail.buyMessage == "Buy text", "DyAG PROFILEMAIL buy message");
    failures += Expect(parsed.Value().profileMail.aboutMessage.empty(), "DyAG PROFILEMAIL empty about message");
    failures += Expect(parsed.Value().profileMail.profileMessage == "Profile text", "DyAG PROFILEMAIL profile message");
    failures += Expect(parsed.Value().BuildData() == data, "DyAG PROFILEMAIL BuildData");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build DyAG PROFILEMAIL");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainDyag(62, 2, 3, -1, data), "roundtrip DyAG PROFILEMAIL");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::ProfileWindow::Parse("DyAG 1 2 3 4"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "DyAG",
        "DyAG missing outer data");
    failures += ExpectError(
        cg::packet::receive::ProfileWindow::Parse("DyAG _ 2 3 4 data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "DyAG",
        "DyAG bad outer i62");
    failures += ExpectError(
        cg::packet::receive::ProfileWindow::Parse(BuildPlainDyag(
            62,
            2,
            3,
            -1,
            JoinZ({"10", "Hero", "25", "Warrior", "Title", "", "1001", "1", "", "2", "Buy", "3", ""}))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "DyAG",
        "DyAG PROFILEMAIL not 14 fields");
    failures += ExpectError(
        cg::packet::receive::ProfileWindow::Parse(BuildPlainDyag(
            62,
            2,
            3,
            -1,
            JoinZ({"7", "2", "100", "200", "Name", "Message", ""}))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "DyAG",
        "DyAG PROFILELIST invalid record count");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestStartPayload();
    failures += TestProfileListPayload();
    failures += TestProfileMailPayload();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
