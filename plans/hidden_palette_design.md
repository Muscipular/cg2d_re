# 隐藏调色盘设计记录

## IDA 路径

隐藏调色盘不是按 `GraphicInfo.mapNo` 反查 `AnimeInfo`。旧客户端路径如下：

1. `Anime action.pal` 是 palette cache/`queue1` 下标。
2. `queue1[pal]` 取出隐藏调色盘 `mapNo`。
3. `Graphic::getMapNoIndex(mapNo)` 将隐藏调色盘 `mapNo` 转成 Graphic index。
4. `Graphic::readGraphicDataByIndex` 读取该 Graphic 的解码数据。
5. `Graphic::LoadPatelet` 从 `pixelData + width * height` 后复制 palette bytes，最多 768 字节。

已确认地址：

- `0x459960 GraphicInit`：读取 `GraphicInfo.mapNo`，建立 `mapNo -> graphicIndex`。
- `0x45B760 AnimeLoadAction`：读取 `action.pal`。
- `0x517DA0 Yobi::PlayFrame`：播放动画时把 `action.pal` 放入 palette cache index。
- `0x523140 Graphic::LoadPatelet` / `0x522CB0`：按 pal 查 `queue1`，再按 `queue1[pal]` 的 `mapNo` 找 Graphic，读取尾部 palette bytes。
- `0x522B30` / `0x522900`：构建 `queue1`，来源包含 IDA 静态表和 `CGLoadSpr.dbf`。

## 字段语义

- `AnimeActionHeader::pal`：`queue1` 下标，不是 `mapNo`、`seqNo` 或 `globalIndex`。
- `queue1` 值：隐藏调色盘 Graphic 的 `mapNo`。
- `GraphicInfo::mapNo`：旧客户端资源编号，用于 `Graphic::getMapNoIndex(mapNo)`。
- `GraphicInfo::seqNo`：同一 binSet 内 Graphic 序号，用于动画帧按 `graphicSeqNo` 找帧图。
- `GraphicRef::globalIndex`：当前实现为了跨 binSet 定位帧 Graphic 的派生索引，不参与隐藏调色盘查询。

## 当前实现边界

- Resource 层读取可选 `CGLoadSpr.dbf`，路径由工具按 `cgmsv.yml` 根目录推导为 `data/CGLoadSpr.dbf`。
- Resource 层公开 `LoadHiddenPaletteByMapNo(mapNo)`，只通过 `mapNo -> GraphicRef` 读取该 Graphic 尾部内嵌 palette；`LoadHiddenPaletteByPalIndex(pal)` 仅负责先查 `queue1` 映射再复用该入口。
- `CGLoadSpr.dbf` 是 `DBF\0` 头 + LZO1X 压缩数据；解压后为 `[count][recordSize][key,mapNo]...`。
- IDA 静态 `queue1` 表内容暂未内置；仅使用 IDA 确认的静态表长度 1752 来定位 `CGLoadSpr.dbf` 的追加区。因此当前可解析的 pal 范围是 `1752..1752+CGLoadSpr记录数-1`。
- 缺少 `CGLoadSpr.dbf`、pal 未命中、隐藏 Graphic 缺失或 palette bytes 无效时，anim_viewer 保持默认 palette，并在输出或 warning 摘要中说明 fallback。
- 读取隐藏 palette 时只读取隐藏 palette Graphic 解码数据中 `pixelData + width * height` 后的内嵌 palette bytes，不混用 `mapNo`、`seqNo`、`globalIndex`。
