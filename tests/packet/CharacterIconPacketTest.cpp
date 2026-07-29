#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

namespace {

using CharacterIcon = cg::packet::receive::CharacterIcon;
using CharacterIconEntry = cg::packet::receive::CharacterIconEntry;

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, CharacterIcon::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, CharacterIcon::Spec);
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

std::string BuildPlainNi(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "NI ";
    }

    std::string plain = "NI ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectCharacterIconPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    cg::packet::PacketFieldType fieldType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(CharacterIcon::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == fieldType, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectEntry(
    const CharacterIconEntry& entry,
    int objectIndex,
    int x,
    int y,
    int action,
    int direction,
    const std::vector<int>& options,
    const char* message)
{
    int failures = 0;
    failures += Expect(entry.objectIndex == objectIndex, message);
    failures += Expect(entry.x == x, message);
    failures += Expect(entry.y == y, message);
    failures += Expect(entry.action == action, message);
    failures += Expect(entry.direction == direction, message);
    failures += Expect(entry.options == options, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "1|10|20|3|4";
    const std::string plain = BuildPlainNi(payload);
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(plain);

    int failures = Expect(static_cast<bool>(parsed), "parse NI outer");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectCharacterIconPacket(
        parsed.Value(),
        payload,
        cg::packet::PacketFieldType::EscapedString,
        "parse NI outer packet");
    const cg::packet::PacketResult<std::string> rebuilt = BuildPacket(parsed.Value());
    failures += Expect(static_cast<bool>(rebuilt), "generic build NI outer");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == plain, "generic NI ParsePacket BuildPacket");
    }
    return failures;
}

int TestFourShapes()
{
    const std::vector<std::string> payloads = {
        "1|10|20|3|4",
        "2|11|21|4|5|90",
        "3|12|22|5|6|91|92|93",
        "4|13|23|6|7|91|92|93|94|95|96|97",
    };
    const std::vector<std::vector<int>> options = {
        {},
        {90},
        {91, 92, 93},
        {91, 92, 93, 94, 95, 96, 97},
    };

    int failures = 0;
    for (std::size_t index = 0; index < payloads.size(); ++index) {
        const cg::packet::PacketResult<CharacterIcon> parsed =
            CharacterIcon::Parse(BuildPlainNi(payloads[index]));
        failures += Expect(static_cast<bool>(parsed), "parse NI known shape");
        if (!parsed.HasValue()) {
            continue;
        }

        failures += Expect(parsed.Value().entries.size() == 1U, "parse NI known shape count");
        if (parsed.Value().entries.size() == 1U) {
            failures += ExpectEntry(
                parsed.Value().entries[0],
                static_cast<int>(index + 1U),
                static_cast<int>(index + 10U),
                static_cast<int>(index + 20U),
                static_cast<int>(index + 3U),
                static_cast<int>(index + 4U),
                options[index],
                "parse NI known shape fields");
        }

        const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
        failures += Expect(static_cast<bool>(rebuilt), "build NI known shape");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == payloads[index], "roundtrip NI known shape");
        }
    }
    return failures;
}

int TestMultiEntry()
{
    const std::string payload =
        "1|10|20|3|4,"
        "2|11|21|4|5|90,"
        "a|12|22|5|6|91|92|93,"
        "4|13|23|6|7|91|92|93|94|95|96|97";
    const cg::packet::PacketResult<CharacterIcon> parsed =
        CharacterIcon::Parse(BuildPlainNi(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse NI multi entry");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 4U, "parse NI multi count");
    if (parsed.Value().entries.size() == 4U) {
        failures += ExpectEntry(
            parsed.Value().entries[0],
            1,
            10,
            20,
            3,
            4,
            {},
            "parse NI multi entry 0");
        failures += ExpectEntry(
            parsed.Value().entries[1],
            2,
            11,
            21,
            4,
            5,
            {90},
            "parse NI multi entry 1");
        failures += ExpectEntry(
            parsed.Value().entries[2],
            10,
            12,
            22,
            5,
            6,
            {91, 92, 93},
            "parse NI multi entry 2");
    }

    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build NI multi entry");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip NI multi entry");
    }
    return failures;
}

