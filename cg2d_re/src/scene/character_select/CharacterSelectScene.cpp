#include "cg/core/LegacyNoOp.hpp"
#include "../login/LoginSavedState.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/text/Encoding.hpp"
#include <optional>
#include <system_error>
#include <string>

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include "cg/scene/login/CharacterCreationRequest.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/input/Input.hpp"
#include "cg/scene/login/LoginMessage.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "cg/scene/login/LoginScene.hpp"
#include "cg/render/Palette.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/AlbumState.hpp"
#include "../../world/MailState.hpp"
#include "../../world/PlayerState.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring> // 原CRT接口：0x0053BE40；原参数／返回合同由直接调用承接。
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。
#include <random>

#include "../../platform/MainLoopDependencies.hpp"
#include "cg/core/vm.h"

namespace cg::character_select::detail {

// 54E0BC起十二条创建页矩形/图号；状态-1仍可读取前一项height。
static constexpr std::array<CharacterSelectUiRecord, 12> kCreationUi54E0BC {{
    {{407, 130, 9, 13}, 245066, 245067, 245068},
    {{438, 130, 9, 13}, 245063, 245064, 245065},
    {{407, 148, 9, 13}, 245066, 245067, 245068},
    {{438, 148, 9, 13}, 245063, 245064, 245065},
    {{457, 150, 9, 13}, 245066, 245067, 245068},
    {{533, 150, 9, 13}, 245063, 245064, 245065},
    {{504, 199, 11, 11}, 243006, 243007, 243008},
    {{518, 199, 11, 11}, 243003, 243004, 243005},
    {{504, 307, 11, 11}, 243006, 243007, 243008},
    {{518, 307, 11, 11}, 243003, 243004, 243005},
    {{450, 372, 65, 17}, 243048, 243049, 243050},
    {{371, 372, 66, 17}, 245045, 245046, 245047},
}};

// 54DDE8为同表第14项，不另建状态或重复表。
static constexpr std::array<std::int32_t, 84> kCreationMotion54DDB0 {{ // 原常量参考：0x0054DDB0。
    100002, 100027, 100052, 100077, 100102, 100127, 100152,
    100252, 100277, 100302, 100327, 100352, 100377, 100402,
    105002, 105027, 105052, 105077, 105102, 105127, 105152,
    105252, 105277, 105302, 105327, 105352, 105377, 105402,
    106002, 106027, 106052, 106077, 106102, 106127, 106152,
    106252, 106277, 106302, 106327, 106352, 106377, 106402,
    106002, 106027, 106052, 106077, 106102, 106127, 106152,
    106252, 106277, 106302, 106327, 106352, 106377, 106402,
    105502, 105527, 104402, 104432, 104462, 104492, 120102,
    100900, 100900, 100900, 120126, 100900, 100900, 100900,
    105502, 105527, 104402, 104432, 104462, 104492, 120102,
    100900, 100900, 100900, 120126, 100900, 100900, 100900,
}};

static constexpr std::array<std::int32_t, 42> kCreationGraphic55C354 {{ // 原常量参考：0x0055C354。
    240000, 240100, 240200, 240300, 240400, 240500, 240600,
    240700, 240800, 240900, 241000, 241100, 241200, 241300,
    241400, 241500, 241600, 241700, 241800, 241900, 242000,
    242100, 242200, 242300, 242400, 242500, 242600, 242700,
    205000, 205100, 205200, 205300, 205400, 205500, 270000,
    205700, 205800, 205900, 270100, 206100, 206200, 206300,
}};

static constexpr std::array<std::array<std::int32_t, 2>, 14> kCreationSelectDraw54DF00 {{ // 原常量参考：0x0054DF00。
    {{111, 169}}, {{111, 243}}, {{111, 317}}, {{179, 133}}, {{179, 207}},
    {{179, 281}}, {{179, 355}}, {{315, 169}}, {{315, 243}}, {{315, 317}},
    {{247, 133}}, {{247, 207}}, {{247, 281}}, {{247, 355}},
}};
static constexpr std::array<std::array<std::int32_t, 2>, 14> kCreationSelectHit54DF70 {{ // 原常量参考：0x0054DF70。
    {{79, 133}}, {{79, 207}}, {{79, 281}}, {{147, 97}}, {{147, 171}},
    {{147, 245}}, {{147, 319}}, {{283, 133}}, {{283, 207}}, {{283, 281}},
    {{215, 97}}, {{215, 171}}, {{215, 245}}, {{215, 319}},
}};
static constexpr std::array<std::int32_t, 2> kCreationSelectSize54DFE0 {{64, 72}}; // 原常量参考：0x0054DFE0。
static constexpr std::array<CharacterSelectUiRecord, 2> kCreationSelectButtons54DFE8 {{ // 原常量参考：0x0054DFE8。
    {{256, 400, 65, 17}, 243048, 243049, 243050},
    {{105, 400, 66, 17}, 245045, 245046, 245047},
}};
// 原54E020起是两个矩形、两组三图号及三版本标签，不是七元UI记录。
static constexpr std::array<cg::ui::UiControlBounds, 2> kCreationVersionHit54E020 {{
    {222, 420, 18, 18}, {186, 420, 18, 18},
}};
static constexpr std::array<std::array<std::int32_t, 3>, 2> kCreationVersionImages54E040 {{ // 原常量参考：0x0054E040。
    {{243033, 243034, 243035}}, {{243030, 243031, 243032}},
}};
static constexpr std::array<std::int32_t, 3> kCreationVersionLabels54E058 {{ // 原常量参考：0x0054E058。
    245481, 245482, 245483,
}};
static constexpr std::array<cg::ui::UiControlBounds, 2> kCreationPreviewHit54E064 {{ // 原常量参考：0x0054E064。
    {455, 138, 125, 125}, {455, 291, 125, 125},
}};
static constexpr std::array<CharacterSelectUiRecord, 2> kCreationVariantButtons54E084 {{ // 原常量参考：0x0054E084。
    {{458, 268, 18, 18}, 243030, 243031, 243032},
    {{559, 268, 18, 18}, 243033, 243034, 243035},
}};
static constexpr std::array<std::int32_t, 28> kCreationGraphic55C3FC {{ // 原常量参考：0x0055C3FC。
    202800, 202900, 203000, 203100, 203200, 203300, 203400,
    203500, 203600, 203700, 203800, 203900, 204000, 204100,
    200000, 200200, 200800, 201000, 201200, 201400, 201600,
    200400, 200600, 201800, 202000, 202200, 202400, 202600,
}};
static constexpr std::array<std::int32_t, 28> kCreationGraphic55C46C {{ // 原常量参考：0x0055C46C。
    30000, 34000, 38000, 46000, 42000, 50000, 54000,
    70000, 74000, 78000, 82000, 86000, 90000, 94000,
    33100, 33100, 33100, 33100, 33100, 33100, 33100,
    33101, 33101, 33101, 33101, 33101, 33101, 33101,
}};

static constexpr std::array<const wchar_t*, 6> kCreationErrors {{
    L"没有输入姓名！", // 55C9A0。
    L"角色名称重复", // 55C918。
    L"请把持有点数分给能力！", // 55C968。
    L"请把持有点数分给属性！", // 55C950。
    L"不能使用含有空白文字的名字或系统字元！", // 55C928。
    L"人物名称不可含有非法字元！", // 55CE14。
}};

static std::uint8_t CreationMouseByte(const std::uint32_t& value)
{
    return *reinterpret_cast<const std::uint8_t*>(&value);
}

// 两处模式0/1映射均在外调后重取变体/样式/颜色，不能提前快照。
static std::int32_t MapCreationGraphic(const std::int32_t base, const std::int32_t mode)
{
    std::int32_t graphic = Sub_51BF00(base);
    if (mode == 0) {
        graphic = Sub_51BAC0(graphic);
    }
    const std::int32_t variant = static_cast<std::int8_t>(state::gValue9A5B00);
    const std::int32_t style = static_cast<std::int8_t>(state::gValue9A5668);
    const std::int32_t color = static_cast<std::int8_t>(state::gValue9A4B80);
    return graphic + color + (mode == 0
        ? 10 * (style + 100 * variant) : 5 * (style + 5 * variant));
}

// 原14个离散备份W32与9A4F38输入备份，仅4642C0/464380直接访问。
struct CreationSnapshot {
    std::int32_t value9A50A4; // 来源全局：0x009A50A4。
    std::int32_t value9A4B98; // 来源全局：0x009A4B98。
    std::int32_t value9A50A0; // 来源全局：0x009A50A0。
    std::int32_t value9A5670; // 来源全局：0x009A5670。
    std::int32_t value9A4E98; // 来源全局：0x009A4E98。
    std::int32_t value9A4E84; // 来源全局：0x009A4E84。
    std::int32_t value9A50E4; // 来源全局：0x009A50E4。
    std::int32_t value9A60E8; // 来源全局：0x009A60E8。
    std::int32_t value9A5094; // 来源全局：0x009A5094。
    std::int32_t value9A57E8; // 来源全局：0x009A57E8。
    std::int32_t value9A61F4; // 来源全局：0x009A61F4。
    std::int32_t value9A554C; // 来源全局：0x009A554C。
    std::int32_t value9A5664; // 来源全局：0x009A5664。
    std::int32_t value9A5954; // 来源全局：0x009A5954。
    input::InputContext input9A4F38; // 来源全局：0x009A4F38。
};

static CreationSnapshot gCreationSnapshot {};

// 原五个离散S32和9A6E88观察槽，仅45E7B0/45E7D0使用。
struct CreationRequestProgress {
    std::int32_t phase; // 9A50E0。
    std::int32_t columns; // 9A565C。
    std::int32_t rows; // 9A566C。
    std::int32_t x; // 9A4E90。
    std::int32_t y; // 9A50D8。
    cg::yobi::YobiAction* action; // 9A6E88，非拥有，完成时标死后清槽。
};

static CreationRequestProgress gCreationRequestProgress {};
static std::uint8_t gCreationRestore9A6EB8 = 0; // 原全局：0x009A6EB8。 原创建场景同次调用的恢复回边标志。
static std::uint8_t ReadCreationSnapshotByte(const std::int32_t& value)
{
    return *reinterpret_cast<const std::uint8_t*>(&value);
}

// 原函数：0x0050E770。
std::uint8_t Sub_50E770(std::uintptr_t, const std::int32_t pukVersion)
{
    const std::uint8_t version = static_cast<std::uint8_t>(pukVersion);
    state::gPukVersion = version;
    return version;
}

// 原函数：0x0045E7B0。
std::int32_t Sub_45E7B0()
{
    const std::int32_t selected = login::gSelectedCharacterIndex;
    gCreationRequestProgress.phase = 0;
    world::detail::Sub_5345A0(selected);
    return world::detail::Sub_534770();
}

// 原函数：0x0045E7D0。
std::int32_t Sub_45E7D0()
{
    const std::wstring_view text = L"创造角色中" /* 原常量参考：0x0055C9C0。 */;
    if (gCreationRequestProgress.phase == 0) {
        gCreationRequestProgress.phase = 1;
        const auto textWidth = static_cast<std::uint32_t>(cg::render::TextMeasure(text, 0));
        std::int32_t columns = std::bit_cast<std::int32_t>((textWidth + 63U) >> 6);
        gCreationRequestProgress.columns = columns;
        if (columns < 2) {
            columns = 2;
            gCreationRequestProgress.columns = 2;
        }
        const std::int32_t unscaledX = (10 - columns) * 64;
        const std::int32_t width = columns * 64;
        const std::int32_t x = unscaledX / 2;
        gCreationRequestProgress.rows = 2;
        gCreationRequestProgress.x = x;
        gCreationRequestProgress.y = 192;
        gCreationRequestProgress.action = cg::yobi::CreateModalAction_5044E0(
            x, 192, width, 96, 4, 1);
    }
    if (gCreationRequestProgress.phase == 1) {
        login::StartCharacterCreationRequest();
        ++gCreationRequestProgress.phase;
    }
    if (gCreationRequestProgress.phase == 2) {
        const std::int32_t status = login::PollCharacterCreationRequest();
        if (status == 1 || status < 0) {
            const std::int32_t result = status == 1 ? 1 : 2;
            cg::yobi::YobiAction* const action = gCreationRequestProgress.action;
            if (action != nullptr) {
                cg::yobi::MarkDeath(action);
                gCreationRequestProgress.action = nullptr;
            }
            return result;
        }
    }
    cg::yobi::YobiAction* const action = gCreationRequestProgress.action;
    if (action != nullptr && cg::yobi::IsYobiActionReady(action, 1)) {
        const std::int32_t rows = gCreationRequestProgress.rows;
        const std::int32_t unscaledY = 48 * rows - 16;
        const std::int32_t top = gCreationRequestProgress.y;
        const std::int32_t y = unscaledY / 2 + top;
        const auto textWidth = static_cast<std::uint32_t>(cg::render::TextMeasure(text, 0));
        const std::int32_t columns = gCreationRequestProgress.columns;
        const std::int32_t unscaledX = 64 * columns - static_cast<std::int32_t>(textWidth);
        const std::int32_t left = gCreationRequestProgress.x;
        const std::int32_t x = unscaledX / 2 + left;
        cg::render::DrawText3_456560(
            static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), 2, 0, text, 0);
    }
    return 0;
}

