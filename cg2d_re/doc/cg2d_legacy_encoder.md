# cg2d 旧保存数据编码证据

## 函数边界与调用关系

- 原函数：`sub_52D310@[0x52D310,0x52D457)`，共 327 字节、130 条指令、
  29 个基本块，无 callee。
- 四个 caller/callsite 分别是 `sub_5186A0@0x51872A`、
  `sub_520200@0x520250`、`sub_532FC0@0x53312F` 与
  `sub_533730@0x5337B0`。
- 四处调用均不使用 EAX，源码因此在 `LoginSavedState.cpp` 匿名命名空间定义
  私有 `[[maybe_unused]] void EncodeLegacyBytes_52D310(...)`；本项不接 caller，
  不增加公开声明。

六个参数依次为：可改写的源字节、源字节数、模式、目标字节、编码长度输出、
目标容量。原函数不检查空指针、长度、容量或缓冲区重叠。

## 容量不足分支

函数先把 `sourceBytes+1` 按 x86 低 32 位回绕物化为 `required`，再对
`required` 与 `destinationCapacity` 作有符号比较。严格大于容量时：

1. 先写 `*encodedBytes=destinationCapacity`；
2. 容量大于零才从下标 0 开始前向逐字节复制
   `destination[index]=source[index]`；
3. 每写一个字节并递增下标后，都重新读取 `*encodedBytes` 作为循环上界；
4. 结束后直接返回，不计算控制字节，不改写源数据，也不插入控制字节。

这里不能把复制长度缓存为入口容量：若长度输出与其他参数缓冲区发生别名，循环尾
对 `*encodedBytes` 的逐轮重读会反馈此前写入。

## 正常编码分支

`required<=destinationCapacity` 时先写 `*encodedBytes=required`，再严格按以下
时序处理：

1. `sourceBytes>0` 时只计算一次 `mode%7`。随后遍历全部源字节；每轮仅独立
   读取一次原字节，先把该原值按 W8 回绕累加到 `control`。
2. 每轮先比较 `mode%7` 与 `index%5`。仅在不相等时，才依次单独计算
   `index%2`、`mode%2` 并比较；任一条件命中便把刚才读取的原字节逐位取反，
   原地写回 `source[index]`。
3. 源变换完成后重新读取一次 `*encodedBytes`；该值小于等于零时立即返回。
4. 否则计算有符号余数 `mode%sourceBytes`，取其绝对值得到控制位置 `K`。
5. 从编码下标 0 开始输出：
   - `index<K`：写变换后的
     `source[index] + control * ((low32(index*index)) % 3)`；
   - `index==K`：写 `control`；
   - `index>K`：写变换后的
     `source[index-1] + control * ((low32(index*index)) % 7)`。
6. 加法、乘法及写出均只保留字节低 8 位。下标平方先保留 x86 低 32 位，
   再按有符号值参与 `%3` 或 `%7`。
7. 每写一个目标字节并递增下标后，都重新读取 `*encodedBytes` 作有符号循环
   上界；不能缓存首次读取值。

因此正常分支会先原地改变源缓冲区，再从该缓冲区构造含一个控制字节的目标流；
源与目标或长度输出重叠时，后续读取仍保留原二进制的别名反馈顺序。

## 四个 caller 的固定参数

- `sub_5186A0@0x51872A`：源是从 `gAlbum` 复制出的局部缓冲，
  `sourceBytes=0x3C00C`、`mode=0`、`destinationCapacity=4`；目标为已分配缓冲，
  长度写到局部变量。`required=0x3C00D`，因此固定走容量不足分支并复制 4 字节。
- `sub_520200@0x520250`：源是从 `0xFF4918` 复制出的 `0x510` 字节局部记录，
  `sourceBytes=0x510`、`mode=0`、目标为 `0xA20` 字节局部缓冲，
  `destinationCapacity=0xA20`，长度写到局部变量。
- `sub_532FC0@0x53312F`：源是该函数构造的 `0x510` 字节局部记录，其余固定值为
  `mode=0`、`destinationCapacity=0xA20`，目标和长度输出均为局部对象。
- `sub_533730@0x5337B0`：源是物理保存记录的 `0x510` 字节局部副本，其余固定值为
  `mode=0`、`destinationCapacity=0xA20`，目标和长度输出均为局部对象。

后三处正常保存路径均产生 `0x511` 字节编码流。

## 正常域与异常域

- 现有三个完整记录 caller 使用 `sourceBytes=0x510`，平方的低 32 位与地址范围均在
  已知正常域内；相册 caller 固定走原样截断分支。
- `sourceBytes=0` 且进入正常分支时，初始编码长度为 1，随后会执行
  `mode%sourceBytes`；原 x86 会触发除法异常，本实现不补零长度门禁。
- `sourceBytes+1`、下标平方及字节算术显式保留 32 位或 8 位回绕；其余除法异常、
  非法指针、负长度、越界、容量不实和重叠造成的结果均保留为调用方前提或
  C++ 异常域，不增加拒绝、钳制或临时副本。
- 本项不实现 caller、loader/map 接线或专用 mode 0 包装，也不修改 CMake。

行为证据只取自当前 `ida-17524 / cg2d.exe`；STONAGE 不参与结论。
