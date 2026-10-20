# Lua UI 轻量引用统一 SPEC（LUR01）

状态：设计确认中；已确认所有窗口统一增删改清空、采用storeMap并移除排序数组，尚未修改运行代码。日期：2026-10-20。

本方案承接`cg2d_lua_ui_native_proxy_spec.md`。获确认并实施后，以本方案覆盖旧文档中窗口代理分型、窗口引用代次校验、连续元素数组、elementsSorted缓存和控件身份的相关约定；其余行为保持。

## 1. 目标与已确认方向

- 原生窗口、Lua创建窗口及没有Lua绑定的扩展窗口，统一返回同一种窗口引用。
- 用户已确认所有窗口允许修改、新增、删除和清空元素，包括原生固定布局窗口；统一接口，不以是否已有Lua绑定区分能力。
- 元素统一由`storeMap<elementId, Element>`管理；旧代码改为按elementId查找，移除连续实例数组和elementsSorted排序数组。
- Lua创建元素时，elementId为0、nil、未填写或空字符串则使用从10000开始的全局自增ID；其余仅接受正整数并原样使用。C++定义的元素使用enum管理的固定ID。
- Lua窗口引用只保存窗口ID，不检查实例代次；允许旧引用访问同ID重新打开的窗口。
- luaWindow、luaControl只负责引用和参数转换，属性、子元素、回调引用及生命周期仍属于原生UI。
- 引用可以保存在Lua变量中；每次操作临时解析UI对象，不缓存底层对象指针，不由Lua GC销毁UI。
- 本轮先确认SPEC，不执行代码重构、编译或运行。

## 2. 当前源码中的问题

| 当前实现 | 问题及调整方向 |
|---|---|
| `LuaWindowRef`与`LuaNativeWindowRef`分别注册元表 | 合并为同一窗口引用类型和方法入口 |
| `FindWindow`对扩展窗口读取`window.binding` | 无绑定也必须返回引用；查找不能隐式创建Lua绑定 |
| `ResolveWindow`返回`LuaWindow*` | 改为从UI管理器解析`WindowInstance*`，有效性不依赖绑定 |
| `LuaWindow`同时承担回调桥接及AddXXX等操作 | 回调桥接独立为`LuaWindowBinding`；UI操作通过临时解析结果执行 |
| 动态控件用controlId，原生控件用数组下标 | 为所有`WindowElement`引入统一稳定身份`elementId` |
| 原生固定数组与动态组件数组释放方式不同 | 全部迁入原生storeMap，统一增删和资源释放，见第7节 |

## 3. 包装对象与临时视图

建议保留现有Lua可见元表名`LuaWindowRef`、`LuaControlRef`，减少脚本迁移；不再注册独立的原生窗口/原生元素元表。

概念结构如下，最终类型与项目现有整数约定对齐：

```cpp
struct LuaWindowRef {
    int windowId;
};

struct LuaControlRef {
    int windowId;
    std::uint32_t elementId;
};
```

- Lua userdata只保存以上标识，不保存UIWindow指针、WindowElement指针、owner、controls或属性副本。
- C++方法入口解析当前窗口及元素，局部借用结果完成操作。无需再分配一个长期存在的C++包装对象。
- 调用Lua回调可能关闭窗口或删除元素；回调后需要继续访问UI时，必须重新解析，不能沿用回调前的局部指针。
- 重复FindWindow可以产生不同userdata，但都引用同一窗口ID；本轮不新增包装对象缓存或`__eq`协议。
- GC只回收引用。关闭窗口仍通过UI管理器执行，资源、属性、组件及回调引用由原生生命周期释放。

## 4. elementId：固定ID与自动分配（规则已确认）

在`WindowElement`增加非零elementId，创建成功后Lua只读，始终与storeMap的key一致。ID是整数，不能把字符串、布尔或小数隐式转换为ID。

| Lua创建时的elementId | 处理 |
|---|---|
| 未填写、nil、0、空字符串`""` | 从10000开始使用全局自增分配器 |
| 正整数 | 原样使用传入ID，不自动偏移或重编号 |
| 负数、小数、非空字符串、布尔、table等 | 参数错误，不创建元素 |

