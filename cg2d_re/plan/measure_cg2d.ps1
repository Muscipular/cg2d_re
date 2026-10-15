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
    $currentSemanticSource = $owner -match '^doc/cg2d_semantic_function_mappings\.json#/(records|pending_records)/'
    $pathCacheKey = if ($currentSemanticSource) { 'current:' + $path } else { 'attachment:' + $path }
    if ($pathCache.ContainsKey($pathCacheKey)) { return }
    $target = [System.IO.Path]::GetFullPath((Join-Path $projectRoot ($path -split '#',2)[0]))
    if (-not $target.StartsWith($projectRoot + [System.IO.Path]::DirectorySeparatorChar, [System.StringComparison]::OrdinalIgnoreCase)) {
        $errors.Add("$owner 路径越出本线：$path"); return
    }
    $exists = Test-Path -LiteralPath $target -PathType Leaf
    # MOP01：仅历史附件按固定退休提交的直接前态读取，白名单/旧证据保持，不依赖tag。
    $relativePath = ($path -split '#',2)[0]
    $historicalAsm = @('src/platform/LegacyAudioMath.asm',
        'src/platform/MapEffectMath.asm', 'src/platform/LegacyFloatEnvironment.asm')
    if (-not $exists -and $relativePath -cin $historicalAsm -and -not $currentSemanticSource) {
        & git -C $projectRoot cat-file -e ("HEAD^{/^\[#\] 按标准数学近似合同将音频ASM迁为CPP([[:space:]]|$)}^:cg2d_re/" + $relativePath) 2>$null
        $exists = $LASTEXITCODE -eq 0
    }
    # MC01-C1：六私有C++库/NCE头退休；仅历史附件可读前态，当前语义source不得回退。
    $historicalCpp = @('src/platform/LegacyNarrowString.cpp', 'src/platform/LegacyNarrowString.hpp',
        'src/platform/LegacyStreamBuffer.cpp', 'src/platform/LegacyStreamBuffer.hpp',
        'src/platform/LegacyFileBuffer.cpp', 'src/platform/LegacyFileBuffer.hpp',
        'src/platform/LegacyIos.cpp', 'src/platform/LegacyIos.hpp',
        'src/platform/LegacyLocale.cpp', 'src/platform/LegacyLocale.hpp',
        'src/platform/LegacyException.cpp', 'src/platform/LegacyException.hpp',
        'src/platform/NativeCppException.hpp')
    if (-not $exists -and $relativePath -cin $historicalCpp -and -not $currentSemanticSource) {
        & git -C $projectRoot cat-file -e ("HEAD^{/^\[#\] 将C[+][+]私有库对象收敛到MSVC标准库([[:space:]]|$)}^:cg2d_re/" + $relativePath) 2>$null
        $exists = $LASTEXITCODE -eq 0
    }
    # MC01-C2：浮点文本/数值/环境组仅保历史附件；真实业务读取已迁既有模块。
    $historicalNumeric = @('src/platform/LegacyFloatFormat.cpp', 'src/platform/LegacyFloatFormat.hpp',
        'src/platform/LegacyFloatNumeric.cpp', 'src/platform/LegacyFloatNumeric.hpp',
        'src/platform/LegacyFloatEnvironment.cpp', 'src/platform/LegacyFloatEnvironment.hpp',
        'src/platform/LegacyFloatMath.cpp')
    if (-not $exists -and $relativePath -cin $historicalNumeric -and -not $currentSemanticSource) {
        & git -C $projectRoot cat-file -e ("HEAD^{/^\[#\] 将浮点格式与数值私有职责交由MSVC承接([[:space:]]|$)}^:cg2d_re/" + $relativePath) 2>$null
        $exists = $LASTEXITCODE -eq 0
    }
    # MC01-C3：旧信号表/raise/filter退休，历史附件独立于当前source校验。
    $historicalSignal = @('src/platform/LegacySignal.cpp', 'src/platform/LegacySignal.hpp')
    if (-not $exists -and $relativePath -cin $historicalSignal -and -not $currentSemanticSource) {
        & git -C $projectRoot cat-file -e ("HEAD^{/^\[#\] 将错误信号与私有退出设施收敛到MSVC([[:space:]]|$)}^:cg2d_re/" + $relativePath) 2>$null
        $exists = $LASTEXITCODE -eq 0
    }
    # MC01-C4：私有Heap包装/模式/handler退休，历史附件核固定前态，当前source禁回退。
    $historicalHeap = @('src/platform/LegacyHeap.cpp', 'src/platform/LegacyHeap.hpp')
    if (-not $exists -and $relativePath -cin $historicalHeap -and -not $currentSemanticSource) {
        & git -C $projectRoot cat-file -e ("HEAD^{/^\[#\] 将通用堆策略与初始化收敛到MSVC([[:space:]]|$)}^:cg2d_re/" + $relativePath) 2>$null
        $exists = $LASTEXITCODE -eq 0
    }
    # MC01-C5：旧多字节/NLS/ctype存储退休，仅历史附件可核前态，当前source禁回退。
    $historicalMultibyte = @('src/platform/LegacyMultibyte.cpp', 'src/platform/LegacyMultibyte.hpp',
        'src/platform/LegacyStringCompareState.hpp')
    if (-not $exists -and $relativePath -cin $historicalMultibyte -and -not $currentSemanticSource) {
        & git -C $projectRoot cat-file -e ("HEAD^{/^\[#\] 将多字节与私有参数准备设施收敛到MSVC([[:space:]]|$)}^:cg2d_re/" + $relativePath) 2>$null
        $exists = $LASTEXITCODE -eq 0
    }
    # DCS01：比较包装退休；历史附件核格式化迁移提交中的固定前态，当前语义source禁回退。
    $historicalStringCompare = @('src/platform/LegacyStringCompare.cpp', 'include/cg/platform/LegacyStringCompare.hpp')
    if (-not $exists -and $relativePath -cin $historicalStringCompare -and -not $currentSemanticSource) {
        & git -C $projectRoot cat-file -e ("HEAD^{/^\[#\] 格式化与扫描直接使用MSVC CRT([[:space:]]|$)}:cg2d_re/" + $relativePath) 2>$null
        $exists = $LASTEXITCODE -eq 0
    }
    # DLI01／DRT01：仅退休附件按商店命令清理后的固定前态读取，当前source禁回退。
    $historicalIoRuntime = @('src/platform/LegacyLowIo.cpp', 'src/platform/LegacyLowIo.hpp',
        'src/platform/LegacyRuntimeError.cpp', 'src/platform/LegacyRuntimeError.hpp')
    if (-not $exists -and $relativePath -cin $historicalIoRuntime -and -not $currentSemanticSource) {
        & git -C $projectRoot cat-file -e ("HEAD^{/^\[#\] 简化商店命令读取并修正取消项范围([[:space:]]|$)}:cg2d_re/" + $relativePath) 2>$null
        $exists = $LASTEXITCODE -eq 0
    }
    # DSI01：标准文件流包装退休；仅历史附件可读取上一包装清理提交中的前态。
    if (-not $exists -and $relativePath -ceq 'src/platform/LegacyStdio.cpp' -and -not $currentSemanticSource) {
        & git -C $projectRoot cat-file -e ("HEAD^{/^\[#\] 删除描述符和运行时终止的冗余包装([[:space:]]|$)}:cg2d_re/" + $relativePath) 2>$null
        $exists = $LASTEXITCODE -eq 0
    }
    # LWR01：旧目录处理器已移除；历史附件只核固定追踪提交，当前source不得回退。
    if (-not $exists -and $relativePath -ceq 'src/LegacyCatalogWindows.cpp' -and -not $currentSemanticSource) {
        & git -C $projectRoot cat-file -e ("HEAD^{/^\[#\] 追踪旧窗口分发并同步语义函数名([[:space:]]|$)}:cg2d_re/" + $relativePath) 2>$null
        $exists = $LASTEXITCODE -eq 0
    }
    # LWR02：旧显示设置空壳退休；仅历史附件核固定归位提交，当前source不得回退。
    $historicalDisplaySettings = @('src/DisplaySettings.cpp', 'include/cg/DisplaySettings.hpp')
    if (-not $exists -and $relativePath -cin $historicalDisplaySettings -and -not $currentSemanticSource) {
        & git -C $projectRoot cat-file -e ("HEAD^{/^\[#\] 将单窗口类型与状态收归实现文件([[:space:]]|$)}:cg2d_re/" + $relativePath) 2>$null
        $exists = $LASTEXITCODE -eq 0
    }
    # LWR03：旧选择与专属面板头退休；只有历史附件允许读取固定归位前态。
    $historicalLegacySelection = @('src/ui/LegacyWindowSelection.cpp', 'include/cg/ui/LegacyWindowSelection.hpp', 'src/LegacyPanelRuntime.hpp', 'src/LegacyPanelTexts.hpp')
    if (-not $exists -and $relativePath -cin $historicalLegacySelection -and -not $currentSemanticSource) {
        & git -C $projectRoot cat-file -e ("HEAD^{/^\[#\] 将单窗口类型与状态收归实现文件([[:space:]]|$)}:cg2d_re/" + $relativePath) 2>$null
        $exists = $LASTEXITCODE -eq 0
    }
    Require $exists "$owner 引用不存在：$path"
    $pathCache[$pathCacheKey] = $true
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
$originalFunctionById = @{} # MPL01只用于核原元数据范围，不进入源码映射或改变分母。
foreach ($f in $inventory.functions) {
    Require (-not $functions.ContainsKey($f.id)) "重复原函数ID：$($f.id)"
    $originalFunctionById[$f.id] = $f
    $functions[$f.id] = [ordered]@{
        id=$f.id; original_entry=('0x{0:X}' -f $f.va); original_name=$f.name
        semantic_indices=@(); pending_indices=@(); direct_snapshot_definitions=@()
        baseline_documents=@(); classification_refs=@(); disposition=$null
        feature_memberships=@(); sources=@()
    }
}
$unassignedRangeById=@{}
foreach ($bounds in $inventory.unassigned_executable_ranges) {
    $rangeId='X:{0:X8}-{1:X8}' -f ($bounds[0]-$inventory.baseline.imagebase),($bounds[1]-$inventory.baseline.imagebase)
    $unassignedRangeById[$rangeId]=$bounds
}
$semanticWithoutId = [System.Collections.Generic.List[object]]::new()
foreach ($section in @('records','pending_records')) {
    for ($i=0; $i -lt $semantic[$section].Count; $i++) {
        $r = $semantic[$section][$i]
        $ref = "doc/cg2d_semantic_function_mappings.json#/$section/$i"
        foreach ($p in @($r.evidence_documents)) { if ($p) { Check-Path $p $ref } }
        if (-not $r.inventory_id) {
            $row=[ordered]@{reference=$ref;original_entry=$r.original_entry;status=$r.status}
            if ($r.code_identity) {
                $identity=$r.code_identity
                $shapeValid=$identity.kind -eq 'unassigned_range_fragment' -and $unassignedRangeById.ContainsKey([string]$identity.inventory_range_id) -and $identity.start -match '^0x[0-9A-Fa-f]+$' -and $identity.end_exclusive -match '^0x[0-9A-Fa-f]+$'
                Require $shapeValid "$ref 非函数代码身份形态/原区间无效"
                if ($shapeValid) {
                    $bounds=$unassignedRangeById[$identity.inventory_range_id]
                    $start=[Convert]::ToInt64($identity.start.Substring(2),16)
                    $end=[Convert]::ToInt64($identity.end_exclusive.Substring(2),16)
                    Require ($start -ge $bounds[0] -and $end -le $bounds[1] -and $start -lt $end) "$ref 代码片段越出原库存区间"
                    Require ($identity.start -eq $r.original_entry -and $identity.end_exclusive -eq $r.original_end_exclusive) "$ref 代码身份与原入口/边界不符"
                }
                # LWR03：4FC730无独立F身份，已随旧handler退休；核原身份和历史来源，不伪造当前正文。
                $retiredLegacyFragment = $r.original_entry -eq '0x4fc730' -and $r.scope_disposition -eq 'excluded' -and $r.retirement_spec -eq 'doc/cg2d_legacy_window_retirement_spec.md#legacy-window-simplification-lwr03' -and @($r.historical_sources).Count -gt 0
                Require (@($identity.evidence_refs).Count -gt 0 -and (@($r.sources).Count -gt 0 -or $retiredLegacyFragment)) "$ref 代码身份缺原证据或源码关联"
                if ($retiredLegacyFragment) {
                    Check-Path $r.retirement_spec "$ref 历史排除"
                    $row['scope_disposition']='excluded'
                }
                foreach ($path in @($identity.evidence_refs)) { Check-Path $path "$ref 代码身份" }
                $row['code_identity']=$identity
            }
            $semanticWithoutId.Add($row)
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
$semanticCodeRangeCount=@($semanticWithoutId | Where-Object { $null -ne $_.code_identity }).Count
$unboundSemanticCount=$semanticWithoutId.Count-$semanticCodeRangeCount
foreach ($r in $direct.records) {
    Require ($functions.ContainsKey($r.inventory_id)) "直接定位未知ID：$($r.inventory_id)"
    if ($functions.ContainsKey($r.inventory_id)) {
        $functions[$r.inventory_id].direct_snapshot_definitions += @($r.definitions)
        foreach ($d in $r.definitions) { Check-Path $d.path '历史定位' }
    }
}
$classifications = [System.Collections.Generic.List[object]]::new()
$declaredExcluded=@{}; $declaredMixed=@{}
$classificationDataByPath=@{} # MPL01处置只可引用本次已纳管的权威边界记录。
foreach ($ref in $semantic.classification_references) {
    Check-Path $ref.path '分类引用'
    $data = Read-Json $ref.path
    $classificationDataByPath[$ref.path]=$data
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
    $featureRow=[ordered]@{candidate_function_ids=@($candidates);id=$f.id;title=$f.title;kind=$f.kind;scope=$f.scope;baseline_documents=$f.baseline_documents;original_inventory_ids=$f.original_inventory_ids;closure_task_ids=$f.closure_task_ids;dependencies=$f.dependencies;gates=$states;accepted=$isAccepted;blockers=$f.blockers}
    if ($f.Contains('resumed_original_inventory_ids') -and @($f.resumed_original_inventory_ids).Count -gt 0) {
        Require ($f.scope -eq 'paused' -and @($f.resumed_source_evidence_refs).Count -gt 0) "$($f.id) 局部恢复缺范围/证据"
        Require ((Unique $f.resumed_original_inventory_ids).Count -eq @($f.resumed_original_inventory_ids).Count) "$($f.id) 局部恢复ID重复"
        foreach ($id in $f.resumed_original_inventory_ids) { Require ($id -in $f.original_inventory_ids) "$($f.id) 局部恢复ID超出原成员：$id" }
        foreach ($ref in $f.resumed_source_evidence_refs) { Check-Path $ref $f.id }
        $featureRow['resumed_original_inventory_ids']=$f.resumed_original_inventory_ids
        $featureRow['resumed_source_evidence_refs']=$f.resumed_source_evidence_refs
    }
    $featureRows.Add($featureRow)
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
    Require ($d.disposition -in @('equivalent','adapted','host','excluded','mixed','merged','metadata_only')) "$($d.inventory_id) 无效处置"
    Require ($d.epoch -eq $registry.epoch -and @($d.evidence_refs).Count -gt 0) "$($d.inventory_id) 处置证据未闭合"
    foreach ($p in $d.evidence_refs) { Check-Path $p $d.inventory_id }
    if ($d.disposition -eq 'excluded') { Require ($declaredExcluded.ContainsKey($d.inventory_id) -and -not $declaredMixed.ContainsKey($d.inventory_id)) "$($d.inventory_id) 不在完整入口具名排除集合" }
    if ($d.disposition -eq 'metadata_only') {
        $referenceShape=[string]$d.metadata_reference -match '^(doc/cg2d_function_boundary_classifications\.json)#/records/([0-9]+)$'
        Require ($referenceShape -and $d.metadata_kind -eq 'alignment_padding' -and $d.evidence_refs -contains $d.metadata_reference) "$($d.inventory_id) 元数据处置缺确切NOP分类引用"
        Require (-not $declaredExcluded.ContainsKey($d.inventory_id) -and -not $declaredMixed.ContainsKey($d.inventory_id)) "$($d.inventory_id) 元数据处置与具名范围重叠"
        if ($referenceShape) {
            $path=$Matches[1]; $index=[int]$Matches[2]
            $proofAvailable=$classificationDataByPath.ContainsKey($path) -and $index -lt @($classificationDataByPath[$path].records).Count -and $originalFunctionById.ContainsKey($d.inventory_id)
            Require $proofAvailable "$($d.inventory_id) 元数据分类记录或原库存不存在"
            if ($proofAvailable) {
                $r=$classificationDataByPath[$path].records[$index]; $original=$originalFunctionById[$d.inventory_id]
                $addressShape=$r.original_entry -match '^0x[0-9A-Fa-f]+$' -and $r.original_end -match '^0x[0-9A-Fa-f]+$' -and $r.preceding_return -match '^0x[0-9A-Fa-f]+$' -and $r.next_entry -match '^0x[0-9A-Fa-f]+$'
                Require ($r.inventory_id -eq $d.inventory_id -and $r.status -eq '已确认五字节NOP填充区间' -and $addressShape) "$($d.inventory_id) 元数据分类身份/用途不符"
                if ($addressShape) {
                    $start=[Convert]::ToInt64($r.original_entry.Substring(2),16); $end=[Convert]::ToInt64($r.original_end.Substring(2),16)
                    $returnAddress=[Convert]::ToInt64($r.preceding_return.Substring(2),16); $nextEntry=[Convert]::ToInt64($r.next_entry.Substring(2),16)
                    $completePadding=$start -eq $original.va -and $end -eq $original.entry_chunk_end -and @($original.chunks).Count -eq 1 -and $original.chunks[0][0] -eq $start -and $original.chunks[0][1] -eq $end -and ($end-$start) -eq 5 -and @($r.bytes).Count -eq 5 -and @($r.bytes | Where-Object { $_ -ne 144 }).Count -eq 0 -and $returnAddress -lt $start -and $nextEntry -eq $end
                    Require $completePadding "$($d.inventory_id) 元数据处置未覆盖完整原NOP范围"
                }
                $f=$functions[$d.inventory_id]
                Require ($f.direct_snapshot_definitions.Count -eq 0 -and $f.semantic_indices.Count -eq 0 -and $f.pending_indices.Count -eq 0 -and $f.sources.Count -eq 0) "$($d.inventory_id) 元数据处置不能覆盖源码/语义实现"
            }
        }
    }
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
    $f=$functions[$_]; $f.baseline_documents=@(Unique $f.baseline_documents); $f.feature_memberships=@(Unique $f.feature_memberships)
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
# GQ01仅派生登记差集；历史定位空不等于当前源码不存在，不自动认定功能完成。
$featureScopeById=@{}
foreach ($feature in $featureRows) { $featureScopeById[$feature.id]=$feature.scope }
$gapRows=[System.Collections.Generic.List[object]]::new()
foreach ($function in $functionRows) {
    if ($function.direct_snapshot_definitions.Count -gt 0 -or $function.semantic_indices.Count -gt 0 -or $function.pending_indices.Count -gt 0 -or $function.sources.Count -gt 0) { continue }
    $memberIds=@($function.feature_memberships | Where-Object { $_ })
    $kind='mapping_location_unrecorded'
    $scope='included_unassigned'
    $next='先核具名范围、当前源码与原身份，确认真实缺口后开发或迁移'
    if ($declaredExcluded.ContainsKey($function.id)) {
        $scope='excluded'; $next='核具名排除接线，本体保持排除'
    } elseif ($declaredMixed.ContainsKey($function.id)) {
        $scope='mixed'; $next='核保留分支及其实际接线，不整体排除'
    } elseif ($function.disposition.disposition -eq 'metadata_only') {
        $kind='metadata_padding'; $scope='metadata_only'; $next='已按完整NOP范围处置；保留原库存/分类依据，不生成空函数，相邻入口及未知alias另跟踪'
    } elseif ($memberIds.Count -gt 0 -and @($memberIds | Where-Object { $featureScopeById[$_] -eq 'paused' }).Count -eq $memberIds.Count) {
        $scope='paused'; $next='保留未闭缺口，恢复本项后继续'
    } elseif ($function.disposition.disposition -eq 'host') {
        $scope='host'; $next='核实际宿主提供者、兼容字段与活动接线'
    }
    $receiptEvidence=@($function.baseline_documents | Where-Object { $_ })
    if ($scope -eq 'metadata_only') { $receiptEvidence=Unique (@($receiptEvidence)+@($function.disposition.evidence_refs)) }
    $gapRows.Add([ordered]@{
        kind=$kind;inventory_id=$function.id;original_entry=$function.original_entry;original_name=$function.original_name
        scope=$scope;disposition=$function.disposition.disposition;metadata_reference='doc/cg2d_ida_inventory.json'
        evidence_references=@($receiptEvidence);classification_refs=@($function.classification_refs);feature_memberships=$memberIds;next_action=$next
    })
}
$locationGapCount=@($gapRows | Where-Object kind -eq 'mapping_location_unrecorded').Count
foreach ($row in @($semanticWithoutId | Where-Object { $null -eq $_.code_identity })) {
    $candidateIds=@($functionRows | Where-Object original_entry -eq $row.original_entry | ForEach-Object id)
    $next=if ($candidateIds.Count -eq 1) {'核该原函数完整边界/上下文与当前源码，确认后补确切ID'} else {'核代码段、原函数/chunk及未归属区间，保留真实代码身份'}
    $gapRows.Add([ordered]@{
        kind='semantic_identity_unrecorded';inventory_id=$null;original_entry=$row.original_entry;original_name=$null
        scope='identity_pending';status=$row.status;metadata_reference=$row.reference;candidate_inventory_ids=$candidateIds;next_action=$next
    })
}
$reviewGapCount=@($gapRows | Where-Object { $_.scope -notin @('excluded','paused','metadata_only') }).Count
$pausedGapCount=@($gapRows | Where-Object scope -eq 'paused').Count
$excludedGapCount=@($gapRows | Where-Object scope -eq 'excluded').Count
$metadataGapCount=@($gapRows | Where-Object scope -eq 'metadata_only').Count
$gapCounts=[ordered]@{mapping_location_unrecorded=$locationGapCount;semantic_identity_unrecorded=$unboundSemanticCount;total=$gapRows.Count;needs_scope_or_mapping_review=$reviewGapCount;paused_receipt_items=$pausedGapCount;excluded_receipt_items=$excludedGapCount;metadata_only_receipt_items=$metadataGapCount}
$metrics=[ordered]@{
    inventory_functions=$functions.Count
    development_gap_counts=$gapCounts
    functions_with_document_links=$linked
    document_link_percent=(Percent $linked $functions.Count)
    functions_without_document_links=@($functionRows|Where-Object {$_.baseline_documents.Count -eq 0}|ForEach-Object id)
    historical_direct_location_ids=@($functionRows|Where-Object {$_.direct_snapshot_definitions.Count -gt 0}).Count
    semantic_records=$semantic.records.Count
    semantic_inventory_ids=@($functionRows|Where-Object {$_.semantic_indices.Count -gt 0}).Count
    semantic_without_inventory_id=$semanticWithoutId.Count
    semantic_code_range_records=$semanticCodeRangeCount
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
if ($registry.Contains('build_validation')) {
    $build = $registry.build_validation
    foreach ($ref in $build.evidence_refs) { Check-Path $ref '构建验收' }
    Require ($build.client_link_passed -eq ($build.client_link_exit_code -eq 0)) '构建验收链接状态与退出码不符'
    Require ($build.unresolved_symbol_count -ge 0 -and $build.unresolved_paused_audio_count -ge 0 -and $build.unresolved_paused_audio_count -le $build.unresolved_symbol_count) '构建验收未解析符号计数不符'
    $globalReady = $globalReady -and $build.client_link_passed
    $metrics.global_accepted = $globalReady
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
    development_gap_queue=@($gapRows)
    imports=$imports;unassigned_ranges=$ranges;classification_sources=@($classifications)
    features=@($featureRows);tasks=@($tasks.Keys|Sort-Object|ForEach-Object {$tasks[$_]})
}
if ($registry.Contains('build_validation')) { $snapshot.build_validation = $registry.build_validation }
# 每个身份/功能一行，避免派生快照制造十万行缩进差异；内容仍为标准JSON。
$jsonLines=[System.Collections.Generic.List[string]]::new()
$jsonLines.Add('{')
$keys=@($snapshot.Keys)
for ($ki=0;$ki -lt $keys.Count;$ki++) {
    $key=$keys[$ki]; $suffix=if ($ki -lt $keys.Count-1) {','} else {''}
    if ($key -in @('functions','imports','unassigned_ranges','features','tasks','development_gap_queue')) {
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
$lines.Add("| 语义登记 | $($semantic.records.Count)条/$($metrics.semantic_inventory_ids)个原ID | 无独立函数ID另列$($semanticWithoutId.Count)条，其中原区间绑定${semanticCodeRangeCount}条 |")
$lines.Add("| 具名分类引用 | $($metrics.functions_with_classification_refs)个原ID | 含排除/混合/合并，不计为实现 |")
$lines.Add("| 已具名全入口排除 / 混合入口 | $($declaredExcluded.Count) / $($declaredMixed.Count) | 不计为实现/验收；混合入口不得整体扣除 |")
$lines.Add("| 已审核函数处置 | $($dispositionIds.Count)/$($functions.Count) | 初始仅导入关系，未逐ID重新认定 |")
$lines.Add("| 导入槽 / 未归属区间 | $($imports.Count) / $($ranges.Count) | 全部保留稳定ID；已审处置$reviewedImports / $reviewedRanges |")
$lines.Add("| 已登记叶功能 | $($leaf.Count) | 非全功能分母；域不重复计入，暂停保留 |")
$lines.Add("| 已验收叶功能 | $acceptedCount/$($leaf.Count) | 仅登记集合，不能代表整体完成率 |")
$lines.Add("| 已知收尾任务 | $($tasks.Count)；验收关闭$($metrics.tasks_closed) | 任务不是全功能目录 |")
if ($registry.Contains('build_validation')) {
    $build = $registry.build_validation
    $configureState = if ($build.configure_exit_code -eq 0) { '通过' } else { '失败' }
    $libraryState = if ($build.static_libraries_exit_code -eq 0) { '通过' } else { '失败' }
    $linkState = if ($build.client_link_passed) { '通过' } else { '失败' }
    $lines.Add("| 实际构建（$($build.configuration)） | 配置$configureState / 三库$libraryState / 客户端链接$linkState | $($build.recorded_on)；未解析$($build.unresolved_symbol_count)项，其中暂停音频$($build.unresolved_paused_audio_count)项；[构建证据](../doc/cg2d_build_acceptance_spec.md)，不代运行或功能验收 |")
}
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
    $scopeNote = if ($f.Contains('resumed_original_inventory_ids') -and @($f.resumed_original_inventory_ids).Count -gt 0) { "（已恢复$(@($f.resumed_original_inventory_ids).Count)根；其余范围保留）" } else { '' }
    $lines.Add("| $($f.id) | $($f.scope)$($scopeNote)：$(Cell $f.title) | $docLinks | $($f.original_inventory_ids.Count)/$($f.candidate_function_ids.Count) | $($f.closure_task_ids -join '、') | $state |")
}
$lines.Add('')
$lines.Add('## 4. 当前推进规则')
$lines.Add('')
$lines.Add('先从已有合同补原成员和B证据，再逐项推进I/W/S；R保持blocked。优先处理已知P/C/U任务及无合同关联ID的归属，不把历史定位缺失当作未实现。原格式明文保持；必要音频/本次构建授权及局部恢复沿登记簿，未恢复范围和测试/运行/摘要限制保留。')
$lines.Add('本看板只汇总结构化登记；变更后需做人工影响分析。静态汇总器退出0不等于软件验收通过。')
$lines.Add('')
$lines.Add('## 5. 开发差集队列')
$lines.Add('')
$lines.Add('下表只列定位/语义身份登记的缺口，供下一步选项推进；历史定位缺失不证明当前源码未实现。先核具名范围及当前代码，确有缺口再开发，原库存名称只供定位。必要音频局部恢复不代表整项验收，未恢复范围与原格式明文等边界保留，队列不参与完成率。')
$lines.Add('')
$lines.Add('| 登记差集 | 数量 | 判定与下一步 |')
$lines.Add('|---|---:|---|')
$lines.Add("| 定位及语义关联均未登记 | $locationGapCount | 历史定位/语义/pending/source均空；先核当前源码和原身份 |")
$lines.Add("| 语义身份尚未登记 | $unboundSemanticCount | 原语义引用/入口保留；已按原区间绑定的代码记录不混计，候选ID不自动赋值 |")
$lines.Add("| 需先核范围/定位/身份 | $reviewGapCount | 下方明细；不等于待实现或已允许运行的项数 |")
$lines.Add("| 已明确暂停 / 具名排除的本队列项 | $pausedGapCount / $excludedGapCount | 全部保留快照；暂停数只限明确成员，不是整个音频分母 |")
$lines.Add("| 已处置纯NOP元数据项 | $metadataGapCount | 保留原库存/队列身份；不计源码实现或范围排除，不为填充生成空函数 |")
$lines.Add('')
$lines.Add('| 类别 | 原ID或语义引用 | 原入口 | 原库存名称 | 范围 | 下一步 |')
$lines.Add('|---|---|---|---|---|---|')
foreach ($gap in @($gapRows | Where-Object { $_.scope -notin @('excluded','paused','metadata_only') })) {
    $identity=if ($gap.inventory_id) { $gap.inventory_id } else { $gap.metadata_reference }
    $lines.Add("| $($gap.kind) | $(Cell $identity) | $($gap.original_entry) | $(Cell $gap.original_name) | $($gap.scope) | $(Cell $gap.next_action) |")
}
$lines.Add('')
$lines.Add('完整范围依据、已有处置及候选ID见快照development_gap_queue；metadata_only纯NOP项单列且保留原库存/队列身份，不计业务实现或范围排除；非函数代码绑定见semantic_without_inventory_id。逐项补事实后由汇总器更新，不手改队列，不以此关闭R1/R7或整体验收。')
$dashboard=($lines -join [char]10)+[char]10
$outputs=[ordered]@{'plan/cg2d_traceability_snapshot.json'=$json;'plan/cg2d_measurement_dashboard.md'=$dashboard}
foreach ($path in $outputs.Keys) {
    $full=Join-Path $projectRoot $path
    if ($Check) {
        if (-not (Test-Path -LiteralPath $full) -or [System.IO.File]::ReadAllText($full,$utf8) -cne $outputs[$path]) { throw "派生文件过期：$path；请先更新汇总。" }
    } else { [System.IO.File]::WriteAllText($full,$outputs[$path],$utf8) }
}
Write-Output "静态台账校验通过：$($functions.Count)函数、$($imports.Count)导入、$($ranges.Count)区间、$($leaf.Count)叶功能、$($tasks.Count)任务；验收结论请读看板。"
