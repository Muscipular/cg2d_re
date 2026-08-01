#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::ItemRecipe::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ItemRecipe::Spec);
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
int ExpectErrorAt(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, "xmgQ", message);
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
    std::string bytes;
    bytes.reserve(values.size());
    for (const unsigned int value : values) {
        bytes.push_back(static_cast<char>(value));
    }
    return bytes;
}

std::string BuildPlainXmgQ(int haveSkillIndex, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedHaveSkillIndex = cg::packet::EncodeInt62(haveSkillIndex);
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedHaveSkillIndex.HasValue() || !encodedPayload.HasValue()) {
        return "xmgQ  ";
    }

    std::string plain = "xmgQ ";
    plain.append(encodedHaveSkillIndex.Value());
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

std::vector<std::string> BuildRecipeParts()
{
    return {
        "100",
        "3",
        "200",
        "Recipe\\zItem",
        "Info\\nLine",
        "15",
        "1",
        "300",
        "Mat0\\cName",
        "2",
        "301",
        "Mat1\\yName",
        "3",
        "302",
        "Mat2",
        "4",
        "303",
        "Mat3",
        "5",
        "304",
        "Mat4",
        "6",
    };
}

cg::packet::receive::ItemRecipeRecord BuildSemanticRecipeRecord()
{
    cg::packet::receive::ItemRecipeRecord record;
    record.recipeId = 100;
    record.itemLevel = 3;
    record.recipeItemId = 200;
    record.itemNameRaw = "Recipe|Item";
    record.itemInfoRaw = "Info\nLine";
    record.forcePoint = 15;
    record.usable = 1;
    record.materials = {{
        {300, "Mat0,Name", 2},
        {301, "Mat1\\Name", 3},
        {302, "Mat2", 4},
        {303, "Mat3", 5},
        {304, "Mat4", 6},
    }};
    return record;
}

std::string JoinTrailing(const std::vector<std::string>& parts)
{
    std::string payload;
    for (const std::string& part : parts) {
        payload.append(part);
        payload.push_back('|');
    }
    return payload;
}

std::string BuildOneRecipePayload()
{
    return JoinTrailing(BuildRecipeParts());
}

int ExpectItemRecipePacket(
    const cg::packet::Packet& packet,
    int haveSkillIndex,
    std::string_view value,
    cg::packet::PacketFieldType dataFieldType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ItemRecipe::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "haveskillindex", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(haveSkillIndex), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == dataFieldType, message);
    failures += Expect(packet.fields[1].value == value, message);
    return failures;
}

int ExpectMaterial(
    const cg::packet::receive::ItemRecipeMaterial& material,
    int itemId,
    std::string_view nameRaw,
    int required,
    const char* message)
{
    int failures = 0;
    failures += Expect(material.itemId == itemId, message);
    failures += Expect(material.nameRaw == nameRaw, message);
    failures += Expect(material.required == required, message);
    return failures;
}

