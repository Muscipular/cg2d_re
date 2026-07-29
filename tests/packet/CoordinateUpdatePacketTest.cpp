#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::CoordinateUpdate;

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

const std::array<std::string_view, 12U> FieldNames = {{
    "mapId",
    "floor",
    "maxX",
    "maxY",
    "x",
    "y",
    "configId",
    "sequenceNumber",
    "variant",
    "bgm",
    "cut",
    "warpNumber",
}};

const std::array<int, 12U> FieldValues = {{
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 36, -1,
}};

int TestSpecAndToPacket()
{
    const CoordinateUpdate update(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 36, -1);
    const cg::packet::Packet packet = update.ToPacket();

    int failures = 0;
    failures += Expect(CoordinateUpdate::Code == "GmNM", "GmNM code");
    failures += Expect(
        CoordinateUpdate::Direction == cg::packet::PacketDirection::ServerToClient,
        "GmNM direction");
    failures += Expect(CoordinateUpdate::Fields.size() == FieldNames.size(), "GmNM fields");
    failures += Expect(CoordinateUpdate::Spec.code == CoordinateUpdate::Code, "GmNM Spec code");
    failures += Expect(
        CoordinateUpdate::Spec.direction == CoordinateUpdate::Direction,
        "GmNM Spec direction");
    failures += Expect(CoordinateUpdate::Spec.fieldCount == FieldNames.size(), "GmNM Spec count");
    failures += Expect(packet.code == cg::packet::PacketCode("GmNM"), "GmNM packet code");
    failures += Expect(packet.direction == CoordinateUpdate::Direction, "GmNM packet direction");
    failures += Expect(packet.fields.size() == FieldNames.size(), "GmNM packet fields");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(CoordinateUpdate::Fields[index].name == FieldNames[index], "GmNM field name");
        failures += Expect(
            CoordinateUpdate::Fields[index].type == cg::packet::PacketFieldType::Int62,
            "GmNM field type");
        if (index < packet.fields.size()) {
            failures += Expect(packet.fields[index].name == FieldNames[index], "GmNM packet field name");
            failures += Expect(
                packet.fields[index].type == cg::packet::PacketFieldType::Int62,
                "GmNM packet field type");
            failures += Expect(
                packet.fields[index].value == std::to_string(FieldValues[index]),
                "GmNM packet field value");
        }
    }
    return failures;
}

int ExpectValues(const CoordinateUpdate& update, const char* message)
{
    int failures = 0;
    failures += Expect(update.mapId == 1, message);
    failures += Expect(update.floor == 2, message);
    failures += Expect(update.maxX == 3, message);
    failures += Expect(update.maxY == 4, message);
    failures += Expect(update.x == 5, message);
    failures += Expect(update.y == 6, message);
    failures += Expect(update.configId == 7, message);
    failures += Expect(update.sequenceNumber == 8, message);
    failures += Expect(update.variant == 9, message);
    failures += Expect(update.bgm == 10, message);
    failures += Expect(update.cut == 36, message);
    failures += Expect(update.warpNumber == -1, message);
    return failures;
}

int TestTypedParseAndCanonical()
{
    const std::string canonical = "GmNM 1 2 3 4 5 6 7 8 9 a A -1 ";
    const cg::packet::PacketResult<CoordinateUpdate> parsed = CoordinateUpdate::Parse(canonical);

    int failures = Expect(parsed.HasValue(), "GmNM typed parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectValues(parsed.Value(), "GmNM typed values");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), CoordinateUpdate::Spec);
    failures += Expect(built.HasValue(), "GmNM typed build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == canonical, "GmNM typed canonical");
    }
    return failures;
}

int TestGenericRoundtripAndNegativeValues()
{
    const std::string plain = "GmNM 1 2 3 4 5 6 7 8 9 a A -1";
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(plain, CoordinateUpdate::Spec);

    int failures = Expect(parsed.HasValue(), "GmNM generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value(), CoordinateUpdate::Spec);
    failures += Expect(built.HasValue(), "GmNM generic build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == plain + ' ', "GmNM generic canonical");
    }

    const cg::packet::PacketResult<CoordinateUpdate> negative =
        CoordinateUpdate::Parse("GmNM -1 -2 -3 -4 -5 -6 -7 -8 -9 -a -A -b ");
    failures += Expect(negative.HasValue(), "GmNM negative parse");
    if (negative.HasValue()) {
        failures += Expect(negative.Value().mapId == -1, "GmNM negative mapId");
        failures += Expect(negative.Value().variant == -9, "GmNM negative variant");
        failures += Expect(negative.Value().bgm == -10, "GmNM negative bgm");
        failures += Expect(negative.Value().cut == -36, "GmNM negative cut");
        failures += Expect(negative.Value().warpNumber == -11, "GmNM negative warp");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 12U>& tokens)
{
    std::string plain = "GmNM";
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
        CoordinateUpdate::Parse("bad 1 2 3 4 5 6 7 8 9 a A -1 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "GmNM wrong code");
    failures += ExpectError(
        CoordinateUpdate::Parse("GmNM 1 2 3 4 5 6 7 8 9 a A"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "GmNM",
        true,
        11U,
        "GmNM missing field");
    failures += ExpectError(
        CoordinateUpdate::Parse("GmNM 1 2 3 4 5 6 7 8 9 a A -1 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "GmNM",
        true,
        12U,
        "GmNM extra field");

    const std::array<std::string, 12U> valid = {{
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "A", "-1",
    }};
    for (std::size_t index = 0U; index < valid.size(); ++index) {
        std::array<std::string, 12U> invalid = valid;
        invalid[index] = "_";
        failures += ExpectError(
            CoordinateUpdate::Parse(BuildPlain(invalid)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "GmNM",
            true,
            index,
            "GmNM invalid i62");

        std::array<std::string, 12U> overflow = valid;
        overflow[index] = "2lkCB2";
        failures += ExpectError(
            CoordinateUpdate::Parse(BuildPlain(overflow)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "GmNM",
            true,
            index,
            "GmNM i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestTypedParseAndCanonical()
        + TestGenericRoundtripAndNegativeValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " coordinate update packet test(s) failed\n";
        return 1;
    }
    return 0;
}
