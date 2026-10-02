# cg2d 文件流基础入口承接合同

## 本步范围与工程裁决（2026-10-02）

按总PLAN的R1覆盖收口，核原fopen/fsopen、fgets、rewind、fputs、fclose六入口，
并对应现有Blacklist、Input与ClientEntry中的真实标准库调用。
保留现有宿主FILE*和原调用顺序，不复制x86 FILE布局，不新增句柄表或包装。
本步仅登记已证消费域及差异，不把所有原caller、文件子系统或运行验收一起通过。

## 原边界与行为

| 原入口 | 结束地址（不含） | 指令数 / 直接引用数 | 已证合同 |
|---|---|---|---|
| fclose 53B1D0 | 53B226 | 35 / 96 | 先flush、释放缓冲、close；关闭失败结果为-1，成功才释放非空临时文件名；最终清流flags |
| fgets 53B226 | 53B27D | 43 / 8 | 最多count-1字节，保留换行；读到部分内容后EOF仍补NUL并返回buffer |
| rewind 53B27D | 53B2D1 | 32 / 1 | flush、清流错误/EOF及底层EOF位、更新流方向，再lseek(fd,0,0) |
| fsopen 53B2D1 | 53B2F1 | 11 / 1 | 先getstream；空直接返回，否则按filename/mode/share/stream调用openfile |
| fopen 53B2F1 | 53B304 | 6 / 67 | 固定share=40h调用fsopen，返回流指针或null |
| fputs 53B356 | 53B399 | 30 / 2 | strlen后设置临时缓冲，fwrite(source,1,length)，恢复缓冲；写满返回0，否则-1 |

fgets的count<=0立即返回null，不写目标；count=1写buffer[0]=0后返回buffer，
不读流数据。若首次取字节即EOF，直接返回null且不写buffer，包括不写首字节。
这一事实不解决402020/4021A0的首次读取失败后未写局部问题，待批补值保持。

fclose的字符串流标志40h分支返回-1并清flags；非活动流也返回-1。
不据此推断所有宿主FILE内部字段、errno或资源释放时点等价。
rewind唯一401081 caller随即取流EOF位，不消费原lseek残留EAX；
新端void rewind保持该返回消费合同。

fputs的两处原调用401236/401241都在黑名单持久化4011A0，先写记录再写换行，
返回均丢弃。既有父本体即使fopen失败也继续执行原后续，不添加成功兜底、
失败门禁或把父AL=1解读成落盘成功。

## 当前真实来源

- src/Blacklist.cpp：LoadBlacklistFile及PersistBlacklistFile_4011A0使用原rb+/w模式，
  按原位置调用fopen、rewind、fgets、fputs、fclose。
- src/Input.cpp：Sub_449DA0、Sub_449FA0、Sub_44A090、Sub_44B050的打开/关闭消费；
  本步不变更既有短写、异常路径或日志流共享owner。
- src/platform/ClientEntry.cpp：WinMain的argument.ini读取使用fopen/fgets/fclose，
  行缓冲的既有原位清零不推广给其他fgets调用。
- fsopen在原二进制只有fopen一个直接来源；新端未直接调用_fsopen，
  而是由宿主fopen的common_fsopen承接该私有路径。

上述是明确关联的消费者，不是96个关闭、67个打开或8个读行caller全量验收。
音频中的文件调用不借此恢复；旧日志待决仍保持。

## 宿主源码与差异

已读本机UCRT 10.0.26100.0的stdio/fopen.cpp、fgets.cpp、rewind.cpp、
fputs.cpp、fclose.cpp，不下载依赖或执行文件操作。

宿主fopen使用_SH_DENYNO调用common_fsopen；_s接口改用_SH_SECURE，
因此不把当前调用改成fopen_s。路径和mode原字节直接进入宿主窄接口，
不在本步转换编码或改模式。

