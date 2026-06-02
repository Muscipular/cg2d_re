#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::CharacterIcon::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::CharacterIcon::Spec);
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
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::CharacterIcon::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectEntry(
    const cg::packet::receive::CharacterIconEntry& entry,
    int objectIndex,
    const std::vector<std::string>& fields,
    const char* message)
{
    int failures = 0;
    failures += Expect(entry.objectIndex == objectIndex, message);
    failures += Expect(entry.fields.size() == fields.size(), message);
    if (entry.fields.size() != fields.size()) {
        return failures;
    }

    for (std::size_t index = 0; index < fields.size(); ++index) {
        failures += Expect(entry.fields[index] == fields[index], message);
    }
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "1|10|20|3|4";
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainNi(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse NI outer");
    if (parsed.HasValue()) {
        failures += ExpectCharacterIconPacket(parsed.Value(), payload, "parse NI outer packet");
    }
    return failures;
}

int TestTypedSingleEntry()
{
    const std::string payload = "1|10|20|3|4";
    const cg::packet::PacketResult<cg::packet::receive::CharacterIcon> parsed =
        cg::packet::receive::CharacterIcon::Parse(BuildPlainNi(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse NI single entry");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().data == payload, "parse NI single data");
    failures += Expect(parsed.Value().entries.size() == 1U, "parse NI single count");
    if (parsed.Value().entries.size() == 1U) {
        failures += ExpectEntry(parsed.Value().entries[0], 1, {"1", "10", "20", "3", "4"}, "parse NI single fields");
    }
    return failures;
}

int TestMultiEntry()
{
    const std::string payload =
        "1|10|20|3|4,"
        "2|11|21|4|5|90,"
        "a|12|22|5|6|91|92|93";
    const cg::packet::PacketResult<cg::packet::receive::CharacterIcon> parsed =
        cg::packet::receive::CharacterIcon::Parse(BuildPlainNi(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse NI multi entry");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 3U, "parse NI multi count");
    if (parsed.Value().entries.size() == 3U) {
        failures += ExpectEntry(parsed.Value().entries[0], 1, {"1", "10", "20", "3", "4"}, "parse NI multi entry 0");
        failures += ExpectEntry(parsed.Value().entries[1], 2, {"2", "11", "21", "4", "5", "90"}, "parse NI multi entry 1");
        failures += ExpectEntry(parsed.Value().entries[2], 10, {"a", "12", "22", "5", "6", "91", "92", "93"}, "parse NI multi entry 2");
    }
    return failures;
}

int TestFieldCountRoundTrip()
{
    const std::vector<std::string> payloads = {
        "1|10|20|3|4",
        "2|11|21|4|5|90",
        "3|12|22|5|6|91|92|93",
        "4|13|23|6|7|91|92|93|94|95|96|97",
    };

    int failures = 0;
    for (const std::string& payload : payloads) {
        const cg::packet::PacketResult<cg::packet::receive::CharacterIcon> parsed =
            cg::packet::receive::CharacterIcon::Parse(BuildPlainNi(payload));
        failures += Expect(static_cast<bool>(parsed), "parse NI field count");
        if (!parsed.HasValue()) {
            continue;
        }

        failures += Expect(parsed.Value().BuildData() == payload, "build NI payload");
        const cg::packet::Packet packet = parsed.Value().ToPacket();
        failures += ExpectCharacterIconPacket(packet, payload, "NI ToPacket");

        const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
        failures += Expect(static_cast<bool>(built), "build NI packet");
        if (!built.HasValue()) {
            continue;
        }

        const cg::packet::PacketResult<cg::packet::receive::CharacterIcon> parsedAgain =
            cg::packet::receive::CharacterIcon::Parse(built.Value());
        failures += Expect(static_cast<bool>(parsedAgain), "parse rebuilt NI packet");
        if (parsedAgain.HasValue()) {
            failures += Expect(parsedAgain.Value().BuildData() == payload, "roundtrip NI payload");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("NI"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NI",
        "NI missing data");
    failures += ExpectError(
        cg::packet::receive::CharacterIcon::Parse("NI"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NI",
        "typed NI missing data");
    failures += ExpectError(
        ParsePacket("NI one caInfo "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NI",
        "NI extra outer token");
    failures += ExpectError(
        cg::packet::receive::CharacterIcon::Parse(BuildPlainNi("1|2|3|4|5|6|7")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NI",
        "typed NI bad entry field count");
    failures += ExpectError(
        cg::packet::receive::CharacterIcon::Parse(BuildPlainNi("_|2|3|4|5")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "NI",
        "typed NI bad object index");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedSingleEntry();
    failures += TestMultiEntry();
    failures += TestFieldCountRoundTrip();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
