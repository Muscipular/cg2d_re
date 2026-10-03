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

## 初始化与全流清理职责 SPEC（2026-10-02）

本板块继续由宿主CRT持有FILE和描述符表；按既有工程委托，不另建原表或额外
应用层清理器。这里确定六入口的生命周期职责及明确差异，当前WinMain只作应用
入口定位，不能当作六个原CRT函数的直接实现或最终可执行目标的接线证明。

| 原入口 | 结束地址（不含） | 指令数 / 直接引用数 | 宿主职责 |
|---|---|---|---|
| 全流刷新 53BB99 | 53BBA2 | 4 / 1 | _flushall |
| flsall 53BBA2 | 53BC0F | 45 / 2 | common_flush_all |
| fcloseall 5446AD | 544705 | 34 / 1 | _fcloseall |
| initstdio 53E9A5 | 53EA4A | 59 / 1 | __acrt_initialize_stdio |
| endstdio 53EA4A | 53EA5E | 5 / 1 | __acrt_uninitialize_stdio |
| ioinit 53F52D | 53F6D8 | 143 / 1 | __acrt_initialize_lowio |

共290条。initstdio/endstdio的引用是CRT数据槽553174/553188，
不是业务call；ioinit唯一调用位于start@53DDB5的53DE24。
其余入向分别是endstdio→53BB99→flsall、fflush(null)→flsall及endstdio→fcloseall。

### 原全流刷新和关闭

53BB99固定传1到flsall。flsall从0遍历1086AC4表，每轮重读1087AE0上限；
非空且flags&83h才是活动流。mode1对每个活动流调用fflush，非EOF则计数；
mode0仅处理flags&2写流，任一EOF令最终错误为-1，否则0。
其他mode不刷新，返回0；宿主bool接口仅关联原已证的0/1两条调用。

fcloseall从索引3起，活动流fclose非EOF才计数；非空槽索引>=20时，
无论是否活动或关闭成功，都free槽并清空表项；3..19保留静态FILE存储。
返回成功关闭数，不能按宿主注释推成任意失败就返回EOF。

宿主stdio/fflush.cpp在全局锁内快照表尾，每流加锁前后重新判断；
有效但无需flush/commit的流直接累加count并跳过实际刷新。
宿主stdio/closeall.cpp从_IOB_ENTRIES=3起，对非空动态槽删除锁、free并置null。
因此只关联刷新/关闭职责，不宣称原逐轮读上限、调用次数或槽释放时点相同。

### 原stdio初始化和退出条件

initstdio将0上限设512，低于20则设20；calloc(count,4)失败回退20再试，
仍失败调用_amsg_exit(26)。20个表项指向58F8B8..58FB38的32B静态FILE。
标准fd0/1/2的低层句柄为-1或0时，把对应FILE.fd写-1。

endstdio先全流刷新，再检查byte1086020；仅非0才尾跳fcloseall。
该字节的已证写者是53C91C内53C949，沿既有[退出合同](cg2d_startup_flow.md)
处理，不重做退出算法或把清理条件改为常量。

宿主stdio/_file.cpp只有3个静态流，表上限默认512、下限和回退均为3；
二次分配失败返回-1，标准无控制台fd采用_NO_CONSOLE_FILENO。
退出则无条件_flushall、_fcloseall，再释放标准缓冲/锁和整个指针表。
initializers/stdio_initializer.cpp把初始化/退出注册到.CRT$XIC/.CRT$XPXA。
这些是本机源代码证据，当前最终目标的链接、注册执行和退出顺序仍未验。

### 低层句柄初始化与继承

原ioinit先malloc256B，容纳32项8B记录，失败调用_amsg_exit(27)；
每项置handle=-1、flags=0、+5字节为LF。GetStartupInfoA读取继承块：
S32数量N、N个W8标志、N个W32句柄；最多2048，扩容失败只继承已分配容量。
句柄非-1且FOPEN置位才接入；FPIPE绕过GetFileType，其余要求类型查询非0。
标准0/1/2有继承句柄则补文本位，否则用GetStdHandle查询；
无有效句柄时保留-1并标FDEV。最后调用SetHandleCount，EAX保留其结果。

宿主lowio/ioinit.cpp用GetStartupInfoW，数量仍int、标志仍W8，句柄数组实际为
intptr_t；不能按旧DWORD注释认定其仍是W32。分组64项、128组，上限8192。
继承另外排除_NO_CONSOLE_FILENO；无有效控制台句柄采用-2。
__acrt_initialize_lowio实际返回bool，不能按注释误写0/-1；
internal/initialization.cpp将它与__acrt_uninitialize_lowio配对，
后者释放句柄数组和锁。没有给未定位的原退出入口伪造映射。

句柄按已证地址语义由宿主intptr_t承接，不拓宽N或flags，不额外实现旧32位父进程
继承块解析。跨位宽继承、资源失效时点和初始化失败传播仍未验。
本步只新增六项限定记录；源码/CMake不变，不构建、测试、运行客户端或校验哈希。

## 描述符管理与低层读写 SPEC（2026-10-02）

继续现有宿主FILE/描述符owner，按工程委托只关联实际文件消费所需职责，
不混用原10869C0表、宿主__pioinfo和独立Win32句柄，也不添加第二套槽管理器。
下列六入口474条的全部22个直接引用都在CRT范围，应用源码只作传递消费根。

| 原入口 | 结束地址（不含） | 指令数 / 直接引用数 | 宿主提供者 |
|---|---|---|---|
| alloc_osfhnd 543711 | 5437A6 | 56 / 1 | lowio/osfinfo.cpp::_alloc_osfhnd |
| set_osfhnd 5437A6 | 54381D | 44 / 1 | lowio/osfinfo.cpp::__acrt_lowio_set_os_handle |
| free_osfhnd 54381D | 543897 | 45 / 1 | lowio/osfinfo.cpp::_free_osfhnd |
| get_osfhandle 543897 | 5438D4 | 16 / 8 | lowio/osfinfo.cpp::_get_osfhandle |
| write 541279 | 541426 | 139 / 7 | lowio/write.cpp::_write |
| read 541426 | 54161C | 174 / 4 | lowio/read.cpp::_read |

### 原槽分配、发布和释放顺序

alloc仅由sopen@53D5C4调用。依序扫描组及组内32项，找到FOPEN清零项只把
handle置-1并返回索引；不会在此设置FOPEN或重置全部其他字段。
遇首个空组malloc256B，失败返回-1；成功先增加uNumber32、发布组指针，
逐项写flags0、handle-1、lookahead LF，再返回组首索引。组内每项8B，最多64组。

set仅由sopen@53D640调用。unsigned fd必须低于uNumber且原handle为-1；
当58F8A4为1，fd0/1/2先SetStdHandle，对其返回不设门禁，随后写入新handle并返0。
原sopen在5437A6返回后才于53D669发布含FOPEN的flags；不能提前到alloc中。

free仅由close@53D386调用。校验范围、FOPEN及handle!=-1；
控制台标准槽先SetStdHandle(null)，再将handle置-1并返0。
它不关闭系统句柄、不清flags；原close在53D38E随后清flags。
set/free/get失败写原DOS错误0、errno9并返回-1。
get校验范围和FOPEN后直接取W32 handle，不另校验该值是否-1。

宿主alloc在全局索引锁内分配/搜索，成功时槽锁仍持有，已把FOPEN置位；
新组calloc并初始化额外字段/锁。set/free具有对应标准句柄更新职责，
但使用宿主应用类型和intptr_t句柄；get还经过宿主无控制台/参数校验策略。
这些时序和错误状态不宣称等价，不用宿主errno自动填充本线旧错误槽。

原get的八入向来自close四处、lseek、commit、5438D4和54B658。
原fstat53CCD0直接读旧表，不是get调用者；当前ReadLegacyFileCreationTime32
的_get_osfhandle是既有x64句柄借用适配，不能反写成原调用证据。

### 原低层write及宿主差异

七入向来自flush、fwrite、flsbuf两处、5438D4及54B450两处。
原先校验fd/FOPEN，数量0返回0；追加位置位时先lseek(END,0)，忽略定位失败。
二进制直接WriteFile；文本逐字节将LF扩成CRLF，写入后以已写总量减已增加CR数返回。
原转换缓冲在输出字符后检查400h阈值，末字符为LF时该轮可达到401h，
不是按输入字符固定1024切块，也不能把新增CR计数改为仅成功写出部分。

累计已写非0时保留部分结果；尚无写出而有系统错误时，错误5映射旧errno9/DOS5，
其余走542F44。无写出也无系统错误时，仅设备首字节Ctrl-Z返回0，
否则errno28/DOS0返回-1。

宿主ANSI文本helper同有LF扩展及累计计数职责，但BUF_SIZE实际为5*1024，
不能照其“4K”注释记录；预留末字节后填充，系统调用分块和短写时点不同。
追加使用内部64位定位，同样忽略定位失败；另有锁、空指针校验、Unicode模式、
控制台编码转换与现代错误槽。现有Input的fwrite根只关联其文件传递路径，
不因此将其他C++流、设备或控制台行为验收。

### 原低层read及宿主差异

四入向来自fread、sopen文本尾检查、filbuf及54B574。
原先校验fd/FOPEN，数量0或低层EOF位已置返回0。管道/设备lookahead非LF时，
先把该字节写入目标、减剩余数量并将槽回置LF，再调用ReadFile。
错误5写errno9/DOS5；错误109返回0；其他错误走542F44。
这些失败不撤销先前已写入的lookahead字节。

二进制返回已取字节数；文本原地压缩CRLF，首字节LF的FCRLF判断还要求
本次ReadFile数量非0。Ctrl-Z立即停止：非设备置FEOFLAG，设备不置位，
两者都不把这个Ctrl-Z复制到结果区。原5415E8..5415F8为直接依据。

缓冲末尾CR再读1字节：设备/管道的非LF预读保存回lookahead，输出CR；
LF则输出LF。可定位文件在当前结果为空且预读LF时直接输出LF，
否则lseek(CUR,-1)退回预读字节并忽略返回，仅预读非LF才输出CR。
原peek ReadFile失败还取GetLastError，非0才直接走保留CR分支；
错误0继续检查数量，不把它简化成“系统调用失败必定直接返回”。

宿主read及translate_text_mode_nolock另有INT_MAX数量门禁、锁、模式处理和内部
64位回退；设备Ctrl-Z分支会复制该字节后停止，与原明确不同。
宿主peek只要ReadFile失败就保留CR，不走原错误0路径。
FCRLF使用传入转换数量；lookahead、控制台及扩展模式仍有独立状态差异，
本步不以同名API或代码注释消除这些差异。

本组只归属现有Sub_4C9060的打开/取长/关闭、Input历史文件读写等真实依赖。
完整文本/设备、短读短写故障、其他C++流父链和最终文件板块验收仍未完成。
源码/CMake不变；不构建、测试、运行客户端、实际游戏文件操作或哈希校验。

## 格式化公共入口与有界标题消费（2026-10-02）

本步沿现有宿主格式化调用，不重建原32B临时FILE或另一套变参ABI。
三个原公共入口的边界与核心调用如下，84条是包装层指令数，不含输入/输出引擎：

| 原入口 | 结束地址（不含） | 指令数 / 直接引用数 | 原包装行为 |
|---|---|---|---|
| sprintf 53B304 | 53B356 | 32 / 1022 | 临时流flags42h、count7FFFFFFFh，调用53EB28 |
| snprintf 53D21D | 53D26E | 33 / 9 | 同flags42h，count取传入W32，调用53EB28 |
| sscanf 53CADC | 53CB10 | 19 / 22 | flags49h，先strlen确定W32长度，再调用53DEF4 |

两个输出包装都先保存核心返回，再尝试写终止NUL；
剩余count减一后非负则直接写，负数走flsbuf，随后仍返回保存的核心结果，
不以末尾NUL写入失败覆盖它。刚好写满容量不会因此强制改最后一个字节为NUL。
输入包装只是提供现有字符串长度与变参起点，不凭19条推断全部扫描语法已验。

### 九处原有界标题调用

| 原父入口 | 数量 | 格式 | 当前实现 |
|---|---|---|---|
| 48B560 | 256 | %s(%d) | WindowRuntime::Sub_48B560 |
| 4B8310、4B8780、4B8C90 | 各100 | %s(%d) | 同地址Sub入口 |
| 4C17B0、4C3A50 | 各两处100 | %s(%s%d) / %s(%s) | 同地址Sub入口 |
| 4DA400 | 28 | %s(%d) | LegacyUiRecord::Sub_4DA400 |

原调用点分别48B68C、4B8430、4B88A4、4B8DB0、4C18DD、4C18FF、
4C3B7D、4C3B9F、4DA457；全部弃返回并继续后续描述/绘制。
当前九处都用_snprintf，未改成标准snprintf、_snprintf_s或追加尾零。
旧4DA400的32B局部只传28，现有其他局部的初始化边界保持，不在本步重写。

UCRT stdio.h的_snprintf→_vsnprintf→_vsnprintf_l传
_CRT_INTERNAL_PRINTF_LEGACY_VSPRINTF_NULL_TERMINATION，
再进入stdio/output.cpp::__stdio_common_vsprintf。
legacy分支在已用数量不等于容量时补NUL，刚好填满保留末字节；
标准snprintf分支则在填满时改写最后字节并继续计所需长度。
只关联上述正容量、窄%s/%d消费及其弃返回合同，不将宿主全部格式/失败行为判等。

### sprintf与sscanf限定根

sprintf仅在本步关联Input::Sub_449FA0@449FBB、Sub_44A090@44A0B5的
既有路径拼接：原字节路径/文件名、目标128B、弃返回和后续文件打开顺序不改。
原1022处引用远多于本次两个消费点；其余动态格式、locale、浮点/宽字符、
私有引擎和错误状态不凭宿主std::sprintf存在一并验收。

sscanf仅关联StartupCommon::ParseClientVersion原4659F0@465A09：
固定输入Ver 1.02.02、固定格式Ver %d.%d.%d、三个S32输出以及原返回传递。
宿主stdio/input.cpp::__stdio_common_vsscanf使用有界字符串adapter及宿主locale，
具有非空参数校验；原内部FILE、长度读取方式及完整扫描/错误状态不等价。
其余命令行、场景及暂停音频的调用继续各自验收，不借此完成newest排除链。

本步新增三项限定记录，源码/CMake不改；不新增包装、失败门禁或缓冲清零。
未构建、测试、运行格式化样例、客户端或哈希校验，格式化整板块仍未完成。

## 格式化字符写出与变参取值依赖（2026-10-02）

本组承接前述sprintf/_snprintf的宿主输出引擎，定位六个原私有辅助入口，
共90条、22个直接引用。不把x86变参栈布局复制到x64，不新建临时旧FILE或参数影子表。

| 原入口 | 结束地址（不含） | 指令数 / 直接引用数 | 原职责 |
|---|---|---|---|
| write_char 53F2C6 | 53F2FB | 25 / 4 | 写低U8或调flsbuf，更新S32输出计数 |
| write_multi_char 53F2FB | 53F32C | 22 / 3 | S32正数量重复写，失败即停 |
| write_string 53F32C | 53F364 | 27 / 3 | 按数量逐字节读取/写出，失败即停 |
| get_int_arg 53F364 | 53F371 | 5 / 10 | 参数游标先加4，再取前一W32槽 |
| get_int64_arg 53F371 | 53F381 | 6 / 1 | 游标先加8，低W32到EAX、高W32到EDX |
| get_short_arg 53F381 | 53F38F | 5 / 1 | 游标先加4，只读前槽低W16到AX |

### 写出与失败传播

write_char先减FILE.count；非负则写参数低U8、递增FILE.ptr，
否则把完整参数交flsbuf。返回EOF时将调用方计数写-1，否则递增计数。
它不以输入字节FF直接判失败，也不先因传入计数已-1而跳过写操作。

write_multi_char和write_string只有初始S32数量>0才开始；
每次调用write_char后检查计数是否恰为-1，再决定继续。
字符串按有符号char扩展传给write_char，但落盘/写缓冲仍取低U8；
每轮先取该字节并前移局部源游标，然后才尝试写出。失败不会回滚已写前缀。

UCRT corecrt_internal_stdio_output.h提供output_adapter_common、
string_output_adapter以及write_multiple_characters。
字符串adapter会在可用容量内批量memcpy，再更新位置/计数；
标准snprintf可启用continue_count，legacy路径不因满缓冲继续计全部需要量。
原逐字节读取/修改和宿主批量复制不是任意源目标别名下的等价证明，
本步只延续前述实际路径/标题消费，不扩大成所有重叠或负长度输入都已验。

宿主stream adapter的write_string_impl对EILSEQ存在写'?'后继续的恢复分支，
原三个helper没有这个恢复分支。该宿主分支不作为本线允许替换/丢字的新增裁决，
宽流及编码错误路径仍保持未验；不改应用错误槽来掩盖差异。

### 变参物理槽不等于业务类型

get_int_arg十个调用都在53EB28：53EC1D/53EC5D用于宽度/精度；
53ED7B及53EE65取得后作为地址使用；53F017的返回用于写回W16或W32计数。
因此W32取槽helper不能统一命名为“业务有符号整数”，更不能把实际指针截断成W32。

数值分支53F06A后movsx AX，53F076后movzx AX，再决定高W32；
53F08B走S32符号扩展，53F093清高W32；53F052读取完整64位槽。
符号/无符号与短值选择发生在父引擎，不由get_int_arg擅自决定。

get_short_arg唯一53EE05调用后，把返回交wctomb，后者只取W16。
原helper没有movsx/movzx EAX，其高位仍来自参数游标，不能凭函数名写成
返回“已符号扩展S16”的完整EAX合同，也无需对未消费的高位造地址残值。

宿主使用read_va_arg<T>及extract_argument_from_va_list，宽度/精度为int，
字符串/写回分别取char*/void*，整数按格式选择相应宽度和符号。
x64参数位置由宿主va_list/ABI负责；只拓宽真实地址，数值语义不随之扩为64位。
这里只定位实际实现及类型边界，不宣称所有旧格式/ABI/错误和父引擎已验收。

本步仅补六项限定映射；既有应用源码/CMake不改。
未构建、测试、运行格式化样例或哈希校验，53EB28完整状态机仍待独立收口。

## 格式化输出核心与fprintf链（2026-10-02）

本步核output@[53EB28,53F2A6)623条及fprintf@[53BC0F,53BC41)22条，
共645条。输出核心仅三个直接caller：sprintf@53B32D、fprintf@53BC28、
_snprintf@53D245。此前公共包装、六个写出/取参helper和宽转换记录共同构成
依赖关系；宿主承接仍只限已列实际消费，不把本次取证记为旧核心直接实现。

### 八状态分派与表边界

53EBA4实际W32跳表为[53F2A6,53F2C6)，八项32B位于函数结束之后，
不属于623条指令，也不是独立函数。八目标均在53EB28主体内：

| 状态 | 实际目标 | 职责 |
|---|---|---|
| 0 | 53ECDB | 普通字节及原lead-byte处理 |
| 1 | 53EBAB | 清当前格式标志、宽度等，精度=-1 |
| 2 | 53EBC8 | 空格、#、+、-、0标志 |
| 3 | 53EC14 | 宽度数字或* |
| 4 | 53EC4B | 小数点后精度置0 |
| 5 | 53EC54 | 精度数字或* |
| 6 | 53EC8C | 长度修饰符 |
| 7 | 53ED20 | 类型分派 |

分类及转移压在[54F96C,54F9C5)89个字节中：
53EB80以54F94C+字符索引读取低4位，字符仅在20h..78h范围才查表；
其他含高位有符号字节走类别0。
53EB8D以54F96C+类别*8+旧状态取字节，符号扩展后右移4位得到新状态。
共九类、八状态；前72B的高半字节转移与89B低半字节字符分类共用存储，
不是两张各自可随意扩容的独立表。

类别1为%，2为点，3为*，4为0，5为1..9，6为空格/#/+/-；
类别7包括F/I/L/N/h/l/w，但实际修饰分支只对I64、h、l、w写对应标志。
类别8还含B等字符，不能仅因进入类型类别就宣称每一字母都有正常类型实现；
类型树没有相应分支的情形不凭现代printf常识补逻辑。

### 已证数值、文本和计数行为

宽度*为负时置左对齐并取负；精度*为负恢复-1。
有符号整数通过父分支区分S16/S32/S64，非有符号数值分别U16/U32/U64；
未指定I64时高W32清零。整数精度默认1，显式精度清零填充标志并最大取512。
数值为0时清十六进制前缀；八进制#需要时补首0。
原p把精度固定8并采用大写十六进制，属于原32位地址显示；不能把此规则
拿来截断新端实际指针，现代指针格式仍是单独的x64边界。

窄s与宽s/S按精度限制扫描字节或W16；空源分别使用原窄/宽null文本指针。
Z按记录内S16长度和+4文本指针处理，宽标志下长度逻辑右移1；
不把这份旧32位记录布局转嫁到宿主对象。
n在53F017取真实指针，h标志写低W16，否则写W32计数，并跳过输出。
原没有全局“启用n”门禁，也不因为I64修饰而把该写回扩为W64。

普通字节命中旧ctype lead位时先写首字节，再读取和输出下一字节。
宽单字符转换失败只抑制当前字段；宽字符串逐W16转窄，转换返回<=0即停止该段，
但53F262前没有主动把累计计数改为-1。已成功写出的前缀不回滚。
公共输出依次为左空格、符号/前缀、零填充、正文、右空格；
原并非每一步前都重新以负计数阻止进入辅助调用，外层下一字符才统一检查负计数。

### 浮点槽和临时缓冲

浮点默认精度6，g精度0改1，显式精度最大512；超过163时尝试malloc(precision+349)，
失败把精度回退163，成功则本字段尾free并清局部指针。
53EF18/53EF33/53EF44分别通过58FB50/58FB5C/58FB54间接调用转换、强制小数点、
g尾零裁剪。三槽已加载初值均为545031的浮点trap，不能当作已初始化转换器。

53B45F真实写者把这些槽设置为54019A、53FDD5、53FE2F；
它还设置58FB58、58FB60、58FB64。53B447调用此初始化后做P5探测、
精度设置及FNCLEX。这里仅核浮点依赖边界，不把PC53既有窄适配推广为
全部浮点格式已完成，也不在新端造这些x86槽或声称启动时点已验。

### 宿主差异与fprintf消费

宿主corecrt_internal_stdio_output.h::output_processor按原生va_list/adapter工作，
整数显式精度按请求扩缓冲；浮点经__acrt_fp_format及自己的扩容/失败回退，
不沿原163/512和函数槽启动机制。新端已有运行库职责不证明旧资源失败时点等价。

UCRT printf_count_output.cpp的启用状态初值0，type_case_n在禁用时走校验失败；
原n直接写回。这里不擅自启用进程全局选项，也不称未核的动态格式与旧n等价。
宿主非UTF-8宽字符串转换失败将输出计数设-1；原上述路径保留此前累计值。
这些是明确的未闭合差异，不因公共sprintf/_snprintf映射存在而消失。

原fprintf有11个直接引用；本步只关联Input::Sub_449DA0的六处日志输出。
包装先stbuf保存临时缓冲标志，再调output保存结果，最后ftbuf(flag,FILE)，
返回仍为核心结果，不用收尾返回覆盖。宿主common_vfprintf使用参数校验、
流锁及temporary_buffering_guard；既有stbuf/ftbuf差异继续有效，不重做其审计。
其他旧日志待批项、输入/网络日志父链及实际文件持久化不计通过。

本步仅补输出核心/fprintf限定归属和跳表分类，不改业务源码或运行库配置。
未构建、测试、运行格式化样例、客户端或哈希校验；总格式化与启动退出仍未验收。

## 文件位置、缓冲调整及宽字符流的覆盖层级（2026-10-03）

本组十四入口999条代码指令、25直接引用；八叶408条完整读取，其余六项只边界/调用归属。
当前应用源码没有这些原入口的独立显式调用，不将运行库父链定位当作客户端已实现/已运行。

| 入口 | 指令数 | 消费与证据层级 |
| --- | --- | --- |
| 54ACB7/54ACD9 | 13/8 | 全读；548B68/548BEA文件位置获取、恢复 |
| 54ACF1 | 64 | 全读；窄/宽filebuf两setbuf调用 |
| 54AD92/54AF70 | 91/90 | 全读；宽filebuf输出/读取 |
| 54AE87 | 90 | 仅边界；宽filebuf回推，内部getbuf/wctomb |
| 54B239/54B3C0 | 144/58 | 仅边界；64位tell/seek及缓冲/文本偏移修正仍待 |
| 54B450/54B574 | 112/80 | 仅边界；宽字符缓冲写出/补入仍待 |
| 54B658 | 59 | 全读；tell/seek私有底层64位定位 |
| 5438D4 | 107 | 仅边界；53D44C打开链内部调整长度，包含模式切换/位置恢复 |
| 5463A3 | 35 | 全读；仅长度调整两处切换文本/二进制 |
| 54463F | 48 | 全读；输入核心、窄filebuf及宽读取的字节回推 |

fgetpos先把tell结果按低/高W32写回，即使失败也发布-1，再判两半均FFFFFFFF返回-1；
fsetpos读两半，以origin0交64位seek。fpos是64位数值，不随x64再扩成别的宽度。
底层lseeki64检查fd有效/打开后调SetFilePointer；低返回FFFFFFFF才查GetLastError，
非零映射错误并返回完整-1，成功清fd的02H标志。无效fd还清旧doserror并设errno9；
没有在调用前加新的LastError清零或把返回低半部单独当失败。

setvbuf的无缓冲mode4不检查容量；其他只接受mode0/40H及2..7FFFFFFF容量，取偶数后
先flush、freebuf并清缓冲相关标志。内置缓冲用2B；用户缓冲直接借用；自行分配失败
增加108603C并返-1，不恢复旧缓冲。宿主setvbuf保留相似次序但有锁/校验并以calloc
代替原malloc，分配/清零和错误行为差异继续单列，不回写旧FILE布局。

fputwc在文本流先wctomb：-1设置旧errno42并返WEOF；结果1写一字节，其他成功结果
走两字节路径，各次写入都检查失败，成功返回原W16。非文本/字符串流直接W16缓冲。
fgetwc文本流先读一字节，依旧ctype lead标志读取第二字节；第二字节EOF时尝试回推
首字节后返WEOF，不擅自把回推视为必成功；转换失败设errno42。其他流直接取W16。
ungetc的字符串流只能回推同一字节，失配还原指针；成功增加count、清EOF并设读标志。

旧setmode仅接受4000H/8000H，改变fd文本位并返回旧模式，非法模式errno22、非法fd
errno9。本机setmode.cpp另支持WTEXT/U8TEXT/U16TEXT；lseek.cpp用SetFilePointerEx。
fgetpos.cpp同样在有效参数下先写_ftelli64结果，但另有参数验证。宿主对应源仅说明
差异与职责，不表示完整旧宽流/长度调整/失败恢复已由当前客户端接线。

十四项sources为空，semantic records550；当前stdio、文本及错误状态实现保持，不建
影子FILE/fd表、不恢复暂停音频或扩大CRT总提案。仅静态核分层证据、记录和UTF-8/LF，
未构建、查产物、新增或执行测试、运行文件/宽字符样例或哈希校验。完整FILE父链、
运行库承接及总PLAN继续未完成。

## 窄宽流缓冲推进与批量传输入口（2026-10-03）

当前原会话完整读取以下十四个streambuf入口，共312条代码指令、20个直接引用。
其中十处为虚表数据项，不能因无普通call入向就判不可达，也不把数据项算业务caller。
只新增取证分类，sources全部为空；未创建新端旧streambuf、FILE/fd影子表或代用包装。

| 入口..结束 | 职责 | 指令 | 直接引用 |
|---|---|---:|---:|
| 0x5486C9..0x5486E6 | 窄uflow | 14 | 1 |
| 0x5486E6..0x54875A | 窄xsgetn | 52 | 2 |
| 0x54875A..0x5487D0 | 窄xsputn | 53 | 2 |
| 0x548EA5..0x548EB5 | 窄Gndec | 7 | 1 |
| 0x548EB5..0x548EC5 | 窄Gninc | 7 | 2 |
| 0x548EC5..0x548ED5 | 窄Pninc | 7 | 1 |
| 0x549787..0x5497A8 | 宽uflow | 14 | 1 |
| 0x5497A8..0x54982F | 宽xsgetn | 61 | 2 |
| 0x54982F..0x5498B7 | 宽xsputn | 62 | 2 |
| 0x549E08..0x549E1C | 宽gbump | 7 | 1 |
| 0x549E1C..0x549E2D | 宽Gndec | 7 | 1 |
| 0x549E2D..0x549E3D | 宽Gninc | 7 | 2 |
| 0x549E3D..0x549E51 | 宽pbump | 7 | 1 |
| 0x549E51..0x549E61 | 宽Pninc | 7 | 1 |

### 状态单位与读写顺序

读侧+1C、写侧+20是指向游标槽的指针，+2C/+30是指向余量槽的指针；
函数更新的是间接槽，不能改为每个调用者私有的游标/计数副本。
窄字符每项1B，宽字符每项2B，数量和返回完成数仍是原S32字符数。
本轮不推定整个streambuf/FILE对象完整布局或这些借用槽的最终owner。

- 0x5486C9：先调虚表+10的underflow，完整EAX为FFFFFFFF即返回；否则Gninc后从旧游标读U8并零扩展。
- 0x5486E6：S32请求非正返回0；有游标且余量正时取较小数量交541620，随后重读余量/游标槽再更新；否则虚调+14，遇FFFFFFFF停止并返回已复制数。
- 0x54875A：正请求按可用余量批量541620复制后重读写槽，或零扩展源U8调用虚表+4；完整FFFFFFFF停止，返回完成数量。
- 0x548EA5：先递增所指余量，再递减所指游标1B，重读游标槽后返回新游标。
- 0x548EB5：先递减所指余量，再保存旧游标并发布旧值+1B，返回旧游标。
- 0x548EC5：输出侧先减所指余量，保存旧游标后发布+1B，返回旧游标。
- 0x549787：先虚调underflow并仅比较AX与FFFF；EOF路径返回0000FFFF，成功Gninc后读取W16到AX，不能泛化为完整W32字符返回。
- 0x5497A8：S32请求非正返回0；余量先shl1/sar1，再按较小数逐W16正向复制，之后gbump；无可用块则虚调+14，仅AX==FFFF时停止。
- 0x54982F：可用余量经shl1/sar1后逐W16正向复制并pbump；无块时源W16到AX后虚调+4，仅AX==FFFF停止，返回已完成字符数。
- 0x549E08：所指余量先减S32字符数，再重取游标槽并加2*字符数，不按字节数直接减余量。
- 0x549E1C：先增所指余量，再将游标减2B，重读槽后返回新游标。
- 0x549E2D：先减所指余量，保存旧游标并发布旧值+2B，返回旧游标。
- 0x549E3D：输出侧先从余量减S32字符数，再重取游标槽并加2*字符数。
- 0x549E51：输出侧先减所指余量，保存旧游标再发布+2B，返回旧游标。

窄字符批量路径使用541620；宽字符有明确的逐W16正向复制顺序，不能仅凭函数名
把两者统一成按字节memcpy、按字符memmove或宿主traits调用后就称等价。
宽余量的shl1/sar1事实单列，不新增范围钳制或专项故障模拟。
批量函数的无缓冲路径调用虚表+14或+4；虚表槽宽度/字节偏移属于原x86 ABI，
新端尚未承接，不能直接把这些偏移搬到x64虚表。

所有单步叶都有间接状态副作用；Gndec返回移动后的地址，Gninc/Pninc返回移动前地址，
不能按同形七指令合并成相同返回合同。窄uflow比较完整EAX的FFFFFFFF，
宽uflow/批量路径比较AX的FFFF，宽成功路径未普遍保证高W16值。

### 剩余边界与验证

完整filebuf虚调用目标、构造/绑定/解绑、流状态、异常及所有父调用生命周期仍待。
CRT宿主提案尚未确认；现有应用用标准流并不证明本表私有布局、共享槽与失败路径已承接。
本轮静态检查原ID/地址、312指令/20引用、旧记录保持与JSON/UTF-8/LF；
未构建、查产物、新增或执行测试、运行流/拷贝样例、客户端或哈希校验。

## 流缓冲绑定、关闭与位置对象边界（2026-10-03）

承接上一节，完整读取23入口507条代码指令/34直接引用，其中22处虚表数据项。
下表只登记原本体事实和未决承接，sources均为空；不新增影子FILE、旧虚表或析构包装。

| 入口..结束 | 职责 | 指令 | 引用 |
|---|---|---:|---:|
| 0x548CAC..0x548CD7 | 窄setbuf | 18 | 1 |
| 0x548CD7..0x548CF6 | 窄析构包装this-4 | 12 | 1 |
| 0x548CF6..0x548D12 | 窄析构包装this | 11 | 1 |
| 0x548D12..0x548D31 | 窄析构包装this-8 | 12 | 1 |
| 0x548D31..0x548D4D | 窄streambuf析构包装 | 11 | 1 |
| 0x548D4D..0x548D69 | 窄filebuf析构包装 | 11 | 1 |
| 0x548ED5..0x548EFC | 窄close | 20 | 1 |
| 0x548EFC..0x548F74 | 窄_Init | 40 | 2 |
| 0x549147..0x549194 | 反向字节回推 | 36 | 2 |
| 0x549CE5..0x549D13 | 宽setbuf | 20 | 1 |
| 0x549D13..0x549D2C | 窄宽共享sync | 12 | 2 |
| 0x549D2C..0x549D4B | 宽析构包装this-4 | 12 | 1 |
| 0x549D4B..0x549D67 | 宽析构包装this | 11 | 1 |
| 0x549D67..0x549D86 | 宽析构包装this-8 | 12 | 1 |
| 0x549D86..0x549DA2 | 宽streambuf析构包装 | 11 | 1 |
| 0x549DA2..0x549DBE | 宽filebuf析构包装 | 11 | 1 |
| 0x549E61..0x549E88 | 宽close | 20 | 1 |
| 0x549E88..0x549EDE | 宽_Init | 28 | 2 |
| 0x549EDE..0x549F20 | 缓冲槽重绑定 | 25 | 4 |
| 0x548B68..0x548BEA | filebuf相对定位 | 52 | 2 |
| 0x548BEA..0x548CAC | filebuf位置恢复 | 78 | 2 |
| 0x5498B7..0x5498EE | 基类相对定位失败 | 22 | 2 |
| 0x5498EE..0x549925 | 基类位置恢复失败 | 22 | 2 |

### 绑定、关闭与析构

窄_Init先写+48=(mode==1)，mode0先析构/重建+4C locale并清+44，再共同reset。
FILE非空且+48为0时，+1C/+20借用FILE的游标槽，+0C/+10借用FILE+8，
+2C/+30借用FILE+4。宽_Init不作这次FILE别名绑定，不能合并两者为同一模板行为。
窄状态来自两次独立读取1087B00，宽来自1087AF8；都先写+40，随后清+38并写+3C。

