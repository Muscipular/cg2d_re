# Yobi 扩展字段与载荷类型化（YTS01）

日期：2026-10-16。用户要求判断bytes290结构化和payload联合体，并明确“做完spec后执行”；本方案完成后直接实施，不另等确认。

## 现有事实

- `YobiAction::bytes290`长20B，`CopyYobiAction_41EB10`复制完整五个W32；`UpdateBattleAction113_40EFE0`读取相对+4的S32时长，技能创建路径写入60并再次读取。其它16B目前没有独立业务解释，不猜字段含义或地址属性。
- `payload`是独立calloc的对象指针，由`DeleteYobiAction_404D10`先free，再销毁／释放action。当前实际转换涉及下表12种类型；零载荷对象仍不额外分配。
- 窗口两种载荷同为0x24，不能按大小推断类型；回调可替换，也不能作为统一释放的类型判据。
- `CopyYobiAction_41EB10`暂时复制源指针，`CloneBattleAction_41EAB0`随后恢复新分配的战斗载荷并浅复制其内容。类型元数据必须与对应指针一起复制／恢复。

## bytes290 方案

在内部base.hpp定义20B、对齐4的`YobiExtraState290`，成员保持原次序：

```cpp
struct YobiExtraState290 {
    std::array<std::byte, 4> bytes290;
    std::int32_t castDuration294;
    std::array<std::byte, 12> bytes298;
};
```

主对象字段改为`extraState290`。原+4读写改为`castDuration294`，完整复制改为同类型赋值；保留各次读取位置，不跨调用合并读取。加入大小、对齐、偏移和可平凡复制断言，未知区继续保留字节。

## payload 方案

采用`YobiPayload`指针联合体，配单字节`YobiPayloadKind`标记。联合体存12个具名T*，不内联载荷，不持有第二份分配指针；通用未分类分配保留`raw`成员和`Raw`类别，零载荷为`None`。不使用RTTI或外部依赖。

| 类型 | 联合体成员 |
|---|---|
| BattleActorPayload | battleActor |
| BattleScenePayload | battleScene |
| BattleRideCarrierPayload | battleRideCarrier |
| BattleRingEmitterPayload | battleRingEmitter |
| BattleFollowOffsetPayload | battleFollowOffset |
| BattleTransitionEffectPayload | battleTransitionEffect |
| BattleTextPayload | battleText |
| MapActorPayload | mapActor |
| MapCharaPayload | mapChara |
| MapTransitionPayload | mapTransition |
| WindowOpeningPayload | windowOpening |
| YobiUIData | windowUi |

1. 类别枚举置于现有公共Yobi头，指针联合体和12项前置声明仅在base.hpp；载荷完整定义留在所属功能文件。匿名命名空间中的载荷类型移入同一`cg::login::detail`，常量和其它私有实现保持原作用域。
2. `payloadKind`放在priority／displayPriority之后的本机对齐空隙，不复用业务字段；核对原有全部成员的偏移、大小和构造次序保持。联合体仍为一个指针大小。
3. 通用创建入口增加默认`Raw`的第三参数，保留原两参数调用形式与任意字节数合同。现有12类非零分配明确传入对应类别；零尺寸始终保持None／空raw。分配使用同一calloc、尺寸和失败分支，成功时只激活对应成员，不通过尺寸或回调推断类别。
4. 正常消费者直接访问对应成员，保留原显式const投影和取样位置。无新增运行时类型拒绝、空值保护或业务默认值。
5. 通用释放按类别读取有效成员，统一交原free；禁止写入具名成员后读取非活动raw成员。`CopyYobiAction_41EB10`复制联合体及类别，克隆同时恢复独立战斗指针和类别；嵌套文本／借用引用清理不变。
6. 默认／RawFields构造明确激活空raw并设置None；保留十个CryptInt的构造路径、次数和RNG顺序。

## 范围与验收

