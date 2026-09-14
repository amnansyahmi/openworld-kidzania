# Backend boundary

No backend, authentication provider or database is deployed in milestone 0.1. The Unreal process owns a single local player's jobs, balance and save; Web owns only connection preferences. This is intentional: no browser coin API and no imitation login flow.

`Contracts/progress-v1.schema.json` specifies the **future** service representation. It is not the binary Unreal SaveGame format and is not currently read or written by the game. Implement an explicit adapter and migration when introducing accounts; do not rename `.sav` to JSON.

Future account milestone:

- Authenticate the launcher through a server session, then issue a short-lived, single-use game-session ticket.
- Allocate one isolated game process and save namespace per user. A shared Pixel Streaming camera is not multiplayer.
- Only an authenticated game process may submit reward receipts. The server validates the job outcome and awards the configured reward in one transaction. Never trust browser-submitted balances.
- Enforce a unique `(profile_id, receipt_id)` key and atomically update the coin ledger, career XP and completed mission record. Retry requests with the same receipt ID.
- Parent/guardian account UX, child data handling, recovery, retention and region must be designed before public account collection. No identity data is collected by this local build.

Suggested future relational tables: `profiles`, `character_choices`, `career_progress`, `mission_runs`, `reward_receipts`, `wallet_entries`, `save_snapshots`. Provider selection and actual migrations remain unimplemented.
