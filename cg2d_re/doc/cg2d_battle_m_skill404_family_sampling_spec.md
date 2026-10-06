# BNO01 技能404/406共用主体取样与骑乘生产

<a id="battle-m-skill404-family-sampling-baseline"></a>
## 原端与范围

只修复 `src/YobiRuntime.cpp#ProcessBattleMSkill404Family`。原端完整正常主体为43097A..431C9C（半开），同属父427470，1286条4898B、每页最多80条共17页、147call；全部直接目标在本段或父默认437A29，真实尾在段内，无复用尾。[证据](cg2d_battle_m_skill404_family_sampling_evidence.json)保存全部连续指令/字节、调用/出口、改前根、23补丁和旧812完整前态。完整父17298/SEH、所有owner/消费者与运行另验。

404沿[BNM01实际180B压缩路由](cg2d_battle_m_skill263_400_401_sampling_evidence.json)→43097A；406沿本步实际42FC06比较406、42FC0C大于分支、42FC12相等直跳43097A，三节点18B只定位406，不能误归为压缩表case。主范围之外的路由节点不计1286分母；既有表不重复计新增，不宣其它case全部开发。

[BMO01](cg2d_battle_m_skill4_sampling_evidence.json)具名保留父入口八W32业务零写：普通firstImage/secondImage及三组两项数组，入口一次、跨target保留；本步不重新补零。selfFlags仍原未指定局部，不新增默认/旧栈模型。角色fields288为当前四W16数组，原+288/+28A/+28C/+28E业务零/置一顺序保留；carrier只已证两native角色引用/gearImage0C/末8B零写，未写flags08不补值，不宣完整owner寿命。

<a id="battle-m-skill404-family-sampling-decision"></a>
## 工程裁决与21组23处修复

依既有工程委托先SPEC后落码。单根21组正常差异、23补丁定义/23替换处；预读缓存声明/取样/首EffectList属于一组，分别三定义，其余各一组。只表达原取样与宽度，不新增接口/owner、类型、依赖、保护或异常模型。

| 补丁 | 改前差异 | 原端及修复 |
|---|---|---|
| 01 | 初caster pub后重读位置 | GetPos返回同parsed位置pub/actor |
| 02..04 | 预读255短路不读caster、首EffectList重新读位置 | 每GetPos后先live caster位置，再255/相等比较；退出后actor沿该次缓存位置 |
| 05 | 第一坐标先X/两次索引，caster取样在所有偏移后 | 一次S32索引、Y点→X点；0..4/10..14读caster后Y/X偏移，5..9/15..19偏移后读caster，再actor/EffectList |
| 06 | 第二坐标先X/两次索引及统一caster时点 | 一次S32索引；按下表分别在点读取前、点后偏移前或偏移后取caster，Y→X/偏移，actor/EffectList后写固定casterPayload 100C |
| 07 | effectCount先caster位置/初Effect实参分别重读 | live caster位置→固定casterPayload effectCountCA0→actor，记录同caster位置/1/0/0，effectIndex固定供后继写 |
| 08..10 | target比较在pub后/判空和self识别重读global | GetPos返回target先255比较再pub，同parsed位置判空/与live caster比较、firstPosition赋同parsed值 |
| 11 | rider field1D0先读源值 | live rider指针→first固定源字段→写该rider |
| 12..13 | rider offset先rider再caster位置 | 每轴分别caster位置→live rider→source actor；原X double/float算术和Y字段复制保持 |
| 14..15 | 两组索引共四非地址变量拓成size_t | 每组imageIndex/index恢复S32，首正项/默认0和两项界限保持 |
| 16 | 普通Effect隐含参数顺序 | firstImage→amount→live flags→live target，传固定second |
| 17 | 普通Aux隐含参数顺序 | 所选firstImages值→secondImage→firstImage→live flags→live target，传固定second |
| 18 | 34之后current先target，256/52重新读global | target先current比较，256/52同位置，52之后51另live |
| 19 | 两最终位置分别在pub前后取样 | firstPosition→secondPosition→caster pub→preview快照→value pub |
| 20 | 尾坐标两次索引/X先Y | finalPreview选择原S32索引一次，Y→X，固定second EffectList |
| 21 | selfFlags40门禁使用W32取样 | 实际栈槽W8，显式字节读取；后200000门禁仍W32 |
| 22 | 自身Effect/Aux实参取样隐含 | firstImage→selfFlags→secondPosition→second Effect；另selfFlags→secondPosition→second Aux，原零参数保持 |
| 23 | second flagsFDC bit2门禁使用W32读取 | 原431C52实际W8内存读取，显式字节取样 |

第二坐标的八分支取样合同（位置分段仍原有符号比较，不加>=0等保护）：

| 位置段 | 406（singlePosition） | 404 |
|---|---|---|
| <5、10..14 | 索引→caster位置→Y点→X点→Y/X偏移→actor | 同顺序，偏移及100C按404值 |
| 5..9、15..19 | 索引→Y点→X点→Y/X偏移→caster位置→actor | 索引→Y点→X点→caster位置→Y/X偏移→actor |

