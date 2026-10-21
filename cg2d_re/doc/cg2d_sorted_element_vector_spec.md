# SortedElementMap 主存储改为 vector 的 SPEC

编号：SEV01。日期：2026-10-22。状态：用户已确认，已完成源码改造及静态核对；尚未编译或运行验收。

## 1. 目标与授权边界

将 SortedElementMap 的唯一活动元素存储从 std::map 改为 vector，使 elementAtIndex(index) 能按当前排序位置直接访问，复杂度为 O(1)，同时保持元素对象地址稳定及按 elementId 查询的能力。

用户先授权整理SPEC，随后询问删除差异，并在澄清后回复“ok”确认实施。现有鼠标回调继续接收nativeSlot，elementAtIndex的排序位置不作为该参数的替代。PetSkills及上一轮崩溃修复不顺带重写。

## 2. 当前源码约束

- WindowRuntimeState.hpp：Storage 为 map<pair<seqNo, elementId>, WindowElement>，byId 哈希表借用节点内的元素地址；retiredElements 保存取出的 map 节点。
- WindowElements.cpp：WindowElementCursor 保存排序键及创建序号上限，不跨回调保存迭代器；删除其他元素后按键继续，本轮忽略新建元素。
- notifyUpdate 在遍历深度非零时拒绝重排；LuaUIProxy.cpp 修改 seqNo 后调用它，失败时恢复字段。
- map 的键与元素中的 seqNo 是分离的。直接改 seqNo 后，实际遍历次序仍由旧键决定，直到 notifyUpdate 发布新顺序。vector 不能直接以可变字段作为二分查找依据。
- 当前删除先标记 removed，再删除子孙、恢复相关输入状态，最后从活动容器提取到退役列表；在此期间，ID查询及游标跳过逻辑删除对象，但 size 仍可能暂时包含它。
- LuaUI.cpp、LuaUIProxy.cpp、WindowExtension.cpp 存在直接遍历容器的消费者；Lua 解绑还会遍历退役元素，不能只调整 UI 容器本身。

## 3. 建议存储形式

```cpp
using OrderKey = std::pair<std::int32_t, ElementId>;
using ElementOwner = std::unique_ptr<WindowElement>;
using Storage = std::vector<std::pair<OrderKey, ElementOwner>>;
```

每个条目保存已发布的排序键和唯一拥有的元素对象。与裸 vector<unique_ptr<WindowElement>> 相比，保留排序键是为了维持 notifyUpdate 的发布时机，不是再建立一份排序索引。

活动 vector 是唯一活动元素存储；byId 保持 unordered_map<ElementId, WindowElement*>，只借用对象地址。vector 扩容、插入、删除、重排只移动键与 unique_ptr，不移动 WindowElement。保留 SortedElementMap 类型名，避免无关调用改名。

retiredElements 改为拥有已移出活动列表的 ElementOwner。活动与退役之间转移所有权，不复制元素。ReleaseElement 继续负责组件、Lua引用及载荷释放，最后由 unique_ptr 销毁元素对象；不能仅靠默认析构替代现有释放过程。

## 4. 查询与排序合同

| 操作 | 合同 | 复杂度 |
| --- | --- | --- |
| elementAtIndex(index)，含const重载 | index从0开始；检查边界后直接返回对应条目的对象指针；负数或越界返回nullptr | O(1) |
| FindElementById / operator[] | 保持按elementId查询，缺失返回nullptr，不插入 | 平均O(1) |
| size / empty | 表示活动元素数量及是否为空 | O(1) |
| 插入 | 按已发布键二分定位，然后插入唯一owner并发布byId索引 | O(n) |
| 移出活动元素 | 定位条目，移出owner并压紧vector，删除byId索引 | O(n) |
| notifyUpdate | 遍历外刷新条目键，再按(seqNo, elementId)升序排序 | O(n log n) |
| 游标继续 | 按保存的已发布键做upper_bound，不保存vector迭代器或位置 | 每次定位O(log n) |

隐藏元素仍属于活动列表，占用排序位置；逻辑删除元素不属于活动列表。排序位置不是稳定身份，也不是原生槽号：增删或notifyUpdate后需重新获取。返回指针仍是借用，不允许在元素释放后继续使用。

创建保留当前初始发布键及创建序号顺序；组件或原型随后写入seqNo时，不提前改变已发布顺序。notifyUpdate 对相同seqNo继续以elementId排序；遍历中重排仍按现有规则拒绝，Lua异常恢复行为保持。

## 5. 遍历、删除与释放

### 5.1 回调期间增删

WindowElementCursor 保留当前 key、limit、started 语义。每次 next 重新在vector中查找大于上次键的位置，跳过 creationSerial 超过构造时上限的对象。新增元素不能在同一次遍历中执行；当前或后续元素被删除时，不重复或错跳其他存活元素。

元素对象地址稳定不代表vector条目地址稳定。所有跨回调的vector迭代器、条目引用及下标都必须消除；只允许保存元素身份、已发布键或符合既有生命周期约束的对象指针。直接范围遍历的消费者须逐项核对是否可能重入增删，有回调边界的路径改用游标。

