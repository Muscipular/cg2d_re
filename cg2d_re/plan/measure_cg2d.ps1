#requires -Version 7.0
<#
静态台账汇总器：只读取JSON/Markdown、核身份与文件引用、写本目录两个派生文件。
不编译、不执行测试/客户端/算法样例、不访问IDA/网络、不计算摘要。
-Check只在内存生成并比较派生文本；退出0只证明结构与汇总一致，不代表行为验收。
#>
[CmdletBinding()]
param([switch]$Check)

$ErrorActionPreference = 'Stop'
$projectRoot = [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$utf8 = [System.Text.UTF8Encoding]::new($false, $true)
$errors = [System.Collections.Generic.List[string]]::new()
$pathCache = @{}
function Read-Text([string]$path) {
    [System.IO.File]::ReadAllText((Join-Path $projectRoot $path), $utf8)
}
function Read-Json([string]$path) { Read-Text $path | ConvertFrom-Json -AsHashtable }
function Require([bool]$condition, [string]$message) {
    if (-not $condition) { $errors.Add($message) }
}
function Check-Path([string]$path, [string]$owner) {
    if ([string]::IsNullOrWhiteSpace($path)) { $errors.Add("$owner 缺证据路径"); return }
    if ($pathCache.ContainsKey($path)) { return }
    $target = [System.IO.Path]::GetFullPath((Join-Path $projectRoot ($path -split '#',2)[0]))
    if (-not $target.StartsWith($projectRoot + [System.IO.Path]::DirectorySeparatorChar, [System.StringComparison]::OrdinalIgnoreCase)) {
        $errors.Add("$owner 路径越出本线：$path"); return
    }
    Require (Test-Path -LiteralPath $target -PathType Leaf) "$owner 引用不存在：$path"
    $pathCache[$path] = $true
}
function Unique($items) { @($items | Where-Object { $_ } | Sort-Object -Unique) }
function Percent([int]$n,[int]$d) { if ($d -eq 0) { return $null }; [math]::Round(100.0*$n/$d,2) }
function Cell($value) { ([string]$value).Replace('|','/').Replace([string][char]10,' ') }
function Gate-State($feature,[string]$gate) {
    if ($feature.gates.Contains($gate)) { return $feature.gates[$gate].state }
    if ($gate -eq 'R' -and -not $registry.execution_policy.runtime_allowed) { return 'blocked' }
    return 'unassessed'
}

$inventory = Read-Json 'doc/cg2d_ida_inventory.json'
$semantic = Read-Json 'doc/cg2d_semantic_function_mappings.json'
$direct = Read-Json 'doc/cg2d_direct_function_locations.json'
$registry = Read-Json 'plan/cg2d_acceptance_registry.json'
$taskText = Read-Text 'plan/cg2d_spec_closure_checklist.md'
Require ($registry.schema_version -eq 1) '不支持的登记簿版本'
Require ($registry.features.Count -gt 0) '功能目录不能为空'
Require (@($registry.catalog_changes | Where-Object revision -eq $registry.catalog_revision).Count -eq 1) '目录版本缺唯一变更记录'
Require ($registry.inventory_expected.functions -eq $inventory.functions.Count) '函数分母变化须登记依据'
Require ($registry.inventory_expected.functions -eq $inventory.counts.function_count) '原库存函数计数不一致'
Require ($registry.inventory_expected.unassigned_ranges -eq $inventory.unassigned_executable_ranges.Count) '区间分母变化须登记依据'

$functions = @{}
foreach ($f in $inventory.functions) {
    Require (-not $functions.ContainsKey($f.id)) "重复原函数ID：$($f.id)"
    $functions[$f.id] = [ordered]@{
        id=$f.id; original_entry=('0x{0:X}' -f $f.va); original_name=$f.name
        semantic_indices=@(); pending_indices=@(); direct_snapshot_definitions=@()
        baseline_documents=@(); classification_refs=@(); disposition=$null
        feature_memberships=@(); sources=@()
    }
}
$semanticWithoutId = [System.Collections.Generic.List[object]]::new()
foreach ($section in @('records','pending_records')) {
    for ($i=0; $i -lt $semantic[$section].Count; $i++) {
        $r = $semantic[$section][$i]
        $ref = "doc/cg2d_semantic_function_mappings.json#/$section/$i"
        foreach ($p in @($r.evidence_documents)) { if ($p) { Check-Path $p $ref } }
        if (-not $r.inventory_id) {
            $semanticWithoutId.Add([ordered]@{reference=$ref;original_entry=$r.original_entry;status=$r.status})
            continue
        }
        Require ($functions.ContainsKey($r.inventory_id)) "$ref 身份不在原库存"
        if (-not $functions.ContainsKey($r.inventory_id)) { continue }
        $f = $functions[$r.inventory_id]
        Require ([Convert]::ToInt64($r.original_entry.Substring(2),16) -eq [Convert]::ToInt64($f.original_entry.Substring(2),16)) "$ref 原入口不符"
        if ($section -eq 'records') { $f.semantic_indices += $i } else { $f.pending_indices += $i }
        $f.baseline_documents += @($r.evidence_documents)
        if ($r.decision_spec) { Check-Path $r.decision_spec $ref; $f.baseline_documents += $r.decision_spec }
        foreach ($source in @($r.sources)) {
            if ($source.path) { Check-Path $source.path $ref; $f.sources += $source }
        }
    }
}
foreach ($r in $direct.records) {
    Require ($functions.ContainsKey($r.inventory_id)) "直接定位未知ID：$($r.inventory_id)"
    if ($functions.ContainsKey($r.inventory_id)) {
        $functions[$r.inventory_id].direct_snapshot_definitions += @($r.definitions)
        foreach ($d in $r.definitions) { Check-Path $d.path '历史定位' }
    }
}
$classifications = [System.Collections.Generic.List[object]]::new()
$declaredExcluded=@{}; $declaredMixed=@{}
foreach ($ref in $semantic.classification_references) {
    Check-Path $ref.path '分类引用'
    $data = Read-Json $ref.path
    $classifications.Add([ordered]@{path=$ref.path;record_count=$data.records.Count;scope=$ref.scope})
    for ($i=0; $i -lt $data.records.Count; $i++) {
        $r = $data.records[$i]
        if (-not $r.inventory_id) { continue }
        Require ($functions.ContainsKey($r.inventory_id)) "分类未知ID：$($r.inventory_id)"
        if (-not $functions.ContainsKey($r.inventory_id)) { continue }
        $f = $functions[$r.inventory_id]
        $f.classification_refs += [ordered]@{path=$ref.path;index=$i;status=$r.status}
        if ($r.status -like '用户范围排除*') { $declaredExcluded[$r.inventory_id]=$true }
        if ($r.retained_bits_per_pixel) { $declaredMixed[$r.inventory_id]=$true }
        $docs = @($data.evidence_document) + @($ref.evidence_document) + @($r.evidence_documents)
        foreach ($p in $docs) { if ($p) { Check-Path $p '分类合同'; $f.baseline_documents += $p } }
    }
}
foreach ($entry in $registry.newest_excluded_entries) {
    $id = 'F:{0:X8}' -f ([Convert]::ToInt64($entry.Substring(2),16) - $inventory.baseline.imagebase)
    Require ($functions.ContainsKey($id)) "newest排除入口无库存ID：$entry"
    if ($functions.ContainsKey($id)) {
        $functions[$id].classification_refs += [ordered]@{path='plan/cg2d_full_reconstruction_plan.md';index=$null;status='用户范围排除：newest五函数链'}
        $functions[$id].baseline_documents += 'doc/cg2d_login_state_controller.md'
        $declaredExcluded[$id]=$true
    }
}

$tasks = @{}
foreach ($m in [regex]::Matches($taskText,'(?m)^\| ([PCUGTRV]\d{2}) / ([^|]+)\|([^|]+)\|([^|]+)\|([^|]+)\|')) {
    $id=$m.Groups[1].Value
    Require (-not $tasks.ContainsKey($id)) "重复任务：$id"
    $tasks[$id] = [ordered]@{id=$id;name=$m.Groups[2].Value.Trim();current=$m.Groups[3].Value.Trim();next=$m.Groups[4].Value.Trim();completion=$m.Groups[5].Value.Trim();feature_ids=@();acceptance='open'}
}
$expectedTasks = @()
foreach ($pair in @(@('P',18),@('C',12),@('U',11),@('G',5),@('T',3),@('R',5),@('V',6))) {
    for ($i=1;$i -le $pair[1];$i++) { $expectedTasks += '{0}{1:D2}' -f $pair[0],$i }
}
Require (@(Compare-Object $expectedTasks @($tasks.Keys)).Count -eq 0) '60项任务编号缺失或变更'

$featureMap = @{}
foreach ($f in $registry.features) {
    Require (-not $featureMap.ContainsKey($f.id)) "重复功能ID：$($f.id)"
    $featureMap[$f.id] = $f
    Require ($f.kind -in @('domain','flow')) "$($f.id) 功能类型无效"
    Require ($f.scope -in @('included','paused','excluded')) "$($f.id) 范围无效"
    Require (@($f.baseline_documents).Count -gt 0) "$($f.id) 缺基线合同"
    Require (-not [string]::IsNullOrWhiteSpace($f.acceptance_focus)) "$($f.id) 缺验收边界"
    foreach ($p in @($f.baseline_documents)+@($f.decision_refs)) { Check-Path $p $f.id }
    if ($f.scope -eq 'excluded') { Require ($f.decision_refs.Count -gt 0) "$($f.id) 排除缺授权" }
    foreach ($id in $f.original_inventory_ids) {
        Require ($functions.ContainsKey($id)) "$($f.id) 未知原成员：$id"
        if ($functions.ContainsKey($id)) { $functions[$id].feature_memberships += $f.id }
    }
    Require ((Unique $f.original_inventory_ids).Count -eq $f.original_inventory_ids.Count) "$($f.id) 重复原成员"
    foreach ($id in $f.closure_task_ids) {
        Require ($tasks.ContainsKey($id)) "$($f.id) 未知任务：$id"
        if ($tasks.ContainsKey($id)) { $tasks[$id].feature_ids += $f.id }
    }
}
foreach ($f in $registry.features) {
    if ($f.parent_id) { Require ($featureMap.ContainsKey($f.parent_id) -and $featureMap[$f.parent_id].kind -eq 'domain') "$($f.id) 父域不存在/类型不符" }
    foreach ($dep in $f.dependencies) {
        Require ($featureMap.ContainsKey($dep) -and $dep -ne $f.id -and $featureMap[$dep].kind -eq 'flow') "$($f.id) 无效依赖：$dep"
    }
}
$visiting=@{}; $visited=@{}
function Visit-Feature([string]$id) {
    if ($visited.ContainsKey($id) -or -not $featureMap.ContainsKey($id)) { return }
    if ($visiting.ContainsKey($id)) { $errors.Add("循环功能依赖：$id"); return }
    $visiting[$id]=$true
    foreach ($dep in $featureMap[$id].dependencies) { Visit-Feature $dep }
    $visiting.Remove($id); $visited[$id]=$true
}
foreach ($id in @($featureMap.Keys)) { Visit-Feature $id }

$evidenceMap=@{}
$gateOrder=@('B','I','W','S','R')
foreach ($e in $registry.evidence) {
    Require (-not $evidenceMap.ContainsKey($e.id)) "重复证据：$($e.id)"
    $evidenceMap[$e.id]=$e
    Require ($featureMap.ContainsKey($e.feature_id)) "$($e.id) 未知功能"
    Require ($e.gate -in $gateOrder) "$($e.id) 未知门禁"
    foreach ($field in @('id','epoch','baseline_reference','implementation_reference','method','observation','expected','actual','result','recorded_on')) {
        Require (-not [string]::IsNullOrWhiteSpace([string]$e[$field])) "$($e.id) 缺字段：$field"
    }
    foreach ($field in @('baseline_reference','implementation_reference')) {
        Require ($e[$field] -notin @('HEAD','最新版','同上','当前')) "$($e.id) 版本定位不可追溯"
    }
    Require ($e.method -in @('static','runtime')) "$($e.id) 无效证据方式"
    Require ($e.result -in @('pass','fail')) "$($e.id) 无效证据结果"
    Require ($e.Contains('differences')) "$($e.id) 缺差异声明"
    Require (@($e.artifacts).Count -gt 0) "$($e.id) 缺证据附件"
    foreach ($p in $e.artifacts) { Check-Path $p $e.id }
    foreach ($difference in @($e.differences)) {
        if ($difference) { Require (-not [string]::IsNullOrWhiteSpace($difference.description)) "$($e.id) 差异缺说明"; Check-Path $difference.decision_ref $e.id }
    }
    if ($e.gate -eq 'R') {
        Require ($e.method -eq 'runtime') "$($e.id) 静态证据不能满足R"
        foreach ($field in @('environment_reference','input_reference','output_reference')) { Check-Path $e[$field] $e.id }
    }
}
$documentMembers=@{}
foreach ($fid in $functions.Keys) {
    foreach ($doc in (Unique $functions[$fid].baseline_documents)) {
        if (-not $documentMembers.ContainsKey($doc)) { $documentMembers[$doc]=@() }
        $documentMembers[$doc]+=$fid
    }
}
$gateCounts=[ordered]@{B=0;I=0;W=0;S=0;R=0}
$accepted=@{}
$featureRows=[System.Collections.Generic.List[object]]::new()
foreach ($f in $registry.features) {
    $states=[ordered]@{}
    foreach ($unknown in $f.gates.Keys) { Require ($unknown -in $gateOrder) "$($f.id) 未知门禁键：$unknown" }
    for ($i=0;$i -lt $gateOrder.Count;$i++) {
        $g=$gateOrder[$i]; $state=Gate-State $f $g; $states[$g]=$state
        Require ($state -in @('unassessed','blocked','pass','fail')) "$($f.id)/$g 无效状态"
        if ($state -eq 'fail') {
            $proof=@($f.gates[$g].evidence_ids)
            Require ($proof.Count -gt 0) "$($f.id)/$g 失败结论缺证据"
            foreach ($eid in $proof) {
                Require ($evidenceMap.ContainsKey($eid)) "$($f.id)/$g 缺失败证据$eid"
                if ($evidenceMap.ContainsKey($eid)) {
                    $e=$evidenceMap[$eid]
                    Require ($e.feature_id -eq $f.id -and $e.gate -eq $g -and $e.epoch -eq $registry.epoch -and $e.result -eq 'fail') "$($f.id)/$g 失败证据不匹配"
                }
            }
        }
        if ($state -eq 'pass') {
            Require ($f.kind -eq 'flow' -and $f.scope -eq 'included') "$($f.id)/$g 域/暂停/排除不得通过"
            Require ($f.original_inventory_ids.Count -gt 0) "$($f.id)/$g 缺明确原成员"
            Require ($f.member_inventory_closed -eq $true -and @($f.member_inventory_evidence_refs).Count -gt 0) "$($f.id)/$g 原成员枚举未闭合"
            foreach ($p in $f.member_inventory_evidence_refs) { Check-Path $p $f.id }
            if ($g -eq 'R') { Require $registry.execution_policy.runtime_allowed "$($f.id) 运行尚未恢复" }
            for ($j=0;$j -lt $i;$j++) { Require ($states[$gateOrder[$j]] -eq 'pass') "$($f.id)/$g 前置门禁未通过" }
            $proof=@($f.gates[$g].evidence_ids)
            Require ($proof.Count -gt 0) "$($f.id)/$g 缺验收证据"
            foreach ($eid in $proof) {
                Require ($evidenceMap.ContainsKey($eid)) "$($f.id)/$g 缺证据$eid"
                if (-not $evidenceMap.ContainsKey($eid)) { continue }
                $e=$evidenceMap[$eid]
                Require ($e.feature_id -eq $f.id -and $e.gate -eq $g -and $e.epoch -eq $registry.epoch -and $e.result -eq 'pass') "$($f.id)/$g 证据对象/门禁/版本/结果不符"
            }
            if ($f.kind -eq 'flow') { $gateCounts[$g]++ }
        }
    }
    $isAccepted=($f.kind -eq 'flow' -and $f.scope -eq 'included' -and @($states.Values|Where-Object {$_ -ne 'pass'}).Count -eq 0 -and $f.blockers.Count -eq 0)
    $accepted[$f.id]=$isAccepted
    $candidates=Unique @($f.baseline_documents | ForEach-Object { if ($documentMembers.ContainsKey($_)) { $documentMembers[$_] } })
    $featureRows.Add([ordered]@{candidate_function_ids=@($candidates);id=$f.id;title=$f.title;kind=$f.kind;scope=$f.scope;baseline_documents=$f.baseline_documents;original_inventory_ids=$f.original_inventory_ids;closure_task_ids=$f.closure_task_ids;dependencies=$f.dependencies;gates=$states;accepted=$isAccepted;blockers=$f.blockers})
}
foreach ($f in $registry.features) {
    if ($accepted[$f.id]) {
        foreach ($dep in $f.dependencies) { Require ($accepted[$dep]) "$($f.id) 验收依赖未闭合：$dep" }
    }
}
$dispositionIds=@{}
foreach ($d in @($registry.function_dispositions)) {
    if (-not $d) { continue }
    Require ($functions.ContainsKey($d.inventory_id) -and -not $dispositionIds.ContainsKey($d.inventory_id)) "无效/重复处置ID：$($d.inventory_id)"
    Require ($d.disposition -in @('equivalent','adapted','host','excluded','mixed','merged')) "$($d.inventory_id) 无效处置"
    Require ($d.epoch -eq $registry.epoch -and @($d.evidence_refs).Count -gt 0) "$($d.inventory_id) 处置证据未闭合"
    foreach ($p in $d.evidence_refs) { Check-Path $p $d.inventory_id }
    if ($d.disposition -eq 'excluded') { Require ($declaredExcluded.ContainsKey($d.inventory_id) -and -not $declaredMixed.ContainsKey($d.inventory_id)) "$($d.inventory_id) 不在完整入口具名排除集合" }
    $dispositionIds[$d.inventory_id]=$true
    if ($functions.ContainsKey($d.inventory_id)) { $functions[$d.inventory_id].disposition=$d }
}
$taskAcceptanceIds=@{}
foreach ($t in $registry.task_acceptance) {
    Require ($tasks.ContainsKey($t.task_id) -and -not $taskAcceptanceIds.ContainsKey($t.task_id)) "无效/重复任务验收：$($t.task_id)"
    $taskAcceptanceIds[$t.task_id]=$true
    Require ($t.state -in @('open','closed')) "$($t.task_id) 任务状态无效"
    if ($t.state -eq 'closed') {
        Require ($t.epoch -eq $registry.epoch -and @($t.evidence_refs).Count -gt 0) "$($t.task_id) 任务缺有效证据"
        foreach ($p in $t.evidence_refs) { Check-Path $p $t.task_id }
    }
    if ($tasks.ContainsKey($t.task_id)) { $tasks[$t.task_id].acceptance=$t.state }
}
$closureNames=@('function_classification','feature_enumeration','data_and_indirect_targets','unassigned_ranges','scope_exceptions','final_acceptance')
Require (@(Compare-Object $closureNames @($registry.closures.Keys)).Count -eq 0) '闭包项缺失或未知'
foreach ($name in $closureNames) {
    $c=$registry.closures[$name]
    Require ($c.state -in @('open','closed')) "闭包状态无效：$name"
    if ($c.state -eq 'closed') {
        Require (@($c.evidence_refs).Count -gt 0 -and -not [string]::IsNullOrWhiteSpace($c.reason)) "闭包缺依据：$name"
        foreach ($p in $c.evidence_refs) { Check-Path $p $name }
    }
}
if ($registry.closures.function_classification.state -eq 'closed') {
    Require ($dispositionIds.Count -eq $functions.Count) '函数分类闭包仍缺逐ID处置'
}

$functionRows=@($functions.Keys | Sort-Object | ForEach-Object {
    $f=$functions[$_]; $f.baseline_documents=Unique $f.baseline_documents; $f.feature_memberships=Unique $f.feature_memberships
    $f
})
$imports=@(foreach ($module in $inventory.imports) { foreach ($e in $module.entries) {
    [ordered]@{id=('I:{0:X8}' -f $e.rva);module=$module.name;name=$e.name;ordinal=$e.ordinal;state='unassessed';source='doc/cg2d_ida_inventory.json'}
}})
Require ($imports.Count -eq $registry.inventory_expected.imports -and $imports.Count -eq $inventory.counts.import_count) '导入分母变化须登记依据'
Require ((Unique @($imports|ForEach-Object id)).Count -eq $imports.Count) '导入ID重复'
$ranges=@(foreach ($r in $inventory.unassigned_executable_ranges) {
    [ordered]@{id=('X:{0:X8}-{1:X8}' -f ($r[0]-$inventory.baseline.imagebase),($r[1]-$inventory.baseline.imagebase));start=('0x{0:X}' -f $r[0]);end_exclusive=('0x{0:X}' -f $r[1]);state='unassessed';source='doc/cg2d_ida_inventory.json'}
})
Require ((Unique @($ranges|ForEach-Object id)).Count -eq $ranges.Count) '区间ID重复'
$auxiliary=@{}
foreach ($row in @($imports)+@($ranges)) { $auxiliary[$row.id]=$row; $row['disposition']=$null }
$auxSeen=@{}
foreach ($d in @($registry.auxiliary_dispositions)) {
    if (-not $d) { continue }
    Require ($auxiliary.ContainsKey($d.id) -and -not $auxSeen.ContainsKey($d.id)) "未知/重复辅助身份：$($d.id)"
    Require ($d.epoch -eq $registry.epoch -and $d.disposition -in @('mapped','excluded','merged') -and @($d.evidence_refs).Count -gt 0) "$($d.id) 辅助处置缺当前证据"
    foreach ($p in $d.evidence_refs) { Check-Path $p $d.id }
    $auxSeen[$d.id]=$true
    if ($auxiliary.ContainsKey($d.id)) { $auxiliary[$d.id]['disposition']=$d; $auxiliary[$d.id]['state']='reviewed' }
}
$reviewedImports=@($imports|Where-Object state -eq 'reviewed').Count
$reviewedRanges=@($ranges|Where-Object state -eq 'reviewed').Count
if ($registry.closures.data_and_indirect_targets.state -eq 'closed') { Require ($reviewedImports -eq $imports.Count) '数据/间接目标闭包仍缺导入槽处置' }
if ($registry.closures.unassigned_ranges.state -eq 'closed') { Require ($reviewedRanges -eq $ranges.Count) '区间闭包仍缺逐区间处置' }
$leaf=@($featureRows|Where-Object {$_.kind -eq 'flow' -and $_.scope -ne 'excluded'})
$acceptedCount=@($leaf|Where-Object accepted).Count
$globalReady=(@($registry.closures.Values|Where-Object state -ne 'closed').Count -eq 0 -and $leaf.Count -gt 0 -and $acceptedCount -eq $leaf.Count -and @($tasks.Values|Where-Object acceptance -ne 'closed').Count -eq 0)
if ($registry.closures.final_acceptance.state -eq 'closed') { Require $globalReady '整体验收声明不满足闭包、功能或60任务门禁' }
$linked=@($functionRows|Where-Object {$_.baseline_documents.Count -gt 0}).Count
$metrics=[ordered]@{
    inventory_functions=$functions.Count
    functions_with_document_links=$linked
    document_link_percent=(Percent $linked $functions.Count)
    functions_without_document_links=@($functionRows|Where-Object {$_.baseline_documents.Count -eq 0}|ForEach-Object id)
    historical_direct_location_ids=@($functionRows|Where-Object {$_.direct_snapshot_definitions.Count -gt 0}).Count
    semantic_records=$semantic.records.Count
    semantic_inventory_ids=@($functionRows|Where-Object {$_.semantic_indices.Count -gt 0}).Count
    semantic_without_inventory_id=$semanticWithoutId.Count
    pending_records=$semantic.pending_records.Count
    functions_with_classification_refs=@($functionRows|Where-Object {$_.classification_refs.Count -gt 0}).Count
    declared_excluded_ids=@($declaredExcluded.Keys|Sort-Object);declared_mixed_ids=@($declaredMixed.Keys|Sort-Object)
    reviewed_function_dispositions=$dispositionIds.Count
    imports=$imports.Count;unassigned_ranges=$ranges.Count;reviewed_imports=$reviewedImports;reviewed_ranges=$reviewedRanges
    registered_features=$featureRows.Count;registered_leaf_features=$leaf.Count
    paused_features=@($leaf|Where-Object scope -eq 'paused').Count
    gates_pass=$gateCounts;accepted_leaf_features=$acceptedCount
    tasks=$tasks.Count;tasks_closed=@($tasks.Values|Where-Object acceptance -eq 'closed').Count
    full_feature_denominator=$(if ($registry.closures.feature_enumeration.state -eq 'closed') { $leaf.Count } else { $null });global_accepted=$globalReady
}
if ($errors.Count -gt 0) {
    foreach ($message in $errors) { Write-Error $message -ErrorAction Continue }
    throw "静态台账校验失败：$($errors.Count)项；未写派生文件。"
}
$snapshot=[ordered]@{
    schema_version=1;epoch=$registry.epoch;catalog_revision=$registry.catalog_revision
    limitation='文件/身份关联与验收登记快照，不证明行为等价；直接定位为历史快照。导入/未归属区间尚未逐项重认定。'
    metrics=$metrics;closures=$registry.closures
    functions=$functionRows;semantic_without_inventory_id=@($semanticWithoutId)
    imports=$imports;unassigned_ranges=$ranges;classification_sources=@($classifications)
    features=@($featureRows);tasks=@($tasks.Keys|Sort-Object|ForEach-Object {$tasks[$_]})
}
# 每个身份/功能一行，避免派生快照制造十万行缩进差异；内容仍为标准JSON。
$jsonLines=[System.Collections.Generic.List[string]]::new()
$jsonLines.Add('{')
$keys=@($snapshot.Keys)
for ($ki=0;$ki -lt $keys.Count;$ki++) {
    $key=$keys[$ki]; $suffix=if ($ki -lt $keys.Count-1) {','} else {''}
    if ($key -in @('functions','imports','unassigned_ranges','features','tasks')) {
        $rows=@($snapshot[$key])
        $jsonLines.Add('  "'+$key+'": [')
        for ($ri=0;$ri -lt $rows.Count;$ri++) {
            $rowSuffix=if ($ri -lt $rows.Count-1) {','} else {''}
            $jsonLines.Add('    '+(ConvertTo-Json -InputObject $rows[$ri] -Depth 40 -Compress)+$rowSuffix)
        }
        $jsonLines.Add('  ]'+$suffix)
    } else {
        $jsonLines.Add('  "'+$key+'": '+(ConvertTo-Json -InputObject $snapshot[$key] -Depth 40 -Compress)+$suffix)
    }
}
$jsonLines.Add('}')
$json=($jsonLines -join [char]10)+[char]10
$lines=[System.Collections.Generic.List[string]]::new()
$lines.Add('# cg2d 度量与验收看板')
$lines.Add('')
$lines.Add('> 自动生成，禁止手改。入口：[体系SPEC](../doc/cg2d_traceability_acceptance_spec.md) · [登记簿](cg2d_acceptance_registry.json) · [完整快照](cg2d_traceability_snapshot.json) · [唯一进度](cg2d_full_reconstruction_progress.md)。')
$denominatorState=if ($null -eq $metrics.full_feature_denominator) {'全功能分母未闭合'} else {'已冻结功能分母：'+$metrics.full_feature_denominator}
$overallState=if ($globalReady) {'已登记整体验收通过，证据见登记簿'} else {'整体未验收'}
$lines.Add("> 验收批次：$($registry.epoch)；目录版本：$($registry.catalog_revision)。$denominatorState；$overallState。门禁通过数只统计本批归档证据，不等于原有实现数量。")
$lines.Add('')
$lines.Add('## 1. 分母与可计算指标')
$lines.Add('')
$lines.Add('| 指标 | 当前值 | 含义/限制 |')
$lines.Add('|---|---:|---|')
$lines.Add("| 原函数纳管 | $($functions.Count)/$($inventory.functions.Count) | 仅库存身份完整 |")
$lines.Add("| 合同文件关联 | $linked/$($functions.Count)（$($metrics.document_link_percent)%） | 文件关联率，不是功能/基线验收完成率 |")
$lines.Add("| 尚无合同关联 | $($functions.Count-$linked) | 完整ID见快照metrics.functions_without_document_links；可能已有局部合同尚未机器登记 |")
$lines.Add("| 历史源码定位 | $($metrics.historical_direct_location_ids) | 自动定位快照，非当前实现数 |")
$lines.Add("| 语义登记 | $($semantic.records.Count)条/$($metrics.semantic_inventory_ids)个原ID | 无独立ID记录另列$($semanticWithoutId.Count)条 |")
$lines.Add("| 具名分类引用 | $($metrics.functions_with_classification_refs)个原ID | 含排除/混合/合并，不计为实现 |")
$lines.Add("| 已具名全入口排除 / 混合入口 | $($declaredExcluded.Count) / $($declaredMixed.Count) | 不计为实现/验收；混合入口不得整体扣除 |")
$lines.Add("| 已审核函数处置 | $($dispositionIds.Count)/$($functions.Count) | 初始仅导入关系，未逐ID重新认定 |")
$lines.Add("| 导入槽 / 未归属区间 | $($imports.Count) / $($ranges.Count) | 全部保留稳定ID；已审处置$reviewedImports / $reviewedRanges |")
$lines.Add("| 已登记叶功能 | $($leaf.Count) | 非全功能分母；域不重复计入，暂停保留 |")
$lines.Add("| 已验收叶功能 | $acceptedCount/$($leaf.Count) | 仅登记集合，不能代表整体完成率 |")
$lines.Add("| 已知收尾任务 | $($tasks.Count)；验收关闭$($metrics.tasks_closed) | 任务不是全功能目录 |")
$lines.Add('')
$lines.Add('## 2. 门禁与闭包')
$lines.Add('')
$lines.Add('| B基线 | I实现 | W接线/寿命 | S静态对照 | R运行 |')
$lines.Add('|---:|---:|---:|---:|---:|')
$lines.Add("| $($gateCounts.B)/$($leaf.Count) | $($gateCounts.I)/$($leaf.Count) | $($gateCounts.W)/$($leaf.Count) | $($gateCounts.S)/$($leaf.Count) | $($gateCounts.R)/$($leaf.Count) |")
$lines.Add('')
foreach ($name in $closureNames) { $lines.Add("- $name：$($registry.closures[$name].state)；$(Cell $registry.closures[$name].reason)") }
$lines.Add('')
$lines.Add('## 3. 功能追踪矩阵')
$lines.Add('')
$lines.Add('unassessed=未按门禁归档；blocked=执行限制/阻碍；pass/fail=有具名证据的结果。合同存在不等于基线完整。依赖为验收前提，不作为运行架构或调用关系的新事实。同合同候选只按明确文档引用联结，可能含共享/其他功能，必须人工核原成员。')
$lines.Add('')
$lines.Add('| 功能ID | 范围/名称 | 合同 | 已审/同合同候选成员 | 任务 | B/I/W/S/R |')
$lines.Add('|---|---|---|---:|---|---|')
foreach ($f in $featureRows) {
    $docLinks=@($f.baseline_documents|ForEach-Object {'['+[System.IO.Path]::GetFileNameWithoutExtension($_)+'](../'+$_+')'}) -join '、'
    $state=@($gateOrder|ForEach-Object {$f.gates[$_]}) -join '/'
    $lines.Add("| $($f.id) | $($f.scope)：$(Cell $f.title) | $docLinks | $($f.original_inventory_ids.Count)/$($f.candidate_function_ids.Count) | $($f.closure_task_ids -join '、') | $state |")
}
$lines.Add('')
$lines.Add('## 4. 当前推进规则')
$lines.Add('')
$lines.Add('先从已有合同补原成员和B证据，再逐项推进I/W/S；R保持blocked。优先处理已知P/C/U任务及无合同关联ID的归属，不把历史定位缺失当作未实现。原格式明文、音频暂停、无构建/测试/运行/摘要限制保持。')
$lines.Add('本看板只汇总结构化登记；变更后需做人工影响分析。静态汇总器退出0不等于软件验收通过。')
$dashboard=($lines -join [char]10)+[char]10
$outputs=[ordered]@{'plan/cg2d_traceability_snapshot.json'=$json;'plan/cg2d_measurement_dashboard.md'=$dashboard}
foreach ($path in $outputs.Keys) {
    $full=Join-Path $projectRoot $path
    if ($Check) {
        if (-not (Test-Path -LiteralPath $full) -or [System.IO.File]::ReadAllText($full,$utf8) -cne $outputs[$path]) { throw "派生文件过期：$path；请先更新汇总。" }
    } else { [System.IO.File]::WriteAllText($full,$outputs[$path],$utf8) }
}
Write-Output "静态台账校验通过：$($functions.Count)函数、$($imports.Count)导入、$($ranges.Count)区间、$($leaf.Count)叶功能、$($tasks.Count)任务；验收结论请读看板。"
