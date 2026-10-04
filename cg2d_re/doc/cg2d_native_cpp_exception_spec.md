# 五类本机 C++ 异常接线 SPEC

> NCE01；2026-10-05按D02与工程委托实施。[运行库边界](cg2d_crt_runtime_boundary_spec.md) §2批准以真实本机抛出/捕获承接私有54B123职责；本组迁移所有当前九原点，不复造旧核心、ThrowInfo/RVA或FS链。C06、硬件SEH及完整SDK/运行仍开放。

<a id="native-cpp-exception-baseline"></a>
## 1. 原体、类型及当前缺口

[只读证据](cg2d_native_cpp_exception_evidence.json)保留54B123二十四条、五Raise各十条、54A67A/54A899各35条、549F62六十条；九根204条计入重复共享尾，唯一198条。54A30D属三个原根，不拆新F。5501E8八W32种子为E06D7363/1/0/0/3/19930520/0/0；54B123最终RaiseException，原非继续标志为1。九直接入向与当前源码九调用对应。

五原ThrowInfo的真实unwind及捕获数组如下，属性均0，基础exception原名为全局.?AVexception@@，不能未经映射当成宿主std::exception：

| 原ThrowInfo | 派生→基类 | 原copy / unwind | 本机类型 |
|---|---|---|---|
| 551880 | logic_error→exception | 54A7AE / 54A755 | LogicError→Exception |
| 551810 | length_error→logic_error→exception | 54A881 / 54A80B | LengthError→LogicError→Exception |
| 551938 | out_of_range→logic_error→exception | 54A969 / 54A8F3 | RangeError→LogicError→Exception |
| 551658 | runtime_error→exception | 54A0E5 / 54A08C | RuntimeError→Exception |
| 5515E8 | failure→runtime_error→exception | 54A1B8 / 54A142 | StreamFailure→RuntimeError→Exception |

当前五Raise已先本族copy到临时28B原对象视图，再依opaque ThrowInfo调未实现54B123；两个字符串异常及ios clear已构造临时异常，另有clear null/null重抛。旧数据可追踪，不构成可用x64 SDK对象。所有本族新端字段视图都是同一已证48B LegacyLogicErrorObject别名；基类为24B，message为24B。NSH01/EXH01已分别承接message/text块，非删除析构负责字段，不释放对象storage。

## 2. 单一原生owner与复制策略

真实compiler异常对象持有一个48B业务payload，不把Legacy对象本身传作SDK异常。普通派生tag都经同一持有者；只有最内层owner按实际类型调用一次原非删除析构，禁止基类/派生重复清理。payload字段/宽度保持，构造前不补零或解释未写prefix/padding；新本机虚表、类型名/布局/记录属于D02私有ABI差异。

原catchable数组证明不同捕获层级使用各自copy。基础Exception按值复制只走54B0B6/54B100，不读message；按值截成LogicError/RuntimeError分别走对应copy/析构；具体派生仍各自callback。不得默认浅复制拥有块或删除赋值操作后偷偷生成浅复制；本组不提供未使用的赋值/移动操作。

<a id="native-cpp-exception-decision"></a>
## 3. 具名实施决定

