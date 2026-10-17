# TMD SDK 接入说明

2026-10-17，按用户要求迁入twcg_hook的3rd/tmd及include/vm.h封装，并复制用户提供的SecureEngineSDK64.lib。

## 文件及开关

- `3rd/tmd/`保留原SDK目录的10个文件，逐字节复制，不改厂商头文件；额外加入用户提供的x64库。原32位库及VMProtect文件仅随目录保存，当前客户端不链接它们。
- `include/cg/core/vm.h`迁入原Themida宏封装，保留VM_START／END、VM_LIGHT、VM_HARD、VM_HARD2及原调试分支的空宏。取消头文件硬编码TMD，改由CMake控制；原文件未启用的VMProtect替代分支不接入。
- CMake选项`TMD`默认OFF；使用`-DTMD=ON`启用非Debug配置的TMD编译宏、SDK包含目录及SecureEngineSDK64.lib链接依赖。Debug仍使用MY_DEBUG空宏，不包含SDK头、不链接SDK库。
- `cg2d_re_tmd`为INTERFACE目标；2026-10-18自有源码统一归入EXE后，只连接cg2d_re_client，不传播至SDL、yaml-cpp、LZ4或LuaJIT，也不重新构建这些依赖。

需要标记的源码包含`cg/core/vm.h`，在业务区域直接成对使用相应START／END宏。此次仅迁入SDK和封装、接通构建配置，未自行选择或插入业务保护区域，未执行加壳。

## 库与验证范围

用户提供的库经COFF归档静态读取确认架构为0x8664（x64），封装使用的FALCON／PUMA以及原调试兼容宏对应符号、字符串保护符号均存在。该文件是指向SecureEngineSDK64.dll的导入库，不是可按/MT重编的第三方源码库；实际SDK调用的运行依赖须提供匹配DLL，当前用户未提供该DLL。

SDK目录10个文件及额外x64库逐字节复制一致，未计算摘要；自有CMake和封装头按UTF-8读取，差异检查通过。未配置、编译、链接、运行或加壳，不能将静态符号检查视为完整链接或保护效果验收。用户随后明确要求提交代码，本次接入与文档按授权提交。
