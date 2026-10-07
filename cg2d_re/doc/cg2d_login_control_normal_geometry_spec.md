# 登录控件正常坐标与字段取样SPEC（BSD01）

本项按工程委托先裁决再实施，仅解决LoginInput.cpp两个既有入口的正常S32运算与原字段取样顺序差异。遵循[最新决议](cg2d_closure_decisions_spec.md)、[输入公共合同](cg2d_login_input.md)与[追踪验收体系](cg2d_traceability_acceptance_spec.md)，不扩展登录业务。

<a id="login-control-geometry-baseline"></a>
## 原版基线与开发缺口

权威来源为已打开cg2d.exe的只读IDA会话，具体指令、原字节与入向见[机器证据](cg2d_login_control_normal_geometry_evidence.json)。

| 原入口／真实身份 | 指令／字节／取证分页 | 直接调用／目标 | 真实调用点 |
|---|---|---|---|
| 5036F0／F:001036F0 | 55／142B／1页 | 1／1 | 45CA20@45CAF7、45CB7A |
| 503780／F:00103780 | 121／297B／2页 | 4／3 | 45CA20@45CD95、45CDA4 |

两根完整176条439B、5直接调用／3唯一目标、20跳转／3普通RET；不将取到完整指令等同于所有依赖验收。两相邻真实X：50377E..503780为2B、5038A9..5038B0为7B，均全90／逐BYTE canonical nop、无入向，前RET后真实入口；只登记原填充用途，不造函数或范围排除。

现源将正常S32加减改成uint32运算与bit_cast回绕模型；绘制入口两命中提前取width，基础图先drawX后drawY，覆盖矩形先left再top且width提前，均与原指令顺序不一致。正常数值本身已一致，本步不宣称发现运行故障；恢复原读取与计算先后，沿RPLAN.24撤掉专项回绕模型。

<a id="login-control-geometry-decision"></a>
## 工程裁决与实施边界

只改src/LoginInput.cpp五个精确片段，不新增工具、状态、依赖或接口：

1. 查询非空后依次top、left、height、width；普通S32先算height+top，再width+left，调用5049A0。
2. 基础图选择的命中分支取top、height、left，先算bottom，再取width并算right；第二次命中独立重新取样，遵循同序。
3. 三种基础图分支汇合后drawY先于drawX；仍只提交一次521B10。
4. 覆盖阶段取top、left、height，先算height+top+2与top-2，再取width、算width+left+2与left-2；计算用正常S32，522180实参仍取低W16。

保持固定0x20记录／八S32字段及借用指针、descriptor16（偏移8..11全FF，其余零）、空指针与原早退、mode1常态／2激活／其它命中选图、state非零才第二次命中、mode严格0才覆盖／state2才step1、原图号与layer边界。查询命中后edge-held-pulse低BYTE各一次，最后两位复用同edge，返回0x40和八位累计；不改已有数值载体、RNG、命中与图像依赖正文。

不添加越界、溢出、回绕或故障模型／钳制／限制；保真实W8/W16边界转换，低位转换不是新的异常模拟。网络原格式明文、音频暂停及所有全局执行限制保持。

<a id="login-control-geometry-acceptance"></a>
## 可度量验收

| 验收项 | 通过条件 |
|---|---|
| 源码增量 | HEAD命名快照整源为前态，只有证据中的五处授权替换；两个入口之外正文保持 |
| 原版顺序 | 两完整根指令／字节／分页连续；四真实入向、三命中／两绘制原调用点及字段先后可追 |
| 输出合同 | 九位鼠标结果／三别名单读与edge复用保持；descriptor、模式早退、图号／layer、最终四低W16保持 |
| 身份与范围 | 仅新增两真实F与INPUT两成员；两X9B只填充用途；冻结定位／pending／缺口队列保 |
| 文档／任务 | 子SPEC、证据、两旧入口文档、映射／登记簿、U10／R04与唯一进度同步 |
| 静态汇总 | 集中源码／证据／完整机器与文档增量检查、measure及-Check、完整派生快照／UTF-8-LF与引用通过 |

以上只构成本次局部I/S。全部功能叶门禁、完整父与命中/绘制消费／未知别名与寿命、SDK-机器ABI、链接／像素与运行仍按原标准推进；不以局部通过关闭整体输入或SPEC。

<a id="login-control-geometry-source-state"></a>
## 源码与追踪状态

实施落点：QueryLoginControlState_5036F0一个片段、DrawLoginControl_503780四个片段，单文件两个既有体。新增映射1585／1586和两adapted处置，INPUT23→25；语义1587／有效1580、文档1698／3680（46.14%）、处置720／E828、R7 292余2769。U10／R04仅current／引用仍open，源码closed11和P05既有closed保持；完整功能叶0及globalfalse保持。

实施快照：`cg2d-re-login-control-normal-geometry-20261007`。实际静态结果在小步完成后补录，不执行构建、测试或运行。

本步静态结果：单整源五授权片段／两体普通S32与字段阶段、最终低W16及鼠标结果／模式保持，两完整原根176条439B与四入向／两X9B、完整机器与文档增量均通过；measure及-Check完成、完整派生快照／13文件UTF-8-LF与两I-S各14实际来源通过。未构建、测试或运行，不提升完整父／依赖、寿命、机器ABI或整体SPEC验收。
