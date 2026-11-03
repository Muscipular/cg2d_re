#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <string_view>
#include <string>

namespace cg::session { class ClientSession; }

namespace cg::input {

// 输入数值字段；保留原成员身份，文字位置／长度统一使用UTF-16码元。
struct InputFields {
    std::int32_t x; // 原成员偏移：+0x120。
    std::int32_t y; // 原成员偏移：+0x124。
    std::int32_t caretByteOffset; // 原成员偏移：+0x128。
    std::int32_t totalByteLength; // 原成员偏移：+0x12C。
    std::int32_t textX; // 正文绘制起点X，像素；原成员偏移：+0x130。
    std::int32_t textY; // 正文绘制起点Y，像素；原成员偏移：+0x134。
    std::int32_t renderMode; // 32位渲染模式；原成员偏移：+0x138，绘制边界取低8位。
    std::int32_t fontType; // 输入正文及光标使用的字体编号；原成员偏移：+0x13C。
    std::int32_t color; // 输入正文及光标使用的颜色索引；原成员偏移：+0x140。
    std::int32_t maxLine; // 原成员偏移：+0x144。
    std::int32_t maxCharPerLine; // 原成员偏移：+0x148。
    std::int32_t lineHeight; // 多行正文相邻行的Y步进，像素；原成员偏移：+0x14C。
    std::int32_t encrypt; // 原成员偏移：+0x150。
    std::int32_t visibleWidth; // 单行正文可视宽度，像素；0表示绘制时不裁短；原成员偏移：+0x154。
    std::int32_t visibleStartOffset; // 单行正文可见起点，UTF-16码元偏移；原成员偏移：+0x158。
};
// 现场输入只持有动态UTF-16；GBK转换留在协议／存档提交边界。
class InputContext {
private:
    InputFields ownFields_ {};
public:
    InputContext() noexcept;
    InputContext(const InputContext& other);
    InputContext& operator=(const InputContext& other);
    ~InputContext() = default;
    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] std::size_t length() const;
    void clearText() noexcept;
    void reset() noexcept;
    void setText(std::wstring_view value);

    std::wstring text; // 原+0x00；现场输入正文，不限制旧字节容量。
    std::int32_t& x; // 原成员偏移：+0x120。
    std::int32_t& y; // 原成员偏移：+0x124。
    std::int32_t& caretByteOffset; // 原+0x128；当前为UTF-16码元偏移。
    std::int32_t& totalByteLength; // 原+0x12C；长度单位与当前输入存储一致。
    std::int32_t& textX; // 正文绘制起点X，像素；区别于光标坐标x；原成员偏移：+0x130。
    std::int32_t& textY; // 正文绘制起点Y，像素；区别于光标坐标y；原成员偏移：+0x134。
    std::int32_t& renderMode; // 32位渲染模式；原成员偏移：+0x138，引用自身数值存储。
    std::int32_t& fontType; // 输入正文及光标使用的字体编号；原成员偏移：+0x13C。
    std::int32_t& color; // 输入正文及光标使用的颜色索引；原成员偏移：+0x140。
    std::int32_t& maxLine; // 原成员偏移：+0x144。
    std::int32_t& maxCharPerLine; // 原成员偏移：+0x148。
    std::int32_t& lineHeight; // 多行正文相邻行的Y步进，像素；原成员偏移：+0x14C。
    std::int32_t& encrypt; // 原成员偏移：+0x150。
    std::int32_t& visibleWidth; // 单行正文可视宽度，像素；0表示绘制时不裁短；原成员偏移：+0x154。
    std::int32_t& visibleStartOffset; // 原+0x158；单行可见起点，UTF-16码元偏移；聊天日志借用见WindowChat_PreviousFont。
};

// 当前输入为非拥有借用；切换会话不改变该会话的输入选择、光标及控制门禁。
struct InputRuntimeState {
    InputContext* currentContext = nullptr; // 原0x626C04，可显式设空。
    std::int32_t chatPanelVisible = 0; // 原0xC9A96C。
    std::int32_t caretBlinkPhase = 0; // 原0x626C0C，接受字符后写20。
    std::int32_t chatLogSuppressed = 0; // 原0x626C14，聊天回复与UI共用。
    std::int32_t lineOverflowRollbackMode = 0; // 原0x626C20。
    std::int32_t insertRejected = 0; // 原0x626C28。
};

