#pragma once

#include "base.hpp"

// 本组内部共享声明；跨模块接口见 cg/Yobi/YobiRuntime.hpp。

namespace cg::yobi::detail {
// 原函数：0x0040DBE0。
void UpdateBattleAction103_40DBE0(YobiAction* action); // BCPY01：独立复制体/七动作/效果四值按原序取样。
// 原函数：0x0040E4B0。
void UpdateBattleAction109_40E4B0(YobiAction* action); // BKE01：独立复制体25/110/107及四值4-flags-3-position读取，设备后完整flags与双推进保持。
// 原函数：0x0040EB00。
void UpdateBattleAction112_40EB00(YobiAction* action); // BKE01：双复制体/第三列表选图、一次事件/三Y-X及运动读序、两Append不同四值顺序。
// 原函数：0x0040BD60。
void UpdateBattleAction41_40BD60(YobiAction* action); // BWT01：静态两22项表，固定point先Y后X。
// 原函数：0x0040BDB0。
void UpdateBattleAction42_40BDB0(YobiAction* action); // BWT01：全局目标先Y后X，转向本对象。
// 原函数：0x0040BE30。
std::int32_t UpdateBattleAction43_40BE30(YobiAction* action); // BWT01：有other才转向，共同100/0/播放返回。
// 原函数：0x0040BDF0。
void UpdateBattleAction44_40BDF0(YobiAction* action); // BWT01：目标朝本对象转向，推进本对象。
// 原函数：0x0040B130。
void UpdateBattleAction45_40B130(YobiAction* action); // BDE01：三字段读齐/按原序发布、临时销毁后推进。
// 原函数：0x0040B200。
void UpdateBattleAction46_40B200(YobiAction* action); // BDE01：骑乘短路/缩放/清理及不同219-48-创建推进分支。
// 原函数：0x0040B3F0。
void UpdateBattleAction47_40B3F0(YobiAction* action); // BDE01：整FDC置2后推进。
// 原函数：0x0040B490。
void UpdateBattleAction48_40B490(YobiAction* action); // BDE01：两设备退场，旧alpha前置/新alpha重取。
// 原函数：0x0040B5B0。
void UpdateBattleAction49_40B5B0(YobiAction* action); // BDE01：1024严格2才清零/切51。
// 原函数：0x0040B3C0。
void UpdateBattleAction50_40B3C0(YobiAction* action); // BDE01：仅动作10/严格终帧推进。
// 原函数：0x0040BE70。
YobiAction* UpdateBattleAction51_40BE70(YobiAction* action); // BDE01：清全局槽先标死，保T*返回。
// 原函数：0x0040BE90。
void UpdateBattleAction52_40BE90(YobiAction* action); // BDE01：固定caster/逐次目标，追加13-47-51后启动。
// 原函数：0x0040BF20。
std::int32_t UpdateBattleAction53_40BF20(YobiAction* action); // BDE01：未舍入缩放/创建后重读，末共同播放返回。
// 原函数：0x0040BFE0。
std::int32_t UpdateBattleAction54_40BFE0(YobiAction* action); // BSA01：未舍入放大，type23后精确1/重查设备。
// 原函数：0x0040C0C0。
void UpdateBattleAction55_40C0C0(YobiAction* action); // BSA01：播放→实际扣FP→推进。
// 原函数：0x0040C160。
void UpdateBattleAction56_40C160(YobiAction* action); // BSA01：动作5终帧清index/remain/event。
// 原函数：0x0040C1A0。
void UpdateBattleAction57_40C1A0(YobiAction* action); // BSA01：动作13终帧只清index/remain。
// 原函数：0x0040C1E0。
void UpdateBattleAction58_40C1E0(YobiAction* action); // BSA01：动作14终帧只清index/remain。
// 原函数：0x0040C240。
void UpdateBattleAction59_40C240(YobiAction* action); // BSA01：动作15终帧只清index/remain。
// 原函数：0x0040C220。
void UpdateBattleAction60_40C220(YobiAction* action); // BSA01：仅动作15/推进，无播放。
// 原函数：0x0040C280。
void UpdateBattleAction61_40C280(YobiAction* action); // BSA01：动作12终帧推进，不清帧。
// 原函数：0x0040C2B0。
void UpdateBattleAction62_40C2B0(YobiAction* action); // BSA01：动作18终帧推进，不清帧。
// 原函数：0x0040C2E0。
std::int32_t UpdateBattleAction63_40C2E0(YobiAction* action); // BSA01：旧70推进，20/50方向与互斥播放。
// 原函数：0x0040CBB0。
void UpdateBattleAction64_40CBB0(YobiAction* action); // BTE01：仅动作0/推进，无播放。
// 原函数：0x0040C380。
std::int32_t UpdateBattleAction65_40C380(YobiAction* action); // BTE01：旧70/八计数点、fresh方向与互斥播放。
// 原函数：0x0040C480。
void UpdateBattleAction66_40C480(YobiAction* action); // BTE01：1014图号/推进后清事件。
// 原函数：0x004106B0。
void UpdateBattleAction67_4106B0(YobiAction* action); // BTE01：入口事件/逐目标启动/哨兵额外推进。
// 原函数：0x0040C0E0。
void UpdateBattleAction68_40C0E0(YobiAction* action); // BTE01：创建后BYTE门禁/总推进。
// 原函数：0x0040C120。
void UpdateBattleAction69_40C120(YobiAction* action); // BTE01：30帧创建后BYTE门禁/总推进。
// 原函数：0x0040C4D0。
void UpdateBattleAction71_40C4D0(YobiAction* action); // BTE01：逐目标启动/哨兵推进，原无播放。
// 原函数：0x0040CAA0。
void UpdateBattleAction72_40CAA0(YobiAction* action); // BTE01：caster原00计数/源推进。
// 原函数：0x0040CB10。
void UpdateBattleAction75_40CB10(YobiAction* action); // BCLC01：1004置1/推进，无播放。
// 原函数：0x0040CB30。
void UpdateBattleAction76_40CB30(YobiAction* action); // BCLC01：1004清零/推进，无播放。
// 原函数：0x0040CB50。
void UpdateBattleAction77_40CB50(YobiAction* action); // BCLC01：查询后仅置设备对应绘制位。
// 原函数：0x0040CB80。
void UpdateBattleAction78_40CB80(YobiAction* action); // BCLC01：查询后仅清设备对应绘制位。
// 原函数：0x0040CBD0。
void UpdateBattleAction79_40CBD0(YobiAction* action); // BCLC01：五槽逐次清理、两标志持样/发布后推进。
// 原函数：0x0040CC80。
void UpdateBattleAction80_40CC80(YobiAction* action); // BCLC01：X先持样/独立1018音效。
// 原函数：0x0040CCB0。
void UpdateBattleAction81_40CCB0(YobiAction* action); // BCLC01：X先持样/独立101C音效。
// 原函数：0x0040CCE0。
void UpdateBattleAction82_40CCE0(YobiAction* action); // BCLC01：X先持样/独立1020音效。
// 原函数：0x0040CD10。
std::int32_t UpdateBattleAction83_40CD10(YobiAction* action); // BCLC01：未舍入阈值/推进后重取载荷置1000位。
// 原函数：0x0040CD70。
void UpdateBattleAction84_40CD70(YobiAction* action); // BRAF01：新载荷清位/入口载荷BYTE门禁。
// 原函数：0x0040CDE0。
void UpdateBattleAction85_40CDE0(YobiAction* action); // BRAF01：放大旋转/2002名称表/入口BYTE门禁。
// 原函数：0x0040C520。
void UpdateBattleAction86_40C520(YobiAction* action); // BRAF01：固定视觉与动态HP/phase目标，临时复制原读序。
// 原函数：0x0040C780。
std::int32_t UpdateBattleAction87_40C780(YobiAction* action); // BRAF01：十位置RNG序/新270清零推进/末播放。
// 原函数：0x0040C8D0。
void UpdateBattleAction88_40C8D0(YobiAction* action); // BRAF01：振幅6/启用1/推进。
// 原函数：0x0040C8F0。
void UpdateBattleAction89_40C8F0(YobiAction* action); // BRAF01：仅启用0/推进。
// 原函数：0x0040C910。
void UpdateBattleAction90_40C910(YobiAction* action); // BRAF01：逐目标/事件重取/同帧延迟减1/四边界退出。
// 原函数：0x0040CF30。
std::int32_t UpdateBattleAction91_40CF30(YobiAction* action); // BRAF01：对象flags置2→推进→播放返回。
// 原函数：0x0040CF50。
std::int32_t UpdateBattleAction92_40CF50(YobiAction* action); // BRAF01：对象flags清2→推进→播放返回。
// 原函数：0x0040CF70。
void UpdateBattleAction93_40CF70(YobiAction* action); // BSTL01：活动数量先判，再清alive并推进。
// 原函数：0x0040D010。
void UpdateBattleAction94_40D010(YobiAction* action); // BSTL01：四次独立目标读取/单圈角度/50步进。
// 原函数：0x0040D0B0。
void UpdateBattleAction95_40D0B0(YobiAction* action); // BSTL01：旧16门禁/推进后清FF0与原HP零复制读序。
// 原函数：0x0040D170。
void UpdateBattleAction96_40D170(YobiAction* action); // BSTL01：当前目标hidden置0/source推进。
// 原函数：0x0040D1A0。
void UpdateBattleAction97_40D1A0(YobiAction* action); // BSTL01：当前目标hidden置1/source推进。
// 原函数：0x0040D1D0。
void UpdateBattleAction98_40D1D0(YobiAction* action); // BSTL01：双选图门禁/两次skill读取/铺格生产。
// 原函数：0x0040D390。
std::int32_t UpdateBattleAction99_40D390(YobiAction* action); // BCPY01：rate25→推进→清两帧字段→返回0。
// 原函数：0x0040D3C0。
void UpdateBattleAction100_40D3C0(YobiAction* action); // BCPY01：入口frameIndex先目标，S8序列/分档/fresh结束哨兵。
// 原函数：0x0040D580。
std::int32_t UpdateBattleAction101_40D580(YobiAction* action); // BCPY01：位置逐次重取，两侧中心/面对点及末播放返回。
// 原函数：0x0040D6C0。
void UpdateBattleAction102_40D6C0(YobiAction* action); // BCPY01：完整逐效果伤害/回复与原BYTE消费、动态HP零复制。
// 原函数：0x0040DC90。
void UpdateBattleAction104_40DC90(YobiAction* action); // BIE01：先读全局玩家位，成功分支重取；按原CryptInt前两W32显示并保额外效果推进。
// 原函数：0x0040DE40。
std::int32_t UpdateBattleAction105_40DE40(YobiAction* action); // BIE01：游标先源位置，两次门禁独立；旧计数>=120后推进源/入口目标，末播放返回。
// 原函数：0x0040DF50。
void UpdateBattleAction106_40DF50(YobiAction* action); // BIE01：完整动作106仅角色动作13→Advance，无播放/速率/帧清零。
// 原函数：0x0040DF70。
YobiAction* UpdateBattleAction107_40DF70(YobiAction* action); // BIE01：完整动作107调用MarkDeath并返回真实T*；物理释放沿scheduler。
// 原函数：0x0040DF80。
void UpdateBattleAction108_40DF80(YobiAction* action); // BIE01：入口事件持样/逐效果do，13 BYTE与临时HP复制读序；免疫只按完整flags。
// 原函数：0x0040E560。
void UpdateBattleAction110_40E560(YobiAction* action); // BKE01：旧FF0/1038蓄力及两侧HP零逐项复制；五BYTE与真实事件循环/旧FD0收尾。
// 原函数：0x0040E970。
std::int32_t UpdateBattleAction111_40E970(YobiAction* action); // BKE01：两次fresh BYTE FDC/40次散射，原角度与四边界后仍Move/Play S32。
// 原函数：0x0040EFE0。
void UpdateBattleAction113_40EFE0(YobiAction* action); // BKE01：worldY先载荷；两轴各自先借用父再数值，旧294/FF0及末fresh父标死保持。
// 原函数：0x0040F0A0。
void UpdateBattleAction114_40F0A0(YobiAction* action); // BCVI01：缓存静态零初值/负HP复制原读序，完整动作合同沿BPR01。
// 原函数：0x0040FB80。
void UpdateBattleAction115_40FB80(YobiAction* action); // BFW01：固定载荷/完整flags的CH门禁，工厂后重取目标隐藏；全路径phase162。
// 原函数：0x0040FC10。
void UpdateBattleFactoryAction_40FC10(YobiAction* action); // 31动作共享30种映射；118不创建但仍phase162。
// 原函数：0x0040FF90。
void UpdateBattleActions127To129_40FF90(YobiAction* action); // BFW01：完整FDC持样/条件播放，后取phase选7/8/9末参0，全路径推进。
// 原函数：0x0040FF40。
void UpdateBattleAction138_40FF40(YobiAction* action); // BFW01：完整FDC持样/条件播放，后取1024创建type20，全路径phase162。
// 原函数：0x00410000。
void UpdateBattleAction139_410000(YobiAction* action); // BFW01：事件/完成两次独立特效指针持样，98单次属性；405不同取样/四临时复制原序。
// 原函数：0x00410730。
void UpdateBattleAction140_410730(YobiAction* action); // BFL01：入口技能持样，特效末参0或先推进61–63目标，最后推进源。
// 原函数：0x00410E90。
void UpdateBattleAction141_410E90(YobiAction* action); // 13D8/13DC与技能选择减益图，重读FDC决定播放。
// 原函数：0x00410F70。
void UpdateBattleAction142_410F70(YobiAction* action); // BFL01原播放/推进后，G03同步减益类型并清变化标记。
// 原函数：0x00410FC0。
void UpdateBattleAction143_410FC0(YobiAction* action); // 两型状态图号/13E0索引，调用后条件播放并推进。
// 原函数：0x00411240。
void UpdateBattleAction144_411240(YobiAction* action); // BFL01：无空门禁标死1068，fresh完整FDC取样后先清槽再发布掩码值，播放/推进。
// 原函数：0x00411290。
void UpdateBattleAction145_411290(YobiAction* action); // BFL01：一次技能选择四图，创建返回弃置；其它技能亦共同推进，无播放。
// 原函数：0x00411320。
void UpdateBattleAction147_411320(YobiAction* action); // BAS01：完整FDC的AH800开关；创建失败仍置位，标死后fresh取旗位、先清105C再发布。
// 原函数：0x0040FEF0。
void UpdateBattleAction148_40FEF0(YobiAction* action); // BAS01：完整FDC持样/AL60播放门禁，后取1024创建type33，全路径phase162。
// 原函数：0x0040F790。
void UpdateBattleAction154_40F790(YobiAction* action); // type24发布1060，等待特效phase严格1。
// 原函数：0x0040F800。
void UpdateBattleAction155_40F800(YobiAction* action); // 两次独立重播种/缩放目标与方向差异音效，收尾令1060.phase=2。
// 原函数：0x0040FB30。
void UpdateBattleAction156_40FB30(YobiAction* action); // 1060.phase严格3才标死、清槽并推进。
// 原函数：0x00412870。
void SetBattleAction208Target_412870(YobiAction* action); // 按目标坐标界线写FE8/FEC，最后phase6。
// 原函数：0x00412900。
std::int32_t UpdateBattleAction209_412900(YobiAction* action); // 完整动作209，效果按调用边界重读。
// 原函数：0x00412F70。
YobiAction* UpdateBattleAction210_412F70(YobiAction* action); // 追近后播放，再取一次随机创建位置特效。
// 原函数：0x00413070。
void ApplyBattleSceneEffects_413070(YobiAction* action); // 动作211：四独立W16写，效果先于动作推进。
// 原函数：0x00413110。
void UpdateBattleAction212_413110(YobiAction* action); // 新计数与当前flags按S32比较，达到才推进效果/动作。
// 原函数：0x004131B0。
std::int32_t UpdateBattleAction213_4131B0(YobiAction* action); // 动画结束顺序清四附件，保留分支返回值。
// 原函数：0x004132B0。
void UpdateBattleAction214_4132B0(YobiAction* action); // 两设备分支：闪烁后建特效，或先快照/后重读30F衰减。
// 原函数：0x004133D0。
void UpdateBattleAction215_4133D0(YobiAction* action); // 1024严格为2才清槽并推进动作。
// 原函数：0x00413400。
void UpdateBattleAction216_413400(YobiAction* action); // 施法特效、帧事件逐项消费及播放后独立平铺尾。
// 原函数：0x00413550。
void UpdateBattleAction218_413550(YobiAction* action); // 克隆换图、双对象渐变及13AC/13B0完成回写。
// 原函数：0x00413750。
void UpdateBattleAction219_413750(YobiAction* action); // 恢复原图后按FDC走48或居中特效/动作推进。
// 原函数：0x00413980。
void UpdateBattleAction220_413980(YobiAction* action); // 恢复原图、动作10终帧及透明度后转214。
// 原函数：0x00413B40。
void UpdateBattleAction221_413B40(YobiAction* action); // 效果双次推进、固定目标HP及原源HP清零差异。
// 原函数：0x00413E90。
std::int32_t UpdateBattleAction222_413E90(YobiAction* action); // 追踪/返回转向，标死后仍移动播放并返回帧结果。
// 原函数：0x00413FE0。
void UpdateBattleAction223_413FE0(YobiAction* action); // 发布或先刷新后清宠物快照槽，再按效果/动作顺序推进。
// 原函数：0x0040B180。
void UpdateBattleAction251_40B180(YobiAction* action); // 两次独立CryptInt(1)，按mask/encoded/count分别写HP和FP。
// 原函数：0x0040FE50。
void UpdateBattleAction253_40FE50(YobiAction* action); // 播放后判phase253/1024，创建type43后总是推进。
// 原函数：0x0040FEB0。
void UpdateBattleAction254_40FEB0(YobiAction* action); // 仅+10非空才标死并清+10/1024，空槽保留1024。

} // namespace cg::yobi::detail
