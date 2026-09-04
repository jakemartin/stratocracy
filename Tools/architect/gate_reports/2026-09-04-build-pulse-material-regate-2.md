# Gate: W8 build-pulse material — RE-GATE 2, first over a committed tree — strat-integration-reviewer

## Scope

Main tree `E:/MultiAgent/Stratocracy`, branch `master`, HEAD `fa715d3`, ONE AHEAD of
`origin/master` (`4e4e2d1`) and UNPUSHED. `git status --porcelain` printed nothing — the working
tree is clean, so the tree gated and the tree committed are the same bytes, which is the one
thing the two prior gates on this pass could not say.

Audited: the whole pass, `4e4e2d1..fa715d3`, **six files**, re-read from scratch rather than
scoped to the two prior findings. **THE COMMIT MESSAGE IS PART OF THE SUBJECT** and was held to
the record's standard, because the previous gate's second finding was a false sentence in it.

    $ git diff --name-status HEAD~1 HEAD
    M       Content/StratArt/Materials/MI_Overlay_BuildPulse.uasset
    A       Content/StratArt/Materials/M_OverlayPulse.uasset
    A       Tools/architect/gate_reports/2026-09-04-build-pulse-material-regate.md
    A       Tools/architect/gate_reports/2026-09-04-build-pulse-material.md
    M       Tools/architect/state/content.md
    M       Tools/architect/state/global.md

Nothing outside `Content/` and `Tools/architect/`; the same command piped through
`grep -v '^Content/' | grep -v '^Tools/architect/'` returns no output.

The two prior reports, `2026-09-04-build-pulse-material.md` (`VERDICT: BLOCK`) and
`2026-09-04-build-pulse-material-regate.md` (`VERDICT: BLOCK`), were read and are left
byte-identical; the clean `git status` above, taken again after this file was the only write, is
the evidence. This gate is read-only apart from this one new file.

## Checklist

| # | Check | Result |
|---|---|---|
| 1 | Vendored bytes untouched (`Source/StratRules`, `Data`) | pass |
| 2 | No `strat::` call outside `StratBridge/` `StratRules/` | pass |
| 3 | No vendored header in front of UHT | pass |
| 4 | `TPimplPtr`, not `TUniquePtr`, for `FStratBridge` members | pass |
| 5 | Module arrows unchanged and correct (16 rows / 10 fields) | pass |
| 6 | New modules registered; `StratRules` still absent | pass |
| 7 | `Source/Stratocracy/` untouched | pass |
| 8 | No `/Game/` literal in gameplay C++ | pass |
| 9 | No widget-side arithmetic | pass |
| 10 | Lane discipline | pass (route question flagged, non-gating — Observation 1) |
| 11 | Prose blocks present; measurements recorded as measurements | pass |
| 12 | Nothing staged; working tree clean | pass |
| — | **THE ARTIFACT** (project ruling: assets and code outrank the record) | pass |
| — | **THE COMMIT MESSAGE** | pass (one amendable sentence — Observation 2) |

### THE ARTIFACT, FIRST — verified by content hash, not by a SHA

The committed LFS pointer and the working file's own sha256 agree on both paths, which is the
check the commit message says it made. Re-made here independently:

    $ git show HEAD:Content/StratArt/Materials/M_OverlayPulse.uasset
    version https://git-lfs.github.com/spec/v1
    oid sha256:3a8aa7cbc34b3169018bfcd622028b956470f1786d8073a29772a58292fbbb24
    size 11384

    $ sha256sum Content/StratArt/Materials/M_OverlayPulse.uasset
    3a8aa7cbc34b3169018bfcd622028b956470f1786d8073a29772a58292fbbb24
    $ stat -c '%s' Content/StratArt/Materials/M_OverlayPulse.uasset
    11384

