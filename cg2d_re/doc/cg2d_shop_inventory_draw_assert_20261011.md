# 鉴定背包绘制数组断言诊断（2026-10-11）

用户请求分析 `cg2d_assert_20261011_193613_845_25876_43108_1.dmp`；本次只取证和记录，不修改业务源码。

## 结论

鉴定模式的正常库存映射哨兵 `20` 被 `Sub_4BC660` 绘制分支作为商品下标读取，触发 MSVC `std::array<char, 63200>::operator[]` 的范围断言。原端同一指令读取的是商品区后方既有浏览记录的别名字节；当前拆分存储后，尚未在这一消费者恢复正确读取来源。

故障位置为 `src/ui/WindowRuntime.cpp:21190`：

```cpp
if (gShopItemsCF4FE0[mappedItem * 0xC58 + 0xC3C] == 0) {
```

商品区只有 `20 * 0xC58 = 0xF6E0`（63200）字节。本次 `mappedItem = 20`，实际下标 `20 * 0xC58 + 0xC3C = 0x1031C`（66332），超出数组范围。

## 转储证据

转储来源：`H:/Games/cgmsv_21.2a/CrossGate7.1.0/cg2d_assert_20261011_193613_845_25876_43108_1.dmp`，断言线程 43108，主模块基址 `0x7FF7D1360000`。Debug 报告回调以空异常指针生成转储，文件没有 ExceptionStream；不能将等待写转储时的线程 RIP 当成故障指令。

本地 PDB 的 GUID `4D8FC5CF-51B9-46D9-BAB2-9AA19EB17735`、Age 32 与转储主模块 CodeView 完全一致。仅使用已有 DbgHelp 离线解析符号及标准库读取转储，没有启动或附加客户端，也没有计算文件摘要。

| 栈返回地址 RVA | 对应函数与源码 |
| --- | --- |
| `0x115F5D` | `std::array<char,63200>::operator[]`，MSVC `array:531` |
| `0x64688` | `cg::ui::detail::Sub_4BC660`，`WindowRuntime.cpp:21190` |
| `0xB4958` | `cg::ui::detail::Sub_473560`，`WindowRuntime.cpp:44261` |
| `0x30C50` | `cg::ui::UpdateWindows_473F80`，`WindowRuntime.cpp:44488` |
| `0x30E8E` | `cg::ui::RenderWindows_504120`，`WindowRuntime.cpp:44553` |
| `0x238028` | `cg::world::detail::DrawNormalScene_51C7D0`，`SceneState.cpp:6839` |

源码行按返回地址减 1 定位。实际调用栈的 `0x999253DBA8` 保存返回地址 `0x7FF7D13C4688`；`operator[]` 入口的参数保存指令为 `mov [rsp+10h], rdx` 和 `mov [rsp+8], rcx`。相应槽 `0x999253DBB0` 保存 `this = 0x7FF7D18CF310`，与 `gShopItemsCF4FE0` 的符号地址相同；`0x999253DBB8` 保存 `_Pos = 0x1031C`，直接确认实际访问对象和下标。

转储中模式 `gMaterialOccupancyCC4894[20] = 4`，即鉴定；20 个库存映射为：

```text
20 20 20 20 20 20 20 20 20 20 20 20 0 20 20 20 20 20 20 20
```

`gShopValueCF467C = 0`，商品列表绘制数为 20。原别名对应的新端 `gStallBrowseD046D8 + 0xC24` 当前字节为 `0`；这是该转储的值，不作为恒零规则。

## 原端 IDA 证据

MCP 恢复后重新选择当前 `cg2d.exe` 会话，`survey_binary` 确认 x86、基址 `0x400000`，自动分析已完成。只读有界指令查询，未重开、修改或保存数据库。

- `0x4B7AB6` 写模式 `4`；`0x4B7EE6` 写模式 `5`，分别对应鉴定和修理回复生产者。
- `0x4B7E00` 设 `EBX = 20`；`0x4B7E31` 先写映射 `-1`，玩家物品 used 非零时 `0x4B7E3A` 写 `BL = 20`，只有扫描找到真实商品且索引小于 20 才由 `0x4B7E44` 覆盖。因此 `20` 是正常的“有物品但无服务商品匹配”哨兵。修理生产者 `0x4B8261/0x4B826A/0x4B8274` 同义。
- 绘制 `0x4BCD66..0x4BCD7C` 只跳过负映射和 used 为零，没有 `<20` 条件；原 `0x4BCE07` 重新读 S8 映射，`0x4BCE0E..0x4BCE16` 计算步长，`0x4BCE19` 从 `0xCF5C1C + mappedItem * 0xC58` 读取 W8。机器码 `8A 04 CD 1C 5C CF 00` 确认实际读取基址。
- 当映射为 20，原地址为 `0xCF4FE0 + 20 * 0xC58 + 0xC3C = 0xD052FC`。它属于首条浏览物品 `0xD04744 + 0xBB8`，对应既有 `gStallBrowseD046D8.items[0].description[30][72]`；布局和清零/生产/搬移来源见 [窗口53批量与拖放加入 SPEC](ui_windows/window_53.md#evidence-105)。
- 批量预扫 `0x4BAA95` 机器码 `8A 04 D5 1C 5C CF 00` 读取同一基址。当前 `Sub_4BAA50` 已针对正常 `20/-1` 恢复正确来源，此次转储指向另一个尚未调整的消费者 `Sub_4BC660`。

## 最小修复建议与边界

仅在 `Sub_4BC660` 最后绘制分支的 `mappedItem == 20` 时，允许字节改读 `gStallBrowseD046D8.items[0].description[30][72]`；`0..19` 继续读取原商品字段。该分支已有负映射跳过条件，当前证据不需要增加 `-1` 路径。按读取结果保留原 `23040` 标记、玩家图标、数量及先后顺序。

这是恢复已确认读取来源的局部建议，不增加商品容量或新状态，也不改变原映射生产与加入门槛。本次故障已定位，业务修复、编译和用户复现验证均未执行，完整商店及运行验收状态保持。
