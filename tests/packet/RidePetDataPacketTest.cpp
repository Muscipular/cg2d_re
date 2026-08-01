#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

constexpr int FullMarkerFlag = 1;
constexpr int HpFlag = 1 << 3;
constexpr int ForcePointFlag = 1 << 5;
constexpr int NameFlag = 1 << 22;
constexpr int UserPetNameFlag = 1 << 23;
constexpr int UnknownFlag = 1 << 26;

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::RidePetData::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::RidePetData::Spec);
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

std::string EncodeI62(int value)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeInt62(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string EncodeLegacy(std::string_view value)
{
    const cg::packet::PacketResult<std::string> encoded =
        cg::packet::EncodeLegacyPayloadText(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string BuildPlainIsSy(int petIndex, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload =
        cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        return "IsSy  ";
    }

    std::string plain = "IsSy ";
    plain.append(EncodeI62(petIndex));
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

std::string WithGbkTrailPipe(std::string_view prefix)
{
    std::string value(prefix);
    value.push_back(static_cast<char>(0x96));
    value.push_back(static_cast<char>(0x7C));
    return value;
}

void AppendPayloadField(std::string& data, std::string_view value)
{
    data.append(value);
    data.push_back('|');
}

cg::packet::receive::RidePetPayload BuildFullPayloadValue()
{
    cg::packet::receive::RidePetPayload payload;
    payload.fullMarker = true;
    payload.flags = FullMarkerFlag;
    payload.baseImageNumber = 101;
    payload.tribe = 2;
    payload.hp = 103;
    payload.maxHp = 104;
    payload.forcePoint = 105;
    payload.maxForcePoint = 106;
    payload.exp = 107;
    payload.nextExp = 108;
    payload.level = 9;
    payload.attack = 110;
    payload.defense = 111;
    payload.agility = 112;
    payload.magicPower = 113;
    payload.recovery = 114;
    payload.loyalty = 115;
    payload.earth = 1;
    payload.water = 2;
    payload.fire = 3;
    payload.wind = 4;
    payload.slot = 5;
    payload.changeNameFlag = 1;
    payload.name = WithGbkTrailPipe("Ride,Pet|Name\\Path\n");
    payload.userPetName = WithGbkTrailPipe("User|Pet,Name\\Tail\n");
    payload.departureBattleStatus = 6;
    payload.injury = 7;
    return payload;
}

std::string BuildFullPayloadData(const cg::packet::receive::RidePetPayload& payload)
{
    std::string data = "1|";
    AppendPayloadField(data, std::to_string(payload.baseImageNumber));
    AppendPayloadField(data, std::to_string(payload.tribe));
    AppendPayloadField(data, std::to_string(payload.hp));
    AppendPayloadField(data, std::to_string(payload.maxHp));
    AppendPayloadField(data, std::to_string(payload.forcePoint));
    AppendPayloadField(data, std::to_string(payload.maxForcePoint));
    AppendPayloadField(data, std::to_string(payload.exp));
    AppendPayloadField(data, std::to_string(payload.nextExp));
    AppendPayloadField(data, std::to_string(payload.level));
    AppendPayloadField(data, std::to_string(payload.attack));
    AppendPayloadField(data, std::to_string(payload.defense));
    AppendPayloadField(data, std::to_string(payload.agility));
    AppendPayloadField(data, std::to_string(payload.magicPower));
    AppendPayloadField(data, std::to_string(payload.recovery));
    AppendPayloadField(data, std::to_string(payload.loyalty));
    AppendPayloadField(data, std::to_string(payload.earth));
    AppendPayloadField(data, std::to_string(payload.water));
    AppendPayloadField(data, std::to_string(payload.fire));
    AppendPayloadField(data, std::to_string(payload.wind));
    AppendPayloadField(data, std::to_string(payload.slot));
    AppendPayloadField(data, std::to_string(payload.changeNameFlag));
    AppendPayloadField(data, EncodeLegacy(payload.name));
    AppendPayloadField(data, EncodeLegacy(payload.userPetName));
    AppendPayloadField(data, std::to_string(payload.departureBattleStatus));
    AppendPayloadField(data, std::to_string(payload.injury));
    return data;
}

std::string BuildPartialPayload(int flags, const std::vector<std::string>& fields)
{
    std::string data = EncodeI62(flags);
    data.push_back('|');
    for (const std::string& field : fields) {
        AppendPayloadField(data, field);
    }
    return data;
}

int ExpectRidePetPacket(
    const cg::packet::Packet& packet,
    int petIndex,
    std::string_view data,
    cg::packet::PacketFieldType dataType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::RidePetData::Code), message);
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
    failures += Expect(packet.fields[1].value == data, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string data = BuildPartialPayload(HpFlag | ForcePointFlag, {"123", "45"});
    int failures = Expect(
        cg::packet::receive::RidePetData::Fields[1U].type ==
            cg::packet::PacketFieldType::SubPayload,
        "IsSy data spec SubPayload");
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainIsSy(0, data));

    failures += Expect(static_cast<bool>(parsed), "parse IsSy outer");
    if (parsed.HasValue()) {
        failures += ExpectRidePetPacket(
            parsed.Value(),
            0,
            data,
            cg::packet::PacketFieldType::EscapedString,
            "parse IsSy outer packet");
    }
    return failures;
}

int TestClearPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::RidePetData> parsed =
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "0"));

    int failures = Expect(static_cast<bool>(parsed), "parse IsSy clear literal 0");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().payload.clear, "parse IsSy clear flag");
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "build IsSy clear data");
    if (data.HasValue()) {
        failures += Expect(data.Value() == "0", "build IsSy clear literal 0");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "IsSy clear ToPacket");
    if (packet.HasValue()) {
        failures += ExpectRidePetPacket(
            packet.Value(),
            0,
            "0",
            cg::packet::PacketFieldType::SubPayload,
            "IsSy clear ToPacket fields");
    }
    return failures;
}

