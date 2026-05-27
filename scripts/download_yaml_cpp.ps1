[CmdletBinding()]
param()

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'
[Console]::OutputEncoding = New-Object System.Text.UTF8Encoding($false)

$version = 'yaml-cpp-0.9.0'
$repoRoot = Split-Path -Parent $PSScriptRoot
$targetRoot = Join-Path $repoRoot 'third_party\yaml-cpp'
$targetDir = Join-Path $targetRoot $version
$url = "https://github.com/jbeder/yaml-cpp/archive/refs/tags/$version.zip"

function Get-FullPath {
    param([Parameter(Mandatory)][string]$Path)
    return [System.IO.Path]::GetFullPath($Path)
}

function Test-InsideDirectory {
    param(
        [Parameter(Mandatory)][string]$Child,
        [Parameter(Mandatory)][string]$Parent
    )

    $childFull = Get-FullPath -Path $Child
    $parentFull = Get-FullPath -Path $Parent
    if (-not $parentFull.EndsWith([System.IO.Path]::DirectorySeparatorChar)) {
        $parentFull += [System.IO.Path]::DirectorySeparatorChar
    }

    return $childFull.StartsWith($parentFull, [System.StringComparison]::OrdinalIgnoreCase)
}

if (Test-Path -LiteralPath (Join-Path $targetDir 'CMakeLists.txt') -PathType Leaf) {
    Write-Host "已存在：yaml-cpp $version"
    exit 0
}

New-Item -ItemType Directory -Force -Path $targetRoot | Out-Null

$tempRoot = Join-Path ([System.IO.Path]::GetTempPath()) ('cgmsv-yaml-cpp-' + [System.Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Force -Path $tempRoot | Out-Null

try {
    $zipPath = Join-Path $tempRoot "$version.zip"
    Write-Host "下载：yaml-cpp $version"
    Invoke-WebRequest -Uri $url -OutFile $zipPath

    Write-Host "解压：$version.zip"
    Expand-Archive -LiteralPath $zipPath -DestinationPath $tempRoot -Force

    $expanded = Join-Path $tempRoot "yaml-cpp-$version"
    if (-not (Test-Path -LiteralPath (Join-Path $expanded 'CMakeLists.txt') -PathType Leaf)) {
        throw "yaml-cpp 压缩包目录结构异常：$expanded"
    }

    if (Test-Path -LiteralPath $targetDir) {
        if (-not (Test-InsideDirectory -Child $targetDir -Parent $targetRoot)) {
            throw "拒绝删除目标目录外路径：$targetDir"
        }
        Remove-Item -LiteralPath $targetDir -Recurse -Force
    }

    Move-Item -LiteralPath $expanded -Destination $targetDir
}
finally {
    if (Test-Path -LiteralPath $tempRoot) {
        Remove-Item -LiteralPath $tempRoot -Recurse -Force
    }
}

Write-Host "yaml-cpp 已就绪：$targetDir"