// 原函数：0x004626A0。
void ProcessCreationScene_4626A0(const std::int32_t entryGate)
{
    std::int32_t sceneState = world::gCurrentSceneState;
    if (sceneState == 0) {
        Sub_462870();
        sceneState = world::gCurrentSceneState;
    }
    std::int32_t gate = entryGate;
    bool selectCharacter = false;
    if (sceneState == 1) {
        Sub_462880(1);
        world::gCurrentSceneState = 3;
        selectCharacter = true;
    } else {
        if (sceneState == 2) {
            Sub_462880(0);
            sceneState = world::gCurrentSceneState;
            ++sceneState;
            world::gCurrentSceneState = sceneState;
        }
        selectCharacter = sceneState == 3;
    }
    if (selectCharacter) {
        gate = Sub_462930();
        if (gate == 1) {
            sceneState = 10;
            world::gCurrentSceneState = sceneState;
        } else {
            if (gate == 2) {
                world::SetCurrentSceneTypeAndState(3, 10);
            }
            sceneState = world::gCurrentSceneState;
        }
    }

    // 原462726回边只重做后半分派，必须保留当前局部状态和ESI门禁。
    for (;;) {
        bool editCharacter = false;
        bool skipRemainingStages = false;
        if (sceneState == 10 && gate != 1) {
            skipRemainingStages = true;
        } else if (sceneState == 10 || sceneState == 11) {
            Sub_463100();
            world::gCurrentSceneState = 12;
            editCharacter = true;
        } else if (sceneState == 12) {
            editCharacter = true;
        }
        if (editCharacter) {
            if (gCreationRestore9A6EB8 == 1) {
                Sub_464380();
                gCreationRestore9A6EB8 = 0;
            }
            gate = Sub_4631D0();
            if (gate == 1) {
                const std::int32_t selected = login::gSelectedCharacterIndex;
                world::gCurrentSceneState = 30;
                cg::login::Sub_51B3B0(static_cast<std::uint32_t>(selected));
                const std::int32_t unusedSelected = login::gSelectedCharacterIndex;
                static_cast<void>(unusedSelected);
                cg::core::NullSub_509080();
                cg::login::detail::Sub_520200();
                input::SetContext_44B330(nullptr);
                sceneState = world::gCurrentSceneState;
            } else if (gate == 2) {
                sceneState = gate;
                world::gCurrentSceneState = sceneState;
                skipRemainingStages = true;
            } else {
                sceneState = world::gCurrentSceneState;
            }
        }
        if (!skipRemainingStages) {
            bool processRequest = false;
            if (sceneState == 30) {
                Sub_45E7B0();
                world::gCurrentSceneState = 31;
                processRequest = true;
            } else if (sceneState == 31) {
                processRequest = true;
            }
            if (processRequest) {
                gate = Sub_45E7D0();
                if (gate == 1) {
                    state::gPendingCharacterCount =
                        static_cast<std::int16_t>(state::gPendingCharacterCount + 1);
                    world::SetCurrentSceneType(3);
                    state::gCharacterEntryState = 1;
                    sceneState = world::gCurrentSceneState;
                } else if (gate == 2) {
                    world::gCurrentSceneState = 100;
                    sceneState = 100;
                } else {
                    sceneState = world::gCurrentSceneState;
                }
            }
            if (sceneState == 100) {
                login::ResetLoginMessage();
                sceneState = world::gCurrentSceneState;
                ++sceneState;
                world::gCurrentSceneState = sceneState;
            }
            if (sceneState == 101) {
                if (login::UpdateLoginMessage(L"已经有相同名字的角色存在了！" /* 原常量参考：0x0055C980。 */) != 0) {
                    sceneState = 10;
                    gCreationRestore9A6EB8 = 1;
                    world::gCurrentSceneState = sceneState;
                    gate = 1;
                    continue;
                }
                sceneState = world::gCurrentSceneState;
            }
        }
        if (gCreationRestore9A6EB8 != 1) {
            return;
        }
    }
}

// 原函数：0x00462870。
void Sub_462870()
{
    cg::yobi::YobiClearUp_404D30();
    ++world::gCurrentSceneState;
}

// 原函数：0x00462880。
void Sub_462880(const std::uint8_t resetSelection)
{
    if (resetSelection != 0) {
        const std::uint8_t pukVersion = state::gPukVersion;
        state::gValue9A57E0 = 0;
        if (pukVersion >= 5) {
            state::gValue9A57E0 = 1;
        }
        state::gValue9A4CFC = 0;
        state::gValue9A5B00 = 0;
    }

    cg::yobi::YobiAction* const firstAction =
        cg::yobi::NewYobiAction_404AE0(2, 0);
    state::gCreateActions[0] = firstAction;
    if (firstAction != nullptr) {
        cg::yobi::SetYobiActionField30(firstAction, 517);
        cg::yobi::SetYobiActionField34(state::gCreateActions[0], 243);
        cg::yobi::SetYobiActionField2B8(state::gCreateActions[0], 5);
    }

    cg::yobi::YobiAction* const secondAction =
        cg::yobi::NewYobiAction_404AE0(2, 0);
    state::gCreateActions[1] = secondAction;
    if (secondAction != nullptr) {
        cg::yobi::SetYobiActionField30(secondAction, 517);
        cg::yobi::SetYobiActionField34(state::gCreateActions[1], 396);
        cg::yobi::SetYobiActionField2B8(state::gCreateActions[1], 5);
    }
    input::SetContext_44B330(nullptr);
}

