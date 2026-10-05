# NAS01 适配器标识取样与局部清零 SPEC

<a id="adapter-sampling-baseline"></a>
## 原端基线

仅核 `534EF0..535085` 的155条/405字节、`54AB16..54AB1C` 的一条/6字节API跳板及其既有接线；[逐指令证据](cg2d_network_adapter_sampling_evidence.json)来自已打开cg2d会话的只读Python。原ID与库存分母不变。

- 两次GetAdaptersAddresses：Family=2、Flags=0、Reserved=null；首次输出为空、size预清W32，只有返回111才分配并再次调用；第二次非零或找不到合格项才释放并返回-1。成功路径原不释放，保持现有行为。
- 首次API后两个局部槽相距16字节；534F12/16、1A/1E、22/26交替清各槽前三个W32，534F2D/31各清偏移12的一字节。每槽实际清13字节，不能据16字节步长把尾三字节也清零，亦不能把已证清零长度说成原声明容量。
- 每项先判断OperStatus=1，再在534F77读取一次IfType；仅6或71跳到相应格式支路。两个支路各以5、4、3、2、1、0顺序读取PhysicalAddress的六个U8，零扩展后按0..5顺序交同一534FEF格式调用。
- 原58F734的23字节含NUL串保持为`%02x%02x%02x%02x%02x%02x`。先核以太文本长度12、再核无线文本长度12；匹配时复制含NUL的13字节，正常返回0，不增加PhysicalAddressLength门槛。
- 54AB16只跳54D388导入槽；两直接caller为534F09/534F56。槽值不是实现正文或运行提供者证明。原适配器根唯一直接caller5086B4弃返回，既有LoginState中后续复制/请求保持，本步不验其完整父。

<a id="adapter-sampling-decision"></a>
## 工程裁决与实现

依[本线工程委托](../AGENTS.md)，在既有LegacyNetworkAdapter.cpp修正取样和清零，不新增包装、缓冲提供者或协议格式。

1. 保留两个16字节本机局部承载；撤全数组值初始化，以三个交替W32复制清零和两次偏移12的U8清零表达原序，仅清13字节。尾三字节无新读写，不建立异常补零或安全门槛。
2. Up项保存一次IfType，用同一值决定资格和格式目标；没有合格项时先走原失败返回，不消费尚未赋值的类型局部。
3. 物理地址借只读volatile BYTE视图，分六条语句按5→0读取，每个结果显式U32；原格式仍以0→5参数序消费。此视图仅约束本组U8取样，不把SDK对象或跨线程状态改成全局volatile，不增加线程同步。
4. 沿现有SDK原生结构字段及GetAdaptersAddresses调用承载x64地址；保留原W32数值和API参数，不复制旧x86偏移或构造旧跳板。导入与跳板按既有真实宿主提供者登记。

<a id="adapter-sampling-acceptance"></a>
## 本步验收边界

必要静态对照：原155条连续覆盖/405字节、八清零原点及局部槽距离；一条6字节跳板与两call/导入关系；类型单取值、两支路各六U8原序、23字节格式和现有失败/成功释放路径；精确源码增量、语义/处置/导入/任务/派生度量及链接编码。

不构建、测试、运行客户端或API/算法样例，不计算摘要。源码及限定静态证据分别归档I/S，完整FUNC-NETWORK、U10/R01/R02/R04/V03仍open；实际SDK返回、生成ABI/加载/链接、完整父/owner/未知alias/原格式明文互通及运行不在本步已验范围。音频暂停、两加解密叶排除和RPLAN.24保持。

<a id="adapter-sampling-source-state"></a>
## 当前交付

本步修正[既有适配器根](../src/platform/LegacyNetworkAdapter.cpp)，其[声明](../include/cg/platform/LegacyNetworkAdapter.hpp)及[真实父接线](../src/LoginState.cpp)保持。534EF0登记adapted，54AB16登记host，I:0014D388登记mapped；更新原534EF0同一语义记录，并补原API跳板语义，保留原格式点与分母，不新增格式调用计数。

固定命名快照：`cg2d-re-network-adapter-sampling-20261005`。仅本组有限静态通过可计，不据此关闭[总SPEC §10门禁](../plan/cg2d_full_reconstruction_plan.md#10-完成门禁)或整体目标。
