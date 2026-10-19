# IWT01 内部文本迁移清单

此清单按实际字段读写链登记，不以含char的文件数作为完成率。各组生产—存储—消费及生命周期已完成静态核对，2026-10-20通过build.bat Debug统一编译及链接；未启动客户端。

| 对象/接口 | 分类与读写链 | 当前状态 |
| --- | --- | --- |
| UIControl::name、TextData::text、TextInputData::lastText | 内部文本；LuaUI创建/属性读写、TextInputControl回调边界 | 已宽化；Lua的GBK字节容量保留 |
| PngImageState::path、CanvasShape::path | 内部路径文本；Lua属性输入、PNG宽路径、画布宽字符命令解析 | 已宽化 |
| FormatMapDataPath、MapEx文件名、BigMap缓存路径 | 内部文件名；SceneState地图读写、MapEx、BigMap | 已宽化，文件内容保持 |
| gDataPath、gMapPath | 内部配置路径；命令行解析后解码，图鉴/聊天/GraphicRead消费 | 已宽化；命令行整体已迁移为宽文本 |
| AlbumRecord | 内部名称/说明；回复解析、列表/详情、相册存取 | 已宽化；仅AlbumDiskRecord/AlbumDiskStorage保留固定文件布局 |
| ChatHistory621FF8、Record5FB7F8 | 内部输入历史/显示文本；Input生产、WindowChat消费、历史文件存取 | 已宽化；ChatHistoryDisk仅为文件边界，聊天气泡亦已迁移 |
| 聊天日志路径/文件名 | 内部路径；ChatHistory写日志、WindowAdventureLog读取 | 已宽化；日志文件内容仍为GBK |
| SkillRuntimeRecord/SkillTechniqueRecord/SkillRecipeRecord/SkillRecipeItem、职业名 | 内部文本；SceneState回复生产、技能/制造/战斗/UI/Lua消费 | 已宽化；重置改为对象赋值，负索引不再跨字段读取 |
| PetTechniqueRecord | 内部名称/说明；宠物/摆摊/交易/银行回复生产，技能/UI/Lua/自动战斗消费 | 已宽化；嵌入记录复制与重置已迁移 |
| WindowStorageItem/Pet/PetSummary、WindowTradeItem/Pet | 内部展示文本；银行/交易回复或玩家记录生产，窗口/提示消费 | 已宽化；源宠物名称及其消费者已统一 |
| WindowShopItem/Detail、WindowCrystalItem、WindowConfirmationItem | 内部商品名称/描述/附文；三商店回复生产、提示/确认消费 | 已宽化；旧gbkLines/stride绘制重载已移除 |
| PetRuntimeRecord原名/自定义名 | 内部业务文本；宠物及摊位回复、UI、交易、Lua | 已宽化；ClearRaw全部逐字段清理，原加密值重置保持 |
| 玩家自身名称/称号/附文、TitleRecord、队伍槽名称 | 内部业务文本；回复生产、人物UI、窗口标题、地图身份 | 已宽化；称号对象复制/临时引用排序、玩家字段清理已迁移；场景重建直接传四段宽文本 |
| ReplyRecordF7C950 | 内部物品属性文本；TgX回复与属性提示 | 已宽化；原字段容量/失败分支/居中公式保持 |
| YobiAction::displayName及对象构造 | 内部绘制文本；克隆、RenderChara消费 | 已宽化；删除覆盖对象的memset，普通字段成员初始化 |
| WorldObjectRecord、Yobi名称/头顶文字 | 内部业务文本；世界及战斗回复、骑乘复制、图鉴匹配、黑名单、UI及Lua | 已宽化；玩家身份入口已统一为宽文本 |
| 战斗命名效果、战斗文字行 | 内部名称/显示文本；C/M解析、战斗匹配及面板绘制 | 已宽化；原协议缓冲区保留字节 |
| 技能/物品目标名称及属性名单、技能结果名称、窗口45/46回调 | 内部展示文本与借用接口 | 已宽化；源玩家/宠物亦已统一 |
| 摊位卖家名/摊位名/描述 | 内部展示文本；回复生产及浏览绘制 | 已宽化；保留入口编码长度截断 |
| 聊天气泡 | 协议解码后姓名、前缀过滤、正文分割及显示 | 已宽化；字体保持，后续原字节长度别名仍按ToGbk计算 |
| MailState普通/迷你联系人 | 内部姓名/称号/附文；回复、列表、收件人及排序 | 已宽化；普通、迷你及公会联系人统一使用宽文本 |
| 公会资料/职级/成员及编辑草稿 | 内部业务文本；回复、排序、邮件、资料窗口 | 已宽化；旧成员文本区拆为具名字段，草稿偏移访问改为类型访问，无效职级/成员索引已保护 |
| MailState三类邮件日期/正文 | 内部文本与固定文件记录；插入/阅读/普通和公会存档 | 已宽化；磁盘记录独立，按用户确认仅保存日期时安全截断到17个GBK字节 |
| MailState个人资料/类别及迷你邮件收件人 | 内部业务文本与固定文件记录；编辑/搜索/查看/发送 | 已宽化；ProfileDiskRecord/CategoryDiskStorage保留旧GBK格式及短读字段发布，文件路径已宽化 |
| BattleState、场景其它显示文本 | 业务显示文本或尚未解析的网络指令 | 战斗结果名称已宽化；C/M队列及TakeBattle字符串为原协议解析边界 |
| 登录角色列表与创建/选择/删除名称 | 内部名称/职业；网络回复、角色选择与创建请求 | 已宽化；名称GBK容量与禁止字节检查留在提交边界，记录重置为对象赋值 |
| 公告板/治疗名单/交易目标与对方姓名/宠物技能学习选项 | 内部展示文本；协议回复生产、显示/选择回调消费 | 已宽化；记录对象清理和选择回调同步迁移，协议解析局部字节保留 |
| 登录输入/账号密码、快捷聊天存档、自身摆摊文本 | 内部文本；输入、自动登录、存档及摆摊请求 | 已宽化；SavedDiskRecord单独保留510H布局，摊位名保存按用户确认截到15个GBK字节，密码不写磁盘 |
| BinLoader六类资源路径、Bin目录、调色板路径 | 内部路径文本；YAML/命令行输入、资源与音频/大地图消费 | 已宽化，YAML的UTF-8路径在读入边界解码，文件打开使用_wfopen |
| 登录存档目录及黑名单名称/路径 | 内部路径与业务文本；文件与窗口消费 | 已宽化；登录保存打开/删除使用宽接口，黑名单仅在文件读写时转换GBK |
| 启动命令行、服务器地址/端口文本 | 内部配置文本；wWinMain/命令解析、自动登录与连接 | 已宽化；IPv4/DNS及校验字节仅在外部接口边界转换 |
| Lua文件路径、寻路地图、截图、cut/aut、模块资源与摘要、网络日志路径 | 内部路径文本 | 已宽化；Lua/YAML/SDL等只接受窄字符的外部接口保留调用边界字节，系统日志路径与入口已宽化 |
| 业务日志、渲染错误、战斗时间、MAC标识与Lua聊天入口 | 内部文本或外部API临时适配 | 已宽化；SDL错误在入口解码UTF-8，Lua/异常诊断的窄字符串仅在日志外部适配处使用，外观/喂食/交易参数格式化已统一宽文本 |
| 网络原始封包、文件原始字节、压缩/像素/数值状态 | 边界格式或非文本二进制 | 保留字节类型，具体用途见下表 |

