#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string_view>
#include <string>

namespace cg {

// 原 nullsub_1@[0x509080,0x509081)：唯一指令为 retn，不读取参数且不定义返回值。
void NullSub_509080() noexcept; // UTE01：原单RET/参数未读，真实空操作；机器残值ABI待。

} // namespace cg

namespace cg::input {

inline constexpr std::size_t InputTextCapacity = 0x120;

// 原0x15C值布局只用于明确GBK叠加存储，原成员偏移保留。
struct InputFields {
    std::int32_t x; // 原成员偏移：+0x120。
    std::int32_t y; // 原成员偏移：+0x124。
    std::int32_t caretByteOffset; // 原成员偏移：+0x128。
    std::int32_t totalByteLength; // 原成员偏移：+0x12C。
    std::int32_t field130; // 原成员偏移：+0x130。
    std::int32_t field134; // 原成员偏移：+0x134。
    std::uint8_t field138[4]; // 原成员偏移：+0x138。
    std::int32_t field13C; // 原成员偏移：+0x13C。
    std::int32_t field140; // 原成员偏移：+0x140。
    std::int32_t maxLine; // 原成员偏移：+0x144。
    std::int32_t maxCharPerLine; // 原成员偏移：+0x148。
    std::int32_t field14C; // 原成员偏移：+0x14C。
    std::int32_t encrypt; // 原成员偏移：+0x150。
    std::int32_t field154; // 原成员偏移：+0x154。
    std::int32_t field158; // 原成员偏移：+0x158。
};
struct LegacyInputStorage {
    char text[InputTextCapacity]; // 原+0x00，GBK，容量含NUL。
    InputFields fields;
};
static_assert(offsetof(LegacyInputStorage, fields) == 0x120);
static_assert(sizeof(LegacyInputStorage) == 0x15C);
static_assert(offsetof(InputFields, caretByteOffset) + 0x120 == 0x128);
static_assert(offsetof(InputFields, field158) + 0x120 == 0x158);

// UTL02：普通输入持有动态UTF-16；旧叠加视图直接关联真实字段，不复制权威状态。
class InputContext {
private:
    InputFields ownFields_ {};
    LegacyInputStorage* legacy_ = nullptr;
    InputFields& fields() noexcept { return legacy_ != nullptr ? legacy_->fields : ownFields_; }
    explicit InputContext(LegacyInputStorage* legacy) noexcept;
public:
    InputContext() noexcept;
    explicit InputContext(LegacyInputStorage& legacy) noexcept;
    InputContext(const InputContext& other);
    InputContext& operator=(const InputContext& other);
    ~InputContext() = default;
    [[nodiscard]] LegacyInputStorage* legacyStorage() const noexcept { return legacy_; }
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] std::size_t length() const;
    void clearText() noexcept;
    void reset() noexcept;
    void setText(std::wstring_view value);
    [[nodiscard]] std::string utf8() const;

    std::wstring text; // 原+0x00；仅普通现场输入持有正文，不限制旧字节容量。
    std::int32_t& x; // 原成员偏移：+0x120。
    std::int32_t& y; // 原成员偏移：+0x124。
    std::int32_t& caretByteOffset; // 原+0x128；原生输入为UTF-16码元，旧存储视图为GBK字节。
    std::int32_t& totalByteLength; // 原+0x12C；长度单位与当前输入存储一致。
    std::int32_t& field130; // 原成员偏移：+0x130。
    std::int32_t& field134; // 原成员偏移：+0x134。
    std::uint8_t (&field138)[4]; // 原成员偏移：+0x138。
    std::int32_t& field13C; // 原成员偏移：+0x13C。
    std::int32_t& field140; // 原成员偏移：+0x140。
    std::int32_t& maxLine; // 原成员偏移：+0x144。
    std::int32_t& maxCharPerLine; // 原成员偏移：+0x148。
    std::int32_t& field14C; // 原成员偏移：+0x14C。
    std::int32_t& encrypt; // 原成员偏移：+0x150。
    std::int32_t& field154; // 原成员偏移：+0x154。
    std::int32_t& field158; // 原+0x158；可见起点，原生UTF-16／旧存储GBK偏移。
};

