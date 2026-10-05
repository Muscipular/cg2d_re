# CPH01 编译器帮助入口与调色板正常算术

<a id="compiler-helper-baseline"></a>
## 原入口与当前源码差异

[两完整原体](cg2d_compiler_helper_evidence.json)：53B498..53B4BF的15条/39B，53BDC0..53BDEF的17条/47B，共32条/86B。__ftol原取x87输入、保存控制字/设置向零舍入、FISTP到S64、恢复控制字后返回EDX:EAX；__alloca_probe原按0x1000逐页触读并调整x86栈/返回。前者340直接call+3代码jump，后者33直接call；统计/样例不代全部父核对。

当前[Palette.cpp](../src/Palette.cpp)的TruncatedLowByte已先转S64再取低U8，却额外对非有限/超S64域返回0，模拟FISTP异常整数不定值；Add32也先U32相加再bit_cast回S32模拟回绕。这两处专项异常承载与RPLAN.24冲突。TruncatedLowByte六个调用、Add32五个调用本来已在UpdatePalette_450BB0，不新造本体/原身份。

原[调色板合同](cg2d_resource_implementation.md)保蓝未存F32结果、绿/红已存F32，以及各自有色调浮点状态和低AL消费。六转换邻窗去重28条/150B，仅证450EC4/450ED3/450EE2/450F40/450F50/450F60及低AL写，不重验完整357条。三已声明S32计数62B09C/628FDC/629310的原邻窗和五个1u/10u源码表达式记录在同一证据内。

<a id="compiler-helper-decision"></a>
## 工程裁决和实际提供者

按[D02受限宿主承接](cg2d_crt_host_migration_spec.md)与[RPLAN.24/工程委托](../AGENTS.md)：

- 撤TruncatedLowByte非有限/超S64检查与返回0，保正常static_cast<S64>后取U8；蓝/绿/红取值、F32存储、六调用和字段宽度不变。
- Add32改正常S32 value + 显式S32 increment；接口和五调用保持，实际increment仅1/10。原计数/节流/有符号比较与发布序不改，不扩宽非地址字段。
- 53B498、53BDC0登记host编译器私有职责。前者由现有显式整数转换表达式承担，例为TruncatedLowByte的S64→U8及[纹理坐标](../src/render/LegacyTextureDrawing.cpp)的S64→S32；后者由本机编译器为自动局部存储生成的帧/探页机制承担，例为[明文两帧父](../src/network/NetworkRuntime.cpp)的0x4004/0x4002 std::array。没有同名假帮助体、手工x64汇编或手动__chkstk调用。
- 不迁改现有业务宽度/借用/调用/释放，不恢复旧x86寄存器和栈协议，不将host标原体等价或范围排除。CRT内部机制按D02接受；PC53/业务正常结果与现有局部兼容实现保留。
- 不新增溢出/越界保护、拒绝、钳制、补零、异常重同步或回绕模拟，不称NaN/越界/栈故障等价、安全或已修复。

[微软转换文档](https://learn.microsoft.com/en-us/cpp/cpp/standard-conversions?view=msvc-170)确认正常可表示域的浮点到整数向零截断；它不证明原全部输入或生成指令。[微软__chkstk说明](https://learn.microsoft.com/en-us/windows/win32/devnotes/-win32-__chkstk)说明栈检查由编译器按架构生成；实际x64帧、阈值触发/探页、ABI/库符号未编译运行验证，不能用文档替当前产物证据。

<a id="compiler-helper-acceptance"></a>
## 本步限定静态

两原体32条/86B连续覆盖及引用类型/样例，六调色板转换边/低AL邻窗、三计数地址读写和原signed比较；源码两块精确增量、六转换/五加法表达式及其他源不变；两编译器host来源角色、既有Palette语义追加局部依据、登记/派生度量同步。

本组I/S只登记调色板实际修正及限定静态，不验全部340/3/33父、未知间接alias/全浮点环境/实际宽返回消费、资源/完整调色板357条或ABI/链接/画面/运行。C05/G04/R01/R04/V04保持open，完整FUNC-CRT/FUNC-RENDER、R1/R7/R8仍待；音频暂停、原格式明文和执行限制保持。

<a id="compiler-helper-source-state"></a>
## 当前交付

两匿名工具恢复正常S32/S64承载，保五加法和六转换调用；源头增加两个host语义/处置、既有450BB0同记录追加本步依据。CRT已知成员145→147仍未全闭，渲染成员保持26。不把编译器角色映射称所有调用父已迁移/ABI通过。

固定命名快照：`cg2d-re-compiler-helper-20261005`。
