# 原生元素统一控件模型（NEU01）

状态：2026-10-22，用户回复“go”确认；源码迁移与静态核对完成，编译、链接及运行验收未执行。

## 1. 已确认方向与目标

用户要求评估并合并UIControl，明确“以原生为准”，且确认控件专属业务数据仍归入WindowElementData。

本方案以现有WindowElement、WindowElementType、原生鼠标分发和绘制链为唯一基础。原生窗口与脚本创建的元素共用同一对象模型、载荷类型、排序和生命周期；移除UIControl对象体系及WindowComponentStore包装。不新增Native/Interactive模式、第二套控件类型枚举或按创建来源选择行为的分支。

公共属性归入WindowElement；类型专属数据通过WindowElementData的类型化指针持有。输入、动画、滚动、地图等专属功能保留，但通过原生元素处理链扩展；旧UIControl独有的通用处理规则不作为兼容基准。

本方案确认后覆盖旧LUP/LUR方案中要求保留UIControl、component/components桥接的具体结构；[SEV01](cg2d_sorted_element_vector_spec.md)的单一活动存储、稳定对象地址和延迟回收，以及[MEI01](cg2d_window_mouse_element_id_spec.md)的ElementId回调契约继续有效。

## 2. 当前源码依据

| 位置 | 已核对事实 | 对方案的影响 |
| --- | --- | --- |
| src/ui/WindowRuntimeState.hpp | WindowElement已经持有几何、类型、ID、owner、排序、Lua引用和释放标志；data部分成员仍指向UI控件对象。 | 不再增加另一份活动元素状态；data改指向纯载荷。 |
| src/ui/controls/UIControl.hpp、UIControl.cpp | UIControl另存ID、窗口身份、type、parentId、鼠标状态；包含draft/currentElement和虚函数分发。 | 去除重复身份、草稿元素和双向对象指针。 |
| src/ui/WindowComponents.cpp | bindPayload将data指向组件内部成员；add复制draft并恢复ID/creationSerial；组件表不拥有独立元素数组。 | 创建直接初始化最终元素，载荷独立拥有。 |
| src/ui/WindowRuntime.cpp | 鼠标和绘制遇component时改走UIControl；原生分支已按类型及ElementId处理。 | 删除提前分流，原生分支成为共用入口。 |
| src/ui/WindowExtension.cpp | 两处components->publish之后立即调用相同的WindowRuntime_UpdateElementOrder。 | 删除包装后保留一次原有排序发布。 |
| src/ui/WindowElements.cpp | 删除时先attached=false并失焦，再extract和removed=true；游标期间不回收，子孙先于父对象释放。 | 合并字段时必须保留删除中间态与释放顺序。 |
| src/script/LuaControlEvents.cpp、LuaUIProxy.cpp | 活动Lua引用已经在WindowElement；LuaElementEvents负责转接，旧动态控件没有监听函数时可默认消费命中。 | 引用不迁出元素；转接收敛到窗口绑定，默认消费差异见第6节。 |

依据为本线程读取的当前源码，不将历史运行结果当成本方案验收。

## 3. WindowElement公共模型

### 3.1 字段归属

| 原UIControl内容 | 目标处理 |
| --- | --- |
| id、type、windowOwner | 分别统一为elementId、WindowElementType、owner。 |
| winId、generation | 从owner读取；跨回调前复制窗口ID/代次与元素ID，回调后重新解析，不在活动元素再存一份窗口身份。 |
| name、parentId、hitable、mouseState | 迁入WindowElement；parentId使用ElementId，0表示根元素。原生默认hitable=true、parentId=0、mouseState=None。 |
| x/y/w/h/visible等element()访问 | 直接访问WindowElement；保持原生字段宽度和含义。 |
| callbacks | 活动Lua引用继续使用refUpdate至refEnter；事件转接收敛到WindowBinding，见第7节。 |
| attached | 取消该字段，保留removed，并增加detaching表示删除前失焦阶段，见第8节。 |
| draft、currentElement、element() | 删除；创建参数与临时载荷不构成另一份WindowElement。 |
| animationPayload、inputPayload | 合入data对应载荷，删除桥接副本。 |

WindowElement::component与WindowInstance::components最终删除。relatedElementId、removed、ownsPayload、creationSerial继续保留；不在本轮顺带删除这些已确认必要的职责。

原生state保持当前位含义，不能直接用UIControl::mouseState覆盖；mouseState仅表达通用悬停/按下状态，由统一事件结果更新。detaching与removed也不挤占未明确的state位。

### 3.2 创建与排序

