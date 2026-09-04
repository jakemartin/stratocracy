# Gate: the 2026-09-04 editor-driver route ruling, RE-GATE — strat-integration-reviewer

## Scope

Working-tree diff in the MAIN TREE, `E:/MultiAgent/Stratocracy`, branch `master`, over HEAD
`369513facaee461b73f4e8e825b668f8e5a5a9d6`. Re-gate after a `BLOCK`; the whole change was
re-read, not only the prior findings. Nothing staged, nothing committed by this gate.

`git status --porcelain` printed exactly:

```
 M CLAUDE.md
 M Tools/architect/state/content.md
?? Tools/architect/gate_reports/2026-09-04-editor-driver-route-ruling.md
```

Two modified files, **91 insertions and 9 deletions**, plus the prior gate report, untracked.
Working-tree content hashes (sha256) at the time of this gate:

```
852c4ab3c5b4c43d1c2b3feec9eed581a937d97abd6642579317f8187fc77c1c  CLAUDE.md
8d39e022d6564fbe5dc2d477aadd2bea617d513153d5d9955112edee0b229293  Tools/architect/state/content.md
8cb6f0363aea300bd5fd5793170d12f93ffd23c2610d9d2d79ba6d13d33b5634  Tools/architect/gate_reports/2026-09-04-editor-driver-route-ruling.md
```

`git diff --stat -- Source Content Config Data Stratocracy.uproject` printed NOTHING. No code,
asset, config, data or `.uproject` change is present, as intended.

