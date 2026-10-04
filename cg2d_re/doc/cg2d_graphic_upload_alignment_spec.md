# 图形节点上传、条件取样与调色板借用 SPEC

> GUP01；2026-10-05依工程委托采用，对应G01/G02/G04。沿[DX §5.8–5.9、§5.18](cg2d_graphic_dx_cache.md)、[x64资源归属](cg2d_graphic_dx_cache_x64_spec.md)、[GDL01](cg2d_graphic_load_alignment_spec.md)与RPLAN.24。先记局部决定再修源码，不改变现有32位CPU上传和资源owner。

<a id="graphic-upload-baseline"></a>
## 1. 原证据与现有差异

[只读证据](cg2d_graphic_upload_alignment_evidence.json)保存520B10完整227条/690B，[520B10,520DC2)；三个直接调用均来自520DD0，521032/52124E/521573。原节点三快照、图形S32号/描述符、DX缓存指针和共享资源贯穿本体。

| 阶段 | 原读取/调用 | 实施决定 |
|---|---|---|
| 入口 | paletteChoice/version先取，后图号、mixMode、DX基址；DX entry由入口地址固定供后续使用 | 分阶段取原值，固定entry不换成装载后新基址；保低W8位4置两个本地快照0 |
| 两次匹配/限流 | 520B54节点choice先取且限流用保存DX；version仅choice相同才读，nodeIndex/graphicIndex仅两前值匹配才读；限流后再独立核三字段 | 撤销提前读取图形索引；保存第一次choice供限流；各条件内先nodeIndex再图形+22。限流count保存一次后增，不重读choice/count |
| 资源读取 | 先图形号后缓冲图号，缺图沿保存号调用45A900；成功重读当前图形号发布，失败ptr/high/width/palette/图号-1→0 | 显式图号/缓冲阶段，原真实读取、失败顺序和返回保持 |
| 块尺寸 | blockX S16→TextureSizeW S32→DX.width S16；TextureSizeH S32→blockY S16→DX.height S16；清黑比较重新取TextureSizeW但使用保存Height | 不提前读两个DX尺寸，不重复blockX/Y；撤销清黑时重新取Height。原部分块清黑调用/条件保持 |
| 独立表 | 先+22 S16及资源索引record；record空直接收尾，不读混合位；非空才读位4选+404/+4 | 同次借record的原色/色调表地址，调用者在record非空后才读mixMode，不重复索引查找或跨子数组算地址 |
| 普通表 | 旧palette地址保存一次，字段比较后失效；空引用时看paletteSize，正值先Width→mode→Height→Data→图号领取mode2，非正才新读mode决定mode0 | 按各分支原阶段读取，不跨独立表/普通表预取applyTone；真实palette调用/发布/AutoMap保持 |
| 上传参数/写尾 | 每上传点在表处理后新读DX.height、blockY、blockX、surface；普通表调用后flag=0、再按当前size>0写1，最后两个低W16快照/+22 | 撤销复用尺寸阶段旧Height和未定求值序的参数读；原普通flag写序/独立表空收尾/快照最新索引保持 |

原三个caller由两节点遍历helper承接：整数一个，动态/固定缩放复用另一个；这是指定接线，完整DrawAllQueued/模式/画面及资源寿命不验。本组不修改已批准450400 CPU写入核心/显式source/stride接口，不补DX7 Lock/Unlock机制，不以此豁免上层取样/像素/生命周期。

<a id="graphic-upload-decision"></a>
## 2. 具名局部实施决定

1. GraphicDxCache.cpp仅替换LoadGraphicTexture_520B10本体：按上表恢复真实条件读取和阶段；保既有两次快照核对、原10次限流、共享读图/失败五槽、清黑、独立/普通表和两上传/写尾。
2. 资源侧既有BorrowIndependentPaletteColors32改为void(index, originalColors&, tonedColors&)；仅这一上传caller、声明/定义三处同步。一次保存私有record指针，缺record时两输出null，存在时借同一record两表地址。保持私有0xB04布局/记录池和索引池，不公开私有record/新全局类型、不造副本，不加索引范围检查或跨表指针运算。
3. 两const uint32_t*非拥有输出用于原“先record、后mode”的选择阶段，地址形成不读色表像素；只在上传同步消费，池释放后失效。颜色解释、色调算法、独立表刷新/加载和一般调色板提供者不改。
4. 保原入口固定DX entry与第一choice保存值；同一函数中真实全局/字段的再次读取分别保留，不能把全部合并为快照，也不能无证改为所有字段重复取样。
5. 原低S16坐标/尺寸、完整S32限流/图号/尺寸、W8 mode/flag、三W16快照及原生非拥有地址保持；ABI/调用栈、SDK、显式CPU参数和画面/完整父不因此通过。
6. 根520B10补唯一语义、限定处置，I/W/S各一。RENDER/RESOURCE成员增加，G01/G02/G04继续open；总SPEC/分母闭包不刷新或关闭。DX旧“父未实现”按日期历史与当前已有消费纠正，其他历史不改。
7. RPLAN.24不新增NULL/零尺寸/越界/容量保护、不补默认表或回滚/预清、不模拟溢出/故障；未构建/查产物/新增或执行测试/运行/摘要。单线程、音频暂停、网络原格式明文保持。

