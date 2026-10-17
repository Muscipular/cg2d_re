# SDL3 本地源码与静态链接 SPEC

日期：2026-10-17。状态：源码与静态链接接入完成，Debug构建、PE导入项及连续构建缓存验收通过。

## 当前事实

用户要求将SDL3复制到本子项目3rd目录并改为静态链接。当前依赖为 `third_party/sdl/SDL3-3.4.8` 的VC开发包；本地cmake/SDL3Config.cmake声明SDL3.lib为IMPORTED_IMPLIB、SDL3.dll为IMPORTED_LOCATION，创建SHARED IMPORTED目标，并明确设置 `SDL3_SDL3-static_FOUND FALSE`。SDL3_test.lib仅为测试支持库，不能替代SDL3实现库。当前third_party/sdl下未发现SDL3源码构建目录或源码压缩包。

因此，直接复制现有SDK可以迁移位置，但不能完成静态链接；也不能把导入库改名为静态目标。推荐取得同版本3.4.8源码，纳入 `cg2d_re/3rd/SDL3` 后随工程编译。

## 拟执行范围

1. 取得SDL官方release-3.4.8源码，放入3rd/SDL3，保留LICENSE.txt及上游来源／版本说明；不升级版本、不下载SDL_image或SDL_ttf、不添加自动联网下载逻辑。原third_party SDK保留，根工程不改。
2. 本子项目移除旧SDL3_DIR及find_package路径依赖，改用add_subdirectory(3rd/SDL3 EXCLUDE_FROM_ALL)。缺失源码时明确报错，不静默回退至DLL SDK。
3. 明确配置SDL_STATIC=ON、SDL_SHARED=OFF；关闭SDL_TEST_LIBRARY、SDL_TESTS、SDL_EXAMPLES、SDL_INSTALL等非必要目标。保留SDL音频、窗口及渲染等现有能力，不用裁剪功能来解决链接问题。
4. SDL使用系统CRT，子目录作用域内指定CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded，使SDL自身及内部对象目标匹配当前客户端各配置的/MT合同；不全局改变其他工程或LuaJIT的CRT选择。
5. core、startup、render及历史工具的链接项显式使用SDL3::SDL3-static，由上游目标传递静态依赖及头文件要求；不自行枚举或猜测Win32系统库，不手工模拟SDL导出宏。
6. 删除客户端与历史工具的SDL DLL POST_BUILD复制命令。历史工具仍EXCLUDE_FROM_ALL，不恢复构建。既有输出目录DLL不自动删除。
7. 保持TAOMOLI选项，将同名业务宏限定到全部自有目标，避免污染第三方编译命令；NOMINMAX／WIN32_LEAN_AND_MEAN继续全局定义。业务源码、SDL调用顺序及资源生命周期不改。源码纳入Git，生成物仍留在已忽略build目录；不提交下载缓存。

上游3.4.8的[构建文档](https://raw.githubusercontent.com/libsdl-org/SDL/release-3.4.8/docs/README-cmake.md)明确支持源码子目录接入及SDL3::SDL3-static目标；相关选项见[该版本CMakeLists.txt](https://raw.githubusercontent.com/libsdl-org/SDL/release-3.4.8/CMakeLists.txt)。取得完整源码后继续核对其CRT设置、内部目标及依赖，避免仅凭目标名称认定静态链接成功。

## 验收与执行限制

静态核对版本、许可证、源码目标、CRT合同、全部链接项与DLL复制步骤；显式UTF-8读取项目文本，第三方文件按原字节复制，保留上游编码与换行。进度同步本线唯一记录，分步精确提交并排除用户已有规则、协议字典及采样改动。

按本次静态接入及缓存要求运行build.bat Debug进行SDL静态库与客户端编译／链接，并只读检查客户端PE导入项，确认不再导入SDL3.dll；不运行客户端或测试，不部署，不计算文件摘要。配置／编译失败时修复本次接入问题，不删功能、不恢复动态链接来代替验收。

本线AGENTS.md第5条禁止自行引入／下载外部依赖；前一步因此先调查并给出同版本源码方案。用户随后要求本次同时做好CMake依赖缓存，现按该静态接入方案及新增约束实施，仅取得已指定SDL3的同版本官方源码，不扩大到其他依赖。

## 新增：第三方增量缓存

复用已有build/windows-msvc-x64目录，SDL、yaml-cpp和LZ4拥有各自目标及稳定对象目录；正常构建不清缓存、不强制重建、不重复下载解压。保留CMake正常重配置和依赖检查，不以跳过配置掩盖源码或工具链变化。TAOMOLI仅传给自有目标，SDL使用发布包固定REVISION.txt，避免业务宏或宿主提交号造成依赖重编。

首次构建完成后，对第三方对象文件和静态库记录路径、长度、修改时间；不改源码连续再执行一次build.bat Debug，核对上述记录完全保持。读取生成工程确认第三方没有TAOMOLI且SDL使用MultiThreaded。Debug／Release各自保存配置产物；工具链、架构、依赖源码或公共选项变化仍正常重编。实现说明见[cg2d_sdl3_vendor.md](cg2d_sdl3_vendor.md)。

## 执行记录

2026-10-17：官方3.4.8发布包2180个文件按原字节复制到3rd/SDL3，保留许可证及版本记录。顶层改为只构建SDL3-static，SDL子目录指定/MT，去除两处DLL复制规则，业务宏改为自有目标私有定义。旧SDK及用户协议字典保持；下载与解压缓存位于忽略的build目录，不提交。

2026-10-17验收：首次build.bat Debug退出0，120条警告；紧接着无改动再次执行，退出0，耗时约2.6秒，本轮0条警告。SDL3的271个、yaml-cpp的30个、LZ4的2个对象／静态库文件共303项，路径、长度及修改时间全部保持；增量轮次无警告不代表全量警告已清理。三方生成工程均在四种配置使用MultiThreaded，且不含TAOMOLI宏。只读检查客户端PE导入表，无SDL3.dll。未构建其他配置、运行测试或客户端、部署或计算摘要。
