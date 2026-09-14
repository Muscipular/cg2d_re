#requires -Version 7.0
param(
    [Parameter(Mandatory = $true)]
    [string]$ObjectPath,

    [Parameter(Mandatory = $true)]
    [ValidateSet('x86', 'x64')]
    [string]$Architecture
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

# 仅检查 R3.1.1 的普通 COFF 数据对象，不加载或执行对象代码。
# 指纹来自 IDA 输入 dad3f292c6df28e9b7208ad1d339f56ab68c1e728f6595e849e37296551c0f08。
$expectedTables = [ordered]@{
    Graphic = '0b8b5208670efed3b45a6c81b57acee8a84e731a5a3827a540353bcbc948e858'
    GraphicInfo = '318cc0618c52ede9e7bfa4552d1aa49ca492c00d217a5550a1376254152e9775'
    Anime = '1a1648c65b5f2d2f6e440ee3d29d793c6fc3411e2751232e382d8981790a1ee9'
    AnimeInfo = '0085f7743e5cefd453dbf746286e0ebdf26d0e556dfde08668d5710cba0b475f'
    Coordinate = '1d4bdf72dc5bd504bb8385283025512625c99e46852c942f3bbd1046463963eb'
    CoordinateInfo = 'af331e70688ce893efb11ca696de3778d4fef59382a72814bb85fdeb5e5fca06'
}

if (-not [BitConverter]::IsLittleEndian) {
    throw '此验证脚本仅支持小端验证宿主。'
}
$path = (Resolve-Path -LiteralPath $ObjectPath).Path
$bytes = [System.IO.File]::ReadAllBytes($path)
if ($bytes.Length -lt 20) {
    throw 'COFF 文件头不完整。'
}
$expectedMachine = if ($Architecture -eq 'x86') { 0x014C } else { 0x8664 }
if ([BitConverter]::ToUInt16($bytes, 0) -ne $expectedMachine) {
    throw '对象架构与指定的验证架构不一致。'
}
if ([BitConverter]::ToUInt16($bytes, 16) -ne 0) {
    throw '只接受无可选头的普通 COFF 对象。'
}
$sectionCount = [BitConverter]::ToUInt16($bytes, 2)
if ($sectionCount -eq 0 -or 20 + $sectionCount * 40 -gt $bytes.Length) {
    throw 'COFF 节表不完整。'
}

$dataSections = @()
for ($index = 0; $index -lt $sectionCount; ++$index) {
    $offset = 20 + $index * 40
    $name = [System.Text.Encoding]::ASCII.GetString($bytes, $offset, 8).TrimEnd([char]0)
    $rawSize = [BitConverter]::ToUInt32($bytes, $offset + 16)
    $rawOffset = [BitConverter]::ToUInt32($bytes, $offset + 20)
    $relocationCount = [BitConverter]::ToUInt16($bytes, $offset + 32)
    $flags = [BitConverter]::ToUInt32($bytes, $offset + 36)
    if ([uint64]$rawOffset + $rawSize -gt [uint64]$bytes.Length) {
        throw "COFF 节数据越界：$name"
    }
    if (($flags -band 536870912) -ne 0 -and $rawSize -ne 0) {
        throw '此小步的数据对象不应包含可执行代码。'
    }
    if ($name -eq '.data') {
        $dataSections += [pscustomobject]@{
            RawOffset = $rawOffset
            RawSize = $rawSize
            Flags = $flags
            RelocationCount = $relocationCount
        }
    }
}

if ($dataSections.Count -ne 1) {
    throw '预期唯一的连续 .data 存储区。'
}
$data = $dataSections[0]
if ($data.RawSize -ne 0x1B00) {
    throw '资源名存储区长度不是 6912 字节。'
}
if (($data.Flags -band 2147483648) -eq 0 -or ($data.Flags -band 0x40) -eq 0) {
    throw '资源名存储区必须是可写的已初始化数据。'
}
if ($data.RelocationCount -ne 0) {
    throw '内嵌字符数组不应包含指针重定位。'
}

$sha256 = [System.Security.Cryptography.SHA256]::Create()
try {
    $index = 0
    foreach ($entry in $expectedTables.GetEnumerator()) {
        $offset = [int]$data.RawOffset + $index * 0x480
        $digest = $sha256.ComputeHash($bytes, $offset, 0x480)
        $actual = [BitConverter]::ToString($digest).Replace('-', '').ToLowerInvariant()
        if ($actual -cne $entry.Value) {
            throw "资源表指纹不匹配：$($entry.Key)"
        }
        ++$index
    }
}
finally {
    $sha256.Dispose()
}

"${Architecture}：6 张表、54 槽、6912 字节及可写存储检查通过。"
