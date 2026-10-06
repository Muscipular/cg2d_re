# BPA01 按匹配值绘框的记录与坐标生产

<a id="matching-hover-record-baseline"></a>
## 缺口与原依据

507090已有C++正文和[早期文本合同](cg2d_sub_507140.md#graphic-hover-507090-implementation)，但独立机器映射/功能成员为空。当前源码在读取y/x后才通过索引取得DX条目，四边算式留在调用表达式，并按索引重定位两个平行记录；原端先保存实际DX条目，按四边计算顺序生产，沿两指针推进。本步补具体阶段差异及原身份，不重验已过的命中父、排序或描边包装。

[完整原证据](cg2d_matching_hover_record_sampling_evidence.json)覆盖真实F:00107090@[507090,50713F)59条175B、一页1call/5内跳/1ret，唯一普通调用点5076BD/父507140。选定父窗口[5076B1,5076CA)8条25B另列：重新取value4、有符号>=3才绘框，随后无条件排序；不把这8条计入本根59条，不以窗口代完整799条父验收。

<a id="matching-hover-record-decision"></a>
## 工程裁决与最小实施范围

依既有委托，只改GraphicPendingQueue.cpp该完整正文，不新增helper、私有状态、公开API或原入口，不改头/布局/实际父/绘框/排序/provider及owner。

1. 入口读唯一count为S16，非正直接返回；根接任意S32匹配参数，不复制父的>=3门槛。正count后先初始化平行层槽游标，再主记录游标；每轮先取实际record.value4，再取参数比较。
2. 匹配时先U8层、S32image；沿<0xD4分支选择CE/DA，之后采样同一DX基址和完整W32 hover，再保存实际DX条目native地址，然后依次采样S32 y/x、S16高度/宽度。保持同一下标的平行层，不经已排序cacheIndex回查，不改第三种绘框层级。
3. 正常S32四边按bottom=y+height+2、top=y-2、right=width+x+2、left=x-2显式生产；只在既有绘框实参处转U16，七参(left,top,right,bottom,layer,hover,0)。不以C++调用表达式的未固定求值次序表示原四边生产，不新增溢出/回绕模拟。
4. 绘框或不匹配后先fresh S16 count，再++S32 index、++native主记录、++native层记录，用同一计数样本判断回边；绘框追加同一队列，新项参与后续遍历。不能缓存入口长度、首次命中退出、重新排序或新增count写。
5. native队列48B/DX条目32B/平行层4B沿现行x64合同，仅指针拓宽，W32/W16/U8保持；各指针非拥有，只保存真实条目地址，不复制表/record、调用尺寸getter/加载器或新建索引句柄表。

当前无介入时旧正常坐标/颜色和匹配结果一致，修正取样/计算/游标阶段，不称像素错误。已有GraphicHitTest.cpp定义并生产gValueD69768，父源码已调用本根再排序；早期“未定义/父缺失”是历史，不新增固定0或影子槽。此处仅核当前接线，不重验完整父或提升其运行门禁。

<a id="matching-hover-record-acceptance"></a>
## 度量、定向验收与未闭范围

静态核59条175B连续覆盖/真实原ID、唯一引用和实际父窗口；整CPP仅一个完整正文差异，原field采样、DX地址取得、四边算式、七实参与动态count—两游标推进对应。原绘框/父/头/provider与其它正文保持，不运行算法样例或异常输入。

新增机器1413对应已有原根：语义1413→1414/有效ID1408→1409、文档关联1526→1527（41.47%→41.49%）、分类533→534、RENDER45→46枚举未闭；两独立根内局部static I/S E673→675，冻定位1/旧行号保。RESOURCE41/AUDIO32暂停、pending18、队列175/需核46/暂停25、R7区间67/3061及全部完整门禁保持；60源码closed10/完整验收0、accepted_leaf_features=0/global_accepted=false不变。

U08/U09/G01/G02/R01/R04/V04只同步进展和引用，仍open；不关闭整个命中/描边/渲染或C09。完整父/所有callee、正常动态别名/队列-表与纹理-Back-owner、场景/失效/退出、SDK/机器ABI和全模式像素运行仍待；局部静态不代运行或整体R1/R7/R8。

<a id="matching-hover-record-source-state"></a>
## 当前交付

交付单正文实修、完整原证据、独立SPEC/机器映射/成员与分类、两局部static I/S及唯一进度/七open任务/看板。固定命名快照：`cg2d-re-matching-hover-record-sampling-20261006`；13文件精确提交，用户既有六项排除。

仅必要静态与measure/-Check，未构建、测试、运行或计算摘要；不新增拒绝/补零/保护/钳制、回绕或故障等价门禁。后续沿实际匹配生产—平行队列—绘框追加—排序—case2消费者/Back与dirty重建寿命推进；网络原格式只明文、内存CryptInt保持、音频暂停，整体SPEC目标继续。
