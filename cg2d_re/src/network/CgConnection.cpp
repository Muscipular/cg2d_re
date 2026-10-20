#include "cg/core/LegacyNoOp.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/script/LuaRuntime.hpp"
#include "cg/core/m_log.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "CgConnectionLegacy.hpp"
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

static std::int32_t DefaultNetworkCallback(
    std::uintptr_t,
    const void*,
    std::uint32_t)
{
    return 0;
}

// N02：网络对象拥有半帧、连续块输出和待分行正文，不借用recv局部缓冲。
struct CompressedReceiveState {
    std::vector<std::uint8_t> frame; // size为头部声明的总帧长，包含四字节头。
    std::size_t frameBytes = 0; // 已填充的帧字节数。
    // 多一个块仅供“先解压再判逻辑容量”，不得发布超过0x8100的整帧输出。
    std::array<char, kLz4FrameDecodedCapacity + kLz4BlockSize> frameOutput;
    std::array<char, kDecodedQueueCapacity> lines;
    std::size_t lineBytes = 0; // 含完整LF行及尚未收齐的尾行。
};

// CTO01：每个连接对象私有的主线程协议工作区；不暴露可变状态。
struct CgConnection::Impl {
    std::uint16_t protocolModeDA3B22; // 原全局：0x00DA3B22。
    __time64_t time62B394; // 原全局：0x0062B394。 原 __time32_t，独立于 0xDA3B28 回复码。
    std::tm* localTime62B380; // 原全局：0x0062B380。 原 W32 指针槽，借用CRT同线程共享std::tm。
    using SendCallback = std::function<std::int32_t(std::uintptr_t, const void*, std::uint32_t)>;
    SendCallback sendCallback; // 原 0x58B5E8。
    std::uint32_t workspaceCapacity;
    std::uint8_t* sendMessageBuffer; // 原全局：0x0058B5F0。 发送命令与字段的完整消息拼接缓冲区。
    std::uint8_t* reservedProtocolBuffer; // 原全局：0x0058B5F4。 保留缓冲区，当前仅有生命周期操作。
    std::uint8_t* textTokenBuffer; // 原全局：0x0058B5F8。 文本字段转义、反转义共用的临时缓冲区。
    std::uint8_t* integerTokenBuffer; // 原全局：0x0058B5FC。 整数字段及尾随空格的临时缓冲区。
    char** pointerTable58B614; // 原全局：0x0058B614。
    std::uint8_t* reservedTripleBuffer0; // 原全局：0x0058B604。 三倍容量保留缓冲区，当前仅有生命周期操作。
    std::uint8_t* reservedTripleBuffer1; // 原全局：0x0058B608。 三倍容量保留缓冲区，当前仅有生命周期操作。
    std::uint8_t* reservedTripleBuffer2; // 原全局：0x0058B60C。 三倍容量保留缓冲区，当前仅有生命周期操作。
    std::uint8_t* reservedTripleBuffer3; // 原全局：0x0058B610。 三倍容量保留缓冲区，当前仅有生命周期操作。
    SendProtocolObserver sendObserver58B61C; // 原全局：0x0058B61C。
    std::wstring sendLogPathDA3B58; // 原全局：0x00DA3B58。
    std::wstring receiveLogPathDA3F9C; // 原全局：0x00DA3F9C。
    std::uint8_t integerFormatScratch[64]; // 原 [0xDA3F58,0xDA3F98)。
    std::uint8_t** workspaceSlots; // 原 0xDA3F98，18 个同SDK域的字节缓冲指针。
    std::uint8_t active;   // 原 0xD9F2C8，网络运行期活动门禁。
    // 保留原[0xD8F2B8,0xD9F2C0)布局供主线程分发/接收；待发字节由后端独占。
    std::array<std::uint8_t, kQueueStorageSize> queueBytes; // 原全局：0x00D8F2B8。
    std::int32_t receiveLengthD69790; // 原 0xD69790，独立于连续队列。
    std::uint8_t flagsD9F2C0; // 原全局：0x00D9F2C0。
    std::uint64_t lastTickD9F2C4; // 原全局：0x00D9F2C4。
    std::int32_t errorD9F2CC; // 原全局：0x00D9F2CC。
    std::uint32_t receiveResetD9F3C4; // 原全局：0x00D9F3C4。
    std::uint32_t frameModuloD9F3C8; // 原全局：0x00D9F3C8。
    std::uint8_t value58ADA4 = 0x80; // 原全局：0x0058ADA4。 原ed回复独立字节，不属于58ADA8的排除链文本。
    std::wstring addressTextDA33D8; // 原全局：0x00DA33D8。 lSZ回复的地址文字。
    // N01：稳定节点持有注册头／闭包，回调内追加注册不使当前函数对象失效。
    std::list<std::pair<std::string, ProtocolReceiveHandler>> receiveHandlers;
    bool gameplayHandlersRegistered = false; // G01/G06/G07：业务回调只注册一次，重连保留。
    std::list<std::pair<std::string, ProtocolSendHandler>> sendHandlers;
    CompressedReceiveState compressedReceive; // N02：会话数据随初始化／关闭重置。
    platform::CgTransport transport;
    platform::ConnectionId connectionId {};
    bool ioEnabled {};
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
    std::int32_t SendProtocolBuffer_517480(const std::uintptr_t socket, char* const buffer);
    std::int32_t SendConnectionProbe_512C10(const std::uintptr_t socket, const std::wstring_view text);
    std::int32_t AppendReceiveQueue_5081E0(const void* const source, const std::uint32_t size);
    std::int32_t ConsumeReceiveQueue_508230(const std::int32_t size);
    std::uint16_t ReadProtocolModeDA3B22() noexcept;
    void WriteProtocolModeDA3B22(const std::uint16_t value) noexcept;
    void SplitReceivedBuffer_5173B0(char* const buffer);
    std::int32_t NetworkDispatch_512E40(const std::uintptr_t socket, const void* const received);
    void HandleServerAddressChangeReply_50E6E0(const std::uintptr_t, const char* const text);
    std::int32_t HandleVersionUpgradeReply_50E780(const std::uintptr_t, const std::int32_t value);
    std::int32_t AllocateNetworkWorkspace_516B60(const SendCallback& callback, const std::uint32_t bufferLength, const std::uintptr_t);
    void ReleaseProtocolWorkspace();
    bool AppendDecodedProtocolBytes(const void* const data, const std::size_t size);
    std::int32_t ReceiveProtocolLine( void* const destination, const std::int32_t capacity);
    void HandleResponse_508800(const std::uintptr_t, const std::int32_t value, const char* const text);
    platform::ConnectionId BeginConnect(std::wstring host, std::uint16_t port, bool noDelay);
    platform::ConnectionSnapshot PollConnection();
    void Close();
    void StopIo();
    std::int32_t SendPacket(const std::uintptr_t connection, const void* const data, const std::uint32_t size);
    std::int32_t Initialize();
    ConnectionPumpResult Pump();
    void Shutdown();
    template <typename Value>
    Value DecodeDispatchArgument(const std::size_t index, const bool workspaceBeforeCapacity);
    template <typename... Values, std::size_t... Indices>
    std::tuple<Values...> DecodeDispatchArguments(std::index_sequence<Indices...>, const std::uint32_t workspaceBeforeCapacityMask);
    template <auto Callback, typename... Values>
    bool TryDispatchReply(const char* const head, const char* const command, const std::uintptr_t socket, const std::uint32_t workspaceBeforeCapacityMask = 0U);
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
static void IgnoreReplyArguments_509080(const std::uintptr_t, Values...)
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
    const char* const head, const char* const command, const std::uintptr_t socket,
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
    std::apply([this, socket](const auto... arguments) {
        if constexpr (std::is_member_function_pointer_v<decltype(Callback)>)
            static_cast<void>((this->*Callback)(socket, arguments...));
        else
            static_cast<void>(Callback(socket, arguments...));
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

std::int32_t CgConnection::Impl::SendProtocolBuffer_517480(
    const std::uintptr_t socket,
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
        if (socket != connectionId || !this->active) return -100;
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

    return this->sendCallback(socket, buffer, length);
}

std::int32_t CgConnection::Impl::SendConnectionProbe_512C10(
    const std::uintptr_t socket,
    const std::wstring_view text)
{
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
        socket,
        reinterpret_cast<char*>(this->sendMessageBuffer));
}

std::int32_t CgConnection::Impl::AppendReceiveQueue_5081E0(
    const void* const source,
    const std::uint32_t size)
{
    if (!ioEnabled) {
        return -100;
    }

    const std::int32_t previous = this->receiveLengthD69790;
    const std::int32_t next =
        previous + static_cast<std::int32_t>(size);
    if (next > static_cast<std::int32_t>(kQueueCapacity)) {
        return -1;
    }

    std::memcpy(
        this->queueBytes.data() + kReceiveQueueOffset + previous,
        source,
        size);
    this->receiveLengthD69790 = next;
    return 0;
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

std::uint16_t CgConnection::Impl::ReadProtocolModeDA3B22() noexcept
{
    return this->protocolModeDA3B22;
}

void CgConnection::Impl::WriteProtocolModeDA3B22(const std::uint16_t value) noexcept
{
    this->protocolModeDA3B22 = value;
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
    const std::uintptr_t socket, const void* const received)
{
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
    if (socket != connectionId || !this->active) return 0;
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
            handler(head, this->pointerTable58B614);
            if (socket != connectionId || !ioEnabled) return 0;
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
    script::LuaReceivePacket(head, this->pointerTable58B614);
    if (socket != connectionId || !ioEnabled) return 0;
    restoreArguments();

    // I为完整S32解析，T为同编号工作区char*；顺序与原108条比较路径一致。
    using I = std::int32_t;
    using T = char*;
    if (TryDispatchReply<world::HandlePlayerPositionReply_5093C0, I, I, I>(head, "QBt", socket)) return 0;
    if (TryDispatchReply<world::HandleMapRectangleCrcReply_509090, I, I, I, I, I, I, I, I, I>(head, "jmb", socket)) return 0;
    if (TryDispatchReply<world::HandleMapRectangleDataReply_509100, I, I, I, I, I, I, T>(head, "aBS", socket)) return 0;
    if (TryDispatchReply<world::HandleMapTransitionReply_50CF90, I, I>(head, "EZ", socket)) return 0;
    if (TryDispatchReply<world::Sub_50D080, I, I>(head, "OaHi", socket)) return 0;
    if (TryDispatchReply<world::HandleBattleEntryReply_50D120, I, I>(head, "dAgg", socket)) return 0;
    if (TryDispatchReply<world::HandleBattleResultsReply_50D500, T>(head, "Gn", socket)) return 0;
    if (TryDispatchReply<HandleDuelPointResultReply_50D700, T>(head, "hB", socket)) return 0;
    if (TryDispatchReply<world::HandleBattleStreamReply_50D3C0, T>(head, "LYIm", socket)) return 0;
    if (TryDispatchReply<world::HandleFullPlayerInventoryReply_50BFD0, T>(head, "ELoy", socket)) return 0;
    if (TryDispatchReply<world::HandlePlayerInventoryUpdatesReply_50C260, T>(head, "rU", socket)) return 0;
    if (TryDispatchReply<world::HandlePlayerWorldTimeReply_50C870, I, I, I, I>(head, "PudO", socket)) return 0;
    if (TryDispatchReply<world::HandleInventorySlotSwapReply_50C900, I, I>(head, "vWpn", socket)) return 0;
    if (TryDispatchReply<world::HandleSkillRecipesReply_50C580, I, T>(head, "xmgQ", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::HandlePlayerTimerReply_50C7A0, I, I>(head, "MTg", socket)) return 0;
    if (TryDispatchReply<world::detail::HandleContactMailReply_50DC70, I, T, I>(head, "Mhbt", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::HandleAlbumEntryMaskReply_50E5B0, I, T>(head, "Ilz", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::HandleAlbumEntryReply_50E5D0, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, T, T>(head, "dM", socket, 0x30000U)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<I, T>, I, T>(head, "Dn", socket, 0x4U)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<I>, I>(head, "IYo", socket)) return 0;
    if (TryDispatchReply<HandleMapActorTransitionReply_50DD80, I, I, I, I, I, I, I, T>(head, "lDuJ", socket, 0x100U)) return 0;
    if (TryDispatchReply<world::detail::HandleContactListReply_50D740, T>(head, "gnxd", socket)) return 0;
    if (TryDispatchReply<world::detail::HandleContactExtendedTextReply_50D8C0, T>(head, "PhdQ", socket)) return 0;
    if (TryDispatchReply<world::detail::HandleContactUpdateReply_50D9E0, I, T>(head, "vI", socket, 0x4U)) return 0;
    if (TryDispatchReply<HandleGuildMailResetReply_50E830>(head, "GC", socket)) return 0;
    if (TryDispatchReply<cg::world::HandleGuildInfoReply_50E850, T>(head, "aDn", socket)) return 0;
    if (TryDispatchReply<cg::world::HandleGuildRankDefinitionsReply_50EB40, T>(head, "znv", socket)) return 0;
    if (TryDispatchReply<cg::world::UpdateOrderedRecordBatch_50EC50, T>(head, "yuD", socket)) return 0;
    if (TryDispatchReply<cg::world::UpdateOrderedRecords_50EEB0, I, T>(head, "Kka", socket, 0x4U)) return 0;
    if (TryDispatchReply<HandleGuildMemberRemovalReply_50F130, I>(head, "oRut", socket)) return 0;
    if (TryDispatchReply<HandleGuildMailReply_50F190, I, T, I>(head, "Rll", socket, 0x4U)) return 0;
    if (TryDispatchReply<cg::world::HandleGuildRemovalReply_50F280, I>(head, "frQp", socket)) return 0;
    if (TryDispatchReply<world::detail::HandleProfileCategoryVersionsReply_50F3A0, T>(head, "HNP", socket)) return 0;
    if (TryDispatchReply<world::detail::HandlePrimaryProfileCategoriesReply_50F2A0, T>(head, "eZT", socket)) return 0;
    if (TryDispatchReply<world::detail::HandleSecondaryProfileCategoriesReply_50F8E0, T>(head, "IUn", socket)) return 0;
    if (TryDispatchReply<world::detail::HandlePlayerProfileReply_50F450, T, I, T, T, T, I, I, T, I, T, I, T, T>(head, "DpSY", socket, 0x1128U)) return 0;
    if (TryDispatchReply<world::detail::HandleMiniMailContactRemovalReply_50F430, T>(head, "cNB", socket)) return 0;
    if (TryDispatchReply<world::detail::HandleMiniMailReply_50F7F0, I, T, I>(head, "ow", socket, 0x4U)) return 0;
    if (TryDispatchReply<ui::WindowProfile_ReceiveReply, I, I, I, I, T>(head, "DyAG", socket, 0x20U)) return 0;
    if (TryDispatchReply<world::detail::HandleMiniMailConversationReply_50F9E0, T>(head, "KyN", socket)) return 0;
    if (TryDispatchReply<world::HandlePlayerChatReply_50CAE0, I, T, I, I>(head, "zdr", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::HandleSystemChatReply_50CE10, T>(head, "Zd", socket)) return 0;
    if (TryDispatchReply<world::HandlePlayerPrimaryStatsReply_509650, T>(head, "aPc", socket)) return 0;
    if (TryDispatchReply<world::HandlePlayerSecondaryStatsReply_509BA0, T>(head, "UU", socket)) return 0;
    if (TryDispatchReply<world::HandlePetPrimaryStatsReply_50A6B0, I, T>(head, "IsSy", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::HandlePetSecondaryStatsReply_50AC20, I, T>(head, "AU", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::HandlePartyActorVitalsReply_50AEF0, T>(head, "TNT", socket)) return 0;
    if (TryDispatchReply<world::HandleMapObjectListReply_50B380, T>(head, "nsY", socket)) return 0;
    if (TryDispatchReply<world::HandlePartyMemberReply_50A3E0, I, T>(head, "JJQj", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::HandlePlayerTitlesReply_50A300, T>(head, "Sc", socket)) return 0;
    if (TryDispatchReply<world::HandleWorldActorActionsReply_50BCD0, T>(head, "NI", socket)) return 0;
    if (TryDispatchReply<world::HandleWorldObjectRemovalReply_50BF20, T>(head, "uSB", socket)) return 0;
    if (TryDispatchReply<world::HandlePlayerProfessionReply_509E30, I, T>(head, "jv", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::HandlePlayerSkillsReply_509EB0, T>(head, "cNE", socket)) return 0;
    if (TryDispatchReply<world::HandlePlayerSkillTechniquesReply_50A120, I, T>(head, "Rspn", socket, 0x4U)) return 0;
    if (TryDispatchReply<world::HandlePetTechniquesReply_50B0D0, I, T>(head, "OBee", socket, 0x4U)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<T>, T>(head, "Yo", socket)) return 0;
    if (TryDispatchReply<world::HandlePlayerOptionsReply_50BFA0, I>(head, "SkBD", socket)) return 0;
    if (TryDispatchReply<Sub_50D3B0, I>(head, "sZWu", socket)) return 0;
    if (TryDispatchReply<world::HandlePartyMembershipReply_50D000, I, I>(head, "bNY", socket)) return 0;
    if (TryDispatchReply<ui::WindowSkill_ReceiveTextTargetReply, T>(head, "ysVc", socket)) return 0;
    if (TryDispatchReply<ui::WindowSkill_ReceiveAttributeTargetReply, T>(head, "ND", socket)) return 0;
    if (TryDispatchReply<ui::WindowItemUse_ReceiveTextTargetReply, T>(head, "LY", socket)) return 0;
    if (TryDispatchReply<ui::WindowItemUse_ReceiveAttributeTargetReply, T>(head, "nFV", socket)) return 0;
    if (TryDispatchReply<ui::WindowWorkResult_ReceiveReply, I, I, T>(head, "JgO", socket, 0x8U)) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveTargetReply, T>(head, "Cxo", socket)) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveStartReply, T, I>(head, "ms", socket)) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveCancelReply>(head, "NdM", socket)) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveItemReply, T>(head, "nUbJ", socket)) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveStateReply, I>(head, "hJq", socket)) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceivePetReply, I, T>(head, "RwK", socket, 0x4U)) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceivePetSkillReply, I, T>(head, "Yq", socket, 0x4U)) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveRemoteResetReply>(head, "xEg", socket)) return 0;
    if (TryDispatchReply<ui::WindowTrade_ReceiveCloseReply, I>(head, "We", socket)) return 0;
    if (TryDispatchReply<HandleFailureNoticeReply_50CF20, I, I, I, I>(head, "BHe", socket)) return 0;
    if (TryDispatchReply<HandleRemainingAttributePointsReply_50CF60, I>(head, "kg", socket)) return 0;
    if (TryDispatchReply<world::HandlePetAttributePointsReply_50CF70, I, I>(head, "sA", socket)) return 0;
    if (TryDispatchReply<world::HandlePlayerBattleRowReply_50BFC0, I>(head, "Ov", socket)) return 0;
    if (TryDispatchReply<ui::WindowDialog_ReceiveReply, I, I, I, I, T>(head, "BZ", socket, 0x20U)) return 0;
    if (TryDispatchReply<world::HandleDirectionalMapEffectsReply_50E1A0, I, I, T>(head, "zzd", socket, 0x8U)) return 0;
    if (TryDispatchReply<world::HandleNotificationSoundReply_50E210, I>(head, "TFCu", socket)) return 0;
    if (TryDispatchReply<HandleMapBgmChangeReply_50E310, I>(head, "wiB", socket)) return 0;
    if (TryDispatchReply<HandlePaletteTransitionReply_50E630, I, I>(head, "bJg", socket)) return 0;
    if (TryDispatchReply<world::HandleMapSceneEffectReply_50E650, I, I, I, I, I>(head, "PVfz", socket)) return 0;
    if (TryDispatchReply<HandleServerSoundPlayback_50E680, I, I, I>(head, "murD", socket)) return 0;
    if (TryDispatchReply<HandleSpatialSoundSourceReply_50E6C0, I, I, I>(head, "ftaN", socket)) return 0;
    if (TryDispatchReply<world::HandleMapNameReply_509550, T>(head, "TeD", socket)) return 0;
    if (TryDispatchReply<world::HandleMapChangeReply_5093F0, I, I, I, I, I, I, I, I, I, I, I, I>(head, "GmNM", socket)) return 0;
    if (TryDispatchReply<&Impl::HandleResponse_508800, I, T>(head, "whV", socket, 0x4U)) return 0;
    if (TryDispatchReply<login::HandleCharacterCreationResponse_508EE0, T, I, T>(head, "DZ", socket, 0x8U)) return 0;
    if (TryDispatchReply<login::HandleCharacterDeletionResponse_509020, T, T>(head, "oy", socket, 0x4U)) return 0;
    if (TryDispatchReply<login::HandleLoginResponse_508BC0, T, T>(head, "hPUd", socket, 0x4U)) return 0;
    if (TryDispatchReply<login::HandleCharacterListResponse_5089B0, I, T>(head, "Daj", socket, 0x4U)) return 0;
    if (TryDispatchReply<login::detail::HandleLogoutReply_508CF0, T, T>(head, "HMNy", socket, 0x4U)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<T>, T>(head, "cJzI", socket)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<I, I>, I, I>(head, "Bi", socket)) return 0;
    if (TryDispatchReply<IgnoreReplyArguments_509080<T>, T>(head, "LOh", socket)) return 0;
    if (TryDispatchReply<&Impl::HandleServerAddressChangeReply_50E6E0, T>(head, "lSZ", socket)) return 0;
    if (TryDispatchReply<character_select::detail::HandlePukVersionReply_50E770, I>(head, "PfM", socket)) return 0;
    if (TryDispatchReply<&Impl::HandleVersionUpgradeReply_50E780, I>(head, "ed", socket)) return 0;
    // COR10：追踪器已移除，旧tq配置仍按两个整数解码后忽略。
    if (TryDispatchReply<IgnoreReplyArguments_509080<I, I>, I, I>(head, "tq", socket)) return 0;
    if (TryDispatchReply<ui::WindowStall_ReceiveOpenReply, I>(head, "Tmrx", socket)) return 0;
    if (TryDispatchReply<ui::WindowStall_ReceiveCloseReply, I>(head, "KXhH", socket)) return 0;
    if (TryDispatchReply<ui::WindowStall_ReceiveBrowseReply, I, I, T, T, T, T>(head, "ft", socket, 0x68U)) return 0;
    if (TryDispatchReply<ui::WindowStall_ReceivePurchaseReply, I>(head, "Yx", socket)) return 0;
    static_cast<void>(TryDispatchReply<world::HandlePetInventoryUpdatesReply_510150, I, T>(head, "iKe", socket, 0x4U));
    static_cast<void>(TryDispatchReply<HandleInventorySlotSwapReply_510460, I, I, I, I>(head, "kiG", socket));
    static_cast<void>(TryDispatchReply<HandleItemDetailReply_510490, T>(head, "TgX", socket));
    return -1;
}

void CgConnection::Impl::HandleServerAddressChangeReply_50E6E0(const std::uintptr_t, const char* const text)
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

std::int32_t CgConnection::Impl::HandleVersionUpgradeReply_50E780(const std::uintptr_t, const std::int32_t value)
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
    const SendCallback& callback,
    const std::uint32_t bufferLength,
    const std::uintptr_t)
{
    this->sendCallback = &DefaultNetworkCallback;
    if (callback != nullptr) {
        this->sendCallback = callback;
    }

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
void CgConnection::Impl::HandleResponse_508800(
    const std::uintptr_t,
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
    auto snapshot = transport.PollConnection(connectionId);
    if (snapshot.state == platform::ConnectionState::PolicyReady) {
        platform::ApplyServerAddressPolicyResult(snapshot.policyAddress);
        transport.ContinueAfterPolicy(snapshot.id);
        snapshot = transport.PollConnection(snapshot.id);
    }
    return snapshot;
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
std::int32_t CgConnection::Impl::SendPacket(
    const std::uintptr_t connection,
    const void* const data,
    const std::uint32_t size)
{
    if (!ioEnabled) {
        return -100;
    }
    const auto id = connectionId;
    if (id == 0 || (connection != 0 && connection != id)) return -100;

    // L04：回调可重入发送；持有字节快照，避免共享格式化缓冲被覆盖。
    const std::string packet(static_cast<const char*>(data), size);
    script::RecordAutoBattleAction(packet);
    script::LuaSendPacket(packet);
    if (id != connectionId || !ioEnabled) return -100;
    for (auto& [header, handler] : this->sendHandlers) {
        if (packet.starts_with(header)) {
            handler(header.c_str(), packet.data() + header.size(),
                static_cast<std::int32_t>(packet.size() - header.size()));
            break;
        }
    }

    // N03：成功入队的JFVf替换待发队列，六字节前导不递归触发发送回调。
    constexpr std::string_view loginPrelude = "XCNY \n";
    const bool startsLogin = packet.starts_with("JFVf");
    if (id != connectionId || !ioEnabled) return -100;
    transport.SetIoEnabled(id, true);
    const auto queued = transport.TryQueueSend(id,
        {reinterpret_cast<const std::uint8_t*>(packet.data()), packet.size()}, startsLogin,
        startsLogin ? loginPrelude : std::string_view {});
    if (queued != platform::NetworkQueueResult::Accepted)
        return queued == platform::NetworkQueueResult::QueueFull ? -1 : -100;
    // 成功入队后记录明文；不把队列日志当作socket发送成功，避免按TCP分片重复打印。
    if (startsLogin) {
        m_logw(L"[network][send queued][%zu] %.*hs", loginPrelude.size(),
            static_cast<int>(loginPrelude.size()), loginPrelude.data());
    }
    m_logw(L"[network][send queued][%u] %.*hs%hs", size, static_cast<int>(size),
        packet.data(), packet.ends_with('\n') ? "" : "\n");
    return 0;
}

std::int32_t CgConnection::Impl::Initialize()
{
    if (!this->gameplayHandlersRegistered) {
        RegisterReceive("BtWt", [](const char*, char** const arguments) {
            const std::int32_t wait = ParseProtocolInteger_517190(arguments[1]);
            world::gBattleCommandWaitMs = std::bit_cast<std::uint32_t>(wait);
            world::gBattleShowEnemyHpBars = ParseProtocolInteger_517190(arguments[2]) != 0;
            m_log_iw(L"Set battleCommandWait = %d\n", wait);
        });
        RegisterReceive("SpDt", [](const char*, char** const arguments) {
            world::gChatMinimumIntervalMs = std::bit_cast<std::uint32_t>(
                ParseProtocolInteger_517190(arguments[1]));
        });
        RegisterReceive("INVCF", [](const char*, char** const arguments) {
            const std::int32_t bagPages = ParseProtocolInteger_517190(arguments[1]);
            const std::int32_t bankPages = ParseProtocolInteger_517190(arguments[2]);
            const std::int32_t bagPage = ParseProtocolInteger_517190(arguments[3]);
            ui::WindowRuntime_SetInventoryPages(bagPages, bankPages, bagPage);
        });
        this->gameplayHandlersRegistered = true;
    }
    if (this->active) Shutdown();
    int transportError = 0;
    if (!transport.Open(transportError)) {
        this->errorD9F2CC = transportError;
        return 0;
    }

    if (AllocateNetworkWorkspace_516B60(
        [this](std::uintptr_t id, const void* data, std::uint32_t size) { return SendPacket(id, data, size); },
        0x10000U,
        connectionId) != 0) {
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

CgConnection::CgConnection() : impl_(std::make_unique<Impl>()) {}
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
std::int32_t CgConnection::SendPacket(platform::ConnectionId id, const void* data, std::uint32_t size)
{ return impl_->SendPacket(id, data, size); }
ConnectionPumpResult CgConnection::Pump() { return impl_->Pump(); }
void CgConnection::RegisterReceive(const char* header, ProtocolReceiveHandler handler)
{ impl_->RegisterReceive(header, std::move(handler)); }
void CgConnection::RegisterSend(const char* header, ProtocolSendHandler handler)
{ impl_->sendHandlers.emplace_back(header, std::move(handler)); }
void CgConnection::Close() { impl_->Close(); }
void CgConnection::StopIo() { impl_->StopIo(); }

// 只供旧组包实现同步借用；不作为CgConnection公开API。
char* detail::ConnectionLegacyAccess::OutputBuffer(CgConnection& owner) noexcept
{ return reinterpret_cast<char*>(owner.impl_->sendMessageBuffer); }
std::uint32_t detail::ConnectionLegacyAccess::BufferCapacity(CgConnection& owner) noexcept
{ return owner.impl_->workspaceCapacity; }

std::int32_t detail::ConnectionLegacyAccess::AllocateProtocolBuffers_516DA0(CgConnection& owner,
    const std::uint32_t bufferLength)
{
    return owner.impl_->AllocateProtocolBuffers_516DA0(bufferLength);
}

std::uint8_t* detail::ConnectionLegacyAccess::FormatProtocolInteger_517670(CgConnection& owner, const std::int32_t value)
{
    return owner.impl_->FormatProtocolInteger_517670(value);
}

std::uint8_t* detail::ConnectionLegacyAccess::FormatProtocolIntegerToken_517120(CgConnection& owner, const std::int32_t value)
{
    return owner.impl_->FormatProtocolIntegerToken_517120(value);
}

std::uint8_t* detail::ConnectionLegacyAccess::FormatProtocolTextToken_517160(CgConnection& owner, const std::wstring_view text)
{
    return owner.impl_->FormatProtocolTextToken_517160(text);
}

std::uint8_t* detail::ConnectionLegacyAccess::EscapeProtocolText_5171F0(CgConnection& owner, const std::wstring_view text)
{
    return owner.impl_->EscapeProtocolText_5171F0(text);
}

std::uint8_t* detail::ConnectionLegacyAccess::UnescapeProtocolText_5172D0(CgConnection& owner, const char* const text)
{
    return owner.impl_->UnescapeProtocolText_5172D0(text);
}

std::uint8_t* detail::ConnectionLegacyAccess::DecodeOptionalProtocolText_5171B0(CgConnection& owner, const char* const text)
{
    return owner.impl_->DecodeOptionalProtocolText_5171B0(text);
}

std::int32_t detail::ConnectionLegacyAccess::SendProtocolBuffer_517480(CgConnection& owner,
    const std::uintptr_t socket,
    char* const buffer)
{
    return owner.impl_->SendProtocolBuffer_517480(socket, buffer);
}

std::int32_t detail::ConnectionLegacyAccess::SendConnectionProbe_512C10(CgConnection& owner,
    const std::uintptr_t socket,
    const std::wstring_view text)
{
    return owner.impl_->SendConnectionProbe_512C10(socket, text);
}

std::int32_t detail::ConnectionLegacyAccess::AppendReceiveQueue_5081E0(CgConnection& owner,
    const void* const source,
    const std::uint32_t size)
{
    return owner.impl_->AppendReceiveQueue_5081E0(source, size);
}

std::int32_t detail::ConnectionLegacyAccess::ConsumeReceiveQueue_508230(CgConnection& owner, const std::int32_t size)
{
    return owner.impl_->ConsumeReceiveQueue_508230(size);
}

std::uint16_t detail::ConnectionLegacyAccess::ReadProtocolModeDA3B22(CgConnection& owner) noexcept
{
    return owner.impl_->ReadProtocolModeDA3B22();
}

void detail::ConnectionLegacyAccess::WriteProtocolModeDA3B22(CgConnection& owner, const std::uint16_t value) noexcept
{
    return owner.impl_->WriteProtocolModeDA3B22(value);
}

void detail::ConnectionLegacyAccess::SplitReceivedBuffer_5173B0(CgConnection& owner, char* const buffer)
{
    return owner.impl_->SplitReceivedBuffer_5173B0(buffer);
}

std::int32_t detail::ConnectionLegacyAccess::NetworkDispatch_512E40(CgConnection& owner,
    const std::uintptr_t socket, const void* const received)
{
    return owner.impl_->NetworkDispatch_512E40(socket, received);
}

void detail::ConnectionLegacyAccess::HandleServerAddressChangeReply_50E6E0(CgConnection& owner, const std::uintptr_t, const char* const text)
{
    return owner.impl_->HandleServerAddressChangeReply_50E6E0(0, text);
}

std::int32_t detail::ConnectionLegacyAccess::HandleVersionUpgradeReply_50E780(CgConnection& owner, const std::uintptr_t, const std::int32_t value)
{
    return owner.impl_->HandleVersionUpgradeReply_50E780(0, value);
}

std::int32_t detail::ConnectionLegacyAccess::AllocateNetworkWorkspace_516B60(CgConnection& owner,
    const NetworkCallback callback,
    const std::uint32_t bufferLength,
    const std::uintptr_t)
{
    return owner.impl_->AllocateNetworkWorkspace_516B60(callback, bufferLength, 0);
}

} // namespace cg::network
