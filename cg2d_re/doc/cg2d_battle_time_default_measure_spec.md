# BOJ01 时间数字与默认测宽接线 SPEC

> 2026-10-06，既有工程委托下先登记再实施。沿 [BOI01 数字创建/更新](cg2d_battle_number_production_update_spec.md) 的真实未决依赖，补1072时间数字余106条、完整默认测宽22条，并修复两个数字父调用的实际目标错误；其余已过数字/更新体保持。网络原格式仅明文，内存CryptInt保，音频暂停。

<a id="battle-time-default-measure-baseline"></a>
## 1. 基线与已证冲突

已打开cg2d.exe只读实际指令为权威。1072/F:000206C0原[stdio合同](cg2d_stdio_contract.md)只覆盖入口格式附近50条，余106条对象/资源布局未全验。本步保原格式、前缀与正常S32除余事实，续原ID不造重复记录。

| 原入口/ID | 源符号 | 全指令/代码B | call/分支/ret | 本步补丁/语义 |
| --- | --- | --- | --- | --- |
| 0x4206C0/F:000206C0 | NewBattleTimeNumber_4206C0 | 156/560 | 11/12/1 | 3/1072续记 |
| 0x44B260/F:0004B260 | MeasureDefaultText_44B260 | 22/50 | 0/4/1 | 0/1398新增 |

两完整单chunk原根178条610B/三页11call-16普通内部分支/两ret无switch；四入向均普通call（Time一处，Default三处）。三图号槽12B、%s%d:%02d含NUL10B、负号含NUL2B，共24B原数据。字体0双宽558F74/单宽558F6C和字体2单宽558F9C是原live数据地址及既有逻辑字段，不将读取到的某个值固化成初始化常量。完整原体/字节/出口/入向、改前根和四补丁见[证据](cg2d_battle_time_default_measure_evidence.json)。

**实际接线冲突**：Time原420718和Number原41F242均直接call44B260；现有源码用了TextMeasure(buffer,0)，它对应通用44B2A0，按每字重新读宽度。已实现/已声明的MeasureDefaultText_44B260非空入口只取一次字体0双宽/单宽，原指令与现文一致。职责、读取次数和原ID不同，不能仅因字体索引相同接受替换。恢复两个原直接目标，不新建工具或接口。

Number41F242的原调用事实复用BOI01完整归档，不重验其余191条；只捕当前改前根及精确接线差异。其旧E-BATTLE-NUMBER-PRODUCTION-UPDATE-0001F1A0-I-20261006证明了含错误目标的旧快照，**改为fail**并附本SPEC原因及后续修正证据；保持旧snapshot/artifacts/expected，不删记录。BOI01的原字节S证据和格式事实有效，已过更新回调不受该接线影响，不混同为全部旧证据失效。

## 2. 完整正常合同

### 1. NewBattleTimeNumber_4206C0

未清256B buffer格式%s%d:%02d、S32 seconds正常负数取反magnitude、负前缀-否则借当前LegacyText593850 T*，正常有符号/60与%60保持旧50格式事实；原44B260默认字体0缓存宽度返回S32除2后固定strlen。每个原S8字节index独立map0起、0..9取行0数字基+字节-0，-取行0列2，:取行0列3，map0仅continue但原index递增/间距与延迟不压缩；支持字符才NewYobiAction(2,0)，失败原return0保已建对象。callback41F110/W8 CF，S8第二次字节构造临时CryptInt，encoded-mask-readCount三样本后W32/W32/W8写目标、不写token/reserved/析构。image0/speed18/x<320角270否则90/flags2，MapNoIndex→GetGraphicXY两个S16返回不增门禁；graphicX先S32、字体2 live sWidth、graphicX转F64后sWidth*原index，再依序+x-halfWidth+advance落F32X，后graphicY+y落F32Y。X截断→Y样本→Xpub→Y截断→nameX样本→Ypub→flagsOR40000 pub→B8=-8→58=2原index/两W8=4→nameX截断/nameY样本先nameXpub/nameY截断pub→W16field2700；原无PlayFrame/整体回滚，最终S32return0。

### 2. MeasureDefaultText_44B260

native非拥有const char*原无空指针门禁；先读U8首字节，为NUL原返回0且不读字体宽度。非空才先固定字体0 doubleWidth558F74/S32、后singleWidth558F6C/S32；每圈当前U8高位80置则cursor+=2/加固定双宽，否则cursor++/加固定单宽，累计后读下一首字节直到NUL，原S32宽度返回。不做Unicode/GBK严格解码、第二字节/容量/边界检查或宽度每字刷新，不变通用44B2A0职责。

