# LUP01：Lua UI 原生窗口与控件代理

## 1. 目标与授权

按用户要求完成 SPEC 后直接实施，所有修改结束后使用现有 build.bat 做 Debug 编译及链接验证。Lua UI 只负责属性代理、参数转换和事件转接；窗口及控件生命周期由原生 UI 管理。兼容已有原生窗口和 Lua 接口，不启动客户端、不新增依赖、不使用 RTTI。

## 2. 现状及必须保持的行为

- UIWindowInstance 已是 ui::WindowInstance 的别名；LuaWindow 仍单独拥有 controls/retiredControls，执行反向绘制及鼠标遍历。
- 原生 WindowElement 是连续数组，业务依赖原索引和相邻元素；elementsSorted 是非拥有指针数组，不得改为派生元素数组。
- 原生文本使用宽高作为绘制偏移，Lua 文本使用宽高作为区域；图片三态、输入框、滚动裁剪及动画定位也有差异，迁移不能直接套用旧类型绘制而改变效果。
- Lua 窗口当前有实例代次检查，原生代理只有 ID；统一补齐代次，关闭后旧代理不可操作重开的同 ID 窗口。
- ShutdownLuaUI 原本清理扩展窗口；迁移后仍清理，由原生 UI 生命周期执行，先释放绑定再关闭 Lua state。

## 3. 数据与所有权

1. 保留旧类型 0～7、旧字段及 WindowPrototype 初始化方式。新增成员追加并默认初始化，旧窗口不要求注册脚本信息。
2. WindowElement 仍是普通连续值类型，扩展能力采用组合，不继承。新增原生组件引用与稳定 ID/父级关系；共用位置、尺寸、可见性、seqNo，不保留另一份活动属性。
3. 原生组件承接原 UIControl 的图片三态、文本、动画、输入、PNG、Canvas、MapEx 和 ScrollView 功能。已有类型载荷直接引用实际数据；新类型追加到枚举及指针联合体。组件存在时采用对应原生组件行为，旧元素继续既有分支。
4. 窗口拥有动态元素存储、组件和待回收对象；组件拥有自身资源及事件绑定。LuaWindow 不拥有窗口或控件，只保留非拥有窗口引用和 Lua registry 引用。
5. Lua userdata 不拥有任何窗口资源；GC 只销毁代理。窗口关闭统一标记，调度安全点完成释放。原生实例代次与单调控件 ID 防止代理串用。

## 4. 修改与调度

- Lua 创建的动态窗口允许添加/清空控件；原生窗口先开放已有控件属性访问，不开放数组增删，保护固定索引和邻接关系。
- 新控件创建后可立即通过返回代理读写；数组发布在安全点进行。遍历中清空立即使旧控件失效，实际释放延后；禁止回调导致活动数组或缓存悬空。
- 统一使用 WindowElement 的 seqNo 和 elementsSorted。旧 Lua 后添加控件优先，默认 seqNo 按添加顺序递减；旧原生默认顺序保持。
- 父子控件共用窗口元素存储，ScrollView 不再拥有另一套控件树。遍历按同父级过滤排序缓存，父级负责子坐标偏移、裁剪和事件消费。
- 保留原生鼠标回调；脚本监听单独绑定，禁止替换原业务回调。关闭/隐藏/清空过程中及时恢复输入焦点，清理拖动状态。
- 保留旧Lua鼠标回调未配置时返回1的消费约定，hitable控件继续阻止点击穿透；原生旧元素未配置脚本监听时返回0，不抢占原业务处理。
- 排序仍在 update 维护，元素变动时刷新，至少每秒重排；绘制不临时排序。

## 5. Lua 接口

- 保留 WinMgr 创建/查找/关闭/置前以及 AddImage、AddAnime、AddText、AddTextInput、AddPngImage、AddCanvas、AddMap、AddScrollView、Set、ClearChildren 等现有名称与返回形式。
- 增加 __newindex，属性赋值与 Set 共用处理，包含必要的隐藏/焦点/字体副作用。
- window.elements 是集合代理，Lua 索引从 1 开始，对应原数组索引 0；排序不改变访问索引。count 保留原 Lua 顶级数量含义，elements 长度反映全部元素。
- 窗口开放 x/y/width/height/layer/visible；控件开放 x/y/width/height/visible/seqNo 和类型适用的数据属性，包括 text/font/color/image。ID、类型、指针、缓存只读，不把结构体内存直接暴露给 Lua。
- 既有控件回调及文本 change/enter 仍由桥接调用；UI 层不包含 lua_State、registry 索引或 Lua C API。
- 字符串沿既有 Lua 编码边界转换为内部 wstring，保留字体传递及原输入长度规则。
- 用户补充：扩展属性直接挂载在 WindowInstance / WindowElement 上，按需分配 `std::map<std::string, std::variant<std::string, std::int32_t>>`。仅接受字符串和S32整数；拒绝布尔、非整数、table、函数、userdata等值，不递归转换。原生属性优先且不能被扩展属性覆盖；事件回调仍走独立绑定接口。扩展字符串是Lua原始字节值，只有写入UI文本字段才执行既有编码转换。
- 用户补充：onUpdate/onDraw/onEvent 的 registry 引用直接存放在窗口/控件的 refUpdate/refDraw/refEvent。回调值不进入扩展属性表；桥接保留所属Lua状态，统一解绑。旧 update/draw 字段作为窗口别名。窗口 onEvent 参数为窗口、Lua元素索引、鼠标标记；控件 onEvent 保持控件、鼠标标记。原生旧业务回调先执行，脚本事件追加，消费结果合并。
- 用户再次补充：控件其余既有事件 onClick/onPress/onHover/onLeave/onDrag/onDrop/onChange/onEnter 全部改为元素上的对应 refClick/refPress/refHover/refLeave/refDrag/refDrop/refChange/refEnter，不在绑定对象保留另一份活动引用。

