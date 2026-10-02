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
