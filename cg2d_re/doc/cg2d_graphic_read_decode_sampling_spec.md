# BOL01 图形读盘与解码正常取样 SPEC

2026-10-06：沿BOK01图形生产的真实读盘消费补缺口，按已授权未决工程先SPEC后本线程裁决。已有两原ID续记；正确解码正文保持，修读盘取样和旧接口额外限定，不代全资源/owner/运行验收。

<a id="graphic-read-decode-sampling-baseline"></a>
## 原基线

| 入口 / 语义 / 原ID | 源码符号 | 完整条数 / 字节 / 页 | call / 内部分支 / ret | 普通入向 / 不同父 |
| --- | --- | --- | --- | --- |
| 45A900 / 167 / F:0005A900 | GraphicRead.cpp / ReadGraphicDataByIndex | 75 / 222 / 1 | 4 / 7 / 2 | 4 / 4 |
| 52D5C0 / 168 / F:0012D5C0 | GraphicDecode.cpp / ReadGraphicData | 182 / 461 / 3 | 0 / 25 / 3 | 2 / 1，均来自45A900 |
| 合计 | 两个已有单chunk，均无switch | 257 / 683 / 4 | 4 / 32 / 5 | 6个普通call引用；父按根分列 |

[完整证据](cg2d_graphic_read_decode_sampling_evidence.json)保原四分页、chunk、入向、旧两个完整语义、改前两源码正文/头、二源码补丁/二头注释修正/二旧SPEC文字更正。读盘原两FILE call是45A94D→53BC41、45A966→53BCCD；两核心call为45A99B/45A9C8→52D5C0。原没有外部RLE调用；repeat的rep stosd/stosb由宿主字节写承接既有正常合同。

<a id="graphic-read-decode-sampling-decision"></a>
## 已裁决的具体实现

### 读盘：修二处正常取样

S32 index<0或>=gMax返0，不清任何输出；先取主表实际记录地址/复制32B快照，后从同一地址读S16 fileIndex；缓存该九槽FILE，null返0/不加槽范围保护。缓存FILE按快照S32 offset/BEGIN0 seek，弃返回；seek后重取同槽FILE再取快照U32 length，以共享读区/length/1 read，返回仅1继续，其它返0不清输出。可省略palette参数是否非空先取样，后发布*destination=独立共享解码区，无论core结果均保此发布；缺省palette由未清局部W32槽承接，length独立未清局部W32槽，两原call45A99B/45A9C8均同52D5C0，core返回非空→1/空→0，无新增尺寸/短读恢复/回滚/预清。主表/FILE/缓冲区与输出native非拥有、字段原S32/U32/S16，不扩大原数组声明/容量或全部caller-owner验收。

第一补丁固定已算出的记录地址，快照后从同一地址读取fileIndex，保已检查FILE用于seek，seek后才重取FILE并取快照length。第二补丁以局部bool保存原在目标发布前的可选palette判断，再保原发布后分支、缺省局部槽与共同core接口。不复制两段相同core表达式或扩接口；两原调用点相同目标/参数职责如实分列。

### 解码：正确正文保持，撤额外缩范围

六参数native非拥有源/目标槽T**/四U32输出，初读*destination/缓存目标，null先返空且不读源；依序RD两签名不符返空不写输出。头先缓存U8 flags/U32 width/height，不读03。raw不读header.length：cursor+10，扩展头取palLength/游标+4→pub palOut→W32 width*height再fresh *palOut相加，无扩展先乘再pub pal0；前向每字节读/推进源/写目标/计数，最后length→width→height输出并返初始目标。RLE先固定source+header.length边界后cursor+10/可选pal+4/pub pal或0，仅命令间cursor<end比较；repeat先value(40置零且不消耗value，否则读value)，长度20优先10且都用低4位/后1或2字节不加1，固定value填充，不承诺原stosd/stosb机器写宽/故障原子性；literal只看10而忽略20/40，保原count>=FFFFF失败比较，前向逐字节source读→目标写→目标/源推进→count--。零长度仍消费命令与所需value/长度；不加命令内部边界、像素数限制或修复；成功length=W32 width*height非实际写量且不含pal，再pub width/height并返初始target。缓存目标/头/边界及原读写序保有效对象的正常别名，不以旧两两不重叠额外缩范围；不把原地址回绕、越界/溢出故障或ABI当保证。当前正确核心正文不改，接口仅撤额外限制/规范正常宽度注释，运行与所有父/owner仍未验。

原没有两两不重叠检查；旧头及旧SPEC§5将这项当调用前提，范围比原正常取样/写序窄。本步删除该额外约束，继续要求实际访问的对象有效、类型化输出对象存活/对齐和数组内指针运算；按原采样和写序处理正常别名，不添加新的业务保护。源、目标或输出别名可能改变后续所读字节/最终输出，不能用预先全缓存载荷、memmove或输出合并写替代现有前向逐字节处理。

旧§6在旧限定范围下的验证/编译属历史事实，仍保留；不追认为当前运行验收。旧注释“回绕保证”改为正常W32宽度，按RPLAN.24不新增或模拟溢出/越界/钳制/补零/故障路径，不宣原x86地址/故障/ABI等价。公开类型不变，不增加依赖/RTTI。

<a id="graphic-read-decode-sampling-acceptance"></a>
## 可度量的局部验收

| 项目 | 通过条件 |
| --- | --- |
| 原覆盖 | 两单chunk完整257条683B/4页、4call/32内部分支/5ret，无switch；6普通入向按4父/1父分列 |
| 源码 | 读盘整CPP仅二补丁、正确core整CPP保持；头仅二注释修正，声明/数值宽度/native地址保持 |
| 正常合同 | 32B快照后同地址S16槽、FILE缓存/seek后重取、读取严格1、可选palette判定/目标发布/原失败不清输出，核心raw/RLE/cache/输出写序按完整原证据 |
| 台账 | 两原167/168完整历史深拷贝续记，1399语义/1394有效ID/1512关联不变；分类510→512 |
| 功能/局部证据 | RESOURCE29→31；两根各独立I/S，static、runtime=false，E609→613；member_inventory_closed=false、完整gates空 |
| 派生 | measure生成/-Check一致，仅两根/RESOURCE/两分类指标与七open进展/引用；其它队列/功能/完整门禁保持 |

不把公开注释更正当新增解码算法，未把不重叠限定范围下的历史证据当当前全部正常别名已运行。未构建/调试/新增或执行测试/运行客户端或算法样例/hash。60源码closed10/完整验收0、global_accepted=false、accepted_leaf_features=0及R1/R7/R8状态保持。

<a id="graphic-read-decode-sampling-source-state"></a>
## 状态与继续推进

固定命名快照：`cg2d-re-graphic-read-decode-sampling-20261006`。只修实际消费链缺口，167旧FILE证据继续保、168正确core正文和原符号继续保；两个不带地址后缀的既有源码名，其历史direct_snapshot_definitions仍为空而sources明确。

原[图形信息](cg2d_graphic_info.md)、[资源实现](cg2d_resource_implementation.md)、[解码SPEC](cg2d_graphic_decode.md)同步；旧解码SPEC两前提文字直接更正并附当前范围，原取证/历史运行记录不删。U08/U09/G01/G02/R01/R04/V04保持open，继续真实内容的渲染/调色板消费及所有者/回收接线，不全面复验已过数字/stdio/容器。网络原格式明文、内存CryptInt保、音频暂停。
