[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
$RepoRoot = (Resolve-Path "$PSScriptRoot/../..").Path
$Versions = Get-Content "$RepoRoot/Infrastructure/versions.json" -Raw | ConvertFrom-Json
$Destination = "$RepoRoot/Infrastructure/ThirdParty/PixelStreamingInfrastructure"
function Check-Native { if ($LASTEXITCODE -ne 0) { throw "Command failed with exit code $LASTEXITCODE" } }
Get-Command git, node, npm -ErrorAction Stop | Out-Null
if (!(Test-Path "$Destination/.git")) {
    if (Test-Path $Destination) { throw "Destination exists but is not a Git checkout: $Destination" }
    git clone --branch $Versions.infrastructureBranch --single-branch $Versions.infrastructureRepository $Destination
    Check-Native
}
Push-Location $Destination
try {
    $Dirty = git status --porcelain
    Check-Native
    if ($Dirty) { throw 'Streaming checkout contains edits. Preserve them before changing its version.' }
    git fetch origin $Versions.infrastructureCommit
    Check-Native
    git checkout --detach $Versions.infrastructureCommit
    Check-Native
    # Build only the local signalling dependencies. No SFU, TURN service or GPU cloud is created.
    npm ci --workspace Common --workspace Signalling --workspace SignallingWebServer --include-workspace-root
    Check-Native
    npm run build:cjs --workspace Common
    Check-Native
    npm run build:cjs --workspace Signalling
    Check-Native
    npm run build --workspace SignallingWebServer
    Check-Native
} finally { Pop-Location }
Write-Host 'Streaming dependencies are ready. Next: Start-Signalling.ps1.'
