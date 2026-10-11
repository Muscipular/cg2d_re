@echo off
setlocal EnableExtensions DisableDelayedExpansion
for /f "tokens=2 delims=:" %%C in ('chcp') do set "CG_BUILD_OLD_CP=%%C"
chcp 65001 >nul

rem 无参数时构建 Debug 并保留窗口；指定 Debug 或 Release 时不暂停。
set "CG_BUILD_EXIT_CODE=1"
set "CG_BUILD_PAUSE=0"
set "CG_BUILD_ENTERED=0"
set "CG_BUILD_PRESET=windows-msvc-x64-debug"
set "CG_BUILD_CONFIG=Debug"
if "%~1"=="" set "CG_BUILD_PAUSE=1"
if /i "%~1"=="/?" goto :help
if /i "%~1"=="-h" goto :help
if not "%~2"=="" goto :invalid_arguments
if /i "%~1"=="Release" goto :release
if /i "%~1"=="Debug" goto :configure
if "%~1"=="" goto :configure
goto :invalid_arguments

:release
set "CG_BUILD_PRESET=windows-msvc-x64-release"
set "CG_BUILD_CONFIG=Release"

:configure
pushd "%~dp0"
if errorlevel 1 goto :directory_failed
set "CG_BUILD_ENTERED=1"

rem 优先使用 VS 2026 配套 CMake，避免 PATH 中旧版本不识别现有生成器。
set "CG_BUILD_CMAKE="
set "CG_BUILD_VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%CG_BUILD_VSWHERE%" (
    for /f "usebackq delims=" %%C in (`call "%CG_BUILD_VSWHERE%" -latest -prerelease -products * -version "[18.0,19.0)" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find "Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"`) do set "CG_BUILD_CMAKE=%%C"
)
if not defined CG_BUILD_CMAKE (
    for /f "delims=" %%C in ('where cmake.exe 2^>nul') do if not defined CG_BUILD_CMAKE set "CG_BUILD_CMAKE=%%C"
)
if not defined CG_BUILD_CMAKE goto :cmake_missing

rem Visual Studio 生成器自行发现 MSVC，无需预先进入开发者命令提示符。
echo [1/2] 配置 MSVC x64...
"%CG_BUILD_CMAKE%" --preset windows-msvc-x64
set "CG_BUILD_EXIT_CODE=%errorlevel%"
if not "%CG_BUILD_EXIT_CODE%"=="0" goto :configure_failed

echo [2/2] 构建 %CG_BUILD_CONFIG% 客户端...
"%CG_BUILD_CMAKE%" --build --preset "%CG_BUILD_PRESET%" --target cg2d_re_client --parallel
set "CG_BUILD_EXIT_CODE=%errorlevel%"
if not "%CG_BUILD_EXIT_CODE%"=="0" goto :build_failed

echo 构建成功："%~dp0build\windows-msvc-x64\%CG_BUILD_CONFIG%\cg2d_re_client.exe"
goto :finish

:directory_failed
echo [错误] 无法进入脚本所在目录。
goto :finish

:cmake_missing
echo [错误] 找不到 CMake。请安装 VS 2026 的桌面 C++ 和 CMake 工具，或将兼容版本的 CMake 加入 PATH。
goto :finish

:configure_failed
echo [错误] CMake 配置失败，退出码：%CG_BUILD_EXIT_CODE%。
goto :finish

:build_failed
echo [错误] 客户端构建失败，退出码：%CG_BUILD_EXIT_CODE%。
goto :finish

:invalid_arguments
set "CG_BUILD_EXIT_CODE=2"
echo [错误] 仅接受 Debug 或 Release。
goto :usage

:help
set "CG_BUILD_EXIT_CODE=0"
:usage
echo 用法：build.bat [Debug^|Release]
echo 无参数默认 Debug，构建后暂停；指定配置时不暂停。

:finish
if "%CG_BUILD_ENTERED%"=="1" popd
if "%CG_BUILD_PAUSE%"=="1" pause
if defined CG_BUILD_OLD_CP chcp %CG_BUILD_OLD_CP% >nul
endlocal & exit /b %CG_BUILD_EXIT_CODE%
