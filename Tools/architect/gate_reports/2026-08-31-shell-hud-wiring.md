# Gate account: the title screen is wired to the shell HUD

**Subject.** The uncommitted working tree of `E:/MultiAgent/Stratocracy` on branch `master` at
HEAD `9c5fb3f`, 2026-08-31. Seven paths. The change makes `AStratShellHUD` — built inert by
`ee4acf5` — actually reach the player, and stamps the prose that change falsified.

**Gate.** `strat-integration-reviewer`, three rounds, read-only, on the UNCOMMITTED tree.

**WHO PERSISTED THIS FILE, SAID BECAUSE THE DIRECTORY HAS AN OWNER.** Written by the
`coordinator`. `Tools/architect/` is `strat-data-steward`'s lane and `gate_reports/` is not
carved out of it in `CLAUDE.md`, so this is an out-of-lane write on the same footing as the two
reports already in this directory. It is recorded here rather than left to be inferred. It is
the THIRD out-of-lane write by the `coordinator` in this session — see the standing-condition
note in `Tools/architect/state/content.md`, which flags the pattern as needing a user ruling.

**Why the tree was gated before the commit rather than after.** The recorded lesson from
2026-08-30: gating an uncommitted tree cost one round; gating after the commit cost seven
correction commits. The reviewer can read a working tree.

---

## THE THREE VERDICTS, ALL PRESENT IN THIS FILE ON PURPOSE

| Round | Scope | Verdict |
|---|---|---|
| 1 | Four paths — the asset change and its two record files | `VERDICT: PASS`, zero findings, three observations |
| 2 | Seven paths — after acting on observations B, C and D | `VERDICT: BLOCK`, one finding |
| 3 | Re-check of Finding 1 only | `VERDICT: PASS`, zero findings |

**This table exists because the W6 lane report does not have one.** `grep VERDICT` over
`2026-08-30-w6-title-menu.md` returns nothing: five BLOCK rounds are written up, a sixth gate is
named twice as the corrector of that file's own sentences, and no verdict is recorded — so the
`VERDICT: PASS` W6 was merged on is not producible from a checkout. That is a disclosed gap in
this project's record. It is not repeated here.

---

## The change

    M .agents/ue-project-context.md                    1/1      coordinator
    M Content/StratPlay/BP_StratShellGameMode.uasset   2/2      strat-editor-builder
    M Source/StratPlay/StratShellHUD.h                 49/0     strat-gameplay-engineer
    M Tools/architect/state/content.md                 202/1    strat-editor-builder
    M Tools/architect/state/engine.md                  71/0     strat-gameplay-engineer
    M Tools/architect/state/global.md                  30/4     coordinator
    ?? Content/UI/BP_StratShellHUD.uasset              new      coordinator (OUT OF LANE)

`BP_StratShellHUD` is a Blueprint subclass of the native `AStratShellHUD` with `MenuWidgetClass`
= `WBP_TitleMenu_C`; its other three properties are left at their C++ defaults and are therefore
absent from the package, which is the expected shape. `BP_StratShellGameMode`'s `HUDClass` moved
from the engine default `/Script/Engine.HUD` to `BP_StratShellHUD_C`, and the six menu-building
nodes were removed from its EventGraph — BOTH IN ONE SAVE, because either half alone is broken:
`HUDClass` without the strip draws TWO MENUS, and the strip without `HUDClass` draws none.

---

## Round 1 — VERDICT: PASS

Twelve checks, zero findings. The central claim was measured rather than taken on report: the
reviewer compared the HEAD LFS blob (`.git/lfs/objects/dd/9b/dd9b8b8e…`, resolved from the
pointer at `HEAD:Content/StratPlay/BP_StratShellGameMode.uasset`) against the worktree package
using ONE scanner over both sides.

    probe                            HEAD     WORKTREE
    K2Node_CreateWidget              True     False
    AddToViewport                    True     False
    DelayUntilNextTick               True     False
    SetInputMode_UIOnlyEx            True     False
    bShowMouseCursor                 True     False
    GetPlayerController              True     False
    WBP_TitleMenu                    True     False
    HUDClass                         False    True
    BP_StratShellHUD_C               False    True

The instrument is shown able to see every one of the six nodes before their absence is read as
evidence. **The menu is reachable by exactly one route: GameMode → HUD → widget.**