549EDE先将六间接槽重绑到对象自身+4/+8/+14/+18/+24/+28，按写侧基址/游标/余量、
读侧基址/游标/余量顺序清值。它不清整个streambuf，也不是释放旧内存。
这些原x86字段偏移不作为直接可用的x64布局；最终对象owner与底层FILE承接继续未决。

窄/宽close都只有fclose成功才_Init(nullptr,2)并返回this，失败返回空且不执行重置。
setbuf保持mode0，窄传原字节数，宽先按W32把字符数乘2；不存在FILE或setvbuf失败返回空。
549D13由窄宽两个虚表共同引用，FILE空返回0，fflush只有负数时返回-1。

十个删除型析构包装保留各自this调整与真实目标：
548CD7(-4→54823D)、548CF6(→54852C)、548D12(-8→5481F9)、548D31(→5486B1)、
548D4D(→5487DE)、549D2C(-4→549324)、549D4B(→549638)、549D67(-8→5492E0)、
549D86(→54976A)、549DA2(→549935)。flag仅bit0决定是否delete；返回保存的原/调整后地址，
即使已释放也不改成空。仅核这些包装不等于其被调析构和异常清理已全部实现。

### 回推与位置结果

549147从源末端逐U8调用ungetc，完整FFFFFFFF停止，仅AL表达最终成功与否。
保留失败尾的条件fgetc路径，不发明恢复全部已回推字节的事务。

548B68/548BEA被窄宽filebuf虚表共享。前者先fseek，再重读FILE作fgetpos；
后者先fsetpos，附加偏移非0才fseek(CUR)，再fgetpos。后者成功且+44非空时，
先恢复转换状态+40，再对关联旧字符串erase(0,npos)，不能省略共享字符串副作用。

位置结果整块复制六W32（24B），包含已写+0、+8/+C和+10。
548B68及基类默认失败入口5498B7/5498EE在本体不写局部结果的+4/+14；
548BEA使用传入位置对象，仅覆盖上述已写字段，其他槽随输入保留。
不将未写字节补零、不声称所有24B都有确定值，也不把默认失败入口当空函数。
5498B7与5498EE分别ret10H/20H，保持两种调用合同与原身份。

失败位置的5500E0/E4已加载值均0。1087B00/1087AF8/1087B04本轮读取时未加载，
不把读取API的填充值当作原运行时值；三者仍为真实可变状态依赖。

### 当前进度

新增23项仅完成本体取证与职责定位，构造、虚调用父链、FILE/locale/位置对象共享和
异常寿命仍未实现或未验收。待批CRT宿主提案不因这些记录自动生效。
仅静态核对稳定ID、计数/入向、旧记录保持及UTF-8/LF；未构建、查产物、新增或执行测试、
运行流/回推/定位样例、客户端或哈希校验。

## 窄宽filebuf读写与转换状态边界（2026-10-03）

完整读取八个filebuf读写入口647条代码指令，八个直接入向均为各自虚表数据项。
不是无调用入口，也不以指令数替代新端实现；以下八项sources保持为空。

| 入口..结束 | 职责 | 指令 | 虚表引用 |
|---|---|---:|---|
| 0x548843..0x548979 | 窄overflow | 124 | 0x54FFD8 |
| 0x548979..0x548A26 | 窄pbackfail | 70 | 0x54FFDC |
| 0x548A26..0x548A53 | 窄underflow | 24 | 0x54FFE4 |
| 0x548A53..0x548B68 | 窄uflow | 106 | 0x54FFE8 |
| 0x54999A..0x549AE2 | 宽overflow | 124 | 0x550074 |
| 0x549AE2..0x549B99 | 宽pbackfail | 69 | 0x550078 |
| 0x549B99..0x549BC7 | 宽underflow | 24 | 0x550080 |
| 0x549BC7..0x549CE5 | 宽uflow | 106 | 0x550084 |

### 写入

548843/54999A先处理EOF参数，再检查当前输出游标及余量。已有空间先推进指针/减余量，
随后写入字符；窄为U8、宽为W16，不能把写入放在指针推进前。无FILE则失败，
无+38转换器时分别fputc/fputwc，成功回传原参数；窄fputc实参由低U8符号扩展。

有转换器时都使用+44指向的旧窄字符串：先erase(0,npos)，append(8,0)，按原次序多次
Freeze并重新读取buffer/长度；窄输入范围1B，宽2B。随后经转换器虚表+14调用：
结果0进入字节输出；结果1要求输入next已前进，否则失败；结果3直接fputc/fputwc，
其他结果失败。字节输出的fwrite(size=1,count=产生字节数)必须返回完整count，
不把短写当成功，也不补重试/缓冲回滚。

宽EOF相关路径有的只写AX，有的写完整0000FFFF；不能一概宣称高W16同值。
本轮不建立依赖未规定高位的现代返回接口。

### 回推与只查看

pbackfail先查已有读前缀。EOF参数或前一字符相等时，Gndec按窄1B/宽2B移动；
EOF回推成功返回0，其他保留原参数。未命中时必须有FILE且参数非EOF：
无转换器走ungetc/ungetwc；有转换器不再增加字符相等检查，而是要求+44字节串非空，
Freeze后调用549147将整串反向逐字节推回。只有回推成功才erase并将+3C恢复到+40。

underflow在已有缓冲时直接读当前字符且不推进；否则先缓存同一个虚表指针，
经+14消费，再把返回值交缓存表+8回推，最终返回回推结果。不能换成重新取虚表、
直接fgetc后返回原字符、固定成功或无副作用peek。

### 读取与部分转换

uflow缓冲命中时先Gninc，再读取原游标；无FILE失败，无转换器分别fgetc/fgetwc。
转换路径先将+40备份到+3C、清+44窄字符串；之后均用fgetc每次取一个原字节并append。
每次转换前都将+3C恢复到+40，Freeze/取指针后经转换器虚表+10调用：
结果0返回转换产生的字符，结果1继续读字节，结果3需累计窄1B/宽2B后以541620复制；
其他结果或底层EOF失败。失败不新增清串/恢复状态/回推已消费字节。
宽读取的转换输入仍是窄字节串，不能按fgetwc重写这条路径。

### 当前未完成项

本节只按实际返回码0/1/3记录分流，不以宿主facet枚举或类名推断虚目标已经承接。
转换器+38、字节串+44、状态+3C/+40的完整生产/绑定及FILE/locale寿命仍待收口；
原虚表偏移属于x86 ABI，不直接移植为x64偏移。CRT宿主提案仍待明确确认。

静态核对新增稳定ID/地址、647指令/八虚表引用、原记录保持及JSON/UTF-8/LF；
未构建、查产物、新增或执行测试、运行读写/转换样例、客户端或哈希校验。
不将本体取证计为父应用、字符转换或完整标准流运行通过。

## 原FILE存储、清理及描述符关闭实施 SPEC（2026-10-03）

CRT宿主总提案未获批准，本节将退出所需的旧FILE本体逐步落地；
此前“仅宿主职责关联、不建旧表”不作为本组新实现的口径。新增LegacyStdio及
LegacyLowIo内部模块，承载唯一映射到原地址的状态，不从宿主FILE拷贝内部布局。
现有业务std::FILE消费者仍待整体迁移，不将两种对象混用，也不将新表空闲状态
算作当前业务资源已被清理。

### 存储、初始化与分配

原FILE的32B八字段依序为current指针、S32 remaining、base指针、W32 flags、
S32 descriptor、S32 characterBuffer、S32 bufferSize、临时文件名指针。
只拓宽三个指针，新布局偏移0/8/16/24/28/32/36/40、大小48，以静态断言约束。
原20个静态FILE中，首项current/base指向1086AE0的4096B虚拟零缓冲，
flags=101H、fd=0、bufferSize=1000H；第二/三项flags=2、fd=1/2，其他字皆0。
新端以真实字符数组及原生指针保留，不将stdin初值改成空或动态分配。

1086AC4为FILE**，1087AE0为S32表上限，108603C为W32累计计数，均虚拟零。
53E9A5按原顺序将上限0改512、小于20改20，调用准确声明53BFF8 calloc；
失败发布空、改上限20重试，仍失败走已实现错误26。随后逐槽重读共享表指针，
填入20个静态FILE；再直读底层fd0/1/2句柄，只有-1或0才把FILE.fd写-1。
53BFF8仍未实现，不以未经批准的宿主calloc或Malloc+清零伪装原分配失败语义。

53F848从0扫描一次采样的上限/表，遇空槽用已批准Malloc分配sizeof(FILE)，
API后通过重读的全局表发布并取回；失败返空。选中槽只依原顺序写fd=-1、
remaining=0、flags=0、base/current/tempName为空，不清characterBuffer或bufferSize。
544815先增加累计计数，Malloc(4096)并发布base；失败OR标志4、改借本FILE的
characterBuffer前2字节，成功OR标志8/容量4096；末尾count=0、current=base，
不加内存不足早退、补零或互斥清标志。

### 全流与单流清理

53BB99固定调用53BBA2(1)。后者每轮重读上限和表；活动条件flags&83H，
mode1按fflush非-1累计成功数，mode0只刷写流并累计错误-1，其他模式不刷新。
5446AD从3遍历，活动流fclose非-1才加计数；索引>=20的非空槽即使关闭失败
也经已批准Free释放，并在返回后重读全局表清槽。静态3..19不释放存储。

53BB02非空先flush，非0转-1；成功且commit位4000H才调用541222并将非0归一为-1。
53BB3D仅写方向且有108H缓冲标志、current-base的S32字节数>0才写；
短写置20H并返-1，写满且80H置位才清写位2；公共尾始终重读base、清count、重置current。
541279底层write保留准确声明、尚未实现，不以宿主_write操作另一张描述符表。
53F38F只释放活动且8位缓冲，返回后只清flags的8/400位及current/base/count。
53B1D0字符串流直接-1并清flags；其他活动流依序flush/freebuf/close，
关闭失败结果改-1，成功才释放并清临时文件名；最后清flags。不修复原失败顺序。

### 原生句柄表、commit与close

10869C0..1086AC0为64个组指针，每组32个原8B描述符；
HANDLE拓宽后每项16B，flags/管道暂存字节/未解释2字节保留，其他只作对齐填充。
组指针和1086AC0容量沿虚拟零合同；53F52D真实分配/继承生产仍未实现，
不借宿主描述符、不为无生产的表增加假条目或容错默认句柄。
原生Win32类型只在内部LegacyLowIo边界出现。

543897先U32 fd与容量比较、再读FOPEN；非法清DOS错误、写errno9并返HANDLE -1。
54381D要求有效/打开且句柄非-1；应用类型1且fd0/1/2才SetStdHandle(对应号,null)，
不检查返回；之后重读组指针将句柄改-1，保留flags。非法同样清DOS错误、errno9/-1。
541222验证失败只写errno9/-1、不清旧DOS错误；FlushFileBuffers失败后
GetLastError若为0仍返回0，非0才写DOS错误及errno9，不能改为通用错误映射。

53D30A有效流先get_osfhandle；fd1/2依序读2再读1，句柄相等则跳系统关闭。
其他有效句柄再次get后CloseHandle；失败保存GetLastError。
随后无论关闭结果均调用54381D，并重读组指针清flags；保存错误非0才调用现有
MapLegacyFileError_542F44并返-1，否则0。不预清成功路径的errno、不吞失效分支副作用。

本步接回退出模块原全流刷新/关闭两项声明；低层write、calloc、lowio生产、
fopen/openfile及业务FILE迁移仍未完成。只作静态核对，不构建/测试、读写文件、
关闭句柄、运行客户端或哈希校验，不宣称完整FILE或退出验收。

## 描述符生产、定位与写入实施 SPEC（2026-10-03）

本步实现53F52D/53F493/541279，接通既有LegacyLowIo表及FILE flush的write依赖；
不调用宿主_ioinit/_lseek/_write，不另建表。原calloc、打开/读取及业务FILE迁移仍待。

### 53F52D真实生产及继承边界

首次Malloc请求32*sizeof(LegacyDescriptorRecord)，失败走原错误27，然后发布第0组
及容量32；每项依序写flags=0、HANDLE=-1、pipeByte=LF，不清另外2字节或对齐填充。
初始化循环保留组指针重读，容量是W32，每组仍32项。GetStartupInfoA的本机结构
不预清，沿原只检查cbReserved2非零且lpReserved2非空，未增加块长度校验。

继承布局为S32原始数量N、N个W8 flags、N个原生HANDLE；句柄区起点必须先用未裁剪N
计算，再把实际继承数按S32裁至2048。按已有指针拓宽裁决仅HANDLE的4字节变8字节，
原标志区步长与数量不扩宽。对照本机UCRT lowio/ioinit.cpp的UNALIGNED intptr_t*
实际定义确认本机父进程格式；仅作为ABI资料，不采用宿主分组64/上限8192等行为。
此处面向新端原生继承块，不自动解码另一种32位父块；跨位宽继承仍未验收。
用定长字节复制读取未对齐N/HANDLE，保留原字节表示，不增类型检查或句柄转换表。

容量不足则从第1组分配，每次先增容量32再发布组、逐项初始化；
任一分配失败把实际继承数收缩到当前容量，不撤销已分配组。
继承句柄不等于-1且FOPEN置位才考虑；FPIPE直接接受，否则GetFileType非0才接受。
API后重新读取继承块的HANDLE/flags再写当前原组，保留其间共享内容读取顺序。

最后处理fd0/1/2：已有非-1句柄只补文本位80H；缺失则先flags=81H，
GetStdHandle后只以-1拒绝，否则GetFileType。失败补FDEV40H且保留原-1；
成功发布句柄，按类型低U8的2/3补FDEV/FPIPE。不能把句柄0提前视为无效，
不能跳过原GetFileType(0)调用。结尾SetHandleCount(当前容量)，原样返回U32。

### 53F493定位

先按U32描述符范围/FOPEN检查，无效DOS0/errno9/-1；通过后用原getter，
HANDLE=-1只写errno9。SetFilePointer仅S32低偏移、无高字输出，不预清LastError。
只有结果FFFFFFFF时GetLastError，非0映射错误并返-1；否则重读组清EOF位2，
按原W32位型返回位置。FFFFFFFF且错误0仍清EOF并返回其S32位型，不加现代大文件拒绝。

### 541279写入

描述符验证在count==0之前；非法即DOS0/errno9/-1。有效零请求0。
追加标志20H调用lseek(fd,0,2)且忽略失败，随后重读组/文本位80H。
二进制只作一次WriteFile；文本保留原Buffer从EBP-414H到-10H的1028字节区域，
按“写完本输入字节后输出长度是否小于1024”分块，LF先CR再LF，所以一块可到1025B。
原计数均W32，API的写出计数不预清，仅返回成功后读取；
短写判断是S32比较，不能改成U32或按输入字节固定1024切块。

累计物理写出字节与已生成CR数分开。任意累计非0即返回两者W32差，
包括后续块失败/短写所预先生成的CR；不钳制、不根据成功字节重新数CR。
总写出0且错误非0：错误5特设errno9/DOS5，其他走542F44；
错误0则重读FDEV，首源字节Ctrl-Z时0，否则errno28/DOS0/-1。
成功/部分写入不清旧errno，不回滚追加定位，不重试短写或撤销部分输出。

仅静态核原指令、分支、字节/参数宽度和新增差异；不执行实际初始化、
句柄继承、定位、写入、客户端、编译、测试或哈希校验。

## 低层文本读取、缓冲填充与读行实施 SPEC（2026-10-03）

本步实现541426/53F3BA/53C8B3/53B226，并补同组544705/53D3BD。
使用原描述符、FILE、缓冲和错误槽，不接宿主_read/fgetc/fgets；
原业务FILE迁移及打开/calloc链仍待，不将暂未迁移的调用者记作已接。

### 低层read与CR/LF边界

541426先U32范围/FOPEN校验，无效DOS0/errno9/-1；有效count0或低层EOF位2置位返0。
设备/管道48H且pipeByte不是LF时，先把暂存字节写到输出、请求量减1，
累计设1，再重读组将pipeByte复位LF。即使剩余请求0也继续原ReadFile。
主ReadFile失败：错误5特设errno9/DOS5，断管109直接0，其他走旧错误映射/-1。
不返回此前暂存的累计数、不回滚已写输出，也不恢复已消费的pipeByte。

主ReadFile成功后把API读数加累计；二进制直接返回W32位型。
文本位80H时，标志4只按本次API读数非0且输出首字节LF决定，不按累计是否非0。
随后原位压缩：内部CRLF写LF，其他CR保留；Ctrl-Z停止扫描，只有非设备才置EOF2。
设备Ctrl-Z同样不复制该字节。未消费的原缓冲尾不补零、不回写到系统文件。

末尾CR先推进源指针，再用同一个NumberOfBytesRead槽额外ReadFile一字节；
失败仅GetLastError非0才直接输出CR，错误0继续按原计数槽/局部字节处理。
不在peek前清计数或局部字节，不擅自给API未写内容补默认值；该特殊失败仍未验。
设备/管道：peek为LF输出LF，否则输出CR并把peek存回pipeByte。
普通文件：只有当前输出为空且peek LF才直接输出LF；其他先lseek(fd,-1,CUR)，
忽略定位返回，peek非LF才输出CR，peek LF本次不输出，留待下次读取。
保留API之后重读组/flags及原位源/目标先后，返回压缩后的S32字节数。

### filbuf、fgetc与fgets

原无效描述符回退58FB40的8字节已加载为FFFFFFFF/00/0A/0000，
映射到唯一原生LegacyDescriptorRecord，不能把回退flags猜成文本位80H。
53F3BA非活动或字符串流直接-1；写方向置FILE错误20H后-1。
其他先置读位1；无10CH缓冲标志才调用已有getbuf，有缓冲只把current重置base。
read结果先发布remaining，只有0/-1分别置EOF10H/错误20H并清remaining；
不扩大成所有负数都走同一失败门禁。

成功后，FILE flags&82H为0且底层flags&82H为82H时置FILE 2000H，
descriptor=-1使用上述真实回退记录。缓冲尺寸512且自有8、无400H才改4096，
不重新分配；之后按原序采样current、发布count-1、读U8、推进current。
53C8B3先S32减remaining，负才filbuf，否则取U8并推进，不提前判EOF位。

53B226保持原内联逐字节逻辑，不把原调用证据误写成fgetc调用。
count<=0不写目标且不读流；count=1只补首NUL。最多读count-1，保留LF；
第一次即EOF返回空且不写目标，已有字节后EOF才补NUL并返回原buffer。
既有402020/4021A0未写局部待批项不因本步自动补值。

### isatty与filelength

544705只检U32范围，不额外检FOPEN，返回低层FDEV的0/40H，不写错误槽。
53D3BD先检原范围/FOPEN；查询CUR失败-1直接结束，否则查询END，
仅位置不同才尝试恢复CUR，返回END查询值、不用恢复失败覆盖。
全部使用已实现原低W32 lseek，保留其EOF/错误槽副作用。

仅静态核原指令、回退字节、别名/先后和新增差异；不运行读取样例、
文件操作、编译、测试、客户端或哈希校验，不恢复暂停音频。

## 描述符与FILE打开链实施 SPEC（2026-10-03）

本步实现543711/5437A6/53D435/53D44C、5438D4/5463A3及
53F6D8/53B2D1/53B2F1，复用同一描述符/FILE表、读写/定位及错误槽。
1086178默认打开模式、1085FE8权限屏蔽、10862AC默认FILE标志三个W32
均为无文件后备区，沿虚拟零合同；保持可变，不因无直接写者改为常量。
路径、mode直接按原字节处理，不换CreateFileW或安全模式解析器。

### 描述符申请与句柄发布

543711按64组、每组32项找FOPEN未置项，只写HANDLE=-1并返回索引，不置FOPEN。
遇首空组以已批准Malloc分配原生32项，失败立即-1；成功先容量+32、发布组，
再按原序写flags0/handle-1/pipeByte LF，其他字节不清。无空位返回-1。
5437A6只要求U32范围及当前handle=-1，不检查FOPEN；应用类型1且fd0/1/2
先SetStdHandle(新handle)，不检查返回，随后重读组写handle并返0。
失败DOS0/errno9/-1。不能把FOPEN提前到申请或句柄发布内。

53D435固定share40H转53D44C。后者SECURITY_ATTRIBUTES用SDK原生尺寸，
仅指针字段/结构ABI适配，继承BOOL及原标志保持。oflag80H禁止继承并带10H低层位；
无binary8000H且显式text4000H或默认模式非8000H才带text80H。
访问低2位只接受0/1/2；share只接受10/20/30/40H，对应0/读共享/写共享/两者。
创建组合oflag&700H逐项保留：0/400→OPEN_EXISTING，100→OPEN_ALWAYS，
200/600→TRUNCATE_EXISTING，300→CREATE_ALWAYS，500/700→CREATE_NEW。
访问/share非法先errno22再DOS0；申请失败DOS0/errno24。

create100H时依据(~umask & pmode)&80H选择NORMAL/READONLY；
temporary40H加DELETE_ON_CLOSE及DELETE权限，short-lived1000H加TEMPORARY，
顺序提示20H优先于随机10H。CreateFileA失败按GetLastError映射；
GetFileType为0时先CloseHandle，再取GetLastError，不先保存类型查询错误。
这里GetFileType比较完整DWORD的2/3，不套初始化路径的低U8比较。

调用set_osfhnd后忽略返回，再发布含FOPEN的flags。非设备/管道、文本且读写位2
才检查文件末字节：seek(-1,END)若-1且DOS错误131则跳过，其余失败close后-1。
先清原临时字节，再read1；只有返回0且字节为Ctrl-Z才chsize(先前末位置)。
之后seek(0,BEGIN)，失败同样close/-1。只对非设备/管道且oflag8H设置追加20H。
原参数栈高字节仅在路径/共享参数最后消费后改作临时标志/字节；
新端使用对应U8局部，不截断或改写借用路径指针，不造额外全局状态。

### 长度调整与setmode

5463A3先范围/FOPEN检查，失败errno9；mode只接受8000/4000H，否则errno22。
两失败都不清DOS错误；只切换低层text80H并返回先前8000/4000H模式。

5438D4使用原4096B局部块。先查询CUR，再查询END，任一步-1立即返回且不新增恢复。
S32目标长度减END为正时，清局部4096B、setmode(binary)，按min(剩余,4096)写零；
写-1时仅DOS5转errno13，结果-1，仍按原序恢复模式并最终恢复原位置。
不增加零写重试上限、额外负长度门禁或异常RAII清理。
差值负时seek(目标,BEGIN)返回被忽略，随后getter/SetEndOfFile；
失败errno13并取系统错误到DOS槽；差值0不改长度。
后两路径及扩展路径最终都尝试恢复原位置，忽略恢复返回，返回原调整结果。

### FILE模式与打开入口

53F6D8先采样10862AC，首字符只接受r/w/a，对应原open flags0/301/109H及读/写标志。
后缀支持+、b/t、c/n、D、T、S/R，按各自原互斥/重复门禁累计；
未知或重复仅令解析标志0，下次循环仍先读下一字节，再停止并尝试打开，
不是整个mode判无效。+清访问位1并置2，同时FILE清读写方向并置80H；
c/n独占提交选项，S/R独占访问提示，b/t独占文本模式。
首前缀无效直接空，不新增errno；sopen固定pmode=1A4H，失败保留getstream的选中槽状态。
成功先累计计数+1，再依序flags/count/current/base/tempName/fd发布，不清charbuf/bufsize。

53B2D1先getstream，空直接返空；否则交原四参openfile。
53B2F1固定share40H转fsopen。67个原fopen引用仅作为消费范围证据，
当前业务std::FILE还未整体迁移；不将这些新本体等同已接所有调用者。

只静态核原指令、字段/分支/生命周期及新增差异；不创建或截断文件、
执行打开/写零/模式切换、构建、测试、客户端或哈希校验。calloc及完整启动仍待。

## 批量读写与缓冲输出实施 SPEC（2026-10-03）

按第8条工程委托，本步采用原FILE/描述符唯一状态继续补齐八个本体，不替换业务std::FILE。
目标为541620、53BCCD、53C680、53F8C0、53CFDC、53EA5E、53EAEB、53B356。
只对指针拓宽，尺寸/元素数/返回保持W32；不扩展calloc或CRT宿主替代批准范围。

### 原复制与批量传输

541620全指令及六组实际跳表已核：正向前缀表541684为541690/5416BC/5416E0；
正向W32表5416FC为0..7项，字节尾表541768为0..3项；
逆向前缀表54180C为541818/541838/541860，W32表541894按负索引至5418B0，
字节尾表541900为0..3项。目标在源之后且位于源区间内时逆向，否则正向。
按目标四字节对齐先处理前缀/高端字节，再逐W32读后写，最后逐字节处理尾部；
单W32用局部及std::memcpy表达无对齐/别名限制的读写，不整区调用库复制或提前快照。
指针比较仅边界转uintptr_t。正常域零长度无读写；不构造区间之前的C++指针。
这不是原网络53D710的替代或全局调用迁移。

fread/fwrite先计算U32总数，零立即返回且不读流。初始块长由10CH/缓冲尺寸或4096决定。
缓冲计数判断为非零，复制量按U32较小数；保持复制后对同一FILE字段的重读更新顺序。
fread直读0/-1分别置EOF10H/错误20H，保留短读字节；小块经filbuf，写目标后重读块长。
fwrite大块按108H决定先flush，写-1或U32短写置20H，返回已完成元素数；
小块源S8扩展给flsbuf，成功才推进，重读bufferSize后仅该路径按原S32<=0改块长1。
总数及元素返回规则保持，不新加溢出/负计数保护。

### 缓冲输出及临时缓冲

flsbuf入口先采样descriptor，要求82H且非字符串40H；读转写先清count，仅原EOF允许转换。
清EOF/置写位/清count后，无10CH缓冲时仅标准输出/错误且isatty非零跳过getbuf。
108H缓冲先算S32旧长度，发布current=base+1及count=size-1，再write或追加seek；
无论write失败或seek失败都重读base存入新低U8，之后才按原已写量判断返回/错误。
无缓冲一次写参数低字节。fputc先减count，有空间写低U8后重读current并推进。

stbuf先isatty，且只接受唯一static FILE的stdout/stderr；先增累计数，再检查10CH。
1086040/1086044是两个原缓存指针，无加载文件后备，沿既有映像虚拟零合同初始化为空。
各缓存按需Malloc4096，失败借本FILE字符槽2B；按原字段顺序发布，OR1102H并返回1。
缓存不由ftbuf释放。ftbuf即使参数0，只要仍有1000H也flush；参数非零才在flush后
清1100H、bufferSize/current/base，不额外清count，不以flush失败阻止拆除。
fputs沿既有strlen正常NUL字节串合同取U32长度，stbuf→fwrite(size1)→ftbuf，
只按fwrite数量决定0/-1；不把最终flush失败改作返回，也不加RAII。

原映像头不在IDA加载区，不能用未加载字节推导PE节表；本步缓存初值仅沿已记录的
.data无文件后备虚拟零合同，不把get_bytes返回的FFFFFFFF当作实际初值。

本步先完整实现上述依赖，再集中静态核对新增指令/分支/共享字段及元数据。
不运行样例、文件操作、构建或测试；calloc、定位/格式化剩余本体、业务迁移与启动仍待，
音频暂停，CRT宿主提案及待批局部补值保持。

## 文件定位与缓冲调整实施 SPEC（2026-10-03）

按第8条工程委托，实现53BC41 fseek、53CB78 ftell、53B27D rewind、
54ACF1 setvbuf、54463F ungetc五本体；复用唯一LegacyFile、低层描述符、
Malloc/Free及1085FE0/108603C，不创建宿主FILE副本。原位置/容量保持S32/U32，
仅游标地址拓宽。沿真实依赖完成该板块后集中静态核对，不执行定位/缓冲样例。

fseek先检查83H活动及origin0/1/2，失败只写errno22并返回-1。
成功先清EOF10H；CUR调用ftell后直接相加并改BEGIN，不以ftell=-1增加早退。
忽略flush返回，之后80H流清方向3；否则读流且自有8H、非用户尺寸400H才将size设512。
最后低层seek只有返回-1才映射失败，成功0；不让先前刷新结果覆盖它。

ftell入口采样fd，把负count清0；底层CUR结果S32<0直接-1。
无108H缓冲返回位置-count。有缓冲先采样current/base、计算S32已消费量；
方向3非零且底层文本80H时，按地址从base到current，每个LF给逻辑量加1；
无方向且无更新80H才errno22/-1。底层位置0直接返回修正后的已消费量。
读方向count0时只返回底层位置；否则算原未转换块长(current-base)+count。
文本流再seek END：结果恰等于先前位置时，重读base并扫描固定原块长中的LF，
再按FILE2000H补1；不把增长后的计数变成扫描上界。
非EOF分支恢复旧位置，忽略恢复结果；块长U32<=512、自有8H、非400H才按512，
其他按当前bufferSize，再重读原fd对应组，按低层4H补1。
最后位置减物理块长再加已消费量；不扩展为宿主64位文件位置或增加恢复保护。

rewind入口保存fd，忽略flush返回，清FILE30H；fd=-1使用已有可变58FB40回退项，
清低层2H；重读FILE，80H流清方向3，最后忽略seek(0,BEGIN)返回。
保持void调用合同，不另设定位失败IOERROR。

setvbuf mode4不检查size，其他只接受0/40H及U32 size2..7FFFFFFF；
无效直接-1，不访问FILE或写errno。容量取偶数，flush/freebuf后仅清flags低W16
中的3D0CH（保持高W16）。mode4借charbuf前2B并置4；用户buffer直接借用并置500H；
空buffer调用Malloc，成功OR408H，失败累计108603C后-1，不恢复已释放缓冲。
成功按count0→size→base→current顺序发布，不清缓冲内容或扩大所有权。

ungetc仅完整S32 -1早退且不读FILE；要求read或update且非write。
base空才getbuf；current==base时count非零失败，否则先把current加1。
字符串流先退游标，只有原字节等于输入低U8才成功，失配恢复刚退的游标；
普通流退游标后写低U8。随后先采样flags，再count+1，再清EOF/置read，返回U8；
保留缓冲与字段别名导致的重读顺序，不增加容量或字符编码门禁。

所有原直接入向单列，仅fseek内部ftell及低层真实依赖在本步接通；
GraphicInfo、Blacklist、输入日志、filebuf、格式输入与宽读取等父调用未迁移，
不把本体存在当业务接线、链接或运行通过。CRT宿主提案与calloc/完整启动仍待；
音频继续暂停，不构建、查产物、新增/执行测试或哈希校验。

## 64位文件定位与位置保存实施 SPEC（2026-10-03）

按第8条工程委托，补齐54B658 lseeki64、54B239 ftelli64、54B3C0 fseeki64、
54ACB7 fgetpos、54ACD9 fsetpos五本体，复用唯一低层表、FILE及错误槽。
原端本来即为S64文件位置；缓冲数量、已消费/物理块长仍保持W32，不因宿主x64扩大。
内部LegacyFilePosition用两个U32字段low/high表达原8B数值布局（偏移0/4、对齐4）；
参数是已证类型指针，不用宿主fpos_t，也不把位置误作地址。

lseeki64先U32 fd范围/FOPEN检查，失败DOS0/errno9/-1；保存组槽和索引。
拆原S64参数低/高W32，再get_osfhandle；handle=-1只errno9，不额外清DOS。
SetFilePointer沿原API传低有符号LONG及高LONG指针，不提前清LastError。
只有低返回FFFFFFFF才读GetLastError，非零映射失败；否则重读组清EOF2H，
按API输出高/低W32合回S64。原-1完整结果不提前拆成仅低半失败。

ftelli64保持与原完整144条指令一致的独立本体，不把32位ftell简单提升调用：
入口采样fd并清负count，CUR结果S64<0才-1；无108H缓冲时count先S32符号扩展再相减。
有缓冲的current-base、累计LF及物理块长均U32；位置0时已消费量零扩展为64位，
最终扣物理块长、加已消费量同样零扩展（原sbb/adc高半使用0）。
文本EOF/非EOF、固定原块长扫描、FILE2000H/低层4H、512B条件和忽略恢复失败
保持原顺序；API之后重读组/FILE。原TEST后JB不可能成立，仅高半负值判S64负。

fseeki64要求活动83H及origin0/1/2，失败只errno22。清EOF后CUR查询S64位置相加，
不新增查询失败早退；忽略flush返回，再更新方向或512B缓冲尺寸，最后lseeki64。
只有完整64位全1返回失败，其他位置成功0，不改既有32位fseek接口或数值域。

fgetpos调用tell后依序写low再high，即使失败仍发布两半FFFFFFFF；
随后重读位置两半，仅同时全1返-1，否则0。
fsetpos先采样high再low，按位组合S64，以BEGIN交真实fseeki64并原样返回。
不添加空指针检查、偏移钳制、文件长度预查询、回滚或额外状态。

本步完成五本体及内部链；窄filebuf的548B68/548BEA父调用尚未迁移，
不把内部链闭合当完整业务文件/启动或运行验收。此前32位定位和缓冲正文保持。
仅静态核新增证据/分支/数值宽度/别名/声明与元数据；不实际定位/文件操作、
构建、查产物、新增/执行测试或哈希校验。calloc、宽字符流、格式化、业务迁移
及完整初始化仍待，音频暂停，CRT宿主总提案及既有补值待批保持。

## 宽字符文件流实施 SPEC（2026-10-03）

按第8条工程委托，实现54AD92/54AF70/54AE87及54B450/54B574五本体。
字符/返回保持U16，FFFF为WEOF；缓冲计数为S32字节数，原W16读写以单次
std::memcpy局部表达未对齐访问，不逐字节改写原W16访问粒度。
复用现有窄filbuf/flsbuf/ungetc、低层读写/定位、getbuf及原mbtowc/wctomb/ctype表，
不借宿主宽FILE或现代Unicode流，不改变locale、错误槽或原代码页。

flswbuf保持窄版本对应门禁/时序，但新游标base+2、count=size-2、写入W16；
旧缓冲写失败或append seek失败仍先重读base存W16，再判返回。
无缓冲一次写2B，返回输入低W16或FFFF。filwbuf对read=0置EOF10H，
read=1或-1置错误20H，三者都清count返FFFF；成功count=read-2、先取W16再current+2。
保留FILE2000H同步、fd=-1回退项、512→4096条件，不重新分配或补第二字节。

fputwc字符串流/非文本直接减count2，足够时写W16后重读current加2，否则flswbuf。
文本先wctomb，-1设errno42/FFFF；结果1输出首字节，其他成功结果仍只输出前两字节，
每次减count，缓冲不足传S8给窄flsbuf，每次失败立即FFFF，成功返回原W16。
原参数槽复用改为独立4B转换槽；正常成功消费字节均由wctomb写入，失败不读取。
不按结果循环输出全部字节，不新增容量/编码门禁。

fgetwc文本流按原ctype表8000H判lead，只读取1或2字节；
第二字节EOF时把首字节按S8交ungetc，忽略回推结果后FFFF。
mbtowc=-1设errno42，其他返回目标W16。原目标为保存流指针参数的+2字节：
mbtowc存在非负且未写目标分支，故不能把目标改成未初始化或清零局部。
工程裁决：独立wchar_t输出槽初值取原生stream地址对象表示的第2/3字节
（uintptr_t右移16后取低W16），继续保持已证+2字节别名的数值初值；
原生stream指针完整保存并用于访问，不用截断值寻址，不猜原端地址常量。
该值仅在原转换未写目标路径可见，不将异常转换视为正常解码成功或运行已验。
原4B输入局部只写实际读到的1/2B，不补其余；非文本路径减count2后取W16，
不足交filwbuf，原失败不新增保护。

