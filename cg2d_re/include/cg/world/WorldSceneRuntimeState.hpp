#pragma once

#include "cg/world/PlayerSlots.hpp"
#include "cg/world/PlayerItemRecord.hpp"
#include "cg/world/PlayerRuntimeSlots.hpp"
#include "cg/world/PetRuntimeRecord.hpp"
#include "cg/world/SkillRuntimeSlots.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

namespace cg::yobi { struct YobiAction; }
namespace cg::world {

// 战斗具名效果记录，空名称表示未使用槽。
struct BattleNamedEffectRecord {
    std::wstring name; // 原 +0。
    std::int32_t value = 0; // 原 +20。
};

// 断线场景确认框的文字、布局和动作借用。
struct DisconnectDialogState {
    std::wstring text; // 原 0x9A4D10。
    yobi::YobiAction* action = nullptr; // 原 0x62B5B4。
    std::int32_t sentinel = 0; // 原 0x62B3A8。
    std::int32_t width = 0; // 原 0x62B32C。
    std::int32_t height = 0; // 原 0x62B304。
    std::int32_t x = 0; // 原 0x62B390。
    std::int32_t y = 0; // 原 0x62B2EC。
};

// 场景 7 的登出过渡、请求进度与结果提示；动作由 Yobi 持有。
struct LogoutSceneState {
    std::uint32_t transitionPhase = 0; // 原 0x9A6E94。
    std::wstring text; // 原 0x9A5B08。
    std::uint32_t requestPhase = 0; // 原 0x9A6204。
    std::int32_t rows = 0; // 原 0x9A5674。
    std::int32_t columns = 0; // 原 0x9A594C。
    std::int32_t top = 0; // 原 0x9A61F8。
    std::int32_t left = 0; // 原 0x9A5AC0。
    yobi::YobiAction* action = nullptr; // 原 0x9A6E98。
};

// 原FF0C14四条结果记录；50D500按两次W16、一次S32的顺序发布。
struct BattleResultRecord {
    std::uint16_t value00; // 原成员偏移：+0x00。
    std::uint16_t value02; // 原成员偏移：+0x02。
    std::int32_t value04; // 原成员偏移：+0x04。
};
static_assert(offsetof(BattleResultRecord, value00) == 0);
static_assert(offsetof(BattleResultRecord, value02) == 2);
static_assert(offsetof(BattleResultRecord, value04) == 4);
static_assert(sizeof(BattleResultRecord) == 8);

// 原FF0C8C十五条技能结果；+2两字节不由50D500写入，现随技能容量扩展。
struct BattleSkillResultRecord {
    std::int16_t value00; // 原成员偏移：+0x00。
    std::array<std::byte, 2> bytes02; // 原成员偏移：+0x02。
    std::int32_t value04; // 原成员偏移：+0x04。
};
static_assert(offsetof(BattleSkillResultRecord, value00) == 0);
static_assert(offsetof(BattleSkillResultRecord, bytes02) == 2);
static_assert(offsetof(BattleSkillResultRecord, value04) == 4);
static_assert(sizeof(BattleSkillResultRecord) == 8);

// 网络结果、场景待显示模式与窗口30/31共用到期时钟属于同一会话。
struct BattleResultState {
    std::uint16_t pendingMode = 0; // 原0xFF0C10，分发按S16读取。
    std::array<BattleResultRecord, 4> records {}; // 原0xFF0C14。
    std::array<BattleSkillResultRecord, kSkillSlotCount> skills {}; // 原0xFF0C8C。
    std::array<std::wstring, 3> names {}; // 原0xFF0C34。
    std::array<std::int32_t, 3> graphics {}; // 原0xFF0D0C。
    std::int32_t dpDelta = 0; // 原0xFF0D04。
    std::int32_t dpTotal = 0; // 原0xFF0D08。
    std::uint64_t closeTick = 0; // 原0xCB0F78，两个结果窗共用，不按窗口复制。
};
static_assert(sizeof(BattleResultState::records) == 0x20);
static_assert(sizeof(BattleResultState::skills) == sizeof(BattleSkillResultRecord) * kSkillSlotCount);
static_assert(sizeof(BattleResultState::graphics) == 0x0C);

// 地图名计时提示的时间、非拥有开窗动作及目标格门禁。
struct MapNoticeState {
    std::uint64_t startTick = 0; // 原0x62B5B0。
    yobi::YobiAction* opening = nullptr; // 原0x62B5B8，动画完成通过此稳定槽清空。
    std::uint32_t inputAndDrawOverride = 0; // 原0x62B398。
};

// 战斗场景的遍历游标与退出动作借用；动作本体由 Yobi 持有。
struct BattleSceneState {
    std::int32_t sceneIndex = 0; // 原 0x5ABFD8。
    yobi::YobiAction* sceneAction = nullptr; // 原 0x5ABA94，非拥有场景根动作。
    yobi::YobiAction* exitAction = nullptr; // 原 0x5ABCDC。
    yobi::YobiAction* entryNoticeAction = nullptr; // 原0x5ABFB8，窗口32开窗动作的非拥有引用。
    std::int32_t backgroundImage = 0; // 原 0x59F9F8，入战回复、淡变动作和重建共用。
    std::int32_t backgroundSnapshot = 0; // 原 0x5ABCD8，绘制后重读的背景图号。
    std::uint32_t exitNotificationPending = 0; // 原 0x5ABCB0。
    std::uint32_t entryPending = 0; // 原 0x5ABFC4，入战回复与世界场景共用。
    std::uint32_t firstTurnPending = 0; // 原 0x5ABC6C，首条 A 回复更新回合快照。
    std::uint8_t attributeEffectFlags = 0; // 原 0x59F9E0，称号值决定属性效果门禁。
    std::int32_t battleType = 0; // 原 0x5ABFC8，入战回复完整 S32 类型。
    std::uint32_t exitRequested = 0xFFFFFFFFu; // 原 0x5ABB20，初值、初始化和退出动作共用。
    std::uint32_t endBits = 0; // 原 0x5ABBC0，A 回复中的角色结束位。
    std::uint32_t turn = 0; // 原 0x5ABB74，A 回复回合数。
    std::uint32_t turnSnapshot = 0; // 原 0x5ABC08，首包及事件分发回合快照。
    std::int32_t petSkillGraphicIndex = -1; // 原 0x553F8C，宠物技能图形绘制索引。
    std::int32_t cWriteIndex = 0; // 原 0x5AAA6C，C 四槽写索引。
    std::int32_t cReadIndex = 0; // 原 0x5AAA44，C 四槽读索引。
    std::int32_t mWriteIndex = 0; // 原 0x5A3A1C，M 四槽写索引。
    std::int32_t mReadIndex = 0; // 原 0x5AAA68，M 四槽读索引。
    std::array<char, 0x4000> cQueue {}; // 原 0x5A4A3C，四槽各 0x1000。
    std::array<char, 0x4000> mQueue {}; // 原 0x59FA18，四槽各 0x1000。
    std::array<char, 0x1000> cText {}; // 原 0x5A3A20，活动 C 消息。
    std::int32_t cOffset = 0; // 原 0x5ABA80，C 字段解析游标。
    std::uint32_t fieldFlags = 0; // 原 0x5ABB58，C 消息字段标志。
    std::array<std::array<std::int32_t, 2>, 22> effectPairs {}; // 原 0x5A8A40，预扫描与角色效果共用。
    std::array<BattleNamedEffectRecord, 20> namedEffects {}; // 原 0x5ABCE0，名称映射效果值。
    std::array<char, 0x1000> mText {}; // 原 0x5AAA7C，活动 M 消息。
    std::int32_t mScanOffset = 0; // 原 0x5A8A3C，M 头扫描游标。
    std::int32_t mPreviewOffset = 0; // 原 0x5AAA60，M 预扫描游标。
    std::uint32_t activeEventMask = 0; // 原 0x5AAA74，32 个事件槽位图。
    std::array<yobi::YobiAction*, 32> eventActions {}; // 原 0x5ABA9C，非拥有事件引用。
    std::array<std::uint8_t, 22> previewParticipants {}; // 原 0x5AAA48，预扫描冲突计数。
    std::uint32_t eventDispatchBusy = 0; // 原 0x5ABB1C，事件分发门禁。
    std::uint32_t turnMessagesExhausted = 0; // 原 0x5ABB28，当前回合 M 文本耗尽。
    std::int32_t pendingGroupCount = 0; // 原 0x5ABB24，延迟事件组数。
};
static_assert(sizeof(BattleSceneState::cQueue) == 4 * 0x1000);
static_assert(sizeof(BattleSceneState::mQueue) == 4 * 0x1000);
static_assert(sizeof(BattleSceneState::cText) == 0x1000);
static_assert(sizeof(BattleSceneState::mText) == 0x1000);
static_assert(sizeof(BattleSceneState::effectPairs) == 22 * 2 * sizeof(std::int32_t));
static_assert(sizeof(BattleSceneState::namedEffects) == 20 * sizeof(BattleNamedEffectRecord));
static_assert(sizeof(BattleSceneState::eventActions) == 32 * sizeof(yobi::YobiAction*));

// 100 项随机旋转表及跨场景消费游标，表初始化不重置游标。
struct RotationState {
    std::array<std::int32_t, 100> table {}; // 原 0xC8A7B0，长度 0x190。
    std::int32_t cursor = 0; // 原 0xC8A940。
};

// 聊天发送节流及回复生产的独立门禁，同一账号会话内保持原读写序。
struct ChatSendState {
    std::uint64_t previousTick = 0; // 原 0xDA3B3C。
    std::uint64_t currentTick = 0; // 原 0xDA3B40。
    std::uint8_t disabled = 0; // 原 0xDA3B4C。
    std::int32_t replyGate = 0; // 原 0xDA3B54。
};

// 原 [0x5FB7F8, 0x621FF8)：512 项、每项 0x134 字节，保留未明字段。
struct Record5FB7F8 {
    std::wstring text; // 原成员偏移：+0x00。
    std::int32_t color; // 原成员偏移：+0x124。
    std::int32_t state; // 原成员偏移：+0x128。
    std::int32_t field12C; // 原成员偏移：+0x12C。
    std::int32_t fontType; // 原成员偏移：+0x130。
};

// 显示环表及范围提示由会话持有；原 volatile 发布和前项字体别名保持。
struct ChatDisplayState {
    Record5FB7F8 records[512] {}; // 原 0x5FB7F8。
    volatile std::int32_t writeIndex = 0; // 原 0x626C08。
    volatile std::int32_t count = 0; // 原 0xC9A944。
    volatile std::uint32_t lifetime = 0; // 原 0x626C10。
    std::uint32_t rangeOverlayFrames = 0; // 原 0x626BFC。
    std::int32_t range = 5; // 原 0x554640。
};

// 日志写流与日期文件名槽属于会话；查看器继续使用同一个文件名槽。
struct ChatLogState {
    std::wstring path; // 原 0x62699C。
    std::FILE* volatile stream = nullptr; // 原 0x626C1C。
    std::wstring fileName = L"chat_000000.txt"; // 原 0x554648。

