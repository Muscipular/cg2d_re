#include "cg/network/CgConnection.hpp"
#include "cg/core/LegacyNoOp.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/script/LuaRuntime.hpp"
#include "cg/core/m_log.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/text/Encoding.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/platformTool.hpp"

#include "../scene/character_select/CharacterSelectSceneDependencies.hpp"
#include "../scene/login/LoginRequestDependencies.hpp"
#include "../scene/login/LoginStateDependencies.hpp"
#include "../world/MailState.hpp"
#include "../platform/MainLoopDependencies.hpp"
#include "NetworkRuntimeDependencies.hpp"
#include "NetworkDispatchDependencies.hpp"

#include "cg/scene/login/CharacterCreationRequest.hpp"
#include "cg/scene/login/CharacterDeletionRequest.hpp"
#include "cg/scene/login/CharacterListRequest.hpp"
#include "cg/input/Input.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "cg/world/GuildState.hpp"
#include "cg/scene/login/LoginConnection.hpp"
#include "cg/scene/login/LoginRequest.hpp"
#include "cg/scene/login/LoginScene.hpp"
#include "cg/render/Palette.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include "cg/world/SceneState.hpp"
#include "../world/AlbumState.hpp"
#include "../world/BattleState.hpp"
#include "../world/ChatHistory.hpp"
#include "../world/PlayerState.hpp"
#include "../world/WorldMapState.hpp"
#include "../world/WorldObjectState.hpp"
#include "../world/WorldUpdateDependencies.hpp"

#include <lz4.h>

#include <ctime>
#include <Windows.h> // 原调试输出与系统错误常量；网络SDK操作已移至平台后端。

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstdlib> // 原CRT接口：0x0053C649、0x0053BEC0、0x0053BE40；原参数／返回合同由直接调用承接。
#include <cstring>
#include <format>
#include <list>
#include <string>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <cstdio> // 原CRT：0x0053B1D0、0x0053B2F1；直接沿MSVC文件流。 // 原格式化／扫描接口：0x0053B304、0x0053BC0F；直接沿MSVC CRT。

#include "cg/core/vm.h"

// CTM01：本地CRT时间槽显式__time64_t；协议与存档字段仍按各自原格式承接。
static_assert(sizeof(__time64_t) == sizeof(std::int64_t));
static_assert(alignof(__time64_t) == alignof(std::int64_t));

namespace cg::network {

// N02：参考块输出、整帧输出与解压正文累积容量。
static constexpr std::int32_t kLz4BlockSize = 0x4000;
static constexpr std::size_t kLz4FrameDecodedCapacity = 0x8100;
static constexpr std::size_t kDecodedQueueCapacity = 0x80000;
static constexpr std::uint8_t kLz4FrameMarker = 0xEF;
static constexpr std::size_t kWorkspaceSlotCount = 18;
static constexpr std::size_t kReceiveQueueOffset = 0xC004;
static constexpr std::size_t kQueueCapacity = 0x4000;
static constexpr std::size_t kSendLengthOffset = kReceiveQueueOffset + kQueueCapacity;
static constexpr std::size_t kQueueStorageSize = kSendLengthOffset + sizeof(std::int32_t);
static constexpr std::size_t kDispatchOffset = 0x4000;
static constexpr std::size_t kDispatchSize = 0x8000;
static_assert(kReceiveQueueOffset == 0xD9B2BC - 0xD8F2B8);
static_assert(kSendLengthOffset == 0xD9F2BC - 0xD8F2B8);
static_assert(kQueueStorageSize == 0x10008);
static_assert(kDispatchOffset == 0xD932B8 - 0xD8F2B8);
static_assert(kDispatchOffset + kDispatchSize + 4 == kReceiveQueueOffset);

using SendProtocolObserver = void (*)(
    std::uint32_t direction,
    char* buffer);

// N02：网络对象拥有半帧、连续块输出和待分行正文，不借用recv局部缓冲。
struct CompressedReceiveState {
    std::vector<std::uint8_t> frame; // size为头部声明的总帧长，包含四字节头。
    std::size_t frameBytes = 0; // 已填充的帧字节数。
    // 多一个块仅供“先解压再判逻辑容量”，不得发布超过0x8100的整帧输出。
    std::array<char, kLz4FrameDecodedCapacity + kLz4BlockSize> frameOutput {};
    std::array<char, kDecodedQueueCapacity> lines {};
    std::size_t lineBytes = 0; // 含完整LF行及尚未收齐的尾行。
};

// CTO01：每个连接对象私有的主线程协议工作区；不暴露可变状态。
struct CgConnection::Impl {
    CgConnection& owner; // 仅主线程使用，后台传输不借用连接对象。
    bool contextBound = false;
    platform::ConnectionId contextId = 0;
    explicit Impl(CgConnection& connection) : owner(connection) {}
    std::uint16_t protocolModeDA3B22 = 0; // 原全局：0x00DA3B22。
    __time64_t time62B394 = 0; // 原全局：0x0062B394。 原 __time32_t，独立于 0xDA3B28 回复码。
    std::tm* localTime62B380 = nullptr; // 原全局：0x0062B380。 原 W32 指针槽，借用CRT同线程共享std::tm。
    std::uint32_t workspaceCapacity = 0;
    std::uint8_t* sendMessageBuffer = nullptr; // 原全局：0x0058B5F0。 发送命令与字段的完整消息拼接缓冲区。
    std::uint8_t* reservedProtocolBuffer = nullptr; // 原全局：0x0058B5F4。 保留缓冲区，当前仅有生命周期操作。
    std::uint8_t* textTokenBuffer = nullptr; // 原全局：0x0058B5F8。 文本字段转义、反转义共用的临时缓冲区。
    std::uint8_t* integerTokenBuffer = nullptr; // 原全局：0x0058B5FC。 整数字段及尾随空格的临时缓冲区。
    char** pointerTable58B614 = nullptr; // 原全局：0x0058B614。
    std::uint8_t* reservedTripleBuffer0 = nullptr; // 原全局：0x0058B604。 三倍容量保留缓冲区，当前仅有生命周期操作。
    std::uint8_t* reservedTripleBuffer1 = nullptr; // 原全局：0x0058B608。 三倍容量保留缓冲区，当前仅有生命周期操作。
    std::uint8_t* reservedTripleBuffer2 = nullptr; // 原全局：0x0058B60C。 三倍容量保留缓冲区，当前仅有生命周期操作。
    std::uint8_t* reservedTripleBuffer3 = nullptr; // 原全局：0x0058B610。 三倍容量保留缓冲区，当前仅有生命周期操作。
    SendProtocolObserver sendObserver58B61C = nullptr; // 原全局：0x0058B61C。
    std::wstring sendLogPathDA3B58; // 原全局：0x00DA3B58。
    std::wstring receiveLogPathDA3F9C; // 原全局：0x00DA3F9C。
    std::uint8_t integerFormatScratch[64] {}; // 原 [0xDA3F58,0xDA3F98)。
    std::uint8_t** workspaceSlots = nullptr; // 原 0xDA3F98，18 个同SDK域的字节缓冲指针。
    std::uint8_t active = 0;   // 原 0xD9F2C8，网络运行期活动门禁。
    // 保留原[0xD8F2B8,0xD9F2C0)布局供主线程分发/接收；待发字节由后端独占。
    std::array<std::uint8_t, kQueueStorageSize> queueBytes {}; // 原全局：0x00D8F2B8。
    std::int32_t receiveLengthD69790 = 0; // 原 0xD69790，独立于连续队列。
    std::uint8_t flagsD9F2C0 = 0; // 原全局：0x00D9F2C0。
    std::uint64_t lastTickD9F2C4 = 0; // 原全局：0x00D9F2C4。
    std::int32_t errorD9F2CC = 0; // 原全局：0x00D9F2CC。
    std::uint32_t receiveResetD9F3C4 = 0; // 原全局：0x00D9F3C4。
    std::uint32_t frameModuloD9F3C8 = 0; // 原全局：0x00D9F3C8。
    std::uint8_t value58ADA4 = 0x80; // 原全局：0x0058ADA4。 原ed回复独立字节，不属于58ADA8的排除链文本。
    std::wstring addressTextDA33D8; // 原全局：0x00DA33D8。 lSZ回复的地址文字。
    // N01：稳定节点持有注册头／闭包，回调内追加注册不使当前函数对象失效。
    std::list<std::pair<std::string, ProtocolReceiveHandler>> receiveHandlers;
    bool gameplayHandlersRegistered = false; // G01/G06/G07：业务回调只注册一次，重连保留。
    std::list<std::pair<std::string, ProtocolSendHandler>> sendHandlers;
    CompressedReceiveState compressedReceive; // N02：会话数据随初始化／关闭重置。
    platform::CgTransport transport;
    platform::ConnectionId connectionId = 0;
    bool ioEnabled = false;
    ~Impl() { Shutdown(); }
    void RegisterReceive(const char* header, ProtocolReceiveHandler handler)
    { receiveHandlers.emplace_back(header, std::move(handler)); }
    std::int32_t AllocateProtocolBuffers_516DA0(const std::uint32_t bufferLength);
    std::uint8_t* FormatProtocolInteger_517670(const std::int32_t value);
    std::uint8_t* FormatProtocolIntegerToken_517120(const std::int32_t value);
    std::uint8_t* FormatProtocolTextToken_517160(const std::wstring_view text);
    std::uint8_t* EscapeProtocolText_5171F0(const std::wstring_view text);
    std::uint8_t* UnescapeProtocolText_5172D0(const char* const text);
    std::uint8_t* DecodeOptionalProtocolText_5171B0(const char* const text);
    SendResult SendProtocolBuffer_517480(const platform::ConnectionId expectedId, char* const buffer);
    SendResult SendConnectionProbe_512C10(const platform::ConnectionId expectedId, const std::wstring_view text);

    std::int32_t ConsumeReceiveQueue_508230(const std::int32_t size);