LFS confirmed for the new asset: `git check-attr filter diff merge text -- Content/UI/BP_StratShellHUD.uasset`
returns `filter: lfs`, so it commits as a pointer and not as package bytes.

Three observations, all acted on before round 2:

- **B.** `Source/StratPlay/StratShellHUD.h`'s `NOT IN THIS ROUND` block was stale — *"this class
  is inert until an asset points at it"* is false once an asset points at it. Engineer's lane.
- **C.** `content.md`'s new entry retracted the false `GameDefaultMap` handoff in its own copy
  while the identical present-tense claim stood unstamped in the dated 2026-08-30 W6 entry of the
  same file. A retraction covering one copy and not its twin is the half-adopted-marker failure.
- **D.** `.agents/ue-project-context.md` omitted `UMG` from `StratPlay`'s private dependencies.
  No crew agent owns that file; the `coordinator` fixes it in the phase commit.

`VERDICT: PASS`

---

## Round 2 — VERDICT: BLOCK

Scope widened to seven paths after B, C and D were acted on. All twelve checks passed. One
finding, and it was in the `coordinator`'s own file:

**Finding 1 — `global.md`'s carried-debt ledger asserted, in its topmost live entry, a debt that
this same tree discharges.** `grep -n "DEBT IS UNCHANGED" Tools/architect/state/global.md`
returned two hits. The line-64 copy sits in the topmost entry — the one a reader treats as
current — four lines above text ADDED BY THIS DIFF describing that very debt being discharged.
A reader consulting the ledger was being sent to do work that is done. This is the project's own
recorded failure: *a correction that only adds leaves both claims live*.

The reviewer verified the discharge independently of every record file, from bytes, with a
negative control on the HEAD side, and confirmed the name table is legible in both packages
(`EventGraph` and `ReceiveBeginPlay` readable in each) so the absences are measurements.

`VERDICT: BLOCK`

---

## Round 3 — VERDICT: PASS

Only `Tools/architect/state/global.md` changed (`12 4` → `30 4`). The reviewer VERIFIED that
claim rather than accepting it, blob-hashing the other six paths and finding every one identical
to what it had gated, so checks 1–12 carry forward legitimately.

Both copies of the debt sentence are now stamped adjacently, neither deleted. The line-64 stamp
carries the evidence and attributes acting separately (`strat-editor-builder` on the GameMode,
`coordinator` out of lane and under no clause on `BP_StratShellHUD`); the line-170 stamp in the
date-scoped historical entry POINTS UP for the evidence rather than restating it, which keeps one
copy of a moving fact in the file and leaves the other a pointer. **The conditional hazard
sentence is retained byte-for-byte in both places and both stamps say in terms that it is not
retracted** — setting one half without the other still draws two menus, and that stays true of
any future map adopting this class.

The reviewer was asked whether the discharge should instead be stated in the topmost entry's own
opening, and refused: that would create a second live assertion about a subject that moves, in
the one file whose header forbids restating a fact it already owns.

`VERDICT: PASS`

---

## Evidence a checkout can re-execute

    python Tools/architect/strat_banner_sweep.py > sweep.out 2>&1; echo $?
    # EXIT=0, SWEEP CLEAN, suite claims found: 49 (1 live, 48 stamped)

    python Tools/architect/strat_suite_report_gate.py
    # 318/318 Success, 0 failed, 0 notRun, 0 succeededWithWarnings
    # reportCreatedOn 2026.08.31-16.32.53, SUITE REPORT GATE CLEAN

**REDIRECT, NEVER PIPE.** `python … | tail` followed by `echo $?` reports `tail`'s status, not
the script's. The claim that `strat_banner_sweep.py` "has always exited 0" is false and
originated exactly that way.

**The suite was NOT re-run after the comment-only header stamp, deliberately, and the reviewer
upheld it on the count's own definition rather than on the phrase "comment-only":**
`IMPLEMENT_SIMPLE_AUTOMATION_TEST` totals 318 at HEAD and 318 in the worktree, collected by one
function over both sides, and the diff over `Source/` touches that token on zero `+`/`-` lines.
No clause was added, removed or renamed. Re-running would also have overwritten
`Saved/AutomationReport/index.json` and invalidated `global.md`'s live citation a second time —
the self-measuring trap that produced a sweep failure earlier the same day.

