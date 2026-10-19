#pragma once

#include "LoginSavedState.hpp"

#include "LoginMessageDependencies.hpp"
#include "../../ui/UiControlRecord.hpp"

#include "cg/audio/LegacyAudio.hpp"

#include <cstdint>

namespace cg::login {

// VG01：共享生成版本正文；文字过长时向左调整，保留原Y、颜色与图层。
std::int32_t DrawClientVersion(std::int16_t preferredX, std::int16_t y, std::int8_t color);

} // namespace cg::login
