# Visual direction and content ownership

KidzCity is an original, family-friendly career city with realistic proportions, a colourful contemporary environment and credible materials. Use the movement/camera readability of a modern third-person open world and the lived-in interactions of a life simulation. Do not copy recognisable characters, logos, buildings, signage or trade dress from GTA, The Sims or KidZania.

## Downtown target

Keep the slice to a compact, walkable district roughly 180–240 metres across. The hospital is the first detailed interior; all five locations should ultimately have enterable spaces. A short loop road supports the car and a later fire-truck mission. Place sidewalks and storefront entrances along clear sightlines, with a small plaza and marked crossings. Scale buildings around real doors, floors and human heights rather than arranging giant toy blocks.

| Element | Direction | Acceptance concern |
| --- | --- | --- |
| Characters | Realistic stylised proportions, natural skin/hair/clothes, friendly expressions | No block bodies, rigid sliding, oversized heads or mismatched skeletons |
| Architecture | Contemporary Malaysian-inspired civic/retail details where appropriate, original signs | Door heights, facade depth, believable interiors and collision |
| Surfaces | PBR roughness/normal detail; road wear, painted markings, glass and tiled interiors | No flat untextured primitives, excessive gloss or mismatched texel density |
| Foliage | Curated street trees/shrubs, readable silhouettes, restrained wind | Overdraw, LOD transitions and no foliage obscuring entrances |
| Lighting | Warm daylight, coherent shadows, inviting interior light, readable night | Exposure pumping, dark hospital transitions and noisy reflections |
| Animation | Idle/walk/run/start/stop/jump/land; enter/exit car; idle activities | Foot contact, turn response, transition pops, seat/hand alignment |
| Interface | Small contextual prompts, restrained wallet/job feedback, accessible text | UI must not become a cartoon dashboard over the city |
| Audio | Footsteps, vehicle/traffic ambience, indoor ambience, friendly job feedback | Distance falloff, looping artifacts, harsh alarms and volume imbalance |

## Content delivery checklist

Track each imported pack/asset with: source URL, vendor/author, licence and version, acquisition date, allowed project/team usage, redistribution restrictions, skeleton/scale notes, LOD/Nanite suitability and intended destination. Do not commit restricted source assets to this public repository. Git LFS stores large assets efficiently; it does not grant redistribution rights.

Acquire or author these sets before the visual slice can be completed:

- One player humanoid and a small compatible NPC set; normal-proportion mesh and complete locomotion clips.
- Clinic visitor, receptionist/doctor clothing and patient/treatment animations.
- Modular roads, sidewalks, junctions, markings, street fixtures and foliage.
- Hospital/fire station/restaurant/supermarket/bank shells, doors and interior furniture.
- A rigged car compatible with Chaos Vehicles and a later rigged fire truck.
- Seating, talk, wait, shop, facility-use and work animations on the chosen skeleton.
- Audio for movement, city ambience, interior changes and jobs.

No assets were purchased or downloaded in this source-foundation milestone. The manifest names desired assets; it does not assert that those assets exist. Do not substitute generated city screenshots for rendered gameplay evidence.

## Rendering budget

Initial benchmark target is 1080p/60 on the selected local GPU, including encoding overhead; this is a target, not a measured result. Keep the first crowd modest (about 12–20 pedestrians and 4–6 background vehicles), then measure before adding density. Use LODs or suitable Nanite assets, sensible texture budgets and limited shadow-casting lights. Avoid assuming Lumen, high-cost hair, maximum shadows and dense foliage can all run at their highest settings on unspecified hardware.

High and Balanced presets are starting points. Measure with Unreal Insights and GPU profiling, both standalone and streamed. Record GPU model, driver, engine version, scene population, resolution, graphics settings, GPU/CPU frame times, encoder latency, end-to-end input latency, bitrate, packet loss and browser decode performance. Resolve sustained frame-time overruns before adding art density.

Later automatic adaptation should use measured WebRTC stats and server frame time with hysteresis/cooldowns: reduce transmission bitrate first, then render resolution/expensive effects, and recover slowly. Do not oscillate presets frame by frame. Touch controls and mobile thermal/network testing are a separate milestone.