// 原函数：0x00462930。
std::int32_t Sub_462930()
{
    std::uint8_t selected = state::gValue9A4CFC;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x3FFFFFFF;
    descriptor.color = color;
    std::int32_t result = 0;
    std::int8_t confirmState = 0;
    std::int8_t cancelState = 0;
    std::int8_t versionPlus = 0;
    std::int8_t versionMinus = 0;
    std::int8_t variantMinus = 0;
    std::int8_t variantPlus = 0;
    const auto hover = []() -> std::int8_t {
        return (CreationMouseByte(platform::gLegacyMouseState.heldButtons) & 1) != 0 ? 1 : 2;
    };

    for (std::int32_t index = 0; index < 14; ++index) {
        const std::int32_t version = static_cast<std::int8_t>(state::gValue9A57E0);
        const std::int32_t image = kCreationGraphic55C354[14 * version + index];
        if (image >= 206000 && image <= 206300) {
            if (static_cast<std::int8_t>(selected) >= 7 && selected != 10) {
                selected = 0;
                state::gValue9A4CFC = 0;
            }
            continue;
        }
        const auto& position = kCreationSelectHit54DF70[index];
        const std::int32_t top = position[1];
        const std::int32_t height = kCreationSelectSize54DFE0[1];
        const std::int32_t left = position[0];
        const std::int32_t width = kCreationSelectSize54DFE0[0];
        if (cg::ui::HitTestMouseRect_5049A0(left, top, left + width, top + height, -1) != 0
            && (CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            state::gValue9A4CFC = static_cast<std::uint8_t>(index);
            state::gValue9A5B00 = 0;
            break;
        }
        selected = state::gValue9A4CFC;
    }
    if (Sub_464290(&kCreationSelectButtons54DFE8[0].bounds) != 0) {
        confirmState = hover();
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            audio::PlaySe_52C450(53, 320);
            result = 1;
        }
    }
    if (Sub_464290(&kCreationSelectButtons54DFE8[1].bounds) != 0) {
        cancelState = hover();
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            audio::PlaySe_52C450(54, 320);
            result = 2;
        }
    }
    if (state::gPukVersion >= 5) {
        if (Sub_464290(&kCreationVersionHit54E020[0]) != 0) {
            versionPlus = hover();
            if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
                const std::uint8_t puk = state::gPukVersion;
                const std::int32_t version = static_cast<std::int8_t>(state::gValue9A57E0);
                const std::int32_t limit = puk >= 8 ? 2 : 1;
                if (version < limit) {
                    ++state::gValue9A57E0;
                } else {
                    state::gValue9A57E0 = static_cast<std::uint8_t>(limit);
                }
            }
        }
        if (Sub_464290(&kCreationVersionHit54E020[1]) != 0) {
            versionMinus = hover();
            if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
                if (static_cast<std::int8_t>(state::gValue9A57E0) > 0) {
                    --state::gValue9A57E0;
                } else {
                    state::gValue9A57E0 = 0;
                }
            }
        }
    }
    if (Sub_464290(&kCreationVariantButtons54E084[0].bounds) != 0) {
        variantMinus = hover();
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            const auto next = static_cast<std::uint8_t>(state::gValue9A5B00 - 1);
            state::gValue9A5B00 = next;
            if (static_cast<std::int8_t>(next) < 0) {
                state::gValue9A5B00 = 3;
            }
        }
    }
    if (Sub_464290(&kCreationVariantButtons54E084[1].bounds) != 0) {
        variantPlus = hover();
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            const auto next = static_cast<std::uint8_t>(state::gValue9A5B00 + 1);
            state::gValue9A5B00 = next;
            if (static_cast<std::int8_t>(next) > 3) {
                state::gValue9A5B00 = 0;
            }
        }
    }

    if (Sub_464290(&kCreationPreviewHit54E064[0]) != 0
        || Sub_464290(&kCreationPreviewHit54E064[1]) != 0) {
        cg::yobi::YobiAction* first;
        cg::yobi::YobiAction* second;
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            first = state::gCreateActions[0];
            if (first != nullptr) {
                cg::yobi::SetYobiActionField2B8(
                    first, cg::yobi::GetYobiActionField2B8(first) - 1);
                first = state::gCreateActions[0];
                if (cg::yobi::GetYobiActionField2B8(first) < 0) {
                    cg::yobi::SetYobiActionField2B8(first, 7);
                    first = state::gCreateActions[0];
                }
            }
            second = state::gCreateActions[1];
            if (second != nullptr) {
                cg::yobi::SetYobiActionField2B8(
                    second, cg::yobi::GetYobiActionField2B8(second) - 1);
                second = state::gCreateActions[1];
                if (cg::yobi::GetYobiActionField2B8(second) < 0) {
                    cg::yobi::SetYobiActionField2B8(second, 7);
                    second = state::gCreateActions[1];
                }
                first = state::gCreateActions[0];
            }
        } else {
            second = state::gCreateActions[1];
            first = state::gCreateActions[0];
        }
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 2) != 0) {
            if (first != nullptr) {
                cg::yobi::SetYobiActionField2B8(
                    first, cg::yobi::GetYobiActionField2B8(first) + 1);
                cg::yobi::YobiAction* const current = state::gCreateActions[0];
                if (cg::yobi::GetYobiActionField2B8(current) > 7) {
                    cg::yobi::SetYobiActionField2B8(current, 0);
                }
                second = state::gCreateActions[1];
            }
            if (second != nullptr) {
                cg::yobi::SetYobiActionField2B8(
                    second, cg::yobi::GetYobiActionField2B8(second) + 1);
                second = state::gCreateActions[1];
                if (cg::yobi::GetYobiActionField2B8(second) > 7) {
                    cg::yobi::SetYobiActionField2B8(second, 0);
                }
            }
        }
    }

    const auto& selectedPosition = kCreationSelectDraw54DF00[static_cast<std::int8_t>(state::gValue9A4CFC)];
    cg::render::DrawGraphic(selectedPosition[0], selectedPosition[1], 10, 245042, 0, nullptr);
    if (render::HasLegacy3DDevice_44B610() != 0) {
        const auto& position = kCreationSelectDraw54DF00[static_cast<std::int8_t>(state::gValue9A4CFC)];
        cg::render::DrawGraphic(position[0], position[1], 10, 245043, 0, &descriptor);
    }
    cg::render::DrawGraphic(
        138, 409, 10, std::bit_cast<std::array<std::int32_t, 7>>(kCreationSelectButtons54DFE8[1])[4 + cancelState], 0, nullptr);
    cg::render::DrawGraphic(
        289, 409, 10, std::bit_cast<std::array<std::int32_t, 7>>(kCreationSelectButtons54DFE8[0])[4 + confirmState], 0, nullptr);
    if (state::gPukVersion >= 5) {
        cg::render::DrawGraphic(230, 429, 10, kCreationVersionImages54E040[0][versionPlus], 0, nullptr);
        cg::render::DrawGraphic(194, 429, 10, kCreationVersionImages54E040[1][versionMinus], 0, nullptr);
    }
    cg::render::DrawGraphic(
        203, 412, 10, kCreationVersionLabels54E058[static_cast<std::int8_t>(state::gValue9A57E0)], 0, nullptr);
    const std::uint8_t labelPuk = state::gPukVersion;
    cg::render::DrawGraphic(
        221, 412, 10, labelPuk < 5 ? 245481 : (labelPuk < 8 ? 245482 : 245483), 0, nullptr);

    const std::uint8_t listPuk = state::gPukVersion;
    const std::int32_t* const graphicTable = listPuk >= 7 ? kCreationGraphic55C354.data()
        : (listPuk >= 5 ? kCreationGraphic55C3FC.data() : kCreationGraphic55C46C.data());
    for (std::int32_t index = 0; index < 14; ++index) {
        const std::int32_t version = static_cast<std::int8_t>(state::gValue9A57E0);
        const auto& position = kCreationSelectDraw54DF00[index];
        cg::render::DrawGraphic(
            position[0], position[1], 10, graphicTable[14 * version + index], 0, nullptr);
    }
    cg::render::DrawGraphic(214, 254, 10, 245041, 0, nullptr);
    cg::render::DrawGraphic(
        467, 277, 10, std::bit_cast<std::array<std::int32_t, 7>>(kCreationVariantButtons54E084[0])[4 + variantMinus], 0, nullptr);
    cg::render::DrawGraphic(
        568, 277, 10, std::bit_cast<std::array<std::int32_t, 7>>(kCreationVariantButtons54E084[1])[4 + variantPlus], 0, nullptr);

    cg::yobi::YobiAction* const second = state::gCreateActions[1];
    if (second != nullptr) {
        cg::yobi::SetYobiActionField15(second, 2);
        const std::int32_t version = static_cast<std::int8_t>(state::gValue9A57E0);
        const std::int32_t character = static_cast<std::int8_t>(state::gValue9A4CFC);
        const std::int32_t variant = static_cast<std::int8_t>(state::gValue9A5B00);
        const std::int32_t motion = kCreationMotion54DDB0[28 * version + character + 14] + 6 * variant;
        cg::yobi::SetYobiActionField2A8(state::gCreateActions[1], motion);
        cg::yobi::Sub_517880(state::gCreateActions[1], 100, 1, 0);
        if (state::gPukVersion >= 5 && state::gValue9A57E0 == 0) {
            const std::int32_t graphic = cg::yobi::GetYobiActionField20(state::gCreateActions[1]);
            render::DrawGraphic2_521DD0(517, 396, 10, graphic, 0, 0, nullptr);
        }
    }
    cg::yobi::YobiAction* const first = state::gCreateActions[0];
    if (first != nullptr) {
        cg::yobi::SetYobiActionField15(first, 2);
        const std::uint8_t puk = state::gPukVersion;
        const std::int32_t version = static_cast<std::int8_t>(state::gValue9A57E0);
        const std::int32_t character = static_cast<std::int8_t>(state::gValue9A4CFC);
        std::int32_t motion;
        if (puk < 5) {
            const std::int32_t graphic = kCreationGraphic55C46C[14 * version + character];
            if (graphic == 33100 || graphic == 33101) {
                const std::int32_t variant = static_cast<std::int8_t>(state::gValue9A5B00);
                motion = (graphic == 33100 ? 100900 : 100901) + 6 * variant;
            } else {
                const std::int32_t variant = static_cast<std::int8_t>(state::gValue9A5B00);
                motion = kCreationMotion54DDB0[28 * version + character] + 6 * variant;
            }
        } else {
            const std::int32_t variant = static_cast<std::int8_t>(state::gValue9A5B00);
            motion = kCreationMotion54DDB0[28 * version + character] + 6 * variant;
        }
        cg::yobi::SetYobiActionField2A8(state::gCreateActions[0], motion);
        if (cg::yobi::Sub_517880(state::gCreateActions[0], 100, 1, 0) != 0) {
            cg::yobi::AdvanceCharacterPreviewAnimation(state::gCreateActions[0]);
            cg::yobi::YobiAction* const currentSecond = state::gCreateActions[1];
            if (currentSecond != nullptr) {
                cg::yobi::AdvanceCharacterPreviewAnimation(currentSecond);
            }
        }
    }
    cg::render::DrawGraphic(518, 275, 1, 245044, 0, nullptr);
    cg::render::DrawGraphic(320, 240, 1, 245040, 0, nullptr);
    return result;
}