宿主fgets保留count=1、换行、部分EOF和立即EOF不写buffer的规则；
额外参数校验、流锁、ANSI模式验证及无效参数处理与原不完全相同。
不能因标准库实现先验证stream，就说原count<=0路径也读取stream。

宿主fputs正常写满实际返回0，短写返回EOF；fclose正常先flush/freebuffer/close，
但宿主流分配、锁、校验、free_stream和errno传播不同。
宿主rewind显式重置缓冲，lseek返回-1时设置IOERROR；原rewind本体在lseek后
直接返回，没有同一失败后判断。原flush内部及低层文件函数另行归属，不外推等价。

不创建另一套旧CRT流状态，也不宣称宿主校验、异常通知、故障时点或所有文件失败
与原端一致。源码/CMake保持，只做新增映射与合同静态核对。
当前未构建、测试、读写实际游戏文件、运行客户端或进行哈希校验。

## 读写、定位与刷新组（2026-10-02）

本组沿现有标准库调用追加七入口，合计431条；保持FILE*、模式、原字节和调用顺序，
不新增读写重试、短读回滚、自动清零或把用户未决的日志缓冲问题自动解决。

| 原入口 | 结束地址（不含） | 指令数 / 直接引用数 | 已证行为 |
|---|---|---|---|
| fflush 53BB02 | 53BB3D | 29 / 5 | null调用flsall(0)；非空先flush，失败-1，成功且commit标志置位才commit，归一化0/-1 |
| fseek 53BC41 | 53BCCD | 54 / 25 | 校验流/whence，清EOF，SEEK_CUR用ftell换成绝对偏移，flush后lseek，成功0/失败-1 |
| fread 53BCCD | 53BDB5 | 92 / 55 | W32乘积0即返0；缓冲复制/整块read/单字节填充，返回已完整读入元素数 |
| fwrite 53C680 | 53C78A | 106 / 29 | W32乘积0即返0；缓冲复制/整块write/单字节写入，短写设错误并返回完整元素数 |
| fgetc 53C8B3 | 53C8CD | 12 / 39 | 先减剩余量；缓冲有字节则返回U8并推进指针，否则filbuf |
| ftell 53CB78 | 53CCD0 | 123 / 2 | 先将负缓冲剩余量清0，查询底层当前位置，按缓冲/文本换行及读写模式修正 |
| fputc 53CFDC | 53D000 | 15 / 6 | 先减剩余量；有空间写低W8并返回U8，否则flsbuf |

fread/fwrite缓冲复制实际调用541620，不能因名称为_memcpy_0就并到前轮
网络重叠复制53D710。底层read/write/filbuf/flsbuf/flush及541620的全部内部职责
不由这七项自动验收。无源数据或目标数据“整块先快照”的新增处理。

### 具体消费者

- Input的Sub_44A090一次读取大小4844h、数量1：返回0仍保留已读部分字节，
  关闭后仅以count==1返回成功；打开失败才按既有顺序尝试创建，不补清历史区。
- Sub_449FA0按原记录偏移定位，先写121h记录，再定位4840h写4B索引；
  两次fseek返回被丢弃，fwrite仅判断完整元素数量，写失败仍按现有原路径关闭。
  Sub_449DA0的日志fflush返回被丢弃，不新增磁盘提交成功判定。
- GraphicInit_459960先fseek(0,SEEK_END)，再ftell，将返回按U32累计；
  仍不增加-1失败门禁。原ftell另一个caller是fseek本体，不将这项当普通文件大小API。
- Palette的InitPalette32/ReadPalette/UpdatePalette_450BB0逐次fgetc后取低U8，
  包括EOF低字节FF；不把失败改为停止调色板读取或补0。
- SceneState::Sub_4687B0四次fputc依次写M/A/P/1，返回丢弃；其余地图头fgetc
  保持各自原消费。音频读取和其他资源父链不据本组恢复或一并验收。

### 宿主边界

