#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::TechUseResult::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::TechUseResult::Spec);
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
int ExpectFieldError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::string_view packetCode,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, packetCode, message);
    if (!result.HasError()) {
        return failures;
    }

    failures += Expect(result.Error().fieldIndex.has_value(), message);
    if (result.Error().fieldIndex.has_value()) {
        failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
    }
    return failures;
}

std::string EncodeLegacy(std::string_view value)
{
    const cg::packet::PacketResult<std::string> encoded =
        cg::packet::EncodeLegacyPayloadText(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string WithGbkTrailPipe(std::string_view prefix)
{
    std::string value(prefix);
    value.push_back(static_cast<char>(0x96));
    value.push_back(static_cast<char>(0x7C));
    return value;
}

std::string BuildType1Data(const std::vector<std::string>& atoms)
{
    std::string data = "100|5|1|2|3|4|0|123|";
    for (const std::string& atom : atoms) {
        data.append(EncodeLegacy(atom));
        data.push_back('|');
    }
    return data;
}

std::string BuildPlainJgO(int result, int type, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedResult = cg::packet::EncodeInt62(result);
    const cg::packet::PacketResult<std::string> encodedType = cg::packet::EncodeInt62(type);
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedResult.HasValue() || !encodedType.HasValue() || !encodedPayload.HasValue()) {
        return "JgO   ";
    }

    std::string plain = "JgO ";
    plain.append(encodedResult.Value());
    plain.push_back(' ');
    plain.append(encodedType.Value());
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectTechUseResultPacket(
    const cg::packet::Packet& packet,
    int result,
    int type,
    std::string_view data,
    cg::packet::PacketFieldType dataType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::TechUseResult::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "result", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(result), message);
    failures += Expect(packet.fields[1].name == "type", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[1].value == std::to_string(type), message);
    failures += Expect(packet.fields[2].name == "data", message);
    failures += Expect(packet.fields[2].type == dataType, message);
    failures += Expect(packet.fields[2].value == data, message);
    return failures;
}

int ExpectBaseStats(
    const cg::packet::receive::TechUsePayload& payload,
    int exp,
    int fame,
    int levelUp,
    int stamina,
    int dexterity,
    int intelligence,
    const char* message)
{
    int failures = 0;
    failures += Expect(payload.exp == exp, message);
    failures += Expect(payload.fame == fame, message);
    failures += Expect(payload.levelUp == levelUp, message);
    failures += Expect(payload.stamina == stamina, message);
    failures += Expect(payload.dexterity == dexterity, message);
    failures += Expect(payload.intelligence == intelligence, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = BuildType1Data({"atom"});
    int failures = Expect(
        cg::packet::receive::TechUseResult::Fields[2U].type ==
            cg::packet::PacketFieldType::SubPayload,
        "JgO data spec SubPayload");
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlainJgO(0, 1, payload));

    failures += Expect(static_cast<bool>(parsed), "parse JgO outer");
    if (parsed.HasValue()) {
        failures += ExpectTechUseResultPacket(
            parsed.Value(),
            0,
            1,
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse JgO outer packet");
    }
    return failures;
}

int TestType1Payloads()
{
    int failures = 0;

    const std::string noAtomsPayload = BuildType1Data({});
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> noAtoms =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, noAtomsPayload));
    failures += Expect(static_cast<bool>(noAtoms), "parse JgO type1 zero atoms");
    if (noAtoms.HasValue()) {
        failures += Expect(noAtoms.Value().payload.deleteAtomsRaw.empty(), "JgO type1 zero atom count");
        const cg::packet::PacketResult<std::string> builtData = noAtoms.Value().BuildData();
        failures += Expect(static_cast<bool>(builtData), "build JgO type1 zero atom data");
        if (builtData.HasValue()) {
            failures += Expect(builtData.Value() == noAtomsPayload, "JgO type1 zero atom build");
            failures += Expect(builtData.Value().back() == '|', "JgO type1 zero atom trailing delimiter");
        }
    }

    const std::string semanticAtom = WithGbkTrailPipe("Atom,One|Path\\Line\n");
    const std::string oneAtomPayload = BuildType1Data({semanticAtom});
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> oneAtom =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, oneAtomPayload));
    failures += Expect(static_cast<bool>(oneAtom), "parse JgO type1 one atom");
    if (oneAtom.HasValue()) {
        failures += Expect(oneAtom.Value().result == 0, "JgO type1 result");
        failures += Expect(oneAtom.Value().type == 1, "JgO type1 type");
        failures += ExpectBaseStats(oneAtom.Value().payload, 100, 5, 1, 2, 3, 4, "JgO type1 base stats");
        failures += Expect(oneAtom.Value().payload.injuryFlag == 0, "JgO type1 injuryFlag");
        failures += Expect(oneAtom.Value().payload.itemImageNumber == 123, "JgO type1 itemImageNumber");
        failures += Expect(oneAtom.Value().payload.deleteAtomsRaw.size() == 1U, "JgO type1 one atom count");
        if (oneAtom.Value().payload.deleteAtomsRaw.size() == 1U) {
            failures += Expect(
                oneAtom.Value().payload.deleteAtomsRaw[0] == semanticAtom,
                "JgO type1 one atom semantic GBK/legacy");
        }
        const cg::packet::PacketResult<std::string> builtData = oneAtom.Value().BuildData();
        failures += Expect(static_cast<bool>(builtData), "build JgO type1 one atom data");
        if (builtData.HasValue()) {
            failures += Expect(builtData.Value() == oneAtomPayload, "JgO type1 one atom build");
            failures += Expect(builtData.Value().back() == '|', "JgO type1 fixed trailing delimiter");
        }
    }

    const std::vector<std::string> expectedAtoms = {
        "a",
        "b|pipe",
        "c,comma",
        "d\\slash",
        "e\nline",
        WithGbkTrailPipe("f"),
    };
    const std::string sixAtomsPayload = BuildType1Data(expectedAtoms);
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> sixAtoms =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, sixAtomsPayload));
    failures += Expect(static_cast<bool>(sixAtoms), "parse JgO type1 six atoms");
    if (sixAtoms.HasValue()) {
        failures += Expect(sixAtoms.Value().payload.deleteAtomsRaw.size() == 6U, "JgO type1 six atom count");
        failures += Expect(sixAtoms.Value().payload.deleteAtomsRaw == expectedAtoms, "JgO type1 six atom semantic values");
        const cg::packet::PacketResult<std::string> builtData = sixAtoms.Value().BuildData();
        failures += Expect(static_cast<bool>(builtData), "build JgO type1 six atom data");
        if (builtData.HasValue()) {
            failures += Expect(builtData.Value() == sixAtomsPayload, "JgO type1 six atom build");
        }
    }

    return failures;
}