## Live PIE, which no clause can reach

Run on `/Game/StratMaps/Lvl_Title` before the editor was closed. **Only a viewport sees these,
and a screenshot is not a test** — this is an observation, not a regression pin.

- `obj list class=WBP_TitleMenu_C` → **`1 Objects`**, and `GetAll StratShellHUD MenuWidget` names
  that same instance. The two-menu hazard is closed BY COUNT, not by argument.
- `bMenuDrewOnAConfiguredShell = True`, `LastFailureReason` empty — the requirement the class
  exists to make observable holds.
- **`ConfigurationTicksWaited = 0`.** The Blueprint graph needed a one-tick hop because it fired
  off `Event BeginPlay`, ahead of `ConfigureMatchDestination`. The HUD's `BeginPlay` lands after
  `AStratShellGameMode::BeginPlay` has returned, so the C++ route REMOVES the ordering race
  rather than waiting it out. The 4-tick budget is untouched slack.
- `Saved/Screenshots/WindowsEditor/ScreenShot00074.png` via `playtest_console("shot showui")`:
  one menu, `New Match` and `Quit` enabled, `Continue` greyed *"No saved match."*, `Return to
  Title` greyed *"No match in progress."* — W6's recorded routes, now through the HUD.
- `playtest_observe` returned a viewport with NO widgets composited. That is this project's
  recorded capture limitation, not an absent menu; `shot showui` is the control.

## Two operator errors in this pass, both recorded rather than buried

**1. `-NoHotReloadFromIDE` was put in a dispatch brief for a SAME-TREE build with the editor
open.** That flag is for WORKTREES ONLY. It defeats the Live Coding mutex but not the DLL write
lock, so all four compiles succeeded and the link died at `LNK1104`. `strat-gameplay-engineer`
caught it and reported the brief as wrong. The editor was then closed and the tree rebuilt:
`Result: Succeeded`, `Binaries/Win64/UnrealEditor-StratPlay.dll` relinked with its mtime moving
to 2026-08-31 13:12 — the positive evidence that a build happened rather than a cached no-op.
**This was the second occurrence of this same error.**

**2. The editor was force-terminated, discarding an in-memory dirty package.**
`GetDirtyContentPackages` reported `/Game/StratPlay/BP_StratShellGameMode` dirty (PIE noise; it
had been saved earlier and the on-disk bytes were already gated). `CloseMainWindow()` hung on a
save prompt for 90 s, so the process was killed rather than allowed to write. `git hash-object`
before and after is identical on all three assets, which is what makes the discard provably clean:

    8c79b20d2af1fe5566485fa40a15d1a5c5ad1f63  Content/StratPlay/BP_StratShellGameMode.uasset
    7f95c1d719bf90e889e0bf53a9b9bf9b82829b67  Content/UI/BP_StratShellHUD.uasset
    68e4566a1436b758a4783de37a1f8779e51af9e8  Content/StratMaps/Lvl_Title.umap

## Open, and named rather than left to be found

- **The out-of-lane authoring of `BP_StratShellHUD` sits under NO clause.** The editor-driver
  clause conditions on `execute_script` being ABSENT; it was PRESENT. An earlier draft of
  `content.md` cited that clause and the citation was withdrawn. This is the second exception in
  two days bending the lane rule around the `coordinator`'s tool surface — once tool-absent, once
  tool-present-but-client-blocked. **Whether a BLOCKED tool falls inside any existing clause is
  the user's to rule.** The reviewer flagged it so it is not declared a non-precedent a third
  time.
- **Still unpinned by any clause:** the menu APPEARING, the re-arming timer FIRING, and
  `ApplyMenuInputMode`. All need a viewport the suite does not have.
- **An inert `Event BeginPlay` stub remains** in `BP_StratShellGameMode`'s EventGraph, fully
  disconnected. `delete_node` refuses override event nodes and the API offers no inverse. The
  reviewer judged it acceptable: an empty `ReceiveBeginPlay` override does not suppress the
  native path, and asset surgery would risk a package for no behavioural gain.
- **`AStratShellHUD` is `STRATPLAY_API` while its convention sibling `AStratScoreboardHUD` is
  `STRATUI_API`.** Two `AHUD` subclasses that cite each other's design in two different modules.
  Not a layering violation and blocks nothing; worth an engineer-lane decision before the next HUD.
