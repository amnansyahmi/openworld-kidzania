[CmdletBinding()]
param([Parameter(Mandatory)][string]$UnrealRoot)
$ErrorActionPreference = 'Stop'
$RepoRoot = (Resolve-Path "$PSScriptRoot/../..").Path
$Editor = "$UnrealRoot/Engine/Binaries/Win64/UnrealEditor-Cmd.exe"
if (!(Test-Path $Editor)) { throw "Unreal commandlet not found: $Editor" }
& $Editor "$RepoRoot/Unreal/KidzCity.uproject" -Unattended -NullRHI -NoSound '-ExecCmds=Automation RunTests KidzCity' '-TestExit=Automation Test Queue Empty' "-ReportExportPath=$RepoRoot/TestResults/Unreal" -log
if ($LASTEXITCODE -ne 0) { throw "Unreal automation failed: $LASTEXITCODE" }
$ReportFile = "$RepoRoot/TestResults/Unreal/index.json"
if (!(Test-Path $ReportFile)) { throw 'Automation did not write its report.' }
$Report = Get-Content $ReportFile -Raw | ConvertFrom-Json
if ($Report.failed -gt 0 -or ($Report.succeeded + $Report.succeededWithWarnings) -lt 2) {
    throw 'Expected at least two passing KidzCity tests with zero failures. Inspect TestResults/Unreal/index.json.'
}
Write-Host 'Unreal rules/save automation passed. This command does not test rendering or WebRTC.'