// UTL02：行范围使用UTF-16码元偏移，end不含CR，nextStart包含分行字符。
struct TextLine {
    std::size_t start;
    std::size_t end;
    std::size_t nextStart;
};
[[nodiscard]] std::vector<TextLine> SplitLines(const InputContext& context);
void AppendText(session::ClientSession& session, std::wstring_view utf16);
[[nodiscard]] std::wstring ContextText(const InputContext& context);
[[nodiscard]] std::size_t ContextUtf16Offset(const InputContext& context, std::int32_t unitOffset);
void SetVisibleUtf16Offset(InputContext& context, std::size_t utf16Offset);

// 原函数：0x0044A160。
void LoadInitialText_44A160(session::ClientSession& session, std::wstring_view initialText);

// 原 sub_404060@[0x404060,0x40411E)，返回 true 表示拒绝输入文本。
[[nodiscard]] bool IsRejectedInputText_404060(std::wstring_view text);

// 原 sub_44B190@[0x44B190,0x44B1C5)，判断零结尾旧 ANSI 串的末单元。
[[nodiscard]] std::int32_t ClassifyLastAnsiUnit_44B190(const char* text);
// 原44B570：现按40个UTF-16码元判断完整字符边界，第二参数不消费。
std::int32_t ClassifyChatSplit_44B570(std::wstring_view text, std::int32_t unused);

// 原 sub_44B440@[0x44B440,0x44B49D)，现按低S16码元偏移判断UTF-16边界，返回AL=0/1/2。
[[nodiscard]] std::uint8_t ClassifyLegacyBoundary_44B440(
    std::wstring_view text, std::int16_t unitOffset);

// 原 sub_44B4A0@[0x44B4A0,0x44B561)：跳过旧双字节尾字节，返回可写的首个匹配位置。
[[nodiscard]] char* FindLegacyByte_44B4A0(char* text, const char* target);

// 原 stru_626AA0@0x626AA0：本项只保留地址身份与映像零初始化。
// 原连续区间 [0x5F9758,0x5FB7F8)：24 项 InputContext 的唯一 owner。
// 原回车分派按地址比较的七个独立 InputContext；各自保留原地址身份。
extern InputContext gInputContextCC46E0; // 原全局：0x00CC46E0。
extern InputContext gInputContextD2DFD0; // 原全局：0x00D2DFD0。 旧22宠物改名输入，末尾不包含独立D2E12C计时槽。
// 原 CC21E8：交易数值输入，末尾不包含独立的 CC2344 状态。
// 原CB0878：窗口42数量输入，独立于交易输入与聊天输入。
// 原CA0018：窗口41存取数值输入，不包含相邻CA0174对象槽。
// 原CC49C0：窗口51标题编辑，末尾不包含CC4B1C状态。
// 原CA02E0：通用对话36/37共享输入上下文，现由所属会话的原生对话状态持有。
extern InputContext gInputContextD2E2E0; // 原全局：0x00D2E2E0。 旧人物501FD0称号编辑，独立于现代窗口9。
extern InputContext gInputContextCB01F8; // 原全局：0x00CB01F8。 旧选择32物品名称输入；CB0320/24为本对象字段别名。
extern InputContext gInputContextCF47E0; // 原全局：0x00CF47E0。 旧53输入；只承载15CH虚拟零，完整初始化生产仍待。
extern InputContext gInputContext1050B80; // 原全局：0x01050B80。 旧87公会宠物邮件草稿，独立15CH虚拟零对象。
extern InputContext gInputContext1051348; // 原全局：0x01051348。 旧90公会普通邮件草稿，独立于宠物邮件输入。
extern InputContext gInputContexts1050F18[3]; // 原全局：0x01050F18。 旧95三项资料输入，原15CH步长，末界不含105132C。
// 原 Input::SetContext@[0x44B330,0x44B33A)。
void SetContext_44B330(session::ClientSession& session, InputContext* context);

// 原 503E40/503E60：当前上下文地址比较，返回完整 W32 的 0 或 1。
std::int32_t IsCurrentContext_503E40(session::ClientSession& session, const InputContext* context);
// 原44B340：Shift优先于Ctrl，扫描F1..F8并追加最后命中的同源预设文本。
std::int32_t ProcessPresetHotkeys_44B340(session::ClientSession& session);

// 原 sub_449CF0@[0x449CF0,0x449D92)。
void InitializeContext_449CF0(
    session::ClientSession& session,
    InputContext* context,
    std::int32_t textX,
    std::int32_t textY,
    std::int32_t renderMode,
    std::int32_t fontType,
    std::int32_t color,
    std::wstring_view initialText,
    std::int32_t maxLine,
    std::int32_t maxCharPerLine,
    std::int32_t lineHeight,
    std::int32_t encrypt);