    void SplitReceivedBuffer_5173B0(char* const buffer);
    std::int32_t NetworkDispatch_512E40(const platform::ConnectionId expectedId, const void* const received);
    void HandleServerAddressChangeReply_50E6E0(cg::network::CgConnection& networkConnection, const char* const text);
    std::int32_t HandleVersionUpgradeReply_50E780(cg::network::CgConnection& networkConnection, const std::int32_t value);
    std::int32_t AllocateNetworkWorkspace_516B60(const std::uint32_t bufferLength);
    void ReleaseProtocolWorkspace();
    bool AppendDecodedProtocolBytes(const void* const data, const std::size_t size);
    std::int32_t ReceiveProtocolLine( void* const destination, const std::int32_t capacity);
    void HandleResponse_508800(cg::network::CgConnection& networkConnection, const std::int32_t value, const char* const text);
    platform::ConnectionId BeginConnect(std::wstring host, std::uint16_t port, bool noDelay);
    platform::ConnectionSnapshot PollConnection();
    void Close();
    void StopIo();
    SendResult SendPacket(const platform::ConnectionId expectedId, const void* const data,
        const std::uint32_t size, bool replacePending = false);
    std::int32_t Initialize();
    ConnectionPumpResult Pump();
    void Shutdown();
    template <typename Value>
    Value DecodeDispatchArgument(const std::size_t index, const bool workspaceBeforeCapacity);
    template <typename... Values, std::size_t... Indices>
    std::tuple<Values...> DecodeDispatchArguments(std::index_sequence<Indices...>, const std::uint32_t workspaceBeforeCapacityMask);
    template <auto Callback, typename... Values>
    bool TryDispatchReply(const char* const head, const char* const command, const std::uint32_t workspaceBeforeCapacityMask = 0U);
};

#ifdef TAOMOLI
// 输入须为有效的标准Base64字节，length不含尾NUL；原地解码并返回实际二进制长度。
__declspec(noinline)
static std::size_t DecodeBase64InPlace(char* const target, const std::size_t length)
{
    VM_LIGHT_START
    std::array<std::uint8_t, 256> decodeTable {};
    char alphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (std::uint8_t index = 0; index < 64; ++index) {
        decodeTable[static_cast<unsigned char>(alphabet[index])] = index;
    }
    std::uint32_t bits = 0;
    std::uint32_t bitCount = 0;
    std::size_t decodedSize = 0;
    for (std::size_t index = 0; index < length; ++index) {
        const auto character = static_cast<unsigned char>(target[index]);
        if (character == '=') {
            break;
        }
        bits = (bits << 6) | decodeTable[character];
        bitCount += 6;
        if (bitCount >= 8) {
            bitCount -= 8;
            target[decodedSize++] =
                static_cast<char>((bits >> bitCount) & 0xFFu);
        }
    }
    VM_LIGHT_END
    return decodedSize;
}
#endif

static LZ4_streamDecode_t initDic()
{
    VM_HARD_START
    const char* dictionary = nullptr;
    std::size_t dictionarySize = 0;
#ifdef TAOMOLI
#include "ProtocolReceiveDictionary.hpp"
    static char kProtocolReceiveDictionary[sizeof(kProtocolReceiveDictionary2)];
    memcpy(kProtocolReceiveDictionary, kProtocolReceiveDictionary2, sizeof(kProtocolReceiveDictionary2));
    dictionarySize = DecodeBase64InPlace(
        kProtocolReceiveDictionary, sizeof(kProtocolReceiveDictionary) - 1);
    dictionary = kProtocolReceiveDictionary;
#endif
    LZ4_streamDecode_t initialStream{};
    static_cast<void>(LZ4_setStreamDecode(
        &initialStream, dictionary, static_cast<std::int32_t>(dictionarySize)));
    VM_HARD_END
    return initialStream;
}

// 固定字典只解码并绑定一次；服务端每帧重置压缩流，此处不能沿用上一帧历史。
static const LZ4_streamDecode_t dictionaryStream = initDic();

// N02：一帧内连续解压，之前的输出保持原地址，供后续LZ4块引用。
static std::int32_t DecodeCompressedProtocolFrame(CompressedReceiveState& state)
{
    static_assert(std::is_trivially_copyable_v<LZ4_streamDecode_t>);
    LZ4_streamDecode_t stream = dictionaryStream;
    std::size_t offset = 4;
    std::size_t used = 0;
    while (offset < state.frame.size()) {
        if (offset + 2 > state.frame.size()) {
            return -1;
        }
        const std::int32_t compressedSize =
            (static_cast<std::int32_t>(state.frame[offset]) << 8) | state.frame[offset + 1];
        offset += 2;
        if (compressedSize == 0) {
            break;
        }
        if (compressedSize > LZ4_COMPRESSBOUND(kLz4BlockSize)
            || offset + compressedSize > state.frame.size()) {
            return -1;
        }
        const std::int32_t decodedSize = LZ4_decompress_safe_continue(
            &stream, reinterpret_cast<const char*>(state.frame.data() + offset),
            state.frameOutput.data() + used, compressedSize, kLz4BlockSize);
        offset += compressedSize;
        if (decodedSize <= 0) {
            if (decodedSize < 0) {
                m_log_iw(L"LZ4 DECODE ERROR=%d FRAME=%zu BLOCK=%d OFFSET=%zu\n",
                    decodedSize, state.frame.size(), compressedSize, offset - compressedSize);
            }
            break;
        }
        if (static_cast<std::size_t>(decodedSize) > kLz4FrameDecodedCapacity - used) {
            return -1;
        }
        m_logw(L"[network][recv][lz4] compressed=%d decoded=%d bytes\n",
            compressedSize, decodedSize);
        used += decodedSize;
    }
    return static_cast<std::int32_t>(used);
}

static std::int32_t BuildProtocolCommand_517510(
    char* const buffer,
    const char* const command)
{
    // 与服务端nrproto_CreateHeader一致；明文命令不混淆、不生成密文状态尾。
    static_cast<void>(std::sprintf(buffer, "%s ", command));
    return 0;
}

// 原509080不读取参数；六个调用形态仍先完成各自的全部解码。
template <typename... Values>
// 原函数：0x00509080。
static void IgnoreReplyArguments_509080(cg::network::CgConnection&, Values...)
{
    cg::core::NullSub_509080();
}

template <typename Value>
Value CgConnection::Impl::DecodeDispatchArgument(
    const std::size_t index, const bool workspaceBeforeCapacity)
{
    static_assert(std::is_same_v<Value, std::int32_t> || std::is_same_v<Value, char*>);
    if constexpr (std::is_same_v<Value, std::int32_t>) {
        return ParseProtocolInteger_517190(this->pointerTable58B614[index]);
    } else {
        std::uint8_t* const decoded = DecodeOptionalProtocolText_5171B0(this->pointerTable58B614[index]);
        std::int32_t capacity;
        std::uint8_t** workspace;
        // 原85字段分两种基址/容量读取顺序；槽解引用均在两次读取后。
        if (workspaceBeforeCapacity) {
            workspace = this->workspaceSlots;
            capacity = std::bit_cast<std::int32_t>(this->workspaceCapacity);
        } else {
            capacity = std::bit_cast<std::int32_t>(this->workspaceCapacity);
            workspace = this->workspaceSlots;
        }
        std::uint8_t* const destination = workspace[index];
        return reinterpret_cast<char*>(
            CopyProtocolBytesAndReturn_517690(destination, capacity, decoded));
    }
}

template <typename... Values, std::size_t... Indices>
std::tuple<Values...> CgConnection::Impl::DecodeDispatchArguments(
    std::index_sequence<Indices...>, const std::uint32_t workspaceBeforeCapacityMask)
{
    // bit N对应原第N参数读序；见doc/cg2d_dispatch_account_field_read_spec.md。
    // 花括号初始化保证先完成第N项，再读取第N+1项及其共享临时区。
    return std::tuple<Values...> {
        DecodeDispatchArgument<Values>(
            Indices + 1,
            (workspaceBeforeCapacityMask & (1U << (Indices + 1))) != 0U)...
    };
}

template <auto Callback, typename... Values>
bool CgConnection::Impl::TryDispatchReply(
    const char* const head, const char* const command,
    const std::uint32_t workspaceBeforeCapacityMask)
{
    // 原108块均先取头U8再取命令U8；每对比较后以同头样本判NUL。
    auto* receivedBytes = reinterpret_cast<const unsigned char*>(head);
    auto* commandBytes = reinterpret_cast<const unsigned char*>(command);
    for (;;) {
        const unsigned char receivedFirst = receivedBytes[0];
        const unsigned char commandFirst = commandBytes[0];
        if (receivedFirst != commandFirst) {
            return false;
        }
        if (receivedFirst == 0) {
            break;
        }
        const unsigned char receivedSecond = receivedBytes[1];
        const unsigned char commandSecond = commandBytes[1];
        if (receivedSecond != commandSecond) {
            return false;
        }
        receivedBytes += 2;
        commandBytes += 2;
        if (receivedSecond == 0) {
            break;
        }
    }
    const auto values = DecodeDispatchArguments<Values...>(
        std::index_sequence_for<Values...> {}, workspaceBeforeCapacityMask);
    std::apply([this](const auto... arguments) {
        if constexpr (std::is_member_function_pointer_v<decltype(Callback)>)
            static_cast<void>((this->*Callback)(owner, arguments...));
        else
            static_cast<void>(Callback(owner, arguments...));
    }, values);
    return true;
}

std::int32_t CgConnection::Impl::AllocateProtocolBuffers_516DA0(
    const std::uint32_t bufferLength)
{
    this->workspaceCapacity = bufferLength;

    // NTM01：仅在旧工作区已释放后调用；分配失败由唯一释放路径回收。
    this->sendMessageBuffer = nullptr;
    this->reservedProtocolBuffer = nullptr;
    this->textTokenBuffer = nullptr;
    this->integerTokenBuffer = nullptr;
    this->pointerTable58B614 = nullptr;
    this->reservedTripleBuffer0 = nullptr;
    this->reservedTripleBuffer1 = nullptr;
    this->reservedTripleBuffer2 = nullptr;
    this->reservedTripleBuffer3 = nullptr;

    this->sendMessageBuffer = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(bufferLength) + 2));
    std::uint8_t* const second = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(this->workspaceCapacity)));
    // 原16DF9/16E0A：先取下一请求长度，再发布本次分配结果。
    const std::uint32_t thirdLength = this->workspaceCapacity;
    this->reservedProtocolBuffer = second;
    std::uint8_t* const third = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(thirdLength)));
    const std::uint32_t fourthLength = this->workspaceCapacity;
    this->textTokenBuffer = third;
    this->integerTokenBuffer = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(fourthLength)));
    this->pointerTable58B614 = static_cast<char**>(
        std::malloc(
            static_cast<std::size_t>(this->workspaceCapacity) * sizeof(char*)));
    this->reservedTripleBuffer0 = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(this->workspaceCapacity * 3U)));
    this->reservedTripleBuffer1 = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(this->workspaceCapacity * 3U)));
    this->reservedTripleBuffer2 = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(this->workspaceCapacity * 3U)));
    std::uint8_t* const fourthTriple = static_cast<std::uint8_t*>(
        std::malloc(static_cast<std::size_t>(this->workspaceCapacity * 3U)));

    // 原16E7E/16E84：末结果发布前取首清零长度和指针；零计数不执行REP写入。
    std::uint32_t clearLength = this->workspaceCapacity;
    std::uint8_t* clearBuffer = this->sendMessageBuffer;
    this->reservedTripleBuffer3 = fourthTriple;
    if (!this->sendMessageBuffer || !this->reservedProtocolBuffer
        || !this->textTokenBuffer || !this->integerTokenBuffer
        || !this->pointerTable58B614 || !this->reservedTripleBuffer0
        || !this->reservedTripleBuffer1 || !this->reservedTripleBuffer2
        || !this->reservedTripleBuffer3) {
        ReleaseProtocolWorkspace();
        return -1;
    }
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = this->workspaceCapacity;
    clearBuffer = this->reservedProtocolBuffer;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = this->workspaceCapacity;
    clearBuffer = this->textTokenBuffer;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = this->workspaceCapacity;
    clearBuffer = this->integerTokenBuffer;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = this->workspaceCapacity;
    char** const clearTable = this->pointerTable58B614;
    if (clearLength != 0U) {
        std::fill_n(clearTable, static_cast<std::size_t>(clearLength), nullptr);
    }
    clearLength = this->workspaceCapacity * 3U;
    clearBuffer = this->reservedTripleBuffer0;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = this->workspaceCapacity * 3U;
    clearBuffer = this->reservedTripleBuffer1;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = this->workspaceCapacity * 3U;
    clearBuffer = this->reservedTripleBuffer2;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }
    clearLength = this->workspaceCapacity * 3U;
    clearBuffer = this->reservedTripleBuffer3;
    if (clearLength != 0U) {
        std::memset(clearBuffer, 0, static_cast<std::size_t>(clearLength));
    }

    return 0;
}

std::uint8_t* CgConnection::Impl::FormatProtocolInteger_517670(const std::int32_t value)
{
    static_cast<void>(
        detail::EncodeProtocolBase62_517730(value, this->integerFormatScratch, 64));
    return this->integerFormatScratch;
}

std::uint8_t* CgConnection::Impl::FormatProtocolIntegerToken_517120(const std::int32_t value)
{
    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        this->workspaceCapacity);
    std::uint8_t* const formatted = FormatProtocolInteger_517670(value);
    std::uint8_t* const firstDestination =
        this->integerTokenBuffer;
    CopyProtocolBytes_517080(firstDestination, formatted, firstCapacity);

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        this->workspaceCapacity);
    std::uint8_t* const secondDestination =
        this->integerTokenBuffer;
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(secondDestination),
        " ",
        secondCapacity));

    return this->integerTokenBuffer;
}

std::uint8_t* CgConnection::Impl::FormatProtocolTextToken_517160(const std::wstring_view text)
{
    std::uint8_t* const escaped = EscapeProtocolText_5171F0(text);
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        this->workspaceCapacity);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(escaped), " ", capacity));
    return escaped;
}

