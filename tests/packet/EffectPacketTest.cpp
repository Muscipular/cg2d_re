#include "packet/packet.hpp"

#include <cstddef>
#include <iostream>
#include <initializer_list>
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
        cg::packet::PacketCode(cg::packet::receive::Effect::Code),
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

int ExpectEffectPacket(
    const cg::packet::Packet& packet,
    std::string_view effType,
    std::string_view tmEffSize,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::Effect::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "EffType", cg::packet::PacketFieldType::Int62, effType, message);
    failures += ExpectField(packet, 1U, "TM_EffSize", cg::packet::PacketFieldType::Int62, tmEffSize, message);
    failures += ExpectField(packet, 2U, "data", cg::packet::PacketFieldType::EscapedString, data, message);
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("zzd 1 2 opt ");

    int failures = Expect(static_cast<bool>(parsed), "parse zzd");
    if (parsed.HasValue()) {
        failures += ExpectEffectPacket(parsed.Value(), "1", "2", "opt", "parse zzd packet");
    }
    return failures;
}

int TestInt62Parse()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("zzd a Z data ");

    int failures = Expect(static_cast<bool>(parsed), "parse zzd i62");
    if (parsed.HasValue()) {
        failures += ExpectEffectPacket(parsed.Value(), "10", "61", "data", "parse zzd i62 packet");
    }

    const cg::packet::PacketResult<cg::packet::receive::Effect> typed =
        cg::packet::receive::Effect::Parse("zzd a Z data ");
    failures += Expect(static_cast<bool>(typed), "typed parse zzd i62");
    if (typed.HasValue()) {
        failures += Expect(typed.Value().effType == 10, "typed zzd effType");
        failures += Expect(typed.Value().tmEffSize == 61, "typed zzd tmEffSize");
        failures += Expect(typed.Value().data == "data", "typed zzd data");
    }
    return failures;
}

int TestTypedRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::receive::Effect> parsed =
        cg::packet::receive::Effect::Parse("zzd 1 2 opt ");

    int failures = Expect(static_cast<bool>(parsed), "typed parse zzd");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().effType == 1, "zzd effType");
    failures += Expect(parsed.Value().tmEffSize == 2, "zzd tmEffSize");
    failures += Expect(parsed.Value().data == "opt", "zzd data");
    failures += ExpectEffectPacket(parsed.Value().ToPacket(), "1", "2", "opt", "zzd ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build zzd packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "zzd 1 2 opt ", "roundtrip zzd packet");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::receive::Effect effect(1, 2, "");
    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(effect.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build zzd empty data");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "zzd 1 2  ", "zzd empty data plain");

    const cg::packet::PacketResult<cg::packet::receive::Effect> parsed =
        cg::packet::receive::Effect::Parse("zzd 1 2 ");
    failures += Expect(static_cast<bool>(parsed), "parse zzd empty data");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().data.empty(), "zzd empty data value");
    }
    return failures;
}

int TestEscapedDataRoundtrip()
{
    const std::string data = "A B\\C|D";
    const cg::packet::receive::Effect effect(1, 2, data);
    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(effect.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build zzd escaped data");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "zzd 1 2 A\\SB\\\\C|D ", "zzd escaped plain");

    const cg::packet::PacketResult<cg::packet::receive::Effect> parsed =
        cg::packet::receive::Effect::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse zzd escaped data");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().data == data, "zzd escaped data value");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("zzd 1 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zzd",
        true,
        2U,
        "zzd missing data");
    failures += ExpectError(
        cg::packet::receive::Effect::Parse("zzd 1 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zzd",
        true,
        2U,
        "typed zzd missing data");
    failures += ExpectError(
        cg::packet::ParsePacket("zzd 1 2 data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zzd",
        true,
        3U,
        "zzd extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("zzd _ 2 data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "zzd",
        true,
        0U,
        "zzd bad effType");
    failures += ExpectError(
        cg::packet::receive::Effect::Parse("zzd 1 _ data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "zzd",
        true,
        1U,
        "typed zzd bad tmEffSize");
    failures += ExpectError(
        cg::packet::ParsePacket("zzd 2lkCB2 2 data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "zzd",
        true,
        0U,
        "zzd effType overflow");
    failures += ExpectError(
        cg::packet::receive::Effect::Parse("zzd 1 2lkCB2 data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "zzd",
        true,
        1U,
        "typed zzd tmEffSize overflow");
    failures += ExpectError(
        cg::packet::receive::Effect::Parse("Zd data "),
        cg::packet::PacketErrorCode::InvalidCode,
        "Zd",
        false,
        0U,
        "typed zzd wrong code");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {})),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zzd",
        true,
        0U,
        "zzd build missing fields");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("EffType", cg::packet::PacketFieldType::Int62, "1"),
                Field("TM_EffSize", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::EscapedString, "data"),
                Field("extra", cg::packet::PacketFieldType::EscapedString, "x"),
            })),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zzd",
        true,
        3U,
        "zzd build extra field");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("EffType", cg::packet::PacketFieldType::Int62, "1"),
                Field("TM_EffSize", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::EscapedString, "data"),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "zzd",
        false,
        0U,
        "zzd build wrong direction");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("effType", cg::packet::PacketFieldType::Int62, "1"),
                Field("TM_EffSize", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::EscapedString, "data"),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "zzd",
        true,
        0U,
        "zzd build wrong field name");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("EffType", cg::packet::PacketFieldType::EscapedString, "1"),
                Field("TM_EffSize", cg::packet::PacketFieldType::Int62, "2"),
                Field("data", cg::packet::PacketFieldType::EscapedString, "data"),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "zzd",
        true,
        0U,
        "zzd build wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacket();
    failures += TestInt62Parse();
    failures += TestTypedRoundtrip();
    failures += TestEmptyData();
    failures += TestEscapedDataRoundtrip();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
