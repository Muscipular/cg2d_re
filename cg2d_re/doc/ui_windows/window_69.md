# 窗口69：晶石商店69

[总索引](../cg2d_ui_window_runtime.md) | [文档组织 SPEC](README.md)

> 迁自 `810c6f75` 的完整证据章节，按原取证顺序保留。历史“当前/仍缺/未实现”不是现状；当前结论见[总索引](../cg2d_ui_window_runtime.md)与[唯一进度](../../plan/cg2d_full_reconstruction_progress.md)。仅静态核对，未运行验收。

## 窗口身份

以下来自现有[主原型表](../../src/ui/WindowPrototypes.cpp)，不是新增取证或运行验收。

| 项目 | 已记录值 |
|---|---|
| window id | 69 |
| 原型地址 | `54ECC0` |
| 元素表 / 数量 | `gWindowElements575988` / 70 |
| update | `WindowCrystalShop_Update` |
| draw | `WindowCrystalShop_Draw` |
| release | `WindowCrystalShop_Release` |

当前元素及三主回调已有本体/绑定；这不表示完整上游、输入、显示或实际收发已验收。

两窗口均有70项元素，但元素表、主回调、商品表和部分状态分别独立；正文对比记录的共享计数、选择槽与NPC文本不能拆成私有副本。

## 本窗口明细

| 环节 | 本ID合同 |
|---|---|
| 商品生产 | 4C13B0解析；CB2230表20项、每项C54H，整表清零F690H |
| NPC正文与商品字段 | 字段3..5生产正文行1..12，字段6开始商品；NPC文本仍与关联窗口共用CC24D4 |
| 独立状态 | D67B98/S32、D2EA5B/U8、CB01F0图号/S32、D2D810类型/S32 |
| 限额差异 | remaining!=-1且小于共享索引记录limit时，仅本窗回写该记录limit；不把此分支推广到70 |
| 余额与确认 | 使用同源FF02CC；确认发送512620的命令为0，区别于70的命令1 |
| 元素差异 | 575988共70项；三底图为17F32/17F33/17F34，不与70的独立图形源合并 |

逐字段解析、重复采样、确认失败路径、各回调及原表摘要见下列共享证据中的69分支；此处不复制共享合同。

## 证据导航

本ID的详细合同与关联窗口共同取证，完整正文单一存放于下列共享章节。

## 共享与关联

- [公共窗口运行期与移动父链](shared_runtime.md)
- [对话父路由与BZ回复](shared_dialog_routes.md)
- [对话父路由49D2E0续接证据（2026-09-28）](shared_dialog_routes.md#evidence-066)
- [对话父路由与BZ回复实施 SPEC（2026-09-29）](shared_dialog_routes.md#evidence-132)
- [窗口69/70共享晶石商店合同](shared_69_70.md)
- [窗口69/70晶石商品生产 SPEC（2026-09-29）](shared_69_70.md#evidence-124)
- [晶石商店列表与统计刷新 SPEC（2026-09-29）](shared_69_70.md#evidence-125)
- [晶石商店主回调与商品提示 SPEC（2026-09-29）](shared_69_70.md#evidence-126)
- [晶石商店分页与取消 SPEC（2026-09-29）](shared_69_70.md#evidence-127)
- [晶石商店数量增减 SPEC（2026-09-29）](shared_69_70.md#evidence-128)
- [晶石商店购买确认闭环 SPEC（2026-09-29）](shared_69_70.md#evidence-129)
- [晶石商店背包网格 SPEC（2026-09-29）](shared_69_70.md#evidence-130)
- [晶石商店完整元素原型 SPEC（2026-09-29）](shared_69_70.md#evidence-131)
- [窗口70：晶石商店70](window_70.md)
