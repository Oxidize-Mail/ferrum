# Domain entity types — design decisions

Decisions for issue [#27](https://github.com/Oxidize-Mail/ferrum/issues/27): the C++ types for
the data described in `reputation-xp-schema.md` (#10). These types cross the `IDatabase`
boundary, so they're also part of #11's answer to *"what does a row or result look like as it
crosses the boundary."*

These were agreed before splitting the work into the rep side and the XP side, so both halves
read as one codebase.

## Decisions

### 1. Mirror the tables or the domain?

**Mirror the domain.** Where several tables describe one concept, one class represents them. For
example, a rep grant holds its recipients rather than exposing `RepAudit` and `RepRecipient` as
separate types. Storage-only details, such as `rep_recipient.giver_id` (which exists only so the
cooldown index works), stay in the backend and aren't visible to command code.

### 2. Illegal states

**Yes, the type makes them impossible to write.** The "both or neither" and "if X then Y" rules
(`revoked_at`/`revoked_by`, `source = admin` ⇔ `awarded_by`, `source = weekly_checkin` ⇔
`reference_id`, `source = thanks_message` ⇔ `msg_id`) are enforced through setters. There's no
setter that sets one half of a pair without the other, so callers can't build a combination the
schema's `CHECK` constraints would reject.

### 3. Plain aggregates or classes with invariants?

**No, there's nothing left for a constructor to check.** Because the setters (decision 2) already
guarantee validity, constructors don't repeat those checks.

### 4. Timestamps

**`std::chrono` time points.** The ISO-8601 TEXT format in the schema doc is a storage detail. It
exists only in the backend, which converts to and from `std::chrono` at the binding layer.

### 5. Snowflakes

**`uint64_t`, with no DPP dependency.** The entity headers don't include DPP. Code at the edge
casts `dpp::snowflake` to `uint64_t` before building an entity. The signed cast for SQLite stays
at the binding layer, as the schema doc says.

### 6. Source columns

**`enum class`, with its string conversion living next to it.** Each enum has a function in the
same header that converts it to its text form. The enum owns the enum-to-text mapping, so there's
one place to update when a value is added.

### 7. New vs stored

**Same type.** The value you insert is the same type as the value you read back. There aren't
separate "new" and "stored" types for `audit_id` and `event_id`, which the database assigns.

### 8. Derived values

**Calculated from the last good DB insert for the user's rep and XP.** The cached `reputation`
and `xp_total` values on the user come from the most recent successful write to the log tables
(`rep_recipient` and `experience_event`), not from values that command code tracks separately.
Level is never stored. It's computed from `xp_total` by the level-curve pure function whenever
it's needed.

## Constraints from the issue

These come from #27's "Done when" section and apply regardless of the decisions above:

- The types don't mention SQLite: no `sqlite3` headers, no TEXT timestamp format, no
  signed-snowflake cast.
- The rep-amount-by-chain-length formula (1 ≤ n ≤ 10) and the level-from-`xp_total` loop are pure
  functions with tests.
- Everything builds under `project_warnings` and is linked into `test_ferrum`.
