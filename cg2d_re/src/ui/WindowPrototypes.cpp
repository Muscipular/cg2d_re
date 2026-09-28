#include "WindowPrototypeDependencies.hpp"

#include "cg/Input.hpp"

namespace cg::ui::detail {
namespace {

// 原56CCC8的四字节元数据和593850借用，不创建另一份默认文本。
WindowTextPrototype gWindowText56CCC8 {{}, input::LegacyText593850()};

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

WindowElementPrototype gWindowElements56CCD0[1] {
    {3, 0, 44, 0, 0, 1, {.text = &gWindowText56CCC8}, &Sub_4B0F10},
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
