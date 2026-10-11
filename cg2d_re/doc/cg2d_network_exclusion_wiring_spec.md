# 原格式明文网络接线总 SPEC

> 2026-10-09当前接线沿[本机服务端明文合同](cg2d_network_compatibility_spec.md#local-server-plaintext-contract)：InitializeNetworkRuntime绑定QueuePlaintextProtocolBytes，NetworkLoop调用ReceivePlaintextProtocolLine；原命令和原字段以LF收发。下文旧W16/403C50/403E50接线为历史实施记录，不能作为当前客户端接线。两算法叶排除不变，实际互通仍待验收。

> 2026-10-04最新有效要求：兼容原有格式、不自创或修改协议；不做加解密，只做明文传送。两算法叶继续排除，403E50/403C50非加密职责保留。
> 已决定按[原格式明文子SPEC](cg2d_network_compatibility_spec.md)实施，**两帧父源码及既有直接/回调接线已落位，完整功能与互通未验**。旧候选协议选择不再生效；历史全文见Git命名快照 `cg2d-re-spec-history-20261004-before-delegated-decisions`。

## 已确认范围

发送在原401960位置直接传原非加密组包得到的明文；接收在原401AB0位置直接取帧正文作为原解密结果对应输入。包头保持原W16读写/字节序/计数约定，只随实际明文字节登记长度；其余正文、命令/状态、队列、失败与处理顺序不自定义。

不实现401960/401AB0，不增加假加解密函数、不恢复密文模式。原加密专属填充/错误不伪装为仍执行；非加密状态与消费者不能一并删除。

## 真实接线与原证据

[NetworkRuntime.cpp](../src/network/NetworkRuntime.cpp)中InitializeNetworkRuntime_508140仍将Sub_403E50传给工作区；NetworkLoop_507DF0仍调用Sub_403C50，仅返回0才派发。两父当前已有完整非加密本体，详见[实施与静态收口](cg2d_network_plaintext_frame_spec.md#frame-source-closure)。516B60的空参数默认回调仍仅属于原缺省路径；508140明确绑定真实发送父，不以默认0代帧实现。

[原合同](cg2d_network_runtime.md)保留W16包长/包长+2等待、目的容量、CR/LF、NUL、队列消耗、593F*状态和发送长度/报告证据；[算法分类](cg2d_network_cipher_scope.json)保留两叶边界/授权及历史源码移除状态。客户端帧结构兼容与加解密算法逐次等价是不同事项，本轮不承诺密文模式对端可接明文。

## 实施与验收

依子SPEC的N1–N5成对实现发送/取包并清无用算法依赖；不新加长度限制、行分包、协议字段、探测/回退、异常重同步或私有影子owner。cg3a PacketBuilder/Utils只辅助应用文本理解，不决定或改写原传输层。

当前只做静态核对，不构建/测试/运行/试包；真实原格式明文往返与V03仍待。newest排除、音频暂停及其他任务边界不变。

## 三个发送侧状态入口的显式存储关联（2026-10-02）

| 原入口 | 原行为/唯一调用点 | 新端唯一存储与本轮对应动作 |
|---|---|---|
| 4589A0..4589A6 | 读W32[73ECF4]返回；403EB3 | MainLoopState的S32 gWindowReportValue |
| 4589B0..4589BB | 清W32[73ECF4]，无规定EAX；403ED4 | 同一槽，403E50已在原FC发送时点清零 |
| 4589C0..4589C6 | 读W32[73ED08]返回；403EC6 | 同文件U32 gWindowReportDeadline |

发送父原序：首读值signed>0→510140==1→读取deadline并将完整W32按signed判负→写局部标记FC→4589B0清零→直接跳走，不消费EAX。不能发明bool/固定成功，不能把U32 deadline的负值门禁写成普通无符号比较；也不是73ED00的double截止时间。

2026-10-12，[NWR01](cg2d_network_compatibility_spec.md#window-report-plaintext-source-state)已将三状态操作内联接回活动QueuePlaintextProtocolBytes：当前轮询／容量门禁后，保持首标记优先及报告三条件，再清唯一报告槽；不写FC／13等旧帧首字节。上述403E50位置是原端历史事实，不表示当前明文链调用旧帧；初始owner与真实发送期间清零分别证明，完整网络／运行未验。

这些是三项存储关联/共六指令，不是三个新包装；403E50本轮已按原三个时点直接读写同源槽，完整证据见帧子SPEC。装载零初值不代发送期间清零；报告未随算法排除而删除，完整报告生产/生命周期仍待。暂停音频及其他验收边界不变。
