# 旧分隔符文本扫描单BYTE取样子SPEC（BSX01）

2026-10-07。沿创建编辑页的真实45E770依赖推进，按AGENTS第8条工程委托先裁决再实施。源码只调整单字节取样数据流，不改姓名业务或现编码／协议规则。

<a id="legacy-separator-byte-baseline"></a>
## 原端基线与实际缺口

原45E770完整[45E770,45E7A9)，25条57B／一页，无调用、八跳转／两RET。十个实际CALL在七个父：4631D0@4638EA；482270@482927／482AB7；49DA30@49DB10／49DB2C；4AFB10@4AFB71；4AFC00@4AFC61；4AFCF0@4AFD1E；4F2130@4F22A5／4F23ED。不将其它调用父外推为全验。

45E776读首BYTE到AL，45E778用AL判NUL；同AL在45E785 unsigned比较81、45E790／45E794比较7C／5C，尾字节状态先判定并消费后清零。45E798先读[edx+1]，45E79B再递增指针，同样本在45E79C判NUL及下轮分类。当前for门禁读取*cursor，循环体又读取*cursor；恢复每个当前位置一次U8样本，保持下一BYTE取样先于指针递增。

原空串、末lead后直接遇NUL都返回完整S32 0；非尾7C或5C立即完整1，两字节尾部不检查这两个分隔符。当前普通值正确，本步恢复读取数据流，不称运行故障、不新增严格解码／半单元拒绝／限长。

本轮另沿选择／编辑／输入初始化实际依赖取证，姓名常量55C9BC=20 00、55CE30=81 40 00已逐字节证实，源码现正确；不能根据字符外观或编码猜测将81 40统一成A1 40。选择552条1992B、编辑1146条4285B、初始化52条196B和矩形17条36B均保源，不为缺文档或取证数量造源码修改／新F／整体验收身份。

X:0005E7A9-0005E7B0七字节全90逐BYTE真实NOP，无入向，前RET后45E7B0真入口，只原对齐用途。

<a id="legacy-separator-byte-decision"></a>
## 实施裁决与保持项

单源一精确片段／一既有原语：初始化current U8一次；while(current!=0)中复用同BYTE，末尾取cursor[1]再++cursor，保局部trailingByte状态及全部0／1返回。const char*仍调用期非拥有，源文本不写，不新增owner／缓存／句柄／全局／辅助函数或外依赖／RTTI。

保原0x81 unsigned比较、尾单元跳过和两分隔符、NUL结束／末lead行为、接口S32及全部其它Input源和所有消费者。Root只覆盖自身数据流和人物实际接线；INPUT与CHARACTER各登记真实成员与局部I-S，所有其它调用父／输出包或输入链不由此关闭。

RPLAN.24不专项模拟或处理异常／越界／溢出／回绕，不加保护／补零／限制／新字符分类。网络原格式只明文，音频暂停、既定排除与执行限制保持。

<a id="legacy-separator-byte-acceptance"></a>
## 可度量验收

1. HEAD整个Input.cpp以一唯一from→to构expected逐字相等；首BYTE和下一BYTE各一次，NUL与分类复用同样本、下一采样先于指针递增，不改类别／返回或源内容。
2. 原25条57B／一页连续字节、无call／八jump／两RET及十实际CALL／七父、单7B用途归档；两姓名常量只证保持项，不伪报修复。
3. 新1602对应F:0005E770／单adapted，INPUT26→27、CHARACTER17→18；语义1602→1603／有效1595→1596、文档1713→1714／3680（46.58%）、处置737→738、E858→862（两域各I-S）；R7 308→309／剩2752；每项15实际来源。
4. U11／V03／R04只current／引用仍open；P05／P08原closed、closed11／完整叶0／globalfalse及全部完整门禁、冻结定位／pending18／差队列保持。四集中静态、measure／-Check同句柄终态、完整派生快照及13文件UTF-8无BOM-LF／四锚点／真实引用／差异通过后记实际结果。

完整所有父、姓名／输入／输出数据生产消费、全部正常alias-owner寿命、CRT-SDK／机器ABI／链接、人物创建-主地图／原格式明文往返／画面／运行R8未验；局部不代整体完成，不构建／测试／运行或计算摘要。

<a id="legacy-separator-byte-source-state"></a>
## 实施追踪

预定Git命名快照：cg2d-re-legacy-separator-byte-sampling-20261007。对应[原证据](cg2d_legacy_separator_byte_sampling_evidence.json)、[输入合同](cg2d_input_context.md)、[人物场景](cg2d_character_select_scene.md)、[唯一进度](../plan/cg2d_full_reconstruction_progress.md)。集中静态与派生终态后追加结果。

实际结果（2026-10-07）：Input.cpp单片段／一原语已实施，当前位置U8供NUL／分类、下一BYTE先取再增指针；原81规则／尾字节／7C和5C／完整0-1、源文本及全部其它源保持。25条57B／一页、十CALL七父、人物实际调用与原2000／814000姓名常量、7B用途证据落地；其它已取证选择／编辑／初始化／矩形保源，不伪报业务修复或原父全验。四集中静态、measure与-Check同句柄终态0、完整派生快照、13文件编码／四锚点／四局部I-S各15实来源引用与差异通过。新1602单F／adapted、INPUT27／CHARACTER18真实成员、单用途与两域I-S落实；语义1603有效1596／文档1714（46.58%）／738处置／E862／R7 309余2752。U11／V03／R04仍open，P05／P08原closed及全部完整门禁保持；完整输入／全部父／消费／正常alias-owner寿命、人物创建-主地图／链接／明文互通／画面／运行与整体验收未达成。
