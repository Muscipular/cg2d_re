#include "packet/Packet.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::TradePlayerList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::TradePlayerList::Spec);
}

using TradePlayerList = cg::packet::receive::TradePlayerList;
using TradePlayerRecord = cg::packet::receive::TradePlayerRecord;

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

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string output;
    output.reserve(values.size());
    for (const unsigned int value : values) {
        output.push_back(static_cast<char>(value));
    }
    return output;
}

cg::packet::PacketField Field(
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string value)
{
    cg::packet::PacketField field;
    field.name = std::string(name);
    field.type = type;
    field.value = std::move(value);
    return field;
}

cg::packet::Packet PacketOf(
    cg::packet::PacketDirection direction,
    std::initializer_list<cg::packet::PacketField> fields)
{
    cg::packet::Packet packet(
        cg::packet::PacketCode(TradePlayerList::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlain(std::string_view payload)
{
    std::string plain = "Cxo ";
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

std::string BuildPlain(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return BuildRawPlain("");
    }
    return BuildRawPlain(encoded.Value());
}

int ExpectTradePlayerListPacket(
    const cg::packet::Packet& packet,
    std::string_view payload,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(TradePlayerList::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == payload, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlain("Alice|Bob|"));

    int failures = Expect(static_cast<bool>(parsed), "parse Cxo outer");
    if (parsed.HasValue()) {
        failures += ExpectTradePlayerListPacket(parsed.Value(), "Alice|Bob|", "parse Cxo packet");
    }
    return failures;
}

int TestTwoRecordsRoundtrip()
{
    const cg::packet::PacketResult<TradePlayerList> parsed =
        TradePlayerList::Parse(BuildPlain("Alice|Bob|"));

    int failures = Expect(static_cast<bool>(parsed), "parse Cxo two records");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 2U, "Cxo record count");
    if (parsed.Value().records.size() == 2U) {
        failures += Expect(parsed.Value().records[0].nameRaw == "Alice", "Cxo first name");
        failures += Expect(parsed.Value().records[1].nameRaw == "Bob", "Cxo second name");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "Cxo BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == "Alice|Bob|", "Cxo BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "Cxo ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build Cxo packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain("Alice|Bob|"), "Cxo build plain");
    }
    return failures;
}

int TestSemanticLegacyRoundtrip()
{
    const std::string firstName = Bytes({0x81U, 0x7CU, 0x41U, 0x7CU, 0x42U});
    const std::string secondName = Bytes({0x81U, 0x5CU, 0x43U, 0x5CU, 0x44U});
    std::string payload = Bytes({0x81U, 0x7CU, 0x41U, 0x5CU, 0x7AU, 0x42U, 0x7CU});
    payload.append(Bytes({0x81U, 0x5CU, 0x43U, 0x5CU, 0x79U, 0x44U, 0x7CU}));
    const cg::packet::PacketResult<TradePlayerList> parsed = TradePlayerList::Parse(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Cxo semantic legacy names");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 2U, "Cxo semantic legacy count");
    if (parsed.Value().records.size() != 2U) {
        return failures;
    }

    failures += Expect(parsed.Value().records[0].nameRaw == firstName, "Cxo first semantic name");
    failures += Expect(parsed.Value().records[1].nameRaw == secondName, "Cxo second semantic name");

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "Cxo semantic BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "Cxo semantic BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "Cxo semantic ToPacket");
    if (packet.HasValue()) {
        failures += Expect(packet.Value().fields.size() == 1U, "Cxo semantic ToPacket field count");
        if (packet.Value().fields.size() == 1U) {
            failures += Expect(
                packet.Value().fields[0].type == cg::packet::PacketFieldType::SubPayload,
                "Cxo semantic ToPacket field type");
            failures += Expect(packet.Value().fields[0].value == payload, "Cxo semantic ToPacket value");
        }

        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "Cxo semantic BuildPacket");
        if (built.HasValue()) {
            failures += Expect(built.Value() == BuildPlain(payload), "Cxo semantic BuildPacket value");
        }
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::PacketResult<TradePlayerList> parsed = TradePlayerList::Parse("Cxo  ");

    int failures = Expect(static_cast<bool>(parsed), "parse Cxo empty data");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.empty(), "Cxo empty records");
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "Cxo empty BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value().empty(), "Cxo empty payload");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "Cxo empty ToPacket");
    if (packet.HasValue()) {
        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "Cxo empty BuildPacket");
        if (built.HasValue()) {
            failures += Expect(built.Value() == "Cxo  ", "Cxo empty BuildPacket value");
        }
    }
    return failures;
}

int TestPayloadErrors()
{
    int failures = 0;
    failures += ExpectError(
        TradePlayerList::Parse(BuildPlain("Alice|Bob")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Cxo",
        "Cxo missing trailing delimiter");
    failures += ExpectError(
        TradePlayerList::Parse(BuildPlain("Alice||Bob|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Cxo",
        "Cxo middle empty segment");
    failures += ExpectError(
        TradePlayerList::Parse(BuildPlain("|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Cxo",
        "Cxo delimiter only");
    return failures;
}

int TestPacketErrors()
{
    int failures = 0;
    failures += ExpectError(
        TradePlayerList::Parse("Rll 1 text 3 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "Rll",
        "Cxo wrong code");
    failures += ExpectError(
        TradePlayerList::Parse("Cxo"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Cxo",
        "Cxo missing field");
    failures += ExpectError(
        TradePlayerList::Parse("Cxo Alice| Bob| "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Cxo",
        "Cxo extra field");
    failures += ExpectError(
        TradePlayerList::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("data", cg::packet::PacketFieldType::Unknown, "Alice|")})),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "Cxo",
        "Cxo wrong field type");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    failures += ExpectError(
        TradePlayerList(std::vector<TradePlayerRecord> {{""}}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Cxo",
        "Cxo build empty name");
    failures += ExpectFieldError(
        TradePlayerList(std::vector<TradePlayerRecord> {{Bytes({0x81U})}}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Cxo",
        0U,
        "Cxo build truncated GBK name");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {Field("data", cg::packet::PacketFieldType::EscapedString, "Alice|")})),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Cxo",
        "Cxo build wrong direction");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTwoRecordsRoundtrip();
    failures += TestSemanticLegacyRoundtrip();
    failures += TestEmptyData();
    failures += TestPayloadErrors();
    failures += TestPacketErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
