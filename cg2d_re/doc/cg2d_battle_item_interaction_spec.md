# 战斗物品与目标发布 SPEC（BIP01）

2026-10-10。沿BEI01真实物品/目标依赖，依子AGENTS第8条工程委托裁决。同一cg2d_re分支、不打tag。

<a id="battle-item-baseline"></a>
## 完整原根与既有依赖

499C30完整1175条4105B/十九页，62E8十九目标/154跳两RET。两data引用是4C6EC0+50持址及原型槽3，不计普通CALL。旧900有格式及CAB30窄消费记录与现adapted处置/既有UI成员，全部历史保追加，不造新F或新处置。43C560沿BQI01完整266条目标选择合同复用，518C60沿BEI01完整九条空槽复用，提供者正文不改，不重取完整根。

<a id="battle-item-decision"></a>
## 实施合同与边界

BIP01战斗背包499C30完整1175条4105B/十九页、62E8十九目标/154跳两RET；两data持址为4C6EC0+50及原型槽3，不造普通CALL/全分发验收。先十五技能按skillId2002取首匹配与十五技术value8C连续正计usableLevel，原十五级S32表及玩家同源W32等级/业务上限保持；原skillIndex/usableLevel初值-1，bomb cost按skill+24+94H*level读S32，level0读experience、不加技能不存在/负等级保护或改变FP/RNG消费。S8mode正时二十玩家slot8..27，每轮fresh窗口/元素，五列四行左15/上48及禁图22738、占用BYTE/used W16、GBK不能装备/使用提示、层223、白/暗两descriptor、count>0的%3d/未清12B和低S16文字保持。普通event60仅dragkind!=2过滤；drop40只手位2/3统计非type7武器，真实518C60空槽/失败或原255u Q命令，窗口8000→选择槽5清0→MSVC格式/当前socket发送→动作1/声57→reset保持。hover21二十格命中、used与type/hasDescription Word/battleUseFlag低BYTE，medicine43/flag40000、bomb51/原level发布/skill成本、手位全空/FP<cost/5ABFCC!=1条件与短路保持；不能用支路原512B姓名复制→中间canUse写→尾cannotUse覆盖完整保，不清理或合并旧副作用。hover改变先页0再hover发布，双击focus27或0且used==1：bomb条件、装备type0..7/70..79及双手/空槽/未鉴定失败，容量bit8按原未加8的hover槽读，不改为实际背包槽。成功装备Q用hover+8 U32；普通使用先窗口8000→当前position S32→target W16按S16扩展为W32→43C560，沿已完整BQI01共享目标发布与原0/1返回，非零先selectedItem=hover+8/声77再pending S32=1和CC4D40 BYTE=1；零分支重新取position，selected发布→I两个U32格式/发送→Flush→动作1/声57再77。当前原命令/字段/LF明文保持，不增加发送失败回滚。单击/拖动bomb成本和used/flag/medicine门槛、48BF10与声51，右键pages快照→新page发布及原业务归零保持；两后框沿source>=8/hover>=0、不新增<28限制；末清textqueue3/593850空文本再逐used槽，bomb、不可使用/装备、未鉴定/medicine三个禁图门槛独立，允许原重复22738，occupancy后白/暗及第二%3d保持。三个逻辑组十二片段：六battleUseFlag低BYTE（五实际背包/一未加8容量索引）、五grid明确elementX→windowX/X→elementY→windowY/Y，区域一处按currentwindow→U32颜色→elements→windowY/X→elementX/height→elementY/width/原四边；均fresh原次数，不新增通用helper/新公共类型。原五MSVC格式点/12B与256B未清、GBK/两16B描述符/固定slot20和全部正常/失败/发布/RNG序保持。两整源仅上述十二片段和一定义/一声明两合同注释；旧900格式/CAB30历史保追加、既有adapted处置只追加合同/refs，不重复F或处置，既有UI成员不重复，其余INPUT/RENDER/NETWORK/BATTLE各加本根；五限定S零I九open。完整技能/物品/占用/目标参数生产、动态callback/窗口/后续pending消费者/所有callee/alias-owner寿命、SDK/生成BYTE/机器ABI/实际战斗操作画面声音与运行待。48有界代码/所有完整枚举gates/六closure/总SPEC§10保持；无新API/helper/结构/字段/owner/保护/补零/默认/新钳制/回滚/异常模型，MSVC/WAV-only/明文网络保持。

十五级表/技能技术及成本现正文正确；原未加8容量标志与level0经验读取保留，不能按推测“修正”业务。物品不可使用支路原中间提示与尾覆盖保持。五grid每次fresh借window/element，先elementX/windowX及X后elementY/windowY及Y；区域只持本处U32颜色与八字段，五格子算法、低S16文字/低U16描边和当前GBK/MSVC保持。

本步仅追到selected/pending及命令选择/发送/Flush发布。后续497100/497770/498520处理、完整参数生产和全生命周期仍待，不以root正文覆盖全战斗/完整网络或整功能验收。

<a id="battle-item-acceptance"></a>
## 限定静态验收

核完整1175条/62E8、六BYTE/五grid/区域取序、五格式点及target S16扩展/selected/pending发布、原失败和重复副作用。两整源精确差异/两注释，技能/物品/窗口类型、BQI01/BEI01提供者与旧900/CAB30记录/处置和既有UI成员保。无新增F/处置、四功能新增本根、五限定S零I/九open；全部生产/后续消费者、callback/callee/alias-owner寿命、SDK/生成取样/实际交互像素声音与运行待。完整枚举/六closure/总SPEC§10保持。

只做必要静态文本及measure_cg2d.ps1/-Check，不构建/测试/运行/检查产物/摘要/新依赖。

<a id="battle-item-source-state"></a>
## 当前状态

前提交“[#] 补齐战斗装备与背包空槽合同”；拟提交“[#] 补齐战斗物品与目标发布合同”。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。

语义2306=2258F关联+48有界代码记录、处置1760均保持，证据1558；INPUT136/UIRUNTIME173/RENDER100/NETWORK92/BATTLE428。文档关联2368/3680=64.35%仅关联率保持；184叶验收0、60任务关闭11开放49、六closure及完整门禁保持。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。