1. ID仍由现有分配入口提供，固定ID/自动ID及重复ID处理保持MEI01/LUR01契约。
2. 创建参数和专属载荷先在局部准备；必要资源与尚未转移的Lua引用由局部所有者负责。通过WindowRuntime_AddElement创建最终稳定对象，再初始化公共字段并转移载荷/引用。失败路径不得留下可分发的半初始化对象；不另建持久draft元素或组件容器。
3. 排序统一使用已发布的(seqNo, elementId)，元素地址不随vector排序改变。显式seqNo、原型默认序号和现有背包分页序号均保持。
4. 建议取消动态控件的独立-nextId计数：动态创建未指定seqNo时取当前窗口最大有效seqNo加1，空窗口取0。这样默认追加到已有序列之后；不增加第二个排序计数器或排序缓存。旧动态控件默认后创建者优先的变化列入第6节，随本SPEC确认。
5. creationSerial继续独立生成；序号、ID和创建先后不互相替代。notifyUpdate发布时机与遍历期间禁止重排保持。

## 4. WindowElementData与专属数据

### 4.1 统一原则

WindowElementData继续为类型化指针联合体，由WindowElementType决定有效成员。只保存指针，不把字符串、地图缓存、输入上下文或所有类型的状态一起内嵌进WindowElement。

同一元素类型采用同一载荷结构和解释方式，不设置“原生载荷/脚本载荷”两种布局。确有额外能力时在对应载荷中增加具名字段或可选的类型专属状态指针；是否配置该能力只影响该能力，不选择另一条整体绘制/事件路径。

| 类型 | 载荷归属与处理 |
| --- | --- |
| EventArea | 保持无专属载荷；通用状态在WindowElement。 |
| Graphic | 继续WindowGraphicData。正常图、悬停图、按下图配置归该载荷；可选图态记录保存配置，imageNo继续作为当前原生绘制图号。没有图态配置时不覆盖业务回调写入的imageNo。 |
| Animation | 继续WindowAnimationData，只有一个真实action资源。资源编号、动作、方向、反向播放及需要的配置状态归该载荷；不再同时保存UIAnimeControl::action与animationPayload.action。帧推进和位置计算走原生动画分支。 |
| Text | 直接使用WindowTextData的style/text，删除UIControl::TextData包装。 |
| MouseArea、InputCaret、Number | 保持现有载荷和业务规则。 |
| Input | 继续WindowInputData，context仍是唯一活动上下文入口。动态输入所需容量、lastText、focused、previousInput以及自有InputContext放入可选输入状态；context指向其中稳定的上下文。原生借用上下文不复制、不取得其所有权。 |
| PngImage | UIPngImageControl*改为WindowPngImageData*；保存各图态的路径、源矩形和颜色等。 |
| ScrollView | UIScrollViewControl*改为WindowScrollViewData*；保存滚动位置、内容高度、滚动条外观及滑块拖动状态。子元素通过parentId查询，不由载荷重复拥有。 |
| Canvas | UICanvasControl*改为WindowCanvasData*；保存图元集合。 |
| MapEx | UIMapExControl*改为WindowMapExData*；保存地图位置、标注、点击状态、磁盘/可见区缓存和纹理资源。原地图算法和缓存淘汰规则保持。 |

“业务数据在data中”仅指控件专属数据；背包、交易、人物、网络等既有业务模块状态不搬入元素。

### 4.2 布局与分配

当前WindowGraphicData、WindowInputData、WindowAnimationData有sizeof断言，并存在calloc/free路径。扩充载荷时必须同步核对声明、静态原型初始化、分配尺寸、字段复制、释放及所有布局断言。

- 保持已有字段语义与必要前缀偏移；新增可选字段显式初始化为空。sizeof断言更新为新宿主结构的事实，不能直接删除断言来规避核对。
- 含非平凡C++成员的新载荷/附属状态使用匹配的new/delete；已有calloc/free载荷若保留该分配方式，其新增内容必须允许零初始化，并在释放前显式销毁其拥有的附属状态。
- 自动战斗窗口借用成员数组的ownsPayload=false继续有效；不能对这些数组或其借用内容执行free/delete。
- 纹理、动画和输入上下文各保留单一所有者。资源标死/释放沿现有边界执行，不新增公共裸SDK对象接口。
- 本轮允许上述已列宿主结构扩充，不改变协议、资源文件或存档布局；发现被当作外部格式使用的结构时停止该处实施并补充SPEC。

## 5. 统一处理链

### 5.1 鼠标与回调

