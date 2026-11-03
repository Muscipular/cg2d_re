#pragma once

#include "cg/platform/CgTransport.hpp"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <string_view>

namespace cg::login { struct LoginRequestState; }

namespace cg::network {

class CgConnection;

// 通用与具名业务发包的本地入队结果；底层值保留Lua数字返回合同。
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
    std::uint8_t value = 0;
    bool terminated = false;
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
    // NSM01：原 457AB0 的时钟与发送顺序随本连接实例保留。
    void UpdateKeepAlive();
    // NSM01：请求/回复回调使用同一连接的登录请求状态。
    [[nodiscard]] login::LoginRequestState& LoginRequests() noexcept;
    // 拥有式编码结果，复制原临时区字节；文本容量与编码合同保持。
    std::string EncodeInteger(std::int32_t value);
    std::string EscapeText(std::wstring_view text);
    std::string UnescapeText(const char* text);
    // 业务请求保留原字段与入队结果；未初始化或已Shutdown时返回Unavailable。

    // 账号、人物选择、登出与保活。
    SendResult SendAccountLoginRequest(const std::wstring_view executableDigest,
        const std::wstring_view password,
        const std::wstring_view account,
        const std::wstring_view adapterId);
    SendResult SendCharacterListRequest();
    SendResult SendCharacterCreationRequest(const std::int32_t field1,
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
    SendResult SendCharacterDeletionRequest(const std::int32_t registNumber);
    SendResult SendCharacterLoginRequest(const std::int32_t field4C,
        const std::int32_t field50,
        const std::int32_t field18);
    SendResult SendLogoutRequest();
    SendResult RequestLogoutToTown();
    SendResult SendLoginTimestamp(const std::uint32_t tickBits);
    SendResult SendVersionUpgradeRequest(const std::int32_t version,
        const std::wstring_view payload);
    SendResult SendKeepAliveRequest();

    // 地图、移动、队伍与场景交互。
    SendResult SendPlayerMovementPath(const std::int32_t first,
        const std::int32_t second,
        const std::wstring_view text,
        const std::int32_t fourth);
    SendResult SendMapTransitionRequest(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third,
        const std::int32_t fourth,
        const std::int32_t fifth);
    SendResult RequestMapRectangle(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third,
        const std::int32_t fourth,
        const std::int32_t fifth,
        const std::int32_t sixth);
    SendResult SendPlayerCoordinates(const std::int32_t first,
        const std::int32_t second);
    SendResult SendPlayerPositionAndDirection(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third);
    SendResult SendPositionDirectionInteraction(const std::int32_t x,
        const std::int32_t y, const std::int32_t direction);
    SendResult SendTeamJoinLeaveRequest(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third);
    SendResult SendWorldActionRequest(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third);
    SendResult SendPlayerMotionRequest(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third);
    SendResult SendDialogResponse(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third,
        const std::int32_t fourth,
        const std::int32_t fifth,
        const std::wstring_view text);

    // 聊天、联系人、邮件与人物资料。
    SendResult SendChatMessage(const std::int32_t first,
        const std::int32_t second,
        const std::wstring_view text,
        const std::int32_t fourth,
        const std::int32_t fifth,
        const std::int32_t sixth);
    SendResult RequestAlbumEntries(const std::int32_t* values,
        const std::int32_t count);
    SendResult RequestContactDeletion(const std::int32_t value);
    SendResult SendContactCardRequest(const std::int32_t first,
        const std::int32_t second);
    SendResult SendContactMail(const std::int32_t first,
        const std::wstring_view text, const std::int32_t third);
    SendResult SendContactPetMail(const std::int32_t first, const std::int32_t second,
        const std::int32_t third, const std::wstring_view text, const std::int32_t fifth);
    SendResult RequestProfileCategoryUpdates(const std::int32_t value);
    SendResult SendPlayerProfile(const std::int32_t field1,
        const std::wstring_view text2,
        const std::int32_t field3,
        const std::wstring_view text4,
        const std::int32_t field5,
        const std::wstring_view text6,
        const std::wstring_view text7);
    SendResult RequestPlayerProfile(const std::int32_t first,
        const std::int32_t second);
    SendResult RequestMiniMailContactDeletion(const std::int32_t value,
        const std::wstring_view text);
    SendResult SendMiniMail(const std::wstring_view text,
        const std::int32_t value);
    SendResult SetProfileVisibilityRequest(const std::int32_t value);

    // 公会信息、成员与邮件。
    SendResult SendGuildTextUpdate(const std::int32_t kind, const std::wstring_view text);
    SendResult SendGuildRankDefinition(const std::int32_t first,
        const std::int32_t second,
        const std::wstring_view text);
    SendResult SendGuildMemberRankChange(const std::int32_t first,
        const std::int32_t second);
    SendResult SendGuildInvitation(const std::int32_t first,
        const std::int32_t second);
    SendResult RequestGuildMemberDeletion(const std::int32_t value);
    SendResult SendGuildMail(const std::int32_t first,
        const std::wstring_view text,
        const std::int32_t third);
    SendResult SendGuildPetMail(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third,
        const std::wstring_view text,
        const std::int32_t fifth);

    // 人物属性、称号与选项。
    SendResult SendPlayerOptions(const std::int32_t value);
    SendResult SelectPlayerTitleRequest(const std::int32_t value);
    SendResult DeletePlayerTitleRequest(const std::int32_t value);
    SendResult SetCustomPlayerTitleRequest(const std::wstring_view text);
    SendResult IncreasePlayerAttributeRequest(const std::int32_t value);

    // 物品、背包与银行；银行两条rh入口保留各自字段编码。
    SendResult RequestItemTargetAttributes();
    SendResult SelectItemTextTarget(const std::int32_t value);
    SendResult SendItemTargetPair(const std::int32_t first,
        const std::int32_t second);
    SendResult SendItemUseRequest(const std::int32_t x, const std::int32_t y,
        const std::int32_t item, const std::int32_t target);
    SendResult BeginItemUseRequest(const std::int32_t x, const std::int32_t y,
        const std::int32_t index, const std::int32_t group);
    SendResult DropItemRequest(const std::int32_t x, const std::int32_t y,
        const std::int32_t group, const std::int32_t index);
    SendResult DropGoldRequest(const std::int32_t x,
        const std::int32_t y, const std::int32_t amount);
    SendResult MoveItemBetweenOwnersRequest(const std::int32_t sourceGroup, const std::int32_t source,
        const std::int32_t targetGroup, const std::int32_t target, const std::int32_t amount);
    SendResult MoveItemStackRequest(const std::int32_t source,
        const std::int32_t target, const std::int32_t amount);
    SendResult SelectBankPageRequest(const std::int32_t value);
    SendResult MoveItemToBagPage(std::int32_t source, std::int32_t page);
    SendResult SelectBagPage(std::int32_t page);
    SendResult SelectInventoryBankPage(std::int32_t page);

    // 宠物状态、属性、技能与命名。
    SendResult SendPetStates(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third,
        const std::int32_t fourth,
        const std::int32_t fifth);
    SendResult MovePetSlotRequest(const std::int32_t first,
        const std::int32_t second);
    SendResult DropPetRequest(const std::int32_t x,
        const std::int32_t y,
        const std::int32_t pet);
    SendResult IncreasePetAttributeRequest(const std::int32_t first,
        const std::int32_t second);
    SendResult SwapPetSkillOrderRequest(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third);
    SendResult RenamePetRequest(const std::int32_t value,
        const std::wstring_view text);

    // 技能目标、使用、合成与重生。
    SendResult RequestSkillTargetAttributes(const std::int32_t value);
    SendResult SelectSkillTextTarget(const std::int32_t value);
    SendResult SendSkillUseRequest(const std::int32_t first,
        const std::int32_t second,
        const std::int32_t third,
        const std::wstring_view text);
    SendResult SwapPlayerSkillOrderRequest(const std::int32_t first,
        const std::int32_t second);
    SendResult SendCraftDuration(const std::int32_t value);
    SendResult SendSkillRebirthSelection(const std::int32_t value);

    // 交易与摆摊。
    SendResult SendTradeRequest();
    SendResult SelectTradePartnerRequest(const std::int32_t index);
    SendResult SendTradeOffer(const std::wstring_view first,
        const std::wstring_view second,
        const std::int32_t third);
    SendResult SendTradeDecision(const std::int32_t value);
    SendResult WithdrawTradeConfirmation();
    SendResult PublishStallRequest(const std::wstring_view first,
        const std::wstring_view second,
        const std::wstring_view third);
    SendResult CloseStallRequest();
    SendResult RequestStallContents(const std::int32_t value);
    SendResult PurchaseStallOffer(const std::int32_t value);

    // 战斗、观战与退出确认。
    SendResult ToggleBattleRowRequest();
    SendResult SendDuelRequest(const std::int32_t first,
        const std::int32_t second);
    SendResult SendSpectateRequest(const std::int32_t first,
        const std::int32_t second);
    SendResult SendIuEPacket();
    SendResult SendBattlePacket(const std::wstring_view text);
    SendResult AcknowledgeBattleExit(const std::int32_t value);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

// 主线程唯一业务连接，寿命覆盖初始化、重连和退出。
CgConnection& GetGameConnection() noexcept;

} // namespace cg::network
