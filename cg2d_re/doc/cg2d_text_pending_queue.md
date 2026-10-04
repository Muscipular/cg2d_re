# 文本待提交链还原

## 目标与证据

[TQP01](cg2d_text_queue_alignment_spec.md#text-queue-producer-baseline)与[证据](cg2d_text_queue_alignment_evidence.json)保存三个绘制入口及图形提交入口的完整原体和854实际call点。

| 原入口 | 原完整指令/范围 | 当前实现 |
|---|---|---|
| DrawText3 | 79 / [456560,456668) | 保既有真实DrawText2复用font0；业务字段/分支/返回按原对照，不宣称原栈或机器call轨迹 |
| DrawText2 | 80 / [456670,45677B) | signed8模式、参数字体、固定marker F9；恢复原复制/取尾与空链层取样 |
| DrawText | 81 / [456780,456891) | unsigned8模式、参数marker；W32复制后/W8前读取尾槽 |
| QueueText | 22 / [521A30,521A98) | 原signed16满队列失败及真实记录写序/重读/返回保持 |

四入口引用15/19/816/4，父数14/12/188/4；只是原引用库存，不是所有上游/原新参数和寿命已验。只有三绘制signed32文本计数>=4096、图形提交signed16计数>=4096的原门禁返回−2，额外保护已按具名决定撤销。

## 文本记录与令牌

唯一文本池是4096项gTextPendingRecords，原62ECB8步长110H；六槽尾队列原62ECA0，字体层原558F50。记录+0/+2坐标S16，+4/+5/+6/+7颜色/字体/模式和值7，+8标记，+9为255B文本，+108为本组不显式写的S32字段，+10C是现有下一项一基token。header/owner/初始化不改，不整项清零或清NUL后旧字节。

gGraphicCache4Queue保持六个W32槽，0为空，i+1表示文本记录i；原字体层四字节记录D2/D4/D8/DF/E2/E6及各后三零字节保持。QueueText继续复用唯一LegacyPendingStorage，原FF4EB8/101CEB8/1020EB8身份和既有x64记录48B承载不变，不建立第二图形队列。

独立悬停索引D68BEC的gHoveredTextPendingIndex初值沿原零填充。当前[GraphicHitTest.cpp](../src/render/GraphicHitTest.cpp)已有Sub_507140及共享槽生产；旧“尚未实现”过时，本组不改/验该生产者的完整业务/寿命。FindClickedTextIndex_45D7A0先单次取鼠标低字节bit0，再判count<=0，再取悬停索引；正计数按原返回首个相同下标或−1，源码保持。

## DrawText2写入与排队

signed W32文本计数>=4096才返回−2。否则按入口计数写x/y/mode/color/font/value7/F9，再扫描原source；含NUL数量低U32，逐W32读/写前移，最后余W8。不提前校验模式/负计数/指针/255B NUL或改逐字节顺序，不显式写field108。

完整复制后按signed8模式取尾槽，再清新记录next。空尾先读取实际fontLayer.layer、更新尾槽，真实QueueText返回弃用，再重读文本计数/返回该signed32旧值并写U32加一。非空先连旧尾，再重读计数，以该值更新尾/返回/递增。不增加tailToken或重读索引拒绝、补NUL/回滚。

两个容量独立；即使QueueText因图形容量满返回−2，已建立的文本链和随后文本计数推进仍按原保持。当前输入绘制多行复用真实入口，单行按原阶段直接写同一池/链，详见[ITD01](cg2d_input_text_alignment_spec.md#input-text-draw-source-closure)。

## DrawText3固定字体复用

原正常业务与DrawText2差异只有font0。当前保既有真实core复用：DrawText3_456560将字体0传入DrawText2_456670，同一真实字段/复制/分支/计数路径执行；不是固定成功或默认正文。三个原QueueText入边中的本入口经这一调用链承接，不能把词法call数量当缺体或原栈/机器调用轨迹等价。

本组不改包装源码；校正真实core后沿font0原字段/正常路径归档，只限定功能静态对照，原native分解/故障与全部caller另行验收。

## DrawText差异

模式低unsigned8、字体参数、marker第八参数保持。它先写记录/扫描/复制W32，随后读取对应尾槽，再复制余W8，才清next；这一中间取样不能与signed入口合并。

空尾层先读/尾槽后写、真实QueueText弃返回以及续尾/计数重读/returned S32与written U32均按原保持。新端额外负计数/模式/源/限长/损坏尾与重读索引拒绝已撤销，不加新的容量补值或恢复动作。

## QueueText写入

图形W16计数先按signed16解释，只>=4096返回−2。通过后，初始索引写cache3.layer，再取当前W16写同一cache3.cacheIndex；顺序写imageNo−1、value4=0、batchType=4、reference.textToken；末重新读当前W16，signed扩展返回并低W16加一。

不写cache3+3、记录坐标/descriptor/尾字节，不检查headToken为零/域外或新增负计数失败。原文本指针沿既有一基token承载，池/布局/owner保持；异常对象/索引/越界/溢出依RPLAN.24不专项模拟、不称安全或故障等价。

## 当前未闭合项

原ResetGraphicCache_456D30的既有局部源码收口仍保持：清六W32尾槽、返回0，不清文本记录/计数；“收口”不代整个运行寿命。

当前[DrawAllQueued.cpp](../src/render/DrawAllQueued.cpp)已实现完整批次分发，case4真实调用RenderTextToSurface_456D70，旧“只有声明/未接”已过时。原521723取图形记录+0C/521727调用456D70的局部窗口与当前token消费相互对照，本组不验全部520DD0。文本消费/共享Back DIB依既有[消费SPEC](cg2d_sdl_presenter.md)继续推进；原间接虚调用operand18H保持未解析，不能当绝对callee。

[TQP01局部验收](cg2d_text_queue_alignment_spec.md#text-queue-producer-source-closure)只四生产根/实际生产消费局部；854全部上游参数与业务、间接alias、排序/完整消费、初始化退出和输入渲染寿命、SDK/字形/画面运行未验。未构建/查产物/新增或执行测试/运行/摘要；单线程、音频暂停及网络原格式明文保持。
