# MCP01 观察节点生产字段复制与原取样

<a id="map-chara-producer-baseline"></a>
## 缺口及实际生产者

[MCL01](cg2d_map_chara_link_spec.md)已修追加/摘链，三个实际生产入口43F330/43FEB0/440B70在[Yobi合同](cg2d_yobi_runtime.md)已有完整源码。前两入口的既有数值载体复制顺序不符原端，440B70的载荷取样晚于追加、子节点字段写沿用创建返回而未按原从发布槽复读。两回调本体已存在，前置声明“尚待落位”注释失实。

[原证据](cg2d_map_chara_producer_evidence.json)保存三体288条1126B及三直接call/两父：43F330为122条481B，43FEB0为106条436B，440B70为60条209B。三完整原体保存只为本次正常生产复制/取样局部，不代全部288条或完整父/callee与运行。

1. 43F330四坐标目标+200/+210/+1E0/+1F0均先从临时载体读mask/encoded/readCount，再按W32 mask→W32 encoded→W8 count发布；当前通用AssignFromInt经CopyPayload是encoded→mask→count，不适用该局部。+270清W16仍在第二/第三坐标之间。readiness原43F48F/493/497先读count/mask/encoded，43F49B/4A5/4AB按同序写，当前后两项反序。
2. 43FEB0四坐标均先读count/mask/encoded，再按W8 count→W32 mask→W32 encoded发布；当前局部lambda后两项反序。原两入口共九组构造/析构（43F330五、43FEB0四），值参数/目标/调用序和随机消费保持。
3. 440B70原440BA2先读当次方向、440BA9取payload、440BB2写方向，其余头字段后440BC9追加，之后五轮填槽。当前payload在追加后才取。成功子创建先发布partyActions槽，第一图号写仍用返回child；随后440C10/1E/2A各从发布槽重新取指针再写方向/动作/序号，不能沿用child到全部字段。
4. 既有创建优先级2、原0x30→已定x64 0x50载荷、失败立即空返回、callback/值字段、投影和+0.5/S64→低S32、父追加/五槽正值判定及子创建失败的正值+空槽均保持。这里仅复制既有内存数值载体，不增加网络加解密，原格式明文规则不变。

<a id="map-chara-producer-decision"></a>
## 委托裁决与实际源码

1. 43F330用该函数局部assignCoordinate按原mask/encoded/count取读/发布，替换四个不适用的通用AssignFromInt调用；不改通用CopyPayload/AssignFromInt或其他调用者。每组仍同一次ConstructFromInt、同值参数与一次Destroy，四目标顺序/+270位置保持。
2. readiness及43FEB0现有lambda先依原序保存三个定宽临时值，再按count/mask/encoded发布；不整对象复制或清padding，不增构造/析构、随机、保护或默认值。
3. 440B70方向先取样，再保存现有MapCharaPayload*，之后方向/头字段写和原追加；五轮继续使用这个原取样payload。子创建返回/槽发布/失败条件和首次child图号写保持，随后三个字段各按原复读partyActions[index]取得YobiAction*再写，不统一快照/重读新载荷或增加空槽保护。
4. 共六种片段/六局部块：五行为块、一个注释。注释只改为真实回调前置声明说明，两个回调实现/调用关系不变。没有新接口/全局类型/文件工具、分配器/头owner、网络或音频改动。
5. 三已有本体/真实声明与专用原生创建依赖补正式原身份/来源及adapted，MAP/LOOP共用同一ID/原证据。原三call/两父不代全传递参数、288条其他语义/五槽全上游/所有alias与owner寿命或实际ABI/SDK/场景画面RNG运行。
6. RPLAN.24异常不专项保护、拒绝、补零、钳制、回绕或故障模拟/称安全等价；不变子创建失败为整组回滚，不更换非地址字段宽度、实际LegacyCryptInt或原随机实现。

<a id="map-chara-producer-acceptance"></a>
## 限定验收与追踪

- 当前全文仅HEAD加六模式/六块；九组临时值读/复制的两个原序与构造/析构次数可核，440B70 payload在追加前/每成功子槽后三复读及五轮/失败发布可核。原所有投影/字段宽度/创建/回调/参数/返回和其它源/头/汇编、MCL01/MQC01正文保持。
- 三原288条1126B/库存与三call两父，原四坐标/readiness写序、载荷/三子槽复读及实际本体/声明可追；不称全部288条、完整两父/所有callee和生产消费寿命或运行通过。
- 语义1285→1288/有效ID1281→1284、处置378→381，MAP41→44/LOOP6→9成员；E221→225只两域各局部I/S，四记录共用三源本体实际修正/原证据，不记双份函数。
- G01/G02/U11/R01/R04/V04仍open，全部B/D/W/R/完整功能、R1/R7/R8/60任务关闭和整体SPEC不提升。不编译/调试/测试/运行/检查产物或计算摘要。

<a id="map-chara-producer-source-state"></a>
## 当前交付

三个生产者的数值载体复制及父载荷/发布子槽取样恢复原顺序，失实回调注释已更新；创建/失败、投影/回调、原生头和随机消费保持。局部开发/静态与身份登记不代完整观察链/场景或所有owner寿命验收。

固定命名快照：`cg2d-re-map-chara-producer-20261005`。
