#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace cg::platform {

// 原 x86 CryptInt 的固定内存布局。legacyVftToken 保存原 32 位地址位模式，
// 不是新端可解引用的虚表指针。
class LegacyCryptInt {
public:
    // 原0x00401C70：默认构造零值，只消费一次RNG。
    LegacyCryptInt();
    // 原0x00401CF0：结束对象寿命，只恢复原token。
    // 原0x00401CA0销毁部分归入析构；动态释放沿标准delete表达式。
    ~LegacyCryptInt();

    // 原字节载荷无读取副作用；用于calloc零填充与既有原始快照。
    struct RawPayload {
        std::uint32_t legacyVftToken;
        std::uint32_t encoded;
        std::uint32_t mask;
        std::uint8_t readCount;
        // 原+0x0D～+0x0F：由编译器尾部padding承接。
    };
    explicit LegacyCryptInt(const RawPayload& raw) noexcept; // 不消费RNG。
    void RestoreRaw(const RawPayload& raw) noexcept; // 恢复四个有效字段，不改变寿命。

    explicit LegacyCryptInt(std::int32_t value); // 原401CC0：一次RNG构造。
    explicit LegacyCryptInt(std::uint32_t value); // 同一W32位模式构造。
    LegacyCryptInt(const LegacyCryptInt&) = default; // 同类原始字段复制，不读取或取RNG。
    // 原0x00438270：仅复制三个载荷字段，保留目标token，不读取或取RNG。
    LegacyCryptInt& operator=(const LegacyCryptInt& source) noexcept;

    LegacyCryptInt& operator=(std::int32_t value); // 原整数临时构造／载荷赋值。
    LegacyCryptInt& operator=(std::uint32_t value); // 只发布三个载荷字段，保留目标token。

    // 每次转换单读并消费原计数／刷新；标量两侧算术、比较沿C++整数提升。
    // 原标量比较：0x00401D00、0x00401D40、0x00401D70、0x00401DA0、0x00401DB0、0x00401F80、0x00401FA0、0x00401FC0。
    // 原401FA0的八个已知调用只消费AL；整数左侧<沿单次转换与内建比较。
    // 原标量算术：0x00401E10、0x00401E60、0x00401E90、0x00401FE0；通过转换与内建operator承接。
    operator std::int32_t();
    explicit operator std::uint32_t();
    // 原函数：0x00401E20。
    LegacyCryptInt& operator+=(std::int32_t amount); // 原401E20，单读后SetNorVal。
    LegacyCryptInt& operator+=(std::uint32_t amount); // 无符号W32加后写回位模式。
    // 原函数：0x00401E40。
    LegacyCryptInt& operator-=(std::int32_t amount); // 原401E40，单读后SetNorVal。
    LegacyCryptInt& operator-=(std::uint32_t amount); // 无符号W32减后写回位模式。
    LegacyCryptInt& operator++(); // 原0x00401DE0：返回本对象。
    std::int32_t operator++(int); // 单读递增，返回递增前明文。

    // 同类双值运算先右后左各读一次，即使两引用指向同一对象。
    friend std::int32_t operator+(LegacyCryptInt& lhs, LegacyCryptInt& rhs);
    // 原0x00401E70@[401E70,401E8B)：右→左读取后求差。
    friend std::int32_t operator-(LegacyCryptInt& lhs, LegacyCryptInt& rhs);
    friend std::int32_t operator*(LegacyCryptInt& lhs, LegacyCryptInt& rhs);
    friend bool operator==(LegacyCryptInt& lhs, LegacyCryptInt& rhs);
    // 原0x00401D20@[0x401D20,0x401D40)：右→左读取后比较。
    friend bool operator!=(LegacyCryptInt& lhs, LegacyCryptInt& rhs);
    // 原函数：0x00401D50。
    friend bool operator>(LegacyCryptInt& lhs, LegacyCryptInt& rhs);
    // 原函数：0x00401D80。
    friend bool operator>=(LegacyCryptInt& lhs, LegacyCryptInt& rhs);
    // 原函数：0x00401DC0。
    friend bool operator<(LegacyCryptInt& lhs, LegacyCryptInt& rhs);
    friend bool operator<=(LegacyCryptInt& lhs, LegacyCryptInt& rhs);

    // 原 CryptInt::SetNorVal@[0x401F50,0x401F5F)，不更新 mask 或 readCount。
    void SetNorVal_401F50(std::int32_t value);

    // 原0x00401EC0@[0x401EC0,0x401F45)，原0x00401EA0跳转共用读取正文；读取会改计数／mask。
    std::int32_t value();

    std::uint32_t legacyVftToken; // 原成员偏移：+0x00。
    std::uint32_t encoded; // 原成员偏移：+0x04。
    std::uint32_t mask; // 原成员偏移：+0x08。
    std::uint8_t readCount; // 原成员偏移：+0x0C。
    // 原+0x0D～+0x0F：由编译器尾部padding承接。

private:
    // 明文转原W32位模式，供encoded异或与mask位模式转换使用。
    static std::uint32_t ToBits(std::int32_t value);
    // 解码后的W32位模式恢复为有符号明文。
    static std::int32_t FromBits(std::uint32_t value);
    // 沿共享NextRandom一次取mask，构造与读计数刷新共用。
    static std::uint32_t NextMask();
};
static_assert(sizeof(LegacyCryptInt) == 16);
static_assert(std::is_standard_layout_v<LegacyCryptInt>);
static_assert(!std::is_trivially_destructible_v<LegacyCryptInt>);
static_assert(sizeof(LegacyCryptInt::RawPayload) == 16);
static_assert(std::is_trivially_copyable_v<LegacyCryptInt::RawPayload>);
static_assert(offsetof(LegacyCryptInt, legacyVftToken) == 0x00);
static_assert(offsetof(LegacyCryptInt, encoded) == 0x04);
static_assert(offsetof(LegacyCryptInt, mask) == 0x08);
static_assert(offsetof(LegacyCryptInt, readCount) == 0x0C);

inline constexpr std::uint32_t kLegacyCryptIntVftToken = 0x0054D3A0u;

} // namespace cg::platform
