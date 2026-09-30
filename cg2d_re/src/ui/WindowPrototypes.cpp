#include "WindowPrototypeDependencies.hpp"

#include "cg/Input.hpp"

namespace cg::ui::detail {
namespace {

WindowGraphicData gWindowGraphic55F4C0 {0x3B95F, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F4D8 {0x3B962, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F4F0 {0x3B965, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F508 {0x3B968, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F520 {0x3B96B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F538 {0x3B96E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F550 {0x3B971, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F568 {0x18665, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F580 {0x3B95C, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F598 {0x3B95D, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowTextPrototype gWindowText55F5B0 {
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}},
    "\xAC\xDB\xC3\xF6\xB8\xEA\xB0\x54",
};
WindowTextPrototype gWindowText55F5B8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, "--------",
};

// 原56CCC8的四字节元数据和593850借用，不创建另一份默认文本。
WindowTextPrototype gWindowText56CCC8 {{}, input::LegacyText593850()};

WindowGraphicData gWindowGraphic561A40 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561A58 {0x3BA24, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561A70 {0x3BA27, 0, 0, 0, 0, 0x80FFFFFFu};
WindowGraphicData gWindowGraphic561A88 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561AA0 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561AB8 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561AD0 {0x3B571, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561AE8 {0x3B57A, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561B00 {0x3B57A, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561B18 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561B30 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindow10Job5860A4[] = "\xB5\x4C\xC2\xBE";
constexpr char kWindow10Slots58609C[] = "10/10";
constexpr char kWindow10Experience5861E0[] = "\xB8\x67\xC5\xE7\xAD\xC8";
constexpr char kWindow10Level586228[] = "\xB5\xA5\xAF\xC5";
constexpr char kWindow10Skill586094[] = "\xA7\xDE\xAF\xE0";
constexpr char kWindow10Fp5861C0[] = "\xC5\x5D\xA4\x4F";
WindowTextPrototype gWindowText561B48 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow10Job5860A4,
};
WindowTextPrototype gWindowText561B50 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow10Slots58609C,
};
WindowTextPrototype gWindowText561B58 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow10Experience5861E0,
};
WindowTextPrototype gWindowText561B60 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow10Level586228,
};
WindowTextPrototype gWindowText561B68 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow10Skill586094,
};
WindowTextPrototype gWindowText561B70 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow10Fp5861C0,
};
std::uint32_t gWindowMouseFlagsD67780 = 0;

WindowGraphicData gWindowGraphic56B5E0 {0x3B9C0, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56B5F8 {0x3B9C5, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56B610 {0x3B9D1, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56B628 {0x3B9C8, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56B640 {0x3B9D4, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56B658 {0x3B9CB, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56B670 {0x3B9D7, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56B688 {0x3B9DA, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56B6A0 {0x3B9CE, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56C798 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56C7B0 {0x3BA74, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56C7C8 {0x3BA75, 0, 0, 0, 0, 0x80FFFFFFu};
WindowGraphicData gWindowGraphic56C7E0 {0x3BA76, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindow32Text585F70[] = "\xB0\xBD\xC5\xA7";
constexpr char kWindow32Text585F68[] = "\xB3\x51\xB0\xBD\xC5\xA7";
WindowTextPrototype gWindowText56C9A8 {{}, kWindow32Text585F70};
WindowTextPrototype gWindowText56C9B0 {{}, kWindow32Text585F68};
constexpr char kWindow26Name586230[] = "\xA6\x57\xBA\xD9";
WindowTextPrototype gWindowText56B6F8 {
    {std::byte{0x7F}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindow26Name586230,
};
WindowTextPrototype gWindowText56B700 {
    {std::byte{0x7F}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindow10Fp5861C0,
};
WindowTextPrototype gWindowText56B708 {
    {std::byte{0x7F}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindow10Skill586094,
};

WindowGraphicData gWindowGraphic55F780 {0, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F798 {0x3B614, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F7B0 {0x3BAC5, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F7C8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F7E0 {0x3BAB6, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F7F8 {0x3B583, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F810 {0x3BAB8, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F828 {0x3BAC0, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F840 {0x3BABD, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F858 {0x3BABB, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F870 {0x3BAC7, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55F960 {0x3BACC, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55FA50 {0x3BAD1, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55FB40 {0x3BAD6, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55FC30 {0x3BAC6, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55FC48 {0x3BACB, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55FC60 {0x3BAD0, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55FC78 {0x3BAD5, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55FC90 {0x3BAB2, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55FCA8 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic55FCC0 {0x3BAB1, 0, 0, 0, 0, 0x80808080u};
WindowGraphicData gWindowGraphic55FCD8 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindow7Penalty586224[] = "x5";
constexpr char kWindow7Job58621C[] = "\xC2\xBE\xB7\x7E";
constexpr char kWindow7Guild586214[] = "\xAE\x61\xB1\xDA";
constexpr char kWindow7GuildTitle586208[] = "\xAE\x61\xB1\xDA\xBA\xD9\xB8\xB9";
constexpr char kWindow7Title5861FC[] = "\xAF\x53\xAE\xED\xBA\xD9\xB8\xB9";
constexpr char kWindow7OwnTitle5861F0[] = "\xA6\xDB\xA9\x77\xBA\xD9\xB8\xB9";
constexpr char kWindow7Tribe5861E8[] = "\xBA\xD8\xB1\xDA";
constexpr char kWindow7NextExperience5861D0[] = "\xA4\x55\xA4\x40\xAF\xC5\xB8\x67\xC5\xE7\xAD\xC8";
constexpr char kWindow7Hp5861C8[] = "\xA5\xCD\xA9\x52\xAD\xC8";
constexpr char kWindow7DuelPoint5861B4[] = "\xB9\xEF\xBE\xD4\xB3\xD3\xA7\x51\xAD\xC8";
WindowTextPrototype gWindowText55FCF0 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow26Name586230,
};
WindowTextPrototype gWindowText55FCF8 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow10Level586228,
};
WindowTextPrototype gWindowText55FD00 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow7Penalty586224,
};
WindowTextPrototype gWindowText55FD08 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow7Job58621C,
};
WindowTextPrototype gWindowText55FD10 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow7Guild586214,
};
WindowTextPrototype gWindowText55FD18 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow7GuildTitle586208,
};
WindowTextPrototype gWindowText55FD20 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow7Title5861FC,
};
WindowTextPrototype gWindowText55FD28 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow7OwnTitle5861F0,
};
WindowTextPrototype gWindowText55FD30 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow7Tribe5861E8,
};
WindowTextPrototype gWindowText55FD38 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow10Experience5861E0,
};
WindowTextPrototype gWindowText55FD40 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow7NextExperience5861D0,
};
WindowTextPrototype gWindowText55FD48 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow7Hp5861C8,
};
WindowTextPrototype gWindowText55FD50 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow10Fp5861C0,
};
WindowTextPrototype gWindowText55FD58 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow7DuelPoint5861B4,
};

WindowGraphicData gWindowGraphic560680 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560698 {0x3BAB5, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5606B0 {0x3B584, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5606C8 {0x3BAB8, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5606E0 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5606F8 {0x3BAB4, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560710 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560728 {0x3BAB1, 0, 0, 0, 0, 0x80FFFFFFu};
WindowGraphicData gWindowGraphic560740 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindow8Text5861A8[] = "\xB7\x73\xBC\x57\xC2\x49\xBC\xC6";
constexpr char kWindow8Text5861A0[] = "\xC5\xE9\xA4\x4F";
constexpr char kWindow8Text586198[] = "\xA4\x4F\xB6\x71";
constexpr char kWindow8Text586190[] = "\xB1\x6A\xAB\xD7";
constexpr char kWindow8Text586188[] = "\xB3\x74\xAB\xD7";
constexpr char kWindow8Text586180[] = "\xC5\x5D\xAA\x6B";
constexpr char kWindow8Text586178[] = "\xA7\xF0\xC0\xBB";
constexpr char kWindow8Text586170[] = "\xA8\xBE\xBF\x6D";
constexpr char kWindow8Text586168[] = "\xB1\xD3\xB1\xB6";
constexpr char kWindow8Text586160[] = "\xBA\xEB\xAF\xAB";
constexpr char kWindow8Text586158[] = "\xA6\x5E\xB4\x5F";
constexpr char kWindow8Text586150[] = "\xBE\x79\xA4\x4F";
constexpr char kWindow8Text586148[] = "\xA7\xDC\xAC\x72";
constexpr char kWindow8Text586140[] = "\xA7\xDC\xA9\xFC\xBA\xCE";
constexpr char kWindow8Text586138[] = "\xA7\xDC\xA5\xDB\xA4\xC6";
constexpr char kWindow8Text586130[] = "\xA7\xDC\xBE\x4B";
constexpr char kWindow8Text586128[] = "\xA7\xDC\xB2\x56\xB6\xC3";
constexpr char kWindow8Text586120[] = "\xA7\xDC\xBF\xF2\xA7\xD1";
constexpr char kWindow8Text586118[] = "\xA5\xB2\xB1\xFE\xB2\x76";
constexpr char kWindow8Text586110[] = "\xA4\xCF\xC0\xBB\xB2\x76";
constexpr char kWindow8Text586108[] = "\xA9\x52\xA4\xA4\xB2\x76";
constexpr char kWindow8Text586100[] = "\xB0\x7B\xB8\xFA\xB2\x76";
constexpr char kWindow8Text5860F8[] = "\xAD\x40\xA4\x4F";
constexpr char kWindow8Text5860F0[] = "\xC6\x46\xA5\xA9";
constexpr char kWindow8Text5860E8[] = "\xB4\xBC\xA4\x4F";
constexpr char kWindow8Text5860E0[] = "\xC5\x5D\xA7\xF0";
constexpr char kWindow8Text5860D8[] = "\xC5\x5D\xA8\xBE";
WindowTextPrototype gWindowText560758 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow26Name586230,
};
WindowTextPrototype gWindowText560760 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow10Level586228,
};
WindowTextPrototype gWindowText560768 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text5861A8,
};
WindowTextPrototype gWindowText560770 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text5861A0,
};
WindowTextPrototype gWindowText560778 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586198,
};
WindowTextPrototype gWindowText560780 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586190,
};
WindowTextPrototype gWindowText560788 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586188,
};
WindowTextPrototype gWindowText560790 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586180,
};
WindowTextPrototype gWindowText560798 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586178,
};
WindowTextPrototype gWindowText5607A0 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586170,
};
WindowTextPrototype gWindowText5607A8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586168,
};
WindowTextPrototype gWindowText5607B0 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586160,
};
WindowTextPrototype gWindowText5607B8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586158,
};
WindowTextPrototype gWindowText5607C0 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586150,
};
WindowTextPrototype gWindowText5607C8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586148,
};
WindowTextPrototype gWindowText5607D0 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586140,
};
WindowTextPrototype gWindowText5607D8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586138,
};
WindowTextPrototype gWindowText5607E0 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586130,
};
WindowTextPrototype gWindowText5607E8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586128,
};
WindowTextPrototype gWindowText5607F0 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586120,
};
WindowTextPrototype gWindowText5607F8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586118,
};
WindowTextPrototype gWindowText560800 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586110,
};
WindowTextPrototype gWindowText560808 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586108,
};
WindowTextPrototype gWindowText560810 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text586100,
};
WindowTextPrototype gWindowText560818 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text5860F8,
};
WindowTextPrototype gWindowText560820 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text5860F0,
};
WindowTextPrototype gWindowText560828 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text5860E8,
};
WindowTextPrototype gWindowText560830 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text5860E0,
};
WindowTextPrototype gWindowText560838 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow8Text5860D8,
};

