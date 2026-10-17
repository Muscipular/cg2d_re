# SDL3 本地依赖

版本：3.4.8。源码目录：`3rd/SDL3`。

来源：[SDL官方3.4.8源码发布包](https://github.com/libsdl-org/SDL/releases/download/release-3.4.8/SDL3-3.4.8.zip)。取得日期：2026-10-17。目录保留发布包原文件及LICENSE.txt；本工程配置不修改上游源码。

SDL3随工程静态构建，使用上游SDL3::SDL3-static目标及其依赖信息，所有配置匹配客户端/MT。测试库、测试、示例和安装目标关闭。原third_party下的动态SDK不再用于本子项目，其他工程可继续使用。

## 增量构建

- 持续使用现有windows-msvc-x64预设和build/windows-msvc-x64目录。正常构建复用CMakeCache、对象文件和静态库，不执行clean、--fresh或重新解压依赖。
- Debug与Release由同一多配置生成器维护各自对象文件，不混用编译选项。首次切换配置会构建该配置缺少的产物，此后各自复用。
- TAOMOLI仅定义到自有目标，切换业务分支不改变SDL3、yaml-cpp或LZ4编译命令。NOMINMAX／WIN32_LEAN_AND_MEAN仍为稳定的全局定义。
- 使用官方发布包中的REVISION.txt，SDL版本信息不随宿主仓库提交变化；上游configure_file在内容不变时保留生成头文件时间戳。
- 修改依赖源码、公共编译选项、架构或工具链时，仍需正常重编受影响的依赖，不能用跳过构建掩盖缓存失效。

本项目使用CMake/MSBuild原生增量构建，不增加外部编译缓存工具。配置步骤重复运行不等同于依赖重新编译；实际是否复用以对象文件和库的时间戳、构建结果核对。
