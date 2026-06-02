#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

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
    failures += Expect(packet.fields[2].type == cg::packet::PacketFieldType::EscapedString, message);
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
    const std::string payload = "100|5|1|2|3|4|0|123";
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlainJgO(0, 1, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse JgO outer");
    if (parsed.HasValue()) {
        failures += ExpectTechUseResultPacket(parsed.Value(), 0, 1, payload, "parse JgO outer packet");
    }
    return failures;
}

int TestType1Payloads()
{
    int failures = 0;

    const std::string noAtomsPayload = "100|5|1|2|3|4|0|123";
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> noAtoms =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, noAtomsPayload));
    failures += Expect(static_cast<bool>(noAtoms), "parse JgO type1 no atoms");
    if (noAtoms.HasValue()) {
        failures += Expect(noAtoms.Value().result == 0, "JgO type1 result");
        failures += Expect(noAtoms.Value().type == 1, "JgO type1 type");
        failures += ExpectBaseStats(noAtoms.Value().payload, 100, 5, 1, 2, 3, 4, "JgO type1 base stats");
        failures += Expect(noAtoms.Value().payload.injuryFlag == 0, "JgO type1 injuryFlag");
        failures += Expect(noAtoms.Value().payload.itemImageNumber == 123, "JgO type1 itemImageNumber");
        failures += Expect(noAtoms.Value().payload.deleteAtomsRaw.empty(), "JgO type1 no atom count");
        failures += Expect(noAtoms.Value().BuildData() == noAtomsPayload, "JgO type1 no atom build");
    }

    const std::string oneAtomPayload = "100|5|1|2|3|4|0|123|atomA";
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> oneAtom =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, oneAtomPayload));
    failures += Expect(static_cast<bool>(oneAtom), "parse JgO type1 one atom");
    if (oneAtom.HasValue()) {
        failures += Expect(oneAtom.Value().payload.deleteAtomsRaw.size() == 1U, "JgO type1 one atom count");
        if (oneAtom.Value().payload.deleteAtomsRaw.size() == 1U) {
            failures += Expect(oneAtom.Value().payload.deleteAtomsRaw[0] == "atomA", "JgO type1 one atom value");
        }
        failures += Expect(oneAtom.Value().BuildData() == oneAtomPayload, "JgO type1 one atom build");
    }

    const std::string sixAtomsPayload = "100|5|1|2|3|4|0|123|a|b|c|d|e|f";
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> sixAtoms =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, sixAtomsPayload));
    failures += Expect(static_cast<bool>(sixAtoms), "parse JgO type1 six atoms");
    if (sixAtoms.HasValue()) {
        failures += Expect(sixAtoms.Value().payload.deleteAtomsRaw.size() == 6U, "JgO type1 six atom count");
        failures += Expect(sixAtoms.Value().BuildData() == sixAtomsPayload, "JgO type1 six atom build");
    }

    const std::string tailEmptyPayload = "100|5|1|2|3|4|0|123|a||";
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> tailEmpty =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, tailEmptyPayload));
    failures += Expect(static_cast<bool>(tailEmpty), "parse JgO type1 tail empty atoms");
    if (tailEmpty.HasValue()) {
        failures += Expect(tailEmpty.Value().payload.deleteAtomsRaw.size() == 3U, "JgO type1 tail empty count");
        failures += Expect(tailEmpty.Value().BuildData() == tailEmptyPayload, "JgO type1 tail empty build");
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
        failures += Expect(parsed.Value().BuildData() == payload, "JgO type2 build");
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
        failures += Expect(parsed.Value().BuildData() == payload, "JgO type3 build");
    }
    return failures;
}

int TestType4Payloads()
{
    int failures = 0;

    const std::string payload8 = "10|20|1|3|4|5|888|Name\\zItem\\c7";
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> parsed8 =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 4, payload8));
    failures += Expect(static_cast<bool>(parsed8), "parse JgO type4 8 fields");
    if (parsed8.HasValue()) {
        failures += Expect(parsed8.Value().result == 0, "JgO type4 result zero");
        failures += ExpectBaseStats(parsed8.Value().payload, 10, 20, 1, 3, 4, 5, "JgO type4 base stats");
        failures += Expect(parsed8.Value().payload.itemImageNumber == 888, "JgO type4 image");
        failures += Expect(parsed8.Value().payload.itemNameRaw == "Name\\zItem\\c7", "JgO type4 raw name");
        failures += Expect(!parsed8.Value().payload.hasPetHelpFlag, "JgO type4 no pet help");
        failures += Expect(parsed8.Value().BuildData() == payload8, "JgO type4 8 build");
    }

    const std::string payload9 = "10|20|1|3|4|5|888|Name|1";
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> parsed9 =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(1, 4, payload9));
    failures += Expect(static_cast<bool>(parsed9), "parse JgO type4 9 fields");
    if (parsed9.HasValue()) {
        failures += Expect(parsed9.Value().payload.hasPetHelpFlag, "JgO type4 has pet help");
        failures += Expect(parsed9.Value().payload.petHelpFlag == 1, "JgO type4 pet help");
        failures += Expect(parsed9.Value().BuildData() == payload9, "JgO type4 9 build");
    }

    return failures;
}

int TestRoundtrip()
{
    const std::string payload = "10|20|1|3|4|5|888|Name\\zItem\\c7";
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> parsed =
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 4, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse JgO roundtrip source");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build JgO packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainJgO(0, 4, payload), "build JgO packet value");
    const cg::packet::PacketResult<cg::packet::receive::TechUseResult> roundtrip =
        cg::packet::receive::TechUseResult::Parse(built.Value());
    failures += Expect(static_cast<bool>(roundtrip), "roundtrip JgO packet");
    if (roundtrip.HasValue()) {
        failures += Expect(roundtrip.Value().payload.itemNameRaw == "Name\\zItem\\c7", "roundtrip JgO raw name");
    }
    return failures;
}

int TestBuildPacketShape()
{
    cg::packet::receive::TechUsePayload payload;
    payload.exp = 10;
    payload.fame = 20;
    payload.levelUp = 1;
    payload.stamina = 3;
    payload.dexterity = 4;
    payload.intelligence = 5;
    payload.itemImageNumber = 888;
    payload.itemNameRaw = "Name";
    payload.hasPetHelpFlag = true;
    payload.petHelpFlag = 1;
    const cg::packet::receive::TechUseResult techUseResult(0, 4, std::move(payload));

    const std::string data = "10|20|1|3|4|5|888|Name|1";
    int failures = ExpectTechUseResultPacket(techUseResult.ToPacket(), 0, 4, data, "JgO ToPacket type4");
    const cg::packet::PacketResult<std::string> built = BuildPacket(techUseResult.ToPacket());
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
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JgO",
        "JgO type1 missing fields");
    failures += ExpectError(
        cg::packet::receive::TechUseResult::Parse(BuildPlainJgO(0, 1, "1|2|3|4|5|6|7|8|a|b|c|d|e|f|g")),
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
