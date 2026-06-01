#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <utility>

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

std::string BuildPlainIsSy(int petIndex, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPetIndex = cg::packet::EncodeInt62(petIndex);
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPetIndex.HasValue() || !encodedPayload.HasValue()) {
        return "IsSy  ";
    }

    std::string plain = "IsSy ";
    plain.append(encodedPetIndex.Value());
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

std::string BuildPartialNamePayload(std::string_view name)
{
    const cg::packet::PacketResult<std::string> flags = cg::packet::EncodeInt62(1 << 22);
    if (!flags.HasValue()) {
        return "";
    }

    std::string payload = flags.Value();
    payload.push_back('|');
    payload.append(name);
    payload.push_back('|');
    return payload;
}

int ExpectRidePetPacket(
    const cg::packet::Packet& packet,
    int petIndex,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::RidePetData::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "petIndex", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields[0].value == std::to_string(petIndex), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields[1].value == data, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(BuildPlainIsSy(0, "E|123|45|"));

    int failures = Expect(static_cast<bool>(parsed), "parse IsSy outer");
    if (parsed.HasValue()) {
        failures += ExpectRidePetPacket(parsed.Value(), 0, "E|123|45|", "parse IsSy outer packet");
    }
    return failures;
}

int TestClearPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::RidePetData> parsed =
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "0"));

    int failures = Expect(static_cast<bool>(parsed), "parse IsSy clear");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().petIndex == 0, "parse IsSy clear petIndex");
    failures += Expect(parsed.Value().payload.clear, "parse IsSy clear flag");
    failures += Expect(!parsed.Value().payload.fullMarker, "parse IsSy clear not full");
    failures += Expect(parsed.Value().BuildData() == "0", "build IsSy clear payload");
    failures += ExpectRidePetPacket(parsed.Value().ToPacket(), 0, "0", "IsSy clear ToPacket");
    return failures;
}

int TestFullMarkerPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::RidePetData> parsed =
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "1|"));

    int failures = Expect(static_cast<bool>(parsed), "parse IsSy full marker");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().petIndex == 0, "parse IsSy full petIndex");
    failures += Expect(!parsed.Value().payload.clear, "parse IsSy full not clear");
    failures += Expect(parsed.Value().payload.fullMarker, "parse IsSy full flag");
    failures += Expect(parsed.Value().payload.flags == 1, "parse IsSy full flags");
    failures += Expect(parsed.Value().BuildData() == "1|", "build IsSy full payload");
    return failures;
}

int TestPartialPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::RidePetData> parsed =
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(2, "E|123|45|"));

    int failures = Expect(static_cast<bool>(parsed), "parse IsSy partial");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().petIndex == 2, "parse IsSy partial petIndex");
    failures += Expect(!parsed.Value().payload.clear, "parse IsSy partial not clear");
    failures += Expect(!parsed.Value().payload.fullMarker, "parse IsSy partial not full");
    failures += Expect(parsed.Value().payload.flags == 40, "parse IsSy partial flags");
    failures += Expect(parsed.Value().payload.hp == 123, "parse IsSy partial hp");
    failures += Expect(parsed.Value().payload.forcePoint == 45, "parse IsSy partial forcePoint");
    failures += Expect(parsed.Value().BuildData() == "E|123|45|", "build IsSy partial payload");
    return failures;
}

int TestOuterEscapedName()
{
    const std::string payload = BuildPartialNamePayload("Fast Pet\\nRaw");
    const cg::packet::PacketResult<cg::packet::receive::RidePetData> parsed =
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(1, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse IsSy escaped name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().payload.name == "Fast Pet\\nRaw", "parse IsSy escaped name value");
    failures += Expect(parsed.Value().BuildData() == payload, "build IsSy escaped name payload");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build IsSy escaped name packet");
    if (built.HasValue()) {
        const cg::packet::PacketResult<cg::packet::receive::RidePetData> roundtrip =
            cg::packet::receive::RidePetData::Parse(built.Value());
        failures += Expect(static_cast<bool>(roundtrip), "roundtrip IsSy escaped name packet");
        if (roundtrip.HasValue()) {
            failures += Expect(roundtrip.Value().payload.name == "Fast Pet\\nRaw", "roundtrip IsSy name");
        }
    }
    return failures;
}

int TestBuildPacketShape()
{
    cg::packet::receive::RidePetPayload payload;
    payload.flags = 40;
    payload.hp = 123;
    payload.forcePoint = 45;
    const cg::packet::receive::RidePetData ridePetData(0, std::move(payload));

    int failures = ExpectRidePetPacket(ridePetData.ToPacket(), 0, "E|123|45|", "IsSy ToPacket partial");
    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(ridePetData.ToPacket());
    failures += Expect(static_cast<bool>(built), "build IsSy partial packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainIsSy(0, "E|123|45|"), "build IsSy partial value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse("OBee 0 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "OBee",
        "IsSy code mismatch");
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse("IsSy 0"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IsSy",
        "IsSy outer count");
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse("IsSy _ 0 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        "IsSy bad petIndex");
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "")),
        cg::packet::PacketErrorCode::EmptyInput,
        "IsSy",
        "IsSy empty data");
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "E|123|45")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        "IsSy missing trailing delimiter");
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "_|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        "IsSy bad flags");
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "0|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        "IsSy zero flags not literal");
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "9|123|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        "IsSy unknown bit");
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "E|123|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IsSy",
        "IsSy active field count");
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "1|123|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IsSy",
        "IsSy full marker field count");
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "E|abc|45|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        "IsSy bad numeric");
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "E|2147483648|45|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        "IsSy numeric overflow");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestClearPayload();
    failures += TestFullMarkerPayload();
    failures += TestPartialPayload();
    failures += TestOuterEscapedName();
    failures += TestBuildPacketShape();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