// UTL02：行范围使用UTF-16码元偏移，end不含CR，nextStart包含分行字符。
struct TextLine {
    std::size_t start;
    std::size_t end;
    std::size_t nextStart;
};
[[nodiscard]] std::vector<TextLine> SplitLines(const InputContext& context);
void AppendText(std::wstring_view utf16);
// 窄CRT兼容边界；内部只编辑UTF-16。
void AppendText(std::string_view utf8);
[[nodiscard]] std::wstring ContextText(const InputContext& context);
[[nodiscard]] std::size_t ContextUtf16Offset(const InputContext& context, std::int32_t storedByteOffset);
void SetVisibleUtf16Offset(InputContext& context, std::size_t utf16Offset);

void LoadInitialText_44A160(std::wstring_view initialText);

// 原 sub_404060@[0x404060,0x40411E)，返回 true 表示拒绝输入文本。
[[nodiscard]] bool IsRejectedInputText_404060(const char* legacyText);

// 原 sub_44B190@[0x44B190,0x44B1C5)，判断零结尾旧 ANSI 串的末单元。
[[nodiscard]] std::int32_t ClassifyLastAnsiUnit_44B190(const char* text);
// 原44B570：固定扫描40字节边界；非正S8按两字节推进，第二参数不消费。
std::int32_t ClassifyChatSplit_44B570(const char* text, std::int32_t unused);

// 原 sub_44B440@[0x44B440,0x44B49D)，按低 S16 偏移判断旧双字节边界，返回 AL=0/1/2。
[[nodiscard]] std::uint8_t ClassifyLegacyBoundary_44B440(
    const char* text, std::int16_t byteOffset); // HBN01：见历史浏览/字节滚动完整合同。

// 原 sub_44B4A0@[0x44B4A0,0x44B561)：跳过旧双字节尾字节，返回可写的首个匹配位置。
[[nodiscard]] char* FindLegacyByte_44B4A0(char* text, const char* target);

// 原 0x626C04：当前输入上下文；允许由 SetContext 写为 nullptr。
extern InputContext* gCurrentInputData;

// 原 stru_626AA0@0x626AA0：本项只保留地址身份与映像零初始化。
extern InputContext gInputContext626AA0;
// 原连续区间 [0x5F9758,0x5FB7F8)：24 项 InputContext 的唯一 owner。
extern InputContext gInputContexts5F9758[24]; // 原全局：0x005F9758。
// 旧6的15CH*(行+26*页)地址视图；24以后的原地址落入既有聊天存储，不创建额外预设。
InputContext* BorrowLegacyPresetContext5F9758(std::int32_t index) noexcept;
// 原回车分派按地址比较的七个独立 InputContext；各自保留原地址身份。
extern InputContext gInputContextD66CA8; // 原全局：0x00D66CA8。
extern InputContext gInputContextCB03F0; // 原全局：0x00CB03F0。
extern InputContext gInputContextCC46E0; // 原全局：0x00CC46E0。
extern InputContext gInputContextCC2080; // 原全局：0x00CC2080。
extern InputContext gInputContextD2D828; // 原全局：0x00D2D828。
extern InputContext gInputContextCA0180; // 原全局：0x00CA0180。
extern InputContext gInputContextCB0610; // 原全局：0x00CB0610。 原窗口61关键词输入，独立15CH虚拟零对象。
extern InputContext gInputContextCB0028; // 原全局：0x00CB0028。 原摆摊名称输入，与说明上下文独立。
extern InputContext gInputContextCC2378; // 原全局：0x00CC2378。 原摆摊说明输入，15CH虚拟零对象。
extern InputContext gInputContextCF4E60; // 原全局：0x00CF4E60。 原摆摊定价输入，独立15CH虚拟零对象。
extern InputContext gInputContextD2DE70; // 原全局：0x00D2DE70。 原窗口11输入，不包含紧邻D2DFCC的结果经验槽。
extern InputContext gInputContextD2DFD0; // 原全局：0x00D2DFD0。 旧22宠物改名输入，末尾不包含独立D2E12C计时槽。
extern InputContext gInputContextD2DC10; // 原全局：0x00D2DC10。 原窗口9称号输入，独立15CH虚拟零对象。
extern InputContext gInputContextD2D4A0; // 原全局：0x00D2D4A0。 原宠物详情共享名称输入，独立15CH虚拟零对象。
extern InputContext gInputContextD64CB0; // 原全局：0x00D64CB0。 原窗口58第一条资料输入。
extern InputContext gInputContextD2D670; // 原全局：0x00D2D670。 原窗口58第二条资料输入。
extern InputContext gInputContextCAFEC8; // 原全局：0x00CAFEC8。 原窗口58第三条资料输入。
extern InputContext gInputContextC8B1B8; // 原全局：0x00C8B1B8。
// 原 CC21E8：交易数值输入，末尾不包含独立的 CC2344 状态。
extern InputContext gInputContextCC21E8; // 原全局：0x00CC21E8。
// 原CB0878：窗口42数量输入，独立于交易输入与聊天输入。
extern InputContext gInputContextCB0878;
// 原CA0018：窗口41存取数值输入，不包含相邻CA0174对象槽。
extern InputContext gInputContextCA0018; // 原全局：0x00CA0018。
// 原CC49C0：窗口51标题编辑，末尾不包含CC4B1C状态。
extern InputContext gInputContextCC49C0; // 原全局：0x00CC49C0。
// 原CA02E0：通用对话36/37共享输入上下文，UI只借用。
extern InputContext gInputContextCA02E0;
extern InputContext gInputContextD2E130; // 原全局：0x00D2E130。 窗口48九个输入原型共用的唯一上下文。
extern InputContext gInputContextD2E2E0; // 原全局：0x00D2E2E0。 旧人物501FD0称号编辑，独立于现代窗口9。
extern InputContext gInputContextCB01F8; // 原全局：0x00CB01F8。 旧选择32物品名称输入；CB0320/24为本对象字段别名。
extern InputContext gInputContextCF47E0; // 原全局：0x00CF47E0。 旧53输入；只承载15CH虚拟零，完整初始化生产仍待。
extern InputContext gInputContext1050B80; // 原全局：0x01050B80。 旧87公会宠物邮件草稿，独立15CH虚拟零对象。
extern InputContext gInputContext1051348; // 原全局：0x01051348。 旧90公会普通邮件草稿，独立于宠物邮件输入。
extern InputContext gInputContexts1050F18[3]; // 原全局：0x01050F18。 旧95三项资料输入，原15CH步长，末界不含105132C。
// 原 dword_C9A96C@0xC9A96C：该特殊上下文的外部控制值，映像初值为零。
extern std::int32_t gValueC9A96C;

