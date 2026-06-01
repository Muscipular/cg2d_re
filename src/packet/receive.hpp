#ifndef CG_ENGINE_RECEIVE_H
#define CG_ENGINE_RECEIVE_H
#include "PacketParser.hpp"
#include "PacketTypes.hpp"
#include "PayloadCodec.hpp"

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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

    struct ObjectListRecord
    {
        std::vector<std::string> fields;
        int objectIndex = 0;

        ObjectListRecord(std::vector<std::string> fields, int objectIndex)
            : fields(std::move(fields)),
              objectIndex(objectIndex)
        {
        }
    };

    /**
     * SEND_C ObjectList		nsY {data:es}
     */
    struct ObjectList
    {
        static constexpr std::string_view Code = "nsY";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view RecordDelimiter = "\\t";
        static constexpr std::string_view FieldDelimiter = "|";

        std::string data;
        std::vector<ObjectListRecord> records;

        ObjectList(std::string data, std::vector<ObjectListRecord> records)
            : data(std::move(data)),
              records(std::move(records))
        {
        }

        explicit ObjectList(std::vector<ObjectListRecord> records)
            : records(std::move(records))
        {
            data = BuildData();
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::vector<std::string> recordPayloads;
            recordPayloads.reserve(records.size());
            for (const ObjectListRecord& record : records) {
                PacketResult<std::string> payload = BuildPayload(
                    std::span<const std::string>(record.fields.data(), record.fields.size()),
                    FieldDelimiter);
                if (payload.HasValue()) {
                    recordPayloads.push_back(payload.Value());
                }
            }

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(recordPayloads.data(), recordPayloads.size()),
                RecordDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }

            return {};
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::Xlsx,
            });
            return packet;
        }

        static PacketResult<ObjectList> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ObjectList>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = PacketError::Make(PacketErrorCode::InvalidCode, "ObjectList Parse 收到非 nsY 封包");
                error.packetCode = packet.code;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<ObjectList>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                PacketError error = PacketError::Make(PacketErrorCode::DecodeFailed, "ObjectList Parse 封包方向不匹配");
                error.packetCode = PacketCode(Code);
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<ObjectList>::Failure(std::move(error));
            }

            if (packet.fields.size() != 1U) {
                PacketError error = PacketError::Make(PacketErrorCode::FieldCountMismatch, "ObjectList Parse 字段数量不匹配");
                error.packetCode = PacketCode(Code);
                error.fieldIndex = packet.fields.empty() ? 0U : 1U;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<ObjectList>::Failure(std::move(error));
            }

            const PacketField& field = packet.fields.front();
            if (field.name != "data") {
                PacketError error = PacketError::Make(PacketErrorCode::DecodeFailed, "ObjectList Parse 字段名不匹配");
                error.packetCode = PacketCode(Code);
                error.fieldIndex = 0U;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<ObjectList>::Failure(std::move(error));
            }

            if (field.type != PacketFieldType::EscapedString) {
                PacketError error = PacketError::Make(PacketErrorCode::InvalidFieldType, "ObjectList Parse 字段类型不匹配");
                error.packetCode = PacketCode(Code);
                error.fieldIndex = 0U;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<ObjectList>::Failure(std::move(error));
            }

            PacketResult<std::vector<ObjectListRecord>> records = ParseRecords(field.value);
            if (!records.HasValue()) {
                return PacketResult<ObjectList>::Failure(records.Error());
            }

            return PacketResult<ObjectList>::Success(ObjectList(field.value, records.Value()));
        }

    private:
        [[nodiscard]] static bool IsKnownRecordFieldCount(std::size_t fieldCount) noexcept
        {
            return fieldCount == 4U || fieldCount == 7U || fieldCount >= 16U;
        }

        [[nodiscard]] static std::size_t ObjectIndexFieldIndex(std::size_t fieldCount) noexcept
        {
            return fieldCount >= 16U ? 1U : 0U;
        }

        [[nodiscard]] static PacketError MakePayloadError(
            PacketErrorCode code,
            std::string message,
            std::size_t recordIndex)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.fieldIndex = recordIndex;
            error.source = PacketSourceTag::Xlsx;
            return error;
        }

        [[nodiscard]] static PacketResult<std::vector<ObjectListRecord>> ParseRecords(std::string_view payload)
        {
            PacketResult<std::vector<std::string>> recordPayloads = SplitPayload(payload, RecordDelimiter);
            if (!recordPayloads.HasValue()) {
                return PacketResult<std::vector<ObjectListRecord>>::Failure(recordPayloads.Error());
            }

            std::vector<ObjectListRecord> parsedRecords;
            parsedRecords.reserve(recordPayloads.Value().size());
            for (std::size_t recordIndex = 0; recordIndex < recordPayloads.Value().size(); ++recordIndex) {
                PacketResult<std::vector<std::string>> fields = SplitPayload(
                    recordPayloads.Value()[recordIndex],
                    FieldDelimiter);
                if (!fields.HasValue()) {
                    return PacketResult<std::vector<ObjectListRecord>>::Failure(fields.Error());
                }

                if (!IsKnownRecordFieldCount(fields.Value().size())) {
                    return PacketResult<std::vector<ObjectListRecord>>::Failure(MakePayloadError(
                        PacketErrorCode::FieldCountMismatch,
                        "ObjectList record 字段数量不匹配",
                        recordIndex));
                }

                const std::size_t objectIndexFieldIndex = ObjectIndexFieldIndex(fields.Value().size());
                PacketResult<int> objectIndex = DecodeInt62(fields.Value()[objectIndexFieldIndex]);
                if (!objectIndex.HasValue()) {
                    PacketError error = objectIndex.Error();
                    error.packetCode = PacketCode(Code);
                    error.fieldIndex = recordIndex;
                    error.source = PacketSourceTag::Xlsx;
                    return PacketResult<std::vector<ObjectListRecord>>::Failure(std::move(error));
                }

                parsedRecords.emplace_back(fields.Value(), objectIndex.Value());
            }

            return PacketResult<std::vector<ObjectListRecord>>::Success(std::move(parsedRecords));
        }
    };
}

#endif //CG_ENGINE_RECEIVE_H
