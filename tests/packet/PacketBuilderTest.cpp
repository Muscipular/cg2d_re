#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"
#include "packet/Send.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <limits>
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

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string text;
    text.reserve(values.size());
    for (const unsigned int value : values) {
        text.push_back(static_cast<char>(value));
    }
    return text;
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
    std::string_view code,
    cg::packet::PacketDirection direction,
    std::initializer_list<cg::packet::PacketField> fields)
{
    cg::packet::Packet packet(cg::packet::PacketCode(code), direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

int ExpectNoNewline(std::string_view text, const char* message)
{
    int failures = 0;
    failures += Expect(text.find('\n') == std::string_view::npos, message);
    failures += Expect(text.find('\r') == std::string_view::npos, message);
    return failures;
}

int ExpectBuild(
    const cg::packet::Packet& packet,
    const cg::packet::PacketSpec& packetSpec,
    std::string_view expected,
    const char* message)
{
    const cg::packet::PacketResult<std::string> result = cg::packet::BuildPacket(packet, packetSpec);
    int failures = Expect(static_cast<bool>(result), message);
    if (result.HasValue()) {
        failures += Expect(result.Value() == expected, message);
        failures += ExpectNoNewline(result.Value(), message);
    }
    return failures;
}

int ExpectError(
    const cg::packet::PacketResult<std::string>& result,
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

int ExpectParsedField(
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

int TestKnownPackets()
{
    int failures = 0;
    failures += ExpectBuild(
        PacketOf(
            "JFVf",
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("clientHash", cg::packet::PacketFieldType::EscapedString, "hash"),
                Field("password", cg::packet::PacketFieldType::EscapedString, "pass"),
                Field("account", cg::packet::PacketFieldType::EscapedString, "account"),
            }),
        cg::packet::send::ClientLogin::Spec,
        "JFVf hash pass account ",
        "build JFVf");

    failures += ExpectBuild(
        PacketOf(
            "whV",
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("result", cg::packet::PacketFieldType::Int62, "0"),
                Field("data", cg::packet::PacketFieldType::EscapedString, "busy"),
            }),
        cg::packet::receive::ClientLoginResult::Spec,
        "whV 0 busy ",
        "build whV");

    failures += ExpectBuild(
        PacketOf(
            "CCL",
            cg::packet::PacketDirection::ClientToServer,
            {Field("time", cg::packet::PacketFieldType::Int62, "10")}),
        cg::packet::send::CheckCommand::Spec,
        "CCL a ",
        "build CCL 10");

    failures += ExpectBuild(
        PacketOf("Gp", cg::packet::PacketDirection::ClientToServer, {}),
        cg::packet::send::CharacterListRequest::Spec,
        "Gp ",
        "build Gp");
    return failures;
}

int TestEscapedStringAndTailEmpty()
{
    int failures = 0;
    failures += ExpectBuild(
        PacketOf(
            "JFVf",
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("clientHash", cg::packet::PacketFieldType::EscapedString, "a b"),
                Field("password", cg::packet::PacketFieldType::EscapedString, "line\nret\rslash\\"),
                Field("account", cg::packet::PacketFieldType::EscapedString, ""),
            }),
        cg::packet::send::ClientLogin::Spec,
        "JFVf a\\Sb line\\nret\\rslash\\\\  ",
        "build JFVf escaped and tail empty");

    failures += ExpectBuild(
        PacketOf(
            "whV",
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("result", cg::packet::PacketFieldType::Int62, "0"),
                Field("data", cg::packet::PacketFieldType::EscapedString, ""),
            }),
        cg::packet::receive::ClientLoginResult::Spec,
        "whV 0  ",
        "build whV empty es");
    return failures;
}

int TestInt62Values()
{
    int failures = 0;
    failures += ExpectBuild(
        PacketOf(
            "CCL",
            cg::packet::PacketDirection::ClientToServer,
            {Field("time", cg::packet::PacketFieldType::Int62, "0")}),
        cg::packet::send::CheckCommand::Spec,
        "CCL 0 ",
        "build i62 zero");
    failures += ExpectBuild(
        PacketOf(
            "CCL",
            cg::packet::PacketDirection::ClientToServer,
            {Field("time", cg::packet::PacketFieldType::Int62, "-1")}),
        cg::packet::send::CheckCommand::Spec,
        "CCL -1 ",
        "build i62 negative");
    failures += ExpectBuild(
        PacketOf(
            "CCL",
            cg::packet::PacketDirection::ClientToServer,
            {Field("time", cg::packet::PacketFieldType::Int62, std::to_string(std::numeric_limits<int>::max()))}),
        cg::packet::send::CheckCommand::Spec,
        "CCL 2lkCB1 ",
        "build i62 int max");
    failures += ExpectBuild(
        PacketOf(
            "CCL",
            cg::packet::PacketDirection::ClientToServer,
            {Field("time", cg::packet::PacketFieldType::Int62, std::to_string(std::numeric_limits<int>::min()))}),
        cg::packet::send::CheckCommand::Spec,
        "CCL -2lkCB2 ",
        "build i62 int min");
    return failures;
}

