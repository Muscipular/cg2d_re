# NQA01 原格式明文队列正常长度算术

<a id="network-queue-arithmetic-baseline"></a>
## 实际缺口与原基线

原格式明文两帧父与InitializeNetworkRuntime_508140/NetworkLoop_507DF0接线本来已有；[旧接线SPEC](cg2d_network_exclusion_wiring_spec.md)及[帧合同](cg2d_network_plaintext_frame_spec.md)已记录，不能把历史“待接”当今天缺代码。当前缺口是发送父两处及三个队列叶的U32加减/bit_cast回绕承载，与RPLAN.24冲突。

[限定原证据](cg2d_network_queue_arithmetic_evidence.json)共80条/257B：发送容量窗403E7D..403E99为5条/28B，长度发布窗403FCE..403FE0为6条/18B；接收入队5081E0..50822B为27条/75B，接收消耗508230..508274、发送消耗508280..5082C4各21条/68B。只核这两发送窗和三个完整叶，不重新验发送父全部146条。

容量窗len+size+1后原JG；接收入队previous+size后原JLE，正常不超过0x4000才复制，再发布D69790。两消耗入口先同源D9F2C9门禁，S32 size>=0x4000返-1，否则读旧长、减size、先发布剩余W32再移动、返0。正常路径保留0/-1/-100；没有新增size负值或超过当前长度拒绝。

原发送队列长度地址D9F2BC、接收长度D69790已由[既有唯一owner](cg2d_network_buffer_lifecycle_spec.md)承载。三个叶各只有一直接call：507F13入队、403D97接收消耗、5080FA发送消耗；发送父由50816E地址参数绑定。记录引用不重验完整Loop/帧父或全部命令。

<a id="network-queue-arithmetic-decision"></a>
## 工程裁决与实现边界

按[工程委托](../AGENTS.md)及[RPLAN.24](cg2d_closure_decisions_spec.md)，在既有NetworkRuntime.cpp恢复五处正常S32算术：发送容量和发布长度、接收入队next、两消耗remaining。U32 size/frameLength在这些S32运算边界作显式S32转换，接口、复制数量和其他原字段宽度保持；本步只承诺正常长度域，不外推任意W32位型。

保留原两个signed容量门禁的差异：入队大于0x4000拒绝，消耗大于等于0x4000拒绝。原先复制后发布的入队、先发布后移动的消耗及发送DWORD复制→发布len→尾byte顺序保持；Send/Receive memcpy/memmove的既有承接不重裁。

W16包头/字节序/实际明文长度、原首标记与RNG/窗口报告、正文/旧尾、独立后读尾长和小/大包状态保留。plaintextLength/stateLength的U32原字段表达、位掩码/逻辑移位、报告截止W32按S32比较保留；不能把原协议字段/位型运算一并删掉。两算法叶排除，原格式明文、不加解密继续。

不加溢出/越界保护、拒绝、钳制、补零、输入限制、异常重同步或回绕模拟，不把异常域称修复/等价/安全。未改变原帧结构、业务正文或对端协议。

<a id="network-queue-arithmetic-acceptance"></a>
## 本步验收

必要限定静态：80条/257B五段的连续覆盖和实际signed分支/原长度读写时点，四身份及四入向边；源码五块精确增量、接口/唯一owner/复制数量及其他源码不变。更新三已有语义并补接收入队当前身份，新增三个adapted处置、发送父原adapted追加依据，登记本组限定I/S、相关open任务并生成看板/独立Check。

本步不构建、测试、运行客户端/算法样例或计算摘要。实际生成ABI、真实半包/连续包/部分发送、全部命令/上游/间接alias/共享寿命、SDK和明文互通/V03仍未验，完整FUNC-NETWORK及R1/R7/R8不据此关闭；音频暂停及现行执行限制保持。

<a id="network-queue-arithmetic-source-state"></a>
## 当前交付

[五处源码](../src/network/NetworkRuntime.cpp)已恢复正常S32长度算术；[声明](../include/cg/network/NetworkRuntime.hpp)和私有发送签名不改。三个队列叶新增当前adapted处置，接收入队补显式语义；发送父及两消耗原身份保持，不把既有全体说成新实现。

当前网络总合同首段“帧父待接”改为两帧父本体/绑定已接、完整功能和互通未验，历史日期记录保持。FUNC-NETWORK已知成员44→47仍未全闭，U10/R01/R04/V03继续open，P09/P10源码工作项原关闭状态保持。

固定命名快照：`cg2d-re-network-queue-arithmetic-20261005`。
