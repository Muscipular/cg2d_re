# 回车文本比较字节取样 SPEC（BTG01）

日期：2026-10-07。沿主场景/人物创建的按键分发→回车消费推进，按工程委托先记裁决再实施。原格式明文、音频暂停、RPLAN.24与执行限制保持。

<a id="enter-text-byte-baseline"></a>
## 原基线与差异

| 原根 | 完整范围/指令/字节/页 | 调用/消费 |
|---|---|---|
| 4596D0 按键分发 | 主[4596D0,45975F)37条143B、尾[401920,401953)17条51B；合54条194B/一页 | 12direct十二目标/13jump三RET；十九code入向来自WorldUpdate、正常场景、战斗三个父 |
| 44AB60 回车消费 | [44AB60,44ADAF)175条591B/三页70+70+35 | 九direct八目标/31jump一RET；4596D0@45974B唯一caller |

合229条785B/四页三chunk，21direct二十唯一目标、无间接CALL、44jump四RET。按键尾401920归同一4596D0，不造新F、不把它混入401960网络加解密叶排除。44AB60已有766与完整格式生产消费字段，保持后追加；父只有现正文/冻结定位，新1613语义补基线，不造新原函数。

按键入口现正确：Ctrl低W8 bit2再Shift bit2门禁，只有均未中才按Up/Down/Left/Right bit10现场取样调用；Backspace/Delete/Tab/Enter按同bit10依序，Tab仍真实空509080。End bit4命中才跳原尾，保5043D0(2)分支、5042D0(100)/原PlaySe与504180(99)调用及音频暂停；不新增声音或重新接尾。

回车入口IME首BYTE非零直接返，不取input/不发phase20；空IME才一次current context。聊天context626AA0 length0也早返；其它上下文继续。原历史tail W32取样，旧低W8真实MOVSX到S8给旧记录，普通S32下一值pub后若>=64再pub0。每记录289B/64槽与现owner保持；不删真实W8→S8视图、不加异常回绕/越界模型或改变已有环门槛。

聊天比较44ABC6先读context输入BYTE，44ABCA再旧记录BYTE；相等后复用同输入BYTE判NUL，不匹配即停止，仅消费全等谓词。相等恢复旧低S8 tail；不相等按原含NUL复制到下一记录，再449FA0读当前tail，回来44AC2B现场重新取context。当前历史比较用宿主strcmp，恢复显式逐BYTE骨架，正常去重结果此前正确，不称实测聊天故障。

颜色44AC3C先取，再44AC42写cursor=-1/50C940(context,color)，当前顺序正确保持。随后三次现场重新取current context，分别清length W32、caret W32、text首BYTE；末再一次取样用于后续分派，不能缓存一次代所有读取。六具名context D66CA8/CB03F0/CC46E0/CC2080/D2D828/CA0180按原次序仅AppendChar CR一次并重读context。

黑名单contextC8B1B8比较44ACDC先输入BYTE、44ACE0再玩家name@FDA808 BYTE，相等后同输入样本判NUL；现BorrowPlayerDisplayText(1)借同name owner，仅一次取地址，当前宿主strcmp替原读取，恢复逐BYTE/仅相等谓词。地址固定且原常量非NULL测试不新增业务拒绝，text/name不复制/清洗/转码/限长。

匹配本人、已在名单、添加成功/失败四原文及两原sprintf点/三个互斥有参源码表达式保持。四正文含NUL：554938 27B、554928 15B、5532C4 17B、5532B0 17B；未清256B message只按原格式写，清text BYTE→caret W32→length W32，再44B050(message,4,3)/SetContext626AA0/phase20。Contains/Add原AL消费与已实现provider保持，不扩大为名单文件/CRT/跨模块生命周期已验收。

三相邻padding均实际逐BYTE90/NN_NOP、无入向、前RET后真入口：主分派后1B、原尾后13B、Enter后1B。尾13B已有RPA01 mapped，整对象/引用/处置保持，本步只记两个新1B用途，不能重复增加R7或把相邻401960排除改为已实现。