**The gate-report directory.** `git diff --stat -- Tools/architect/gate_reports` printed nothing,
so every TRACKED report is byte-identical to HEAD. The prior report is UNTRACKED and cannot be
checked that way, so it is pinned by content instead: sha256 `8cb6f036…` above, size 17824 bytes,
mtime `2026-09-04 17:28:44 -0400` — which PRECEDES both repair writes (`CLAUDE.md` 17:29:55,
`content.md` 17:30:53). Consistent with it being untouched by the repairs. That is evidence, not
proof: no earlier hash of that file exists in this tree to compare against, so this line records
the hash so the NEXT reader can.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside `StratBridge`/`StratRules` | pass — the lowercase-initial call grep over `Source/`, minus the two permitted directories, returned zero lines; no `Source/` file is modified at all |
| 3 | No vendored header in front of UHT | pass — no header changed |
| 4 | `TPimplPtr` not `TUniquePtr` for `FStratBridge` members | pass — no header changed |
| 5 | Module arrows unchanged and correct | pass — derived below, 16 rows / 10 field names |
| 6 | New modules registered, unless no module object | pass — `IMPLEMENT_MODULE` appears in exactly `StratBridge.cpp`, `Stratocracy.cpp`, `StratPlay.cpp`, `StratUI.cpp`; all four are in the `.uproject` Modules array; `Source/StratRules/` carries none and is correctly absent |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` literal in gameplay C++ | pass — the only non-comment hit is the import commandlet's `FString::Printf(TEXT("/Game/StratData/%s"), …)`, a standing exception; every other hit is prose forbidding the literal |
| 9 | No widget-side arithmetic | pass — no `StratUI` file changed |
| 10 | Lane discipline | pass — see Observations |
| 11 | Prose blocks present | pass — every added block is why-first, dated, and names its authority |
| 12 | Nothing staged or committed | pass — porcelain shows two ` M` entries and one `??`; no index column |
| — | Repair of prior finding 4 (`CLAUDE.md` limit sentence) | pass |
| — | Repair of prior finding 3 (`content.md` header pointer) | **BLOCK** — finding 4 (the words are right; the write is unattributed) |
| — | Repair of prior finding 1 (2026-08-29 entry) | **BLOCK** — finding 2 (the stamp overclaims) |
| — | Repair of prior finding 2 (2026-09-02 entry) | pass |
| — | Claims the ruling now falsifies, re-swept by shape | **BLOCK** — findings 1 and 3 |
| — | Coherence of `CLAUDE.md` as a whole | pass — see Observations |
| — | Fallback obligations on the `content.md` writes | **BLOCK** — finding 4; partial, see Observations |
| — | `strat_banner_sweep.py` | pass — re-run here, `SWEEP CLEAN`, exit 0, plain and `--explain` |

### The four repairs, checked one by one

**Prior finding 4 — repaired, correctly.** `CLAUDE.md` now reads *"Nothing here widens the lane
rule for a session where **`strat-editor-builder` can reach the editor** — when that agent can
work at all, `Content/` is its lane and only its lane."*, with the retired wording quoted in the
parenthetical and explained as the gloss the ruling deletes. The limit now turns on the same
subject as the condition. No finding.

**Prior finding 3 — the WORDS are repaired.** The header pointer now reads *"who may do the ASSET
WORK, BY ANY ROUTE, when `strat-editor-builder` cannot reach the editor"*, with *"who may drive
the live editor"* quoted as the retired text and the reason given — *"a citation landing here
must not be sent to a narrower rule than the one that governs"*. That is right. The WRITE is a
separate matter: finding 4.

**Prior finding 2 — repaired.** The 2026-09-02 bullet's *"still the gap neither clause models: A
TOOL ABSENT FROM THE CLIENT SURFACE WHILE THE SERVER STILL SERVES IT"* now carries a stamp
retiring it where it sits, and the stamp's claims check out: the clause does now state its
condition at the tool surface, and it does name that pass by date.

**Prior finding 1 — stamped, but the stamp overclaims.** Finding 2 below.

### Is the written condition unchanged since 2026-08-26? YES, and it is load-bearing

All four repairs rest on the claim that the OPERATIVE condition sentence has said *"absent from
the session's tool surface"* since 2026-08-26 and is untouched here. Verified two ways:

- The sentence appears in this diff as CONTEXT, not as an added line: *"**The editor-driver
  clause.** When `execute_script` is absent from the session's tool surface,"*.
- `git log -S'absent from the session' -- CLAUDE.md` returns exactly two commits, the later of
  which is **`4b206ab`, dated 2026-08-26** (*"The guard built to catch a stale provenance
  sentence…"*). Nothing since. The gloss moved; the rule did not.

### Module arrows, derived not trusted

The `.Build.cs` extraction printed **16 rows** across five modules — all five present, so the
extraction did not silently fail:

```
StratBridge  PublicDependencyModuleNames    Core,CoreUObject,Engine,Stratocracy
StratBridge  PrivateDependencyModuleNames   (none)
StratBridge  PrivateDependencyModuleNames   StratRules
StratBridge  PublicIncludePaths             StratBridge,StratRules
StratPlay    PublicDependencyModuleNames    Core,CoreUObject,Engine,StratUI
StratPlay    PrivateDependencyModuleNames   StratBridge,UMG,EnhancedInput
StratPlay    PublicIncludePaths             StratPlay
StratRules   PublicDependencyModuleNames    Core
StratRules   PrivateDependencyModuleNames   (none)
StratRules   PublicIncludePaths             StratRules
StratUI      PublicDependencyModuleNames    Core,CoreUObject,Engine,StratBridge
StratUI      PrivateDependencyModuleNames   UMG,Slate,SlateCore
StratUI      PublicIncludePaths             StratUI
Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,EnhancedInput,
                                            AIModule,NavigationSystem,StateTreeModule,
                                            GameplayStateTreeModule,Niagara,UMG,Slate,Json,StratRules
