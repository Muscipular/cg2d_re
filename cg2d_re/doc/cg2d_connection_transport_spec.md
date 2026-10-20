# CgConnection 与 CgTransport 对象封装 SPEC

日期：2026-10-20。编号：CTO01。状态：待确认，未修改运行代码。

目标：在已实施的[NTM01](cg2d_network_modernization_spec.md)上，将连接生命周期与字节传输分别封装为对象，为后续迁移协议函数、登录流程和Lua入口提供稳定边界。本轮保持单游戏连接和既有协议行为。

## 1. 当前边界与调整

目前NetworkTransport已经拥有I/O线程、Socket、DNS和有界队列；当前会话标识位于gConnectionSocket，主线程协议状态位于NetworkRuntime.cpp的静态NetworkRuntimeState，连接开始、查询、关闭和Pump仍由自由函数协调。

仅重命名传输类不能解决这些状态分散问题。本方案将NetworkTransport改名为CgTransport，并由CgConnection拥有传输对象、当前会话和主线程协议状态；保留旧函数作为迁移适配入口。

| 对象 | 命名空间 | 职责 |
|---|---|---|
| CgTransport | cg::platform | 字节传输：Winsock、异步DNS、IPv4/IPv6候选、事件等待、工作线程、16KiB收发队列、短写、背压及取消 |
| CgConnection | cg::network | 当前会话生命周期、地址策略协调、主线程协议工作区、LF/压缩接收状态、收发Hook、Pump及终止结果消费 |
| 原登录/世界/Lua模块 | 保持现有命名空间 | 登录步骤、错误提示、场景切换、业务处理和Lua执行环境；调用连接对象或旧适配入口 |

CgConnection以组合方式独占一个CgTransport，不继承传输类。两个对象均禁止复制和移动，使用PImpl隐藏实现；公开接口不暴露SOCKET、HANDLE、SDK地址结构、内部可变状态或队列内存。

## 2. 状态所有权

| 当前状态 | 封装后的唯一所有者 |
|---|---|
| NetworkTransport::Impl中的原生资源、后台状态与字节队列 | CgTransport::Impl |
| 当前ConnectionId | CgConnection::Impl；0表示无会话 |
| NetworkRuntimeState中的协议工作区、原始接收区、压缩半帧/半行、计时、活动状态、协议字段与Hook注册表 | CgConnection::Impl；字段原义和容量保持 |
| gRequestPollingEnabled、登录步骤、请求计时与提示 | 继续由登录模块管理；适配层将收发门禁传给连接对象 |
| 断线场景状态 | 继续由世界/主循环适配层消费连接终止结果并更新 |
| Lua状态与业务对象 | 原模块持有，传输线程不得访问 |

进程仍通过现有NetworkRuntime适配层持有一个CgConnection，寿命覆盖网络初始化到退出。连接对象替代原gNetworkRuntimeState/gNetworkTransport两个静态拥有者，不增加另一份协议工作区或发送队列。

旧gConnectionSocket暂作为只读兼容镜像保留，由适配层在连接开始/关闭时统一同步；它不再决定连接对象的当前身份。新代码调用Id()，旧uintptr_t参数只表达调用者预期的会话身份，发送前仍需校验。禁止新增长期镜像或其他模块写入。

ConnectionId继续由传输对象单调分配，重连及Shutdown/Initialize不重置身份序列；本阶段不销毁并重建进程持有的连接对象来重连。

## 3. 建议接口

以下名称确定职责，具体参数沿当前返回值合同适配；不为未来需求预建抽象基类。

| CgConnection入口 | 语义 |
|---|---|
| Initialize / Shutdown | 初始化或释放传输和主线程工作区，失败回收，重复关闭安全 |
| BeginConnect / PollConnection / Close | 开始、查询或取消当前会话；协调原地址映射与策略结果裁决 |
| Id | 返回当前会话身份，用于回调前后及旧参数校验 |
| SetIoEnabled | 接收主线程发布的门禁，再同步给传输对象 |
| SendPacket | 接收已有发送入口构造的完整明文消息，保留回调、JFVf替换和旧返回合同；不会重复添加LF |
| Pump | 消费入口时刻的接收字节预算，沿原解析与分发流程处理，发布终止结果 |
| RegisterReceive / RegisterSend | 持有主线程Hook，注册顺序和重连保留语义不变 |
| StopIo | 关闭会话并停止、等待I/O线程，用于Lua/UI销毁之前；此时保留主线程协议工作区 |