// 原45D720：清154/158，按Y/X/render/color写入；第四参数不消费。
// WIC01：原滚动零/坐标模式颜色发布序与真实context返回，见doc/cg2d_window_input_context_dispatch_spec.md。
InputContext* ConfigureInputLayoutAndResetScroll_45D720(
    InputContext* context, std::int32_t x, std::int32_t y,
    std::int32_t unused, std::int32_t color, std::int32_t renderMode);
// 原45D760：按X/Y/color/render/font写入，不清154/158。
// WIC01：原坐标颜色模式字体发布序与真实context返回，见doc/cg2d_window_input_context_dispatch_spec.md。
InputContext* SetInputPositionAndStyle_45D760(
    InputContext* context, std::int32_t x, std::int32_t y,
    std::int32_t color, std::int32_t renderMode, std::int32_t fontType);

// 原 Input::AppendChar@[0x44ADB0,0x44B044)。
void AppendChar_44ADB0(session::ClientSession& session, wchar_t character);
// 原函数：0x0044A1B0。
void BrowseChatHistory_44A1B0(session::ClientSession& session); // 同源64条历史，Ctrl/Shift门禁差异及输入滚动尾；由场景调用。 // HBN01：见历史浏览/字节滚动完整合同。

// 原476740：拒绝单字节逗号/竖线/反斜杠及全空白，80..FF按双字节扫描。
std::int32_t IsValidDelimitedFieldText_476740(std::wstring_view text);
// 原45E770：只在81..FF后跳过一字节，命中单字节竖线/反斜杠返回1。
std::int32_t ContainsPipeOrBackslash_45E770(std::wstring_view text);

// 原 sub_52D1B0：旧文本字节转义，始终返回目标缓冲指针。
// 旧GBK协议／存档域的字节转义，容量仍按GBK字节计算。
char* EscapeGbkText(const char* source, char* destination, std::int32_t capacity);

std::wstring EscapeText(std::wstring_view source);
// 容量为转义后GBK字节数（含NUL），正文和返回值仍为动态UTF-16。
std::wstring EscapeText(std::wstring_view source, std::size_t gbkCapacity);
std::wstring EscapeTextAndNotify(std::wstring_view source, std::size_t gbkCapacity);
// 输入上下文的动态UTF-16转义边界，GBK容量在提交前检查。
// 原函数：0x0052D2D0。
std::wstring EscapeInputText_52D2D0(
    const InputContext* context, std::size_t gbkCapacity);

// 原 52D150/52D2F0：原位还原旧字节转义；包装随后调用空叶并透传原指针。
char* UnescapeLegacyText_52D150(char* text); // UTE01：原位BYTE取样/固定S32长度与计数，同text返回。
// 原函数：0x0052D2F0。
char* UnescapeLegacyTextAndNotify_52D2F0(char* text); // UTE01：反转义/原空操作/同text，非新增通知。

// 原0x00503AB0：显示分行只输出动态UTF-16；固定GBK记录保留原字节解析接口。
std::int32_t ExtractGbkTextLine(std::wstring& destination, const char* source, std::int32_t line, std::int32_t width);
std::int32_t ExtractTextLine(std::wstring& destination, std::wstring_view source, std::int32_t line, std::int32_t width);

// R07：仅对话正文使用；颜色标记零列、美元转义一列，CR／LF独立换行、0x01按八行分页。
std::int32_t ExtractDialogTextLine(std::wstring& destination, const char* source, std::int32_t line, std::int32_t width);

// 原503AB0：GBK协议／资源输入边界，转换后按旧逻辑列预算提取指定行，CR/LF分开处理，0x01跳到下个8行边界。
std::int32_t ExtractLegacyLine_503AB0(
    char* destination, std::int32_t capacity, const char* source,
    std::int32_t line, std::int32_t width);

// 原 sub_52D570：现从动态UTF-16正文删除指定完整字符集合，返回低W8零。
char RemoveLegacyBytes_52D570(std::wstring& text, std::wstring_view excluded);

// 原 sub_4596D0：按唯一旧键状态表分派输入编辑及尾部音频动作。
void ProcessKeyInput_4596D0(session::ClientSession& session);

// 原44A410：输入光标及IME光标绘制。
void DrawAndAdvanceInputCaret_44A410(session::ClientSession& session);

} // namespace cg::input
