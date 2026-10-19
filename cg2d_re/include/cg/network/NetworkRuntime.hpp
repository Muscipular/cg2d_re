#pragma once

#include <cstdint>
#include <functional>
#include <string_view>

namespace cg::network {

// N01：接收参数第0项为头、末项为null；正文借用到回调结束。
using ProtocolReceiveHandler = std::function<void(const char* header, char** arguments)>;
// 发送正文位于注册前缀之后，长度包含原LF；不拥有传入字节。
using ProtocolSendHandler = std::function<void(
    const char* header, const char* data, std::int32_t length)>;

// 按注册顺序执行首个匹配项；接收匹配完整头，发送匹配前缀。
// 注册复制消息头并持有回调，重连保留；同网络线程调用。
void RegisterProtocolReceive(const char* header, ProtocolReceiveHandler handler);
void RegisterProtocolSend(const char* header, ProtocolSendHandler handler);
// L04：Lua发送沿用唯一明文队列和已有发送观察回调。
std::int32_t SendLuaPacket(const char* data, std::uint32_t length);

using NetworkCallback = std::int32_t (*)(
    std::uintptr_t socket,
    const void* data,
    std::uint32_t size);

// 原 sub_516DA0@[0x516DA0,0x517038)：分配并清零九项协议缓冲。
std::int32_t AllocateProtocolBuffers_516DA0(std::uint32_t bufferLength);

// 原 sub_517080@[0x517080,0x5170C1)：按有符号 W32 上限复制协议字节并补 NUL。
void CopyProtocolBytes_517080(
    std::uint8_t* destination,
    const std::uint8_t* source,
    std::int32_t capacity);

// 原 [0x517040,0x517080)：先清输出 W32，再按参数首指针选择复制来源。
void ReadProtocolCommandHeader_517040(
    std::uint32_t* output,
    std::uint8_t* destination,
    std::int32_t capacity,
    char* const* protocolArgs);

// 原 sub_517670@[0x517670,0x51768A)：格式化有符号 W32 到共享协议缓冲。
std::uint8_t* FormatProtocolInteger_517670(std::int32_t value);

// 原 sub_517690@[0x517690,0x5176AC)：复制协议字节并返回原目标指针。
std::uint8_t* CopyProtocolBytesAndReturn_517690(
    std::uint8_t* destination,
    std::int32_t capacity,
    const std::uint8_t* source);

// 原 sub_5170D0@[0x5170D0,0x51711C)：在有符号 W32 容量内追加协议文本。
std::int32_t AppendProtocolText_5170D0(
    char* destination,
    const char* source,
    std::int32_t capacity);

// 原 sub_517120@[0x517120,0x517160)：格式化协议整数并追加 ASCII 空格。
std::uint8_t* FormatProtocolIntegerToken_517120(std::int32_t value);

// 原 sub_517160@[0x517160,0x517186)：转义协议文本并追加 ASCII 空格。
std::uint8_t* FormatProtocolTextToken_517160(std::wstring_view text);

// 原 sub_517190@[0x517190,0x5171A3)：空指针返回 0，否则委派协议整数解析。
std::int32_t ParseProtocolInteger_517190(const char* text);

// 原 sub_52CEA0@[0x52CEA0,0x52CECB)：跳过高位字节对，定位下一段。
const char* FindNextDelimitedField_52CEA0(
    const char* source, char delimiter);

// 原 sub_52CF30@[0x52CF30,0x52CF98)：复制字段并补 NUL；遇分隔符返回 0。
std::int32_t CopyDelimitedField_52CF30(
    const char* source, char delimiter, std::int32_t capacity,
    char* destination);

// 原 sub_52CED0@[0x52CED0,0x52CF27)：按序号选字段并保留复制返回。
std::int32_t ExtractDelimitedField_52CED0(
    const char* source, char delimiter, std::int32_t ordinal,
    std::int32_t capacity, char* destination);

// 原 sub_52CFA0@[0x52CFA0,0x52CFED)：取至多 127 字节字段，空串返回 -1。
std::int32_t ParseDelimitedInteger_52CFA0(
    const char* source, char delimiter, std::int32_t ordinal);

// 原 sub_52CFF0@[0x52CFF0,0x52D04F)：空字段返回 2 且不写整数输出。
std::int32_t ParseDelimitedIntegerStatus_52CFF0(
    const char* source, char delimiter, std::int32_t ordinal,
    std::int32_t* output);

// 原 base62Decode@[0x52D050,0x52D0C2)：逐字节先乘 62，负号可在任意位置。
std::int32_t Base62Decode_52D050(const char* text);

// 原 sub_52D0D0@[0x52D0D0,0x52D11D)：提取字段后解码 base62。
std::int32_t ParseDelimitedBase62_52D0D0(
    const char* source, char delimiter, std::int32_t ordinal);

// 原 sub_5171F0@[0x5171F0,0x5172CE)：向共享缓冲逐字节转义协议文本。
// UWF01：参数为UTF-16正文，返回原协议GBK文本token，容量不足/不可编码抛GbkStorageError。
std::uint8_t* EscapeProtocolText_5171F0(std::wstring_view text);

// 原 sub_5172D0@[0x5172D0,0x5173A6)：向共享缓冲逐字节反转义协议文本。
std::uint8_t* UnescapeProtocolText_5172D0(const char* text);

// 原 [0x5171B0,0x5171E2)：非空反转义，空参复制共享 593850 文本。
std::uint8_t* DecodeOptionalProtocolText_5171B0(const char* text);

// 原 sub_517480@[0x517480,0x517502)：记录、观察并发送协议行缓冲。
std::int32_t SendProtocolBuffer_517480(
    std::uintptr_t socket,
    char* buffer);

// 原 sub_512C10@[0x512C10,0x512C58)：构造 Or 文本命令并透传发送返回。
std::int32_t SendConnectionProbe_512C10(std::uintptr_t socket, std::wstring_view text);

// 原 sub_512DF0@[0x512DF0,0x512E38)：构造 CCL 计时命令并透传发送返回。
std::int32_t SendLoginTimestamp_512DF0(std::uintptr_t socket, std::uint32_t tickBits);

// 原 sub_512D30@[0x512D30,0x512D9B)：依序追加版本整数与文本，发送 Xoee。
std::int32_t SendVersionUpgradeRequest_512D30(
    std::uintptr_t socket, std::int32_t version, std::wstring_view payload);

// 原 sub_512DA0@[0x512DA0,0x512DE8)：追加单个整数并发送 IPy。
std::int32_t SendCraftDuration_512DA0(std::uintptr_t socket, std::int32_t value);

// 原 sub_512B00@[0x512B00,0x512B8E)：依次追加三项整数并发送 emRV 命令。
std::int32_t SendCharacterLoginRequest_512B00(
    std::uintptr_t socket,
    std::int32_t field4C,
    std::int32_t field50,
    std::int32_t field18);

// 原 sub_512AB0@[0x512AB0,0x512AF8)：追加登记号并发送 Godf 命令。
std::int32_t SendCharacterDeletionRequest_512AB0(std::uintptr_t socket, std::int32_t registNumber);

// 原 sub_5128C0@[0x5128C0,0x512AAC)：依序追加十二项整数与第二项文本，发送 aSEl。
std::int32_t Sub_5128C0(
    std::uintptr_t socket,
    std::int32_t field1,
    std::wstring_view field2,
    std::int32_t field3,
    std::int32_t field4,
    std::int32_t field5,
    std::int32_t field6,
    std::int32_t field7,
    std::int32_t field8,
    std::int32_t field9,
    std::int32_t field10,
    std::int32_t field11,
    std::int32_t field12,
    std::int32_t field13);

// 原 sub_510630@[0x510630,0x510704)：五整数依序追加并发送 pg。
std::int32_t Sub_510630(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::int32_t third,
    std::int32_t fourth,
    std::int32_t fifth);

// 原 sub_510570@[0x510570,0x510621)：两整数、文本、末整数依序追加并发送 zA。
std::int32_t Sub_510570(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::wstring_view text,
    std::int32_t fourth);

// 原 sub_510710@[0x510710,0x510807)：六整数依序追加并发送 UUN。
std::int32_t Sub_510710(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::int32_t third,
    std::int32_t fourth,
    std::int32_t fifth,
    std::int32_t sixth);

// 原510AC0：iVfo四整数，逐项采容量/格式化/追加，透传发送返回。
std::int32_t SendItemUseRequest_510AC0(
    std::uintptr_t socket, std::int32_t x, std::int32_t y,
    std::int32_t item, std::int32_t target);

// NFI01：fIM三整数/普通空格，原time/RNG消费与发送返回保持；原17替换不接现明文格式。
// 原函数：0x00510B80。
std::int32_t Sub_510B80(
    std::uintptr_t socket, std::int32_t x, std::int32_t y, std::int32_t direction);

// 原 sub_510810@[0x510810,0x51087B)：两整数依序追加并发送 KeOX。
std::int32_t SendPlayerCoordinates_510810(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second);

// 原快捷交互请求：两整数独立采容量/格式化/追加，透传真实发送返回。
// 原函数：0x00510880。
std::int32_t Sub_510880(
    std::uintptr_t socket, std::int32_t first, std::int32_t second); // ik
// 原函数：0x00510940。
std::int32_t Sub_510940(
    std::uintptr_t socket, std::int32_t first, std::int32_t second); // IMv
// 原函数：0x005111E0。
std::int32_t Sub_5111E0(
    std::uintptr_t socket, std::int32_t first, std::int32_t second); // vcAz

// NLD01／原512800：摘要、口令、账号、适配器四文本原序发送JFVf，末发送S32透传。
std::int32_t Sub_512800(
    std::uintptr_t socket,
    std::wstring_view executableDigest,
    std::wstring_view password,
    std::wstring_view account,
    std::wstring_view adapterId);

// 原 sub_511030@[0x511030,0x5110AE)：发送数量及其后的 W32 列表 kza。
std::int32_t Sub_511030(
    std::uintptr_t socket, const std::int32_t* values,
    std::int32_t count);

// 原511190：FPkR联系人删除请求，整数保持S32，返回实际发送结果。
std::int32_t Sub_511190(std::uintptr_t socket, std::int32_t value);

// 原511250：Ufm公会文本变更，容量/输出各自重取，返回真实发送结果。
std::int32_t Sub_511250(
    std::uintptr_t socket, std::int32_t kind, std::wstring_view text);

// 原 sub_5112C0@[0x5112C0,0x51134E)：构造 GY、追加两整数与原字节文本并发送。
std::int32_t Sub_5112C0(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::wstring_view text);

// 原 sub_511350@[0x511350,0x5113BB)：追加两个 W32 整数并发送 uHb。
std::int32_t Sub_511350(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second);

// 原 sub_5113C0@[0x5113C0,0x51142B)：追加两个 W32 整数并发送 npzi。
std::int32_t Sub_5113C0(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second);

// 原 sub_511430@[0x511430,0x511478)：追加一个 W32 整数并发送 dbQC。
std::int32_t Sub_511430(std::uintptr_t socket, std::int32_t value);

// 原510FA0：CV按整数/文本/整数分段采容量并发送，不共用uId命令头。
std::int32_t Sub_510FA0(
    std::uintptr_t socket, std::int32_t first, std::wstring_view text, std::int32_t third);
// 原 sub_511480@[0x511480,0x51150E)：依次追加整数、原字节文本和整数并发送 uId。
std::int32_t Sub_511480(
    std::uintptr_t socket,
    std::int32_t first,
    std::wstring_view text,
    std::int32_t third);

// 原5110B0：普通宠物邮件rMsv，三整数/文本/末整数各自采容量并发送。
std::int32_t Sub_5110B0(
    std::uintptr_t socket, std::int32_t first, std::int32_t second,
    std::int32_t third, std::wstring_view text, std::int32_t fifth);
// 原 sub_511510@[0x511510,0x5115E4)：三整数、原字节文本、末整数发送 PfqV。
std::int32_t Sub_511510(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::int32_t third,
    std::wstring_view text,
    std::int32_t fifth);

// 原 sub_5115F0@[0x5115F0,0x511638)：追加 W32 整数并发送 HoW。
std::int32_t Sub_5115F0(std::uintptr_t socket, std::int32_t value);

// 原 sub_511640@[0x511640,0x51175A)：七项整数/文本按原序发送 Ot。
std::int32_t Sub_511640(
    std::uintptr_t socket,
    std::int32_t field1,
    std::wstring_view text2,
    std::int32_t field3,
    std::wstring_view text4,
    std::int32_t field5,
    std::wstring_view text6,
    std::wstring_view text7);

// 原 sub_511760@[0x511760,0x5117CB)：构造 TLKH、追加两个整数并发送。
std::int32_t Sub_511760(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second);

// 原 sub_5117D0@[0x5117D0,0x51183B)：构造 wD、追加整数与原字节文本并发送。
std::int32_t Sub_5117D0(
    std::uintptr_t socket,
    std::int32_t value,
    std::wstring_view text);

// 原 sub_511840@[0x511840,0x5118AB)：构造 GH、追加原字节文本与整数并发送。
std::int32_t Sub_511840(
    std::uintptr_t socket,
    std::wstring_view text,
    std::int32_t value);

// 原 sub_5118B0@[0x5118B0,0x5118F8)：构造 lz、追加单整数并发送。
std::int32_t Sub_5118B0(std::uintptr_t socket, std::int32_t value);

// 原 sub_511900@[0x511900,0x5119F7)：组装 uSr 的两整数、文本、三整数并发送。
std::int32_t Sub_511900(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::wstring_view text,
    std::int32_t fourth,
    std::int32_t fifth,
    std::int32_t sixth); // CTS01：见聊天发送完整合同。

// 原 sub_511A00@[0x511A00,0x511A48)：构造 dcMv、追加一个整数并发送。
std::int32_t Sub_511A00(std::uintptr_t socket, std::int32_t value);

// 原 sub_511A50@[0x511A50,0x511ADE)：构造 zn、追加三个整数并发送。
std::int32_t Sub_511A50(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::int32_t third);

// 原 sub_511AE0@[0x511AE0,0x511BB4)：构造 LKQy、追加五个整数并发送。
std::int32_t Sub_511AE0(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::int32_t third,
    std::int32_t fourth,
    std::int32_t fifth);

// 原 sub_511BC0@[0x511BC0,0x511C2B)：构造 Ukuy、追加两个整数并发送。
std::int32_t Sub_511BC0(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second);

// 原 sub_511C30@[0x511C30,0x511C78)：构造 iET、追加一个整数并发送。
std::int32_t Sub_511C30(std::uintptr_t socket, std::int32_t value);

// 原 sub_511C80@[0x511C80,0x511CC8)：构造 McVa、追加一个整数并发送。
std::int32_t Sub_511C80(std::uintptr_t socket, std::int32_t value);

// 原 sub_511D10@[0x511D10,0x511D58)：构造 mjCv、追加一个整数并发送。
std::int32_t Sub_511D10(std::uintptr_t socket, std::int32_t value);

// 原 sub_511D60@[0x511D60,0x511DCB)：构造 BG、追加两个整数并发送。
std::int32_t Sub_511D60(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second);

// 原 sub_511DD0@[0x511DD0,0x511E5E)：构造 MIS、追加三个整数并发送。
std::int32_t Sub_511DD0(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::int32_t third);

// 原 sub_511E60@[0x511E60,0x511EEE)：构造 klF、追加三个整数并发送。
std::int32_t Sub_511E60(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::int32_t third);

// 原 sub_511EF0@[0x511EF0,0x511FA1)：构造 sM、追加三个整数及文本并发送。
std::int32_t Sub_511EF0(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::int32_t third,
    std::wstring_view text);

// 原 sub_511FF0@[0x511FF0,0x512038)：构造 QeuC、追加选择编号并发送。
std::int32_t Sub_511FF0(std::uintptr_t socket, std::int32_t index);

// 原 sub_512040@[0x512040,0x5120CE)：构造 aiAb、追加两段文本及整数并发送。
std::int32_t Sub_512040(
    std::uintptr_t socket,
    std::wstring_view first,
    std::wstring_view second,
    std::int32_t third);

// 原 sub_5120D0@[0x5120D0,0x51215E)：构造 ylGN、追加三段文本并发送。
std::int32_t Sub_5120D0(
    std::uintptr_t socket,
    std::wstring_view first,
    std::wstring_view second,
    std::wstring_view third);

// 原 sub_512160@[0x512160,0x512185)：构造 JMh 并直接发送。
std::int32_t Sub_512160(std::uintptr_t socket);

// 原 sub_512190@[0x512190,0x5121D8)：构造 ElVN、追加一个整数并发送。
std::int32_t Sub_512190(std::uintptr_t socket, std::int32_t value);

// 原 sub_5121E0@[0x5121E0,0x512228)：构造 wAo、追加一个整数并发送。
std::int32_t Sub_5121E0(std::uintptr_t socket, std::int32_t value);

// 原 sub_512270@[0x512270,0x5122B8)：构造 OU、追加一个整数并发送。
std::int32_t Sub_512270(std::uintptr_t socket, std::int32_t value);

// 原 sub_5122C0@[0x5122C0,0x512308)：构造 zGp、追加一个整数并发送。
std::int32_t Sub_5122C0(std::uintptr_t socket, std::int32_t value);

// 原 sub_512310@[0x512310,0x512358)：构造 tj、追加一个整数并发送。
std::int32_t Sub_512310(std::uintptr_t socket, std::int32_t value);

// 原 sub_512360@[0x512360,0x5123A8)：构造 ACwB、追加原字节文本并发送。
std::int32_t Sub_512360(std::uintptr_t socket, std::wstring_view text);

// 原 sub_5123B0@[0x5123B0,0x5123F8)：构造 IHw、追加一个整数并发送。
std::int32_t Sub_5123B0(std::uintptr_t socket, std::int32_t value);

// 原 sub_512400@[0x512400,0x51246B)：构造 kjSK、追加两个整数并发送。
std::int32_t Sub_512400(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second);

// 原 sub_512470@[0x512470,0x5124DB)：构造 Noeb、追加两个整数并发送。
std::int32_t Sub_512470(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second);

// 原 sub_5124E0@[0x5124E0,0x51256E)：构造 HJQp、追加三个整数并发送。
std::int32_t Sub_5124E0(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::int32_t third);

// 原 sub_5125B0@[0x5125B0,0x51261B)：构造 ycX、追加整数及原字节文本并发送。
std::int32_t Sub_5125B0(
    std::uintptr_t socket, std::int32_t value, std::wstring_view text);

// 原 sub_512620@[0x512620,0x512717)：构造 xD、追加五整数与原字节文本并发送。
std::int32_t Sub_512620(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::int32_t third,
    std::int32_t fourth,
    std::int32_t fifth,
    std::wstring_view text); // SDM01：xD五整数+文本，六容量/输出独立取样，真实S32结果透传。

// 原 [0x510F50,0x510F98)：构造fS、追加一个整数，技能回复使用共享连接调用。
std::int32_t Sub_510F50(std::uintptr_t socket, std::int32_t value);

// 原[510E30,510EBE)：FvWm放地请求，三轮各自读取容量与共享缓冲。
std::int32_t Sub_510E30(
    std::uintptr_t socket, std::int32_t x, std::int32_t y, std::int32_t pet);

// 原[510EC0,510F4E)：构造yi，依次追加来源槽号、目标槽号及数量并发送。
std::int32_t Sub_510EC0(
    std::uintptr_t socket, std::int32_t source, std::int32_t target, std::int32_t amount);

// 原510DA0：woz丢钱请求，按X/Y/数量逐字段追加并透传发送返回。
std::int32_t Sub_510DA0(
    std::uintptr_t socket, std::int32_t x, std::int32_t y, std::int32_t amount); // MDR01：woz三个S32/三容量和输出取样，真实S32发送结果透传。

// 原5109B0：Rg战斗请求，转义一项原字节文本并透传发送结果。
std::int32_t SendBattlePacket_5109B0(std::uintptr_t socket, std::wstring_view text);

// 原516D70：构造无载荷iuE后重取共享缓冲发送，透传发送返回。
std::int32_t SendIuEPacket_516D70(std::uintptr_t socket);

// 原[510CE0,510D91)：构造QpfE，依次追加X/Y/组号/物品槽号并发送。
std::int32_t Sub_510CE0(
    std::uintptr_t socket, std::int32_t x, std::int32_t y,
    std::int32_t group, std::int32_t index); // IMT01：QpfE四原字段/独立容量与buffer重读，当前LF明文，S32透传；全owner/互通待。

// 原510A00：Ak物品使用，依次追加X/Y/物品槽号/组号并透传发送结果。
std::int32_t Sub_510A00(
    std::uintptr_t socket, std::int32_t x, std::int32_t y,
    std::int32_t index, std::int32_t group);

// 原516C90：Cpa跨组物品移动，五个数值保持S32及原字段顺序。
std::int32_t Sub_516C90(
    std::uintptr_t socket, std::int32_t sourceGroup, std::int32_t source,
    std::int32_t targetGroup, std::int32_t target, std::int32_t amount);

// 原[5108F0,510938)：构造eL并追加一个整数，透传共享协议缓冲发送返回。
std::int32_t Sub_5108F0(std::uintptr_t socket, std::int32_t value);

// 原 sub_512720@[0x512720,0x5127AE)：构造 cZt、追加三个整数并发送。
std::int32_t Sub_512720(
    std::uintptr_t socket,
    std::int32_t first,
    std::int32_t second,
    std::int32_t third);

// 原[5127B0,5127F8)：rh存放分页请求，追加整数后透传发送结果。
std::int32_t Sub_5127B0(std::uintptr_t socket, std::int32_t value);

// 原 [0x512B90,0x512BCC)：Gp 角色列表请求，追加共享文本并发送。
std::int32_t SendCharacterListRequest_512B90(std::uintptr_t socket);

// 原 [0x512BD0,0x512C0C)：WA 登出请求，追加共享文本并发送。
std::int32_t SendLogoutRequest_512BD0(std::uintptr_t socket);

// 原 [0x512C60,0x512C9C)：DE 保活发送，追加共享 593850 原字节文本。
std::int32_t SendKeepAliveRequest_512C60(std::uintptr_t socket);

// 原五个无额外参数的请求叶；各自构造命令、追加同一 593850 文本并透传发送返回。
// 原函数：0x00511CD0。
std::int32_t RequestItemTargetAttributes_511CD0(std::uintptr_t socket); // as
// 原函数：0x00511FB0。
std::int32_t SendTradeRequest_511FB0(std::uintptr_t socket); // ixH
// 原函数：0x00512230。
std::int32_t WithdrawTradeConfirmation_512230(std::uintptr_t socket); // suI
// 原函数：0x00512570。
std::int32_t ToggleBattleRowRequest_512570(std::uintptr_t socket); // ih
// 原函数：0x00512CF0。
std::int32_t RequestLogoutToTown_512CF0(std::uintptr_t socket); // lO

// 原 sub_516B60@0x516B60：安装回调并分配 18 项网络工作区。
std::int32_t AllocateNetworkWorkspace_516B60(
    NetworkCallback callback,
    std::uint32_t bufferLength,
    std::uintptr_t unusedSocket);

// 原 sub_508140@0x508140：启动 Winsock 并创建网络运行期工作区。
std::int32_t InitializeNetworkRuntime_508140();

// 原 NetworkLoop@[0x507DF0,0x50813A)：主循环每帧调用的网络收发与派发入口。
std::uint8_t NetworkLoop_507DF0();

// 原 0x5081E0：向共享接收区追加字节，成功后发布接收长度。
std::int32_t AppendReceiveQueue_5081E0(
    const void* source, std::uint32_t size);

// 原 0x508230/0x508280：先发布剩余长度，再将队列前移。
std::int32_t ConsumeReceiveQueue_508230(std::int32_t size);
// 原函数：0x00508280。
std::int32_t ConsumeSendQueue_508280(std::int32_t size);

// 只读原 W8 0xD9F2C8 活动门禁；状态仍由网络运行期唯一持有。
std::uint8_t ReadNetworkRuntimeActive() noexcept;

// 原 0xD9F2CC：507DF0 写、455C90 条件读取的唯一错误码槽。
std::int32_t ReadNetworkErrorD9F2CC() noexcept;

// 原 0xDA3B22 的字段级访问；不公开网络运行期完整状态。
std::uint16_t ReadProtocolModeDA3B22() noexcept;
void WriteProtocolModeDA3B22(std::uint16_t value) noexcept;

// 原 sub_5081A0@0x5081A0：关闭 Winsock 并释放网络运行期工作区。
void ShutdownNetworkRuntime_5081A0();

} // namespace cg::network
