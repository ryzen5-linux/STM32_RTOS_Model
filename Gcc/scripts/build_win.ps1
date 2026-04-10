param(
    [ValidateSet('all', 'elf', 'hex', 'bin', 'size', 'clean', 'print-vars', 'rebuild')]
    [string]$Action = 'all',
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$MakeArgs
)

$ErrorActionPreference = 'Stop'

function Append-LogLine {
    param(
        [string]$LogPath,
        [string]$Text
    )

    $attempt = 0
    while ($attempt -lt 10) {
        try {
            $stream = [System.IO.File]::Open($LogPath, [System.IO.FileMode]::Append, [System.IO.FileAccess]::Write, [System.IO.FileShare]::ReadWrite)
            try {
                $writer = New-Object System.IO.StreamWriter($stream, [System.Text.Encoding]::ASCII)
                $writer.WriteLine($Text)
                $writer.Flush()
            }
            finally {
                if ($writer) { $writer.Dispose() }
                $stream.Dispose()
            }

            return
        }
        catch {
            Start-Sleep -Milliseconds 50
            $attempt++
        }
    }

    throw "Unable to append to log file: $LogPath"
}

function Write-Log {
    param(
        [string]$Message,
        [string]$LogPath
    )

    $line = ('[{0}] {1}' -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss'), $Message)
    Write-Host $line
    Append-LogLine -LogPath $LogPath -Text $line
}

function Invoke-LoggedMake {
    param(
        [string]$MakeExe,
        [string[]]$Arguments,
        [string]$LogPath
    )

    $output = & $MakeExe @Arguments 2>&1
    if ($null -ne $output) {
        foreach ($line in $output) {
            $text = [string]$line
            Write-Host $text
            Append-LogLine -LogPath $LogPath -Text $text
        }
    }

    return $LASTEXITCODE
}

function Get-NormalizedRelativePath {
    param(
        [string]$Root,
        [string]$Path
    )

    $relative = $Path.Substring($Root.Length).TrimStart([char[]]@('\', '/'))
    return ($relative -replace '\\', '/')
}

function Test-IsAllowedPath {
    param(
        [string]$RelativePath
    )

    if ($RelativePath.StartsWith('Gcc/build/')) {
        return $false
    }

    if ($RelativePath -match '^BSP/CMSIS/startup_stm32f10x_.*\.[sS]$') {
        return $false
    }

    if ($RelativePath.StartsWith('FreeRTOS/Source/portable/')) {
        return ($RelativePath -eq 'FreeRTOS/Source/portable/GCC/ARM_CM3/port.c') -or
               ($RelativePath -eq 'FreeRTOS/Source/portable/GCC/ARM_CM3/portmacro.h') -or
               ($RelativePath -eq 'FreeRTOS/Source/portable/MemMang/heap_4.c')
    }

    return $true
}

function Get-MakeCommand {
    foreach ($name in @('mingw32-make.exe', 'make.exe', 'gmake.exe')) {
        $cmd = Get-Command $name -ErrorAction SilentlyContinue
        if ($null -ne $cmd) {
            return $cmd.Source
        }
    }

    throw 'No make tool found in PATH. Install mingw32-make or make, then retry.'
}

function Get-ToolchainBin {
    param(
        [string]$ProjectRoot
    )

    $toolchainRoot = Join-Path (Split-Path $ProjectRoot -Parent) 'GNU Arm Embedded Toolchain'
    if (-not (Test-Path $toolchainRoot)) {
        throw "Toolchain root not found: $toolchainRoot"
    }

    $directBin = Join-Path $toolchainRoot 'bin'
    $directGcc = Join-Path $directBin 'arm-none-eabi-gcc.exe'
    if (Test-Path $directGcc) {
        return ($directBin -replace '\\', '/')
    }

    $versionDir = Get-ChildItem -Path $toolchainRoot -Directory |
        Sort-Object Name -Descending |
        Where-Object { Test-Path (Join-Path $_.FullName 'bin\arm-none-eabi-gcc.exe') } |
        Select-Object -First 1

    if ($null -eq $versionDir) {
        throw "No versioned GNU Arm Embedded Toolchain with bin/arm-none-eabi-gcc.exe found under $toolchainRoot"
    }

    return ((Join-Path $versionDir.FullName 'bin') -replace '\\', '/')
}

function Write-AutoManifest {
    param(
        [string]$ProjectRoot,
        [string]$OutputPath
    )

    $allFiles = Get-ChildItem -Path $ProjectRoot -Recurse -File | ForEach-Object {
        $relative = Get-NormalizedRelativePath -Root $ProjectRoot -Path $_.FullName
        [PSCustomObject]@{
            Relative = $relative
            Directory = [System.IO.Path]::GetDirectoryName($relative).Replace('\', '/')
        }
    }

    $allowed = $allFiles | Where-Object { Test-IsAllowedPath -RelativePath $_.Relative }

    $cSources = $allowed |
        Where-Object { $_.Relative.ToLower().EndsWith('.c') } |
        Select-Object -ExpandProperty Relative |
        Sort-Object -Unique

    $asmSources = $allowed |
        Where-Object { $_.Relative.EndsWith('.s') -or $_.Relative.EndsWith('.S') } |
        Select-Object -ExpandProperty Relative |
        Sort-Object -Unique

    $includeDirs = $allowed |
        Where-Object { $_.Relative.ToLower().EndsWith('.h') } |
        Select-Object -ExpandProperty Directory |
        Where-Object { $_ -and $_ -ne '.' } |
        Sort-Object -Unique

    [string[]]$content = @()
    $content += ('AUTO_SRC_C := ' + ($cSources -join ' '))
    $content += ('AUTO_SRC_ASM := ' + ($asmSources -join ' '))
    $content += ('AUTO_INC_DIRS := ' + ($includeDirs -join ' '))

    New-Item -ItemType Directory -Force -Path (Split-Path $OutputPath -Parent) | Out-Null
    [System.IO.File]::WriteAllLines($OutputPath, $content, [System.Text.Encoding]::ASCII)

    return [PSCustomObject]@{
        CSources = $cSources
        AsmSources = $asmSources
        IncludeDirs = $includeDirs
    }
}

function Get-SourceModuleName {
    param([string]$Path)
    if ($Path.StartsWith('BSP/'))              { return 'Board Support Package (BSP)' }
    if ($Path.StartsWith('Driver/'))           { return 'Peripheral device drivers' }
    if ($Path.StartsWith('FreeRTOS/Source/'))  { return 'FreeRTOS kernel' }
    if ($Path.StartsWith('FreeRTOS/App/'))     { return 'FreeRTOS application' }
    if ($Path.StartsWith('Libraries/'))        { return 'STM32 standard peripheral library' }
    return 'Other'
}

function Write-ManifestSummary {
    param(
        [object]$Manifest,
        [string]$LogPath
    )

    $totalSourceCount = $Manifest.CSources.Count + $Manifest.AsmSources.Count
    $asmWord = if ($Manifest.AsmSources.Count -eq 1) { 'file' } else { 'files' }

    # Group all source files by module
    $moduleOrder = @(
        'Board Support Package (BSP)',
        'Peripheral device drivers',
        'FreeRTOS kernel',
        'FreeRTOS application',
        'STM32 standard peripheral library',
        'Other'
    )
    $groups = @{}
    foreach ($mod in $moduleOrder) { $groups[$mod] = @{ C = [System.Collections.Generic.List[string]]::new(); Asm = [System.Collections.Generic.List[string]]::new() } }

    foreach ($f in $Manifest.CSources)    { $groups[(Get-SourceModuleName $f)].C.Add($f) }
    foreach ($f in $Manifest.AsmSources)  { $groups[(Get-SourceModuleName $f)].Asm.Add($f) }

    $activeModules = $moduleOrder | Where-Object { $groups[$_].C.Count -gt 0 -or $groups[$_].Asm.Count -gt 0 }
    $moduleCount   = @($activeModules).Count
    $modWord       = if ($moduleCount -eq 1) { 'module' } else { 'modules' }

    Write-Log -Message ("This build will compile {0} source files in total ({1} C files, {2} assembly {3}), organized across {4} {5}." -f `
        $totalSourceCount, $Manifest.CSources.Count, $Manifest.AsmSources.Count, $asmWord, $moduleCount, $modWord) -LogPath $LogPath

    foreach ($mod in $activeModules) {
        $g = $groups[$mod]
        $parts = @()
        if ($g.C.Count   -gt 0) { $cWord = if ($g.C.Count   -eq 1) {'file'} else {'files'}; $parts += ("{0} C {1}"        -f $g.C.Count,   $cWord) }
        if ($g.Asm.Count -gt 0) { $aWord = if ($g.Asm.Count -eq 1) {'file'} else {'files'}; $parts += ("{0} assembly {1}" -f $g.Asm.Count, $aWord) }

        Write-Log -Message ("  {0} -- {1}:" -f $mod, ($parts -join ', ')) -LogPath $LogPath
        foreach ($f in ($g.C   | Sort-Object)) { Write-Log -Message ("    {0}" -f $f) -LogPath $LogPath }
        foreach ($f in ($g.Asm | Sort-Object)) { Write-Log -Message ("    {0}  [assembly]" -f $f) -LogPath $LogPath }
    }

    $dirWord = if ($Manifest.IncludeDirs.Count -eq 1) { 'directory' } else { 'directories' }
    Write-Log -Message ("The compiler will search for headers in {0} include {1}:" -f $Manifest.IncludeDirs.Count, $dirWord) -LogPath $LogPath
    foreach ($dir in $Manifest.IncludeDirs) {
        Write-Log -Message ("    {0}" -f $dir) -LogPath $LogPath
    }
}

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$gccDir = Split-Path -Parent $scriptDir
$projectRoot = Split-Path -Parent $gccDir
$buildDir = Join-Path $gccDir 'build'
$makefilePath = Join-Path $gccDir 'Makefile'
$autoMkPath = Join-Path $buildDir 'auto_sources.mk'
$logPath = Join-Path $buildDir 'build.log'
$toolchainBin = Get-ToolchainBin -ProjectRoot $projectRoot
$makeExe = Get-MakeCommand

New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
Set-Content -Path $logPath -Value @() -Encoding Ascii
$manifest = Write-AutoManifest -ProjectRoot $projectRoot -OutputPath $autoMkPath
Set-Content -Path $logPath -Value @(
    ('[{0}] Build script started' -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss')),
    ('[{0}] Action={1}' -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss'), $Action),
    ('[{0}] ProjectRoot={1}' -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss'), $projectRoot),
    ('[{0}] GccDir={1}' -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss'), $gccDir),
    ('[{0}] BuildDir={1}' -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss'), $buildDir),
    ('[{0}] Makefile={1}' -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss'), $makefilePath),
    ('[{0}] AutoManifest={1}' -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss'), $autoMkPath),
    ('[{0}] ToolchainBin={1}' -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss'), $toolchainBin),
    ('[{0}] Make={1}' -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss'), $makeExe),
    ('[{0}] ExtraMakeArgs={1}' -f (Get-Date -Format 'yyyy-MM-dd HH:mm:ss'), (($MakeArgs | ForEach-Object { $_ }) -join ' '))
) -Encoding Ascii

$commonArgs = @(
    '-f',
    $makefilePath,
    'HOST_OS=windows',
    ('TOOLCHAIN_BIN=' + $toolchainBin),
    'AUTO_MK=build/auto_sources.mk'
)

if ($MakeArgs) {
    $commonArgs += $MakeArgs
}

Push-Location $gccDir
try {
    Write-Log -Message ("Log file: {0}" -f $logPath) -LogPath $logPath
    Write-ManifestSummary -Manifest $manifest -LogPath $logPath
    Write-Log -Message 'Running make print-vars' -LogPath $logPath
    $printVarsExitCode = Invoke-LoggedMake -MakeExe $makeExe -Arguments ($commonArgs + 'print-vars') -LogPath $logPath
    if ($printVarsExitCode -ne 0) {
        Write-Log -Message ("print-vars failed with exit code {0}" -f $printVarsExitCode) -LogPath $logPath
        exit $printVarsExitCode
    }

    if ($Action -eq 'rebuild') {
        Write-Log -Message 'Running make clean' -LogPath $logPath
        $exitCode = Invoke-LoggedMake -MakeExe $makeExe -Arguments ($commonArgs + 'clean') -LogPath $logPath
        if ($exitCode -ne 0) {
            Write-Log -Message ("Build failed with exit code {0}" -f $exitCode) -LogPath $logPath
            exit $exitCode
        }

        Write-Log -Message 'Running make all' -LogPath $logPath
        $exitCode = Invoke-LoggedMake -MakeExe $makeExe -Arguments ($commonArgs + 'all') -LogPath $logPath
        Write-Log -Message ("Build finished with exit code {0}" -f $exitCode) -LogPath $logPath
        exit $exitCode
    }

    Write-Log -Message ("Running make {0}" -f $Action) -LogPath $logPath
    $exitCode = Invoke-LoggedMake -MakeExe $makeExe -Arguments ($commonArgs + $Action) -LogPath $logPath
    Write-Log -Message ("Build finished with exit code {0}" -f $exitCode) -LogPath $logPath
    exit $exitCode
}
finally {
    Pop-Location
}