    ChatLogState() = default;
    ~ChatLogState()
    {
        if (std::FILE* const current = stream; current != nullptr) { std::fclose(current); }
    }
    ChatLogState(const ChatLogState&) = delete;
    ChatLogState& operator=(const ChatLogState&) = delete;
    ChatLogState(ChatLogState&&) = delete;
    ChatLogState& operator=(ChatLogState&&) = delete;
};

// 64 条聊天输入历史、磁盘尾索引与当前浏览游标，运行期由会话持有。
struct ChatInputHistoryState {
    std::array<std::wstring, 64> records {}; // 原 0x621FF8。
    std::uint32_t tail626838 = 0; // 原 0x626838。
    std::int32_t cursor62683C = 0; // 原 0x62683C。
};

// 地图角色图号/坐标去重键；每帧只清计数，表尾保留原内容。
struct MapActorDrawKey {
    std::int32_t graphicIndex;
    std::int32_t mapX;
    std::int32_t mapY;
};
static_assert(sizeof(MapActorDrawKey) == 12);

// 四象限转场的阶段及坐标；同一场景会话内四块连续推进。
struct QuadrantTransitionState {
    std::int32_t phase = -1; // 原 0x58B6D8，加载区初值 -1。
    std::array<std::int32_t, 4> x {}; // 原 0xFF275C。
    std::array<std::int32_t, 4> y {}; // 原 0xFF47D0。
};
static_assert(sizeof(QuadrantTransitionState::x) == 16);
static_assert(sizeof(QuadrantTransitionState::y) == 16);

// 64 画块随机显隐/下落转场；状态 1/2 的意义由模式决定。
struct TileTransitionState {
    std::int32_t phase = -1; // 原 0x58B6DC，加载区初值 -1。
    std::int32_t completed = 0; // 原 0xFF46C0。
    std::int32_t started = 0; // 原 0xFF28BC。
    std::array<std::int32_t, 64> state {}; // 原 0xFF27B8。
    std::array<std::int32_t, 64> speed {}; // 原 0xFF46C4。
    std::array<std::int32_t, 64> sourceY {}; // 原 0xFF47E4。
    std::array<std::int32_t, 64> y {}; // 原 0xFF0D7C。
    std::array<std::int32_t, 64> x {}; // 原 0xFF265C。
    std::array<std::int32_t, 64> sourceX {}; // 原 0xFF0E7C。
};

struct GridTransitionOffset {
    std::int32_t x;
    std::int32_t y;
};
static_assert(sizeof(GridTransitionOffset) == 8);
static_assert(offsetof(GridTransitionOffset, y) == 4);

// 网格/顶底扫描线共用初始化标志和比例，中心扫描线的比例独立。
struct ImageTransitionState {
    std::uint32_t initialized = 0; // 原 0xFF4910。
    std::int32_t scale = 0; // 原 0xFF0D78。
    std::int32_t halfScale = 0; // 原 0xFF0F7C。
    std::array<GridTransitionOffset, 960> offsets {}; // 原 0xFF28C0。
};
static_assert(sizeof(ImageTransitionState::offsets) == 0xFF46C0 - 0xFF28C0);

// 原三组 481 项滑屏行数据；窗口布局仅按尺寸增量扩容，不覆盖已有内容。
struct SlideTransitionLines {
    std::vector<float> speed = std::vector<float>(481, 0.0f); // 原 0xFF0FA8。
    std::vector<float> x = std::vector<float>(481, 0.0f); // 原 0xFF172C。
    std::vector<float> y = std::vector<float>(481, 0.0f); // 原 0xFF1EB0。
};

// 九模式滑屏的前向与反向数值。
struct SlideTransitionState {
    SlideTransitionLines lines {};
    float x = 0.0f; // 原 0xFF48E8。
    float alternateX = 0.0f; // 原 0xFF48EC。
    float y = 0.0f; // 原 0xFF48F0。
    float alternateY = 0.0f; // 原 0xFF48F4。
    float velocity = 0.0f; // 原 0xFF48F8。
    float activeLines = 0.0f; // 原 0xFF48FC。
    std::int32_t reversePhase = -1; // 原 0x58B6D4，加载区初值 -1。
    std::int32_t reverseActiveLines = 0; // 原 0xFF276C。
    float reverseVelocity = 0.0f; // 原 0xFF47C4。
    float reverseX = 0.0f; // 原 0xFF4900。
    float reverseAlternateX = 0.0f; // 原 0xFF4904。
    float reverseY = 0.0f; // 原 0xFF4908。
    float reverseAlternateY = 0.0f; // 原 0xFF490C。
};

// NPC对话及各操作窗口共用的请求/距离快照，不与窗口51的独立请求槽合并。
struct NpcDialogRuntimeState {
    std::int32_t type = -1; // 原0x581800。
    std::int32_t value = 0; // 原0xCF4E5C，当前保留原第二参数发布。
    std::int32_t request = 0; // 原0xCC21E0。
    std::int32_t target = 0; // 原0xD2DE6C。
    std::int32_t originX = 0; // 原0xCC4D34，普通S32而非CryptInt。
    std::int32_t originY = 0; // 原0xCC4D3C。
};

// 世界场景的会话运行期数据；进程设备及只读资源不在此复制。
struct WorldSceneRuntimeState {
    std::unique_ptr<PlayerRuntimeSlots> player; // 原FDA700，中央初始化前为空，独占玩家主记录。
    std::unique_ptr<std::array<PetRuntimeRecord, 5>> petRecords; // 原F633F8，原中央位置构造五宠物。
    std::array<PetPairRecord, 5> petPairs {}; // 原DE0B08，同会话宠物显示排序对。
    std::int32_t petCount = 0; // 原DE0B00。
    std::uint32_t petDetailDraftChanged = 0; // 原D67B28，详情与属性/技能窗口共用。
    SkillRuntimeSlots playerSkills {}; // 原F1BAE0，职业名/技能/索引对的实际唯一owner。
    std::uint16_t skillReorderPending = 0; // 原DA3B20，独立于技能聚合清理的拖放门禁。
    std::array<TitleRecord, 64> playerTitles {}; // 原FF0508，原始称号表。
    std::array<TitleRecord, 64> sortedPlayerTitles {}; // 原DE03F8，独立排序副本。
    std::int32_t initialDuelPoints = 0; // 原FDA6F8，不在原玩家六块清理范围。
    std::int32_t unspentAttributePoints = 0; // 原D2F788。
    std::uint32_t gameplayRepliesSuppressed = 0; // 原9A6E0C，完整W32门禁。
    std::uint32_t logoutToTownPending = 0; // 原D67E78。
    std::array<PetMapCharaSlot, 5> petMapSlots {}; // 原FF02E0，动作保持非拥有借用。
    alignas(std::uint32_t) std::array<std::uint16_t, 5> petFollowOrder {}; // 原9FB938，原10B别名区。
    std::uint16_t partyActive = 0; // 原FF0D36，协议与场景共用。
    DisconnectDialogState disconnectDialog {};
    LogoutSceneState logout {};
    BattleSceneState battle {};
    BattleResultState battleResult {};
    MapNoticeState mapNotice {};
    NpcDialogRuntimeState npcDialog {};
    std::array<PlayerItemRecord, 28> playerItems {}; // 原FDA8A4..FF02B4，实际玩家物品唯一owner。
    RotationState rotation {};
    ChatSendState chat {};
    ChatDisplayState chatDisplay {};
    ChatLogState chatLog {};
    ChatInputHistoryState chatInputHistory {};
    QuadrantTransitionState quadrantTransition {};
    TileTransitionState tileTransition {};
    ImageTransitionState imageTransition {};
    SlideTransitionState slideTransition {};
    std::uint32_t transitionResetPending = 1; // 原 0x58B6D0，各转场模式共用的首次初始化门禁。
    std::uint32_t capturedTransitionVariant = 0; // 原 0xFF48E4，捕获画面后的完整 W32 取样。
    std::array<MapActorDrawKey, 100> mapActorDrawKeys {}; // 原 0x5ABFE0。
    std::int32_t mapActorDrawKeyCount = 0; // 原 0x5AC494。
    std::uint8_t worldEntryRepliesPendingMask = 0; // 原 0x9A4B94，入场等待与地图回复的两位。
    std::int32_t pendingMapPaletteIndex = 0; // 原 0xFF0D4C，地图名回复生产、场景调色板消费。
    std::uint32_t mapRectangleRequestPending = 0; // 原 0xC82208，请求、CRC/数据回复共用。
    std::uint32_t mapTransitionReplyPending = 0; // 原 0xC82210，转场等待与回复共用。
    std::uint32_t mapTransitionAwaitingRectangle = 0; // 原 0xC82214，转场及矩形回复共用。
    std::uint32_t mapNameReceivedDuringTransition = 0; // 原 0xC82218，地图名及矩形回复共用。
    std::uint32_t mapTransitionObservationPrepared = 0; // 原 0xC8220C，地图变化回复消费。
    std::uint32_t mapTransitionRequestSequence = 0; // 原 0xC8221C，两类请求共用递增序号。
    std::uint16_t pendingMapTransitionSequence = 0; // 原 0x9DAD54，普通转场回复匹配。
    std::uint16_t pendingDirectionalTransitionSequence = 0; // 原 0x9FB92C，定向转场回复匹配。
    std::int32_t frameBackgroundMode = 0; // 原 0x73ECC0，世界场景生产、主循环清屏消费。
    std::int32_t forceRender = 0; // 原 0x73ECE4，普通场景请求、主循环消费。
    std::uint32_t pendingMapBgmIndex = 0; // 原 0xFF0D5C，地图回复生产、普通场景消费。
    std::uint32_t mapBgmChangePending = 0; // 原 0xFF0D58。
    std::int32_t worldBgmResumeIndex = 0; // 原 0xFF0D68，普通场景恢复号。
    std::int32_t normalSceneInitialized = 0; // 原 0xFF0D50，普通场景初始化门禁。
    std::int32_t normalSceneWaitFrames = 0; // 原 0xFF0D54，普通场景等待帧数。
    std::uint32_t observationTravelTransitionCount = 0; // 原 0xFF0D74，观察转场次数。
    std::uint64_t lastAdjacentInteractionTick = 0; // 原 0xFF0D38，邻格交互节流。
    std::uint32_t movementPathFourth = 0; // 原 0x9EE8F8，地图回复生产、zA 发送消费。
    std::uint64_t lastCursorFacingRequestTick = 0; // 原 0xC82238，目标格转向节流。
    std::uint64_t lastExplicitFacingRequestTick = 0; // 原 0xC8223C，显式转向节流。
    std::array<char, 100> moveDirectionText {}; // 原 0x9E03D0，先写前缀，发送门禁后才写 NUL。
    std::uint64_t mapRectangleRequestStartTick = 0; // 原 0xAF10C8，缺块请求取时钟后发布。
};

static_assert(sizeof(WorldSceneRuntimeState::petFollowOrder) == 10);

} // namespace cg::world
