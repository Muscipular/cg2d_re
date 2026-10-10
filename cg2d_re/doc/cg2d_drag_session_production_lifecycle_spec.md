# 拖动会话生产、焦点绑定与清理 SPEC（DLP01）

2026-10-10。沿现鼠标分发/更新父补实际会话生产到清理合同，依子AGENTS第8条裁决。同一cg2d_re分支，不打tag。

<a id="drag-session-baseline"></a>
## 原根与当前提供者

六原根完整129条483B/六页、零E8/两寄存器回调、十三跳十一RET，116直接CALL/50父只核调用点和四后邻指令，不代完整父或全部返回消费。现有定义均在WindowRuntime.cpp；私有声明在WindowRuntimeState.hpp，取消声明在既有公开WindowRuntime.hpp。

| 入口（尾不含） | 指令/字节 | 实际调用点 |
| --- | ---: | ---: |
| 475DA0–475DEF | 21/79 | 11 |
| 476490–4764FC | 25/108 | 9 |
| 476500–476562 | 26/98 | 37 |
| 4763B0–4763D5 | 11/37 | 1 |
| 4763E0–476401 | 7/33 | 49 |
| 476410–476490 | 39/128 | 9 |

原476444经ECX回调，476525经EAX回调，两者参数message/flags/source保持。现WindowDragCallback为三个S32参数的非拥有原生函数指针，仅拓宽地址，不造回调表或owner。旧473560/473E40和473F80合同复用，不重采或改正文。

<a id="drag-session-decision"></a>
## 会话生产及清理顺序

475DA0仅入口phase==0且kind非零时启动；依kind→当前窗口ID取样→callback→当前elementIndex取样→focusWindow→source→focusIndex→phase1发布，成功S32 1、失败0。476490同样门槛，依kind/source/callback→当前windowID样本→S16 x→当前elementIndex样本→S16 y→focusWindow/focusIndex→phase3发布。负kind不拒绝，两个坐标不扩宽或钳制。

4763B0只有已有kind非零才写两个S32焦点槽，成功1，否则0；不修改phase。4763E0总按focusIndex=-1→kind=0→focusWindow=0→callback=null→phase=0清五槽并S32返0；保来源与坐标，不能合并为八槽清理。

476410入口phase0早退；其它phase保存callback，存在时入口phase1发message1、其它发message4，flags0/source原值；回调后才重取phase，无回调保入口样本。随后按kind/source/callback/x/y/focusWindow/focusIndex清理，最后依该phase==1进入2，否则6。回调可能改phase，不能用入口值替代。

476500只入口phase4/5才可选回调message3/flags0/source；之后按kind/source/callback/x/y/focusWindow/phase0/focusIndex=-1清理。回调后不再判新phase，不能套用取消或五槽重置。原callback空门槛保持。

六正确正文及返回边界保持，只增加六定义/六真实声明共十二注释。四函数S32结果有据；两个void沿现接口承载状态副作用，原EAX残留及全父是否丢弃返回不因四后邻快照称完整机器ABI等价。零body补丁/零I；不新增API/helper、字段、布局、owner、状态副本、保护、补零、默认值、限制、钳制、回滚或异常模型。

<a id="drag-session-acceptance"></a>
## 限定静态验收

核六连续原根及各失败/成功、两回调、所有字段宽度/写序、116实际调用点和父，三整源精确差异仅十二注释，六body/共享十个声明与回调真实类型保持。只依据实际原指令与源码，不执行状态/算法样例。

新增六语义F/六adapted处置，INPUT/UIRUNTIME各六成员，两限定S零I，十任务仍open。全外父、间接回调/载荷与alias-owner寿命、两个void的所有原EAX消费及SDK/机器ABI、真实拖动/取消/完成和退出/运行仍待；48有界代码、全部完整枚举/gates/六closure和总SPEC§10保持。可选图形1020EC4生产缺口独立保留，不用零值、无写xref或恒空5222D0决议替代它。

只做必要静态文本和measure_cg2d.ps1/-Check。不构建、测试、运行客户端或算法、检查产物、计算摘要或引依赖；网络/文本、MSVC CRT及WAV-only边界保持。

<a id="drag-session-source-state"></a>
## 当前状态

前提交“[#] 补齐悬停文本消费并对齐颜色读取”；拟提交“[#] 补齐拖动会话生产与清理合同”。合同对齐、登记、静态汇总/-Check终态退出0及派生范围核对通过。语义2277=2229F关联+48有界代码，处置1720/证据1517、INPUT97/UIRUNTIME134；文档关联2339/3680=63.56%仅关联率。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持，整体active/未完成。