`oid 3a8aa7c...` size `11384` — **the delivered, rewired master, not the `64ec50f...` size 6477
pre-rewire duplicate the first gate found in the index.** The instance:

    $ git show HEAD:Content/StratArt/Materials/MI_Overlay_BuildPulse.uasset
    oid sha256:d6a6e2aa3ff693d106625abc6b7bc04ea9a341c8ecb387b50742b8330855c833
    size 7893
    $ sha256sum Content/StratArt/Materials/MI_Overlay_BuildPulse.uasset
    d6a6e2aa3ff693d106625abc6b7bc04ea9a341c8ecb387b50742b8330855c833

**The bytes were read, not inferred from the size.** A printable-string scan of the committed
master:

    MaterialExpressionTime                   PRESENT
    MaterialExpressionSine                   PRESENT
    MaterialExpressionMultiply               PRESENT
    MaterialExpressionAdd                    PRESENT
    MaterialExpressionLinearInterpolate      PRESENT
    MaterialExpressionScalarParameter        PRESENT
    PulseSpeed / PulseMin / PulseMax         PRESENT
    VectorParameter                          PRESENT   (control: the search can see strings)

Every node the message names — `Time`, `Sine`, `Multiply`, `Add`, `Lerp` — and all three scalar
parameters are in the committed object.

**THE REPARENT IS COMMITTED WITH IT, and the discriminating control is the previous commit's own
object.** HEAD's instance names its new parent:

    /Game/StratArt/Materials/M_OverlayPulse
    /Script/Engine.Material'/Game/StratArt/Materials/M_OverlayPulse.M_OverlayPulse'

and the parent commit's object for the same path (LFS `e6d3478...` size 7933, still present in
`.git/lfs/objects/e6/d3/`) names the old one:

    /Game/AdvancedTurnBasedTileToolkit/Core/GridUI/M_Translucent
    /Script/Engine.Material'/Game/.../M_Translucent.M_Translucent'

So the reparent is what the diff on that path *is*. **A second control, in the tree and not in
the diff:** the three sibling overlays are still on the shared toolkit master —

    MI_Overlay_Reach     -> /Game/AdvancedTurnBasedTileToolkit/Core/GridUI/M_Translucent
    MI_Overlay_Target    -> /Game/AdvancedTurnBasedTileToolkit/Core/GridUI/M_Translucent
    MI_Overlay_Objective -> /Game/AdvancedTurnBasedTileToolkit/Core/GridUI/M_Translucent

— so exactly one instance moved, which is what a duplicate-and-reparent is supposed to do and
what a scan of only the changed file could not have said.

`_TmpBrokenProbe` is gone from the index and from disk; `git ls-files` matches it nowhere and
`Content/StratArt/Materials/` lists nineteen assets, none of them the probe.

### How each checklist item was run

**1.** `git diff --stat -- Source/StratRules Data` printed nothing, and so did
`git diff --stat origin/master HEAD -- Source Data Config Stratocracy.uproject` — those trees are
byte-identical to `4e4e2d1`. The manifests, which are the authority for the gate hashes, read
`"rulesCommit": "96d93ea9..."` and `"dataCommit": "c2f58608..."`, derived from the files rather
than quoted from any checklist.

**2.** `grep -rnE 'strat::[a-z][A-Za-z0-9_]*[[:space:]]*\(' Source/`, with `Source/StratBridge/`
and `Source/StratRules/` excluded, returned **zero lines**. No hit to judge this pass; the usual
crop of legal type names did not even reach the lowercase-initial filter.

**3.** Eight headers include a vendored header (`StratBridge.h` itself and seven under
`Source/StratRules/`). Each was counted for `UCLASS(`/`USTRUCT(`/`UENUM(` at line start:
**0 in all eight.** No vendored header sits in front of UHT.

**4.** `TUniquePtr<FStratBridge>` appears exactly once in the tree, inside a comment in
`Source/StratUI/StratViewModel.h` recording the `C4150` measurement. Every real member is
`TPimplPtr<FStratBridge>` — `StratMatchSubsystem.h:2496`, `StratScoreboardHUD.h:1235`.

