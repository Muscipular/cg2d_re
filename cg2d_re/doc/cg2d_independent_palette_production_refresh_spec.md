# BOM01 独立调色板生产与版本刷新 SPEC

2026-10-06：沿BOL01读取内容到实际调色板/绘制消费补缺口，先SPEC后本线程裁决实施。按总规划§4/§5既定只还原32位色渲染；原索引资源/调色板仍保，16/24位渲染分支不新增实现。当前两个原ID缺独立机器登记，不以旧文档存在代验收。

<a id="independent-palette-production-refresh-baseline"></a>
## 原基线

| 原入口 / 新语义 / 原ID | 真实源码 | 完整条数 / 字节 / 页 | call / 内部分支 / ret | 普通入向 / 父 |
| --- | --- | --- | --- | --- |
| 522CB0 / 1399 / F:00122CB0 | GraphicRead.cpp / LoadIndependentPalette32_522CB0 | 313 / 1163 / 4 | 2 / 29 / 1 | 1 / 1，来自523165 |
| 523140 / 1400 / F:00123140 | 资源helper及render包装承接同一原ID | 30 / 87 / 1 | 1 / 5 / 3 | 2 / 1，父520DD0 |
| 合计 | 两单chunk、无switch | 343 / 1250 / 5 | 3 / 34 / 4 | 3普通call引用，父按根分列 |

[完整证据](cg2d_independent_palette_production_refresh_evidence.json)保原五页、chunk、入向、三源码正文/原头、七生产补丁和两个通道承载补丁。生产原调用522CEF→45A8B0、522D0D→45A900；刷新523165→522CB0，原父两call520E6C/521484。所有父/heap/上传运行不被这三引用替代。

| 已排除原低色深区间（尾不含） | 位深 | 字节 |
| --- | --- | --- |
| 522E47..522EB1 | 24 | 106 |
| 522EB1..522F23 | 16 | 114 |
| 522FCB..523066 | 24 | 155 |
| 523066..52310C | 16 | 166 |

四段共541B，仅在完整原体中纳盘，不计新端实现；混合生产入口仍纳管共享和32位路径，不整函数排除。原32位0xB04记录：两S16版本、+04原色256W32、+404色调256W32、+804的768B BGR；原8B索引项仅record指针在x64拓宽，loaded/U8与unknown[3]保。

<a id="independent-palette-production-refresh-decision"></a>
## 具体差异与裁决

### 生产七处恢复

原S32 index<0或>=count仅return；入口从本次索引表读loaded，非零跳读图/原色生成但仍色调/版本，不写loaded。零则队列S32图号→GetMapIndex/忽略返回→ReadDataByIndex用当前data/width/height与局部paletteLength，失败先索引表样本→gGraphicDataBuffer=-1→该表再取记录清768B BGR；成功width→data→height/W32像素积、局部长度/源地址/索引表先于gGraphicDataBuffer=imageNo，之后该表取记录复制L BGR。L<768则复制后新表/记录从3*(L/3)起清768-L，不改原起点/长度、不钳制/补全末余字节。原色目标表一次借实际record的+04，0索引W32=0；1..255每项新索引表/记录，R(+2)-B(+0)-G(+1)取样、pack00RRGGBB、先W32颜色再纯黑W32=1；仅loaded0分支W32计数加一。色调目标表再一次借+404/0W32=0；每1..255先索引表→redMap→该表record→R字节查red，greenMap→G字节查green，B字节先于blueMap→查blue；先W32颜色再纯黑1，不把源三字节/三表永久缓存或合写。尾两W16分别新索引表先于活动号/帧版本样本，随后该表取record/pub。原0xB04无指针记录/768B BGR/两256W32色表保，原8B index仅record地址拓宽为16B且loaded/unknown三字节保。原完整313条含已排除16/24色路径，仅共享及32色实施；全caller/分配-owner/上传像素/运行未验。

当前entry引用贯穿查图、读盘及所有表写；颜色纯黑也被合并为一次W32。本步撤长借引用、按原阶段重取索引/记录，固定每张实际目标表与逐项源记录的不同借用范围；原色和色调都恢复完整颜色发布后纯黑再写1。保原失败清BGR、L/3补清、loaded不置位、计数与两版本W16，不把原业务补清泛化为安全补零。

### 通道承载原顺序