// 原 [593850,5939B4) 的前缀文本；紧邻独立 InputContext@593858。
const char* LegacyText593850() noexcept;
// 旧99借同一连续owner内的593858上下文，保留前缀与上下文的原地址关系。
InputContext* BorrowLegacyBlacklistContext593858() noexcept;

// 下列状态只保留当前已取证的读写语义，不扩展业务命名。
extern std::int32_t gValue626C0C; // 原全局：0x00626C0C。 输入相位计数；接受字符后写 20。
extern std::int32_t gValue626C14; // 原全局：0x00626C14。 聊天回复跳过日志/名字处理的共享门禁，UI与协议写者共用。
extern std::int32_t gValue626C20; // 原全局：0x00626C20。 超过 maxLine 时选择截断或回滚分支。
extern std::int32_t gValue626C28; // 原全局：0x00626C28。 特定回滚路径写 1。

// 原 Input::SetContext@[0x44B330,0x44B33A)。
void SetContext_44B330(InputContext* context);

// 原 503E40/503E60：当前上下文地址比较，返回完整 W32 的 0 或 1。
std::int32_t IsCurrentContext_503E40(const InputContext* context);
// 原函数：0x00503E60。
std::int32_t IsChatContext_503E60();
// 原44B340：Shift优先于Ctrl，扫描F1..F8并追加最后命中的同源预设文本。
std::int32_t ProcessPresetHotkeys_44B340();

// 原 sub_449CF0@[0x449CF0,0x449D92)。
void InitializeContext_449CF0(
    InputContext* context,
    std::int32_t field130,
    std::int32_t field134,
    std::int32_t renderMode,
    std::int32_t fontType,
    std::int32_t color,
    const char* initialText,
    std::int32_t maxLine,
    std::int32_t maxCharPerLine,
    std::int32_t field14C,
    std::int32_t encrypt);
void InitializeContext_449CF0(
    InputContext* context,
    std::int32_t field130,
    std::int32_t field134,
    std::int32_t renderMode,
    std::int32_t fontType,
    std::int32_t color,
    std::wstring_view initialText,
    std::int32_t maxLine,
    std::int32_t maxCharPerLine,
    std::int32_t field14C,
    std::int32_t encrypt);

// 原45D720：清154/158，按Y/X/render/color写入；第四参数不消费。
// WIC01：原滚动零/坐标模式颜色发布序与真实context返回，见doc/cg2d_window_input_context_dispatch_spec.md。
InputContext* Sub_45D720(
    InputContext* context, std::int32_t x, std::int32_t y,
    std::int32_t unused, std::int32_t color, std::int32_t renderMode);
