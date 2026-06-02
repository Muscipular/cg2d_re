#ifndef CG_ENGINE_RECEIVE_H
#define CG_ENGINE_RECEIVE_H
#include "PacketParser.hpp"
#include "PacketTypes.hpp"
#include "PayloadCodec.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
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
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ClientLoginResult>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(
                    PacketErrorCode::InvalidCode,
                    "ClientLoginResult Parse 收到非 whV 封包");
                error.packetCode = packet.code;
                return PacketResult<ClientLoginResult>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ClientLoginResult>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ClientLoginResult Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ClientLoginResult>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ClientLoginResult Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& resultField = packet.fields[ResultFieldIndex];
            if (resultField.name != "result") {
                return PacketResult<ClientLoginResult>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ClientLoginResult Parse result 字段名不匹配",
                    ResultFieldIndex));
            }

            if (resultField.type != PacketFieldType::Int62) {
                return PacketResult<ClientLoginResult>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ClientLoginResult Parse result 字段类型不匹配",
                    ResultFieldIndex));
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (dataField.name != "data") {
                return PacketResult<ClientLoginResult>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ClientLoginResult Parse data 字段名不匹配",
                    DataFieldIndex));
            }

            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<ClientLoginResult>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ClientLoginResult Parse data 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<int> result = ParseResultValue(resultField.value);
            if (!result.HasValue()) {
                return PacketResult<ClientLoginResult>::Failure(result.Error());
            }

            return PacketResult<ClientLoginResult>::Success(ClientLoginResult(result.Value(), dataField.value));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t ResultFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ExistingPrototype;
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

        [[nodiscard]] static PacketResult<int> ParseResultValue(std::string_view text)
        {
            bool negative = false;
            std::size_t index = 0U;
            if (!text.empty() && text.front() == '-') {
                negative = true;
                index = 1U;
            }

            if (index == text.size()) {
                return PacketResult<int>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ClientLoginResult result 为空",
                    ResultFieldIndex));
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
                        "ClientLoginResult result 包含非法字符",
                        ResultFieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ClientLoginResult result 超出 int 范围",
                        ResultFieldIndex));
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
     * SEND_CP2 CharData_2		UU {data:es}
     * data: flagMask|value...|，flagMask 为 i62；后续字段按 bit 顺序保留 raw 字符串。
     */
    struct CharacterProfile2
    {
        static constexpr std::string_view Code = "UU";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int flagMask = 0;
        std::vector<std::string> values;

        CharacterProfile2(int flagMask, std::vector<std::string> values)
            : flagMask(flagMask),
              values(std::move(values))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            PacketResult<std::string> encodedFlagMask = EncodeInt62(flagMask);
            std::string payload = encodedFlagMask.HasValue() ? encodedFlagMask.Value() : std::string();
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

        static PacketResult<CharacterProfile2> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<CharacterProfile2>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "CharacterProfile2 Parse 收到非 UU 封包");
                error.packetCode = packet.code;
                return PacketResult<CharacterProfile2>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<CharacterProfile2>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "CharacterProfile2 Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<CharacterProfile2>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "CharacterProfile2 Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<CharacterProfile2>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "CharacterProfile2 Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<CharacterProfile2>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "CharacterProfile2 Parse 字段类型不匹配",
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

        [[nodiscard]] static PacketResult<CharacterProfile2> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<CharacterProfile2>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "CharacterProfile2 payload 为空",
                    DataFieldIndex));
            }

            if (payload.back() != '|') {
                return PacketResult<CharacterProfile2>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "CharacterProfile2 payload 缺少尾随分隔符",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> parts = SplitPayload(payload, PayloadDelimiter);
            if (!parts.HasValue()) {
                PacketError error = parts.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<CharacterProfile2>::Failure(std::move(error));
            }

            std::vector<std::string> payloadParts = std::move(parts.Value());
            payloadParts.pop_back();
            if (payloadParts.empty() || payloadParts.front().empty()) {
                return PacketResult<CharacterProfile2>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "CharacterProfile2 payload flagMask 为空",
                    DataFieldIndex));
            }

            PacketResult<int> decodedFlagMask = DecodeInt62(payloadParts.front());
            if (!decodedFlagMask.HasValue()) {
                PacketError error = decodedFlagMask.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<CharacterProfile2>::Failure(std::move(error));
            }

            std::vector<std::string> rawValues;
            rawValues.reserve(payloadParts.size() - 1U);
            for (std::size_t index = 1U; index < payloadParts.size(); ++index) {
                if (payloadParts[index].empty()) {
                    return PacketResult<CharacterProfile2>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "CharacterProfile2 payload value 为空",
                        DataFieldIndex));
                }

                rawValues.push_back(std::move(payloadParts[index]));
            }

            return PacketResult<CharacterProfile2>::Success(
                CharacterProfile2(decodedFlagMask.Value(), std::move(rawValues)));
        }
    };

    /**
     * SEND_TITLE 称号列表 slot：titleName|titleMakeId|titleId，空 slot 为 ||。
     */
    struct TitleSlot
    {
        std::string titleName;
        int titleMakeId = 0;
        int titleId = 0;
        bool empty = false;

        TitleSlot() = default;

        TitleSlot(std::string titleName, int titleMakeId, int titleId)
            : titleName(std::move(titleName)),
              titleMakeId(titleMakeId),
              titleId(titleId),
              empty(false)
        {
        }

        [[nodiscard]] static TitleSlot Empty()
        {
            TitleSlot slot;
            slot.empty = true;
            return slot;
        }
    };

    /**
     * SEND_TITLE 称号列表		Sc {data:es}
     * data: titleName|titleMakeId|titleId xN；titleMakeId/titleId 为 raw decimal，不使用 i62。
     */
    struct TitleList
    {
        static constexpr std::string_view Code = "Sc";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        std::vector<TitleSlot> slots;

        explicit TitleList(std::vector<TitleSlot> slots)
            : slots(std::move(slots))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::vector<std::string> values;
            values.reserve(slots.size() * SlotFieldCount);
            for (const TitleSlot& slot : slots) {
                if (slot.empty) {
                    values.emplace_back();
                    values.emplace_back();
                    values.emplace_back();
                    continue;
                }

                values.push_back(slot.titleName);
                values.push_back(std::to_string(slot.titleMakeId));
                values.push_back(std::to_string(slot.titleId));
            }

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(values.data(), values.size()),
                PayloadDelimiter);
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

        static PacketResult<TitleList> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<TitleList>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "TitleList Parse 收到非 Sc 封包");
                error.packetCode = packet.code;
                return PacketResult<TitleList>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<TitleList>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "TitleList Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<TitleList>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TitleList Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<TitleList>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "TitleList Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<TitleList>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "TitleList Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::vector<TitleSlot>> slots = ParseSlots(field.value);
            if (!slots.HasValue()) {
                return PacketResult<TitleList>::Failure(slots.Error());
            }

            return PacketResult<TitleList>::Success(TitleList(std::move(slots.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t SlotFieldCount = 3U;

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

        [[nodiscard]] static PacketResult<std::vector<TitleSlot>> ParseSlots(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<std::vector<TitleSlot>>::Success({});
            }

            PacketResult<std::vector<std::string>> parts = SplitPayload(payload, PayloadDelimiter);
            if (!parts.HasValue()) {
                PacketError error = parts.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<std::vector<TitleSlot>>::Failure(std::move(error));
            }

            if ((parts.Value().size() % SlotFieldCount) != 0U) {
                return PacketResult<std::vector<TitleSlot>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TitleList payload slot 字段数量不匹配",
                    DataFieldIndex));
            }

            std::vector<TitleSlot> slots;
            slots.reserve(parts.Value().size() / SlotFieldCount);
            for (std::size_t index = 0; index < parts.Value().size(); index += SlotFieldCount) {
                const std::string& titleName = parts.Value()[index];
                const std::string& titleMakeIdText = parts.Value()[index + 1U];
                const std::string& titleIdText = parts.Value()[index + 2U];
                if (titleName.empty() && titleMakeIdText.empty() && titleIdText.empty()) {
                    slots.push_back(TitleSlot::Empty());
                    continue;
                }

                PacketResult<int> titleMakeId = ParseDecimalField(titleMakeIdText, index + 1U);
                if (!titleMakeId.HasValue()) {
                    return PacketResult<std::vector<TitleSlot>>::Failure(titleMakeId.Error());
                }

                PacketResult<int> titleId = ParseDecimalField(titleIdText, index + 2U);
                if (!titleId.HasValue()) {
                    return PacketResult<std::vector<TitleSlot>>::Failure(titleId.Error());
                }

                slots.emplace_back(titleName, titleMakeId.Value(), titleId.Value());
            }

            return PacketResult<std::vector<TitleSlot>>::Success(std::move(slots));
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
                    "TitleList payload 十进制字段为空",
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
                        "TitleList payload 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TitleList payload 十进制字段超出 int 范围",
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
     * SEND_PRL profile list entry：itemId|graphicNo|itemName。
     */
    struct ProfileListEntry
    {
        int itemId = 0;
        int graphicNo = 0;
        std::string itemName;

        ProfileListEntry(int itemId, int graphicNo, std::string itemName)
            : itemId(itemId),
              graphicNo(graphicNo),
              itemName(std::move(itemName))
        {
        }
    };

    /**
     * SEND_PRL 收到 profile list		eZT {data:es}
     * data: version|itemId|graphicNo|itemName...，version/itemId/graphicNo 为 raw decimal，不使用 i62。
     */
    struct ProfileList
    {
        static constexpr std::string_view Code = "eZT";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int version = 0;
        std::vector<ProfileListEntry> entries;

        ProfileList(int version, std::vector<ProfileListEntry> entries)
            : version(version),
              entries(std::move(entries))
        {
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

        static PacketResult<ProfileList> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ProfileList>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ProfileList Parse 收到非 eZT 封包");
                error.packetCode = packet.code;
                return PacketResult<ProfileList>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ProfileList>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileList Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ProfileList>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileList Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<ProfileList>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileList Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<ProfileList>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ProfileList Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t EntryFieldCount = 3U;

        [[nodiscard]] std::string BuildData() const
        {
            std::vector<std::string> values;
            values.reserve(1U + (entries.size() * EntryFieldCount));
            values.push_back(std::to_string(version));
            for (const ProfileListEntry& entry : entries) {
                values.push_back(std::to_string(entry.itemId));
                values.push_back(std::to_string(entry.graphicNo));
                values.push_back(entry.itemName);
            }

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(values.data(), values.size()),
                PayloadDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }

            return {};
        }

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

        [[nodiscard]] static PacketResult<ProfileList> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<ProfileList>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "ProfileList payload 为空",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> parts = SplitPayload(payload, PayloadDelimiter);
            if (!parts.HasValue()) {
                PacketError error = parts.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<ProfileList>::Failure(std::move(error));
            }

            std::vector<std::string> payloadParts = std::move(parts.Value());
            if (((payloadParts.size() - 1U) % EntryFieldCount) != 0U) {
                return PacketResult<ProfileList>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileList payload entry 字段数量不匹配",
                    DataFieldIndex));
            }

            PacketResult<int> versionValue = ParseDecimalField(payloadParts.front(), 0U);
            if (!versionValue.HasValue()) {
                return PacketResult<ProfileList>::Failure(versionValue.Error());
            }

            std::vector<ProfileListEntry> entries;
            entries.reserve((payloadParts.size() - 1U) / EntryFieldCount);
            for (std::size_t index = 1U; index < payloadParts.size(); index += EntryFieldCount) {
                PacketResult<int> itemId = ParseDecimalField(payloadParts[index], index);
                if (!itemId.HasValue()) {
                    return PacketResult<ProfileList>::Failure(itemId.Error());
                }

                PacketResult<int> graphicNo = ParseDecimalField(payloadParts[index + 1U], index + 1U);
                if (!graphicNo.HasValue()) {
                    return PacketResult<ProfileList>::Failure(graphicNo.Error());
                }

                entries.emplace_back(itemId.Value(), graphicNo.Value(), std::move(payloadParts[index + 2U]));
            }

            return PacketResult<ProfileList>::Success(ProfileList(versionValue.Value(), std::move(entries)));
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
                    "ProfileList payload 十进制字段为空",
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
                        "ProfileList payload 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileList payload 十进制字段超出 int 范围",
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
     * SEND_PRA about list entry：itemId|graphicNo|itemName。
     */
    struct AboutListEntry
    {
        int itemId = 0;
        int graphicNo = 0;
        std::string itemName;

        AboutListEntry(int itemId, int graphicNo, std::string itemName)
            : itemId(itemId),
              graphicNo(graphicNo),
              itemName(std::move(itemName))
        {
        }
    };

    /**
     * SEND_PRA 收到 about list		IUn {data:es}
     * data: version|itemId|graphicNo|itemName...，version/itemId/graphicNo 为 raw decimal，不使用 i62。
     */
    struct AboutList
    {
        static constexpr std::string_view Code = "IUn";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int version = 0;
        std::vector<AboutListEntry> entries;

        AboutList(int version, std::vector<AboutListEntry> entries)
            : version(version),
              entries(std::move(entries))
        {
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

        static PacketResult<AboutList> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<AboutList>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "AboutList Parse 收到非 IUn 封包");
                error.packetCode = packet.code;
                return PacketResult<AboutList>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<AboutList>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "AboutList Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<AboutList>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "AboutList Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<AboutList>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "AboutList Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<AboutList>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "AboutList Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t EntryFieldCount = 3U;

        [[nodiscard]] std::string BuildData() const
        {
            std::vector<std::string> values;
            values.reserve(1U + (entries.size() * EntryFieldCount));
            values.push_back(std::to_string(version));
            for (const AboutListEntry& entry : entries) {
                values.push_back(std::to_string(entry.itemId));
                values.push_back(std::to_string(entry.graphicNo));
                values.push_back(entry.itemName);
            }

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(values.data(), values.size()),
                PayloadDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }

            return {};
        }

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

        [[nodiscard]] static PacketResult<AboutList> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<AboutList>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "AboutList payload 为空",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> parts = SplitPayload(payload, PayloadDelimiter);
            if (!parts.HasValue()) {
                PacketError error = parts.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<AboutList>::Failure(std::move(error));
            }

            std::vector<std::string> payloadParts = std::move(parts.Value());
            if (((payloadParts.size() - 1U) % EntryFieldCount) != 0U) {
                return PacketResult<AboutList>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "AboutList payload entry 字段数量不匹配",
                    DataFieldIndex));
            }

            PacketResult<int> versionValue = ParseDecimalField(payloadParts.front(), 0U);
            if (!versionValue.HasValue()) {
                return PacketResult<AboutList>::Failure(versionValue.Error());
            }

            std::vector<AboutListEntry> entries;
            entries.reserve((payloadParts.size() - 1U) / EntryFieldCount);
            for (std::size_t index = 1U; index < payloadParts.size(); index += EntryFieldCount) {
                PacketResult<int> itemId = ParseDecimalField(payloadParts[index], index);
                if (!itemId.HasValue()) {
                    return PacketResult<AboutList>::Failure(itemId.Error());
                }

                PacketResult<int> graphicNo = ParseDecimalField(payloadParts[index + 1U], index + 1U);
                if (!graphicNo.HasValue()) {
                    return PacketResult<AboutList>::Failure(graphicNo.Error());
                }

                entries.emplace_back(itemId.Value(), graphicNo.Value(), std::move(payloadParts[index + 2U]));
            }

            return PacketResult<AboutList>::Success(AboutList(versionValue.Value(), std::move(entries)));
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
                    "AboutList payload 十进制字段为空",
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
                        "AboutList payload 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "AboutList payload 十进制字段超出 int 范围",
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
     * SEND_PRD ProfileData		DpSY {name:es} {lv:i62} {job:es} {title:es} {guildName:es} {graphic:i62} {sid:i62} {smsg:es} {bid:i62} {bmsg:es} {aid:i62} {amsg:es} {pmsg:es}
     * smsg/bmsg/amsg/pmsg 按外层 es 保存，不拆二级 payload。
     */
    struct ProfileData
    {
        static constexpr std::string_view Code = "DpSY";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        std::string name;
        int level = 0;
        std::string jobName;
        std::string title;
        std::string guildName;
        int graphic = 0;
        int sellFlag = 0;
        std::string sellMessage;
        int buyFlag = 0;
        std::string buyMessage;
        int aboutFlag = 0;
        std::string aboutMessage;
        std::string profileMessage;

        ProfileData(
            std::string name,
            int level,
            std::string jobName,
            std::string title,
            std::string guildName,
            int graphic,
            int sellFlag,
            std::string sellMessage,
            int buyFlag,
            std::string buyMessage,
            int aboutFlag,
            std::string aboutMessage,
            std::string profileMessage)
            : name(std::move(name)),
              level(level),
              jobName(std::move(jobName)),
              title(std::move(title)),
              guildName(std::move(guildName)),
              graphic(graphic),
              sellFlag(sellFlag),
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
            packet.fields.reserve(FieldCount);
            packet.fields.push_back(PacketField {
                "name",
                PacketFieldType::EscapedString,
                name,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "lv",
                PacketFieldType::Int62,
                std::to_string(level),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "job",
                PacketFieldType::EscapedString,
                jobName,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "title",
                PacketFieldType::EscapedString,
                title,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "guildName",
                PacketFieldType::EscapedString,
                guildName,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "graphic",
                PacketFieldType::Int62,
                std::to_string(graphic),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "sid",
                PacketFieldType::Int62,
                std::to_string(sellFlag),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "smsg",
                PacketFieldType::EscapedString,
                sellMessage,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "bid",
                PacketFieldType::Int62,
                std::to_string(buyFlag),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "bmsg",
                PacketFieldType::EscapedString,
                buyMessage,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "aid",
                PacketFieldType::Int62,
                std::to_string(aboutFlag),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "amsg",
                PacketFieldType::EscapedString,
                aboutMessage,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "pmsg",
                PacketFieldType::EscapedString,
                profileMessage,
                PacketSourceTag::Xlsx,
            });
            return packet;
        }

        static PacketResult<ProfileData> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ProfileData>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ProfileData Parse 收到非 DpSY 封包");
                error.packetCode = packet.code;
                return PacketResult<ProfileData>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ProfileData>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileData Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ProfileData>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileData Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "name",
                "lv",
                "job",
                "title",
                "guildName",
                "graphic",
                "sid",
                "smsg",
                "bid",
                "bmsg",
                "aid",
                "amsg",
                "pmsg",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::EscapedString,
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
                PacketFieldType::EscapedString,
                PacketFieldType::EscapedString,
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
                PacketFieldType::EscapedString,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<ProfileData>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileData Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<ProfileData>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "ProfileData Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> levelValue = ParseDecimalField(packet.fields[LevelFieldIndex].value, LevelFieldIndex);
            if (!levelValue.HasValue()) {
                return PacketResult<ProfileData>::Failure(levelValue.Error());
            }
            PacketResult<int> graphicValue = ParseDecimalField(packet.fields[GraphicFieldIndex].value, GraphicFieldIndex);
            if (!graphicValue.HasValue()) {
                return PacketResult<ProfileData>::Failure(graphicValue.Error());
            }
            PacketResult<int> sellFlagValue = ParseDecimalField(packet.fields[SellFlagFieldIndex].value, SellFlagFieldIndex);
            if (!sellFlagValue.HasValue()) {
                return PacketResult<ProfileData>::Failure(sellFlagValue.Error());
            }
            PacketResult<int> buyFlagValue = ParseDecimalField(packet.fields[BuyFlagFieldIndex].value, BuyFlagFieldIndex);
            if (!buyFlagValue.HasValue()) {
                return PacketResult<ProfileData>::Failure(buyFlagValue.Error());
            }
            PacketResult<int> aboutFlagValue = ParseDecimalField(packet.fields[AboutFlagFieldIndex].value, AboutFlagFieldIndex);
            if (!aboutFlagValue.HasValue()) {
                return PacketResult<ProfileData>::Failure(aboutFlagValue.Error());
            }

            return PacketResult<ProfileData>::Success(ProfileData(
                packet.fields[NameFieldIndex].value,
                levelValue.Value(),
                packet.fields[JobNameFieldIndex].value,
                packet.fields[TitleFieldIndex].value,
                packet.fields[GuildNameFieldIndex].value,
                graphicValue.Value(),
                sellFlagValue.Value(),
                packet.fields[SellMessageFieldIndex].value,
                buyFlagValue.Value(),
                packet.fields[BuyMessageFieldIndex].value,
                aboutFlagValue.Value(),
                packet.fields[AboutMessageFieldIndex].value,
                packet.fields[ProfileMessageFieldIndex].value));
        }

    private:
        static constexpr std::size_t FieldCount = 13U;
        static constexpr std::size_t NameFieldIndex = 0U;
        static constexpr std::size_t LevelFieldIndex = 1U;
        static constexpr std::size_t JobNameFieldIndex = 2U;
        static constexpr std::size_t TitleFieldIndex = 3U;
        static constexpr std::size_t GuildNameFieldIndex = 4U;
        static constexpr std::size_t GraphicFieldIndex = 5U;
        static constexpr std::size_t SellFlagFieldIndex = 6U;
        static constexpr std::size_t SellMessageFieldIndex = 7U;
        static constexpr std::size_t BuyFlagFieldIndex = 8U;
        static constexpr std::size_t BuyMessageFieldIndex = 9U;
        static constexpr std::size_t AboutFlagFieldIndex = 10U;
        static constexpr std::size_t AboutMessageFieldIndex = 11U;
        static constexpr std::size_t ProfileMessageFieldIndex = 12U;

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
                    "ProfileData Parse i62 字段为空",
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
                        "ProfileData Parse i62 字段包含非十进制字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileData Parse i62 字段超出 int 范围",
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
     * SEND_PRE 删除迷你邮件成员		cNB {data:es}
     * data: profileIndex|name，profileIndex 为 raw decimal，不使用 i62；回写不带尾随 |。
     */
    struct ProfileMailDelete
    {
        static constexpr std::string_view Code = "cNB";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int profileIndex = 0;
        std::string name;

        ProfileMailDelete(int profileIndex, std::string name)
            : profileIndex(profileIndex),
              name(std::move(name))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::string payload = std::to_string(profileIndex);
            payload.push_back('|');
            payload.append(name);
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

        static PacketResult<ProfileMailDelete> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ProfileMailDelete>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ProfileMailDelete Parse 收到非 cNB 封包");
                error.packetCode = packet.code;
                return PacketResult<ProfileMailDelete>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ProfileMailDelete>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMailDelete Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ProfileMailDelete>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileMailDelete Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<ProfileMailDelete>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMailDelete Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<ProfileMailDelete>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ProfileMailDelete Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t PayloadFieldCount = 2U;

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

        [[nodiscard]] static PacketResult<ProfileMailDelete> ParseData(std::string_view payload)
        {
            PacketResult<std::vector<std::string>> parts = SplitPayload(payload, PayloadDelimiter);
            if (!parts.HasValue()) {
                PacketError error = parts.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<ProfileMailDelete>::Failure(std::move(error));
            }

            std::vector<std::string> payloadParts = std::move(parts.Value());
            if (payloadParts.size() != PayloadFieldCount) {
                return PacketResult<ProfileMailDelete>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileMailDelete payload 段数量不匹配",
                    DataFieldIndex));
            }

            PacketResult<int> profileIndexValue = ParseDecimalField(payloadParts[0], 0U);
            if (!profileIndexValue.HasValue()) {
                return PacketResult<ProfileMailDelete>::Failure(profileIndexValue.Error());
            }

            return PacketResult<ProfileMailDelete>::Success(
                ProfileMailDelete(profileIndexValue.Value(), std::move(payloadParts[1])));
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
                    "ProfileMailDelete payload 十进制字段为空",
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
                        "ProfileMailDelete payload 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileMailDelete payload 十进制字段超出 int 范围",
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
     * SEND_PRM 迷你信件回执		ow {profileIndex:i62} {dtext:es} {color:i62}
     * dtext 保存原始 es 字符串，不拆内部 sentTimeText|message|。
     */
    struct ProfileMailReceipt
    {
        static constexpr std::string_view Code = "ow";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        int profileIndex = 0;
        std::string text;
        int color = 0;

        ProfileMailReceipt(int profileIndex, std::string text, int color)
            : profileIndex(profileIndex),
              text(std::move(text)),
              color(color)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.reserve(FieldCount);
            packet.fields.push_back(PacketField {
                "profileIndex",
                PacketFieldType::Int62,
                std::to_string(profileIndex),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "dtext",
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

        static PacketResult<ProfileMailReceipt> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ProfileMailReceipt>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ProfileMailReceipt Parse 收到非 ow 封包");
                error.packetCode = packet.code;
                return PacketResult<ProfileMailReceipt>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ProfileMailReceipt>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMailReceipt Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ProfileMailReceipt>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileMailReceipt Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "profileIndex",
                "dtext",
                "color",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
                PacketFieldType::Int62,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<ProfileMailReceipt>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileMailReceipt Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<ProfileMailReceipt>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "ProfileMailReceipt Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> profileIndexValue = ParseDecimalField(
                packet.fields[ProfileIndexFieldIndex].value,
                ProfileIndexFieldIndex);
            if (!profileIndexValue.HasValue()) {
                return PacketResult<ProfileMailReceipt>::Failure(profileIndexValue.Error());
            }

            PacketResult<int> colorValue = ParseDecimalField(packet.fields[ColorFieldIndex].value, ColorFieldIndex);
            if (!colorValue.HasValue()) {
                return PacketResult<ProfileMailReceipt>::Failure(colorValue.Error());
            }

            return PacketResult<ProfileMailReceipt>::Success(ProfileMailReceipt(
                profileIndexValue.Value(),
                packet.fields[TextFieldIndex].value,
                colorValue.Value()));
        }

    private:
        static constexpr std::size_t FieldCount = 3U;
        static constexpr std::size_t ProfileIndexFieldIndex = 0U;
        static constexpr std::size_t TextFieldIndex = 1U;
        static constexpr std::size_t ColorFieldIndex = 2U;

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
                    "ProfileMailReceipt Parse i62 字段为空",
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
                        "ProfileMailReceipt Parse i62 字段包含非十进制字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileMailReceipt Parse i62 字段超出 int 范围",
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
     * SEND_PRAD 收到迷你信件		KyN {data:es}
     * data: profileIndex|faceImageNumber|level|name|guildName|ownTitle|senderProfileIndex|sentTimeText|message||color。
     * 第 10 段固定为空；数值字段为 raw decimal，不使用 i62。
     */
    struct ProfileMailArrive
    {
        static constexpr std::string_view Code = "KyN";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int profileIndex = 0;
        int faceImageNumber = 0;
        int level = 0;
        std::string name;
        std::string guildName;
        std::string ownTitle;
        int senderProfileIndex = 0;
        std::string sentTimeText;
        std::string message;
        int color = 0;

        ProfileMailArrive(
            int profileIndex,
            int faceImageNumber,
            int level,
            std::string name,
            std::string guildName,
            std::string ownTitle,
            int senderProfileIndex,
            std::string sentTimeText,
            std::string message,
            int color)
            : profileIndex(profileIndex),
              faceImageNumber(faceImageNumber),
              level(level),
              name(std::move(name)),
              guildName(std::move(guildName)),
              ownTitle(std::move(ownTitle)),
              senderProfileIndex(senderProfileIndex),
              sentTimeText(std::move(sentTimeText)),
              message(std::move(message)),
              color(color)
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            const std::array<std::string, PayloadFieldCount> values = {
                std::to_string(profileIndex),
                std::to_string(faceImageNumber),
                std::to_string(level),
                name,
                guildName,
                ownTitle,
                std::to_string(senderProfileIndex),
                sentTimeText,
                message,
                std::string(),
                std::to_string(color),
            };

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(values.data(), values.size()),
                PayloadDelimiter);
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

        static PacketResult<ProfileMailArrive> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ProfileMailArrive>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ProfileMailArrive Parse 收到非 KyN 封包");
                error.packetCode = packet.code;
                return PacketResult<ProfileMailArrive>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ProfileMailArrive>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMailArrive Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ProfileMailArrive>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileMailArrive Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<ProfileMailArrive>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMailArrive Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<ProfileMailArrive>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ProfileMailArrive Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t PayloadFieldCount = 11U;
        static constexpr std::size_t ProfileIndexPayloadIndex = 0U;
        static constexpr std::size_t FaceImageNumberPayloadIndex = 1U;
        static constexpr std::size_t LevelPayloadIndex = 2U;
        static constexpr std::size_t NamePayloadIndex = 3U;
        static constexpr std::size_t GuildNamePayloadIndex = 4U;
        static constexpr std::size_t OwnTitlePayloadIndex = 5U;
        static constexpr std::size_t SenderProfileIndexPayloadIndex = 6U;
        static constexpr std::size_t SentTimeTextPayloadIndex = 7U;
        static constexpr std::size_t MessagePayloadIndex = 8U;
        static constexpr std::size_t EmptyPayloadIndex = 9U;
        static constexpr std::size_t ColorPayloadIndex = 10U;

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

        [[nodiscard]] static PacketResult<ProfileMailArrive> ParseData(std::string_view payload)
        {
            PacketResult<std::vector<std::string>> parts = SplitPayload(payload, PayloadDelimiter);
            if (!parts.HasValue()) {
                PacketError error = parts.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<ProfileMailArrive>::Failure(std::move(error));
            }

            std::vector<std::string> payloadParts = std::move(parts.Value());
            if (payloadParts.size() != PayloadFieldCount) {
                return PacketResult<ProfileMailArrive>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileMailArrive payload 段数量不匹配",
                    DataFieldIndex));
            }

            if (!payloadParts[EmptyPayloadIndex].empty()) {
                return PacketResult<ProfileMailArrive>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMailArrive payload 第 10 段必须为空",
                    EmptyPayloadIndex));
            }

            PacketResult<int> profileIndexValue = ParseDecimalField(
                payloadParts[ProfileIndexPayloadIndex],
                ProfileIndexPayloadIndex);
            if (!profileIndexValue.HasValue()) {
                return PacketResult<ProfileMailArrive>::Failure(profileIndexValue.Error());
            }

            PacketResult<int> faceImageNumberValue = ParseDecimalField(
                payloadParts[FaceImageNumberPayloadIndex],
                FaceImageNumberPayloadIndex);
            if (!faceImageNumberValue.HasValue()) {
                return PacketResult<ProfileMailArrive>::Failure(faceImageNumberValue.Error());
            }

            PacketResult<int> levelValue = ParseDecimalField(payloadParts[LevelPayloadIndex], LevelPayloadIndex);
            if (!levelValue.HasValue()) {
                return PacketResult<ProfileMailArrive>::Failure(levelValue.Error());
            }

            PacketResult<int> senderProfileIndexValue = ParseDecimalField(
                payloadParts[SenderProfileIndexPayloadIndex],
                SenderProfileIndexPayloadIndex);
            if (!senderProfileIndexValue.HasValue()) {
                return PacketResult<ProfileMailArrive>::Failure(senderProfileIndexValue.Error());
            }

            PacketResult<int> colorValue = ParseDecimalField(payloadParts[ColorPayloadIndex], ColorPayloadIndex);
            if (!colorValue.HasValue()) {
                return PacketResult<ProfileMailArrive>::Failure(colorValue.Error());
            }

            return PacketResult<ProfileMailArrive>::Success(ProfileMailArrive(
                profileIndexValue.Value(),
                faceImageNumberValue.Value(),
                levelValue.Value(),
                std::move(payloadParts[NamePayloadIndex]),
                std::move(payloadParts[GuildNamePayloadIndex]),
                std::move(payloadParts[OwnTitlePayloadIndex]),
                senderProfileIndexValue.Value(),
                std::move(payloadParts[SentTimeTextPayloadIndex]),
                std::move(payloadParts[MessagePayloadIndex]),
                colorValue.Value()));
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
                    "ProfileMailArrive payload 十进制字段为空",
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
                        "ProfileMailArrive payload 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileMailArrive payload 十进制字段超出 int 范围",
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
     * SEND_PP 队伍参数记录：objectIndex|hp|maxHp|forcePoint|maxForcePoint。
     */
    struct PartyParamEntry
    {
        int objectIndex = 0;
        int hp = 0;
        int maxHp = 0;
        int forcePoint = 0;
        int maxForcePoint = 0;
    };

    /**
     * SEND_PP 队伍参数		TNT {data:es}
     * data: 1..5 条记录，每条 5 段；objectIndex 为 i62，其余为 raw decimal，整体保留尾随 `|`。
     */
    struct PartyParam
    {
        static constexpr std::string_view Code = "TNT";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        std::vector<PartyParamEntry> entries;

        explicit PartyParam(std::vector<PartyParamEntry> entries)
            : entries(std::move(entries))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::string payload;
            for (const PartyParamEntry& entry : entries) {
                PacketResult<std::string> objectIndex = EncodeInt62(entry.objectIndex);
                payload.append(objectIndex.HasValue() ? objectIndex.Value() : std::string());
                payload.push_back('|');
                payload.append(std::to_string(entry.hp));
                payload.push_back('|');
                payload.append(std::to_string(entry.maxHp));
                payload.push_back('|');
                payload.append(std::to_string(entry.forcePoint));
                payload.push_back('|');
                payload.append(std::to_string(entry.maxForcePoint));
                payload.push_back('|');
            }
            return payload;
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<PartyParam> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<PartyParam>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "PartyParam Parse 收到非 TNT 封包");
                error.packetCode = packet.code;
                return PacketResult<PartyParam>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<PartyParam>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "PartyParam Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<PartyParam>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "PartyParam Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<PartyParam>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PartyParam Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<PartyParam>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "PartyParam Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t EntryFieldCount = 5U;
        static constexpr std::size_t MaxEntryCount = 5U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<PartyParam> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<PartyParam>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "PartyParam payload 为空",
                    DataFieldIndex));
            }

            if (payload.back() != '|') {
                return PacketResult<PartyParam>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PartyParam payload 缺少尾随分隔符",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<PartyParam>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            parts.pop_back();
            if (parts.empty() || (parts.size() % EntryFieldCount) != 0U) {
                return PacketResult<PartyParam>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "PartyParam payload 段数量不是 5 的倍数",
                    DataFieldIndex));
            }

            const std::size_t entryCount = parts.size() / EntryFieldCount;
            if (entryCount == 0U || entryCount > MaxEntryCount) {
                return PacketResult<PartyParam>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "PartyParam payload 记录数不在 1..5 范围",
                    DataFieldIndex));
            }

            std::vector<PartyParamEntry> entries;
            entries.reserve(entryCount);
            for (std::size_t entryIndex = 0; entryIndex < entryCount; ++entryIndex) {
                const std::size_t baseIndex = entryIndex * EntryFieldCount;
                PacketResult<int> objectIndex = ParseObjectIndex(parts[baseIndex], baseIndex);
                if (!objectIndex.HasValue()) {
                    return PacketResult<PartyParam>::Failure(objectIndex.Error());
                }

                PacketResult<int> hp = ParseDecimalField(parts[baseIndex + 1U], baseIndex + 1U);
                if (!hp.HasValue()) {
                    return PacketResult<PartyParam>::Failure(hp.Error());
                }

                PacketResult<int> maxHp = ParseDecimalField(parts[baseIndex + 2U], baseIndex + 2U);
                if (!maxHp.HasValue()) {
                    return PacketResult<PartyParam>::Failure(maxHp.Error());
                }

                PacketResult<int> forcePoint = ParseDecimalField(parts[baseIndex + 3U], baseIndex + 3U);
                if (!forcePoint.HasValue()) {
                    return PacketResult<PartyParam>::Failure(forcePoint.Error());
                }

                PacketResult<int> maxForcePoint = ParseDecimalField(parts[baseIndex + 4U], baseIndex + 4U);
                if (!maxForcePoint.HasValue()) {
                    return PacketResult<PartyParam>::Failure(maxForcePoint.Error());
                }

                entries.push_back(PartyParamEntry {
                    objectIndex.Value(),
                    hp.Value(),
                    maxHp.Value(),
                    forcePoint.Value(),
                    maxForcePoint.Value(),
                });
            }

            return PacketResult<PartyParam>::Success(PartyParam(std::move(entries)));
        }

        [[nodiscard]] static PacketResult<int> ParseObjectIndex(std::string_view text, std::size_t fieldIndex)
        {
            if (text.empty() || text == "-") {
                return PacketResult<int>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PartyParam payload objectIndex 为空",
                    fieldIndex));
            }

            PacketResult<int> decoded = DecodeInt62(text);
            if (!decoded.HasValue()) {
                PacketError error = decoded.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = fieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<int>::Failure(std::move(error));
            }
            return decoded;
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
                    "PartyParam payload 十进制字段为空",
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
                        "PartyParam payload 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "PartyParam payload 十进制字段超出 int 范围",
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

    enum class PartyMemberStatusKind
    {
        EmptyOrRemove,
        All,
    };

    /**
     * SEND_CN 队伍成员完整状态：objectIndexllevel|maxHp|hp|forcePoint|name。
     */
    struct PartyMemberStatusEntry
    {
        int objectIndex = 0;
        int level = 0;
        int maxHp = 0;
        int hp = 0;
        int forcePoint = 0;
        std::string name;
    };

    /**
     * SEND_CN 队伍成员状态		JJQj {partySlot:i62} {data:es}
     * data: `0|` 表示空队员/离队；`1|objectIndexllevel|maxHp|hp|forcePoint|name|` 表示完整状态。
     */
    struct PartyMemberStatus
    {
        static constexpr std::string_view Code = "JJQj";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int partySlot = 0;
        PartyMemberStatusKind kind = PartyMemberStatusKind::EmptyOrRemove;
        PartyMemberStatusEntry member;

        explicit PartyMemberStatus(int partySlot)
            : partySlot(partySlot)
        {
        }

        PartyMemberStatus(int partySlot, PartyMemberStatusEntry member)
            : partySlot(partySlot),
              kind(PartyMemberStatusKind::All),
              member(std::move(member))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            if (kind == PartyMemberStatusKind::EmptyOrRemove) {
                return "0|";
            }

            std::string payload = "1|";
            payload.append(std::to_string(member.objectIndex));
            payload.push_back('l');
            payload.append(std::to_string(member.level));
            payload.push_back('|');
            payload.append(std::to_string(member.maxHp));
            payload.push_back('|');
            payload.append(std::to_string(member.hp));
            payload.push_back('|');
            payload.append(std::to_string(member.forcePoint));
            payload.push_back('|');
            payload.append(member.name);
            payload.push_back('|');
            return payload;
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "partySlot",
                PacketFieldType::Int62,
                std::to_string(partySlot),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<PartyMemberStatus> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<PartyMemberStatus>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(
                    PacketErrorCode::InvalidCode,
                    "PartyMemberStatus Parse 收到非 JJQj 封包");
                error.packetCode = packet.code;
                return PacketResult<PartyMemberStatus>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<PartyMemberStatus>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "PartyMemberStatus Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<PartyMemberStatus>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "PartyMemberStatus Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& partySlotField = packet.fields[PartySlotFieldIndex];
            if (partySlotField.name != "partySlot") {
                return PacketResult<PartyMemberStatus>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PartyMemberStatus Parse partySlot 字段名不匹配",
                    PartySlotFieldIndex));
            }

            if (partySlotField.type != PacketFieldType::Int62) {
                return PacketResult<PartyMemberStatus>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "PartyMemberStatus Parse partySlot 字段类型不匹配",
                    PartySlotFieldIndex));
            }

            PacketResult<int> partySlot = ParseDecimalField(partySlotField.value, PartySlotFieldIndex);
            if (!partySlot.HasValue()) {
                return PacketResult<PartyMemberStatus>::Failure(partySlot.Error());
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (dataField.name != "data") {
                return PacketResult<PartyMemberStatus>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PartyMemberStatus Parse data 字段名不匹配",
                    DataFieldIndex));
            }

            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<PartyMemberStatus>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "PartyMemberStatus Parse data 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(partySlot.Value(), dataField.value);
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t PartySlotFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;
        static constexpr std::size_t AllPayloadFieldCount = 6U;
        static constexpr std::size_t ObjectLevelFieldIndex = 1U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<PartyMemberStatus> ParseData(int partySlot, std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<PartyMemberStatus>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "PartyMemberStatus payload 为空",
                    DataFieldIndex));
            }

            if (payload.back() != '|') {
                return PacketResult<PartyMemberStatus>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PartyMemberStatus payload 缺少尾随分隔符",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<PartyMemberStatus>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            parts.pop_back();
            if (parts.empty()) {
                return PacketResult<PartyMemberStatus>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "PartyMemberStatus payload 缺少 fieldMask",
                    DataFieldIndex));
            }

            PacketResult<int> fieldMask = ParseFieldMask(parts.front());
            if (!fieldMask.HasValue()) {
                return PacketResult<PartyMemberStatus>::Failure(fieldMask.Error());
            }

            if (fieldMask.Value() == 0) {
                if (payload != "0|") {
                    return PacketResult<PartyMemberStatus>::Failure(MakeFieldError(
                        PacketErrorCode::FieldCountMismatch,
                        "PartyMemberStatus fieldMask 0 payload 必须为 0|",
                        DataFieldIndex));
                }
                return PacketResult<PartyMemberStatus>::Success(PartyMemberStatus(partySlot));
            }

            if (fieldMask.Value() != 1) {
                return PacketResult<PartyMemberStatus>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PartyMemberStatus 不支持的 fieldMask",
                    DataFieldIndex));
            }

            if (parts.size() != AllPayloadFieldCount) {
                return PacketResult<PartyMemberStatus>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "PartyMemberStatus fieldMask 1 payload 段数量不匹配",
                    DataFieldIndex));
            }

            PacketResult<PartyMemberStatusEntry> member = ParseAllPayload(parts);
            if (!member.HasValue()) {
                return PacketResult<PartyMemberStatus>::Failure(member.Error());
            }

            return PacketResult<PartyMemberStatus>::Success(PartyMemberStatus(partySlot, std::move(member.Value())));
        }

        [[nodiscard]] static PacketResult<int> ParseFieldMask(std::string_view text)
        {
            PacketResult<int> decoded = DecodeInt62(text);
            if (!decoded.HasValue()) {
                PacketError error = decoded.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<int>::Failure(std::move(error));
            }
            return decoded;
        }

        [[nodiscard]] static PacketResult<PartyMemberStatusEntry> ParseAllPayload(
            const std::vector<std::string>& parts)
        {
            const std::string& objectAndLevel = parts[ObjectLevelFieldIndex];
            const std::size_t separator = objectAndLevel.find('l');
            if (separator == std::string::npos ||
                separator == 0U ||
                separator + 1U == objectAndLevel.size() ||
                objectAndLevel.find('l', separator + 1U) != std::string::npos) {
                return PacketResult<PartyMemberStatusEntry>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PartyMemberStatus objectIndexllevel 格式不匹配",
                    ObjectLevelFieldIndex));
            }

            PacketResult<int> objectIndex = ParseDecimalField(
                std::string_view(objectAndLevel).substr(0U, separator),
                ObjectLevelFieldIndex);
            if (!objectIndex.HasValue()) {
                return PacketResult<PartyMemberStatusEntry>::Failure(objectIndex.Error());
            }

            PacketResult<int> level = ParseDecimalField(
                std::string_view(objectAndLevel).substr(separator + 1U),
                ObjectLevelFieldIndex);
            if (!level.HasValue()) {
                return PacketResult<PartyMemberStatusEntry>::Failure(level.Error());
            }

            PacketResult<int> maxHp = ParseDecimalField(parts[2], 2U);
            if (!maxHp.HasValue()) {
                return PacketResult<PartyMemberStatusEntry>::Failure(maxHp.Error());
            }

            PacketResult<int> hp = ParseDecimalField(parts[3], 3U);
            if (!hp.HasValue()) {
                return PacketResult<PartyMemberStatusEntry>::Failure(hp.Error());
            }

            PacketResult<int> forcePoint = ParseDecimalField(parts[4], 4U);
            if (!forcePoint.HasValue()) {
                return PacketResult<PartyMemberStatusEntry>::Failure(forcePoint.Error());
            }

            return PacketResult<PartyMemberStatusEntry>::Success(PartyMemberStatusEntry {
                objectIndex.Value(),
                level.Value(),
                maxHp.Value(),
                hp.Value(),
                forcePoint.Value(),
                parts[5],
            });
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
                    "PartyMemberStatus 十进制字段为空",
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
                        "PartyMemberStatus 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "PartyMemberStatus 十进制字段超出 int 范围",
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
     * SEND_CT 技能记录：techName|comment|forcePoint|field|targetFlag|canUse。
     */
    struct TechRecord
    {
        std::string techName;
        std::string comment;
        int forcePoint = 0;
        int field = 0;
        int targetFlag = 0;
        int canUse = 0;
    };

    /**
     * SEND_CT 技能列表		Rspn {skillIndex:i62} {data:es}
     * data: 每条 6 段，数值为 raw decimal，整体保留尾随 `|`；空 data 表示无记录。
     */
    struct TechList
    {
        static constexpr std::string_view Code = "Rspn";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int skillIndex = 0;
        std::vector<TechRecord> records;

        TechList(int skillIndex, std::vector<TechRecord> records)
            : skillIndex(skillIndex),
              records(std::move(records))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::string payload;
            for (const TechRecord& record : records) {
                payload.append(record.techName);
                payload.push_back('|');
                payload.append(record.comment);
                payload.push_back('|');
                payload.append(std::to_string(record.forcePoint));
                payload.push_back('|');
                payload.append(std::to_string(record.field));
                payload.push_back('|');
                payload.append(std::to_string(record.targetFlag));
                payload.push_back('|');
                payload.append(std::to_string(record.canUse));
                payload.push_back('|');
            }
            return payload;
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "skillIndex",
                PacketFieldType::Int62,
                std::to_string(skillIndex),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<TechList> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<TechList>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "TechList Parse 收到非 Rspn 封包");
                error.packetCode = packet.code;
                return PacketResult<TechList>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<TechList>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "TechList Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<TechList>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TechList Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& skillIndexField = packet.fields[SkillIndexFieldIndex];
            if (skillIndexField.name != "skillIndex") {
                return PacketResult<TechList>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "TechList Parse skillIndex 字段名不匹配",
                    SkillIndexFieldIndex));
            }

            if (skillIndexField.type != PacketFieldType::Int62) {
                return PacketResult<TechList>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "TechList Parse skillIndex 字段类型不匹配",
                    SkillIndexFieldIndex));
            }

            PacketResult<int> skillIndex = ParseDecimalField(skillIndexField.value, SkillIndexFieldIndex);
            if (!skillIndex.HasValue()) {
                return PacketResult<TechList>::Failure(skillIndex.Error());
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (dataField.name != "data") {
                return PacketResult<TechList>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "TechList Parse data 字段名不匹配",
                    DataFieldIndex));
            }

            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<TechList>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "TechList Parse data 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::vector<TechRecord>> records = ParseRecords(dataField.value);
            if (!records.HasValue()) {
                return PacketResult<TechList>::Failure(records.Error());
            }

            return PacketResult<TechList>::Success(TechList(skillIndex.Value(), std::move(records.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t SkillIndexFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;
        static constexpr std::size_t RecordFieldCount = 6U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<std::vector<TechRecord>> ParseRecords(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<std::vector<TechRecord>>::Success({});
            }

            if (payload.back() != '|') {
                return PacketResult<std::vector<TechRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "TechList payload 缺少尾随分隔符",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<std::vector<TechRecord>>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            parts.pop_back();
            if (parts.empty() || (parts.size() % RecordFieldCount) != 0U) {
                return PacketResult<std::vector<TechRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TechList payload 段数量不是 6 的倍数",
                    DataFieldIndex));
            }

            std::vector<TechRecord> records;
            records.reserve(parts.size() / RecordFieldCount);
            for (std::size_t recordIndex = 0; recordIndex < parts.size() / RecordFieldCount; ++recordIndex) {
                const std::size_t baseIndex = recordIndex * RecordFieldCount;
                PacketResult<int> forcePoint = ParseDecimalField(parts[baseIndex + 2U], baseIndex + 2U);
                if (!forcePoint.HasValue()) {
                    return PacketResult<std::vector<TechRecord>>::Failure(forcePoint.Error());
                }

                PacketResult<int> field = ParseDecimalField(parts[baseIndex + 3U], baseIndex + 3U);
                if (!field.HasValue()) {
                    return PacketResult<std::vector<TechRecord>>::Failure(field.Error());
                }

                PacketResult<int> targetFlag = ParseDecimalField(parts[baseIndex + 4U], baseIndex + 4U);
                if (!targetFlag.HasValue()) {
                    return PacketResult<std::vector<TechRecord>>::Failure(targetFlag.Error());
                }

                PacketResult<int> canUse = ParseDecimalField(parts[baseIndex + 5U], baseIndex + 5U);
                if (!canUse.HasValue()) {
                    return PacketResult<std::vector<TechRecord>>::Failure(canUse.Error());
                }

                records.push_back(TechRecord {
                    parts[baseIndex],
                    parts[baseIndex + 1U],
                    forcePoint.Value(),
                    field.Value(),
                    targetFlag.Value(),
                    canUse.Value(),
                });
            }

            return PacketResult<std::vector<TechRecord>>::Success(std::move(records));
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
                    "TechList 十进制字段为空",
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
                        "TechList 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TechList 十进制字段超出 int 范围",
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
     * SEND_CS 角色技能记录：skillSlot|skillId|name|level|exp|maxLevel|nextExp|requiredSlots|operation|consumptionRate|seqNo。
     */
    struct CharacterSkillRecord
    {
        int skillSlot = 0;
        int skillId = 0;
        std::string name;
        int level = 0;
        int exp = 0;
        int maxLevel = 0;
        int nextExp = 0;
        int requiredSlots = 0;
        int operation = 0;
        int consumptionRate = 0;
        int seqNo = 0;
        bool emptySlot = false;

        [[nodiscard]] static CharacterSkillRecord Empty(int skillSlot)
        {
            CharacterSkillRecord record;
            record.skillSlot = skillSlot;
            record.emptySlot = true;
            return record;
        }
    };

    /**
     * SEND_CS CHAR_sendSkillData		cNE {data:es}
     * data: 每条 11 段，数值为 raw decimal，name 保留二级转义 raw 字符串，整体回写保留尾随 `|`。
     */
    struct CharacterSkillData
    {
        static constexpr std::string_view Code = "cNE";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        std::vector<CharacterSkillRecord> records;

        explicit CharacterSkillData(std::vector<CharacterSkillRecord> records)
            : records(std::move(records))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::string payload;
            for (const CharacterSkillRecord& record : records) {
                payload.append(std::to_string(record.skillSlot));
                payload.push_back('|');

                if (record.emptySlot) {
                    for (std::size_t index = 0; index < EmptySlotFieldCount; ++index) {
                        payload.push_back('|');
                    }
                    continue;
                }

                payload.append(std::to_string(record.skillId));
                payload.push_back('|');
                payload.append(record.name);
                payload.push_back('|');
                payload.append(std::to_string(record.level));
                payload.push_back('|');
                payload.append(std::to_string(record.exp));
                payload.push_back('|');
                payload.append(std::to_string(record.maxLevel));
                payload.push_back('|');
                payload.append(std::to_string(record.nextExp));
                payload.push_back('|');
                payload.append(std::to_string(record.requiredSlots));
                payload.push_back('|');
                payload.append(std::to_string(record.operation));
                payload.push_back('|');
                payload.append(std::to_string(record.consumptionRate));
                payload.push_back('|');
                payload.append(std::to_string(record.seqNo));
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

        static PacketResult<CharacterSkillData> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<CharacterSkillData>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(
                    PacketErrorCode::InvalidCode,
                    "CharacterSkillData Parse 收到非 cNE 封包");
                error.packetCode = packet.code;
                return PacketResult<CharacterSkillData>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<CharacterSkillData>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "CharacterSkillData Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<CharacterSkillData>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "CharacterSkillData Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<CharacterSkillData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "CharacterSkillData Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<CharacterSkillData>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "CharacterSkillData Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::vector<CharacterSkillRecord>> records = ParseRecords(field.value);
            if (!records.HasValue()) {
                return PacketResult<CharacterSkillData>::Failure(records.Error());
            }

            return PacketResult<CharacterSkillData>::Success(CharacterSkillData(std::move(records.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t RecordFieldCount = 11U;
        static constexpr std::size_t EmptySlotFieldCount = RecordFieldCount - 1U;

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

        [[nodiscard]] static PacketResult<std::vector<CharacterSkillRecord>> ParseRecords(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<std::vector<CharacterSkillRecord>>::Success({});
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<std::vector<CharacterSkillRecord>>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            if ((parts.size() % RecordFieldCount) != 0U && !parts.empty() && parts.back().empty()) {
                parts.pop_back();
            }

            if (parts.empty() || (parts.size() % RecordFieldCount) != 0U) {
                return PacketResult<std::vector<CharacterSkillRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "CharacterSkillData payload 段数量不是 11 的倍数",
                    DataFieldIndex));
            }

            std::vector<CharacterSkillRecord> records;
            records.reserve(parts.size() / RecordFieldCount);
            for (std::size_t recordIndex = 0; recordIndex < parts.size() / RecordFieldCount; ++recordIndex) {
                const std::size_t baseIndex = recordIndex * RecordFieldCount;
                PacketResult<int> skillSlot = ParseDecimalField(parts[baseIndex], baseIndex);
                if (!skillSlot.HasValue()) {
                    return PacketResult<std::vector<CharacterSkillRecord>>::Failure(skillSlot.Error());
                }

                if (IsEmptySlot(parts, baseIndex)) {
                    records.push_back(CharacterSkillRecord::Empty(skillSlot.Value()));
                    continue;
                }

                PacketResult<int> skillId = ParseDecimalField(parts[baseIndex + 1U], baseIndex + 1U);
                if (!skillId.HasValue()) {
                    return PacketResult<std::vector<CharacterSkillRecord>>::Failure(skillId.Error());
                }

                PacketResult<int> level = ParseDecimalField(parts[baseIndex + 3U], baseIndex + 3U);
                if (!level.HasValue()) {
                    return PacketResult<std::vector<CharacterSkillRecord>>::Failure(level.Error());
                }

                PacketResult<int> exp = ParseDecimalField(parts[baseIndex + 4U], baseIndex + 4U);
                if (!exp.HasValue()) {
                    return PacketResult<std::vector<CharacterSkillRecord>>::Failure(exp.Error());
                }

                PacketResult<int> maxLevel = ParseDecimalField(parts[baseIndex + 5U], baseIndex + 5U);
                if (!maxLevel.HasValue()) {
                    return PacketResult<std::vector<CharacterSkillRecord>>::Failure(maxLevel.Error());
                }

                PacketResult<int> nextExp = ParseDecimalField(parts[baseIndex + 6U], baseIndex + 6U);
                if (!nextExp.HasValue()) {
                    return PacketResult<std::vector<CharacterSkillRecord>>::Failure(nextExp.Error());
                }

                PacketResult<int> requiredSlots = ParseDecimalField(parts[baseIndex + 7U], baseIndex + 7U);
                if (!requiredSlots.HasValue()) {
                    return PacketResult<std::vector<CharacterSkillRecord>>::Failure(requiredSlots.Error());
                }

                PacketResult<int> operation = ParseDecimalField(parts[baseIndex + 8U], baseIndex + 8U);
                if (!operation.HasValue()) {
                    return PacketResult<std::vector<CharacterSkillRecord>>::Failure(operation.Error());
                }

                PacketResult<int> consumptionRate = ParseDecimalField(parts[baseIndex + 9U], baseIndex + 9U);
                if (!consumptionRate.HasValue()) {
                    return PacketResult<std::vector<CharacterSkillRecord>>::Failure(consumptionRate.Error());
                }

                PacketResult<int> seqNo = ParseDecimalField(parts[baseIndex + 10U], baseIndex + 10U);
                if (!seqNo.HasValue()) {
                    return PacketResult<std::vector<CharacterSkillRecord>>::Failure(seqNo.Error());
                }

                records.push_back(CharacterSkillRecord {
                    skillSlot.Value(),
                    skillId.Value(),
                    parts[baseIndex + 2U],
                    level.Value(),
                    exp.Value(),
                    maxLevel.Value(),
                    nextExp.Value(),
                    requiredSlots.Value(),
                    operation.Value(),
                    consumptionRate.Value(),
                    seqNo.Value(),
                    false,
                });
            }

            return PacketResult<std::vector<CharacterSkillRecord>>::Success(std::move(records));
        }

        [[nodiscard]] static bool IsEmptySlot(const std::vector<std::string>& parts, std::size_t baseIndex)
        {
            for (std::size_t index = 1U; index < RecordFieldCount; ++index) {
                if (!parts[baseIndex + index].empty()) {
                    return false;
                }
            }
            return true;
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
                    "CharacterSkillData 十进制字段为空",
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
                        "CharacterSkillData 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "CharacterSkillData 十进制字段超出 int 范围",
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
     * SEND_I 道具记录：slot|nameRaw|nameColor|explanationRaw|imageId|ableUseField|battleUseFlag|target|level|flag|itemId|type|remain|memoRaw。
     */
    struct ItemRecord
    {
        int slot = 0;
        std::string nameRaw;
        int nameColor = 0;
        std::string explanationRaw;
        int imageId = 0;
        int ableUseField = 0;
        int battleUseFlag = 0;
        int target = 0;
        int level = 0;
        int flag = 0;
        int itemId = 0;
        int type = 0;
        int remain = 0;
        std::string memoRaw;
        bool clearSlot = false;

        [[nodiscard]] static ItemRecord Clear(int slot)
        {
            ItemRecord record;
            record.slot = slot;
            record.clearSlot = true;
            return record;
        }
    };

    struct ItemRecordCodecContext
    {
        std::string_view code;
        PacketSourceTag source = PacketSourceTag::Unknown;
        std::string_view packetName;
        std::size_t dataFieldIndex = 0U;
    };

    struct ItemRecordCodec
    {
        static constexpr std::string_view PayloadDelimiter = "|";
        static constexpr std::size_t RecordFieldCount = 14U;
        static constexpr std::size_t ClearFieldCount = RecordFieldCount - 1U;

        [[nodiscard]] static std::string BuildData(const std::vector<ItemRecord>& records)
        {
            std::vector<std::string> parts;
            parts.reserve(records.size() * RecordFieldCount);
            for (const ItemRecord& record : records) {
                parts.push_back(std::to_string(record.slot));
                if (record.clearSlot) {
                    for (std::size_t index = 0; index < ClearFieldCount; ++index) {
                        parts.emplace_back();
                    }
                    continue;
                }

                parts.push_back(record.nameRaw);
                parts.push_back(std::to_string(record.nameColor));
                parts.push_back(record.explanationRaw);
                parts.push_back(std::to_string(record.imageId));
                parts.push_back(std::to_string(record.ableUseField));
                parts.push_back(std::to_string(record.battleUseFlag));
                parts.push_back(std::to_string(record.target));
                parts.push_back(std::to_string(record.level));
                parts.push_back(std::to_string(record.flag));
                parts.push_back(std::to_string(record.itemId));
                parts.push_back(std::to_string(record.type));
                parts.push_back(std::to_string(record.remain));
                parts.push_back(record.memoRaw);
            }

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(parts.data(), parts.size()),
                PayloadDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }

            return {};
        }

        [[nodiscard]] static PacketResult<std::vector<ItemRecord>> ParseRecords(
            std::string_view payload,
            const ItemRecordCodecContext& context)
        {
            if (payload.empty()) {
                return PacketResult<std::vector<ItemRecord>>::Success({});
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(context.code);
                error.fieldIndex = context.dataFieldIndex;
                error.source = context.source;
                return PacketResult<std::vector<ItemRecord>>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            if (parts.empty() || (parts.size() % RecordFieldCount) != 0U) {
                return PacketResult<std::vector<ItemRecord>>::Failure(MakeFieldError(
                    context,
                    PacketErrorCode::FieldCountMismatch,
                    Message(context, " payload 段数量不是 14 的倍数"),
                    context.dataFieldIndex));
            }

            std::vector<ItemRecord> records;
            records.reserve(parts.size() / RecordFieldCount);
            for (std::size_t recordIndex = 0; recordIndex < parts.size() / RecordFieldCount; ++recordIndex) {
                const std::size_t baseIndex = recordIndex * RecordFieldCount;
                PacketResult<int> slot = ParseDecimalField(context, parts[baseIndex], baseIndex);
                if (!slot.HasValue()) {
                    return PacketResult<std::vector<ItemRecord>>::Failure(slot.Error());
                }

                if (parts[baseIndex + 1U].empty()) {
                    if (!IsClearRecord(parts, baseIndex)) {
                        return PacketResult<std::vector<ItemRecord>>::Failure(MakeFieldError(
                            context,
                            PacketErrorCode::DecodeFailed,
                            Message(context, " clear record 非 slot 字段必须为空"),
                            baseIndex + 1U));
                    }

                    records.push_back(ItemRecord::Clear(slot.Value()));
                    continue;
                }

                PacketResult<int> nameColor = ParseDecimalField(context, parts[baseIndex + 2U], baseIndex + 2U);
                if (!nameColor.HasValue()) {
                    return PacketResult<std::vector<ItemRecord>>::Failure(nameColor.Error());
                }

                PacketResult<int> imageId = ParseDecimalField(context, parts[baseIndex + 4U], baseIndex + 4U);
                if (!imageId.HasValue()) {
                    return PacketResult<std::vector<ItemRecord>>::Failure(imageId.Error());
                }

                PacketResult<int> ableUseField = ParseDecimalField(context, parts[baseIndex + 5U], baseIndex + 5U);
                if (!ableUseField.HasValue()) {
                    return PacketResult<std::vector<ItemRecord>>::Failure(ableUseField.Error());
                }

                PacketResult<int> battleUseFlag = ParseDecimalField(context, parts[baseIndex + 6U], baseIndex + 6U);
                if (!battleUseFlag.HasValue()) {
                    return PacketResult<std::vector<ItemRecord>>::Failure(battleUseFlag.Error());
                }

                PacketResult<int> target = ParseDecimalField(context, parts[baseIndex + 7U], baseIndex + 7U);
                if (!target.HasValue()) {
                    return PacketResult<std::vector<ItemRecord>>::Failure(target.Error());
                }

                PacketResult<int> level = ParseDecimalField(context, parts[baseIndex + 8U], baseIndex + 8U);
                if (!level.HasValue()) {
                    return PacketResult<std::vector<ItemRecord>>::Failure(level.Error());
                }

                PacketResult<int> flag = ParseDecimalField(context, parts[baseIndex + 9U], baseIndex + 9U);
                if (!flag.HasValue()) {
                    return PacketResult<std::vector<ItemRecord>>::Failure(flag.Error());
                }

                PacketResult<int> itemId = ParseDecimalField(context, parts[baseIndex + 10U], baseIndex + 10U);
                if (!itemId.HasValue()) {
                    return PacketResult<std::vector<ItemRecord>>::Failure(itemId.Error());
                }

                PacketResult<int> type = ParseDecimalField(context, parts[baseIndex + 11U], baseIndex + 11U);
                if (!type.HasValue()) {
                    return PacketResult<std::vector<ItemRecord>>::Failure(type.Error());
                }

                PacketResult<int> remain = ParseDecimalField(context, parts[baseIndex + 12U], baseIndex + 12U);
                if (!remain.HasValue()) {
                    return PacketResult<std::vector<ItemRecord>>::Failure(remain.Error());
                }

                records.push_back(ItemRecord {
                    slot.Value(),
                    parts[baseIndex + 1U],
                    nameColor.Value(),
                    parts[baseIndex + 3U],
                    imageId.Value(),
                    ableUseField.Value(),
                    battleUseFlag.Value(),
                    target.Value(),
                    level.Value(),
                    flag.Value(),
                    itemId.Value(),
                    type.Value(),
                    remain.Value(),
                    parts[baseIndex + 13U],
                    false,
                });
            }

            return PacketResult<std::vector<ItemRecord>>::Success(std::move(records));
        }

    private:
        [[nodiscard]] static std::string Message(const ItemRecordCodecContext& context, std::string_view suffix)
        {
            std::string message(context.packetName);
            message.append(suffix);
            return message;
        }

        [[nodiscard]] static PacketError MakePacketError(
            const ItemRecordCodecContext& context,
            PacketErrorCode code,
            std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(context.code);
            error.source = context.source;
            return error;
        }

        [[nodiscard]] static PacketError MakeFieldError(
            const ItemRecordCodecContext& context,
            PacketErrorCode code,
            std::string message,
            std::size_t fieldIndex)
        {
            PacketError error = MakePacketError(context, code, std::move(message));
            error.fieldIndex = fieldIndex;
            return error;
        }

        [[nodiscard]] static bool IsClearRecord(const std::vector<std::string>& parts, std::size_t baseIndex)
        {
            for (std::size_t index = 1U; index < RecordFieldCount; ++index) {
                if (!parts[baseIndex + index].empty()) {
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]] static PacketResult<int> ParseDecimalField(
            const ItemRecordCodecContext& context,
            std::string_view text,
            std::size_t fieldIndex)
        {
            bool negative = false;
            std::size_t index = 0;
            if (!text.empty() && text.front() == '-') {
                negative = true;
                index = 1U;
            }

            if (index == text.size()) {
                return PacketResult<int>::Failure(MakeFieldError(
                    context,
                    PacketErrorCode::DecodeFailed,
                    Message(context, " 十进制字段为空"),
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
                        context,
                        PacketErrorCode::DecodeFailed,
                        Message(context, " 十进制字段包含非法字符"),
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        context,
                        PacketErrorCode::DecodeFailed,
                        Message(context, " 十进制字段超出 int 范围"),
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
     * SEND_I CHAR_sendItemData		rU {data:es}
     * data: 每条 14 段，数值为 raw decimal，name/explanation/memo 保留二级转义 raw 字符串。
     */
    struct ItemData
    {
        static constexpr std::string_view Code = "rU";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        std::vector<ItemRecord> records;

        explicit ItemData(std::vector<ItemRecord> records)
            : records(std::move(records))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            return ItemRecordCodec::BuildData(records);
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

        static PacketResult<ItemData> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ItemData>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ItemData Parse 收到非 rU 封包");
                error.packetCode = packet.code;
                return PacketResult<ItemData>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ItemData>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ItemData Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ItemData>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ItemData Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<ItemData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ItemData Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<ItemData>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ItemData Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::vector<ItemRecord>> records = ItemRecordCodec::ParseRecords(
                field.value,
                ItemRecordCodecContext {Code, PacketSourceTag::Xlsx, "ItemData", DataFieldIndex});
            if (!records.HasValue()) {
                return PacketResult<ItemData>::Failure(records.Error());
            }

            return PacketResult<ItemData>::Success(ItemData(std::move(records.Value())));
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
    };

    /**
     * SEND_PET_ITEM nrproto_iKe_send		iKe {petSlot:i62} {data:es}
     * data: 复用普通物品 14 段记录；空 payload 为 0 records，不表示清空全部。
     */
    struct PetItemData
    {
        static constexpr std::string_view Code = "iKe";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int petSlot = 0;
        std::vector<ItemRecord> records;

        PetItemData(int petSlot, std::vector<ItemRecord> records)
            : petSlot(petSlot),
              records(std::move(records))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            return ItemRecordCodec::BuildData(records);
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "petSlot",
                PacketFieldType::Int62,
                std::to_string(petSlot),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<PetItemData> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<PetItemData>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<PetItemData> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "PetItemData Parse 收到非 iKe 封包");
                error.packetCode = packet.code;
                return PacketResult<PetItemData>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<PetItemData>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "PetItemData Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<PetItemData>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "PetItemData Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& petSlotField = packet.fields[PetSlotFieldIndex];
            if (petSlotField.name != "petSlot") {
                return PacketResult<PetItemData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PetItemData Parse petSlot 字段名不匹配",
                    PetSlotFieldIndex));
            }

            if (petSlotField.type != PacketFieldType::Int62) {
                return PacketResult<PetItemData>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "PetItemData Parse petSlot 字段类型不匹配",
                    PetSlotFieldIndex));
            }

            PacketResult<int> petSlotValue = ParseDecimalField(petSlotField.value, PetSlotFieldIndex);
            if (!petSlotValue.HasValue()) {
                return PacketResult<PetItemData>::Failure(petSlotValue.Error());
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (dataField.name != "data") {
                return PacketResult<PetItemData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PetItemData Parse data 字段名不匹配",
                    DataFieldIndex));
            }

            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<PetItemData>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "PetItemData Parse data 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::vector<ItemRecord>> records = ItemRecordCodec::ParseRecords(
                dataField.value,
                ItemRecordCodecContext {Code, PacketSourceTag::ServerC, "PetItemData", DataFieldIndex});
            if (!records.HasValue()) {
                return PacketResult<PetItemData>::Failure(records.Error());
            }

            return PacketResult<PetItemData>::Success(PetItemData(petSlotValue.Value(), std::move(records.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t PetSlotFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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
                    "PetItemData 十进制字段为空",
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
                        "PetItemData 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "PetItemData 十进制字段超出 int 范围",
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
     * SEND_EQUIP_SUIT 装备套装		TgX {data:es}
     * data: 空值表示 inactive；非空为 27 段逗号 payload，字符串字段保留 raw escaped。
     */
    struct EquipSuit
    {
        static constexpr std::string_view Code = "TgX";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = ",";

        bool active = false;
        std::string nameRaw;
        std::string skillNameRaw;
        int specialTitleFlag = 0;
        int attackPercent = 0;
        int attackValue = 0;
        int defensePercent = 0;
        int defenseValue = 0;
        int agilityPercent = 0;
        int agilityValue = 0;
        int spiritPercent = 0;
        int spiritValue = 0;
        int recoveryPercent = 0;
        int recoveryValue = 0;
        int hpPercent = 0;
        int hpValue = 0;
        int fpPercent = 0;
        int fpValue = 0;
        int poisonResist = 0;
        int sleepResist = 0;
        int stoneResist = 0;
        int drunkResist = 0;
        int confusionResist = 0;
        int amnesiaResist = 0;
        int critical = 0;
        int counter = 0;
        int hitRate = 0;
        int avoid = 0;

        EquipSuit() = default;

        EquipSuit(
            std::string nameRaw,
            std::string skillNameRaw,
            int specialTitleFlag,
            int attackPercent,
            int attackValue,
            int defensePercent,
            int defenseValue,
            int agilityPercent,
            int agilityValue,
            int spiritPercent,
            int spiritValue,
            int recoveryPercent,
            int recoveryValue,
            int hpPercent,
            int hpValue,
            int fpPercent,
            int fpValue,
            int poisonResist,
            int sleepResist,
            int stoneResist,
            int drunkResist,
            int confusionResist,
            int amnesiaResist,
            int critical,
            int counter,
            int hitRate,
            int avoid)
            : active(true),
              nameRaw(std::move(nameRaw)),
              skillNameRaw(std::move(skillNameRaw)),
              specialTitleFlag(specialTitleFlag),
              attackPercent(attackPercent),
              attackValue(attackValue),
              defensePercent(defensePercent),
              defenseValue(defenseValue),
              agilityPercent(agilityPercent),
              agilityValue(agilityValue),
              spiritPercent(spiritPercent),
              spiritValue(spiritValue),
              recoveryPercent(recoveryPercent),
              recoveryValue(recoveryValue),
              hpPercent(hpPercent),
              hpValue(hpValue),
              fpPercent(fpPercent),
              fpValue(fpValue),
              poisonResist(poisonResist),
              sleepResist(sleepResist),
              stoneResist(stoneResist),
              drunkResist(drunkResist),
              confusionResist(confusionResist),
              amnesiaResist(amnesiaResist),
              critical(critical),
              counter(counter),
              hitRate(hitRate),
              avoid(avoid)
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            if (!active) {
                return {};
            }

            const std::array<std::string, PayloadFieldCount> values = {
                nameRaw,
                skillNameRaw,
                std::to_string(specialTitleFlag),
                std::to_string(attackPercent),
                std::to_string(attackValue),
                std::to_string(defensePercent),
                std::to_string(defenseValue),
                std::to_string(agilityPercent),
                std::to_string(agilityValue),
                std::to_string(spiritPercent),
                std::to_string(spiritValue),
                std::to_string(recoveryPercent),
                std::to_string(recoveryValue),
                std::to_string(hpPercent),
                std::to_string(hpValue),
                std::to_string(fpPercent),
                std::to_string(fpValue),
                std::to_string(poisonResist),
                std::to_string(sleepResist),
                std::to_string(stoneResist),
                std::to_string(drunkResist),
                std::to_string(confusionResist),
                std::to_string(amnesiaResist),
                std::to_string(critical),
                std::to_string(counter),
                std::to_string(hitRate),
                std::to_string(avoid),
            };

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(values.data(), values.size()),
                PayloadDelimiter);
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

        static PacketResult<EquipSuit> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<EquipSuit>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<EquipSuit> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "EquipSuit Parse 收到非 TgX 封包");
                error.packetCode = packet.code;
                return PacketResult<EquipSuit>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<EquipSuit>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "EquipSuit Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<EquipSuit>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "EquipSuit Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<EquipSuit>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "EquipSuit Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<EquipSuit>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "EquipSuit Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t PayloadFieldCount = 27U;
        static constexpr std::size_t StatFieldCount = 24U;

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

        [[nodiscard]] static PacketResult<EquipSuit> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<EquipSuit>::Success(EquipSuit());
            }

            PacketResult<std::vector<std::string>> split = SplitPayloadLegacyAware(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<EquipSuit>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != PayloadFieldCount) {
                return PacketResult<EquipSuit>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "EquipSuit payload 段数量不匹配",
                    DataFieldIndex));
            }

            PacketResult<int> flag = ParseDecimalField(parts[2U], 2U);
            if (!flag.HasValue()) {
                return PacketResult<EquipSuit>::Failure(flag.Error());
            }
            if (flag.Value() != 0 && flag.Value() != 1) {
                return PacketResult<EquipSuit>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "EquipSuit specialTitleFlag 只能为 0 或 1",
                    2U));
            }

            EquipSuit equipSuit;
            equipSuit.active = true;
            equipSuit.nameRaw = parts[0U];
            equipSuit.skillNameRaw = parts[1U];
            equipSuit.specialTitleFlag = flag.Value();

            for (std::size_t index = 0; index < StatFieldCount; ++index) {
                const std::size_t payloadIndex = index + 3U;
                PacketResult<int> value = ParseDecimalField(parts[payloadIndex], payloadIndex);
                if (!value.HasValue()) {
                    return PacketResult<EquipSuit>::Failure(value.Error());
                }
                AssignStatValue(equipSuit, index, value.Value());
            }

            return PacketResult<EquipSuit>::Success(std::move(equipSuit));
        }

        static void AssignStatValue(EquipSuit& equipSuit, std::size_t index, int value)
        {
            switch (index) {
            case 0U:
                equipSuit.attackPercent = value;
                break;
            case 1U:
                equipSuit.attackValue = value;
                break;
            case 2U:
                equipSuit.defensePercent = value;
                break;
            case 3U:
                equipSuit.defenseValue = value;
                break;
            case 4U:
                equipSuit.agilityPercent = value;
                break;
            case 5U:
                equipSuit.agilityValue = value;
                break;
            case 6U:
                equipSuit.spiritPercent = value;
                break;
            case 7U:
                equipSuit.spiritValue = value;
                break;
            case 8U:
                equipSuit.recoveryPercent = value;
                break;
            case 9U:
                equipSuit.recoveryValue = value;
                break;
            case 10U:
                equipSuit.hpPercent = value;
                break;
            case 11U:
                equipSuit.hpValue = value;
                break;
            case 12U:
                equipSuit.fpPercent = value;
                break;
            case 13U:
                equipSuit.fpValue = value;
                break;
            case 14U:
                equipSuit.poisonResist = value;
                break;
            case 15U:
                equipSuit.sleepResist = value;
                break;
            case 16U:
                equipSuit.stoneResist = value;
                break;
            case 17U:
                equipSuit.drunkResist = value;
                break;
            case 18U:
                equipSuit.confusionResist = value;
                break;
            case 19U:
                equipSuit.amnesiaResist = value;
                break;
            case 20U:
                equipSuit.critical = value;
                break;
            case 21U:
                equipSuit.counter = value;
                break;
            case 22U:
                equipSuit.hitRate = value;
                break;
            case 23U:
                equipSuit.avoid = value;
                break;
            default:
                break;
            }
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
                    "EquipSuit 十进制字段为空",
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
                        "EquipSuit 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "EquipSuit 十进制字段超出 int 范围",
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
     * SEND_IA 道具信息槽：nameRaw|color|descriptionRaw|imageNo|canUseInField|battleUseFlag|target|level|flag|itemId|itemType|count|memoRaw。
     */
    struct ItemInfoSlot
    {
        std::string nameRaw;
        int color = 0;
        std::string descriptionRaw;
        int imageNo = 0;
        int canUseInField = 0;
        int battleUseFlag = 0;
        int target = 0;
        int level = 0;
        int flag = 0;
        int itemId = 0;
        int itemType = 0;
        int count = 0;
        std::string memoRaw;
        bool emptySlot = true;
    };

    /**
     * SEND_IA nrproto_IA_send		ELoy {data:es}
     * data: ignored0 + 28 个隐式道具槽；每槽 13 段，无显式 slotIndex。
     */
    struct ItemInfoList
    {
        static constexpr std::string_view Code = "ELoy";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";
        static constexpr std::size_t SlotCount = 28U;
        static constexpr std::size_t SlotFieldCount = 13U;

        std::string ignored0;
        std::array<ItemInfoSlot, SlotCount> slots = {};

        ItemInfoList(std::string ignored0, std::array<ItemInfoSlot, SlotCount> slots)
            : ignored0(std::move(ignored0)),
              slots(std::move(slots))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            if (ignored0.empty() && IsAllSlotsEmpty()) {
                return {};
            }

            std::vector<std::string> parts;
            parts.reserve(1U + (SlotCount * SlotFieldCount));
            parts.push_back(ignored0);

            for (const ItemInfoSlot& slot : slots) {
                if (slot.emptySlot) {
                    for (std::size_t index = 0; index < SlotFieldCount; ++index) {
                        parts.emplace_back();
                    }
                    continue;
                }

                parts.push_back(slot.nameRaw);
                parts.push_back(std::to_string(slot.color));
                parts.push_back(slot.descriptionRaw);
                parts.push_back(std::to_string(slot.imageNo));
                parts.push_back(std::to_string(slot.canUseInField));
                parts.push_back(std::to_string(slot.battleUseFlag));
                parts.push_back(std::to_string(slot.target));
                parts.push_back(std::to_string(slot.level));
                parts.push_back(std::to_string(slot.flag));
                parts.push_back(std::to_string(slot.itemId));
                parts.push_back(std::to_string(slot.itemType));
                parts.push_back(std::to_string(slot.count));
                parts.push_back(slot.memoRaw);
            }

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(parts.data(), parts.size()),
                PayloadDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }

            return {};
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<ItemInfoList> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ItemInfoList>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ItemInfoList Parse 收到非 ELoy 封包");
                error.packetCode = packet.code;
                return PacketResult<ItemInfoList>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ItemInfoList>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ItemInfoList Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ItemInfoList>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ItemInfoList Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<ItemInfoList>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ItemInfoList Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<ItemInfoList>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ItemInfoList Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t TotalPayloadFieldCount = 1U + (SlotCount * SlotFieldCount);

        [[nodiscard]] bool IsAllSlotsEmpty() const
        {
            for (const ItemInfoSlot& slot : slots) {
                if (!slot.emptySlot) {
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<ItemInfoList> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<ItemInfoList>::Success(ItemInfoList(std::string(), {}));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<ItemInfoList>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            if (parts.size() != TotalPayloadFieldCount) {
                return PacketResult<ItemInfoList>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ItemInfoList payload 段数量不匹配",
                    DataFieldIndex));
            }

            std::array<ItemInfoSlot, SlotCount> slots = {};
            for (std::size_t slotIndex = 0; slotIndex < SlotCount; ++slotIndex) {
                const std::size_t baseIndex = 1U + (slotIndex * SlotFieldCount);
                if (IsEmptySlot(parts, baseIndex)) {
                    continue;
                }

                PacketResult<ItemInfoSlot> slot = ParseSlot(parts, baseIndex);
                if (!slot.HasValue()) {
                    return PacketResult<ItemInfoList>::Failure(slot.Error());
                }
                slots[slotIndex] = std::move(slot.Value());
            }

            return PacketResult<ItemInfoList>::Success(ItemInfoList(std::move(parts[0]), std::move(slots)));
        }

        [[nodiscard]] static bool IsEmptySlot(const std::vector<std::string>& parts, std::size_t baseIndex)
        {
            for (std::size_t index = 0; index < SlotFieldCount; ++index) {
                if (!parts[baseIndex + index].empty()) {
                    return false;
                }
            }
            return true;
        }

        [[nodiscard]] static PacketResult<ItemInfoSlot> ParseSlot(
            const std::vector<std::string>& parts,
            std::size_t baseIndex)
        {
            PacketResult<int> color = ParseDecimalField(parts[baseIndex + 1U], baseIndex + 1U);
            if (!color.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(color.Error());
            }

            PacketResult<int> imageNo = ParseDecimalField(parts[baseIndex + 3U], baseIndex + 3U);
            if (!imageNo.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(imageNo.Error());
            }

            PacketResult<int> canUseInField = ParseDecimalField(parts[baseIndex + 4U], baseIndex + 4U);
            if (!canUseInField.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(canUseInField.Error());
            }

            PacketResult<int> battleUseFlag = ParseDecimalField(parts[baseIndex + 5U], baseIndex + 5U);
            if (!battleUseFlag.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(battleUseFlag.Error());
            }

            PacketResult<int> target = ParseDecimalField(parts[baseIndex + 6U], baseIndex + 6U);
            if (!target.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(target.Error());
            }

            PacketResult<int> level = ParseDecimalField(parts[baseIndex + 7U], baseIndex + 7U);
            if (!level.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(level.Error());
            }

            PacketResult<int> flag = ParseDecimalField(parts[baseIndex + 8U], baseIndex + 8U);
            if (!flag.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(flag.Error());
            }

            PacketResult<int> itemId = ParseDecimalField(parts[baseIndex + 9U], baseIndex + 9U);
            if (!itemId.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(itemId.Error());
            }

            PacketResult<int> itemType = ParseDecimalField(parts[baseIndex + 10U], baseIndex + 10U);
            if (!itemType.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(itemType.Error());
            }

            PacketResult<int> count = ParseDecimalField(parts[baseIndex + 11U], baseIndex + 11U);
            if (!count.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(count.Error());
            }

            PacketResult<std::string> nameRaw = ValidateRawStringField(parts[baseIndex], baseIndex);
            if (!nameRaw.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(nameRaw.Error());
            }

            PacketResult<std::string> descriptionRaw = ValidateRawStringField(parts[baseIndex + 2U], baseIndex + 2U);
            if (!descriptionRaw.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(descriptionRaw.Error());
            }

            PacketResult<std::string> memoRaw = ValidateRawStringField(parts[baseIndex + 12U], baseIndex + 12U);
            if (!memoRaw.HasValue()) {
                return PacketResult<ItemInfoSlot>::Failure(memoRaw.Error());
            }

            return PacketResult<ItemInfoSlot>::Success(ItemInfoSlot {
                std::move(nameRaw.Value()),
                color.Value(),
                std::move(descriptionRaw.Value()),
                imageNo.Value(),
                canUseInField.Value(),
                battleUseFlag.Value(),
                target.Value(),
                level.Value(),
                flag.Value(),
                itemId.Value(),
                itemType.Value(),
                count.Value(),
                std::move(memoRaw.Value()),
                false,
            });
        }

        [[nodiscard]] static PacketResult<std::string> ValidateRawStringField(
            std::string_view text,
            std::size_t fieldIndex)
        {
            for (std::size_t index = 0; index < text.size(); ++index) {
                if (!cg::packet::detail::IsLegacyDoubleByteLead(text[index])) {
                    continue;
                }

                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ItemInfoList raw 字符串旧双字节字符被截断",
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
                    "ItemInfoList 十进制字段为空",
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
                        "ItemInfoList 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ItemInfoList 十进制字段超出 int 范围",
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
     * SEND_IR 配方材料：materialItemId|materialName|required。
     */
    struct ItemRecipeMaterial
    {
        int itemId = 0;
        std::string nameRaw;
        int required = 0;
    };

    /**
     * SEND_IR 道具配方记录：22 段，字符串字段为内层 makeEscapeString 后的 raw 字符串。
     */
    struct ItemRecipeRecord
    {
        int recipeId = 0;
        int itemLevel = 0;
        int recipeItemId = 0;
        std::string itemNameRaw;
        std::string itemInfoRaw;
        int forcePoint = 0;
        int usable = 0;
        std::array<ItemRecipeMaterial, 5U> materials = {};
    };

    /**
     * SEND_IR 道具配方列表		xmgQ {haveskillindex:i62} {data:es}
     * data: 每条 22 段，数值为 raw decimal，整体保留尾随 `|`；record 数量为 1..56。
     */
    struct ItemRecipe
    {
        static constexpr std::string_view Code = "xmgQ";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int haveSkillIndex = 0;
        std::vector<ItemRecipeRecord> records;

        ItemRecipe(int haveSkillIndex, std::vector<ItemRecipeRecord> records)
            : haveSkillIndex(haveSkillIndex),
              records(std::move(records))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::string payload;
            for (const ItemRecipeRecord& record : records) {
                payload.append(std::to_string(record.recipeId));
                payload.push_back('|');
                payload.append(std::to_string(record.itemLevel));
                payload.push_back('|');
                payload.append(std::to_string(record.recipeItemId));
                payload.push_back('|');
                payload.append(record.itemNameRaw);
                payload.push_back('|');
                payload.append(record.itemInfoRaw);
                payload.push_back('|');
                payload.append(std::to_string(record.forcePoint));
                payload.push_back('|');
                payload.append(std::to_string(record.usable));
                payload.push_back('|');
                for (const ItemRecipeMaterial& material : record.materials) {
                    payload.append(std::to_string(material.itemId));
                    payload.push_back('|');
                    payload.append(material.nameRaw);
                    payload.push_back('|');
                    payload.append(std::to_string(material.required));
                    payload.push_back('|');
                }
            }
            return payload;
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "haveskillindex",
                PacketFieldType::Int62,
                std::to_string(haveSkillIndex),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<ItemRecipe> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ItemRecipe>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ItemRecipe Parse 收到非 xmgQ 封包");
                error.packetCode = packet.code;
                return PacketResult<ItemRecipe>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ItemRecipe>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ItemRecipe Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ItemRecipe>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ItemRecipe Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& haveSkillIndexField = packet.fields[HaveSkillIndexFieldIndex];
            if (haveSkillIndexField.name != "haveskillindex") {
                return PacketResult<ItemRecipe>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ItemRecipe Parse haveskillindex 字段名不匹配",
                    HaveSkillIndexFieldIndex));
            }

            if (haveSkillIndexField.type != PacketFieldType::Int62) {
                return PacketResult<ItemRecipe>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ItemRecipe Parse haveskillindex 字段类型不匹配",
                    HaveSkillIndexFieldIndex));
            }

            PacketResult<int> haveSkillIndex = ParseDecimalField(
                haveSkillIndexField.value,
                HaveSkillIndexFieldIndex);
            if (!haveSkillIndex.HasValue()) {
                return PacketResult<ItemRecipe>::Failure(haveSkillIndex.Error());
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (dataField.name != "data") {
                return PacketResult<ItemRecipe>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ItemRecipe Parse data 字段名不匹配",
                    DataFieldIndex));
            }

            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<ItemRecipe>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ItemRecipe Parse data 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::vector<ItemRecipeRecord>> records = ParseRecords(dataField.value);
            if (!records.HasValue()) {
                return PacketResult<ItemRecipe>::Failure(records.Error());
            }

            return PacketResult<ItemRecipe>::Success(ItemRecipe(
                haveSkillIndex.Value(),
                std::move(records.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t HaveSkillIndexFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;
        static constexpr std::size_t RecordFieldCount = 22U;
        static constexpr std::size_t MaterialCount = 5U;
        static constexpr std::size_t MaterialFieldCount = 3U;
        static constexpr std::size_t MaxRecordCount = 56U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<std::vector<ItemRecipeRecord>> ParseRecords(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<std::vector<ItemRecipeRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "ItemRecipe payload 为空",
                    DataFieldIndex));
            }

            if (payload.back() != '|') {
                return PacketResult<std::vector<ItemRecipeRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ItemRecipe payload 缺少尾随分隔符",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<std::vector<ItemRecipeRecord>>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            parts.pop_back();
            if (parts.empty() || (parts.size() % RecordFieldCount) != 0U) {
                return PacketResult<std::vector<ItemRecipeRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ItemRecipe payload 段数量不是 22 的倍数",
                    DataFieldIndex));
            }

            const std::size_t recordCount = parts.size() / RecordFieldCount;
            if (recordCount > MaxRecordCount) {
                return PacketResult<std::vector<ItemRecipeRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ItemRecipe payload record 数量超过 56",
                    DataFieldIndex));
            }

            std::vector<ItemRecipeRecord> records;
            records.reserve(recordCount);
            for (std::size_t recordIndex = 0; recordIndex < recordCount; ++recordIndex) {
                PacketResult<ItemRecipeRecord> record = ParseRecord(parts, recordIndex * RecordFieldCount);
                if (!record.HasValue()) {
                    return PacketResult<std::vector<ItemRecipeRecord>>::Failure(record.Error());
                }
                records.push_back(std::move(record.Value()));
            }

            return PacketResult<std::vector<ItemRecipeRecord>>::Success(std::move(records));
        }

        [[nodiscard]] static PacketResult<ItemRecipeRecord> ParseRecord(
            const std::vector<std::string>& parts,
            std::size_t baseIndex)
        {
            PacketResult<int> recipeId = ParseDecimalField(parts[baseIndex], baseIndex);
            if (!recipeId.HasValue()) {
                return PacketResult<ItemRecipeRecord>::Failure(recipeId.Error());
            }

            PacketResult<int> itemLevel = ParseDecimalField(parts[baseIndex + 1U], baseIndex + 1U);
            if (!itemLevel.HasValue()) {
                return PacketResult<ItemRecipeRecord>::Failure(itemLevel.Error());
            }

            PacketResult<int> recipeItemId = ParseDecimalField(parts[baseIndex + 2U], baseIndex + 2U);
            if (!recipeItemId.HasValue()) {
                return PacketResult<ItemRecipeRecord>::Failure(recipeItemId.Error());
            }

            PacketResult<std::string> itemNameRaw = ValidateRawStringField(parts[baseIndex + 3U], baseIndex + 3U);
            if (!itemNameRaw.HasValue()) {
                return PacketResult<ItemRecipeRecord>::Failure(itemNameRaw.Error());
            }

            PacketResult<std::string> itemInfoRaw = ValidateRawStringField(parts[baseIndex + 4U], baseIndex + 4U);
            if (!itemInfoRaw.HasValue()) {
                return PacketResult<ItemRecipeRecord>::Failure(itemInfoRaw.Error());
            }

            PacketResult<int> forcePoint = ParseDecimalField(parts[baseIndex + 5U], baseIndex + 5U);
            if (!forcePoint.HasValue()) {
                return PacketResult<ItemRecipeRecord>::Failure(forcePoint.Error());
            }

            PacketResult<int> usable = ParseDecimalField(parts[baseIndex + 6U], baseIndex + 6U);
            if (!usable.HasValue()) {
                return PacketResult<ItemRecipeRecord>::Failure(usable.Error());
            }
            if (usable.Value() != 0 && usable.Value() != 1) {
                return PacketResult<ItemRecipeRecord>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ItemRecipe usable 只能为 0/1",
                    baseIndex + 6U));
            }

            ItemRecipeRecord record;
            record.recipeId = recipeId.Value();
            record.itemLevel = itemLevel.Value();
            record.recipeItemId = recipeItemId.Value();
            record.itemNameRaw = std::move(itemNameRaw.Value());
            record.itemInfoRaw = std::move(itemInfoRaw.Value());
            record.forcePoint = forcePoint.Value();
            record.usable = usable.Value();

            for (std::size_t materialIndex = 0; materialIndex < MaterialCount; ++materialIndex) {
                const std::size_t materialBase = baseIndex + 7U + (materialIndex * MaterialFieldCount);
                PacketResult<ItemRecipeMaterial> material = ParseMaterial(parts, materialBase);
                if (!material.HasValue()) {
                    return PacketResult<ItemRecipeRecord>::Failure(material.Error());
                }
                record.materials[materialIndex] = std::move(material.Value());
            }

            return PacketResult<ItemRecipeRecord>::Success(std::move(record));
        }

        [[nodiscard]] static PacketResult<ItemRecipeMaterial> ParseMaterial(
            const std::vector<std::string>& parts,
            std::size_t baseIndex)
        {
            PacketResult<int> itemId = ParseDecimalField(parts[baseIndex], baseIndex);
            if (!itemId.HasValue()) {
                return PacketResult<ItemRecipeMaterial>::Failure(itemId.Error());
            }

            PacketResult<std::string> nameRaw = ValidateRawStringField(parts[baseIndex + 1U], baseIndex + 1U);
            if (!nameRaw.HasValue()) {
                return PacketResult<ItemRecipeMaterial>::Failure(nameRaw.Error());
            }

            PacketResult<int> required = ParseDecimalField(parts[baseIndex + 2U], baseIndex + 2U);
            if (!required.HasValue()) {
                return PacketResult<ItemRecipeMaterial>::Failure(required.Error());
            }

            return PacketResult<ItemRecipeMaterial>::Success(ItemRecipeMaterial {
                itemId.Value(),
                std::move(nameRaw.Value()),
                required.Value(),
            });
        }

        [[nodiscard]] static PacketResult<std::string> ValidateRawStringField(
            std::string_view text,
            std::size_t fieldIndex)
        {
            for (std::size_t index = 0; index < text.size(); ++index) {
                if (!cg::packet::detail::IsLegacyDoubleByteLead(text[index])) {
                    continue;
                }

                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ItemRecipe raw 字符串旧双字节字符被截断",
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
                    "ItemRecipe 十进制字段为空",
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
                        "ItemRecipe 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ItemRecipe 十进制字段超出 int 范围",
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
     * SEND_AL 图鉴页标志		Ilz {albumversion:i62} {data:es}
     * data: `page0|page1|...|`，page 为 raw decimal int；不在 codec 展开每页 bit 语义。
     */
    struct AlbumFlags
    {
        static constexpr std::string_view Code = "Ilz";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int albumVersion = 0;
        std::vector<int> pages;

        AlbumFlags(int albumVersion, std::vector<int> pages)
            : albumVersion(albumVersion),
              pages(std::move(pages))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::string payload;
            for (const int page : pages) {
                payload.append(std::to_string(page));
                payload.push_back('|');
            }
            return payload;
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "albumversion",
                PacketFieldType::Int62,
                std::to_string(albumVersion),
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

        static PacketResult<AlbumFlags> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<AlbumFlags>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "AlbumFlags Parse 收到非 Ilz 封包");
                error.packetCode = packet.code;
                return PacketResult<AlbumFlags>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<AlbumFlags>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "AlbumFlags Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<AlbumFlags>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "AlbumFlags Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& albumVersionField = packet.fields[AlbumVersionFieldIndex];
            if (albumVersionField.name != "albumversion") {
                return PacketResult<AlbumFlags>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "AlbumFlags Parse albumversion 字段名不匹配",
                    AlbumVersionFieldIndex));
            }

            if (albumVersionField.type != PacketFieldType::Int62) {
                return PacketResult<AlbumFlags>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "AlbumFlags Parse albumversion 字段类型不匹配",
                    AlbumVersionFieldIndex));
            }

            PacketResult<int> albumVersion = ParseDecimalField(
                albumVersionField.value,
                AlbumVersionFieldIndex);
            if (!albumVersion.HasValue()) {
                return PacketResult<AlbumFlags>::Failure(albumVersion.Error());
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (dataField.name != "data") {
                return PacketResult<AlbumFlags>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "AlbumFlags Parse data 字段名不匹配",
                    DataFieldIndex));
            }

            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<AlbumFlags>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "AlbumFlags Parse data 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::vector<int>> pages = ParsePages(dataField.value);
            if (!pages.HasValue()) {
                return PacketResult<AlbumFlags>::Failure(pages.Error());
            }

            return PacketResult<AlbumFlags>::Success(AlbumFlags(albumVersion.Value(), std::move(pages.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t AlbumVersionFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;

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

        [[nodiscard]] static PacketResult<std::vector<int>> ParsePages(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<std::vector<int>>::Success({});
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<std::vector<int>>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            if (!parts.empty() && parts.back().empty()) {
                parts.pop_back();
            }

            std::vector<int> pages;
            pages.reserve(parts.size());
            for (std::size_t index = 0; index < parts.size(); ++index) {
                PacketResult<int> page = ParseDecimalField(parts[index], index);
                if (!page.HasValue()) {
                    return PacketResult<std::vector<int>>::Failure(page.Error());
                }
                pages.push_back(page.Value());
            }

            return PacketResult<std::vector<int>>::Success(std::move(pages));
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
                    "AlbumFlags 十进制字段为空",
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
                        "AlbumFlags 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "AlbumFlags 十进制字段超出 int 范围",
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
     * SEND_ALI 图鉴信息		dM {albumId:i62} {imageNumber:i62} {catchFlag:i62} {rare:i62} {type:i62}
     * {vital:i62} {strength:i62} {toughness:i62} {quick:i62} {magic:i62} {earth:i62} {water:i62}
     * {fire:i62} {wind:i62} {slot:i62} {comment:es} {name:es}
     */
    struct AlbumInfo
    {
        static constexpr std::string_view Code = "dM";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        int albumId = 0;
        int imageNumber = 0;
        int catchFlag = 0;
        int rare = 0;
        int type = 0;
        int vital = 0;
        int strength = 0;
        int toughness = 0;
        int quick = 0;
        int magic = 0;
        int earth = 0;
        int water = 0;
        int fire = 0;
        int wind = 0;
        int slot = 0;
        std::string comment;
        std::string name;

        AlbumInfo(
            int albumId,
            int imageNumber,
            int catchFlag,
            int rare,
            int type,
            int vital,
            int strength,
            int toughness,
            int quick,
            int magic,
            int earth,
            int water,
            int fire,
            int wind,
            int slot,
            std::string comment,
            std::string name)
            : albumId(albumId),
              imageNumber(imageNumber),
              catchFlag(catchFlag),
              rare(rare),
              type(type),
              vital(vital),
              strength(strength),
              toughness(toughness),
              quick(quick),
              magic(magic),
              earth(earth),
              water(water),
              fire(fire),
              wind(wind),
              slot(slot),
              comment(std::move(comment)),
              name(std::move(name))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.reserve(FieldCount);

            const std::array<int, NumericFieldCount> values = {
                albumId,
                imageNumber,
                catchFlag,
                rare,
                type,
                vital,
                strength,
                toughness,
                quick,
                magic,
                earth,
                water,
                fire,
                wind,
                slot,
            };

            for (std::size_t index = 0; index < NumericFieldCount; ++index) {
                packet.fields.push_back(PacketField {
                    std::string(FieldNames[index]),
                    PacketFieldType::Int62,
                    std::to_string(values[index]),
                    PacketSourceTag::ServerC,
                });
            }
            packet.fields.push_back(PacketField {
                std::string(FieldNames[CommentFieldIndex]),
                PacketFieldType::EscapedString,
                comment,
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                std::string(FieldNames[NameFieldIndex]),
                PacketFieldType::EscapedString,
                name,
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<AlbumInfo> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<AlbumInfo>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "AlbumInfo Parse 收到非 dM 封包");
                error.packetCode = packet.code;
                return PacketResult<AlbumInfo>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<AlbumInfo>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "AlbumInfo Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<AlbumInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "AlbumInfo Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != FieldNames[index]) {
                    return PacketResult<AlbumInfo>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "AlbumInfo Parse 字段名不匹配",
                        index));
                }

                const PacketFieldType expectedType =
                    index < NumericFieldCount ? PacketFieldType::Int62 : PacketFieldType::EscapedString;
                if (packet.fields[index].type != expectedType) {
                    return PacketResult<AlbumInfo>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "AlbumInfo Parse 字段类型不匹配",
                        index));
                }
            }

            std::array<int, NumericFieldCount> values = {};
            for (std::size_t index = 0; index < NumericFieldCount; ++index) {
                PacketResult<int> value = ParseDecimalField(packet.fields[index].value, index);
                if (!value.HasValue()) {
                    return PacketResult<AlbumInfo>::Failure(value.Error());
                }
                values[index] = value.Value();
            }

            return PacketResult<AlbumInfo>::Success(AlbumInfo(
                values[0],
                values[1],
                values[2],
                values[3],
                values[4],
                values[5],
                values[6],
                values[7],
                values[8],
                values[9],
                values[10],
                values[11],
                values[12],
                values[13],
                values[14],
                packet.fields[CommentFieldIndex].value,
                packet.fields[NameFieldIndex].value));
        }

    private:
        static constexpr std::size_t FieldCount = 17U;
        static constexpr std::size_t NumericFieldCount = 15U;
        static constexpr std::size_t CommentFieldIndex = 15U;
        static constexpr std::size_t NameFieldIndex = 16U;
        static constexpr std::array<std::string_view, FieldCount> FieldNames = {
            "albumId",
            "imageNumber",
            "catchFlag",
            "rare",
            "type",
            "vital",
            "strength",
            "toughness",
            "quick",
            "magic",
            "earth",
            "water",
            "fire",
            "wind",
            "slot",
            "comment",
            "name",
        };

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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
                    "AlbumInfo 十进制字段为空",
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
                        "AlbumInfo 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "AlbumInfo 十进制字段超出 int 范围",
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
     * SEND_ALN 图鉴名称		Dn {albumId:i62} {name:es}
     * name 仅按外层 es 处理，不拆二级 payload。
     */
    struct AlbumName
    {
        static constexpr std::string_view Code = "Dn";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        int albumId = 0;
        std::string name;

        AlbumName(int albumId, std::string name)
            : albumId(albumId),
              name(std::move(name))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "albumId",
                PacketFieldType::Int62,
                std::to_string(albumId),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "name",
                PacketFieldType::EscapedString,
                name,
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<AlbumName> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<AlbumName>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "AlbumName Parse 收到非 Dn 封包");
                error.packetCode = packet.code;
                return PacketResult<AlbumName>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<AlbumName>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "AlbumName Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<AlbumName>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "AlbumName Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& albumIdField = packet.fields[AlbumIdFieldIndex];
            if (albumIdField.name != "albumId") {
                return PacketResult<AlbumName>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "AlbumName Parse albumId 字段名不匹配",
                    AlbumIdFieldIndex));
            }

            if (albumIdField.type != PacketFieldType::Int62) {
                return PacketResult<AlbumName>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "AlbumName Parse albumId 字段类型不匹配",
                    AlbumIdFieldIndex));
            }

            PacketResult<int> albumId = ParseDecimalField(albumIdField.value, AlbumIdFieldIndex);
            if (!albumId.HasValue()) {
                return PacketResult<AlbumName>::Failure(albumId.Error());
            }

            const PacketField& nameField = packet.fields[NameFieldIndex];
            if (nameField.name != "name") {
                return PacketResult<AlbumName>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "AlbumName Parse name 字段名不匹配",
                    NameFieldIndex));
            }

            if (nameField.type != PacketFieldType::EscapedString) {
                return PacketResult<AlbumName>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "AlbumName Parse name 字段类型不匹配",
                    NameFieldIndex));
            }

            return PacketResult<AlbumName>::Success(AlbumName(albumId.Value(), nameField.value));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t AlbumIdFieldIndex = 0U;
        static constexpr std::size_t NameFieldIndex = 1U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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
                    "AlbumName 十进制字段为空",
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
                        "AlbumName 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "AlbumName 十进制字段超出 int 范围",
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
     * SEND_PME 宠邮动画 payload：typeMb|objectIndex|x|y|dir|imageNo|lv|field40|nameRaw|titleDataRaw|flg1|flg2。
     */
    struct PetMailAnimationPayload
    {
        int typeMb = 0;
        int objectIndex = 0;
        int x = 0;
        int y = 0;
        int dir = 0;
        int imageNo = 0;
        int lv = 0;
        int field40 = 0;
        std::string nameRaw;
        std::string titleDataRaw;
        int flg1 = 0;
        int flg2 = 0;
    };

    /**
     * SEND_PME 宠邮动画		lDuJ {objIndex:i62} {graphicsno:i62} {x:i62} {y:i62} {dir:i62} {flg:i62} {no:i62} {data:es}
     * flg 为 0 时忽略 data；非 0 时 data 必须为 12 段 payload，raw 字符串保留 \c/\z/\y/\n。
     */
    struct PetMailAnimation
    {
        static constexpr std::string_view Code = "lDuJ";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int objIndex = 0;
        int graphicsNo = 0;
        int x = 0;
        int y = 0;
        int dir = 0;
        int flg = 0;
        int no = 0;
        std::string data;
        bool hasPayload = false;
        PetMailAnimationPayload payload;

        PetMailAnimation(
            int objIndex,
            int graphicsNo,
            int x,
            int y,
            int dir,
            int flg,
            int no,
            std::string data)
            : objIndex(objIndex),
              graphicsNo(graphicsNo),
              x(x),
              y(y),
              dir(dir),
              flg(flg),
              no(no),
              data(std::move(data))
        {
        }

        PetMailAnimation(
            int objIndex,
            int graphicsNo,
            int x,
            int y,
            int dir,
            int flg,
            int no,
            PetMailAnimationPayload payload)
            : objIndex(objIndex),
              graphicsNo(graphicsNo),
              x(x),
              y(y),
              dir(dir),
              flg(flg),
              no(no),
              hasPayload(true),
              payload(std::move(payload))
        {
            data = BuildData();
        }

        [[nodiscard]] std::string BuildData() const
        {
            if (!hasPayload) {
                return data;
            }

            const std::array<std::string, PayloadFieldCount> values = {
                std::to_string(payload.typeMb),
                EncodeInt62OrEmpty(payload.objectIndex),
                std::to_string(payload.x),
                std::to_string(payload.y),
                std::to_string(payload.dir),
                std::to_string(payload.imageNo),
                std::to_string(payload.lv),
                std::to_string(payload.field40),
                payload.nameRaw,
                payload.titleDataRaw,
                std::to_string(payload.flg1),
                std::to_string(payload.flg2),
            };

            PacketResult<std::string> built = BuildPayload(
                std::span<const std::string>(values.data(), values.size()),
                PayloadDelimiter);
            return built.HasValue() ? built.Value() : std::string();
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.reserve(FieldCount);
            packet.fields.push_back(PacketField {
                "objIndex",
                PacketFieldType::Int62,
                std::to_string(objIndex),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "graphicsno",
                PacketFieldType::Int62,
                std::to_string(graphicsNo),
                PacketSourceTag::Xlsx,
            });
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
                "dir",
                PacketFieldType::Int62,
                std::to_string(dir),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "flg",
                PacketFieldType::Int62,
                std::to_string(flg),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "no",
                PacketFieldType::Int62,
                std::to_string(no),
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

        static PacketResult<PetMailAnimation> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<PetMailAnimation>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "PetMailAnimation Parse 收到非 lDuJ 封包");
                error.packetCode = packet.code;
                return PacketResult<PetMailAnimation>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<PetMailAnimation>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "PetMailAnimation Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<PetMailAnimation>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "PetMailAnimation Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "objIndex",
                "graphicsno",
                "x",
                "y",
                "dir",
                "flg",
                "no",
                "data",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
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
                    return PacketResult<PetMailAnimation>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "PetMailAnimation Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<PetMailAnimation>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "PetMailAnimation Parse 字段类型不匹配",
                        index));
                }
            }

            std::array<int, DataFieldIndex> values = {};
            for (std::size_t index = 0; index < DataFieldIndex; ++index) {
                PacketResult<int> value = ParseDecimalField(packet.fields[index].value, index);
                if (!value.HasValue()) {
                    return PacketResult<PetMailAnimation>::Failure(value.Error());
                }
                values[index] = value.Value();
            }

            const std::string& dataField = packet.fields[DataFieldIndex].value;
            if (values[FlgFieldIndex] == 0) {
                return PacketResult<PetMailAnimation>::Success(PetMailAnimation(
                    values[0],
                    values[1],
                    values[2],
                    values[3],
                    values[4],
                    values[5],
                    values[6],
                    dataField));
            }

            PacketResult<PetMailAnimationPayload> parsedPayload = ParsePayload(dataField);
            if (!parsedPayload.HasValue()) {
                return PacketResult<PetMailAnimation>::Failure(parsedPayload.Error());
            }

            return PacketResult<PetMailAnimation>::Success(PetMailAnimation(
                values[0],
                values[1],
                values[2],
                values[3],
                values[4],
                values[5],
                values[6],
                std::move(parsedPayload.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 8U;
        static constexpr std::size_t FlgFieldIndex = 5U;
        static constexpr std::size_t DataFieldIndex = 7U;
        static constexpr std::size_t PayloadFieldCount = 12U;

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

        [[nodiscard]] static std::string EncodeInt62OrEmpty(int value)
        {
            PacketResult<std::string> encoded = EncodeInt62(value);
            return encoded.HasValue() ? encoded.Value() : std::string();
        }

        [[nodiscard]] static PacketResult<PetMailAnimationPayload> ParsePayload(std::string_view text)
        {
            if (text.empty()) {
                return PacketResult<PetMailAnimationPayload>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "PetMailAnimation payload 为空",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(text, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<PetMailAnimationPayload>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != PayloadFieldCount) {
                return PacketResult<PetMailAnimationPayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "PetMailAnimation payload 段数量不匹配",
                    DataFieldIndex));
            }

            PetMailAnimationPayload parsed;
            PacketResult<int> typeMb = ParseDecimalField(parts[0], 0U);
            if (!typeMb.HasValue()) {
                return PacketResult<PetMailAnimationPayload>::Failure(typeMb.Error());
            }
            PacketResult<int> objectIndex = ParsePayloadObjectIndex(parts[1], 1U);
            if (!objectIndex.HasValue()) {
                return PacketResult<PetMailAnimationPayload>::Failure(objectIndex.Error());
            }
            PacketResult<int> x = ParseDecimalField(parts[2], 2U);
            if (!x.HasValue()) {
                return PacketResult<PetMailAnimationPayload>::Failure(x.Error());
            }
            PacketResult<int> y = ParseDecimalField(parts[3], 3U);
            if (!y.HasValue()) {
                return PacketResult<PetMailAnimationPayload>::Failure(y.Error());
            }
            PacketResult<int> dir = ParseDecimalField(parts[4], 4U);
            if (!dir.HasValue()) {
                return PacketResult<PetMailAnimationPayload>::Failure(dir.Error());
            }
            PacketResult<int> imageNo = ParseDecimalField(parts[5], 5U);
            if (!imageNo.HasValue()) {
                return PacketResult<PetMailAnimationPayload>::Failure(imageNo.Error());
            }
            PacketResult<int> lv = ParseDecimalField(parts[6], 6U);
            if (!lv.HasValue()) {
                return PacketResult<PetMailAnimationPayload>::Failure(lv.Error());
            }
            PacketResult<int> field40 = ParseDecimalField(parts[7], 7U);
            if (!field40.HasValue()) {
                return PacketResult<PetMailAnimationPayload>::Failure(field40.Error());
            }
            PacketResult<int> flg1 = ParseDecimalField(parts[10], 10U);
            if (!flg1.HasValue()) {
                return PacketResult<PetMailAnimationPayload>::Failure(flg1.Error());
            }
            PacketResult<int> flg2 = ParseDecimalField(parts[11], 11U);
            if (!flg2.HasValue()) {
                return PacketResult<PetMailAnimationPayload>::Failure(flg2.Error());
            }

            parsed.typeMb = typeMb.Value();
            parsed.objectIndex = objectIndex.Value();
            parsed.x = x.Value();
            parsed.y = y.Value();
            parsed.dir = dir.Value();
            parsed.imageNo = imageNo.Value();
            parsed.lv = lv.Value();
            parsed.field40 = field40.Value();
            parsed.nameRaw = parts[8];
            parsed.titleDataRaw = parts[9];
            parsed.flg1 = flg1.Value();
            parsed.flg2 = flg2.Value();
            return PacketResult<PetMailAnimationPayload>::Success(std::move(parsed));
        }

        [[nodiscard]] static PacketResult<int> ParsePayloadObjectIndex(
            std::string_view text,
            std::size_t fieldIndex)
        {
            if (text.empty() || text == "-") {
                return PacketResult<int>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PetMailAnimation payload objectIndex 为空",
                    fieldIndex));
            }

            PacketResult<int> decoded = DecodeInt62(text);
            if (!decoded.HasValue()) {
                PacketError error = decoded.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = fieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<int>::Failure(std::move(error));
            }
            return decoded;
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
                    "PetMailAnimation 十进制字段为空",
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
                        "PetMailAnimation 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "PetMailAnimation 十进制字段超出 int 范围",
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

    struct RidePetPayload
    {
        bool clear = false;
        bool fullMarker = false;
        int flags = 0;
        int baseImageNumber = 0;
        int tribe = 0;
        int hp = 0;
        int maxHp = 0;
        int forcePoint = 0;
        int maxForcePoint = 0;
        int exp = 0;
        int nextExp = 0;
        int level = 0;
        int attack = 0;
        int defense = 0;
        int agility = 0;
        int magicPower = 0;
        int recovery = 0;
        int loyalty = 0;
        int earth = 0;
        int water = 0;
        int fire = 0;
        int wind = 0;
        int slot = 0;
        int changeNameFlag = 0;
        std::string name;
        std::string userPetName;
        int departureBattleStatus = 0;
        int injury = 0;

        [[nodiscard]] static RidePetPayload Clear()
        {
            RidePetPayload payload;
            payload.clear = true;
            return payload;
        }

        [[nodiscard]] static RidePetPayload FullMarker()
        {
            RidePetPayload payload;
            payload.fullMarker = true;
            payload.flags = FullMarkerFlag;
            return payload;
        }

        static constexpr int FullMarkerFlag = 1;
    };

    /**
     * SEND_KP CHAR_send_KP_String 骑宠资料		IsSy {petIndex:i62} {data:es}
     * data: literal `0` 表示清空；`1|` 表示 full/all marker；partial 为 `flags|field...|`。
     */
    struct RidePetData
    {
        static constexpr std::string_view Code = "IsSy";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int petIndex = 0;
        RidePetPayload payload;

        RidePetData(int petIndex, RidePetPayload payload)
            : petIndex(petIndex),
              payload(std::move(payload))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            if (payload.clear) {
                return "0";
            }

            if (payload.fullMarker) {
                return "1|";
            }

            PacketResult<std::string> encodedFlags = EncodeInt62(payload.flags);
            std::string data = encodedFlags.HasValue() ? encodedFlags.Value() : std::string();
            data.push_back('|');
            AppendNumberField(data, payload.flags, BaseImageNumberFlag, payload.baseImageNumber);
            AppendNumberField(data, payload.flags, TribeFlag, payload.tribe);
            AppendNumberField(data, payload.flags, HpFlag, payload.hp);
            AppendNumberField(data, payload.flags, MaxHpFlag, payload.maxHp);
            AppendNumberField(data, payload.flags, ForcePointFlag, payload.forcePoint);
            AppendNumberField(data, payload.flags, MaxForcePointFlag, payload.maxForcePoint);
            AppendNumberField(data, payload.flags, ExpFlag, payload.exp);
            AppendNumberField(data, payload.flags, NextExpFlag, payload.nextExp);
            AppendNumberField(data, payload.flags, LevelFlag, payload.level);
            AppendNumberField(data, payload.flags, AttackFlag, payload.attack);
            AppendNumberField(data, payload.flags, DefenseFlag, payload.defense);
            AppendNumberField(data, payload.flags, AgilityFlag, payload.agility);
            AppendNumberField(data, payload.flags, MagicPowerFlag, payload.magicPower);
            AppendNumberField(data, payload.flags, RecoveryFlag, payload.recovery);
            AppendNumberField(data, payload.flags, LoyaltyFlag, payload.loyalty);
            AppendNumberField(data, payload.flags, EarthFlag, payload.earth);
            AppendNumberField(data, payload.flags, WaterFlag, payload.water);
            AppendNumberField(data, payload.flags, FireFlag, payload.fire);
            AppendNumberField(data, payload.flags, WindFlag, payload.wind);
            AppendNumberField(data, payload.flags, SlotFlag, payload.slot);
            AppendNumberField(data, payload.flags, ChangeNameFlag, payload.changeNameFlag);
            AppendStringField(data, payload.flags, NameFlag, payload.name);
            AppendStringField(data, payload.flags, UserPetNameFlag, payload.userPetName);
            AppendNumberField(data, payload.flags, DepartureBattleStatusFlag, payload.departureBattleStatus);
            AppendNumberField(data, payload.flags, InjuryFlag, payload.injury);
            return data;
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "petIndex",
                PacketFieldType::Int62,
                std::to_string(petIndex),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<RidePetData> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetData>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "RidePetData Parse 收到非 IsSy 封包");
                error.packetCode = packet.code;
                return PacketResult<RidePetData>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<RidePetData>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetData Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<RidePetData>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "RidePetData Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& petIndexField = packet.fields[PetIndexFieldIndex];
            if (petIndexField.name != "petIndex") {
                return PacketResult<RidePetData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetData Parse petIndex 字段名不匹配",
                    PetIndexFieldIndex));
            }

            if (petIndexField.type != PacketFieldType::Int62) {
                return PacketResult<RidePetData>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "RidePetData Parse petIndex 字段类型不匹配",
                    PetIndexFieldIndex));
            }

            PacketResult<int> petIndex = ParseDecimalField(petIndexField.value, PetIndexFieldIndex);
            if (!petIndex.HasValue()) {
                return PacketResult<RidePetData>::Failure(petIndex.Error());
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (dataField.name != "data") {
                return PacketResult<RidePetData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetData Parse data 字段名不匹配",
                    DataFieldIndex));
            }

            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<RidePetData>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "RidePetData Parse data 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<RidePetPayload> payload = ParsePayload(dataField.value);
            if (!payload.HasValue()) {
                return PacketResult<RidePetData>::Failure(payload.Error());
            }

            return PacketResult<RidePetData>::Success(RidePetData(petIndex.Value(), std::move(payload.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t PetIndexFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;
        static constexpr int BaseImageNumberFlag = 1 << 1;
        static constexpr int TribeFlag = 1 << 2;
        static constexpr int HpFlag = 1 << 3;
        static constexpr int MaxHpFlag = 1 << 4;
        static constexpr int ForcePointFlag = 1 << 5;
        static constexpr int MaxForcePointFlag = 1 << 6;
        static constexpr int ExpFlag = 1 << 7;
        static constexpr int NextExpFlag = 1 << 8;
        static constexpr int LevelFlag = 1 << 9;
        static constexpr int AttackFlag = 1 << 10;
        static constexpr int DefenseFlag = 1 << 11;
        static constexpr int AgilityFlag = 1 << 12;
        static constexpr int MagicPowerFlag = 1 << 13;
        static constexpr int RecoveryFlag = 1 << 14;
        static constexpr int LoyaltyFlag = 1 << 15;
        static constexpr int EarthFlag = 1 << 16;
        static constexpr int WaterFlag = 1 << 17;
        static constexpr int FireFlag = 1 << 18;
        static constexpr int WindFlag = 1 << 19;
        static constexpr int SlotFlag = 1 << 20;
        static constexpr int ChangeNameFlag = 1 << 21;
        static constexpr int NameFlag = 1 << 22;
        static constexpr int UserPetNameFlag = 1 << 23;
        static constexpr int DepartureBattleStatusFlag = 1 << 24;
        static constexpr int InjuryFlag = 1 << 25;
        static constexpr int PartialFlagsMask = (1 << 26) - 2;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        static void AppendNumberField(std::string& data, int flags, int flag, int value)
        {
            if ((flags & flag) == 0) {
                return;
            }

            data.append(std::to_string(value));
            data.push_back('|');
        }

        static void AppendStringField(std::string& data, int flags, int flag, const std::string& value)
        {
            if ((flags & flag) == 0) {
                return;
            }

            data.append(value);
            data.push_back('|');
        }

        [[nodiscard]] static std::size_t CountPayloadFields(int flags) noexcept
        {
            std::size_t count = 0;
            for (int flag = BaseImageNumberFlag; flag <= InjuryFlag; flag <<= 1) {
                if ((flags & flag) != 0) {
                    ++count;
                }
            }
            return count;
        }

        [[nodiscard]] static PacketResult<RidePetPayload> ParsePayload(std::string_view text)
        {
            if (text.empty()) {
                return PacketResult<RidePetPayload>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "RidePetData payload 为空",
                    DataFieldIndex));
            }

            if (text == "0") {
                return PacketResult<RidePetPayload>::Success(RidePetPayload::Clear());
            }

            if (text.back() != '|') {
                return PacketResult<RidePetPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetData payload 缺少尾随分隔符",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(text, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<RidePetPayload>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            parts.pop_back();
            if (parts.empty() || parts.front().empty() || parts.front() == "-") {
                return PacketResult<RidePetPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetData flags 字段为空",
                    DataFieldIndex));
            }

            PacketResult<int> flags = DecodeInt62(parts.front());
            if (!flags.HasValue()) {
                PacketError error = flags.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<RidePetPayload>::Failure(std::move(error));
            }

            if (flags.Value() < 0) {
                return PacketResult<RidePetPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetData flags 不能为负",
                    DataFieldIndex));
            }

            if (flags.Value() == 0) {
                return PacketResult<RidePetPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetData flags 为 0 但 payload 不是 literal 0",
                    DataFieldIndex));
            }

            if (flags.Value() == RidePetPayload::FullMarkerFlag) {
                if (parts.size() != 1U) {
                    return PacketResult<RidePetPayload>::Failure(MakeFieldError(
                        PacketErrorCode::FieldCountMismatch,
                        "RidePetData full marker 不应携带字段",
                        DataFieldIndex));
                }
                return PacketResult<RidePetPayload>::Success(RidePetPayload::FullMarker());
            }

            if ((flags.Value() & ~PartialFlagsMask) != 0) {
                return PacketResult<RidePetPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetData flags 包含未知 bit",
                    DataFieldIndex));
            }

            const std::size_t expectedPartCount = 1U + CountPayloadFields(flags.Value());
            if (parts.size() != expectedPartCount) {
                return PacketResult<RidePetPayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "RidePetData payload 字段数量不匹配",
                    DataFieldIndex));
            }

            RidePetPayload payload;
            payload.flags = flags.Value();
            std::size_t partIndex = 1U;
            PacketResult<bool> parsed = ParseNumberField(parts, partIndex, flags.Value(), BaseImageNumberFlag, &RidePetPayload::baseImageNumber, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), TribeFlag, &RidePetPayload::tribe, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), HpFlag, &RidePetPayload::hp, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), MaxHpFlag, &RidePetPayload::maxHp, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), ForcePointFlag, &RidePetPayload::forcePoint, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), MaxForcePointFlag, &RidePetPayload::maxForcePoint, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), ExpFlag, &RidePetPayload::exp, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), NextExpFlag, &RidePetPayload::nextExp, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), LevelFlag, &RidePetPayload::level, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), AttackFlag, &RidePetPayload::attack, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), DefenseFlag, &RidePetPayload::defense, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), AgilityFlag, &RidePetPayload::agility, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), MagicPowerFlag, &RidePetPayload::magicPower, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), RecoveryFlag, &RidePetPayload::recovery, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), LoyaltyFlag, &RidePetPayload::loyalty, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), EarthFlag, &RidePetPayload::earth, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), WaterFlag, &RidePetPayload::water, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), FireFlag, &RidePetPayload::fire, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), WindFlag, &RidePetPayload::wind, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), SlotFlag, &RidePetPayload::slot, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), ChangeNameFlag, &RidePetPayload::changeNameFlag, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            ParseStringField(parts, partIndex, flags.Value(), NameFlag, &RidePetPayload::name, payload);
            ParseStringField(parts, partIndex, flags.Value(), UserPetNameFlag, &RidePetPayload::userPetName, payload);
            parsed = ParseNumberField(parts, partIndex, flags.Value(), DepartureBattleStatusFlag, &RidePetPayload::departureBattleStatus, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }
            parsed = ParseNumberField(parts, partIndex, flags.Value(), InjuryFlag, &RidePetPayload::injury, payload);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetPayload>::Failure(parsed.Error());
            }

            return PacketResult<RidePetPayload>::Success(std::move(payload));
        }

        [[nodiscard]] static PacketResult<bool> ParseNumberField(
            const std::vector<std::string>& parts,
            std::size_t& partIndex,
            int flags,
            int flag,
            int RidePetPayload::* member,
            RidePetPayload& payload)
        {
            if ((flags & flag) == 0) {
                return PacketResult<bool>::Success(true);
            }

            PacketResult<int> value = ParseDecimalField(parts[partIndex], partIndex);
            if (!value.HasValue()) {
                return PacketResult<bool>::Failure(value.Error());
            }

            payload.*member = value.Value();
            ++partIndex;
            return PacketResult<bool>::Success(true);
        }

        static void ParseStringField(
            const std::vector<std::string>& parts,
            std::size_t& partIndex,
            int flags,
            int flag,
            std::string RidePetPayload::* member,
            RidePetPayload& payload)
        {
            if ((flags & flag) == 0) {
                return;
            }

            payload.*member = parts[partIndex];
            ++partIndex;
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
                    "RidePetData 十进制字段为空",
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
                        "RidePetData 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "RidePetData 十进制字段超出 int 范围",
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

    struct RidePetExtraPayload
    {
        bool clear = false;
        int flags = 0;
        int vital = 0;
        int strength = 0;
        int tough = 0;
        int quick = 0;
        int magic = 0;
        int critical = 0;
        int counter = 0;
        int hitRate = 0;
        int avoid = 0;
        int poison = 0;
        int sleep = 0;
        int stone = 0;
        int drunk = 0;
        int confusion = 0;
        int amnesia = 0;
        int petSeqNo = 0;
        int weight = 0;
        int maxWeight = 0;
        int itemHave = 0; // unknown，旧协议字段名暂按 itemHave 保留。

        [[nodiscard]] static RidePetExtraPayload Clear()
        {
            RidePetExtraPayload payload;
            payload.clear = true;
            return payload;
        }
    };

    /**
     * SEND_KP2 nrproto_KP2_send 骑宠扩展资料		AU {petIndex:i62} {data:es}
     * data: literal `0` 表示清空；`flags == 1` 为 all marker，但必须携带全部 19 个字段。
     */
    struct RidePetExtraData
    {
        static constexpr std::string_view Code = "AU";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int petIndex = 0;
        RidePetExtraPayload payload;

        RidePetExtraData(int petIndex, RidePetExtraPayload payload)
            : petIndex(petIndex),
              payload(std::move(payload))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            if (payload.clear) {
                return "0";
            }

            PacketResult<std::string> encodedFlags = EncodeInt62(payload.flags);
            std::string data = encodedFlags.HasValue() ? encodedFlags.Value() : std::string();
            data.push_back('|');
            for (const RidePetExtraFieldSpec& field : PayloadFields) {
                if (ShouldAppendField(payload.flags, field.flag)) {
                    data.append(std::to_string(payload.*(field.member)));
                    data.push_back('|');
                }
            }
            return data;
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "petIndex",
                PacketFieldType::Int62,
                std::to_string(petIndex),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<RidePetExtraData> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<RidePetExtraData>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "RidePetExtraData Parse 收到非 AU 封包");
                error.packetCode = packet.code;
                return PacketResult<RidePetExtraData>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<RidePetExtraData>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetExtraData Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<RidePetExtraData>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "RidePetExtraData Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& petIndexField = packet.fields[PetIndexFieldIndex];
            if (petIndexField.name != "petIndex") {
                return PacketResult<RidePetExtraData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetExtraData Parse petIndex 字段名不匹配",
                    PetIndexFieldIndex));
            }

            if (petIndexField.type != PacketFieldType::Int62) {
                return PacketResult<RidePetExtraData>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "RidePetExtraData Parse petIndex 字段类型不匹配",
                    PetIndexFieldIndex));
            }

            PacketResult<int> petIndex = ParseDecimalField(petIndexField.value, PetIndexFieldIndex);
            if (!petIndex.HasValue()) {
                return PacketResult<RidePetExtraData>::Failure(petIndex.Error());
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (dataField.name != "data") {
                return PacketResult<RidePetExtraData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetExtraData Parse data 字段名不匹配",
                    DataFieldIndex));
            }

            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<RidePetExtraData>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "RidePetExtraData Parse data 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<RidePetExtraPayload> payload = ParsePayload(dataField.value);
            if (!payload.HasValue()) {
                return PacketResult<RidePetExtraData>::Failure(payload.Error());
            }

            return PacketResult<RidePetExtraData>::Success(RidePetExtraData(petIndex.Value(), std::move(payload.Value())));
        }

    private:
        struct RidePetExtraFieldSpec
        {
            int flag = 0;
            int RidePetExtraPayload::* member = nullptr;
        };

        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t PetIndexFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;
        static constexpr int AllMarkerFlag = 1;
        static constexpr int VitalFlag = 1 << 1;
        static constexpr int StrengthFlag = 1 << 2;
        static constexpr int ToughFlag = 1 << 3;
        static constexpr int QuickFlag = 1 << 4;
        static constexpr int MagicFlag = 1 << 5;
        static constexpr int CriticalFlag = 1 << 6;
        static constexpr int CounterFlag = 1 << 7;
        static constexpr int HitRateFlag = 1 << 8;
        static constexpr int AvoidFlag = 1 << 9;
        static constexpr int PoisonFlag = 1 << 10;
        static constexpr int SleepFlag = 1 << 11;
        static constexpr int StoneFlag = 1 << 12;
        static constexpr int DrunkFlag = 1 << 13;
        static constexpr int ConfusionFlag = 1 << 14;
        static constexpr int AmnesiaFlag = 1 << 15;
        static constexpr int PetSeqNoFlag = 1 << 16;
        static constexpr int WeightFlag = 1 << 17;
        static constexpr int MaxWeightFlag = 1 << 18;
        static constexpr int ItemHaveFlag = 1 << 19;
        static constexpr int PartialFlagsMask = (1 << 20) - 2;
        static constexpr std::array<RidePetExtraFieldSpec, 19> PayloadFields = {{
            {VitalFlag, &RidePetExtraPayload::vital},
            {StrengthFlag, &RidePetExtraPayload::strength},
            {ToughFlag, &RidePetExtraPayload::tough},
            {QuickFlag, &RidePetExtraPayload::quick},
            {MagicFlag, &RidePetExtraPayload::magic},
            {CriticalFlag, &RidePetExtraPayload::critical},
            {CounterFlag, &RidePetExtraPayload::counter},
            {HitRateFlag, &RidePetExtraPayload::hitRate},
            {AvoidFlag, &RidePetExtraPayload::avoid},
            {PoisonFlag, &RidePetExtraPayload::poison},
            {SleepFlag, &RidePetExtraPayload::sleep},
            {StoneFlag, &RidePetExtraPayload::stone},
            {DrunkFlag, &RidePetExtraPayload::drunk},
            {ConfusionFlag, &RidePetExtraPayload::confusion},
            {AmnesiaFlag, &RidePetExtraPayload::amnesia},
            {PetSeqNoFlag, &RidePetExtraPayload::petSeqNo},
            {WeightFlag, &RidePetExtraPayload::weight},
            {MaxWeightFlag, &RidePetExtraPayload::maxWeight},
            {ItemHaveFlag, &RidePetExtraPayload::itemHave},
        }};

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static bool ShouldAppendField(int flags, int flag) noexcept
        {
            return flags == AllMarkerFlag || (flags & flag) != 0;
        }

        [[nodiscard]] static std::size_t CountPayloadFields(int flags) noexcept
        {
            std::size_t count = 0;
            for (const RidePetExtraFieldSpec& field : PayloadFields) {
                if ((flags & field.flag) != 0) {
                    ++count;
                }
            }
            return count;
        }

        [[nodiscard]] static PacketResult<RidePetExtraPayload> ParsePayload(std::string_view text)
        {
            if (text.empty()) {
                return PacketResult<RidePetExtraPayload>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "RidePetExtraData payload 为空",
                    DataFieldIndex));
            }

            if (text == "0") {
                return PacketResult<RidePetExtraPayload>::Success(RidePetExtraPayload::Clear());
            }

            if (text.back() != '|') {
                return PacketResult<RidePetExtraPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetExtraData payload 缺少尾随分隔符",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(text, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<RidePetExtraPayload>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            parts.pop_back();
            if (parts.empty() || parts.front().empty() || parts.front() == "-") {
                return PacketResult<RidePetExtraPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetExtraData flags 字段为空",
                    DataFieldIndex));
            }

            PacketResult<int> flags = DecodeInt62(parts.front());
            if (!flags.HasValue()) {
                PacketError error = flags.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<RidePetExtraPayload>::Failure(std::move(error));
            }

            if (flags.Value() < 0) {
                return PacketResult<RidePetExtraPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetExtraData flags 不能为负",
                    DataFieldIndex));
            }

            if (flags.Value() == 0) {
                return PacketResult<RidePetExtraPayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "RidePetExtraData flags 为 0 但 payload 不是 literal 0",
                    DataFieldIndex));
            }

            std::size_t expectedPartCount = 0;
            if (flags.Value() == AllMarkerFlag) {
                expectedPartCount = 1U + PayloadFields.size();
            } else {
                if ((flags.Value() & AllMarkerFlag) != 0) {
                    return PacketResult<RidePetExtraPayload>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "RidePetExtraData partial flags 不允许 bit0",
                        DataFieldIndex));
                }
                if ((flags.Value() & ~PartialFlagsMask) != 0) {
                    return PacketResult<RidePetExtraPayload>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "RidePetExtraData flags 包含未知 bit",
                        DataFieldIndex));
                }
                expectedPartCount = 1U + CountPayloadFields(flags.Value());
            }

            if (parts.size() != expectedPartCount) {
                return PacketResult<RidePetExtraPayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "RidePetExtraData payload 字段数量不匹配",
                    DataFieldIndex));
            }

            RidePetExtraPayload payload;
            payload.flags = flags.Value();
            std::size_t partIndex = 1U;
            for (const RidePetExtraFieldSpec& field : PayloadFields) {
                if (!ShouldAppendField(flags.Value(), field.flag)) {
                    continue;
                }

                PacketResult<int> value = ParseDecimalField(parts[partIndex], partIndex);
                if (!value.HasValue()) {
                    return PacketResult<RidePetExtraPayload>::Failure(value.Error());
                }

                payload.*(field.member) = value.Value();
                ++partIndex;
            }

            return PacketResult<RidePetExtraPayload>::Success(std::move(payload));
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
                    "RidePetExtraData 十进制字段为空",
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
                        "RidePetExtraData 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "RidePetExtraData 十进制字段超出 int 范围",
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

    struct TechUsePayload
    {
        int exp = 0;
        int fame = 0;
        int levelUp = 0;
        int stamina = 0;
        int dexterity = 0;
        int intelligence = 0;
        int injuryFlag = 0;
        int itemImageNumber = 0;
        int value = 0; // type=3 语义 unknown，按原字段保留。
        std::vector<std::string> deleteAtomsRaw;
        std::string itemNameRaw;
        bool hasPetHelpFlag = false;
        int petHelpFlag = 0;
    };

    /**
     * SEND_TU techuse 技能使用结果		JgO {result:i62} {type:i62} {data:es}
     * data 内数值为 raw signed decimal；字符串字段保留 raw escaped，不做二次 unescape。
     */
    struct TechUseResult
    {
        static constexpr std::string_view Code = "JgO";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int result = 0;
        int type = 0;
        TechUsePayload payload;

        TechUseResult(int result, int type, TechUsePayload payload)
            : result(result),
              type(type),
              payload(std::move(payload))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::vector<std::string> parts;
            if (type == 1) {
                parts.reserve(8U + payload.deleteAtomsRaw.size());
                AppendBaseStats(parts);
                parts.push_back(std::to_string(payload.injuryFlag));
                parts.push_back(std::to_string(payload.itemImageNumber));
                for (const std::string& atom : payload.deleteAtomsRaw) {
                    parts.push_back(atom);
                }
                return JoinParts(parts);
            }

            if (type == 2) {
                parts.reserve(7U);
                AppendBaseStats(parts);
                parts.push_back(std::to_string(payload.injuryFlag));
                return JoinParts(parts);
            }

            if (type == 3) {
                parts.reserve(7U);
                AppendBaseStats(parts);
                parts.push_back(std::to_string(payload.value));
                return JoinParts(parts);
            }

            if (type == 4) {
                parts.reserve(payload.hasPetHelpFlag ? 9U : 8U);
                AppendBaseStats(parts);
                parts.push_back(std::to_string(payload.itemImageNumber));
                parts.push_back(payload.itemNameRaw);
                if (payload.hasPetHelpFlag) {
                    parts.push_back(std::to_string(payload.petHelpFlag));
                }
                return JoinParts(parts);
            }

            return {};
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "result",
                PacketFieldType::Int62,
                std::to_string(result),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "type",
                PacketFieldType::Int62,
                std::to_string(type),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<TechUseResult> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<TechUseResult>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "TechUseResult Parse 收到非 JgO 封包");
                error.packetCode = packet.code;
                return PacketResult<TechUseResult>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<TechUseResult>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "TechUseResult Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<TechUseResult>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TechUseResult Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "result",
                "type",
                "data",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<TechUseResult>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TechUseResult Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<TechUseResult>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "TechUseResult Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> resultValue = ParseDecimalField(packet.fields[ResultFieldIndex].value, ResultFieldIndex);
            if (!resultValue.HasValue()) {
                return PacketResult<TechUseResult>::Failure(resultValue.Error());
            }

            PacketResult<int> typeValue = ParseDecimalField(packet.fields[TypeFieldIndex].value, TypeFieldIndex);
            if (!typeValue.HasValue()) {
                return PacketResult<TechUseResult>::Failure(typeValue.Error());
            }

            if (typeValue.Value() < 1 || typeValue.Value() > 4) {
                return PacketResult<TechUseResult>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "TechUseResult type 不在 1..4 范围内",
                    TypeFieldIndex));
            }

            PacketResult<TechUsePayload> payloadValue = ParsePayload(typeValue.Value(), packet.fields[DataFieldIndex].value);
            if (!payloadValue.HasValue()) {
                return PacketResult<TechUseResult>::Failure(payloadValue.Error());
            }

            return PacketResult<TechUseResult>::Success(TechUseResult(
                resultValue.Value(),
                typeValue.Value(),
                std::move(payloadValue.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 3U;
        static constexpr std::size_t ResultFieldIndex = 0U;
        static constexpr std::size_t TypeFieldIndex = 1U;
        static constexpr std::size_t DataFieldIndex = 2U;
        static constexpr std::size_t Type1BaseFieldCount = 8U;
        static constexpr std::size_t Type1MaxFieldCount = 14U;
        static constexpr std::size_t Type2FieldCount = 7U;
        static constexpr std::size_t Type3FieldCount = 7U;
        static constexpr std::size_t Type4MinFieldCount = 8U;
        static constexpr std::size_t Type4MaxFieldCount = 9U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        void AppendBaseStats(std::vector<std::string>& parts) const
        {
            parts.push_back(std::to_string(payload.exp));
            parts.push_back(std::to_string(payload.fame));
            parts.push_back(std::to_string(payload.levelUp));
            parts.push_back(std::to_string(payload.stamina));
            parts.push_back(std::to_string(payload.dexterity));
            parts.push_back(std::to_string(payload.intelligence));
        }

        [[nodiscard]] static std::string JoinParts(const std::vector<std::string>& parts)
        {
            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(parts.data(), parts.size()),
                PayloadDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }

            return {};
        }

        [[nodiscard]] static PacketResult<TechUsePayload> ParsePayload(int type, std::string_view text)
        {
            if (text.empty()) {
                return PacketResult<TechUsePayload>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "TechUseResult payload 为空",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(text, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<TechUsePayload>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (type == 1) {
                return ParseType1(parts);
            }
            if (type == 2) {
                return ParseType2(parts);
            }
            if (type == 3) {
                return ParseType3(parts);
            }
            return ParseType4(parts);
        }

        [[nodiscard]] static PacketResult<TechUsePayload> ParseType1(const std::vector<std::string>& parts)
        {
            if (parts.size() < Type1BaseFieldCount || parts.size() > Type1MaxFieldCount) {
                return PacketResult<TechUsePayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TechUseResult type1 payload 字段数量不匹配",
                    DataFieldIndex));
            }

            TechUsePayload payload;
            PacketResult<bool> base = ParseBaseStats(parts, payload);
            if (!base.HasValue()) {
                return PacketResult<TechUsePayload>::Failure(base.Error());
            }

            PacketResult<int> injuryFlag = ParseDecimalField(parts[6], 6U);
            if (!injuryFlag.HasValue()) {
                return PacketResult<TechUsePayload>::Failure(injuryFlag.Error());
            }
            PacketResult<int> itemImageNumber = ParseDecimalField(parts[7], 7U);
            if (!itemImageNumber.HasValue()) {
                return PacketResult<TechUsePayload>::Failure(itemImageNumber.Error());
            }

            payload.injuryFlag = injuryFlag.Value();
            payload.itemImageNumber = itemImageNumber.Value();
            payload.deleteAtomsRaw.reserve(parts.size() - Type1BaseFieldCount);

            bool seenEmptyAtom = false;
            std::size_t nonEmptyAtomCount = 0;
            for (std::size_t index = Type1BaseFieldCount; index < parts.size(); ++index) {
                if (parts[index].empty()) {
                    seenEmptyAtom = true;
                } else {
                    if (seenEmptyAtom) {
                        return PacketResult<TechUsePayload>::Failure(MakeFieldError(
                            PacketErrorCode::DecodeFailed,
                            "TechUseResult type1 deleteAtomRaw 中间空段后仍有内容",
                            index));
                    }
                    ++nonEmptyAtomCount;
                }
                payload.deleteAtomsRaw.push_back(parts[index]);
            }

            if (nonEmptyAtomCount > 6U) {
                return PacketResult<TechUsePayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TechUseResult type1 deleteAtomRaw 非空数量超过 6",
                    DataFieldIndex));
            }

            return PacketResult<TechUsePayload>::Success(std::move(payload));
        }

        [[nodiscard]] static PacketResult<TechUsePayload> ParseType2(const std::vector<std::string>& parts)
        {
            if (parts.size() != Type2FieldCount) {
                return PacketResult<TechUsePayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TechUseResult type2 payload 字段数量不匹配",
                    DataFieldIndex));
            }

            TechUsePayload payload;
            PacketResult<bool> base = ParseBaseStats(parts, payload);
            if (!base.HasValue()) {
                return PacketResult<TechUsePayload>::Failure(base.Error());
            }

            PacketResult<int> injuryFlag = ParseDecimalField(parts[6], 6U);
            if (!injuryFlag.HasValue()) {
                return PacketResult<TechUsePayload>::Failure(injuryFlag.Error());
            }
            payload.injuryFlag = injuryFlag.Value();
            return PacketResult<TechUsePayload>::Success(std::move(payload));
        }

        [[nodiscard]] static PacketResult<TechUsePayload> ParseType3(const std::vector<std::string>& parts)
        {
            if (parts.size() != Type3FieldCount) {
                return PacketResult<TechUsePayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TechUseResult type3 payload 字段数量不匹配",
                    DataFieldIndex));
            }

            TechUsePayload payload;
            PacketResult<bool> base = ParseBaseStats(parts, payload);
            if (!base.HasValue()) {
                return PacketResult<TechUsePayload>::Failure(base.Error());
            }

            PacketResult<int> value = ParseDecimalField(parts[6], 6U);
            if (!value.HasValue()) {
                return PacketResult<TechUsePayload>::Failure(value.Error());
            }
            payload.value = value.Value();
            return PacketResult<TechUsePayload>::Success(std::move(payload));
        }

        [[nodiscard]] static PacketResult<TechUsePayload> ParseType4(const std::vector<std::string>& parts)
        {
            if (parts.size() < Type4MinFieldCount || parts.size() > Type4MaxFieldCount) {
                return PacketResult<TechUsePayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TechUseResult type4 payload 字段数量不匹配",
                    DataFieldIndex));
            }

            TechUsePayload payload;
            PacketResult<bool> base = ParseBaseStats(parts, payload);
            if (!base.HasValue()) {
                return PacketResult<TechUsePayload>::Failure(base.Error());
            }

            PacketResult<int> itemImageNumber = ParseDecimalField(parts[6], 6U);
            if (!itemImageNumber.HasValue()) {
                return PacketResult<TechUsePayload>::Failure(itemImageNumber.Error());
            }
            payload.itemImageNumber = itemImageNumber.Value();
            payload.itemNameRaw = parts[7];

            if (parts.size() == Type4MaxFieldCount) {
                PacketResult<int> petHelpFlag = ParseDecimalField(parts[8], 8U);
                if (!petHelpFlag.HasValue()) {
                    return PacketResult<TechUsePayload>::Failure(petHelpFlag.Error());
                }
                payload.hasPetHelpFlag = true;
                payload.petHelpFlag = petHelpFlag.Value();
            }

            return PacketResult<TechUsePayload>::Success(std::move(payload));
        }

        [[nodiscard]] static PacketResult<bool> ParseBaseStats(
            const std::vector<std::string>& parts,
            TechUsePayload& payload)
        {
            PacketResult<int> exp = ParseDecimalField(parts[0], 0U);
            if (!exp.HasValue()) {
                return PacketResult<bool>::Failure(exp.Error());
            }
            PacketResult<int> fame = ParseDecimalField(parts[1], 1U);
            if (!fame.HasValue()) {
                return PacketResult<bool>::Failure(fame.Error());
            }
            PacketResult<int> levelUp = ParseDecimalField(parts[2], 2U);
            if (!levelUp.HasValue()) {
                return PacketResult<bool>::Failure(levelUp.Error());
            }
            PacketResult<int> stamina = ParseDecimalField(parts[3], 3U);
            if (!stamina.HasValue()) {
                return PacketResult<bool>::Failure(stamina.Error());
            }
            PacketResult<int> dexterity = ParseDecimalField(parts[4], 4U);
            if (!dexterity.HasValue()) {
                return PacketResult<bool>::Failure(dexterity.Error());
            }
            PacketResult<int> intelligence = ParseDecimalField(parts[5], 5U);
            if (!intelligence.HasValue()) {
                return PacketResult<bool>::Failure(intelligence.Error());
            }

            payload.exp = exp.Value();
            payload.fame = fame.Value();
            payload.levelUp = levelUp.Value();
            payload.stamina = stamina.Value();
            payload.dexterity = dexterity.Value();
            payload.intelligence = intelligence.Value();
            return PacketResult<bool>::Success(true);
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
                    "TechUseResult 十进制字段为空",
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
                        "TechUseResult 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TechUseResult 十进制字段超出 int 范围",
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
     * SEND_PT 宠物技能记录：skillSlot|techId|name|description|forcePoint|techField|target|seqNo。
     */
    struct PetTechRecord
    {
        int skillSlot = 0;
        int techId = 0;
        std::string name;
        std::string description;
        int forcePoint = 0;
        int techField = 0;
        int target = 0;
        int seqNo = 0;
    };

    /**
     * SEND_PT 宠物技能列表		OBee {petIndex:i62} {data:es}
     * data: 每条 8 段，数值为 raw decimal，整体保留尾随 `|`；旧 C 无技能不发包，空 data 视为错误。
     */
    struct PetTechData
    {
        static constexpr std::string_view Code = "OBee";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int petIndex = 0;
        std::vector<PetTechRecord> records;

        PetTechData(int petIndex, std::vector<PetTechRecord> records)
            : petIndex(petIndex),
              records(std::move(records))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::string payload;
            for (const PetTechRecord& record : records) {
                payload.append(std::to_string(record.skillSlot));
                payload.push_back('|');
                payload.append(std::to_string(record.techId));
                payload.push_back('|');
                payload.append(record.name);
                payload.push_back('|');
                payload.append(record.description);
                payload.push_back('|');
                payload.append(std::to_string(record.forcePoint));
                payload.push_back('|');
                payload.append(std::to_string(record.techField));
                payload.push_back('|');
                payload.append(std::to_string(record.target));
                payload.push_back('|');
                payload.append(std::to_string(record.seqNo));
                payload.push_back('|');
            }
            return payload;
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "petIndex",
                PacketFieldType::Int62,
                std::to_string(petIndex),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<PetTechData> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<PetTechData>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "PetTechData Parse 收到非 OBee 封包");
                error.packetCode = packet.code;
                return PacketResult<PetTechData>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<PetTechData>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "PetTechData Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<PetTechData>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "PetTechData Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& petIndexField = packet.fields[PetIndexFieldIndex];
            if (petIndexField.name != "petIndex") {
                return PacketResult<PetTechData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PetTechData Parse petIndex 字段名不匹配",
                    PetIndexFieldIndex));
            }

            if (petIndexField.type != PacketFieldType::Int62) {
                return PacketResult<PetTechData>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "PetTechData Parse petIndex 字段类型不匹配",
                    PetIndexFieldIndex));
            }

            PacketResult<int> petIndex = ParseDecimalField(petIndexField.value, PetIndexFieldIndex);
            if (!petIndex.HasValue()) {
                return PacketResult<PetTechData>::Failure(petIndex.Error());
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (dataField.name != "data") {
                return PacketResult<PetTechData>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PetTechData Parse data 字段名不匹配",
                    DataFieldIndex));
            }

            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<PetTechData>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "PetTechData Parse data 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::vector<PetTechRecord>> records = ParseRecords(dataField.value);
            if (!records.HasValue()) {
                return PacketResult<PetTechData>::Failure(records.Error());
            }

            return PacketResult<PetTechData>::Success(PetTechData(petIndex.Value(), std::move(records.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t PetIndexFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;
        static constexpr std::size_t RecordFieldCount = 8U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<std::vector<PetTechRecord>> ParseRecords(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<std::vector<PetTechRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "PetTechData payload 为空",
                    DataFieldIndex));
            }

            if (payload.back() != '|') {
                return PacketResult<std::vector<PetTechRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "PetTechData payload 缺少尾随分隔符",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<std::vector<PetTechRecord>>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            parts.pop_back();
            if (parts.empty() || (parts.size() % RecordFieldCount) != 0U) {
                return PacketResult<std::vector<PetTechRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "PetTechData payload 段数量不是 8 的倍数",
                    DataFieldIndex));
            }

            std::vector<PetTechRecord> records;
            records.reserve(parts.size() / RecordFieldCount);
            for (std::size_t recordIndex = 0; recordIndex < parts.size() / RecordFieldCount; ++recordIndex) {
                const std::size_t baseIndex = recordIndex * RecordFieldCount;
                PacketResult<int> skillSlot = ParseDecimalField(parts[baseIndex], baseIndex);
                if (!skillSlot.HasValue()) {
                    return PacketResult<std::vector<PetTechRecord>>::Failure(skillSlot.Error());
                }

                PacketResult<int> techId = ParseDecimalField(parts[baseIndex + 1U], baseIndex + 1U);
                if (!techId.HasValue()) {
                    return PacketResult<std::vector<PetTechRecord>>::Failure(techId.Error());
                }

                PacketResult<int> forcePoint = ParseDecimalField(parts[baseIndex + 4U], baseIndex + 4U);
                if (!forcePoint.HasValue()) {
                    return PacketResult<std::vector<PetTechRecord>>::Failure(forcePoint.Error());
                }

                PacketResult<int> techField = ParseDecimalField(parts[baseIndex + 5U], baseIndex + 5U);
                if (!techField.HasValue()) {
                    return PacketResult<std::vector<PetTechRecord>>::Failure(techField.Error());
                }

                PacketResult<int> target = ParseDecimalField(parts[baseIndex + 6U], baseIndex + 6U);
                if (!target.HasValue()) {
                    return PacketResult<std::vector<PetTechRecord>>::Failure(target.Error());
                }

                PacketResult<int> seqNo = ParseDecimalField(parts[baseIndex + 7U], baseIndex + 7U);
                if (!seqNo.HasValue()) {
                    return PacketResult<std::vector<PetTechRecord>>::Failure(seqNo.Error());
                }

                records.push_back(PetTechRecord {
                    skillSlot.Value(),
                    techId.Value(),
                    parts[baseIndex + 2U],
                    parts[baseIndex + 3U],
                    forcePoint.Value(),
                    techField.Value(),
                    target.Value(),
                    seqNo.Value(),
                });
            }

            return PacketResult<std::vector<PetTechRecord>>::Success(std::move(records));
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
                    "PetTechData 十进制字段为空",
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
                        "PetTechData 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "PetTechData 十进制字段超出 int 范围",
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
     * SEND_S Profile 状态		Yo {data:es}
     * data 仅按外层 es 保存；旧 C/IDA 未证实二级结构，不拆 payload。
     */
    struct ProfileStatus
    {
        static constexpr std::string_view Code = "Yo";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        std::string data;

        explicit ProfileStatus(std::string data)
            : data(std::move(data))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                data,
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<ProfileStatus> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ProfileStatus>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ProfileStatus Parse 收到非 Yo 封包");
                error.packetCode = packet.code;
                return PacketResult<ProfileStatus>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ProfileStatus>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileStatus Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ProfileStatus>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileStatus Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<ProfileStatus>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileStatus Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<ProfileStatus>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "ProfileStatus Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return PacketResult<ProfileStatus>::Success(ProfileStatus(field.value));
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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
    };

    struct FrontCharRecord
    {
        std::string name;
        int nonPlayerFlag = 0;
    };

    namespace detail
    {
        struct FrontCharListCodec
        {
            static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
            static constexpr std::string_view PayloadDelimiter = "|";
            static constexpr std::size_t FieldCount = 1U;
            static constexpr std::size_t DataFieldIndex = 0U;
            static constexpr std::size_t RecordFieldCount = 2U;

            [[nodiscard]] static PacketResult<std::string> BuildData(
                std::span<const FrontCharRecord> records,
                std::string_view code,
                std::string_view packetName)
            {
                std::string payload;
                for (std::size_t recordIndex = 0; recordIndex < records.size(); ++recordIndex) {
                    const FrontCharRecord& record = records[recordIndex];
                    if (record.nonPlayerFlag != 0 && record.nonPlayerFlag != 1) {
                        return PacketResult<std::string>::Failure(MakeFieldError(
                            code,
                            PacketErrorCode::EncodeFailed,
                            MakeMessage(packetName, "nonPlayerFlag 只能为 0 或 1"),
                            (recordIndex * RecordFieldCount) + 1U));
                    }

                    payload.append(record.name);
                    payload.push_back('|');
                    payload.append(std::to_string(record.nonPlayerFlag));
                    payload.push_back('|');
                }
                return PacketResult<std::string>::Success(std::move(payload));
            }

            [[nodiscard]] static PacketResult<Packet> ToPacket(
                std::span<const FrontCharRecord> records,
                std::string_view code,
                std::string_view packetName)
            {
                PacketResult<std::string> data = BuildData(records, code, packetName);
                if (!data.HasValue()) {
                    return PacketResult<Packet>::Failure(data.Error());
                }

                Packet packet(PacketCode(code), Direction, PacketSourceTag::ServerC);
                packet.fields.push_back(PacketField {
                    "data",
                    PacketFieldType::EscapedString,
                    data.Value(),
                    PacketSourceTag::ServerC,
                });
                return PacketResult<Packet>::Success(std::move(packet));
            }

            [[nodiscard]] static PacketResult<std::vector<FrontCharRecord>> Parse(
                std::string_view view,
                std::string_view code,
                std::string_view packetName)
            {
                PacketResult<Packet> parsed = ParsePacket(view);
                if (!parsed.HasValue()) {
                    return PacketResult<std::vector<FrontCharRecord>>::Failure(parsed.Error());
                }

                const Packet& packet = parsed.Value();
                if (packet.code.View() != code) {
                    PacketError error = MakePacketError(
                        code,
                        PacketErrorCode::InvalidCode,
                        MakeMessage(packetName, "Parse 收到非目标封包"));
                    error.packetCode = packet.code;
                    return PacketResult<std::vector<FrontCharRecord>>::Failure(std::move(error));
                }

                if (packet.direction != Direction) {
                    return PacketResult<std::vector<FrontCharRecord>>::Failure(MakePacketError(
                        code,
                        PacketErrorCode::DecodeFailed,
                        MakeMessage(packetName, "Parse 封包方向不匹配")));
                }

                if (packet.fields.size() != FieldCount) {
                    return PacketResult<std::vector<FrontCharRecord>>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::FieldCountMismatch,
                        MakeMessage(packetName, "Parse 字段数量不匹配"),
                        packet.fields.empty() ? 0U : FieldCount));
                }

                const PacketField& field = packet.fields[DataFieldIndex];
                if (field.name != "data") {
                    return PacketResult<std::vector<FrontCharRecord>>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::DecodeFailed,
                        MakeMessage(packetName, "Parse 字段名不匹配"),
                        DataFieldIndex));
                }

                if (field.type != PacketFieldType::EscapedString) {
                    return PacketResult<std::vector<FrontCharRecord>>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::InvalidFieldType,
                        MakeMessage(packetName, "Parse 字段类型不匹配"),
                        DataFieldIndex));
                }

                return ParseRecords(field.value, code, packetName);
            }

        private:
            [[nodiscard]] static std::string MakeMessage(std::string_view packetName, std::string_view text)
            {
                std::string message(packetName);
                message.push_back(' ');
                message.append(text);
                return message;
            }

            [[nodiscard]] static PacketError MakePacketError(
                std::string_view code,
                PacketErrorCode errorCode,
                std::string message)
            {
                PacketError error = PacketError::Make(errorCode, std::move(message));
                error.packetCode = PacketCode(code);
                error.source = PacketSourceTag::ServerC;
                return error;
            }

            [[nodiscard]] static PacketError MakeFieldError(
                std::string_view code,
                PacketErrorCode errorCode,
                std::string message,
                std::size_t fieldIndex)
            {
                PacketError error = MakePacketError(code, errorCode, std::move(message));
                error.fieldIndex = fieldIndex;
                return error;
            }

            [[nodiscard]] static PacketResult<std::vector<FrontCharRecord>> ParseRecords(
                std::string_view payload,
                std::string_view code,
                std::string_view packetName)
            {
                if (payload.empty()) {
                    return PacketResult<std::vector<FrontCharRecord>>::Success({});
                }

                if (payload.back() != '|') {
                    return PacketResult<std::vector<FrontCharRecord>>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::DecodeFailed,
                        MakeMessage(packetName, "payload 缺少尾随分隔符"),
                        DataFieldIndex));
                }

                PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
                if (!split.HasValue()) {
                    PacketError error = split.Error();
                    error.packetCode = PacketCode(code);
                    error.fieldIndex = DataFieldIndex;
                    error.source = PacketSourceTag::ServerC;
                    return PacketResult<std::vector<FrontCharRecord>>::Failure(std::move(error));
                }

                std::vector<std::string> parts = std::move(split.Value());
                parts.pop_back();
                if ((parts.size() % RecordFieldCount) != 0U) {
                    return PacketResult<std::vector<FrontCharRecord>>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::FieldCountMismatch,
                        MakeMessage(packetName, "payload 段数量不是 2 的倍数"),
                        DataFieldIndex));
                }

                std::vector<FrontCharRecord> records;
                records.reserve(parts.size() / RecordFieldCount);
                for (std::size_t recordIndex = 0; recordIndex < parts.size() / RecordFieldCount; ++recordIndex) {
                    const std::size_t baseIndex = recordIndex * RecordFieldCount;
                    PacketResult<int> nonPlayerFlag =
                        ParseNonPlayerFlag(parts[baseIndex + 1U], baseIndex + 1U, code, packetName);
                    if (!nonPlayerFlag.HasValue()) {
                        return PacketResult<std::vector<FrontCharRecord>>::Failure(nonPlayerFlag.Error());
                    }

                    records.push_back(FrontCharRecord {
                        parts[baseIndex],
                        nonPlayerFlag.Value(),
                    });
                }

                return PacketResult<std::vector<FrontCharRecord>>::Success(std::move(records));
            }

            [[nodiscard]] static PacketResult<int> ParseNonPlayerFlag(
                std::string_view text,
                std::size_t fieldIndex,
                std::string_view code,
                std::string_view packetName)
            {
                if (text.empty()) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::DecodeFailed,
                        MakeMessage(packetName, "nonPlayerFlag 为空"),
                        fieldIndex));
                }

                const std::int64_t maxValue = static_cast<std::int64_t>(std::numeric_limits<int>::max());
                std::int64_t value = 0;
                for (const char ch : text) {
                    if (ch < '0' || ch > '9') {
                        return PacketResult<int>::Failure(MakeFieldError(
                            code,
                            PacketErrorCode::DecodeFailed,
                            MakeMessage(packetName, "nonPlayerFlag 包含非十进制字符"),
                            fieldIndex));
                    }

                    const int digit = ch - '0';
                    if (value > (maxValue - digit) / 10) {
                        return PacketResult<int>::Failure(MakeFieldError(
                            code,
                            PacketErrorCode::DecodeFailed,
                            MakeMessage(packetName, "nonPlayerFlag 超出 int 范围"),
                            fieldIndex));
                    }

                    value = (value * 10) + digit;
                }

                if (value != 0 && value != 1) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::DecodeFailed,
                        MakeMessage(packetName, "nonPlayerFlag 只能为 0 或 1"),
                        fieldIndex));
                }

                return PacketResult<int>::Success(static_cast<int>(value));
            }
        };
    } // namespace detail

    /**
     * SEND_GFL CHAR_sendFrontCharList		ysVc {data:es}
     * data: `name|nonPlayerFlag|` 重复记录，空 data 表示空列表；name 保留 makeEscapeString 后的 raw 字符串。
     */
    struct FrontCharList
    {
        static constexpr std::string_view Code = "ysVc";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        std::vector<FrontCharRecord> records;

        explicit FrontCharList(std::vector<FrontCharRecord> records)
            : records(std::move(records))
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            return detail::FrontCharListCodec::BuildData(records, Code, "FrontCharList");
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            return detail::FrontCharListCodec::ToPacket(records, Code, "FrontCharList");
        }

        static PacketResult<FrontCharList> Parse(std::string_view view)
        {
            PacketResult<std::vector<FrontCharRecord>> records =
                detail::FrontCharListCodec::Parse(view, Code, "FrontCharList");
            if (!records.HasValue()) {
                return PacketResult<FrontCharList>::Failure(records.Error());
            }

            return PacketResult<FrontCharList>::Success(FrontCharList(std::move(records.Value())));
        }
    };

    /**
     * SEND_GFLI CHAR_sendFrontCharList		LY {data:es}
     * data: `name|nonPlayerFlag|` 重复记录，空 data 表示空列表；name 保留 makeEscapeString 后的 raw 字符串。
     */
    struct ItemFrontCharList
    {
        static constexpr std::string_view Code = "LY";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        std::vector<FrontCharRecord> records;

        explicit ItemFrontCharList(std::vector<FrontCharRecord> records)
            : records(std::move(records))
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            return detail::FrontCharListCodec::BuildData(records, Code, "ItemFrontCharList");
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            return detail::FrontCharListCodec::ToPacket(records, Code, "ItemFrontCharList");
        }

        static PacketResult<ItemFrontCharList> Parse(std::string_view view)
        {
            PacketResult<std::vector<FrontCharRecord>> records =
                detail::FrontCharListCodec::Parse(view, Code, "ItemFrontCharList");
            if (!records.HasValue()) {
                return PacketResult<ItemFrontCharList>::Failure(records.Error());
            }

            return PacketResult<ItemFrontCharList>::Success(ItemFrontCharList(std::move(records.Value())));
        }
    };

    struct CharacterPetRecord
    {
        std::string name;
        int level = 0;
        int injury = 0;
        int hp = 0;
        int maxHp = 0;
        int forcePoint = 0;
        int maxForcePoint = 0;
    };

    namespace detail
    {
        struct CharacterPetListCodec
        {
            static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
            static constexpr std::string_view PayloadDelimiter = "|";
            static constexpr std::size_t FieldCount = 1U;
            static constexpr std::size_t DataFieldIndex = 0U;
            static constexpr std::size_t RecordFieldCount = 7U;
            static constexpr std::size_t MaxRecordCount = 6U;

            [[nodiscard]] static PacketResult<std::string> BuildData(
                std::span<const CharacterPetRecord> records,
                std::string_view code,
                std::string_view packetName)
            {
                if (records.size() > MaxRecordCount) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::EncodeFailed,
                        MakeMessage(packetName, "记录数超过 6"),
                        DataFieldIndex));
                }

                std::string payload;
                for (std::size_t recordIndex = 0; recordIndex < records.size(); ++recordIndex) {
                    const CharacterPetRecord& record = records[recordIndex];
                    const std::size_t baseIndex = recordIndex * RecordFieldCount;
                    payload.append(record.name);
                    payload.push_back('|');

                    const std::array<int, 6> values = {{
                        record.level,
                        record.injury,
                        record.hp,
                        record.maxHp,
                        record.forcePoint,
                        record.maxForcePoint,
                    }};
                    for (std::size_t valueIndex = 0; valueIndex < values.size(); ++valueIndex) {
                        PacketResult<int> value =
                            ValidateDecimalField(values[valueIndex], baseIndex + valueIndex + 1U, code, packetName);
                        if (!value.HasValue()) {
                            return PacketResult<std::string>::Failure(value.Error());
                        }

                        payload.append(std::to_string(value.Value()));
                        payload.push_back('|');
                    }
                }
                return PacketResult<std::string>::Success(std::move(payload));
            }

            [[nodiscard]] static PacketResult<Packet> ToPacket(
                std::span<const CharacterPetRecord> records,
                std::string_view code,
                std::string_view packetName)
            {
                PacketResult<std::string> data = BuildData(records, code, packetName);
                if (!data.HasValue()) {
                    return PacketResult<Packet>::Failure(data.Error());
                }

                Packet packet(PacketCode(code), Direction, PacketSourceTag::ServerC);
                packet.fields.push_back(PacketField {
                    "data",
                    PacketFieldType::EscapedString,
                    data.Value(),
                    PacketSourceTag::ServerC,
                });
                return PacketResult<Packet>::Success(std::move(packet));
            }

            [[nodiscard]] static PacketResult<std::vector<CharacterPetRecord>> Parse(
                std::string_view view,
                std::string_view code,
                std::string_view packetName)
            {
                PacketResult<Packet> parsed = ParsePacket(view);
                if (!parsed.HasValue()) {
                    return PacketResult<std::vector<CharacterPetRecord>>::Failure(parsed.Error());
                }

                const Packet& packet = parsed.Value();
                if (packet.code.View() != code) {
                    PacketError error = MakePacketError(
                        code,
                        PacketErrorCode::InvalidCode,
                        MakeMessage(packetName, "Parse 收到非目标封包"));
                    error.packetCode = packet.code;
                    return PacketResult<std::vector<CharacterPetRecord>>::Failure(std::move(error));
                }

                if (packet.direction != Direction) {
                    return PacketResult<std::vector<CharacterPetRecord>>::Failure(MakePacketError(
                        code,
                        PacketErrorCode::DecodeFailed,
                        MakeMessage(packetName, "Parse 封包方向不匹配")));
                }

                if (packet.fields.size() != FieldCount) {
                    return PacketResult<std::vector<CharacterPetRecord>>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::FieldCountMismatch,
                        MakeMessage(packetName, "Parse 字段数量不匹配"),
                        packet.fields.empty() ? 0U : FieldCount));
                }

                const PacketField& field = packet.fields[DataFieldIndex];
                if (field.name != "data") {
                    return PacketResult<std::vector<CharacterPetRecord>>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::DecodeFailed,
                        MakeMessage(packetName, "Parse 字段名不匹配"),
                        DataFieldIndex));
                }

                if (field.type != PacketFieldType::EscapedString) {
                    return PacketResult<std::vector<CharacterPetRecord>>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::InvalidFieldType,
                        MakeMessage(packetName, "Parse 字段类型不匹配"),
                        DataFieldIndex));
                }

                return ParseRecords(field.value, code, packetName);
            }

        private:
            [[nodiscard]] static std::string MakeMessage(std::string_view packetName, std::string_view text)
            {
                std::string message(packetName);
                message.push_back(' ');
                message.append(text);
                return message;
            }

            [[nodiscard]] static PacketError MakePacketError(
                std::string_view code,
                PacketErrorCode errorCode,
                std::string message)
            {
                PacketError error = PacketError::Make(errorCode, std::move(message));
                error.packetCode = PacketCode(code);
                error.source = PacketSourceTag::ServerC;
                return error;
            }

            [[nodiscard]] static PacketError MakeFieldError(
                std::string_view code,
                PacketErrorCode errorCode,
                std::string message,
                std::size_t fieldIndex)
            {
                PacketError error = MakePacketError(code, errorCode, std::move(message));
                error.fieldIndex = fieldIndex;
                return error;
            }

            [[nodiscard]] static PacketResult<std::vector<CharacterPetRecord>> ParseRecords(
                std::string_view payload,
                std::string_view code,
                std::string_view packetName)
            {
                if (payload.empty()) {
                    return PacketResult<std::vector<CharacterPetRecord>>::Success({});
                }

                if (payload.back() != '|') {
                    return PacketResult<std::vector<CharacterPetRecord>>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::DecodeFailed,
                        MakeMessage(packetName, "payload 缺少尾随分隔符"),
                        DataFieldIndex));
                }

                PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
                if (!split.HasValue()) {
                    PacketError error = split.Error();
                    error.packetCode = PacketCode(code);
                    error.fieldIndex = DataFieldIndex;
                    error.source = PacketSourceTag::ServerC;
                    return PacketResult<std::vector<CharacterPetRecord>>::Failure(std::move(error));
                }

                std::vector<std::string> parts = std::move(split.Value());
                parts.pop_back();
                if ((parts.size() % RecordFieldCount) != 0U) {
                    return PacketResult<std::vector<CharacterPetRecord>>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::FieldCountMismatch,
                        MakeMessage(packetName, "payload 段数量不是 7 的倍数"),
                        DataFieldIndex));
                }

                const std::size_t recordCount = parts.size() / RecordFieldCount;
                if (recordCount > MaxRecordCount) {
                    return PacketResult<std::vector<CharacterPetRecord>>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::FieldCountMismatch,
                        MakeMessage(packetName, "payload 记录数超过 6"),
                        DataFieldIndex));
                }

                std::vector<CharacterPetRecord> records;
                records.reserve(recordCount);
                for (std::size_t recordIndex = 0; recordIndex < recordCount; ++recordIndex) {
                    const std::size_t baseIndex = recordIndex * RecordFieldCount;
                    PacketResult<int> level =
                        ParseDecimalField(parts[baseIndex + 1U], baseIndex + 1U, code, packetName);
                    if (!level.HasValue()) {
                        return PacketResult<std::vector<CharacterPetRecord>>::Failure(level.Error());
                    }

                    PacketResult<int> injury =
                        ParseDecimalField(parts[baseIndex + 2U], baseIndex + 2U, code, packetName);
                    if (!injury.HasValue()) {
                        return PacketResult<std::vector<CharacterPetRecord>>::Failure(injury.Error());
                    }

                    PacketResult<int> hp =
                        ParseDecimalField(parts[baseIndex + 3U], baseIndex + 3U, code, packetName);
                    if (!hp.HasValue()) {
                        return PacketResult<std::vector<CharacterPetRecord>>::Failure(hp.Error());
                    }

                    PacketResult<int> maxHp =
                        ParseDecimalField(parts[baseIndex + 4U], baseIndex + 4U, code, packetName);
                    if (!maxHp.HasValue()) {
                        return PacketResult<std::vector<CharacterPetRecord>>::Failure(maxHp.Error());
                    }

                    PacketResult<int> forcePoint =
                        ParseDecimalField(parts[baseIndex + 5U], baseIndex + 5U, code, packetName);
                    if (!forcePoint.HasValue()) {
                        return PacketResult<std::vector<CharacterPetRecord>>::Failure(forcePoint.Error());
                    }

                    PacketResult<int> maxForcePoint =
                        ParseDecimalField(parts[baseIndex + 6U], baseIndex + 6U, code, packetName);
                    if (!maxForcePoint.HasValue()) {
                        return PacketResult<std::vector<CharacterPetRecord>>::Failure(maxForcePoint.Error());
                    }

                    records.push_back(CharacterPetRecord {
                        parts[baseIndex],
                        level.Value(),
                        injury.Value(),
                        hp.Value(),
                        maxHp.Value(),
                        forcePoint.Value(),
                        maxForcePoint.Value(),
                    });
                }

                return PacketResult<std::vector<CharacterPetRecord>>::Success(std::move(records));
            }

            [[nodiscard]] static PacketResult<int> ParseDecimalField(
                std::string_view text,
                std::size_t fieldIndex,
                std::string_view code,
                std::string_view packetName)
            {
                if (text.empty()) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::DecodeFailed,
                        MakeMessage(packetName, "十进制字段为空"),
                        fieldIndex));
                }

                const std::int64_t maxValue = static_cast<std::int64_t>(std::numeric_limits<int>::max());
                std::int64_t value = 0;
                for (const char ch : text) {
                    if (ch < '0' || ch > '9') {
                        return PacketResult<int>::Failure(MakeFieldError(
                            code,
                            PacketErrorCode::DecodeFailed,
                            MakeMessage(packetName, "十进制字段包含非法字符"),
                            fieldIndex));
                    }

                    const int digit = ch - '0';
                    if (value > (maxValue - digit) / 10) {
                        return PacketResult<int>::Failure(MakeFieldError(
                            code,
                            PacketErrorCode::DecodeFailed,
                            MakeMessage(packetName, "十进制字段超出 int 范围"),
                            fieldIndex));
                    }

                    value = (value * 10) + digit;
                }

                return PacketResult<int>::Success(static_cast<int>(value));
            }

            [[nodiscard]] static PacketResult<int> ValidateDecimalField(
                int value,
                std::size_t fieldIndex,
                std::string_view code,
                std::string_view packetName)
            {
                if (value < 0) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        code,
                        PacketErrorCode::EncodeFailed,
                        MakeMessage(packetName, "十进制字段不能为负"),
                        fieldIndex));
                }

                return PacketResult<int>::Success(value);
            }
        };
    } // namespace detail

    /**
     * SEND_GPD CHAR_sendCharAndPetList		ND {data:es}
     * data: `name|level|injury|hp|maxHp|forcePoint|maxForcePoint|`，0..6 条记录；name 保留 raw escaped 字符串。
     */
    struct CharacterPetList
    {
        static constexpr std::string_view Code = "ND";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        std::vector<CharacterPetRecord> records;

        explicit CharacterPetList(std::vector<CharacterPetRecord> records)
            : records(std::move(records))
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            return detail::CharacterPetListCodec::BuildData(records, Code, "CharacterPetList");
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            return detail::CharacterPetListCodec::ToPacket(records, Code, "CharacterPetList");
        }

        static PacketResult<CharacterPetList> Parse(std::string_view view)
        {
            PacketResult<std::vector<CharacterPetRecord>> records =
                detail::CharacterPetListCodec::Parse(view, Code, "CharacterPetList");
            if (!records.HasValue()) {
                return PacketResult<CharacterPetList>::Failure(records.Error());
            }

            return PacketResult<CharacterPetList>::Success(CharacterPetList(std::move(records.Value())));
        }
    };

    /**
     * SEND_GPDI CHAR_sendCharAndPetList		nFV {data:es}
     * data: `name|level|injury|hp|maxHp|forcePoint|maxForcePoint|`，0..6 条记录；name 保留 raw escaped 字符串。
     */
    struct ItemCharacterPetList
    {
        static constexpr std::string_view Code = "nFV";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        std::vector<CharacterPetRecord> records;

        explicit ItemCharacterPetList(std::vector<CharacterPetRecord> records)
            : records(std::move(records))
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            return detail::CharacterPetListCodec::BuildData(records, Code, "ItemCharacterPetList");
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            return detail::CharacterPetListCodec::ToPacket(records, Code, "ItemCharacterPetList");
        }

        static PacketResult<ItemCharacterPetList> Parse(std::string_view view)
        {
            PacketResult<std::vector<CharacterPetRecord>> records =
                detail::CharacterPetListCodec::Parse(view, Code, "ItemCharacterPetList");
            if (!records.HasValue()) {
                return PacketResult<ItemCharacterPetList>::Failure(records.Error());
            }

            return PacketResult<ItemCharacterPetList>::Success(ItemCharacterPetList(std::move(records.Value())));
        }
    };

    enum class ProfileWindowPayloadKind
    {
        Start,
        ProfileList,
        ProfileMail,
    };

    /**
     * SEND_PRW PROFILELIST 记录：seekPos|graphics|name|message。
     */
    struct ProfileWindowListRecord
    {
        bool empty = false;
        int seekPos = 0;
        int graphics = 0;
        std::string name;
        std::string message;
    };

    /**
     * SEND_PRW PROFILELIST payload：categoryId|categoryIndex|20 records|。
     */
    struct ProfileWindowProfileList
    {
        int categoryId = 0;
        int categoryIndex = 0;
        std::array<ProfileWindowListRecord, 20> records = {};
    };

    /**
     * SEND_PRW PROFILEMAIL payload：14 段，无尾随分隔符。
     */
    struct ProfileWindowProfileMail
    {
        int seekPos = 0;
        std::string name;
        int level = 0;
        std::string jobName;
        std::string ownTitle;
        std::string guildName;
        int faceImageNumber = 0;
        int sellFlag = 0;
        std::string sellMessage;
        int buyFlag = 0;
        std::string buyMessage;
        int aboutFlag = 0;
        std::string aboutMessage;
        std::string profileMessage;
    };

    /**
     * SEND_PRW 交流版		DyAG {windowType:i62} {buttonType:i62} {seqNo:i62} {objectIndex:i62} {data:es}
     * data 使用字面 \z 表示逻辑 |；仅解析 START/PROFILELIST/PROFILEMAIL 三种已证 payload。
     */
    struct ProfileWindow
    {
        static constexpr std::string_view Code = "DyAG";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "\\z";

        int windowType = 0;
        int buttonType = 0;
        int seqNo = 0;
        int objectIndex = 0;
        std::string data;
        ProfileWindowPayloadKind payloadKind = ProfileWindowPayloadKind::Start;
        std::vector<int> flags;
        ProfileWindowProfileList profileList;
        ProfileWindowProfileMail profileMail;

        ProfileWindow(
            int windowType,
            int buttonType,
            int seqNo,
            int objectIndex,
            std::vector<int> flags)
            : windowType(windowType),
              buttonType(buttonType),
              seqNo(seqNo),
              objectIndex(objectIndex),
              payloadKind(ProfileWindowPayloadKind::Start),
              flags(std::move(flags))
        {
            data = BuildData();
        }

        ProfileWindow(
            int windowType,
            int buttonType,
            int seqNo,
            int objectIndex,
            ProfileWindowProfileList profileList)
            : windowType(windowType),
              buttonType(buttonType),
              seqNo(seqNo),
              objectIndex(objectIndex),
              payloadKind(ProfileWindowPayloadKind::ProfileList),
              profileList(std::move(profileList))
        {
            data = BuildData();
        }

        ProfileWindow(
            int windowType,
            int buttonType,
            int seqNo,
            int objectIndex,
            ProfileWindowProfileMail profileMail)
            : windowType(windowType),
              buttonType(buttonType),
              seqNo(seqNo),
              objectIndex(objectIndex),
              payloadKind(ProfileWindowPayloadKind::ProfileMail),
              profileMail(std::move(profileMail))
        {
            data = BuildData();
        }

        [[nodiscard]] std::string BuildData() const
        {
            switch (payloadKind) {
            case ProfileWindowPayloadKind::Start:
                return BuildStartData();
            case ProfileWindowPayloadKind::ProfileList:
                return BuildProfileListData();
            case ProfileWindowPayloadKind::ProfileMail:
                return BuildProfileMailData();
            }

            return {};
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.reserve(FieldCount);
            packet.fields.push_back(PacketField {
                "windowType",
                PacketFieldType::Int62,
                std::to_string(windowType),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "buttonType",
                PacketFieldType::Int62,
                std::to_string(buttonType),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "seqNo",
                PacketFieldType::Int62,
                std::to_string(seqNo),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "objectIndex",
                PacketFieldType::Int62,
                std::to_string(objectIndex),
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

        static PacketResult<ProfileWindow> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ProfileWindow Parse 收到非 DyAG 封包");
                error.packetCode = packet.code;
                return PacketResult<ProfileWindow>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ProfileWindow>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileWindow Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ProfileWindow>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileWindow Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "windowType",
                "buttonType",
                "seqNo",
                "objectIndex",
                "data",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<ProfileWindow>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileWindow Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<ProfileWindow>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "ProfileWindow Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> windowTypeValue = ParseDecimalField(
                packet.fields[WindowTypeFieldIndex].value,
                WindowTypeFieldIndex);
            if (!windowTypeValue.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(windowTypeValue.Error());
            }

            PacketResult<int> buttonTypeValue = ParseDecimalField(
                packet.fields[ButtonTypeFieldIndex].value,
                ButtonTypeFieldIndex);
            if (!buttonTypeValue.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(buttonTypeValue.Error());
            }

            PacketResult<int> seqNoValue = ParseDecimalField(packet.fields[SeqNoFieldIndex].value, SeqNoFieldIndex);
            if (!seqNoValue.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(seqNoValue.Error());
            }

            PacketResult<int> objectIndexValue = ParseDecimalField(
                packet.fields[ObjectIndexFieldIndex].value,
                ObjectIndexFieldIndex);
            if (!objectIndexValue.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(objectIndexValue.Error());
            }

            return ParseData(
                windowTypeValue.Value(),
                buttonTypeValue.Value(),
                seqNoValue.Value(),
                objectIndexValue.Value(),
                packet.fields[DataFieldIndex].value);
        }

    private:
        static constexpr std::size_t FieldCount = 5U;
        static constexpr std::size_t WindowTypeFieldIndex = 0U;
        static constexpr std::size_t ButtonTypeFieldIndex = 1U;
        static constexpr std::size_t SeqNoFieldIndex = 2U;
        static constexpr std::size_t ObjectIndexFieldIndex = 3U;
        static constexpr std::size_t DataFieldIndex = 4U;
        static constexpr std::size_t ProfileListRecordCount = 20U;
        static constexpr std::size_t ProfileListRecordFieldCount = 4U;
        static constexpr std::size_t ProfileListPayloadFieldCount =
            2U + (ProfileListRecordCount * ProfileListRecordFieldCount);
        static constexpr std::size_t ProfileListPayloadFieldCountWithTail = ProfileListPayloadFieldCount + 1U;
        static constexpr std::size_t ProfileMailPayloadFieldCount = 14U;

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

        [[nodiscard]] std::string BuildStartData() const
        {
            std::string payload;
            for (const int flag : flags) {
                payload.append(std::to_string(flag));
                payload.append(PayloadDelimiter);
            }
            return payload;
        }

        [[nodiscard]] std::string BuildProfileListData() const
        {
            std::vector<std::string> values;
            values.reserve(ProfileListPayloadFieldCountWithTail);
            values.push_back(std::to_string(profileList.categoryId));
            values.push_back(std::to_string(profileList.categoryIndex));

            for (const ProfileWindowListRecord& record : profileList.records) {
                if (record.empty) {
                    values.push_back("-1");
                    values.emplace_back();
                    values.emplace_back();
                    values.emplace_back();
                    continue;
                }

                values.push_back(std::to_string(record.seekPos));
                values.push_back(std::to_string(record.graphics));
                values.push_back(record.name);
                values.push_back(record.message);
            }

            values.emplace_back();
            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(values.data(), values.size()),
                PayloadDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }

            return {};
        }

        [[nodiscard]] std::string BuildProfileMailData() const
        {
            const std::array<std::string, ProfileMailPayloadFieldCount> values = {
                std::to_string(profileMail.seekPos),
                profileMail.name,
                std::to_string(profileMail.level),
                profileMail.jobName,
                profileMail.ownTitle,
                profileMail.guildName,
                std::to_string(profileMail.faceImageNumber),
                std::to_string(profileMail.sellFlag),
                profileMail.sellMessage,
                std::to_string(profileMail.buyFlag),
                profileMail.buyMessage,
                std::to_string(profileMail.aboutFlag),
                profileMail.aboutMessage,
                profileMail.profileMessage,
            };

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(values.data(), values.size()),
                PayloadDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }

            return {};
        }

        [[nodiscard]] static bool EndsWithDelimiter(std::string_view payload) noexcept
        {
            return payload.size() >= PayloadDelimiter.size() &&
                payload.substr(payload.size() - PayloadDelimiter.size()) == PayloadDelimiter;
        }

        [[nodiscard]] static bool IsDecimalSyntax(std::string_view text) noexcept
        {
            std::size_t index = 0;
            if (!text.empty() && text.front() == '-') {
                index = 1U;
            }

            if (index == text.size()) {
                return false;
            }

            for (; index < text.size(); ++index) {
                const char ch = text[index];
                if (ch < '0' || ch > '9') {
                    return false;
                }
            }

            return true;
        }

        [[nodiscard]] static bool CanParseStart(const std::vector<std::string>& parts) noexcept
        {
            if (parts.size() <= 1U) {
                return false;
            }

            for (std::size_t index = 0; index + 1U < parts.size(); ++index) {
                if (!IsDecimalSyntax(parts[index])) {
                    return false;
                }
            }

            return true;
        }

        [[nodiscard]] static bool IsEmptyProfileListRecord(
            const std::vector<std::string>& parts,
            std::size_t baseIndex) noexcept
        {
            return parts[baseIndex] == "-1" &&
                parts[baseIndex + 1U].empty() &&
                parts[baseIndex + 2U].empty() &&
                parts[baseIndex + 3U].empty();
        }

        [[nodiscard]] static PacketResult<ProfileWindow> ParseData(
            int windowType,
            int buttonType,
            int seqNo,
            int objectIndex,
            std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<ProfileWindow>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "ProfileWindow data 为空",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<ProfileWindow>::Failure(std::move(error));
            }

            const bool hasTrailingDelimiter = EndsWithDelimiter(payload);
            std::vector<std::string> parts = std::move(split.Value());
            if (hasTrailingDelimiter && parts.size() == ProfileListPayloadFieldCountWithTail) {
                return ParseProfileList(windowType, buttonType, seqNo, objectIndex, std::string(payload), parts);
            }

            if (!hasTrailingDelimiter && parts.size() == ProfileMailPayloadFieldCount) {
                return ParseProfileMail(windowType, buttonType, seqNo, objectIndex, std::string(payload), parts);
            }

            if (hasTrailingDelimiter && CanParseStart(parts)) {
                return ParseStart(windowType, buttonType, seqNo, objectIndex, std::string(payload), parts);
            }

            return PacketResult<ProfileWindow>::Failure(MakeFieldError(
                PacketErrorCode::FieldCountMismatch,
                "ProfileWindow data 段数量不匹配",
                DataFieldIndex));
        }

        [[nodiscard]] static PacketResult<ProfileWindow> ParseStart(
            int windowType,
            int buttonType,
            int seqNo,
            int objectIndex,
            std::string originalPayload,
            const std::vector<std::string>& parts)
        {
            std::vector<int> parsedFlags;
            parsedFlags.reserve(parts.size() - 1U);
            for (std::size_t index = 0; index + 1U < parts.size(); ++index) {
                PacketResult<int> flag = ParseDecimalField(parts[index], index);
                if (!flag.HasValue()) {
                    return PacketResult<ProfileWindow>::Failure(flag.Error());
                }
                parsedFlags.push_back(flag.Value());
            }

            ProfileWindow packet(windowType, buttonType, seqNo, objectIndex, std::move(parsedFlags));
            packet.data = std::move(originalPayload);
            return PacketResult<ProfileWindow>::Success(std::move(packet));
        }

        [[nodiscard]] static PacketResult<ProfileWindow> ParseProfileList(
            int windowType,
            int buttonType,
            int seqNo,
            int objectIndex,
            std::string originalPayload,
            const std::vector<std::string>& parts)
        {
            PacketResult<int> categoryId = ParseDecimalField(parts[0], 0U);
            if (!categoryId.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(categoryId.Error());
            }

            PacketResult<int> categoryIndex = ParseDecimalField(parts[1], 1U);
            if (!categoryIndex.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(categoryIndex.Error());
            }

            ProfileWindowProfileList listPayload;
            listPayload.categoryId = categoryId.Value();
            listPayload.categoryIndex = categoryIndex.Value();

            for (std::size_t recordIndex = 0; recordIndex < ProfileListRecordCount; ++recordIndex) {
                const std::size_t baseIndex = 2U + (recordIndex * ProfileListRecordFieldCount);
                ProfileWindowListRecord record;
                if (IsEmptyProfileListRecord(parts, baseIndex)) {
                    record.empty = true;
                    record.seekPos = -1;
                    listPayload.records[recordIndex] = std::move(record);
                    continue;
                }

                PacketResult<int> seekPos = ParseDecimalField(parts[baseIndex], baseIndex);
                if (!seekPos.HasValue()) {
                    return PacketResult<ProfileWindow>::Failure(seekPos.Error());
                }

                PacketResult<int> graphics = ParseDecimalField(parts[baseIndex + 1U], baseIndex + 1U);
                if (!graphics.HasValue()) {
                    return PacketResult<ProfileWindow>::Failure(graphics.Error());
                }

                record.seekPos = seekPos.Value();
                record.graphics = graphics.Value();
                record.name = parts[baseIndex + 2U];
                record.message = parts[baseIndex + 3U];
                listPayload.records[recordIndex] = std::move(record);
            }

            ProfileWindow packet(windowType, buttonType, seqNo, objectIndex, std::move(listPayload));
            packet.data = std::move(originalPayload);
            return PacketResult<ProfileWindow>::Success(std::move(packet));
        }

        [[nodiscard]] static PacketResult<ProfileWindow> ParseProfileMail(
            int windowType,
            int buttonType,
            int seqNo,
            int objectIndex,
            std::string originalPayload,
            const std::vector<std::string>& parts)
        {
            ProfileWindowProfileMail mailPayload;
            PacketResult<int> seekPos = ParseDecimalField(parts[0], 0U);
            if (!seekPos.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(seekPos.Error());
            }
            PacketResult<int> level = ParseDecimalField(parts[2], 2U);
            if (!level.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(level.Error());
            }
            PacketResult<int> faceImageNumber = ParseDecimalField(parts[6], 6U);
            if (!faceImageNumber.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(faceImageNumber.Error());
            }
            PacketResult<int> sellFlag = ParseDecimalField(parts[7], 7U);
            if (!sellFlag.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(sellFlag.Error());
            }
            PacketResult<int> buyFlag = ParseDecimalField(parts[9], 9U);
            if (!buyFlag.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(buyFlag.Error());
            }
            PacketResult<int> aboutFlag = ParseDecimalField(parts[11], 11U);
            if (!aboutFlag.HasValue()) {
                return PacketResult<ProfileWindow>::Failure(aboutFlag.Error());
            }

            mailPayload.seekPos = seekPos.Value();
            mailPayload.name = parts[1];
            mailPayload.level = level.Value();
            mailPayload.jobName = parts[3];
            mailPayload.ownTitle = parts[4];
            mailPayload.guildName = parts[5];
            mailPayload.faceImageNumber = faceImageNumber.Value();
            mailPayload.sellFlag = sellFlag.Value();
            mailPayload.sellMessage = parts[8];
            mailPayload.buyFlag = buyFlag.Value();
            mailPayload.buyMessage = parts[10];
            mailPayload.aboutFlag = aboutFlag.Value();
            mailPayload.aboutMessage = parts[12];
            mailPayload.profileMessage = parts[13];

            ProfileWindow packet(windowType, buttonType, seqNo, objectIndex, std::move(mailPayload));
            packet.data = std::move(originalPayload);
            return PacketResult<ProfileWindow>::Success(std::move(packet));
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
                    "ProfileWindow 十进制字段为空",
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
                        "ProfileWindow 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileWindow 十进制字段超出 int 范围",
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
     * SEND_STK SYSTEMMSG 系统信息		Zd {message:es}
     * message 仅按外层 es 处理，不拆二级 payload。
     */
    struct SystemMessage
    {
        static constexpr std::string_view Code = "Zd";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        std::string message;

        explicit SystemMessage(std::string message)
            : message(std::move(message))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "message",
                PacketFieldType::EscapedString,
                message,
                PacketSourceTag::Xlsx,
            });
            return packet;
        }

        static PacketResult<SystemMessage> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<SystemMessage>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = PacketError::Make(PacketErrorCode::InvalidCode, "SystemMessage Parse 收到非 Zd 封包");
                error.packetCode = packet.code;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<SystemMessage>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                PacketError error = PacketError::Make(PacketErrorCode::DecodeFailed, "SystemMessage Parse 封包方向不匹配");
                error.packetCode = PacketCode(Code);
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<SystemMessage>::Failure(std::move(error));
            }

            if (packet.fields.size() != FieldCount) {
                PacketError error = PacketError::Make(PacketErrorCode::FieldCountMismatch, "SystemMessage Parse 字段数量不匹配");
                error.packetCode = PacketCode(Code);
                error.fieldIndex = packet.fields.empty() ? 0U : FieldCount;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<SystemMessage>::Failure(std::move(error));
            }

            const PacketField& field = packet.fields[MessageFieldIndex];
            if (field.name != "message") {
                PacketError error = PacketError::Make(PacketErrorCode::DecodeFailed, "SystemMessage Parse 字段名不匹配");
                error.packetCode = PacketCode(Code);
                error.fieldIndex = MessageFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<SystemMessage>::Failure(std::move(error));
            }

            if (field.type != PacketFieldType::EscapedString) {
                PacketError error = PacketError::Make(PacketErrorCode::InvalidFieldType, "SystemMessage Parse 字段类型不匹配");
                error.packetCode = PacketCode(Code);
                error.fieldIndex = MessageFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<SystemMessage>::Failure(std::move(error));
            }

            return PacketResult<SystemMessage>::Success(SystemMessage(field.value));
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t MessageFieldIndex = 0U;
    };

    /**
     * SEND_EF 特效封包		zzd {EffType:i62} {TM_EffSize:i62} {data:es}
     * data 为 opaque 外层 es，不拆二级 payload。
     */
    struct Effect
    {
        static constexpr std::string_view Code = "zzd";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        int effType = 0;
        int tmEffSize = 0;
        std::string data;

        Effect(int effType, int tmEffSize, std::string data)
            : effType(effType),
              tmEffSize(tmEffSize),
              data(std::move(data))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "EffType",
                PacketFieldType::Int62,
                std::to_string(effType),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "TM_EffSize",
                PacketFieldType::Int62,
                std::to_string(tmEffSize),
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

        static PacketResult<Effect> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<Effect>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "Effect Parse 收到非 zzd 封包");
                error.packetCode = packet.code;
                return PacketResult<Effect>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<Effect>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "Effect Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<Effect>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "Effect Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "EffType",
                "TM_EffSize",
                "data",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<Effect>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "Effect Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<Effect>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "Effect Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> effTypeValue = ParseDecimalField(packet.fields[EffTypeFieldIndex].value, EffTypeFieldIndex);
            if (!effTypeValue.HasValue()) {
                return PacketResult<Effect>::Failure(effTypeValue.Error());
            }

            PacketResult<int> tmEffSizeValue =
                ParseDecimalField(packet.fields[TmEffSizeFieldIndex].value, TmEffSizeFieldIndex);
            if (!tmEffSizeValue.HasValue()) {
                return PacketResult<Effect>::Failure(tmEffSizeValue.Error());
            }

            return PacketResult<Effect>::Success(Effect(
                effTypeValue.Value(),
                tmEffSizeValue.Value(),
                packet.fields[DataFieldIndex].value));
        }

    private:
        static constexpr std::size_t FieldCount = 3U;
        static constexpr std::size_t EffTypeFieldIndex = 0U;
        static constexpr std::size_t TmEffSizeFieldIndex = 1U;
        static constexpr std::size_t DataFieldIndex = 2U;

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
                    "Effect Parse i62 字段为空",
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
                        "Effect Parse i62 字段包含非十进制字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "Effect Parse i62 字段超出 int 范围",
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
     * SEND_RS 战斗获得物品 Gn {data:es}
     * data: actor0,actor1,actor2,actor3,itemGroup,skill0,...,skill14。
     */
    struct BattleRewardActor
    {
        int targetIndex = 0;
        int levelUpFlag = 0;
        int expDelta = 0;
    };

    struct BattleRewardItem
    {
        bool hasItem = false;
        int imageNumber = 0;
        bool hasNameRaw = false;
        std::string nameRaw;
    };

    struct BattleRewardSkill
    {
        int levelUpFlag = 0;
        int expDelta = 0;
    };

    struct BattleReward
    {
        static constexpr std::string_view Code = "Gn";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view TopLevelDelimiter = ",";
        static constexpr std::string_view SegmentDelimiter = "|";
        static constexpr std::string_view ItemNameMarker = "\\z";
        static constexpr std::size_t ActorCount = 4U;
        static constexpr std::size_t ItemCount = 3U;
        static constexpr std::size_t SkillCount = 15U;

        std::array<BattleRewardActor, ActorCount> actors {};
        std::array<BattleRewardItem, ItemCount> items {};
        std::array<BattleRewardSkill, SkillCount> skills {};

        BattleReward() = default;

        BattleReward(
            std::array<BattleRewardActor, ActorCount> actors,
            std::array<BattleRewardItem, ItemCount> items,
            std::array<BattleRewardSkill, SkillCount> skills)
            : actors(std::move(actors)),
              items(std::move(items)),
              skills(std::move(skills))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::vector<std::string> parts;
            parts.reserve(PayloadFieldCount);

            for (const BattleRewardActor& actor : actors) {
                parts.push_back(BuildActor(actor));
            }
            parts.push_back(BuildItemGroup());
            for (const BattleRewardSkill& skill : skills) {
                parts.push_back(BuildSkill(skill));
            }

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(parts.data(), parts.size()),
                TopLevelDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }
            return {};
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ClientIda);
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ClientIda,
            });
            return packet;
        }

        static PacketResult<BattleReward> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<BattleReward>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<BattleReward> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "BattleReward Parse 收到非 Gn 封包");
                error.packetCode = packet.code;
                return PacketResult<BattleReward>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<BattleReward>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "BattleReward Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<BattleReward>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "BattleReward Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<BattleReward>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "BattleReward Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<BattleReward>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "BattleReward Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t ItemGroupIndex = 4U;
        static constexpr std::size_t SkillStartIndex = 5U;
        static constexpr std::size_t PayloadFieldCount = ActorCount + 1U + SkillCount;
        static constexpr std::size_t ActorFieldCount = 3U;
        static constexpr std::size_t SkillFieldCount = 2U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ClientIda;
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

        [[nodiscard]] static std::string BuildActor(const BattleRewardActor& actor)
        {
            const std::array<std::string, ActorFieldCount> parts = {
                std::to_string(actor.targetIndex),
                std::to_string(actor.levelUpFlag),
                BuildInt62(actor.expDelta),
            };
            return JoinParts(parts);
        }

        [[nodiscard]] std::string BuildItemGroup() const
        {
            const std::array<std::string, ItemCount> parts = {
                BuildItem(items[0]),
                BuildItem(items[1]),
                BuildItem(items[2]),
            };
            return JoinParts(parts);
        }

        [[nodiscard]] static std::string BuildItem(const BattleRewardItem& item)
        {
            if (!item.hasItem) {
                return {};
            }

            std::string data = std::to_string(item.imageNumber);
            if (item.hasNameRaw || !item.nameRaw.empty()) {
                data.append(ItemNameMarker);
                data.append(item.nameRaw);
            }
            return data;
        }

        [[nodiscard]] static std::string BuildSkill(const BattleRewardSkill& skill)
        {
            const std::array<std::string, SkillFieldCount> parts = {
                std::to_string(skill.levelUpFlag),
                BuildInt62(skill.expDelta),
            };
            return JoinParts(parts);
        }

        template <std::size_t Count>
        [[nodiscard]] static std::string JoinParts(const std::array<std::string, Count>& parts)
        {
            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(parts.data(), parts.size()),
                SegmentDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }
            return {};
        }

        [[nodiscard]] static std::string BuildInt62(int value)
        {
            PacketResult<std::string> encoded = EncodeInt62(value);
            if (encoded.HasValue()) {
                return encoded.Value();
            }
            return {};
        }

        [[nodiscard]] static PacketResult<BattleReward> ParseData(std::string_view payload)
        {
            PacketResult<std::vector<std::string>> split = SplitPayload(payload, TopLevelDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ClientIda;
                return PacketResult<BattleReward>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != PayloadFieldCount) {
                return PacketResult<BattleReward>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "BattleReward payload 顶层段数量不匹配",
                    DataFieldIndex));
            }

            BattleReward reward;
            for (std::size_t index = 0; index < ActorCount; ++index) {
                PacketResult<BattleRewardActor> actor = ParseActor(parts[index], index);
                if (!actor.HasValue()) {
                    return PacketResult<BattleReward>::Failure(actor.Error());
                }
                reward.actors[index] = actor.Value();
            }

            PacketResult<std::array<BattleRewardItem, ItemCount>> items = ParseItemGroup(parts[ItemGroupIndex]);
            if (!items.HasValue()) {
                return PacketResult<BattleReward>::Failure(items.Error());
            }
            reward.items = items.Value();

            for (std::size_t index = 0; index < SkillCount; ++index) {
                const std::size_t payloadIndex = SkillStartIndex + index;
                PacketResult<BattleRewardSkill> skill = ParseSkill(parts[payloadIndex], payloadIndex);
                if (!skill.HasValue()) {
                    return PacketResult<BattleReward>::Failure(skill.Error());
                }
                reward.skills[index] = skill.Value();
            }

            return PacketResult<BattleReward>::Success(std::move(reward));
        }

        [[nodiscard]] static PacketResult<BattleRewardActor> ParseActor(
            std::string_view text,
            std::size_t payloadIndex)
        {
            PacketResult<std::vector<std::string>> split = SplitPayload(text, SegmentDelimiter);
            if (!split.HasValue()) {
                return PacketResult<BattleRewardActor>::Failure(split.Error());
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != ActorFieldCount) {
                return PacketResult<BattleRewardActor>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "BattleReward actor 段数量不匹配",
                    payloadIndex));
            }

            PacketResult<int> targetIndex = ParseDecimalField(parts[0], payloadIndex);
            if (!targetIndex.HasValue()) {
                return PacketResult<BattleRewardActor>::Failure(targetIndex.Error());
            }
            PacketResult<int> levelUpFlag = ParseDecimalField(parts[1], payloadIndex);
            if (!levelUpFlag.HasValue()) {
                return PacketResult<BattleRewardActor>::Failure(levelUpFlag.Error());
            }
            PacketResult<int> expDelta = ParseInt62Field(parts[2], payloadIndex);
            if (!expDelta.HasValue()) {
                return PacketResult<BattleRewardActor>::Failure(expDelta.Error());
            }

            return PacketResult<BattleRewardActor>::Success(BattleRewardActor {
                targetIndex.Value(),
                levelUpFlag.Value(),
                expDelta.Value(),
            });
        }

        [[nodiscard]] static PacketResult<std::array<BattleRewardItem, ItemCount>> ParseItemGroup(
            std::string_view text)
        {
            PacketResult<std::vector<std::string>> split = SplitPayload(text, SegmentDelimiter);
            if (!split.HasValue()) {
                return PacketResult<std::array<BattleRewardItem, ItemCount>>::Failure(split.Error());
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != ItemCount) {
                return PacketResult<std::array<BattleRewardItem, ItemCount>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "BattleReward itemGroup 项数量不匹配",
                    ItemGroupIndex));
            }

            std::array<BattleRewardItem, ItemCount> items {};
            for (std::size_t index = 0; index < ItemCount; ++index) {
                PacketResult<BattleRewardItem> item = ParseItem(parts[index]);
                if (!item.HasValue()) {
                    return PacketResult<std::array<BattleRewardItem, ItemCount>>::Failure(item.Error());
                }
                items[index] = item.Value();
            }
            return PacketResult<std::array<BattleRewardItem, ItemCount>>::Success(std::move(items));
        }

        [[nodiscard]] static PacketResult<BattleRewardItem> ParseItem(std::string_view text)
        {
            BattleRewardItem item;
            if (text.empty()) {
                return PacketResult<BattleRewardItem>::Success(std::move(item));
            }

            const std::size_t markerPos = text.find(ItemNameMarker);
            const std::string_view imageText = markerPos == std::string_view::npos
                ? text
                : text.substr(0, markerPos);

            PacketResult<int> imageNumber = ParseDecimalField(imageText, ItemGroupIndex);
            if (!imageNumber.HasValue()) {
                return PacketResult<BattleRewardItem>::Failure(imageNumber.Error());
            }

            item.hasItem = true;
            item.imageNumber = imageNumber.Value();
            item.hasNameRaw = markerPos != std::string_view::npos;
            if (item.hasNameRaw) {
                item.nameRaw = std::string(text.substr(markerPos + ItemNameMarker.size()));
            }
            return PacketResult<BattleRewardItem>::Success(std::move(item));
        }

        [[nodiscard]] static PacketResult<BattleRewardSkill> ParseSkill(
            std::string_view text,
            std::size_t payloadIndex)
        {
            PacketResult<std::vector<std::string>> split = SplitPayload(text, SegmentDelimiter);
            if (!split.HasValue()) {
                return PacketResult<BattleRewardSkill>::Failure(split.Error());
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != SkillFieldCount) {
                return PacketResult<BattleRewardSkill>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "BattleReward skill 段数量不匹配",
                    payloadIndex));
            }

            PacketResult<int> levelUpFlag = ParseDecimalField(parts[0], payloadIndex);
            if (!levelUpFlag.HasValue()) {
                return PacketResult<BattleRewardSkill>::Failure(levelUpFlag.Error());
            }
            PacketResult<int> expDelta = ParseInt62Field(parts[1], payloadIndex);
            if (!expDelta.HasValue()) {
                return PacketResult<BattleRewardSkill>::Failure(expDelta.Error());
            }

            return PacketResult<BattleRewardSkill>::Success(BattleRewardSkill {
                levelUpFlag.Value(),
                expDelta.Value(),
            });
        }

        [[nodiscard]] static PacketResult<int> ParseInt62Field(std::string_view text, std::size_t fieldIndex)
        {
            if (text.empty() || text == "-") {
                return PacketResult<int>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "BattleReward i62 字段为空",
                    fieldIndex));
            }

            PacketResult<int> decoded = DecodeInt62(text);
            if (!decoded.HasValue()) {
                PacketError error = decoded.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = fieldIndex;
                error.source = PacketSourceTag::ClientIda;
                return PacketResult<int>::Failure(std::move(error));
            }
            return decoded;
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
                    "BattleReward 十进制字段为空",
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
                        "BattleReward 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "BattleReward 十进制字段超出 int 范围",
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
     * SEND_RD BATTLE_GetDuelPoint		hB {data:es}
     * data: dpAdd:i62|duelPointNow:i62|，必须保留尾随空段。
     */
    struct DuelPoint
    {
        static constexpr std::string_view Code = "hB";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int dpAdd = 0;
        int duelPointNow = 0;

        DuelPoint(int dpAdd, int duelPointNow)
            : dpAdd(dpAdd),
              duelPointNow(duelPointNow)
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            const std::array<std::string, PayloadFieldCount> parts = {
                BuildInt62(dpAdd),
                BuildInt62(duelPointNow),
                std::string(),
            };

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(parts.data(), parts.size()),
                PayloadDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }
            return {};
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<DuelPoint> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<DuelPoint>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<DuelPoint> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "DuelPoint Parse 收到非 hB 封包");
                error.packetCode = packet.code;
                return PacketResult<DuelPoint>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<DuelPoint>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "DuelPoint Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<DuelPoint>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "DuelPoint Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<DuelPoint>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "DuelPoint Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<DuelPoint>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "DuelPoint Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t PayloadFieldCount = 3U;
        static constexpr std::size_t DpAddIndex = 0U;
        static constexpr std::size_t DuelPointNowIndex = 1U;
        static constexpr std::size_t TailIndex = 2U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static std::string BuildInt62(int value)
        {
            PacketResult<std::string> encoded = EncodeInt62(value);
            if (encoded.HasValue()) {
                return encoded.Value();
            }
            return {};
        }

        [[nodiscard]] static PacketResult<DuelPoint> ParseData(std::string_view payload)
        {
            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<DuelPoint>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != PayloadFieldCount) {
                return PacketResult<DuelPoint>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "DuelPoint payload 段数量不匹配",
                    DataFieldIndex));
            }

            if (!parts[TailIndex].empty()) {
                return PacketResult<DuelPoint>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "DuelPoint payload 缺少尾随空段",
                    DataFieldIndex));
            }

            PacketResult<int> dpAddValue = ParseInt62PayloadField(parts[DpAddIndex], DpAddIndex);
            if (!dpAddValue.HasValue()) {
                return PacketResult<DuelPoint>::Failure(dpAddValue.Error());
            }

            PacketResult<int> duelPointNowValue =
                ParseInt62PayloadField(parts[DuelPointNowIndex], DuelPointNowIndex);
            if (!duelPointNowValue.HasValue()) {
                return PacketResult<DuelPoint>::Failure(duelPointNowValue.Error());
            }

            return PacketResult<DuelPoint>::Success(DuelPoint(dpAddValue.Value(), duelPointNowValue.Value()));
        }

        [[nodiscard]] static PacketResult<int> ParseInt62PayloadField(
            std::string_view text,
            std::size_t fieldIndex)
        {
            if (text.empty()) {
                return PacketResult<int>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "DuelPoint payload i62 字段为空",
                    fieldIndex));
            }

            PacketResult<int> decoded = DecodeInt62(text);
            if (!decoded.HasValue()) {
                PacketError error = decoded.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = fieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<int>::Failure(std::move(error));
            }

            return decoded;
        }
    };

    /**
     * SEND_B 战斗命令		LYIm {command:es}
     * command 为 opaque 战斗命令串，仅按外层 es 处理。
     */
    struct BattleCommand
    {
        static constexpr std::string_view Code = "LYIm";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        std::string command;

        explicit BattleCommand(std::string command)
            : command(std::move(command))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "command",
                PacketFieldType::EscapedString,
                command,
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<BattleCommand> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<BattleCommand>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "BattleCommand Parse 收到非 LYIm 封包");
                error.packetCode = packet.code;
                return PacketResult<BattleCommand>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<BattleCommand>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "BattleCommand Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<BattleCommand>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "BattleCommand Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[CommandFieldIndex];
            if (field.name != "command") {
                return PacketResult<BattleCommand>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "BattleCommand Parse 字段名不匹配",
                    CommandFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<BattleCommand>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "BattleCommand Parse 字段类型不匹配",
                    CommandFieldIndex));
            }

            return PacketResult<BattleCommand>::Success(BattleCommand(field.value));
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t CommandFieldIndex = 0U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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
    };

    /**
     * SEND_MSG recvmsg 名片邮件 payload。
     * 普通邮件 3 段：sentTimeText|messageRaw|-1。
     * 宠邮 6 段：sentTimeText|messageRaw|petBaseImageNumber|petLevel|petNameRaw|itemBaseImageNumber。
     */
    struct ProfileMailMessagePayload
    {
        bool isPetMail = false;
        std::string sentTimeText;
        std::string messageRaw;
        int petBaseImageNumberOrMarker = -1;
        int petLevel = 0;
        std::string petNameRaw;
        int itemBaseImageNumber = 0;
    };

    /**
     * SEND_MSG recvmsg 名片邮件		Mhbt {aindex:i62} {text:es} {color:i62}
     * text 为内部 `|` payload，仅支持普通邮件 3 段与宠邮 6 段。
     */
    struct ProfileMailMessage
    {
        static constexpr std::string_view Code = "Mhbt";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int aindex = 0;
        ProfileMailMessagePayload payload;
        int color = 0;

        ProfileMailMessage(int aindex, ProfileMailMessagePayload payload, int color)
            : aindex(aindex),
              payload(std::move(payload)),
              color(color)
        {
        }

        [[nodiscard]] std::string BuildText() const
        {
            std::string text;
            text.append(payload.sentTimeText);
            text.push_back('|');
            text.append(payload.messageRaw);
            text.push_back('|');
            text.append(std::to_string(payload.petBaseImageNumberOrMarker));
            if (payload.isPetMail) {
                text.push_back('|');
                text.append(std::to_string(payload.petLevel));
                text.push_back('|');
                text.append(payload.petNameRaw);
                text.push_back('|');
                text.append(std::to_string(payload.itemBaseImageNumber));
            }
            return text;
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "aindex",
                PacketFieldType::Int62,
                std::to_string(aindex),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "text",
                PacketFieldType::EscapedString,
                BuildText(),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "color",
                PacketFieldType::Int62,
                std::to_string(color),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<ProfileMailMessage> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ProfileMailMessage>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ProfileMailMessage Parse 收到非 Mhbt 封包");
                error.packetCode = packet.code;
                return PacketResult<ProfileMailMessage>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ProfileMailMessage>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMailMessage Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ProfileMailMessage>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileMailMessage Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "aindex",
                "text",
                "color",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
                PacketFieldType::Int62,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<ProfileMailMessage>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileMailMessage Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<ProfileMailMessage>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "ProfileMailMessage Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> aindexValue = ParseDecimalField(packet.fields[AindexFieldIndex].value, AindexFieldIndex);
            if (!aindexValue.HasValue()) {
                return PacketResult<ProfileMailMessage>::Failure(aindexValue.Error());
            }

            PacketResult<ProfileMailMessagePayload> payloadValue = ParsePayloadText(packet.fields[TextFieldIndex].value);
            if (!payloadValue.HasValue()) {
                return PacketResult<ProfileMailMessage>::Failure(payloadValue.Error());
            }

            PacketResult<int> colorValue = ParseDecimalField(packet.fields[ColorFieldIndex].value, ColorFieldIndex);
            if (!colorValue.HasValue()) {
                return PacketResult<ProfileMailMessage>::Failure(colorValue.Error());
            }

            return PacketResult<ProfileMailMessage>::Success(ProfileMailMessage(
                aindexValue.Value(),
                std::move(payloadValue.Value()),
                colorValue.Value()));
        }

    private:
        static constexpr std::size_t FieldCount = 3U;
        static constexpr std::size_t AindexFieldIndex = 0U;
        static constexpr std::size_t TextFieldIndex = 1U;
        static constexpr std::size_t ColorFieldIndex = 2U;
        static constexpr std::size_t NormalPayloadFieldCount = 3U;
        static constexpr std::size_t PetPayloadFieldCount = 6U;
        static constexpr std::size_t PetBaseImageFieldIndex = 2U;
        static constexpr std::size_t PetLevelFieldIndex = 3U;
        static constexpr std::size_t ItemBaseImageFieldIndex = 5U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<ProfileMailMessagePayload> ParsePayloadText(std::string_view text)
        {
            PacketResult<std::vector<std::string>> split = SplitPayload(text, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = TextFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<ProfileMailMessagePayload>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != NormalPayloadFieldCount && parts.size() != PetPayloadFieldCount) {
                return PacketResult<ProfileMailMessagePayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ProfileMailMessage payload 段数量不匹配",
                    TextFieldIndex));
            }

            PacketResult<int> petBaseImageNumberOrMarker = ParseDecimalField(
                parts[PetBaseImageFieldIndex],
                PetBaseImageFieldIndex);
            if (!petBaseImageNumberOrMarker.HasValue()) {
                return PacketResult<ProfileMailMessagePayload>::Failure(petBaseImageNumberOrMarker.Error());
            }

            if (parts.size() == NormalPayloadFieldCount && petBaseImageNumberOrMarker.Value() != -1) {
                return PacketResult<ProfileMailMessagePayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ProfileMailMessage 普通邮件标记必须为 -1",
                    PetBaseImageFieldIndex));
            }

            ProfileMailMessagePayload payload;
            payload.isPetMail = parts.size() == PetPayloadFieldCount;
            payload.sentTimeText = parts[0];
            payload.messageRaw = parts[1];
            payload.petBaseImageNumberOrMarker = petBaseImageNumberOrMarker.Value();

            if (payload.isPetMail) {
                PacketResult<int> petLevel = ParseDecimalField(parts[PetLevelFieldIndex], PetLevelFieldIndex);
                if (!petLevel.HasValue()) {
                    return PacketResult<ProfileMailMessagePayload>::Failure(petLevel.Error());
                }

                PacketResult<int> itemBaseImageNumber = ParseDecimalField(
                    parts[ItemBaseImageFieldIndex],
                    ItemBaseImageFieldIndex);
                if (!itemBaseImageNumber.HasValue()) {
                    return PacketResult<ProfileMailMessagePayload>::Failure(itemBaseImageNumber.Error());
                }

                payload.petLevel = petLevel.Value();
                payload.petNameRaw = parts[4];
                payload.itemBaseImageNumber = itemBaseImageNumber.Value();
            }

            return PacketResult<ProfileMailMessagePayload>::Success(std::move(payload));
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
                    "ProfileMailMessage 十进制字段为空",
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
                        "ProfileMailMessage 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ProfileMailMessage 十进制字段超出 int 范围",
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

    struct ChatMessagePayload
    {
        std::string textRaw;
    };

    /**
     * SEND_TK 聊天消息		zdr {objectIndex:i62} {message:es} {color:i62} {fontSize:i62}
     * message: `P|textRaw`，textRaw 保留 raw escaped 内容，不做业务显示还原。
     */
    struct ChatMessage
    {
        static constexpr std::string_view Code = "zdr";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int objectIndex = 0;
        ChatMessagePayload payload;
        int color = 0;
        int fontSize = 0;

        ChatMessage(int objectIndex, ChatMessagePayload payload, int color, int fontSize)
            : objectIndex(objectIndex),
              payload(std::move(payload)),
              color(color),
              fontSize(fontSize)
        {
        }

        [[nodiscard]] std::string BuildMessage() const
        {
            std::string message = "P|";
            message.append(payload.textRaw);
            return message;
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "objectIndex",
                PacketFieldType::Int62,
                std::to_string(objectIndex),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "message",
                PacketFieldType::EscapedString,
                BuildMessage(),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "color",
                PacketFieldType::Int62,
                std::to_string(color),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "fontSize",
                PacketFieldType::Int62,
                std::to_string(fontSize),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<ChatMessage> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<ChatMessage>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "ChatMessage Parse 收到非 zdr 封包");
                error.packetCode = packet.code;
                return PacketResult<ChatMessage>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<ChatMessage>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "ChatMessage Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<ChatMessage>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ChatMessage Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "objectIndex",
                "message",
                "color",
                "fontSize",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
                PacketFieldType::Int62,
                PacketFieldType::Int62,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<ChatMessage>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ChatMessage Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<ChatMessage>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "ChatMessage Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> objectIndexValue = ParseDecimalField(
                packet.fields[ObjectIndexFieldIndex].value,
                ObjectIndexFieldIndex);
            if (!objectIndexValue.HasValue()) {
                return PacketResult<ChatMessage>::Failure(objectIndexValue.Error());
            }

            PacketResult<ChatMessagePayload> payloadValue = ParseMessage(packet.fields[MessageFieldIndex].value);
            if (!payloadValue.HasValue()) {
                return PacketResult<ChatMessage>::Failure(payloadValue.Error());
            }

            PacketResult<int> colorValue = ParseDecimalField(packet.fields[ColorFieldIndex].value, ColorFieldIndex);
            if (!colorValue.HasValue()) {
                return PacketResult<ChatMessage>::Failure(colorValue.Error());
            }

            PacketResult<int> fontSizeValue = ParseDecimalField(
                packet.fields[FontSizeFieldIndex].value,
                FontSizeFieldIndex);
            if (!fontSizeValue.HasValue()) {
                return PacketResult<ChatMessage>::Failure(fontSizeValue.Error());
            }

            return PacketResult<ChatMessage>::Success(ChatMessage(
                objectIndexValue.Value(),
                std::move(payloadValue.Value()),
                colorValue.Value(),
                fontSizeValue.Value()));
        }

    private:
        static constexpr std::size_t FieldCount = 4U;
        static constexpr std::size_t ObjectIndexFieldIndex = 0U;
        static constexpr std::size_t MessageFieldIndex = 1U;
        static constexpr std::size_t ColorFieldIndex = 2U;
        static constexpr std::size_t FontSizeFieldIndex = 3U;
        static constexpr std::size_t PayloadFieldCount = 2U;
        static constexpr std::size_t MarkerPayloadIndex = 0U;
        static constexpr std::size_t TextPayloadIndex = 1U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<ChatMessagePayload> ParseMessage(std::string_view message)
        {
            PacketResult<std::vector<std::string>> split = SplitPayload(message, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = MessageFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<ChatMessagePayload>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != PayloadFieldCount) {
                return PacketResult<ChatMessagePayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "ChatMessage payload 段数量不匹配",
                    MessageFieldIndex));
            }

            if (parts[MarkerPayloadIndex] != "P") {
                return PacketResult<ChatMessagePayload>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "ChatMessage payload marker 必须为 P",
                    MessageFieldIndex));
            }

            ChatMessagePayload payload;
            payload.textRaw = parts[TextPayloadIndex];
            return PacketResult<ChatMessagePayload>::Success(std::move(payload));
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
                    "ChatMessage i62 字段为空",
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
                        "ChatMessage i62 字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "ChatMessage i62 字段超出 int 范围",
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
     * SEND_WN 窗口封包		BZ {windowtype:i62} {buttontype:i62} {seqno:i62} {objindex:i62} {data:es}
     * data 为 opaque 外层 es，不拆窗口业务 payload。
     */
    struct WindowPacket
    {
        static constexpr std::string_view Code = "BZ";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        int windowtype = 0;
        int buttontype = 0;
        int seqno = 0;
        int objindex = 0;
        std::string data;

        WindowPacket(int windowtype, int buttontype, int seqno, int objindex, std::string data)
            : windowtype(windowtype),
              buttontype(buttontype),
              seqno(seqno),
              objindex(objindex),
              data(std::move(data))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "windowtype",
                PacketFieldType::Int62,
                std::to_string(windowtype),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "buttontype",
                PacketFieldType::Int62,
                std::to_string(buttontype),
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
                "data",
                PacketFieldType::EscapedString,
                data,
                PacketSourceTag::Xlsx,
            });
            return packet;
        }

        static PacketResult<WindowPacket> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<WindowPacket>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "WindowPacket Parse 收到非 BZ 封包");
                error.packetCode = packet.code;
                return PacketResult<WindowPacket>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<WindowPacket>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "WindowPacket Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<WindowPacket>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "WindowPacket Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "windowtype",
                "buttontype",
                "seqno",
                "objindex",
                "data",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<WindowPacket>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "WindowPacket Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<WindowPacket>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "WindowPacket Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> windowtypeValue = ParseDecimalField(
                packet.fields[WindowTypeFieldIndex].value,
                WindowTypeFieldIndex);
            if (!windowtypeValue.HasValue()) {
                return PacketResult<WindowPacket>::Failure(windowtypeValue.Error());
            }

            PacketResult<int> buttontypeValue = ParseDecimalField(
                packet.fields[ButtonTypeFieldIndex].value,
                ButtonTypeFieldIndex);
            if (!buttontypeValue.HasValue()) {
                return PacketResult<WindowPacket>::Failure(buttontypeValue.Error());
            }

            PacketResult<int> seqnoValue = ParseDecimalField(packet.fields[SeqNoFieldIndex].value, SeqNoFieldIndex);
            if (!seqnoValue.HasValue()) {
                return PacketResult<WindowPacket>::Failure(seqnoValue.Error());
            }

            PacketResult<int> objindexValue = ParseDecimalField(
                packet.fields[ObjIndexFieldIndex].value,
                ObjIndexFieldIndex);
            if (!objindexValue.HasValue()) {
                return PacketResult<WindowPacket>::Failure(objindexValue.Error());
            }

            return PacketResult<WindowPacket>::Success(WindowPacket(
                windowtypeValue.Value(),
                buttontypeValue.Value(),
                seqnoValue.Value(),
                objindexValue.Value(),
                packet.fields[DataFieldIndex].value));
        }

    private:
        static constexpr std::size_t FieldCount = 5U;
        static constexpr std::size_t WindowTypeFieldIndex = 0U;
        static constexpr std::size_t ButtonTypeFieldIndex = 1U;
        static constexpr std::size_t SeqNoFieldIndex = 2U;
        static constexpr std::size_t ObjIndexFieldIndex = 3U;
        static constexpr std::size_t DataFieldIndex = 4U;

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
                    "WindowPacket Parse i62 字段为空",
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
                        "WindowPacket Parse i62 字段包含非十进制字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "WindowPacket Parse i62 字段超出 int 范围",
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
     * SEND_StallBrowse 摊位浏览		ft {result:i62} {objIndex:i62} {name:es} {description:es} {item:es} {pet:es}
     * item/pet 为内嵌 `|` payload，本层仅保留外层 es 解码后的 raw 字符串。
     */
    struct StallBrowse
    {
        static constexpr std::string_view Code = "ft";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        int result = 0;
        int objIndex = 0;
        std::string name;
        std::string description;
        std::string item;
        std::string pet;

        StallBrowse(
            int result,
            int objIndex,
            std::string name,
            std::string description,
            std::string item,
            std::string pet)
            : result(result),
              objIndex(objIndex),
              name(std::move(name)),
              description(std::move(description)),
              item(std::move(item)),
              pet(std::move(pet))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "result",
                PacketFieldType::Int62,
                std::to_string(result),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "objIndex",
                PacketFieldType::Int62,
                std::to_string(objIndex),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "name",
                PacketFieldType::EscapedString,
                name,
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "description",
                PacketFieldType::EscapedString,
                description,
                PacketSourceTag::Xlsx,
            });
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
            return packet;
        }

        static PacketResult<StallBrowse> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<StallBrowse>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "StallBrowse Parse 收到非 ft 封包");
                error.packetCode = packet.code;
                return PacketResult<StallBrowse>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<StallBrowse>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "StallBrowse Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<StallBrowse>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "StallBrowse Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "result",
                "objIndex",
                "name",
                "description",
                "item",
                "pet",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
                PacketFieldType::EscapedString,
                PacketFieldType::EscapedString,
                PacketFieldType::EscapedString,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<StallBrowse>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "StallBrowse Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<StallBrowse>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "StallBrowse Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> resultValue = ParseDecimalField(packet.fields[ResultFieldIndex].value, ResultFieldIndex);
            if (!resultValue.HasValue()) {
                return PacketResult<StallBrowse>::Failure(resultValue.Error());
            }

            PacketResult<int> objIndexValue = ParseDecimalField(
                packet.fields[ObjIndexFieldIndex].value,
                ObjIndexFieldIndex);
            if (!objIndexValue.HasValue()) {
                return PacketResult<StallBrowse>::Failure(objIndexValue.Error());
            }

            return PacketResult<StallBrowse>::Success(StallBrowse(
                resultValue.Value(),
                objIndexValue.Value(),
                packet.fields[NameFieldIndex].value,
                packet.fields[DescriptionFieldIndex].value,
                packet.fields[ItemFieldIndex].value,
                packet.fields[PetFieldIndex].value));
        }

    private:
        static constexpr std::size_t FieldCount = 6U;
        static constexpr std::size_t ResultFieldIndex = 0U;
        static constexpr std::size_t ObjIndexFieldIndex = 1U;
        static constexpr std::size_t NameFieldIndex = 2U;
        static constexpr std::size_t DescriptionFieldIndex = 3U;
        static constexpr std::size_t ItemFieldIndex = 4U;
        static constexpr std::size_t PetFieldIndex = 5U;

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
                    "StallBrowse Parse i62 字段为空",
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
                        "StallBrowse Parse i62 字段包含非十进制字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "StallBrowse Parse i62 字段超出 int 范围",
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
     * SEND_CJ SendJobName		jv {jobassort:i62} {name:es}
     */
    struct JobName
    {
        static constexpr std::string_view Code = "jv";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        int jobAssort = 0;
        std::string name;

        JobName(int jobAssort, std::string name)
            : jobAssort(jobAssort),
              name(std::move(name))
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "jobassort",
                PacketFieldType::Int62,
                std::to_string(jobAssort),
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

        static PacketResult<JobName> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<JobName>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "JobName Parse 收到非 jv 封包");
                error.packetCode = packet.code;
                return PacketResult<JobName>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<JobName>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "JobName Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<JobName>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "JobName Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "jobassort",
                "name",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<JobName>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "JobName Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<JobName>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "JobName Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> jobAssortValue = ParseDecimalField(
                packet.fields[JobAssortFieldIndex].value,
                JobAssortFieldIndex);
            if (!jobAssortValue.HasValue()) {
                return PacketResult<JobName>::Failure(jobAssortValue.Error());
            }

            return PacketResult<JobName>::Success(JobName(
                jobAssortValue.Value(),
                packet.fields[NameFieldIndex].value));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t JobAssortFieldIndex = 0U;
        static constexpr std::size_t NameFieldIndex = 1U;

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
                    "JobName Parse i62 字段为空",
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
                        "JobName Parse i62 字段包含非十进制字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "JobName Parse i62 字段超出 int 范围",
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

    struct AddressBookSlot
    {
        int use = 0;
        std::string charNameRaw;
        std::string ownTitleRaw;
        int registSeqNumber = 0;
        int level = 0;
        int online = 0;
        int graphicsNo = 0;

        [[nodiscard]] bool IsUsed() const noexcept
        {
            return use > 0;
        }
    };

    /**
     * SEND_AB SendAddressbook		gnxd {data:es}
     * data: 固定 60 槽 AddressBook；每槽 use|charNameRaw|ownTitleRaw|registSeqNumber|level|online|graphicsNo。
     */
    struct AddressBook
    {
        static constexpr std::string_view Code = "gnxd";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";
        static constexpr std::size_t SlotCount = 60U;
        static constexpr std::size_t SlotFieldCount = 7U;
        static constexpr std::size_t PayloadFieldCount = SlotCount * SlotFieldCount;

        std::array<AddressBookSlot, SlotCount> slots = {};

        AddressBook() = default;

        explicit AddressBook(std::array<AddressBookSlot, SlotCount> slots)
            : slots(std::move(slots))
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            std::array<std::string, PayloadFieldCount> parts = {};
            for (std::size_t slotIndex = 0; slotIndex < slots.size(); ++slotIndex) {
                const AddressBookSlot& slot = slots[slotIndex];
                const std::size_t baseIndex = slotIndex * SlotFieldCount;
                if (!SlotHasBuildData(slot)) {
                    continue;
                }

                if (slot.use <= 0) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::EncodeFailed,
                        "AddressBook BuildData use 必须大于 0",
                        baseIndex));
                }

                parts[baseIndex] = std::to_string(slot.use);
                parts[baseIndex + 1U] = slot.charNameRaw;
                parts[baseIndex + 2U] = slot.ownTitleRaw;
                parts[baseIndex + 3U] = std::to_string(slot.registSeqNumber);
                parts[baseIndex + 4U] = std::to_string(slot.level);
                parts[baseIndex + 5U] = std::to_string(slot.online);
                parts[baseIndex + 6U] = std::to_string(slot.graphicsNo);
            }

            return BuildPayload(std::span<const std::string>(parts.data(), parts.size()), PayloadDelimiter);
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
            return PacketResult<Packet>::Success(std::move(packet));
        }

        static PacketResult<AddressBook> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<AddressBook>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "AddressBook Parse 收到非 gnxd 封包");
                error.packetCode = packet.code;
                return PacketResult<AddressBook>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<AddressBook>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "AddressBook Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<AddressBook>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "AddressBook Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<AddressBook>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "AddressBook Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<AddressBook>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "AddressBook Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;

        [[nodiscard]] static bool SlotHasBuildData(const AddressBookSlot& slot) noexcept
        {
            return slot.use != 0 ||
                !slot.charNameRaw.empty() ||
                !slot.ownTitleRaw.empty() ||
                slot.registSeqNumber != 0 ||
                slot.level != 0 ||
                slot.online != 0 ||
                slot.graphicsNo != 0;
        }

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

        [[nodiscard]] static PacketResult<AddressBook> ParseData(std::string_view payload)
        {
            PacketResult<std::vector<std::string>> split = SplitPayloadLegacyAware(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<AddressBook>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            if (parts.size() == PayloadFieldCount + 1U && !parts.empty() && parts.back().empty()) {
                parts.pop_back();
            }

            if (parts.size() != PayloadFieldCount) {
                return PacketResult<AddressBook>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "AddressBook payload 段数量不匹配",
                    DataFieldIndex));
            }

            std::array<AddressBookSlot, SlotCount> slots = {};
            for (std::size_t slotIndex = 0; slotIndex < SlotCount; ++slotIndex) {
                const std::size_t baseIndex = slotIndex * SlotFieldCount;
                if (SlotPartsEmpty(parts, baseIndex)) {
                    continue;
                }

                if (parts[baseIndex].empty()) {
                    return PacketResult<AddressBook>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "AddressBook payload use 为空但槽位非空",
                        baseIndex));
                }

                PacketResult<int> use = ParseDecimalField(parts[baseIndex], baseIndex);
                if (!use.HasValue()) {
                    return PacketResult<AddressBook>::Failure(use.Error());
                }
                if (use.Value() <= 0) {
                    return PacketResult<AddressBook>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "AddressBook payload use 必须大于 0",
                        baseIndex));
                }

                PacketResult<int> registSeqNumber = ParseDecimalField(parts[baseIndex + 3U], baseIndex + 3U);
                if (!registSeqNumber.HasValue()) {
                    return PacketResult<AddressBook>::Failure(registSeqNumber.Error());
                }

                PacketResult<int> level = ParseDecimalField(parts[baseIndex + 4U], baseIndex + 4U);
                if (!level.HasValue()) {
                    return PacketResult<AddressBook>::Failure(level.Error());
                }

                PacketResult<int> online = ParseDecimalField(parts[baseIndex + 5U], baseIndex + 5U);
                if (!online.HasValue()) {
                    return PacketResult<AddressBook>::Failure(online.Error());
                }

                PacketResult<int> graphicsNo = ParseDecimalField(parts[baseIndex + 6U], baseIndex + 6U);
                if (!graphicsNo.HasValue()) {
                    return PacketResult<AddressBook>::Failure(graphicsNo.Error());
                }

                slots[slotIndex] = AddressBookSlot {
                    use.Value(),
                    parts[baseIndex + 1U],
                    parts[baseIndex + 2U],
                    registSeqNumber.Value(),
                    level.Value(),
                    online.Value(),
                    graphicsNo.Value(),
                };
            }

            return PacketResult<AddressBook>::Success(AddressBook(std::move(slots)));
        }

        [[nodiscard]] static bool SlotPartsEmpty(
            const std::vector<std::string>& parts,
            std::size_t baseIndex) noexcept
        {
            for (std::size_t index = 0; index < SlotFieldCount; ++index) {
                if (!parts[baseIndex + index].empty()) {
                    return false;
                }
            }
            return true;
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
                    "AddressBook payload 十进制字段为空",
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
                        "AddressBook payload 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "AddressBook payload 十进制字段超出 int 范围",
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
     * SEND_ABI ADDRESSBOOK_sendAddressbookTableOne		vI {registry:i62} {data:es}
     * data: use|charNameRaw|ownTitleRaw|registSeqNumber|level|online|graphicsNo|clearGraphicsFlag|field9Raw|field10Raw。
     */
    struct AddressBookInfo
    {
        static constexpr std::string_view Code = "vI";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";
        static constexpr std::size_t PayloadFieldCount = 10U;

        int registry = 0;
        int use = 0;
        std::string charNameRaw;
        std::string ownTitleRaw;
        int registSeqNumber = 0;
        int level = 0;
        int online = 0;
        int graphicsNo = 0;
        int clearGraphicsFlag = 0;
        std::string field9Raw;
        std::string field10Raw;

        AddressBookInfo() = default;

        AddressBookInfo(
            int registry,
            int use,
            std::string charNameRaw,
            std::string ownTitleRaw,
            int registSeqNumber,
            int level,
            int online,
            int graphicsNo,
            int clearGraphicsFlag,
            std::string field9Raw,
            std::string field10Raw)
            : registry(registry),
              use(use),
              charNameRaw(std::move(charNameRaw)),
              ownTitleRaw(std::move(ownTitleRaw)),
              registSeqNumber(registSeqNumber),
              level(level),
              online(online),
              graphicsNo(graphicsNo),
              clearGraphicsFlag(clearGraphicsFlag),
              field9Raw(std::move(field9Raw)),
              field10Raw(std::move(field10Raw))
        {
        }

        [[nodiscard]] bool IsActive() const noexcept
        {
            return use > 0;
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::array<std::string, PayloadFieldCount> parts = {};
            parts[UsePayloadIndex] = std::to_string(use);

            if (IsActive()) {
                parts[CharNamePayloadIndex] = charNameRaw;
                parts[OwnTitlePayloadIndex] = ownTitleRaw;
                parts[RegistSeqNumberPayloadIndex] = std::to_string(registSeqNumber);
                parts[LevelPayloadIndex] = std::to_string(level);
                parts[OnlinePayloadIndex] = std::to_string(online);
                parts[GraphicsNoPayloadIndex] = std::to_string(graphicsNo);
                parts[ClearGraphicsFlagPayloadIndex] = std::to_string(clearGraphicsFlag);
                parts[Field9PayloadIndex] = field9Raw;
                parts[Field10PayloadIndex] = field10Raw;
            }

            PacketResult<std::string> payload = BuildPayload(
                std::span<const std::string>(parts.data(), parts.size()),
                PayloadDelimiter);
            if (payload.HasValue()) {
                return payload.Value();
            }
            return {};
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ClientIda);
            packet.fields.push_back(PacketField {
                "registry",
                PacketFieldType::Int62,
                std::to_string(registry),
                PacketSourceTag::ClientIda,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                BuildData(),
                PacketSourceTag::ClientIda,
            });
            return packet;
        }

        static PacketResult<AddressBookInfo> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<AddressBookInfo>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<AddressBookInfo> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "AddressBookInfo Parse 收到非 vI 封包");
                error.packetCode = packet.code;
                return PacketResult<AddressBookInfo>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<AddressBookInfo>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "AddressBookInfo Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<AddressBookInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "AddressBookInfo Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& registryField = packet.fields[RegistryFieldIndex];
            if (registryField.name != "registry") {
                return PacketResult<AddressBookInfo>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "AddressBookInfo Parse 字段名不匹配",
                    RegistryFieldIndex));
            }
            if (registryField.type != PacketFieldType::Int62) {
                return PacketResult<AddressBookInfo>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "AddressBookInfo Parse 字段类型不匹配",
                    RegistryFieldIndex));
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (dataField.name != "data") {
                return PacketResult<AddressBookInfo>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "AddressBookInfo Parse 字段名不匹配",
                    DataFieldIndex));
            }
            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<AddressBookInfo>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "AddressBookInfo Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<int> registryValue = ParseDecimalField(registryField.value, RegistryFieldIndex);
            if (!registryValue.HasValue()) {
                return PacketResult<AddressBookInfo>::Failure(registryValue.Error());
            }

            return ParseData(registryValue.Value(), dataField.value);
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t RegistryFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;
        static constexpr std::size_t UsePayloadIndex = 0U;
        static constexpr std::size_t CharNamePayloadIndex = 1U;
        static constexpr std::size_t OwnTitlePayloadIndex = 2U;
        static constexpr std::size_t RegistSeqNumberPayloadIndex = 3U;
        static constexpr std::size_t LevelPayloadIndex = 4U;
        static constexpr std::size_t OnlinePayloadIndex = 5U;
        static constexpr std::size_t GraphicsNoPayloadIndex = 6U;
        static constexpr std::size_t ClearGraphicsFlagPayloadIndex = 7U;
        static constexpr std::size_t Field9PayloadIndex = 8U;
        static constexpr std::size_t Field10PayloadIndex = 9U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ClientIda;
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

        [[nodiscard]] static PacketResult<AddressBookInfo> ParseData(int registry, std::string_view payload)
        {
            PacketResult<std::vector<std::string>> split = SplitPayloadLegacyAware(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ClientIda;
                return PacketResult<AddressBookInfo>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            if (parts.size() == PayloadFieldCount + 1U && !parts.empty() && parts.back().empty()) {
                parts.pop_back();
            }

            if (parts.size() != PayloadFieldCount) {
                return PacketResult<AddressBookInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "AddressBookInfo payload 段数量不匹配",
                    DataFieldIndex));
            }

            PacketResult<int> useValue = ParseDecimalField(parts[UsePayloadIndex], UsePayloadIndex);
            if (!useValue.HasValue()) {
                return PacketResult<AddressBookInfo>::Failure(useValue.Error());
            }

            AddressBookInfo info;
            info.registry = registry;
            info.use = useValue.Value();
            info.charNameRaw = parts[CharNamePayloadIndex];
            info.ownTitleRaw = parts[OwnTitlePayloadIndex];
            info.field9Raw = parts[Field9PayloadIndex];
            info.field10Raw = parts[Field10PayloadIndex];

            if (!info.IsActive()) {
                return PacketResult<AddressBookInfo>::Success(std::move(info));
            }

            PacketResult<int> registSeqNumber =
                ParseDecimalField(parts[RegistSeqNumberPayloadIndex], RegistSeqNumberPayloadIndex);
            if (!registSeqNumber.HasValue()) {
                return PacketResult<AddressBookInfo>::Failure(registSeqNumber.Error());
            }

            PacketResult<int> level = ParseDecimalField(parts[LevelPayloadIndex], LevelPayloadIndex);
            if (!level.HasValue()) {
                return PacketResult<AddressBookInfo>::Failure(level.Error());
            }

            PacketResult<int> online = ParseDecimalField(parts[OnlinePayloadIndex], OnlinePayloadIndex);
            if (!online.HasValue()) {
                return PacketResult<AddressBookInfo>::Failure(online.Error());
            }

            PacketResult<int> graphicsNo = ParseDecimalField(parts[GraphicsNoPayloadIndex], GraphicsNoPayloadIndex);
            if (!graphicsNo.HasValue()) {
                return PacketResult<AddressBookInfo>::Failure(graphicsNo.Error());
            }

            PacketResult<int> clearGraphicsFlag =
                ParseDecimalField(parts[ClearGraphicsFlagPayloadIndex], ClearGraphicsFlagPayloadIndex);
            if (!clearGraphicsFlag.HasValue()) {
                return PacketResult<AddressBookInfo>::Failure(clearGraphicsFlag.Error());
            }

            info.registSeqNumber = registSeqNumber.Value();
            info.level = level.Value();
            info.online = online.Value();
            info.graphicsNo = graphicsNo.Value();
            info.clearGraphicsFlag = clearGraphicsFlag.Value();
            return PacketResult<AddressBookInfo>::Success(std::move(info));
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
                    "AddressBookInfo 十进制字段为空",
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
                        "AddressBookInfo 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "AddressBookInfo 十进制字段超出 int 范围",
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
     * SEND_GI guildinfo		aDn {data:es}
     * data: flag:i62|[fields...]|；除 flag 外数值为 raw decimal，字段由 flag bit 控制。
     */
    struct GuildInfo
    {
        static constexpr std::string_view Code = "aDn";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        static constexpr int AllFieldsFlag = 0x0001;
        static constexpr int GuildIdFlag = 0x0002;
        static constexpr int GuildNameFlag = 0x0004;
        static constexpr int RoomNameFlag = 0x0008;
        static constexpr int TitleIdFlag = 0x0010;
        static constexpr int LastUpdateFlag = 0x0020;
        static constexpr int SortTypeFlag = 0x0040;
        static constexpr int ServerNoFlag = 0x0080;
        static constexpr int CommitionFlag = 0x0100;
        static constexpr int Breeding0Flag = 0x0200;
        static constexpr int Breeding1Flag = 0x0400;
        static constexpr int Breeding2Flag = 0x0800;
        static constexpr int RegistSeqNumberFlag = 0x1000;
        static constexpr int KnownFlagMask = 0x1FFF;

        int flag = 0;
        std::optional<int> guildId;
        std::optional<std::string> guildNameRaw;
        std::optional<std::string> roomNameRaw;
        std::optional<int> titleId;
        std::optional<int> lastUpdate;
        std::optional<int> sortType;
        std::optional<int> serverNo;
        std::optional<int> commition;
        std::optional<std::string> breeding0Raw;
        std::optional<std::string> breeding1Raw;
        std::optional<std::string> breeding2Raw;
        std::optional<int> registSeqNumber;

        GuildInfo() = default;

        explicit GuildInfo(int flag)
            : flag(flag)
        {
        }

        [[nodiscard]] bool HasField(int fieldBit) const noexcept
        {
            return IsFieldSelected(flag, fieldBit);
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            if (std::optional<PacketError> error = ValidateFlag(flag, PacketErrorCode::EncodeFailed, FlagPayloadIndex)) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }

            PacketResult<std::string> encodedFlag = EncodeInt62(flag);
            if (!encodedFlag.HasValue()) {
                PacketError error = encodedFlag.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = FlagPayloadIndex;
                error.source = PacketSourceTag::ClientIda;
                return PacketResult<std::string>::Failure(std::move(error));
            }

            std::vector<std::string> parts;
            parts.reserve(PayloadFieldMaxCount);
            parts.push_back(encodedFlag.Value());

            if (std::optional<PacketError> error = AppendIntField(parts, guildId, GuildIdFlag, "GuildInfo BuildData guildId 缺失")) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }
            if (std::optional<PacketError> error = AppendStringField(parts, guildNameRaw, GuildNameFlag, "GuildInfo BuildData guildNameRaw 缺失")) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }
            if (std::optional<PacketError> error = AppendStringField(parts, roomNameRaw, RoomNameFlag, "GuildInfo BuildData roomNameRaw 缺失")) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }
            if (std::optional<PacketError> error = AppendIntField(parts, titleId, TitleIdFlag, "GuildInfo BuildData titleId 缺失")) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }
            if (std::optional<PacketError> error = AppendIntField(parts, lastUpdate, LastUpdateFlag, "GuildInfo BuildData lastUpdate 缺失")) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }
            if (std::optional<PacketError> error = AppendIntField(parts, sortType, SortTypeFlag, "GuildInfo BuildData sortType 缺失")) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }
            if (std::optional<PacketError> error = AppendIntField(parts, serverNo, ServerNoFlag, "GuildInfo BuildData serverNo 缺失")) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }
            if (std::optional<PacketError> error = AppendIntField(parts, commition, CommitionFlag, "GuildInfo BuildData commition 缺失")) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }
            if (std::optional<PacketError> error = AppendStringField(parts, breeding0Raw, Breeding0Flag, "GuildInfo BuildData breeding0Raw 缺失")) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }
            if (std::optional<PacketError> error = AppendStringField(parts, breeding1Raw, Breeding1Flag, "GuildInfo BuildData breeding1Raw 缺失")) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }
            if (std::optional<PacketError> error = AppendStringField(parts, breeding2Raw, Breeding2Flag, "GuildInfo BuildData breeding2Raw 缺失")) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }
            if (std::optional<PacketError> error = AppendIntField(parts, registSeqNumber, RegistSeqNumberFlag, "GuildInfo BuildData registSeqNumber 缺失")) {
                return PacketResult<std::string>::Failure(std::move(*error));
            }

            parts.emplace_back();
            return BuildPayload(std::span<const std::string>(parts.data(), parts.size()), PayloadDelimiter);
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            PacketResult<std::string> data = BuildData();
            if (!data.HasValue()) {
                return PacketResult<Packet>::Failure(data.Error());
            }

            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ClientIda);
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                data.Value(),
                PacketSourceTag::ClientIda,
            });
            return PacketResult<Packet>::Success(std::move(packet));
        }

        static PacketResult<GuildInfo> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<GuildInfo>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<GuildInfo> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "GuildInfo Parse 收到非 aDn 封包");
                error.packetCode = packet.code;
                return PacketResult<GuildInfo>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<GuildInfo>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "GuildInfo Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<GuildInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "GuildInfo Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<GuildInfo>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "GuildInfo Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<GuildInfo>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "GuildInfo Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t FlagPayloadIndex = 0U;
        static constexpr std::size_t PayloadFieldMaxCount = 14U;

        [[nodiscard]] static bool IsFieldSelected(int flagValue, int fieldBit) noexcept
        {
            return (flagValue & (AllFieldsFlag | fieldBit)) != 0;
        }

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ClientIda;
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

        [[nodiscard]] static std::optional<PacketError> ValidateFlag(
            int flagValue,
            PacketErrorCode code,
            std::size_t fieldIndex)
        {
            if (flagValue < 0) {
                return MakeFieldError(code, "GuildInfo flag 不能为负数", fieldIndex);
            }

            if ((flagValue & ~KnownFlagMask) != 0) {
                return MakeFieldError(code, "GuildInfo flag 包含未知 bit", fieldIndex);
            }

            return std::nullopt;
        }

        [[nodiscard]] std::optional<PacketError> AppendIntField(
            std::vector<std::string>& parts,
            const std::optional<int>& value,
            int fieldBit,
            std::string message) const
        {
            if (!HasField(fieldBit)) {
                return std::nullopt;
            }

            if (!value.has_value()) {
                return MakeFieldError(PacketErrorCode::EncodeFailed, std::move(message), parts.size());
            }

            parts.push_back(std::to_string(value.value()));
            return std::nullopt;
        }

        [[nodiscard]] std::optional<PacketError> AppendStringField(
            std::vector<std::string>& parts,
            const std::optional<std::string>& value,
            int fieldBit,
            std::string message) const
        {
            if (!HasField(fieldBit)) {
                return std::nullopt;
            }

            if (!value.has_value()) {
                return MakeFieldError(PacketErrorCode::EncodeFailed, std::move(message), parts.size());
            }

            parts.push_back(value.value());
            return std::nullopt;
        }

        [[nodiscard]] static PacketResult<GuildInfo> ParseData(std::string_view payload)
        {
            PacketResult<std::vector<std::string>> split = SplitPayloadLegacyAware(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ClientIda;
                return PacketResult<GuildInfo>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            if (parts.size() < 2U || !parts.back().empty()) {
                return PacketResult<GuildInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "GuildInfo payload 缺少尾随空段",
                    DataFieldIndex));
            }
            parts.pop_back();

            PacketResult<int> flagValue = ParseFlagField(parts[FlagPayloadIndex]);
            if (!flagValue.HasValue()) {
                return PacketResult<GuildInfo>::Failure(flagValue.Error());
            }

            const std::size_t expectedFieldCount = ExpectedPayloadFieldCount(flagValue.Value());
            if (parts.size() != expectedFieldCount) {
                return PacketResult<GuildInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "GuildInfo payload 段数量不匹配",
                    DataFieldIndex));
            }

            GuildInfo info(flagValue.Value());
            std::size_t payloadIndex = 1U;

            if (IsFieldSelected(info.flag, GuildIdFlag)) {
                PacketResult<int> value = ParseDecimalField(parts[payloadIndex], payloadIndex);
                if (!value.HasValue()) {
                    return PacketResult<GuildInfo>::Failure(value.Error());
                }
                info.guildId = value.Value();
                ++payloadIndex;
            }
            if (IsFieldSelected(info.flag, GuildNameFlag)) {
                info.guildNameRaw = std::move(parts[payloadIndex]);
                ++payloadIndex;
            }
            if (IsFieldSelected(info.flag, RoomNameFlag)) {
                info.roomNameRaw = std::move(parts[payloadIndex]);
                ++payloadIndex;
            }
            if (IsFieldSelected(info.flag, TitleIdFlag)) {
                PacketResult<int> value = ParseDecimalField(parts[payloadIndex], payloadIndex);
                if (!value.HasValue()) {
                    return PacketResult<GuildInfo>::Failure(value.Error());
                }
                info.titleId = value.Value();
                ++payloadIndex;
            }
            if (IsFieldSelected(info.flag, LastUpdateFlag)) {
                PacketResult<int> value = ParseDecimalField(parts[payloadIndex], payloadIndex);
                if (!value.HasValue()) {
                    return PacketResult<GuildInfo>::Failure(value.Error());
                }
                info.lastUpdate = value.Value();
                ++payloadIndex;
            }
            if (IsFieldSelected(info.flag, SortTypeFlag)) {
                PacketResult<int> value = ParseDecimalField(parts[payloadIndex], payloadIndex);
                if (!value.HasValue()) {
                    return PacketResult<GuildInfo>::Failure(value.Error());
                }
                info.sortType = value.Value();
                ++payloadIndex;
            }
            if (IsFieldSelected(info.flag, ServerNoFlag)) {
                PacketResult<int> value = ParseDecimalField(parts[payloadIndex], payloadIndex);
                if (!value.HasValue()) {
                    return PacketResult<GuildInfo>::Failure(value.Error());
                }
                info.serverNo = value.Value();
                ++payloadIndex;
            }
            if (IsFieldSelected(info.flag, CommitionFlag)) {
                PacketResult<int> value = ParseDecimalField(parts[payloadIndex], payloadIndex);
                if (!value.HasValue()) {
                    return PacketResult<GuildInfo>::Failure(value.Error());
                }
                info.commition = value.Value();
                ++payloadIndex;
            }
            if (IsFieldSelected(info.flag, Breeding0Flag)) {
                info.breeding0Raw = std::move(parts[payloadIndex]);
                ++payloadIndex;
            }
            if (IsFieldSelected(info.flag, Breeding1Flag)) {
                info.breeding1Raw = std::move(parts[payloadIndex]);
                ++payloadIndex;
            }
            if (IsFieldSelected(info.flag, Breeding2Flag)) {
                info.breeding2Raw = std::move(parts[payloadIndex]);
                ++payloadIndex;
            }
            if (IsFieldSelected(info.flag, RegistSeqNumberFlag)) {
                PacketResult<int> value = ParseDecimalField(parts[payloadIndex], payloadIndex);
                if (!value.HasValue()) {
                    return PacketResult<GuildInfo>::Failure(value.Error());
                }
                info.registSeqNumber = value.Value();
            }

            return PacketResult<GuildInfo>::Success(std::move(info));
        }

        [[nodiscard]] static PacketResult<int> ParseFlagField(std::string_view text)
        {
            PacketResult<int> decoded = DecodeInt62(text);
            if (!decoded.HasValue()) {
                PacketError error = decoded.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = FlagPayloadIndex;
                error.source = PacketSourceTag::ClientIda;
                return PacketResult<int>::Failure(std::move(error));
            }

            if (std::optional<PacketError> error =
                    ValidateFlag(decoded.Value(), PacketErrorCode::DecodeFailed, FlagPayloadIndex)) {
                return PacketResult<int>::Failure(std::move(*error));
            }

            return decoded;
        }

        [[nodiscard]] static std::size_t ExpectedPayloadFieldCount(int flagValue) noexcept
        {
            std::size_t fieldCount = 1U;
            fieldCount += IsFieldSelected(flagValue, GuildIdFlag) ? 1U : 0U;
            fieldCount += IsFieldSelected(flagValue, GuildNameFlag) ? 1U : 0U;
            fieldCount += IsFieldSelected(flagValue, RoomNameFlag) ? 1U : 0U;
            fieldCount += IsFieldSelected(flagValue, TitleIdFlag) ? 1U : 0U;
            fieldCount += IsFieldSelected(flagValue, LastUpdateFlag) ? 1U : 0U;
            fieldCount += IsFieldSelected(flagValue, SortTypeFlag) ? 1U : 0U;
            fieldCount += IsFieldSelected(flagValue, ServerNoFlag) ? 1U : 0U;
            fieldCount += IsFieldSelected(flagValue, CommitionFlag) ? 1U : 0U;
            fieldCount += IsFieldSelected(flagValue, Breeding0Flag) ? 1U : 0U;
            fieldCount += IsFieldSelected(flagValue, Breeding1Flag) ? 1U : 0U;
            fieldCount += IsFieldSelected(flagValue, Breeding2Flag) ? 1U : 0U;
            fieldCount += IsFieldSelected(flagValue, RegistSeqNumberFlag) ? 1U : 0U;
            return fieldCount;
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
                    "GuildInfo payload 十进制字段为空",
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
                        "GuildInfo payload 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "GuildInfo payload 十进制字段超出 int 范围",
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

    struct GuildTitleRecord
    {
        int seq = 0;
        int commition = 0;
        std::string titleNameRaw;
    };

    /**
     * SEND_GT guildtitle		znv {data:es}
     * data: cnt|seq|commition|titleNameRaw x cnt；数值为 raw decimal，titleNameRaw 保留 raw escaped。
     */
    struct GuildTitle
    {
        static constexpr std::string_view Code = "znv";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";
        static constexpr std::size_t RecordFieldCount = 3U;

        std::vector<GuildTitleRecord> records;

        GuildTitle() = default;

        explicit GuildTitle(std::vector<GuildTitleRecord> records)
            : records(std::move(records))
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            if (records.empty()) {
                return "0";
            }

            std::vector<std::string> parts;
            parts.reserve(1U + (records.size() * RecordFieldCount));
            parts.push_back(std::to_string(records.size()));
            for (const GuildTitleRecord& record : records) {
                parts.push_back(std::to_string(record.seq));
                parts.push_back(std::to_string(record.commition));
                parts.push_back(record.titleNameRaw);
            }

            PacketResult<std::string> payload =
                BuildPayload(std::span<const std::string>(parts.data(), parts.size()), PayloadDelimiter);
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

        static PacketResult<GuildTitle> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<GuildTitle>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "GuildTitle Parse 收到非 znv 封包");
                error.packetCode = packet.code;
                return PacketResult<GuildTitle>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<GuildTitle>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "GuildTitle Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<GuildTitle>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "GuildTitle Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<GuildTitle>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "GuildTitle Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<GuildTitle>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "GuildTitle Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t CountPayloadIndex = 0U;

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

        [[nodiscard]] static PacketResult<GuildTitle> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<GuildTitle>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "GuildTitle payload 为空",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayloadLegacyAware(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<GuildTitle>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            PacketResult<int> countValue = ParseDecimalField(parts[CountPayloadIndex], CountPayloadIndex);
            if (!countValue.HasValue()) {
                return PacketResult<GuildTitle>::Failure(countValue.Error());
            }

            if (countValue.Value() < 0) {
                return PacketResult<GuildTitle>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "GuildTitle payload cnt 不能为负数",
                    CountPayloadIndex));
            }

            const std::size_t recordCount = static_cast<std::size_t>(countValue.Value());
            const std::size_t expectedFieldCount = 1U + (recordCount * RecordFieldCount);
            if (parts.size() == expectedFieldCount + 1U && !parts.empty() && parts.back().empty()) {
                parts.pop_back();
            }

            if (parts.size() != expectedFieldCount) {
                return PacketResult<GuildTitle>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "GuildTitle payload 段数量不匹配",
                    DataFieldIndex));
            }

            std::vector<GuildTitleRecord> records;
            records.reserve(recordCount);
            for (std::size_t index = 1U; index < parts.size(); index += RecordFieldCount) {
                PacketResult<int> seq = ParseDecimalField(parts[index], index);
                if (!seq.HasValue()) {
                    return PacketResult<GuildTitle>::Failure(seq.Error());
                }

                PacketResult<int> commition = ParseDecimalField(parts[index + 1U], index + 1U);
                if (!commition.HasValue()) {
                    return PacketResult<GuildTitle>::Failure(commition.Error());
                }

                records.push_back(GuildTitleRecord {
                    seq.Value(),
                    commition.Value(),
                    std::move(parts[index + 2U]),
                });
            }

            return PacketResult<GuildTitle>::Success(GuildTitle(std::move(records)));
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
                    "GuildTitle payload 十进制字段为空",
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
                        "GuildTitle payload 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "GuildTitle payload 十进制字段超出 int 范围",
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

    struct GuildMemberRecord
    {
        int flag = 0;
        int registSeqNumber = 0;
        std::string nameTitleRaw;
        int registSeqNumber2 = 0;
        int level = 0;
        int online = 0;
        int graphicsNo = 0;
        int toGuildTitleId = 0;
    };

    /**
     * SEND_GM guildmember		yuD {data:es}
     * data: flag|registSeqNumber|nameTitleRaw|registSeqNumber2|level|online|graphicsNo|toGuildTitleID x N。
     */
    struct GuildMember
    {
        static constexpr std::string_view Code = "yuD";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";
        static constexpr std::size_t RecordFieldCount = 8U;

        std::vector<GuildMemberRecord> records;

        GuildMember() = default;

        explicit GuildMember(std::vector<GuildMemberRecord> records)
            : records(std::move(records))
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            std::vector<std::string> parts;
            parts.reserve(records.size() * RecordFieldCount);
            for (const GuildMemberRecord& record : records) {
                if (record.flag <= 0) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::EncodeFailed,
                        "GuildMember BuildData flag 必须大于 0",
                        parts.size()));
                }

                parts.push_back(std::to_string(record.flag));
                parts.push_back(std::to_string(record.registSeqNumber));
                parts.push_back(record.nameTitleRaw);
                parts.push_back(std::to_string(record.registSeqNumber2));
                parts.push_back(std::to_string(record.level));
                parts.push_back(std::to_string(record.online));
                parts.push_back(std::to_string(record.graphicsNo));
                parts.push_back(std::to_string(record.toGuildTitleId));
            }

            return BuildPayload(std::span<const std::string>(parts.data(), parts.size()), PayloadDelimiter);
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
            return PacketResult<Packet>::Success(std::move(packet));
        }

        static PacketResult<GuildMember> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<GuildMember>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "GuildMember Parse 收到非 yuD 封包");
                error.packetCode = packet.code;
                return PacketResult<GuildMember>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<GuildMember>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "GuildMember Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<GuildMember>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "GuildMember Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<GuildMember>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "GuildMember Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<GuildMember>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "GuildMember Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            return ParseData(field.value);
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;
        static constexpr std::size_t FlagPayloadIndex = 0U;
        static constexpr std::size_t RegistSeqNumberPayloadIndex = 1U;
        static constexpr std::size_t NameTitlePayloadIndex = 2U;
        static constexpr std::size_t RegistSeqNumber2PayloadIndex = 3U;
        static constexpr std::size_t LevelPayloadIndex = 4U;
        static constexpr std::size_t OnlinePayloadIndex = 5U;
        static constexpr std::size_t GraphicsNoPayloadIndex = 6U;
        static constexpr std::size_t ToGuildTitleIdPayloadIndex = 7U;

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

        [[nodiscard]] static PacketResult<GuildMember> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<GuildMember>::Success(GuildMember());
            }

            PacketResult<std::vector<std::string>> split = SplitPayloadLegacyAware(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<GuildMember>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            if ((parts.size() % RecordFieldCount) == 1U && !parts.empty() && parts.back().empty()) {
                parts.pop_back();
            }

            if ((parts.size() % RecordFieldCount) != 0U) {
                return PacketResult<GuildMember>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "GuildMember payload 段数量不匹配",
                    DataFieldIndex));
            }

            std::vector<GuildMemberRecord> records;
            records.reserve(parts.size() / RecordFieldCount);
            for (std::size_t index = 0; index < parts.size(); index += RecordFieldCount) {
                PacketResult<int> flag = ParseDecimalField(parts[index + FlagPayloadIndex], index + FlagPayloadIndex);
                if (!flag.HasValue()) {
                    return PacketResult<GuildMember>::Failure(flag.Error());
                }
                if (flag.Value() <= 0) {
                    return PacketResult<GuildMember>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "GuildMember payload flag 必须大于 0",
                        index + FlagPayloadIndex));
                }

                PacketResult<int> registSeqNumber = ParseDecimalField(
                    parts[index + RegistSeqNumberPayloadIndex],
                    index + RegistSeqNumberPayloadIndex);
                if (!registSeqNumber.HasValue()) {
                    return PacketResult<GuildMember>::Failure(registSeqNumber.Error());
                }

                PacketResult<int> registSeqNumber2 = ParseDecimalField(
                    parts[index + RegistSeqNumber2PayloadIndex],
                    index + RegistSeqNumber2PayloadIndex);
                if (!registSeqNumber2.HasValue()) {
                    return PacketResult<GuildMember>::Failure(registSeqNumber2.Error());
                }

                PacketResult<int> level = ParseDecimalField(parts[index + LevelPayloadIndex], index + LevelPayloadIndex);
                if (!level.HasValue()) {
                    return PacketResult<GuildMember>::Failure(level.Error());
                }

                PacketResult<int> online = ParseDecimalField(parts[index + OnlinePayloadIndex], index + OnlinePayloadIndex);
                if (!online.HasValue()) {
                    return PacketResult<GuildMember>::Failure(online.Error());
                }

                PacketResult<int> graphicsNo = ParseDecimalField(
                    parts[index + GraphicsNoPayloadIndex],
                    index + GraphicsNoPayloadIndex);
                if (!graphicsNo.HasValue()) {
                    return PacketResult<GuildMember>::Failure(graphicsNo.Error());
                }

                PacketResult<int> toGuildTitleId = ParseDecimalField(
                    parts[index + ToGuildTitleIdPayloadIndex],
                    index + ToGuildTitleIdPayloadIndex);
                if (!toGuildTitleId.HasValue()) {
                    return PacketResult<GuildMember>::Failure(toGuildTitleId.Error());
                }

                records.push_back(GuildMemberRecord {
                    flag.Value(),
                    registSeqNumber.Value(),
                    std::move(parts[index + NameTitlePayloadIndex]),
                    registSeqNumber2.Value(),
                    level.Value(),
                    online.Value(),
                    graphicsNo.Value(),
                    toGuildTitleId.Value(),
                });
            }

            return PacketResult<GuildMember>::Success(GuildMember(std::move(records)));
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
                    "GuildMember payload 十进制字段为空",
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
                        "GuildMember payload 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "GuildMember payload 十进制字段超出 int 范围",
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
     * SEND_GMI guildmemberinfo		Kka {index:i62} {data:es}
     * data: flag|registSeqNumber|nameTitleRaw|registSeqNumberAgain|level|online|graphicsNo|toGuildTitleID|。
     */
    struct GuildMemberInfo
    {
        static constexpr std::string_view Code = "Kka";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int index = 0;
        int flag = 0;
        int registSeqNumber = 0;
        std::string nameTitleRaw;
        int registSeqNumberAgain = 0;
        int level = 0;
        int online = 0;
        int graphicsNo = 0;
        int toGuildTitleId = 0;

        GuildMemberInfo() = default;

        GuildMemberInfo(
            int index,
            int flag,
            int registSeqNumber,
            std::string nameTitleRaw,
            int registSeqNumberAgain,
            int level,
            int online,
            int graphicsNo,
            int toGuildTitleId)
            : index(index),
              flag(flag),
              registSeqNumber(registSeqNumber),
              nameTitleRaw(std::move(nameTitleRaw)),
              registSeqNumberAgain(registSeqNumberAgain),
              level(level),
              online(online),
              graphicsNo(graphicsNo),
              toGuildTitleId(toGuildTitleId)
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            const std::array<std::string, PayloadFieldCount + 1U> parts = {
                std::to_string(flag),
                std::to_string(registSeqNumber),
                nameTitleRaw,
                std::to_string(registSeqNumberAgain),
                std::to_string(level),
                std::to_string(online),
                std::to_string(graphicsNo),
                std::to_string(toGuildTitleId),
                std::string(),
            };

            return BuildPayload(std::span<const std::string>(parts.data(), parts.size()), PayloadDelimiter);
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            PacketResult<std::string> data = BuildData();
            if (!data.HasValue()) {
                return PacketResult<Packet>::Failure(data.Error());
            }

            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "index",
                PacketFieldType::Int62,
                std::to_string(index),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                data.Value(),
                PacketSourceTag::ServerC,
            });
            return PacketResult<Packet>::Success(std::move(packet));
        }

        static PacketResult<GuildMemberInfo> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<GuildMemberInfo>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<GuildMemberInfo> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "GuildMemberInfo Parse 收到非 Kka 封包");
                error.packetCode = packet.code;
                return PacketResult<GuildMemberInfo>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<GuildMemberInfo>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "GuildMemberInfo Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<GuildMemberInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "GuildMemberInfo Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const PacketField& indexField = packet.fields[IndexFieldIndex];
            if (indexField.name != "index") {
                return PacketResult<GuildMemberInfo>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "GuildMemberInfo Parse index 字段名不匹配",
                    IndexFieldIndex));
            }

            if (indexField.type != PacketFieldType::Int62) {
                return PacketResult<GuildMemberInfo>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "GuildMemberInfo Parse index 字段类型不匹配",
                    IndexFieldIndex));
            }

            const PacketField& dataField = packet.fields[DataFieldIndex];
            if (dataField.name != "data") {
                return PacketResult<GuildMemberInfo>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "GuildMemberInfo Parse data 字段名不匹配",
                    DataFieldIndex));
            }

            if (dataField.type != PacketFieldType::EscapedString) {
                return PacketResult<GuildMemberInfo>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "GuildMemberInfo Parse data 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<int> indexValue = ParseDecimalField(indexField.value, IndexFieldIndex);
            if (!indexValue.HasValue()) {
                return PacketResult<GuildMemberInfo>::Failure(indexValue.Error());
            }

            PacketResult<GuildMemberInfo> parsed = ParseData(dataField.value);
            if (!parsed.HasValue()) {
                return parsed;
            }

            GuildMemberInfo result = std::move(parsed.Value());
            result.index = indexValue.Value();
            return PacketResult<GuildMemberInfo>::Success(std::move(result));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t IndexFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;
        static constexpr std::size_t PayloadFieldCount = 8U;
        static constexpr std::size_t FlagPayloadIndex = 0U;
        static constexpr std::size_t RegistSeqNumberPayloadIndex = 1U;
        static constexpr std::size_t NameTitlePayloadIndex = 2U;
        static constexpr std::size_t RegistSeqNumberAgainPayloadIndex = 3U;
        static constexpr std::size_t LevelPayloadIndex = 4U;
        static constexpr std::size_t OnlinePayloadIndex = 5U;
        static constexpr std::size_t GraphicsNoPayloadIndex = 6U;
        static constexpr std::size_t ToGuildTitleIdPayloadIndex = 7U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<GuildMemberInfo> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<GuildMemberInfo>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "GuildMemberInfo payload 为空",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayloadLegacyAware(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<GuildMemberInfo>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            if (parts.size() != PayloadFieldCount + 1U || parts.back() != "") {
                return PacketResult<GuildMemberInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "GuildMemberInfo payload 段数量不匹配",
                    DataFieldIndex));
            }
            parts.pop_back();

            PacketResult<int> flag = ParseDecimalField(parts[FlagPayloadIndex], FlagPayloadIndex);
            if (!flag.HasValue()) {
                return PacketResult<GuildMemberInfo>::Failure(flag.Error());
            }

            PacketResult<int> registSeqNumber = ParseDecimalField(
                parts[RegistSeqNumberPayloadIndex],
                RegistSeqNumberPayloadIndex);
            if (!registSeqNumber.HasValue()) {
                return PacketResult<GuildMemberInfo>::Failure(registSeqNumber.Error());
            }

            PacketResult<int> registSeqNumberAgain = ParseDecimalField(
                parts[RegistSeqNumberAgainPayloadIndex],
                RegistSeqNumberAgainPayloadIndex);
            if (!registSeqNumberAgain.HasValue()) {
                return PacketResult<GuildMemberInfo>::Failure(registSeqNumberAgain.Error());
            }

            PacketResult<int> level = ParseDecimalField(parts[LevelPayloadIndex], LevelPayloadIndex);
            if (!level.HasValue()) {
                return PacketResult<GuildMemberInfo>::Failure(level.Error());
            }

            PacketResult<int> online = ParseDecimalField(parts[OnlinePayloadIndex], OnlinePayloadIndex);
            if (!online.HasValue()) {
                return PacketResult<GuildMemberInfo>::Failure(online.Error());
            }

            PacketResult<int> graphicsNo = ParseDecimalField(parts[GraphicsNoPayloadIndex], GraphicsNoPayloadIndex);
            if (!graphicsNo.HasValue()) {
                return PacketResult<GuildMemberInfo>::Failure(graphicsNo.Error());
            }

            PacketResult<int> toGuildTitleId = ParseDecimalField(
                parts[ToGuildTitleIdPayloadIndex],
                ToGuildTitleIdPayloadIndex);
            if (!toGuildTitleId.HasValue()) {
                return PacketResult<GuildMemberInfo>::Failure(toGuildTitleId.Error());
            }

            return PacketResult<GuildMemberInfo>::Success(GuildMemberInfo(
                0,
                flag.Value(),
                registSeqNumber.Value(),
                std::move(parts[NameTitlePayloadIndex]),
                registSeqNumberAgain.Value(),
                level.Value(),
                online.Value(),
                graphicsNo.Value(),
                toGuildTitleId.Value()));
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
                    "GuildMemberInfo 十进制字段为空",
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
                        "GuildMemberInfo 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "GuildMemberInfo 十进制字段超出 int 范围",
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
     * SEND_GML 家族邮件 payload。
     * 普通邮件 3 段：sentTimeText|messageRaw|-1。
     * 宠邮 6 段：sentTimeText|messageRaw|petBaseImageNumber|petLevel|petNameRaw|itemBaseImageNumber。
     */
    struct GuildMailPayload
    {
        bool isPetMail = false;
        std::string sentTimeText;
        std::string messageRaw;
        int petBaseImageNumberOrMarker = -1;
        int petLevel = 0;
        std::string petNameRaw;
        int itemBaseImageNumber = 0;
    };

    /**
     * SEND_GML 家族邮件		Rll {fromregistseqnumber:i62} {text:es} {color:i62}
     * text 为内部 `|` payload，仅支持普通邮件 3 段与宠邮 6 段。
     */
    struct GuildMail
    {
        static constexpr std::string_view Code = "Rll";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        int fromRegistSeqNumber = 0;
        GuildMailPayload payload;
        int color = 0;

        GuildMail(int fromRegistSeqNumber, GuildMailPayload payload, int color)
            : fromRegistSeqNumber(fromRegistSeqNumber),
              payload(std::move(payload)),
              color(color)
        {
        }

        [[nodiscard]] std::string BuildData() const
        {
            std::string text;
            text.append(payload.sentTimeText);
            text.push_back('|');
            text.append(payload.messageRaw);
            text.push_back('|');
            if (!payload.isPetMail) {
                text.append("-1");
                return text;
            }

            text.append(std::to_string(payload.petBaseImageNumberOrMarker));
            text.push_back('|');
            text.append(std::to_string(payload.petLevel));
            text.push_back('|');
            text.append(payload.petNameRaw);
            text.push_back('|');
            text.append(std::to_string(payload.itemBaseImageNumber));
            return text;
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::Xlsx);
            packet.fields.push_back(PacketField {
                "fromregistseqnumber",
                PacketFieldType::Int62,
                std::to_string(fromRegistSeqNumber),
                PacketSourceTag::Xlsx,
            });
            packet.fields.push_back(PacketField {
                "text",
                PacketFieldType::EscapedString,
                BuildData(),
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

        static PacketResult<GuildMail> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<GuildMail>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<GuildMail> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(PacketErrorCode::InvalidCode, "GuildMail Parse 收到非 Rll 封包");
                error.packetCode = packet.code;
                return PacketResult<GuildMail>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<GuildMail>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "GuildMail Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<GuildMail>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "GuildMail Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "fromregistseqnumber",
                "text",
                "color",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
                PacketFieldType::Int62,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<GuildMail>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "GuildMail Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<GuildMail>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "GuildMail Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> fromRegistSeqNumberValue =
                ParseDecimalField(packet.fields[FromRegistSeqNumberFieldIndex].value, FromRegistSeqNumberFieldIndex);
            if (!fromRegistSeqNumberValue.HasValue()) {
                return PacketResult<GuildMail>::Failure(fromRegistSeqNumberValue.Error());
            }

            PacketResult<GuildMailPayload> payloadValue = ParsePayloadText(packet.fields[TextFieldIndex].value);
            if (!payloadValue.HasValue()) {
                return PacketResult<GuildMail>::Failure(payloadValue.Error());
            }

            PacketResult<int> colorValue = ParseDecimalField(packet.fields[ColorFieldIndex].value, ColorFieldIndex);
            if (!colorValue.HasValue()) {
                return PacketResult<GuildMail>::Failure(colorValue.Error());
            }

            return PacketResult<GuildMail>::Success(GuildMail(
                fromRegistSeqNumberValue.Value(),
                std::move(payloadValue.Value()),
                colorValue.Value()));
        }

    private:
        static constexpr std::size_t FieldCount = 3U;
        static constexpr std::size_t FromRegistSeqNumberFieldIndex = 0U;
        static constexpr std::size_t TextFieldIndex = 1U;
        static constexpr std::size_t ColorFieldIndex = 2U;
        static constexpr std::size_t NormalPayloadFieldCount = 3U;
        static constexpr std::size_t PetPayloadFieldCount = 6U;
        static constexpr std::size_t PetBaseImageFieldIndex = 2U;
        static constexpr std::size_t PetLevelFieldIndex = 3U;
        static constexpr std::size_t PetNameFieldIndex = 4U;
        static constexpr std::size_t ItemBaseImageFieldIndex = 5U;

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

        [[nodiscard]] static PacketResult<GuildMailPayload> ParsePayloadText(std::string_view text)
        {
            if (text.empty()) {
                return PacketResult<GuildMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "GuildMail payload 为空",
                    TextFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(text, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = TextFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<GuildMailPayload>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != NormalPayloadFieldCount && parts.size() != PetPayloadFieldCount) {
                return PacketResult<GuildMailPayload>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "GuildMail payload 段数量不匹配",
                    TextFieldIndex));
            }

            GuildMailPayload payload;
            payload.isPetMail = parts.size() == PetPayloadFieldCount;
            payload.sentTimeText = parts[0];
            payload.messageRaw = parts[1];

            if (!payload.isPetMail) {
                if (parts[PetBaseImageFieldIndex] != "-1") {
                    return PacketResult<GuildMailPayload>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "GuildMail 普通邮件标记必须为 -1",
                        PetBaseImageFieldIndex));
                }
                payload.petBaseImageNumberOrMarker = -1;
                return PacketResult<GuildMailPayload>::Success(std::move(payload));
            }

            PacketResult<int> petBaseImageNumber =
                ParseDecimalField(parts[PetBaseImageFieldIndex], PetBaseImageFieldIndex);
            if (!petBaseImageNumber.HasValue()) {
                return PacketResult<GuildMailPayload>::Failure(petBaseImageNumber.Error());
            }

            PacketResult<int> petLevel = ParseDecimalField(parts[PetLevelFieldIndex], PetLevelFieldIndex);
            if (!petLevel.HasValue()) {
                return PacketResult<GuildMailPayload>::Failure(petLevel.Error());
            }

            PacketResult<int> itemBaseImageNumber =
                ParseDecimalField(parts[ItemBaseImageFieldIndex], ItemBaseImageFieldIndex);
            if (!itemBaseImageNumber.HasValue()) {
                return PacketResult<GuildMailPayload>::Failure(itemBaseImageNumber.Error());
            }

            payload.petBaseImageNumberOrMarker = petBaseImageNumber.Value();
            payload.petLevel = petLevel.Value();
            payload.petNameRaw = parts[PetNameFieldIndex];
            payload.itemBaseImageNumber = itemBaseImageNumber.Value();
            return PacketResult<GuildMailPayload>::Success(std::move(payload));
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
                    "GuildMail 十进制字段为空",
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
                        "GuildMail 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "GuildMail 十进制字段超出 int 范围",
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

    struct TradePlayerRecord
    {
        std::string nameRaw;
    };

    /**
     * SEND_TRPL CHAR_Trade_getPlayerList 获得可交易人员列表		Cxo {data:es}
     * data: `nameRaw|` 重复记录，空 data 表示空列表；nameRaw 保留 makeEscapeString 后的 raw 字符串。
     */
    struct TradePlayerList
    {
        static constexpr std::string_view Code = "Cxo";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";

        std::vector<TradePlayerRecord> records;

        TradePlayerList() = default;

        explicit TradePlayerList(std::vector<TradePlayerRecord> records)
            : records(std::move(records))
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            std::string payload;
            for (const TradePlayerRecord& record : records) {
                if (record.nameRaw.empty()) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::EncodeFailed,
                        "TradePlayerList BuildData nameRaw 为空",
                        DataFieldIndex));
                }

                if (record.nameRaw.find('|') != std::string::npos) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::EncodeFailed,
                        "TradePlayerList BuildData nameRaw 包含 literal |",
                        DataFieldIndex));
                }

                payload.append(record.nameRaw);
                payload.push_back('|');
            }

            return PacketResult<std::string>::Success(std::move(payload));
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            PacketResult<std::string> data = BuildData();
            if (!data.HasValue()) {
                return PacketResult<Packet>::Failure(data.Error());
            }

            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                data.Value(),
                PacketSourceTag::ServerC,
            });
            return PacketResult<Packet>::Success(std::move(packet));
        }

        static PacketResult<TradePlayerList> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<TradePlayerList>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<TradePlayerList> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(
                    PacketErrorCode::InvalidCode,
                    "TradePlayerList Parse 收到非 Cxo 封包");
                error.packetCode = packet.code;
                return PacketResult<TradePlayerList>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<TradePlayerList>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "TradePlayerList Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<TradePlayerList>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TradePlayerList Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<TradePlayerList>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "TradePlayerList Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<TradePlayerList>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "TradePlayerList Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::vector<TradePlayerRecord>> recordsValue = ParseRecords(field.value);
            if (!recordsValue.HasValue()) {
                return PacketResult<TradePlayerList>::Failure(recordsValue.Error());
            }

            return PacketResult<TradePlayerList>::Success(TradePlayerList(std::move(recordsValue.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<std::vector<TradePlayerRecord>> ParseRecords(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<std::vector<TradePlayerRecord>>::Success({});
            }

            if (payload.back() != '|') {
                return PacketResult<std::vector<TradePlayerRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "TradePlayerList payload 缺少尾随分隔符",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<std::vector<TradePlayerRecord>>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            parts.pop_back();
            if (parts.empty()) {
                return PacketResult<std::vector<TradePlayerRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "TradePlayerList payload 不能只有尾随分隔符",
                    DataFieldIndex));
            }

            std::vector<TradePlayerRecord> records;
            records.reserve(parts.size());
            for (std::string& nameRaw : parts) {
                if (nameRaw.empty()) {
                    return PacketResult<std::vector<TradePlayerRecord>>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TradePlayerList payload 包含空 nameRaw",
                        DataFieldIndex));
                }

                records.push_back(TradePlayerRecord {std::move(nameRaw)});
            }

            return PacketResult<std::vector<TradePlayerRecord>>::Success(std::move(records));
        }
    };

    /**
     * SEND_TRS CHAR_Trade_startTrade 被交易		ms {name:es} {level:i62}
     * nameRaw 保留 makeEscapeString 后的 raw 字符串；本层不做 Unescape2/GBK。
     */
    struct TradeStart
    {
        static constexpr std::string_view Code = "ms";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;

        std::string nameRaw;
        int level = 0;

        TradeStart(std::string nameRaw, int level)
            : nameRaw(std::move(nameRaw)),
              level(level)
        {
        }

        [[nodiscard]] Packet ToPacket() const
        {
            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "name",
                PacketFieldType::EscapedString,
                nameRaw,
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "level",
                PacketFieldType::Int62,
                std::to_string(level),
                PacketSourceTag::ServerC,
            });
            return packet;
        }

        static PacketResult<TradeStart> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<TradeStart>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<TradeStart> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(
                    PacketErrorCode::InvalidCode,
                    "TradeStart Parse 收到非 ms 封包");
                error.packetCode = packet.code;
                return PacketResult<TradeStart>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<TradeStart>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "TradeStart Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<TradeStart>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TradeStart Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "name",
                "level",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::EscapedString,
                PacketFieldType::Int62,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<TradeStart>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TradeStart Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<TradeStart>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "TradeStart Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> levelValue = ParseDecimalField(
                packet.fields[LevelFieldIndex].value,
                LevelFieldIndex);
            if (!levelValue.HasValue()) {
                return PacketResult<TradeStart>::Failure(levelValue.Error());
            }

            return PacketResult<TradeStart>::Success(TradeStart(
                packet.fields[NameFieldIndex].value,
                levelValue.Value()));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t NameFieldIndex = 0U;
        static constexpr std::size_t LevelFieldIndex = 1U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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
                    "TradeStart Parse i62 字段为空",
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
                        "TradeStart Parse i62 字段包含非十进制字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TradeStart Parse i62 字段超出 int 范围",
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
     * SEND_TRLI 交易窗口物品信息：
     * tradeWindowIndex|nameRaw|color|descriptionRaw|imageNo|canUseInField|battleUseFlag|target|level|flag|itemId|itemType|count|memoRaw。
     */
    struct TradeItemInfoRecord
    {
        int tradeWindowIndex = 0;
        std::string nameRaw;
        int color = 0;
        std::string descriptionRaw;
        int imageNo = 0;
        int canUseInField = 0;
        int battleUseFlag = 0;
        int target = 0;
        int level = 0;
        int flag = 0;
        int itemId = 0;
        int itemType = 0;
        int count = 0;
        std::string memoRaw;
    };

    /**
     * SEND_TRLI CHAR_Trade_setTradeList 对方物品信息		nUbJ {data:es}
     * data: 1..20 条 14 段记录；payload 兼容无尾随 `|`，BuildData 固定输出尾随 `|`。
     */
    struct TradeItemInfo
    {
        static constexpr std::string_view Code = "nUbJ";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";
        static constexpr std::size_t RecordFieldCount = 14U;
        static constexpr std::size_t MaxRecordCount = 20U;

        std::vector<TradeItemInfoRecord> records;

        TradeItemInfo() = default;

        explicit TradeItemInfo(std::vector<TradeItemInfoRecord> records)
            : records(std::move(records))
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            if (records.empty()) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    PacketErrorCode::EncodeFailed,
                    "TradeItemInfo BuildData records 为空",
                    DataFieldIndex));
            }

            if (records.size() > MaxRecordCount) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    PacketErrorCode::EncodeFailed,
                    "TradeItemInfo BuildData records 超过 20 条",
                    DataFieldIndex));
            }

            std::string payload;
            for (const TradeItemInfoRecord& record : records) {
                PacketResult<bool> strings = ValidateBuildStrings(record);
                if (!strings.HasValue()) {
                    return PacketResult<std::string>::Failure(strings.Error());
                }

                payload.append(std::to_string(record.tradeWindowIndex));
                payload.push_back('|');
                payload.append(record.nameRaw);
                payload.push_back('|');
                payload.append(std::to_string(record.color));
                payload.push_back('|');
                payload.append(record.descriptionRaw);
                payload.push_back('|');
                payload.append(std::to_string(record.imageNo));
                payload.push_back('|');
                payload.append(std::to_string(record.canUseInField));
                payload.push_back('|');
                payload.append(std::to_string(record.battleUseFlag));
                payload.push_back('|');
                payload.append(std::to_string(record.target));
                payload.push_back('|');
                payload.append(std::to_string(record.level));
                payload.push_back('|');
                payload.append(std::to_string(record.flag));
                payload.push_back('|');
                payload.append(std::to_string(record.itemId));
                payload.push_back('|');
                payload.append(std::to_string(record.itemType));
                payload.push_back('|');
                payload.append(std::to_string(record.count));
                payload.push_back('|');
                payload.append(record.memoRaw);
                payload.push_back('|');
            }

            return PacketResult<std::string>::Success(std::move(payload));
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            PacketResult<std::string> data = BuildData();
            if (!data.HasValue()) {
                return PacketResult<Packet>::Failure(data.Error());
            }

            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                data.Value(),
                PacketSourceTag::ServerC,
            });
            return PacketResult<Packet>::Success(std::move(packet));
        }

        static PacketResult<TradeItemInfo> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<TradeItemInfo>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<TradeItemInfo> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(
                    PacketErrorCode::InvalidCode,
                    "TradeItemInfo Parse 收到非 nUbJ 封包");
                error.packetCode = packet.code;
                return PacketResult<TradeItemInfo>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<TradeItemInfo>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "TradeItemInfo Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<TradeItemInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TradeItemInfo Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<TradeItemInfo>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "TradeItemInfo Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<TradeItemInfo>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "TradeItemInfo Parse 字段类型不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::vector<TradeItemInfoRecord>> recordsValue = ParseRecords(field.value);
            if (!recordsValue.HasValue()) {
                return PacketResult<TradeItemInfo>::Failure(recordsValue.Error());
            }

            return PacketResult<TradeItemInfo>::Success(TradeItemInfo(std::move(recordsValue.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 1U;
        static constexpr std::size_t DataFieldIndex = 0U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<std::vector<TradeItemInfoRecord>> ParseRecords(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<std::vector<TradeItemInfoRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "TradeItemInfo payload 为空",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<std::vector<TradeItemInfoRecord>>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            if (parts.size() == 1U && parts.front().empty()) {
                return PacketResult<std::vector<TradeItemInfoRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "TradeItemInfo payload 为空",
                    DataFieldIndex));
            }

            if ((parts.size() % RecordFieldCount) == 1U && !parts.empty() && parts.back().empty()) {
                parts.pop_back();
            }

            if (parts.empty() || (parts.size() % RecordFieldCount) != 0U) {
                return PacketResult<std::vector<TradeItemInfoRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TradeItemInfo payload 段数量不是 14 的倍数",
                    DataFieldIndex));
            }

            const std::size_t recordCount = parts.size() / RecordFieldCount;
            if (recordCount > MaxRecordCount) {
                return PacketResult<std::vector<TradeItemInfoRecord>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TradeItemInfo payload records 超过 20 条",
                    DataFieldIndex));
            }

            std::vector<TradeItemInfoRecord> records;
            records.reserve(recordCount);
            for (std::size_t recordIndex = 0; recordIndex < recordCount; ++recordIndex) {
                PacketResult<TradeItemInfoRecord> record = ParseRecord(
                    parts,
                    recordIndex * RecordFieldCount);
                if (!record.HasValue()) {
                    return PacketResult<std::vector<TradeItemInfoRecord>>::Failure(record.Error());
                }
                records.push_back(std::move(record.Value()));
            }

            return PacketResult<std::vector<TradeItemInfoRecord>>::Success(std::move(records));
        }

        [[nodiscard]] static PacketResult<TradeItemInfoRecord> ParseRecord(
            const std::vector<std::string>& parts,
            std::size_t baseIndex)
        {
            PacketResult<int> tradeWindowIndex = ParseDecimalField(parts[baseIndex], baseIndex);
            if (!tradeWindowIndex.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(tradeWindowIndex.Error());
            }

            PacketResult<std::string> nameRaw = ValidateRawStringField(parts[baseIndex + 1U], baseIndex + 1U);
            if (!nameRaw.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(nameRaw.Error());
            }

            PacketResult<int> color = ParseDecimalField(parts[baseIndex + 2U], baseIndex + 2U);
            if (!color.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(color.Error());
            }

            PacketResult<std::string> descriptionRaw =
                ValidateRawStringField(parts[baseIndex + 3U], baseIndex + 3U);
            if (!descriptionRaw.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(descriptionRaw.Error());
            }

            PacketResult<int> imageNo = ParseDecimalField(parts[baseIndex + 4U], baseIndex + 4U);
            if (!imageNo.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(imageNo.Error());
            }

            PacketResult<int> canUseInField = ParseDecimalField(parts[baseIndex + 5U], baseIndex + 5U);
            if (!canUseInField.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(canUseInField.Error());
            }

            PacketResult<int> battleUseFlag = ParseDecimalField(parts[baseIndex + 6U], baseIndex + 6U);
            if (!battleUseFlag.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(battleUseFlag.Error());
            }

            PacketResult<int> target = ParseDecimalField(parts[baseIndex + 7U], baseIndex + 7U);
            if (!target.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(target.Error());
            }

            PacketResult<int> level = ParseDecimalField(parts[baseIndex + 8U], baseIndex + 8U);
            if (!level.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(level.Error());
            }

            PacketResult<int> flag = ParseDecimalField(parts[baseIndex + 9U], baseIndex + 9U);
            if (!flag.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(flag.Error());
            }

            PacketResult<int> itemId = ParseDecimalField(parts[baseIndex + 10U], baseIndex + 10U);
            if (!itemId.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(itemId.Error());
            }

            PacketResult<int> itemType = ParseDecimalField(parts[baseIndex + 11U], baseIndex + 11U);
            if (!itemType.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(itemType.Error());
            }

            PacketResult<int> count = ParseDecimalField(parts[baseIndex + 12U], baseIndex + 12U);
            if (!count.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(count.Error());
            }

            PacketResult<std::string> memoRaw = ValidateRawStringField(parts[baseIndex + 13U], baseIndex + 13U);
            if (!memoRaw.HasValue()) {
                return PacketResult<TradeItemInfoRecord>::Failure(memoRaw.Error());
            }

            return PacketResult<TradeItemInfoRecord>::Success(TradeItemInfoRecord {
                tradeWindowIndex.Value(),
                std::move(nameRaw.Value()),
                color.Value(),
                std::move(descriptionRaw.Value()),
                imageNo.Value(),
                canUseInField.Value(),
                battleUseFlag.Value(),
                target.Value(),
                level.Value(),
                flag.Value(),
                itemId.Value(),
                itemType.Value(),
                count.Value(),
                std::move(memoRaw.Value()),
            });
        }

        [[nodiscard]] static PacketResult<std::string> ValidateRawStringField(
            std::string_view text,
            std::size_t fieldIndex)
        {
            for (std::size_t index = 0; index < text.size(); ++index) {
                if (!cg::packet::detail::IsLegacyDoubleByteLead(text[index])) {
                    continue;
                }

                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TradeItemInfo raw 字符串旧双字节字符被截断",
                        fieldIndex));
                }
                ++index;
            }

            return PacketResult<std::string>::Success(std::string(text));
        }

        [[nodiscard]] static PacketResult<bool> ValidateBuildStrings(const TradeItemInfoRecord& record)
        {
            if (record.nameRaw.find('|') != std::string::npos ||
                record.descriptionRaw.find('|') != std::string::npos ||
                record.memoRaw.find('|') != std::string::npos) {
                return PacketResult<bool>::Failure(MakeFieldError(
                    PacketErrorCode::EncodeFailed,
                    "TradeItemInfo BuildData raw 字符串包含 literal |",
                    DataFieldIndex));
            }

            return PacketResult<bool>::Success(true);
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
                    "TradeItemInfo 十进制字段为空",
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
                        "TradeItemInfo 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TradeItemInfo 十进制字段超出 int 范围",
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
     * SEND_TRLP 交易窗口宠物信息：
     * freeNameRaw|level|tribe|maxHp|maxForcePoint|vital|strength|tough|quick|magic|loyalty|skillSlot|nameRaw|imageId|attack|defense|agility|magicPower|recovery|earth|water|fire|wind|。
     */
    struct TradePetInfoData
    {
        std::string freeNameRaw;
        int level = 0;
        int tribe = 0;
        int maxHp = 0;
        int maxForcePoint = 0;
        int vital = 0;
        int strength = 0;
        int tough = 0;
        int quick = 0;
        int magic = 0;
        int loyalty = 0;
        int skillSlot = 0;
        std::string nameRaw;
        int imageId = 0;
        int attack = 0;
        int defense = 0;
        int agility = 0;
        int magicPower = 0;
        int recovery = 0;
        int earth = 0;
        int water = 0;
        int fire = 0;
        int wind = 0;
    };

    /**
     * SEND_TRLP CHAR_Trade_setTradeList 对方宠物信息		RwK {petPlace:i62} {data:es}
     * data: 固定 23 字段并必须尾随 `|`；数值为 raw signed decimal，字符串 raw escaped 保留。
     */
    struct TradePetInfo
    {
        static constexpr std::string_view Code = "RwK";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";
        static constexpr std::size_t PayloadFieldCount = 23U;
        static constexpr std::size_t PayloadPartCount = PayloadFieldCount + 1U;

        int petPlace = 0;
        TradePetInfoData data;

        TradePetInfo() = default;

        TradePetInfo(int petPlace, TradePetInfoData data)
            : petPlace(petPlace),
              data(std::move(data))
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            PacketResult<std::string> freeNameRaw = ValidateRawStringField(
                data.freeNameRaw,
                FreeNameFieldIndex);
            if (!freeNameRaw.HasValue()) {
                return PacketResult<std::string>::Failure(freeNameRaw.Error());
            }

            PacketResult<std::string> nameRaw = ValidateRawStringField(data.nameRaw, NameFieldIndex);
            if (!nameRaw.HasValue()) {
                return PacketResult<std::string>::Failure(nameRaw.Error());
            }

            std::string payload;
            const auto appendString = [&payload](std::string_view value) {
                payload.append(value);
                payload.push_back('|');
            };
            const auto appendInt = [&payload](int value) {
                payload.append(std::to_string(value));
                payload.push_back('|');
            };

            appendString(data.freeNameRaw);
            appendInt(data.level);
            appendInt(data.tribe);
            appendInt(data.maxHp);
            appendInt(data.maxForcePoint);
            appendInt(data.vital);
            appendInt(data.strength);
            appendInt(data.tough);
            appendInt(data.quick);
            appendInt(data.magic);
            appendInt(data.loyalty);
            appendInt(data.skillSlot);
            appendString(data.nameRaw);
            appendInt(data.imageId);
            appendInt(data.attack);
            appendInt(data.defense);
            appendInt(data.agility);
            appendInt(data.magicPower);
            appendInt(data.recovery);
            appendInt(data.earth);
            appendInt(data.water);
            appendInt(data.fire);
            appendInt(data.wind);

            return PacketResult<std::string>::Success(std::move(payload));
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            PacketResult<std::string> payload = BuildData();
            if (!payload.HasValue()) {
                return PacketResult<Packet>::Failure(payload.Error());
            }

            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "petPlace",
                PacketFieldType::Int62,
                std::to_string(petPlace),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                payload.Value(),
                PacketSourceTag::ServerC,
            });
            return PacketResult<Packet>::Success(std::move(packet));
        }

        static PacketResult<TradePetInfo> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<TradePetInfo>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<TradePetInfo> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(
                    PacketErrorCode::InvalidCode,
                    "TradePetInfo Parse 收到非 RwK 封包");
                error.packetCode = packet.code;
                return PacketResult<TradePetInfo>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<TradePetInfo>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "TradePetInfo Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<TradePetInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TradePetInfo Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "petPlace",
                "data",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<TradePetInfo>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TradePetInfo Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<TradePetInfo>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "TradePetInfo Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> petPlaceValue = ParseDecimalField(
                packet.fields[PetPlaceFieldIndex].value,
                PetPlaceFieldIndex);
            if (!petPlaceValue.HasValue()) {
                return PacketResult<TradePetInfo>::Failure(petPlaceValue.Error());
            }

            PacketResult<TradePetInfoData> dataValue = ParseData(packet.fields[DataFieldIndex].value);
            if (!dataValue.HasValue()) {
                return PacketResult<TradePetInfo>::Failure(dataValue.Error());
            }

            return PacketResult<TradePetInfo>::Success(TradePetInfo(
                petPlaceValue.Value(),
                std::move(dataValue.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t PetPlaceFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;
        static constexpr std::size_t FreeNameFieldIndex = 0U;
        static constexpr std::size_t NameFieldIndex = 12U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<TradePetInfoData> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<TradePetInfoData>::Failure(MakeFieldError(
                    PacketErrorCode::EmptyInput,
                    "TradePetInfo payload 为空",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<TradePetInfoData>::Failure(std::move(error));
            }

            const std::vector<std::string>& parts = split.Value();
            if (parts.size() != PayloadPartCount || !parts.back().empty()) {
                return PacketResult<TradePetInfoData>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TradePetInfo payload 段数量不匹配",
                    DataFieldIndex));
            }

            PacketResult<std::string> freeNameRaw = ValidateRawStringField(
                parts[FreeNameFieldIndex],
                FreeNameFieldIndex);
            if (!freeNameRaw.HasValue()) {
                return PacketResult<TradePetInfoData>::Failure(freeNameRaw.Error());
            }

            PacketResult<std::string> nameRaw = ValidateRawStringField(parts[NameFieldIndex], NameFieldIndex);
            if (!nameRaw.HasValue()) {
                return PacketResult<TradePetInfoData>::Failure(nameRaw.Error());
            }

            std::array<int, PayloadFieldCount> numericValues = {};
            const std::array<std::size_t, 21U> numericFieldIndexes = {
                1U,
                2U,
                3U,
                4U,
                5U,
                6U,
                7U,
                8U,
                9U,
                10U,
                11U,
                13U,
                14U,
                15U,
                16U,
                17U,
                18U,
                19U,
                20U,
                21U,
                22U,
            };
            for (const std::size_t fieldIndex : numericFieldIndexes) {
                PacketResult<int> value = ParseDecimalField(parts[fieldIndex], fieldIndex);
                if (!value.HasValue()) {
                    return PacketResult<TradePetInfoData>::Failure(value.Error());
                }
                numericValues[fieldIndex] = value.Value();
            }

            return PacketResult<TradePetInfoData>::Success(TradePetInfoData {
                std::move(freeNameRaw.Value()),
                numericValues[1],
                numericValues[2],
                numericValues[3],
                numericValues[4],
                numericValues[5],
                numericValues[6],
                numericValues[7],
                numericValues[8],
                numericValues[9],
                numericValues[10],
                numericValues[11],
                std::move(nameRaw.Value()),
                numericValues[13],
                numericValues[14],
                numericValues[15],
                numericValues[16],
                numericValues[17],
                numericValues[18],
                numericValues[19],
                numericValues[20],
                numericValues[21],
                numericValues[22],
            });
        }

        [[nodiscard]] static PacketResult<std::string> ValidateRawStringField(
            std::string_view text,
            std::size_t fieldIndex)
        {
            if (text.find('|') != std::string_view::npos) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    PacketErrorCode::EncodeFailed,
                    "TradePetInfo raw 字符串包含 literal |",
                    fieldIndex));
            }

            for (std::size_t index = 0; index < text.size(); ++index) {
                if (!cg::packet::detail::IsLegacyDoubleByteLead(text[index])) {
                    continue;
                }

                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TradePetInfo raw 字符串旧双字节字符被截断",
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
                    "TradePetInfo 十进制字段为空",
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
                        "TradePetInfo 十进制字段包含非法字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TradePetInfo 十进制字段超出 int 范围",
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
     * SEND_TRLPS CHAR_Trade_setTradeList 对方宠物技能信息		Yq {petIndex:i62} {data:es}
     * data: 0..10 条隐式技能名 `skillNameRaw|...|`；空 data 表示 0 条，非空必须尾随 `|`。
     */
    struct TradePetSkillInfo
    {
        static constexpr std::string_view Code = "Yq";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";
        static constexpr std::size_t MaxSkillCount = 10U;

        int petIndex = 0;
        std::vector<std::string> skillNameRaws;

        TradePetSkillInfo() = default;

        TradePetSkillInfo(int petIndex, std::vector<std::string> skillNameRaws)
            : petIndex(petIndex),
              skillNameRaws(std::move(skillNameRaws))
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            if (skillNameRaws.size() > MaxSkillCount) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    PacketErrorCode::EncodeFailed,
                    "TradePetSkillInfo BuildData 技能名超过 10 条",
                    DataFieldIndex));
            }

            std::string payload;
            for (std::size_t index = 0; index < skillNameRaws.size(); ++index) {
                PacketResult<std::string> skillNameRaw = ValidateRawStringField(skillNameRaws[index], index);
                if (!skillNameRaw.HasValue()) {
                    return PacketResult<std::string>::Failure(skillNameRaw.Error());
                }

                payload.append(skillNameRaws[index]);
                payload.push_back('|');
            }

            return PacketResult<std::string>::Success(std::move(payload));
        }

        [[nodiscard]] PacketResult<Packet> ToPacket() const
        {
            PacketResult<std::string> payload = BuildData();
            if (!payload.HasValue()) {
                return PacketResult<Packet>::Failure(payload.Error());
            }

            Packet packet(PacketCode(Code), Direction, PacketSourceTag::ServerC);
            packet.fields.push_back(PacketField {
                "petIndex",
                PacketFieldType::Int62,
                std::to_string(petIndex),
                PacketSourceTag::ServerC,
            });
            packet.fields.push_back(PacketField {
                "data",
                PacketFieldType::EscapedString,
                payload.Value(),
                PacketSourceTag::ServerC,
            });
            return PacketResult<Packet>::Success(std::move(packet));
        }

        static PacketResult<TradePetSkillInfo> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<TradePetSkillInfo>::Failure(parsed.Error());
            }

            return FromPacket(parsed.Value());
        }

        static PacketResult<TradePetSkillInfo> FromPacket(const Packet& packet)
        {
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(
                    PacketErrorCode::InvalidCode,
                    "TradePetSkillInfo Parse 收到非 Yq 封包");
                error.packetCode = packet.code;
                return PacketResult<TradePetSkillInfo>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<TradePetSkillInfo>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "TradePetSkillInfo Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<TradePetSkillInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TradePetSkillInfo Parse 字段数量不匹配",
                    packet.fields.size() < FieldCount ? packet.fields.size() : FieldCount));
            }

            const std::array<std::string_view, FieldCount> fieldNames = {
                "petIndex",
                "data",
            };
            const std::array<PacketFieldType, FieldCount> fieldTypes = {
                PacketFieldType::Int62,
                PacketFieldType::EscapedString,
            };

            for (std::size_t index = 0; index < FieldCount; ++index) {
                if (packet.fields[index].name != fieldNames[index]) {
                    return PacketResult<TradePetSkillInfo>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TradePetSkillInfo Parse 字段名不匹配",
                        index));
                }

                if (packet.fields[index].type != fieldTypes[index]) {
                    return PacketResult<TradePetSkillInfo>::Failure(MakeFieldError(
                        PacketErrorCode::InvalidFieldType,
                        "TradePetSkillInfo Parse 字段类型不匹配",
                        index));
                }
            }

            PacketResult<int> petIndexValue = ParseDecimalField(
                packet.fields[PetIndexFieldIndex].value,
                PetIndexFieldIndex);
            if (!petIndexValue.HasValue()) {
                return PacketResult<TradePetSkillInfo>::Failure(petIndexValue.Error());
            }

            PacketResult<std::vector<std::string>> skillNameRawsValue = ParseData(packet.fields[DataFieldIndex].value);
            if (!skillNameRawsValue.HasValue()) {
                return PacketResult<TradePetSkillInfo>::Failure(skillNameRawsValue.Error());
            }

            return PacketResult<TradePetSkillInfo>::Success(TradePetSkillInfo(
                petIndexValue.Value(),
                std::move(skillNameRawsValue.Value())));
        }

    private:
        static constexpr std::size_t FieldCount = 2U;
        static constexpr std::size_t PetIndexFieldIndex = 0U;
        static constexpr std::size_t DataFieldIndex = 1U;

        [[nodiscard]] static PacketError MakePacketError(PacketErrorCode code, std::string message)
        {
            PacketError error = PacketError::Make(code, std::move(message));
            error.packetCode = PacketCode(Code);
            error.source = PacketSourceTag::ServerC;
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

        [[nodiscard]] static PacketResult<std::vector<std::string>> ParseData(std::string_view payload)
        {
            if (payload.empty()) {
                return PacketResult<std::vector<std::string>>::Success({});
            }

            if (payload.back() != '|') {
                return PacketResult<std::vector<std::string>>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "TradePetSkillInfo payload 缺少尾随分隔符",
                    DataFieldIndex));
            }

            PacketResult<std::vector<std::string>> split = SplitPayload(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::ServerC;
                return PacketResult<std::vector<std::string>>::Failure(std::move(error));
            }

            std::vector<std::string> skillNameRaws = std::move(split.Value());
            skillNameRaws.pop_back();
            if (skillNameRaws.size() > MaxSkillCount) {
                return PacketResult<std::vector<std::string>>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "TradePetSkillInfo payload 技能名超过 10 条",
                    DataFieldIndex));
            }

            for (std::size_t index = 0; index < skillNameRaws.size(); ++index) {
                PacketResult<std::string> skillNameRaw = ValidateRawStringField(skillNameRaws[index], index);
                if (!skillNameRaw.HasValue()) {
                    return PacketResult<std::vector<std::string>>::Failure(skillNameRaw.Error());
                }
            }

            return PacketResult<std::vector<std::string>>::Success(std::move(skillNameRaws));
        }

        [[nodiscard]] static PacketResult<std::string> ValidateRawStringField(
            std::string_view text,
            std::size_t fieldIndex)
        {
            if (text.find('|') != std::string_view::npos) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    PacketErrorCode::EncodeFailed,
                    "TradePetSkillInfo raw 字符串包含 literal |",
                    fieldIndex));
            }

            for (std::size_t index = 0; index < text.size(); ++index) {
                if (!cg::packet::detail::IsLegacyDoubleByteLead(text[index])) {
                    continue;
                }

                if (index + 1U >= text.size()) {
                    return PacketResult<std::string>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TradePetSkillInfo raw 字符串旧双字节字符被截断",
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
                    "TradePetSkillInfo i62 字段为空",
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
                        "TradePetSkillInfo i62 字段包含非十进制字符",
                        fieldIndex));
                }

                const int digit = ch - '0';
                if (value > (limit - digit) / 10) {
                    return PacketResult<int>::Failure(MakeFieldError(
                        PacketErrorCode::DecodeFailed,
                        "TradePetSkillInfo i62 字段超出 int 范围",
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

    struct AddressBookGuildInfoRecord
    {
        std::string guildNameRaw;
        std::string guildTitleRaw;
    };

    /**
     * SEND_ABG SendAddressbookGuildInfo		PhdQ {data:es}
     * data: 0..60 条家族地址簿记录；每条 guildNameRaw|guildTitleRaw。
     */
    struct AddressBookGuildInfo
    {
        static constexpr std::string_view Code = "PhdQ";
        static constexpr PacketDirection Direction = PacketDirection::ServerToClient;
        static constexpr std::string_view PayloadDelimiter = "|";
        static constexpr std::size_t MaxRecordCount = 60U;
        static constexpr std::size_t RecordFieldCount = 2U;

        std::vector<AddressBookGuildInfoRecord> records;

        AddressBookGuildInfo() = default;

        explicit AddressBookGuildInfo(std::vector<AddressBookGuildInfoRecord> records)
            : records(std::move(records))
        {
        }

        [[nodiscard]] PacketResult<std::string> BuildData() const
        {
            if (records.size() > MaxRecordCount) {
                return PacketResult<std::string>::Failure(MakeFieldError(
                    PacketErrorCode::EncodeFailed,
                    "AddressBookGuildInfo BuildData 记录数超过上限",
                    DataFieldIndex));
            }

            std::vector<std::string> parts;
            parts.reserve(records.size() * RecordFieldCount);
            for (const AddressBookGuildInfoRecord& record : records) {
                parts.push_back(record.guildNameRaw);
                parts.push_back(record.guildTitleRaw);
            }

            return BuildPayload(std::span<const std::string>(parts.data(), parts.size()), PayloadDelimiter);
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
            return PacketResult<Packet>::Success(std::move(packet));
        }

        static PacketResult<AddressBookGuildInfo> Parse(std::string_view view)
        {
            PacketResult<Packet> parsed = ParsePacket(view);
            if (!parsed.HasValue()) {
                return PacketResult<AddressBookGuildInfo>::Failure(parsed.Error());
            }

            const Packet& packet = parsed.Value();
            if (packet.code.View() != Code) {
                PacketError error = MakePacketError(
                    PacketErrorCode::InvalidCode,
                    "AddressBookGuildInfo Parse 收到非 PhdQ 封包");
                error.packetCode = packet.code;
                return PacketResult<AddressBookGuildInfo>::Failure(std::move(error));
            }

            if (packet.direction != Direction) {
                return PacketResult<AddressBookGuildInfo>::Failure(MakePacketError(
                    PacketErrorCode::DecodeFailed,
                    "AddressBookGuildInfo Parse 封包方向不匹配"));
            }

            if (packet.fields.size() != FieldCount) {
                return PacketResult<AddressBookGuildInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "AddressBookGuildInfo Parse 字段数量不匹配",
                    packet.fields.empty() ? 0U : FieldCount));
            }

            const PacketField& field = packet.fields[DataFieldIndex];
            if (field.name != "data") {
                return PacketResult<AddressBookGuildInfo>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "AddressBookGuildInfo Parse 字段名不匹配",
                    DataFieldIndex));
            }

            if (field.type != PacketFieldType::EscapedString) {
                return PacketResult<AddressBookGuildInfo>::Failure(MakeFieldError(
                    PacketErrorCode::InvalidFieldType,
                    "AddressBookGuildInfo Parse 字段类型不匹配",
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

        [[nodiscard]] static PacketResult<AddressBookGuildInfo> ParseData(std::string_view payload)
        {
            PacketResult<std::vector<std::string>> split = SplitPayloadLegacyAware(payload, PayloadDelimiter);
            if (!split.HasValue()) {
                PacketError error = split.Error();
                error.packetCode = PacketCode(Code);
                error.fieldIndex = DataFieldIndex;
                error.source = PacketSourceTag::Xlsx;
                return PacketResult<AddressBookGuildInfo>::Failure(std::move(error));
            }

            std::vector<std::string> parts = std::move(split.Value());
            if ((parts.size() % RecordFieldCount) != 0U && !parts.empty() && parts.back().empty()) {
                parts.pop_back();
            }

            if ((parts.size() % RecordFieldCount) != 0U) {
                return PacketResult<AddressBookGuildInfo>::Failure(MakeFieldError(
                    PacketErrorCode::FieldCountMismatch,
                    "AddressBookGuildInfo payload 段数量不匹配",
                    DataFieldIndex));
            }

            const std::size_t recordCount = parts.size() / RecordFieldCount;
            if (recordCount > MaxRecordCount) {
                return PacketResult<AddressBookGuildInfo>::Failure(MakeFieldError(
                    PacketErrorCode::DecodeFailed,
                    "AddressBookGuildInfo payload 记录数超过上限",
                    DataFieldIndex));
            }

            std::vector<AddressBookGuildInfoRecord> records;
            records.reserve(recordCount);
            for (std::size_t index = 0; index < parts.size(); index += RecordFieldCount) {
                records.push_back(AddressBookGuildInfoRecord {
                    parts[index],
                    parts[index + 1U],
                });
            }

            return PacketResult<AddressBookGuildInfo>::Success(AddressBookGuildInfo(std::move(records)));
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