std::uint8_t* CgConnection::Impl::EscapeProtocolText_5171F0(const std::wstring_view text)
{
    // UWF01：正文直接以UTF-16转义，仅在写入原协议缓冲时编码GBK。
    const std::wstring_view contents = text;
    std::wstring escaped;
    for (std::size_t offset = 0; offset < contents.size();) {
        const std::size_t next = cg::text::Next(contents, offset);
        const wchar_t unit = contents[offset];
        if (unit == L'\\' || unit == L' ') { escaped += L'\\'; escaped += unit == L' ' ? L'S' : L'\\'; }
        else if (unit == L'\n' || unit == L'\r') { escaped += L'\\'; escaped += unit == L'\n' ? L'n' : L'r'; }
        else { escaped.append(contents, offset, next - offset); }
        offset = next;
    }
    const std::string stored = cg::text::ToGbk(escaped);
    // 末尾空格与NUL由517160追加；提交前检查，不在协议边界静默截字。
    if (stored.size() + 1 >= this->workspaceCapacity) {
        throw cg::text::GbkStorageError(ERROR_INSUFFICIENT_BUFFER, std::system_category(), "文字超过GBK协议字段容量");
    }
    std::memcpy(this->textTokenBuffer, stored.c_str(), stored.size() + 1);
    return this->textTokenBuffer;
}

std::uint8_t* CgConnection::Impl::UnescapeProtocolText_5172D0(const char* const text)
{
    this->textTokenBuffer[0] = 0;
    if (*text == '\0') {
        this->textTokenBuffer[0] = 0;
        return this->textTokenBuffer;
    }

    const auto* cursor = reinterpret_cast<const unsigned char*>(text);
    std::uint32_t destinationIndex = 0;
    for (;;) {
        const unsigned char currentByte = *cursor;
        if (currentByte >= 0x80U) {
            std::uint8_t* const firstDestination =
                this->textTokenBuffer;
            ++destinationIndex;
            ++cursor;
            firstDestination[destinationIndex - 1U] = currentByte;
            const unsigned char secondByte = *cursor;
            this->textTokenBuffer[destinationIndex] = secondByte;
            ++destinationIndex;
        }
        else if (currentByte == static_cast<unsigned char>('\\')) {
            const unsigned char escapedByte = cursor[1];
            if (escapedByte == 0U) {
                this->textTokenBuffer[destinationIndex] = '\\';
                ++destinationIndex;
            }
            else if (escapedByte == static_cast<unsigned char>('S')) {
                std::uint8_t* const destination =
                    this->textTokenBuffer;
                ++destinationIndex;
                ++cursor;
                destination[destinationIndex - 1U] = ' ';
            }
            else if (escapedByte == static_cast<unsigned char>('n')) {
                std::uint8_t* const destination =
                    this->textTokenBuffer;
                ++destinationIndex;
                ++cursor;
                destination[destinationIndex - 1U] = '\n';
            }
            else if (escapedByte == static_cast<unsigned char>('r')) {
                std::uint8_t* const destination =
                    this->textTokenBuffer;
                ++destinationIndex;
                ++cursor;
                destination[destinationIndex - 1U] = '\r';
            }
            else {
                this->textTokenBuffer[destinationIndex] = '\\';
                ++destinationIndex;
                ++cursor;
            }
        }
        else {
            this->textTokenBuffer[destinationIndex] = currentByte;
            ++destinationIndex;
        }

        const unsigned char nextByte = cursor[1];
        ++cursor;
        if (nextByte == 0U) {
            this->textTokenBuffer[destinationIndex] = 0;
            return this->textTokenBuffer;
        }
    }
}

std::uint8_t* CgConnection::Impl::DecodeOptionalProtocolText_5171B0(const char* const text)
{
    if (text != nullptr) {
        return UnescapeProtocolText_5172D0(text);
    }
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        this->workspaceCapacity);
    std::uint8_t* const destination = this->textTokenBuffer;
    CopyProtocolBytes_517080(
        destination,
        reinterpret_cast<const std::uint8_t*>(""),
        capacity);
    return this->textTokenBuffer;
}

SendResult CgConnection::Impl::SendProtocolBuffer_517480(
    const platform::ConnectionId expectedId,
    char* const buffer)
{
    // UTL02：5171F0已将各文本token编码为GBK；日志、观察者和回调共用原协议字节。
    if (this->sendLogPathDA3B58[0] != '\0') {
        std::FILE* const file = _wfopen(
            this->sendLogPathDA3B58.c_str(),
            L"a+");
        if (file != nullptr) {
            static_cast<void>(std::fprintf(file, "->%s\n", buffer));
            static_cast<void>(std::fclose(file));
        }
    }

    const SendProtocolObserver observer =
        this->sendObserver58B61C;
    if (observer != nullptr) {
        observer(1U, buffer);
        if (expectedId != connectionId || !this->active) return SendResult::Unavailable;
    }

    std::uint32_t length =
        static_cast<std::uint32_t>(std::strlen(buffer));
    const std::uint32_t bufferLength =
        this->workspaceCapacity;
    const std::uint32_t threshold = bufferLength * 3U;
    if (length < threshold) {
        buffer[length] = '\n';
        buffer[length + 1U] = '\0';
        ++length;
    }

    return SendPacket(expectedId, buffer, length);
}

SendResult CgConnection::Impl::SendConnectionProbe_512C10(
    const platform::ConnectionId expectedId,
    const std::wstring_view text)
{
    const auto session = owner.CaptureSession();
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(this->sendMessageBuffer), "Or"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        this->workspaceCapacity);
    std::uint8_t* const token = FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(this->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));
    return SendProtocolBuffer_517480(
        expectedId,
        reinterpret_cast<char*>(this->sendMessageBuffer));
}

std::int32_t CgConnection::Impl::ConsumeReceiveQueue_508230(const std::int32_t size)
{
    if (!ioEnabled) {
        return -100;
    }
    if (size > static_cast<std::int32_t>(kQueueCapacity)) {
        return -1;
    }

    const std::int32_t remaining =
        this->receiveLengthD69790 - size;
    this->receiveLengthD69790 = remaining;
    std::uint8_t* const receive =
        this->queueBytes.data() + kReceiveQueueOffset;
    std::memmove(receive, receive + size, static_cast<std::uint32_t>(remaining));
    return 0;
}

void CgConnection::Impl::SplitReceivedBuffer_5173B0(char* const buffer)
{
    const auto connection = connectionId;
    const SendProtocolObserver observer = this->sendObserver58B61C;
    if (observer != nullptr) {
        observer(0U, buffer);
        if (connection != connectionId || !this->active) return;
    }
    if (this->receiveLogPathDA3F9C.c_str()[0] != '\0') {
        std::FILE* const file = _wfopen(
            this->receiveLogPathDA3F9C.c_str(), L"a+");
        if (file != nullptr) {
            static_cast<void>(std::fprintf(file, "%s\n", buffer));
            static_cast<void>(std::fclose(file));
        }
    }
    std::int32_t count = 0;
    if (buffer[0] != '\0') {
        std::int32_t index = 0;
        do {
            if (index == 0) {
                char** const table = this->pointerTable58B614;
                ++count;
                table[count - 1] = buffer;
            }
            if (buffer[index] == ' ') {
                this->pointerTable58B614[count] = buffer + index + 1;
                ++count;
            }
            ++index;
        } while (buffer[index] != '\0');
    }
    char* cursor = buffer;
    if (*cursor != '\0') {
        do {
            if (*cursor == ' ' || *cursor == '\n' || *cursor == '\r') {
                *cursor = '\0';
            }
            ++cursor;
        } while (*cursor != '\0');
    }
    this->pointerTable58B614[count] = nullptr;
}

