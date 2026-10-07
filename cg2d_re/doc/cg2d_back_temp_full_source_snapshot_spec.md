# Back→Temp整源复制与转场准备返回 SPEC

编号BVF01；2026-10-08。修复转场准备的完整返回值与共享复制的源矩形差异；先明确合同，再实施及登记限定静态结果。

<a id="back-temp-baseline"></a>
## 原基线与差异

[完整原证](cg2d_back_temp_full_source_snapshot_evidence.json)保存准备根全部指令、六调用窗口、九共享复制窗口、零实参生产与源/机器前态。

| 项目 | 原证 | 当前缺口 |
|---|---|---|
| F:0011D750 / 新语义1761 | [51D750,51D781)，18条49B/一页，1 direct RAND、1 indirect复制、零跳转/一返回；六引用/四父 | 私有声明及返回窄化为S8，与实际完整EAX不符；IDA的char原型不作为事实 |
| 随机状态 | 先复制，忽略结果后一次RAND(0,35)；51D776只AND AL,FE，51D77B写完整EAX至FF48E4，51D780直接RET | 现完整W32写入正确，返回只低S8；写入/返回应沿同一完整持样 |
| 共享整源复制 | 九使用点皆Temp.BltFast(0,0,Back,NULL,16) | CopyBackToTemp固定640×480矩形，丢失原空源RECT的完整Back范围 |
| X:0011D781-0011D790 | 15B逐字节90/NN_NOP，无入向，后为真实51D790入口 | 用途未登记，不能当函数/范围排除 |

六调用局部共84条418B，每窗14条，来自43CBB0两处、45EEE0、51C430和51C7D0两处；六处均不消费该返回，现六源码调用同样弃值。本步核该接口消费，不代四父完整验收。

九共享复制窗累计200条712B，含邻指令、重叠及准备18条/场景9完整40条163B，不当九完整父；使用点如下：

| 原复制点 | 现来源/条件 | 原零参数生产 |
|---|---|---|
| 521816 | DrawAllQueued.cpp / operation7，C82204非零 | EBX；原入口归零及公共归零/回边，GQD01完整根case7准确复用 |
| 43CB7B | MapSceneDrawing.cpp / 战斗背景快照 | 43CB11将ESI归零 |
| 466CAC | RendererRebuild.cpp / scene10重建快照 | 466B9C置ESI=0 |
| 51C910 / 51CBF1 / 51D11A | SceneState.cpp / normal三条快照路径 | 各实际case的EBX归零 |
| 5218E1 | GraphicPendingQueue.cpp / 波纹启动 | 521875将EBX归零 |
| 51D76A | SceneState.cpp / 随机准备 | 三处直接push0 |
| 51D6A7 | SceneState.cpp / scene9快照 | 51D649将EBX归零；BUS01既有1719保持 |

