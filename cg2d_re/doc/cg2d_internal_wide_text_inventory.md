# IWT01 内部文本迁移清单

此清单按实际字段读写链登记，不以含char的文件数作为完成率。所有当前批次仅完成静态核对，统一编译等待全部迁移完成。

| 对象/接口 | 分类与读写链 | 当前状态 |
| --- | --- | --- |
| UIControl::name、TextData::text、TextInputData::lastText | 内部文本；LuaUI创建/属性读写、TextInputControl回调边界 | 已宽化；Lua的GBK字节容量保留 |
| PngImageState::path、CanvasShape::path | 内部路径文本；Lua属性输入、PNG宽路径、画布宽字符命令解析 | 已宽化 |
| FormatMapDataPath、MapEx文件名、BigMap缓存路径 | 内部文件名；SceneState地图读写、MapEx、BigMap | 已宽化，文件内容保持 |
| gDataPath、gMapPath | 内部配置路径；命令行解析后解码，图鉴/聊天/GraphicRead消费 | 已宽化；命令行整体迁移待后续 |
| AlbumRecord | 内部名称/说明；回复解析、列表/详情、相册存取 | 已宽化；仅AlbumDiskRecord/AlbumDiskStorage保留固定文件布局 |
| ChatHistory621FF8、Record5FB7F8 | 内部输入历史/显示文本；Input生产、WindowChat消费、历史文件存取 | 已宽化；ChatHistoryDisk仅为文件边界，聊天气泡待迁移 |
| 聊天日志路径/文件名 | 内部路径；ChatHistory写日志、WindowAdventureLog读取 | 已宽化；日志文件内容仍为GBK |
| SkillRuntimeRecord/SkillTechniqueRecord/SkillRecipeRecord/SkillRecipeItem、职业名 | 内部文本；SceneState回复生产、技能/制造/战斗/UI/Lua消费 | 已宽化；重置改为对象赋值，负索引不再跨字段读取 |
| PetTechniqueRecord | 内部名称/说明；宠物/摆摊/交易/银行回复生产，技能/UI/Lua/自动战斗消费 | 已宽化；嵌入记录复制与重置已迁移 |
| WindowStorageItem/Pet/PetSummary、WindowTradeItem/Pet | 内部展示文本；银行/交易回复或玩家记录生产，窗口/提示消费 | 已宽化；尚未宽化的原宠物名称临时读取链待合并 |
| WindowShopItem/Detail、WindowCrystalItem、WindowConfirmationItem | 内部商品名称/描述/附文；三商店回复生产、提示/确认消费 | 已宽化；旧gbkLines/stride绘制重载已移除 |
| 玩家自身名称/称号、TitleRecord、ReplyRecordF7C950、宠物自身名称 | 内部业务文本 | 待迁移完整生产、查询、UI/网络发送链 |
| YobiAction::displayName及对象构造 | 内部绘制文本；克隆、RenderChara消费 | 已宽化；删除覆盖对象的memset，普通字段成员初始化 |
| WorldObjectState、Yobi名称/头顶文字、聊天气泡 | 内部业务文本 | 待迁移其余写入、查询 |
| MailState联系人/邮件 | 内部业务文本与固定文件记录 | 待拆分文件记录并迁移内部存储 |
| BattleState、场景其它显示文本 | 业务显示文本或尚未解析的网络指令 | 待逐字段分类并迁移 |
| 其余UI目标名单/学习技能选项/输入辅助 | 内部文本；回复和Lua生产 | 待迁移 |
| 资源路径、调色板路径、登录存档路径、Lua文件路径、音频路径 | 内部路径文本 | 待迁移；path不属于保留窄文本的文件边界 |
| 业务日志、格式化、比较与剩余窄文本 | 内部文本或外部API临时适配 | 待最终逐项复核 |
| 网络原始封包、文件原始字节、压缩/像素/数值状态 | 边界格式或非文本二进制 | 保留字节类型；仍需在最终残留核对中逐项注明实际用途 |

涉及wstring对象的memset/memcpy/calloc/realloc/fread/fwrite必须随所属组一起处理；不得以结构名含Record或历史偏移固定为由跳过。