ungetwc仅低W16为FFFF直接失败，要求read或update且非write，base空才getbuf。
文本经wctomb，失败errno42；current==base且count0才按转换返回量先推进，
否则原非零count失败；转换量非1先回写第二字节，再回写第一字节，
count加转换返回量，之后取flags清EOF/置read并返回原W16。
字符串流及非文本不检查被回推字符相等：先保存flags，必要时current+2，
count+2、current-2、发布flags、写W16，最后重读current处W16返回。
这是宽/窄回推的真实差异，不抽公共实现改变字段别名顺序。

只在本体完成后集中核新增原指令、调用、状态及局部复用，业务宽filebuf父链
仍未迁移；不运行转换/读写样例、实际文件、构建、产物检查或测试、哈希校验。
音频暂停，calloc/格式化/完整初始化/业务迁移及CRT宿主提案等剩余保持。

## 格式化字节输入输出依赖实施 SPEC（2026-10-03）

按第8条工程委托，先补齐核心实际直接依赖53F2C6/53F2FB/53F32C和
53E950/53E96A/53E981六本体。沿既有LegacyStdio文件，不为少量包装另建模块；
只有原地址和职责命名，不添加私有FILE、计数全局或宿主格式化代理。

write_char与现有fputc的缓冲写/完整S32参数交flsbuf/低U8返回时序逐项相同，
本体复用WriteLegacyStreamByte_53CFDC，之后才读取/更新调用者S32计数指针：
返回完整-1则写-1，否则加1。即使计数入口已-1仍尝试写，不能提前退出。
write_multi_char仅S32数量>0才进入，每次write_char后检查计数恰-1，失败停止。
write_string同样只处理正数量，逐次先读取S8并推进源游标，再尝试写入；
源可与目标缓冲相叠，不改为批量memcpy、整串快照、strlen或预检查。

53E950逐字节读取与现有fgetc相同，复用ReadLegacyStreamByte_53C8B3，
保持count先减、U8返回、current取旧值后更新及filbuf失败返回。
53E96A只在完整S32字符非-1时调用真实ungetc，所有caller不消费返回，
采用void，不为EOF路径伪造原残留寄存器值。
53E981每次先增加调用者计数，再读字节，再用原53BDEF分类；
空白持续循环，返回首个非空白值，EOF也先计数并分类，不提前特判或回推。
计数可与FILE字段相叠，复用调用之前/之后的访问顺序保持。

输出核心八状态、浮点可变槽及输入扫描核心/原生变参ABI仍是后续完整任务；
本步仅完成六本体与实际窄FILE内部依赖，不声明公共格式化业务已迁移。
原get_int_arg同时承担数值和指针槽，后续必须在父核心按语义取原生参数，
不能先实现一个统一W32取值器截断地址，也不因此用宿主printf替代核心。

完成该依赖组后静态核对新增指令、S32/S8/U8、计数/游标/别名/失败次序、
源码来源和元数据。不开启构建、产物检查、样例/测试、客户端或哈希校验；
音频暂停与既有待批内容保持。

## 格式化输出核心实施 SPEC（2026-10-03）

按第8条工程委托，实现53EB28完整八状态输出及53B304/53D21D/53BC0F三包装，
以独立LegacyFormat.cpp/.hpp承载较完整解析模块，加入既有startup源码清单。
复用已实现字节helper、wctomb、Malloc/Free和错误退出；不引入外部格式库。
原89B分类/状态表、八项实际跳表、窄/宽null文本及三个浮点槽加载值已读取。

状态、flags、宽度/精度、计数/文本数量均W32；原生va_list仅负责x64参数位置。
宽度/精度按int取值；短字符/短整数按默认整型提升取值再读低W16，long保持Windows32位；
I64取S64/U64。字符串、Z记录和n目标按真实指针读取，不实现统一W32取槽器。
Z仅指针拓宽为原生+8字段，S16长度及+2未消费W16保留，size16；
宽Z将S16扩S32后按U32逻辑右移1。n仅h写W16、其余W32，不因I64写64位。

p作为地址显示边界，固定精度由原8改为sizeof(void*)*2（x64为16），
参数读取完整void*后按uintptr_t转无符号数值，保持大写十六进制/前缀/填充规则，
不受h等数值截短分支影响；这是已批准“地址不得截W32”的局部工程落地。
非地址数值宽度、整数精度上限512、符号、八进制#、零值抑制十六进制前缀保持。

普通lead字节先写自身，再取下一字节并写，不在两次之间新增失败早退。
每轮末先读取下个格式字节，下一轮才按负累计数退出。
进入%只复位原已写的字段，不清上个正文长度和正文指针；未识别类型沿原共用输出尾，
不按现代printf补分支。初始正文长度为0，原未消费正文指针以scratch地址承载，
不读取未初始化C++指针；后续正文指针保留前字段，包括原复用/生命周期行为。
无已写路径的局部内容不补零，普通解析无新增非法格式门禁。

窄/宽s/S和C/c沿原修饰门禁；宽字符转换失败仅抑制当前字段，宽字符串转换<=0
只停止正文，不主动将累计计数写-1。输出始终按左空格、符号/前缀、零填充、
正文、右空格的原序，不能在每段前新增失败早退；已写前缀不撤回。
正文宽字符串循环不会因字节helper写失败而提前停止剩余转换调用。
512B局部缓冲保留；数字反向写采用索引表示，避免仅为循环表达构造起点前指针。
八进制空正文的未消费越界读取不专项模拟，沿RPLAN.24只保留补0结果。

浮点参数为64位double对象表示，精度默认6/g0改1/上限512；
precision>163时Malloc(precision+349)，失败回退163，成功字段尾Free。
本步定义58FB50/54/5C三个唯一原生类型回调槽，初值经类型适配入口调用真实545031：
53DEAB(error2)后不返回，不引入固定成功、宿主浮点格式化或替代转换。
转换回调接(double*,char*,S32格式,S32精度,S32大写)，另外两个接char*，原caller弃返回。
读取回调槽的时点、#且精度0强制点、g非#裁零及负号拆离保持。
53B45F安装六槽和真实浮点转换器仍未实现，不把初始trap当浮点功能完成；
本步不提前安装或越过真实startup时序。

sprintf/snprintf只初始化原临时FILE四字段；保留未使用其余字段。
窄flsbuf的descriptor采样移至82H/40H拒绝之后：该拒绝路径不消费fd，
避免读取原临时FILE未初始化fd产生C++未定义值；正常路径仍在所有可变调用之前采样，
所有返回、标志写入及后续调用顺序不变。该差异仅惰性读取无用字段，不补未知fd值。
包装先保存核心结果再尝试尾NUL，恰满不强制截末字节，尾写失败不覆盖核心返回；
fprintf先stbuf，核心后ftbuf，返回核心结果，不增加RAII或宿主验证。

这使输出解析/包装本体具备实际实现；应用1022/9/11处调用未整体迁移，
浮点安装/转换、输入扫描、calloc/业务FILE/完整初始化仍待。
只做针对新增源码/表值/状态/ABI/参数流与元数据的静态核对；
不构建、执行格式样例/测试或哈希校验，音频及待批事项保持。

## 浮点输出分发与数字排版实施 SPEC（2026-10-03）

按第8条工程委托，沿真实依赖实现54019A、53FED3/53FFD7/5400B5、
540150/540177、53FDD5/53FE2F、5401EB/544EA0及53DC64共11本体。
浮点排版单列LegacyFloatFormat模块，tolower进入既有LegacyMultibyte；
不改宿主浮点环境、不用printf/to_chars替代原数字转换。

原数字结果记录+0符号S32、+4小数点位置S32、+8结果S32、+C数字串指针；
新LegacyFloatDigits只拓宽指针至+16，size24。1086048为唯一非拥有结果指针，
108604C复用模式U8、1086050指数S32、1086054进位U8均沿既有虚拟零；
58FE40原已加载2E000000的首U8作为可变小数点字节，不猜locale其他数据。
544F17结果生产依赖精确声明，真实dtold/I10_OUTPUT及96位数学依赖另步实现，
本步不补固定数字/成功，不把排版本体算完整可用浮点转换。

e正常路径先fltout(double)，按precision+1调用fptostr，目的区预留符号和小数点位置；
缓存路径使用同一1086048并按precision>0移位。随后写负号、移动首数字并写小数点，
复制原e+000含NUL，按uppercase改E，数字串非零才用decimal-1改指数符号及三位数。
尾指数目的偏移在shift/数字转换之后重新读取108604C，保留共享状态时序。
f正常路径按decimal+precision调用fptostr；缓存路径在先前指数恰等precision时
写额外0/NUL。保留非正decimal补整数0、小数点移位及负decimal前导零；
缓存路径不按precision裁前导零数量，非缓存路径才取两者较小值。

g先生产并发布结果指针和舍入前decimal-1，fptostr后重读全局结果记录，
记录进位及舍入后指数；指数<-4或>=precision走缓存e，其他缓存f。
进位且走f时删除当前数字串最后一字节；两个缓存包装置模式1、调用后清0，
不保存恢复入口旧值，不引入RAII。
分发只把e/E交e，f交f，其余交g；真实返回为目标char*，
58FB50回调类型据此改为char*返回，原核心仍弃返回，初始trap继续保持。

shift只在位移非0时strlen+1含NUL整体重叠搬移；fptostr先放首0，
正precision逐字节消费数字、源NUL后补0，再NUL；precision非负且下一源S8>='5'
才从尾向前处理9→0并进位。首位成1只增加同一record.decimal，
否则将首0后的字符串移回原起点。两次53D710调用使用已证正常重叠复制合同
以std::memmove承接，不把原网络两个消费者改为新接口。

forcepoint先原tolower首字节判e，否则前移再按共享MB_CUR_MAX/ctype数字分类推进；
不把负号单独特判，不预查NUL。插入共享小数点后按原逐字节交换搬移至NUL。
cropzeros仅首个共享小数点之后找e/E/NUL，倒退删除0和可能的小数点，再逐字节
前移尾部（含NUL）。不调用新字符串库优化改变别名读取时点。
tolower沿原C-locale ASCII快速分支；非C按大写位及共享NLS映射，保留S32比较、
高字节lead构造、API失败原值及两字节返回组合，不借宿主tolower。

本步不安装53B45F的六槽、不伪装缺失的输入浮点写回/positive/数字生产依赖已完成；
原输出三个回调初值仍trap。只核新增指令/数据/字段/调用、源码和元数据，
不构建、样例/测试、文件操作或哈希校验；音频暂停及待批事项保持。

本步将既有541955从匿名命名空间移至cg::platform并在内部头文件声明，
只改变链接可见性以供forcepoint复用，原函数正文与其他分类/NLS调用保持。

## 扩展浮点96位运算实施 SPEC（2026-10-03）

按第8条工程委托，新增内部LegacyFloatNumeric模块，实现546D95 addl、
546DB6 add_12、546E14/546E42移位、547971 ld12mul、547B91 multtenpow12
及544F7B dtold七本体；这是数字生产链的真实运算依赖，不以宿主浮点算术替代。
LegacyFloat96固定12B原字节区，LegacyFloat80固定10B；用显式W16/W32加载存储
保持未对齐字段和交叠粒度，局部读取工具在多次真实字段访问间复用。
地址为原生类型指针，数值仍原W16/W32，进位/借位是正常多字精度算法，不是额外故障模拟。

addl先算W32和，以结果小于任一加数判carry，最后写目标并返0/1。
add_12保持低字相加、给中间字加1并必要时增高字、中间字再加源中间字、
必要时增高字，最后高字相加并返回最后carry；源在各步才读取，
即使src==dst也不能改为先快照全部三个源字。
左移先读低/中、写低、再读高、写中/高；右移先读高/中、写中、
再读低并写高/低，保持原返回目标地址。

ld12mul先采样双方符号/指数，局部12B积清零。按原W16指数门禁处理饱和、
过小及零值分支；左零只清指数，右零/过小按+8/+4/+0次序清三字。
一般乘法按五轮、每轮递减项数的W16相乘，对交叠W32积位置用addl，
carry只给对应下一W16加1，不改为“完整160位乘积后截取”的另一算法。
规范化与次正规处理保留低W16有符号比较、逐位移位及丢弃低位的粘滞位。
舍入条件为低W16>8000H或低17位==18000H，按+2/+6 W32及+10 W16逐级进位。
最后写目标+0 W16、+2 W32、+6 W32和+10符号指数；饱和写原无穷表示，
不增加NaN/零乘无穷等宿主规则或拒绝门禁。

multtenpow12指数0直接返回且不清低W16；负数选负幂表，之后按每次三位取八进制位，
逐组7个条目选因子。两个表各29条12B已从原加载区读取：
5925D8..592734及592738..592894；正常原扩展指数范围所需条目完整。
因子低W16>=8000H时只复制该12B到局部并将+2 W32减1，再调真实ld12mul；
其他直接借只读表项。preserveExtra=0时先清输入最低W16，不改变其余数值字段。
用组索引表达原table-0CH基址计算，不构造表前指针，不新增指数范围钳制。

dtold读取double原表示的+6 W16、+4/+0 W32，提取符号和11位指数。
正常指数加3C00H、全1指数转7FFFH，尾数移11位并补显式整数位。
次正规按原移位规范化并逐次减指数；正/负零都写原全零10B结果，不保留负零符号。
保留先写高/低尾数、规范化低/高更新及最后符号指数写序；
不调用宿主long double转换，不假定Windows long double有80位精度。
原返回残留寄存器不被唯一fltout caller消费，使用void。

数字生成546F36/544F17及六槽安装仍待下一链路收口；不得将新运算模块
或幂表存在当浮点输出已可用。仅静态核原指令、全部表字节、布局、进位/别名和
新增源码/元数据，不构建、不执行数值样例或测试/产物检查/哈希校验，音频及待批保持。

## 十进制数字生成与结果发布实施 SPEC（2026-10-03）

按第8条工程委托，实现546F36 I10_OUTPUT与544F17 fltout，接通既有e/f/g
到dtold、96位缩放/乘法/移位/加法及数字生成的真实依赖。六槽安装仍另行处理。

LegacyFloatDecimalText固定26B：+0 S16小数点位置、+2 S8符号、+3 U8数字数、
+4连续22B字符区；容量来自原上限21有效数字加1个舍入位/最终NUL的实际访问，
不把之后到10862D0的未消费间隔解释为额外字符。原80位参数按值传递，
不用宿主long double ABI；复制的10B是有效数值，原栈额外2B不被原核心消费。
输出字符区不整块清零，保留本次终止字节后的历史内容。

I10_OUTPUT先按符号位写'-'或空格；精确零、舍入归零及指定模式非正有效位数
统一按原序写decimal0/sign空格/count1/"0\0"并返1。
7FFF指数分支按尾数高位/低字及符号区分1#SNAN、1#IND、1#INF、1#QNAN；
四字符串已从原加载区读取含NUL的完整字节，decimal1/count5或6、返回0保持，
不替换成宿主nan/inf文本，不新增特殊值规则。

有限值按原整数常数4D10H/4DH/134312F4H估算十进制指数，
构造12B扩展值并调multtenpow12(-低S16估算值,preserveExtra1)。
规范化指数>=3FFFH时估算值+1，再乘原局部0.1的12B常量
CCCCCCCCCCCCCCCCCCCCFB3F（与幂表中的舍入因子不同，不合并）。
按模式位1决定precision是否加S16指数，原上限21保持，不新增下限门禁。
随后清扩展指数、左移8次；负二进制偏移按取负后的低U8右移次数。
每位先复制当前96位数、左移两次、加保存值、再左移，读取最高字节为十进制位，
清该字节后写出；共有效数字数+1次，保留最后一位用于原舍入。

舍入位以S8与'5'比较，>=5从尾向前9→0并进位，跨首位则增加S16指数；
其他路径删尾0，全删则回原零输出。数字数按原地址差的低U8等价偏移计算，
写回count后以S8扩展定位NUL。用完整输出对象字节偏移保持头字段和数字区的
已证访问关系，不仅把字符区视为独立数组，也不增加负精度保护。

fltout按原顺序dtold→I10_OUTPUT(17,0,唯一10862B0记录)，然后依序发布
结果W32到10862D8、符号S8扩S32到10862D0、指数S16扩S32到10862D4、
唯一digits原生指针到10862DC，返回同一LegacyFloatDigits记录。
新记录只拓宽地址，复用/覆盖生命周期保持；不创建每次分配的结果或私有缓存。

本步只实现字节算法和原特殊值分支，未运行验证x87异常状态、信令NaN传送等行为，
不以静态核对宣称浮点异常环境等价。原三个输出回调仍为trap，未提前安装部分槽。
完整安装、输入扫描、calloc、业务迁移/初始化仍待；不构建、运行样例/测试、
检查产物、文件操作或哈希校验，音频暂停与既有待批事项保持。

## 扩展数到32/64位浮点表示实施 SPEC（2026-10-03）

按第8条工程委托，先实现输入浮点链的544BB9 ld12cvt、544D25/544D3B两个包装，
以及5449BF/544A08/544A5E/544AEA/544B05/544B11/544B2C七个尾数工具。
继续放在LegacyFloatNumeric内部模块，不引入宿主long double或浮点强转，
不提前安装缺少文本解析依赖的回调。

原转换尾数局部区按高/中/低W32顺序存放，与原低字节在前的LegacyFloat96不同。
新增LegacyFloatMantissa96固定12B字节区表达该顺序，未对齐W32读写复用现有工具；
拷贝保持三次先读后写，不整块快照；清零从高字向低字写，零检测遇首非零即返回。
544A08按位号所在W32加单bit，再逐字向高位传播真实addl进位并返回最终carry。
所有位号/移位距离保留S32除32、余32；W32移位按原指令低5位计数，
特别是移位距离0或整32时不能在C++中直接执行移位32。

5449BF按指定位置以下的掩码和之后两字检查尾部全零，返回1/0。
544A5E严格保持原门禁：先看precision对应位；置位且ZeroTail(precision+1)返回0，
才调用IncMan(precision-1)，然后与原全1左移掩码相与、清后续W32。
该门禁与保留掩码均按原指令，不替换为通常的宿主舍入或另加最低保留位判断。
ShrMan先高到低传播字内移位丢出的位，再低到高处理整W32位移/前部补零；
不将它替换成相反读取次序或宿主96位计算。

转换参数固定6个S32，共24B：上界、最小指数门限、精度、指数位数、
目标位数、偏置。原5921C0为(1024,-1023,53,11,64,1023)，
5921D8为(128,-127,24,8,32,127)，两段加载字节已逐字段取证。
本步只表达这两段已消费的只读参数，不据5921EC的其他索引读取推测相邻整表；
后续对应消费者仍需按其真实范围接线。

ld12cvt先完整采样输入符号/指数、+6与+2的W32及低W16左移16的尾数。
零指数且尾数全零返回0；非零则清尾数返回2，符号仍进入最终表示。
其他先保存原尾数、舍入并据carry加指数，再按参数区分清零、次正规、
上界和正常路径；次正规还原原尾数后移位/舍入/再移位并返2，
上界清尾数/置首位/移位、组合指数并返1，正常清显式首位并返0。
统一以原字段拼接W32表示；64位目标先写高W32再写低W32，
32位仅写一个W32，其他目标位数不写输出；不新增NaN/Inf专门转换规则。

两个包装传入各自原参数并返回核心状态。文本解析546419及544D51/544D7E、
输入赋值/六槽安装、完整启动另行接通。本步静态核对，不构建、运行样例/测试、
查产物或哈希校验；不宣称宿主浮点异常环境、完整输入或全PLAN已验收。

## 浮点文本输入与六槽安装实施 SPEC（2026-10-03）

按第8条工程委托，实现546419 strgtold12、546E6F mtold12、544D51/544D7E
文本转换包装、53FE95输入赋值、53FE7D非负判断及53B45F六槽安装本体。
数值累加放在LegacyFloatNumeric，文本/共享小数点及安装放在LegacyFloatFormat，
六个唯一回调槽继续由LegacyFormat持有。先补齐全部槽目标，再提供原序安装函数，
不在全局初始化或无证据入口自动调用；53B447浮点环境初始化父仍后续接线。

strgtold12按5468BA处12项跳表实现原状态转换。状态数值不变，
首部只跳空格/TAB/LF/CR；ASCII起始1..9与后续共享ctype数字分类保持区别。
每次分类重读MB_CUR_MAX及可变ctype/NLS，不缓存locale，也不使用宿主解析器。
小数点读取唯一58FE40字节；D/E/d/e均可引入指数，参数控制无标记的正负指数。
原始输入与指数起点分别保留为原生指针；失败按状态回退，结束指针先于数值输出发布，
不把所有失败统一回退或强制拒绝尾随文本。

数字缓冲为原25个U8，不添加终止符或预清零。整数超出25位时仅增十进制调整，
小数超出25位后不再减调整；指数先累计到超过5200时置5201，然后继续消费数字。
暂存25位时原5467A8/5467AE（807DBB05/FE45BB）实际访问缓冲+23的S8比较/U8递增，
之后仅保留24位、调整+1；不改为检查+24、不另加十进制进位。
再去尾0并调用mtold12；无显式指数才加默认尺度，无小数点才减默认小数位数。
综合指数大于5200/小于-5200时输出原饱和/零和2/1状态，未见有效数字返回4；
其他缩放产生的范围结果不额外改返回码。最后按+6 W32、+2 W32、+10 W16、
+0 W16顺序写输出，包含失败/零值的原符号。超界分支未消费的指针临时寄存器值
不转成数值字段；其真实输出均随后被已证饱和或清零覆盖。

mtold12先顺序清输出三字，每个原数字先做96位乘十，再读该S8数字到低W32相加；
保留负字节的符号扩展，不替换成ASCII转换。规范化先逐16位、再逐bit左移，
原W32指数累加FFF0/FFFF并最终写低W16；不增加count0/全零输入的返回保护。
该父链只在已有数字且去尾0后调用，不宣称异常输入已安全处理。

两个文本包装均先strgtold12(后四参数全0)，忽略解析状态，再执行对应
二进制表示转换并返回后者状态。输入赋值忽略包装返回，double局部先复制低W32
再高W32到目标，float局部复制W32；原复用文本参数槽的数值写入独立为float局部，
已证全W32覆盖，保持原生source指针完整，不补初值。

53FE7D与原加载8B零比较，非负（含正负零）返1，负值/无序返0。
本步用相应C++比较表达数值结果，未模拟或验证x87状态字/信令NaN异常传送。
六槽原加载初值均545031，新增输入赋值/非负/格式别名槽仍由精确类型适配调用
同一错误出口；安装按58FB54、58FB50、58FB58、58FB5C、58FB60、58FB64次序，
最后格式别名与主格式槽指向同一已实现转换器。原安装返回未被唯一父消费。

本步只做新增源码、参数/字节/跳表、共享槽和文档静态核对，不构建、运行测试/
数值样例、查产物或哈希校验。六槽安装本体齐备不代表真实启动已调用；
输入扫描核心、业务迁移和浮点环境完整验收仍待，音频及既有待批保持。

## 格式化输入核心与字符串包装实施 SPEC（2026-10-03）

按第8条工程委托，实现53DEF4完整输入核心（906条）与53CADC字符串包装；
以LegacyScan.cpp承载扫描器，继续使用LegacyFormat.hpp/LegacyStdio及既有
ctype、宽字符转换、浮点赋值槽，不引入宿主scanf或新依赖。版本解析4659F0
改接真实字符串包装；其余21个原调用点不在本步批量迁移，暂停/排除范围不动。

读取计数为S32，成功赋值数为S32，已完成字段标记为原U8；修饰字节保留
h/l/L/w、F/N、重复*及I64的原增减/符号扩展，宽度按S32累计。
输入空白、格式空白、普通字节/lead后续字节、格式结束后的EOF/%n例外，
按原读取/回推/计数次序实现；匹配lead双字节成功仅计一个字符，
失败先回推第二字节再第一字节。宽度为0仍在预读后回推，不新增提前拒绝。

原全部可赋值参数均为目标地址；在本项目Win64可变参数ABI边界用原生va_list
提取void*，后续用字节写入对应原数值宽度。不复制x86参数槽。
未知转换（含%%）按原字面匹配，原预取后回滚但不消费的地址槽无可观察用途，
新端不取该无用槽；被抑制的字符串用U32已接受单位计数代替未初始化地址的
算术比较，保留零/非零成功判定，不产生私有输出或补初值。
字符/字符串目标保持原生字节指针，宽输出每单位写W16。

整数保留i/x前缀探测与回推、p独立原路径、符号吃完宽度时的停止门禁、
数字分类及hextodec再减30H、正常W32/W64算术、宽度耗尽不额外读/回推等规则。
I64只在原显式修饰时启用数值64位分支；固定3/4位左移及乘10直接用U64表达，
仅覆盖原两移位和一乘法调用域，不宣称实现通用__allshl/__allmul。
n不增加赋值数，按原length0写W16/其他写W32；I64n原64位局部初清0，
仍写该0而不擅自改为字符计数。

地址适配仅针对p：普通p沿原独立十六进制解析路径，以uintptr_t累积和
写入完整void*表示，避免把输出格式的完整指针重新截断；不因此扩大普通整数，
不补0x前缀支持。I64p保留原I64分支的十进制判定，不擅改异常组合的进制规则。
该工程决策不改变数值格式/I64n或其他字段，也不运行地址样例。

c/s/[沿原32B位集；两固定源分别为58F8B0的"]"及58F8A8的空格、TAB-CR、"]"，
按实际加载字节构造；正反集、首位]、倒序范围及范围后重置前字符保持。
不新增遇NUL就退出的缺失]保护。宽输出按原lead字节多读一字节，再调mbtowc，
返回被忽略、DstCh跨字符/字段复用，源字节只按原位置写入。
原C在首%n/空格式等未读取域、DstCh首次转换未写域的初值未定义；
本步保持未初始化，不猜0、不宣称这些未写值或故障现场等价通过。

浮点文本缓冲按原局部352B，宽度上限349、负号保留/正号省略、
小数点读取后重读共享字节、仅e/E指数、各次宽度递减及读取次序保持；
完成后回推尾字节，再按原长度修饰调用唯一58FB58槽，抑制赋值不调用槽。
不自动安装浮点回调，继续由真实启动链负责。

53CADC仅写临时FILE的flags49/base/current及strlen低W32计数，再调用核心，
不预清其他字段或修改源文本；已实现filbuf的字符串拒绝先于fd读取。
本步静态核对新增状态/别名/宽度/字节及有限业务接线，不构建、执行样例/测试、
查产物或哈希校验；完整输入运行、启动环境及其余调用迁移仍未验。

## 字符串扫描业务接线实施 SPEC（2026-10-03）

按第8条工程委托，把既有ScanLegacyString_53CADC接入465F00命令选项、
507C80 IP选项、50CE10网络消息字段及50D3C0战斗字段，不改变业务解析结构。
本次只替换调用目标并引入内部声明，不重做已确认的关键字、资源名、共享布局、
局部初值、循环/调用顺序或错误后处理。

本次覆盖20个原调用点：465F00有16处（五个字符串、七个数字、
两个坐标别名、窗口位置和渲染设备），507C80有1处，50CE10有1处，
50D3C0有2处。现有代码把两处坐标别名扫描合并在既有双表循环中，
因此源码实际替换19处调用；不把语句数与原调用点数混为同一统计。
所有格式均从原加载区读取含NUL字节并与现有源码核对：
%s、%d、*%d、%d,%d、%d:、@FIND_LV1@SE:%d，以及六项/两项%X竖线序列。

命令选项继续保留原不检查扫描返回的行为，value、winpos局部等未写值
不补初值、不增加格式校验；newest参数仍写原gNewestText，只迁移扫描入口，
不恢复已排除的newest网络/文件处理链。IP仍把解析返回用于越界出口，
保留原index的参数槽表示初值、地址/端口字段写序及已有边界。
网络消息的SE字段仍按既有格式解析后进入现有后处理，音效调用本身不修改；
战斗P/A分别保留六/两W32目标、忽略返回和后续标志更新顺序。

上一轮版本4659F0已接1处，本步后原22个sscanf入向中21处有本线接线，
余下524060的BGM配置扫描属于暂停音频，本轮不改、不算排除或完成。
不依据调用接线宣称未复核的整段父本体或完整网络/启动已验收；
本步函数映射只更新既有记录的实际接线及调用点证据，不新增整段父函数完成项。

本步静态核对替换差异、目标/格式/返回消费、原调用点及格式字节；
不执行扫描样例、构建、测试、客户端或哈希校验。音频暂停、完整启动/
运行验收及此前未初始化值/待批内容保持。

## 启动资源与商店文本输出接线实施 SPEC（2026-10-03）

按第8条工程委托，把命令选项的8处资源/保存路径输出、532E70存档子目录
的1处输出，以及4B8310/4B8780/4B8C90/4C17B0/4C3A50的5处价格输出，
共14个原sprintf调用点接入FormatLegacyString_53B304。
同时将原全部9个53D21D有界标题调用接入FormatLegacyBoundedString_53D21D：
48B560一处、4B8310/4B8780/4B8C90各一处、4C17B0/4C3A50各两处、
4DA400一处。其余sprintf及流输出不在本步批量替换。

只改变调用目标及必要内部声明引用，不改变格式、可变参数、目标、容量、
分支、重读共享项、返回消费或后续界面处理。命令资源格式表沿已有已确认
合同保留，value未写时不补值；存档目录仍忽略格式化/目录API结果并返回1，
本步不执行目录操作。价格仍为原%10d和各共享11B槽，不增加容量参数。

九处有界调用保留原容量256/100/28，格式分别%s(%d)、%s(%s%d)、%s(%s)。
原格式与588370/588358两段中文提示均从加载区核对完整含NUL字节，
新端继续使用原字节字面值，不重编码、不更换措辞。
保持原未预清的局部缓冲、容量耗尽时不补NUL、后续strlen/分行/提示调用，
不把本次接线当作截断/未终止异常已经安全处理。

本步共23处调用：14无界、9有界。只更新既有包装和相关已有父记录的接线，
不凭调用点迁移宣布未重新核对的整段UI本体或全部1022个sprintf引用已完成。
有界9点接线齐备仍不等于完整运行/视觉/异常验收通过。

静态核对原目标/参数/容量、格式字节、限定替换差异、元数据和进度；
不构建、运行格式化样例/测试/客户端、执行文件操作或哈希校验。
浮点环境启动、其他输出/流调用、calloc、音频暂停及既有待批保持。

## 聊天历史文件完整读写接线实施 SPEC（2026-10-03）

按第8条工程委托，在Input.cpp完整迁移44A090加载/创建与449FA0原位更新两入口，
沿已有53B304/53B2F1/53BCCD/53C680/53BC41/53B1D0真实本体。
两个父共59/72条、1/1直接入向，原17个调用指令对应当前15个源码调用表达式；
既有源码把读取成功/失败关闭、尾写成功/失败关闭合并，各运行分支仍只关一次，
本步不为对应静态地址而重复关闭或拆出多余逻辑。

| 原父 | 原调用点 | 真实目标 |
|---|---|---|
| 44A090 | 44A0B5 | FormatLegacyString_53B304 |
| 44A090 | 44A0C4、44A0DC | OpenLegacyStream_53B2F1 |
| 44A090 | 44A126 | ReadLegacyStream_53BCCD |
| 44A090 | 44A0F7 | WriteLegacyStream_53C680 |
| 44A090 | 44A0FD、44A134、44A146 | CloseLegacyStream_53B1D0 |
| 449FA0 | 449FBB | FormatLegacyString_53B304 |
| 449FA0 | 449FCA | OpenLegacyStream_53B2F1 |
| 449FA0 | 449FF3、44A03D | SeekLegacyStream_53BC41 |
| 449FA0 | 44A015、44A04F | WriteLegacyStream_53C680 |
| 449FA0 | 44A023、44A05D、44A06F | CloseLegacyStream_53B1D0 |

三个局部流使用LegacyFile*，内部平台头只在cpp引用，公开输入接口/日志流owner不改。
两次格式化仍用未初始化128B局部path、共享可写gDataPath及%s\%s，
chathis.dat、rb/wb/r+b模式按原含NUL字节，不加预清、容量保护或转码。
历史文件块仍为单个4844H字节对象：64×121H原始记录加W32尾字段；
独立62683C游标不并入文件，加载前不清块，短读已写字节不撤回。

44A090先写游标-1，rb成功才读(4844H,1)并关闭，完整读取项数按原
44A12E/44A132的cmp/jnb >=1判断。现有源码==1在迁移时改为原条件，
返回仍S32的0/1；关闭结果不改变读取结果。rb失败才尝试wb，
成功写(4844H,1)且忽略结果后关闭；无论创建成功与否都写游标63并返回0。
原返回项数为W32，两个count/written局部改用uint32_t，不扩大为size_t。

449FA0保留r+b打开失败返回0；seek(index*121H,BEGIN)结果忽略，
随后写(121H,1)，不足1先关闭再返回0。首次写成功才seek(4840H,BEGIN)，
写入参数index本身4B而不是读取共享尾字段，关闭后按项数>=1返回0/1。
两个seek保持低W32偏移及origin0；不增加索引/失败保护，不回滚已有文件字节。

其余输入记录、回车提交、翻页、日志初始化/写入和既有待批局部初值不改；
不将本步混入音频或newest范围。共享文件块与gDataPath生产/消费者保持既有唯一owner。
真实CRT表初始化、模式3及完整启动仍未闭合，本步不自动初始化或执行文件操作。
只静态核本板块完整原指令/入向、路径/模式字节、传输尺寸和返回/关闭分支、
限定源码差异和进度；不构建、链接、测试、运行样例/客户端、查产物或校验哈希。

## 邮件、个人资料与分类文件接线实施 SPEC（2026-10-03）

按第8条工程委托，完整迁移MailState.cpp的七个文件入口：
5348A0/534910分类写入/读取，534770/5347F0个人资料写入/读取，
518CB0普通及公会邮件载入，519300/519550两类邮件保存。
局部流全部使用LegacyFile*，LegacyFormat/LegacyStdio仅在cpp引用；
32个原CRT调用全部接既有真实53B304/53B2F1/53BCCD/53C680/53B1D0。
七父共272条指令、39直接入向；不以这些局部计数推算总体完成。

| 原父范围 | 指令/入向 | 原文件/格式调用点 |
|---|---:|---|
| 5348A0..534905 | 31/1 | 5348B6→53B304、5348C5→53B2F1、5348E0→53C680、5348E6→53B1D0 |
| 534910..534975 | 31/1 | 534926→53B304、534935→53B2F1、534950→53BCCD、534956→53B1D0 |
| 534770..5347EB | 36/2 | 53479C→53B304、5347AB→53B2F1、5347C6→53C680、5347CC→53B1D0 |
| 5347F0..534891 | 47/1 | 53481C→53B304、53482B→53B2F1、534847→53BCCD、53484D→53B1D0 |
| 518CB0..518D94 | 65/1 | 518CDC/518D47→53B304、518CEB/518D56→53B2F1、518D13/518D7E→53BCCD、518D19/518D84→53B1D0 |
| 519300..51936C | 31/17 | 51932C→53B304、51933B→53B2F1、519356→53C680、51935C→53B1D0 |
| 519550..5195BC | 31/16 | 51957C→53B304、51958B→53B2F1、5195A6→53C680、5195AC→53B1D0 |

