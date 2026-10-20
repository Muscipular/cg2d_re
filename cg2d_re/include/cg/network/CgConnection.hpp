#pragma once

#include "cg/platform/CgTransport.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace cg::network {

class CgConnection;

// SendPacket的本地入队结果；底层值保留Lua及既有S32业务返回合同。
enum class SendResult : std::int32_t {
    Queued = 0,          // 队列已接收，尚不代表TCP发送完成或服务端处理成功。
    QueueFull = -1,      // 队列剩余容量不足，包含单条消息过大的情形。
    Unavailable = -100,  // 收发未启用、连接不可用、会话变化或正在停止。
};

// 正文/参数借用到同步回调结束；注册表随连接对象保留，重连不清空。
using ProtocolReceiveHandler = std::function<void(CgConnection& connection, const char* header, char** arguments)>;
using ProtocolSendHandler = std::function<void(
    CgConnection& connection, const char* header, const char* data, std::int32_t length)>;

// Pump只报告终止，不在连接对象中切换场景；terminal仅在terminated为true时有效。
struct ConnectionPumpResult {
    std::uint8_t value {};
    bool terminated {};
    platform::ConnectionSnapshot terminal;
};

// CTO01：主线程协议与会话owner，组合私有CgTransport；当前业务仅接入一个实例。
class CgConnection final {
public:
    CgConnection();
    ~CgConnection();
    CgConnection(const CgConnection&) = delete;
    CgConnection& operator=(const CgConnection&) = delete;
    CgConnection(CgConnection&&) = delete;
    CgConnection& operator=(CgConnection&&) = delete;

    [[nodiscard]] bool Initialize();
    void Shutdown();
    [[nodiscard]] platform::ConnectionId BeginConnect(std::wstring host, std::uint16_t port, bool noDelay);
    [[nodiscard]] platform::ConnectionSnapshot PollConnection();
    [[nodiscard]] platform::ConnectionId Id() const noexcept;
    [[nodiscard]] bool Active() const noexcept;
    [[nodiscard]] std::int32_t Error() const noexcept;
    void SetIoEnabled(bool enabled);
    // 完整明文消息，不添加LF、不识别协议；会话身份由对象捕获并在回调后校验。
    // replacePending由业务选择是否替换旧待发字节，默认追加。
    [[nodiscard]] SendResult SendPacket(const void* data, std::uint32_t size, bool replacePending = false);
    [[nodiscard]] ConnectionPumpResult Pump();
    void RegisterReceive(const char* header, ProtocolReceiveHandler handler);
    void RegisterSend(const char* header, ProtocolSendHandler handler);
    void Close();
    void StopIo(); // Lua/UI关闭前调用；不释放主线程协议工作区。

    // 同步作用域保留入口会话；不暴露编号，退出时恢复上一层上下文。
    class SessionScope final {
    public:
        ~SessionScope();
        SessionScope(const SessionScope&) = delete;
        SessionScope& operator=(const SessionScope&) = delete;
        SessionScope(SessionScope&&) = delete;
        SessionScope& operator=(SessionScope&&) = delete;
    private:
        friend class CgConnection;
        explicit SessionScope(CgConnection& owner) noexcept;
        CgConnection& owner_;
        bool previousBound_;
        platform::ConnectionId previousId_;
    };
    // 仅限同一主线程同步调用栈，不得跨帧或跨Lua异步任务保存。
    [[nodiscard]] SessionScope CaptureSession() noexcept;
    [[nodiscard]] bool IoEnabled() const noexcept;
    void ResetLoginReply() noexcept;
    [[nodiscard]] bool LoginAccepted() const noexcept;
    // 拥有式编码结果，复制原临时区字节；文本容量与编码合同保持。
    std::string EncodeInteger(std::int32_t value);
    std::string EscapeText(std::wstring_view text);
    std::string UnescapeText(const char* text);
    // 业务请求保留原字段/返回；未初始化或已Shutdown时返回-100。

