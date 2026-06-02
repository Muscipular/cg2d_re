#include "packet/packet.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::StallBrowse::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::StallBrowse::Spec);
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
    bool hasFieldIndex,
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

    if (hasFieldIndex) {
        failures += Expect(error.fieldIndex.has_value(), message);
        if (error.fieldIndex.has_value()) {
            failures += Expect(*error.fieldIndex == fieldIndex, message);
        }
    } else {
        failures += Expect(!error.fieldIndex.has_value(), message);
    }
    return failures;
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
        cg::packet::PacketCode(cg::packet::receive::StallBrowse::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectStallBrowsePacket(
    const cg::packet::Packet& packet,
    std::string_view result,
    std::string_view objIndex,
    std::string_view name,
    std::string_view description,
    std::string_view item,
    std::string_view pet,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::StallBrowse::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 6U, message);
    if (packet.fields.size() != 6U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "result", cg::packet::PacketFieldType::Int62, result, message);
    failures += ExpectField(packet, 1U, "objIndex", cg::packet::PacketFieldType::Int62, objIndex, message);
    failures += ExpectField(packet, 2U, "name", cg::packet::PacketFieldType::EscapedString, name, message);
    failures += ExpectField(packet, 3U, "description", cg::packet::PacketFieldType::EscapedString, description, message);
    failures += ExpectField(packet, 4U, "item", cg::packet::PacketFieldType::EscapedString, item, message);
    failures += ExpectField(packet, 5U, "pet", cg::packet::PacketFieldType::EscapedString, pet, message);
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("ft 0 2 Shop\\SName Line\\nTwo 0| 0| ");

    int failures = Expect(static_cast<bool>(parsed), "parse ft packet");
    if (parsed.HasValue()) {
        failures += ExpectStallBrowsePacket(
            parsed.Value(),
            "0",
            "2",
            "Shop Name",
            "Line\nTwo",
            "0|",
            "0|",
            "parse ft fields");
    }
    return failures;
}

int TestTypedRoundtrip()
{
    const cg::packet::receive::StallBrowse packet(0, 2, "Shop Name", "Line\nTwo", "0|", "0|");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build ft packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "ft 0 2 Shop\\SName Line\\nTwo 0| 0| ", "ft build order");

    const cg::packet::PacketResult<cg::packet::receive::StallBrowse> parsed =
        cg::packet::receive::StallBrowse::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "typed parse ft packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().result == 0, "ft result");
        failures += Expect(parsed.Value().objIndex == 2, "ft objIndex");
        failures += Expect(parsed.Value().name == "Shop Name", "ft name");
        failures += Expect(parsed.Value().description == "Line\nTwo", "ft description");
        failures += Expect(parsed.Value().item == "0|", "ft item");
        failures += Expect(parsed.Value().pet == "0|", "ft pet");
        failures += ExpectStallBrowsePacket(
            parsed.Value().ToPacket(),
            "0",
            "2",
            "Shop Name",
            "Line\nTwo",
            "0|",
            "0|",
            "ft ToPacket");
    }
    return failures;
}

int TestFailureResultsAllowEmptyStrings()
{
    int failures = 0;
    for (const int result : {1, 2, 3}) {
        const cg::packet::receive::StallBrowse packet(result, 2, "", "", "", "");
        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.ToPacket());
        failures += Expect(static_cast<bool>(built), "build ft failure result");
        if (!built.HasValue()) {
            continue;
        }

        const cg::packet::PacketResult<cg::packet::receive::StallBrowse> parsed =
            cg::packet::receive::StallBrowse::Parse(built.Value());
        failures += Expect(static_cast<bool>(parsed), "parse ft failure result");
        if (parsed.HasValue()) {
            failures += Expect(parsed.Value().result == result, "ft failure result value");
            failures += Expect(parsed.Value().name.empty(), "ft empty name");
            failures += Expect(parsed.Value().description.empty(), "ft empty description");
            failures += Expect(parsed.Value().item.empty(), "ft empty item");
            failures += Expect(parsed.Value().pet.empty(), "ft empty pet");
        }
    }
    return failures;
}

int TestItemPetRawRoundtrip()
{
    const std::string item = "2|0|name\\value|literal\\Sslash|tail|";
    const std::string pet = "1|pet\\value|skill|literal\\\\slash|";
    const cg::packet::receive::StallBrowse packet(0, 2, "shop", "desc", item, pet);
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build ft raw item pet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::StallBrowse> parsed =
        cg::packet::receive::StallBrowse::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse ft raw item pet");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().item == item, "ft raw item");
    failures += Expect(parsed.Value().pet == pet, "ft raw pet");

    const cg::packet::PacketResult<std::string> rebuilt = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(rebuilt), "rebuild ft raw item pet");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == built.Value(), "ft raw item pet roundtrip");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("ft 0 Shop Desc 0| 0|"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ft",
        true,
        5U,
        "ft old schema field count");
    failures += ExpectError(
        cg::packet::receive::StallBrowse::Parse("ft 0 Shop Desc 0| 0|"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ft",
        true,
        5U,
        "typed ft old schema field count");
    failures += ExpectError(
        ParsePacket("ft _ 2 name desc 0| 0| "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ft",
        true,
        0U,
        "ft bad result");
    failures += ExpectError(
        cg::packet::receive::StallBrowse::Parse("ft 0 _ name desc 0| 0| "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ft",
        true,
        1U,
        "typed ft bad objIndex");
    failures += ExpectError(
        cg::packet::receive::StallBrowse::Parse("BZ 0 1 2 -1 data "),
        cg::packet::PacketErrorCode::InvalidCode,
        "BZ",
        false,
        0U,
        "typed ft wrong code");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ClientToServer, {
            Field("result", cg::packet::PacketFieldType::Int62, "0"),
            Field("objIndex", cg::packet::PacketFieldType::Int62, "2"),
            Field("name", cg::packet::PacketFieldType::EscapedString, "shop"),
            Field("description", cg::packet::PacketFieldType::EscapedString, "desc"),
            Field("item", cg::packet::PacketFieldType::EscapedString, "0|"),
            Field("pet", cg::packet::PacketFieldType::EscapedString, "0|"),
        })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ft",
        false,
        0U,
        "ft build wrong direction");
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {
            Field("result", cg::packet::PacketFieldType::Int62, "0"),
            Field("objindex", cg::packet::PacketFieldType::Int62, "2"),
            Field("name", cg::packet::PacketFieldType::EscapedString, "shop"),
            Field("description", cg::packet::PacketFieldType::EscapedString, "desc"),
            Field("item", cg::packet::PacketFieldType::EscapedString, "0|"),
            Field("pet", cg::packet::PacketFieldType::EscapedString, "0|"),
        })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ft",
        true,
        1U,
        "ft build wrong field name");
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {
            Field("result", cg::packet::PacketFieldType::Int62, "0"),
            Field("objIndex", cg::packet::PacketFieldType::Int62, "2"),
            Field("name", cg::packet::PacketFieldType::EscapedString, "shop"),
            Field("description", cg::packet::PacketFieldType::EscapedString, "desc"),
            Field("item", cg::packet::PacketFieldType::Unknown, "0|"),
            Field("pet", cg::packet::PacketFieldType::EscapedString, "0|"),
        })),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "ft",
        true,
        4U,
        "ft build wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacket();
    failures += TestTypedRoundtrip();
    failures += TestFailureResultsAllowEmptyStrings();
    failures += TestItemPetRawRoundtrip();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
