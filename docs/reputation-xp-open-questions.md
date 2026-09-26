# Reputation & XP schema — still to decide

Companion to `reputation-xp-schema.md`, for issue [#10](https://github.com/Oxidize-Mail/ferrum/issues/10).
These are the things that came up while drafting the schema that still need a decision from
both of us before it's "done."

## Abuse prevention (the big one — none of this is decided yet)

For each of these, we need to pick: enforced by the schema (`UNIQUE`/`CHECK`, can't be
bypassed by a buggy command), or enforced by application code (a cooldown check, easier to
change later)?

- **Self-rep.** Giver and recipient now live in separate tables (`repAudit.giver_id` vs.
  `rep_recipient.recipient_id`), so this can't be a same-row `CHECK` constraint in SQLite —
  it has to be application-enforced (or a trigger). Confirm we're OK with app-only here.
- **Repeat-repping the same target.** Is there a cooldown per (giver, recipient) pair, or
  just a global per-giver cooldown? Does it reset daily/weekly/never?
- **Rep-trading rings** (A reps B, B reps A, back and forth). Do we detect/prevent this at
  all for v1, or accept the risk and revisit later?
- **XP farming via short/spam messages.** What actually triggers an `experience_event` —
  every message, messages over some length, messages spaced some minimum time apart? This
  isn't just an abuse question, it's also undefined in the schema itself right now.

## Open schema questions

- **`experience_granted` as INTEGER** — we confirmed `level` is `REAL` (fractional levels,
  e.g. `3.25`), but didn't explicitly confirm whether XP awarded per event should also be
  fractional or is always a whole number. Doc currently assumes whole numbers.
- **Chain-length multiplier formula.** We decided `chain_length` gets snapshotted onto
  `repAudit`, but not the actual formula for turning chain length into `rep_amount`. Linear?
  Capped? Diminishing returns for very long chains?
- **Do we cap chain length** we're willing to walk back (performance/abuse: someone could
  reply-chain very deeply to inflate rep)?

## Indexes to revisit once the above is settled

The three indexes in the main doc cover cooldown checks, chain traversal, and leaderboard
reads. Once the abuse rules above are decided, check whether they need their own index (e.g.
a per-(giver, recipient) cooldown would want `rep_recipient(recipient_id, audit_id)` joined
back to `repAudit.given_on_utc`, rather than what's there now).

## Housekeeping

- [ ] Confirm who "the other collaborator" is for the sign-off checkbox in the main doc.
- [ ] `PRAGMA foreign_keys = ON` needs to actually get wired into the app's DB connection setup — noted in the design doc, not yet implemented.
