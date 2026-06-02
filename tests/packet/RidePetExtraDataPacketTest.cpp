#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::receive::RidePetExtraData::Spec;
}

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, PacketSpec());
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, PacketSpec());
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

std::string BuildPlainAU(int petIndex, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPetIndex = cg::packet::EncodeInt62(petIndex);
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPetIndex.HasValue() || !encodedPayload.HasValue()) {
        return "AU  ";
    }

    std::string plain = "AU ";
    plain.append(encodedPetIndex.Value());
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

std::string BuildAllPayload()
{
    return "1|10|-2|3|4|5|6|7|8|9|10|11|12|13|14|15|16|17|18|19|";
}

std::string BuildUnknownBitPayload()
{
    const cg::packet::PacketResult<std::string> flags = cg::packet::EncodeInt62(1 << 20);
    if (!flags.HasValue()) {
        return "";
    }

    std::string payload = flags.Value();
    payload.append("|10|");
    return payload;
}

int ExpectRidePetExtraPacket(
    const cg::packet::Packet& packet,
    int petIndex,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::RidePetExtraData::Code), message);
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
    failures += Expect(packet.fields[1].value == data, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlainAU(2, "0"));

    int failures = Expect(static_cast<bool>(parsed), "parse AU outer");
    if (parsed.HasValue()) {
        failures += ExpectRidePetExtraPacket(parsed.Value(), 2, "0", "parse AU outer packet");
    }
    return failures;
}

int TestClearPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::RidePetExtraData> parsed =
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "0"));

    int failures = Expect(static_cast<bool>(parsed), "parse AU clear");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().petIndex == 2, "parse AU clear petIndex");
    failures += Expect(parsed.Value().payload.clear, "parse AU clear flag");
    failures += Expect(parsed.Value().BuildData() == "0", "build AU clear payload");
    failures += ExpectRidePetExtraPacket(parsed.Value().ToPacket(), 2, "0", "AU clear ToPacket");
    return failures;
}

int TestPartialPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::RidePetExtraData> parsed =
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "6|10|-2|"));

    int failures = Expect(static_cast<bool>(parsed), "parse AU partial");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().petIndex == 2, "parse AU partial petIndex");
    failures += Expect(!parsed.Value().payload.clear, "parse AU partial not clear");
    failures += Expect(parsed.Value().payload.flags == 6, "parse AU partial flags");
    failures += Expect(parsed.Value().payload.vital == 10, "parse AU partial vital");
    failures += Expect(parsed.Value().payload.strength == -2, "parse AU partial strength");
    failures += Expect(parsed.Value().BuildData() == "6|10|-2|", "build AU partial payload");
    return failures;
}

int TestAllPayload()
{
    const std::string payload = BuildAllPayload();
    const cg::packet::PacketResult<cg::packet::receive::RidePetExtraData> parsed =
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(1, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse AU all marker");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().payload.flags == 1, "parse AU all flags");
    failures += Expect(parsed.Value().payload.vital == 10, "parse AU all vital");
    failures += Expect(parsed.Value().payload.strength == -2, "parse AU all strength");
    failures += Expect(parsed.Value().payload.tough == 3, "parse AU all tough");
    failures += Expect(parsed.Value().payload.quick == 4, "parse AU all quick");
    failures += Expect(parsed.Value().payload.magic == 5, "parse AU all magic");
    failures += Expect(parsed.Value().payload.critical == 6, "parse AU all critical");
    failures += Expect(parsed.Value().payload.counter == 7, "parse AU all counter");
    failures += Expect(parsed.Value().payload.hitRate == 8, "parse AU all hitRate");
    failures += Expect(parsed.Value().payload.avoid == 9, "parse AU all avoid");
    failures += Expect(parsed.Value().payload.poison == 10, "parse AU all poison");
    failures += Expect(parsed.Value().payload.sleep == 11, "parse AU all sleep");
    failures += Expect(parsed.Value().payload.stone == 12, "parse AU all stone");
    failures += Expect(parsed.Value().payload.drunk == 13, "parse AU all drunk");
    failures += Expect(parsed.Value().payload.confusion == 14, "parse AU all confusion");
    failures += Expect(parsed.Value().payload.amnesia == 15, "parse AU all amnesia");
    failures += Expect(parsed.Value().payload.petSeqNo == 16, "parse AU all petSeqNo");
    failures += Expect(parsed.Value().payload.weight == 17, "parse AU all weight");
    failures += Expect(parsed.Value().payload.maxWeight == 18, "parse AU all maxWeight");
    failures += Expect(parsed.Value().payload.itemHave == 19, "parse AU all itemHave");
    failures += Expect(parsed.Value().BuildData() == payload, "build AU all payload");
    return failures;
}

int TestRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::receive::RidePetExtraData> parsed =
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "6|10|-2|"));

    int failures = Expect(static_cast<bool>(parsed), "parse AU roundtrip source");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build AU packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainAU(2, "6|10|-2|"), "build AU packet value");
    const cg::packet::PacketResult<cg::packet::receive::RidePetExtraData> roundtrip =
        cg::packet::receive::RidePetExtraData::Parse(built.Value());
    failures += Expect(static_cast<bool>(roundtrip), "roundtrip AU packet");
    if (roundtrip.HasValue()) {
        failures += Expect(roundtrip.Value().payload.vital == 10, "roundtrip AU vital");
        failures += Expect(roundtrip.Value().payload.strength == -2, "roundtrip AU strength");
    }
    return failures;
}

int TestBuildPacketShape()
{
    cg::packet::receive::RidePetExtraPayload payload;
    payload.flags = 6;
    payload.vital = 10;
    payload.strength = -2;
    const cg::packet::receive::RidePetExtraData ridePetExtraData(2, payload);

    int failures = ExpectRidePetExtraPacket(ridePetExtraData.ToPacket(), 2, "6|10|-2|", "AU ToPacket partial");
    const cg::packet::PacketResult<std::string> built = BuildPacket(ridePetExtraData.ToPacket());
    failures += Expect(static_cast<bool>(built), "build AU partial packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainAU(2, "6|10|-2|"), "build AU partial value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse("IsSy 2 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "IsSy",
        "AU code mismatch");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse("AU 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "AU",
        "AU outer count missing");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse("AU 2 0 extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "AU",
        "AU outer count extra");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse("AU _ 0 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "AU",
        "AU bad petIndex");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "")),
        cg::packet::PacketErrorCode::EmptyInput,
        "AU",
        "AU empty data");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "0|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "AU",
        "AU zero flags not literal");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "6|10|-2")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "AU",
        "AU missing trailing delimiter");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "AU",
        "AU empty flags");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "_|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "AU",
        "AU bad flags");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "6|10|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "AU",
        "AU field count missing");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "6|10|-2|3|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "AU",
        "AU field count extra");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "6|10||")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "AU",
        "AU empty numeric");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "6|10|A|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "AU",
        "AU bad numeric");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "6|2147483648|-2|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "AU",
        "AU numeric overflow");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "1|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "AU",
        "AU all marker missing fields");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, "3|10|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "AU",
        "AU partial bit0");
    failures += ExpectError(
        cg::packet::receive::RidePetExtraData::Parse(BuildPlainAU(2, BuildUnknownBitPayload())),
        cg::packet::PacketErrorCode::DecodeFailed,
        "AU",
        "AU unknown bit");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestClearPayload();
    failures += TestPartialPayload();
    failures += TestAllPayload();
    failures += TestRoundtrip();
    failures += TestBuildPacketShape();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
