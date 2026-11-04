#include "cg/network/CgConnection.hpp"
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
#include "CharacterSelectSceneState.hpp"
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
static std::int32_t MapCreationGraphic(
    const CharacterCreationDraftState& draft, const std::int32_t base, const std::int32_t mode)
{
    std::int32_t graphic = ConvertModernCharacterGraphicToEarlier_51BF00(base);
    if (mode == 0) {
        graphic = ConvertCompactCharacterGraphicToClassic_51BAC0(graphic);
    }
    const std::int32_t variant = static_cast<std::int8_t>(draft.variantIndex9A5B00);
    const std::int32_t style = static_cast<std::int8_t>(draft.styleIndex9A5668);
    const std::int32_t color = static_cast<std::int8_t>(draft.colorIndex9A4B80);
    return graphic + color + (mode == 0
        ? 10 * (style + 100 * variant) : 5 * (style + 5 * variant));
}

static std::uint8_t ReadCreationSnapshotByte(const std::int32_t& value)
{
    return *reinterpret_cast<const std::uint8_t*>(&value);
}

// 原函数：0x0050E770。
std::uint8_t HandlePukVersionReply_50E770(cg::network::CgConnection& networkConnection, const std::int32_t pukVersion)
{
    const std::uint8_t version = static_cast<std::uint8_t>(pukVersion);
    networkConnection.PukVersion() = version;
    return version;
}

// 原函数：0x0045E7B0。
std::int32_t ResetCharacterCreationRequest_45E7B0(network::CgConnection& connection)
{
    const std::int32_t selected = connection.LoginRequests().selectedCharacterIndex;
    connection.CharacterSelectUi().creationProgress.phase = 0;
    world::detail::ResetCharacterProfile_5345A0(connection.Owner(), selected);
    return world::detail::SavePlayerProfilesFile_534770(connection.Owner());
}

// 原函数：0x0045E7D0。
std::int32_t UpdateCharacterCreationRequest_45E7D0(network::CgConnection& connection)
{
    auto& progress = connection.CharacterSelectUi().creationProgress;
    const std::wstring_view text = L"创造角色中" /* 原常量参考：0x0055C9C0。 */;
    if (progress.phase == 0) {
        progress.phase = 1;
        const auto textWidth = static_cast<std::uint32_t>(cg::render::TextMeasure(text, 0));
        std::int32_t columns = std::bit_cast<std::int32_t>((textWidth + 63U) >> 6);
        progress.columns = columns;
        if (columns < 2) {
            columns = 2;
            progress.columns = 2;
        }
        const std::int32_t unscaledX = (10 - columns) * 64;
        const std::int32_t width = columns * 64;
        const std::int32_t x = unscaledX / 2;
        progress.rows = 2;
        progress.x = x;
        progress.y = 192;
        progress.action = cg::yobi::CreateModalAction_5044E0(
            connection.Owner(), x, 192, width, 96, 4, 1);
    }
    if (progress.phase == 1) {
        login::StartCharacterCreationRequest(connection);
        ++progress.phase;
    }
    if (progress.phase == 2) {
        const std::int32_t status = login::PollCharacterCreationRequest(connection);
        if (status == 1 || status < 0) {
            const std::int32_t result = status == 1 ? 1 : 2;
            cg::yobi::YobiAction* const action = progress.action;
            if (action != nullptr) {
                cg::yobi::MarkDeath(action);
                progress.action = nullptr;
            }
            return result;
        }
    }
    cg::yobi::YobiAction* const action = progress.action;
    if (action != nullptr && cg::yobi::IsYobiActionReady(action, 1)) {
        const std::int32_t rows = progress.rows;
        const std::int32_t unscaledY = 48 * rows - 16;
        const std::int32_t top = progress.y;
        const std::int32_t y = unscaledY / 2 + top;
        const auto textWidth = static_cast<std::uint32_t>(cg::render::TextMeasure(text, 0));
        const std::int32_t columns = progress.columns;
        const std::int32_t unscaledX = 64 * columns - static_cast<std::int32_t>(textWidth);
        const std::int32_t left = progress.x;
        const std::int32_t x = unscaledX / 2 + left;
        cg::render::DrawText3_456560(
            static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), 2, 0, text, 0);
    }
    return 0;
}

// 原函数：0x004626A0。
void ProcessCreationScene_4626A0(network::CgConnection& connection, const std::int32_t entryGate)
{
    auto& sceneUi = connection.CharacterSelectUi();
    std::int32_t sceneState = connection.Owner().WorldScene().sceneState;
    if (sceneState == 0) {
        AdvanceCharacterCreationScene_462870(connection);
        sceneState = connection.Owner().WorldScene().sceneState;
    }
    std::int32_t gate = entryGate;
    bool selectCharacter = false;
    if (sceneState == 1) {
        InitializeCharacterAppearanceSelection_462880(connection, 1);
        connection.Owner().WorldScene().sceneState = 3;
        selectCharacter = true;
    } else {
        if (sceneState == 2) {
            InitializeCharacterAppearanceSelection_462880(connection, 0);
            sceneState = connection.Owner().WorldScene().sceneState;
            ++sceneState;
            connection.Owner().WorldScene().sceneState = sceneState;
        }
        selectCharacter = sceneState == 3;
    }
    if (selectCharacter) {
        gate = UpdateCharacterAppearanceSelection_462930(connection);
        if (gate == 1) {
            sceneState = 10;
            connection.Owner().WorldScene().sceneState = sceneState;
        } else {
            if (gate == 2) {
                world::SetCurrentSceneTypeAndState(connection.Owner(), 3, 10);
            }
            sceneState = connection.Owner().WorldScene().sceneState;
        }
    }

    // 原462726回边只重做后半分派，必须保留当前局部状态和ESI门禁。
    for (;;) {
        bool editCharacter = false;
        bool skipRemainingStages = false;
        if (sceneState == 10 && gate != 1) {
            skipRemainingStages = true;
        } else if (sceneState == 10 || sceneState == 11) {
            InitializeCharacterCreationAttributes_463100(connection);
            connection.Owner().WorldScene().sceneState = 12;
            editCharacter = true;
        } else if (sceneState == 12) {
            editCharacter = true;
        }
        if (editCharacter) {
            if (sceneUi.creationRestore == 1) {
                RestoreCharacterCreationDraftSnapshot_464380(connection);
                sceneUi.creationRestore = 0;
            }
            gate = UpdateCharacterCreationAttributes_4631D0(connection);
            if (gate == 1) {
                const std::int32_t selected = connection.LoginRequests().selectedCharacterIndex;
                connection.Owner().WorldScene().sceneState = 30;
                cg::login::ClearCharacterSlot_51B3B0(connection, static_cast<std::uint32_t>(selected));
                const std::int32_t unusedSelected = connection.LoginRequests().selectedCharacterIndex;
                static_cast<void>(unusedSelected);
                cg::core::NullSub_509080();
                cg::login::detail::SaveCurrentLoginRecord_520200();
                input::SetContext_44B330(connection.Owner(), nullptr);
                sceneState = connection.Owner().WorldScene().sceneState;
            } else if (gate == 2) {
                sceneState = gate;
                connection.Owner().WorldScene().sceneState = sceneState;
                skipRemainingStages = true;
            } else {
                sceneState = connection.Owner().WorldScene().sceneState;
            }
        }
        if (!skipRemainingStages) {
            bool processRequest = false;
            if (sceneState == 30) {
                ResetCharacterCreationRequest_45E7B0(connection);
                connection.Owner().WorldScene().sceneState = 31;
                processRequest = true;
            } else if (sceneState == 31) {
                processRequest = true;
            }
            if (processRequest) {
                gate = UpdateCharacterCreationRequest_45E7D0(connection);
                if (gate == 1) {
                    sceneUi.pendingCharacterCount =
                        static_cast<std::int16_t>(sceneUi.pendingCharacterCount + 1);
                    world::SetCurrentSceneType(connection.Owner(), 3);
                    sceneUi.characterEntryState = 1;
                    sceneState = connection.Owner().WorldScene().sceneState;
                } else if (gate == 2) {
                    connection.Owner().WorldScene().sceneState = 100;
                    sceneState = 100;
                } else {
                    sceneState = connection.Owner().WorldScene().sceneState;
                }
            }
            if (sceneState == 100) {
                login::ResetLoginMessage(connection);
                sceneState = connection.Owner().WorldScene().sceneState;
                ++sceneState;
                connection.Owner().WorldScene().sceneState = sceneState;
            }
            if (sceneState == 101) {
                if (login::UpdateLoginMessage(connection, L"已经有相同名字的角色存在了！" /* 原常量参考：0x0055C980。 */) != 0) {
                    sceneState = 10;
                    sceneUi.creationRestore = 1;
                    connection.Owner().WorldScene().sceneState = sceneState;
                    gate = 1;
                    continue;
                }
                sceneState = connection.Owner().WorldScene().sceneState;
            }
        }
        if (sceneUi.creationRestore != 1) {
            return;
        }
    }
}