int TestRepeatedTail()
{
    int failures = 0;
    failures += ExpectBuild(
        PacketOf(
            "kza",
            cg::packet::PacketDirection::ClientToServer,
            {Field("count", cg::packet::PacketFieldType::Int62, "0")}),
        cg::packet::send::AlbumInfoRequest::Spec,
        "kza 0 ",
        "build kza zero");
    failures += ExpectBuild(
        PacketOf(
            "kza",
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("count", cg::packet::PacketFieldType::Int62, "2"),
                Field("albumid", cg::packet::PacketFieldType::Int62, "10"),
                Field("albumid", cg::packet::PacketFieldType::Int62, "20"),
            }),
        cg::packet::send::AlbumInfoRequest::Spec,
        "kza 2 a k ",
        "build kza repeated tail");
    failures += ExpectBuild(
        PacketOf(
            "kza",
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("count", cg::packet::PacketFieldType::Int62, "1"),
                Field("albumid", cg::packet::PacketFieldType::Int62, "10"),
            }),
        cg::packet::send::AlbumInfoRequest::Spec,
        "kza 1 a ",
        "build kza single repeated tail");

    const auto expectFieldError = [&](const cg::packet::Packet& packet,
                                      cg::packet::PacketErrorCode code,
                                      std::size_t fieldIndex,
                                      const char* message) {
        return ExpectError(
            cg::packet::BuildPacket(packet, cg::packet::send::AlbumInfoRequest::Spec),
            code,
            "kza",
            true,
            fieldIndex,
            message);
    };

    failures += expectFieldError(
        PacketOf(
            "kza",
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("count", cg::packet::PacketFieldType::Int62, "2"),
                Field("albumid", cg::packet::PacketFieldType::Int62, "10"),
            }),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        2U,
        "build kza repeated tail missing");
    failures += expectFieldError(
        PacketOf(
            "kza",
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("count", cg::packet::PacketFieldType::Int62, "1"),
                Field("albumid", cg::packet::PacketFieldType::Int62, "10"),
                Field("albumid", cg::packet::PacketFieldType::Int62, "20"),
            }),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        2U,
        "build kza repeated tail extra");
    failures += expectFieldError(
        PacketOf(
            "kza",
            cg::packet::PacketDirection::ClientToServer,
            {Field("count", cg::packet::PacketFieldType::Int62, "-1")}),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        0U,
        "build kza negative count");
    failures += expectFieldError(
        PacketOf(
            "kza",
            cg::packet::PacketDirection::ClientToServer,
            {Field("count", cg::packet::PacketFieldType::Int62, "a")}),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "build kza invalid count");
    failures += expectFieldError(
        PacketOf(
            "kza",
            cg::packet::PacketDirection::ClientToServer,
            {Field("count", cg::packet::PacketFieldType::Int62, "2147483648")}),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "build kza overflow count");
    failures += expectFieldError(
        PacketOf(
            "kza",
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("count", cg::packet::PacketFieldType::Int62, "1"),
                Field("albumid", cg::packet::PacketFieldType::Int62, "a"),
            }),
        cg::packet::PacketErrorCode::EncodeFailed,
        1U,
        "build kza invalid albumid");
    failures += expectFieldError(
        PacketOf(
            "kza",
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("count", cg::packet::PacketFieldType::Int62, "1"),
                Field("albumid", cg::packet::PacketFieldType::Int62, "2147483648"),
            }),
        cg::packet::PacketErrorCode::EncodeFailed,
        1U,
        "build kza overflow albumid");

    constexpr cg::packet::PacketSpec invalidSpec {
        "bad",
        cg::packet::PacketDirection::ClientToServer,
        nullptr,
        0U,
        {true, 0U},
    };
    failures += ExpectError(
        cg::packet::BuildPacket(
            PacketOf("bad", cg::packet::PacketDirection::ClientToServer, {}),
            invalidSpec),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "bad",
        true,
        0U,
        "build invalid repeated tail spec");
    return failures;
}

