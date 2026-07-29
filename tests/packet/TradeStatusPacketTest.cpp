#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

using cg::packet::receive::TradeStatus;
using cg::packet::receive::TradeStatusKind;

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

cg::packet::Packet MakePacket(
    cg::packet::PacketCode code,
    cg::packet::PacketDirection direction,
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string value)
{
    cg::packet::Packet packet(std::move(code), direction);
    packet.fields.push_back(cg::packet::PacketField {
        name,
        type,
        std::move(value),
    });
    return packet;
}

int TestSpecEnumAndToPacket()
{
    static_assert(static_cast<int>(TradeStatusKind::Terminated) == 0);
    static_assert(static_cast<int>(TradeStatusKind::CounterpartyConfirmed) == 1);
    static_assert(static_cast<int>(TradeStatusKind::Completed) == 2);

    const TradeStatus status(TradeStatusKind::CounterpartyConfirmed);
    const cg::packet::Packet packet = status.ToPacket();

    int failures = 0;
    failures += Expect(TradeStatus::Code == "We", "We code");
    failures += Expect(
        TradeStatus::Direction == cg::packet::PacketDirection::ServerToClient,
        "We direction");
    failures += Expect(TradeStatus::Fields.size() == 1U, "We fields");
    failures += Expect(TradeStatus::Fields[0].name == "status", "We field name");
    failures += Expect(
        TradeStatus::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "We field type");
    failures += Expect(TradeStatus::Spec.code == TradeStatus::Code, "We Spec code");
    failures += Expect(TradeStatus::Spec.direction == TradeStatus::Direction, "We Spec direction");
    failures += Expect(TradeStatus::Spec.fieldCount == 1U, "We Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("We"), "We packet code");
    failures += Expect(packet.direction == TradeStatus::Direction, "We packet direction");
    failures += Expect(packet.fields.size() == 1U, "We packet fields");
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "status", "We packet field name");
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            "We packet field type");
        failures += Expect(packet.fields[0].value == "1", "We packet field value");
    }
    return failures;
}

int TestOfficialStatusesAndCanonical()
{
    struct Case
    {
        std::string_view input;
        TradeStatusKind expected;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {"We 0", TradeStatusKind::Terminated, "We 0 "},
        {"We 1 ", TradeStatusKind::CounterpartyConfirmed, "We 1 "},
        {"We 2", TradeStatusKind::Completed, "We 2 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const cg::packet::PacketResult<TradeStatus> parsed = TradeStatus::Parse(testCase.input);
        failures += Expect(parsed.HasValue(), "We official status parse");
        if (!parsed.HasValue()) {
            continue;
        }
        failures += Expect(parsed.Value().status == testCase.expected, "We official status value");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(parsed.Value().ToPacket(), TradeStatus::Spec);
        failures += Expect(built.HasValue(), "We typed build");
        if (built.HasValue()) {
            failures += Expect(built.Value() == testCase.canonical, "We typed canonical");
        }
    }
    return failures;
}

int TestGenericUnknownPreserved()
{
    int failures = 0;
    for (const std::string_view input : {"We -1", "We 3 "}) {
        const cg::packet::PacketResult<cg::packet::Packet> parsed =
            cg::packet::ParsePacket(input, TradeStatus::Spec);
        failures += Expect(parsed.HasValue(), "We generic unknown parse");
        if (!parsed.HasValue()) {
            continue;
        }

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(parsed.Value(), TradeStatus::Spec);
        failures += Expect(built.HasValue(), "We generic unknown build");
        if (built.HasValue()) {
            const std::string_view expected = input.starts_with("We -1") ? "We -1 " : "We 3 ";
            failures += Expect(built.Value() == expected, "We generic unknown preserved");
        }
    }
    return failures;
}

int TestTypedUnknownRejected()
{
    int failures = 0;
    failures += ExpectError(
        TradeStatus::Parse("We -1 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "We",
        true,
        0U,
        "We typed negative unknown");
    failures += ExpectError(
        TradeStatus::Parse("We 3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "We",
        true,
        0U,
        "We typed positive unknown");

    const cg::packet::Packet unknown = MakePacket(
        cg::packet::PacketCode("We"),
        cg::packet::PacketDirection::ServerToClient,
        "status",
        cg::packet::PacketFieldType::Int62,
        "3");
    failures += ExpectError(
        TradeStatus::FromPacket(unknown),
        cg::packet::PacketErrorCode::DecodeFailed,
        "We",
        true,
        0U,
        "We FromPacket unknown");
    return failures;
}

int TestFromPacketValidation()
{
    int failures = 0;
    failures += ExpectError(
        TradeStatus::FromPacket(MakePacket(
            cg::packet::PacketCode("bad"),
            cg::packet::PacketDirection::ServerToClient,
            "status",
            cg::packet::PacketFieldType::Int62,
            "0")),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "We FromPacket wrong code");
    failures += ExpectError(
        TradeStatus::FromPacket(MakePacket(
            cg::packet::PacketCode("We"),
            cg::packet::PacketDirection::ClientToServer,
            "status",
            cg::packet::PacketFieldType::Int62,
            "0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "We",
        false,
        0U,
        "We FromPacket wrong direction");
    failures += ExpectError(
        TradeStatus::FromPacket(MakePacket(
            cg::packet::PacketCode("We"),
            cg::packet::PacketDirection::ServerToClient,
            "flag",
            cg::packet::PacketFieldType::Int62,
            "0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "We",
        true,
        0U,
        "We FromPacket wrong name");
    failures += ExpectError(
        TradeStatus::FromPacket(MakePacket(
            cg::packet::PacketCode("We"),
            cg::packet::PacketDirection::ServerToClient,
            "status",
            cg::packet::PacketFieldType::EscapedString,
            "0")),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "We",
        true,
        0U,
        "We FromPacket wrong type");
    return failures;
}

int TestWireErrors()
{
    int failures = 0;
    failures += ExpectError(
        TradeStatus::Parse("bad 1 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "We wrong code");
    failures += ExpectError(
        TradeStatus::Parse("We"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "We",
        true,
        0U,
        "We missing field");
    failures += ExpectError(
        TradeStatus::Parse("We 1 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "We",
        true,
        1U,
        "We extra field");
    failures += ExpectError(
        TradeStatus::Parse("We _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "We",
        true,
        0U,
        "We invalid i62");
    failures += ExpectError(
        TradeStatus::Parse("We 2lkCB2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "We",
        true,
        0U,
        "We i62 overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecEnumAndToPacket()
        + TestOfficialStatusesAndCanonical()
        + TestGenericUnknownPreserved()
        + TestTypedUnknownRejected()
        + TestFromPacketValidation()
        + TestWireErrors();
    if (failures != 0) {
        std::cerr << failures << " trade status packet test(s) failed\n";
        return 1;
    }
    return 0;
}
