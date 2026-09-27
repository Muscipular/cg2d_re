# 动画索引运行时证据

## 1. 表 owner 与边界

2026-09-27，本线程只读现有 `ida-17524/cg2d.exe`
核 `GraphicAlloc@45750F–4575F9`、
`AnimeInit@45BE3C–45BED1` 与三个查询叶。
原 W32 槽 `gAnimate@9A4A58`、
`gAnimateCount@9A4A60`、
`gMAX_ANIMATE_COUNT@9A4A4C` 映像初值均为零；
`GraphicAlloc@457514` 将最大数写为 20500，
`457546–457551` 分配 738000 字节并发布表指针，
`4575E5–4575F9` 才按 `20500×0x24` 清零。
分配失败与其他图形分配的顺序仍应在
`GraphicAlloc` 本体落位时按全部指令处理，
不能把本次查询叶当成初始化。

`AnimeInit@45BE3C–45BEB6` 将 ESI 项、每项步长
`0x24`，逐条写入同一表；`45BEC6` 把 ESI 原样
发布为 `gAnimateCount`，`45BECC` 再以
`qsort(base=gAnimate, count=ESI, size=0x24,
compare=45B420)` 排序。该 W32 是**元素数**，
不是末项索引。新端 `AnimationRuntime` 暂只
持同源指针和两个 S32 槽，记录中除首 W32
外保留 0x20 原字节；`GraphicAlloc/AnimeInit`
生产及记录其他字段仍未接入。

## 2. 三个查询叶

`45B420@[45B420,45B439)` 是 qsort 比较器。
`45B428/42A/42C` 先取两记录首 W32，按无符号
比较返回 `-1/0/1`；相等返回零，左小于右返回
-1，左大于右返回 1。新端不以有符号减法
冒充该比较，保留相等元素的比较值。

`Animate::BinarySearch@[45B390,45B3F7)` 的入参
依次为无符号 W32 图号和有符号 S32 `min/max`。
起始 `min>max` 返回 -1；`max-min>=10` 时取
`min+(max-min)/2` 的整数中点，按原表步长
`0x24` 读取首 W32，与图号作无符号比较；
相等立即返回中点，大于图号将 `max=mid-1`，
小于图号将 `min=mid+1`，交错后返回 -1。
跨度小于 10 时从 `min` 到 `max` **包含上界**
逐项顺扫；首个相等即返回下标，否则 -1。
新端不重新排序、去重或改成半开区间。

`Animate::getIndex@[45B400,45B41D)` 只读取
`gAnimateCount`，传 `(图号,0,count)` 给上叶，
负结果转成 -1，其余原样返回。直接 caller
包括 `Yobi::FrameMove@517974`、
`Yobi::PlayFrame@517F7E` 与
`CoordLoad@45BFE8`；三个调用链均未因
本叶静态落位而闭合。

## 3. 满容量一过尾

原 `AnimeInit` 只排序前 `count` 项，
而 `getIndex` 将 **count** 当包含上界：
`count<20500` 时，`GraphicAlloc` 清零过的
尾项仍位于已分配区间，不能静默改作
`count-1`；`count==20500` 时，原指令可能
读出所分配的 `20500×0x24` 之外一项，
其原 x86 内容没有确定的业务值。

新端的 x64 backing 在后续 `GraphicAlloc`
实现中须为索引 20500 额外预留一项零哨兵，
而对真实 `count` 项的清零、填充及 qsort
范围保持原样。这是消除宿主越界的确定性
兼容裁决，不声称原端满容量一过尾值为零；
只有受该未定义值影响的满容量分支不纳入
逐次等价。当前 producer 未落位，指针仍为空，
不得在分配前调用查询或把静态库编译当作
`FrameMove`、SDL 画面、完整链接/运行验收。