## 6. 实施顺序

1. 原生组件与窗口扩展生命周期，迁移控件实现及调度。
2. LuaWindow 收敛为绑定，既有接口接入原生元素，补充属性/集合代理和原生代次检查。
3. 核对原生旧分支、自动战斗窗口、输入/动画/纹理资源、回调增删/关闭和排序；同步进度。
4. 全部修改完成后 build.bat Debug，处理本次引入的问题，完成静态差异检查和提交。

## 7. 验收与限制

- 静态确认 LuaWindow 不再拥有 controls/retiredControls，组件不依赖 Lua，公共属性单一来源，原生旧索引/回调/分配族保持。
- 确认原生/扩展关闭与 Lua shutdown 都能释放组件、回调引用和排序缓存，代理代次一致。
- Debug 编译及链接通过；原生 UI、旧 Lua 脚本显示与交互、滚动/三态/输入/动态增删等运行验收需用户客户端验证，编译通过不计运行通过。

## 8. 当前状态

主体实现、静态差异检查及Debug编译/链接已完成。主循环旧UpdateLuaScene入口保留为转调；清理旧拖动管理残留及本次新增编译警告。最后补齐旧Lua默认鼠标消费规则后再次执行build.bat Debug，退出码0。未运行客户端，界面及交互验收待用户验证。

## 9. 实际接口与资源归属

| 项目 | 实现位置/含义 |
|---|---|
| 组件和动态元素 | ui/WindowComponents，WindowInstance::components拥有；LuaWindow只借用窗口 |
| 扩展窗口生命周期 | ui/WindowExtension，加入统一窗口调度、关闭与安全点回收 |
| 公共属性 | WindowElement中的x/y/width/height/visible/seqNo，组件通过element()访问 |
| 原生已有类型 | Graphic=1、Animation=2、Text=3、Input=5；组件绑定union到真实载荷 |
| 新增类型 | PngImage=8、Canvas=9、MapEx=10、ScrollView=11；原0～7编号保持 |
| 对象扩展属性 | WindowInstance::properties、WindowElement::properties，指向按需创建的WindowPropertyMap |
| 窗口回调 | refUpdate/refDraw/refEvent；onUpdate/onDraw参数为窗口，onEvent参数为窗口、元素索引、鼠标标记 |
| 控件回调 | refUpdate/refDraw/refEvent及refClick/refPress/refHover/refLeave/refDrag/refDrop/refChange/refEnter |
| 控件回调参数 | 更新/绘制为控件；鼠标为控件、事件标记；输入change/enter为控件、文本 |
| Lua代理 | LuaUIProxy，动态控件保持LuaControlRef，原生控件用带窗口代次的索引代理 |

旧Lua控件的type属性仍使用既有脚本编号，不将旧脚本编号误当原生WindowElementType。原生控件代理的type返回原生类型编号。新增类型无需改变旧WindowPrototype初始化；新组件通过原生动态组件创建入口构造。

原生窗口旧state不含统一显隐门禁，因此脚本显隐使用默认false的scriptHidden，未设置过的旧窗口照常调度。Lua动态窗口保持既有ACTIVE语义。隐藏时释放所属输入焦点；关闭仍沿原业务释放回调。

## 10. 使用示例（文档，不执行）

```lua
local status, window = WinMgr.NewWindow {
    id = 120, x = 100, y = 100, width = 200, height = 120,
    visible = true,
    onUpdate = function(self) end
}
local label = window:AddText {
    name = "label", x = 10, y = 10, width = 120, height = 16,
    text = "状态", font = 0, color = 127
}
window.page = 1             -- 原生窗口properties中的int32
label.description = "说明" -- 原生控件properties中的string
label.seqNo = -10
label.text = "更新后的状态"
label.onDraw = function(self) end
label.onDraw = nil          -- 解除registry引用；普通扩展属性不接受nil
local first = window.elements[1]
local total = #window.elements

local native = WinMgr.FindWindow(71)
if native then
    native.x = 80
    native.elements[1].seqNo = 0
end
```

