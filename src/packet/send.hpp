#ifndef CG_ENGINE_SEND_H
#define CG_ENGINE_SEND_H
#include "PacketTypes.hpp"

#include <string>
#include <string_view>
#include <utility>

namespace cg::packet::send
{
    /**
     * RECV_ClientLogin 账号密码 JFVf {clientHash:es} {password:es} {account:es}
     */
    struct ClientLogin
    {
        static constexpr std::string_view Code = "JFVf";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        std::string clientHash;
        std::string password;
        std::string account;

        ClientLogin(const std::string& client_hash, const std::string& account, const std::string& password)
            : clientHash(client_hash),
              password(password),
              account(account)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ExistingPrototype);
            packet.fields.push_back(PacketField {
                "clientHash",
                PacketFieldType::EscapedString,
                clientHash,
                PacketSourceTag::ExistingPrototype,
            });
            packet.fields.push_back(PacketField {
                "password",
                PacketFieldType::EscapedString,
                password,
                PacketSourceTag::ExistingPrototype,
            });
            packet.fields.push_back(PacketField {
                "account",
                PacketFieldType::EscapedString,
                account,
                PacketSourceTag::ExistingPrototype,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            PacketError error = PacketError::Make(PacketErrorCode::NotImplemented, "ClientLogin Encode 待 M6.7/M6.8 实现");
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ExistingPrototype;
            return PacketResult<std::string>::Failure(std::move(error));
        }
    };
}

#endif //CG_ENGINE_SEND_H