<a id="enter-text-byte-decision"></a>
## 实施裁决

Input.cpp既有Sub_44AB60两唯一片段：历史输入→旧记录BYTE比较与同输入NUL；黑名单输入→玩家name BYTE比较与同输入NUL。两局部bool只表达原相等谓词，各局部索引从0开始，不新增比较helper/外调/共享状态；不公开内部SBB差值，不改void业务声明或原未定义寄存器残值。

现历史普通S32推进/64环、真实低W8/S8、复制/重取context、颜色/游标/清理顺序、六context CR、四格式/名单provider及提示切换/phase全部保持，父按键和所有其它源码/头保持。仅恢复已证读序，不做异常模拟、补零/保护/钳制/拒绝/限长或新编码。

766原字段/格式/role/source/入向和旧边界完整保持后追加，另补按键父1613语义；INPUT27→29及两处置（父equivalent/Enter adapted）、两局部I-S、两个新1B用途和三open任务current/引用。完整父/二十provider/正常资源寿命/ABI/链接/明文互通/运行仍未验。

<a id="enter-text-byte-acceptance"></a>
## 可度量验收

| 项 | 标准 |
|---|---|
| 基线 | 两完整根229条785B/四页三chunk，21direct二十目标/无间接/44jump四RET及19点三父/Enter唯一入向 |
| 比较 | 历史/黑名单均输入BYTE先取、对方BYTE后取，失配短路、NUL用同输入样本，仅全等/无helper |
| 原保持项 | IME/聊天空早返、低W8/S8与普通S32+64环、449FA0后取context、颜色先读/游标-1、三次新context清理及末取样、六CR、四格式/AL/未清256B/提示切换/phase20 |
| 源差异 | 整Input.cpp=HEAD+两唯一片段／一个业务体；父按键/所有其它源头/音频现调用与明文格式保持 |
| 登记 | 766全原字段保持后追加、一新父语义、INPUT29/两处置/两I-S、两个新1B用途、既有13B mapped整对象保持 |
| 派生 | measure/-Check终态0；完整快照增量/UTF-8无BOM/LF/真实引用与差异文本核对 |
| 完整门禁 | 全父/二十provider/全部owner-alias正常寿命/SDK-ABI/链接/画面/互通/运行R8不代验；原P05-P08 closed/pending7保 |

<a id="enter-text-byte-source-state"></a>
## 实施状态

方案按工程委托裁决；实施/集中静态与派生结果在本节和唯一进度追加。两原正常比较结果此前正确，不称运行故障；音频暂停/执行限制保，不构建测试运行/计算摘要，不手改派生看板/快照或提升完整门禁。

### 集中静态与派生完成记录

2026-10-07：Enter一个原业务体两比较片段已实施。整Input.cpp=HEAD+两唯一片段，所有其它关联源码/头及父按键正文保持；229条原指令与逐chunk完整785B/四页三块、十九code入向三父/Enter唯一消费、三padding及四提示含NUL字节、整个机器增量、四文档与三任务六列，四组集中静态全部通过。登记中早返误写同步纠正为IME非空/聊天空，不改原代码控制流。measure/-Check同会话终态0，完整派生快照增量通过；12文件UTF-8无BOM/LF、四锚点及两局部I-S各26真实来源/差异文本通过。

真实结果：语义1614有效1607/文档1725（46.88%）、INPUT29/处置754/E880/R7 324余2737，仅两个新1B推进用途；旧766全部格式与生产消费字段、旧13B RPA01整对象及全部引用保持。其它五域/冻结定位/pending18/差队列/原任务与完整门禁保。已确认的两比较取样差异已修复；全父/二十provider/全部正常owner-alias寿命、SDK-ABI/链接/画面/原格式明文互通和运行R8继续。未构建/编译/测试/运行客户端或算法样例、未计算摘要。
