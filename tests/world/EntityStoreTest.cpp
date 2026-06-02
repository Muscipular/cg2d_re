#include "world/EntityStore.hpp"

#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>

namespace {

static_assert(!std::is_convertible_v<int, cg::world::EntityId>);

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

int TestDefaultEntityIdIsInvalid()
{
    const cg::world::EntityId id;
    return Expect(!id.IsValid(), "默认 EntityId 应无效");
}

int TestCreateAndFind()
{
    int failures = 0;
    cg::world::EntityStore store;

    cg::world::EntityData data;
    const cg::world::EntityId id = store.Create(data);

    failures += Expect(id.IsValid(), "Create 返回有效 EntityId");
    failures += Expect(store.ActiveCount() == 1U, "Create 后 active 为 1");

    cg::world::EntityData* found = store.Find(id);
    failures += Expect(found != nullptr, "Create 后可 Find");
    if (found != nullptr) {
        failures += Expect(found->id == id, "EntityData.id 写入返回 id");
    }

    const cg::world::EntityStore& constStore = store;
    failures += Expect(constStore.Find(id) == found, "const Find 返回同一实体");
    return failures;
}

int TestDestroyInvalidatesOldId()
{
    int failures = 0;
    cg::world::EntityStore store;

    const cg::world::EntityId id = store.Create();
    failures += Expect(store.Destroy(id), "Destroy 有效 id 成功");
    failures += Expect(store.ActiveCount() == 0U, "Destroy 后 active 为 0");
    failures += Expect(store.Find(id) == nullptr, "Destroy 后旧 id 不可访问");
    failures += Expect(!store.Destroy(id), "重复 Destroy 旧 id 失败");
    return failures;
}

int TestSlotReuseChangesGeneration()
{
    int failures = 0;
    cg::world::EntityStore store;

    const cg::world::EntityId first = store.Create();
    failures += Expect(store.Destroy(first), "Destroy first 成功");

    const cg::world::EntityId second = store.Create();
    failures += Expect(second.Index() == first.Index(), "复用被删除槽位");
    failures += Expect(second.Generation() != first.Generation(), "复用槽位 generation 变化");
    failures += Expect(store.Find(first) == nullptr, "复用后旧 id 仍不可访问");
    failures += Expect(store.Find(second) != nullptr, "复用后新 id 可访问");
    return failures;
}

int TestCreateManyEntities()
{
    constexpr std::size_t EntityCount = 16000U;

    int failures = 0;
    cg::world::EntityStore store;
    store.Reserve(EntityCount);

    std::vector<cg::world::EntityId> ids;
    ids.reserve(EntityCount);
    for (std::size_t i = 0; i < EntityCount; ++i) {
        ids.push_back(store.Create());
    }

    failures += Expect(store.ActiveCount() == EntityCount, "创建 16000 个实体后 active 正确");
    for (const cg::world::EntityId id : ids) {
        failures += Expect(store.Find(id) != nullptr, "批量创建实体均可访问");
    }

    for (const cg::world::EntityId id : ids) {
        failures += Expect(store.Destroy(id), "批量 Destroy 成功");
    }
    failures += Expect(store.ActiveCount() == 0U, "批量删除后 active 为 0");
    return failures;
}

int TestClear()
{
    int failures = 0;
    cg::world::EntityStore store;

    const cg::world::EntityId id = store.Create();
    store.Clear();

    failures += Expect(store.ActiveCount() == 0U, "Clear 后 active 为 0");
    failures += Expect(store.Find(id) == nullptr, "Clear 后旧 id 不可访问");
    failures += Expect(store.Create() != id, "Clear 后重新创建不会复活旧 id");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestDefaultEntityIdIsInvalid();
    failures += TestCreateAndFind();
    failures += TestDestroyInvalidatesOldId();
    failures += TestSlotReuseChangesGeneration();
    failures += TestCreateManyEntities();
    failures += TestClear();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
