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
        failures += Expect(error.fieldIndex.value() == fieldIndex, message);
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

std::string EncodeLegacy(std::string_view value)
{
    const cg::packet::PacketResult<std::string> encoded =
        cg::packet::EncodeLegacyPayloadText(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string JoinPayload(const std::vector<std::string>& parts)
{
    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPayload(
        std::span<const std::string>(parts.data(), parts.size()),
        cg::packet::receive::PetMailAnimation::PayloadDelimiter);
    return built.HasValue() ? built.Value() : std::string();
}

std::string WithGbkTrailPipe(std::string_view prefix)
{
    std::string value(prefix);
    value.push_back(static_cast<char>(0x81));
    value.push_back(static_cast<char>(0x7C));
    return value;
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
    payload.nameRaw = WithGbkTrailPipe("Pet,Name|Path\\Line\n");
    payload.titleData.freeName = WithGbkTrailPipe("Free\\z");
    payload.titleData.guildName = WithGbkTrailPipe("Guild,Name|");
    payload.titleData.guildTitle = "Title\\Path\nLine|End";
    payload.flg1 = 1;
    payload.flg2 = 0;
    payload.ignoredTailRaw = {
        "server-tail-0",
        WithGbkTrailPipe("server-tail-1"),
        "raw\\z",
        "",
        "-1",
        "server-tail-5",
    };
    return payload;
}

std::vector<std::string> BuildPayloadParts(
    const cg::packet::receive::PetMailAnimationPayload& payload)
{
    const std::vector<std::string> titleParts = {
        payload.titleData.freeName,
        EncodeLegacy(payload.titleData.guildName),
        EncodeLegacy(payload.titleData.guildTitle),
    };

    return {
        std::to_string(payload.typeMb),
        EncodeI62(payload.objectIndex),
        std::to_string(payload.x),
        std::to_string(payload.y),
        std::to_string(payload.dir),
        std::to_string(payload.imageNo),
        std::to_string(payload.lv),
        std::to_string(payload.field40),
        EncodeLegacy(payload.nameRaw),
        EncodeLegacy(JoinPayload(titleParts)),
        std::to_string(payload.flg1),
        std::to_string(payload.flg2),
    };
}

std::string BuildPayloadData(const cg::packet::receive::PetMailAnimationPayload& payload)
{
    std::vector<std::string> parts = BuildPayloadParts(payload);
    parts.insert(parts.end(), payload.ignoredTailRaw.begin(), payload.ignoredTailRaw.end());
    return JoinPayload(parts);
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
    cg::packet::PacketFieldType dataType,
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
    failures += ExpectField(packet, 7U, "data", dataType, data, message);
    return failures;
}

int TestParsePacketOuter()
{
    int failures = Expect(
        cg::packet::receive::PetMailAnimation::Fields[7U].type ==
            cg::packet::PacketFieldType::SubPayload,
        "lDuJ data spec SubPayload");
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainLduj(1, 62, -3, 4, 5, 0, 7, ""));

    failures += Expect(static_cast<bool>(parsed), "parse lDuJ outer");
    if (parsed.HasValue()) {
        failures += ExpectPetMailAnimationPacket(
            parsed.Value(),
            0,
            "",
            cg::packet::PacketFieldType::EscapedString,
            "parse lDuJ outer packet");
    }
    return failures;
}

int TestFlagZeroRoundtrip()
{
    const std::string opaqueData = WithGbkTrailPipe("opaque\\z|data");
    const cg::packet::PacketResult<cg::packet::receive::PetMailAnimation> parsed =
        cg::packet::receive::PetMailAnimation::Parse(
            BuildPlainLduj(1, 62, -3, 4, 5, 0, 7, opaqueData));

    int failures = Expect(static_cast<bool>(parsed), "parse lDuJ flg 0");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().objIndex == 1, "lDuJ flg 0 objIndex");
    failures += Expect(parsed.Value().graphicsNo == 62, "lDuJ flg 0 graphicsNo");
    failures += Expect(parsed.Value().x == -3, "lDuJ flg 0 x");
    failures += Expect(parsed.Value().flg == 0, "lDuJ flg 0 flg");
    failures += Expect(!parsed.Value().hasPayload, "lDuJ flg 0 no payload");
    failures += Expect(parsed.Value().data == opaqueData, "lDuJ flg 0 opaque data");

    const cg::packet::PacketResult<std::string> builtData = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(builtData), "lDuJ flg 0 BuildData");
    if (builtData.HasValue()) {
        failures += Expect(builtData.Value() == opaqueData, "lDuJ flg 0 BuildData opaque");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "lDuJ flg 0 ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectPetMailAnimationPacket(
        packet.Value(),
        0,
        opaqueData,
        cg::packet::PacketFieldType::SubPayload,
        "lDuJ flg 0 ToPacket fields");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build lDuJ flg 0");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == BuildPlainLduj(1, 62, -3, 4, 5, 0, 7, opaqueData),
            "roundtrip lDuJ flg 0");
    }
    return failures;
}

