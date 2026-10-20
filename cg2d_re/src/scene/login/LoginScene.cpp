#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include "version.h"
#include "cg/scene/login/LoginScene.hpp"
#include <string>
#include "cg/text/Encoding.hpp"

#include "LoginSceneDependencies.hpp"

#include "cg/scene/login/LoginInput.hpp"
#include "cg/scene/login/LoginMessage.hpp"
#include "cg/scene/login/LoginState.hpp"
#include "cg/render/Palette.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/LegacyModuleDigest.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/platformTool.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/world/SceneState.hpp"

#include <Windows.h>
#include <shellapi.h>

#include <array>
#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib> // 原CRT接口：0x0053C649；原参数／返回合同由直接调用承接。
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <format>
#include <cstring>

#include "cg/core/vm.h"

namespace cg::login {

// 原559384窗口标题正文8B；当前宽正文由SetWindowTextW消费。
static constexpr wchar_t kWindowName[] = L"魔力宝贝";
static constexpr wchar_t kVersion[] = CG_CLIENT_VERSION_W;

std::int32_t DrawClientVersion(const std::int16_t preferredX, const std::int16_t y, const std::int8_t color)
{
    const auto x = static_cast<std::int16_t>(std::clamp(
        620 - render::TextMeasure(kVersion, 0), 0, static_cast<int>(preferredX)));
    return render::DrawText3_456560(x, y, 0, color, kVersion, 0);
}
// 原 0x55C62C「無法連上伺服器。 」正文 17 字节；字符串字面量隐式追加 NUL。
static constexpr wchar_t kCannotConnect[] =
    L"无法连上伺服器。 ";
// 原 0x55C6E4「此帳號已在線上」正文 14 字节；字符串字面量隐式追加 NUL。
static constexpr wchar_t kAlreadyOnline[] =
    L"此帐号已在线上";
// 原 0x55C6A4「目前停權中」正文 10 字节；字符串字面量隐式追加 NUL。
static constexpr wchar_t kSuspended[] =
    L"目前停权中";
static constexpr wchar_t kMultiline[] = L"Multiline";
// 原 0x55C71C「--->請速回 SOFTSTAR 網站」正文 24 字节；字符串字面量隐式追加 NUL。
static constexpr wchar_t kAccountHelp4[] =
    L"";
// 原 0x55C700「--->註冊新帳號並移轉舊帳號」正文 26 字节；字符串字面量隐式追加 NUL。
static constexpr wchar_t kAccountHelp5[] =
    L"";

// 原559958起25个64B槽的正文；当前静态中文采用UTF-16，索引仍由CategoryTitle承接。
static constexpr std::array<const wchar_t*, 25> kCategoryTitles {
    L"卡莲", // 0x559958 卡蓮
    L"金牛座", // 0x559998 金牛座
    L"双子座", // 0x5599D8 雙子座
    L"香港", // 0x559A18 香港
    L"狮子座", // 0x559A58 獅子座
    L"处女座", // 0x559A98 處女座
    L"天秤座", // 0x559AD8 天秤座
    L"歌姬", // 0x559B18 歌姬
    L"樱之舞", // 0x559B58 櫻之舞
    L"摩羯座", // 0x559B98 摩羯座
    L"水瓶座", // 0x559BD8 水瓶座
    L"双鱼座", // 0x559C18 雙魚座
    L"香港", // 0x559C58 香港
    L"测试", // 0x559C98 測試
    L"九龙", // 0x559CD8 九龍
    L"卡莲", // 0x559D18 卡蓮
    L"金牛座", // 0x559D58 金牛座
    L"露比", // 0x559D98 露比
    L"香港", // 0x559DD8 香港
    L"狮子座", // 0x559E18 獅子座
    L"处女座", // 0x559E58 處女座
    L"露比", // 0x559E98 露比
    L"心美", // 0x559ED8 心美
    L"露比", // 0x559F18 露比
    L"心美", // 0x559F58 心美
};

// 原 25 项标题表之后，group 25..39 会按 64 字节步长继续读取相邻映像。
// 这些是各实际地址首个 NUL 前的精确 ASCII 字节，不是正式标题表成员。
static constexpr std::array<const wchar_t*, 15> kAdjacentCategoryTitlePrefixes {
    L"I", L"", L"Z", L"\x07V",
    L"d", L"d", L"d", L"d", L"d",
    L"x", L"x", L"x", L"x", L"x", L"x",
};

static constexpr std::array<const wchar_t*, 20> kServerLabels {
    L"I", L"II", L"III", L"IV", L"V", L"VI", L"VII", L"VIII", L"IX", L"X",
    L"11", L"12", L"13", L"14", L"15", L"16", L"17", L"18", L"19", L"20",
};

// 与角色选择及登录输入按钮共用同一七W32布局。
using LoginUiRecord = cg::ui::UiControlRecord;

static LoginUiRecord gSwitchSceneUi {
    {540, 360, 64, 16}, 245045, 245047, 245046,
};
static LoginUiRecord gCloseUi {
    {540, 392, 64, 16}, 245153, 245155, 245154,
};
static LoginUiRecord gPreviousPageUi {
    {130, 310, 16, 16}, 243030, 243032, 243031,
};
static LoginUiRecord gNextPageUi {
    {254, 310, 16, 16}, 243033, 243035, 243034,
};

static std::array<LoginUiRecord, 20> gServerUi {{
    {{109, 108, 98, 34}, 30151, 30161, 30161},
    {{109, 140, 98, 34}, 30152, 30162, 30162},
    {{109, 172, 98, 34}, 30153, 30163, 30163},
    {{109, 204, 98, 34}, 30154, 30164, 30164},
    {{109, 236, 98, 34}, 30155, 30165, 30165},
    {{209, 108, 98, 34}, 30156, 30166, 30166},
    {{209, 140, 98, 34}, 30157, 30167, 30167},
    {{209, 172, 98, 34}, 30158, 30168, 30168},
    {{209, 204, 98, 34}, 30159, 30169, 30169},
    {{209, 236, 98, 34}, 30160, 30170, 30170},
    {{109, 108, 98, 34}, 30151, 30161, 30161},
    {{109, 140, 98, 34}, 30152, 30162, 30162},
    {{109, 172, 98, 34}, 30153, 30163, 30163},
    {{109, 204, 98, 34}, 30154, 30164, 30164},
    {{109, 236, 98, 34}, 30155, 30165, 30165},
    {{209, 108, 98, 34}, 30156, 30166, 30166},
    {{209, 140, 98, 34}, 30157, 30167, 30167},
    {{209, 172, 98, 34}, 30158, 30168, 30168},
    {{209, 204, 98, 34}, 30159, 30169, 30169},
    {{209, 236, 98, 34}, 30160, 30170, 30170},
}};

// 原 [0x55C038,0x55C6C8) 是一次 60 * 0x1C 的扫描视图，不是同质记录数组。
// 前 25 项后，扫描会穿过 4 字节填充、0x55C2F8 控件及相邻表/旧编码文本。
// 因而保留整段单一只读原始映像，仅在扫描边界按原偏移转为值记录。
static const std::array<std::uint32_t, 420> gCategoryScanImage {
    0x000000D1u, 0x0000006Cu, 0x00000062u, 0x00000022u, 0x00018206u, 0x00018207u, 0x00018207u,
    0x000000D1u, 0x000000ECu, 0x00000062u, 0x00000022u, 0x000075C7u, 0x000075C8u, 0x000075C8u,
    0x000000D1u, 0x000000ACu, 0x00000062u, 0x00000022u, 0x000075F9u, 0x000075FAu, 0x000075FAu,
    0x000000D1u, 0x000000CCu, 0x00000062u, 0x00000022u, 0x000077EDu, 0x000077EEu, 0x000077EEu,
    0x000000D1u, 0x0000008Cu, 0x00000062u, 0x00000022u, 0x0000765Du, 0x0000765Eu, 0x0000765Eu,
    0x000000D1u, 0x0000010Cu, 0x00000062u, 0x00000022u, 0x0000768Fu, 0x00007690u, 0x00007690u,
    0x0000006Du, 0x000000CCu, 0x00000062u, 0x00000022u, 0x000076C1u, 0x000076C2u, 0x000076C2u,
    0x0000006Du, 0x000000ACu, 0x00000062u, 0x00000022u, 0x000181F0u, 0x000181F1u, 0x000181F1u,
    0x0000006Du, 0x0000006Cu, 0x00000062u, 0x00000022u, 0x0001851Fu, 0x00018520u, 0x00018520u,
    0x0000006Du, 0x0000012Cu, 0x00000062u, 0x00000022u, 0x00007757u, 0x00007758u, 0x00007758u,
    0x0000006Du, 0x0000010Cu, 0x00000062u, 0x00000022u, 0x00007789u, 0x0000778Au, 0x0000778Au,
    0x0000006Du, 0x0000012Cu, 0x00000062u, 0x00000022u, 0x000077BBu, 0x000077BCu, 0x000077BCu,
    0x000000E0u, 0x00000152u, 0x00000062u, 0x00000022u, 0x000077EDu, 0x000077EEu, 0x000077EEu,
    0x000000D1u, 0x0000010Cu, 0x0000005Cu, 0x00000022u, 0x0000781Fu, 0x00007820u, 0x00007820u,
    0x0000007Cu, 0x00000152u, 0x00000062u, 0x00000022u, 0x00007851u, 0x00007852u, 0x00007852u,
    0x0000006Du, 0x0000006Cu, 0x00000062u, 0x00000022u, 0x00018206u, 0x00018207u, 0x00018207u,
    0x0000006Du, 0x000000ACu, 0x00000062u, 0x00000022u, 0x000075C7u, 0x000075C8u, 0x000075C8u,
    0x0000006Du, 0x0000006Cu, 0x00000062u, 0x00000022u, 0x00007883u, 0x00007884u, 0x00007884u,
    0x0000006Du, 0x000000CCu, 0x00000062u, 0x00000022u, 0x000077EDu, 0x000077EEu, 0x000077EEu,
    0x0000006Du, 0x000000ECu, 0x00000062u, 0x00000022u, 0x0000765Du, 0x0000765Eu, 0x0000765Eu,
    0x0000006Du, 0x0000010Cu, 0x00000062u, 0x00000022u, 0x0000768Fu, 0x00007690u, 0x00007690u,
    0x0000006Du, 0x0000008Cu, 0x00000062u, 0x00000022u, 0x00007883u, 0x00007884u, 0x00007884u,
    0x0000006Du, 0x0000006Cu, 0x00000062u, 0x00000022u, 0x000078B5u, 0x000078B6u, 0x000078B6u,
    0x0000006Du, 0x000000CCu, 0x00000062u, 0x00000022u, 0x00007883u, 0x00007884u, 0x00007884u,
    0x0000006Du, 0x000000ECu, 0x00000062u, 0x00000022u, 0x000078B5u, 0x000078B6u, 0x000078B6u,
    0x00000000u, 0x000000A8u, 0x00000152u, 0x0000005Cu, 0x00000022u, 0x0003BD35u, 0x0003BD37u, 0x0003BD36u,
    0x00000000u, 0x00000216u, 0x000001B6u, 0x00000040u, 0x00000010u, 0x0003BD35u, 0x0003BD37u, 0x0003BD36u,
    0x00000000u, 0x00000128u, 0x000001B1u, 0x00000031u, 0x00000011u, 0x0003BB3Eu, 0x0003BB40u, 0x0003BB3Fu,
    0x0003A980u, 0x0003A9E4u, 0x0003AA48u, 0x0003AAACu, 0x0003AB10u, 0x0003AB74u, 0x0003ABD8u, 0x0003AC3Cu,
    0x0003ACA0u, 0x0003AD04u, 0x0003AD68u, 0x0003ADCCu, 0x0003AE30u, 0x0003AE94u, 0x0003AEF8u, 0x0003AF5Cu,
    0x0003AFC0u, 0x0003B024u, 0x0003B088u, 0x0003B0ECu, 0x0003B150u, 0x0003B1B4u, 0x0003B218u, 0x0003B27Cu,
    0x0003B2E0u, 0x0003B344u, 0x0003B3A8u, 0x0003B40Cu, 0x000320C8u, 0x0003212Cu, 0x00032190u, 0x000321F4u,
    0x00032258u, 0x000322BCu, 0x00041EB0u, 0x00032384u, 0x000323E8u, 0x0003244Cu, 0x00041F14u, 0x00032514u,
    0x00032578u, 0x000325DCu, 0x00031830u, 0x00031894u, 0x000318F8u, 0x0003195Cu, 0x000319C0u, 0x00031A24u,
    0x00031A88u, 0x00031AECu, 0x00031B50u, 0x00031BB4u, 0x00031C18u, 0x00031C7Cu, 0x00031CE0u, 0x00031D44u,
    0x00030D40u, 0x00030E08u, 0x00031060u, 0x00031128u, 0x000311F0u, 0x000312B8u, 0x00031380u, 0x00030ED0u,
    0x00030F98u, 0x00031448u, 0x00031510u, 0x000315D8u, 0x000316A0u, 0x00031768u, 0x00007530u, 0x000084D0u,
    0x00009470u, 0x0000B3B0u, 0x0000A410u, 0x0000C350u, 0x0000D2F0u, 0x00011170u, 0x00012110u, 0x000130B0u,
    0x00014050u, 0x00014FF0u, 0x00015F90u, 0x00016F30u, 0x0000814Cu, 0x0000814Cu, 0x0000814Cu, 0x0000814Cu,
    0x0000814Cu, 0x0000814Cu, 0x0000814Cu, 0x0000814Du, 0x0000814Du, 0x0000814Du, 0x0000814Du, 0x0000814Du,
    0x0000814Du, 0x0000814Du, 0x00000000u, 0x00000050u, 0x0000014Fu, 0x0000002Cu, 0x00000014u, 0x0000007Cu,
    0x0000014Fu, 0x0000002Cu, 0x00000014u, 0x000000A8u, 0x0000014Fu, 0x0000002Cu, 0x00000014u, 0x000000D4u,
    0x0000014Fu, 0x0000002Cu, 0x00000014u, 0x00000100u, 0x0000014Fu, 0x0000002Cu, 0x00000014u, 0x0000012Cu,
    0x0000014Fu, 0x0000002Cu, 0x00000014u, 0x00000158u, 0x0000014Fu, 0x0000002Cu, 0x00000014u, 0x00000184u,
    0x0000014Fu, 0x0000002Cu, 0x00000014u, 0x000001B0u, 0x0000014Fu, 0x0000002Cu, 0x00000014u, 0x000001DCu,
    0x0000014Fu, 0x0000002Cu, 0x00000014u, 0xFFFFFFFEu, 0xBFA5A3A4u, 0xBAAA54BDu, 0xB9B862B1u, 0x4BB14DA9u,
    0x43A158BDu, 0x00000000u, 0xE9BFD0BDu, 0xD7A24AA4u, 0x4DA9D2A2u, 0xCFA2DEA2u, 0xE1A2E1A2u, 0xDDA2E5A2u,
    0xD2A2E0A2u, 0x00000000u, 0x2E312E37u, 0x00003030u, 0x746C754Du, 0x6E696C69u, 0x654E5F65u, 0x00000077u,
    0x77A954BDu, 0x00000000u, 0x746C754Du, 0x6E696C69u, 0x00000065u, 0x4AA46EB5u, 0xD2C37BBBu, 0xD1B1A2A5u,
    0x000043A1u, 0xBBA5A9AAu, 0x7EBBF9BFu, 0xD0BD41A1u, 0x73B7ABADu, 0xCAB0D2B1u, 0x4FA45DC5u, 0xA9A85FC4u,
    0x000043A1u, 0x6BAA4CB5u, 0x57A473B3u, 0x73B7F3A7u, 0x41AAF8A6u, 0x0000B9BEu, 0x6BAA4CB5u, 0x57A473B3u,
    0x41AAF8A6u, 0x43A1B9BEu, 0x00000020u, 0xF8A6B9A6u, 0xB9BE41AAu, 0xA3B4A3A4u, 0xD5B8D1A8u, 0x41AAB1AAu,
    0x0000C8B0u, 0xF0A5D0BDu, 0x40A4A7AEu, 0xC6B755A4u, 0x41A1ABB9u, 0xEDACC6BCu, 0x41A6E1ABu, 0xD5B8C1B9u,
    0x000043A1u, 0x41AAF8A6u, 0x74ADB9BEu, 0x4CB9FCB2u, 0x41A1ABADu, 0x79B5D0BDu, 0x41A6D4ADu, 0x43A1D5B8u,
    0x00000000u, 0xBEAABCA5u, 0xF9BFBAAAu, 0x00007EBBu, 0x65ABD8A5u, 0x76C5B1B0u, 0x0000A4A4u, 0x4FB670ADu,
    0xA1A6E8A4u, 0xDDB0B3A6u, 0x000044C3u, 0xC8AD78C0u, 0xA1B6C9AEu,
};

static_assert(sizeof(gCategoryScanImage) == 0x690);

static std::int32_t gSelectedServerGroup = 0; // 原 0x9A5B04，映像零填充。
static std::int32_t gServerPage = 0;          // 原 0x9A6EA4，映像零填充。
static std::wstring gLoginSceneMessage {};       // 原 0x9A60F0，映像零填充。
static std::int32_t gOpenUrlAfterMessage = 0; // 原 0x9A6EA8，映像零填充。

// 该映像只读且不全是控件；转换不外借异型指针或引入持久副本。
static cg::ui::UiControlRecord CategoryScanRecord(std::int32_t index)
{
    cg::ui::UiControlRecord controlRecord;
    std::memcpy(&controlRecord,
        gCategoryScanImage.data() + static_cast<std::size_t>(index) * 7,
        sizeof(controlRecord));
    return controlRecord;
}

static cg::ui::UiControlRecord SelectedCategoryControl()
{
    cg::ui::UiControlRecord controlRecord;
    std::memcpy(&controlRecord,
        gCategoryScanImage.data() + 0x2C0 / sizeof(std::uint32_t),
        sizeof(controlRecord));
    return controlRecord;
}

static void CopyText(std::wstring& destination, std::wstring_view source)
{
    destination = source;
}

static void IncrementSceneState()
{
    ++cg::world::gCurrentSceneState;
}

static void SetBaseWindowTitle(std::wstring& buffer)
{
    buffer = std::format(L"{} {}", kWindowName, kVersion);
    ::SetWindowTextW(reinterpret_cast<HWND>(cg::platform::gLegacyWindow), buffer.c_str());
}

static const wchar_t* CategoryTitle(std::int32_t categoryIndex)
{
    if (categoryIndex < static_cast<std::int32_t>(kCategoryTitles.size())) {
        return kCategoryTitles[static_cast<std::size_t>(categoryIndex)];
    }
    return kAdjacentCategoryTitlePrefixes[
        static_cast<std::size_t>(categoryIndex)
        - kCategoryTitles.size()];
}

static void SetServerWindowTitle(
    std::wstring& buffer,
    std::int32_t serverIndex,
    std::int32_t page,
    std::int32_t slot)
{
    const wchar_t* const category = CategoryTitle(serverIndex / 20);
    const wchar_t* const server =
        kServerLabels[static_cast<std::size_t>(page * 10 + slot)];
    buffer = std::format(
        L"{} {} [{} {}]",
        kWindowName,
        kVersion,
        category,
        server);
    ::SetWindowTextW(reinterpret_cast<HWND>(cg::platform::gLegacyWindow), buffer.c_str());
}

static void CopyAccountHelpLines()
{
    CopyText(cg::login::gLoginMsgLines_9A620C[3], L"您可能遇到以下情形之一：" /* 原 0x55C798「您可能遇到以下 3 種情形之一：」正文 29 字节；字符串字面量隐式追加 NUL。 */);
    CopyText(cg::login::gLoginMsgLines_9A620C[4], L"1.密码错误，请查明后重新输入" /* 原 0x55C778「1.密碼錯誤，請查明後重新輸入」正文 28 字节；字符串字面量隐式追加 NUL。 */);
    CopyText(cg::login::gLoginMsgLines_9A620C[5], L"2.角色已经在线或者卡号" /* 原 0x55C754「2.未儲值，請至 SOFTSTAR 網站儲值」正文 32 字节；字符串字面量隐式追加 NUL。 */);
    // CopyText(cg::login::gLoginMsgLines_9A620C[6], L"3.卡号" /* 原 0x55C738「3.新會員系統中查無此帳號」正文 24 字节；字符串字面量隐式追加 NUL。 */);
}

static void DrawCategoryControls()
{
    for (std::int32_t index = 0; index < 60; ++index) {
        if (cg::login::gLoginServerGroupCounts_D9F2D4[static_cast<std::size_t>(index)] > 0) {
            if (cg::login::gSelectedLoginServerGroup_55A054 == index) {
                const auto controlRecord = SelectedCategoryControl();
                cg::ui::DrawUiControl_45EFD0(
                    &controlRecord, 10, 0, 0, 0);
            } else if (cg::login::gSelectedLoginServerGroup_55A054 < 0) {
                const auto controlRecord = CategoryScanRecord(index);
                cg::ui::DrawUiControl_45EFD0(
                    &controlRecord, 10, 0, 0, 0);
            }
        }
    }
}

static bool ReadServerAvailability(std::int32_t category, std::int32_t page, std::int32_t slot)
{
    // LNW01：原0x00460B1D／0x00460BD7的负分类旁读退休；未选分类没有服务器项。
    // 原0x00D68B87／0x00D68C48是固定GBK区字节，不能索引动态UTF-16正文。
    if (category < 0) {
        return false;
    }
    const std::int32_t index = category * 20 + page * 10 + slot;
    return cg::login::gLoginServerRecords_D69798[static_cast<std::size_t>(index)].available != 0;
}

std::array<LoginServerRecord, 800> gLoginServerRecords_D69798 {}; // 原全局：0x00D69798。
std::array<std::int32_t, 60> gLoginServerGroupCounts_D9F2D4 {}; // 原全局：0x00D9F2D4。
std::int16_t gSelectedServerIndex = -1; // 原全局：0x0058AD94。
std::wstring gPassword {}; // 原全局：0x00DA38DC。
std::wstring gExecutableDigest {}; // 原全局：0x00DA3A1C。
std::wstring gAccount {}; // 原全局：0x00DA3A5C。
std::int16_t gLoginReplyErrorCode_DA3B24 = 0; // 原全局：0x00DA3B24。

static_assert(sizeof(gLoginServerGroupCounts_D9F2D4) == 60 * sizeof(std::int32_t));

namespace detail {

// NLD01：负索引／>=800返回-1；复制地址／atoi低W16端口，不读available；成功后应用A10策略。
// 原函数：0x00507D90。
std::int32_t GetServerAddressInfo_507D90(
    std::int32_t serverIndex,
    std::wstring& serverAddress,
    std::uint16_t* port)
{
    if (serverIndex < 0
        || serverIndex >= static_cast<std::int32_t>(gLoginServerRecords_D69798.size())) {
        return -1;
    }

    VM_START
    const auto& record = gLoginServerRecords_D69798[static_cast<std::size_t>(serverIndex)];
    serverAddress = record.address;
    *port = static_cast<std::uint16_t>(std::wcstol(record.portText.c_str(), nullptr, 10));
    // NTM01：映射/策略DNS由BeginServerConnection接续，查询本身不再阻塞主线程。
    VM_END
    return 0;
}

} // namespace detail

// 原函数：0x0045FEA0。
std::int32_t ProcessScene_45FEA0()
{
    std::int32_t interactiveTail = 0;
    std::wstring title;

    if (world::gCurrentSceneState == 0) {
        world::gCurrentSceneState = 1;
        SetBaseWindowTitle(title);
        world::gNetworkDisconnectState_D9F2CA = 0;
        world::gSceneTransitionResetPending_58B6D0 = 1;
        gSelectedServerGroup = 0;
        static_cast<void>(render::SelectPalette(27, 0));
        cg::audio::LoadAndPlayBGM(17);
    }

    if (world::gCurrentSceneState == 1) {
        interactiveTail = 1;
        if ((cg::ui::QueryUiControlInputState_45F100(&gCloseUi) & 1) != 0) {
            detail::PlaySe(54, 320, 240);
            ::PostMessageW(
                reinterpret_cast<HWND>(platform::gLegacyWindow), 0x10, 0, 0);
        } else if ((cg::ui::QueryUiControlInputState_45F100(&gSwitchSceneUi) & 1) != 0) {
            world::SetPendingSceneType(1);
            static_cast<void>(render::SelectPalette(20, 1));
            detail::PlaySe(54, 320, 240);
            cg::audio::BeginBgmFadeOut_52B990();
        } else if (const auto selectedControl = SelectedCategoryControl();
            (cg::ui::QueryUiControlInputState_45F100(&selectedControl) & 1) != 0) {
            gSelectedLoginServerGroup_55A054 = -1;
            gServerPage = 0;
            detail::PlaySe(54, 320, 240);
        } else {
            std::int32_t category;
            for (std::int32_t index = 0; index < 60; ++index) {
                const auto categoryControl = CategoryScanRecord(index);
                const std::int32_t input = cg::ui::QueryUiControlInputState_45F100(&categoryControl);
                category = gSelectedLoginServerGroup_55A054;
                if ((input & 1) != 0
                    && gLoginServerGroupCounts_D9F2D4[static_cast<std::size_t>(index)] > 0
                    && category < 0) {
                    gSelectedLoginServerGroup_55A054 = index;
                    detail::PlaySe(51, 320, 240);

                    const std::int32_t sourceA = categoryControl.activeImageNo;
                    const std::int32_t sourceB = categoryControl.normalImageNo;
                    const std::int32_t imageDelta = sourceB - sourceA;
                    std::int32_t imageNo = sourceA + 2;
                    for (std::int32_t server = 0; server < 20; ++server) {
                        gServerUi[static_cast<std::size_t>(server)].normalImageNo =
                            imageDelta + imageNo;
                        gServerUi[static_cast<std::size_t>(server)].activeImageNo =
                            imageNo;
                        imageNo += 2;
                    }
                    category = gSelectedLoginServerGroup_55A054;
                    break;
                }
            }
            for (std::int32_t slot = 0; slot < 10; ++slot) {
                if (category >= 0) {
                    const std::uint8_t input = static_cast<std::uint8_t>(
                        cg::ui::QueryUiControlInputState_45F100(&gServerUi[static_cast<std::size_t>(slot)]));
                    if ((input & 0x40u) != 0) {
                        gSelectedServerGroup = slot;
                    }
                    if (static_cast<std::int8_t>(input) < 0) {
                        const std::int32_t page = gServerPage;
                        const std::int32_t serverIndex =
                            category * 20 + page * 10 + slot;
                        if (gLoginServerRecords_D69798[static_cast<std::size_t>(serverIndex)].available != 0) {
                            SetServerWindowTitle(title, serverIndex, page, slot);
                            gSelectedServerIndex = static_cast<std::int16_t>(serverIndex);
                            gSelectedServerGroup = slot;
                            detail::PlaySe(53, 320, 240);
                            platform::ComputeExecutableDigest_5202E0(gExecutableDigest);
                            ReadInputValueFF494F_5203E0(gPassword);
                            ReadInputValueFF4982_520440(gAccount);
                            IncrementSceneState();
                            break;
                        }
                    }
                }
            }
        }
    }

    if (world::gCurrentSceneState == 2) {
        ResetValues_45D980();
        IncrementSceneState();
    }

    if (world::gCurrentSceneState == 3) {
        const std::int32_t result = UpdateServerConnection();
        switch (result) {
        case 1:
            world::SetCurrentSceneType(3);
            break;
        case -1:
            world::gCurrentSceneState = 100;
            CopyText(gLoginSceneMessage, L"NETWORK还没准备好。" /* 原 0x55C7C4「NETWORK還沒準備好。」正文 19 字节；字符串字面量隐式追加 NUL。 */);
            cg::network::ShutdownNetworkRuntime_5081A0();
            break;
        case -2: {
            world::gCurrentSceneState = 100;
            const std::int16_t error = gLoginReplyErrorCode_DA3B24;
            if (error > 1234) {
                CopyText(gLoginSceneMessage, kCannotConnect);
                gLoginReplyErrorCode_DA3B24 = 0;
            } else if (error == 1234) {
                gLoginReplyErrorCode_DA3B24 = 0;
                CopyText(gLoginSceneMessage, L"此伺服器很拥挤。" /* 原 0x55CAD0「此伺服器很擁擠。」正文 16 字节；字符串字面量隐式追加 NUL。 */);
            } else {
                switch (error) {
                case 1:
                    CopyText(gLoginSceneMessage, L"版本错误" /* 原 0x55C7B8「版本錯誤」正文 8 字节；字符串字面量隐式追加 NUL。 */);
                    gLoginReplyErrorCode_DA3B24 = 0;
                    break;
                case 2:
                case 5:
                    CopyText(gLoginSceneMessage, kMultiline);
                    CopyAccountHelpLines();
                    // CopyText(gLoginMsgLines_9A620C[10], kAccountHelp4);
                    gLoginReplyErrorCode_DA3B24 = 0;
                    // CopyText(gLoginMsgLines_9A620C[11], kAccountHelp5);
                    break;
                case 3:
                    CopyText(gLoginSceneMessage, kMultiline);
                    CopyText(gLoginSceneMessage, L"Multiline_New");
                    CopyText(gLoginMsgLines_9A620C[0], L"        密码错误请重试        " /* 原 0x55CCEC「＝＝＝＝ 為了維護玩家帳號安全性 ＝＝＝＝」正文 40 字节；字符串字面量隐式追加 NUL。 */);
                    CopyText(gLoginMsgLines_9A620C[1], L"" /* 原 0x55CCC0「大宇資訊已於２００５／０５／１３全面變更」正文 40 字节；字符串字面量隐式追加 NUL。 */);
                    // CopyText(gValue9A620C[2], L"" /* 原 0x55CC94「所有玩家會員密碼、以及魔力寶貝遊戲密碼，」正文 40 字节；字符串字面量隐式追加 NUL。 */);
                    // CopyText(gValue9A620C[3], L"" /* 原 0x55CC6C「請玩家至SOFTSTAR會員中心領取新的密碼，」正文 38 字节；字符串字面量隐式追加 NUL。 */);
                    // CopyText(gValue9A620C[4], L"" /* 原 0x55CC48「詳情請參閱SOFTSTAR官方網站資訊：」正文 32 字节；字符串字面量隐式追加 NUL。 */);
                    // CopyText(gValue9A620C[5], L"" /* 原 0x55CC24「　　　　http://www.softstar.com.tw」正文 34 字节；字符串字面量隐式追加 NUL。 */);
                    CopyAccountHelpLines();
                    gLoginReplyErrorCode_DA3B24 = 0;
                    // CopyText(gLoginMsgLines_9A620C[2], kAccountHelp4);
                    // CopyText(gLoginMsgLines_9A620C[3], kAccountHelp5);
                    break;
                case 4:
                    CopyText(gLoginSceneMessage, L"点数不足" /* 原 0x55C6F4「點數不足」正文 8 字节；字符串字面量隐式追加 NUL。 */);
                    gLoginReplyErrorCode_DA3B24 = 0;
                    break;
                case 6:
                    CopyText(gLoginSceneMessage, kAlreadyOnline);
                    gLoginReplyErrorCode_DA3B24 = 0;
                    break;
                case 7:
                    gLoginReplyErrorCode_DA3B24 = 0;
                    CopyText(gLoginSceneMessage, kAlreadyOnline);
                    break;
                case 8:
                    CopyText(gLoginSceneMessage, L"登入伺服器发生错误" /* 原 0x55C6D0「登入伺服器發生錯誤」正文 18 字节；字符串字面量隐式追加 NUL。 */);
                    gLoginReplyErrorCode_DA3B24 = 0;
                    break;
                case 9:
                    gLoginReplyErrorCode_DA3B24 = 0;
                    CopyText(gLoginSceneMessage, L"储值时间已到" /* 原 0x55C6C0「儲值時間已到」正文 12 字节；字符串字面量隐式追加 NUL。 */);
                    break;
                case 10:
                    CopyText(gLoginSceneMessage, L"计费方式有问题" /* 原 0x55C6B0「計費方式有問題」正文 14 字节；字符串字面量隐式追加 NUL。 */);
                    gLoginReplyErrorCode_DA3B24 = 0;
                    break;
                case 11:
                case 13:
                    gLoginReplyErrorCode_DA3B24 = 0;
                    CopyText(gLoginSceneMessage, kSuspended);
                    break;
                case 12:
                case 14:
                    CopyText(gLoginSceneMessage, kSuspended);
                    gLoginReplyErrorCode_DA3B24 = 0;
                    break;
                case 15:
                    gLoginReplyErrorCode_DA3B24 = 0;
                    CopyText(gLoginSceneMessage, L"未知的错误" /* 原 0x55C698「未知的錯誤」正文 10 字节；字符串字面量隐式追加 NUL。 */);
                    break;
                case 16:
                    CopyText(gLoginSceneMessage, L"伺服器负荷过重，请稍候再试。" /* 原 0x55C678「伺服器負荷過重，請稍候再試。」正文 28 字节；字符串字面量隐式追加 NUL。 */);
                    gLoginReplyErrorCode_DA3B24 = 0;
                    break;
                case 18:
                    gLoginReplyErrorCode_DA3B24 = 0;
                    CopyText(gLoginSceneMessage, L"请休息一下滑鼠，数秒后再尝试。" /* 原 0x55C658「請休息一下滑鼠，數秒後再嘗試。」正文 30 字节；字符串字面量隐式追加 NUL。 */);
                    break;
                case 19:
                    CopyText(gLoginSceneMessage, L"此伺服器不提供试玩服务" /* 原 0x55C640「此伺服器不提供試玩服務」正文 22 字节；字符串字面量隐式追加 NUL。 */);
                    gLoginReplyErrorCode_DA3B24 = 0;
                    break;
                case 20:
                    gLoginReplyErrorCode_DA3B24 = 0;
                    CopyText(gLoginSceneMessage, L"此电脑没有权限登入。（行动锁上锁）" /* 原 0x55CC00「此電腦沒有權限登入。（行動鎖上鎖）」正文 34 字节；字符串字面量隐式追加 NUL。 */);
                    break;
                case 21:
                    CopyText(gLoginSceneMessage, kMultiline);
                    CopyText(gLoginMsgLines_9A620C[6], L"本台机器之机码已停用。" /* 原 0x55CBE8「本台機器之機碼已停用。」正文 22 字节；字符串字面量隐式追加 NUL。 */);
                    CopyText(gLoginMsgLines_9A620C[7], L"请检查游戏点数。" /* 原 0x55CBD4「請檢查遊戲點數。」正文 16 字节；字符串字面量隐式追加 NUL。 */);
                    CopyText(gLoginMsgLines_9A620C[8], L"可能是点数不足导致登录之机码停用。" /* 原 0x55CBB0「可能是點數不足導致登錄之機碼停用。」正文 34 字节；字符串字面量隐式追加 NUL。 */);
                    CopyText(gLoginMsgLines_9A620C[9], L"储值后请稍待数分钟系统处理时间。" /* 原 0x55CB8C「儲值後請稍待數分鐘系統處理時間。」正文 32 字节；字符串字面量隐式追加 NUL。 */);
                    // CopyText(gLoginMsgLines_9A620C[10], L" ");
                    // CopyText(gLoginMsgLines_9A620C[11], L" ");
                    gLoginReplyErrorCode_DA3B24 = 0;
                    break;
                case 22:
                    gLoginReplyErrorCode_DA3B24 = 0;
                    CopyText(gLoginSceneMessage, L"此帐号已使用行动锁冻结。" /* 原 0x55CB70「此帳號已使用行動鎖凍結。」正文 24 字节；字符串字面量隐式追加 NUL。 */);
                    break;
                case 23:
                    gOpenUrlAfterMessage = 1;
                    gLoginReplyErrorCode_DA3B24 = 0;
                    CopyText(gLoginSceneMessage, L"游戏密码已经过期。\n" /* 原55CAE4密码过期说明：139B正文含五个LF，字面量仅追加末尾NUL。 */);
                    break;
                default:
                    CopyText(gLoginSceneMessage, kCannotConnect);
                    gLoginReplyErrorCode_DA3B24 = 0;
                    break;
                }
            }
            cg::network::ShutdownNetworkRuntime_5081A0();
            break;
        }
        case -3:
            world::gCurrentSceneState = 100;
            CopyText(gLoginSceneMessage, L"无法连上更新伺服器" /* 原 0x55C618「無法連上更新伺服器」正文 18 字节；字符串字面量隐式追加 NUL。 */);
            cg::network::ShutdownNetworkRuntime_5081A0();
            break;
        case -4:
            world::gCurrentSceneState = 100;
            CopyText(gLoginSceneMessage, L"版本错误，请重新启动魔力宝贝。" /* 原 0x55C5F8「版本錯誤，請重新啟動魔力寶貝。」正文 30 字节；字符串字面量隐式追加 NUL。 */);
            cg::network::ShutdownNetworkRuntime_5081A0();
            break;
        default:
            break;
        }
    }

    if (world::gCurrentSceneState == 100) {
        ResetLoginMessage();
        IncrementSceneState();
    }

    if (world::gCurrentSceneState == 101 && UpdateLoginMessage(gLoginSceneMessage) != 0) {
        if (gOpenUrlAfterMessage != 0) {
            // ::ShellExecuteA(
            //     reinterpret_cast<HWND>(platform::gLegacyWindow),
            //     nullptr,
            //     "http://www.softstar.com.tw/Game/Index.aspx",
            //     nullptr,
            //     nullptr,
            //     1);
            gOpenUrlAfterMessage = 0;
            world::SetPendingSceneType(1);
        }
        world::gCurrentSceneState = 1;
        SetBaseWindowTitle(title);
    }

    if (world::gCurrentSceneState == 102) {
        if (UpdateLoginMessage(gLoginSceneMessage) != 0) {
            world::gCurrentSceneState = 1;
        }
        SetBaseWindowTitle(title);
    }

    cg::ui::DrawUiControl_45EFD0(&gCloseUi, 10, 0, 0, 0);
    cg::ui::DrawUiControl_45EFD0(&gSwitchSceneUi, 10, 0, 0, 0);
    if (interactiveTail != 0) {
        DrawCategoryControls();
        if (gSelectedLoginServerGroup_55A054 >= 0
            && gLoginServerGroupCounts_D9F2D4[static_cast<std::size_t>(gSelectedLoginServerGroup_55A054)] > 10) {
            cg::ui::DrawUiControl_45EFD0(&gNextPageUi, 10, 0, 0, 0);
            cg::ui::DrawUiControl_45EFD0(&gPreviousPageUi, 10, 0, 0, 0);
            if ((cg::ui::QueryUiControlInputState_45F100(&gNextPageUi) & 1) != 0) {
                if (gServerPage < 1) {
                    ++gServerPage;
                }
                detail::PlaySe(51, 320, 240);
            }
            if ((cg::ui::QueryUiControlInputState_45F100(&gPreviousPageUi) & 1) != 0) {
                if (gServerPage > 0) {
                    --gServerPage;
                }
                detail::PlaySe(51, 320, 240);
            }
        }

        for (std::int32_t slot = 0; slot < 10; ++slot) {
            const bool available =
                ReadServerAvailability(gSelectedLoginServerGroup_55A054, gServerPage, slot);
            if (available && gSelectedLoginServerGroup_55A054 >= 0) {
                if (gServerPage == 0) {
                    cg::ui::DrawUiControl_45EFD0(
                        &gServerUi[static_cast<std::size_t>(slot)], 10, 0, 0, 0);
                } else if (gServerPage == 1) {
                    cg::ui::DrawUiControl_45EFD0(
                        &gServerUi[static_cast<std::size_t>(slot + 10)], 10, 0, 0, 0);
                }
            }
        }
    } else {
        DrawCategoryControls();
        for (std::int32_t slot = 0; slot < 10; ++slot) {
            if (ReadServerAvailability(gSelectedLoginServerGroup_55A054, gServerPage, slot)) {
                const std::int32_t style = slot == gSelectedServerGroup ? 2 : 1;
                if (gServerPage == 0) {
                    cg::ui::DrawUiControl_45EFD0(
                        &gServerUi[static_cast<std::size_t>(slot)], 10, 0, 0, style);
                } else if (gServerPage == 1) {
                    cg::ui::DrawUiControl_45EFD0(
                        &gServerUi[static_cast<std::size_t>(slot + 10)], 10, 0, 0, style);
                }
            }
        }
    }

    static_cast<void>(render::HasLegacy3DDevice_44B610());
    cg::render::QueueOriginalPaletteGraphic_45F090(49, 67, 3, 0x3BEA3, 0, -1);
    cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 3, 0x3BEA2, 0, -1), true);
    return DrawClientVersion(540, 424, 0);
}

} // namespace cg::login
