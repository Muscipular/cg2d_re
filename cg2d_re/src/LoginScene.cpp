#include "cg/LoginScene.hpp"

#include "LoginSceneDependencies.hpp"

#include "cg/LoginMessage.hpp"
#include "cg/LoginState.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/world/SceneState.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace {

constexpr char kWindowName[] = "魔力寶貝";
constexpr char kVersion[] = "7.1.00";
constexpr char kPasswordExpiredUrl[] = "http://www.softstar.com.tw/Game/Index.aspx";

constexpr char kNetworkNotReady[] = "NETWORK還沒準備好。";
constexpr char kCannotConnect[] = "無法連上伺服器。 ";
constexpr char kServerCrowded[] = "此伺服器很擁擠。";
constexpr char kVersionError[] = "版本錯誤";
constexpr char kInsufficientPoints[] = "點數不足";
constexpr char kAlreadyOnline[] = "此帳號已在線上";
constexpr char kLoginServerError[] = "登入伺服器發生錯誤";
constexpr char kStoredTimeExpired[] = "儲值時間已到";
constexpr char kBillingError[] = "計費方式有問題";
constexpr char kSuspended[] = "目前停權中";
constexpr char kUnknownError[] = "未知的錯誤";
constexpr char kServerBusy[] = "伺服器負荷過重，請稍候再試。";
constexpr char kWaitBeforeRetry[] = "請休息一下滑鼠，數秒後再嘗試。";
constexpr char kNoTrialService[] = "此伺服器不提供試玩服務";
constexpr char kMachineLocked[] = "此電腦沒有權限登入。（行動鎖上鎖）";
constexpr char kAccountFrozen[] = "此帳號已使用行動鎖凍結。";
constexpr char kCannotConnectUpdate[] = "無法連上更新伺服器";
constexpr char kRestartForVersion[] = "版本錯誤，請重新啟動魔力寶貝。";
constexpr char kMultiline[] = "Multiline";
constexpr char kMultilineNew[] = "Multiline_New";

constexpr char kAccountHelp0[] = "您可能遇到以下 3 種情形之一：";
constexpr char kAccountHelp1[] = "1.密碼錯誤，請查明後重新輸入";
constexpr char kAccountHelp2[] = "2.未儲值，請至 SOFTSTAR 網站儲值";
constexpr char kAccountHelp3[] = "3.新會員系統中查無此帳號";
constexpr char kAccountHelp4[] = "--->請速回 SOFTSTAR 網站";
constexpr char kAccountHelp5[] = "--->註冊新帳號並移轉舊帳號";

constexpr char kPasswordChange0[] = "＝＝＝＝ 為了維護玩家帳號安全性 ＝＝＝＝";
constexpr char kPasswordChange1[] = "大宇資訊已於２００５／０５／１３全面變更";
constexpr char kPasswordChange2[] = "所有玩家會員密碼、以及魔力寶貝遊戲密碼，";
constexpr char kPasswordChange3[] = "請玩家至SOFTSTAR會員中心領取新的密碼，";
constexpr char kPasswordChange4[] = "詳情請參閱SOFTSTAR官方網站資訊：";
constexpr char kPasswordChange5[] = "　　　　http://www.softstar.com.tw";

constexpr char kMachineCode0[] = "本台機器之機碼已停用。";
constexpr char kMachineCode1[] = "請檢查遊戲點數。";
constexpr char kMachineCode2[] = "可能是點數不足導致登錄之機碼停用。";
constexpr char kMachineCode3[] = "儲值後請稍待數分鐘系統處理時間。";

constexpr char kPasswordExpiredMessage[] =
    "親愛的玩家您好：\n"
    "由於您的遊戲密碼已經過期，\n"
    "請登入大宇會員系統更換遊戲密碼，\n"
    "請定期更換密碼以確保帳號安全。\n"
    "點擊確定，將前往SOFTSTAR網頁。\n";

