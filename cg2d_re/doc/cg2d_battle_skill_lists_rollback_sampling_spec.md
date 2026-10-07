# BQP01 技能列表重建与制造回退取样 SPEC

日期：2026-10-07。沿BQO01初始化/返回调用链推进普通列表、宠物/骑宠模式与回退，按工程委托先记方案后实施。

<a id="battle-skill-lists-rollback-sampling-baseline"></a>
## 原版基线

[完整证据](cg2d_battle_skill_lists_rollback_sampling_evidence.json)七根393条1373B/九页、十物理call/六callee、37普通jump和一外部47A140尾转移、七ret；无switch/新R7。38个代码入向仅登记，四调用点47B0EF/47B0FD/485690/4856A2没有IDA函数父，只保真实指令，不把缺F当范围排除或另造F。BQO01已过初始化/返回完整父引用承接，不全面复检全部父。

辅助边界核事实：401EA0五字节E91B000000尾跳401EC0，现坐标Read401EC0承接正确，不能误认只读/不同解码；401F80(cost,rightFP)经同别名取右FP一次，比cost>FP，保有符号比较、读取计数与可能RNG。401EC0的既有机制不另实现，RPLAN.24下不专项其异常/SEH分支或模拟。

<a id="battle-skill-lists-rollback-sampling-decision"></a>
## 工程裁决

四体六片段：479B80 position按原EAX计算/发布/条件清0/传slider同一次缓存，不重新读共享scroll；479660减后counter先发布，分支用同cache/W8 flag短路；旧occupancy[7] W8取后先减值，再清flag再回写cell。两宠物列表初界与每轮尾count W32先读取后index++，骑宠名称按position→elements→action→textData取样。

新增一个明确bool/S32/YobiAction*非拥有桥`IsCostGreaterThanYobiActionFp(cost, action)`，在Yobi实现内把私有FP借给已存在的IntGreaterCrypt_401F80，公共接口不暴露FP存储。只有模式5/6两父接线；旧LessThan逆向比较正常数值和单读副作用等价，不把它记数值/计数RNG故障，不全局替换其它消费者。不删弃返FP检查。两列表重建/原W8减计数与比较叶三正确体保；原负maximum/输入失败和名称旧字节保持，不擅加钳制、拒绝、补零或故障/回绕模拟。网络511DD0原MIS(socket,x,y,62)接线保，原格式明文，不新增加解密/协议/解码或外部依赖、RTTI。

## 七函数完整正常合同

### Sub_479B80（0x00479B80）

479B80普通技能列表：15配对按index S16查输入skillIndex，首匹配或无匹配回row0，不以used过滤索引搜索。原EAX held position=row-9先算、mode W32=0后scroll=position发布，position负与skillIndex负两原路径各position=0/scroll=0。slider参数position取held值，不再读共享scroll；之后fresh oldMaximum W32、elements[11] T*传4742C0弃返。1..16隐藏、1/3显示；15 used W16计数，maximum=count-10原负时0；10..16显示，27.data图3BA24、17..26写47B820真callback，保持fresh elements逐次读取。79条256B/一页、一call/12jump/一ret；只一个position计算/缓存片段实修。

### Sub_479C80（0x00479C80）

479C80普通技术列表：mode W32=1/scroll0，pairIndex S32非负才取配对index S16存选中技能，负保持旧值；oldMaximum W32先取，elements11 T*传4742C0(max,0)弃返。1..16先隐藏，2/4/5/9显示、6再显式隐藏；fresh selectedSkill技术name[0] W8逐行到首NUL/15计count，maximum=count-10原负时0；10..16显示后14/15隐藏，27.data图3BA26、17..26写47BC00。75条273B/一页、一call/七jump/一ret；完整正确体保，不造新clear或业务分支。

### Sub_479660（0x00479660）

479660制造回退：原D67AF8 W32一次读/减1/先发布，分支复用减后held值；非0返回、不读W8 flag5F9754；零才读flag，零仍返回。有效才Y GetVal401EA0→X GetVal401EA0→fresh socket调用511DD0(socket,x,y,62)弃返；401EA0仅E91B000000尾跳401EC0，现Read401EC0承接正确/两Y-X单读和计数RNG保持，不误改成只读getter或另算法。发送后旧occupancy[7]实际W8取样、寄存器减值→flag W8=0→原cell W8回写；然后原47A140尾跳对应源void调用后退出，不虚计原call。24条85B/一页、三call/两门禁jump和一外尾转移/一ret。两片段仅counter held分支、旧cell减值在flag清零前；W32/W8储存及正常S32算术、短路和真实原MIS网络三参序均保。

### Sub_4796E0（0x004796E0）

4796E0仅DEC BYTE D67AFC/RET：2条7B/一页、无call/jump、一ret，源uint8--正确体保，不扩大为其它计数/cleanup、不声明异常或回绕模拟。四原调用点47B0EF/47B0FD/485690/4856A2位于未列F的代码区间，只登记真实CALL/无IDA父，不据此排除或伪造F身份。

### Sub_47A1A0（0x0047A1A0）

