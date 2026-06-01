#ifndef CG_ENGINE_RECEIVE_H
#define CG_ENGINE_RECEIVE_H
#include "PacketParser.hpp"
#include "PacketTypes.hpp"
#include "PayloadCodec.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
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
     * SEND_PRV 简介版本内容		HNP {data:es}
     * data: profileVersion|aboutVersion|mailCharge|seekPos，均为十进制 int，不使用 i62。
     */
    struct ProfileVersion
    {
        static constexpr std::string_view Code = "HNP";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int profileVersion = 0;
        int aboutVersion = 0;
        int mailCharge = 0;
        int seekPos = 0;

        ProfileVersion(int profileVersion, int aboutVersion, int mailCharge, int seekPos)
            : profileVersion(profileVersion),
              aboutVersion(aboutVersion),
              mailCharge(mailCharge),
              seekPos(seekPos)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            const std::array<std::string, PayloadFieldCount> values = {
                std::to_string(profileVersion),
                std::to_string(aboutVersion),
                std::to_string(mailCharge),
                std::to_string(seekPos),
            };

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(values.data(), values.size()),
                PayloadDelimiter);

            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                payload.HasValue() ? payload.Value() : std::string(),
                PacketSourceTag::Xlsx,
            });
            return packet;
        }

        static PacketResult<ProfileVersion> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ProfileVersion>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ProfileVersion Parse 收到非 HNP 封包");
                error.packetCode = packet.code;
                return PacketResult<ProfileVersion>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ProfileVersion>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileVersion Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ProfileVersion>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileVersion Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<ProfileVersion>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileVersion Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<ProfileVersion>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ProfileVersion Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::array<int, PayloadFieldCount>> values = ParsePayload(field.value);
            if (!values.HasValue()) {
                return PacketResult<ProfileVersion>::Failure(values.Error());
            }

            return PacketResult<ProfileVersion>::Success(ProfileVersion(
                values.Value()[0],
                values.Value()[1],
                values.Value()[2],
                values.Value()[3]));
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t PayloadFieldCount = 4U;

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

        [[nodiscard]] static PacketResult<std::array<int, PayloadFieldCount>> ParsePayload(std::string_view payload)
        {
            PacketResult<std::vector<std::string>> parts = SplitPayload(payload, PayloadDelimiter);
            if (!parts.HasValue()) {
                PacketError error = parts.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<std::array<int, PayloadFieldCount>>::Failure(std::move(error));
            }

            if (parts.Value().size() != PayloadFieldCount) {
                return PacketResult<std::array<int, PayloadFieldCount>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileVersion payload 段数量不匹配",
                    DataFieldIndex));
            }

            std::array<int, PayloadFieldCount> values = {};
            for (std::size_t index = 0; index < PayloadFieldCount; ++index) {
                PacketResult<int> value = ParseDecimalField(parts.Value()[index], index);
                if (!value.HasValue()) {
                    return PacketResult<std::array<int, PayloadFieldCount>>::Failure(value.Error());
                }
                values[index] = value.Value();
            }

            return PacketResult<std::array<int, PayloadFieldCount>>::Success(values);
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
                    "ProfileVersion payload 十进制字段为空",
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
                        "ProfileVersion payload 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileVersion payload 十进制字段超出 int 范围",
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
     * SEND_CP CharData_1		aPc {data:es}
     * data: flags|value...|，flags 为 i62；后续字段按 flag bit 顺序保留 raw 字符串。
     */
    struct CharacterProfile
    {
        static constexpr std::string_view Code = "aPc";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int flags = 0;
        std::vector<std::string> values;

        CharacterProfile(int flags, std::vector<std::string> values)
            : flags(flags),
              values(std::move(values))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            PacketResult<std::string> encodedFlags = EncodeInt62(flags);
            std::string payload = encodedFlags.HasValue() ? encodedFlags.Value() : std::string();
            payload.push_back('|');
            for (const std::string& value : values) {
                payload.append(value);
                payload.push_back('|');
            }
            return payload;
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

        static PacketResult<CharacterProfile> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<CharacterProfile>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "CharacterProfile Parse 收到非 aPc 封包");
                error.packetCode = packet.code;
                return PacketResult<CharacterProfile>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<CharacterProfile>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "CharacterProfile Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<CharacterProfile>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "CharacterProfile Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<CharacterProfile>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "CharacterProfile Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<CharacterProfile>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "CharacterProfile Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;

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

        [[nodiscard]] static PacketResult<CharacterProfile> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<CharacterProfile>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "CharacterProfile payload 为空",
                    DataFieldIndex));
            }

            if (payload.back() != '|') {
                return PacketResult<CharacterProfile>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "CharacterProfile payload 缺少尾随分隔符",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> parts = SplitPayload(payload, PayloadDelimiter);
            if (!parts.HasValue()) {
                PacketError error = parts.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<CharacterProfile>::Failure(std::move(error));
            }

            std::vector<std::string> payloadParts = std::move(parts.Value());
            payloadParts.pop_back();
            if (payloadParts.empty() || payloadParts.front().empty()) {
                return PacketResult<CharacterProfile>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "CharacterProfile payload flags 为空",
                    DataFieldIndex));
            }

            PacketResult<int> decodedFlags = DecodeInt62(payloadParts.front());
            if (!decodedFlags.HasValue()) {
                PacketError error = decodedFlags.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<CharacterProfile>::Failure(std::move(error));
            }

            std::vector<std::string> rawValues;
            rawValues.reserve(payloadParts.size() - 1U);
            for (std::size_t index = 1U; index < payloadParts.size(); ++index) {
                rawValues.push_back(std::move(payloadParts[index]));
            }

            return PacketResult<CharacterProfile>::Success(CharacterProfile(decodedFlags.Value(), std::move(rawValues)));
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

    struct CharacterIconEntry
    {
        std::vector<std::string> fields;
        int objectIndex = 0;

        CharacterIconEntry(std::vector<std::string> fields, int objectIndex)
            : fields(std::move(fields)),
              objectIndex(objectIndex)
        {
        }
    };

    /**
     * SEND_CA 发送头顶图标		NI {data:es}
     */
    struct CharacterIcon
    {
        static constexpr std::string_view Code = "NI";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view EntryDelimiter = ",";
        static constexpr std::string_view FieldDelimiter = "|";

        std::string data;
        std::vector<CharacterIconEntry> entries;

        CharacterIcon(std::string data, std::vector<CharacterIconEntry> entries)
            : data(std::move(data)),
              entries(std::move(entries))
        {
        }

        explicit CharacterIcon(std::vector<CharacterIconEntry> entries)
            : entries(std::move(entries))
        {
            data = BuildData();
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::vector<std::string> entryPayloads;
            entryPayloads.reserve(entries.size());
            for (const CharacterIconEntry& entry : entries) {
                PacketResult<std::string> payload = BuildPayload(
                    std::span<const std::string>(entry.fields.data(), entry.fields.size()),
                    FieldDelimiter);
                if (payload.HasValue()) {
                    entryPayloads.push_back(payload.Value());
                }
            }

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(entryPayloads.data(), entryPayloads.size()),
                EntryDelimiter);
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

        static PacketResult<CharacterIcon> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<CharacterIcon>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = PacketError::Make(PacketErrorCode::InvalidCode, "CharacterIcon Parse 收到非 NI 封包");
                error.packetCode = packet.code;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<CharacterIcon>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                PacketError error = PacketError::Make(PacketErrorCode::DecodeFailed, "CharacterIcon Parse 封包方向不匹配");
                error.packetCode = PacketCode(Code);
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<CharacterIcon>::Failure(std::move(error));
            }

            if (packet.fields.size() != 1U) {
                PacketError error = PacketError::Make(PacketErrorCode::FieldCountMismatch, "CharacterIcon Parse 字段数量不匹配");
                error.packetCode = PacketCode(Code);
                error.fieldIndex = packet.fields.empty() ? 0U : 1U;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<CharacterIcon>::Failure(std::move(error));
            }

            const PacketField& field = packet.fields.front();
            if (field.name != "data") {
                PacketError error = PacketError::Make(PacketErrorCode::DecodeFailed, "CharacterIcon Parse 字段名不匹配");
                error.packetCode = PacketCode(Code);
                error.fieldIndex = 0U;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<CharacterIcon>::Failure(std::move(error));
            }

            if (field.type != PacketFieldType::EscapedString) {
                PacketError error = PacketError::Make(PacketErrorCode::InvalidFieldType, "CharacterIcon Parse 字段类型不匹配");
                error.packetCode = PacketCode(Code);
                error.fieldIndex = 0U;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<CharacterIcon>::Failure(std::move(error));
            }

            PacketResult<std::vector<CharacterIconEntry>> entries = ParseEntries(field.value);
            if (!entries.HasValue()) {
                return PacketResult<CharacterIcon>::Failure(entries.Error());
            }

            return PacketResult<CharacterIcon>::Success(CharacterIcon(field.value, entries.Value()));
        }

    private:
        [[nodiscard]] static bool IsKnownEntryFieldCount(std::size_t fieldCount) noexcept
        {
            return fieldCount == 5U || fieldCount == 6U || fieldCount == 8U || fieldCount == 12U;
        }

        [[nodiscard]] static PacketError MakePayloadError(
            PacketErrorCode code,
            std::string message,
            std::size_t entryIndex)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.fieldIndex = entryIndex;
            error.source = PacketSourceTag::Xlsx;
            return error;
        }

        [[nodiscard]] static PacketResult<std::vector<CharacterIconEntry>> ParseEntries(std::string_view payload)
        {
            PacketResult<std::vector<std::string>> entryPayloads = SplitPayload(payload, EntryDelimiter);
            if (!entryPayloads.HasValue()) {
                return PacketResult<std::vector<CharacterIconEntry>>::Failure(entryPayloads.Error());
            }

            std::vector<CharacterIconEntry> parsedEntries;
            parsedEntries.reserve(entryPayloads.Value().size());
            for (std::size_t entryIndex = 0; entryIndex < entryPayloads.Value().size(); ++entryIndex) {
                PacketResult<std::vector<std::string>> fields = SplitPayload(
                    entryPayloads.Value()[entryIndex],
                    FieldDelimiter);
                if (!fields.HasValue()) {
                    return PacketResult<std::vector<CharacterIconEntry>>::Failure(fields.Error());
                }

                if (!IsKnownEntryFieldCount(fields.Value().size())) {
                    return PacketResult<std::vector<CharacterIconEntry>>::Failure(MakePayloadError(
                        PacketErrorCode::FieldCountMismatch,
                        "CharacterIcon payload entry 字段数量不匹配",
                        entryIndex));
                }

                PacketResult<int> objectIndex = DecodeInt62(fields.Value()[0]);
                if (!objectIndex.HasValue()) {
                    PacketError error = objectIndex.Error();
                    error.packetCode = PacketCode(Code);
                    error.fieldIndex = entryIndex;
                    error.source = PacketSourceTag::Xlsx;
                    return PacketResult<std::vector<CharacterIconEntry>>::Failure(std::move(error));
                }

                parsedEntries.emplace_back(fields.Value(), objectIndex.Value());
            }

            return PacketResult<std::vector<CharacterIconEntry>>::Success(std::move(parsedEntries));
        }
    };

    /**
     * SEND_CD 发送删除ObjectIndex列表		uSB {data:es}
     */
    struct ObjectDelete
    {
        static constexpr std::string_view Code = "uSB";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view IndexDelimiter = ",";

        std::vector<int> objectIndexes;

        explicit ObjectDelete(std::vector<int> objectIndexes)
            : objectIndexes(std::move(objectIndexes))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::vector<std::string> values;
            values.reserve(objectIndexes.size());
            for (const int objectIndex : objectIndexes) {
                PacketResult<std::string> encoded = EncodeInt62(objectIndex);
                values.push_back(encoded.HasValue() ? encoded.Value() : std::string());
            }

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(values.data(), values.size()),
                IndexDelimiter);
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

        static PacketResult<ObjectDelete> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ObjectDelete>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ObjectDelete Parse 收到非 uSB 封包");
                error.packetCode = packet.code;
                return PacketResult<ObjectDelete>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ObjectDelete>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ObjectDelete Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ObjectDelete>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ObjectDelete Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<ObjectDelete>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ObjectDelete Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<ObjectDelete>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ObjectDelete Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::vector<int>> objectIndexes = ParseIndexes(field.value);
            if (!objectIndexes.HasValue()) {
                return PacketResult<ObjectDelete>::Failure(objectIndexes.Error());
            }

            return PacketResult<ObjectDelete>::Success(ObjectDelete(std::move(objectIndexes.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;

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

        [[nodiscard]] static PacketResult<std::vector<int>> ParseIndexes(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<std::vector<int>>::Success({});
            }

            PacketResult<std::vector<std::string>> parts = SplitPayload(payload, IndexDelimiter);
            if (!parts.HasValue()) {
                PacketError error = parts.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<std::vector<int>>::Failure(std::move(error));
            }

            std::vector<int> indexes;
            indexes.reserve(parts.Value().size());
            for (std::size_t index = 0; index < parts.Value().size(); ++index) {
                if (parts.Value()[index].empty()) {
                    return PacketResult<std::vector<int>>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ObjectDelete payload objectIndex 为空",
                        index));
                }

                PacketResult<int> objectIndex = DecodeInt62(parts.Value()[index]);
                if (!objectIndex.HasValue()) {
                    PacketError error = objectIndex.Error();
                    error.packetCode = PacketCode(Code);
                    error.fieldIndex = index;
                    error.source = PacketSourceTag::Xlsx;
                    return PacketResult<std::vector<int>>::Failure(std::move(error));
                }

                indexes.push_back(objectIndex.Value());
            }

            return PacketResult<std::vector<int>>::Success(std::move(indexes));
        }
    };

    /**
     * SEND_MAP_DATA 地图数据		aBS {mapid:i62} {floor:i62} {x1:i62} {y1:i62} {x2:i62} {y2:i62} {data:es}
     */
    struct MapData
    {
        static constexpr std::string_view Code = "aBS";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view LayerDelimiter = "|";
        static constexpr std::string_view CellDelimiter = ",";

        int mapId = 0;
        int floor = 0;
        int x1 = 0;
        int y1 = 0;
        int x2 = 0;
        int y2 = 0;
        std::vector<int> tileData;
        std::vector<int> objectData;
        std::vector<int> eventDataRaw;

        MapData(
            int mapId,
            int floor,
            int x1,
            int y1,
            int x2,
            int y2,
            std::vector<int> tileData,
            std::vector<int> objectData,
            std::vector<int> eventDataRaw)
            : mapId(mapId),
              floor(floor),
              x1(x1),
              y1(y1),
              x2(x2),
              y2(y2),
              tileData(std::move(tileData)),
              objectData(std::move(objectData)),
              eventDataRaw(std::move(eventDataRaw))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::vector<std::string> layerPayloads;
            layerPayloads.reserve(3U);
            layerPayloads.push_back(BuildLayer(tileData, false));
            layerPayloads.push_back(BuildLayer(objectData, true));
            layerPayloads.push_back(BuildLayer(eventDataRaw, true));

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(layerPayloads.data(), layerPayloads.size()),
                LayerDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }

            return {};
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
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::Xlsx,
            });
            return packet;
        }

        static PacketResult<MapData> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<MapData>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "MapData Parse 收到非 aBS 封包");
                error.packetCode = packet.code;
                return PacketResult<MapData>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<MapData>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "MapData Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<MapData>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "MapData Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "mapid",
                "floor",
                "x1",
                "y1",
                "x2",
                "y2",
                "data",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<MapData>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "MapData Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<MapData>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "MapData Parse 字段类型不匹配",
                        index));
                }
            }

            std::array<int, DataFieldIndex> values = {};
            for (std::size_t index = 0; index < DataFieldIndex; ++index) {
                PacketResult<int> value = ParseDecimalField(packet.fields[index].value, index);
                if (!value.HasValue()) {
                    return PacketResult<MapData>::Failure(value.Error());
                }
                values[index] = value.Value();
            }

            if (values[4] < values[2]) {
                return PacketResult<MapData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "MapData Parse x2 小于 x1",
                    4U));
            }
            if (values[5] < values[3]) {
                return PacketResult<MapData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "MapData Parse y2 小于 y1",
                    5U));
            }

            const std::int64_t width64 = static_cast<std::int64_t>(values[4]) - static_cast<std::int64_t>(values[2]);
            const std::int64_t height64 = static_cast<std::int64_t>(values[5]) - static_cast<std::int64_t>(values[3]);
            const std::size_t width = static_cast<std::size_t>(width64);
            const std::size_t height = static_cast<std::size_t>(height64);
            if (height != 0U && width > (std::numeric_limits<std::size_t>::max() / height)) {
                return PacketResult<MapData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "MapData Parse 面积溢出",
                    DataFieldIndex));
            }
            const std::size_t area = width * height;
            const std::string& payload = packet.fields[DataFieldIndex].value;

            if (area == 0U && payload.empty()) {
                return PacketResult<MapData>::Success(MapData(
                    values[0],
                    values[1],
                    values[2],
                    values[3],
                    values[4],
                    values[5],
                    {},
                    {},
                    {}));
            }

            PacketResult<std::vector<std::string>> layers = SplitPayload(payload, LayerDelimiter);
            if (!layers.HasValue()) {
                PacketError error = layers.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<MapData>::Failure(std::move(error));
            }

            if (layers.Value().size() != 3U) {
                return PacketResult<MapData>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "MapData payload 段数量不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::vector<int>> tileLayer = ParseLayer(layers.Value()[0], area, false);
            if (!tileLayer.HasValue()) {
                return PacketResult<MapData>::Failure(tileLayer.Error());
            }

            PacketResult<std::vector<int>> objectLayer = ParseLayer(layers.Value()[1], area, true);
            if (!objectLayer.HasValue()) {
                return PacketResult<MapData>::Failure(objectLayer.Error());
            }

            PacketResult<std::vector<int>> eventLayer = ParseLayer(layers.Value()[2], area, true);
            if (!eventLayer.HasValue()) {
                return PacketResult<MapData>::Failure(eventLayer.Error());
            }

            return PacketResult<MapData>::Success(MapData(
                values[0],
                values[1],
                values[2],
                values[3],
                values[4],
                values[5],
                std::move(tileLayer.Value()),
                std::move(objectLayer.Value()),
                std::move(eventLayer.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 7U;
        static constexpr std::size_t DataFieldIndex = 6U;

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
                    "MapData Parse i62 字段为空",
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
                        "MapData Parse i62 字段包含非十进制字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "MapData Parse i62 字段超出 int 范围",
                        fieldIndex));
                }

                value = (value * 10) + digit;
            }

            if (negative) {
                value = -value;
            }

            return PacketResult<int>::Success(static_cast<int>(value));
        }

        [[nodiscard]] static PacketResult<std::vector<int>> ParseLayer(
            std::string_view layer,
            std::size_t area,
            bool emptyCellAsZero)
        {
            if (area == 0U && layer.empty()) {
                return PacketResult<std::vector<int>>::Success({});
            }

            PacketResult<std::vector<std::string>> cells = SplitPayload(layer, CellDelimiter);
            if (!cells.HasValue()) {
                PacketError error = cells.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<std::vector<int>>::Failure(std::move(error));
            }

            if (cells.Value().size() != area) {
                return PacketResult<std::vector<int>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "MapData payload 单层元素数量不匹配",
                    DataFieldIndex));
            }

            std::vector<int> values;
            values.reserve(cells.Value().size());
            for (const std::string& cell : cells.Value()) {
                if (cell.empty()) {
                    if (emptyCellAsZero) {
                        values.push_back(0);
                        continue;
                    }

                    return PacketResult<std::vector<int>>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "MapData tileData 空字段非法",
                        DataFieldIndex));
                }

                PacketResult<int> value = DecodeInt62(cell);
                if (!value.HasValue()) {
                    PacketError error = value.Error();
                    error.packetCode = PacketCode(Code);
                    error.fieldIndex = DataFieldIndex;
                    error.source = PacketSourceTag::Xlsx;
                    return PacketResult<std::vector<int>>::Failure(std::move(error));
                }

                values.push_back(value.Value());
            }

            return PacketResult<std::vector<int>>::Success(std::move(values));
        }

        [[nodiscard]] static std::string BuildLayer(const std::vector<int>& values, bool zeroAsEmpty)
        {
            std::vector<std::string> cells;
            cells.reserve(values.size());
            for (const int value : values) {
                if (zeroAsEmpty && value == 0) {
                    cells.emplace_back();
                    continue;
                }

                PacketResult<std::string> encoded = EncodeInt62(value);
                cells.push_back(encoded.HasValue() ? encoded.Value() : std::string());
            }

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(cells.data(), cells.size()),
                CellDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }

            return {};
        }
    };
}

#endif //CG_ENGINE_RECEIVE_H
