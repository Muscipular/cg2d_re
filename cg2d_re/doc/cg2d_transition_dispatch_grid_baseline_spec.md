# 转场控制父与64块、960块、双半幅基线 SPEC

编号BVD01；2026-10-08。沿现转场生产—分派—消费，补四个已有正确入口的完整原新基线；不为纳管改正确代码，源码0改动，无新I。

<a id="transition-dispatch-grid-baseline"></a>
## 原基线

[完整原证](cg2d_transition_dispatch_grid_baseline_evidence.json)保存四根/全部指令、21局部窗、两真实表/对齐解码与源及机器前态。

| 原身份 / 新语义 | 完整根 | 现来源 |
|---|---|---|
| F:0011EF80 / 1755 | 425条1653B/七页，14 direct/0 indirect/58跳转/2返回 | SceneState.cpp::Sub_51EF80 |
| F:0011F610 / 1756 | 168条591B/三页，10 direct/1 indirect/17跳转/2返回 | SceneState.cpp::Sub_51F610 |
| F:0011FC90 / 1757 | 101条330B/两页，1 direct/2 indirect/8跳转/3返回 | SceneState.cpp::Sub_51FC90 |
| F:0011FDE0 / 1758 | 285条752B/五页，36 direct/0 indirect/4跳转/37返回 | WorldUpdate.cpp::Sub_51FDE0 |

合979条3326B/17页、61 direct/3 indirect/87跳转/44返回，21引用五父。控制父12引用来自43CBB0、45EEE0、51C430、51C7D0四父；三个子根9引用均同51FDE0。21局部累计293条953B，九内部窗准确复用本轮完整控制父，最后5200C1窗到真实末界仅13条，其余20窗14条；窗口含重叠/邻指令，不当所有父完整覆盖。

64块425条原证承接此前同选定只读IDA所得全文；本轮其真实四分派表项及完整父验证连接，不重取正确已过块。已纳管51EC10、51F860/51FA70沿[BVB01](cg2d_quadrant_transition_blit_baseline_spec.md)/[BVC01](cg2d_scanline_trigger_sampling_spec.md)保，不重写旧记录。

| R7区间 | 实际用途 |
|---|---|
| 51F5F5–51F610，27B | 首3B 8D4900解码LEA ECX,[ECX+0]，不是NN_NOP；51F5F8四项16B跳转表，后8B NN_NOP/90 |
| 51F85F–51F860，1B | 实际NN_NOP/90，无入向，后真入口 |
| 51FDDA–51FDE0，6B | 实际NN_NOP/90，无入向，后真入口 |
| 5200D0–520160，144B | 原36项跳转表，27/33同520094，不是填充或新函数 |

四区间合178B，两表40项160B、15B实际NN_NOP及3B同寄存器零位移LEA。两表基址均由实际索引jmp操作数取得，各唯一入向来自其jmp，全部表项指向对应完整根的已解码指令；对齐字节无入向，所有后界均为真入口。不能把LEA标NN_NOP或把两个表当对齐。

<a id="transition-dispatch-grid-decision"></a>
## 正确实现保留裁决

1. 64块先RAND再payload位解释/ftol、原小于1取1和低S8零取1、共享触发/四模式初始化正确；只在原候选拒绝路径重抽，不冻结/额外抽RNG。每行state/speed/sourceY/destinationY/X初始化次序、case0/1计数、case2持样计数/更新末发布与case3逐块fresh计数保持；两遍先状态2后状态1、不同RECT生产/Temp借用/低S16坐标次序和完成count/encoded/mask三字段齐采同序发布/析构/phase/0或1保持。
2. 960块初始化960个x→y偏移，源逆序620→0/464→0、每块fresh比例、y/x两个S32乘积与向零除64、232/310偏移/full S32直接native提交正确保；绘后fresh比例加value/方向阈值、三字段mask/encoded/count齐采发布/析构/复位和原返回保。此处正负方向都同发布序，不套BVC01扫描线方向分支。
3. 双半幅低S8零取8、触发时原AND只CL导致mode非零初始0/零初始240正确；上半持样分母、下半独立fresh比例除法/第二Temp借用再生产矩形、每组复制后fresh比例/循环和绘后mode更新严格阈值保持。直接native正常资源失败沿既有裁决，不增裁剪/异常保护或虚称成功。
4. 完整36项分派按原W32 selector/default1、七目标/模式、payload原W32透传、20/21明确零、24/25低S8零取-1或1、27/33同目的/所有返回直透保持。完整根不消费的额外零实参不扩新端接口；已有七目标真实接线保。51D790/51E0D0完整根及所有其他父/callee尚未纳管，本分派完整不外推全转场已验收。
5. 五相关源、四根与初始化助手共五完整正文保持；四adapted与四X用途、LOOP/RENDER仅两个限定S各25来源、G02/R04/V04仍open。无新I、不提升完整成员/叶/域或全局门禁。
6. 原范围/网络原格式明文、两加解密叶排除、音频暂停/RPLAN.24保。不构建/配置/调试/测试/运行、产物或摘要；异常不专项处理/模拟/记等价，不降低总SPEC像素/ABI/寿命/运行与R8标准。

<a id="transition-dispatch-grid-acceptance"></a>
## 度量与验收

| 项目 | 可复核要求 / 预期 | 边界 |
|---|---|---|
| 原证与源 | 四完整根979条3326B/17页；21局部累计293条953B、五相关源/五完整正文与HEAD一致 | 代码0改动/无新I，不当全父 |
| 用途 | 四X178B，两表40项160B/15B NN_NOP/3B LEA各按实际解码/索引入向 | 不新增分母或排除，不混LEA与NN_NOP |
| 原新与任务 | 1755–1758四adapted；LOOP168→172/RENDER69→73，两限定S；三任务仍open | 旧BVB/BVC记录与全门禁保 |
| 看板 | 语义1759/有效1752、合同1870（50.82%）、F900/E1062、R7已审462余2599；measure/Check退出0 | 分类120/mixed3/全入口排除113保持 |

<a id="transition-dispatch-grid-source-state"></a>
## 收口

先行SPEC后原新/用途/七对应文档/三任务/唯一进度，集中静态、文本派生后记录实际值并精确提交，固定cg2d-re-transition-dispatch-grid-baseline-20261008。完整两大FX根/其余父/callee、owner-alias寿命、机器ABI/链接/像素/正常失败/运行与R8继续；pending18/未知ECX、closed11/原差队列/所有旧记录与全部闭包保，整体目标active。

### BVD01 当前正确基线限定静态结果

集中静态通过：四完整根979条3326B/17页、21局部累计293条953B/九准确复用与末窗真实13条，四区间178B的两表40项160B/15B NN_NOP及3B同ECX零位移LEA按实际解码/索引入向区分；五相关源/五完整正文与HEAD一致、真实36分派与各原生产/采样/native失败及发布保持。源码0改动/无新I，两完整机器与完整派生只有具名差集；十四文件UTF8无BOM/LF、四锚点/两限定S各25真实来源通过。

文本汇总与Check均退出0；实际语义1759/有效1752、合同1870（50.82%）、F900/E1062、R7已审462余2599、LOOP172/RENDER73。原分类120/mixed3/完整入口排除113、pending18/未知ECX、closed11/差队列/所有旧记录与完整闭包保持；完整51D790/51E0D0及其余父/callee、owner-alias寿命、机器ABI/链接/像素/正常失败/运行及R8未验，整体目标继续。
