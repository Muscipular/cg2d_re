# 属性页与宠物页签 SPEC（SAP01）

2026-10-10。沿SEV01按钮及格式消费依赖，依子AGENTS第8条工程委托裁决。同一cg2d_re分支，不打tag。

<a id="selector-attribute-baseline"></a>
## 原端证据

4C75A0完整239条753B/四页，488EC0完整81条226B/两页，499B60完整71条197B/两页；合391条1176B/八页、十三E8五目标/39跳六RET。两属性包装父各完整22B，仅低AL归一为S32；六宠物页签回调位于两原型数组的19..21/17..19项，是data绑定，不算普通CALL。属性580900表24项384B，index/label W32地址/percent低BYTE/color S32，原索引/颜色及24项边界逐项保；原标签字节只保历史，当前沿已批GBK，不恢复Big5。原格式根旧921已有部分格式证，保留全历史追加，不重复F。

<a id="selector-attribute-decision"></a>
## 实施合同与边界

SAP01三完整原根391条1176B/八页，十三E8五目标/39跳六RET，24项384B属性表、两22B完整包装父/低AL归一和两原型数组六callback槽有据。75A0原低BYTE state bit2或S32零、BYTE3FC或reply.used S32零时声51/320→先元素/回调快照→清3FC→回填/AL1；否则仅page2绘制，reply.value48 S32与hasText24 BYTE、借32B text04/text25、24S32值与原512B未清buffer保持。图descriptor0地址先元素Y/X，fresh当前window后windowY/X；既有局部drawText以三种具名内部取序保标题元素Y/windowY后元素X/windowX、特殊/两尾文本当前window先元素Y/X再windowY/X、属性元素Y/X先当前window再windowY/X。每次重借元素/window，不缓存整页；低S16坐标/S8颜色、非零最多14行/第8换列、原表索引顺序/颜色/percent BYTE、四±百分比格式与两现MSVC格式原点和已批GBK保持。488EC0/499B60现正文正确保：index-19/-17，五8B对used W16/索引S16/pet flags W16（1或2/2或1原序），匹配递减到负，无匹配且S8旧mode等ordinal+1才U8减1；负ordinal若已found不减mode，bit2声55/320后fresh窗口索引graphic图244306，不补容量/tab/index保护。成功时前父低AL归一为S32 0/1，后父忽略结果原EBP0返回S32 0；两属性包装父仅AL归一，不据高24位/全机器ABI等价。六原型callback槽仅数据指针对应19..21/17..19，不作为新增CALL或整原型/分发验收。两组四处读取序/六外层合同注释及一局部enum说明、两正确body/两整源其余及类型/现包装/原型绑定保持；旧921格式历史保追加、两新F三adapted/三限定S零I/十任务open。全回复生产/表标签与字体/宠物对生产/动态callback全分发、所有callee/外父/alias-owner寿命、SDK/生成取样/真实布局交互声效与运行待；无新公开API/通用helper/字段/owner/保护/默认/补零/钳制/回滚/异常模型。WAV-only/明文网络/MSVC、48有界代码/完整枚举gates/六closure/总SPEC§10保持。

沿既有本函数局部drawText，内部枚举仅区分已证的三种字段取序，不新增通用工具或公开类型。原低S16坐标/S8色值保；普通正常值的坐标计算未据本步宣称已出错。标题strlen与居中、特殊提示、14非零行/第8换列与两尾提示、GBK标签和MSVC格式callee保持。原flags1/2及2/1、found负ordinal与未找到模式调整、成功结果差异保持，不增加防御分支或表容量限制。

<a id="selector-attribute-acceptance"></a>
## 限定静态验收

只核三连续完整原根/十三E8及384B表、完整两包装父低AL消费与六data绑定、两整源精确差异/类型/现caller及原型绑定保持。旧921仅追加、两新F/三adapted、三限定S零I/十open。全部回复/宠物数据生产、动态回调分发/callee、alias-owner寿命、SDK/生成取样/实际布局交互和运行待，完整枚举/六closure/总SPEC§10不提升。

只做必要静态文本与measure_cg2d.ps1/-Check，不构建/测试/运行/检查产物/摘要/新依赖。

<a id="selector-attribute-source-state"></a>
## 当前状态

前提交“[#] 补齐选择器按钮事件与结果消费”；拟提交“[#] 补齐属性页与宠物页签合同”。方案先记录，源码对齐/登记及静态汇总/-Check终态退出0/派生范围核对通过。

语义2304=2256F关联+48有界代码记录，函数处置1756/证据1544，INPUT133/UIRUNTIME170/RENDER97；文档关联预期2366/3680=64.29%仅关联率。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。
