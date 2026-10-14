#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <format>
#include <stdexcept>
#include <vector>
#include <string>
#include <system_error>
#include <string_view>

namespace cg::text {

// 仅GBK写入边界的失败标记，仍兼容std::system_error捕获。
class GbkStorageError final : public std::system_error {
public:
    using std::system_error::system_error;
};

// UTL02：界面内部UTF-16；以下窄接口仅供CRT及既有协议接口的局部UTF-8视图。
[[nodiscard]] std::wstring ToUtf16(std::string_view utf8);
// 仅用于已明确GBK的协议／资源字段，不尝试猜测输入编码。
// 仅旧A显示来源；保留原系统ACP解释，不据此认定原文编码。
[[nodiscard]] std::string ToGbk(std::string_view utf8);
// 整串写回旧GBK字段；超容量与编码失败均抛GbkStorageError，失败前不发布。
void StoreGbk(char* destination, std::size_t capacity, std::string_view utf8);

// UWF01：格式与实参只接收宽文本，结果直接写入动态UTF-16正文；原0x0053D21D的私有显示缓冲调用由此承接。
template<typename... Args>
std::int32_t FormatTo(std::wstring& destination, const std::wstring_view format, Args&&... args)
{
    destination = std::vformat(format, std::make_wformat_args(args...));
    return static_cast<std::int32_t>(destination.size());
}

// UTL02：界面权威正文为动态UTF-16；窄字符串只在明确兼容边界转换。
[[nodiscard]] std::wstring FromGbkWide(std::string_view gbk);
[[nodiscard]] std::wstring FromLegacyDisplayWide(std::string_view bytes);
[[nodiscard]] std::string ToGbk(std::wstring_view utf16);
void StoreGbk(char* destination, std::size_t capacity, std::wstring_view utf16);
[[nodiscard]] std::size_t Next(std::wstring_view utf16, std::size_t offset) noexcept;
[[nodiscard]] std::size_t Previous(std::wstring_view utf16, std::size_t offset) noexcept;
[[nodiscard]] std::size_t Prefix(std::wstring_view utf16, std::size_t unitBudget) noexcept;
[[nodiscard]] std::size_t Count(std::wstring_view utf16) noexcept;

// 参数为有效UTF-8，偏移／预算均为字节；返回完整字符边界。
[[nodiscard]] std::size_t Next(std::string_view utf8, std::size_t offset) noexcept;
[[nodiscard]] std::size_t Previous(std::string_view utf8, std::size_t offset) noexcept;
[[nodiscard]] std::size_t Prefix(std::string_view utf8, std::size_t byteBudget) noexcept;
[[nodiscard]] std::size_t Count(std::string_view utf8) noexcept;
std::size_t Copy(char* destination, std::size_t capacity, std::string_view utf8) noexcept;

// 旧多NUL提示块仍连续存储；各可读字面量的隐式NUL直接构成行分隔。
template<std::size_t... Sizes>
[[nodiscard]] constexpr auto JoinLines(const char (&... lines)[Sizes]) noexcept
{
    std::array<char, (Sizes + ...)> result{};
    std::size_t offset = 0;
    const auto append = [&](const auto& line) {
        for (const char byte : line) {
            result[offset++] = byte;
        }
    };
    (append(lines), ...);
    return result;
}

} // namespace cg::text