    // 账号、人物选择、登出与保活。
    std::int32_t SendAccountLoginRequest(const std::wstring_view executableDigest,
        const std::wstring_view password,
        const std::wstring_view account,
        const std::wstring_view adapterId);
    std::int32_t SendCharacterListRequest();
    std::int32_t SendCharacterCreationRequest(const std::int32_t field1,
        const std::wstring_view field2,
        const std::int32_t field3,
        const std::int32_t field4,
        const std::int32_t field5,
        const std::int32_t field6,
        const std::int32_t field7,
        const std::int32_t field8,
        const std::int32_t field9,
        const std::int32_t field10,
        const std::int32_t field11,
        const std::int32_t field12,
        const std::int32_t field13);
    std::int32_t SendCharacterDeletionRequest(const std::int32_t registNumber);
    std::int32_t SendCharacterLoginRequest(const std::int32_t field4C,
        const std::int32_t field50,
        const std::int32_t field18);
    std::int32_t SendLogoutRequest();
    std::int32_t RequestLogoutToTown();
    std::int32_t SendLoginTimestamp(const std::uint32_t tickBits);
    std::int32_t SendVersionUpgradeRequest(const std::int32_t version,
        const std::wstring_view payload);
    std::int32_t SendKeepAliveRequest();

    // 地图、移动、队伍与场景交互。
    std::int32_t SendPlayerMovementPath(const std::int32_t first,
        const std::int32_t second,
        const std::wstring_view text,
        const std::int32_t fourth);
    std::int32_t SendMapTransitionRequest(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third,
        const std::int32_t fourth,
        const std::int32_t fifth);
    std::int32_t RequestMapRectangle(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third,
        const std::int32_t fourth,
        const std::int32_t fifth,
        const std::int32_t sixth);
    std::int32_t SendPlayerCoordinates(const std::int32_t first,
        const std::int32_t second);
    std::int32_t SendPlayerPositionAndDirection(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third);
    std::int32_t SendPositionDirectionInteraction(const std::int32_t x,
        const std::int32_t y, const std::int32_t direction);
    std::int32_t SendTeamJoinLeaveRequest(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third);
    std::int32_t SendWorldActionRequest(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third);
    std::int32_t SendPlayerMotionRequest(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third);
    std::int32_t SendDialogResponse(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third,
        const std::int32_t fourth,
        const std::int32_t fifth,
        const std::wstring_view text);

    // 聊天、联系人、邮件与人物资料。
    std::int32_t SendChatMessage(const std::int32_t first,
        const std::int32_t second,
        const std::wstring_view text,
        const std::int32_t fourth,
        const std::int32_t fifth,
        const std::int32_t sixth);
    std::int32_t RequestAlbumEntries(const std::int32_t* values,
        const std::int32_t count);
    std::int32_t RequestContactDeletion(const std::int32_t value);
    std::int32_t SendContactCardRequest(const std::int32_t first,
        const std::int32_t second);
    std::int32_t SendContactMail(const std::int32_t first,
        const std::wstring_view text, const std::int32_t third);
    std::int32_t SendContactPetMail(const std::int32_t first, const std::int32_t second,
        const std::int32_t third, const std::wstring_view text, const std::int32_t fifth);
    std::int32_t RequestProfileCategoryUpdates(const std::int32_t value);
    std::int32_t SendPlayerProfile(const std::int32_t field1,
        const std::wstring_view text2,
        const std::int32_t field3,
        const std::wstring_view text4,
        const std::int32_t field5,
        const std::wstring_view text6,
        const std::wstring_view text7);
    std::int32_t RequestPlayerProfile(const std::int32_t first,
        const std::int32_t second);
    std::int32_t RequestMiniMailContactDeletion(const std::int32_t value,
        const std::wstring_view text);
    std::int32_t SendMiniMail(const std::wstring_view text,
        const std::int32_t value);
    std::int32_t SetProfileVisibilityRequest(const std::int32_t value);

    // 公会信息、成员与邮件。
    std::int32_t SendGuildTextUpdate(const std::int32_t kind, const std::wstring_view text);
    std::int32_t SendGuildRankDefinition(const std::int32_t first,
        const std::int32_t second,
        const std::wstring_view text);
    std::int32_t SendGuildMemberRankChange(const std::int32_t first,
        const std::int32_t second);
    std::int32_t SendGuildInvitation(const std::int32_t first,
        const std::int32_t second);
    std::int32_t RequestGuildMemberDeletion(const std::int32_t value);
    std::int32_t SendGuildMail(const std::int32_t first,
        const std::wstring_view text,
        const std::int32_t third);
    std::int32_t SendGuildPetMail(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third,
        const std::wstring_view text,
        const std::int32_t fifth);