### 3. Number41F1A0局部接线

原41F242→44B260，[BOI01归档](cg2d_battle_number_production_update_evidence.json)已证。改为MeasureDefaultText_44B260(buffer)/2，原未清buffer/%d、flags追加+再-、固定strlen、四列图号/S8三字段复制-资源/布局/失败break保旧对象/return0及41F110更新合同保持；仅本次默认测宽目标及来源登记变更，不宣二次全根复验。默认字体0测宽与逐字指定fontType.sWidth布局仍分离；Time布局固定font2宽，不合并两生产者。

时间负秒正常取反/前缀借用/有符号除余与未清256B buffer旧50事实保，极值溢出按RPLAN.24不专项处理。借用prefix可含未支持字节，默认测宽仍测原整字节文本；字符生产仍只0..9/-/:，跳过的原index仍计间距和2index延迟，不改按有效字符数计数。无新UTF8迁移、严格解码、边界拒绝/补零、font限值/钳制或回绕模型。

<a id="battle-time-default-measure-decision"></a>
## 3. 本线程裁决与四补丁

| 差异 | 证据 | 实施 |
| --- | --- | --- |
| 两数字父错接通用44B2A0测宽 | Time420718→44B260；Number41F242→44B260（BOI01） | 各一补丁恢复既有MeasureDefaultText_44B260；私有声明已存在，不扩公开API |
| Time临时encoded/mask/count边读边pub | 4207AC/7B0/7B4→7B8/7C2/7C8 | 三值先按W32/W32/W8取样，再同序写目标；token/reserved/析构保，不照搬Number的count先顺序 |
| Time字体宽度/乘index先于graphicX取样 | 420825→42082A→420834→420838→42083B/842/84D/851 | S16 graphicX扩S32先读，live font2宽度次之，再graphicX转F64后宽度乘原index及坐标算式/F32；与Number的FILD先字体宽度不同 |

默认测宽正文正确保持，Time已有X截断/Y先Xpub、nameX先Ypub、flags先B8、nameY先nameXpub顺序正确保持，不强行套另一个生产者。两函数新I/S及Number局部接线I/S各两条，均static。撤销旧错误I是修正已证事实，不降低门禁或批准错误目标。

<a id="battle-time-default-measure-acceptance"></a>
## 4. 可度量局部验收

| 项 | 本步证据 | 界限 |
| --- | --- | --- |
| 原S | 两完整178条610B/三页11call-16内跳/两ret及24B数据；Default三普通caller/Time一caller；Number原call复用BOI01 | 不代三字体caller/Timer原父411CB0及资源-draw/owner全部闭包 |
| 源I | 完整CPP仅四补丁，Default与通用Font.cpp正文/私有声明保持；两原默认目标、Time不同三字段/字体取样与正确pub、原索引跳过和失败return0保 | 不编译/运行；字体宽度生产者/资源像素、字符载荷消费/回收与PC53继续 |
| 台账 | 1072原记录续记、1398 Default新ID，1071附本次局部接线事实；两新增adapted分类/BATTLE101→103，六新局部I/S E593→599，旧Number I一条改fail/保原材料 | member_inventory_closed=false、完整gates保；旧fail不被删或掩成全门禁已过 |
| 计量 | 语义1398→1399/有效ID1393→1394、分类505→507、关联1511→1512/3680=41.09%；七open与唯一进度/看板同步 | 175队列、R7区间67/3061、60源码closed10/完整验收0/P01、全验收false/0保持；关联率不代整体完成率 |

仅定向文本静态和获准measure/-Check；不构建/调试/新增或执行测试/运行客户端或算法/hash，全部原标准不降。

<a id="battle-time-default-measure-source-state"></a>
## 5. 当前状态与接续

本SPEC先落盘，随后恢复两个原call、Time两处取样、记录续记/新增与旧I失效说明，并追加BOI01更正、相关文档/七open/唯一进度与派生看板，精确提交为 `cg2d-re-battle-time-default-measure-20261006`。

两种数字构造者到同一原更新回调和默认测宽的本步接线已按原恢复；默认宽度缓存与通用每字读取职责分开。全部宽度/图号-XY资源生产消费、字载荷绘制、原父/owner回收-重进退出/ABI-PC53-画面及R1/R7/R8仍沿真实依赖推进。