现有ColorChannelMaps顺序/注释为R-G-B，原5562E8/5562EC/5562F0实际B-G-R，原初值分别指向628FFC/6290FC/629200，各为可写通道数组。三个原W32槽12B原字节及全部数据引用保在证据；32位生产直接使用F0取R、EC取G、E8取B。

本步将结构恢复B-G-R并以native指针宽度作布局断言，唯一位置初始化同步从{redMap,greenMap,blueMap}改为{blueMap,greenMap,redMap}。具名red/green/blue仍各指向原同一新端数组，不改变Tone成员指针消费者或GPC01颜色helper源码/具名值。只拓宽已证地址，记录的S16/W32/U8不扩。相关结构/初始化两处修正与生产七补丁分列，不重报其它父完整审查。

### 刷新与包装正确正文保持

原graphic+22的S16索引<=0或>=S32 count仅W16清0并返U8=2；有效时借本次record，null进生产叶不新建或保护；非空先S16 paletteChoice扩S32比较完整活动号，相等才读S16 version比较完整帧，全同返回U8=0，不刷新。其它经522CB0再返U8=1，不宣刷新成功或吞原空record行为。现有资源侧S16引用helper与render包装共同承接同一原ID；包装从descriptor+0E(原graphic+22) memcpy读S16，仅result==2才W16写回，其它保持字段、返同U8。原父DrawAllQueued两call520E6C/521484弃返回继续原图加载，全部父闭包/owner运行另验。正确两正文保持，无新空表/空record/版本回绕/故障策略。

一个原ID由两个当前函数分工承接，不算两个原函数。刷新资源声明及包装声明不扩；原无“刷新成功才继续”的额外门禁。当前renderer实际消费接线已有局部证据，本步不全面重验DrawAllQueued/GUP01/GDL01或堆owner。

<a id="independent-palette-production-refresh-acceptance"></a>
## 可度量的局部验收

| 项目 | 局部通过条件 |
| --- | --- |
| 原覆盖 | 两根343条1250B/5页、3call/34内跳/4ret；无switch，入向三call分父；低色深541B仅纳盘 |
| 开发差异 | GraphicRead.cpp仅生产七归档补丁，刷新及包装保持；Palette.hpp仅通道承载/断言，Palette.cpp仅唯一位置初始化，具名数组身份保 |
| 正常合同 | 各阶段索引/record重取、固定目标/逐项源、通道样本/颜色W32两次发布/两个版本W16，失败/loaded/计数/768B原补清/refresh返回写序正确 |
| 原新映射 | 新语义1399/1400纳管两个已有库存ID；刷新两源码同ID，旧历史直接定义1/2保持不伪造位置 |
| 台账增量 | 1399→1401语义、1394→1396有效ID、分类512→514；合同关联1512→1514/3680=41.14% |
| 两真实功能域 | RESOURCE31→33、RENDER26→28；两根各域独立I/S新增8条static E613→621，仍只有两个原函数 |
| 完整门禁 | 两域member_inventory_closed=false/gates空，七open仅进展/引用；60源码closed10/完整验收0、global_accepted=false/accepted_leaf_features=0 |

字段布局断言属于源内约束，未执行编译/测试。当前仅正常共享/32位局部静态，不代所有别名、主表/FILE/队列/堆分配-释放/重入owner、实际数组初始化/全Tone父、像素上传/画面、R1/R7/R8或运行。原查表/读盘已有BOK01/BOL01证据按当前范围复用，不重新全面审查。

<a id="independent-palette-production-refresh-source-state"></a>
## 状态与后续推进

固定命名快照：`cg2d-re-independent-palette-production-refresh-20261006`；15文件精确提交，用户既有六项排除。原[DX与调色板合同](cg2d_graphic_dx_cache.md)§5.16更新取样阶段及通道承载；两库存ID从仅历史位置补成独立当前合同/分类/局部证据。刷新空记录继续进原生产叶，无新恢复，原目标表/版本引用生命周期不被延长。

继续真实通道生产、渲染/调色板消费者和owner开发，完整父/运行仍需真实证据。仅定向文本静态和measure/-Check，无构建/调试/测试/客户端或算法样例/hash；RPLAN.24不增溢出/越界/拒绝/钳制/故障模型、不宣异常或原STL/机器ABI等价。网络原格式明文、内存CryptInt保、音频暂停。