int ExpectFirstRecipe(const cg::packet::receive::ItemRecipeRecord& record, const char* message)
{
    int failures = 0;
    failures += Expect(record.recipeId == 100, message);
    failures += Expect(record.itemLevel == 3, message);
    failures += Expect(record.recipeItemId == 200, message);
    failures += Expect(record.itemNameRaw == "Recipe|Item", message);
    failures += Expect(record.itemInfoRaw == "Info\nLine", message);
    failures += Expect(record.forcePoint == 15, message);
    failures += Expect(record.usable == 1, message);
    failures += ExpectMaterial(record.materials[0], 300, "Mat0,Name", 2, message);
    failures += ExpectMaterial(record.materials[1], 301, "Mat1\\Name", 3, message);
    failures += ExpectMaterial(record.materials[2], 302, "Mat2", 4, message);
    failures += ExpectMaterial(record.materials[3], 303, "Mat3", 5, message);
    failures += ExpectMaterial(record.materials[4], 304, "Mat4", 6, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = BuildOneRecipePayload();
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainXmgQ(62, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse xmgQ outer");
    if (parsed.HasValue()) {
        failures += ExpectItemRecipePacket(
            parsed.Value(),
            62,
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse xmgQ outer packet");
    }
    failures += Expect(
        cg::packet::receive::ItemRecipe::Fields[1].type == cg::packet::PacketFieldType::SubPayload,
        "xmgQ data spec is SubPayload");
    return failures;
}

int TestSingleRecordRoundtrip()
{
    const std::string payload = BuildOneRecipePayload();
    const cg::packet::PacketResult<cg::packet::receive::ItemRecipe> parsed =
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(5, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse xmgQ single record");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().haveSkillIndex == 5, "xmgQ haveSkillIndex");
    failures += Expect(parsed.Value().records.size() == 1U, "xmgQ record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectFirstRecipe(parsed.Value().records[0], "xmgQ first record");
    }
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "xmgQ BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "xmgQ BuildData value");
        failures += Expect(data.Value().back() == '|', "xmgQ BuildData trailing delimiter");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "xmgQ ToPacket");
    if (packet.HasValue()) {
        failures += ExpectItemRecipePacket(
            packet.Value(),
            5,
            payload,
            cg::packet::PacketFieldType::SubPayload,
            "xmgQ ToPacket value");

        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "build xmgQ packet");
        if (built.HasValue()) {
            failures += Expect(built.Value() == BuildPlainXmgQ(5, payload), "xmgQ packet roundtrip");
        }
    }
    return failures;
}

int TestMultipleRecords()
{
    std::vector<std::string> second = BuildRecipeParts();
    second[0] = "101";
    second[6] = "0";
    const std::string payload = BuildOneRecipePayload() + JoinTrailing(second);

    const cg::packet::PacketResult<cg::packet::receive::ItemRecipe> parsed =
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(8, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse xmgQ multiple records");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 2U, "xmgQ multiple record count");
    if (parsed.Value().records.size() == 2U) {
        failures += Expect(parsed.Value().records[0].recipeId == 100, "xmgQ first recipe id");
        failures += Expect(parsed.Value().records[1].recipeId == 101, "xmgQ second recipe id");
        failures += Expect(parsed.Value().records[1].usable == 0, "xmgQ second usable");
    }
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "xmgQ multiple BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "xmgQ multiple BuildData value");
    }
    return failures;
}

int TestLegacyTextCanonical()
{
    const std::string gbkEast = Bytes({0x96U, 0x7cU});
    std::string legacyText = gbkEast;
    legacyText.append("\\nline\\ccomma\\zpipe\\yslash\\q");
    std::string semanticText = gbkEast;
    semanticText.append("\nline,comma|pipe\\slashq");
    std::string canonicalText = gbkEast;
    canonicalText.append("\\nline\\ccomma\\zpipe\\yslashq");
    const std::vector<std::size_t> textIndexes = {3U, 4U, 8U, 11U, 14U, 17U, 20U};

    std::vector<std::string> parts = BuildRecipeParts();
    std::vector<std::string> canonicalParts = parts;
    for (const std::size_t index : textIndexes) {
        parts[index] = legacyText;
        canonicalParts[index] = canonicalText;
    }
    const std::string payload = JoinTrailing(parts);
    const std::string canonicalPayload = JoinTrailing(canonicalParts);

    const cg::packet::PacketResult<cg::packet::receive::ItemRecipe> parsed =
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(4, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse xmgQ legacy texts");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "xmgQ legacy record count");
    if (parsed.Value().records.size() == 1U) {
        const cg::packet::receive::ItemRecipeRecord& record = parsed.Value().records[0];
        failures += Expect(record.itemNameRaw == semanticText, "xmgQ legacy itemName semantic");
        failures += Expect(record.itemInfoRaw == semanticText, "xmgQ legacy itemInfo semantic");
        for (const cg::packet::receive::ItemRecipeMaterial& material : record.materials) {
            failures += Expect(material.nameRaw == semanticText, "xmgQ legacy material semantic");
        }
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "xmgQ legacy canonical BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonicalPayload, "xmgQ legacy canonical BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "xmgQ legacy canonical ToPacket");
    if (packet.HasValue()) {
        failures += ExpectItemRecipePacket(
            packet.Value(),
            4,
            canonicalPayload,
            cg::packet::PacketFieldType::SubPayload,
            "xmgQ legacy canonical ToPacket value");

        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "build xmgQ legacy canonical packet");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == BuildPlainXmgQ(4, canonicalPayload),
                "xmgQ legacy canonical packet value");
        }
    }
    return failures;
}

int TestSignedNumericsRoundtrip()
{
    std::vector<std::string> parts = BuildRecipeParts();
    parts[0] = "-1";
    parts[1] = "-2";
    parts[2] = "-3";
    parts[5] = "-4";
    parts[7] = "-5";
    parts[9] = "-6";
    const std::string payload = JoinTrailing(parts);
    const cg::packet::PacketResult<cg::packet::receive::ItemRecipe> parsed =
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(-7, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse xmgQ signed numerics");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().haveSkillIndex == -7, "xmgQ signed haveSkillIndex");
    failures += Expect(parsed.Value().records[0].recipeId == -1, "xmgQ signed recipeId");
    failures += Expect(parsed.Value().records[0].itemLevel == -2, "xmgQ signed itemLevel");
    failures += Expect(parsed.Value().records[0].recipeItemId == -3, "xmgQ signed recipeItemId");
    failures += Expect(parsed.Value().records[0].forcePoint == -4, "xmgQ signed forcePoint");
    failures += Expect(parsed.Value().records[0].materials[0].itemId == -5, "xmgQ signed material itemId");
    failures += Expect(parsed.Value().records[0].materials[0].required == -6, "xmgQ signed material required");
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "xmgQ signed BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "xmgQ signed BuildData value");
    }
    return failures;
}

