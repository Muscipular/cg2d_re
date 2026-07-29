#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::CharacterLogoutResult;

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
    const CharacterLogoutResult result("successful", "");
    const cg::packet::Packet packet = result.ToPacket();

    int failures = 0;
    failures += Expect(CharacterLogoutResult::Code == "HMNy", "HMNy code");
    failures += Expect(
        CharacterLogoutResult::Direction == cg::packet::PacketDirection::ServerToClient,
        "HMNy direction");
    failures += Expect(CharacterLogoutResult::Fields.size() == 2U, "HMNy fields");
    failures += Expect(CharacterLogoutResult::Fields[0].name == "result", "HMNy result name");
    failures += Expect(CharacterLogoutResult::Fields[1].name == "msg", "HMNy msg name");
    failures += Expect(
        CharacterLogoutResult::Fields[0].type == cg::packet::PacketFieldType::EscapedString,
        "HMNy result type");
    failures += Expect(
        CharacterLogoutResult::Fields[1].type == cg::packet::PacketFieldType::EscapedString,
        "HMNy msg type");
    failures += Expect(CharacterLogoutResult::Spec.code == CharacterLogoutResult::Code, "HMNy Spec code");
    failures += Expect(
        CharacterLogoutResult::Spec.direction == CharacterLogoutResult::Direction,
        "HMNy Spec direction");
    failures += Expect(CharacterLogoutResult::Spec.fieldCount == 2U, "HMNy Spec count");
    failures += Expect(packet.code == cg::packet::PacketCode("HMNy"), "HMNy packet code");
    failures += Expect(packet.direction == CharacterLogoutResult::Direction, "HMNy packet direction");
    failures += Expect(packet.fields.size() == 2U, "HMNy packet fields");
    if (packet.fields.size() == 2U) {
        failures += Expect(packet.fields[0].name == "result", "HMNy packet result name");
        failures += Expect(packet.fields[0].value == "successful", "HMNy packet result value");
        failures += Expect(packet.fields[1].name == "msg", "HMNy packet msg name");
        failures += Expect(packet.fields[1].value.empty(), "HMNy packet empty msg");
    }
    return failures;
}

int TestSuccessfulEmptyMessage()
{
    const cg::packet::PacketResult<CharacterLogoutResult> parsed =
        CharacterLogoutResult::Parse("HMNy successful  ");

    int failures = Expect(parsed.HasValue(), "HMNy successful parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().result == "successful", "HMNy successful result");
    failures += Expect(parsed.Value().message.empty(), "HMNy successful empty msg");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), CharacterLogoutResult::Spec);
    failures += Expect(built.HasValue(), "HMNy successful build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "HMNy successful  ", "HMNy successful canonical");
    }
    return failures;
}

int TestEscapedMessageAndUnknownResult()
{
    const std::string wire = "HMNy failed Not\\SLogged\\Sin\\n ";
    const cg::packet::PacketResult<CharacterLogoutResult> failed =
        CharacterLogoutResult::Parse(wire);

    int failures = Expect(failed.HasValue(), "HMNy failed parse");
    if (failed.HasValue()) {
        failures += Expect(failed.Value().result == "failed", "HMNy failed result");
        failures += Expect(failed.Value().message == "Not Logged in\n", "HMNy failed message");
        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(failed.Value().ToPacket(), CharacterLogoutResult::Spec);
        failures += Expect(built.HasValue(), "HMNy failed build");
        if (built.HasValue()) {
            failures += Expect(built.Value() == wire, "HMNy failed canonical");
        }
    }

    const cg::packet::PacketResult<CharacterLogoutResult> unknown =
        CharacterLogoutResult::Parse("HMNy pending message ");
    failures += Expect(unknown.HasValue(), "HMNy unknown result parse");
    if (unknown.HasValue()) {
        failures += Expect(unknown.Value().result == "pending", "HMNy unknown result retained");
        failures += Expect(unknown.Value().message == "message", "HMNy unknown message");
    }
    return failures;
}

int TestGbkMessage()
{
    std::string wire = "HMNy failed ";
    wire.push_back(static_cast<char>(0x81U));
    wire.push_back(static_cast<char>(0x7CU));
    wire.push_back(' ');

    const cg::packet::PacketResult<CharacterLogoutResult> parsed =
        CharacterLogoutResult::Parse(wire);
    int failures = Expect(parsed.HasValue(), "HMNy GBK parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().message.size() == 2U, "HMNy GBK message size");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), CharacterLogoutResult::Spec);
    failures += Expect(built.HasValue(), "HMNy GBK build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == wire, "HMNy GBK canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        CharacterLogoutResult::Parse("bad successful  "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "HMNy wrong code");
    failures += ExpectError(
        CharacterLogoutResult::Parse("HMNy successful"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "HMNy",
        true,
        1U,
        "HMNy missing field");
    failures += ExpectError(
        CharacterLogoutResult::Parse("HMNy failed message extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "HMNy",
        true,
        2U,
        "HMNy extra field");

    std::string invalidResult = "HMNy ";
    invalidResult.push_back(static_cast<char>(0x80U));
    invalidResult.append(" message ");
    failures += ExpectError(
        CharacterLogoutResult::Parse(invalidResult),
        cg::packet::PacketErrorCode::DecodeFailed,
        "HMNy",
        true,
        0U,
        "HMNy invalid result es");

    std::string invalidMessage = "HMNy failed ";
    invalidMessage.push_back(static_cast<char>(0x80U));
    invalidMessage.push_back(' ');
    failures += ExpectError(
        CharacterLogoutResult::Parse(invalidMessage),
        cg::packet::PacketErrorCode::DecodeFailed,
        "HMNy",
        true,
        1U,
        "HMNy invalid msg es");

    std::string truncatedResult = "HMNy ";
    truncatedResult.push_back(static_cast<char>(0x81U));
    truncatedResult.append(" message ");
    failures += ExpectError(
        CharacterLogoutResult::Parse(truncatedResult),
        cg::packet::PacketErrorCode::DecodeFailed,
        "HMNy",
        true,
        0U,
        "HMNy truncated result GBK");

    std::string truncatedMessage = "HMNy failed ";
    truncatedMessage.push_back(static_cast<char>(0x81U));
    truncatedMessage.push_back(' ');
    failures += ExpectError(
        CharacterLogoutResult::Parse(truncatedMessage),
        cg::packet::PacketErrorCode::DecodeFailed,
        "HMNy",
        true,
        1U,
        "HMNy truncated message GBK");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestSuccessfulEmptyMessage()
        + TestEscapedMessageAndUnknownResult()
        + TestGbkMessage()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " character logout result packet test(s) failed\n";
        return 1;
    }
    return 0;
}
