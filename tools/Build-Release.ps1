param(
    [switch]$AllowMissingWdk,
    [switch]$RunAuthorizedNetworkIntegration
)
$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$build = Join-Path $root 'build'
$research = Join-Path $root 'HWIDResearch'
$researchBuild = Join-Path $research 'build'
$release = Join-Path $root 'release'
$vsCandidates = @(
    'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat',
    'C:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
)
$vsDevCmd = $vsCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $vsDevCmd) { throw 'MSVC BuildTools VsDevCmd.bat was not found.' }
$commit = (git -C $root rev-parse HEAD).Trim()
$commitShort = (git -C $root rev-parse --short HEAD).Trim()
$wdkHeader = Get-ChildItem 'C:\Program Files (x86)\Windows Kits\10\Include' -Recurse -Filter Wdf.h -ErrorAction SilentlyContinue | Select-Object -First 1
$wdkAvailable = $null -ne $wdkHeader
$driverStatus = if ($wdkAvailable) { 'DETECTED_NOT_BUILT_BY_THIS_SCRIPT' } else { 'NOT_BUILT_WDK_MISSING' }
if (-not $wdkAvailable -and -not $AllowMissingWdk) { throw 'WDK/KMDF is missing. Re-run with -AllowMissingWdk only to build user-mode artifacts without publishing a complete driver release.' }

if (Test-Path $build) { Remove-Item -LiteralPath $build -Recurse -Force }
if (Test-Path $researchBuild) { Remove-Item -LiteralPath $researchBuild -Recurse -Force }
New-Item -ItemType Directory -Force -Path $release | Out-Null
$env:Path = 'C:\Qt\6.8.3\msvc2022_64\bin;' + $env:Path
function Invoke-DevCommand([string]$command) {
    $log = Join-Path $root 'build-command.log'
    & cmd /c "call `"$vsDevCmd`" -arch=x64 -host_arch=x64 >nul && $command > `"$log`" 2>&1"
    if ($LASTEXITCODE -ne 0) { Get-Content $log -Tail 80; throw "Build command failed ($LASTEXITCODE): $command" }
}
Invoke-DevCommand "cmake -S `"$root`" -B `"$build`" -G `"NMake Makefiles`" -DCMAKE_BUILD_TYPE=Release"
Invoke-DevCommand "cmake --build `"$build`" --target GhostSystemManager InventoryProbe PersistenceProbe ConfigurationProbe PhysicalIntegrationProbe"
Invoke-DevCommand "cmake -S `"$research`" -B `"$researchBuild`" -G `"NMake Makefiles`" -DCMAKE_BUILD_TYPE=Release"
Invoke-DevCommand "cmake --build `"$researchBuild`""
if ($wdkAvailable) {
    $msbuildCandidates = @('C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe','C:\Program Files\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe')
    $msbuild = $msbuildCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
    if (-not $msbuild) { throw 'WDK was found but MSBuild was not found.' }
    & $msbuild (Join-Path $research 'Driver\HWIDResearch.vcxproj') /t:Rebuild /p:Configuration=Release /p:Platform=x64 /m /nologo > (Join-Path $root 'driver-build.log') 2>&1
    if ($LASTEXITCODE -ne 0) { Get-Content (Join-Path $root 'driver-build.log') -Tail 100; throw 'KMDF driver build failed.' }
    $sys = Get-ChildItem (Join-Path $research 'Driver') -Recurse -Filter 'HWIDResearch.sys' -ErrorAction SilentlyContinue | Select-Object -First 1
    if (-not $sys) { throw 'MSBuild completed but no HWIDResearch.sys was produced.' }
    Copy-Item -LiteralPath $sys.FullName -Destination (Join-Path $release 'HWIDResearch.sys') -Force
    $driverStatus = 'BUILT_WDK'
}

$testResults = [ordered]@{}
& (Join-Path $build 'InventoryProbe.exe') | Out-Null; $testResults.InventoryProbe = if ($LASTEXITCODE -eq 0) { 'PASS' } else { 'FAIL' }
& (Join-Path $build 'PersistenceProbe.exe') | Out-Null; $testResults.PersistenceProbe = if ($LASTEXITCODE -eq 0) { 'PASS' } else { 'FAIL' }
& (Join-Path $build 'ConfigurationProbe.exe') | Out-Null; $testResults.ConfigurationProbe = if ($LASTEXITCODE -eq 0) { 'PASS' } else { 'FAIL' }
& (Join-Path $build 'PhysicalIntegrationProbe.exe') | Out-Null; $testResults.PhysicalIntegrationProbe = if ($LASTEXITCODE -eq 0) { 'PASS_NOT_TESTED' } else { 'FAIL' }
& (Join-Path $researchBuild 'HWIDResearchVirtualIdentityProbe.exe') | Out-Null; $testResults.VirtualIdentityProbe = if ($LASTEXITCODE -eq 0) { 'PASS' } else { 'FAIL' }
& (Join-Path $researchBuild 'HWIDResearchHardwareProbe.exe') | Out-Null; $testResults.HardwareProbe = if ($LASTEXITCODE -eq 0) { 'PASS' } else { 'FAIL' }
& (Join-Path $researchBuild 'HWIDResearchIdentityFlowProbe.exe') | Out-Null; $testResults.IdentityFlowProbe = if ($LASTEXITCODE -eq 0) { 'PASS' } else { 'FAIL' }
& (Join-Path $researchBuild 'HWIDResearchHypervisorProbe.exe') | Out-Null; $testResults.HypervisorProbe = if ($LASTEXITCODE -eq 0) { 'PASS' } else { 'FAIL' }
if ($RunAuthorizedNetworkIntegration) { $env:GHOST_RUN_NETWORK_INTEGRATION = '1'; & (Join-Path $build 'PhysicalIntegrationProbe.exe') | Out-Null; $testResults.AuthorizedNetworkIntegration = if ($LASTEXITCODE -eq 0) { 'PASS' } else { 'FAIL' } }
if ($testResults.Values -contains 'FAIL') { throw 'At least one mandatory test failed; release was not published.' }

$artifact = Join-Path $build 'GhostSystemManager.exe'
Copy-Item -LiteralPath $artifact -Destination (Join-Path $release 'GhostSystemManager.exe') -Force
$hash = (Get-FileHash -Algorithm SHA256 (Join-Path $release 'GhostSystemManager.exe')).Hash
$manifest = [ordered]@{ project='GhostSystemManager'; commit=$commit; commitShort=$commitShort; builtAt=(Get-Date).ToUniversalTime().ToString('o'); configuration='Release'; executable='release/GhostSystemManager.exe'; sha256=$hash; dependencies=@('MSVC 19.44','Qt 6.8.3 msvc2022_64','Windows SDK 10.0.26100.0'); wdkAvailable=$wdkAvailable; driverStatus=$driverStatus; tests=$testResults; publicationEligible=($driverStatus -eq 'BUILT_WDK' -and -not ($testResults.Values -contains 'FAIL')) }
$manifest | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $release 'build-manifest.json') -Encoding UTF8
Write-Output ($manifest | ConvertTo-Json -Depth 6)
if (-not $wdkAvailable) { Write-Warning 'User-mode release published with publicationEligible=false because WDK/KMDF is missing.' }

