---
name: strat-parallel
description: Run the remaining Stratocracy work across several git worktrees at once — the tree topology, the concurrency rules that replace the editor-closed/editor-open phase alternation, per-tree command templates derived from each tree's own root, and the merge protocol. Use when the user says "start a lane", "spin up a slot", "run lanes in parallel", "merge the branch", or invokes /strat-parallel.
---

# Parallel lanes — worktree orchestration

This skill runs in the **main thread**. It replaces `strat-hotseat`'s single-tree,
editor-closed/editor-open phase alternation for the remaining UI-heavy milestones.
`strat-hotseat` stays as the historical record of the closed hot-seat milestone, and its lane
definitions, agent contracts and reviewer-gate discipline are all still in force — read it for
those. What changes here is *where* work happens and *how many* things may happen at once.

Everything stated as measured below was measured on this machine on 2026-08-20 and the measurement
is named. Everything not measured is marked **unverified**. The distinction matters more than
usual here, because two rules in the plan this skill implements were wrong the first time for
exactly one reason: a limitation was asserted rather than run.

---

## Why worktrees at all

The remaining work — production menu (§2.11.5, the MVP blocker), match-flow surfaces, onboarding
(§2.11.6), save-slot UI, balance lock, packaging — is UI-heavy, and UI work is the *least* parallel
kind of work in this repo. It ends in binary `.uasset` files that cannot merge, authored through a
single live editor.

So the goal is not "more agents on the same thing". It is: **the serialized editor lane never
blocks the parallel C++ lanes, and never idles.** Wave *n*'s asset authoring runs at the same time
as wave *n+1*'s C++.

---

## Topology

| Tree | Path | Branch | Role | `Intermediate/`? |
|---|---|---|---|---|
| **integration** | `E:\MultiAgent\Stratocracy` | `master` | The only editor tree. NeoStack, asset authoring, PIE, the suite of record, every `state/` commit. | yes |
| **slot-1** | `E:\MultiAgent\Strat-wt\slot-1` | `feat/<name>` | Build slot — engineer + test-author pair | yes — 3.8 GB, built and green |
| **slot-2** | `E:\MultiAgent\Strat-wt\slot-2` | `feat/<name>` | Build slot — second pair | yes — 3.8 GB |
| **draft-N** | `E:\MultiAgent\Strat-wt\draft-<name>` | `feat/<name>` | Drafting lane: tooling, tests against existing symbols, `Config/`, evidence, reviewer audits | no (~900 MB) |

**Lanes are cheap; build slots are not.** A drafting worktree never compiles and costs ~900 MB. A
build slot has been built once and keeps its `Intermediate/`: 3.8 GB measured (898 MB checkout +
2.8 GB `Intermediate` + 131 MB `Binaries`).

**Slot affinity — a feature keeps one slot for its whole life.** A slot is a resource a lane
borrows, but round-robining branches through a slot thrashes the incremental state and re-pays the
full build, which is the cost the slot exists to avoid.

**Measured capacity:** 16C/24T, 64 GB RAM, 993 GB free on `E:`. Disk is a non-issue; **CPU during
concurrent compiles is the limit.**

### Creating a lane

```
git -C "E:/MultiAgent/Stratocracy" worktree add "E:/MultiAgent/Strat-wt/<name>" -b feat/<name>
```

**Do not use the Agent tool's ephemeral `isolation: "worktree"` for anything that compiles.** It is
auto-cleaned, so every dispatch re-pays the full build. Persistent named worktrees for build
lanes; ephemeral isolation is fine for the read-only reviewer.

**The hook already works there.** A linked worktree resolves hook lookup through the *common*
directory, so the `pre-commit` installed once in the main clone runs in every worktree of it.
Measured: `git rev-parse --git-path hooks` in a probe worktree returned
`E:/MultiAgent/Stratocracy/.git/hooks`, and it refused the same staged bytes the integration tree
refused. `install.sh` is once per **clone**, not once per worktree.

---

## The concurrency rules

These replace the editor-closed/editor-open phase invariant.

1. **One editor, one tree.** The editor may only ever open the integration tree. Never build the
   integration tree while its editor is open — `Build.bat` fails on the DLL write lock.

