# Stratocracy state — global

> **Sole writer: `coordinator + strat-data-steward`.** Split out of `Tools/architect/state.md` on 2026-08-20, which is now
> FROZEN as the historical record of closed milestones. This directory is the live record.
>
> **Holds:** Milestone status, the banner, the candidate backlog, and the cross-lane carried-debt ledger. **The only file that may state a suite count or a phase verdict.**
>
> **One file owns each fact class.** Other files may *link* to a fact recorded here; they may not
> restate it. Only `global.md` may carry a live suite count or a phase verdict — a live `N/N`
> in any other file is a finding, enforced by `strat_banner_sweep.py`'s RECORD OWNERSHIP check.
> Everything under `## NEXT` is swept as live; stamp an entry that has become history rather
> than deleting it, exactly as `state.md` did.

_Last run 2026-08-21 (THE STRIP IS WIRED END TO END AND HAS NEVER DRAWN. Both halves of that
sentence are load-bearing and the second is the one a later reader will otherwise lose.
`WBP_DirectiveStrip` is reparented onto `UStratGuidanceWidget` and
`BP_StratScoreboardHUD.GuidanceWidgetClass` now names it, so the route from
`FStratViewModel::Guidance` to a widget is complete in every link. NOTHING HAS BEEN OBSERVED ON
SCREEN. Authored by the coordinator over the editor's HTTP endpoint under the standing ruling;
gated and PASSED with zero findings.
WHAT THE EVIDENCE REACHES, AND THE THREE THINGS IT DOES NOT. It proves the property path
resolves, the parent class binds, all four `Return Value` pins are `connected`, and the suite
stayed 148/148 across the change. It proves NOTHING about (a) whether the strip's Slate hierarchy
lays out at top centre, (b) whether `AddToViewport` yields a VISIBLE widget for this class, or
(c) whether `StripBorder.Visibility` evaluates to `Visible` on a real frame while a beat holds the
line. A reparent that destroys a variable and re-scopes four bindings is PRECISELY the change
that leaves all of those green and the screen blank. **§2.11.6-B IS NOT CLOSED AND IS NOT CLAIMED
HERE.** It closes when a human-driven playtest has seen a directive on screen -- automated PIE
input cannot do it, because injection never reaches `UPlayerInput`. OWNED: coordinator, to
schedule with the user.
THE REPARENT WAS PROVED AT THE PACKAGE-BYTE LEVEL, NOT BY THE COMPILER'S SILENCE. The gate
diffed the real bytes and found the corroboration this pass had not: `NewVariables` and
`EStratGuidanceBeat::None` BOTH DISAPPEAR -- the local `Guidance` variable AND its serialized
default, so the shadow is genuinely gone rather than hidden -- while `StratGuidanceWidget:Guidance`
APPEARS, which is the member reference re-scoping to the native class. That is byte-level proof
the four bindings resolve to the INHERITED property. `remove_variable`'s "variable not found" was
truthful rather than a miss. Re-measured independently afterwards: `WontTick` True -> False,
`WillTick` False -> True, `NewVariables` True -> False, `StratGuidanceWidget:Guidance` False ->
True.
THE REPARENT FLIPPED THE WIDGET'S TICK PREDICTION AND THIS PASS DID NOT NOTICE. `WBP_DirectiveStrip`
serialized `WontTick` before and serializes `WillTick` now, with `bClassRequiresNativeTick` newly
present -- while `UStratGuidanceWidget` overrides NO `NativeTick` (grepped: the only out-of-line
definition in its `.cpp` is `PushGuidance`). The likely mechanism is the UMG compiler treating any
native `UUserWidget` subclass parent as tick-requiring; THE MECHANISM IS NOT VERIFIED and is
recorded as an inference from serialized bytes. Cost is one per-frame no-op tick on one HUD widget,
negligible -- but it is a real behavioural delta this pass introduced and the next profiling pass
should not have to rediscover why the strip ticks.
`GuidanceZOrder` STAYS AT ITS C++ DEFAULT OF 10 AND IS NOT DERIVABLE FROM THE GDD -- do not cite
§2.11 for it. The GDD carries no z-order language anywhere; what it fixes is GEOMETRY, and it
SEPARATES these two surfaces: §2.11.4 puts the scoreboard TOP-LEFT, §2.11.2 draws the directive
strip TOP CENTRE. They are not designed to overlap, so ordering is not load-bearing for occlusion
in the intended layout. 10-over-0 is right as the SAFE DIRECTION UNDER A LAYOUT NOBODY HAS SEEN:
a full-width strip can overlap a top-left panel at real resolutions whatever the mock-up says, and
§2.11.6's "no beat expires unheard" fails SILENTLY if the line is drawn behind something. Above is
recoverable; behind is a blank teach. It is deliberately NOT overridden on the Blueprint -- the
property is ABSENT from the `.uasset`, which under this tree's own rule is STRONGER evidence that
it equals the C++ default than setting it would be.
NEITHER EDITED ASSET IS PINNED BY ANY CLAUSE, and that is a new gap this pass created. Nothing
asserts `WBP_DirectiveStrip`'s parent is `UStratGuidanceWidget`, and nothing asserts
`BP_StratScoreboardHUD.GuidanceWidgetClass` is non-null; both facts are held by `.uasset` bytes
alone and would regress silently under an editor accident. A CDO-default clause is cheap and would
have caught the pre-existing `None`. CANDIDATE FOR `strat-test-author`, not owed by this pass.
THE SUITE WAS RE-RUN AFTER THE ASSET EDITS RATHER THAN ASSUMED UNAFFECTED, because clauses in this
tree do read `.uasset` bytes: `succeeded 148 / failed 0 / notRun 0`, zero non-Success,
`reportCreatedOn 2026.08.22-00.21.06`. `Saved/SaveGames/` held ZERO files either side and the
directory mtime MOVED, 20:15:10 -> 20:21:06 -- the control discharged a second time on this tree,
not borrowed.
A MEASUREMENT INSTRUMENT WAS WRONG TWICE IN ONE HOUR AND BOTH WOULD HAVE PRODUCED CONFIDENT
FALSEHOODS. Reading pins, `p.linked` DOES NOT EXIST -- the field is `connected` -- so a check
written against it reported ZERO connected bindings immediately after the reparent, which reads
exactly like the reparent silently breaking every binding, and a clean compile would not have
contradicted it because an unconnected return simply yields a default. Separately, `.uasset` is
LFS-FILTERED here, so `git show HEAD:<path>` returns the THREE-LINE POINTER and not the package;
a byte comparison against it disagreed with the gate on two tokens until the bytes were smudged.
`git hash-object` applies the filters and stays trustworthy; `git show` does not. Both times the
gate's reading was correct and this pass's was not.)


