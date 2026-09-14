[CmdletBinding(DefaultParameterSetName='Editor')]
param(
    [Parameter(Mandatory, ParameterSetName='Editor')][string]$UnrealRoot,
    [Parameter(Mandatory, ParameterSetName='Packaged')][string]$Executable,
    [ValidateSet('High','Balanced','Mobile')][string]$Quality = 'High'
)
$ErrorActionPreference = 'Stop'
$RepoRoot = (Resolve-Path "$PSScriptRoot/../..").Path
$Map = '/Game/Maps/L_Downtown'
$Resolution = if ($Quality -eq 'Mobile') { @('-ResX=1280','-ResY=720') } else { @('-ResX=1920','-ResY=1080') }
$Fps = if ($Quality -eq 'Mobile') { 30 } else { 60 }
$Level = if ($Quality -eq 'High') { 2 } else { 1 }
$RenderArgs = @('-RenderOffScreen','-AudioMixer','-ForceRes','-Unattended','-PixelStreamingURL=ws://127.0.0.1:8888','-PixelStreamingID=KidzCity') + $Resolution
$RenderArgs += "-ExecCmds=t.MaxFPS $Fps,sg.ViewDistanceQuality $Level,sg.ShadowQuality $Level,sg.PostProcessQuality $Level,sg.TextureQuality $Level,sg.EffectsQuality $Level,sg.FoliageQuality $Level"
if ($PSCmdlet.ParameterSetName -eq 'Editor') {
    $Editor = "$UnrealRoot/Engine/Binaries/Win64/UnrealEditor.exe"
    if (!(Test-Path $Editor)) { throw "Unreal Editor not found at $Editor" }
    $Version = Get-Content "$UnrealRoot/Engine/Build/Build.version" -Raw | ConvertFrom-Json
    if ($Version.MajorVersion -ne 5 -or $Version.MinorVersion -ne 7) { throw 'This branch requires Unreal Engine 5.7.' }
    if (!(Test-Path "$RepoRoot/Unreal/Content/Maps/L_Downtown.umap")) { throw 'Downtown content is not authored yet. Follow Docs/EDITOR_SETUP.md.' }
    & $Editor "$RepoRoot/Unreal/KidzCity.uproject" $Map -game @RenderArgs -log
} else {
    if (!(Test-Path $Executable)) { throw "Packaged game not found at $Executable" }
    & $Executable $Map @RenderArgs -log
}
if ($LASTEXITCODE -ne 0) { throw "Game exited with code $LASTEXITCODE" }
