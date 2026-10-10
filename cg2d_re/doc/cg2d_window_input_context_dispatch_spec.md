# 输入原型四模式分发与上下文字段发布 SPEC（WIC01）

2026-10-10。沿WDP01/CFD01核输入元素接线，依子AGENTS第8条先记原事实和边界。同一cg2d_re分支、不打tag；现GBK、原格式明文、MSVC CRT及WAV-only保持。

<a id="input-create-baseline"></a>
## 原根及旧分发表

| 原根 | 完整范围（尾不含） | 指令/字节 | E8/跳/RET |
|---|---|---|---|
| 476570，原型四模式分发 | 476570–47666D | 103/253 | 3/4/5 |
| 45D720，坐标/模式/颜色及滚动归零 | 45D720–45D75B | 13/59 | 0/0/1 |
| 45D760，坐标/颜色/模式/字体 | 45D760–45D797 | 12/55 | 0/0/1 |

共128条367B/四页、三实际E8/三目标、四跳七RET；80普通CALL入向，其中分发到两叶内部2，其余外部78/53父仅来源定位。原四槽表476670为4765A8/4765F7/47661B/47663B；旧1935/F:00076570及X:0007666D-00076680的19B范围保历史，本步追加完整分发合同，不重复原F或重分类。

旧372/449CF0初始化、CFD点击父与CWC初始化父已有对应证据，保身份；本步不据调用引用外推它们或其余父完整生命周期。

<a id="input-create-decision"></a>
## x64表示与模式合同

原首W32先拒绝0，再拒绝高BYTE非零，返回AL0。新端沿[既有带标记引用裁决](ui_windows/shared_runtime.md)保持WindowInputReference：Context分支承载非拥有InputContext*，空值为nullptr；RejectedValue分支承载原拒绝W32并保低24位。该适配已明确不把原FF000000掩码套到实际x64指针，也不恢复地址令牌表或原数值地址。原型x/y/renderMode/fontType/color/maxLine/maxCharPerLine/value224/encrypt均保S32，文本为现有204H字节区；表示及其生产者不在本步改动。

有效context的mode按原U32分派。mode0先encrypt→value224→maxCharPerLine→maxLine→text地址→color→fontType→renderMode→prototype.y+y→prototype.x+x，调用449CF0初始化并返BYTE1。持有入口context，不把原字段改成合并快照或新容量策略。

mode1先renderMode→color→maxCharPerLine→y坐标→x坐标，调用45D720，然后落入mode2；第三个整数实参maxCharPerLine在45D720中原本未使用，保未用，不推导为新限制。mode2先x坐标→重新读原型context→写field130，再y坐标→重新读原型context→写field134，返回BYTE1。落入前callee可能改变原型引用，两次现场context读取不得合并为入口context。

mode3先fontType→renderMode→入口context.field140→y坐标→x坐标，调用45D760后返BYTE1；颜色来自真实context，不能改为原型color。mode超3也原返1，无新动作；不新增无效mode错误或固定额外成功分支。

45D720持有实参context，依次发布field154=0、field158=0、field134=y、field130=x、field138四BYTE=renderMode、field140=color，返同一真实context地址。45D760依次发布field130=x、field134=y、field140=color、field138四BYTE=renderMode、field13C=fontType，返同一context地址。现有memcpy保field138四原始字节，保未写的字段和文本，未扩成新清零。

本链只初始化/更新既有context，不创建新载荷；完整输入元素载荷owner另有生产释放链。三个正确正文全部保持，零逻辑组、三定义/三真实声明六关联注释；没有新API/函数/类型/字段/布局/owner/分配释放，不新增保护、默认值、补零、钳制、容量限制、回滚或异常/溢出模型。原空/拒绝值门槛及滚动零发布保持。

<a id="input-create-acceptance"></a>
## 限定静态验收

核三完整原根、全部实际E8/跳/RET、内部2/外78入向、原四槽表及旧19B范围；四整源精确重放六注释，三个body及其他source/类型/表示保持。核真实公共与内部声明及field138四BYTE、现场context重读和返回，不代编译布局或优化后机器读取验收。

旧1935全CAC08来源/表/范围子记录保后追加、两个新语义/三个F处置，INPUT/UIRUNTIME各三成员、两限定S零I、九任务open；所有枚举、完整门禁、六closure和总SPEC§10保持，关联率不是完成率。

仅必要文本静态与measure_cg2d.ps1/-Check，不编译、测试、运行客户端或算法、查看产物或计算摘要。53外父、原型标记/所有context/输入元素载荷的全部生产消费与alias-owner寿命、449CF0其余依赖/字体/IME/CRT/SDK/机器ABI/优化后读取、实际初始化/切换/焦点/编辑和全生命周期仍待。

<a id="input-create-source-state"></a>
## 当前状态

本步前提交说明“[#] 补齐数字绘制初始化并明确测宽顺序”；拟提交说明“[#] 补齐输入模式分发及上下文字段发布合同”。三完整原根/四槽旧表、六注释精确差异、旧1935保历史追加、登记、静态汇总/Check与派生范围核对通过。语义2258=2210F+48X、函数处置1699/证据1497，文档关联2320/3680=63.04%仅关联率，INPUT78/UIRUNTIME114且枚举未闭。184叶验收0、60任务关闭11/开放49、六closure开放及全部完整门禁保持；未构建/测试/运行，整体active/未完成。