以WindowRuntime_DispatchElementMouse为入口，删除component->process分流。保留原生窗口焦点、拖动物品phase、命中顺序、MouseFlag生成和事件消费规则；回调参数仍为ElementId。

处理顺序以现有原生链为基准：产生事件，执行原生/类型专属鼠标处理，再通过WindowBinding分发元素事件和窗口事件。每次可重入调用后检查窗口代次及元素身份；旧对象被删除或同ID替换后不能继续处理旧对象。一次事件不得同时经旧LuaElementEvents和新绑定分发。

图态变化、输入聚焦、滚动条及地图操作迁为具名类型处理函数或原生回调。已有原生业务mouseCallback不被通用默认回调覆盖；专属处理对已有能力未启用的原生元素不产生附加行为。

通用mouseState、hitable、onLeave与脚本拖动捕获在统一链中处理。物品拖放和脚本指针拖动现有职责继续区分，不重写它们的业务状态机；不再由UIControl另算一遍鼠标标志。

### 5.2 绘制与输入

以WindowRuntime_DrawCurrent及其原生type分派为基准，删除component->render分流。图形、文本、动画、数字和原生输入沿当前图层、锚点、坐标及资源接口；新载荷类型在同一分派中进入具名绘制函数。

输入聚焦统一关联WindowInputData/context及元素身份，移除UITextInputControl*焦点指针。保留输入限制、文本变更/回车事件与失焦恢复职责；原生输入上下文的所有权、聊天恢复和当前输入指针不因迁移改变。旧动态输入框独立背景/边框不作为默认绘制规则保留。

### 5.3 滚动容器与裁剪

parentId为0的元素由窗口入口遍历；容器子元素由该容器在同一套元素绘制/鼠标函数中递归处理，避免同帧重复更新、绘制和分发。使用既有窗口存储按parentId查找，不新增持久子节点数组；单次操作的借用快照允许保留，跨回调须复查身份。

父容器提供坐标偏移和相交裁剪区域，子元素继续使用自身原生类型规则。裁剪仅约束显示及命中范围，不回写元素原始几何，不引入组件专用坐标模型。根元素无父裁剪时，原生路径的坐标计算保持不变。

原生没有对应功能的PNG、画布、地图与滚动容器，保留其专属算法；这一保留不恢复UIControl通用分发器。

## 6. 以原生为准的具名迁移差异

下列为方案要求显式确认的变化，不应在实施后误记为“全部旧UIControl行为不变”。

| 项目 | 旧UIControl路径 | 本方案 |
| --- | --- | --- |
| 无监听函数时消费命中 | 动态控件可因默认回调结果1消费。 | 沿原生返回值规则；没有实际处理、绑定返回或捕获时不额外消费。 |
| 鼠标事件生成 | process根据自己的焦点/捕获条件计算标志。 | 统一采用原生事件生成及消费链。 |
| Lua的type数值 | 动态控件读UIControl::TYPE，原生元素读WindowElementType，两者编号不同。 | 统一返回WindowElementType；创建时既有类型名称继续映射。实施前列出本线脚本中依赖旧数字的调用者并同步，不能只删除旧枚举。 |
| 默认排序 | 组件nextId生成负seqNo。 | 建议无显式seqNo时追加到现有序列末端，见3.2；按原生遍历规则决定绘制与命中顺序。 |
| 图形/文本/动画位置 | 存在动态图层、文本定位和动画宽高中心定位等独立规则。 | 采用对应原生类型的图层、锚点和几何解释。旧脚本画面可能变化，不能承诺逐像素保持。 |
| 输入外观和显示门禁 | 动态输入有独立背景、边框及绘制路径。 | 采用原生输入显示/焦点链；不为旧外观保留第二条默认绘制分支。 |
| 容器裁剪 | UIControl子树中独立计算及分发。 | 同一原生元素函数接收父容器偏移/裁剪；专属滚动功能继续保留。 |

Lua创建函数、属性名、既有数据参数和事件名尽量保持原接口；type的数值变化以上表为准，“接口保留”不等于保留本表已明确改变的效果。若某个现有参数无法映射为原生载荷或类型专属功能，必须列出具体调用者及差异补充确认，不能静默忽略参数或编造业务替代。

## 7. Lua、窗口包装与模块边界

