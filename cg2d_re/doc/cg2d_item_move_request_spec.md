# 物品搬移与数量请求 SPEC（IMT01）

2026-10-11。沿IDE01实际请求/搬移及数量消费依赖，依子AGENTS第8条委托裁决。同一cg2d_re分支、不打tag。

<a id="item-move-baseline"></a>
## 完整原根与历史

510CE0=55条177B，4886E0=97条283B，488800=70条190B，4885A0=88条307B；合310条957B/七页，30E8十五目标/28跳八RET。十七CALL/各四后邻来自各原调用方，仅来源和局部关系；4885A0另有4887A3数量callback持址，不计普通CALL。旧934仅格式部分证据，保全部历史补完整根，不重复F。

当前四正文均与原合同相符，本步仅四定义/四声明八注释，不伪称新增行为或修复数值。当前网络采用原命令/字段/LF明文，原编码及外壳只作历史对照，不恢复已排除加密。

<a id="item-move-decision"></a>
## 实施合同与边界

IMT01丢物请求510CE0完整55条177B/一页、直接/拆分入口4886E0完整97条283B/两页、模式间移动488800完整70条190B/两页及数量消费4885A0完整88条307B/两页；合310条957B/七页，30E8十五目标/28跳八RET。十七入CALL及各四后邻只有来源/局部关系：请求3点2父、拆分10点8父、模式间3点2父、数量消费1点1父及一个callback data持址；不代全外父消费/数量框完整callback与owner验收。510CE0现正确正文保：原QpfE四S32字段x/y/group/index，每项先当前capacity W32按S32位模式→真实517120→重读buffer T*→5170D0，四次独立样本，不冻结buffer/长度或省调用；末重读buffer后传入socket→517480，现S32返回透传。当前517510/517120/5170D0/517480沿已批本机服务端原命令字段/LF明文承接，原编码/外壳只是历史对照，不恢复加密/旧外壳或新增校验协议。4886E0原source/target S32负或相同拒绝保；Ctrl状态原低BYTE bit2清为split，D67AE8 W32非零反转；source<100从唯一玩家count S32否则首组C44+8，target同门槛读used S16，两样本先于后续source/target<8、count>0、used0/split条件。直接分支先source全局再target→4885A0(-1)→声51，split分支先target全局再source→真实48AC60(window,element,原图244309,count,count,0,4885A0callback,1,0,0)；现既有void amount lambda承接真实4885A0副作用，不新造F/amount/失败回滚或把callback返回记全ABI等价。488800原target=-1先声51再source<0/target<-1拒绝，from/to按原低U8比较；from非零同mode先targetGroup后sourceGroup两次fresh71B0+1再516C90，非同sourceGroup/targetGroup0，from0/to非0只目标Group；两mode0不发送，原group/slot/-1数量及发送结果不消费保持，不合并同mode的两次读取或缓存跨分支。4885A0先读source，source7短路不先读target，任一7先47D490再重取source/target；双<100直接510EC0，source<8或41/38窗口均无返回原source，否则先41后38借固定WindowState地址，384B未清I三个S32→原+128输出/255转义→fresh userState W32/value14 W32→玩家Y后X CryptInt→当前socket512620/真实S32返回；不缓存转义前状态，不补窗口/索引/容量/amount保护。四现正确正文全部保持，四定义/四声明八合同注释，没有行为/算法/签名/调用次数变化；旧934格式历史保追加，三新F四adapted、INPUT/UI各三成员/NETWORK四，三限定S零I八open。全物品/数量/键盘/共享窗口与缓冲区生产、拆分callback完整消费/所有外父与callee/alias-owner寿命、SDK/生成取样/机器ABI/完整互通、实际移动拆分/声效/画面与运行待。MSVC/WAV-only/明文网络、48有界代码/完整枚举gates/六closure/总SPEC§10保持，无新API/helper/类型/字段/owner/保护/补零/默认值/新钳制/回滚/异常模型。

Ctrl源现已是uint8数组，无需人为再别名读取。分支原source/target门槛、count S32/used S16、玩家/C44来源和两个不同发布序保持。原两次同mode Group读取保独立真实调用；数量lambda是既有CPP适配，不新增独立F或声明原callback完整ABI已闭。

4885A0转义后再取窗口固定状态/玩家Y-X/socket，不冻结未知数量框生命周期；四字节非地址字段不拓宽，窗口/buffer只借已证T*。正常和失败各原结果保持，不补保护、回滚、默认值或清零。

<a id="item-move-acceptance"></a>
## 限定静态验收

核四连续完整根/30E8、原四容量和buffer采样、split及模式Group顺序、窗口41/38优先/转义后状态及真实返回。四整源只八注释/四正确正文、真实网络/数量提供者与类型/旧934格式历史保持。三新F/四adapted、三限定S零I/八open；全部callback消费/数量框owner、输入/缓冲区生产、callee/外父/SDK/生成读取/机器ABI/完整互通及实际操作运行待，完整枚举/六closure/总SPEC§10不提升。

只做必要静态文本及measure_cg2d.ps1/-Check，不构建/测试/运行/检查产物/摘要/新依赖。

<a id="item-move-source-state"></a>
## 当前状态

前提交“[#] 补齐物品拖动入口与回调合同”；拟提交“[#] 补齐物品搬移与数量请求合同”。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。

语义2312=2264F关联+48有界代码记录，函数处置1767/证据1566；INPUT142/UIRUNTIME179/NETWORK97，RENDER101/BATTLE430保持。文档关联预期2374/3680=64.51%仅关联率；184叶验收0、60任务关闭11开放49、六closure及完整门禁保持。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。
