# Gate: the 2026-09-04 editor-driver route ruling, RE-GATE 2 — strat-integration-reviewer

## Scope

Working-tree diff in the MAIN TREE, absolute path `E:/MultiAgent/Stratocracy`, branch `master`,
over HEAD `369513facaee461b73f4e8e825b668f8e5a5a9d6`. Third gate on this change; the whole change
was re-read from the tree, not scoped to the two prior BLOCKs. Nothing staged, nothing committed
by this gate. `git worktree list` shows the main tree plus `Strat-wt/slot-1`, `Strat-wt/slot-2`
and five `.claude/worktrees/` checkouts; every path below is in the main tree and nothing under
those other roots was read as evidence.

`git status --porcelain` printed exactly:

```
 M CLAUDE.md
 M Tools/architect/state/content.md
 M Tools/architect/state/global.md
?? Tools/architect/gate_reports/2026-09-04-editor-driver-route-ruling-regate.md
?? Tools/architect/gate_reports/2026-09-04-editor-driver-route-ruling.md
```

Three modified files, **124 insertions and 9 deletions**, plus the two untracked prior reports.
Working-tree sha256 at the time of this gate:

```
852c4ab3c5b4c43d1c2b3feec9eed581a937d97abd6642579317f8187fc77c1c  CLAUDE.md
49da35f0b2961a1aae4e8a8978b32eb3714cb3bbc917e2518de9983facd42383  Tools/architect/state/content.md
b12de12bcba6267839cdb66e33506206c3bac9bf141c31b891c673693447b797  Tools/architect/state/global.md
8cb6f0363aea300bd5fd5793170d12f93ffd23c2610d9d2d79ba6d13d33b5634  ...editor-driver-route-ruling.md
486b5ddd639cf86ae5f181845a6d472e278a637d31094635ab7817bdf6e00732  ...editor-driver-route-ruling-regate.md
```

**The prior reports were checked by CONTENT, not by status, and one of them is provably
untouched.** The regate report recorded the first report's sha256 as `8cb6f036…`; it still hashes
to `8cb6f036…`, so that file is byte-identical to what the second gate saw. The regate report
itself carries no earlier recorded hash, so its `486b5ddd…` is recorded here for the next reader
rather than claimed as proof; its mtime `2026-09-04T17:50:06` precedes both record writes
(`content.md` 17:51:44, `global.md` 17:52:01), which is consistent with it being untouched.
`git diff --stat -- Tools/architect/gate_reports` printed nothing, so every TRACKED report in that
directory is identical to HEAD.

**`CLAUDE.md` DID NOT MOVE THIS ROUND.** Its sha256 `852c4ab3…` is the same value the regate report
recorded, and its mtime is `2026-09-04T17:29:55` — earlier than that report. All four claimed
repairs landed in `content.md` and `global.md`. Two of this gate's findings are therefore against
`CLAUDE.md` text that neither prior round examined on this axis.

`git diff --stat -- Source Content Config Data Stratocracy.uproject .agents .claude .github
Tools/architect/kb_snapshot` printed NOTHING. No code, asset, config, data, `.uproject`, agent
definition or snapshot change is present, as intended.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside `StratBridge`/`StratRules` | pass — the lowercase-initial call grep over `Source/`, minus the two permitted directories, returned zero lines; no `Source/` file is modified |
| 3 | No vendored header in front of UHT | pass — `grep -rn '#include "StratBridge.h"' Source --include=*.h` returned nothing; no header changed |
| 4 | `TPimplPtr` not `TUniquePtr` for `FStratBridge` members | pass — no header changed |
| 5 | Module arrows unchanged and correct | pass — derived below, 16 rows / 10 field names, sixteenth row read as guarded |
| 6 | New modules registered, unless no module object | pass — `IMPLEMENT_MODULE` in `StratBridge.cpp`, `StratPlay.cpp`, `StratUI.cpp`, plus `IMPLEMENT_PRIMARY_GAME_MODULE` in `Stratocracy.cpp`; all four named in the `.uproject` Modules array; `Source/StratRules/` carries neither macro and is correctly absent |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` literal in gameplay C++ | pass — outside `Tests/` the only non-comment hit is `ImportStratDataCommandlet.cpp`'s `FString::Printf(TEXT("/Game/StratData/%s"), Spec.AssetName)`, a standing exception; every other hit is prose forbidding the literal |
| 9 | No widget-side arithmetic | pass — no `StratUI` file changed |
| 10 | Lane discipline | pass — see Observations |
| 11 | Prose blocks present | pass — every added block is why-first, dated, and names its authority |
| 12 | Nothing staged or committed | pass — porcelain shows three ` M` entries and two `??`; no index column, HEAD still `369513f` |
| — | Repair 1: the 2026-08-31 pointer stamped | pass |
| — | Repair 2: the two factual errors in the 2026-09-02 stamp | **BLOCK** — finding 1; one error was corrected and replaced with a new false claim |
| — | Repair 3: the fourth copy in `global.md` stamped | pass |
| — | Repair 4: the `content.md` header amendment attributed | **BLOCK** — finding 1 site A; the attribution itself is correct |
| — | Every new stamp true | **BLOCK** — findings 1 and 3 |
| — | Any stamp overclaims | **BLOCK** — finding 3; the "fourth and only one outside `content.md`" claim itself is CORRECT |
| — | Coherence of `CLAUDE.md` as a whole | pass with a seam — see Observations |
| — | Independent claim-shape sweep | **BLOCK** — finding 2 |
| — | Fallback obligations on the `content.md` writes | pass |
| — | `strat_banner_sweep.py` | pass — re-run here, `SWEEP CLEAN`, exit 0, plain and `--explain` |

### Module arrows, derived not trusted

The `.Build.cs` extraction printed **16 rows**, all five modules present, so the extraction did not
silently fail:

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
Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...
```