1. 新增本模块私有NativeCppException.hpp，声明cg::platform::native_cpp六种真实C++类型及同一payload owner；实现落在既有LegacyException.cpp，不改CMake/新增cpp或依赖。Root Exception不继承std::exception，避免加原未证明的标准库捕获层级；旧全局exception/五族映射到本机具名tag。
2. owner的kind只控制本机类型复制/清理；原methods、text、完整ownsText、message/prefix/共享标记与逻辑容量仍在同一payload，不造消息副本。what()基础用既有54B116，五派生用共享54A062，保原空回退与消息字节，不使用std::ios_base::failure后缀。
3. 五Raise直接throw对应本机prvalue；本机构造在同一表达式调用一次原本族copy。删原手工临时副本再传54B123的路径，避免复制两次。项目既有C++20提供同类型prvalue直接构造异常对象；不额外调用原构造/RNG或预清。
4. 54A67A/54A899和549F62已经构造异常payload，改throw对应类型的AdoptPayload入口。入口只以trivially-copyable字节转移现48B payload，既有message/text指针和标记不增引用，不清原临时字段、不追加原临时析构。该POD临时无自动析构；只有compiler持有的异常对象最终按原ThrowInfo unwind回调清理，避免双重owner。原临时message catch/Tidy/throw原序保持。
5. clear低U8重抛分支改直接throw;，传播当前异常；不新造failure或成功返回。没有活动异常时由本机语言运行时处理非法重抛，不加备用异常/保护或专项模拟。状态/掩码/最高U8前缀、优先消息及单state临时串清理源码保持。
6. 明确定义基础和所有五派生copy构造，对应原捕获层级/切片；删除复制赋值，防止默认浅赋值。非删除析构按kind仅调用一次原本族析构，异常继续原传播；what不使用RTTI。禁止dynamic_cast/typeid、手工x64 ThrowInfo、把原地址token当原生类型或C++虚表。
7. 同小步撤销旧54B123可调用声明及五不再传出的opaque ThrowInfo声明，保原JSON原始数据/身份和完整旧捕获数组/描述符证据，不填空SDK对象。其他Legacy复制/析构/what/方法表/捕获数据源码不改，原SEH与C06未实现私有职责不由本组自动替换。
8. 九原根及原copy/析构/what成员的当前source关联成组同步，原9数字call点保为基线，当前提供者注明throw/throw;及原生类，不记54B123原体等价。C07推进实际源码，SDK编译/链接/展开未验，不提前关闭C06/C07或完整CRT。

<a id="native-cpp-exception-acceptance"></a>
## 4. 局部门禁与限制

| 门禁 | 本组观察/标准 |
|---|---|
| I | 六类实际C++ tag/同一owner、五copy与三转移、对应清理和what；基础/派生切片分别走原callback |
| W | 五Raise、两个字符串父及clear两个点共八throw/一throw;接通；旧54B123/opaque SDK数据不再用于当前路径，无额外临时原析构 |
| S | 九根204/198唯一及五unwind/十三类型层级可追踪；扣除NCE01新增与限定接线后旧正文一致；无原元数据手造/RTTI/跨对象释放 |
| R | 禁构建/测试/运行；实际C++20生成ABI、按值捕获/清理/非法重抛和SDK/硬件SEH/完整生命周期未验 |

本组仅归档限定I/W/S；编译器生成ABI是采用的实现机制，不能以未编译源码宣称SDK验收。C06/C07、完整功能门禁和R8保持开放。

<a id="native-cpp-exception-source-closure"></a>
## 5. 本组源码收口

NativeCppException.hpp六实际C++类型和LegacyException.cpp同一payload owner已落位；明确基础/派生copy、五原copy及六原非删除析构、两个what提供者，复制赋值删除。五Raise改throw本机prvalue，一次原本族copy；三已准备payload改AdoptPayload字节转移，不增共享引用、不加原临时析构；clear低U8重抛为throw;。

旧54B123可调用声明/五opaque ThrowInfo声明撤销，五Raise按真实noreturn声明；原JSON/捕获数组/描述符和数字call基线保留，当前compiler不消费旧SDK数据。复制/析构/方法表/消息/原临时串清理正文保持；源中仅新增本机类型实现、九接线及必要include/注释，旧头仅限定声明/注释。原基本字段布局48B payload/24B基础/24B字符串不改，SDK对象布局由本机compiler持有。

九原根与十四原copy/清理/what成员共23既有语义同步；1152/1149有效ID、18pending/4分类引用保持。九原根按本组adapted使已审78→87/3680，三限定E-NATIVE-CPP-EXCEPTION-I/W/S-20261005使累计33→36；204重复归属/198唯一只支撑本组，不加原F或以源码数推总覆盖。实现固定至Git命名快照cg2d-re-native-cpp-exception-source-20261005。

C06/C07保持open：本机类型、九原点和当前正常C++清理源码已接，实际C++20生成ABI、按值捕获/非法重抛、SDK/硬件SEH/完整CRT与运行未验。源码任务关闭10/60、叶验收0/182、完整功能门禁及R8不提升；禁构建/检查产物/新增或执行测试/运行/摘要、音频暂停及原格式明文保持。
