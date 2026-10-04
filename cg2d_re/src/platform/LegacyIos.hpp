#pragma once

#include <cstddef>
#include <cstdint>

namespace cg::platform {

struct LegacyIosBaseStorage;
struct LegacyLocaleImplementation;

// 原删除槽返回所属对象地址；仅该多态边界为非拥有void*，真实本体仍类型化。
// 原550144仅已证删除槽0；8B为消费前缀，不推完整SDK虚表/分配规模。
struct LegacyIosBaseMethods final {
    void* (*deletingDestructor)(
        LegacyIosBaseStorage* storage, std::uint32_t flags);
};
static_assert(sizeof(LegacyIosBaseMethods) == 8);
extern const LegacyIosBaseMethods gLegacyIosBaseMethods550144;

// 原登记地址数组/独立U8计数，仅真实外部声明；未加载初值/实际容量不补证为零或9/12。
extern LegacyIosBaseStorage* gLegacyStandardStreamRegistry10866F8[];
extern std::uint8_t gLegacyStandardStreamReferences108671C[];

// 原回调call后由caller清12B栈：event/this/index三参，返回寄存器不消费。
using LegacyIosCallback = void (__cdecl *)(
    std::int32_t event, LegacyIosBaseStorage* storage, std::int32_t index);

// 原值链本组仅消费+00 next；这是节点头视图，不断言整节点容量或分配规模。
struct LegacyIosValueNodeHeader final {
    LegacyIosValueNodeHeader* next;
};

// 原回调节点已证+00..+0B视图；仅两地址拓宽，index保持S32，不推整节点分配规模。
struct LegacyIosCallbackNode final {
    LegacyIosCallbackNode* next; // 原+00。
    std::int32_t index;          // 原+04；传回调第三参数。
    LegacyIosCallback callback; // 原+08；cdecl三参，原不检查空函数。
};
static_assert(offsetof(LegacyIosCallbackNode, index) == 8);
static_assert(offsetof(LegacyIosCallbackNode, callback) == 16);
static_assert(sizeof(LegacyIosCallbackNode) == 24);

// 原ios_base已证+00..+27；数字保持W32位型，未消费符号/格式语义不另推定。
// 不自动构析/初始化，不与宿主std::ios_base互转，不建立登记表/回调影子状态。
struct LegacyIosBaseStorage final {
    const void* virtualTable;                         // 原+00；真实原生方法表前缀，不接SDK虚表。
    std::uint32_t stateBits;                          // 原+04；clear先发布低三位，实际SDK抛出提供者待。
    std::uint32_t exceptionBits;                      // 原+08。
    std::uint32_t formatBits;                         // 原+0C。
    std::uint32_t precisionBits;                      // 原+10；本组不解释符号性。
    std::uint32_t widthBits;                          // 原+14；本组不解释符号性。
    LegacyIosValueNodeHeader* valueHead;              // 原+18；实际值链节点头。
    LegacyIosCallbackNode* callbackHead;              // 原+1C；实际回调链。
    LegacyLocaleImplementation* localeImplementation; // 原+20；locale单指针字段。
    std::uint32_t registryIndex;                      // 原+24；原jnb/jb按U32比较。
};
static_assert(offsetof(LegacyIosBaseStorage, stateBits) == 8);
static_assert(offsetof(LegacyIosBaseStorage, valueHead) == 32);
static_assert(offsetof(LegacyIosBaseStorage, callbackHead) == 40);
static_assert(offsetof(LegacyIosBaseStorage, localeImplementation) == 48);
static_assert(offsetof(LegacyIosBaseStorage, registryIndex) == 56);
static_assert(sizeof(LegacyIosBaseStorage) == 64);

// 原54A23C：回调返回后才读当前node.next；不重取表头、预取next或增保护。
void CallLegacyIosCallbacks_54A23C(LegacyIosBaseStorage* storage, std::int32_t event);
// 原54A2CB：event0→值链释放→缓存回调头/清值头→回调链释放/清回调头。
void TidyLegacyIos_54A2CB(LegacyIosBaseStorage* storage);

// 原54A25E：按对象U32索引扫描至8，仍写最终槽/增U8计数，不增容量门禁。
void RegisterLegacyStandardStream_54A25E(LegacyIosBaseStorage* storage);
// 原54A293：locale先构造，再六字段/两链初始化，最后真实clear(0,0)。
void InitializeLegacyIosBase_54A293(LegacyIosBaseStorage* storage);
// 原clear状态/failure准备及展开已实现；低U8重抛/最高U8临时串前缀，实际SDK抛出待。
void ClearLegacyIos_549F62(
    LegacyIosBaseStorage* storage, std::uint32_t stateBits, std::uint32_t rethrowBits);
// 原完整析构及删除；共享U8按S8门禁，正常/展开locale次序与原堆域保持。
void DestroyLegacyIosBase_54A1D0(LegacyIosBaseStorage* storage);
LegacyIosBaseStorage* DeleteLegacyIosBase_54A220(
    LegacyIosBaseStorage* storage, std::uint32_t flags);

// 原basic_ios在ios_base之后的已消费+28/+2C/+30字段；仅已证地址拓宽。
// tie为真实ostream对象地址，尚未定义其虚基布局，不当作basic_ios地址互转。
template <typename Character>
struct LegacyStreamBufferObject;
template <typename Character>
struct LegacyOutputStreamStorage;
template <typename Character>
struct LegacyBasicIosStorage final {
    LegacyIosBaseStorage base;                         // 原+00..+27，真实首成员。
    LegacyStreamBufferObject<Character>* buffer;       // 原+28，真实非拥有streambuf。
    LegacyOutputStreamStorage<Character>* tiedStream;  // 原+2C，真实非拥有ostream。
    Character fill;                                   // 原+30，窄低U8/宽U16。
};
using LegacyNarrowBasicIos = LegacyBasicIosStorage<char>;
using LegacyWideBasicIos = LegacyBasicIosStorage<std::uint16_t>;
static_assert(offsetof(LegacyNarrowBasicIos, base) == 0);
static_assert(offsetof(LegacyNarrowBasicIos, buffer) == 64);
static_assert(offsetof(LegacyNarrowBasicIos, tiedStream) == 72);
static_assert(offsetof(LegacyNarrowBasicIos, fill) == 80);
static_assert(sizeof(LegacyNarrowBasicIos) == 88);
static_assert(offsetof(LegacyWideBasicIos, base) == 0);
static_assert(offsetof(LegacyWideBasicIos, buffer) == 64);
static_assert(offsetof(LegacyWideBasicIos, tiedStream) == 72);
static_assert(offsetof(LegacyWideBasicIos, fill) == 80);
static_assert(sizeof(LegacyWideBasicIos) == 88);

// 原两basic_ios表只已证删除槽0；沿真实ios_base首成员接口适配，8B为消费前缀。
// 不推完整SDK/虚基或分配布局，不擦除函数地址。
extern const LegacyIosBaseMethods gLegacyNarrowBasicIosMethods54FFBC;
extern const LegacyIosBaseMethods gLegacyWideBasicIosMethods550058;

// 原init：base返回后重取buffer、空才clear；registerBits只检查低U8。
void InitializeLegacyNarrowIos_548E65(
    LegacyNarrowBasicIos* storage, LegacyStreamBufferObject<char>* buffer,
    std::uint32_t registerBits);
void InitializeLegacyWideIos_549DBE(
    LegacyWideBasicIos* storage, LegacyStreamBufferObject<std::uint16_t>* buffer,
    std::uint32_t registerBits);
// 原析构先写各自basic_ios表再真实ios_base析构；本组this不作-4/-8调整。
void DestroyLegacyNarrowIos_54852C(LegacyNarrowBasicIos* storage);
void DestroyLegacyWideIos_549638(LegacyWideBasicIos* storage);
LegacyNarrowBasicIos* DeleteLegacyNarrowIos_548CF6(
    LegacyNarrowBasicIos* storage, std::uint32_t flags);
LegacyWideBasicIos* DeleteLegacyWideIos_549D4B(
    LegacyWideBasicIos* storage, std::uint32_t flags);

// 原虚基表两W32，首字未消费；偏移按实际x64成员位置，不把x86字节偏移当新端地址。
struct LegacyStreamVirtualBaseLayout final {
    std::uint32_t firstBits;
    std::int32_t iosByteOffset;
};
static_assert(offsetof(LegacyStreamVirtualBaseLayout, iosByteOffset) == 4);
static_assert(sizeof(LegacyStreamVirtualBaseLayout) == 8);

// 原ostream首vbtable地址/实际basic_ios虚基，输入另有未解释W32槽；不接SDK继承布局。
template <typename Character>
struct LegacyOutputStreamStorage final {
    const LegacyStreamVirtualBaseLayout* virtualBaseLayout;
    LegacyBasicIosStorage<Character> ios;
};
template <typename Character>
struct LegacyInputStreamStorage final {
    const LegacyStreamVirtualBaseLayout* virtualBaseLayout;
    std::uint32_t countBits; // 原+04；本组完整构造写0，登记构造不写，不推符号/业务含义。
    LegacyBasicIosStorage<Character> ios;
};
using LegacyNarrowOutputStream = LegacyOutputStreamStorage<char>;
static_assert(offsetof(LegacyNarrowOutputStream, virtualBaseLayout) == 0);
static_assert(offsetof(LegacyNarrowOutputStream, ios) == 8);
static_assert(sizeof(LegacyNarrowOutputStream) == 96);
extern const LegacyStreamVirtualBaseLayout gLegacyNarrowOutputVirtualBase54FFC0;
extern const LegacyIosBaseMethods gLegacyNarrowOutputMethods54FFB4;
LegacyNarrowOutputStream* ConstructLegacyNarrowOutput_548463(
    LegacyNarrowOutputStream* storage, LegacyStreamBufferObject<char>* buffer,
    std::uint32_t registerBits, std::uint32_t initializeBits, std::uint32_t virtualBaseBits);
LegacyNarrowOutputStream* RegisterConstructLegacyNarrowOutput_5484CD(
    LegacyNarrowOutputStream* storage, std::uint32_t ignoredStackWord, std::uint32_t virtualBaseBits);
void DestroyLegacyNarrowOutput_54823D(LegacyNarrowOutputStream* storage);
void* DeleteLegacyNarrowOutputFromIos_548CD7(LegacyIosBaseStorage* virtualBase, std::uint32_t flags);

using LegacyNarrowInputStream = LegacyInputStreamStorage<char>;
static_assert(offsetof(LegacyNarrowInputStream, virtualBaseLayout) == 0);
static_assert(offsetof(LegacyNarrowInputStream, ios) == 16);
static_assert(sizeof(LegacyNarrowInputStream) == 104);
static_assert(offsetof(LegacyNarrowInputStream, countBits) == 8);
extern const LegacyStreamVirtualBaseLayout gLegacyNarrowInputVirtualBase54FFC8;
extern const LegacyIosBaseMethods gLegacyNarrowInputMethods54FFAC;
LegacyNarrowInputStream* ConstructLegacyNarrowInput_548537(
    LegacyNarrowInputStream* storage, LegacyStreamBufferObject<char>* buffer,
    std::uint32_t registerBits, std::uint32_t virtualBaseBits);
LegacyNarrowInputStream* RegisterConstructLegacyNarrowInput_54859F(
    LegacyNarrowInputStream* storage, std::uint32_t ignoredStackWord, std::uint32_t virtualBaseBits);
void DestroyLegacyNarrowInput_5481F9(LegacyNarrowInputStream* storage);
void* DeleteLegacyNarrowInputFromIos_548D12(LegacyIosBaseStorage* virtualBase, std::uint32_t flags);

using LegacyWideOutputStream = LegacyOutputStreamStorage<std::uint16_t>;
static_assert(offsetof(LegacyWideOutputStream, virtualBaseLayout) == 0);
static_assert(offsetof(LegacyWideOutputStream, ios) == 8);
static_assert(sizeof(LegacyWideOutputStream) == 96);
extern const LegacyStreamVirtualBaseLayout gLegacyWideOutputVirtualBase55005C;
extern const LegacyIosBaseMethods gLegacyWideOutputMethods550050;
LegacyWideOutputStream* ConstructLegacyWideOutput_549525(
    LegacyWideOutputStream* storage, LegacyStreamBufferObject<std::uint16_t>* buffer,
    std::uint32_t registerBits, std::uint32_t initializeBits, std::uint32_t virtualBaseBits);
LegacyWideOutputStream* RegisterConstructLegacyWideOutput_54958F(
    LegacyWideOutputStream* storage, std::uint32_t ignoredStackWord, std::uint32_t virtualBaseBits);
void DestroyLegacyWideOutput_549324(LegacyWideOutputStream* storage);
void* DeleteLegacyWideOutputFromIos_549D2C(LegacyIosBaseStorage* virtualBase, std::uint32_t flags);

using LegacyWideInputStream = LegacyInputStreamStorage<std::uint16_t>;
static_assert(offsetof(LegacyWideInputStream, virtualBaseLayout) == 0);
static_assert(offsetof(LegacyWideInputStream, ios) == 16);
static_assert(sizeof(LegacyWideInputStream) == 104);
static_assert(offsetof(LegacyWideInputStream, countBits) == 8);
extern const LegacyStreamVirtualBaseLayout gLegacyWideInputVirtualBase550064;
extern const LegacyIosBaseMethods gLegacyWideInputMethods550048;
LegacyWideInputStream* ConstructLegacyWideInput_549643(
    LegacyWideInputStream* storage, LegacyStreamBufferObject<std::uint16_t>* buffer,
    std::uint32_t registerBits, std::uint32_t virtualBaseBits);
LegacyWideInputStream* RegisterConstructLegacyWideInput_5496AB(
    LegacyWideInputStream* storage, std::uint32_t ignoredStackWord, std::uint32_t virtualBaseBits);
void DestroyLegacyWideInput_5492E0(LegacyWideInputStream* storage);
void* DeleteLegacyWideInputFromIos_549D67(LegacyIosBaseStorage* virtualBase, std::uint32_t flags);

// 原独立标准流初始化计数；已加载初值-1，S32/两次实际读取，不增锁或一次性门禁。
extern volatile std::int32_t gLegacyNarrowStandardInitCount592894;
extern volatile std::int32_t gLegacyWideStandardInitCount5929EC;
// 原14个全局对象仅真实外部声明；首字节未加载，不推零初值/完整SDK分配或建立影子owner。
template <typename Character>
struct LegacyFileBufferStorage;
extern LegacyFileBufferStorage<char> gLegacyNarrowStandardInputBuffer1086470;
extern LegacyFileBufferStorage<char> gLegacyNarrowStandardOutputBuffer1086348;
extern LegacyFileBufferStorage<char> gLegacyNarrowStandardErrorBuffer10863D8;
extern LegacyNarrowInputStream gLegacyNarrowStandardInput1086430;
extern LegacyNarrowOutputStream gLegacyNarrowStandardOutput10863A0;
extern LegacyNarrowOutputStream gLegacyNarrowStandardError10864C8;
extern LegacyNarrowOutputStream gLegacyNarrowStandardLog1086310;
// 原控制器this仅保存/返回，不读取字段；不猜Init对象容量或自动接启动。
void* InitializeLegacyNarrowStandardStreams_5482DD(void* initializer);
extern LegacyFileBufferStorage<std::uint16_t> gLegacyWideStandardInputBuffer1086610;
extern LegacyFileBufferStorage<std::uint16_t> gLegacyWideStandardOutputBuffer10865B8;
extern LegacyFileBufferStorage<std::uint16_t> gLegacyWideStandardErrorBuffer10866A0;
extern LegacyWideInputStream gLegacyWideStandardInput1086578;
extern LegacyWideOutputStream gLegacyWideStandardOutput1086668;
extern LegacyWideOutputStream gLegacyWideStandardError1086500;
extern LegacyWideOutputStream gLegacyWideStandardLog1086540;
// 原控制器this仅保存/返回，不读取字段；不猜Init对象容量或自动接启动。
void* InitializeLegacyWideStandardStreams_549397(void* initializer);
// 原构造展开只当前state的一次54843B；两实参是原实际地址，叶不消费/不析构对象。
void UnwindLegacyStandardStream_54843B(void* currentObject, void* fixedObject);

// 原5495EE窄宽共用刷新；状态&6门禁，实际sync完整-1才重取虚基/clear。
template <typename Character>
LegacyOutputStreamStorage<Character>* FlushLegacyOutput_5495EE(
    LegacyOutputStreamStorage<Character>* storage);
// 原退出包装this未消费，只递减独立W32计数，0时按输出/错误/日志刷新；不直接析构流。
void ReleaseLegacyNarrowStandardStreams_5491F6();
void ReleaseLegacyWideStandardStreams_549220();
// 原两CRT项/晚段对象动作及原退出回调；只显式调用，不自动接当前启动。
std::int32_t InitializeLegacyNarrowErrorBuffer_548252();
LegacyFileBufferStorage<char>* ConstructLegacyNarrowErrorBufferLate_54825C();
void DestroyLegacyNarrowErrorBufferAtExit_548275();
std::int32_t InitializeLegacyWideErrorStream_549339();
LegacyWideOutputStream* RegisterLegacyWideErrorStreamLate_549343();
void DestroyLegacyWideErrorStreamAtExit_54935E();

// 原五缓冲CRT项：晚段只原表写/返this，随后登记真实退出析构；不重建FILE/locale。
LegacyFileBufferStorage<char>* ConstructLegacyNarrowInputBufferLate_54817A();
void DestroyLegacyNarrowInputBufferAtExit_548193();
std::int32_t InitializeLegacyNarrowInputBuffer_548170();
LegacyFileBufferStorage<char>* ConstructLegacyNarrowOutputBufferLate_5481A7();
void DestroyLegacyNarrowOutputBufferAtExit_5481C0();
std::int32_t InitializeLegacyNarrowOutputBuffer_54819D();
LegacyFileBufferStorage<std::uint16_t>* ConstructLegacyWideInputBufferLate_549234();
void DestroyLegacyWideInputBufferAtExit_54924D();
std::int32_t InitializeLegacyWideInputBuffer_54922A();
LegacyFileBufferStorage<std::uint16_t>* ConstructLegacyWideOutputBufferLate_549261();
void DestroyLegacyWideOutputBufferAtExit_54927A();
std::int32_t InitializeLegacyWideOutputBuffer_549257();
LegacyFileBufferStorage<std::uint16_t>* ConstructLegacyWideErrorBufferLate_54928E();
void DestroyLegacyWideErrorBufferAtExit_5492A7();
std::int32_t InitializeLegacyWideErrorBuffer_549284();

// 原两个Init对象地址只传给控制器/返回，无字段证据；保持不完整类型/extern，不造1B替身。
struct LegacyStandardStreamInitializer;
extern LegacyStandardStreamInitializer gLegacyNarrowStandardInitializer1086539;
extern LegacyStandardStreamInitializer gLegacyWideStandardInitializer1086538;
void* ConstructLegacyNarrowStandardInitializer_5491E0();
void* ConstructLegacyWideStandardInitializer_54920A();
std::int32_t RegisterLegacyNarrowStandardInitializer_5491D6();
std::int32_t RegisterLegacyWideStandardInitializer_549200();

// 原窄输入流CRT三入口：登记构造(0,1)/同对象析构/真实退出登记返回。
LegacyNarrowInputStream* RegisterLegacyNarrowInputStreamLate_5481D4();
void DestroyLegacyNarrowInputStreamAtExit_5481EF();
std::int32_t InitializeLegacyNarrowInputStream_5481CA();

// 原窄输出流CRT三入口：登记构造(0,1)/同对象析构/真实退出登记返回。
LegacyNarrowOutputStream* RegisterLegacyNarrowOutputStreamLate_548218();
void DestroyLegacyNarrowOutputStreamAtExit_548233();
std::int32_t InitializeLegacyNarrowOutputStream_54820E();

// 原窄错误流CRT三入口：登记构造(0,1)/同对象析构/真实退出登记返回。
LegacyNarrowOutputStream* RegisterLegacyNarrowErrorStreamLate_548289();
void DestroyLegacyNarrowErrorStreamAtExit_5482A4();
std::int32_t InitializeLegacyNarrowErrorStream_54827F();

// 原窄日志流CRT三入口：登记构造(0,1)/同对象析构/真实退出登记返回。
LegacyNarrowOutputStream* RegisterLegacyNarrowLogStreamLate_5482B8();
void DestroyLegacyNarrowLogStreamAtExit_5482D3();
std::int32_t InitializeLegacyNarrowLogStream_5482AE();

// 原宽输入流CRT三入口：登记构造(0,1)/同对象析构/真实退出登记返回。
LegacyWideInputStream* RegisterLegacyWideInputStreamLate_5492BB();
void DestroyLegacyWideInputStreamAtExit_5492D6();
std::int32_t InitializeLegacyWideInputStream_5492B1();

// 原宽输出流CRT三入口：登记构造(0,1)/同对象析构/真实退出登记返回。
LegacyWideOutputStream* RegisterLegacyWideOutputStreamLate_5492FF();
void DestroyLegacyWideOutputStreamAtExit_54931A();
std::int32_t InitializeLegacyWideOutputStream_5492F5();

// 原宽日志流CRT三入口：登记构造(0,1)/同对象析构/真实退出登记返回。
LegacyWideOutputStream* RegisterLegacyWideLogStreamLate_549372();
void DestroyLegacyWideLogStreamAtExit_54938D();
std::int32_t InitializeLegacyWideLogStream_549368();

// 原C++表前段两个独立Init对象仅实际地址声明，容量/初值未证，不造对象替身。
extern LegacyStandardStreamInitializer gLegacyNarrowEarlyStandardInitializer628FE9;
void* ConstructLegacyNarrowEarlyStandardInitializer_453E10();
void ReleaseLegacyNarrowEarlyStandardStreamsAtExit_453E30();
std::int32_t InitializeLegacyNarrowEarlyStandardStreams_453E00();

extern LegacyStandardStreamInitializer gLegacyWideEarlyStandardInitializer628FE8;
void* ConstructLegacyWideEarlyStandardInitializer_453E50();
void ReleaseLegacyWideEarlyStandardStreamsAtExit_453E70();
std::int32_t InitializeLegacyWideEarlyStandardStreams_453E40();

} // namespace cg::platform
