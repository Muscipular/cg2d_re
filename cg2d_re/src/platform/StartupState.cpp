#include "cg/platform/Startup.hpp"

namespace cg::platform {

char* gCommandLine = nullptr;
char gCommandLineStorage[4096] {};
char gDataPath[128] = "data";
std::int32_t gMonitorKeyboard = 0;
std::uint8_t gRequestedRenderMode = 0;
std::uint8_t gValueFF4E28 = 0;

std::uintptr_t gInstance = 0;
std::uintptr_t gStartupMutex = 0;
std::uintptr_t gCursorHandles[11] {};
std::int32_t gShowCommand = 0;
char gWindowClassName[9] = "\xC5\x5D\xA4\x4F\xC4\x5F\xA8\xA9";
std::int32_t gWindowMode = 0;
std::int32_t gWindowWidth = 640;
std::int32_t gWindowHeight = 480;
std::int32_t gWindowActiveState = 2;
std::int32_t gStartX = 0;
std::int32_t gStartY = 0;
std::uint32_t gDesktopColor = 0;
std::uint32_t gValueD67AE4 = 0;
std::uint32_t gValueD68DC4 = 0;
std::uint32_t gValue9A6F44 = 0;
std::int32_t gVersionMajor = 0;
std::int32_t gVersionMinor = 0;
std::int32_t gVersionPatch = 0;

} // namespace cg::platform
