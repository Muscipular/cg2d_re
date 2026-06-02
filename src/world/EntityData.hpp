#pragma once

#include "world/EntityId.hpp"

namespace cg::world {

// EntityData 保持 world 中立实体数据，不承载未确认的业务字段。
struct EntityData {
    EntityId id{};
};

} // namespace cg::world
