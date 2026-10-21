# 鼠标回调统一元素ID（MEI01）

状态：2026-10-22，用户回复“ok”确认；实现及静态核对完成，未编译或运行验收。

## 目标与现状

用户要求评估将WindowElementPrototype::mouseCallback的index改为elementId并移除nativeSlot，同时询问relatedElementId、removed、ownsPayload及creationSerial是否仍有必要。

改动前WindowElementCallback签名为S32 index/S32 state；WindowRuntime_DispatchElementMouse实际传入element.nativeSlot。NativeElementKey将非负槽号转换为slot+1，元素ID为U32；排序位置由elementAtIndex单独表达。因此这次修改涉及参数语义与所有消费者，不能只重命名参数或替换调度实参。

## 已确认实施范围

1. WindowElementCallback统一为`std::int32_t (*)(ElementId elementId, std::int32_t state)`；原型与实例共用该类型，分发直接传当前元素的elementId。
2. 同步所有绑定函数、前置声明、回调表及直接调用。元素查找直接使用ID；固定元素判断使用对应窗口枚举。列表行号、背包位置等业务索引按原业务基址显式换算，保持原值、分支和调用顺序，不把业务索引统一改成ID。
3. 当前元素与拖动焦点元素同步保存ElementId，空值使用0；同步WindowDrag_ReadFocusElement、WindowDrag_SetFocus及其调用者。拖动物品来源sourceIndex仍保留业务索引含义。
4. 删除WindowElement::nativeSlot及全部读写，包括原型创建与AutoBattleWindow创建。创建过程的局部槽号、原型数组索引及仍有用途的NativeElementKey不因此全部删除；elementAtIndex继续表示已发布排序位置。
5. 不新增逐元素替代槽号、ID到槽号的缓存或另一套存储，不改绘制排序、业务数据、载荷生命周期及Lua按ID访问合同。

## 其余字段结论

| 字段 | 当前消费者与职责 | 本次处理 |
| --- | --- | --- |
| relatedElementId | WindowScroll_ReadPosition、SetPosition、ApplyWheel及鼠标滚动处理按ID查找关联滑块；表示两个元素之间的关系。创建时仍按原槽前项赋值。 | 保留。不能以排序前项或任意ID减一替代关联；本次不扩展滚动条配置设计。 |
| removed | RemoveElement移出活动表后标记，旧对象延迟回收；LuaUIProxy在同一批事件的每次回调前检查，WindowRuntime_IsElementInteractive也检查。 | 保留。活动容器中的部分检查可有冗余，但借用旧对象的事件链仍需要失效判断。 |
| ownsPayload | ReleaseElement据此决定是否释放非组件载荷；AutoBattleWindow置false，图片载荷指向其成员数组。 | 保留，防止对借用数组元素执行free；不改所有权设计。 |
| creationSerial | AddElement分配递增序号，游标创建时冻结上限，next跳过之后新建的元素；组件发布保留序号。 | 保留，确保当前遍历不接纳新对象，包括同ID重建；固定ID和排序位置不能替代创建先后。 |

## 验收与边界

- 静态核对所有回调赋值、声明、定义和直接调用；重点核对index参与的switch、范围、减法、数组下标和拖动源参数，避免一位偏差及无符号减法改变旧业务语义。
- 核对焦点设置、比较和清空全部统一为ID/0；不得混用旧槽号与新ID。
- 检索确认nativeSlot在当前实现中无残留；保留历史文档作为原契约记录。
- 核对自动战斗窗口、动态分页元素及滚动条相关调用；保留既有元素删除、借用载荷与游标行为。
- 仅做源码、UTF-8/换行及Git差异静态核对；不编译、不运行、不新增或执行测试，不将静态核对计为运行验收。

本方案经用户确认，覆盖SEV01中保留nativeSlot回调契约的原范围。

## 实施与静态核对记录

- 356个具名回调、709处声明/定义及自动战斗六个匿名回调改用ElementId；确认框转发、动态回调表与16处具名直接调用同步，当前元素与拖动焦点统一为ID/0。删除nativeSlot全部源码读写，大地图绘制按ID判断元素区域。
- 静态核对原型表2596项固定ID均与原槽号加1一致；当前元素直接按ID查询，固定元素分支使用所在窗口枚举。跨窗口共享逻辑或无可见专用枚举的位置保留NativeElementKey常量表达，避免为本次迁移扩展公共枚举。业务行号、原型数组及仍接收旧槽号的既有辅助接口按需从ID换算有符号局部索引，不引入存储字段或缓存。
- 355个常规回调正文按ID/槽号映射、枚举分支及转发实参反向归一后，非注释词法序列与改动前一致；大地图转发、OnElement及绘制、自动战斗成员载荷访问、确认框转发和焦点生命周期另行静态核对。保留relatedElementId、removed、ownsPayload、creationSerial及对应行为。
- 检查动态绑定、声明/定义、直接调用、lambda捕获、ID比较与有符号业务运算；当前源码无nativeSlot或旧焦点索引全局名残留。UTF-8与既有换行保持，Git差异检查通过。未编译、运行客户端、新增或执行测试；静态核对不代表运行验收。
