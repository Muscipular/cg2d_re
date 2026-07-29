#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using ProfileWindow = cg::packet::receive::ProfileWindow;
using ProfileWindowListRecord = cg::packet::receive::ProfileWindowListRecord;
using ProfileWindowPayloadKind = cg::packet::receive::ProfileWindowPayloadKind;
using ProfileWindowProfileList = cg::packet::receive::ProfileWindowProfileList;
using ProfileWindowProfileMail = cg::packet::receive::ProfileWindowProfileMail;

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, ProfileWindow::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, ProfileWindow::Spec);
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
int ExpectErrorAt(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
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
        failures += Expect(error.packetCode->View() == ProfileWindow::Code, message);
    }
    failures += Expect(error.fieldIndex.has_value(), message);
    if (error.fieldIndex.has_value()) {
        failures += Expect(error.fieldIndex.value() == fieldIndex, message);
    }
    return failures;
}

std::string EncodeI62(int value)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeInt62(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string EncodeLegacy(std::string_view value)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeLegacyPayloadText(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string JoinPipe(const std::vector<std::string>& parts)
{
    const cg::packet::PacketResult<std::string> payload =
        cg::packet::BuildPayload(std::span<const std::string>(parts.data(), parts.size()), "|");
    return payload.HasValue() ? payload.Value() : std::string();
}

std::string EncodeWholePayload(const std::vector<std::string>& parts)
{
    return EncodeLegacy(JoinPipe(parts));
}

std::string BuildPlainDyag(
    int windowType,
    int buttonType,
    int seqNo,
    int objectIndex,
    std::string_view data)
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
    if (encodedData.HasValue()) {
        plain.append(encodedData.Value());
    }
    plain.push_back(' ');
    return plain;
}

std::string BuildRawDataTokenDyag(std::string_view dataToken)
{
    std::string plain = "DyAG ";
    plain.append(EncodeI62(38));
    plain.append(" 0 0 0 ");
    plain.append(dataToken);
    plain.push_back(' ');
    return plain;
}

std::vector<std::string> MakeProfileListParts(std::size_t activeCount)
{
    std::vector<std::string> parts;
    parts.reserve(83U);
    parts.push_back("7");
    parts.push_back("2");
    for (std::size_t index = 0; index < 20U; ++index) {
        if (index >= activeCount) {
            parts.push_back("-1");
            parts.emplace_back();
            parts.emplace_back();
            parts.emplace_back();
            continue;
        }

        parts.push_back(std::to_string(100 + static_cast<int>(index)));
        parts.push_back(std::to_string(200 + static_cast<int>(index)));
        parts.push_back(std::string("Name") + std::to_string(index));
        parts.push_back(std::string("Message") + std::to_string(index));
    }
    parts.emplace_back();
    return parts;
}

std::vector<std::string> MakeProfileMailParts()
{
    return {
        "10",
        "Hero",
        "25",
        "Warrior",
        "Title",
        "Guild",
        "1001",
        "1",
        "Sell",
        "2",
        "Buy",
        "3",
        "About",
        "Profile",
    };
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    int windowType,
    int buttonType,
    std::string_view data,
    cg::packet::PacketFieldType dataType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(ProfileWindow::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 5U, message);
    if (packet.fields.size() != 5U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "windowType", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(windowType), message);
    failures += Expect(packet.fields[1].name == "buttonType", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[1].value == std::to_string(buttonType), message);
    failures += Expect(packet.fields[2].name == "seqNo", message);
    failures += Expect(packet.fields[2].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[2].value == "3", message);
    failures += Expect(packet.fields[3].name == "objectIndex", message);
    failures += Expect(packet.fields[3].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[3].value == "-1", message);
    failures += Expect(packet.fields[4].name == "data", message);
    failures += Expect(packet.fields[4].type == dataType, message);
    failures += Expect(packet.fields[4].value == data, message);
    return failures;
}

int TestGenericOuterRoundtrip()
{
    const std::string data = EncodeLegacy("0|-2|3|");
    const std::string plain = BuildPlainDyag(38, 3, 3, -1, data);
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(plain);

    int failures = Expect(static_cast<bool>(parsed), "parse DyAG generic outer");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectPacket(
        parsed.Value(),
        38,
        3,
        data,
        cg::packet::PacketFieldType::EscapedString,
        "parse DyAG generic outer fields");
    const cg::packet::PacketResult<std::string> rebuilt = BuildPacket(parsed.Value());
    failures += Expect(static_cast<bool>(rebuilt), "build DyAG generic outer");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == plain, "roundtrip DyAG generic outer");
    }
    return failures;
}