2. **One builder agent per tree**, and `strat-test-author` follows a green build in that same tree.
   The old rule, now scoped per-tree instead of globally.

3. **At most two trees compiling or running the suite at once.** A headless suite run holds read
   locks on that tree's DLLs and blocks its linker, so a run and a build *in one tree* are also
   mutually exclusive. Cap each build with `-MaxParallelActions=10`.

4. **`Content/` is touched on the integration tree only.** `.uasset` has no merge. A feature branch
   that needs an asset does not create one; it lands its C++ and the asset is authored in the next
   editor batch.

5. **A record claim may be written wherever a real artifact backs it — which is any *built* slot.**
   This rule was wrong twice, both times because a limitation was asserted without being run.
   First form, "the sweep is toothless in a worktree": false — the `IMPLEMENT_SIMPLE_AUTOMATION_TEST`
   macro census reads `Source/`, which a worktree has in full, and refuses a wrong count anywhere.
   Second form, "a worktree cannot check greenness": also false — the probe ran the full suite and
   wrote its own `Saved/AutomationReport/index.json` (`108 succeeded / 0 failed / 0 notRun`).
   The surviving, narrower limit: **a *drafting* worktree that has never been built has no report**,
   so it can falsify a wrong count but not the claim "zero non-Success".
   Keep `state/` commits on the integration tree for **ordering** reasons — one writer, one
   history — not because the guard is weaker elsewhere.

6. **Vendored `Source/StratRules/` and `Data/` stay byte-untouched in every tree.** A `Ui.h` change
   is filed upstream to `E:\MultiAgent\stratocracy-crew` and re-vendored. Never edited here.

7. **The reviewer gates a merge, not just a phase.** `VERDICT: PASS` on the rebased branch before
   merge, and a re-gate on the integration tree after it — a merge is a post-pass change.

### The static gate comes before the build slot

`strat-integration-reviewer`'s audit is **compile-free** and catches the three failures this
project hits most: `strat::` outside `StratBridge`/`StratRules` (`LNK2019`, measured 8×), a
vendored header in front of UHT, and a `/Game/` literal in gameplay C++. Run it in the drafting
lane *before* the branch takes a build slot. That converts most would-be broken builds into a
static finding without pretending a compile is optional — it is not: there is no offline functional
test in this project, and the suite is UE automation tests run against built DLLs.

---

## Command templates — derive the path, never paste it

> **THIS IS THE MOST DANGEROUS ITEM IN THIS FILE.** `strat-hotseat`'s recorded commands hardcode
> `E:\MultiAgent\Stratocracy\…`. An agent working in slot-1 that runs them **builds and tests the
> integration tree and reports green for code it never compiled.** Every build or suite command in
> every dispatch prompt must carry paths derived from *that tree's own*
> `git rev-parse --show-toplevel`, and the agent must echo the project path it actually built.

Put this at the top of every dispatch prompt for a lane, verbatim:

```bash
TOP=$(git rev-parse --show-toplevel)
UPROJ=$(cygpath -w "$TOP/Stratocracy.uproject")
echo "THIS AGENT IS WORKING IN: $TOP"
echo "AND WILL BUILD: $UPROJ"
```

**Build** (a lane tree — note the flag, and see below):

```bash
"C:/Program Files/Epic Games/UE_5.8/Engine/Build/BatchFiles/Build.bat" \
  StratocracyEditor Win64 Development -project="$UPROJ" \
  -waitmutex -NoHotReloadFromIDE -MaxParallelActions=10 > "$TOP/build.log" 2>&1
REAL_EXIT=$?; echo "REAL_EXIT=$REAL_EXIT"; grep -E "^Result:" "$TOP/build.log"
```

**Headless suite** (a lane tree):

```bash
"C:/Program Files/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" "$UPROJ" \
  -ExecCmds="Automation RunTests Stratocracy;Quit" -unattended -nopause -nosplash -nullrhi \
  -ReportExportPath="$(cygpath -w "$TOP/Saved/AutomationReport")" > "$TOP/suite.log" 2>&1
REAL_EXIT=$?; echo "REAL_EXIT=$REAL_EXIT"
```

