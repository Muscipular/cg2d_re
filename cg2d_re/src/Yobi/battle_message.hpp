#pragma once

#include "base.hpp"

#include <array>
#include <cstdint>

namespace cg::login {

namespace detail {

inline constexpr std::array<const char*, 25> kBattleMHeaders553E94 { // 原常量参考：0x00553E94。
    "SKL", "ESC", "CLR", "SUP", "POS", "MON", "EQU", "ITM", "CMB", "PSN",
    "ANR", "PUE", "ACT", "CFP", "CHT", "PNO", "PLV", "CBC", "PDN", "REB",
    "BST", "GMN", "RID", "DSK", "END",
};

// 共享原状态，只在本组实现中定义；原全局：0x005AAA64。
extern std::int32_t gBattleMCasterPosition5AAA64;

// 共享原状态，只在本组实现中定义；原全局：0x005A4A20。
extern std::uint32_t gBattleMResultFlags5A4A20;

// 共享原状态，只在本组实现中定义；原全局：0x005ABA7C。
extern std::int32_t gBattleMValue5ABA7C;

// 共享原状态，只在本组实现中定义；原全局：0x005AAA40。
extern std::int32_t gBattleMTargetPosition5AAA40;

// 共享原状态，只在本组实现中定义；原全局：0x005AAA78。
extern std::int32_t gBattleMSkill5AAA78;

// 共享原状态，只在本组实现中定义；原全局：0x005A4A38。
extern std::int32_t gBattleMTechnique5A4A38;

// 共享原状态，只在本组实现中定义；原全局：0x005A4A34。
extern std::int32_t gBattleMValue5A4A34;

// 共享原状态，只在本组实现中定义；原全局：0x005ABA90。
extern std::int32_t gBattleMValue5ABA90;

// 共享原状态，只在本组实现中定义；原全局：0x005ABA84。
extern std::uint32_t gBattleMFlags5ABA84;

// 共享原状态，只在本组实现中定义；原全局：0x005A4A28。
extern std::int32_t gBattleMValue5A4A28;

// 共享原状态，只在本组实现中定义；原全局：0x0059FA14。
extern std::int32_t gBattleMMessageValue59FA14;

// 共享原状态，只在本组实现中定义；原全局：0x005A4A2C。
extern std::int32_t gBattleMSkillLevel5A4A2C;

// 共享原状态，只在本组实现中定义；原全局：0x005A3A18。
extern std::int32_t gBattleMSkillLevelIndex5A3A18;

enum class BattleMDataHeader : std::int32_t {
    Action = 12, ConsumeFp = 13, Chat = 14, ChangeBody = 17, AppendSkill = 23, End = 24,
};

enum class BattleMStatusHeader : std::int32_t {
    Equipment = 6, StatusValue = 9, Debuff = 10, Power = 11, Rebirth = 19, QueuedValue = 20,
};

} // namespace detail

} // namespace cg::login
