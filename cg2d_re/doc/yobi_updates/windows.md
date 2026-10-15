# Yobi Update：窗口与模态UI

[分类总表与口径](../cg2d_yobi_update_coverage.md) · [唯一执行进度](../../plan/cg2d_full_reconstruction_progress.md)

## 登记边界

本表登记当前源码中的 8 个不同原地址回调。各行本体存在、所列直接或参数绑定已在源码出现；仅静态登记，不重验历史语义，不表示全部原端绑定已枚举、整链已接或运行通过。源码行号固定于本次登记快照，地址/符号为稳定身份；后续变动以符号重新定位。

[源码](../../src/Yobi/)；“绑定点”中的行号指赋值行，参数传入另写初始化路径。最终通用回收沿总表的公共生命周期，表内列局部退出责任。

## 回调明细

| 原地址/本体 | 创建与绑定点 | 载荷与生产者 | 阶段/依赖 | 退出、剩余与证据 |
|---|---|---|---|---|
| `4758D0` / `Sub_4758D0` | `Sub_475950`:1792 | WindowOpeningPayload；475950生产，completionSlot借用 | 展开完成调用OpenWindow_471440；window id取实参 | 完成清回写槽并标死；窗口公共生命周期见共享明细；[详细证据](../ui_windows/shared_runtime.md) |
| `48ABC0` / `Sub_48ABC0` | `Sub_48AC60`:1894 | WindowOpeningPayload；48AC60生产，数量回调另存2C联合槽 | 数量展开后48A830；不是窗口42主回调本身 | 完成回写/标死；其他打开caller仍待；[详细证据](../ui_windows/window_42.md) |
| `4ABE60` / `Sub_4ABE60` | `Sub_4ABEF0`:1991 | WindowOpeningPayload；4ABEF0生产 | 存放展开后4A83A0 | 完成回写/标死；窗口38独立证据；[详细证据](../ui_windows/window_38.md) |
| `4ACDF0` / `Sub_4ACDF0` | `Sub_4ACE80`:2085 | WindowOpeningPayload；4ACE80生产 | 存放展开后4AC0C0 | 完成回写/标死；窗口41独立证据；[详细证据](../ui_windows/window_41.md) |
| `5046D0` / `UpdateModalAction_5046D0` | `Sub_5044E0`:23607 | YobiUIData；5044E0生产；winId是样式参数，不等同71槽窗口ID | modal整块/九宫格展开及绘制 | 本体不自行MarkDeath；调用方/公共回收责任见原modal合同；[详细证据](../cg2d_yobi_runtime.md) |

### 旧面板预览增量（2026-10-04状态核对）

以下3个目标/4个直接赋值已存在于 `YobiRuntime.cpp`，此前分类漏记。lambda仅丢弃原回调返回值，以原目标去重；不计成4个新业务回调。

| 原目标 | 创建/绑定符号 | 载荷、更新与剩余 |
|---|---|---|
| `4D6690` | `Sub_4D6640`、`Sub_4F9FF0`各一处 | payload=0的旧面板预览；按动作状态调用517880；两个creator仍分别核借用/清理 |
| `4EBFD0` | `Sub_4EBF70` | payload=0，沿原循环门禁调用517880；旧页面完整往返未验 |
| `4FACE0` | `Sub_4FAAA0` | 旧面板预览，原图号映射/更新合同保留；场景关闭与清理仍待整链验收 |

详见[旧面板公共合同](../ui_windows/shared_legacy_panels.md)，完整owner/退出不能由无payload推出自动完成。

## 生命周期任务

本类只登记 `YobiAction.callback`，不把71槽窗口的创建/更新/绘制表、元素回调或数值回调算成同一种回调。窗口明细继续按window id维护；`4758D0` 动态开窗在公共文件记录，各窗口只引用它。`5044E0` 的winId控制modal样式，不得机械归为对应window id。后续逐条核开窗、完成回写、窗口关闭、展开对象及静态预览的清理责任；未列的窗口仍按总窗口缺口推进。
