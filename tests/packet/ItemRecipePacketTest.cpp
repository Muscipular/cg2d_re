#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
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
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
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
    failures += Expect(record.itemNameRaw == "Recipe\\zItem", message);
    failures += Expect(record.itemInfoRaw == "Info\\nLine", message);
    failures += Expect(record.forcePoint == 15, message);
    failures += Expect(record.usable == 1, message);
    failures += ExpectMaterial(record.materials[0], 300, "Mat0\\cName", 2, message);
    failures += ExpectMaterial(record.materials[1], 301, "Mat1\\yName", 3, message);
    failures += ExpectMaterial(record.materials[2], 302, "Mat2", 4, message);
    failures += ExpectMaterial(record.materials[3], 303, "Mat3", 5, message);
    failures += ExpectMaterial(record.materials[4], 304, "Mat4", 6, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = BuildOneRecipePayload();
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(BuildPlainXmgQ(62, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse xmgQ outer");
    if (parsed.HasValue()) {
        failures += ExpectItemRecipePacket(parsed.Value(), 62, payload, "parse xmgQ outer packet");
    }
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
    failures += Expect(parsed.Value().BuildData() == payload, "xmgQ BuildData");
    failures += ExpectItemRecipePacket(parsed.Value().ToPacket(), 5, payload, "xmgQ ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build xmgQ packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainXmgQ(5, payload), "xmgQ packet roundtrip");
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
    failures += Expect(parsed.Value().BuildData() == payload, "xmgQ multiple BuildData");
    return failures;
}

int TestRawInnerEscapesPreserved()
{
    const std::string rawZ = "A\\zB";
    const std::string rawC = "A\\cB";
    const std::string rawY = "A\\yB";
    const std::string rawN = "A\\nB";
    std::vector<std::string> parts = BuildRecipeParts();
    parts[3] = rawZ;
    parts[4] = rawC;
    parts[8] = rawY;
    parts[11] = rawN;
    const std::string payload = JoinTrailing(parts);

    const cg::packet::PacketResult<cg::packet::receive::ItemRecipe> parsed =
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(4, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse xmgQ raw escapes");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "xmgQ raw record count");
    if (parsed.Value().records.size() == 1U) {
        failures += Expect(parsed.Value().records[0].itemNameRaw == rawZ, "xmgQ raw itemName");
        failures += Expect(parsed.Value().records[0].itemInfoRaw == rawC, "xmgQ raw itemInfo");
        failures += Expect(parsed.Value().records[0].materials[0].nameRaw == rawY, "xmgQ raw material0");
        failures += Expect(parsed.Value().records[0].materials[1].nameRaw == rawN, "xmgQ raw material1");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "xmgQ raw BuildData");
    return failures;
}

int ExpectBadPayloadField(std::size_t index, std::string value, const char* message)
{
    std::vector<std::string> parts = BuildRecipeParts();
    parts[index] = std::move(value);
    return ExpectError(
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(1, JoinTrailing(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "xmgQ",
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
    failures += ExpectError(
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(1, "")),
        cg::packet::PacketErrorCode::EmptyInput,
        "xmgQ",
        "xmgQ empty data");

    std::string payload = BuildOneRecipePayload();
    payload.pop_back();
    failures += ExpectError(
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(1, payload)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "xmgQ",
        "xmgQ missing trailing delimiter");

    std::vector<std::string> parts = BuildRecipeParts();
    parts.pop_back();
    failures += ExpectError(
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(1, JoinTrailing(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "xmgQ",
        "xmgQ 21 fields");

    parts = BuildRecipeParts();
    parts.push_back("extra");
    failures += ExpectError(
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(1, JoinTrailing(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "xmgQ",
        "xmgQ 23 fields");

    std::string tooManyRecords;
    for (std::size_t index = 0; index < 57U; ++index) {
        tooManyRecords.append(BuildOneRecipePayload());
    }
    failures += ExpectError(
        cg::packet::receive::ItemRecipe::Parse(BuildPlainXmgQ(1, tooManyRecords)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "xmgQ",
        "xmgQ 57 records");

    failures += ExpectBadPayloadField(0U, "", "xmgQ empty recipeId");
    failures += ExpectBadPayloadField(0U, "x", "xmgQ bad recipeId");
    failures += ExpectBadPayloadField(1U, "x", "xmgQ bad itemLevel");
    failures += ExpectBadPayloadField(2U, "2147483648", "xmgQ overflow recipeItemId");
    failures += ExpectBadPayloadField(5U, "x", "xmgQ bad forcePoint");
    failures += ExpectBadPayloadField(9U, "x", "xmgQ bad material required");
    failures += ExpectBadPayloadField(6U, "2", "xmgQ usable out of range");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestSingleRecordRoundtrip();
    failures += TestMultipleRecords();
    failures += TestRawInnerEscapesPreserved();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
