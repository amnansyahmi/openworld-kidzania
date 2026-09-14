# Career design and implementation sequence

The initial successful slice is Doctor plus free exploration and a driveable car. Firefighter and Restaurant are specified here so later systems follow the same rules, but they are not implemented playable careers in this foundation.

## Doctor — first playable career

Current native sequence: interact with hospital workstation → start a clinic case → select the correct action for each of three stages → persist reward/XP → return to exploration. Cases alternate between a scraped-knee visit and a routine check-up. Number keys select actions, incorrect choices keep the current stage and reduce the completion reward. This supplies a functioning gameplay state machine for presentation to connect to.

Final slice presentation: enter the hospital on foot; start shift with a uniform transition; greet an animated patient; inspect the visit card and visible cues; select the appropriate tool tray; carry out a gentle interactive treatment/check-up; explain completion; show the earned KidzCoins. Keep cases light and fictional, with no graphic injury or frightening medical detail. The spatial tool interaction and patient animations remain to be built.

For each stage, native logic must verify the active job, patient, expected action, proximity and completion condition. Selecting a UI label alone must not bypass a future hands-on task.

## Firefighter — second career

Job flow: station dispatch → accept a minor, non-graphic emergency → equip uniform → enter fire truck → drive to the incident → park in a marked area → take hose → extinguish target zones → confirm the site is safe → complete and save reward.

Implement a native incident actor that owns target heat/progress and validates hose direction, range, line of sight and accumulated contact time. Vehicle arrival checks belong to a world trigger and must verify the assigned vehicle. Do not advance arrival merely when the player presses a button. Use small controlled incidents without casualties, weapons or frightening imagery. Driving and hose controls should remain forgiving and readable.

This requires fire-truck art/physics, hose interaction, incident effects, dispatch UI/audio and a real job definition/executor. They do not exist yet.

## Restaurant — third career

Job flow: accept an order from a customer → read a concise order card → collect the right ingredients → perform an interactive preparation sequence → plate the food → deliver to the correct customer → complete and save reward.

Native order IDs tie ingredient/preparation progress and serving to the correct customer. Wrong ingredients allow recovery. Do not use time pressure as the only challenge. Presentation needs counters, food props, cooking/serving actions and seated customers. This does not require a web backend for the local slice.

## Shared extension rules

- Use stable career, case, objective and receipt IDs; display names are presentation.
- Keep one active job in the local slice. Reject overlapping jobs and duplicate payout.
- Build reusable native objective validation when the second career requires it; do not force driving/hose/cooking into the Doctor multiple-choice rules.
- Save completion and payout together. Make cancellation and failed-save retry explicit.
- Avoid random scoring or rewarding a job merely because its start button was clicked.
- Add meaningful domain tests for invalid actions, cancellation, completion and duplicate rewards, then engine tests for the physical interaction.
- Only add online accounts, leaderboards and multi-user economies after the local loop and service authority model are tested.
