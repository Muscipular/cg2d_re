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
    $Destination = Join-Path $scriptRoot '..\third_party\sdl'
}

$packages = @(
    @{
        Name = 'SDL3'
        Version = '3.4.8'
        File = 'SDL3-devel-3.4.8-VC.zip'
        Url = 'https://github.com/libsdl-org/SDL/releases/download/release-3.4.8/SDL3-devel-3.4.8-VC.zip'
        Directory = 'SDL3-3.4.8'
        Required = @(
            'cmake\SDL3Config.cmake',
            'lib\x64\SDL3.dll',
            'lib\x64\SDL3.lib'
        )
    },
    @{
        Name = 'SDL3_image'
        Version = '3.4.4'
        File = 'SDL3_image-devel-3.4.4-VC.zip'
        Url = 'https://github.com/libsdl-org/SDL_image/releases/download/release-3.4.4/SDL3_image-devel-3.4.4-VC.zip'
        Directory = 'SDL3_image-3.4.4'
        Required = @(
            'cmake\SDL3_imageConfig.cmake',
            'lib\x64\SDL3_image.dll',
            'lib\x64\SDL3_image.lib'
        )
    },
    @{
        Name = 'SDL3_ttf'
        Version = '3.2.2'
        File = 'SDL3_ttf-devel-3.2.2-VC.zip'
        Url = 'https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-devel-3.2.2-VC.zip'
        Directory = 'SDL3_ttf-3.2.2'
        Required = @(
            'cmake\SDL3_ttfConfig.cmake',
            'lib\x64\SDL3_ttf.dll',
            'lib\x64\SDL3_ttf.lib'
        )
    }
)

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

$tempRoot = Join-Path ([System.IO.Path]::GetTempPath()) ('cgmsv-sdl3-' + [System.Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Force -Path $tempRoot | Out-Null

try {
    foreach ($package in $packages) {
        $packageRoot = Join-Path $destinationFull $package['Directory']
        if (-not $Force -and (Test-Package -Package $package -Root $destinationFull)) {
            Write-Host "Exists: $($package['Name']) $($package['Version'])"
            continue
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
        Expand-Archive -LiteralPath $zipPath -DestinationPath $destinationFull -Force

        if (-not (Test-Package -Package $package -Root $destinationFull)) {
            throw "Dependency validation failed: $($package['Name'])"
        }
    }
}
finally {
    if (Test-Path -LiteralPath $tempRoot) {
        Remove-Item -LiteralPath $tempRoot -Recurse -Force
    }
}

Write-Host "SDL3 dependencies ready: $destinationFull"
