#include "packet/packet.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::receive::WindowPacket::Spec;
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
        cg::packet::PacketCode(cg::packet::receive::WindowPacket::Code),
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

int ExpectWindowPacket(
    const cg::packet::Packet& packet,
    std::string_view windowtype,
    std::string_view buttontype,
    std::string_view seqno,
    std::string_view objindex,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::WindowPacket::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 5U, message);
    if (packet.fields.size() != 5U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "windowtype", cg::packet::PacketFieldType::Int62, windowtype, message);
    failures += ExpectField(packet, 1U, "buttontype", cg::packet::PacketFieldType::Int62, buttontype, message);
    failures += ExpectField(packet, 2U, "seqno", cg::packet::PacketFieldType::Int62, seqno, message);
    failures += ExpectField(packet, 3U, "objindex", cg::packet::PacketFieldType::Int62, objindex, message);
    failures += ExpectField(packet, 4U, "data", cg::packet::PacketFieldType::EscapedString, data, message);
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("BZ 0 1 2 -1 hello ");

    int failures = Expect(static_cast<bool>(parsed), "parse BZ suggested packet");
    if (parsed.HasValue()) {
        failures += ExpectWindowPacket(parsed.Value(), "0", "1", "2", "-1", "hello", "parse BZ packet fields");
    }
    return failures;
}

int TestTypedRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::receive::WindowPacket> parsed =
        cg::packet::receive::WindowPacket::Parse("BZ 0 1 2 -1 hello ");

    int failures = Expect(static_cast<bool>(parsed), "typed parse BZ suggested packet");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().windowtype == 0, "BZ windowtype");
    failures += Expect(parsed.Value().buttontype == 1, "BZ buttontype");
    failures += Expect(parsed.Value().seqno == 2, "BZ seqno");
    failures += Expect(parsed.Value().objindex == -1, "BZ objindex");
    failures += Expect(parsed.Value().data == "hello", "BZ data");
    failures += ExpectWindowPacket(parsed.Value().ToPacket(), "0", "1", "2", "-1", "hello", "BZ ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build BZ packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "BZ 0 1 2 -1 hello ", "roundtrip BZ packet");
    }
    return failures;
}

int TestEscapedDataRoundtrip()
{
    const std::string data = "hello world\nliteral\\nslash\\S";
    const cg::packet::receive::WindowPacket packet(0, 1, 2, -1, data);
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build BZ escaped data");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(
        built.Value() == "BZ 0 1 2 -1 hello\\Sworld\\nliteral\\\\nslash\\\\S ",
        "BZ escaped data plain");

    const cg::packet::PacketResult<cg::packet::receive::WindowPacket> parsed =
        cg::packet::receive::WindowPacket::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse BZ escaped data");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().data == data, "BZ escaped data value");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::receive::WindowPacket packet(0, -1, 2, -1, "");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build BZ empty data");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "BZ 0 -1 2 -1  ", "BZ empty data plain");

    const cg::packet::PacketResult<cg::packet::receive::WindowPacket> parsed =
        cg::packet::receive::WindowPacket::Parse("BZ 0 -1 2 -1 ");
    failures += Expect(static_cast<bool>(parsed), "parse BZ empty data");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().buttontype == -1, "BZ buttontype -1");
        failures += Expect(parsed.Value().data.empty(), "BZ empty data value");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("BZ 0 1 2 -1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "BZ",
        true,
        4U,
        "BZ missing data");
    failures += ExpectError(
        cg::packet::receive::WindowPacket::Parse("BZ 0 1 2 -1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "BZ",
        true,
        4U,
        "typed BZ missing data");
    failures += ExpectError(
        ParsePacket("BZ _ 1 2 -1 hello "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "BZ",
        true,
        0U,
        "BZ bad windowtype");
    failures += ExpectError(
        cg::packet::receive::WindowPacket::Parse("BZ 0 1 _ -1 hello "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "BZ",
        true,
        2U,
        "typed BZ bad seqno");
    failures += ExpectError(
        cg::packet::receive::WindowPacket::Parse("Zd hello "),
        cg::packet::PacketErrorCode::InvalidCode,
        "Zd",
        false,
        0U,
        "typed BZ wrong code");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ClientToServer, {
            Field("windowtype", cg::packet::PacketFieldType::Int62, "0"),
            Field("buttontype", cg::packet::PacketFieldType::Int62, "1"),
            Field("seqno", cg::packet::PacketFieldType::Int62, "2"),
            Field("objindex", cg::packet::PacketFieldType::Int62, "-1"),
            Field("data", cg::packet::PacketFieldType::EscapedString, "hello"),
        })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "BZ",
        false,
        0U,
        "BZ build wrong direction");
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {
            Field("windowType", cg::packet::PacketFieldType::Int62, "0"),
            Field("buttontype", cg::packet::PacketFieldType::Int62, "1"),
            Field("seqno", cg::packet::PacketFieldType::Int62, "2"),
            Field("objindex", cg::packet::PacketFieldType::Int62, "-1"),
            Field("data", cg::packet::PacketFieldType::EscapedString, "hello"),
        })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "BZ",
        true,
        0U,
        "BZ build wrong field name");
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {
            Field("windowtype", cg::packet::PacketFieldType::Int62, "0"),
            Field("buttontype", cg::packet::PacketFieldType::RawString, "1"),
            Field("seqno", cg::packet::PacketFieldType::Int62, "2"),
            Field("objindex", cg::packet::PacketFieldType::Int62, "-1"),
            Field("data", cg::packet::PacketFieldType::EscapedString, "hello"),
        })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "BZ",
        true,
        1U,
        "BZ build wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacket();
    failures += TestTypedRoundtrip();
    failures += TestEscapedDataRoundtrip();
    failures += TestEmptyData();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