47A1A0宠物技术列表：mode5/scroll0，1..16隐藏、2显示/maximum0、10..16显示先完成，然后GetBattlePetIndex返回S32负则直接退出，不清新增字段或假创建。有效固定pet T*，user name6BD首W8非NUL优先，否则name6AC，复制原旧字节到window元素2.text。一次pet.slot value6A4 W32初界，逐pair index S16取tech record，name首W8非NUL才fresh position→405B40 petPosition→fresh entry T*→record.fp W32；原cost-first401F80(cost,&action FP)经typed bridge，单读右FP/计数RNG且弃bool返仍调用，不合并或删除。每轮尾fresh slot W32先取、后index++，再held count比较；结束8显示、fresh slot-10写maximum不钳制负值、7显示、27.data图3BA28、17..26写47CC80。108条380B/两页、三call/九jump/一ret；一个loop片段明示尾界取序及cost-first接线，普通数值/FP读取次数与旧实现一致。

### Sub_47A320（0x0047A320）

47A320骑宠技术列表：mode6/scroll0、1..16隐藏/2显示/maximum0/10..16显示后读取ridePet W32按S32负则退出。有效fresh position→elements T*→displayAction T*→元素2.textData T*，借displayAction+C4旧名称复制，不新名称副本或改容量；固定ride pet T*、一次slot W32初界。各pair index S16/name W8非NUL才fresh currentPosition/entry T*→record.fp W32→cost-first401F80单右值读/计数RNG，弃返不删调用。每轮尾先fresh slot W32再index++，出口8显示/fresh slot-10原负值保持/7显示/图3BA28/17..26写47CFE0。100条355B/两页、一call/七jump/一ret；两个片段恢复名称目标取序和loop尾界/cost-first接线，不改其它业务。

### IntGreaterCrypt_401F80（0x00401F80）

401F80正常比较叶：arg1 LegacyCryptInt*借右对象，调用401EA0→401EC0一次，后读arg0 S32并按有符号lhs>rhs置AL bool返回；5条17B/一页、一call/无jump/一ret。既有IntGreaterCrypt_401F80(lhs,rhs)正文正确保，仅新增其原语义基线/equivalent与BATTLE依赖身份。新login::detail::IsCostGreaterThanYobiActionFp(cost,YobiAction*)以action私有barValue1B0调用该叶；公共边界仅bool/S32/非拥有T*，不暴露私有FP字段/存储，不添nil成功或RNG/解码替代。旧IsYobiActionFpLessThan的正常反向比较与一次读取等价，原消费者不全局替换；只两宠物列表接新cost-first桥，不称旧正常FP数值或计数RNG错误。15原入向只登记，不推全部父或异常/生成机器ABI验收。

<a id="battle-skill-lists-rollback-sampling-acceptance"></a>
## 可验收范围

七完整原根每条指令/字节/边界、call/尾转移/返回与模式回调写入可追；六已有片段及新增桥/声明按精确差异，三正确体和其它源码/共享字段保。别名401EA0→401EC0承接、两原401F80检查→桥→已存在平台叶单读、loop尾界/回退发布/短路与原MIS发送后序逐项静态核。辅助事实只限定相关正常调用，不代重验整个解码/异常或全部父。

新增1534..1540七正常身份/equivalent，语义1534→1541/有效1529→1536、关联1647→1654/3680（44.95%）、分类660→667/BATTLE212→219枚举open/E782→784，两局部I/S各二十真实来源；14精确文件。R7 262/3061余2799、差队列172/待归属43、U08/R01/R04/V04仅当前/引用仍open，与60任务源码closed10/完整验收0/叶0/global=false保。汇总器和-Check生成看板/快照，不手改生成件。

不构建、调试、测试、运行客户端/样例或计算摘要。完整生产/未决mode callback/全部父/owner寿命、窗口10/26及57C368 token、SDK/生成机器ABI/画面运行和R8不据本步验收；音频暂停及RPLAN.24限制保持。

<a id="battle-skill-lists-rollback-sampling-source-state"></a>
## 当前交付

七完整393条1373B/九页、十物理call/六callee、37普通jump+一47A140外尾转移/七ret，无switch/新R7。四既有体六片段恢复普通列表held position、counter减后held谓词/旧W8 cell取序、两宠物loop初/尾W32界先index++与骑宠名称目标取序；新增一bool/S32/非拥有T* cost-first桥及声明，两父接现正确401F80。普通数值/原FP单读次数和计数RNG一致；两列表/字节减计数/比较叶三正确体与原回调、失败/负maximum、原MIS原格式明文及Y-X/socket序保持。401EA0实际尾跳401EC0，既有坐标取值正确不改；无新解码/网络加解密。38代码入向仅登记，四无IDA父代码区间CALL只真实登记不伪造F/排除；其它生产/模式callback/全部父/owner寿命、窗口10-26/57C368 token、SDK/生成机器ABI/画面运行及R8未验，音频暂停与RPLAN.24不专项异常/保护/模拟保。

命名快照：`cg2d-re-battle-skill-lists-rollback-sampling-20261007`；唯一进度记录实际检查与提交。
