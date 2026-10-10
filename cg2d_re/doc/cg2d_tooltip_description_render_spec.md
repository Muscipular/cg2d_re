# 描述提示绘制与文字入队 SPEC（TDR01）

<a id="tooltip-description-baseline"></a>
## 原端证据与范围

2026-10-11，已打开cg2d.exe只读会话。48B810完整108条283B/两页，六E8四目标/五跳一RET，九CALL八父及有限后邻有据。SPD01存放提示48B350仅其中一个已完整父。旧分色808/文字队列TQP01/图形BOS01已有完整合同，不重复取证，本步保其当前实现和历史。

TDR01仅48B810完整108条283B/两页，六E8四目标/五跳一RET，九入CALL来自八父及四有限后邻有据，不将对齐字节/下一原根纳入完整父。九参数x/y原S32、title/footer原非拥有文本指针、color原低S8、lines同源非拥有行指针/stride与lineCount S32；只新F/处置，不重计旧分色808或队列/图形根。当前完整正文正确保，仅定义/声明两合同注释，不为数目改功能或增加接口/helper。原16B descriptor全部零后+8 W32颜色FFFFFFFF/+13 BYTE4保；先456D50模式5清文字尾，456780入空593850字符串(0,0,mode5,font1,color0/value7-marker0)，不是清所有owner或绘制背景。title非空才DrawText((x+10)低S16,(y+10)低S16,5,12,titleColor低S8,title,0,0)，不新增空串/长度门槛。title处理后gTooltipColorD67D30 S32清0，每条description读取首BYTE非零才4FBA30；正文先signextend y低W16，再signextend x低W16，x+10、y+lineY+22按原正常S32传分色leaf，不把标题/正文的截断阶段互套。每轮不论空行或callee结果都lines+=stride与lineY+=13，循环原lineCount>0逐项，<=0跳正文但仍footer/背景，不新增上界或改成固定八行；当前SPD01调用96/8只具体caller，不外推所有九父。4FBA30为既有已完整核原94条分色leaf，只复用现源/历史：$前缀同源宿主临时块、失败返回0/调用者弃返，原绘制→测宽→X推进→源置delimiter+2→释放→S8数字颜色/global更新→再次查找，跨行共享color延续和正常尾段低S8色彩保持，不重建CRT或私自缓存码页转换。当前456780实际先写字段再扫描strlen+1，逐W32/余W8复制含NUL到gTextPendingRecords[].text[255]，前缀在返回后测宽再释放，因此本步同当前源的正常临时文本生命周期接线有据；不据本步重验既有队列原根/所有上游、空尾/续尾/全token或SDK寿命，不引全额容量/保护或延迟借用源指针。footer非空才DrawText((x+10)低S16,(y+127)低S16,5,12,footerColor低S8,footer,0,0)，不使用body颜色或因lineCount变化移动footer。末背景在所有文字之后DrawGraphic3(x/y低S16,layerE6,graphic3BA5D,value4=0,isGraphicNo=1,batchType=0,&descriptor)，返回真实S32原callee结果，不伪造成功或返最后文字值。既有real DrawGraphic3/图形队列与W16承载保，原native DX差异按获批边界，不借descriptor/返回接线关pixel或全生命周期。两整源仅两注释、八提供者(四原callee含分色、两text-copy helper、当前SPD01父与空字串getter)、两类型/两global及四旧语义保持；九CALL只局部来源，全八父/文字全队列owner-alias及背景图/SDK/生成读取/机器ABI/所有画面与互通运行待。一新F一adapted、UI-RENDER各一成员、两限定S零I八open；不引新字段/type/owner/API/helper、lineCount/index/空/容量保护/补零/默认/钳制/回滚或异常模型，MSVC/WAV-only/原命令字段LF明文及所有完整门禁保，整体未完成。

<a id="tooltip-description-decision"></a>
## 方案及取舍

按子AGENTS第8条委托先记方案，完整正常正文及局部失败处理已符合原合同，全保，仅定义/声明两注释。标题/footer与正文截断阶段不同，保留实际原序；不扩成固定八行API或将层5清尾改为全队列清理。既有文字入队即时复制与临时prefix后释放的正常关系可由当前源证明，不外推全SDK/所有资源owner/alias生命周期。

<a id="tooltip-description-acceptance"></a>
## 本步验收

完整108条连续覆盖、E8位移/五跳一RET与九入CALL四后邻；两整源精确回放仅两注释、body全部保，描述符及文字record类型、八提供者/两global/四旧语义保持。标题/正文/页脚阶段、原行门槛与颜色续接/背景返回及当前含NUL文字复制后prefix释放顺序静态有据；仅当前正常局部关系。新F/处置，两功能各一成员/两限定S零I八open，派生六metric/一F/两功能/八任务及汇总/-Check终态0；其它和完整门禁保。未构建/测试/运行/产物/摘要。

<a id="tooltip-description-source-state"></a>
## 当前状态

补齐层5描述提示绘制48B810完整108条283B/两页，六E8四目标/五跳一RET，九CALL八父局部来源有据。现正文正确全保，仅两合同注释；标题/正文不同截断阶段、跨行色彩、可选footer及背景最后入队/真实返回明确。当前文字含NUL复制与分色前缀释放的正常接线有据，一新F/处置、两限定S零I八open，完整owner/像素画面/ABI及运行待。

语义2333=2285F关联+48有界代码，关联2395/3680=65.08%仅关联率；处置1796/证据1601，UIRUNTIME206/RENDER117，其它保持。登记、静态汇总/-Check终态退出0及派生范围核对通过；仅本步静态合同，整体未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_tooltip_description_render_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐描述提示绘制与文字入队合同」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x48B810 | 0x48B92B | 108/283 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