CgTransport沿用当前Open、BeginConnect、PollConnection、ContinueAfterPolicy、SetIoEnabled、TryQueueSend、DrainReceived、CloseConnection和Stop能力。CgConnection私下使用这些接口，上层业务不取得传输对象。传输层不识别JFVf、XCNY、LF、LZ4或Lua；追加/替换由连接层选择，后端只执行受锁保护的字节操作。

PollConnection/Pump输出值类型结果，不通过后台回调修改登录或世界全局。地址策略仍在主线程裁决；策略DNS完成前不能进入游戏连接解析。

## 4. 旧接口迁移方式

1. 新增include/cg/network/CgConnection.hpp及对应实现；NetworkTransport头和实现改名为CgTransport，更新CMake及引用，不永久保留两套同义类。
2. 将原主线程状态移入连接对象。依赖该状态的解析、组包和生命周期辅助函数收敛为私有成员/内部实现；不通过公开State()、可变引用或全局宏暴露整个状态。
3. 保留BeginServerConnection、PollServerConnection、CloseServerConnection、StopNetworkIo、InitializeNetworkRuntime、NetworkLoop、ShutdownNetworkRuntime等旧入口，改为向唯一CgConnection转发和适配原业务状态。
4. 原协议命令函数及业务handler签名首轮保持。需要工作区的函数通过对象的对应内部操作取得结果，纯编码算法和无连接状态的业务处理继续保留原职责；不把全部业务函数机械迁为连接类成员。
5. 登录、Lua和退出调用链接入对象后的实际路径。后续可以逐个替换旧入口，最终移除gConnectionSocket镜像及旧适配函数；本轮不批量改写全部业务调用者。

验收要求是真实状态和调用链迁入对象；仅增加两个转发外壳、内部继续使用原全局owner不算完成。首次封装后业务仍依赖现有全局世界/Lua状态，不宣称已支持多个可同时运行的独立连接实例。

## 5. 生命周期与行为约束

- CgConnection公开业务操作仅在主线程调用；CgTransport内部I/O线程不取得CgConnection、Lua或业务对象的借用指针。
- 回调执行时不持传输锁；每次回调结束重新核对会话身份和活动状态，关闭或重连后不继续派发旧消息。
- Close只取消本会话并清理会话缓存，保留注册表；StopIo先关闭并等待线程退出；Shutdown再释放主线程工作区。析构为资源回收兜底，不在析构中调用业务或Lua回调。
- 保留双栈候选顺序、数字scope、地址策略、600000ms期限、30000/60000ms计时、16KiB队列、JFVf原子替换、EOF排空和两阶段退出。
- 保留当前GBK、LF、压缩格式、字典、字段及Hook顺序。本轮不引入依赖、RTTI、连接池、自动重连、并行业务handler或额外协议抽象层。

## 6. 实施与验收

按传输类迁移、连接状态归属、兼容入口接线、集中核对的顺序推进，每个可验证小步同步唯一进度并提交。

静态验收检查：旧类引用清理；Socket/DNS仍仅在平台边界；主线程状态只有一个owner；无公开可变状态逃逸；当前身份单一来源；所有入口到对象的实际调用路径完整；初始化失败、重连、回调重入与两阶段关闭覆盖。

实施后的编译/链接以及运行验收分别记录；本轮仅输出SPEC，不沿用上一任务的单次构建授权，不编译、运行或新增测试。NTM01尚未完成的IPv4/IPv6互通、短写、背压和取消竞态验收继续保留，不能由封装或历史构建替代。

## 7. 待确认

确认采用上述分层：CgConnection拥有当前会话和主线程协议状态，组合CgTransport；保留旧函数适配及只读身份镜像，暂不迁移全部业务函数或支持多连接。确认后实施本轮对象封装。