本机UCRT对应stdio/fread.cpp、fwrite.cpp、fseek.cpp、ftell.cpp、fgetc.cpp、
fputc.cpp、fflush.cpp的相关实现已核。正常非溢出数量下，读写均返回完整元素数，
失败之前产生的部分字节不撤销；宿主size_t乘积校验、缓冲尺寸、锁、无效参数和errno
与原W32私有实现不同。RPLAN.24不要求专项模拟乘积溢出，也不授权新增业务保护。

宿主fseek公开long参数在Windows目标仍为32位，内部提升为64位；具有二进制只读
缓冲内定位快路，不总是调用底层定位。常规路径仍忽略flush返回后继续定位，
但内部缓冲重置、64位偏移及校验差异不得外推为原全部文件操作等价。
宿主ftell从64位结果转换long，超过LONG_MAX返回-1；另有UTF文本分支，
当前Graphic的rb读取不改成UTF模式，其他文本流定位仍另行验收。

宿主fflush(null)会遍历宿主流表，不是原静态流表；本步只关联当前日志消费，
不声称全量刷新、退出表或异常时资源状态等价。fgetc/fputc的正常U8/EOF结果可对应，
宿主ANSI流检查和锁仍是差异。原fseek/ftell写1085FE0的错误状态也不能冒称
已由宿主errno自动写入本线gLegacyErrno；本组消费者未以该槽读取这些调用结果。

源码/CMake保持；只补映射与上述边界，未运行文件读写、构建、测试或哈希校验。

## 私有流与缓冲依赖组 SPEC（2026-10-02）

按工程委托，继续由现有宿主FILE及对应stdio实现持有流表、文件描述符与缓冲，
不在应用层重建旧32B FILE、旧全局表、缓存指针或第二份计数。
以下九入口均已查全部直接入向，来源均在CRT范围；本步通过既有真实fopen/
fclose/fread/fwrite/fgetc/fputc/fputs/fflush消费路径关联，不伪装为应用直接定义。

| 原入口 | 结束地址（不含） | 指令数 / 直接引用数 | 原职责 |
|---|---|---|---|
| flush 53BB3D | 53BB99 | 40 / 9 | 写流且有大缓冲才写累计数据；短写置错误，公共尾总会ptr=base、count=0 |
| freebuf 53F38F | 53F3BA | 17 / 2 | 活动且CRT缓冲才释放base，清8/400标志和ptr/base/count |
| getstream 53F848 | 53F8C0 | 50 / 1 | 从槽0找非活动流或空槽，空槽分配32B；选中后只初始化已证字段 |
| openfile 53F6D8 | 53F848 | 124 / 1 | 解析原mode并调用sopen，成功增加108603C并发布FILE字段 |
| filbuf 53F3BA | 53F493 | 78 / 6 | 转读模式、必要时getbuf、read；EOF/失败置不同位，成功返回首U8 |
| flsbuf 53F8C0 | 53F9D5 | 105 / 7 | 转写门禁、必要时getbuf、写旧缓冲后放新低W8，失败EOF |
| stbuf 53EA5E | 53EAEB | 53 / 2 | 仅isatty且stdout/stderr，无缓冲时借两槽临时缓冲；分配失败用2B内部槽 |
| ftbuf 53EAEB | 53EB28 | 23 / 2 | 标志非0且临时缓冲时flush并拆除；标志0但仍有临时缓冲时也flush |
| getbuf 544815 | 544859 | 19 / 6 | 先增108603C、malloc(4096)，失败用FILE+14内部2B槽，最后ptr=base/count=0 |

合计509条。getstream的原共享表1086AC4/上限1087AE0、stbuf两槽1086040、
计数108603C均只记录职责；它们不是新端需要额外定义的应用共享owner。
低层read/write/sopen/isatty/lseek、全流初始化/退出和其他C++流caller仍独立处理。

### 关键失败与状态次序

