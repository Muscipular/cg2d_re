#include "packet/packet.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using TradePetSkillInfo = cg::packet::receive::TradePetSkillInfo;

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

cg::packet::PacketField Field(
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string value)
{
    cg::packet::PacketField field;
    field.name = std::string(name);
    field.type = type;
    field.value = std::move(value);
    return field;
}

cg::packet::Packet PacketOf(
    cg::packet::PacketDirection direction,
    std::initializer_list<cg::packet::PacketField> fields)
{
    cg::packet::Packet packet(
        cg::packet::PacketCode(TradePetSkillInfo::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlain(int petIndex, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPetIndex = cg::packet::EncodeInt62(petIndex);
    if (!encodedPetIndex.HasValue()) {
        return "Yq ";
    }

    std::string plain("Yq ");
    plain.append(encodedPetIndex.Value());
    plain.push_back(' ');
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

std::string BuildPlain(int petIndex, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return BuildRawPlain(petIndex, "");
    }
    return BuildRawPlain(petIndex, encoded.Value());
}

std::string BuildPayload(std::size_t count)
{
    std::string payload;
    for (std::size_t index = 0; index < count; ++index) {
        payload.append("Skill");
        payload.append(std::to_string(index));
        payload.push_back('|');
    }
    return payload;
}

int ExpectTradePetSkillInfoPacket(
    const cg::packet::Packet& packet,
    int petIndex,
    std::string_view payload,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(TradePetSkillInfo::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "petIndex", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(petIndex), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].value == payload, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "A|B|";
    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(BuildPlain(2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Yq outer");
    if (parsed.HasValue()) {
        failures += ExpectTradePetSkillInfoPacket(parsed.Value(), 2, payload, "parse Yq packet");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::PacketResult<TradePetSkillInfo> parsed = TradePetSkillInfo::Parse(BuildRawPlain(2, ""));

    int failures = Expect(static_cast<bool>(parsed), "parse Yq empty data");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().petIndex == 2, "Yq empty petIndex");
    failures += Expect(parsed.Value().skillNameRaws.empty(), "Yq empty skill count");

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "Yq empty BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value().empty(), "Yq empty BuildData value");
    }
    return failures;
}

int TestRoundtrip()
{
    const std::string payload = "A\\zB||C\\nD|";
    const TradePetSkillInfo packet(2, {"A\\zB", "", "C\\nD"});

    const cg::packet::PacketResult<std::string> data = packet.BuildData();
    int failures = Expect(static_cast<bool>(data), "Yq BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "Yq BuildData payload");
    }

    const cg::packet::PacketResult<cg::packet::Packet> typedPacket = packet.ToPacket();
    failures += Expect(static_cast<bool>(typedPacket), "Yq ToPacket");
    if (!typedPacket.HasValue()) {
        return failures;
    }

    failures += ExpectTradePetSkillInfoPacket(typedPacket.Value(), 2, payload, "Yq ToPacket fields");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(typedPacket.Value());
    failures += Expect(static_cast<bool>(built), "build Yq packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<TradePetSkillInfo> parsed = TradePetSkillInfo::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip Yq");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().petIndex == 2, "roundtrip Yq petIndex");
        failures += Expect(parsed.Value().skillNameRaws.size() == 3U, "roundtrip Yq count");
        if (parsed.Value().skillNameRaws.size() == 3U) {
            failures += Expect(parsed.Value().skillNameRaws[0] == "A\\zB", "roundtrip Yq first");
            failures += Expect(parsed.Value().skillNameRaws[1].empty(), "roundtrip Yq middle empty");
            failures += Expect(parsed.Value().skillNameRaws[2] == "C\\nD", "roundtrip Yq third");
        }
    }
    return failures;
}

int TestRawEscapesPreserved()
{
    const std::string payload = "A\\zB|C\\cD|E\\yF|G\\nH|";
    const cg::packet::PacketResult<TradePetSkillInfo> parsed = TradePetSkillInfo::Parse(BuildRawPlain(2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Yq raw escaped strings");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().skillNameRaws.size() == 4U, "Yq raw escaped count");
    if (parsed.Value().skillNameRaws.size() == 4U) {
        failures += Expect(parsed.Value().skillNameRaws[0] == "A\\zB", "Yq raw z");
        failures += Expect(parsed.Value().skillNameRaws[1] == "C\\cD", "Yq raw c");
        failures += Expect(parsed.Value().skillNameRaws[2] == "E\\yF", "Yq raw y");
        failures += Expect(parsed.Value().skillNameRaws[3] == "G\\nH", "Yq raw n");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "Yq raw BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "Yq raw payload");
    }
    return failures;
}

int TestMaxCount()
{
    int failures = 0;

    const std::string tenPayload = BuildPayload(10U);
    const cg::packet::PacketResult<TradePetSkillInfo> parsed = TradePetSkillInfo::Parse(BuildPlain(2, tenPayload));
    failures += Expect(static_cast<bool>(parsed), "parse Yq 10 skills");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().skillNameRaws.size() == 10U, "Yq 10 skill count");
    }

    failures += ExpectError(
        TradePetSkillInfo::Parse(BuildPlain(2, BuildPayload(11U))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Yq",
        "Yq parse 11 skills");
    failures += ExpectError(
        TradePetSkillInfo(2, std::vector<std::string>(11U, "Skill")).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Yq",
        "Yq build 11 skills");
    return failures;
}

int TestPayloadErrors()
{
    int failures = 0;

    failures += ExpectError(
        TradePetSkillInfo::Parse(BuildPlain(2, "A|B")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yq",
        "Yq missing tail");
    failures += ExpectError(
        TradePetSkillInfo(2, {"A|B"}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Yq",
        "Yq build literal delimiter");

    std::string truncatedLegacy(1U, static_cast<char>(0x80));
    failures += ExpectError(
        TradePetSkillInfo(2, {std::move(truncatedLegacy)}).BuildData(),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yq",
        "Yq build truncated legacy char");
    return failures;
}

int TestOuterErrors()
{
    int failures = 0;

    failures += ExpectError(
        TradePetSkillInfo::Parse("RwK 2 A| "),
        cg::packet::PacketErrorCode::InvalidCode,
        "RwK",
        "Yq wrong code");
    failures += ExpectError(
        TradePetSkillInfo::Parse("Yq 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Yq",
        "Yq missing data");
    failures += ExpectError(
        TradePetSkillInfo::Parse("Yq 2 data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Yq",
        "Yq extra outer token");
    failures += ExpectError(
        TradePetSkillInfo::Parse("Yq _ data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yq",
        "Yq bad petIndex i62");
    failures += ExpectError(
        TradePetSkillInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("petIndex", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::EscapedString, "A|"),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yq",
        "Yq manual wrong direction");
    failures += ExpectError(
        TradePetSkillInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("petPlace", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::EscapedString, "A|"),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yq",
        "Yq manual wrong field name");
    failures += ExpectError(
        TradePetSkillInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("petIndex", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::RawString, "A|"),
            })),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "Yq",
        "Yq manual wrong field type");
    failures += ExpectError(
        TradePetSkillInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("petIndex", cg::packet::PacketFieldType::Int62, "bad"),
                Field("data", cg::packet::PacketFieldType::EscapedString, "A|"),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yq",
        "Yq manual bad petIndex");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyData();
    failures += TestRoundtrip();
    failures += TestRawEscapesPreserved();
    failures += TestMaxCount();
    failures += TestPayloadErrors();
    failures += TestOuterErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
