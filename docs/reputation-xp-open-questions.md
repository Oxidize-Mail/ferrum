# Reputation & XP schema — still to decide

Companion to `reputation-xp-schema.md`, for issue [#10](https://github.com/Oxidize-Mail/ferrum/issues/10).
These still need a decision from both of us before the design is "done."

## Formulas

- **Rep rounding.** `rep_amount` is an INTEGER, but the formula produces decimals. Floor,
  round, or ceiling?
- **How gentle the diminishing returns are.** With 0.98, the 10th message in a chain is still
  worth 8.3 rep versus 10 for the first, so it's close to linear. Is that the intent, or
  should the factor be lower (e.g. 0.9)?

## Rep mechanics

- **Slash-command amount cap.** The rep slash command takes the amount as an argument. What's
  the maximum? Without one, any member can hand out 1,000,000 rep in one command. Should the
  cap be a fixed number (e.g. the ~91 a full 10-message chain earns), or vary by role?
- **Chain length for a thank-you that isn't a reply.** Is it 1 (the thank-you message
  itself), or 0?
- **Detecting "thank you."** What phrases count ("thanks", "ty", "thank you")? This is app
  logic, not schema, but it decides what ends up in `repAudit`.
- **Revoked grants and the cooldown.** Does a revoked grant still count toward the 1-hour pair
  cooldown? (Suggestion: yes. It's simpler and stops a revoke-and-regrant loop.)
- **Self-rep as a CHECK.** `giver_id` is on `repRecipient`, so `CHECK (giver_id <> recipient_id)`
  would cost nothing and make self-rep impossible even if a new command forgets the app check.
  Worth adding on top of the app check?

## XP mechanics

- **What counts as AFK.** Decided: AFK users don't earn voice XP. Does "AFK" mean only
  being in the server's AFK channel, or also self-deafened, self-muted, or alone in a
  channel? Discord only moves idle users to the AFK channel after a timeout, and only if the
  server has one configured.
- **Weekly check-in event ID.** Is `reference_id` the Discord message ID of the check-in post,
  or an ID from a table of our own? If we ever want to query "past check-ins," it probably
  needs its own table.

## Archival and compaction (proposal — needs agreement)

The idea on the table: once a month, archive old data and replace a month of per-minute voice
rows with one summary row per user.

**Recommendation: design it now, but don't build it until the database size is actually a
problem.** At roughly 110k voice rows/month (20 members, 3 h/day each), a year is about
1.3M rows, maybe 50–100 MB. That's well within what SQLite handles comfortably, and every
compaction permanently gives up some audit detail.

When compaction is built, this is the proposed shape:

- **Compact only voice XP.** Admin awards and weekly check-ins are few, and each row carries
  something a summary can't: *who* awarded it, or *which* event it was for. Keep them as-is.
- **Only compact months older than a revocation window** (e.g. 3 months). Once voice rows are
  summarized they can't be revoked one at a time, so there must be a cutoff after which voice
  XP is final. Pick the window length.
- **Summary row:** one `experienceEvent` per user per month with `source = 'voice_monthly'`
  and `occurred_at` = the first instant of that month. `experience_granted` is the sum of that
  user's **non-revoked** voice rows for the month. Revoked rows are dropped, not summed.
- **One transaction per month:** insert the summary rows, delete the originals, and check
  that each affected user's `xp_total` is unchanged. Compaction must never change anyone's
  XP. If a total changes, roll back.
- **Make it safe to re-run:** a partial unique index on `experienceEvent (user_id, occurred_at)
  WHERE source = 'voice_monthly'` means an accidental second run fails instead of doubling XP.
- **Schema impact:** `'voice_monthly'` must be added to the `source` CHECK list.
- **"Archive" = export, then delete.** If the originals should be kept somewhere, dump them
  to a file (e.g. monthly CSV/SQLite file) *before* the delete, inside the same job.

**Don't compact the rep tables** (`repAudit`, `repRecipient`, `message`). They're low-volume
(at most 10 messages per grant), and the full history is the whole point of keeping them:
disputes, revocations, and detecting rep-trading rings later. If the stored message *content*
becomes a concern (size or privacy), the lighter option is to null out `content` after N months
and keep the rows, so chains still reconstruct. That would make `content` nullable.

Decisions needed:
- [ ] Agree to "design now, build when needed," or build compaction in v1?
- [ ] Revocation window before voice XP becomes final (suggest 3 months).
- [ ] Keep an export of the compacted rows, or just delete?
- [ ] Ever clear old `message.content`, and if so after how long?

## Other

- **Privileged intent.** Storing full message content depends on Discord's Message Content
  intent. Make sure it's enabled for the bot.

## Housekeeping

- [ ] `PRAGMA foreign_keys = ON` needs to actually be wired into the app's DB connection setup.
      It's noted in the design doc but not implemented yet.
- [ ] Default title spelled `'peasant'` in the doc (the answer said "peasent"). Confirm.
