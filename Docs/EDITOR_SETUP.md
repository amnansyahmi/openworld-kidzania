# Open and build in Unreal Editor

## Prerequisites

Use a Windows development PC with Unreal Engine **5.7**, the Visual Studio C++/Windows SDK toolchain supported by that engine installation, Git LFS, Node.js 22.18+ and a GPU supported by Pixel Streaming encoding. This repository does not bundle Unreal, Epic template content or third-party assets. See [Epic's Pixel Streaming reference](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-pixel-streaming-reference) for hardware details.

```powershell
git clone https://github.com/amnansyahmi/openworld-kidzania.git
cd openworld-kidzania
git switch feature/unreal-foundation
git lfs install
git lfs pull
./Infrastructure/Scripts/Build-Unreal.ps1 -UnrealRoot 'C:/Program Files/Epic Games/UE_5.7'
./Infrastructure/Scripts/Test-Unreal.ps1 -UnrealRoot 'C:/Program Files/Epic Games/UE_5.7'
```

Open `Unreal/KidzCity.uproject` after the build. Allow the enabled plugins to load. If the Editor asks to rebuild modules, allow it. There is deliberately no default downtown map until it has been authored. Do not enable the original PixelStreaming plugin alongside PixelStreaming2.

## Author the player first

1. Create `/Game/Core/BP_CityGameMode`, parent `CityGameMode`.
2. Create `/Game/Characters/BP_CityPlayer`, parent `CityCharacter`. Assign a licensed realistic-proportion humanoid skeletal mesh. Match capsule height, mesh orientation and ground offset to its skeleton; do not model the player out of primitives.
3. Create an Animation Blueprint derived from `CityAnimInstance` for the selected skeleton. Retarget properly licensed idle, walk, run, start, stop, jump and landing clips. Make a BlendSpace using `Speed`, a grounded/in-air state transition using `Falling`, short transition blends, turn-in-place where necessary, and foot IK. Avoid foot sliding and capsule/mesh mismatch.
4. Set `BP_CityGameMode.DefaultPawnClass` to `BP_CityPlayer`. The base controller constructs its Enhanced Input mappings in C++; no InputAction assets are required for the foundation.
5. `CityHUD` is a native functional development HUD. Replace it with a compact UMG presentation bound to `CityProgression.OnChanged` after gameplay validation. It is not the final art direction.

## Hospital and Doctor

Create `/Game/Locations/BP_Hospital` from licensed/environment meshes or original work. Make the lobby and treatment room physically enterable with continuous floor collision; a facade with a teleport menu is not sufficient.

Place a `CityDoctorStation` in the treatment room or as a Child Actor in the hospital prefab. Assign a real reception/table/tool-tray mesh to its `Mesh` component, with Visibility blocking. Its root should be within reach of the intended player position. The focus trace checks 260 cm from the player to the hit point, and the job starts within 400 cm of the station root. The shift cancels beyond 500 cm. Design the workstation around those distances.

Connect `OnShiftStarted` and `CityProgression.OnChanged` to an authored patient, uniform presentation and treatment animations. The current native interaction uses number keys 1–3 to select contextual actions, X to cancel. Two visit types alternate across completed jobs. For the final mini-game, implement tool selection and treatment interactions that validate patient stage before calling progression; do not grant rewards from Blueprint UI events.

For doors, create a `CityDoor` subclass and use a door mesh with its hinge aligned to the root. Set `OpenAngle` and collision appropriately. Test both player and NPC clearances. The simple native rotating door does not yet prevent closing onto an occupant; add occupancy sensing before treating it as final. For the initial hospital route, an unobstructed automatic/open entrance is acceptable.

## Vehicle

Create `/Game/Vehicles/BP_CityCar`, parent `CityVehicle`. Assign a rigged skeletal car mesh, a valid vehicle PhysicsAsset and a Chaos vehicle AnimBP. The C++ defaults expect `wheel_fl`, `wheel_fr`, `wheel_rl`, `wheel_rr`; change WheelSetups in the subclass to match the actual rig. Set wheel radii/width, suspension travel, centre of mass, mesh physics/collision and bone orientation from that asset. Enable physics simulation on the vehicle mesh as required by the Chaos setup.

The native class supplies torque, steering, acceleration/braking/reverse routing and possession. W/S accelerate or brake before changing direction; A/D steer; Space is handbrake; E exits when stopped and a clear position beside the vehicle is found. Driver entry is unavailable during a clinic task. Tune `ExitSideOffset` to the actual vehicle width. Entry currently hides the character while in the car; a seated driver mesh and entry/exit animation are presentation work, not implemented here.

## Downtown asset assembly

Create the named Blueprints in `Unreal/Config/Downtown.manifest.json`:

| Blueprint | Required content |
| --- | --- |
| `BP_DowntownEnvironment` | Authored road/sidewalk layout, plaza, curbs, foliage, street lights and collision |
| `BP_Hospital` | Enterable lobby/treatment room and configured Doctor station |
| `BP_FireStation` | Fire bay, small briefing space and building entrance |
| `BP_Restaurant` | Dining/counter/kitchen spaces and entrance |
| `BP_Supermarket` | Aisles, checkout and entrance |
| `BP_Bank` | Lobby, counters and entrance |
| `BP_CityCar` | Configured player-driveable Chaos vehicle |
| `BP_Citizen` | `CityPedestrian` subclass with a mesh and authored animation logic |

Manifest positions are an initial layout in Unreal centimetres. Revise them to fit actual building dimensions and street widths. They are not a finished city design. Keep the public-road network small and looped for the vertical slice.

In the Editor Python console, use your checkout's absolute path:

```python
import sys
sys.path.append(r'C:/Projects/openworld-kidzania/Unreal/Scripts')
import downtown
downtown.validate()
downtown.assemble()
```

Validation aborts if required assets are missing. Assembly uses actual Blueprint assets and refuses to overwrite an existing map. It creates `L_Downtown`, its actors, PlayerStart and GameMode assignment. It does **not** manufacture block buildings or replace missing assets with cubes. Once the map exists, subsequent editing happens in the Editor.

## Navigation, pedestrians and traffic

1. Add NavMeshBoundsVolume over walkable streetside and interior areas. Inspect the green navmesh overlay. Floors/doorways must have valid clearance. Keep normal pedestrian navigation on sidewalks; use deliberate crossing links/areas to prevent random routes across traffic lanes. The source does not automatically author those links.
2. Place `CityActivityPoint` actors for walking destinations, conversation spaces, seats, queue points, checkout, facilities, building entry/exit and job positions. Use multiple points; without reachable points the native pedestrian has no destination. Set durations and point rotations for the associated animations. Ordinary movement should occur between points, not teleport between them.
3. Bind `BP_Citizen.OnActivityChanged` to the correct animation/montage. Paired conversation, seat alignment and facility attachments require Blueprint presentation and authored sockets. A state name alone is not a finished behaviour.
4. For crossings, place `CityCrossingSignal` and a `CityActivityPoint` of type Crossing on the far sidewalk. Set its EntryOffset on the near sidewalk and assign the signal. The near-side approach path must not itself cross the road. Ensure the far-side route crosses the marked lane and reaches the sidewalk.
5. Place `CityTrafficRoute` actors; edit their spline points into closed lane loops. Add `CityTrafficVehicle` instances with real meshes, correct road-height pivot and swept root collision. Set start distances far enough apart to avoid initial overlap. Configure each stop's signal and distance along that route. Check stopping positions before the zebra crossing, not inside it.
6. Background traffic is kinematic spline traffic and cannot be entered. Player cars are the separate Chaos class. Use both in the slice.

## Lighting, defaults and packaging

Add movable sun/moon DirectionalLights, SkyAtmosphere, a movable SkyLight with appropriate real-time capture, ExponentialHeightFog and a calibrated PostProcessVolume. Assign the sun/moon to `CityDayNight`; set Atmosphere Sun Light indices 0/1. Balance exposure for both the street and hospital. Warm fixtures and streetlights must still illuminate interiors/roads at night. See `ART_DIRECTION.md` for the visual budget.

Set Project Settings → Maps & Modes → Editor Startup Map and Game Default Map to `/Game/Maps/L_Downtown`. Save all assets and project settings. The world-specific GameMode should remain `BP_CityGameMode`.

Run Standalone Game first, then:

```powershell
./Infrastructure/Scripts/Build-Unreal.ps1 -UnrealRoot 'C:/Program Files/Epic Games/UE_5.7' -Package
```

Commit authored `.uasset`/`.umap` files through Git LFS. Check imported asset redistribution rights first. Generated build, cache and saved files are ignored. Package only after the browser acceptance path works locally in Standalone.