// 原函数：0x00463100。
std::uint32_t Sub_463100()
{
    cg::yobi::YobiAction* const first = state::gCreateActions[0];
    if (first != nullptr) {
        cg::yobi::MarkDeath(first);
    }
    cg::yobi::YobiAction* const second = state::gCreateActions[1];
    if (second != nullptr) {
        cg::yobi::MarkDeath(second);
    }
    state::gValue9A5E78 = 0;
    state::gValue9A5FA4 = 0;
    state::gValue9A5FA0 = 0;
    input::InitializeContext_449CF0(
        &state::gCreationInput9A51E8, 352, 108, 0, 13, 131, {}, 1, 16, 0, 0);
    input::SetContext_44B330(&state::gCreationInput9A51E8);
    state::gValue9A5FD4 = 0;
    state::gValue9A50D0 = 0;
    state::gValue9A5FD8 = 0;
    const std::uint32_t version = state::gPukVersion;
    state::gValue9A5668 = 0;
    state::gValue9A4B80 = 0;
    state::gValue9A4D04 = 30;
    state::gValue9A50C8 = 10;
    if (version >= 7) {
        state::gValue9A4B88 = 2;
        state::gValue9A5AFC = 1;
    } else {
        state::gValue9A5AFC = 1;
        state::gValue9A4B88 = version >= 5 ? 1 : 0;
    }
    return version;
}

