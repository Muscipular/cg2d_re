#pragma once

#include <cstdint>

namespace cg::login::character_deletion_request_detail {

// 原 sub_512AB0@0x512AB0；返回值在 sub_508F70 中被忽略。
std::int32_t Sub_512AB0(
    std::uintptr_t socket,
    std::int32_t registNumber);

} // namespace cg::login::character_deletion_request_detail