int ExpectBadPayloadField(std::size_t index, std::string value, const char* message)
{
    std::vector<std::string> parts = BuildRecipeParts();
    parts[index] = std::move(value);
    return ExpectErrorAt(
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(1, JoinTrailing(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        index,
        message);
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::ItemRecipe::Parse("Rspn 1 data "),
        cg::packet::PacketErrorCode::InvalidCode,
        "Rspn",
        "xmgQ wrong code");
    failures += ExpectError(
        cg::packet::receive::ItemRecipe::Parse("xmgQ 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "xmgQ",
        "xmgQ missing outer field");
    failures += ExpectError(
        cg::packet::receive::ItemRecipe::Parse("xmgQ 1 data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "xmgQ",
        "xmgQ extra outer field");
    failures += ExpectError(
        cg::packet::receive::ItemRecipe::Parse("xmgQ _ data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "xmgQ",
        "xmgQ bad haveskillindex i62");
    failures += ExpectErrorAt(
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(1, "")),
        cg::packet::PacketErrorCode::EmptyInput,
        1U,
        "xmgQ empty data");

    std::string payload = BuildOneRecipePayload();
    payload.pop_back();
    failures += ExpectErrorAt(
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(1, payload)),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "xmgQ missing trailing delimiter");

    std::vector<std::string> parts = BuildRecipeParts();
    parts.pop_back();
    failures += ExpectErrorAt(
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(1, JoinTrailing(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        1U,
        "xmgQ 21 fields");

    parts = BuildRecipeParts();
    parts.push_back("extra");
    failures += ExpectErrorAt(
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(1, JoinTrailing(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        1U,
        "xmgQ 23 fields");

    std::string tooManyRecords;
    for (std::size_t index = 0; index < 57U; ++index) {
        tooManyRecords.append(BuildOneRecipePayload());
    }
    failures += ExpectErrorAt(
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(1, tooManyRecords)),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "xmgQ 57 records");

    failures += ExpectBadPayloadField(0U, "", "xmgQ empty recipeId");
    failures += ExpectBadPayloadField(0U, "x", "xmgQ bad recipeId");
    failures += ExpectBadPayloadField(1U, "x", "xmgQ bad itemLevel");
    failures += ExpectBadPayloadField(2U, "2147483648", "xmgQ overflow recipeItemId");
    failures += ExpectBadPayloadField(5U, "x", "xmgQ bad forcePoint");
    failures += ExpectBadPayloadField(9U, "x", "xmgQ bad material required");
    failures += ExpectBadPayloadField(6U, "2", "xmgQ usable out of range");
    for (const std::size_t index : {3U, 4U, 8U, 11U, 14U, 17U, 20U}) {
        failures += ExpectBadPayloadField(index, "Text\\", "xmgQ legacy text trailing backslash");
    }
    return failures;
}

int TestBuildAndToPacketErrors()
{
    int failures = 0;

    const cg::packet::receive::ItemRecipe empty(1, {});
    failures += ExpectErrorAt(
        empty.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        1U,
        "xmgQ BuildData zero records");

    std::vector<cg::packet::receive::ItemRecipeRecord> tooManyRecords(
        57U,
        BuildSemanticRecipeRecord());
    const cg::packet::receive::ItemRecipe tooMany(1, std::move(tooManyRecords));
    failures += ExpectErrorAt(
        tooMany.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        1U,
        "xmgQ BuildData 57 records");

    cg::packet::receive::ItemRecipeRecord badUsableRecord = BuildSemanticRecipeRecord();
    badUsableRecord.usable = 2;
    const cg::packet::receive::ItemRecipe badUsable(1, {std::move(badUsableRecord)});
    failures += ExpectErrorAt(
        badUsable.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        6U,
        "xmgQ BuildData usable out of range");

    const std::string truncatedGbk = Bytes({0x81U});
    cg::packet::receive::ItemRecipeRecord badItemNameRecord = BuildSemanticRecipeRecord();
    badItemNameRecord.itemNameRaw = truncatedGbk;
    const cg::packet::receive::ItemRecipe badItemName(1, {std::move(badItemNameRecord)});
    failures += ExpectErrorAt(
        badItemName.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        3U,
        "xmgQ BuildData item name truncated GBK context");

    cg::packet::receive::ItemRecipeRecord badItemInfoRecord = BuildSemanticRecipeRecord();
    badItemInfoRecord.itemInfoRaw = truncatedGbk;
    const cg::packet::receive::ItemRecipe badItemInfo(1, {std::move(badItemInfoRecord)});
    failures += ExpectErrorAt(
        badItemInfo.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        4U,
        "xmgQ BuildData item info truncated GBK context");

    std::vector<cg::packet::receive::ItemRecipeRecord> secondRecords = {
        BuildSemanticRecipeRecord(),
        BuildSemanticRecipeRecord(),
    };
    secondRecords[1].materials[4].nameRaw = truncatedGbk;
    const cg::packet::receive::ItemRecipe badSecondMaterial(1, std::move(secondRecords));
    failures += ExpectErrorAt(
        badSecondMaterial.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        42U,
        "xmgQ BuildData second record material truncated GBK context");
    failures += ExpectErrorAt(
        badSecondMaterial.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        42U,
        "xmgQ ToPacket second record material truncated GBK context");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestSingleRecordRoundtrip();
    failures += TestMultipleRecords();
    failures += TestSignedNumericsRoundtrip();
    failures += TestLegacyTextCanonical();
    failures += TestErrors();
    failures += TestBuildAndToPacketErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