- 全局自增计数器由原生UI统一管理，跨窗口共享；首次候选值为10000，不因窗口关闭、清空或同ID重开而重置。显式ID创建不消耗自动序号。
- C++定义的元素传入固定ID，由各窗口的具名enum管理；原型、分页元素、自动战斗等不得依赖运行期自动编号来定位其原有元素。
- ID唯一性作用域为单个窗口。同ID可用于不同窗口，完整定位为`{windowId, elementId}`；不限制显式ID必须小于10000。
- 所有原生原型实例化、分页附加元素和动态组件创建，均在进入storeMap前确定ID。原实例数组下标访问全部改为具名enum或明确业务关联的ID查找。
- `elementId`与`seqNo`、业务槽号分别独立，修改属性或排序不改变ID。新增另一个元素须使用该窗口内未占用的ID。
- 删除元素后旧控件引用暂时失效；若同窗口又创建相同ID，旧引用指向新元素。窗口重开并恢复固定enum ID时同样如此，不增加控件代次来阻止此行为。此前“所有元素ID永不复用、旧引用永不重新命中”的建议废止。
- 当前拖动捕获和待派发窗口事件的内部生命周期校验不等同于Lua引用解析，删除元素时仍取消其进行中的交互，不能让旧拖动转移到同ID新元素。
- 建议使用std::uint32_t承载elementId；超出类型范围的输入报错，不截断，自动计数耗尽不回绕。

冲突处理（用户已确认）：显式ID已被当前窗口占用时，新增报错并保留原元素，不覆盖、不自动换号；自动候选ID若在目标窗口已占用，则继续递增查找未占用ID。显式ID不推进全局计数器，不影响其他窗口使用同一固定ID。

## 5. Lua公共接口

### 5.1 查找和读取

```lua
local window = WinMgr.FindWindow(windowId)
if window then
    window.x = 100
    window.y = 120

    local element = window.elements[elementId] -- elementId来自创建结果或已保存的元素引用
    if element then
        local stableId = element.elementId
        element.seqNo = 10
    end
end
```

- `FindWindow`返回当前存在且未标记关闭的窗口引用，否则返回nil；不区分创建来源或Lua绑定状态。
- `NewWindow`保持原返回数量和状态值，返回的窗口对象使用统一引用类型。命中已有扩展窗口时，不再强转或要求其已有Lua绑定。
- 所有窗口回调中的self也使用同一窗口引用。
- `window.elements`使用统一集合包装；集合按窗口ID查询当前元素，旧集合允许跟随同ID重开的窗口。
- `elements[elementId]`改为按ID访问，不再表示第几个元素；找不到返回nil。集合数量由storeMap的有效元素数量决定，不将ID当连续序号。
- 控件回调self、AddXXX返回值和elements访问返回值统一为`LuaControlRef`。
- 元素`id/controlId`统一作为只读elementId别名；旧脚本中把id当下标或业务槽号的用法须同步迁移，不能保留两套含义。
- `window.count`统一为有效元素总数，包含容器子元素。旧动态窗口只计根元素的用法须同步迁移。
- 以上集合及ID语义属于本次兼容变更，实施时检索仓库脚本并更新；仓库外脚本需要按新接口迁移，不能声称未经迁移即可兼容。

### 5.2 属性与方法

- 共用一套属性解析与赋值：位置、尺寸、层级、可见性、elements、回调及扩展属性。
- `Set`与直接属性赋值使用同一实现，避免两种入口产生不同结果。
- `Close`、`CheckKeyState`、`ShowTips`、`DrawRect`等公共操作不再要求Lua绑定；DrawRect按对应原生可见性判断，不能对所有窗口直接套用扩展窗口ACTIVE位。
- 普通属性读取、赋值及string/int扩展属性操作不创建Lua绑定。
- `dragMove`继续使用既有扩展窗口拖动实现；固定窗口不新增第二套拖动协议，设置不支持的属性明确报错。
- 窗口失效时`valid=false`，窗口`id/winId`仍返回引用保存的ID，其余业务属性返回nil；普通操作方法返回false，赋值及AddXXX明确报错，不自动创建窗口。
- 控件对应ID不存在时`valid=false`，`elementId`仍可读取，其余业务属性返回nil；写入和控件操作明确报失效错误。同窗口再次创建该ID后，引用恢复有效并指向新元素。
- 已保存的窗口引用在同ID重开后恢复valid，可访问新窗口；这是用户允许的行为，不增加代次拒绝。

## 6. Lua回调桥接

- 现有`LuaWindow : WindowBinding`改为职责明确的`LuaWindowBinding`，只负责Lua状态、回调派发和引用释放，不代表窗口是否存在。
- `refUpdate/refDraw/refEvent`及元素的全部refXXXX继续存放在WindowInstance/WindowElement。
- 首次设置非空Lua回调时按需建立桥接；未绑定对象设置nil为无操作，不因清空回调创建绑定。
- Lua registry引用只能在所属Lua状态内读写。保留单一回调owner约束，跨状态访问非空回调明确报错，不能把ref整数拿到另一个registry解释。
- 普通UI属性不受“哪个Lua状态先绑定回调”的限制；回调owner仅约束回调操作。
- 普通属性操作不能重置既有ref字段。原生与动态元素的无回调初值在各自创建路径正确初始化。
- 窗口关闭、组件删除和脚本退出沿现有释放链清理引用，保持回调中关闭/删除后的重新解析。
- OnWindowOpen/OnWindowClose继续传窗口ID，订阅和注销接口保持。OnWindowOpen回调中FindWindow必须能取得无Lua绑定的窗口。

