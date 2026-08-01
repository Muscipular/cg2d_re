#include "packet/Packet.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::TradePetSkillInfo::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::TradePetSkillInfo::Spec);
}

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

std::string LegacyDoubleByteText(unsigned char trail, std::string_view suffix)
{
    std::string text;
    text.push_back(static_cast<char>(0x81U));
    text.push_back(static_cast<char>(trail));
    text.append(suffix);
    return text;
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
    cg::packet::PacketFieldType dataType,
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
    failures += Expect(packet.fields[1].type == dataType, message);
    failures += Expect(packet.fields[1].value == payload, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "A|B|";
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlain(2, payload));

    int failures = Expect(TradePetSkillInfo::Spec.fieldCount == 2U, "Yq Spec exact2");
    failures += Expect(
        TradePetSkillInfo::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "Yq Spec petIndex i62");
    failures += Expect(
        TradePetSkillInfo::Fields[1].type == cg::packet::PacketFieldType::SubPayload,
        "Yq Spec data SubPayload");
    failures += Expect(static_cast<bool>(parsed), "parse Yq outer");
    if (parsed.HasValue()) {
        failures += ExpectTradePetSkillInfoPacket(
            parsed.Value(),
            2,
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse Yq packet");
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
    int failures = ExpectError(result, code, TradePetSkillInfo::Code, message);
    if (!result.HasError()) {
        return failures;
    }

    failures += Expect(result.Error().fieldIndex.has_value(), message);
    if (result.Error().fieldIndex.has_value()) {
        failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
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

int TestRawBuildAndSemanticParse()
{
    const std::string payload = "A\\zB|C\\cD|E\\yF|G\\nH|";
    const TradePetSkillInfo packet(2, {"A\\zB", "C\\cD", "E\\yF", "G\\nH"});

    const cg::packet::PacketResult<std::string> data = packet.BuildData();
    int failures = Expect(static_cast<bool>(data), "Yq Server raw BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "Yq BuildData preserves raw legacy tokens");
    }

    const cg::packet::PacketResult<cg::packet::Packet> typedPacket = packet.ToPacket();
    failures += Expect(static_cast<bool>(typedPacket), "Yq ToPacket");
    if (!typedPacket.HasValue()) {
        return failures;
    }

    failures += ExpectTradePetSkillInfoPacket(
        typedPacket.Value(),
        2,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "Yq ToPacket fields");

    const cg::packet::PacketResult<std::string> built = BuildPacket(typedPacket.Value());
    failures += Expect(static_cast<bool>(built), "build Yq packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<TradePetSkillInfo> parsed = TradePetSkillInfo::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse Yq Server raw payload");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().petIndex == 2, "Yq parsed petIndex");
        failures += Expect(parsed.Value().skillNameRaws.size() == 4U, "Yq semantic skill count");
        if (parsed.Value().skillNameRaws.size() == 4U) {
            failures += Expect(parsed.Value().skillNameRaws[0] == "A|B", "Yq semantic z");
            failures += Expect(parsed.Value().skillNameRaws[1] == "C,D", "Yq semantic c");
            failures += Expect(parsed.Value().skillNameRaws[2] == "E\\F", "Yq semantic y");
            failures += Expect(parsed.Value().skillNameRaws[3] == "G\nH", "Yq semantic n");
        }
        failures += ExpectFieldError(
            parsed.Value().BuildData(),
            cg::packet::PacketErrorCode::EncodeFailed,
            0U,
            "Yq semantic literal pipe is not Server raw");
    }

    const TradePetSkillInfo literalPacket(2, {"A,B", "C\nD"});
    const cg::packet::PacketResult<std::string> literalData = literalPacket.BuildData();
    failures += Expect(literalData.HasValue(), "Yq literal comma and newline BuildData");
    if (literalData.HasValue()) {
        failures += Expect(literalData.Value() == "A,B|C\nD|", "Yq literal text stays raw");
        const cg::packet::PacketResult<TradePetSkillInfo> literalParsed =
            TradePetSkillInfo::Parse(BuildPlain(2, literalData.Value()));
        failures += Expect(literalParsed.HasValue(), "Yq literal text parses");
        if (literalParsed.HasValue()) {
            failures += Expect(
                literalParsed.Value().skillNameRaws == std::vector<std::string>({"A,B", "C\nD"}),
                "Yq literal text semantic values");
        }
    }
    return failures;
}

int TestGbkRawRoundtrip()
{
    const std::string first = LegacyDoubleByteText(0x7CU, "First");
    const std::string second = LegacyDoubleByteText(0x5CU, "Second");
    std::string payload = first;
    payload.push_back('|');
    payload.append(second);
    payload.push_back('|');
    const cg::packet::PacketResult<TradePetSkillInfo> parsed =
        TradePetSkillInfo::Parse(BuildPlain(2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Yq GBK trail bytes");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().skillNameRaws.size() == 2U, "Yq GBK skill count");
    if (parsed.Value().skillNameRaws.size() == 2U) {
        failures += Expect(parsed.Value().skillNameRaws[0] == first, "Yq GBK trail 0x7C preserved");
        failures += Expect(parsed.Value().skillNameRaws[1] == second, "Yq GBK trail 0x5C preserved");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "Yq GBK raw BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "Yq GBK raw roundtrip");
    }
    return failures;
}

int TestEmptySkillNames()
{
    const cg::packet::PacketResult<TradePetSkillInfo> singleEmpty =
        TradePetSkillInfo::Parse(BuildPlain(2, "|"));
    int failures = Expect(singleEmpty.HasValue(), "parse Yq one empty skill");
    if (singleEmpty.HasValue()) {
        failures += Expect(
            singleEmpty.Value().skillNameRaws.size() == 1U &&
                singleEmpty.Value().skillNameRaws[0].empty(),
            "Yq one empty skill value");
    }

    const std::string payload = "|Middle||";
    const cg::packet::PacketResult<TradePetSkillInfo> parsed =
        TradePetSkillInfo::Parse(BuildPlain(2, payload));

    failures += Expect(parsed.HasValue(), "parse Yq empty skill names");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().skillNameRaws.size() == 3U, "Yq empty skill count");
    if (parsed.Value().skillNameRaws.size() == 3U) {
        failures += Expect(parsed.Value().skillNameRaws[0].empty(), "Yq first empty skill");
        failures += Expect(parsed.Value().skillNameRaws[1] == "Middle", "Yq middle skill");
        failures += Expect(parsed.Value().skillNameRaws[2].empty(), "Yq final empty skill");
    }

    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(rebuilt.HasValue(), "Yq empty skills BuildData");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "Yq empty skills canonical payload");
    }
    return failures;
}

int TestSignedPetIndex()
{
    int failures = 0;
    for (const int petIndex : {
             -3,
             std::numeric_limits<int>::min(),
             std::numeric_limits<int>::max(),
         }) {
        const cg::packet::PacketResult<TradePetSkillInfo> parsed =
            TradePetSkillInfo::Parse(BuildPlain(petIndex, "Skill|"));
        failures += Expect(parsed.HasValue(), "parse Yq signed petIndex");
        if (parsed.HasValue()) {
            failures += Expect(parsed.Value().petIndex == petIndex, "Yq signed petIndex value");
        }
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

    failures += ExpectFieldError(
        TradePetSkillInfo::Parse(BuildPlain(2, "A|B")),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "Yq missing tail");
    failures += ExpectFieldError(
        TradePetSkillInfo(2, {"A|B"}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "Yq build literal delimiter");

    std::string truncatedLegacy(1U, static_cast<char>(0x81U));
    failures += ExpectFieldError(
        TradePetSkillInfo(2, {"Good", std::move(truncatedLegacy)}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        1U,
        "Yq build truncated legacy char");

    failures += ExpectFieldError(
        TradePetSkillInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("petIndex", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::EscapedString, "Good|Bad\\|"),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "Yq parse truncated inner legacy escape");

    const TradePetSkillInfo trailingBackslash(2, {"Tail\\"});
    const cg::packet::PacketResult<std::string> trailingBackslashData =
        trailingBackslash.BuildData();
    failures += Expect(trailingBackslashData.HasValue(), "Yq Server raw trailing backslash builds");
    if (trailingBackslashData.HasValue()) {
        failures += Expect(
            trailingBackslashData.Value() == "Tail\\|",
            "Yq Server raw trailing backslash preserved");
        failures += ExpectFieldError(
            TradePetSkillInfo::Parse(BuildPlain(2, trailingBackslashData.Value())),
            cg::packet::PacketErrorCode::DecodeFailed,
            0U,
            "Yq cg2d semantic parse rejects truncated inner escape");
    }

    std::string truncatedPayload = "Good|";
    truncatedPayload.push_back(static_cast<char>(0x81U));
    failures += ExpectFieldError(
        TradePetSkillInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("petIndex", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::EscapedString, std::move(truncatedPayload)),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "Yq parse truncated GBK payload");
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
                Field("data", cg::packet::PacketFieldType::Unknown, "A|"),
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
    failures += TestRawBuildAndSemanticParse();
    failures += TestGbkRawRoundtrip();
    failures += TestEmptySkillNames();
    failures += TestSignedPetIndex();
    failures += TestMaxCount();
    failures += TestPayloadErrors();
    failures += TestOuterErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