仅修改现有Yobi源／头、公共创建声明及本SPEC／进度。全量检查实际payload引用、非零创建参数、通用释放、复制／克隆和12类声明作用域；逐处反向投影与修改前完整源码对照，核原地址注释及非类型化业务正文、字段读写次序和布局算术。

按本轮授权只做源码实施与必要静态验证，不构建、测试、运行客户端或算法、部署、计算摘要；静态断言不记为已编译。不生成／更新JSON台账，不刷新派生定位，不新增依赖，不调整原异常路径。UTF-8文件直接apply_patch；每个可验证小步同步进度并精确提交，既有缓存排除。

## 实施结果

24份源／头文件已完成类型化：20B扩展结构保留16B未知区和S32时长，四处时长读写及完整复制改成员访问；931处载荷转换改为具名指针访问（原const投影保留），31处非零创建明确类别。八个原匿名载荷定义及其MapTransitionSlots18支撑类型归入所属功能的detail作用域，不搬动业务实现或动态初始化。

联合体及类别在通用创建／释放、默认／RawFields构造和复制／克隆成对维护；14类（None、Raw和12种具体载荷）的分支对应关系、12类唯一定义及所有访问点静态检查通过。51份完整源／头迁移稿逐字核对一致，24个非基础CPP的业务token反向投影及基础构造／分配／释放限定差异检查通过；原地址／十六进制常量集合保持。按现有x64类型大小／对齐计算，原有103个成员的偏移和宽度、848B总大小及8B对齐保持，payloadKind只使用原displayPriority之后的对齐空隙。该结果是布局算术核对，尚未实际编译新增断言。

UTF-8无BOM／LF和Git差异检查通过；未构建、测试、运行、部署或计算摘要，未生成／更新JSON台账或派生定位，不提升运行验收。源码、SPEC及两份进度精确提交，既有缓存保留并排除。

## YCA01：克隆字段复制改为赋值运算符

用户继续提出将`CopyYobiAction_41EB10`改成`operator=`，沿本次“做完spec后执行”授权实施。本函数只有`CloneBattleAction_41EAB0`一个调用点；当前复制包含链表链接和拥有载荷的指针，调用方随后恢复新节点链接、独立战斗载荷和类别，再复制载荷内容，不能将其作为不受限的通用浅赋值接口。

1. 在内部`YobiAction`声明私有`YobiAction& operator=(const YobiAction& source) noexcept`，仅将现有克隆函数声明为friend。全部数据成员仍为public，布局、默认／RawFields构造及析构保持。
2. 原41EB10完整复制正文迁入base.cpp的成员实现，严格保留读写顺序：destination改this、source指针改const引用、返回目标引用。十个CryptInt仍调用原同类赋值，三个联合槽与所有浮点仍按原宽度memcpy；payload及类别仍成对复制，不改为整对象字节复制或默认成员赋值。
3. 唯一调用改为`*action = *source;`，原调用前的链接／载荷快照、调用后的恢复及独立载荷浅复制保持；不增加分配、释放、自赋值分支或RNG，不修改其它复制／移动成员和公开克隆接口。
4. 只修改base.cpp／base.hpp／battle_effect.cpp及本SPEC／进度，以完整正文迁移、唯一调用、friend可见性和编码／差异做静态检查。不构建、测试、运行、部署或计算摘要，不生成／更新JSON台账或派生定位；UTF-8直接apply_patch，检查后精确提交。

YCA01已实施：原复制函数完整迁入base.cpp，克隆入口使用私有赋值运算符；三份实际源码与迁移稿全文一致，运算符正文按this／引用逆向投影与原函数逐字一致，克隆正文除调用表达式外逐字保持。十CryptInt赋值、全部浮点／联合槽memcpy、原读写顺序及所有数据成员保持，friend声明与既有公开克隆声明匹配，旧帮助函数在活动源码零残留。六文件UTF-8无BOM／LF和Git差异静态检查通过；未编译、测试或运行，不生成／更新JSON台账，精确提交本步。
