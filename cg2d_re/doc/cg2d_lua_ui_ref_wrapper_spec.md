# Lua UI 轻量引用统一 SPEC（LUR01）

状态：已实施，最终Debug编译及链接通过；未运行客户端，交互验收待进行。日期：2026-10-20。

本方案承接`cg2d_lua_ui_native_proxy_spec.md`。获确认并实施后，以本方案覆盖旧文档中窗口代理分型、窗口引用代次校验、连续元素数组、elementsSorted缓存和控件身份的相关约定；其余行为保持。

## 1. 目标与已确认方向

- 原生窗口、Lua创建窗口及没有Lua绑定的扩展窗口，统一返回同一种窗口引用。
- 用户已确认所有窗口允许修改、新增、删除和清空元素，包括原生固定布局窗口；统一接口，不以是否已有Lua绑定区分能力。
- 元素统一由封装容器`SortedElementMap sortedMap`管理；旧代码改为按elementId查找，移除连续实例数组和elementsSorted排序数组。
- Lua创建元素时，elementId为0、nil、未填写或空字符串则使用从10000开始的全局自增ID；其余仅接受正整数并原样使用。C++定义的元素使用enum管理的固定ID。
- UIWindow提供按elementId读写已有元素的索引器；原有C++逻辑查找不到元素时跳过依赖它的逻辑，不自动创建元素。
- 用户确认按seqNo排序，绘制和鼠标处理使用同一顺序；elementId负责身份查找，不替代seqNo作为排序主键。
- Lua窗口引用只保存窗口ID，不检查实例代次；允许旧引用访问同ID重新打开的窗口。
- luaWindow、luaControl只负责引用和参数转换，属性、子元素、回调引用及生命周期仍属于原生UI。
- 引用可以保存在Lua变量中；每次操作临时解析UI对象，不缓存底层对象指针，不由Lua GC销毁UI。
- 用户已明确要求实施本SPEC，完成修改后执行build.bat验证；不运行客户端。

## 2. 迁移前问题与本次调整

| 迁移前实现 | 本次调整 |
|---|---|
| `LuaWindowRef`与`LuaNativeWindowRef`分别注册元表 | 合并为同一窗口引用类型和方法入口 |
| `FindWindow`对扩展窗口读取`window.binding` | 无绑定也必须返回引用；查找不能隐式创建Lua绑定 |
| `ResolveWindow`返回`LuaWindow*` | 改为从UI管理器解析`WindowInstance*`，有效性不依赖绑定 |
| `LuaWindow`同时承担回调桥接及AddXXX等操作 | 回调桥接独立为`LuaWindowBinding`；UI操作通过临时解析结果执行 |
| 动态控件用controlId，原生控件用数组下标 | 为所有`WindowElement`引入统一稳定身份`elementId` |
| 原生固定数组与动态组件数组释放方式不同 | 全部迁入原生sortedMap，统一增删和资源释放，见第7节 |

## 3. 包装对象与临时视图

保留现有Lua可见元表名`LuaWindowRef`、`LuaControlRef`，减少脚本迁移；不再注册独立的原生窗口/原生元素元表。

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

在`WindowElement`增加非零elementId，创建成功后Lua只读，始终与sortedMap的key一致。ID是整数，不能把字符串、布尔或小数隐式转换为ID。

| Lua创建时的elementId | 处理 |
|---|---|
| 未填写、nil、0、空字符串`""` | 从10000开始使用全局自增分配器 |
| 正整数 | 原样使用传入ID，不自动偏移或重编号 |
| 负数、小数、非空字符串、布尔、table等 | 参数错误，不创建元素 |

- 全局自增计数器由原生UI统一管理，跨窗口共享；首次候选值为10000，不因窗口关闭、清空或同ID重开而重置。显式ID创建不消耗自动序号。
- C++定义的元素传入固定ID，由各窗口的具名enum管理；原型、分页元素、自动战斗等不得依赖运行期自动编号来定位其原有元素。
- ID唯一性作用域为单个窗口。同ID可用于不同窗口，完整定位为`{windowId, elementId}`；不限制显式ID必须小于10000。
- 所有原生原型实例化、分页附加元素和动态组件创建，均在进入sortedMap前确定ID。原实例数组下标访问全部改为具名enum或明确业务关联的ID查找。
- `elementId`与`seqNo`、业务槽号分别独立，修改属性或排序不改变ID。新增另一个元素须使用该窗口内未占用的ID。
- 删除元素后旧控件引用暂时失效；若同窗口又创建相同ID，旧引用指向新元素。窗口重开并恢复固定enum ID时同样如此，不增加控件代次来阻止此行为。此前“所有元素ID永不复用、旧引用永不重新命中”的建议废止。
- 当前拖动捕获和待派发窗口事件的内部生命周期校验不等同于Lua引用解析，删除元素时仍取消其进行中的交互，不能让旧拖动转移到同ID新元素。
- 使用std::uint32_t承载elementId；超出类型范围的输入报错，不截断，自动计数耗尽不回绕。

