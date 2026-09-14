# Local Unreal → signalling → WebRTC → browser

This milestone runs on one development PC. The browser installs no game; the development PC must have Unreal and a supported encoding GPU. A launcher alone cannot provide the game without that process running.

## Version and ports

| Component | Version / endpoint |
| --- | --- |
| Unreal project | UE 5.7, PixelStreaming2 plugin enabled |
| Signalling source | Exact SHA in `Infrastructure/versions.json`, from Epic's UE5.7 branch |
| Browser library | `@epicgames-ps/lib-pixelstreamingfrontend-ue5.7@0.3.1` |
| Web development server | `http://127.0.0.1:5173` by default |
| Browser signalling WebSocket | `ws://127.0.0.1:8880` |
| Unreal streamer WebSocket | `ws://127.0.0.1:8888` |
| WebRTC | Negotiated ICE UDP media/data, not carried through the Vite HTTP server |

UE 5.7 was selected as a supported baseline, not a claim that it is the newest engine release. Keep engine and streaming release branches matched. The exact source pin avoids silent upstream changes.

## One-time setup

First complete `EDITOR_SETUP.md`. There must be a compiled game and an authored `L_Downtown`.

```powershell
./Infrastructure/Scripts/Setup-Streaming.ps1
npm ci --prefix Web
```

The script clones the pinned official infrastructure into ignored `Infrastructure/ThirdParty/`, installs its locked Common/Signalling/SignallingWebServer workspaces and builds those components. It does not create a TURN server, SFU service, cloud instance or paid resource. Node/Git dependency downloads still need internet access.

## Start three terminals

Terminal 1 — signalling:

```powershell
./Infrastructure/Scripts/Start-Signalling.ps1
```

Terminal 2 — the actual Unreal game (Standalone via Editor):

```powershell
./Infrastructure/Scripts/Start-Game.ps1 -UnrealRoot 'C:/Program Files/Epic Games/UE_5.7'
```

Alternatively, run your packaged executable:

```powershell
./Infrastructure/Scripts/Start-Game.ps1 -Executable './Builds/Windows/KidzCity.exe'
```

Terminal 3 — browser launcher:

```powershell
npm run dev --prefix Web
```

Open the address printed by Vite and click **Play game**. If audio/video autoplay is blocked, click **Start video and audio**. Click inside the game to enable input. Escape releases pointer lock. Disconnect ends the browser connection, not the Unreal process. Stop the game and signalling terminals when finished.

The signalling script allows only one browser subscriber. It uses explicit modern `--player_port` and `--streamer_port` flags, not legacy `HttpPort` configuration. It loads ICE settings from `local-peer-options.json` to avoid PowerShell/Windows native-command JSON quoting differences. It invokes the upstream built JS directly so upstream `npm start` HTTPS-redirect defaults do not alter the local setup.

## Graphics and connection quality

`Start-Game.ps1 -Quality High` sets 1920×1080, a 60 FPS cap and scalability level 2. Balanced retains 1080p with level 1. Mobile starts the game at 1280×720, a 30 FPS cap and level 1. F6/F7 switch the in-game scalability level during testing.

Browser Settings select a target streaming frame rate and bitrate envelope on the next connection: High up to 20 Mbps, Balanced 12 Mbps, Mobile 6 Mbps. Epic's browser settings use **kbps** internally. These are requested ceilings, not a bandwidth or frame-rate guarantee. WebRTC adapts bitrate to congestion; this build has no custom automatic render-resolution/graphics-quality controller. Browser Mobile alone does not change the Unreal render resolution; restart the game with the Mobile preset for 720p.

Desktop keyboard/mouse is the first input target. Touch/gamepad control and mobile-specific UI are not yet enabled. Selecting Mobile tunes transmission settings, not input compatibility.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| Play fails or times out | Signalling is running on 8880; Unreal connected on 8888; only one browser subscriber |
| “Downtown content is not authored yet” | Complete Editor asset/map setup; source alone does not create the city |
| Audio/video needs a gesture | Click Start video and audio; check browser autoplay settings |
| Blank game video | GPU encoder logs, real map, PlayerStart/GameMode, camera, packaged content and frame rendering |
| Character invisible | Player BP mesh and AnimBP assignment; native character deliberately contains no proxy mesh |
| Character or car not responding | Click viewport to capture input; check possession, Unreal input mappings, focus and vehicle physics |
| Car cannot exit | Stop the vehicle and clear one side; exit positions are collision-checked |
| NPCs stationary | Reachable activity points, AI possession, navmesh, crossing signals and authored animation hooks |
| Stream stops when window is minimized | Use the provided offscreen launch flags |
| HTTPS page rejects ws:// | Use local HTTP for this milestone. Future HTTPS hosting needs WSS signalling and a proper certificate |

## Network boundaries and later deployment

The web development server binds loopback. Upstream signalling may listen on all interfaces; using loopback connection URLs is not a listener bind restriction. Keep the development PC firewall restricting inbound access. This setup has no authentication and should not be exposed publicly. It creates no firewall rules automatically.

LAN testing requires explicit listening/address configuration and firewall scope; `127.0.0.1` in another device means that device, not the game PC. Cross-network testing usually needs STUN/TURN and secured signalling. Those services are deliberately deferred. Future public sessions require authenticated tickets, per-user game processes/save namespaces, admission/idle limits, restart cleanup and capacity/cost planning.

References: [Epic local setup](https://dev.epicgames.com/documentation/en-us/unreal-engine/getting-started-with-pixel-streaming-in-unreal-engine), [official infrastructure and version branches](https://github.com/EpicGames/PixelStreamingInfrastructure), [pinned signalling CLI source](https://github.com/EpicGames/PixelStreamingInfrastructure/blob/aa4972e5c9d53b0032d4f209e3f7728218b6213f/SignallingWebServer/src/index.ts).
