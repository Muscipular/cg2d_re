# 窗口创建、七类载荷与动画源槽 SPEC（WPL01）

2026-10-10。沿WDP01/WIC01核载荷生产者，依子AGENTS第8条先记事实和工程边界。同一cg2d_re分支、不打tag；现GBK、原格式明文、MSVC CRT及WAV-only保持。

<a id="window-payload-baseline"></a>
## 完整创建原根与既有释放

471440–4718C7（尾不含）完整384条1159B/六页、13实际E8/五目标、40跳五RET，另一次真实update间接调用；52普通CALL入向来自21父，仅来源定位。旧1929/F:00071440只关联CAC08数据和源码，本步保其历史，再追加完整创建合同。

七槽表4718C8为4715C2/471603/471657/471733/47174F/47174F/471764，type5/6共原4B分配段。旧X:000718C7-000718F0及41B邻域保，不新处置。原58609C六BYTE为31302F313000，即10/10加NUL；当前ASCII特殊文本保。

FreeWindow_471270已由旧1797/BVR01完整纳管并适配，保原身份/处置和正确正文；本步沿该证据核owner，不重登记释放函数。先前采集同时取得释放原根的140条426B，本步只作创建关联核对，不增加新F或复记验收。公开WindowRuntime.hpp只前置声明实例，真实载荷/元素/实例类型在WindowRuntimeState.hpp。

<a id="window-payload-decision"></a>
## 原创建和发布合同

类别恰1先关闭类别2、恰2先关闭类别3；扫描71槽，同id存在则重新取槽实例置state|2000再返回新槽值。否则找第一空槽，无槽返null；calloc实例失败返null。原实例E0H、元素24H按既有x64原生类型sizeof承载已证地址字段，非地址宽度/字段和数量策略不改。

从实际原型逐字段发布id/state/layer/width/height/flags/update/draw/release/elementCount。count非零分配元素数组，失败按原调用FreeWindow再返回null；不补新的回滚/保护/计数归零或失败模型。循环按原现场elementCount，持有当前原型元素，各type/state0/x/y/width/height/visible发布保，默认callback为既有4B0F10。

| type | 原外层字节 | 当前载荷/内容 | owner及释放 |
|---|---|---|---|
| 1 | 18H | WindowGraphicData六W32，imageNo→color→offsetX/Y→width/height | UI持有外层，BVR释放 |
| 2 | 8 | WindowAnimationData：YobiAction*及W32 value04 | UI持外层，action由Yobi管理；释放先标死，再free外层 |
| 3 | 8 | WindowTextData：四BYTE及text*，另分配256B文本 | UI持外层与文本，先free文本再free外层 |
| 4 | C | WindowMouseData：flags及两个S32增量 | UI持外层，复制flags，其余沿calloc零 |
| 5 | 4 | WindowInputData：非拥有InputContext* | UI持外层，context另有owner |
| 6 | 4 | 原未消费W32存储，沿现有uint32分配 | UI持外层；不猜新字段/业务 |
| 7 | 8 | WindowNumberData：借用text*及color/style/flags/byte07四BYTE | UI持外层，借用字符串不随外层free |

已证指针扩大，现sizeof不等于原x86外层字节；不以本步宣编译布局通过。各原分配失败/null分支保持。type3原文本复制、调用4740F0、10/10比较及可能替换、两个BYTE复制保持，不扩大清零或容量。type7仍只复制真实text指针及后四BYTE，不复制/释放借用字符串。

动画原序：NewYobiAction(2,0)返回→发布载荷action→用原返回action读/写flags|2→重新读载荷action清callback→读原型图号指针槽→重新读载荷action→读图号W32→发布action.imageNo→再读载荷action发布priority DF→外层value04=0。现helper虽已按引用读action槽，但图号指针按值提前传入，首flags也从槽重读，未完整保原取样。

按工程委托修改唯一已有helper边界：ConfigureWindowElementAction接收初始返回action、现有YobiAction*&载荷槽和int32_t* const&原型图号槽；只借现有槽，且已确认一个CPP调用点。先用初始action写flags，再现场槽清callback；显式取图号指针、取目标action、读图号值再写，末次新槽写priority。不向UI公开Yobi私有布局，不新建类型/API/helper、owner或影子槽；当前两个参数改为三个参数，声明/定义/唯一调用一起更新。图号源槽和值均不写，引用不逃逸；完整其他模块/机器ABI及运行仍待。

一逻辑组三处取样：首flags目标、延迟图号槽、图号指针→action槽→值的交错顺序；四关联注释。其余六类、拖动矩形每字段新原型/count、最后发布全局槽/state位/保存坐标→旗位→临时当前窗口→update(-1)→发布state.window→恢复旧当前窗口及真实返回保持。原拖动数正分支不增加十项上限。

不新增保护、默认值、补零、钳制、容量限制、回滚或异常/溢出模型；原calloc/业务零发布和失败分支保，普通堆沿MSVC直接CRT及已决malloc族。

<a id="window-payload-acceptance"></a>
## 限定静态验收

核创建完整原根/实际E8/五RET/间接update/52入向、七槽旧表、六BYTE特殊文本和真实结构；四整源精确重放唯一helper边界的一组三处/四注释，其余创建/旧释放/其他caller和类型保持。只有现有helper声明改变，不增加原F或新的外部SDK/协议。

旧1929全CAC08来源/表/范围保后追加，一F处置、INPUT/UIRUNTIME/RENDER/YOBI各一成员、四限定S零I；九任务open，语义身份/关联数量不变。所有枚举、完整门禁、六closure与总SPEC§10保持。

仅必要文本静态及measure_cg2d.ps1/-Check，不编译、测试、运行客户端或算法、查看产物或计算摘要。21外父、原型表/七载荷/嵌套文本/输入context/Yobi及回调全部生产消费和alias-owner寿命、完整Yobi死亡回收/资源/CRT/SDK/机器ABI/优化后读取/实际创建更新释放和全生命周期仍待；两个完整创建/释放原根不代完整窗口功能闭合。

<a id="window-payload-source-state"></a>
## 当前状态

本步前提交说明“[#] 补齐输入模式分发及上下文字段发布合同”；拟提交说明“[#] 补齐窗口载荷创建并恢复动画源槽取样”。源码修正、登记、静态汇总/-Check终态退出0及派生范围核对通过；语义2258=2210F+48X、文档关联2320/3680=63.04%保持，函数处置1700/证据1501，INPUT79/UIRUNTIME115/RENDER85/YOBI102。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持；未构建、测试或运行，整体active/未完成。