std::int32_t CgConnection::Impl::NetworkDispatch_512E40(
    const platform::ConnectionId expectedId, const void* const received)
{
    const auto session = owner.CaptureSession();
    using namespace detail; // 原业务handler仍由各模块持有。
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        this->workspaceCapacity);
    CopyProtocolBytes_517080(
        this->sendMessageBuffer,
        static_cast<const std::uint8_t*>(received), capacity);
    const auto receivedSize = std::strlen(
        reinterpret_cast<const char*>(this->sendMessageBuffer)) + 1;
    SplitReceivedBuffer_5173B0(
        reinterpret_cast<char*>(this->sendMessageBuffer));
    if (expectedId != connectionId || !this->active) return 0;
    const std::vector<std::uint8_t> receivedPacket(this->sendMessageBuffer,
        this->sendMessageBuffer + receivedSize);
    std::vector<char*> receivedArguments;
    for (auto** argument = this->pointerTable58B614; *argument; ++argument) {
        receivedArguments.push_back(*argument);
    }
    receivedArguments.push_back(nullptr);
    std::uint32_t headerValue;
    char head[1024];
    ReadProtocolCommandHeader_517040(&headerValue, reinterpret_cast<std::uint8_t*>(head), sizeof(head),
        this->pointerTable58B614);
    // 服务端明文头直接是原命令（如whV），不再执行原密文命令逆变换。
    for (auto& [header, handler] : this->receiveHandlers) {
        if (header == head) {
            handler(owner, head, this->pointerTable58B614);
            if (expectedId != connectionId || !ioEnabled) return 0;
            break;
        }
    }

    // Lua回调可能发送报文并覆写共享缓冲；恢复已分词快照，避免重复调用收包观察器。
    const auto restoreArguments = [&] {
        std::memcpy(this->sendMessageBuffer, receivedPacket.data(), receivedSize);
        std::copy(receivedArguments.begin(), receivedArguments.end(),
            this->pointerTable58B614);
    };
    restoreArguments();
    script::LuaReceivePacket(owner, head, this->pointerTable58B614);
    if (expectedId != connectionId || !ioEnabled) return 0;
    restoreArguments();

    // I为完整S32解析，T为同编号工作区char*；顺序与原108条比较路径一致。
    using I = std::int32_t;
    using T = char*;
    if (TryDispatchReply<world::HandlePlayerPositionReply_5093C0, I, I, I>(head, "QBt")) return 0;
    if (TryDispatchReply<world::HandleMapRectangleCrcReply_509090, I, I, I, I, I, I, I, I, I>(head, "jmb")) return 0;
    if (TryDispatchReply<world::HandleMapRectangleDataReply_509100, I, I, I, I, I, I, T>(head, "aBS")) return 0;
    if (TryDispatchReply<world::HandleMapTransitionReply_50CF90, I, I>(head, "EZ")) return 0;
    if (TryDispatchReply<world::Sub_50D080, I, I>(head, "OaHi")) return 0;
    if (TryDispatchReply<world::HandleBattleEntryReply_50D120, I, I>(head, "dAgg")) return 0;
    if (TryDispatchReply<world::HandleBattleResultsReply_50D500, T>(head, "Gn")) return 0;
    if (TryDispatchReply<HandleDuelPointResultReply_50D700, T>(head, "hB")) return 0;
    if (TryDispatchReply<world::HandleBattleStreamReply_50D3C0, T>(head, "LYIm")) return 0;
    if (TryDispatchReply<world::HandleFullPlayerInventoryReply_50BFD0, T>(head, "ELoy")) return 0;
    if (TryDispatchReply<world::HandlePlayerInventoryUpdatesReply_50C260, T>(head, "rU")) return 0;
    if (TryDispatchReply<world::HandlePlayerWorldTimeReply_50C870, I, I, I, I>(head, "PudO")) return 0;
    if (TryDispatchReply<world::HandleInventorySlotSwapReply_50C900, I, I>(head, "vWpn")) return 0;
    if (TryDispatchReply<world::HandleSkillRecipesReply_50C580, I, T>(head, "xmgQ", 0x4U)) return 0;
    if (TryDispatchReply<world::HandlePlayerTimerReply_50C7A0, I, I>(head, "MTg")) return 0;
    if (TryDispatchReply<world::detail::HandleContactMailReply_50DC70, I, T, I>(head, "Mhbt", 0x4U)) return 0;
    if (TryDispatchReply<world::HandleAlbumEntryMaskReply_50E5B0, I, T>(head, "Ilz", 0x4U)) return 0;
    if (TryDispatchReply<world::HandleAlbumEntryReply_50E5D0, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, T, T>(head, "dM", 0x30000U)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<I, T>, I, T>(head, "Dn", 0x4U)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<I>, I>(head, "IYo")) return 0;
    if (TryDispatchReply<HandleMapActorTransitionReply_50DD80, I, I, I, I, I, I, I, T>(head, "lDuJ", 0x100U)) return 0;
    if (TryDispatchReply<world::detail::HandleContactListReply_50D740, T>(head, "gnxd")) return 0;
    if (TryDispatchReply<world::detail::HandleContactExtendedTextReply_50D8C0, T>(head, "PhdQ")) return 0;
    if (TryDispatchReply<world::detail::HandleContactUpdateReply_50D9E0, I, T>(head, "vI", 0x4U)) return 0;
    if (TryDispatchReply<HandleGuildMailResetReply_50E830>(head, "GC")) return 0;
    if (TryDispatchReply<cg::world::HandleGuildInfoReply_50E850, T>(head, "aDn")) return 0;
    if (TryDispatchReply<cg::world::HandleGuildRankDefinitionsReply_50EB40, T>(head, "znv")) return 0;
    if (TryDispatchReply<cg::world::UpdateOrderedRecordBatch_50EC50, T>(head, "yuD")) return 0;
    if (TryDispatchReply<cg::world::UpdateOrderedRecords_50EEB0, I, T>(head, "Kka", 0x4U)) return 0;
    if (TryDispatchReply<HandleGuildMemberRemovalReply_50F130, I>(head, "oRut")) return 0;
    if (TryDispatchReply<HandleGuildMailReply_50F190, I, T, I>(head, "Rll", 0x4U)) return 0;
    if (TryDispatchReply<cg::world::HandleGuildRemovalReply_50F280, I>(head, "frQp")) return 0;
    if (TryDispatchReply<world::detail::HandleProfileCategoryVersionsReply_50F3A0, T>(head, "HNP")) return 0;
    if (TryDispatchReply<world::detail::HandlePrimaryProfileCategoriesReply_50F2A0, T>(head, "eZT")) return 0;
    if (TryDispatchReply<world::detail::HandleSecondaryProfileCategoriesReply_50F8E0, T>(head, "IUn")) return 0;
    if (TryDispatchReply<world::detail::HandlePlayerProfileReply_50F450, T, I, T, T, T, I, I, T, I, T, I, T, T>(head, "DpSY", 0x1128U)) return 0;
    if (TryDispatchReply<world::detail::HandleMiniMailContactRemovalReply_50F430, T>(head, "cNB")) return 0;
    if (TryDispatchReply<world::detail::HandleMiniMailReply_50F7F0, I, T, I>(head, "ow", 0x4U)) return 0;
    if (TryDispatchReply<ui::WindowProfile_ReceiveReply, I, I, I, I, T>(head, "DyAG", 0x20U)) return 0;
    if (TryDispatchReply<world::detail::HandleMiniMailConversationReply_50F9E0, T>(head, "KyN")) return 0;
    if (TryDispatchReply<world::HandlePlayerChatReply_50CAE0, I, T, I, I>(head, "zdr", 0x4U)) return 0;
    if (TryDispatchReply<world::HandleSystemChatReply_50CE10, T>(head, "Zd")) return 0;
    if (TryDispatchReply<world::HandlePlayerPrimaryStatsReply_509650, T>(head, "aPc")) return 0;
    if (TryDispatchReply<world::HandlePlayerSecondaryStatsReply_509BA0, T>(head, "UU")) return 0;
    if (TryDispatchReply<world::HandlePetPrimaryStatsReply_50A6B0, I, T>(head, "IsSy", 0x4U)) return 0;
    if (TryDispatchReply<world::HandlePetSecondaryStatsReply_50AC20, I, T>(head, "AU", 0x4U)) return 0;
    if (TryDispatchReply<world::HandlePartyActorVitalsReply_50AEF0, T>(head, "TNT")) return 0;
    if (TryDispatchReply<world::HandleMapObjectListReply_50B380, T>(head, "nsY")) return 0;
    if (TryDispatchReply<world::HandlePartyMemberReply_50A3E0, I, T>(head, "JJQj", 0x4U)) return 0;
    if (TryDispatchReply<world::HandlePlayerTitlesReply_50A300, T>(head, "Sc")) return 0;
    if (TryDispatchReply<world::HandleWorldActorActionsReply_50BCD0, T>(head, "NI")) return 0;
    if (TryDispatchReply<world::HandleWorldObjectRemovalReply_50BF20, T>(head, "uSB")) return 0;
    if (TryDispatchReply<world::HandlePlayerProfessionReply_509E30, I, T>(head, "jv", 0x4U)) return 0;
    if (TryDispatchReply<world::HandlePlayerSkillsReply_509EB0, T>(head, "cNE")) return 0;
    if (TryDispatchReply<world::HandlePlayerSkillTechniquesReply_50A120, I, T>(head, "Rspn", 0x4U)) return 0;
    if (TryDispatchReply<world::HandlePetTechniquesReply_50B0D0, I, T>(head, "OBee", 0x4U)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<T>, T>(head, "Yo")) return 0;
    if (TryDispatchReply<world::HandlePlayerOptionsReply_50BFA0, I>(head, "SkBD")) return 0;
    if (TryDispatchReply<Sub_50D3B0, I>(head, "sZWu")) return 0;
    if (TryDispatchReply<world::HandlePartyMembershipReply_50D000, I, I>(head, "bNY")) return 0;
    if (TryDispatchReply<ui::WindowSkill_ReceiveTextTargetReply, T>(head, "ysVc")) return 0;
    if (TryDispatchReply<ui::WindowSkill_ReceiveAttributeTargetReply, T>(head, "ND")) return 0;
    if (TryDispatchReply<ui::WindowItemUse_ReceiveTextTargetReply, T>(head, "LY")) return 0;
    if (TryDispatchReply<ui::WindowItemUse_ReceiveAttributeTargetReply, T>(head, "nFV")) return 0;
    if (TryDispatchReply<ui::WindowWorkResult_ReceiveReply, I, I, T>(head, "JgO", 0x8U)) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveTargetReply, T>(head, "Cxo")) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveStartReply, T, I>(head, "ms")) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveCancelReply>(head, "NdM")) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveItemReply, T>(head, "nUbJ")) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveStateReply, I>(head, "hJq")) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceivePetReply, I, T>(head, "RwK", 0x4U)) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceivePetSkillReply, I, T>(head, "Yq", 0x4U)) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveRemoteResetReply>(head, "xEg")) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveCloseReply, I>(head, "We")) return 0;
    if (TryDispatchReply<HandleFailureNoticeReply_50CF20, I, I, I, I>(head, "BHe")) return 0;
    if (TryDispatchReply<HandleRemainingAttributePointsReply_50CF60, I>(head, "kg")) return 0;
    if (TryDispatchReply<world::HandlePetAttributePointsReply_50CF70, I, I>(head, "sA")) return 0;
    if (TryDispatchReply<world::HandlePlayerBattleRowReply_50BFC0, I>(head, "Ov")) return 0;
    if (TryDispatchReply<ui::WindowDialog_ReceiveReply, I, I, I, I, T>(head, "BZ", 0x20U)) return 0;
    if (TryDispatchReply<world::HandleDirectionalMapEffectsReply_50E1A0, I, I, T>(head, "zzd", 0x8U)) return 0;
    if (TryDispatchReply<world::HandleNotificationSoundReply_50E210, I>(head, "TFCu")) return 0;
    if (TryDispatchReply<HandleMapBgmChangeReply_50E310, I>(head, "wiB")) return 0;
    if (TryDispatchReply<HandlePaletteTransitionReply_50E630, I, I>(head, "bJg")) return 0;
    if (TryDispatchReply<world::HandleMapSceneEffectReply_50E650, I, I, I, I, I>(head, "PVfz")) return 0;
    if (TryDispatchReply<HandleServerSoundPlayback_50E680, I, I, I>(head, "murD")) return 0;
    if (TryDispatchReply<HandleSpatialSoundSourceReply_50E6C0, I, I, I>(head, "ftaN")) return 0;
    if (TryDispatchReply<world::HandleMapNameReply_509550, T>(head, "TeD")) return 0;
    if (TryDispatchReply<world::HandleMapChangeReply_5093F0, I, I, I, I, I, I, I, I, I, I, I, I>(head, "GmNM")) return 0;
    if (TryDispatchReply<&Impl::HandleResponse_508800, I, T>(head, "whV", 0x4U)) return 0;
    if (TryDispatchReply<login::HandleCharacterCreationResponse_508EE0, T, I, T>(head, "DZ", 0x8U)) return 0;
    if (TryDispatchReply<login::HandleCharacterDeletionResponse_509020, T, T>(head, "oy", 0x4U)) return 0;
    if (TryDispatchReply<login::HandleLoginResponse_508BC0, T, T>(head, "hPUd", 0x4U)) return 0;
    if (TryDispatchReply<login::HandleCharacterListResponse_5089B0, I, T>(head, "Daj", 0x4U)) return 0;
    if (TryDispatchReply<login::detail::HandleLogoutReply_508CF0, T, T>(head, "HMNy", 0x4U)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<T>, T>(head, "cJzI")) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<I, I>, I, I>(head, "Bi")) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<T>, T>(head, "LOh")) return 0;
    if (TryDispatchReply<&Impl::HandleServerAddressChangeReply_50E6E0, T>(head, "lSZ")) return 0;
    if (TryDispatchReply<character_select::detail::HandlePukVersionReply_50E770, I>(head, "PfM")) return 0;
    if (TryDispatchReply<&Impl::HandleVersionUpgradeReply_50E780, I>(head, "ed")) return 0;
    // COR10：追踪器已移除，旧tq配置仍按两个整数解码后忽略。
    if (TryDispatchReply<IgnoreReplyArguments_509080<I, I>, I, I>(head, "tq")) return 0;
    if (TryDispatchReply<ui::WindowStall_ReceiveOpenReply, I>(head, "Tmrx")) return 0;
    if (TryDispatchReply<ui::WindowStall_ReceiveCloseReply, I>(head, "KXhH")) return 0;
    if (TryDispatchReply<ui::WindowStall_ReceiveBrowseReply, I, I, T, T, T, T>(head, "ft", 0x68U)) return 0;
    if (TryDispatchReply<ui::WindowStall_ReceivePurchaseReply, I>(head, "Yx")) return 0;
    static_cast<void>(TryDispatchReply<world::HandlePetInventoryUpdatesReply_510150, I, T>(head, "iKe", 0x4U));
    static_cast<void>(TryDispatchReply<HandleInventorySlotSwapReply_510460, I, I, I, I>(head, "kiG"));
    static_cast<void>(TryDispatchReply<HandleItemDetailReply_510490, T>(head, "TgX"));
    return -1;
}

void CgConnection::Impl::HandleServerAddressChangeReply_50E6E0(cg::network::CgConnection&, const char* const text)
{
    auto& previous = this->addressTextDA33D8;
    const auto address = cg::text::FromGbkWide(text);
    if (!previous.empty()) {
        if (previous.find(address) != std::wstring::npos) {
            return;
        }
        std::wstring message;
        cg::text::FormatTo(message, L"IP 位址由[{}]变成[{}]。" /* 原常量参考：0x0058B0B0。 */, previous, address);
        world::AppendChatMessage_44B050(message, 4, 3);
        world::AppendChatMessage_44B050(L"网路有可能是被再连接了。" /* 原常量参考：0x0058B094。 */, 4, 3);
    }
    previous = address;
}