原flush即使没有写数据，公共尾仍清count并重置ptr；失败后也走同一尾。
原freebuf不释放用户自带缓冲，不额外清bufsiz。原getbuf分配失败不返回失败码，
而是切入2B备用缓冲；不新增“malloc失败则整次文件操作必然失败”的应用分支。

原openfile首字节只接受r/w/a；有效前缀之后，重复或未支持的模式字符会停止后缀
解析，然后仍以已累计flags进入sopen。不能将这个行为误写成所有未知后缀都返回null。
宿主parse_mode会校验未知后缀、支持前导空格和额外编码/创建模式；
当前已关联调用继续使用既有原模式，不扩大接受范围或新改应用模式。

原flsbuf从读转写只检查已置EOF；宿主还会查询当前位置和文件长度判定EOF，
因此混合更新流的状态转换并非逐项相同。追加且旧缓冲为空时，原lseek失败被忽略，
随后仍写新字节进base；宿主内部先设IOERROR并提前离开write_buffer_nolock。
该宿主helper返回bool，而该分支写return eof(-1)，会转为true，
外层可能仍返回输入字符；本步按实际源码记录，不按注释推断必返EOF。
不修改宿主库，也不把这些差异记成原失败行为已等价。

### 宿主提供者与边界

本机UCRT的stdio/stream.cpp、openfile.cpp、_getbuf.cpp、_freebuf.cpp、
_filbuf.cpp、_flsbuf.cpp、_sftbuf.cpp及fflush.cpp对应上述职责：

- 流分配从_IOB_ENTRIES之后扫描，使用calloc和锁；区别于原从槽0起、32B malloc及选择性初始化。
- 普通缓冲用calloc，原用malloc；两者失败都有2B内部缓冲，但分配时点、大小宏、
  原未写字节和指针复用不据此判等。
- 宿主flush在不可刷新流上提前返回，且写前重置缓冲；原公共尾在写后执行。
  fseek/rewind等宿主caller另有reset_buffer，不能只看同名helper推全链时序相同。
- 宿主stderr即使非tty也可临时缓冲；原先要求isatty。宿主ftbuf标志为false直接返回，
  原flag0且保有临时标志仍flush。当前文件流消费不因此自动覆盖标准流重入/退出行为。
- 宿主filbuf窄字符路径保留U8/EOF、备用缓冲和读错误职责；额外校验、锁和共享状态
  与原不逐项等价，不将宽字符分支一并列作本线需求已完成。

只补九项限定职责映射，源码/CMake和原分母保持。全部旧caller的传递依赖、
标准流生命周期、系统调用失败与最终文件验收仍未完成。
不执行文件操作、构建、测试、客户端或哈希校验。

## 低层句柄与创建时间消费域（2026-10-02）

按现有工程委托，窗口存档继续使用宿主CRT描述符；stdio的低层句柄表也由宿主持有。
原资源时间窄适配则保持已实现的Win32句柄借用和旧时间计算，不改成现代_fstat/time_t。

| 原入口 | 结束地址（不含） | 指令数 / 直接引用数 | 本次归属 |
|---|---|---|---|
| close 53D30A | 53D3BD | 66 / 3 | 窗口存档直接_close及既有stdio关闭职责 |
| filelength 53D3BD | 53D435 | 45 / 1 | 窗口存档直接_filelength |
| open 53D435 | 53D44C | 7 / 1 | 窗口存档直接_open，share固定40h |
| sopen 53D44C | 53D705 | 230 / 2 | open/openfile的宿主私有打开职责 |
| lseek 53F493 | 53F52D | 51 / 18 | 文件定位/取长的低层职责 |
| commit 541222 | 541279 | 26 / 1 | fflush按commit标志调用的低层职责 |
| isatty 544705 | 54472B | 12 / 3 | stdio临时/普通缓冲选择职责 |
| fstat 53CCD0 | 53CF48 | 210 / 6 | 仅既有ReadLegacyFileCreationTime32创建时间消费域 |

