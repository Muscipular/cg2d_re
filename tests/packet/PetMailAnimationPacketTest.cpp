#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::PetMailAnimation::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::PetMailAnimation::Spec);
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

std::string EncodeI62(int value)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeInt62(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string EncodeEs(std::string_view value)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string BuildPlainLduj(
    int objIndex,
    int graphicsNo,
    int x,
    int y,
    int dir,
    int flg,
    int no,
    std::string_view data)
{
    std::string plain = "lDuJ ";
    plain.append(EncodeI62(objIndex));
    plain.push_back(' ');
    plain.append(EncodeI62(graphicsNo));
    plain.push_back(' ');
    plain.append(EncodeI62(x));
    plain.push_back(' ');
    plain.append(EncodeI62(y));
    plain.push_back(' ');
    plain.append(EncodeI62(dir));
    plain.push_back(' ');
    plain.append(EncodeI62(flg));
    plain.push_back(' ');
    plain.append(EncodeI62(no));
    plain.push_back(' ');
    plain.append(EncodeEs(data));
    plain.push_back(' ');
    return plain;
}

std::string BuildPayloadData()
{
    return "2|10|11|12|3|456|7|-8|Pet\\cName|Title\\zData\\yMark\\nLine|1|0";
}

cg::packet::receive::PetMailAnimationPayload BuildPayloadValue()
{
    cg::packet::receive::PetMailAnimationPayload payload;
    payload.typeMb = 2;
    payload.objectIndex = 62;
    payload.x = 11;
    payload.y = 12;
    payload.dir = 3;
    payload.imageNo = 456;
    payload.lv = 7;
    payload.field40 = -8;
    payload.nameRaw = "Pet\\cName";
    payload.titleDataRaw = "Title\\zData\\yMark\\nLine";
    payload.flg1 = 1;
    payload.flg2 = 0;
    return payload;
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

int ExpectPetMailAnimationPacket(
    const cg::packet::Packet& packet,
    int flg,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::PetMailAnimation::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 8U, message);
    if (packet.fields.size() != 8U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "objIndex", cg::packet::PacketFieldType::Int62, "1", message);
    failures += ExpectField(packet, 1U, "graphicsno", cg::packet::PacketFieldType::Int62, "62", message);
    failures += ExpectField(packet, 2U, "x", cg::packet::PacketFieldType::Int62, "-3", message);
    failures += ExpectField(packet, 3U, "y", cg::packet::PacketFieldType::Int62, "4", message);
    failures += ExpectField(packet, 4U, "dir", cg::packet::PacketFieldType::Int62, "5", message);
    failures += ExpectField(packet, 5U, "flg", cg::packet::PacketFieldType::Int62, std::to_string(flg), message);
    failures += ExpectField(packet, 6U, "no", cg::packet::PacketFieldType::Int62, "7", message);
    failures += ExpectField(packet, 7U, "data", cg::packet::PacketFieldType::EscapedString, data, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainLduj(1, 62, -3, 4, 5, 0, 7, ""));

    int failures = Expect(static_cast<bool>(parsed), "parse lDuJ outer");
    if (parsed.HasValue()) {
        failures += ExpectPetMailAnimationPacket(parsed.Value(), 0, "", "parse lDuJ outer packet");
    }
    return failures;
}

int TestFlagZeroRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::receive::PetMailAnimation> parsed =
        cg::packet::receive::PetMailAnimation::Parse(BuildPlainLduj(1, 62, -3, 4, 5, 0, 7, ""));

    int failures = Expect(static_cast<bool>(parsed), "parse lDuJ flg 0");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().objIndex == 1, "lDuJ flg 0 objIndex");
    failures += Expect(parsed.Value().graphicsNo == 62, "lDuJ flg 0 graphicsNo");
    failures += Expect(parsed.Value().x == -3, "lDuJ flg 0 x");
    failures += Expect(parsed.Value().flg == 0, "lDuJ flg 0 flg");
    failures += Expect(!parsed.Value().hasPayload, "lDuJ flg 0 no payload");
    failures += Expect(parsed.Value().BuildData().empty(), "lDuJ flg 0 data empty");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build lDuJ flg 0");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainLduj(1, 62, -3, 4, 5, 0, 7, ""), "roundtrip lDuJ flg 0");
    }
    return failures;
}