// 原函数：0x004631D0。
std::int32_t Sub_4631D0()
{
    auto* const attributeBytes = reinterpret_cast<std::uint8_t*>(&state::gValue9A5FD4);
    const std::array<std::uint8_t*, 5> attributes {{
        attributeBytes, attributeBytes + 1, attributeBytes + 2, attributeBytes + 3,
        &state::gValue9A5FD8,
    }};
    auto* const elements = reinterpret_cast<std::uint8_t*>(&state::gValue9A50D0);
    std::array<std::int8_t, 6> arrows {};
    std::array<std::int8_t, 5> attributeMinus {};
    std::array<std::int8_t, 5> attributePlus {};
    std::array<std::int8_t, 4> elementMinus {};
    std::array<std::int8_t, 4> elementPlus {};
    std::int8_t confirmState = 0;
    std::int8_t cancelState = 0;
    std::int32_t result = 0;
    std::wstring buffer;

    std::int32_t total = static_cast<std::int8_t>(*attributes[3]);
    total += static_cast<std::int8_t>(*attributes[2]);
    total += static_cast<std::int8_t>(*attributes[4]);
    total += static_cast<std::int8_t>(*attributes[1]);
    total += static_cast<std::int8_t>(*attributes[0]);
    total += static_cast<std::int8_t>(state::gValue9A4D04);
    const std::int32_t attributeLimit = total >> 1;
    const std::uint8_t puk = state::gPukVersion;
    const std::uint8_t maxVersion = puk >= 7 ? 2 : (puk >= 5 ? 1 : 0);
    const bool showVersionControls = maxVersion != state::gValue9A57E0;

    const auto hover = []() -> std::int8_t {
        return (CreationMouseByte(platform::gLegacyMouseState.heldButtons) & 1) != 0 ? 1 : 2;
    };
    const auto hitRow = [](const std::size_t record, const std::int32_t top) {
        const auto& bounds = kCreationUi54E0BC[record].bounds;
        return cg::ui::HitTestMouseRect_5049A0(
            bounds.left, top, bounds.left + bounds.width, top + bounds.height, -1);
    };

    if (state::gValue9A5AFC == 1) {
        for (std::size_t index = 0; index < 4; ++index) {
            if (Sub_464290(&kCreationUi54E0BC[index].bounds) != 0) {
                arrows[index] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    auto& value = index < 2 ? state::gValue9A5668 : state::gValue9A4B80;
                    if ((index & 1) == 0) {
                        const auto next = static_cast<std::uint8_t>(value - 1);
                        value = next;
                        if (static_cast<std::int8_t>(next) < 0) {
                            value = 4;
                        }
                    } else {
                        const auto next = static_cast<std::uint8_t>(value + 1);
                        value = next;
                        if (static_cast<std::int8_t>(next) > 4) {
                            value = 0;
                        }
                    }
                    audio::PlaySe_52C450(51, 320);
                }
            }
        }
        if (showVersionControls) {
            if (Sub_464290(&kCreationUi54E0BC[4].bounds) != 0) {
                arrows[4] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    const std::uint8_t value = state::gValue9A4B88;
                    const std::uint8_t version = state::gValue9A57E0;
                    const auto next = static_cast<std::uint8_t>(value - 1);
                    state::gValue9A4B88 = next;
                    if (static_cast<std::int8_t>(next) < (version != 0 ? 1 : 0)) {
                        state::gValue9A4B88 = maxVersion;
                    }
                    audio::PlaySe_52C450(51, 320);
                }
            }
            if (Sub_464290(&kCreationUi54E0BC[5].bounds) != 0) {
                arrows[5] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    const std::uint8_t value = state::gValue9A4B88;
                    const std::uint8_t version = state::gValue9A57E0;
                    const auto next = static_cast<std::uint8_t>(value + 1);
                    state::gValue9A4B88 = next;
                    if (static_cast<std::int8_t>(next) > maxVersion) {
                        state::gValue9A4B88 = version != 0 ? 1 : 0;
                    }
                    audio::PlaySe_52C450(51, 320);
                }
            }
        }

        const std::int32_t remaining = static_cast<std::int8_t>(state::gValue9A4D04);
        for (std::size_t row = 0; row < attributes.size(); ++row) {
            const std::int32_t value = static_cast<std::int8_t>(*attributes[row]);
            if (value <= 0) {
                attributeMinus[row] = -1;
            }
            if (remaining <= 0 || value >= attributeLimit) {
                attributePlus[row] = -1;
            }
        }
        for (std::size_t row = 0; row < attributes.size(); ++row) {
            const std::int32_t top = kCreationUi54E0BC[6].bounds.top
                + 14 * static_cast<std::int32_t>(row);
            if (static_cast<std::int8_t>(*attributes[row]) > 0 && hitRow(6, top) != 0) {
                attributeMinus[row] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    const std::uint8_t value = *attributes[row];
                    if (static_cast<std::int8_t>(value) > 0) {
                        const std::uint8_t left = state::gValue9A4D04;
                        *attributes[row] = static_cast<std::uint8_t>(value - 1);
                        state::gValue9A4D04 = static_cast<std::uint8_t>(left + 1);
                        audio::PlaySe_52C450(51, 320);
                    } else {
                        audio::PlaySe_52C450(55, 320);
                    }
                }
                break;
            }
            if (static_cast<std::int8_t>(state::gValue9A4D04) > 0
                && static_cast<std::int8_t>(*attributes[row]) < attributeLimit && hitRow(7, top) != 0) {
                attributePlus[row] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    bool changed = false;
                    if (static_cast<std::int8_t>(state::gValue9A4D04) > 0) {
                        const std::uint8_t value = *attributes[row];
                        if (static_cast<std::int8_t>(value) < attributeLimit) {
                            const std::uint8_t left = state::gValue9A4D04;
                            *attributes[row] = static_cast<std::uint8_t>(value + 1);
                            state::gValue9A4D04 = static_cast<std::uint8_t>(left - 1);
                            changed = true;
                        }
                    }
                    audio::PlaySe_52C450(changed ? 51 : 55, 320);
                }
                break;
            }
        }

        for (std::size_t row = 0; row < 4; ++row) {
            const std::int32_t value = static_cast<std::int8_t>(elements[row]);
            if (value <= 0) {
                elementMinus[row] = -1;
            }
            if (value >= 10) {
                elementPlus[row] = -1;
            }
        }
        for (std::size_t row = 0; row < 4; ++row) {
            const std::int32_t top = kCreationUi54E0BC[8].bounds.top
                + 14 * static_cast<std::int32_t>(row);
            if (static_cast<std::int8_t>(elements[row]) > 0 && hitRow(8, top) != 0) {
                elementMinus[row] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    const std::uint8_t value = elements[row];
                    if (static_cast<std::int8_t>(value) > 0) {
                        const std::uint8_t left = state::gValue9A50C8;
                        elements[row] = static_cast<std::uint8_t>(value - 1);
                        state::gValue9A50C8 = static_cast<std::uint8_t>(left + 1);
                        audio::PlaySe_52C450(51, 320);
                    } else {
                        audio::PlaySe_52C450(55, 320);
                    }
                }
                break;
            }
            if (static_cast<std::int8_t>(elements[row]) < 10 && hitRow(9, top) != 0) {
                elementPlus[row] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    const std::size_t opposite = row < 2 ? row + 2 : row - 2;
                    if (elements[opposite] == 0) {
                        if (static_cast<std::int8_t>(state::gValue9A50C8) > 0) {
                            const std::uint8_t value = elements[row];
                            const std::uint8_t left = state::gValue9A50C8;
                            elements[row] = static_cast<std::uint8_t>(value + 1);
                            state::gValue9A50C8 = static_cast<std::uint8_t>(left - 1);
                            audio::PlaySe_52C450(51, 320);
                        } else {
                            std::size_t donor = 0;
                            while (donor < 4
                                && (donor == row || static_cast<std::int8_t>(elements[donor]) <= 0)) {
                                ++donor;
                            }
                            if (donor < 4) {
                                ++elements[row];
                                --elements[donor];
                                audio::PlaySe_52C450(51, 320);
                            } else {
                                audio::PlaySe_52C450(55, 320);
                            }
                        }
                    }
                }
                break;
            }
        }

        if (Sub_464290(&kCreationUi54E0BC[10].bounds) != 0) {
            confirmState = hover();
            if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
                const std::int32_t cursor = state::gCreationInput9A51E8.caretByteOffset;
                if (cursor <= 0) {
                    state::gValue9A5AFC = 100;
                } else if (static_cast<std::int8_t>(state::gValue9A4D04) > 0) {
                    state::gValue9A5AFC = 104;
                } else if (static_cast<std::int8_t>(state::gValue9A50C8) > 0) {
                    state::gValue9A5AFC = 106;
                } else {
                    const std::wstring utf16Name(state::gCreationInput9A51E8.text,
                        cg::text::Prefix(state::gCreationInput9A51E8.text, static_cast<std::size_t>(cursor)));
                    std::optional<std::string> stored;
                    try { stored = cg::text::ToGbk(utf16Name); }
                    catch (const std::system_error&) { state::gValue9A5AFC = 108; }
                    if (stored) {
                        if (stored->size() >= state::gCreationNameFF0D18.size()) {
                            state::gValue9A5AFC = 108;
                        } else {
                            char* const name = state::gCreationNameFF0D18.data();
                            std::memcpy(name, stored->c_str(), stored->size() + 1);
                            if (login::CharacterNameExists_51B140(name) != 0) {
                                state::gValue9A5AFC = 102;
                            } else if (std::strstr(name, " ") != nullptr
                                || std::strstr(name, "\x81\x40") != nullptr
                                || input::IsRejectedInputText_404060(utf16Name)) {
                                state::gValue9A5AFC = 108;
                            } else if (input::Sub_45E770(utf16Name) == 1) {
                                state::gValue9A5AFC = 110;
                            } else {
                                state::gValue9A5AFC = 10;
                                audio::PlaySe_52C450(53, 320);
                            }
                        }
                    }
                }
            }
        }
        if (Sub_464290(&kCreationUi54E0BC[11].bounds) != 0) {
            cancelState = hover();
            if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
                audio::PlaySe_52C450(54, 320);
                result = 2;
            }
        }
    }

    for (std::size_t row = 0; row < 4; ++row) {
        const std::size_t opposite = row < 2 ? row + 2 : row - 2;
        if (elements[opposite] != 0) {
            elementMinus[row] = -1;
            elementPlus[row] = -1;
        }
    }
    bool skipMessages = false;
    if (state::gValue9A5AFC == 10) {
        const std::int32_t answer = ProcessDefaultConfirmationDialog_45E090(
            320, 240, L"可以吗？" /* 原常量参考：0x0055C9B0。 */, 0);
        if (answer == 1) {
            const std::int32_t version = static_cast<std::int8_t>(state::gValue9A57E0);
            const std::uint8_t mode = state::gValue9A4B88;
            result = 1;
            const std::int32_t character = static_cast<std::int8_t>(state::gValue9A4CFC);
            const std::int32_t variant = static_cast<std::int8_t>(state::gValue9A5B00);
            state::gValue9A5AFC = 11;
            if (mode == 0) {
                state::gPreviewToggleState = 0;
                state::gValueDE0AFC = kCreationMotion54DDB0[28 * version + character] + 6 * variant;
            } else {
                state::gValueDE0AFC = kCreationMotion54DDB0[28 * version + character + 14] + 6 * variant;
                state::gPreviewToggleState = 1;
            }
            // 原ESI沿用上方四项互斥循环终值，未知模式不是零图号。
            std::int32_t graphic = 4;
            if (mode == 2) {
                const std::int32_t style = static_cast<std::int8_t>(state::gValue9A5668);
                graphic = kCreationGraphic55C354[14 * version + character];
                const std::int32_t color = static_cast<std::int8_t>(state::gValue9A4B80);
                graphic += 5 * (5 * variant + style) + color;
            } else if (mode == 1 || mode == 0) {
                graphic = MapCreationGraphic(kCreationGraphic55C354[14 * version + character], mode);
            }

            const std::int32_t attribute0 = static_cast<std::int8_t>(*attributes[0]);
            const std::int32_t attribute1 = static_cast<std::int8_t>(*attributes[1]);
            const std::int32_t attribute2 = static_cast<std::int8_t>(*attributes[2]);
            state::gValueF633EC = attribute0;
            state::gValueF633F0 = attribute1;
            const std::int32_t attribute3 = static_cast<std::int8_t>(*attributes[3]);
            const std::int32_t attribute4 = static_cast<std::int8_t>(*attributes[4]);
            state::gValueDE0AF8 = attribute3;
            const std::uint32_t elementSnapshot = state::gValue9A50D0;
            state::gValueDE03F4 = attribute2;
            state::gValueF7CA00 = attribute4;
            const std::int32_t element2 = static_cast<std::int8_t>(elements[2]);
            state::gValueF7C948 = static_cast<std::int8_t>(elementSnapshot);
            state::gSelectedGraphicNo = graphic;
            const std::int32_t element3 = static_cast<std::int8_t>(elements[3]);
            state::gValueFF03D0 = static_cast<std::int8_t>(elementSnapshot >> 8);
            state::gValueF633F4 = element2;
            state::gValueDE0B04 = element3;
            world::gValueFDA6F8 = 0;
            cg::audio::Sub_52B990();
            Sub_4642C0();
        } else if (answer == 2) {
            state::gValue9A5AFC = 1;
            skipMessages = true;
        }
    }
    if (!skipMessages) {
        for (std::size_t index = 0; index < kCreationErrors.size(); ++index) {
            const std::int32_t phase = 100 + 2 * static_cast<std::int32_t>(index);
            if (state::gValue9A5AFC == phase) {
                login::ResetLoginMessage();
                ++state::gValue9A5AFC;
                audio::PlaySe_52C450(55, 320);
            }
            if (state::gValue9A5AFC == phase + 1
                && login::UpdateLoginMessage(kCreationErrors[index]) != 0) {
                state::gValue9A5AFC = 1;
                break;
            }
        }
    }

    const auto drawButton = [](const std::int32_t x, const std::int32_t y,
                               const std::size_t record, const std::int8_t buttonState) {
        cg::render::DrawGraphic(
            x, y, 10, std::bit_cast<std::array<std::int32_t, 7>>(kCreationUi54E0BC[record])[4 + buttonState], 0, nullptr);
    };
    drawButton(411, 137, 0, arrows[0]);
    drawButton(442, 137, 1, arrows[1]);
    drawButton(411, 155, 2, arrows[2]);
    drawButton(442, 155, 3, arrows[3]);
    if (showVersionControls) {
        drawButton(461, 157, 4, arrows[4]);
        drawButton(537, 157, 5, arrows[5]);
    }
    for (std::size_t row = 0; row < attributes.size(); ++row) {
        const std::int32_t y = 205 + 14 * static_cast<std::int32_t>(row);
        drawButton(509, y, 6, attributeMinus[row]);
        drawButton(523, y, 7, attributePlus[row]);
    }
    for (std::size_t row = 0; row < 4; ++row) {
        const std::int32_t y = 313 + 14 * static_cast<std::int32_t>(row);
        if (elementMinus[row] >= 0) {
            drawButton(509, y, 8, elementMinus[row]);
        }
        if (elementPlus[row] >= 0) {
            drawButton(523, y, 9, elementPlus[row]);
        }
    }
    drawButton(403, 381, 11, cancelState);
    drawButton(483, 381, 10, confirmState);

    std::uint32_t elementSnapshot = state::gValue9A50D0;
    std::int32_t count = 0;
    if (static_cast<std::int8_t>(elementSnapshot) > 0) {
        do {
            cg::render::DrawGraphic(406 + 10 * count, 313, 10, 245052, 0, nullptr);
            elementSnapshot = state::gValue9A50D0;
            ++count;
        } while (count < static_cast<std::int8_t>(elementSnapshot));
    }
    count = 0;
    if (static_cast<std::int8_t>(elementSnapshot >> 8) > 0) {
        do {
            cg::render::DrawGraphic(406 + 10 * count, 327, 10, 245053, 0, nullptr);
            const std::int32_t value = static_cast<std::int8_t>(elements[1]);
            ++count;
            if (count >= value) {
                break;
            }
        } while (true);
    }
    for (std::size_t row = 2; row < 4; ++row) {
        const std::int32_t value = static_cast<std::int8_t>(elements[row]);
        count = 0;
        if (value > 0) {
            do {
                cg::render::DrawGraphic(
                    406 + 10 * count, 313 + 14 * static_cast<std::int32_t>(row),
                    10, 245052 + static_cast<std::int32_t>(row), 0, nullptr);
                const std::int32_t current = static_cast<std::int8_t>(elements[row]);
                ++count;
                if (count >= current) {
                    break;
                }
            } while (true);
        }
    }
    // 与确认路径不同，此处未知模式沿用第四元素条的计数终值。
    std::int32_t graphic = count;
    const std::int32_t mode = static_cast<std::int8_t>(state::gValue9A4B88);
    if (mode == 2) {
        const std::int32_t version = static_cast<std::int8_t>(state::gValue9A57E0);
        const std::int32_t character = static_cast<std::int8_t>(state::gValue9A4CFC);
        const std::int32_t variant = static_cast<std::int8_t>(state::gValue9A5B00);
        const std::int32_t style = static_cast<std::int8_t>(state::gValue9A5668);
        graphic = kCreationGraphic55C354[14 * version + character];
        const std::int32_t color = static_cast<std::int8_t>(state::gValue9A4B80);
        graphic += 5 * (5 * variant + style) + color;
    } else if (mode == 1 || mode == 0) {
        const std::int32_t version = static_cast<std::int8_t>(state::gValue9A57E0);
        const std::int32_t character = static_cast<std::int8_t>(state::gValue9A4CFC);
        graphic = MapCreationGraphic(kCreationGraphic55C354[14 * version + character], mode);
    }
    if (static_cast<std::int8_t>(state::gValue9A4B88) > maxVersion) {
        graphic = static_cast<std::int8_t>(state::gValue9A4CFC) >= 14 ? 33101 : 33100;
    }
    cg::render::DrawGraphic(500, 126, 10, graphic, 0, nullptr);
    render::DrawInputText_4568A0(&state::gCreationInput9A51E8);
    constexpr std::int8_t color = std::bit_cast<std::int8_t>(std::uint8_t{131});
    cg::text::FormatTo(buffer, L"{:02}", static_cast<std::int32_t>(static_cast<std::int8_t>(state::gValue9A4D04)));
    render::DrawText_456780(518, 185, 0, 13, color, buffer, 0, 0);
    for (std::size_t row = 0; row < attributes.size(); ++row) {
        cg::text::FormatTo(buffer, L"{:02}", static_cast<std::int32_t>(static_cast<std::int8_t>(*attributes[row])));
        render::DrawText_456780(
            479, static_cast<std::int16_t>(199 + 14 * row), 0, 13, color, buffer, 0, 0);
    }
    cg::text::FormatTo(buffer, L"{:02}", static_cast<std::int32_t>(static_cast<std::int8_t>(state::gValue9A50C8)));
    render::DrawText_456780(518, 293, 0, 13, color, buffer, 0, 0);
    cg::render::DrawGraphic(442, 241, 10, 245051, 0, nullptr);
    cg::render::DrawGraphic(320, 240, 3, 245050, 0, nullptr);
    return result;
}