int TestCanonicalRoundTrip()
{
    CharacterIcon icon({
        CharacterIconEntry(10, -10, 20, 3, 4, {90}),
        CharacterIconEntry(2, 11, -21, 5, 6, {91, 92, 93}),
    });
    const std::string payload =
        "a|-10|20|3|4|90,"
        "2|11|-21|5|6|91|92|93";

    const cg::packet::PacketResult<cg::packet::Packet> packet = icon.ToPacket();
    int failures = Expect(static_cast<bool>(packet), "NI ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += ExpectCharacterIconPacket(
        packet.Value(),
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "NI ToPacket data");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build NI packet");
    if (!built.HasValue()) {
        return failures;
    }
    failures += Expect(built.Value() == BuildPlainNi(payload), "build NI canonical wire");

    const cg::packet::PacketResult<CharacterIcon> parsed = CharacterIcon::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse rebuilt NI");
    if (parsed.HasValue()) {
        const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
        failures += Expect(static_cast<bool>(rebuilt), "rebuild parsed NI");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == payload, "canonical NI roundtrip");
        }
    }
    return failures;
}

int TestIntegerLimits()
{
    const std::string payload =
        "2lkCB1|-2147483648|2147483647|-1|0|-2147483648,"
        "-2lkCB2|2147483647|-2147483648|0|1";
    const cg::packet::PacketResult<CharacterIcon> parsed =
        CharacterIcon::Parse(BuildPlainNi(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse NI int limits");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 2U, "parse NI int limits count");
    if (parsed.Value().entries.size() == 2U) {
        failures += ExpectEntry(
            parsed.Value().entries[0],
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            -1,
            0,
            {std::numeric_limits<int>::min()},
            "parse NI int max entry");
        failures += ExpectEntry(
            parsed.Value().entries[1],
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            0,
            1,
            {},
            "parse NI int min entry");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("NI"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NI",
        "NI missing data");
    failures += ExpectError(
        CharacterIcon::Parse("NI"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NI",
        "typed NI missing data");
    failures += ExpectError(
        ParsePacket("NI one caInfo "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NI",
        "NI extra outer token");
    failures += ExpectError(
        CharacterIcon::Parse(BuildPlainNi("")),
        cg::packet::PacketErrorCode::EmptyInput,
        "NI",
        "typed NI empty payload");
    failures += ExpectError(
        CharacterIcon::Parse(BuildPlainNi("1|2|3|4")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NI",
        "typed NI missing entry field");
    failures += ExpectError(
        CharacterIcon::Parse(BuildPlainNi("1|2|3|4|5|6|7|8|9|10|11|12|13")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NI",
        "typed NI extra entry field");
    failures += ExpectError(
        CharacterIcon::Parse(BuildPlainNi("1|+2|3|4|5")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "NI",
        "typed NI invalid decimal");
    failures += ExpectError(
        CharacterIcon::Parse(BuildPlainNi("1|2147483648|3|4|5")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "NI",
        "typed NI positive decimal overflow");
    failures += ExpectError(
        CharacterIcon::Parse(BuildPlainNi("1|-2147483649|3|4|5")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "NI",
        "typed NI negative decimal overflow");
    failures += ExpectError(
        CharacterIcon::Parse(BuildPlainNi("1||3|4|5")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "NI",
        "typed NI empty decimal");
    failures += ExpectError(
        CharacterIcon::Parse(BuildPlainNi("_|2|3|4|5")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "NI",
        "typed NI invalid objectIndex");
    failures += ExpectError(
        CharacterIcon::Parse(BuildPlainNi("2lkCB2|2|3|4|5")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "NI",
        "typed NI objectIndex overflow");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    const CharacterIcon empty(std::vector<CharacterIconEntry> {});
    failures += ExpectError(
        empty.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "NI",
        "NI build empty entries");
    failures += ExpectError(
        empty.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "NI",
        "NI ToPacket empty entries");

    const CharacterIcon invalidOptions({
        CharacterIconEntry(1, 2, 3, 4, 5, {6, 7}),
    });
    failures += ExpectError(
        invalidOptions.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "NI",
        "NI build invalid options count");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestFourShapes();
    failures += TestMultiEntry();
    failures += TestCanonicalRoundTrip();
    failures += TestIntegerLimits();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
