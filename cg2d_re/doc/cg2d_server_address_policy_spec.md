# 服务器地址校验并发兼容方案（SDP01）

日期：2026-10-24。用户已批准WinINet DoH与系统DNS并行、任一路通过即放行、全部失败才提示、不直接退出；随后要求同时接入Cloudflare与阿里DoH。本合同记录该已授权范围及实现取舍。

## 验证点与时机（2026-10-25修订）

- 按用户最新要求，DoH只扩展策略查询来源，不改变原有验证点或延后验证时机。每次连接仍由BeginConnect调用PrepareServerAddressPolicy，再在CgTransport::Impl::RunPolicy阶段完成策略查询与裁决；保留原查询名、TAOMOLI映射/免查询分支、首个A记录取值及固定摘要比较规则。
- 保留PolicyReady/ContinueAfterPolicy握手：查询结果就绪后，由主线程CgConnection::Impl::PollConnection在原ApplyServerAddressPolicyResult校验点消费当前会话结果；通过后才允许工作线程继续真实服务器地址处理、解析及TCP连接。移除进程退出行为不构成迁移或撤去该校验点的授权。
- 任一路结果就绪即提交原校验点判定；任一路通过即放行并取消其余请求，不为等待其余线路或10秒截止时间延后已有结果的验证。单路未通过仍等待其他线路，全部未通过或截止时仍无通过才按既有失败流程处理。
- 校验必须保持在真实服务器地址处理、解析及TCP连接之前，不得移到连接成功、登录请求发送或登录响应处理时，也不得先连接再补验。

## 查询与裁决

- 每次连接保留PrepareServerAddressPolicy产生的查询名及既有TAOMOLI映射/免查询分支，不改游戏协议与真实服务器连接解析。
- 同时启动三个异步请求：系统DnsQueryEx的A查询，以及WinINet分别向`https://cloudflare-dns.com/dns-query`、`https://dns.alidns.com/dns-query`发起的标准DNS二进制POST。每家仅请求一次，不做后台轮询或自动重试。
- 任一路的有效地址通过原有固定摘要规则，即继续连接并取消其他请求。单路解析失败、空结果、Fake-IP或校验不符不影响尚未完成的线路。以查询名/CNAME链的Answer首个A记录为该路校验值，不接受附加区的无关A记录，也不因存在多个地址扩大为任意地址命中规则。
- 协调等待使用单次10秒截止时间，WinINet连接/发送/接收也设置10秒超时。截止时未完成者记超时；三路全部未通过才返回Policy失败。初始化资源错误可能提前结束无法启动的单路，其他线路继续。
- 空结果不再沿旧逻辑放行，固定摘要比较保持；取消/过期会话没有失败弹窗。普通DNS仍可单独放行，这是用户明确选定的兼容性策略，不声称增强授权安全性。

## HTTPS及DNS响应

- WinINet采用用户现有代理配置，使用HTTPS并保留默认主机名/证书验证，不增加忽略证书标志或交互式证书窗口。不发送Cookie/自动认证，不自动跟随重定向，不把校验响应写入WinINet缓存。
- DoH使用`application/dns-message`，要求HTTP 200及匹配的内容类型。响应最大65535字节；检查DNS响应位、opcode、截断位、状态码、事务ID、单个Question的名称/类型/A类，再交Windows DNS记录解析器处理。
- Question比较保留标签边界、允许压缩指针并限制循环/越界；CNAME跟踪限制为记录数量以内。系统DNS和DoH的地址结果均在原校验点复用同一纯校验函数，CNG失败不产生通过结果。

## 并发与生命周期

- CgTransport现有工作线程负责启动、等待和发布查询结果，主线程继续按快照轮询并在原校验点裁决。无需为各路增加常驻线程；系统回调只写本请求完成状态、唤醒事件，禁止访问传输对象、UI或会话状态。
- DNS请求及结果结构由共享上下文持有；DnsCancelQuery后由完成回调释放其保留引用。WinINet请求正文、分段读缓冲区、事件及上下文一直保留到INTERNET_STATUS_HANDLE_CLOSING最后通知。取消不等待其他线路成功返回，也不复用未完成请求的缓冲区。
- 三路统一RAII取消；成功、取消、超时、异常和正常失败均关闭WinINet句柄并取消仍在等待的DNS。关闭回调只释放自身资源，不会覆盖新请求结果。
- 继续使用ConnectionId及传输锁保护发布；主线程消费结果及工作线程继续连接前均检查当前会话。保留PolicyReady/ContinueAfterPolicy握手及ApplyServerAddressPolicyResult原校验点，仅将其进程退出行为改为既有连接失败处理。

## 失败提示与实现边界

- 失败内部发布ConnectionPhase::Policy，通过现有登录错误流程统一显示普通连接失败，返回既有连接失败值，不调用_Exit/ExitProcess。按用户最新要求，不向界面暴露校验内容或原因。
- Policy失败分支在详细连接日志之前处理，不输出分路结果、错误码或策略标识；移除诊断字符串的构造及跨层传递，其他网络错误的既有日志与提示保持。
- 新增平台私有ServerAddressPolicy模块，Win32句柄不进入业务公开接口；CMake仅登记本模块并链接系统自带wininet/dnsapi，不引入外部依赖。
- 本次按项目限制仅执行静态源码、SDK声明、引用和差异检查，未编译、运行客户端、执行测试、查询实际校验域名或计算任何文件摘要。三路竞速、代理/TUN、证书失败、超时、取消和连续重连仍待运行验收。

## 修订与实现状态（2026-10-25）

- 用户GO后已恢复PolicyReady/ContinueAfterPolicy及CgConnection::Impl::PollConnection中的ApplyServerAddressPolicyResult校验点。后台只提取并发布各路首个IPv4结果，传输锁内最多暂存三条；主线程逐条裁决，任一路通过即唤醒工作线程并取消其余请求，未通过只移出当前结果。
- 三路查询继续共用10秒截止时间；查询结束时，已发布但尚未经主线程裁决的结果保留原握手等待，避免抢先判为失败。全部已就绪结果均未通过且其他查询结束后才发布Policy失败；取消、关闭或新ConnectionId使旧结果失效，真实服务器地址处理仍在RunPolicy通过之后。
- 按用户补充要求恢复原主线程校验说明、结果字段注释及固定校验值旁的原注释；涉及旧空结果放行和进程退出的说明按已批准行为更新。本轮静态检查通过，未编译、测试或运行客户端，三路竞速及重连等实际行为仍待运行验收。

## 接口依据

- Microsoft Learn：DnsQueryEx、DnsCancelQuery、DnsExtractRecordsFromMessage_W、WinINet异步操作与InternetCloseHandle；本地Windows SDK 10.0.26100.0的WinDNS.h、windnsdef.h及WinInet.h核对声明/结构。
- Cloudflare官方DNS wireformat说明：POST路径`/dns-query`、请求/响应`application/dns-message`。
- 阿里云公共DNS官方加密DNS说明：DoH地址`https://dns.alidns.com/dns-query`，RFC 8484标准报文。