// 原函数：0x004642C0。
std::int32_t Sub_4642C0()
{
    const std::int32_t valueF633F0 = state::gValueF633F0;
    const std::int32_t valueF633EC = state::gValueF633EC;
    gCreationSnapshot.value9A50A4 = valueF633F0;
    const std::int32_t valueF7CA00 = state::gValueF7CA00;
    const std::int32_t valueDE03F4 = state::gValueDE03F4;
    gCreationSnapshot.value9A4B98 = valueF7CA00;
    const std::int32_t valueF633F4 = state::gValueF633F4;
    gCreationSnapshot.value9A50A0 = valueF633EC;
    const std::int32_t valueDE0AF8 = state::gValueDE0AF8;
    gCreationSnapshot.value9A5670 = valueF633F4;
    const std::int32_t value9A50C8 = std::bit_cast<std::int8_t>(state::gValue9A50C8);
    gCreationSnapshot.value9A4E98 = valueDE0AF8;
    const std::int32_t valueFF03D0 = state::gValueFF03D0;
    gCreationSnapshot.value9A4E84 = valueDE03F4;
    const std::int32_t valueF7C948 = state::gValueF7C948;
    gCreationSnapshot.value9A50E4 = value9A50C8;
    gCreationSnapshot.value9A60E8 = valueFF03D0;
    const std::int32_t value9A4B88 = std::bit_cast<std::int8_t>(state::gValue9A4B88);
    const std::int32_t value9A4D04 = std::bit_cast<std::int8_t>(state::gValue9A4D04);
    gCreationSnapshot.value9A5094 = valueF7C948;
    const std::int32_t valueDE0B04 = state::gValueDE0B04;
    gCreationSnapshot.value9A57E8 = value9A4B88;
    gCreationSnapshot.value9A61F4 = valueDE0B04;
    const std::int32_t value9A5668 = std::bit_cast<std::int8_t>(state::gValue9A5668);
    gCreationSnapshot.value9A554C = value9A4D04;
    const std::int32_t value9A4B80 = std::bit_cast<std::int8_t>(state::gValue9A4B80);
    gCreationSnapshot.input9A4F38 = state::gCreationInput9A51E8;
    gCreationSnapshot.value9A5664 = value9A5668;
    gCreationSnapshot.value9A5954 = value9A4B80;
    return value9A4B80;
}

// 原函数：0x00464380。
std::uint8_t Sub_464380()
{
    auto* const firstBytes = reinterpret_cast<std::uint8_t*>(&state::gValue9A5FD4);
    auto* const secondBytes = reinterpret_cast<std::uint8_t*>(&state::gValue9A50D0);
    const std::uint8_t value9A50A4 = ReadCreationSnapshotByte(gCreationSnapshot.value9A50A4);
    const std::uint8_t value9A50A0 = ReadCreationSnapshotByte(gCreationSnapshot.value9A50A0);
    const std::uint8_t value9A4E84 = ReadCreationSnapshotByte(gCreationSnapshot.value9A4E84);
    firstBytes[1] = value9A50A4;
    const std::uint8_t value9A4B98 = ReadCreationSnapshotByte(gCreationSnapshot.value9A4B98);
    firstBytes[0] = value9A50A0;
    const std::uint8_t value9A4E98 = ReadCreationSnapshotByte(gCreationSnapshot.value9A4E98);
    state::gValue9A5FD8 = value9A4B98;
    const std::uint8_t value9A5670 = ReadCreationSnapshotByte(gCreationSnapshot.value9A5670);
    firstBytes[2] = value9A4E84;
    const std::uint8_t value9A5094 = ReadCreationSnapshotByte(gCreationSnapshot.value9A5094);
    secondBytes[2] = value9A5670;
    const std::uint8_t value9A50E4 = ReadCreationSnapshotByte(gCreationSnapshot.value9A50E4);
    firstBytes[3] = value9A4E98;
    const std::uint8_t value9A60E8 = ReadCreationSnapshotByte(gCreationSnapshot.value9A60E8);
    state::gValue9A50C8 = value9A50E4;
    const std::uint8_t value9A57E8 = ReadCreationSnapshotByte(gCreationSnapshot.value9A57E8);
    secondBytes[0] = value9A5094;
    const std::uint8_t value9A61F4 = ReadCreationSnapshotByte(gCreationSnapshot.value9A61F4);
    secondBytes[1] = value9A60E8;
    const std::uint8_t value9A554C = ReadCreationSnapshotByte(gCreationSnapshot.value9A554C);
    state::gValue9A4B88 = value9A57E8;
    secondBytes[3] = value9A61F4;
    const std::uint8_t value9A5664 = ReadCreationSnapshotByte(gCreationSnapshot.value9A5664);
    state::gValue9A4D04 = value9A554C;
    const std::uint8_t value9A5954 = ReadCreationSnapshotByte(gCreationSnapshot.value9A5954);
    state::gValue9A5668 = value9A5664;
    state::gCreationInput9A51E8 = gCreationSnapshot.input9A4F38;
    state::gValue9A4B80 = value9A5954;
    return value9A5954;
}

// 原函数：0x00464290。
std::int32_t Sub_464290(const cg::ui::UiControlBounds* const rectangle)
{
    const std::int32_t top = rectangle->top;
    const std::int32_t left = rectangle->left;
    const std::int32_t height = rectangle->height;
    const std::int32_t width = rectangle->width;
    return cg::ui::HitTestMouseRect_5049A0(
        left, top, left + width, top + height, -1);
}

std::wstring gText9A4EAC; // 原全局：0x009A4EAC。

} // namespace cg::character_select::detail

namespace cg::character_select::state {

std::uint8_t gPukVersion = 0; // 原全局：0x009A6E1C。
std::uint8_t gGraphicVersionOverride58B6A0 = 0xFF; // 原全局：0x0058B6A0。
std::uint8_t gValue9A57E0 = 0; // 原全局：0x009A57E0。
std::uint8_t gValue9A4CFC = 0; // 原全局：0x009A4CFC。
std::uint8_t gValue9A5B00 = 0; // 原全局：0x009A5B00。
std::array<cg::yobi::YobiAction*, 2> gCreateActions {};
input::InputContext gCreationInput9A51E8 {}; // 原全局：0x009A51E8。
std::uint8_t gValue9A5E78 = 0; // 原全局：0x009A5E78。
std::int32_t gValue9A5FA4 = 0; // 原全局：0x009A5FA4。
std::int32_t gValue9A5FA0 = 0; // 原全局：0x009A5FA0。
std::uint32_t gValue9A5FD4 = 0; // 原全局：0x009A5FD4。
std::uint32_t gValue9A50D0 = 0; // 原全局：0x009A50D0。
std::uint8_t gValue9A5FD8 = 0; // 原全局：0x009A5FD8。
std::uint8_t gValue9A5668 = 0; // 原全局：0x009A5668。
std::uint8_t gValue9A4B80 = 0; // 原全局：0x009A4B80。
std::uint8_t gValue9A4D04 = 0; // 原全局：0x009A4D04。
std::uint8_t gValue9A50C8 = 0; // 原全局：0x009A50C8。
std::uint8_t gValue9A4B88 = 0; // 原全局：0x009A4B88。
std::int32_t gValue9A5AFC = 0; // 原全局：0x009A5AFC。
std::int32_t gValueF633F0 = 0; // 原全局：0x00F633F0。
std::int32_t gValueF7CA00 = 0; // 原全局：0x00F7CA00。
std::int32_t gValueF633EC = 0; // 原全局：0x00F633EC。
std::int32_t gValueF633F4 = 0; // 原全局：0x00F633F4。
std::int32_t gValueDE0AF8 = 0; // 原全局：0x00DE0AF8。
std::int32_t gValueDE03F4 = 0; // 原全局：0x00DE03F4。
std::int32_t gValueFF03D0 = 0; // 原全局：0x00FF03D0。
std::int32_t gValueF7C948 = 0; // 原全局：0x00F7C948。
std::int32_t gValueDE0B04 = 0; // 原全局：0x00DE0B04。
std::int32_t gValueDE0AFC = 0; // 原全局：0x00DE0AFC。
std::array<char, 24> gCreationNameFF0D18 {}; // 原全局：0x00FF0D18。
std::array<cg::yobi::YobiAction*, 2> gPreviewActions {};
cg::yobi::YobiAction* gCharacterModalAction = nullptr; // 原全局：0x009A6E64。
std::int16_t gPendingCharacterCount = 0; // 原全局：0x00F7CA06。
std::int32_t gSelectedGraphicNo = 0; // 原全局：0x00F7C9FC。
std::uint16_t gCharacterEntryState = 0; // 原全局：0x009A6E10。
std::int32_t gPreviewToggleState = 0; // 原全局：0x009A6E14。
char gSelectedCharacterName[256] {}; // 原全局：0x00DA391C。
char gDeleteCharacterName[132] {}; // 原全局：0x00DA3A9C。

static_assert(sizeof(gCreateActions) == 2 * sizeof(cg::yobi::YobiAction*));
static_assert(sizeof(gPreviewActions) == 2 * sizeof(cg::yobi::YobiAction*));
static_assert(sizeof(gSelectedCharacterName) == 256);
static_assert(sizeof(gDeleteCharacterName) == 132);

} // namespace cg::character_select::state