// 原函数：0x00462870。
void AdvanceCharacterCreationScene_462870(network::CgConnection& connection)
{
    cg::yobi::YobiClearUp_404D30(connection.Owner());
    ++connection.Owner().WorldScene().sceneState;
}

// 原函数：0x00462880。
void InitializeCharacterAppearanceSelection_462880(network::CgConnection& connection, const std::uint8_t resetSelection)
{
    auto& draft = connection.CharacterSelectUi().creationDraft;
    if (resetSelection != 0) {
        const std::uint8_t pukVersion = connection.PukVersion();
        draft.characterFamily9A57E0 = 0;
        if (pukVersion >= 5) {
            draft.characterFamily9A57E0 = 1;
        }
        draft.characterIndex9A4CFC = 0;
        draft.variantIndex9A5B00 = 0;
    }

    cg::yobi::YobiAction* const firstAction =
        cg::yobi::NewYobiAction_404AE0(connection.Owner(), 2, 0);
    draft.createActions[0] = firstAction;
    if (firstAction != nullptr) {
        cg::yobi::SetYobiActionField30(firstAction, 517);
        cg::yobi::SetYobiActionField34(draft.createActions[0], 243);
        cg::yobi::SetYobiActionField2B8(draft.createActions[0], cg::yobi::YobiDirection_Down);
    }

    cg::yobi::YobiAction* const secondAction =
        cg::yobi::NewYobiAction_404AE0(connection.Owner(), 2, 0);
    draft.createActions[1] = secondAction;
    if (secondAction != nullptr) {
        cg::yobi::SetYobiActionField30(secondAction, 517);
        cg::yobi::SetYobiActionField34(draft.createActions[1], 396);
        cg::yobi::SetYobiActionField2B8(draft.createActions[1], cg::yobi::YobiDirection_Down);
    }
    input::SetContext_44B330(connection.Owner(), nullptr);
}

