#pragma once

#include <cstdint>

namespace cg::login::character_list_request_detail {

// 原 sub_512B90@0x512B90；返回值在 sub_508910 中被忽略。
std::int32_t Sub_512B90(std::uintptr_t socket);

} // namespace cg::login::character_list_request_detail