std::int32_t CgConnection::Impl::HandleVersionUpgradeReply_50E780(cg::network::CgConnection&, const std::int32_t value)
{
    const std::int32_t sceneType = world::gCurrentSceneType;
    const auto lowByte = static_cast<std::uint8_t>(value);
    this->value58ADA4 = lowByte;
    if (sceneType != 13) {
        return value;
    }
    const std::int32_t state = lowByte != 0 ? 200 : 100;
    world::gCurrentSceneState = state;
    return state;
}

std::int32_t CgConnection::Impl::AllocateNetworkWorkspace_516B60(
    const std::uint32_t bufferLength)
{
    ReleaseProtocolWorkspace();
    if (AllocateProtocolBuffers_516DA0(bufferLength) != 0) return -1;

    this->workspaceSlots = static_cast<std::uint8_t**>(
        std::malloc(kWorkspaceSlotCount * sizeof(std::uint8_t*)));
    if (this->workspaceSlots == nullptr) {
        ReleaseProtocolWorkspace();
        return -1;
    }

    std::fill_n(
        this->workspaceSlots,
        kWorkspaceSlotCount,
        nullptr);
    for (std::size_t index = 0; index < kWorkspaceSlotCount; ++index) {
        this->workspaceSlots[index] = static_cast<std::uint8_t*>(
            std::malloc(static_cast<std::size_t>(bufferLength)));
        if (this->workspaceSlots[index] == nullptr) {
            ReleaseProtocolWorkspace();
            return -1;
        }
    }
    return 0;
}

// NTM01/CTO01：初始化失败与重复关闭共用释放路径，清空所有拥有指针。
void CgConnection::Impl::ReleaseProtocolWorkspace()
{
    std::free(std::exchange(this->sendMessageBuffer, nullptr));
    std::free(std::exchange(this->reservedProtocolBuffer, nullptr));
    std::free(std::exchange(this->textTokenBuffer, nullptr));
    std::free(std::exchange(this->integerTokenBuffer, nullptr));
    std::free(std::exchange(this->pointerTable58B614, nullptr));
    std::free(std::exchange(this->reservedTripleBuffer0, nullptr));
    std::free(std::exchange(this->reservedTripleBuffer1, nullptr));
    std::free(std::exchange(this->reservedTripleBuffer2, nullptr));
    std::free(std::exchange(this->reservedTripleBuffer3, nullptr));
    if (this->workspaceSlots) {
        for (std::size_t index = 0; index < kWorkspaceSlotCount; ++index)
            std::free(this->workspaceSlots[index]);
        std::free(std::exchange(this->workspaceSlots, nullptr));
    }
    this->workspaceCapacity = 0;
}

// 参考buffEx容量语义：不足时清旧正文，当前输入保留待下次重试。
bool CgConnection::Impl::AppendDecodedProtocolBytes(const void* const data, const std::size_t size)
{
    CompressedReceiveState& state = this->compressedReceive;
    if (size >= state.lines.size() - state.lineBytes) {
        state.lineBytes = 0;
        state.lines[0] = '\0';
        return false;
    }
    std::memcpy(state.lines.data() + state.lineBytes, data, size);
    state.lineBytes += size;
    state.lines[state.lineBytes] = '\0';
    return true;
}

// N02：普通LF行与0xEF帧共用累积正文；半帧/半行保留到后续接收。
std::int32_t CgConnection::Impl::ReceiveProtocolLine(
    void* const destination,
    const std::int32_t capacity)
{
    if (!ioEnabled) {
        return -100;
    }
    CompressedReceiveState& state = this->compressedReceive;
    std::uint8_t* const receive =
        this->queueBytes.data() + kReceiveQueueOffset;
    for (;;) {
        const auto* const newline = static_cast<const char*>(
            std::memchr(state.lines.data(), '\n', state.lineBytes));
        if (newline != nullptr) {
            const std::size_t lineLength = static_cast<std::size_t>(newline - state.lines.data()) + 1;
            if (capacity <= static_cast<std::int32_t>(lineLength)) {
                ::OutputDebugStringW(L"\nBuffer Too Small\n");
                return -1;
            }
            std::memcpy(destination, state.lines.data(), lineLength);
            static_cast<char*>(destination)[lineLength] = '\0';
            // 普通行和解压后的行共用此处；在分词／回调改写缓冲前打印完整封包。
            m_logw(L"[network][recv][%zu] %.*hs", lineLength,
                static_cast<int>(lineLength), static_cast<const char*>(destination));
            state.lineBytes -= lineLength;
            std::memmove(state.lines.data(), state.lines.data() + lineLength, state.lineBytes);
            state.lines[state.lineBytes] = '\0';
            return 0;
        }

        const auto available = static_cast<std::size_t>(this->receiveLengthD69790);
        if (!state.frame.empty()) {
            const std::size_t count = std::min<std::size_t>(available, state.frame.size() - state.frameBytes);
            std::memcpy(state.frame.data() + state.frameBytes, receive, count);
            state.frameBytes += count;
            static_cast<void>(ConsumeReceiveQueue_508230(static_cast<std::int32_t>(count)));
            if (state.frameBytes < state.frame.size()) {
                return -1;
            }
            const std::int32_t decodedSize = DecodeCompressedProtocolFrame(state);
            if (decodedSize < 0
                || !AppendDecodedProtocolBytes(state.frameOutput.data(),
                    static_cast<std::size_t>(decodedSize))) {
                return -1;
            }
            state.frame.clear();
            state.frameBytes = 0;
            continue;
        }
        if (available == 0) {
            return -1;
        }
        if (receive[0] == kLz4FrameMarker) {
            if (available < 4) {
                return -1;
            }
            const std::uint32_t frameLength =
                (static_cast<std::uint32_t>(receive[1]) << 16)
                | (static_cast<std::uint32_t>(receive[2]) << 8)
                | receive[3];
            if (frameLength < 4) {
                return -1;
            }
            state.frame.resize(frameLength);
            state.frameBytes = 0;
            continue;
        }

        const auto* const plainNewline = static_cast<const std::uint8_t*>(
            std::memchr(receive, '\n', available));
        const std::size_t count = plainNewline == nullptr ? available
            : static_cast<std::size_t>(plainNewline - receive) + 1;
        if (!AppendDecodedProtocolBytes(receive, count)) {
            return -1;
        }
        static_cast<void>(ConsumeReceiveQueue_508230(static_cast<std::int32_t>(count)));
    }
}

// 原0x00508800：登录回复更新当前连接协议状态，原业务字段保持。
void CgConnection::Impl::HandleResponse_508800(cg::network::CgConnection&,
    const std::int32_t value,
    const char* const text)
{
    if (login::gRequestStatus != 1) {
        static_cast<void>(login::detail::AppendSystemLog_44B5C0(L"LOGIN REPLY IGNORED"));
        return;
    }

    login::gRequestStatus = 2;
    login::gLoginReplyErrorCode_DA3B24 = std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(value));
    const auto diagnostic = std::format(L"LOGIN REPLY RESULT={}", value);
    static_cast<void>(login::detail::AppendSystemLog_44B5C0(diagnostic));
    if (value != 0) {
        if (std::strcmp(text, "busy") == 0) {
            login::gLoginReplyErrorCode_DA3B24 = 1234;
        }
        return;
    }

    this->protocolModeDA3B22 = 1;
    this->time62B394 = ::_time64(nullptr);
    this->localTime62B380 =
        ::_localtime64(&this->time62B394);

    const char* const separator = const_cast<char*>(std::strstr(text, "|"));
    character_select::detail::gText9A4EAC.clear();
    if (separator != nullptr) {
        character_select::detail::gText9A4EAC = cg::text::FromGbkWide(
            std::string_view(text, static_cast<std::uint32_t>(separator - text)));
        // 原0x00404600的防护规则解析已移除（PRM01），仅保留分隔符前的登录正文。
        return;
    }

    character_select::detail::gText9A4EAC = cg::text::FromGbkWide(text);
}

platform::ConnectionId CgConnection::Impl::BeginConnect(std::wstring host, std::uint16_t port, bool noDelay)
{
    Close();
    try {
        auto policyQuery = platform::PrepareServerAddressPolicy(host, port);
        this->frameModuloD9F3C8 = 0;
        this->flagsD9F2C0 = 0;
        this->errorD9F2CC = 0;
        const auto id = transport.BeginConnect(std::move(host), port, noDelay, std::move(policyQuery));
        connectionId = id;
        return connectionId;
    } catch (const std::exception&) {
        return 0;
    }
}

platform::ConnectionSnapshot CgConnection::Impl::PollConnection()
{
    return transport.PollConnection(connectionId);
}

void CgConnection::Impl::Close()
{
    transport.CloseConnection(connectionId);
    connectionId = 0;
    ioEnabled = false;
    this->receiveLengthD69790 = 0;
    this->compressedReceive.frame.clear();
    this->compressedReceive.frameBytes = 0;
    this->compressedReceive.lineBytes = 0;
}

void CgConnection::Impl::StopIo()
{
    Close();
    transport.Stop();
}

// 完整明文消息沿原Hook顺序提交；只在传输层的短锁内追加或替换字节。
SendResult CgConnection::Impl::SendPacket(
    const platform::ConnectionId expectedId,
    const void* const data,
    const std::uint32_t size,
    bool replacePending)
{
    if (!ioEnabled) {
        return SendResult::Unavailable;
    }
    const auto id = connectionId;
    if (id == 0 || expectedId != id) return SendResult::Unavailable;

    // L04：回调可重入发送；持有字节快照，避免共享格式化缓冲被覆盖。
    const std::string packet(static_cast<const char*>(data), size);
    script::RecordAutoBattleAction(packet);
    script::LuaSendPacket(owner, packet);
    if (id != connectionId || !ioEnabled) return SendResult::Unavailable;
    for (auto& [header, handler] : this->sendHandlers) {
        if (packet.starts_with(header)) {
            handler(owner, header.c_str(), packet.data() + header.size(),
                static_cast<std::int32_t>(packet.size() - header.size()));
            break;
        }
    }

    if (id != connectionId || !ioEnabled) return SendResult::Unavailable;
    transport.SetIoEnabled(id, true);
    const auto queued = transport.TryQueueSend(id,
        {reinterpret_cast<const std::uint8_t*>(packet.data()), packet.size()}, replacePending);
    if (queued != platform::NetworkQueueResult::Accepted)
        return queued == platform::NetworkQueueResult::QueueFull ? SendResult::QueueFull : SendResult::Unavailable;
    // 成功入队后记录明文；不把队列日志当作socket发送成功，避免按TCP分片重复打印。
    m_logw(L"[network][send queued][%u] %.*hs%hs", size, static_cast<int>(size),
        packet.data(), packet.ends_with('\n') ? "" : "\n");
    return SendResult::Queued;
}

std::int32_t CgConnection::Impl::Initialize()
{
    if (!this->gameplayHandlersRegistered) {
        RegisterReceive("BtWt", [](CgConnection&, const char*, char** const arguments) {
            const std::int32_t wait = ParseProtocolInteger_517190(arguments[1]);
            world::gBattleCommandWaitMs = std::bit_cast<std::uint32_t>(wait);
            world::gBattleShowEnemyHpBars = ParseProtocolInteger_517190(arguments[2]) != 0;
            m_log_iw(L"Set battleCommandWait = %d\n", wait);
        });
        RegisterReceive("SpDt", [](CgConnection&, const char*, char** const arguments) {
            world::gChatMinimumIntervalMs = std::bit_cast<std::uint32_t>(
                ParseProtocolInteger_517190(arguments[1]));
        });
        RegisterReceive("INVCF", [](CgConnection&, const char*, char** const arguments) {
            const std::int32_t bagPages = ParseProtocolInteger_517190(arguments[1]);
            const std::int32_t bankPages = ParseProtocolInteger_517190(arguments[2]);
            const std::int32_t bagPage = ParseProtocolInteger_517190(arguments[3]);
            ui::WindowRuntime_SetInventoryPages(bagPages, bankPages, bagPage);
        });
        this->gameplayHandlersRegistered = true;
    }
    if (this->active) Shutdown();
    owner.SetIoEnabled(false);
    int transportError = 0;
    if (!transport.Open(transportError)) {
        this->errorD9F2CC = transportError;
        return 0;
    }

    if (AllocateNetworkWorkspace_516B60(
        0x10000U) != 0) {
        transport.Stop();
        return 0;
    }
    // 新会话复用同一队列owner，仅重置有效数量与轮询状态，避免遗留半包。
    constexpr std::int32_t emptyLength = 0;
    std::memcpy(this->queueBytes.data() + kSendLengthOffset,
        &emptyLength, sizeof(emptyLength));
    this->receiveLengthD69790 = 0;
    this->compressedReceive.frame.clear();
    this->compressedReceive.frameBytes = 0;
    this->compressedReceive.lineBytes = 0;
    this->frameModuloD9F3C8 = 0;
    this->flagsD9F2C0 = 0;
    this->errorD9F2CC = 0;
    this->active = 1;
    return 1;
}