WindowGraphicData gWindowGraphic560DA0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560DB8 {0x3BAB5, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560DD0 {0x3B583, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560DE8 {0x3BAB9, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560E00 {0x3BAC2, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560E18 {0x3B574, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560E30 {0x3B577, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560E48 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560E60 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560E78 {0x3B59D, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560E90 {0x3BAB3, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560EA8 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560EC0 {0x3BAB1, 0, 0, 0, 0, 0x80808080u};
WindowGraphicData gWindowGraphic560ED8 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560EF0 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560F08 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic560F20 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindow9Title5860D0[] = "\xBA\xD9\xB8\xB9";
WindowTextPrototype gWindowText560F38 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow26Name586230,
};
WindowTextPrototype gWindowText560F40 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow10Level586228,
};
WindowTextPrototype gWindowText560F48 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow9Title5860D0,
};
std::uint32_t gWindowMouseFlagsD676F0 = 0;
std::uint32_t gWindowMouseFlagsD6772C = 0;

WindowGraphicData gWindowGraphic5613F0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561408 {0x3BAB5, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561420 {0x3B583, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561438 {0x3BAB8, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561450 {0x3C110, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561468 {0x3C10F, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561480 {0x3C10E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561498 {0x3BAB1, 0, 0, 0, 0, 0x80808080u};
WindowGraphicData gWindowGraphic5614B0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5614C8 {0x3BAC2, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5614E0 {0x3C0F0, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5614F8 {0x3C113, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindow58Guild5860C8[] = "\xA4\xBD\xB7\x7C";
constexpr char kWindow58Sell5860B8[] = "SELL";
constexpr char kWindow58Buy5860B4[] = "BUY";
constexpr char kWindow58About5860AC[] = "ABOUT";
WindowTextPrototype gWindowText561510 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow26Name586230,
};
WindowTextPrototype gWindowText561518 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow10Level586228,
};
WindowTextPrototype gWindowText561520 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow9Title5860D0,
};
WindowTextPrototype gWindowText561528 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow7Job58621C,
};
WindowTextPrototype gWindowText561530 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow58Guild5860C8,
};
WindowTextPrototype gWindowText561540 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow58Sell5860B8,
};
WindowTextPrototype gWindowText561548 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow58Buy5860B4,
};
WindowTextPrototype gWindowText561550 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow58About5860AC,
};

WindowGraphicData gWindowGraphic561FE0 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic561FF8 {0x3BA38, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562010 {0x3BA3A, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562028 {0x3BA3B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562040 {0x3B56B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562058 {0x3BA31, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562070 {0x3B565, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562088 {0x3BA3C, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5620A0 {0x3B5C3, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5620B8 {0x3BAC2, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindow11Placeholder586078[] =
    "\xA1\x48\xA1\x48\xA1\x48\xA1\x48\xA1\x48\xA1\x48\xA1\x48\xA1\x48\xA1\x48\xA1\x48"
    "\xA1\x40\xA1\x40\x30\x30\x30";
WindowTextPrototype gWindowText5620D0 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow11Placeholder586078,
};
std::int32_t gWindowAnimationImage5620D8 = 0x1AF16;
std::int32_t gWindowAnimationImage5620DC = 0x1AF16;

WindowGraphicData gWindowGraphic5628A8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5628C0 {0x3BA3F, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5628D8 {0x3BA3E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5628F0 {0x3BA31, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562908 {0x3B565, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindow13Placeholder586060[] =
    "\xA1\x48\xA1\x48\xA1\x48\xA1\x48\xA1\x48\xA1\x48\xA1\x48\xA1\x48\xA1\x48\xA1\x48";
WindowTextPrototype gWindowText562920 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow13Placeholder586060,
};
std::int32_t gWindowAnimationImage562928 = 0x1AF16;
std::int32_t gWindowAnimationImage56292C = 0x1AF16;

WindowGraphicData gWindowGraphic562568 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562580 {0x3BA2E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic562598 {0x3BA30, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5625C8 {0x3B56B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5625E0 {0x3B565, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5625F8 {0x3BA34, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowTextPrototype gWindowText562610 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow13Placeholder586060,
};
std::int32_t gWindowAnimationImage562618 = 0x1AF16;
std::int32_t gWindowAnimationImage56261C = 0x1AF16;

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

WindowGraphicData gWindowGraphic56D010 {243096, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56D028 {243090, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56D040 {243093, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56D058 {243042, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56D070 {243030, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56D088 {243033, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindowChoicePlaceholder585F64[] = "\xA1\xBD";
constexpr char kWindowChoiceTitle585F54[] = "\xBD\xD0\xBF\xEF\xBE\xDC\xB9\xEF\xB6\x48\xA1\x43";
WindowTextPrototype gWindowText56D0A0 {
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindowChoicePlaceholder585F64,
};
WindowTextPrototype gWindowText56D0A8 {
    {std::byte{4}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindowChoiceTitle585F54,
};

WindowGraphicData gWindowGraphic56D310 {245320, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56D328 {245332, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56D340 {243220, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56D358 {243220, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56D370 {243220, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56D388 {243220, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56D3A0 {243220, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic56D3B8 {243220, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowTextPrototype gWindowText56D3D0 {
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindowChoicePlaceholder585F64,
};
WindowTextPrototype gWindowText56D3D8 {
    {std::byte{4}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindowChoiceTitle585F54,
};
WindowTextPrototype gWindowText56D3E0 {
    {std::byte{0}, std::byte{1}, std::byte{0}, std::byte{0}},
    "\xB0\xB7\xB1\x64\xB5\x7B\xAB\xD7\x20\x20\xB5\xA5\xAF\xC5\x20\xA6"
    "\x57\xBA\xD9\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
    "\xC5\xE9\xA4\x4F\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20"
    "\xC5\x5D\xA4\x4F",
};

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

WindowGraphicData gWindowGraphic573AF0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573B08 {0x3BA9C, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573B20 {0x3BA9D, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573B38 {0x3BA9E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573B50 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573B68 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573B80 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573B98 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573BB0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573BC8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573BE0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573BF8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573C10 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573C28 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573C40 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573C58 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573C70 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573C88 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573CA0 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573CB8 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic573CD0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindowShopPlaceholder585DE8[] = "\xA1\x48\xA1\x48\xA1\x48\xA1\x48";
constexpr char kWindowShopUnavailable585DD8[] =
    "\xA1\xAF\xA5\xD8\xAB\x65\xB5\x4C\xAA\x6B\xB8\xCB\xB3\xC6";
WindowTextPrototype gWindowText573CF8 {
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, kWindowShopPlaceholder585DE8,
};
WindowTextPrototype gWindowText573D00 {
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, kWindowShopPlaceholder585DE8,
};
WindowTextPrototype gWindowText573D08 {
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, kWindowShopUnavailable585DD8,
};
std::int32_t gWindowAnimationImage573D10 = 100000;
WindowNumberData gWindowNumber573D18 {gShopMoneyTextCC1A00.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber573D20 {gShopValueTextCF4A28.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber573D28 {gShopTotalTextCC4B2C.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber573D30 {gShopPriceTextsD673EC.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber573D38 {gShopPriceTextsD673EC.data() + 11, 0, 3, 1, 0};
WindowNumberData gWindowNumber573D40 {gShopPriceTextsD673EC.data() + 22, 0, 3, 1, 0};
WindowNumberData gWindowNumber573D48 {gShopPriceTextsD673EC.data() + 33, 0, 3, 1, 0};
WindowNumberData gWindowNumber573D50 {gShopPriceTextsD673EC.data() + 44, 0, 3, 1, 0};
WindowNumberData gWindowNumber573D58 {gShopCountTextsD2D988.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber573D60 {gShopCountTextsD2D988.data() + 11, 0, 3, 1, 0};
WindowNumberData gWindowNumber573D68 {gShopCountTextsD2D988.data() + 22, 0, 3, 1, 0};
WindowNumberData gWindowNumber573D70 {gShopCountTextsD2D988.data() + 33, 0, 3, 1, 0};
WindowNumberData gWindowNumber573D78 {gShopCountTextsD2D988.data() + 44, 0, 3, 1, 0};
std::uint32_t gWindowMouseFlagsD678F0 = 0;

WindowGraphicData gWindowGraphic574668 {243000, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic574680 {244383, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic574698 {244384, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5746B0 {243096, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5746C8 {243090, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5746E0 {243093, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5746F8 {243048, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic574710 {245320, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic574728 {244387, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic574740 {243103, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic574758 {244386, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic574770 {243127, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic574788 {243030, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5747A0 {243033, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowTextPrototype gWindowText5747C0 {
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, kWindowPlaceholder586054,
};
WindowTextPrototype gWindowText5747C8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowPlaceholder586054,
};
WindowTextPrototype gWindowText5747D0 {
    {std::byte{0x8A}, std::byte{1}, std::byte{0}, std::byte{0}}, kWindowPlaceholder586054,
};
constexpr char kWindow54Zero585DF4[] = "0000000000";
WindowTextPrototype gWindowText5747D8 {
    {std::byte{0x8A}, std::byte{8}, std::byte{0}, std::byte{0}}, kWindow54Zero585DF4,
};
std::int32_t gWindowAnimationImage5747E0 = 100000;
WindowNumberData gWindowNumber5747E8 {gWindow54MoneyTextC9A9A4.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber5747F0 {gWindow54AmountTextD45F78.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber5747F8 {gWindow54SkillValueTextsCF4978.data(), 0, 0, 1, 0};
WindowNumberData gWindowNumber574800 {gWindow54SkillValueTextsCF4978.data() + 11, 0, 0, 1, 0};
WindowNumberData gWindowNumber574808 {gWindow54SkillValueTextsCF4978.data() + 22, 0, 0, 1, 0};
WindowNumberData gWindowNumber574810 {gWindow54SkillValueTextsCF4978.data() + 33, 0, 0, 1, 0};
WindowNumberData gWindowNumber574818 {gWindow54SkillValueTextsCF4978.data() + 44, 0, 0, 1, 0};
WindowNumberData gWindowNumber574820 {gWindow54SkillPriceTextsCF493C.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber574828 {gWindow54SkillPriceTextsCF493C.data() + 11, 0, 3, 1, 0};
WindowNumberData gWindowNumber574830 {gWindow54SkillPriceTextsCF493C.data() + 22, 0, 3, 1, 0};
WindowNumberData gWindowNumber574838 {gWindow54SkillPriceTextsCF493C.data() + 33, 0, 3, 1, 0};
WindowNumberData gWindowNumber574840 {gWindow54SkillPriceTextsCF493C.data() + 44, 0, 3, 1, 0};
WindowNumberData gWindowNumber574848 {gWindow54ValueTextsCB035C.data(), 0, 0, 1, 0};
WindowNumberData gWindowNumber574850 {gWindow54ValueTextsCB035C.data() + 11, 0, 0, 1, 0};
WindowNumberData gWindowNumber574858 {gWindow54ValueTextsCB035C.data() + 22, 0, 0, 1, 0};
WindowNumberData gWindowNumber574860 {gWindow54ValueTextsCB035C.data() + 33, 0, 0, 1, 0};
WindowNumberData gWindowNumber574868 {gWindow54ValueTextsCB035C.data() + 44, 0, 0, 1, 0};
WindowNumberData gWindowNumber574870 {gWindow54ValueTextsCB035C.data() + 55, 0, 0, 1, 0};
WindowNumberData gWindowNumber574878 {gWindow54ValueTextsCB035C.data() + 66, 0, 0, 1, 0};
WindowNumberData gWindowNumber574880 {gWindow54ValueTextsCB035C.data() + 77, 0, 0, 1, 0};
WindowNumberData gWindowNumber574888 {gWindow54ValueTextsCB035C.data() + 88, 0, 0, 1, 0};
WindowNumberData gWindowNumber574890 {gWindow54ValueTextsCB035C.data() + 99, 0, 0, 1, 0};
WindowNumberData gWindowNumber574898 {gWindow54ValueTextsC8B318.data(), 0, 0, 1, 0};
WindowNumberData gWindowNumber5748A0 {gWindow54ValueTextsC8B318.data() + 11, 0, 0, 1, 0};
WindowNumberData gWindowNumber5748A8 {gWindow54ValueTextsC8B318.data() + 22, 0, 0, 1, 0};
WindowNumberData gWindowNumber5748B0 {gWindow54ValueTextsC8B318.data() + 33, 0, 0, 1, 0};
WindowNumberData gWindowNumber5748B8 {gWindow54ValueTextsC8B318.data() + 44, 0, 0, 1, 0};
WindowNumberData gWindowNumber5748C0 {gWindow54ValueTextsC8B318.data() + 55, 0, 0, 1, 0};
WindowNumberData gWindowNumber5748C8 {gWindow54ValueTextsC8B318.data() + 66, 0, 0, 1, 0};
WindowNumberData gWindowNumber5748D0 {gWindow54ValueTextsC8B318.data() + 77, 0, 0, 1, 0};
WindowNumberData gWindowNumber5748D8 {gWindow54ValueTextsC8B318.data() + 88, 0, 0, 1, 0};
WindowNumberData gWindowNumber5748E0 {gWindow54ValueTextsC8B318.data() + 99, 0, 0, 1, 0};
std::uint32_t gWindowMouseFlagsD67900 = 0;

WindowGraphicData gWindowGraphic5756F0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575708 {0x17F32, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575720 {0x17F33, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575738 {0x17F34, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575750 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575768 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575780 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575798 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5757B0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5757C8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5757E0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5757F8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575810 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575828 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575840 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575858 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575870 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic575888 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5758A0 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5758B8 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5758D0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindowShopRemaining585DCC[] = "\xB3\xD1\xBE\x6C\x20\x25\x32\x64\x20\xAD\xD3";
WindowTextPrototype gWindowText5758F8 {
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, kWindowShopPlaceholder585DE8,
};
WindowTextPrototype gWindowText575900 {
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, kWindowShopPlaceholder585DE8,
};
WindowTextPrototype gWindowText575908 {
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, kWindowShopUnavailable585DD8,
};
WindowTextPrototype gWindowText575910 {
    {std::byte{1}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowShopRemaining585DCC,
};
std::int32_t gWindowAnimationImage575918 = 100000;
WindowNumberData gWindowNumber575920 {gWindow69BalanceTextD2E448.data(), 10, 3, 1, 0};
WindowNumberData gWindowNumber575930 {gWindow69TotalTextCC48B0.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber575938 {gWindow69PriceTextsD2D7CC.data(), 10, 3, 1, 0};
WindowNumberData gWindowNumber575940 {gWindow69PriceTextsD2D7CC.data() + 11, 10, 3, 1, 0};
WindowNumberData gWindowNumber575948 {gWindow69PriceTextsD2D7CC.data() + 22, 10, 3, 1, 0};
WindowNumberData gWindowNumber575950 {gWindow69PriceTextsD2D7CC.data() + 33, 10, 3, 1, 0};
WindowNumberData gWindowNumber575958 {gWindow69PriceTextsD2D7CC.data() + 44, 10, 3, 1, 0};
WindowNumberData gWindowNumber575960 {gWindow69CountTextsD2EA24.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber575968 {gWindow69CountTextsD2EA24.data() + 11, 0, 3, 1, 0};
WindowNumberData gWindowNumber575970 {gWindow69CountTextsD2EA24.data() + 22, 0, 3, 1, 0};
WindowNumberData gWindowNumber575978 {gWindow69CountTextsD2EA24.data() + 33, 0, 3, 1, 0};
WindowNumberData gWindowNumber575980 {gWindow69CountTextsD2EA24.data() + 44, 0, 3, 1, 0};
std::uint32_t gWindowMouseFlagsD67910 = 0;

WindowGraphicData gWindowGraphic5762F0 {0x3B538, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576308 {0x18661, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576320 {0x18662, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576338 {0x18663, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576350 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576368 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576380 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576398 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5763B0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5763C8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5763E0 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5763F8 {0x3B53E, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576410 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576428 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576440 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576458 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576470 {0x3B53B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576488 {0x3B568, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5764A0 {0x3BE48, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5764B8 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5764D0 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowTextPrototype gWindowText5764F8 {
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}}, kWindowShopPlaceholder585DE8,
};
WindowTextPrototype gWindowText576500 {
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, kWindowShopPlaceholder585DE8,
};
WindowTextPrototype gWindowText576508 {
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}}, kWindowShopUnavailable585DD8,
};
WindowTextPrototype gWindowText576510 {
    {std::byte{1}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowShopRemaining585DCC,
};
std::int32_t gWindowAnimationImage576518 = 100000;
WindowNumberData gWindowNumber576520 {gWindow70BalanceTextCF3F14.data(), 10, 3, 1, 0};
WindowNumberData gWindowNumber576530 {gWindow70TotalTextD2D804.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber576538 {gWindow70PriceTextsCF49C4.data(), 10, 3, 1, 0};
WindowNumberData gWindowNumber576540 {gWindow70PriceTextsCF49C4.data() + 11, 10, 3, 1, 0};
WindowNumberData gWindowNumber576548 {gWindow70PriceTextsCF49C4.data() + 22, 10, 3, 1, 0};
WindowNumberData gWindowNumber576550 {gWindow70PriceTextsCF49C4.data() + 33, 10, 3, 1, 0};
WindowNumberData gWindowNumber576558 {gWindow70PriceTextsCF49C4.data() + 44, 10, 3, 1, 0};
WindowNumberData gWindowNumber576560 {gWindow70CountTextsCC4848.data(), 0, 3, 1, 0};
WindowNumberData gWindowNumber576568 {gWindow70CountTextsCC4848.data() + 11, 0, 3, 1, 0};
WindowNumberData gWindowNumber576570 {gWindow70CountTextsCC4848.data() + 22, 0, 3, 1, 0};
WindowNumberData gWindowNumber576578 {gWindow70CountTextsCC4848.data() + 33, 0, 3, 1, 0};
WindowNumberData gWindowNumber576580 {gWindow70CountTextsCC4848.data() + 44, 0, 3, 1, 0};
std::uint32_t gWindowMouseFlagsD67920 = 0;

WindowGraphicData gWindowGraphic576EF0 {243048, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576F08 {30000, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576F20 {245042, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576F38 {243030, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic576F50 {243033, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindow56Message585D98[] =
    "MASSAGE789\xB2" "123456789\xC6" "123456789\xBB" "123456789\xD6" "123456789";
constexpr char kWindow56Class585D90[] = "\xC1\x79\xB3\xA1";
constexpr char kWindow56Fifth585D88[] = "\xB2\xB4\xB7\xFA";
constexpr char kWindow56Ones585D80[] = "\xBC\x4C\xA4\xDA";
WindowTextPrototype gWindowText576F68 {{}, kWindow56Message585D98};
WindowTextPrototype gWindowText576F70 {{}, kWindow56Class585D90};
WindowTextPrototype gWindowText576F78 {{}, kWindow56Fifth585D88};
WindowTextPrototype gWindowText576F80 {{}, kWindow56Ones585D80};
std::int32_t gWindowAnimationImage576F88 = 100000;

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

WindowGraphicData gWindowGraphic578BE8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic577170 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic577188 {0x3BB00, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5771A0 {0x3BB01, 0, 0, 0, 0, 0x80FFFFFFu};
WindowGraphicData gWindowGraphic5771B8 {0x3BB03, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5771E8 {0x3B598, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic577200 {0x3B592, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic577218 {0x3B595, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic577248 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic577260 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic577278 {0x3C386, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindow59Placeholder585D78[] = "\x3F\x82\xA2\x82\xC4\x82\xDE";
WindowTextPrototype gWindowText577290 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindow59Placeholder585D78,
};
std::uint32_t gWindowMouseFlagsD67930 = 0;

WindowGraphicData gWindowGraphic577740 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic577758 {0x3C119, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic577770 {0x3BB3D, 0, 0, 0, 0, 0x8FFFFFFFu};
WindowGraphicData gWindowGraphic577788 {0x3C11A, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowTextPrototype gWindowText5777A0 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, "\x83\x4A\x83\x65\x83\x53\x3F",
};

WindowGraphicData gWindowGraphic578C00 {0x3C11B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic578C18 {0x3C11C, 0, 0, 0, 0, 0x8FFFFFFFu};
WindowGraphicData gWindowGraphic578C30 {0x3C12A, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic578C48 {0x3C11D, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic578C60 {0x3B556, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic578C78 {0x3B559, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic578C90 {0x3C120, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindowTextBytes585D70[] = "\x83\x4A\x83\x65\x83\x53\x3F";
constexpr char kWindowTextBytes585D64[] = "\x3F\x83\x62\x83\x5A\x81\x5B\x83\x57";
constexpr char kWindowTextBytes585D5C[] = "\x82\xC8\x82\xDC\x82\xA6";
constexpr char kWindowTextBytes585D54[] = "\x3F\x8D\xF5\x95\xB6\x3F";
WindowTextPrototype gWindowText578CA8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D70,
};
WindowTextPrototype gWindowText578CB0 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D64,
};
WindowTextPrototype gWindowText578CB8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D5C,
};
WindowTextPrototype gWindowText578CC0 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D54,
};

WindowGraphicData gWindowGraphic5795A8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic579620 {0x3C10F, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic579638 {0x3C10D, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic579650 {0x3BAB1, 0, 0, 0, 0, 0x80808080u};
WindowGraphicData gWindowGraphic579698 {0x3C0F0, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5796B0 {0x3BA1B, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindowTextBytes585D4C[] = "\x96\xBC\x91\x4F";
constexpr char kWindowTextBytes585D44[] = "\x3F\x83\x78\x3F";
constexpr char kWindowTextBytes585D38[] = "\x82\xBD\x82\xA2\x82\xC6\x82\xE9";
constexpr char kWindowTextBytes585D28[] = "\x82\xB5\x82\xE5\x82\xAD\x82\xAC\x82\xE5\x82\xA4";
constexpr char kWindowTextBytes585D20[] = "\x82\xAC\x82\xE9\x82\xC7";
constexpr char kWindowTextBytes585D10[] = "\x82\xB9\x82\xE9";
constexpr char kWindowTextBytes585D08[] = "\x82\xCE\x82\xA2";
constexpr char kWindowTextBytes585D00[] = "\x3F\x82\xCE\x82\xA4\x82\xC6";
WindowTextPrototype gWindowText5796C8 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D4C,
};
WindowTextPrototype gWindowText5796D0 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D44,
};
WindowTextPrototype gWindowText5796D8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D38,
};
WindowTextPrototype gWindowText5796E0 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D28,
};
WindowTextPrototype gWindowText5796E8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D20,
};
WindowTextPrototype gWindowText5796F8 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D10,
};
WindowTextPrototype gWindowText579700 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D08,
};
WindowTextPrototype gWindowText579708 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D00,
};
WindowGraphicData gWindowGraphic5799D0 {0x3BA0B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic5799E8 {0x3BA06, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic579A00 {0x3BA07, 0, 0, 0, 0, 0x80FFFFFFu};
WindowGraphicData gWindowGraphic579A78 {0x3BA1B, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic579AA8 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic579B08 {0x3C123, 0, 0, 0, 0, 0xFFFFFFFFu};
constexpr char kWindowTextBytes585CF4[] = "\x82\xC4\x82\xB7\x82\xC6\x82\xB3\x82\xF1";
WindowTextPrototype gWindowText579B20 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585CF4,
};

WindowGraphicData gWindowGraphic579F20 {0x3B539, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic579F98 {0x3C10F, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic579FB0 {0x3C10D, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic579FC8 {0x3BAB1, 0, 0, 0, 0, 0x80808080u};
WindowGraphicData gWindowGraphic57A010 {0x3C0F0, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowTextPrototype gWindowText57A028 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D4C,
};
WindowTextPrototype gWindowText57A030 {
    {std::byte{0x7F}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D44,
};
WindowTextPrototype gWindowText57A038 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D38,
};
WindowTextPrototype gWindowText57A040 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D28,
};
WindowTextPrototype gWindowText57A048 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D20,
};
WindowTextPrototype gWindowText57A058 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D10,
};
WindowTextPrototype gWindowText57A060 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D08,
};
WindowTextPrototype gWindowText57A068 {
    {std::byte{0}, std::byte{12}, std::byte{0}, std::byte{0}}, kWindowTextBytes585D00,
};

WindowGraphicData gWindowGraphic57B288 {243000, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B2A0 {98891, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B2B8 {98888, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B2D0 {98885, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B2E8 {98870, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B300 {98882, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B318 {98873, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B360 {98859, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B378 {98861, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B390 {98854, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B3A8 {243096, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B3C0 {243090, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B3D8 {243093, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B3F0 {98864, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowGraphicData gWindowGraphic57B408 {98867, 0, 0, 0, 0, 0xFFFFFFFFu};
WindowTextPrototype gWindowText57B420 {
    {std::byte{0x7F}, std::byte{8}, std::byte{0}, std::byte{0}},
    "\xA3\xBF\xA3\xBF\xA3\xBF\xA3\xBF",
};
WindowTextPrototype gWindowText57B428 {
    {std::byte{0x8C}, std::byte{4}, std::byte{0}, std::byte{0}},
    "\xBD\xD0\xBF\xE9\xA4\x4A\xA9\xB1\xA6\x57\xA4\x4B\xAD\xD3\xA6\x72",
};
WindowTextPrototype gWindowText57B430 {
    {std::byte{0}, std::byte{8}, std::byte{0}, std::byte{0}},
    "\xBD\xD0\xBF\xE9\xA4\x4A\xA9\xB1\xE7\x45\xB4\x79\xAD\x7A",
};
WindowNumberData gWindowNumber57B450 {gStallMoneyText57B440.data(), 0, 3, 1, 0};
std::int32_t gWindowAnimationImage57B458 = 100000;
std::uint32_t gWindowMouseFlagsD67A10 = 0;

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

WindowElementPrototype gWindowElements57B508[48] {
    {0, 0, 0, 549, 366, 1, {.unusedValue = 0}, &Sub_4D0230},
    {5, 0, 0, 549, 366, 1, {.unusedValue = 0}, &Sub_4D2260},
    {1, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic57B288}, &Sub_4CD420},
    {7, 393, 30, 0, 0, 1, {.number = &gWindowNumber57B450}, &Sub_4B0F10},
    {3, 412, 44, 0, 14, 1, {.text = &gWindowText57B420}, &Sub_4B0F10},
    {3, 230, 25, 0, 14, 1, {.text = &gWindowText57B428}, &Sub_4B0F10},
    {5, 230, 11, 120, 16, 1, {.unusedValue = 0}, &Sub_4D2170},
    {3, 298, 51, 0, 0, 1, {.text = &gWindowText57B430}, &Sub_4B0F10},
    {5, 299, 51, 167, 57, 1, {.unusedValue = 0}, &Sub_4D2080},
    {2, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage57B458}, &Sub_4B0F10},
    {0, 15, 28, 274, 325, 1, {.unusedValue = 0}, &Sub_4CE0F0},
    {0, 292, 28, 250, 325, 1, {.unusedValue = 0}, &Sub_4CE370},
    {0, 22, 53, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 22, 109, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 22, 165, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 22, 221, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 22, 277, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 144, 53, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 144, 109, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 144, 165, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 144, 221, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 144, 277, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 292, 127, 248, 198, 1, {.unusedValue = 0}, &Sub_4D1D50},
    {0, 22, 53, 240, 90, 1, {.unusedValue = 0}, &Sub_4D0EB0},
    {0, 22, 144, 240, 90, 1, {.unusedValue = 0}, &Sub_4D0EB0},
    {0, 22, 235, 240, 90, 1, {.unusedValue = 0}, &Sub_4D0EB0},
    {0, 292, 127, 231, 48, 1, {.unusedValue = 0}, &Sub_4D16F0},
    {0, 292, 177, 231, 48, 1, {.unusedValue = 0}, &Sub_4D16F0},
    {0, 292, 227, 231, 48, 1, {.unusedValue = 0}, &Sub_4D16F0},
    {0, 292, 277, 231, 48, 1, {.unusedValue = 0}, &Sub_4D16F0},
    {1, 45, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2A0}, &Sub_4CF940},
    {1, 45, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2B8}, &Sub_4CF940},
    {1, 188, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2E8}, &Sub_4CF940},
    {1, 461, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B300}, &Sub_4CF940},
    {1, 322, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B318}, &Sub_4CF940},
    {1, 26, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B3F0}, &Sub_4CE3F0},
    {1, 90, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B408}, &Sub_4CE3F0},
    {1, 267, 65, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &Sub_4B0F10},
    {4, 267, 65, 11, 248, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &Sub_4741A0},
    {1, 267, 55, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &Sub_4CDE40},
    {1, 267, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &Sub_4CDFA0},
    {1, 527, 139, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &Sub_4B0F10},
    {4, 527, 139, 11, 174, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &Sub_4741A0},
    {1, 527, 129, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &Sub_4CE1F0},
    {1, 527, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &Sub_4CE2B0},
    {1, 22, 53, 243, 274, 0, {.graphic = &gWindowGraphic57B360}, &Sub_4B0F10},
    {1, 292, 127, 248, 198, 0, {.graphic = &gWindowGraphic57B378}, &Sub_4B0F10},
    {1, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic57B390}, &Sub_4B0F10},
};

WindowElementPrototype gWindowElements57BB08[43] {
    {0, 0, 0, 549, 366, 1, {.unusedValue = 0}, &Sub_4D0230},
    {0, 0, 0, 549, 366, 1, {.unusedValue = 0}, &Sub_4D2840},
    {1, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic57B288}, &Sub_4CD420},
    {7, 393, 30, 0, 0, 1, {.number = &gWindowNumber57B450}, &Sub_4B0F10},
    {3, 412, 44, 0, 14, 1, {.text = &gWindowText57B420}, &Sub_4B0F10},
    {3, 230, 25, 0, 14, 1, {.text = &gWindowText57B428}, &Sub_4B0F10},
    {0, 299, 51, 167, 57, 1, {.unusedValue = 0}, &Sub_4B0F10},
    {2, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage57B458}, &Sub_4B0F10},
    {0, 15, 28, 274, 325, 1, {.unusedValue = 0}, &Sub_4CE0F0},
    {0, 292, 28, 250, 325, 1, {.unusedValue = 0}, &Sub_4CE370},
    {0, 22, 53, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 22, 109, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 22, 165, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 22, 221, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 22, 277, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 144, 53, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 144, 109, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 144, 165, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 144, 221, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 144, 277, 48, 48, 1, {.unusedValue = 0}, &Sub_4D04A0},
    {0, 292, 127, 248, 198, 1, {.unusedValue = 0}, &Sub_4D1D50},
    {0, 22, 53, 240, 90, 1, {.unusedValue = 0}, &Sub_4D0EB0},
    {0, 22, 144, 240, 90, 1, {.unusedValue = 0}, &Sub_4D0EB0},
    {0, 22, 235, 240, 90, 1, {.unusedValue = 0}, &Sub_4D0EB0},
    {0, 292, 127, 231, 48, 1, {.unusedValue = 0}, &Sub_4D16F0},
    {0, 292, 177, 231, 48, 1, {.unusedValue = 0}, &Sub_4D16F0},
    {0, 292, 227, 231, 48, 1, {.unusedValue = 0}, &Sub_4D16F0},
    {0, 292, 277, 231, 48, 1, {.unusedValue = 0}, &Sub_4D16F0},
    {1, 188, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B2D0}, &Sub_4CF940},
    {1, 461, 335, 49, 17, 1, {.graphic = &gWindowGraphic57B300}, &Sub_4CF940},
    {1, 26, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B3F0}, &Sub_4CE3F0},
    {1, 90, 28, 63, 18, 1, {.graphic = &gWindowGraphic57B408}, &Sub_4CE3F0},
    {1, 267, 65, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &Sub_4B0F10},
    {4, 267, 65, 11, 248, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &Sub_4741A0},
    {1, 267, 55, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &Sub_4CDE40},
    {1, 267, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &Sub_4CDFA0},
    {1, 527, 139, 0, 14, 1, {.graphic = &gWindowGraphic57B3A8}, &Sub_4B0F10},
    {4, 527, 139, 11, 174, 1, {.mouseFlags = &gWindowMouseFlagsD67A10}, &Sub_4741A0},
    {1, 527, 129, 11, 11, 1, {.graphic = &gWindowGraphic57B3C0}, &Sub_4CE1F0},
    {1, 527, 312, 11, 11, 1, {.graphic = &gWindowGraphic57B3D8}, &Sub_4CE2B0},
    {1, 22, 53, 243, 274, 0, {.graphic = &gWindowGraphic57B360}, &Sub_4B0F10},
    {1, 292, 127, 248, 198, 0, {.graphic = &gWindowGraphic57B378}, &Sub_4B0F10},
    {1, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic57B390}, &Sub_4B0F10},
};

WindowElementPrototype gWindowElements577298[32] {
    {1, 206, 8, 11, 11, 1, {.graphic = &gWindowGraphic577170}, &Sub_4C1FB0},
    {1, 206, 41, 0, 14, 1, {.graphic = &gWindowGraphic5771E8}, &Sub_4B0F10},
    {4, 205, 41, 11, 134, 1, {.mouseFlags = &gWindowMouseFlagsD67930}, &Sub_4C9B80},
    {1, 206, 30, 11, 11, 1, {.graphic = &gWindowGraphic577200}, &Sub_4C96F0},
    {1, 206, 174, 11, 11, 1, {.graphic = &gWindowGraphic577218}, &Sub_4C96F0},
    {1, 89, 7, 0, 0, 1, {.graphic = &gWindowGraphic577278}, &Sub_4B0F10},
    {1, 40, 189, 20, 20, 1, {.graphic = &gWindowGraphic577248}, &Sub_4C96F0},
    {1, 170, 189, 20, 20, 1, {.graphic = &gWindowGraphic577260}, &Sub_4C96F0},
    {0, 0, 0, 225, 220, 1, {.unusedValue = 0}, &Sub_4C9BE0},
    {3, 20, 30, 0, 0, 1, {.text = &gWindowText577290}, &Sub_4B0F10},
    {3, 20, 46, 0, 0, 1, {.text = &gWindowText577290}, &Sub_4B0F10},
    {3, 20, 62, 0, 0, 1, {.text = &gWindowText577290}, &Sub_4B0F10},
    {3, 20, 78, 0, 0, 1, {.text = &gWindowText577290}, &Sub_4B0F10},
    {3, 20, 94, 0, 0, 1, {.text = &gWindowText577290}, &Sub_4B0F10},
    {3, 20, 110, 0, 0, 1, {.text = &gWindowText577290}, &Sub_4B0F10},
    {3, 20, 126, 0, 0, 1, {.text = &gWindowText577290}, &Sub_4B0F10},
    {3, 20, 142, 0, 0, 1, {.text = &gWindowText577290}, &Sub_4B0F10},
    {3, 20, 158, 0, 0, 1, {.text = &gWindowText577290}, &Sub_4B0F10},
    {3, 20, 174, 0, 0, 1, {.text = &gWindowText577290}, &Sub_4B0F10},
    {1, 15, 28, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &Sub_4C94F0},
    {1, 15, 44, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &Sub_4C94F0},
    {1, 15, 60, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &Sub_4C94F0},
    {1, 15, 76, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &Sub_4C94F0},
    {1, 15, 92, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &Sub_4C94F0},
    {1, 15, 108, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &Sub_4C94F0},
    {1, 15, 124, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &Sub_4C94F0},
    {1, 15, 140, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &Sub_4C94F0},
    {1, 15, 156, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &Sub_4C94F0},
    {1, 15, 172, 186, 16, 1, {.graphic = &gWindowGraphic5771B8}, &Sub_4C94F0},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic577188}, &Sub_4B0F10},
    {1, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic5771A0}, &Sub_4B0F10},
    {0, 225, 0, 20, 130, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements5777A8[162] {
    {3, 50, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 50, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 150, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 268, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 368, 378, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 78, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 98, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 118, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 138, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 158, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 178, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 198, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 218, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 238, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 258, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 278, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 298, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 318, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 338, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {3, 484, 358, 0, 0, 1, {.text = &gWindowText5777A0}, &Sub_4B0F10},
    {1, 48, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 48, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 148, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 266, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 366, 376, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 76, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 96, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 116, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 136, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 156, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 176, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 196, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 216, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 236, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 256, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 276, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 296, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 316, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 336, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 482, 356, 90, 12, 1, {.graphic = &gWindowGraphic577788}, &Sub_4CA1A0},
    {1, 593, 8, 12, 12, 1, {.graphic = &gWindowGraphic577740}, &Sub_4C1FB0},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic577758}, &Sub_4B0F10},
    {1, 15, 30, 0, 0, 1, {.graphic = &gWindowGraphic577770}, &Sub_4B0F10},
    {0, 600, 0, 34, 130, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements578CC8[71] {
    {1, 284, 9, 10, 10, 1, {.graphic = &gWindowGraphic578BE8}, &Sub_4C1FB0},
    {3, 120, 30, 0, 0, 1, {.text = &gWindowText578CA8}, &Sub_4B0F10},
    {1, 140, 8, 0, 0, 1, {.graphic = &gWindowGraphic578C48}, &Sub_4B0F10},
    {1, 190, 367, 55, 20, 1, {.graphic = &gWindowGraphic578C90}, &Sub_4CA6F0},
    {1, 250, 367, 20, 20, 1, {.graphic = &gWindowGraphic578C60}, &Sub_4CA6F0},
    {1, 270, 367, 20, 20, 1, {.graphic = &gWindowGraphic578C78}, &Sub_4CA6F0},
    {3, 20, 370, 0, 0, 1, {.text = &gWindowText578CC0}, &Sub_4B0F10},
    {5, 20, 370, 168, 14, 1, {.unusedValue = 0}, &Sub_4CAB90},
    {3, 20, 47, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 63, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 79, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 95, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 111, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 127, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 143, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 159, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 175, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 191, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 207, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 223, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 239, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 255, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 271, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 287, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 303, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 319, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 335, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 20, 351, 0, 0, 1, {.text = &gWindowText578CB0}, &Sub_4B0F10},
    {3, 195, 47, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 63, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 79, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 95, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 111, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 127, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 143, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 159, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 175, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 191, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 207, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 223, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 239, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 255, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 271, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 287, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 303, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 319, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 335, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {3, 195, 351, 0, 0, 1, {.text = &gWindowText578CB8}, &Sub_4B0F10},
    {1, 15, 45, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 61, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 77, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 93, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 109, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 125, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 141, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 157, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 173, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 189, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 205, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 221, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 237, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 253, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 269, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 285, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 301, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 317, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 333, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 15, 349, 280, 16, 1, {.graphic = &gWindowGraphic578C30}, &Sub_4CA9C0},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic578C00}, &Sub_4B0F10},
    {1, 14, 27, 0, 0, 1, {.graphic = &gWindowGraphic578C18}, &Sub_4B0F10},
    {0, 300, 0, 30, 130, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements579710[22] {
    {1, 257, 32, 40, 30, 1, {.graphic = &gWindowGraphic5796B0}, &Sub_4CB2F0},
    {3, 70, 10, 0, 0, 1, {.text = &gWindowText5796C8}, &Sub_4B0F10},
    {3, 272, 11, 0, 0, 1, {.text = &gWindowText5796D0}, &Sub_4B0F10},
    {3, 140, 39, 0, 0, 1, {.text = &gWindowText5796D8}, &Sub_4B0F10},
    {3, 140, 59, 0, 0, 1, {.text = &gWindowText5796E0}, &Sub_4B0F10},
    {3, 140, 79, 0, 0, 1, {.text = &gWindowText5796E8}, &Sub_4B0F10},
    {3, 97, 108, 0, 0, 1, {.text = &gWindowText5796F8}, &Sub_4B0F10},
    {3, 97, 130, 0, 0, 1, {.text = &gWindowText579700}, &Sub_4B0F10},
    {3, 97, 152, 0, 0, 1, {.text = &gWindowText579708}, &Sub_4B0F10},
    {3, 97, 170, 0, 0, 1, {.text = &gWindowText579708}, &Sub_4B0F10},
    {3, 97, 184, 0, 0, 1, {.text = &gWindowText579708}, &Sub_4B0F10},
    {3, 97, 198, 0, 0, 1, {.text = &gWindowText579708}, &Sub_4B0F10},
    {3, 97, 212, 0, 0, 1, {.text = &gWindowText579708}, &Sub_4B0F10},
    {1, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic5795A8}, &Sub_4C1FB0},
    {1, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic5795A8}, &Sub_4B0F10},
    {1, 73, 102, 20, 20, 1, {.graphic = &gWindowGraphic579698}, &Sub_4B0F10},
    {1, 73, 124, 20, 20, 1, {.graphic = &gWindowGraphic579698}, &Sub_4B0F10},
    {1, 73, 146, 20, 20, 1, {.graphic = &gWindowGraphic579698}, &Sub_4B0F10},
    {1, 15, 36, 0, 0, 1, {.graphic = &gWindowGraphic579620}, &Sub_4B0F10},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic579638}, &Sub_4B0F10},
    {1, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic579650}, &Sub_4B0F10},
    {0, 307, 0, 30, 130, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements579B28[16] {
    {1, 191, 9, 11, 11, 1, {.graphic = &gWindowGraphic579AA8}, &Sub_4C1FB0},
    {5, 22, 70, 170, 120, 1, {.unusedValue = 0}, &Sub_4CB920},
    {1, 154, 28, 50, 18, 1, {.graphic = &gWindowGraphic579A78}, &Sub_4CB6A0},
    {1, 80, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &Sub_4CB6A0},
    {1, 72, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &Sub_4CB6A0},
    {1, 64, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &Sub_4CB6A0},
    {1, 56, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &Sub_4CB6A0},
    {1, 48, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &Sub_4CB6A0},
    {1, 40, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &Sub_4CB6A0},
    {1, 32, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &Sub_4CB6A0},
    {1, 24, 30, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &Sub_4CB6A0},
    {1, 20, 28, 0, 0, 1, {.graphic = &gWindowGraphic579B08}, &Sub_4CB6A0},
    {3, 21, 48, 0, 0, 1, {.text = &gWindowText579B20}, &Sub_4B0F10},
    {1, 16, 46, 0, 0, 1, {.graphic = &gWindowGraphic5799D0}, &Sub_4B0F10},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5799E8}, &Sub_4B0F10},
    {1, 14, 23, 0, 0, 1, {.graphic = &gWindowGraphic579A00}, &Sub_4B0F10},
};

WindowElementPrototype gWindowElements57A070[21] {
    {3, 70, 10, 0, 0, 1, {.text = &gWindowText57A028}, &Sub_4B0F10},
    {3, 272, 11, 0, 0, 1, {.text = &gWindowText57A030}, &Sub_4B0F10},
    {3, 140, 39, 0, 0, 1, {.text = &gWindowText57A038}, &Sub_4B0F10},
    {3, 140, 59, 0, 0, 1, {.text = &gWindowText57A040}, &Sub_4B0F10},
    {3, 140, 79, 0, 0, 1, {.text = &gWindowText57A048}, &Sub_4B0F10},
    {3, 97, 108, 0, 0, 1, {.text = &gWindowText57A058}, &Sub_4B0F10},
    {3, 97, 130, 0, 0, 1, {.text = &gWindowText57A060}, &Sub_4B0F10},
    {3, 97, 152, 0, 0, 1, {.text = &gWindowText57A068}, &Sub_4B0F10},
    {3, 97, 170, 0, 0, 1, {.text = &gWindowText57A068}, &Sub_4B0F10},
    {3, 97, 184, 0, 0, 1, {.text = &gWindowText57A068}, &Sub_4B0F10},
    {3, 97, 198, 0, 0, 1, {.text = &gWindowText57A068}, &Sub_4B0F10},
    {3, 97, 212, 0, 0, 1, {.text = &gWindowText57A068}, &Sub_4B0F10},
    {1, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic579F20}, &Sub_4C1FB0},
    {1, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic579F20}, &Sub_4B0F10},
    {1, 73, 102, 20, 20, 1, {.graphic = &gWindowGraphic57A010}, &Sub_478330},
    {1, 73, 124, 20, 20, 1, {.graphic = &gWindowGraphic57A010}, &Sub_478330},
    {1, 73, 146, 20, 20, 1, {.graphic = &gWindowGraphic57A010}, &Sub_478330},
    {1, 15, 36, 0, 0, 1, {.graphic = &gWindowGraphic579F98}, &Sub_4B0F10},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic579FB0}, &Sub_4B0F10},
    {1, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic579FC8}, &Sub_4B0F10},
    {0, 315, 0, 20, 130, 1, {.unusedValue = 0}, &Sub_474170},
};

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

WindowElementPrototype gWindowElements56B710[14] {
    {1, 13, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B5F8}, &Sub_4972B0},
    {1, 13, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B610}, &Sub_4973C0},
    {1, 83, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B628}, &Sub_4975A0},
    {1, 83, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B640}, &Sub_497770},
    {1, 153, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B658}, &Sub_497900},
    {1, 153, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B670}, &Sub_497A60},
    {1, 223, 45, 64, 20, 1, {.graphic = &gWindowGraphic56B688}, &Sub_497B80},
    {1, 223, 20, 64, 20, 1, {.graphic = &gWindowGraphic56B6A0}, &Sub_497C80},
    {1, 216, 46, 64, 20, 1, {.graphic = &gWindowGraphic56B628}, &Sub_498160},
    {1, 216, 46, 64, 20, 1, {.graphic = &gWindowGraphic56B688}, &Sub_497B80},
    {3, 20, 24, 0, 0, 1, {.text = &gWindowText56B6F8}, &Sub_4B0F10},
    {3, 215, 24, 0, 0, 1, {.text = &gWindowText56B700}, &Sub_4B0F10},
    {3, 20, 48, 0, 0, 1, {.text = &gWindowText56B708}, &Sub_4B0F10},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic56B5E0}, &Sub_4B0F10},
};

WindowElementPrototype gWindowElements55F5C0[14] {
    {0, 0, 0, 640, 30, 1, {.unusedValue = 0}, &Sub_473840},
    {1, 10, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F4C0}, &Sub_473840},
    {1, 86, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F4D8}, &Sub_473840},
    {1, 163, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F4F0}, &Sub_473840},
    {1, 239, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F508}, &Sub_473840},
    {1, 316, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F520}, &Sub_473840},
    {1, 392, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F538}, &Sub_473840},
    {1, 464, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F550}, &Sub_473840},
    {1, 555, 7, 60, 20, 1, {.graphic = &gWindowGraphic55F568}, &Sub_473840},
    {3, 565, 11, 0, 0, 1, {.text = &gWindowText55F5B8}, &Sub_4B0F10},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F580}, &Sub_4B0F10},
    {3, 21, 10, 0, 0, 1, {.text = &gWindowText55F5B0}, &Sub_4B0F10},
    {3, 408, 10, 0, 0, 1, {.text = &gWindowText55F5B0}, &Sub_4B0F10},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F598}, &Sub_4B0F10},
};

WindowElementPrototype gWindowElements55FD60[73] {
    {3, 70, 10, 0, 0, 1, {.text = &gWindowText55FCF0}, &Sub_4B0F10},
    {3, 272, 11, 0, 0, 1, {.text = &gWindowText55FCF8}, &Sub_4B0F10},
    {3, 70, 101, 0, 0, 1, {.text = &gWindowText55FD00}, &Sub_4B0F10},
    {3, 165, 31, 0, 0, 1, {.text = &gWindowText55FD08}, &Sub_4B0F10},
    {3, 165, 49, 0, 0, 1, {.text = &gWindowText55FD10}, &Sub_4B0F10},
    {3, 165, 68, 0, 0, 1, {.text = &gWindowText55FD18}, &Sub_4B0F10},
    {3, 165, 86, 0, 0, 1, {.text = &gWindowText55FD20}, &Sub_4B0F10},
    {3, 165, 104, 0, 0, 1, {.text = &gWindowText55FD28}, &Sub_4B0F10},
    {3, 89, 123, 0, 0, 1, {.text = &gWindowText55FD30}, &Sub_4B0F10},
    {3, 89, 141, 0, 0, 1, {.text = &gWindowText55FD38}, &Sub_4B0F10},
    {3, 89, 158, 0, 0, 1, {.text = &gWindowText55FD40}, &Sub_4B0F10},
    {3, 89, 176, 0, 0, 1, {.text = &gWindowText55FD48}, &Sub_4B0F10},
    {3, 89, 194, 0, 0, 1, {.text = &gWindowText55FD50}, &Sub_4B0F10},
    {3, 89, 211, 0, 0, 1, {.text = &gWindowText55FD58}, &Sub_4B0F10},
    {1, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic55F780}, &Sub_4B0F10},
    {1, 48, 103, 0, 0, 1, {.graphic = &gWindowGraphic55F798}, &Sub_4B0F10},
    {1, 60, 104, 0, 0, 1, {.graphic = &gWindowGraphic55F7B0}, &Sub_4B0F10},
    {1, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic55F7C8}, &Sub_4C1FB0},
    {1, 151, 230, 0, 0, 1, {.graphic = &gWindowGraphic55F7E0}, &Sub_4B0F10},
    {1, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic55F7F8}, &Sub_474BE0},
    {1, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic55F810}, &Sub_474BE0},
    {1, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic55FCD8}, &Sub_474BE0},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55F858}, &Sub_4B0F10},
    {1, 217, 200, 36, 25, 1, {.graphic = &gWindowGraphic55F828}, &Sub_474BE0},
    {1, 253, 200, 36, 25, 1, {.graphic = &gWindowGraphic55F840}, &Sub_474BE0},
    {1, 229, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &Sub_4B0F10},
    {1, 237, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &Sub_4B0F10},
    {1, 245, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &Sub_4B0F10},
    {1, 253, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &Sub_4B0F10},
    {1, 261, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &Sub_4B0F10},
    {1, 269, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &Sub_4B0F10},
    {1, 277, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &Sub_4B0F10},
    {1, 285, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &Sub_4B0F10},
    {1, 293, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &Sub_4B0F10},
    {1, 301, 130, 0, 0, 1, {.graphic = &gWindowGraphic55F870}, &Sub_4B0F10},
    {1, 229, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &Sub_4B0F10},
    {1, 237, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &Sub_4B0F10},
    {1, 245, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &Sub_4B0F10},
    {1, 253, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &Sub_4B0F10},
    {1, 261, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &Sub_4B0F10},
    {1, 269, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &Sub_4B0F10},
    {1, 277, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &Sub_4B0F10},
    {1, 285, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &Sub_4B0F10},
    {1, 293, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &Sub_4B0F10},
    {1, 301, 147, 0, 0, 1, {.graphic = &gWindowGraphic55F960}, &Sub_4B0F10},
    {1, 229, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &Sub_4B0F10},
    {1, 237, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &Sub_4B0F10},
    {1, 245, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &Sub_4B0F10},
    {1, 253, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &Sub_4B0F10},
    {1, 261, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &Sub_4B0F10},
    {1, 269, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &Sub_4B0F10},
    {1, 277, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &Sub_4B0F10},
    {1, 285, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &Sub_4B0F10},
    {1, 293, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &Sub_4B0F10},
    {1, 301, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FA50}, &Sub_4B0F10},
    {1, 229, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &Sub_4B0F10},
    {1, 237, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &Sub_4B0F10},
    {1, 245, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &Sub_4B0F10},
    {1, 253, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &Sub_4B0F10},
    {1, 261, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &Sub_4B0F10},
    {1, 269, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &Sub_4B0F10},
    {1, 277, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &Sub_4B0F10},
    {1, 285, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &Sub_4B0F10},
    {1, 293, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &Sub_4B0F10},
    {1, 301, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FB40}, &Sub_4B0F10},
    {1, 195, 130, 0, 0, 1, {.graphic = &gWindowGraphic55FC30}, &Sub_4B0F10},
    {1, 195, 147, 0, 0, 1, {.graphic = &gWindowGraphic55FC48}, &Sub_4B0F10},
    {1, 195, 163, 0, 0, 1, {.graphic = &gWindowGraphic55FC60}, &Sub_4B0F10},
    {1, 195, 179, 0, 0, 1, {.graphic = &gWindowGraphic55FC78}, &Sub_4B0F10},
    {1, 16, 29, 0, 0, 1, {.graphic = &gWindowGraphic55FC90}, &Sub_4B0F10},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic55FCA8}, &Sub_4B0F10},
    {1, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic55FCC0}, &Sub_4B0F10},
    {0, 315, 0, 20, 130, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements560840[43] {
    {3, 70, 10, 0, 0, 1, {.text = &gWindowText560758}, &Sub_4B0F10},
    {3, 272, 11, 0, 0, 1, {.text = &gWindowText560760}, &Sub_4B0F10},
    {3, 130, 31, 0, 0, 1, {.text = &gWindowText560768}, &Sub_4B0F10},
    {3, 124, 48, 0, 0, 1, {.text = &gWindowText560770}, &Sub_4B0F10},
    {3, 124, 64, 0, 0, 1, {.text = &gWindowText560778}, &Sub_4B0F10},
    {3, 124, 80, 0, 0, 1, {.text = &gWindowText560780}, &Sub_4B0F10},
    {3, 124, 96, 0, 0, 1, {.text = &gWindowText560788}, &Sub_4B0F10},
    {3, 124, 112, 0, 0, 1, {.text = &gWindowText560790}, &Sub_4B0F10},
    {3, 46, 159, 0, 0, 1, {.text = &gWindowText560798}, &Sub_4B0F10},
    {3, 46, 175, 0, 0, 1, {.text = &gWindowText5607A0}, &Sub_4B0F10},
    {3, 46, 191, 0, 0, 1, {.text = &gWindowText5607A8}, &Sub_4B0F10},
    {3, 46, 207, 0, 0, 1, {.text = &gWindowText560830}, &Sub_4B0F10},
    {3, 124, 159, 0, 0, 1, {.text = &gWindowText5607B0}, &Sub_4B0F10},
    {3, 124, 175, 0, 0, 1, {.text = &gWindowText5607B8}, &Sub_4B0F10},
    {3, 124, 191, 0, 0, 1, {.text = &gWindowText5607C0}, &Sub_4B0F10},
    {3, 124, 207, 0, 0, 1, {.text = &gWindowText560838}, &Sub_4B0F10},
    {3, 212, 48, 0, 0, 1, {.text = &gWindowText5607C8}, &Sub_4B0F10},
    {3, 212, 64, 0, 0, 1, {.text = &gWindowText5607D0}, &Sub_4B0F10},
    {3, 212, 80, 0, 0, 1, {.text = &gWindowText5607D8}, &Sub_4B0F10},
    {3, 284, 48, 0, 0, 1, {.text = &gWindowText5607E0}, &Sub_4B0F10},
    {3, 284, 64, 0, 0, 1, {.text = &gWindowText5607E8}, &Sub_4B0F10},
    {3, 284, 80, 0, 0, 1, {.text = &gWindowText5607F0}, &Sub_4B0F10},
    {3, 212, 137, 0, 0, 1, {.text = &gWindowText5607F8}, &Sub_4B0F10},
    {3, 212, 153, 0, 0, 1, {.text = &gWindowText560800}, &Sub_4B0F10},
    {3, 284, 137, 0, 0, 1, {.text = &gWindowText560808}, &Sub_4B0F10},
    {3, 284, 153, 0, 0, 1, {.text = &gWindowText560810}, &Sub_4B0F10},
    {3, 212, 191, 0, 0, 1, {.text = &gWindowText560818}, &Sub_4B0F10},
    {3, 284, 191, 0, 0, 1, {.text = &gWindowText560820}, &Sub_4B0F10},
    {3, 284, 207, 0, 0, 1, {.text = &gWindowText560828}, &Sub_4B0F10},
    {1, 94, 48, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &Sub_475E00},
    {1, 94, 64, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &Sub_475E00},
    {1, 94, 80, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &Sub_475E00},
    {1, 94, 96, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &Sub_475E00},
    {1, 94, 112, 11, 11, 1, {.graphic = &gWindowGraphic5606E0}, &Sub_475E00},
    {1, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic560680}, &Sub_4C1FB0},
    {1, 151, 230, 38, 25, 1, {.graphic = &gWindowGraphic560698}, &Sub_475E00},
    {1, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic5606B0}, &Sub_4B0F10},
    {1, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic5606C8}, &Sub_475E00},
    {1, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic560740}, &Sub_475E00},
    {1, 15, 28, 0, 0, 1, {.graphic = &gWindowGraphic5606F8}, &Sub_4B0F10},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic560710}, &Sub_4B0F10},
    {1, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic560728}, &Sub_4B0F10},
    {0, 315, 0, 20, 130, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements560F50[37] {
    {5, 105, 31, 144, 12, 1, {.unusedValue = 0}, &Sub_477540},
    {1, 239, 104, 0, 14, 1, {.graphic = &gWindowGraphic560ED8}, &Sub_4B0F10},
    {4, 239, 104, 11, 72, 1, {.mouseFlags = &gWindowMouseFlagsD676F0}, &Sub_4782E0},
    {1, 239, 93, 11, 11, 1, {.graphic = &gWindowGraphic560EF0}, &Sub_477710},
    {1, 239, 175, 11, 11, 1, {.graphic = &gWindowGraphic560F08}, &Sub_477710},
    {0, 0, 0, 315, 261, 1, {.unusedValue = 0}, &Sub_4782A0},
    {3, 70, 10, 0, 0, 1, {.text = &gWindowText560F38}, &Sub_4B0F10},
    {3, 272, 11, 0, 0, 1, {.text = &gWindowText560F40}, &Sub_4B0F10},
    {3, 105, 31, 0, 0, 1, {.text = &gWindowText560F48}, &Sub_4B0F10},
    {3, 105, 64, 0, 0, 1, {.text = &gWindowText560F48}, &Sub_4B0F10},
    {3, 25, 94, 0, 0, 1, {.text = &gWindowText560F48}, &Sub_4B0F10},
    {3, 25, 110, 0, 0, 1, {.text = &gWindowText560F48}, &Sub_4B0F10},
    {3, 25, 126, 0, 0, 1, {.text = &gWindowText560F48}, &Sub_4B0F10},
    {3, 25, 142, 0, 0, 1, {.text = &gWindowText560F48}, &Sub_4B0F10},
    {3, 25, 158, 0, 0, 1, {.text = &gWindowText560F48}, &Sub_4B0F10},
    {3, 25, 174, 0, 0, 1, {.text = &gWindowText560F48}, &Sub_4B0F10},
    {1, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic560DA0}, &Sub_4C1FB0},
    {1, 151, 230, 38, 25, 1, {.graphic = &gWindowGraphic560DB8}, &Sub_477710},
    {1, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic560DD0}, &Sub_477710},
    {1, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic560DE8}, &Sub_4B0F10},
    {1, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic560F20}, &Sub_477710},
    {1, 260, 27, 49, 17, 1, {.graphic = &gWindowGraphic560E00}, &Sub_477710},
    {1, 260, 61, 49, 17, 1, {.graphic = &gWindowGraphic560E18}, &Sub_477710},
    {1, 260, 175, 49, 17, 1, {.graphic = &gWindowGraphic560E30}, &Sub_477710},
    {1, 220, 203, 18, 18, 1, {.graphic = &gWindowGraphic560E48}, &Sub_477710},
    {1, 199, 203, 18, 18, 1, {.graphic = &gWindowGraphic560E60}, &Sub_477710},
    {1, 23, 92, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &Sub_477600},
    {1, 23, 108, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &Sub_477600},
    {1, 23, 124, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &Sub_477600},
    {1, 23, 140, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &Sub_477600},
    {1, 23, 156, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &Sub_477600},
    {1, 23, 172, 216, 16, 1, {.graphic = &gWindowGraphic560E78}, &Sub_477600},
    {1, 15, 28, 0, 0, 1, {.graphic = &gWindowGraphic560E90}, &Sub_4B0F10},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic560EA8}, &Sub_4B0F10},
    {1, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic560EC0}, &Sub_4B0F10},
    {4, 0, 0, 374, 281, 1, {.mouseFlags = &gWindowMouseFlagsD6772C}, &Sub_477710},
    {0, 315, 0, 20, 130, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements561558[34] {
    {3, 70, 10, 0, 0, 1, {.text = &gWindowText561510}, &Sub_4B0F10},
    {3, 272, 11, 0, 0, 1, {.text = &gWindowText561518}, &Sub_4B0F10},
    {3, 140, 39, 0, 0, 1, {.text = &gWindowText561520}, &Sub_4B0F10},
    {3, 140, 59, 0, 0, 1, {.text = &gWindowText561528}, &Sub_4B0F10},
    {3, 140, 79, 0, 0, 1, {.text = &gWindowText561530}, &Sub_4B0F10},
    {5, 90, 105, 168, 14, 1, {.unusedValue = 0}, &Sub_4792D0},
    {5, 90, 129, 168, 14, 1, {.unusedValue = 0}, &Sub_4792D0},
    {5, 90, 153, 168, 14, 1, {.unusedValue = 0}, &Sub_4792D0},
    {5, 90, 177, 168, 54, 1, {.unusedValue = 0}, &Sub_4792D0},
    {3, 97, 108, 0, 0, 1, {.text = &gWindowText561540}, &Sub_4B0F10},
    {3, 97, 130, 0, 0, 1, {.text = &gWindowText561548}, &Sub_4B0F10},
    {3, 97, 152, 0, 0, 1, {.text = &gWindowText561550}, &Sub_4B0F10},
    {3, 97, 170, 0, 0, 1, {.text = &gWindowText561550}, &Sub_4B0F10},
    {3, 97, 184, 0, 0, 1, {.text = &gWindowText561550}, &Sub_4B0F10},
    {3, 97, 198, 0, 0, 1, {.text = &gWindowText561550}, &Sub_4B0F10},
    {3, 97, 212, 0, 0, 1, {.text = &gWindowText561550}, &Sub_4B0F10},
    {1, 296, 8, 12, 12, 1, {.graphic = &gWindowGraphic5613F0}, &Sub_4C1FB0},
    {1, 15, 29, 64, 72, 1, {.graphic = &gWindowGraphic5614F8}, &Sub_478330},
    {1, 15, 29, 0, 0, 1, {.graphic = &gWindowGraphic5613F0}, &Sub_4B0F10},
    {1, 151, 230, 38, 25, 1, {.graphic = &gWindowGraphic561408}, &Sub_478330},
    {1, 204, 231, 38, 25, 1, {.graphic = &gWindowGraphic561420}, &Sub_478330},
    {1, 258, 230, 38, 25, 1, {.graphic = &gWindowGraphic561438}, &Sub_478330},
    {1, 94, 230, 38, 25, 1, {.graphic = &gWindowGraphic561450}, &Sub_4B0F10},
    {1, 266, 105, 20, 20, 1, {.graphic = &gWindowGraphic5614B0}, &Sub_478330},
    {1, 266, 128, 20, 20, 1, {.graphic = &gWindowGraphic5614B0}, &Sub_478330},
    {1, 266, 151, 20, 20, 1, {.graphic = &gWindowGraphic5614B0}, &Sub_478330},
    {1, 73, 102, 20, 20, 1, {.graphic = &gWindowGraphic5614E0}, &Sub_478330},
    {1, 73, 124, 20, 20, 1, {.graphic = &gWindowGraphic5614E0}, &Sub_478330},
    {1, 73, 146, 20, 20, 1, {.graphic = &gWindowGraphic5614E0}, &Sub_478330},
    {1, 257, 32, 40, 30, 1, {.graphic = &gWindowGraphic5614C8}, &Sub_478330},
    {1, 15, 36, 0, 0, 1, {.graphic = &gWindowGraphic561468}, &Sub_4B0F10},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic561480}, &Sub_4B0F10},
    {1, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic561498}, &Sub_4B0F10},
    {0, 315, 0, 20, 130, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements561B78[30] {
    {1, 222, 9, 11, 11, 1, {.graphic = &gWindowGraphic561A40}, &Sub_4C1FB0},
    {3, 82, 12, 0, 0, 1, {.text = &gWindowText561B48}, &Sub_4B0F10},
    {3, 87, 12, 0, 0, 1, {.text = &gWindowText561B68}, &Sub_4B0F10},
    {3, 66, 32, 0, 0, 1, {.text = &gWindowText561B50}, &Sub_4B0F10},
    {1, 16, 28, 50, 15, 1, {.graphic = &gWindowGraphic561AD0}, &Sub_47B110},
    {3, 103, 31, 0, 0, 1, {.text = &gWindowText561B58}, &Sub_4B0F10},
    {1, 184, 28, 50, 15, 1, {.graphic = &gWindowGraphic561AE8}, &Sub_47B210},
    {3, 95, 31, 0, 0, 1, {.text = &gWindowText561B70}, &Sub_4B0F10},
    {1, 16, 28, 49, 17, 1, {.graphic = &gWindowGraphic561B00}, &Sub_47C950},
    {3, 190, 13, 0, 0, 1, {.text = &gWindowText561B60}, &Sub_4B0F10},
    {1, 223, 57, 0, 14, 1, {.graphic = &gWindowGraphic561A88}, &Sub_4B0F10},
    {4, 223, 57, 11, 140, 1, {.mouseFlags = &gWindowMouseFlagsD67780}, &Sub_4741A0},
    {1, 223, 47, 11, 11, 1, {.graphic = &gWindowGraphic561AA0}, &Sub_47B3D0},
    {1, 223, 196, 11, 11, 1, {.graphic = &gWindowGraphic561AB8}, &Sub_47B470},
    {1, 195, 28, 18, 18, 1, {.graphic = &gWindowGraphic561B18}, &Sub_47B520},
    {1, 215, 28, 18, 18, 1, {.graphic = &gWindowGraphic561B30}, &Sub_47B5D0},
    {0, 0, 0, 241, 258, 1, {.unusedValue = 0}, &Sub_47B680},
    {0, 14, 46, 207, 16, 1, {.unusedValue = 0}, &Sub_47B820},
    {0, 14, 62, 207, 16, 1, {.unusedValue = 0}, &Sub_47B820},
    {0, 14, 78, 207, 16, 1, {.unusedValue = 0}, &Sub_47B820},
    {0, 14, 94, 207, 16, 1, {.unusedValue = 0}, &Sub_47B820},
    {0, 14, 110, 207, 16, 1, {.unusedValue = 0}, &Sub_47B820},
    {0, 14, 126, 207, 16, 1, {.unusedValue = 0}, &Sub_47B820},
    {0, 14, 142, 207, 16, 1, {.unusedValue = 0}, &Sub_47B820},
    {0, 14, 158, 207, 16, 1, {.unusedValue = 0}, &Sub_47B820},
    {0, 14, 174, 207, 16, 1, {.unusedValue = 0}, &Sub_47B820},
    {0, 14, 190, 207, 16, 1, {.unusedValue = 0}, &Sub_47B820},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic561A58}, &Sub_4B0F10},
    {1, 12, 27, 0, 0, 1, {.graphic = &gWindowGraphic561A70}, &Sub_4B0F10},
    {0, 243, 5, 18, 79, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements5620E0[31] {
    {0, 0, 0, 330, 110, 1, {.unusedValue = 0}, &Sub_485200},
    {0, 0, 0, 330, 110, 1, {.unusedValue = 0}, &Sub_485350},
    {1, 543, 8, 11, 11, 1, {.graphic = &gWindowGraphic561FE0}, &Sub_485160},
    {2, 103, 225, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5620DC}, &Sub_4B0F10},
    {2, 103, 225, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5620D8}, &Sub_4B0F10},
    {2, 103, 225, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5620DC}, &Sub_4B0F10},
    {0, 20, 57, 150, 12, 1, {.unusedValue = 0}, &Sub_485030},
    {5, 20, 53, 0, 0, 1, {.unusedValue = 0}, &Sub_4B0F10},
    {1, 190, 54, 49, 17, 1, {.graphic = &gWindowGraphic5620B8}, &Sub_482270},
    {3, 20, 29, 0, 0, 1, {.text = &gWindowText5620D0}, &Sub_4B0F10},
    {3, 20, 43, 0, 0, 1, {.text = &gWindowText5620D0}, &Sub_4B0F10},
    {3, 20, 57, 0, 0, 1, {.text = &gWindowText5620D0}, &Sub_4B0F10},
    {3, 20, 71, 0, 0, 1, {.text = &gWindowText5620D0}, &Sub_4B0F10},
    {3, 20, 85, 0, 0, 1, {.text = &gWindowText5620D0}, &Sub_4B0F10},
    {1, 20, 29, 0, 0, 1, {.graphic = &gWindowGraphic562010}, &Sub_4B0F10},
    {0, 190, 75, 48, 48, 1, {.unusedValue = 0}, &Sub_483EB0},
    {0, 190, 125, 48, 48, 1, {.unusedValue = 0}, &Sub_483EB0},
    {0, 190, 175, 48, 48, 1, {.unusedValue = 0}, &Sub_483EB0},
    {0, 243, 75, 48, 48, 1, {.unusedValue = 0}, &Sub_483EB0},
    {0, 243, 125, 48, 48, 1, {.unusedValue = 0}, &Sub_483EB0},
    {0, 243, 175, 48, 48, 1, {.unusedValue = 0}, &Sub_483EB0},
    {1, 231, 175, 48, 48, 1, {.graphic = &gWindowGraphic5620A0}, &Sub_4847C0},
    {1, 190, 27, 0, 0, 1, {.graphic = &gWindowGraphic562088}, &Sub_484FB0},
    {1, 231, 45, 0, 0, 1, {.graphic = &gWindowGraphic562088}, &Sub_484FB0},
    {1, 188, 73, 0, 0, 1, {.graphic = &gWindowGraphic562028}, &Sub_4B0F10},
    {1, 21, 209, 49, 17, 1, {.graphic = &gWindowGraphic562040}, &Sub_482270},
    {1, 21, 209, 49, 17, 1, {.graphic = &gWindowGraphic562058}, &Sub_482FA0},
    {1, 126, 209, 49, 17, 1, {.graphic = &gWindowGraphic562070}, &Sub_4830B0},
    {0, 307, 28, 248, 204, 1, {.unusedValue = 0}, &Sub_483150},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic561FF8}, &Sub_4B0F10},
    {0, 566, 5, 18, 79, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements562620[15] {
    {1, 178, 9, 11, 11, 1, {.graphic = &gWindowGraphic562568}, &Sub_4C1FB0},
    {2, 107, 204, 0, 0, 0, {.animationImageNo = &gWindowAnimationImage56261C}, &Sub_4B0F10},
    {2, 107, 204, 0, 0, 0, {.animationImageNo = &gWindowAnimationImage562618}, &Sub_4B0F10},
    {2, 107, 204, 0, 0, 0, {.animationImageNo = &gWindowAnimationImage56261C}, &Sub_4B0F10},
    {3, 16, 27, 0, 0, 1, {.text = &gWindowText562610}, &Sub_4B0F10},
    {3, 16, 41, 0, 0, 1, {.text = &gWindowText562610}, &Sub_4B0F10},
    {3, 16, 55, 0, 0, 1, {.text = &gWindowText562610}, &Sub_4B0F10},
    {3, 16, 69, 0, 0, 1, {.text = &gWindowText562610}, &Sub_4B0F10},
    {1, 16, 43, 0, 0, 1, {.graphic = &gWindowGraphic562598}, &Sub_4B0F10},
    {1, 15, 40, 0, 0, 1, {.graphic = &gWindowGraphic5625F8}, &Sub_4B0F10},
    {0, 143, 29, 0, 0, 1, {.unusedValue = 0}, &Sub_486F20},
    {1, 29, 203, 49, 17, 1, {.graphic = &gWindowGraphic5625C8}, &Sub_486CF0},
    {1, 129, 203, 49, 17, 1, {.graphic = &gWindowGraphic5625E0}, &Sub_486E50},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic562580}, &Sub_4B0F10},
    {0, 201, 5, 18, 79, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements562930[14] {
    {1, 178, 9, 11, 11, 1, {.graphic = &gWindowGraphic5628A8}, &Sub_4C1FB0},
    {2, 107, 224, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56292C}, &Sub_4B0F10},
    {2, 107, 224, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage562928}, &Sub_4B0F10},
    {2, 107, 224, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage56292C}, &Sub_4B0F10},
    {3, 25, 28, 0, 0, 1, {.text = &gWindowText562920}, &Sub_4B0F10},
    {3, 25, 42, 0, 0, 1, {.text = &gWindowText562920}, &Sub_4B0F10},
    {3, 25, 56, 0, 0, 1, {.text = &gWindowText562920}, &Sub_4B0F10},
    {3, 25, 70, 0, 0, 1, {.text = &gWindowText562920}, &Sub_4B0F10},
    {1, 24, 28, 0, 0, 1, {.graphic = &gWindowGraphic5628D8}, &Sub_4B0F10},
    {1, 24, 55, 0, 0, 1, {.graphic = &gWindowGraphic5628D8}, &Sub_4B0F10},
    {1, 29, 203, 49, 17, 1, {.graphic = &gWindowGraphic5628F0}, &Sub_487D60},
    {1, 129, 203, 49, 17, 1, {.graphic = &gWindowGraphic562908}, &Sub_487E80},
    {1, 0, 0, 0, 0, 1, {.graphic = &gWindowGraphic5628C0}, &Sub_4B0F10},
    {0, 201, 5, 18, 79, 1, {.unusedValue = 0}, &Sub_474170},
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

WindowElementPrototype gWindowElements56C800[8] {
    {1, 302, 9, 11, 11, 1, {.graphic = &gWindowGraphic56C798}, &Sub_4C1FB0},
    {1, 258, 173, 57, 17, 1, {.graphic = &gWindowGraphic56C7E0}, &Sub_49BA10},
    {0, 15, 28, 300, 48, 1, {.unusedValue = 0}, &Sub_49BB00},
    {0, 15, 76, 300, 48, 1, {.unusedValue = 0}, &Sub_49BB00},
    {0, 15, 124, 300, 48, 1, {.unusedValue = 0}, &Sub_49BB00},
    {1, 0, 0, 320, 353, 1, {.graphic = &gWindowGraphic56C7B0}, &Sub_4B0F10},
    {1, 12, 25, 320, 353, 1, {.graphic = &gWindowGraphic56C7C8}, &Sub_4B0F10},
    {0, 320, 7, 18, 78, 1, {.unusedValue = 0}, &Sub_474170},
};

WindowElementPrototype gWindowElements56C9B8[2] {
    {3, 78, 40, 0, 0, 1, {.text = &gWindowText56C9A8}, &Sub_4B0F10},
    {3, 68, 40, 0, 0, 1, {.text = &gWindowText56C9B0}, &Sub_4B0F10},
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

WindowElementPrototype gWindowElements56D0B0[19] {
    {1, 270, 60, 0, 14, 1, {.graphic = &gWindowGraphic56D010}, &Sub_4B0F10},
    {4, 270, 60, 11, 85, 1,
        {.mouseFlags = reinterpret_cast<std::uint32_t*>(&gWindowGraphic56D010.imageNo)}, &Sub_4741A0},
    {1, 270, 50, 11, 11, 1, {.graphic = &gWindowGraphic56D028}, &Sub_49F260},
    {1, 270, 144, 11, 11, 1, {.graphic = &gWindowGraphic56D040}, &Sub_49F310},
    {1, 282, 47, 18, 18, 1, {.graphic = &gWindowGraphic56D070}, &Sub_49F3D0},
    {1, 282, 140, 18, 18, 1, {.graphic = &gWindowGraphic56D088}, &Sub_49F490},
    {0, 0, 0, 320, 192, 1, {.unusedValue = 0}, &Sub_49F570},
    {1, 135, 159, 49, 17, 1, {.graphic = &gWindowGraphic56D058}, &Sub_49F5C0},
    {3, 70, 25, 0, 0, 1, {.text = &gWindowText56D0A8}, &Sub_4B0F10},
    {3, 40, 55, 0, 0, 1, {.text = &gWindowText56D0A0}, &Sub_4B0F10},
    {3, 40, 75, 0, 0, 1, {.text = &gWindowText56D0A0}, &Sub_4B0F10},
    {3, 40, 95, 0, 0, 1, {.text = &gWindowText56D0A0}, &Sub_4B0F10},
    {3, 40, 115, 0, 0, 1, {.text = &gWindowText56D0A0}, &Sub_4B0F10},
    {3, 40, 135, 0, 0, 1, {.text = &gWindowText56D0A0}, &Sub_4B0F10},
    {0, 40, 55, 200, 13, 1, {.unusedValue = 0}, &Sub_49F680},
    {0, 40, 75, 200, 13, 1, {.unusedValue = 0}, &Sub_49F680},
    {0, 40, 95, 200, 13, 1, {.unusedValue = 0}, &Sub_49F680},
    {0, 40, 115, 200, 13, 1, {.unusedValue = 0}, &Sub_49F680},
    {0, 40, 135, 200, 13, 1, {.unusedValue = 0}, &Sub_49F680},
};

WindowElementPrototype gWindowElements56D3E8[34] {
    {1, 151, 200, 66, 17, 1, {.graphic = &gWindowGraphic56D310}, &Sub_49FA40},
    {1, 303, 200, 66, 17, 1, {.graphic = &gWindowGraphic56D328}, &Sub_49FB00},
    {3, 180, 25, 0, 0, 1, {.text = &gWindowText56D3D8}, &Sub_4B0F10},
    {3, 40, 45, 0, 0, 1, {.text = &gWindowText56D3E0}, &Sub_4B0F10},
    {1, 57, 72, 0, 0, 1, {.graphic = &gWindowGraphic56D340}, &Sub_4B0F10},
    {1, 57, 92, 0, 0, 1, {.graphic = &gWindowGraphic56D358}, &Sub_4B0F10},
    {1, 57, 112, 0, 0, 1, {.graphic = &gWindowGraphic56D370}, &Sub_4B0F10},
    {1, 57, 132, 0, 0, 1, {.graphic = &gWindowGraphic56D388}, &Sub_4B0F10},
    {1, 57, 152, 0, 0, 1, {.graphic = &gWindowGraphic56D3A0}, &Sub_4B0F10},
    {1, 57, 172, 0, 0, 1, {.graphic = &gWindowGraphic56D3B8}, &Sub_4B0F10},
    {3, 40, 70, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 90, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 110, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 130, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 150, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 170, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {0, 40, 70, 441, 13, 1, {.unusedValue = 0}, &Sub_49FBC0},
    {0, 40, 90, 441, 13, 1, {.unusedValue = 0}, &Sub_49FBC0},
    {0, 40, 110, 441, 13, 1, {.unusedValue = 0}, &Sub_49FBC0},
    {0, 40, 130, 441, 13, 1, {.unusedValue = 0}, &Sub_49FBC0},
    {0, 40, 150, 441, 13, 1, {.unusedValue = 0}, &Sub_49FBC0},
    {0, 40, 170, 441, 13, 1, {.unusedValue = 0}, &Sub_49FBC0},
    {3, 40, 70, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 90, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 110, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 130, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 150, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 170, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 70, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 90, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 110, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 130, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 150, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
    {3, 40, 170, 0, 0, 1, {.text = &gWindowText56D3D0}, &Sub_4B0F10},
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

WindowElementPrototype gWindowElements573D80[66] {
    {0, 0, 0, 640, 383, 1, {.unusedValue = 0}, &Sub_4BD100},
    {1, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic573AF0}, &Sub_4C1FB0},
    {1, 294, 333, 66, 17, 1, {.graphic = &gWindowGraphic573C88}, &Sub_4B9F40},
    {1, 467, 333, 66, 17, 1, {.graphic = &gWindowGraphic573CA0}, &Sub_4BA950},
    {1, 371, 333, 25, 19, 1, {.graphic = &gWindowGraphic573CA0}, &Sub_4BAA50},
    {3, 412, 44, 0, 14, 1, {.text = &gWindowText573CF8}, &Sub_4B0F10},
    {7, 124, 37, 0, 0, 1, {.number = &gWindowNumber573D20}, &Sub_4B0F10},
    {7, 255, 37, 0, 0, 1, {.number = &gWindowNumber573D28}, &Sub_4B0F10},
    {7, 389, 30, 0, 0, 1, {.number = &gWindowNumber573D18}, &Sub_4B0F10},
    {2, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage573D10}, &Sub_4B0F10},
    {1, 267, 85, 0, 14, 1, {.graphic = &gWindowGraphic573B50}, &Sub_4B0F10},
    {4, 267, 85, 11, 243, 1, {.mouseFlags = &gWindowMouseFlagsD678F0}, &Sub_4741A0},
    {1, 267, 75, 11, 11, 1, {.graphic = &gWindowGraphic573B68}, &Sub_4BAD10},
    {1, 267, 327, 11, 11, 1, {.graphic = &gWindowGraphic573B80}, &Sub_4BAE10},
    {1, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic573CB8}, &Sub_4BAF10},
    {1, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic573CD0}, &Sub_4BB010},
    {0, 15, 28, 274, 325, 1, {.unusedValue = 0}, &Sub_4BB130},
    {3, 154, 90, 0, 14, 1, {.text = &gWindowText573D00}, &Sub_4B0F10},
    {3, 154, 145, 0, 14, 1, {.text = &gWindowText573D00}, &Sub_4B0F10},
    {3, 154, 200, 0, 14, 1, {.text = &gWindowText573D00}, &Sub_4B0F10},
    {3, 154, 255, 0, 14, 1, {.text = &gWindowText573D00}, &Sub_4B0F10},
    {3, 154, 310, 0, 14, 1, {.text = &gWindowText573D00}, &Sub_4B0F10},
    {3, 154, 102, 0, 14, 1, {.text = &gWindowText573D00}, &Sub_4B0F10},
    {3, 154, 157, 0, 14, 1, {.text = &gWindowText573D00}, &Sub_4B0F10},
    {3, 154, 212, 0, 14, 1, {.text = &gWindowText573D00}, &Sub_4B0F10},
    {3, 154, 267, 0, 14, 1, {.text = &gWindowText573D00}, &Sub_4B0F10},
    {3, 154, 322, 0, 14, 1, {.text = &gWindowText573D00}, &Sub_4B0F10},
    {3, 154, 120, 0, 14, 1, {.text = &gWindowText573D08}, &Sub_4B0F10},
    {3, 154, 175, 0, 14, 1, {.text = &gWindowText573D08}, &Sub_4B0F10},
    {3, 154, 230, 0, 14, 1, {.text = &gWindowText573D08}, &Sub_4B0F10},
    {3, 154, 285, 0, 14, 1, {.text = &gWindowText573D08}, &Sub_4B0F10},
    {3, 154, 340, 0, 14, 1, {.text = &gWindowText573D08}, &Sub_4B0F10},
    {7, 129, 108, 0, 0, 1, {.number = &gWindowNumber573D30}, &Sub_4B0F10},
    {7, 129, 163, 0, 0, 1, {.number = &gWindowNumber573D38}, &Sub_4B0F10},
    {7, 129, 218, 0, 0, 1, {.number = &gWindowNumber573D40}, &Sub_4B0F10},
    {7, 129, 273, 0, 0, 1, {.number = &gWindowNumber573D48}, &Sub_4B0F10},
    {7, 129, 328, 0, 0, 1, {.number = &gWindowNumber573D50}, &Sub_4B0F10},
    {7, 132, 90, 0, 0, 1, {.number = &gWindowNumber573D58}, &Sub_4B0F10},
    {7, 132, 145, 0, 0, 1, {.number = &gWindowNumber573D60}, &Sub_4B0F10},
    {7, 132, 200, 0, 0, 1, {.number = &gWindowNumber573D68}, &Sub_4B0F10},
    {7, 132, 255, 0, 0, 1, {.number = &gWindowNumber573D70}, &Sub_4B0F10},
    {7, 132, 310, 0, 0, 1, {.number = &gWindowNumber573D78}, &Sub_4B0F10},
    {1, 80, 90, 11, 11, 1, {.graphic = &gWindowGraphic573B98}, &Sub_4BB1D0},
    {1, 80, 145, 11, 11, 1, {.graphic = &gWindowGraphic573BB0}, &Sub_4BB1D0},
    {1, 80, 200, 11, 11, 1, {.graphic = &gWindowGraphic573BC8}, &Sub_4BB1D0},
    {1, 80, 255, 11, 11, 1, {.graphic = &gWindowGraphic573BE0}, &Sub_4BB1D0},
    {1, 80, 310, 11, 11, 1, {.graphic = &gWindowGraphic573BF8}, &Sub_4BB1D0},
    {1, 95, 90, 11, 11, 1, {.graphic = &gWindowGraphic573C10}, &Sub_4BB6E0},
    {1, 95, 145, 11, 11, 1, {.graphic = &gWindowGraphic573C28}, &Sub_4BB6E0},
    {1, 95, 200, 11, 11, 1, {.graphic = &gWindowGraphic573C40}, &Sub_4BB6E0},
    {1, 95, 255, 11, 11, 1, {.graphic = &gWindowGraphic573C58}, &Sub_4BB6E0},
    {1, 95, 310, 11, 11, 1, {.graphic = &gWindowGraphic573C70}, &Sub_4BB6E0},
    {0, 27, 73, 48, 48, 1, {.unusedValue = 0}, &Sub_4BBED0},
    {0, 27, 128, 48, 48, 1, {.unusedValue = 0}, &Sub_4BBED0},
    {0, 27, 183, 48, 48, 1, {.unusedValue = 0}, &Sub_4BBED0},
    {0, 27, 238, 48, 48, 1, {.unusedValue = 0}, &Sub_4BBED0},
    {0, 27, 293, 48, 48, 1, {.unusedValue = 0}, &Sub_4BBED0},
    {1, 27, 70, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &Sub_4B0F10},
    {1, 27, 125, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &Sub_4B0F10},
    {1, 27, 180, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &Sub_4B0F10},
    {1, 27, 235, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &Sub_4B0F10},
    {1, 27, 290, 40, 11, 1, {.graphic = &gWindowGraphic573B38}, &Sub_4B0F10},
    {0, 290, 125, 252, 202, 1, {.unusedValue = 0}, &Sub_4BC660},
    {1, 15, 28, 274, 325, 1, {.graphic = &gWindowGraphic573B20}, &Sub_4BCEE0},
    {1, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic573B08}, &Sub_4B0F10},
    {0, 552, 5, 18, 80, 1, {.unusedValue = 0}, &Sub_474170},
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

// 原5748E8的107项；选择回调由模式刷新发布，不替换初始4B0F10。
WindowElementPrototype gWindowElements5748E8[107] {
    {0, 0, 0, 640, 383, 1, {.unusedValue = 0}, &Sub_4C0F20},
    {1, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic574668}, &Sub_4C1FB0},
    {1, 203, 326, 66, 17, 1, {.graphic = &gWindowGraphic5746F8}, &Sub_4BFBC0},
    {1, 467, 326, 66, 17, 1, {.graphic = &gWindowGraphic574710}, &Sub_4BFCB0},
    {3, 412, 44, 0, 14, 1, {.text = &gWindowText5747C0}, &Sub_4B0F10},
    {3, 78, 52, 0, 14, 1, {.text = &gWindowText5747D8}, &Sub_4B0F10},
    {7, 262, 37, 0, 14, 1, {.number = &gWindowNumber5747F0}, &Sub_4B0F10},
    {7, 389, 30, 0, 14, 1, {.number = &gWindowNumber5747E8}, &Sub_4B0F10},
    {2, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage5747E0}, &Sub_4B0F10},
    {1, 502, 146, 0, 14, 1, {.graphic = &gWindowGraphic5746B0}, &Sub_4B0F10},
    {4, 502, 146, 11, 137, 1, {.mouseFlags = &gWindowMouseFlagsD67900}, &Sub_4741A0},
    {1, 502, 136, 11, 11, 1, {.graphic = &gWindowGraphic5746C8}, &Sub_4BFF60},
    {1, 502, 282, 11, 11, 1, {.graphic = &gWindowGraphic5746E0}, &Sub_4C0160},
    {1, 472, 300, 18, 18, 1, {.graphic = &gWindowGraphic574788}, &Sub_4C0380},
    {1, 498, 300, 18, 18, 1, {.graphic = &gWindowGraphic5747A0}, &Sub_4C0590},
    {0, 286, 27, 273, 280, 1, {.unusedValue = 0}, &Sub_4C0800},
    {1, 499, 132, 11, 11, 1, {.graphic = &gWindowGraphic574728}, &Sub_4B0F10},
    {1, 269, 85, 0, 14, 1, {.graphic = &gWindowGraphic5746B0}, &Sub_4B0F10},
    {4, 269, 85, 11, 244, 1, {.mouseFlags = &gWindowMouseFlagsD67900}, &Sub_4741A0},
    {1, 269, 75, 11, 11, 1, {.graphic = &gWindowGraphic5746C8}, &Sub_4BFF60},
    {1, 269, 327, 11, 11, 1, {.graphic = &gWindowGraphic5746E0}, &Sub_4C0160},
    {1, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic574788}, &Sub_4C0380},
    {1, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic5747A0}, &Sub_4C0590},
    {0, 17, 28, 274, 325, 1, {.unusedValue = 0}, &Sub_4C0800},
    {3, 33, 87, 0, 0, 1, {.text = &gWindowText5747D0}, &Sub_4B0F10},
    {3, 33, 127, 0, 0, 1, {.text = &gWindowText5747D0}, &Sub_4B0F10},
    {3, 33, 147, 0, 0, 1, {.text = &gWindowText5747D0}, &Sub_4B0F10},
    {3, 33, 167, 0, 0, 1, {.text = &gWindowText5747D0}, &Sub_4B0F10},
    {3, 33, 187, 0, 0, 1, {.text = &gWindowText5747D0}, &Sub_4B0F10},
    {3, 33, 207, 0, 0, 1, {.text = &gWindowText5747D0}, &Sub_4B0F10},
    {3, 33, 227, 0, 0, 1, {.text = &gWindowText5747D0}, &Sub_4B0F10},
    {3, 33, 247, 0, 0, 1, {.text = &gWindowText5747D0}, &Sub_4B0F10},
    {3, 33, 267, 0, 0, 1, {.text = &gWindowText5747D0}, &Sub_4B0F10},
    {3, 33, 287, 0, 0, 1, {.text = &gWindowText5747D0}, &Sub_4B0F10},
    {3, 296, 135, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 296, 151, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 296, 167, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 296, 183, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 296, 199, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 296, 215, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 296, 231, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 296, 247, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 296, 263, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 296, 279, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 446, 135, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 446, 151, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 446, 167, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 446, 183, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 446, 199, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {7, 454, 136, 0, 0, 1, {.number = &gWindowNumber574848}, &Sub_4B0F10},
    {7, 454, 152, 0, 0, 1, {.number = &gWindowNumber574850}, &Sub_4B0F10},
    {7, 454, 168, 0, 0, 1, {.number = &gWindowNumber574858}, &Sub_4B0F10},
    {7, 454, 184, 0, 0, 1, {.number = &gWindowNumber574860}, &Sub_4B0F10},
    {7, 454, 200, 0, 0, 1, {.number = &gWindowNumber574868}, &Sub_4B0F10},
    {7, 454, 216, 0, 0, 1, {.number = &gWindowNumber574870}, &Sub_4B0F10},
    {7, 454, 232, 0, 0, 1, {.number = &gWindowNumber574878}, &Sub_4B0F10},
    {7, 454, 248, 0, 0, 1, {.number = &gWindowNumber574880}, &Sub_4B0F10},
    {7, 454, 264, 0, 0, 1, {.number = &gWindowNumber574888}, &Sub_4B0F10},
    {7, 454, 280, 0, 0, 1, {.number = &gWindowNumber574890}, &Sub_4B0F10},
    {7, 492, 136, 0, 0, 1, {.number = &gWindowNumber574898}, &Sub_4B0F10},
    {7, 492, 152, 0, 0, 1, {.number = &gWindowNumber5748A0}, &Sub_4B0F10},
    {7, 492, 168, 0, 0, 1, {.number = &gWindowNumber5748A8}, &Sub_4B0F10},
    {7, 492, 184, 0, 0, 1, {.number = &gWindowNumber5748B0}, &Sub_4B0F10},
    {7, 492, 200, 0, 0, 1, {.number = &gWindowNumber5748B8}, &Sub_4B0F10},
    {7, 492, 216, 0, 0, 1, {.number = &gWindowNumber5748C0}, &Sub_4B0F10},
    {7, 492, 232, 0, 0, 1, {.number = &gWindowNumber5748C8}, &Sub_4B0F10},
    {7, 492, 248, 0, 0, 1, {.number = &gWindowNumber5748D0}, &Sub_4B0F10},
    {7, 492, 264, 0, 0, 1, {.number = &gWindowNumber5748D8}, &Sub_4B0F10},
    {7, 492, 280, 0, 0, 1, {.number = &gWindowNumber5748E0}, &Sub_4B0F10},
    {1, 290, 133, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &Sub_4B0F10},
    {1, 290, 149, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &Sub_4B0F10},
    {1, 290, 165, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &Sub_4B0F10},
    {1, 290, 181, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &Sub_4B0F10},
    {1, 290, 197, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &Sub_4B0F10},
    {1, 290, 213, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &Sub_4B0F10},
    {1, 290, 229, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &Sub_4B0F10},
    {1, 290, 245, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &Sub_4B0F10},
    {1, 290, 261, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &Sub_4B0F10},
    {1, 290, 277, 207, 16, 1, {.graphic = &gWindowGraphic574740}, &Sub_4B0F10},
    {3, 41, 82, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 41, 137, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 41, 192, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 41, 247, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {3, 41, 302, 0, 0, 1, {.text = &gWindowText5747C8}, &Sub_4B0F10},
    {7, 239, 83, 0, 14, 1, {.number = &gWindowNumber5747F8}, &Sub_4B0F10},
    {7, 239, 138, 0, 14, 1, {.number = &gWindowNumber574800}, &Sub_4B0F10},
    {7, 239, 193, 0, 14, 1, {.number = &gWindowNumber574808}, &Sub_4B0F10},
    {7, 239, 248, 0, 14, 1, {.number = &gWindowNumber574810}, &Sub_4B0F10},
    {7, 239, 303, 0, 14, 1, {.number = &gWindowNumber574818}, &Sub_4B0F10},
    {7, 233, 108, 0, 14, 1, {.number = &gWindowNumber574820}, &Sub_4B0F10},
    {7, 233, 163, 0, 14, 1, {.number = &gWindowNumber574828}, &Sub_4B0F10},
    {7, 233, 218, 0, 14, 1, {.number = &gWindowNumber574830}, &Sub_4B0F10},
    {7, 233, 273, 0, 14, 1, {.number = &gWindowNumber574838}, &Sub_4B0F10},
    {7, 233, 328, 0, 14, 1, {.number = &gWindowNumber574840}, &Sub_4B0F10},
    {1, 37, 80, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &Sub_4B0F10},
    {1, 37, 135, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &Sub_4B0F10},
    {1, 37, 190, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &Sub_4B0F10},
    {1, 37, 245, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &Sub_4B0F10},
    {1, 37, 300, 207, 16, 1, {.graphic = &gWindowGraphic574770}, &Sub_4B0F10},
    {1, 27, 70, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &Sub_4B0F10},
    {1, 27, 125, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &Sub_4B0F10},
    {1, 27, 180, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &Sub_4B0F10},
    {1, 27, 235, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &Sub_4B0F10},
    {1, 27, 290, 241, 55, 1, {.graphic = &gWindowGraphic574758}, &Sub_4B0F10},
    {1, 17, 28, 274, 325, 1, {.graphic = &gWindowGraphic574698}, &Sub_4B0F10},
    {1, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic574680}, &Sub_4B0F10},
    {0, 552, 5, 18, 80, 1, {.unusedValue = 0}, &Sub_474170},
};

// 原575988的完整70项，显示缓冲与69更新/绘制保持同源。
WindowElementPrototype gWindowElements575988[70] {
    {0, 0, 0, 640, 383, 1, {.unusedValue = 0}, &Sub_4C2FA0},
    {1, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic5756F0}, &Sub_4C1FB0},
    {1, 294, 333, 66, 17, 1, {.graphic = &gWindowGraphic575888}, &Sub_4C21B0},
    {1, 467, 333, 66, 17, 1, {.graphic = &gWindowGraphic5758A0}, &Sub_4C2460},
    {1, 371, 333, 25, 19, 1, {.graphic = &gWindowGraphic5758A0}, &Sub_4C4680},
    {3, 425, 44, 0, 14, 1, {.text = &gWindowText5758F8}, &Sub_4B0F10},
    {7, 260, 37, 0, 0, 1, {.number = &gWindowNumber575930}, &Sub_4B0F10},
    {7, 400, 32, 0, 0, 1, {.number = &gWindowNumber575920}, &Sub_4B0F10},
    {2, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage575918}, &Sub_4B0F10},
    {1, 267, 85, 0, 14, 1, {.graphic = &gWindowGraphic575750}, &Sub_4B0F10},
    {4, 267, 85, 11, 243, 1, {.mouseFlags = &gWindowMouseFlagsD67910}, &Sub_4741A0},
    {1, 267, 75, 11, 11, 1, {.graphic = &gWindowGraphic575768}, &Sub_4C2530},
    {1, 267, 327, 11, 11, 1, {.graphic = &gWindowGraphic575780}, &Sub_4C25B0},
    {1, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic5758B8}, &Sub_4C2630},
    {1, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic5758D0}, &Sub_4C26B0},
    {0, 15, 28, 274, 325, 1, {.unusedValue = 0}, &Sub_4C2760},
    {3, 154, 90, 0, 14, 1, {.text = &gWindowText575900}, &Sub_4B0F10},
    {3, 154, 145, 0, 14, 1, {.text = &gWindowText575900}, &Sub_4B0F10},
    {3, 154, 200, 0, 14, 1, {.text = &gWindowText575900}, &Sub_4B0F10},
    {3, 154, 255, 0, 14, 1, {.text = &gWindowText575900}, &Sub_4B0F10},
    {3, 154, 310, 0, 14, 1, {.text = &gWindowText575900}, &Sub_4B0F10},
    {3, 154, 102, 0, 14, 1, {.text = &gWindowText575900}, &Sub_4B0F10},
    {3, 154, 157, 0, 14, 1, {.text = &gWindowText575900}, &Sub_4B0F10},
    {3, 154, 212, 0, 14, 1, {.text = &gWindowText575900}, &Sub_4B0F10},
    {3, 154, 267, 0, 14, 1, {.text = &gWindowText575900}, &Sub_4B0F10},
    {3, 154, 322, 0, 14, 1, {.text = &gWindowText575900}, &Sub_4B0F10},
    {3, 154, 120, 0, 14, 1, {.text = &gWindowText575908}, &Sub_4B0F10},
    {3, 154, 175, 0, 14, 1, {.text = &gWindowText575908}, &Sub_4B0F10},
    {3, 154, 230, 0, 14, 1, {.text = &gWindowText575908}, &Sub_4B0F10},
    {3, 154, 285, 0, 14, 1, {.text = &gWindowText575908}, &Sub_4B0F10},
    {3, 154, 340, 0, 14, 1, {.text = &gWindowText575908}, &Sub_4B0F10},
    {3, 83, 87, 0, 14, 1, {.text = &gWindowText575910}, &Sub_4B0F10},
    {3, 83, 142, 0, 14, 1, {.text = &gWindowText575910}, &Sub_4B0F10},
    {3, 83, 197, 0, 14, 1, {.text = &gWindowText575910}, &Sub_4B0F10},
    {3, 83, 252, 0, 14, 1, {.text = &gWindowText575910}, &Sub_4B0F10},
    {3, 83, 307, 0, 14, 1, {.text = &gWindowText575910}, &Sub_4B0F10},
    {7, 129, 108, 0, 0, 1, {.number = &gWindowNumber575938}, &Sub_4B0F10},
    {7, 129, 163, 0, 0, 1, {.number = &gWindowNumber575940}, &Sub_4B0F10},
    {7, 129, 218, 0, 0, 1, {.number = &gWindowNumber575948}, &Sub_4B0F10},
    {7, 129, 273, 0, 0, 1, {.number = &gWindowNumber575950}, &Sub_4B0F10},
    {7, 129, 328, 0, 0, 1, {.number = &gWindowNumber575958}, &Sub_4B0F10},
    {7, 132, 90, 0, 0, 1, {.number = &gWindowNumber575960}, &Sub_4B0F10},
    {7, 132, 145, 0, 0, 1, {.number = &gWindowNumber575968}, &Sub_4B0F10},
    {7, 132, 200, 0, 0, 1, {.number = &gWindowNumber575970}, &Sub_4B0F10},
    {7, 132, 255, 0, 0, 1, {.number = &gWindowNumber575978}, &Sub_4B0F10},
    {7, 132, 310, 0, 0, 1, {.number = &gWindowNumber575980}, &Sub_4B0F10},
    {1, 80, 90, 11, 11, 1, {.graphic = &gWindowGraphic575798}, &Sub_4C27B0},
    {1, 80, 145, 11, 11, 1, {.graphic = &gWindowGraphic5757B0}, &Sub_4C27B0},
    {1, 80, 200, 11, 11, 1, {.graphic = &gWindowGraphic5757C8}, &Sub_4C27B0},
    {1, 80, 255, 11, 11, 1, {.graphic = &gWindowGraphic5757E0}, &Sub_4C27B0},
    {1, 80, 310, 11, 11, 1, {.graphic = &gWindowGraphic5757F8}, &Sub_4C27B0},
    {1, 95, 90, 11, 11, 1, {.graphic = &gWindowGraphic575810}, &Sub_4C28C0},
    {1, 95, 145, 11, 11, 1, {.graphic = &gWindowGraphic575828}, &Sub_4C28C0},
    {1, 95, 200, 11, 11, 1, {.graphic = &gWindowGraphic575840}, &Sub_4C28C0},
    {1, 95, 255, 11, 11, 1, {.graphic = &gWindowGraphic575858}, &Sub_4C28C0},
    {1, 95, 310, 11, 11, 1, {.graphic = &gWindowGraphic575870}, &Sub_4C28C0},
    {0, 27, 73, 48, 48, 1, {.unusedValue = 0}, &Sub_4C29E0},
    {0, 27, 128, 48, 48, 1, {.unusedValue = 0}, &Sub_4C29E0},
    {0, 27, 183, 48, 48, 1, {.unusedValue = 0}, &Sub_4C29E0},
    {0, 27, 238, 48, 48, 1, {.unusedValue = 0}, &Sub_4C29E0},
    {0, 27, 293, 48, 48, 1, {.unusedValue = 0}, &Sub_4C29E0},
    {1, 27, 70, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &Sub_4B0F10},
    {1, 27, 125, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &Sub_4B0F10},
    {1, 27, 180, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &Sub_4B0F10},
    {1, 27, 235, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &Sub_4B0F10},
    {1, 27, 290, 40, 11, 1, {.graphic = &gWindowGraphic575738}, &Sub_4B0F10},
    {0, 290, 125, 252, 202, 1, {.unusedValue = 0}, &Sub_4C2B20},
    {1, 15, 28, 274, 325, 1, {.graphic = &gWindowGraphic575720}, &Sub_4B0F10},
    {1, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic575708}, &Sub_4B0F10},
    {0, 552, 5, 18, 80, 1, {.unusedValue = 0}, &Sub_474170},
};

// 原576588的完整70项，保留70背包回调的单格命中语义。
WindowElementPrototype gWindowElements576588[70] {
    {0, 0, 0, 640, 383, 1, {.unusedValue = 0}, &Sub_4C50C0},
    {1, 528, 9, 11, 11, 1, {.graphic = &gWindowGraphic5762F0}, &Sub_4C1FB0},
    {1, 294, 333, 66, 17, 1, {.graphic = &gWindowGraphic576488}, &Sub_4C43C0},
    {1, 467, 333, 66, 17, 1, {.graphic = &gWindowGraphic5764A0}, &Sub_4C2460},
    {1, 371, 333, 25, 19, 1, {.graphic = &gWindowGraphic5764A0}, &Sub_4C4680},
    {3, 425, 44, 0, 14, 1, {.text = &gWindowText5764F8}, &Sub_4B0F10},
    {7, 260, 37, 0, 0, 1, {.number = &gWindowNumber576530}, &Sub_4B0F10},
    {7, 400, 32, 0, 0, 1, {.number = &gWindowNumber576520}, &Sub_4B0F10},
    {2, 508, 105, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage576518}, &Sub_4B0F10},
    {1, 267, 85, 0, 14, 1, {.graphic = &gWindowGraphic576350}, &Sub_4B0F10},
    {4, 267, 85, 11, 243, 1, {.mouseFlags = &gWindowMouseFlagsD67920}, &Sub_4741A0},
    {1, 267, 75, 11, 11, 1, {.graphic = &gWindowGraphic576368}, &Sub_4C46C0},
    {1, 267, 327, 11, 11, 1, {.graphic = &gWindowGraphic576380}, &Sub_4C4740},
    {1, 226, 55, 18, 18, 1, {.graphic = &gWindowGraphic5764B8}, &Sub_4C47C0},
    {1, 246, 55, 18, 18, 1, {.graphic = &gWindowGraphic5764D0}, &Sub_4C4840},
    {0, 15, 28, 274, 325, 1, {.unusedValue = 0}, &Sub_4C48F0},
    {3, 154, 90, 0, 14, 1, {.text = &gWindowText576500}, &Sub_4B0F10},
    {3, 154, 145, 0, 14, 1, {.text = &gWindowText576500}, &Sub_4B0F10},
    {3, 154, 200, 0, 14, 1, {.text = &gWindowText576500}, &Sub_4B0F10},
    {3, 154, 255, 0, 14, 1, {.text = &gWindowText576500}, &Sub_4B0F10},
    {3, 154, 310, 0, 14, 1, {.text = &gWindowText576500}, &Sub_4B0F10},
    {3, 154, 102, 0, 14, 1, {.text = &gWindowText576500}, &Sub_4B0F10},
    {3, 154, 157, 0, 14, 1, {.text = &gWindowText576500}, &Sub_4B0F10},
    {3, 154, 212, 0, 14, 1, {.text = &gWindowText576500}, &Sub_4B0F10},
    {3, 154, 267, 0, 14, 1, {.text = &gWindowText576500}, &Sub_4B0F10},
    {3, 154, 322, 0, 14, 1, {.text = &gWindowText576500}, &Sub_4B0F10},
    {3, 154, 120, 0, 14, 1, {.text = &gWindowText576508}, &Sub_4B0F10},
    {3, 154, 175, 0, 14, 1, {.text = &gWindowText576508}, &Sub_4B0F10},
    {3, 154, 230, 0, 14, 1, {.text = &gWindowText576508}, &Sub_4B0F10},
    {3, 154, 285, 0, 14, 1, {.text = &gWindowText576508}, &Sub_4B0F10},
    {3, 154, 340, 0, 14, 1, {.text = &gWindowText576508}, &Sub_4B0F10},
    {3, 83, 87, 0, 14, 1, {.text = &gWindowText576510}, &Sub_4B0F10},
    {3, 83, 142, 0, 14, 1, {.text = &gWindowText576510}, &Sub_4B0F10},
    {3, 83, 197, 0, 14, 1, {.text = &gWindowText576510}, &Sub_4B0F10},
    {3, 83, 252, 0, 14, 1, {.text = &gWindowText576510}, &Sub_4B0F10},
    {3, 83, 307, 0, 14, 1, {.text = &gWindowText576510}, &Sub_4B0F10},
    {7, 129, 108, 0, 0, 1, {.number = &gWindowNumber576538}, &Sub_4B0F10},
    {7, 129, 163, 0, 0, 1, {.number = &gWindowNumber576540}, &Sub_4B0F10},
    {7, 129, 218, 0, 0, 1, {.number = &gWindowNumber576548}, &Sub_4B0F10},
    {7, 129, 273, 0, 0, 1, {.number = &gWindowNumber576550}, &Sub_4B0F10},
    {7, 129, 328, 0, 0, 1, {.number = &gWindowNumber576558}, &Sub_4B0F10},
    {7, 132, 90, 0, 0, 1, {.number = &gWindowNumber576560}, &Sub_4B0F10},
    {7, 132, 145, 0, 0, 1, {.number = &gWindowNumber576568}, &Sub_4B0F10},
    {7, 132, 200, 0, 0, 1, {.number = &gWindowNumber576570}, &Sub_4B0F10},
    {7, 132, 255, 0, 0, 1, {.number = &gWindowNumber576578}, &Sub_4B0F10},
    {7, 132, 310, 0, 0, 1, {.number = &gWindowNumber576580}, &Sub_4B0F10},
    {1, 80, 90, 11, 11, 1, {.graphic = &gWindowGraphic576398}, &Sub_4C4940},
    {1, 80, 145, 11, 11, 1, {.graphic = &gWindowGraphic5763B0}, &Sub_4C4940},
    {1, 80, 200, 11, 11, 1, {.graphic = &gWindowGraphic5763C8}, &Sub_4C4940},
    {1, 80, 255, 11, 11, 1, {.graphic = &gWindowGraphic5763E0}, &Sub_4C4940},
    {1, 80, 310, 11, 11, 1, {.graphic = &gWindowGraphic5763F8}, &Sub_4C4940},
    {1, 95, 90, 11, 11, 1, {.graphic = &gWindowGraphic576410}, &Sub_4C4A50},
    {1, 95, 145, 11, 11, 1, {.graphic = &gWindowGraphic576428}, &Sub_4C4A50},
    {1, 95, 200, 11, 11, 1, {.graphic = &gWindowGraphic576440}, &Sub_4C4A50},
    {1, 95, 255, 11, 11, 1, {.graphic = &gWindowGraphic576458}, &Sub_4C4A50},
    {1, 95, 310, 11, 11, 1, {.graphic = &gWindowGraphic576470}, &Sub_4C4A50},
    {0, 27, 73, 48, 48, 1, {.unusedValue = 0}, &Sub_4C4B70},
    {0, 27, 128, 48, 48, 1, {.unusedValue = 0}, &Sub_4C4B70},
    {0, 27, 183, 48, 48, 1, {.unusedValue = 0}, &Sub_4C4B70},
    {0, 27, 238, 48, 48, 1, {.unusedValue = 0}, &Sub_4C4B70},
    {0, 27, 293, 48, 48, 1, {.unusedValue = 0}, &Sub_4C4B70},
    {1, 27, 70, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &Sub_4B0F10},
    {1, 27, 125, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &Sub_4B0F10},
    {1, 27, 180, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &Sub_4B0F10},
    {1, 27, 235, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &Sub_4B0F10},
    {1, 27, 290, 40, 11, 1, {.graphic = &gWindowGraphic576338}, &Sub_4B0F10},
    {0, 290, 125, 252, 202, 1, {.unusedValue = 0}, &Sub_4C4CB0},
    {1, 15, 28, 274, 325, 1, {.graphic = &gWindowGraphic576320}, &Sub_4B0F10},
    {1, 0, 0, 40, 11, 1, {.graphic = &gWindowGraphic576308}, &Sub_4B0F10},
    {0, 552, 5, 18, 80, 1, {.unusedValue = 0}, &Sub_474170},
};

// 原576F90的十五项，三个分量按钮复用源图形但实例各自分配。
WindowElementPrototype gWindowElements576F90[15] {
    {1, 197, 206, 66, 17, 1, {.graphic = &gWindowGraphic576EF0}, &Sub_4C5C30},
    {1, 48, 19, 0, 0, 1, {.graphic = &gWindowGraphic576F20}, &Sub_4B0F10},
    {1, 48, 19, 0, 0, 1, {.graphic = &gWindowGraphic576F08}, &Sub_4B0F10},
    {1, 144, 26, 18, 18, 1, {.graphic = &gWindowGraphic576F38}, &Sub_4C5D00},
    {3, 194, 28, 0, 0, 1, {.text = &gWindowText576F70}, &Sub_4B0F10},
    {1, 260, 26, 18, 18, 1, {.graphic = &gWindowGraphic576F50}, &Sub_4C5D00},
    {1, 144, 47, 18, 18, 1, {.graphic = &gWindowGraphic576F38}, &Sub_4C5DE0},
    {3, 194, 49, 0, 0, 1, {.text = &gWindowText576F78}, &Sub_4B0F10},
    {1, 260, 47, 18, 18, 1, {.graphic = &gWindowGraphic576F50}, &Sub_4C5DE0},
    {1, 144, 68, 18, 18, 1, {.graphic = &gWindowGraphic576F38}, &Sub_4C5EB0},
    {3, 194, 70, 0, 0, 1, {.text = &gWindowText576F80}, &Sub_4B0F10},
    {1, 260, 68, 18, 18, 1, {.graphic = &gWindowGraphic576F50}, &Sub_4C5EB0},
    {2, 390, 120, 0, 0, 1, {.animationImageNo = &gWindowAnimationImage576F88}, &Sub_4B0F10},
    {3, 20, 147, 0, 0, 1, {.text = &gWindowText576F68}, &Sub_4B0F10},
    {3, 20, 163, 0, 0, 1, {.text = &gWindowText576F68}, &Sub_4B0F10},
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