constexpr std::array<const char*, 25> kCategoryTitles {
    "卡蓮", "金牛座", "雙子座", "香港", "獅子座",
    "處女座", "天秤座", "歌姬", "櫻之舞", "摩羯座",
    "水瓶座", "雙魚座", "香港", "測試", "九龍",
    "卡蓮", "金牛座", "露比", "香港", "獅子座",
    "處女座", "露比", "心美", "露比", "心美",
};

// 原 25 项标题表之后，group 25..39 会按 64 字节步长继续读取相邻映像。
// 这些是各实际地址首个 NUL 前的精确 ASCII 字节，不是正式标题表成员。
constexpr std::array<const char*, 15> kAdjacentCategoryTitlePrefixes {
    "I", "", "Z", "\x07V",
    "d", "d", "d", "d", "d",
    "x", "x", "x", "x", "x", "x",
};

constexpr std::array<const char*, 20> kServerLabels {
    "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X",
    "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
};

struct LoginUiRecord {
    std::array<std::int32_t, 7> values;
};

static_assert(sizeof(LoginUiRecord) == 0x1C);

LoginUiRecord gSwitchSceneUi {{540, 360, 64, 16, 245045, 245047, 245046}};
LoginUiRecord gCloseUi {{540, 392, 64, 16, 245153, 245155, 245154}};
LoginUiRecord gPreviousPageUi {{130, 310, 16, 16, 243030, 243032, 243031}};
LoginUiRecord gNextPageUi {{254, 310, 16, 16, 243033, 243035, 243034}};

std::array<LoginUiRecord, 20> gServerUi {{
    {{{109, 108, 98, 34, 30151, 30161, 30161}}},
    {{{109, 140, 98, 34, 30152, 30162, 30162}}},
    {{{109, 172, 98, 34, 30153, 30163, 30163}}},
    {{{109, 204, 98, 34, 30154, 30164, 30164}}},
    {{{109, 236, 98, 34, 30155, 30165, 30165}}},
    {{{209, 108, 98, 34, 30156, 30166, 30166}}},
    {{{209, 140, 98, 34, 30157, 30167, 30167}}},
    {{{209, 172, 98, 34, 30158, 30168, 30168}}},
    {{{209, 204, 98, 34, 30159, 30169, 30169}}},
    {{{209, 236, 98, 34, 30160, 30170, 30170}}},
    {{{109, 108, 98, 34, 30151, 30161, 30161}}},
    {{{109, 140, 98, 34, 30152, 30162, 30162}}},
    {{{109, 172, 98, 34, 30153, 30163, 30163}}},
    {{{109, 204, 98, 34, 30154, 30164, 30164}}},
    {{{109, 236, 98, 34, 30155, 30165, 30165}}},
    {{{209, 108, 98, 34, 30156, 30166, 30166}}},
    {{{209, 140, 98, 34, 30157, 30167, 30167}}},
    {{{209, 172, 98, 34, 30158, 30168, 30168}}},
    {{{209, 204, 98, 34, 30159, 30169, 30169}}},
    {{{209, 236, 98, 34, 30160, 30170, 30170}}},
}};

