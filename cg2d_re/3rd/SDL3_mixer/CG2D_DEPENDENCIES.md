# 本地依赖来源（LBG01）

- SDL3_mixer：官方 release-3.2.4 发布源码包 `SDL3_mixer-3.2.4.zip`，未导入 VisualC 预编译 DLL；许可见 LICENSE.txt。
- libogg：该发布标签的 external/ogg 子模块快照 `936fdd822a4e4fc963197b3eda931b89b52859a0`，来源 https://github.com/libsdl-org/ogg ，版本1.3.5，许可见 external/ogg/COPYING。
- libvorbis：同标签 external/vorbis 子模块快照 `eefe62244630a2d2c37ba5e6fb1a1fd8ad93ece5`，来源 https://github.com/libsdl-org/vorbis ，版本1.3.7，许可见 external/vorbis/COPYING。
- MP3：使用发布包内 src/dr_libs/dr_mp3.h，许可随该文件保留。

只构建WAV、dr_mp3、libvorbisfile三条解码路径。上游其余源码为发布包原内容，未启用对应功能，未下载其他external依赖。外部库源码保持原样；配置在父项目CMakeLists.txt中限定，不在构建期间联网。
