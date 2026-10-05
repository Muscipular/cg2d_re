# BMH01 战斗六类数据消息取样

日期：2026-10-06；按既有工程委托先记录后实施，沿[BMG01事件入口](cg2d_battle_event_execution_gates_spec.md)后的实际25头目标处理ACTION/FP/CHT/变身/追加技能/结束六类数据消息，修复五项正常取样差异。未核消息目标继续保留待办。

<a id="battle-m-data-sampling-baseline"></a>
## 原端有界基线

| case / 消息 | 原区间 | 指令 / 字节 | 区间内call |
| --- | --- | ---: | ---: |
| 12 / Action | 42A12D..42A166 | 16 / 57 | 4 |
| 13 / ConsumeFp | 42A166..42A22A | 41 / 196 | 9 |
| 14 / Chat | 42A22A..42A34B | 82 / 289 | 11 |
| 17 / ChangeBody | 42A520..42A768 | 134 / 584 | 28 |
| 23 / AppendSkill | 43797B..437A06 | 42 / 139 | 8 |
| 24 / End | 437A06..437A29 | 10 / 35 | 2 |
| 共用完成尾 | 42AAFE..42AB0C | 4 / 14 | 1 |

七个同属427470的有界区间共329条1314B、九指令页/63个实际call；前六块325条1300B/62call，不能漏掉FP失败/非负、变身失败或骑乘不更新、ACTION空角色等共用完成尾。[实际字节和源前态](cg2d_battle_m_data_sampling_evidence.json)可追。复用BMG01已证25槽表/真实43B63D data绑定，不重复重验其余目标或工厂。

既有438270完整9条27B再次按五个真实调用确认CopyPayload的encodedW32→maskW32→readCountW8逐读逐写；目标token/reserved不变、无明文读取/随机调用，原目标引用返回在这些点弃用，现代void承接沿旧221合同。正确实现/既有语义保持，不将9条重复算入329条父范围，也不统一成场景退出的三读后三写序。

## 六类生产、消费和返回

Action取位置并发布caster，借同slot角色；非空创建附属效果类型2，空与非空均Finish后由总入口回存，不补空角色或改变取包次数。ConsumeFp取位置、先借actor/判断存在再发布caster；存在才再消费扣除量，减FP及Less(0)前分别live复读caster/角色，负值分支构造0载体、构造后复读caster/角色复制并析构；否则不构造。所有路径Finish，再回存。

Chat入口substate58一次S32取样；0时取caster、color/font/duration，先发布A4再共享消息值，256B字符串未预清、原音频74/320门禁和聊天显示保持。聊天调用后live读caster；<20且借用角色非空时加载其graphicIndex，加载返回后重新读caster，再读共享lifetime，再按保存caster借当前角色；随后原DX缓存基址先取，才从同角色读graphicIndex W32、读取表内低S16高度取负，创建(-30,offsetY,lifetime,22335)效果。无角色或position>=20也按原递增58。substate1先A4−1/pub→共享消息值pub→**重新读A4**判断<=0才Finish；其它substate无消息动作但父仍回存，不改成用cached remaining判定。

ChangeBody同FP先actor存在条件再caster发布；空路径Finish且不再消费属性。正常路径图号映射后live复读caster/角色写imageNo，四次各自取值→Construct→live caster/角色→CopyPayload→Destroy，分别170/180/1B0/1C0，构造/随机消费及析构顺序保持。随后取riderValue，两轮分别取gear/firstImage/secondImage，SetGear前及SetAuxGear前各live借当前角色；末再借当前角色/载荷读rider13C8，非空且riderValue非0才按该角色imageNo更新骑乘，之后Finish。不能跨取包/构造/附件调用缓存旧角色或载荷。

AppendSkill依次取并发布caster/resultFlags，取technique后读同resultFlags；先比较400，再发布technique，再消费同条件追加类型8。非400路径**发布后**比较1000，命中追加类型11，其它不追加；随后CheckReady(scene,0)==1才live取角色406260并Finish。End也仅ready==1才Finish，之后sceneState=7；保Finish之后的状态发布及所有返回。以上六块返回现代helper后总入口仍Save13项，不把helper return当原父直接返回。

<a id="battle-m-data-sampling-decision"></a>
## 五项差异与现代承载裁决

| 差异 | 具体恢复 |
| --- | --- |
| FP角色存在条件 | 42A17C先测试actor，17E caster发布，184分支；保存actorPresent，发布后消费同条件。 |
| 变身角色存在条件 | 42A536先测试actor，538 caster发布，53E失败分支；同样保存条件，空路径保持Finish。 |
| Chat加载后取样 | 42A30C先读caster，311读lifetime，31D读表取角色；显式currentPosition先保存，不提前跨LoadGraphic缓存角色或lifetime。 |
| Chat图号与缓存 | 42A324缓存基址先读，32A读同actor graphicIndex，330读低S16高度；新增ReadGraphicDxHeightFromField(const S32&)在render边界内先native表基址、再读借用字段。参数只绑定确证S32字段引用，调用期有效，不转换成值临时或暴露DX/SDL对象。 |
| 追加技能首门禁 | 4379A6比较400先于9AC technique发布，9B3比较1000在发布后；只缓存首appendEight条件，第二比较时点及原flags取样保持。 |

两CPP六片段（Yobi五/缓存承载一）加声明头一片段，共七处；新读取接口是现代顺序承载、无独立原F/X/注册，旧ReadGraphicDxHeight及其它全部读取消费者保持。新接口不做加载、保护/补零/钳制/容量或所有权变更，已证native基址仅拓宽地址，索引仍S32/高度S16。CopyPayload、58/A4复读、原对象/随机/音频消费和所有其它正文保留。16B内存数值载体不属网络加解密，网络原格式明文/音频暂停继续。

<a id="battle-m-data-sampling-acceptance"></a>
## 验收标准

| 项 | 本步要求 |
| --- | --- |
| 原范围 | 七区间329条1314B九页连续完整、63call含共用完成尾，五CopyPayload点及既有9条27B合同区分。 |
| 实现 | HEAD精确重放Yobi五片段、缓存CPP新增字段读取及头唯一声明；原DX入口/其它消费者、核心CopyPayload、共享数据布局保持。 |
| 接线 | 两次actor条件先发布、加载后caster→lifetime→角色、cacheBase→graphic字段→S16高度、400比较先technique发布；六case与Finish/父回存真实可追。 |
| 台账 | 旧812追加有界六目标/共享完成尾和现代读取接口，无新原身份/adapted/完整成员；三域六局部I/S，七任务open。 |
| 验证 | 必要静态及获准文本汇总/Check；不构建、测试、运行或摘要，不宣编译访问指令同形/异常等价。 |

<a id="battle-m-data-sampling-source-state"></a>
## 当前状态与后续

六类数据消息五项差异落码，现代字段借用读取接入真实聊天特效；完整427470其余消息目标/全部资源owner重进退出与借用失效、SDK/SEH/ABI/PC53/时钟RNG/画面继续按实际缺口开发验收。原17298是含两chunk父统计，不以六块/63call或新helper计全父通过。

语义1350/有效ID1345、代码片段5、adapted456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680=39.76%保持；E347→353只三域六局部I/S，Render仅字段读取/消费者局部。R7区间67/3061、队列175、60任务及已闭10与全部完整门禁保持；关联率不是完成率。本组16文件，固定快照：`cg2d-re-battle-m-data-sampling-20261006`。