Stratocracy  PrivateDependencyModuleNames   (none)
Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,…
```

The sixteenth row is still GUARDED, read from the file rather than inferred from the extraction:
`StratBridge.Build.cs` carries `bool bCompileVendoredRulesHere = Target.LinkType !=
TargetLinkType.Monolithic;`, then `if (!bCompileVendoredRulesHere)` and inside it
`PrivateDependencyModuleNames.Add("StratRules");`. `StratRules` appears in no
`PublicDependencyModuleNames` and in no UNCONDITIONAL private array. No drift, no cycle;
`StratUI` has gained nothing.

The field census printed **TEN** names: the three graph fields in their `AddRange` form, plus
`PrivateDependencyModuleNames.Add`, `PrivateDefinitions.Add`, `PCHUsage =`, `bUseUnity =`,
`ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`, and the known string-literal false
positive — printed on this run as `DGE=`, the tail of
`PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + …)`. The standing block records that
false positive as `BRIDGE=`; it is the same literal and the same non-field, and the census is
deliberately left un-narrowed rather than taught to skip it. **No eleventh name.**

### strat_banner_sweep.py, re-run here

Run from the repo root with no pipe, then again with `--explain`. Both exit 0.

```
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 389 entries, all Success (2026.09.04-16.30.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=389, COMPLEX/_CLASS=0
  report identity: mtime=1788539420, reportCreatedOn='2026.09.04-16.30.20'
  suite claims found: 62 (1 live, 61 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

`--explain` enumerates the 62 claims, 61 stamped and one live. **This vouches for nothing under
gate.** The sweep collects claims by the SUITE-COUNT shape and the TRACKED-ITEM shape. It
collects no claim of the shape *"this is the user's to rule"* or *"the gap neither clause
models"*, and it walked past all of findings 1, 2 and 3 below in files it reads.

### The claim-shape sweep this gate ran, and its instrument's control

The seat's own sweep terms were the phrases the PRIOR GATE had quoted. That is a phrase sweep
wearing a shape sweep's name, and it is short by the same measure as the sweep before it: it
cannot see a live claim phrased in words no prior gate happened to quote. This gate swept, over
`CLAUDE.md`, `Tools/architect/state/*.md` and `.agents/`:

`ABSENT FROM THE CLIENT SURFACE`, `neither clause models`, `gap neither clause`, `who may drive`,
`drive the live editor`, `driving the live editor`, `driving the editor`, `editor IS reachable`,
`editor is unreachable`, `s to rule`, `unruled`, `sentence of its own`,
`not the one the clause names`, `still open`, `mirror-image`, `remains open`, `no clause`,
`falls inside`, `the route to the editor was`, `covered by the clause`, `not modelled`.

**The instrument needed a control and got one.** The first run used `grep -rniF` and returned
ZERO lines for EVERY term, including terms this gate had already read with its own eyes — the
recorded `-i -F` abort on this box. A control that must hit (`grep -rn "editor-driver clause"`)
exposed it; re-running without `-F` produced the hits below. An empty sweep here is
indistinguishable from a clean one, which is why the control ran first.

Hits under `.claude/worktrees/` are stale separate checkouts and are out of scope for this diff.

## Findings

### Finding 1 — BLOCK. This diff creates a contradiction inside `content.md`: an entry says a gap is "still open" that the same diff stamps closed.

`Tools/architect/state/content.md`, in the **2026-08-31** entry, the sentence closing the
BLOCKED-BY-THE-CLIENT paragraph — locate it by the words *"falls inside any existing clause"*:

> *"Whether an actor whose tool is BLOCKED BY THE CLIENT, rather than absent from it, falls
> inside any existing clause is the user's to rule; **the entry below this one already raised the
> mirror-image gap and it is still open.**"*

The mirror-image gap it points at is the 2026-08-29 entry's *"THE REAL GAP IS NARROWER…"*
paragraph — the entry BELOW it in this reverse-chronological file. **This diff stamps exactly
that paragraph as closed**, in the words *"RULED, AND EVERY CLAUSE OF THE PARAGRAPH ABOVE IS
RETIRED."* So the pointer's trailing clause is false the moment this diff lands, and it carries
no stamp of its own.

This is not a new species of defect; it is the prior gate's findings 1 and 2 wearing a third
face, in the same file the repair pass edited three times. It matters most of the four because it
is the site that keeps the STILL-OPEN blocked-by-client gap legible: a reader who follows the
pointer lands on a paragraph that now says the opposite of what the pointer promises.

Owner of the fix: the `coordinator`, under `content.md`'s fallback condition — the same authority
under which the three stamps were written.

---

### Finding 2 — BLOCK. The stamp on the 2026-08-29 entry overclaims, and git falsifies one clause of it.

`Tools/architect/state/content.md`, in the stamp this diff adds to the 2026-08-29 entry — locate
it by the words *"A correct reading and a misreading of the same rule stood four lines apart"*:

> *"**A correct reading and a misreading of the same rule stood four lines apart in one entry for
> two days**, and the misreading is the one that got carried forward into two later entries **and
> a header pointer**."*

Two problems, the first checkable from git:

1. **The header pointer PREDATES the misreading and cannot have been carried forward from it.**
   `git log -S'WHERE THE OTHER PERMISSION LIVES' -- Tools/architect/state/content.md` returns a
   single commit, **`4b206ab`, 2026-08-26**. `git log -S'THE REAL GAP IS NARROWER' --
   Tools/architect/state/content.md` returns **`1a3520b`, 2026-08-29**. The pointer's *"who may
   drive the live editor"* was written THREE DAYS BEFORE the entry the stamp says it descends
   from. Both sentences share the same gloss; neither is descended from the other, and the stamp
   asserts a direction of travel git contradicts.
2. **"for two days" matches no reading of the dates.** The entry is dated 2026-08-29 and the stamp
   retiring it is dated 2026-09-04 — six days. The nearer of the two carry-forwards the stamp
   names is the 2026-09-02 entry — four days. Nothing here is two days.

The rest of the stamp is TRUE and worth keeping: the correct reading *"the clause is about what
the LANE AGENT can reach, not about what every actor can"* is in that entry; the misreading
*"read that absence as meaning the editor is unreachable"* is in that entry; the two stand four
LINES apart, with one sentence between them — both verified by reading them. The two later
entries are real. **Only the header pointer's chronology and the "two days" duration are wrong**
— and a false provenance sentence inside the stamp that exists to repair a false reading is the
exact shape this record has already paid for once: commit `4b206ab`'s own subject line opens
*"The guard built to catch a stale provenance sentence…"*.

Owner of the fix: the `coordinator`, same authority.

---

### Finding 3 — BLOCK. A fourth live copy of the retired claim, in `global.md`, unswept and unstamped.

`Tools/architect/state/global.md`, inside the 2026-08-29 stamp on the W4/W5 asset item — locate
it by the words *"the route to the editor was Bash over HTTP"*:

> *"The third route this bullet named is the one that was taken, with a wrinkle it did not
> foresee: `CLAUDE.md`'s editor-driver clause covered the acting, but the route to the editor was
> Bash over HTTP rather than either MCP client, **and whether that satisfies this record's
> fallback condition is the user's to rule.**"*

That question is the one the 2026-09-04 ruling answers. `CLAUDE.md` now names this very route and
this very date as covered — *"raw HTTP to `127.0.0.1:9315` on 2026-08-29 and again on
2026-09-02"* — and rules *"Any route the `coordinator` can reach is covered."* The sentence is
live, present tense, and hands a reader an answered question as an open one.

Two honest qualifications, neither of which saves it. First, it names *"this record's fallback
condition"*, which is `content.md`'s FILE-WRITE clause, whose trigger is the absent tool and not
the route — so the sentence was mis-filed when written. Second, it sits inside a `[STAMPED …]`
block; but that block stamps the item DISCHARGED, and this clause is an OPEN question the stamp
itself raises, not narration of a past state. Either way, the file that carries the project's
status tells a reader the route question is still the user's.

**This is the measured coverage gap in the seat's sweep:** none of the six terms the seat reports
using — `ABSENT FROM THE CLIENT SURFACE`, `neither clause models`, `gap neither clause`,
`who may drive`, `drive the live editor`, `editor IS reachable` — matches this sentence, or
finding 1's. Both were found by sweeping the CLAIM shape (*"is the user's to rule"*,
*"still open"*) rather than the prior gate's quotations.

Owner of the fix: the `coordinator`, which owns `global.md` jointly with the steward.

---

### Finding 4 — BLOCK. Three of the four 2026-09-04 writes to `content.md` name their writer and control; the header amendment does not, under a provenance sentence dating the paragraph to 2026-08-26.

`Tools/architect/state/content.md`, the header paragraph headed *"WHERE THE OTHER PERMISSION
LIVES, since 2026-08-26"*. This diff inserts into it:

> *"— **who may do the ASSET WORK, BY ANY ROUTE, when `strat-editor-builder` cannot reach the
> editor** (this pointer read "who may drive the live editor" until the 2026-09-04 route ruling,
> and a citation landing here must not be sent to a narrower rule than the one that governs) —"*

and leaves the paragraph's closing sentence untouched:

> *"This paragraph was added by the `coordinator` under the fallback condition, measured with its
> control."*

That closing sentence is this file's own discharge of the header's obligation (2) — *"say so in
the entry, naming the coordinator as its writer"* — and as it stands it now covers, without
distinguishing them, one write of 2026-08-26 and one of 2026-09-04 made under a separately
re-measured control. A reader attributes the whole paragraph, new words included, to the earlier
write. The three body stamps in this same diff each attribute themselves inline and by date; the
header amendment is the one site that does not, and it is the site a citation lands on.

The repair is a clause, not a rewrite: name the 2026-09-04 amendment, its writer and its
condition where the amendment sits, exactly as the body stamps do.

Owner of the fix: the `coordinator`, under `content.md`'s fallback condition.

## Observations

- **The scope question stays settled the way the prior gate settled it.** Restating the trigger as
  the lane agent's tool surface is not a change of condition: the operative sentence has read
  *"absent from the session's tool surface"* since `4b206ab` (2026-08-26) and this diff does not
  touch it. Re-verified from git here rather than carried over from the prior report.
- **`CLAUDE.md` is coherent after the two edits, with one seam worth naming.** The in-session
  clause's contrast — *"That clause is triggered by AN ABSENT TOOL: `strat-editor-builder` holds
  no Bash and no `execute_script`, so it cannot act at all"* — still reads true, because the
  widening freed the ROUTE and left the TRIGGER alone. The transcription clause (triggered by A
  MERGE) and the `content.md` fallback-writer paragraph are untouched and remain distinct, and the
  parenthetical at *"That second permission was worded 'driving the editor'"* is accurate against
  HEAD. **The seam:** the operative sentence conditions on `execute_script` being *"absent from
  the session's tool surface"* — the SESSION's surface — while the restatement four lines below
  says the trigger is that *"`strat-editor-builder` cannot reach the editor"* — the LANE AGENT's
  reach. In practice both draw from one surface and the clause ties them (*"which obligation (1)
  measures at the TOOL SURFACE"*), so this is not a finding today. It is the residue of a real
  distinction, and it is why the blocked-by-client gap is arguable in both directions: read at the
  operative sentence, a tool PRESENT but BLOCKED is not "absent" and the gap stays open, which is
  what the stamps claim; read at the restatement, "cannot reach the editor" would swallow it.
  **The stamps' claim that the gap is open is correct on the operative text**, and this note
  records where a future reader could reach the other answer.
- **The stamps' judgements about what the ruling did NOT reach are sound.** Blocked-by-client and
  absent-from-client are different states; `content.md` raises the former separately in the
  2026-08-31 entry, and this diff does not touch its subject. That entry is finding 1's site for a
  different reason — its trailing clause, not its subject.
- **Obligation (1) is discharged once for three writes.** The stamp on the topmost entry SHOWS the
  control — *"both `execute_script` names absent from one lookup that returned `unreal_status` and
  `list_unreal_projects` from the same server"* — while the two added stamps say only *"MEASURED
  WITH ITS CONTROL"*. All three are one pass on one file and the shown measurement sits a few
  hundred lines above, so this is noted rather than charged. As always, that measurement lives in
  the session and not in the tree: a checkout can confirm the claim was MADE, not that it was
  TRUE. Standing unfalsifiability, already declared by this project about itself.
- **The seat's judgement on the 2026-08-26 discharge block is CORRECT, and this gate confirms it.**
  The *"who may drive the editor when the lane's own agent cannot reach it"* occurrence there sits
  inside `It read: "…"`, in a block already stamped *"THE RULING CARRIED HERE WAS MADE ON
  2026-08-26 AND THIS ITEM IS DISCHARGED"*. Historical, quoted, correctly left.
- **Non-gating drift, `global.md`, the 2026-08-26 banner** — locate by *"A LANE RULE FINALLY HAS
  THE WORD IT WAS MISSING"*: *"Writing that file and driving the editor were always two
  permissions"* words the second permission the way `CLAUDE.md` has now retired it. It is
  narration of that day and reads correctly as history; recorded here only so a later shape sweep
  does not rediscover it as new.
- **Stamp wording, non-gating.** The topmost entry's stamp says *"THE SENTENCE ABOVE IS NO LONGER
  LIVE"*, singular, while the bullet it retires carries two live claims — its bolded heading
  *"WHETHER IT SATISFIES THE CLAUSE IS THE USER'S TO RULE"* and *"the route is not the one the
  clause names"*. The stamp's body answers both explicitly, so it is adequate; the stamp on the
  2026-08-29 entry shows the better form — *"EVERY CLAUSE OF THE PARAGRAPH ABOVE IS RETIRED"*.
- **The dispatching brief swapped two of its own items** — it attaches the *"still the gap neither
  clause models"* quote (which is the 2026-09-02 entry) to "the 2026-08-29 entry", and the
  four-lines-apart stamp (which sits on the 2026-08-29 entry) to "the 2026-09-02 entry". Nothing
  in this gate rests on the brief; both sites were located in the tree by their own words.
- **Lane note, non-gating.** `CLAUDE.md` is owned by no crew lane. `content.md` was written under
  its own fallback clause, cited at three of the four sites — finding 4 is the fourth.

VERDICT: BLOCK