// 原 [0x55C038,0x55C6C8) 是一次 60 * 0x1C 的扫描视图，不是同质记录数组。
// 前 25 项后，扫描会穿过 4 字节填充、0x55C2F8 控件及相邻表/旧编码文本。
// 因而保留整段单一原始映像，并只通过地址偏移传给原 UI 依赖。
std::array<std::uint32_t, 420> gCategoryScanImage {
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

std::int32_t gSelectedServerGroup = 0; // 原 0x9A5B04，映像零填充。
std::int32_t gServerPage = 0;          // 原 0x9A6EA4，映像零填充。
char gLoginSceneMessage[260] {};       // 原 0x9A60F0，映像零填充。
std::int32_t gOpenUrlAfterMessage = 0; // 原 0x9A6EA8，映像零填充。

static_assert(sizeof(gLoginSceneMessage) == 260);
static_assert(sizeof(kPasswordExpiredMessage) <= 256);

void* CategoryScanRecord(std::int32_t index)
{
    return gCategoryScanImage.data() + static_cast<std::size_t>(index) * 7;
}

void* SelectedCategoryControl()
{
    return gCategoryScanImage.data() + 0x2C0 / sizeof(std::uint32_t);
}

std::int32_t CategoryField(std::int32_t index, std::size_t field)
{
    return std::bit_cast<std::int32_t>(
        gCategoryScanImage[static_cast<std::size_t>(index) * 7 + field]);
}

template <std::size_t DestinationSize, std::size_t SourceSize>
void CopyText(char (&destination)[DestinationSize], const char (&source)[SourceSize])
{
    static_assert(SourceSize <= DestinationSize);
    std::memcpy(destination, source, SourceSize);
}

void IncrementSceneState()
{
    cg::world::gCurrentSceneState = std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(cg::world::gCurrentSceneState) + 1u);
}

void SetBaseWindowTitle(char (&buffer)[256])
{
    std::snprintf(buffer, sizeof(buffer), "%s %s", kWindowName, kVersion);
    cg::platform::detail::SetWindowTextA_54D2E0(cg::platform::gLegacyWindow, buffer);
}

const char* CategoryTitle(std::int32_t categoryIndex)
{
    if (categoryIndex < static_cast<std::int32_t>(kCategoryTitles.size())) {
        return kCategoryTitles[static_cast<std::size_t>(categoryIndex)];
    }
    return kAdjacentCategoryTitlePrefixes[
        static_cast<std::size_t>(categoryIndex)
        - kCategoryTitles.size()];
}

void SetServerWindowTitle(
    char (&buffer)[256],
    std::int32_t serverIndex,
    std::int32_t page,
    std::int32_t slot)
{
    const char* const category = CategoryTitle(serverIndex / 20);
    const char* const server =
        kServerLabels[static_cast<std::size_t>(page * 10 + slot)];
    std::snprintf(
        buffer,
        sizeof(buffer),
        "%s %s [%s %s]",
        kWindowName,
        kVersion,
        category,
        server);
    if (slot == 9) {
        std::snprintf(
            buffer,
            sizeof(buffer),
            "%s %s [逆 %s]",
            kWindowName,
            kVersion,
            category);
    }
    cg::platform::detail::SetWindowTextA_54D2E0(cg::platform::gLegacyWindow, buffer);
}

void CopyAccountHelpLines()
{
    CopyText(cg::login::gValue9A620C[6], kAccountHelp0);
    CopyText(cg::login::gValue9A620C[7], kAccountHelp1);
    CopyText(cg::login::gValue9A620C[8], kAccountHelp2);
    CopyText(cg::login::gValue9A620C[9], kAccountHelp3);
}

void DrawCategoryControls()
{
    for (std::int32_t index = 0; index < 60; ++index) {
        if (cg::login::gValueD9F2D4[static_cast<std::size_t>(index)] > 0) {
            if (cg::login::gValue55A054 == index) {
                cg::login::detail::Sub_45EFD0(
                    SelectedCategoryControl(), 10, 0, 0, 0);
            } else if (cg::login::gValue55A054 < 0) {
                cg::login::detail::Sub_45EFD0(
                    CategoryScanRecord(index), 10, 0, 0, 0);
            }
        }
    }
}

bool ReadServerAvailability(std::int32_t category, std::int32_t page, std::int32_t slot)
{
    if (category < 0) {
        return cg::login::detail::ReadNegativeSelectionAvailability(page, slot) != 0;
    }
    const std::int32_t index = category * 20 + page * 10 + slot;
    return cg::login::gValueD69798[static_cast<std::size_t>(index)].available != 0;
}

} // namespace