1. Lua查找/读写统一返回或解析WindowElement，再按type访问data；不再经ResolveControl或类型虚函数读取同一数据。
2. refUpdate至refEnter仍是唯一活动Lua引用。创建期间尚未转移的引用继续使用局部RAII清理；成功转移后原引用置空，避免双重解绑。
3. 当前唯一WindowElementEvents实现为LuaElementEvents。本方案移除这层持久控件事件包装与WindowEventBinding；元素鼠标/帧事件使用已有WindowBinding，文本变更/回车在该绑定增加具名文本通知入口。UI核心不直接依赖Lua注册表操作。
4. 脚本关闭时覆盖活动与退役元素解绑，元素回收不能重复释放引用。回调替换/关闭脚本后不得继续使用失效Lua状态；事件分发仍按每次回调重新取引用的既有契约执行。
5. 内部跨回调身份采用窗口ID、窗口代次及元素ID，必要时对比重新查询得到的对象身份；不长期保存旧UIControl*或载荷指针。既有Lua公开引用格式与同ID解析语义不在本轮擅改。
6. WindowComponentStore的find/add迁入现有元素操作入口；clear/rootCount无调用者，删除；changed无读取，删除；publish的重复调用删除。nextId按3.2处理，不保留components对象仅作空值门禁。
7. WindowExtension仅保留窗口管理、窗口拖动等职责，元素更新/绘制走共用链；不合并窗口对象层级、不改窗口打开/关闭及存档流程。
8. 原controls各文件可继续承载对应数据类型及函数，避免把地图、输入、滚动实现堆入WindowRuntime.cpp；文件名调整按实际依赖需要，不为此次重构拆出大量小工具文件。

## 8. 删除、回收与资源释放

### 8.1 两个失效阶段

当前attached=false发生在失焦之前，removed=true发生在移出活动表之后。不能把两处简单替换为同一个removed写入。

建议在WindowElement保留removed并增加detaching=false：

1. 沿当前顺序准备退役容量并取消指针捕获。
2. 对需要失焦的元素设置detaching=true，再处理自身/子树失焦；从此停止交互事件。此时一般ID查询仍可取得活动对象，以保留当前中间态。
3. 从sortedMap及byId提取原对象，设置removed=true，放入退役存储，更新活动数量。
4. 按原规则递归删除子元素，父对象的退役释放位置排在子孙之后；不允许旧删除误操作同ID新对象。
5. 存在活动游标时不回收。统一安全点销毁类型资源、Lua引用和公共属性，各只执行一次。

detaching只表达删除前阶段，不作为原生/动态模式或另一份活动存储。隐藏、禁用和清空仍按各自现有入口处理，不擅自把它们全部等同于删除。

### 8.2 释放责任

统一由现有元素释放链按type销毁专属载荷，取消“有component则删除组件并提前返回”的分支。公共Lua引用和properties在统一入口处理；ownsPayload=false只跳过载荷及其拥有资源的释放，不跳过公共元素清理。

动画资源标死、输入上下文失焦、地图缓存纹理释放及容器子孙顺序必须逐类登记对应入口。未发布的创建失败载荷也使用匹配释放方式，但不能当作已挂接元素派发失焦/离开事件。

## 9. 实施小步与完成标准

用户已确认本方案，按以下顺序由本线程实施，每步同步唯一进度并精确提交：

1. **公共结构与载荷**：增加必要公共字段，整理各type载荷及分配/释放对应表；统一类型编号，保留旧前缀含义，更新布局断言。过渡接线必须标注临时状态。
2. **创建与查询**：Lua/原型/自动战斗创建直接使用最终WindowElement；迁移parentId查找、默认序号和引用接线，去除draft及组件store。复杂类型尚未迁完时不能宣称模型统一完成。
3. **分发与类型功能**：统一原生鼠标、绘制、输入焦点和容器递归，逐类迁移PNG、画布、动画、输入、滚动、地图；排除同帧重复更新/事件。
4. **生命周期与Lua**：统一解绑、删除中间态、子孙顺序和安全点回收；移除事件包装及UIControl继承树、component/components和全部旧访问点。
5. **静态验收**：按第10节核对全部差异，清理临时接线和失效注释，记录实现与未完成运行验收的边界。

每个过渡提交必须接线完整，不能引入空实现或固定成功替代未迁移功能。最终不保留可运行的UIControl兼容分支。

## 10. 验收与风险边界

### 本次允许的静态验收