window.elements包含当前有效元素及尚未发布的新元素；使用原数组/加入顺序，从1开始，越界返回nil。排序不改变访问索引。ClearChildren使旧控件代理立即失效，新增控件使用新ID。关闭窗口或清空控件释放其扩展属性和全部回调引用，不允许Lua GC越过原生窗口生命周期销毁对象。

## 11. 客户端验收项（本轮未运行）

| 场景 | 预期 |
|---|---|
| 打开原生背包、银行、小地图、大地图、自动战斗面板 | 原索引、分页/滚动、按钮、遮挡及输入路径保持 |
| 运行旧Lua窗口脚本 | 原NewWindow/AddXXX/Set接口、图片三态、字体、输入、地图及滚动保持 |
| 修改seqNo、添加控件 | 渲染和鼠标统一排序，集合索引保持，父子裁剪生效 |
| 回调中ClearChildren后添加控件 | 旧代理立即失效，遍历不访问释放内存，新控件可读写并在安全点发布 |
| 回调中隐藏/关闭窗口或父控件 | 后续相关控件停止处理，输入焦点恢复，无点击穿透变化 |
| 关闭后以相同ID重开 | 原代理valid=false，新代理独立，属性表和ref不串用 |
| 设置string/int32扩展属性及onXXXX | 属性落在UI对象；回调替换/设nil正确解绑；复杂扩展属性报错 |
| Lua GC、Lua退出、客户端关闭 | GC只回收代理；原生窗口生命周期释放组件、属性、纹理、动画和registry引用 |

## 12. LUP02：控件按住移动与拖动捕获

用户要求补齐普通Move到onDrag的处理，直接使用apply_patch实施，完成后build.bat Debug验证。

- 仅可命中且已绑定refDrag的控件在左键按下时获得捕获；支持Lua动态控件和已有原生元素的Lua代理。没有onDrag的旧控件不改变行为。
- 捕获归属由原生UI保存窗口ID、实例代次和稳定控件ID（原生固定元素使用原索引），不长期缓存元素地址。父容器裁剪仅参与初次命中。
- 捕获期间即使指针移出控件、滚动容器或UI窗口，仍向原控件派发；同帧其它控件及窗口不能接收这次鼠标交互。控件拖动优先于所属UI窗口的位置拖动。
- 沿现有帧输入采样，不新增Move事件位。按住时保持onPress语义，仅坐标变化时附带DRAGGING并触发onDrag；一帧内多次Move合并为最新坐标，不自动修改控件位置。
- 实际移动后收到左键松开边沿，先处理最终坐标变化，再仅发送一次DRAG_FINISH/onDrop。单击没有移动不发送onDrag/onDrop。没有松开边沿却丢失按住状态，按取消处理。
- 捕获的控件/父级隐藏、禁用命中、删除、取消onDrag，窗口隐藏/关闭、客户端失焦，均取消捕获且不发送onDrop。取消后当前帧仍消费鼠标，防止穿透。
- 独立于原生物品拖放phase，不写物品拖放的焦点/来源/回调状态；原生拖放已开始时不建立新捕获，接管时取消控件捕获，保留原有事件处理。
- 窗口边界指游戏内UI窗口；不新增操作系统级SetCapture。离开游戏客户区导致按住状态清除，或应用失焦时取消。
- 回调继续使用(self, flags)，沿现有onEvent/onPress/onDrag/onDrop分派；回调中关闭、清空及重建后重新核验身份，避免悬空引用或重复结束事件。

验收覆盖：按下静止、按住移动、移出控件/UI窗口、滚动容器子控件、最后移动与松开同帧、无移动松开、回调隐藏/清空/关闭、同ID重开、失焦、原生物品拖放并存。运行验收仍待客户端验证。

实施验证：已完成LUP02，补齐同批回调派发前的可交互性检查。最终build.bat Debug编译及链接通过（退出码0），git diff --check通过；未运行客户端。

## 13. 全局窗口打开订阅

新增`WinMgr.OnWindowOpen(callback)`，回调参数为窗口ID，返回与OnWindowClose相同的订阅句柄，支持`handle:Unregister()`。

```lua
local opened = WinMgr.OnWindowOpen(function(windowId)
    local window = WinMgr.FindWindow(windowId)
    -- 在这里访问窗口属性及子控件。
end)

-- 不再监听时注销。
opened:Unregister()
```

- 原生及扩展窗口创建成功后排队，下一次UI更新时通知，避开创建调用方尚未完成的业务初始化；每个新实例通知一次。
- 复用已有窗口、置前和重新显示不触发；注册时不扫描已有窗口。订阅会接收派发时仍待处理的打开事件。
- 派发前核验ID及实例代次，已关闭或被替换的实例跳过。某个订阅回调关闭窗口后，停止该实例剩余打开回调。
- 订阅列表使用快照，回调内新增订阅不参与当前事件，注销立即生效；回调中新建的窗口留到下一次UI更新通知，避免递归。
- Lua回调错误记录日志，不中断其他有效订阅；关闭脚本时沿现有全局事件清理释放引用，UI关闭时清空待处理通知。

本次完成静态接线及差异检查；未编译、未运行客户端。