前七项共437条为本轮指令核对；fstat沿[已有完整取证与实施合同](cg2d_resource_sources.md)
核当前源码和六处直接消费，不重复执行时间算法审计。合计元数据647条不等于全部重新实现。

### 窗口存档与低层结果

4C9089..4C90B3依次传pmode=180h、oflag=102h和原路径；
现有Sub_4C9060准确使用_O_RDWR|_O_CREAT及_S_IREAD|_S_IWRITE。
只有fd!=-1才调用_filelength，把完整S32结果写CC4238，再_close并忽略其返回。
不增加长度失败门禁，不把FILE*与独立fd合并，不扩为_filelengthi64。

原filelength先lseek(CUR,0)，失败-1直接结束；否则lseek(END,0)，
位置不同才尝试恢复，返回末尾查询结果、不以恢复失败覆盖它。宿主同有这一顺序，
但依赖宿主long定位与锁。原lseek用SetFilePointer/低W32及GetLastError判断，
成功后清低层EOF；宿主long路径用SetFilePointerEx、先保存位置，
超过LONG_MAX会恢复并返回-1。当前大文件/故障时点不宣称等价，不新增模拟回绕。

原close比较stdout/stderr底层句柄相同则跳CloseHandle，无论系统关闭是否成功，
随后都释放原描述符槽并清flags，再按保存错误返回。宿主还先要求另一个标准
描述符处于打开状态才走共享句柄特例；锁、校验与错误槽不同，原流状态不复制。
原isatty超范围直接0，不写错误槽；有效返回FDEV的40h位，不是固定布尔1。
宿主无效参数校验会写errno，不能把它视为本线旧错误槽同步。

原commit校验后FlushFileBuffers：系统调用失败取GetLastError，
只有错误非0才写旧DOS错误/errno9并返-1，错误0仍返回0；
宿主失败分支会最终返回-1。这里仅关联既有fflush的传递职责，不记失败完全等价。

### 打开及原状态归属

原sopen处理继承、二进制/默认文本、访问/共享/创建、只读属性、临时/访问提示，
分配描述符后CreateFileA，取类型、发布借用句柄与flags，再处理文本更新尾Ctrl-Z
及追加位。访问位或共享值非法写旧errno22/DOS0，分配槽失败errno24/DOS0；
系统失败沿542F44。其实际两个caller分别为open与openfile，没有应用直接_sopen调用。

宿主lowio/open.cpp由窄路径转换后进入宽路径CreateFileW，具有锁、参数校验、
扩展文本模式和失败清理；仍不以此批准改写应用路径编码/模式或创建第二套旧句柄表。
本文只关联真实窗口常量参数及既有stdio路径，完整低层打开/读写/关闭失败状态未验。
原末尾Ctrl-Z分支保留证据，不用常识改写原read返回值判定。
宿主errno/_doserrno不自动写回gLegacyErrno/gLegacyDosError。

### fstat创建时间窄域

全部六处直接调用来自45A290/45A420，每次只用返回及stat+20h创建时间。
当前ResourcePaths::ReadAutoMapFileTimes循环承接六逻辑点，
调用platform::ReadLegacyFileCreationTime32，再按原首文件/后续文件失败差异处理。
公开接口不提供mode/uid/size等其他stat字段，不称通用_fstat已实现。

句柄仅由_fileno/_get_osfhandle借用，时间仍依次转换修改、非零访问、创建时间，
保留ConvertLocalTime_541160的共享时区/DST副作用；creation为零回退已转换mtime。
既有设备/管道、PeekNamedPipe、原错误映射及失败不改。没有改成宿主64位time_t，
没有略掉未作为最终返回的访问时间转换，也没有重做该本体。

本步源码/CMake不变；只补八项限定记录，未操作真实文件、构建、测试或哈希校验。