- 结构：活动元素只有sortedMap一份，byId仅借用；无UIControl实例、继承树、draft/currentElement、component/components及WindowComponentStore；API注册字符串中的历史名称单独区分。
- 载荷：每种type的创建、访问、复制/移动、释放对应一致；所有扩充字段有初值，分配族匹配；借用载荷与自有输入上下文不误释放。新布局断言有对应字段与消费证据。
- 行为：无组件来源模式；原生旧窗口分支除必要结构接线外不改变；动态路径差异仅限第6节和已列专属能力。
- 事件：原生回调、Lua元素/窗口事件、文本事件分别只分发一次；回调删除、替换、隐藏、关闭后的继续条件逐项核对。
- 生命周期：删除前失焦、逻辑删除、同ID重建、子孙释放及活动游标回收限制保持可追踪；创建失败资源与待转移Lua引用有清理路径。
- 容器：根/子元素不重复遍历，坐标偏移和相交裁剪传递明确，滚动位置与业务数据保留在data。
- 工程：仅改本线必要源/头及文档；不新增依赖、不使用RTTI、不修改旧工程；UTF-8、原有换行和Git差异检查通过。

### 尚不允许执行的验收

遵守当前限制，不编译、不运行客户端、不新增或执行测试、不计算摘要。后续获得明确授权后，才验证原生窗口、动态图文/动画、输入切换、滚动子树、地图/画布、自动战斗借用载荷及回调中删除的实际行为。

静态验收不证明编译、链接或画面正确；本方案也不承诺对象尺寸、分配次数或帧耗时一定下降。主要风险是动态界面按原生规则迁移后的外观/命中变化，以及载荷扩充和事件重入造成的生命周期遗漏；不得用旧编译结果或少量窗口验证代替完整范围。

## 11. 已批准方案

以原生为唯一行为基准，WindowElement保存公共属性、WindowElementData保存类型专属业务数据；移除UIControl及component/components包装；采用具名可选载荷状态扩展能力，允许同步更新宿主载荷尺寸；以detaching保留删除前失焦阶段；动态默认序号改为追加，旧动态交互/绘制按第6节迁移。用户已回复“go”批准实施。

## 12. 实施结果与静态核对（2026-10-22）

- 创建与查询直接使用WindowElement；公共名称、父ID、鼠标状态和命中开关不再有控件副本。UIControl及派生类型、WindowComponentStore、component/components、WindowElementEvents/WindowEventBinding、ResolveControl均已删除。原controls文件名和Lua常量表名称保留，不代表仍有旧对象或旧分发路径。
- 图形/输入/动画载荷分别为36/16/24字节，原前缀和新增成员偏移均有断言；原生calloc/free与非平凡载荷new/delete对应。自有输入上下文和动画配置各只有一个所有者；自动战斗数组继续ownsPayload=false。未发布载荷及待转移Lua引用由PendingLuaElement统一清理。
- 鼠标统一从原生事件生成进入类型处理与WindowBinding；无监听默认返回0，输入聚焦、滚动和捕获等实际处理仍消费事件。跨回调引用核对窗口代次、元素ID及creationSerial；指针拖动也使用同一身份规则。文本变更/回车经elementText通知，不保留控件事件包装。
- 绘制由原生类型分派统一进入。滚动子树传坐标与相交裁剪；图形/动画裁剪原生入队记录，保留图层、翻转和调色板；文本裁剪可见字形后仍用原生DrawText2，不走旧颜色转义入口。PNG、画布和地图继续使用各自专属算法。
- 删除仍分detaching与removed两个阶段；失焦、取消捕获、子孙先于父对象释放和活动游标期间不回收的规则保留。删除被借用的输入恢复目标时同时清理恢复引用，避免恢复到已退役载荷。
- Lua几何写入按原生int32字段承接；借用InputContext的text/font/color仍直接写原上下文。未配置动画附属状态或输入容量能力时，不把同名自定义属性静默吞掉。默认seqNo取当前最大值加1；显式序号继续走原发布入口。

Lua常量名称保留，值统一如下：

| 常量名称 | 原动态值 | 当前原生值 |
| --- | ---: | ---: |
| NONE | 0 | 0 |
| IMAGE | 1 | 1 |
| TEXT | 2 | 3 |
| PNG_IMAGE | 3 | 8 |
| TEXT_INPUT | 4 | 5 |
| ANIME | 5 | 2 |
| SCROLL_VIEW | 6 | 11 |
| MAP | 7 | 10 |
| CANVAS | 8 | 9 |

本线仅找到第三方示例Lua文件，未发现业务Lua文件需要同步旧数字比较；运行目录中的外部脚本不在本次源码核对范围。已核对新增类型/函数接线、旧符号残留、载荷分配释放、原生根元素绘制差异和回调后的继续条件；完成括号与枚举引用静态检查、UTF-8/换行核对及Git差异检查。上述结果不代替编译、链接或运行验收，第10节后续验收范围保持。