ConnectionPumpResult CgConnection::Impl::Pump()
{
    const std::uint8_t flags = this->flagsD9F2C0;
    if ((flags & 1U) == 0) {
        this->flagsD9F2C0 = static_cast<std::uint8_t>(flags | 1U);
        this->lastTickD9F2C4 = platform::ReadTickCount64();
    }
    if (this->active == 0) {
        return {};
    }

    const auto id = connectionId;
    const auto snapshot = PollConnection();
    transport.SetIoEnabled(id, ioEnabled);
    if (!ioEnabled || id == 0) {
        return {};
    }

    const std::uint32_t frame =
        (this->frameModuloD9F3C8 + 1U) & 7U;
    this->frameModuloD9F3C8 = frame;
    // 后台新到数据留给下一轮，防止持续输入令本帧永远无法结束。
    std::size_t receiveBudget = snapshot.receivedBytes;

    std::uint8_t* const dispatch =
        this->queueBytes.data() + kDispatchOffset;
    const volatile std::uint32_t* const length =
        &this->workspaceCapacity;
    for (;;) {
        const auto previous = static_cast<std::size_t>(this->receiveLengthD69790);
        const auto space = kQueueCapacity - previous;
        if (receiveBudget && space) {
            const auto count = transport.DrainReceived(id,
                {this->queueBytes.data() + kReceiveQueueOffset + previous,
                    (std::min)(space, receiveBudget)});
            this->receiveLengthD69790 += static_cast<std::int32_t>(count);
            receiveBudget -= count;
            if (count) this->receiveResetD9F3C4 = 0;
        }
        const auto beforeParse = this->receiveLengthD69790;
        if (ReceiveProtocolLine(dispatch, static_cast<std::int32_t>(kDispatchSize)) != 0) {
            if (receiveBudget && this->receiveLengthD69790 < beforeParse) continue;
            break;
        }
        const bool diagnoseLogin = login::gConnectionStep >= 71
            && login::gConnectionStep <= 80;
        const std::int32_t dispatchResult = NetworkDispatch_512E40(
            connectionId, dispatch);
        if (id != connectionId || !this->active || !ioEnabled) return {};
        std::memset(dispatch, 0, kDispatchSize);
        std::memset(this->sendMessageBuffer, 0, *length);
        std::memset(this->reservedProtocolBuffer, 0, *length);
        std::memset(this->textTokenBuffer, 0, *length);
        std::memset(this->integerTokenBuffer, 0, *length);
        std::fill_n(this->pointerTable58B614, *length, nullptr);
        std::memset(this->reservedTripleBuffer0, 0, 3U * *length);
        std::memset(this->reservedTripleBuffer1, 0, 3U * *length);
        std::memset(this->reservedTripleBuffer2, 0, 3U * *length);
        std::memset(this->reservedTripleBuffer3, 0, 3U * *length);
        std::fill_n(this->pointerTable58B614, *length, nullptr);
        if (diagnoseLogin) {
            const auto diagnostic = std::format(L"LOGIN DISPATCH RESULT={}", dispatchResult);
            static_cast<void>(login::detail::AppendSystemLog_44B5C0(diagnostic));
        }
    }

    const auto completed = transport.PollConnection(id);
    if (completed.lastSendTick) this->lastTickD9F2C4 = completed.lastSendTick;
    if ((completed.state == platform::ConnectionState::Closed || completed.state == platform::ConnectionState::Failed)
        && completed.receivedBytes == 0) {
        this->errorD9F2CC = completed.error;
        Close();
        return {0, true, completed};
    }

    const std::uint64_t now = platform::ReadTickCount64();
    if (frame == 0 && this->lastTickD9F2C4 + 30000U < now
        && this->active == 1) {
        return {static_cast<std::uint8_t>(
            SendConnectionProbe_512C10(connectionId, L"nr"))};
    }
    return {static_cast<std::uint8_t>(now)};
}

void CgConnection::Impl::Shutdown()
{
    StopIo();

    this->active = 0;
    std::vector<std::uint8_t>().swap(this->compressedReceive.frame);
    this->compressedReceive.frameBytes = 0;
    this->compressedReceive.lineBytes = 0;

    ReleaseProtocolWorkspace();
}

CgConnection::CgConnection() : impl_(std::make_unique<Impl>(*this)) {}
CgConnection::~CgConnection() = default;
bool CgConnection::Initialize() { return impl_->Initialize() != 0; }
void CgConnection::Shutdown() { impl_->Shutdown(); }
platform::ConnectionId CgConnection::BeginConnect(std::wstring host, std::uint16_t port, bool noDelay)
{ return impl_->BeginConnect(std::move(host), port, noDelay); }
platform::ConnectionSnapshot CgConnection::PollConnection() { return impl_->PollConnection(); }
platform::ConnectionId CgConnection::Id() const noexcept { return impl_->connectionId; }
bool CgConnection::Active() const noexcept { return impl_->active != 0; }
std::int32_t CgConnection::Error() const noexcept { return impl_->errorD9F2CC; }
void CgConnection::SetIoEnabled(bool enabled)
{
    impl_->ioEnabled = enabled;
    impl_->transport.SetIoEnabled(impl_->connectionId, enabled);
}
SendResult CgConnection::SendPacket(const void* data, std::uint32_t size, bool replacePending)
{
    const auto session = CaptureSession();
    return impl_->SendPacket(impl_->contextId, data, size, replacePending);
}
ConnectionPumpResult CgConnection::Pump() { return impl_->Pump(); }
void CgConnection::RegisterReceive(const char* header, ProtocolReceiveHandler handler)
{ impl_->RegisterReceive(header, std::move(handler)); }
void CgConnection::RegisterSend(const char* header, ProtocolSendHandler handler)
{ impl_->sendHandlers.emplace_back(header, std::move(handler)); }
void CgConnection::Close() { impl_->Close(); }
void CgConnection::StopIo() { impl_->StopIo(); }

// 唯一对象跨重连/Shutdown保留，身份序列不重置。
static CgConnection gGameConnection;
CgConnection& GetGameConnection() noexcept { return gGameConnection; }

CgConnection::SessionScope::SessionScope(CgConnection& owner) noexcept
    : owner_(owner), previousBound_(owner.impl_->contextBound), previousId_(owner.impl_->contextId)
{
    if (!owner_.impl_->contextBound) owner_.impl_->contextId = owner_.Id();
    owner_.impl_->contextBound = true;
}
CgConnection::SessionScope::~SessionScope()
{
    owner_.impl_->contextBound = previousBound_;
    owner_.impl_->contextId = previousId_;
}
CgConnection::SessionScope CgConnection::CaptureSession() noexcept { return SessionScope(*this); }
bool CgConnection::IoEnabled() const noexcept { return impl_->ioEnabled; }
void CgConnection::ResetLoginReply() noexcept { impl_->protocolModeDA3B22 = 0; }
bool CgConnection::LoginAccepted() const noexcept { return impl_->protocolModeDA3B22 != 0; }
std::string CgConnection::EncodeInteger(std::int32_t value)
{ return reinterpret_cast<const char*>(impl_->FormatProtocolInteger_517670(value)); }
std::string CgConnection::EscapeText(std::wstring_view text)
{ return reinterpret_cast<const char*>(impl_->EscapeProtocolText_5171F0(text)); }
std::string CgConnection::UnescapeText(const char* text)
{ return reinterpret_cast<const char*>(impl_->UnescapeProtocolText_5172D0(text)); }

// 这三条UI路径保留原十进制字段，不合并到Base62的同名协议。
SendResult CgConnection::MoveItemToBagPage(std::int32_t source, std::int32_t page)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    const auto* encoded = impl_->FormatProtocolInteger_517670(source);
    char packet[64];
    std::snprintf(packet, sizeof(packet), "SWITMP %s %d -1", reinterpret_cast<const char*>(encoded), page);
    return impl_->SendProtocolBuffer_517480(impl_->contextId, packet);
}
SendResult CgConnection::SelectBagPage(std::int32_t page)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    char packet[64];
    std::snprintf(packet, sizeof(packet), "SWITM %d", page);
    return impl_->SendProtocolBuffer_517480(impl_->contextId, packet);
}
SendResult CgConnection::SelectInventoryBankPage(std::int32_t page)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    char packet[64];
    std::snprintf(packet, sizeof(packet), "rh %d", page);
    return impl_->SendProtocolBuffer_517480(impl_->contextId, packet);
}

// 原 [0x512B90,0x512BCC)：列表请求的三次共享缓冲读取保持独立。
SendResult CgConnection::SendCharacterListRequest()
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "Gp"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        "",
        capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512BD0,0x512C0C)：登出请求的三次共享缓冲读取保持独立。
SendResult CgConnection::SendLogoutRequest()
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "WA"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        "",
        capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512C60,0x512C9C)：保活命令后追加 593850 当前文本并发送。
SendResult CgConnection::SendKeepAliveRequest()
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "DE"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        "",
        capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511CD0,0x511D0C)：as 请求，追加共享原字节文本。
SendResult CgConnection::RequestItemTargetAttributes()
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "as"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        "",
        capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511FB0,0x511FEC)：ixH 请求，追加共享原字节文本。
SendResult CgConnection::SendTradeRequest()
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "ixH"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        "",
        capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512230,0x51226C)：suI 请求，追加共享原字节文本。
SendResult CgConnection::WithdrawTradeConfirmation()
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "suI"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        "",
        capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512570,0x5125AC)：ih 请求，追加共享原字节文本。
SendResult CgConnection::ToggleBattleRowRequest()
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "ih"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        "",
        capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512CF0,0x512D2C)：lO 请求，追加共享原字节文本。
SendResult CgConnection::RequestLogoutToTown()
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "lO"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        "",
        capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512DF0,0x512E38)：容量仅在命令构造之后、整数格式化之前采样。
SendResult CgConnection::SendLoginTimestamp(const std::uint32_t tickBits)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "CCL"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(
        std::bit_cast<std::int32_t>(tickBits));
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512D30,0x512D9B)：两项分别采样容量，格式化后重读输出缓冲。
SendResult CgConnection::SendVersionUpgradeRequest(const std::int32_t version,
    const std::wstring_view payload)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "Xoee"));

    const std::int32_t integerCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const integerToken = impl_->FormatProtocolIntegerToken_517120(version);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(integerToken),
        integerCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(payload);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512DA0,0x512DE8)：命令构造后采一次容量，再格式化和追加整数。
SendResult CgConnection::SendCraftDuration(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "IPy"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x510570,0x510621)：四项各自采容量、格式化后重读共享输出缓冲。
SendResult CgConnection::SendPlayerMovementPath(const std::int32_t first,
    const std::int32_t second,
    const std::wstring_view text,
    const std::int32_t fourth)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "zA"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const fourthToken = impl_->FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512B00,0x512B8E)：每项均先读容量，格式化后重新读取输出缓冲。
SendResult CgConnection::SendCharacterLoginRequest(const std::int32_t field4C,
    const std::int32_t field50,
    const std::int32_t field18)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "emRV"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(field4C);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(field50);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(field18);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512AB0,0x512AF8)：一次容量采样，输出缓冲在各调用点重新读取。