## 7. 统一元素存储及结构操作（已确认范围）

### 7.1 原生存储与所有权

- 每个窗口以`std::map<ElementId, WindowElement> storeMap`作为唯一活动元素存储，不保留原生实例数组和动态元素数组两套数据。
- 移除elements指针、elementsSorted及其source/count/tick缓存字段，取消原1秒重建排序数组逻辑。原型描述可以继续用只读表，实例化结果必须进入storeMap。
- 原生UI统一拥有元素、组件及载荷。组件只借用所属map节点，不再把draft和vector中的元素交替作为活动对象。
- C++创建/析构正确构造map及载荷；相应WindowInstance创建不再直接calloc非平凡对象。更新受影响的布局断言、释放路径和自动战斗固定数组析构，不能仅替换字段类型。
- 查询使用find，不使用`operator[]`，避免删除后旧代码读写时自动补回空元素。

### 7.2 统一接口

所有窗口均支持下列接口，不保留固定布局例外：

```lua
local window = WinMgr.FindWindow(windowId)
local element = window:AddText({elementId = 0, text = "示例", x = 8, y = 8}) -- 自动分配
local id = element.elementId
window.elements[id].visible = true
window:RemoveElement(id) -- 建议新增的单元素删除接口
window:ClearChildren()  -- 清空全部元素，窗口本身仍存在
```

- AddXXX保持既有参数及返回形式，增加可选elementId字段并按第4节解析；底层全部进入同一原生新增路径，成功返回统一元素引用。Set及属性赋值不能改elementId。
- 建议`RemoveElement(elementId)`存在则删除并返回true，不存在返回false；不提供按排序位置删除的入口。
- ClearChildren清空全部有效元素，窗口属性及窗口回调保留；空集合重复清空允许成功。
- 删除容器时建议同时删除其子孙元素；父子关系保存elementId，不依赖连续内存。
- 删除元素先使引用、命中、输入焦点及拖动捕获失效，再释放组件、载荷和元素refXXXX；所有释放统一执行且仅执行一次。
- 回调中新增元素进入下一轮遍历；删除立即逻辑生效，物理释放延后到当前回调/遍历安全点，避免当前方法仍借用的组件被析构。
- 清空后普通update/draw不应通过旧查找隐式重建原控件。确需业务创建的路径必须使用显式新增接口，与每帧属性更新分开。

### 7.3 旧业务访问迁移

- 每个窗口的固定控件以具名enum定义固定elementId，创建和访问使用同一枚举值；网格类元素以明确的枚举ID表或业务槽号关联elementId，不保留元素副本或兼容数组。
- 原`elements[n]`、`base + slot`、指针减法求下标及`element - 1`逐项改为枚举ID查找或明确关联，不假定ID连续性代表内存或控件相邻关系。
- 滚动条相邻滑块、输入框与光标等关系明确记录对方elementId，删除任一方后相关操作安全停止，不能查找map的前一项替代。
- 回调参数不再由元素地址减数组首址产生。UI调度传elementId，业务处理需要物品/技能槽号时读取明确的业务关联，不做elementId减常量。
- 元素可能被Lua删除：所有旧更新、绘制和事件代码都需处理find失败；跳过缺失的显示元素，依赖完整控件组的操作在组不完整时停止，不以占位元素掩盖缺失。

### 7.4 移除排序数组后的顺序（建议确认）

用户已确认移除排序数组，未要求取消seqNo。storeMap以elementId为key，直接遍历不能表达seqNo。

建议保留seqNo排序，并在原生UI中使用`std::set<std::pair<SeqNo, ElementId>>`作为顺序索引：

- 集合只存排序键和ID，不存元素副本/指针；元素本体仅在storeMap。
- `(seqNo, elementId)`提供稳定顺序；原生原型初始seqNo保持既有顺序，旧Lua后加入优先的初值规则保持。
- 绘制与鼠标处理统一使用这套顺序，保持当前小seqNo优先规则；父子裁剪继续按明确父关系执行。
- 新增、删除和seqNo变更通过原生接口同步更新集合；迁移所有直接seqNo赋值，不能留下索引与属性不一致的路径。
- 遍历期间变更在安全点发布顺序：本轮新增不提前处理、删除立即跳过、同一元素本轮只处理一次；不再恢复常驻或临时排序数组。
- 删除、清空和关闭同时清理顺序索引，旧每秒刷新机制完全移除。