The sixteenth row is GUARDED, read from the file and not inferred from the extraction.
`Source/StratBridge/StratBridge.Build.cs` carries `bool bCompileVendoredRulesHere = Target.LinkType
!= TargetLinkType.Monolithic;`, then `PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" ...)`,
then `if (!bCompileVendoredRulesHere)` and inside it `PrivateDependencyModuleNames.Add("StratRules");`.
`StratRules` appears in no `PublicDependencyModuleNames` and in no UNCONDITIONAL private array. No
drift, no cycle; `StratUI` has gained nothing.

The field census printed **TEN** names — `PublicDependencyModuleNames.AddRange`,
`PrivateDependencyModuleNames.AddRange`, `PrivateDependencyModuleNames.Add`,
`PublicIncludePaths.AddRange`, `PrivateDefinitions.Add`, `PCHUsage =`, `bUseUnity =`,
`ShadowVariableWarningLevel =`, `bCompileVendoredRulesHere =`, and the known string-literal false
positive, printed on this run as `BRIDGE=`. **No eleventh name.**

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
EXIT=0
```

`--explain` enumerates the 62 claims, 61 stamped and one live. **This vouches for nothing under
gate.** The sweep collects claims by the SUITE-COUNT shape and the TRACKED-ITEM shape; it collects
no claim of the shape *"this pointer is where the wording originated"* or *"three passes in a
row"*, and it walked past all three findings below in files it reads.

### The claim-shape sweep this gate ran, and its instrument's control

Swept over `CLAUDE.md`, all six `Tools/architect/state/*.md`, the frozen `Tools/architect/state.md`,
`.agents/`, `.claude/agents/` and `.claude/skills/`. **The instrument was controlled first:**
`grep -rn "editor-driver clause"` over that set returned **30 lines**, so an empty result below is a
real absence and not this box's recorded `-i -F` abort. Terms: `user's to rule`, `s to rule`,
`is still open`, `still open`, `remains open`, `neither clause`, `no clause`, `does not model`,
`not modelled`, `drive the live editor`, `driving the live editor`, `driving the editor`,
`editor IS reachable`, `editor is unreachable`, `unruled`, `sentence of its own`,
`the clause names`, `falls inside`, `CARRIED FOR A RULING`, `left for the user`, `the user's call`,
`yet to be ruled`, `awaiting a ruling`, `not covered by`, `gap`.

That sweep confirms one of the seat's claims and produced one new finding. **The seat's
"fourth live copy, and the only one outside `content.md`" is CORRECT**: outside `content.md` the
only other occurrence on this axis is `global.md`'s `_Last run 2026-08-26_` banner narration
(*"Writing that file and driving the editor were always two permissions"*), which is history of that
day's rule text rather than a live open question, and the prior gate reached the same reading.
`engine.md`, `tests.md`, `data.md`, `decisions.md`, `state.md`, `.agents/` and `.claude/` carry
nothing on this axis; their `user's to rule` and `unruled` hits are the transcription clause and
the W-series wave IDs, unrelated. The new finding the sweep produced is finding 2, from the term
`driving the editor` — a term the seat's reported pattern list does not contain.

## Findings

### Finding 1 — BLOCK. The repair of the false provenance claim replaced it with a different false provenance claim, at two sites. Git dates the narrower wording to an ENTRY, one day BEFORE the header pointer that now claims to be its origin.

This is the round-2 finding wearing a new face, and it is the most serious finding here because it
is a defect introduced BY a repair, in the sentences whose whole subject is direction of travel.

**Site A — `Tools/architect/state/content.md`, the header amendment**, locate by the words
*"this pointer is where that narrower wording ORIGINATED"*:

> *"it read "who may drive the live editor" from `4b206ab` on 2026-08-26 until the route ruling
> of that date, and a citation landing here must not be sent to a narrower rule than the one that
> governs — **this pointer is where that narrower wording ORIGINATED**, and the entries below it
> inherited it rather than the other way round"*

**Site B — the same file, inside the stamp on the 2026-08-29 entry**, locate by the words
*"THE DIRECTION MATTERS AND A FIRST DRAFT OF THIS STAMP HAD IT BACKWARDS"*:

> *"`git log -S` dates that pointer to `4b206ab`, 2026-08-26 — **THREE DAYS BEFORE THIS ENTRY**.
> **The header pointer is the misreading's SOURCE and not its downstream copy.**"*

**What git actually says, run in the main tree:**

```
$ git log --format='%h %ad' --date=short -S "who may drive the editor" -- Tools/architect/state/content.md
905b1c1 2026-08-25
```

`905b1c1` is **2026-08-25, ONE DAY BEFORE `4b206ab`**, and what it added was not the header — it
was a bullet in the entry BELOW the header. That bullet is still in the tree today, quoted inside a
stamp; locate it by the words *"CARRIED FOR A RULING"*:

> *"the thing practice now needs a word for is not the file write, it is **who may drive the editor
> when the lane's own agent cannot reach it**."*

And `git show 4b206ab -- Tools/architect/state/content.md` shows that very line being REMOVED and
re-quoted in the same commit that ADDED the header pointer — a `-` line at diff position 101 and the
`+` re-quote at 110, alongside the `+` header pointer at 79-84. The pointer did not originate the
narrower wording; it inherited it from an entry below it, which is exactly *"the other way round"*
from what site A asserts. `4b206ab` put the identical gloss into `CLAUDE.md`'s clause in the same
commit (`git log -S 'drive the live editor' -- CLAUDE.md` returns `4b206ab` alone), so even setting
the 2026-08-25 entry aside, the pointer is at best one of two co-equal siblings and cannot be
singled out as the source.

The half of site B that git DOES support is the half about this entry: 2026-08-26 precedes
2026-08-29, so the pointer is not a copy of the 2026-08-29 entry. **The word "SOURCE" is what
fails**, and it fails on the same instrument the stamp itself invokes. Round 2 blocked because
*"the stamp asserts a direction of travel git contradicts"*; the repair asserts a new one.

The repair is one clause at each site, not a rewrite: name `905b1c1` / 2026-08-25 and the entry
below as where the wording entered this file, and keep the true part — that the pointer predates the
2026-08-29 entry and so is not its downstream copy.

Owner of the fix: the `coordinator`, under `content.md`'s fallback condition, the same authority
under which both sites were written.

---

### Finding 2 — BLOCK. A live copy of the retired gloss sits three lines above the amendment that retires it, in the same paragraph, in the mirror of a `CLAUDE.md` sentence this change DID amend.

`Tools/architect/state/content.md`, the header, locate by the words *"The sentence above is correct
and is not being widened"*:

> *"**WHERE THE OTHER PERMISSION LIVES, since 2026-08-26.** The sentence above is correct and is not
> being widened: **this clause has never licensed driving the editor**, and three consecutive
> `strat-integration-reviewer` gates turned on that exact distinction..."*

`CLAUDE.md`'s mirror of that same sentence WAS amended by this diff, and to exactly the wording this
one lacks:

```
-**Writing `content.md` and driving the editor are two different permissions, and until
+**Writing `content.md` and doing the asset work are two different permissions, and until
```

So the diff corrected one copy of the sentence and left the other, and the one it left is the
NEGATIVE-SCOPE statement — the sentence a reader uses to decide what the file-write fallback does
NOT cover. Under the 2026-09-04 ruling the true statement is *"has never licensed the ASSET WORK, by
any route"*. As written it forbids only driving the editor, which leaves a reader with a live
inference that a non-editor-driving route — a headless commandlet, say, which is precisely the route
this ruling was made about — might fall inside the file-write fallback after all. That is the same
mis-citation hazard the amendment three lines below it names in terms: *"a citation landing here
must not be sent to a narrower rule than the one that governs"*.

**This is a coverage gap in the sweep, measured rather than asserted.** The pattern list the seat
reports using — `user's to rule`, `is still open`, `neither clause`, `does not model`,
`drive the live editor`, `editor IS reachable` — contains no term that matches this sentence. It was
found here by the term `driving the editor`, which produced exactly two hits in the state files:
this one, and `global.md`'s historical banner narration, which is correctly left as history.

Owner of the fix: the `coordinator`, under `content.md`'s fallback condition.

---

### Finding 3 — BLOCK. The ruling's own count of the passes it is drawn from is short by one, at three sites. A fourth clause-covered pass on an unnamed route is in the same file, with its own base commit and its own control.

`CLAUDE.md`, the added ruling block, locate by the words *"THE ROUTE IS NOT PART OF THE CONDITION"*:

> *"This clause once read "may drive the live editor", and **three passes in a row** used a route
> that wording did not name — raw HTTP to `127.0.0.1:9315` **on 2026-08-29 and again on 2026-09-02**,
> and on 2026-09-04 a headless `UnrealEditor-Cmd.exe -run=pythonscript` commandlet..."*

`Tools/architect/state/content.md`, in the stamp on the topmost entry, locate by the words *"the two
came apart"*:

> *"the two came apart **three times, twice with the editor alive** and serving `execute_script`
> behind a latched connector, and once, in this pass, with no editor at all."*

`Tools/architect/state/global.md`, in the new stamp, locate by the words *"one of the three passes
the ruling is drawn from"*.

**`content.md` holds FOUR clause-covered passes on a route the old wording did not name, not three.**
Each names the editor-driver clause for the ACTING and each records its own control:

| entry, located by its own words | date / base | route, in the entry's words |
|---|---|---|
| *"W4'S FOUR MARKER SLOTS AND W5'S PLAYBACK PACE ARE SET"* | 2026-08-29 | *"driven over its own Streamable-HTTP MCP endpoint at `127.0.0.1:9315/mcp` with `curl` from Bash"* |
| *"W8 ITEM (4)'S PULSE MATERIAL IS AUTHORED AND ASSIGNED"* | **2026-09-01/02, base `4a89ac2`** | *"**So the route taken was Bash over raw HTTP**, the same route this record notes was used on 2026-08-29"* |
| *"THE COMMAND BAR IS ON SCREEN AND THE USER DROVE ITS BUTTONS"* | 2026-09-02, base `91927e9` | *"The route used was raw HTTP from Bash, which `strat-editor-builder` does not have."* |
| *"...SO THIS WAS AN AUTHORING BATCH, NOT AN ASSIGNMENT BATCH"* (topmost) | 2026-09-04 | headless `-run=pythonscript` commandlet |

The 2026-09-01/02 entry is a distinct pass from the 2026-09-02 one — different base commit,
different asset (`MI_Overlay_BuildPulse` against `WBP_CommandBar`), its own separate absence
measurement — so *"on 2026-08-29 and again on 2026-09-02"* collapses two passes into one date and
*"three passes in a row"* undercounts by one.

The same undercount reaches `content.md`'s *"twice with the editor alive"*. That 2026-09-01/02 entry
records the two coming apart as sharply as any of them: *"`execute_script` was requested BY NAME from
the connector and did not come back... **THE EDITOR ITSELF SERVES THAT TOOL AND NOTHING ELSE:** a raw
MCP `tools/list` over `127.0.0.1:9315` returns exactly one tool, `execute_script`."* That is a THIRD
alive-editor instance, so the true figures are four passes and three coming-apart, not three and two.

**Two things this finding is NOT.** It is not a challenge to the ruling — four identical exceptions
argue for it at least as well as three. And it is not a charge against the sentence *"Each was
recorded as unruled and left for the user"*, which is true of the three the block names; the
2026-09-01/02 entry filed no wrinkle at all, and that is worth saying when the count is corrected,
because it is the one thing that distinguishes it from the other three.

**These sites belong to this gate rather than to a prior one:** `CLAUDE.md` is byte-identical to what
round 2 gated (sha256 `852c4ab3...`, mtime 17:29:55), and neither prior round checked this claim
against the entry list. The `content.md` and `global.md` sites are new in this round's diff.

Owner of the fix: the `coordinator` — `CLAUDE.md` is owned by no crew lane; `content.md` under its
fallback condition; `global.md` in lane.

## Observations

- **Repairs 1, 3 and 4 are sound and are not repeated as findings.** The 2026-08-31 pointer now
  carries a stamp that separates the CLOSED mirror-image gap from the STILL-OPEN
  blocked-by-the-client one, and both halves check out against the sites they name: the
  2026-08-29 entry's *"ABSENT FROM THE CLIENT SURFACE WHILE THE SERVER STILL SERVES IT"* is stamped
  closed at its own site, and the 2026-08-31 bullet's own *"tool PRESENT but BLOCKED BY THE CLIENT"*
  question is untouched by the ruling and correctly left live. The `global.md` stamp is flat rather
  than nested, in lane, and its *"FOURTH LIVE COPY ... AND THE ONLY ONE OUTSIDE `content.md`"* claim
  was independently swept here and holds. The header amendment now names its date, its writer and
  its fallback authority, which is exactly what round 2's finding 4 asked for.
- **Correcting uncommitted stamp text IN PLACE was the right call here, and the ground given for it
  is checkable.** The stamp says *"Corrected before commit rather than stamped, since no version of
  this stamp has ever shipped"*. `git log -S` finds no commit containing that text, and this record's
  stamp-never-delete convention exists so a reader is not shown a corrected claim as though it were
  the original — a text no checkout has ever contained cannot mislead a reader who never saw it.
  Stamping it instead would have grown a stamp-on-a-stamp whose subject was a draft. The judgement
  is correct; finding 1 is about WHAT the in-place correction says, not about the decision to make it.
- **`CLAUDE.md` is coherent after its two edits, with the same seam round 2 named.** The in-session
  clause's contrast (*"triggered by AN ABSENT TOOL ... so it cannot act at all"*) still reads true,
  because the widening freed the ROUTE and left the TRIGGER alone. The transcription clause
  (triggered by A MERGE) and the `content.md` fallback-writer paragraph are untouched and remain
  distinct. The seam persists and is still non-gating: the operative sentence conditions on
  `execute_script` being *"absent from the session's tool surface"* while the restatement four lines
  below says the trigger is that *"`strat-editor-builder` cannot reach the editor"*. Read at the
  operative sentence, a tool PRESENT but BLOCKED is not "absent" and the blocked-by-client gap stays
  open — which is what the stamps claim, and they are right on the operative text.
- **The new obligation `CLAUDE.md` adds is discharged by the entry it governs.** *"Name the route
  taken and how its precondition was measured"* — the topmost `content.md` entry names the commandlet,
  names `PythonScriptPlugin` reaching it transitively through `PCGPythonInterop`, and records that
  `tasklist` matched `unreal` zero times against 364 processes and `netstat` saw none on 9315. The
  block's claim that a commandlet *"cannot prove a material compiles and has no pixels"* is backed at
  that entry with both controls: `get_statistics` all-zeros for the known-good `M_Translucent` as well
  as the new master, and a deliberately broken material — a dangling `Sine` feeding Opacity — logging
  nothing beside the subject.
- **The fallback obligations are discharged on all four `content.md` writes.** Obligation (2) is now
  inline at every site including the header. Obligation (1)'s control is SHOWN once, in the topmost
  entry's stamp (*"both `execute_script` names absent from one lookup that returned `unreal_status`
  and `list_unreal_projects` from the same server"*), and the other three say only *"MEASURED WITH ITS
  CONTROL"*. One pass, one file, one measurement a few hundred lines above — noted, not charged. As
  always, that measurement lives in the session and not in the tree: a checkout can confirm the claim
  was MADE, not that it was TRUE. Standing unfalsifiability, already declared by this project about
  itself.
- **Lane note, non-gating.** `CLAUDE.md` is owned by no crew lane. `global.md`'s writers are the
  `coordinator` and the steward, and the stamp says so in terms. `content.md` was written under its
  own fallback clause, cited at all four sites. No wave was in flight over this change in the main
  tree, and no in-session-clause write to `Source/` was attempted.
- **Historical occurrences correctly left, recorded so a later sweep does not rediscover them.**
  `content.md`'s *"who may drive the editor when the lane's own agent cannot reach it"* sits inside
  `It read: "..."` in a block already stamped DISCHARGED; its *"the ACTING -- driving the live editor
  and its PIE session"* in the 2026-08-27 entry narrates what was actually done that day;
  `global.md`'s `_Last run 2026-08-26_` banner narrates the rule as worded then. All three are
  history, not live claims. Note that the first of these is finding 1's evidence — it is the origin
  the header amendment disclaims.

VERDICT: BLOCK
