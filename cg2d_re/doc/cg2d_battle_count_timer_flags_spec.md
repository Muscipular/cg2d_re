# 战斗计数、玩家计时与字节状态发布 SPEC（BPV01）

2026-10-07；推进动作165..170剩余本体与真实消费。工程委托有效，原事实来自已打开IDA只读会话。

<a id="battle-count-timer-flags-baseline"></a>
## 完整原根与具体差异

| 动作/原根 | 完整原指令 | call/普通jump/ret | 唯一入向 |
|---|---|---|---|
| 165 / 411D30..411F98 | 182条616B/三页 | 12/19/5 | 4053F7 |
| 166 / 411CB0..411D2B | 37条123B/一页 | 3/2/1 | 4053EC |
| 167 / 411FA0..411FB3 | 6条19B/一页 | 1/0/1 | 405402 |
| 168 / 411FC0..411FD3 | 6条19B/一页 | 1/0/1 | 40540D |
| 169 / 411FE0..411FF3 | 6条19B/一页 | 1/0/1 | 405418 |
| 170 / 412000..412013 | 6条19B/一页 | 1/0/1 | 405423 |

合243条815B/八页、19call/七被调入口、21普通内部jump/10ret；六普通点同父404DB0，只18条压action/弃返/公共尾邻窗。[完整证据](cg2d_battle_count_timer_flags_evidence.json)保所有指令、四原F32常数与既有计时源绑定。

165的411DC1直接TEST BYTE flagsFDC,2，411E50 MOV AL BYTE后TEST2；现两W32掩码消费恢复原低W8对象表示读取。初始411D68的W32缓存/后段TEST BL和中段fresh W32门槛保，正常旧掩码值一致，不声称数值/并发或生成机器故障。

原角度FCOM先比较未落位中间量再FST到F32，原速度FST保中间量再FCOMP；现有double比较承载该阶段保持，不能误改为重读已落位F32。166的正常原S32内联数学是timer-1000*seconds及负值业务归零，既有helper对应同一共享owner，不造原call/F。

<a id="battle-count-timer-flags-decision"></a>
## 实施裁决与完整正常合同

165：FE4原W32/C04决定character8/9，rate50；初始flagsFDC W32缓存bit1，FCC按S32先增/发布。count<14*(bit+1)时角度+180保未落位中间比较360/F32发布与必要减360、speed5，fresh低W8 bit2设gate1/alive1→Move→PlayFrame1返回。count<14*bit+34时方向S32+1/达到8减8，fresh W32 flags重算门槛与fresh FCC比较，相等时speed5、fresh低W8 bit2及CountActive<=1才gate0，alive0/FF00；FF0==0建图110006，fresh旧FF0先增且旧值>=2清0；speed减原F32 0.3保未落位中间判定和原零业务限制→Move→PlayFrame1返回。count>=14*bit+54时FCC0，原初始缓存bit2清则6/100→439340→fresh phase11时4/100；否则fresh FE4/3010决定6/100或1/250→439340。其它公共尾PlayFrame1返回；缓存与fresh W32不得合并。

166：先共享当前玩家位置再借payload，位置相等时fresh queuedTargets[value13A4]供既有ConsumeBattlePlayerTimer，正常timerRemaining扣1000*seconds先W32发布、负才业务归零。这是原411CC5..411CFD内联数学承接，不是原call或新F。随后fresh index→Y→value→X、4206C0(x,y,-value)弃返→43B830弃返→439340；保S32算术/业务下限和原队列、计时共享owner，不加异常回绕/保护。

167：原W8字段308写8→439340，现有SetYobiActionField308同一原字节承载，正确本体保。

168：原W8字段308写0→439340，原字段别名和调用序保。

169：原W8字段309写255→439340，现有SetYobiActionField309同一原字节承载，正确本体保。

170：原W8字段309写0→439340，原字段别名和调用序保。

只165在移动及计数门槛后旗位阶段各一次unsigned byte对象表示读取，字段仍W32、所有写者保；其他原初次缓存/fresh门槛、FE4 W32和S32计数、方向及未落位浮点比较保持。54D438/54D424/54D464/54D420原F32字节分别00003443/0000b443/9a99993e/00000000，180/360/0.3/0值与真实F32存储可追，实际x87/生成机器精度未验。

166与四字节发布动作正确体保持，ConsumeBattlePlayerTimer仅原内联数学源承接；43B830/439340原弃返不变，不扩展计时策略/协议、数学故障或队列寿命。

<a id="battle-count-timer-flags-acceptance"></a>
## 限定验收

- 六完整根243条815B/八页、19call/七入口、21普通内部jump/10ret与六唯一父18条邻窗、四实际F32常数、两W8消费点和166正常内联计时可追。
- 整个YobiRuntime仅165两片段，另五本体及计时helper/头和全部其它源保持；原缓存与fresh W32、未落位比较/实际存储、计数/字节发布及返回消费保。
- 新增1440..1445六原已有定义正常语义/equivalent：语义1440→1446/有效1435→1441、关联1553→1559/3680（42.36%）、BATTLE117→123枚举open、分类565→571、E742→744仅两局部I/S。R7 259/3061/余2802、差队列172/待归属43与全部完整门禁保。
- U08/R01/R04/V04只当前/引用仍open，60任务原关闭条件保。六完整原根243条815B/八页、19实际call/七入口、21普通内部jump/10ret、六唯一普通入向同父404DB0的18条邻窗与四原F32常数，只当前完整正常源计数/计时/字节发布及接线静态。165两旗位阶段恢复实际低W8，正常旧掩码值一致、不称数值/并发或生成机器bug；原未落位浮点比较、缓存与fresh W32、S32数学、166既有内联timer承接及五正确体保持。计时helper不新造原call/F或复制owner；全父/全部子依赖与其它生产/alias-owner寿命、SDK/生成机器ABI及实际x87精度/时钟RNG/画面运行未新验，原S32业务范围和RPLAN.24不专项异常/保护/模拟/故障等价保。

<a id="battle-count-timer-flags-source-state"></a>
## 当前交付

计数动作两W8消费恢复，六完整正常本体及计时/字段发布接线纳管；快照 `cg2d-re-battle-count-timer-flags-20261007`。未编译/调试/测试/运行/摘要，完整战斗/生命周期/画面与R8未关闭。
