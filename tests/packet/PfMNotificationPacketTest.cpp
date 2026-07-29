#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::receive::PfMNotification;

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
    const PfMNotification notification(1);
    const cg::packet::Packet packet = notification.ToPacket();

    int failures = 0;
    failures += Expect(PfMNotification::Code == "PfM", "PfM code");
    failures += Expect(
        PfMNotification::Direction == cg::packet::PacketDirection::ServerToClient,
        "PfM direction");
    failures += Expect(PfMNotification::Fields.size() == 1U, "PfM fields");
    failures += Expect(PfMNotification::Fields[0].name == "TrialFlg", "PfM field name");
    failures += Expect(
        PfMNotification::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "PfM field type");
    failures += Expect(PfMNotification::Spec.code == PfMNotification::Code, "PfM Spec code");
    failures += Expect(
        PfMNotification::Spec.direction == PfMNotification::Direction,
        "PfM Spec direction");
    failures += Expect(PfMNotification::Spec.fieldCount == 1U, "PfM Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("PfM"), "PfM packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        "PfM packet direction");
    failures += Expect(packet.fields.size() == 1U, "PfM packet fields");
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "TrialFlg", "PfM packet field name");
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            "PfM packet field type");
        failures += Expect(packet.fields[0].value == "1", "PfM packet field value");
    }
    return failures;
}

int TestParseAndCanonicalRoundtrip()
{
    const cg::packet::PacketResult<PfMNotification> parsed = PfMNotification::Parse("PfM 1 ");

    int failures = Expect(parsed.HasValue(), "PfM parse one");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().value == 1, "PfM parsed value");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), PfMNotification::Spec);
    failures += Expect(built.HasValue(), "PfM build canonical");
    if (!built.HasValue()) {
        return failures;
    }
    failures += Expect(built.Value() == "PfM 1 ", "PfM canonical value");

    const cg::packet::PacketResult<PfMNotification> reparsed =
        PfMNotification::Parse(built.Value());
    failures += Expect(reparsed.HasValue(), "PfM reparse canonical");
    if (reparsed.HasValue()) {
        failures += Expect(reparsed.Value().value == 1, "PfM reparsed value");
    }
    return failures;
}

int TestNegativeValue()
{
    const cg::packet::PacketResult<PfMNotification> parsed = PfMNotification::Parse("PfM -1 ");

    int failures = Expect(parsed.HasValue(), "PfM parse negative");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().value == -1, "PfM negative value");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), PfMNotification::Spec);
    failures += Expect(built.HasValue(), "PfM build negative");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "PfM -1 ", "PfM negative canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        PfMNotification::Parse("bad 1 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "PfM wrong code");
    failures += ExpectError(
        PfMNotification::Parse("PfM"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PfM",
        true,
        0U,
        "PfM missing field");
    failures += ExpectError(
        PfMNotification::Parse("PfM 1 2 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PfM",
        true,
        1U,
        "PfM extra field");
    failures += ExpectError(
        PfMNotification::Parse("PfM _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "PfM",
        true,
        0U,
        "PfM invalid i62");
    failures += ExpectError(
        PfMNotification::Parse("PfM 2lkCB2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "PfM",
        true,
        0U,
        "PfM i62 overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket() + TestParseAndCanonicalRoundtrip() + TestNegativeValue() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " PfM notification packet test(s) failed\n";
        return 1;
    }
    return 0;
}