int TestPayloadRoundtrip()
{
    const cg::packet::receive::PetMailAnimationPayload expectedPayload = BuildPayloadValue();
    const std::string data = BuildPayloadData(expectedPayload);
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
    failures += Expect(
        parsed.Value().payload.nameRaw == expectedPayload.nameRaw,
        "lDuJ payload name semantic bytes");
    failures += Expect(
        parsed.Value().payload.titleData.freeName == expectedPayload.titleData.freeName,
        "lDuJ payload title freeName outer-only");
    failures += Expect(
        parsed.Value().payload.titleData.guildName == expectedPayload.titleData.guildName,
        "lDuJ payload title guildName inner legacy");
    failures += Expect(
        parsed.Value().payload.titleData.guildTitle == expectedPayload.titleData.guildTitle,
        "lDuJ payload title guildTitle inner legacy");
    failures += Expect(
        parsed.Value().payload.ignoredTailRaw == expectedPayload.ignoredTailRaw,
        "lDuJ payload 18-field Server tail preserved");

    const cg::packet::PacketResult<std::string> builtData = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(builtData), "lDuJ payload BuildData");
    if (builtData.HasValue()) {
        failures += Expect(builtData.Value() == data, "lDuJ payload BuildData bytes");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "lDuJ payload ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build lDuJ payload");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainLduj(1, 62, -3, 4, 5, 1, 7, data), "roundtrip lDuJ payload");
    }
    return failures;
}

int TestBuildFromValue()
{
    const cg::packet::receive::PetMailAnimationPayload payload = BuildPayloadValue();
    const std::string data = BuildPayloadData(payload);
    const cg::packet::receive::PetMailAnimation packetValue(1, 62, -3, 4, 5, 1, 7, payload);
    const cg::packet::PacketResult<cg::packet::Packet> packet = packetValue.ToPacket();

    int failures = Expect(static_cast<bool>(packet), "lDuJ value ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += ExpectPetMailAnimationPacket(
        packet.Value(),
        1,
        data,
        cg::packet::PacketFieldType::SubPayload,
        "lDuJ ToPacket");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build lDuJ value");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == BuildPlainLduj(1, 62, -3, 4, 5, 1, 7, data),
            "build lDuJ value text");
    }
    return failures;
}

int TestThirteenFieldsAccepted()
{
    cg::packet::receive::PetMailAnimationPayload expectedPayload = BuildPayloadValue();
    expectedPayload.ignoredTailRaw = {"extra"};
    const std::string data = BuildPayloadData(expectedPayload);
    const cg::packet::PacketResult<cg::packet::receive::PetMailAnimation> parsed =
        cg::packet::receive::PetMailAnimation::Parse(
            BuildPlainLduj(1, 62, -3, 4, 5, 1, 7, data));

    int failures = Expect(static_cast<bool>(parsed), "lDuJ payload 13 fields accepted");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(
        parsed.Value().payload.ignoredTailRaw == expectedPayload.ignoredTailRaw,
        "lDuJ payload 13th field preserved");
    const cg::packet::PacketResult<std::string> builtData = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(builtData), "lDuJ payload 13 fields BuildData");
    if (builtData.HasValue()) {
        failures += Expect(builtData.Value() == data, "lDuJ payload 13 fields roundtrip");
    }
    return failures;
}

