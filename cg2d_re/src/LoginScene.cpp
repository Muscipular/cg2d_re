#include "cg/LoginScene.hpp"

#include "LoginSceneDependencies.hpp"
#include "platform/LegacyFormat.hpp"

#include "cg/LoginInput.hpp"
#include "cg/LoginMessage.hpp"
#include "cg/LoginState.hpp"
#include "cg/Palette.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/LegacyModuleDigest.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/world/SceneState.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>

namespace {

// 原559384窗口标题正文8B；当前GBK文字由SetWindowTextA消费。
constexpr char kWindowName[] = "\xC4\xA7\xC1\xA6\x8C\x9A\xD8\x90";
static_assert(sizeof(kWindowName) == 9);
constexpr char kVersion[] = "7.1.00";
constexpr char kPasswordExpiredUrl[] = "http://www.softstar.com.tw/Game/Index.aspx";

// 原 0x55C7C4「NETWORK還沒準備好。」正文 19 字节；字符串字面量隐式追加 NUL。
constexpr char kNetworkNotReady[] =
    "\x4E\x45\x54\x57\x4F\x52\x4B\xDF\x80\x9B\x5D\x9C\xCA\x82\xE4\xBA\xC3\xA1\xA3";
// 原 0x55C62C「無法連上伺服器。 」正文 17 字节；字符串字面量隐式追加 NUL。
constexpr char kCannotConnect[] =
    "\x9F\x6F\xB7\xA8\xDF\x42\xC9\xCF\xCB\xC5\xB7\xFE\xC6\xF7\xA1\xA3\x20";
// 原 0x55CAD0「此伺服器很擁擠。」正文 16 字节；字符串字面量隐式追加 NUL。
constexpr char kServerCrowded[] =
    "\xB4\xCB\xCB\xC5\xB7\xFE\xC6\xF7\xBA\xDC\x93\xED\x94\x44\xA1\xA3";
// 原 0x55C7B8「版本錯誤」正文 8 字节；字符串字面量隐式追加 NUL。
constexpr char kVersionError[] =
    "\xB0\xE6\xB1\xBE\xE5\x65\xD5\x60";
// 原 0x55C6F4「點數不足」正文 8 字节；字符串字面量隐式追加 NUL。
constexpr char kInsufficientPoints[] =
    "\xFC\x63\x94\xB5\xB2\xBB\xD7\xE3";
// 原 0x55C6E4「此帳號已在線上」正文 14 字节；字符串字面量隐式追加 NUL。
constexpr char kAlreadyOnline[] =
    "\xB4\xCB\x8E\xA4\xCC\x96\xD2\xD1\xD4\xDA\xBE\x80\xC9\xCF";
// 原 0x55C6D0「登入伺服器發生錯誤」正文 18 字节；字符串字面量隐式追加 NUL。
constexpr char kLoginServerError[] =
    "\xB5\xC7\xC8\xEB\xCB\xC5\xB7\xFE\xC6\xF7\xB0\x6C\xC9\xFA\xE5\x65\xD5\x60";
// 原 0x55C6C0「儲值時間已到」正文 12 字节；字符串字面量隐式追加 NUL。
constexpr char kStoredTimeExpired[] =
    "\x83\xA6\xD6\xB5\x95\x72\xE9\x67\xD2\xD1\xB5\xBD";
// 原 0x55C6B0「計費方式有問題」正文 14 字节；字符串字面量隐式追加 NUL。
constexpr char kBillingError[] =
    "\xD3\x8B\xD9\x4D\xB7\xBD\xCA\xBD\xD3\xD0\x86\x96\xEE\x7D";
// 原 0x55C6A4「目前停權中」正文 10 字节；字符串字面量隐式追加 NUL。
constexpr char kSuspended[] =
    "\xC4\xBF\xC7\xB0\xCD\xA3\x99\xE0\xD6\xD0";
// 原 0x55C698「未知的錯誤」正文 10 字节；字符串字面量隐式追加 NUL。
constexpr char kUnknownError[] =
    "\xCE\xB4\xD6\xAA\xB5\xC4\xE5\x65\xD5\x60";
// 原 0x55C678「伺服器負荷過重，請稍候再試。」正文 28 字节；字符串字面量隐式追加 NUL。
constexpr char kServerBusy[] =
    "\xCB\xC5\xB7\xFE\xC6\xF7\xD8\x93\xBA\xC9\xDF\x5E\xD6\xD8\xA3\xAC\xD5\x88\xC9\xD4\xBA\xF2\xD4\xD9\xD4\x87\xA1\xA3";
// 原 0x55C658「請休息一下滑鼠，數秒後再嘗試。」正文 30 字节；字符串字面量隐式追加 NUL。
constexpr char kWaitBeforeRetry[] =
    "\xD5\x88\xD0\xDD\xCF\xA2\xD2\xBB\xCF\xC2\xBB\xAC\xCA\xF3\xA3\xAC\x94\xB5\xC3\xEB\xE1\xE1\xD4\xD9\x87\x4C\xD4\x87\xA1\xA3";
// 原 0x55C640「此伺服器不提供試玩服務」正文 22 字节；字符串字面量隐式追加 NUL。
constexpr char kNoTrialService[] =
    "\xB4\xCB\xCB\xC5\xB7\xFE\xC6\xF7\xB2\xBB\xCC\xE1\xB9\xA9\xD4\x87\xCD\xE6\xB7\xFE\x84\xD5";
// 原 0x55CC00「此電腦沒有權限登入。（行動鎖上鎖）」正文 34 字节；字符串字面量隐式追加 NUL。
constexpr char kMachineLocked[] =
    "\xB4\xCB\xEB\x8A\xC4\x58\x9B\x5D\xD3\xD0\x99\xE0\xCF\xDE\xB5\xC7\xC8\xEB\xA1\xA3\xA3\xA8\xD0\xD0\x84\xD3\xE6\x69\xC9\xCF\xE6\x69\xA3\xA9";
// 原 0x55CB70「此帳號已使用行動鎖凍結。」正文 24 字节；字符串字面量隐式追加 NUL。
constexpr char kAccountFrozen[] =
    "\xB4\xCB\x8E\xA4\xCC\x96\xD2\xD1\xCA\xB9\xD3\xC3\xD0\xD0\x84\xD3\xE6\x69\x83\xF6\xBD\x59\xA1\xA3";
// 原 0x55C618「無法連上更新伺服器」正文 18 字节；字符串字面量隐式追加 NUL。
constexpr char kCannotConnectUpdate[] =
    "\x9F\x6F\xB7\xA8\xDF\x42\xC9\xCF\xB8\xFC\xD0\xC2\xCB\xC5\xB7\xFE\xC6\xF7";
// 原 0x55C5F8「版本錯誤，請重新啟動魔力寶貝。」正文 30 字节；字符串字面量隐式追加 NUL。
constexpr char kRestartForVersion[] =
    "\xB0\xE6\xB1\xBE\xE5\x65\xD5\x60\xA3\xAC\xD5\x88\xD6\xD8\xD0\xC2\x86\xA2\x84\xD3\xC4\xA7\xC1\xA6\x8C\x9A\xD8\x90\xA1\xA3";

static_assert(sizeof(kNetworkNotReady) == 20);
static_assert(sizeof(kCannotConnect) == 18);
static_assert(sizeof(kServerCrowded) == 17);
static_assert(sizeof(kVersionError) == 9);
static_assert(sizeof(kInsufficientPoints) == 9);
static_assert(sizeof(kAlreadyOnline) == 15);
static_assert(sizeof(kLoginServerError) == 19);
static_assert(sizeof(kStoredTimeExpired) == 13);
static_assert(sizeof(kBillingError) == 15);
static_assert(sizeof(kSuspended) == 11);
static_assert(sizeof(kUnknownError) == 11);
static_assert(sizeof(kServerBusy) == 29);
static_assert(sizeof(kWaitBeforeRetry) == 31);
static_assert(sizeof(kNoTrialService) == 23);
static_assert(sizeof(kMachineLocked) == 35);
static_assert(sizeof(kAccountFrozen) == 25);
static_assert(sizeof(kCannotConnectUpdate) == 19);
static_assert(sizeof(kRestartForVersion) == 31);

constexpr char kMultiline[] = "Multiline";
constexpr char kMultilineNew[] = "Multiline_New";

// 原 0x55C798「您可能遇到以下 3 種情形之一：」正文 29 字节；字符串字面量隐式追加 NUL。
constexpr char kAccountHelp0[] =
    "\xC4\xFA\xBF\xC9\xC4\xDC\xD3\xF6\xB5\xBD\xD2\xD4\xCF\xC2\x20\x33\x20\xB7\x4E\xC7\xE9\xD0\xCE\xD6\xAE\xD2\xBB\xA3\xBA";
// 原 0x55C778「1.密碼錯誤，請查明後重新輸入」正文 28 字节；字符串字面量隐式追加 NUL。
constexpr char kAccountHelp1[] =
    "\x31\x2E\xC3\xDC\xB4\x61\xE5\x65\xD5\x60\xA3\xAC\xD5\x88\xB2\xE9\xC3\xF7\xE1\xE1\xD6\xD8\xD0\xC2\xDD\x94\xC8\xEB";
// 原 0x55C754「2.未儲值，請至 SOFTSTAR 網站儲值」正文 32 字节；字符串字面量隐式追加 NUL。
constexpr char kAccountHelp2[] =
    "\x32\x2E\xCE\xB4\x83\xA6\xD6\xB5\xA3\xAC\xD5\x88\xD6\xC1\x20\x53\x4F\x46\x54\x53\x54\x41\x52\x20\xBE\x57\xD5\xBE\x83\xA6\xD6\xB5";
// 原 0x55C738「3.新會員系統中查無此帳號」正文 24 字节；字符串字面量隐式追加 NUL。
constexpr char kAccountHelp3[] =
    "\x33\x2E\xD0\xC2\x95\xFE\x86\x54\xCF\xB5\xBD\x79\xD6\xD0\xB2\xE9\x9F\x6F\xB4\xCB\x8E\xA4\xCC\x96";
// 原 0x55C71C「--->請速回 SOFTSTAR 網站」正文 24 字节；字符串字面量隐式追加 NUL。
constexpr char kAccountHelp4[] =
    "\x2D\x2D\x2D\x3E\xD5\x88\xCB\xD9\xBB\xD8\x20\x53\x4F\x46\x54\x53\x54\x41\x52\x20\xBE\x57\xD5\xBE";
// 原 0x55C700「--->註冊新帳號並移轉舊帳號」正文 26 字节；字符串字面量隐式追加 NUL。
constexpr char kAccountHelp5[] =
    "\x2D\x2D\x2D\x3E\xD4\x5D\x83\xD4\xD0\xC2\x8E\xA4\xCC\x96\x81\x4B\xD2\xC6\xDE\x44\xC5\x66\x8E\xA4\xCC\x96";

static_assert(sizeof(kAccountHelp0) == 30);
static_assert(sizeof(kAccountHelp1) == 29);
static_assert(sizeof(kAccountHelp2) == 33);
static_assert(sizeof(kAccountHelp3) == 25);
static_assert(sizeof(kAccountHelp4) == 25);
static_assert(sizeof(kAccountHelp5) == 27);

// 原 0x55CCEC「＝＝＝＝ 為了維護玩家帳號安全性 ＝＝＝＝」正文 40 字节；字符串字面量隐式追加 NUL。
constexpr char kPasswordChange0[] =
    "\xA3\xBD\xA3\xBD\xA3\xBD\xA3\xBD\x20\x9E\xE9\xC1\xCB\xBE\x53\xD7\x6F\xCD\xE6\xBC\xD2\x8E\xA4\xCC\x96\xB0\xB2\xC8\xAB\xD0\xD4\x20\xA3\xBD\xA3\xBD\xA3\xBD\xA3\xBD";
// 原 0x55CCC0「大宇資訊已於２００５／０５／１３全面變更」正文 40 字节；字符串字面量隐式追加 NUL。
constexpr char kPasswordChange1[] =
    "\xB4\xF3\xD3\xEE\xD9\x59\xD3\x8D\xD2\xD1\xEC\xB6\xA3\xB2\xA3\xB0\xA3\xB0\xA3\xB5\xA3\xAF\xA3\xB0\xA3\xB5\xA3\xAF\xA3\xB1\xA3\xB3\xC8\xAB\xC3\xE6\xD7\x83\xB8\xFC";
// 原 0x55CC94「所有玩家會員密碼、以及魔力寶貝遊戲密碼，」正文 40 字节；字符串字面量隐式追加 NUL。
constexpr char kPasswordChange2[] =
    "\xCB\xF9\xD3\xD0\xCD\xE6\xBC\xD2\x95\xFE\x86\x54\xC3\xDC\xB4\x61\xA1\xA2\xD2\xD4\xBC\xB0\xC4\xA7\xC1\xA6\x8C\x9A\xD8\x90\xDF\x5B\x91\xF2\xC3\xDC\xB4\x61\xA3\xAC";
// 原 0x55CC6C「請玩家至SOFTSTAR會員中心領取新的密碼，」正文 38 字节；字符串字面量隐式追加 NUL。
constexpr char kPasswordChange3[] =
    "\xD5\x88\xCD\xE6\xBC\xD2\xD6\xC1\x53\x4F\x46\x54\x53\x54\x41\x52\x95\xFE\x86\x54\xD6\xD0\xD0\xC4\xEE\x49\xC8\xA1\xD0\xC2\xB5\xC4\xC3\xDC\xB4\x61\xA3\xAC";
// 原 0x55CC48「詳情請參閱SOFTSTAR官方網站資訊：」正文 32 字节；字符串字面量隐式追加 NUL。
constexpr char kPasswordChange4[] =
    "\xD4\x94\xC7\xE9\xD5\x88\x85\xA2\xE9\x86\x53\x4F\x46\x54\x53\x54\x41\x52\xB9\xD9\xB7\xBD\xBE\x57\xD5\xBE\xD9\x59\xD3\x8D\xA3\xBA";
// 原 0x55CC24「　　　　http://www.softstar.com.tw」正文 34 字节；字符串字面量隐式追加 NUL。
constexpr char kPasswordChange5[] =
    "\xA1\xA1\xA1\xA1\xA1\xA1\xA1\xA1\x68\x74\x74\x70\x3A\x2F\x2F\x77\x77\x77\x2E\x73\x6F\x66\x74\x73\x74\x61\x72\x2E\x63\x6F\x6D\x2E\x74\x77";

static_assert(sizeof(kPasswordChange0) == 41);
static_assert(sizeof(kPasswordChange1) == 41);
static_assert(sizeof(kPasswordChange2) == 41);
static_assert(sizeof(kPasswordChange3) == 39);
static_assert(sizeof(kPasswordChange4) == 33);
static_assert(sizeof(kPasswordChange5) == 35);

// 原 0x55CBE8「本台機器之機碼已停用。」正文 22 字节；字符串字面量隐式追加 NUL。
constexpr char kMachineCode0[] =
    "\xB1\xBE\xCC\xA8\x99\x43\xC6\xF7\xD6\xAE\x99\x43\xB4\x61\xD2\xD1\xCD\xA3\xD3\xC3\xA1\xA3";
// 原 0x55CBD4「請檢查遊戲點數。」正文 16 字节；字符串字面量隐式追加 NUL。
constexpr char kMachineCode1[] =
    "\xD5\x88\x99\x7A\xB2\xE9\xDF\x5B\x91\xF2\xFC\x63\x94\xB5\xA1\xA3";
// 原 0x55CBB0「可能是點數不足導致登錄之機碼停用。」正文 34 字节；字符串字面量隐式追加 NUL。
constexpr char kMachineCode2[] =
    "\xBF\xC9\xC4\xDC\xCA\xC7\xFC\x63\x94\xB5\xB2\xBB\xD7\xE3\x8C\xA7\xD6\xC2\xB5\xC7\xE4\x9B\xD6\xAE\x99\x43\xB4\x61\xCD\xA3\xD3\xC3\xA1\xA3";
// 原 0x55CB8C「儲值後請稍待數分鐘系統處理時間。」正文 32 字节；字符串字面量隐式追加 NUL。
constexpr char kMachineCode3[] =
    "\x83\xA6\xD6\xB5\xE1\xE1\xD5\x88\xC9\xD4\xB4\xFD\x94\xB5\xB7\xD6\xE7\x8A\xCF\xB5\xBD\x79\xCC\x8E\xC0\xED\x95\x72\xE9\x67\xA1\xA3";

static_assert(sizeof(kMachineCode0) == 23);
static_assert(sizeof(kMachineCode1) == 17);
static_assert(sizeof(kMachineCode2) == 35);
static_assert(sizeof(kMachineCode3) == 33);

// 原55CAE4密码过期说明：139B正文含五个LF，字面量仅追加末尾NUL。
constexpr char kPasswordExpiredMessage[] =
    "\xD3\x48\x90\xDB\xB5\xC4\xCD\xE6\xBC\xD2\xC4\xFA\xBA\xC3\xA3\xBA\x0A"
    "\xD3\xC9\xEC\xB6\xC4\xFA\xB5\xC4\xDF\x5B\x91\xF2\xC3\xDC\xB4\x61\xD2\xD1\xBD\x9B\xDF\x5E\xC6\xDA\xA3\xAC\x0A"
    "\xD5\x88\xB5\xC7\xC8\xEB\xB4\xF3\xD3\xEE\x95\xFE\x86\x54\xCF\xB5\xBD\x79\xB8\xFC\x93\x51\xDF\x5B\x91\xF2\xC3\xDC\xB4\x61\xA3\xAC\x0A"
    "\xD5\x88\xB6\xA8\xC6\xDA\xB8\xFC\x93\x51\xC3\xDC\xB4\x61\xD2\xD4\xB4\x5F\xB1\xA3\x8E\xA4\xCC\x96\xB0\xB2\xC8\xAB\xA1\xA3\x0A"
    "\xFC\x63\x93\xF4\xB4\x5F\xB6\xA8\xA3\xAC\x8C\xA2\xC7\xB0\xCD\xF9\x53\x4F\x46\x54\x53\x54\x41\x52\xBE\x57\xED\x93\xA1\xA3\x0A";
static_assert(sizeof(kPasswordExpiredMessage) == 140);

// 原559958起25个64B槽的NUL前正文；当前静态中文采用GBK，索引仍由CategoryTitle承接。
constexpr std::array<const char*, 25> kCategoryTitles {
    "\xBF\xA8\xC9\x8F", // 0x559958 卡蓮
    "\xBD\xF0\xC5\xA3\xD7\xF9", // 0x559998 金牛座
    "\xEB\x70\xD7\xD3\xD7\xF9", // 0x5599D8 雙子座
    "\xCF\xE3\xB8\xDB", // 0x559A18 香港
    "\xAA\x7B\xD7\xD3\xD7\xF9", // 0x559A58 獅子座
    "\xCC\x8E\xC5\xAE\xD7\xF9", // 0x559A98 處女座
    "\xCC\xEC\xB3\xD3\xD7\xF9", // 0x559AD8 天秤座
    "\xB8\xE8\xBC\xA7", // 0x559B18 歌姬
    "\x99\xD1\xD6\xAE\xCE\xE8", // 0x559B58 櫻之舞
    "\xC4\xA6\xF4\xC9\xD7\xF9", // 0x559B98 摩羯座
    "\xCB\xAE\xC6\xBF\xD7\xF9", // 0x559BD8 水瓶座
    "\xEB\x70\xF4\x7E\xD7\xF9", // 0x559C18 雙魚座
    "\xCF\xE3\xB8\xDB", // 0x559C58 香港
    "\x9C\x79\xD4\x87", // 0x559C98 測試
    "\xBE\xC5\xFD\x88", // 0x559CD8 九龍
    "\xBF\xA8\xC9\x8F", // 0x559D18 卡蓮
    "\xBD\xF0\xC5\xA3\xD7\xF9", // 0x559D58 金牛座
    "\xC2\xB6\xB1\xC8", // 0x559D98 露比
    "\xCF\xE3\xB8\xDB", // 0x559DD8 香港
    "\xAA\x7B\xD7\xD3\xD7\xF9", // 0x559E18 獅子座
    "\xCC\x8E\xC5\xAE\xD7\xF9", // 0x559E58 處女座
    "\xC2\xB6\xB1\xC8", // 0x559E98 露比
    "\xD0\xC4\xC3\xC0", // 0x559ED8 心美
    "\xC2\xB6\xB1\xC8", // 0x559F18 露比
    "\xD0\xC4\xC3\xC0", // 0x559F58 心美
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
    std::array<std::int32_t, 7> values; // 原成员偏移：+0x00。
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
    ++cg::world::gCurrentSceneState;
}

void SetBaseWindowTitle(char (&buffer)[256])
{
    cg::platform::FormatLegacyString_53B304(buffer, "%s %s", kWindowName, kVersion);
    cg::platform::SetLegacyWindowText(cg::platform::gLegacyWindow, buffer);
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
    cg::platform::FormatLegacyString_53B304(
        buffer,
        "%s %s [%s %s]",
        kWindowName,
        kVersion,
        category,
        server);
    if (slot == 9) {
        cg::platform::FormatLegacyString_53B304(
            buffer,
            "%s %s [\xC4\xE6 %s]", // 原55CD18，特殊标题中的“逆”保留两字节。
            kWindowName,
            kVersion,
            category);
    }
    cg::platform::SetLegacyWindowText(cg::platform::gLegacyWindow, buffer);
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

std::int32_t gValueD69794 = 0; // 原全局：0x00D69794。
std::array<LoginServerRecord, 800> gValueD69798 {}; // 原全局：0x00D69798。
std::array<std::int32_t, 60> gValueD9F2D4 {}; // 原全局：0x00D9F2D4。
std::int16_t gValue58AD94 = -1; // 原全局：0x0058AD94。
char gValueDA38DC[64] {}; // 原全局：0x00DA38DC。
char gValueDA3A1C[64] {}; // 原全局：0x00DA3A1C。
char gValueDA3A5C[64] {}; // 原全局：0x00DA3A5C。
std::int16_t gValueDA3B24 = 0; // 原全局：0x00DA3B24。

static_assert(sizeof(gValueD69798) == 800 * 0xC1);
static_assert(sizeof(gValueD9F2D4) == 60 * sizeof(std::int32_t));

namespace detail {

// NLD01：负索引／>=800返回-1；地址含NUL复制、atoi低W16端口，成功0，不读available。
// 原函数：0x00507D90。
std::int32_t GetServerAddressInfo_507D90(
    std::int32_t serverIndex,
    char* serverAddress,
    std::uint16_t* port)
{
    if (serverIndex < 0
        || serverIndex >= static_cast<std::int32_t>(gValueD69798.size())) {
        return -1;
    }

    const auto& record = gValueD69798[static_cast<std::size_t>(serverIndex)];
    std::strcpy(serverAddress, reinterpret_cast<const char*>(record.opaque.data()));
    *port = static_cast<std::uint16_t>(
        platform::Atoi_53C649(reinterpret_cast<const char*>(record.opaque.data() + 0x80)));
    return 0;
}

} // namespace detail

// 原函数：0x0045FEA0。
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
        static_cast<void>(render::SelectPalette(27, 0));
        detail::LoadAndPlayBGM(17);
    }

