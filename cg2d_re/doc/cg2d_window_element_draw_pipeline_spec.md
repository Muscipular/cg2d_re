# 窗口元素绘制完整分发 SPEC（WDP01）

2026-10-10。沿CLD01核通用绘制父，依子AGENTS第8条先记原事实、取舍和实施边界。同一cg2d_re分支、不打tag；现GBK、原格式明文、MSVC CRT、WAV-only和SDL渲染决议保持。

<a id="window-draw-baseline"></a>
## 完整原根与旧分发表

4729B0–472CFE（尾不含）完整277条846B/五页、15实际E8/13目标、30跳一RET；51普通CALL入向来自34父，引用数量不代各父完整验收。旧1932/F:000729B0只纳CAC08分表及源码关联，本步保全部历史来源/范围子记录，再追加完整当前合同，不新建同一原F身份。

原1..7七槽分发表472D00的目标为472A28/472B3D/472C20/472CC2/472C5F/472C50/472C9D；type4与超范围、visible非1均进入公共下一元素。旧X:00072CFE-00072D20包含完整34B表邻域，复用原字节，不新处置或重分类。

原元素步长24H，元素数量、type、visible及坐标/尺寸保持W32，原地址槽由既有原生指针承载。现WindowGraphicData的offsetX/Y/width/height仍S32存储，原裁剪分支读取这四槽的低W16；WindowNumberData为text指针及color/style/flags/byte07四BYTE，不改类型/布局或写方。

<a id="window-draw-decision"></a>
## 分发和读取合同

入口取得当前窗口，只有visible恰1的元素参与。坐标按原先element.x→window.x→window.y→element.y取样，随后分派type；每元素末重新取当前窗口，增加S32索引再读新elementCount。末尾无论元素数是否正，依序清文本队列层3，再调用476680绘范围提示。

type1：入口只读一次imageNo，零跳过；四W32裁剪字段按原短路门槛测试。裁剪时GetMapNoIndex使用入口缓存图号，再GetXY；四个裁剪槽各只复制低两BYTE至uint16局部，先offsetX/加图偏移、offsetY/加图偏移、width、height。不把存储字段缩为W16，也不把W32读取后截断当原两BYTE取样；标准memcpy承载读宽，机器优化读取仍待。

两图形路径都构造既有16B descriptor：四W16矩形、W32 color、BYTE cropped、BYTE mode4和末两零。无裁剪路径重新取当前窗口→新imageNo→element.y→window.y→element.x→window.x；裁剪路径新imageNo→element.y→element.x→新当前窗口→window.y→window.x。按两条原路径分别持有取样，保持DrawGraphic3的DF/0/1/0和返回弃置，不跨资源callee缓存旧窗口或复用入口图号。

type2：先载荷value04再action，FrameMove(action,100,stop!=0,0)，后取action graphicIndex，零跳过。原内联mask309与descriptor13、替换BYTE310合成，flags308按1/2/4/8选color2的BYTE2/1/0/3写descriptor10/9/8/11。现既有ApplyWindowAnimationDrawMode抽取仍共用，改以unsigned char视图读取被选单BYTE，保各条件/读取/写入序与返回同一drawMode地址；color2仍S32、其生产和其他调用源码保持。只证明本原父的读宽，其他父完整读取/owner/机器ABI仍待。

动画Y偏移callee后新当前窗口/element.y/window.y，再X偏移callee后新当前窗口/element.x/window.x，DrawGraphic2保持DF、0、0及真实descriptor。GetYobiActionField20正确body保持，不为抽取helper新增原F。

type3：原text指针为空就跳过；减元素height，再取BYTE color、BYTE font，减width，以低S16坐标/低BYTE参数、mode3/flags0绘制。type5：载荷必须等于焦点；当前context为626AA0时C9A96C必须恰1才进聊天滚动，否则原载荷非空才进固定1024滚动。type6以x+5/y调用CLD日志绘制。type7原载荷为空就跳过，先flags BYTE→color BYTE→style BYTE→text指针，再调用4771B0；明确四字段顺序，保原参数值、DF及返回弃置。所有既有空指针/焦点/图号/visible门槛已由原码证实，保持。

六逻辑组十三处：入口坐标一、入口图号缓存一、四裁剪低W16、两图形坐标路径、数字载荷读取一、四动画颜色BYTE。两定义和两真实声明四关联注释。没有新API/函数/字段/布局/owner/分配释放，只有既有helper取样更新；不新增保护、默认值、补零、钳制、容量限制、回滚或异常/溢出模型，原descriptor零写入保。

<a id="window-draw-acceptance"></a>
## 限定静态验收

核完整原根/实际E8/跳/RET、51入向、七实际表项/旧范围、各BYTE/W16指令和前后取样；四整源精确重放六组十三处/四注释，GetField20和全部其他body/布局保持。Yobi共用helper的全部既有调用源码保持，不把本步核对外推为那些父完整验收。

旧1932保历史追加、一F处置；INPUT/UIRUNTIME/RENDER/YOBI各一个成员、四限定S零I，九任务保持open。语义/F身份数与文档关联数保持，不把补全旧父误算为新身份或新关联；所有枚举、完整门禁、六closure和总SPEC§10保持。

仅必要文本静态与measure_cg2d.ps1/-Check，不编译、测试、运行客户端或算法、查看产物或计算摘要。51调用父、元素创建释放及六载荷/纹理/动画/字体/输入/日志/提示的全生产消费和alias-owner寿命、CRT/SDK/机器ABI/优化后读取、真实文字/像素/动画及全生命周期仍待。

<a id="window-draw-source-state"></a>
## 当前状态

本步前提交说明“[#] 补齐聊天日志绘制并明确尺寸取样顺序”；拟提交说明“[#] 补齐窗口绘制分发并恢复字段取样合同”。完整原根、七槽旧表、六组十三处/四注释精确差异、旧1932保历史追加、登记、静态汇总/Check与派生范围核对通过。语义2253=2205F+48X及文档关联2315/3680=62.91%保持，函数处置1693/证据1493，INPUT75/UIRUNTIME108/RENDER81/YOBI101且枚举未闭。184叶验收0、60任务关闭11/开放49、六closure开放及全部完整门禁保持；未构建/测试/运行，整体active/未完成。