5348A0/534910以未清400H局部path格式化bin\%s/category.dat，wb+/rb+失败返0；
打开成功读写单个13ED4H分类区，忽略读写和关闭结果后返1。
534770/5347F0同用未清400H path、%s\%s/profile.dat，每次目录取自
105C394→105C36C映射→1058698路径，使用已有ReadSelectedSavedDirectory同源入口。
读写单个1050H资料区（两份828H记录），wb+/rb+失败返0；
成功忽略读写/关闭结果返1。5347F0在关闭之后重新读取当前角色S16，
再复制该角色828H持久记录到10738E8当前资料；短读仍执行复制，不预清资料区。

518CB0使用未清80H path，先重读当前保存目录格式化mail.dat/rb。
普通邮件打开失败调用519300并立即返回其结果，不继续打开公会邮件。
成功读取(1,5DC00H)并关闭，两结果忽略；随后重新取保存目录再格式化guildmail.dat/rb。
公会邮件打开失败调用519550并返回其结果；成功读取(1,139480H)，返回关闭的S32结果。
519300/519550分别使用80H path和wb，打开失败沿原EAX为0返回0；
打开成功写(1,5DC00H)/(1,139480H)且忽略写结果，直接返回关闭结果。
不得改成固定成功、不因短读/短写新增拒绝、补写、重试或回滚。

分类13ED4H、资料1050H、普通邮件5DC00H和公会邮件139480H现有唯一存储均不含指针；
既有布局断言与原传输常量相符。传输size/count在调用边界显式转uint32_t，
只拓宽局部文件指针，不拓宽原数值字段、不改变原字节文件格式。
不改清空/接收/赠送/排序/网络发布等生产与消费正文，也不预清加载目标或路径。

实际含NUL字节：58F630=62696e5c257300（bin\%s），
58F638=63617465676f72792e64617400（category.dat），
58F624=70726f66696c652e64617400（profile.dat），
58B6C4=6d61696c2e64617400（mail.dat），58B6B4=6775696c646d61696c2e64617400（guildmail.dat），
553514=25735c257300（%s\%s），5884F0=77622b00（wb+），5531A8=72622b00（rb+），
554920=726200（rb），55491C=776200（wb）。兼容文本不转码，不替换无界格式化。

只静态核本板块完整指令/入向、原字节、传输宽度、两级创建短路、复制/关闭次序、
限定源码/映射差异和唯一进度；保留真实CRT初始化、模式3及文件内容/运行验收未完成。
不恢复音频，不修改既有待批或范围排除，不新增启动调用。
不构建、链接、查产物、新增/执行测试、读写邮件样例/客户端或校验哈希。

## 日志写入及现代查看器文件整链接线实施 SPEC（2026-10-03）

按第8条工程委托，本线程采用聊天共享写入流、系统/协议局部日志流和
现代窗口66独立读取流统一LegacyFile的方案；删除已可由真实原FILE槽
承接的UI私有包装。沿完整生产/写入/读取/定位/flush/关闭接线，
不处理旧402020/4021A0待批局部初值，不修改音频、协议与其他UI正文。

| 原父范围 | 完整指令/直接入向 | 原文件/格式调用点 |
|---|---:|---|
| 449DA0..449F9C | 154/1 | 两53B304、三53B2F1、六53BC0F、一53B1D0、一53BB02 |
| 44B050..44B170 | 85/56 | 44B0FB→53BC0F、44B107→53B1D0、44B116→53B2F1 |
| 44B5A0..44B5BE | 10/1 | 44B5AA→53B2F1、44B5B7→53B1D0 |
| 44B5C0..44B5F4 | 18/31 | 44B5CB→53B2F1、44B5E4→53BC0F、44B5EA→53B1D0 |
| 5173B0..517474 | 75/1 | 5173DC→53B2F1、5173F1→53BC0F、5173F7→53B1D0 |
| 517480..517502 | 49/84 | 517499→53B2F1、5174AE→53BC0F、5174B4→53B1D0 |
| 4CBD30..4CBD8D | 26/1 | 4CBD84→53B304 |
| 4CC540..4CC69A | 94/3 | 4CC550/4CC5D6→53B1D0、4CC57E/4CC59A→53B304、4CC5A9/4CC5E5→53B2F1、4CC643→53B226 |
| 4CC340..4CC538 | 166/1 | 4CC3C5→53BC41、4CC3E0→53B226 |
| 4CBDB0..4CC324 | 420/1 | 4CC046/4CC1E5→53B1D0 |

十父完整1097条、180直接引用与所有调用已取证；39个原文件/格式点
对应38源码表达式，日期双分支已合并的一处关闭保持各路径动态一次。
626C1C十四引用与D67BB8九引用同归上述生产/消费父；
原回调表的数据入向保留parent空值，不把180引用当全部caller已验收。

449DA0仍先初始化聊天记录/输入状态，再采样volatile共享写入流，非空直接返回。
路径按同源旧时间对象/16B文件名/260B路径与Log目录生产，两格式接53B304。
r打开后立即发布626C1C；缺文件时a打开并发布，原三次分隔/标题输出仍
无条件调用，不能加append失败保护。已有r流先关闭，再a重开并发布。
gate非空才写会话三输出，各输出重新取共享volatile流/时间字段；
最后无条件flush当前共享流，结果忽略。目录/时间/状态正文不变。

44B050保留颜色/字体S32门槛、分段复制和复制中途采样流的时点。
流非空时依序采样时间→重读写索引/记录→格式化输出→重取共享流关闭
→a重开→重读写索引→发布新流。随后原字段/循环索引/数量更新保持，
打开失败仍发布空，写入/关闭/重开返回不新增门槛。
共享流声明/定义同步改为LegacyFile* volatile；头文件仅前置声明类型，
不新增流owner、锁或对象包装，原重复采样与存储生命期不改变。

44B5A0以systemlog.txt/w清日志，44B5C0以a写%s换行；
打开失败0，成功关闭的S32结果按原W32位模式返回，写返回忽略。
接收协议先observer(0)再以配置路径/a+写%s换行并关闭，之后才分词；
发送先以配置路径/a+写->%s换行并关闭，再observer(1)，之后才原
长度/换行处理与发送。缺配置/打开失败不阻断其他正文，
两协议日志不涉及已排除加密或暂停音频，不计所有间接caller可达。

现代66的D67BB8直接保存原生LegacyFile*，与聊天写入流是两个原槽。
移除LogViewerFileSlot、gLogViewerFileStorage及两私有open/close助手，
所有生产/消费/关闭同时改型。真实fclose仅清flags而不释放FILE对象，
日期按钮关闭后保留外部指针，4CC540仍对同一非空槽再调用关闭，
不人为省略第二次调用，也不新增引用计数、关闭状态或失败回收。

4CBD30首次借原共享LegacyTm并写同源文件名的格式接53B304，其余正文保持。
4CC540先关闭非空槽，再两个格式/256B未清path，rt成功先发布error0，
关闭该流再a+t打开并发布。任一打开失败只写error1，旧显示上限/
593F84保持；成功才门禁1/上限0，不先seek、不新增目录创建。
4CC61E及4CC68B/4CC68E改为同一FILE flags低U8/10H直接读取；
每轮原1000B局部清零，fgets(1000)返回忽略，strlen>65加(length-1)/65，
再无条件上限+1并重取共享槽，EOF空读取那次仍计数，不改为成功才计数。
这次1000B清零有4CC640的rep stosd证据，不外推旧待批入口。

4CC340仍入口清1000B和10000×65B表，失败门槛/页起点生产保持。
读取门禁成立时seek(S32 0,BEGIN0)忽略返回、fgets(S32 1000)直到空，
每次从同一D67BB8重取流；原旧字节bit80、63/64列换行、10000门槛、
14行目标/页与窗口重读以及失败文本保持，不重验显示/点击/日期算法。
4CBDB0两日期关闭仍在日期字段修改前，后续4CC540、文件名回写、
页起点/滑块/音效顺序不改；不改变既有合分支表达式或恢复音频推进。

r/a/w/rt/a+t/a+、路径格式/Log/chat文件名、systemlog.txt、十个
fprintf点所消费原字节（含旧编码标题、81 40、换行及唯一NUL）
已逐字节对应。仅文件调用/五格式/类型与私有包装删除发生差异，
所有状态、协议、UI与其余格式正文逐字保持；完整CRT启动/模式3、
旧日志待批及其他FILE链、文件内容和运行验收仍待。
只静态核新增指令/引用/调用/字节/差异，不构建、链接、查产物、
新增/执行测试、读写日志样例、运行客户端或校验哈希。

## 入口argument.ini真实FILE整链接线实施SPEC（2026-10-03）

按第8条工程委托，以已有真实原FILE/格式核心替换WinMain的宿主stdio；
直接保存非拥有LegacyFile*，不增加包装、第二份流状态、失败保护或新依赖。
范围仅465790的argument.ini文件链，窗口/命令选项/初始化尾部正文保持。

原完整[465790,4659ED)共164条、唯一入向53DE7E（父53DDB5）。
465805→53B304、465814→53B2F1、46583B→53B226、
465890→53B1D0四调用均整链接线；465889直接读取FILE flags低U8/10H，
46588D在原字符串追加后回到465822，不用宿主feof或新状态缓存。

CGstart命中才先清4096B命令行和64B文件名，以%s生成
.\data\argument.ini，再按r打开。fileName64、line1028与fileCommandLine4096
局部容量保持，不把最后三行缓冲字节额外清零。
打开成功才do读取：每轮先清line前1025B，再fgets(S32 1024)，
忽略返回后按strlen长度作原S32累计；total>=4096有符号比较，
达到门槛不追加，未达到才strcat，再检查同一FILE低U8 EOF位。
首EOF因既有清零而追加空串后结束；读错误未置EOF的原循环不另加门禁。
正常EOF或容量退出均只关闭一次，S32关闭返回忽略。
打开失败仍发布已清空的fileCommandLine；成功也沿同一局部缓冲发布，
后续互斥量/updated门禁/窗口注册/九尾调用及最终0返回逐字保持。

已读取至实际NUL的原字节：55D03C=4347737461727400；
55D028=2e5c646174615c617267756d656e742e696e6900；
5535F8=257300；554908=7200。原模式/路径/命令字节不转码，
不重新验收命令解析、窗口或完整启动。当前仅静态核新增接线与差异；
不构建、查产物、测试、读写ini样例、运行客户端或校验哈希。

## 截图文件探测与原格式整链接线实施SPEC（2026-10-03）

按第8条工程委托，453760公共文件前导及已采用GDI分支改接真实FILE/格式；
探测流直接为非拥有LegacyFile*，删除宿主FILE混用，不增加槽表或包装。
既有SDL至GDI原生替换、掩码发布、捕获/BMP/释放/消息/节流正文保持。

原完整[453760,453DE0)共491条、唯一入向503C99（父503C80）。
本步对应五原点：4537DF→53B304路径，4537EE→53B2F1探测打开、
4537FB→53B1D0关闭，453823→53B304满额提示，
453B45→53B304 GDI公共提示。前两格式各一个源码表达式，
公共提示依原六路径各一个，共八格式加两FILE表达式；
每次动态执行仍路径一次、非空探测关闭一次、GDI提示一次，不重复输出。

ClientToScreen/目录与原errno17门槛/time/localtime先后不改，
序号S32从0到999，每轮重读同一借用时间对象月/日/年，以
screenshot\\CG%02d%02d%02d_%03d.bmp和year%100/month+1/day/sequence
写入原256B未清path。r打开为空立即停止探测，不额外区别不存在/权限错误；
非空关闭，S32返回忽略后递增。1000次打开均成功才以原无参数格式
写1280B未清message→44B050(color4,font3)→返回，不进入捕获。

GDI公共453B45由六原push选择格式：453A2B/556640 G1、
453A52/55661C G2、453A9D/5565F8 G3、453ADB/5565D4 G4、
453B2B/5565BC成功与453B38/556598写失败(7)。
各对应现有分支的256B未清message与同一path实参；
消息、malloc/free与GDI资源残留次序不改，BMP叶仍按Windows真实写入。
实际十字符串272B含唯一NUL逐字节保存，不转码或重解原乱码。

4539B4的Primary失败提示与453D8A/453DA9的Primary成功/失败格式
沿既有SDL截图替换裁决保留证据，不恢复COM分支、不列作本轮真实接线点；
也不因本步迁移格式而宣称原故障、捕获可用性或像素输出已验收。
当前只静态核新增调用/分支格式/字节/完整源码差异，不构建、查产物、
新增/执行测试、创建目录、探测/写入截图样例、读屏、运行或校验哈希。

## 三个业务FILE入口的剩余引用分类（2026-10-03）

本轮只读逐项比较53B2F1/53B1D0/53B226直接入向与当前业务接线记录，
真实引用数/已登记点分别67/62、96/86、8/4；152个登记地址均真实匹配。
余19引用按具体父分类，不以源码无宿主FILE推导原全部文件链完成：

- 旧402020/4021A0共9：打开402095/4020D1，读行40211C/4024BF/402529，
  关闭4020C2/4023C4/4028E6/40294A。既有首次未写1000B待批与完整生命周期继续待。
- 暂停音频524060/524230/52BE60共7：打开524071/52B2A6/52BF75，
  读行5240B9，关闭52417C/52B320/52C01F，不推进、不记排除或完成。
- CRT内部关闭共3：5446CF属既有CloseAllLegacyStreams_5446AD本体，
  不重复登记为新业务迁移；548EE0/549E6C属未承接窄/宽filebuf close，
  两父各20条，只有fclose成功才_Init(nullptr,2)，owner/绑定/虚调用仍待。
  当前局部事实不授权CRT宿主提案，不将两内部链的空sources改为已实现。

## 连接请求与收发错误格式整链接线实施SPEC（2026-10-03）

按第8条工程委托，将既有508360/5082D0/508310全部格式生产者接原53B304；
复用现有两个源码及内部头，不新增包装、消息owner、依赖或状态。
三父完整339/21/21共381条，三个直接入向均明确；八请求格式点与
两错误格式点共十点对应十源码表达式，各条动态路径保持恰调用一次。

5083BD/5083F6/50853A/508574/508669/50879E六次原调用仅压
目标DA34DC与提示常量，当前源码额外%s参数与原不符，本次恢复直接常量格式；
5084A4保留原IP失败格式及serverAddress，50850F保留CONNECT ERROR=%d与
已采样S32错误，十组原常量均读到首NUL并保留旧编码/尾空格/拼写。
共享1024B gRequestMessage不改容量、不先清、不新增错误替代文本；
serverAddress、CONNECT局部消息及其余栈/SDK对象沿既有代码，不转记原帧布局重验。

连接失败仍格式日志→真实44B5C0→关闭socket→发布失败提示→返-5；
地址/例外/拒绝三路径仍先格式提示再关闭，其他各负返回/状态写保持。
超时仍现有U32 tick差>600000才直接格式提示并返-1。
连接状态、socket/adapter/协议调用、采样/重读、门禁与整个控制器正文逐字保持，
不以格式收口重新验收网络/登录算法，不恢复newest或加密排除链。

5082D0/508310各先采WSAGetLastError S32，在256B未清局部格式化，
再调用已真实FILE输出/关闭的44B5C0，最后仅错误10035转0、其他原样返回；
不修正SNED拼写，不把would-block过滤提前，不将日志失败作为新网络结果。
去除LoginState已无消费者的cstdio头，NetworkRuntime其他格式仍保留该头。
当前只静态核新增调用/字节/参数/日志消费顺序及限定源码/JSON差异，
不构建、查产物、新增/执行测试、连接网络、读写日志样例、运行或校验哈希。

## 协议回包提示与命令前缀原格式整链接线实施SPEC（2026-10-03）

按第8条工程委托，既有50CF20/50E6E0/50F190/517510四父的全部格式
统一真实53B304，复用NetworkRuntime现有内部头，不新增包装、缓冲或owner。
范围仅三回包提示与已有非加密命令前缀；排除403C50/403E50仍保持，
不改命令变换/校验和/队列/发送或音频正文，不解除任何既有待批项。

四父完整16/46/73/113共248条、1/1/1/83共86直接入向。
50CF38/50E71A/50F23A/517590四格式点对应四源码表达式，
50F23A两格式push50F21C/50F230在同一原点汇合，保留现有条件表达式，
每条动态路径仍恰格式化一次，不复制两份调用或扩大调用次数。

50CF20仅status等于0才在256B未清局部以58B03C无参格式输出，
随后44B050(color4,font3)，未消费参数/早返和消息顺序保持。
50E6E0复用唯一DA33D8旧文本，非空且strstr未命中才以58B0B0格式
写256B未清message，参数先previous再text；随后变更提示→58B094警告→
回写旧文本，命中早返不回写、原空文本直接复制保持，不加新门禁或截断。

50F190仍name32/text300/message256未清，提取/反转义后先存查找索引、
插入原记录，再取该旧索引对应外部+18姓名作为%s，不能换输入name。
kind S32恰-1选58B0EC，否则58B0C8；格式→519550→原音效调用→44B050顺序保持。
不增加索引/提取失败保护，不继续实现音频，不重验公会存档/文本算法。

517510仍在既有4038A0命令变换后以58AA5C的%s加尾空格格式写借用buffer，
只传同一8B局部temp地址；不读取容量/裁剪输出/补写临时后缀，
后续密码长度、同一校验和状态清写、itoa16、拷贝不含NUL、尾部W16长度与0返回
逐字保持，不改任何位宽/分支/RNG或恢复排除链，也不将83入向全记作已验收。

六实际原字节组含首NUL已对应，包含未格式化警告和公会两分支；
不转码、不改变空格/旧编码/参数。原格式返回均忽略，删除已无消费者的cstdio。
当前只静态核新增调用/字节/参数/消费者次序与整份源码/JSON差异；
不构建、查产物、新增/执行测试、计算校验和样例、发包、运行或校验哈希。

## 角色创建数值原格式显示整链接线实施SPEC（2026-10-03）

本轮先核当前分派清单尾与NetworkDispatchDependencies.hpp，唯一残留回复声明为
50E680/murD，按音频暂缓保持，不沿历史“15回复”数量重新实现已有目标。
该直接清单核对不代表整个dispatcher、加密排除接线、链接或运行验收已完成。

按第8条工程委托，已有创建页4631D0共同显示尾的全部三格式点接真实53B304；
只新增既有内部格式头并删无消费者的cstdio，不新增函数/缓冲/owner/依赖。
完整父边界[4631D0,46428D)的指令统计1146、唯一caller462770（父4626A0）
已核；本轮完整读取[464178,46428D)83条共同尾，仅核新增格式接线，
不重复审查未改按钮/属性编辑/确认/消息/预览，也不放行4626A0入口门禁待决。

464197消费464182对9A4D04的S8→S32提升；
4641D4消费4641C2对9A5FD4起五连续字节的S8→S32提升，
46420A比较5且46420D回读，现有4B低字节+独立9A5FD8第五槽别名保持；
464221消费46420F对9A50C8的S8→S32提升。
原55CE0C的2530326400即%02d，三次push同一地址，目标仍32B未清局部buffer，
沿现有CreationSignedByte返回S32，不将原数据改U32/U64或改无符号格式。

三源码表达式及中间五轮保留，动态每次共同尾仍格式七次/显示七项；
每次格式后立即DrawText，先518/185，再五行479/(199+14*i)，最后518/293；
mode0/font13/color低U8 83H/两尾参数0和借用buffer地址保持，
不截断、补零、缓存格式结果、提前取全部字段或另设数值字符串表。
格式返回忽略，原DrawInputText在前、两背景绘制在后、既有S32结果最后返回；
文本消费者/队列/资源正文逐字保持，不改其复制或生命周期。

当前只静态核父边界/共同尾、三调用、字段宽度/符号、原格式字节、
绘制时点与整份源码/JSON限定差异，不构建、查产物、新增/执行测试、
执行数值格式/绘制样例、运行客户端或校验哈希；创建流程及画面仍未验收。

## 回车黑名单提示与清理序整链接线实施SPEC（2026-10-03）

按第8条工程委托，既有44AB60的黑名单提示生产者改接真实53B304，
复用输入/玩家名/黑名单/聊天/上下文owner，不新增包装、缓冲或依赖。
同时按原指令修正已发现的字段写序；只修改三格式表达式、这一清理片段和
无消费者的cstdio头，其他IME/聊天提交/历史/多行处理正文逐字保持。

原完整[44AB60,44ADAF)175条、唯一45974B（父4596D0）直接入向已核。
44AD13为自录无参提示，44AD66为已有/新增成功/新增失败三格式公共点；
原两点对应现有三个互斥源码表达式，每条黑名单路径动态仍只格式化一次。
四原格式push44AD0D/44AD60/44AD48/44AD54及各自首NUL字节明确。

输入与借用玩家名相同才用554938无参格式，未命中后才Contains，
其AL非零选554928；AL为零才Add，按其AL选择5532C4/5532B0，
公共三结果都仅以同一C8B1B8.text为%s，不提前复制、转码或冻结查询结果。
message仍原256B未清局部，格式返回忽略，不增截断、失败保护或字符串表。

44AD77清C8B1B8首字节，44AD7D清caret(+128)，44AD83清length(+12C)，
三写均先于44AD89通知，44AD93之后才切回626AA0，末尾44AD9B写相位20。
现有源码误为首字节→length→caret，本次恢复首字节→caret→length；
InputContext公开偏移断言与原字段对应，仅调换已存在的两个清零语句，
不改字段类型/归属或补写其他字节，不把聊天分支的length→caret→首字节改序。

聊天历史/发送分支仍按原重读当前上下文，多行六对象仍AppendChar(CR)后
重读并再判断黑名单对象；IME非空及聊天空输入早返、末相位更新保持。
黑名单查找/加入及44B050真实下游沿既有实现，不在此回车叶代建状态。
当前只静态核完整父/两格式/四字节/分支参数/字段清理与通知/切换时点、
整份源码/JSON限定差异，不构建、查产物、新增/执行测试、模拟按键/
读写黑名单或日志样例、运行客户端或校验哈希；完整交互仍未运行验收。


## 角色槽三处数值与文本格式接线（2026-10-03）

局部 SPEC：将既有 `ProcessCharacterSlot_461B30` 的全部三个宿主
sprintf 表达式接入已有真实 `FormatLegacyString_53B304`。按工程委托采用
此最小方案，只增内部格式头、删除无消费者的 cstdio；原槽位 UI、预览、
记录 owner 与返回正文保持，不推定偏移字段的业务名称。

本轮核对父 `[461B30,46208F)` 边界、343 条指令统计及两入向
`460FF0@4614F8`、`462330@46257B`；新取证范围是显示及预览相邻段
`[461E93,461F99)`，没有重新逐条核对其余父本体。

| 原格式点 | 格式原地址及含 NUL 字节 | 参数合同 | 即刻消费者 |
|---|---|---|---|
| 461EC2 | 553FDC：`25 34 64 00`（`%4d`），461EBC 压入 | 同一 0x94 字节记录 +12 的 S16，经 movsx 提升为 S32 | 461EE6 → DrawText2_456670 |
| 461F0E | 553FDC：`25 34 64 00`（`%4d`），461F08 压入 | 同一记录 +14 的 S32；有符号比较 ≥10000 时仅局部值改为 9999 | 461F2C → DrawText2_456670 |
| 461F48 | 5535F8：`25 73 00`（`%s`），461F42 压入 | 借用同一记录 +54 的原 NUL 文本 | 461F66 → DrawText2_456670 |

三次共用既有未清的 256 字节 buffer，各次格式后立即按原坐标、mode 0、
font 9、低 U8 色号及末参数 0 绘制，再采样预览 action、非空时写 +2B0=1、
重取同槽 action 并 FrameMove(100,0,0)。+00 名称仍直接绘制，不经格式。
`%4d` 是最小宽度，不新增截断；不转码、补终止、清尾、改为无符号值或
回写 +14，不新增异常保护。保持原两次角色存在判定及其他 UI/背景次序。

本轮只静态核对三个格式/原字节/字段宽度及符号、阈值与即刻消费者，
整份源码限定差异、文档追加及 JSON 登记；不重新核对其余槽位本体或
消费者内部。未构建、查产物、新增/执行测试、运行格式/绘制样例、
读写角色资源、运行客户端或校验哈希；完整角色槽交互与画面仍未运行验收。


## 配置读取后的路径与命令行四格式接线（2026-10-03）

局部 SPEC：`ReadSetupIni_465A80` 的既有四个宿主 sprintf 表达式全部
改接 `FormatLegacyString_53B304`，公共入口仍是 `ReadSetupIni()`。
按工程委托裁决此最小方案，复用已有内部格式头，不增配置封装或共享
存储；原 A 接口、命令行 owner、容量、预清差异、分支及已批屏幕键盘
排除保持。不是重新启用屏幕键盘，也不引入未批准的缓冲补值。

本轮只读完整父 `[465A80,465EFC)` 的 347 条指令及唯一入向
`465790@4659B7`。四格式及一个文件名参数均读至原首 NUL：

| 原格式点 | 原格式地址及含 NUL 字节 | 生产与消费 |
|---|---|---|
| 465B49 | 55D064：`25 73 25 73 5C 25 73 00` | 目录 buffer、唯一 gDataPath、55D154 文件名依次传入，生成256B fileName，供原四次配置读取 |
| 465C43 | 55D0F8：`77 69 6E 64 6F 77 6D 6F 64 65 20 25 73 00` | NORMAL 且原命令不含 windowmode 时生成4096B屏幕局部，465C52才切换共享指针 |
| 465D5A | 55D0B8：`6F 6E 6C 79 61 63 74 69 76 65 20 25 73 00` | ONLYACTIVE 且屏幕处理后的指针不含 onlyactive 时生成4096B活动局部，465D69才切换共享指针 |
| 465EE0 | 5535F8：`25 73 00` | 当前共享指针生成唯一4096B gCommandLineStorage，465EE8再切换共享指针，原S32计数保留至返回 |

55D154 文件名是含 NUL 的 `43 47 2D 53 65 74 75 70 2E 69 6E 69 00`
（CG-Setup.ini）。各次格式参数直接借用原字节串，不转码、截断、动态
扩容或增加范围/重叠保护。仍按目录固定下标255判断、ScreenMode→Effect→
ActiveMode→MonitorKeyboard读取、屏幕→活动改写、Effect优先级、CLOSE
判定、末次格式→指针持久化→返回的顺序；FULLSCREEN/NOACTIVE仍从
复制缓冲区头跳过11B，不使用strstr命中地址。两个前缀同时生成时仍为
onlyactive windowmode；活动局部原本不预清，不补零。

仅新增格式差异与上述调用/指针时序、原字节、整份源码限定替换、
两文档仅追加及JSON限定差异静态核对，不重新验收Win32 API或原字符串
工具内部。未构建、查产物、新增/执行测试、运行算法/配置样例、读写INI、
运行客户端或校验哈希；完整配置及启动交互仍未运行验收。


## 登录存档复位与删除路径四格式接线（2026-10-03）

局部 SPEC：`LoginSavedState.cpp` 余下四个宿主 sprintf 表达式全改接
已有真实 `FormatLegacyString_53B304`，删除无消费者 cstdio，复用已有
内部格式头。按工程委托采用此最小方案，只处理复位目录及删除路径；
既有初始化/删除 caller、真实 FILE 读写、编码及设置恢复正文均保持。
不扩展保存流程，不改变删除失败的业务处理，不实际操作存档文件。

本轮只读完整 `533360@[533360,5333CF)` 的35条及
`533860@[533860,5338E1)` 的41条，入向分别为533300@533300、
464840@464D25。四原点与三个删除消费者逐项核对：

| 原格式点 | 原字节合同 | 消费及原次序 |
|---|---|---|
| 533399 | 58F5B0：`64 61 74 61 5C 25 64 00`（data反斜杠%d）；533393压入 | 十轮S32 index 0..9分别写256B路径槽；每轮再active=0、map=-1、清同一records[0]的0x510B |
| 533885 | 553514：`25 73 5C 25 73 00`；53387F压入 | 物理槽目录、55D980的save.dat生成同一未清1024B path；533898立即DeleteFileA，忽略返回 |
| 5338AA | 同553514；5338A4压入 | 同一目录、58B6C4的mail.dat；5338B7立即DeleteFileA，忽略返回 |
| 5338C9 | 同553514；5338C3压入 | 同一目录、58B6B4的guildmail.dat；5338D6立即DeleteFileA，其完整BOOL32为父返回 |

三个文件名各自含NUL为 `73 61 76 65 2E 64 61 74 00`、
`6D 61 69 6C 2E 64 61 74 00`、
`67 75 69 6C 64 6D 61 69 6C 2E 64 61 74 00`；DeleteFileA由
53388A从IAT54D114加载后连续三次间接调用，不把寄存器编号当目标地址。
复位的四入口W32写序仍是newPhysical=-1、visibleCount=0、selected=0、
mappingCount=0；固定records[0]清十次不改为records[index]，末返回0。
路径只写格式结果及NUL，不清槽尾。删除不采样逻辑槽/存在性、不转码、
截断、补零、范围保护或合并成功标志，前两次失败不阻断后续删除。
原物理槽S32参数及共享路径owner不变，不新增私有存储。

当前初始化通过InitializeSavedState_533300调用复位；登录能力4
464840的既有流程经Sub_533860转发到私有删除本体。旧局部文档的
“本项不接caller”是当时步骤记录，不据此宣称当前caller缺失。
本轮静态核四格式/五字节组/35+41条/两入向、共享字段与即刻API顺序、
整份源码限定替换/其他正文逐字保持、三文档追加与JSON限定差异；
不重新核FILE/编码/设置恢复/API内部。未构建、查看产物、新增/执行测试、
运行格式/复位/删除样例、读写或删除存档、运行客户端或校验哈希；
完整存档交互及最终运行仍未验收。


## 旧技能选择、学习及遗忘九格式接线（2026-10-03）

局部 SPEC：LegacyUiRecord中的旧55/56/57三个既有父，共九宿主sprintf
表达式全部接入已有真实53B304。按工程委托裁决此最小方案，复用已有
格式头；该大文件仍有其他板块宿主stdio，保留cstdio。只更换三父格式，
不新建分派、预览/表/正文owner，不扩展学习资格或改变选择与发送门禁。

本轮只读三父边界 `[4D6710,4D6A26)`/229条、
`[4D6A60,4D6E6A)`/314条、`[4D6EA0,4D743C)`/438条统计，
逐项取证413条格式相邻指令及遗忘循环额外8条，共421条不同指令。
没有重新逐条核对其余父；三入口当前IDA引用均为空，不据此判死代码或
补造caller。401EA0只有单条jmp401EC0，保留新端现有直接Read_401EC0
适配及每个回传的Y→X读取/RNG消费时点，不额外读取坐标。

| 原格式点 | 原格式压入/原地址 | 参数与即刻消费者 |
|---|---|---|
| 4D686B | 4D6865/5531A4：%d | 点击第0/1按钮时S32 index+1，16B buffer→52D2D0@4D687C（另一16B/容量15）→512620@4D68B5 |
| 4D6B7F | 4D6B79/5531A4：%d | 两按钮各自S32 index+1，16B→52D2D0@4D6B90→512620@4D6BC9 |
| 4D6C8E | 4D6C88/5889A8：%7d | 原gold S32，未清64B text→456780@4D6CBA |
| 4D6D7B | 4D6D75/588C00 | CC2E68 S32，原含%d旧字节格式→456780@4D6DA6 |
| 4D6DBE | 4D6DB8/588BEC | CF4A3C S32，原含%d旧字节格式→456780@4D6DE7 |
| 4D702C | 4D7026/5531A4：%d | 原selection S32/许可非负分支，16B→52D2D0@4D703D→512620@4D7075 |
| 4D723A | 4D7234/553FB4：%2d | 合法技能行重新读取CB01A4[index] S32，未清256B text→456780@4D7269 |
| 4D7283 | 4D727D/5531A4：%d | 同行重新读取CB09D8[index] S32，同一text→456780@4D72B2 |
| 4D7351 | 4D734B/588C48 | 原合计S32，原含%2d/10旧字节格式，同一text→456780@4D7378 |

六唯一含NUL格式组：5531A4=`256400`，5889A8=`25376400`，
553FB4=`25326400`，588C00=`a8cfa5cea7deafe0c4e6a6ecbcc6a147256420c4e600`，
588BEC=`bec7b2dfb64fa5cea147256420aaf7b9f400`，
588C48=`b27ba662a7deafe0c4e6a8cfa5ceb671aabaa658ad70a1472532642f313000`。
三旧字节数组不重译、不转码；原最小数字宽度不改为截断或范围限制。
学习三显示即时绘制、原mode2/font0/color0/末两参数0保持；遗忘十五行
先许可判断、负值跳过该行，合法行先累计，再名字/第一值/第二值/图形，
合计在循环后才格式并绘制，保留原共享表读取时点，不缓存成私有行副本。
回传保留16B两局部/容量15、target/request/Y/X/socket取样、mode0和关闭
时点；旧55第三按钮不发送、旧56第二按钮发送、旧57的99取消与扫描15
读取既有CB01E0窄边界许可保持。不清尾/补零、钳制合计、扩表或加保护。

本轮静态核九点/九表达式/六字节组及参数/消费者、整份源码三父限定
替换/其余正文逐字保持、四文档追加与JSON限定差异。其他格式仍待；
旧55正文完整生产、独立图号衔接、旧表实际路由及生命周期/收发/画面
仍未闭合。不重验其余父、格式/转义/网络/绘制内部，未构建、查产物、
新增/执行测试、模拟点击或格式/发包/绘制样例、读写资源、运行客户端
或校验哈希，不计整个技能界面或总PLAN验收完成。


## 旧买卖列表及确认十八原格式点接线（2026-10-03）

局部 SPEC：LegacyUiRecord的旧63/65/64/66四个既有父，全部十六宿主
sprintf表达式接真实53B304，对应十八原点。按工程委托采用最小方案，
复用既有格式头，保留其他板块仍需的cstdio；不重写商品/分页/数量/
资格/预览/确认/关闭/提示，已有共用卖出表达式保持，不新增私有商品
或分派。原音效调用正文不改，不恢复暂停的完整音频。

本轮只读四父 `[4D9220,4DA3F2)`/1081条、`[4DA580,4DA9F9)`/351条、
`[4DAA40,4DBD23)`/1175条、`[4DBD30,4DC28E)`/439条边界统计，
共3046条；实际新读格式/拼接/发送/显示相邻860条不同指令，不重新
逐条核对其余父。四原入口当前引用均为空，不判死代码或补造caller。

