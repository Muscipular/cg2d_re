[CmdletBinding()]
param(
    [string]$Destination = '',
    [switch]$Force
)

$ErrorActionPreference = 'Stop'
$ProgressPreference = 'SilentlyContinue'
[Console]::OutputEncoding = New-Object System.Text.UTF8Encoding($false)

$scriptRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
if ([string]::IsNullOrWhiteSpace($Destination)) {
    $Destination = Join-Path $scriptRoot '..\third_party\yaml-cpp'
}

$package = @{
    Name = 'yaml-cpp'
    Version = '0.9.0'
    File = 'yaml-cpp-0.9.0.zip'
    Url = 'https://github.com/jbeder/yaml-cpp/archive/refs/tags/yaml-cpp-0.9.0.zip'
    ArchiveDirectory = 'yaml-cpp-yaml-cpp-0.9.0'
    Directory = 'yaml-cpp-0.9.0'
    Required = @(
        'CMakeLists.txt'
    )
}

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

function Test-Package {
    param(
        [Parameter(Mandatory)][hashtable]$Package,
        [Parameter(Mandatory)][string]$Root
    )

    $packageRoot = Join-Path $Root $Package['Directory']
    foreach ($relativePath in $Package['Required']) {
        $fullPath = Join-Path $packageRoot $relativePath
        if (-not (Test-Path -LiteralPath $fullPath -PathType Leaf)) {
            return $false
        }
    }

    return $true
}

$destinationFull = Get-FullPath -Path $Destination
New-Item -ItemType Directory -Force -Path $destinationFull | Out-Null

$tempRoot = Join-Path ([System.IO.Path]::GetTempPath()) ('cgmsv-yaml-cpp-' + [System.Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Force -Path $tempRoot | Out-Null

try {
    $packageRoot = Join-Path $destinationFull $package['Directory']
    if (-not $Force -and (Test-Package -Package $package -Root $destinationFull)) {
        Write-Host "Exists: $($package['Name']) $($package['Version'])"
        exit 0
    }

    if ($Force -and (Test-Path -LiteralPath $packageRoot)) {
        if (-not (Test-InsideDirectory -Child $packageRoot -Parent $destinationFull)) {
            throw "Refuse to delete path outside destination: $packageRoot"
        }
        Remove-Item -LiteralPath $packageRoot -Recurse -Force
    }

    $zipPath = Join-Path $tempRoot $package['File']
    Write-Host "Download: $($package['Name']) $($package['Version'])"
    Invoke-WebRequest -Uri $package['Url'] -OutFile $zipPath

    Write-Host "Extract: $($package['File'])"
    Expand-Archive -LiteralPath $zipPath -DestinationPath $tempRoot -Force

    $expanded = Join-Path $tempRoot $package['ArchiveDirectory']
    if (-not (Test-Path -LiteralPath (Join-Path $expanded 'CMakeLists.txt') -PathType Leaf)) {
        throw "Dependency archive layout is invalid: $($package['Name'])"
    }

    if (Test-Path -LiteralPath $packageRoot) {
        if (-not (Test-InsideDirectory -Child $packageRoot -Parent $destinationFull)) {
            throw "Refuse to delete path outside destination: $packageRoot"
        }
        Remove-Item -LiteralPath $packageRoot -Recurse -Force
    }

    Move-Item -LiteralPath $expanded -Destination $packageRoot

    if (-not (Test-Package -Package $package -Root $destinationFull)) {
        throw "Dependency validation failed: $($package['Name'])"
    }
}
finally {
    if (Test-Path -LiteralPath $tempRoot) {
        Remove-Item -LiteralPath $tempRoot -Recurse -Force
    }
}

Write-Host "yaml-cpp dependency ready: $destinationFull"
