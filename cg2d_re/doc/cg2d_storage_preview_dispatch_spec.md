# 存放预览分发与物品描述消费 SPEC（SPD01）

<a id="storage-preview-baseline"></a>
## 原端证据与范围

2026-10-11，基于已打开cg2d.exe只读会话。窗口41的4A9160、窗口38的4AC1B0和三条公共4AC1EC、描述桥48B350完整共201条667B/六页，21E8十个唯一目标/16跳七RET。普通描述页数由IDR01生产、SEP01面板发布预览和翻页，实际经两个窗口draw callback进入48B350/48B810。只本步完整原根与这些接线有据，所有文字队列、动态owner和画面运行未验。

SPD01只存放描述预览链：4A9160完整114条355B/两页，4AC1B0完整19条60B/一页及公共4AC1EC完整三条11B/一页，48B350完整65条241B/两页，合201条667B/六页、21E8十个唯一目标/16跳七RET。4AC1B0原两个出口（负preview跳/外部预览后清参落入）都进4AC1EC，再4729B0→EAX1/RET；单看IDA十九条主根不能闭该路径。现同Sub_4AC1B0共用CPP尾保，不按误标locale引库功能，不补独立假叶。old969全部格式历史/old231公共尾历史追加，不重复旧F；两个新F及四adapted。窗口41先普通S32 CC4218→原未清16B/现MSVC%10d→fresh current元素13 text载荷→含NUL复制；再fresh窗口/elements→21载荷flags只原BYTE/bit1命中才474270(held21,2)→真实S32结果写value24；再fresh窗口/elements→26载荷flags只原BYTE/bit1命中才474270(held26,2)→value28。字段flags仍W32，两个读取点用unsigned char视图，两组两处，仅这两处正文对齐。随后S32 actionFlags1C：负跳预览但仍绘图；<200下<100原玩家48B1A0、>=100原存放48B350(index-100,page=value20)，不添index/page上界；>=200先减200发布同field→X CryptInt Less320 AL判断决定x413/0→fresh同field再分index<100为4B3BF0，否则仅原AL减100低BYTE传现4B4140，不擅把其高24bits设新布局或改变BYTE接口。原小步只输入/分派，既有各provider正文/签名及鼠标RNG读序保持；每路径预览后DrawCurrentWindow/返回完整S32 1，源码共用尾合并不漏任一绘图。窗口38同S32 preview负仍共用绘图尾，非负原比较<100取同value20 S32 page→玩家/外部物品预览后同绘图尾；无41滚动或宠物分支。两current prototype原48B/十二W32只对应38/41行，draw槽偏移40持实际入口，当前原生callback接线保持，非所有callback/owner寿命已闭。
48B350仅固定第一组CC4D58+index*C44记录，不借current side另选组；key=index+100，source不同才鼠标X GetVal→X低W16发布→Y GetVal→Y低W16发布→X Less320→fresh X W16加60/-257→Y Less240→fresh Y W16加60/-174，source相同跳四次CryptInt，不缓存用于两个比较的X/Y明值或改变RNG次数/发布序。401EA0为单JMP401EC0有据，现既有Read_401EC0 provider复用。之后无条件同S32 key发布；一次原W32 record+C40取值后test CH1（bit100）选择+C29 memo/颜色5，否则+C name/原bit9颜色0-1，这里W32读取本来正确保持，不从testCH误推应读BYTE。描述原+29+page*768，96B每行/八行一页；先tooltip X W16快照再Y W16快照，现低S16界面消费，调用48B810(x,y,title,color,lines,96,8,nullptr,0)真实S32返回透传。原两CALL确实来自两个本步主root，清参后绘图/返回邻窗有据；不把有provider名或静态接线当完整文字队列/画面验收。三当前CPP函数只第一根两BYTE改动，另两正确正文/共用尾保；三定义/一个既有声明四注释，无头文件声明的两个callback不新增公头接口。四类型/九实际callee、五唯一global/两个prototype行及七旧记录保持，未改SGM01生产、IDR01描述/SEP01页数发布链，完整数据格式/描述页数—行消费、共享owner-alias寿命/生成读取和机器ABI及所有文字/画面/互通运行待。INPUT两成员/UI和RENDER各四成员，三限定S零I八open；不引新helper/字段/API/类型/owner、index/page/容量/空保护/补零/默认/钳制/回滚或异常模型，MSVC/WAV-only/原命令字段LF明文及全部完整门禁保持，整体未完成。

<a id="storage-preview-decision"></a>
## 方案及取舍

按子AGENTS第8条既有委托，先记方案再改窗口41两处W32 flags读取为原BYTE读取；保持W32字段及existing ptr接口。其它两个CPP函数、公共尾合并、title W32、四次CryptInt正常发布和八行描述桥均已正确，正文保持，仅合同注释。十九条主根不能遗漏公共尾，不按库函数误名加locale，不增加独立叶/helper。两个callback没有现公头声明，不为记录合同新增接口。

<a id="storage-preview-acceptance"></a>
## 本步验收

四原根及两个公共尾控制边连续覆盖/E8位移、两prototype draw槽、tooltip两CALL及401EA0单跳有据；两滚动F6 BYTE恢复，两组两处/四注释，两整源精确回放、另两正确正文保，四类型/九callee/五global与两个prototype行及七旧语义保持。两旧历史追加/两新F四adapted、三限定S零I八open；派生六metric/四F/三功能/八任务，汇总/-Check终态0，其它及所有完整门禁保。未构建/测试/运行/产物/摘要。

<a id="storage-preview-source-state"></a>
## 当前状态

补齐窗口41/38物品描述预览分发及48B350标题/八行描述消费，含窗口38三条公共尾，共201条667B/六页、21E8十个唯一目标/16跳七RET。恢复两个滚动flags的BYTE读取，两组两处/四注释；W32标题字段和四次鼠标CryptInt、原预览及真实返回保持。两个旧历史追加/两个新F/四处置，三限定S零I八open，全部owner/文字及画面/ABI与运行待。

语义2332=2284F关联+48有界代码，关联2394/3680=65.05%仅关联率；处置1795/证据1599，INPUT158/UIRUNTIME205/RENDER116，其它保持。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