    if (world::gCurrentSceneState == 1) {
        interactiveTail = 1;
        if ((detail::Sub_45F100(&gCloseUi) & 1) != 0) {
            detail::PlaySe(54, 320, 240);
            platform::PostLegacyWindowMessage(
                platform::gLegacyWindow, 0x10, 0, 0);
        } else if ((detail::Sub_45F100(&gSwitchSceneUi) & 1) != 0) {
            world::SetPendingSceneType(1);
            static_cast<void>(render::SelectPalette(20, 1));
            detail::PlaySe(54, 320, 240);
            detail::Sub_52B990();
        } else if ((detail::Sub_45F100(SelectedCategoryControl()) & 1) != 0) {
            gValue55A054 = -1;
            gServerPage = 0;
            detail::PlaySe(54, 320, 240);
        } else {
            std::int32_t category;
            for (std::int32_t index = 0; index < 60; ++index) {
                const std::int32_t input = detail::Sub_45F100(CategoryScanRecord(index));
                category = gValue55A054;
                if ((input & 1) != 0
                    && gValueD9F2D4[static_cast<std::size_t>(index)] > 0
                    && category < 0) {
                    gValue55A054 = index;
                    detail::PlaySe(51, 320, 240);

                    const std::int32_t sourceA = CategoryField(index, 5);
                    const std::int32_t sourceB = CategoryField(index, 4);
                    const std::int32_t imageDelta = sourceB - sourceA;
                    std::int32_t imageNo = sourceA + 2;
                    for (std::int32_t server = 0; server < 20; ++server) {
                        gServerUi[static_cast<std::size_t>(server)].values[4] =
                            imageDelta + imageNo;
                        gServerUi[static_cast<std::size_t>(server)].values[5] =
                            imageNo;
                        imageNo += 2;
                    }
                    category = gValue55A054;
                    break;
                }
            }
            for (std::int32_t slot = 0; slot < 10; ++slot) {
                if (category >= 0) {
                    const std::uint8_t input = static_cast<std::uint8_t>(
                        detail::Sub_45F100(&gServerUi[static_cast<std::size_t>(slot)]));
                    if ((input & 0x40u) != 0) {
                        gSelectedServerGroup = slot;
                    }
                    if (static_cast<std::int8_t>(input) < 0) {
                        const std::int32_t page = gServerPage;
                        const std::int32_t serverIndex =
                            category * 20 + page * 10 + slot;
                        if (gValueD69798[static_cast<std::size_t>(serverIndex)].available != 0) {
                            SetServerWindowTitle(title, serverIndex, page, slot);
                            gValue58AD94 = static_cast<std::int16_t>(serverIndex);
                            gSelectedServerGroup = slot;
                            detail::PlaySe(53, 320, 240);
                            platform::ComputeExecutableDigest_5202E0(gValueDA3A1C);
                            ReadInputValueFF494F_5203E0(gValueDA38DC);
                            ReadInputValueFF4982_520440(gValueDA3A5C);
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
            CopyText(gLoginSceneMessage, kNetworkNotReady);
            cg::network::ShutdownNetworkRuntime_5081A0();
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
            cg::network::ShutdownNetworkRuntime_5081A0();
            break;
        }
        case -3:
            world::gCurrentSceneState = 100;
            CopyText(gLoginSceneMessage, kCannotConnectUpdate);
            cg::network::ShutdownNetworkRuntime_5081A0();
            break;
        case -4:
            world::gCurrentSceneState = 100;
            CopyText(gLoginSceneMessage, kRestartForVersion);
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
            platform::ExecuteLegacyShell(
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

    static_cast<void>(render::HasLegacy3DDevice_44B610());
    detail::Sub_45F090(49, 67, 3, 0x3BEA3, 0, -1);
    detail::Sub_45F090(0, 0, 3, 0x3BEA2, 0, -1);
    return detail::DrawText3(540, 424, 0, 0, kVersion, 0);
}

} // namespace cg::login