int TestStartPayload()
{
    int failures = 0;
    const cg::packet::PacketResult<ProfileWindow> empty =
        ProfileWindow::Parse(BuildPlainDyag(38, 3, 3, -1, ""));
    failures += Expect(static_cast<bool>(empty), "parse DyAG START empty");
    if (empty.HasValue()) {
        failures += Expect(empty.Value().flags.empty(), "DyAG START empty flags");
        const cg::packet::PacketResult<std::string> data = empty.Value().BuildData();
        failures += Expect(static_cast<bool>(data), "build DyAG START empty");
        if (data.HasValue()) {
            failures += Expect(data.Value().empty(), "DyAG START empty canonical");
        }
    }

    const std::string data = EncodeLegacy("0|-2|3|");
    const cg::packet::PacketResult<ProfileWindow> parsed =
        ProfileWindow::Parse(BuildPlainDyag(38, 3, 3, -1, data));
    failures += Expect(static_cast<bool>(parsed), "parse DyAG START dynamic");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().windowType == 38, "DyAG START wire windowType");
    failures += Expect(parsed.Value().payloadKind == ProfileWindowPayloadKind::Start, "DyAG START kind");
    failures += Expect(parsed.Value().flags == std::vector<int>({0, -2, 3}), "DyAG START dynamic flags");
    const cg::packet::PacketResult<std::string> rebuiltData = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuiltData), "build DyAG START dynamic");
    if (rebuiltData.HasValue()) {
        failures += Expect(rebuiltData.Value() == data, "roundtrip DyAG START data");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "DyAG START ToPacket");
    if (packet.HasValue()) {
        failures += ExpectPacket(
            packet.Value(),
            38,
            3,
            data,
            cg::packet::PacketFieldType::SubPayload,
            "DyAG START ToPacket fields");
    }
    return failures;
}

int TestProfileListCounts()
{
    int failures = 0;
    const std::vector<std::size_t> activeCounts = {0U, 3U, 20U};
    const std::vector<int> buttonTypes = {3, 19, 35};
    for (std::size_t caseIndex = 0; caseIndex < activeCounts.size(); ++caseIndex) {
        const std::size_t activeCount = activeCounts[caseIndex];
        const std::string data = EncodeWholePayload(MakeProfileListParts(activeCount));
        const cg::packet::PacketResult<ProfileWindow> parsed = ProfileWindow::Parse(
            BuildPlainDyag(39, buttonTypes[caseIndex], 3, -1, data));
        failures += Expect(static_cast<bool>(parsed), "parse DyAG PROFILELIST active count");
        if (!parsed.HasValue()) {
            continue;
        }

        failures += Expect(
            parsed.Value().payloadKind == ProfileWindowPayloadKind::ProfileList,
            "DyAG PROFILELIST kind");
        failures += Expect(parsed.Value().profileList.categoryId == 7, "DyAG PROFILELIST categoryId");
        failures += Expect(parsed.Value().profileList.categoryIndex == 2, "DyAG PROFILELIST categoryIndex");
        for (std::size_t index = 0; index < 20U; ++index) {
            failures += Expect(
                parsed.Value().profileList.records[index].empty == (index >= activeCount),
                "DyAG PROFILELIST empty slots");
        }

        const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
        failures += Expect(static_cast<bool>(rebuilt), "build DyAG PROFILELIST active count");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == data, "roundtrip DyAG PROFILELIST active count");
        }
    }
    return failures;
}

