# Status — 2026-09-14

This is milestone **0.1: native and streaming foundation**. The requested first successful vertical slice is **not yet complete**. The user explicitly changed the brief to a fresh start; the supplied GitHub repository was empty at inspection, so there is no legacy KEEP/REWRITE/MIGRATE/REMOVE inventory to invent.

| Area | Implemented in source | Remaining before claiming playable/polished |
| --- | --- | --- |
| Character | Native walking, running, jump, turn-to-movement, spring-arm camera and camera collision | Licensed, proportionate mesh; animation assets; AnimBP blending/IK; compile and tune in UE |
| Interaction | Camera focus trace, physical proximity and second occlusion check; interface and prompt | Place/configure real meshes with appropriate collision; final UI |
| Doctor | Two three-stage clinic cases, contextual choices, retry scoring, cancellation and receipt-based payout | Hospital interior, patient presentation, uniform transition, animations, spatial/tool manipulation beyond the current choice-based interaction |
| Coins and saves | Versioned local save, XP, receipts, overflow checks, invalid/future-save protection; explicit checkpoints | UE automation run, disk-failure tests, crash/power-loss resilience, later account migration |
| Driving | Chaos vehicle class, four-wheel setup, forward/reverse/brake controls, possession and checked exit positions | Rigged vehicle, physics asset, wheel tuning and driving tests |
| Pedestrians | Navigation controller, reservations, timed activity selection, player acknowledgement and signal-controlled crossing state | Meshes, real sitting/talking/shopping montages and paired activities; navigation/crossing authoring and QA |
| Traffic | Closed-loop spline traffic, obstacle sweeps, acceleration/braking and crossing stops | Routes, vehicles, signal wiring; intersection and clearance QA |
| Day/night | Sun/moon rotation and intensity controller | Lighting actors, calibrated exposure, skylight, interiors and night art pass; time is not persisted yet |
| Five locations | Native location enum and downtown assembly manifest | All five real building prefabs/interiors; hospital is the first fully functional interior target |
| Firefighter | Career specification and milestone sequence | Dispatch, fire truck, drive-to-incident, hose interaction, hazard progress, payout |
| Restaurant | Career specification and milestone sequence | Order generation, preparation interaction, serving and payout |
| Browser | Real Epic client, connect/disconnect, playback, fullscreen, focus handling, quality preferences, timeout errors | Actual UE-to-browser session validation; automated quality adaptation and touch/gamepad controls |
| Account menu | Future entry points visibly unavailable | Login, profile, character customisation and backend |

## Verification in this environment

- Passed: native engine-independent job-rule tests built with C++20 and warnings treated as errors.
- Passed: TypeScript type check, Vite production build and browser-settings unit tests.
- Passed: repository version/header/manifest checks and Python script syntax check.
- Unreal Editor, UnrealBuildTool and a GPU runtime are unavailable here. **No Unreal build, packaged executable, rendered downtown, frame-rate measurement or real WebRTC game session has been produced.**

Read `ACCEPTANCE.md` for the required engine/hardware checks. The CI workflow is deliberately named “Source checks (not an Unreal build)”.

## Next engineering milestones

1. **UE compile gate:** run Build-Unreal and Test-Unreal; fix UHT/compiler issues against the pinned engine if any. Source has been reviewed but is not engine-compiled.
2. **Hospital block:** acquire/import assets, author the third-person character, one street, hospital exterior/interior and one configured car. Connect Doctor patient/tool presentation.
3. **Living downtown:** author the other four locations, activity points, sidewalks, signal-controlled crossings and traffic loops. Add ambient audio and authored NPC animations.
4. **First successful slice:** package and stream the full browser → hospital → Doctor → reward → outside → vehicle loop; record performance evidence.
5. **Other careers:** implement Firefighter, then Restaurant, using native job logic and presentation-only Blueprints. Add account services only after the local loop works.

No paid GPU infrastructure, external services, art purchases or public deployment were performed.
