#pragma once

#include "cg/input/Input.hpp"

#include <array>
#include <cstdint>
#include <string>

namespace cg::yobi { struct YobiAction; }
namespace cg::character_select {

// 角色列表请求进度框的阶段、布局及 Yobi 动作借用。
struct CharacterListProgressState {
    std::uint16_t stage = 0; // 原 0x9A6E5C。
    yobi::YobiAction* action = nullptr; // 原 0x9A6E60。
    std::wstring text;
    std::int32_t columns = 0;
    std::int32_t rows = 0;
    std::int32_t x = 0;
    std::int32_t y = 0;
};

// 角色删除请求进度框的阶段、布局及 Yobi 动作借用。
struct CharacterDeletionProgressState {
    std::uint16_t stage = 0; // 原 0x9A6E54。
    yobi::YobiAction* action = nullptr; // 原 0x9A6E58。
    std::int32_t x = 0; // 原 0x9A4EA4。
    std::int32_t y = 0; // 原 0x9A5FDC。
    std::int32_t columns = 0; // 原 0x9A50DC。
    std::int32_t rows = 0; // 原 0x9A5950。
};

// 角色认证失败确认框的阶段、布局及 Yobi 动作借用。
struct AuthenticationFailureState {
    std::uint16_t stage = 0; // 原 0x9A6E34。
    yobi::YobiAction* action = nullptr; // 原 0x9A6E38。
    std::int32_t sentinel = -2; // 原 0x55A050。
    std::int32_t width = 0; // 原 0x9A50CC。
    std::int32_t height = 0; // 原 0x9A5E6C。
    std::int32_t x = 0; // 原 0x9A5654。
    std::int32_t y = 0; // 原 0x9A57DC。
};

// 创建请求进度框的阶段、布局及 Yobi 动作借用。
struct CharacterCreationProgressState {
    std::int32_t phase = 0; // 原 0x9A50E0。
    std::int32_t columns = 0; // 原 0x9A565C。
    std::int32_t rows = 0; // 原 0x9A566C。
    std::int32_t x = 0; // 原 0x9A4E90。
    std::int32_t y = 0; // 原 0x9A50D8。
    yobi::YobiAction* action = nullptr; // 原 0x9A6E88，非拥有。
};

// 创建页确认后供请求发送及草稿快照读取的原十项独立数值与名称。
struct CharacterCreationSubmissionState {
    std::int32_t attribute0F633EC = 0;
    std::int32_t attribute1F633F0 = 0;
    std::int32_t attribute2DE03F4 = 0;
    std::int32_t attribute3DE0AF8 = 0;
    std::int32_t attribute4F7CA00 = 0;
    std::int32_t element0F7C948 = 0;
    std::int32_t element1FF03D0 = 0;
    std::int32_t element2F633F4 = 0;
    std::int32_t element3DE0B04 = 0;
    std::int32_t motionImageDE0AFC = 0;
    std::wstring nameFF0D18;
};

// 创建中的外观、属性、输入和动作借用；与提交后的请求字段分开。
struct CharacterCreationDraftState {
    std::uint8_t characterFamily9A57E0 = 0;
    std::uint8_t characterIndex9A4CFC = 0;
    std::uint8_t variantIndex9A5B00 = 0;
    std::array<yobi::YobiAction*, 2> createActions {};
    input::InputContext input9A51E8;
    std::uint32_t attributePointBytes9A5FD4 = 0;
    std::uint32_t elementPointBytes9A50D0 = 0;
    std::uint8_t fifthAttributePoints9A5FD8 = 0;
    std::uint8_t styleIndex9A5668 = 0;
    std::uint8_t colorIndex9A4B80 = 0;
    std::uint8_t remainingAttributePoints9A4D04 = 0;
    std::uint8_t remainingElementPoints9A50C8 = 0;
    std::uint8_t graphicGeneration9A4B88 = 0;
    std::int32_t attributePhase9A5AFC = 0;
};

// 原十四项离散备份及输入上下文，创建失败返回编辑页时恢复。
struct CharacterCreationDraftSnapshotState {
    std::int32_t value9A50A4 = 0;
    std::int32_t value9A4B98 = 0;
    std::int32_t value9A50A0 = 0;
    std::int32_t value9A5670 = 0;
    std::int32_t value9A4E98 = 0;
    std::int32_t value9A4E84 = 0;
    std::int32_t value9A50E4 = 0;
    std::int32_t value9A60E8 = 0;
    std::int32_t value9A5094 = 0;
    std::int32_t value9A57E8 = 0;
    std::int32_t value9A61F4 = 0;
    std::int32_t value9A554C = 0;
    std::int32_t value9A5664 = 0;
    std::int32_t value9A5954 = 0;
    input::InputContext input9A4F38;
};

// 角色选择场景的可变运行期状态；动作本体尚待随场景隔离。
struct CharacterSelectSceneState {
    std::uint8_t pukVersion = 0; // 原 0x9A6E1C；网络回复生产，选角和窗口消费。
    std::wstring loginNotice; // 原 0x9A4EAC；登录回复生产、角色选择绘制。
    yobi::YobiAction* versionUpgradeAction = nullptr; // 原 0x9A6EB4。
    yobi::YobiAction* legacyVersionUpgradeAction = nullptr; // 原 0x9A6E68。
    std::int32_t updateAvailable = 0; // 原 0x9A6EAC；选角升级按钮门禁。
    yobi::YobiAction* deleteConfirmationAction = nullptr; // 原 0x9A6EB0；删除确认动作借用。
    std::array<yobi::YobiAction*, 2> previewActions {}; // 原 0x9A6E44、0x9A6E48。
    CharacterCreationProgressState creationProgress {};
    CharacterCreationDraftState creationDraft {};
    CharacterCreationSubmissionState creationSubmission {};
    CharacterCreationDraftSnapshotState creationSnapshot {};
    std::uint8_t creationRestore = 0; // 原 0x9A6EB8，创建场景同次调用的恢复回边。
    std::int16_t pendingCharacterCount = 0; // 原 0xF7CA06。
    std::uint16_t characterEntryState = 0; // 原 0x9A6E10。
    std::int32_t previewToggleState = 0; // 原 0x9A6E14，创建回复和选角预览共用。
    std::int32_t selectedGraphicNo = 0; // 原 0xF7C9FC，创建请求与进入世界共用。
    CharacterListProgressState listProgress {};
    CharacterDeletionProgressState deletionProgress {};
    AuthenticationFailureState authenticationFailure {};
};

} // namespace cg::character_select
