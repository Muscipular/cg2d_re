# 存放外部物品面板 SPEC（SEP01）

<a id="storage-item-panel-baseline"></a>
## 原端证据与范围

2026-10-11，已打开cg2d.exe只读原会话。窗口41的4AA3D0完整478条1516B/八页，27E8十六目标/52跳一RET；窗口38的4AC890完整426条1369B/七页，23E8十四目标/42跳一RET。两个旧F971/973原数量格式合同保；本步补完整原根和实际事件/字段/图形消费，非所有动态父、SDK或运行验收。

SEP01仅两个完整外部物品面板4AA3D0/4AC890，共904条2885B/十五页、50E8十六唯一目标/94跳两RET；旧971/973全部格式/数量历史保，仅完整合同追加，各一个adapted，不重计F。两S32 index/state，原BL事件位：41只drag type2、38以两次fresh查询允许2或5，source<8清60H；41不因此设consumed，38原设1。40H拖放先三getter(dropX/dropY/source)后一次gCurrentWindow与元素地址，持有S32 elementX/windowX/windowY/elementY及20格扫描，50横步/52纵步、48半开矩形，首匹配退出，原source!=target且source<100才读玩家flags低BYTE 60H门槛；38即使外部source也仍从原玩家表读S32 itemType，38/52不移，不添新来源或上界。21H悬停每格fresh窗口/元素，按elementX→windowX→left→elementY→windowY→top的S32原取样，原-1参数命中，先匹配结束；后drag type非0且20H才fresh source/其<100低BYTEflags使hover=-1。四个实际F6索引BYTE测试恢复，现玩家flags原W16字段不改，只通过unsigned char视图取低字节，保短路与次数。41保原获批静态GBK提示、hover变化清page/value20→发布hover；41有10H边键取回库存逻辑：focus两次短路仅41或0，MouseReset后扫描玩家槽8..27 W16 used，满或外部未用声55，成功原4886E0传41/index/hover+100/target+8；该分支consume1且不落双击页分支。38无此取回分支/提示。两panel events1且hover>=0，2H双击读取W16 used成功才48BF10(hover+100,mode0/1)→声51，consume1；随后fresh mouse edge BYTE bit2，fresh W16 used→S32 page先读/C3C pages后读→page+1发布，>=pages原归0，consume1。前调用/声效后不缓存mouse/used/page/页数，不加默认或钳制。两拖动来源框type41为2、38为2或5/source>=100；来源框与当前hover框各fresh window/held element，S32四坐标读取序明确，低W16边界经现真实Hover提供者，层223/来源框图254与current frame保持；hover框draw后再fresh W16 used、成功发布window41/38 actionFlags1C=hover+100。命中/来源框/当前框/每槽普通图四阶段各原S32读取序落实，drop元素地址只持一次，合计两根十二组十四处，不在所有循环或API之间缓存current window。两末层3清尾→空593850文字，描述色FFFFFFFF/byte13=4，20记录W16 used门槛，普通图中心41(+37,+51)、38(+36,+64)，图号record+4 S32，调用图后才fresh count+8 S32；正值用原未清12B/现MSVC%3d，同层3/字体1/颜色1/中心-3,+7低S16文字，原格式有据、不补零/保护。实际consumed S32 0/1保，不以两个父根闭所有callback/owner/ABI或画面运行。原无直接CALL入本根，41一code-offset MOV callback发布4A968C与两prototype itemhead refs，actual W32匹配为571160的35槽/571740的3槽，32B原prototype八W32及current native callback/Data接线保；不把item首地址当callback字面值或推全表/动态owner，旧把code-offset列data的历史保并追加当前有据分界。四注释及上述限定源码调整，五类型/十六真实callee/原发布父、两表/四global定义声明与五旧语义保；三个限定S零I八open，INPUT/UI/RENDER各两成员，完整数据生产消费/描述绘制及请求互通、shared owner-alias寿命/SDK/生成读取和机器ABI及运行待；不引新函数/helper/字段/API/类型/owner、索引/容量/空保护/补零/默认/异常或失败回滚，MSVC/WAV-only/原命令字段LF明文及全部完整门禁保，整体未完成。

<a id="storage-item-panel-decision"></a>
## 方案及取舍

按子AGENTS第8条既有委托，先记方案再对齐：四原F6 indexed BYTE与现W16 flags读取不符，只通过unsigned char视图读低字节，不改共用PlayerItemRecord字段。drop阶段只持一次元素地址；命中、来源框、当前框、逐槽普通图各显式按elementX/windowX/elementY/windowY原S32次序读取。两根各六组七处，四注释，保短路、原模式差别、全部门槛及真实S32 consumed。其它业务正文、已接真实callee/两callback表/发布父保持，不新增helper或索引保护。

<a id="storage-item-panel-acceptance"></a>
## 本步验收

完整904条连续覆盖/E8原位目标/94跳两RET，四F6 BYTE与各坐标原取样地址；两整源精确回放仅十二组十四处及四注释，五原类型/十六实际callee/一个发布父、两表/四全局定义声明及五旧语义保持。prototype itemhead引用与实际函数指针分开，35/3槽原八W32和当前原生callback接线有据；不借此关全动态路由或owner。旧971/973全部格式历史只追加，两个adapted、三功能各两成员/三限定S零I八open，派生只一metric/两F/三功能/八任务；汇总/-Check须终态0。未构建/测试/运行/产物/摘要，全部完整门禁待。

<a id="storage-item-panel-source-state"></a>
## 当前状态

旧971/973补齐两个外部物品面板完整904条2885B/十五页，50E8十六唯一目标/94跳两RET。恢复四处flags低BYTE及两根十二组十四处持址/坐标取样，原短路、窗口38类型门槛、41快捷取回、页数与图后数量消费保持。两原prototype 35/3槽及一个code-offset发布有据；历史追加/两处置、三限定S零I八open，完整owner/全部描绘与请求互通/ABI及运行待。

语义2330=2282F关联+48有界代码及关联2392/3680=65.00%保持；处置1791/证据1596，INPUT156/UIRUNTIME201/RENDER112，其它保持。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

