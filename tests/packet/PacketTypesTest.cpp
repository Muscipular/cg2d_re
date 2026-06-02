#include "packet/PacketBuilder.hpp"
#include "packet/PacketTypes.hpp"
#include "packet/receive.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

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

int TestPacketExpression()
{
    int failures = 0;
    cg::packet::send::ClientLogin login("hash", "account", "password");
    const cg::packet::Packet packet = login.ToPacket();

    failures += Expect(packet.code == cg::packet::PacketCode("JFVf"), "ClientLogin code");
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, "ClientLogin direction");
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

int TestClientLoginResultParseAndBuild()
{
    int failures = 0;

    const cg::packet::PacketResult<cg::packet::receive::ClientLoginResult> emptyData =
        cg::packet::receive::ClientLoginResult::Parse("whV 0  ");
    failures += Expect(static_cast<bool>(emptyData), "ClientLoginResult parse empty data");
    if (emptyData.HasValue()) {
        failures += Expect(
            emptyData.Value().result == cg::packet::receive::ClientLoginResult::ClientLogin_AUTH_OK,
            "ClientLoginResult empty data result");
        failures += Expect(emptyData.Value().data.empty(), "ClientLoginResult empty data value");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(emptyData.Value().ToPacket(), cg::packet::receive::ClientLoginResult::Spec);
        failures += Expect(static_cast<bool>(built), "ClientLoginResult build empty data");
        if (built.HasValue()) {
            failures += Expect(built.Value() == "whV 0  ", "ClientLoginResult build empty data value");
        }
    }

    const cg::packet::PacketResult<cg::packet::receive::ClientLoginResult> busy =
        cg::packet::receive::ClientLoginResult::Parse("whV 9 busy ");
    failures += Expect(static_cast<bool>(busy), "ClientLoginResult parse busy");
    if (busy.HasValue()) {
        failures += Expect(
            busy.Value().result == cg::packet::receive::ClientLoginResult::ClientLogin_AUTH_ERROR_OTHER,
            "ClientLoginResult busy result");
        failures += Expect(busy.Value().data == "busy", "ClientLoginResult busy data");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(busy.Value().ToPacket(), cg::packet::receive::ClientLoginResult::Spec);
        failures += Expect(static_cast<bool>(built), "ClientLoginResult build busy");
        if (built.HasValue()) {
            failures += Expect(built.Value() == "whV 9 busy ", "ClientLoginResult build busy value");
        }
    }

    const cg::packet::PacketResult<cg::packet::receive::ClientLoginResult> faild =
        cg::packet::receive::ClientLoginResult::Parse("whV 1 faild ");
    failures += Expect(static_cast<bool>(faild), "ClientLoginResult parse faild");
    if (faild.HasValue()) {
        failures += Expect(faild.Value().data == "faild", "ClientLoginResult faild data");
    }

    const cg::packet::PacketResult<cg::packet::receive::ClientLoginResult> incorrect =
        cg::packet::receive::ClientLoginResult::Parse("whV 2 incorrect ");
    failures += Expect(static_cast<bool>(incorrect), "ClientLoginResult parse incorrect");
    if (incorrect.HasValue()) {
        failures += Expect(incorrect.Value().data == "incorrect", "ClientLoginResult incorrect data");
    }

    return failures;
}

int TestClientLoginResultParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::ClientLoginResult::Parse("JFVf hash pass account "),
        cg::packet::PacketErrorCode::InvalidCode,
        "JFVf",
        false,
        0U,
        "ClientLoginResult wrong code");
    failures += ExpectError(
        cg::packet::receive::ClientLoginResult::Parse("whV 0"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "whV",
        true,
        1U,
        "ClientLoginResult missing data");
    failures += ExpectError(
        cg::packet::receive::ClientLoginResult::Parse("whV _ busy "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "whV",
        true,
        0U,
        "ClientLoginResult invalid result");
    failures += ExpectError(
        cg::packet::receive::ClientLoginResult::Parse("whV 9 busy extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "whV",
        true,
        2U,
        "ClientLoginResult extra field");
    return failures;
}

int TestClientLoginResultBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = cg::packet::receive::ClientLoginResult(9, "busy").ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongDirection, cg::packet::receive::ClientLoginResult::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "whV",
        false,
        0U,
        "ClientLoginResult build wrong direction");

    cg::packet::Packet wrongName = cg::packet::receive::ClientLoginResult(9, "busy").ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        cg::packet::BuildPacket(wrongName, cg::packet::receive::ClientLoginResult::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "whV",
        true,
        0U,
        "ClientLoginResult build wrong field name");

    cg::packet::Packet wrongType = cg::packet::receive::ClientLoginResult(9, "busy").ToPacket();
    wrongType.fields[1].type = cg::packet::PacketFieldType::Unknown;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongType, cg::packet::receive::ClientLoginResult::Spec),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "whV",
        true,
        1U,
        "ClientLoginResult build wrong field type");
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

int TestClientLoginEncode()
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
    failures += Expect(static_cast<bool>(encoded), "ClientLogin encode success");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "JFVf hash password account ", "ClientLogin encode value");
    }
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestPacketExpression();
    failures += TestReceivePacketExpression();
    failures += TestClientLoginResultParseAndBuild();
    failures += TestClientLoginResultParseErrors();
    failures += TestClientLoginResultBuildErrors();
    failures += TestPacketResult();
    failures += TestClientLoginEncode();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
