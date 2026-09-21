---
name: subagent-dev-workflow
description: 组织会修改项目文件的开发任务，通过独立 subAgent 按分析、编码、审查和验证闭环执行。仅在用户显式调用时使用；不适用于纯问答、只读状态检查或解释。
---

# SubAgent 开发执行

## 适用边界

仅用于会修改项目文件的开发任务。纯问答、只读状态检查、解释或报告不使用本技能。

主 Agent 是唯一编排者和提交者：负责读取项目规则、拆分任务、维护 Agent 状态、处理用户确认、更新进度文件和执行 Git 暂存/提交。subAgent 不得承担编排或提交职责。

## 共同约束

主 Agent 先把当前工作拆成一个单一、可验证的 `task_id`。每个 subAgent 只处理一个 `task_id`，默认使用 `xhigh` 思考强度，并遵守以下约束：

- 不派生其他 subAgent，不自行扩展目标、路径或验收标准。
- 只操作 `allowed_paths`，避开 `forbidden_paths` 和无关的已有改动。
- 不执行 `git add`、`git commit`，不改写或丢弃用户改动。
- 文本读写显式使用 UTF-8，并遵循项目已有格式。
- 不引入外部依赖；需要依赖时只报告需求和影响。
- 结论必须附带文件、行号、命令输出或其他可核验的证据。
- 需求、权限、工具、工作树归属或事实不明确时，返回 `NEEDS_CLARIFICATION`，不得猜测。

发现工作树改动与 `allowed_paths` 重叠、无法确认改动归属或继续操作可能覆盖他人工作时，立即停止并由主 Agent 请求用户处理。不得以缩小验证范围或降低验收标准绕过阻塞。

## 任务交接

每次创建或继续 subAgent 时，主 Agent 提供完整交接；不得依赖未落盘的隐含上下文：

```yaml
task_id: 单一任务标识
objective: 本任务唯一目标
acceptance_criteria: 可逐项验证的验收标准
allowed_paths: 允许读取或修改的路径及权限
forbidden_paths: 禁止触碰的路径
applicable_rules_and_spec: 项目规则与已确认 SPEC
user_constraints: 用户明确约束和授权边界
dirty_worktree: 已有改动、归属及重叠检查结果
validation_plan: 计划执行的检查及明确不执行的检查
previous_findings: 上一轮问题、修复证据或失败证据
```

## 执行流程

严格按以下顺序推进，不得合并角色：

1. **分析 subAgent（只读）**：调查现状、依赖关系、影响模块、风险和验证方式，不修改文件。输出 `READY` 后完成交接并结束。
2. **主 Agent 门禁**：检查分析证据，处理所有待澄清问题。设计、重构、优化或项目规则要求确认 SPEC 时，先向用户提交 SPEC；未获确认不得编码。任何已确认 SPEC 或范围发生实质变化时，必须立即停止当前循环，启动新的分析 subAgent，重新完成门禁并取得用户确认，之后才能继续编码；不得在原开发、review 或验证循环中直接继续。
3. **开发编码 subAgent**：按已确认范围做最小修改并自查，只生成进度记录草稿，不写进度文件、不暂存、不提交。
4. **独立 review subAgent（只读）**：核对差异、验收标准、范围和回归风险，不得直接修复。`CHANGES_REQUESTED` 必须回到原开发 subAgent 修复，再交给原 reviewer 复审。
5. **独立验证 subAgent（只读）**：仅在 review `PASS` 后执行约定的构建、测试、静态检查或其他验证，不得修复。验证失败时，将证据送回原开发 subAgent，随后重新 review，再重新验证。
6. **完成**：仅当 review 和验证均为 `PASS`，主 Agent 才更新本项目 `plans/progess.md`，精确暂存本任务文件并执行小步提交。主 Agent 是该文件的唯一写入者和本任务的唯一提交者；用户明确要求不提交时，只更新进度，不暂存或提交。

开发与 review 最多三轮：首次编码后 review 计为第 1 轮，每次验证失败后返回开发再 review 也占用下一轮。第 3 轮仍为 `CHANGES_REQUESTED`，或第 3 轮 review 通过后验证仍失败时，停止并报告剩余问题；不得开启第 4 轮。替换 Agent 不重置轮数。

任一角色返回 `BLOCKED` 时，主 Agent 必须立即停止当前流程，保留阻塞原因及全部证据并向用户报告，不得继续编码、复审或验证。

## 角色输出契约

分析 subAgent 返回：

```yaml
status: READY | NEEDS_CLARIFICATION | BLOCKED
evidence: 已核验事实及来源
affected_modules: 受影响模块及关系
constraints: 适用规则和限制
proposed_spec: 建议实现边界；无需 SPEC 时说明原因
paths: 建议 allowed_paths 与 forbidden_paths
acceptance: 可验证验收项
validation: 建议验证计划
risks: 风险及影响
questions: 必须由用户回答的问题
```

开发 subAgent 返回：

```yaml
status: DONE | NEEDS_CLARIFICATION | BLOCKED
changed_files: 实际修改文件
summary: 对应目标的最小实现说明
self_checks: 已执行检查及结果
known_risks: 未解决风险或限制
progress_entry_draft: 供主 Agent 写入进度文件的草稿
```

review subAgent 返回：

```yaml
verdict: PASS | CHANGES_REQUESTED | NEEDS_CLARIFICATION | BLOCKED
findings:
  - severity: 严重程度
    file: 文件路径
    line: 行号；不适用时说明
    evidence: 可复核证据
    expected_change: 期望修复结果
acceptance_matrix: 每项验收标准的状态与证据
scope_check: 路径和任务边界检查
regression_risks: 回归风险
progress_check: 进度草稿是否准确完整
```

验证 subAgent 返回：

```yaml
verdict: PASS | FAIL | NEEDS_CLARIFICATION | BLOCKED
commands: 实际执行的命令或检查
results: 每项结果及证据
unverified_items: 未验证项、原因和影响
```

`PASS` 不得含无证据的推断。任何影响验收的未验证项都必须阻止完成。

## subAgent 复用

主 Agent 维护 `task_id / role / agent_id / iteration / status` 映射，并按以下规则复用：

- 仅同一 `task_id`、同一角色优先复用；不同角色始终隔离，不同 `task_id` 禁止复用。
- 分析 Agent 在 `READY` 交接前可继续补充同一任务；`READY` 后结束。SPEC 或范围实质变化时新建分析 Agent。
- review 要求修复时复用原开发 Agent；仅在其不可用、持续阻塞或上下文失效时替换。
- 同一任务复审复用原 reviewer，以保持问题追踪一致；reviewer 不得转任开发或验证。
- 验证 Agent 必须独立于分析、开发和 reviewer。同一验证方案的重跑可复用原验证 Agent；验证方案或改动范围变化时新建验证 Agent。
- Agent 替换不得清零 review 轮数或丢弃既有 findings、修复证据和验证证据。
- 任务通过、阻塞或取消后结束该任务的全部 subAgent。不得跨任务携带对话上下文，只能复用已落盘的 SPEC、进度和验证证据。
