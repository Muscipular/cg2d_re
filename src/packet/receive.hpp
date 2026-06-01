#ifndef CG_ENGINE_RECEIVE_H
#define CG_ENGINE_RECEIVE_H
#include "PacketTypes.hpp"

#include <string>
#include <string_view>
#include <utility>

namespace cg::packet::receive
{
    /**
     * SEND_ClientLogin 登录角色表结果		whV {result:i62} {data:es}
     */
    struct ClientLoginResult
    {
        static constexpr std::string_view Code = "whV";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        enum
        {
            ClientLogin_AUTH_OK, //data:  (null)
            ClientLogin_AUTH_ERROR_CDKEY, //data: ("fail"|"block")
            ClientLogin_AUTH_ERROR_BADNAME, //data: ("fail"|"block")
            ClientLogin_AUTH_ERROR_PASSWORD, //data: ("fail"|"block")
            ClientLogin_AUTH_ERROR_EXPIRE,
            ClientLogin_AUTH_ERROR_LOCKOUT,
            ClientLogin_AUTH_ERROR_ISAO_DB,
            ClientLogin_AUTH_ERROR_CHARNAME,
            ClientLogin_AUTH_ERROR_END,
            ClientLogin_AUTH_ERROR_OTHER, //  data: ("fail"|"block"|"busy")
        };

        int result;
        std::string data;

        ClientLoginResult(int result, const std::string& data)
            : result(result), data(data)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ExistingPrototype);
            packet.fields.push_back(PacketField {
                "result",
                PacketFieldType::Int62,
                std::to_string(result),
                PacketSourceTag::ExistingPrototype,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                data,
                PacketSourceTag::ExistingPrototype,
            });
            return packet;
        }

        static PacketResult<ClientLoginResult> Parse(std::string_view view)
        {
            PacketError error = PacketError::Make(PacketErrorCode::NotImplemented, "ClientLoginResult Parse 待 M6.7/M6.15 实现");
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ExistingPrototype;
            if (view.empty()) {
                error.code = PacketErrorCode::EmptyInput;
                error.message = "ClientLoginResult Parse 收到空输入";
            }
            return PacketResult<ClientLoginResult>::Failure(std::move(error));
        }
    };
}

#endif //CG_ENGINE_RECEIVE_H
