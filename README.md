# KidzCity — Unreal foundation

An original, family-friendly career city. Unreal Engine **5.7** renders the game; Pixel Streaming **2** delivers video/audio and input through WebRTC to a lightweight browser launcher. Development starts on a local Windows PC. No cloud resources are provisioned.

**Current milestone: source foundation, not a finished playable city.** The C++ systems and browser launcher are implemented. This repository does not yet contain authored `.umap`/`.uasset` files, character meshes, animation clips, finished interiors, or a packaged game. Unreal compilation, visual QA and end-to-end streaming still need a machine with UE 5.7 and an encoding-capable GPU. Do not interpret the launcher or passing web tests as proof of the vertical slice.

## Start here

1. Read [current status and exact remaining work](Docs/STATUS.md).
2. Follow [Unreal Editor setup](Docs/EDITOR_SETUP.md) to compile native classes and connect real assets.
3. Follow [local Pixel Streaming](Docs/LOCAL_STREAMING.md) to connect the browser.
4. Run the [vertical-slice acceptance checks](Docs/ACCEPTANCE.md).

## Repository

| Directory | Responsibility |
| --- | --- |
| `Unreal/` | Native gameplay, Unreal config, assembly manifest and Editor script |
| `Web/` | Actual Pixel Streaming browser client and stream preferences |
| `Backend/` | Future service boundary and proposed profile contract; no running backend |
| `Infrastructure/` | Pinned upstream streaming version and local PowerShell scripts |
| `Docs/` | Architecture, setup, visual specification and completion gates |
| `Tests/` | Engine-independent native job-rule tests and repository consistency checks |

## Available checks without Unreal

```sh
python3 Tests/check_repository.py
g++ -std=c++20 -Wall -Wextra -Werror -I Unreal/Source/KidzCity/Public Tests/job_rules_test.cpp -o /tmp/kidzcity-rules
/tmp/kidzcity-rules
npm ci --prefix Web
npm run build --prefix Web
npm test --prefix Web
```

The browser-native benchmark is on `feature/browser-webgpu-benchmark`. Start it with `npm run dev --prefix Web`; it renders directly in the browser. The Unreal Pixel Streaming launcher remains on `feature/unreal-foundation`.

Project source and third-party assets have separate ownership/licensing requirements; see [asset policy](Docs/ART_DIRECTION.md). The GitHub repository name is historical context only; in-game naming and creative content use the original KidzCity concept.