## 3. 数据、真实依赖与可观测边界

- paletteChoice/version入口本地值在mix位4时写0，原首choice为S16保存值，限流只在queueMode非零、DX.flag非零且保存choice>=0时读S32 count；count>=10原早返1，否则保存count+1写回。
- 首/次快照的paletteIndex读取各在前两项匹配的分支中；另一次+22在清黑后选择独立/普通表，最终再新读+22发布。四次各有阶段，不把某次取样跨回调复用。
- 读图失败次序与GDL01不同：ptr→height→width→paletteSize→图号-1→0。成功/已有buffer继续取原入口固定DX记录，不把帧内资源重读误当重新定位DX的授权。
- 矩形只依原尺寸裁最后块；上传source Y在原表处理后以新DX.height−新blockY−1计算，blockX/surface亦该阶段取样。两路同步消费现有共享source/width及既有CPU倒行写入；本组不更改backend/源格式或借用存储。
- 独立record空仍写三快照并返回1，不上传、不AutoMap、不改DX.flag；非空位4=0选同record toned，否则original。普通table失效先旧image=-1再entry.palette=null；真实领取结果直接发布，空palette仍走原AutoMap，随后flag写序保持。
- 最后先写paletteChoice低W16、version低W16，再从当前graphic描述符+22取W16写node.paletteIndex；不更新reuseState、loaded或资源计数。
- 所有原callee、调用点及间接/固定地址区别在证据保留；借用同一GraphicRead资源池、GraphicPaletteCache普通表、既有TextureSurface/TextureNode/DX owner。完整初始化/场景/上传/清理及间接alias仍待逐链闭合。

<a id="graphic-upload-acceptance"></a>
## 4. 局部验收

| 门禁 | 标准 |
|---|---|
| I | 根本体及既有单一资源借用声明/定义三处落位，真实条件读取/保存值/两上传参数阶段及原失败/返回保持 |
| W | 原三调用与新两遍历helper复用、双色表同record借用/唯一owner/已有backend可追；完整父/模式/寿命不验 |
| S | 原完整227条、阶段/实际callee、三限定文件补丁外全文/既有台账外部保持，无新保护/表副本或私有record外露 |
| R | 原新资源/初值/输入下的限流、上传/画面、模式往返/缓存失效和资源寿命仍未验，执行限制下保持blocked |

G01/G02/G04及完整功能/闭包继续open。

<a id="graphic-upload-source-closure"></a>
## 5. GUP01源码小步结论（2026-10-05）

上传根本体和既有资源借用声明/定义三处已实施并通过集中静态核对。227条/690B连续至实际边界，三caller窗口36条、11绝对data operand与五实际callee可追。两节点遍历helper承接三原点；动态/固定缩放复用DrawScaledNodes，整数由DrawIntegerNodes承接，本次不重验完整父。

恢复条件索引、保存首choice/count、固定entry、各尺寸/混合位/表与上传参数阶段；原失败五槽、清黑条件、独立空记录收尾/普通AutoMap、flag和三W16快照及返回保持。单次资源record取样同时借出原色/色调两地址，由上传端在record存在后读取mode选择；私有0xB04布局/两表/索引和记录池owner保持，不公开私有结构、新建全局类型或色表副本。

三限定文件补丁外全文及其它源码保持，既有450400核心/显式source/stride、普通palette算法均未改。1200旧语义及全部登记外部/政策保持；新增唯一F:00120B10后1201条/1198有效原ID、审核115/3680、局部证据75。18 pending/4分类引用/3无独立ID、任务10/60及叶0/182不变，完整RENDER/RESOURCE/G01/G02/G04与全局闭包不提升。固定源码快照为`cg2d-re-graphic-upload-source-20261005`。

原520D17额外dataref仅保留工具报告，不以未解释引用制造身份；绝对operand槽1020EC0明确。完整父/模式/上游/间接alias、资源表/节点初始化与失效、全部借用/退出清理、SDK/机器ABI/像素画面继续未验。未构建/查产物/新增或执行测试/运行/摘要，RPLAN.24异常不记安全/故障等价。