**5.** The derivation was extracted from the standing instrument and run, not retyped. It printed
**16 rows**, matching every row of the carried block:

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
    Stratocracy  PublicDependencyModuleNames    Core,CoreUObject,Engine,InputCore,
                                                EnhancedInput,AIModule,NavigationSystem,
                                                StateTreeModule,GameplayStateTreeModule,
                                                Niagara,UMG,Slate,Json,StratRules
    Stratocracy  PrivateDependencyModuleNames   (none)
    Stratocracy  PublicIncludePaths             Stratocracy,Stratocracy/Variant_Strategy,...

All five modules present — none missing, which would itself have been a finding. **The sixteenth
row was read at its site rather than trusted from the extraction**, which is textual and cannot
show a guard: `Source/StratBridge/StratBridge.Build.cs` has it inside

    bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;
    PrivateDefinitions.Add("STRAT_VENDORED_RULES_IN_BRIDGE=" + (bCompileVendoredRulesHere ? "1" : "0"));
    if (!bCompileVendoredRulesHere)
    {
        PrivateDependencyModuleNames.Add("StratRules");
    }

— monolithic targets only, one expression driving both halves, exactly as documented. Not a
finding. `StratUI` has no `Stratocracy` dependency; no cycle appears.

The field census printed **10 names**:

    BRIDGE=            PCHUsage =                        PrivateDefinitions.Add
    PrivateDependencyModuleNames.Add                     PrivateDependencyModuleNames.AddRange
    PublicDependencyModuleNames.AddRange                 PublicIncludePaths.AddRange
    ShadowVariableWarningLevel =                         bCompileVendoredRulesHere =
    bUseUnity =

No eleventh name. `BRIDGE=` is the documented false positive — the tail of the string literal
`"STRAT_VENDORED_RULES_IN_BRIDGE="` quoted above, reached because the `//`-strip cannot see
inside a quoted string. Not chased, not filtered out. **One instrument note worth recording:**
run without `tr -d '\r'`, both commands print mangled rows on this box — `StratBridge` came back
as `atBridge`, `BRIDGE=` as `DGE=` — because the `.Build.cs` files are CRLF and `tr '\n' ' '`
leaves the CR embedded, and a bare CR overwrites the start of the printed line. The counts are
unaffected; the *names* are not, and a reviewer comparing names row by row would see a difference
that is a terminal artifact and not drift.

**6.** `Stratocracy.uproject` lists `Stratocracy`, `StratBridge`, `StratUI`, `StratPlay`.
`IMPLEMENT_MODULE` was grepped per directory before any registration was judged missing: present
in `StratBridge/StratBridge.cpp`, `StratPlay/StratPlay.cpp`, `StratUI/StratUI.cpp`; **absent from
`Source/StratRules/`**, which is why its absence from the array is correct and not a finding. No
new module directory in this diff.

**7.** `git diff --stat -- Source/Stratocracy` printed nothing.

**8.** Every `"/Game/` hit in `Source/` is under a `Tests/` directory or in
`Source/Stratocracy/StratData/ImportStratDataCommandlet.cpp:54` — the two standing exceptions. No
gameplay C++ hit.

**9.** No `Source/StratUI` or `Content/UI` path is in this diff; `git diff --stat HEAD~1 HEAD --
Source/StratUI Content/UI` printed nothing. Nothing this pass could have introduced widget-side
arithmetic.

**10.** Content-lane assets plus record files plus two gate reports; nothing in `Source/`,
`Config/`, `Data/` or `Tests/`. The acting was the `coordinator`'s under a clause whose named
mechanism is not the mechanism used — see Observation 1, non-gating.

**11.** Both record files carry why-first blocks in house style, and the measurements are
recorded as measurements — including the three that measured *nothing*, which are the harder half
to write.

**12.** `git status --porcelain` printed nothing. The commit itself was made by the `coordinator`
on the user's instruction while the previous gate was open; that is disclosed in the commit
message and in `content.md`, is unfalsifiable from a checkout by the previous gate's own
statement, and is the user's call in any case. Nothing is staged now.

### The record checks the brief asked for

**No retired claim is still greppable as live.** A shape sweep over `Tools/architect/state/` for
*does not pulse*, *static tint*, *no time input*, *cannot author a material graph*, *no
node-graph authoring* and *index holds nothing* returns four live-looking sites, and **every one
of them is stamped at its own site, in the same commit**:

- `content.md`, *"AND IT DOES NOT PULSE, WHICH IS THE HALF THAT IS STILL OWED"* — stamp begins on
  the next line, *"THIS BULLET IS DISCHARGED AND IS STAMPED IN PLACE RATHER THAN DELETED"*, and
  it also narrows the bullet's *"there is no node-graph authoring through this route"* to the Lua
  API rather than leaving it to mislead.
- `global.md`, *"AND IT STILL DOES NOT PULSE. A static tint ships where Sec 2.11.5 asks for a
  pulse"* — stamp immediately below, naming **both** sentences as now false.
- `global.md`, *"WHAT W8 STILL OWES, AND IT IS ONE THING: THE PULSE DOES NOT PULSE"* — stamp
  immediately below, discharging the bullet and explicitly declining to close the bullet after it.
- `content.md`'s own *"A COMMIT AGAINST THAT INDEX WOULD HAVE SHIPPED A MATERIAL THAT DOES NOT
  PULSE"* is counterfactual prose about the index, not a live claim about the asset.

`strat_banner_sweep.py` collects claims by shape and collects **none of this shape**, so it
vouches for none of the four; the sweep above is this gate's own and is stated as such.

**No record file rests a claim on the orphaned SHA.** `git grep 54709ac` finds it in the prior
re-gate report (ten times, as that report's inlined instruments) and **once** in the record, at
`content.md:171`:

    inlines its instruments as `git show 54709ac:...` and `git cat-file -p 54709ac:...`, and
    those commands FAIL IN ANY CLONE because that hash was orphaned by the amendment the report
    itself asked for.

**Ruling: that is a mention, not a citation.** It rests no claim on the object; the SHA is the
*subject* of a disclosed defect, and the sentence containing it asserts that the commands fail.
The same paragraph then tells a reader what to substitute — `oid 64ec50f...` size 6477 and
`oid 3a8aa7c...` size 11384 — both of which this gate re-measured above and both of which are
checkable in any clone with no SHA at all. **The decision to leave the prior report byte-intact
was right**: a gate report rewritten to agree with a tree that came after it stops being
evidence, and the disclosure in `content.md` is adequate because it names the file, quotes the
command form, states the failure and supplies the durable substitute. `git cat-file -t 54709ac`
still answers `commit` on *this* box while `git branch -a --contains 54709ac` answers nothing —
which is exactly the trap the disclosure exists to defuse, since the object is reachable here and
in no clone.

**Record ownership.** `python Tools/architect/strat_banner_sweep.py --explain`, run by this gate
without a pipe:

    Banner sweep: content.md, data.md, decisions.md, engine.md, global.md, tests.md
      automation report: 389 entries, all Success (2026.09.04-16.30.20)
      macro census: IMPLEMENT_SIMPLE_AUTOMATION_TEST=389, COMPLEX/_CLASS=0
      report identity: mtime=1788539420, reportCreatedOn='2026.09.04-16.30.20'
      suite claims found: 62 (1 live, 61 stamped)
      tracked items declared: 2
      ...
           global.md:32    LIVE    389/389  ...The suite is **389/389**, every entry Success...
      ...
    SWEEP CLEAN -- no self-contradiction found

**One live suite claim, in `global.md`, and none anywhere else.** No phase verdict outside
`global.md`. The new `content.md` entry says so itself and keeps to it: *"No suite count and no
verdict is stated here; both are `global.md`'s alone."*

**Attribution.** `content.md`'s header makes an entry citing one authority for both halves a
finding. The entry opens:

    **ACTING: the `coordinator`, under `CLAUDE.md`'s EDITOR-DRIVER CLAUSE. WRITING: the
    `coordinator`, under THIS FILE'S FALLBACK CONDITION.**

Two authorities, two halves, inline. The fallback's obligation (1) is discharged with a genuine
control in one lookup — two `execute_script` names absent while
`mcp__NeoStack_Connect__unreal_status` and `mcp__NeoStack_Connect__list_unreal_projects`, same
lookup and same server, came back — and obligation (2) names the writer. Every block appended to
that entry since names the `coordinator`: *"[CORRECTED 2026-09-04 BY THE `coordinator` ... AFTER
`strat-integration-reviewer` BLOCKED ON IT"* and *"[STAMPED 2026-09-04 BY THE `coordinator` ...
AFTER THE RE-GATE BLOCKED ON IT"*. `global.md`'s two stamps say why they need no clause at all
(*"`global.md` is already this seat's to write"*), which is correct.

**Does the entry over-claim about compilation?** No — it under-claims, deliberately, and the
under-claim is the strongest thing in it. It records three instruments as **silent rather than
clean**: `get_statistics` returning all zeros *for the shipped known-good `M_Translucent` too*; a
zero-`LogMaterial` authoring log with a deliberately-broken control material that **also** logged
nothing, so the route is proven not to surface translation errors at all; and a later editor log
naming the material zero times. It then states flatly *"NOTHING IN THIS TREE ESTABLISHED THAT THE
MATERIAL COMPILES; the human sighting below is what establishes it"*, and gives the reasoning
that makes the sighting load-bearing — a material with translation errors renders as the default
checker and does not animate. It also records that the user was asked *explicitly* whether they
saw the Material Editor preview sphere or the game, and quotes the answer: *"I saw it on the hex
in a match, the cycle speed is fine."* That question is the difference between "the material
animates" and "the pulse is drawn where Sec 2.11.5 asks for it", and asking it was right. **The
entry claims no more than that.**

### The commit message

Held to the record's standard, sentence by sentence on the load-bearing ones.

- **The rewritten subject line is true of `fa715d3`.** It now says an *unexamined* commit would
  have shipped the pre-rewire duplicate and that **this commit does not** — verified above by the
  pointer-versus-sha256 comparison it names, and by the node scan of the committed bytes. The
  previous gate's Finding 2, that the sentence named a subject it did not describe, is repaired
  and not merely reworded.
- **`389/389`, and this is the one claim in the message with no artifact in the repository.** The
  message says the suite *"ran 389/389 after the asset change, to a report path outside the
  canonical one so the banner's cited artifact was not overwritten."* Both halves check out, but
  only after going outside the tree. Inside it: the canonical
  `Saved/AutomationReport/index.json` has `reportCreatedOn 2026.09.04-16.30.20`, mtime
  **12:30:20 local**, 389 entries all Success — the run behind the `global.md:32` banner, and
  **older than the asset**, whose mtime is **15:07:00 local**. No `index.json` anywhere under the
  repository is newer than 12:31; every other `Saved/AutomationReport*` is from a previous week
  or is a one-entry control. So the canonical artifact was indeed not overwritten, and the
  claimed post-asset run is not in the tree. **It is real, and this gate found it**, in the
  session scratchpad at `.../scratchpad/AutomationReport-pulse/index.json`:

      mtime 2026-09-04 15:13:34 local
      reportCreatedOn 2026.09.04-19.13.34
      succeeded 389  failed 0  notRun 0  succeededWithWarnings 0
      entries 389    distinct states: {'Success'}

  389/389, all Success, at 15:13 local — **after** the 15:07 asset write and **before** the 15:50
  commit. The sentence is TRUE. See Observation 3 for the cost of where the artifact lives.
- **"every change since is prose"** — consistent with the diff: the only non-prose paths are the
  two assets, written at 15:07, before the 15:13 run.
- **"NO GATE HAS RETURNED PASS ON THIS TREE"** — true as read, and see Observation 2.
- The staging narrative (`64ec50f...` size 6477 in the index, `MaterialExpressionTime` / `Sine` /
  `PulseSpeed` absent from it, `VectorParameter` present as control) matches the first gate report
  and is past tense about a condition that no longer exists. Nothing in it is asserted of the
  current tree.

## Findings

None.

## Observations

Non-gating.

**1. The route is still outside any clause that plainly covers it, and this gate does not close
that.** `CLAUDE.md`'s editor-driver clause licenses the `coordinator` to *"drive the live editor
to do content-lane work"*; the work was done by a headless `UnrealEditor-Cmd.exe
-run=pythonscript` commandlet, with **no editor running at all** — `tasklist` matched `unreal`
zero times against a control of 364 processes, `netstat` showed nothing on 9315 against a control
of 37 LISTENING sockets, all seven `runtimes.json` entries were fossils. The clause's *condition*
held and its *purpose* held; its named *mechanism* did not. The entry does not claim coverage —
it carries a bullet headed **"WHETHER IT SATISFIES THE CLAUSE IS THE USER'S TO RULE"** and says
*"the route is not the one the clause names, and that is flagged here rather than assumed
covered."* That is the honest handling and it is why this is an Observation: a reviewer reports,
and blocking on an unruled question would be ruling it. **It wants a user ruling before the next
pass takes this route as settled.** One wrinkle a reader should have: the entry's own opening
line, and `global.md`'s stamp, both say the acting was *"under `CLAUDE.md`'s EDITOR-DRIVER
CLAUSE"* flatly. Each does point at the flag — `global.md` names *"the route question it leaves
for the user to rule"* in the same block — so no reader is misled, and acting under no named
authority at all would be worse. The first gate on this pass reached the same call.

**2. `NO GATE HAS RETURNED PASS ON THIS TREE` is true as I read it and false as I file this, and
the message is amendable until push.** This is the same shape as the defect the pass just
repaired — *"a record sentence asserting the state of its own index cannot survive being
recorded"* — one level out: a message sentence asserting the state of its own gating cannot
survive being gated. It is not a finding, because it was true of every moment before this verdict
and only this verdict falsifies it; blocking on it would make it true again, which is a paradox
rather than a review. But **this commit is unpushed and a pushed message cannot be amended.** The
`coordinator` should amend that sentence before pushing — to a form anchored to the pass rather
than to the gating, e.g. naming the two BLOCKs by their committed report filenames and stating
what they found, which stays true forever. The durable-form reasoning the same commit applies to
`content.md` applies to it unchanged.

**3. The `389/389` evidence lives in a session scratchpad, which is not a checkout and not
durable.** The run is real and this gate read it, but the message routes past the canonical
report deliberately and then names no path, so a reader has nothing to follow, and the temp
directory is gone once the session is. The figures are inlined in this report above for exactly
that reason. If the record is ever to rest a claim on that run, the durable move is the one this
project already knows: inline the report's own numbers and its `reportCreatedOn`, never its path.
Nothing in the record currently rests on it — `global.md:32` cites the 12:30 canonical report and
the pass moved no count — so nothing is broken today.

**4. `uncommitted` in the new entry's provenance line is the same tense hazard the entry itself
names, and it is a corpus-wide convention rather than this pass's defect.** `content.md:48` reads
*"over base `4e4e2d1`, no worktree and no merge, uncommitted"*, which the commit carrying it
falsifies. So does every other entry written that way: a case-insensitive sweep for `uncommitted`
and `not yet committed` across `Tools/architect/state/` returns dozens, including 30+ in
`engine.md` alone. Flagged here because the pass has just spent two gates learning that a record
sentence about its own commit state cannot survive the commit, and this is the last instance of
that shape still standing in the file. Not gated, because gating it would condemn the whole
corpus on a pass that did not introduce it. Worth a steward decision on the convention, not a fix
in this commit.

**5. The stamp on the 2026-09-02 bullet at `content.md:426` names its writer but no authority for
the write.** It opens *"[STAMPED 2026-09-04 BY THE `coordinator`"* and stops there, while the
same file's header makes the authority for a `coordinator` write load-bearing and the project's
own inline-attribution reasoning is that a reader arriving by a citation lands on the block, not
on the header above it. The `global.md` stamps do better — they say in terms why they need no
clause. The fallback condition *was* measured with a control in the same commit, one entry up, so
nothing is unsupported; the block just does not carry its own support. Non-gating.

**6. Neither the build nor the suite was run by this gate, by role.** The `389/389` figures above
are read out of exported report JSON, not produced. `strat-test-author` owns that report.

VERDICT: PASS