// 原45D760：按X/Y/color/render/font写入，不清154/158。
// WIC01：原坐标颜色模式字体发布序与真实context返回，见doc/cg2d_window_input_context_dispatch_spec.md。
InputContext* Sub_45D760(
    InputContext* context, std::int32_t x, std::int32_t y,
    std::int32_t color, std::int32_t renderMode, std::int32_t fontType);

// 原 Input::AppendChar@[0x44ADB0,0x44B044)。
void AppendChar_44ADB0(char character);
// 原函数：0x0044A160。
void LoadInitialText_44A160(const char* initialText); // 向当前动态输入逐字追加；旧存储视图单独检查容量。
// 原函数：0x0044A1B0。
void BrowseChatHistory_44A1B0(); // 同源64条历史，Ctrl/Shift门禁差异及输入滚动尾；由场景调用。 // HBN01：见历史浏览/字节滚动完整合同。

// 原476740：拒绝单字节逗号/竖线/反斜杠及全空白，80..FF按双字节扫描。
std::int32_t Sub_476740(const char* text);
// 原45E770：只在81..FF后跳过一字节，命中单字节竖线/反斜杠返回1。
std::int32_t Sub_45E770(const char* text);

// 原 sub_52D1B0：旧文本字节转义，始终返回目标缓冲指针。
// 旧GBK协议／存档域的字节转义，容量仍按GBK字节计算。
char* EscapeGbkText(const char* source, char* destination, std::int32_t capacity);

std::wstring EscapeText(std::wstring_view source);
// 容量为转义后GBK字节数（含NUL），正文和返回值仍为动态UTF-16。
std::wstring EscapeText(std::wstring_view source, std::size_t gbkCapacity);
std::wstring EscapeTextAndNotify(std::wstring_view source, std::size_t gbkCapacity);
// 固定ASCII组包的窄适配；容量为字节含NUL，超容量整串拒绝。
char* EscapeLegacyText_52D1B0(
    const char* source, char* destination, std::int32_t capacity); // ETE01原根，UTL02替代旧截断。

// 原 sub_52D2D0 实际透传字符地址；玩家短称号不具有完整 InputContext 布局。
char* EscapeLegacyTextAndNotify_52D2D0(
    const char* source, char* destination, std::int32_t capacity); // ETE01原空叶／同destination返回保持。
// 输入上下文的动态UTF-16转义边界，GBK容量在提交前检查。
// 原函数：0x0052D2D0。
std::wstring EscapeInputText_52D2D0(
    const InputContext* context, std::size_t gbkCapacity);

// 原 52D150/52D2F0：原位还原旧字节转义；包装随后调用空叶并透传原指针。
char* UnescapeLegacyText_52D150(char* text); // UTE01：原位BYTE取样/固定S32长度与计数，同text返回。
// 原函数：0x0052D2F0。
char* UnescapeLegacyTextAndNotify_52D2F0(char* text); // UTE01：反转义/原空操作/同text，非新增通知。

// UTL01：source为UTF-8，输出预算为字节，旧列预算不切开完整Unicode字符。
// 原0x00503AB0的GBK输入、UTF-8显示输出边界；固定记录仍用原接口。
std::int32_t ExtractGbkTextLine(
    char* destination, std::int32_t capacity, const char* source,
    std::int32_t requestedLine, std::int32_t width);
std::int32_t ExtractGbkTextLine(std::wstring& destination, const char* source, std::int32_t line, std::int32_t width);
std::int32_t ExtractTextLine(std::wstring& destination, std::wstring_view source, std::int32_t line, std::int32_t width);
std::int32_t ExtractTextLine(char* destination, std::int32_t capacity, std::wstring_view source, std::int32_t line, std::int32_t width);
std::int32_t ExtractTextLine(char* destination, std::int32_t capacity, const char* source, std::int32_t line, std::int32_t width);

// 原503AB0：GBK协议／资源输入边界，转换后按旧逻辑列预算提取指定行，CR/LF分开处理，0x01跳到下个8行边界。
std::int32_t ExtractLegacyLine_503AB0(
    char* destination, std::int32_t capacity, const char* source,
    std::int32_t line, std::int32_t width);

// 原 sub_52D570：逐字节从原缓冲区删除指定集合，尾部写 NUL，返回低 W8 零。
char RemoveLegacyBytes_52D570(char* text, const char* excluded);

// 原 sub_4596D0：按唯一旧键状态表分派输入编辑及尾部音频动作。
void ProcessKeyInput_4596D0();

} // namespace cg::input
