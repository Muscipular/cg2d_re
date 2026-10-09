# 旧多字节整串转换 SPEC（MWS01）

> 2026-10-10当前多字节/参数准备边界：[MC01-C5](cg2d_crt_host_migration_spec.md#msvc-crt-multibyte-source-state-20261010)已退休无现业务调用的旧ctype/多字节/NLS/转换表与私有argv/环境、双BYTE桥、重复前置及专用致命入口；Input两项ASCII非零分类直接MSVC。应用命令四正文/发布槽、真实WinMain尾、数组terminate与所有业务/字段/字节/owner保持；未更改字体/GBK、协议/资源/存档编码或开全局locale。下文旧库表/状态/handler/9/9/8分支和其实施前态只作固定历史，不要求复刻，也不判原未知alias/邮件重叠已闭。全体实际CRT提供者与业务边界继续审计，全部适用SDK/正常结果/运行门禁保持，未编译/运行。

2026-10-05，按既有工程委托补通用CRT转换依赖；唯一音频业务消费者继续暂停。
上位：[文本编码](cg2d_text_encoding_spec.md)及[字符串合同](cg2d_legacy_string_compare.md)。
固定快照：`cg2d-re-multibyte-string-source-20261005`；[原证据](cg2d_multibyte_string_evidence.json)。

<a id="multibyte-string-baseline"></a>
## 原基线与真实用途

原53C9DD..53CADC完整94条/255B，唯一业务入向4551A6属于455100的DirectMusic装载，
其唯一父454C30当前音频暂停。它不是字体初始化/字形函数，不按相近地址猜用途。
本步仅补独立通用转换，不实现、注册或调用这两个音频父，不能登记音频恢复/验收。

原dest/source是地址，MaxCount及size_t返回均W32；新端地址拓宽、数量/返回仍U32。
53CAB5的strlen是19真实＋4宿主之外唯一未落位长度原点。
已有LegacyTextLocaleStorage.ctypeLocale/转换代码页引用、可变58FC30分类指针和errno是唯一真实owner，
复用现有存储，不猜编码/新代码页，不建宿主locale影子。

<a id="multibyte-string-decision"></a>
## 完整转换实施裁决

1. 实现在既有LegacyMultibyte.cpp，私有头声明Mbstowcs_53C9DD(wchar_t*,const char*,U32)->U32。
   复用该模块既有wchar_t为2B约束、StrLen、代码页、ctype及errno，不增cpp/依赖/一般接口迁移。
2. dest非空且MaxCount=0直接0，不读source/locale。C locale时每轮读U8写W16，
   **写后重新读source当前字节**判NUL，不能缓存写前字节；NUL已写但不计数，
   达MaxCount返回该数量、不附加NUL/清目标。
3. dest空先判locale，C locale直接真实53F9E0(source)，忽略MaxCount；
   非C locale调用MultiByteToWideChar(当前CP,原flags9,source,-1,null,0)，成功减1。
4. dest非空/非C先以flags9、sourceSize=-1/MaxCount调用同一Win32 API；成功减1，
   失败立即GetLastError；仅ERROR_INSUFFICIENT_BUFFER进入原截取回退，
   其他失败写共享errno42、返W32 FFFFFFFF，不增加新错误码/成功门禁。
5. 回退cursor从source起，remaining=MaxCount-1；每轮先查当前NUL，
   以当前可变ctype指针的U16 8000H位判lead，命中先多进一字节，随后共同进一字节。
   减remaining但按**递减前**值判继续，保MaxCount=1仍处理一字符的原do语义，
   不改成remaining>0预判，不补lead第二字节校验/MaxCount保护。
6. 回退扫描完重新读取当前代码页，用原flags1、实际字节跨度、原目标和MaxCount调用API；
   成功直接返实际W32转换数，不减1/补NUL，失败errno42/FFFFFFFF。
   原指针差仅在API数量边界转当前W32位型，数量不随size_t或地址宽度扩大；
   API cchWideChar按原W32位型传S32，不钳制/拒绝高位数量。
7. 保三个API点各自重新读代码页，ctype每字符重读；成功不清errno，
   不预清输出/增加nullptr保护、宿主mbstowcs或UTF-8自动迁移。
   越界/数学溢出沿RPLAN.24不专项处理，不声明故障等价/x64安全。
8. 函数本体先落位但不添加原音频455100调用。原SDL音频替换/暂停及当前文本显示边界不变，
   该业务父后续由获准音频工作决定如何承接实际文件名/资源语义。

<a id="multibyte-string-acceptance"></a>
## 限定验收与门禁

- 原完整94条、三API/唯一长度点及指令中的别名重读、截取回退门槛可追。
- 两既有源/头仅增加完整本体/声明；原字符/mb分类、四其他转换、初值和provider保持。
- 新增父语义1及长度原点1，长度20真实/24＋4已批准宿主；
  本体/内部strlen接线不表示唯一音频业务入向已接或音频工作项完成。
- R1 adapted只登记本组地址拓宽/U32数值及Win32提供者；
  SDK/API实际成功失败、全部locale/代码页生产/别名、文字/音频完整链及运行未验。
- C12/T01/T03/V04及完整门禁保持open，任务/叶不提升；
  不构建/查产物/新增或执行测试/运行/摘要，不运行转换样例。

<a id="multibyte-string-source-state"></a>
## 本组源码状态

补通用53C9DD完整转换与53CAB5真实长度调用，原共享状态、输出/源重读与原失败码保持。
455100/454C30继续暂停且未接线；不把通用函数落位当音频恢复或整条文本验收。
