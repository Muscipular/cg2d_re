# cg2d 旧保存数据解码证据

## 函数边界与调用关系

- 原函数：`sub_52D460@[0x52D460,0x52D569)`，共 `0x109`（265）字节、
  115 条指令、18 个基本块，无 callee。
- 两个直接调用点分别是 `sub_518780@0x518823` 与
  `sub_532ED0@0x532F83`。
- 源码仅在 `LoginSavedState.cpp` 匿名命名空间定义私有
  `[[maybe_unused]] DecodeLegacyBytes_52D460`；本项不接 caller，也不增加公开声明。

## 完整算法

参数依次为源字节、源字节数、模式、目标字节及解码长度输出。函数首先以 32 位
有符号运算计算 `remainder = mode % (sourceBytes - 1)`；该除法发生在写出长度
之前。随后执行：

1. 写 `*decodedBytes = sourceBytes - 1`。
2. 取 `controlIndex = abs(remainder)`，再读
   `control = source[controlIndex]`。
3. `sourceBytes<=0` 时跳过第一循环；否则遍历
   `index=[0, sourceBytes)`。每轮先独立读取一次 `*decodedBytes` 并计算
   `firstControlIndex=abs(mode % *decodedBytes)`：
   - `index` 小于 `firstControlIndex` 时，向 `destination[index]` 写
     `uint8(source[index] - control * ((index * index) % 3))`；
   - 无论首次判断真假，随后都再次独立读取 `*decodedBytes`，计算
     `secondControlIndex=abs(mode % *decodedBytes)`；
   - `index` 大于 `secondControlIndex` 时，向 `destination[index-1]` 写
     `uint8(source[index] - control * ((index * index) % 7))`。
   两次判断是独立 `if`，不构成 `else-if`；若两次读取间长度因重叠写入而变化，
   同一轮允许两个写分支都执行。
4. 第二循环入口先读取 `lastDecodedBytes=*decodedBytes` 并令 `index=0`；
   首次值小于等于零时直接返回，且不计算 `mode % 7`。
5. 首次值为正时只计算一次 `modeModulo7=mode%7`。每轮先单独计算
   `index%5`；仅当其不等于 `modeModulo7` 时，才严格依次单独计算
   `index%2`、`mode%2`。任一比较命中便逐字节取反 `destination[index]`。
6. 完成可选取反后，才在循环尾重新读取
   `lastDecodedBytes=*decodedBytes`，随后递增 `index` 并与该尾读值作有符号比较。
   退出时返回这次最后读取的值。

该写法保留模 7/模 5 比较优先于两个模 2 运算的短路顺序，同时避免在循环中
重复计算 `mode%7`。

## 正常保存数据域

能力保存读取路径的正常输入是 `mode=0`、`sourceBytes=0x511`。此时：

- 写出的解码长度为 `0x510`；
- 初始控制下标为 0，`source[0]` 是控制字节；
- 第一循环跳过源下标 0，把源下标 `1..0x510` 映射到目标
  `0..0x50F`；
- 第二循环对“下标为 5 的倍数或偶数”的目标字节取反；
- 最终返回 `0x510`。

现有两个 caller 观测到的最大源长度是 `0xA20`，因此正常调用范围内
`index * index` 不会溢出 32 位有符号整数。

## 短输入、超长输入与 C++ 边界

- `sourceBytes=0` 时，除数为 `-1`；除 `mode=INT32_MIN` 的异常组合外，
  长度会写成 `-1`，仍会读取 `source[0]`，两个循环均不写目标并返回 `-1`。
- `sourceBytes=1` 时首次除数为零；原 x86 `idiv` 在写长度前失败，C++ 表达式
  同样不具备可继续执行的定义，本实现不补保护。
- 短正长度 `sourceBytes>=2` 仍按同一算法删除一个控制字节并产生
  `sourceBytes-1` 字节，不存在专用分支。
- 超长输入不做源/目标容量检查。`sourceBytes>46341` 时循环中的有符号
  `index * index` 可能进入 C++ 溢出未定义域；已知 caller 最大值不触及该域。
- `sourceBytes=INT32_MIN` 的减一，以及 `INT32_MIN % -1`，也属于 C++
  无定义的有符号算术组合；它们不在现有正常调用域内。
- 函数不检查空指针、容量或缓冲区重叠，也不把异常参数归一化；调用方必须满足
  指针有效、缓冲区足够及算术可定义等前提。

本项不实现 loader、map、caller 或 mode 0 专用包装，不修改 CMake。行为证据只取自
当前 `ida-17524 / cg2d.exe`；STONAGE 不参与结论。
