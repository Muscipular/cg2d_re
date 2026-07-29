#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::CharacterLoginResult;

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

int TestSpecAndToPacket()
{
    const CharacterLoginResult loginResult("successful", "");
    const cg::packet::Packet packet = loginResult.ToPacket();

    int failures = 0;
    failures += Expect(CharacterLoginResult::Code == "hPUd", "hPUd code");
    failures += Expect(
        CharacterLoginResult::Direction == cg::packet::PacketDirection::ServerToClient,
        "hPUd direction");
    failures += Expect(CharacterLoginResult::Fields.size() == 2U, "hPUd fields");
    failures += Expect(CharacterLoginResult::Fields[0].name == "result", "hPUd result name");
    failures += Expect(CharacterLoginResult::Fields[1].name == "msg", "hPUd msg name");
    failures += Expect(
        CharacterLoginResult::Fields[0].type == cg::packet::PacketFieldType::EscapedString,
        "hPUd result type");
    failures += Expect(
        CharacterLoginResult::Fields[1].type == cg::packet::PacketFieldType::EscapedString,
        "hPUd msg type");
    failures += Expect(CharacterLoginResult::Spec.code == CharacterLoginResult::Code, "hPUd Spec code");
    failures += Expect(
        CharacterLoginResult::Spec.direction == CharacterLoginResult::Direction,
        "hPUd Spec direction");
    failures += Expect(CharacterLoginResult::Spec.fieldCount == 2U, "hPUd Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("hPUd"), "hPUd packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        "hPUd packet direction");
    failures += Expect(packet.fields.size() == 2U, "hPUd packet fields");
    if (packet.fields.size() == 2U) {
        failures += Expect(packet.fields[0].name == "result", "hPUd packet result name");
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::EscapedString,
            "hPUd packet result type");
        failures += Expect(packet.fields[0].value == "successful", "hPUd packet result value");
        failures += Expect(packet.fields[1].name == "msg", "hPUd packet msg name");
        failures += Expect(
            packet.fields[1].type == cg::packet::PacketFieldType::EscapedString,
            "hPUd packet msg type");
        failures += Expect(packet.fields[1].value.empty(), "hPUd packet empty msg");
    }
    return failures;
}

int TestSuccessfulEmptyMessage()
{
    const cg::packet::PacketResult<CharacterLoginResult> parsed =
        CharacterLoginResult::Parse("hPUd successful  ");

    int failures = Expect(parsed.HasValue(), "hPUd successful parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().result == "successful", "hPUd successful result");
    failures += Expect(parsed.Value().message.empty(), "hPUd successful empty msg");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), CharacterLoginResult::Spec);
    failures += Expect(rebuilt.HasValue(), "hPUd successful build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "hPUd successful  ", "hPUd successful canonical");
    }
    return failures;
}

int TestEscapedMessageAndUnknownResult()
{
    const cg::packet::PacketResult<CharacterLoginResult> failed =
        CharacterLoginResult::Parse("hPUd failed authorization\\Srequired\\n ");

    int failures = Expect(failed.HasValue(), "hPUd failed parse");
    if (failed.HasValue()) {
        failures += Expect(failed.Value().result == "failed", "hPUd failed result");
        failures += Expect(
            failed.Value().message == "authorization required\n",
            "hPUd failed decoded msg");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(failed.Value().ToPacket(), CharacterLoginResult::Spec);
        failures += Expect(rebuilt.HasValue(), "hPUd failed build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "hPUd failed authorization\\Srequired\\n ",
                "hPUd failed canonical");
        }
    }

    const cg::packet::PacketResult<CharacterLoginResult> unknown =
        CharacterLoginResult::Parse("hPUd pending message ");
    failures += Expect(unknown.HasValue(), "hPUd unknown result parse");
    if (unknown.HasValue()) {
        failures += Expect(unknown.Value().result == "pending", "hPUd unknown result preserved");
        failures += Expect(unknown.Value().message == "message", "hPUd unknown msg preserved");
    }
    return failures;
}

int TestBackslashAndGbkMessage()
{
    const cg::packet::PacketResult<CharacterLoginResult> backslash =
        CharacterLoginResult::Parse("hPUd failed path\\\\value ");

    int failures = Expect(backslash.HasValue(), "hPUd backslash parse");
    if (backslash.HasValue()) {
        failures += Expect(backslash.Value().message == "path\\value", "hPUd backslash decoded");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(backslash.Value().ToPacket(), CharacterLoginResult::Spec);
        failures += Expect(rebuilt.HasValue(), "hPUd backslash build");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == "hPUd failed path\\\\value ", "hPUd backslash canonical");
        }
    }

    std::string gbkWire = "hPUd failed ";
    gbkWire.push_back(static_cast<char>(0x81U));
    gbkWire.push_back(static_cast<char>(0x7CU));
    gbkWire.push_back(' ');
    const cg::packet::PacketResult<CharacterLoginResult> gbk =
        CharacterLoginResult::Parse(gbkWire);
    failures += Expect(gbk.HasValue(), "hPUd GBK parse");
    if (gbk.HasValue()) {
        failures += Expect(gbk.Value().message.size() == 2U, "hPUd GBK msg size");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(gbk.Value().ToPacket(), CharacterLoginResult::Spec);
        failures += Expect(rebuilt.HasValue(), "hPUd GBK build");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == gbkWire, "hPUd GBK canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        CharacterLoginResult::Parse("bad successful  "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "hPUd wrong code");
    failures += ExpectError(
        CharacterLoginResult::Parse("hPUd successful"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hPUd",
        true,
        1U,
        "hPUd missing field");
    failures += ExpectError(
        CharacterLoginResult::Parse("hPUd successful message extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hPUd",
        true,
        2U,
        "hPUd extra field");

    std::string invalidResult = "hPUd ";
    invalidResult.push_back(static_cast<char>(0x80U));
    invalidResult.append(" message ");
    failures += ExpectError(
        CharacterLoginResult::Parse(invalidResult),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hPUd",
        true,
        0U,
        "hPUd invalid result es");

    std::string invalidMessage = "hPUd failed ";
    invalidMessage.push_back(static_cast<char>(0x80U));
    invalidMessage.push_back(' ');
    failures += ExpectError(
        CharacterLoginResult::Parse(invalidMessage),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hPUd",
        true,
        1U,
        "hPUd invalid msg es");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestSuccessfulEmptyMessage()
        + TestEscapedMessageAndUnknownResult()
        + TestBackslashAndGbkMessage()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " character login result packet test(s) failed\n";
        return 1;
    }
    return 0;
}
