#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform {

// 原 x86 CryptInt 的固定内存布局。legacyVftToken 保存原 32 位地址位模式，
// 不是新端可解引用的虚表指针。
struct LegacyCryptInt {
    std::uint32_t legacyVftToken;
    std::uint32_t encoded;
    std::uint32_t mask;
    std::uint8_t readCount;
    std::uint8_t reserved[3];
};
static_assert(sizeof(LegacyCryptInt) == 16);
static_assert(offsetof(LegacyCryptInt, legacyVftToken) == 0x00);
static_assert(offsetof(LegacyCryptInt, encoded) == 0x04);
static_assert(offsetof(LegacyCryptInt, mask) == 0x08);
static_assert(offsetof(LegacyCryptInt, readCount) == 0x0C);
static_assert(offsetof(LegacyCryptInt, reserved) == 0x0D);

inline constexpr std::uint32_t kLegacyCryptIntVftToken = 0x0054D3A0u;

namespace legacy_crypt_int {

// 原CRT 553004：先构造5939C8/5939D8，退出按相反顺序销毁。
void ConstructStartupValues_401C20();
void DestroyStartupValues_401C50();

// 原 sub_401C70@[0x401C70,0x401C92)。storage 必须指向可写的 16 字节对象。
LegacyCryptInt* ConstructZero_401C70(LegacyCryptInt* storage);

// 原 CryptInt_Init@[0x401CC0,0x401CE7)。
LegacyCryptInt* ConstructFromInt_401CC0(
    LegacyCryptInt* storage,
    std::int32_t value);

// 原 CryptInt_delete@[0x401CF0,0x401CF7)，只恢复 legacyVftToken。
void Destroy_401CF0(LegacyCryptInt* value);

// 原 CryptInt::SetNorVal@[0x401F50,0x401F5F)，不更新 mask 或 readCount。
void SetNorVal_401F50(LegacyCryptInt* target, std::int32_t value);

// 原赋值展开只复制 +0x04、+0x08、+0x0C；保留目标 token 与 reserved。
void CopyPayload(LegacyCryptInt* target, const LegacyCryptInt* source);

// 对应原“构造 int 临时量、复制载荷、析构临时量”的赋值形态。
void AssignFromInt(LegacyCryptInt* target, std::int32_t value);

// 原 sub_401EC0@[0x401EC0,0x401F45)，读取会修改 readCount，必要时换 mask。
std::int32_t Read_401EC0(LegacyCryptInt* value);

// 原401DE0：单次读取后自增并SetNorVal，返回新值；原未使用的S32参数保留。
std::int32_t Increment_401DE0(LegacyCryptInt* value, std::int32_t unused);

// 原 CryptInt::add@[0x401E10,0x401E1C)，只读取一次后作 int32 位模式回绕加法。
std::int32_t Add_401E10(LegacyCryptInt* value, std::int32_t amount);
void AddAssign_401E20(LegacyCryptInt* value, std::int32_t amount); // 单次读取后加并SetNorVal。
void SubtractAssign_401E40(LegacyCryptInt* value, std::int32_t amount); // 单次读取后减并SetNorVal。

// 原 CryptInt::dec@[0x401E60,0x401E6C)，只读取一次后作完整 W32 差值。
std::int32_t Subtract_401E60(LegacyCryptInt* value, std::int32_t amount);

// 原401E70@[401E70,401E8B)：先读右对象，再读左对象，返回lhs-rhs。
std::int32_t Difference_401E70(LegacyCryptInt* lhs, LegacyCryptInt* rhs);

// 原 CryptInt::mul@[0x401E90,0x401E9D)，单次读取后返回乘积低 W32。
std::int32_t Multiply_401E90(LegacyCryptInt* value, std::int32_t amount);

// 下列比较按有符号int32；整数比较单读，加密双值比较按各自合同独立读取。
bool Equal_401D00(LegacyCryptInt* value, std::int32_t expected);
// 原 CryptInt::neq@[0x401D20,0x401D40)：先读右对象，再读左对象。
bool NotEqual_401D20(LegacyCryptInt* lhs, LegacyCryptInt* rhs);
bool Greater_401D40(LegacyCryptInt* value, std::int32_t threshold);
bool GreaterCrypt_401D50(LegacyCryptInt* lhs, LegacyCryptInt* rhs); // 先右后左各读一次，即使别名。
bool GreaterEqual_401D70(LegacyCryptInt* value, std::int32_t minimum);
bool GreaterEqualCrypt_401D80(LegacyCryptInt* lhs, LegacyCryptInt* rhs); // 先右后左各读一次，即使别名。
bool LessEqual_401DA0(LegacyCryptInt* value, std::int32_t maximum);
bool Less_401DB0(LegacyCryptInt* value, std::int32_t maximum);
bool LessCrypt_401DC0(LegacyCryptInt* lhs, LegacyCryptInt* rhs); // 先右后左独立读取，即使别名。
bool IntGreaterCrypt_401F80(std::int32_t lhs, LegacyCryptInt* rhs); // 右值单读，保留计数/RNG副作用。
bool IntLessEqualCrypt_401FC0(std::int32_t lhs, LegacyCryptInt* rhs);

// 原 401FE0 经 401EA0 尾跳读取右侧一次，再作完整 W32 的 lhs-rhs。
std::int32_t IntMinusCrypt_401FE0(std::int32_t lhs, LegacyCryptInt* rhs);

} // namespace legacy_crypt_int
} // namespace cg::platform
