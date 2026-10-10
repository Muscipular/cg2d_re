# 物品拖动入口与回调 SPEC（IDE01）

2026-10-10。沿物品/目标生产的真实拖动依赖推进，依子AGENTS第8条工程委托裁决。同一cg2d_re分支、不打tag。

<a id="item-drag-baseline"></a>
## 完整原根与既有提供者

48BF10完整九条30B/一页、48BC90完整189条635B/三页、48BB30完整21条54B/一页；合219条719B/五页，23E8十一目标/33跳八RET。入口十三CALL来自十三父；分类四CALL来自两父，本回调两点、4B2730两点，共十七CALL/各四邻窗只登记来源，不代完整外父结果消费。回调两data持址为48BF18及48BE27，非普通CALL。三根现正文真实存在，但此前无独立F语义/处置。

先查明497100/BQK01、497770/BQL01及498520/BQU01已有完整原证，保既有合同与源，不重复取完整根。475DA0/476490/4763E0/476410沿DLP01完整会话提供者复用，真实reset/setup/cancel及返回保持。

<a id="item-drag-decision"></a>
## 实施合同与边界

IDE01物品拖动入口48BF10九条30B/一页、48BC90回调189条635B/三页、48BB30分类21条54B/一页；合219条719B/五页、23E8十一目标/33跳八RET。入口十三入CALL来自十三父，分类四CALL来自两父（回调两点/另父两点）；十七CALL/各四后邻只登记来源，不代全外父消费。回调两data持址分别入口及本体475490参数，不计普通CALL/整动态分发验收。48BF10先写mode S32→真实475DA0(kind2,sourceIndex,48BC90)，原S32返回透传，失败不回滚mode；现正确正文保持。48BB30原signed槽0..7→1、8..27→2、100..119→3、其它0，完整W32类型非bool，业务区间保、现正确正文保持，不把分类扩成所有调用方拒绝门禁。48BC90原16B descriptor零模板/80FFFFFF/byte13=4保持；message!=0只message2且mode W32非零/动作flags低BYTE80清/440B50零才发送，S8库存mode正且source<6 group=71B0+1否则0；真实玩家Y后X CryptInt→当前socket510CE0原参数/明文，返回不消费，不新增发送失败回滚。message0首次fresh分类后按kind1库存S8mode选择玩家/宠物、kind2玩家、kind3首组CC4D58+(source-100)*C44的used W16，失效只原reset而不提前退出；未知分类也继续鼠标和cursor阶段。鼠标低BYTE bit1先reset→mouseX后Y各原低S16→475490(kind2,source,x,y,同回调)；再fresh低BYTE bit2时原CancelWindowDrag，之后仍RequestCursor(1,0)→第二次fresh分类，不冻结入口分类/mode/bitmap，也不因取消停绘。分类1重取S8mode后先持descriptor地址再按玩家/宠物读S32 image（宠物真实71B0）；分类2先descriptor地址后玩家image，分类3先descriptor地址后CC4D58+4 memcpy S32 image；无匹配原返回。最后mouseY后X各真实CryptInt→DrawGraphic层230/原image/同半透明descriptor。两逻辑组六处仅三BYTE（动作现有字节接口及两独立click取样）与三分支descriptor持址（kind1先mode再地址），非拥有地址/字段位宽与原player/pet/detail区别、S16/S32/调用/读序和正常失败返回保持；两正确正文不改，仅三定义/三声明六注释。DLP01四真实会话提供者与既有SLC/PSP/IER/BEI/BIP及BQU/BQK/BQL源保持，不重取已验完整根；后续目标三消费者已具完整合同，仅定位不虚称本步重验。三新F/三adapted、五限定S零I/九open；十七全调用点的外父/动态回调完整消费、detail/pet/玩家数据生产及共享owner/SDK/生成BYTE/机器ABI/真实拖放取消/画面声效与运行待。不新增API/通用helper/类型/字段/owner/保护/默认值/补零/钳制/回滚/异常模型；MSVC/WAV-only/当前原命令字段LF明文、48有界代码/完整枚举gates/六closure/总SPEC§10保持。

入口与signed分类正确正文保持，只补合同。回调两逻辑组六处按原字节和持址取序，不造新helper；S32 mode正常和失败都先发布，无补回滚。message0失效/左键reset或右键取消并不提前返回，cursor及第二次fresh分类/绘图继续；已知业务区间是原分类逻辑，不新增所有调用方索引保护或冻结detail整布局。

三绘图分支分别赋非拥有descriptor指针，不给默认NULL或补值；kind1原先读取mode低S8再持地址，宠物71B0后借image，最后鼠标Y/X独立CryptInt读取。客户端仍只播放WAV，message2沿现有原命令/字段/LF明文提供者，不改其它网络业务。

<a id="item-drag-acceptance"></a>
## 限定静态验收

核三完整根/23E8、三BYTE/三descriptor分支、S32入口透传/分类、W16 used/S32 image C44消费与鼠标轴/两fresh分类、十七CALL来源/两data持址。两整源精确差异/六注释，两正确正文和既有DLP/目标消费者/类型保持。三新F三adapted/五限定S零I九open；完整调用方/动态回调/玩家宠物detail生产/alias-owner寿命、SDK/生成取样/机器ABI/实际拖放取消/画面运行待，完整枚举/六closure/总SPEC§10保持。

只做必要静态文本及measure_cg2d.ps1/-Check，不构建/测试/运行/检查产物/摘要/新依赖。

<a id="item-drag-source-state"></a>
## 当前状态

前提交“[#] 补齐战斗物品与目标发布合同”；拟提交“[#] 补齐物品拖动入口与回调合同”。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。

语义2309=2261F关联+48有界代码记录，函数处置1763/证据1563；INPUT139/UIRUNTIME176/RENDER101/NETWORK93/BATTLE430。文档关联预期2371/3680=64.43%仅关联率；184叶验收0、60任务关闭11开放49、六closure及完整门禁保持。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。
