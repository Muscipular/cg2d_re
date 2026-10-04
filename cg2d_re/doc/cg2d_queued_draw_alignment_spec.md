# 完整队列绘制与DX基址取样对照 SPEC

> GQD01；2026-10-05依工程委托采用，沿[DrawAllQueued完整实施合同](cg2d_sdl_presenter.md#drawallqueued-完整实施-spec)、[DX字段/链合同](cg2d_graphic_dx_cache.md)、[GUP01](cg2d_graphic_upload_alignment_spec.md)和RPLAN.24。对应G01/G02/G04；先記决定再修实际源，保留完整13批次及既有CPU native边界。

<a id="queued-draw-baseline"></a>
## 1. 原证据与当前差异

[只读证据](cg2d_queued_draw_alignment_evidence.json)保存520DD0完整769条/2658B，[520DD0,521832)，14直接引用及13项跳表。三纹理路径分别整数、对象缩放和固定三比例；本组不以一个路径替代其余批次。

| 阶段 | 原读取 | 决定 |
|---|---|---|
| 普通crop初始界限 | 520EEA一次取DX基址，520EF7宽/520F08高都沿该基址；先X界限通过才读高 | 保存dxBase，分支中读取宽/高，不把两个getter重取的基址当原同一次 |
| 负坐标修正 | 仅X<0时先取完整originX，调整宽/原点及X=0，再520F25重取基址；Y<0同理520F41 | 保每个原分支的基址读取，不无条件更新或移到两个分支之后 |
| 右下裁剪/链头 | 520F55/520F64均沿最后保存ECX基址，520FC4或5211EB的head也沿它 | 最终宽高/链头用保存基址；私有遍历helper接实际head，不在helper入口重读全局表 |
| 普通dirty收尾 | 初始head空直接5213C1沿保存基址取palette；有链处理后5213BB才重取全局基址 | 区分空链与遍历后。只有非空链处理完才更新dxBase，原dirtyCount清零/早退保持 |
| 固定三比例 | 5214E9重取基址读当前宽，521522取head继续沿它；head空5216D1也沿它，遍历后5216CB新取 | 同样传保存head、仅非空处理后重取基址；不把固定路径改为虚拟action或普通crop |
| 初始Back门禁 | 520DF2的COM+60H操作，非零直接521434返回，只有入口上传count清零 | 已批准IsLost→唯一CPU Back存活映射保持；GetCpuBack为空不是本组误删的额外门禁 |

原COM+60/14/18/1CH只保存间接操作数与未解析目标，不把偏移当callee绝对地址。完整769条取得不等于全部callee/14完整父/资源寿命或运行通过；本组集中核已批准范围内根本体及六处基址/链头源差异。

## 2. 完整批次与保持项

- 入口上传count清0、Back失效早返、各Has/Begin/End位置沿原CPU映射；非正S16计数也仍执行正常公共尾。cache3的S16索引定位48B承载，W8 batch只分发0..12，其余走公共下一项，不重排/清两个计数或文本链。
- 普通0/1仍图号门禁→独立表刷新→实际LoadGraphic，成功后新取batch，仅0改原描述符Y/X零、低W16宽/高。S16矩形、负坐标对完整S32原点修正、缩右下/非正早退保持；任何早退不清palette.dirtyCount。
- 整数节点先Frame→blockX→reuseState写，再X右/Y下/X左/Y上界；缩放先Frame→blockY→reuseState写，再Y下/Y上/X右/X左。只相交才上传，返回0继续，处理完才读取next；三路径均保矩形left/right/top/bottom各原阶段。
- 对象scale的x87 C3相等/unordered既有判断保持：X命中才读Y，整数/缩放分发不改。动态绘制固定本轮action供各轴F32取值、先X后Y与原点/镜像读取次序保持；固定0.25/0.5/0.75不读action。现有F64/向零S64及低位S16承接不在本组重设计或声明机器浮点ABI通过。
- 原三upload点由两遍历helper共用，真实GUP01上传/dirty精确busy、三render回调参数/矩形和调色板字段取样不改；只传原head，不移动节点、建新链或改变借用寿命。
- case2/3矩形/FillDirtyArea，case4独立Has两次End→原文本→Begin，case5先Y后X，case6/7先C82204/原X后Y及无色键矩形/整幅复制，case8波纹、case9条件效果均保持。case10/11/12仍固定路径，不遗漏或重写已实现批次。
- 公共尾先Has/End；仅上传count为0才把628FDC版本写独立58B7E0，再AgePalette、清特殊W8。Back失效跳整个尾、队列每轮先新读S16 count再本地项数/游标推进，owner不变。
- 调色板dirtyCount属于PaletteCacheItem+0C，不是DX项；仅几何/读取成功且链头空或链处理完才判palette非空清0。头空与非空的基址阶段各沿真实指令，不能“统一新读”或“统一旧快照”。

<a id="queued-draw-decision"></a>
## 3. 实施决定

1. DrawAllQueued.cpp限定六处补丁：两private helper接保存TextureNode* head，普通crop基址/负坐标原点取样和普通尾，固定比例当前宽/尾。各公开接口、所有native/其他helper算法、分发和owner一字不改。
2. 普通crop采用DxGraphicEntry* dxBase保存一次初始界限基址，按原X/Y负分支分别重取；当前裁剪/初始head和空链palette沿它。非空链所有节点处理结束后仅一次重取全局DX基址，再dirty收尾。固定路径保存5214E9对应基址，按同一原规则处理head/dirty。
3. 私有helper不自行按imageNo重新定位head；只借调用者传入的非拥有TextureNode*，next在当前节点处理后读取。保两原节点循环顺序、field widths、GetGraphicDxWidth/Height在实际镜像分支内的原新读及原getters，不引新类型/工具文件或公开SDL对象。
4. 769条原完整根按适用scope登记唯一映射/限定处置；I/W/S各一，RENDER/RESOURCE成员增加，G01/G02/G04仍open。14原引用与13源码调用公共复用具名记录，不把该关系当14父/运行验收。
5. 原实施合同局部补基址生命周期说明；根完整C++ source已有且本步实修，不将旧“DrawAllQueued未实现”当当前缺口。不刷新总SPEC/索引、其他历史或原SDK替换边界。
6. RPLAN.24异常/越界/数学溢出不专项模拟，不新增保护/默认值/清理/新失败，不称x64安全或故障等价；未构建/查产物/新增或执行测试/运行/摘要，单线程、音频暂停、原格式明文网络保持。

<a id="queued-draw-acceptance"></a>
## 4. 局部验收

| 门禁 | 本次标准 |
|---|---|
| I | 六限定源码补丁落实原DX/crop/head/dirty各阶段，完整分发/两循环/三纹理及各原门禁/失败写尾保持 |
| W | 原14call/源码13调用与两fade父共用、传真实head/唯一队列节点和Back/场景/实际回调可追；全部父/间接alias/资源寿命不验 |
| S | 根769条连续边界、跳表/真实calls和未解析COM操作区分、六補丁外全文/旧台账保持；原获批CPU门禁/接口不扩豁免 |
| R | 同资源/输入/初值的完整批次/模式/画面、裁剪/缩放、所有资源借用失效与场景/退出清理仍未验，执行限制下blocked |

G01/G02/G04、完整功能门禁/全局闭包不提升。

<a id="queued-draw-source-closure"></a>
## 5. GQD01源码小步结论（2026-10-05）

六处源码补丁已实施并通过集中静态核对：完整根769条/2658B连续至实际边界、13跳表/默认路径、14caller局部112条有证。32个调用操作中7个COM偏移保持未解析，不当原函数地址；14逻辑调用由13源码调用承接，两fade父loop共一个公共帧尾，FadeIn另有最终直接调用。

普通/固定DX基址的界限→分支重取→clip/head→空链/非空dirty尾沿原阶段恢复。private helper只借保存head，不另定位。六补丁外整份源保持，公开分发本体、矩形算法及两遍历循环除初始化之外全文不变；原13批次/三纹理、各门禁/上传失败继续/next、浮点和native IsLost/场景映射、队列count重读及公共尾保持。只有DrawAllQueued.cpp改源，其他源/头/父不改。

1201旧语义全文、75旧证据/115旧处置及全局policy和登记外部保持；新增唯一F:00120DD0后1202条/1199有效原ID、审核116/3680、局部证据78。3无独立ID/18 pending/4分类引用、任务10/60、叶0/182、完整RENDER/RESOURCE/G01/G02/G04和全局闭包不提升。固定源码快照为`cg2d-re-queued-draw-source-20261005`。

完整14父/所有callee和上游、间接alias、节点/表/文本/对象借用及场景/退出清理、SDK/机器浮点ABI、同资源输入的全模式/像素画面运行仍未验。未构建/查产物/新增或执行测试/运行/摘要；异常不记安全/故障等价。