涉及wstring对象的memset/memcpy/calloc/realloc/fread/fwrite必须随所属组一起处理；不得以结构名含Record或历史偏移固定为由跳过。

## 最终保留的窄字节类别（2026-10-20）

| 位置/类型 | 保留原因 |
| --- | --- |
| NetworkRuntime原始队列、协议字典、Extract/Parse/转义工具、Receive/Handle回复参数 | 未解析协议及逐字段解码边界，编码长度按原字节数 |
| BattleState的C/M队列、battle_message的TakeBattle/String/Header及preview/ignored局部缓冲 | 含控制命令的原始战斗协议；业务名称和显示行解码后为wstring |
| SceneState的gMoveDirectionText、WindowBattlePets/Items/Command及WindowInventory的packet | 已编码方向/战斗动作/扩展命令载荷，不是显示文本 |
| AlbumDisk、ChatHistoryDisk、MailDisk、ProfileDisk、CategoryDisk、SavedDisk | 旧文件布局专用记录；仅边界读写，运行对象没有GBK影子副本 |
| RendererResources的update.dat正文、BigMapSurface的stamp/identity、MapEx的magic | 文件正文、缓存二进制标识或魔数；不是路径或用户文本 |
| WindowBigMap的line/fields、BinLoader/Font的YAML scalar、LuaIO的文件/YAML数据 | 文件读取/外部解析器边界；载入业务字段后解码 |
| LuaData/LuaUI的字段键、函数名、元表名和Lua tostring局部缓冲 | Lua C API标识及返回值适配，遵守现有外部GBK合同 |
| LuaBridgeValue/stringValue、header、packet回调快照 | 通用Lua值及原始封包跨回调传递，保持Lua二进制字符串和显式长度，不能按显示文字解码 |
| platformTool的DNS字符串及摘要字节、SDL driver、系统异常what()/Lua错误 | 窄字符外部API或编码适配边界；自有诊断格式及SDL错误缓存已宽化 |
| UI回复局部name/description/line及容量预检fileBytes、Encoding/Input的GBK临时串 | 原协议/存档容量与分行边界，之后立即解码；不作为长期业务状态 |
| gShopSelectedSlots、char font/hover、像素/压缩及数值标志 | 数值或二进制数据，非文本 |
| src/tools中的历史MIDI工具 | 按现行项目规则排除，不进入当前客户端及本次迁移 |

生命周期复核：Yobi对象在分配后construct_at、释放前destroy_at；文字行使用wstring数组new[]/delete[]。玩家、宠物、技能、邮件、标题、公会和窗口运行对象采用对象赋值/逐字段清理；文件IO只作用于独立磁盘记录。仍存在的memcpy/memset仅用于数值、指针联合体、协议/文件字节及像素等平凡对象。