int TestFullPayload()
{
    const cg::packet::receive::RidePetPayload expectedPayload = BuildFullPayloadValue();
    const std::string data = BuildFullPayloadData(expectedPayload);
    const cg::packet::PacketResult<cg::packet::receive::RidePetData> parsed =
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(3, data));

    int failures = Expect(static_cast<bool>(parsed), "parse IsSy full 25 fields");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().petIndex == 3, "parse IsSy full petIndex");
    failures += Expect(parsed.Value().payload.fullMarker, "parse IsSy full marker");
    failures += Expect(parsed.Value().payload.flags == FullMarkerFlag, "parse IsSy full flags");
    failures += Expect(parsed.Value().payload.baseImageNumber == 101, "parse IsSy full base image");
    failures += Expect(parsed.Value().payload.injury == 7, "parse IsSy full injury");
    failures += Expect(parsed.Value().payload.name == expectedPayload.name, "parse IsSy full name semantic GBK/legacy");
    failures += Expect(
        parsed.Value().payload.userPetName == expectedPayload.userPetName,
        "parse IsSy full userPetName semantic GBK/legacy");

    const cg::packet::PacketResult<std::string> builtData = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(builtData), "build IsSy full data");
    if (builtData.HasValue()) {
        failures += Expect(builtData.Value() == data, "roundtrip IsSy full data");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "IsSy full ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectRidePetPacket(
        packet.Value(),
        3,
        data,
        cg::packet::PacketFieldType::SubPayload,
        "IsSy full ToPacket fields");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build IsSy full packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainIsSy(3, data), "roundtrip IsSy full packet");
    }
    return failures;
}

int TestPartialPayload()
{
    const int flags = HpFlag | ForcePointFlag | NameFlag | UserPetNameFlag;
    const std::string name = WithGbkTrailPipe("Partial,Name|Tail\\Line\n");
    const std::string userPetName = WithGbkTrailPipe("User|Partial,Name");
    const std::string data = BuildPartialPayload(
        flags,
        {"123", "45", EncodeLegacy(name), EncodeLegacy(userPetName)});
    const cg::packet::PacketResult<cg::packet::receive::RidePetData> parsed =
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(2, data));

    int failures = Expect(static_cast<bool>(parsed), "parse IsSy partial");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(!parsed.Value().payload.fullMarker, "parse IsSy partial not full");
    failures += Expect(parsed.Value().payload.flags == flags, "parse IsSy partial flags");
    failures += Expect(parsed.Value().payload.hp == 123, "parse IsSy partial hp");
    failures += Expect(parsed.Value().payload.forcePoint == 45, "parse IsSy partial forcePoint");
    failures += Expect(parsed.Value().payload.name == name, "parse IsSy partial name semantic bytes");
    failures += Expect(
        parsed.Value().payload.userPetName == userPetName,
        "parse IsSy partial userPetName semantic bytes");

    const cg::packet::PacketResult<std::string> builtData = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(builtData), "build IsSy partial data");
    if (builtData.HasValue()) {
        failures += Expect(builtData.Value() == data, "roundtrip IsSy partial data");
    }
    return failures;
}