SendResult CgConnection::SendCharacterDeletionRequest(const std::int32_t registNumber)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "Godf"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(registNumber);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// NLD01／原512800：摘要、口令、账号、适配器四轮独立采容量，转义后重读输出缓冲。
SendResult CgConnection::SendAccountLoginRequest(const std::wstring_view executableDigest,
    const std::wstring_view password,
    const std::wstring_view account,
    const std::wstring_view adapterId)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "JFVf"));

    const std::int32_t digestCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const digestToken =
        impl_->FormatProtocolTextToken_517160(executableDigest);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(digestToken),
        digestCapacity));

    const std::int32_t passwordCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const passwordToken = impl_->FormatProtocolTextToken_517160(password);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(passwordToken),
        passwordCapacity));

    const std::int32_t accountCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const accountToken =
        impl_->FormatProtocolTextToken_517160(account);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(accountToken),
        accountCapacity));

    const std::int32_t adapterCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const adapterToken =
        impl_->FormatProtocolTextToken_517160(adapterId);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(adapterToken),
        adapterCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5128C0,0x512AAC)：每项独立读取容量，格式化后重读输出缓冲。
SendResult CgConnection::SendCharacterCreationRequest(const std::int32_t field1,
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
    const std::int32_t field13)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "aSEl"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(field1);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(field2);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::array<std::int32_t, 11> remainingIntegerFields {
        field3, field4, field5, field6, field7, field8,
        field9, field10, field11, field12, field13
    };
    for (const std::int32_t field : remainingIntegerFields) {
        const std::int32_t capacity = std::bit_cast<std::int32_t>(
            impl_->workspaceCapacity);
        std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(field);
        static_cast<void>(AppendProtocolText_5170D0(
            reinterpret_cast<char*>(impl_->sendMessageBuffer),
            reinterpret_cast<const char*>(token),
            capacity));
    }

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x510630,0x510704)：五轮各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::SendMapTransitionRequest(const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth,
    const std::int32_t fifth)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "pg"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const fourthToken = impl_->FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const fifthToken = impl_->FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x510710,0x510807)：六轮各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::RequestMapRectangle(const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth,
    const std::int32_t fifth,
    const std::int32_t sixth)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "UUN"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const fourthToken = impl_->FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const fifthToken = impl_->FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    const std::int32_t sixthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const sixthToken = impl_->FormatProtocolIntegerToken_517120(sixth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(sixthToken),
        sixthCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原函数：0x00510AC0。
SendResult CgConnection::SendItemUseRequest(const std::int32_t x, const std::int32_t y,
    const std::int32_t item, const std::int32_t target)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "iVfo"));
    const std::array<std::int32_t, 4> fields {x, y, item, target};
    for (const std::int32_t field : fields) {
        const std::int32_t capacity = std::bit_cast<std::int32_t>(
            impl_->workspaceCapacity);
        std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(field);
        static_cast<void>(AppendProtocolText_5170D0(
            reinterpret_cast<char*>(impl_->sendMessageBuffer),
            reinterpret_cast<const char*>(token), capacity));
    }
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// NFI01：fIM三Base62整数按现明文普通空格发送；保原time/seed/rand与共享缓冲时序。
// 原函数：0x00510B80。
SendResult CgConnection::SendPositionDirectionInteraction(const std::int32_t x,
    const std::int32_t y, const std::int32_t direction)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "fIM"));
    const std::array<std::int32_t, 3> fields {x, y, direction};
    for (const std::int32_t field : fields) {
        const std::int32_t capacity = std::bit_cast<std::int32_t>(
            impl_->workspaceCapacity);
        std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(field);
        static_cast<void>(AppendProtocolText_5170D0(
            reinterpret_cast<char*>(impl_->sendMessageBuffer),
            reinterpret_cast<const char*>(token), capacity));
    }

    char* buffer = reinterpret_cast<char*>(impl_->sendMessageBuffer);
    std::int32_t selected = 1;
    std::int32_t spaces = 0;
    std::uint32_t index = 0;
    if (std::strlen(buffer) != 0) {
        do {
            if (buffer[index] == ' ') {
                ++spaces;
            }
            if (spaces >= 3) {
                break;
            }
            ++index;
        } while (index < std::strlen(buffer));
        if (spaces > 0) {
            const __time64_t seconds = ::_time64(nullptr);
            static_cast<void>(
                platform::SeedRandom_53B4BF(static_cast<std::uint32_t>(seconds)));
            selected = platform::Rand16() % spaces;
            if (selected == 0) {
                selected = 1;
            }
            buffer = reinterpret_cast<char*>(impl_->sendMessageBuffer);
        }
    }
    std::int32_t ordinal = 0;
    for (index = 0; index < std::strlen(buffer); ++index) {
        if (buffer[index] == ' ') {
            if (ordinal == selected) {
                // 服务端不以0x17终止整数token，保普通空格；原选择/RNG消费及fresh借用保持。
                buffer = reinterpret_cast<char*>(impl_->sendMessageBuffer);
                break;
            }
            ++ordinal;
        }
    }
    return impl_->SendProtocolBuffer_517480(impl_->contextId, buffer);
}

// 原 [0x510810,0x51087B)：两轮各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::SendPlayerCoordinates(const std::int32_t first,
    const std::int32_t second)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "KeOX"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511030,0x5110AE)：先追加数量，再逐项读取 W32 并分别采容量。
SendResult CgConnection::RequestAlbumEntries(const std::int32_t* values,
    const std::int32_t count)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "kza"));

    const std::int32_t countCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const countToken = impl_->FormatProtocolIntegerToken_517120(count);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(countToken),
        countCapacity));

    for (std::int32_t index = 0; index < count; ++index) {
        const std::int32_t capacity = std::bit_cast<std::int32_t>(
            impl_->workspaceCapacity);
        const std::int32_t value = values[index];
        std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
        static_cast<void>(AppendProtocolText_5170D0(
            reinterpret_cast<char*>(impl_->sendMessageBuffer),
            reinterpret_cast<const char*>(token),
            capacity));
    }

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511190,0x5111D8)：FPkR单整数发送；容量先采，输出缓冲独立重取。
SendResult CgConnection::RequestContactDeletion(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "FPkR"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token), capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原函数：0x00511250。
SendResult CgConnection::SendGuildTextUpdate(const std::int32_t kind, const std::wstring_view text)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "Ufm"));
    const std::int32_t kindCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const kindToken = impl_->FormatProtocolIntegerToken_517120(kind);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(kindToken), kindCapacity));
    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken), textCapacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5112C0,0x51134E)：两整数、文本各自采容量并重读输出缓冲。
SendResult CgConnection::SendGuildRankDefinition(const std::int32_t first,
    const std::int32_t second,
    const std::wstring_view text)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "GY"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511350,0x5113BB)：两整数分别采容量并在格式化后重读输出缓冲。
SendResult CgConnection::SendGuildMemberRankChange(const std::int32_t first,
    const std::int32_t second)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "uHb"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x510880,0x5108EB)：对战请求，发送坐标不是目标格坐标。
SendResult CgConnection::SendDuelRequest(const std::int32_t first,
    const std::int32_t second)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "ik"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x510940,0x5109AB)：观战命令原字节为大小写敏感的IMv。
SendResult CgConnection::SendSpectateRequest(const std::int32_t first,
    const std::int32_t second)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "IMv"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5111E0,0x51124B)：名片请求，两字段分别采容量并重读共享缓冲。
SendResult CgConnection::SendContactCardRequest(const std::int32_t first,
    const std::int32_t second)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "vcAz"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5113C0,0x51142B)：两整数各自采容量并在格式化后重读输出缓冲。
SendResult CgConnection::SendGuildInvitation(const std::int32_t first,
    const std::int32_t second)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "npzi"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511430,0x511478)：单次采容量、格式化后重读输出缓冲发送。
SendResult CgConnection::RequestGuildMemberDeletion(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "dbQC"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原[510FA0,51102E)：三次采容量与格式化后的输出地址重取独立。
SendResult CgConnection::SendContactMail(const std::int32_t first,
    const std::wstring_view text, const std::int32_t third)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "CV"));
    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken), firstCapacity));
    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken), textCapacity));
    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken), thirdCapacity));
    return impl_->SendProtocolBuffer_517480(impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511480,0x51150E)：整数、文本、整数分别采容量并重读输出缓冲。
SendResult CgConnection::SendGuildMail(const std::int32_t first,
    const std::wstring_view text,
    const std::int32_t third)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "uId"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原[5110B0,511184)：五字段各自采容量，格式化后重取输出地址。
SendResult CgConnection::SendContactPetMail(const std::int32_t first, const std::int32_t second,
    const std::int32_t third, const std::wstring_view text, const std::int32_t fifth)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "rMsv"));
    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken), firstCapacity));
    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken), secondCapacity));
    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken), thirdCapacity));
    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken), textCapacity));
    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const fifthToken = impl_->FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fifthToken), fifthCapacity));
    return impl_->SendProtocolBuffer_517480(impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511510,0x5115E4)：三整数、文本、末整数分别采容量并重读输出缓冲。
SendResult CgConnection::SendGuildPetMail(const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::wstring_view text,
    const std::int32_t fifth)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "PfqV"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const fifthToken = impl_->FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5115F0,0x511638)：单次采容量、整数格式化后重读缓冲发送。
SendResult CgConnection::RequestProfileCategoryUpdates(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "HoW"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511640,0x51175A)：七项依次采容量、格式化并重读输出缓冲。
SendResult CgConnection::SendPlayerProfile(const std::int32_t field1,
    const std::wstring_view text2,
    const std::int32_t field3,
    const std::wstring_view text4,
    const std::int32_t field5,
    const std::wstring_view text6,
    const std::wstring_view text7)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "Ot"));

    const std::int32_t capacity1 = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token1 = impl_->FormatProtocolIntegerToken_517120(field1);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token1),
        capacity1));

    const std::int32_t capacity2 = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token2 = impl_->FormatProtocolTextToken_517160(text2);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token2),
        capacity2));

    const std::int32_t capacity3 = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token3 = impl_->FormatProtocolIntegerToken_517120(field3);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token3),
        capacity3));

    const std::int32_t capacity4 = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token4 = impl_->FormatProtocolTextToken_517160(text4);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token4),
        capacity4));

    const std::int32_t capacity5 = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token5 = impl_->FormatProtocolIntegerToken_517120(field5);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token5),
        capacity5));

    const std::int32_t capacity6 = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token6 = impl_->FormatProtocolTextToken_517160(text6);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token6),
        capacity6));

    const std::int32_t capacity7 = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token7 = impl_->FormatProtocolTextToken_517160(text7);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token7),
        capacity7));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511760,0x5117CB)：两项各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::RequestPlayerProfile(const std::int32_t first,
    const std::int32_t second)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "TLKH"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5117D0,0x51183B)：整数、文本各自采容量并在格式化后重读输出缓冲。
SendResult CgConnection::RequestMiniMailContactDeletion(const std::int32_t value,
    const std::wstring_view text)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "wD"));

    const std::int32_t valueCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const valueToken = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(valueToken),
        valueCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511840,0x5118AB)：文本、整数各自采容量并在格式化后重读输出缓冲。
SendResult CgConnection::SendMiniMail(const std::wstring_view text,
    const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "GH"));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t valueCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const valueToken = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(valueToken),
        valueCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5118B0,0x5118F8)：单次容量采样、整数格式化后重读缓冲发送。
SendResult CgConnection::SetProfileVisibilityRequest(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "lz"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511900,0x5119F7)：六项各自先读容量，格式化后重读输出缓冲。
// CTS01：原六字段逐项容量→token→新buffer→append，末发送S32透传。
SendResult CgConnection::SendChatMessage(const std::int32_t first,
    const std::int32_t second,
    const std::wstring_view text,
    const std::int32_t fourth,
    const std::int32_t fifth,
    const std::int32_t sixth)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "uSr"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const fourthToken = impl_->FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const fifthToken = impl_->FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    const std::int32_t sixthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const sixthToken = impl_->FormatProtocolIntegerToken_517120(sixth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(sixthToken),
        sixthCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511A00,0x511A48)：构造命令后只采一次容量，追加并发送。
