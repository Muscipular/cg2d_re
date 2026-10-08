#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"
#include "platform/LegacyFormat.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include "cg/CharacterCreationRequest.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/Input.hpp"
#include "cg/LoginMessage.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/Palette.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyTime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/RendererResources.hpp"
#include "cg/world/SceneState.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::character_select::detail {
namespace {

// 54E0BC起十二条创建页矩形/图号；状态-1仍可读取前一项height。
constexpr std::array<CharacterSelectUiRecord, 12> kCreationUi54E0BC {{
    {{407, 130, 9, 13, 245066, 245067, 245068}},
    {{438, 130, 9, 13, 245063, 245064, 245065}},
    {{407, 148, 9, 13, 245066, 245067, 245068}},
    {{438, 148, 9, 13, 245063, 245064, 245065}},
    {{457, 150, 9, 13, 245066, 245067, 245068}},
    {{533, 150, 9, 13, 245063, 245064, 245065}},
    {{504, 199, 11, 11, 243006, 243007, 243008}},
    {{518, 199, 11, 11, 243003, 243004, 243005}},
    {{504, 307, 11, 11, 243006, 243007, 243008}},
    {{518, 307, 11, 11, 243003, 243004, 243005}},
    {{450, 372, 65, 17, 243048, 243049, 243050}},
    {{371, 372, 66, 17, 245045, 245046, 245047}},
}};

// 54DDE8为同表第14项，不另建状态或重复表。
constexpr std::array<std::int32_t, 84> kCreationMotion54DDB0 {{
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

constexpr std::array<std::int32_t, 42> kCreationGraphic55C354 {{
    240000, 240100, 240200, 240300, 240400, 240500, 240600,
    240700, 240800, 240900, 241000, 241100, 241200, 241300,
    241400, 241500, 241600, 241700, 241800, 241900, 242000,
    242100, 242200, 242300, 242400, 242500, 242600, 242700,
    205000, 205100, 205200, 205300, 205400, 205500, 270000,
    205700, 205800, 205900, 270100, 206100, 206200, 206300,
}};

constexpr std::array<std::array<std::int32_t, 2>, 14> kCreationSelectDraw54DF00 {{
    {{111, 169}}, {{111, 243}}, {{111, 317}}, {{179, 133}}, {{179, 207}},
    {{179, 281}}, {{179, 355}}, {{315, 169}}, {{315, 243}}, {{315, 317}},
    {{247, 133}}, {{247, 207}}, {{247, 281}}, {{247, 355}},
}};
constexpr std::array<std::array<std::int32_t, 2>, 14> kCreationSelectHit54DF70 {{
    {{79, 133}}, {{79, 207}}, {{79, 281}}, {{147, 97}}, {{147, 171}},
    {{147, 245}}, {{147, 319}}, {{283, 133}}, {{283, 207}}, {{283, 281}},
    {{215, 97}}, {{215, 171}}, {{215, 245}}, {{215, 319}},
}};
constexpr std::array<std::int32_t, 2> kCreationSelectSize54DFE0 {{64, 72}};
constexpr std::array<CharacterSelectUiRecord, 2> kCreationSelectButtons54DFE8 {{
    {{256, 400, 65, 17, 243048, 243049, 243050}},
    {{105, 400, 66, 17, 245045, 245046, 245047}},
}};
// 原54E020起是两个矩形、两组三图号及三版本标签，不是七元UI记录。
constexpr std::array<std::array<std::int32_t, 4>, 2> kCreationVersionHit54E020 {{
    {{222, 420, 18, 18}}, {{186, 420, 18, 18}},
}};
constexpr std::array<std::array<std::int32_t, 3>, 2> kCreationVersionImages54E040 {{
    {{243033, 243034, 243035}}, {{243030, 243031, 243032}},
}};
constexpr std::array<std::int32_t, 3> kCreationVersionLabels54E058 {{
    245481, 245482, 245483,
}};
constexpr std::array<std::array<std::int32_t, 4>, 2> kCreationPreviewHit54E064 {{
    {{455, 138, 125, 125}}, {{455, 291, 125, 125}},
}};
constexpr std::array<CharacterSelectUiRecord, 2> kCreationVariantButtons54E084 {{
    {{458, 268, 18, 18, 243030, 243031, 243032}},
    {{559, 268, 18, 18, 243033, 243034, 243035}},
}};
constexpr std::array<std::int32_t, 28> kCreationGraphic55C3FC {{
    202800, 202900, 203000, 203100, 203200, 203300, 203400,
    203500, 203600, 203700, 203800, 203900, 204000, 204100,
    200000, 200200, 200800, 201000, 201200, 201400, 201600,
    200400, 200600, 201800, 202000, 202200, 202400, 202600,
}};
constexpr std::array<std::int32_t, 28> kCreationGraphic55C46C {{
    30000, 34000, 38000, 46000, 42000, 50000, 54000,
    70000, 74000, 78000, 82000, 86000, 90000, 94000,
    33100, 33100, 33100, 33100, 33100, 33100, 33100,
    33101, 33101, 33101, 33101, 33101, 33101, 33101,
}};

constexpr char kCreationConfirm55C9B0[] = "\xBF\xC9\xD2\xD4\x86\xE1\xA3\xBF";
constexpr std::array<const char*, 6> kCreationErrors {{
    "\x9B\x5D\xD3\xD0\xDD\x94\xC8\xEB\xD0\xD5\xC3\xFB\xA3\xA1", // 55C9A0。
    "\xBD\xC7\xC9\xAB\xC3\xFB\xB7\x51\xD6\xD8\xD1\x7D", // 55C918。
    "\xD5\x88\xB0\xD1\xB3\xD6\xD3\xD0\xFC\x63\x94\xB5\xB7\xD6\xBD\x6F\xC4\xDC\xC1\xA6\xA3\xA1", // 55C968。
    "\xD5\x88\xB0\xD1\xB3\xD6\xD3\xD0\xFC\x63\x94\xB5\xB7\xD6\xBD\x6F\x8C\xD9\xD0\xD4\xA3\xA1", // 55C950。
    "\xB2\xBB\xC4\xDC\xCA\xB9\xD3\xC3\xBA\xAC\xD3\xD0\xBF\xD5\xB0\xD7\xCE\xC4\xD7\xD6\xB5\xC4\xC3\xFB\xD7\xD6\xBB\xF2\xCF\xB5\xBD\x79\xD7\xD6\xD4\xAA\xA3\xA1", // 55C928。
    "\xC8\xCB\xCE\xEF\xC3\xFB\xB7\x51\xB2\xBB\xBF\xC9\xBA\xAC\xD3\xD0\xB7\xC7\xB7\xA8\xD7\xD6\xD4\xAA\xA3\xA1", // 55CE14。
}};

std::int32_t CreationSignedByte(const std::uint8_t value)
{
    return std::bit_cast<std::int8_t>(value);
}

std::uint8_t CreationMouseByte(const std::uint32_t& value)
{
    return *reinterpret_cast<const std::uint8_t*>(&value);
}

// 两处模式0/1映射均在外调后重取变体/样式/颜色，不能提前快照。
std::int32_t MapCreationGraphic(const std::int32_t base, const std::int32_t mode)
{
    std::int32_t graphic = Sub_51BF00(base);
    if (mode == 0) {
        graphic = Sub_51BAC0(graphic);
    }
    const std::int32_t variant = CreationSignedByte(state::gValue9A5B00);
    const std::int32_t style = CreationSignedByte(state::gValue9A5668);
    const std::int32_t color = CreationSignedByte(state::gValue9A4B80);
    return graphic + color + (mode == 0
        ? 10 * (style + 100 * variant) : 5 * (style + 5 * variant));
}

// 原14个离散备份W32与9A4F38输入备份，仅4642C0/464380直接访问。
struct CreationSnapshot {
    std::int32_t value9A50A4;
    std::int32_t value9A4B98;
    std::int32_t value9A50A0;
    std::int32_t value9A5670;
    std::int32_t value9A4E98;
    std::int32_t value9A4E84;
    std::int32_t value9A50E4;
    std::int32_t value9A60E8;
    std::int32_t value9A5094;
    std::int32_t value9A57E8;
    std::int32_t value9A61F4;
    std::int32_t value9A554C;
    std::int32_t value9A5664;
    std::int32_t value9A5954;
    input::InputContext input9A4F38;
};

CreationSnapshot gCreationSnapshot {};

// 原五个离散S32和9A6E88观察槽，仅45E7B0/45E7D0使用。
struct CreationRequestProgress {
    std::int32_t phase; // 9A50E0。
    std::int32_t columns; // 9A565C。
    std::int32_t rows; // 9A566C。
    std::int32_t x; // 9A4E90。
    std::int32_t y; // 9A50D8。
    login::YobiAction* action; // 9A6E88，非拥有，完成时标死后清槽。
};

CreationRequestProgress gCreationRequestProgress {};
constexpr char kCreationProgress55C9C0[] = "\x84\x93\xD4\xEC\xBD\xC7\xC9\xAB\xD6\xD0";
static_assert(sizeof(kCreationProgress55C9C0) == 11);

std::uint8_t gCreationRestore9A6EB8 = 0; // 原创建场景同次调用的恢复回边标志。
constexpr char kCreationDuplicate55C980[] =
    "\xD2\xD1\xBD\x9B\xD3\xD0\xCF\xE0\xCD\xAC\xC3\xFB\xD7\xD6"
    "\xB5\xC4\xBD\xC7\xC9\xAB\xB4\xE6\xD4\xDA\xC1\xCB\xA3\xA1";
static_assert(sizeof(kCreationDuplicate55C980) == 29);

std::uint8_t ReadCreationSnapshotByte(const std::int32_t& value)
{
    return *reinterpret_cast<const std::uint8_t*>(&value);
}

} // namespace

std::uint8_t Sub_50E770(std::uintptr_t, const std::uint8_t pukVersion)
{
    state::gPukVersion = pukVersion;
    return pukVersion;
}

std::int32_t Sub_45E7B0()
{
    const std::int32_t selected = login::gSelectedCharacterIndex;
    gCreationRequestProgress.phase = 0;
    world::detail::Sub_5345A0(selected);
    return world::detail::Sub_534770();
}

std::int32_t Sub_45E7D0()
{
    char text[sizeof(kCreationProgress55C9C0)];
    std::memcpy(text, kCreationProgress55C9C0, sizeof(text));
    if (gCreationRequestProgress.phase == 0) {
        gCreationRequestProgress.phase = 1;
        const auto length = static_cast<std::uint32_t>(std::strlen(text));
        std::int32_t columns = std::bit_cast<std::int32_t>((9U * (length + 7U)) >> 6);
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
        gCreationRequestProgress.action = login::detail::Sub_5044E0(
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
            login::YobiAction* const action = gCreationRequestProgress.action;
            if (action != nullptr) {
                login::detail::MarkDeath(action);
                gCreationRequestProgress.action = nullptr;
            }
            return result;
        }
    }
    login::YobiAction* const action = gCreationRequestProgress.action;
    if (action != nullptr && login::detail::IsYobiActionReady(action, 1)) {
        const std::int32_t rows = gCreationRequestProgress.rows;
        const std::int32_t unscaledY = 48 * rows - 16;
        const std::int32_t top = gCreationRequestProgress.y;
        const std::int32_t y = unscaledY / 2 + top;
        const auto length = static_cast<std::uint32_t>(std::strlen(text));
        const std::int32_t columns = gCreationRequestProgress.columns;
        const auto textUnits = static_cast<std::int32_t>(length >> 1);
        const std::int32_t unscaledX = 64 * columns - 17 * textUnits;
        const std::int32_t left = gCreationRequestProgress.x;
        const std::int32_t x = unscaledX / 2 + left;
        login::detail::DrawText3(
            static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), 2, 0, text, 0);
    }
    return 0;
}

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
                login::Sub_51B3B0(static_cast<std::uint32_t>(selected));
                const std::int32_t unusedSelected = login::gSelectedCharacterIndex;
                static_cast<void>(unusedSelected);
                cg::NullSub_509080();
                Sub_520200();
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
                if (login::UpdateLoginMessage(kCreationDuplicate55C980) != 0) {
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

void Sub_462870()
{
    login::detail::YobiClearUp_404D30();
    ++world::gCurrentSceneState;
}

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

    login::YobiAction* const firstAction =
        login::detail::NewYobiAction_404AE0(2, 0);
    state::gCreateActions[0] = firstAction;
    if (firstAction != nullptr) {
        login::detail::SetYobiActionField30(firstAction, 517);
        login::detail::SetYobiActionField34(state::gCreateActions[0], 243);
        login::detail::SetYobiActionField2B8(state::gCreateActions[0], 5);
    }

    login::YobiAction* const secondAction =
        login::detail::NewYobiAction_404AE0(2, 0);
    state::gCreateActions[1] = secondAction;
    if (secondAction != nullptr) {
        login::detail::SetYobiActionField30(secondAction, 517);
        login::detail::SetYobiActionField34(state::gCreateActions[1], 396);
        login::detail::SetYobiActionField2B8(state::gCreateActions[1], 5);
    }
    input::SetContext_44B330(nullptr);
}

std::int32_t Sub_462930()
{
    std::uint8_t selected = state::gValue9A4CFC;
    render::GraphicDescriptor16 descriptor {};
    const std::uint32_t color = 0x3FFFFFFF;
    std::memcpy(descriptor.bytes.data() + 8, &color, sizeof(color));
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
        const std::int32_t version = CreationSignedByte(state::gValue9A57E0);
        const std::int32_t image = kCreationGraphic55C354[14 * version + index];
        if (image >= 206000 && image <= 206300) {
            if (CreationSignedByte(selected) >= 7 && selected != 10) {
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
        if (login::detail::Sub_5049A0(left, top, left + width, top + height, -1) != 0
            && (CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            state::gValue9A4CFC = static_cast<std::uint8_t>(index);
            state::gValue9A5B00 = 0;
            break;
        }
        selected = state::gValue9A4CFC;
    }
    if (Sub_464290(kCreationSelectButtons54DFE8[0].values.data()) != 0) {
        confirmState = hover();
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            audio::PlaySe_52C450(53, 320);
            result = 1;
        }
    }
    if (Sub_464290(kCreationSelectButtons54DFE8[1].values.data()) != 0) {
        cancelState = hover();
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            audio::PlaySe_52C450(54, 320);
            result = 2;
        }
    }
    if (state::gPukVersion >= 5) {
        if (Sub_464290(kCreationVersionHit54E020[0].data()) != 0) {
            versionPlus = hover();
            if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
                const std::uint8_t puk = state::gPukVersion;
                const std::int32_t version = CreationSignedByte(state::gValue9A57E0);
                const std::int32_t limit = puk >= 8 ? 2 : 1;
                if (version < limit) {
                    ++state::gValue9A57E0;
                } else {
                    state::gValue9A57E0 = static_cast<std::uint8_t>(limit);
                }
            }
        }
        if (Sub_464290(kCreationVersionHit54E020[1].data()) != 0) {
            versionMinus = hover();
            if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
                if (CreationSignedByte(state::gValue9A57E0) > 0) {
                    --state::gValue9A57E0;
                } else {
                    state::gValue9A57E0 = 0;
                }
            }
        }
    }
    if (Sub_464290(kCreationVariantButtons54E084[0].values.data()) != 0) {
        variantMinus = hover();
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            const auto next = static_cast<std::uint8_t>(state::gValue9A5B00 - 1);
            state::gValue9A5B00 = next;
            if (CreationSignedByte(next) < 0) {
                state::gValue9A5B00 = 3;
            }
        }
    }
    if (Sub_464290(kCreationVariantButtons54E084[1].values.data()) != 0) {
        variantPlus = hover();
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            const auto next = static_cast<std::uint8_t>(state::gValue9A5B00 + 1);
            state::gValue9A5B00 = next;
            if (CreationSignedByte(next) > 3) {
                state::gValue9A5B00 = 0;
            }
        }
    }

    if (Sub_464290(kCreationPreviewHit54E064[0].data()) != 0
        || Sub_464290(kCreationPreviewHit54E064[1].data()) != 0) {
        login::YobiAction* first;
        login::YobiAction* second;
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            first = state::gCreateActions[0];
            if (first != nullptr) {
                login::detail::SetYobiActionField2B8(
                    first, login::detail::GetYobiActionField2B8(first) - 1);
                first = state::gCreateActions[0];
                if (login::detail::GetYobiActionField2B8(first) < 0) {
                    login::detail::SetYobiActionField2B8(first, 7);
                    first = state::gCreateActions[0];
                }
            }
            second = state::gCreateActions[1];
            if (second != nullptr) {
                login::detail::SetYobiActionField2B8(
                    second, login::detail::GetYobiActionField2B8(second) - 1);
                second = state::gCreateActions[1];
                if (login::detail::GetYobiActionField2B8(second) < 0) {
                    login::detail::SetYobiActionField2B8(second, 7);
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
                login::detail::SetYobiActionField2B8(
                    first, login::detail::GetYobiActionField2B8(first) + 1);
                login::YobiAction* const current = state::gCreateActions[0];
                if (login::detail::GetYobiActionField2B8(current) > 7) {
                    login::detail::SetYobiActionField2B8(current, 0);
                }
                second = state::gCreateActions[1];
            }
            if (second != nullptr) {
                login::detail::SetYobiActionField2B8(
                    second, login::detail::GetYobiActionField2B8(second) + 1);
                second = state::gCreateActions[1];
                if (login::detail::GetYobiActionField2B8(second) > 7) {
                    login::detail::SetYobiActionField2B8(second, 0);
                }
            }
        }
    }

    const auto& selectedPosition = kCreationSelectDraw54DF00[CreationSignedByte(state::gValue9A4CFC)];
    login::detail::DrawGraphic(selectedPosition[0], selectedPosition[1], 10, 245042, 0, nullptr);
    if (render::HasLegacy3DDevice_44B610() != 0) {
        const auto& position = kCreationSelectDraw54DF00[CreationSignedByte(state::gValue9A4CFC)];
        login::detail::DrawGraphic(position[0], position[1], 10, 245043, 0, &descriptor);
    }
    login::detail::DrawGraphic(
        138, 409, 10, kCreationSelectButtons54DFE8[1].values[4 + cancelState], 0, nullptr);
    login::detail::DrawGraphic(
        289, 409, 10, kCreationSelectButtons54DFE8[0].values[4 + confirmState], 0, nullptr);
    if (state::gPukVersion >= 5) {
        login::detail::DrawGraphic(230, 429, 10, kCreationVersionImages54E040[0][versionPlus], 0, nullptr);
        login::detail::DrawGraphic(194, 429, 10, kCreationVersionImages54E040[1][versionMinus], 0, nullptr);
    }
    login::detail::DrawGraphic(
        203, 412, 10, kCreationVersionLabels54E058[CreationSignedByte(state::gValue9A57E0)], 0, nullptr);
    const std::uint8_t labelPuk = state::gPukVersion;
    login::detail::DrawGraphic(
        221, 412, 10, labelPuk < 5 ? 245481 : (labelPuk < 8 ? 245482 : 245483), 0, nullptr);

    const std::uint8_t listPuk = state::gPukVersion;
    const std::int32_t* const graphicTable = listPuk >= 7 ? kCreationGraphic55C354.data()
        : (listPuk >= 5 ? kCreationGraphic55C3FC.data() : kCreationGraphic55C46C.data());
    for (std::int32_t index = 0; index < 14; ++index) {
        const std::int32_t version = CreationSignedByte(state::gValue9A57E0);
        const auto& position = kCreationSelectDraw54DF00[index];
        login::detail::DrawGraphic(
            position[0], position[1], 10, graphicTable[14 * version + index], 0, nullptr);
    }
    login::detail::DrawGraphic(214, 254, 10, 245041, 0, nullptr);
    login::detail::DrawGraphic(
        467, 277, 10, kCreationVariantButtons54E084[0].values[4 + variantMinus], 0, nullptr);
    login::detail::DrawGraphic(
        568, 277, 10, kCreationVariantButtons54E084[1].values[4 + variantPlus], 0, nullptr);

    login::YobiAction* const second = state::gCreateActions[1];
    if (second != nullptr) {
        login::detail::SetYobiActionField15(second, 2);
        const std::int32_t version = CreationSignedByte(state::gValue9A57E0);
        const std::int32_t character = CreationSignedByte(state::gValue9A4CFC);
        const std::int32_t variant = CreationSignedByte(state::gValue9A5B00);
        const std::int32_t motion = kCreationMotion54DDB0[28 * version + character + 14] + 6 * variant;
        login::detail::SetYobiActionField2A8(state::gCreateActions[1], motion);
        login::detail::Sub_517880(state::gCreateActions[1], 100, 1, 0);
        if (state::gPukVersion >= 5 && state::gValue9A57E0 == 0) {
            const std::int32_t graphic = login::detail::GetYobiActionField20(state::gCreateActions[1]);
            render::DrawGraphic2_521DD0(517, 396, 10, graphic, 0, 0, nullptr);
        }
    }
    login::YobiAction* const first = state::gCreateActions[0];
    if (first != nullptr) {
        login::detail::SetYobiActionField15(first, 2);
        const std::uint8_t puk = state::gPukVersion;
        const std::int32_t version = CreationSignedByte(state::gValue9A57E0);
        const std::int32_t character = CreationSignedByte(state::gValue9A4CFC);
        std::int32_t motion;
        if (puk < 5) {
            const std::int32_t graphic = kCreationGraphic55C46C[14 * version + character];
            if (graphic == 33100 || graphic == 33101) {
                const std::int32_t variant = CreationSignedByte(state::gValue9A5B00);
                motion = (graphic == 33100 ? 100900 : 100901) + 6 * variant;
            } else {
                const std::int32_t variant = CreationSignedByte(state::gValue9A5B00);
                motion = kCreationMotion54DDB0[28 * version + character] + 6 * variant;
            }
        } else {
            const std::int32_t variant = CreationSignedByte(state::gValue9A5B00);
            motion = kCreationMotion54DDB0[28 * version + character] + 6 * variant;
        }
        login::detail::SetYobiActionField2A8(state::gCreateActions[0], motion);
        if (login::detail::Sub_517880(state::gCreateActions[0], 100, 1, 0) != 0) {
            login::detail::Sub_45E6E0(state::gCreateActions[0]);
            login::YobiAction* const currentSecond = state::gCreateActions[1];
            if (currentSecond != nullptr) {
                login::detail::Sub_45E6E0(currentSecond);
            }
        }
    }
    login::detail::DrawGraphic(518, 275, 1, 245044, 0, nullptr);
    login::detail::DrawGraphic(320, 240, 1, 245040, 0, nullptr);
    return result;
}