// 原函数：0x00462930。
std::int32_t UpdateCharacterAppearanceSelection_462930(network::CgConnection& connection)
{
    auto& draft = connection.CharacterSelectUi().creationDraft;
    std::uint8_t selected = draft.characterIndex9A4CFC;
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
        const std::int32_t version = static_cast<std::int8_t>(draft.characterFamily9A57E0);
        const std::int32_t image = kCreationGraphic55C354[14 * version + index];
        if (image >= 206000 && image <= 206300) {
            if (static_cast<std::int8_t>(selected) >= 7 && selected != 10) {
                selected = 0;
                draft.characterIndex9A4CFC = 0;
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
            draft.characterIndex9A4CFC = static_cast<std::uint8_t>(index);
            draft.variantIndex9A5B00 = 0;
            break;
        }
        selected = draft.characterIndex9A4CFC;
    }
    if (HitTestCharacterCreationControl_464290(&kCreationSelectButtons54DFE8[0].bounds) != 0) {
        confirmState = hover();
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            audio::PlaySe_52C450(53, 320);
            result = 1;
        }
    }
    if (HitTestCharacterCreationControl_464290(&kCreationSelectButtons54DFE8[1].bounds) != 0) {
        cancelState = hover();
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            audio::PlaySe_52C450(54, 320);
            result = 2;
        }
    }
    if (connection.PukVersion() >= 5) {
        if (HitTestCharacterCreationControl_464290(&kCreationVersionHit54E020[0]) != 0) {
            versionPlus = hover();
            if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
                const std::uint8_t puk = connection.PukVersion();
                const std::int32_t version = static_cast<std::int8_t>(draft.characterFamily9A57E0);
                const std::int32_t limit = puk >= 8 ? 2 : 1;
                if (version < limit) {
                    ++draft.characterFamily9A57E0;
                } else {
                    draft.characterFamily9A57E0 = static_cast<std::uint8_t>(limit);
                }
            }
        }
        if (HitTestCharacterCreationControl_464290(&kCreationVersionHit54E020[1]) != 0) {
            versionMinus = hover();
            if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
                if (static_cast<std::int8_t>(draft.characterFamily9A57E0) > 0) {
                    --draft.characterFamily9A57E0;
                } else {
                    draft.characterFamily9A57E0 = 0;
                }
            }
        }
    }
    if (HitTestCharacterCreationControl_464290(&kCreationVariantButtons54E084[0].bounds) != 0) {
        variantMinus = hover();
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            const auto next = static_cast<std::uint8_t>(draft.variantIndex9A5B00 - 1);
            draft.variantIndex9A5B00 = next;
            if (static_cast<std::int8_t>(next) < 0) {
                draft.variantIndex9A5B00 = 3;
            }
        }
    }
    if (HitTestCharacterCreationControl_464290(&kCreationVariantButtons54E084[1].bounds) != 0) {
        variantPlus = hover();
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            const auto next = static_cast<std::uint8_t>(draft.variantIndex9A5B00 + 1);
            draft.variantIndex9A5B00 = next;
            if (static_cast<std::int8_t>(next) > 3) {
                draft.variantIndex9A5B00 = 0;
            }
        }
    }

    if (HitTestCharacterCreationControl_464290(&kCreationPreviewHit54E064[0]) != 0
        || HitTestCharacterCreationControl_464290(&kCreationPreviewHit54E064[1]) != 0) {
        cg::yobi::YobiAction* first;
        cg::yobi::YobiAction* second;
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
            first = draft.createActions[0];
            if (first != nullptr) {
                cg::yobi::SetYobiActionField2B8(
                    first, cg::yobi::GetYobiActionField2B8(first) - 1);
                first = draft.createActions[0];
                if (cg::yobi::GetYobiActionField2B8(first) < 0) {
                    cg::yobi::SetYobiActionField2B8(first, cg::yobi::YobiDirection_Left);
                    first = draft.createActions[0];
                }
            }
            second = draft.createActions[1];
            if (second != nullptr) {
                cg::yobi::SetYobiActionField2B8(
                    second, cg::yobi::GetYobiActionField2B8(second) - 1);
                second = draft.createActions[1];
                if (cg::yobi::GetYobiActionField2B8(second) < 0) {
                    cg::yobi::SetYobiActionField2B8(second, cg::yobi::YobiDirection_Left);
                    second = draft.createActions[1];
                }
                first = draft.createActions[0];
            }
        } else {
            second = draft.createActions[1];
            first = draft.createActions[0];
        }
        if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 2) != 0) {
            if (first != nullptr) {
                cg::yobi::SetYobiActionField2B8(
                    first, cg::yobi::GetYobiActionField2B8(first) + 1);
                cg::yobi::YobiAction* const current = draft.createActions[0];
                if (cg::yobi::GetYobiActionField2B8(current) > 7) {
                    cg::yobi::SetYobiActionField2B8(current, cg::yobi::YobiDirection_UpLeft);
                }
                second = draft.createActions[1];
            }
            if (second != nullptr) {
                cg::yobi::SetYobiActionField2B8(
                    second, cg::yobi::GetYobiActionField2B8(second) + 1);
                second = draft.createActions[1];
                if (cg::yobi::GetYobiActionField2B8(second) > 7) {
                    cg::yobi::SetYobiActionField2B8(second, cg::yobi::YobiDirection_UpLeft);
                }
            }
        }
    }

    const auto& selectedPosition = kCreationSelectDraw54DF00[static_cast<std::int8_t>(draft.characterIndex9A4CFC)];
    cg::render::DrawGraphic(selectedPosition[0], selectedPosition[1], 10, 245042, 0, nullptr);
    if (render::HasLegacy3DDevice_44B610() != 0) {
        const auto& position = kCreationSelectDraw54DF00[static_cast<std::int8_t>(draft.characterIndex9A4CFC)];
        cg::render::DrawGraphic(position[0], position[1], 10, 245043, 0, &descriptor);
    }
    cg::render::DrawGraphic(
        138, 409, 10, std::bit_cast<std::array<std::int32_t, 7>>(kCreationSelectButtons54DFE8[1])[4 + cancelState], 0, nullptr);
    cg::render::DrawGraphic(
        289, 409, 10, std::bit_cast<std::array<std::int32_t, 7>>(kCreationSelectButtons54DFE8[0])[4 + confirmState], 0, nullptr);
    if (connection.PukVersion() >= 5) {
        cg::render::DrawGraphic(230, 429, 10, kCreationVersionImages54E040[0][versionPlus], 0, nullptr);
        cg::render::DrawGraphic(194, 429, 10, kCreationVersionImages54E040[1][versionMinus], 0, nullptr);
    }
    cg::render::DrawGraphic(
        203, 412, 10, kCreationVersionLabels54E058[static_cast<std::int8_t>(draft.characterFamily9A57E0)], 0, nullptr);
    const std::uint8_t labelPuk = connection.PukVersion();
    cg::render::DrawGraphic(
        221, 412, 10, labelPuk < 5 ? 245481 : (labelPuk < 8 ? 245482 : 245483), 0, nullptr);

    const std::uint8_t listPuk = connection.PukVersion();
    const std::int32_t* const graphicTable = listPuk >= 7 ? kCreationGraphic55C354.data()
        : (listPuk >= 5 ? kCreationGraphic55C3FC.data() : kCreationGraphic55C46C.data());
    for (std::int32_t index = 0; index < 14; ++index) {
        const std::int32_t version = static_cast<std::int8_t>(draft.characterFamily9A57E0);
        const auto& position = kCreationSelectDraw54DF00[index];
        cg::render::DrawGraphic(
            position[0], position[1], 10, graphicTable[14 * version + index], 0, nullptr);
    }
    cg::render::DrawGraphic(214, 254, 10, 245041, 0, nullptr);
    cg::render::DrawGraphic(
        467, 277, 10, std::bit_cast<std::array<std::int32_t, 7>>(kCreationVariantButtons54E084[0])[4 + variantMinus], 0, nullptr);
    cg::render::DrawGraphic(
        568, 277, 10, std::bit_cast<std::array<std::int32_t, 7>>(kCreationVariantButtons54E084[1])[4 + variantPlus], 0, nullptr);

    cg::yobi::YobiAction* const second = draft.createActions[1];
    if (second != nullptr) {
        cg::yobi::SetYobiActionField15(second, 2);
        const std::int32_t version = static_cast<std::int8_t>(draft.characterFamily9A57E0);
        const std::int32_t character = static_cast<std::int8_t>(draft.characterIndex9A4CFC);
        const std::int32_t variant = static_cast<std::int8_t>(draft.variantIndex9A5B00);
        const std::int32_t motion = kCreationMotion54DDB0[28 * version + character + 14] + 6 * variant;
        cg::yobi::SetYobiActionField2A8(draft.createActions[1], motion);
        cg::yobi::AdvanceYobiAnimationFrame_517880(draft.createActions[1], 100, 1, 0);
        if (connection.PukVersion() >= 5 && draft.characterFamily9A57E0 == 0) {
            const std::int32_t graphic = cg::yobi::GetYobiActionField20(draft.createActions[1]);
            render::DrawGraphic2_521DD0(517, 396, 10, graphic, 0, 0, nullptr);
        }
    }
    cg::yobi::YobiAction* const first = draft.createActions[0];
    if (first != nullptr) {
        cg::yobi::SetYobiActionField15(first, 2);
        const std::uint8_t puk = connection.PukVersion();
        const std::int32_t version = static_cast<std::int8_t>(draft.characterFamily9A57E0);
        const std::int32_t character = static_cast<std::int8_t>(draft.characterIndex9A4CFC);
        std::int32_t motion;
        if (puk < 5) {
            const std::int32_t graphic = kCreationGraphic55C46C[14 * version + character];
            if (graphic == 33100 || graphic == 33101) {
                const std::int32_t variant = static_cast<std::int8_t>(draft.variantIndex9A5B00);
                motion = (graphic == 33100 ? 100900 : 100901) + 6 * variant;
            } else {
                const std::int32_t variant = static_cast<std::int8_t>(draft.variantIndex9A5B00);
                motion = kCreationMotion54DDB0[28 * version + character] + 6 * variant;
            }
        } else {
            const std::int32_t variant = static_cast<std::int8_t>(draft.variantIndex9A5B00);
            motion = kCreationMotion54DDB0[28 * version + character] + 6 * variant;
        }
        cg::yobi::SetYobiActionField2A8(draft.createActions[0], motion);
        if (cg::yobi::AdvanceYobiAnimationFrame_517880(draft.createActions[0], 100, 1, 0) != 0) {
            cg::yobi::AdvanceCharacterPreviewAnimation(draft.createActions[0]);
            cg::yobi::YobiAction* const currentSecond = draft.createActions[1];
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
std::uint32_t InitializeCharacterCreationAttributes_463100(network::CgConnection& connection)
{
    auto& draft = connection.CharacterSelectUi().creationDraft;
    cg::yobi::YobiAction* const first = draft.createActions[0];
    if (first != nullptr) {
        cg::yobi::MarkDeath(first);
    }
    cg::yobi::YobiAction* const second = draft.createActions[1];
    if (second != nullptr) {
        cg::yobi::MarkDeath(second);
    }
    input::InitializeContext_449CF0(
        connection.Owner(),
        &draft.input9A51E8, 352, 108, 0, 13, 131, {}, 1, 16, 0, 0);
    input::SetContext_44B330(connection.Owner(), &draft.input9A51E8);
    draft.attributePointBytes9A5FD4 = 0;
    draft.elementPointBytes9A50D0 = 0;
    draft.fifthAttributePoints9A5FD8 = 0;
    const std::uint32_t version = connection.PukVersion();
    draft.styleIndex9A5668 = 0;
    draft.colorIndex9A4B80 = 0;
    draft.remainingAttributePoints9A4D04 = 30;
    draft.remainingElementPoints9A50C8 = 10;
    if (version >= 7) {
        draft.graphicGeneration9A4B88 = 2;
        draft.attributePhase9A5AFC = 1;
    } else {
        draft.attributePhase9A5AFC = 1;
        draft.graphicGeneration9A4B88 = version >= 5 ? 1 : 0;
    }
    return version;
}

// 原函数：0x004631D0。
std::int32_t UpdateCharacterCreationAttributes_4631D0(network::CgConnection& connection)
{
    auto& draft = connection.CharacterSelectUi().creationDraft;
    auto* const attributeBytes = reinterpret_cast<std::uint8_t*>(&draft.attributePointBytes9A5FD4);
    const std::array<std::uint8_t*, 5> attributes {{
        attributeBytes, attributeBytes + 1, attributeBytes + 2, attributeBytes + 3,
        &draft.fifthAttributePoints9A5FD8,
    }};
    auto* const elements = reinterpret_cast<std::uint8_t*>(&draft.elementPointBytes9A50D0);
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
    total += static_cast<std::int8_t>(draft.remainingAttributePoints9A4D04);
    const std::int32_t attributeLimit = total >> 1;
    const std::uint8_t puk = connection.PukVersion();
    const std::uint8_t maxVersion = puk >= 7 ? 2 : (puk >= 5 ? 1 : 0);
    const bool showVersionControls = maxVersion != draft.characterFamily9A57E0;

    const auto hover = []() -> std::int8_t {
        return (CreationMouseByte(platform::gLegacyMouseState.heldButtons) & 1) != 0 ? 1 : 2;
    };
    const auto hitRow = [](const std::size_t record, const std::int32_t top) {
        const auto& bounds = kCreationUi54E0BC[record].bounds;
        return cg::ui::HitTestMouseRect_5049A0(
            bounds.left, top, bounds.left + bounds.width, top + bounds.height, -1);
    };

    if (draft.attributePhase9A5AFC == 1) {
        for (std::size_t index = 0; index < 4; ++index) {
            if (HitTestCharacterCreationControl_464290(&kCreationUi54E0BC[index].bounds) != 0) {
                arrows[index] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    auto& value = index < 2 ? draft.styleIndex9A5668 : draft.colorIndex9A4B80;
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
            if (HitTestCharacterCreationControl_464290(&kCreationUi54E0BC[4].bounds) != 0) {
                arrows[4] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    const std::uint8_t value = draft.graphicGeneration9A4B88;
                    const std::uint8_t version = draft.characterFamily9A57E0;
                    const auto next = static_cast<std::uint8_t>(value - 1);
                    draft.graphicGeneration9A4B88 = next;
                    if (static_cast<std::int8_t>(next) < (version != 0 ? 1 : 0)) {
                        draft.graphicGeneration9A4B88 = maxVersion;
                    }
                    audio::PlaySe_52C450(51, 320);
                }
            }
            if (HitTestCharacterCreationControl_464290(&kCreationUi54E0BC[5].bounds) != 0) {
                arrows[5] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    const std::uint8_t value = draft.graphicGeneration9A4B88;
                    const std::uint8_t version = draft.characterFamily9A57E0;
                    const auto next = static_cast<std::uint8_t>(value + 1);
                    draft.graphicGeneration9A4B88 = next;
                    if (static_cast<std::int8_t>(next) > maxVersion) {
                        draft.graphicGeneration9A4B88 = version != 0 ? 1 : 0;
                    }
                    audio::PlaySe_52C450(51, 320);
                }
            }
        }

        const std::int32_t remaining = static_cast<std::int8_t>(draft.remainingAttributePoints9A4D04);
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
                        const std::uint8_t left = draft.remainingAttributePoints9A4D04;
                        *attributes[row] = static_cast<std::uint8_t>(value - 1);
                        draft.remainingAttributePoints9A4D04 = static_cast<std::uint8_t>(left + 1);
                        audio::PlaySe_52C450(51, 320);
                    } else {
                        audio::PlaySe_52C450(55, 320);
                    }
                }
                break;
            }
            if (static_cast<std::int8_t>(draft.remainingAttributePoints9A4D04) > 0
                && static_cast<std::int8_t>(*attributes[row]) < attributeLimit && hitRow(7, top) != 0) {
                attributePlus[row] = hover();
                if ((CreationMouseByte(platform::gLegacyMouseState.pulseButtons) & 1) != 0) {
                    bool changed = false;
                    if (static_cast<std::int8_t>(draft.remainingAttributePoints9A4D04) > 0) {
                        const std::uint8_t value = *attributes[row];
                        if (static_cast<std::int8_t>(value) < attributeLimit) {
                            const std::uint8_t left = draft.remainingAttributePoints9A4D04;
                            *attributes[row] = static_cast<std::uint8_t>(value + 1);
                            draft.remainingAttributePoints9A4D04 = static_cast<std::uint8_t>(left - 1);
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
                        const std::uint8_t left = draft.remainingElementPoints9A50C8;
                        elements[row] = static_cast<std::uint8_t>(value - 1);
                        draft.remainingElementPoints9A50C8 = static_cast<std::uint8_t>(left + 1);
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
                        if (static_cast<std::int8_t>(draft.remainingElementPoints9A50C8) > 0) {
                            const std::uint8_t value = elements[row];
                            const std::uint8_t left = draft.remainingElementPoints9A50C8;
                            elements[row] = static_cast<std::uint8_t>(value + 1);
                            draft.remainingElementPoints9A50C8 = static_cast<std::uint8_t>(left - 1);
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

        if (HitTestCharacterCreationControl_464290(&kCreationUi54E0BC[10].bounds) != 0) {
            confirmState = hover();
            if ((CreationMouseByte(platform::gLegacyMouseState.edgeButtons) & 1) != 0) {
                const std::int32_t cursor = draft.input9A51E8.caretByteOffset;
                if (cursor <= 0) {
                    draft.attributePhase9A5AFC = 100;
                } else if (static_cast<std::int8_t>(draft.remainingAttributePoints9A4D04) > 0) {
                    draft.attributePhase9A5AFC = 104;
                } else if (static_cast<std::int8_t>(draft.remainingElementPoints9A50C8) > 0) {
                    draft.attributePhase9A5AFC = 106;
                } else {
                    const std::wstring utf16Name(draft.input9A51E8.text,
                        cg::text::Prefix(draft.input9A51E8.text, static_cast<std::size_t>(cursor)));
                    std::optional<std::string> stored;
                    try { stored = cg::text::ToGbk(utf16Name); }
                    catch (const std::system_error&) { draft.attributePhase9A5AFC = 108; }
                    if (stored) {
                        if (stored->size() >= 24) {
                            draft.attributePhase9A5AFC = 108;
                        } else {
                            connection.CharacterSelectUi().creationSubmission.nameFF0D18 = utf16Name;
                            const auto& name = connection.CharacterSelectUi().creationSubmission.nameFF0D18;
                            if (login::CharacterNameExists_51B140(connection, name) != 0) {
                                draft.attributePhase9A5AFC = 102;
                            } else if (name.find(L' ') != std::wstring::npos
                                || stored->find("\x81\x40") != std::string::npos
                                || input::IsRejectedInputText_404060(utf16Name)) {
                                draft.attributePhase9A5AFC = 108;
                            } else if (input::ContainsPipeOrBackslash_45E770(utf16Name) == 1) {
                                draft.attributePhase9A5AFC = 110;
                            } else {
                                draft.attributePhase9A5AFC = 10;
                                audio::PlaySe_52C450(53, 320);
                            }
                        }
                    }
                }
            }
        }
        if (HitTestCharacterCreationControl_464290(&kCreationUi54E0BC[11].bounds) != 0) {
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
    if (draft.attributePhase9A5AFC == 10) {
        const std::int32_t answer = ProcessDefaultConfirmationDialog_45E090(
            connection, 320, 240, L"可以吗？" /* 原常量参考：0x0055C9B0。 */, 0);
        if (answer == 1) {
            const std::int32_t version = static_cast<std::int8_t>(draft.characterFamily9A57E0);
            const std::uint8_t mode = draft.graphicGeneration9A4B88;
            result = 1;
            const std::int32_t character = static_cast<std::int8_t>(draft.characterIndex9A4CFC);
            const std::int32_t variant = static_cast<std::int8_t>(draft.variantIndex9A5B00);
            draft.attributePhase9A5AFC = 11;
            if (mode == 0) {
                connection.CharacterSelectUi().previewToggleState = 0;
                connection.CharacterSelectUi().creationSubmission.motionImageDE0AFC = kCreationMotion54DDB0[28 * version + character] + 6 * variant;
            } else {
                connection.CharacterSelectUi().creationSubmission.motionImageDE0AFC = kCreationMotion54DDB0[28 * version + character + 14] + 6 * variant;
                connection.CharacterSelectUi().previewToggleState = 1;
            }
            // 原ESI沿用上方四项互斥循环终值，未知模式不是零图号。
            std::int32_t graphic = 4;
            if (mode == 2) {
                const std::int32_t style = static_cast<std::int8_t>(draft.styleIndex9A5668);
                graphic = kCreationGraphic55C354[14 * version + character];
                const std::int32_t color = static_cast<std::int8_t>(draft.colorIndex9A4B80);
                graphic += 5 * (5 * variant + style) + color;
            } else if (mode == 1 || mode == 0) {
                graphic = MapCreationGraphic(draft, kCreationGraphic55C354[14 * version + character], mode);
            }

            const std::int32_t attribute0 = static_cast<std::int8_t>(*attributes[0]);
            const std::int32_t attribute1 = static_cast<std::int8_t>(*attributes[1]);
            const std::int32_t attribute2 = static_cast<std::int8_t>(*attributes[2]);
            auto& submission = connection.CharacterSelectUi().creationSubmission;
            submission.attribute0F633EC = attribute0;
            submission.attribute1F633F0 = attribute1;
            const std::int32_t attribute3 = static_cast<std::int8_t>(*attributes[3]);
            const std::int32_t attribute4 = static_cast<std::int8_t>(*attributes[4]);
            submission.attribute3DE0AF8 = attribute3;
            const std::uint32_t elementSnapshot = draft.elementPointBytes9A50D0;
            submission.attribute2DE03F4 = attribute2;
            submission.attribute4F7CA00 = attribute4;
            const std::int32_t element2 = static_cast<std::int8_t>(elements[2]);
            submission.element0F7C948 = static_cast<std::int8_t>(elementSnapshot);
            connection.CharacterSelectUi().selectedGraphicNo = graphic;
            const std::int32_t element3 = static_cast<std::int8_t>(elements[3]);
            submission.element1FF03D0 = static_cast<std::int8_t>(elementSnapshot >> 8);
            submission.element2F633F4 = element2;
            submission.element3DE0B04 = element3;
            connection.Owner().WorldScene().initialDuelPoints = 0;
            cg::audio::BeginBgmFadeOut_52B990();
            SaveCharacterCreationDraftSnapshot_4642C0(connection);
        } else if (answer == 2) {
            draft.attributePhase9A5AFC = 1;
            skipMessages = true;
        }
    }
    if (!skipMessages) {
        for (std::size_t index = 0; index < kCreationErrors.size(); ++index) {
            const std::int32_t phase = 100 + 2 * static_cast<std::int32_t>(index);
            if (draft.attributePhase9A5AFC == phase) {
                login::ResetLoginMessage(connection);
                ++draft.attributePhase9A5AFC;
                audio::PlaySe_52C450(55, 320);
            }
            if (draft.attributePhase9A5AFC == phase + 1
                && login::UpdateLoginMessage(connection, kCreationErrors[index]) != 0) {
                draft.attributePhase9A5AFC = 1;
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

    std::uint32_t elementSnapshot = draft.elementPointBytes9A50D0;
    std::int32_t count = 0;
    if (static_cast<std::int8_t>(elementSnapshot) > 0) {
        do {
            cg::render::DrawGraphic(406 + 10 * count, 313, 10, 245052, 0, nullptr);
            elementSnapshot = draft.elementPointBytes9A50D0;
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
    const std::int32_t mode = static_cast<std::int8_t>(draft.graphicGeneration9A4B88);
    if (mode == 2) {
        const std::int32_t version = static_cast<std::int8_t>(draft.characterFamily9A57E0);
        const std::int32_t character = static_cast<std::int8_t>(draft.characterIndex9A4CFC);
        const std::int32_t variant = static_cast<std::int8_t>(draft.variantIndex9A5B00);
        const std::int32_t style = static_cast<std::int8_t>(draft.styleIndex9A5668);
        graphic = kCreationGraphic55C354[14 * version + character];
        const std::int32_t color = static_cast<std::int8_t>(draft.colorIndex9A4B80);
        graphic += 5 * (5 * variant + style) + color;
    } else if (mode == 1 || mode == 0) {
        const std::int32_t version = static_cast<std::int8_t>(draft.characterFamily9A57E0);
        const std::int32_t character = static_cast<std::int8_t>(draft.characterIndex9A4CFC);
        graphic = MapCreationGraphic(draft, kCreationGraphic55C354[14 * version + character], mode);
    }
    if (static_cast<std::int8_t>(draft.graphicGeneration9A4B88) > maxVersion) {
        graphic = static_cast<std::int8_t>(draft.characterIndex9A4CFC) >= 14 ? 33101 : 33100;
    }
    cg::render::DrawGraphic(500, 126, 10, graphic, 0, nullptr);
    render::DrawInputText_4568A0(connection.Owner(), &draft.input9A51E8);
    constexpr std::int8_t color = std::bit_cast<std::int8_t>(std::uint8_t{131});
    cg::text::FormatTo(buffer, L"{:02}", static_cast<std::int32_t>(static_cast<std::int8_t>(draft.remainingAttributePoints9A4D04)));
    render::DrawText_456780(518, 185, 0, 13, color, buffer, 0, 0);
    for (std::size_t row = 0; row < attributes.size(); ++row) {
        cg::text::FormatTo(buffer, L"{:02}", static_cast<std::int32_t>(static_cast<std::int8_t>(*attributes[row])));
        render::DrawText_456780(
            479, static_cast<std::int16_t>(199 + 14 * row), 0, 13, color, buffer, 0, 0);
    }
    cg::text::FormatTo(buffer, L"{:02}", static_cast<std::int32_t>(static_cast<std::int8_t>(draft.remainingElementPoints9A50C8)));
    render::DrawText_456780(518, 293, 0, 13, color, buffer, 0, 0);
    cg::render::DrawGraphic(442, 241, 10, 245051, 0, nullptr);
    cg::render::DrawGraphic(320, 240, 3, 245050, 0, nullptr);
    return result;
}

// 原函数：0x004642C0。
std::int32_t SaveCharacterCreationDraftSnapshot_4642C0(network::CgConnection& connection)
{
    auto& draft = connection.CharacterSelectUi().creationDraft;
    auto& submission = connection.CharacterSelectUi().creationSubmission;
    auto& snapshot = connection.CharacterSelectUi().creationSnapshot;
    const std::int32_t valueF633F0 = submission.attribute1F633F0;
    const std::int32_t valueF633EC = submission.attribute0F633EC;
    snapshot.value9A50A4 = valueF633F0;
    const std::int32_t valueF7CA00 = submission.attribute4F7CA00;
    const std::int32_t valueDE03F4 = submission.attribute2DE03F4;
    snapshot.value9A4B98 = valueF7CA00;
    const std::int32_t valueF633F4 = submission.element2F633F4;
    snapshot.value9A50A0 = valueF633EC;
    const std::int32_t valueDE0AF8 = submission.attribute3DE0AF8;
    snapshot.value9A5670 = valueF633F4;
    const std::int32_t value9A50C8 = std::bit_cast<std::int8_t>(draft.remainingElementPoints9A50C8);
    snapshot.value9A4E98 = valueDE0AF8;
    const std::int32_t valueFF03D0 = submission.element1FF03D0;
    snapshot.value9A4E84 = valueDE03F4;
    const std::int32_t valueF7C948 = submission.element0F7C948;
    snapshot.value9A50E4 = value9A50C8;
    snapshot.value9A60E8 = valueFF03D0;
    const std::int32_t value9A4B88 = std::bit_cast<std::int8_t>(draft.graphicGeneration9A4B88);
    const std::int32_t value9A4D04 = std::bit_cast<std::int8_t>(draft.remainingAttributePoints9A4D04);
    snapshot.value9A5094 = valueF7C948;
    const std::int32_t valueDE0B04 = submission.element3DE0B04;
    snapshot.value9A57E8 = value9A4B88;
    snapshot.value9A61F4 = valueDE0B04;
    const std::int32_t value9A5668 = std::bit_cast<std::int8_t>(draft.styleIndex9A5668);
    snapshot.value9A554C = value9A4D04;
    const std::int32_t value9A4B80 = std::bit_cast<std::int8_t>(draft.colorIndex9A4B80);
    snapshot.input9A4F38 = draft.input9A51E8;
    snapshot.value9A5664 = value9A5668;
    snapshot.value9A5954 = value9A4B80;
    return value9A4B80;
}

// 原函数：0x00464380。
std::uint8_t RestoreCharacterCreationDraftSnapshot_464380(network::CgConnection& connection)
{
    auto& draft = connection.CharacterSelectUi().creationDraft;
    auto& snapshot = connection.CharacterSelectUi().creationSnapshot;
    auto* const firstBytes = reinterpret_cast<std::uint8_t*>(&draft.attributePointBytes9A5FD4);
    auto* const secondBytes = reinterpret_cast<std::uint8_t*>(&draft.elementPointBytes9A50D0);
    const std::uint8_t value9A50A4 = ReadCreationSnapshotByte(snapshot.value9A50A4);
    const std::uint8_t value9A50A0 = ReadCreationSnapshotByte(snapshot.value9A50A0);
    const std::uint8_t value9A4E84 = ReadCreationSnapshotByte(snapshot.value9A4E84);
    firstBytes[1] = value9A50A4;
    const std::uint8_t value9A4B98 = ReadCreationSnapshotByte(snapshot.value9A4B98);
    firstBytes[0] = value9A50A0;
    const std::uint8_t value9A4E98 = ReadCreationSnapshotByte(snapshot.value9A4E98);
    draft.fifthAttributePoints9A5FD8 = value9A4B98;
    const std::uint8_t value9A5670 = ReadCreationSnapshotByte(snapshot.value9A5670);
    firstBytes[2] = value9A4E84;
    const std::uint8_t value9A5094 = ReadCreationSnapshotByte(snapshot.value9A5094);
    secondBytes[2] = value9A5670;
    const std::uint8_t value9A50E4 = ReadCreationSnapshotByte(snapshot.value9A50E4);
    firstBytes[3] = value9A4E98;
    const std::uint8_t value9A60E8 = ReadCreationSnapshotByte(snapshot.value9A60E8);
    draft.remainingElementPoints9A50C8 = value9A50E4;
    const std::uint8_t value9A57E8 = ReadCreationSnapshotByte(snapshot.value9A57E8);
    secondBytes[0] = value9A5094;
    const std::uint8_t value9A61F4 = ReadCreationSnapshotByte(snapshot.value9A61F4);
    secondBytes[1] = value9A60E8;
    const std::uint8_t value9A554C = ReadCreationSnapshotByte(snapshot.value9A554C);
    draft.graphicGeneration9A4B88 = value9A57E8;
    secondBytes[3] = value9A61F4;
    const std::uint8_t value9A5664 = ReadCreationSnapshotByte(snapshot.value9A5664);
    draft.remainingAttributePoints9A4D04 = value9A554C;
    const std::uint8_t value9A5954 = ReadCreationSnapshotByte(snapshot.value9A5954);
    draft.styleIndex9A5668 = value9A5664;
    draft.input9A51E8 = snapshot.input9A4F38;
    draft.colorIndex9A4B80 = value9A5954;
    return value9A5954;
}

// 原函数：0x00464290。
std::int32_t HitTestCharacterCreationControl_464290(const cg::ui::UiControlBounds* const rectangle)
{
    const std::int32_t top = rectangle->top;
    const std::int32_t left = rectangle->left;
    const std::int32_t height = rectangle->height;
    const std::int32_t width = rectangle->width;
    return cg::ui::HitTestMouseRect_5049A0(
        left, top, left + width, top + height, -1);
}


} // namespace cg::character_select::detail

namespace cg::character_select::state {

std::uint8_t gGraphicVersionOverride58B6A0 = 0xFF; // 原全局：0x0058B6A0。

} // namespace cg::character_select::state

namespace cg::character_select {

static constexpr std::size_t kLegacyNameCapacity = 20;

static constexpr detail::CharacterSelectUiRecord gReturnUi {
    {534, 438, 64, 16}, 245045, 245047, 245046,
};
static constexpr detail::CharacterSelectUiRecord gUpdateUi {
    {296, 433, 49, 17}, 244542, 244544, 244543,
};

static void CopyRecordName(std::wstring& destination, const login::CharacterRecord& record)
{
    const std::wstring name = record.name;
    cg::core::NullSub_509080();
    destination = name;
}

static void ClearPreviewActions(network::CgConnection& connection)
{
    auto& actions = connection.CharacterSelectUi().previewActions;
    if (actions[0] != nullptr) {
        actions[0] = nullptr;
    }
    if (actions[1] != nullptr) {
        actions[1] = nullptr;
    }
}

// CUN01：原460FF0角色进入分支，供Debug直入及非Debug确认后共用，调用顺序保持。
static void EnterSelectedCharacter(network::CgConnection& connection)
{
    auto& characters = connection.LoginRequests().characters;
    const login::CharacterRecord& record =
        characters[static_cast<std::size_t>(connection.LoginRequests().selectedCharacterIndex)];
    CopyRecordName(connection.CharacterSelectUi().selectedCharacterName, record);
    const std::int16_t selected = connection.LoginRequests().selectedCharacterIndex;
    const login::CharacterRecord* const selectedRecord =
        characters.data() + selected;
    connection.CharacterSelectUi().selectedGraphicNo = selectedRecord->graphicNo;
    world::SetCurrentSceneType(connection.Owner(), 5);
    login::detail::PlaySe(59, 320, 240);
    connection.CharacterSelectUi().characterEntryState = 0;
    cg::audio::BeginBgmFadeOut_52B990();
    cg::yobi::YobiClearUp_404D30(connection.Owner());
    ClearPreviewActions(connection);
}

#ifndef _DEBUG
// CUN01：本端人物页扩展状态；等待明确选择，不复用原0..21业务状态。
static constexpr std::int32_t kUsageNoticeState = 22;
thread_local cg::platform::LegacyCryptInt state1(1);
thread_local cg::platform::LegacyCryptInt state2(std::random_device().operator()());

#define MIXVL 0x1a57b94f
#define MXTRUE 0x8ef61

// CUN01：沿现有游戏模态动作与文字按钮，返回0等待、1同意、2退出。
static std::int32_t ProcessUsageNotice(network::CgConnection& connection)
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
        {264-12, 368, 240, 366, 64, 24, L"同意"},
        {360-12, 368, 336, 366, 64, 24, L"结束"},
    }};

    auto*& action = connection.CharacterSelectUi().characterModalAction;
    if (action == nullptr) {
        action = cg::yobi::CreateModalAction_5044E0(connection.Owner(), 32, 72, 576, 336, 4, 1);
        goto RET; // 创建当帧不消费人物选择的点击；失败时下一帧重试。
    }
    if (!cg::yobi::IsYobiActionReady(action, 1)) {
        goto RET;
    }

    for (std::size_t index = 0; index < buttons.size(); ++index) {
        if ((detail::ReadCharacterModalButtonMouseState_503990(&buttons[index]) & 1) != 0) {
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
        detail::DrawCharacterModalButton_503A20(&button, 2, 4, 1, cg::render::gHoverFramePaletteIndex_D69768, 214);
    }
RET:
    VM_DOLPHIN_WHITE_END
    return iRet;
}
#endif

static void UpdateCharacterGraphic(
    network::CgConnection& connection, login::CharacterRecord& record,
    std::int32_t action)
{
    const std::uint8_t pukVersion = connection.PukVersion();
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
                record.graphicNo = detail::ConvertClassicCharacterGraphicToCompact_51BB80(record.graphicNo);
            } else {
                const std::int32_t first = detail::ConvertClassicCharacterGraphicToCompact_51BB80(
                    record.graphicNo);
                record.graphicNo = first;
                record.graphicNo = detail::ConvertEarlierCharacterGraphicToModern_51BFF0(first);
            }
        } else if (graphicClass == 1) {
            if (selector == 2) {
                record.graphicNo = detail::ConvertEarlierCharacterGraphicToModern_51BFF0(record.graphicNo);
            } else if (selector == 0) {
                record.graphicNo = imageNo >= 200000 && imageNo < 202700
                    ? detail::ConvertEarlierCharacterGraphicToModern_51BFF0(record.graphicNo)
                    : detail::ConvertCompactCharacterGraphicToClassic_51BAC0(record.graphicNo);
            }
        } else if (graphicClass == 2) {
            if (selector == 3 && imageNo >= 240000 && imageNo < 241400) {
                const std::int32_t first = detail::ConvertModernCharacterGraphicToEarlier_51BF00(
                    record.graphicNo);
                record.graphicNo = first;
                record.graphicNo = detail::ConvertCompactCharacterGraphicToClassic_51BAC0(first);
            } else {
                record.graphicNo = detail::ConvertModernCharacterGraphicToEarlier_51BF00(record.graphicNo);
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
        record.graphicNo = detail::ConvertClassicCharacterGraphicToCompact_51BB80(record.graphicNo);
    } else if (graphicClass == 1) {
        record.graphicNo = detail::ConvertCompactCharacterGraphicToClassic_51BAC0(record.graphicNo);
    }
}

// 原函数：0x00460FF0。
#pragma optimize("", off)
std::int32_t ProcessScene_460FF0(network::CgConnection& connection)
{
    VM_DOLPHIN_WHITE_START
    std::int32_t sceneResult;
    auto& sceneUi = connection.CharacterSelectUi();
    // 单次执行块使普通提前退出先经过VM结束标记，不改变后续帧的状态流转。
    do {
        std::int32_t interactive = 1;

        const std::uint32_t mainState = static_cast<std::uint32_t>(connection.Owner().WorldScene().sceneState);
        if (mainState <= 22u) {
            if (mainState == 0u) {
                connection.Owner().WorldScene().sceneState = 1;
                connection.PukVersion() = 0;
                ResetCharacterListProgress_45DD50(connection);
                sceneUi.updateAvailable = 0;
                render::SelectPalette(0, 0);
            } else if (mainState == 1u) {
                const std::int32_t result = UpdateCharacterListProgress_45DD60(connection);
                if (result == 1) {
                    connection.Owner().WorldScene().sceneState = 9;
                } else if (result == -3) {
                    connection.Owner().WorldScene().sceneState = 2;
                    connection.Shutdown();
                    connection.DisconnectState() = 0;
                } else if (result == -4) {
                    connection.Owner().WorldScene().sceneState = 7;
                    connection.Shutdown();
                    connection.DisconnectState() = 0;
                } else if (result < 0) {
                    connection.Owner().WorldScene().sceneState = 4;
                    connection.Shutdown();
                    connection.DisconnectState() = 0;
                }
            } else if (mainState == 2u) {
                login::ResetLoginMessage(connection);
                connection.Owner().WorldScene().sceneState = 6;
            } else if (mainState == 3u) {
                // 原状态3不执行操作。
            } else if (mainState == 4u) {
                login::ResetLoginMessage(connection);
                connection.Owner().WorldScene().sceneState = 5;
            } else if (mainState == 5u) {
                if (login::UpdateLoginMessage(connection, L"角色资料读取失败。" /* 原 0x55CD68: 角色资料读取失败提示。 */) != 0) {
                    world::SetCurrentSceneType(connection.Owner(), 2);
                }
            } else if (mainState == 6u) {
                if (login::UpdateLoginMessage(connection, L"目前伺服器正在停机维护中。" /* 原 0x55CD4C: 服务器维护提示。 */) != 0) {
                    world::SetCurrentSceneType(connection.Owner(), 2);
                }
            } else if (mainState == 7u) {
                ResetAuthenticationFailure_45D7D0(connection);
                connection.Owner().WorldScene().sceneState = 8;
            } else if (mainState == 8u) {
                if (UpdateAuthenticationFailure_45D7E0(connection) != 0) {
                    world::SetCurrentSceneType(connection.Owner(), 2);
                }
            } else if (mainState == 9u) {
                const std::int32_t capability = resource::ProbePaletteCapability_45C970();
                const std::uint8_t pukVersion = connection.PukVersion();
                render::gPaletteCapability = capability;
                if (pukVersion == 1u) {
                    connection.Owner().WorldScene().sceneState = 10;
                } else if ((capability < 2 && pukVersion >= 5u)
                    || (capability < 1 && pukVersion >= 3u)
                    || (capability < 3 && pukVersion >= 7u)
                    || (capability < 4 && pukVersion >= 8u)) {
                    connection.Owner().WorldScene().sceneState = 19;
                } else {
                    if (capability == 1) {
                        if (pukVersion == 0u || pukVersion == 2u) {
                            sceneUi.updateAvailable = 1;
                        }
                    } else if (capability >= 2
                        && (pukVersion == 0u || pukVersion == 2u)) {
                        sceneUi.updateAvailable = 1;
                    } else if (capability >= 3
                        && pukVersion >= 5u
                        && pukVersion <= 6u) {
                        sceneUi.updateAvailable = 1;
                    } else if (capability == 4 && pukVersion == 7u) {
                        sceneUi.updateAvailable = 1;
                    }
                    connection.Owner().WorldScene().sceneState = 10;
                }
            } else if (mainState == 10u) {
                connection.Owner().WorldScene().sceneState = 11;
                ProcessCharacterPreview_460D00(connection, 0);
                ProcessCharacterPreview_460D00(connection, 1);
            } else if (mainState == 11u) {
                interactive = 0;
            } else if (mainState == 12u) {
                const std::int32_t result =
                    ProcessCharacterDeletionWarning_45E0C0(connection, 320, 240);
                if (result == 1) {
                    connection.Owner().WorldScene().sceneState = 13;
                    sceneUi.deleteConfirmationAction =
                        cg::yobi::CreateModalAction_5044E0(connection.Owner(), 130, 300, 340, 52, 4, 1);
                } else if (result == 2) {
                    connection.Owner().WorldScene().sceneState = 10;
                }
            } else if (mainState == 13u) {
                const std::int32_t result =
                    ProcessDefaultConfirmationDialog_45E090(
                        connection,
                        320,
                        240,
                        L"真的要删除吗？" /* 原 0x55C854: 删除角色确认提示。 */,
                        0);
                if (cg::yobi::IsYobiActionReady(sceneUi.deleteConfirmationAction, 1)) {
                    cg::render::DrawText3_456560(166, 326, 2, 4, L"想要删除时，请按着 CTRL键不放" /* 原 0x55CD94: 按住 CTRL 删除角色提示。 */, 0);
                    cg::render::DrawText3_456560(166, 346, 2, 4, L"再点选『是』。        " /* 原 0x55CD7C: 再次点击“是”的提示，保留八个尾空格。 */, 0);
                    if (result == 1) {
                        if ((platform::gKeyStates[17] & 2u) != 0) {
                            connection.Owner().WorldScene().sceneState = 14;
                            login::detail::PlaySe(284, 320, 240);
                            if (sceneUi.deleteConfirmationAction != nullptr) {
                                cg::yobi::MarkDeath(sceneUi.deleteConfirmationAction);
                                sceneUi.deleteConfirmationAction = nullptr;
                            }
                            ResetCharacterDeletionProgress_45DBF0(connection);
                            connection.Owner().WorldScene().sceneState = 14;
                        }
                    } else if (result == 2) {
                        connection.Owner().WorldScene().sceneState = 10;
                        if (sceneUi.deleteConfirmationAction != nullptr) {
                            cg::yobi::MarkDeath(sceneUi.deleteConfirmationAction);
                            sceneUi.deleteConfirmationAction = nullptr;
                        }
                    }
                }
            } else if (mainState == 14u) {
                const std::int32_t result = UpdateCharacterDeletionProgress_45DC00(connection);
                if (result == 1) {
                    cg::login::ClearCharacterSlot_51B3B0(connection, static_cast<std::uint32_t>(
                        static_cast<std::int32_t>(connection.LoginRequests().selectedCharacterIndex)));
                    const std::int16_t unusedSelected = connection.LoginRequests().selectedCharacterIndex;
                    static_cast<void>(unusedSelected);
                    cg::core::NullSub_509080();
                    cg::login::detail::SaveCurrentLoginRecord_520200();
                    if (sceneUi.pendingCharacterCount > 0) {
                        --sceneUi.pendingCharacterCount;
                    }
                    connection.Owner().WorldScene().sceneState = 10;
                } else if (result == -2) {
                    connection.Owner().WorldScene().sceneState = 15;
                }
            } else if (mainState == 15u) {
                login::ResetLoginMessage(connection);
                connection.Owner().WorldScene().sceneState = 16;
            } else if (mainState == 16u) {
                if (login::UpdateLoginMessage(connection, L"无法删除此角色。" /* 原 0x55CD38: 无法删除角色提示。 */) != 0) {
                    connection.Owner().WorldScene().sceneState = 0;
                }
            } else if (mainState == 17u) {
                login::ResetLoginMessage(connection);
                connection.Owner().WorldScene().sceneState = 18;
            } else if (mainState == 18u) {
                const std::int32_t result =
                    ProcessConfirmationDialog_45DEE0(
                        connection,
                        320,
                        240,
                        L"要进行更新吗？" /* 原 0x55CD28: 更新确认提示。 */,
                        0,
                        384,
                        96);
                if (result == 1) {
                    world::SetCurrentSceneType(connection.Owner(), 13);
                } else if (result == 2) {
                    connection.Owner().WorldScene().sceneState = 10;
                }
            } else if (mainState == 19u) {
                login::ResetLoginMessage(connection);
                connection.Owner().WorldScene().sceneState = 20;
            } else if (mainState == 20u) {
                if (login::UpdateLoginMessage(connection, L"这个帐号仅适用于资料片版本，请将用户端的版本更新至资料片。" /* 原 0x55C818: 资料片专用账号及客户端更新提示。 */) != 0) {
                    world::SetCurrentSceneType(connection.Owner(), 2);
                }
            } else if (mainState == 21u) {
                if (ProcessCharacterModal_462090(connection, 320, 240) == 1) {
                    connection.Owner().WorldScene().sceneState = 10;
                }
            }
#ifndef _DEBUG
            else if (mainState == kUsageNoticeState) {
                const std::int32_t choice = ProcessUsageNotice(connection);
                if (choice == 1) {
                    EnterSelectedCharacter(connection);
                } else if (choice == 2) {
                    // CUN02：直接结束进程，跳过存在纹理寿命冲突的CRT静态析构。
                    std::_Exit(cg::platform::Rand16());
                }
            }
#endif
        }

        const std::uint32_t secondaryState = static_cast<std::uint32_t>(connection.Owner().WorldScene().sceneState);
        if (secondaryState <= 20u
            && (secondaryState <= 9u || secondaryState >= 19u)) {
            sceneResult = cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 3, 245060, 0, -1), true);
            break;
        }

        for (std::uint32_t slot = 0; slot < login::CharacterRecordCount; ++slot) {
            login::CharacterRecord& record =
                connection.LoginRequests().characters[static_cast<std::size_t>(slot)];
            const std::int32_t action =
                ProcessCharacterSlot_461B30(connection, slot, interactive);
#ifndef _DEBUG
            if (connection.Owner().WorldScene().sceneState == kUsageNoticeState) {
                continue; // 原空槽按钮仍可能返回创建动作；模态等待时只绘制、不派发。
            }
#endif
            const std::uint32_t actionIndex = static_cast<std::uint32_t>(action);
            if (actionIndex > 6u) {
                continue;
            }

            if (actionIndex == 0u) {
                connection.LoginRequests().selectedCharacterIndex = static_cast<std::int16_t>(slot);
                world::ClearCurrentCharacterAlbum_518BE0(connection.Owner());
                cg::world::detail::ClearCurrentCharacterContactMail_519290(connection.Owner());
                cg::world::detail::ClearCurrentCharacterGuildMail_5194C0(connection.Owner());
                cg::world::detail::SaveContactMailFile_519300(connection.Owner());
                cg::world::detail::SaveGuildMailFile_519550(connection.Owner());
                world::SetCurrentSceneType(connection.Owner(), 4);
                login::detail::PlaySe(53, 320, 240);
                cg::yobi::YobiClearUp_404D30(connection.Owner());
                ClearPreviewActions(connection);
            } else if (actionIndex == 1u) {
                connection.LoginRequests().selectedCharacterIndex = static_cast<std::int16_t>(slot);
                // CUN01：连接阶段已校验索引；服务器记录保留地址映射前的名称。
                const std::int16_t selectedServerIndex =
                    connection.LoginRequests().selectedServerIndex;
                const wchar_t* const serverAddress = login::gLoginServerRecords_D69798[
                    static_cast<std::size_t>(selectedServerIndex)].address.data();
                if (std::wstring_view(serverAddress) == L"taomoli") {
                    EnterSelectedCharacter(connection);
                } else {
                    connection.Owner().WorldScene().sceneState = kUsageNoticeState;
                    interactive = 1; // 同帧即锁住其他槽位，等待下一帧显示提示。
                }
            } else if (actionIndex == 2u) {
                connection.LoginRequests().selectedCharacterIndex = static_cast<std::int16_t>(slot);
                if (record.deleteFlag != 0) {
                    const std::wstring name = record.name;
                    cg::core::NullSub_509080();
                    connection.Owner().WorldScene().sceneState = 12;
                    connection.CharacterSelectUi().deleteCharacterName = name;
                } else {
                    connection.Owner().WorldScene().sceneState = 21;
                }
                login::detail::PlaySe(55, 320, 240);
            } else if (actionIndex == 3u || actionIndex == 4u) {
                UpdateCharacterGraphic(connection, record, static_cast<std::int32_t>(actionIndex));
                login::detail::PlaySe(51, 320, 240);
            } else if (actionIndex == 5u || actionIndex == 6u) {
                const std::int32_t value = record.previewState;
                if (value == 0) {
                    record.previewState = 1;
                } else if (value == 1) {
                    record.previewState = 0;
                }
                sceneUi.previewToggleState =
                    record.previewState;
                ProcessCharacterPreview_460D00(connection, slot);
                login::detail::PlaySe(51, 320, 240);
            }
        }

        std::int32_t uiStyle = interactive;
        if (sceneUi.updateAvailable != 0) {
            if (uiStyle == 0
                && connection.Owner().WorldScene().sceneState == 11
                && (cg::ui::QueryUiControlInputState_45F100(&gUpdateUi) & 1) != 0) {
                world::SetCurrentSceneType(connection.Owner(), 13);
                uiStyle = 1;
            }
            cg::ui::DrawUiControl_45EFD0(&gUpdateUi, 5, 0, 0, uiStyle);
            cg::render::QueueOriginalPaletteGraphic_45F090(252, 392, 4, 245078, 0, -1);
        }

        if (uiStyle == 0
            && connection.Owner().WorldScene().sceneState == 11
            && (cg::ui::QueryUiControlInputState_45F100(&gReturnUi) & 1) != 0) {
            cg::yobi::YobiClearUp_404D30(connection.Owner());
            ClearPreviewActions(connection);
            connection.Shutdown();
            connection.DisconnectState() = 0;
            world::SetCurrentSceneType(connection.Owner(), 2);
            login::detail::PlaySe(54, 320, 240);
        }
        cg::ui::DrawUiControl_45EFD0(&gReturnUi, 5, 0, 0, uiStyle);
        cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 3, 245060, 0, -1), true);

#ifndef _DEBUG
        if (connection.Owner().WorldScene().sceneState == kUsageNoticeState) {
            sceneResult = 0;
            break; // 等待时不执行页尾升级发送／状态复位，但仍经过VM_HARD_END。
        }
#endif

        sceneResult = sceneUi.updateAvailable;
        if (sceneResult != 0) {
            const std::uint8_t pukVersion = connection.PukVersion();
            sceneResult = static_cast<std::int32_t>(pukVersion);
            if (pukVersion == 0u || pukVersion == 2u) {
                sceneResult = static_cast<std::int32_t>(connection.SendVersionUpgradeRequest(2, state::kVersion20));
            } else if (pukVersion == 5u || pukVersion == 6u) {
                sceneResult = static_cast<std::int32_t>(connection.SendVersionUpgradeRequest(4, state::kVersion30));
            } else if (pukVersion == 7u) {
                sceneResult = static_cast<std::int32_t>(connection.SendVersionUpgradeRequest(5, detail::gVersionPayload9A5C10));
            }
            connection.Owner().WorldScene().sceneState = 0;
        }
    } while (false);
    VM_DOLPHIN_WHITE_END
    return sceneResult;
}
#pragma optimize("", on)

} // namespace cg::character_select
