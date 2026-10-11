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
// 原553004入口：先构造同一对象，再即时以宿主std::atexit登记同一退出回调；完整应用序未闭。
// 原函数：0x00401C10。
std::int32_t InitializeLegacyStartupValues_401C10();
// 原函数：0x00401C20。
void ConstructStartupValues_401C20();
// 原函数：0x00401C50。
void DestroyStartupValues_401C50();

// 原 sub_401C70@[0x401C70,0x401C92)。storage 必须指向可写的 16 字节对象。
LegacyCryptInt* ConstructZero_401C70(LegacyCryptInt* storage);

// 原 CryptInt_Init@[0x401CC0,0x401CE7)。
LegacyCryptInt* ConstructFromInt_401CC0(
    LegacyCryptInt* storage,
    std::int32_t value);

// 原 CryptInt_delete@[0x401CF0,0x401CF7)，只恢复 legacyVftToken。
void Destroy_401CF0(LegacyCryptInt* value);

// 原401CA0：先析构，flags bit0置位才free；返回原指针，不表示释放后仍可访问。
LegacyCryptInt* DestroyAndFree_401CA0(LegacyCryptInt* value, std::uint32_t flags);

// 原 CryptInt::SetNorVal@[0x401F50,0x401F5F)，不更新 mask 或 readCount。
void SetNorVal_401F50(LegacyCryptInt* target, std::int32_t value);

// 原赋值展开只复制 +0x04、+0x08、+0x0C；保留目标 token 与 reserved。
void CopyPayload(LegacyCryptInt* target, const LegacyCryptInt* source);

// 对应原“构造 int 临时量、复制载荷、析构临时量”的赋值形态。
void AssignFromInt(LegacyCryptInt* target, std::int32_t value);

// 原 sub_401EC0@[0x401EC0,0x401F45)，读取会修改 readCount，必要时换 mask。
// 关联原入口：0x00401EA0（跳转共用本函数读取正文）。
std::int32_t Read_401EC0(LegacyCryptInt* value);

// 原401DE0：单次读取后自增并SetNorVal，返回新值；原未使用的S32参数保留。
std::int32_t Increment_401DE0(LegacyCryptInt* value, std::int32_t unused);

// 原401E10：单读后S32加法；CQA01按RPLAN.24不专设数学回绕/异常结果。
std::int32_t Add_401E10(LegacyCryptInt* value, std::int32_t amount);
// 原函数：0x00401E20。
void AddAssign_401E20(LegacyCryptInt* value, std::int32_t amount); // 单次读取后加并SetNorVal。
// 原函数：0x00401E40。
void SubtractAssign_401E40(LegacyCryptInt* value, std::int32_t amount); // 单次读取后减并SetNorVal。

// 原401E60：单读后S32差值；异常域同CQA01，编码异或边界保持。
std::int32_t Subtract_401E60(LegacyCryptInt* value, std::int32_t amount);

// 原401E70@[401E70,401E8B)：先读右对象，再读左对象，返回lhs-rhs。
std::int32_t Difference_401E70(LegacyCryptInt* lhs, LegacyCryptInt* rhs);

// 原401E90机器乘积取低W32；新端S32普通乘法，异常域同CQA01。
std::int32_t Multiply_401E90(LegacyCryptInt* value, std::int32_t amount);

// 下列比较按有符号int32；整数比较单读，加密双值比较按各自合同独立读取。
// 原函数：0x00401D00。
bool Equal_401D00(LegacyCryptInt* value, std::int32_t expected);
// 原 CryptInt::neq@[0x401D20,0x401D40)：先读右对象，再读左对象。
bool NotEqual_401D20(LegacyCryptInt* lhs, LegacyCryptInt* rhs);
// 原函数：0x00401D40。
bool Greater_401D40(LegacyCryptInt* value, std::int32_t threshold);
// 原函数：0x00401D50。
bool GreaterCrypt_401D50(LegacyCryptInt* lhs, LegacyCryptInt* rhs); // 先右后左各读一次，即使别名。
// 原函数：0x00401D70。
bool GreaterEqual_401D70(LegacyCryptInt* value, std::int32_t minimum);
// 原函数：0x00401D80。
bool GreaterEqualCrypt_401D80(LegacyCryptInt* lhs, LegacyCryptInt* rhs); // 先右后左各读一次，即使别名。
// 原函数：0x00401DA0。
bool LessEqual_401DA0(LegacyCryptInt* value, std::int32_t maximum);
// 原函数：0x00401DB0。
bool Less_401DB0(LegacyCryptInt* value, std::int32_t maximum);
// 原函数：0x00401DC0。
bool LessCrypt_401DC0(LegacyCryptInt* lhs, LegacyCryptInt* rhs); // 先右后左独立读取，即使别名。
// 原函数：0x00401F80。
bool IntGreaterCrypt_401F80(std::int32_t lhs, LegacyCryptInt* rhs); // 右值单读，保留计数/RNG副作用。
// 原401FA0：单读右对象，S32 lhs<rhs覆盖返回AL，高24位保留读取值；条件只消费低W8。
std::uint32_t IntLessCryptRaw_401FA0(std::int32_t lhs, LegacyCryptInt* rhs);
// RHL01：原401FC0单读右对象，再按有符号S32比较lhs<=rhs；已证调用仅消费AL。
bool IntLessEqualCrypt_401FC0(std::int32_t lhs, LegacyCryptInt* rhs);

// 原401FE0经401EA0尾跳单读右对象；新端S32 lhs-rhs，异常域同CQA01。
std::int32_t IntMinusCrypt_401FE0(std::int32_t lhs, LegacyCryptInt* rhs);

} // namespace legacy_crypt_int
} // namespace cg::platform
