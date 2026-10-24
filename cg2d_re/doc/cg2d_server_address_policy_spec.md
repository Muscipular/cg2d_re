# 服务器地址校验并发兼容方案（SDP01）

日期：2026-10-24；2026-10-25修订。用户已批准系统DNS、Cloudflare和阿里WinINet DoH并行；最新明确调用方不等待返回，任一路通过即可，全部失败时退出进程。本修订覆盖此前同步等待及普通连接失败提示的约定。

## 验证点与时机（2026-10-25修订）

- PrepareServerAddressPolicy仍是唯一发起入口，在函数内完成地址映射、构造查询名并启动三路后台任务后立即返回void；不以返回true冒充校验已经通过。按用户最新许可，真实服务器连接不等待策略结果，查询与校验在后台继续完成，不迁至PollConnection或传输工作线程。
- 以用户指定Git版本的固定校验值、原注释及VM标记为对照。Prepare主体、后台请求/裁决及函数内定义的固定校验回调均保留直接成对的VM_HARD_START/VM_HARD_END；保留TAOMOLI映射/免查询、首个A记录和固定比较规则。
- 每路地址就绪即在该路后台线程中调用固定校验；任一路通过即记录通过并唤醒其余线程取消查询。单路未通过只记本路完成，三路全部结束且没有通过才退出进程，不能在单路不符时提前退出。
- 本条覆盖旧同步等待、RunPolicy、PolicyReady/ContinueAfterPolicy及PollConnection裁决约定；已存在的界面、连接、登录和协议流程不承接策略结果。策略退出仅按本次独立查询批次裁决，不引入后台重试或轮询。

## 查询与裁决

- 每次连接保留PrepareServerAddressPolicy产生的查询名及既有TAOMOLI映射/免查询分支，不改游戏协议与真实服务器连接解析。
- 通过三个分离的std::thread并行执行系统DnsQueryEx的A查询及两家WinINet标准DNS二进制POST，DoH端点仍为`https://cloudflare-dns.com/dns-query`、`https://dns.alidns.com/dns-query`。每家仅请求一次，不做后台轮询或自动重试。
- 任一路的有效地址通过原有固定摘要规则，即继续连接并取消其他请求。单路解析失败、空结果、Fake-IP或校验不符不影响尚未完成的线路。以查询名/CNAME链的Answer首个A记录为该路校验值，不接受附加区的无关A记录，也不因存在多个地址扩大为任意地址命中规则。
- 三路共用单次10秒截止时间，WinINet连接/发送/接收也设置10秒超时。截止时未完成者记超时；线程创建、初始化资源或查询失败只结束对应单路，其他线路继续。批次必要资源无法建立时三路均无法启动，按全部失败退出。
- 空结果不再沿旧逻辑放行，固定摘要比较保持。普通DNS仍可单独放行，这是用户明确选定的兼容性策略，不声称增强授权安全性。

## HTTPS及DNS响应

- WinINet采用用户现有代理配置，使用HTTPS并保留默认主机名/证书验证，不增加忽略证书标志或交互式证书窗口。不发送Cookie/自动认证，不自动跟随重定向，不把校验响应写入WinINet缓存。
- DoH使用`application/dns-message`，要求HTTP 200及匹配的内容类型。响应最大65535字节；检查DNS响应位、opcode、截断位、状态码、事务ID、单个Question的名称/类型/A类，再交Windows DNS记录解析器处理。
- Question比较保留标签边界、允许压缩指针并限制循环/越界；CNAME跟踪限制为记录数量以内。系统DNS和DoH的地址结果均在原校验点复用同一纯校验函数，CNG失败不产生通过结果。

## 并发与生命周期

- 采用std::thread(...).detach()启动三路任务，不保留需要调用方等待的future，也不进行join。每条线程独占本路查询的启动、解析和取消，系统回调只写本请求完成状态/唤醒事件，禁止访问传输对象、UI或会话状态。
- 批次共享上下文按值持有查询名及校验回调，线程持有shared_ptr；禁止借用Prepare栈、连接对象或UI数据。上下文独立持有Winsock引用、取消事件、共享截止时间、通过标志和三路剩余数量，最后线程退出后释放，避免调用返回或传输关闭导致悬空。
- 通过标志先发布再递减剩余数量；最后完成者仅在没有通过时调用std::_Exit(std::rand())。其余线程通过共享事件响应成功取消，并各自在本路关闭句柄，不跨线程同时修改查询状态。
- DNS请求及结果结构由共享上下文持有；DnsCancelQuery后由完成回调释放其保留引用。WinINet请求正文、分段读缓冲区、事件及上下文一直保留到INTERNET_STATUS_HANDLE_CLOSING最后通知。取消不等待其他线路成功返回，也不复用未完成请求的缓冲区。
- 各路线程在成功取消、超时、异常或正常结束时关闭本路WinINet句柄并取消仍在等待的DNS；查询对象及系统回调继续共享持有请求缓冲。关闭回调只释放自身资源，不会覆盖后续请求结果。
- 传输层不再接收policyApproved或登记Policy失败快照，只执行真实服务器连接；ConnectionId及传输锁继续保护既有连接状态和取消，不参与独立校验批次裁决。

## 失败提示与实现边界

- 按用户最新确认，全部失败直接调用std::_Exit(std::rand())；移除单路校验不符立即退出及旧Policy失败提示接线，不向界面或系统日志输出校验内容、分路结果、错误码或策略标识。其他网络错误的既有日志与提示保持。
- 新增平台私有ServerAddressPolicy模块，Win32句柄不进入业务公开接口；CMake仅登记本模块并链接系统自带wininet/dnsapi，不引入外部依赖。
- 本次按项目限制仅执行静态源码、SDK声明、引用和差异检查，未编译、运行客户端、执行测试、查询实际校验域名或计算任何文件摘要。三路竞速、代理/TUN、证书失败、超时、取消和连续重连仍待运行验收。

## 修订与实现状态（2026-10-25）

- 已实现三路std::thread分离任务；Prepare改为void发起入口，线程按值持有批次及静态DoH端点，调用方不等待查询结果。按用户授权将原单路立即退出移到三路聚合裁决，移除传输层bool策略门禁与旧Policy失败提示。
- 线程捕获/资源持有、通过标志与剩余计数先后、单次10秒截止和取消唤醒静态核对通过；固定值、原注释、原地址规则、请求/响应解析及系统回调实现与Git基线一致，十份变更文件UTF-8及Git差异检查通过。
- 用户随后明确要求build.bat并提交，本轮已执行build.bat Debug，配置、编译和链接成功，退出码0，产物为build/windows-msvc-x64/Debug/cg2d_re.exe。构建包含三路后台线程及Prepare/传输接线，仍有C4458、C4244、C4701告警；未测试、运行客户端、实际查询策略域名或重新加壳，后台竞速、取消、超时及受保护代码的实际行为仍须对应运行验收。

## 接口依据

- Microsoft Learn：DnsQueryEx、DnsCancelQuery、DnsExtractRecordsFromMessage_W、WinINet异步操作与InternetCloseHandle；本地Windows SDK 10.0.26100.0的WinDNS.h、windnsdef.h及WinInet.h核对声明/结构。
- Cloudflare官方DNS wireformat说明：POST路径`/dns-query`、请求/响应`application/dns-message`。
- 阿里云公共DNS官方加密DNS说明：DoH地址`https://dns.alidns.com/dns-query`，RFC 8484标准报文。
- Microsoft Learn：thread::detach及<future>析构等待约定；本次无需返回异步结果，选择分离线程，避免局部std::async future析构等待。