| 父 | 原格式点 | 参数及消费者 |
|---|---|---|
| 4D9220买入列表 | 4D9F78/4D9FD6/4DA105/4DA165/4DA1B7 | +C24价格、+C28选择数、gold、C8AF6C合计、CC1A0C页+1/D2D63C页数全部S32；各自立即456780@4D9FBA/4DA003/4DA134/4DA193/4DA1E3 |
| 4DA580买入确认 | 4DA6FF/4DA87B/4DA8DC | S32列表index/selected拼接后52D2D0@4DA76A→512620@4DA7A7；selected与gold/total各自绘制4DA8AA/4DA90B |
| 4DAA40卖出列表 | 4DB741/4DB79F/4DB92A/4DB975/4DB9C9 | +C24价格、+C28选择数、gold、CF3FCC合计、CF49FC页+1/CA0ABC页数全部S32；各自立即456780@4DB783/4DB7CB/4DB959/4DB9A6/4DB9F7 |
| 4DBD30卖出确认 | 4DBEAD、4DBFDB/4DC04A/4DC0DE、4DC189 | S32物品+C2C索引/selected拼接后52D2D0@4DBF0F→512620@4DBF4B；名称及+C38/selected绘制4DC157；gold/total绘制4DC1BA |

八唯一原格式含NUL字节：5889A8=`25376400`（%7d），
553FB4=`25326400`（%2d），586D48=`25642f256400`（%d/%d），
586874=`25647c256400`（%d|%d），5889F8=`7825326400`（x%2d），
588E4C=`abf9a6b3aaf7c342202025376420aaf7b9f42020c1cab652aaf7c342202025376420aaf7b9f400`，
588F28=`2573282564817e25642900`，
588F00=`abf9a6b3aaf7c342202025376420aaf7b9f4202020bde6a558aaf7c342202025376420aaf7b9f400`。
三个既有字节数组及数量文案中81/7E两原字节保持，不重译或猜分隔编码。

卖出数量的三个原点是100H优先、无100H且200H、两位均无三个互斥分支，
仅+C30>1时分别格式，既有源码共用同一名称/数值表达式；每条路径动态
仍一次。100H名称借+C44备注/色5，200H及普通借+1名称/色1或0；
持有数<=1仍直接复制名称。两个标志分支仍借旧65坐标，普通才借旧66，
不把三个原点漏算、不重复执行格式或展开无须改动的现有公共表达式。
两列表各共用未清128B buffer；确认各共用512B连续区和32B entry，
确认仅清原文首字节，二十项used且selected非零才拼接，第二项起先加|。
买入用列表index，卖出用+C2C物品索引；后半256B转义、容量255，
target/request/Y/X/socket、mode0与随后关闭次序保持，不恢复包加解密。
保留字段重读、借用共享owner、价格/合计色号及原mode2/font0/末参数0，
不转码、限长、补零、清尾、扩容、钳制数量或新增异常保护。

静态核十八原点/十六表达式/八原字节/分支对应/参数与消费者，整份
源码四父仅十六标识符替换/其余正文及常量逐字保持、五文档追加、
JSON限定差异；不重验其余父或格式/转义/网络/绘制内部。商品/图号/
页数完整生产、旧表真实路由及实际输入/收发/画面仍未闭合。未构建、
查产物、新增/执行测试、模拟点击或格式/拼接/发包/绘制样例、读写
商品资源、运行客户端或校验哈希，不记整个买卖界面已运行验收。


## 旧修理与鉴定四页十四格式接线（2026-10-03）

局部 SPEC：LegacyUiRecord的旧70/71修理列表/确认与旧68/69鉴定
列表/确认，全部十四宿主sprintf表达式接真实53B304。按工程委托采用
最小方案，复用既有格式头，保留其他板块stdio仍需的cstdio；不重写
记录、分页、数量、资格、预览、确认、关闭或提示，不建立私有商品/
图号/页数/分派。不修改原音效调用正文，不恢复暂停的完整音频。

本轮只读四父 `[4DC7E0,4DDA2A)`/1140条、`[4DDB20,4DDF75)`/352条、
`[4DDFC0,4DF20A)`/1140条、`[4DF300,4DF726)`/332条边界统计，共2964条；
新取证格式/拼接/回传/显示相邻186/171/186/171共714条不同指令。
不重新逐条核对其余父；四入口当前原引用均为空，不判死代码或补造caller。

| 父 | 原格式点 | 原参数及消费者 |
|---|---|---|
| 4DC7E0修理列表 | 4DD465/4DD4C4/4DD643/4DD68E/4DD6E1 | CE4548记录+C24价格/+C28选择数、gold、D460C4合计、CF4A08页+1/CC4B24页数皆S32；即时456780@4DD4A4/4DD4F0/4DD672/4DD6BD/4DD70D |
| 4DDB20修理确认 | 4DDC89/4DDE5D | 有效选中记录+C2C物品索引S32拼接后52D2D0@4DDCE3→512620@4DDD1F；gold/total旧字节汇总即时绘制4DDE89 |
| 4DDFC0鉴定列表 | 4DEC45/4DECA4/4DEE23/4DEE6E/4DEEC1 | C8B390记录+C24价格/+C28选择数、gold、CB1E3C合计、CC3F00页+1/C9A950页数皆S32；即时456780@4DEC84/4DECD0/4DEE52/4DEE9D/4DEEED |
| 4DF300鉴定确认 | 4DF469/4DF60D | 有效选中记录+C2C物品索引S32拼接后52D2D0@4DF4C3→512620@4DF4FF；gold/total旧字节汇总即时绘制4DF639 |

六唯一原格式含NUL字节逐一读取：
- 0x5889A8：`25376400`。
- 0x553FB4：`25326400`。
- 0x586D48：`25642f256400`。
- 0x5531A4：`256400`。
- 0x5890A4：`abf9a6b3aaf7c342202025376420aaf7b9f420202020add7b27aaaf7c342202025376420aaf7b9f400`。
- 0x589150：`abf9a6b3aaf7c342202025376420aaf7b9f420202020c5b2a977aaf7c342202025376420aaf7b9f400`。

两列表各共用未清128B buffer：选择数先采样以确定价格色0/7，价格
格式/绘制后再次读取选择数，再格式/绘制；不合并成一次读取。两个
确认各用未清32B entry与512B连续buffer，确认仅清原文首字节，二十
项used且+C28非零才拼接，第二项起先加|，格式仅回传+C2C索引，不
发送选择数量、不换成列表索引。前/后256B原文/转义区、容量255、
target/request/Y/X/socket/mode0、关闭与取消原分支保持。两汇总先读
各自total再gold，格式变参仍gold/total顺序，原文本字节/空格保持。
即时文字mode2/font0/原色号/末两参数0及坐标、共享owner保持；名称
绘制及原修理备注/鉴定普通名称差异不改。沿既有401EA0→401EC0读值
适配，不新增坐标采样/RNG消费，不恢复包加解密。不转码、限长、扩容、
补零、清尾、钳制或新增异常保护，字段数值不拓宽。

静态核十四原点/十四表达式/六字节组/字段及消费者、整份源码四父
仅十四标识符替换/其余正文及常量逐字保持、五文档追加与JSON限定
差异；不重验其余父或格式/转义/网络/绘制内部。完整记录/图号/页数
生产、旧表实际路由及输入/收发/画面仍未闭合。未构建、查产物、新增/
执行测试、模拟点击或格式/拼接/发包/绘制样例、读写资源、运行客户端
或校验哈希，不计整个修理/鉴定界面已运行验收。


## 旧以物易物列表与确认八格式接线（2026-10-03）

局部 SPEC：LegacyUiRecord旧75/76的两既有父，全部八宿主sprintf
表达式接真实53B304。按工程委托采用最小方案，复用既有格式头，
保留其他板块仍需的cstdio；不重写材料统计、资格、分页、数量、
预览、关闭或提示，不建立新的商品/材料/字体/路由owner，不恢复音频。

本轮只读 `[4D36F0,4D495D)`/1112条与`[4D4A50,4D4ECC)`/349条
边界统计，共1461条；新取证两父格式/拼接/回传/测宽/显示相邻
216/201共417条不同指令，不重新逐条核对其余父。两原入口引用均为空，
不判死代码或补造caller，旧表路由及上游生产保持待决。

| 父 | 原格式点 | 参数及消费者 |
|---|---|---|
| 4D36F0列表 | 4D43FD/4D445B/4D45B6/4D462D/4D46AD | C2CH记录材料数/数量、CF4A24可用数、D2F334总需求、CB10D0页+1/C8A954页数皆S32；各自即时456780@4D443F/4D4487/4D45E5/4D465E/4D46DA |
| 4D4A50确认 | 4D4BCF | 二十项used且数量非零，S32列表index/selected以%d|%d拼接；52D2D0@4D4C3A→512620@4D4C77 |
| 4D4A50显示 | 4D4D51/4D4DB8 | 名称绘制后重读selected供x%2d，456780@4D4D80；汇总借CB0184名称、CF4A24与D2F334，44B260@4D4DC5测宽后456780@4D4DF8居中 |

六唯一格式含NUL字节逐一读取：
- 0x5889A8：`25376400`。
- 0x553FB4：`25326400`。
- 0x586D48：`25642f256400`。
- 0x586874：`25647c256400`。
- 0x5889F8：`7825326400`。
- 0x5889D8：`2573202025376420add3202020a5e6b4abadd3bcc6202025376420add300`。

列表沿同一CA0B30二十条C2CH记录；图标后采样材料数、总需求、可用数，
以原有符号比较available < cost+total决定色7/0。价格格式/绘制后重读
选择数，原128B未清buffer、单位5889A4的色4及其绘制时点保持。
确认只清512B原文首字节，以未清32B entry逐项拼接，第二项起先加|；
数量非零含负数仍选择，零条也继续转义/发送空串，不加>0或计数门禁。
前/后256B原文/转义区、容量255、target/request/Y/X/socket/mode0及
关闭/取消保持。绘名后再取数量，不缓存发送前值。汇总先D2F334再
CF4A24，变参为CB0184名称/available/total，保留原旧字节与空格；
格式后使用已实现44B260固定字体快照测宽，再重取面板Y+254，X为
320-width/2，不加面板X、不改用另一测宽器。沿已有读值适配，不新增
坐标读取或RNG消费。保留mode2/font0/原色号/末两参数0，不转码、
限长、扩容、补零、清尾、钳制或新增异常保护，不恢复包加解密。

静态核八点/八表达式/六原字节组/参数及转义/回传/测宽/绘制消费者、
整份源码两父仅八标识符替换/其他正文及常量逐字保持、三文档追加
与JSON限定差异；不重验其余父或格式/转义/网络/字体/绘制内部。
完整商品/图号/页数/旧表路由及输入/收发/画面仍未闭合；未构建、
查产物、新增/执行测试、模拟点击或统计/格式/拼接/发包/测宽/绘制样例、
读写资源、运行客户端或校验哈希，不计整个以物易物界面已运行验收。

## 2026-10-03 仓库金币存取格式接线 SPEC 与裁决

本步只迁移既有 `Sub_4E2D90/4E3750` 的十处格式表达式，统一调用
`platform::FormatLegacyString_53B304`，保留既有64B未清局部、S32参数、
同源余额/草稿/费用和即刻绘制。按子项目工程委托采用此方案，直接替换
调用标识符；不新增格式适配、独立草稿、初始化、转码、限长或异常保护。
旧金额本体、外层回调、按钮/键盘/上限/确认登记及提示返回均不改。

当前原父范围为4E2D90..4E3743（600条统计）、4E3750..4E401C
（542条统计）；本步读取格式/显示相邻188/187条，另读两父34条入口/EDI
赋值与外层19条回调装载相邻指令，不将其记为1142条全文重新验收。
两个原引用4E0441/4E044C是4E0380向局部表写入函数指针，均非直接call；
当前源码四页回调表已绑定两子页。本步保留该真实内部接线，不据此推断
旧窗口表、完整输入或数据生产已闭合。

| 父 | 原53B304点 → 即刻456780点 | 格式参数与相对面板坐标 |
| --- | --- | --- |
| 4E2D90 | 4E3392 → 4E33BD | 重读D67618对应组末FBF4的S32余额；%7d；146,150 |
| 4E2D90 | 4E33D3 → 4E33FC | 既有S32上限；%7d；421,118 |
| 4E2D90 | 4E3418 → 4E3443 | CC4B2C+44别名CC4B58草稿，格式时重读；%7d；421,158 |
| 4E2D90 | 4E3582 → 4E35B0 | 消息为0分支的D40B04手续费；原金币字节；139,306 |
| 4E2D90 | 4E35F7 → 4E3623 | 格式前重读玩家+FC金额；原金币字节；417,306 |
| 4E3750 | 4E3CB7 → 4E3CE4 | 格式前重读玩家+FC金额；%7d；423,151 |
| 4E3750 | 4E3CFA → 4E3D22 | 既有S32上限；%7d；143,118 |
| 4E3750 | 4E3D3E → 4E3D6B | D2EA98取出草稿；%7d；143,158 |
| 4E3750 | 4E3E80 → 4E3EAC | 底部重新读取D67618对应组末FBF4余额；原金币字节；139,306 |
| 4E3750 | 4E3EF1 → 4E3F1D | D40B04手续费；原金币字节；417,306 |

六个%7d原格式指向5889A8，含NUL字节为25376400；四个金币格式
指向588FC4，含NUL字节为25376420aaf7b9f400，沿既有
`kLegacyStorageAmount588FC4`，不转换原AA F7 B9 F4。
两页均先三行纯数值（mode2/font0/color0），再原按钮和背景，
再底部文案/两金额（mode2/font1/color0）；DrawText末两参数均0。
面板Y先于X在各次绘制重新读取，坐标传递保留S16转换，不缓存底部
余额或玩家金额到入口值；消息非0时不补造手续费格式调用。

板块实施后集中核对十处原格式/压入点/参数/消费点和常量字节，整份
LegacyUiRecord.cpp应等于旧文件仅这十处标识符替换，已有头文件不变。
JSON仅共享格式项和两个新增父项，其他记录、待批补值与分类保持；
两份合同仅追加，唯一进度追加本轮。不触发总SPEC/索引对齐或日志压缩。
业务格式累计150个原点，其他872个原引用不计完成。
未构建、链接、检查产物、新增/执行测试、运行存取/格式/绘制样例、
读写资源、运行客户端或校验哈希，不计整个仓库界面已运行验收。

## 2026-10-03 仓库外层与物品/宠物剩余格式接线 SPEC 与裁决

本步迁移旧72六个既有父的全部剩余宿主sprintf：23个原53B304点对应
14个现有表达式，直接调用 `platform::FormatLegacyString_53B304`。
按子项目工程委托采用只替换调用标识符的方案；既有lambda与属性循环
保留，各次采样/动态调用/消费者不合并，不新增适配、转码、限长、
初始化、保护或共享状态。其他父、本体/回调/输入/收发/音效均不改。

| 父及原末端 | 指令统计 / 本轮相邻指令 | 原格式点 / 现有表达式 | 原格式与消费者 |
| --- | --- | --- | --- |
| 4E0380..4E0BBF | 586 / 196 | 7 / 2 | 确认六字段回传1点；六项汇总6点由一个drawCount表达式承载 |
| 4E0D10..4E0FEF | 212 / 42 | 1 / 1 | %3d物品数量，4E0EFF→4E0F4F |
| 4E10F0..4E1837 | 414 / 47 | 2 / 2 | %7d来源数量/拆分草稿，4E16D4→4E16F7、4E1713→4E1733 |
| 4E1B10..4E1EDB | 317 / 190 | 9 / 5 | 等级、HP/FP、五属性、末字段；属性五原点仍共用一个循环表达式 |
| 4E2030..4E28E9 | 659 / 67 | 2 / 2 | 手续费/玩家金额，4E25B2→4E25DB、4E2620→4E2649 |
| 4E28F0..4E2D88 | 357 / 67 | 2 / 2 | 手续费/玩家金额，4E2BDB→4E2C04、4E2C49→4E2C72 |

六父共2545条统计，本轮格式/消费相邻609条；另核七处原引用指令、
EBP/EDI零定义及消息0/1分支，不作为其余本体全文重新验收。
外层入口原引用当前为空，不判死代码。物品列表/拆分分别由4E2030
两处/一处直接调用，详情由4E28F0两处直接调用；4E2030/4E28F0
各有一个4E0380局部表指针装载，不能计成直接call或推完整旧表路由。

4E06B3用64B未清buffer，原5891DC含NUL字节
25647c25647c25647c25647c25647c256400。确认二十物品/五宠物掩码只按
S16选择>0，不检查used；先取存入金额再取取出金额，变参顺序仍
物品取/存、宠物取/存、金额取/存，均S32。4E06CA转义至128B/容量127，
target→request→Y→X→socket取样，4E0706 mode0回传后按原顺序关闭和
音效调用；不恢复加密。子页和逐帧统计次序、used与确认门禁差异保持。

六项汇总依序CF4A04、D2D824、CF49B8、D2DAD4、D2D600、D2F79C，
S32且逐项采样后立即格式绘制；原点4E090A/4E0950/4E0998/
4E09E1/4E0A2A/4E0A72，对应消费者4E0934/4E097C/4E09C6/
4E0A0E/4E0A56/4E0AA0。三格式为5891D4的%2d及AD D3、
5891CC的%d及B0 A6、588FC4的%7d及AA F7 B9 F4；
含NUL分别25326420add300、256420b0a600、25376420aaf7b9f400。
各次重读面板Y+38再X+37/131/180/311/405/455，mode2/font1，
左三color1、右三color5，末两参数0；不预采样六字段或改成一个大格式。

物品列表图标后读取同一C44H记录+8的S32数量，仅>0才%3d/即时绘制，
128B未清buffer、mode2/font1/color0和网格坐标保持；选择/图标/覆盖顺序不改。
拆分层先前已关闭仍按原处理键盘/上限，显示重新读取
FDA8A4[source+8].count及CB2220，256B未清buffer、入口坐标
x+88/y+46与y+86、mode4/font1/color0保持，不改变登记或既有上限处理。

宠物详情同一154H记录：+0C等级%3d；先读+14最大HP再+10当前HP，
变参current/max；先读+1C最大FP再+18当前FP，仍current/max；
+20/+24/+28/+2C/+30依次%3d，再+44用%2d。128B未清buffer，
mode4/font1/color0、x+191，y+30/90/110/130/150/170/190/210/230及
九即时绘制保持；名称/种族测宽、预览/属性条和固定分页文本不改。
原格式553FCC/553FE8/553FB4含NUL字节分别25336400、
2534642f25346400、25326400；不展开现有五属性循环。

物品和宠物子页各自消息0才格式手续费，其他消息不补造调用；底部
玩家金额在格式前重读，64B未清buffer、原金币字节、相对面板
139/306和417/306、mode2/font1/color0、先Y后X及背景后续次序保持。

板块实施后集中核对23原点/14表达式、全部原格式/参数/消费点，
整份源码仅14调用标识符替换；两合同仅追加，JSON仅共享项和六新增父，
其他记录/待批补值/分类保持，唯一进度追加。不对齐总SPEC/索引或压缩日志。
业务格式累计173个原点，其他849个原引用不计完成。完整容量/展示区
生产、旧表handler及输入/收发/画面仍待；未构建、链接、检查产物、
新增/执行测试、模拟掩码/统计/拆分/属性/格式/转义/回传/绘制样例、
读写资源、运行客户端或校验哈希，不计旧72运行验收。

## 2026-10-03 旧交易公开载荷与数值格式接线 SPEC 与裁决

本步仅将旧52六个既有父的14个宿主sprintf表达式接
`platform::FormatLegacyString_53B304`，对应23个原点。按子项目工程
委托采用直接替换标识符的方案，保留现有六汇总lambda、五属性循环、
各次读取/消费者及缓冲；不新增格式适配、转码、限长、初始化或保护，
不改公开/撤回/确认、输入/数量、共享状态、预览、提示、音效或返回。

| 父及末端 | 指令统计 / 本轮相邻指令 | 原点 / 表达式 | 格式链 |
| --- | --- | --- | --- |
| 4E5E40..4E6CBF | 1027 / 272 | 8 / 3 | 公开物品/宠物索引两点，六汇总六点仍共用一drawCount表达式 |
| 4E6E10..4E70A9 | 192 / 39 | 1 / 1 | 4E7009→4E7050物品数量 |
| 4E7380..4E773E | 312 / 190 | 9 / 5 | 等级/血魔/五属性/技能数，五属性仍共用循环表达式 |
| 4E7870..4E7FB7 | 414 / 47 | 2 / 2 | 4E7E54→4E7E77来源数量，4E7E93→4E7EB3拆分草稿 |
| 4E82F0..4E83C3 | 68 / 26 | 1 / 1 | 4E835E→4E838C对方金额 |
| 4E8AD0..4E9274 | 459 / 51 | 2 / 2 | 4E9038→4E905F玩家金额，4E907B→4E90A7己方提供额 |

六父共2472条统计，本步读取格式/消费相邻625条，另核七引用指令及
格式绘制所用零值/mode寄存器定义，不作为其余本体全文重新验收。
外层当前原入口引用为空不判死代码；列表两处、详情两处、拆分一处
为真实直接call，两个金额页各为外层局部表装载，现有内部调用保持；
完整旧表handler路由不能据这些引用计完成。

公开阶段0沿used W16非零且S16选择>0扫描己方二十物品/五宠物，
4E6323以%d生成S32物品槽号+8，4E63FF以%d生成S32宠物索引。
16B未清buffer与四1024B原文/复制区保持，各原文只清首字节，
第二项起先加原竖线；两次原53BEC0 strncpy(...,1024)在全部拼接后执行，
不改成转义或额外清尾。4E648E沿既有512040消费两复制区，
先采样D046D0提供额再socket，之后原阶段/音效/tick顺序保持。
原格式5531A4含NUL为256400；仅迁移格式，不重验复制或发包内部。

六汇总按CC4C94、C8AF68、CF4688、CB03CC、CF4AB8、D046D0
逐项读取S32值并即时格式绘制；原点4E684A/4E688D/4E68D2/
4E6918/4E695E/4E69A3，对应4E6871/4E68B6/4E68FD/
4E6942/4E6987/4E69CE。原5894A4/58949C/589494含NUL字节为
253264add300、2564b0a600、253764aaf7b9f400，数字与单位之间无空格，
沿既有交易常量，不换仓库格式。128B未清buffer，当前面板Y+38与
X+37/131/180/331/405/455，mode2/font1/color0、末两参数0保持，
不缓存六值或扩成一个大格式。

物品列表同一C4CH记录+8 S32数量在图标后读取，仅>0才%3d即时绘制，
128B未清buffer、交易网格及mode2/font1/color0保持。
拆分显示重新读取FDA8A4[sourceD45F90+8].count，再D2D490草稿，
原%7d/256B未清buffer、入口x+88/y+46或86、mode4/font1/color0保持；
关闭后仍键盘与原上限处理不动，不借用仓库草稿或状态。

宠物详情同一5FCH记录+0C等级%3d；先+14最大HP再+10当前HP、
先+1C最大FP再+18当前FP，变参仍current/max用%4d/%4d；
五属性+38/+3C/+40/+44/+48依次%3d，+5C技能数%2d。
128B未清buffer、九即时消费者、mode4/font1/color0、入口x+191及
y+30/90/110/130/150/170/190/210/230保持；五属性循环不展开，
名称+5D8、种族/名称测宽、预览/属性条及固定分页正文不动。
553FCC/553FE8/553FB4含NUL为25336400、2534642f25346400、25326400。

对方金额CF4688在格式时读取，32B未清buffer，当前面板146/150、
mode2/font0/color0保持。己方金额第一格式仍用键盘/撤回后采样玩家
金额的原快照，原业务上限回写后第二格式重读D046D0；128B未清buffer、
当前面板421/118或158、mode2/font0/color0保持。三个原格式
均5889A8的%7d/25376400，不改金额编辑、撤回、门禁或按钮/背景顺序。

板块实施后集中核对23原点/14表达式、八含NUL原格式组、S32值/字段、
拼接/复制/回传及21文字消费者；整份源码仅14标识符替换。
两合同/唯一进度仅追加，JSON仅共享项及六新增父，其他记录/待批/分类不变，
不对齐总SPEC/索引或压缩日志。格式业务累计196点，其他826原引用不计完成。
完整数据生产、旧表handler与输入/收发/画面验收仍待；
未构建、链接、检查产物、新增/执行测试、模拟选择/统计/拆分/属性/
格式/复制/发包/绘制样例、读写资源、运行客户端或校验哈希，
不计旧52交易运行验收，不恢复音频或包加解密。

## 2026-10-03 摊位五格式与复制消费者

