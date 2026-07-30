#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::FieldRecoveryItemUseRequest;

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

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }
    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int TestSpecAndEncode()
{
    const FieldRecoveryItemUseRequest request(10, 20, 8, 1);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(FieldRecoveryItemUseRequest::Code == "iVfo", "iVfo code");
    failures += Expect(
        FieldRecoveryItemUseRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "iVfo direction");
    failures += Expect(FieldRecoveryItemUseRequest::Fields.size() == 4U, "iVfo fields");
    failures += Expect(FieldRecoveryItemUseRequest::Fields[0].name == "x", "iVfo x name");
    failures += Expect(FieldRecoveryItemUseRequest::Fields[1].name == "y", "iVfo y name");
    failures += Expect(
        FieldRecoveryItemUseRequest::Fields[2].name == "haveitemindex",
        "iVfo haveitemindex name");
    failures += Expect(
        FieldRecoveryItemUseRequest::Fields[3].name == "toindex",
        "iVfo toindex name");
    for (const cg::packet::PacketFieldSpec& field : FieldRecoveryItemUseRequest::Fields) {
        failures += Expect(field.type == cg::packet::PacketFieldType::Int62, "iVfo field type");
    }
    failures += Expect(
        FieldRecoveryItemUseRequest::Spec.code == FieldRecoveryItemUseRequest::Code,
        "iVfo Spec code");
    failures += Expect(
        FieldRecoveryItemUseRequest::Spec.direction == FieldRecoveryItemUseRequest::Direction,
        "iVfo Spec direction");
    failures += Expect(
        FieldRecoveryItemUseRequest::Spec.fieldCount == 4U,
        "iVfo Spec field count");
    failures += Expect(request.x == 10, "iVfo x member");
    failures += Expect(request.y == 20, "iVfo y member");
    failures += Expect(request.haveitemindex == 8, "iVfo haveitemindex member");
    failures += Expect(request.toindex == 1, "iVfo toindex member");
    failures += Expect(packet.code == cg::packet::PacketCode("iVfo"), "iVfo packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "iVfo packet direction");
    failures += Expect(packet.fields.size() == 4U, "iVfo packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "iVfo x field");
    failures += ExpectField(packet, 1U, "y", "20", "iVfo y field");
    failures += ExpectField(
        packet,
        2U,
        "haveitemindex",
        "8",
        "iVfo haveitemindex field");
    failures += ExpectField(packet, 3U, "toindex", "1", "iVfo toindex field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "iVfo Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "iVfo a k 8 1 ", "iVfo Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("iVfo a k 8 1", FieldRecoveryItemUseRequest::Spec);

    int failures = Expect(parsed.HasValue(), "iVfo generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "10", "iVfo parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "20", "iVfo parsed y");
    failures += ExpectField(
        parsed.Value(),
        2U,
        "haveitemindex",
        "8",
        "iVfo parsed haveitemindex");
    failures += ExpectField(
        parsed.Value(),
        3U,
        "toindex",
        "1",
        "iVfo parsed toindex");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), FieldRecoveryItemUseRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "iVfo generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "iVfo a k 8 1 ", "iVfo generic canonical");
    }
    return failures;
}

int TestSignedAndExtremeValues()
{
    int failures = 0;
    const cg::packet::PacketResult<std::string> signedValues =
        FieldRecoveryItemUseRequest(-1, -2, -3, -4).Encode();
    failures += Expect(signedValues.HasValue(), "iVfo signed values");
    if (signedValues.HasValue()) {
        failures += Expect(
            signedValues.Value() == "iVfo -1 -2 -3 -4 ",
            "iVfo signed canonical");
    }

    const FieldRecoveryItemUseRequest extremes(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max());
    const cg::packet::PacketResult<std::string> extremeEncoded = extremes.Encode();
    failures += Expect(extremeEncoded.HasValue(), "iVfo cg3a int extreme values");
    if (extremeEncoded.HasValue()) {
        failures += Expect(
            extremeEncoded.Value() == "iVfo -2lkCB2 2lkCB1 -2lkCB2 2lkCB1 ",
            "iVfo cg3a int extreme canonical");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 4U>& tokens)
{
    return "iVfo " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ' + tokens[3] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k 8 1 ", FieldRecoveryItemUseRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "iVfo wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("iVfo a k 8", FieldRecoveryItemUseRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "iVfo",
        true,
        3U,
        "iVfo missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("iVfo a k 8 1 0 ", FieldRecoveryItemUseRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "iVfo",
        true,
        4U,
        "iVfo extra field");

    const std::array<std::string, 4U> validTokens = {{"a", "k", "8", "1"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 4U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(invalidTokens),
                FieldRecoveryItemUseRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "iVfo",
            true,
            index,
            "iVfo invalid i62");

        std::array<std::string, 4U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(overflowTokens),
                FieldRecoveryItemUseRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "iVfo",
            true,
            index,
            "iVfo i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestGenericRoundtrip()
        + TestSignedAndExtremeValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " field recovery item use request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