**Banner sweep**, which already derives its own file list from `Tools/architect/state/`:

```bash
python "$(git rev-parse --show-toplevel)/Tools/architect/strat_banner_sweep.py" --explain
```

### `-NoHotReloadFromIDE`, and exactly when it is wrong

**An open editor blocks the editor-target build in *every* worktree on this machine, not just its
own.** `Build.bat` refuses with *"Unable to build while Live Coding is active"*. The guard is
`HotReload.CheckForLiveCodingSessionActive`, and it tests a mutex named from the **output
executable path** — which for an editor target is the *shared engine* `UnrealEditor.exe`, identical
for every project and every worktree, and it is a `Global\` mutex, so it is machine-wide. UBT's own
debug log records the name it checked:

```
Global\LiveCoding_C++Program Files+Epic Games+UE_5.8+Engine+Binaries+Win64+UnrealEditor.exe
```

The guard's first condition is `BuildConfiguration.bAllowHotReloadFromIDE`, and
`-NoHotReloadFromIDE` short-circuits it before the mutex is checked. **Measured:** the probe
rebuilt to `Result: Succeeded`, `REAL_EXIT=0`, 82 actions, 216 s, **with the editor open on the
integration tree throughout**, and the integration tree was untouched — its binaries kept their
2026-08-19 dates and `git status` stayed clean.

A worktree writes only its own `Binaries/` and `Intermediate/`, so it does not contend with the
DLLs the editor holds.

> **Never pass `-NoHotReloadFromIDE` when building the tree the editor actually has open.** There
> it would try to overwrite DLLs the editor is holding. That is rule 1, and the flag is the one
> thing that can defeat it.

### Never pipe a build or a suite run

`Result: Failed` once arrived with **process exit code 0**, because the build was piped to `tail`
and the pipe's exit status masked it. Redirect to a file, capture `$?` on the very next line, and
quote the tool's own `Result:` line — not an exit code, and never a summary of one. An agent that
pipes its build to `tail` or `head` will report a failed build as a pass.

---

## Merge protocol

A branch is ready to merge when its own tree is green and the reviewer has passed the **rebased**
branch — not the branch as it was written.

1. **Rebase in the lane tree**, never on the integration tree:
   `git -C <lane> fetch . master:master && git -C <lane> rebase master`
2. **Rebuild and re-run the suite in that lane tree.** A rebase is a change; a green from before it
   describes different bytes. Quote the new `Result:` line and the new report counts.
3. **Reviewer gate on the rebased branch** → `VERDICT: PASS`. A `BLOCK` goes back to the lane.
4. **Hand the merge to the user.** *Agents do not commit or merge* — staging, commits and merges
   are the user's call, in every tree.
5. **After the merge lands: re-run the suite on the integration tree, and re-gate narrowly.**
   A `VERDICT: PASS` describes a tree, not a branch, and a merge is a post-pass change.
6. **Then the record.** The owning lane's file under `Tools/architect/state/` gets the entry;
   `state/global.md` gets the banner and the suite count — it is the only file that may carry a
   live one, enforced by the sweep's `RECORD OWNERSHIP` check.

### Clause counting across a merge

Count new clauses by **set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST`**, never by an
acceptance-ID grep — an ID grep once undercounted 8 as 5. The suite figure must move by exactly
that difference.

---

## Checklist before dispatching any lane

- [ ] `git -C <tree> status --short` is clean in **every** tree.
- [ ] The dispatch prompt derives `TOP` from that tree and the agent is told to echo it.
- [ ] The prompt says the tree outranks this file, the plan, and the prompt itself. **The brief is
      not ground truth** — this skill's own numbers, and the coordinator's dispatch prompts, have
      both carried wrong figures into the record before. Pass derived measurements.
- [ ] No more than two trees will compile or run the suite at once.
- [ ] `-NoHotReloadFromIDE` is present for a lane tree and absent for the editor's tree.
- [ ] No build or suite command is piped.
- [ ] The lane does not touch `Content/`, `Source/StratRules/`, or `Data/`.
- [ ] `strat-integration-reviewer` has statically gated the branch before it takes a build slot.
