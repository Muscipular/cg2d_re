#pragma once

#include <cstdint>
#include <string_view>

namespace cg::network {
class CgConnection;
}

namespace cg::session { class ClientSession; }

namespace cg::character_select {

// 原 [0x51B740,0x51B7CE)：采样覆盖/Puk版本后按类别决定保留或回退图号。
// LDT01：以当前版本低8值选择图号；原图/分类3减5000/既有51B910回退。
std::int32_t ResolveCharacterImageForVersion(
    session::ClientSession& session, std::int32_t imageNo);


// 原 sub_45DD50@[0x45DD50,0x45DD5A)。
void ResetCharacterListProgress_45DD50(network::CgConnection& connection);

// 原 sub_45DD60@[0x45DD60,0x45DED9)。
std::int32_t UpdateCharacterListProgress_45DD60(network::CgConnection& connection);

// 原 sub_45D7D0@[0x45D7D0,0x45D7DA)；只清认证失败提示阶段。
void ResetAuthenticationFailure_45D7D0(network::CgConnection& connection);

// 原 sub_45D7E0@[0x45D7E0,0x45D97D)。
std::int32_t UpdateAuthenticationFailure_45D7E0(network::CgConnection& connection);

// 原 sub_45DBF0@[0x45DBF0,0x45DBFA)；只清角色删除进度阶段。
void ResetCharacterDeletionProgress_45DBF0(network::CgConnection& connection);

// 原 sub_45DC00@[0x45DC00,0x45DD45)。
std::int32_t UpdateCharacterDeletionProgress_45DC00(network::CgConnection& connection);

// 原 sub_45DEE0@[0x45DEE0,0x45E089)。
std::int32_t ProcessConfirmationDialog_45DEE0(
    network::CgConnection& connection,
    std::int32_t x,
    std::int32_t y,
    std::wstring_view text,
    std::int8_t color,
    std::int32_t width,
    std::int32_t height);

// 原 sub_45E090@[0x45E090,0x45E0B1)。
std::int32_t ProcessDefaultConfirmationDialog_45E090(
    network::CgConnection& connection,
    std::int32_t x,
    std::int32_t y,
    std::wstring_view text,
    std::int8_t color);

// 原 sub_45E0C0@[0x45E0C0,0x45E3C1)。
std::int32_t ProcessCharacterDeletionWarning_45E0C0(
    network::CgConnection& connection,
    std::int32_t x,
    std::int32_t y);

// 原 sub_460D00@[0x460D00,0x460FD8)；前置条件是 slot 位于 [0, 1]。
void ProcessCharacterPreview_460D00(network::CgConnection& connection, std::uint32_t slot);

// 原 sub_462090@[0x462090,0x462327)。
std::int32_t ProcessCharacterModal_462090(
    network::CgConnection& connection,
    std::int32_t x,
    std::int32_t y);

// 原 sub_462330@[0x462330,0x4625A0)。
std::int32_t ProcessScene_462330(network::CgConnection& connection);

// 原 sub_45E3D0@[0x45E3D0,0x45E5E2)；低调色板能力的版本升级场景。
std::int32_t ProcessLegacyVersionScene_45E3D0(network::CgConnection& connection);

// 原 sub_460FF0@[0x460FF0,0x461A77)。
std::int32_t ProcessScene_460FF0(network::CgConnection& connection);

// 原 sub_461B30@[0x461B30,0x46208F)；前置条件是 slot 位于 [0, 1]。
std::int32_t ProcessCharacterSlot_461B30(
    network::CgConnection& connection,
    std::uint32_t slot,
    std::int32_t mode);

} // namespace cg::character_select
