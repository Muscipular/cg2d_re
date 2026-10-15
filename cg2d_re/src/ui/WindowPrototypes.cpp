#include "WindowPrototypeDependencies.hpp"
#include "cg/text/Encoding.hpp"
#include <string>

#include "../world/WorldUpdateDependencies.hpp"
#include "cg/Input.hpp"

namespace cg::ui::detail {

namespace {

WindowGraphicData gWindowGraphic55EC58 {0x18229, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EC58。
WindowGraphicData gWindowGraphic55EC70 {0x3B90E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EC70。
WindowGraphicData gWindowGraphic55EC88 {0x3B911, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EC88。
WindowGraphicData gWindowGraphic55ECA0 {0x3B914, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ECA0。
WindowGraphicData gWindowGraphic55ECB8 {0x3B917, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ECB8。
WindowGraphicData gWindowGraphic55ECD0 {0x3B91A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ECD0。
WindowGraphicData gWindowGraphic55ECE8 {0x3B91D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ECE8。
WindowGraphicData gWindowGraphic55ED00 {0x3B920, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ED00。
WindowGraphicData gWindowGraphic55ED18 {0x3B923, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ED18。
WindowGraphicData gWindowGraphic55ED30 {0x18223, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ED30。
WindowGraphicData gWindowGraphic55ED48 {0x3B90D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055ED48。
std::uint32_t gWindowMouseFlagsD67620 = 0; // 原全局：0x00D67620。

WindowGraphicData gWindowGraphic55EF00 {0x3B92B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF00。
WindowGraphicData gWindowGraphic55EF18 {0x3B92F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF18。
WindowGraphicData gWindowGraphic55EF30 {0x3B932, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF30。
WindowGraphicData gWindowGraphic55EF48 {0x3B935, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF48。
WindowGraphicData gWindowGraphic55EF60 {0x3B938, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF60。
WindowGraphicData gWindowGraphic55EF78 {0x3B93B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF78。
WindowGraphicData gWindowGraphic55EF90 {0x3B93E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EF90。
WindowGraphicData gWindowGraphic55EFA8 {0x3B941, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EFA8。
WindowGraphicData gWindowGraphic55EFC0 {0x3B950, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EFC0。
WindowGraphicData gWindowGraphic55EFD8 {0x3B954, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055EFD8。
WindowGraphicData gWindowGraphic55EFF0 {0x3B92D, 0, 3, 60, 15, 0xFFFFFFFFu}; // 原全局：0x0055EFF0。
WindowGraphicData gWindowGraphic55F008 {0x3B664, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F008。
WindowGraphicData gWindowGraphic55F020 {0x3B664, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F020。
WindowGraphicData gWindowGraphic55F038 {0x3B664, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F038。
WindowGraphicData gWindowGraphic55F050 {0x3B92A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F050。
WindowGraphicData gWindowGraphic55F068 {0x3B92C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F068。
WindowNumberData gWindowNumber55F090 {&gStatusHpTextD40B18, 10, 0, 0, 0}; // 原全局：0x0055F090。
WindowNumberData gWindowNumber55F098 {&gStatusFpTextD45D50, 10, 0, 0, 0}; // 原全局：0x0055F098。
std::uint32_t gWindowMouseFlagsD67650 = 0; // 原全局：0x00D67650。

WindowGraphicData gWindowGraphic56AEF0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056AEF0。
WindowGraphicData gWindowGraphic56AF08 {0x3B97A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056AF08。
WindowGraphicData gWindowGraphic56AF20 {0x3B97B, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0056AF20。
WindowTextPrototype gWindowText56AF38 { // 原全局：0x0056AF38。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, cg::text::FromGbkWide(input::LegacyText593850()),
};
WindowTextPrototype gWindowText56AF40 { // 原全局：0x0056AF40。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, cg::text::FromGbkWide(input::LegacyText593850()),
};
WindowTextPrototype gWindowText56AF48 { // 原全局：0x0056AF48。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, cg::text::FromGbkWide(input::LegacyText593850()),
};

WindowGraphicData gWindowGraphic56B0D8 {0x3B985, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B0D8。
WindowGraphicData gWindowGraphic56B0F0 {0x3B988, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B0F0。
WindowGraphicData gWindowGraphic56B108 {0x3B98B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B108。
WindowGraphicData gWindowGraphic56B120 {0x3B98E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B120。
WindowGraphicData gWindowGraphic56B138 {0x3B991, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B138。
WindowGraphicData gWindowGraphic56B150 {0x3B994, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B150。
WindowGraphicData gWindowGraphic56B168 {0x3B997, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B168。
WindowGraphicData gWindowGraphic56B180 {0x3B99A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B180。
WindowGraphicData gWindowGraphic56B198 {0x3B99D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B198。
WindowGraphicData gWindowGraphic56B1B0 {0x3B9A0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B1B0。
WindowGraphicData gWindowGraphic56B1C8 {0x3B9A3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B1C8。
WindowGraphicData gWindowGraphic56B1E0 {0x3B9A6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B1E0。
WindowGraphicData gWindowGraphic56B1F8 {0x3B9A9, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B1F8。
WindowGraphicData gWindowGraphic56B210 {0x3B9AC, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B210。
WindowGraphicData gWindowGraphic56B228 {0x3B9AF, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B228。
WindowGraphicData gWindowGraphic56B240 {0x3B9B2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B240。
WindowGraphicData gWindowGraphic56B258 {0x3B9B5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B258。
WindowGraphicData gWindowGraphic56B270 {0x3B9B8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B270。
WindowGraphicData gWindowGraphic56B288 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B288。
WindowGraphicData gWindowGraphic56B2A0 {0x3B984, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B2A0。

WindowGraphicData gWindowGraphic570B10 {245167, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570B10。
WindowGraphicData gWindowGraphic570B28 {245160, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570B28。
WindowGraphicData gWindowGraphic570B40 {245187, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570B40。
WindowGraphicData gWindowGraphic570B58 {245172, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570B58。
WindowGraphicData gWindowGraphic570B70 {245206, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570B70。
WindowGraphicData gWindowGraphic570B88 {245192, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570B88。
WindowGraphicData gWindowGraphic570BA0 {245161, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570BA0。
WindowGraphicData gWindowGraphic570BD0 {245171, 0, 0, 0, 0, 0x30FFFFFFu}; // 原全局：0x00570BD0。
WindowGraphicData gWindowGraphic570BE8 {245169, 0, 0, 0, 0, 0x30FFFFFFu}; // 原全局：0x00570BE8。
WindowGraphicData gWindowGraphic570C00 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570C00。
WindowGraphicData gWindowGraphic570C18 {245165, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570C18。
WindowGraphicData gWindowGraphic570C30 {245164, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570C30。
WindowGraphicData gWindowGraphic570C48 {245166, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570C48。
WindowGraphicData gWindowGraphic570C60 {245212, 0, 0, 0, 0, 0xA0FFFFFFu}; // 原全局：0x00570C60。
WindowGraphicData gWindowGraphic570C78 {243091, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570C78。
WindowGraphicData gWindowGraphic570C90 {243094, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570C90。
std::uint32_t gWindowMouseFlagsD67870 = 0; // 原全局：0x00D67870。

WindowGraphicData gWindowGraphic57A310 {243001, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A310。
WindowGraphicData gWindowGraphic57A328 {36813, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A328。
WindowGraphicData gWindowGraphic57A340 {244481, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0057A340。
WindowGraphicData gWindowGraphic57A358 {244483, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A358。
WindowGraphicData gWindowGraphic57A370 {243380, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A370。
WindowGraphicData gWindowGraphic57A388 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A388。
WindowGraphicData gWindowGraphic57A3A0 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A3A0。
WindowGraphicData gWindowGraphic57A3B8 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A3B8。
WindowGraphicData gWindowGraphic57A3D0 {244485, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A3D0。
WindowGraphicData gWindowGraphic57A3E8 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A3E8。
WindowGraphicData gWindowGraphic57A400 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A400。
WindowGraphicData gWindowGraphic57A418 {36806, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A418。
WindowGraphicData gWindowGraphic57A430 {36809, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A430。
WindowTextPrototype gWindowText57A448 { // 原全局：0x0057A448。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"???",
};
std::uint32_t gWindowMouseFlagsD679B8 = 0; // 原全局：0x00D679B8。

WindowGraphicData gWindowGraphic57ADD8 {243001, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057ADD8。
WindowGraphicData gWindowGraphic57ADF0 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057ADF0。
WindowGraphicData gWindowGraphic57AE08 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057AE08。
WindowGraphicData gWindowGraphic57AE20 {36814, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057AE20。
WindowGraphicData gWindowGraphic57AE38 {36804, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057AE38。
WindowGraphicData gWindowGraphic57AE50 {36805, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x0057AE50。
WindowGraphicData gWindowGraphic57AE68 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057AE68。
WindowGraphicData gWindowGraphic57AE80 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057AE80。
WindowGraphicData gWindowGraphic57AE98 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057AE98。
WindowTextPrototype gWindowText57AEB0 { // 原全局：0x0057AEB0。
    {std::byte{1}, std::byte{12}, std::byte{0}, std::byte{0}}, cg::text::FromLegacyDisplayWide(world::detail::gLogFileName554648),
};
WindowTextPrototype gWindowText57AEB8 { // 原全局：0x0057AEB8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, cg::text::FromGbkWide(input::LegacyText593850()),
};
std::uint32_t gWindowMouseFlagsD679C8 = 0; // 原全局：0x00D679C8。

WindowGraphicData gWindowGraphic55F4C0 {0x3B95F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F4C0。
WindowGraphicData gWindowGraphic55F4D8 {0x3B962, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F4D8。
WindowGraphicData gWindowGraphic55F4F0 {0x3B965, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F4F0。
WindowGraphicData gWindowGraphic55F508 {0x3B968, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F508。
WindowGraphicData gWindowGraphic55F520 {0x3B96B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F520。
WindowGraphicData gWindowGraphic55F538 {0x3B96E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F538。
WindowGraphicData gWindowGraphic55F550 {0x3B971, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F550。
WindowGraphicData gWindowGraphic55F568 {0x18665, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F568。
WindowGraphicData gWindowGraphic55F580 {0x3B95C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F580。
WindowGraphicData gWindowGraphic55F598 {0x3B95D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F598。
WindowTextPrototype gWindowText55F5B0 { // 原全局：0x0055F5B0。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}},
    L"相關資訊",
};
WindowTextPrototype gWindowText55F5B8 { // 原全局：0x0055F5B8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"--------",
};

// 原56CCC8的四字节元数据和593850借用，不创建另一份默认文本。
WindowTextPrototype gWindowText56CCC8 {{}, cg::text::FromGbkWide(input::LegacyText593850())}; // 原全局：0x0056CCC8。

WindowGraphicData gWindowGraphic561A40 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561A40。
WindowGraphicData gWindowGraphic561A58 {0x3BA24, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561A58。
WindowGraphicData gWindowGraphic561A70 {0x3BA27, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00561A70。
WindowGraphicData gWindowGraphic561A88 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561A88。
WindowGraphicData gWindowGraphic561AA0 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561AA0。
WindowGraphicData gWindowGraphic561AB8 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561AB8。
WindowGraphicData gWindowGraphic561AD0 {0x3B571, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561AD0。
WindowGraphicData gWindowGraphic561AE8 {0x3B57A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561AE8。
WindowGraphicData gWindowGraphic561B00 {0x3B57A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561B00。
WindowGraphicData gWindowGraphic561B18 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561B18。
WindowGraphicData gWindowGraphic561B30 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561B30。
WindowTextPrototype gWindowText561B48 { // 原全局：0x00561B48。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"無職", // 原常量参考：0x005860A4。
};
WindowTextPrototype gWindowText561B50 { // 原全局：0x00561B50。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"10/10", // 原常量参考：0x0058609C。
};
WindowTextPrototype gWindowText561B58 { // 原全局：0x00561B58。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"經驗值", // 原常量参考：0x005861E0。
};
WindowTextPrototype gWindowText561B60 { // 原全局：0x00561B60。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等級", // 原常量参考：0x00586228。
};
WindowTextPrototype gWindowText561B68 { // 原全局：0x00561B68。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"技能", // 原常量参考：0x00586094。
};
WindowTextPrototype gWindowText561B70 { // 原全局：0x00561B70。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔力", // 原常量参考：0x005861C0。
};
std::uint32_t gWindowMouseFlagsD67780 = 0; // 原全局：0x00D67780。

WindowGraphicData gWindowGraphic56D8B0 {0x3B612, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D8B0。
WindowGraphicData gWindowGraphic56D8E0 {0x3B60C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D8E0。
WindowGraphicData gWindowGraphic56D940 {0x3B544, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D940。
WindowGraphicData gWindowGraphic56D958 {0x3B547, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D958。
WindowGraphicData gWindowGraphic56D970 {0x3B541, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D970。
WindowGraphicData gWindowGraphic56D988 {0x3B54A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D988。
WindowGraphicData gWindowGraphic56D9A0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D9A0。
WindowGraphicData gWindowGraphic56D9B8 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D9B8。
WindowGraphicData gWindowGraphic56D9D0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D9D0。
WindowGraphicData gWindowGraphic56D9E8 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D9E8。
WindowGraphicData gWindowGraphic56DA00 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA00。
WindowGraphicData gWindowGraphic56DA18 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA18。
WindowGraphicData gWindowGraphic56DA30 {0x3B57D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA30。
WindowGraphicData gWindowGraphic56DA48 {0x3B580, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA48。
WindowGraphicData gWindowGraphic56DA60 {0x3B9E0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA60。
WindowGraphicData gWindowGraphic56DA78 {0x3B5C6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA78。
WindowGraphicData gWindowGraphic56DA90 {0x3B9E5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DA90。
WindowGraphicData gWindowGraphic56DAA8 {0x3B9DE, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DAA8。
WindowGraphicData gWindowGraphic56DAC0 {0x3B9DF, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0056DAC0。
WindowGraphicData gWindowGraphic56DAD8 {0x3C115, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DAD8。
WindowGraphicData gWindowGraphic56DAF0 {0x3C114, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056DAF0。
WindowTextPrototype gWindowText56DB08 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"name"}; // 原全局：0x0056DB08。 // 原常量参考：0x00585F14。
WindowTextPrototype gWindowText56DB10 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"level"}; // 原全局：0x0056DB10。 // 原常量参考：0x00585F0C。
WindowTextPrototype gWindowText56DB18 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"guild"}; // 原全局：0x0056DB18。 // 原常量参考：0x00585F04。
WindowTextPrototype gWindowText56DB20 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"title"}; // 原全局：0x0056DB20。 // 原常量参考：0x00585EFC。
WindowTextPrototype gWindowText56DB28 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"server"}; // 原全局：0x0056DB28。 // 原常量参考：0x00585EF4。
WindowTextPrototype gWindowText56DB30 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"guildserver"}; // 原全局：0x0056DB30。 // 原常量参考：0x00585EE8。
std::uint32_t gWindowMouseFlagsD67840 = 0; // 原全局：0x00D67840。

WindowGraphicData gWindowGraphic56F7A8 {0x3B59C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F7A8。
WindowGraphicData gWindowGraphic56F7C0 {0x3B562, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F7C0。
WindowGraphicData gWindowGraphic56F7D8 {0x3B56E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F7D8。
WindowGraphicData gWindowGraphic56F7F0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F7F0。
WindowGraphicData gWindowGraphic56F808 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F808。
WindowGraphicData gWindowGraphic56F820 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F820。
WindowGraphicData gWindowGraphic56F838 {0x3BAA3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056F838。
WindowTextPrototype gWindowText56F850 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族稱號"}; // 原全局：0x0056F850。 // 原常量参考：0x00585ED8。
WindowTextPrototype gWindowText56F858 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"成員名稱"}; // 原全局：0x0056F858。 // 原常量参考：0x00585ECC。
std::uint32_t gWindowMouseFlagsD67860 = 0; // 原全局：0x00D67860。

WindowGraphicData gWindowGraphic56FCA8 {0x3BA1B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056FCA8。
WindowGraphicData gWindowGraphic56FCC0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056FCC0。
WindowGraphicData gWindowGraphic56FCD8 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056FCD8。
WindowGraphicData gWindowGraphic56FCF0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056FCF0。
WindowGraphicData gWindowGraphic56FD08 {0x3BA0C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056FD08。
WindowGraphicData gWindowGraphic56FD20 {0x3BA06, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056FD20。
WindowGraphicData gWindowGraphic56FD38 {0x3BA07, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0056FD38。
WindowTextPrototype gWindowText56FD50 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"內容"}; // 原全局：0x0056FD50。 // 原常量参考：0x00585EC4。
WindowTextPrototype gWindowText56FD58 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"頁數"}; // 原全局：0x0056FD58。 // 原常量参考：0x00585EBC。
WindowTextPrototype gWindowText56FD60 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名稱"}; // 原全局：0x0056FD60。 // 原常量参考：0x00586230。
WindowTextPrototype gWindowText56FD68 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"00/00 00:00"}; // 原全局：0x0056FD68。 // 原常量参考：0x00585EB0。

WindowGraphicData gWindowGraphic5700B8 {0x3BA0B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005700B8。
WindowGraphicData gWindowGraphic5700D0 {0x3BA06, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005700D0。
WindowGraphicData gWindowGraphic5700E8 {0x3BA07, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005700E8。
WindowGraphicData gWindowGraphic570148 {0x3BA08, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570148。
WindowGraphicData gWindowGraphic570160 {0x3BA1B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570160。
WindowGraphicData gWindowGraphic570190 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570190。
WindowTextPrototype gWindowText5701F0 {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"測試人員"}; // 原全局：0x005701F0。 // 原常量参考：0x00585EA4。

WindowGraphicData gWindowGraphic570118 {0x3BA0D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570118。
WindowGraphicData gWindowGraphic570130 {0x3BA0E, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00570130。
WindowGraphicData gWindowGraphic570178 {0x3BE54, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570178。
WindowGraphicData gWindowGraphic5701A8 {0x3B5C3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005701A8。
WindowGraphicData gWindowGraphic5701C0 {0x3BE5D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005701C0。
WindowGraphicData gWindowGraphic5701D8 {0x3BE5A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005701D8。

WindowGraphicData gWindowGraphic570560 {0x3BE34, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x00570560。
WindowGraphicData gWindowGraphic570578 {0x3BE35, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x00570578。
WindowGraphicData gWindowGraphic570590 {0x3BE36, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x00570590。
WindowGraphicData gWindowGraphic5705A8 {0x3BE37, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x005705A8。
WindowGraphicData gWindowGraphic5705C0 {0x3BE38, 0, 0, 74, 54, 0x80FFFFFFu}; // 原全局：0x005705C0。
WindowGraphicData gWindowGraphic5705D8 {0x3BE39, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x005705D8。
WindowGraphicData gWindowGraphic5705F0 {0x3BE3A, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x005705F0。
WindowGraphicData gWindowGraphic570608 {0x3BE3B, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x00570608。
WindowGraphicData gWindowGraphic570620 {0x3BE3C, 0, 0, 74, 54, 0xFFFFFFFFu}; // 原全局：0x00570620。
WindowGraphicData gWindowGraphic570638 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570638。
WindowGraphicData gWindowGraphic570650 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570650。
WindowGraphicData gWindowGraphic570668 {0x3BE4B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570668。
WindowGraphicData gWindowGraphic570680 {0x3BE4E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570680。
WindowGraphicData gWindowGraphic570698 {0x3BE51, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00570698。
WindowGraphicData gWindowGraphic5706B0 {0x3BE54, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005706B0。
WindowTextPrototype gWindowText5706C8 {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"content"}; // 原全局：0x005706C8。 // 原常量参考：0x00585E9C。

WindowGraphicData gWindowGraphic56B5E0 {0x3B9C0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B5E0。
WindowGraphicData gWindowGraphic56B5F8 {0x3B9C5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B5F8。
WindowGraphicData gWindowGraphic56B610 {0x3B9D1, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B610。
WindowGraphicData gWindowGraphic56B628 {0x3B9C8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B628。
WindowGraphicData gWindowGraphic56B640 {0x3B9D4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B640。
WindowGraphicData gWindowGraphic56B658 {0x3B9CB, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B658。
WindowGraphicData gWindowGraphic56B670 {0x3B9D7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B670。
WindowGraphicData gWindowGraphic56B688 {0x3B9DA, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B688。
WindowGraphicData gWindowGraphic56B6A0 {0x3B9CE, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B6A0。
WindowGraphicData gWindowGraphic56C798 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C798。
WindowGraphicData gWindowGraphic56C7B0 {0x3BA74, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C7B0。
WindowGraphicData gWindowGraphic56C7C8 {0x3BA75, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0056C7C8。
WindowGraphicData gWindowGraphic56C7E0 {0x3BA76, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C7E0。
WindowTextPrototype gWindowText56B8D0 {{}, L"觀戰結束"}; // 原全局：0x0056B8D0。 // 原常量参考：0x00554184。
WindowGraphicData gWindowGraphic56B918 {0x3BA7E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B918。
WindowGraphicData gWindowGraphic56B930 {0x3BA7F, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x0056B930。
WindowGraphicData gWindowGraphic56B960 {0x3B614, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B960。
WindowGraphicData gWindowGraphic56B978 {0x3BA80, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056B978。
WindowTextPrototype gWindowText56B990 { // 原全局：0x0056B990。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？？？？？", // 原常量参考：0x00585FA0。
};
WindowTextPrototype gWindowText56B998 { // 原全局：0x0056B998。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"+000000", // 原常量参考：0x00585F98。
};
WindowTextPrototype gWindowText56B9A0 {{}, L"？？？？？？？？"}; // 原全局：0x0056B9A0。 // 原常量参考：0x00585FA0。
WindowTextPrototype gWindowText56B9A8 { // 原全局：0x0056B9A8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？？？？？", // 原常量参考：0x00585FA0。
};
WindowTextPrototype gWindowText56B9B0 { // 原全局：0x0056B9B0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"+000", // 原常量参考：0x00585F90。
};
WindowGraphicData gWindowGraphic56C2A0 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C2A0。
WindowTextPrototype gWindowText56C2B8 { // 原全局：0x0056C2B8。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"結果", // 原常量参考：0x00585F88。
};
WindowTextPrototype gWindowText56C2C0 { // 原全局：0x0056C2C0。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"?????  +000000", // 原常量参考：0x00585F78。
};
WindowTextPrototype gWindowText56C9A8 {{}, L"偷襲"}; // 原全局：0x0056C9A8。 // 原常量参考：0x00585F70。
WindowTextPrototype gWindowText56C9B0 {{}, L"被偷襲"}; // 原全局：0x0056C9B0。 // 原常量参考：0x00585F68。
WindowTextPrototype gWindowText56B6F8 { // 原全局：0x0056B6F8。
    {std::byte{0x7F}, std::byte{1}, std::byte{0}, std::byte{0}}, L"名稱", // 原常量参考：0x00586230。
};
WindowTextPrototype gWindowText56B700 { // 原全局：0x0056B700。
    {std::byte{0x7F}, std::byte{1}, std::byte{0}, std::byte{0}}, L"魔力", // 原常量参考：0x005861C0。
};
WindowTextPrototype gWindowText56B708 { // 原全局：0x0056B708。
    {std::byte{0x7F}, std::byte{1}, std::byte{0}, std::byte{0}}, L"技能", // 原常量参考：0x00586094。
};

WindowGraphicData gWindowGraphic55F780 {0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F780。
WindowGraphicData gWindowGraphic565608 {0x3BAEC, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565608。
WindowGraphicData gWindowGraphic565620 {0x3BAED, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00565620。
WindowGraphicData gWindowGraphic565638 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565638。
WindowGraphicData gWindowGraphic565650 {0x3B56E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565650。
WindowGraphicData gWindowGraphic565698 {0x3BAEE, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565698。
WindowGraphicData gWindowGraphic5656B0 {0x3BAF5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005656B0。
WindowGraphicData gWindowGraphic5656C8 {0x3B584, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005656C8。
WindowGraphicData gWindowGraphic5656E0 {0x3BAF2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005656E0。
WindowGraphicData gWindowGraphic565878 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565878。
WindowGraphicData gWindowGraphic565890 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565890。
WindowGraphicData gWindowGraphic5658A8 {0x3B5B7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005658A8。
WindowGraphicData gWindowGraphic5658C0 {0x3BAF7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005658C0。
WindowGraphicData gWindowGraphic565908 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565908。
WindowGraphicData gWindowGraphic565920 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565920。
WindowTextPrototype gWindowText565950 { // 原全局：0x00565950。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名稱", // 原常量参考：0x00586230。
};
WindowTextPrototype gWindowText565988 { // 原全局：0x00565988。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"1/5", // 原常量参考：0x0058600C。
};
WindowTextPrototype gWindowText565990 { // 原全局：0x00565990。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"技能", // 原常量参考：0x00586004。
};
WindowTextPrototype gWindowText5659A0 { // 原全局：0x005659A0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"進行攻擊", // 原常量参考：0x00585FF4。
};
WindowNumberData gWindowNumbers5667C0[10] { // 原全局：0x005667C0。
    {&gPetTechniqueCostsD66C34[0], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[1], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[2], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[3], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[4], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[5], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[6], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[7], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[8], 0, 0, 0, 0},
    {&gPetTechniqueCostsD66C34[9], 0, 0, 0, 0},
};

WindowGraphicData gWindowGraphic563388 {0x3B5D6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00563388。
WindowGraphicData gWindowGraphic565668 {0x3BAEF, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565668。
WindowGraphicData gWindowGraphic565938 {0x3B614, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565938。
WindowTextPrototype gWindowText565958 { // 原全局：0x00565958。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"10000000/10000000", // 原常量参考：0x00586020。
};
WindowTextPrototype gWindowText565960 { // 原全局：0x00565960。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"100000/100000", // 原常量参考：0x00586034。
};
WindowTextPrototype gWindowText565968 { // 原全局：0x00565968。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"100000/100000", // 原常量参考：0x00586034。
};
WindowTextPrototype gWindowText565970 { // 原全局：0x00565970。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"種類", // 原常量参考：0x00586018。
};
WindowTextPrototype gWindowText565978 { // 原全局：0x00565978。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"999", // 原常量参考：0x00586014。
};
WindowTextPrototype gWindowText565980 { // 原全局：0x00565980。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"99", // 原常量参考：0x00586010。
};
WindowTextPrototype gWindowText5659A8 { // 原全局：0x005659A8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"99", // 原常量参考：0x00586010。
};
WindowTextPrototype gWindowText5659B0 { // 原全局：0x005659B0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"0/0", // 原常量参考：0x00585FF0。
};
std::uint32_t gWindowAnimationImage5659B8 = 100000; // 原全局：0x005659B8。
std::uint32_t gWindowMouseFlagsD677B0 = 0; // 原全局：0x00D677B0。
WindowGraphicData gWindowGraphic565680 {0x3BAF0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00565680。
WindowGraphicData gWindowGraphic5658D8 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005658D8。
WindowGraphicData gWindowGraphic566F18 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566F18。
WindowGraphicData gWindowGraphic566F30 {0x3BB00, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566F30。
WindowGraphicData gWindowGraphic566F48 {0x3BB01, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00566F48。
WindowGraphicData gWindowGraphic566F60 {0x3BB03, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566F60。
WindowGraphicData gWindowGraphic566F78 {0x3B6B4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566F78。
WindowGraphicData gWindowGraphic566F90 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566F90。
WindowGraphicData gWindowGraphic566FA8 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566FA8。
WindowGraphicData gWindowGraphic566FC0 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566FC0。
WindowGraphicData gWindowGraphic566FD8 {0x3BB05, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566FD8。
WindowGraphicData gWindowGraphic566FF0 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00566FF0。
WindowGraphicData gWindowGraphic567008 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567008。
WindowTextPrototype gWindowText567020 { // 原全局：0x00567020。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"寵物名稱", // 原常量参考：0x00586048。
};
std::uint32_t gWindowMouseFlagsD677C0 = 0; // 原全局：0x00D677C0。
WindowGraphicData gWindowGraphic5679B0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005679B0。
WindowGraphicData gWindowGraphic5679C8 {0x3BB0A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005679C8。
WindowGraphicData gWindowGraphic5679E0 {0x3BB0B, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005679E0。
WindowGraphicData gWindowGraphic5679F8 {0x3BB0B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005679F8。
WindowGraphicData gWindowGraphic567A10 {0x3BB0C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A10。
WindowGraphicData gWindowGraphic567A40 {0x3B55C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A40。
WindowGraphicData gWindowGraphic567A58 {0x3BE5D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A58。
WindowGraphicData gWindowGraphic567A70 {0x3BE5A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A70。
WindowGraphicData gWindowGraphic567A88 {0x3B55F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567A88。
WindowGraphicData gWindowGraphic567AA0 {0x3BAC7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00567AA0。
WindowTextPrototype gWindowText567AB8 { // 原全局：0x00567AB8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"寵物名稱", // 原常量参考：0x00586048。
};
WindowTextPrototype gWindowText567AC0 { // 原全局：0x00567AC0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"000", // 原常量参考：0x00585FEC。
};
WindowTextPrototype gWindowText567AC8 { // 原全局：0x00567AC8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"00", // 原常量参考：0x00585FE8。
};
WindowTextPrototype gWindowText567AD0 { // 原全局：0x00567AD0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"種類", // 原常量参考：0x00586018。
};
WindowTextPrototype gWindowText567AD8 { // 原全局：0x00567AD8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"255/255", // 原常量参考：0x00585FE0。
};
std::uint32_t gWindowAnimationImage567AE0 = 100000; // 原全局：0x00567AE0。
std::uint32_t gWindowMouseFlagsD677D0 = 0; // 原全局：0x00D677D0。
WindowGraphicData gWindowGraphic568720 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568720。
WindowGraphicData gWindowGraphic568738 {0x3B8BE, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568738。
WindowGraphicData gWindowGraphic568750 {0x3B8C1, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568750。
WindowGraphicData gWindowGraphic568768 {0x3B8C4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568768。
WindowGraphicData gWindowGraphic568780 {0x3B8C7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568780。
WindowGraphicData gWindowGraphic568798 {0x3B8CA, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568798。
WindowGraphicData gWindowGraphic5687B0 {0x3B8D0, 0, 0, 0, 79, 0xFFFFFFFFu}; // 原全局：0x005687B0。
WindowGraphicData gWindowGraphic5687C8 {0x3B8DA, 0, 0, 0, 128, 0xFFFFFFFFu}; // 原全局：0x005687C8。
WindowGraphicData gWindowGraphic5687E0 {0x3B8EE, 0, 0, 0, 203, 0xFFFFFFFFu}; // 原全局：0x005687E0。
WindowGraphicData gWindowGraphic5687F8 {0x3B8F8, 0, 0, 0, 90, 0xFFFFFFFFu}; // 原全局：0x005687F8。
WindowGraphicData gWindowGraphic568810 {0x3B8D1, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568810。
WindowGraphicData gWindowGraphic568828 {0x3B8D4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568828。
WindowGraphicData gWindowGraphic568840 {0x3B8DB, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568840。
WindowGraphicData gWindowGraphic568858 {0x3B8E1, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568858。
WindowGraphicData gWindowGraphic568870 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568870。
WindowGraphicData gWindowGraphic568888 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568888。
WindowGraphicData gWindowGraphic5688A0 {0x3B8EF, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005688A0。
WindowGraphicData gWindowGraphic5688B8 {0x3B8F9, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005688B8。
WindowGraphicData gWindowGraphic5688D0 {0x3B8BC, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005688D0。
WindowGraphicData gWindowGraphic5688E8 {0x3B8BD, 0, 0, 0, 0, 0x99FFFFFFu}; // 原全局：0x005688E8。
WindowGraphicData gWindowGraphic568900 {0x3BE63, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568900。
WindowGraphicData gWindowGraphic568918 {0x3BE78, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568918。
WindowGraphicData gWindowGraphic568930 {0x3B5E2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568930。
WindowGraphicData gWindowGraphic568948 {0x3B5E5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568948。
WindowGraphicData gWindowGraphic568960 {0x3B5E8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568960。
WindowGraphicData gWindowGraphic568978 {0x3B5EB, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568978。
WindowGraphicData gWindowGraphic568990 {0x3B5EE, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568990。
WindowNumberData gWindowNumber5689B8 {&gSettingsEffectTextD2EA64, 0, 3, 0, 0}; // 原全局：0x005689B8。
WindowNumberData gWindowNumber5689C0 {&gSettingsMusicTextD3041C, 0, 3, 0, 0}; // 原全局：0x005689C0。
WindowGraphicData gWindowGraphic568E70 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568E70。
WindowGraphicData gWindowGraphic568E88 {0x3B902, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568E88。
WindowGraphicData gWindowGraphic568EA0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568EA0。
WindowGraphicData gWindowGraphic568EB8 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568EB8。
WindowGraphicData gWindowGraphic568ED0 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568ED0。
WindowGraphicData gWindowGraphic568EE8 {0x3B903, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568EE8。
WindowGraphicData gWindowGraphic568F00 {0x3B906, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568F00。
WindowGraphicData gWindowGraphic568F18 {0x3B909, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568F18。
WindowGraphicData gWindowGraphic568F30 {0x3B59C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568F30。
WindowGraphicData gWindowGraphic568F48 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568F48。
WindowGraphicData gWindowGraphic568F60 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00568F60。
std::uint32_t gWindowMouseFlagsD677F0 = 0; // 原全局：0x00D677F0。

WindowGraphicData gWindowGraphic55F798 {0x3B614, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F798。
WindowGraphicData gWindowGraphic55F7B0 {0x3BAC5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F7B0。
WindowGraphicData gWindowGraphic55F7C8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F7C8。
WindowGraphicData gWindowGraphic55F7E0 {0x3BAB6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F7E0。
WindowGraphicData gWindowGraphic55F7F8 {0x3B583, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F7F8。
WindowGraphicData gWindowGraphic55F810 {0x3BAB8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F810。
WindowGraphicData gWindowGraphic55F828 {0x3BAC0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F828。
WindowGraphicData gWindowGraphic55F840 {0x3BABD, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F840。
WindowGraphicData gWindowGraphic55F858 {0x3BABB, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F858。
WindowGraphicData gWindowGraphic55F870 {0x3BAC7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F870。
WindowGraphicData gWindowGraphic55F960 {0x3BACC, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055F960。
WindowGraphicData gWindowGraphic55FA50 {0x3BAD1, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FA50。
WindowGraphicData gWindowGraphic55FB40 {0x3BAD6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FB40。
WindowGraphicData gWindowGraphic55FC30 {0x3BAC6, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC30。
WindowGraphicData gWindowGraphic55FC48 {0x3BACB, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC48。
WindowGraphicData gWindowGraphic55FC60 {0x3BAD0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC60。
WindowGraphicData gWindowGraphic55FC78 {0x3BAD5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC78。
WindowGraphicData gWindowGraphic55FC90 {0x3BAB2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FC90。
WindowGraphicData gWindowGraphic55FCA8 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FCA8。
WindowGraphicData gWindowGraphic55FCC0 {0x3BAB1, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x0055FCC0。
WindowGraphicData gWindowGraphic55FCD8 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0055FCD8。
WindowTextPrototype gWindowText55FCF0 { // 原全局：0x0055FCF0。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名稱", // 原常量参考：0x00586230。
};
WindowTextPrototype gWindowText55FCF8 { // 原全局：0x0055FCF8。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等級", // 原常量参考：0x00586228。
};
WindowTextPrototype gWindowText55FD00 { // 原全局：0x0055FD00。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"x5", // 原常量参考：0x00586224。
};
WindowTextPrototype gWindowText55FD08 { // 原全局：0x0055FD08。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"職業", // 原常量参考：0x0058621C。
};
WindowTextPrototype gWindowText55FD10 { // 原全局：0x0055FD10。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族", // 原常量参考：0x00586214。
};
WindowTextPrototype gWindowText55FD18 { // 原全局：0x0055FD18。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族稱號", // 原常量参考：0x00586208。
};
WindowTextPrototype gWindowText55FD20 { // 原全局：0x0055FD20。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"特殊稱號", // 原常量参考：0x005861FC。
};
WindowTextPrototype gWindowText55FD28 { // 原全局：0x0055FD28。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"自定稱號", // 原常量参考：0x005861F0。
};
WindowTextPrototype gWindowText55FD30 { // 原全局：0x0055FD30。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"種族", // 原常量参考：0x005861E8。
};
WindowTextPrototype gWindowText55FD38 { // 原全局：0x0055FD38。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"經驗值", // 原常量参考：0x005861E0。
};
WindowTextPrototype gWindowText55FD40 { // 原全局：0x0055FD40。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"下一級經驗值", // 原常量参考：0x005861D0。
};
WindowTextPrototype gWindowText55FD48 { // 原全局：0x0055FD48。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"生命值", // 原常量参考：0x005861C8。
};
WindowTextPrototype gWindowText55FD50 { // 原全局：0x0055FD50。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔力", // 原常量参考：0x005861C0。
};
WindowTextPrototype gWindowText55FD58 { // 原全局：0x0055FD58。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"對戰勝利值", // 原常量参考：0x005861B4。
};

WindowGraphicData gWindowGraphic563298 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00563298。
WindowGraphicData gWindowGraphic5632B0 {0x3BAE2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005632B0。
WindowGraphicData gWindowGraphic5632C8 {0x3BAE3, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005632C8。
WindowGraphicData gWindowGraphic5632E0 {0x3B5C4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005632E0。
WindowGraphicData gWindowGraphic5632F8 {0x3B5CF, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005632F8。
WindowGraphicData gWindowGraphic563478 {0x3B586, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00563478。
WindowGraphicData gWindowGraphic563490 {0x3B589, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00563490。
WindowGraphicData gWindowGraphic5634A8 {0x3B58C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005634A8。
WindowGraphicData gWindowGraphic5634C0 {0x3B58F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005634C0。
WindowTextPrototype gWindowText5634D8 { // 原全局：0x005634D8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"寵物名稱", // 原常量参考：0x00586048。
};
WindowTextPrototype gWindowText5634E0 { // 原全局：0x005634E0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"300", // 原常量参考：0x00586044。
};
WindowTextPrototype gWindowText5634E8 { // 原全局：0x005634E8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"100000/100000", // 原常量参考：0x00586034。
};
WindowTextPrototype gWindowText5634F0 { // 原全局：0x005634F0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"100000/100000", // 原常量参考：0x00586034。
};
WindowTextPrototype gWindowText5634F8 { // 原全局：0x005634F8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"種族", // 原常量参考：0x005861E8。
};
std::uint32_t gWindowMouseFlagsD677A0 = 0; // 原全局：0x00D677A0。

WindowGraphicData gWindowGraphic560680 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560680。
WindowGraphicData gWindowGraphic560698 {0x3BAB5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560698。
WindowGraphicData gWindowGraphic5606B0 {0x3B584, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005606B0。
WindowGraphicData gWindowGraphic5606C8 {0x3BAB8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005606C8。
WindowGraphicData gWindowGraphic5606E0 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005606E0。
WindowGraphicData gWindowGraphic5606F8 {0x3BAB4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005606F8。
WindowGraphicData gWindowGraphic560710 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560710。
WindowGraphicData gWindowGraphic560728 {0x3BAB1, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00560728。
WindowGraphicData gWindowGraphic560740 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560740。
WindowTextPrototype gWindowText560758 { // 原全局：0x00560758。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名稱", // 原常量参考：0x00586230。
};
WindowTextPrototype gWindowText560760 { // 原全局：0x00560760。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等級", // 原常量参考：0x00586228。
};
WindowTextPrototype gWindowText560768 { // 原全局：0x00560768。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"新增點數", // 原常量参考：0x005861A8。
};
WindowTextPrototype gWindowText560770 { // 原全局：0x00560770。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"體力", // 原常量参考：0x005861A0。
};
WindowTextPrototype gWindowText560778 { // 原全局：0x00560778。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"力量", // 原常量参考：0x00586198。
};
WindowTextPrototype gWindowText560780 { // 原全局：0x00560780。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"強度", // 原常量参考：0x00586190。
};
WindowTextPrototype gWindowText560788 { // 原全局：0x00560788。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"速度", // 原常量参考：0x00586188。
};
WindowTextPrototype gWindowText560790 { // 原全局：0x00560790。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔法", // 原常量参考：0x00586180。
};
WindowTextPrototype gWindowText560798 { // 原全局：0x00560798。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"攻擊", // 原常量参考：0x00586178。
};
WindowTextPrototype gWindowText5607A0 { // 原全局：0x005607A0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"防禦", // 原常量参考：0x00586170。
};
WindowTextPrototype gWindowText5607A8 { // 原全局：0x005607A8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"敏捷", // 原常量参考：0x00586168。
};
WindowTextPrototype gWindowText5607B0 { // 原全局：0x005607B0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"精神", // 原常量参考：0x00586160。
};
WindowTextPrototype gWindowText5607B8 { // 原全局：0x005607B8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"回復", // 原常量参考：0x00586158。
};
WindowTextPrototype gWindowText5607C0 { // 原全局：0x005607C0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魅力", // 原常量参考：0x00586150。
};
WindowTextPrototype gWindowText5607C8 { // 原全局：0x005607C8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗毒", // 原常量参考：0x00586148。
};
WindowTextPrototype gWindowText5607D0 { // 原全局：0x005607D0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗昏睡", // 原常量参考：0x00586140。
};
WindowTextPrototype gWindowText5607D8 { // 原全局：0x005607D8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗石化", // 原常量参考：0x00586138。
};
WindowTextPrototype gWindowText5607E0 { // 原全局：0x005607E0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗醉", // 原常量参考：0x00586130。
};
WindowTextPrototype gWindowText5607E8 { // 原全局：0x005607E8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗混亂", // 原常量参考：0x00586128。
};
WindowTextPrototype gWindowText5607F0 { // 原全局：0x005607F0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"抗遺忘", // 原常量参考：0x00586120。
};
WindowTextPrototype gWindowText5607F8 { // 原全局：0x005607F8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"必殺率", // 原常量参考：0x00586118。
};
WindowTextPrototype gWindowText560800 { // 原全局：0x00560800。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"反擊率", // 原常量参考：0x00586110。
};
WindowTextPrototype gWindowText560808 { // 原全局：0x00560808。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"命中率", // 原常量参考：0x00586108。
};
WindowTextPrototype gWindowText560810 { // 原全局：0x00560810。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"閃躲率", // 原常量参考：0x00586100。
};
WindowTextPrototype gWindowText560818 { // 原全局：0x00560818。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"耐力", // 原常量参考：0x005860F8。
};
WindowTextPrototype gWindowText560820 { // 原全局：0x00560820。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"靈巧", // 原常量参考：0x005860F0。
};
WindowTextPrototype gWindowText560828 { // 原全局：0x00560828。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"智力", // 原常量参考：0x005860E8。
};
WindowTextPrototype gWindowText560830 { // 原全局：0x00560830。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔攻", // 原常量参考：0x005860E0。
};
WindowTextPrototype gWindowText560838 { // 原全局：0x00560838。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"魔防", // 原常量参考：0x005860D8。
};

WindowGraphicData gWindowGraphic560DA0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560DA0。
WindowGraphicData gWindowGraphic560DB8 {0x3BAB5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560DB8。
WindowGraphicData gWindowGraphic560DD0 {0x3B583, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560DD0。
WindowGraphicData gWindowGraphic560DE8 {0x3BAB9, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560DE8。
WindowGraphicData gWindowGraphic560E00 {0x3BAC2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E00。
WindowGraphicData gWindowGraphic560E18 {0x3B574, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E18。
WindowGraphicData gWindowGraphic560E30 {0x3B577, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E30。
WindowGraphicData gWindowGraphic560E48 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E48。
WindowGraphicData gWindowGraphic560E60 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E60。
WindowGraphicData gWindowGraphic560E78 {0x3B59D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E78。
WindowGraphicData gWindowGraphic560E90 {0x3BAB3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560E90。
WindowGraphicData gWindowGraphic560EA8 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560EA8。
WindowGraphicData gWindowGraphic560EC0 {0x3BAB1, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x00560EC0。
WindowGraphicData gWindowGraphic560ED8 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560ED8。
WindowGraphicData gWindowGraphic560EF0 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560EF0。
WindowGraphicData gWindowGraphic560F08 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560F08。
WindowGraphicData gWindowGraphic560F20 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00560F20。
WindowTextPrototype gWindowText560F38 { // 原全局：0x00560F38。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名稱", // 原常量参考：0x00586230。
};
WindowTextPrototype gWindowText560F40 { // 原全局：0x00560F40。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等級", // 原常量参考：0x00586228。
};
WindowTextPrototype gWindowText560F48 { // 原全局：0x00560F48。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"稱號", // 原常量参考：0x005860D0。
};
std::uint32_t gWindowMouseFlagsD676F0 = 0; // 原全局：0x00D676F0。
std::uint32_t gWindowMouseFlagsD6772C = 0; // 原全局：0x00D6772C。

WindowGraphicData gWindowGraphic5613F0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005613F0。
WindowGraphicData gWindowGraphic561408 {0x3BAB5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561408。
WindowGraphicData gWindowGraphic561420 {0x3B583, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561420。
WindowGraphicData gWindowGraphic561438 {0x3BAB8, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561438。
WindowGraphicData gWindowGraphic561450 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561450。
WindowGraphicData gWindowGraphic561468 {0x3C10F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561468。
WindowGraphicData gWindowGraphic561480 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561480。
WindowGraphicData gWindowGraphic561498 {0x3BAB1, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x00561498。
WindowGraphicData gWindowGraphic5614B0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005614B0。
WindowGraphicData gWindowGraphic5614C8 {0x3BAC2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005614C8。
WindowGraphicData gWindowGraphic5614E0 {0x3C0F0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005614E0。
WindowGraphicData gWindowGraphic5614F8 {0x3C113, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005614F8。
WindowTextPrototype gWindowText561510 { // 原全局：0x00561510。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名稱", // 原常量参考：0x00586230。
};
WindowTextPrototype gWindowText561518 { // 原全局：0x00561518。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等級", // 原常量参考：0x00586228。
};
WindowTextPrototype gWindowText561520 { // 原全局：0x00561520。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"稱號", // 原常量参考：0x005860D0。
};
WindowTextPrototype gWindowText561528 { // 原全局：0x00561528。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"職業", // 原常量参考：0x0058621C。
};
WindowTextPrototype gWindowText561530 { // 原全局：0x00561530。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"公會", // 原常量参考：0x005860C8。
};
WindowTextPrototype gWindowText561540 { // 原全局：0x00561540。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"SELL", // 原常量参考：0x005860B8。
};
WindowTextPrototype gWindowText561548 { // 原全局：0x00561548。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"BUY", // 原常量参考：0x005860B4。
};
WindowTextPrototype gWindowText561550 { // 原全局：0x00561550。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"ABOUT", // 原常量参考：0x005860AC。
};

WindowGraphicData gWindowGraphic561FE0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561FE0。
WindowGraphicData gWindowGraphic561FF8 {0x3BA38, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00561FF8。
WindowGraphicData gWindowGraphic562010 {0x3BA3A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562010。
WindowGraphicData gWindowGraphic562028 {0x3BA3B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562028。
WindowGraphicData gWindowGraphic562040 {0x3B56B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562040。
WindowGraphicData gWindowGraphic562058 {0x3BA31, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562058。
WindowGraphicData gWindowGraphic562070 {0x3B565, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562070。
WindowGraphicData gWindowGraphic562088 {0x3BA3C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562088。
WindowGraphicData gWindowGraphic5620A0 {0x3B5C3, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005620A0。
WindowGraphicData gWindowGraphic5620B8 {0x3BAC2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005620B8。
WindowTextPrototype gWindowText5620D0 { // 原全局：0x005620D0。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？？？？？？？　　000", // 原常量参考：0x00586078。
};
std::int32_t gWindowAnimationImage5620D8 = 0x1AF16; // 原全局：0x005620D8。
std::int32_t gWindowAnimationImage5620DC = 0x1AF16; // 原全局：0x005620DC。

WindowGraphicData gWindowGraphic5628A8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005628A8。
WindowGraphicData gWindowGraphic5628C0 {0x3BA3F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005628C0。
WindowGraphicData gWindowGraphic5628D8 {0x3BA3E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005628D8。
WindowGraphicData gWindowGraphic5628F0 {0x3BA31, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005628F0。
WindowGraphicData gWindowGraphic562908 {0x3B565, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562908。
WindowTextPrototype gWindowText562920 { // 原全局：0x00562920。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？？？？？？？", // 原常量参考：0x00586060。
};
std::int32_t gWindowAnimationImage562928 = 0x1AF16; // 原全局：0x00562928。
std::int32_t gWindowAnimationImage56292C = 0x1AF16; // 原全局：0x0056292C。

WindowGraphicData gWindowGraphic562568 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562568。
WindowGraphicData gWindowGraphic562580 {0x3BA2E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562580。
WindowGraphicData gWindowGraphic562598 {0x3BA30, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562598。
WindowGraphicData gWindowGraphic5625C8 {0x3B56B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005625C8。
WindowGraphicData gWindowGraphic5625E0 {0x3B565, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005625E0。
WindowGraphicData gWindowGraphic5625F8 {0x3BA34, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005625F8。
WindowTextPrototype gWindowText562610 { // 原全局：0x00562610。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？？？？？？？", // 原常量参考：0x00586060。
};
std::int32_t gWindowAnimationImage562618 = 0x1AF16; // 原全局：0x00562618。
std::int32_t gWindowAnimationImage56261C = 0x1AF16; // 原全局：0x0056261C。

WindowGraphicData gWindowGraphic562B98 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562B98。
WindowGraphicData gWindowGraphic562BB0 {244290, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562BB0。
WindowGraphicData gWindowGraphic562BC8 {244291, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562BC8。
WindowGraphicData gWindowGraphic562BE0 {244294, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562BE0。
WindowGraphicData gWindowGraphic562BF8 {98131, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562BF8。
WindowGraphicData gWindowGraphic562C10 {98140, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562C10。
WindowGraphicData gWindowGraphic562C28 {98143, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562C28。
WindowGraphicData gWindowGraphic562C40 {98146, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562C40。
WindowGraphicData gWindowGraphic562C58 {244297, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00562C58。
WindowTextPrototype gWindowText562C70 { // 原全局：0x00562C70。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};
WindowTextPrototype gWindowText562C78 { // 原全局：0x00562C78。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};
std::int32_t gWindowAnimationImage562C80 = 100000; // 原全局：0x00562C80。
WindowNumberData gWindowNumber562CB0 {&gInventoryMoneyTextCB086C, 10, 3, 1, 0}; // 原全局：0x00562CB0。
WindowNumberData gWindowNumber562CB8 {&gInventoryValueTextCC3ECC, 10, 3, 1, 0}; // 原全局：0x00562CB8。

WindowGraphicData gWindowGraphic56C348 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C348。
WindowGraphicData gWindowGraphic56C360 {0x3BA42, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C360。
WindowGraphicData gWindowGraphic56C378 {0x3BA46, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C378。
WindowGraphicData gWindowGraphic56C390 {0x17F63, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C390。
WindowGraphicData gWindowGraphic56C3A8 {0x17F53, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C3A8。
WindowGraphicData gWindowGraphic56C3C0 {0x17F5C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C3C0。
WindowGraphicData gWindowGraphic56C3D8 {0x17F5F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C3D8。
WindowGraphicData gWindowGraphic56C3F0 {0x3BA49, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C3F0。
std::int32_t gWindowAnimationImage56C418 = 100000; // 原全局：0x0056C418。
WindowNumberData gWindowNumber56C420 {&gBattleMoneyTextCF469C, 10, 3, 1, 0}; // 原全局：0x0056C420。
WindowNumberData gWindowNumber56C428 {&gBattleValueTextC8A960, 10, 3, 1, 0}; // 原全局：0x0056C428。

WindowGraphicData gWindowGraphic56C9F8 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056C9F8。
WindowGraphicData gWindowGraphic56CA10 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CA10。
WindowGraphicData gWindowGraphic56CA28 {0x3BE4B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CA28。
WindowGraphicData gWindowGraphic56CA40 {0x3BE4E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CA40。
WindowGraphicData gWindowGraphic56CA58 {0x3BE54, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CA58。
WindowGraphicData gWindowGraphic56CA70 {0x3BE51, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CA70。
std::int32_t gWindowAnimationImageD6780C = 0; // 原全局：0x00D6780C。

WindowGraphicData gWindowGraphic56D010 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D010。
WindowGraphicData gWindowGraphic56D028 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D028。
WindowGraphicData gWindowGraphic56D040 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D040。
WindowGraphicData gWindowGraphic56D058 {243042, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D058。
WindowGraphicData gWindowGraphic56D070 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D070。
WindowGraphicData gWindowGraphic56D088 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D088。
WindowTextPrototype gWindowText56D0A0 { // 原全局：0x0056D0A0。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■", // 原常量参考：0x00585F64。
};
WindowTextPrototype gWindowText56D0A8 { // 原全局：0x0056D0A8。
    {std::byte{4}, std::byte{1}, std::byte{0}, std::byte{0}}, L"請選擇對象。", // 原常量参考：0x00585F54。
};

WindowGraphicData gWindowGraphic56D310 {245320, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D310。
WindowGraphicData gWindowGraphic56D328 {245332, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D328。
WindowGraphicData gWindowGraphic56D340 {243220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D340。
WindowGraphicData gWindowGraphic56D358 {243220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D358。
WindowGraphicData gWindowGraphic56D370 {243220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D370。
WindowGraphicData gWindowGraphic56D388 {243220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D388。
WindowGraphicData gWindowGraphic56D3A0 {243220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D3A0。
WindowGraphicData gWindowGraphic56D3B8 {243220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056D3B8。
WindowTextPrototype gWindowText56D3D0 { // 原全局：0x0056D3D0。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■", // 原常量参考：0x00585F64。
};
WindowTextPrototype gWindowText56D3D8 { // 原全局：0x0056D3D8。
    {std::byte{4}, std::byte{1}, std::byte{0}, std::byte{0}}, L"請選擇對象。", // 原常量参考：0x00585F54。
};
WindowTextPrototype gWindowText56D3E0 { // 原全局：0x0056D3E0。
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}},
    L"健康程度  等級 名稱             體力            魔力",
};

WindowTextPrototype gWindowText56D828 { // 原全局：0x0056D828。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, cg::text::FromGbkWide(input::LegacyText593850()),
};
std::int32_t gWindowAnimationImage56D830 = 100000; // 原全局：0x0056D830。

WindowGraphicData gWindowGraphic5734D8 {0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005734D8。
WindowGraphicData gWindowGraphic5734F0 {0x3BB3C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005734F0。
WindowGraphicData gWindowGraphic573508 {0x3BB3D, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00573508。
WindowGraphicData gWindowGraphic573520 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573520。
WindowGraphicData gWindowGraphic573538 {0x3B577, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573538。
WindowGraphicData gWindowGraphic573550 {0x3BB3E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573550。
WindowGraphicData gWindowGraphic573568 {0x3BB41, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573568。
WindowGraphicData gWindowGraphic573580 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573580。
WindowGraphicData gWindowGraphic573598 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573598。
WindowTextPrototype gWindowText5735B0 { // 原全局：0x005735B0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"留言板文字列",
};
WindowTextPrototype gWindowText5735B8 { // 原全局：0x005735B8。
    {std::byte{0}, std::byte{10}, std::byte{0}, std::byte{0}}, L"名稱",
};
WindowTextPrototype gWindowText5735C0 { // 原全局：0x005735C0。
    {std::byte{0}, std::byte{10}, std::byte{0}, std::byte{0}}, L"留言時間",
};

WindowGraphicData gWindowGraphic573AF0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573AF0。
WindowGraphicData gWindowGraphic573B08 {0x3BA9C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B08。
WindowGraphicData gWindowGraphic573B20 {0x3BA9D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B20。
WindowGraphicData gWindowGraphic573B38 {0x3BA9E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B38。
WindowGraphicData gWindowGraphic573B50 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B50。
WindowGraphicData gWindowGraphic573B68 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B68。
WindowGraphicData gWindowGraphic573B80 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B80。
WindowGraphicData gWindowGraphic573B98 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573B98。
WindowGraphicData gWindowGraphic573BB0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573BB0。
WindowGraphicData gWindowGraphic573BC8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573BC8。
WindowGraphicData gWindowGraphic573BE0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573BE0。
WindowGraphicData gWindowGraphic573BF8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573BF8。
WindowGraphicData gWindowGraphic573C10 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573C10。
WindowGraphicData gWindowGraphic573C28 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573C28。
WindowGraphicData gWindowGraphic573C40 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573C40。
WindowGraphicData gWindowGraphic573C58 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573C58。
WindowGraphicData gWindowGraphic573C70 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573C70。
WindowGraphicData gWindowGraphic573C88 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573C88。
WindowGraphicData gWindowGraphic573CA0 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573CA0。
WindowGraphicData gWindowGraphic573CB8 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573CB8。
WindowGraphicData gWindowGraphic573CD0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00573CD0。
WindowTextPrototype gWindowText573CF8 { // 原全局：0x00573CF8。
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00585DE8。
};
WindowTextPrototype gWindowText573D00 { // 原全局：0x00573D00。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00585DE8。
};
WindowTextPrototype gWindowText573D08 { // 原全局：0x00573D08。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"＊目前無法裝備", // 原常量参考：0x00585DD8。
};
std::int32_t gWindowAnimationImage573D10 = 100000; // 原全局：0x00573D10。
WindowNumberData gWindowNumber573D18 {&gShopMoneyTextCC1A00, 0, 3, 1, 0}; // 原全局：0x00573D18。
WindowNumberData gWindowNumber573D20 {&gShopValueTextCF4A28, 0, 3, 1, 0}; // 原全局：0x00573D20。
WindowNumberData gWindowNumber573D28 {&gShopTotalTextCC4B2C, 0, 3, 1, 0}; // 原全局：0x00573D28。
WindowNumberData gWindowNumber573D30 {&gShopPriceTextsD673EC[0], 0, 3, 1, 0}; // 原全局：0x00573D30。
WindowNumberData gWindowNumber573D38 {&gShopPriceTextsD673EC[1], 0, 3, 1, 0}; // 原全局：0x00573D38。
WindowNumberData gWindowNumber573D40 {&gShopPriceTextsD673EC[2], 0, 3, 1, 0}; // 原全局：0x00573D40。
WindowNumberData gWindowNumber573D48 {&gShopPriceTextsD673EC[3], 0, 3, 1, 0}; // 原全局：0x00573D48。
WindowNumberData gWindowNumber573D50 {&gShopPriceTextsD673EC[4], 0, 3, 1, 0}; // 原全局：0x00573D50。
WindowNumberData gWindowNumber573D58 {&gShopCountTextsD2D988[0], 0, 3, 1, 0}; // 原全局：0x00573D58。
WindowNumberData gWindowNumber573D60 {&gShopCountTextsD2D988[1], 0, 3, 1, 0}; // 原全局：0x00573D60。
WindowNumberData gWindowNumber573D68 {&gShopCountTextsD2D988[2], 0, 3, 1, 0}; // 原全局：0x00573D68。
WindowNumberData gWindowNumber573D70 {&gShopCountTextsD2D988[3], 0, 3, 1, 0}; // 原全局：0x00573D70。
WindowNumberData gWindowNumber573D78 {&gShopCountTextsD2D988[4], 0, 3, 1, 0}; // 原全局：0x00573D78。
WindowGraphicData gWindowGraphicD678E0 {}; // 原全局：0x00D678E0。
static_assert(sizeof(WindowGraphicData) == 0x18);
static_assert(offsetof(WindowGraphicData, height) == 0x10);
// 51的flags源与49图形源+10H重叠，实例仍分别按type4/type1创建。
std::uint32_t& gWindowMouseFlagsD678F0 = // 原全局：0x00D678F0。
    reinterpret_cast<std::uint32_t&>(gWindowGraphicD678E0.height);

WindowGraphicData gWindowGraphic572E20 {0x3BA15, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572E20。
WindowGraphicData gWindowGraphic572E38 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572E38。
WindowGraphicData gWindowGraphic572E50 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572E50。
WindowGraphicData gWindowGraphic572E68 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572E68。
WindowGraphicData gWindowGraphic572E80 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572E80。
WindowGraphicData gWindowGraphic572E98 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572E98。
WindowGraphicData gWindowGraphic572EB0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572EB0。
WindowGraphicData gWindowGraphic572EC8 {0x3BA17, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572EC8。
WindowGraphicData gWindowGraphic572EF8 {0x3BA16, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00572EF8。
WindowGraphicData gWindowGraphic572F10 {0x3BA18, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572F10。
WindowGraphicData gWindowGraphic572F28 {0x3BA1E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572F28。
WindowGraphicData gWindowGraphic572F40 {0x3BA21, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572F40。
WindowTextPrototype gWindowText572F70 { // 原全局：0x00572F70。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"人員",
};

WindowGraphicData gWindowGraphic574668 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574668。
WindowGraphicData gWindowGraphic574680 {244383, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574680。
WindowGraphicData gWindowGraphic574698 {244384, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574698。
WindowGraphicData gWindowGraphic5746B0 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005746B0。
WindowGraphicData gWindowGraphic5746C8 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005746C8。
WindowGraphicData gWindowGraphic5746E0 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005746E0。
WindowGraphicData gWindowGraphic5746F8 {243048, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005746F8。
WindowGraphicData gWindowGraphic574710 {245320, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574710。
WindowGraphicData gWindowGraphic574728 {244387, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574728。
WindowGraphicData gWindowGraphic574740 {243103, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574740。
WindowGraphicData gWindowGraphic574758 {244386, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574758。
WindowGraphicData gWindowGraphic574770 {243127, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574770。
WindowGraphicData gWindowGraphic574788 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00574788。
WindowGraphicData gWindowGraphic5747A0 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005747A0。
WindowTextPrototype gWindowText5747C0 { // 原全局：0x005747C0。
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};
WindowTextPrototype gWindowText5747C8 { // 原全局：0x005747C8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};
WindowTextPrototype gWindowText5747D0 { // 原全局：0x005747D0。
    {std::byte{0x8A}, std::byte{1}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00586054。
};
WindowTextPrototype gWindowText5747D8 { // 原全局：0x005747D8。
    {std::byte{0x8A}, std::byte{8}, std::byte{0}, std::byte{0}}, L"0000000000", // 原常量参考：0x00585DF4。
};
std::int32_t gWindowAnimationImage5747E0 = 100000; // 原全局：0x005747E0。
WindowNumberData gWindowNumber5747E8 {&gWindow54MoneyTextC9A9A4, 0, 3, 1, 0}; // 原全局：0x005747E8。
WindowNumberData gWindowNumber5747F0 {&gWindow54AmountTextD45F78, 0, 3, 1, 0}; // 原全局：0x005747F0。
WindowNumberData gWindowNumber5747F8 {&gWindow54SkillValueTextsCF4978[0], 0, 0, 1, 0}; // 原全局：0x005747F8。
WindowNumberData gWindowNumber574800 {&gWindow54SkillValueTextsCF4978[1], 0, 0, 1, 0}; // 原全局：0x00574800。
WindowNumberData gWindowNumber574808 {&gWindow54SkillValueTextsCF4978[2], 0, 0, 1, 0}; // 原全局：0x00574808。
WindowNumberData gWindowNumber574810 {&gWindow54SkillValueTextsCF4978[3], 0, 0, 1, 0}; // 原全局：0x00574810。
WindowNumberData gWindowNumber574818 {&gWindow54SkillValueTextsCF4978[4], 0, 0, 1, 0}; // 原全局：0x00574818。
WindowNumberData gWindowNumber574820 {&gWindow54SkillPriceTextsCF493C[0], 0, 3, 1, 0}; // 原全局：0x00574820。
WindowNumberData gWindowNumber574828 {&gWindow54SkillPriceTextsCF493C[1], 0, 3, 1, 0}; // 原全局：0x00574828。
WindowNumberData gWindowNumber574830 {&gWindow54SkillPriceTextsCF493C[2], 0, 3, 1, 0}; // 原全局：0x00574830。
WindowNumberData gWindowNumber574838 {&gWindow54SkillPriceTextsCF493C[3], 0, 3, 1, 0}; // 原全局：0x00574838。
WindowNumberData gWindowNumber574840 {&gWindow54SkillPriceTextsCF493C[4], 0, 3, 1, 0}; // 原全局：0x00574840。
WindowNumberData gWindowNumber574848 {&gWindow54ValueTextsCB035C[0], 0, 0, 1, 0}; // 原全局：0x00574848。
WindowNumberData gWindowNumber574850 {&gWindow54ValueTextsCB035C[1], 0, 0, 1, 0}; // 原全局：0x00574850。
WindowNumberData gWindowNumber574858 {&gWindow54ValueTextsCB035C[2], 0, 0, 1, 0}; // 原全局：0x00574858。
WindowNumberData gWindowNumber574860 {&gWindow54ValueTextsCB035C[3], 0, 0, 1, 0}; // 原全局：0x00574860。
WindowNumberData gWindowNumber574868 {&gWindow54ValueTextsCB035C[4], 0, 0, 1, 0}; // 原全局：0x00574868。
WindowNumberData gWindowNumber574870 {&gWindow54ValueTextsCB035C[5], 0, 0, 1, 0}; // 原全局：0x00574870。
WindowNumberData gWindowNumber574878 {&gWindow54ValueTextsCB035C[6], 0, 0, 1, 0}; // 原全局：0x00574878。
WindowNumberData gWindowNumber574880 {&gWindow54ValueTextsCB035C[7], 0, 0, 1, 0}; // 原全局：0x00574880。
WindowNumberData gWindowNumber574888 {&gWindow54ValueTextsCB035C[8], 0, 0, 1, 0}; // 原全局：0x00574888。
WindowNumberData gWindowNumber574890 {&gWindow54ValueTextsCB035C[9], 0, 0, 1, 0}; // 原全局：0x00574890。
WindowNumberData gWindowNumber574898 {&gWindow54ValueTextsC8B318[0], 0, 0, 1, 0}; // 原全局：0x00574898。
WindowNumberData gWindowNumber5748A0 {&gWindow54ValueTextsC8B318[1], 0, 0, 1, 0}; // 原全局：0x005748A0。
WindowNumberData gWindowNumber5748A8 {&gWindow54ValueTextsC8B318[2], 0, 0, 1, 0}; // 原全局：0x005748A8。
WindowNumberData gWindowNumber5748B0 {&gWindow54ValueTextsC8B318[3], 0, 0, 1, 0}; // 原全局：0x005748B0。
WindowNumberData gWindowNumber5748B8 {&gWindow54ValueTextsC8B318[4], 0, 0, 1, 0}; // 原全局：0x005748B8。
WindowNumberData gWindowNumber5748C0 {&gWindow54ValueTextsC8B318[5], 0, 0, 1, 0}; // 原全局：0x005748C0。
WindowNumberData gWindowNumber5748C8 {&gWindow54ValueTextsC8B318[6], 0, 0, 1, 0}; // 原全局：0x005748C8。
WindowNumberData gWindowNumber5748D0 {&gWindow54ValueTextsC8B318[7], 0, 0, 1, 0}; // 原全局：0x005748D0。
WindowNumberData gWindowNumber5748D8 {&gWindow54ValueTextsC8B318[8], 0, 0, 1, 0}; // 原全局：0x005748D8。
WindowNumberData gWindowNumber5748E0 {&gWindow54ValueTextsC8B318[9], 0, 0, 1, 0}; // 原全局：0x005748E0。
std::uint32_t gWindowMouseFlagsD67900 = 0; // 原全局：0x00D67900。

WindowGraphicData gWindowGraphic5756F0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005756F0。
WindowGraphicData gWindowGraphic575708 {0x17F32, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575708。
WindowGraphicData gWindowGraphic575720 {0x17F33, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575720。
WindowGraphicData gWindowGraphic575738 {0x17F34, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575738。
WindowGraphicData gWindowGraphic575750 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575750。
WindowGraphicData gWindowGraphic575768 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575768。
WindowGraphicData gWindowGraphic575780 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575780。
WindowGraphicData gWindowGraphic575798 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575798。
WindowGraphicData gWindowGraphic5757B0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005757B0。
WindowGraphicData gWindowGraphic5757C8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005757C8。
WindowGraphicData gWindowGraphic5757E0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005757E0。
WindowGraphicData gWindowGraphic5757F8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005757F8。
WindowGraphicData gWindowGraphic575810 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575810。
WindowGraphicData gWindowGraphic575828 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575828。
WindowGraphicData gWindowGraphic575840 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575840。
WindowGraphicData gWindowGraphic575858 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575858。
WindowGraphicData gWindowGraphic575870 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575870。
WindowGraphicData gWindowGraphic575888 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00575888。
WindowGraphicData gWindowGraphic5758A0 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005758A0。
WindowGraphicData gWindowGraphic5758B8 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005758B8。
WindowGraphicData gWindowGraphic5758D0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005758D0。
WindowTextPrototype gWindowText5758F8 { // 原全局：0x005758F8。
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00585DE8。
};
WindowTextPrototype gWindowText575900 { // 原全局：0x00575900。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00585DE8。
};
WindowTextPrototype gWindowText575908 { // 原全局：0x00575908。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"＊目前無法裝備", // 原常量参考：0x00585DD8。
};
WindowTextPrototype gWindowText575910 { // 原全局：0x00575910。
    {std::byte{1}, std::byte{12}, std::byte{0}, std::byte{0}}, L"剩餘 %2d 個", // 原常量参考：0x00585DCC。
};
std::int32_t gWindowAnimationImage575918 = 100000; // 原全局：0x00575918。
WindowNumberData gWindowNumber575920 {&gWindow69BalanceTextD2E448, 10, 3, 1, 0}; // 原全局：0x00575920。
WindowNumberData gWindowNumber575930 {&gWindow69TotalTextCC48B0, 0, 3, 1, 0}; // 原全局：0x00575930。
WindowNumberData gWindowNumber575938 {&gWindow69PriceTextsD2D7CC[0], 10, 3, 1, 0}; // 原全局：0x00575938。
WindowNumberData gWindowNumber575940 {&gWindow69PriceTextsD2D7CC[1], 10, 3, 1, 0}; // 原全局：0x00575940。
WindowNumberData gWindowNumber575948 {&gWindow69PriceTextsD2D7CC[2], 10, 3, 1, 0}; // 原全局：0x00575948。
WindowNumberData gWindowNumber575950 {&gWindow69PriceTextsD2D7CC[3], 10, 3, 1, 0}; // 原全局：0x00575950。
WindowNumberData gWindowNumber575958 {&gWindow69PriceTextsD2D7CC[4], 10, 3, 1, 0}; // 原全局：0x00575958。
WindowNumberData gWindowNumber575960 {&gWindow69CountTextsD2EA24[0], 0, 3, 1, 0}; // 原全局：0x00575960。
WindowNumberData gWindowNumber575968 {&gWindow69CountTextsD2EA24[1], 0, 3, 1, 0}; // 原全局：0x00575968。
WindowNumberData gWindowNumber575970 {&gWindow69CountTextsD2EA24[2], 0, 3, 1, 0}; // 原全局：0x00575970。
WindowNumberData gWindowNumber575978 {&gWindow69CountTextsD2EA24[3], 0, 3, 1, 0}; // 原全局：0x00575978。
WindowNumberData gWindowNumber575980 {&gWindow69CountTextsD2EA24[4], 0, 3, 1, 0}; // 原全局：0x00575980。
std::uint32_t gWindowMouseFlagsD67910 = 0; // 原全局：0x00D67910。

WindowGraphicData gWindowGraphic5762F0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005762F0。
WindowGraphicData gWindowGraphic576308 {0x18661, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576308。
WindowGraphicData gWindowGraphic576320 {0x18662, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576320。
WindowGraphicData gWindowGraphic576338 {0x18663, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576338。
WindowGraphicData gWindowGraphic576350 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576350。
WindowGraphicData gWindowGraphic576368 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576368。
WindowGraphicData gWindowGraphic576380 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576380。
WindowGraphicData gWindowGraphic576398 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576398。
WindowGraphicData gWindowGraphic5763B0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005763B0。
WindowGraphicData gWindowGraphic5763C8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005763C8。
WindowGraphicData gWindowGraphic5763E0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005763E0。
WindowGraphicData gWindowGraphic5763F8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005763F8。
WindowGraphicData gWindowGraphic576410 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576410。
WindowGraphicData gWindowGraphic576428 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576428。
WindowGraphicData gWindowGraphic576440 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576440。
WindowGraphicData gWindowGraphic576458 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576458。
WindowGraphicData gWindowGraphic576470 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576470。
WindowGraphicData gWindowGraphic576488 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576488。
WindowGraphicData gWindowGraphic5764A0 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005764A0。
WindowGraphicData gWindowGraphic5764B8 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005764B8。
WindowGraphicData gWindowGraphic5764D0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005764D0。
WindowTextPrototype gWindowText5764F8 { // 原全局：0x005764F8。
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00585DE8。
};
WindowTextPrototype gWindowText576500 { // 原全局：0x00576500。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"？？？？", // 原常量参考：0x00585DE8。
};
WindowTextPrototype gWindowText576508 { // 原全局：0x00576508。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, L"＊目前無法裝備", // 原常量参考：0x00585DD8。
};
WindowTextPrototype gWindowText576510 { // 原全局：0x00576510。
    {std::byte{1}, std::byte{12}, std::byte{0}, std::byte{0}}, L"剩餘 %2d 個", // 原常量参考：0x00585DCC。
};
std::int32_t gWindowAnimationImage576518 = 100000; // 原全局：0x00576518。
WindowNumberData gWindowNumber576520 {&gWindow70BalanceTextCF3F14, 10, 3, 1, 0}; // 原全局：0x00576520。
WindowNumberData gWindowNumber576530 {&gWindow70TotalTextD2D804, 0, 3, 1, 0}; // 原全局：0x00576530。
WindowNumberData gWindowNumber576538 {&gWindow70PriceTextsCF49C4[0], 10, 3, 1, 0}; // 原全局：0x00576538。
WindowNumberData gWindowNumber576540 {&gWindow70PriceTextsCF49C4[1], 10, 3, 1, 0}; // 原全局：0x00576540。
WindowNumberData gWindowNumber576548 {&gWindow70PriceTextsCF49C4[2], 10, 3, 1, 0}; // 原全局：0x00576548。
WindowNumberData gWindowNumber576550 {&gWindow70PriceTextsCF49C4[3], 10, 3, 1, 0}; // 原全局：0x00576550。
WindowNumberData gWindowNumber576558 {&gWindow70PriceTextsCF49C4[4], 10, 3, 1, 0}; // 原全局：0x00576558。
WindowNumberData gWindowNumber576560 {&gWindow70CountTextsCC4848[0], 0, 3, 1, 0}; // 原全局：0x00576560。
WindowNumberData gWindowNumber576568 {&gWindow70CountTextsCC4848[1], 0, 3, 1, 0}; // 原全局：0x00576568。
WindowNumberData gWindowNumber576570 {&gWindow70CountTextsCC4848[2], 0, 3, 1, 0}; // 原全局：0x00576570。
WindowNumberData gWindowNumber576578 {&gWindow70CountTextsCC4848[3], 0, 3, 1, 0}; // 原全局：0x00576578。
WindowNumberData gWindowNumber576580 {&gWindow70CountTextsCC4848[4], 0, 3, 1, 0}; // 原全局：0x00576580。
std::uint32_t gWindowMouseFlagsD67920 = 0; // 原全局：0x00D67920。

WindowGraphicData gWindowGraphic576EF0 {243048, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576EF0。
WindowGraphicData gWindowGraphic576F08 {30000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576F08。
WindowGraphicData gWindowGraphic576F20 {245042, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576F20。
WindowGraphicData gWindowGraphic576F38 {243030, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576F38。
WindowGraphicData gWindowGraphic576F50 {243033, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00576F50。
WindowTextPrototype gWindowText576F68 {{}, L"請選擇臉部、眼睛及嘴巴。"}; // 原全局：0x00576F68。 // 原常量参考：0x00585D98。
WindowTextPrototype gWindowText576F70 {{}, L"臉部"}; // 原全局：0x00576F70。 // 原常量参考：0x00585D90。
WindowTextPrototype gWindowText576F78 {{}, L"眼睛"}; // 原全局：0x00576F78。 // 原常量参考：0x00585D88。
WindowTextPrototype gWindowText576F80 {{}, L"嘴巴"}; // 原全局：0x00576F80。 // 原常量参考：0x00585D80。
std::int32_t gWindowAnimationImage576F88 = 100000; // 原全局：0x00576F88。

WindowGraphicData gWindowGraphic563080 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00563080。
WindowGraphicData gWindowGraphic563098 {244309, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00563098。
WindowGraphicData gWindowGraphic5630B0 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005630B0。
WindowGraphicData gWindowGraphic5630C8 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005630C8。
WindowNumberData gWindowNumber5630E8 {&gNumberWindowTextCC4D1C, 10, 3, 1, 0}; // 原全局：0x005630E8。

WindowGraphicData gWindowGraphic571018 {244390, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571018。
WindowGraphicData gWindowGraphic571030 {243048, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571030。
WindowGraphicData gWindowGraphic571048 {243027, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571048。
WindowGraphicData gWindowGraphic571060 {243021, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571060。
WindowGraphicData gWindowGraphic571078 {243024, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571078。
WindowGraphicData gWindowGraphic571090 {243143, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571090。
WindowGraphicData gWindowGraphic5710A8 {244391, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005710A8。
WindowGraphicData gWindowGraphic5710C0 {243141, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005710C0。
WindowGraphicData gWindowGraphic5710D8 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005710D8。
WindowGraphicData gWindowGraphic5710F0 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005710F0。
WindowGraphicData gWindowGraphic571108 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571108。
WindowGraphicData gWindowGraphic571120 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571120。
WindowTextPrototype gWindowText571148 { // 原全局：0x00571148。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, cg::text::FromGbkWide(input::LegacyText593850()),
};
WindowNumberData gWindowNumber571150 {&gStorageAmountTextsCB1DF8[0], 0, 3, 1, 0}; // 原全局：0x00571150。
WindowNumberData gWindowNumber571158 {&gStorageAmountTextsCB1DF8[1], 0, 3, 1, 0}; // 原全局：0x00571158。
std::uint32_t gWindowMouseFlagsD67880 = 0; // 原全局：0x00D67880。
std::uint32_t gWindowMouseFlagsD6788C = 0; // 原全局：0x00D6788C。

WindowGraphicData gWindowGraphic5716C8 {245221, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005716C8。
WindowGraphicData gWindowGraphic5716E0 {243048, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005716E0。
WindowGraphicData gWindowGraphic5716F8 {245222, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005716F8。
WindowGraphicData gWindowGraphic571710 {245220, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571710。
WindowGraphicData gWindowGraphic571728 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571728。

WindowGraphicData gWindowGraphic5718A8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005718A8。
WindowGraphicData gWindowGraphic5718C0 {0x3BA6A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005718C0。
WindowGraphicData gWindowGraphic5718D8 {0x3BA6B, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005718D8。
WindowGraphicData gWindowGraphic5718F0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005718F0。
WindowGraphicData gWindowGraphic571908 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571908。
WindowGraphicData gWindowGraphic571920 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571920。
WindowGraphicData gWindowGraphic571938 {0x3B9F7, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571938。
WindowGraphicData gWindowGraphic571950 {0x3BA6C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571950。
WindowGraphicData gWindowGraphic571968 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571968。
WindowGraphicData gWindowGraphic571980 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571980。
WindowTextPrototype gWindowText571998 { // 原全局：0x00571998。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族獸", // 原常量参考：0x00585E88。
};
WindowTextPrototype gWindowTexts5719A0[9] { // 原全局：0x005719A0。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
    {{std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"狀態"}, // 原常量参考：0x00585E80。
};
std::uint32_t gWindowMouseFlagsD67898 = 0; // 原全局：0x00D67898。

WindowGraphicData gWindowGraphic571DF0 {0x3B9FD, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571DF0。
WindowGraphicData gWindowGraphic571E08 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E08。
WindowGraphicData gWindowGraphic571E20 {0x3B56E, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E20。
WindowGraphicData gWindowGraphic571E38 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E38。
WindowGraphicData gWindowGraphic571E50 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E50。
WindowGraphicData gWindowGraphic571E68 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571E68。
WindowGraphicData gWindowGraphic571EB0 {0x3B9F4, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571EB0。
WindowGraphicData gWindowGraphic571EC8 {0x3B9F5, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571EC8。
WindowGraphicData gWindowGraphic571EE0 {0x3B9F2, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00571EE0。
WindowGraphicData gWindowGraphic571EF8 {0x3B9F3, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00571EF8。
WindowTextPrototype gWindowText571F10 { // 原全局：0x00571F10。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"guild", // 原常量参考：0x00585F04。
};
std::uint32_t gWindowMouseFlagsD678A8 = 0; // 原全局：0x00D678A8。

WindowGraphicData gWindowGraphic572940 {0x3BA88, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572940。
WindowGraphicData gWindowGraphic572958 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572958。
WindowGraphicData gWindowGraphic572970 {0x3BA89, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572970。
WindowGraphicData gWindowGraphic572988 {0x3BA8C, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572988。
WindowGraphicData gWindowGraphic5729A0 {0x3B562, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005729A0。
WindowGraphicData gWindowGraphic572AD8 {0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00572AD8。
WindowTextPrototype gWindowText572AF0 { // 原全局：0x00572AF0。
    {std::byte{0x8A}, std::byte{1}, std::byte{0}, std::byte{0}}, cg::text::FromGbkWide(input::LegacyText593850()),
};
std::uint32_t gWindowMouseFlagsD678B8 = 0; // 原全局：0x00D678B8。
std::uint32_t gWindowMouseFlagsD678C4 = 0; // 原全局：0x00D678C4。
std::uint32_t gWindowMouseFlagsD678D0 = 0; // 原全局：0x00D678D0。

WindowGraphicData gWindowGraphic56CCF0 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CCF0。
WindowGraphicData gWindowGraphic56CD08 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD08。
WindowGraphicData gWindowGraphic56CD20 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD20。
WindowGraphicData gWindowGraphic56CD38 {0x3B562, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD38。
WindowGraphicData gWindowGraphic56CD50 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD50。
WindowGraphicData gWindowGraphic56CD68 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0056CD68。
WindowTextPrototype gWindowTexts56CD80[5] { // 原全局：0x0056CD80。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
    {{std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, L"■"}, // 原常量参考：0x00585F64。
};
WindowTextPrototype gWindowText56CDA8 { // 原全局：0x0056CDA8。
    {std::byte{4}, std::byte{1}, std::byte{0}, std::byte{0}}, L"請選擇對象。", // 原常量参考：0x00585F54。
};

WindowGraphicData gWindowGraphic578BE8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578BE8。
WindowGraphicData gWindowGraphic577170 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577170。
WindowGraphicData gWindowGraphic577188 {0x3BB00, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577188。
WindowGraphicData gWindowGraphic5771A0 {0x3BB01, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x005771A0。
WindowGraphicData gWindowGraphic5771B8 {0x3BB03, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005771B8。
WindowGraphicData gWindowGraphic5771E8 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005771E8。
WindowGraphicData gWindowGraphic577200 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577200。
WindowGraphicData gWindowGraphic577218 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577218。
WindowGraphicData gWindowGraphic577248 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577248。
WindowGraphicData gWindowGraphic577260 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577260。
WindowGraphicData gWindowGraphic577278 {0x3C386, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577278。
WindowTextPrototype gWindowText577290 { // 原全局：0x00577290。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"物品", // 原常量参考：0x00585D78。
};
std::uint32_t gWindowMouseFlagsD67930 = 0; // 原全局：0x00D67930。

WindowGraphicData gWindowGraphic577740 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577740。
WindowGraphicData gWindowGraphic577758 {0x3C119, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577758。
WindowGraphicData gWindowGraphic577770 {0x3BB3D, 0, 0, 0, 0, 0x8FFFFFFFu}; // 原全局：0x00577770。
WindowGraphicData gWindowGraphic577788 {0x3C11A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00577788。
WindowTextPrototype gWindowText5777A0 { // 原全局：0x005777A0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"類別",
};

WindowGraphicData gWindowGraphic578C00 {0x3C11B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578C00。
WindowGraphicData gWindowGraphic578C18 {0x3C11C, 0, 0, 0, 0, 0x8FFFFFFFu}; // 原全局：0x00578C18。
WindowGraphicData gWindowGraphic578C30 {0x3C12A, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578C30。
WindowGraphicData gWindowGraphic578C48 {0x3C11D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578C48。
WindowGraphicData gWindowGraphic578C60 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578C60。
WindowGraphicData gWindowGraphic578C78 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578C78。
WindowGraphicData gWindowGraphic578C90 {0x3C120, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00578C90。
WindowTextPrototype gWindowText578CA8 { // 原全局：0x00578CA8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"類別", // 原常量参考：0x00585D70。
};
WindowTextPrototype gWindowText578CB0 { // 原全局：0x00578CB0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"訊息", // 原常量参考：0x00585D64。
};
WindowTextPrototype gWindowText578CB8 { // 原全局：0x00578CB8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"名稱", // 原常量参考：0x00585D5C。
};
WindowTextPrototype gWindowText578CC0 { // 原全局：0x00578CC0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"介紹內容", // 原常量参考：0x00585D54。
};

WindowGraphicData gWindowGraphic5795A8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005795A8。
WindowGraphicData gWindowGraphic579620 {0x3C10F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579620。
WindowGraphicData gWindowGraphic579638 {0x3C10D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579638。
WindowGraphicData gWindowGraphic579650 {0x3BAB1, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x00579650。
WindowGraphicData gWindowGraphic579698 {0x3C0F0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579698。
WindowGraphicData gWindowGraphic5796B0 {0x3BA1B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005796B0。
WindowTextPrototype gWindowText5796C8 { // 原全局：0x005796C8。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"姓名", // 原常量参考：0x00585D4C。
};
WindowTextPrototype gWindowText5796D0 { // 原全局：0x005796D0。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等級", // 原常量参考：0x00585D44。
};
WindowTextPrototype gWindowText5796D8 { // 原全局：0x005796D8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"稱號", // 原常量参考：0x00585D38。
};
WindowTextPrototype gWindowText5796E0 { // 原全局：0x005796E0。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"職業", // 原常量参考：0x00585D28。
};
WindowTextPrototype gWindowText5796E8 { // 原全局：0x005796E8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族", // 原常量参考：0x00585D20。
};
WindowTextPrototype gWindowText5796F8 { // 原全局：0x005796F8。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"出售", // 原常量参考：0x00585D10。
};
WindowTextPrototype gWindowText579700 { // 原全局：0x00579700。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"購買", // 原常量参考：0x00585D08。
};
WindowTextPrototype gWindowText579708 { // 原全局：0x00579708。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"自我介紹", // 原常量参考：0x00585D00。
};
WindowGraphicData gWindowGraphic5799D0 {0x3BA0B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005799D0。
WindowGraphicData gWindowGraphic5799E8 {0x3BA06, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x005799E8。
WindowGraphicData gWindowGraphic579A00 {0x3BA07, 0, 0, 0, 0, 0x80FFFFFFu}; // 原全局：0x00579A00。
WindowGraphicData gWindowGraphic579A78 {0x3BA1B, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579A78。
WindowGraphicData gWindowGraphic579AA8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579AA8。
WindowGraphicData gWindowGraphic579B08 {0x3C123, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579B08。
WindowTextPrototype gWindowText579B20 { // 原全局：0x00579B20。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"收件人", // 原常量参考：0x00585CF4。
};

WindowGraphicData gWindowGraphic579F20 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579F20。
WindowGraphicData gWindowGraphic579F98 {0x3C10F, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579F98。
WindowGraphicData gWindowGraphic579FB0 {0x3C10D, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x00579FB0。
WindowGraphicData gWindowGraphic579FC8 {0x3BAB1, 0, 0, 0, 0, 0x80808080u}; // 原全局：0x00579FC8。
WindowGraphicData gWindowGraphic57A010 {0x3C0F0, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057A010。
WindowTextPrototype gWindowText57A028 { // 原全局：0x0057A028。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"姓名", // 原常量参考：0x00585D4C。
};
WindowTextPrototype gWindowText57A030 { // 原全局：0x0057A030。
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, L"等級", // 原常量参考：0x00585D44。
};
WindowTextPrototype gWindowText57A038 { // 原全局：0x0057A038。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"稱號", // 原常量参考：0x00585D38。
};
WindowTextPrototype gWindowText57A040 { // 原全局：0x0057A040。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"職業", // 原常量参考：0x00585D28。
};
WindowTextPrototype gWindowText57A048 { // 原全局：0x0057A048。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"家族", // 原常量参考：0x00585D20。
};
WindowTextPrototype gWindowText57A058 { // 原全局：0x0057A058。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"出售", // 原常量参考：0x00585D10。
};
WindowTextPrototype gWindowText57A060 { // 原全局：0x0057A060。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"購買", // 原常量参考：0x00585D08。
};
WindowTextPrototype gWindowText57A068 { // 原全局：0x0057A068。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, L"自我介紹", // 原常量参考：0x00585D00。
};

WindowGraphicData gWindowGraphic57B288 {243000, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B288。
WindowGraphicData gWindowGraphic57B2A0 {98891, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B2A0。
WindowGraphicData gWindowGraphic57B2B8 {98888, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B2B8。
WindowGraphicData gWindowGraphic57B2D0 {98885, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B2D0。
WindowGraphicData gWindowGraphic57B2E8 {98870, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B2E8。
WindowGraphicData gWindowGraphic57B300 {98882, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B300。
WindowGraphicData gWindowGraphic57B318 {98873, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B318。
WindowGraphicData gWindowGraphic57B360 {98859, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B360。
WindowGraphicData gWindowGraphic57B378 {98861, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B378。
WindowGraphicData gWindowGraphic57B390 {98854, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B390。
WindowGraphicData gWindowGraphic57B3A8 {243096, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B3A8。
WindowGraphicData gWindowGraphic57B3C0 {243090, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B3C0。
WindowGraphicData gWindowGraphic57B3D8 {243093, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B3D8。
WindowGraphicData gWindowGraphic57B3F0 {98864, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B3F0。
WindowGraphicData gWindowGraphic57B408 {98867, 0, 0, 0, 0, 0xFFFFFFFFu}; // 原全局：0x0057B408。
WindowTextPrototype gWindowText57B420 { // 原全局：0x0057B420。
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}},
    L"ˋˋˋˋ",
};
WindowTextPrototype gWindowText57B428 { // 原全局：0x0057B428。
    {std::byte{0x8C}, std::byte{4}, std::byte{0}, std::byte{0}},
    L"請輸入店名八個字",
};
WindowTextPrototype gWindowText57B430 { // 原全局：0x0057B430。
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}},
    L"請輸入店舖描述",
};
WindowNumberData gWindowNumber57B450 {&gStallMoneyText57B440, 0, 3, 1, 0}; // 原全局：0x0057B450。
std::int32_t gWindowAnimationImage57B458 = 100000; // 原全局：0x0057B458。
std::uint32_t gWindowMouseFlagsD67A10 = 0; // 原全局：0x00D67A10。

// 同一4A4210在原表也用于release；仅适配未消费返回值，不转换函数指针。
// WCL01：窗口release家族丢弃原4A4210常量1；真实七类资源回收留在FreeWindow，非此适配。
// 原函数：0x004A4210。
void WindowRuntime_ReleaseDefault()
{
    static_cast<void>(WindowRuntime_ReturnOne(0));
}

// 原型记录下标；窗口36/37共用Dialog，窗口38起不能直接用WindowId索引。
enum WindowPrototypeIndex : std::size_t {
    WindowPrototypeIndex_None = 0,
    WindowPrototypeIndex_StatusBar = 1,
    WindowPrototypeIndex_CompactStatusBar = 2,
    WindowPrototypeIndex_CollapsedStatusBar = 3,
    WindowPrototypeIndex_ShortcutBar = 4,
    WindowPrototypeIndex_CollapsedShortcutBar = 5,
    WindowPrototypeIndex_BottomBar = 6,
    WindowPrototypeIndex_PlayerProfile = 7,
    WindowPrototypeIndex_PlayerAttributes = 8,
    WindowPrototypeIndex_PlayerTitles = 9,
    WindowPrototypeIndex_Skill = 10,
    WindowPrototypeIndex_Craft = 11,
    WindowPrototypeIndex_Gather = 12,
    WindowPrototypeIndex_SkillResult = 13,
    WindowPrototypeIndex_Inventory = 14,
    WindowPrototypeIndex_PetList = 15,
    WindowPrototypeIndex_PetDetail = 16,
    WindowPrototypeIndex_PetAttributes = 17,
    WindowPrototypeIndex_PetSkills = 18,
    WindowPrototypeIndex_AlbumList = 19,
    WindowPrototypeIndex_AlbumDetail = 20,
    WindowPrototypeIndex_Contacts = 21,
    WindowPrototypeIndex_MailCompose = 22,
    WindowPrototypeIndex_PetMailCompose = 23,
    WindowPrototypeIndex_Settings = 24,
    WindowPrototypeIndex_Hotkeys = 25,
    WindowPrototypeIndex_BattleCommand = 26,
    WindowPrototypeIndex_BattleItems = 27,
    WindowPrototypeIndex_BattlePets = 28,
    WindowPrototypeIndex_Spectator = 29,
    WindowPrototypeIndex_BattleResult = 30,
    WindowPrototypeIndex_DpResult = 31,
    WindowPrototypeIndex_BattleStateNotice = 32,
    WindowPrototypeIndex_MiniMap = 33,
    WindowPrototypeIndex_Emote = 34,
    WindowPrototypeIndex_Chat = 35,
    WindowPrototypeIndex_Dialog = 36, // 窗口36/37共用。
    WindowPrototypeIndex_ItemStorage = 37,
    WindowPrototypeIndex_TimerNotice = 38,
    WindowPrototypeIndex_MailRead = 39,
    WindowPrototypeIndex_Bank = 40,
    WindowPrototypeIndex_Quantity = 41,
    WindowPrototypeIndex_Trade = 42,
    WindowPrototypeIndex_TradeTarget = 43,
    WindowPrototypeIndex_TextTarget = 44,
    WindowPrototypeIndex_AttributeTarget = 45,
    WindowPrototypeIndex_Confirmation = 46,
    WindowPrototypeIndex_GuildDetails = 47,
    WindowPrototypeIndex_GroupSelect = 48,
    WindowPrototypeIndex_MessageBoard = 49,
    WindowPrototypeIndex_TextList = 50,
    WindowPrototypeIndex_ShopChoice = 51,
    WindowPrototypeIndex_Shop = 52,
    WindowPrototypeIndex_PetSkillChoice = 53,
    WindowPrototypeIndex_GuildRank = 54,
    WindowPrototypeIndex_Appearance = 55,
    WindowPrototypeIndex_CompactChat = 56,
    WindowPrototypeIndex_OwnProfile = 57,
    WindowPrototypeIndex_OwnProfileCategory = 58,
    WindowPrototypeIndex_ProfileCategory = 59,
    WindowPrototypeIndex_ProfileSearch = 60,
    WindowPrototypeIndex_ProfileView = 61,
    WindowPrototypeIndex_MiniMail = 62,
    WindowPrototypeIndex_ProfileReply = 63,
    WindowPrototypeIndex_Blacklist = 64,
    WindowPrototypeIndex_AdventureLog = 65,
    WindowPrototypeIndex_OwnStall = 66,
    WindowPrototypeIndex_StallBrowse = 67,
    WindowPrototypeIndex_CrystalShop = 68,
    WindowPrototypeIndex_CrystalExchangeShop = 69,
    WindowPrototypeIndex_Count = 70,
};

// 注释为窗口编号和原型原地址；36/37只有一个可变记录。
std::array<WindowPrototype, WindowPrototypeIndex_Count> gWindowPrototypeRecords {{
    {0, 0, 0, 0, 0, 0, 0x00000000u, 0, nullptr, nullptr, nullptr, nullptr}, // 0: 54EFB8
    {0, 5, 0, 0, 197, 40, 0x80000080u, 24, gWindowElements55F0A0, &WindowStatusBar_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 1: 54E2A0
    {0, 5, 0, 0, 197, 32, 0x80000080u, 7, gWindowElements55F3A0, &WindowCompactStatusBar_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 2: 54E2D0
    {0, 5, 0, 0, 19, 34, 0x80000080u, 2, gWindowElements55F480, &WindowRuntime_ReturnOne, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 3: 54E300
    {0, 5, 437, 0, 210, 28, 0x80000080u, 11, gWindowElements55ED60, &WindowShortcutBar_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 4: 54E240
    {0, 5, 626, 0, 14, 33, 0x80000080u, 2, gWindowElements55EEC0, &WindowShortcutBar_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 5: 54E270
    {2, 5, 0, 452, 640, 30, 0x80010101u, 14, gWindowElements55F5C0, &WindowBottomBar_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 6: 54E330
    {0, 4, 15, 97, 315, 261, 0x80000080u, 73, gWindowElements55FD60, &WindowPlayerProfile_Update, &WindowRuntime_DrawDefault, &WindowPlayerProfile_Release}, // 7: 54E360
    {0, 4, 15, 97, 315, 261, 0x80000080u, 43, gWindowElements560840, &WindowPlayerAttributes_Update, &WindowRuntime_DrawDefault, &WindowPlayerAttributes_Release}, // 8: 54E390
    {0, 4, 15, 97, 315, 261, 0x80000080u, 37, gWindowElements560F50, &WindowPlayerTitles_Update, &WindowRuntime_DrawDefault, &WindowPlayerTitles_Release}, // 9: 54E3C0
    {0, 4, 25, 122, 241, 258, 0x80010101u, 30, gWindowElements561B78, &WindowSkill_Update, &WindowSkill_Draw, &WindowSkill_Release}, // 10: 54E420
    {0, 4, 37, 102, 563, 239, 0x80010101u, 31, gWindowElements5620E0, &WindowCraft_Update, &WindowCraft_Draw, &WindowCraft_Release}, // 11: 54E450
    {0, 4, 383, 118, 198, 232, 0x80010101u, 15, gWindowElements562620, &WindowGather_Update, &WindowRuntime_DrawDefault, &WindowGather_Release}, // 12: 54E480
    {0, 4, 383, 118, 198, 232, 0x80010101u, 14, gWindowElements562930, &WindowSkillResult_Update, &WindowSkillResult_Draw, &WindowSkillResult_Release}, // 13: 54E4B0
    {0, 4, 340, 120, 278, 259, 0x80010101u, 24, gWindowElements562CD8, &WindowInventory_Update, &WindowInventory_Draw, &WindowInventory_Release}, // 14: 54E4E0
    {0, 4, 3, 85, 323, 281, 0x80010101u, 259, gWindowElements563500, &WindowPetList_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 15: 54E540
    {0, 4, 365, 83, 245, 306, 0x80010101u, 66, gWindowElements5659C0, &WindowPetDetail_Update, &WindowRuntime_DrawDefault, &WindowPetDetail_Release}, // 16: 54E570
    {0, 4, 365, 83, 245, 306, 0x80010101u, 46, gWindowElements566200, &WindowPetAttributes_Update, &WindowRuntime_DrawDefault, &WindowPetAttributes_Release}, // 17: 54E5A0
    {0, 4, 365, 83, 245, 306, 0x80010101u, 51, gWindowElements566810, &WindowPetSkills_Update, &WindowPetSkills_Draw, &WindowPetSkills_Release}, // 18: 54E5D0
    {0, 4, 38, 95, 225, 220, 0x80010101u, 71, gWindowElements567028, &WindowAlbumList_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 19: 54E600
    {0, 4, 357, 90, 256, 289, 0x80010101u, 82, gWindowElements567AE8, &WindowAlbumDetail_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 20: 54E630
    {0, 4, 2, 64, 331, 381, 0x80010101u, 111, gWindowElements56DB38, &WindowContacts_Update, &WindowContacts_Draw, &WindowRuntime_ReleaseDefault}, // 21: 54E990
    {0, 4, 383, 81, 211, 210, 0x80010101u, 9, gWindowElements5701F8, &WindowMailCompose_Update, &WindowRuntime_DrawDefault, &WindowMailCompose_Release}, // 22: 54EA20
    {0, 4, 383, 81, 211, 259, 0x80010101u, 13, gWindowElements570318, &WindowPetMailCompose_Update, &WindowRuntime_DrawDefault, &WindowMailCompose_Release}, // 23: 54EA50
    {0, 4, 409, 100, 207, 222, 0x80010101u, 32, gWindowElements5689C8, &WindowSettings_Update, &WindowSettings_Draw, &WindowSettings_Release}, // 24: 54E660
    {0, 4, 15, 117, 286, 233, 0x80010101u, 25, gWindowElements568F78, &WindowHotkeys_Update, &WindowHotkeys_Draw, &WindowRuntime_ReleaseDefault}, // 25: 54E690
    {2, 5, 341, 0, 299, 69, 0x80010101u, 14, gWindowElements56B710, &WindowBattleCommand_Update, &WindowBattleCommand_Draw, &WindowRuntime_ReleaseDefault}, // 26: 54E720
    {0, 4, 100, 100, 276, 253, 0x80010101u, 22, gWindowElements56C430, &WindowBattleItems_Update, &WindowBattleItems_Draw, &WindowRuntime_ReleaseDefault}, // 27: 54E7E0
    {0, 4, 100, 100, 356, 223, 0x80010101u, 8, gWindowElements56C800, &WindowBattlePets_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 28: 54E810
    {2, 3, 492, 0, 148, 108, 0x80010101u, 2, gWindowElements56B8D8, &WindowRuntime_ReturnOne, &WindowRuntime_DrawCompactFrame, &WindowRuntime_ReleaseDefault}, // 29: 54E750
    {1, 3, 40, 40, 490, 301, 0x80010101u, 66, gWindowElements56B9B8, &WindowBattleResult_Update, &WindowBattleResult_Draw, &WindowRuntime_ReleaseDefault}, // 30: 54E780
    {3, 3, 245, 170, 150, 140, 0x80010101u, 4, gWindowElements56C2C8, &WindowDpResult_Update, &WindowRuntime_DrawLargeFrame, &WindowRuntime_ReleaseDefault}, // 31: 54E7B0
    {2, 3, 224, 162, 192, 96, 0x80010101u, 2, gWindowElements56C9B8, &WindowBattleStateNotice_Update, &WindowRuntime_DrawCompactFrame, &WindowRuntime_ReleaseDefault}, // 32: 54E840
    {0, 4, 353, 55, 184, 21, 0x80010101u, 7, gWindowElements56AF50, &WindowMiniMap_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 33: 54E6C0
    {0, 4, 575, 97, 61, 260, 0x80010101u, 20, gWindowElements56B2B8, &WindowEmote_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 34: 54E6F0
    {0, 4, 364, 428, 640, 25, 0x80010101u, 20, gWindowElements570CB0, &WindowChat_Update, &WindowChat_Draw, &WindowRuntime_ReleaseDefault}, // 35: 54EAB0
    {2, 3, 0, 0, 0, 30, 0x80010101u, 19, gWindowElements56CA88, &WindowDialog_Update, &WindowDialog_Draw, &WindowDialog_Release}, // 36: 54E870
    {0, 4, 300, 82, 567, 339, 0x80010101u, 6, gWindowElements571740, &WindowItemStorage_Update, &WindowItemStorage_Draw, &WindowItemStorage_Release}, // 38: 54EB40
    {3, 3, 196, 180, 256, 96, 0x80010101u, 1, gWindowElements56CCD0, &WindowTimerNotice_Update, &WindowRuntime_DrawCompactFrame, &WindowRuntime_ReleaseDefault}, // 39: 54E8A0
    {0, 4, 383, 71, 211, 206, 0x80010101u, 21, gWindowElements56FD70, &WindowMailRead_Update, &WindowMailRead_Draw, &WindowRuntime_ReleaseDefault}, // 40: 54E9F0
    {0, 4, 20, 25, 567, 339, 0x80010101u, 38, gWindowElements571160, &WindowBank_Update, &WindowBank_Draw, &WindowBank_Release}, // 41: 54EB10
    {0, 4, 100, 100, 126, 136, 0x80010101u, 8, gWindowElements5630F0, &WindowQuantity_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 42: 54E510
    {0, 4, 20, 25, 567, 339, 0x80010101u, 20, gWindowElements572AF8, &WindowTrade_Update, &WindowTrade_Draw, &WindowTrade_Release}, // 43: 54EBD0
    {0, 4, 160, 125, 320, 192, 0x80010101u, 19, gWindowElements56CDB0, &WindowTradeTarget_Update, &WindowTradeTarget_Draw, &WindowRuntime_ReleaseDefault}, // 44: 54E8D0
    {0, 4, 160, 125, 320, 192, 0x80010101u, 19, gWindowElements56D0B0, &WindowTextTarget_Update, &WindowTextTarget_Draw, &WindowTextTarget_Release}, // 45: 54E900
    {0, 4, 60, 105, 520, 243, 0x80010101u, 34, gWindowElements56D3E8, &WindowAttributeTarget_Update, &WindowRuntime_DrawLargeFrame, &WindowAttributeTarget_Release}, // 46: 54E930
    {0, 4, 209, 159, 222, 162, 0x80010101u, 34, gWindowElements5706D0, &WindowConfirmation_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 47: 54EA80
    {0, 4, 100, 60, 317, 325, 0x80010101u, 76, gWindowElements571F18, &WindowGuildDetails_Update, &WindowGuildDetails_Draw, &WindowRuntime_ReleaseDefault}, // 48: 54EBA0
    {0, 4, 68, 84, 280, 317, 0x80010101u, 37, gWindowElements572F90, &WindowGroupSelect_Update, &WindowGroupSelect_Draw, &WindowGroupSelect_Release}, // 49: 54EC00
    {0, 4, 5, 20, 634, 424, 0x80010101u, 36, gWindowElements5735C8, &WindowMessageBoard_Update, &WindowRuntime_DrawDefault, &WindowMessageBoard_Release}, // 50: 54EC30
    {0, 4, 100, 100, 231, 305, 0x80010101u, 26, gWindowElements5719E8, &WindowTextList_Update, &WindowTextList_Draw, &WindowTextList_Release}, // 51: 54EB70
    {2, 4, 209, 117, 222, 216, 0x80010101u, 3, gWindowElements56D838, &WindowShopChoice_Update, &WindowShopChoice_Draw, &WindowShopChoice_Release}, // 52: 54E960
    {0, 4, 10, 30, 549, 366, 0x80010101u, 66, gWindowElements573D80, &WindowShop_Update, &WindowShop_Draw, &WindowShop_Release}, // 53: 54EC60
    {0, 4, 10, 30, 549, 366, 0x80010101u, 107, gWindowElements5748E8, &WindowPetSkillChoice_Update, &WindowPetSkillChoice_Draw, &WindowPetSkillChoice_Release}, // 54: 54EC90
    {0, 4, 100, 100, 243, 206, 0x80000080u, 29, gWindowElements56F860, &WindowGuildRank_Update, &WindowGuildRank_Draw, &WindowRuntime_ReleaseDefault}, // 55: 54E9C0
    {0, 4, 96, 108, 448, 240, 0x80010101u, 15, gWindowElements576F90, &WindowAppearance_Update, &WindowAppearance_Draw, &WindowAppearance_Release}, // 56: 54ED20
    {0, 4, 0, 428, 0, 0, 0x80010101u, 2, gWindowElements570FD8, &WindowCompactChat_Update, &WindowChat_Draw, &WindowRuntime_ReleaseDefault}, // 57: 54EAE0
    {0, 4, 15, 97, 315, 261, 0x80000080u, 34, gWindowElements561558, &WindowOwnProfile_Update, &WindowRuntime_DrawDefault, &WindowOwnProfile_Release}, // 58: 54E3F0
    {0, 4, 38, 95, 225, 220, 0x80010101u, 32, gWindowElements577298, &WindowOwnProfileCategory_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 59: 54ED60
    {0, 4, 5, 20, 634, 424, 0x80000080u, 162, gWindowElements5777A8, &WindowProfileCategory_Update, &WindowRuntime_DrawDefault, &WindowProfileCategory_Release}, // 60: 54ED90
    {0, 4, 5, 20, 305, 398, 0x80000080u, 71, gWindowElements578CC8, &WindowProfileSearch_Update, &WindowRuntime_DrawDefault, &WindowProfileSearch_Release}, // 61: 54EDC0
    {0, 4, 5, 20, 317, 250, 0x80000080u, 22, gWindowElements579710, &WindowProfileView_Update, &WindowRuntime_DrawDefault, &WindowProfileView_Release}, // 62: 54EDF0
    {0, 4, 383, 81, 232, 210, 0x80000080u, 16, gWindowElements579B28, &WindowMiniMail_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 63: 54EE20
    {0, 4, 15, 97, 317, 250, 0x80000080u, 21, gWindowElements57A070, &WindowProfileReply_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 64: 54EE50
    {0, 4, 38, 95, 251, 210, 0x80010101u, 71, gWindowElements57A450, &WindowBlacklist_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 65: 54EE80
    {0, 4, 15, 97, 465, 321, 0x80000080u, 25, gWindowElements57AEC0, &WindowAdventureLog_Update, &WindowRuntime_DrawDefault, &WindowRuntime_ReleaseDefault}, // 66: 54EEB0
    {0, 4, 10, 30, 549, 366, 0x80000080u, 48, gWindowElements57B508, &WindowStall_Update, &WindowStall_Draw, &WindowStall_Release}, // 67: 54EF58
    {0, 4, 10, 30, 549, 366, 0x80000080u, 43, gWindowElements57BB08, &WindowStall_Update, &WindowStall_Draw, &WindowStall_Release}, // 68: 54EF88
    {0, 4, 10, 30, 549, 366, 0x80010101u, 70, gWindowElements575988, &WindowCrystalShop_Update, &WindowCrystalShop_Draw, &WindowCrystalShop_Release}, // 69: 54ECC0
    {0, 4, 10, 30, 549, 366, 0x80010101u, 70, gWindowElements576588, &WindowCrystalExchangeShop_Update, &WindowCrystalExchangeShop_Draw, &WindowCrystalExchangeShop_Release}, // 70: 54ECF0
}};

} // namespace

WindowPrototype& gWindowPrototype54E2A0 = gWindowPrototypeRecords[WindowPrototypeIndex_StatusBar]; // 原全局：0x0054E2A0。
WindowTextPrototype gWindowText572F58 { // 原全局：0x00572F58。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"持有名片＆上線的人員",
};
WindowTextPrototype gWindowText572F60 { // 原全局：0x00572F60。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"同家族內上線的人員",
};
WindowTextPrototype gWindowText572F68 { // 原全局：0x00572F68。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"同家族全部的人員",
};
WindowTextPrototype gWindowText572F78 { // 原全局：0x00572F78。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"交換過名片的人",
};
WindowTextPrototype gWindowText572F80 { // 原全局：0x00572F80。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"家族成員",
};
WindowTextPrototype gWindowText572F88 { // 原全局：0x00572F88。
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}},
    L"家族稱號",
};
WindowPrototype& gWindowPrototype54E240 = gWindowPrototypeRecords[WindowPrototypeIndex_ShortcutBar]; // 原全局：0x0054E240。
WindowPrototype& gWindowPrototype54E330 = gWindowPrototypeRecords[WindowPrototypeIndex_BottomBar]; // 原全局：0x0054E330。

WindowElementPrototype gWindowElements55ED60[11] { // 原全局：0x0055ED60。
    {WindowElementType_Graphic, 14, 3, 20, 20, 1, {.graphic = &gWindowGraphic55EC70}, &WindowShortcutBar_HandleButtons},
    {WindowElementType_Graphic, 35, 3, 20, 20, 1, {.graphic = &gWindowGraphic55EC88}, &WindowShortcutBar_HandleButtons},
    {WindowElementType_Graphic, 56, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ECA0}, &WindowShortcutBar_HandleButtons},
    {WindowElementType_Graphic, 77, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ECB8}, &WindowShortcutBar_HandleButtons},
    {WindowElementType_Graphic, 98, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ECD0}, &WindowShortcutBar_HandleButtons},
    {WindowElementType_Graphic, 119, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ECE8}, &WindowShortcutBar_HandleButtons},
    {WindowElementType_Graphic, 140, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ED00}, &WindowShortcutBar_HandleButtons},
    {WindowElementType_Graphic, 161, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ED18}, &WindowShortcutBar_HandleButtons},
    {WindowElementType_Graphic, 182, 3, 20, 20, 1, {.graphic = &gWindowGraphic55ED30}, &WindowShortcutBar_HandleButtons},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55EC58}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 0, 0, 14, 33, 1, {.mouseFlags = &gWindowMouseFlagsD67620}, &WindowShortcutBar_ButtonCollapse},
};

WindowElementPrototype gWindowElements55EEC0[2] { // 原全局：0x0055EEC0。
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55ED48}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 0, 0, 14, 33, 1, {.mouseFlags = &gWindowMouseFlagsD67620}, &WindowShortcutBar_ButtonExpand},
};

WindowElementPrototype gWindowElements56AF50[7] { // 原全局：0x0056AF50。
    {WindowElementType_Graphic, 165, 4, 11, 11, 1, {.graphic = &gWindowGraphic56AEF0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Text, 51, 21, 0, 0, 1, {.text = &gWindowText56AF38}, &WindowMiniMap_RefreshPlayerX},
    {WindowElementType_Text, 159, 21, 0, 0, 1, {.text = &gWindowText56AF40}, &WindowMiniMap_RefreshPlayerY},
    {WindowElementType_Text, 160, 138, 0, 0, 1, {.text = &gWindowText56AF48}, &WindowElement_NoOp},
    {WindowElementType_EventArea, -19, 81, 0, 0, 1, {}, &WindowMiniMap_DrawMap},
    {WindowElementType_Graphic, -16, 0, 0, 0, 1, {.graphic = &gWindowGraphic56AF08}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 4, 16, 0, 0, 1, {.graphic = &gWindowGraphic56AF20}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements56B2B8[20] { // 原全局：0x0056B2B8。
    {WindowElementType_Graphic, 5, 25, 23, 23, 1, {.graphic = &gWindowGraphic56B0D8}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 50, 23, 23, 1, {.graphic = &gWindowGraphic56B0F0}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 75, 23, 23, 1, {.graphic = &gWindowGraphic56B108}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 100, 23, 23, 1, {.graphic = &gWindowGraphic56B120}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 125, 23, 23, 1, {.graphic = &gWindowGraphic56B138}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 150, 23, 23, 1, {.graphic = &gWindowGraphic56B150}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 175, 23, 23, 1, {.graphic = &gWindowGraphic56B168}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 200, 23, 23, 1, {.graphic = &gWindowGraphic56B180}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 5, 225, 23, 23, 1, {.graphic = &gWindowGraphic56B198}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 25, 23, 23, 1, {.graphic = &gWindowGraphic56B1B0}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 50, 23, 23, 1, {.graphic = &gWindowGraphic56B1C8}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 75, 23, 23, 1, {.graphic = &gWindowGraphic56B1E0}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 100, 23, 23, 1, {.graphic = &gWindowGraphic56B1F8}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 125, 23, 23, 1, {.graphic = &gWindowGraphic56B210}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 150, 23, 23, 1, {.graphic = &gWindowGraphic56B228}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 175, 23, 23, 1, {.graphic = &gWindowGraphic56B240}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 200, 23, 23, 1, {.graphic = &gWindowGraphic56B258}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 29, 225, 23, 23, 1, {.graphic = &gWindowGraphic56B270}, &WindowEmote_ButtonEmote},
    {WindowElementType_Graphic, 45, 7, 11, 11, 1, {.graphic = &gWindowGraphic56B288}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic56B2A0}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements570CB0[20] { // 原全局：0x00570CB0。
    {WindowElementType_Input, 0, 0, 0, 0, 1, {}, &WindowChat_HandleInput},
    {WindowElementType_InputCaret, 0, 0, 0, 0, 1, {}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 0, 0, 0, 0, 1, {}, &WindowChat_Scroll},
    {WindowElementType_Graphic, 7, 3, 18, 16, 1, {.graphic = &gWindowGraphic570BA0}, &WindowChat_HandleButtons},
    {WindowElementType_Graphic, 27, 3, 18, 16, 1, {.graphic = &gWindowGraphic570B40}, &WindowChat_HandleButtons},
    {WindowElementType_Graphic, 48, 3, 18, 16, 1, {.graphic = &gWindowGraphic570B58}, &WindowChat_HandleButtons},
    {WindowElementType_Graphic, 68, 3, 18, 16, 1, {.graphic = &gWindowGraphic570B70}, &WindowChat_HandleButtons},
    {WindowElementType_Graphic, 87, 3, 18, 16, 1, {.graphic = &gWindowGraphic570B88}, &WindowChat_HandleButtons},
    {WindowElementType_Graphic, 0, 0, 11, 14, 1, {.graphic = &gWindowGraphic570C00}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 239, 104, 11, 100, 1, {.mouseFlags = &gWindowMouseFlagsD67870}, &WindowChat_DragScroll},
    {WindowElementType_Graphic, 0, 0, 14, 534, 1, {.graphic = &gWindowGraphic570C18}, &WindowChat_HandleButtons},
    {WindowElementType_Graphic, 0, 0, 14, 16, 1, {.graphic = &gWindowGraphic570C78}, &WindowChat_HandleButtons},
    {WindowElementType_Graphic, 0, 0, 14, 16, 1, {.graphic = &gWindowGraphic570C90}, &WindowChat_HandleButtons},
    {WindowElementType_Graphic, 0, 0, 14, 16, 1, {.graphic = &gWindowGraphic570C30}, &WindowChat_HandleButtons},
    {WindowElementType_Graphic, 0, 0, 14, 16, 1, {.graphic = &gWindowGraphic570C48}, &WindowChat_HandleButtons},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic570B10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 454, 0, 0, 0, 1, {.graphic = &gWindowGraphic570B28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, -370, 0, 0, 1, {.graphic = &gWindowGraphic570BD0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, -386, 0, 0, 1, {.graphic = &gWindowGraphic570BE8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 1, 1, 0, 0, 1, {.graphic = &gWindowGraphic570C60}, &WindowChat_HandleButtons},
};

WindowElementPrototype gWindowElements570FD8[2] { // 原全局：0x00570FD8。
    {WindowElementType_Input, 0, 0, 0, 0, 1, {}, &WindowChat_HandleInput},
    {WindowElementType_InputCaret, 0, 0, 0, 0, 1, {}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements57A450[71] { // 原全局：0x0057A450。
    {WindowElementType_Graphic, 206, 8, 11, 11, 1, {.graphic = &gWindowGraphic57A310}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 206, 41, 0, 14, 1, {.graphic = &gWindowGraphic57A388}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 205, 41, 11, 134, 1, {.mouseFlags = &gWindowMouseFlagsD679B8}, &WindowBlacklist_DragScroll},
    {WindowElementType_Graphic, 206, 31, 11, 11, 1, {.graphic = &gWindowGraphic57A3A0}, &WindowBlacklist_HandleButtons},
    {WindowElementType_Graphic, 206, 174, 11, 11, 1, {.graphic = &gWindowGraphic57A3B8}, &WindowBlacklist_HandleButtons},
    {WindowElementType_Graphic, 40, 189, 20, 20, 1, {.graphic = &gWindowGraphic57A3E8}, &WindowBlacklist_HandleButtons},
    {WindowElementType_Graphic, 60, 189, 20, 20, 1, {.graphic = &gWindowGraphic57A400}, &WindowBlacklist_HandleButtons},
    {WindowElementType_Graphic, 100, 189, 38, 25, 1, {.graphic = &gWindowGraphic57A418}, &WindowBlacklist_HandleButtons},
    {WindowElementType_Graphic, 155, 189, 38, 25, 1, {.graphic = &gWindowGraphic57A430}, &WindowBlacklist_HandleButtons},
    {WindowElementType_Graphic, 19, 31, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 31, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 31, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 47, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 47, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 47, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 63, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 63, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 63, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 79, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 79, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 79, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 95, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 95, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 95, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 111, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 111, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 111, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 127, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 127, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 127, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 143, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 143, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 143, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 159, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 159, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 159, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 175, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 175, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 175, 0, 0, 1, {.graphic = &gWindowGraphic57A370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 31, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 47, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 63, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 79, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 95, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 111, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 127, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 143, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 159, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 175, 0, 0, 1, {.graphic = &gWindowGraphic57A3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 29, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 45, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 61, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 77, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 93, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 109, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 125, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 141, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 157, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 173, 0, 0, 1, {.text = &gWindowText57A448}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 28, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 44, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 60, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 76, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 92, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 108, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 124, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 140, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 156, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 15, 172, 186, 16, 1, {.graphic = &gWindowGraphic57A358}, &WindowBlacklist_EditRow},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic57A328}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic57A340}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements57AEC0[25] { // 原全局：0x0057AEC0。
    {WindowElementType_Graphic, 419, 54, 0, 14, 1, {.graphic = &gWindowGraphic57AE68}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 419, 54, 11, 221, 1, {.mouseFlags = &gWindowMouseFlagsD679C8}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 419, 43, 11, 11, 1, {.graphic = &gWindowGraphic57AE80}, &WindowAdventureLog_HandleNavigation},
    {WindowElementType_Graphic, 419, 275, 11, 11, 1, {.graphic = &gWindowGraphic57AE98}, &WindowAdventureLog_HandleNavigation},
    {WindowElementType_Text, 150, 286, 0, 0, 1, {.text = &gWindowText57AEB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 434, 8, 12, 12, 1, {.graphic = &gWindowGraphic57ADD8}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 380, 283, 18, 18, 1, {.graphic = &gWindowGraphic57ADF0}, &WindowAdventureLog_HandleNavigation},
    {WindowElementType_Graphic, 360, 283, 18, 18, 1, {.graphic = &gWindowGraphic57AE08}, &WindowAdventureLog_HandleNavigation},
    {WindowElementType_Graphic, 60, 284, 0, 0, 1, {.graphic = &gWindowGraphic57AE20}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic57AE38}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic57AE50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 45, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 61, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 77, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 93, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 109, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 125, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 141, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 157, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 173, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 189, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 205, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 221, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 237, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 253, 0, 0, 1, {.text = &gWindowText57AEB8}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements55F0A0[24] { // 原全局：0x0055F0A0。
    {WindowElementType_Number, 20, 19, 0, 0, 1, {.number = &gWindowNumber55F090}, &WindowElement_NoOp},
    {WindowElementType_Number, 20, 29, 0, 0, 1, {.number = &gWindowNumber55F098}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 63, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF18}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 82, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 101, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF48}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 120, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF60}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 139, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 158, 3, 14, 14, 1, {.graphic = &gWindowGraphic55EF90}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 145, 25, 0, 0, 1, {.graphic = &gWindowGraphic55F008}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 158, 25, 0, 0, 1, {.graphic = &gWindowGraphic55F020}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 25, 0, 0, 1, {.graphic = &gWindowGraphic55F038}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 6, 42, 36, 20, 1, {.graphic = &gWindowGraphic55EFA8}, &WindowStatusBar_HandleHintButton},
    {WindowElementType_Graphic, 51, 41, 36, 20, 1, {.graphic = &gWindowGraphic55EFC0}, &WindowStatusBar_HandleHintButton},
    {WindowElementType_Graphic, 83, 41, 36, 20, 1, {.graphic = &gWindowGraphic55EFD8}, &WindowStatusBar_HandleHintButton},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55EF00}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 3, 4, 0, 0, 1, {.graphic = &gWindowGraphic55EFF0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 175, 0, 15, 33, 1, {}, &WindowStatusBar_ButtonCompact},
    {WindowElementType_MouseArea, 63, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons},
    {WindowElementType_MouseArea, 82, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons},
    {WindowElementType_MouseArea, 101, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons},
    {WindowElementType_MouseArea, 120, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons},
    {WindowElementType_MouseArea, 139, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons},
    {WindowElementType_MouseArea, 158, 3, 14, 14, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowStatusBar_HandleOptionButtons},
    {WindowElementType_EventArea, 0, 40, 120, 30, 1, {}, &WindowStatusBar_HandleHover},
};

WindowElementPrototype gWindowElements55F3A0[7] { // 原全局：0x0055F3A0。
    {WindowElementType_Number, 20, 2, 0, 0, 1, {.number = &gWindowNumber55F090}, &WindowElement_NoOp},
    {WindowElementType_Number, 20, 12, 0, 0, 1, {.number = &gWindowNumber55F098}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 146, 9, 0, 0, 1, {.graphic = &gWindowGraphic55F008}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 158, 9, 0, 0, 1, {.graphic = &gWindowGraphic55F020}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 9, 0, 0, 1, {.graphic = &gWindowGraphic55F038}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F050}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 181, 0, 16, 37, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowCompactStatusBar_ButtonCollapse},
};

WindowElementPrototype gWindowElements55F480[2] { // 原全局：0x0055F480。
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F068}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 0, 0, 19, 34, 1, {.mouseFlags = &gWindowMouseFlagsD67650}, &WindowCollapsedStatusBar_ButtonExpand},
};

WindowElementPrototype gWindowElements572F90[37] { // 原全局：0x00572F90。
    {WindowElementType_Graphic, 239, 9, 11, 11, 1, {.graphic = &gWindowGraphic572E38}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 239, 88, 11, 11, 1, {.graphic = &gWindowGraphic572E50}, &WindowGroupSelect_HandleScrollButtons},
    {WindowElementType_Graphic, 239, 275, 11, 11, 1, {.graphic = &gWindowGraphic572E80}, &WindowGroupSelect_HandleScrollButtons},
    {WindowElementType_Graphic, 239, 99, 10, 14, 1, {.graphic = &gWindowGraphic572E68}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 99, 10, 177, 1, {.graphic = &gWindowGraphicD678E0}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_EventArea, 0, 0, 280, 317, 1, {}, &WindowGroupSelect_Scroll},
    {WindowElementType_Graphic, 20, 288, 18, 18, 1, {.graphic = &gWindowGraphic572E98}, &WindowGroupSelect_HandlePageButtons},
    {WindowElementType_Graphic, 60, 288, 18, 18, 1, {.graphic = &gWindowGraphic572EB0}, &WindowGroupSelect_HandlePageButtons},
    {WindowElementType_Graphic, 111, 30, 18, 18, 1, {.graphic = &gWindowGraphic572E98}, &WindowGroupSelect_TabCategory},
    {WindowElementType_Graphic, 218, 30, 18, 18, 1, {.graphic = &gWindowGraphic572EB0}, &WindowGroupSelect_TabCategory},
    {WindowElementType_Text, 131, 33, 0, 0, 1, {.text = &gWindowText572F78}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 57, 0, 0, 1, {.text = &gWindowText572F58}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 93, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 122, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 151, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 180, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 209, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 238, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 267, 0, 0, 1, {.text = &gWindowText572F70}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 55, 19, 19, 1, {.graphic = &gWindowGraphic572F28}, &WindowGroupSelect_ButtonSelectAll},
    {WindowElementType_Graphic, 214, 91, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 214, 120, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 214, 149, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 214, 178, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 214, 207, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 214, 236, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 214, 265, 19, 19, 1, {.graphic = &gWindowGraphic572F40}, &WindowGroupSelect_SelectRow},
    {WindowElementType_Graphic, 16, 50, 0, 0, 1, {.graphic = &gWindowGraphic572F10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 86, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 115, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 144, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 173, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 202, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 231, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 260, 0, 0, 1, {.graphic = &gWindowGraphic572EC8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic572E20}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 10, 25, 0, 0, 1, {.graphic = &gWindowGraphic572EF8}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements56DB38[111] { // 原全局：0x0056DB38。
    {WindowElementType_Graphic, 259, 45, 41, 17, 0, {.graphic = &gWindowGraphic56DA90}, &WindowContacts_ButtonGuildDetails},
    {WindowElementType_Graphic, 19, 30, 18, 18, 1, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 124, 30, 18, 18, 1, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 19, 49, 18, 18, 1, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 124, 49, 18, 18, 1, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 19, 333, 18, 18, 1, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 42, 333, 18, 18, 1, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 42, 33, 0, 0, 1, {.graphic = &gWindowGraphic56D8B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 42, 52, 0, 0, 1, {.graphic = &gWindowGraphic56D8E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 310, 9, 11, 11, 1, {.graphic = &gWindowGraphic56D9A0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 218, 353, 39, 24, 1, {.graphic = &gWindowGraphic56DA30}, &WindowContacts_TabCategory},
    {WindowElementType_Graphic, 271, 352, 38, 25, 1, {.graphic = &gWindowGraphic56DA48}, &WindowContacts_TabCategory},
    {WindowElementType_Graphic, 143, 349, 60, 25, 1, {.graphic = &gWindowGraphic56DAD8}, &WindowContacts_TabCategory},
    {WindowElementType_Graphic, 120, 8, 60, 25, 1, {.graphic = &gWindowGraphic56DAF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 310, 69, 11, 11, 1, {.graphic = &gWindowGraphic56D9B8}, &WindowContacts_ScrollRow},
    {WindowElementType_Graphic, 310, 315, 11, 11, 1, {.graphic = &gWindowGraphic56D9E8}, &WindowContacts_ScrollRow},
    {WindowElementType_Graphic, 310, 80, 10, 14, 1, {.graphic = &gWindowGraphic56D9D0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 309, 80, 11, 235, 1, {.mouseFlags = &gWindowMouseFlagsD67840}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_EventArea, 0, 0, 331, 381, 1, {.unusedValue = 0}, &WindowContacts_Scroll},
    {WindowElementType_Graphic, 219, 45, 19, 19, 1, {.graphic = &gWindowGraphic56D970}, &WindowContacts_ButtonGroupMail},
    {WindowElementType_Text, 200, 30, 0, 0, 1, {.text = &gWindowText56DB30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 142, 84, 18, 18, 0, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleRankButton},
    {WindowElementType_Graphic, 282, 84, 18, 18, 0, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleRankButton},
    {WindowElementType_Graphic, 142, 171, 18, 18, 0, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleRankButton},
    {WindowElementType_Graphic, 282, 171, 18, 18, 0, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleRankButton},
    {WindowElementType_Graphic, 142, 258, 18, 18, 0, {.graphic = &gWindowGraphic56DA00}, &WindowContacts_HandleRankButton},
    {WindowElementType_Graphic, 282, 258, 18, 18, 0, {.graphic = &gWindowGraphic56DA18}, &WindowContacts_HandleRankButton},
    {WindowElementType_Graphic, 222, 120, 19, 19, 1, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 243, 120, 19, 19, 1, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 264, 120, 19, 19, 1, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 130, 71, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 279, 71, 0, 0, 1, {.text = &gWindowText56DB10}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 87, 0, 0, 1, {.text = &gWindowText56DB18}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 102, 0, 0, 1, {.text = &gWindowText56DB20}, &WindowElement_NoOp},
    {WindowElementType_Text, 151, 133, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 222, 207, 19, 19, 1, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 243, 207, 19, 19, 1, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 264, 207, 19, 19, 1, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 130, 158, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 279, 158, 0, 0, 1, {.text = &gWindowText56DB10}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 174, 0, 0, 1, {.text = &gWindowText56DB18}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 189, 0, 0, 1, {.text = &gWindowText56DB20}, &WindowElement_NoOp},
    {WindowElementType_Text, 151, 220, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 222, 303, 19, 19, 1, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 243, 303, 19, 19, 1, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 264, 303, 19, 19, 1, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 130, 245, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 279, 245, 0, 0, 1, {.text = &gWindowText56DB10}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 261, 0, 0, 1, {.text = &gWindowText56DB18}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 276, 0, 0, 1, {.text = &gWindowText56DB20}, &WindowElement_NoOp},
    {WindowElementType_Text, 151, 307, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 67, 292, 87, 1, {.graphic = &gWindowGraphic56DA60}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 16, 154, 292, 87, 1, {.graphic = &gWindowGraphic56DA60}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 16, 241, 292, 87, 1, {.graphic = &gWindowGraphic56DA60}, &WindowContacts_HandleNavigation},
    {WindowElementType_Graphic, 242, 72, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 72, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 72, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 80, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 80, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 102, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 102, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 102, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 110, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 110, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 131, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 131, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 131, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 139, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 139, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 160, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 160, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 160, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 168, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 168, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 189, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 189, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 189, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 197, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 197, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 218, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 218, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 218, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 226, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 226, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 247, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 247, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 247, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 255, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 255, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 276, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 276, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 276, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 284, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 284, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 242, 305, 19, 19, 0, {.graphic = &gWindowGraphic56D958}, &WindowContacts_OpenMail},
    {WindowElementType_Graphic, 263, 305, 19, 19, 0, {.graphic = &gWindowGraphic56D940}, &WindowContacts_ButtonCompose},
    {WindowElementType_Graphic, 284, 305, 19, 19, 0, {.graphic = &gWindowGraphic56D988}, &WindowContacts_ButtonDelete},
    {WindowElementType_Text, 56, 313, 0, 0, 1, {.text = &gWindowText56DB08}, &WindowElement_NoOp},
    {WindowElementType_Text, 173, 313, 0, 0, 1, {.text = &gWindowText56DB28}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 68, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 97, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 126, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 155, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 184, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 213, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 242, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 271, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 15, 300, 230, 29, 0, {.graphic = &gWindowGraphic56DA78}, &WindowContacts_HandleAvatar},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic56DAA8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic56DAC0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 331, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements56F860[29] { // 原全局：0x0056F860。
    {WindowElementType_Text, 7, 21, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 37, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 53, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 69, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 85, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 101, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 117, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 133, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 149, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 7, 165, 0, 0, 1, {.text = &gWindowText56F850}, &WindowElement_NoOp},
    {WindowElementType_Text, 8, 181, 0, 0, 1, {.text = &gWindowText56F858}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 150, 181, 49, 17, 1, {.graphic = &gWindowGraphic56F7C0}, &WindowGuildRank_ButtonClose},
    {WindowElementType_Graphic, 206, 181, 32, 17, 1, {.graphic = &gWindowGraphic56F7D8}, &WindowGuildRank_ButtonOk},
    {WindowElementType_Graphic, 224, 0, 0, 14, 1, {.graphic = &gWindowGraphic56F7F0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 225, 33, 11, 135, 1, {.mouseFlags = &gWindowMouseFlagsD67860}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 224, 22, 11, 11, 1, {.graphic = &gWindowGraphic56F808}, &WindowGuildRank_HandleScrollButtons},
    {WindowElementType_Graphic, 224, 168, 11, 11, 1, {.graphic = &gWindowGraphic56F820}, &WindowGuildRank_HandleScrollButtons},
    {WindowElementType_EventArea, 0, 0, 243, 206, 1, {.unusedValue = 0}, &WindowGuildRank_Scroll},
    {WindowElementType_Graphic, 221, 18, 0, 0, 1, {.graphic = &gWindowGraphic56F838}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 5, 19, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 35, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 51, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 67, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 83, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 99, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 115, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 131, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 147, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
    {WindowElementType_Graphic, 5, 163, 216, 16, 1, {.graphic = &gWindowGraphic56F7A8}, &WindowGuildRank_SelectRow},
};

WindowElementPrototype gWindowElements56FD70[21] { // 原全局：0x0056FD70。
    {WindowElementType_Graphic, 191, 9, 11, 11, 1, {.graphic = &gWindowGraphic56FCC0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 154, 27, 49, 17, 1, {.graphic = &gWindowGraphic56FCA8}, &WindowMailRead_ButtonReply},
    {WindowElementType_Graphic, 15, 180, 18, 18, 1, {.graphic = &gWindowGraphic56FCD8}, &WindowMailRead_ButtonPrevious},
    {WindowElementType_Graphic, 77, 180, 18, 18, 1, {.graphic = &gWindowGraphic56FCF0}, &WindowMailRead_ButtonNext},
    {WindowElementType_Text, 25, 52, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 64, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 76, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 88, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 100, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 112, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 124, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 136, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 148, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 160, 0, 0, 1, {.text = &gWindowText56FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 35, 183, 0, 0, 1, {.text = &gWindowText56FD58}, &WindowElement_NoOp},
    {WindowElementType_Text, 18, 30, 0, 0, 1, {.text = &gWindowText56FD60}, &WindowElement_NoOp},
    {WindowElementType_Text, 115, 180, 0, 0, 1, {.text = &gWindowText56FD68}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 45, 0, 0, 1, {.graphic = &gWindowGraphic56FD08}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 230, 210, 1, {.graphic = &gWindowGraphic56FD20}, &WindowMailRead_HandleAvatar},
    {WindowElementType_Graphic, 13, 25, 0, 0, 1, {.graphic = &gWindowGraphic56FD38}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 207, 0, 25, 100, 1, {}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements5701F8[9] { // 原全局：0x005701F8。
    {WindowElementType_Graphic, 191, 9, 11, 11, 1, {.graphic = &gWindowGraphic570190}, &WindowRuntime_ButtonClose},
    {WindowElementType_Input, 22, 70, 170, 120, 1, {}, &WindowMailCompose_HandleInput},
    {WindowElementType_Graphic, 16, 28, 56, 16, 1, {.graphic = &gWindowGraphic570148}, &WindowMailCompose_HandleRecipients},
    {WindowElementType_Graphic, 154, 28, 50, 18, 1, {.graphic = &gWindowGraphic570160}, &WindowMailCompose_HandleRecipients},
    {WindowElementType_Text, 21, 48, 0, 0, 1, {.text = &gWindowText5701F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 46, 0, 0, 1, {.graphic = &gWindowGraphic5700B8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5700D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 14, 23, 0, 0, 1, {.graphic = &gWindowGraphic5700E8}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 207, 0, 25, 100, 1, {}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements570318[13] { // 原全局：0x00570318。
    {WindowElementType_Graphic, 191, 9, 11, 11, 1, {.graphic = &gWindowGraphic570190}, &WindowRuntime_ButtonClose},
    {WindowElementType_Input, 33, 84, 170, 120, 1, {}, &WindowMailCompose_HandleInput},
    {WindowElementType_Graphic, 15, 29, 56, 16, 1, {.graphic = &gWindowGraphic570178}, &WindowPetMailCompose_HandleSendButtons},
    {WindowElementType_Graphic, 154, 29, 50, 18, 1, {.graphic = &gWindowGraphic570160}, &WindowPetMailCompose_HandleSendButtons},
    {WindowElementType_Graphic, 151, 199, 0, 0, 1, {.graphic = &gWindowGraphic5701A8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 151, 199, 50, 50, 1, {.graphic = &gWindowGraphic5701A8}, &WindowPetMailCompose_HandleGiftItem},
    {WindowElementType_Text, 21, 49, 0, 0, 1, {.text = &gWindowText5701F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 16, 199, 0, 0, 1, {.text = &gWindowText5701F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 224, 13, 11, 1, {.graphic = &gWindowGraphic5701C0}, &WindowPetMailCompose_HandleSendButtons},
    {WindowElementType_Graphic, 66, 224, 13, 11, 1, {.graphic = &gWindowGraphic5701D8}, &WindowPetMailCompose_HandleSendButtons},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic570118}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 13, 26, 0, 0, 1, {.graphic = &gWindowGraphic570130}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 207, 0, 25, 100, 1, {}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements5706D0[34] { // 原全局：0x005706D0。
    {WindowElementType_Text, 30, 10, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 23, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 36, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 49, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 62, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 75, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 88, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 101, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 114, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 127, 0, 0, 1, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 30, 140, 0, 0, 0, {.text = &gWindowText5706C8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 45, 130, 66, 17, 1, {.graphic = &gWindowGraphic570638}, &WindowConfirmation_ButtonOk},
    {WindowElementType_Graphic, 111, 130, 66, 17, 1, {.graphic = &gWindowGraphic570650}, &WindowConfirmation_ButtonCancel},
    {WindowElementType_Graphic, 15, 130, 66, 17, 0, {.graphic = &gWindowGraphic570668}, &WindowConfirmation_ButtonActionTwo},
    {WindowElementType_Graphic, 81, 130, 66, 17, 0, {.graphic = &gWindowGraphic570680}, &WindowConfirmation_ButtonActionThree},
    {WindowElementType_Graphic, 15, 130, 66, 17, 0, {.graphic = &gWindowGraphic570698}, &WindowConfirmation_ButtonActionFour},
    {WindowElementType_Graphic, 81, 130, 66, 17, 0, {.graphic = &gWindowGraphic5706B0}, &WindowConfirmation_ButtonActionFive},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic570560}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 74, 0, 0, 0, 1, {.graphic = &gWindowGraphic570578}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 148, 0, 0, 0, 1, {.graphic = &gWindowGraphic570590}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705A8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 215, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705D8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 108, 0, 0, 1, {.graphic = &gWindowGraphic5705F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 74, 156, 0, 0, 1, {.graphic = &gWindowGraphic570608}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 148, 108, 0, 0, 1, {.graphic = &gWindowGraphic570620}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 74, 0, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 148, 0, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 74, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 148, 54, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 108, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 74, 108, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 148, 108, 0, 0, 1, {.graphic = &gWindowGraphic5705C0}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements566810[51] { // 原全局：0x00566810。
    {WindowElementType_Input, 66, 31, 96, 12, 1, {.unusedValue = 0}, &WindowPetDetail_EditName},
    {WindowElementType_Graphic, 225, 9, 12, 12, 1, {.graphic = &gWindowGraphic565638}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 80, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656B0}, &WindowPetDetail_SwitchPage},
    {WindowElementType_Graphic, 133, 276, 35, 25, 1, {.graphic = &gWindowGraphic5656C8}, &WindowPetDetail_SwitchPage},
    {WindowElementType_Graphic, 186, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 187, 28, 32, 16, 1, {.graphic = &gWindowGraphic565650}, &WindowPetSkills_HandleButtons},
    {WindowElementType_Graphic, 175, 253, 49, 15, 1, {.graphic = &gWindowGraphic5658C0}, &WindowPetSkills_HandleButtons},
    {WindowElementType_Graphic, 21, 253, 13, 15, 1, {.graphic = &gWindowGraphic565878}, &WindowPetSkills_HandleButtons},
    {WindowElementType_Graphic, 65, 253, 13, 15, 1, {.graphic = &gWindowGraphic565890}, &WindowPetSkills_HandleButtons},
    {WindowElementType_Graphic, 225, 50, 11, 11, 1, {.graphic = &gWindowGraphic565908}, &WindowPetSkills_HandleButtons},
    {WindowElementType_Graphic, 225, 195, 11, 11, 1, {.graphic = &gWindowGraphic565920}, &WindowPetSkills_HandleButtons},
    {WindowElementType_Text, 66, 31, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 44, 256, 0, 0, 1, {.text = &gWindowText565988}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 50, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 66, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 82, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 98, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 114, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 130, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 146, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 162, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 178, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 194, 0, 0, 1, {.text = &gWindowText565990}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 51, 0, 0, 1, {.number = &gWindowNumbers5667C0[0]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 67, 0, 0, 1, {.number = &gWindowNumbers5667C0[1]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 83, 0, 0, 1, {.number = &gWindowNumbers5667C0[2]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 99, 0, 0, 1, {.number = &gWindowNumbers5667C0[3]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 115, 0, 0, 1, {.number = &gWindowNumbers5667C0[4]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 131, 0, 0, 1, {.number = &gWindowNumbers5667C0[5]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 147, 0, 0, 1, {.number = &gWindowNumbers5667C0[6]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 163, 0, 0, 1, {.number = &gWindowNumbers5667C0[7]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 179, 0, 0, 1, {.number = &gWindowNumbers5667C0[8]}, &WindowElement_NoOp},
    {WindowElementType_Number, 201, 195, 0, 0, 1, {.number = &gWindowNumbers5667C0[9]}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 213, 0, 0, 1, {.text = &gWindowText5659A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 225, 0, 0, 1, {.text = &gWindowText5659A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 237, 0, 0, 1, {.text = &gWindowText5659A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 249, 0, 0, 1, {.text = &gWindowText5659A0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 17, 48, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 64, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 80, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 96, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 112, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 128, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 144, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 160, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 176, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 192, 206, 15, 1, {.graphic = &gWindowGraphic5658A8}, &WindowPetSkills_HandleSkillRow},
    {WindowElementType_Graphic, 17, 32, 0, 0, 1, {.graphic = &gWindowGraphic565698}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic565608}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 14, 25, 0, 0, 1, {.graphic = &gWindowGraphic565620}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 250, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements5659C0[66] { // 原全局：0x005659C0。
    {WindowElementType_Input, 66, 31, 96, 12, 1, {.unusedValue = 0}, &WindowPetDetail_EditName},
    {WindowElementType_Graphic, 225, 9, 12, 12, 1, {.graphic = &gWindowGraphic565638}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 80, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 133, 276, 35, 25, 1, {.graphic = &gWindowGraphic5656C8}, &WindowPetDetail_SwitchPage},
    {WindowElementType_Graphic, 186, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656E0}, &WindowPetDetail_SwitchPage},
    {WindowElementType_Graphic, 187, 28, 32, 16, 1, {.graphic = &gWindowGraphic565650}, &WindowPetDetail_HandleButtons},
    {WindowElementType_Graphic, 175, 253, 49, 15, 1, {.graphic = &gWindowGraphic5658C0}, &WindowPetDetail_HandleButtons},
    {WindowElementType_Graphic, 21, 253, 13, 15, 1, {.graphic = &gWindowGraphic565878}, &WindowPetDetail_HandleButtons},
    {WindowElementType_Graphic, 65, 253, 13, 15, 1, {.graphic = &gWindowGraphic565890}, &WindowPetDetail_HandleButtons},
    {WindowElementType_Text, 66, 31, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 200, 141, 0, 0, 1, {.graphic = &gWindowGraphic565938}, &WindowElement_NoOp},
    {WindowElementType_Text, 63, 65, 0, 0, 1, {.text = &gWindowText565958}, &WindowElement_NoOp},
    {WindowElementType_Text, 48, 82, 0, 0, 1, {.text = &gWindowText565960}, &WindowElement_NoOp},
    {WindowElementType_Text, 142, 82, 0, 0, 1, {.text = &gWindowText565968}, &WindowElement_NoOp},
    {WindowElementType_Text, 62, 99, 0, 0, 1, {.text = &gWindowText565970}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 99, 0, 0, 1, {.text = &gWindowText565978}, &WindowElement_NoOp},
    {WindowElementType_Text, 206, 99, 0, 0, 1, {.text = &gWindowText565980}, &WindowElement_NoOp},
    {WindowElementType_Text, 44, 256, 0, 0, 1, {.text = &gWindowText565988}, &WindowElement_NoOp},
    {WindowElementType_Text, 63, 48, 0, 0, 1, {.text = &gWindowText5659A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 39, 115, 0, 0, 1, {.text = &gWindowText5659B0}, &WindowElement_NoOp},
    {WindowElementType_Animation, 80, 220, 0, 0, 1, {.animationImageNo = reinterpret_cast<std::int32_t*>(&gWindowAnimationImage5659B8)}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 157, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 163, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 169, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 175, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 181, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 187, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 193, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 199, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 211, 166, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 157, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 163, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 169, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 175, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 181, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 187, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 193, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 199, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 211, 182, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 157, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 163, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 169, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 175, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 181, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 187, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 193, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 199, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 211, 198, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 157, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 163, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 169, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 175, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 181, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 187, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 193, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 199, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 211, 214, 0, 0, 1, {.graphic = &gWindowGraphic563388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 23, 32, 0, 0, 1, {.graphic = &gWindowGraphic565668}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic565608}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 14, 25, 0, 0, 1, {.graphic = &gWindowGraphic565620}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 19, 132, 114, 110, 1, {.mouseFlags = &gWindowMouseFlagsD677B0}, &WindowPetDetail_HandleButtons},
    {WindowElementType_EventArea, 250, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements566200[46] { // 原全局：0x00566200。
    {WindowElementType_Input, 66, 31, 96, 12, 1, {.unusedValue = 0}, &WindowPetDetail_EditName},
    {WindowElementType_Graphic, 225, 9, 12, 12, 1, {.graphic = &gWindowGraphic565638}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 80, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656B0}, &WindowPetDetail_SwitchPage},
    {WindowElementType_Graphic, 133, 276, 35, 25, 1, {.graphic = &gWindowGraphic5656C8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 186, 275, 35, 25, 1, {.graphic = &gWindowGraphic5656E0}, &WindowPetDetail_SwitchPage},
    {WindowElementType_Graphic, 187, 28, 32, 16, 1, {.graphic = &gWindowGraphic565650}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 175, 253, 49, 15, 1, {.graphic = &gWindowGraphic5658C0}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 21, 253, 13, 15, 1, {.graphic = &gWindowGraphic565878}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 65, 253, 13, 15, 1, {.graphic = &gWindowGraphic565890}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 61, 113, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 61, 128, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 61, 143, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 61, 158, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Graphic, 61, 173, 10, 10, 1, {.graphic = &gWindowGraphic5658D8}, &WindowPetAttributes_HandleButtons},
    {WindowElementType_Text, 66, 31, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 63, 48, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 55, 65, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 65, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 44, 256, 0, 0, 1, {.text = &gWindowText565988}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 98, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 114, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 129, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 144, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 159, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 174, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 197, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 93, 212, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 97, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 98, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 114, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 113, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 144, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 144, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 159, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 159, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 174, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 174, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 197, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 197, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 212, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 205, 212, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Text, 155, 227, 0, 0, 1, {.text = &gWindowText565950}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 18, 32, 0, 0, 1, {.graphic = &gWindowGraphic565680}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic565608}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 14, 25, 0, 0, 1, {.graphic = &gWindowGraphic565620}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 250, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements567028[71] { // 原全局：0x00567028。
    {WindowElementType_Graphic, 206, 8, 11, 11, 1, {.graphic = &gWindowGraphic566F18}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 206, 41, 0, 14, 1, {.graphic = &gWindowGraphic566F90}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 205, 41, 11, 134, 1, {.mouseFlags = &gWindowMouseFlagsD677C0}, &WindowAlbumList_DragScroll},
    {WindowElementType_Graphic, 206, 30, 11, 11, 1, {.graphic = &gWindowGraphic566FA8}, &WindowAlbumList_HandleScrollButtons},
    {WindowElementType_Graphic, 206, 174, 11, 11, 1, {.graphic = &gWindowGraphic566FC0}, &WindowAlbumList_HandleScrollButtons},
    {WindowElementType_Graphic, 40, 189, 20, 20, 1, {.graphic = &gWindowGraphic566FF0}, &WindowAlbumList_HandleScrollButtons},
    {WindowElementType_Graphic, 170, 189, 20, 20, 1, {.graphic = &gWindowGraphic567008}, &WindowAlbumList_HandleScrollButtons},
    {WindowElementType_EventArea, 0, 0, 225, 220, 1, {.unusedValue = 0}, &WindowAlbumList_Scroll},
    {WindowElementType_Graphic, 19, 31, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 31, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 31, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 47, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 47, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 47, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 63, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 63, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 63, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 79, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 79, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 79, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 95, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 95, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 95, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 111, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 111, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 111, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 127, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 127, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 127, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 143, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 143, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 143, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 159, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 159, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 159, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 19, 175, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 175, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 175, 0, 0, 1, {.graphic = &gWindowGraphic566F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 31, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 47, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 63, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 79, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 95, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 111, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 127, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 143, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 159, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 47, 175, 0, 0, 1, {.graphic = &gWindowGraphic566FD8}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 29, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 45, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 61, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 77, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 93, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 109, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 125, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 141, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 157, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Text, 75, 173, 0, 0, 1, {.text = &gWindowText567020}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 28, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 44, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 60, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 76, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 92, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 108, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 124, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 140, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 156, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 15, 172, 186, 16, 1, {.graphic = &gWindowGraphic566F60}, &WindowAlbumList_SelectEntry},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic566F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic566F48}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 225, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements567AE8[82] { // 原全局：0x00567AE8。
    {WindowElementType_Graphic, 235, 8, 11, 11, 1, {.graphic = &gWindowGraphic5679B0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Animation, 186, 135, 0, 0, 1, {.animationImageNo = reinterpret_cast<std::int32_t*>(&gWindowAnimationImage567AE0)}, &WindowElement_NoOp},
    {WindowElementType_Text, 60, 28, 0, 0, 1, {.text = &gWindowText567AB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 214, 28, 0, 0, 1, {.text = &gWindowText567AC0}, &WindowElement_NoOp},
    {WindowElementType_Text, 95, 47, 0, 0, 1, {.text = &gWindowText567AC8}, &WindowElement_NoOp},
    {WindowElementType_Text, 63, 63, 0, 0, 1, {.text = &gWindowText567AD0}, &WindowElement_NoOp},
    {WindowElementType_Text, 48, 262, 0, 0, 1, {.text = &gWindowText567AD8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 18, 175, 0, 0, 1, {.graphic = &gWindowGraphic5679F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 54, 173, 0, 0, 1, {.graphic = &gWindowGraphic5679F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 18, 261, 13, 11, 1, {.graphic = &gWindowGraphic567A40}, &WindowAlbumDetail_HandleNavigation},
    {WindowElementType_Graphic, 38, 261, 10, 10, 1, {.graphic = &gWindowGraphic567A58}, &WindowAlbumDetail_HandleNavigation},
    {WindowElementType_Graphic, 90, 261, 10, 10, 1, {.graphic = &gWindowGraphic567A70}, &WindowAlbumDetail_HandleNavigation},
    {WindowElementType_Graphic, 102, 261, 13, 11, 1, {.graphic = &gWindowGraphic567A88}, &WindowAlbumDetail_HandleNavigation},
    {WindowElementType_Graphic, 31, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 43, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 49, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 55, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 61, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 67, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 79, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 85, 100, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 43, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 49, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 55, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 61, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 67, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 79, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 85, 116, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 43, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 49, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 55, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 61, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 67, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 79, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 85, 132, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 31, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 43, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 49, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 55, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 61, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 67, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 79, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 85, 148, 0, 0, 1, {.graphic = &gWindowGraphic567AA0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 145, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 185, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 225, 173, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 145, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 185, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 225, 194, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 145, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 185, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 225, 215, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 145, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 185, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 225, 236, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 145, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 165, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 185, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 205, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 225, 257, 0, 0, 1, {.graphic = &gWindowGraphic567A10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 11, 11, 1, {.graphic = &gWindowGraphic5679C8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 13, 26, 11, 11, 1, {.graphic = &gWindowGraphic5679E0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 131, 59, 114, 110, 1, {.mouseFlags = &gWindowMouseFlagsD677D0}, &WindowAlbumDetail_HandleNavigation},
    {WindowElementType_EventArea, 256, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements5689C8[32] { // 原全局：0x005689C8。
    {WindowElementType_Graphic, 186, 9, 11, 11, 1, {.graphic = &gWindowGraphic568720}, &WindowSettings_ButtonClose},
    {WindowElementType_Graphic, 69, 51, 70, 17, 1, {.graphic = &gWindowGraphic568738}, &WindowSettings_ButtonLogoutPanel},
    {WindowElementType_Graphic, 69, 79, 70, 17, 1, {.graphic = &gWindowGraphic568750}, &WindowSettings_ButtonInputPanel},
    {WindowElementType_Graphic, 69, 109, 70, 17, 1, {.graphic = &gWindowGraphic568768}, &WindowSettings_ButtonAudioPanel},
    {WindowElementType_Graphic, 69, 142, 70, 17, 1, {.graphic = &gWindowGraphic568780}, &WindowSettings_ButtonDisplayPanel},
    {WindowElementType_Graphic, 69, 174, 70, 17, 1, {.graphic = &gWindowGraphic568798}, &WindowSettings_ButtonHotkeys},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic5688D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 13, 26, 40, 11, 1, {.graphic = &gWindowGraphic5688E8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -111, 30, 102, 18, 1, {.graphic = &gWindowGraphic568810}, &WindowSettings_ButtonLogout},
    {WindowElementType_Graphic, -111, 58, 102, 18, 1, {.graphic = &gWindowGraphic568828}, &WindowSettings_ButtonServerSelect},
    {WindowElementType_Graphic, -117, 40, 88, 18, 1, {.graphic = &gWindowGraphic568840}, &WindowSettings_ToggleItemStacking},
    {WindowElementType_Graphic, -117, 75, 88, 18, 1, {.graphic = &gWindowGraphic568858}, &WindowSettings_ToggleCursor},
    {WindowElementType_Graphic, -117, 110, 88, 18, 1, {.graphic = &gWindowGraphic568900}, &WindowSettings_CycleHotkeyMode},
    {WindowElementType_Number, -72, 51, 0, 0, 1, {.number = &gWindowNumber5689C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -98, 47, 18, 18, 1, {.graphic = &gWindowGraphic568870}, &WindowSettings_ButtonMusicDecrease},
    {WindowElementType_Graphic, -35, 47, 18, 18, 1, {.graphic = &gWindowGraphic568888}, &WindowSettings_ButtonMusicIncrease},
    {WindowElementType_Number, -72, 95, 0, 0, 1, {.number = &gWindowNumber5689B8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -98, 91, 18, 18, 1, {.graphic = &gWindowGraphic568870}, &WindowSettings_ButtonSoundDecrease},
    {WindowElementType_Graphic, -35, 91, 18, 18, 1, {.graphic = &gWindowGraphic568888}, &WindowSettings_ButtonSoundIncrease},
    {WindowElementType_Graphic, -88, 141, 63, 18, 1, {.graphic = &gWindowGraphic5688A0}, &WindowSettings_ToggleChannels},
    {WindowElementType_Graphic, -88, 185, 63, 18, 1, {.graphic = &gWindowGraphic568918}, &WindowSettings_ToggleAudioFlag},
    {WindowElementType_Graphic, -45, 25, 44, 18, 1, {.graphic = &gWindowGraphic5688B8}, &WindowSettings_ToggleScanlines},
    {WindowElementType_Graphic, 0, 72, 22, 19, 0, {.graphic = &gWindowGraphic568930}, &WindowSettings_DrawRenderModeButton},
    {WindowElementType_Graphic, -120, 72, 22, 19, 1, {.graphic = &gWindowGraphic568948}, &WindowSettings_DrawRenderModeButton},
    {WindowElementType_Graphic, -89, 72, 22, 19, 1, {.graphic = &gWindowGraphic568960}, &WindowSettings_DrawRenderModeButton},
    {WindowElementType_Graphic, -58, 72, 22, 19, 1, {.graphic = &gWindowGraphic568978}, &WindowSettings_DrawRenderModeButton},
    {WindowElementType_Graphic, -27, 72, 22, 19, 1, {.graphic = &gWindowGraphic568990}, &WindowSettings_DrawRenderModeButton},
    {WindowElementType_Graphic, -130, 10, 150, 79, 1, {.graphic = &gWindowGraphic5687B0}, &WindowElement_HandleSideArea},
    {WindowElementType_Graphic, -130, 10, 150, 128, 1, {.graphic = &gWindowGraphic5687C8}, &WindowElement_HandleSideArea},
    {WindowElementType_Graphic, -130, 10, 150, 203, 1, {.graphic = &gWindowGraphic5687E0}, &WindowElement_HandleSideArea},
    {WindowElementType_Graphic, -130, 10, 150, 90, 1, {.graphic = &gWindowGraphic5687F8}, &WindowElement_HandleSideArea},
    {WindowElementType_EventArea, 209, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements568F78[25] { // 原全局：0x00568F78。
    {WindowElementType_Input, 80, 41, 0, 0, 1, {.unusedValue = 0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 265, 9, 11, 11, 1, {.graphic = &gWindowGraphic568E70}, &WindowHotkeys_ButtonClose},
    {WindowElementType_Graphic, 253, 68, 0, 14, 1, {.graphic = &gWindowGraphic568EA0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 254, 68, 11, 137, 1, {.mouseFlags = &gWindowMouseFlagsD677F0}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 253, 58, 11, 11, 1, {.graphic = &gWindowGraphic568EB8}, &WindowHotkeys_ButtonPreviousRow},
    {WindowElementType_Graphic, 253, 204, 11, 11, 1, {.graphic = &gWindowGraphic568ED0}, &WindowHotkeys_ButtonNextRow},
    {WindowElementType_Graphic, 227, 35, 18, 18, 1, {.graphic = &gWindowGraphic568F48}, &WindowHotkeys_ButtonPreviousPage},
    {WindowElementType_Graphic, 245, 35, 18, 18, 1, {.graphic = &gWindowGraphic568F60}, &WindowHotkeys_ButtonNextPage},
    {WindowElementType_EventArea, 0, 0, 286, 233, 1, {.unusedValue = 0}, &WindowHotkeys_Scroll},
    {WindowElementType_Graphic, 25, 56, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 72, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 88, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 104, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 120, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 136, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 152, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 168, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 184, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 200, 216, 16, 1, {.graphic = &gWindowGraphic568F30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 25, 36, 49, 17, 1, {.graphic = &gWindowGraphic568EE8}, &WindowHotkeys_TabFirstCategory},
    {WindowElementType_Graphic, 75, 36, 49, 17, 1, {.graphic = &gWindowGraphic568F00}, &WindowHotkeys_TabSecondCategory},
    {WindowElementType_Graphic, 125, 36, 49, 17, 1, {.graphic = &gWindowGraphic568F18}, &WindowHotkeys_TabThirdCategory},
    {WindowElementType_Graphic, 175, 36, 49, 17, 1, {.graphic = &gWindowGraphic568F18}, &WindowHotkeys_TabPresetText},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic568E88}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 288, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements57B508[48] { // 原全局：0x0057B508。
    {WindowElementType_EventArea, 0, 0, 549, 366, 1, {.unusedValue = 0}, &WindowStall_DrawTooltipOverlay},
    {WindowElementType_Input, 0, 0, 549, 366, 1, {.unusedValue = 0}, &WindowStall_HandlePriceInput},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic57B288}, &WindowStall_ButtonClose},
    {WindowElementType_Number, 393, 30, 0, 0, 1, {.number = &gWindowNumber57B450}, &WindowElement_NoOp},
    {WindowElementType_Text, 412, 44, 0, 14, 1, {.text = &gWindowText57B420}, &WindowElement_NoOp},
    {WindowElementType_Text, 230, 25, 0, 14, 1, {.text = &gWindowText57B428}, &WindowElement_NoOp},
    {WindowElementType_Input, 230, 11, 120, 16, 1, {.unusedValue = 0}, &WindowStall_EditName},
    {WindowElementType_Text, 298, 51, 0, 0, 1, {.text = &gWindowText57B430}, &WindowElement_NoOp},
    {WindowElementType_Input, 299, 51, 167, 57, 1, {.unusedValue = 0}, &WindowStall_EditDescription},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage57B458}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 15, 28, 274, 325, 1, {.unusedValue = 0}, &WindowStall_ScrollListings},
    {WindowElementType_EventArea, 292, 28, 250, 325, 1, {.unusedValue = 0}, &WindowStall_ScrollPetCandidates},
    {WindowElementType_EventArea, 22, 53, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 109, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 165, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 221, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 277, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 53, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 109, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 165, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 221, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 277, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 292, 127, 248, 198, 1, {.unusedValue = 0}, &WindowStall_HandleInventory},
    {WindowElementType_EventArea, 22, 53, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow},
    {WindowElementType_EventArea, 22, 144, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow},
    {WindowElementType_EventArea, 22, 235, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow},
    {WindowElementType_EventArea, 292, 127, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_EventArea, 292, 177, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_EventArea, 292, 227, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_EventArea, 292, 277, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_Graphic, 45, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2A0}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 45, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2B8}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 188, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2E8}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 461, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B300}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 322, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B318}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 26, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B3F0}, &WindowStall_TabCategory},
    {WindowElementType_Graphic, 90, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B408}, &WindowStall_TabCategory},
    {WindowElementType_Graphic, 267, 65, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 267, 65, 11, 248, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 267, 55, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &WindowStall_ButtonPreviousListing},
    {WindowElementType_Graphic, 267, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &WindowStall_ButtonNextListing},
    {WindowElementType_Graphic, 527, 139, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 527, 139, 11, 174, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 527, 129, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &WindowStall_ButtonPreviousPet},
    {WindowElementType_Graphic, 527, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &WindowStall_ButtonNextPet},
    {WindowElementType_Graphic, 22, 53, 243, 274, 0, {.graphic = &gWindowGraphic57B360}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 292, 127, 248, 198, 0, {.graphic = &gWindowGraphic57B378}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic57B390}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements57BB08[43] { // 原全局：0x0057BB08。
    {WindowElementType_EventArea, 0, 0, 549, 366, 1, {.unusedValue = 0}, &WindowStall_DrawTooltipOverlay},
    {WindowElementType_EventArea, 0, 0, 549, 366, 1, {.unusedValue = 0}, &WindowStall_HandlePurchaseConfirmation},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic57B288}, &WindowStall_ButtonClose},
    {WindowElementType_Number, 393, 30, 0, 0, 1, {.number = &gWindowNumber57B450}, &WindowElement_NoOp},
    {WindowElementType_Text, 412, 44, 0, 14, 1, {.text = &gWindowText57B420}, &WindowElement_NoOp},
    {WindowElementType_Text, 230, 25, 0, 14, 1, {.text = &gWindowText57B428}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 299, 51, 167, 57, 1, {.unusedValue = 0}, &WindowElement_NoOp},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage57B458}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 15, 28, 274, 325, 1, {.unusedValue = 0}, &WindowStall_ScrollListings},
    {WindowElementType_EventArea, 292, 28, 250, 325, 1, {.unusedValue = 0}, &WindowStall_ScrollPetCandidates},
    {WindowElementType_EventArea, 22, 53, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 109, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 165, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 221, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 22, 277, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 53, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 109, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 165, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 221, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 144, 277, 48, 48, 1, {.unusedValue = 0}, &WindowStall_HandleItemRow},
    {WindowElementType_EventArea, 292, 127, 248, 198, 1, {.unusedValue = 0}, &WindowStall_HandleInventory},
    {WindowElementType_EventArea, 22, 53, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow},
    {WindowElementType_EventArea, 22, 144, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow},
    {WindowElementType_EventArea, 22, 235, 240, 90, 1, {.unusedValue = 0}, &WindowStall_HandlePetRow},
    {WindowElementType_EventArea, 292, 127, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_EventArea, 292, 177, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_EventArea, 292, 227, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_EventArea, 292, 277, 231, 48, 1, {.unusedValue = 0}, &WindowStall_HandlePetCandidate},
    {WindowElementType_Graphic, 188, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2D0}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 461, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B300}, &WindowStall_HandleButtons},
    {WindowElementType_Graphic, 26, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B3F0}, &WindowStall_TabCategory},
    {WindowElementType_Graphic, 90, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B408}, &WindowStall_TabCategory},
    {WindowElementType_Graphic, 267, 65, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 267, 65, 11, 248, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 267, 55, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &WindowStall_ButtonPreviousListing},
    {WindowElementType_Graphic, 267, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &WindowStall_ButtonNextListing},
    {WindowElementType_Graphic, 527, 139, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 527, 139, 11, 174, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 527, 129, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &WindowStall_ButtonPreviousPet},
    {WindowElementType_Graphic, 527, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &WindowStall_ButtonNextPet},
    {WindowElementType_Graphic, 22, 53, 243, 274, 0, {.graphic = &gWindowGraphic57B360}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 292, 127, 248, 198, 0, {.graphic = &gWindowGraphic57B378}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic57B390}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements577298[32] { // 原全局：0x00577298。
    {WindowElementType_Graphic, 206, 8, 11, 11, 1, {.graphic = &gWindowGraphic577170}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 206, 41, 0, 14, 1, {.graphic = &gWindowGraphic5771E8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 205, 41, 11, 134, 1, {.mouseFlags = &gWindowMouseFlagsD67930}, &WindowOwnProfileCategory_DragScroll},
    {WindowElementType_Graphic, 206, 30, 11, 11, 1, {.graphic = &gWindowGraphic577200}, &WindowOwnProfileCategory_HandleScrollButtons},
    {WindowElementType_Graphic, 206, 174, 11, 11, 1, {.graphic = &gWindowGraphic577218}, &WindowOwnProfileCategory_HandleScrollButtons},
    {WindowElementType_Graphic, 89, 7, 0, 0, 1, {.graphic = &gWindowGraphic577278}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 40, 189, 20, 20, 1, {.graphic = &gWindowGraphic577248}, &WindowOwnProfileCategory_HandleScrollButtons},
    {WindowElementType_Graphic, 170, 189, 20, 20, 1, {.graphic = &gWindowGraphic577260}, &WindowOwnProfileCategory_HandleScrollButtons},
    {WindowElementType_EventArea, 0, 0, 225, 220, 1, {.unusedValue = 0}, &WindowOwnProfileCategory_Scroll},
    {WindowElementType_Text, 20, 30, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 46, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 62, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 78, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 94, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 110, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 126, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 142, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 158, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 174, 0, 0, 1, {.text = &gWindowText577290}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 28, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 44, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 60, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 76, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 92, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 108, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 124, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 140, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 156, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 15, 172, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &WindowOwnProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic577188}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic5771A0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 225, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements5777A8[162] { // 原全局：0x005777A8。
    {WindowElementType_Text, 50, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 50, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 150, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 268, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 368, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 484, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 48, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 48, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 148, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 266, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 366, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 482, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &WindowProfileCategory_SelectCategory},
    {WindowElementType_Graphic, 593, 8, 12, 12, 1, {.graphic = &gWindowGraphic577740}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic577758}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 30, 0, 0, 1, {.graphic = &gWindowGraphic577770}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 600, 0, 34, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements578CC8[71] { // 原全局：0x00578CC8。
    {WindowElementType_Graphic, 284, 9, 10, 10, 1, {.graphic = &gWindowGraphic578BE8}, &WindowRuntime_ButtonClose},
    {WindowElementType_Text, 120, 30, 0, 0, 1, {.text = &gWindowText578CA8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 140, 8, 0, 0, 1, {.graphic = &gWindowGraphic578C48}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 367, 55, 20, 1, {.graphic = &gWindowGraphic578C90}, &WindowProfileSearch_HandleSearchButtons},
    {WindowElementType_Graphic, 250, 367, 20, 20, 1, {.graphic = &gWindowGraphic578C60}, &WindowProfileSearch_HandleSearchButtons},
    {WindowElementType_Graphic, 270, 367, 20, 20, 1, {.graphic = &gWindowGraphic578C78}, &WindowProfileSearch_HandleSearchButtons},
    {WindowElementType_Text, 20, 370, 0, 0, 1, {.text = &gWindowText578CC0}, &WindowElement_NoOp},
    {WindowElementType_Input, 20, 370, 168, 14, 1, {.unusedValue = 0}, &WindowProfileSearch_EditKeyword},
    {WindowElementType_Text, 20, 47, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 63, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 79, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 95, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 111, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 127, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 143, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 159, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 175, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 191, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 207, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 223, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 239, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 255, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 271, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 287, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 303, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 319, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 335, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 351, 0, 0, 1, {.text = &gWindowText578CB0}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 47, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 63, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 79, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 95, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 111, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 127, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 143, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 159, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 175, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 191, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 207, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 223, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 239, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 255, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 271, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 287, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 303, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 319, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 335, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Text, 195, 351, 0, 0, 1, {.text = &gWindowText578CB8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 45, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 61, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 77, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 93, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 109, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 125, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 141, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 157, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 173, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 189, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 205, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 221, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 237, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 253, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 269, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 285, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 301, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 317, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 333, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 15, 349, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &WindowProfileSearch_SelectResult},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic578C00}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 14, 27, 0, 0, 1, {.graphic = &gWindowGraphic578C18}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 300, 0, 30, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements579710[22] { // 原全局：0x00579710。
    {WindowElementType_Graphic, 257, 32, 40, 30, 1, {.graphic = &gWindowGraphic5796B0}, &WindowProfileView_ButtonMail},
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText5796C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText5796D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 39, 0, 0, 1, {.text = &gWindowText5796D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 59, 0, 0, 1, {.text = &gWindowText5796E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 79, 0, 0, 1, {.text = &gWindowText5796E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 108, 0, 0, 1, {.text = &gWindowText5796F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 130, 0, 0, 1, {.text = &gWindowText579700}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 152, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 170, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 184, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 198, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 212, 0, 0, 1, {.text = &gWindowText579708}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic5795A8}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic5795A8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 102, 20, 20, 1, {.graphic = &gWindowGraphic579698}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 124, 20, 20, 1, {.graphic = &gWindowGraphic579698}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 146, 20, 20, 1, {.graphic = &gWindowGraphic579698}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 36, 0, 0, 1, {.graphic = &gWindowGraphic579620}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic579638}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic579650}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 307, 0, 30, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements579B28[16] { // 原全局：0x00579B28。
    {WindowElementType_Graphic, 191, 9, 11, 11, 1, {.graphic = &gWindowGraphic579AA8}, &WindowRuntime_ButtonClose},
    {WindowElementType_Input, 22, 70, 170, 120, 1, {.unusedValue = 0}, &WindowMiniMail_HandleInput},
    {WindowElementType_Graphic, 154, 28, 50, 18, 1, {.graphic = &gWindowGraphic579A78}, &WindowMiniMail_Send},
    {WindowElementType_Graphic, 80, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send},
    {WindowElementType_Graphic, 72, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send},
    {WindowElementType_Graphic, 64, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send},
    {WindowElementType_Graphic, 56, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send},
    {WindowElementType_Graphic, 48, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send},
    {WindowElementType_Graphic, 40, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send},
    {WindowElementType_Graphic, 32, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send},
    {WindowElementType_Graphic, 24, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send},
    {WindowElementType_Graphic, 20, 28, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &WindowMiniMail_Send},
    {WindowElementType_Text, 21, 48, 0, 0, 1, {.text = &gWindowText579B20}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 46, 0, 0, 1, {.graphic = &gWindowGraphic5799D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5799E8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 14, 23, 0, 0, 1, {.graphic = &gWindowGraphic579A00}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements57A070[21] { // 原全局：0x0057A070。
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText57A028}, &WindowElement_NoOp},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText57A030}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 39, 0, 0, 1, {.text = &gWindowText57A038}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 59, 0, 0, 1, {.text = &gWindowText57A040}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 79, 0, 0, 1, {.text = &gWindowText57A048}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 108, 0, 0, 1, {.text = &gWindowText57A058}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 130, 0, 0, 1, {.text = &gWindowText57A060}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 152, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 170, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 184, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 198, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 212, 0, 0, 1, {.text = &gWindowText57A068}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic579F20}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic579F20}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 73, 102, 20, 20, 1, {.graphic = &gWindowGraphic57A010}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 73, 124, 20, 20, 1, {.graphic = &gWindowGraphic57A010}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 73, 146, 20, 20, 1, {.graphic = &gWindowGraphic57A010}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 15, 36, 0, 0, 1, {.graphic = &gWindowGraphic579F98}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic579FB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic579FC8}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements56CCD0[1] { // 原全局：0x0056CCD0。
    {WindowElementType_Text, 0, 44, 0, 0, 1, {.text = &gWindowText56CCC8}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements5630F0[8] { // 原全局：0x005630F0。
    {WindowElementType_Input, 0, 0, 126, 136, 1, {.unusedValue = 0}, &WindowQuantity_HandleInput},
    {WindowElementType_Graphic, 106, 8, 11, 11, 1, {.graphic = &gWindowGraphic563080}, &WindowRuntime_ButtonClose},
    {WindowElementType_EventArea, 18, 46, 106, 85, 1, {.unusedValue = 0}, &WindowQuantity_ButtonOk},
    {WindowElementType_Graphic, 92, 29, 11, 11, 1, {.graphic = &gWindowGraphic5630B0}, &WindowQuantity_ButtonIncrease},
    {WindowElementType_Graphic, 105, 29, 11, 11, 1, {.graphic = &gWindowGraphic5630C8}, &WindowQuantity_ButtonDecrease},
    {WindowElementType_Number, 85, 30, 0, 0, 1, {.number = &gWindowNumber5630E8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 126, 136, 1, {.graphic = &gWindowGraphic563098}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 128, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements56B710[14] { // 原全局：0x0056B710。
    {WindowElementType_Graphic, 13, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B5F8}, &WindowBattleCommand_ButtonAttack},
    {WindowElementType_Graphic, 13, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B610}, &WindowBattleCommand_ButtonDefend},
    {WindowElementType_Graphic, 83, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B628}, &WindowBattleCommand_ButtonSkill},
    {WindowElementType_Graphic, 83, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B640}, &WindowBattleCommand_ButtonItem},
    {WindowElementType_Graphic, 153, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B658}, &WindowBattleCommand_ButtonPet},
    {WindowElementType_Graphic, 153, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B670}, &WindowBattleCommand_ButtonFormation},
    {WindowElementType_Graphic, 223, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B688}, &WindowBattleCommand_ButtonEscape},
    {WindowElementType_Graphic, 223, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B6A0}, &WindowBattleCommand_ButtonRebirth},
    {WindowElementType_Graphic, 216, 46, 64, 20, 1, {.graphic = &gWindowGraphic56B628}, &WindowBattleCommand_ButtonPetSkills},
    {WindowElementType_Graphic, 216, 46, 64, 20, 1, {.graphic = &gWindowGraphic56B688}, &WindowBattleCommand_ButtonEscape},
    {WindowElementType_Text, 20, 24, 0, 0, 1, {.text = &gWindowText56B6F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 215, 24, 0, 0, 1, {.text = &gWindowText56B700}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 48, 0, 0, 1, {.text = &gWindowText56B708}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic56B5E0}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements55F5C0[14] { // 原全局：0x0055F5C0。
    {WindowElementType_EventArea, 0, 0, 640, 30, 1, {.unusedValue = 0}, &WindowBottomBar_HandleButtons},
    {WindowElementType_Graphic, 10, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F4C0}, &WindowBottomBar_HandleButtons},
    {WindowElementType_Graphic, 86, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F4D8}, &WindowBottomBar_HandleButtons},
    {WindowElementType_Graphic, 163, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F4F0}, &WindowBottomBar_HandleButtons},
    {WindowElementType_Graphic, 239, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F508}, &WindowBottomBar_HandleButtons},
    {WindowElementType_Graphic, 316, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F520}, &WindowBottomBar_HandleButtons},
    {WindowElementType_Graphic, 392, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F538}, &WindowBottomBar_HandleButtons},
    {WindowElementType_Graphic, 464, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F550}, &WindowBottomBar_HandleButtons},
    {WindowElementType_Graphic, 555, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F568}, &WindowBottomBar_HandleButtons},
    {WindowElementType_Text, 565, 11, 0, 0, 1, {.text = &gWindowText55F5B8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F580}, &WindowElement_NoOp},
    {WindowElementType_Text, 21, 10, 0, 0, 1, {.text = &gWindowText55F5B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 408, 10, 0, 0, 1, {.text = &gWindowText55F5B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F598}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements55FD60[73] { // 原全局：0x0055FD60。
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText55FCF0}, &WindowElement_NoOp},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText55FCF8}, &WindowElement_NoOp},
    {WindowElementType_Text, 70, 101, 0, 0, 1, {.text = &gWindowText55FD00}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 31, 0, 0, 1, {.text = &gWindowText55FD08}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 49, 0, 0, 1, {.text = &gWindowText55FD10}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 68, 0, 0, 1, {.text = &gWindowText55FD18}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 86, 0, 0, 1, {.text = &gWindowText55FD20}, &WindowElement_NoOp},
    {WindowElementType_Text, 165, 104, 0, 0, 1, {.text = &gWindowText55FD28}, &WindowElement_NoOp},
    {WindowElementType_Text, 89, 123, 0, 0, 1, {.text = &gWindowText55FD30}, &WindowElement_NoOp},
    {WindowElementType_Text, 89, 141, 0, 0, 1, {.text = &gWindowText55FD38}, &WindowElement_NoOp},
    {WindowElementType_Text, 89, 158, 0, 0, 1, {.text = &gWindowText55FD40}, &WindowElement_NoOp},
    {WindowElementType_Text, 89, 176, 0, 0, 1, {.text = &gWindowText55FD48}, &WindowElement_NoOp},
    {WindowElementType_Text, 89, 194, 0, 0, 1, {.text = &gWindowText55FD50}, &WindowElement_NoOp},
    {WindowElementType_Text, 89, 211, 0, 0, 1, {.text = &gWindowText55FD58}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic55F780}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 48, 103, 0, 0, 1, {.graphic = &gWindowGraphic55F798}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 60, 104, 0, 0, 1, {.graphic = &gWindowGraphic55F7B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic55F7C8}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 151, 230, 0, 0, 1, {.graphic = &gWindowGraphic55F7E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic55F7F8}, &WindowPlayerProfile_HandleButtons},
    {WindowElementType_Graphic, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic55F810}, &WindowPlayerProfile_HandleButtons},
    {WindowElementType_Graphic, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic55FCD8}, &WindowPlayerProfile_HandleButtons},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F858}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 217, 200, 36, 25, 1, {.graphic = &gWindowGraphic55F828}, &WindowPlayerProfile_HandleButtons},
    {WindowElementType_Graphic, 253, 200, 36, 25, 1, {.graphic = &gWindowGraphic55F840}, &WindowPlayerProfile_HandleButtons},
    {WindowElementType_Graphic, 229, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 237, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 245, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 253, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 261, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 269, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 285, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 293, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 301, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 229, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 237, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 245, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 253, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 261, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 269, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 285, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 293, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 301, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 229, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 237, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 245, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 253, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 261, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 269, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 285, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 293, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 301, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 229, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 237, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 245, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 253, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 261, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 269, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 285, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 293, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 301, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 195, 130, 0, 0, 1, {.graphic = &gWindowGraphic55FC30}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 195, 147, 0, 0, 1, {.graphic = &gWindowGraphic55FC48}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 195, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FC60}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 195, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FC78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 29, 0, 0, 1, {.graphic = &gWindowGraphic55FC90}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55FCA8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic55FCC0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements560840[43] { // 原全局：0x00560840。
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText560758}, &WindowElement_NoOp},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText560760}, &WindowElement_NoOp},
    {WindowElementType_Text, 130, 31, 0, 0, 1, {.text = &gWindowText560768}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 48, 0, 0, 1, {.text = &gWindowText560770}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 64, 0, 0, 1, {.text = &gWindowText560778}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 80, 0, 0, 1, {.text = &gWindowText560780}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 96, 0, 0, 1, {.text = &gWindowText560788}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 112, 0, 0, 1, {.text = &gWindowText560790}, &WindowElement_NoOp},
    {WindowElementType_Text, 46, 159, 0, 0, 1, {.text = &gWindowText560798}, &WindowElement_NoOp},
    {WindowElementType_Text, 46, 175, 0, 0, 1, {.text = &gWindowText5607A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 46, 191, 0, 0, 1, {.text = &gWindowText5607A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 46, 207, 0, 0, 1, {.text = &gWindowText560830}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 159, 0, 0, 1, {.text = &gWindowText5607B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 175, 0, 0, 1, {.text = &gWindowText5607B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 191, 0, 0, 1, {.text = &gWindowText5607C0}, &WindowElement_NoOp},
    {WindowElementType_Text, 124, 207, 0, 0, 1, {.text = &gWindowText560838}, &WindowElement_NoOp},
    {WindowElementType_Text, 212, 48, 0, 0, 1, {.text = &gWindowText5607C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 212, 64, 0, 0, 1, {.text = &gWindowText5607D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 212, 80, 0, 0, 1, {.text = &gWindowText5607D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 48, 0, 0, 1, {.text = &gWindowText5607E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 64, 0, 0, 1, {.text = &gWindowText5607E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 80, 0, 0, 1, {.text = &gWindowText5607F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 212, 137, 0, 0, 1, {.text = &gWindowText5607F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 212, 153, 0, 0, 1, {.text = &gWindowText560800}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 137, 0, 0, 1, {.text = &gWindowText560808}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 153, 0, 0, 1, {.text = &gWindowText560810}, &WindowElement_NoOp},
    {WindowElementType_Text, 212, 191, 0, 0, 1, {.text = &gWindowText560818}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 191, 0, 0, 1, {.text = &gWindowText560820}, &WindowElement_NoOp},
    {WindowElementType_Text, 284, 207, 0, 0, 1, {.text = &gWindowText560828}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 94, 48, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 94, 64, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 94, 80, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 94, 96, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 94, 112, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic560680}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 151, 230, 38, 25, 1, {.graphic = &gWindowGraphic560698}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic5606B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic5606C8}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic560740}, &WindowPlayerAttributes_HandleButtons},
    {WindowElementType_Graphic, 15, 28, 0, 0, 1, {.graphic = &gWindowGraphic5606F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic560710}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic560728}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements560F50[37] { // 原全局：0x00560F50。
    {WindowElementType_Input, 105, 31, 144, 12, 1, {.unusedValue = 0}, &WindowPlayerTitles_EditDraft},
    {WindowElementType_Graphic, 239, 104, 0, 14, 1, {.graphic = &gWindowGraphic560ED8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 239, 104, 11, 72, 1, {.mouseFlags = &gWindowMouseFlagsD676F0}, &WindowPlayerTitles_DragScroll},
    {WindowElementType_Graphic, 239, 93, 11, 11, 1, {.graphic = &gWindowGraphic560EF0}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 239, 175, 11, 11, 1, {.graphic = &gWindowGraphic560F08}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_EventArea, 0, 0, 315, 261, 1, {.unusedValue = 0}, &WindowPlayerTitles_Scroll},
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText560F38}, &WindowElement_NoOp},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText560F40}, &WindowElement_NoOp},
    {WindowElementType_Text, 105, 31, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 105, 64, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 94, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 110, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 126, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 142, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 158, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 174, 0, 0, 1, {.text = &gWindowText560F48}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic560DA0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 151, 230, 38, 25, 1, {.graphic = &gWindowGraphic560DB8}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic560DD0}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic560DE8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic560F20}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 260, 27, 49, 17, 1, {.graphic = &gWindowGraphic560E00}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 260, 61, 49, 17, 1, {.graphic = &gWindowGraphic560E18}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 260, 175, 49, 17, 1, {.graphic = &gWindowGraphic560E30}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 220, 203, 18, 18, 1, {.graphic = &gWindowGraphic560E48}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 199, 203, 18, 18, 1, {.graphic = &gWindowGraphic560E60}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_Graphic, 23, 92, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle},
    {WindowElementType_Graphic, 23, 108, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle},
    {WindowElementType_Graphic, 23, 124, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle},
    {WindowElementType_Graphic, 23, 140, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle},
    {WindowElementType_Graphic, 23, 156, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle},
    {WindowElementType_Graphic, 23, 172, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &WindowPlayerTitles_SelectTitle},
    {WindowElementType_Graphic, 15, 28, 0, 0, 1, {.graphic = &gWindowGraphic560E90}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic560EA8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic560EC0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 0, 0, 374, 281, 1, {.mouseFlags = &gWindowMouseFlagsD6772C}, &WindowPlayerTitles_HandleButtons},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements561558[34] { // 原全局：0x00561558。
    {WindowElementType_Text, 70, 10, 0, 0, 1, {.text = &gWindowText561510}, &WindowElement_NoOp},
    {WindowElementType_Text, 272, 11, 0, 0, 1, {.text = &gWindowText561518}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 39, 0, 0, 1, {.text = &gWindowText561520}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 59, 0, 0, 1, {.text = &gWindowText561528}, &WindowElement_NoOp},
    {WindowElementType_Text, 140, 79, 0, 0, 1, {.text = &gWindowText561530}, &WindowElement_NoOp},
    {WindowElementType_Input, 90, 105, 168, 14, 1, {.unusedValue = 0}, &WindowOwnProfile_EditField},
    {WindowElementType_Input, 90, 129, 168, 14, 1, {.unusedValue = 0}, &WindowOwnProfile_EditField},
    {WindowElementType_Input, 90, 153, 168, 14, 1, {.unusedValue = 0}, &WindowOwnProfile_EditField},
    {WindowElementType_Input, 90, 177, 168, 54, 1, {.unusedValue = 0}, &WindowOwnProfile_EditField},
    {WindowElementType_Text, 97, 108, 0, 0, 1, {.text = &gWindowText561540}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 130, 0, 0, 1, {.text = &gWindowText561548}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 152, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 170, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 184, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 198, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp},
    {WindowElementType_Text, 97, 212, 0, 0, 1, {.text = &gWindowText561550}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic5613F0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 15, 29, 64, 72, 1, {.graphic = &gWindowGraphic5614F8}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic5613F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 151, 230, 38, 25, 1, {.graphic = &gWindowGraphic561408}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic561420}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic561438}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic561450}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 266, 105, 20, 20, 1, {.graphic = &gWindowGraphic5614B0}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 266, 128, 20, 20, 1, {.graphic = &gWindowGraphic5614B0}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 266, 151, 20, 20, 1, {.graphic = &gWindowGraphic5614B0}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 73, 102, 20, 20, 1, {.graphic = &gWindowGraphic5614E0}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 73, 124, 20, 20, 1, {.graphic = &gWindowGraphic5614E0}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 73, 146, 20, 20, 1, {.graphic = &gWindowGraphic5614E0}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 257, 32, 40, 30, 1, {.graphic = &gWindowGraphic5614C8}, &WindowProfile_HandleButtons},
    {WindowElementType_Graphic, 15, 36, 0, 0, 1, {.graphic = &gWindowGraphic561468}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic561480}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic561498}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 315, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements561B78[30] { // 原全局：0x00561B78。
    {WindowElementType_Graphic, 222, 9, 11, 11, 1, {.graphic = &gWindowGraphic561A40}, &WindowRuntime_ButtonClose},
    {WindowElementType_Text, 82, 12, 0, 0, 1, {.text = &gWindowText561B48}, &WindowElement_NoOp},
    {WindowElementType_Text, 87, 12, 0, 0, 1, {.text = &gWindowText561B68}, &WindowElement_NoOp},
    {WindowElementType_Text, 66, 32, 0, 0, 1, {.text = &gWindowText561B50}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 28, 50, 15, 1, {.graphic = &gWindowGraphic561AD0}, &WindowSkill_ButtonBack},
    {WindowElementType_Text, 103, 31, 0, 0, 1, {.text = &gWindowText561B58}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 184, 28, 50, 15, 1, {.graphic = &gWindowGraphic561AE8}, &WindowSkill_ButtonUse},
    {WindowElementType_Text, 95, 31, 0, 0, 1, {.text = &gWindowText561B70}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 28, 49, 17, 1, {.graphic = &gWindowGraphic561B00}, &WindowSkill_ButtonPetSkillBack},
    {WindowElementType_Text, 190, 13, 0, 0, 1, {.text = &gWindowText561B60}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 223, 57, 0, 14, 1, {.graphic = &gWindowGraphic561A88}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 223, 57, 11, 140, 1, {.mouseFlags = &gWindowMouseFlagsD67780}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 223, 47, 11, 11, 1, {.graphic = &gWindowGraphic561AA0}, &WindowSkill_ButtonPreviousRow},
    {WindowElementType_Graphic, 223, 196, 11, 11, 1, {.graphic = &gWindowGraphic561AB8}, &WindowSkill_ButtonNextRow},
    {WindowElementType_Graphic, 195, 28, 18, 18, 1, {.graphic = &gWindowGraphic561B18}, &WindowSkill_ButtonPreviousPage},
    {WindowElementType_Graphic, 215, 28, 18, 18, 1, {.graphic = &gWindowGraphic561B30}, &WindowSkill_ButtonNextPage},
    {WindowElementType_EventArea, 0, 0, 241, 258, 1, {.unusedValue = 0}, &WindowSkill_Scroll},
    {WindowElementType_EventArea, 14, 46, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 62, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 78, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 94, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 110, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 126, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 142, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 158, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 174, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_EventArea, 14, 190, 207, 16, 1, {.unusedValue = 0}, &WindowSkill_HandleSkillRow},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic561A58}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic561A70}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 243, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements5620E0[31] { // 原全局：0x005620E0。
    {WindowElementType_EventArea, 0, 0, 330, 110, 1, {.unusedValue = 0}, &WindowCraft_DrawInvalidEngravingNotice},
    {WindowElementType_EventArea, 0, 0, 330, 110, 1, {.unusedValue = 0}, &WindowCraft_DrawEmptyEngravingNotice},
    {WindowElementType_Graphic, 543, 8, 11, 11, 1, {.graphic = &gWindowGraphic561FE0}, &WindowCraft_ButtonCloseIcon},
    {WindowElementType_Animation, 103, 225, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5620DC}, &WindowElement_NoOp},
    {WindowElementType_Animation, 103, 225, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5620D8}, &WindowElement_NoOp},
    {WindowElementType_Animation, 103, 225, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5620DC}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 20, 57, 150, 12, 1, {.unusedValue = 0}, &WindowCraft_HandleEngravingInput},
    {WindowElementType_Input, 20, 53, 0, 0, 1, {.unusedValue = 0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 54, 49, 17, 1, {.graphic = &gWindowGraphic5620B8}, &WindowCraft_ButtonExecute},
    {WindowElementType_Text, 20, 29, 0, 0, 1, {.text = &gWindowText5620D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 43, 0, 0, 1, {.text = &gWindowText5620D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 57, 0, 0, 1, {.text = &gWindowText5620D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 71, 0, 0, 1, {.text = &gWindowText5620D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 85, 0, 0, 1, {.text = &gWindowText5620D0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 20, 29, 0, 0, 1, {.graphic = &gWindowGraphic562010}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 190, 75, 48, 48, 1, {.unusedValue = 0}, &WindowCraft_HandleMaterialSlot},
    {WindowElementType_EventArea, 190, 125, 48, 48, 1, {.unusedValue = 0}, &WindowCraft_HandleMaterialSlot},
    {WindowElementType_EventArea, 190, 175, 48, 48, 1, {.unusedValue = 0}, &WindowCraft_HandleMaterialSlot},
    {WindowElementType_EventArea, 243, 75, 48, 48, 1, {.unusedValue = 0}, &WindowCraft_HandleMaterialSlot},
    {WindowElementType_EventArea, 243, 125, 48, 48, 1, {.unusedValue = 0}, &WindowCraft_HandleMaterialSlot},
    {WindowElementType_EventArea, 243, 175, 48, 48, 1, {.unusedValue = 0}, &WindowCraft_HandleMaterialSlot},
    {WindowElementType_Graphic, 231, 175, 48, 48, 1, {.graphic = &gWindowGraphic5620A0}, &WindowCraft_HandleSingleMaterial},
    {WindowElementType_Graphic, 190, 27, 0, 0, 1, {.graphic = &gWindowGraphic562088}, &WindowCraft_DrawResultImage},
    {WindowElementType_Graphic, 231, 45, 0, 0, 1, {.graphic = &gWindowGraphic562088}, &WindowCraft_DrawResultImage},
    {WindowElementType_Graphic, 188, 73, 0, 0, 1, {.graphic = &gWindowGraphic562028}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 21, 209, 49, 17, 1, {.graphic = &gWindowGraphic562040}, &WindowCraft_ButtonExecute},
    {WindowElementType_Graphic, 21, 209, 49, 17, 1, {.graphic = &gWindowGraphic562058}, &WindowCraft_ButtonContinue},
    {WindowElementType_Graphic, 126, 209, 49, 17, 1, {.graphic = &gWindowGraphic562070}, &WindowCraft_ButtonClose},
    {WindowElementType_EventArea, 307, 28, 248, 204, 1, {.unusedValue = 0}, &WindowCraft_HandleInventory},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic561FF8}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 566, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements562620[15] { // 原全局：0x00562620。
    {WindowElementType_Graphic, 178, 9, 11, 11, 1, {.graphic = &gWindowGraphic562568}, &WindowRuntime_ButtonClose},
    {WindowElementType_Animation, 107, 204, 0, 0, 0, {.animationImageNo = &gWindowAnimationImage56261C}, &WindowElement_NoOp},
    {WindowElementType_Animation, 107, 204, 0, 0, 0, {.animationImageNo = &gWindowAnimationImage562618}, &WindowElement_NoOp},
    {WindowElementType_Animation, 107, 204, 0, 0, 0, {.animationImageNo = &gWindowAnimationImage56261C}, &WindowElement_NoOp},
    {WindowElementType_Text, 16, 27, 0, 0, 1, {.text = &gWindowText562610}, &WindowElement_NoOp},
    {WindowElementType_Text, 16, 41, 0, 0, 1, {.text = &gWindowText562610}, &WindowElement_NoOp},
    {WindowElementType_Text, 16, 55, 0, 0, 1, {.text = &gWindowText562610}, &WindowElement_NoOp},
    {WindowElementType_Text, 16, 69, 0, 0, 1, {.text = &gWindowText562610}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 43, 0, 0, 1, {.graphic = &gWindowGraphic562598}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 40, 0, 0, 1, {.graphic = &gWindowGraphic5625F8}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 143, 29, 0, 0, 1, {.unusedValue = 0}, &WindowGather_DrawResultImage},
    {WindowElementType_Graphic, 29, 203, 49, 17, 1, {.graphic = &gWindowGraphic5625C8}, &WindowGather_ButtonContinue},
    {WindowElementType_Graphic, 129, 203, 49, 17, 1, {.graphic = &gWindowGraphic5625E0}, &WindowGather_ButtonClose},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic562580}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 201, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements562930[14] { // 原全局：0x00562930。
    {WindowElementType_Graphic, 178, 9, 11, 11, 1, {.graphic = &gWindowGraphic5628A8}, &WindowRuntime_ButtonClose},
    {WindowElementType_Animation, 107, 224, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56292C}, &WindowElement_NoOp},
    {WindowElementType_Animation, 107, 224, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage562928}, &WindowElement_NoOp},
    {WindowElementType_Animation, 107, 224, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56292C}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 28, 0, 0, 1, {.text = &gWindowText562920}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 42, 0, 0, 1, {.text = &gWindowText562920}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 56, 0, 0, 1, {.text = &gWindowText562920}, &WindowElement_NoOp},
    {WindowElementType_Text, 25, 70, 0, 0, 1, {.text = &gWindowText562920}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 24, 28, 0, 0, 1, {.graphic = &gWindowGraphic5628D8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 24, 55, 0, 0, 1, {.graphic = &gWindowGraphic5628D8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 29, 203, 49, 17, 1, {.graphic = &gWindowGraphic5628F0}, &WindowSkillResult_ButtonContinue},
    {WindowElementType_Graphic, 129, 203, 49, 17, 1, {.graphic = &gWindowGraphic562908}, &WindowSkillResult_ButtonClose},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5628C0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 201, 5, 18, 79, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements562CD8[24] { // 原全局：0x00562CD8。
    {WindowElementType_Graphic, 258, 8, 11, 11, 1, {.graphic = &gWindowGraphic562B98}, &WindowInventory_ButtonClose},
    {WindowElementType_Graphic, 221, 28, 49, 17, 1, {.graphic = &gWindowGraphic562BC8}, &WindowInventory_HandleMoney},
    {WindowElementType_Number, 198, 31, 0, 0, 1, {.number = &gWindowNumber562CB0}, &WindowInventory_RefreshMoney},
    {WindowElementType_Number, 99, 31, 0, 0, 1, {.number = &gWindowNumber562CB8}, &WindowInventory_RefreshExtraValue},
    {WindowElementType_Graphic, 8, 0, 278, 259, 1, {.graphic = &gWindowGraphic562BB0}, &WindowInventory_HandleItems},
    {WindowElementType_Animation, -81, 163, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage562C80}, &WindowElement_NoOp},
    {WindowElementType_Text, -81, 28, 0, 0, 0, {.text = &gWindowText562C70}, &WindowElement_NoOp},
    {WindowElementType_Text, -81, 48, 0, 0, 0, {.text = &gWindowText562C78}, &WindowElement_NoOp},
    {WindowElementType_EventArea, -114, 188, 33, 17, 0, {.unusedValue = 0}, &WindowInventory_TogglePreview},
    {WindowElementType_EventArea, -81, 188, 33, 17, 0, {.unusedValue = 0}, &WindowInventory_CycleEquipment},
    {WindowElementType_EventArea, -121, 73, 78, 113, 0, {.unusedValue = 0}, &WindowInventory_RotatePreview},
    {WindowElementType_Graphic, -170, 73, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -41, 73, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -170, 123, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -41, 123, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -170, 23, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -41, 23, 48, 48, 0, {.graphic = &gWindowGraphic562C58}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -16, 17, 293, 222, 0, {.graphic = &gWindowGraphic562C40}, &WindowInventory_HandleEquipment},
    {WindowElementType_Graphic, 2, 17, 19, 46, 1, {.graphic = &gWindowGraphic562BE0}, &WindowInventory_TabEquipment},
    {WindowElementType_Graphic, 2, 62, 19, 46, 1, {.graphic = &gWindowGraphic562BF8}, &WindowInventory_TabPet},
    {WindowElementType_Graphic, 2, 107, 19, 46, 1, {.graphic = &gWindowGraphic562C10}, &WindowInventory_TabPet},
    {WindowElementType_Graphic, 2, 152, 19, 46, 1, {.graphic = &gWindowGraphic562C28}, &WindowInventory_TabPet},
    {WindowElementType_EventArea, -197, 17, 293, 222, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
    {WindowElementType_EventArea, 282, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements563500[259] { // 原全局：0x00563500。
    {WindowElementType_Graphic, 302, 8, 11, 11, 1, {.graphic = &gWindowGraphic563298}, &WindowRuntime_ButtonClose},
    {WindowElementType_Text, 23, 34, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 156, 34, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 47, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 60, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 133, 49, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 82, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 156, 82, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 95, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 108, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 133, 97, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 130, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 156, 130, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 143, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 156, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 133, 145, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 178, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 156, 178, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 191, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 204, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 133, 193, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp},
    {WindowElementType_Text, 23, 226, 0, 0, 1, {.text = &gWindowText5634D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 156, 226, 0, 0, 1, {.text = &gWindowText5634E0}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 239, 0, 0, 1, {.text = &gWindowText5634E8}, &WindowElement_NoOp},
    {WindowElementType_Text, 52, 252, 0, 0, 1, {.text = &gWindowText5634F0}, &WindowElement_NoOp},
    {WindowElementType_Text, 133, 241, 0, 0, 1, {.text = &gWindowText5634F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 33, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 43, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 53, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 63, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 81, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 91, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 101, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 111, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 129, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 139, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 149, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 159, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 177, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 187, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 197, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 207, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 225, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 235, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 245, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 190, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 196, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 202, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 208, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 214, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 226, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 232, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 238, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 244, 255, 0, 0, 1, {.graphic = &gWindowGraphic5632F8}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 259, 54, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 102, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 150, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 198, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 246, 23, 18, 1, {.graphic = &gWindowGraphic563478}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 54, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 102, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 150, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 198, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 246, 23, 18, 1, {.graphic = &gWindowGraphic563490}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 34, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 82, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 130, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 178, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 259, 226, 23, 18, 1, {.graphic = &gWindowGraphic5634A8}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 34, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 82, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 130, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 178, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 285, 226, 23, 18, 1, {.graphic = &gWindowGraphic5634C0}, &WindowPetList_HandleButtons},
    {WindowElementType_Graphic, 15, 28, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 76, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 124, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 172, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 220, 0, 0, 1, {.graphic = &gWindowGraphic5632E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5632B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 14, 26, 0, 0, 1, {.graphic = &gWindowGraphic5632C8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 15, 28, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons},
    {WindowElementType_MouseArea, 15, 76, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons},
    {WindowElementType_MouseArea, 15, 124, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons},
    {WindowElementType_MouseArea, 15, 172, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons},
    {WindowElementType_MouseArea, 15, 220, 242, 47, 1, {.mouseFlags = &gWindowMouseFlagsD677A0}, &WindowPetList_HandleButtons},
    {WindowElementType_EventArea, 323, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements56C800[8] { // 原全局：0x0056C800。
    {WindowElementType_Graphic, 302, 9, 11, 11, 1, {.graphic = &gWindowGraphic56C798}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 258, 173, 57, 17, 1, {.graphic = &gWindowGraphic56C7E0}, &WindowBattlePets_ButtonReturn},
    {WindowElementType_EventArea, 15, 28, 300, 48, 1, {.unusedValue = 0}, &WindowBattlePets_SelectRow},
    {WindowElementType_EventArea, 15, 76, 300, 48, 1, {.unusedValue = 0}, &WindowBattlePets_SelectRow},
    {WindowElementType_EventArea, 15, 124, 300, 48, 1, {.unusedValue = 0}, &WindowBattlePets_SelectRow},
    {WindowElementType_Graphic, 0, 0, 320, 353, 1, {.graphic = &gWindowGraphic56C7B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 25, 320, 353, 1, {.graphic = &gWindowGraphic56C7C8}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 320, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements56B8D8[2] { // 原全局：0x0056B8D8。
    {WindowElementType_Text, 32, 46, 0, 0, 1, {.text = &gWindowText56B8D0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 32, 46, 83, 16, 1, {.unusedValue = 0}, &WindowSpectator_ButtonEnd},
};

WindowElementPrototype gWindowElements56B9B8[66] { // 原全局：0x0056B9B8。
    {WindowElementType_EventArea, 0, 27, 490, 274, 1, {.unusedValue = 0}, &WindowBattleResult_ButtonClose},
    {WindowElementType_Graphic, 268, 40, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 268, 59, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 268, 78, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 268, 97, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 72, 47, 0, 0, 1, {.graphic = &gWindowGraphic56B960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 72, 66, 0, 0, 1, {.graphic = &gWindowGraphic56B960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 72, 85, 0, 0, 1, {.graphic = &gWindowGraphic56B960}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 72, 104, 0, 0, 1, {.graphic = &gWindowGraphic56B960}, &WindowElement_NoOp},
    {WindowElementType_Text, 86, 46, 0, 0, 1, {.text = &gWindowText56B990}, &WindowElement_NoOp},
    {WindowElementType_Text, 86, 65, 0, 0, 1, {.text = &gWindowText56B990}, &WindowElement_NoOp},
    {WindowElementType_Text, 86, 84, 0, 0, 1, {.text = &gWindowText56B990}, &WindowElement_NoOp},
    {WindowElementType_Text, 86, 103, 0, 0, 1, {.text = &gWindowText56B990}, &WindowElement_NoOp},
    {WindowElementType_Text, 226, 46, 0, 0, 1, {.text = &gWindowText56B998}, &WindowElement_NoOp},
    {WindowElementType_Text, 226, 65, 0, 0, 1, {.text = &gWindowText56B998}, &WindowElement_NoOp},
    {WindowElementType_Text, 226, 84, 0, 0, 1, {.text = &gWindowText56B998}, &WindowElement_NoOp},
    {WindowElementType_Text, 226, 103, 0, 0, 1, {.text = &gWindowText56B998}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 44, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 60, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 76, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 92, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 108, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 124, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 140, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 156, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 172, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 188, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 204, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 220, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 236, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 468, 252, 0, 0, 1, {.graphic = &gWindowGraphic56B978}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 50, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 66, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 82, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 98, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 114, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 130, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 146, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 162, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 178, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 194, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 210, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 226, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 242, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 283, 258, 0, 0, 1, {.text = &gWindowText56B9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 50, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 66, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 82, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 98, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 114, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 130, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 146, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 162, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 178, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 194, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 210, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 226, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 242, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 448, 258, 0, 0, 1, {.text = &gWindowText56B9B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 72, 171, 0, 0, 1, {.text = &gWindowText56B9A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 72, 220, 0, 0, 1, {.text = &gWindowText56B9A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 72, 269, 0, 0, 1, {.text = &gWindowText56B9A0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic56B918}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 27, 0, 0, 1, {.graphic = &gWindowGraphic56B930}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 9, 0, 481, 27, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
    {WindowElementType_EventArea, 493, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements56C2C8[4] { // 原全局：0x0056C2C8。
    {WindowElementType_Graphic, 42, 106, 66, 17, 1, {.graphic = &gWindowGraphic56C2A0}, &WindowRuntime_ButtonCloseResult},
    {WindowElementType_Text, 20, 10, 0, 0, 1, {.text = &gWindowText56C2B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 40, 0, 0, 1, {.text = &gWindowText56C2C0}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 65, 0, 0, 1, {.text = &gWindowText56C2C0}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements56C9B8[2] { // 原全局：0x0056C9B8。
    {WindowElementType_Text, 78, 40, 0, 0, 1, {.text = &gWindowText56C9A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 68, 40, 0, 0, 1, {.text = &gWindowText56C9B0}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements56C430[22] { // 原全局：0x0056C430。
    {WindowElementType_Graphic, 258, 8, 11, 11, 1, {.graphic = &gWindowGraphic56C348}, &WindowRuntime_ButtonClose},
    {WindowElementType_Number, 198, 31, 0, 0, 1, {.number = &gWindowNumber56C420}, &WindowBattleItems_RefreshMoney},
    {WindowElementType_Number, 99, 31, 0, 0, 1, {.number = &gWindowNumber56C428}, &WindowBattleItems_RefreshExtraValue},
    {WindowElementType_Graphic, 8, 0, 276, 253, 1, {.graphic = &gWindowGraphic56C360}, &WindowBattleItems_HandleInventory},
    {WindowElementType_Animation, -81, 163, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56C418}, &WindowElement_NoOp},
    {WindowElementType_Text, -81, 28, 0, 0, 0, {.text = &gWindowText562C70}, &WindowElement_NoOp},
    {WindowElementType_Text, -81, 48, 0, 0, 0, {.text = &gWindowText562C78}, &WindowElement_NoOp},
    {WindowElementType_EventArea, -114, 188, 33, 17, 0, {.unusedValue = 0}, &WindowBattleItems_TogglePreview},
    {WindowElementType_EventArea, -81, 188, 33, 17, 0, {.unusedValue = 0}, &WindowBattleItems_CycleEquipment},
    {WindowElementType_EventArea, -121, 73, 78, 113, 0, {.unusedValue = 0}, &WindowBattleItems_RotatePreview},
    {WindowElementType_Graphic, -170, 73, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -41, 73, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -170, 123, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -41, 123, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -170, 23, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, -41, 23, 48, 48, 0, {.graphic = &gWindowGraphic56C3F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 2, 17, 19, 46, 1, {.graphic = &gWindowGraphic56C378}, &WindowBattleItems_TabEquipment},
    {WindowElementType_Graphic, 2, 62, 19, 46, 1, {.graphic = &gWindowGraphic56C3A8}, &WindowBattleItems_TabPet},
    {WindowElementType_Graphic, 2, 107, 19, 46, 1, {.graphic = &gWindowGraphic56C3C0}, &WindowBattleItems_TabPet},
    {WindowElementType_Graphic, 2, 152, 19, 46, 1, {.graphic = &gWindowGraphic56C3D8}, &WindowBattleItems_TabPet},
    {WindowElementType_Graphic, -197, 17, 212, 210, 1, {.graphic = &gWindowGraphic56C390}, &WindowBattleItems_HandleEquipment},
    {WindowElementType_EventArea, 320, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements56CA88[19] { // 原全局：0x0056CA88。
    {WindowElementType_Graphic, 0, 0, 66, 17, 1, {.graphic = &gWindowGraphic56C9F8}, &WindowDialog_HandleButtons},
    {WindowElementType_Graphic, 0, 20, 66, 17, 1, {.graphic = &gWindowGraphic56CA10}, &WindowDialog_HandleButtons},
    {WindowElementType_Graphic, 0, 20, 66, 17, 1, {.graphic = &gWindowGraphic56CA28}, &WindowDialog_HandleButtons},
    {WindowElementType_Graphic, 60, 0, 66, 17, 1, {.graphic = &gWindowGraphic56CA40}, &WindowDialog_HandleButtons},
    {WindowElementType_Graphic, 60, 20, 66, 17, 1, {.graphic = &gWindowGraphic56CA58}, &WindowDialog_HandleButtons},
    {WindowElementType_Graphic, 120, 0, 66, 17, 1, {.graphic = &gWindowGraphic56CA70}, &WindowDialog_HandleButtons},
    {WindowElementType_EventArea, 120, 0, 28, 8, 1, {.unusedValue = 0}, &WindowDialog_HandleTextSelection},
    {WindowElementType_Input, 80, 42, 0, 0, 1, {.unusedValue = 0}, &WindowElement_NoOp},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp},
    {WindowElementType_Animation, 0, 0, 0, 0, 1, {.animationImageNo = &gWindowAnimationImageD6780C}, &WindowElement_NoOp},
    // 原运行第19项与下一文本/元素源字节重叠；值1不是函数，visible0阻止正常调用。
    {WindowElementType_EventArea, 0x593850, 3, 0, 44, 0, {.unusedValue = 0},
        reinterpret_cast<WindowElementCallback>(std::uintptr_t{1})},
};

WindowElementPrototype gWindowElements56CDB0[19] { // 原全局：0x0056CDB0。
    {WindowElementType_Graphic, 270, 60, 0, 14, 1, {.graphic = &gWindowGraphic56CCF0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 270, 60, 11, 85, 1,
        {.mouseFlags = reinterpret_cast<std::uint32_t*>(&gWindowGraphic56CCF0.imageNo)}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 270, 50, 11, 11, 1, {.graphic = &gWindowGraphic56CD08}, &WindowTradeTarget_ButtonPreviousRow},
    {WindowElementType_Graphic, 270, 144, 11, 11, 1, {.graphic = &gWindowGraphic56CD20}, &WindowTradeTarget_ButtonNextRow},
    {WindowElementType_Graphic, 282, 47, 18, 18, 1, {.graphic = &gWindowGraphic56CD50}, &WindowTradeTarget_ButtonPreviousPage},
    {WindowElementType_Graphic, 282, 140, 18, 18, 1, {.graphic = &gWindowGraphic56CD68}, &WindowTradeTarget_ButtonNextPage},
    {WindowElementType_EventArea, 0, 0, 320, 192, 1, {.unusedValue = 0}, &WindowTradeTarget_Scroll},
    {WindowElementType_Graphic, 135, 159, 49, 17, 1, {.graphic = &gWindowGraphic56CD38}, &WindowTradeTarget_ButtonCancel},
    {WindowElementType_Text, 70, 25, 0, 0, 1, {.text = &gWindowText56CDA8}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 55, 0, 0, 1, {.text = &gWindowTexts56CD80[0]}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 75, 0, 0, 1, {.text = &gWindowTexts56CD80[1]}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 95, 0, 0, 1, {.text = &gWindowTexts56CD80[2]}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 115, 0, 0, 1, {.text = &gWindowTexts56CD80[3]}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 135, 0, 0, 1, {.text = &gWindowTexts56CD80[4]}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 40, 55, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 75, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 95, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 115, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 135, 200, 13, 1, {.unusedValue = 0}, &WindowTradeTarget_SelectRow},
};

WindowElementPrototype gWindowElements56D0B0[19] { // 原全局：0x0056D0B0。
    {WindowElementType_Graphic, 270, 60, 0, 14, 1, {.graphic = &gWindowGraphic56D010}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 270, 60, 11, 85, 1,
        {.mouseFlags = reinterpret_cast<std::uint32_t*>(&gWindowGraphic56D010.imageNo)}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 270, 50, 11, 11, 1, {.graphic = &gWindowGraphic56D028}, &WindowTextTarget_ButtonPreviousRow},
    {WindowElementType_Graphic, 270, 144, 11, 11, 1, {.graphic = &gWindowGraphic56D040}, &WindowTextTarget_ButtonNextRow},
    {WindowElementType_Graphic, 282, 47, 18, 18, 1, {.graphic = &gWindowGraphic56D070}, &WindowTextTarget_ButtonPreviousPage},
    {WindowElementType_Graphic, 282, 140, 18, 18, 1, {.graphic = &gWindowGraphic56D088}, &WindowTextTarget_ButtonNextPage},
    {WindowElementType_EventArea, 0, 0, 320, 192, 1, {.unusedValue = 0}, &WindowTextTarget_Scroll},
    {WindowElementType_Graphic, 135, 159, 49, 17, 1, {.graphic = &gWindowGraphic56D058}, &WindowTextTarget_ButtonCancel},
    {WindowElementType_Text, 70, 25, 0, 0, 1, {.text = &gWindowText56D0A8}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 55, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 75, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 95, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 115, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 135, 0, 0, 1, {.text = &gWindowText56D0A0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 40, 55, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow},
    {WindowElementType_EventArea, 40, 75, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow},
    {WindowElementType_EventArea, 40, 95, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow},
    {WindowElementType_EventArea, 40, 115, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow},
    {WindowElementType_EventArea, 40, 135, 200, 13, 1, {.unusedValue = 0}, &WindowTextTarget_SelectRow},
};

WindowElementPrototype gWindowElements56D3E8[34] { // 原全局：0x0056D3E8。
    {WindowElementType_Graphic, 151, 200, 66, 17, 1, {.graphic = &gWindowGraphic56D310}, &WindowAttributeTarget_ButtonCancel},
    {WindowElementType_Graphic, 303, 200, 66, 17, 1, {.graphic = &gWindowGraphic56D328}, &WindowAttributeTarget_ButtonBack},
    {WindowElementType_Text, 180, 25, 0, 0, 1, {.text = &gWindowText56D3D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 45, 0, 0, 1, {.text = &gWindowText56D3E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 57, 72, 0, 0, 1, {.graphic = &gWindowGraphic56D340}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 57, 92, 0, 0, 1, {.graphic = &gWindowGraphic56D358}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 57, 112, 0, 0, 1, {.graphic = &gWindowGraphic56D370}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 57, 132, 0, 0, 1, {.graphic = &gWindowGraphic56D388}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 57, 152, 0, 0, 1, {.graphic = &gWindowGraphic56D3A0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 57, 172, 0, 0, 1, {.graphic = &gWindowGraphic56D3B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 70, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 90, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 110, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 130, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 150, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 170, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 40, 70, 441, 13, 1, {.unusedValue = 0}, &WindowAttributeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 90, 441, 13, 1, {.unusedValue = 0}, &WindowAttributeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 110, 441, 13, 1, {.unusedValue = 0}, &WindowAttributeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 130, 441, 13, 1, {.unusedValue = 0}, &WindowAttributeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 150, 441, 13, 1, {.unusedValue = 0}, &WindowAttributeTarget_SelectRow},
    {WindowElementType_EventArea, 40, 170, 441, 13, 1, {.unusedValue = 0}, &WindowAttributeTarget_SelectRow},
    {WindowElementType_Text, 40, 70, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 90, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 110, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 130, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 150, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 170, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 70, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 90, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 110, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 130, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 150, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 40, 170, 0, 0, 1, {.text = &gWindowText56D3D0}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements56D838[3] { // 原全局：0x0056D838。
    {WindowElementType_Text, 40, 10, 0, 0, 1, {.text = &gWindowText56D828}, &WindowElement_NoOp},
    {WindowElementType_Animation, 40, 135, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56D830}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 99, 142, 108, 0, 1, {.unusedValue = 0}, &WindowShopChoice_SelectRow},
};

WindowElementPrototype gWindowElements5735C8[36] { // 原全局：0x005735C8。
    {WindowElementType_Graphic, 593, 8, 12, 12, 1, {.graphic = &gWindowGraphic573520}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Input, 27, 362, 503, 42, 1, {.unusedValue = 0}, &WindowMessageBoard_HandleInput},
    {WindowElementType_Graphic, 22, 38, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 135, 46, 0, 0, 1, {.text = &gWindowText5735B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 330, 46, 0, 0, 1, {.text = &gWindowText5735C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 530, 46, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Text, 100, 73, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 89, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 105, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 22, 114, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 135, 122, 0, 0, 1, {.text = &gWindowText5735B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 330, 122, 0, 0, 1, {.text = &gWindowText5735C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 530, 122, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Text, 100, 150, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 166, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 182, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 22, 190, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 135, 198, 0, 0, 1, {.text = &gWindowText5735B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 330, 198, 0, 0, 1, {.text = &gWindowText5735C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 530, 198, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Text, 100, 226, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 242, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 258, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 22, 266, 0, 0, 1, {.graphic = &gWindowGraphic5734D8}, &WindowElement_NoOp},
    {WindowElementType_Text, 135, 274, 0, 0, 1, {.text = &gWindowText5735B8}, &WindowElement_NoOp},
    {WindowElementType_Text, 330, 274, 0, 0, 1, {.text = &gWindowText5735C0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 530, 274, 49, 17, 1, {.graphic = &gWindowGraphic573538}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Text, 100, 302, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 318, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Text, 100, 334, 0, 0, 1, {.text = &gWindowText5735B0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 538, 364, 49, 17, 1, {.graphic = &gWindowGraphic573550}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Graphic, 538, 386, 49, 17, 1, {.graphic = &gWindowGraphic573568}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Graphic, 576, 342, 17, 17, 1, {.graphic = &gWindowGraphic573580}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Graphic, 536, 342, 17, 17, 1, {.graphic = &gWindowGraphic573598}, &WindowMessageBoard_HandleButtons},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5734F0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 15, 30, 0, 0, 1, {.graphic = &gWindowGraphic573508}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements573D80[66] { // 原全局：0x00573D80。
    {WindowElementType_EventArea, 0, 0, 640, 383, 1, {.unusedValue = 0}, &WindowShop_HandleConfirmation},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic573AF0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 294, 333, 66, 17, 1, {.graphic = &gWindowGraphic573C88}, &WindowShop_ButtonOk},
    {WindowElementType_Graphic, 467, 333, 66, 17, 1, {.graphic = &gWindowGraphic573CA0}, &WindowShop_ButtonCancel},
    {WindowElementType_Graphic, 371, 333, 25, 19, 1, {.graphic = &gWindowGraphic573CA0}, &WindowShop_ButtonAddAll},
    {WindowElementType_Text, 412, 44, 0, 14, 1, {.text = &gWindowText573CF8}, &WindowElement_NoOp},
    {WindowElementType_Number, 124, 37, 0, 0, 1, {.number = &gWindowNumber573D20}, &WindowElement_NoOp},
    {WindowElementType_Number, 255, 37, 0, 0, 1, {.number = &gWindowNumber573D28}, &WindowElement_NoOp},
    {WindowElementType_Number, 389, 30, 0, 0, 1, {.number = &gWindowNumber573D18}, &WindowElement_NoOp},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage573D10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 267, 85, 0, 14, 1, {.graphic = &gWindowGraphic573B50}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 267, 85, 11, 243, 1, {.mouseFlags = &gWindowMouseFlagsD678F0}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 267, 75, 11, 11, 1, {.graphic = &gWindowGraphic573B68}, &WindowShop_ButtonPreviousRow},
    {WindowElementType_Graphic, 267, 327, 11, 11, 1, {.graphic = &gWindowGraphic573B80}, &WindowShop_ButtonNextRow},
    {WindowElementType_Graphic, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic573CB8}, &WindowShop_ButtonPreviousPage},
    {WindowElementType_Graphic, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic573CD0}, &WindowShop_ButtonNextPage},
    {WindowElementType_EventArea, 15, 28, 274, 325, 1, {.unusedValue = 0}, &WindowShop_Scroll},
    {WindowElementType_Text, 154, 90, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 145, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 200, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 255, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 310, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 102, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 157, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 212, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 267, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 322, 0, 14, 1, {.text = &gWindowText573D00}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 120, 0, 14, 1, {.text = &gWindowText573D08}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 175, 0, 14, 1, {.text = &gWindowText573D08}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 230, 0, 14, 1, {.text = &gWindowText573D08}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 285, 0, 14, 1, {.text = &gWindowText573D08}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 340, 0, 14, 1, {.text = &gWindowText573D08}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 108, 0, 0, 1, {.number = &gWindowNumber573D30}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 163, 0, 0, 1, {.number = &gWindowNumber573D38}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 218, 0, 0, 1, {.number = &gWindowNumber573D40}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 273, 0, 0, 1, {.number = &gWindowNumber573D48}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 328, 0, 0, 1, {.number = &gWindowNumber573D50}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 90, 0, 0, 1, {.number = &gWindowNumber573D58}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 145, 0, 0, 1, {.number = &gWindowNumber573D60}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 200, 0, 0, 1, {.number = &gWindowNumber573D68}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 255, 0, 0, 1, {.number = &gWindowNumber573D70}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 310, 0, 0, 1, {.number = &gWindowNumber573D78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 80, 90, 11, 11, 1, {.graphic = &gWindowGraphic573B98}, &WindowShop_ButtonDecrease},
    {WindowElementType_Graphic, 80, 145, 11, 11, 1, {.graphic = &gWindowGraphic573BB0}, &WindowShop_ButtonDecrease},
    {WindowElementType_Graphic, 80, 200, 11, 11, 1, {.graphic = &gWindowGraphic573BC8}, &WindowShop_ButtonDecrease},
    {WindowElementType_Graphic, 80, 255, 11, 11, 1, {.graphic = &gWindowGraphic573BE0}, &WindowShop_ButtonDecrease},
    {WindowElementType_Graphic, 80, 310, 11, 11, 1, {.graphic = &gWindowGraphic573BF8}, &WindowShop_ButtonDecrease},
    {WindowElementType_Graphic, 95, 90, 11, 11, 1, {.graphic = &gWindowGraphic573C10}, &WindowShop_ButtonIncrease},
    {WindowElementType_Graphic, 95, 145, 11, 11, 1, {.graphic = &gWindowGraphic573C28}, &WindowShop_ButtonIncrease},
    {WindowElementType_Graphic, 95, 200, 11, 11, 1, {.graphic = &gWindowGraphic573C40}, &WindowShop_ButtonIncrease},
    {WindowElementType_Graphic, 95, 255, 11, 11, 1, {.graphic = &gWindowGraphic573C58}, &WindowShop_ButtonIncrease},
    {WindowElementType_Graphic, 95, 310, 11, 11, 1, {.graphic = &gWindowGraphic573C70}, &WindowShop_ButtonIncrease},
    {WindowElementType_EventArea, 27, 73, 48, 48, 1, {.unusedValue = 0}, &WindowShop_HandleItemIcon},
    {WindowElementType_EventArea, 27, 128, 48, 48, 1, {.unusedValue = 0}, &WindowShop_HandleItemIcon},
    {WindowElementType_EventArea, 27, 183, 48, 48, 1, {.unusedValue = 0}, &WindowShop_HandleItemIcon},
    {WindowElementType_EventArea, 27, 238, 48, 48, 1, {.unusedValue = 0}, &WindowShop_HandleItemIcon},
    {WindowElementType_EventArea, 27, 293, 48, 48, 1, {.unusedValue = 0}, &WindowShop_HandleItemIcon},
    {WindowElementType_Graphic, 27, 70, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 125, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 180, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 235, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 290, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 290, 125, 252, 202, 1, {.unusedValue = 0}, &WindowShop_HandleInventory},
    {WindowElementType_Graphic, 15, 28, 274, 325, 1, {.graphic = &gWindowGraphic573B20}, &WindowShop_HandleSaleDrop},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic573B08}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 552, 5, 18, 80, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements571160[38] { // 原全局：0x00571160。
    {WindowElementType_Input, 0, 0, 567, 339, 1, {.unusedValue = 0}, &WindowBank_FocusAmountInput},
    {WindowElementType_Graphic, 547, 9, 11, 11, 1, {.graphic = &gWindowGraphic571120}, &WindowBank_ButtonCloseIcon},
    {WindowElementType_Graphic, 476, 308, 66, 17, 1, {.graphic = &gWindowGraphic571030}, &WindowBank_ButtonClose},
    {WindowElementType_Graphic, 26, 307, 24, 19, 1, {.graphic = &gWindowGraphic571030}, &WindowBank_ButtonFirstPage},
    {WindowElementType_Graphic, 55, 307, 24, 19, 1, {.graphic = &gWindowGraphic571030}, &WindowBank_ButtonSecondPage},
    {WindowElementType_Graphic, 26, 307, 24, 19, 1, {.graphic = &gWindowGraphic571030}, &WindowBank_DrawFirstPageSelection},
    {WindowElementType_Graphic, 55, 307, 24, 19, 1, {.graphic = &gWindowGraphic571030}, &WindowBank_DrawSecondPageSelection},
    {WindowElementType_Graphic, 298, 264, 80, 15, 1, {.graphic = &gWindowGraphic571048}, &WindowBank_TabItems},
    {WindowElementType_Graphic, 383, 264, 80, 15, 1, {.graphic = &gWindowGraphic571060}, &WindowBank_TabPets},
    {WindowElementType_Graphic, 469, 264, 80, 15, 1, {.graphic = &gWindowGraphic571078}, &WindowBank_TabDeposit},
    {WindowElementType_Graphic, 26, 264, 80, 15, 1, {.graphic = &gWindowGraphic571048}, &WindowBank_TabItems},
    {WindowElementType_Graphic, 111, 264, 80, 15, 1, {.graphic = &gWindowGraphic571060}, &WindowBank_TabPets},
    {WindowElementType_Graphic, 197, 264, 80, 15, 1, {.graphic = &gWindowGraphic571078}, &WindowBank_TabWithdraw},
    {WindowElementType_Text, 478, 328, 80, 15, 1, {.text = &gWindowText571148}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 297, 53, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &WindowBank_HandleOwnPetRow},
    {WindowElementType_Graphic, 297, 122, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &WindowBank_HandleOwnPetRow},
    {WindowElementType_Graphic, 297, 191, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &WindowBank_HandleOwnPetRow},
    {WindowElementType_Graphic, 25, 53, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &WindowBank_HandleStoredPetRow},
    {WindowElementType_Graphic, 25, 122, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &WindowBank_HandleStoredPetRow},
    {WindowElementType_Graphic, 25, 191, 240, 69, 1, {.graphic = &gWindowGraphic5710C0}, &WindowBank_HandleStoredPetRow},
    {WindowElementType_Graphic, 538, 65, 0, 14, 1, {.graphic = &gWindowGraphic5710D8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 538, 65, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD67880}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 538, 55, 11, 11, 1, {.graphic = &gWindowGraphic5710F0}, &WindowBank_ButtonPreviousPet},
    {WindowElementType_Graphic, 538, 247, 11, 11, 1, {.graphic = &gWindowGraphic571108}, &WindowBank_ButtonNextPet},
    {WindowElementType_EventArea, 286, 27, 273, 280, 1, {.unusedValue = 0}, &WindowBank_ScrollPets},
    {WindowElementType_Graphic, 266, 65, 0, 14, 1, {.graphic = &gWindowGraphic5710D8}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 266, 65, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD6788C}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 266, 55, 11, 11, 1, {.graphic = &gWindowGraphic5710F0}, &WindowBank_ButtonPreviousPet},
    {WindowElementType_Graphic, 266, 247, 11, 11, 1, {.graphic = &gWindowGraphic571108}, &WindowBank_ButtonNextPet},
    {WindowElementType_EventArea, 14, 27, 273, 280, 1, {.unusedValue = 0}, &WindowBank_ScrollPets},
    {WindowElementType_EventArea, 375, 135, 106, 85, 1, {.unusedValue = 0}, &WindowBank_HandleAmount},
    {WindowElementType_EventArea, 103, 135, 106, 85, 1, {.unusedValue = 0}, &WindowBank_HandleAmount},
    {WindowElementType_Number, 465, 115, 0, 0, 1, {.number = &gWindowNumber571150}, &WindowElement_NoOp},
    {WindowElementType_Number, 193, 115, 0, 0, 1, {.number = &gWindowNumber571158}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 286, 27, 273, 280, 1, {.graphic = &gWindowGraphic571090}, &WindowBank_HandleOwnItems},
    {WindowElementType_Graphic, 14, 27, 273, 280, 1, {.graphic = &gWindowGraphic5710A8}, &WindowBank_HandleStoredItems},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic571018}, &WindowBank_DrawCounts},
    {WindowElementType_EventArea, 571, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements571740[6] { // 原全局：0x00571740。
    {WindowElementType_Graphic, 547, 9, 11, 11, 1, {.graphic = &gWindowGraphic571728}, &WindowBank_ButtonCloseIcon},
    {WindowElementType_Graphic, 476, 308, 66, 17, 1, {.graphic = &gWindowGraphic5716E0}, &WindowBank_ButtonClose},
    {WindowElementType_Graphic, 286, 27, 273, 280, 1, {.graphic = &gWindowGraphic5716F8}, &WindowItemStorage_HandleOwnItems},
    {WindowElementType_Graphic, 14, 27, 273, 280, 1, {.graphic = &gWindowGraphic571710}, &WindowItemStorage_HandleStoredItems},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5716C8}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 571, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements5719E8[26] { // 原全局：0x005719E8。
    {WindowElementType_EventArea, 0, 0, 330, 110, 1, {.unusedValue = 0}, &WindowTextList_DrawConfirmation},
    {WindowElementType_Graphic, 211, 8, 11, 11, 1, {.graphic = &gWindowGraphic5718A8}, &WindowRuntime_ButtonClose},
    {WindowElementType_EventArea, 57, 30, 150, 12, 1, {.unusedValue = 0}, &WindowTextList_EditTitle},
    {WindowElementType_Text, 57, 30, 0, 0, 1, {.text = &gWindowText571998}, &WindowElement_NoOp},
    {WindowElementType_Input, 57, 30, 0, 0, 1, {.unusedValue = 0}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 64, 0, 0, 1, {.text = &gWindowTexts5719A0[0]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 81, 0, 0, 1, {.text = &gWindowTexts5719A0[1]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 98, 0, 0, 1, {.text = &gWindowTexts5719A0[2]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 115, 0, 0, 1, {.text = &gWindowTexts5719A0[3]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 132, 0, 0, 1, {.text = &gWindowTexts5719A0[4]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 149, 0, 0, 1, {.text = &gWindowTexts5719A0[5]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 166, 0, 0, 1, {.text = &gWindowTexts5719A0[6]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 183, 0, 0, 1, {.text = &gWindowTexts5719A0[7]}, &WindowElement_NoOp},
    {WindowElementType_Text, 17, 200, 0, 0, 1, {.text = &gWindowTexts5719A0[8]}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 212, 75, 0, 14, 1, {.graphic = &gWindowGraphic5718F0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 212, 75, 11, 136, 1, {.mouseFlags = &gWindowMouseFlagsD67898}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 212, 65, 11, 11, 1, {.graphic = &gWindowGraphic571908}, &WindowTextList_ButtonPreviousRow},
    {WindowElementType_Graphic, 212, 210, 11, 11, 1, {.graphic = &gWindowGraphic571920}, &WindowTextList_ButtonNextRow},
    {WindowElementType_Graphic, 169, 225, 18, 18, 1, {.graphic = &gWindowGraphic571968}, &WindowTextList_ButtonPreviousPage},
    {WindowElementType_Graphic, 189, 225, 18, 18, 1, {.graphic = &gWindowGraphic571980}, &WindowTextList_ButtonNextPage},
    {WindowElementType_EventArea, 0, 0, 231, 305, 1, {.unusedValue = 0}, &WindowTextList_Scroll},
    {WindowElementType_Graphic, 165, 43, 57, 17, 1, {.graphic = &gWindowGraphic571938}, &WindowTextList_SubmitTitle},
    {WindowElementType_Graphic, 74, 237, 57, 17, 1, {.graphic = &gWindowGraphic571950}, &WindowTextList_SubmitSelection},
    {WindowElementType_EventArea, 15, 225, 48, 48, 1, {.unusedValue = 0}, &WindowTextList_HandleItemSelection},
    {WindowElementType_Graphic, 0, 0, 267, 305, 1, {.graphic = &gWindowGraphic5718C0}, &WindowTextList_DrawBackgroundTooltip},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic5718D8}, &WindowElement_NoOp},
};

WindowElementPrototype gWindowElements571F18[76] { // 原全局：0x00571F18。
    {WindowElementType_Input, 116, 31, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 116, 49, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 163, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 182, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 201, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 220, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 239, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 258, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Input, 38, 277, 144, 14, 1, {.unusedValue = 0}, &WindowGuildDetails_HandleInput},
    {WindowElementType_Text, 116, 31, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 116, 49, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 165, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 184, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 203, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 222, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 241, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 260, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 38, 279, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 163, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 182, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 201, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 220, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 239, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 258, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 201, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 220, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 239, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 258, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 277, 277, 0, 0, 1, {.graphic = &gWindowGraphic571DF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 18, 161, 0, 0, 1, {.graphic = &gWindowGraphic571EC8}, &WindowElement_NoOp},
    {WindowElementType_Text, 116, 70, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 116, 90, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Text, 116, 110, 0, 0, 1, {.text = &gWindowText571F10}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 161, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 180, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 199, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 218, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 237, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 256, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 16, 275, 0, 0, 1, {.graphic = &gWindowGraphic571EB0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 273, 28, 32, 17, 0, {.graphic = &gWindowGraphic571E20}, &WindowGuildDetails_HandleActionButton},
    {WindowElementType_Graphic, 273, 46, 32, 17, 0, {.graphic = &gWindowGraphic571E20}, &WindowGuildDetails_HandleActionButton},
    {WindowElementType_Graphic, 299, 163, 11, 11, 1, {.graphic = &gWindowGraphic571E38}, &WindowGuildDetails_HandleScrollButtons},
    {WindowElementType_Graphic, 299, 280, 11, 11, 1, {.graphic = &gWindowGraphic571E68}, &WindowGuildDetails_HandleScrollButtons},
    {WindowElementType_Graphic, 299, 173, 0, 0, 1, {.graphic = &gWindowGraphic571E50}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 298, 173, 10, 94, 1, {.mouseFlags = &gWindowMouseFlagsD678A8}, &WindowGuildDetails_HandleScrollFocus},
    {WindowElementType_EventArea, 0, 0, 317, 325, 1, {.unusedValue = 0}, &WindowGuildDetails_Scroll},
    {WindowElementType_Graphic, 299, 9, 11, 11, 1, {.graphic = &gWindowGraphic571E08}, &WindowGuildDetails_ButtonClose},
    {WindowElementType_Graphic, 265, 296, 57, 17, 0, {.graphic = &gWindowGraphic571E20}, &WindowGuildDetails_HandleActionButton},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic571EE0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic571EF8}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 317, 0, 20, 130, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

WindowElementPrototype gWindowElements572AF8[20] { // 原全局：0x00572AF8。
    {WindowElementType_Input, 0, 0, 567, 339, 1, {.unusedValue = 0}, &WindowTrade_HandleAmountInput},
    {WindowElementType_Graphic, 547, 9, 11, 11, 1, {.graphic = &gWindowGraphic572958}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 476, 309, 49, 17, 1, {.graphic = &gWindowGraphic572970}, &WindowTrade_ButtonPublish},
    {WindowElementType_Graphic, 446, 309, 49, 17, 1, {.graphic = &gWindowGraphic572988}, &WindowTrade_ButtonConfirm},
    {WindowElementType_Graphic, 502, 309, 49, 17, 1, {.graphic = &gWindowGraphic5729A0}, &WindowTrade_ButtonWithdraw},
    {WindowElementType_EventArea, 376, 136, 106, 85, 1, {.unusedValue = 0}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 541, 66, 0, 14, 1, {.unusedValue = 0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 541, 66, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD678C4}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 15, 28, 273, 280, 1, {.graphic = &gWindowGraphic572AD8}, &WindowTrade_HandleSelectedPanel},
    {WindowElementType_EventArea, 541, 66, 0, 14, 1, {.unusedValue = 0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 541, 66, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD678B8}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 297, 28, 273, 280, 1, {.graphic = &gWindowGraphic572AD8}, &WindowTrade_HandleOwnPanel},
    {WindowElementType_EventArea, 541, 66, 0, 14, 1, {.unusedValue = 0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 541, 66, 11, 183, 1, {.mouseFlags = &gWindowMouseFlagsD678D0}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 15, 28, 273, 280, 1, {.graphic = &gWindowGraphic572AD8}, &WindowTrade_HandleRemoteSelection},
    {WindowElementType_Text, 133, 310, 0, 0, 1, {.text = &gWindowText572AF0}, &WindowElement_NoOp},
    {WindowElementType_Text, 281, 310, 0, 0, 1, {.text = &gWindowText572AF0}, &WindowElement_NoOp},
    {WindowElementType_Text, 317, 310, 0, 0, 1, {.text = &gWindowText572AF0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic572940}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 571, 7, 18, 78, 1, {.unusedValue = 0}, &WindowElement_NoOp},
};

// 原5748E8的107项；选择回调由模式刷新发布，不替换初始4B0F10。
WindowElementPrototype gWindowElements5748E8[107] {
    {WindowElementType_EventArea, 0, 0, 640, 383, 1, {.unusedValue = 0}, &WindowPetSkillChoice_HandleConfirmation},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic574668}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 203, 326, 66, 17, 1, {.graphic = &gWindowGraphic5746F8}, &WindowPetSkillChoice_ButtonLearn},
    {WindowElementType_Graphic, 467, 326, 66, 17, 1, {.graphic = &gWindowGraphic574710}, &WindowPetSkillChoice_ButtonBack},
    {WindowElementType_Text, 412, 44, 0, 14, 1, {.text = &gWindowText5747C0}, &WindowElement_NoOp},
    {WindowElementType_Text, 78, 52, 0, 14, 1, {.text = &gWindowText5747D8}, &WindowElement_NoOp},
    {WindowElementType_Number, 262, 37, 0, 14, 1, {.number = &gWindowNumber5747F0}, &WindowElement_NoOp},
    {WindowElementType_Number, 389, 30, 0, 14, 1, {.number = &gWindowNumber5747E8}, &WindowElement_NoOp},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5747E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 502, 146, 0, 14, 1, {.graphic = &gWindowGraphic5746B0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 502, 146, 11, 137, 1, {.mouseFlags = &gWindowMouseFlagsD67900}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 502, 136, 11, 11, 1, {.graphic = &gWindowGraphic5746C8}, &WindowPetSkillChoice_ButtonPreviousRow},
    {WindowElementType_Graphic, 502, 282, 11, 11, 1, {.graphic = &gWindowGraphic5746E0}, &WindowPetSkillChoice_ButtonNextRow},
    {WindowElementType_Graphic, 472, 300, 18, 18, 1, {.graphic = &gWindowGraphic574788}, &WindowPetSkillChoice_ButtonPreviousPage},
    {WindowElementType_Graphic, 498, 300, 18, 18, 1, {.graphic = &gWindowGraphic5747A0}, &WindowPetSkillChoice_ButtonNextPage},
    {WindowElementType_EventArea, 286, 27, 273, 280, 1, {.unusedValue = 0}, &WindowPetSkillChoice_Scroll},
    {WindowElementType_Graphic, 499, 132, 11, 11, 1, {.graphic = &gWindowGraphic574728}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 269, 85, 0, 14, 1, {.graphic = &gWindowGraphic5746B0}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 269, 85, 11, 244, 1, {.mouseFlags = &gWindowMouseFlagsD67900}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 269, 75, 11, 11, 1, {.graphic = &gWindowGraphic5746C8}, &WindowPetSkillChoice_ButtonPreviousRow},
    {WindowElementType_Graphic, 269, 327, 11, 11, 1, {.graphic = &gWindowGraphic5746E0}, &WindowPetSkillChoice_ButtonNextRow},
    {WindowElementType_Graphic, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic574788}, &WindowPetSkillChoice_ButtonPreviousPage},
    {WindowElementType_Graphic, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic5747A0}, &WindowPetSkillChoice_ButtonNextPage},
    {WindowElementType_EventArea, 17, 28, 274, 325, 1, {.unusedValue = 0}, &WindowPetSkillChoice_Scroll},
    {WindowElementType_Text, 33, 87, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 127, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 147, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 167, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 187, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 207, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 227, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 247, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 267, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 33, 287, 0, 0, 1, {.text = &gWindowText5747D0}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 135, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 151, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 167, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 183, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 199, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 215, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 231, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 247, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 263, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 296, 279, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 446, 135, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 446, 151, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 446, 167, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 446, 183, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 446, 199, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 136, 0, 0, 1, {.number = &gWindowNumber574848}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 152, 0, 0, 1, {.number = &gWindowNumber574850}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 168, 0, 0, 1, {.number = &gWindowNumber574858}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 184, 0, 0, 1, {.number = &gWindowNumber574860}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 200, 0, 0, 1, {.number = &gWindowNumber574868}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 216, 0, 0, 1, {.number = &gWindowNumber574870}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 232, 0, 0, 1, {.number = &gWindowNumber574878}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 248, 0, 0, 1, {.number = &gWindowNumber574880}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 264, 0, 0, 1, {.number = &gWindowNumber574888}, &WindowElement_NoOp},
    {WindowElementType_Number, 454, 280, 0, 0, 1, {.number = &gWindowNumber574890}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 136, 0, 0, 1, {.number = &gWindowNumber574898}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 152, 0, 0, 1, {.number = &gWindowNumber5748A0}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 168, 0, 0, 1, {.number = &gWindowNumber5748A8}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 184, 0, 0, 1, {.number = &gWindowNumber5748B0}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 200, 0, 0, 1, {.number = &gWindowNumber5748B8}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 216, 0, 0, 1, {.number = &gWindowNumber5748C0}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 232, 0, 0, 1, {.number = &gWindowNumber5748C8}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 248, 0, 0, 1, {.number = &gWindowNumber5748D0}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 264, 0, 0, 1, {.number = &gWindowNumber5748D8}, &WindowElement_NoOp},
    {WindowElementType_Number, 492, 280, 0, 0, 1, {.number = &gWindowNumber5748E0}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 133, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 149, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 165, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 181, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 197, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 213, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 229, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 245, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 261, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 290, 277, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &WindowElement_NoOp},
    {WindowElementType_Text, 41, 82, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 41, 137, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 41, 192, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 41, 247, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Text, 41, 302, 0, 0, 1, {.text = &gWindowText5747C8}, &WindowElement_NoOp},
    {WindowElementType_Number, 239, 83, 0, 14, 1, {.number = &gWindowNumber5747F8}, &WindowElement_NoOp},
    {WindowElementType_Number, 239, 138, 0, 14, 1, {.number = &gWindowNumber574800}, &WindowElement_NoOp},
    {WindowElementType_Number, 239, 193, 0, 14, 1, {.number = &gWindowNumber574808}, &WindowElement_NoOp},
    {WindowElementType_Number, 239, 248, 0, 14, 1, {.number = &gWindowNumber574810}, &WindowElement_NoOp},
    {WindowElementType_Number, 239, 303, 0, 14, 1, {.number = &gWindowNumber574818}, &WindowElement_NoOp},
    {WindowElementType_Number, 233, 108, 0, 14, 1, {.number = &gWindowNumber574820}, &WindowElement_NoOp},
    {WindowElementType_Number, 233, 163, 0, 14, 1, {.number = &gWindowNumber574828}, &WindowElement_NoOp},
    {WindowElementType_Number, 233, 218, 0, 14, 1, {.number = &gWindowNumber574830}, &WindowElement_NoOp},
    {WindowElementType_Number, 233, 273, 0, 14, 1, {.number = &gWindowNumber574838}, &WindowElement_NoOp},
    {WindowElementType_Number, 233, 328, 0, 14, 1, {.number = &gWindowNumber574840}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 80, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 135, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 190, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 245, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 37, 300, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 70, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 125, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 180, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 235, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 290, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 17, 28, 274, 325, 1, {.graphic = &gWindowGraphic574698}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic574680}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 552, 5, 18, 80, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原575988的完整70项，显示缓冲与69更新/绘制保持同源。
WindowElementPrototype gWindowElements575988[70] {
    {WindowElementType_EventArea, 0, 0, 640, 383, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleConfirmation},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic5756F0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 294, 333, 66, 17, 1, {.graphic = &gWindowGraphic575888}, &WindowCrystalShop_ButtonOk},
    {WindowElementType_Graphic, 467, 333, 66, 17, 1, {.graphic = &gWindowGraphic5758A0}, &WindowCrystalTrade_ButtonCancel},
    {WindowElementType_Graphic, 371, 333, 25, 19, 1, {.graphic = &gWindowGraphic5758A0}, &WindowCrystalTrade_DrawButton},
    {WindowElementType_Text, 425, 44, 0, 14, 1, {.text = &gWindowText5758F8}, &WindowElement_NoOp},
    {WindowElementType_Number, 260, 37, 0, 0, 1, {.number = &gWindowNumber575930}, &WindowElement_NoOp},
    {WindowElementType_Number, 400, 32, 0, 0, 1, {.number = &gWindowNumber575920}, &WindowElement_NoOp},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage575918}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 267, 85, 0, 14, 1, {.graphic = &gWindowGraphic575750}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 267, 85, 11, 243, 1, {.mouseFlags = &gWindowMouseFlagsD67910}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 267, 75, 11, 11, 1, {.graphic = &gWindowGraphic575768}, &WindowCrystalShop_ButtonPreviousRow},
    {WindowElementType_Graphic, 267, 327, 11, 11, 1, {.graphic = &gWindowGraphic575780}, &WindowCrystalShop_ButtonNextRow},
    {WindowElementType_Graphic, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic5758B8}, &WindowCrystalShop_ButtonPreviousPage},
    {WindowElementType_Graphic, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic5758D0}, &WindowCrystalShop_ButtonNextPage},
    {WindowElementType_EventArea, 15, 28, 274, 325, 1, {.unusedValue = 0}, &WindowCrystalShop_Scroll},
    {WindowElementType_Text, 154, 90, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 145, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 200, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 255, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 310, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 102, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 157, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 212, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 267, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 322, 0, 14, 1, {.text = &gWindowText575900}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 120, 0, 14, 1, {.text = &gWindowText575908}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 175, 0, 14, 1, {.text = &gWindowText575908}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 230, 0, 14, 1, {.text = &gWindowText575908}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 285, 0, 14, 1, {.text = &gWindowText575908}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 340, 0, 14, 1, {.text = &gWindowText575908}, &WindowElement_NoOp},
    {WindowElementType_Text, 83, 87, 0, 14, 1, {.text = &gWindowText575910}, &WindowElement_NoOp},
    {WindowElementType_Text, 83, 142, 0, 14, 1, {.text = &gWindowText575910}, &WindowElement_NoOp},
    {WindowElementType_Text, 83, 197, 0, 14, 1, {.text = &gWindowText575910}, &WindowElement_NoOp},
    {WindowElementType_Text, 83, 252, 0, 14, 1, {.text = &gWindowText575910}, &WindowElement_NoOp},
    {WindowElementType_Text, 83, 307, 0, 14, 1, {.text = &gWindowText575910}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 108, 0, 0, 1, {.number = &gWindowNumber575938}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 163, 0, 0, 1, {.number = &gWindowNumber575940}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 218, 0, 0, 1, {.number = &gWindowNumber575948}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 273, 0, 0, 1, {.number = &gWindowNumber575950}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 328, 0, 0, 1, {.number = &gWindowNumber575958}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 90, 0, 0, 1, {.number = &gWindowNumber575960}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 145, 0, 0, 1, {.number = &gWindowNumber575968}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 200, 0, 0, 1, {.number = &gWindowNumber575970}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 255, 0, 0, 1, {.number = &gWindowNumber575978}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 310, 0, 0, 1, {.number = &gWindowNumber575980}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 80, 90, 11, 11, 1, {.graphic = &gWindowGraphic575798}, &WindowCrystalShop_ButtonDecrease},
    {WindowElementType_Graphic, 80, 145, 11, 11, 1, {.graphic = &gWindowGraphic5757B0}, &WindowCrystalShop_ButtonDecrease},
    {WindowElementType_Graphic, 80, 200, 11, 11, 1, {.graphic = &gWindowGraphic5757C8}, &WindowCrystalShop_ButtonDecrease},
    {WindowElementType_Graphic, 80, 255, 11, 11, 1, {.graphic = &gWindowGraphic5757E0}, &WindowCrystalShop_ButtonDecrease},
    {WindowElementType_Graphic, 80, 310, 11, 11, 1, {.graphic = &gWindowGraphic5757F8}, &WindowCrystalShop_ButtonDecrease},
    {WindowElementType_Graphic, 95, 90, 11, 11, 1, {.graphic = &gWindowGraphic575810}, &WindowCrystalShop_ButtonIncrease},
    {WindowElementType_Graphic, 95, 145, 11, 11, 1, {.graphic = &gWindowGraphic575828}, &WindowCrystalShop_ButtonIncrease},
    {WindowElementType_Graphic, 95, 200, 11, 11, 1, {.graphic = &gWindowGraphic575840}, &WindowCrystalShop_ButtonIncrease},
    {WindowElementType_Graphic, 95, 255, 11, 11, 1, {.graphic = &gWindowGraphic575858}, &WindowCrystalShop_ButtonIncrease},
    {WindowElementType_Graphic, 95, 310, 11, 11, 1, {.graphic = &gWindowGraphic575870}, &WindowCrystalShop_ButtonIncrease},
    {WindowElementType_EventArea, 27, 73, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleItemIcon},
    {WindowElementType_EventArea, 27, 128, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleItemIcon},
    {WindowElementType_EventArea, 27, 183, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleItemIcon},
    {WindowElementType_EventArea, 27, 238, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleItemIcon},
    {WindowElementType_EventArea, 27, 293, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleItemIcon},
    {WindowElementType_Graphic, 27, 70, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 125, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 180, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 235, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 290, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 290, 125, 252, 202, 1, {.unusedValue = 0}, &WindowCrystalShop_HandleInventory},
    {WindowElementType_Graphic, 15, 28, 274, 325, 1, {.graphic = &gWindowGraphic575720}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic575708}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 552, 5, 18, 80, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原576588的完整70项，保留70背包回调的单格命中语义。
WindowElementPrototype gWindowElements576588[70] {
    {WindowElementType_EventArea, 0, 0, 640, 383, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleConfirmation},
    {WindowElementType_Graphic, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic5762F0}, &WindowRuntime_ButtonClose},
    {WindowElementType_Graphic, 294, 333, 66, 17, 1, {.graphic = &gWindowGraphic576488}, &WindowCrystalExchangeShop_ButtonOk},
    {WindowElementType_Graphic, 467, 333, 66, 17, 1, {.graphic = &gWindowGraphic5764A0}, &WindowCrystalTrade_ButtonCancel},
    {WindowElementType_Graphic, 371, 333, 25, 19, 1, {.graphic = &gWindowGraphic5764A0}, &WindowCrystalTrade_DrawButton},
    {WindowElementType_Text, 425, 44, 0, 14, 1, {.text = &gWindowText5764F8}, &WindowElement_NoOp},
    {WindowElementType_Number, 260, 37, 0, 0, 1, {.number = &gWindowNumber576530}, &WindowElement_NoOp},
    {WindowElementType_Number, 400, 32, 0, 0, 1, {.number = &gWindowNumber576520}, &WindowElement_NoOp},
    {WindowElementType_Animation, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage576518}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 267, 85, 0, 14, 1, {.graphic = &gWindowGraphic576350}, &WindowElement_NoOp},
    {WindowElementType_MouseArea, 267, 85, 11, 243, 1, {.mouseFlags = &gWindowMouseFlagsD67920}, &WindowElement_HandleVerticalScroll},
    {WindowElementType_Graphic, 267, 75, 11, 11, 1, {.graphic = &gWindowGraphic576368}, &WindowCrystalExchangeShop_ButtonPreviousRow},
    {WindowElementType_Graphic, 267, 327, 11, 11, 1, {.graphic = &gWindowGraphic576380}, &WindowCrystalExchangeShop_ButtonNextRow},
    {WindowElementType_Graphic, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic5764B8}, &WindowCrystalExchangeShop_ButtonPreviousPage},
    {WindowElementType_Graphic, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic5764D0}, &WindowCrystalExchangeShop_ButtonNextPage},
    {WindowElementType_EventArea, 15, 28, 274, 325, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_Scroll},
    {WindowElementType_Text, 154, 90, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 145, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 200, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 255, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 310, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 102, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 157, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 212, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 267, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 322, 0, 14, 1, {.text = &gWindowText576500}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 120, 0, 14, 1, {.text = &gWindowText576508}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 175, 0, 14, 1, {.text = &gWindowText576508}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 230, 0, 14, 1, {.text = &gWindowText576508}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 285, 0, 14, 1, {.text = &gWindowText576508}, &WindowElement_NoOp},
    {WindowElementType_Text, 154, 340, 0, 14, 1, {.text = &gWindowText576508}, &WindowElement_NoOp},
    {WindowElementType_Text, 83, 87, 0, 14, 1, {.text = &gWindowText576510}, &WindowElement_NoOp},
    {WindowElementType_Text, 83, 142, 0, 14, 1, {.text = &gWindowText576510}, &WindowElement_NoOp},
    {WindowElementType_Text, 83, 197, 0, 14, 1, {.text = &gWindowText576510}, &WindowElement_NoOp},
    {WindowElementType_Text, 83, 252, 0, 14, 1, {.text = &gWindowText576510}, &WindowElement_NoOp},
    {WindowElementType_Text, 83, 307, 0, 14, 1, {.text = &gWindowText576510}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 108, 0, 0, 1, {.number = &gWindowNumber576538}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 163, 0, 0, 1, {.number = &gWindowNumber576540}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 218, 0, 0, 1, {.number = &gWindowNumber576548}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 273, 0, 0, 1, {.number = &gWindowNumber576550}, &WindowElement_NoOp},
    {WindowElementType_Number, 129, 328, 0, 0, 1, {.number = &gWindowNumber576558}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 90, 0, 0, 1, {.number = &gWindowNumber576560}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 145, 0, 0, 1, {.number = &gWindowNumber576568}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 200, 0, 0, 1, {.number = &gWindowNumber576570}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 255, 0, 0, 1, {.number = &gWindowNumber576578}, &WindowElement_NoOp},
    {WindowElementType_Number, 132, 310, 0, 0, 1, {.number = &gWindowNumber576580}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 80, 90, 11, 11, 1, {.graphic = &gWindowGraphic576398}, &WindowCrystalExchangeShop_ButtonDecrease},
    {WindowElementType_Graphic, 80, 145, 11, 11, 1, {.graphic = &gWindowGraphic5763B0}, &WindowCrystalExchangeShop_ButtonDecrease},
    {WindowElementType_Graphic, 80, 200, 11, 11, 1, {.graphic = &gWindowGraphic5763C8}, &WindowCrystalExchangeShop_ButtonDecrease},
    {WindowElementType_Graphic, 80, 255, 11, 11, 1, {.graphic = &gWindowGraphic5763E0}, &WindowCrystalExchangeShop_ButtonDecrease},
    {WindowElementType_Graphic, 80, 310, 11, 11, 1, {.graphic = &gWindowGraphic5763F8}, &WindowCrystalExchangeShop_ButtonDecrease},
    {WindowElementType_Graphic, 95, 90, 11, 11, 1, {.graphic = &gWindowGraphic576410}, &WindowCrystalExchangeShop_ButtonIncrease},
    {WindowElementType_Graphic, 95, 145, 11, 11, 1, {.graphic = &gWindowGraphic576428}, &WindowCrystalExchangeShop_ButtonIncrease},
    {WindowElementType_Graphic, 95, 200, 11, 11, 1, {.graphic = &gWindowGraphic576440}, &WindowCrystalExchangeShop_ButtonIncrease},
    {WindowElementType_Graphic, 95, 255, 11, 11, 1, {.graphic = &gWindowGraphic576458}, &WindowCrystalExchangeShop_ButtonIncrease},
    {WindowElementType_Graphic, 95, 310, 11, 11, 1, {.graphic = &gWindowGraphic576470}, &WindowCrystalExchangeShop_ButtonIncrease},
    {WindowElementType_EventArea, 27, 73, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleItemIcon},
    {WindowElementType_EventArea, 27, 128, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleItemIcon},
    {WindowElementType_EventArea, 27, 183, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleItemIcon},
    {WindowElementType_EventArea, 27, 238, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleItemIcon},
    {WindowElementType_EventArea, 27, 293, 48, 48, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleItemIcon},
    {WindowElementType_Graphic, 27, 70, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 125, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 180, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 235, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 27, 290, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 290, 125, 252, 202, 1, {.unusedValue = 0}, &WindowCrystalExchangeShop_HandleInventory},
    {WindowElementType_Graphic, 15, 28, 274, 325, 1, {.graphic = &gWindowGraphic576320}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic576308}, &WindowElement_NoOp},
    {WindowElementType_EventArea, 552, 5, 18, 80, 1, {.unusedValue = 0}, &WindowElement_HandleSideArea},
};

// 原576F90的十五项，三个分量按钮复用源图形但实例各自分配。
WindowElementPrototype gWindowElements576F90[15] {
    {WindowElementType_Graphic, 197, 206, 66, 17, 1, {.graphic = &gWindowGraphic576EF0}, &WindowAppearance_ButtonOk},
    {WindowElementType_Graphic, 48, 19, 0, 0, 1, {.graphic = &gWindowGraphic576F20}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 48, 19, 0, 0, 1, {.graphic = &gWindowGraphic576F08}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 144, 26, 18, 18, 1, {.graphic = &gWindowGraphic576F38}, &WindowAppearance_CycleClass},
    {WindowElementType_Text, 194, 28, 0, 0, 1, {.text = &gWindowText576F70}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 260, 26, 18, 18, 1, {.graphic = &gWindowGraphic576F50}, &WindowAppearance_CycleClass},
    {WindowElementType_Graphic, 144, 47, 18, 18, 1, {.graphic = &gWindowGraphic576F38}, &WindowAppearance_CycleFifthDigit},
    {WindowElementType_Text, 194, 49, 0, 0, 1, {.text = &gWindowText576F78}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 260, 47, 18, 18, 1, {.graphic = &gWindowGraphic576F50}, &WindowAppearance_CycleFifthDigit},
    {WindowElementType_Graphic, 144, 68, 18, 18, 1, {.graphic = &gWindowGraphic576F38}, &WindowAppearance_CycleOnesDigit},
    {WindowElementType_Text, 194, 70, 0, 0, 1, {.text = &gWindowText576F80}, &WindowElement_NoOp},
    {WindowElementType_Graphic, 260, 68, 18, 18, 1, {.graphic = &gWindowGraphic576F50}, &WindowAppearance_CycleOnesDigit},
    {WindowElementType_Animation, 390, 120, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage576F88}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 147, 0, 0, 1, {.text = &gWindowText576F68}, &WindowElement_NoOp},
    {WindowElementType_Text, 20, 163, 0, 0, 1, {.text = &gWindowText576F68}, &WindowElement_NoOp},
};

// 71个原生指针；原36/37同源，不因窗口编号不同复制对象。
std::array<WindowPrototype*, kWindowCount> gWindowPrototypes57C130 { // 原全局：0x0057C130。
    &gWindowPrototypeRecords[WindowPrototypeIndex_None], // 0
    &gWindowPrototypeRecords[WindowPrototypeIndex_StatusBar],
    &gWindowPrototypeRecords[WindowPrototypeIndex_CompactStatusBar],
    &gWindowPrototypeRecords[WindowPrototypeIndex_CollapsedStatusBar],
    &gWindowPrototypeRecords[WindowPrototypeIndex_ShortcutBar],
    &gWindowPrototypeRecords[WindowPrototypeIndex_CollapsedShortcutBar],
    &gWindowPrototypeRecords[WindowPrototypeIndex_BottomBar],
    &gWindowPrototypeRecords[WindowPrototypeIndex_PlayerProfile],
    &gWindowPrototypeRecords[WindowPrototypeIndex_PlayerAttributes],
    &gWindowPrototypeRecords[WindowPrototypeIndex_PlayerTitles],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Skill], // 10
    &gWindowPrototypeRecords[WindowPrototypeIndex_Craft],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Gather],
    &gWindowPrototypeRecords[WindowPrototypeIndex_SkillResult],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Inventory],
    &gWindowPrototypeRecords[WindowPrototypeIndex_PetList],
    &gWindowPrototypeRecords[WindowPrototypeIndex_PetDetail],
    &gWindowPrototypeRecords[WindowPrototypeIndex_PetAttributes],
    &gWindowPrototypeRecords[WindowPrototypeIndex_PetSkills],
    &gWindowPrototypeRecords[WindowPrototypeIndex_AlbumList],
    &gWindowPrototypeRecords[WindowPrototypeIndex_AlbumDetail], // 20
    &gWindowPrototypeRecords[WindowPrototypeIndex_Contacts],
    &gWindowPrototypeRecords[WindowPrototypeIndex_MailCompose],
    &gWindowPrototypeRecords[WindowPrototypeIndex_PetMailCompose],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Settings],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Hotkeys],
    &gWindowPrototypeRecords[WindowPrototypeIndex_BattleCommand],
    &gWindowPrototypeRecords[WindowPrototypeIndex_BattleItems],
    &gWindowPrototypeRecords[WindowPrototypeIndex_BattlePets],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Spectator],
    &gWindowPrototypeRecords[WindowPrototypeIndex_BattleResult], // 30
    &gWindowPrototypeRecords[WindowPrototypeIndex_DpResult],
    &gWindowPrototypeRecords[WindowPrototypeIndex_BattleStateNotice],
    &gWindowPrototypeRecords[WindowPrototypeIndex_MiniMap],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Emote],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Chat],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Dialog],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Dialog], // 37
    &gWindowPrototypeRecords[WindowPrototypeIndex_ItemStorage],
    &gWindowPrototypeRecords[WindowPrototypeIndex_TimerNotice],
    &gWindowPrototypeRecords[WindowPrototypeIndex_MailRead], // 40
    &gWindowPrototypeRecords[WindowPrototypeIndex_Bank],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Quantity],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Trade],
    &gWindowPrototypeRecords[WindowPrototypeIndex_TradeTarget],
    &gWindowPrototypeRecords[WindowPrototypeIndex_TextTarget],
    &gWindowPrototypeRecords[WindowPrototypeIndex_AttributeTarget],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Confirmation],
    &gWindowPrototypeRecords[WindowPrototypeIndex_GuildDetails],
    &gWindowPrototypeRecords[WindowPrototypeIndex_GroupSelect],
    &gWindowPrototypeRecords[WindowPrototypeIndex_MessageBoard], // 50
    &gWindowPrototypeRecords[WindowPrototypeIndex_TextList],
    &gWindowPrototypeRecords[WindowPrototypeIndex_ShopChoice],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Shop],
    &gWindowPrototypeRecords[WindowPrototypeIndex_PetSkillChoice],
    &gWindowPrototypeRecords[WindowPrototypeIndex_GuildRank],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Appearance],
    &gWindowPrototypeRecords[WindowPrototypeIndex_CompactChat],
    &gWindowPrototypeRecords[WindowPrototypeIndex_OwnProfile],
    &gWindowPrototypeRecords[WindowPrototypeIndex_OwnProfileCategory],
    &gWindowPrototypeRecords[WindowPrototypeIndex_ProfileCategory], // 60
    &gWindowPrototypeRecords[WindowPrototypeIndex_ProfileSearch],
    &gWindowPrototypeRecords[WindowPrototypeIndex_ProfileView],
    &gWindowPrototypeRecords[WindowPrototypeIndex_MiniMail],
    &gWindowPrototypeRecords[WindowPrototypeIndex_ProfileReply],
    &gWindowPrototypeRecords[WindowPrototypeIndex_Blacklist],
    &gWindowPrototypeRecords[WindowPrototypeIndex_AdventureLog],
    &gWindowPrototypeRecords[WindowPrototypeIndex_OwnStall],
    &gWindowPrototypeRecords[WindowPrototypeIndex_StallBrowse],
    &gWindowPrototypeRecords[WindowPrototypeIndex_CrystalShop],
    &gWindowPrototypeRecords[WindowPrototypeIndex_CrystalExchangeShop], // 70
};

} // namespace cg::ui::detail
