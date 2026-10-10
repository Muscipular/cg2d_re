# 选择器构造、固定预览与退出 SPEC（SIC01）

2026-10-10。沿PSP01预览对象生产及旧生命周期合同，依子AGENTS第8条委托裁决。同一cg2d_re分支、不打tag。

<a id="selector-construction-baseline"></a>
## 完整原根与原范围

4C61B0为139条661B/三页，488400/499900各两条10B，488420为75条358B/两页，499920两条10B，合220条1049B/八页、20E8两目标、24跳两RET。61B0与488420各两chunk，正常110/46条，EH各29条136B；488420主chunk六B间隙的90*6为实际读取，并被前缀JMP跨过，原区间跨度1055B与实际指令1049B分开。488430在488420内部，两固定退出借同体，不新增原F。原54ED50初始模板16B零，仅初值证，不把空Xrefs外推不可变/无alias；保持既有正常descriptor构造，不新冻结来源或补保护。四包装沿旧114/116/5/117，不覆盖旧历史，一新61B0 F/五处置。

<a id="selector-construction-decision"></a>
## 正常合同与实施边界

SIC01五原根/七chunk完整FuncItems220条1049B/八页，20实际E8两目标、24跳两RET；构造139条661B两chunk含29条136B EH，退出488420为75条358B两chunk含29条136B EH。其主chunk另有48842A..30六B间隙（90*6）被前缀JMP跨过，区间跨度与实际指令字节分开，不造F/X或验收。488430是488420内共享尾，499920尾入，不新增原F。两构造wrapper固定CF3B10/D2F338并tailJMP61B0；现绑定唯一静态预览0/1，正常typed this返回保。61B0入口持同非拥有preview地址，十Cry按170/180/190/1A0/1B0/1C0/1E0/1F0/200/210构造并尾取flags|2；initialized0/value64=0/step21/previous=-1/selected=-1、两BYTE开关1/两图索引0→同preview flags/callback及固定F5E988技能源S32图号/layerDF/direction5→hover两索引-1/两原4B区清0/count0/末BYTE0→三16B descriptor正常初值保持：首FFFFFFFF/另两80FFFFFF、BYTE13=4，余依现零模板。原54ED50初始16B全零有据，Xrefs为空不证明全alias不可改，不新增模板冻结、补零或整体清对象，动态来源/alias完整闭包待。唯一一组一处持址贯穿Construct/Finish；两整CPP/两头其余全文、四wrapper与四helper正确body及类型/储宽保持，五定义/五真实声明十注释，匿名wrapper无公头不造声明。退出借固定静态0/1不重读可改state.preview，十Cry按210..170逆序，不free/清选择/合并owner，不进入通用Yobi链；两现构造后独立std::atexit及First→Second旧初始化body保持，Crypto叶/十RNG消费顺序不改。四旧114/116/5/117全历史追加、一新F五adapted/四限定S零I/十任务open；旧MSVC CRT/SDL/WAV-only/明文网络和48有界代码/完整枚举gates/六closure/总SPEC§10保持。完整原EH/SEH/机器ABI仅捕获边界、不记异常等价或再造私有CRT处理器；全部正常字段/模板/skill/preview生产消费、间接alias和跨模块/快速退出/SDK与生成字段访问、全应用顺序/实际生命周期及运行未闭，无新API/helper/结构/字段/owner/保护/默认值/新钳制/回滚/异常模型。

两个匿名构造wrapper先绑定Borrow(0/1)至自己的preview，再真实61B0 this返回；两个退出wrapper直接Borrow(0/1)析构固定对象，不从改动后的preview字段取值。唯一改动是61B0入口持非拥有preview，Construct和Finish都用同样本；原state其它字段/未初始化元素/回调/page/mode保持，不整对象清0。Cry十次顺序原源已存，构造和退出helper正确body保持，仅新注释，不据调用点提升叶算法或全RNG/生命周期验收。既有std::atexit登记/First→Second接线复用，原私人x86 EH段仅纳盘，宿主MC01决议保持，不重造故障/回滚/异常模型或称运行等价。

<a id="selector-construction-acceptance"></a>
## 限定静态验收

核五原根七chunk全部FuncItems、明确六B间隙/双EH分界，20实际E8及24跳/原尾转地址、两个固定对象和原构造/逆析十槽、flags及初始化写序/三个descriptor初值。四整源精确差异一组/十注释，九来源定义中四wrapper/四helper正确body与五类型保持；匿名wrapper无公头，不造声明。旧四全记录历史保、一新F五处置、四限定S零I/十open。全部模板/skill/字段及对象生产消费、完整alias-owner寿命/全应用原序/完整EH/SEH/机器ABI/SDK/快速退出及实际生命周期/运行待，完整枚举/gates/六closure/184叶验收/总SPEC§10不提升。

只做必要静态文本与measure_cg2d.ps1/-Check，不构建/测试/运行/产物检查/摘要/依赖。

<a id="selector-construction-source-state"></a>
## 当前状态

前提交“[#] 补齐宠物选择与预览消费合同”；拟提交“[#] 补齐选择器构造与固定退出对象合同”。方案已先记录并实施，源码及登记必要静态核对通过；静态汇总/-Check终态均退出0，派生范围核对通过。

语义2298=2250F关联+48有界代码记录，函数处置1749/证据1537，INPUT126/UIRUNTIME163/RENDER95/YOBI106；文档关联2360/3680=64.13%仅关联率。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持。登记、静态汇总/-Check终态均退出0；派生仅六metrics/五F/四功能/十任务变化，完整门禁保持。