int TestErrors()
{
    const cg::packet::receive::PetMailAnimationPayload validPayload = BuildPayloadValue();
    std::vector<std::string> elevenParts = BuildPayloadParts(validPayload);
    elevenParts.pop_back();

    std::vector<std::string> badObjectIndexParts = BuildPayloadParts(validPayload);
    badObjectIndexParts[1U] = "_";
    std::vector<std::string> badDecimalParts = BuildPayloadParts(validPayload);
    badDecimalParts[0U] = "x";
    std::vector<std::string> overflowParts = BuildPayloadParts(validPayload);
    overflowParts[0U] = "2147483648";
    std::vector<std::string> badNameParts = BuildPayloadParts(validPayload);
    badNameParts[8U] = "name\\";
    std::vector<std::string> badTitleArityParts = BuildPayloadParts(validPayload);
    badTitleArityParts[9U] = EncodeLegacy("free|guild");
    std::vector<std::string> truncatedTitleParts = BuildPayloadParts(validPayload);
    truncatedTitleParts[9U] = "free\\zbad\\y\\ztitle";

    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse("lDuJ 1 10 -3 4 5 0 7"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "lDuJ",
        7U,
        "lDuJ missing data");
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse(BuildPlainLduj(1, 62, -3, 4, 5, 1, 7, "")),
        cg::packet::PacketErrorCode::EmptyInput,
        "lDuJ",
        7U,
        "lDuJ nonzero flg empty payload");
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse("lDuJ _ 10 -3 4 5 0 7  "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lDuJ",
        0U,
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
            JoinPayload(elevenParts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "lDuJ",
        7U,
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
            JoinPayload(badObjectIndexParts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lDuJ",
        1U,
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
            JoinPayload(badDecimalParts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lDuJ",
        0U,
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
            JoinPayload(overflowParts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lDuJ",
        0U,
        "lDuJ payload decimal overflow");
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse(BuildPlainLduj(
            1,
            62,
            -3,
            4,
            5,
            1,
            7,
            JoinPayload(badNameParts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lDuJ",
        8U,
        "lDuJ payload name truncated legacy");
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse(BuildPlainLduj(
            1,
            62,
            -3,
            4,
            5,
            1,
            7,
            JoinPayload(badTitleArityParts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "lDuJ",
        9U,
        "lDuJ payload title exact3");
    failures += ExpectError(
        cg::packet::receive::PetMailAnimation::Parse(BuildPlainLduj(
            1,
            62,
            -3,
            4,
            5,
            1,
            7,
            JoinPayload(truncatedTitleParts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "lDuJ",
        9U,
        "lDuJ payload title inner legacy truncated");

    const cg::packet::receive::PetMailAnimation rawNonzero(
        1, 62, -3, 4, 5, 1, 7, std::string("opaque"));
    failures += ExpectError(
        rawNonzero.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "lDuJ",
        7U,
        "lDuJ raw nonzero flg missing payload");
    failures += ExpectError(
        rawNonzero.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "lDuJ",
        7U,
        "lDuJ raw nonzero ToPacket error propagation");

    cg::packet::receive::PetMailAnimationPayload badNamePayload = validPayload;
    badNamePayload.nameRaw.assign(1U, static_cast<char>(0x81));
    const cg::packet::receive::PetMailAnimation badNameValue(
        1, 62, -3, 4, 5, 1, 7, std::move(badNamePayload));
    failures += ExpectError(
        badNameValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "lDuJ",
        8U,
        "lDuJ build name truncated GBK");

    cg::packet::receive::PetMailAnimationPayload badTitlePayload = validPayload;
    badTitlePayload.titleData.guildName.assign(1U, static_cast<char>(0x81));
    const cg::packet::receive::PetMailAnimation badTitleValue(
        1, 62, -3, 4, 5, 1, 7, std::move(badTitlePayload));
    failures += ExpectError(
        badTitleValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "lDuJ",
        9U,
        "lDuJ build title truncated GBK");

    cg::packet::receive::PetMailAnimationPayload ambiguousTitlePayload = validPayload;
    ambiguousTitlePayload.titleData.freeName = "free|name";
    const cg::packet::receive::PetMailAnimation ambiguousTitleValue(
        1, 62, -3, 4, 5, 1, 7, std::move(ambiguousTitlePayload));
    failures += ExpectError(
        ambiguousTitleValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "lDuJ",
        9U,
        "lDuJ build title freeName logical delimiter rejected");
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
    failures += TestThirteenFieldsAccepted();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