### 5.2 已确认的行为差异

为使elementAtIndex始终能直接下标访问，活动vector不保留removed占位。在现有逻辑删除点，同时移出活动vector和byId并更新活动数量，再继续子孙删除与输入处理。此前的取消拖动及组件失焦阶段保持原顺序。

嵌套删除处理会更早看到父元素退出size、elementCount及底层活动存储。需纠正初稿：现有elementAtIndex和游标本来就跳过removed，因此它们看到的有效元素位置通常不变；提前变化的是数量和底层容器内容。ID查询原本在该阶段已返回空，ID身份失效时机保持。用户已在此澄清后确认，不宣称完全无行为变化。

同ID重新创建也可能更早成功：旧实现尚未提取的map键可能阻止插入，新方案已移除活动条目。旧对象与新对象由对象身份和creationSerial区分，旧删除过程不得擦除后来新建对象的byId记录；这一时机差异已随方案确认。

移出的父对象立即由退役存储持有，确保递归与重入期间仍有唯一owner，并可被Lua解绑访问。实际载荷释放仍在安全点进行，保持子孙先于父对象的退役释放次序；必要时在删除结束时调整退役队列顺序，仅移动owner。存在活动游标或未结束的删除过程时不得回收其借用对象，不新增第二份活动元素集合。

### 5.3 异常与一致性

- 创建时，元素分配、vector插入与byId发布作为同一事务；任何阶段失败都回滚已完成部分，不留下单边索引或泄漏owner。
- 移除前准备好接收owner所需的退役容量；失败时不先遗失活动owner。子孙递归或输入处理异常时，已移出的对象仍归退役列表管理。
- 删除相同ID或关闭窗口不能重复释放；Lua解绑覆盖活动及退役对象。新建同ID对象继续由creationSerial区分本轮遍历资格。
- 不迁移分配族、不引入外部依赖、不更改载荷所有权或脚本身份规则。

## 6. 实施范围

1. WindowRuntimeState.hpp：Storage、拥有类型、索引接口、退役存储及游标说明。
2. WindowElements.cpp：创建、索引事务、重排、游标、移除、清空和回收。
3. LuaUI.cpp：活动／退役元素解绑遍历；LuaUIProxy.cpp：活动元素遍历及重排异常链。
4. WindowExtension.cpp：隐藏窗口的输入恢复遍历；WindowComponents.cpp：组件借用地址、发布排序及数量观察点核对，确需适配才修改。
5. WindowComponents.hpp、controls/UIControl.hpp：同步不再准确的map节点注释。

不重写窗口业务回调、不把nativeSlot改成排序位置、不更改Lua elements按ID访问的含义。确认后以本SPEC覆盖LUR01中要求map节点／node_type的具体实现；其单一活动存储、稳定身份、延迟回收、禁止附加排序缓存等约束继续保留。

## 7. 验收要求

- 静态检查：不存在map专属node_type／mapped／extract依赖残留；byId与活动vector一致；无跨回调保存vector迭代器；查询边界和const版本一致；直接索引内部不遍历、不重建缓存。
- 地址与排序：扩容、插入、删除其他对象及重排后存活对象地址不变；相同seqNo按ID稳定定序；seqNo写入在notifyUpdate前后遵循发布合同。
- 遍历：删除当前／后续／父子元素，回调中新建或删除后重建同ID元素，均保持本轮资格与继续位置；遍历中重排拒绝后旧顺序有效。
- 生命周期：父子释放次序、删除中的Lua解绑、窗口关闭、输入失焦与活动数量观察符合第5节；分配失败后所有权和两个索引一致。
- 回归范围：原生与动态窗口、Lua增删和seqNo设置、窗口隐藏／关闭、商店滚轮等既有元素查询路径。鼠标回调仍收到原生槽号。

当前仅允许必要静态核对；不得将静态通过等同于上述运行验收通过。编译、测试和客户端回归需另获授权。本SPEC确认只授权实现该设计，不自动恢复运行权限。

## 8. 实施与验证记录

- 主存储使用本SPEC的键／unique_ptr条目；两个elementAtIndex重载仅检查边界并直接下标访问。byId仍借用稳定对象地址。
- 创建通过二分定位插入vector，再发布byId；byId发布失败时擦除新条目。移出同时核对ID及对象地址，旧对象不会误删同ID新对象。
- notifyUpdate保留遍历拒绝及旧键发布规则，键未变化时不排序。游标每次按已发布键重新upper_bound，保留创建序号上限。
- 删除先准备退役容量，再进行原有取消拖动／组件失焦；移出后立即更新活动数量。父owner先进入退役列表，局部RAII对象在正常或异常退出时将其移到队尾，保持已删除子孙先释放；游标存活期间回收器延期。
- Lua绑定初始化／解绑及窗口隐藏的直接遍历改用游标；退役Lua解绑改为访问unique_ptr中的对象。组件持有的对象地址与公开回调参数保持。
- 只完成源码调用关系、所有权路径、异常回滚、遗留map节点API、编码换行和Git差异的静态核对。第7节运行场景仍待后续授权执行，未新增测试或用静态结果代替运行结论。
