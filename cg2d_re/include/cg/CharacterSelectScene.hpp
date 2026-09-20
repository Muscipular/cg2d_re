#pragma once

#include <cstdint>

namespace cg::character_select {

// 原 sub_45DD50@[0x45DD50,0x45DD5A)。
void ResetCharacterListProgress_45DD50();

// 原 sub_45DD60@[0x45DD60,0x45DED9)。
std::int32_t UpdateCharacterListProgress_45DD60();

// 原 sub_45D7D0@[0x45D7D0,0x45D7DA)；只清认证失败提示阶段。
void ResetAuthenticationFailure_45D7D0();

// 原 sub_45D7E0@[0x45D7E0,0x45D97D)。
std::int32_t UpdateAuthenticationFailure_45D7E0();

// 原 sub_45DBF0@[0x45DBF0,0x45DBFA)；只清角色删除进度阶段。
void ResetCharacterDeletionProgress_45DBF0();

// 原 sub_45DC00@[0x45DC00,0x45DD45)。
std::int32_t UpdateCharacterDeletionProgress_45DC00();

// 原 sub_460D00@[0x460D00,0x460FD8)；前置条件是 slot 位于 [0, 1]。
std::int32_t ProcessCharacterPreview_460D00(std::uint32_t slot);

// 原 sub_462090@[0x462090,0x462327)。
std::int32_t ProcessCharacterModal_462090(
    std::int32_t x,
    std::int32_t y);

// 原 sub_462330@[0x462330,0x4625A0)。
std::int32_t ProcessScene_462330();

// 原 sub_460FF0@[0x460FF0,0x461A77)。
std::int32_t ProcessScene_460FF0();

// 原 sub_461B30@[0x461B30,0x46208F)；前置条件是 slot 位于 [0, 1]。
std::int32_t ProcessCharacterSlot_461B30(
    std::uint32_t slot,
    std::int32_t mode);

} // namespace cg::character_select
