[CmdletBinding()]
param([Parameter(Mandatory)][string]$UnrealRoot, [switch]$Package)
$ErrorActionPreference = 'Stop'
$RepoRoot = (Resolve-Path "$PSScriptRoot/../..").Path
$Project = "$RepoRoot/Unreal/KidzCity.uproject"
$Version = Get-Content "$UnrealRoot/Engine/Build/Build.version" -Raw | ConvertFrom-Json
if ($Version.MajorVersion -ne 5 -or $Version.MinorVersion -ne 7) { throw 'This branch requires Unreal Engine 5.7.' }
if ($Package) {
    if (!(Test-Path "$RepoRoot/Unreal/Content/Maps/L_Downtown.umap")) { throw 'Author and validate L_Downtown before packaging.' }
    & "$UnrealRoot/Engine/Build/BatchFiles/RunUAT.bat" BuildCookRun "-project=$Project" -noP4 -platform=Win64 -clientconfig=Development -build -cook -map=/Game/Maps/L_Downtown -stage -pak -archive "-archivedirectory=$RepoRoot/Builds"
} else {
    & "$UnrealRoot/Engine/Build/BatchFiles/Build.bat" KidzCityEditor Win64 Development "-Project=$Project" -WaitMutex -NoHotReloadFromIDE
}
if ($LASTEXITCODE -ne 0) { throw "Unreal build failed: $LASTEXITCODE" }