int TestPayloadRoundtrip()
{
    const std::string data = BuildPayloadData();
    const cg::packet::PacketResult<cg::packet::receive::PetMailAnimation> parsed =
        cg::packet::receive::PetMailAnimation::Parse(BuildPlainLduj(1, 62, -3, 4, 5, 1, 7, data));

    int failures = Expect(static_cast<bool>(parsed), "parse lDuJ payload");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().hasPayload, "lDuJ payload present");
    failures += Expect(parsed.Value().payload.typeMb == 2, "lDuJ payload typeMb");
    failures += Expect(parsed.Value().payload.objectIndex == 62, "lDuJ payload objectIndex");
    failures += Expect(parsed.Value().payload.x == 11, "lDuJ payload x");
    failures += Expect(parsed.Value().payload.y == 12, "lDuJ payload y");
    failures += Expect(parsed.Value().payload.imageNo == 456, "lDuJ payload imageNo");
    failures += Expect(parsed.Value().payload.field40 == -8, "lDuJ payload field40");
    failures += Expect(parsed.Value().payload.nameRaw == "Pet\\cName", "lDuJ payload nameRaw");
    failures += Expect(
        parsed.Value().payload.titleDataRaw == "Title\\zData\\yMark\\nLine",
        "lDuJ payload titleDataRaw");
    failures += Expect(parsed.Value().BuildData() == data, "lDuJ payload BuildData");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build lDuJ payload");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainLduj(1, 62, -3, 4, 5, 1, 7, data), "roundtrip lDuJ payload");
    }
    return failures;
}

int TestBuildFromValue()
{
    const cg::packet::receive::PetMailAnimation packetValue(1, 62, -3, 4, 5, 1, 7, BuildPayloadValue());
    const cg::packet::Packet packet = packetValue.ToPacket();

    int failures = ExpectPetMailAnimationPacket(packet, 1, BuildPayloadData(), "lDuJ ToPacket");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build lDuJ value");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == BuildPlainLduj(1, 62, -3, 4, 5, 1, 7, BuildPayloadData()),
            "build lDuJ value text");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse("lDuJ 1 10 -3 4 5 0 7"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "lDuJ",
        "lDuJ missing data");
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse(BuildPlainLduj(1, 62, -3, 4, 5, 1, 7, "")),
        cg::packet::PacketErrorCode::EmptyInput,
        "lDuJ",
        "lDuJ nonzero flg empty payload");
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse("lDuJ _ 10 -3 4 5 0 7  "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lDuJ",
        "lDuJ bad outer i62");
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse(BuildPlainLduj(
            1,
            62,
            -3,
            4,
            5,
            1,
            7,
            "2|10|11|12|3|456|7|-8|Pet|Title|1")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "lDuJ",
        "lDuJ payload 11 fields");
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse(BuildPlainLduj(
            1,
            62,
            -3,
            4,
            5,
            1,
            7,
            "2|10|11|12|3|456|7|-8|Pet|Title|1|0|extra")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "lDuJ",
        "lDuJ payload 13 fields");
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse(BuildPlainLduj(
            1,
            62,
            -3,
            4,
            5,
            1,
            7,
            "2|_|11|12|3|456|7|-8|Pet|Title|1|0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lDuJ",
        "lDuJ payload bad objectIndex");
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse(BuildPlainLduj(
            1,
            62,
            -3,
            4,
            5,
            1,
            7,
            "x|10|11|12|3|456|7|-8|Pet|Title|1|0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lDuJ",
        "lDuJ payload bad decimal");
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse(BuildPlainLduj(
            1,
            62,
            -3,
            4,
            5,
            1,
            7,
            "2147483648|10|11|12|3|456|7|-8|Pet|Title|1|0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lDuJ",
        "lDuJ payload decimal overflow");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestFlagZeroRoundtrip();
    failures += TestPayloadRoundtrip();
    failures += TestBuildFromValue();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
