# 服务器地址校验并发兼容方案（SDP01）

日期：2026-10-24。用户已批准WinINet DoH与系统DNS并行、任一路通过即放行、全部失败才提示、不直接退出；随后要求同时接入Cloudflare与阿里DoH。本合同记录该已授权范围及实现取舍。

## 验证点与时机（2026-10-25修订）

- 用户最新明确：PrepareServerAddressPolicy内就要完成验证，包含DNS/DoH请求。该函数在本次调用内完成地址映射、查询名构造、三路请求、等待和固定校验后返回bool；通过后才提交真实服务器连接，不能仅返回查询名而把请求或验证延后。
- 以用户指定Git版本的固定校验值、原注释及VM标记为对照。PrepareServerAddressPolicy主体、三路查询协调函数及函数内地址校验回调均直接使用VM_HARD_START/VM_HARD_END成对保护，正常成功和失败统一在结束标记后返回；保留TAOMOLI映射/免查询、首个A记录和固定比较规则。
- 任一路结果就绪即在PrepareServerAddressPolicy本次调用线程内判定；任一路通过即返回成功并取消其余请求，不为等待其余线路或10秒截止时间延后验证。单路未通过仍等待其他线路，全部未通过或截止时仍无通过才返回失败。
- 本条覆盖旧NTM01及本SPEC此前关于后台RunPolicy、PolicyReady/ContinueAfterPolicy和主线程PollConnection裁决的约定；这些接线移除。当前调用线程会在函数内等待策略查询，保留单次10秒截止，不把验证推迟到实际服务器地址处理、解析、TCP连接或登录收发之后。

## 查询与裁决

- 每次连接保留PrepareServerAddressPolicy产生的查询名及既有TAOMOLI映射/免查询分支，不改游戏协议与真实服务器连接解析。
- 同时启动三个异步请求：系统DnsQueryEx的A查询，以及WinINet分别向`https://cloudflare-dns.com/dns-query`、`https://dns.alidns.com/dns-query`发起的标准DNS二进制POST。每家仅请求一次，不做后台轮询或自动重试。
- 任一路的有效地址通过原有固定摘要规则，即继续连接并取消其他请求。单路解析失败、空结果、Fake-IP或校验不符不影响尚未完成的线路。以查询名/CNAME链的Answer首个A记录为该路校验值，不接受附加区的无关A记录，也不因存在多个地址扩大为任意地址命中规则。
- 协调等待使用单次10秒截止时间，WinINet连接/发送/接收也设置10秒超时。截止时未完成者记超时；三路全部未通过才返回Policy失败。初始化资源错误可能提前结束无法启动的单路，其他线路继续。
- 空结果不再沿旧逻辑放行，固定摘要比较保持。普通DNS仍可单独放行，这是用户明确选定的兼容性策略，不声称增强授权安全性。

## HTTPS及DNS响应

- WinINet采用用户现有代理配置，使用HTTPS并保留默认主机名/证书验证，不增加忽略证书标志或交互式证书窗口。不发送Cookie/自动认证，不自动跟随重定向，不把校验响应写入WinINet缓存。
- DoH使用`application/dns-message`，要求HTTP 200及匹配的内容类型。响应最大65535字节；检查DNS响应位、opcode、截断位、状态码、事务ID、单个Question的名称/类型/A类，再交Windows DNS记录解析器处理。
- Question比较保留标签边界、允许压缩指针并限制循环/越界；CNAME跟踪限制为记录数量以内。系统DNS和DoH的地址结果均在原校验点复用同一纯校验函数，CNG失败不产生通过结果。

## 并发与生命周期

- PrepareServerAddressPolicy的调用线程负责启动、等待和裁决，内部沿用三路系统异步请求，无需增加常驻线程。系统回调只写本请求完成状态、唤醒事件，禁止访问传输对象、UI或会话状态；结果解析与固定校验均在本次Prepare调用结束前完成。
- DNS请求及结果结构由共享上下文持有；DnsCancelQuery后由完成回调释放其保留引用。WinINet请求正文、分段读缓冲区、事件及上下文一直保留到INTERNET_STATUS_HANDLE_CLOSING最后通知。取消不等待其他线路成功返回，也不复用未完成请求的缓冲区。
- 三路统一RAII取消；成功、超时、异常和全部失败均关闭WinINet句柄并取消仍在等待的DNS。关闭回调只释放自身资源，不会覆盖后续请求结果。
- 策略函数返回后才登记新ConnectionId；传输层只消费已完成的bool结果，通过时登记真实连接任务，失败时发布Policy失败快照，不再执行策略请求或验证。真实连接及取消继续受ConnectionId和传输锁保护。

## 失败提示与实现边界

- 失败内部发布ConnectionPhase::Policy，通过现有登录错误流程统一显示普通连接失败，返回既有连接失败值，不调用_Exit/ExitProcess。按用户最新要求，不向界面暴露校验内容或原因。
- Policy失败分支在详细连接日志之前处理，不输出分路结果、错误码或策略标识；移除诊断字符串的构造及跨层传递，其他网络错误的既有日志与提示保持。
- 新增平台私有ServerAddressPolicy模块，Win32句柄不进入业务公开接口；CMake仅登记本模块并链接系统自带wininet/dnsapi，不引入外部依赖。
- 本次按项目限制仅执行静态源码、SDK声明、引用和差异检查，未编译、运行客户端、执行测试、查询实际校验域名或计算任何文件摘要。三路竞速、代理/TUN、证书失败、超时、取消和连续重连仍待运行验收。

## 修订与实现状态（2026-10-25）

- 按用户再次明确的位置要求，撤回此前将PollConnection当作验证点的错误接线。PrepareServerAddressPolicy改为返回bool，在内部直接发起并等待三路DNS/DoH查询，并用本函数内的回调完成固定校验；不再返回查询名或把结果交给后续轮询。
- 移除CgTransport的RunPolicy、PolicyReady、ContinueAfterPolicy及策略地址暂存；传输层只按已经完成的验证结果启动连接或发布普通连接失败。保留三路单次10秒截止、固定校验值和原注释，Prepare主体、请求协调与固定校验均加入明确的VM_HARD成对标记。
- 本轮调用时序、固定值/原注释、VM成对标记及出口、回调寿命和UTF-8/Git差异静态检查通过；未编译、测试、运行客户端、实际查询策略域名或重新加壳，策略调用等待、三路竞速及受保护代码的实际行为仍待对应运行验收。

## 接口依据

- Microsoft Learn：DnsQueryEx、DnsCancelQuery、DnsExtractRecordsFromMessage_W、WinINet异步操作与InternetCloseHandle；本地Windows SDK 10.0.26100.0的WinDNS.h、windnsdef.h及WinInet.h核对声明/结构。
- Cloudflare官方DNS wireformat说明：POST路径`/dns-query`、请求/响应`application/dns-message`。
- 阿里云公共DNS官方加密DNS说明：DoH地址`https://dns.alidns.com/dns-query`，RFC 8484标准报文。
