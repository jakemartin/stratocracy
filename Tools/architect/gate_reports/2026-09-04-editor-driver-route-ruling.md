# Gate: the 2026-09-04 editor-driver route ruling — strat-integration-reviewer

## Scope

Working-tree diff in the MAIN TREE, `E:/MultiAgent/Stratocracy`, branch `master`, over HEAD
`369513facaee461b73f4e8e825b668f8e5a5a9d6`. Nothing staged, nothing committed by this gate.

`git status --porcelain` printed exactly:

```
 M CLAUDE.md
 M Tools/architect/state/content.md
```

Two files, 50 insertions and 4 deletions. Working-tree content hashes at the time of this gate:

```
27f62f11206245c08ee0a8d1039f7f17cd8a919e6c1ca0772ac7817f25487f07  CLAUDE.md
4a81e82d55dc42f8a37c2de7137f1327e7b3a81274ea6601b5836cab5c11854a  Tools/architect/state/content.md
```

HEAD blobs they replace: `25e25ff2714…` and `87c9a25867…`.

`git diff --stat -- Source Content Config Stratocracy.uproject` printed NOTHING. No code, no
asset, no config, no `.uproject` change is present, as intended.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched | pass — `git diff --stat -- Source/StratRules Data` printed nothing |
| 2 | No `strat::` call outside `StratBridge`/`StratRules` | pass — the lowercase-initial call grep over `Source/`, minus the two permitted directories, returned zero lines; and no `Source/` file is modified at all |
| 3 | No vendored header in front of UHT | pass — no header changed |
| 4 | `TPimplPtr` not `TUniquePtr` for `FStratBridge` members | pass — no header changed |
| 5 | Module arrows unchanged and correct | pass — derived, see below |
| 6 | New modules registered, unless no module object | pass — no new module; `IMPLEMENT_MODULE` lives in exactly `StratBridge`, `Stratocracy`, `StratPlay`, `StratUI`, all four in the `.uproject` Modules array; `Source/StratRules/` carries none and is correctly absent |
| 7 | `Source/Stratocracy/` untouched | pass — `git diff --stat -- Source/Stratocracy` printed nothing |
| 8 | No `/Game/` literal in gameplay C++ | pass — every hit is under `Source/StratBridge/Tests/`, the standing fixture exception |
| 9 | No widget-side arithmetic | pass — no `StratUI` file changed |
| 10 | Lane discipline | pass — see Observations |
| 11 | Prose blocks present | pass — both edits are why-first prose in house style, dated, and name the ruling |
| 12 | Nothing staged or committed | pass — porcelain shows two ` M` entries, no index column |
| — | Coherence of the rule change against the rest of `CLAUDE.md` | **BLOCK** — finding 4 |
| — | Retired claims still live in `Tools/architect/state/` | **BLOCK** — findings 1, 2, 3 |
| — | Fallback obligations on the `content.md` write | pass — see Observations |
| — | `strat_banner_sweep.py` | pass — re-run here, SWEEP CLEAN, exit 0 |

### Module arrows, derived not trusted

The `.Build.cs` extraction printed **16 rows** across five modules, including the conditional
row, exactly as the standing block predicts:

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

All five modules appear, so the extraction did not silently fail. The sixteenth row is the
guarded one and it is still guarded — `StratBridge.Build.cs` reads
`bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;` and the
`PrivateDependencyModuleNames.Add("StratRules")` sits inside `if (!bCompileVendoredRulesHere)`.
`StratRules` does NOT appear in `StratBridge`'s `PublicDependencyModuleNames`, and does not
appear unconditionally in the private array. No drift. No cycle. `StratUI` has gained nothing.

The field census printed **TEN** names, matching the recorded figure — the three graph fields
plus `PCHUsage`, `bUseUnity`, `ShadowVariableWarningLevel`, `PrivateDependencyModuleNames.Add`,
`PrivateDefinitions.Add`, `bCompileVendoredRulesHere =` and the known `BRIDGE=` false positive,
which is the tail of the string literal `"STRAT_VENDORED_RULES_IN_BRIDGE="`. No eleventh name.

### strat_banner_sweep.py, re-run here

Run without a pipe, then again with `--explain`. Both exit 0.

```
Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
  automation report: 389 entries, all Success (2026.09.04-16.30.20)
  macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=389, COMPLEX/_CLASS=0
  report identity: mtime=1788539420, reportCreatedOn='2026.09.04-16.30.20'
  suite claims found: 62 (1 live, 61 stamped)
  tracked items declared: 2

SWEEP CLEAN -- no self-contradiction found
```

`--explain` confirms the single LIVE claim is `global.md:32`, at `389/389`, and that the other 61
are stamped. **This vouches for none of the subject under gate.** The sweep collects claims by
the suite-count shape and by the tracked-item shape; it collects no claim of the shape "this
route is unruled" or "the clause is about driving the live editor". Its green says nothing about
findings 1, 2 and 3, all of which it walked straight past in a file it reads.