std::uint32_t Sub_463100()
{
    login::YobiAction* const first = state::gCreateActions[0];
    if (first != nullptr) {
        login::detail::MarkDeath(first);
    }
    login::YobiAction* const second = state::gCreateActions[1];
    if (second != nullptr) {
        login::detail::MarkDeath(second);
    }
    state::gValue9A5E78 = 0;
    state::gValue9A5FA4 = 0;
    state::gValue9A5FA0 = 0;
    input::InitializeContext_449CF0(
        &state::gCreationInput9A51E8, 352, 108, 0, 13, 131, nullptr, 1, 16, 0, 0);
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
    char buffer[32];

    std::int32_t total = CreationSignedByte(*attributes[3]);
    total += CreationSignedByte(*attributes[2]);
    total += CreationSignedByte(*attributes[4]);
    total += CreationSignedByte(*attributes[1]);
    total += CreationSignedByte(*attributes[0]);
    total += CreationSignedByte(state::gValue9A4D04);
    const std::int32_t attributeLimit = total >> 1;
    const std::uint8_t puk = state::gPukVersion;
    const std::uint8_t maxVersion = puk >= 7 ? 2 : (puk >= 5 ? 1 : 0);
    const bool showVersionControls = maxVersion != state::gValue9A57E0;

    const auto hover = []() -> std::int8_t {
        return (CreationMouseByte(platform::gLegacyMouseState.heldButtons) & 1) != 0 ? 1 : 2;
    };
    const auto hitRow = [](const std::size_t record, const std::int32_t top) {
        const auto& values = kCreationUi54E0BC[record].values;
        return login::detail::Sub_5049A0(
            values[0], top, values[0] + values[2], top + values[3], -1);
    };

    if (state::gValue9A5AFC == 1) {
        for (std::size_t index = 0; index < 4; ++index) {
            if (Sub_464290(kCreationUi54E0BC[index].values.data()) != 0) {
                arrows[index] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    auto& value = index < 2 ? state::gValue9A5668 : state::gValue9A4B80;
                    if ((index & 1) == 0) {
                        const auto next = static_cast<std::uint8_t>(value - 1);
                        value = next;
                        if (CreationSignedByte(next) < 0) {
                            value = 4;
                        }
                    } else {
                        const auto next = static_cast<std::uint8_t>(value + 1);
                        value = next;
                        if (CreationSignedByte(next) > 4) {
                            value = 0;
                        }
                    }
                    audio::PlaySe_52C450(51, 320);
                }
            }
        }
        if (showVersionControls) {
            if (Sub_464290(kCreationUi54E0BC[4].values.data()) != 0) {
                arrows[4] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    const std::uint8_t value = state::gValue9A4B88;
                    const std::uint8_t version = state::gValue9A57E0;
                    const auto next = static_cast<std::uint8_t>(value - 1);
                    state::gValue9A4B88 = next;
                    if (CreationSignedByte(next) < (version != 0 ? 1 : 0)) {
                        state::gValue9A4B88 = maxVersion;
                    }
                    audio::PlaySe_52C450(51, 320);
                }
            }
            if (Sub_464290(kCreationUi54E0BC[5].values.data()) != 0) {
                arrows[5] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    const std::uint8_t value = state::gValue9A4B88;
                    const std::uint8_t version = state::gValue9A57E0;
                    const auto next = static_cast<std::uint8_t>(value + 1);
                    state::gValue9A4B88 = next;
                    if (CreationSignedByte(next) > maxVersion) {
                        state::gValue9A4B88 = version != 0 ? 1 : 0;
                    }
                    audio::PlaySe_52C450(51, 320);
                }
            }
        }

        const std::int32_t remaining = CreationSignedByte(state::gValue9A4D04);
        for (std::size_t row = 0; row < attributes.size(); ++row) {
            const std::int32_t value = CreationSignedByte(*attributes[row]);
            if (value <= 0) {
                attributeMinus[row] = -1;
            }
            if (remaining <= 0 || value >= attributeLimit) {
                attributePlus[row] = -1;
            }
        }
        for (std::size_t row = 0; row < attributes.size(); ++row) {
            const std::int32_t top = kCreationUi54E0BC[6].values[1]
                + 14 * static_cast<std::int32_t>(row);
            if (CreationSignedByte(*attributes[row]) > 0 && hitRow(6, top) != 0) {
                attributeMinus[row] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    const std::uint8_t value = *attributes[row];
                    if (CreationSignedByte(value) > 0) {
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
            if (CreationSignedByte(state::gValue9A4D04) > 0
                && CreationSignedByte(*attributes[row]) < attributeLimit && hitRow(7, top) != 0) {
                attributePlus[row] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    bool changed = false;
                    if (CreationSignedByte(state::gValue9A4D04) > 0) {
                        const std::uint8_t value = *attributes[row];
                        if (CreationSignedByte(value) < attributeLimit) {
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
            const std::int32_t value = CreationSignedByte(elements[row]);
            if (value <= 0) {
                elementMinus[row] = -1;
            }
            if (value >= 10) {
                elementPlus[row] = -1;
            }
        }
        for (std::size_t row = 0; row < 4; ++row) {
            const std::int32_t top = kCreationUi54E0BC[8].values[1]
                + 14 * static_cast<std::int32_t>(row);
            if (CreationSignedByte(elements[row]) > 0 && hitRow(8, top) != 0) {
                elementMinus[row] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    const std::uint8_t value = elements[row];
                    if (CreationSignedByte(value) > 0) {
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
            if (CreationSignedByte(elements[row]) < 10 && hitRow(9, top) != 0) {
                elementPlus[row] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    const std::size_t opposite = row < 2 ? row + 2 : row - 2;
                    if (elements[opposite] == 0) {
                        if (CreationSignedByte(state::gValue9A50C8) > 0) {
                            const std::uint8_t value = elements[row];
                            const std::uint8_t left = state::gValue9A50C8;
                            elements[row] = static_cast<std::uint8_t>(value + 1);
                            state::gValue9A50C8 = static_cast<std::uint8_t>(left - 1);
                            audio::PlaySe_52C450(51, 320);
                        } else {
                            std::size_t donor = 0;
                            while (donor < 4
                                && (donor == row || CreationSignedByte(elements[donor]) <= 0)) {
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

        if (Sub_464290(kCreationUi54E0BC[10].values.data()) != 0) {
            confirmState = hover();
            if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
                const std::int32_t cursor = state::gCreationInput9A51E8.caretByteOffset;
                if (cursor <= 0) {
                    state::gValue9A5AFC = 100;
                } else if (CreationSignedByte(state::gValue9A4D04) > 0) {
                    state::gValue9A5AFC = 104;
                } else if (CreationSignedByte(state::gValue9A50C8) > 0) {
                    state::gValue9A5AFC = 106;
                } else {
                    char* const name = state::gCreationNameFF0D18.data();
                    std::strcpy(name, state::gCreationInput9A51E8.text);
                    name[cursor] = '\0';
                    if (login::CharacterNameExists_51B140(state::gCreationInput9A51E8.text) != 0) {
                        state::gValue9A5AFC = 102;
                    } else if (platform::StrStr_53BE40(name, " ") != nullptr
                        || platform::StrStr_53BE40(name, "\x81\x40") != nullptr
                        || input::IsRejectedInputText_404060(name)) {
                        state::gValue9A5AFC = 108;
                    } else if (input::Sub_45E770(name) == 1) {
                        state::gValue9A5AFC = 110;
                    } else {
                        state::gValue9A5AFC = 10;
                        audio::PlaySe_52C450(53, 320);
                    }
                }
            }
        }
        if (Sub_464290(kCreationUi54E0BC[11].values.data()) != 0) {
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
            320, 240, kCreationConfirm55C9B0, 0);
        if (answer == 1) {
            const std::int32_t version = CreationSignedByte(state::gValue9A57E0);
            const std::uint8_t mode = state::gValue9A4B88;
            result = 1;
            const std::int32_t character = CreationSignedByte(state::gValue9A4CFC);
            const std::int32_t variant = CreationSignedByte(state::gValue9A5B00);
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
                const std::int32_t style = CreationSignedByte(state::gValue9A5668);
                graphic = kCreationGraphic55C354[14 * version + character];
                const std::int32_t color = CreationSignedByte(state::gValue9A4B80);
                graphic += 5 * (5 * variant + style) + color;
            } else if (mode == 1 || mode == 0) {
                graphic = MapCreationGraphic(kCreationGraphic55C354[14 * version + character], mode);
            }

            const std::int32_t attribute0 = CreationSignedByte(*attributes[0]);
            const std::int32_t attribute1 = CreationSignedByte(*attributes[1]);
            const std::int32_t attribute2 = CreationSignedByte(*attributes[2]);
            state::gValueF633EC = attribute0;
            state::gValueF633F0 = attribute1;
            const std::int32_t attribute3 = CreationSignedByte(*attributes[3]);
            const std::int32_t attribute4 = CreationSignedByte(*attributes[4]);
            state::gValueDE0AF8 = attribute3;
            const std::uint32_t elementSnapshot = state::gValue9A50D0;
            state::gValueDE03F4 = attribute2;
            state::gValueF7CA00 = attribute4;
            const std::int32_t element2 = CreationSignedByte(elements[2]);
            state::gValueF7C948 = CreationSignedByte(static_cast<std::uint8_t>(elementSnapshot));
            state::gSelectedGraphicNo = graphic;
            const std::int32_t element3 = CreationSignedByte(elements[3]);
            state::gValueFF03D0 = CreationSignedByte(static_cast<std::uint8_t>(elementSnapshot >> 8));
            state::gValueF633F4 = element2;
            state::gValueDE0B04 = element3;
            world::detail::gValueFDA6F8 = 0;
            audio::Sub_52B990();
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
        login::detail::DrawGraphic(
            x, y, 10, kCreationUi54E0BC[record].values[4 + buttonState], 0, nullptr);
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
    if (CreationSignedByte(static_cast<std::uint8_t>(elementSnapshot)) > 0) {
        do {
            login::detail::DrawGraphic(406 + 10 * count, 313, 10, 245052, 0, nullptr);
            elementSnapshot = state::gValue9A50D0;
            ++count;
        } while (count < CreationSignedByte(static_cast<std::uint8_t>(elementSnapshot)));
    }
    count = 0;
    if (CreationSignedByte(static_cast<std::uint8_t>(elementSnapshot >> 8)) > 0) {
        do {
            login::detail::DrawGraphic(406 + 10 * count, 327, 10, 245053, 0, nullptr);
            const std::int32_t value = CreationSignedByte(elements[1]);
            ++count;
            if (count >= value) {
                break;
            }
        } while (true);
    }
    for (std::size_t row = 2; row < 4; ++row) {
        const std::int32_t value = CreationSignedByte(elements[row]);
        count = 0;
        if (value > 0) {
            do {
                login::detail::DrawGraphic(
                    406 + 10 * count, 313 + 14 * static_cast<std::int32_t>(row),
                    10, 245052 + static_cast<std::int32_t>(row), 0, nullptr);
                const std::int32_t current = CreationSignedByte(elements[row]);
                ++count;
                if (count >= current) {
                    break;
                }
            } while (true);
        }
    }
    // 与确认路径不同，此处未知模式沿用第四元素条的计数终值。
    std::int32_t graphic = count;
    const std::int32_t mode = CreationSignedByte(state::gValue9A4B88);
    if (mode == 2) {
        const std::int32_t version = CreationSignedByte(state::gValue9A57E0);
        const std::int32_t character = CreationSignedByte(state::gValue9A4CFC);
        const std::int32_t variant = CreationSignedByte(state::gValue9A5B00);
        const std::int32_t style = CreationSignedByte(state::gValue9A5668);
        graphic = kCreationGraphic55C354[14 * version + character];
        const std::int32_t color = CreationSignedByte(state::gValue9A4B80);
        graphic += 5 * (5 * variant + style) + color;
    } else if (mode == 1 || mode == 0) {
        const std::int32_t version = CreationSignedByte(state::gValue9A57E0);
        const std::int32_t character = CreationSignedByte(state::gValue9A4CFC);
        graphic = MapCreationGraphic(kCreationGraphic55C354[14 * version + character], mode);
    }
    if (CreationSignedByte(state::gValue9A4B88) > maxVersion) {
        graphic = CreationSignedByte(state::gValue9A4CFC) >= 14 ? 33101 : 33100;
    }
    login::detail::DrawGraphic(500, 126, 10, graphic, 0, nullptr);
    render::DrawInputText_4568A0(&state::gCreationInput9A51E8);
    constexpr std::int8_t color = std::bit_cast<std::int8_t>(std::uint8_t{131});
    platform::FormatLegacyString_53B304(buffer, "%02d", CreationSignedByte(state::gValue9A4D04));
    render::DrawText_456780(518, 185, 0, 13, color, buffer, 0, 0);
    for (std::size_t row = 0; row < attributes.size(); ++row) {
        platform::FormatLegacyString_53B304(buffer, "%02d", CreationSignedByte(*attributes[row]));
        render::DrawText_456780(
            479, static_cast<std::int16_t>(199 + 14 * row), 0, 13, color, buffer, 0, 0);
    }
    platform::FormatLegacyString_53B304(buffer, "%02d", CreationSignedByte(state::gValue9A50C8));
    render::DrawText_456780(518, 293, 0, 13, color, buffer, 0, 0);
    login::detail::DrawGraphic(442, 241, 10, 245051, 0, nullptr);
    login::detail::DrawGraphic(320, 240, 3, 245050, 0, nullptr);
    return result;
}

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
    std::memcpy(
        &gCreationSnapshot.input9A4F38, &state::gCreationInput9A51E8,
        sizeof(input::InputContext));
    gCreationSnapshot.value9A5664 = value9A5668;
    gCreationSnapshot.value9A5954 = value9A4B80;
    return value9A4B80;
}

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
    std::memcpy(
        &state::gCreationInput9A51E8, &gCreationSnapshot.input9A4F38,
        sizeof(input::InputContext));
    state::gValue9A4B80 = value9A5954;
    return value9A5954;
}

std::int32_t Sub_464290(const std::int32_t* const rectangle)
{
    const std::int32_t top = rectangle[1];
    const std::int32_t left = rectangle[0];
    const std::int32_t height = rectangle[3];
    const std::int32_t width = rectangle[2];
    return login::detail::Sub_5049A0(
        left, top, left + width, top + height, -1);
}

char gText9A4EAC[128] {};

} // namespace cg::character_select::detail

namespace cg::character_select::state {

std::uint8_t gPukVersion = 0;
std::uint8_t gGraphicVersionOverride58B6A0 = 0xFF;
std::uint8_t gValue9A57E0 = 0;
std::uint8_t gValue9A4CFC = 0;
std::uint8_t gValue9A5B00 = 0;
std::array<login::YobiAction*, 2> gCreateActions {};
input::InputContext gCreationInput9A51E8 {};
std::uint8_t gValue9A5E78 = 0;
std::int32_t gValue9A5FA4 = 0;
std::int32_t gValue9A5FA0 = 0;
std::uint32_t gValue9A5FD4 = 0;
std::uint32_t gValue9A50D0 = 0;
std::uint8_t gValue9A5FD8 = 0;
std::uint8_t gValue9A5668 = 0;
std::uint8_t gValue9A4B80 = 0;
std::uint8_t gValue9A4D04 = 0;
std::uint8_t gValue9A50C8 = 0;
std::uint8_t gValue9A4B88 = 0;
std::int32_t gValue9A5AFC = 0;
std::int32_t gValueF633F0 = 0;
std::int32_t gValueF7CA00 = 0;
std::int32_t gValueF633EC = 0;
std::int32_t gValueF633F4 = 0;
std::int32_t gValueDE0AF8 = 0;
std::int32_t gValueDE03F4 = 0;
std::int32_t gValueFF03D0 = 0;
std::int32_t gValueF7C948 = 0;
std::int32_t gValueDE0B04 = 0;
std::int32_t gValueDE0AFC = 0;
std::array<char, 24> gCreationNameFF0D18 {};
std::array<login::YobiAction*, 2> gPreviewActions {};
login::YobiAction* gCharacterModalAction = nullptr;
std::int16_t gPendingCharacterCount = 0;
std::int32_t gSelectedGraphicNo = 0;
std::uint16_t gCharacterEntryState = 0;
std::int32_t gPreviewToggleState = 0;
char gSelectedCharacterName[256] {};
char gDeleteCharacterName[132] {};

static_assert(sizeof(gCreateActions) == 2 * sizeof(login::YobiAction*));
static_assert(sizeof(gPreviewActions) == 2 * sizeof(login::YobiAction*));
static_assert(sizeof(gSelectedCharacterName) == 256);
static_assert(sizeof(gDeleteCharacterName) == 132);

} // namespace cg::character_select::state

namespace cg::character_select {
namespace {

// 原 0x55CD68: 角色资料读取失败提示。
constexpr char kCharacterDataReadFailed[] =
    "\xBD\xC7\xC9\xAB\xD9\x59\xC1\xCF\xD7\x78\xC8\xA1\xCA\xA7\x94\xA1\xA1\xA3";
// 原 0x55CD4C: 服务器维护提示。
constexpr char kServerMaintenance[] =
    "\xC4\xBF\xC7\xB0\xCB\xC5\xB7\xFE\xC6\xF7\xD5\xFD\xD4\xDA\xCD\xA3"
    "\x99\x43\xBE\x53\xD7\x6F\xD6\xD0\xA1\xA3";
// 原 0x55C854: 删除角色确认提示。
constexpr char kDeleteQuestion[] =
    "\xD5\xE6\xB5\xC4\xD2\xAA\x84\x68\xB3\xFD\x86\xE1\xA3\xBF";
// 原 0x55CD94: 按住 CTRL 删除角色提示。
constexpr char kDeleteWithControl[] =
    "\xCF\xEB\xD2\xAA\x84\x68\xB3\xFD\x95\x72\xA3\xAC\xD5\x88\xB0\xB4"
    "\xD6\xF8\x20\x43\x54\x52\x4C\xE6\x49\xB2\xBB\xB7\xC5";
// 原 0x55CD7C: 再次点击“是”的提示，保留八个尾空格。
constexpr char kClickYesAgain[] =
    "\xD4\xD9\xFC\x63\xDF\x78\xA1\xBA\xCA\xC7\xA1\xBB\xA1\xA3"
    "\x20\x20\x20\x20\x20\x20\x20\x20";
// 原 0x55CD38: 无法删除角色提示。
constexpr char kCannotDeleteCharacter[] =
    "\x9F\x6F\xB7\xA8\x84\x68\xB3\xFD\xB4\xCB\xBD\xC7\xC9\xAB\xA1\xA3";
// 原 0x55CD28: 更新确认提示。
constexpr char kUpdateQuestion[] =
    "\xD2\xAA\xDF\x4D\xD0\xD0\xB8\xFC\xD0\xC2\x86\xE1\xA3\xBF";
// 原 0x55C818: 资料片专用账号及客户端更新提示。
constexpr char kExpansionOnlyAccount[] =
    "\xDF\x40\x82\x80\x8E\xA4\xCC\x96\x83\x48\xDF\x6D\xD3\xC3\xEC\xB6"
    "\xD9\x59\xC1\xCF\xC6\xAC\xB0\xE6\xB1\xBE\xA3\xAC\xD5\x88\x8C\xA2"
    "\xD3\xC3\x91\xF4\xB6\xCB\xB5\xC4\xB0\xE6\xB1\xBE\xB8\xFC\xD0\xC2"
    "\xD6\xC1\xD9\x59\xC1\xCF\xC6\xAC\xA1\xA3";

static_assert(sizeof(kCharacterDataReadFailed) == 19);
static_assert(sizeof(kServerMaintenance) == 27);
static_assert(sizeof(kDeleteQuestion) == 15);
static_assert(sizeof(kDeleteWithControl) == 30);
static_assert(sizeof(kClickYesAgain) == 23);
static_assert(sizeof(kCannotDeleteCharacter) == 17);
static_assert(sizeof(kUpdateQuestion) == 15);
static_assert(sizeof(kExpansionOnlyAccount) == 59);

constexpr std::size_t kNameOffset = 0x00;
constexpr std::size_t kGraphicNoOffset = 0x18;
constexpr std::size_t kDeleteFlagOffset = 0x20;
constexpr std::size_t kGraphicClassOffset = 0x24;
constexpr std::size_t kPreviewToggleOffset = 0x50;
constexpr std::size_t kLegacyNameCapacity = 20;

static_assert(kNameOffset == 0);
static_assert(kGraphicNoOffset + sizeof(std::int32_t) <= login::CharacterRecordSize);
static_assert(kDeleteFlagOffset + sizeof(std::int32_t) <= login::CharacterRecordSize);
static_assert(kGraphicClassOffset + sizeof(std::int32_t) <= login::CharacterRecordSize);
static_assert(kPreviewToggleOffset + sizeof(std::int32_t) <= login::CharacterRecordSize);

detail::CharacterSelectUiRecord gReturnUi {
    {534, 438, 64, 16, 245045, 245047, 245046},
};
detail::CharacterSelectUiRecord gUpdateUi {
    {296, 433, 49, 17, 244542, 244544, 244543},
};

// 原 0x9A6EAC 与 0x9A6EB0 只被本控制器直接持有。
std::int32_t gUpdateAvailable = 0;
login::YobiAction* gDeleteConfirmationAction = nullptr;

std::uint32_t UnsignedBits(std::int32_t value)
{
    return std::bit_cast<std::uint32_t>(value);
}

std::int32_t ReadRecordInt32(
    const login::CharacterRecordBytes& record,
    std::size_t offset)
{
    std::int32_t value;
    std::memcpy(&value, record.data() + offset, sizeof(value));
    return value;
}

void WriteRecordInt32(
    login::CharacterRecordBytes& record,
    std::size_t offset,
    std::int32_t value)
{
    std::memcpy(record.data() + offset, &value, sizeof(value));
}

const char* ReadRecordName(const login::CharacterRecordBytes& record)
{
    // 与原 strcpy 相同，调用前提是 +0x00 处存在适合 20 字节临时区的 NUL 文本。
    return reinterpret_cast<const char*>(record.data() + kNameOffset);
}

void CopyRecordName(char* destination, const login::CharacterRecordBytes& record)
{
    char name[kLegacyNameCapacity];
    std::strcpy(name, ReadRecordName(record));
    cg::NullSub_509080();
    std::strcpy(destination, name);
}

void ClearPreviewActions()
{
    if (state::gPreviewActions[0] != nullptr) {
        state::gPreviewActions[0] = nullptr;
    }
    if (state::gPreviewActions[1] != nullptr) {
        state::gPreviewActions[1] = nullptr;
    }
}

void UpdateCharacterGraphic(
    login::CharacterRecordBytes& record,
    std::int32_t action)
{
    const std::uint8_t pukVersion = state::gPukVersion;
    if (pukVersion >= 7u) {
        const std::int32_t imageNo = ReadRecordInt32(record, kGraphicNoOffset);
        if (imageNo == 33100 || imageNo == 33101) {
            return;
        }

        if (imageNo >= 30000 && imageNo < 98000) {
            WriteRecordInt32(record, kGraphicClassOffset, 0);
        } else if (imageNo >= 202800 && imageNo < 204200) {
            WriteRecordInt32(record, kGraphicClassOffset, 1);
        } else if (imageNo >= 200000 && imageNo < 202700) {
            WriteRecordInt32(record, kGraphicClassOffset, 1);
        } else if ((imageNo >= 240000 && imageNo <= 242800)
            || (imageNo >= 205000 && imageNo <= 205600)
            || (imageNo >= 270000 && imageNo <= 270200)) {
            WriteRecordInt32(record, kGraphicClassOffset, 2);
        }

        const std::int32_t graphicClass =
            ReadRecordInt32(record, kGraphicClassOffset);
        const std::int32_t selector = graphicClass + 2 * action - 7;

        if (graphicClass == 0) {
            if (selector == 1) {
                WriteRecordInt32(
                    record,
                    kGraphicNoOffset,
                    detail::Sub_51BB80(ReadRecordInt32(record, kGraphicNoOffset)));
            } else {
                const std::int32_t first = detail::Sub_51BB80(
                    ReadRecordInt32(record, kGraphicNoOffset));
                WriteRecordInt32(record, kGraphicNoOffset, first);
                WriteRecordInt32(
                    record,
                    kGraphicNoOffset,
                    detail::Sub_51BFF0(first));
            }
        } else if (graphicClass == 1) {
            if (selector == 2) {
                WriteRecordInt32(
                    record,
                    kGraphicNoOffset,
                    detail::Sub_51BFF0(ReadRecordInt32(record, kGraphicNoOffset)));
            } else if (selector == 0) {
                WriteRecordInt32(
                    record,
                    kGraphicNoOffset,
                    imageNo >= 200000 && imageNo < 202700
                        ? detail::Sub_51BFF0(ReadRecordInt32(record, kGraphicNoOffset))
                        : detail::Sub_51BAC0(ReadRecordInt32(record, kGraphicNoOffset)));
            }
        } else if (graphicClass == 2) {
            if (selector == 3 && imageNo >= 240000 && imageNo < 241400) {
                const std::int32_t first = detail::Sub_51BF00(
                    ReadRecordInt32(record, kGraphicNoOffset));
                WriteRecordInt32(record, kGraphicNoOffset, first);
                WriteRecordInt32(
                    record,
                    kGraphicNoOffset,
                    detail::Sub_51BAC0(first));
            } else {
                WriteRecordInt32(
                    record,
                    kGraphicNoOffset,
                    detail::Sub_51BF00(ReadRecordInt32(record, kGraphicNoOffset)));
            }
        }
        return;
    }

    if (pukVersion < 5u) {
        return;
    }

    const std::int32_t imageNo = ReadRecordInt32(record, kGraphicNoOffset);
    if (imageNo == 33100 || imageNo == 33101) {
        return;
    }

    if (imageNo >= 30000 && imageNo < 98000) {
        WriteRecordInt32(record, kGraphicClassOffset, 0);
    } else if (imageNo >= 202800 && imageNo < 204200) {
        WriteRecordInt32(record, kGraphicClassOffset, 1);
    }

    const std::int32_t graphicClass = ReadRecordInt32(record, kGraphicClassOffset);
    if (graphicClass == 0) {
        WriteRecordInt32(
            record,
            kGraphicNoOffset,
            detail::Sub_51BB80(ReadRecordInt32(record, kGraphicNoOffset)));
    } else if (graphicClass == 1) {
        WriteRecordInt32(
            record,
            kGraphicNoOffset,
            detail::Sub_51BAC0(ReadRecordInt32(record, kGraphicNoOffset)));
    }
}

} // namespace

std::int32_t ProcessScene_460FF0()
{
    std::int32_t interactive = 1;

    const std::uint32_t mainState = UnsignedBits(world::gCurrentSceneState);
    if (mainState <= 21u) {
        switch (mainState) {
        case 0:
            world::gCurrentSceneState = 1;
            state::gPukVersion = 0;
            ResetCharacterListProgress_45DD50();
            gUpdateAvailable = 0;
            render::SelectPalette(0, 0);
            break;
        case 1: {
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
            break;
        }
        case 2:
            login::ResetLoginMessage();
            world::gCurrentSceneState = 6;
            break;
        case 3:
            break;
        case 4:
            login::ResetLoginMessage();
            world::gCurrentSceneState = 5;
            break;
        case 5:
            if (login::UpdateLoginMessage(kCharacterDataReadFailed) != 0) {
                world::SetCurrentSceneType(2);
            }
            break;
        case 6:
            if (login::UpdateLoginMessage(kServerMaintenance) != 0) {
                world::SetCurrentSceneType(2);
            }
            break;
        case 7:
            ResetAuthenticationFailure_45D7D0();
            world::gCurrentSceneState = 8;
            break;
        case 8:
            if (UpdateAuthenticationFailure_45D7E0() != 0) {
                world::SetCurrentSceneType(2);
            }
            break;
        case 9: {
            const std::int32_t capability = resource::ProbePaletteCapability_45C970();
            const std::uint8_t pukVersion = state::gPukVersion;
            render::gPaletteCapability = capability;
            if (pukVersion == 1u) {
                world::gCurrentSceneState = 10;
                break;
            }
            if ((capability < 2 && pukVersion >= 5u)
                || (capability < 1 && pukVersion >= 3u)
                || (capability < 3 && pukVersion >= 7u)
                || (capability < 4 && pukVersion >= 8u)) {
                world::gCurrentSceneState = 19;
                break;
            }

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
            break;
        }
        case 10:
            world::gCurrentSceneState = 11;
            ProcessCharacterPreview_460D00(0);
            ProcessCharacterPreview_460D00(1);
            break;
        case 11:
            interactive = 0;
            break;
        case 12: {
            const std::int32_t result =
                ProcessCharacterDeletionWarning_45E0C0(320, 240);
            if (result == 1) {
                world::gCurrentSceneState = 13;
                gDeleteConfirmationAction =
                    login::detail::Sub_5044E0(130, 300, 340, 52, 4, 1);
            } else if (result == 2) {
                world::gCurrentSceneState = 10;
            }
            break;
        }
        case 13: {
            const std::int32_t result =
                ProcessDefaultConfirmationDialog_45E090(
                    320,
                    240,
                    kDeleteQuestion,
                    0);
            if (login::detail::IsYobiActionReady(gDeleteConfirmationAction, 1)) {
                login::detail::DrawText3(166, 326, 2, 4, kDeleteWithControl, 0);
                login::detail::DrawText3(166, 346, 2, 4, kClickYesAgain, 0);
                if (result == 1) {
                    if ((platform::gKeyStates[17] & 2u) != 0) {
                        world::gCurrentSceneState = 14;
                        login::detail::PlaySe(284, 320, 240);
                        if (gDeleteConfirmationAction != nullptr) {
                            login::detail::MarkDeath(gDeleteConfirmationAction);
                            gDeleteConfirmationAction = nullptr;
                        }
                        ResetCharacterDeletionProgress_45DBF0();
                        world::gCurrentSceneState = 14;
                    }
                } else if (result == 2) {
                    world::gCurrentSceneState = 10;
                    if (gDeleteConfirmationAction != nullptr) {
                        login::detail::MarkDeath(gDeleteConfirmationAction);
                        gDeleteConfirmationAction = nullptr;
                    }
                }
            }
            break;
        }
        case 14: {
            const std::int32_t result = UpdateCharacterDeletionProgress_45DC00();
            if (result == 1) {
                detail::Sub_51B3B0(static_cast<std::uint32_t>(
                    static_cast<std::int32_t>(login::gSelectedCharacterIndex)));
                const std::int16_t unusedSelected = login::gSelectedCharacterIndex;
                static_cast<void>(unusedSelected);
                cg::NullSub_509080();
                detail::Sub_520200();
                if (state::gPendingCharacterCount > 0) {
                    --state::gPendingCharacterCount;
                }
                world::gCurrentSceneState = 10;
            } else if (result == -2) {
                world::gCurrentSceneState = 15;
            }
            break;
        }
        case 15:
            login::ResetLoginMessage();
            world::gCurrentSceneState = 16;
            break;
        case 16:
            if (login::UpdateLoginMessage(kCannotDeleteCharacter) != 0) {
                world::gCurrentSceneState = 0;
            }
            break;
        case 17:
            login::ResetLoginMessage();
            world::gCurrentSceneState = 18;
            break;
        case 18: {
            const std::int32_t result =
                ProcessConfirmationDialog_45DEE0(
                    320,
                    240,
                    kUpdateQuestion,
                    0,
                    384,
                    96);
            if (result == 1) {
                world::SetCurrentSceneType(13);
            } else if (result == 2) {
                world::gCurrentSceneState = 10;
            }
            break;
        }
        case 19:
            login::ResetLoginMessage();
            world::gCurrentSceneState = 20;
            break;
        case 20:
            if (login::UpdateLoginMessage(kExpansionOnlyAccount) != 0) {
                world::SetCurrentSceneType(2);
            }
            break;
        case 21:
            if (ProcessCharacterModal_462090(320, 240) == 1) {
                world::gCurrentSceneState = 10;
            }
            break;
        default:
            break;
        }
    }

    const std::uint32_t secondaryState = UnsignedBits(world::gCurrentSceneState);
    if (secondaryState <= 20u
        && (secondaryState <= 9u || secondaryState >= 19u)) {
        return detail::Sub_45F090(0, 0, 3, 245060, 0, -1);
    }

    for (std::uint32_t slot = 0; slot < login::CharacterRecordCount; ++slot) {
        login::CharacterRecordBytes& record =
            login::gCharacterRecords[static_cast<std::size_t>(slot)];
        const std::int32_t action =
            ProcessCharacterSlot_461B30(slot, interactive);
        const std::uint32_t actionIndex = UnsignedBits(action);
        if (actionIndex > 6u) {
            continue;
        }

        switch (actionIndex) {
        case 0:
            login::gSelectedCharacterIndex = static_cast<std::int16_t>(slot);
            world::detail::Sub_518BE0();
            detail::Sub_519290();
            detail::Sub_5194C0();
            detail::Sub_519300();
            detail::Sub_519550();
            world::SetCurrentSceneType(4);
            login::detail::PlaySe(53, 320, 240);
            login::detail::YobiClearUp_404D30();
            ClearPreviewActions();
            break;
        case 1: {
            login::gSelectedCharacterIndex = static_cast<std::int16_t>(slot);
            CopyRecordName(state::gSelectedCharacterName, record);
            const std::int16_t selected = login::gSelectedCharacterIndex;
            const login::CharacterRecordBytes* const selectedRecord =
                login::gCharacterRecords.data() + selected;
            state::gSelectedGraphicNo = ReadRecordInt32(*selectedRecord, kGraphicNoOffset);
            world::SetCurrentSceneType(5);
            login::detail::PlaySe(59, 320, 240);
            state::gCharacterEntryState = 0;
            detail::Sub_52B990();
            login::detail::YobiClearUp_404D30();
            ClearPreviewActions();
            break;
        }
        case 2: {
            login::gSelectedCharacterIndex = static_cast<std::int16_t>(slot);
            if (ReadRecordInt32(record, kDeleteFlagOffset) != 0) {
                char name[kLegacyNameCapacity];
                std::strcpy(name, ReadRecordName(record));
                cg::NullSub_509080();
                world::gCurrentSceneState = 12;
                std::strcpy(state::gDeleteCharacterName, name);
            } else {
                world::gCurrentSceneState = 21;
            }
            login::detail::PlaySe(55, 320, 240);
            break;
        }
        case 3:
        case 4:
            UpdateCharacterGraphic(record, static_cast<std::int32_t>(actionIndex));
            login::detail::PlaySe(51, 320, 240);
            break;
        case 5:
        case 6: {
            const std::int32_t value = ReadRecordInt32(record, kPreviewToggleOffset);
            if (value == 0) {
                WriteRecordInt32(record, kPreviewToggleOffset, 1);
            } else if (value == 1) {
                WriteRecordInt32(record, kPreviewToggleOffset, 0);
            }
            state::gPreviewToggleState =
                ReadRecordInt32(record, kPreviewToggleOffset);
            ProcessCharacterPreview_460D00(slot);
            login::detail::PlaySe(51, 320, 240);
            break;
        }
        default:
            break;
        }
    }

    std::int32_t uiStyle = interactive;
    if (gUpdateAvailable != 0) {
        if (uiStyle == 0
            && world::gCurrentSceneState == 11
            && (detail::Sub_45F100(&gUpdateUi) & 1) != 0) {
            world::SetCurrentSceneType(13);
            uiStyle = 1;
        }
        detail::Sub_45EFD0(&gUpdateUi, 5, 0, 0, uiStyle);
        detail::Sub_45F090(252, 392, 4, 245078, 0, -1);
    }

    if (uiStyle == 0
        && world::gCurrentSceneState == 11
        && (detail::Sub_45F100(&gReturnUi) & 1) != 0) {
        login::detail::YobiClearUp_404D30();
        ClearPreviewActions();
        cg::network::ShutdownNetworkRuntime_5081A0();
        world::SetCurrentSceneType(2);
        login::detail::PlaySe(54, 320, 240);
    }
    detail::Sub_45EFD0(&gReturnUi, 5, 0, 0, uiStyle);
    detail::Sub_45F090(0, 0, 3, 245060, 0, -1);

    std::int32_t result = gUpdateAvailable;
    if (result != 0) {
        const std::uint8_t pukVersion = state::gPukVersion;
        result = static_cast<std::int32_t>(pukVersion);
        switch (pukVersion) {
        case 0:
        case 2:
            result = detail::Sub_512D30(
                platform::gConnectionSocket, 2, state::kVersion20);
            break;
        case 5:
        case 6:
            result = detail::Sub_512D30(
                platform::gConnectionSocket, 4, state::kVersion30);
            break;
        case 7:
            result = detail::Sub_512D30(
                platform::gConnectionSocket, 5, detail::gVersionPayload9A5C10);
            break;
        default:
            break;
        }
        world::gCurrentSceneState = 0;
    }
    return result;
}

} // namespace cg::character_select
