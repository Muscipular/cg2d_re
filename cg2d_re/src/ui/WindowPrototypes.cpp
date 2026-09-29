#include "WindowPrototypeDependencies.hpp"

#include "cg/Input.hpp"

namespace cg::ui::detail {
namespace {

// 原56CCC8的四字节元数据和593850借用，不创建另一份默认文本。
WindowTextPrototype gWindowText56CCC8 {{}, input::LegacyText593850()};

WindowGraphicData gWindowGraphic562B98 {243000, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562BB0 {244290, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562BC8 {244291, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562BE0 {244294, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562BF8 {98131, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562C10 {98140, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562C28 {98143, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562C40 {98146, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562C58 {244297, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindowPlaceholder586054[] = "\x81\x48\x81\x48\x81\x48\x81\x48";
WindowTextPrototype gWindowText562C70 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowPlaceholder586054,
};
WindowTextPrototype gWindowText562C78 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowPlaceholder586054,
};
std::int32_t gWindowAnimationImage562C80 = 100000;
WindowNumberData gWindowNumber562CB0 {gInventoryMoneyTextCB086C.data(), 10, 3, 1, 0};
WindowNumberData gWindowNumber562CB8 {gInventoryValueTextCC3ECC.data(), 10, 3, 1, 0};

WindowGraphicData gWindowGraphic56C348 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56C360 {0x3BA42, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56C378 {0x3BA46, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56C390 {0x17F63, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56C3A8 {0x17F53, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56C3C0 {0x17F5C, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56C3D8 {0x17F5F, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56C3F0 {0x3BA49, 0, 0, 0, 0, 0xFFFFFFFFu};
std::int32_t gWindowAnimationImage56C418 = 100000;
WindowNumberData gWindowNumber56C420 {gBattleMoneyTextCF469C.data(), 10, 3, 1, 0};
WindowNumberData gWindowNumber56C428 {gBattleValueTextC8A960.data(), 10, 3, 1, 0};

WindowGraphicData gWindowGraphic56C9F8 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56CA10 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56CA28 {0x3BE4B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56CA40 {0x3BE4E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56CA58 {0x3BE54, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56CA70 {0x3BE51, 0, 0, 0, 0, 0xFFFFFFFFu};
std::int32_t gWindowAnimationImageD6780C = 0;

WindowTextPrototype gWindowText56D828 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, input::LegacyText593850(),
};
std::int32_t gWindowAnimationImage56D830 = 100000;

WindowGraphicData gWindowGraphic5734D8 {0, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5734F0 {0x3BB3C, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573508 {0x3BB3D, 0, 0, 0, 0, 0x80FFFFFFu};
WindowGraphicData gWindowGraphic573520 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573538 {0x3B577, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573550 {0x3BB3E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573568 {0x3BB41, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573580 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573598 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowTextPrototype gWindowText5735B0 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    "\xAF\x64\xA8\xA5\xAA\x4F\xA4\xE5\xA6\x72\xA6\x43",
};
WindowTextPrototype gWindowText5735B8 {
    {std::byte{0}, std::byte{10}, std::byte{0}, std::byte{0}}, "\xA6\x57\xBA\xD9",
};
WindowTextPrototype gWindowText5735C0 {
    {std::byte{0}, std::byte{10}, std::byte{0}, std::byte{0}}, "\xAF\x64\xA8\xA5\xAE\xC9\xB6\xA1",
};

WindowGraphicData gWindowGraphic563080 {243000, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic563098 {244309, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5630B0 {243090, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5630C8 {243093, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowNumberData gWindowNumber5630E8 {gNumberWindowTextCC4D1C.data(), 10, 3, 1, 0};

WindowGraphicData gWindowGraphic571018 {244390, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571030 {243048, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571048 {243027, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571060 {243021, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571078 {243024, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571090 {243143, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5710A8 {244391, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5710C0 {243141, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5710D8 {243096, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5710F0 {243090, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571108 {243093, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571120 {243000, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowTextPrototype gWindowText571148 {
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, input::LegacyText593850(),
};
WindowNumberData gWindowNumber571150 {gStorageAmountTextsCB1DF8.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber571158 {gStorageAmountTextsCB1DF8.data() + 11, 0, 3, 1, 0};
std::uint32_t gWindowMouseFlagsD67880 = 0;
std::uint32_t gWindowMouseFlagsD6788C = 0;

WindowGraphicData gWindowGraphic5716C8 {245221, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5716E0 {243048, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5716F8 {245222, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571710 {245220, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571728 {243000, 0, 0, 0, 0, 0xFFFFFFFFu};

WindowGraphicData gWindowGraphic5718A8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5718C0 {0x3BA6A, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5718D8 {0x3BA6B, 0, 0, 0, 0, 0x80FFFFFFu};
WindowGraphicData gWindowGraphic5718F0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571908 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571920 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571938 {0x3B9F7, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571950 {0x3BA6C, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571968 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic571980 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindowTitle585E88[] = "\xAE\x61\xB1\xDA\xC3\x7E";
constexpr char kWindowLine585E80[] = "\xAA\xAC\xBA\x41";
WindowTextPrototype gWindowText571998 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTitle585E88,
};
WindowTextPrototype gWindowTexts5719A0[9] {
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowLine585E80},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowLine585E80},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowLine585E80},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowLine585E80},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowLine585E80},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowLine585E80},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowLine585E80},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowLine585E80},
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowLine585E80},
};
std::uint32_t gWindowMouseFlagsD67898 = 0;

WindowGraphicData gWindowGraphic572940 {0x3BA88, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic572958 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic572970 {0x3BA89, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic572988 {0x3BA8C, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5729A0 {0x3B562, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic572AD8 {0, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowTextPrototype gWindowText572AF0 {
    {std::byte{0x8A}, std::byte{1}, std::byte{0}, std::byte{0}}, input::LegacyText593850(),
};
std::uint32_t gWindowMouseFlagsD678B8 = 0;
std::uint32_t gWindowMouseFlagsD678C4 = 0;
std::uint32_t gWindowMouseFlagsD678D0 = 0;

WindowGraphicData gWindowGraphic56CCF0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56CD08 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56CD20 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56CD38 {0x3B562, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56CD50 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56CD68 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindowListPlaceholder585F64[] = "\xA1\xBD";
constexpr char kWindowListTitle585F54[] =
    "\xBD\xD0\xBF\xEF\xBE\xDC\xB9\xEF\xB6\x48\xA1\x43";
WindowTextPrototype gWindowTexts56CD80[5] {
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindowListPlaceholder585F64},
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindowListPlaceholder585F64},
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindowListPlaceholder585F64},
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindowListPlaceholder585F64},
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindowListPlaceholder585F64},
};
WindowTextPrototype gWindowText56CDA8 {
    {std::byte{4}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindowListTitle585F54,
};

// 同一4A4210在原表也用于release；仅适配未消费返回值，不转换函数指针。
void ReleaseWindow_4A4210()
{
    static_cast<void>(Sub_4A4210(0));
}

// 注释为窗口编号和原型原地址；36/37只有一个可变记录。
std::array<WindowPrototype, 70> gWindowPrototypeRecords {{
    {0, 0, 0, 0, 0, 0, 0x00000000u, 0, nullptr, nullptr, nullptr, nullptr}, // 0: 54EFB8
    {0, 5, 0, 0, 197, 40, 0x80000080u, 24, gWindowElements55F0A0, &Sub_470A80, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 1: 54E2A0
    {0, 5, 0, 0, 197, 32, 0x80000080u, 7, gWindowElements55F3A0, &Sub_470AB0, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 2: 54E2D0
    {0, 5, 0, 0, 19, 34, 0x80000080u, 2, gWindowElements55F480, &Sub_4A4210, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 3: 54E300
    {0, 5, 437, 0, 210, 28, 0x80000080u, 11, gWindowElements55ED60, &Sub_46FBA0, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 4: 54E240
    {0, 5, 626, 0, 14, 33, 0x80000080u, 2, gWindowElements55EEC0, &Sub_46FBA0, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 5: 54E270
    {2, 5, 0, 452, 640, 30, 0x80010101u, 14, gWindowElements55F5C0, &Sub_4731E0, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 6: 54E330
    {0, 4, 15, 97, 315, 261, 0x80000080u, 73, gWindowElements55FD60, &Sub_474540, &Sub_46FBC0, &Sub_474310}, // 7: 54E360
    {0, 4, 15, 97, 315, 261, 0x80000080u, 43, gWindowElements560840, &Sub_475DF0, &Sub_46FBC0, &Sub_474360}, // 8: 54E390
    {0, 4, 15, 97, 315, 261, 0x80000080u, 37, gWindowElements560F50, &Sub_4774C0, &Sub_46FBC0, &Sub_4744A0}, // 9: 54E3C0
    {0, 4, 25, 122, 241, 258, 0x80010101u, 30, gWindowElements561B78, &Sub_47A620, &Sub_47A7A0, &Sub_47B0E0}, // 10: 54E420
    {0, 4, 37, 102, 563, 239, 0x80010101u, 31, gWindowElements5620E0, &Sub_4810A0, &Sub_482000, &Sub_481010}, // 11: 54E450
    {0, 4, 383, 118, 198, 232, 0x80010101u, 15, gWindowElements562620, &Sub_486040, &Sub_46FBC0, &Sub_485690}, // 12: 54E480
    {0, 4, 383, 118, 198, 232, 0x80010101u, 14, gWindowElements562930, &Sub_487AE0, &Sub_487B80, &Sub_487660}, // 13: 54E4B0
    {0, 4, 340, 120, 278, 259, 0x80010101u, 24, gWindowElements562CD8, &Sub_488990, &Sub_488B10, &Sub_488510}, // 14: 54E4E0
    {0, 4, 3, 85, 323, 281, 0x80010101u, 259, gWindowElements563500, &Sub_48C1B0, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 15: 54E540
    {0, 4, 365, 83, 245, 306, 0x80010101u, 66, gWindowElements5659C0, &Sub_48D9A0, &Sub_46FBC0, &Sub_48DCC0}, // 16: 54E570
    {0, 4, 365, 83, 245, 306, 0x80010101u, 46, gWindowElements566200, &Sub_48DAB0, &Sub_46FBC0, &Sub_48DD00}, // 17: 54E5A0
    {0, 4, 365, 83, 245, 306, 0x80010101u, 51, gWindowElements566810, &Sub_48DBB0, &Sub_48DCB0, &Sub_48DD40}, // 18: 54E5D0
    {0, 4, 38, 95, 225, 220, 0x80010101u, 71, gWindowElements567028, &Sub_4923F0, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 19: 54E600
    {0, 4, 357, 90, 256, 289, 0x80010101u, 82, gWindowElements567AE8, &Sub_492410, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 20: 54E630
    {0, 4, 2, 64, 331, 381, 0x80010101u, 111, gWindowElements56DB38, &Sub_4A0970, &Sub_4A09B0, &ReleaseWindow_4A4210}, // 21: 54E990
    {0, 4, 383, 81, 211, 210, 0x80010101u, 9, gWindowElements5701F8, &Sub_4A53F0, &Sub_46FBC0, &Sub_4A5650}, // 22: 54EA20
    {0, 4, 383, 81, 211, 259, 0x80010101u, 13, gWindowElements570318, &Sub_4A56F0, &Sub_46FBC0, &Sub_4A5650}, // 23: 54EA50
    {0, 4, 409, 100, 207, 222, 0x80010101u, 32, gWindowElements5689C8, &Sub_493D60, &Sub_493DB0, &Sub_493E10}, // 24: 54E660
    {0, 4, 15, 117, 286, 233, 0x80010101u, 25, gWindowElements568F78, &Sub_495130, &Sub_4951C0, &ReleaseWindow_4A4210}, // 25: 54E690
    {2, 5, 341, 0, 299, 69, 0x80010101u, 14, gWindowElements56B710, &Sub_496B90, &Sub_496D50, &ReleaseWindow_4A4210}, // 26: 54E720
    {0, 4, 100, 100, 276, 253, 0x80010101u, 22, gWindowElements56C430, &Sub_499990, &Sub_4999D0, &ReleaseWindow_4A4210}, // 27: 54E7E0
    {0, 4, 100, 100, 356, 223, 0x80010101u, 8, gWindowElements56C800, &Sub_49B630, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 28: 54E810
    {2, 3, 492, 0, 148, 108, 0x80010101u, 2, gWindowElements56B8D8, &Sub_4A4210, &Sub_49BDB0, &ReleaseWindow_4A4210}, // 29: 54E750
    {1, 3, 40, 40, 490, 301, 0x80010101u, 66, gWindowElements56B9B8, &Sub_4991C0, &Sub_4996C0, &ReleaseWindow_4A4210}, // 30: 54E780
    {3, 3, 245, 170, 150, 140, 0x80010101u, 4, gWindowElements56C2C8, &Sub_499790, &Sub_499830, &ReleaseWindow_4A4210}, // 31: 54E7B0
    {2, 3, 224, 162, 192, 96, 0x80010101u, 2, gWindowElements56C9B8, &Sub_49BD40, &Sub_49BDB0, &ReleaseWindow_4A4210}, // 32: 54E840
    {0, 4, 353, 55, 184, 21, 0x80010101u, 7, gWindowElements56AF50, &Sub_495D70, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 33: 54E6C0
    {0, 4, 575, 97, 61, 260, 0x80010101u, 20, gWindowElements56B2B8, &Sub_495F50, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 34: 54E6F0
    {0, 4, 364, 428, 640, 25, 0x80010101u, 20, gWindowElements570CB0, &Sub_4A6DD0, &Sub_4A82C0, &ReleaseWindow_4A4210}, // 35: 54EAB0
    {2, 3, 0, 0, 0, 30, 0x80010101u, 19, gWindowElements56CA88, &Sub_49C710, &Sub_49CAD0, &Sub_49CDF0}, // 36: 54E870
    {0, 4, 300, 82, 567, 339, 0x80010101u, 6, gWindowElements571740, &Sub_4AC140, &Sub_4AC1B0, &Sub_4AC0F0}, // 38: 54EB40
    {3, 3, 196, 180, 256, 96, 0x80010101u, 1, gWindowElements56CCD0, &Sub_49E750, &Sub_49BDB0, &ReleaseWindow_4A4210}, // 39: 54E8A0
    {0, 4, 383, 71, 211, 206, 0x80010101u, 21, gWindowElements56FD70, &Sub_4A4620, &Sub_4A4650, &ReleaseWindow_4A4210}, // 40: 54E9F0
    {0, 4, 20, 25, 567, 339, 0x80010101u, 38, gWindowElements571160, &Sub_4A9020, &Sub_4A9160, &Sub_4A83D0}, // 41: 54EB10
    {0, 4, 100, 100, 126, 136, 0x80010101u, 8, gWindowElements5630F0, &Sub_48A8C0, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 42: 54E510
    {0, 4, 20, 25, 567, 339, 0x80010101u, 20, gWindowElements572AF8, &Sub_4B0290, &Sub_4B08D0, &Sub_4B0200}, // 43: 54EBD0
    {0, 4, 160, 125, 320, 192, 0x80010101u, 19, gWindowElements56CDB0, &Sub_49E8F0, &Sub_49EAA0, &ReleaseWindow_4A4210}, // 44: 54E8D0
    {0, 4, 160, 125, 320, 192, 0x80010101u, 19, gWindowElements56D0B0, &Sub_49F060, &Sub_49F1A0, &Sub_49F040}, // 45: 54E900
    {0, 4, 60, 105, 520, 243, 0x80010101u, 34, gWindowElements56D3E8, &Sub_49F7B0, &Sub_499830, &Sub_49F790}, // 46: 54E930
    {0, 4, 209, 159, 222, 162, 0x80010101u, 34, gWindowElements5706D0, &Sub_4AE5F0, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 47: 54EA80
    {0, 4, 100, 60, 317, 325, 0x80010101u, 76, gWindowElements571F18, &Sub_4AEAE0, &Sub_4AEC10, &ReleaseWindow_4A4210}, // 48: 54EBA0
    {0, 4, 68, 84, 280, 317, 0x80010101u, 37, gWindowElements572F90, &Sub_4B4B00, &Sub_4B4B90, &Sub_4B4B70}, // 49: 54EC00
    {0, 4, 5, 20, 634, 424, 0x80010101u, 36, gWindowElements5735C8, &Sub_4B5AD0, &Sub_46FBC0, &Sub_4B5B50}, // 50: 54EC30
    {0, 4, 100, 100, 231, 305, 0x80010101u, 26, gWindowElements5719E8, &Sub_4AD800, &Sub_4AD9F0, &Sub_4AD780}, // 51: 54EB70
    {2, 4, 209, 117, 222, 216, 0x80010101u, 3, gWindowElements56D838, &Sub_49FD20, &Sub_49FE40, &Sub_49FF70}, // 52: 54E960
    {0, 4, 10, 30, 549, 366, 0x80010101u, 66, gWindowElements573D80, &Sub_4B9310, &Sub_4B99D0, &Sub_4B6DC0}, // 53: 54EC60
    {0, 4, 10, 30, 549, 366, 0x80010101u, 107, gWindowElements5748E8, &Sub_4BF7C0, &Sub_4BF950, &Sub_4BD500}, // 54: 54EC90
    {0, 4, 100, 100, 243, 206, 0x80000080u, 29, gWindowElements56F860, &Sub_4A4180, &Sub_4A41A0, &ReleaseWindow_4A4210}, // 55: 54E9C0
    {0, 4, 96, 108, 448, 240, 0x80010101u, 15, gWindowElements576F90, &Sub_4C5850, &Sub_4C5B70, &Sub_4C5C00}, // 56: 54ED20
    {0, 4, 0, 428, 0, 0, 0x80010101u, 2, gWindowElements570FD8, &Sub_4A8240, &Sub_4A82C0, &ReleaseWindow_4A4210}, // 57: 54EAE0
    {0, 4, 15, 97, 315, 261, 0x80000080u, 34, gWindowElements561558, &Sub_478320, &Sub_46FBC0, &Sub_4744F0}, // 58: 54E3F0
    {0, 4, 38, 95, 225, 220, 0x80010101u, 32, gWindowElements577298, &Sub_4C94B0, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 59: 54ED60
    {0, 4, 5, 20, 634, 424, 0x80000080u, 162, gWindowElements5777A8, &Sub_4C9EE0, &Sub_46FBC0, &Sub_4CA300}, // 60: 54ED90
    {0, 4, 5, 20, 305, 398, 0x80000080u, 71, gWindowElements578CC8, &Sub_4CA410, &Sub_46FBC0, &Sub_4CAB60}, // 61: 54EDC0
    {0, 4, 5, 20, 317, 250, 0x80000080u, 22, gWindowElements579710, &Sub_4CAF60, &Sub_46FBC0, &Sub_4CB530}, // 62: 54EDF0
    {0, 4, 383, 81, 232, 210, 0x80000080u, 16, gWindowElements579B28, &Sub_4CB550, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 63: 54EE20
    {0, 4, 15, 97, 317, 250, 0x80000080u, 21, gWindowElements57A070, &Sub_4CB9E0, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 64: 54EE50
    {0, 4, 38, 95, 251, 210, 0x80010101u, 71, gWindowElements57A450, &Sub_4CC6C0, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 65: 54EE80
    {0, 4, 15, 97, 465, 321, 0x80000080u, 25, gWindowElements57AEC0, &Sub_4CBD90, &Sub_46FBC0, &ReleaseWindow_4A4210}, // 66: 54EEB0
    {0, 4, 10, 30, 549, 366, 0x80000080u, 48, gWindowElements57B508, &Sub_4CD4F0, &Sub_4CDB10, &Sub_4CDE00}, // 67: 54EF58
    {0, 4, 10, 30, 549, 366, 0x80000080u, 43, gWindowElements57BB08, &Sub_4CD4F0, &Sub_4CDB10, &Sub_4CDE00}, // 68: 54EF88
    {0, 4, 10, 30, 549, 366, 0x80010101u, 70, gWindowElements575988, &Sub_4C1C50, &Sub_4C1EC0, &Sub_4C1390}, // 69: 54ECC0
    {0, 4, 10, 30, 549, 366, 0x80010101u, 70, gWindowElements576588, &Sub_4C3F00, &Sub_4C4180, &Sub_4C3530}, // 70: 54ECF0
}};

} // namespace

WindowPrototype& gWindowPrototype54E2A0 = gWindowPrototypeRecords[1];
WindowPrototype& gWindowPrototype54E240 = gWindowPrototypeRecords[4];
WindowPrototype& gWindowPrototype54E330 = gWindowPrototypeRecords[6];

WindowElementPrototype gWindowElements56CCD0[1] {
    {3, 0, 44, 0, 0, 1, {.text = &gWindowText56CCC8}, &Sub_4B0F10},
};

WindowElementPrototype gWindowElements5630F0[8] {
    {5, 0, 0, 126, 136, 1, {.unusedValue = 0}, &Sub_48A900},
    {1, 106, 8, 11, 11, 1, {.graphic = &gWindowGraphic563080}, &Sub_4C1FB0},
    {0, 18, 46, 106, 85, 1, {.unusedValue = 0}, &Sub_48A950},
    {1, 92, 29, 11, 11, 1, {.graphic = &gWindowGraphic5630B0}, &Sub_48AA60},
    {1, 105, 29, 11, 11, 1, {.graphic = &gWindowGraphic5630C8}, &Sub_48AB10},
    {7, 85, 30, 0, 0, 1, {.number = &gWindowNumber5630E8}, &Sub_4B0F10},
    {1, 0, 0, 126, 136, 1, {.graphic = &gWindowGraphic563098}, &Sub_4B0F10},
    {0, 128, 7, 18, 78, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements562CD8[24] {
    {1, 258, 8, 11, 11, 1, {.graphic = &gWindowGraphic562B98}, &Sub_488C50},
    {1, 221, 28, 49, 17, 1, {.graphic = &gWindowGraphic562BC8}, &Sub_488D30},
    {7, 198, 31, 0, 0, 1, {.number = &gWindowNumber562CB0}, &Sub_488E60},
    {7, 99, 31, 0, 0, 1, {.number = &gWindowNumber562CB8}, &Sub_488E80},
    {1, 8, 0, 278, 259, 1, {.graphic = &gWindowGraphic562BB0}, &Sub_488FB0},
    {2, -81, 163, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage562C80}, &Sub_4B0F10},
    {3, -81, 28, 0, 0, 0, {.text = &gWindowText562C70}, &Sub_4B0F10},
    {3, -81, 48, 0, 0, 0, {.text = &gWindowText562C78}, &Sub_4B0F10},
    {0, -114, 188, 33, 17, 0, {.unusedValue = 0}, &Sub_488930},
    {0, -81, 188, 33, 17, 0, {.unusedValue = 0}, &Sub_488950},
    {0, -121, 73, 78, 113, 0, {.unusedValue = 0}, &Sub_488970},
    {1, -170, 73, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &Sub_4B0F10},
    {1, -41, 73, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &Sub_4B0F10},
    {1, -170, 123, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &Sub_4B0F10},
    {1, -41, 123, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &Sub_4B0F10},
    {1, -170, 23, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &Sub_4B0F10},
    {1, -41, 23, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &Sub_4B0F10},
    {1, -16, 17, 293, 222, 0, {.graphic = &gWindowGraphic562C40}, &Sub_489760},
    {1, 2, 17, 19, 46, 1, {.graphic = &gWindowGraphic562BE0}, &Sub_488EA0},
    {1, 2, 62, 19, 46, 1, {.graphic = &gWindowGraphic562BF8}, &Sub_488EC0},
    {1, 2, 107, 19, 46, 1, {.graphic = &gWindowGraphic562C10}, &Sub_488EC0},
    {1, 2, 152, 19, 46, 1, {.graphic = &gWindowGraphic562C28}, &Sub_488EC0},
    {0, -197, 17, 293, 222, 1, {.unusedValue = 0}, &Sub_474170},
    {0, 282, 7, 18, 78, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements56C430[22] {
    {1, 258, 8, 11, 11, 1, {.graphic = &gWindowGraphic56C348}, &Sub_4C1FB0},
    {7, 198, 31, 0, 0, 1, {.number = &gWindowNumber56C420}, &Sub_499B00},
    {7, 99, 31, 0, 0, 1, {.number = &gWindowNumber56C428}, &Sub_499B20},
    {1, 8, 0, 276, 253, 1, {.graphic = &gWindowGraphic56C360}, &Sub_499C30},
    {2, -81, 163, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56C418}, &Sub_4B0F10},
    {3, -81, 28, 0, 0, 0, {.text = &gWindowText562C70}, &Sub_4B0F10},
    {3, -81, 48, 0, 0, 0, {.text = &gWindowText562C78}, &Sub_4B0F10},
    {0, -114, 188, 33, 17, 0, {.unusedValue = 0}, &Sub_499930},
    {0, -81, 188, 33, 17, 0, {.unusedValue = 0}, &Sub_499950},
    {0, -121, 73, 78, 113, 0, {.unusedValue = 0}, &Sub_499970},
    {1, -170, 73, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &Sub_4B0F10},
    {1, -41, 73, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &Sub_4B0F10},
    {1, -170, 123, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &Sub_4B0F10},
    {1, -41, 123, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &Sub_4B0F10},
    {1, -170, 23, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &Sub_4B0F10},
    {1, -41, 23, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &Sub_4B0F10},
    {1, 2, 17, 19, 46, 1, {.graphic = &gWindowGraphic56C378}, &Sub_499B40},
    {1, 2, 62, 19, 46, 1, {.graphic = &gWindowGraphic56C3A8}, &Sub_499B60},
    {1, 2, 107, 19, 46, 1, {.graphic = &gWindowGraphic56C3C0}, &Sub_499B60},
    {1, 2, 152, 19, 46, 1, {.graphic = &gWindowGraphic56C3D8}, &Sub_499B60},
    {1, -197, 17, 212, 210, 1, {.graphic = &gWindowGraphic56C390}, &Sub_49AC40},
    {0, 320, 7, 18, 78, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements56CA88[19] {
    {1, 0, 0, 66, 17, 1, {.graphic = &gWindowGraphic56C9F8}, &Sub_49CE70},
    {1, 0, 20, 66, 17, 1, {.graphic = &gWindowGraphic56CA10}, &Sub_49CE70},
    {1, 0, 20, 66, 17, 1, {.graphic = &gWindowGraphic56CA28}, &Sub_49CE70},
    {1, 60, 0, 66, 17, 1, {.graphic = &gWindowGraphic56CA40}, &Sub_49CE70},
    {1, 60, 20, 66, 17, 1, {.graphic = &gWindowGraphic56CA58}, &Sub_49CE70},
    {1, 120, 0, 66, 17, 1, {.graphic = &gWindowGraphic56CA70}, &Sub_49CE70},
    {0, 120, 0, 28, 8, 1, {.unusedValue = 0}, &Sub_49CF70},
    {5, 80, 42, 0, 0, 1, {.unusedValue = 0}, &Sub_4B0F10},
    {2, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &Sub_4B0F10},
    {2, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &Sub_4B0F10},
    {2, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &Sub_4B0F10},
    {2, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &Sub_4B0F10},
    {2, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &Sub_4B0F10},
    {2, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &Sub_4B0F10},
    {2, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &Sub_4B0F10},
    {2, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &Sub_4B0F10},
    {2, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &Sub_4B0F10},
    {2, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &Sub_4B0F10},
    // 原运行第19项与下一文本/元素源字节重叠；值1不是函数，visible0阻止正常调用。
    {0, 0x593850, 3, 0, 44, 0, {.unusedValue = 0},
        reinterpret_cast<WindowElementCallback>(std::uintptr_t{1})},
};

WindowElementPrototype gWindowElements56CDB0[19] {
    {1, 270, 60, 0, 14, 1, {.graphic = &gWindowGraphic56CCF0}, &Sub_4B0F10},
    {4, 270, 60, 11, 85, 1,
        {.mouseFlags = reinterpret_cast<std::uint32_t*>(&gWindowGraphic56CCF0.imageNo)}, &Sub_4741A0},
    {1, 270, 50, 11, 11, 1, {.graphic = &gWindowGraphic56CD08}, &Sub_49EB50},
    {1, 270, 144, 11, 11, 1, {.graphic = &gWindowGraphic56CD20}, &Sub_49EC00},
    {1, 282, 47, 18, 18, 1, {.graphic = &gWindowGraphic56CD50}, &Sub_49ECC0},
    {1, 282, 140, 18, 18, 1, {.graphic = &gWindowGraphic56CD68}, &Sub_49ED80},
    {0, 0, 0, 320, 192, 1, {.unusedValue = 0}, &Sub_49EE60},
    {1, 135, 159, 49, 17, 1, {.graphic = &gWindowGraphic56CD38}, &Sub_49EEB0},
    {3, 70, 25, 0, 0, 1, {.text = &gWindowText56CDA8}, &Sub_4B0F10},
    {3, 40, 55, 0, 0, 1, {.text = &gWindowTexts56CD80[0]}, &Sub_4B0F10},
    {3, 40, 75, 0, 0, 1, {.text = &gWindowTexts56CD80[1]}, &Sub_4B0F10},
    {3, 40, 95, 0, 0, 1, {.text = &gWindowTexts56CD80[2]}, &Sub_4B0F10},
    {3, 40, 115, 0, 0, 1, {.text = &gWindowTexts56CD80[3]}, &Sub_4B0F10},
    {3, 40, 135, 0, 0, 1, {.text = &gWindowTexts56CD80[4]}, &Sub_4B0F10},
    {0, 40, 55, 200, 13, 1, {.unusedValue = 0}, &Sub_49EF60},
    {0, 40, 75, 200, 13, 1, {.unusedValue = 0}, &Sub_49EF60},
    {0, 40, 95, 200, 13, 1, {.unusedValue = 0}, &Sub_49EF60},
    {0, 40, 115, 200, 13, 1, {.unusedValue = 0}, &Sub_49EF60},
    {0, 40, 135, 200, 13, 1, {.unusedValue = 0}, &Sub_49EF60},
};

WindowElementPrototype gWindowElements56D838[3] {
    {3, 40, 10, 0, 0, 1, {.text = &gWindowText56D828}, &Sub_4B0F10},
    {2, 40, 135, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56D830}, &Sub_4B0F10},
    {0, 99, 142, 108, 0, 1, {.unusedValue = 0}, &Sub_49FFA0},
};

WindowElementPrototype gWindowElements5735C8[36] {
    {1, 593, 8, 12, 12, 1, {.graphic = &gWindowGraphic573520}, &Sub_4B5C30},
    {5, 27, 362, 503, 42, 1, {.unusedValue = 0}, &Sub_4B5B70},
    {1, 22, 38, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &Sub_4B0F10},
    {3, 135, 46, 0, 0, 1, {.text = &gWindowText5735B8}, &Sub_4B0F10},
    {3, 330, 46, 0, 0, 1, {.text = &gWindowText5735C0}, &Sub_4B0F10},
    {1, 530, 46, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &Sub_4B5C30},
    {3, 100, 73, 0, 0, 1, {.text = &gWindowText5735B0}, &Sub_4B0F10},
    {3, 100, 89, 0, 0, 1, {.text = &gWindowText5735B0}, &Sub_4B0F10},
    {3, 100, 105, 0, 0, 1, {.text = &gWindowText5735B0}, &Sub_4B0F10},
    {1, 22, 114, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &Sub_4B0F10},
    {3, 135, 122, 0, 0, 1, {.text = &gWindowText5735B8}, &Sub_4B0F10},
    {3, 330, 122, 0, 0, 1, {.text = &gWindowText5735C0}, &Sub_4B0F10},
    {1, 530, 122, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &Sub_4B5C30},
    {3, 100, 150, 0, 0, 1, {.text = &gWindowText5735B0}, &Sub_4B0F10},
    {3, 100, 166, 0, 0, 1, {.text = &gWindowText5735B0}, &Sub_4B0F10},
    {3, 100, 182, 0, 0, 1, {.text = &gWindowText5735B0}, &Sub_4B0F10},
    {1, 22, 190, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &Sub_4B0F10},
    {3, 135, 198, 0, 0, 1, {.text = &gWindowText5735B8}, &Sub_4B0F10},
    {3, 330, 198, 0, 0, 1, {.text = &gWindowText5735C0}, &Sub_4B0F10},
    {1, 530, 198, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &Sub_4B5C30},
    {3, 100, 226, 0, 0, 1, {.text = &gWindowText5735B0}, &Sub_4B0F10},
    {3, 100, 242, 0, 0, 1, {.text = &gWindowText5735B0}, &Sub_4B0F10},
    {3, 100, 258, 0, 0, 1, {.text = &gWindowText5735B0}, &Sub_4B0F10},
    {1, 22, 266, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &Sub_4B0F10},
    {3, 135, 274, 0, 0, 1, {.text = &gWindowText5735B8}, &Sub_4B0F10},
    {3, 330, 274, 0, 0, 1, {.text = &gWindowText5735C0}, &Sub_4B0F10},
    {1, 530, 274, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &Sub_4B5C30},
    {3, 100, 302, 0, 0, 1, {.text = &gWindowText5735B0}, &Sub_4B0F10},
    {3, 100, 318, 0, 0, 1, {.text = &gWindowText5735B0}, &Sub_4B0F10},
    {3, 100, 334, 0, 0, 1, {.text = &gWindowText5735B0}, &Sub_4B0F10},
    {1, 538, 364, 49, 17, 1, {.graphic = &gWindowGraphic573550}, &Sub_4B5C30},
    {1, 538, 386, 49, 17, 1, {.graphic = &gWindowGraphic573568}, &Sub_4B5C30},
    {1, 576, 342, 17, 17, 1, {.graphic = &gWindowGraphic573580}, &Sub_4B5C30},
    {1, 536, 342, 17, 17, 1, {.graphic = &gWindowGraphic573598}, &Sub_4B5C30},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5734F0}, &Sub_4B0F10},
    {1, 15, 30, 0, 0, 1, {.graphic = &gWindowGraphic573508}, &Sub_4B0F10},
};

WindowElementPrototype gWindowElements571160[38] {
    {5, 0, 0, 567, 339, 1, {.unusedValue = 0}, &Sub_4A92D0},
    {1, 547, 9, 11, 11, 1, {.graphic = &gWindowGraphic571120}, &Sub_4AC200},
    {1, 476, 308, 66, 17, 1, {.graphic = &gWindowGraphic571030}, &Sub_4A9350},
    {1, 26, 307, 24, 19, 1, {.graphic = &gWindowGraphic571030}, &Sub_4A93E0},
    {1, 55, 307, 24, 19, 1, {.graphic = &gWindowGraphic571030}, &Sub_4A94A0},
    {1, 26, 307, 24, 19, 1, {.graphic = &gWindowGraphic571030}, &Sub_4A9560},
    {1, 55, 307, 24, 19, 1, {.graphic = &gWindowGraphic571030}, &Sub_4A9580},
    {1, 298, 264, 80, 15, 1, {.graphic = &gWindowGraphic571048}, &Sub_4A95A0},
    {1, 383, 264, 80, 15, 1, {.graphic = &gWindowGraphic571060}, &Sub_4A96D0},
    {1, 469, 264, 80, 15, 1, {.graphic = &gWindowGraphic571078}, &Sub_4A9820},
    {1, 26, 264, 80, 15, 1, {.graphic = &gWindowGraphic571048}, &Sub_4A95A0},
    {1, 111, 264, 80, 15, 1, {.graphic = &gWindowGraphic571060}, &Sub_4A96D0},
    {1, 197, 264, 80, 15, 1, {.graphic = &gWindowGraphic571078}, &Sub_4A9990},
    {3, 478, 328, 80, 15, 1, {.text = &gWindowText571148}, &Sub_4B0F10},
    {1, 297, 53, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &Sub_4AAEC0},
    {1, 297, 122, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &Sub_4AAEC0},
    {1, 297, 191, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &Sub_4AAEC0},
    {1, 25, 53, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &Sub_4AB710},
    {1, 25, 122, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &Sub_4AB710},
    {1, 25, 191, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &Sub_4AB710},
    {1, 538, 65, 0, 14, 1, {.graphic = &gWindowGraphic5710D8}, &Sub_4B0F10},
    {4, 538, 65, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD67880}, &Sub_4741A0},
    {1, 538, 55, 11, 11, 1, {.graphic = &gWindowGraphic5710F0}, &Sub_4AA9C0},
    {1, 538, 247, 11, 11, 1, {.graphic = &gWindowGraphic571108}, &Sub_4AAA70},
    {0, 286, 27, 273, 280, 1, {.unusedValue = 0}, &Sub_4AAB20},
    {1, 266, 65, 0, 14, 1, {.graphic = &gWindowGraphic5710D8}, &Sub_4B0F10},
    {4, 266, 65, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD6788C}, &Sub_4741A0},
    {1, 266, 55, 11, 11, 1, {.graphic = &gWindowGraphic5710F0}, &Sub_4AA9C0},
    {1, 266, 247, 11, 11, 1, {.graphic = &gWindowGraphic571108}, &Sub_4AAA70},
    {0, 14, 27, 273, 280, 1, {.unusedValue = 0}, &Sub_4AAB20},
    {0, 375, 135, 106, 85, 1, {.unusedValue = 0}, &Sub_4ABB20},
    {0, 103, 135, 106, 85, 1, {.unusedValue = 0}, &Sub_4ABB20},
    {7, 465, 115, 0, 0, 1, {.number = &gWindowNumber571150}, &Sub_4B0F10},
    {7, 193, 115, 0, 0, 1, {.number = &gWindowNumber571158}, &Sub_4B0F10},
    {1, 286, 27, 273, 280, 1, {.graphic = &gWindowGraphic571090}, &Sub_4A9D10},
    {1, 14, 27, 273, 280, 1, {.graphic = &gWindowGraphic5710A8}, &Sub_4AA3D0},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic571018}, &Sub_4A9B00},
    {0, 571, 7, 18, 78, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements571740[6] {
    {1, 547, 9, 11, 11, 1, {.graphic = &gWindowGraphic571728}, &Sub_4AC200},
    {1, 476, 308, 66, 17, 1, {.graphic = &gWindowGraphic5716E0}, &Sub_4A9350},
    {1, 286, 27, 273, 280, 1, {.graphic = &gWindowGraphic5716F8}, &Sub_4AC290},
    {1, 14, 27, 273, 280, 1, {.graphic = &gWindowGraphic571710}, &Sub_4AC890},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5716C8}, &Sub_4B0F10},
    {0, 571, 7, 18, 78, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements5719E8[26] {
    {0, 0, 0, 330, 110, 1, {.unusedValue = 0}, &Sub_4ADAA0},
    {1, 211, 8, 11, 11, 1, {.graphic = &gWindowGraphic5718A8}, &Sub_4C1FB0},
    {0, 57, 30, 150, 12, 1, {.unusedValue = 0}, &Sub_4AE260},
    {3, 57, 30, 0, 0, 1, {.text = &gWindowText571998}, &Sub_4B0F10},
    {5, 57, 30, 0, 0, 1, {.unusedValue = 0}, &Sub_4B0F10},
    {3, 17, 64, 0, 0, 1, {.text = &gWindowTexts5719A0[0]}, &Sub_4B0F10},
    {3, 17, 81, 0, 0, 1, {.text = &gWindowTexts5719A0[1]}, &Sub_4B0F10},
    {3, 17, 98, 0, 0, 1, {.text = &gWindowTexts5719A0[2]}, &Sub_4B0F10},
    {3, 17, 115, 0, 0, 1, {.text = &gWindowTexts5719A0[3]}, &Sub_4B0F10},
    {3, 17, 132, 0, 0, 1, {.text = &gWindowTexts5719A0[4]}, &Sub_4B0F10},
    {3, 17, 149, 0, 0, 1, {.text = &gWindowTexts5719A0[5]}, &Sub_4B0F10},
    {3, 17, 166, 0, 0, 1, {.text = &gWindowTexts5719A0[6]}, &Sub_4B0F10},
    {3, 17, 183, 0, 0, 1, {.text = &gWindowTexts5719A0[7]}, &Sub_4B0F10},
    {3, 17, 200, 0, 0, 1, {.text = &gWindowTexts5719A0[8]}, &Sub_4B0F10},
    {1, 212, 75, 0, 14, 1, {.graphic = &gWindowGraphic5718F0}, &Sub_4B0F10},
    {4, 212, 75, 11, 136, 1, {.mouseFlags = &gWindowMouseFlagsD67898}, &Sub_4741A0},
    {1, 212, 65, 11, 11, 1, {.graphic = &gWindowGraphic571908}, &Sub_4ADBD0},
    {1, 212, 210, 11, 11, 1, {.graphic = &gWindowGraphic571920}, &Sub_4ADC80},
    {1, 169, 225, 18, 18, 1, {.graphic = &gWindowGraphic571968}, &Sub_4ADD50},
    {1, 189, 225, 18, 18, 1, {.graphic = &gWindowGraphic571980}, &Sub_4ADE10},
    {0, 0, 0, 231, 305, 1, {.unusedValue = 0}, &Sub_4ADEF0},
    {1, 165, 43, 57, 17, 1, {.graphic = &gWindowGraphic571938}, &Sub_4ADF40},
    {1, 74, 237, 57, 17, 1, {.graphic = &gWindowGraphic571950}, &Sub_4AE170},
    {0, 15, 225, 48, 48, 1, {.unusedValue = 0}, &Sub_4AE350},
    {1, 0, 0, 267, 305, 1, {.graphic = &gWindowGraphic5718C0}, &Sub_4AE5B0},
    {1, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic5718D8}, &Sub_4B0F10},
};

WindowElementPrototype gWindowElements572AF8[20] {
    {5, 0, 0, 567, 339, 1, {.unusedValue = 0}, &Sub_4B09E0},
    {1, 547, 9, 11, 11, 1, {.graphic = &gWindowGraphic572958}, &Sub_4C1FB0},
    {1, 476, 309, 49, 17, 1, {.graphic = &gWindowGraphic572970}, &Sub_4B0A50},
    {1, 446, 309, 49, 17, 1, {.graphic = &gWindowGraphic572988}, &Sub_4B0DB0},
    {1, 502, 309, 49, 17, 1, {.graphic = &gWindowGraphic5729A0}, &Sub_4B0CF0},
    {0, 376, 136, 106, 85, 1, {.unusedValue = 0}, &Sub_4B0F10},
    {0, 541, 66, 0, 14, 1, {.unusedValue = 0}, &Sub_4B0F10},
    {4, 541, 66, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD678C4}, &Sub_4741A0},
    {1, 15, 28, 273, 280, 1, {.graphic = &gWindowGraphic572AD8}, &Sub_4B3430},
    {0, 541, 66, 0, 14, 1, {.unusedValue = 0}, &Sub_4B0F10},
    {4, 541, 66, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD678B8}, &Sub_4741A0},
    {1, 297, 28, 273, 280, 1, {.graphic = &gWindowGraphic572AD8}, &Sub_4B2480},
    {0, 541, 66, 0, 14, 1, {.unusedValue = 0}, &Sub_4B0F10},
    {4, 541, 66, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD678D0}, &Sub_4741A0},
    {1, 15, 28, 273, 280, 1, {.graphic = &gWindowGraphic572AD8}, &Sub_4B3580},
    {3, 133, 310, 0, 0, 1, {.text = &gWindowText572AF0}, &Sub_4B0F10},
    {3, 281, 310, 0, 0, 1, {.text = &gWindowText572AF0}, &Sub_4B0F10},
    {3, 317, 310, 0, 0, 1, {.text = &gWindowText572AF0}, &Sub_4B0F10},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic572940}, &Sub_4B0F10},
    {0, 571, 7, 18, 78, 1, {.unusedValue = 0}, &Sub_4B0F10},
};

// 71个原生指针；原36/37同源，不因窗口编号不同复制对象。
std::array<WindowPrototype*, kWindowCount> gWindowPrototypes57C130 {
    &gWindowPrototypeRecords[0], // 0
    &gWindowPrototypeRecords[1],
    &gWindowPrototypeRecords[2],
    &gWindowPrototypeRecords[3],
    &gWindowPrototypeRecords[4],
    &gWindowPrototypeRecords[5],
    &gWindowPrototypeRecords[6],
    &gWindowPrototypeRecords[7],
    &gWindowPrototypeRecords[8],
    &gWindowPrototypeRecords[9],
    &gWindowPrototypeRecords[10], // 10
    &gWindowPrototypeRecords[11],
    &gWindowPrototypeRecords[12],
    &gWindowPrototypeRecords[13],
    &gWindowPrototypeRecords[14],
    &gWindowPrototypeRecords[15],
    &gWindowPrototypeRecords[16],
    &gWindowPrototypeRecords[17],
    &gWindowPrototypeRecords[18],
    &gWindowPrototypeRecords[19],
    &gWindowPrototypeRecords[20], // 20
    &gWindowPrototypeRecords[21],
    &gWindowPrototypeRecords[22],
    &gWindowPrototypeRecords[23],
    &gWindowPrototypeRecords[24],
    &gWindowPrototypeRecords[25],
    &gWindowPrototypeRecords[26],
    &gWindowPrototypeRecords[27],
    &gWindowPrototypeRecords[28],
    &gWindowPrototypeRecords[29],
    &gWindowPrototypeRecords[30], // 30
    &gWindowPrototypeRecords[31],
    &gWindowPrototypeRecords[32],
    &gWindowPrototypeRecords[33],
    &gWindowPrototypeRecords[34],
    &gWindowPrototypeRecords[35],
    &gWindowPrototypeRecords[36],
    &gWindowPrototypeRecords[36], // 37
    &gWindowPrototypeRecords[37],
    &gWindowPrototypeRecords[38],
    &gWindowPrototypeRecords[39], // 40
    &gWindowPrototypeRecords[40],
    &gWindowPrototypeRecords[41],
    &gWindowPrototypeRecords[42],
    &gWindowPrototypeRecords[43],
    &gWindowPrototypeRecords[44],
    &gWindowPrototypeRecords[45],
    &gWindowPrototypeRecords[46],
    &gWindowPrototypeRecords[47],
    &gWindowPrototypeRecords[48],
    &gWindowPrototypeRecords[49], // 50
    &gWindowPrototypeRecords[50],
    &gWindowPrototypeRecords[51],
    &gWindowPrototypeRecords[52],
    &gWindowPrototypeRecords[53],
    &gWindowPrototypeRecords[54],
    &gWindowPrototypeRecords[55],
    &gWindowPrototypeRecords[56],
    &gWindowPrototypeRecords[57],
    &gWindowPrototypeRecords[58],
    &gWindowPrototypeRecords[59], // 60
    &gWindowPrototypeRecords[60],
    &gWindowPrototypeRecords[61],
    &gWindowPrototypeRecords[62],
    &gWindowPrototypeRecords[63],
    &gWindowPrototypeRecords[64],
    &gWindowPrototypeRecords[65],
    &gWindowPrototypeRecords[66],
    &gWindowPrototypeRecords[67],
    &gWindowPrototypeRecords[68],
    &gWindowPrototypeRecords[69], // 70
};

} // namespace cg::ui::detail