int TestFullMarkerFactory()
{
    const cg::packet::receive::RidePetData value(
        0,
        cg::packet::receive::RidePetPayload::FullMarker());
    const cg::packet::PacketResult<std::string> data = value.BuildData();

    int failures = Expect(static_cast<bool>(data), "IsSy FullMarker factory BuildData");
    if (!data.HasValue()) {
        return failures;
    }
    const cg::packet::PacketResult<std::vector<std::string>> parts =
        cg::packet::SplitPayloadLegacyAware(
            data.Value(),
            cg::packet::receive::RidePetData::PayloadDelimiter);
    failures += Expect(static_cast<bool>(parts), "IsSy FullMarker factory split");
    if (parts.HasValue()) {
        failures += Expect(parts.Value().size() == 27U, "IsSy FullMarker factory flags+25+tail");
        failures += Expect(parts.Value().back().empty(), "IsSy FullMarker factory trailing delimiter");
    }
    return failures;
}

int TestErrors()
{
    const std::string fullData = BuildFullPayloadData(BuildFullPayloadValue());
    std::string fullMissingTail = fullData;
    fullMissingTail.pop_back();
    std::string fullExtra = fullData;
    fullExtra.append("extra|");
    const std::string partialData = BuildPartialPayload(HpFlag | ForcePointFlag, {"123", "45"});

    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::RidePetData::Parse("OBee 0 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "OBee",
        "IsSy code mismatch");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse("IsSy 0"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IsSy",
        1U,
        "IsSy outer count");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse("IsSy _ 0 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        0U,
        "IsSy bad petIndex");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "")),
        cg::packet::PacketErrorCode::EmptyInput,
        "IsSy",
        1U,
        "IsSy empty data");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, fullMissingTail)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        1U,
        "IsSy missing trailing delimiter");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "_|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        1U,
        "IsSy bad flags");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "0|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        1U,
        "IsSy flags0 active rejected");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, BuildPartialPayload(3, {"123"}))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        1U,
        "IsSy partial bit0 mixed");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, BuildPartialPayload(UnknownFlag, {"123"}))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        1U,
        "IsSy unknown bit");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "1|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IsSy",
        1U,
        "IsSy full marker truncated");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "E|123|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IsSy",
        1U,
        "IsSy partial truncated");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, partialData + "extra|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IsSy",
        1U,
        "IsSy partial extra field");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, fullExtra)),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IsSy",
        1U,
        "IsSy full extra field");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "E|abc|45|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        1U,
        "IsSy bad numeric");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(0, "E|2147483648|45|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        1U,
        "IsSy numeric overflow");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(
            0,
            BuildPartialPayload(NameFlag, {"name\\"}))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        22U,
        "IsSy name legacy truncated");
    failures += ExpectFieldError(
        cg::packet::receive::RidePetData::Parse(BuildPlainIsSy(
            0,
            BuildPartialPayload(UserPetNameFlag, {"user\\"}))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IsSy",
        23U,
        "IsSy userPetName legacy truncated");

    cg::packet::receive::RidePetPayload flags0Payload;
    const cg::packet::receive::RidePetData flags0Value(0, flags0Payload);
    failures += ExpectFieldError(
        flags0Value.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IsSy",
        1U,
        "IsSy build flags0 active");
    failures += ExpectFieldError(
        flags0Value.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IsSy",
        1U,
        "IsSy ToPacket flags0 propagation");

    cg::packet::receive::RidePetPayload mixedPayload;
    mixedPayload.flags = 3;
    const cg::packet::receive::RidePetData mixedValue(0, mixedPayload);
    failures += ExpectFieldError(
        mixedValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IsSy",
        1U,
        "IsSy build partial bit0 mixed");

    cg::packet::receive::RidePetPayload unknownPayload;
    unknownPayload.flags = UnknownFlag;
    const cg::packet::receive::RidePetData unknownValue(0, unknownPayload);
    failures += ExpectFieldError(
        unknownValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IsSy",
        1U,
        "IsSy build unknown bit");

    cg::packet::receive::RidePetPayload badNamePayload;
    badNamePayload.flags = NameFlag;
    badNamePayload.name.assign(1U, static_cast<char>(0x96));
    const cg::packet::receive::RidePetData badNameValue(0, std::move(badNamePayload));
    failures += ExpectFieldError(
        badNameValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IsSy",
        22U,
        "IsSy build name truncated GBK");

    cg::packet::receive::RidePetPayload badUserNamePayload;
    badUserNamePayload.flags = UserPetNameFlag;
    badUserNamePayload.userPetName.assign(1U, static_cast<char>(0x96));
    const cg::packet::receive::RidePetData badUserNameValue(0, std::move(badUserNamePayload));
    failures += ExpectFieldError(
        badUserNameValue.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IsSy",
        23U,
        "IsSy build userPetName truncated GBK");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestClearPayload();
    failures += TestFullPayload();
    failures += TestPartialPayload();
    failures += TestFullMarkerFactory();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
