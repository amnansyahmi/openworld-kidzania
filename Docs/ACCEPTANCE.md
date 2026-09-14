# Vertical slice acceptance

None of the engine or gameplay checks below is marked passed by source generation. Run them on the actual UE/GPU development PC and attach results to the branch/PR. A passing launcher build is not an Unreal game test.

## Gates

| Gate | Required evidence |
| --- | --- |
| Native compile | UE 5.7 UHT and Development Editor build succeed from a clean checkout |
| Unreal automation | `Test-Unreal.ps1` reports both Doctor rules and SaveRoundTrip passing |
| Content | Authored map and correctly licensed assets exist; no missing references, shaders or LFS pointers |
| Local standalone | Player spawns in real Downtown with the intended mesh, camera and animations |
| Browser path | Actual Unreal video/audio and responsive input in a desktop browser |
| Full loop | Continuous recording of the complete Doctor + driving path below |
| Performance | Recorded hardware/settings and measured frame times/stream behaviour; 60 FPS is not asserted without measurement |

## First successful loop

1. Start local signalling, the Unreal game and Web launcher; open browser and select Play.
2. Confirm streamed frames show the actual 3D scene; enable input and audio.
3. Walk, run and jump; rotate the follow camera near walls; check camera collision and animation transitions.
4. Explore the small downtown; see pedestrians moving between activities and vehicles moving/stopping.
5. Walk through the hospital entrance and into the treatment area.
6. Start Doctor shift; see an actual patient and the intended shift presentation.
7. Complete an interactive job; verify wrong input does not skip stages and the final reward is issued once.
8. Verify the wallet increases by the configured amount and Doctor XP increases by 25.
9. Walk back outside, enter the configured car and drive around the loop.
10. Stop and exit the vehicle safely; verify the player is visible, walking and controllable again.
11. Close/restart the game; verify completed-job balance/XP and the last saved checkpoint survive.

## Edge-case checks

- Try E with a wall between player and target and at excessive distance: no interaction.
- Start a shift twice, choose out-of-order actions, cancel with X and walk away: no early or duplicate reward.
- Simulate a save write failure in an isolated test profile: no in-memory reward stays credited; after recovery, explicit retry grants exactly one receipt.
- Back up the save; test unreadable/future-version files: loading reports a problem and never overwrites them silently.
- Test F5 while airborne and on different maps: no invalid cross-map checkpoint restore.
- Enter/exit cars parked beside walls and moving cars; no clipping or unsafe exit. Test reverse braking and handbrake release.
- Disconnect/reconnect and switch browser focus while accelerating: ensure inputs release, no stuck movement and no second browser gains simultaneous control. This requires actual Pixel Streaming testing.
- Block an NPC path, remove an activity target and exercise crossing red/green transitions. NPCs recover from failed paths; occupied crossings stop cars.
- Test seating, talking, shopping, entering/exiting and facilities with actual animation hooks; an enum change is not sufficient evidence.
- Run day/night and enter the hospital at both extremes; check exposure, shadows and visibility.
- Test 1080p High/Balanced and 720p Mobile server presets. Record frame-time and latency evidence over a representative exploration/job/drive run.

## Known foundation limits to resolve

The native clinic is currently choice-based, the player disappears inside the car instead of using a seated driver presentation, simple door rotation lacks occupancy sensing, activity animations/paired interactions need authoring, and custom adaptive render quality/touch input are absent. A final polished slice must address the relevant presentation/interaction gaps, not merely compile this code.
