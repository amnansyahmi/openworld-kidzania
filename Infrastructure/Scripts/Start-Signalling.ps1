[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
$RepoRoot = (Resolve-Path "$PSScriptRoot/../..").Path
$Versions = Get-Content "$RepoRoot/Infrastructure/versions.json" -Raw | ConvertFrom-Json
$Destination = "$RepoRoot/Infrastructure/ThirdParty/PixelStreamingInfrastructure"
$Entry = "$Destination/SignallingWebServer/dist/index.js"
if (!(Test-Path $Entry)) { throw 'Run Setup-Streaming.ps1 first.' }
$Actual = git -C $Destination rev-parse HEAD
if ($LASTEXITCODE -ne 0 -or $Actual -ne $Versions.infrastructureCommit) { throw 'Streaming checkout does not match versions.json.' }
Write-Host 'Single local session: browser signalling 8880; Unreal streamer 8888.'
Write-Host 'The upstream listeners can bind all interfaces. Keep these ports restricted by your local firewall; do not expose them publicly.'
# Use the JS entry directly: upstream npm start adds HTTPS redirect defaults.
node $Entry --no_config --player_port 8880 --streamer_port 8888 --sfu_port 8889 --max_players 1 --public_ip 127.0.0.1 --peer_options '{"iceServers":[]}'
if ($LASTEXITCODE -ne 0) { throw "Signalling exited with code $LASTEXITCODE" }
