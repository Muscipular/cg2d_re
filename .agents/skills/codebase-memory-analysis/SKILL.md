---
name: codebase-memory-analysis
description: 使用 codebase-memory-mcp 知识图谱分析代码库。需要定位函数、类、路由或变量，追踪调用关系，理解模块架构，评估改动影响范围，或以证据回答代码实现问题时使用。
---

# 代码库图谱分析

使用 `codebase-memory-mcp` 作为代码发现的首选来源，基于实际符号、调用关系和源码片段作答。默认只分析；仅在用户明确要求时修改代码。

## 前提

- 若仓库尚未建立索引，先运行 `index_repository`；若图谱结果明显过期或不完整，说明原因后再索引。
- 在答复中区分图谱直接证据与推断；无法证实时明确说明。

## 分析流程

1. 明确要找的代码实体、行为或变更范围；不要把未指定的业务问题扩大为全仓库审查。
2. 按下列优先级发现代码：
   1. `search_graph`：按名称或模式定位函数、类、路由、变量。
   2. `trace_path`：追踪入站调用者或出站依赖。
   3. `get_code_snippet`：阅读已定位实体的实现。
   4. `query_graph`：仅在需要多跳关系、聚合或复杂筛选时执行 Cypher 查询。
   5. `get_architecture`：需要模块级概览时获取架构摘要。
3. 先用窄查询定位，再扩大模式；获得调用关系后读取关键实现，不要凭名称推断行为。
4. 汇总结论时列出相关符号、关系方向、关键实现依据和不确定项。对于改动影响分析，分别给出直接调用者、直接依赖与需要人工确认的间接影响。

## 常用查询模式

```text
# 定位处理器或相似符号
search_graph(name_pattern=".*OrderHandler.*")

# 谁调用该函数
trace_path(function_name="OrderHandler", direction="inbound")

# 该函数调用什么
trace_path(function_name="OrderHandler", direction="outbound")

# 阅读已确认的实现
get_code_snippet(qualified_name="pkg/orders.OrderHandler")
```

## 降级规则

- 查找字符串字面量、错误消息、配置值或非代码文件时，使用 `rg` 等文本检索。
- 图谱工具无法提供足够结果时，再使用文本检索或文件读取补充，并在结论中说明该降级。
- MCP 未配置、不可用或索引失败时，说明具体阻塞原因；不得虚构图谱查询结果。
