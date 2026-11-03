#pragma once
#include <string>
#include <string_view>

#include <cstdint>

namespace cg::network { class CgConnection; }
namespace cg::session { class ClientSession; }

namespace cg::login::detail {

// 原 sub_52D310@[0x52D310,0x52D457)：编码旧保存数据字节流。
void EncodeLegacyBytes_52D310(
    std::uint8_t* const source,
    const std::int32_t sourceBytes,
    const std::int32_t mode,
    std::uint8_t* const destination,
    std::int32_t* const encodedBytes,
    const std::int32_t destinationCapacity);

// 原 sub_52D460@[0x52D460,0x52D569)：解码旧保存数据字节流。
std::int32_t DecodeLegacyBytes_52D460(
    const std::uint8_t* const source,
    const std::int32_t sourceBytes,
    const std::int32_t mode,
    std::uint8_t* const destination,
    std::int32_t* const decodedBytes);

// 原函数：0x005201B0。
std::int32_t RestoreCurrentSavedState_5201B0(session::ClientSession& session);

// 原函数：0x00533300。
std::int32_t InitializeSavedState_533300();

// 原 [0x520510,0x520545)：四项显示状态交错读写当前保存记录。
std::int32_t CaptureCharacterDisplaySettings_520510(session::ClientSession& session, const std::int32_t selected);

// 原 [0x520550,0x52058D)：按两组读取、零扩展并发布四项显示状态。
std::int32_t RestoreCharacterDisplaySettings_520550(session::ClientSession& session, const std::int32_t selected);

// 原 [0x520750,0x520764)：只存 BGM 目标低 W16，返回原索引。
std::int32_t CaptureCharacterBgmTarget_520750(const std::int32_t selected);

// 原 [0x520770,0x520784)：从当前记录的 W16 零扩展恢复同一 BGM 目标。
std::int32_t RestoreCharacterBgmTarget_520770(const std::int32_t selected);

// 原 sub_520470@[0x520470,0x5204BF)：将音频设置写入当前保存记录。
std::int32_t CaptureAudioPreferences_520470();

// 原 sub_520590@[0x520590,0x5205D5)：将六项标量低字节写入当前记录。
std::uint8_t CaptureClientPreferenceBytes_520590();

// 原 sub_520640@[0x520640,0x520681)：保存 24 个输入上下文的零结尾文本。
std::uint32_t CapturePresetChatTexts_520640(session::ClientSession& session);

// 原 sub_5206F0@[0x5206F0,0x5206FB)：保存单字节状态并返回 AL。
std::uint8_t CaptureContactSortMode_5206F0();

// 原 sub_520720@[0x520720,0x52072B)：保存 +0x315 的低字节并返回 AL。
std::uint8_t Sub_520720();

// 原 [0x520160,0x5201A8)：各保存叶按序执行，末尾透传持久化返回。
std::int32_t SaveCurrentClientPreferences_520160(network::CgConnection& connection);

// 原函数：0x00520200。
std::int32_t SaveCurrentLoginRecord_520200();

std::int32_t ReadNewSavedPhysicalSlot105C368();

std::int32_t ReadMappedSavedPhysicalSlot105C36C(
    const std::int32_t index);

const wchar_t* ReadSelectedSavedDirectory();

// 原函数：0x005336D0。
std::int32_t LoadSelectedLoginRecord_5336D0();

// 原函数：0x00533860。
std::int32_t DeleteSavedAccountFiles_533860(const std::int32_t physicalSlot);

void RestoreCapability4CurrentSavedRecord(session::ClientSession& session);

// ALM02：参数已按50字节验证；同步当前记录和选中账号槽，仅写内存，供返回登录界面使用。
void CacheAutoLoginInputValues(std::wstring_view account, std::wstring_view password);

// 当前记录的密码／账号缓存；分别对应FF494F／FF4982。
std::wstring& GetCurrentSavedInputValueFF494F();

std::wstring& GetCurrentSavedInputValueFF4982();

std::wstring& GetCurrentSavedStallNameFF4D18();

std::wstring& GetCurrentSavedStallDescriptionFF4D28();

// 当前存档索引状态；借用存档模块唯一存储。
extern std::int32_t& gSelectedSavedSlot105C394;

// 当前存档索引状态；借用存档模块唯一存储。
extern std::int32_t& gVisibleSavedSlotCount105C364;

// 原全局：0x0105C364。

// 原函数：0x00533570。
void Sub_533570(
    std::wstring_view source,
    const std::int32_t logicalSlot);

// 原函数：0x005335B0。
void WriteSavedPassword_5335B0(
    std::wstring_view source,
    const std::int32_t logicalSlot);

// 原函数：0x00533640。
void WriteSavedAccount_533640(
    std::wstring_view source,
    const std::int32_t logicalSlot);

// 原函数：0x005335F0。
std::wstring& ReadSavedPassword_5335F0(
    std::wstring& output,
    const std::int32_t logicalSlot);

// 原函数：0x00533680。
std::wstring& ReadSavedAccount_533680(
    std::wstring& output,
    const std::int32_t logicalSlot);

// 原函数：0x00533730。
void SaveCredentialRecordsWithoutPasswords_533730();

} // namespace cg::login::detail