    // 人物属性、称号与选项。
    std::int32_t SendPlayerOptions(const std::int32_t value);
    std::int32_t SelectPlayerTitleRequest(const std::int32_t value);
    std::int32_t DeletePlayerTitleRequest(const std::int32_t value);
    std::int32_t SetCustomPlayerTitleRequest(const std::wstring_view text);
    std::int32_t IncreasePlayerAttributeRequest(const std::int32_t value);

    // 物品、背包与银行；银行两条rh入口保留各自字段编码。
    std::int32_t RequestItemTargetAttributes();
    std::int32_t SelectItemTextTarget(const std::int32_t value);
    std::int32_t SendItemTargetPair(const std::int32_t first,
        const std::int32_t second);
    std::int32_t SendItemUseRequest(const std::int32_t x, const std::int32_t y,
        const std::int32_t item, const std::int32_t target);
    std::int32_t BeginItemUseRequest(const std::int32_t x, const std::int32_t y,
        const std::int32_t index, const std::int32_t group);
    std::int32_t DropItemRequest(const std::int32_t x, const std::int32_t y,
        const std::int32_t group, const std::int32_t index);
    std::int32_t DropGoldRequest(const std::int32_t x,
        const std::int32_t y, const std::int32_t amount);
    std::int32_t MoveItemBetweenOwnersRequest(const std::int32_t sourceGroup, const std::int32_t source,
        const std::int32_t targetGroup, const std::int32_t target, const std::int32_t amount);
    std::int32_t MoveItemStackRequest(const std::int32_t source,
        const std::int32_t target, const std::int32_t amount);
    std::int32_t SelectBankPageRequest(const std::int32_t value);
    std::int32_t MoveItemToBagPage(std::int32_t source, std::int32_t page);
    std::int32_t SelectBagPage(std::int32_t page);
    std::int32_t SelectInventoryBankPage(std::int32_t page);

    // 宠物状态、属性、技能与命名。
    std::int32_t SendPetStates(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third,
        const std::int32_t fourth,
        const std::int32_t fifth);
    std::int32_t MovePetSlotRequest(const std::int32_t first,
        const std::int32_t second);
    std::int32_t DropPetRequest(const std::int32_t x,
        const std::int32_t y,
        const std::int32_t pet);
    std::int32_t IncreasePetAttributeRequest(const std::int32_t first,
        const std::int32_t second);
    std::int32_t SwapPetSkillOrderRequest(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third);
    std::int32_t RenamePetRequest(const std::int32_t value,
        const std::wstring_view text);

    // 技能目标、使用、合成与重生。
    std::int32_t RequestSkillTargetAttributes(const std::int32_t value);
    std::int32_t SelectSkillTextTarget(const std::int32_t value);
    std::int32_t SendSkillUseRequest(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third,
        const std::wstring_view text);
    std::int32_t SwapPlayerSkillOrderRequest(const std::int32_t first,
        const std::int32_t second);
    std::int32_t SendCraftDuration(const std::int32_t value);
    std::int32_t SendSkillRebirthSelection(const std::int32_t value);

    // 交易与摆摊。
    std::int32_t SendTradeRequest();
    std::int32_t SelectTradePartnerRequest(const std::int32_t index);
    std::int32_t SendTradeOffer(const std::wstring_view first,
        const std::wstring_view second,
        const std::int32_t third);
    std::int32_t SendTradeDecision(const std::int32_t value);
    std::int32_t WithdrawTradeConfirmation();
    std::int32_t PublishStallRequest(const std::wstring_view first,
        const std::wstring_view second,
        const std::wstring_view third);
    std::int32_t CloseStallRequest();
    std::int32_t RequestStallContents(const std::int32_t value);
    std::int32_t PurchaseStallOffer(const std::int32_t value);

    // 战斗、观战与退出确认。
    std::int32_t ToggleBattleRowRequest();
    std::int32_t SendDuelRequest(const std::int32_t first,
        const std::int32_t second);
    std::int32_t SendSpectateRequest(const std::int32_t first,
        const std::int32_t second);
    std::int32_t SendIuEPacket();
    std::int32_t SendBattlePacket(const std::wstring_view text);
    std::int32_t AcknowledgeBattleExit(const std::int32_t value);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// 主线程唯一业务连接，寿命覆盖初始化、重连和退出。
CgConnection& GetGameConnection() noexcept;

} // namespace cg::network
