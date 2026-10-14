# 窗口36：通用对话共享实例

[总索引](../cg2d_ui_window_runtime.md) | [文档组织 SPEC](README.md)

> 迁自 `810c6f75` 的完整证据章节，按原取证顺序保留。历史“当前/仍缺/未实现”不是现状；当前结论见[总索引](../cg2d_ui_window_runtime.md)与[唯一进度](../../plan/cg2d_full_reconstruction_progress.md)。仅静态核对，未运行验收。

## 窗口身份

以下来自现有[主原型表](../../src/ui/WindowPrototypes.cpp)，不是新增取证或运行验收。

| 项目 | 已记录值 |
|---|---|
| window id | 36 |
| 原型地址 | `54E870` |
| 元素表 / 数量 | `gWindowElements56CA88` / 19 |
| update | `WindowDialog_Update` |
| draw | `WindowDialog_Draw` |
| release | `WindowDialog_Release` |

当前元素及三主回调已有本体/绑定；这不表示完整上游、输入、显示或实际收发已验收。

36/37两槽共享54E870和同一19项元素表，不能按两个ID重复计为两组完成。49C600创建37并处理36/37旧实例；协议类型37与window id 37不是同一编号体系，详见对应生产章节。

## 证据导航

本ID的详细合同与关联窗口共同取证，完整正文单一存放于下列共享章节。

## 共享与关联

- [公共窗口运行期与移动父链](shared_runtime.md)
- [对话父路由与BZ回复](shared_dialog_routes.md)
- [对话父路由49D2E0续接证据（2026-09-28）](shared_dialog_routes.md#evidence-066)
- [对话父路由与BZ回复实施 SPEC（2026-09-29）](shared_dialog_routes.md#evidence-132)
- [窗口36/37共享原型与运行合同](shared_36_37.md)
- [通用对话创建与内嵌图形 SPEC（2026-09-28）](shared_36_37.md#evidence-081)
- [通用对话三个主回调 SPEC（2026-09-28）](shared_36_37.md#evidence-082)
- [通用对话鼠标交互 SPEC（2026-09-28）](shared_36_37.md#evidence-083)
- [通用对话运行原型19项定案（2026-09-28）](shared_36_37.md#evidence-084)
- [窗口37：通用对话创建入口](window_37.md)
- [窗口39：计时提示](window_39.md)
