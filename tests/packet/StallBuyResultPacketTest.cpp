#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::StallBuyResult;

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

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("Yx"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "result", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const StallBuyResult result(0);
    const cg::packet::Packet packet = result.ToPacket();

    int failures = 0;
    failures += Expect(StallBuyResult::Code == "Yx", "Yx code");
    failures += Expect(
        StallBuyResult::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "Yx direction");
    failures += Expect(StallBuyResult::Fields.size() == 1U, "Yx fields");
    failures += Expect(
        StallBuyResult::Fields[0].name == "result",
        "Yx result name");
    failures += Expect(
        StallBuyResult::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "Yx result type");
    failures += Expect(
        StallBuyResult::Spec.code == StallBuyResult::Code,
        "Yx Spec code");
    failures += Expect(
        StallBuyResult::Spec.direction == StallBuyResult::Direction,
        "Yx Spec direction");
    failures += Expect(
        StallBuyResult::Spec.fieldCount == 1U,
        "Yx Spec field count");
    failures += Expect(result.result == 0, "Yx member");
    failures += ExpectPacket(packet, "0", "Yx ToPacket");

    const cg::packet::PacketResult<StallBuyResult> converted =
        StallBuyResult::FromPacket(packet);
    failures += Expect(converted.HasValue(), "Yx FromPacket valid");
    if (converted.HasValue()) {
        failures += Expect(
            converted.Value().result == 0,
            "Yx FromPacket result");
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(packet, StallBuyResult::Spec);
    failures += Expect(built.HasValue(), "Yx build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "Yx 0 ", "Yx canonical");
    }

    const cg::packet::PacketResult<StallBuyResult> typed =
        StallBuyResult::Parse("Yx 0");
    failures += Expect(typed.HasValue(), "Yx typed parse");
    if (typed.HasValue()) {
        failures += Expect(typed.Value().result == 0, "Yx typed value");
        const cg::packet::PacketResult<std::string> typedBuilt =
            cg::packet::BuildPacket(
                typed.Value().ToPacket(),
                StallBuyResult::Spec);
        failures += Expect(typedBuilt.HasValue(), "Yx typed build");
        if (typedBuilt.HasValue()) {
            failures += Expect(
                typedBuilt.Value() == "Yx 0 ",
                "Yx typed canonical");
        }
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("Yx 0", StallBuyResult::Spec);
    failures += Expect(generic.HasValue(), "Yx generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(generic.Value(), "0", "Yx generic packet");
        const cg::packet::PacketResult<std::string> genericBuilt =
            cg::packet::BuildPacket(generic.Value(), StallBuyResult::Spec);
        failures += Expect(genericBuilt.HasValue(), "Yx generic build");
        if (genericBuilt.HasValue()) {
            failures += Expect(
                genericBuilt.Value() == "Yx 0 ",
                "Yx generic canonical");
        }
    }
    return failures;
}

int TestSignedAndBusinessValues()
{
    struct Case
    {
        int result;
        std::string_view expected;
    };

    // 0..12/100 与 xlsx 13 仅为业务样本；其他值证明 codec 不收窄 wire 值域。
    constexpr Case Cases[] = {
        {0, "Yx 0 "},
        {1, "Yx 1 "},
        {12, "Yx c "},
        {13, "Yx d "},
        {100, "Yx 1C "},
        {42, "Yx G "},
        {-1, "Yx -1 "},
        {std::numeric_limits<int>::min(), "Yx -2lkCB2 "},
        {std::numeric_limits<int>::max(), "Yx 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const StallBuyResult value(testCase.result);
        failures += ExpectPacket(
            value.ToPacket(),
            std::to_string(testCase.result),
            "Yx signed packet");

        const cg::packet::PacketResult<std::string> encoded =
            cg::packet::BuildPacket(
                value.ToPacket(),
                StallBuyResult::Spec);
        failures += Expect(encoded.HasValue(), "Yx signed build");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "Yx signed canonical");
        }

        const cg::packet::PacketResult<StallBuyResult> parsed =
            StallBuyResult::Parse(testCase.expected);
        failures += Expect(parsed.HasValue(), "Yx signed parse");
        if (parsed.HasValue()) {
            failures += Expect(
                parsed.Value().result == testCase.result,
                "Yx signed value");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        StallBuyResult::Parse("bad 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Yx wrong code");
    failures += ExpectError(
        StallBuyResult::Parse("Yx"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Yx",
        true,
        0U,
        "Yx missing field");
    failures += ExpectError(
        StallBuyResult::Parse("Yx 0 1 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Yx",
        true,
        1U,
        "Yx extra field");
    failures += ExpectError(
        StallBuyResult::Parse("Yx _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yx",
        true,
        0U,
        "Yx invalid i62");
    failures += ExpectError(
        StallBuyResult::Parse("Yx 2lkCB2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yx",
        true,
        0U,
        "Yx positive overflow");
    failures += ExpectError(
        StallBuyResult::Parse("Yx -2lkCB3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yx",
        true,
        0U,
        "Yx negative overflow");

    const std::string pollutedCode = "Yx\x17" "x";
    const std::string pollutedWire = pollutedCode + " 0";
    failures += ExpectError(
        StallBuyResult::Parse(pollutedWire),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "Yx polluted code");

    const std::string pollutedField = "Yx 0\x17" "x";
    failures += ExpectError(
        StallBuyResult::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Yx",
        true,
        1U,
        "Yx polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = StallBuyResult(0).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        StallBuyResult::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Yx FromPacket wrong code");

    cg::packet::Packet wrongDirection = StallBuyResult(0).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        StallBuyResult::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yx",
        false,
        0U,
        "Yx FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("Yx"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        StallBuyResult::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Yx",
        true,
        0U,
        "Yx FromPacket missing field");

    cg::packet::Packet extraField = StallBuyResult(0).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "1",
    });
    failures += ExpectError(
        StallBuyResult::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Yx",
        true,
        1U,
        "Yx FromPacket extra field");

    cg::packet::Packet wrongName = StallBuyResult(0).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        StallBuyResult::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yx",
        true,
        0U,
        "Yx FromPacket wrong name");

    cg::packet::Packet wrongType = StallBuyResult(0).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        StallBuyResult::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "Yx",
        true,
        0U,
        "Yx FromPacket wrong type");

    cg::packet::Packet invalidValue = StallBuyResult(0).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        StallBuyResult::FromPacket(invalidValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yx",
        true,
        0U,
        "Yx FromPacket invalid decimal");

    cg::packet::Packet positiveOverflow = StallBuyResult(0).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        StallBuyResult::FromPacket(positiveOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yx",
        true,
        0U,
        "Yx FromPacket positive decimal overflow");

    cg::packet::Packet negativeOverflow = StallBuyResult(0).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        StallBuyResult::FromPacket(negativeOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Yx",
        true,
        0U,
        "Yx FromPacket negative decimal overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndRoundtrip()
        + TestSignedAndBusinessValues()
        + TestParseErrors()
        + TestFromPacketErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " stall buy result packet test(s) failed\n";
        return 1;
    }
    return 0;
}
