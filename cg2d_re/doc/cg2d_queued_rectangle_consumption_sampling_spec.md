# BOZ01 队列矩形消费者的取样与栈发布

<a id="queued-rectangle-consumption-baseline"></a>
## 当前缺口与原身份

已有描边/着色生产和总绘制分发，但两个真实消费者缺独立机器映射/功能成员；脏区消费者把原四次W16读取合为两个W32快照，描边消费者增加原没有的RECT整块零初始化。本步先明确原正常合同，再恢复这两个源码差异，不重验已过的排序/生产和总分发算法。

[完整原证据](cg2d_queued_rectangle_consumption_sampling_evidence.json)：522210为[522210,522293)53条131B、一页3call/3内跳/3ret；520AD0为[520AD0,520B09)19条57B、一页1call/1内跳/1ret。合72条188B、两页4call/4内跳/4ret。两个原入向调用点5216EE/5216FC均在唯一父520DD0；父选定case2/3窗口[5216E9,521701)8条24B单独纳盘，不计两根分母，不代完整父/所有callee验收。

| 原ID / 源码入口 | 独立生产消费合同 |
|---|---|
| F:00122210 / Sub_522210 | 两W32坐标→16B局部RECT→依高位优先选择描边模式→共享32位Back/调色板消费者 |
| F:00120AD0 / FillDirtyArea_520AD0 | descriptor的mix/颜色→四次W16坐标→FillRect完整返回→共享dirty标志 |

两原根各有一条历史源码定位，不能用定位代独立基线/当前等价或刷新其旧行号。源头为既有唯一队列，父从排序cacheIndex取得同一record，case2传record与imageNo，case3仅record。

<a id="queued-rectangle-consumption-decision"></a>
## 工程裁决与源码范围

依既有工程委托，只修改GraphicPendingQueue.cpp两个正文，头文件、布局、调用父、排序/入队和底层CPU/纹理绘制保持；不新增helper、状态副本、原函数ID或外部依赖。

1. **522210栈发布**：原先读取x/y各一次W32，按right→bottom→left→top发布四个S32矩形字段。低半字按S16符号扩展，高半字原算术右移的正常值由现有高S16转换承接。取消RECT的额外整块零初始化，四字段仍全部赋值；不把它改成脏区四次W16读取。
2. **描边分支**：0x20000000优先模式0，其次0x10000000模式1，再0x40000000模式2；复合标记沿首个命中，三call站点任一路径只一次调用，均无标记不调用。颜色低W16与现行Sub451350原生声明保持，四边界/模式/像素算法不在本步改动。
3. **520AD0取样**：先读取descriptor+0D的U8 mix，再+08的W32 color；然后依次从实际y高半字、y低半字、x高半字、x低半字各memcpy一次W16，作为bottom/top/right/left。以原S16接口传FillRect(left,right,top,bottom,color,mix)，不先缓存两个W32再拆。
4. **返回消费**：FillRect返回完整W32，严格等887601AE才把唯一共享gGraphicDirty写1；其它返回保持原dirty，不补清零。当前32位CPU/SDL接口沿获批替换，不恢复COM/HRESULT机制或扩张其豁免。
5. **承载与寿命**：实际record非拥有借用，x/y原W32位置+0/+4保；descriptor因已批x64地址槽拓宽随真实字段访问，不套原32B偏移作native镜像。RECT仅原有私有实现边界，不新增公开Win32类型；父不消费这两原EAX返回，既有void接口保持，不声明机器ABI/EAX等价。

无介入正常坐标和颜色最终值原先一致，不宣称修复了已有像素错误；改的是已证读取宽度/顺序和额外栈零写。不给异常、越界、溢出加保护/钳制/拒绝或回绕模拟门禁；不运行算法样例。

<a id="queued-rectangle-consumption-acceptance"></a>
## 本步度量与验收边界

必要静态核两完整根字节/连续覆盖、真正稳定ID、两普通入向/同一父和已证case2/3窗口；原parent窗口从实际指令起点5216E9取，不纳入指令中部的伪窗口。源码仅两个完整正文补丁，描边除RECT声明逐字保持，脏区tail判返回/dirty发布逐字保持；四W16真实读点按bottom/top/right/left排列，没有新保护或私有投影。

新增机器映射1411/1412，对应已有两个原身份，冻结定位各1保持；语义1411→1413/有效ID1406→1408、文档关联1524→1526（41.41%→41.47%）、分类531→533、RENDER43→45且枚举未闭。四个独立局部static I/S证据669→673，不累计成四功能或完整运行通过。RESOURCE41/AUDIO32暂停、pending18、队列175及R7区间67/3061、60源码closed10/完整验收0、全部完整门禁/global_accepted=false保持。

U08/U09/G01/G02/R01/R04/V04仅进展和引用，仍open；总分发及所有父/正常动态别名、纹理/面owner和场景/失效/退出、SDK/机器ABI与全模式像素运行仍未验。完整R1/R7/R8和整体SPEC继续，不能以此关闭C09或完整渲染。

<a id="queued-rectangle-consumption-source-state"></a>
## 当前交付

交付两源码正文、独立原证据/SPEC、两机器映射/成员与分类、四局部static I/S及任务/唯一进度/看板对照。命名快照：`cg2d-re-queued-rectangle-consumption-sampling-20261006`；13文件精确提交，用户既有六项排除。只做必要静态和measure/-Check，未构建、测试、运行或计算摘要。

后续沿实际描边/着色生产—同一record—排序层—case2/3—Back/调色板/纹理消费者及dirty重建门禁推进真实owner/画面链；不借本步重验未变算法。网络兼容原格式只明文、内存CryptInt保持、音频暂停，整体SPEC目标不缩。
