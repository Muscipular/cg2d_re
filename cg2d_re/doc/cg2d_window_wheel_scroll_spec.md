# 滚轮滚动、坐标持样与像素余量 SPEC（WWS01）

2026-10-10。沿滚动位置写入/反求链核滚轮生产者，依子AGENTS第8条先记原事实再实施。同一cg2d_re分支，不打tag；当前文本边界、MSVC CRT及WAV-only保持。

<a id="window-wheel-baseline"></a>
## 完整原根与既有依赖

4743B0–474497（尾不含）完整89条231B/两页，四实际E8、两目标（4742C0两次、53B498/__ftol两次）、六跳两RET。29个实际CALL入向逐字节核目标，仅定位，不代所有父验收；其中4A81D0聊天滚轮父已由CFD01完整纳管，复用旧合同。

原54F640四BYTE为8988083C，即IEEE F32位模式3C088889。当前std::bit_cast<float>(uint32_t{0x3C088889})因子保持，不用十进制近似重写或运行数值样例。4742C0已由旧1533/BQO01完整覆盖，正确body与两高度取序保持，不重采/登记。53B498已有host处置，沿现C++数值转换与MSVC承接，不再引入旧CRT帮助体。

## 原入口和位置生产

先取得一次element.data地址并保存，之后才判wheel==0；该分支返回原position，即使不滚动也有原指针读取。wheel非零时maximum<0按原置0；保存前项y，第一次调用4742C0，再取前项y计算余量。

原F32因子乘wheel，FST落存F32样本、再转换未落存乘积取整，整数步加position。position负或大于maximum分别按原置0/maximum并清余量，两个原业务分支不合并。第二次调用4742C0后，读前项y加余量，写回并保存这个结果，跨小数像素计算和整数转换继续使用。

现有double乘积、F32 roundedProduct、两个S32转换及double小数乘除图保持；沿[已批CRT/业务数学边界](cg2d_crt_host_migration_spec.md)使用宿主数学，旧扩展精度/控制字/私有ST0不作为本步恢复目标，也不宣数值/机器运行已验。这里不改变数学类型、公式或舍入存储边界。

<a id="window-wheel-decision"></a>
## 三段源码对齐

第一段：第二次写入后保存remainderY并写前项；按当前height→前项height取span，原小数图得到step，再用保存的remainderY加step得到steppedY、写前项。原下界比较使用此保存值，随后读取当前y；命中用同一当前y写前项。现body重新读前项y累加和比较、以及高度减法顺序未明确，改为上述显式样本。

第二段：按当前height→当前y→前项height→前项y读取，比较两组保存值之和。超过时按原当前y-前项height+当前height写前项；不得分支内重新取字段。原边界调整保持。

第三段：先读前项y，再当前y，随后读取入口保存data的W32 flags；之后才相减得delta，flags置bit0，先写data+8 W32，再写data+0 W32，返回保存的position。现body提前计算差值，改为原字段/flags取样顺序。

三段修改及两条定义/真实声明注释；不新增helper、API、字段、类型、布局、owner或资源分配释放。原初始data/element/前项地址借用、wheel零读序、两次位置写入、最大值/position钳制和余量清零、flags/返回宽度及全部其它源码保持。

原最大值与像素上下界钳制必须保留，不补新的maximum==0、尺寸、null/index/容量保护，默认值、补零、回滚或异常/溢出/零除模型。现type1/type4字节载荷+0/+8访问与固定W32 memcpy保持，不强转异型对象。

<a id="window-wheel-acceptance"></a>
## 限定静态验收

核完整原根连续字节、两RET/六跳/四实际E8、29实际CALL和四原因子BYTE；核位置余量结果持样、两高度/上下界/尾部flags取样及W32发布序。两整源按三段和两注释精确重放，三个真实结构、原数值图、业务分支、setter及其它源保持。

新增一语义F/一adapted处置、INPUT/UIRUNTIME各一成员、两限定S零I；九任务open。旧setter/CRT/聊天父身份和所有其它台账、48有界代码/完整枚举和门禁/六closure/总SPEC§10保持，不把数值源表达式静态通过代宿主数值结果验收。

只做必要文本静态及measure_cg2d.ps1/-Check，不编译、测试、运行客户端或算法，不查看产物、计算摘要或引依赖。29父/完整间接输入、wheel及页码/最大值/坐标/载荷全部生产消费和alias-owner寿命、全部CRT/SDK/生成机器ABI/优化后读取、实际滚轮手感/像素画面与全生命周期待，整体未完成。

<a id="window-wheel-source-state"></a>
## 当前状态

本步前提交“[#] 补齐滚动结果与位置换算传递合同”；拟提交“[#] 补齐滚轮滚动并恢复坐标持样”。源码对齐、登记、静态汇总/-Check终态退出0及派生范围核对通过。语义2266=2218F关联+48有界代码记录，F处置1708/证据1508，INPUT85/UIRUNTIME123；文档关联2328/3680=63.26%仅关联率。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持；未构建、测试或运行，整体active/未完成。

<a id="evidence-archive-20261011"></a>
## 证据归档（2026-10-11）

原记录：`cg2d_window_wheel_scroll_evidence.json`，已提交原文由Git历史保存，最近记录提交为「[#] 补齐滚轮滚动并恢复坐标持样」。源码前后差异、完整指令/依赖/旧台账及重复文档副本不再在工作树另存；本SPEC前述依据、合同和剩余继续有效。

| 原根 | 末界（不含） | 指令/字节 |
|---|---|---|
| 0x4743B0 | 0x474497 | 89/231 |

原验证记录：`necessary_static`=pass（历史限定检查）；`registry`=pass（历史限定检查）；`measurement`=pass（历史限定检查）；`check`=pass（历史限定检查）；`derived_scope`=pass（历史限定检查）；`runtime_verified`=false；`generated_machine_reads_verified`=false；`build_or_test_performed`=false；`digests_computed_or_compared`=false。

本次仅整理文档和引用，不重新核对已通过且未变化的函数，不新增构建/运行或功能验收。完整业务、生命周期、ABI/SDK及运行门禁保持原状态。