int TestType2Payload()
{
    const std::string payload = "10|-20|0|3|4|5|6";
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> parsed =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(1, 2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse JgO type2");
    if (parsed.HasValue()) {
        failures += ExpectBaseStats(parsed.Value().payload, 10, -20, 0, 3, 4, 5, "JgO type2 base stats");
        failures += Expect(parsed.Value().payload.injuryFlag == 6, "JgO type2 injuryFlag");
        const cg::packet::PacketResult<std::string> builtData = parsed.Value().BuildData();
        failures += Expect(static_cast<bool>(builtData), "build JgO type2 data");
        if (builtData.HasValue()) {
            failures += Expect(builtData.Value() == payload, "JgO type2 build");
            failures += Expect(builtData.Value().back() != '|', "JgO type2 no trailing delimiter");
        }
    }
    return failures;
}

int TestType3Payload()
{
    const std::string payload = "10|20|1|3|4|5|-99";
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> parsed =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(1, 3, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse JgO type3");
    if (parsed.HasValue()) {
        failures += ExpectBaseStats(parsed.Value().payload, 10, 20, 1, 3, 4, 5, "JgO type3 base stats");
        failures += Expect(parsed.Value().payload.value == -99, "JgO type3 value");
        const cg::packet::PacketResult<std::string> builtData = parsed.Value().BuildData();
        failures += Expect(static_cast<bool>(builtData), "build JgO type3 data");
        if (builtData.HasValue()) {
            failures += Expect(builtData.Value() == payload, "JgO type3 build");
            failures += Expect(builtData.Value().back() != '|', "JgO type3 no trailing delimiter");
        }
    }
    return failures;
}

int TestType4Payloads()
{
    int failures = 0;

    const std::string itemName = WithGbkTrailPipe("Name|Item,Line\\Tail\n");
    const std::string payload8 = "10|20|1|3|4|5|888|" + EncodeLegacy(itemName);
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> parsed8 =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 4, payload8));
    failures += Expect(static_cast<bool>(parsed8), "parse JgO type4 8 fields");
    if (parsed8.HasValue()) {
        failures += Expect(parsed8.Value().result == 0, "JgO type4 result zero");
        failures += ExpectBaseStats(parsed8.Value().payload, 10, 20, 1, 3, 4, 5, "JgO type4 base stats");
        failures += Expect(parsed8.Value().payload.itemImageNumber == 888, "JgO type4 image");
        failures += Expect(parsed8.Value().payload.itemNameRaw == itemName, "JgO type4 semantic GBK/legacy name");
        failures += Expect(!parsed8.Value().payload.hasPetHelpFlag, "JgO type4 no pet help");
        const cg::packet::PacketResult<std::string> builtData = parsed8.Value().BuildData();
        failures += Expect(static_cast<bool>(builtData), "build JgO type4 8 data");
        if (builtData.HasValue()) {
            failures += Expect(builtData.Value() == payload8, "JgO type4 8 build");
        }
    }

    const std::string payload9 = payload8 + "|-1";
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> parsed9 =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(1, 4, payload9));
    failures += Expect(static_cast<bool>(parsed9), "parse JgO type4 9 fields");
    if (parsed9.HasValue()) {
        failures += Expect(parsed9.Value().payload.hasPetHelpFlag, "JgO type4 has pet help");
        failures += Expect(parsed9.Value().payload.petHelpFlag == -1, "JgO type4 pet help");
        failures += Expect(parsed9.Value().payload.itemNameRaw == itemName, "JgO type4 9 semantic name");
        const cg::packet::PacketResult<std::string> builtData = parsed9.Value().BuildData();
        failures += Expect(static_cast<bool>(builtData), "build JgO type4 9 data");
        if (builtData.HasValue()) {
            failures += Expect(builtData.Value() == payload9, "JgO type4 9 build");
        }
    }

    return failures;
}