第一坐标各分支原Y点先X点；0..4/10..14（含原<5负值分支）caster在两点后、偏移前，其余在两偏移后。原406点选择preview+5或preview、X偏移±121/Y±110；404点4/9/13/18、X±155/Y±35。第二坐标X±55，406Y±50/404Y±35；100C在EffectList之后写固定casterPayload，406同pointPosition，404为3/8/13/18。尾406选择preview或preview-5（5536E8是5536FC前五个S32槽别名），404选择3/8/14/19；只一次索引、Y/X，无新增坐标限制/浮点假等价。

正确生产、消费和动作链保持：

- 首Take弃，第二Take/GetPos caster，初空Finish直接返回；固定casterPayload skill/level及singlePosition=skill406，previewOffset沿scene64，不提交预读到scene游标。预读容量29两次字符串及全部整数/循环消费保持；首EffectList→25/74/40/151→第一坐标→207→第二坐标/100C→230/41。
- 两个438290子角色按原先后创建，各stateFE0、四W16业务零、FC4/FC8、CA4/CA8、scale/位置/phase、flags bit2与callback置值保持；再404AE0创建carrier，固定casterPayload effect1060发布，两角色引用/gearImage0C/末8B零写，未写flags08不解释/补值。227后记录effectIndex/初Effect，再进入目标循环；所有地址仅已证native拓宽，不新生命周期策略。
- self目标：firstPosition/secondPosition/selfFlags、first name/image/两firstImages、field1D0/四CryptInt正常构造-复制-销毁和flagsFDC→W16能力字段/优先级W8-坐标/album，再rider名称、字段/四CryptInt和offset复制；second名称/image/字段/四CryptInt与flagsFDC-W16、优先级/坐标/album；选择carrier gearImage、写原effectIndex的position3/4、两固定payload的skill/level，继续。内存CryptInt已有边界保持，不扩大为网络加解密。
- 普通目标：flags/amount，800000才Aux取包，second25/232→Effect/Aux→target0；原cached W32的AL/AH视图与34/256或52-51、40/123-122-121或233/40、11/76/46反馈及每次live flags重读保持，非地址索引宽度恢复不补数组/局部默认。
- 255/循环空Finish均进入真实尾；先恢复caster/value、尾坐标/second207；self W8 bit40选择234/52/51或原231/36/0与first/second动作链，W32 self200000才Effect/Aux和235/74；second flagsFDC W8 bit2才47/46；另live caster Move→RID→58固定1。初空直接返，不消费此尾。selfFlags仍未指定局部，不模拟历史栈或添加默认成功。

其它全部根/属性/声明/真实分派保持。RPLAN.24不新补零/拒绝/钳制/回绕、越界溢出或故障模型，不宣异常/编译后指令形态等价。PC53/x87与所有borrowed owner重进退出失效仍完整门禁待验，原RNG消费不扩展；网络原格式明文/音频暂停。

<a id="battle-m-skill404-family-sampling-acceptance"></a>
## 可度量验收

| 局部门禁 | 本步证明 |
|---|---|
| I | 单根21组23定义23处；整CPP从HEAD精确重放，其它全部正文/属性、关联类型/分派保持 |
| S | 1286条4898B十七页147call连续实际字节/全部出口，23补丁原EA、406三节点18B及404既有表定位 |
| 生产/消费 | 预读缓存-两段分支坐标/固定100C、carrier初Effect与rider采样、普通Effect/Aux-四S32索引-位置比较、最终恢复/尾Y-X和两实际W8门禁 |
| 保持 | 全部原取包/转换-初始化/字段/八CryptInt临时和四rider复制-动作/初空/循环出口与Move-RID/固定58=1；未指定局部/flags08不补值 |
| 登记 | 旧812仅五引用/一symbol/本步对象，双域四局部I/S，七open与唯一进度同步 |
| 运行与整体 | 不构建/测试/运行/摘要；完整父SEH/全部consumer-接线-owner失效/SDK-ABI-PC53-时钟RNG画面及R1/R7/R8继续 |

E475→479仅YOBI/BATTLE四局部I/S；语义1350/有效ID1345、片段5/分类456、YOBI55/BATTLE52/RENDER26/LOOP32、关联1463/3680、60源码closed10/完整验收0/P01closed保持。不新F/X/adapted、分类或完整成员，不以本根/1286条、子角色生产/静态汇总代完整父/功能/整体通过。

<a id="battle-m-skill404-family-sampling-source-state"></a>
## 源码、进度与剩余

源码 `src/YobiRuntime.cpp#ProcessBattleMSkill404Family`，旧812有界追加，快照 `cg2d-re-battle-m-skill404-family-sampling-20261006`。唯一[进度](../plan/cg2d_full_reconstruction_progress.md)与[看板](../plan/cg2d_measurement_dashboard.md)同步实际事实/剩余，不例行刷新总SPEC/索引或压历史。本步21组23处差异当步修复，继续其它技能真实消费者/全路由、完整父与全部生命周期及适用运行门禁。