namespace cg::login {

std::array<LoginServerRecord, 800> gValueD69798 {};
std::array<std::int32_t, 60> gValueD9F2D4 {};
std::int16_t gValue58AD94 = -1;
char gValueDA38DC[64] {};
char gValueDA3A1C[64] {};
char gValueDA3A5C[64] {};
std::int16_t gValueDA3B24 = 0;

static_assert(sizeof(gValueD69798) == 800 * 0xC1);
static_assert(sizeof(gValueD9F2D4) == 60 * sizeof(std::int32_t));

std::int32_t ProcessScene_45FEA0()
{
    std::int32_t interactiveTail = 0;
    char title[256];

    if (world::gCurrentSceneState == 0) {
        world::gCurrentSceneState = 1;
        SetBaseWindowTitle(title);
        world::gValueD9F2CA = 0;
        world::gValue58B6D0 = 1;
        gSelectedServerGroup = 0;
        detail::Sub_450B70(27, 0);
        detail::LoadAndPlayBGM(17);
    }

    if (world::gCurrentSceneState == 1) {
        interactiveTail = 1;
        if ((detail::Sub_45F100(&gCloseUi) & 1) != 0) {
            detail::PlaySe(54, 320, 240);
            platform::detail::PostMessageA_54D2F0(
                platform::gLegacyWindow, 0x10, 0, 0);
        } else if ((detail::Sub_45F100(&gSwitchSceneUi) & 1) != 0) {
            world::SetPendingSceneType(1);
            detail::Sub_450B70(20, 1);
            detail::PlaySe(54, 320, 240);
            detail::Sub_52B990();
        } else if ((detail::Sub_45F100(SelectedCategoryControl()) & 1) != 0) {
            gValue55A054 = -1;
            gServerPage = 0;
            detail::PlaySe(54, 320, 240);
        } else {
            for (std::int32_t index = 0; index < 60; ++index) {
                const std::int32_t input = detail::Sub_45F100(CategoryScanRecord(index));
                if ((input & 1) != 0
                    && gValueD9F2D4[static_cast<std::size_t>(index)] > 0
                    && gValue55A054 < 0) {
                    gValue55A054 = index;
                    detail::PlaySe(51, 320, 240);

                    const std::int32_t sourceA = CategoryField(index, 5);
                    const std::int32_t sourceB = CategoryField(index, 4);
                    for (std::int32_t server = 0; server < 20; ++server) {
                        const std::uint32_t imageOffset =
                            static_cast<std::uint32_t>(server) * 2u;
                        gServerUi[static_cast<std::size_t>(server)].values[4] =
                            std::bit_cast<std::int32_t>(
                                std::bit_cast<std::uint32_t>(sourceB)
                                + 2u
                                + imageOffset);
                        gServerUi[static_cast<std::size_t>(server)].values[5] =
                            std::bit_cast<std::int32_t>(
                                std::bit_cast<std::uint32_t>(sourceA)
                                + 2u
                                + imageOffset);
                    }
                    break;
                }
            }

            const std::int32_t category = gValue55A054;
            for (std::int32_t slot = 0; slot < 10; ++slot) {
                if (category >= 0) {
                    const std::uint8_t input = static_cast<std::uint8_t>(
                        detail::Sub_45F100(&gServerUi[static_cast<std::size_t>(slot)]));
                    if ((input & 0x40u) != 0) {
                        gSelectedServerGroup = slot;
                    }
                    const std::int32_t serverIndex =
                        category * 20 + gServerPage * 10 + slot;
                    if (static_cast<std::int8_t>(input) < 0
                        && gValueD69798[static_cast<std::size_t>(serverIndex)].available != 0) {
                        SetServerWindowTitle(title, serverIndex, gServerPage, slot);
                        gValue58AD94 = static_cast<std::int16_t>(serverIndex);
                        gSelectedServerGroup = slot;
                        detail::PlaySe(53, 320, 240);
                        detail::Sub_5202E0(gValueDA3A1C);
                        detail::Sub_5203E0(gValueDA38DC);
                        detail::Sub_520440(gValueDA3A5C);
                        IncrementSceneState();
                        break;
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
            CopyText(gLoginSceneMessage, kNetworkNotReady);
            detail::Sub_5081A0();
            break;
        case -2: {
            world::gCurrentSceneState = 100;
            const std::int16_t error = gValueDA3B24;
            if (error > 1234) {
                CopyText(gLoginSceneMessage, kCannotConnect);
                gValueDA3B24 = 0;
            } else if (error == 1234) {
                gValueDA3B24 = 0;
                CopyText(gLoginSceneMessage, kServerCrowded);
            } else {
                switch (error) {
                case 1:
                    CopyText(gLoginSceneMessage, kVersionError);
                    gValueDA3B24 = 0;
                    break;
                case 2:
                case 5:
                    CopyText(gLoginSceneMessage, kMultiline);
                    CopyAccountHelpLines();
                    CopyText(gValue9A620C[10], kAccountHelp4);
                    gValueDA3B24 = 0;
                    CopyText(gValue9A620C[11], kAccountHelp5);
                    break;
                case 3:
                    CopyText(gLoginSceneMessage, kMultiline);
                    CopyText(gLoginSceneMessage, kMultilineNew);
                    CopyText(gValue9A620C[0], kPasswordChange0);
                    CopyText(gValue9A620C[1], kPasswordChange1);
                    CopyText(gValue9A620C[2], kPasswordChange2);
                    CopyText(gValue9A620C[3], kPasswordChange3);
                    CopyText(gValue9A620C[4], kPasswordChange4);
                    CopyText(gValue9A620C[5], kPasswordChange5);
                    CopyAccountHelpLines();
                    gValueDA3B24 = 0;
                    CopyText(gValue9A620C[10], kAccountHelp4);
                    CopyText(gValue9A620C[11], kAccountHelp5);
                    break;
                case 4:
                    CopyText(gLoginSceneMessage, kInsufficientPoints);
                    gValueDA3B24 = 0;
                    break;
                case 6:
                    CopyText(gLoginSceneMessage, kAlreadyOnline);
                    gValueDA3B24 = 0;
                    break;
                case 7:
                    gValueDA3B24 = 0;
                    CopyText(gLoginSceneMessage, kAlreadyOnline);
                    break;
                case 8:
                    CopyText(gLoginSceneMessage, kLoginServerError);
                    gValueDA3B24 = 0;
                    break;
                case 9:
                    gValueDA3B24 = 0;
                    CopyText(gLoginSceneMessage, kStoredTimeExpired);
                    break;
                case 10:
                    CopyText(gLoginSceneMessage, kBillingError);
                    gValueDA3B24 = 0;
                    break;
                case 11:
                case 13:
                    gValueDA3B24 = 0;
                    CopyText(gLoginSceneMessage, kSuspended);
                    break;
                case 12:
                case 14:
                    CopyText(gLoginSceneMessage, kSuspended);
                    gValueDA3B24 = 0;
                    break;
                case 15:
                    gValueDA3B24 = 0;
                    CopyText(gLoginSceneMessage, kUnknownError);
                    break;
                case 16:
                    CopyText(gLoginSceneMessage, kServerBusy);
                    gValueDA3B24 = 0;
                    break;
                case 18:
                    gValueDA3B24 = 0;
                    CopyText(gLoginSceneMessage, kWaitBeforeRetry);
                    break;
                case 19:
                    CopyText(gLoginSceneMessage, kNoTrialService);
                    gValueDA3B24 = 0;
                    break;
                case 20:
                    gValueDA3B24 = 0;
                    CopyText(gLoginSceneMessage, kMachineLocked);
                    break;
                case 21:
                    CopyText(gLoginSceneMessage, kMultiline);
                    CopyText(gValue9A620C[6], kMachineCode0);
                    CopyText(gValue9A620C[7], kMachineCode1);
                    CopyText(gValue9A620C[8], kMachineCode2);
                    CopyText(gValue9A620C[9], kMachineCode3);
                    CopyText(gValue9A620C[10], " ");
                    CopyText(gValue9A620C[11], " ");
                    gValueDA3B24 = 0;
                    break;
                case 22:
                    gValueDA3B24 = 0;
                    CopyText(gLoginSceneMessage, kAccountFrozen);
                    break;
                case 23:
                    gOpenUrlAfterMessage = 1;
                    gValueDA3B24 = 0;
                    CopyText(gLoginSceneMessage, kPasswordExpiredMessage);
                    break;
                default:
                    CopyText(gLoginSceneMessage, kCannotConnect);
                    gValueDA3B24 = 0;
                    break;
                }
            }
            detail::Sub_5081A0();
            break;
        }
        case -3:
            world::gCurrentSceneState = 100;
            CopyText(gLoginSceneMessage, kCannotConnectUpdate);
            detail::Sub_5081A0();
            break;
        case -4:
            world::gCurrentSceneState = 100;
            CopyText(gLoginSceneMessage, kRestartForVersion);
            detail::Sub_5081A0();
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
            platform::detail::ShellExecuteA_54D224(
                platform::gLegacyWindow,
                nullptr,
                kPasswordExpiredUrl,
                nullptr,
                nullptr,
                1);
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

    detail::Sub_45EFD0(&gCloseUi, 10, 0, 0, 0);
    detail::Sub_45EFD0(&gSwitchSceneUi, 10, 0, 0, 0);
    if (interactiveTail != 0) {
        DrawCategoryControls();
        if (gValue55A054 >= 0
            && gValueD9F2D4[static_cast<std::size_t>(gValue55A054)] > 10) {
            detail::Sub_45EFD0(&gNextPageUi, 10, 0, 0, 0);
            detail::Sub_45EFD0(&gPreviousPageUi, 10, 0, 0, 0);
            if ((detail::Sub_45F100(&gNextPageUi) & 1) != 0) {
                if (gServerPage < 1) {
                    ++gServerPage;
                }
                detail::PlaySe(51, 320, 240);
            }
            if ((detail::Sub_45F100(&gPreviousPageUi) & 1) != 0) {
                if (gServerPage > 0) {
                    --gServerPage;
                }
                detail::PlaySe(51, 320, 240);
            }
        }

        for (std::int32_t slot = 0; slot < 10; ++slot) {
            const bool available =
                ReadServerAvailability(gValue55A054, gServerPage, slot);
            if (available && gValue55A054 >= 0) {
                if (gServerPage == 0) {
                    detail::Sub_45EFD0(
                        &gServerUi[static_cast<std::size_t>(slot)], 10, 0, 0, 0);
                } else if (gServerPage == 1) {
                    detail::Sub_45EFD0(
                        &gServerUi[static_cast<std::size_t>(slot + 10)], 10, 0, 0, 0);
                }
            }
        }
    } else {
        DrawCategoryControls();
        for (std::int32_t slot = 0; slot < 10; ++slot) {
            if (ReadServerAvailability(gValue55A054, gServerPage, slot)) {
                const std::int32_t style = slot == gSelectedServerGroup ? 2 : 1;
                if (gServerPage == 0) {
                    detail::Sub_45EFD0(
                        &gServerUi[static_cast<std::size_t>(slot)], 10, 0, 0, style);
                } else if (gServerPage == 1) {
                    detail::Sub_45EFD0(
                        &gServerUi[static_cast<std::size_t>(slot + 10)], 10, 0, 0, style);
                }
            }
        }
    }

    detail::HasIDirect3D7();
    detail::Sub_45F090(49, 67, 3, 0x3BEA3, 0, -1);
    detail::Sub_45F090(0, 0, 3, 0x3BEA2, 0, -1);
    return detail::DrawText3(540, 424, 0, 0, kVersion, 0);
}

} // namespace cg::login