int TestRoundtrip()
{
    const std::string itemName = WithGbkTrailPipe("Round|Trip,Name\\Line\n");
    const std::string payload = "10|20|1|3|4|5|888|" + EncodeLegacy(itemName);
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> parsed =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 4, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse JgO roundtrip source");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "JgO roundtrip ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectTechUseResultPacket(
        packet.Value(),
        0,
        4,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "JgO roundtrip ToPacket fields");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build JgO packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainJgO(0, 4, payload), "build JgO packet value");
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> roundtrip =
        cg::packet::receive::TechUseResult::Parse(built.Value());
    failures += Expect(static_cast<bool>(roundtrip), "roundtrip JgO packet");
    if (roundtrip.HasValue()) {
        failures += Expect(roundtrip.Value().payload.itemNameRaw == itemName, "roundtrip JgO semantic name");
    }
    return failures;
}

int TestBuildPacketShape()
{
    const std::string itemName = "Name|Item,Line\\Tail\n";
    cg::packet::receive::TechUsePayload payload;
    payload.exp = 10;
    payload.fame = 20;
    payload.levelUp = 1;
    payload.stamina = 3;
    payload.dexterity = 4;
    payload.intelligence = 5;
    payload.itemImageNumber = 888;
    payload.itemNameRaw = itemName;
    payload.hasPetHelpFlag = true;
    payload.petHelpFlag = 1;
    const std::string data = "10|20|1|3|4|5|888|" + EncodeLegacy(itemName) + "|1";
    const cg::packet::receive::TechUseResult techUseResult(0, 4, std::move(payload));
    const cg::packet::PacketResult<cg::packet::Packet> packet = techUseResult.ToPacket();
    int failures = Expect(static_cast<bool>(packet), "JgO ToPacket type4");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectTechUseResultPacket(
        packet.Value(),
        0,
        4,
        data,
        cg::packet::PacketFieldType::SubPayload,
        "JgO ToPacket type4 fields");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build JgO type4 packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainJgO(0, 4, data), "build JgO type4 value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse("ow 0 data 1 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "ow",
        "JgO wrong code");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse("JgO 0 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        "JgO outer count missing");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse("JgO 0 1 data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        "JgO outer count extra");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 5, "1|2|3|4|5|6|7")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        "JgO bad type");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse("JgO _ 1 1|2|3|4|5|6|7|8 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        "JgO bad result i62");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse("JgO 0 _ 1|2|3|4|5|6|7|8 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        "JgO bad type i62");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, "")),
        cg::packet::PacketErrorCode::EmptyInput,
        "JgO",
        "JgO empty data");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, "1|2|3|4|5|6|7")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        "JgO type1 missing trailing delimiter");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, "1|2|3|4|5|6|7|8|a|b|c|d|e|f|g|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        "JgO type1 seven atoms");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, "1|2|3|4|5|6|7|8|a||b")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        "JgO type1 middle empty atom");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 2, "1|2|3|4|5|6")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        "JgO type2 missing field");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 2, "1|2|3|4|5|6|7|8")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        "JgO type2 extra field");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 2, "1|2|x|4|5|6|7")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        "JgO type2 bad numeric");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 3, "1|2|3|4|5|6")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        "JgO type3 missing field");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 3, "1|2|3|4|5|6|7|8")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        "JgO type3 extra field");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 3, "1|2|3|4|5|6|2147483648")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        "JgO type3 overflow");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 4, "1|2|3|4|5|6|7")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        "JgO type4 missing field");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 4, "1|2|3|4|5|6|7|Name|1|2")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        "JgO type4 extra field");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 4, "1|2|3|4|5|6|7|Name|x")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        "JgO type4 bad petHelpFlag");

    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse("JgO 0 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        2U,
        "JgO outer missing field index");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse("JgO 0 1 data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        3U,
        "JgO outer extra field index");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse("JgO _ 1 data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        0U,
        "JgO result field index");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse("JgO 0 _ data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        1U,
        "JgO type field index");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, "1|2|3|4|5|6|7|8|atom")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        2U,
        "JgO type1 missing trailing delimiter index");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse(
            BuildPlainJgO(0, 1, "1|2|3|4|5|6|7|8|a|b|c|d|e|f|g|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        2U,
        "JgO type1 seven atoms index");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse(
            BuildPlainJgO(0, 1, "1|2|3|4|5|6|7|8|bad\\|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        8U,
        "JgO type1 first atom legacy truncated index");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse(
            BuildPlainJgO(0, 1, "1|2|3|4|5|6|7|8|a|b|c|d|e|bad\\|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        13U,
        "JgO type1 sixth atom legacy truncated index");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 2, "1|2|x|4|5|6|7")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        2U,
        "JgO type2 decimal local index");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 2, "1|2|3|4|5|6|7|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        2U,
        "JgO type2 trailing delimiter rejected");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 3, "1|2|3|4|5|6|2147483648")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        6U,
        "JgO type3 decimal local index");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 3, "1|2|3|4|5|6|7|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        2U,
        "JgO type3 trailing delimiter rejected");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 4, "1|2|3|4|5|6|7|Name\\")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        7U,
        "JgO type4 itemName legacy truncated index");
    failures += ExpectFieldError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 4, "1|2|3|4|5|6|7|Name|x")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JgO",
        8U,
        "JgO type4 petHelpFlag local index");

    cg::packet::receive::TechUsePayload badAtomPayload;
    badAtomPayload.deleteAtomsRaw.emplace_back(1U, static_cast<char>(0x96));
    const cg::packet::receive::TechUseResult badAtomValue(0, 1, std::move(badAtomPayload));
    failures += ExpectFieldError(
        badAtomValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "JgO",
        8U,
        "JgO build atom truncated GBK");
    failures += ExpectFieldError(
        badAtomValue.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "JgO",
        8U,
        "JgO ToPacket atom encoding propagation");

    cg::packet::receive::TechUsePayload sevenAtomsPayload;
    sevenAtomsPayload.deleteAtomsRaw.assign(7U, "atom");
    const cg::packet::receive::TechUseResult sevenAtomsValue(0, 1, std::move(sevenAtomsPayload));
    failures += ExpectFieldError(
        sevenAtomsValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "JgO",
        2U,
        "JgO build seven atoms rejected");

    cg::packet::receive::TechUsePayload badItemNamePayload;
    badItemNamePayload.itemNameRaw.assign(1U, static_cast<char>(0x96));
    const cg::packet::receive::TechUseResult badItemNameValue(0, 4, std::move(badItemNamePayload));
    failures += ExpectFieldError(
        badItemNameValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "JgO",
        7U,
        "JgO build itemName truncated GBK");
    failures += ExpectFieldError(
        badItemNameValue.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "JgO",
        7U,
        "JgO ToPacket itemName encoding propagation");

    const cg::packet::receive::TechUseResult badTypeValue(
        0,
        5,
        cg::packet::receive::TechUsePayload {});
    failures += ExpectFieldError(
        badTypeValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "JgO",
        1U,
        "JgO build invalid type index");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestType1Payloads();
    failures += TestType2Payload();
    failures += TestType3Payload();
    failures += TestType4Payloads();
    failures += TestRoundtrip();
    failures += TestBuildPacketShape();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