int TestProfileListLegacyAndGbk()
{
    const std::string legacyInner = "line\\ncomma\\cpipe\\zslash\\yunknown\\q";
    const std::string canonicalInner = "line\\ncomma\\cpipe\\zslash\\yunknownq";
    const std::string semantic = "line\ncomma,pipe|slash\\unknownq";
    std::string gbk;
    gbk.push_back(static_cast<char>(0x81));
    gbk.push_back(static_cast<char>(0x7c));

    std::vector<std::string> legacyParts = MakeProfileListParts(1U);
    legacyParts[4] = legacyInner;
    legacyParts[5] = gbk;
    const std::string legacyData = EncodeWholePayload(legacyParts);
    const cg::packet::PacketResult<ProfileWindow> parsed =
        ProfileWindow::Parse(BuildPlainDyag(39, 19, 3, -1, legacyData));

    int failures = Expect(static_cast<bool>(parsed), "parse DyAG PROFILELIST two-layer legacy");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().profileList.records[0].name == semantic, "decode DyAG n/c/z/y unknown");
    failures += Expect(parsed.Value().profileList.records[0].message == gbk, "decode DyAG GBK 81 7C");

    std::vector<std::string> canonicalParts = legacyParts;
    canonicalParts[4] = canonicalInner;
    const std::string canonicalData = EncodeWholePayload(canonicalParts);
    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build DyAG PROFILELIST canonical");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == canonicalData, "canonicalize DyAG two-layer legacy");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "DyAG PROFILELIST ToPacket");
    if (packet.HasValue()) {
        failures += ExpectPacket(
            packet.Value(),
            39,
            19,
            canonicalData,
            cg::packet::PacketFieldType::SubPayload,
            "DyAG PROFILELIST canonical ToPacket");
    }
    return failures;
}

int TestProfileMailPayload()
{
    std::vector<std::string> parts = MakeProfileMailParts();
    parts[1] = EncodeLegacy("Hero\n");
    parts[13] = EncodeLegacy("Profile|Text");
    const std::string data = EncodeWholePayload(parts);
    const cg::packet::PacketResult<ProfileWindow> parsed =
        ProfileWindow::Parse(BuildPlainDyag(40, 51, 3, -1, data));

    int failures = Expect(static_cast<bool>(parsed), "parse DyAG PROFILEMAIL");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().payloadKind == ProfileWindowPayloadKind::ProfileMail, "DyAG PROFILEMAIL kind");
    failures += Expect(parsed.Value().profileMail.seekPos == 10, "DyAG PROFILEMAIL seekPos");
    failures += Expect(parsed.Value().profileMail.name == "Hero\n", "DyAG PROFILEMAIL name");
    failures += Expect(parsed.Value().profileMail.level == 25, "DyAG PROFILEMAIL level");
    failures += Expect(parsed.Value().profileMail.guildName == "Guild", "DyAG PROFILEMAIL guild");
    failures += Expect(parsed.Value().profileMail.buyMessage == "Buy", "DyAG PROFILEMAIL buy");
    failures += Expect(parsed.Value().profileMail.profileMessage == "Profile|Text", "DyAG PROFILEMAIL profile");

    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build DyAG PROFILEMAIL");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == data, "roundtrip DyAG PROFILEMAIL without tail");
    }
    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "DyAG PROFILEMAIL ToPacket");
    if (packet.HasValue()) {
        failures += ExpectPacket(
            packet.Value(),
            40,
            51,
            data,
            cg::packet::PacketFieldType::SubPayload,
            "DyAG PROFILEMAIL ToPacket fields");
    }
    return failures;
}

