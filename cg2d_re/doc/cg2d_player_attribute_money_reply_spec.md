# 玩家属性、金额与上限回复 SPEC（MPR01）

2026-10-11。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。沿子AGENTS第8条工程委托裁决，同一cg2d_re分支，不打tag。

<a id="player-money-baseline"></a>
## 原端合同

MPR01仅aPc主属性回复509650完整444条1350B/七页与UU附加属性回复509BA0完整232条655B/四页；合676条2005B/十一页、70E8六目标/60跳两RET。两根同9A6E0C W32门槛，主根非零按原S32位模式返回gate，附加根void原残留EAX门槛返回只副作用；当前dispatcher的两个TryDispatchReply<T>接同原命令/文本字段/LF明文并丢callback数值，原512E40两CALL后都XOR EAX,EAX，只局部接线/后邻不记完整dispatcher或机器ABI。flags由payload第1项原Base62，字段2起，仅对应(flags&(bit|1))非零才消费/推进，bit0表示全量而不是任何字段强制默认值；不改字段次序或插无掩码字段。主根六CryptInt依次hp/maxHp/fp/maxFp/value58/value68，掩码3/5/9/11H/2001H/4001H，每项真实ParseInteger→ConstructFromInt→临时mask W32/encoded W32/readCount BYTE依次先取样→按同序目标发布→Destroy→field++，保RNG次数/顺序，不复制vft/reserved或省构造；只现parseCrypt lambda一处加三局部样本，六原点同合同、不新造原F/helper。八values78[0..7] masks20..1000，后十二level/values78[8..13]/valueB8/valuesE4[0..3] masks8000..4000000；gold/valueFC门槛8000001H在两文字处理前持样，先gold/field++，titleIndex100门槛10000001H，然后name/ownTitle两256B未清局部Extract(cap255)→Unescape→strlen<=16才复制原17B目标，太长保旧值但field仍++，原失败提取由现既有提取器边界承接，不加校验/清零。末用同gold门槛再Parse当前field到value15BCC并真实S32透传，未命中返0，gold只变化或仅尾命中时不合并/缺字段回退。附加根firstTargets12S32顺序valuesD4[0..3]/valuesBC[0..5]/value104/valueF4，mask2..1000；2001H只原value00低W16，四middle F8/08/17C/16C从4000..20000，40001H先写walkSpeed，<=0原修正100再写，原action非空才同一次T*经既有SetYobiWalkSpeed直接S32发布+284，不新保护/钳制；last五valuesB0[0..1]/values15BD0[0..2]从80000..800000，三原FF02D0/D4/D8上限不新增限制。所有未命中字段保旧、字段推进/取值失败边界与复制/发布序保持；当前Money getters/UI存取丢钱共借同一PlayerRuntimeSlots，类型/七提供者/SetYobi声明、五既有读取消费源与七语义记录及两个当前路由行保持。两新F两adapted、NETWORK/UI各两成员/YOBI一、三限定S零I八open；完整玩家/名字/数值/资源初始化生产与所有其它消费、shared player/action/textbuf alias-owner寿命、SDK/生成读取和机器ABI、完整回复/互通与实际显示操作运行待。仅一共用取样片段/四注释，附加正确正文保持，不增加输入/容量/索引/空保护、补零/默认/回滚、异常溢出模型、字段/类型/API/helper/owner或新协议；MSVC/WAV-only/明文及全部完整门禁保持，整体未完成。

完整原指令、两个dispatcher CALL和六组CryptInt读写原点见[证据](cg2d_player_attribute_money_reply_evidence.json)。[SDM01存取](cg2d_storage_amount_exchange_spec.md)和[MDR01丢钱](cg2d_money_drop_request_spec.md)的金额getter直达同一玩家owner，当前真实调用/字段来源明确，不以此关闭全部属性或网络/owner/ABI门禁。

<a id="player-money-decision"></a>
## 方案及取舍

主根现parseCrypt共用lambda只加mask W32/encoded W32/count BYTE三个局部样本，先读完后按原序发布，六原位置一致；保构造/析构/RNG及字段推进。附加根正确正文保持，步速<=0→100及原action非空直写通过现Yobi边界，不新增空判断；原W16 tribe/三S32上限和未知/未命中字段保旧。原字符容量/16B长度门槛与MSVC CRT保持，currentdispatcher原命令/文本字段LF明文沿已批决议，旧机器返回与主S32/附加void分界保持。

<a id="player-money-acceptance"></a>
## 本步验收

完整676条2005B/十一页、70E8六目标/60跳两RET及两个真实入CALL/四后邻；六CryptInt组、31主消费项/23附加消费项（含复用gold门槛尾）按掩码及条件推进核对。两整源精确回放，一共用片段/四注释，附加正确正文、两类型七提供者/一旧声明、五读取消费源与七旧记录、两现路由行保持。两新F两adapted、三限定S零I八open；派生仅六metrics/两F/三功能/八任务，汇总/-Check必须终态0，全部完整门禁保持。未构建/测试/运行/产物/摘要。

<a id="player-money-source-state"></a>
## 当前状态

补齐主属性aPc和附加属性UU两个完整回复根676条2005B/十一页，70E8六目标/60跳两RET。主根六CryptInt的临时三字段先取样后发布已对齐；原掩码/字段推进、当前金额和三上限、文本长度门槛及步速同步保持。两新F两adapted、三限定S零I八open；当前金额读取/存取与丢钱消费共用真实PlayerRuntimeSlots，完整数据/owner、ABI及回复互通运行待。

语义2329=2281F关联+48有界代码，处置1787/证据1590；文档关联2391/3680=64.97%仅关联率。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

