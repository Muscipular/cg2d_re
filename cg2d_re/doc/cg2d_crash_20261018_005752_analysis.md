# 2026-10-18 00:57:52 选角画面加壳崩溃分析

## 结论

用户确认同一编译产物未加壳正常、Themida处理后崩溃。转储证据高度指向ProcessScene_460FF0整段VM_HARD保护中的switch跳表进入了被虚拟化改写的原代码区域。后续按用户选择改为if分发、仅对该函数临时关闭优化并统一普通返回出口；开启TMD的Release已编译链接并核对指令，保护后画面仍待重新加壳复测。

## 转储与指令证据

- 文件：cg2d_crash_20261018_005752_092_17648_17568_1.dmp，异常线程17568。
- 异常0xC0000005，RIP为0x1401A2979（模块偏移0x1A2979）；异常参数为读及0xFFFFFFFFFFFFFFFF。该参数不直接证明业务代码解引用了-1指针。
- 游戏目录EXE与编译目录原始EXE时间戳均为0x6AD3A816，大小及该区域内容不同；MAP时间戳一致，函数起点0x1401A2920对应ProcessScene_460FF0。无匹配PDB可用，采用MAP、原始指令和转储内存交叉核对。
- 原始EXE在0x1401A2962查switch跳表，0x1401A296D执行jmp rcx；case 0入口为0x1401A296F。转储中的RCX正为0x1401A296F，当前场景子状态所在地址0x14108E608的值为0。
- 转储捕获了[0x1401A28F9,0x1401A29F9)的256字节；原函数序言保留，0x1401A2943已改为跳往保护代码。故障地址0x1401A2979的实际字节为CB，反汇编是retf；周围已非原case 0指令。原始EXE的同一地址位于一条mov指令中间，不能套用原始反汇编来解释保护后的故障。
- 栈上0x14E948保存返回地址0x1401CDFC9；原始EXE该位置之前的call明确指向ProcessScene_460FF0。后续候选返回地址0x14007B90A、0x1401D1CE7分别落在MainLoop及WinMain。此为栈内容和原始调用点核对，不冒充保护后完整自动展开结果。

本次首次批量读取故障附近时请求范围超过转储保存的256字节，曾误报未捕获代码；缩小到实际覆盖范围后已取得并反汇编，以上以转储中的字节为准。

## 源码与官方资料

CharacterSelectScene.cpp的ProcessScene_460FF0从函数开头VM_HARD_START保护至末尾VM_HARD_END，内部包括mainState、actionIndex和pukVersion三处switch；另有提前return。当前证据定位的是第一个switch的case 0路径，不能将其他路径自动计为同一已证故障。

Themida官方FAQ明确说明部分编译器的switch跳表会跳入被虚拟化改写的原区域并异常，给出的源码处理方式是把VM标记放进各case内部：[官方说明](https://www.oreans.com/help/tm/hm_faq_can-vm-macros-protect-switch-s.htm)。官方更新记录另有OPTION_MACROS_SWITCH_CASE_SUPPORT，不能笼统宣称所有版本均不支持switch；当前用户的Themida版本和保护项目选项尚未核实：[版本记录](https://www.oreans.com/ThemidaAllWhatsNew.php)。

建议的源码调整：三处switch分发留在VM区域外，仅对需要保护的分支内部设置成对标记；同时核对提前返回与保护区域边界。若希望保持整函数虚拟化，则需结合实际Themida版本检查switch支持选项，并重新加壳验证。此次仅诊断，未擅自删除保护或降低保护级别。

未启动客户端、调试运行、编译、执行算法样例或计算摘要；仅静态解析转储、COFF/PE及MAP，并查询厂商资料。业务源码及用户已有编辑保持。

## 后续修正与验证（2026-10-18）

用户选择将switch改为if，而非拆分VM保护范围；随后明确只在ProcessScene_460FF0函数前后加入临时pragma，不增加CMake开关。三处分发已改为if／else if，保留mainState取样、状态9的分支优先级、合并case值与原调用次序。单次执行块将两处普通提前返回改为保存结果后退出，在VM_HARD_END之后统一return；用户原有直接结束进程的_Exit路径保持。

使用函数外的`#pragma optimize("", off)`与`#pragma optimize("", on)`限制优化控制范围，函数结束后恢复工程命令行优化设置，依据[MSVC官方说明](https://learn.microsoft.com/en-us/cpp/preprocessor/optimize?view=msvc-170)。保留整段VM_HARD与原0x00460FF0地址注释，不修改其他函数的保护或优化级别。

静态核对目标函数外文本未改变、既有函数调用文本序列一致、无switch且普通return只余一个。用户已授权构建验证：已有TMD=ON／TAOMOLI=ON的Ninja Release构建编译链接成功，复用原第三方缓存；按新MAP及PE异常目录核对完整函数范围[0x1401A29A0,0x1401A382E)，共3726字节，无寄存器／内存间接jmp。VM开始与结束调用分别为0x1401A29BC、0x1401A380A，两个提前完成分支均跳到结束调用，唯一ret位于0x1401A382D。

build.bat Release也已配置、编译、链接成功，输出build/windows-msvc-x64/Release/cg2d_re.exe；该缓存TAOMOLI／TMD均为OFF。额外验证沿既有TAOMOLI／TMD均为ON的build/windows-msvc-x64-release目录构建，未改缓存开关。常规Release中有其他模块的C4101、C4189及C4701警告，本次选角函数无新增编译警告，未顺带更改这些异常分支。

上述指令结果验证的是当前编译产物，不代替Themida处理后的运行验收。未执行加壳、替换游戏目录EXE或运行客户端。本次修正及诊断记录按用户要求提交，保留工作区已有的Debug提示开关等其他编辑。
