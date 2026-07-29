#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::MapChecksum;

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
    bool hasFieldIndex,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    const cg::packet::PacketError& error = result.Error();
    failures += Expect(error.code == code, message);
    failures += Expect(error.packetCode.has_value(), message);
    if (error.packetCode.has_value()) {
        failures += Expect(error.packetCode->View() == packetCode, message);
    }
    failures += Expect(error.fieldIndex.has_value() == hasFieldIndex, message);
    if (hasFieldIndex && error.fieldIndex.has_value()) {
        failures += Expect(*error.fieldIndex == fieldIndex, message);
    }
    return failures;
}

const std::array<std::string_view, 9U> FieldNames = {{
    "mapId",
    "floor",
    "x1",
    "y1",
    "x2",
    "y2",
    "tileChecksum",
    "objectChecksum",
    "eventChecksum",
}};

const std::array<int, 9U> FieldValues = {{
    1, 62, 2, -1, 3, 4, 5, 6, 7,
}};

int TestSpecAndToPacket()
{
    const MapChecksum checksum(1, 62, 2, -1, 3, 4, 5, 6, 7);
    const cg::packet::Packet packet = checksum.ToPacket();

    int failures = 0;
    failures += Expect(MapChecksum::Code == "jmb", "jmb code");
    failures += Expect(
        MapChecksum::Direction == cg::packet::PacketDirection::ServerToClient,
        "jmb direction");
    failures += Expect(MapChecksum::Fields.size() == FieldNames.size(), "jmb fields");
    failures += Expect(MapChecksum::Spec.code == MapChecksum::Code, "jmb Spec code");
    failures += Expect(MapChecksum::Spec.direction == MapChecksum::Direction, "jmb Spec direction");
    failures += Expect(MapChecksum::Spec.fieldCount == FieldNames.size(), "jmb Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("jmb"), "jmb packet code");
    failures += Expect(packet.direction == MapChecksum::Direction, "jmb packet direction");
    failures += Expect(packet.fields.size() == FieldNames.size(), "jmb packet fields");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(MapChecksum::Fields[index].name == FieldNames[index], "jmb field name");
        failures += Expect(
            MapChecksum::Fields[index].type == cg::packet::PacketFieldType::Int62,
            "jmb field type");
        if (index < packet.fields.size()) {
            failures += Expect(packet.fields[index].name == FieldNames[index], "jmb packet field name");
            failures += Expect(
                packet.fields[index].type == cg::packet::PacketFieldType::Int62,
                "jmb packet field type");
            failures += Expect(
                packet.fields[index].value == std::to_string(FieldValues[index]),
                "jmb packet field value");
        }
    }
    return failures;
}

int ExpectFixtureValues(const MapChecksum& checksum, const char* message)
{
    int failures = 0;
    failures += Expect(checksum.mapId == 1, message);
    failures += Expect(checksum.floor == 62, message);
    failures += Expect(checksum.x1 == 2, message);
    failures += Expect(checksum.y1 == -1, message);
    failures += Expect(checksum.x2 == 3, message);
    failures += Expect(checksum.y2 == 4, message);
    failures += Expect(checksum.tileChecksum == 5, message);
    failures += Expect(checksum.objectChecksum == 6, message);
    failures += Expect(checksum.eventChecksum == 7, message);
    return failures;
}

int TestTypedParseAndCanonical()
{
    const std::string canonical = "jmb 1 10 2 -1 3 4 5 6 7 ";
    const cg::packet::PacketResult<MapChecksum> parsed = MapChecksum::Parse(canonical);

    int failures = Expect(parsed.HasValue(), "jmb typed parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectFixtureValues(parsed.Value(), "jmb typed values");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), MapChecksum::Spec);
    failures += Expect(built.HasValue(), "jmb typed build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == canonical, "jmb typed canonical");
    }
    return failures;
}

int TestGenericRoundtripAndUnrestrictedValues()
{
    const std::string plain = "jmb 1 10 2 -1 3 4 5 6 7";
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(plain, MapChecksum::Spec);

    int failures = Expect(parsed.HasValue(), "jmb generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value(), MapChecksum::Spec);
    failures += Expect(built.HasValue(), "jmb generic build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == plain + ' ', "jmb generic canonical");
    }

    const cg::packet::PacketResult<MapChecksum> unrestricted =
        MapChecksum::Parse("jmb 1 0 5 5 1 1 10000 -1 -2 ");
    failures += Expect(unrestricted.HasValue(), "jmb unrestricted parse");
    if (unrestricted.HasValue()) {
        failures += Expect(unrestricted.Value().x1 == 5, "jmb unordered x1 retained");
        failures += Expect(unrestricted.Value().x2 == 1, "jmb unordered x2 retained");
        failures += Expect(
            unrestricted.Value().tileChecksum == 14776336,
            "jmb checksum above 65535 retained");
        failures += Expect(unrestricted.Value().objectChecksum == -1, "jmb negative checksum retained");
        failures += Expect(unrestricted.Value().eventChecksum == -2, "jmb negative event retained");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 9U>& tokens)
{
    std::string plain = "jmb";
    for (const std::string& token : tokens) {
        plain.push_back(' ');
        plain.append(token);
    }
    plain.push_back(' ');
    return plain;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        MapChecksum::Parse("bad 1 10 2 -1 3 4 5 6 7 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "jmb wrong code");
    failures += ExpectError(
        MapChecksum::Parse("jmb 1 10 2 -1 3 4 5 6"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "jmb",
        true,
        8U,
        "jmb missing field");
    failures += ExpectError(
        MapChecksum::Parse("jmb 1 10 2 -1 3 4 5 6 7 8 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "jmb",
        true,
        9U,
        "jmb extra field");

    const std::array<std::string, 9U> valid = {{
        "1", "10", "2", "-1", "3", "4", "5", "6", "7",
    }};
    for (std::size_t index = 0U; index < valid.size(); ++index) {
        std::array<std::string, 9U> invalid = valid;
        invalid[index] = "_";
        failures += ExpectError(
            MapChecksum::Parse(BuildPlain(invalid)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "jmb",
            true,
            index,
            "jmb invalid i62");

        std::array<std::string, 9U> overflow = valid;
        overflow[index] = "2lkCB2";
        failures += ExpectError(
            MapChecksum::Parse(BuildPlain(overflow)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "jmb",
            true,
            index,
            "jmb i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestTypedParseAndCanonical()
        + TestGenericRoundtripAndUnrestrictedValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " map checksum packet test(s) failed\n";
        return 1;
    }
    return 0;
}
