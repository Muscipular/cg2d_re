#pragma once

#include <cstdint>

namespace cg::platform {

struct LegacyFile;

// 原1085FE0共享S32错误槽；文件查询/目录创建或切换写入，截图读取17判断目录已存在。
extern std::int32_t gLegacyErrno_1085FE0;
extern std::uint32_t gLegacyDosError_1085FE4; // 原系统错误槽，关闭/提交分别按原合同写入。
// 原542F44；复用已有错误表及共享槽，返回原表尾关联值，不另建错误映射。
std::uintptr_t MapLegacyFileError_542F44(std::uint32_t code);

// 原53DA45/53DA5C；S32解析，end借用原字节游标，失败/溢出沿旧共享错误槽。
std::int32_t Strtol_53DA45(const char* text, char** endPointer, std::int32_t radix);

// 原53C649；转发到既有Atol_53C5BE，返回原S32数值，不新增转换或校验。
std::int32_t Atoi_53C649(const char* text);

// 原54ABDD；按原W32/进制写数字文本并返回buffer，十进制负数才添加负号。
char* Itoa_54ABDD(std::int32_t value, char* buffer, std::int32_t radix);

// 原53D26E；原字节分词并就地截断，空text续用同一进程共享借用位置。
char* StrTok_53D26E(char* text, const char* delimiters);

// 原53BFC0；U32有界左侧预扫、右/左比较和末位置复读，数量零不读文本。
std::int32_t __cdecl StrNCmp_53BFC0(const char* left, const char* right, std::uint32_t count);

// 原53F9E0；源对齐探零/候选复读定位首NUL，返回U32字节数。
std::uint32_t __cdecl StrLen_53F9E0(const char* text);

// 原544DB0；复用544E21源对齐复制尾，含NUL复制并返回原target。
char* __cdecl StrCpy_544DB0(char* target, const char* source);

// 原544DC0；先目的对齐扫描/候选复读找NUL，再共用复制尾，返回原target。
char* __cdecl StrCat_544DC0(char* target, const char* source);

// 原53C790；U32最多追加count源字节，耗尽补单个NUL，count0不访问文本；返回原target。
char* __cdecl StrNCat_53C790(char* target, const char* source, std::uint32_t count);

// 原53BEC0；U32计数借用复制，遇NUL补至count，满count无NUL不加终止；返回原target。
// 输入与交易载荷复用LegacyTime已有本体，源对齐读取/复读与分段补零次序保持。
char* __cdecl StrNCpy_53BEC0(char* target, const char* source, std::uint32_t count);

// 原53C2F0；低W8目标按源对齐/候选复读搜索，返回借用首匹配或空，目标0可命中NUL。
char* __cdecl StrChr_53C2F0(const char* text, std::int32_t target);

// 原53BE40；空needle返回原源，单字节复用StrChr，按原两字节比对返回借用首匹配或空。
char* __cdecl StrStr_53BE40(const char* text, const char* substring);

// 原53C654；旧字节路径交CreateDirectoryA，非零错误经542F44写共享槽，返回0/-1。
std::int32_t CreateLegacyDirectory_53C654(const char* path);

// 原53DD2F；切目录后更新驱动器环境项，失败沿542F44，不回滚已发生的目录改变。
std::int32_t ChangeLegacyDirectory_53DD2F(const char* path);

// 原0x1086128的九个S32值槽；多个调用方借用同一可变对象，不是现代CRT tm。
struct LegacyTm53B8C6 {
    std::int32_t seconds;       // +00 秒
    std::int32_t minutes;       // +04 分
    std::int32_t hours;         // +08 时
    std::int32_t day;           // +0C 月内日
    std::int32_t month0based;   // +10 从零起的月份
    std::int32_t yearMinus1900; // +14 距1900年
    std::int32_t weekDay;       // +18 星期
    std::int32_t yearDay0based; // +1C 从零起的年内日
    std::int32_t dst;           // +20 夏令时标记
};

// 原 _time@[0x53BA26,0x53BB02)，保留 32 位结果及原时区采样、缓存、转换调用。
// output 可以为 nullptr；其他值须指向可写 int32_t。未以本机 time64/time32 代替原函数。
std::int32_t ReadLegacyTime32_53BA26(std::int32_t* output);

// 原53CCD0在auto.dat六个点的创建时间投影；file借用仍打开的原FILE槽。
// descriptor交同一完整文件状态本体，成功才发布ctime，失败不写output。
std::int32_t ReadLegacyFileCreationTime32(LegacyFile* file, std::int32_t* output);

} // namespace cg::platform
