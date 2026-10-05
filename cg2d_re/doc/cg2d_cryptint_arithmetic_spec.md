# CryptInt 普通算术对齐 SPEC（CQA01）

2026-10-05，处理四个既有入口残留的专项回绕模拟，按现行RPLAN.24实施。
上位：[总SPEC](../plan/cg2d_full_reconstruction_plan.md)、[原CryptInt合同](cg2d_legacy_cryptint.md)。
固定快照：`cg2d-re-cryptint-arithmetic-source-20261005`。

<a id="cryptint-arithmetic-baseline"></a>
## 原事实与当前缺口

[只读原证据](cg2d_cryptint_arithmetic_evidence.json)保存四个完整根15条/55B、114个直接调用引用，
以及401EA0单条5B尾跳；不把引用数当114个完整父已验收。

| 原入口 | 完整范围/指令 | 读取与算术 |
|---|---|---|
| 401E10 | 401E10..401E1C，3条/12B | 先401EC0读取一次，再ADD参数，返回W32 |
| 401E60 | 401E60..401E6C，3条/12B | 先401EC0读取一次，再SUB参数，返回W32 |
| 401E90 | 401E90..401E9D，3条/13B | 先401EC0读取一次，再IMUL参数，返回低W32 |
| 401FE0 | 401FE0..401FF2，6条/18B | 401EA0→401EC0读取右对象一次，左整数减右值，返回W32 |

原机器位型保留为事实；现源码用U32运算/bit_cast额外模拟溢出回绕，与现行异常策略冲突。
Increment_401DE0、Difference_401E70已经采用S32算术，本步保持。

<a id="cryptint-arithmetic-decision"></a>
## 工程裁决与实施

四个源码算术片段改为S32的Read+amount、Read-amount、Read*amount及lhs-Read(rhs)。
读取仍只执行一次；原读取的W8计数、门槛刷新、共享RNG和重编码副作用保持。
AddAssign/SubtractAssign继续先调用对应算术入口，再同一次SetNorVal，不改两包装或调用方。
类型、返回及非地址字段仍为原宽，LegacyCryptInt*只是已拓宽的非拥有地址；16B载荷布局不动。

ToBits/FromBits继续供实际encoded^mask与存储位型边界使用，不能因撤数学回绕删除异或算法。
所有构析、payload复制、比较、读取、随机源和父业务正文保持，网络仅明文的范围不变。
数学溢出/非法地址等异常不增加回绕模拟、保护、拒绝、钳制或补零，
不承诺该域的确定结果/原故障等价或x64安全；正常宽度、读取次数和调用顺序继续验收。

<a id="cryptint-arithmetic-acceptance"></a>
## 限定静态验收

检查一CPP四个精确片段、公开头对应说明、四根及唯一GetVal尾跳、记录与新增引用。
四个既有原身份补语义台账与adapted处置，归入FUNC-CRT；不把既有函数误计为新业务实现。
本组15条/55B及114引用只证明本组基线与入向，不重验全部父、owner/alias、启动或运行。
不构建、不新增或执行测试、不运行算法样例或客户端、不计算摘要，音频暂停保持。
R01/U11、完整CRT/实际ABI/全部活动消费与运行仍未闭，完整门禁和关闭数不提升。

<a id="cryptint-arithmetic-source-state"></a>
## 本组源码状态

四个普通算术入口已撤专设U32/bit_cast回绕，读取一次及既有赋值包装/编码边界保持。
原CryptInt文档早期“回绕加法/避免有符号溢出”等陈述在这四入口上的当前实施由本合同更新；
其他原指令/位型证据与历史验证记录保留，不外推异常等价。