int TestRoundTrip()
{
    const cg::packet::Packet packet = PacketOf(
        "whV",
        cg::packet::PacketDirection::ServerToClient,
        {
            Field("result", cg::packet::PacketFieldType::Int62, "10"),
            Field("data", cg::packet::PacketFieldType::EscapedString, "busy now\\ok"),
        });

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(packet, cg::packet::receive::ClientLoginResult::Spec);
    int failures = Expect(static_cast<bool>(built), "roundtrip build");
    if (!built.HasValue()) {
        return failures;
    }

    failures += ExpectNoNewline(built.Value(), "roundtrip no newline");
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(built.Value(), cg::packet::receive::ClientLoginResult::Spec);
    failures += Expect(static_cast<bool>(parsed), "roundtrip parse");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().code == cg::packet::PacketCode("whV"), "roundtrip code");
        failures += Expect(parsed.Value().direction == cg::packet::PacketDirection::ServerToClient, "roundtrip direction");
        failures += ExpectParsedField(
            parsed.Value(),
            0U,
            "result",
            cg::packet::PacketFieldType::Int62,
            "10",
            "roundtrip result");
        failures += ExpectParsedField(
            parsed.Value(),
            1U,
            "data",
            cg::packet::PacketFieldType::EscapedString,
            "busy now\\ok",
            "roundtrip data");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::BuildPacket(
            PacketOf("", cg::packet::PacketDirection::ClientToServer, {}),
            cg::packet::send::ClientLogin::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "",
        false,
        0U,
        "empty code");
    failures += ExpectError(
        cg::packet::BuildPacket(
            PacketOf("BAD", cg::packet::PacketDirection::ClientToServer, {}),
            cg::packet::send::ClientLogin::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "BAD",
        false,
        0U,
        "wrong spec code");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            "CCL",
            cg::packet::PacketDirection::ServerToClient,
            {Field("time", cg::packet::PacketFieldType::Int62, "1")}),
            cg::packet::send::CheckCommand::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "CCL",
        false,
        0U,
        "direction mismatch");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            "whV",
            cg::packet::PacketDirection::ServerToClient,
            {Field("result", cg::packet::PacketFieldType::Int62, "0")}),
            cg::packet::receive::ClientLoginResult::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "whV",
        true,
        1U,
        "missing field");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            "Gp",
            cg::packet::PacketDirection::ClientToServer,
            {Field("extra", cg::packet::PacketFieldType::Unknown, "x")}),
            cg::packet::send::CharacterListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Gp",
        true,
        0U,
        "extra field");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            "JFVf",
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("wrong", cg::packet::PacketFieldType::EscapedString, "hash"),
                Field("password", cg::packet::PacketFieldType::EscapedString, "pass"),
                Field("account", cg::packet::PacketFieldType::EscapedString, "account"),
            }),
            cg::packet::send::ClientLogin::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "JFVf",
        true,
        0U,
        "field name mismatch");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            "JFVf",
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("clientHash", cg::packet::PacketFieldType::Unknown, "hash"),
                Field("password", cg::packet::PacketFieldType::EscapedString, "pass"),
                Field("account", cg::packet::PacketFieldType::EscapedString, "account"),
            }),
            cg::packet::send::ClientLogin::Spec),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "JFVf",
        true,
        0U,
        "field type mismatch");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            "CCL",
            cg::packet::PacketDirection::ClientToServer,
            {Field("time", cg::packet::PacketFieldType::Unknown, "1")}),
            cg::packet::send::CheckCommand::Spec),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "CCL",
        true,
        0U,
        "unknown unsupported");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            "CCL",
            cg::packet::PacketDirection::ClientToServer,
            {Field("time", cg::packet::PacketFieldType::Int62, "a")}),
            cg::packet::send::CheckCommand::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "CCL",
        true,
        0U,
        "i62 non decimal");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            "CCL",
            cg::packet::PacketDirection::ClientToServer,
            {Field("time", cg::packet::PacketFieldType::Int62, "2147483648")}),
            cg::packet::send::CheckCommand::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "CCL",
        true,
        0U,
        "i62 positive overflow");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            "CCL",
            cg::packet::PacketDirection::ClientToServer,
            {Field("time", cg::packet::PacketFieldType::Int62, "-2147483649")}),
            cg::packet::send::CheckCommand::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "CCL",
        true,
        0U,
        "i62 negative overflow");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            "JFVf",
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("clientHash", cg::packet::PacketFieldType::EscapedString, Bytes({0x81U})),
                Field("password", cg::packet::PacketFieldType::EscapedString, "pass"),
                Field("account", cg::packet::PacketFieldType::EscapedString, "account"),
            }),
            cg::packet::send::ClientLogin::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "JFVf",
        true,
        0U,
        "es truncated double-byte");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestKnownPackets();
    failures += TestEscapedStringAndTailEmpty();
    failures += TestInt62Values();
    failures += TestRepeatedTail();
    failures += TestRoundTrip();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