## Findings

### THE SCOPE QUESTION, ruled first because the seat asked for it plainly

**Item 2 — restating the condition as the lane agent's TOOL SURFACE rather than "the editor is
unreachable" — is NOT an over-reach, and it should NOT be cut back to the literal ask.** It is
not even a change of condition. The clause's operative condition sentence at HEAD reads,
verbatim:

> *"When `execute_script` is absent from the session's tool surface,"*

That is the tool surface, in those words, since the clause was written on 2026-08-26, and this
diff leaves that sentence UNCHANGED. "The editor is unreachable" was never the written condition;
it was a gloss, and the record itself already identified it as a gloss — `content.md` says *"Both
condition on the tool's absence and both read that absence as meaning the editor is
unreachable."* The new paragraph deletes the misreading, not the rule. Separately, the seat's own
justification also holds: a clause whose operative verb is "drive the live editor" cannot
coherently license a commandlet that opens no editor, so widening the route while leaving the
verb would have produced a clause at war with itself. Both arguments point the same way, and the
first is checkable from git rather than from reasoning. **No finding here.**

Relatedly, and also no finding: **the new text does not claim more than the user said.** The
ruling was made with all three passes in view — raw HTTP on 2026-08-29 and 2026-09-02, and the
commandlet on 2026-09-04 — so covering the HTTP route is squarely inside the ask rather than
smuggled in beside it. The stamp itself does not assert that the ruling closes the
"absent from the client while the server still serves it" gap; it asserts the narrower and true
thing, and it explicitly names the BLOCKED-BY-CLIENT gap as untouched and still open. That
second judgement is correct: blocked-by-client and absent-from-client are different states, the
ruling addressed the ROUTE rather than the TRIGGER, and leaving that gap open is right.

The three findings below are all one defect wearing three faces: **the rule change is right, and
the record has not been brought with it.** The sweep the seat ran found one site and missed three.

---

### Finding 1 — BLOCK. A retired claim is still live, in the same file that was stamped.

`Tools/architect/state/content.md`, in the 2026-08-29 entry, at the sentence beginning
*"THE REAL GAP IS NARROWER"*:

> *"**THE REAL GAP IS NARROWER AND IS WORTH STATING PRECISELY: neither this file's fallback nor
> `CLAUDE.md`'s editor-driver clause models a tool being ABSENT FROM THE CLIENT SURFACE WHILE
> THE SERVER STILL SERVES IT.** Both condition on the tool's absence and both read that absence
> as meaning the editor is unreachable. … Whether an actor who reaches a live editor by some
> other transport falls inside these clauses or needs a sentence of its own is the user's to
> rule; until it is, this entry is the only place the question is visible."*

Every clause of that is now false about `CLAUDE.md`'s editor-driver clause, and it is falsified
by the very paragraphs added in this diff. The clause now says *"Any route the `coordinator` can
reach is covered: the NeoStack client, raw HTTP to a live editor, a headless commandlet, or one
not yet invented"* — which answers *"whether an actor who reaches a live editor by some other
transport falls inside these clauses"* with an explicit YES. And it says *"It is NOT 'the editor
is unreachable' and never was"* — which contradicts, word for word, *"both read that absence as
meaning the editor is unreachable."* The trailing *"is the user's to rule; until it is"* is the
exact sentence shape the seat stamped forty lines from the top of this same file, left standing
here.

Owner of the fix: the `coordinator`, under `content.md`'s fallback condition — the same authority
under which the topmost stamp was written.

---

### Finding 2 — BLOCK. The same retired claim, a second time, in the same file.

`Tools/architect/state/content.md`, in the 2026-09-02 entry, at the sentence ending
*"still the gap neither clause models"*:

> *"…and still the gap neither clause models: A TOOL ABSENT FROM THE CLIENT SURFACE WHILE THE
> SERVER STILL SERVES IT."*

This is the earlier claim restated for a second pass. `CLAUDE.md` now models exactly that case by
name and by date: *"on 2026-08-29 and 2026-09-02 the editor was **alive and serving
`execute_script` the whole time** while the connector sat latched stale."* The record asserts
"neither clause models it" about a pass the new clause text now cites as one of its own worked
examples.

Owner of the fix: the `coordinator`, same authority.

---

### Finding 3 — BLOCK. `content.md`'s header describes the clause by its retired scope.

The seat asked whether leaving `content.md`'s header is right. **On the sentence it named, yes.**
The header's *"has no route to the editor and cannot produce the evidence this file holds"* is
the REASON for the fallback, not its condition — the condition sentence beside it is *"only when
`execute_script` is absent from the session's tool surface"* — and the reason is still true of
`strat-editor-builder`, which holds no Bash and so cannot reach a commandlet either. Correctly
left, and correctly reasoned. **But the header carries a second sentence the seat did not
weigh**, in the paragraph headed *"WHERE THE OTHER PERMISSION LIVES, since 2026-08-26"*:

> *"The permission it kept being stretched to cover — **who may drive the live editor when
> `strat-editor-builder` cannot reach it** — is now stated in `CLAUDE.md`'s **The crew** section
> as the *editor-driver clause*… Cite THAT for the acting and THIS for the file write."*

That is the header's authority pointer, and it names the target clause's scope in the wording the
ruling retired. A reader whose acting was a commandlet reads "who may drive the live editor",
concludes the pointer does not reach them, and lands back in the failure the widening exists to
end. The seat itself treats retired wording as needing a marker — that is precisely what item 3
of this diff does inside `CLAUDE.md`, adding *"(That second permission was worded "driving the
editor" until the 2026-09-04 route ruling below…)"*. The same courtesy is owed in the file that
directs the citation.

Owner of the fix: the `coordinator`, same authority.

---

### Finding 4 — BLOCK. A sentence in `CLAUDE.md` is now false, four lines below the paragraph that falsifies it.

`CLAUDE.md`, the paragraph immediately after the clause block, unchanged by this diff:

> *"Nothing here widens the lane rule for a session where the editor IS reachable — when
> `strat-editor-builder` can work at all, `Content/` is its lane and only its lane."*

The clause four lines above now says, in capitals: *"**So do not measure the editor to decide
whether this clause applies; measure the lane agent's tool surface.**"* That sentence's leading
half does precisely the forbidden thing — it conditions the lane rule on *"a session where the
editor IS reachable"*. By the new paragraph's own worked examples, on 2026-08-29 and 2026-09-02
the editor WAS reachable and the clause WAS open, so the sentence as written is false on two of
the three passes the ruling is about.

The trailing half after the em dash is correct and states the right test. The two halves are
offered as equivalent and are not. This is a one-word repair, and it matters because this file
loads every session and because the record already measures what an imprecise sentence about
this exact clause costs: *"Three consecutive `strat-integration-reviewer` gates turned on that
wording while the asset it governed was clean from the first read."* Leaving a second imprecise
sentence about the same clause, in the same section, is that measurement ignored.

Owner of the fix: the `coordinator`, which owns this ruling's transcription; `CLAUDE.md` is owned
by no crew lane.

## Observations

- **Coherence with the other three clauses holds.** The in-session clause's contrast —
  *"That clause is triggered by AN ABSENT TOOL: `strat-editor-builder` holds no Bash and no
  `execute_script`, so it **cannot act at all**"* — still reads true, because the widening freed
  the ROUTE and left the TRIGGER alone. The transcription clause is untouched and remains
  distinct (triggered by A MERGE, licensing only carrying across). The `content.md`
  fallback-writer paragraph in **The crew** still conditions on the absent tool and is
  unaffected. Finding 4 is the only place in `CLAUDE.md` where the widened clause and its
  surrounding prose come apart.
- **Both of the clause's own obligations survived the rewrite intact.** (1) *"Measure the absence
  with a control before invoking it"* is present verbatim. (2) is present with "driving" changed
  to "acting" — *"This clause covers the acting; the `content.md` header covers the file write"* —
  which the widening required. Neither was quietly dropped. The new third paragraph ADDS an
  obligation rather than relaxing one: the route and its precondition must still be named.
- **The obligations on the `content.md` write itself are discharged as far as a checkout can
  see.** The stamp names the coordinator as writer and reports the control —
  *"both `execute_script` names absent from one lookup that returned `unreal_status` and
  `list_unreal_projects` from the same server"*. As with the transcription clause's limit (1) and
  the in-session clause's obligation (1), that measurement lives in the session and not in the
  tree, so a checkout can confirm the claim was MADE and not that it was TRUE. That is the
  standing unfalsifiability this project already declares about itself; it is noted, not charged.
- **The commandlet-limits paragraph is a genuine strengthening and is worth keeping as written.**
  *"A commandlet, in particular, **cannot prove a material compiles and has no pixels**"*, with
  its measured controls, closing on *"A human at the keyboard remains the only instrument for any
  visual claim"*. That is the widening paying for itself rather than merely spending the ruling.
- Three further "drive the editor" hits are historical narration of specific past passes — in
  `content.md` at the 2026-08-27 attribution bullet and at the carried-and-stamped
  *"CARRIED FOR A RULING"* paragraph, and in `global.md` at the 2026-08-26 banner. Each
  correctly describes what those passes did or what the clause said at the time. They are not
  descriptions of the current clause and need no stamp.
- `global.md`'s *"ITEM (3), THE REPAIR-ELIGIBILITY PIP, IS STILL NOT BUILDABLE AND IS THE USER'S
  TO RULE"* is about W8 and is unrelated to this ruling. The seat's third judgement is correct.
- Lane note, non-gating: `CLAUDE.md` is owned by no crew lane, and `content.md` was written under
  its own fallback clause with that clause cited in the entry, so nothing here is out of lane.

VERDICT: BLOCK