4CD4F0共享玩家金额一格式、4CFE70的两报价尾竖线和两条件名称/说明
格式已接真实53B304，同时八处相关复制接真实53BEC0。
[局部SPEC](cg2d_legacy_string_compare.md#2026-10-03-摊位编辑浏览回复与上架文本接线-spec-与裁决)
记录十三点/原字节/计数与消费者。原%d/%d|/%s、S32金额/报价、
16B共享金额/12B报价/33B名称/113B说明与276B转义区保持。
共享金额点4CDAEF的目标压入57B440，实际格式压入4CDAE5指向5531A4；
依第二参数位置取格式，不能用最后一个mapped push推断。
原20/5槽、每次命中后报价重取、三次转义→复制→下一长度取样或socket→
尾NUL→5120D0及后续保存/提示/显示顺序保持，核心本体/头声明逐字不改。
本步三父只改十三调用标识符，不转码、补零或增加限长/保护。
格式累计201原点，其他821未计完成；未构建、测试、模拟格式/复制/
解析/回包或运行，不记完整摊位/CRT验收。


## 2026-10-03 窗口交易公开格式真实接线增量

既有4B0A50两表达式接真实53B304，对应4B0B29/4B0BEC；
原%d|%d第二参数压入4B0B23/4B0BE6均586874，
含NUL字节25647c256400沿已有共享证据，不重复登记。
52B未清buffer、S32 slot/position与两循环逐次格式→拼接→计数保持。
两1024B复制同时接真实53BEC0；四载荷区/原首字节清零/
物品后宠物/金额先于socket及512040消费者顺序不改。
工程裁决与原212条/实际回调槽见[字符串局部合同](cg2d_legacy_string_compare.md)。
整份WindowRuntime只改四标识符，两个核心/声明和原型表保持；
JSON两共享项加一既有父，格式201→203，其余819原引用不计完成。
三合同/唯一进度仅追加，不对齐总SPEC/索引；
未构建、测试、模拟格式/拼接/发送或校验哈希，不计完整交易/CRT运行验收。


## 字符串流和字符串扫描的真实长度依赖（2026-10-03）

依[字符串局部SPEC](cg2d_legacy_string_compare.md#字符串流与扫描包装真实长度板块spec2026-10-03)，
本线程裁决WriteLegacyStreamString_53B356与ScanLegacyString_53CADC各一长度点交
真实53F9E0。两整份源仅两个标识符/LegacyScan一个已有公开头；stdio原U32长度先于
临时缓冲/写流，扫描原flags/base/current先于长度，再同位型remaining/真实va_list及input保持。
既有std::uint32_t转换与std::bit_cast<S32>、FILE布局和未补字段、写满结果/结束flush
取舍、全部扫描/流核心及黑名单/21个已接扫描业务正文逐字不动；1个音频扫描继续暂停。
49条统计只本步32邻读取与2/22原入向，不重验余17/所有上游或FILE/扫描/格式内部。
三个JSON旧项增量、其他828/pending18/分类4保持，合同/唯一进度末追加，不构建、
测试、运行扫描/写流/黑名单样例、读写文件、读资源、运行客户端或hash校验。


## 输出与浮点父的三长度三复制直接接线（2026-10-03）

依[字符串局部SPEC](cg2d_legacy_string_compare.md#输出与浮点排版真实长度复制板块spec2026-10-03)，
本线程裁决输出共用strlen、文本移位/舍入两strlen及科学尾模板/特殊值两strcpy原点交真实核心。
现三文件只六标识符/三个已有公开头；Z空源与浮点共用原长度点映射两个源表达式，
特殊IND/INF和SNAN/QNAN原两复制点映射同一text表达式，不重复计数或新拆逻辑。
e+000含NUL六字节、四特殊串和复制后count5/6、原符号/缓存重读/小数点/指数及
三个归零路径、四长度转换/加NUL与原重叠移动/输出state/浮点运算正文逐字不动。
五父1045统计本步190读取/13直接入向，只核新依赖/共用控制来源，不重验余855或完整排版。
七旧JSON项增量、其他824/pending18/分类4与六槽/CRT真实启动未决保持；合同与唯一
进度仅末追加，不构建、测试、运行浮点/排版/长度/复制算法或样例、读资源、
运行客户端或hash校验，不把本步接线记作x87异常、失败、完整输出/浮点/运行验收。


## 摆摊宠物行十五个真实格式调用（2026-10-03）

局部SPEC与本线程裁决：仅Sub_4D0EB0十五个std::sprintf的callee改为既有真实FormatLegacyString_53B304。
十三信息点4D0F6C/4D0FC0/4D1006/4D1062/4D10B7/4D10FD/4D1143/4D118A/4D11D4/4D121A/
4D1263/4D12AD/4D12F4，与自身/浏览价格4D151E/4D1616一一接线；已有LegacyFormat头及其他源码保持。

5535F8的%s、553FDC的%4d、553FCC的%3d、5531A4的%d含NUL分别257300/25346400/25336400/256400。
名称/种族借用指针、两个401EC0取S32、其余S32字段与两价格owner、32B未清buffer、原忽略返回及
格式后即刻文字消费者/价格再次取样保持；不缓存前次金额，不改绘制坐标/颜色/顺序、共享绘制lambda或表路由。
原658条统计，本步只读255格式邻窗，余403/已过核心和完整父不重验；0直接入向不伪装回调调用。
业务原点203→218/1022，其余804仍待，十五表达式不扩大为所有字段/窗口/CRT验收。
共享项仅追加十五connections、已存在四常量不重复；新增一父映射，semantic831→832，
其他830/pending18/分类4保持。两合同/唯一进度末追加，集中核原字节/参数/原点/顺序和限定差异，
不构建、测试、运行格式/绘制算法或样例、运行客户端或SHA/hash校验，不刷新总SPEC/索引。


## 宠物详情五属性/重量真实格式（2026-10-03）

局部SPEC：4B3BF0的五属性循环/重量及4D0A80的五属性循环，共三个现std::sprintf表达式接真实53B304。
本线程裁决仅改callee，保持原100B未清buffer、S32字段、忽略返回与每次立即456780绘制。
4B3CBA/4B3CF9/4B3D2B/4B3D5D/4B3D8C与4D0B61/4D0BA0/4D0BD2/4D0C04/4D0C33各组五原点
分别由一个循环表达式承接，pet.values70[5+row]保持各自玩家/浏览owner，非两个原调用点。
4B3DC9的%3d / %3d在最大重量+6E0 S32>0才读取重量+6DC，传参重量在前，不交换或预读。
586BBC/5535B8含NUL字节2531306400/253364202f2025336400，已有%10d不重复登记，仅新增重量格式。

原两父745条统计，仅206格式邻窗读取，余539/五直接入向后的父链及已过核心不重验；
借用/索引早退、S16坐标/绘制参数/顺序、预览共享T*和技能直接文本均逐字保持，现有头不变。
业务点218→229/1022，其他793仍待；semantic832→834，仅共享一旧项/两新父，其他831/pending18/分类4保持。
两合同/唯一进度末追加，集中核原点/共用表达式/常量/参数/顺序与限定差异，不构建、测试、
运行格式/绘制/循环算法或样例、运行客户端或SHA/hash校验，不刷新总SPEC/索引，不计完整详情/CRT运行验收。


## 摆摊宠物候选行真实格式（2026-10-03）

局部SPEC：4D16F0五callee接真实53B304，原名称/等级/HP/FP/种族在4D1771/4D17D8/4D1860/4D18E8/4D195E，
5535F8的%s、553FCC的%3d和553FE8的%4d/%4d均沿已登记原含NUL字节，参数仍借用文本或S32。
本线程裁决只改callee，保持32B未清buffer/忽略返回和即刻绘制；四次原401EA0→401EC0的maxHp→hp→maxFp→fp
与中间候选页重新取样保留，格式参数仍当前值在前。种族按S16 0..19，否则只清首字节。
入口名称initialPet借用、其后逐次重取玩家记录、共享绘制lambda/S16坐标/3-12-0参数和全部后续正文不改。
原393条只读107格式/采样邻窗，余286/已过核心不重验；直接入向0不冒充表回调调用。
业务229→234/1022，其他788未计完成；semantic834→835，其他833/pending18/分类4保持，原格式集合不变。
两局部合同/唯一进度末追加并集中限定静态核对，不构建、测试、运行格式/绘制算法或样例、
运行客户端或SHA/hash校验，不刷新总SPEC/索引、不计完整父/67/68/CRT运行验收。

原取值caller实际指向401EA0，其唯一指令jmp 401EC0与现Read_401EC0身份一致；本步只核桥接归属，取值核心不改。


## 摆摊物品价格/两数量真实入口（2026-10-03）

局部SPEC：4D04A0@4D083F/4D08C8、4D1D50@4D1FCD三个表达式仅将std::sprintf接真实53B304，本线程直接实施。
原%d价格为先选择自身/浏览来源的同一S32快照，颜色继续用该快照；两%3d数量均S32>0才格式和即时绘制。
5531A4/553FCC原含NUL字节256400/25336400、三个20B未清局部/忽略返回、owner与原采样/消费者顺序保持。
两父651条统计，仅72格式邻窗读取，余579/核心不重验；无直接入向，不把原回调槽算普通call。
仅源三个callee、一共享旧项/两新父：semantic835→837，其他834/pending18/分类4及原常量集合不变；业务234→237/1022，其他785仍待。
局部合同/进度只末追加，集中核新原点/宽度/正数量门槛/价格快照及限定差异，不构建、测试、运行格式/物品算法或样例、
运行客户端或SHA/hash校验，不刷新总SPEC/索引，不把本步记为完整物品/67/68/CRT运行验收。


## 晶石商店69/70十二原格式点（2026-10-03）

局部SPEC仅将四父的十二std::sprintf表达式接真实53B304，本线程按保留现正文方案实施。
4C2E37/4C4F5E的[kok]/[han]两路径各共一原call，现两互斥表达式保持动态一次；4C2F49/4C5070为S32正数量。
4C31E3/4C31FB、4C5303/4C531B保留factor非0含负值或等0的%2d X %2d/X %2d及S32参数顺序。
4C32FE/4C3348、4C541E/4C5468各两金额原点由同一现循环表达式承接，visible门槛、value采样后S16坐标和4771B0次序保持。
新增四含NUL格式：586BCC=25735b6b6f6b5d00，586BC4=25735b68616e5d00，588194=25326420582025326400，58818C=582025326400。
256B/52B未清buffer、借用名称/memo、忽略返回/即刻复制或绘制及其他完整正文/原内联扫描不改，既有头不动。
四父1319统计，只读252格式邻窗，余1067/核心不重验；入向仅四data xref，普通call0，不冒充回调槽验证。
格式237→249/1022，其他773仍待；semantic837→841，一共享旧项/四新父，其他836/pending18/分类4不改。
两局部合同/唯一进度末追加，集中核共享分支/循环分组/原字节/参数/宽度/取样顺序与限定差异，
不构建、测试、运行格式/商店/绘制算法或样例、运行客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整69/70/CRT运行。


## 晶石商店余额/四确认文案真实入口（2026-10-03）

局部SPEC仅4C1E9D/4C4156两余额和4C2377/4C2389/4C459B/4C45AD四文案接真实53B304，本线程按保留正文方案实施。
%10d原586BBC字节2531306400不重复；69读取玩家晶石S32，70仍先51B5F0(D6791C)取S32，两现有12B共享owner保持。
四原含NUL文案588378/587F0C/5883D4/5883BC与现hex字面量逐字对应，50/21/48/21B均不含%，按原零变参调用；
不添加%s包装、翻译/改空格/猜编码或预清；保留标题→问题及前后金额/项/按钮复制/可见state/音效的原时点。
四父699统计，只读112格式邻窗，余587/核心不重验；四data xref及4C43BF nop的非call普通流入分别登记，普通call0。
原按钮内联复制保持，不增加StrCpy/StrLen入口统计。仅源六callee、一共享旧项/四新父：semantic841→845，其他840/pending18/分类4保持。
业务249→255/1022，其他767未计完成；新增四文案常量，其他已过接线/全CRT启动和69/70运行不据此收口。
局部合同/唯一进度只末追加并集中限定静态核对，不构建、测试、运行格式/商店算法或样例、运行客户端或SHA/hash校验，不刷新总SPEC/索引。


## 外观确认/图号回复两个真实格式点（2026-10-03）

局部SPEC仅4C5C30@4C5C8C、4C5F80@4C5FC8两callee接真实53B304，本线程按保留父正文方案实施。
%d原5531A4字节256400/S32 image或D2F7AC、52B buffer及256B text/escaped未清局部/忽略返回保持。
前者4C5650计算后格式，再按原target/request/Y/X和button1发送；后者格式后255转义，再按原target/request/Y/X和原button发送。
两父119统计，只读50格式邻窗，余69/核心不重验；两data xref不计普通call，所有门禁/早退/关闭/通知/音效/返回正文不改。
格式255→257/1022，其他765仍待；semantic845→847，一共享旧项/两新父，其他844/pending18/分类4和原常量集合保持。
三局部合同/唯一进度末追加并集中限定静态核对，不构建、测试、运行格式/图号/转义/封包算法或样例、运行客户端或SHA/hash校验，
不刷新总SPEC/索引，不恢复暂停音频或改加解密，不据此验收完整父路由/网络帧/CRT/运行。


## 技能选择/确认九个真实格式点（2026-10-03）

局部SPEC仅4C09A2/4C0B45/4C0E26三个选择、4C1016确认回传和4C115D/4C11B1/4C1217/4C12BE/4C1300五显示callee接真实53B304，本线程实施。
遗忘selected+1、宠物selected、学习selected及发布时点、三个16B文本/独立16B容量15转义、确认16B替换槽无转义回传保持。
五显示原%s/%7d字节借名称或独立S32金币/报价，256B未清buffer/忽略返回和即时测宽绘制不改；第二行固定588320示例名测宽保持。
新增588344=bec7b2dfa1792573a17a00，588338=c5fda1792573a17a00，58830C=a8cfa5cea1792573a17aaabaa6ecb86d00，
5882D4=a9d2abf9aaf7a14020253764aaf7b9f400，5882C0=c1caa44aaaf7c34220253764aaf7b9f400；旧%d不重复登记，空槽复制不算新增格式/StrCpy入口点。
四父713统计，只读236格式邻窗，余477/核心不重验；六data xref不计普通call。仅源九callee、一共享旧项/四新父，semantic847→851，其他846/pending18/分类4保持。
格式257→266/1022，其他756仍待。两合同/唯一进度只末追加并集中核原点/字节/参数/宽度/缓冲/测宽与发送时点和限定差异，
不构建、测试、运行格式/技能/封包/测宽算法或样例、运行客户端或SHA/hash校验，不刷新总SPEC/索引，不恢复暂停音频或把本步记为完整技能/网络帧/CRT运行验收。


## 技能列表与金额生产真实格式（2026-10-03）

局部SPEC仅六父十callee接真实53B304，本线程采用保留原正文方案。4BE6C8的%2d/%2d保持两S32参数及格式后第二次独立total读取；
4BE8C1/4BEA18的%10d仍写0，后者原4BE939 xor ebx,ebx已核；4BED19仍写当次所选报价，三个D45F78路径不合并。
4BF265/4BF289、4BF3ED/4BF411两组%2d保持各自字段与S8→S32差异，两个10行/11B槽owner不变；
4BF591/4BF5B0的%5d/%10d保持5行/11B槽与未分页存在性/分页内容差异，原之后金币取样和price重读设色不改。
新增58645C=2532642f25326400、5863C0=25356400，%2d/%10d已有证据不重复；借用元素text/现12B余额前缀、110B/55B文本与原首字节清理/尾区保持。
原1005条仅196格式/零值定义邻窗读取，余809/核心不重验；27普通call入向只记引用，现void签名/名称内联复制/回调路由不改。
仅源十标识符、一共享旧项/六新父：semantic851→857，其他850/pending18/分类4保持；业务266→276/1022，其他746仍待。
两合同/唯一进度只末追加并集中核新原点/字节/宽度/共享owner/独立读取/行步长和限定差异，不构建、测试、
运行格式/列表/取值算法或样例、运行客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整技能/数据生产/CRT/运行。


## 技能模式刷新与主更新真实格式（2026-10-03）

局部SPEC仅4BE46E/4BE56C/4BE58E、4BEF9A/4BF037、4BF70C及4BF903七callee接真实53B304，本线程直接实施。
保留第一次total取样后独立重读、CC2E68所需槽位原26B文案、CF4A3C/所选报价/候选capacity与used数/slot.value04/玩家金币S32来源，
元素借用text、D45F78与C9A9A4现12B前缀/C8B318+row*11及忽略返回、格式前后清首字节/模式/消息/回调时点全不改。
仅新增5881A0字节a9d2bbddaabaa7deafe0c4e6a6ecbcc6b671a147253264c4e600，原%2d/%2d/%10d/%3d不重复；inline hex字面量逐字保持。
四父518统计只读158格式邻窗，余360/核心不重验；九普通call/一data入向只登记，三void/一S32签名保持。
仅源七标识符、一共享旧项/四新父：semantic857→861，其他856/pending18/分类4保持；业务276→283/1022，其他739仍待。
两合同/唯一进度只末追加并集中限定静态核对，不构建、测试、运行格式/刷新/槽位算法或样例、运行客户端或SHA/hash校验，
不刷新总SPEC/索引，不据此验收完整技能/数据生产/父路由/CRT/运行。


## 商店53八个原格式点（2026-10-03）

局部SPEC仅将4BC230/4BC660/4BD100九个std::sprintf表达式接真实53B304，本线程裁决保留现正文方案实施。
4BC5CC/4BCBE7的[kok]/[han]两路径各共一原call，现两互斥表达式动态一次；前父完整U32 flags与后父玩家W16 flags各自保持。
4BCD0F/4BCE6F为两模式组独立循环的S32 count>0数量，两现表达式虽同文也按两原点登记，不按20槽扩大引用。
4BD33C/4BD354保持factor非0含负值或等0的%2d X %2d/X %2d，S32参数顺序不改。
4BD449/4BD493两12B金额记录共用现循环表达式，visible后先S32 value再S16坐标/4771B0，CB1E40及共享C8B088/按钮提示owner保持。
六格式常量均已有字节证据，不新增或重读；256B/52B未清char buffer、即刻复制/绘制、各次取样及其余正文/头/核心不改，原内联复制/扫描不计StrCpy/StrLen入口。
三父1270统计只读182格式邻窗，余1088不重验；入向三data/普通call0，仅登记引用，不冒充实际回调槽验收。
一共享旧项/三新父使semantic861→864，其他860/pending18/分类4保持；格式283→291/1022，其他731仍待，常量61保持。
两合同/唯一进度末追加，集中静态核新原点/callee/共享分支与金额循环分组/原字节/宽度/owner/取样及限定差异，
不构建、测试、运行格式/商店/绘制算法或样例、运行客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整53/CRT/运行。


## 窗口54默认／取消回传一个原格式点（2026-10-03）

局部SPEC由本线程裁决：4BFCB0@4BFDB9的mode1仅一callee接真实53B304，S32常量99、5531A4%d字节256400、16B未清text/escaped与52D2D0容量15保持。
target/request→原Y/X→512620按钮0/escaped→重取窗口state|8000顺序及其余模式/生命周期/音效后mode重取/图号/返回逐字不动，不恢复音频或加解密叶。
原174条统计，只读32邻窗，余142/已过核心和正文不重验；入向一data/普通call0，不冒充复合回调槽验收。
整份源一标识符、S32签名/头保持；semantic864→865，其他863/pending18/分类4保持；格式291→292/1022，其他730仍待，常量61保持。
两合同/唯一进度仅末追加，集中静态核新原点/既有原格式字节/S32/缓冲/转义与消费者顺序/限定JSON及差异，
不构建、测试、运行格式/转义/回传算法或样例、运行客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整54/CRT/运行。


## 资料查询至邮件回复九个原格式点（2026-10-03）

局部SPEC由本线程裁决：4CA1A0/4CA6F0/4CA9C0/4CB2F0/4CB6A0仅七现std::sprintf callee接真实53B304，其余正文不动。
原点4CA261；4CA7B9/4CA886/4CA953；4CAAAC；4CB334；4CB6EF/4CB7EB/4CB896共九，关键词/翻页三独立原call由一现表达式承接，按按钮32/16/1对应，不按表达式数漏计。
新增588524/58854C/588558的%d|%d|、%d|%d|%s、%d|%s与5885A8的%d|%s|%s|%d、587768的时间戳格式及588588原编码费用格式六含NUL字节，具体字节登记同批局部合同/JSON。
S32字段/常量0、借用已转义关键词/名称/正文、原256/1024/64B未清局部与转义容量256/255/1024/1023、target/request-Y-X发送及后续独立重读取样保持。
费用hover/click各自取值不缓存合并；联系人命中后time32/localtime按原指针偏移取S32，year+1900/month+1和原正文记录/清理顺序保持，不验time/联系人本体或恢复音频。
五父107/195/117/100/187共706统计，读取36/120/52/36/153共397格式/参数定义/消费者邻窗，余309/已过正文与核心不重验；五data/普通call0，不推复合回调槽。
整份源七标识符与五S32签名保持；semantic865→870，仅共享旧项/五新父，其他864/pending18/分类4保持；格式292→301/1022、其他721仍待，常量61→67。
四合同/唯一进度仅末追加并集中限定静态核对，不构建、测试、运行查询/邮件/格式/转义/时间算法或样例、客户端或SHA/hash校验，
不刷新总SPEC/索引，不据此验收完整60/61/62/63生产/路由/CRT/失败/运行。


## 黑名单删除提示一个原格式点（2026-10-03）

局部SPEC由本线程裁决：4CC6E0两互斥std::sprintf callee接真实53B304，Contains/Remove的三结果格式在原端共用4CC876一次call，现动态一次，按一原点登记。
58863C/588620/58860C三含NUL原编码格式分别登记成功/失败/不存在，均只借17B name；128B message与既有清理保持，立即44B050(message,4,3)。
原mode/strcmp门槛、页号/记录删除后重读、context清理/输入原型/焦点/hover/图号/返回及内联复制扫描不动，不算StrLen/StrCpy入口新引用。
原223统计只读40格式分支/消费者邻窗，余183/核心不重验；一data/普通call0仅登记，不推实际回调槽。
整份源两标识符与S32签名保持；semantic870→871，其他869/pending18/分类4保持；格式301→302/1022、其他720仍待，常量67→70。
两合同/唯一进度仅末追加，集中限定静态核新原点/callee/分支分组/三原字节/借用源/缓冲/立即消费者与后续重读取样/限定JSON及差异，
不构建、测试、运行黑名单/格式/输入算法或样例、客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整65/CRT/运行。


## 摆摊名称与宠物携装拒绝两个原格式点（2026-10-03）

局部SPEC由本线程裁决：4CE700@4CE841、4CFCA0@4CFD44各一现std::sprintf callee接真实53B304，其余正文不动。
前者仅result0/非空D67BE4借摊名%s，33B局部默认9B含NUL前缀、动作位后24B清理/空名称回退与原玩家摊名复制保持；
后者588900为31B原编码文案、零转换/零变参，128B未清message立即44B050后返回，六槽携装门槛和无携装价格输入路径保持。
已有5535F8的%s字节257300不重读，仅新增588900原文案字节；不算原内联扫描/复制为StrLen/StrCpy入口引用。
两父134/52共186统计，只读42/27共69新邻窗，余117/已过正文和核心不重验；三普通call入向只登记，完整调用父/绑定未验。
整份源仅两标识符、两void签名/头保持；semantic871→873，其他870/pending18/分类4保持；格式302→304/1022，其他718仍待，常量70→71。
三合同/唯一进度末追加并集中限定静态核对，不构建、测试、运行名称/摆摊/携装/输入/格式算法或样例、客户端或SHA/hash校验，
不刷新总SPEC/索引，不据此验收完整67/68生产/父路由/CRT/失败/运行。


## 摆摊价格与购买详情十七原格式点（2026-10-03）

局部SPEC由本线程裁决：4D2260一callee、4D2840十六callee接真实53B304，各与原点1:1，不按输入循环或属性绘制扩大计数。
S32价格/物品正count、借用宠物名/合法种族名、两个独立CryptInt返回与+8/+70/+74/+78/+6A4/+7C/+80/+98/+6C完整S32保持；
种族+4按S16负值/20边界取表名或仅清首字节仍绘制，物品/宠物价格在各原点重新读选择号，原预偏移数组别名不动。
20B buffer/32B details未清，各格式后立即原绘制；布局初始mode快照、购买/取消后窗口重取/隐藏、正文mode再读、输入/数键/原限制/焦点/图形/返回及其余正文不改。
四既有%d/%3d/%s/%4d原字节不重复读取，常量71保持；两CryptInt原callee401EA0到既有401EC0只沿已证关系，不重验核心。
两父435/672共1107统计，读取20/309共329新格式/字段定义/消费者邻窗，余778/已过正文与核心不重验；入口无入向xref不推未调用或实际绑定已验。
整份源十七标识符、两S32签名/头保持；semantic873→875，其他872/pending18/分类4保持；格式304→321/1022，其他701仍待。
三合同/唯一进度仅末追加并集中限定静态核对，不构建、测试、运行价格/输入/购买/详情/格式算法或样例、客户端或SHA/hash校验，
不刷新总SPEC/索引，不据此验收完整67/68生产/父路由/CRT/失败/运行。


## 晶石商店数量与回复七原格式点（2026-10-03）

局部SPEC由本线程裁决：4C1AA0/4C1B00/4C2060/4C39A0/4C3D40/4C3DA0/4C4270各一现callee接真实53B304，
原点4C1ADD/4C1B3C/4C20E8/4C3A34/4C3D7D/4C3DDC/4C42F8，不按行/记录数量扩大原点。
五行11B文本步长/首字节清理、S32 selectedCount/total/remaining或index/quantity、独立owner及原0xC54记录步长保持；
69格式后total→owned与70 owned→total独立取样不互换，剩余先visible后重读scroll/remaining并借元素text原指针。
两回调按value低U8判定，quantity!=0包含负值；原32B/256B未清局部、text仅清首字节、分隔符/格式/追加/计数与转义255/按钮0或1/发送后窗口重取保持，不算内联StrCat新引用。
仅新增585DCC含NUL12B原格式字节b3d1be6c2025326420add300，其他三已有格式不重读；全部正文/七void签名/头和核心保持。
七父392统计只读241新邻窗，余151/已过正文不重验；九普通call/两data入向只登记，完整父/实际复合回调槽未验。
semantic875→882，仅共享旧项/七新父，其他874/pending18/分类4保持；格式321→328/1022，其他694仍待，常量71→72。
两合同/唯一进度末追加并集中限定静态核对，不构建、测试、运行数量/总额/回复/转义/格式算法或样例、客户端或SHA/hash校验，
不刷新总SPEC/索引，不据此验收完整69/70/CRT/失败/运行。


## 商店53数量／价格生产七个原格式点（2026-10-03）

局部SPEC由本线程裁决：六父七std::sprintf callee接真实53B304，原点4B8B77/4B8C2C/4B904B/4B90CC/4B9122/4B9280/4B9299。
两已有%2d/%10d原字节不重读；S32数量/价格/总额、CB03DC有符号S8槽位与CF4FE0步C58共享owner、五行11B首字节清理/可见/重复slot与maximum取样保持。
映射槽总额price→selected与used列表selected→price原次序分开，价格后名称边界/分行不动；
4B9270两格式分别读取D046C4，再price与第三次数量读取后发布total，不合并缓存。
六父269统计只读127新格式/字段取样/消费者邻窗，余142/已过正文/核心不重验；43普通call入向只登记，两parent未知仍null，完整父/绑定未验。
整份源七标识符、六void签名/头保持；semantic882→888，其他881/pending18/分类4保持；格式328→335/1022，其他687仍待，常量72保持。
两合同/唯一进度末追加并集中限定静态核对，不构建、测试、运行数量/总额/价格/名称/格式算法或样例、客户端或SHA/hash校验，
不刷新总SPEC/索引，不据此验收完整53生产/父路由/CRT/失败/运行。


## 商店53主更新与确认回复二十原点（2026-10-03）

局部SPEC由本线程裁决：三父各三现callee接真实53B304，承接3/5/12共二十原call。
回复三%d|%d模式点→一表达式，确认五无变参标题→一表达式、mode2标题借CB0184%-24s另一个、六问题→一表达式；原分支动态一次，不按表达式或循环数替代原引用数。
S32 money/available/amount与独立重读、selection低U8/mode S8、512B同源text/escaped偏移256/32B fragment、
mode3转义256和其余255、C58同源used/非零selected含负值/各index源与拼接-发送-窗口重取次序保持。
六模式原编码标题/问题的空格/NUL及借名最小宽度不截断，原overlayReady/4-5后段textMode重读/共享回调/按钮提示和状态发布、前段/正文/头/核心均不动，不恢复音频。
只新增12原格式常量（1|%d、六标题、五问题），其他四已证原字节不重读；原4B9B36 switch/六x86表项24B仅静态核分组。
三父1411统计只读367新邻窗，余1044/已过前段不重验；七data/普通call0仅登记，实际复合回调槽与完整父未验。
整份源九标识符、两S32/一void签名保持；semantic888→891，其他887/pending18/分类4保持；格式335→355/1022，其他667仍待，常量72→84。
两合同/唯一进度末追加并集中限定静态核对，不构建、测试、运行主更新/回复/确认/转义/格式算法或样例、客户端或SHA/hash校验，
不刷新总SPEC/索引，不据此验收完整53/CRT/失败/运行。


## 晶石69剩余一个原格式点（2026-10-03）

局部SPEC裁决仅将4C1700@4C1794一callee接真实53B304；原585DCC字节/S32 remaining、同源记录/元素text借指针、先隐藏/used和非负门槛/先visible后重读scroll-窗口-remaining顺序保持。
50统计只读33邻窗，余17/已过正文/核心不重验；一普通call只登记、完整父未验，整份源仅一标识符/void签名。
semantic891→892，其他890/pending18/分类4保持；格式355→356/1022、其他666仍待，常量84保持。
两合同/唯一进度仅末追加并集中限定静态核对，不构建、测试、运行算法或样例、客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整69/70/CRT/运行。


## 宠物负重一个原格式点（2026-10-03）

局部SPEC裁决仅将4C69B0@4C6A89一callee接真实53B304；588414原编码13B字节、maximum先weight后S32取样/压参weight-maximum、元素text借指针及格式后text/U8字体重读测宽/居中保持。
96统计只读30邻窗，余66/已过前段/核心不重验；两普通call仅登记，完整父未验；整份源一标识符、void原生指针签名/构析与退出登记保持。
semantic892→893，其他891/pending18/分类4保持；格式356→357/1022、其他665仍待，常量84→85。
两合同/唯一进度仅末追加并集中静态核对，不构建、测试、运行算法或样例、客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整选择器/构析/CRT/运行。


## 仓库计数与余额六个原格式点（2026-10-03）

局部SPEC裁决仅将4A9B00@4A9B28/4A9B82/4A9BC8/4A9C23/4A9C7B/4A9CD4六callee接真实53B304。
四W16 used计数产生S32、money与side的原S32余额、20B未清局部复用与逐次窗口重读/4771B0显示顺序保持；两既有原格式字节不重读，原返回0/正文与头/核心不动。
163统计只读131邻窗，余32/已过正文不重验；一data/普通call0不推绑定。semantic893→894，其他892/pending18/分类4保持；格式357→363/1022、其他659仍待，常量85保持。
两合同/唯一进度只末追加并集中限定静态核对，不构建、测试、运行算法或样例、客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整仓库/CRT/运行。


## 仓库存取款六个原格式点（2026-10-03）

局部SPEC裁决将4ABB20五现callee接真实53B304：G/D两原点共一现表达式，四%9d原点与四现表达式对应。
S32 state/last取样、384B未清同源buffer偏移128/转义255、userState-value14-Y-X发送后重取窗口/确认值/side再清state、两11B显示区/各自side重读顺序保持；原最大值/未初始化局部/返回0和正文不动。
新增587958/587950/5863A0三原格式字节；233统计只读103新邻窗，余130/已过前段/核心不重验；一data/普通call0不推绑定。
semantic894→895、其他893/pending18/分类4保持；格式363→369/1022、其他653仍待，常量85→88。两合同/唯一进度末追加并集中限定静态核对，不构建、测试、运行算法或样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整窗口41/非法路径/CRT/运行。


## 人物面板474FB0真实格式七点（2026-10-03）

局部 SPEC：仅将七现std::sprintf接FormatLegacyString_53B304。4753E5/4755C3/47561C/475675/4756E9/475733/47584B各对应独立原调用；553FCC=%3d、5863A0=%9d、553FE8=%4d/%4d沿既有字节，新增586390=x%d（78256400含NUL）。256B未清复用，七点S32；先maxHp再hp/先maxFp再fp、nextExp哨兵比较与后续读取分开、格式后当前窗口元素重取/扫描复制及penalty>0复制后可见发布保持。void签名与其余正文/核心/头文件不动，内联扫描复制不增加其他CRT入口调用点。

651统计/186新邻窗，余465/已过正文/核心不重验；474540@474540一普通call仅登记。semantic895→896、其他894记录/pending18/分类4保持；格式369→376/1022、余646，常量88→89。两合同与唯一进度末追加并集中限定静态核对，不构建、测试、运行算法/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整窗口7/父路由/CRT/运行。


## 技能窗口47A7A0真实格式十点（2026-10-03）

局部 SPEC：仅六现callee接真实53B304，原十点按mode0/3单槽位格式、mode1/4各三格式、mode2单等级及mode5/6各一战斗FP独立登记；十点共六现表达式，不以现表达式数替代原调用点数。保留256B未清同源数字/描述buffer、W16 used/S16 index/S8 reqSlot累加及全S32变参；经验先next后experience、等级先max后current、FP先max再current且分别重读位置/mode5分别GetPetPosition，格式消费者的skill/窗口重取、借元素7文本与签名/返回1/其他正文保持。

原701仅统计，读取357新格式/参数/消费者及switch/模式0共享路径指令，余344/已过正文/核心不重验；47A82F七case/47B0C4原28B表和47A8CC→47ACA2确认十原点/六现表达式分组，新增586464含NUL原字节/宽度/取样与消费者次序/整份源限定静态核对；54E420一data/普通call0仅登记，不推绑定或完整窗口10验收；未构建或执行。新增586464=%4d/ -- 原9B2534642f202d2d2000，其余三常量沿既有字节。semantic896→897、其他895/pending18/分类4保持；格式376→386/1022，其他636待，常量89→90。两合同/唯一进度末追加并核本次差异；不构建、测试、运行算法/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整窗口10/父路由/CRT/运行。


## 制作材料请求4810A0真实格式六点（2026-10-03）

局部 SPEC：仅六现callee接真实53B304，六原点481681/4816B0/481717/4819B1/481B04/481CBB保持独立。两个256B局部、既有mode13首字节/尾部清理、两遍六槽material>=0先/<0后、全部S32变参与%d|%d普通项在前特殊项在后、slot重读及未赋初值分组路径保持；不新增补值，发送请求按消费时选择S32或S8宽度、target与后续mode/name/隐藏顺序及S32签名/返回1/其余正文保持。内联追加/复制不增加其它CRT入口引用，原网络/输入/计时器等本体不动。

原1117仅统计，只读198新格式/参数/追加及发送前段邻窗，余919/已过正文/核心不重验；六原点/callee/六现表达式/新增586880含NUL字节/S32变参和两遍材料顺序/既有局部清理及未初始化路径/整份源限定静态核对，54E450一data/普通call0仅登记，不推绑定；未构建或执行。新增586880原4B7c256400（|%d含NUL），5531A4/58687C/586874沿既有字节；semantic897→898、其他896/pending18/分类4保持；格式386→392/1022，其他630待；常量90→91。两合同/唯一进度末追加并限定静态核对，不构建、测试、运行组包/格式/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整窗口11/制作/父路由/CRT/运行。


## 图鉴详情492F70真实格式五点（2026-10-03）

局部 SPEC：仅五现callee接真实53B304，五原点493169/4931CE/493273/4938FE/49394E保持独立。256B未清复用buffer、S32索引+1/字段+30H/原960分母、每次索引与记录重取保持；有记录元素3/4先取得elements、扫描后取得data，有记录元素6及空记录元素3/6扫描前取得data，沿原扫描复制和当前窗口别名时序，不改void签名及其余正文/辅助函数/核心/头文件，内联复制不加其他CRT入口点。

原664仅统计，读取156新格式/参数/消费者邻窗，余508/已过正文/核心不重验；五原点/callee/五现表达式/新增586DE0含NUL字节/S32及原960/256B未清局部/扫描前后目标取得次序/整份源限定静态核对；一普通call492410@492421仅登记，不推完整父路由；未构建或执行。新增586DE0原8B2533642f25336400；553FCC/553FB4沿既有字节。semantic898→899、其他897/pending18/分类4保持；格式392→397/1022，其他625待；常量91→92。两合同/唯一进度末追加并限定静态核对，不构建、测试、运行格式/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整窗口20/图鉴/父路由/CRT/运行。


## 战斗命令498520真实格式六点（2026-10-03）

局部 SPEC：仅六现callee接六原点49898C/4989E3/498A30/498CF6/498F2B/498F90。四命令保留U32位模式及原参数/取样/发送→Flush→状态顺序；借原name T*，名称绘制后重新取宠物与技能再S32费用格式，两组格式后Y/X重读和S16坐标消费保持。void签名/未清256B std::array/其余正文及音频暂停边界不动。

五新含NUL原字节：5540A0=487c255800；553FF0=537c25587c25587c255800；554098=497c25587c255800；554060=577c25587c255800；553FE0=252d31367300；553FDC沿既有证据。原761仅统计，只读193新格式/参数/发送-状态及绘制消费者邻窗，余568/已过正文/核心不重验；六原点/callee/六现表达式/五新原格式字节/U32-S32-T*与未清256B局部/取样和消费次序/整份源限定静态核对；一普通call43CBB0@43D05A仅登记；未构建或执行。semantic899→900、其他898/pending18/分类4保持；格式397→403/1022，其他619待；常量92→97。两合同/唯一进度末追加并限定静态核对，不构建、测试、运行组包/格式/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整战斗/父路由/CRT/运行。


## 战斗物品499C30真实格式五点（2026-10-03）

局部 SPEC：仅五现callee接499F99/49A0A0/49A55D/49A682/49ABF3五原点。12B countText和256B buffer未清，S32 count正值门槛与两套绘制坐标保持；两Q原U32 255/hovered+8且窗口/选择先写、发送后actionState，I仅43C560返回0分支重读position/先写selectedItem再U32参数组包、发送→Flush→actionState。S32签名/consumed返回/其余正文和音频暂停边界保持。

新554048含NUL原6B517c25587c00，553FCC/554098沿既有字节。原1175仅统计，只读195新格式/参数/绘制与发送状态邻窗，余980/已过正文/核心不重验；五原点/callee/五现表达式/新554048含NUL字节/S32与U32/原255/12B及256B未清局部/状态和消费者次序/整份源限定静态核对；4C6EC0@4C701A与56C430两data/普通call0仅登记，不推实际绑定；未构建或执行。semantic900→901、其他899/pending18/分类4保持；格式403→408/1022，其他614待；常量97→98。两合同/唯一进度末追加并限定静态核对，不构建、测试、运行物品/组包/格式/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整物品/父路由/CRT/运行。


## 窗口13结果4876B0真实格式十二点及补证计数（2026-10-03）

局部 SPEC：只改十一现callee，承接十二原点；kind0/1共享一现表达式但有4876E0/487733两个原点，487879是已证错位字节中的独立call。原代码范围非IDA函数且inventory_id=null，七错位head以23B六指令补解码，原函数目录/数据库不改。

保留window/返回T*、借原name/text、S32字段门槛、元素7先NUL再三增量顺序、256B未清buffer、直接显示与内联追加及尾部动作/返回/全部正文和音频暂停边界。八含NUL原字节：586B00=b9ef252d32327300；586AAC=252d313673aabaaa76c0f8a6a8a55ca44600；586A94=a140a5cda952adc8abecb45fa44620256420a14300；586A34=252d313673aabaaa76c0f8a5a2b1d1a44600；58660C=a7deafe0b867c5e7adc8202b256400；586600=a140ad40a44f202b256400；5865F4=a140c646a5a9202b256400；5865E8=a140b4bca44f202b256400，源码常量逐项静态一致。IDA原范围324代码head统计；487867..48787E七错位head按原23B六指令纠正，实际323原指令。只读259head（含七错位）及六临时解码，有效258指令，余65/已过正文/核心不重验。十二原点/十一现callee分组/八新原格式字节及源码常量/S32-T*和原owner/顺序/整份源限定静态核对；一普通call487AE0@487AED仅登记；未构建或执行。语义记录901→902（新增已证代码范围记录，IDA函数目录不变），其他900记录/pending18/分类4及顶层保持；真实已接408→420，其中419个IDA直接引用+1个原字节补证；原直接引用总数1022不变，已确认原调用总数1023，余603已证原调用仍待；格式常量98→106。后续覆盖统一按已证原调用池1023记账，同时保留原1022直接引用和补证明细，新增补证必须有原字节/解码/引用事实，不推全池已完成。不构建、测试、运行格式/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整窗口/父路由/CRT/运行。


## 宠物状态48F8F0真实格式九点（2026-10-03）

局部 SPEC：仅九现callee接九原点。U32签名/含NUL长度返回、256B未清buffer/全S32变参、原S16 pair索引和每次选项重读保持；401D00哨兵比较后原401EA0读取不合并，next→exp/maxHp→hp/maxFp→fp及同pet maxWeight→weight顺序保持。经验/HP/FP三个元素扫描后data，其余五显示扫描前data，最终页号在占用/空记录路径之后仍输出选项+1/5，全部正文和既有桥保持。

四新含NUL字节：586D60=2538642f202020202020202d00；586D58=2538642f25386400；586D50=2535642f25356400；586D24=2536642f25366400，其余三常量沿既有原证据。原1003仅统计，只读316新格式/参数/原取值目标与复制消费者邻窗，余687/已过正文/核心不重验；九原点/callee/九现表达式/四新原格式字节/S32与原5/未清256B局部/取样和扫描前后目标时序/整份源限定静态核对；一普通call48D9A0@48DAA3仅登记；未构建或执行。语义记录902→903，其他901记录/pending18/分类4及顶层保持；已接真实调用420→429/已确认1023，余594；其中428个IDA直接引用及1个既有原字节补证，原1022直接引用和补证/已确认总数字段保持；格式常量106→110。两合同/唯一进度末追加并限定静态核对，不构建、测试、运行宠物/格式/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整窗口/父路由/CRT/运行。


## 窗口12结果与技能结果详情十一原格式点（2026-10-03）

局部 SPEC：485AF0五现表达式接四原点，升级%s/T*与经验S32共享485CAF；4EEA40五现表达式接七原点，四健康零变参格式共享4EEDF8/两表达式，四数值原点共享appendValue表达式。总十一原点/十现callee，只改callee。原T*输入/返回与owner、256B未清buffer、首NUL后三增量追加、窗口12字面+%d与窗口13%+d不同原格式、<28补LF/lines<8/升级自带LF与发送结果消费/尾动作和音频暂停边界保持；完整流程不据本步验收。

新十含NUL格式：5898B8=b6cbb6d5b94cabd7b463a4c6a141c646bbeeb1bca446a440aee6a14300；5898A8=b6cbb6d5b463a4c6a446a14300；589820=a7deafe0b867c5e7adc820252b6400；589814=b5a5afc5a457a4c9a1490a00；5899D4=a65eb45fa4462564c249a5cda952a44fa14300；589940=a65eb45fa446b0b7b164aaacba41a14300；589928=a65eb45fa446a4d6b35cb0b7b164aaacba41a14300；589808=ad40a44f20252b6400；5897FC=c646a5a920252b6400；5897F0=b4bca44f20252b6400；%s源参数58661C=b5a5afc5b4a3a4c9a14900，源码常量均静态一致。原1026指令统计/392新指令，余634/已过正文/核心不重验，两普通call仅登记。语义记录903→905，其他902记录/pending18/分类4及顶层保持；已接真实调用429→440/已确认1023，余583，其中439个IDA直接引用及1个既有原字节补证，原1022直接引用/补证/已确认总数字段保持；常量110→120。三合同/唯一进度末追加并限定静态核对，不构建、测试、运行结果/格式/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整结果/窗口/父链/CRT/运行。


## 制造与物品作业结果47E5E0/47F3D0真实格式八点（2026-10-03）

局部 SPEC：只改八现callee接八独立原点（各四）。沿四既有格式字节，window/返回T*、元素11 S32经验、元素12先NUL后256B未清局部三增量/字面+%d顺序保持。47F3D0门槛D2F328/CA0448/CB0F84仍取制造回复，显示CF46B4/CB0A14/D30418仍取物品回复；不合并来源。尾部47D170/门槛47D220/模式2或5发布/真实last返回、全部正文与音频暂停边界保持。

原451+498=949仅统计，读取105+107=212新指令，余346+391=737/已过正文/核心不重验；两普通call47EBE0@47EBEA和47FAC0@47FACA只登记。语义记录905→907，其他904记录/pending18/分类4及顶层保持；已接真实调用440→448/已确认1023，余575，其中447个IDA直接引用+1个既有原字节补证；原1022直接引用/补证/已确认总数字段与120个格式常量保持。两合同/唯一进度末追加并限定静态核对，不构建、测试、运行结果/格式/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整制造/物品/窗口/父路由/CRT/运行。


## 宠物详细属性490780真实格式二十七点（2026-10-03）

局部 SPEC：仅四现callee接27原点（writeNumber单表达式24点，HP/FP/页号各一点）；24数值原字段地址相对F633F8的偏移和消费元素逐项匹配，不把lvFlag+6C缩成W16。U32签名/含NUL返回、256B未清buffer/全S32变参、每次S16 pet pair重读、原24字段与元素顺序、最大值先取/原5/两占用路径后页号、24数值与页号扫描前data及HP/FP扫描后data/原输入取样差异/全部正文保持。

原1388仅统计，取证输出超过工具限制后改六段限定邻窗，读取722唯一新指令，余666/已过正文/核心不重验；二十七原点/callee/四现表达式-24点writeNumber分组、24字段原地址偏移/元素及扫描前后阶段逐项匹配、三已证格式字节/S32与原5/取样/整份源限定静态核对；一普通call48DAB0@48DBA3只登记；未构建或执行。三格式沿既有原字节；语义记录907→908，其他906记录/pending18/分类4及顶层保持；已接真实调用448→475/已确认1023，余548，其中474个IDA直接引用+1个既有原字节补证；原1022直接引用/补证/已确认总数字段与120格式常量保持。两合同/唯一进度末追加并限定静态核对，不构建、测试、运行宠物/格式/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整宠物/窗口17/父路由/CRT/运行。


## 状态选择49F7E0真实格式三点（2026-10-03）

局部 SPEC：三现callee仅承接三原点49F88E/49F90E/49F947。借name T*/level及四HP-FP S32，name前十一空格/四数值前二十八空格、最多六行/原首NUL清理与name非空门槛、名称格式后record颜色/图标重取、等级格式后record重取与maxFp→fp→maxHp→hp取样、原窗口text owner/void-S32签名/全部正文保持。

新两含NUL字节：587410=2020202020202020202020257300；5873E4=202020202020202020202020202020202020202020202020202020202536642f253664202536642f25366400；586BBC沿既有证据。原169仅统计，只读83新格式/参数/重取与状态消费者邻窗，余86/已过正文/核心不重验；三原点/callee/三现表达式/两新含NUL格式字节/T*与S32/空格和取样-字段重取次序/整份源限定静态核对；一普通call49F7B0@49F7BD只登记；未构建或执行。语义记录908→909，其他907记录/pending18/分类4及顶层保持；已接真实调用475→478/已确认1023，余545，其中477个IDA直接引用+1个既有原字节补证；原1022直接引用/补证/已确认总数字段保持；格式常量120→122。两合同/唯一进度末追加并限定静态核对，不构建、测试、运行状态/格式/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整状态/窗口/父路由/CRT/运行。


## 采集4ED6B0与制造4F39F0回复详情十二原格式点（2026-10-03）

局部 SPEC：只替换六现callee接十二原点（各六，appendValue每父四点共一表达式）；S32 result/借text T*/实际WindowAnimationData*返回、256B未清局部、%+d及<28补LF/lines<8/升级原LF与追加后SE64、字段解析和详情owner/尾创建保持。采集result0原零变参Nothing/16B追加-清字段-尾LF次序不动；制造buffer/lostItem各256B、六字段255提取/原解转义/空串break/借buffer T*格式后再lostItem<28补LF-追加，以及末伤病文本不新增格式保持。

新两含NUL原字节：5897DC=a853acdda8eca5f4a6f3aa46a6e8a1430a00；589F9C=257320a5a2a568a446a14900，源码常量一致，其余五格式沿既有证据。原410+453=863仅统计，只读198+198=396新指令，余212+255=467/已过正文/核心不重验；两普通call50C7F0@50C862/50C81A只登记。语义记录909→911，其他908记录/pending18/分类4及顶层保持；已接真实调用478→490/已确认1023，余533，其中489个IDA直接引用+1个既有原字节补证；原1022直接引用/补证/已确认总数字段保持；格式常量122→124。三合同/唯一进度末追加并限定静态核对，不构建、测试、运行回复/格式/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整回复/窗口/父路由/CRT/运行。


## 库存488FB0真实格式两点三现表达式（2026-10-03）

局部 SPEC：三现callee接4895E0共用悬浮原点与489714 count原点；借name/memo T*、memo测长与flags100/200两分支、[kok]/[han]原字符、256B未清同源buffer与D68AE8内联消费、S32正count及S16绘制坐标、S32(index,state)/consumed返回/全部正文和音频暂停边界保持。三原格式586BCC/586BC4/553FCC沿既有证据，不加新门槛/保护/清理或改变owner。

原588仅统计，只读70新格式/参数/消费邻窗及10个memo地址-测长指令，共80，余508/已过正文/核心不重验；两原点/三现callee-两T*格式共享分组/三已证原字节/Word门槛与S32、同源buf和消费次序/整份源限定静态核对；七代码区data及562CD8一data共八data/普通call0只登记，不推绑定；未构建或执行。语义记录911→912，其他910记录/pending18/分类4及顶层保持；已接真实调用490→492/已确认1023，余531，其中491个IDA直接引用+1个既有原字节补证；原1022直接引用/补证/已确认总数字段与124格式常量保持。两合同/唯一进度末追加并限定静态核对，不构建、测试、运行库存/格式/样例/客户端或SHA/hash校验，不刷新总SPEC/索引，不据此验收完整库存/窗口/父路由/CRT/运行。


## 宠物列表48C920三处真实格式（2026-10-03）

局部 SPEC：48CAF5/48CB72/48CBF2分别接等级S32、HP当前/上限S32、FP当前/上限S32；553FCC和586D24沿已登记原字节，不重读或新增。仅接48CAF5/48CB72/48CBF2三原53B304点，void()与整份正文保持。共用未清256B buffer；五行排序索引对只固定地址，各次索引以S16重读。等级+08保持S32，HP/FP各先重取编号读最大值再重取编号读当前值，原401EA0到既有401EC0实现桥保持；格式%3d/%6d/%6d不归一，依次复制textBase+1/+2/+3，格式后重取当前window/data、含NUL消费。名字/颜色/显隐/技能两次查询/状态摘要/属性条/选中图号及关联函数均不动，不增加清理/保护或缓存，不据本步验收完整窗口15/交互/父路由/画面或运行。

原715仅统计，本步只读117格式/参数/消费邻窗及2个索引算址间隙指令，共119，余596及已过核心不重验；三真实原点与三现callee、两已证格式字节、S16索引/S32数值及来源/消费顺序、整份源仅三标识符的静态核对。48C1CF一普通caller、data0只登记；未编译或执行。 语义记录912→913；已接492→495/已确认1023，余528，其中494个IDA直接引用+1个既有原字节补证；原1022直接引用/补证字段、124格式常量、其他911语义记录、pending18/分类4及其余顶层保持。仅追加局部合同及唯一进度，不刷新总SPEC/索引，不据本步推算整体完成率。


## 宠物仓库外部行4AB710三处真实格式（2026-10-03）

局部 SPEC：仅三现callee接4AB7FA/4AB90D/4ABA73，S32(index,state)/consumed返回和全部正文保持。512B buffer不预清；拒绝提示零附加参数写+128后44B050(...,4,3)，原kind1/source<5先快照重量与槽数，固定六槽/slot<itemSlots才取used，重量与有效装备同时存在才拒绝。转移命令两个S32仍petIndex+100及命中后重取S16符号扩展target，写起点→+256/cap255转义→userState/value14→Y→X→socket→512620，满槽/focus不符消费及无额外音效保持。kind3/source<100先固定window、读取既有World等级S32，再取元素13 data/text直接写%10d；不改变CC4218。拖拽/used快照/hover/绘制/坐标/回调与音频暂停保持，不改关联函数或补保护/缓存/清理，不据此验收完整窗口41/父路由/协议/画面或运行。

原290仅统计，只读111格式/参数/消费邻窗及19个转义后坐标/发送/相邻分支指令，共130，余160/已过正文与核心不重验。两新增格式587900/586C0C原字节、586BBC既有证据、零附加参数与S32命令/等级、来源/同源buffer偏移/消费顺序及整份源仅三标识符限定静态核对；571160一data/普通caller0只登记，未构建或执行。 语义记录913→914，其他912旧记录/pending18/分类4及其余顶层保持；已接495→498/已确认1023，余525，其中497个IDA直接引用+1个既有原字节补证；原1022直接引用/补证字段保持，格式常量124→126。只追加局部合同及唯一进度，不刷新总SPEC/索引，不推算整体完成率。


## DP结果499790两处真实格式（2026-10-03）

局部 SPEC：仅两现callee接4997C7/4997ED，S32(value)/返回1及全部正文保持。输入恰-1时先取tick、固定window、发布共享U32 CB0F78=tick+5000，借World FF0D04 S32变化值直接格式到该window元素2 text；再重新取当前window，借FF0D08 S32总量直接格式到元素3 text。保留DP三个空格、%+9d与Total%9d的符号/宽度；不复制共享数值或新增计时owner。随后仍重取tick，仅共享到期tick严格小于当前tick才关闭，原回调/绘制/音频暂停边界不动，不改关联函数、不增清理/保护或一次性标志，不据本步验收完整窗口31/父路由/画面或运行。

原44仅统计，本步只读34格式/参数/消费邻窗，余10及已过主更新/计时/核心不重验。两原真实点/两现callee、58722C/587220两原字节、S32来源与两次window取样/元素2→3直接消费顺序及整份源仅两标识符限定静态核对；54E7B0一data/普通caller0只登记，未构建或执行。 语义记录914→915，其他913旧记录/pending18/分类4及其余顶层保持；已接498→500/已确认1023，余523，其中499个IDA直接引用+1个既有原字节补证；原1022直接引用/补证字段保持，格式常量126→128。只追加局部合同及唯一进度，不刷新总SPEC/索引，不推算整体完成率。


## 状态栏4723D0三处真实格式（2026-10-03）

局部 SPEC：仅三现callee接47246D/472488/4727E8，S32(index,state)/consumed及全部分支返回与正文保持。共用未清256B buffer；index13 hover按原D66E24门槛、candidate<=4宠物/其余玩家两独立原call，借自定义名或基础名T*后读宠物lvFlag6C S32，玩家读D2F788 S32，格式后共用原D68AE8含NUL复制，再按原重新读门槛决定消费/开窗。index11玩家伤势为0且target!=-1才取宠物，原伤势S32四档25/50/75和两名称来源共4727E8一点；先复制对齐4B主体→重新读玩家伤势→复制余字节，别改成一次复制或提前快照。原伤势/加点图号、邮件指针/发布顺序、窗口关闭/创建、关联函数均保持，不加保护/预清/缓存，不据本步验收完整状态栏/邮件/开窗/父路由/画面或运行。

原450仅统计，只读115格式/参数/消费邻窗及39个四档起始/共享复制新指令，共154，余296/已过正文及核心不重验。六格式沿window_1既有逐字节原证，不重读，新增登记到53B304格式池；三真实点/三现callee、四伤势格式共一call、T*/S32来源和复制中重读顺序、整份源仅三标识符限定静态核对。4723CF仅前方NOP自然流type21，普通caller/data均0；55F0A0元素11/12/13绑定沿既有窗口1合同，不据静态call xref缺失推无真实调用，未重验完整绑定或执行。 语义记录915→916，其他914旧记录/pending18/分类4及其余顶层保持；已接500→503/已确认1023，余520，其中502个IDA直接引用+1个既有原字节补证；原1022直接引用/补证字段保持，格式常量128→134。只追加局部合同及唯一进度，不刷新总SPEC/索引，不推算整体完成率。


## 邮件详情4A49B0六原点三现表达式（2026-10-03）

局部 SPEC：本步三现格式callee接六原53B304点：三类别时间共一表达式，公会与普通/迷你两姓名原点共一表达式，页码一原点；4A4B2C跳4A4F22证明普通/迷你姓名共享call，不按类别或现表达式数错计。2048B未清同源buffer，消息未读+0先清→借text02 T*格式→既有斜线查找/元素16，公会无槽空文本路径保持；kind与正文每行独立重采样、十行消费者不合并。姓名原owner T*、公会先前成员查询结果及普通/迷你尾重读联系人、格式后重取元素15保持；姓名复制后重读页号+1 S32和固定S32 10→%2d/%2d→重新取元素14/含NUL长度U32复制并返回；类别外W32(category-2)不改。仅换三标识符，不增预清/保护/缓存，不改关联方或既有字符查找，不据此验收完整邮件/父路由/布局/画面或运行。

两原格式5535F8/58645C沿已登记字节，不重读或新增；既有父的三search点、入向、来源/角色/旧边界与旧验证文字保持，仅追加六format点及本步说明，禁止重复新增父。本步原466统计/196格式-参数-消费邻窗（含与既有查找邻窗相交的原上下文，不累加成新全覆盖），另270本步不读；六原真实点/三现callee分组、两既有格式字节、T*/S32及来源/消费/返回和整份源三标识符限定静态核对；原4A4650一call入向及三search点记录保持，未重验其核心/完整绑定、未构建执行。语义记录916保持，不新增重复父；仅53B304和既有4A49B0两记录变更，其他914记录/pending18/分类4及其余顶层保持。已接503→509/已确认1023，余514，其中508个IDA直接引用+1个既有原字节补证；原1022直接引用/补证字段及134格式常量保持。不刷新总SPEC/索引，不推算整体完成率。


## 宠物仓库自有行4AAEC0两处真实格式（2026-10-03）

局部 SPEC：拖放4AB092和双击4AB1F9各自零附加参数写同一未清128B后44B050；两点独立计数，587900沿已登记字节不重读。自有行按槽数扫描后读重量，重量或装备触发拒绝；事件前后拖动源两次重取/累计及整个正文保持。

原379统计/只读68格式、拒绝门槛及消息消费邻窗，余311和已过核心不重验；两原点/两现callee、既有587900原字节和constexpr、零附加参数/128B同源及消费顺序、整份源仅两标识符限定静态核对。571160一data/普通caller0只登记，未构建执行。 语义916→917，其他915旧记录/pending18/分类4及其余顶层保持；已接509→511/已确认1023，余512，其中510个IDA引用+1个既有补证；原1022直接引用/补证字段及134格式常量保持。完整窗口/父路由/协议/画面/运行不据本步验收；只追加合同及唯一进度，不刷新总SPEC/索引。


## 宠物技能刷新491AA0两处真实格式（2026-10-03）

局部 SPEC：491FBE技能fp S32→%3d→D66C34[row]11B槽，名称复制后重取编号；492070选择S32+1/固定5→%d/%d→元素12，格式后重取text。256B未清、原物理十项/used门槛、未用费用不清、编辑/比较与尾部非拥有元素T*返回正文保持。

原460统计/只读78格式、来源与费用/页码消费邻窗，余382及已过正文/核心不重验；两原点/两现callee、553FCC/586D48既有原字节、S16编号/S32费用页码/对应11B槽及元素12、返回身份和整份源仅两标识符限定静态核对。48DBB0@48DCA3一普通caller/data0只登记，未构建或执行。 语义917→918，其他916旧记录/pending18/分类4及其余顶层保持；已接511→513/已确认1023，余510，其中512个IDA引用+1既有补证；原1022直接引用/补证字段及134格式常量保持。两格式沿既有登记不重读；完整窗口18/编辑/父路由/运行不据本步验收，只追加合同和唯一进度，不刷新总SPEC/索引。


## 设置音量文本494180两处真实格式（2026-10-03）

局部 SPEC：4942C1借音乐S32→D3041C，494307借音效S32→D2EA64，各用原%d；两既有12B文本存储保持。每次格式后477460(...,3)测宽，再独立重读CA02DC并算术右移居中，依次写元素13.x/16.x；不提前采样或改关联正文。仅显示接线，音频后端继续暂缓，不转记排除/音频验收。

原125统计/只读46格式、来源及测宽-偏移-布局消费邻窗，余79/已过正文与核心不重验；两原点/两现callee、5531A4已登记原字节、原dword/source S32与两既有12B存储、两次消费顺序和整份源仅两标识符限定静态核对。普通caller/data均0仅记录静态事实，不推无真实绑定；未构建或执行。 语义918→919，其他917旧记录/pending18/分类4及其余顶层保持；已接513→515/已确认1023，余508，其中514个IDA引用+1既有补证；原1022直接引用/补证字段及134格式常量保持。5531A4不重读；完整窗口24/布局/父路由/画面/运行不据本步验收，只追加合同及唯一进度，不刷新总SPEC/索引。


## 装备页489760两变体共一真实格式（2026-10-03）

局部 SPEC：489D89跳489DA3、name/memo两T*与[kok]/[han]共489DA8一点，只换两现callee。保留清hover前固定物品借用与hitIndex、memo测长和Word flags100/200优先级，第二分支空memo不加检查，256B未清→D68AE8含NUL复制及完整正文/音频暂停保持。

原584统计/只读47格式、memo来源/Word门槛和共享消费邻窗，余537/已过正文与核心不重验；一原点/两现表达式/489D89共用跳转、两既有原格式字节、借用T*及源指针/消费次序、整份源仅两标识符限定静态核对。七data/普通caller0只登记，不推完整绑定，未构建执行。 语义919→920，其他918旧记录/pending18/分类4及其余顶层保持；已接515→516/已确认1023，余507，其中515个IDA引用+1既有补证；原1022直接引用/补证字段及134格式常量保持。586BCC/586BC4沿已登记原证不重读；完整库存/装备/窗口/父路由/运行不据本步验收，只追加合同及唯一进度，不刷新总SPEC/索引。


## 库存选物489F00与说明4C75A0五处真实格式（2026-10-03）

局部 SPEC：489F00两现表达式接槽指令1点与两mode数量2点；4C75A0两表达式接四属性变体共享1点与尾文本1点，最后普通提示无格式call。保留原S32/T*、256B+独立16B/cap15发包次序、512B属性/尾同源、Word/W8门槛/属性顺序与低S16绘制、返回/退出/回调/音频暂停正文。

原598统计/只读129格式/转义-发送/两count消费邻窗及47个S8模式分流指令，共176，余422/已过正文与核心不重验；48A3EB跳48A543为mode1后段、mode2先下行；三原点/两现表达式、两既有原字节/S32、未清256B与独立16B/消费次序和整份源两标识符限定静态核对。四data/普通caller0只登记，未构建执行。 原239统计/只读92格式/参数/消费邻窗及21尾两次绘制指令，共113，余126/已过正文与核心不重验；两原点/两现callee、四属性变体共call/五新增格式原字节、T*/S32与原512B/消费者和整份源两标识符限定静态核对。489EE0@489EEA/49B610@49B61A两普通caller/data0只登记，未构建执行。 语义920→922，其他919旧记录/pending18/分类4及其余顶层保持；已接516→521/已确认1023，余502，其中520个IDA引用+1既有补证；原1022直接引用/补证字段保持，格式常量134→139。5531A4/553FCC沿既有证据不重读；只追加局部合同和唯一进度，不刷新总SPEC/索引，不据此验收完整库存/说明/协议/父路由/运行。


## 倒计时4982C0两处真实格式（2026-10-03）

局部 SPEC：498488的S32秒数→%2d→16B未清，只两字节按S8/空格原门槛绘图；4984E7原回合S32位型+1、原模式门槛→5871E4原字节→同buffer/DrawText3。超时后继续本帧、图号基数/提醒/发送/返回及音频暂停正文保持。

原171统计/只读61格式、参数与数字/回合绘制消费邻窗，余110及已过正文/核心不重验；两原点/两现callee、553FB4/5871E4沿既有共享战斗合同原串字节（不重读）、S32与16B同源/前两S8字符及回合消费次序、整份源仅两标识符限定静态核对。498520@498B6E一普通caller/data0只登记，未构建或执行。 语义922→923，其他921旧记录/pending18/分类4及其余顶层保持；已接521→523/已确认1023，余500，其中522个IDA引用+1既有补证；原1022直接引用/补证字段保持，格式常量139→140。仅显示格式接线，不据此验收完整计时/战斗/父路由/运行；只追加局部合同和唯一进度，不刷新总SPEC/索引。


## 战斗结果4991C0两处真实经验格式（2026-10-03）

局部 SPEC：4993E9借结果S32 value04按%+7d直接写row+13，499513按%+5d写row+45；各次先固定window再读取结果，保留固定结果T*及满经验/满等级的原占位路径。原四结果/技能排序配对门槛/最多10行与尾清14槽、共享到期tick/独立采样/返回1和音频暂停正文不动。

原345统计/只读74格式、经验来源与实例文本消费邻窗，余271及已过正文/核心不重验；两原点/两现callee、587210/587200两新增原字节、S32经验与先固定window/再取结果/直写13和45次序、整份源仅两标识符限定静态核对。54E780一data/普通caller0只登记，不推完整绑定；未构建执行。 语义923→924，其他922旧记录/pending18/分类4及其余顶层保持；已接523→525/已确认1023，余498，其中524个IDA引用+1既有补证；原1022直接引用/补证字段保持，格式常量140→142。只显示依赖接线，不据此验收完整战斗结果/数据生产/父路由/运行；只追加局部合同及唯一进度，不刷新总SPEC/索引。


## 战斗装备49AC40两处真实Q格式（2026-10-03）

局部 SPEC：49AFA4来源槽U32、49B217原FF/U32各用Q|%X|写同一未清256B；窗口关闭/选择槽5清0→格式→发送→动作状态原次序与所有失败/提示/消费/绘制/返回及音频暂停正文保持。固定FF沿原push，不作新补值，不加Flush。

原755统计/只读72格式、参数和关闭-选择-发送-动作消费邻窗，余683及已过正文/核心不重验；两原点/两现callee、554048已登记字节、U32来源槽与原FF/未清256B及消费次序、整份源仅两标识符限定静态核对。4C6EC0@4C7027与56C430两data/普通caller0只登记，不推完整绑定，未构建执行。 语义924→925，其他923旧记录/pending18/分类4及其余顶层保持；已接525→527/已确认1023，余496，其中526个IDA引用+1既有补证；原1022直接引用/补证字段及142格式常量保持。格式沿已登记原证不重读；只追加合同及唯一进度，不刷新总SPEC/索引，不据此验收完整装备/战斗/协议/父路由/运行。


## 交易窗口4B0290两姓名真实格式（2026-10-03）

局部 SPEC：4B045F借D2D654 T*按%16s写元素15，4B04B5借原玩家名T*按%-16s写17；交错初始化里的4B0424窗口快照、分隔符前借玩家名/复制16后重取17及Y→X保存时序保持，最低宽度沿原字节，不截断或转码，不改更新正文/音频暂停。

原425统计/只读63格式、借用来源与交错写-分隔符-载荷/坐标消费邻窗，余362及已过正文/核心不重验；两原点/两现callee、587B20新增原字节/553FE0既有字节、T*/窗口取样-来源-直写次序和整份源仅两标识符限定静态核对。54EBD0一data/普通caller0只登记，不推完整绑定，未构建执行。 语义925→926，其他924旧记录/pending18/分类4及其余顶层保持；已接527→529/已确认1023，余494，其中528个IDA引用+1既有补证；原1022直接引用/补证字段保持，格式常量142→143。553FE0不重读；只追加合同和唯一进度，不刷新总SPEC/索引，不据此验收完整交易/数据生产/布局/父路由/运行。


## 物品作业回复4F1800五原点两现表达式（2026-10-03）

局部 SPEC：经验/耐力/灵巧/智力四S32原点共数值appendValue，升级独立零附加参数原点；原%+d/28B补LF门槛/8段上限及升级自带LF/追加-计数-SE64、未清256B同源、原解析/前后缀/来源/伤势普通文本/尾47FAC0及音频暂停正文保持。

原395统计/只读两段96+64共160格式、S32/零附加参数/门槛-换行-追加消费邻窗，余235及已过正文/核心不重验；五原点/两现表达式与数值共享分组、五已登记原字节（不重读）、源槽/256B同源/原LF及追加次序和整份源仅两标识符限定静态核对。50C7F0@50C832一普通caller/data0只登记，未构建执行。 语义926→927，其他925旧记录/pending18/分类4及其余顶层保持；已接529→534/已确认1023，余489，其中533个IDA引用+1既有补证；原1022直接引用/补证字段及143格式常量保持。不重读五格式、不扩展作业行为；只追加局部合同和唯一进度，不刷新总SPEC/索引，不据此验收完整结果/协议/父路由/运行。


## 技能槽4740F0真实格式一点（2026-10-03）

局部 SPEC：S16槽数沿既有十配对累计S32 count，在未清256B按%2d/10格式后含NUL复制借用destination T*；保留完整S16/最低宽度/空格及分隔符、void返回和调用方门槛，不加钳制、保护或清理。

原38统计/只读25格式、S32参数与目标含NUL复制邻窗，余13/已过累计与核心不重验；一原点/一现callee、553FC4新增原字节、S32及未清256B/借用目标T*消费、整份源仅一标识符限定静态核对。471440@4716AE一普通caller/data0只登记，不推完整构造绑定，未构建执行。 语义927→928，其他926旧记录/pending18/分类4及其余顶层保持；已接534→535/已确认1023，余488，其中534个IDA引用+1既有补证；原1022直接引用/补证字段保持，格式常量143→144。只追加合同及唯一进度，不刷新总SPEC/索引，不据此验收完整技能生产/构造/父路由/运行。


## 玩家属性4767E0二十八原点一现表达式（2026-10-03）

局部 SPEC：28个原点共writeNumber一现callee，三个原格式按元素1/2..15/16..28分组；原普通S32与values78/B0/BC/D4逐项索引顺序、未清256B、格式后data→长度→text/含NUL消费不变，每项结束才取下一值。沿窗口8已有逐项原证，名称和最后加点显隐/独立重读/生产正文保持，原24h/1Ch只作证据，不在新端按偏移强转。

原776仅统计，本步枚举28个53B304真实目标call，并只读元素1/2/16三代表格式/参数/消费者邻窗共45指令，其他731本步不读。28项S32/原读取地址与现字段/元素/复制顺序沿window_8既有逐项原证，不重核原刷新核心；28新接线对照既有表及现静态展开顺序、三个已登记原字节、整份源仅一callee的限定静态核对。475DF0@475DF0一普通caller/data0只登记，未构建或执行。 语义928→929，其他927旧记录/pending18/分类4及其余顶层保持；已接535→563/已确认1023，余460，其中562个IDA引用+1既有补证；原1022直接引用/补证字段及144格式常量保持。三个原格式不重读；只追加合同及唯一进度，不刷新总SPEC/索引，不据此验收完整属性/生产/窗口/父路由/运行。


## 称号478010与简介478A10两等级真实格式（2026-10-04）

局部 SPEC：两个独立FDA704 S32等级原点，各用未清256B buffer/number按%3d格式，格式后先固定当前window/data再测长/读text含NUL复制，分别元素7/1。称号页编辑身份/原型context/草稿/比较/列表与简介owner/公开标志/输入/正文/颜色/变更检测正文均保持，既有资料数据缺口不补。

原208统计/只读25等级格式-参数-data先取/含NUL消费邻窗，余183及已过正文/核心不重验；一原点/一现callee、553FCC已登记原字节/S32与未清256B/元素7和来源/消费次序、整份源仅一标识符限定静态核对。47752C@47752C一普通caller/data0只登记，未构建执行。 原678统计/只读25等级格式-参数-data先取/含NUL消费邻窗，余653及已过正文/核心不重验；一原点/一现callee、553FCC已登记原字节/S32与未清256B/元素1和来源/消费次序、整份源仅一标识符限定静态核对。478320@478320一普通caller/data0只登记，未构建执行。 语义929→931，其他928旧记录/pending18/分类4及其余顶层保持；已接563→565/已确认1023，余458，其中564个IDA引用+1既有补证；原1022直接引用/补证字段及144格式常量保持。553FCC不重读；只追加合同及唯一进度，不刷新总SPEC/索引，不据此验收完整称号/资料/编辑/父路由/运行。


## 制作47DF50及装饰47FAE0/47FD00三处真实格式（2026-10-04）

局部 SPEC：材料name T*/count S32按%-24s%3d直写固定window元素9+row，保留4→0/非负itemId门槛/count非正重置row/格式后重取recipe；两个装饰构造各原push100 S32，21空格+原魔力%3d直写固定window元素10。原释放清表/首NUL/回调/动作返回和模式0/6/15最后发布正文不动，不扩大固定100授权。

原165统计/只读28格式-来源/固定入口window消费者邻窗，余137与已过正文/核心不重验；一原点/一现callee、5865B8原字节/T*和S32及recipe后重取、整份源仅一标识符限定静态核对。各三普通caller/data0只登记，不推完整路由，未构建执行。 原149统计/只读39格式-来源/固定入口window消费者及早推format/原100邻窗，余110与已过正文/核心不重验；一原点/一现callee、58678C原字节/S32原100及21空格、整份源仅一标识符限定静态核对。各三普通caller/data0只登记，不推完整路由，未构建执行。 原149统计/只读39格式-来源/固定入口window消费者及早推format/原100邻窗，余110与已过正文/核心不重验；一原点/一现callee、58678C原字节/S32原100及21空格、整份源仅一标识符限定静态核对。各三普通caller/data0只登记，不推完整路由，未构建执行。 语义931→934，其他930旧记录/pending18/分类4及其余顶层保持；已接565→568/已确认1023，余455，其中567个IDA引用+1既有补证；原1022直接引用/补证字段保持，格式常量144→146。只追加合同及唯一进度，不刷新总SPEC/索引，不据此验收完整制作/装饰/动作/父路由/运行。


## 物品移动及两库存数值回调四个原格式点（2026-10-04）

局部 SPEC：4885A0两窗口分支共一现表达式，S32×3写未清384B，+128/cap255转义，状态后采样→Y→X→最终socket；两个数值回调各一S32/%10d写各自12B显示，返回0。仅三callee标识符调整，所有签名/真实返回/短路/owner/关联正文保持。

物品移动88原指令统计/62新邻窗仅读，其余26与已过核心不重验；两数值父各8指令全部仅读。四原点/三现表达式，两格式原字节（仅586B08新登记）限定静态核。语义934→937；已接568→572/已确认1023（571个IDA直接引用+1既有原字节补证），余451；格式常量146→147。原池1022直接引用及补证明细、其他933旧语义记录、pending18/分类4与其余顶层保持。 完整库存/协议父/CRT/运行未闭合。


## 数量48A950与技能拖动4920F0两个真实格式点（2026-10-04）

局部 SPEC：48AA45在回调/既有SE后重取window，S32/%9d写CC4D1C既有12B并返result；4921F4在画名称后重取两索引读fp，S32/%3d写未清12B再绘费用，最后返回背景绘制值。仅两个callee标识符调整，保持完整正文和各自owner/坐标/返回，音频暂停范围保持。

83/104原指令统计、26/32新邻窗仅读，余57/72和已过核心不重验；两原点/两现表达式/两既有原格式限定静态核。语义937→939，其他936旧记录/pending18/分类4及其余顶层保持；格式调用572→574/已确认1023（573直接引用+1既有补证），余449；格式常量147保持，原1022直接引用及补证明细不动。


## 制作482270与背包483150两个真实格式（2026-10-04）

局部 SPEC：482420在模式3/非220资格比较后重取单选等级，S32 level*10按原21空格魔力%3d写元素11，FP不足仍写；483DE3在图形后从同一item取count，正值S32/%3d写未清12B并绘数量。仅两callee标识符替换，完整签名/采样/owner/门槛/关联调用/返回正文保持，不作溢出模拟或新增保护。

原936/908指令统计、52/34新邻窗仅读，余884/874与已过核心不重验，两原点/两现表达式/两既有格式限定静态核。语义939→941，其他938旧记录/pending18/分类4和其余顶层保持；格式调用574→576/已确认1023（575直接引用+1既有补证），余447；格式常量147保持，原1022直接引用/补证明细保持。


## 设置四音量按钮真实格式（2026-10-04）

局部 SPEC：四按钮各一原点，以既有52C3D0之后重新读取的S32音乐/音效值按%d写D3041C/D2EA64各12B，再测宽、读取偏移并重取window定位13.x/16.x。仅四callee标识符替换，原变化±1/边界失败不钳回/消费返回/入口图形身份/音频调用和暂停范围保持。

各87原指令统计/42新邻窗仅读，其余45与已过核心不重验；四原点/四现表达式、既有格式与应用后重读-文字-布局消费限定静态核。语义941→945，其他940旧记录/pending18/分类4和其余顶层保持；格式调用576→580/已确认1023（579直接引用+1既有补证），余443；格式147及原1022直接引用/补证明细保持。 音频本体/CRT/窗口父/运行未闭合。


## 三宠物详情三十个真实格式点（2026-10-04）

局部 SPEC：4AAB70/4AB400/4B0F20各drawNumber一现表达式承接十原点；十S32按原%3d/%4d/%2d写各父同一未清256B，当前项绘完才读下一项。保留自有pet两独立CryptInt最大值与+98字段、两种普通记录字段偏移差异、字形13/13/8、颜色低U8=138及坐标低S16。前两父used零直接返回，交易父used零仍画背景并返回绘图结果；所有签名/原名/种族/owner和完整正文保持，仅三callee标识符调整。

原276/253/268统计，仅173/172/172新格式/来源和首项消费者邻窗仅读，余103/81/96与已过核心不重验。三十原点/三现表达式、逐项字段/格式/消费者及整份限定差异静态核。语义945→948，其他944旧记录/pending18/分类4与其余顶层保持；格式调用580→610/已确认1023（609直接引用+1既有补证），余413；格式常量147和原1022直接引用/補证明细保持。 完整数据生产/父路由/CRT/画面/运行未闭合。


## 宠物卡与交易统计七个真实格式（2026-10-04）

局部 SPEC：48D540等级S32/%3d、HP和FP各两S32/%6d/%6d，共用未清256B，最大值→当前值原读序、当前值在前的格式参数及文字消费保持。4B1260在真实callback之后以tab2显示金额%9d，再依物品数%3d/宠物数%3d/金额%10d，共用未清52B逐项4771B0消费，displayAmount不补零。仅七callee标识符调整，完整多参签名、非拥有指针、原图形/页签/窗口/返回正文保持。

原359/524指令统计，仅98/90新邻窗仅读，余261/434与已过核心不重验。七原点/七现callee、S32参数及四既有格式/容量和消费者限定静态核。语义948→950，其他947旧记录/pending18/分类4和其余顶层保持；格式调用610→617/已确认1023（616直接引用+1既有补证），余406；格式147及原1022直接引用/补证明细保持。 完整数据生产/父路由/CRT/画面/运行未闭合。


## 宠物移动及战斗库存数值三个真实格式（2026-10-04）

局部 SPEC：48BBF0两S32/P|%d|%d写未清384B，+128/cap255转义后取窗口41状态、Y/X/最终socket，网络返回沿原void弃用；499B10/499B30各普通S32/%10d写独立12B文本并返回0。仅三callee替换，所有签名/原拒绝门槛/owner/正文保持。

56原指令仅37新邻窗仅读、余19与核心不重验；两数值父各8全部仅读。三原点/三现callee、两既有原格式/容量及取值-消费-返回限定静态核。语义950→953，其他949旧记录/pending18/分类4和其余顶层保持；格式调用617→620/已确认1023（619直接引用+1既有补证），余403；格式147及原1022直接引用/补证明细保持。


## 查看与接收资料value108两个真实格式（2026-10-04）

局部 SPEC：4CB040读105EDE8 S32/%3d→未清256B→元素2含NUL消费，4CBA9D读1074218 S32/%d→未清256B→元素1含NUL消费。仅两callee替换，两个独立资料const引用owner、完整图号/文本/当前窗口及载荷重取、1024B/256B四次分行与末行含NUL U32返回保持，不合并owner或新增清零/保护。

原267/270指令统计，各35新格式-来源-消费者邻窗仅读，余232/235和已过核心不重验；两原点/两callee、S32与两既有原字节/256B/消费者限定静态核。语义953→955，其他952旧记录/pending18/分类4和其余顶层保持；格式调用620→622/已确认1023（621直接引用+1既有补证），余401；格式147及原1022直接引用/补证明细保持。 完整资料生产/父路由/CRT/画面/运行未闭合。


## 三交易物品页名称变体与数量真实格式（2026-10-04）

局部 SPEC：4B1E7F/4B2C63/4B3832每个共享name[kok]/memo[han]两T*格式变体，保留memo长度短路、玩家W16/交易W32 flags两次原读取；4B2017/4B2DC0/4B3924各图形后读S32 count，正值按%3d绘制。九现callee对应六原点，各父借同一未清256B，提示含NUL复制与低S16数量绘制消费保持。对方ESI=record+2与[ESI+6]实证对应+08，未据操作数误读字段；所有多参void签名与正文不动。

原566/549/389只读112/112/118新邻窗，余454/437/271和已过核心不重验；六原点/九表达式/三既有格式、T*-S32-标志宽度及消费者限定静态核。语义955→958，其他954旧记录/pending18/分类4和其余顶层保持；格式调用622→628/已确认1023（627直接引用+1既有补证），余395；格式147及原1022直接引用/补证明细保持。 完整交易/数据生产/父路由/CRT/画面/运行未闭合。


## 对话49DA30三个真实格式点（2026-10-04）

局部 SPEC：连续未清1280B五个256B子区保留；type2负firstLine/%d、type35两原位转义文本/%s|%s、独立row>=0的row+1/%d，各按原子区写后转义cap255。校验短路/按钮回复后仍可再发行回复/每次目标请求及Y-X-socket重取/清状态-音效-返回全部正文不动，仅三callee标识符替换，不加限制或保护、不改别名。

原259只读90新格式和邻近转义-采样起点，余169与已过核心不重验；新5872C0字节25737c257300与既有5531A4/S32及T*×2/五区容量-消费者-返回限定静态核。语义958→959，其他957旧记录/pending18/分类4和其余顶层保持；格式调用628→631/已确认1023（630直接引用+1既有补证），余392；格式147→148，原1022直接引用/补证明细保持。 完整对话/协议父/CRT/画面/运行未闭合。


## 治疗49E1D0与49E400七个真实格式（2026-10-04）

局部 SPEC：更新两S32费用/玩家金额分别写64B后绘制；创建固定标题/英文表头/完整治疗、S32 level与T* name列表、原空String1各真实格式写512B后含NUL复制共享行。六已有stdio callee替换；既有空格式首NUL折叠按本板块原调用接线要求恢复显式入口，字节结果/忽略返回/原子区和第9行消费保持。空格式来源用原已确认PE虚拟零合同并明确未加载，不借用可写593850；其他等价裁决保持。

197/276原指令仅56/117新邻窗，余141/159与核心不重验；七原点/七callee、六加载原字节/一虚拟零来源、S32-T*及无变参/64B-512B-记录门槛-逐项消费-返回限定静态核。语义959→961，其他958旧记录/pending18/分类4和其余顶层保持；格式调用631→638/已确认1023（637直接引用+1既有补证），余385；格式148→155（六加载原格式+一已确认虚拟零空格式），原1022直接引用/补证明细保持。 完整治疗/父路由/CRT/画面/运行未闭合。


## 选择命令四个真实格式点（2026-10-04）

局部 SPEC：三个原movsx S8命令→S32和一个index+1 S32按%d写原32B，仅四callee标识符替换；合法索引0..1、其他返回原index、同区+16/cap15转义、状态-Y-X-socket后采样和真实发送返回保持。现共享SendChoiceDialogReply借同一buffer，不更换owner/改签名/新增清零或保护。

四父各38仅30新邻窗，余8与已过核心不重验；四原点/callee/S8-S32与既有原格式/容量-owner-消费-返回限定静态核。语义961→965，其他960旧记录/pending18/分类4和其余顶层保持；格式调用638→642/已确认1023（641直接引用+1既有补证），余381；格式155及原1022直接引用/补证明细保持。 完整命令生产/选择父路由/协议/CRT/运行未闭合。


## 外部4B4140与交易4B4680宠物详情十一原格式（2026-10-04）

局部 SPEC：外部五S32/+30..40和交易五S32/+38..48，各%10d写原未清100B，当前项层5/字体12/颜色0及低S16坐标消费后才取下一项，各五原点共一循环现表达式。外部另先+5FC容量>0才读+5F8重量，两S32/%3d / %3d同区写，绝对Y31保持；交易不新增重量。仅三callee标识符替换，两种低S8索引/记录步长及used零的0/slot返回、所有预览/技能/背景和真实返回保持。

原378/380只读131/109新邻窗，余247/271与已过核心不重验；十一原点/三现callee、两既有原字节及普通S32/100B/消费者与返回限定静态核。语义965→967，其他964旧记录/pending18/分类4及其余顶层保持；格式调用642→653/已确认1023（652直接引用+1既有补证），余370；格式155及原1022直接引用/补证明细保持。 完整数据生产/父路由/CRT/画面/运行未闭合。


## 窗口51标题N与物品I两个真实格式（2026-10-04）

局部 SPEC：标题沿未清512B起点复制→52D2D0到+256→T*/N|%s写起点→52D1B0回+256，cap255；选择沿未清52B、S32/I|%d直接发，不新转义/拒绝负值。仅两callee替换，编辑/点击原门槛、目标请求-Y-X-socket采样、SE/consumed与入口graphic/末负选择图号正文保持，不改关联方。

160/77原指令仅39/39新邻窗，余121/38和核心不重验；两新原格式字节/T*-S32、容量和原位分区/消费者-返回限定静态核。语义967→969，其他966旧记录/pending18/分类4和其余顶层保持；格式调用653→655/已确认1023（654直接引用+1既有补证），余368；格式155→157，原1022直接引用/补证明细保持。 完整窗口51/协议父/CRT/画面/运行未闭合。


## 存放4A9160金额刷新一原格式（2026-10-04）

局部 SPEC：仅替换4A9175格式callee。保留未用S32形参/真实返回1；入口普通dword CC4218 S32金额按既有586BBC %10d写原未清16B，格式后才重取当前窗口元素13载荷并复制含NUL。两次滚动载荷重取、41状态字段/预览分派及修改、原鼠标CryptInt取值、各实际预览调用、DrawCurrentWindow与全部后续正文保持；不与World金额合并，不预清、缓存载荷、钳制或改变相关方。

原114指令统计，仅32条金额来源/格式/后取元素13/含NUL复制及下一载荷邻窗仅读，余82与已过核心不重验；一原点/callee、普通S32/既有原字节/未清16B/元素13消费/返回限定静态核。call0/data1只登记，不推完整窗口路由，未构建执行。 语义969→970，其他968旧记录/pending18/分类4及其余顶层保持；格式调用655→656/已确认1023（655直接引用+1既有补证），余367；格式157及原1022直接引用/唯一补证明细保持。 完整父路由/数据生产/CRT/画面/运行未闭合。


## 窗口41/38四个物品数量原格式（2026-10-04）

局部 SPEC：四现callee各接一原点。自有玩家position+8/C5CH记录+C44、外部CC4D58/C44H记录+8，图形后重取普通S32 count，正值才%3d写同一未清12B并按层3/字体1/颜色1及中心-3/+7低S16消费。41中心37/51与38中心36/64分别保持，38自有额外普通图不去重，41不添重复图。两S32签名/事件/分页/共享状态/used门槛/绘图与consumed返回完整正文保持，不引入预清/保护/数值快照或改变关联方。

四原536/478/486/426统计，各44格式与31记录基址/入口/used-loop新邻窗仅读，共75，余461/403/411/351及核心不重验；四原点/callee、普通S32/既有%3d与12B合同/图形后读数/真实消费及返回限定静态核。data2/2/1/1、普通caller各0只登记；未构建执行。语义970→974，其他969旧记录/pending18/分类4及其余顶层保持；格式调用656→660/已确认1023（659直接引用+1既有补证），余363；格式157及原1022直接引用/唯一补证明细保持。 完整父路由/数据生产/CRT/画面/运行未闭合。


## 窗口50删除与日期五原格式（2026-10-04）

局部 SPEC：删除四case各push S32 0..3汇入一个%d原点，现(index-5)/7保持；原未清512B/+256格式→起点/cap255转义→目标/请求/Y/X/socket→button64发送弃返回→关闭/返回1。四记录时间>0才localtime借结果，依分钟/小时/日/月/年取五S32，以年+1900/月+1/日/时/分按新ASCII日期格式写同一未清256B，再取日期元素4/11/18/25复制含NUL；非正仅visible0，正值不补visible1/空回退。只两现callee接五原点，全部其他switch/正文/分页/焦点与void裁决保持。

原453指令统计，格式与转义/发送/关闭59邻窗，另四case目标/共享汇入及跳表仅读，合计151条唯一指令，余302与已过核心不重验；一个共用原点/callee、四S32推值/既有%d与512B两区合同/真实消费及返回限定静态核。普通call0/data1只登记，不推完整父路由，未构建执行。 原557指令统计，仅252条四时间门槛/localtime来源/五S32/格式-日期载荷与含NUL消费新邻窗仅读，余305与已过核心不重验；四原点/一现循环callee、新原加载ASCII格式/沿既有256B与void裁决、后取载荷/非正可见路径限定静态核。普通call1/data0只登记，不推完整父路由，未构建执行。 语义974→976，其他973旧记录/pending18/分类4及其余顶层保持；格式调用660→665/已确认1023（664直接引用+1既有补证），余358；格式157→158及原1022直接引用/唯一补证明细保持。 完整父路由/数据生产/CRT/画面/运行未闭合。


## 商店4B8670/4B86C0数量和合计两原格式（2026-10-04）

局部 SPEC：仅一现callee接一原点。五行各先重读gShopScrollCC488C S32，再按D2D988/11B步长仅清该行首字节；借CF4FE0+(scroll+row)*C58H，used W8非零才读+C28普通S32所选数，以既有553FB4 %2d直接写原行。原55B owner/五行连续别名和void返回裁决保持；不替换为+C30，不预清其余字节、添加数量正值门槛、scroll快照或改调用方。

仅一现callee接一原点。原先清D2DAFC、20条used W8非零才读+C28所选数与+C24价格、普通S32乘加发布total后，以既有586BBC %10d写同源CC4B2C48B区；格式后重读D2DAFC与同源World FC金额，再取当前元素7、按S32合计<=金额写颜色U8 0否则6。保留原void裁决、旧48B owner及+44其他既有别名，不预清、扩容、保护、钳制、专项模拟溢出或改变关联方。

原28指令统计，仅26条来源/清首NUL或累加发布/格式/循环或后取金额颜色新邻窗仅读，余2与已过核心不重验；一原点/callee、普通S32/既有格式及55B五行11B或48B合同/同源消费/void裁决限定静态核。普通call9/data0只登记，其中无IDA父的caller保留null，不猜补目录，未构建执行。 原29指令统计，仅29条来源/清首NUL或累加发布/格式/循环或后取金额颜色新邻窗仅读，余0与已过核心不重验；一原点/callee、普通S32/既有格式及55B五行11B或48B合同/同源消费/void裁决限定静态核。普通call3/data0只登记，其中无IDA父的caller保留null，不猜补目录，未构建执行。 语义976→978，其他975旧记录/pending18/分类4及其余顶层保持；格式调用665→667/已确认1023（666直接引用+1既有补证），余356；格式158及原1022直接引用/唯一补证明细保持。 完整商品数据/父路由/CRT/画面/运行未闭合。


## 窗口33玩家X/Y两原格式（2026-10-04）

局部 SPEC：495E60/495EB0各一原点/一现callee，先固定当前WindowTextData*再440B50，非零只清该载荷首NUL/返回0；零才读对应CryptInt坐标S32、之后取同一data->text按既有%3d直写/返回0。沿载荷分配，不新增局部缓冲、容量或提前固定字符地址，所有其他正文保持。

两25指令代码段/RET边界、495E9C/495EEC至53B304直接type17引用仅读；IDA无函数元数据/entry引用按既有窗口33合同登记inventory_id null，不伪目录ID，不改数据库，不新增补证。S32/格式字节/载荷身份与后取text/失败路径/返回及整份源仅两callee限定静态核，不重验核心。语义978→980，其他977旧记录/pending18/分类4及其余顶层保持；格式调用667→669/已确认1023（668直接引用+1既有补证），余354；格式158及原1022直接引用/唯一补证明细保持。 完整父路由/载荷生产/CRT/画面/运行未闭合，未构建执行。


## 技能47B6C0/技术47B9B0四原格式（2026-10-04）

局部 SPEC：技能固定配对S16索引借同一记录，S32等级%2d→数字消费后，才取需求槽数S8/符号扩展S32/%2d→数字消费，两个原点/同一未清20B。技术名称后重取当前技能/需求等级S32 %2d/数字消费，再重取kind，非1/2普通S32 FP*fpReduce/100 %3d同区/数字消费，1/2???分支保持，末图形又取currentKind。四原点/四现callee；所有形参、原层宽度/坐标、颜色/战斗FP前段/图号/void裁决和其他正文不动。

原112指令统计，仅63条格式/普通S32或S8来源/逐项数字消费/相邻kind分支新邻窗仅读，余49与已过核心不重验；两个原点/callee、既有字节/20B及形参层宽度/数值时点和消费者/原void裁决限定静态核。普通call3/data0只登记，不推完整父路由，未构建执行。 原180指令统计，仅78条格式/普通S32或S8来源/逐项数字消费/相邻kind分支新邻窗仅读，余102与已过核心不重验；两个原点/callee、既有字节/20B及形参层宽度/数值时点和消费者/原void裁决限定静态核。普通call2/data0只登记，不推完整父路由，未构建执行。 语义980→982，其他979旧记录/pending18/分类4及其余顶层保持；格式调用669→673/已确认1023（672直接引用+1既有补证），余350；格式158及原1022直接引用/唯一补证明细保持。 完整技能数据/父路由/CRT/画面/运行未闭合。


## 配方47C240/宠物47CB00/骑乘47CE70费用三原格式（2026-10-04）

局部 SPEC：名称绘完后，配方重取当前技能、普通S32配方FP*fpReduce/100；宠物/骑乘均重读同一固定技术T*的普通S32 FP，不复用前段cost。各%3d写未清20B→数字(x+186,y+3)/低U8层消费→原图形与void裁决。配方其后再取kind、宠物两个43C4B0查询和405B40位置、骑乘5ABC38宠物/5ABCA4位置、配对S16技术索引/颜色/位/FP前段及全部形参/层/W32描述符与正文不动。

原110指令统计，仅42条名称后费用/普通S32折算或技术FP/格式/数字消费/相邻图形新邻窗仅读，余68与已过核心不重验；一原点/callee、既有%3d/20B合同/固定借用或重取索引时点、原形参层宽度和void裁决限定静态核。普通call1/data0只登记，不推完整父路由，未构建执行。 原123指令统计，仅42条名称后费用/普通S32折算或技术FP/格式/数字消费/相邻图形新邻窗仅读，余81与已过核心不重验；一原点/callee、既有%3d/20B合同/固定借用或重取索引时点、原形参层宽度和void裁决限定静态核。普通call1/data0只登记，不推完整父路由，未构建执行。 原119指令统计，仅42条名称后费用/普通S32折算或技术FP/格式/数字消费/相邻图形新邻窗仅读，余77与已过核心不重验；一原点/callee、既有%3d/20B合同/固定借用或重取索引时点、原形参层宽度和void裁决限定静态核。普通call1/data0只登记，不推完整父路由，未构建执行。 语义982→985，其他981旧记录/pending18/分类4及其余顶层保持；格式调用673→676/已确认1023（675直接引用+1既有补证），余347；格式158及原1022直接引用/唯一补证明细保持。 完整技能数据/父路由/CRT/画面/运行未闭合。


## 战斗宠物49B670/49BA10/49BB00五原格式（2026-10-04）

局部 SPEC：行显示等级普通S32/%3d、CF4B78快照HP/FP各先最大后当前、按当前/最大两S32/%6d/%6d写同一未清256B，每项字体8/色0/指定层和低S16消费后才取下一项。归还U32 255与召唤U32实际槽各M|%X写未清256B，格式后socket/发送弃返回，再重取窗口关闭/动作1/SE57；门槛/失败SE55或无宠物click消费1、图号/尾绘制/全部原形参与返回正文不动。

原292指令统计，仅103条等级/快照数值来源及消费或命令门槛/格式/发送-关闭-动作-音效/相邻返回新邻窗仅读，余189与已过核心/前段不重验；3原点/callee、S32数值与U32十六进制命令/既有两种格式及新M格式原字节、256B/源与消费者/形参和原返回限定静态核。普通call1/data0只登记，不推完整父路由，未构建执行。 原68指令统计，仅42条等级/快照数值来源及消费或命令门槛/格式/发送-关闭-动作-音效/相邻返回新邻窗仅读，余26与已过核心/前段不重验；1原点/callee、S32数值与U32十六进制命令/既有两种格式及新M格式原字节、256B/源与消费者/形参和原返回限定静态核。普通call0/data1只登记，不推完整父路由，未构建执行。 原167指令统计，仅43条等级/快照数值来源及消费或命令门槛/格式/发送-关闭-动作-音效/相邻返回新邻窗仅读，余124与已过核心/前段不重验；1原点/callee、S32数值与U32十六进制命令/既有两种格式及新M格式原字节、256B/源与消费者/形参和原返回限定静态核。普通call0/data1只登记，不推完整父路由，未构建执行。 语义985→988，其他984旧记录/pending18/分类4及其余顶层保持；格式调用676→681/已确认1023（680直接引用+1既有补证），余342；格式158→159及原1022直接引用/唯一补证明细保持。 完整快照生产/父路由/CRT/画面/运行未闭合。


## 窗口26两模式共用FP与三个命令四原格式（2026-10-04）

局部 SPEC：4966B0 mode3/4各最大后当前FP两S32，模式3 E9跳496A42共享当前读取/496A65格式点，现两callee算一原点；%4d/%4d直接写后取窗口26元素11/text，不合并各次位置或405B40查询。4974C0位置→技术→技能重取、三个U32 S命令；497E30原技术ID>=200600目标分支后再技术，两U32 W命令；497FF0重取骑乘→技术，两U32 W命令。各命令未清256B，后取socket发送弃返回及原刷新/音效/状态计数/末宠物/关闭顺序、门槛与负路W|FF/void全文不动。五现callee接四原点，不新增补证或确认池。

IDA已定义247项统计，仅45条原FP来源/后取载荷/共用格式-定位邻窗；模式3分支66条原字节解码统计，仅新增19条控制/FP邻窗与5项跳表取共用点原证，其余前段及核心不重验。两S32/既有格式/后取借用text与独立位置-最大/当前来源、两现callee/一个原点/原void裁决限定静态核。call3/data0只登记，496A65为既有IDA直接引用，不增补证或确认池，未构建执行。 原52指令统计，仅50条命令来源/格式/发送后刷新-状态-音效-关闭及相邻返回新邻窗仅读，余2与前段/核心不重验；一原点/callee、U32/既有原字节/未清256B/来源和消费者-写序/void裁决限定静态核。call0/data5只登记，不推完整父路由，未构建执行。 原107指令统计，仅51条命令来源/格式/发送后刷新-状态-音效-关闭及相邻返回新邻窗仅读，余56与前段/核心不重验；一原点/callee、U32/既有原字节/未清256B/来源和消费者-写序/void裁决限定静态核。call0/data4只登记，不推完整父路由，未构建执行。 原88指令统计，仅47条命令来源/格式/发送后刷新-状态-音效-关闭及相邻返回新邻窗仅读，余41与前段/核心不重验；一原点/callee、U32/既有原字节/未清256B/来源和消费者-写序/void裁决限定静态核。call0/data4只登记，不推完整父路由，未构建执行。 语义988→992，其他987旧记录/pending18/分类4及其余顶层保持；格式调用681→685/已确认1023（684直接引用+1既有补证），余338；格式159及原1022直接引用/唯一补证明细保持。 完整战斗数据/父路由/CRT/画面/运行未闭合。


## 邮件4A33C0/4A3480/4A3540三删除确认原格式（2026-10-04）

局部 SPEC：三入口沿既有独立普通联系人/公会有序记录/迷你邮件名字T*及各删除回调初始化，按58759C/5875F4/587654原旧字节格式/%s写未清1024B，再复制含NUL到同一CC426C，真实CreateWindowAnimation_475AA0(47,1,0,nullptr) T*返回。仅三callee/三原点，确认/取消载荷/初始化及所有其他正文保持，不合并owner或回调，不翻译、转义、预清、扩容或新增保护。

三原51各40新参数-字段/回调-格式-消费-返回邻窗仅读，余各11与初始化/核心不重验；三加载格式与现constexpr/T*/1024B/同源CC426C/真实建窗T*返回限定静态核，各call1/data0只登记。语义992→995，其他991旧记录/pending18/分类4及其余顶层保持；格式调用685→688/已确认1023（687直接引用+1既有补证），余335；格式159→162及原1022直接引用/唯一补证明细保持。 完整删除父链/数据生产/CRT/画面/运行未闭合，未构建执行。


## 邮件4A09E0与两个私有helper七原格式（2026-10-04）

局部 SPEC：两个状态helper只有4A09E0两视图调用，公会一个%s%s现表达式对应两个原点，联系人%s%s及原逆-%s两个现表达式对应四原点，三行另一个S32/%3d数字原点，共七原点/四现callee/三定义。表前缀及全对象638H后缀原别名、server归一一次、离线copy后状态重读/在线%s%s后重读status10才同prefix覆盖/末U8颜色、初始-1和两视图门槛/载荷取值时点差异保持；数字原未清240B→后取载荷含NUL复制，九行无数字格式。所有其余刷新、非拥有三记录和真实108/3672返回正文不动。

原903指令统计，仅153条七格式来源/表取址-借用text/重读状态覆盖或数字后取载荷-颜色消费新邻窗仅读，余750与已过刷新阶段/权限/表/核心不重验。七个IDA直接原点、四现callee/三定义、两个新加载原格式与既有%3d/constexpr、T*/S32/240B或载荷/两视图消费与108/3672返回限定静态核；普通call1/data0只登记，原helper调用仅该父经当前源码定位，未构建执行。 语义995→996，其他994旧记录/pending18/分类4及其余顶层保持；格式调用688→695/已确认1023（694直接引用+1既有补证），余328；格式162→164及原1022直接引用/唯一补证明细保持。 完整邮件父路由/记录生产/CRT/画面/运行未闭合。


## 邮件时间helper七原点与迷你组合一原点（2026-10-04）

局部 SPEC：FormatMailTimestamp587768在4A5970四处/4A6450一处源码调用，对应原端五/两个时间点；同一现格式callee承接七点，每个实际发送后独立time/localtime、依分钟/小时/日/月/年取五S32、原%4d/%2d/%2d %02d:%02d/年+1900/月+1写未清64B，实时Input记对应本地记录，不缓存整批或改月日补齐。迷你category2另S32 key/T*名字/T*正文按新%d|%s|%s写未清1024B combined，保持两次分区转义及整体1024转义/发送后534520/重取当前联系人。两现callee接八原点，原返回/门槛/源序/写序/所有其他正文保持，4A6450本体无源码改变。

原612/460仅240/80新邻窗仅读，余372/380与前段/核心不重验；八原点、两现callee/两个原父、既有时间及新迷你字节/参数-缓冲分区-消费者-返回限定静态核，各call0/data1只登记。语义996→998，其他995旧记录/pending18/分类4及其余顶层保持；格式调用695→703/已确认1023（702直接引用+1既有补证），余320；格式164→165及原1022直接引用/唯一补证明细保持。 完整邮件/时钟/CRT/画面/运行未闭合。


## 宠物邮件4A6AC0附件数量一原格式（2026-10-04）

局部 SPEC：仅一现callee接一原点，原无参/U8真实476570(mode2)返回与完整正文保持。value0C!=-1门槛及元素4无论有无物品都visible0、原独立读槽/图号→当前窗口Y/X物品图消费保持；绘图后再读槽/count普通S32，正值以既有%3d写未清64B。格式后重取当前窗口Y→X，分别+232/+174取低S16、层3/字体1/颜色1及两个尾0文字消费。收件人/宠物名字/两个可用性查询与颜色、末另取窗口Y/X→476570原型57E740/mode2/实际返回正文不动，不缓存槽位或窗口、改hasItem门槛、清零、钳制、添保护或改关联方。

原166指令统计，仅39条绘后重读槽/count门槛/格式/后取窗口坐标-文字消费及末Input调用邻窗仅读，余127与已过前段/核心不重验；一原点/callee、S32/既有原字节/未清64B/低S16及3-1-1消费/真实U8返回限定静态核。普通call1/data0只登记，不推完整宠物邮件父链，未构建执行。 语义998→999，其他997旧记录/pending18/分类4及其余顶层保持；格式调用703→704/已确认1023（703直接引用+1既有补证），余319；格式165及原1022直接引用/唯一补证明细保持。 完整物品/宠物数据/父路由/CRT/画面/运行未闭合。


## 状态栏470AC0/4718F0共用生命/FP六原格式（2026-10-04）

局部 SPEC：FormatStatusBarVitals仅两父各调用一次，四现callee承接六原点；各父战斗HP/普通HP两原点，共享FP一原点。保留场景/位置<20及初始action门槛，战斗最大HP用初始action、当前HP及两FP各重读位置/action，World回退按最大后当前读取；两S32/%4d/%4d写未清256B，HP同源24B含NUL消费后才取FP，FP两分支在当前读/格式汇入一点。FP同源28B由470AC0在strlen+1后、复制前取timeWindow再memcpy，4718F0直接strcpy不加窗口采样；两父所有timer/提示/闪烁/时长/void正文不变，只helper四格式标识符替换。

原463/298仅各97新来源-格式-复制/相邻消费邻窗仅读，余366/201及前段/核心不重验，各call1/data0按当前尾函数身份登记。六原点/四现callee/两个原父/既有字节-S32对-256B及24/28B owner-源序-复制时点/void限定静态核。语义999→1001，其他998旧记录/pending18/分类4及其余顶层保持；格式调用704→710/已确认1023（709直接引用+1既有补证），余313；格式165及原1022直接引用/唯一补证明细保持。 完整状态生产/父路由/CRT/画面/运行未闭合。


## 2026-10-04：旧54行选择发送格式

局部SPEC按工程委托采用既有53B304，仅替换LegacyUiRecord的4D3410格式标识符。原4D3543：5531A4 `%d`（256400）/S32，按钮非零或chosen>=0才以chosen-新读D673D4+1写未清512B；仅按钮命中仍chosen=-1。随后52D2D0到+256/cap255，target/request/Y/X/socket→512620原按钮掩码，弃返回；关闭/仅按钮SE79、后续绘制与完整503440返回不变。

新读53/207原邻窗，其余154沿既有父合同；incoming/data均0，仅此一原点计数，不视为旧路由覆盖。调用累计710→711/1023（710直接+唯一原字节补证1），格式165保持，semantic1001→1002。限定源一标识符、JSON、文档仅末追加、编码与差异静态核对通过；旧面板及相关生产、原父/路由、运行/CRT/总PLAN仍未完成。禁构建/测试/运行/哈希要求保持。


## 2026-10-04：旧技能属性目标列表三处格式

局部SPEC按工程委托采用既有53B304，仅替换LegacyUiRecord的4EFB70三标识符：4EFE86等级S32/553FCC `%3d`、4EFEC7名称T*/553FE0 `%-16s`、4EFF2F四S32/58931C `%4d/%4d %4d/%4d`。共用128B、原17B名称及最小宽度、每轮活数量与格式后布局/色采样保持；四数先maxFP/FP/maxHP/HP，传参HP/maxHP/FP/maxFP。绘制间隔、伤势图、关闭/网络/字节待处理/提示与完整尾返回均不动，不加预清或成功门禁。

新读原邻窗92/508，余416沿既有父合同，新58931C格式16B直接取证；incoming/data均0，旧路由继续未闭合。新增三原点：711→714/1023（713直接+唯一补证1），格式165→166，semantic1002→1003。只核新增差异，源三标识符、JSON、文档末追加/编码与差异通过；不构建/测试/运行/哈希，原父/路由/运行/CRT/总PLAN未完成。


## 2026-10-04：旧技能/物品使用目标两页的页码格式

局部SPEC按工程委托分别采用既有53B304：4F0460@4F0808、4E4A50@4E4D1D各一原点，586D48 `%d/%d`（25642f256400）/页号+1、页数两S32/128B。保留各自先快照多页门槛、标题后独立读页号→页数、格式后重取本页面板Y/X绘X+270/Y+24、随后五行绘制入口另取页号。各自唯一owner/记录、选择/发送/关闭/提示差异、缓存更新、按钮/边框与完整返回不动，不新增格式前预清、保护或成功门禁。

两父原522/295条，各新读44邻窗、余478/251沿既有父合同；incoming/data各0，不代表路由闭合。新增两原点：714→716/1023（715直接+唯一补证1），格式166保持，semantic1003→1005。限定新差异静态核对通过并精确提交；禁构建/测试/运行/哈希，原父/路由/收发/画面/生命周期/运行/CRT/总PLAN仍未完成。


## 2026-10-04：旧固定文字提示四处格式

局部SPEC按工程委托采用既有53B304，只替换4F0CD0@4F0D7E/4F0DBE、4F0E20@4F0ECE/4F0F0E。原格式589C24/589C08/589C4C/589C34均完整旧字节且不含25H，不消费变参，原未清256B、反斜线/竖线/空格及两格式间绘制、各格式后重读面板Y/X保持。第二条原D2F798/CF3B08额外W32沿已批准无消费合同省略，不补未知owner/值/类型，不称生产或访问故障等价完成。状态0/2、确认关闭/SE57后继续帧、按钮/图号与完整返回均不动。

两父各62/93只读新邻窗、余31沿既有合同，四完整格式与现常量逐字节静态对应，incoming/data各0。新增四原点：716→720/1023（719直接+唯一补证1），格式166→170，semantic1005→1007。源仅四标识符、JSON/文档末追加/编码和差异核对通过；不构建/测试/客户端/哈希，未知生产、原父/旧路由/画面/生命周期/运行/CRT/总PLAN未完成。


## 2026-10-04：旧宠物技能目录五处格式

局部SPEC按工程委托只替换4D7470五标识符：4D76EF绝对index S32/5531A4 `%d`/16B→独立16B/cap15，先发布相对C8AF70再fmt/转义→原顺序发送mode0；4D7861真实gold S32/5889A8 `%7d`/256B；4D78AF页号+1/页数两S32/586D48 `%d/%d`；4D7973同源96B记录+4 S32/553FDC `%4d`；4D79DB+8费用S32/5889A8 `%7d`。四显示点共用未清256B，各格式后Y/X重取。每有效行名称绘后新页号/value、value绘后新页号/gold/price及先比较色0/6再fmt保持，原未消费EBP变参不新增值。失败/翻页/预览/关闭与完整返回不动。

新读194/570邻窗、余376沿既有父合同，incoming/data均0；四格式沿已核字节。调用720→725/1023（724直接+唯一补证1），格式170保持，semantic1007→1008。限定源五标识符、JSON/文档末追加/编码与差异静态核对通过；不补CA0478容量/生产、CC3E64图号或C8AF84页数，D45F74定义既有不再报缺。原父/旧路由/网络wire/画面/生命周期/运行/CRT/总PLAN未完成，禁构建/测试/客户端/哈希保持。


## 2026-10-04：旧候选宠物页五处格式

局部SPEC按工程委托只替换4D7C20五原格式点：4D7EB0目录值S32/553FDC `%4d`、4D7F04目录价S32/5889A8 `%7d`、4D7FA0候选值S32/553FCC `%3d`、4D8036真实金额S32/5889A8 `%7d`均同未清64B；4D812B相对hover S32/5531A4 `%d`写16B→独立16B/cap15转义→原mode0发送。技能名称后两次分别页号/目录选择/+4与/+8，候选仅used绘制、名称前allowed色0/7快照且名称绘后才读+4、各格式后面板Y/X及字体区别保持。发送后才发布C8A94C，取消593850和全部原副作用/返回不变。

新读210/454邻窗、余244沿既有父合同，四格式已核字节，incoming/data0。调用725→730/1023＝729直接+唯一补证1，格式170保持，semantic1008→1009。限定新增差异静态核对通过、精确提交；目录三数据缺口、原父/旧路由/完整生命周期/运行/CRT/总PLAN未完成，不构建/测试/客户端/哈希。


## 2026-10-04：旧技能槽页五处格式

局部SPEC按工程委托只改4D8230五格式接口：4D84C0目录+4 S32/553FDC `%4d`，4D8514目录+8 S32/5889A8 `%7d`，4D85AD候选+4 S32/553FCC `%3d`，4D8639技能槽+4 S32/553FDC `%4d`，4D87A0真实金额S32/5889A8 `%7d`，均共用原未清64B。目录两次分别新页号/选择/值或价，候选名称绘后重读选择/值，92B技能槽名称绘后读值，原说明后才取金额，各格式后新Y/X及字体保持。点击/可用图依slots、名字/值依used、hover说明不查used的差异与原完整返回不改。

新读217/509邻窗、余292沿既有父合同，三格式已核字节，incoming/data0。调用730→735/1023＝734直接+唯一补证1，格式170保持，semantic1009→1010。限定源五标识符、JSON/文档末追加/编码与差异静态核对通过；目录三数据缺口、原父/旧路由/画面/生命周期/运行/CRT/总PLAN未完成，禁构建/测试/客户端/哈希保持。


## 2026-10-04：旧宠物技能确认页六处格式

局部SPEC按工程委托只改4D88C0六点：4D89D2槽号S32/5531A4 `%d`/16B直接mode0发送无转义；4D8A82技能T*/588344、4D8AE2候选名称T*/588338、4D8B54已用槽名称T*/58830C、4D8C13金额S32/588D5C、4D8C67目录价S32/588D48均同未清256B。各次目录/选择/字段采样、技能/槽测实际buffer、宠物4D8AEE测固定588320却画真实buffer、空槽只strcpy不新增格式点、原前缀/空白/%7d/后缀和格式后布局保持。原状态/按钮/关闭/SE与完整返回不改。

新读236/363邻窗、余127沿既有父合同，四既有格式沿原证，新588D5C/588D48直接读完整串并与现常量静态对应；incoming/data0。调用735→741/1023＝740直接+唯一补证1，格式170→172，semantic1010→1011。限定源六标识符、JSON/文档末追加/编码和差异核对通过；目录三外部缺口、原父/旧路由/网络wire/画面/生命周期/运行/CRT/总PLAN未完成，禁构建/测试/客户端/哈希保持。
