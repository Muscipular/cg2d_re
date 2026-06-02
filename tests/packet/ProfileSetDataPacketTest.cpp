#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::send::SetProfileData::Spec;
}

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, PacketSpec());
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, PacketSpec());
}

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

int ExpectField(
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

int ExpectSetProfileDataPacket(const cg::packet::Packet& packet, const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::SetProfileData::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 7U, message);
    if (packet.fields.size() != 7U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "sellFlag", cg::packet::PacketFieldType::Int62, "1", message);
    failures += ExpectField(packet, 1U, "sellMsg", cg::packet::PacketFieldType::EscapedString, "Sell", message);
    failures += ExpectField(packet, 2U, "buyFlag", cg::packet::PacketFieldType::Int62, "2", message);
    failures += ExpectField(packet, 3U, "buyMsg", cg::packet::PacketFieldType::EscapedString, "Buy", message);
    failures += ExpectField(packet, 4U, "aboutFlg", cg::packet::PacketFieldType::Int62, "3", message);
    failures += ExpectField(packet, 5U, "aboutMsg", cg::packet::PacketFieldType::EscapedString, "About", message);
    failures += ExpectField(packet, 6U, "ProfileMsg", cg::packet::PacketFieldType::EscapedString, "Profile", message);
    return failures;
}

int TestToPacketAndEncode()
{
    const cg::packet::send::SetProfileData data(1, "Sell", 2, "Buy", 3, "About", "Profile");

    int failures = ExpectSetProfileDataPacket(data.ToPacket(), "Ot ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(data.ToPacket());
    failures += Expect(static_cast<bool>(built), "Ot BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "Ot 1 Sell 2 Buy 3 About Profile ", "Ot BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = data.Encode();
    failures += Expect(static_cast<bool>(encoded), "Ot Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "Ot 1 Sell 2 Buy 3 About Profile ", "Ot Encode value");
    }
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("Ot 1 Sell 2 Buy 3 About Profile ");

    int failures = Expect(static_cast<bool>(parsed), "parse Ot");
    if (parsed.HasValue()) {
        failures += ExpectSetProfileDataPacket(parsed.Value(), "parse Ot packet");
    }
    return failures;
}

int TestInt62Values()
{
    const cg::packet::send::SetProfileData data(10, "Sell", -1, "Buy", 3, "About", "Profile");
    const cg::packet::PacketResult<std::string> encoded = data.Encode();

    int failures = Expect(static_cast<bool>(encoded), "Ot Encode i62 values");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "Ot a Sell -1 Buy 3 About Profile ", "Ot i62 encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "Ot parse i62 values");
    if (parsed.HasValue()) {
        failures += ExpectField(
            parsed.Value(),
            0U,
            "sellFlag",
            cg::packet::PacketFieldType::Int62,
            "10",
            "Ot parsed sellFlag 10");
        failures += ExpectField(
            parsed.Value(),
            2U,
            "buyFlag",
            cg::packet::PacketFieldType::Int62,
            "-1",
            "Ot parsed buyFlag -1");
    }
    return failures;
}

int TestEscapedFieldsRoundtrip()
{
    const cg::packet::send::SetProfileData data(
        1,
        "Sell message",
        2,
        "Buy\nmessage",
        3,
        "About\\message",
        "Profile line\nnext\\tail");

    const cg::packet::PacketResult<std::string> encoded = data.Encode();
    int failures = Expect(static_cast<bool>(encoded), "Ot Encode escaped fields");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value().find('\n') == std::string::npos, "Ot escaped no newline");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "Ot parse escaped fields");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectField(
        parsed.Value(),
        1U,
        "sellMsg",
        cg::packet::PacketFieldType::EscapedString,
        data.sellMessage,
        "Ot roundtrip sellMsg");
    failures += ExpectField(
        parsed.Value(),
        3U,
        "buyMsg",
        cg::packet::PacketFieldType::EscapedString,
        data.buyMessage,
        "Ot roundtrip buyMsg");
    failures += ExpectField(
        parsed.Value(),
        5U,
        "aboutMsg",
        cg::packet::PacketFieldType::EscapedString,
        data.aboutMessage,
        "Ot roundtrip aboutMsg");
    failures += ExpectField(
        parsed.Value(),
        6U,
        "ProfileMsg",
        cg::packet::PacketFieldType::EscapedString,
        data.profileMessage,
        "Ot roundtrip ProfileMsg");
    return failures;
}

int TestEmptyMessages()
{
    const cg::packet::send::SetProfileData data(1, "", 2, "", 3, "", "");
    const cg::packet::PacketResult<std::string> encoded = data.Encode();

    int failures = Expect(static_cast<bool>(encoded), "Ot Encode empty messages");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "Ot 1  2  3   ", "Ot empty messages encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "Ot parse empty messages");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectField(parsed.Value(), 1U, "sellMsg", cg::packet::PacketFieldType::EscapedString, "", "Ot empty sellMsg");
    failures += ExpectField(parsed.Value(), 3U, "buyMsg", cg::packet::PacketFieldType::EscapedString, "", "Ot empty buyMsg");
    failures += ExpectField(parsed.Value(), 5U, "aboutMsg", cg::packet::PacketFieldType::EscapedString, "", "Ot empty aboutMsg");
    failures += ExpectField(parsed.Value(), 6U, "ProfileMsg", cg::packet::PacketFieldType::EscapedString, "", "Ot empty ProfileMsg");
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("Ot 1 Sell 2 Buy 3 About"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Ot",
        true,
        6U,
        "Ot missing field");
    failures += ExpectError(
        ParsePacket("Ot 1 Sell 2 Buy 3 About Profile extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Ot",
        true,
        7U,
        "Ot extra field");
    failures += ExpectError(
        ParsePacket("Ot _ Sell 2 Buy 3 About Profile "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ot",
        true,
        0U,
        "Ot bad sellFlag");
    failures += ExpectError(
        ParsePacket("Ot 1 Sell _ Buy 3 About Profile "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ot",
        true,
        2U,
        "Ot bad buyFlag");
    failures += ExpectError(
        ParsePacket("Ot 1 Sell 2 Buy _ About Profile "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ot",
        true,
        4U,
        "Ot bad aboutFlg");

    cg::packet::Packet wrongName = cg::packet::send::SetProfileData(1, "Sell", 2, "Buy", 3, "About", "Profile").ToPacket();
    wrongName.fields[1].name = "wrong";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Ot",
        true,
        1U,
        "Ot field name mismatch");

    cg::packet::Packet wrongType = cg::packet::send::SetProfileData(1, "Sell", 2, "Buy", 3, "About", "Profile").ToPacket();
    wrongType.fields[4].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Ot",
        true,
        4U,
        "Ot field type mismatch");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestParsePacket();
    failures += TestInt62Values();
    failures += TestEscapedFieldsRoundtrip();
    failures += TestEmptyMessages();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
