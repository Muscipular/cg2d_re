# ida mcp
ida分析实例为cg2d.exe;

# 项目目录
仅处理 cg2d_re 目录

# Namespace 与类型规划
1. 不要随意新增全局类型或跨模块暴露底层对象。
2. 禁用c++ rtti
3. 禁空namespace

# 代码风格
## 逻辑尽量语义化表达
1. 合理使用`enum`、`#define`
2. 简单逻辑如：`a.state == A_STATE_STOP`，请勿定义专门函数进行处理，如果大量重复，优先使用`#define`
3. 类型、函数、字段上标注类型/函数/字段作用及关联的类型/函数/作用，如果是复杂逻辑，可以引用doc

# 思考要求
1. 思考时需要考虑其他模块的影响
2. 避免随意改动其他关联方的函数
3. 工具函数是好，但不能滥用

# 常用文件
"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
"C:\Program Files\Microsoft Visual Studio\18\Enterprise\Common7\Tools\VsDevCmd.bat"
