#pragma once

#include <cstddef>
#include <cstdint>
#include <format>
#include <string>
#include <system_error>
#include <string_view>

namespace cg::text {

// 仅GBK写入边界的失败标记，仍兼容std::system_error捕获。
class GbkStorageError final : public std::system_error {
public:
    using std::system_error::system_error;
};

// UWF01：格式与实参只接收宽文本，结果直接写入动态UTF-16正文；原0x0053D21D的私有显示缓冲调用由此承接。
template<typename... Args>
std::int32_t FormatTo(std::wstring& destination, const std::wstring_view format, Args&&... args)
{
    destination = std::vformat(format, std::make_wformat_args(args...));
    return static_cast<std::int32_t>(destination.size());
}

// UWF01：内部正文为UTF-16；窄字节仅在明确GBK／旧A接口边界转换。
[[nodiscard]] std::wstring FromGbkWide(std::string_view gbk);
[[nodiscard]] std::wstring FromLegacyDisplayWide(std::string_view bytes);
[[nodiscard]] std::string ToGbk(std::wstring_view utf16);
// 整串写回旧GBK字段；超容量与编码失败均抛GbkStorageError，失败前不发布。
void StoreGbk(char* destination, std::size_t capacity, std::wstring_view utf16);
[[nodiscard]] std::size_t Next(std::wstring_view utf16, std::size_t offset) noexcept;
[[nodiscard]] std::size_t Previous(std::wstring_view utf16, std::size_t offset) noexcept;
[[nodiscard]] std::size_t Prefix(std::wstring_view utf16, std::size_t unitBudget) noexcept;
[[nodiscard]] std::size_t Count(std::wstring_view utf16) noexcept;

} // namespace cg::text