冲突处理（用户已确认）：显式ID已被当前窗口占用时，新增报错并保留原元素，不覆盖、不自动换号；自动候选ID若在目标窗口已占用，则继续递增查找未占用ID。显式ID不推进全局计数器，不影响其他窗口使用同一固定ID。

## 5. Lua公共接口

### 5.1 查找和读取

```lua
local window = WinMgr.FindWindow(windowId)
if window then
    window.x = 100
    window.y = 120

    local element = window:FindElementById(elementId) -- elementId来自创建结果或已保存的元素引用
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
- `elements[elementId]`改为按ID访问，不再表示第几个元素；找不到返回nil。集合数量由sortedMap的有效元素数量决定，不将ID当连续序号。
- `window:FindElementById(elementId)`直接执行一次原生ID查询，返回与elements索引相同类型的LuaControlRef；窗口已关闭、元素不存在或已删除时返回nil，不创建元素或Lua回调绑定。沿用统一ID参数解析：0/nil/空字符串查询无效ID并返回nil，不触发自动分配；负数、小数、非空字符串及越界ID报错。
- 控件回调self、AddXXX返回值和elements访问返回值统一为`LuaControlRef`。
- 元素`id/controlId`统一作为只读elementId别名；旧脚本中把id当下标或业务槽号的用法须同步迁移，不能保留两套含义。
- `window.count`统一为有效元素总数，包含容器子元素。旧动态窗口只计根元素的用法须同步迁移。
- 以上集合及ID语义属于本次兼容变更，实施时检索仓库脚本并更新；仓库外脚本需要按新接口迁移，不能声称未经迁移即可兼容。

### 5.2 属性与方法

- 共用一套属性解析与赋值：位置、尺寸、层级、可见性、elements、回调及扩展属性。
- `Set`与直接属性赋值使用同一实现，避免两种入口产生不同结果。
- 2026-10-22按用户要求增加窗口`category`属性：创建参数、`window.category`读取/赋值及`window:Set{category = 2}`均直接映射WindowInstance::category，不写入扩展属性表。沿原S8类别字段承接整数；原生窗口初值只在创建时取kWindowCategories，扩展窗口未指定时为0。赋值本身不触发关闭，后续已有类别策略读取当前值；扩展窗口仍不作为默认窗口保留。本项完成静态接线核对，未编译或运行。
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

- 每个窗口以`SortedElementMap sortedMap`作为唯一活动元素容器，不保留原生实例数组和动态元素数组两套数据。内部有序节点保存元素，ID哈希索引仅借用节点地址，实现平均O(1)的按ID访问；不是第二份元素存储。
- 移除elements指针、elementsSorted及其source/count/tick缓存字段，取消原1秒重建排序数组逻辑。原型描述可以继续用只读表，实例化结果必须进入sortedMap。
- 原生UI统一拥有元素、组件及载荷。组件只借用所属map节点，不再把draft和vector中的元素交替作为活动对象。
- C++创建/析构正确构造map及载荷；相应WindowInstance创建不再直接calloc非平凡对象。更新受影响的布局断言、释放路径和自动战斗固定数组析构，不能仅替换字段类型。
- 业务通过UIWindow的`operator[]`访问已有元素；索引器内部使用容器的ID索引查询，禁止调用会插入元素的std::map::operator[]，避免查询意外插入空元素。

### 7.1.1 UIWindow读写索引器（用户已要求）

UIWindow按elementId返回已有UIElement。为表达“查找不到则跳过”，接口采用可空指针；下列名称对应项目现有窗口/元素类型，不另外建立一份UI对象：

```cpp
UIElement* operator[](ElementId elementId) noexcept;
const UIElement* operator[](ElementId elementId) const noexcept;
```

使用示例（枚举名称为示意，实施时使用对应窗口的实际enum）：

```cpp
if (auto* element = window[static_cast<ElementId>(InventoryElementId::Title)]) {
    element->visible = 1;
}
```

- 非const窗口返回可读写指针；const窗口返回只读指针。返回的是sortedMap中的实际元素，不是副本。
- SortedElementMap和窗口均提供const/非const的`FindElementById(ElementId)`，支持固定枚举ID；operator[]转发到该入口。一次调用只执行一次ID哈希查询，调用方保存返回指针用于判空和后续访问，避免先判空再重复索引。跨回调或结构变更后仍须重新查询。
- 2026-10-20用户补充：原生窗口函数入口获取并检查一次gCurrentWindow，函数内复用当前窗口指针；每个元素仍单独查询、判空。元素缺失只跳过该元素操作，不以return或跨元素的continue阻断后续独立元素。只服务当前元素的回调可以返回，统一调度继续后续元素；依赖锚点的定位操作缺少锚点时只跳过该定位。
- 元素不存在或已逻辑删除时返回nullptr，不抛出查找异常，不返回虚构的空元素，也不自动新增。
- 0仅在新增参数解析时表示自动分配；索引器查询0返回nullptr，不触发分配。
- 缺失单个元素时跳过该元素相关操作；多个元素共同完成一个操作时，先查询所需元素，缺任一个就跳过该操作。与它无关的其他更新继续执行。
- 读取后可以修改元素支持的属性；elementId本身仍只在创建时确定，不能通过指针改写ID或整体覆盖有资源归属的元素。seqNo变更仍走顺序同步接口。
- 不长期保存返回的指针；跨Lua回调、元素删除等可能改变生命周期的操作后重新按ID查询。窗口指针自身是否有效仍由调用方按原生命周期检查。

### 7.2 统一接口

所有窗口均支持下列接口，不保留固定布局例外：

```lua
local window = WinMgr.FindWindow(windowId)
local element = window:AddText({elementId = 0, text = "示例", x = 8, y = 8}) -- 自动分配
local id = element.elementId
window.elements[id].visible = true
window:RemoveElement(id) -- 按ID删除元素
window:ClearChildren()  -- 清空全部元素，窗口本身仍存在
```

- AddXXX保持既有参数及返回形式，增加可选elementId字段并按第4节解析；底层全部进入同一原生新增路径，成功返回统一元素引用。Set及属性赋值不能改elementId。
- `RemoveElement(elementId)`存在则删除并返回true，不存在返回false；不提供按排序位置删除的入口。
- ClearChildren清空全部有效元素，窗口属性及窗口回调保留；空集合重复清空允许成功。
- 删除容器时同时删除其子孙元素；父子关系保存elementId，不依赖连续内存。
- 删除元素先使引用、命中、输入焦点及拖动捕获失效，再释放组件、载荷和元素refXXXX；所有释放统一执行且仅执行一次。
- 回调中新增元素进入下一轮遍历；删除立即逻辑生效，物理释放延后到当前回调/遍历安全点，避免当前方法仍借用的组件被析构。
- 清空后普通update/draw不应通过旧查找隐式重建原控件。确需业务创建的路径必须使用显式新增接口，与每帧属性更新分开。

### 7.3 旧业务访问迁移

- 每个窗口的固定控件以具名enum定义固定elementId，创建和访问使用同一枚举值；网格类元素以明确的枚举ID表或业务槽号关联elementId，不保留元素副本或兼容数组。
- 原`elements[n]`、`base + slot`、指针减法求下标及`element - 1`逐项改为枚举ID查找或明确关联，不假定ID连续性代表内存或控件相邻关系。
- 滚动条相邻滑块、输入框与光标等关系明确记录对方elementId，删除任一方后相关操作安全停止，不能查找map的前一项替代。
- 回调参数不再由元素地址减数组首址产生。UI调度传elementId，业务处理需要物品/技能槽号时读取明确的业务关联，不做elementId减常量。
- 元素可能被Lua删除：所有旧更新、绘制和事件代码使用窗口索引器并判空；查找不到时跳过依赖该元素的逻辑。依赖完整控件组的操作先检查全部关联元素，不以占位元素掩盖缺失，不在检查前完成部分依赖操作。

### 7.4 单一sortedMap、seqNo排序与notifyUpdate（用户已确认）

- 窗口只暴露一个sortedMap元素容器，元素按(seqNo, elementId)有序存储；元素只保存一份。内部ID哈希索引仅用于高效定位节点，不复制载荷或元素。
- 绘制与鼠标处理按同一seqNo主序，小值优先，相同seqNo按elementId稳定排序。ID用于查找，不替代seqNo优先级。
- sortedMap提供`notifyUpdate()`。C++直接修改seqNo后显式调用；Lua属性入口也调用该接口同步排序。节点重新挂接保持元素地址及ID查询有效。
- 用户明确：正在遍历元素时调用notifyUpdate，必须写m_log并抛出异常，不延迟或静默接受。Lua排序属性写入失败时恢复原seqNo。
- 遍历以排序键游标继续，不保存可能被回调删除的map迭代器；新增元素本轮不处理，删除立即跳过，资源在安全点释放。
- 删除排序数组及每秒重排机制；不得恢复第二份元素数组或单独的元素排序副本。

## 8. 实施范围与步骤

1. UI侧引入sortedMap、elementId、从10000开始的全局自增分配器和固定元素enum，统一增删和安全点回收，覆盖原生、分页、固定扩展窗口和动态组件；正确构造和销毁非平凡对象。
2. 窗口引用改为仅ID，统一解析及元表，移除LuaNativeWindowRef分支，FindWindow不读取binding判断存在性。
3. 控件统一按elementId解析，迁移元素集合、回调self及AddXXX返回路径，移除原生元素下标代理。
4. 拆分LuaWindowBinding，将属性、窗口方法和控件方法改为临时解析后直接操作UI。
5. 实现窗口const/非const索引器，迁移全部旧元素下标、指针运算、业务槽位与相邻控件依赖，逐项检查缺失元素时跳过相关逻辑；删除elementsSorted及其维护接口，接入sortedMap.notifyUpdate。
6. 统一Set/直接赋值、RemoveElement/ClearChildren及无效引用处理，迁移仓库内Lua脚本，落实回调owner约束。
7. 更新接口说明与唯一进度，集中检查差异及全部调用点。按用户本次授权执行build.bat完成编译及链接验证，不运行客户端。

主要涉及`LuaUIRefs.hpp`、`LuaUI.hpp/.cpp`、`LuaUIProxy.cpp`、`LuaControlEvents.cpp`、`WindowElement/WindowInstance`定义、`WindowComponents`、`WindowExtension`、全部访问元素的原生窗口、AutoBattle及仓库内Lua脚本。不引入依赖、不改变游戏协议或资源格式。

## 9. 验收项

| 场景 | 预期 |
|---|---|
| 查询原生、Lua及无绑定扩展窗口 | 返回同一元表的窗口引用，查找不创建绑定 |
| 窗口关闭、同ID重开 | 旧窗口引用先失效后指向新窗口，无代次限制 |
| 元素排序及sortedMap增删 | 未删除控件引用仍命中同一elementId，无排序数组 |
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
| C++索引器查询存在/不存在/已逻辑删除的ID | 分别返回实际元素指针/nullptr/nullptr，查询不改变sortedMap大小 |
| const与非const窗口索引访问 | const只读，非const可修改已有元素；缺失时只跳过依赖逻辑 |
| 回调中删除自己/父容器、清空 | 删除立即逻辑生效，安全点回收，无悬空访问或失效迭代器 |
| 遍历回调中修改seqNo | 写m_log并抛异常，Lua属性入口恢复原seqNo |
| 修改seqNo并增删元素 | 绘制和鼠标使用相同顺序，ID索引始终指向正确节点 |
| 遍历中调用notifyUpdate | 写m_log并抛异常，拒绝当次排序更新 |
| 丢弃所有Lua包装对象并触发GC | 原生窗口和控件继续存在 |
| 仓库内Lua NewWindow/Set/AddXXX及Open订阅脚本 | 返回形式保持，集合/ID/count用法完成迁移；仓库外迁移差异明确列出 |

本SPEC已按用户授权实施。最终执行build.bat Debug，编译及链接通过，退出码0；git diff --check通过。仓库中未找到需要迁移的应用Lua脚本，仓库外脚本须按第5节调整集合索引、ID及count语义。未运行客户端，上表交互场景尚未进行运行验收；最终结果同步唯一进度。