有序集合属于待确认的实现细节。若要求连顺序索引也不要保留，需要另选无缓存遍历算法或明确取消seqNo；本方案不擅自取消现有排序行为。

## 8. 实施范围与步骤

1. UI侧引入storeMap、elementId、从10000开始的全局自增分配器和固定元素enum，统一增删和安全点回收，覆盖原生、分页、固定扩展窗口和动态组件；正确构造和销毁非平凡对象。
2. 窗口引用改为仅ID，统一解析及元表，移除LuaNativeWindowRef分支，FindWindow不读取binding判断存在性。
3. 控件统一按elementId解析，迁移元素集合、回调self及AddXXX返回路径，移除原生元素下标代理。
4. 拆分LuaWindowBinding，将属性、窗口方法和控件方法改为临时解析后直接操作UI。
5. 迁移全部旧元素下标、指针运算、业务槽位与相邻控件依赖；删除elementsSorted及其维护接口，落实已确认的排序替代方案。
6. 统一Set/直接赋值、RemoveElement/ClearChildren及无效引用处理，迁移仓库内Lua脚本，落实回调owner约束。
7. 更新接口说明与唯一进度，集中检查差异及全部调用点。按当前默认规则不编译、不运行；用户要求时再执行build.bat验证。

主要涉及`LuaUIRefs.hpp`、`LuaUI.hpp/.cpp`、`LuaUIProxy.cpp`、`LuaControlEvents.cpp`、`WindowElement/WindowInstance`定义、`WindowComponents`、`WindowExtension`、全部访问元素的原生窗口、AutoBattle及仓库内Lua脚本。不引入依赖、不改变游戏协议或资源格式。

## 9. 验收项

| 场景 | 预期 |
|---|---|
| 查询原生、Lua及无绑定扩展窗口 | 返回同一元表的窗口引用，查找不创建绑定 |
| 窗口关闭、同ID重开 | 旧窗口引用先失效后指向新窗口，无代次限制 |
| 元素排序及storeMap增删 | 未删除控件引用仍命中同一elementId，无排序数组 |
| 删除元素或清空 | 对应ID不存在时旧控件引用失效，不通过数组位置访问其他ID |
| 同窗口重新创建相同elementId | 旧控件引用重新有效并指向新元素，旧拖动不恢复 |
| Lua传入0/nil/空字符串或不填elementId | 使用跨窗口共享、从10000开始的全局自增分配器 |
| Lua传入正整数及C++ enum固定ID | 原样作为key，属性读取、集合访问和回调返回一致 |
| 传入负数、小数、非空字符串等无效ID | 明确报错，不创建或覆盖任何元素 |
| 显式ID冲突 | 新增报错，原元素及其资源保持，不自动改号 |
| 自动候选ID冲突 | 全局计数继续递增，跳过目标窗口内已占用的ID |
| 通过elements和回调self访问同一元素 | 相同elementId，统一属性/操作入口 |
| 无绑定窗口修改坐标、文本、扩展属性 | 修改原生对象，不创建回调桥接 |
| 设置/替换/清空各类onXXXX | ref挂在UI对象上，所属Lua状态正确，释放一次 |
| 普通操作与回调中关闭/删除/新增 | 回调后重新解析，无悬空指针，无重复释放 |
| 原生背包、分页、滚动条、自动战斗面板 | 业务槽位及明确控件关联正确，载荷释放及渲染/鼠标顺序保持 |
| 所有窗口调用新增、删除、清空 | 使用同一接口成功，缺失元素的旧业务访问安全停止，无隐式重建 |
| 回调中删除自己/父容器、清空、改变seqNo | 无悬空访问、重复派发或失效迭代器，下一轮顺序正确 |
| 修改seqNo并增删元素 | 绘制和鼠标使用相同顺序，顺序索引与storeMap一致 |
| 丢弃所有Lua包装对象并触发GC | 原生窗口和控件继续存在 |
| 仓库内Lua NewWindow/Set/AddXXX及Open订阅脚本 | 返回形式保持，集合/ID/count用法完成迁移；仓库外迁移差异明确列出 |

本文件为设计稿，验收表尚未执行。所有窗口统一增删改清空、storeMap、移除排序数组、Lua特殊值自动分配/正整数原样使用、C++ enum固定ID及ID冲突处理已确认。有序集合替代排序数组及删除容器的级联规则为本稿建议，待确认后实施；尚未修改运行代码。