namespace cg::character_select {

static constexpr std::size_t kLegacyNameCapacity = 20;

static detail::CharacterSelectUiRecord gReturnUi {
    {534, 438, 64, 16}, 245045, 245047, 245046,
};
static detail::CharacterSelectUiRecord gUpdateUi {
    {296, 433, 49, 17}, 244542, 244544, 244543,
};

// 原 0x9A6EAC 与 0x9A6EB0 只被本控制器直接持有。
static std::int32_t gUpdateAvailable = 0;
static cg::yobi::YobiAction* gDeleteConfirmationAction = nullptr;

static void CopyRecordName(char* destination, const login::CharacterRecord& record)
{
    char name[kLegacyNameCapacity];
    // 与原strcpy相同，调用前提是姓名适合20字节临时区且有NUL终止。
    std::strcpy(name, record.name.data());
    cg::core::NullSub_509080();
    std::strcpy(destination, name);
}

static void ClearPreviewActions()
{
    if (state::gPreviewActions[0] != nullptr) {
        state::gPreviewActions[0] = nullptr;
    }
    if (state::gPreviewActions[1] != nullptr) {
        state::gPreviewActions[1] = nullptr;
    }
}

// CUN01：原460FF0角色进入分支，供Debug直入及非Debug确认后共用，调用顺序保持。
static void EnterSelectedCharacter()
{
    const login::CharacterRecord& record =
        login::gCharacterRecords[static_cast<std::size_t>(login::gSelectedCharacterIndex)];
    CopyRecordName(state::gSelectedCharacterName, record);
    const std::int16_t selected = login::gSelectedCharacterIndex;
    const login::CharacterRecord* const selectedRecord =
        login::gCharacterRecords.data() + selected;
    state::gSelectedGraphicNo = selectedRecord->graphicNo;
    world::SetCurrentSceneType(5);
    login::detail::PlaySe(59, 320, 240);
    state::gCharacterEntryState = 0;
    cg::audio::Sub_52B990();
    cg::yobi::YobiClearUp_404D30();
    ClearPreviewActions();
}

#ifndef _DEBUG
// CUN01：本端人物页扩展状态；等待明确选择，不复用原0..21业务状态。
static constexpr std::int32_t kUsageNoticeState = 22;
thread_local cg::platform::LegacyCryptInt state1(1);
thread_local cg::platform::LegacyCryptInt state2(std::random_device().operator()());

#define MIXVL 0x1a57b94f
#define MXTRUE 0x8ef61

// CUN01：沿现有游戏模态动作与文字按钮，返回0等待、1同意、2退出。
static std::int32_t ProcessUsageNotice()
{
    std::int32_t iRet = 0;
    VM_DOLPHIN_WHITE_START
    std::array<std::wstring_view, 6> lines {{
        L"本程序仅供学习研究使用，严禁用于任何商业用途。",
        L"本程序所涉及的所有版权均归官方所有，未经官方正式授权，",
        L"任何单位或个人不得将本程序用于商业盈利活动。",
        L"若您因误信非官方渠道信息，为使用本程序支付了相关费用，",
        L"请您及时与收费方沟通并申请退款，",
        L"由此产生的一切纠纷与本程序提供方无关。",
    }};
    std::array<detail::CharacterModalUiRecord, 2> buttons {{
        {264, 368, 240, 366, 64, 24, L"同意"},
        {360, 368, 336, 366, 64, 24, L"结束"},
    }};

    auto*& action = state::gCharacterModalAction;
    if (action == nullptr) {
        action = cg::yobi::CreateModalAction_5044E0(32, 72, 576, 336, 4, 1);
        goto RET; // 创建当帧不消费人物选择的点击；失败时下一帧重试。
    }
    if (!cg::yobi::IsYobiActionReady(action, 1)) {
        goto RET;
    }

    for (std::size_t index = 0; index < buttons.size(); ++index) {
        if ((detail::Sub_503990(&buttons[index]) & 1) != 0) {
            login::detail::PlaySe(79, 320, 240);
            cg::yobi::MarkDeath(action);
            action = nullptr;
            iRet = static_cast<std::int32_t>(index) + 1;
            goto RET;
        }
    }

    cg::render::DrawText_456780(304, 96, 2, 0, 4, L"提示", 0, 0);
    for (std::size_t index = 0; index < lines.size(); ++index) {
        cg::render::DrawText_456780(
            56, static_cast<std::int16_t>(128 + index * 24),
            2, 0, 0, lines[index], 0, 0);
    }
    for (const auto& button : buttons) {
        detail::Sub_503A20(&button, 2, 4, 1, cg::render::gValueD69768, 214);
    }
RET:
    VM_DOLPHIN_WHITE_END
    return iRet;
}
#endif

static void UpdateCharacterGraphic(
    login::CharacterRecord& record,
    std::int32_t action)
{
    const std::uint8_t pukVersion = state::gPukVersion;
    if (pukVersion >= 7u) {
        const std::int32_t imageNo = record.graphicNo;
        if (imageNo == 33100 || imageNo == 33101) {
            return;
        }

        if (imageNo >= 30000 && imageNo < 98000) {
            record.graphicClass = 0;
        } else if (imageNo >= 202800 && imageNo < 204200) {
            record.graphicClass = 1;
        } else if (imageNo >= 200000 && imageNo < 202700) {
            record.graphicClass = 1;
        } else if ((imageNo >= 240000 && imageNo <= 242800)
            || (imageNo >= 205000 && imageNo <= 205600)
            || (imageNo >= 270000 && imageNo <= 270200)) {
            record.graphicClass = 2;
        }

        const std::int32_t graphicClass =
            record.graphicClass;
        const std::int32_t selector = graphicClass + 2 * action - 7;

        if (graphicClass == 0) {
            if (selector == 1) {
                record.graphicNo = detail::Sub_51BB80(record.graphicNo);
            } else {
                const std::int32_t first = detail::Sub_51BB80(
                    record.graphicNo);
                record.graphicNo = first;
                record.graphicNo = detail::Sub_51BFF0(first);
            }
        } else if (graphicClass == 1) {
            if (selector == 2) {
                record.graphicNo = detail::Sub_51BFF0(record.graphicNo);
            } else if (selector == 0) {
                record.graphicNo = imageNo >= 200000 && imageNo < 202700
                    ? detail::Sub_51BFF0(record.graphicNo)
                    : detail::Sub_51BAC0(record.graphicNo);
            }
        } else if (graphicClass == 2) {
            if (selector == 3 && imageNo >= 240000 && imageNo < 241400) {
                const std::int32_t first = detail::Sub_51BF00(
                    record.graphicNo);
                record.graphicNo = first;
                record.graphicNo = detail::Sub_51BAC0(first);
            } else {
                record.graphicNo = detail::Sub_51BF00(record.graphicNo);
            }
        }
        return;
    }

    if (pukVersion < 5u) {
        return;
    }

    const std::int32_t imageNo = record.graphicNo;
    if (imageNo == 33100 || imageNo == 33101) {
        return;
    }

    if (imageNo >= 30000 && imageNo < 98000) {
        record.graphicClass = 0;
    } else if (imageNo >= 202800 && imageNo < 204200) {
        record.graphicClass = 1;
    }

    const std::int32_t graphicClass = record.graphicClass;
    if (graphicClass == 0) {
        record.graphicNo = detail::Sub_51BB80(record.graphicNo);
    } else if (graphicClass == 1) {
        record.graphicNo = detail::Sub_51BAC0(record.graphicNo);
    }
}

// 原函数：0x00460FF0。
#pragma optimize("", off)
std::int32_t ProcessScene_460FF0()
{
    VM_DOLPHIN_WHITE_START
    std::int32_t sceneResult;
    // 单次执行块使普通提前退出先经过VM结束标记，不改变后续帧的状态流转。
    do {
        std::int32_t interactive = 1;

        const std::uint32_t mainState = static_cast<std::uint32_t>(world::gCurrentSceneState);
        if (mainState <= 22u) {
            if (mainState == 0u) {
                world::gCurrentSceneState = 1;
                state::gPukVersion = 0;
                ResetCharacterListProgress_45DD50();
                gUpdateAvailable = 0;
                render::SelectPalette(0, 0);
            } else if (mainState == 1u) {
                const std::int32_t result = UpdateCharacterListProgress_45DD60();
                if (result == 1) {
                    world::gCurrentSceneState = 9;
                } else if (result == -3) {
                    world::gCurrentSceneState = 2;
                    cg::network::ShutdownNetworkRuntime_5081A0();
                } else if (result == -4) {
                    world::gCurrentSceneState = 7;
                    cg::network::ShutdownNetworkRuntime_5081A0();
                } else if (result < 0) {
                    world::gCurrentSceneState = 4;
                    cg::network::ShutdownNetworkRuntime_5081A0();
                }
            } else if (mainState == 2u) {
                login::ResetLoginMessage();
                world::gCurrentSceneState = 6;
            } else if (mainState == 3u) {
                // 原状态3不执行操作。
            } else if (mainState == 4u) {
                login::ResetLoginMessage();
                world::gCurrentSceneState = 5;
            } else if (mainState == 5u) {
                if (login::UpdateLoginMessage(L"角色资料读取失败。" /* 原 0x55CD68: 角色资料读取失败提示。 */) != 0) {
                    world::SetCurrentSceneType(2);
                }
            } else if (mainState == 6u) {
                if (login::UpdateLoginMessage(L"目前伺服器正在停机维护中。" /* 原 0x55CD4C: 服务器维护提示。 */) != 0) {
                    world::SetCurrentSceneType(2);
                }
            } else if (mainState == 7u) {
                ResetAuthenticationFailure_45D7D0();
                world::gCurrentSceneState = 8;
            } else if (mainState == 8u) {
                if (UpdateAuthenticationFailure_45D7E0() != 0) {
                    world::SetCurrentSceneType(2);
                }
            } else if (mainState == 9u) {
                const std::int32_t capability = resource::ProbePaletteCapability_45C970();
                const std::uint8_t pukVersion = state::gPukVersion;
                render::gPaletteCapability = capability;
                if (pukVersion == 1u) {
                    world::gCurrentSceneState = 10;
                } else if ((capability < 2 && pukVersion >= 5u)
                    || (capability < 1 && pukVersion >= 3u)
                    || (capability < 3 && pukVersion >= 7u)
                    || (capability < 4 && pukVersion >= 8u)) {
                    world::gCurrentSceneState = 19;
                } else {
                    if (capability == 1) {
                        if (pukVersion == 0u || pukVersion == 2u) {
                            gUpdateAvailable = 1;
                        }
                    } else if (capability >= 2
                        && (pukVersion == 0u || pukVersion == 2u)) {
                        gUpdateAvailable = 1;
                    } else if (capability >= 3
                        && pukVersion >= 5u
                        && pukVersion <= 6u) {
                        gUpdateAvailable = 1;
                    } else if (capability == 4 && pukVersion == 7u) {
                        gUpdateAvailable = 1;
                    }
                    world::gCurrentSceneState = 10;
                }
            } else if (mainState == 10u) {
                world::gCurrentSceneState = 11;
                ProcessCharacterPreview_460D00(0);
                ProcessCharacterPreview_460D00(1);
            } else if (mainState == 11u) {
                interactive = 0;
            } else if (mainState == 12u) {
                const std::int32_t result =
                    ProcessCharacterDeletionWarning_45E0C0(320, 240);
                if (result == 1) {
                    world::gCurrentSceneState = 13;
                    gDeleteConfirmationAction =
                        cg::yobi::CreateModalAction_5044E0(130, 300, 340, 52, 4, 1);
                } else if (result == 2) {
                    world::gCurrentSceneState = 10;
                }
            } else if (mainState == 13u) {
                const std::int32_t result =
                    ProcessDefaultConfirmationDialog_45E090(
                        320,
                        240,
                        L"真的要删除吗？" /* 原 0x55C854: 删除角色确认提示。 */,
                        0);
                if (cg::yobi::IsYobiActionReady(gDeleteConfirmationAction, 1)) {
                    cg::render::DrawText3_456560(166, 326, 2, 4, L"想要删除时，请按着 CTRL键不放" /* 原 0x55CD94: 按住 CTRL 删除角色提示。 */, 0);
                    cg::render::DrawText3_456560(166, 346, 2, 4, L"再点选『是』。        " /* 原 0x55CD7C: 再次点击“是”的提示，保留八个尾空格。 */, 0);
                    if (result == 1) {
                        if ((platform::gKeyStates[17] & 2u) != 0) {
                            world::gCurrentSceneState = 14;
                            login::detail::PlaySe(284, 320, 240);
                            if (gDeleteConfirmationAction != nullptr) {
                                cg::yobi::MarkDeath(gDeleteConfirmationAction);
                                gDeleteConfirmationAction = nullptr;
                            }
                            ResetCharacterDeletionProgress_45DBF0();
                            world::gCurrentSceneState = 14;
                        }
                    } else if (result == 2) {
                        world::gCurrentSceneState = 10;
                        if (gDeleteConfirmationAction != nullptr) {
                            cg::yobi::MarkDeath(gDeleteConfirmationAction);
                            gDeleteConfirmationAction = nullptr;
                        }
                    }
                }
            } else if (mainState == 14u) {
                const std::int32_t result = UpdateCharacterDeletionProgress_45DC00();
                if (result == 1) {
                    cg::login::Sub_51B3B0(static_cast<std::uint32_t>(
                        static_cast<std::int32_t>(login::gSelectedCharacterIndex)));
                    const std::int16_t unusedSelected = login::gSelectedCharacterIndex;
                    static_cast<void>(unusedSelected);
                    cg::core::NullSub_509080();
                    cg::login::detail::Sub_520200();
                    if (state::gPendingCharacterCount > 0) {
                        --state::gPendingCharacterCount;
                    }
                    world::gCurrentSceneState = 10;
                } else if (result == -2) {
                    world::gCurrentSceneState = 15;
                }
            } else if (mainState == 15u) {
                login::ResetLoginMessage();
                world::gCurrentSceneState = 16;
            } else if (mainState == 16u) {
                if (login::UpdateLoginMessage(L"无法删除此角色。" /* 原 0x55CD38: 无法删除角色提示。 */) != 0) {
                    world::gCurrentSceneState = 0;
                }
            } else if (mainState == 17u) {
                login::ResetLoginMessage();
                world::gCurrentSceneState = 18;
            } else if (mainState == 18u) {
                const std::int32_t result =
                    ProcessConfirmationDialog_45DEE0(
                        320,
                        240,
                        L"要进行更新吗？" /* 原 0x55CD28: 更新确认提示。 */,
                        0,
                        384,
                        96);
                if (result == 1) {
                    world::SetCurrentSceneType(13);
                } else if (result == 2) {
                    world::gCurrentSceneState = 10;
                }
            } else if (mainState == 19u) {
                login::ResetLoginMessage();
                world::gCurrentSceneState = 20;
            } else if (mainState == 20u) {
                if (login::UpdateLoginMessage(L"这个帐号仅适用于资料片版本，请将用户端的版本更新至资料片。" /* 原 0x55C818: 资料片专用账号及客户端更新提示。 */) != 0) {
                    world::SetCurrentSceneType(2);
                }
            } else if (mainState == 21u) {
                if (ProcessCharacterModal_462090(320, 240) == 1) {
                    world::gCurrentSceneState = 10;
                }
            }
#ifndef _DEBUG
            else if (mainState == kUsageNoticeState) {
                const std::int32_t choice = ProcessUsageNotice();
                if (choice == 1) {
                    EnterSelectedCharacter();
                } else if (choice == 2) {
                    // CUN02：直接结束进程，跳过存在纹理寿命冲突的CRT静态析构。
                    std::_Exit(cg::platform::Rand16());
                }
            }
#endif
        }

        const std::uint32_t secondaryState = static_cast<std::uint32_t>(world::gCurrentSceneState);
        if (secondaryState <= 20u
            && (secondaryState <= 9u || secondaryState >= 19u)) {
            sceneResult = cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 3, 245060, 0, -1), true);
            break;
        }

