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
| PetRuntimeRecord原名/自定义名 | 内部业务文本；宠物及摊位回复、UI、交易、Lua | 已宽化；ClearRaw全部逐字段清理，原加密值重置保持 |
| 玩家自身名称/称号/附文、TitleRecord、队伍槽名称 | 内部业务文本；回复生产、人物UI、窗口标题、地图身份 | 已宽化；称号对象复制/临时引用排序、玩家字段清理已迁移；场景重建临时转义串待收口 |
| ReplyRecordF7C950 | 内部物品属性文本；TgX回复与属性提示 | 已宽化；原字段容量/失败分支/居中公式保持 |
| YobiAction::displayName及对象构造 | 内部绘制文本；克隆、RenderChara消费 | 已宽化；删除覆盖对象的memset，普通字段成员初始化 |
| WorldObjectRecord、Yobi名称/头顶文字 | 内部业务文本；世界及战斗回复、骑乘复制、图鉴匹配、黑名单、UI及Lua | 已宽化；玩家初始身份仍待玩家名称组统一入口 |
| 战斗命名效果、战斗文字行 | 内部名称/显示文本；C/M解析、战斗匹配及面板绘制 | 已宽化；原协议缓冲区保留字节 |
| 技能/物品目标名称及属性名单、技能结果名称、窗口45/46回调 | 内部展示文本与借用接口 | 已宽化；源玩家/宠物临时解码待所属组收口 |
| 摊位卖家名/摊位名/描述 | 内部展示文本；回复生产及浏览绘制 | 已宽化；保留入口编码长度截断 |
| 聊天气泡 | 拥有的文本行已为wstring，入口仍接协议GBK文本 | 待合并聊天入口宽化；姓名偏移暂按编码字节数计算 |
| MailState普通/迷你联系人 | 内部姓名/称号/附文；回复、列表、收件人及排序 | 已宽化；普通、迷你及公会联系人统一使用宽文本 |
| 公会资料/职级/成员及编辑草稿 | 内部业务文本；回复、排序、邮件、资料窗口 | 已宽化；旧成员文本区拆为具名字段，草稿偏移访问改为类型访问，无效职级/成员索引已保护 |
| MailState三类邮件日期/正文 | 内部文本与固定文件记录；插入/阅读/普通和公会存档 | 已宽化；磁盘记录独立，按用户确认仅保存日期时安全截断到17个GBK字节 |
| MailState个人资料/类别及迷你邮件收件人 | 内部业务文本与固定文件记录；编辑/搜索/查看/发送 | 已宽化；ProfileDiskRecord/CategoryDiskStorage保留旧GBK格式及短读字段发布，文件路径已宽化 |
| BattleState、场景其它显示文本 | 业务显示文本或尚未解析的网络指令 | 战斗结果名称已宽化；C/M队列保留原始协议字节，其余待核对 |
| 公告板/治疗名单/交易目标与对方姓名/宠物技能学习选项 | 内部展示文本；协议回复生产、显示/选择回调消费 | 已宽化；记录对象清理和选择回调同步迁移，协议解析局部字节保留 |
| 其余UI输入辅助及自身摆摊文本 | 内部文本；输入和登录存档生产 | 待随登录存储组迁移 |
| 资源路径、调色板路径、登录存档路径、Lua文件路径、音频路径 | 内部路径文本 | 待迁移；path不属于保留窄文本的文件边界 |
| 业务日志、格式化、比较与剩余窄文本 | 内部文本或外部API临时适配 | 待最终逐项复核 |
| 网络原始封包、文件原始字节、压缩/像素/数值状态 | 边界格式或非文本二进制 | 保留字节类型；仍需在最终残留核对中逐项注明实际用途 |

涉及wstring对象的memset/memcpy/calloc/realloc/fread/fwrite必须随所属组一起处理；不得以结构名含Record或历史偏移固定为由跳过。