SendResult CgConnection::SendPlayerOptions(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "dcMv"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511A50,0x511ADE)：三项各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::SendTeamJoinLeaveRequest(const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "zn"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511AE0,0x511BB4)：五项各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::SendPetStates(const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth,
    const std::int32_t fifth)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "LKQy"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const fourthToken = impl_->FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const fifthToken = impl_->FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511BC0,0x511C2B)：两项各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::MovePetSlotRequest(const std::int32_t first,
    const std::int32_t second)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "Ukuy"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511C30,0x511C78)：构造命令后只采一次容量，追加并发送。
SendResult CgConnection::RequestSkillTargetAttributes(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "iET"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511C80,0x511CC8)：一次容量采样，整数 token 追加后发送。
SendResult CgConnection::SelectSkillTextTarget(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "McVa"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511D10,0x511D58)：一次容量采样，整数 token 追加后发送。
SendResult CgConnection::SelectItemTextTarget(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "mjCv"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511D60,0x511DCB)：两项各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::SendItemTargetPair(const std::int32_t first,
    const std::int32_t second)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "BG"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511DD0,0x511E5E)：三项各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::SendWorldActionRequest(const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "MIS"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511E60,0x511EEE)：三项各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::SendPlayerMotionRequest(const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "klF"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511EF0,0x511FA1)：三个整数及文本各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::SendSkillUseRequest(const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::wstring_view text)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "sM"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x511FF0,0x512038)：容量在格式化前采样，追加和发送各自重读缓冲。
SendResult CgConnection::SelectTradePartnerRequest(const std::int32_t index)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "QeuC"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(index);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token), capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512040,0x5120CE)：两段文本和整数各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::SendTradeOffer(const std::wstring_view first,
    const std::wstring_view second,
    const std::int32_t third)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "aiAb"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolTextToken_517160(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolTextToken_517160(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5120D0,0x51215E)：三段文本各自采容量、格式化后重读输出缓冲。
SendResult CgConnection::PublishStallRequest(const std::wstring_view first,
    const std::wstring_view second,
    const std::wstring_view third)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "ylGN"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolTextToken_517160(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolTextToken_517160(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolTextToken_517160(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512160,0x512185)：建命令后重读缓冲并完整透传发送返回。
SendResult CgConnection::CloseStallRequest()
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "JMh"));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512190,0x5121D8)：一次容量采样，整数 token 追加后发送。
SendResult CgConnection::RequestStallContents(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "ElVN"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5121E0,0x512228)：容量在格式化前采样，追加及发送时重读缓冲。
SendResult CgConnection::PurchaseStallOffer(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "wAo"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原函数：0x005127B0。
SendResult CgConnection::SelectBankPageRequest(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "rh"));
    const std::int32_t capacity =
        std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token), capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512270,0x5122B8)：格式化前采容量，追加与发送各自重读缓冲。
SendResult CgConnection::SendTradeDecision(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "OU"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5122C0,0x512308)：格式化前采容量，追加与发送各自重读缓冲。
SendResult CgConnection::SelectPlayerTitleRequest(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "zGp"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512310,0x512358)：格式化前采容量，追加与发送各自重读缓冲。
SendResult CgConnection::DeletePlayerTitleRequest(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "tj"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512360,0x5123A8)：转义前采容量，追加与发送各自重读缓冲。
SendResult CgConnection::SetCustomPlayerTitleRequest(const std::wstring_view text)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "ACwB"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5123B0,0x5123F8)：格式化前采容量，追加与发送各自重读缓冲。
SendResult CgConnection::IncreasePlayerAttributeRequest(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "IHw"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512400,0x51246B)：两轮分别采容量、格式化后重读共享缓冲。
SendResult CgConnection::IncreasePetAttributeRequest(const std::int32_t first,
    const std::int32_t second)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "kjSK"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512470,0x5124DB)：两轮分别采容量、格式化后重读共享缓冲。
SendResult CgConnection::SwapPlayerSkillOrderRequest(const std::int32_t first,
    const std::int32_t second)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "Noeb"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x510E30,0x510EBE)：放地请求，容量读取先于格式化，缓冲读取在其后。
SendResult CgConnection::DropPetRequest(const std::int32_t x,
    const std::int32_t y,
    const std::int32_t pet)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "FvWm"));
    const std::int32_t xCapacity = std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const xToken = impl_->FormatProtocolIntegerToken_517120(x);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(xToken), xCapacity));
    const std::int32_t yCapacity = std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const yToken = impl_->FormatProtocolIntegerToken_517120(y);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(yToken), yCapacity));
    const std::int32_t petCapacity = std::bit_cast<std::int32_t>(impl_->workspaceCapacity);
    std::uint8_t* const petToken = impl_->FormatProtocolIntegerToken_517120(pet);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(petToken), petCapacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5124E0,0x51256E)：三轮分别采容量、格式化后重读共享缓冲。
SendResult CgConnection::SwapPetSkillOrderRequest(const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "HJQp"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x5125B0,0x51261B)：两轮各自采容量、格式化后重读共享输出缓冲。
SendResult CgConnection::RenamePetRequest(const std::int32_t value,
    const std::wstring_view text)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "ycX"));

    const std::int32_t valueCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const valueToken = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(valueToken),
        valueCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512620,0x512717)：六轮分别采容量、格式化后重读共享输出缓冲。
// SDM01：原512620完整77条；xD五S32及文本各先容量再token/fresh输出，当前原字段/LF明文及S32返回保。
SendResult CgConnection::SendDialogResponse(const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third,
    const std::int32_t fourth,
    const std::int32_t fifth,
    const std::wstring_view text)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "xD"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    const std::int32_t fourthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const fourthToken = impl_->FormatProtocolIntegerToken_517120(fourth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fourthToken),
        fourthCapacity));

    const std::int32_t fifthCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const fifthToken = impl_->FormatProtocolIntegerToken_517120(fifth);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(fifthToken),
        fifthCapacity));

    const std::int32_t textCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const textToken = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(textToken),
        textCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// IMT01：原55条，四S32字段容量先格式/输出重读及S32发送返回，当前原字段/LF明文，正确正文保。
// 原函数：0x00510CE0。
SendResult CgConnection::DropItemRequest(const std::int32_t x, const std::int32_t y,
    const std::int32_t group, const std::int32_t index)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "QpfE"));

    const std::int32_t xCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const xToken = impl_->FormatProtocolIntegerToken_517120(x);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(xToken), xCapacity));

    const std::int32_t yCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const yToken = impl_->FormatProtocolIntegerToken_517120(y);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(yToken), yCapacity));

    const std::int32_t groupCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const groupToken = impl_->FormatProtocolIntegerToken_517120(group);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(groupToken), groupCapacity));

    const std::int32_t indexCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const indexToken = impl_->FormatProtocolIntegerToken_517120(index);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(indexToken), indexCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原函数：0x00510A00。
SendResult CgConnection::BeginItemUseRequest(const std::int32_t x, const std::int32_t y,
    const std::int32_t index, const std::int32_t group)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "Ak"));

    const std::int32_t xCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const xToken = impl_->FormatProtocolIntegerToken_517120(x);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(xToken), xCapacity));

    const std::int32_t yCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const yToken = impl_->FormatProtocolIntegerToken_517120(y);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(yToken), yCapacity));

    const std::int32_t indexCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const indexToken = impl_->FormatProtocolIntegerToken_517120(index);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(indexToken), indexCapacity));

    const std::int32_t groupCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const groupToken = impl_->FormatProtocolIntegerToken_517120(group);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(groupToken), groupCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原函数：0x00516C90。
SendResult CgConnection::MoveItemBetweenOwnersRequest(const std::int32_t sourceGroup, const std::int32_t source,
    const std::int32_t targetGroup, const std::int32_t target, const std::int32_t amount)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "Cpa"));

    const std::int32_t sourceGroupCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const sourceGroupToken = impl_->FormatProtocolIntegerToken_517120(sourceGroup);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(sourceGroupToken), sourceGroupCapacity));

    const std::int32_t sourceCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const sourceToken = impl_->FormatProtocolIntegerToken_517120(source);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(sourceToken), sourceCapacity));

    const std::int32_t targetGroupCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const targetGroupToken = impl_->FormatProtocolIntegerToken_517120(targetGroup);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(targetGroupToken), targetGroupCapacity));

    const std::int32_t targetCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const targetToken = impl_->FormatProtocolIntegerToken_517120(target);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(targetToken), targetCapacity));

    const std::int32_t amountCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const amountToken = impl_->FormatProtocolIntegerToken_517120(amount);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(amountToken), amountCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原函数：0x00516D70。
SendResult CgConnection::SendIuEPacket()
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "iuE"));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原函数：0x005109B0。
SendResult CgConnection::SendBattlePacket(const std::wstring_view text)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "Rg"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolTextToken_517160(text);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token), capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// MDR01：原510DA0完整44条；woz三个S32，各容量先token/fresh输出，末真实S32发送结果，原字段/LF明文。
SendResult CgConnection::DropGoldRequest(const std::int32_t x,
    const std::int32_t y, const std::int32_t amount)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "woz"));

    const std::int32_t xCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const xToken = impl_->FormatProtocolIntegerToken_517120(x);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(xToken), xCapacity));

    const std::int32_t yCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const yToken = impl_->FormatProtocolIntegerToken_517120(y);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(yToken), yCapacity));

    const std::int32_t amountCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const amountToken = impl_->FormatProtocolIntegerToken_517120(amount);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(amountToken), amountCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原函数：0x00510EC0。
SendResult CgConnection::MoveItemStackRequest(const std::int32_t source,
    const std::int32_t target, const std::int32_t amount)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "yi"));

    const std::int32_t sourceCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const sourceToken = impl_->FormatProtocolIntegerToken_517120(source);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(sourceToken), sourceCapacity));

    const std::int32_t targetCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const targetToken = impl_->FormatProtocolIntegerToken_517120(target);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(targetToken), targetCapacity));

    const std::int32_t amountCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const amountToken = impl_->FormatProtocolIntegerToken_517120(amount);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(amountToken), amountCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原函数：0x00510F50。
SendResult CgConnection::SendSkillRebirthSelection(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "fS"));

    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token),
        capacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原函数：0x005108F0。
SendResult CgConnection::AcknowledgeBattleExit(const std::int32_t value)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "eL"));
    const std::int32_t capacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const token = impl_->FormatProtocolIntegerToken_517120(value);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(token), capacity));
    return impl_->SendProtocolBuffer_517480(
        impl_->contextId, reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

// 原 [0x512720,0x5127AE)：三项各自采容量、格式化后重读共享输出缓冲。
SendResult CgConnection::SendPlayerPositionAndDirection(const std::int32_t first,
    const std::int32_t second,
    const std::int32_t third)
{
    const auto session = CaptureSession();
    if (!impl_->active) return SendResult::Unavailable;
    static_cast<void>(BuildProtocolCommand_517510(
        reinterpret_cast<char*>(impl_->sendMessageBuffer), "cZt"));

    const std::int32_t firstCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const firstToken = impl_->FormatProtocolIntegerToken_517120(first);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(firstToken),
        firstCapacity));

    const std::int32_t secondCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const secondToken = impl_->FormatProtocolIntegerToken_517120(second);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(secondToken),
        secondCapacity));

    const std::int32_t thirdCapacity = std::bit_cast<std::int32_t>(
        impl_->workspaceCapacity);
    std::uint8_t* const thirdToken = impl_->FormatProtocolIntegerToken_517120(third);
    static_cast<void>(AppendProtocolText_5170D0(
        reinterpret_cast<char*>(impl_->sendMessageBuffer),
        reinterpret_cast<const char*>(thirdToken),
        thirdCapacity));

    return impl_->SendProtocolBuffer_517480(
        impl_->contextId,
        reinterpret_cast<char*>(impl_->sendMessageBuffer));
}

} // namespace cg::network
