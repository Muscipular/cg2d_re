#include "packet/Send.hpp"

#include <iostream>
#include <limits>
#include <string_view>
#include <vector>

namespace {

using cg::packet::send::AlbumInfoRequest;

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
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
    failures += Expect(
        packet.fields[index].type == cg::packet::PacketFieldType::Int62,
        message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectParseError(
    std::string_view plain,
    cg::packet::PacketErrorCode code,
    std::size_t fieldIndex,
    const char* message)
{
    const cg::packet::PacketResult<cg::packet::Packet> result =
        cg::packet::ParsePacket(plain, AlbumInfoRequest::Spec);
    int failures = Expect(!result, message);
    failures += Expect(result.HasError(), message);
    if (result.HasError()) {
        failures += Expect(result.Error().code == code, message);
        failures += Expect(result.Error().packetCode.has_value(), message);
        if (result.Error().packetCode.has_value()) {
            failures += Expect(result.Error().packetCode->View() == "kza", message);
        }
        failures += Expect(result.Error().fieldIndex == fieldIndex, message);
    }
    return failures;
}

int TestSpec()
{
    int failures = 0;
    failures += Expect(AlbumInfoRequest::Code == "kza", "kza code");
    failures += Expect(
        AlbumInfoRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "kza direction");
    failures += Expect(AlbumInfoRequest::Fields.size() == 2U, "kza template fields");
    failures += Expect(AlbumInfoRequest::Fields[0].name == "count", "kza count name");
    failures += Expect(
        AlbumInfoRequest::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "kza count type");
    failures += Expect(AlbumInfoRequest::Fields[1].name == "albumid", "kza albumid name");
    failures += Expect(
        AlbumInfoRequest::Fields[1].type == cg::packet::PacketFieldType::Int62,
        "kza albumid type");
    failures += Expect(AlbumInfoRequest::Spec.repeatedTail.enabled, "kza repeated tail enabled");
    failures += Expect(
        AlbumInfoRequest::Spec.repeatedTail.countFieldIndex == 0U,
        "kza repeated tail count index");
    failures += Expect(
        cg::packet::detail::IsRepeatedTailSpecValid(AlbumInfoRequest::Spec),
        "kza repeated tail Spec valid");
    return failures;
}

int TestZeroAndMultiple()
{
    int failures = 0;

    const AlbumInfoRequest zero(std::vector<int> {});
    const cg::packet::Packet zeroPacket = zero.ToPacket();
    failures += Expect(zeroPacket.fields.size() == 1U, "kza zero packet field count");
    failures += ExpectField(zeroPacket, 0U, "count", "0", "kza zero count field");
    const cg::packet::PacketResult<std::string> zeroEncoded = zero.Encode();
    failures += Expect(static_cast<bool>(zeroEncoded), "kza zero encode");
    if (zeroEncoded.HasValue()) {
        failures += Expect(zeroEncoded.Value() == "kza 0 ", "kza zero canonical");
    }

    const AlbumInfoRequest single(std::vector<int> {10});
    const cg::packet::PacketResult<std::string> singleEncoded = single.Encode();
    failures += Expect(static_cast<bool>(singleEncoded), "kza single encode");
    if (singleEncoded.HasValue()) {
        failures += Expect(singleEncoded.Value() == "kza 1 a ", "kza single canonical");
    }

    const AlbumInfoRequest multiple(std::vector<int> {10, 20});
    const cg::packet::Packet packet = multiple.ToPacket();
    failures += Expect(packet.fields.size() == 3U, "kza multiple packet field count");
    failures += ExpectField(packet, 0U, "count", "2", "kza multiple count");
    failures += ExpectField(packet, 1U, "albumid", "10", "kza first albumid");
    failures += ExpectField(packet, 2U, "albumid", "20", "kza second albumid");

    const cg::packet::PacketResult<std::string> encoded = multiple.Encode();
    failures += Expect(static_cast<bool>(encoded), "kza multiple encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "kza 2 a k ", "kza multiple canonical");
        const cg::packet::PacketResult<cg::packet::Packet> parsed =
            cg::packet::ParsePacket(encoded.Value(), AlbumInfoRequest::Spec);
        failures += Expect(static_cast<bool>(parsed), "kza multiple roundtrip");
        if (parsed.HasValue()) {
            failures += Expect(parsed.Value().fields.size() == 3U, "kza roundtrip field count");
            failures += ExpectField(parsed.Value(), 0U, "count", "2", "kza roundtrip count");
            failures += ExpectField(parsed.Value(), 1U, "albumid", "10", "kza roundtrip first");
            failures += ExpectField(parsed.Value(), 2U, "albumid", "20", "kza roundtrip second");
        }
    }
    return failures;
}

int TestSignedAlbumIds()
{
    const AlbumInfoRequest request(std::vector<int> {
        -1,
        -62,
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max(),
    });
    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    int failures = Expect(static_cast<bool>(encoded), "kza signed albumids encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "kza 4 -1 -10 -2lkCB2 2lkCB1 ",
            "kza signed albumids canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    const cg::packet::PacketResult<cg::packet::Packet> wrongCode =
        cg::packet::ParsePacket("bad 1 a ", AlbumInfoRequest::Spec);
    failures += Expect(wrongCode.HasError(), "kza wrong code");
    if (wrongCode.HasError()) {
        failures += Expect(
            wrongCode.Error().code == cg::packet::PacketErrorCode::InvalidCode,
            "kza wrong code error");
        failures += Expect(wrongCode.Error().packetCode.has_value(), "kza wrong code context");
        if (wrongCode.Error().packetCode.has_value()) {
            failures += Expect(
                wrongCode.Error().packetCode->View() == "bad",
                "kza wrong code packet");
        }
        failures += Expect(!wrongCode.Error().fieldIndex.has_value(), "kza wrong code no field");
    }

    failures += ExpectParseError(
        "kza 2 a",
        cg::packet::PacketErrorCode::FieldCountMismatch,
        2U,
        "kza missing albumid");
    failures += ExpectParseError(
        "kza 2 a ",
        cg::packet::PacketErrorCode::FieldCountMismatch,
        2U,
        "kza missing albumid with delimiter");
    failures += ExpectParseError(
        "kza 1 a k",
        cg::packet::PacketErrorCode::FieldCountMismatch,
        2U,
        "kza extra albumid");
    failures += ExpectParseError(
        "kza -1",
        cg::packet::PacketErrorCode::FieldCountMismatch,
        0U,
        "kza negative count");
    failures += ExpectParseError(
        "kza _",
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "kza invalid count");
    failures += ExpectParseError(
        "kza 1 _",
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "kza invalid albumid");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSpec();
    failures += TestZeroAndMultiple();
    failures += TestSignedAlbumIds();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    return 0;
}
