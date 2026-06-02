#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::receive::MapData::Spec;
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

std::string EncodeI62(int value)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeInt62(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string BuildPlainAbs(
    int mapId,
    int floor,
    int x1,
    int y1,
    int x2,
    int y2,
    std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        return "aBS ";
    }

    const std::vector<std::string> tokens = {
        "aBS",
        EncodeI62(mapId),
        EncodeI62(floor),
        EncodeI62(x1),
        EncodeI62(y1),
        EncodeI62(x2),
        EncodeI62(y2),
        encodedPayload.Value(),
    };

    const cg::packet::PacketResult<std::string> plain = cg::packet::BuildPlainTokenString(tokens);
    return plain.HasValue() ? plain.Value() : std::string();
}

int ExpectMapDataPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::MapData::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 7U, message);
    if (packet.fields.size() != 7U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "mapid", message);
    failures += Expect(packet.fields[1].name == "floor", message);
    failures += Expect(packet.fields[2].name == "x1", message);
    failures += Expect(packet.fields[3].name == "y1", message);
    failures += Expect(packet.fields[4].name == "x2", message);
    failures += Expect(packet.fields[5].name == "y2", message);
    failures += Expect(packet.fields[6].name == "data", message);
    for (std::size_t index = 0; index < 6U; ++index) {
        failures += Expect(packet.fields[index].type == cg::packet::PacketFieldType::Int62, message);
    }
    failures += Expect(packet.fields[6].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[6].value == value, message);
    return failures;
}

int ExpectMapData(
    const cg::packet::receive::MapData& mapData,
    const std::vector<int>& tileData,
    const std::vector<int>& objectData,
    const std::vector<int>& eventDataRaw,
    const char* message)
{
    int failures = 0;
    failures += Expect(mapData.mapId == 1, message);
    failures += Expect(mapData.floor == 2, message);
    failures += Expect(mapData.x1 == 1, message);
    failures += Expect(mapData.y1 == 2, message);
    failures += Expect(mapData.x2 == 3, message);
    failures += Expect(mapData.y2 == 4, message);
    failures += Expect(mapData.tileData == tileData, message);
    failures += Expect(mapData.objectData == objectData, message);
    failures += Expect(mapData.eventDataRaw == eventDataRaw, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "0,1,2,3|4,0,5,0|6,7,0,8";
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainAbs(1, 2, 1, 2, 3, 4, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aBS outer");
    if (parsed.HasValue()) {
        failures += ExpectMapDataPacket(parsed.Value(), payload, "parse aBS outer packet");
    }
    return failures;
}

int TestTypedParse2x2()
{
    const std::string payload = "0,1,2,3|4,0,5,0|6,7,0,8";
    const cg::packet::PacketResult<cg::packet::receive::MapData> parsed =
        cg::packet::receive::MapData::Parse(BuildPlainAbs(1, 2, 1, 2, 3, 4, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aBS 2x2");
    if (parsed.HasValue()) {
        failures += ExpectMapData(
            parsed.Value(),
            {0, 1, 2, 3},
            {4, 0, 5, 0},
            {6, 7, 0, 8},
            "parse aBS 2x2 data");
    }
    return failures;
}

int TestEmptyObjectEventCells()
{
    const std::string payload = "0,1,2,3|,2,,4|5,,7,";
    const cg::packet::PacketResult<cg::packet::receive::MapData> parsed =
        cg::packet::receive::MapData::Parse(BuildPlainAbs(1, 2, 1, 2, 3, 4, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aBS empty object event cells");
    if (parsed.HasValue()) {
        failures += ExpectMapData(
            parsed.Value(),
            {0, 1, 2, 3},
            {0, 2, 0, 4},
            {5, 0, 7, 0},
            "parse aBS empty object event values");
    }
    return failures;
}

int TestToPacketBuildSeparators()
{
    const std::string payload = "0,1,2,3|,2,,4|5,,7,";
    const cg::packet::receive::MapData mapData(
        1,
        2,
        1,
        2,
        3,
        4,
        {0, 1, 2, 3},
        {0, 2, 0, 4},
        {5, 0, 7, 0});

    int failures = Expect(mapData.BuildData() == payload, "build aBS payload separators");
    const cg::packet::Packet packet = mapData.ToPacket();
    failures += ExpectMapDataPacket(packet, payload, "aBS ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build aBS packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainAbs(1, 2, 1, 2, 3, 4, payload), "build aBS packet value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::MapData::Parse(BuildPlainAbs(1, 2, 1, 2, 3, 4, "0,1,2,3|1,2,3,4")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aBS",
        "aBS payload not three layers");
    failures += ExpectError(
        cg::packet::receive::MapData::Parse(BuildPlainAbs(1, 2, 1, 2, 3, 4, "0,1,2|1,2,3,4|1,2,3,4")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aBS",
        "aBS payload area mismatch");
    failures += ExpectError(
        cg::packet::receive::MapData::Parse(BuildPlainAbs(1, 2, 1, 2, 3, 4, "0,!,2,3|1,2,3,4|1,2,3,4")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "aBS",
        "aBS payload bad i62");
    return failures;
}

int TestZeroAreaEmptyPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::MapData> parsed =
        cg::packet::receive::MapData::Parse(BuildPlainAbs(1, 2, 1, 2, 1, 2, ""));

    int failures = Expect(static_cast<bool>(parsed), "parse aBS zero area empty payload");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().mapId == 1, "aBS zero area mapId");
        failures += Expect(parsed.Value().floor == 2, "aBS zero area floor");
        failures += Expect(parsed.Value().x1 == 1, "aBS zero area x1");
        failures += Expect(parsed.Value().y1 == 2, "aBS zero area y1");
        failures += Expect(parsed.Value().x2 == 1, "aBS zero area x2");
        failures += Expect(parsed.Value().y2 == 2, "aBS zero area y2");
        failures += Expect(parsed.Value().tileData.empty(), "aBS zero area tile");
        failures += Expect(parsed.Value().objectData.empty(), "aBS zero area object");
        failures += Expect(parsed.Value().eventDataRaw.empty(), "aBS zero area event");
    }
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedParse2x2();
    failures += TestEmptyObjectEventCells();
    failures += TestToPacketBuildSeparators();
    failures += TestErrors();
    failures += TestZeroAreaEmptyPayload();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