        for (std::uint32_t slot = 0; slot < login::CharacterRecordCount; ++slot) {
            login::CharacterRecord& record =
                login::gCharacterRecords[static_cast<std::size_t>(slot)];
            const std::int32_t action =
                ProcessCharacterSlot_461B30(slot, interactive);
#ifndef _DEBUG
            if (world::gCurrentSceneState == kUsageNoticeState) {
                continue; // 原空槽按钮仍可能返回创建动作；模态等待时只绘制、不派发。
            }
#endif
            const std::uint32_t actionIndex = static_cast<std::uint32_t>(action);
            if (actionIndex > 6u) {
                continue;
            }

            if (actionIndex == 0u) {
                login::gSelectedCharacterIndex = static_cast<std::int16_t>(slot);
                world::Sub_518BE0();
                cg::world::detail::Sub_519290();
                cg::world::detail::Sub_5194C0();
                cg::world::detail::Sub_519300();
                cg::world::detail::Sub_519550();
                world::SetCurrentSceneType(4);
                login::detail::PlaySe(53, 320, 240);
                cg::yobi::YobiClearUp_404D30();
                ClearPreviewActions();
            } else if (actionIndex == 1u) {
                login::gSelectedCharacterIndex = static_cast<std::int16_t>(slot);
                // CUN01：连接阶段已校验索引；服务器记录保留地址映射前的名称。
                const char* const serverAddress = login::gValueD69798[
                    static_cast<std::size_t>(login::gSelectedServerIndex)].address.data();
                if (std::strcmp(serverAddress, "taomoli") == 0) {
                    EnterSelectedCharacter();
                } else {
                    world::gCurrentSceneState = kUsageNoticeState;
                    interactive = 1; // 同帧即锁住其他槽位，等待下一帧显示提示。
                }
            } else if (actionIndex == 2u) {
                login::gSelectedCharacterIndex = static_cast<std::int16_t>(slot);
                if (record.deleteFlag != 0) {
                    char name[kLegacyNameCapacity];
                    std::strcpy(name, record.name.data());
                    cg::core::NullSub_509080();
                    world::gCurrentSceneState = 12;
                    std::strcpy(state::gDeleteCharacterName, name);
                } else {
                    world::gCurrentSceneState = 21;
                }
                login::detail::PlaySe(55, 320, 240);
            } else if (actionIndex == 3u || actionIndex == 4u) {
                UpdateCharacterGraphic(record, static_cast<std::int32_t>(actionIndex));
                login::detail::PlaySe(51, 320, 240);
            } else if (actionIndex == 5u || actionIndex == 6u) {
                const std::int32_t value = record.previewState;
                if (value == 0) {
                    record.previewState = 1;
                } else if (value == 1) {
                    record.previewState = 0;
                }
                state::gPreviewToggleState =
                    record.previewState;
                ProcessCharacterPreview_460D00(slot);
                login::detail::PlaySe(51, 320, 240);
            }
        }

        std::int32_t uiStyle = interactive;
        if (gUpdateAvailable != 0) {
            if (uiStyle == 0
                && world::gCurrentSceneState == 11
                && (cg::ui::QueryUiControlInputState_45F100(&gUpdateUi) & 1) != 0) {
                world::SetCurrentSceneType(13);
                uiStyle = 1;
            }
            cg::ui::DrawUiControl_45EFD0(&gUpdateUi, 5, 0, 0, uiStyle);
            cg::render::Sub_45F090(252, 392, 4, 245078, 0, -1);
        }

        if (uiStyle == 0
            && world::gCurrentSceneState == 11
            && (cg::ui::QueryUiControlInputState_45F100(&gReturnUi) & 1) != 0) {
            cg::yobi::YobiClearUp_404D30();
            ClearPreviewActions();
            cg::network::ShutdownNetworkRuntime_5081A0();
            world::SetCurrentSceneType(2);
            login::detail::PlaySe(54, 320, 240);
        }
        cg::ui::DrawUiControl_45EFD0(&gReturnUi, 5, 0, 0, uiStyle);
        cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 3, 245060, 0, -1), true);

#ifndef _DEBUG
        if (world::gCurrentSceneState == kUsageNoticeState) {
            sceneResult = 0;
            break; // 等待时不执行页尾升级发送／状态复位，但仍经过VM_HARD_END。
        }
#endif

        sceneResult = gUpdateAvailable;
        if (sceneResult != 0) {
            const std::uint8_t pukVersion = state::gPukVersion;
            sceneResult = static_cast<std::int32_t>(pukVersion);
            if (pukVersion == 0u || pukVersion == 2u) {
                sceneResult = detail::Sub_512D30(
                    platform::gConnectionSocket, 2, state::kVersion20);
            } else if (pukVersion == 5u || pukVersion == 6u) {
                sceneResult = detail::Sub_512D30(
                    platform::gConnectionSocket, 4, state::kVersion30);
            } else if (pukVersion == 7u) {
                sceneResult = detail::Sub_512D30(
                    platform::gConnectionSocket, 5, detail::gVersionPayload9A5C10);
            }
            world::gCurrentSceneState = 0;
        }
    } while (false);
    VM_DOLPHIN_WHITE_END
    return sceneResult;
}
#pragma optimize("", on)

} // namespace cg::character_select
