#pragma once

#include <cstddef>
#include <memory>
#include <utility>

namespace cg::session {

class ClientSession;

// 实际分配块的非拥有会话归属；成员构造/析构期间范围仍有效。
void BindSessionStorage(ClientSession& session, const void* storage, std::size_t extent);
void UnbindSessionStorage(const void* storage) noexcept;
// 独立成员析构只撤销恰好匹配的自身范围，不移除包含它的聚合登记。
void UnbindSessionObjectStorage(const void* storage, std::size_t extent) noexcept;
[[nodiscard]] ClientSession* FindSessionStorageOwner(const void* member) noexcept;
[[nodiscard]] ClientSession& SessionStorageOwner(const void* member);

template <typename T>
struct SessionStorageDeleter {
    void operator()(T* storage) const noexcept
    {
        std::destroy_at(storage);
        UnbindSessionStorage(storage);
        std::allocator<T>{}.deallocate(storage, 1);
    }
};

template <typename T>
using SessionStoragePtr = std::unique_ptr<T, SessionStorageDeleter<T>>;

// 先登记分配块再构造成员，失败前缀由C++析构后才移除所属范围。
template <typename T, typename... Args>
[[nodiscard]] SessionStoragePtr<T> MakeSessionStorage(ClientSession& session, Args&&... args)
{
    T* const storage = std::allocator<T>{}.allocate(1);
    try {
        BindSessionStorage(session, storage, sizeof(T));
        try {
            std::construct_at(storage, std::forward<Args>(args)...);
        } catch (...) {
            UnbindSessionStorage(storage);
            throw;
        }
    } catch (...) {
        std::allocator<T>{}.deallocate(storage, 1);
        throw;
    }
    return SessionStoragePtr<T>(storage);
}

} // namespace cg::session
