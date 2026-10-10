# 装备交互与属性回复 SPEC（IER01）

2026-10-10。沿SAP01属性页生产消费及真实装备依赖推进，依子AGENTS第8条委托裁决。同一cg2d_re分支、不打tag。

<a id="equipment-reply-baseline"></a>
## 原端证据

489760完整584条1919B/十页，510490完整78条222B/两页；合662条2141B/十二页、61E8二十七目标/84跳五RET。七装备data引用为六代码内持址及原型槽17，不伪作CALL；唯一回复原CALL为516B47，516AE7是TgX命令头准备点，不混淆。原回复分发邻窗及返回后OR EAX-1有据，不代完整512E40父验收。旧919已有部分悬浮格式证，保全历史追加。八玩家/六宠物提示W32表共56B逐项保持，文字沿已批GBK，不恢复原Big5。

<a id="equipment-reply-decision"></a>
## 实施合同与边界

IER01库存装备489760及属性回复510490两完整根662条2141B/十二页、61E8二十七目标/84跳五RET，七装备data引用（六代码内持址/一原型槽17）及一个回复入CALL原516B47有据。489760先真实4C72C0借items/count、低AL失败返S32零；原两16B descriptor初始化/FF与80FF/byte13=4保持。低BYTE事件60门禁仅dragkind!=2或source>=100时清两bit/consumed1；drop40按五坐标/动态count逐槽无占用及48半开命中、S8模式/6边界分别同group搬移或跨group调用，原每出口reset及参数/失败结果不消费保持。hover21写原3F8后固定item/命中索引，BYTE模式选八玩家/六宠物GBK提示、原后续清hover不重算固定item；bit2发布窗口2000，无hover且回复used S32非零时声51→fresh窗口/3FC1/槽17回调489EE0后返既有consumed。悬浮改变先clear页，再selectedItem S32快照→发布hover；双击按原Word used/低BYTE canUseInField、特殊图26569双fresh窗口callback/声77/U8占用递增/原reset与475DA0；普通物品动作标志按BYTE已有接口、440B50、S8mode，发布selected后宠物group或玩家Y后X CryptInt原明文请求，现网络/参数/声效和原reset保。hover单击/右键页递增与原pages业务归零、拖框/原hitIndex绘框、used/Word flags100-200两[kok]/[han]共MSVC格式/未清256B保持。末动态count逐used槽x/y→低BYTE flags80→原图22739→fresh occupancy后暗图或普通图，不补索引/count保护、补零或新的失败回滚。四组四处仅两物品字段低BYTE、动作BYTE既有接口及selected在hover发布前取样；其它装备正文与提示表、两descriptor/owner、共享item和完整已批MSVC格式保持。510490正文正确保、unusedSocket地址宽度和借text T*不改：首逗号field1 max31失败先由既有提取器写text04首字节NUL，再本根只used0，不清其余字段；成功used1先于text04反转义，field2 max31失败由既有提取器写text25首字节NUL、返回弃→text25反转义→仅单空格加NUL比较相等才hasText24=0（空串为1），field3→value48 S32，按field4..27顺序写24S32值，无全局门禁/清空整记录/新字段校验。原回复分发先maxlen→workspace→text解码→callback/socket，返回残留由dispatcher后OR EAX-1覆盖；现TgX既有TryDispatchReply弃callback返回与当前本机服务端原命令/原字段/LF明文保持，不以原dispatcher-1证明当前完整分发结果或机器ABI。七装备data不是CALL，不据本步全父/原型/回调分发验收；回复入CALL仅邻窗，装备固定借用/全数据生产、动态callback/slot/拖拽与所有callee/网络send/失败、alias-owner寿命、SDK/生成BYTE/实际操作图像声音和运行待。两定义/两声明四合同注释，四源全文限定diff及正确回复/类型/既有callee/分发/原型绑定保持；旧919格式历史追加、一新F两adapted/四限定S零I八open，48有界代码/所有完整枚举gates/六closure/总SPEC§10保持；MSVC/WAV-only/明文网络保，无新公开API/helper/布局/字段/owner/保护/默认值/补零/新钳制/回滚/异常模型。

依委托仅修四字段读取合同。两物品字段既有W16储宽不改，以unsigned char别名读取原低BYTE；动作标志复用已有ReadPlayerActionFlagsByte实际提供者，不新增工具。selected局部S32样本位于hover写回之前，其余fresh数据保持。普通字段数值未据本步宣称已发生差异；不改变全部拖拽、发包或失败业务。

属性回复正确正文保持。首31字节字段提取失败由既有解析器写目标首字节NUL，本根随后只清used；第二字段提取失败同样写text25首字节NUL，仍继续反转义和后续字段处理，不增校验或整记录清零。原空间文字严格等于单空格才hasText=0，空串为1；现typed字段偏移/宽度与消费者SAP01一致，不增加私有记录或新protocol。网络沿既有原命令、字段与LF明文合同。

<a id="equipment-reply-acceptance"></a>
## 限定静态验收

核两完整原根/61E8、四实际读取合同、回复先发布used/逐字段生产、原数据持址/命令解码邻窗和返回覆写。四整源限定diff、回复正确正文、类型/既有callee/动作字节提供者/分发及原型绑定保持。旧919只追加、一新F/两adapted，四限定S零I/八open；全部生产消费/动态callback/网络/alias-owner寿命、SDK/生成取样/交互像素声音与运行待，完整枚举/六closure/总SPEC§10不提升。

只做必要静态文本及measure_cg2d.ps1/-Check，不构建/测试/运行/检查产物/摘要或引依赖。

<a id="equipment-reply-source-state"></a>
## 当前状态

前提交“[#] 补齐属性页与宠物页签合同”；拟提交“[#] 补齐装备交互与属性回复合同”。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。

语义2305=2257F关联+48有界代码记录，函数处置1758/证据1548，INPUT134/UIRUNTIME172/RENDER98/NETWORK90；文档关联预期2367/3680=64.32%仅关联率。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。
