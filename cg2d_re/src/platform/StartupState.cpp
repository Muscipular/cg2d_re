#include "cg/platform/Startup.hpp"

namespace cg::platform {

char* gCommandLine = nullptr; // 原全局：0x009A6EC4。
char gCommandLineStorage[4096] {}; // 原全局：0x009A6F48。
char gDataPath[128] = "data"; // 原全局：0x0055CF84。
std::int32_t gMonitorKeyboard = 0; // 原全局：0x009A6EC8。
std::uint8_t gRequestedRenderMode = 0; // 原全局：0x00628FE0。
std::uint8_t gValueFF4E28 = 0; // 原全局：0x00FF4E28。

std::uintptr_t gInstance = 0; // 原全局：0x009A6F38。
std::uintptr_t gCursorHandles[11] {}; // 原全局：0x009A6EF8。
std::int32_t gShowCommand = 0; // 原全局：0x009A6F28。
wchar_t gWindowClassName[5] = L"魔力宝贝"; // 原全局：0x00559384。
std::int32_t gWindowMode = 0; // 原全局：0x009A7F48。
std::int32_t gWindowWidth = 640; // 原全局：0x0055CE74。
std::int32_t gWindowHeight = 480; // 原全局：0x0055CE78。
std::int32_t gWindowActiveState = 2; // 原全局：0x0055CE70。
std::int32_t gStartX = 0; // 原全局：0x009A7F50。
std::int32_t gStartY = 0; // 原全局：0x009A7F54。
std::uint32_t gDesktopColor = 0; // 原全局：0x009A6F40。
std::uint32_t gValueD67AE4 = 0; // 原全局：0x00D67AE4。
std::int32_t gValue73ECBC = 0; // 原全局：0x0073ECBC。
std::uint32_t gValueD67AE8 = 0; // 原全局：0x00D67AE8。
std::int16_t gValue5F974C = 0; // 原全局：0x005F974C。
std::uint32_t gContactSortMode_D67CD0 = 0; // 原全局：0x00D67CD0。
std::int32_t gValue553438 = 10; // 原全局：0x00553438。
std::int32_t gValue55463C = 8; // 原全局：0x0055463C。
std::int32_t gValue554640 = 5; // 原全局：0x00554640。
std::int32_t gValue554644 = 1; // 原全局：0x00554644。
std::uint32_t gValueD68858 = 0; // 原全局：0x00D68858。
std::uint32_t gRequestedCursorState_D68DC4 = 0; // 原全局：0x00D68DC4。
std::uint32_t gConsumedCursorState_9A6F44 = 0; // 原全局：0x009A6F44。
std::int32_t gVersionMajor = 0; // 原全局：0x009A7F58。
std::int32_t gVersionMinor = 0; // 原全局：0x009A7F5C。
std::int32_t gVersionPatch = 0; // 原全局：0x009A7F60。

} // namespace cg::platform
