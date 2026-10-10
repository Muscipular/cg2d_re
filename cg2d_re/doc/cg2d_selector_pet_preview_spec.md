# 宠物序号映射、名称负重与预览 SPEC（PSP01）

2026-10-10。沿SPG01选择器的真实数据生产和预览消费，依子AGENTS第8条工程委托裁决。同一cg2d_re分支、不打tag。

<a id="selector-pet-baseline"></a>
## 原根及旧证据

4C6990=10条22B，4C71C0=44条113B，4C69B0=96条265B，4C6D90=88条290B；合238条690B/六页、九E8七目标/21跳六RET，八入CALL仅核点/父及后邻。旧892名称负重原只30条格式邻窗，保全部历史，追加本轮完整96条，三新F/四处置；helper是对内嵌原代码的现实现，不新增原F。复用BOT01完整521DD0证据中的521E3C（8a4c2420），层输入只消费低BYTE，不重取原callee或提升所有父。

<a id="selector-pet-decision"></a>
## 已委托方案及边界

PSP01四完整原根238条690B/六页、九实际E8七目标/21跳六RET、八入CALL/六后邻有据，仅核点不代全外父。6990同state mode读S8，71C0的S32结果写selectedPet70并原返回。71C0只ordinal==-1立即-1；五8B排序对，先matched>=ordinal停止，否则usedW16非零读indexS16、宠物flagsW16==2/1才matched++，轮后position++，结束position--、负返-1否则再读对应S16索引；ordinal0/其它负值与匹配不足原结果保持，不新增保护或固定失败。69B0保selected=-1/AL范围、fresh selected与同宠物previous/updated BYTE门槛、previous先写/updated清0、同非拥有名称元素/data、别名名非空否则原名含NUL复制；名测宽明确U8字体先文本指针，负重原max再weight、格式调用后文本指针先字体；两次fresh元素1 x及S32/2+101居中与两借用目标保持，文本按已批GBK及MC01宿主CRT，不恢复旧Big5/私有复制/格式算法或新增容量保护。6D90保selected负或BYTE3EC0返selected、不加<5；借同preview，读宠物S32图号/写action字段后FrameMove(100,0,0)，graphicIndex0返该result；正图依原掩码及四色BYTE更新后layerU8，两轴依次Y/新windowY+163，再X/elementX/新windowX+101，DrawGraphic2原返回。旧ApplyWindowAnimationDrawMode helper增加detail取序参数：本preview显式true先replacement再previous，已有四单参调用默认false保先previous再replacement，两分支只读后同式发布；mask/flags/通道/desc和同owner及全部其它helper正文保，没有新helper/原F/布局/字段/owner或业务默认值。复用BOT01原521E3C BYTE消费者，只证明layer低BYTE，原PUSH EAX高24位不据此整ABI等价。四组四处（两测宽/一预览接线/一helper取序）、五定义五声明十注释，四整源其余/两正确叶/五类型保持。旧892格式/历史全保追加、三新F四处置、四限定S零I/十任务open；全排序对/宠物/文本/描述符生产消费与所有callee/外父/alias-owner寿命、SDK/生成取样/全机器ABI/真实预览像素及运行仍待，48有界代码/完整枚举gates/六closure/总SPEC§10及WAV-only/明文网络/MSVC保持，无新保护/补零/容量上限/钳制/回滚或异常模型。

现detail helper新增布尔取序参数replacementBeforePrevious，默认false仅承接旧四个单参调用，preview传true。mask非零分支用两局部BYTE分别赋值：true先replacement再previous，false先previous再replacement；两项都取后才原式发布，四色和返回同descriptor不改。无补零、fallback或业务默认字段；其它四调用的源码保持，不要求重验其完整父。两测宽按对应原读取显式持样，名称与负重读序不同；复制/格式继续已批MSVC，文本继续已批GBK，不重建旧CRT。

<a id="selector-pet-acceptance"></a>
## 限定静态验收

核四完整连续原根、九E8七目标/八入CALL，五排序对usedW16/索引S16/flagsW16与边界/发布及原返回，名称/负重两测宽的字段取序与两次居中，preview的推进/同descriptor掩码与四色/Y→X/队列返回。helper两分支BYTE取序，四个旧单参调用保持；四整源精确差异及五类型保持。三新F/旧892补完整四处置、四限定S零I/十open任务，不改完整枚举/gates/六closure/184叶验收/总SPEC§10。生成读取/机器ABI/实际文字预览像素与正常owner、全生产消费/callee/外父及运行仍待。

只做必要静态文本与measure_cg2d.ps1/-Check，不构建/测试/运行/产物检查/摘要/新依赖。

<a id="selector-pet-source-state"></a>
## 当前状态

前提交“[#] 补齐选择器切页与显示合同”；拟提交“[#] 补齐宠物选择与预览消费合同”。方案已先记录并实施，源码及登记必要静态核对通过；静态汇总/-Check终态均退出0，派生范围核对通过。

语义2297=2249F关联+48有界代码记录，函数处置1744/证据1533，INPUT121/UIRUNTIME158/RENDER94/YOBI103；文档关联2359/3680=64.10%仅关联率。184叶验收0、60任务关闭11开放49、六closure及完整门禁保持。登记、静态汇总/-Check终态均退出0；派生仅六metrics/四F/四功能/十任务变化，完整门禁保持。
