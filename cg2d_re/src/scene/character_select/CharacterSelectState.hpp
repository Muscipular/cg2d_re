#pragma once

#include "cg/Yobi/YobiRuntime.hpp"

#include <array>
#include <cstdint>

namespace cg::input {
class InputContext;
}

namespace cg::character_select::state {

// 两个角色选择场景共享的原版本载荷，不建立第二份语义存储。
inline constexpr wchar_t kVersion20[] = L"20031229_20"; // 原 0x55C80C。
inline constexpr wchar_t kVersion30[] = L"20031229_30"; // 原 0x55C800。

// 跨角色选择相关函数共享的原存储；保持唯一实例供后续真实依赖接入。
extern std::uint8_t gGraphicVersionOverride58B6A0; // 原全局：0x0058B6A0。 原覆盖W8，FF时才读取独立Puk版本。
extern std::uint8_t gCreationCharacterFamily_9A57E0; // 原 0x9A57E0，创建界面版本门禁 W8。
extern std::uint8_t gCreationCharacterIndex_9A4CFC; // 原 0x9A4CFC，创建界面选择 W8。
extern std::uint8_t gCreationVariantIndex_9A5B00; // 原 0x9A5B00，创建界面状态 W8。
extern std::array<cg::yobi::YobiAction*, 2> gCreateActions; // 原 0x9A4CB4/0x9A4CB8，非拥有。
// 创建页463100/4631D0及确认快照共用；两个W32同时被按字节访问。
extern input::InputContext gCreationInput9A51E8; // 原全局：0x009A51E8。
extern std::uint32_t gCreationAttributePointBytes_9A5FD4; // 原全局：0x009A5FD4。
extern std::uint32_t gCreationElementPointBytes_9A50D0; // 原全局：0x009A50D0。
extern std::uint8_t gCreationFifthAttributePoints_9A5FD8; // 原全局：0x009A5FD8。
extern std::uint8_t gCreationStyleIndex_9A5668; // 原全局：0x009A5668。
extern std::uint8_t gCreationColorIndex_9A4B80; // 原全局：0x009A4B80。
extern std::uint8_t gCreationRemainingAttributePoints_9A4D04; // 原全局：0x009A4D04。
extern std::uint8_t gCreationRemainingElementPoints_9A50C8; // 原全局：0x009A50C8。
extern std::uint8_t gCreationGraphicGeneration_9A4B88; // 原全局：0x009A4B88。
extern std::int32_t gCreationAttributePhase_9A5AFC; // 原全局：0x009A5AFC。
extern cg::yobi::YobiAction* gCharacterModalAction; // 原 0x9A6E64，原生指针。
extern std::wstring gSelectedCharacterName;     // 原 0xDA391C。
extern std::wstring gDeleteCharacterName;       // 原 0xDA3A9C。

} // namespace cg::character_select::state
