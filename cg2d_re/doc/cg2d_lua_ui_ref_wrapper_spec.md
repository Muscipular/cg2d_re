# Lua UI 轻量引用统一 SPEC（LUR01）

状态：待用户确认，尚未修改运行代码。日期：2026-10-20。

本方案承接`cg2d_lua_ui_native_proxy_spec.md`。获确认并实施后，以本方案覆盖旧文档中窗口代理分型、窗口引用代次校验和控件身份的相关约定；其余行为保持。

## 1. 目标与已确认方向

- 原生窗口、Lua创建窗口及没有Lua绑定的扩展窗口，统一返回同一种窗口引用。
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
| 原生固定数组与动态组件数组释放方式不同 | 统一引用不自动改变数组所有权；结构增删边界见第7节 |

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

## 4. elementId：子元素的统一身份（建议确认）

- 在`WindowElement`增加非零`elementId`，由原生UI分配，Lua只读。
- 建议在当前UI运行期统一递增分配，不因窗口关闭、ClearChildren或同ID重开而重置，不回收复用；不将ID写入游戏存档。
- 原生原型实例化、分页附加元素、固定数组扩展窗口和动态组件创建，均须在元素对Lua可见前获得ID。
- `elementId`与数组下标、`seqNo`、业务槽号分别独立：原数组索引继续供原生业务使用，`seqNo`继续控制绘制与鼠标排序。
- 排序、组件草稿发布和数组迁移保留ID；复制为另一个独立元素时必须分配新ID。
- 控件查找使用`{windowId, elementId}`。删除元素后旧控件引用失效；同窗口ID重开后，旧窗口引用可访问新窗口，旧控件引用不会碰巧命中新元素。
- 不新增控件实例代次字段。当前拖动捕获和待派发窗口事件所用的内部生命周期校验，本轮不因简化Lua窗口引用而直接删除。
- ID分配不能回绕复用；耗尽时明确拒绝新增元素并沿创建失败路径处理。

## 5. Lua公共接口

### 5.1 查找和读取

```lua
local window = WinMgr.FindWindow(windowId)
if window then
    window.x = 100
    window.y = 120

    local element = window.elements[1]
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
- `elements[i]`继续从1开始按原数组/加入顺序访问，越界返回nil；获得元素后返回统一控件引用，而非保存下标。
- 控件回调self、AddXXX返回值和elements访问返回值统一为`LuaControlRef`。
- 现有动态控件`id/controlId`和原生元素`id`的兼容含义暂保留，新增只读`elementId`作为明确的统一身份；不静默改变旧脚本依赖的业务索引。内部解析统一使用elementId。
- `window.count`暂保留既有计数语义：组件窗口计根组件数量，固定数组窗口计元素数量。统一引用不隐式修改旧计数合同。

### 5.2 属性与方法

- 共用一套属性解析与赋值：位置、尺寸、层级、可见性、elements、回调及扩展属性。
- `Set`与直接属性赋值使用同一实现，避免两种入口产生不同结果。
- `Close`、`CheckKeyState`、`ShowTips`、`DrawRect`等公共操作不再要求Lua绑定；DrawRect按对应原生可见性判断，不能对所有窗口直接套用扩展窗口ACTIVE位。
- 普通属性读取、赋值及string/int扩展属性操作不创建Lua绑定。
- `dragMove`继续使用既有扩展窗口拖动实现；固定窗口不新增第二套拖动协议，设置不支持的属性明确报错。
- 窗口失效时`valid=false`，窗口`id/winId`仍返回引用保存的ID，其余业务属性返回nil；普通操作方法返回false，赋值及AddXXX明确报错，不自动创建窗口。
- 控件失效时`valid=false`，`elementId`仍可读取，其余业务属性返回nil；写入和控件操作明确报失效错误，不能操作相同索引的新控件。
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

## 7. 固定布局控件的结构增删（待确认边界）

当前`WindowComponentStore::publish`会替换window.elements，旧原生和部分扩展窗口的固定数组则依赖原索引、载荷释放和自身析构流程。仅换包装类型不能安全解决这部分问题。

本方案建议：

1. 所有窗口统一支持查询、修改已有子元素、设置回调及公共窗口操作，与绑定状态无关。
2. 已使用动态组件容器的窗口继续支持全部AddXXX/ClearChildren；普通Lua新建空窗口沿既有动态容器模式创建。
3. 固定布局窗口本次不接管数组、不清空业务元素。AddXXX/ClearChildren统一入口按原生存储能力检查，明确报“不支持结构增删”，不静默成功或覆盖数组。
4. 能力由原生UI的存储方式决定，不以是否存在Lua绑定判断；固定数组扩展窗口与原生固定窗口采用同样规则。

若要求固定布局窗口也支持AddXXX/ClearChildren，需要扩大本轮范围，先定义附加元素与原有业务元素的布局、索引和释放合同。该选择须在实施前确认，不能用类型统一掩盖能力差异。

## 8. 实施范围与步骤

1. UI侧增加elementId分配与查找，覆盖原生、分页、固定扩展数组和动态组件；核对发布/复制/释放。
2. 窗口引用改为仅ID，统一解析及元表，移除LuaNativeWindowRef分支，FindWindow不读取binding判断存在性。
3. 控件统一按elementId解析，迁移元素集合、回调self及AddXXX返回路径，移除原生元素下标代理。
4. 拆分LuaWindowBinding，将属性、窗口方法和控件方法改为临时解析后直接操作UI。
5. 统一Set/直接赋值和无效引用处理，落实结构增删边界及回调owner约束。
6. 更新接口说明与唯一进度，集中检查差异及全部调用点。按当前默认规则不编译、不运行；用户要求时再执行build.bat验证。

主要涉及`LuaUIRefs.hpp`、`LuaUI.hpp/.cpp`、`LuaUIProxy.cpp`、`LuaControlEvents.cpp`、`WindowElement`定义、`WindowComponents`、`WindowExtension`及原生元素创建路径。不引入依赖、不改变游戏协议或资源格式。

## 9. 验收项

| 场景 | 预期 |
|---|---|
| 查询原生、Lua及无绑定扩展窗口 | 返回同一元表的窗口引用，查找不创建绑定 |
| 窗口关闭、同ID重开 | 旧窗口引用先失效后指向新窗口，无代次限制 |
| 元素排序、数组发布与扩容 | 旧控件引用仍命中同一elementId |
| 删除元素、清空、重开同ID窗口 | 旧控件引用失效，不命中其他元素 |
| 通过elements和回调self访问同一元素 | 相同elementId，统一属性/操作入口 |
| 无绑定窗口修改坐标、文本、扩展属性 | 修改原生对象，不创建回调桥接 |
| 设置/替换/清空各类onXXXX | ref挂在UI对象上，所属Lua状态正确，释放一次 |
| 普通操作与回调中关闭/删除/新增 | 回调后重新解析，无悬空指针，无重复释放 |
| 原生背包、分页、滚动条、自动战斗面板 | 原索引、相邻关系、载荷owner及渲染/鼠标排序保持 |
| 固定布局窗口调用结构增删 | 按已确认合同明确报错，数组未被覆盖 |
| 丢弃所有Lua包装对象并触发GC | 原生窗口和控件继续存在 |
| 旧Lua NewWindow/Set/AddXXX及Open订阅脚本 | 返回形式和已支持行为兼容；新增语义以本SPEC为准 |

本文件为设计稿，验收表尚未执行。需要确认第4节elementId方案及第7节固定布局结构增删边界后再实施。
