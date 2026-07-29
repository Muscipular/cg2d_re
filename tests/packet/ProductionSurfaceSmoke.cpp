#ifndef CG_PACKET_ENABLE_DIAGNOSTIC_TEXT
#error "CG_PACKET_ENABLE_DIAGNOSTIC_TEXT must be defined"
#elif CG_PACKET_ENABLE_DIAGNOSTIC_TEXT != 0
#error "CG_PACKET_ENABLE_DIAGNOSTIC_TEXT must be 0"
#endif

#ifndef CG_PACKET_ENABLE_TEST_HELPERS
#error "CG_PACKET_ENABLE_TEST_HELPERS must be defined"
#elif CG_PACKET_ENABLE_TEST_HELPERS != 0
#error "CG_PACKET_ENABLE_TEST_HELPERS must be 0"
#endif

#if defined(_CPPRTTI) || defined(__GXX_RTTI) || defined(__cpp_rtti)
#error "RTTI must be disabled"
#endif

#include "packet/Packet.hpp"

#include <iostream>

int main()
{
    const cg::packet::PacketResult<std::string> encoded =
        cg::packet::send::CharacterListRequest {}.Encode();
    if (!encoded.HasValue()) {
        std::cerr << "CharacterListRequest::Encode failed\n";
        return 1;
    }
    if (encoded.Value() != "Gp ") {
        std::cerr << "CharacterListRequest::Encode returned unexpected bytes\n";
        return 2;
    }

    const cg::packet::PacketResult<cg::packet::receive::ProcessStatus> parsed =
        cg::packet::receive::ProcessStatus::Parse("cJzI ready\\Sok ");
    if (!parsed.HasValue()) {
        std::cerr << "ProcessStatus::Parse failed\n";
        return 3;
    }
    if (parsed.Value().data != "ready ok") {
        std::cerr << "ProcessStatus::Parse returned unexpected data\n";
        return 4;
    }

    const cg::packet::PacketResult<cg::packet::receive::ProcessStatus> invalid =
        cg::packet::receive::ProcessStatus::Parse("bad ready ");
    if (!invalid.HasError()) {
        std::cerr << "ProcessStatus::Parse accepted an invalid code\n";
        return 5;
    }
    if (invalid.Error().code != cg::packet::PacketErrorCode::InvalidCode) {
        std::cerr << "ProcessStatus::Parse returned an unexpected error code\n";
        return 6;
    }
    if (!invalid.Error().message.empty()) {
        std::cerr << "diagnostic text must be disabled\n";
        return 7;
    }

    return 0;
}