int TestShapeAndSentinelErrors()
{
    int failures = 0;
    for (const int uiWindowType : {60, 61, 62}) {
        failures += ExpectErrorAt(
            ProfileWindow::Parse(BuildPlainDyag(uiWindowType, 3, 3, -1, EncodeLegacy("0|"))),
            cg::packet::PacketErrorCode::DecodeFailed,
            0U,
            "reject DyAG UI windowType");
    }
    failures += ExpectErrorAt(
        ProfileWindow::Parse(BuildPlainDyag(38, 3, 3, -1, EncodeLegacy("0|-2"))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        4U,
        "reject DyAG START missing tail");
    failures += ExpectErrorAt(
        ProfileWindow::Parse(BuildPlainDyag(38, 3, 3, -1, EncodeLegacy("+1|"))),
        cg::packet::PacketErrorCode::DecodeFailed,
        4U,
        "reject DyAG START non-raw signed decimal");

    std::vector<std::string> missingTail = MakeProfileListParts(1U);
    missingTail.pop_back();
    failures += ExpectErrorAt(
        ProfileWindow::Parse(BuildPlainDyag(39, 19, 3, -1, EncodeWholePayload(missingTail))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        4U,
        "reject DyAG PROFILELIST missing tail");

    std::vector<std::string> malformedSentinel = MakeProfileListParts(0U);
    malformedSentinel[3] = "1";
    failures += ExpectErrorAt(
        ProfileWindow::Parse(BuildPlainDyag(39, 19, 3, -1, EncodeWholePayload(malformedSentinel))),
        cg::packet::PacketErrorCode::DecodeFailed,
        4U,
        "reject DyAG malformed sentinel");

    std::vector<std::string> activeAfterSentinel = MakeProfileListParts(0U);
    activeAfterSentinel[6] = "101";
    activeAfterSentinel[7] = "201";
    activeAfterSentinel[8] = "Later";
    activeAfterSentinel[9] = "Active";
    failures += ExpectErrorAt(
        ProfileWindow::Parse(BuildPlainDyag(39, 19, 3, -1, EncodeWholePayload(activeAfterSentinel))),
        cg::packet::PacketErrorCode::DecodeFailed,
        4U,
        "reject DyAG active after sentinel");

    std::vector<std::string> mailWithTail = MakeProfileMailParts();
    mailWithTail.emplace_back();
    failures += ExpectErrorAt(
        ProfileWindow::Parse(BuildPlainDyag(40, 51, 3, -1, EncodeWholePayload(mailWithTail))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        4U,
        "reject DyAG PROFILEMAIL trailing delimiter");
    return failures;
}

int TestTruncatedParseErrors()
{
    int failures = 0;
    failures += ExpectErrorAt(
        ProfileWindow::Parse(BuildPlainDyag(38, 3, 3, -1, "\\")),
        cg::packet::PacketErrorCode::DecodeFailed,
        4U,
        "reject DyAG truncated whole legacy escape");

    std::vector<std::string> innerLegacyParts = MakeProfileListParts(1U);
    innerLegacyParts[4] = "Name\\";
    failures += ExpectErrorAt(
        ProfileWindow::Parse(BuildPlainDyag(39, 19, 3, -1, EncodeWholePayload(innerLegacyParts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        4U,
        "reject DyAG truncated inner legacy escape");

    std::string truncatedGbk(1U, static_cast<char>(0x81));
    failures += ExpectErrorAt(
        ProfileWindow::Parse(BuildRawDataTokenDyag(truncatedGbk)),
        cg::packet::PacketErrorCode::DecodeFailed,
        4U,
        "reject DyAG truncated GBK");
    return failures;
}

int TestBuildErrors()
{
    ProfileWindowProfileList laterActive;
    laterActive.records[1].empty = false;
    laterActive.records[1].seekPos = 1;
    laterActive.records[1].graphics = 2;
    const ProfileWindow badSentinelOrder(39, 19, 3, -1, std::move(laterActive));

    std::string truncatedGbk(1U, static_cast<char>(0x81));
    ProfileWindowProfileList badText;
    badText.records[0].empty = false;
    badText.records[0].seekPos = 1;
    badText.records[0].graphics = 2;
    badText.records[0].name = std::move(truncatedGbk);
    const ProfileWindow badGbk(39, 19, 3, -1, std::move(badText));

    const ProfileWindow kindMismatch(39, 19, 3, -1, std::vector<int> {1});
    int failures = 0;
    failures += ExpectErrorAt(
        badSentinelOrder.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        4U,
        "DyAG BuildData rejects active after sentinel");
    failures += ExpectErrorAt(
        badGbk.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        4U,
        "DyAG BuildData truncated GBK context");
    failures += ExpectErrorAt(
        badGbk.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        4U,
        "DyAG ToPacket truncated GBK context");
    failures += ExpectErrorAt(
        kindMismatch.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "DyAG BuildData kind mismatch context");
    failures += ExpectErrorAt(
        kindMismatch.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "DyAG ToPacket kind mismatch context");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestGenericOuterRoundtrip();
    failures += TestStartPayload();
    failures += TestProfileListCounts();
    failures += TestProfileListLegacyAndGbk();
    failures += TestProfileMailPayload();
    failures += TestShapeAndSentinelErrors();
    failures += TestTruncatedParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
