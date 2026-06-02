#ifndef CG_ENGINE_SEND_H
#define CG_ENGINE_SEND_H
#include "PacketBuilder.hpp"
#include "PacketTypes.hpp"
#include "PayloadCodec.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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
     * RECV_CreateNewChar 创建角色 aSEl
     * {dataPlace:i62} {name:es} {image:i62} {face:i62} {vital:i62} {strength:i62}
     * {tough:i62} {quick:i62} {magic:i62} {earth:i62} {water:i62} {fire:i62} {wind:i62}
     */
    struct CreateNewChar
    {
        static constexpr std::string_view Code = "aSEl";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int dataPlace = 0;
        std::string name;
        int image = 0;
        int face = 0;
        int vital = 0;
        int strength = 0;
        int tough = 0;
        int quick = 0;
        int magic = 0;
        int earth = 0;
        int water = 0;
        int fire = 0;
        int wind = 0;

        CreateNewChar(
            int dataPlace,
            std::string name,
            int image,
            int face,
            int vital,
            int strength,
            int tough,
            int quick,
            int magic,
            int earth,
            int water,
            int fire,
            int wind)
            : dataPlace(dataPlace),
              name(std::move(name)),
              image(image),
              face(face),
              vital(vital),
              strength(strength),
              tough(tough),
              quick(quick),
              magic(magic),
              earth(earth),
              water(water),
              fire(fire),
              wind(wind)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "dataPlace",
                PacketFieldType::Int62,
                std::to_string(dataPlace),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "name",
                PacketFieldType::EscapedString,
                name,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "image",
                PacketFieldType::Int62,
                std::to_string(image),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "face",
                PacketFieldType::Int62,
                std::to_string(face),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "vital",
                PacketFieldType::Int62,
                std::to_string(vital),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "strength",
                PacketFieldType::Int62,
                std::to_string(strength),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "tough",
                PacketFieldType::Int62,
                std::to_string(tough),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "quick",
                PacketFieldType::Int62,
                std::to_string(quick),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "magic",
                PacketFieldType::Int62,
                std::to_string(magic),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "earth",
                PacketFieldType::Int62,
                std::to_string(earth),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "water",
                PacketFieldType::Int62,
                std::to_string(water),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "fire",
                PacketFieldType::Int62,
                std::to_string(fire),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "wind",
                PacketFieldType::Int62,
                std::to_string(wind),
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
     * RECV_MU MAGIC_Use 使用魔法 RXr {x:i62} {y:i62} {array:i62} {toindex:i62}
     */
    struct MagicUseRequest
    {
        static constexpr std::string_view Code = "RXr";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int x = 0;
        int y = 0;
        int array = 0;
        int toindex = 0;

        MagicUseRequest(int x, int y, int array, int toindex)
            : x(x),
              y(y),
              array(array),
              toindex(toindex)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "x",
                PacketFieldType::Int62,
                std::to_string(x),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "y",
                PacketFieldType::Int62,
                std::to_string(y),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "array",
                PacketFieldType::Int62,
                std::to_string(array),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "toindex",
                PacketFieldType::Int62,
                std::to_string(toindex),
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
     * RECV_WN 打开系统窗口 xD {x:i62} {y:i62} {seqno:i62} {objindex:i62} {select:i62} {data:es}
     */
    struct WindowOpenSend
    {
        static constexpr std::string_view Code = "xD";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int x = 0;
        int y = 0;
        int seqno = 0;
        int objindex = 0;
        int select = 0;
        std::string data;

        WindowOpenSend(int x, int y, int seqno, int objindex, int select, std::string data)
            : x(x),
              y(y),
              seqno(seqno),
              objindex(objindex),
              select(select),
              data(std::move(data))
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
                "seqno",
                PacketFieldType::Int62,
                std::to_string(seqno),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "objindex",
                PacketFieldType::Int62,
                std::to_string(objindex),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "select",
                PacketFieldType::Int62,
                std::to_string(select),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                data,
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

    /**
     * RECV_DG DropGold 丢金币 woz {x:i62} {y:i62} {amount:i62}
     */
    struct DropGold
    {
        static constexpr std::string_view Code = "woz";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int x = 0;
        int y = 0;
        int amount = 0;

        DropGold(int x, int y, int amount)
            : x(x),
              y(y),
              amount(amount)
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
                "amount",
                PacketFieldType::Int62,
                std::to_string(amount),
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
     * RECV_GI 变更家族信息 Ufm {c:i62} {data:es}
     */
    struct GuildInfoChange
    {
        static constexpr std::string_view Code = "Ufm";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int c = 0;
        std::string data;

        GuildInfoChange(int c, std::string data)
            : c(c),
              data(std::move(data))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "c",
                PacketFieldType::Int62,
                std::to_string(c),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                data,
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
     * RECV_GT 设置家族职位 GY {index:i62} {bitflag:i62} {data:es}
     */
    struct GuildTitleSet
    {
        static constexpr std::string_view Code = "GY";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int index = 0;
        int bitflag = 0;
        std::string data;

        GuildTitleSet(int index, int bitflag, std::string data)
            : index(index),
              bitflag(bitflag),
              data(std::move(data))
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
                "bitflag",
                PacketFieldType::Int62,
                std::to_string(bitflag),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                data,
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
     * RECV_GML 家族邮件 uId {toregistNumber:i62} {text:es} {color:i62}
     */
    struct GuildMailSend
    {
        static constexpr std::string_view Code = "uId";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int toregistNumber = 0;
        std::string text;
        int color = 0;

        GuildMailSend(int toregistNumber, std::string text, int color)
            : toregistNumber(toregistNumber),
              text(std::move(text)),
              color(color)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "toregistNumber",
                PacketFieldType::Int62,
                std::to_string(toregistNumber),
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

    /**
     * RECV_PRE 删除迷你邮件好友 wD {profileIndex:i62} {name:es}
     */
    struct ProfileMailFriendDelete
    {
        static constexpr std::string_view Code = "wD";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        int profileIndex = 0;
        std::string name;

        ProfileMailFriendDelete(int profileIndex, std::string name)
            : profileIndex(profileIndex),
              name(std::move(name))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "profileIndex",
                PacketFieldType::Int62,
                std::to_string(profileIndex),
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
     * RECV_PRM 发送迷你邮件 GH {data:es} {color:i62}
     * data: profileIndex|name|msg|；profileIndex 为 raw signed decimal，不使用 i62。
     */
    struct ProfileMiniMailSend
    {
        static constexpr std::string_view Code = "GH";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::string_view PayloadDelimiter = "|";

    private:
        struct ProfileMiniMailPayload
        {
            int profileIndex = 0;
            std::string name;
            std::string message;
        };

    public:
        int profileIndex = 0;
        std::string name;
        std::string message;
        int color = 0;

        ProfileMiniMailSend(int profileIndex, std::string name, std::string message, int color)
            : profileIndex(profileIndex),
              name(std::move(name)),
              message(std::move(message)),
              color(color)
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            PacketResult<std::string> nameValue = ValidateBuildStringField(name, NamePayloadIndex);
            if (!nameValue.HasValue()) {
                return PacketResult<std::string>::Failure(nameValue.Error());
            }

            PacketResult<std::string> messageValue = ValidateBuildStringField(message, MessagePayloadIndex);
            if (!messageValue.HasValue()) {
                return PacketResult<std::string>::Failure(messageValue.Error());
            }

            std::string payload;
            payload.append(std::to_string(profileIndex));
            payload.push_back('|');
            payload.append(name);
            payload.push_back('|');
            payload.append(message);
            payload.push_back('|');
            return PacketResult<std::string>::Success(std::move(payload));
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            PacketResult<std::string> data = BuildData();
            if (!data.HasValue()) {
                return PacketResult<Packet>::Failure(data.Error());
            }

            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                data.Value(),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "color",
                PacketFieldType::Int62,
                std::to_string(color),
                PacketSourceTag::Xlsx,
            });
            return PacketResult<Packet>::Success(std::move(packet));
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            PacketResult<Packet> packet = ToPacket();
            if (!packet.HasValue()) {
                return PacketResult<std::string>::Failure(packet.Error());
            }
            return BuildPacket(packet.Value());
        }

        static PacketResult<ProfileMiniMailSend> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ProfileMiniMailSend>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<ProfileMiniMailSend> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ProfileMiniMailSend Parse 收到非 GH 封包");
                error.packetCode = packet.code;
                return PacketResult<ProfileMiniMailSend>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ProfileMiniMailSend>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMiniMailSend Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ProfileMiniMailSend>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileMiniMailSend Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (dataField.name != "data") {
                return PacketResult<ProfileMiniMailSend>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMiniMailSend Parse data 字段名不匹配",
                    DataFieldIndex));
            }

            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<ProfileMiniMailSend>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ProfileMiniMailSend Parse data 字段类型不匹配",
                    DataFieldIndex));
            }

            const PacketField& colorField = packet.fields[ColorFieldIndex];
            if (colorField.name != "color") {
                return PacketResult<ProfileMiniMailSend>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMiniMailSend Parse color 字段名不匹配",
                    ColorFieldIndex));
            }

            if (colorField.type != PacketFieldType::Int62) {
                return PacketResult<ProfileMiniMailSend>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ProfileMiniMailSend Parse color 字段类型不匹配",
                    ColorFieldIndex));
            }

            PacketResult<ProfileMiniMailPayload> payload = ParseData(dataField.value);
            if (!payload.HasValue()) {
                return PacketResult<ProfileMiniMailSend>::Failure(payload.Error());
            }

            PacketResult<int> colorValue = ParseDecimalField(colorField.value, ColorFieldIndex);
            if (!colorValue.HasValue()) {
                return PacketResult<ProfileMiniMailSend>::Failure(colorValue.Error());
            }

            return PacketResult<ProfileMiniMailSend>::Success(ProfileMiniMailSend(
                payload.Value().profileIndex,
                std::move(payload.Value().name),
                std::move(payload.Value().message),
                colorValue.Value()));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t ColorFieldIndex = 1U;
        static constexpr std::size_t PayloadPartCount = 4U;
        static constexpr std::size_t ProfileIndexPayloadIndex = 0U;
        static constexpr std::size_t NamePayloadIndex = 1U;
        static constexpr std::size_t MessagePayloadIndex = 2U;
        static constexpr std::size_t TailPayloadIndex = 3U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::Xlsx;
            return error;
        }

        [[nodiscard]] static PacketError MakeFieldError(
            PacketErrorCode code,
            std::string message,
            std::size_t fieldIndex)
        {
            PacketError error = MakePacketError(code, std::move(message));
            error.fieldIndex = fieldIndex;
            return error;
        }

        [[nodiscard]] static PacketResult<ProfileMiniMailPayload> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "ProfileMiniMailSend payload 为空",
                    DataFieldIndex));
            }

            if (payload.back() != '|') {
                return PacketResult<ProfileMiniMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMiniMailSend payload 缺少尾随分隔符",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<ProfileMiniMailPayload>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != PayloadPartCount || !parts[TailPayloadIndex].empty()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileMiniMailSend payload 段数量不匹配",
                    DataFieldIndex));
            }

            if (parts[ProfileIndexPayloadIndex].empty()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMiniMailSend profileIndex 为空",
                    ProfileIndexPayloadIndex));
            }

            if (parts[NamePayloadIndex].empty()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMiniMailSend name 为空",
                    NamePayloadIndex));
            }

            if (parts[MessagePayloadIndex].empty()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMiniMailSend message 为空",
                    MessagePayloadIndex));
            }

            PacketResult<int> profileIndexValue = ParseDecimalField(
                parts[ProfileIndexPayloadIndex],
                ProfileIndexPayloadIndex);
            if (!profileIndexValue.HasValue()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(profileIndexValue.Error());
            }

            PacketResult<std::string> nameValue = ValidateParsedStringField(parts[NamePayloadIndex], NamePayloadIndex);
            if (!nameValue.HasValue()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(nameValue.Error());
            }

            PacketResult<std::string> messageValue =
                ValidateParsedStringField(parts[MessagePayloadIndex], MessagePayloadIndex);
            if (!messageValue.HasValue()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(messageValue.Error());
            }

            return PacketResult<ProfileMiniMailPayload>::Success(ProfileMiniMailPayload {
                profileIndexValue.Value(),
                std::move(nameValue.Value()),
                std::move(messageValue.Value()),
            });
        }

        [[nodiscard]] static PacketResult<std::string> ValidateParsedStringField(
            std::string_view text,
            std::size_t fieldIndex)
        {
            if (text.find('|') != std::string_view::npos || text.find("\\z") != std::string_view::npos) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMiniMailSend raw 字符串包含非法分隔符",
                    fieldIndex));
            }

            for (std::size_t index = 0; index < text.size(); ++index) {
                if (!cg::packet::detail::IsLegacyDoubleByteLead(text[index])) {
                    continue;
                }

                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileMiniMailSend raw 字符串旧双字节字符被截断",
                        fieldIndex));
                }
                ++index;
            }

            return PacketResult<std::string>::Success(std::string(text));
        }

        [[nodiscard]] static PacketResult<std::string> ValidateBuildStringField(
            std::string_view text,
            std::size_t fieldIndex)
        {
            if (text.empty()) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    PacketErrorCode::EncodeFailed,
                    "ProfileMiniMailSend BuildData raw 字符串为空",
                    fieldIndex));
            }

            if (text.find('|') != std::string_view::npos || text.find("\\z") != std::string_view::npos) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    PacketErrorCode::EncodeFailed,
                    "ProfileMiniMailSend BuildData raw 字符串包含非法分隔符",
                    fieldIndex));
            }

            for (std::size_t index = 0; index < text.size(); ++index) {
                if (!cg::packet::detail::IsLegacyDoubleByteLead(text[index])) {
                    continue;
                }

                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::EncodeFailed,
                        "ProfileMiniMailSend BuildData raw 字符串旧双字节字符被截断",
                        fieldIndex));
                }
                ++index;
            }

            return PacketResult<std::string>::Success(std::string(text));
        }

        [[nodiscard]] static PacketResult<int> ParseDecimalField(std::string_view text, std::size_t fieldIndex)
        {
            bool negative = false;
            std::size_t index = 0;
            if (!text.empty() && text.front() == '-') {
                negative = true;
                index = 1U;
            }

            if (index == text.size()) {
                return PacketResult<int>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMiniMailSend 十进制字段为空",
                    fieldIndex));
            }

            const std::int64_t maxValue = static_cast<std::int64_t>(std::numeric_limits<int>::max());
            const std::int64_t minValue = static_cast<std::int64_t>(std::numeric_limits<int>::min());
            const std::int64_t limit = negative ? (0 - minValue) : maxValue;
            std::int64_t value = 0;

            for (; index < text.size(); ++index) {
                const char ch = text[index];
                if (ch < '0' || ch > '9') {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileMiniMailSend 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileMiniMailSend 十进制字段超出 int 范围",
                        fieldIndex));
                }

                value = (value * 10) + digit;
            }

            if (negative) {
                value = -value;
            }

            return PacketResult<int>::Success(static_cast<int>(value));
        }
    };

    /**
     * RECV_TROP CHAR_Trade_setTradeList 取出物品 aiAb {item:es} {pet:es} {gold:i62}
     * item/pet 为普通外层 es；内部 `|` 保持 opaque，不在此处拆解。
     */
    struct TradeTakeOut
    {
        static constexpr std::string_view Code = "aiAb";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;

        std::string item;
        std::string pet;
        int gold = 0;

        TradeTakeOut(std::string item, std::string pet, int gold)
            : item(std::move(item)),
              pet(std::move(pet)),
              gold(gold)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "item",
                PacketFieldType::EscapedString,
                item,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "pet",
                PacketFieldType::EscapedString,
                pet,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "gold",
                PacketFieldType::Int62,
                std::to_string(gold),
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
     * RECV_StallStart 开始摆摊请求 ylGN {stallName:es} {stallDescription:es} {stallList:es}
     * stallList 为 25 个 raw decimal 价格并带尾随 `|`，price0..19 为背包，price20..24 为宠物。
     */
    struct StallStart
    {
        static constexpr std::string_view Code = "ylGN";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::string_view PayloadDelimiter = "|";
        static constexpr std::size_t ItemPriceCount = 20U;
        static constexpr std::size_t PetPriceCount = 5U;
        static constexpr std::size_t PriceCount = ItemPriceCount + PetPriceCount;

        std::string stallName;
        std::string stallDescription;
        std::array<std::string, PriceCount> prices;

        static std::array<std::string, PriceCount> MakeZeroPrices()
        {
            std::array<std::string, PriceCount> values;
            for (std::string& value : values) {
                value = "0";
            }
            return values;
        }

        StallStart(std::string stallName, std::string stallDescription)
            : stallName(std::move(stallName)),
              stallDescription(std::move(stallDescription)),
              prices(MakeZeroPrices())
        {
        }

        StallStart(
            std::string stallName,
            std::string stallDescription,
            std::array<std::string, PriceCount> prices)
            : stallName(std::move(stallName)),
              stallDescription(std::move(stallDescription)),
              prices(std::move(prices))
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            std::string payload;
            for (std::size_t index = 0; index < prices.size(); ++index) {
                PacketResult<std::string> price = ValidateRawDecimalPrice(
                    prices[index],
                    index,
                    PacketErrorCode::EncodeFailed,
                    "StallStart BuildData price 非 raw decimal");
                if (!price.HasValue()) {
                    return PacketResult<std::string>::Failure(price.Error());
                }

                payload.append(price.Value());
                payload.push_back('|');
            }

            return PacketResult<std::string>::Success(std::move(payload));
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            PacketResult<std::string> stallList = BuildData();
            if (!stallList.HasValue()) {
                return PacketResult<Packet>::Failure(stallList.Error());
            }

            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "stallName",
                PacketFieldType::EscapedString,
                stallName,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "stallDescription",
                PacketFieldType::EscapedString,
                stallDescription,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "stallList",
                PacketFieldType::EscapedString,
                stallList.Value(),
                PacketSourceTag::Xlsx,
            });
            return PacketResult<Packet>::Success(std::move(packet));
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            PacketResult<Packet> packet = ToPacket();
            if (!packet.HasValue()) {
                return PacketResult<std::string>::Failure(packet.Error());
            }
            return BuildPacket(packet.Value());
        }

        static PacketResult<StallStart> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<StallStart>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<StallStart> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "StallStart Parse 收到非 ylGN 封包");
                error.packetCode = packet.code;
                return PacketResult<StallStart>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<StallStart>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "StallStart Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "StallStart Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            if (packet.fields[StallNameFieldIndex].name != "stallName") {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "StallStart Parse stallName 字段名不匹配",
                    StallNameFieldIndex));
            }

            if (packet.fields[StallNameFieldIndex].type != PacketFieldType::EscapedString) {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "StallStart Parse stallName 字段类型不匹配",
                    StallNameFieldIndex));
            }

            if (packet.fields[StallDescriptionFieldIndex].name != "stallDescription") {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "StallStart Parse stallDescription 字段名不匹配",
                    StallDescriptionFieldIndex));
            }

            if (packet.fields[StallDescriptionFieldIndex].type != PacketFieldType::EscapedString) {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "StallStart Parse stallDescription 字段类型不匹配",
                    StallDescriptionFieldIndex));
            }

            const PacketField& stallListField = packet.fields[StallListFieldIndex];
            if (stallListField.name != "stallList") {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "StallStart Parse stallList 字段名不匹配",
                    StallListFieldIndex));
            }

            if (stallListField.type != PacketFieldType::EscapedString) {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "StallStart Parse stallList 字段类型不匹配",
                    StallListFieldIndex));
            }

            PacketResult<std::array<std::string, PriceCount>> parsedPrices = ParseStallList(stallListField.value);
            if (!parsedPrices.HasValue()) {
                return PacketResult<StallStart>::Failure(parsedPrices.Error());
            }

            return PacketResult<StallStart>::Success(StallStart(
                packet.fields[StallNameFieldIndex].value,
                packet.fields[StallDescriptionFieldIndex].value,
                std::move(parsedPrices.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 3U;
        static constexpr std::size_t StallNameFieldIndex = 0U;
        static constexpr std::size_t StallDescriptionFieldIndex = 1U;
        static constexpr std::size_t StallListFieldIndex = 2U;
        static constexpr std::size_t PayloadPartCount = PriceCount + 1U;
        static constexpr std::size_t TailPayloadIndex = PriceCount;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::Xlsx;
            return error;
        }

        [[nodiscard]] static PacketError MakeFieldError(
            PacketErrorCode code,
            std::string message,
            std::size_t fieldIndex)
        {
            PacketError error = MakePacketError(code, std::move(message));
            error.fieldIndex = fieldIndex;
            return error;
        }

        [[nodiscard]] static PacketResult<std::array<std::string, PriceCount>> ParseStallList(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<std::array<std::string, PriceCount>>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "StallStart stallList 为空",
                    StallListFieldIndex));
            }

            if (payload.back() != '|') {
                return PacketResult<std::array<std::string, PriceCount>>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "StallStart stallList 缺少尾随分隔符",
                    StallListFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = StallListFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<std::array<std::string, PriceCount>>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != PayloadPartCount || !parts[TailPayloadIndex].empty()) {
                return PacketResult<std::array<std::string, PriceCount>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "StallStart stallList price 段数量不匹配",
                    StallListFieldIndex));
            }

            std::array<std::string, PriceCount> parsedPrices;
            for (std::size_t index = 0; index < PriceCount; ++index) {
                PacketResult<std::string> price = ValidateRawDecimalPrice(
                    parts[index],
                    index,
                    PacketErrorCode::DecodeFailed,
                    "StallStart stallList price 非 raw decimal");
                if (!price.HasValue()) {
                    return PacketResult<std::array<std::string, PriceCount>>::Failure(price.Error());
                }

                parsedPrices[index] = std::move(price.Value());
            }

            return PacketResult<std::array<std::string, PriceCount>>::Success(std::move(parsedPrices));
        }

        [[nodiscard]] static PacketResult<std::string> ValidateRawDecimalPrice(
            std::string_view text,
            std::size_t fieldIndex,
            PacketErrorCode code,
            std::string message)
        {
            if (text.empty()) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    code,
                    std::move(message),
                    fieldIndex));
            }

            for (const char ch : text) {
                if (ch < '0' || ch > '9') {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        code,
                        std::move(message),
                        fieldIndex));
                }
            }

            return PacketResult<std::string>::Success(std::string(text));
        }
    };
}

#endif //CG_ENGINE_SEND_H
