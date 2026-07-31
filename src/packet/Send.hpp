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
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("clientHash"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("password"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("account"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("clientHash"),
                PacketFieldType::EscapedString,
                clientHash,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("password"),
                PacketFieldType::EscapedString,
                password,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("account"),
                PacketFieldType::EscapedString,
                account,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_CharList 获取角色列表 Gp
     */
    struct CharacterListRequest
    {
        static constexpr std::string_view Code = "Gp";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 0U> Fields = {};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        [[nodiscard]] Packet ToPacket() const
        {
            return Packet(PacketCode(Code), Direction);
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_CharLogin 登录 emRV {registnumber:i62} {renewal:i62} {face:i62}
     */
    struct CharacterLoginRequest
    {
        static constexpr std::string_view Code = "emRV";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("registnumber"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("renewal"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("face"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int registNumber = 0;
        int renewal = 0;
        int face = 0;

        CharacterLoginRequest(int registNumber, int renewal, int face)
            : registNumber(registNumber),
              renewal(renewal),
              face(face)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("registnumber"),
                PacketFieldType::Int62,
                std::to_string(registNumber),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("renewal"),
                PacketFieldType::Int62,
                std::to_string(renewal),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("face"),
                PacketFieldType::Int62,
                std::to_string(face),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_CharLogout 登出 WA
     */
    struct CharacterLogoutRequest
    {
        static constexpr std::string_view Code = "WA";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 0U> Fields = {};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        [[nodiscard]] Packet ToPacket() const
        {
            return Packet(PacketCode(Code), Direction);
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_CheckCommand 发送客户端时间 CCL {time:i62}
     */
    struct CheckCommand
    {
        static constexpr std::string_view Code = "CCL";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("time"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int time = 0;

        explicit CheckCommand(int time)
            : time(time)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("time"),
                PacketFieldType::Int62,
                std::to_string(time),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PRO 设置简介公开状态 lz {profileStatus:i62}
     */
    struct ProfileStatusUpdateRequest
    {
        static constexpr std::string_view Code = "lz";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("profileStatus"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int profileStatus = 0;

        explicit ProfileStatusUpdateRequest(int profileStatus)
            : profileStatus(profileStatus)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("profileStatus"),
                PacketFieldType::Int62,
                std::to_string(profileStatus),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_Echo 回显心跳 Or {payload:es}
     */
    struct EchoRequest
    {
        static constexpr std::string_view Code = "Or";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("payload"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        std::string payload;

        explicit EchoRequest(std::string payload)
            : payload(std::move(payload))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("payload"),
                PacketFieldType::EscapedString,
                payload,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_FCTime 定时心跳 DE
     */
    struct KeepAliveRequest
    {
        static constexpr std::string_view Code = "DE";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 0U> Fields = {};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        [[nodiscard]] Packet ToPacket() const
        {
            return Packet(PacketCode(Code), Direction);
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_W 移动 zA {x:i62} {y:i62} {direction:es} {warpnum:i62}
     * direction 仅接受客户端 active producer 使用的单字节 ASCII [a-hA-H]。
     */
    struct MoveRequest
    {
        static constexpr std::string_view Code = "zA";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 4U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("direction"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("warpnum"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;
        std::string direction;
        int warpNumber = 0;

        MoveRequest(int x, int y, std::string direction, int warpNumber)
            : x(x),
              y(y),
              direction(std::move(direction)),
              warpNumber(warpNumber)
        {
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            if (!IsValidDirection(direction)) {
                PacketError error = PacketError::Make(
                    PacketErrorCode::EncodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT(
                        "MoveRequest direction 必须是单字节 ASCII [a-hA-H]"));
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DirectionFieldIndex;
                return PacketResult<Packet>::Failure(std::move(error));
            }

            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("direction"),
                PacketFieldType::EscapedString,
                direction,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("warpnum"),
                PacketFieldType::Int62,
                std::to_string(warpNumber),
            });
            return PacketResult<Packet>::Success(std::move(packet));
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            PacketResult<Packet> packet = ToPacket();
            if (!packet.HasValue()) {
                return PacketResult<std::string>::Failure(packet.Error());
            }
            return BuildPacket(packet.Value(), Spec);
        }

    private:
        static constexpr std::size_t DirectionFieldIndex = 2U;

        [[nodiscard]] static bool IsValidDirection(std::string_view value) noexcept
        {
            if (value.size() != 1U) {
                return false;
            }

            const char direction = value.front();
            return (direction >= 'a' && direction <= 'h')
                || (direction >= 'A' && direction <= 'H');
        }
    };

    /**
     * RECV_SP 成功离队位置同步 cZt {x:i62} {y:i62} {dir:i62}
     * 三字段保留完整 signed int，不增加坐标、地图或方向业务约束。
     */
    struct PartyLeavePositionSync
    {
        static constexpr std::string_view Code = "cZt";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("dir"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;
        int direction = 0;

        PartyLeavePositionSync(int x, int y, int direction)
            : x(x),
              y(y),
              direction(direction)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("dir"),
                PacketFieldType::Int62,
                std::to_string(direction),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_ID ITEM_USE 使用物品 Ak {x:i62} {y:i62} {haveitemindex:i62} {toindex:i62}
     * Server legacy 不严格校验 arity，且 client/Server item-on-item 索引语义存在冲突；
     * 基础 packet 层保持 raw signed i62，不做转换或业务范围校验。
     */
    struct ItemUseRequest
    {
        static constexpr std::string_view Code = "Ak";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 4U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("haveitemindex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("toindex"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;
        int haveitemindex = 0;
        int toindex = 0;

        ItemUseRequest(int x, int y, int haveitemindex, int toindex)
            : x(x),
              y(y),
              haveitemindex(haveitemindex),
              toindex(toindex)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("haveitemindex"),
                PacketFieldType::Int62,
                std::to_string(haveitemindex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("toindex"),
                PacketFieldType::Int62,
                std::to_string(toindex),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_IDF 场外恢复物品 iVfo {x:i62} {y:i62} {haveitemindex:i62} {toindex:i62}
     * Server legacy 不严格校验 arity；基础 packet 层保留 raw signed i62，
     * 不固化背包页、OperationC、目标或物品效果等运行时业务规则。
     */
    struct FieldRecoveryItemUseRequest
    {
        static constexpr std::string_view Code = "iVfo";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 4U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("haveitemindex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("toindex"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;
        int haveitemindex = 0;
        int toindex = 0;

        FieldRecoveryItemUseRequest(int x, int y, int haveitemindex, int toindex)
            : x(x),
              y(y),
              haveitemindex(haveitemindex),
              toindex(toindex)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("haveitemindex"),
                PacketFieldType::Int62,
                std::to_string(haveitemindex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("toindex"),
                PacketFieldType::Int62,
                std::to_string(toindex),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PI 捡起物品 fIM {x:i62} {y:i62} {dir:i62}
     * 三字段保留完整 signed int；canonical 仅使用普通空格分隔。
     */
    struct PickUpItem
    {
        static constexpr std::string_view Code = "fIM";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("dir"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;
        int direction = 0;

        PickUpItem(int x, int y, int direction)
            : x(x),
              y(y),
              direction(direction)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("dir"),
                PacketFieldType::Int62,
                std::to_string(direction),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_DI 丢物品 QpfE {x:i62} {y:i62} {who:i62} {itemindex:i62}
     * 四字段保留完整 signed int；物品页与人物/宠物槽位规则不下沉封包协议层。
     */
    struct DropItem
    {
        static constexpr std::string_view Code = "QpfE";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 4U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("who"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("itemindex"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;
        int who = 0;
        int itemIndex = 0;

        DropItem(int x, int y, int who, int itemIndex)
            : x(x),
              y(y),
              who(who),
              itemIndex(itemIndex)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("who"),
                PacketFieldType::Int62,
                std::to_string(who),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("itemindex"),
                PacketFieldType::Int62,
                std::to_string(itemIndex),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_CH 作弊检测 PGXE {type:i62}
     * type 的分支语义属于业务层；封包协议保留任意 signed int。
     */
    struct CheatDetectionReport
    {
        static constexpr std::string_view Code = "PGXE";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("type"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int type = 0;

        explicit CheatDetectionReport(int type)
            : type(type)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("type"),
                PacketFieldType::Int62,
                std::to_string(type),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_EV NPC 事件 pg {event:i62} {seqno:i62} {x:i62} {y:i62} {dir:i62}
     */
    struct NpcEventRequest
    {
        static constexpr std::string_view Code = "pg";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 5U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("event"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("seqno"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("dir"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int event = 0;
        int sequenceNumber = 0;
        int x = 0;
        int y = 0;
        int direction = 0;

        NpcEventRequest(int event, int sequenceNumber, int x, int y, int direction)
            : event(event)
            , sequenceNumber(sequenceNumber)
            , x(x)
            , y(y)
            , direction(direction)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("event"),
                PacketFieldType::Int62,
                std::to_string(event),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("seqno"),
                PacketFieldType::Int62,
                std::to_string(sequenceNumber),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("dir"),
                PacketFieldType::Int62,
                std::to_string(direction),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_EN 客户端遇敌 KeOX {x:i62} {y:i62}
     * 坐标、地图与遇敌状态均由上层业务处理；基础 packet 层保留完整 signed int。
     */
    struct EncounterRequest
    {
        static constexpr std::string_view Code = "KeOX";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;

        EncounterRequest(int x, int y)
            : x(x)
            , y(y)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_DU PK 请求 ik {x:i62} {y:i62}
     */
    struct DuelRequest
    {
        static constexpr std::string_view Code = "ik";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;

        DuelRequest(int x, int y)
            : x(x)
            , y(y)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_EO 离开战斗界面 eL {dummy:i62}
     * 客户端约定 dummy 固定为 0，服务端接收后忽略该值。
     */
    struct BattleExitRequest
    {
        static constexpr std::string_view Code = "eL";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr int Dummy = 0;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("dummy"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("dummy"),
                PacketFieldType::Int62,
                std::to_string(Dummy),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_JB deprecated compatibility BattleJoin cEj {x:i62} {y:i62}
     * 当前服务端 handler 为 no-op；仅保留协议兼容，不恢复旧 #if 0 业务逻辑。
     */
    struct BattleJoinRequest
    {
        static constexpr std::string_view Code = "cEj";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;

        BattleJoinRequest(int x, int y)
            : x(x),
              y(y)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_LB 请求观战 IMv {x:i62} {y:i62}
     * 坐标保留完整 signed int，不增加运行时移动与地图业务约束。
     */
    struct BattleWatchRequest
    {
        static constexpr std::string_view Code = "IMv";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;

        BattleWatchRequest(int x, int y)
            : x(x),
              y(y)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_BU 停止观战 NgMy {dummy:i62}
     * 客户端合同固定 dummy 为 0；服务端接收后忽略该值。
     */
    struct BattleWatchStopRequest
    {
        static constexpr std::string_view Code = "NgMy";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr int Dummy = 0;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("dummy"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("dummy"),
                PacketFieldType::Int62,
                std::to_string(Dummy),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_B 战斗指令 Rg {command:es}
     * command 数值操作数沿用客户端 %X 语义，统一编码为大写十六进制。
     */
    class BattleCommandPayload
    {
    public:
        using Operand = std::uint32_t;

        [[nodiscard]] static BattleCommandPayload PlayerSkill(
            Operand skillSlot,
            Operand techniqueSlot,
            Operand target)
        {
            return BattleCommandPayload(
                Kind::PlayerSkill,
                {skillSlot, techniqueSlot, target});
        }

        [[nodiscard]] static BattleCommandPayload PetSkill(
            Operand skillSlot,
            Operand target)
        {
            return BattleCommandPayload(Kind::PetSkill, {skillSlot, target, 0U});
        }

        [[nodiscard]] static BattleCommandPayload PetNoAction()
        {
            return BattleCommandPayload(Kind::PetNoAction);
        }

        [[nodiscard]] static BattleCommandPayload Attack(Operand target)
        {
            return BattleCommandPayload(Kind::Attack, {target, 0U, 0U});
        }

        [[nodiscard]] static BattleCommandPayload Equipment(Operand itemSlot)
        {
            return BattleCommandPayload(Kind::Equipment, {itemSlot, 0U, 0U});
        }

        [[nodiscard]] static BattleCommandPayload PetChange(Operand petSlot)
        {
            return BattleCommandPayload(Kind::PetChange, {petSlot, 0U, 0U});
        }

        [[nodiscard]] static BattleCommandPayload UseItem(
            Operand itemSlot,
            Operand target)
        {
            return BattleCommandPayload(Kind::UseItem, {itemSlot, target, 0U});
        }

        [[nodiscard]] static BattleCommandPayload Position()
        {
            return BattleCommandPayload(Kind::Position);
        }

        [[nodiscard]] static BattleCommandPayload NoAction()
        {
            return BattleCommandPayload(Kind::NoAction);
        }

        [[nodiscard]] static BattleCommandPayload Guard()
        {
            return BattleCommandPayload(Kind::Guard);
        }

        [[nodiscard]] static BattleCommandPayload Escape()
        {
            return BattleCommandPayload(Kind::Escape);
        }

        [[nodiscard]] static BattleCommandPayload WatchStop()
        {
            return BattleCommandPayload(Kind::WatchStop);
        }

        [[nodiscard]] static BattleCommandPayload Rebirth(Operand target)
        {
            return BattleCommandPayload(Kind::Rebirth, {target, 0U, 0U});
        }

        [[nodiscard]] std::string Encode() const
        {
            std::string command;
            switch (kind_) {
            case Kind::PlayerSkill:
                command = "S|";
                AppendHex(command, operands_[0]);
                command.push_back('|');
                AppendHex(command, operands_[1]);
                command.push_back('|');
                AppendHex(command, operands_[2]);
                break;
            case Kind::PetSkill:
                command = "W|";
                AppendHex(command, operands_[0]);
                command.push_back('|');
                AppendHex(command, operands_[1]);
                break;
            case Kind::PetNoAction:
                command = "W|FF";
                break;
            case Kind::Attack:
                command = "H|";
                AppendHex(command, operands_[0]);
                break;
            case Kind::Equipment:
                command = "Q|";
                AppendHex(command, operands_[0]);
                command.push_back('|');
                break;
            case Kind::PetChange:
                command = "M|";
                AppendHex(command, operands_[0]);
                break;
            case Kind::UseItem:
                command = "I|";
                AppendHex(command, operands_[0]);
                command.push_back('|');
                AppendHex(command, operands_[1]);
                break;
            case Kind::Position:
                command = "P";
                break;
            case Kind::NoAction:
                command = "N";
                break;
            case Kind::Guard:
                command = "G";
                break;
            case Kind::Escape:
                command = "E";
                break;
            case Kind::WatchStop:
                command = "U";
                break;
            case Kind::Rebirth:
                command = "R|";
                AppendHex(command, operands_[0]);
                break;
            }
            return command;
        }

    private:
        enum class Kind
        {
            PlayerSkill,
            PetSkill,
            PetNoAction,
            Attack,
            Equipment,
            PetChange,
            UseItem,
            Position,
            NoAction,
            Guard,
            Escape,
            WatchStop,
            Rebirth,
        };

        Kind kind_ = Kind::NoAction;
        std::array<Operand, 3U> operands_ {};

        explicit BattleCommandPayload(
            Kind kind,
            std::array<Operand, 3U> operands = {})
            : kind_(kind),
              operands_(operands)
        {
        }

        static void AppendHex(std::string& output, Operand value)
        {
            static constexpr std::string_view HexDigits = "0123456789ABCDEF";
            std::array<char, 8U> reversed {};
            std::size_t size = 0U;
            do {
                reversed[size++] = HexDigits[value & 0x0FU];
                value >>= 4U;
            } while (value != 0U);

            while (size != 0U) {
                output.push_back(reversed[--size]);
            }
        }
    };

    /**
     * RECV_B 战斗指令 Rg {command:es}
     */
    struct BattleCommandRequest
    {
        static constexpr std::string_view Code = "Rg";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("command"), PacketFieldType::SubPayload},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        BattleCommandPayload command;

        explicit BattleCommandRequest(BattleCommandPayload command)
            : command(std::move(command))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("command"),
                PacketFieldType::EscapedString,
                command.Encode(),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_GFLI 请求物品角色列表 as
     */
    struct ItemFrontCharListRequest
    {
        static constexpr std::string_view Code = "as";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 0U> Fields = {};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        [[nodiscard]] Packet ToPacket() const
        {
            return Packet(PacketCode(Code), Direction);
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_TRPL 请求交易玩家列表 ixH
     */
    struct TradePlayerListRequest
    {
        static constexpr std::string_view Code = "ixH";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 0U> Fields = {};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        [[nodiscard]] Packet ToPacket() const
        {
            return Packet(PacketCode(Code), Direction);
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_TRS 从交易列表选择交易对象 QeuC {tradeListSlot:i62}
     * tradeListSlot 是交易候选列表槽位；基础 packet 层保留任意 signed int，
     * 严格按 exact1 编码，不下沉槽有效性与交易业务规则。
     */
    struct TradeStartRequest
    {
        static constexpr std::string_view Code = "QeuC";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("tradeListSlot"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int tradeListSlot = 0;

        explicit TradeStartRequest(int tradeListSlot)
            : tradeListSlot(tradeListSlot)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("tradeListSlot"),
                PacketFieldType::Int62,
                std::to_string(tradeListSlot),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_TROC 确认或取消交易 OU {result:i62}
     * legacy producer 当前只发送 0/1，但 wire 与服务端均按 signed int
     * 的零/非零处理；基础 packet 层不收窄 enum，也不下沉交易规则。
     */
    struct TradeConfirmationRequest
    {
        static constexpr std::string_view Code = "OU";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("result"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int result = 0;

        explicit TradeConfirmationRequest(int result)
            : result(result)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("result"),
                PacketFieldType::Int62,
                std::to_string(result),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_ST 设置或清除称号 zGp {titleSlot:i62}
     * titleSlot 是持有称号槽位，legacy producer 以 -1 表示清除；
     * 基础 packet 层保留任意 signed int，不下沉称号业务规则。
     */
    struct TitleSelectionRequest
    {
        static constexpr std::string_view Code = "zGp";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("titleSlot"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int titleSlot = 0;

        explicit TitleSelectionRequest(int titleSlot)
            : titleSlot(titleSlot)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("titleSlot"),
                PacketFieldType::Int62,
                std::to_string(titleSlot),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_DT 删除持有称号 tj {titleSlot:i62}
     * titleSlot 是持有称号槽位；基础 packet 层保留任意 signed int，
     * 不下沉槽范围、称号存在或装备状态等业务规则。
     */
    struct TitleDeletionRequest
    {
        static constexpr std::string_view Code = "tj";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("titleSlot"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int titleSlot = 0;

        explicit TitleDeletionRequest(int titleSlot)
            : titleSlot(titleSlot)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("titleSlot"),
                PacketFieldType::Int62,
                std::to_string(titleSlot),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_FT 输入自定义称号 ACwB {title:es}
     * title 是单层 legacy escaped string；基础 packet 层不限制长度，
     * 不承担 locale 转换、角色状态或广播等业务规则。
     */
    struct CustomTitleInputRequest
    {
        static constexpr std::string_view Code = "ACwB";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("title"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        std::string title;

        explicit CustomTitleInputRequest(std::string title)
            : title(std::move(title))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("title"),
                PacketFieldType::EscapedString,
                title,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_LVUP 人物加点 IHw {sqeno:i62}
     * sqeno 的 0..4 映射与角色状态校验属于服务端业务层；
     * 基础 packet 层保留任意 signed int。
     */
    struct CharacterLevelUpPoint
    {
        static constexpr std::string_view Code = "IHw";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("sqeno"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int sqeno = 0;

        explicit CharacterLevelUpPoint(int sqeno)
            : sqeno(sqeno)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("sqeno"),
                PacketFieldType::Int62,
                std::to_string(sqeno),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_SKSW 人物技能移动 Noeb {from:i62} {to:i62}
     * to=-1 是服务端自动整理语义；技能槽范围与技能状态属于业务层，
     * 基础 packet 层保留两个字段的任意 signed int。
     */
    struct CharacterSkillSwap
    {
        static constexpr std::string_view Code = "Noeb";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("from"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("to"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int from = 0;
        int to = 0;

        CharacterSkillSwap(int from, int to)
            : from(from),
              to(to)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("from"),
                PacketFieldType::Int62,
                std::to_string(from),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("to"),
                PacketFieldType::Int62,
                std::to_string(to),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_CharDelete 删除角色 Godf {registNumber:i62}
     * registNumber 是角色注册号；登录态、授权、账号归属与数据库删除结果
     * 属于服务端业务层，基础 packet 层保留任意 signed int。
     */
    struct CharacterDeleteRequest
    {
        static constexpr std::string_view Code = "Godf";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("registNumber"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int registNumber = 0;

        explicit CharacterDeleteRequest(int registNumber)
            : registNumber(registNumber)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("registNumber"),
                PacketFieldType::Int62,
                std::to_string(registNumber),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PVUP 历史迁移 Xoee {mode:i62} {productKey:es}
     * 迁移模式、鉴权与数据库队列属于服务端业务层；基础 packet 层
     * 保留任意 signed int mode 与单层 legacy productKey bytes。
     */
    struct VersionUpgradeRequest
    {
        static constexpr std::string_view Code = "Xoee";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("mode"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("productKey"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int mode = 0;
        std::string productKey;

        VersionUpgradeRequest(int mode, std::string productKey)
            : mode(mode),
              productKey(std::move(productKey))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("mode"),
                PacketFieldType::Int62,
                std::to_string(mode),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("productKey"),
                PacketFieldType::EscapedString,
                productKey,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_TRCL 取消交易选择 suI
     * VERSION_TW 服务端分支传入的 0 是 handler 业务参数，不是 wire 字段。
     */
    struct TradeSelectionCancelRequest
    {
        static constexpr std::string_view Code = "suI";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 0U> Fields = {};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        [[nodiscard]] Packet ToPacket() const
        {
            return Packet(PacketCode(Code), Direction);
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_POS 切换战斗位置 ih
     * 服务端 CHAR_Position(ch, 0) 的 0 是内部 toggle 模式，不是 wire 字段；
     * 新位置只出现在服务端 Ov 回包。
     */
    struct BattlePositionToggleRequest
    {
        static constexpr std::string_view Code = "ih";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 0U> Fields = {};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        [[nodiscard]] Packet ToPacket() const
        {
            return Packet(PacketCode(Code), Direction);
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_CharLoginGate 回城内登录点 lO
     * 与 WA 普通角色登出/服务器选择不同。
     */
    struct ReturnToLoginPointRequest
    {
        static constexpr std::string_view Code = "lO";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 0U> Fields = {};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        [[nodiscard]] Packet ToPacket() const
        {
            return Packet(PacketCode(Code), Direction);
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_StallEnd 结束摆摊请求 JMh
     * 名称保留 Request 后缀，避免与服务端 KXhH 结果类型混淆。
     */
    struct StallEndRequest
    {
        static constexpr std::string_view Code = "JMh";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 0U> Fields = {};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        [[nodiscard]] Packet ToPacket() const
        {
            return Packet(PacketCode(Code), Direction);
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_StallBrowse 摆摊浏览请求 ElVN {objIndex:i62}
     * objIndex 是摊位所属世界对象索引；业务有效性不下沉封包协议层。
     */
    struct StallBrowseRequest
    {
        static constexpr std::string_view Code = "ElVN";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("objIndex"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int objectIndex = 0;

        explicit StallBrowseRequest(int objectIndex)
            : objectIndex(objectIndex)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("objIndex"),
                PacketFieldType::Int62,
                std::to_string(objectIndex),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_StallBuy 摆摊购买请求 wAo {position:i62}
     * position 是卖家摊位绝对槽；宠物/物品槽范围不下沉封包协议层。
     */
    struct StallBuyRequest
    {
        static constexpr std::string_view Code = "wAo";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("position"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int position = 0;

        explicit StallBuyRequest(int position)
            : position(position)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("position"),
                PacketFieldType::Int62,
                std::to_string(position),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_BagPageMgr 菜单型背包窗口请求 SWITMM
     * 客户端合同固定零字段；Server 内建路径对额外 token 较宽松。
     */
    struct BagPageManagerRequest
    {
        static constexpr std::string_view Code = "SWITMM";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 0U> Fields = {};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        [[nodiscard]] Packet ToPacket() const
        {
            return Packet(PacketCode(Code), Direction);
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
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
        static constexpr std::array<PacketFieldSpec, 13U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("dataPlace"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("name"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("image"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("face"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("vital"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("strength"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("tough"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("quick"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("magic"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("earth"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("water"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("fire"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("wind"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("dataPlace"),
                PacketFieldType::Int62,
                std::to_string(dataPlace),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("name"),
                PacketFieldType::EscapedString,
                name,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("image"),
                PacketFieldType::Int62,
                std::to_string(image),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("face"),
                PacketFieldType::Int62,
                std::to_string(face),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("vital"),
                PacketFieldType::Int62,
                std::to_string(vital),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("strength"),
                PacketFieldType::Int62,
                std::to_string(strength),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("tough"),
                PacketFieldType::Int62,
                std::to_string(tough),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("quick"),
                PacketFieldType::Int62,
                std::to_string(quick),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("magic"),
                PacketFieldType::Int62,
                std::to_string(magic),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("earth"),
                PacketFieldType::Int62,
                std::to_string(earth),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("water"),
                PacketFieldType::Int62,
                std::to_string(water),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("fire"),
                PacketFieldType::Int62,
                std::to_string(fire),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("wind"),
                PacketFieldType::Int62,
                std::to_string(wind),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_M 获取地图信息 UUN {mapid:i62} {floor:i62} {x1:i62} {y1:i62} {x2:i62} {y2:i62}
     */
    struct MapDataRequest
    {
        static constexpr std::string_view Code = "UUN";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 6U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("mapid"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("floor"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("x1"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y1"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("x2"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y2"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("mapid"),
                PacketFieldType::Int62,
                std::to_string(mapId),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("floor"),
                PacketFieldType::Int62,
                std::to_string(floor),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x1"),
                PacketFieldType::Int62,
                std::to_string(x1),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y1"),
                PacketFieldType::Int62,
                std::to_string(y1),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x2"),
                PacketFieldType::Int62,
                std::to_string(x2),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y2"),
                PacketFieldType::Int62,
                std::to_string(y2),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PRS SetProfileData 简介内容 Ot {sellFlag:i62} {sellMsg:es} {buyFlag:i62} {buyMsg:es} {aboutFlg:i62} {aboutMsg:es} {ProfileMsg:es}
     */
    struct SetProfileData
    {
        static constexpr std::string_view Code = "Ot";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 7U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("sellFlag"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("sellMsg"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("buyFlag"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("buyMsg"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("aboutFlg"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("aboutMsg"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("ProfileMsg"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("sellFlag"),
                PacketFieldType::Int62,
                std::to_string(sellFlag),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("sellMsg"),
                PacketFieldType::EscapedString,
                sellMessage,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("buyFlag"),
                PacketFieldType::Int62,
                std::to_string(buyFlag),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("buyMsg"),
                PacketFieldType::EscapedString,
                buyMessage,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("aboutFlg"),
                PacketFieldType::Int62,
                std::to_string(aboutFlag),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("aboutMsg"),
                PacketFieldType::EscapedString,
                aboutMessage,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("ProfileMsg"),
                PacketFieldType::EscapedString,
                profileMessage,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_MU MAGIC_Use 使用魔法 RXr {x:i62} {y:i62} {array:i62} {toindex:i62}
     */
    struct MagicUseRequest
    {
        static constexpr std::string_view Code = "RXr";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 4U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("array"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("toindex"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("array"),
                PacketFieldType::Int62,
                std::to_string(array),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("toindex"),
                PacketFieldType::Int62,
                std::to_string(toindex),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PS PETSKILL_Use 宠物技能使用 Flvp {havepetindex:i62} {havepetskill:i62} {toindex:i62} {data:es}
     */
    struct PetSkillUseRequest
    {
        static constexpr std::string_view Code = "Flvp";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 4U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("havepetindex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("havepetskill"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("toindex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("data"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("havepetindex"),
                PacketFieldType::Int62,
                std::to_string(havepetindex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("havepetskill"),
                PacketFieldType::Int62,
                std::to_string(havepetskill),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("toindex"),
                PacketFieldType::Int62,
                std::to_string(toindex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("data"),
                PacketFieldType::EscapedString,
                data,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PLVUP 宠物加点 kjSK {petSlot:i62} {sqeno:i62}
     */
    struct PetLevelUpPoint
    {
        static constexpr std::string_view Code = "kjSK";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("petSlot"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("sqeno"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int petSlot = 0;
        int sqeno = 0;

        PetLevelUpPoint(int petSlot, int sqeno)
            : petSlot(petSlot),
              sqeno(sqeno)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("petSlot"),
                PacketFieldType::Int62,
                std::to_string(petSlot),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("sqeno"),
                PacketFieldType::Int62,
                std::to_string(sqeno),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PSSW SKILL_swapPetSkill 宠物技能移动 HJQp {petSlot:i62} {from:i62} {to:i62}
     */
    struct PetSkillSwap
    {
        static constexpr std::string_view Code = "HJQp";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("petSlot"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("from"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("to"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("petSlot"),
                PacketFieldType::Int62,
                std::to_string(petSlot),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("from"),
                PacketFieldType::Int62,
                std::to_string(from),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("to"),
                PacketFieldType::Int62,
                std::to_string(to),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_KN CHAR_inputUserPetName 宠物姓名 ycX {petSlot:i62} {name:es}
     */
    struct UserPetNameInput
    {
        static constexpr std::string_view Code = "ycX";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("petSlot"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("name"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int petSlot = 0;
        std::string name;

        UserPetNameInput(int petSlot, std::string name)
            : petSlot(petSlot),
              name(std::move(name))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("petSlot"),
                PacketFieldType::Int62,
                std::to_string(petSlot),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("name"),
                PacketFieldType::EscapedString,
                name,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_MSG 发送邮件信息 CV {index:i62} {message:es} {color:i62}
     * 调用方负责 legacy n/c/z/y 内容预转义；message 在此仅是普通外层 es 标量，
     * 不拆 SubPayload，也不增加组标记、索引或颜色业务约束。
     */
    struct MailMessageSend
    {
        static constexpr std::string_view Code = "CV";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("index"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("message"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("color"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("index"),
                PacketFieldType::Int62,
                std::to_string(index),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("message"),
                PacketFieldType::EscapedString,
                message,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("color"),
                PacketFieldType::Int62,
                std::to_string(color),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_TK 发送聊天 uSr {x:i62} {y:i62} {msg:es} {color:i62} {area:i62} {fontSize:i62}
     */
    struct ChatMessageSend
    {
        static constexpr std::string_view Code = "uSr";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 6U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("msg"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("color"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("area"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("fontSize"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("msg"),
                PacketFieldType::EscapedString,
                msg,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("color"),
                PacketFieldType::Int62,
                std::to_string(color),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("area"),
                PacketFieldType::Int62,
                std::to_string(area),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("fontSize"),
                PacketFieldType::Int62,
                std::to_string(fontSize),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_WN 打开系统窗口 xD {x:i62} {y:i62} {seqno:i62} {objindex:i62} {select:i62} {data:es}
     */
    struct WindowOpenSend
    {
        static constexpr std::string_view Code = "xD";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 6U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("seqno"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("objindex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("select"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("data"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("seqno"),
                PacketFieldType::Int62,
                std::to_string(seqno),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("objindex"),
                PacketFieldType::Int62,
                std::to_string(objindex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("select"),
                PacketFieldType::Int62,
                std::to_string(select),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("data"),
                PacketFieldType::EscapedString,
                data,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_Shutdown 停机请求 IcE {passwd:es} {min:i62}
     */
    struct ShutdownRequest
    {
        static constexpr std::string_view Code = "IcE";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("passwd"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("min"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        std::string passwd;
        int min = 0;

        ShutdownRequest(std::string passwd, int min)
            : passwd(std::move(passwd)),
              min(min)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("passwd"),
                PacketFieldType::EscapedString,
                passwd,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("min"),
                PacketFieldType::Int62,
                std::to_string(min),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PMSG 发送宠邮 rMsv {index:i62} {petindex:i62} {itemindex:i62} {message:es} {color:i62}
     * message 是调用方已完成 legacy n/c/z/y 内容预转义后的普通外层 es 标量，
     * 不是 SubPayload；packet 层不执行该内容预转换。
     */
    struct PetMailSend
    {
        static constexpr std::string_view Code = "rMsv";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 5U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("index"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("petindex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("itemindex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("message"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("color"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("index"),
                PacketFieldType::Int62,
                std::to_string(index),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("petindex"),
                PacketFieldType::Int62,
                std::to_string(petindex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("itemindex"),
                PacketFieldType::Int62,
                std::to_string(itemindex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("message"),
                PacketFieldType::EscapedString,
                message,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("color"),
                PacketFieldType::Int62,
                std::to_string(color),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PGML 宠邮登记 PfqV {toregist:i62} {petSlot:i62} {itemSlot:i62} {text:es} {color:i62}
     * text 是调用方已完成 legacy n/c/z/y 内容预转义后的普通外层 es 标量，不是 SubPayload；
     * packet 层不执行该内容预转换，color | 0x100 由 Server handler 处理。
     */
    struct PetMailRegister
    {
        static constexpr std::string_view Code = "PfqV";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 5U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("toregist"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("petSlot"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("itemSlot"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("text"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("color"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("toregist"),
                PacketFieldType::Int62,
                std::to_string(toregist),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("petSlot"),
                PacketFieldType::Int62,
                std::to_string(petSlot),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("itemSlot"),
                PacketFieldType::Int62,
                std::to_string(itemSlot),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("text"),
                PacketFieldType::EscapedString,
                text,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("color"),
                PacketFieldType::Int62,
                std::to_string(color),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_DG DropGold 丢金币 woz {x:i62} {y:i62} {amount:i62}
     * Server 的坐标同步、战斗状态、amount/持有金币、地图与落点检查均为业务规则；
     * wire 保留三个 signed int，固定 exact3，无二级 payload，不兼容 legacy 0x17 分隔异常。
     */
    struct DropGold
    {
        static constexpr std::string_view Code = "woz";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("amount"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("amount"),
                PacketFieldType::Int62,
                std::to_string(amount),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_DP DropPet 丢宠物 FvWm {x:i62} {y:i62} {petindex:i62}
     * Server 业务域要求 petindex 为 0..4；wire 保留完整 signed int，不在 packet 层收窄。
     */
    struct DropPet
    {
        static constexpr std::string_view Code = "FvWm";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("petindex"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;
        int petindex = 0;

        DropPet(int x, int y, int petindex)
            : x(x),
              y(y),
              petindex(petindex)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("petindex"),
                PacketFieldType::Int62,
                std::to_string(petindex),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_MI MoveItem 移动物品 yi {fromindex:i62} {toindex:i62} {num:i62}
     * Server legacy 不严格校验 arity；0x4885E5 仅能静态证明索引 <100，
     * 基础 packet 层保留完整 signed int，不固化索引、页转换或数量业务规则。
     */
    struct ItemMoveRequest
    {
        static constexpr std::string_view Code = "yi";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("fromindex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("toindex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("num"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int fromindex = 0;
        int toindex = 0;
        int num = 0;

        ItemMoveRequest(int fromindex, int toindex, int num)
            : fromindex(fromindex),
              toindex(toindex),
              num(num)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("fromindex"),
                PacketFieldType::Int62,
                std::to_string(fromindex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("toindex"),
                PacketFieldType::Int62,
                std::to_string(toindex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("num"),
                PacketFieldType::Int62,
                std::to_string(num),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_IR RequestItemRecipe 获取配方 fS {haveskillindex:i62}
     * >=100 与 %100 是 Server handler 业务解释；wire 保留完整 signed int，不在 packet 层收窄。
     */
    struct ItemRecipeRequest
    {
        static constexpr std::string_view Code = "fS";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("haveskillindex"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int haveSkillIndex = 0;

        explicit ItemRecipeRequest(int haveSkillIndex)
            : haveSkillIndex(haveSkillIndex)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("haveskillindex"),
                PacketFieldType::Int62,
                std::to_string(haveSkillIndex),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_ALI 请求图鉴数据 kza {count:i62} {albumid:i62}...
     * count 由 albumIds 数量派生；每个 albumid 都是独立 outer i62 token。
     * 当前 Server 单 albumid 合同与 cg2d 不兼容，不作为此 typed 合同来源。
     */
    struct AlbumInfoRequest
    {
        static constexpr std::string_view Code = "kza";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("count"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("albumid"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
            {true, 0U},
        };

        std::vector<int> albumIds;

        explicit AlbumInfoRequest(std::vector<int> albumIds)
            : albumIds(std::move(albumIds))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.reserve(albumIds.size() + 1U);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("count"),
                PacketFieldType::Int62,
                std::to_string(albumIds.size()),
            });
            for (const int albumId : albumIds) {
                packet.fields.push_back(PacketField {
                    CG_PACKET_DIAGNOSTIC_TEXT("albumid"),
                    PacketFieldType::Int62,
                    std::to_string(albumId),
                });
            }
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_ALN 请求图鉴名字 LaR {albumid:i62}
     * 图鉴编号范围、解锁状态与模板存在性均为 Server handler 业务规则；
     * 基础 packet 层保留完整 signed int，不实现 legacy 宽松 arity。
     */
    struct AlbumNameRequest
    {
        static constexpr std::string_view Code = "LaR";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("albumid"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int albumId = 0;

        explicit AlbumNameRequest(int albumId)
            : albumId(albumId)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("albumid"),
                PacketFieldType::Int62,
                std::to_string(albumId),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_DAB 删除名片 FPkR {index:i62}
     * index 保留完整 signed int；地址簿范围、登录状态、删除与回复均由服务端处理。
     */
    struct AddressBookDeleteRequest
    {
        static constexpr std::string_view Code = "FPkR";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("index"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int index = 0;

        explicit AddressBookDeleteRequest(int index)
            : index(index)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("index"),
                PacketFieldType::Int62,
                std::to_string(index),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_AAB 交互名片 vcAz {x:i62} {y:i62}
     * 坐标、距离、地图、状态、目标与名片规则均由服务端处理；
     * 基础 packet 层保留完整 signed int，不复刻 legacy 宽松 arity。
     */
    struct AddressBookExchangeRequest
    {
        static constexpr std::string_view Code = "vcAz";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;

        AddressBookExchangeRequest(int x, int y)
            : x(x),
              y(y)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_GD 解散家族 nkda {guildId:i62}
     * guildId 保留完整 signed int；权限与当前家族匹配由服务端校验。
     */
    struct GuildDissolveRequest
    {
        static constexpr std::string_view Code = "nkda";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("guildId"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int guildId = 0;

        explicit GuildDissolveRequest(int guildId)
            : guildId(guildId)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("guildId"),
                PacketFieldType::Int62,
                std::to_string(guildId),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_GI 变更家族信息 Ufm {c:i62} {data:es}
     * data 是调用方先完成 legacy n/c/z/y 内容预转义、再由 packet 层按普通 outer es 编码的标量，
     * 不是 SubPayload；packet 层不执行内容预转换，也不增加 c 枚举、范围或 handler 业务约束。
     */
    struct GuildInfoChange
    {
        static constexpr std::string_view Code = "Ufm";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("c"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("data"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int c = 0;
        std::string data;

        GuildInfoChange(int c, std::string data)
            : c(c),
              data(std::move(data))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("c"),
                PacketFieldType::Int62,
                std::to_string(c),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("data"),
                PacketFieldType::EscapedString,
                data,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_GT 设置家族职位 GY {index:i62} {bitflag:i62} {data:es}
     */
    struct GuildTitleSet
    {
        static constexpr std::string_view Code = "GY";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("index"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("bitflag"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("data"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("index"),
                PacketFieldType::Int62,
                std::to_string(index),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("bitflag"),
                PacketFieldType::Int62,
                std::to_string(bitflag),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("data"),
                PacketFieldType::EscapedString,
                data,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_GMR 请求家族成员列表 CZ {time:i62}
     * Server 的宽松 arity、家族状态与缓存时间比较均为业务规则；
     * 基础 packet 层保留完整 signed int，不把独立 GI/GM 当作 ACK 或二级 payload，
     * 也不兼容 legacy 0x17 异常。
     */
    struct GuildMemberListRequest
    {
        static constexpr std::string_view Code = "CZ";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("time"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int time = 0;

        explicit GuildMemberListRequest(int time)
            : time(time)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("time"),
                PacketFieldType::Int62,
                std::to_string(time),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_BGT 变更家族成员职位 uHb {index:i62} {title:i62}
     * 基础 packet 层保留两个完整 signed int 并严格按 exact2 编码；
     * Server 宽松 arity、title 范围、权限、成员状态与独立通知均为业务规则，不兼容 legacy 0x17 异常。
     */
    struct GuildMemberTitleChangeRequest
    {
        static constexpr std::string_view Code = "uHb";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("index"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("title"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int index = 0;
        int titleId = 0;

        GuildMemberTitleChangeRequest(int index, int titleId)
            : index(index),
              titleId(titleId)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("index"),
                PacketFieldType::Int62,
                std::to_string(index),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("title"),
                PacketFieldType::Int62,
                std::to_string(titleId),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_AGM 家族邀请 npzi {x:i62} {y:i62}
     * 基础 packet 层保留两个完整 signed int 并严格按 exact2 编码；
     * Server 宽松 arity、坐标、地图、距离、邻格、家族、权限、状态与独立通知均为业务规则，
     * 不兼容 legacy 0x17 异常。
     */
    struct GuildMemberInviteRequest
    {
        static constexpr std::string_view Code = "npzi";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;

        GuildMemberInviteRequest(int x, int y)
            : x(x),
              y(y)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_LG 离开家族 XIsR {guildId:i62}
     * 基础 packet 层保留完整 signed int 并严格按 exact1 编码；
     * Server 宽松 arity、当前家族匹配、家族长、成员状态与独立通知均为业务规则，
     * 不兼容 legacy 0x17 异常。
     */
    struct GuildLeaveRequest
    {
        static constexpr std::string_view Code = "XIsR";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("guildId"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int guildId = 0;

        explicit GuildLeaveRequest(int guildId)
            : guildId(guildId)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("guildId"),
                PacketFieldType::Int62,
                std::to_string(guildId),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_RGM 移除家族成员 dbQC {index:i62}
     * 基础 packet 层保留完整 signed int 并严格按 exact1 编码；
     * Server 宽松 arity、家族、目标、权限、在线状态与解散均为业务规则，
     * 不兼容 legacy 0x17 异常。
     */
    struct GuildMemberRemoveRequest
    {
        static constexpr std::string_view Code = "dbQC";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("index"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int index = 0;

        explicit GuildMemberRemoveRequest(int index)
            : index(index)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("index"),
                PacketFieldType::Int62,
                std::to_string(index),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PRV 请求个人资料列表 HoW {flg:i62}
     * 基础 packet 层保留完整 signed int 并严格按 exact1 编码；
     * Server 宽松 arity、登录、配置、flags mask 与独立列表响应均为业务规则，
     * 不兼容 legacy 0x17 异常。
     */
    struct ProfileListsRequest
    {
        static constexpr std::string_view Code = "HoW";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("flg"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int flags = 0;

        explicit ProfileListsRequest(int flags)
            : flags(flags)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("flg"),
                PacketFieldType::Int62,
                std::to_string(flags),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PRD 请求个人资料 TLKH {flg:i62} {index:i62}
     * 基础 packet 层保留两个字段的完整 signed int 并严格按 exact2 编码；
     * Server 宽松 arity、stall、登录、索引、缓存与 flg 分派均为业务规则，
     * 不兼容 legacy 0x17 异常。
     */
    struct ProfileDataRequest
    {
        static constexpr std::string_view Code = "TLKH";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("flg"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("index"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int flg = 0;
        int index = 0;

        ProfileDataRequest(int flg, int index)
            : flg(flg)
            , index(index)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("flg"),
                PacketFieldType::Int62,
                std::to_string(flg),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("index"),
                PacketFieldType::Int62,
                std::to_string(index),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_FS 更新玩家状态标志 dcMv {flags:i62}
     * 基础 packet 层保留完整 signed int 并严格按 exact1 编码；
     * Server 宽松 arity、登录与各状态位写入均为业务规则，
     * 不兼容 legacy 0x17 异常。
     */
    struct PlayerStatusFlagsUpdateRequest
    {
        static constexpr std::string_view Code = "dcMv";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("flags"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int flags = 0;

        explicit PlayerStatusFlagsUpdateRequest(int flags)
            : flags(flags)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("flags"),
                PacketFieldType::Int62,
                std::to_string(flags),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PR 进出队伍 zn {x:i62} {y:i62} {join:i62}
     * request 0/1 的离队/入队语义及位置、资格、队伍检查均为业务规则；
     * 基础 packet 层保留完整 signed int 并严格按 exact3 编码，
     * 不兼容 legacy 宽松 arity 与 0x17 异常。
     */
    struct PartyMembershipRequest
    {
        static constexpr std::string_view Code = "zn";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("join"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;
        int request = 0;

        PartyMembershipRequest(int x, int y, int request)
            : x(x)
            , y(y)
            , request(request)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.reserve(Fields.size());
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("join"),
                PacketFieldType::Int62,
                std::to_string(request),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_KS 宠物战斗状态
     * LKQy {petStatus0:i62} {petStatus1:i62} {petStatus2:i62}
     *      {petStatus3:i62} {petStatus4:i62}
     * 状态值含义与组合由业务层解释；基础 packet 层保留任意 signed int，
     * 严格按 exact5 编码，不兼容 legacy 宽松 arity 与 0x17 异常。
     */
    struct PetBattleStatusUpdateRequest
    {
        static constexpr std::string_view Code = "LKQy";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 5U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("petStatus0"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("petStatus1"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("petStatus2"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("petStatus3"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("petStatus4"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        std::array<int, 5U> petStatuses;

        explicit PetBattleStatusUpdateRequest(std::array<int, 5U> petStatuses)
            : petStatuses(std::move(petStatuses))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.reserve(Fields.size());
            for (std::size_t index = 0U; index < Fields.size(); ++index) {
                packet.fields.push_back(PacketField {
                    Fields[index].name,
                    PacketFieldType::Int62,
                    std::to_string(petStatuses[index]),
                });
            }
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_MP 移动宠物 Ukuy {fromindex:i62} {toindex:i62}
     * 槽位范围、有效性与宠物状态均为业务规则；基础 packet 层保留任意
     * signed int 并严格按 exact2 编码，不兼容 legacy 宽松 arity 与 0x17 异常。
     */
    struct PetMoveRequest
    {
        static constexpr std::string_view Code = "Ukuy";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("fromindex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("toindex"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int fromindex = 0;
        int toindex = 0;

        PetMoveRequest(int fromindex, int toindex)
            : fromindex(fromindex)
            , toindex(toindex)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.reserve(Fields.size());
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("fromindex"),
                PacketFieldType::Int62,
                std::to_string(fromindex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("toindex"),
                PacketFieldType::Int62,
                std::to_string(toindex),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_GFL 技能前排角色列表 iET {haveskillindex:i62}
     * 字段是持有技能槽/索引而非 SkillId；基础 packet 层保留任意 signed int，
     * 严格按 exact1 编码，不兼容 legacy 宽松 arity 与 0x17 异常。
     */
    struct SkillFrontCharListRequest
    {
        static constexpr std::string_view Code = "iET";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("haveskillindex"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int haveSkillIndex = 0;

        explicit SkillFrontCharListRequest(int haveSkillIndex)
            : haveSkillIndex(haveSkillIndex)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("haveskillindex"),
                PacketFieldType::Int62,
                std::to_string(haveSkillIndex),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_GPD 技能角色宠物列表 McVa {charSlot:i62}
     * charSlot 是临时列表槽位；基础 packet 层保留任意 signed int，
     * 严格按 exact1 编码，不下沉槽有效性与目标业务规则。
     */
    struct SkillCharacterPetListRequest
    {
        static constexpr std::string_view Code = "McVa";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("charSlot"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int charSlot = 0;

        explicit SkillCharacterPetListRequest(int charSlot)
            : charSlot(charSlot)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("charSlot"),
                PacketFieldType::Int62,
                std::to_string(charSlot),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_GPDI 物品角色宠物列表 mjCv {charSlot:i62}
     * charSlot 是临时列表槽位；基础 packet 层保留任意 signed int，
     * 严格按 exact1 编码，不下沉槽有效性与目标业务规则。
     */
    struct ItemCharacterPetListRequest
    {
        static constexpr std::string_view Code = "mjCv";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 1U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("charSlot"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int charSlot = 0;

        explicit ItemCharacterPetListRequest(int charSlot)
            : charSlot(charSlot)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("charSlot"),
                PacketFieldType::Int62,
                std::to_string(charSlot),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_IH 盖章 BG {targetItemSlot:i62} {stampItemSlot:i62}
     * 基础 packet 层保留两个槽位的任意 signed int，严格按目标槽、印章槽
     * exact2 编码，不下沉槽范围与物品业务规则，不兼容 legacy 宽松 arity 与 0x17 异常。
     */
    struct ItemStampRequest
    {
        static constexpr std::string_view Code = "BG";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("targetItemSlot"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("stampItemSlot"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int targetItemSlot = 0;
        int stampItemSlot = 0;

        ItemStampRequest(int targetItemSlot, int stampItemSlot)
            : targetItemSlot(targetItemSlot)
            , stampItemSlot(stampItemSlot)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("targetItemSlot"),
                PacketFieldType::Int62,
                std::to_string(targetItemSlot),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("stampItemSlot"),
                PacketFieldType::Int62,
                std::to_string(stampItemSlot),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_AC 角色动作 MIS {x:i62} {y:i62} {action:i62}
     * 基础 packet 层保留坐标与动作的任意 signed int，严格按 x、y、action
     * exact3 编码，不下沉坐标、XYD 与动作范围业务规则，不兼容 legacy 宽松 arity 与 0x17 异常。
     */
    struct CharacterActionRequest
    {
        static constexpr std::string_view Code = "MIS";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("action"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;
        int action = 0;

        CharacterActionRequest(int x, int y, int action)
            : x(x)
            , y(y)
            , action(action)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("action"),
                PacketFieldType::Int62,
                std::to_string(action),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_ACS 生产技能图标 klF {x:i62} {y:i62} {icon:i62}
     * 基础 packet 层保留坐标与图标的任意 signed int，严格按 x、y、icon
     * exact3 编码，不下沉坐标、技能图标映射与范围业务规则，不兼容 legacy 宽松 arity 与 0x17 异常。
     */
    struct ActionSkillIconRequest
    {
        static constexpr std::string_view Code = "klF";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("x"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("y"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("icon"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int x = 0;
        int y = 0;
        int icon = 0;

        ActionSkillIconRequest(int x, int y, int icon)
            : x(x)
            , y(y)
            , icon(icon)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("x"),
                PacketFieldType::Int62,
                std::to_string(x),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("y"),
                PacketFieldType::Int62,
                std::to_string(y),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("icon"),
                PacketFieldType::Int62,
                std::to_string(icon),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_TU 使用技能 sM
     * {haveSkillIndex:i62} {haveTechIndex:i62} {toIndex:i62} {data:es}
     * data 是由 handler 区分语义的 opaque legacy bytes；基础 packet 层不猜测 variant、
     * 不按竖线拆分，也不下沉技能槽、目标与配方/材料/宝石等业务规则。
     */
    struct TechUseRequest
    {
        static constexpr std::string_view Code = "sM";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 4U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("haveSkillIndex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("haveTechIndex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("toIndex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("data"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int haveSkillIndex = 0;
        int haveTechIndex = 0;
        int toIndex = 0;
        std::string data;

        TechUseRequest(
            int haveSkillIndex,
            int haveTechIndex,
            int toIndex,
            std::string data)
            : haveSkillIndex(haveSkillIndex)
            , haveTechIndex(haveTechIndex)
            , toIndex(toIndex)
            , data(std::move(data))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("haveSkillIndex"),
                PacketFieldType::Int62,
                std::to_string(haveSkillIndex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("haveTechIndex"),
                PacketFieldType::Int62,
                std::to_string(haveTechIndex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("toIndex"),
                PacketFieldType::Int62,
                std::to_string(toIndex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("data"),
                PacketFieldType::EscapedString,
                data,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_GML 家族邮件 uId {toregistNumber:i62} {text:es} {color:i62}
     */
    struct GuildMailSend
    {
        static constexpr std::string_view Code = "uId";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("toregistNumber"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("text"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("color"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("toregistNumber"),
                PacketFieldType::Int62,
                std::to_string(toregistNumber),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("text"),
                PacketFieldType::EscapedString,
                text,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("color"),
                PacketFieldType::Int62,
                std::to_string(color),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
        }
    };

    /**
     * RECV_PRE 删除迷你邮件好友 wD {profileIndex:i62} {name:es}
     */
    struct ProfileMailFriendDelete
    {
        static constexpr std::string_view Code = "wD";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("profileIndex"), PacketFieldType::Int62},
            {CG_PACKET_DIAGNOSTIC_TEXT("name"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

        int profileIndex = 0;
        std::string name;

        ProfileMailFriendDelete(int profileIndex, std::string name)
            : profileIndex(profileIndex),
              name(std::move(name))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("profileIndex"),
                PacketFieldType::Int62,
                std::to_string(profileIndex),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("name"),
                PacketFieldType::EscapedString,
                name,
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
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
        static constexpr std::array<PacketFieldSpec, 2U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("data"), PacketFieldType::SubPayload},
            {CG_PACKET_DIAGNOSTIC_TEXT("color"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };
        static constexpr std::string_view PayloadDelimiter = "|";

    private:
#if CG_PACKET_ENABLE_TEST_HELPERS
        struct ProfileMiniMailPayload
        {
            int profileIndex = 0;
            std::string name;
            std::string message;
        };
#endif

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

            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("data"),
                PacketFieldType::EscapedString,
                data.Value(),
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("color"),
                PacketFieldType::Int62,
                std::to_string(color),
            });
            return PacketResult<Packet>::Success(std::move(packet));
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            PacketResult<Packet> packet = ToPacket();
            if (!packet.HasValue()) {
                return PacketResult<std::string>::Failure(packet.Error());
            }
            return BuildPacket(packet.Value(), Spec);
        }

#if CG_PACKET_ENABLE_TEST_HELPERS
        static PacketResult<ProfileMiniMailSend> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view, Spec);
            if (!parsed.HasValue()) {
                return PacketResult<ProfileMiniMailSend>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<ProfileMiniMailSend> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend Parse 收到非 GH 封包"));
                error.packetCode = packet.code;
                return PacketResult<ProfileMiniMailSend>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ProfileMiniMailSend>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend Parse 封包方向不匹配")));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ProfileMiniMailSend>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend Parse 字段数量不匹配"),
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (!CG_PACKET_FIELD_NAME_MATCHES(dataField.name, CG_PACKET_DIAGNOSTIC_TEXT("data"))) {
                return PacketResult<ProfileMiniMailSend>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend Parse data 字段名不匹配"),
                    DataFieldIndex));
            }

            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<ProfileMiniMailSend>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend Parse data 字段类型不匹配"),
                    DataFieldIndex));
            }

            const PacketField& colorField = packet.fields[ColorFieldIndex];
            if (!CG_PACKET_FIELD_NAME_MATCHES(colorField.name, CG_PACKET_DIAGNOSTIC_TEXT("color"))) {
                return PacketResult<ProfileMiniMailSend>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend Parse color 字段名不匹配"),
                    ColorFieldIndex));
            }

            if (colorField.type != PacketFieldType::Int62) {
                return PacketResult<ProfileMiniMailSend>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend Parse color 字段类型不匹配"),
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
#endif

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t ColorFieldIndex = 1U;
        static constexpr std::size_t PayloadPartCount = 4U;
        static constexpr std::size_t ProfileIndexPayloadIndex = 0U;
        static constexpr std::size_t NamePayloadIndex = 1U;
        static constexpr std::size_t MessagePayloadIndex = 2U;
        static constexpr std::size_t TailPayloadIndex = 3U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string_view message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            return error;
        }

        [[nodiscard]] static PacketError MakeFieldError(
            PacketErrorCode code,
            std::string_view message,
            std::size_t fieldIndex)
        {
            PacketError error = MakePacketError(code, std::move(message));
            error.fieldIndex = fieldIndex;
            return error;
        }

#if CG_PACKET_ENABLE_TEST_HELPERS
        [[nodiscard]] static PacketResult<ProfileMiniMailPayload> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend payload 为空"),
                    DataFieldIndex));
            }

            if (payload.back() != '|') {
                return PacketResult<ProfileMiniMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend payload 缺少尾随分隔符"),
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                return PacketResult<ProfileMiniMailPayload>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != PayloadPartCount || !parts[TailPayloadIndex].empty()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend payload 段数量不匹配"),
                    DataFieldIndex));
            }

            if (parts[ProfileIndexPayloadIndex].empty()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend profileIndex 为空"),
                    ProfileIndexPayloadIndex));
            }

            if (parts[NamePayloadIndex].empty()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend name 为空"),
                    NamePayloadIndex));
            }

            if (parts[MessagePayloadIndex].empty()) {
                return PacketResult<ProfileMiniMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend message 为空"),
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
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend raw 字符串包含非法分隔符"),
                    fieldIndex));
            }

            for (std::size_t index = 0; index < text.size(); ++index) {
                if (!cg::packet::detail::IsLegacyDoubleByteLead(text[index])) {
                    continue;
                }

                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend raw 字符串旧双字节字符被截断"),
                        fieldIndex));
                }
                ++index;
            }

            return PacketResult<std::string>::Success(std::string(text));
        }
#endif

        [[nodiscard]] static PacketResult<std::string> ValidateBuildStringField(
            std::string_view text,
            std::size_t fieldIndex)
        {
            if (text.empty()) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    PacketErrorCode::EncodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend BuildData raw 字符串为空"),
                    fieldIndex));
            }

            if (text.find('|') != std::string_view::npos || text.find("\\z") != std::string_view::npos) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    PacketErrorCode::EncodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend BuildData raw 字符串包含非法分隔符"),
                    fieldIndex));
            }

            for (std::size_t index = 0; index < text.size(); ++index) {
                if (!cg::packet::detail::IsLegacyDoubleByteLead(text[index])) {
                    continue;
                }

                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::EncodeFailed,
                        CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend BuildData raw 字符串旧双字节字符被截断"),
                        fieldIndex));
                }
                ++index;
            }

            return PacketResult<std::string>::Success(std::string(text));
        }

#if CG_PACKET_ENABLE_TEST_HELPERS
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
                    CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend 十进制字段为空"),
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
                        CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend 十进制字段包含非法字符"),
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        CG_PACKET_DIAGNOSTIC_TEXT("ProfileMiniMailSend 十进制字段超出 int 范围"),
                        fieldIndex));
                }

                value = (value * 10) + digit;
            }

            if (negative) {
                value = -value;
            }

            return PacketResult<int>::Success(static_cast<int>(value));
        }
#endif
    };

    /**
     * RECV_TROP CHAR_Trade_setTradeList 取出物品 aiAb {item:es} {pet:es} {gold:i62}
     * item/pet 为普通外层 es；内部 `|` 保持 opaque，不在此处拆解。
     */
    struct TradeTakeOut
    {
        static constexpr std::string_view Code = "aiAb";
        static constexpr PacketDirection Direction = PacketDirection::ClientToServer;
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("item"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("pet"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("gold"), PacketFieldType::Int62},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };

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
            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("item"),
                PacketFieldType::EscapedString,
                item,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("pet"),
                PacketFieldType::EscapedString,
                pet,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("gold"),
                PacketFieldType::Int62,
                std::to_string(gold),
            });
            return packet;
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            return BuildPacket(ToPacket(), Spec);
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
        static constexpr std::array<PacketFieldSpec, 3U> Fields = {{
            {CG_PACKET_DIAGNOSTIC_TEXT("stallName"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("stallDescription"), PacketFieldType::EscapedString},
            {CG_PACKET_DIAGNOSTIC_TEXT("stallList"), PacketFieldType::EscapedString},
        }};
        static constexpr PacketSpec Spec {
            Code,
            Direction,
            Fields.data(),
            Fields.size(),
        };
        static constexpr std::string_view PayloadDelimiter = "|";
        static constexpr std::size_t ItemPriceCount = 20U;
        static constexpr std::size_t PetPriceCount = 5U;
        static constexpr std::size_t PriceCount = ItemPriceCount + PetPriceCount;

        std::string stallName;
        std::string stallDescription;
        std::array<int, PriceCount> prices;

        static std::array<int, PriceCount> MakeZeroPrices()
        {
            std::array<int, PriceCount> values{};
            for (int& value : values) {
                value = 0;
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
            std::array<int, PriceCount> prices)
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
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart BuildData price 非 raw decimal"));
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

            Packet packet(PacketCode(Code), Direction);
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("stallName"),
                PacketFieldType::EscapedString,
                stallName,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("stallDescription"),
                PacketFieldType::EscapedString,
                stallDescription,
            });
            packet.fields.push_back(PacketField {
                CG_PACKET_DIAGNOSTIC_TEXT("stallList"),
                PacketFieldType::EscapedString,
                stallList.Value(),
            });
            return PacketResult<Packet>::Success(std::move(packet));
        }

        [[nodiscard]] PacketResult<std::string> Encode() const
        {
            PacketResult<Packet> packet = ToPacket();
            if (!packet.HasValue()) {
                return PacketResult<std::string>::Failure(packet.Error());
            }
            return BuildPacket(packet.Value(), Spec);
        }

#if CG_PACKET_ENABLE_TEST_HELPERS
        static PacketResult<StallStart> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view, Spec);
            if (!parsed.HasValue()) {
                return PacketResult<StallStart>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<StallStart> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, CG_PACKET_DIAGNOSTIC_TEXT("StallStart Parse 收到非 ylGN 封包"));
                error.packetCode = packet.code;
                return PacketResult<StallStart>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<StallStart>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart Parse 封包方向不匹配")));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart Parse 字段数量不匹配"),
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            if (!CG_PACKET_FIELD_NAME_MATCHES(packet.fields[StallNameFieldIndex].name, CG_PACKET_DIAGNOSTIC_TEXT("stallName"))) {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart Parse stallName 字段名不匹配"),
                    StallNameFieldIndex));
            }

            if (packet.fields[StallNameFieldIndex].type != PacketFieldType::EscapedString) {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart Parse stallName 字段类型不匹配"),
                    StallNameFieldIndex));
            }

            if (!CG_PACKET_FIELD_NAME_MATCHES(packet.fields[StallDescriptionFieldIndex].name, CG_PACKET_DIAGNOSTIC_TEXT("stallDescription"))) {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart Parse stallDescription 字段名不匹配"),
                    StallDescriptionFieldIndex));
            }

            if (packet.fields[StallDescriptionFieldIndex].type != PacketFieldType::EscapedString) {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart Parse stallDescription 字段类型不匹配"),
                    StallDescriptionFieldIndex));
            }

            const PacketField& stallListField = packet.fields[StallListFieldIndex];
            if (!CG_PACKET_FIELD_NAME_MATCHES(stallListField.name, CG_PACKET_DIAGNOSTIC_TEXT("stallList"))) {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart Parse stallList 字段名不匹配"),
                    StallListFieldIndex));
            }

            if (stallListField.type != PacketFieldType::EscapedString) {
                return PacketResult<StallStart>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart Parse stallList 字段类型不匹配"),
                    StallListFieldIndex));
            }

            PacketResult<std::array<int, PriceCount>> parsedPrices = ParseStallList(stallListField.value);
            if (!parsedPrices.HasValue()) {
                return PacketResult<StallStart>::Failure(parsedPrices.Error());
            }

            return PacketResult<StallStart>::Success(StallStart(
                packet.fields[StallNameFieldIndex].value,
                packet.fields[StallDescriptionFieldIndex].value,
                std::move(parsedPrices.Value())));
        }
#endif

    private:
        static constexpr std::size_t FieldCount = 3U;
        static constexpr std::size_t StallNameFieldIndex = 0U;
        static constexpr std::size_t StallDescriptionFieldIndex = 1U;
        static constexpr std::size_t StallListFieldIndex = 2U;
        static constexpr std::size_t PayloadPartCount = PriceCount + 1U;
        static constexpr std::size_t TailPayloadIndex = PriceCount;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string_view message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            return error;
        }

        [[nodiscard]] static PacketError MakeFieldError(
            PacketErrorCode code,
            std::string_view message,
            std::size_t fieldIndex)
        {
            PacketError error = MakePacketError(code, std::move(message));
            error.fieldIndex = fieldIndex;
            return error;
        }

#if CG_PACKET_ENABLE_TEST_HELPERS
        [[nodiscard]] static PacketResult<std::array<int, PriceCount>> ParseStallList(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<std::array<int, PriceCount>>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart stallList 为空"),
                    StallListFieldIndex));
            }

            if (payload.back() != '|') {
                return PacketResult<std::array<int, PriceCount>>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart stallList 缺少尾随分隔符"),
                    StallListFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = StallListFieldIndex;
                return PacketResult<std::array<int, PriceCount>>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != PayloadPartCount || !parts[TailPayloadIndex].empty()) {
                return PacketResult<std::array<int, PriceCount>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart stallList price 段数量不匹配"),
                    StallListFieldIndex));
            }

            std::array<int, PriceCount> parsedPrices;
            for (std::size_t index = 0; index < PriceCount; ++index) {
                PacketResult<int> price = ParseRawDecimalPrice(
                    parts[index],
                    index,
                    PacketErrorCode::DecodeFailed,
                    CG_PACKET_DIAGNOSTIC_TEXT("StallStart stallList price 非 raw decimal"));
                if (!price.HasValue()) {
                    return PacketResult<std::array<int, PriceCount>>::Failure(price.Error());
                }

                parsedPrices[index] = price.Value();
            }

            return PacketResult<std::array<int, PriceCount>>::Success(std::move(parsedPrices));
        }
#endif

        [[nodiscard]] static PacketResult<std::string> ValidateRawDecimalPrice(
            int price,
            std::size_t fieldIndex,
            PacketErrorCode code,
            std::string message)
        {
            if (price < 0) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    code,
                    std::move(message),
                    fieldIndex));
            }

            return PacketResult<std::string>::Success(std::to_string(price));
        }

#if CG_PACKET_ENABLE_TEST_HELPERS
        [[nodiscard]] static PacketResult<int> ParseRawDecimalPrice(
            std::string_view text,
            std::size_t fieldIndex,
            PacketErrorCode code,
            std::string message)
        {
            if (text.empty()) {
                return PacketResult<int>::Failure(MakeFieldError(
                    code,
                    std::move(message),
                    fieldIndex));
            }

            int value = 0;
            for (const char ch : text) {
                if (ch < '0' || ch > '9') {
                    return PacketResult<int>::Failure(MakeFieldError(
                        code,
                        std::move(message),
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (std::numeric_limits<int>::max() - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        code,
                        std::move(message),
                        fieldIndex));
                }

                value = (value * 10) + digit;
            }

            return PacketResult<int>::Success(value);
        }
#endif
    };
}

#endif //CG_ENGINE_SEND_H
