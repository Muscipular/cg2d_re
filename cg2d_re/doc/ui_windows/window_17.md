# 窗口17：宠物详细属性

返回[窗口索引](../cg2d_ui_window_runtime.md)。
原主表54E5A0：46元素566200，update48DAB0、draw46FBC0、release48DD00。
完整update/元素/交互尚待，不把以下释放本体记作完整窗口完成。
名称草稿、序号快照与dirty合同见[共享详情依赖](window_18.md#pet-details-shared)，
共用[窗口15名称输入原型](window_15.md#window15-buttons)。

<a id="window17-release"></a>
## 48DD00坐标释放同步

原[48DD00,48DD33)共13指令/3基本块已只读核对，54E5A0唯一数据绑定。
先固定gWindowStates[17].window；空则不写，非空按
16.x→16.y→18.x→18.y写回状态表，每项分别从固定源窗口读取对应坐标。
不以当前窗口代替窗口17、不检查目标window、不缓存一对坐标用于四次写入。
原恒返回1在release不消费，沿既有void回调适配。

48DAB0仍须接完整490780，不能用窗口18技能刷新替代属性刷新。
仅静态核对，无构建、测试、客户端运行或产物检查。
