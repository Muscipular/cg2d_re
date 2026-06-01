#ifndef CG_ENGINE_RECEIVE_H
#define CG_ENGINE_RECEIVE_H
#include "PacketParser.hpp"
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

    /**
     * SEND_MN SendMapName		TeD {data:es}
     */
    struct MapName
    {
        static constexpr std::string_view Code = "TeD";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        std::string data;

        explicit MapName(std::string data)
            : data(std::move(data))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                data,
                PacketSourceTag::Xlsx,
            });
            return packet;
        }

        static PacketResult<MapName> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<MapName>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = PacketError::Make(PacketErrorCode::InvalidCode, "MapName Parse 收到非 TeD 封包");
                error.packetCode = packet.code;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<MapName>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                PacketError error = PacketError::Make(PacketErrorCode::DecodeFailed, "MapName Parse 封包方向不匹配");
                error.packetCode = PacketCode(Code);
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<MapName>::Failure(std::move(error));
            }

            if (packet.fields.size() != 1U) {
                PacketError error = PacketError::Make(PacketErrorCode::FieldCountMismatch, "MapName Parse 字段数量不匹配");
                error.packetCode = PacketCode(Code);
                error.fieldIndex = packet.fields.empty() ? 0U : 1U;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<MapName>::Failure(std::move(error));
            }

            const PacketField& field = packet.fields.front();
            if (field.name != "data") {
                PacketError error = PacketError::Make(PacketErrorCode::DecodeFailed, "MapName Parse 字段名不匹配");
                error.packetCode = PacketCode(Code);
                error.fieldIndex = 0U;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<MapName>::Failure(std::move(error));
            }

            if (field.type != PacketFieldType::EscapedString) {
                PacketError error = PacketError::Make(PacketErrorCode::InvalidFieldType, "MapName Parse 字段类型不匹配");
                error.packetCode = PacketCode(Code);
                error.fieldIndex = 0U;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<MapName>::Failure(std::move(error));
            }

            return PacketResult<MapName>::Success(MapName(field.value));
        }
    };
}

#endif //CG_ENGINE_RECEIVE_H
