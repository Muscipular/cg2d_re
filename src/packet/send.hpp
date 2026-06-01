#ifndef CG_ENGINE_SEND_H
#define CG_ENGINE_SEND_H
#include "PacketBuilder.hpp"
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

    /**
     * RECV_M 获取地图信息 UUN {mapid:i62} {floor:i62} {x1:i62} {y1:i62} {x2:i62} {y2:i62}
     */
    struct MapDataRequest
    {
        static constexpr std::string_view Code = "UUN";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int mapId = 0;
        int floor = 0;
        int x1 = 0;
        int y1 = 0;
        int x2 = 0;
        int y2 = 0;

        MapDataRequest(int mapId, int floor, int x1, int y1, int x2, int y2)
            : mapId(mapId),
              floor(floor),
              x1(x1),
              y1(y1),
              x2(x2),
              y2(y2)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "mapid",
                PacketFieldType::Int62,
                std::to_string(mapId),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "floor",
                PacketFieldType::Int62,
                std::to_string(floor),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "x1",
                PacketFieldType::Int62,
                std::to_string(x1),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "y1",
                PacketFieldType::Int62,
                std::to_string(y1),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "x2",
                PacketFieldType::Int62,
                std::to_string(x2),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "y2",
                PacketFieldType::Int62,
                std::to_string(y2),
                PacketSourceTag::Xlsx,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket());
        }
    };

    /**
     * RECV_PRS SetProfileData 简介内容 Ot {sellFlag:i62} {sellMsg:es} {buyFlag:i62} {buyMsg:es} {aboutFlg:i62} {aboutMsg:es} {ProfileMsg:es}
     */
    struct SetProfileData
    {
        static constexpr std::string_view Code = "Ot";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int sellFlag = 0;
        std::string sellMessage;
        int buyFlag = 0;
        std::string buyMessage;
        int aboutFlag = 0;
        std::string aboutMessage;
        std::string profileMessage;

        SetProfileData(
            int sellFlag,
            std::string sellMessage,
            int buyFlag,
            std::string buyMessage,
            int aboutFlag,
            std::string aboutMessage,
            std::string profileMessage)
            : sellFlag(sellFlag),
              sellMessage(std::move(sellMessage)),
              buyFlag(buyFlag),
              buyMessage(std::move(buyMessage)),
              aboutFlag(aboutFlag),
              aboutMessage(std::move(aboutMessage)),
              profileMessage(std::move(profileMessage))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "sellFlag",
                PacketFieldType::Int62,
                std::to_string(sellFlag),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "sellMsg",
                PacketFieldType::EscapedString,
                sellMessage,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "buyFlag",
                PacketFieldType::Int62,
                std::to_string(buyFlag),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "buyMsg",
                PacketFieldType::EscapedString,
                buyMessage,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "aboutFlg",
                PacketFieldType::Int62,
                std::to_string(aboutFlag),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "aboutMsg",
                PacketFieldType::EscapedString,
                aboutMessage,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "ProfileMsg",
                PacketFieldType::EscapedString,
                profileMessage,
                PacketSourceTag::Xlsx,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket());
        }
    };

    /**
     * RECV_PS PETSKILL_Use 宠物技能使用 Flvp {havepetindex:i62} {havepetskill:i62} {toindex:i62} {data:es}
     */
    struct PetSkillUseRequest
    {
        static constexpr std::string_view Code = "Flvp";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int havepetindex = 0;
        int havepetskill = 0;
        int toindex = 0;
        std::string data;

        PetSkillUseRequest(int havepetindex, int havepetskill, int toindex, std::string data)
            : havepetindex(havepetindex),
              havepetskill(havepetskill),
              toindex(toindex),
              data(std::move(data))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "havepetindex",
                PacketFieldType::Int62,
                std::to_string(havepetindex),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "havepetskill",
                PacketFieldType::Int62,
                std::to_string(havepetskill),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "toindex",
                PacketFieldType::Int62,
                std::to_string(toindex),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                data,
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket());
        }
    };
}

#endif //CG_ENGINE_SEND_H
