# 世界渲染遮挡排序方案

> 状态：探索性草稿。深度轴、比较器性质、地图物件占地来源和旧客户端行为尚未验证，禁止直接据此实施。

## 目标

解决旋转斜视角下角色、宠物、地图物块之间的前后遮挡不稳定问题。

本方案不依赖旧客户端规则，使用统一的游戏坐标 `ox, oy` 建立遮挡排序。

## 坐标约定

所有参与遮挡排序的对象先转换到游戏坐标：

```text
GameCoord:
    ox
    oy
```

`ox, oy` 是游戏逻辑平面坐标，不是屏幕坐标。

屏幕坐标只用于最终绘制：

```text
screenX = (ox + oy) * tileHalfWidth
screenY = (oy - ox) * tileHalfHeight
```

遮挡排序不直接使用屏幕 `x/y` 作为主排序依据。

## 占地模型

每个可遮挡对象定义整数占地，使用半开区间：

```text
Footprint:
    minOx
    minOy
    maxOx   // 不包含
    maxOy   // 不包含
```

示例：

```text
1x1 at ox,oy:
    minOx = ox
    minOy = oy
    maxOx = ox + 1
    maxOy = oy + 1

2x2 at ox,oy:
    minOx = ox
    minOy = oy
    maxOx = ox + 2
    maxOy = oy + 2

3x1 at ox,oy:
    minOx = ox
    minOy = oy
    maxOx = ox + 3
    maxOy = oy + 1
```

角色、宠物按脚底所在格处理：

```text
Entity footprint:
    minOx = footOx
    minOy = footOy
    maxOx = footOx + 1
    maxOy = footOy + 1
```

## 深度计算

每个对象取占地的前沿点：

```text
frontOx = maxOx - 1
frontOy = maxOy - 1
```

计算排序值：

```text
depth = frontOx + frontOy
side  = frontOy - frontOx
```

含义：

```text
depth 小：靠后，先画
depth 大：靠前，后画
```

## 基础排序规则

同一遮挡层内按以下顺序排序：

```text
layer -> depth -> side -> order
```

伪代码：

```text
function sortKey(item):
    frontOx = item.footprint.maxOx - 1
    frontOy = item.footprint.maxOy - 1

    depth = frontOx + frontOy
    side = frontOy - frontOx

    return (
        item.layer,
        depth,
        side,
        item.order
    )

items.sortBy(sortKey)
```

绘制顺序：

```text
排序靠前的对象先画
排序靠后的对象后画
后画对象遮挡先画对象
```

## 分层规则

参与互相遮挡的对象必须放在同一个遮挡层：

```text
GroundLayer:
    地板

WorldDepthLayer:
    角色
    宠物
    可遮挡地图物块

TopObjectLayer:
    永远压在角色前方的顶部物件

UiLayer:
    UI
```

如果物块和角色不在同一个 `WorldDepthLayer`，`depth` 排序不会生效。

## 明确前后关系增强

基础排序使用 `depth / side / order`。如果后续需要处理大物块和实体边界冲突，可以增加明确前后关系判断：

```text
function isBehind(a, b):
    return a.footprint.maxOx <= b.footprint.minOx
        or a.footprint.maxOy <= b.footprint.minOy
```

排序比较伪代码：

```text
function compare(a, b):
    if a.layer != b.layer:
        return a.layer < b.layer

    if isBehind(a, b):
        return a before b

    if isBehind(b, a):
        return b before a

    return sortKey(a) < sortKey(b)
```

第一版建议先实现基础排序：

```text
layer -> depth -> side -> order
```

明确前后关系判断作为第二阶段增强。

## 大物块处理

`2x2`、`3x1`、`1x3` 等大物块第一版可以整体作为一个对象排序。

如果某个大物块内部同时包含后景部分和前景遮挡部分，整体排序无法解决局部遮挡，需要拆成多个绘制片段：

```text
3x1:
    slice 0: 1x1
    slice 1: 1x1
    slice 2: 1x1

2x2:
    slice 0: 1x1
    slice 1: 1x1
    slice 2: 1x1
    slice 3: 1x1
```

每个 `slice` 独立拥有自己的 `Footprint` 和排序值。

## 草稿结论

第一版候选规则：

```text
1. 所有参与遮挡的对象转换到游戏坐标 ox, oy。
2. 每个对象生成整数半开占地 Footprint。
3. 角色、宠物使用脚底格作为 1x1 Footprint。
4. 地图物块使用实际占地尺寸生成 Footprint。
5. 参与互相遮挡的物块和实体放入同一个 WorldDepthLayer。
6. 排序使用 layer -> depth -> side -> order。
7. depth = (maxOx - 1) + (maxOy - 1)。
8. side = (maxOy - 1) - (maxOx - 1)。
9. 排序靠前先画，排序靠后后画。
10. 局部遮挡错误的大物块后续拆 slice。
```
