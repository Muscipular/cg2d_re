#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::VersionUpgradeResult;

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
        packet.code == cg::packet::PacketCode("ed"),
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

int TestSpecAndSuccess()
{
    const VersionUpgradeResult result(0);
    const cg::packet::Packet packet = result.ToPacket();

    int failures = 0;
    failures += Expect(VersionUpgradeResult::Code == "ed", "ed code");
    failures += Expect(
        VersionUpgradeResult::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "ed direction");
    failures += Expect(
        VersionUpgradeResult::Fields.size() == 1U,
        "ed fields");
    failures += Expect(
        VersionUpgradeResult::Fields[0].name == "result",
        "ed result name");
    failures += Expect(
        VersionUpgradeResult::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "ed result type");
    failures += Expect(
        VersionUpgradeResult::Spec.code == VersionUpgradeResult::Code,
        "ed Spec code");
    failures += Expect(
        VersionUpgradeResult::Spec.direction ==
            VersionUpgradeResult::Direction,
        "ed Spec direction");
    failures += Expect(
        VersionUpgradeResult::Spec.fieldCount == 1U,
        "ed Spec field count");
    failures += Expect(result.result == 0, "ed member");
    failures += ExpectPacket(packet, "0", "ed packet");

    const cg::packet::PacketResult<VersionUpgradeResult> converted =
        VersionUpgradeResult::FromPacket(packet);
    failures += Expect(converted.HasValue(), "ed FromPacket valid");
    if (converted.HasValue()) {
        failures += Expect(converted.Value().result == 0, "ed FromPacket result");
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(packet, VersionUpgradeResult::Spec);
    failures += Expect(built.HasValue(), "ed success build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "ed 0 ", "ed success canonical");
    }
    return failures;
}

int TestTypedAndGenericRoundtrip()
{
    const cg::packet::PacketResult<VersionUpgradeResult> typed =
        VersionUpgradeResult::Parse("ed 0");

    int failures = Expect(typed.HasValue(), "ed typed parse");
    if (!typed.HasValue()) {
        return failures;
    }
    failures += Expect(typed.Value().result == 0, "ed typed value");

    const cg::packet::PacketResult<std::string> typedBuilt =
        cg::packet::BuildPacket(
            typed.Value().ToPacket(),
            VersionUpgradeResult::Spec);
    failures += Expect(typedBuilt.HasValue(), "ed typed build");
    if (typedBuilt.HasValue()) {
        failures += Expect(
            typedBuilt.Value() == "ed 0 ",
            "ed typed canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("ed 0", VersionUpgradeResult::Spec);
    failures += Expect(generic.HasValue(), "ed generic parse");
    if (!generic.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(generic.Value(), "0", "ed generic packet");

    const cg::packet::PacketResult<std::string> genericBuilt =
        cg::packet::BuildPacket(
            generic.Value(),
            VersionUpgradeResult::Spec);
    failures += Expect(genericBuilt.HasValue(), "ed generic build");
    if (genericBuilt.HasValue()) {
        failures += Expect(
            genericBuilt.Value() == "ed 0 ",
            "ed generic canonical");
    }
    return failures;
}

int TestSignedAndBusinessOutsideValues()
{
    struct Case
    {
        int result;
        std::string_view expected;
    };

    // 0..7 是已知业务结果；其他值与 int 边界仅证明 cg3a codec。
    constexpr Case Cases[] = {
        {0, "ed 0 "},
        {1, "ed 1 "},
        {2, "ed 2 "},
        {3, "ed 3 "},
        {4, "ed 4 "},
        {5, "ed 5 "},
        {6, "ed 6 "},
        {7, "ed 7 "},
        {10, "ed a "},
        {-1, "ed -1 "},
        {std::numeric_limits<int>::min(), "ed -2lkCB2 "},
        {std::numeric_limits<int>::max(), "ed 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const VersionUpgradeResult value(testCase.result);
        failures += ExpectPacket(
            value.ToPacket(),
            std::to_string(testCase.result),
            "ed signed packet");

        const cg::packet::PacketResult<std::string> encoded =
            cg::packet::BuildPacket(
                value.ToPacket(),
                VersionUpgradeResult::Spec);
        failures += Expect(encoded.HasValue(), "ed signed build");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "ed signed canonical");
        }

        const cg::packet::PacketResult<VersionUpgradeResult> parsed =
            VersionUpgradeResult::Parse(testCase.expected);
        failures += Expect(parsed.HasValue(), "ed signed parse");
        if (parsed.HasValue()) {
            failures += Expect(
                parsed.Value().result == testCase.result,
                "ed signed value");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        VersionUpgradeResult::Parse("bad 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "ed wrong code");
    failures += ExpectError(
        VersionUpgradeResult::Parse("ed"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ed",
        true,
        0U,
        "ed missing field");
    failures += ExpectError(
        VersionUpgradeResult::Parse("ed 0 1 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ed",
        true,
        1U,
        "ed extra field");
    failures += ExpectError(
        VersionUpgradeResult::Parse("ed _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ed",
        true,
        0U,
        "ed invalid i62");
    failures += ExpectError(
        VersionUpgradeResult::Parse("ed 2lkCB2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ed",
        true,
        0U,
        "ed positive overflow");
    failures += ExpectError(
        VersionUpgradeResult::Parse("ed -2lkCB3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ed",
        true,
        0U,
        "ed negative overflow");

    const std::string pollutedCode = "ed\x17" "x";
    const std::string pollutedWire = pollutedCode + " 0";
    failures += ExpectError(
        VersionUpgradeResult::Parse(pollutedWire),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "ed polluted code");

    const std::string pollutedField = "ed 0\x17" "x";
    failures += ExpectError(
        VersionUpgradeResult::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ed",
        true,
        1U,
        "ed polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = VersionUpgradeResult(0).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        VersionUpgradeResult::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "ed FromPacket wrong code");

    cg::packet::Packet wrongDirection = VersionUpgradeResult(0).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        VersionUpgradeResult::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ed",
        false,
        0U,
        "ed FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("ed"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        VersionUpgradeResult::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ed",
        true,
        0U,
        "ed FromPacket missing field");

    cg::packet::Packet extraField = VersionUpgradeResult(0).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "1",
    });
    failures += ExpectError(
        VersionUpgradeResult::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ed",
        true,
        1U,
        "ed FromPacket extra field");

    cg::packet::Packet wrongName = VersionUpgradeResult(0).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        VersionUpgradeResult::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ed",
        true,
        0U,
        "ed FromPacket wrong name");

    cg::packet::Packet wrongType = VersionUpgradeResult(0).ToPacket();
    wrongType.fields[0].type =
        cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        VersionUpgradeResult::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "ed",
        true,
        0U,
        "ed FromPacket wrong type");

    cg::packet::Packet invalidValue = VersionUpgradeResult(0).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        VersionUpgradeResult::FromPacket(invalidValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ed",
        true,
        0U,
        "ed FromPacket invalid decimal");

    cg::packet::Packet overflowValue = VersionUpgradeResult(0).ToPacket();
    overflowValue.fields[0].value = "2147483648";
    failures += ExpectError(
        VersionUpgradeResult::FromPacket(overflowValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ed",
        true,
        0U,
        "ed FromPacket decimal overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndSuccess()
        + TestTypedAndGenericRoundtrip()
        + TestSignedAndBusinessOutsideValues()
        + TestParseErrors()
        + TestFromPacketErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " version upgrade result packet test(s) failed\n";
        return 1;
    }
    return 0;
}
