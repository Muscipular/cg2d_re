#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cg::ui {

struct TuiTextRun {
    std::uint32_t row = 0U;
    std::uint32_t column = 0U;
    std::string text;
};

struct TuiFrame {
    std::uint32_t rows = 24U;
    std::uint32_t columns = 80U;
    std::vector<TuiTextRun> runs;
};

} // namespace cg::ui
