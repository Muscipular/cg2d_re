#include "packet/PacketTypes.hpp"
#include "packet/receive.hpp"
#include "packet/send.hpp"

#include <iostream>
#include <string>

namespace {

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

int TestPacketExpression()
{
    int failures = 0;
    cg::packet::send::ClientLogin login("hash", "account", "password");
    const cg::packet::Packet packet = login.ToPacket();

    failures += Expect(packet.code == cg::packet::PacketCode("JFVf"), "ClientLogin code");
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, "ClientLogin direction");
    failures += Expect(packet.source == cg::packet::PacketSourceTag::ExistingPrototype, "ClientLogin source");
    failures += Expect(packet.fields.size() == 3, "ClientLogin field count");
    if (packet.fields.size() != 3) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "clientHash", "ClientLogin clientHash name");
    failures += Expect(
        packet.fields[0].type == cg::packet::PacketFieldType::EscapedString,
        "ClientLogin clientHash type");
    failures += Expect(packet.fields[1].name == "password", "ClientLogin password name");
    failures += Expect(packet.fields[2].name == "account", "ClientLogin account name");
    return failures;
}

int TestReceivePacketExpression()
{
    int failures = 0;
    cg::packet::receive::ClientLoginResult loginResult(
        cg::packet::receive::ClientLoginResult::ClientLogin_AUTH_ERROR_OTHER,
        "busy");
    const cg::packet::Packet packet = loginResult.ToPacket();

    failures += Expect(packet.code == cg::packet::PacketCode("whV"), "ClientLoginResult code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        "ClientLoginResult direction");
    failures += Expect(packet.fields.size() == 2, "ClientLoginResult field count");
    if (packet.fields.size() != 2) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "result", "ClientLoginResult result name");
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, "ClientLoginResult result type");
    failures += Expect(packet.fields[1].name == "data", "ClientLoginResult data name");
    failures += Expect(
        packet.fields[1].type == cg::packet::PacketFieldType::EscapedString,
        "ClientLoginResult data type");
    return failures;
}

int TestPacketResult()
{
    int failures = 0;
    const cg::packet::PacketResult<int> success = cg::packet::PacketResult<int>::Success(7);
    const bool successHasValue = success.HasValue();
    failures += Expect(static_cast<bool>(success), "PacketResult success bool");
    failures += Expect(successHasValue, "PacketResult success has value");
    failures += Expect(!success.HasError(), "PacketResult success has no error");
    if (successHasValue) {
        failures += Expect(success.Value() == 7, "PacketResult success value");
    }

    cg::packet::PacketError error =
        cg::packet::PacketError::Make(cg::packet::PacketErrorCode::FieldCountMismatch, "字段数量不匹配");
    error.packetCode = cg::packet::PacketCode("whV");
    error.fieldIndex = 1U;
    const cg::packet::PacketResult<cg::packet::receive::ClientLoginResult> failure =
        cg::packet::PacketResult<cg::packet::receive::ClientLoginResult>::Failure(error);

    const bool failureHasError = failure.HasError();
    failures += Expect(!static_cast<bool>(failure), "PacketResult failure bool");
    failures += Expect(!failure.HasValue(), "PacketResult failure has no value");
    failures += Expect(failureHasError, "PacketResult failure has error");
    if (failureHasError) {
        const cg::packet::PacketError& actualError = failure.Error();
        failures += Expect(
            actualError.code == cg::packet::PacketErrorCode::FieldCountMismatch,
            "PacketResult failure error code");
        failures += Expect(actualError.packetCode.has_value(), "PacketResult failure packet code exists");
        if (actualError.packetCode.has_value()) {
            failures += Expect(actualError.packetCode->View() == "whV", "PacketResult failure packet code");
        }
        failures += Expect(actualError.fieldIndex == 1U, "PacketResult failure field index");
    }
    return failures;
}

int TestTodoResultShape()
{
    int failures = 0;
    const cg::packet::PacketResult<cg::packet::receive::ClientLoginResult> emptyParse =
        cg::packet::receive::ClientLoginResult::Parse("");
    const bool emptyParseHasError = emptyParse.HasError();
    failures += Expect(emptyParseHasError, "ClientLoginResult empty parse has error");
    if (emptyParseHasError) {
        failures += Expect(
            emptyParse.Error().code == cg::packet::PacketErrorCode::EmptyInput,
            "ClientLoginResult empty parse error code");
    }

    const cg::packet::send::ClientLogin login("hash", "account", "password");
    const cg::packet::PacketResult<std::string> encoded = login.Encode();
    const bool encodedHasError = encoded.HasError();
    failures += Expect(encodedHasError, "ClientLogin encode has error");
    if (encodedHasError) {
        failures += Expect(
            encoded.Error().code == cg::packet::PacketErrorCode::NotImplemented,
            "ClientLogin encode error code");
    }
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestPacketExpression();
    failures += TestReceivePacketExpression();
    failures += TestPacketResult();
    failures += TestTodoResultShape();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