_Last run 2026-08-21 (THE GUIDANCE ROUTE LANDED AND THE THREE WIDGETS ARE NO LONGER INERT BY
DESIGN -- though nothing has DRAWN yet, and that distinction is kept below rather than blurred.
The suite **is now 148/148**, 141 -> 148 by set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST`
walked over `Source/` -- +7, none removed, derived independently three times and never
incremented -- zero non-Success, `notRun` 0, `reportCreatedOn 2026.08.21-23.56.04`. Build
`Result: Succeeded`, `REAL_EXIT=0`.
WHAT CLOSED THE GAP THE LAST ENTRY NAMED. `UStratGuidanceWidget` (`Source/StratUI/`) is a
`UCLASS(Abstract, Blueprintable)` `UUserWidget` holding `UPROPERTY(BlueprintReadOnly, Transient)
FStratGuidanceView Guidance`, a `BlueprintImplementableEvent` refresh hook, and a NON-REFLECTED
`PushGuidance`. Lifetime is owned by `AStratScoreboardHUD`; the push happens in
`UStratMatchSubsystem::ApplyView`. `GetViewModel()` was NOT made a `UFUNCTION` -- its header
comment is a ruling with a stated reason, not an oversight, and it stands untouched.
TWO ENGINEERING CALLS WERE BETTER THAN THE BRIEF THAT COMMISSIONED THEM, and the gate improved
on one again. Widget lifetime sits on the HUD rather than the subsystem because creating a
widget from StratPlay would have forced `UMG`, `Slate` and `SlateCore` into `StratPlay.Build.cs`;
no `.Build.cs` changed and no arrow moved. And the push is in `ApplyView` rather than
`RefreshPresentation` because the latter is the UNDECORATED path -- the reviewer then traced it
further and found `RefreshPresentation` ITSELF calls `ApplyView`, so a push there would have been
a DOUBLE push on that path, not merely a dead one. The header records the weaker of the two
reasons.
THE ACCEPTANCE ID WAS RULED `T-INT-05`, AGAINST THE ENGINEER'S PROPOSAL, AND THE GATE AGREED
INDEPENDENTLY. `T-UI-02` is *"the UI queries the module and never recomputes movement"* -- a
query-result comparison of the highlight set against `Move.h::reachable`. `T-UI-06` is the beat-1a
AVAILABILITY property, and its own spec warns that a clause reaching past availability *"has
wandered back into `T-UI-02`"*. These seven pin neither: they pin TRANSPORT, which is
`T-INT-05`'s *"rebuild the screen from the view model alone"*. Precedent decided it rather than
taste -- `T-INT-05.LockArisesFromTheGuidanceLayer` ALREADY files guidance-layer state reaching
presentation under this ID, and `T-INT-05.ApplyViewSpawnsMovesAndDestroys` is the direct sibling
of the new `ApplyViewPushesTheModelsGuidance`. Two consequences worth naming: no ID was minted,
nothing went upstream, and the `T-UI-02` PARTIAL FIT recorded above was not deepened by seven
more clauses.
NO CLAUSE TYPES A GUIDANCE FIELD VALUE ANYWHERE. Equality is
`FStratGuidanceView::StaticStruct()->CompareScriptStruct` and every varied shape comes from
`TFieldIterator<FProperty>`, so no `FString`/`FText` comparison appears in any assertion and the
case-insensitivity trap -- which has already produced one clause that could not fail -- cannot
bite. One of the seven is an INSTRUMENT CONTROL: `GuidanceComparisonDistinguishesViews` forces
the comparator to answer NO once per declared field before any sibling relies on it answering
YES, and asserts `FieldsSeen > 0` so an empty iteration cannot pass it vacuously.
THE STANDING SAVE CONTROL IS DISCHARGED ON THIS PASS AND NO LONGER BORROWED. `Saved/SaveGames/`
held ZERO files before and after, and the directory mtime MOVED, 11:54:15 -> 19:56:04, so the
absence was measured here rather than inherited from a neighbouring run. The borrowed 13:56:49
stamp the last two entries carried is retired.
A BRIEF OF MINE WAS WRONG AND IT COST A RED BUILD. I assumed the test-only concrete subclass could
live in `Source/StratPlay/Tests/`. `UMG` is a PRIVATE dependency of StratUI, so StratPlay cannot
name `UStratGuidanceWidget` by include or by link: ~60 `LNK2019` in `Module.StratPlay.gen.cpp.obj`.
The author refused to fix it by editing `StratPlay.Build.cs`, which is outside its lane, and split
the clauses across two modules behind a UMG-free probe instead. The gate confirmed the probe leaks
no UMG type and no `.Build.cs` moved.
TWO RESIDUES, NAMED RATHER THAN DISCOVERED LATER. `AStratScoreboardHUD::PushGuidance`'s literal
SOURCE SHAPE is not pinned: automation cannot read a function body, and a `.cpp` text scan would
flake on a comment edit while passing on an unexpected branch form. What IS pinned is one
distinguishable shape per declared field, each preceded by a clear-to-default push so "unchanged"
cannot be satisfied by a resident value -- but A BRANCH THAT READS A FIELD AND CHANGES NOTHING
SURVIVES IT, and the green tick is not proof the source is field-blind. Separately,
`AlterOneField` reinterprets ANY `FStructProperty` as an `FIntPoint` while its own block promises
to refuse kinds it cannot vary; harmless today because `FIntPoint` is the struct's only such
field, but it is the reflection-driven design's ONE typed assumption and it is the going-inert
failure that design exists to avoid, relocated. Gate it on
`TBaseStructure<FIntPoint>::Get()`. OWED BY `strat-test-author`.
STILL NOT DRAWN, AND THIS IS THE HONEST STATE OF IT. `WBP_DirectiveStrip` is not yet reparented
onto `UStratGuidanceWidget` and `BP_StratScoreboardHUD` has no `GuidanceWidgetClass` set, so the
route compiles, is pinned by seven clauses, and puts nothing on screen. The reparent must DELETE
the Blueprint's own `Guidance` variable rather than rename the C++ property -- renaming would
orphan all 33 bindings -- and a Set node on that variable would fail to compile against the
inherited `BlueprintReadOnly` one. MEASURED BEFOREHAND: `WBP_DirectiveStrip` holds 21 nodes, 4 of
them `Get Guidance` and ZERO `Set`, with the detector controlled by planting a real `Set Guidance`
node in a throwaway widget and confirming it was seen. `CreateGuidanceWidget` and `GuidanceZOrder`
stay unpinned until then, because they end in `AddToViewport` and a `-nullrhi` run has no viewport.
Gated twice, PASS both times, zero findings each: once over the C++ alone, once over the clauses
as a post-pass change.)


_Last run 2026-08-21 (2.11.6'S FIRST THREE SURFACES ARE BUILT AND THE FIRST GATE ON THEM RETURNED
`VERDICT: BLOCK` -- ON THIS VERY RECORD'S ABSENCE, WHICH IS WHY THIS ENTRY EXISTS. `Content/UI/`
gained `WBP_DirectiveStrip`, `WBP_PreMatchBriefing` and `WBP_OneShotTip`, authored by the
coordinator over the editor's own HTTP MCP endpoint because `strat-editor-builder` still cannot
reach it and has no Bash with which to bypass the proxy. The user ruled on that crossing BEFORE any
asset was touched. Recorded here and NOT in `content.md` on the precedent this file already set for
the `BP_StratGameMode` edit: no lane owns a coordinator-authored asset, and `content.md` has a
different sole writer. The reviewer proposed writing both; it named the right facts and the wrong
file, and the lane rule outranks a gate's filing suggestion.
THE GATE PASSED EVERY BYTE-LEVEL AND STRUCTURAL CHECK AND BLOCKED ON PROSE ALONE. Vendored
`Source/StratRules/` and `Data/` compared BY BLOB across all 32 tracked files, zero mismatches;
zero `Source/`, `Config/` and `.uproject` delta; module arrows RE-DERIVED from every
`Source/*/*.Build.cs` and matching; no widget-side arithmetic; nothing staged. Its single finding
was that all three widget names grepped, AT GATE TIME, to ZERO hits across `Tools/`, `.agents/`
and `CLAUDE.md` -- a grep that THIS ENTRY deliberately falsifies, which is the point of it,
so three real debts lived only in conversation. That is the stale-status-line failure in its
worst direction: committing these files would have made the tree assert a guided-opening UI exists
with nothing saying it is inert.
THE THIRD EXPLANATION FOR THE PROXY LATCH IS NOW ALSO DEAD, disproved rather than argued. The
suspect was `NEOSTACK_PROJECT_DIR` pinning at a stale tree. `runtimes.json` holds EXACTLY ONE
runtime -- `editorPid` 49156, `projectPath` `E:/MultiAgent/Stratocracy/` -- so there is no wrong
tree to be pinned to. Better: `list_unreal_projects` READ THAT SAME FILE SUCCESSFULLY and named
Stratocracy in the same round in which `unreal_status` claimed the file contained no active editors.
That is a contradiction INSIDE the proxy between two of its own tools. Three explanations are now
measured false: missing capability, startup order, project pinning.
THE ABSENCE WAS MEASURED WITH A CONTROL, because an absent tool name proves nothing alone. The
`execute_script` lookup failed twice; the SAME keyword search returned `unreal_status` and
`list_unreal_projects`, which is what shows the instrument could speak. Independently the editor's
own endpoint served `serverInfo: unreal-editor 1.0.0-r4254`, exactly one tool, and round-tripped Lua
against that same pid.
`WBP_DirectiveStrip` CARRIES NO AUTHORED DIRECTIVE STRING. It holds one member, `Guidance`, of type
`FStratGuidanceView`, and drives four properties from four PURE functions that break it --
`DirectiveText.Text`, `WindowEndTag.Text`, and the two visibilities off `bActive` and
`bShowsWindowEndTag` through `To Visibility (Boolean)`. §2.11.6-B's four directives stay where
`FStratGuidedOpening::DirectiveTextFor` already puts them.
THAT THE STRUCT WAS THE RIGHT ONE WAS PROVED THREE WAYS, never assumed from an `[OK]`. A
deliberately bogus type (`FStratNoSuchStructXyzzy`) returned `[FAIL] unknown type` where the real
name returned `true`, so the type oracle can fail. Reflection then offered `Break Strat Guidance
View` and `Switch on EStratGuidanceBeat` from inside that Blueprint. And the Break node's pins are
exactly the struct's TEN fields.
`pure=true` IS LOAD-BEARING AND THE COMPILER SAID SO: the first binding attempt failed with
"needs to be bound to a pure function, 'Get Directive Text' is not pure." Quoted rather than
paraphrased, because it is the entire reason those functions are declared as they are.
A CLEAN COMPILE WAS CAUGHT LYING ABOUT WIRING. `WBP_OneShotTip`'s first build reported `0 errors,
0 warnings` while its text getter was connected to NOTHING: a member variable named `TipText`
collided with the widget also named `TipText`, so the getter lookup resolved to the wrong node.
Variables were renamed and every `connect` is now asserted against `true` rather than inferred from
a green compile. EXIT CODE IS NOT A VERDICT, again, this time wearing a compiler's colours.
VERIFICATION WAS READ BACK INDEPENDENTLY AND ITS INSTRUMENT WAS CONTROLLED. All six binding
functions report their `Return Value` pin `linked=true`; a throwaway function with nothing connected
reports `linked=nil`. The six `true`s mean something only because the check can also say nil.
THE THREE WIDGETS ARE INERT AND THAT IS A C++ GAP, NOT AN EDITOR ONE. There is NO Blueprint-reachable
route to `FStratViewModel::Guidance`: `UStratMatchSubsystem::GetViewModel()` is explicitly not a
`UFUNCTION` (the header says so, and says why -- UHT rejects a reference return), `AppliedModel` is
`UPROPERTY(Transient)` with no `BlueprintReadOnly`, and the subsystem is a bare `UCLASS()`. No amount
of editor access would have closed this. It is `strat-gameplay-engineer`'s and it is not discharged
here.
THE TWO SEAMS ARE NOT EQUALLY SOUND, and the gate found the asymmetry that this session's own write-up
had folded flat. Measured on the bytes afterwards rather than taken on report: `WBP_DirectiveStrip`
carries `/Script/StratUI` once and `StratGuidanceView` THIRTY-THREE times, so its member really is the
reflected struct and reparenting to a future C++ base preserves the bindings. `WBP_OneShotTip` carries
`/Script/StratUI` ZERO times -- its `CurrentTip`, `TipIsShowing` and `TipId` are invented
Blueprint-only names with no counterpart anywhere in `Source/` (grep: zero hits). When the engineer
defines the real tip view struct, nothing constrains its field names, so those two bindings will most
likely NOT survive reparenting. The tip widget is a layout and a placeholder seam; the strip is a
wired one.
A CONTENT DEBT, RULED NON-BLOCKING BY THE GATE: §2.11.6-A's three callout strings are typed into
`WBP_PreMatchBriefing`, because `StratGuidedOpening.h` deliberately declares no C++ home for them.
Its stated reason was that "declaring content for a surface that does not exist is how a string
drifts from the GDD unnoticed" -- the surface now EXISTS, which inverts that reasoning. Note the
asymmetry the gate named: §2.11.6-**B**'s directive strings ARE in C++ and are therefore testable,
while A's three now sit in a binary that no grep or clause can compare against the GDD.
NO SUITE RAN THIS PASS AND NONE IS CLAIMED: no C++ changed, so there was nothing to compile and
nothing to re-measure. `Saved/SaveGames/` was likewise not re-controlled this pass, for the same
reason -- the borrowed 13:56:49 stamp from the previous session's run is still borrowed and must be
re-stamped either side of the next real run.)


_Last run 2026-08-21 (THE PASS BELOW WAS GATED AND PASSED, AND BOTH UPSTREAM REQUESTS WERE
FILED. `strat-integration-reviewer` returned `VERDICT: PASS` with ZERO findings over the shipped
opt-in, the new clause, the hardened banner sweep and the coordinator's `Content/` edit. Twelve
checks; vendored `Source/StratRules/` and `Data/` compared by blob rather than by worktree diff.
It confirmed at its source that the fabricated `StratRules -> (nothing)` finding is dead: it
DERIVED the module arrows from every `Source/*/*.Build.cs` and then read
`.agents/ue-project-context.md` before assigning blame -- reading BOTH artifacts, which is the
step that never happened the four times that finding was reported. The note matched the tree
exactly, and the coordinator's independent derivation agreed. THE FIRST GATE SINCE 314ad99 FIXED
THE CHECKLIST, AND IT CAME BACK CLEAN.
ITS ONE NON-GATING CATCH WAS THE COORDINATOR'S OWN LITTER: `build.log` and `suite.log`, which the
documented build and suite templates write to every tree's root on every pass, were untracked AND
un-ignored, so `git add -A` would have committed them. Closed at `6bc9e00`.
BOTH UPSTREAM REQUESTS ARE FILED, ON THE USER'S EXPLICIT INSTRUCTION, AND NEITHER IS GRANTED. The
guided-opening adjacency request landed at crew `a8c8cdb`; the Q27 acceptance-ID request at crew
`3510bc2`. Both spec-only -- `data/` and `cpp_reference/` untouched in each, no manifest or hash
moved, no scenario authored, no acceptance ID minted. Recorded at `b17c359` with the inverse
hazard named for the first time: this tree has long kept FILED IS NOT LANDED, and the REVERSE
costs as much -- a document still reading "not filed" after its request is committed upstream
sends the next reader to file a duplicate. Both evidence files said exactly that, both were true
when written, and neither is now; the superseded text is stamped and quoted, never deleted.
A CORRECTION WENT UPSTREAM WITH THE Q27 FILING RATHER THAN BEING SMOOTHED OVER. Its draft read as
though Q27 ruled all three beat-1a input gates. Read out of the GDD directly: Q27's own row names
END TURN. The beat-1a row is what extends the gate to SPACE, on the stated footing that those are
the machine's only routes from SELECTED to DONE not passing through MOVED, and separately closes
the ATTACK transition. The filed text states the split and the evidence file quotes the request AS
LANDED, because the landed wording and the drafted wording differ for exactly that reason.
THE VENDORED PIN GAP GREW BY THIS SESSION'S OWN WORK and was re-derived rather than left to
stand: `git rev-list --count cb8e12b..HEAD` in the crew repo, measured independently twice, is
now seventeen commits, up from the fifteen the Q27 draft recorded. Both filings are spec-only so
neither creates re-vendor pressure of its own, but Lane A's eventual re-vendor carries all of
them.
WHAT REMAINS NEEDS SOMEBODY THIS SESSION COULD NOT SUPPLY. 2.11.6's unbuilt widgets need an
editor `strat-editor-builder` can actually reach, and it cannot while the `NeoStack_Connect` proxy
latches, because it has no Bash with which to drive the editor's own endpoint -- the workaround
that carried this pass is not available to that agent. Lane A stays blocked until `uiBuildOptions`
is a real header change rather than a spec entry. And both filings now wait on a Director.)

_[STAMPED 2026-08-21, later session — "both filings now wait on a Director" was true when
written and is now HALF false, which is the direction that wastes a reader: the Q27 filing (crew
`3510bc2`) HAS BEEN RULED. The Director minted the ID, and it is `T-UI-06`. The ruling is written
into crew `spec/ui_spec.md` and committed there at `745dfd8` on the user's instruction, which
takes the crew gap over the vendored `rulesCommit` `cb8e12b` to EIGHTEEN commits, re-derived after
the commit rather than incremented from the seventeen recorded above. And
the number was re-enumerated across the GDD, `data/`, `cpp_reference/` and downstream `Source/`
before it was taken, because its only prior occurrence anywhere was the request's own "e.g." and a
grep that stopped there would have called the number taken. TWO HALVES STAY OPEN and are named
rather than assumed done: the shipped GDD still defines only `T-UI-01`—`T-UI-05`, so it does not
yet know the ID; and downstream's clauses still sit under `T-UI-02` as a recorded partial fit,
because re-pointing them is a test-lane edit needing its own build and suite run. The OTHER filing,
crew `a8c8cdb`, is unchanged and still waits on a Director.]_

_Last run 2026-08-21 (THE SHIPPED OPT-IN LANDED AND THE CLAUSE THAT PINS IT IS NO LONGER
OWED. `BP_StratGameMode`'s `MatchConfig` now carries `bRecordCompletionOnMatchEnd` TRUE, so the
packaged game records a completed match and §2.11.6's guided opening retires instead of
re-arming for every player forever. The suite **was 141/141** at that pass, 140 -> 141 by set-difference
on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` walked over `Source/` -- +1, none removed -- zero
non-Success, `notRun` 0. Build `Result: Succeeded`, `REAL_EXIT=0`, and the new translation unit
is named in the build log as `[3/7] Compile StratShippedGameModeOptIn.cpp` rather than assumed
from a green.
THE ASSET DEFAULT WAS READ BEFORE IT WAS WRITTEN, because the trap here inverts the obvious
reading: a property ABSENT from a `.uasset` proves it equals the C++ default, which is FALSE,
WHICH IS THE DEFECT -- so "not found, therefore unchanged, therefore fine" would have reported
the broken state as healthy. Two instruments agreed, each with its own control. The CDO export
carried `AiMaxCommandsPerTurn=256` and `SaveSlotName="StratocracyMatch"` while the asset's name
table carried NEITHER, which is what proves the asset holds only OVERRIDES and the export holds
the EFFECTIVE value; on both readings the flag was unset, hence false. After the write the same
name-table scan shows the flag present and those two still absent, so the whole-struct write did
not materialise inherited fields as overrides -- the instrument that would have shown that harm
is the one that shows its absence.
`BP_StratGameMode_AiVsAi` was deliberately LEFT FALSE and is byte-identical across the pass. An
AI-vs-AI run reaching a result would write the human player's slot, which is the exact failure
the opt-in predicate exists to prevent. The asymmetry is intentional and is not pinned by a
clause yet; a clause requiring FALSE there is a SECOND clause, never a widening of this one.
THE STANDING CHECK PASSED WITH ITS CONTROL: `Saved/SaveGames/` holds ZERO files after the run
and the directory mtime MOVED, 11:17:34 -> 13:56:49, so the absence was measured on this pass
rather than borrowed from a neighbouring one.
THE LANE WAS CROSSED KNOWINGLY AND THE USER RULED ON IT FIRST. `Content/` is
`strat-editor-builder`'s, but the builder had no route to the editor: the `NeoStack_Connect`
proxy exposed exactly two tools all session, and the builder has no Bash with which to reach
the editor's own MCP endpoint. THE STARTUP-ORDER EXPLANATION FOR THAT LATCH IS FALSE, measured
here -- this session STARTED with the editor already up, ~85 minutes old and heartbeating, and
still got the two-tool surface. The latch is in the proxy alone; the editor's own server was
live and complete throughout and served the read, the write, the compile and the save. So the
coordinator made the asset edit, on the user's explicit ruling, and it is recorded here rather
than in `content.md` because no lane owns it.
The banner sweep now PINS THE ARTIFACT IT READS and would have caught the pre-merge report this
entry replaces -- see `data.md`. Its first quiet pass on a real tree with a fresh report is this
one; before the run it failed on both the count and the identity, and both findings cleared.
NOT YET GATED AT THIS WRITING -- no `VERDICT` has been sought for this pass, and this entry
asserts none. **[STAMPED 2026-08-21: it was gated afterwards and PASSED with zero findings; the
entry above this one carries the verdict. That sentence was true when written and is kept as
written.]**)

