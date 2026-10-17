# 编译警告清理与兼容边界

日期：2026-10-17。范围：本子项目Debug客户端，用户指定build.bat；不运行客户端或测试。

## 已执行的等价清理

- 关闭日志时，宏改用sizeof中的printf／wprintf调用检查表达式类型。实参不求值，不输出日志；消除空受控语句和仅供日志使用的参数／变量警告。
- 对被注释的诊断日志参数、保留的内部诊断函数及四个原端辅助入口标注maybe_unused；不移除原地址及实现。删除纯计算且无消费者的局部变量。
- 地图ID比较显式保留原有无符号转换；Lua整数、窗口坐标与绘制尺寸显式保留既有窄化。动画显示优先级保留原字节位模式。窗口数据指针按控件用途命名，渲染偏移参数不遮蔽成员。
- 仅给能从现有控制流证明读取前必定赋值的局部变量补初值：互补取样分支、特效创建／播放标志、技能81附加值、自身反射目标、窗口坐标／名称及商店详情取样。初值不进入实际消费者；不移动全局取样、RNG或回调调用。
- GetVersionExA保留原4041E0依赖的版本兼容行为，inet_addr／gethostbyname保留原IPv4解析与查询合同；仅在三处调用局部抑制C4996并写明理由，不全局关闭任何警告。

## 不可直接补零的剩余项

下列代码存在没有赋值的分支，或已有原端兼容注释要求保留跨目标旧值。直接补零、返回、增加默认分支或改为缓存先前全局值，会改变现有行为；本轮不将这些更改伪称为等价清理。

| 文件 | 变量／路径 | 当前问题 |
| --- | --- | --- |
| Yobi/battle_skill_status.cpp | statusType、两处lastAmount | 技能switch默认保留跨目标值；目标循环可在金额解析前结束，尾部仍可能读取。 |
| Yobi/battle_skill_special.cpp | statusType、selfFlags | 原432A3E注释明确保留未初始化入口；缺少自身目标记录时尾部仍使用selfFlags。 |
| Yobi/battle_message.cpp | amount | 施法者循环可在首笔金额解析前结束，后续反馈读取没有缺省定义。 |
| Yobi/battle_effect.cpp | result | 窗口宽高使绘制循环不执行时，末次绘制返回值没有定义。 |
| Yobi/map_observation.cpp | 两组relativeX／relativeY | 只对阶段1／2或3／4赋值；异常阶段不具备明确坐标策略。 |
| ui/windows/WindowBank.cpp | maximum、result | 原注释明确要求未识别kind／side／元素索引时不补默认值。 |
| ui/windows/WindowContacts.cpp | record、privileged、maximum、position、count | 类别／模式异常，或两次全局类别取样不一致时，缺少明确借用对象、权限和数量策略。 |
| ui/windows/WindowTrade.cpp | ownPet | 原4B0970注释明确指出异常分类读未初始化字节，不补零或指定分支。 |
| ui/windows/WindowSkill.cpp | position | 原注释明确只在模式5／6赋值，其他模式不专项模拟。 |
| ui/windows/WindowShop.cpp | decrement、message、increment | 数量只在hover等指定路径生产；原注释要求不补异常事件组合的栈残留默认值。 |

本线AGENTS.md“还原标准与异常”要求“异常不专项处理”“不因此擅加拒绝、补零、钳制、保护或输入限制”。上述项保留诊断，不加C4701／C4703禁用项；若后续需要消除，须先明确异常行为SPEC及其兼容范围，不能仅以零警告为理由改业务。

## 验证

首轮build.bat Debug构建成功，收集115条警告。第二轮修改文件增量构建成功，仅输出7条C4701；其余未重编文件仍有已知警告，该数字不代表全项目剩余总数。

完整重编本项目src下源码并成功链接，输出30条警告；发现首轮未重编文件中的5项（资源辅助函数2、旧Winsock API 2、网卡类型条件赋值1）。修正后再次执行build.bat Debug，退出0、本轮0条警告；结合完整重编清单，实际剩余25条C4701，分布在上述10个文件，不能把末轮增量0条当作全项目零警告。合并基线120项，本轮处理95项，其中3项为已说明的局部API兼容抑制。

第三方303个对象／静态库文件的路径、大小和修改时间全部保持；未清理依赖缓存、计算摘要、运行测试或客户端、保存原始构建日志。用户已选择先制定剩余异常分支处理SPEC，确认后继续；本轮保留这25项，不执行异常行为修改。

## 后续：版本查询现代化（2026-10-17）

按用户随后要求，将GetVersionExA替换为系统ntdll.dll的RtlGetVersion，移除该处C4996抑制。微软[接口文档](https://learn.microsoft.com/en-us/windows/win32/devnotes/rtlgetversion)列出其Ntdll入口及版本结构合同。版本信息改为函数内OSVERSIONINFOW，保留平台ID／主次版本条件与状态低字节写入；接口不可用或失败时仍按零字段判断。此处仅消费这三个字段，不保留已无消费者的全局A结构及其布局断言。

这是用户要求的版本来源替换，不保证沿用GetVersionExA的manifest／应用兼容版本身份；正常受支持Windows NT环境仍不命中旧Win9x条件。未改为最低版本检查，也不增加最低系统版本门槛。Debug增量构建通过、无新增警告；当前只剩两处Winsock旧接口的局部C4996兼容抑制，25项异常分支方案仍待确认。
