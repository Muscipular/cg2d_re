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

    /**
     * RECV_PLVUP 宠物加点 kjSK {petSlot:i62} {sqeno:i62}
     */
    struct PetLevelUpPoint
    {
        static constexpr std::string_view Code = "kjSK";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int petSlot = 0;
        int sqeno = 0;

        PetLevelUpPoint(int petSlot, int sqeno)
            : petSlot(petSlot),
              sqeno(sqeno)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "petSlot",
                PacketFieldType::Int62,
                std::to_string(petSlot),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "sqeno",
                PacketFieldType::Int62,
                std::to_string(sqeno),
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
     * RECV_PSSW SKILL_swapPetSkill 宠物技能移动 HJQp {petSlot:i62} {from:i62} {to:i62}
     */
    struct PetSkillSwap
    {
        static constexpr std::string_view Code = "HJQp";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int petSlot = 0;
        int from = 0;
        int to = 0;

        PetSkillSwap(int petSlot, int from, int to)
            : petSlot(petSlot),
              from(from),
              to(to)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "petSlot",
                PacketFieldType::Int62,
                std::to_string(petSlot),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "from",
                PacketFieldType::Int62,
                std::to_string(from),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "to",
                PacketFieldType::Int62,
                std::to_string(to),
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
     * RECV_KN CHAR_inputUserPetName 宠物姓名 ycX {petSlot:i62} {name:es}
     */
    struct UserPetNameInput
    {
        static constexpr std::string_view Code = "ycX";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int petSlot = 0;
        std::string name;

        UserPetNameInput(int petSlot, std::string name)
            : petSlot(petSlot),
              name(std::move(name))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "petSlot",
                PacketFieldType::Int62,
                std::to_string(petSlot),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "name",
                PacketFieldType::EscapedString,
                name,
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
     * RECV_MSG 发送邮件信息 CV {index:i62} {message:es} {color:i62}
     */
    struct MailMessageSend
    {
        static constexpr std::string_view Code = "CV";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int index = 0;
        std::string message;
        int color = 0;

        MailMessageSend(int index, std::string message, int color)
            : index(index),
              message(std::move(message)),
              color(color)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "index",
                PacketFieldType::Int62,
                std::to_string(index),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "message",
                PacketFieldType::EscapedString,
                message,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "color",
                PacketFieldType::Int62,
                std::to_string(color),
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
     * RECV_TK 发送聊天 uSr {x:i62} {y:i62} {msg:es} {color:i62} {area:i62} {fontSize:i62}
     */
    struct ChatMessageSend
    {
        static constexpr std::string_view Code = "uSr";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int x = 0;
        int y = 0;
        std::string msg;
        int color = 0;
        int area = 0;
        int fontSize = 0;

        ChatMessageSend(int x, int y, std::string msg, int color, int area, int fontSize)
            : x(x),
              y(y),
              msg(std::move(msg)),
              color(color),
              area(area),
              fontSize(fontSize)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "x",
                PacketFieldType::Int62,
                std::to_string(x),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "y",
                PacketFieldType::Int62,
                std::to_string(y),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "msg",
                PacketFieldType::EscapedString,
                msg,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "color",
                PacketFieldType::Int62,
                std::to_string(color),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "area",
                PacketFieldType::Int62,
                std::to_string(area),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "fontSize",
                PacketFieldType::Int62,
                std::to_string(fontSize),
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
     * RECV_PMSG 发送宠邮 rMsv {index:i62} {petindex:i62} {itemindex:i62} {message:es} {color:i62}
     */
    struct PetMailSend
    {
        static constexpr std::string_view Code = "rMsv";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int index = 0;
        int petindex = 0;
        int itemindex = 0;
        std::string message;
        int color = 0;

        PetMailSend(int index, int petindex, int itemindex, std::string message, int color)
            : index(index),
              petindex(petindex),
              itemindex(itemindex),
              message(std::move(message)),
              color(color)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "index",
                PacketFieldType::Int62,
                std::to_string(index),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "petindex",
                PacketFieldType::Int62,
                std::to_string(petindex),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "itemindex",
                PacketFieldType::Int62,
                std::to_string(itemindex),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "message",
                PacketFieldType::EscapedString,
                message,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "color",
                PacketFieldType::Int62,
                std::to_string(color),
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
     * RECV_PGML 宠邮登记 PfqV {toregist:i62} {petSlot:i62} {itemSlot:i62} {text:es} {color:i62}
     */
    struct PetMailRegister
    {
        static constexpr std::string_view Code = "PfqV";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int toregist = 0;
        int petSlot = 0;
        int itemSlot = 0;
        std::string text;
        int color = 0;

        PetMailRegister(int toregist, int petSlot, int itemSlot, std::string text, int color)
            : toregist(toregist),
              petSlot(petSlot),
              itemSlot(itemSlot),
              text(std::move(text)),
              color(color)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "toregist",
                PacketFieldType::Int62,
                std::to_string(toregist),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "petSlot",
                PacketFieldType::Int62,
                std::to_string(petSlot),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "itemSlot",
                PacketFieldType::Int62,
                std::to_string(itemSlot),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "text",
                PacketFieldType::EscapedString,
                text,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "color",
                PacketFieldType::Int62,
                std::to_string(color),
                PacketSourceTag::Xlsx,
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