_Last run 2026-08-21 (THE WRITER WAS GATED `VERDICT: BLOCK` AND THE BLOCK WAS RIGHT. The hook
guarded on `ResolveSaveSlotName(FString()).IsEmpty()` while `FStratMatchConfig::SaveSlotName`
defaults to `TEXT("StratocracyMatch")` -- THE PLAYER'S SLOT, never empty -- so the guard
protected nothing in the one case its own comment named. `StratAiMatchClauses.cpp` sets no slot
and plays an AI match to an asserted result, so EVERY SUITE RUN WROTE THE PLAYER'S SLOT, and
the change inverted its own purpose: it landed to stop the guided opening re-arming, and as
written it PERMANENTLY SUPPRESSED the guided opening after one suite run, the flag persisting
in the slot across runs with no diff to blame. IT HAD ALREADY HAPPENED, and an accidental
two-directional control proved it rather than an argument: the integration tree ran the suite
WITHOUT the writer and its `Saved/SaveGames/` held zero files; slot-1 ran it WITH the writer and
held `StratocracyMatch.sav` at 2096 bytes. Nobody designed that experiment.
THE FIX WAS THE PREDICATE, NOT THE FIXTURE: emptiness cannot distinguish UNSET from CHOSEN, so
a slot name answers WHERE and can never answer WHETHER. `FStratMatchConfig::
bRecordCompletionOnMatchEnd` now gates `NoteMatchResultIfEnded` -- the one writer nobody asked
for -- and defaults FALSE in C++, so fixtures NOBODY HAS WRITTEN YET inherit silence.
`RecordMatchCompletionOnSave` and `SaveMatchToSlot` stay ungated, deliberately: a caller that
named a slot has already chosen, and that partition is what keeps the direct-writer clauses
green as controls for an absence. `TOptional<FString>` was rejected on a measured constraint --
not reflectable as a `UPROPERTY`, and this struct reaches a designer through a details panel.
The suite **was 140/140** at that pass, 133 -> 140 by set-difference on
`IMPLEMENT_SIMPLE_AUTOMATION_TEST`, +7 and none removed, re-derived independently, zero
non-Success. Build `Result: Succeeded`. THE CHECK THAT ACTUALLY PINS THIS is stronger than any
clause and is run by hand: after a full suite run `Saved/SaveGames/` holds ZERO files where the
blocked pass left the player's slot. `TheCompletionHookIsSilentWithoutTheOptIn` now carries
that property into the suite, with its positive control INSIDE THE SAME CLAUSE rather than
borrowed from a neighbour -- a borrowed control is an assumption about another clause's health.
A DEBT SHIPS WITH THIS AND IT IS A LIVE DEFECT, NOT A HYPOTHETICAL: the packaged game now
records nothing unless `BP_StratGameMode`'s `MatchConfig` carries the new flag, which no C++
asserts and no clause yet reads. The failure mode moved from CORRUPTING the player's slot to
FORGETTING the player's history, which §2.11.6 prefers, but forgetting is still wrong.
`T-UI-03.TheShippedGameModeOptsIn` is OWED, recorded in `tests.md`, and deliberately unwritten
because it would be red until an editor pass that cannot happen before this C++ merges.
Not yet merged and not yet re-gated at this writing. The entry that follows is preserved
verbatim: THE §2.11.6 MATCH-COMPLETION WRITER LANDED, and the clauses that can see
it. `UStratMatchSubsystem::RecordMatchCompletionOnSave` is called from a private
`NoteMatchResultIfEnded` at the tail of `ApplyView`, gated on `FStratMatchView::bHasResult`,
with a `bMatchResultRecorded` latch cleared in `StartMatchInternal`. It closes a defect where
`bHasCompletedAMatch` had a READER AND NO WRITER, so the guided opening re-armed on every match
instead of only the first -- measured four times independently before it was fixed. The hook
hangs off `ApplyView` rather than the submit path because §2.9's AI ends a match through
`RunAiTurnsNow` -> `RefreshPresentation`, which a submit-path hook would miss. The suite
**was 138/138** at that pass, +5 clauses and none removed, counted by set-difference on
`IMPLEMENT_SIMPLE_AUTOMATION_TEST` and re-derived independently by the coordinator, with zero
non-Success. Build `Result: Succeeded`. Four of the five clauses pin the WRITER without ever
planting the field, driving a real AI-vs-AI result; the fifth turns the marked Infantry's
attack unreachability from prose inference into a MEASUREMENT, carrying its own control that
shows `AttackTargetHexes` returning non-empty for a unit in contact -- an empty answer proves
nothing until the instrument is shown able to speak. A HAZARD THIS WRITER CREATES IS RECORDED
RATHER THAN LEFT TO BE DISCOVERED: now that a completed match writes to the slot its config
names, ANY fixture that plays to a result is a writer, and a fixture that reached a result on
the slot the guided-opening clauses need ABSENT would have disarmed that whole file silently
and greenly, on that run and every run after. Not yet merged, not yet re-gated at this
writing. The Lane B entry that follows is preserved verbatim: LANE B LANDED -- save-slot I/O and the §2.11.6 guided opening, built in
the `slot-1` worktree on `feat/save-and-guidance` off `870c611` and merged to `master`.
`FStratBridge::RestoreFromSaveText` replays a save onto a COPY and verifies `canonicalStateHash`
before adopting, refusing a mutated hash rather than restoring it; `UStratSaveGame` carries the
slot; `SaveMatchToSlot` and `LoadMatchFromSlot` both route through an extracted
`StartMatchInternal`, so exactly one `LoadDefinitions -> LoadScenarioFromFile -> restore ->
AdoptBridge` order survives instead of two that could drift apart; `FStratGuidedOpening` is the
first shipping caller of `SetLockedThisTurn`. The suite **was 133/133** at that pass, +25 clauses and none
removed, counted by set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST` rather than by
acceptance-ID grep, and zero non-Success. Build `Result: Succeeded`. Reviewer `VERDICT: PASS`
with zero findings on the branch. The narrow re-gate of the merged tree plus the three
uncommitted record files then also returned `VERDICT: PASS`, zero findings, 2026-08-21 --
twelve checks, with the vendored bytes compared by git blob hash over 32 files, zero
mismatches, rather than by a text diff. WHEN THIS SENTENCE WAS FIRST WRITTEN IT ASSERTED
THAT RE-GATE IN THE PAST TENSE BEFORE IT HAD RUN. The re-gate itself caught that as a
non-gating observation and it is amended here rather than quietly smoothed. It happened to
come out PASS, which is why it was worth catching at all: a pre-declared gate outcome is
unfalsifiable at the moment of writing, whichever way it later lands. Beat 2 of the
guided opening retires ONLY on a capture pip at `guidedOpening.objective`, following the
2026-08-21 USER RULING and not a reading of §2.11.6, which stays ambiguous -- see
`decisions.md`. TWO ACCEPTANCE IDS WERE REASSIGNED AND NONE MINTED: the guided-opening lookup
moves from `T-SCN-02` to `T-SCN-07`, because its subject is the two authored `guidedOpening`
fields and every guided-opening refusal in `Source/StratRules/Scenario.good.cpp` carries
`T-SCN-07` and no other id; and the Q27 input gates are filed under `T-UI-02`, on the precedent
`T-UI-02.ControllerTicksSoInputDispatches` and `T-UI-02.BoardHexRoundTrip` already set in this
tree for the StratPlay input surface. THE SECOND ONE IS A PARTIAL FIT AND IS RECORDED AS ONE:
`T-UI-02`'s written invariant is the reachable-hex highlight, and the GDD ships no acceptance id
at all for §2.11.6-B's one input-gating constraint -- there is no onboarding family among
T-AI/CAP/COMBAT/DATA/FAME/HEX/INT/MOVE/REPAIR/SAVE/SCN/TURN/UI, enumerated over the GDD rather
than recalled. The missing id is FILED UPSTREAM, not minted here. The 2026-08-20 entry that
follows is preserved verbatim: TOOLING ONLY -- no C++, no editor, and no suite run: three pass-on-failure
sites in this project's own pre-commit hook, committed at `95d955b` and `ebbe20d`. Nothing in that
pass re-measures the suite, so every suite figure in this banner still describes `185e88f` and is
unchanged. See "Pre-commit hook" at the end of this file. The 2026-08-19 entry that follows is
preserved verbatim: post-milestone work: the `chooseBuild` cap RULED and FILED upstream at
crew `85995b8`; the content-independence SCENARIO axis FILED upstream against the same crew HEAD
as a change request in `spec/scenario_spec.md` — filing is not closing, and that axis is STILL
OPEN; ten rotted line-number citations replaced by function-and-branch citations; the
content-independence corpus DISCHARGED on two of three axes at `185e88f`, gated `VERDICT: BLOCK`
then `VERDICT: PASS` after this banner itself was found contradicting the entry below it — the
defect that produced `strat_banner_sweep.py`. Suite 107 -> 108. The milestone narrative that
follows is unchanged and still describes 2026-08-14: log-backed combat outcome milestone: phase 1 CLOSED — the `STRAT-COMBAT`
emitter on `FStratBridge::Submit`, gated three times, zero findings, plus an unplanned 1-in-4 test
flake in a `GLog` capture found and fixed on source-level proof, not a probe; phase 2 CLOSED,
`VERDICT: PASS`, zero findings — six new clauses in `StratCombatOutcomeParity.cpp`, suite 87 → 93,
including the first-ever execution of the `adied=1` counter-kill arm; phase 3 CLOSED
MET-AS-CORRECTED, gated THREE TIMES: first two `VERDICT: BLOCK` (a fixture-count miscount in this
file; then this file's OWN mid-file summary line still claiming "no reviewer verdict yet" after
this same file's phase-3 entry already recorded the first `BLOCK` — the other half of the exact
mistake phase D's entry already documents paying for once), both fixed in place; third
`VERDICT: PASS`, zero findings, 2026-08-14 — the brief's `STRAT-CMD accepted` ↔ `STRAT-COMBAT
resolved` pairing is unreachable in an
AI-vs-AI match (measured 0), the real pairing is `STRAT-AI applied kind=Attack` ↔ `STRAT-COMBAT
resolved` by ordered identity, proven 68/68 with zero mismatches by
`Tools/architect/strat_combat_pairing_gate.py`, suite 93 → 103; phase 4 CLOSED MET, gated once,
`VERDICT: PASS`, zero findings, 2026-08-14 — the only phase this milestone that did not need a
re-gate — the live PIE path (`Saved/Logs/Stratocracy_2.log`, `--pre-sliced` mode added to the
gate for its first non-automation corpus), same gate PASS, 68/68 again, and all eleven
`STRAT-AI turn-ended` hashes byte-identical to phase 3's headless run — the phase-D "avoid
one-corpus proof" precedent is now PARTLY discharged: the HOST-independence half IS discharged,
the CONTENT-independence half is NOT and is carried forward as its own future work **[BANNER
CORRECTED 2026-08-19: the content half is now discharged on two of its three axes at `185e88f` —
first side and buildlist moved, a second corpus graded 42/42 with zero mismatches; the SCENARIO
axis alone remains open. This clause described the state at phase 4 and is corrected here rather
than deleted]**; phase 5
CLOSED (editor closed throughout), the doc pass, gated three times, all `VERDICT: PASS`, zero
findings each — 9 files, 231 insertions/23 deletions vs `HEAD` `ae2f22a`, proven comment-only
(254 changed lines, zero fail the comment-body shape test; `/*`/`*/` counts identical HEAD-vs-
worktree in all nine files), suite unmoved at 103/103, and the standing finding that
`strat::chooseBuild`'s cheapest-affordable selection makes a buildlist's repeated entries
inert — a vendored-behaviour question for `stratocracy-crew`, not a defect here. **THE
LOG-BACKED COMBAT OUTCOME MILESTONE IS NOW COMPLETE — five phases closed, no phase 6 was ever
planned for it.** See "Log-backed combat outcome milestone" below. **POST-MILESTONE, 2026-08-14,
NOT A PHASE:** `a2d370a` discharged the deferred `ESearchCase::CaseSensitive` tightening on the
T-SAVE-05 grep-contract clause and opened a narrower `STRAT-CMD refused`-shape residual in its
place; suite is now **104/104** (was 103/103 as of phase 5, above). See "Grep-contract
case-sensitivity tightening" below. **ALSO POST-MILESTONE, 2026-08-14, NOT A PHASE:** the
`--pre-sliced` zero-event gate debt (phase 5's item 4, carried in NEXT) is DISCHARGED —
`Tools/architect/strat_combat_pairing_gate.py` now refuses a zero-applied/zero-resolved
`--pre-sliced` corpus by default and accepts an explicit `--expect-min-pairs N` floor (any N,
including 0, as an opt-out of the default); a 14th in-script self-test case proves the guard is
falsifiable, and a new 11th checked-in fixture (`fixtures/fail_pre_sliced_zero_events.log`, empty)
demonstrates it against the actual defect. This is a Python-only change — no `IMPLEMENT_SIMPLE_
AUTOMATION_TEST` clause, so the 104/104 C++ suite count is unmoved. See "Pre-sliced zero-event
guard" below. **ALSO POST-MILESTONE, 2026-08-14, NOT A PHASE:** the `STRAT-CMD refused`
grep-contract residual that the previous item opened is now DISCHARGED — two new clauses,
`T-SAVE-05.StratCmdRefusedLineShape` and `T-SAVE-05.GrepContractRejectsARefusedCaseVariant`,
close it the same way the `accepted` side was closed; suite **was 106/106** at that pass (**was 104/104**).
See "STRAT-CMD refused grep-contract residual" below. **ALSO POST-MILESTONE, 2026-08-14, NOT A
PHASE, this steward's own lane:** the fixture-verdict binding debt (open since phase 3, carried
through the pre-sliced zero-event guard entry and the grep-contract residual entry above, both
of which left it untouched) is now DISCHARGED. `Tools/architect/strat_fixture_verdict_binding.py`
(new) re-runnably binds all 11 checked-in `.log` fixtures under `evidence/08-combat-pairing-gate/
fixtures/` to their expected verdict AND their exact invocation, and is shown falsifiable by two
deliberate breaks, both reverted. `fixtures/README.md` (new) documents the `--test-path` /
`--pre-sliced` split in the fixtures directory itself, closing the second, smaller item. This
pass also found and fixed staleness in two checked-in artifacts the debt predicted would go
stale silently: `gate_self_test_output.txt` (10 entries, missing
`fail_pre_sliced_zero_events.log` — confirmed by `grep -c "^== "` against `git ls-files`'s 11)
and `self_test_internal_fixtures_output.txt` (11 entries against the in-script `--self-test`'s
actual 14), both regenerated from a real run; no Python or C++ suite count changes (Python-and-
doc-only work). See "Fixture-verdict binding" below. **ALSO POST-MILESTONE, 2026-08-14, NOT A
PHASE, this steward's own lane — three corrections, all found by the wave-1 reviewer gate as
non-gating observations:** (1) this header's own suite count was stale at 106/106; re-derived
directly against `Saved/AutomationReport/index.json` (read `utf-8-sig`, `reportCreatedOn
2026.08.14-21.47.35`, `succeeded 107 / succeededWithWarnings 0 / failed 0 / notRun 0`, 107
entries, zero non-Success) and corroborated by an independent macro census
(`IMPLEMENT_SIMPLE_AUTOMATION_TEST` sums to 107, `IMPLEMENT_COMPLEX_AUTOMATION_TEST`/`_CLASS`
sums to 0) — suite **was 107/107** at that pass (**was 106/106**) and **was 108/108** as of
`185e88f`, the content-independence clause, SUPERSEDED 2026-08-21 by Lane B and stamped
rather than deleted, per this file's own convention — re-derived both ways, `succeeded 108 / failed 0 /
notRun 0` in `Saved/AutomationReport/index.json` and an independent macro census summing to 108
with COMPLEX at 0. (2) A correction bullet this steward wrote
in the prior pass asserted `T-INT-05.AlreadyActedGuardFiresOnAForeignModel` was still open; it is
not — `Source/StratPlay/Tests/StratSelectionForeignModelClauses.cpp` (untracked) ran green in the
same report above, having landed mid-wave, after that pass. Corrections appended in place at all
three prior mentions (the "Four items carried out of..." NEXT bullet, the "STRAT-CMD refused
grep-contract residual" section, and the "Fixture-verdict binding" section) rather than rewritten
— each was true when written. The other two items those same sentences named,
`chooseBuild`'s buildlist-ratio question and the content-independence corpus, have moved
differently and are kept distinct: `chooseBuild` is now investigated and written up at
`Tools/architect/evidence/upstream-chooseBuild-buildlist-ratio.md` (a request draft awaiting a
Director ruling, **not discharged**); content-independence remains genuinely open. **[STAMPED 2026-08-19 — BOTH halves of this sentence have since moved, and it is stamped rather than rewritten because it was true when written: `chooseBuild` is RULED (per-type population cap) and FILED upstream at crew `85995b8`; content-independence is DISCHARGED on two of its three axes at `185e88f`, the scenario axis alone still open. See the two NEXT bullets dated 2026-08-19.]** (3) The
fixture-count derivation command recorded in this file and in
`evidence/08-combat-pairing-gate/blackboard.md` — `git ls-files fixtures/ | wc -l` — measures
tracked files in the directory, not `.log` fixtures; it read 11 only because this pass's own
`fixtures/README.md` was still untracked, and would read 12 the moment that file is committed
while the `.log` count stays 11. Both prose copies corrected to `git ls-files
'.../fixtures/*.log' | wc -l`, which reads **11** both now and after a hypothetical commit of the
README (verified: `git ls-files --cached --others --exclude-standard '.../fixtures/*.log'` — the
proxy for post-commit tracked state — still returns 11, while the same query without the `*.log`
glob returns 12). See "Fixture-verdict binding" below for the full account. The prior
entry (AI-opponent milestone, phase D CLOSED, COMPLETE) is preserved under "AI-opponent milestone"
further down. **ALSO 2026-08-20, TOOLING ONLY, THIS STEWARD'S OWN LANE -- and it is the
reason this banner now lives in a different file:** the 3,524-line `state.md` is SPLIT into
`Tools/architect/state/`, one file per owning agent, and `state.md` is frozen as the closed-phase
history. The sweep that polices the record now DERIVES its file list from that directory instead of
naming `state.md`, compares live claims ACROSS the set rather than file by file, and enforces that
only this file may carry a live suite count. Two defects were fixed in it and both were shown
falsifiable against a control running HEAD's own bytes: a suite claim ending in a full stop was
invisible to its regex, and the pre-commit hook's typed path meant a commit touching only
`state/tests.md` drew ZERO bytes of output and exit 0. `install.sh` no longer dies in a linked
worktree, and the `strat-hotseat` skill's claim that a new worktree is unprotected is corrected --
it is protected, through the shared common directory. `.claude/skills/strat-parallel/SKILL.md` is
new and carries the worktree topology, the seven concurrency rules, per-tree command templates that
derive every path from that tree's own `git rev-parse --show-toplevel`, and the merge protocol.
**NO SUITE RUN AND NO C++ IN THIS PASS** -- every suite figure above still describes `185e88f` and
is unchanged; the only build was a re-verification that the `slot-1` worktree still compiles
(`Result: Succeeded`, `REAL_EXIT=0`, 82 of 82 actions, editor closed).)_

## BUILT

- `Source/StratUI/StratScoreboardHUD.h` — applied (iteration 1, step `host_h`, live)
- `Source/StratUI/StratScoreboardHUD.cpp` — applied (iteration 1, step `host_cpp`, live)
- `FStratBridge::Reachable` — landed at `e0cc53d` with zero tests; its five clauses are now
  covered (`StratBridgeQueryParity.cpp`, T-UI-02, phase 1, 2026-08-12). Debt discharged.
## NEXT

- **2026-08-21, COORDINATOR, LIVE PIE ON THE INTEGRATION TREE, NO SOURCE AND NO ASSET CHANGE --
  THE STRIP DRAWS, AND IT DRAWS BLANK AND UNCONDITIONALLY.** The three things the previous entry
  said its evidence did not reach are now measured, and measuring them opened a defect that no
  clause can see. Method: PIE from the game's own startup path (log: `Game class is
  'BP_StratGameMode_C'`, then `Guided opening armed for side 0: objective hex (2, 7), window turns
  1-4`), captured with `Shot showui` -- the only capture route that composites the UMG layer.
  - **What is now observed.** A solid rectangle at x 750-805, y 157-194 in a 2544x1320 frame whose
    viewport centre measures x 778; the box centre is 777.5. So (a) the Slate hierarchy DOES lay
    out at top centre, (b) `AddToViewport` DOES yield a visible widget for this class, and (c) it
    DOES paint on a real frame. Those were the three open unknowns.
  - **Identity was established by REMOVAL, not by geometry.** `ke * RemoveFromParent` through the
    PIE console removed every game widget (22 instances, the live strip's `Root` named in the log)
    and the box went to ZERO pixels in the same frame the scoreboard did -- the scoreboard being
    the positive control that the command took. Three cheaper measurements agreed and none of them
    would have been sufficient: only one live `Border` existed in the world and it was
    `StripBorder`; the box width is exactly the border's own `Left`+`Right` padding (28+28); and
    the composited colour matches `BrushColor` over the sky within 1/255 on all three channels.
    A targeted `ke <InstanceName> RemoveFromParent` reported SUCCESS and changed nothing, because
    it resolved a same-named object under a different outer -- one contradicting result against
    three agreeing ones, and it was the one that was wrong.
  - **NO DIRECTIVE TEXT EVER APPEARED, in any frame.** Six captures across four different model
    states are pixel-identical: the same 2128-pixel rectangle, which is padding alone with
    zero-width content. §2.11.6-B's line has still never been on screen.
  - **The text bindings ARE evaluating.** `GetAll TextBlock Text NAME=DirectiveText` reports the
    LIVE instance still holding the design-time `"Text Block"` while the screen shows nothing: a
    UMG property binding drives the Slate attribute and never writes the UPROPERTY back. Read
    `GetAll` for unbound state only -- for painted state it is the wrong instrument.
  - **THE FINDING. The border paints while the widget's own guidance is all default.** `GetAll
    StratGuidanceWidget Guidance` read the live copy as `bActive=False, Beat=None,
    DirectiveText=""`. `GetStripVisibility` is `ToVisibility(Active)` whose False pin default reads
    `Collapsed` off the graph, so a false `bActive` must COLLAPSE the border. It did not. The
    compiled `Bindings` array on the generated class carries all four entries with the right
    `ObjectName`/`PropertyName`/`FunctionName`, `StripBorder`/`Visibility`/`GetStripVisibility`
    among them. So the strip is drawn whenever the HUD creates it, guidance or no guidance, and
    **no push has been observed to reach the widget at all** -- the paint is not evidence of one.
  - **SECOND DEFECT, measured directly in a SECOND, FRESH PIE session rather than inferred from the
    first: the widget's guidance is still all default two seconds after the opening arms.** The log
    line `Guided opening armed for side 0: objective hex (2, 7), window turns 1-4` had already been
    written when `GetAll StratGuidanceWidget Guidance` read `bActive=False, Beat=None,
    DirectiveText=""` off the live instance, and the same session's `GetAll StratScoreboardHUD
    GuidanceStrip` shows the HUD holding a pointer to THAT instance -- so the widget the HUD
    created is the widget being read, and nothing has been pushed into it. `ApplyView`'s
    `HUD->PushGuidance(Model.Guidance)` is guarded only by `FindScoreboardHUD()`, and
    `AStratScoreboardHUD::PushGuidance` is guarded only by `GuidanceStrip != nullptr`, which is
    demonstrably non-null. One of those two links does not run. That is a C++ question, answerable
    in one PIE session with the console route, and it is separate from the visibility-binding
    question above: EITHER defect alone would leave the strip blank, and both are live.
  - **Driven both directions with no simulated input.** The guided opening was skipped on the model
    side and PROVED skipped: a second `ke * SkipGuidance` reported 1 instance succeeded while
    printing no "skipped by the player" line, that log sitting after the `!bActive` early return.
    An explicit `ke * RefreshFromMachine` reconcile also succeeded on the live controller. The
    screen did not move for either. `playtest_key`/`playtest_click` remain unable to drive
    anything; `ke` on a `BlueprintCallable` entry point is a route that does not need them.
  - **Consequence: §2.11.6-B IS NOT CLOSED and must still not be claimed.** What remains is a
    C++/asset question in the engineer's and editor's lanes -- why a bound `Visibility` does not
    take while bound `Text` does, and whether `ApplyView` -> `FindScoreboardHUD` -> `PushGuidance`
    ever reaches this instance. Neither is answerable from the record; both are answerable in one
    PIE session now that the console route exists.

- **A claim in a document is checkable against an artifact, and the artifact wins — this has now
  cost six corrections across four separate occasions in this milestone alone.** (Corrected count:
  the earlier "four separate corrections" undercounted its own tally by conflating occasions with
  corrections — the fourth occasion below is itself three miscounts across two files plus one
  propagation into this file, so six corrections total, on four occasions.) Phase D's gate-half
  produced three prose errors caught by the reviewer's gate on the first occasion: two technical
  (misreading UE 5.8's `AddExpectedMessagePlain(Occurrences=0)` semantics; overstating what the
  `:610` tripwire's substring match actually pins down), one pure bookkeeping (this file and the
  phase evidence blackboard both claiming "no reviewer verdict exists yet" a hundred-plus lines
  after a `BLOCK` verdict on the same piece was already recorded in the same file) — caught
  respectively by checking the code against the claim and by reading the document's own history. A
  fourth occasion, same class, different artifact: `pie-session/narrative.md` and
  `pie-screenshots/narrative.md` hand-transcribed `STRAT-AI applied` command counts (a turn-3 span
  and count, a turn-5 Build/Move/Attack breakdown, a session-2 Move count sourced from "the brief"
  rather than the log printed three lines above the claim) that a `grep -c` against the checked-in
  slice — the artifact sitting in the same directory — falsifies; propagated once into `state.md`
  before the reviewer's re-gate caught it (2026-08-13, `pie-session`/`pie-screenshots`
  `narrative.md` BLOCK findings) — three miscounts plus the one propagation, six corrections in
  total across the four occasions. Where a narrative states a count a one-line command counts
  exactly, derive it and name the command, rather than reading it off a log by eye or trusting a
  dispatching agent's number over the artifact in hand — this applies to counts, to "has a verdict
  run yet" bookkeeping, and to slice boundaries alike; a document is never its own authority over
  the tree or the log it describes.
- `presentation_statelessness` — Presentation statelessness pass (rebuild widgets from the view model) (actionable)
- `production_widget` — Production menu widget (§2.11.5) (blocked on buildlist_query)
- `bridge_event_list` — Bridge ordered event list (§4.9 'command in / events out') (actionable)
- `buildlist_query` — Buildlist query on the Ui.h contract (actionable, excluded: shape unstated in the GDD by explicit decision, and the file is vendored certified bytes in another repo -- T-INT-01 hash-matches it)
- **Hot-seat milestone is COMPLETE (phase 6 closed 2026-08-13); see "Hot-seat milestone —
  COMPLETE" below.** The AI-opponent milestone is now current (phase C closed 2026-08-13; phase D
  — PIE playtest and a machine-repeatable AI-vs-AI gate — is next; see "## AI-opponent milestone"
  below). Remaining out of scope, unchanged: production menu (§2.11.5), guided opening (§2.11.6),
  info panel, toasts, save-slot UI, move-undo.
- **CORRECTED, 2026-08-14 — this bullet's own claim had itself gone stale and was repeated
  uncritically.** It previously asserted "`.agents/ue-project-context.md:195` reads '51/51,
  hot-seat phase 3'", true only at phase 4. The line has been rewritten by later phases since; a
  direct read on 2026-08-14, at the start of the combat-outcome milestone, found it instead reading
  "Last observed here: 86/86, AI-opponent phase B, 2026-08-13" — not the phase-4 text this bullet
  claimed — and that number was itself carried into a dispatch prompt before being checked, the
  same class of error this bullet exists to warn against. The `EnhancedInput` module-arrow gap this
  bullet also raised has not been independently re-checked and is left as an open question for
  `.agents/`'s owner. That file remains outside this steward's lane
  (`.agents/` is not `Config/` or `Tools/architect/`); the line-195 count is now current — see the
  new milestone's header above and `.agents/ue-project-context.md:195` itself, which as of
  2026-08-14 reads "93/93, combat-outcome phase 2, 2026-08-14" (updated by the user, not this
  steward).
- **Phase 6 risk, and the most consequential open item: the `STRAT-CMD` click-to-command gate
  is unclosed.** No `playtest_key` or `playtest_click` produced any `LogStratPlay` output.
  **Correction to this record, made in phase 6:** the original claim here — "total input
  silence, reproduced on Epic's own shipped TopDown template with known-good assets" — had a
  confounded control and is **not established**. The "TopDown template" the control actually
  drove was `Lvl_FerrumCrossing` launched under the TopDown template's GameMode (a consequence
  of the stale-config bug found in phase 6 — see below), and `Lvl_FerrumCrossing`'s `Floor`
  actor had been deleted in phase 5 (recorded above, "The template's `Floor` actor was deleted
  from `Lvl_FerrumCrossing`"). A click on that map hits nothing under the cursor, so
  `CharMoveComp.Velocity` legitimately stayed zero for a reason having nothing to do with
  whether input reaches the game. `Escape` still ending PIE is real and unaffected by this
  correction. The underlying question — does simulated input reach `StratPlayerController` at
  all — is **still open**, restated accurately with new, still-confounded-a-different-way
  measurements under Phase 6 below. Phase 4's standing debt — "`ETriggerEvent::Started` on all
  four input actions is asserted, not measured" — remains open, and phase 6's
  `assert_log_contains` gate still depends on solving it. **Do not action the phase-6 escalation
  that asks to repoint `Lvl_FerrumCrossing`'s GameMode or `GlobalDefaultGameMode` again** — see
  Phase 6 below: the running editor was serving a stale, pre-flip config, and the on-disk
  `Config/DefaultEngine.ini` needs no change.
- **Four items carried out of the log-backed combat outcome milestone's phase 5 (now CLOSED,
  milestone COMPLETE) — none owned by this steward, none built in that doc pass by design:**
  1. `Stratocracy.StratPlay.T-INT-05.AlreadyActedGuardFiresOnAForeignModel` — pin that
     `FStratSelectionMachine` refuses an enemy click when the selected unit has
     `bHasActed == true` while the machine's own `DoneUnits` is empty (a machine built against a
     match already in progress). Proposed by `strat-gameplay-engineer`, correctly not written in
     a doc pass. Owner: `strat-test-author`.
     - **CORRECTION, 2026-08-14, this steward's own lane, later pass — DISCHARGED, landed mid-wave
       after this entry was written.** This item was correctly open when written. It is not open
       now: `Source/StratPlay/Tests/StratSelectionForeignModelClauses.cpp` is in the tree
       (untracked at time of writing — `git status --porcelain` lists it `??`), and
       `Stratocracy.StratPlay.T-INT-05.AlreadyActedGuardFiresOnAForeignModel` reads `state:
       "Success"` in `Saved/AutomationReport/index.json` (read `utf-8-sig`, `reportCreatedOn
       2026.08.14-21.47.35`). Suite is now **107/107** (`succeeded 107 / succeededWithWarnings 0 /
       failed 0 / notRun 0`, 107 entries, zero non-Success — re-derived directly, not taken from a
       report or a dispatch number), corroborated independently by
       `grep -rc IMPLEMENT_SIMPLE_AUTOMATION_TEST Source --include=*.cpp` summing to **107** and
       `IMPLEMENT_COMPLEX_AUTOMATION_TEST`/`_CLASS` summing to **0**. The other three items this
       same numbered list names — the `chooseBuild` buildlist-ratio question and the
       content-independence corpus — have NOT moved the same way: `chooseBuild` is now
       investigated and written up at
       `Tools/architect/evidence/upstream-chooseBuild-buildlist-ratio.md`, a request draft
       awaiting a Director ruling, **not discharged**; content-independence remains genuinely open,
       untouched. These three items are three different states, not one word. **[STAMPED 2026-08-19 — BOTH halves of this sentence have since moved, and it is stamped rather than rewritten because it was true when written: `chooseBuild` is RULED (per-type population cap) and FILED upstream at crew `85995b8`; content-independence is DISCHARGED on two of its three axes at `185e88f`, the scenario axis alone still open. See the two NEXT bullets dated 2026-08-19.]**
  2. **DISCHARGED, `a2d370a`, 2026-08-14 — see "Grep-contract case-sensitivity tightening"
     below.** Tighten `Source/StratPlay/Tests/StratHotSeatReplayParity.cpp`'s shape `TestTrue`
     (`Line.StartsWith(TEXT("STRAT-CMD accepted "))`) to `ESearchCase::CaseSensitive`. Residual
     as originally recorded: a case-only change to the `STRAT-CMD accepted` emitter would slip
     past today — confirmed by re-reading the assertion directly; it currently relies on a
     whole-tree case-variant census (also re-confirmed this phase, exactly two lowercase hits,
     both illustrative, in the test files themselves) rather than on its own shape. Deliberately
     not done in phase 5: retightening an assertion is a code change, not a doc change. Owner: a
     code phase. **A new, narrower residual on the `refused` shape replaces this one — see NEXT
     below and the new section.**
  3. A line-number citation living inside a `TestEqual` message string in
     `Source/StratPlay/Tests/StratSelectionWaitClauses.cpp`
     (`StratSelectionMachine.cpp:156-160`). Currently accurate, but unreachable by a doc pass
     because it sits inside an assertion string rather than a comment. Same owner class as item 2.
     - **DISCHARGED 2026-08-19 — and the file held SEVEN citations, not one, one of which had
       already rotted.** The item named the two in-string ones; a grep for
       `StratSelectionMachine.cpp:` in that file returned seven sites (two inside assertion
       message strings, five in comments), all replaced with function-and-branch citations.
       **`:390`, cited twice as `NotifyCommandApplied`'s `DoneUnits.Add`, was already wrong when
       this pass opened it** — that add is at `411`, and `390` now lands on a `break;` in
       `HandleEvent`'s Move arm, a different function. Re-derived, not assumed:
       `grep -n "DoneUnits.Add(Outcome.UnitId)"` → 411; `sed -n '390p'` → `break;`. The other
       five (`:156-160`, `:158`, `:168`) were still accurate and were replaced anyway, which is
       the point — accuracy today is not the property, unreachability by the next diff is.
       Rebuilt (`Result: Succeeded`, one TU recompiled) and re-run headless: suite **107/107**,
       `succeeded 107 / succeededWithWarnings 0 / failed 0 / notRun 0`, 107 entries, zero
       non-Success, `reportCreatedOn 2026.08.19-17.49.48`; both wait clauses `Success`. Count
       unmoved, as expected for a comment-and-message-string change.
     - **NOT closed by this, and measured rather than guessed: four more line-number citations
       survive in two other files**, outside the file this item named. `StratBridge.cpp:448`
       cites `StratSelectionMachine.cpp:571-575` — checked, still accurate (the submit `switch`).
       `StratHotSeatReplayParity.cpp` carries three, and **two of them have already rotted**:
       `:255-287` (cited twice, once INSIDE an assertion message string at its ATTACK clause)
       lands on the selection arm, not the attack branch — the real enemy-click arm runs from
       roughly the `bHasActed` guard to the `Command = Attack` write; and `:350-356`, cited for
       "an attack ends the unit's turn", lands on the Move arm's "already moved this turn"
       refusal -- that one is `NotifyCommandApplied`'s `Attack` arm, the same add the wait file
       cites. A fifth, `StratSelectionMachine.cpp:519`'s `Save.good.cpp:294-300`, cites a
       VENDORED file by line and is a different problem again.
     - **The three `StratHotSeatReplayParity.cpp` citations were then fixed too, on the user's
       instruction, in the same pass** -- both rotted ones and the one in-string site among them,
       all replaced with the branch names above. `grep -rn "StratSelectionMachine.cpp:"
       Source/StratPlay/Tests/` now returns nothing: the Tests lane carries no line-number
       citation into the machine at all. Rebuilt and re-run: `Result: Succeeded`, suite
       **107/107**, `succeeded 107 / succeededWithWarnings 0 / failed 0 / notRun 0`, 107 entries,
       zero non-Success, `reportCreatedOn 2026.08.19-18.05.58`; `T-UI-01.ClickedAttackIsAccepted
       AndRecorded` -- the clause whose assertion message was edited -- `Success`, as are
       `T-SAVE-05.HotSeatReplayParity` and `T-INT-02.ReplayParityWithHeadless`.
     - **A build trap worth the record: the editor blocks the build, and the first build of this
       pass slipped through anyway.** The second build failed `Result: Failed
       (OtherCompilationError)` on `Unable to build while Live Coding is active` with the editor
       running (PID 51424, started 13:54, i.e. ALREADY RUNNING during the first build, which
       succeeded). So "the editor was open" does not predict the failure on its own -- Live
       Coding has to have engaged -- and an agent that treats a green build as proof the editor
       was closed has it backwards. Verification here waited for the user to close it rather than
       committing on the strength of the edit being "the same class" as one already proven, which
       is not a measurement.
     - **The vendored citation is now fixed too, on the user's instruction -- and measuring it
       turned up 48 more.** `StratSelectionMachine.cpp`'s `CommandKindName` doc block carried
       three line citations, all three now by symbol: `Save.good.cpp:294-300` ->
       `strat::saveCommandName`'s switch; `Save.h:135` / `Save.good.cpp:292` -> declared in
       `Save.h`, defined in `Save.good.cpp`; and `.agents/ue-project-context.md:189-191` -> that
       file's read-only-territory heading. The block now says WHY in as many words: vendored
       bytes move on a re-vendor and nothing in this repository would catch a stale number.
       **`.agents/…:189-191` had already rotted, and this steward rotted it** -- the map ->
       GameMode section committed at `0b70a18` was inserted above it, so the citation now lands
       on that new prose rather than the sharper-clause text it names. Same class as the §2
       staleness this steward's own filing caused in the `chooseBuild` document, on the same day.
       Verified: `Result: Succeeded`; suite **107/107**, zero non-Success, `reportCreatedOn
       2026.08.19-18.28.07`; and the diff proven comment-only, not asserted to be -- every added
       and removed line matches a comment-body shape, non-comment changed lines **0**.
     - **THE REAL SIZE OF THIS, measured rather than estimated: 46 line citations into vendored
       headers survive across 16 files** -- 48 before this pass, less the two the block above
       just fixed. **Both numbers are stated because the command below returns the SECOND one,
       and a note whose figure its own command contradicts is the exact defect this file keeps
       paying for.** Caught by running it after writing the number, not before. Kept on ONE
       line, unwrapped, because a wrapped command is a command nobody can run:

       ```
       grep -rEn "(Save|Ai|Combat|Economy|Move|Turn|Ui|Data|Hex|Driver|Rules)\.(h|good\.cpp|buggy\.cpp):[0-9]" Source/StratPlay Source/StratBridge Source/StratUI Source/Stratocracy | wc -l
       ```

       Run over the four non-vendored `Source/` modules. Concentrated in
       `StratBridge.cpp` (11), `StratUI/StratViewModel.h` (8), `StratBridge.h` (7),
       `StratCombatOutcomeParity.cpp` (4). These are the WORST class of the whole family: a
       re-vendor moves them all at once, silently, and no clause in this repository reads a
       vendored line number to notice. Spot-checked while here: `Save.h:54`'s
       `SaveCommandKind` enum is still accurate. The other 47 were NOT checked one by one and
       are not claimed either way. Unowned -- `StratBridge` and `StratUI` are
       `strat-gameplay-engineer`'s lane, not this steward's, and this is a phase-sized sweep
       rather than a fix to slip into another pass.
     - **Still not fixed, and still measured: one citation outside the Tests lane.**
       `StratBridge.cpp:448`'s `StratSelectionMachine.cpp:571-575` -- accurate today (it names
       the submit `switch`); production code, `strat-gameplay-engineer`'s lane.
  4. **DISCHARGED, 2026-08-14 — see "Pre-sliced zero-event guard" below.** `--pre-sliced` gate
     debt on `Tools/architect/strat_combat_pairing_gate.py`, confirmed untouched by phase 5
     (`git diff --stat ae2f22a -- Tools/architect/` empty at the time). It returned `PASS` and
     exit 0 on an empty or wrong pre-sliced corpus. Shipped both halves of the owed shape — a
     structural default refusal on a zero-event `--pre-sliced` corpus, AND a caller-supplied
     `--expect-min-pairs N` floor, the latter opting out of the former when passed explicitly
     (including `N=0`) — plus a 14th self-test case proving the guard can itself fail.
  Also unchanged and still open, carried again: the content-independence half of the phase-D
  "avoid one-corpus proof" precedent (a different scenario/buildlist/first side, genuinely
  different game content) — the host-independence half was discharged in this milestone's
  phase 4. **[STAMPED 2026-08-19 — DISCHARGED on two of three axes at `185e88f` (first side and buildlist moved; the scenario axis is still open because `Data/` is vendored and holds one scenario). True when written; see the NEXT bullet dated 2026-08-19.]**
- **DISCHARGED, this pass, 2026-08-14 — see "STRAT-CMD refused grep-contract residual" below.**
  The residual as originally opened by `a2d370a`: a case-only change to the `STRAT-CMD refused`
  shape was caught by nothing. Cannot be closed with an expected-message gate — verified against
  `UE_5.8\Engine\Source\Runtime\Core\Public\Misc\AutomationTest.h`:
  `FAutomationExpectedMessage::Matches` uses `FString::Contains` at its `IgnoreCase` default;
  `EAutomationExpectedMessageFlags::Exact` adds only a length equality, and a case variant has
  identical length; both constructors build the pattern with `ERegexPatternFlags::CaseInsensitive`
  hardcoded, not parameterized. Closed instead with `IsGrepContractRefusedLine`, a dedicated
  `ESearchCase::CaseSensitive` predicate, and two new clauses — see the new section for the full
  account, including the falsifiability measurement's stated limit (it exercised the predicate,
  not an edited emitter, so the closure is a sound inference rather than a directly observed one).
