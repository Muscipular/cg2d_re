#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using cg::packet::send::ShutdownRequest;

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

int ExpectShutdownRequestPacket(
    const cg::packet::Packet& packet,
    std::string_view passwd,
    std::string_view min,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(ShutdownRequest::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "passwd", cg::packet::PacketFieldType::EscapedString, passwd, message);
    failures += ExpectField(packet, 1U, "min", cg::packet::PacketFieldType::Int62, min, message);
    return failures;
}

int TestToPacketAndEncode()
{
    const ShutdownRequest request("hogehoge", 10);

    int failures = ExpectShutdownRequestPacket(request.ToPacket(), "hogehoge", "10", "IcE ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "IcE BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "IcE hogehoge a ", "IcE BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "IcE Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "IcE hogehoge a ", "IcE Encode value");
    }
    return failures;
}

int TestPasswdRoundtripValues()
{
    const std::vector<std::pair<std::string, std::string>> cases = {
        {"", "IcE  0 "},
        {"A B", "IcE A\\SB 0 "},
        {"A\\B", "IcE A\\\\B 0 "},
        {"A\nB", "IcE A\\nB 0 "},
        {"A|B", "IcE A|B 0 "},
    };

    int failures = 0;
    for (const auto& entry : cases) {
        const ShutdownRequest request(entry.first, 0);
        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(static_cast<bool>(encoded), "IcE passwd Encode");
        if (!encoded.HasValue()) {
            continue;
        }

        failures += Expect(encoded.Value() == entry.second, "IcE passwd encoded value");

        const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(encoded.Value());
        failures += Expect(static_cast<bool>(parsed), "IcE passwd ParsePacket");
        if (parsed.HasValue()) {
            failures += ExpectShutdownRequestPacket(parsed.Value(), entry.first, "0", "IcE passwd roundtrip packet");
        }
    }
    return failures;
}

int TestMinRoundtripValues()
{
    const std::vector<std::pair<int, std::string>> cases = {
        {0, "IcE pass 0 "},
        {-1, "IcE pass -1 "},
        {10, "IcE pass a "},
    };

    int failures = 0;
    for (const auto& entry : cases) {
        const ShutdownRequest request("pass", entry.first);
        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(static_cast<bool>(encoded), "IcE min Encode");
        if (!encoded.HasValue()) {
            continue;
        }

        failures += Expect(encoded.Value() == entry.second, "IcE min encoded value");

        const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(encoded.Value());
        failures += Expect(static_cast<bool>(parsed), "IcE min ParsePacket");
        if (parsed.HasValue()) {
            failures += ExpectShutdownRequestPacket(
                parsed.Value(),
                "pass",
                std::to_string(entry.first),
                "IcE min roundtrip packet");
        }
    }
    return failures;
}

int TestParsePacketRoundtrip()
{
    const ShutdownRequest request("shutdown pass", 10);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "IcE roundtrip Encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "IcE roundtrip ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectShutdownRequestPacket(parsed.Value(), request.passwd, "10", "IcE roundtrip packet");
    failures += Expect(parsed.Value().direction == cg::packet::PacketDirection::ClientToServer, "IcE direction");

    const cg::packet::PacketResult<std::string> rebuilt = cg::packet::BuildPacket(parsed.Value());
    failures += Expect(static_cast<bool>(rebuilt), "IcE roundtrip BuildPacket");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == encoded.Value(), "IcE roundtrip rebuilt");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("IcE pass"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IcE",
        true,
        1U,
        "IcE missing min");
    failures += ExpectError(
        cg::packet::ParsePacket("IcE pass a extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IcE",
        true,
        2U,
        "IcE extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("IcE pass _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IcE",
        true,
        1U,
        "IcE invalid i62");

    std::string invalidPasswd("IcE ");
    invalidPasswd.push_back(static_cast<char>(0x80));
    invalidPasswd.append(" 0 ");
    failures += ExpectError(
        cg::packet::ParsePacket(invalidPasswd),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IcE",
        true,
        0U,
        "IcE invalid es");
    return failures;
}

int TestManualPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = ShutdownRequest("pass", 10).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IcE",
        false,
        0U,
        "IcE wrong direction");

    cg::packet::Packet wrongCode = ShutdownRequest("pass", 10).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(wrongCode),
        cg::packet::PacketErrorCode::UnknownCode,
        "bad",
        false,
        0U,
        "IcE wrong code");

    cg::packet::Packet wrongName = ShutdownRequest("pass", 10).ToPacket();
    wrongName.fields[0].name = "password";
    failures += ExpectError(
        cg::packet::BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IcE",
        true,
        0U,
        "IcE wrong field name");

    cg::packet::Packet wrongType = ShutdownRequest("pass", 10).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::RawString;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IcE",
        true,
        0U,
        "IcE wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestPasswdRoundtripValues();
    failures += TestMinRoundtripValues();
    failures += TestParsePacketRoundtrip();
    failures += TestParseErrors();
    failures += TestManualPacketErrors();

    if (failures != 0) {
        std::cerr << failures << " shutdown request test(s) failed\n";
        return 1;
    }

    return 0;
}
