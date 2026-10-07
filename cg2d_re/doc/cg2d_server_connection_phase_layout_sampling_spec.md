# 服务器连接阶段与布局取样子SPEC（BSK01）

2026-10-07。沿选服后的真实连接控制器推进，按AGENTS第8条工程委托先记录裁决。仅修改LoginState.cpp既有UpdateServerConnection的三个片段，原事实来自已打开cg2d.exe的只读指令。

<a id="server-connection-phase-layout-baseline"></a>
## 原端基线与源码差异

原45D990范围[45D990,45DBEF)，单块165条607B／三页，20直接调用／17目标、无间接调用，29跳转／1RET；唯一父45FEA0@4601B5。原17目标含既定newest五函数／六调用点，它们继续排除，其余12个目标不能因本根取证而视为完整验收。旧映射191／F:0005D990已有两基线文档与一文案格式点，本步只补全来源，不增加重复身份。

| 区段 | 原指令数据流 | 当前源码差异 | 裁决 |
| --- | --- | --- | --- |
| 入口阶段0窗口几何 | 45D9B7测宽后得到ECX列数，45D9D0先发布初值；ECX<2时另写2，后续以该ECX计算x和width，45DA02／0C／11发布row／x／y；5044E0使用先前参数 | 判断和计算再次读取已发布columnCount，调用参数再读layout.x/y，width晚算 | 保初值／原最小2两次发布，以计算所得本地S32继续计算x／width，先算参数再发布row／x／y；以本地x、192、width创建 |
| 阶段2初始化失败或未进入阶段2 | 原45DB0B设置-1后进入45DB0E重新读取W16 phase；直接3／4／其它入口同走45DB0E | 保留入口phase缓存，失败后不刷新，后续3／4检查继续用旧值 | 失败分支和未进入阶段2的路径各恢复现场W16 phase读取，然后检查3／4 |
| 阶段2／3成功直达轮询 | 原阶段2成功写3、Reset508350、写4后直跳45DB30；阶段3同样重置、写4直达 | 当前强制本地phase=4已承接 | 保当前直达行为，不新增现场读取覆盖它 |
| 绘制尾部 | readiness成功后45DB99测宽；45DBA0取row，算row*48-16；45DBBC取y再算textY；45DBCB取column，算column*64-textWidth；45DBD8取x再算textX | y表达式先包含y字段，x表达式先包含x字段，与原分阶段字段取样不对应 | 各字段用明确S32局部变量，按row／y／column／x及原算式阶段读取；两个最终S16参数保持 |

不推定InitializeNetworkRuntime实际会改phase；修复的是已证45DB0E现场读取和后继分支合同。当前普通几何值原已正确，不声称运行故障。入口一次W16快照保；阶段0后进入2、阶段1直接进入2为既定newest旁路，不恢复排除链或读写其副阶段。

邻接真X:0005DBEF-0005DBF0仅1B 90，逐字节decode确认为NOP、无入向；前45DBEE RET、后45DBF0真入口。仅登记原对齐用途，不造新的F或范围排除。

<a id="server-connection-phase-layout-decision"></a>
## 实施裁决与保持项

单源三个精确from→to、只一个既有业务体变更，不新增工具／owner或接口。布局普通S32运算、原111偏移／64除法、原列数最小2、x／width和row2／y192／height96／参数4／1保持。原列数保护是既有门槛，不新加异常限制或回绕模型。W16 phase、S32 result、非拥有YobiAction观察者及最终S16绘制边界保持。

保Shutdown→PersistAndReset→Initialize顺序、成功3→Reset→4、两原Reset调用位置、一次每帧508360轮询、成功1／负值-2／失败-1，非零结果才MarkDeath／清观察者再LoadBlacklist；后续重新取action／readiness／测宽／文字提交与返回保持。原53B304固定文案、旧字节／唯一256B缓冲和所有其它源码保持，包括Sub_508360、日志、Reset与登录请求／进度体。已正确的连接发送后阶段重读不额外修改。

原格式网络只明文、不加解密；newest五函数、屏幕键盘和既定加解密叶排除保持。音频继续暂停。RPLAN.24不专项模拟异常，不设故障门禁，不新增保护／钳制／补零；x64只承接已有真指针，不拓非地址，不引外依赖或RTTI。

<a id="server-connection-phase-layout-acceptance"></a>
## 可度量验收

1. 三原片段在HEAD整源各唯一命中，由此构造整个expected，与现源逐字相等；其它函数、文案和未变依赖声明保持。
2. 原165条连续字节／三页／607B、20直接／17目标及29跳转／1RET、唯一父与初始化失败现场取样／成功直达、创建参数和row-y-column-x字段阶段逐项静态关联；1B邻接NOP仅原用途分类。
3. 旧191补关系／来源／局部元数据，原一格式点全保，不增语义记录或文档覆盖数。新增单adapted／LOGIN9→10，两局部I-S；语义1591／有效1584、文档1702（46.25%）保持，处置725→726、E840→842；R7 296→297／剩2764。
4. U10／V03／R04只current与引用，继续open。P05原closed、closed11／完整叶0／globalfalse、全完整门禁、冻结定位／无F代码区各7／pending18／差队列全部保持。源码／原证据／完整机器／文档集中静态，通过后measure与-Check同句柄至终态，再比完整派生快照和12文件精确集合、UTF-8无BOM-LF／有效引用／差异。

完整45FEA0父与范围内12依赖、实际连接与登录响应／人物链、全部生产消费／正常alias-owner和寿命、CRT-SDK／机器ABI、链接、原格式明文往返、显示与运行R8仍按原标准。原17目标含排除链的计数不作为恢复声明；本根原取证和局部静态不代完整链或运行门禁。不构建、测试、运行或计算文件摘要的限制继续有效。

<a id="server-connection-phase-layout-source-state"></a>
## 实施追踪

预定固定命名快照：cg2d-re-server-connection-phase-layout-sampling-20261007。对应[原证据](cg2d_server_connection_phase_layout_sampling_evidence.json)、[状态控制器](cg2d_login_state_controller.md)和[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。集中检查与派生终态通过后追加实际结果，不预记验收通过。

实际结果（2026-10-07）：单源三片段／一既有业务体已实施；原165条607B／三页与1B邻接NOP证据落地。源码／原证据／完整机器／文档集中静态、measure与-Check同句柄终态0、完整派生快照及12文件编码／引用／差异通过。旧191补全不增身份或原格式点，单adapted／LOGIN一成员／1B对齐用途和两局部I-S落实；语义1591有效1584／文档1702（46.25%）保持，处置726／E842／R7 297余2764。U10／V03／R04仍open，P05原closed及全部完整门禁保；链接、原格式明文往返、显示／运行与整体验收未达成。
