#include "packet/PacketBuilder.hpp"
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

int ExpectWindowOpenSendPacket(
    const cg::packet::Packet& packet,
    std::string_view x,
    std::string_view y,
    std::string_view seqno,
    std::string_view objindex,
    std::string_view select,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::send::WindowOpenSend::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 6U, message);
    if (packet.fields.size() != 6U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "x", cg::packet::PacketFieldType::Int62, x, message);
    failures += ExpectField(packet, 1U, "y", cg::packet::PacketFieldType::Int62, y, message);
    failures += ExpectField(packet, 2U, "seqno", cg::packet::PacketFieldType::Int62, seqno, message);
    failures += ExpectField(packet, 3U, "objindex", cg::packet::PacketFieldType::Int62, objindex, message);
    failures += ExpectField(packet, 4U, "select", cg::packet::PacketFieldType::Int62, select, message);
    failures += ExpectField(packet, 5U, "data", cg::packet::PacketFieldType::EscapedString, data, message);
    return failures;
}

int TestToPacketAndEncode()
{
    const cg::packet::send::WindowOpenSend request(1, 2, 3, 4, 5, "open");

    int failures = ExpectWindowOpenSendPacket(request.ToPacket(), "1", "2", "3", "4", "5", "open", "xD ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "xD BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "xD 1 2 3 4 5 open ", "xD BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "xD Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "xD 1 2 3 4 5 open ", "xD Encode value");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::send::WindowOpenSend request(1, 2, 3, 4, 5, "");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "xD Encode empty data");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "xD 1 2 3 4 5  ", "xD empty data value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "xD parse empty data");
    if (parsed.HasValue()) {
        failures += ExpectWindowOpenSendPacket(parsed.Value(), "1", "2", "3", "4", "5", "", "xD parsed empty data");
    }
    return failures;
}

int TestEscapedDataRoundtrip()
{
    const cg::packet::send::WindowOpenSend request(1, 2, 3, 4, 5, "A B\\C\nD|E");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "xD Encode escaped data");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "xD 1 2 3 4 5 A\\SB\\\\C\\nD|E ", "xD escaped value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "xD parse escaped data");
    if (parsed.HasValue()) {
        failures += ExpectWindowOpenSendPacket(
            parsed.Value(),
            "1",
            "2",
            "3",
            "4",
            "5",
            request.data,
            "xD escaped roundtrip");
    }
    return failures;
}

int TestPipeDataRoundtrip()
{
    const cg::packet::send::WindowOpenSend request(1, 2, 3, 4, 5, "A|B|C");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "xD Encode pipe data");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "xD 1 2 3 4 5 A|B|C ", "xD pipe unchanged");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "xD parse pipe data");
    if (parsed.HasValue()) {
        failures += ExpectWindowOpenSendPacket(parsed.Value(), "1", "2", "3", "4", "5", request.data, "xD pipe roundtrip");
    }
    return failures;
}

int TestParsePacketRoundtrip()
{
    const cg::packet::send::WindowOpenSend request(10, 20, 3, -1, 16, "window data");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "xD Encode i62 values");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "xD a k 3 -1 g window\\Sdata ", "xD i62 encoded value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "xD parse i62 values");
    if (parsed.HasValue()) {
        failures += ExpectWindowOpenSendPacket(parsed.Value(), "10", "20", "3", "-1", "16", request.data, "xD parsed i62 values");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("xD 1 2 3 4 5"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "xD",
        true,
        5U,
        "xD missing data");
    failures += ExpectError(
        cg::packet::ParsePacket("xD 1 2 3 4 5 data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "xD",
        true,
        6U,
        "xD extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("xD _ 2 3 4 5 data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "xD",
        true,
        0U,
        "xD bad x");
    failures += ExpectError(
        cg::packet::ParsePacket("xD 1 _ 3 4 5 data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "xD",
        true,
        1U,
        "xD bad y");
    failures += ExpectError(
        cg::packet::ParsePacket("xD 1 2 _ 4 5 data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "xD",
        true,
        2U,
        "xD bad seqno");
    failures += ExpectError(
        cg::packet::ParsePacket("xD 1 2 3 _ 5 data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "xD",
        true,
        3U,
        "xD bad objindex");
    failures += ExpectError(
        cg::packet::ParsePacket("xD 1 2 3 4 _ data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "xD",
        true,
        4U,
        "xD bad select");

    std::string truncatedData = "xD 1 2 3 4 5 ";
    truncatedData.push_back(static_cast<char>(0x80U));
    truncatedData.push_back(' ');
    failures += ExpectError(
        cg::packet::ParsePacket(truncatedData),
        cg::packet::PacketErrorCode::DecodeFailed,
        "xD",
        true,
        5U,
        "xD truncated data");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = cg::packet::send::WindowOpenSend(1, 2, 3, 4, 5, "data").ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "xD",
        false,
        0U,
        "xD direction mismatch");

    cg::packet::Packet wrongName = cg::packet::send::WindowOpenSend(1, 2, 3, 4, 5, "data").ToPacket();
    wrongName.fields[5].name = "payload";
    failures += ExpectError(
        cg::packet::BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "xD",
        true,
        5U,
        "xD field name mismatch");

    cg::packet::Packet wrongType = cg::packet::send::WindowOpenSend(1, 2, 3, 4, 5, "data").ToPacket();
    wrongType.fields[5].type = cg::packet::PacketFieldType::RawString;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "xD",
        true,
        5U,
        "xD field type mismatch");

    cg::packet::Packet badInt = cg::packet::send::WindowOpenSend(1, 2, 3, 4, 5, "data").ToPacket();
    badInt.fields[0].value = "abc";
    failures += ExpectError(
        cg::packet::BuildPacket(badInt),
        cg::packet::PacketErrorCode::EncodeFailed,
        "xD",
        true,
        0U,
        "xD bad i62 value");

    cg::packet::Packet badData = cg::packet::send::WindowOpenSend(1, 2, 3, 4, 5, "data").ToPacket();
    badData.fields[5].value.assign(1U, static_cast<char>(0x80U));
    failures += ExpectError(
        cg::packet::BuildPacket(badData),
        cg::packet::PacketErrorCode::EncodeFailed,
        "xD",
        true,
        5U,
        "xD bad es value");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestEmptyData();
    failures += TestEscapedDataRoundtrip();
    failures += TestPipeDataRoundtrip();
    failures += TestParsePacketRoundtrip();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
