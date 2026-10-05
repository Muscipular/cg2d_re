# Winsock导入映射与连接共享状态重读 SPEC（NWI01）

2026-10-05，依据[网络兼容SPEC](cg2d_network_compatibility_spec.md)、
[总SPEC](../plan/cg2d_full_reconstruction_plan.md)及R1/R7纳管要求。
固定快照：`cg2d-re-winsock-import-binding-source-20261005`。

<a id="winsock-import-baseline"></a>
## 原端基线与当前差异

[机器证据](cg2d_winsock_import_binding_evidence.json)保当前原会话的15个WSOCK32序号导入槽、
15个六字节单jmp跳板、全部41个跳板直接入向。
其中26个call来自范围内六父；15个call/jmp来自已排除newest五函数，
共享API不随newest排除，也不恢复该链。htons槽另有一处PE导入描述数据引用，
不将其当业务call。

连接入口508360完整339条/1180B已归档。
原共享socket在508429/5084A9/5084D7/508521/50860F/508644/50866E独立重读；
当前源码在这些位置复用创建或fd_set填充前的socket快照。
原508748在512800返回后先读取连接阶段，再写请求状态1及递增；
当前源码使用入口阶段。两类取值时点必须修正，不能以当前callee通常不改共享值为由省略。

| 导入ID | 原跳板 / API | 序号 | 范围内 / newest入向 |
|---|---|---:|---:|
| I:0014D370 | 00534A36 / closesocket | 3 | 7 / 3 |
| I:0014D354 | 00534A60 / connect | 4 | 1 / 1 |
| I:0014D348 | 00534A72 / htons | 9 | 1 / 1 |
| I:0014D34C | 00534A6C / inet_addr | 10 | 1 / 1 |
| I:0014D378 | 00534A7E / ioctlsocket | 12 | 1 / 0 |
| I:0014D36C | 00534A3C / recv | 16 | 1 / 1 |
| I:0014D364 | 00534A48 / select | 18 | 2 / 1 |
| I:0014D374 | 00534A30 / send | 19 | 1 / 1 |
| I:0014D37C | 00534A78 / setsockopt | 21 | 1 / 0 |
| I:0014D380 | 00534A84 / socket | 23 | 1 / 1 |
| I:0014D350 | 00534A66 / gethostbyname | 52 | 1 / 1 |
| I:0014D358 | 00534A5A / WSAGetLastError | 111 | 3 / 1 |
| I:0014D360 | 00534A4E / WSAStartup | 115 | 1 / 1 |
| I:0014D35C | 00534A54 / WSACleanup | 116 | 1 / 1 |
| I:0014D368 | 00534A42 / __WSAFDIsSet | 151 | 3 / 1 |

<a id="winsock-import-decision"></a>
## 开发与纳管裁决

1. 仅LoginState.cpp中Sub_508360的七个socket使用点改为当时读取共享原生句柄，
   phase71的512800返回后先step=gConnectionStep，再写gRequestStatus=1。
   创建socket的返回仍用于紧随其后的ioctlsocket；
   fd_set的三个数组仍复用原5085B4的同次快照，不将已提交集合随新socket重建。
2. 保15个现有API及26个范围内调用，不新增API或旧跳板C++空包装。
   原WSOCK32模块/序号/IAT和原跳板身份完整保留；
   当前原生调用由既有ws2_32配置/Windows SDK提供。
   15跳板按host登记，15导入槽按mapped登记，只表示接口/提供者已定位。
   该分类不豁免外部ABI、完整父、状态/错误、数据及生命周期验收。
3. SDK10.0.26100.0已有WinSock2.h明确SOCKET=UINT_PTR、
   fd_set为u_int计数加SOCKET数组；当前共享句柄为uintptr_t，只在SDK边界转SOCKET。
   端口/htons为U16，IPv4/非阻塞值为原W32，长度/flags/错误为S32；
   hostent只借用并复制首地址4B，sockaddr_in沿既有原16B传参。
   使用实际SDK结构/声明，不能把原x86 fd_set、WSADATA或hostent布局塞进x64 SDK。
   FD_ISSET宏映射真实__WSAFDIsSet，不引入不同检查算法。
4. 原启动请求1.1、部分收发、would-block过滤、NoDelay/失败返回、日志/提示/关闭次序、
   连接阶段及600000超时、原资源/队列/状态和全部格式字节保持。
   不增加句柄有效性保护、SO_ERROR检查、重试或新的收包规则，
   不把select返回/setsockopt失败另作业务裁决。
5. 原格式明文及两算法叶排除保持；newest的15个API引用只记录排除依据。
   本组不关闭未知alias/间接目标、全部owner/跨页生产与R1/R7，
   不改变CMake、构建配置、旧源码或暂停音频。

<a id="winsock-import-acceptance"></a>
## 可度量验收

| 项 | 标准 |
|---|---|
| I | 一个CPP八处限定修改，七次socket重读及一次phase重读取值时点对应 |
| W | 15槽→15真实jmp跳板→41入向→26实际API调用/15newest排除引用，可逐call追踪 |
| S | 339条/1180B连接体与15条/90B跳板归档，SDK15声明/宏/句柄类型及既有链接配置可追 |
| 未验 | 全部父与状态生产/生命周期、外部ABI/SDK链接、正常失败/真实互通及R1/R7/运行 |

16条既有语义（15跳板和连接父）定点更新，其余1191条及政策保持；
15新host处置使159→174/3680，导入槽已审0→15/218；
语义1207/1204有效原ID、18pending/4分类引用不变，限定证据新增三项。
P09/P10源码工作项原状态保持；U10/V03与R01/R02继续open，
任务10/60、叶0/182及完整门禁不提升。
仅静态，不构建/测试/运行/摘要；SDK声明与源码对应不代表实际ABI或网络运行通过。

<a id="winsock-import-source-closure"></a>
## 源码小步收口

Sub_508360七处在原调用后重读共享socket，phase71发送后先重读连接阶段，
再发布请求状态1及递增。创建返回用于ioctl、三个fd集合同次快照保持。
单CPP八替换外全文保持，其余源码、调用数量、字段宽度、格式/队列与协议字节不改。

15槽和15单jmp/90B跳板、全部41入向归档，26范围内call逐项对应现有API；
15newest引用保排除，htons的PE数据引用不算call。
实际SDK十五声明、FD_ISSET宏及原生句柄/结构、既有ws2_32文本配置可追。
三门禁仅归档本组静态证据，不推SDK ABI/链接、完整父或运行通过。

16条旧语义定点/1191其他与政策保持；15新host使159→174/3680，
15导入mapped使0→15/218，限定E117→120；语义1207/1204有效ID、18pending/4分类保持。
U10/V03/R01/R02继续open，P09/P10原源码工作项状态及任务10/60、叶0/182不提升。
其余203导入/3061区间、未知间接目标/alias、全owner/真实互通与R1/R7仍待，
未构建/链接/测试/运行/摘要；原格式明文与音频暂停保持。

**NWI01交付检查**：12项严格UTF-8无BOM/LF、787个本地Markdown链接、42项限定证据/决议引用及Git差异格式通过；单CPP八替换、16记录/登记簿限定重放、26实际API点对应及measure生成/独立-Check通过。任务10/60、叶0/182与完整门禁不提升。