既有[GQD01](cg2d_queued_draw_alignment_spec.md#queued-draw-baseline)保存520DD0完整769条2658B/14引用及case7，[现复制合同](cg2d_sdl_presenter.md)已明确空源RECT整面请求；本步精确复用，不重计该F或称新增完整父验收。另补入口/公共归零回边/normal分派三原窗口58条219B；寄存器写表区分其它case、RET后的恢复与实际复制路径，不把线性邻接当同一路径。

<a id="back-temp-decision"></a>
## 实施决定

1. Sub_51D750私有声明和实现改为std::uint32_t，低AL仍只清最低位，组合完整result后写FF48E4并返回同值；一次RNG、范围0..35、复制失败后继续执行及六调用弃值保持。恢复的是原完整W32位型，不凭反编译类型窄化。
2. 在既有CopyBackToTemp内借用实际SDL源/目标面，以源面的w/h构造从(0,0)开始的完整矩形，调用既有无源色键复制助手；不缩放、不引入固定640×480裁剪、不新helper/API/owner。源/目标不可用或目标不能容纳整源时返回false，不把部分裁剪当成功，所有九调用原控制流保持。
3. 该失败是原native正常资源失败边界，与[BPB01](cg2d_ripple_full_strip_production_spec.md#ripple-full-strip-source-state)同样保留。Microsoft的[BltFast文档](https://learn.microsoft.com/en-us/windows/win32/api/ddraw/nf-ddraw-idirectdrawsurface7-bltfast)支持其不能裁剪及无效矩形等失败；空源RECT整源合同依据上述既有原基线和九实际零参数，不能声称此网页说明了NULL。不增数学溢出/越界保护、补零、钳制、异常模拟或输入限制。
4. 两cpp两个实现体、两hpp两个接口/说明片段共四片段；除准备函数返回类型与完整返回表达，现九调用、其它函数全文、CPU Back/Temp原owner与借用、锁/色键保存恢复、清理/RNG/tick/计数顺序保持。公开头修正旧640×480说明。
5. 一adapted/一X用途；LOOP、RENDER、MAP、BATTLE各一原成员和I/S两证据，共八限定I-S各28真实本地来源；G02/R04/V04只current/引用推进仍open。仅准备/整源复制合同、实际接口接线和用途验收，不代表完整父/callee、全像素、owner-alias寿命或机器ABI已闭合。
6. 网络原格式明文/两加解密叶排除、音频暂停、RPLAN.24和单线程限制保持。不构建/配置/调试/新增或执行测试/运行客户端或样例/产物检查/摘要计算；运行门禁待明确恢复后执行，不降低标准。

<a id="back-temp-acceptance"></a>
## 度量与验收

| 项目 | 可复核要求 / 预期 | 适用范围 |
|---|---|---|
| 原证 | 准备完整18条49B；六窗84条418B/四父；九窗200条712B；GQD01既有完整根及三零生产窗口58条219B | 六/九局部不是全部父闭包 |
| 源修复 | 两cpp两hpp仅四片段；完整W32同值写入/返回、整Back范围及原native正常失败；六弃返回/九弃复制结果 | 不增RNG、调用方早退、裁剪或新owner |
| 用途 | 一X15B逐字节NN_NOP，无入向/后真入口 | 不新增函数或排除 |
| 原新任务 | 新1761/F:0011D750 adapted；LOOP174→175/RENDER75→76/MAP128→129/BATTLE263→264；八限定I-S/三任务open | 各完整域/叶/全局门禁保持 |
| 看板 | 语义1762/有效1755、合同1873（50.90%）、F903/E1074、R7已审465余2596；measure及Check退出0 | 分类120/mixed3/完整入口排除113保持 |

<a id="back-temp-source-state"></a>
## 收口

先行SPEC后四片段源码、原新与用途/七对应文档/三任务和唯一进度同步；集中静态及文本派生后记录实值，精确提交并固定cg2d-re-back-temp-full-source-snapshot-20261008。原1719与所有旧记录、pending18/未知ECX、closed11/差队列及完整闭包保持。完整像素、其余父/callee与借用寿命、机器ABI/链接/正常失败实测/运行及R8继续，整体目标active。

### BVF01 当前准备与整源复制实修限定静态结果

集中静态通过：准备完整18条49B/六调用局部84条418B/四父、九共享复制窗200条712B含邻指令及重叠，GQD01完整769条2658B及case7精确复用/三零生产窗口58条219B，场景9既有1719保持、一X15B逐字节NN_NOP用途。两cpp两hpp仅四片段：完整W32同值写入/返回，真实Back整源面及原native正常资源失败，不部分裁剪/缩放/色键；六准备/九复制消费均弃值、失败后仍一次RNG，其他五源/所有余体和调用方控制流保。完整机器与完整派生仅具名差集，十八文件UTF8无BOM/LF、四锚点/八限定I-S各28真实来源通过。

文本汇总及Check均退出0；实际语义1762/有效1755、合同1873（50.90%）、F903/E1074、R7已审465余2596、LOOP175/RENDER76/MAP129/BATTLE264。原分类120/mixed3/完整入口排除113、pending18/未知ECX、closed11/差队列/所有旧记录及完整闭包保持；仅准备/整源复制合同、接口接线与用途静态通过，全像素/其它父-callee/owner-alias寿命、机器ABI链接/正常失败实测/运行和R8仍未验，整体目标继续。
