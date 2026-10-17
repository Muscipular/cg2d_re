# 点击“否”退出崩溃分析与修正 SPEC

日期：2026-10-17。编号：CUN02。范围：CUN01拒绝退出及其他主动退出点核对。用户在要求检查其他类似位置后，明确选择“直接exit吧”。

## 转储证据

只读分析用户提供的cg2d_crash_20261017_190132_089_16760_23092_1.dmp；使用同目录cg2d_re.exe／cg2d_re.pdb，调试引擎确认加载匹配的私有PDB，映像时间为2026-10-17 19:01:16。未启动客户端、下载依赖或计算文件摘要。

异常为0xC0000005，RtlFreeHeap路径读取无效地址。调用栈从ProcessScene_460FF0进入CRT common_exit／onexit，再进入gFontGlyphCache的静态析构，经FontGlyphCache::Atlas、TextureSurfaceBinding、TextureSurfaceOwner::Release、TextureSurface析构，最终在释放像素数组时崩溃。人物页现场choice=2、mainState=22，确认是点击“否”的路径。

纹理现场为512×512，引用数在释放中降为0，前后链指针仍非空；全局纹理owner的head_已为nullptr。结合源码中TextureSurfaceOwner析构直接清空整条资源链，而字体缓存的静态析构再次释放其绑定，可判定该直接退出路径存在跨翻译单元析构寿命冲突。转储不是完整分配追踪，不外推为所有堆损坏原因都已排除。

根本问题是该UI分支直接std::exit，绕过了MainLoop中的ShutdownClient_457A40。正常清理会在全局owner仍存活时执行ReleaseFont_456530，重置字形缓存及绑定，然后才返回WinMain进入CRT退出。

## 用户选择与修正

用户明确要求直接退出，最终不采用主循环请求退出方案。std::exit仍会执行CRT静态析构，不能避免当前路径，因此使用std::_Exit：

1. “否”在ProcessUsageNotice标死模态动作、清空引用后调用std::_Exit(std::rand())，保留现有退出码来源，不进入角色登录或CRT静态析构。
2. 核对时用户已同步将ApplyServerAddressPolicy的校验拒绝改为std::_Exit(std::random_device()())；保留该工作区修改及其比较常量，不夹带本次提交。
3. 不新增退出请求状态、主循环消息或调用者返回协议，不更改“是”及提示显示条件。
4. 已查全部src／include：其他两处PostQuitMessage属于现有正常消息退出；SdlPresenter析构中的std::terminate用于拒绝错误线程销毁，不属于本次正常业务std::exit入口，保持。

具名代价：直接退出不执行客户端ShutdownClient、静态对象析构、atexit回调或CRT流刷新，不能保证退出设置及缓冲日志保存。此修改避开已证的退出析构冲突，不宣称修复全局纹理owner与字体缓存的寿命设计；不扩大为资源寿命重构。

## 验证边界

只读转储栈与源代码调用链交叉核对；静态检查当前源码两处业务主动退出均使用std::_Exit，拒绝后无继续登录路径，无新增状态或无关改动。默认不编译、不运行或新增测试，修正后的实际点击退出仍待运行复测。
