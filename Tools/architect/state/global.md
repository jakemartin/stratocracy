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

_Last run 2026-08-22 (BOTH DELIVERY DEFECTS ARE FIXED AND PINNED, AND THE STRIP HAS NOW BEEN
WATCHED WITH THE FIX IN THE TREE, AND IT STILL PAINTS NO DIRECTIVE. **[HEADLINE AMENDED
2026-08-22 12:01 local, coordinator, after the PIE session this banner asked for was actually
run. It formerly recorded that the strip HAD NOT BEEN SEEN since the fix; it has now been seen,
and the answer is negative. The delivery half of this banner is CONFIRMED ON A LIVE WIDGET -- the
widget's guidance now arrives fully populated where it used to arrive default -- and the painted
strip did not follow it. The newest `## NEXT` entry carries the session.]** Both halves are
load-bearing: the two mechanisms that kept it blank are closed in C++ and held by clauses, and the
screen still shows no directive.
**Sec 2.11.6-B IS NOT CLOSED AND IS NOT CLAIMED HERE.** What closes it is a human-driven playtest; the
coordinator PIE session it also named was RUN on 2026-08-22 and returned NO, so it is discharged
rather than owed -- the 08:01-local `## NEXT` entry carries its console measurements.
**[AMENDED 2026-08-22 11:30 local / 15:30 UTC, coordinator, after a SECOND coordinator PIE session.
Sec 2.11.6-B IS STILL NOT CLOSED and this amendment does not close it.** What changed is one rung
below the section: the design-time sentinel experiment the entry above assigned to
`strat-editor-builder` HAS NOW BEEN RUN -- by the coordinator over the editor's own MCP endpoint,
because the proxy served the two-tool surface again and the builder has no Bash. It answers the
fork rather than the section: the binding IS delivering and the bound function supplies a value
that paints zero-width, with the sentinel confirmed present on the live PIE instance as the
positive control, and the widget asset restored to its gated bytes afterwards. **Do not re-dispatch
that experiment** -- the topmost `## NEXT` entry carries it and names the one question left.
Pointer fixed while here: this sentence used to say "the newest `## NEXT` entry", which silently
re-aimed at whatever was inserted next. **And the `12:01 local` stamp in the headline above is
really 12:01 UTC** -- its own capture, `ScreenShot00059.png`, has mtime `08:01:04 -0400` against a
log line reading `12.01.00`, so that session ran FOUR HOURS EARLIER than its stamp reads and this
11:30-local amendment is after it, not before it. Recorded rather than silently rewritten, because
the same mislabel is what makes two entries look out of order.]
THE SUITE **IS NOW 160/160**, 151 -> 160 by set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST`
walked over `Source/` -- +9, none removed, column-0 anchored, re-derived independently by the gate
and again in the merged tree -- zero non-Success, `notRun` 0, 160 entries, all `Success`. The run is
THIS tree's own: build `Result: Succeeded`, `REAL_EXIT=0` with the editor closed, report
`reportCreatedOn 2026.08.22-04.54.16`, and all nine new clauses present in it by name.
`Saved/SaveGames/` held ZERO files either side and the directory mtime MOVED, 22:46:55 -> 00:54:16,
so the control is discharged on this tree again rather than borrowed.
WHAT THE TWO FIXES ARE. Defect 2 is fixed AT THE BUILD, NOT AT THE PUSH: `ApplyView`'s
unconditional-push ruling is untouched and `BuildViewModelForPresentation` -- `BuildViewModel` plus
one `ExecuteIfBound` on a new plain, single-cast, weak, unreflected `FStratViewDecorator` -- now
completes every model the subsystem builds for the screen, so a model missing its guidance section
no longer exists rather than being tolerated at the last moment. `BuildViewModel` stays undecorated
on purpose, because the AI loop reads models nobody draws and decorating there would advance a beat
for a question with no screen behind it. Defect 1 is fixed WITHOUT ASSERTING ANY `BeginPlay` ORDER:
the HUD records every value it is handed and replays it from `CreateGuidanceWidget` once the strip
is in the viewport.
A RECORDED RULING WAS INTERPRETED RATHER THAN FOLLOWED, DELIBERATELY AND IN THE OPEN.
`CreateGuidanceWidget` said the strip CANNOT be pushed to at creation because the HUD "has no way to
ask for that without becoming a second thing that runs matches". That forbids a PULL, and a cache of
what you were handed is not one -- it never asks, has one writer, and every later push overwrites
it, so it changes WHEN a value is delivered and never WHICH. The argument is written into the header
rather than assumed, the sentence that was actually wrong is retracted in place, and the gate judged
the reading correct rather than a weakening. If a later Director disagrees, the alternative is a
HUD-readiness delegate with a check-then-subscribe idiom, and it is strictly more machinery.
GATES: `VERDICT: PASS` with zero findings on the branch in slot-1 -- twelve standing checks plus
eight directed ones, including that no `.Build.cs` is in the diff at all, so `StratPlay` gained no
`UMG`/`Slate`/`SlateCore` -- and a re-gate on this merged tree afterwards.
WHAT THE EVIDENCE STILL DOES NOT REACH, stated because the last four entries were each corrected for
reaching too far: the controller's `BeginPlay` registration and `EndPlay` release are NOT pinned by
any clause, because a transient world never begins play, so that one line still rests on the PIE
session of 2026-08-21; and the first-AI-turn clearing was never observed, only inferred from the
call graph. Both are recorded in `tests.md` and `engine.md` by their own authors.)

_Last run 2026-08-21 (THREE CLAUSES LANDED AND THE STRIP WAS SEEN TO DRAW BLANK. Two separate
things in one merge, and neither is the other's evidence.
THE SUITE **WAS 151/151** at that pass, 148 -> 151 by set-difference on
`IMPLEMENT_SIMPLE_AUTOMATION_TEST`
walked over `Source/` -- +3, none removed, column-0 anchored, re-derived independently by the gate
in the merged tree (SIMPLE 151, COMPLEX/_CLASS 0) and never incremented from a brief -- zero
non-Success, `notRun` 0, 151 entries, `Counter({'Success': 151})`. Build `Result: Succeeded`,
`REAL_EXIT=0`; report `reportCreatedOn 2026.08.22-02.46.56`.
DATES IN THIS RECORD ARE LOCAL; `reportCreatedOn` AND THE LOG ARE UTC, and this entry was first
written dated A DAY AHEAD because its date was taken off a UTC stamp. Every commit of this pass is
`2026-08-21 -0400`, and the entry below it dates itself 2026-08-21 while quoting a `reportCreatedOn`
already past UTC midnight -- so local is the convention and the day-ahead dating was wrong.
Corrected in place, and stated rather than silently fixed, because the next writer reads a UTC stamp
too. (The sweep has a BANNER DATE FRESHNESS check that compares the banner's date against entry
dates, so an ISO date written in PROSE trips it -- measured here, which is why this paragraph
describes the wrong date instead of spelling it.)
THE RUN IS THIS TREE'S OWN, and it was made rather than argued for. An earlier draft of this banner
backed the figure on `slot-1`'s report plus whole-tree byte identity; the sweep's REPORT IDENTITY
check refused it, correctly -- this tree's report was older than the clause files it would have been
describing -- and the editor was closed so the build and the suite could run here. That guard is the
reason the number in this banner means what it says. `Saved/SaveGames/` held ZERO files either side
and the directory mtime MOVED, 20:21:06 -> 22:46:55: the control discharged on this tree again, not
borrowed.
WHAT LANDED. `AlterOneField` in `StratGuidanceRouteClauses.cpp` was reinterpreting ANY
`FStructProperty` as an `FIntPoint` while promising to refuse kinds it could not vary -- the
going-inert shape the property-walk control exists to prevent, relocated one level down. It now
gates on `TBaseStructure<FIntPoint>::Get()`. And the two shipped-asset facts the last entry named as
an unpinned gap are pinned: `WBP_DirectiveStrip`'s ancestry and `BP_StratScoreboardHUD`'s non-null
`GuidanceWidgetClass`, both read off the CDO, both checking their own premise first. The author's
two mutation builds showing both directions red are a SELF-REPORT; the restored-bytes identity is
verified (`git hash-object` -> `c0b8693dbb7d2d6874b7b59264ba83fa9b75a3e4`), the mutation runs are
not, and the gate said so.
THE STRIP DRAWS, AND WHAT IT DRAWS IS AN EMPTY BOX THAT IGNORES THE MODEL. Landed separately at
`d0d2e2e`; the account and both defects are under `## NEXT` and are not restated here.
**Sec 2.11.6-B IS NOT CLOSED AND IS NOT CLAIMED.**
GATES: `VERDICT: PASS` with zero findings on the branch in slot-1 (twelve checks), and `VERDICT:
PASS` with zero findings on the merged tree afterwards, because a verdict describes a tree and a
merge is a post-pass change. The post-merge gate's one substantive remark was about how to attribute
a lane run; the run above makes that attribution unnecessary rather than merely careful.)

_Last run 2026-08-21 (THE STRIP IS WIRED END TO END AND HAS NEVER DRAWN. Both halves of that
sentence are load-bearing and the second is the one a later reader will otherwise lose.
`WBP_DirectiveStrip` is reparented onto `UStratGuidanceWidget` and
`BP_StratScoreboardHUD.GuidanceWidgetClass` now names it, so the route from
`FStratViewModel::Guidance` to a widget is complete in every link. NOTHING HAS BEEN OBSERVED ON
SCREEN. Authored by the coordinator over the editor's HTTP endpoint under the standing ruling;
gated and PASSED with zero findings.
WHAT THE EVIDENCE REACHES, AND THE THREE THINGS IT DOES NOT. It proves the property path
resolves, the parent class binds, all four `Return Value` pins are `connected`, and the suite
**was 148/148** across the change. It proves NOTHING about (a) whether the strip's Slate hierarchy
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
The suite **was 148/148** at that pass, 141 -> 148 by set-difference on `IMPLEMENT_SIMPLE_AUTOMATION_TEST`
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

- **2026-08-22, COORDINATOR, THE DESIGN-TIME SENTINEL EXPERIMENT WAS RUN AND IT ANSWERS THE FORK
  THE ENTRY BELOW LEFT OPEN: THE BINDING IS DELIVERING, AND THE BOUND FUNCTION SUPPLIES A VALUE
  THAT PAINTS ZERO-WIDTH. Sec 2.11.6-B STILL DOES NOT CLOSE, and nothing here claims it does --
  what closes that section is a human-driven playtest, unchanged, and the strip still paints no
  directive.** The entry below assigned this experiment to `strat-editor-builder` and it was run by
  the coordinator instead, for the reason that entry already recorded: `ToolSearch` for
  `execute_script` returned nothing while the same keyword search returned `unreal_status` and
  `list_unreal_projects`, so the builder -- NeoStack tools plus Read/Grep/Glob/Skill and no Bash --
  had no route to the editor. Driven over the editor's own MCP endpoint with `Tools/neostack/ns.py`.
  - **THE EDITOR WAS NEVER THE PROBLEM, AND A RESTART WAS PROPOSED AND REFUSED ON MEASUREMENT.**
    `unreal_status` reported *"No active NeoStackAI editors were found"* while `UnrealEditor.exe`
    was alive, `LISTENING` on 9315 by `netstat`, carrying a heartbeat in `runtimes.json` that
    matched the file mtime to the second, and answering `initialize` with
    `serverInfo: unreal-editor`. Its `tools/list` served `execute_script`. The proxy's "Reason"
    string describes a read it never performed. Restarting the editor would have cost the session
    and fixed nothing.
  - **THE MUTATION, AND IT IS FULLY REVERTED.** `DirectiveText`'s design-time `Text` was set to
    `SENTINEL9137SENTINEL9137` via `configure_widget`, compiled and saved with PIE stopped, then
    restored to `NSLOCTEXT("UMG", "TextBlockDefaultValue", "Text Block")` the same way. Saving
    moved the package bytes even though the value round-tripped, so the gated bytes were put back
    with `git restore`: `git hash-object` on `Content/UI/WBP_DirectiveStrip.uasset` reads
    `c4eb264740ffaa0daeab8843caf7a9a38690dada`, identical to `git rev-parse HEAD:<path>`. The
    editor reports zero dirty content packages and zero dirty map packages afterwards.
  - **THE NEGATIVE OBSERVATION: THE SENTINEL DID NOT PAINT, TO THE PIXEL.** PIE from the game's own
    startup path on `Lvl_FerrumCrossing`, guidance populated on the live widget exactly as in the
    entry below -- `bActive=True, Beat=Beat1a`, the same 76-character directive,
    `bHasObjectiveRing=True, ObjectiveHex=(X=2,Y=7), bEndTurnGated=True`. `Shot showui` at
    2544x1320 gives the strip rectangle at x 750-805, y 157-194, w=56, h=38, 2128 px, centre 777.5
    -- byte-for-byte the recorded baseline, and identical to the earlier capture retained as
    `ScreenShot00059.png`. Width 56 is still the border's own 28+28 padding, so the painted content
    is STILL ZERO-WIDTH with a 24-character sentinel sitting in the property.
  - **THE POSITIVE CONTROL IS THE WHOLE VALUE OF THIS ENTRY, because a blank screen is also what a
    broken experiment looks like.** A sentinel that never reached the running widget would paint
    nothing for reasons having nothing to do with the binding. It reached it: `GetAll TextBlock
    Text` on the live PIE instance returned
    `GameInstance_3.WBP_DirectiveStrip_C_0.WidgetTree_0.DirectiveText.Text =
    NSLOCTEXT("", "78DBFAF7426A5FBCC04C17B9C4577B9A", "SENTINEL9137SENTINEL9137")`. The same
    command returned real `LOCGEN_FORMAT` values for the scoreboard's live TextBlocks in the same
    output, so the reader was shown able to speak on populated live state rather than assumed to
    be. This works because a UMG binding drives the Slate attribute WITHOUT writing the UPROPERTY
    back, so `GetAll` on a bound property reports the design value -- the property read and the
    painted read are two different measurements, which is exactly what this experiment needed.
  - **THE INFERENCE, AND ITS LIMIT STATED RATHER THAN ROUNDED OFF.** A TextBlock with no active
    binding paints its UPROPERTY. This one holds a 24-character sentinel, is `Visible` on the live
    instance, and paints zero-width. So a binding IS active and IS supplying the painted value, and
    that value is not the sentinel and not the 76-character directive. **What was measured is
    zero-width painted content, NOT literally `FText::IsEmpty()`** -- an empty `FText` is the
    natural reading and the only survivor of the exclusion list below, but the measurement is the
    width. `DirectiveText`'s own `Visibility` is unbound, so the `Visible` read is true runtime
    state and a collapsed-text-block branch is dead too.
  - **WHAT IS NOW EXCLUDED, added to the list the entry below built rather than replacing it.** Not
    the input wiring, not variable shadowing, not an unregistered binding, not a dead widget tree,
    not the value on the instance, not the purity of the four graphs -- and now **not a binding
    that fails to run**. The binding runs, wins over the UPROPERTY, and returns something that
    renders to nothing.
  - **THE ONE QUESTION LEFT, and it is narrower than anything this section has carried before.** Why
    does a pure graph wired `Get Guidance` to `Break Strat Guidance View` to the `DirectiveText`
    member to the Return node yield a zero-width value on the very instance whose `Guidance`
    reads a populated 76-character `DirectiveText` in the same PIE session, seconds apart, from the
    same console? Both halves of that sentence are measured on `WBP_DirectiveStrip_C_0`. Next lane
    should read what the binding function actually RETURNS rather than whether it runs -- `ke`
    confirms execution but prints no return value, which is the gap that kept this open.
  - **INSTRUMENT NOTE.** All asset work was done with PIE stopped, per the hazard the entry below
    records; the editor survived both PIE teardowns and no widget was opened while PIE was live.
    The captures live under `Saved/Screenshots/WindowsEditor/`, which is not tracked, so quote the
    geometry above rather than relying on those files surviving.

- **2026-08-22, COORDINATOR, LIVE PIE ON THE INTEGRATION TREE, NO SOURCE AND NO ASSET CHANGE --
  THE DELIVERY FIX IS CONFIRMED ON A LIVE WIDGET, AND THE STRIP STILL PAINTS NO DIRECTIVE.
  Sec 2.11.6-B DOES NOT CLOSE. What remains is inside the widget asset, and that is now separated
  from C++ by measurement rather than by argument -- editor-builder's lane.** The session this
  banner asked for was run: editor launched on this tree, PIE from the game's own startup path
  (`Game class is 'BP_StratGameMode_C'`, then `Guided opening armed for side 0: objective hex
  (2, 7), window turns 1-4` at `12.01.00:065`), and no asset opened at any point in it.
  - **THE DELIVERY HALF WORKS, and this is the first time it has been seen on a live widget.**
    At t+2.4 s, `GetAll StratGuidanceWidget Guidance` on `WBP_DirectiveStrip_C_0` returned
    `bActive=True, Beat=Beat1a, DirectiveText="Select the marked Infantry. Lit hexes are its true
    reach. Click one to move.", bHasObjectiveRing=True, ObjectiveHex=(X=2,Y=7), bEndTurnGated=True`
    -- the whole decorated projection, with no `ke` and no input. The comparable read on
    2026-08-21 was taken at t+2.5 s and returned all-default. Same instrument, same moment in the
    match, opposite result.
  - **THE PAINTED STRIP DID NOT MOVE, to the pixel.** `Shot showui` gives the same 2128-pixel
    rectangle at x 750-805, y 157-194, centre 777.5 in a 2544x1320 frame, which is what all six
    2026-08-21 captures gave. Width 56 is the border's own 28+28 padding, so the content is still
    ZERO-WIDTH: no glyph of the 76-character directive is on screen.
  - **THE DIFFERENTIAL THAT IS NEW, and it is why this run was worth taking.** Every prior capture
    was taken while the guidance was default, so "the painted state is not a function of the
    guidance" rested on three states that never differed in the variable. This run supplies the
    other side: guidance populated, output byte-identical. The variable was finally VARIED, and the
    screen did not respond.
  - **What is already correct, so the next lane does not re-measure it.** `bp:list_bindings()`
    returns all four as `kind=Function` with the right `object_name`/`property_name`/
    `function_name` -- `DirectiveText.Text` to `GetDirectiveText`, `StripBorder.Visibility` to
    `GetStripVisibility`, and the two tag ones. All four graphs are fully wired: `Get Guidance` to
    `Break Strat Guidance View` to the right member to the Return node, and `Active` to
    `To Visibility (Boolean)` whose False pin is `Collapsed`. The blueprint has ZERO local
    variables and its parent is native `StratGuidanceWidget`, so the variable-shadowing branch the
    2026-08-21 correction kept alive is DEAD -- there is nothing to shadow with. And in the live
    PIE world the tree is fully instantiated: `WidgetTree_0` with `Root`, `StripBorder`,
    `StripRow`, `DirectiveText` and `WindowEndTag`, every one of them `Visible`.
  - **A LEAD WAS RAISED HERE AND IS WITHDRAWN IN THE SAME PASS, BEFORE ANY LANE ACTED ON IT.**
    What was observed is real: in all four binding graphs the `K2Node_FunctionEntry` carries an
    exec output pin and the `K2Node_FunctionResult` an exec input pin, and BOTH ARE UNCONNECTED.
    The reading built on it was that these are ordinary impure graphs, so no body ever executes and
    each binding takes its Return pin default -- which would have matched the screen exactly
    (`Visible` for the border whatever `bActive` is, empty for the Text ones). **That reading is
    almost certainly WRONG, and the thing that refutes it was already recorded.** The UMG compiler
    REFUSES a binding to a non-pure function, in its own words -- *"property 'TextDelegate' on
    widget 'DirectiveText' needs to be bound to a pure function, 'Get Directive Text' is not
    pure"* -- and that error is why these four were authored with `add_function(..., pure=true)` in
    the first place. The generated class carries all four bindings, so the blueprint compiled with
    them; therefore the four functions ARE pure, and this reader reports entry/result exec pins on
    a pure graph too. **The pin dump was never evidence of impurity, and it is the reader's shape
    rather than the asset's.** Recorded rather than deleted because the observation stands and the
    next reader will otherwise draw the same inference from the same dump.
  - **SO THE MECHANISM IS STILL OPEN, and these are excluded rather than guessed at.** Not the
    input wiring (`Return Value` reports `connected=true, linked_to_count=1` on every binding
    function). Not variable shadowing (zero local variables; the reparent re-scoped the member to
    native `StratGuidanceWidget:Guidance`). Not an unregistered binding (all four present, correct
    triple). Not a dead widget tree (fully instantiated, all five `Visible`). Not the value
    (`bActive=True` with the 76-character directive on the live instance). The four functions also
    EXECUTE on demand: `ke * GetDirectiveText`, `ke * GetStripVisibility` and
    `ke * GetWindowEndTagText` each answered `1 instances succeeded` with no `Bad or missing
    property` line above the count, which is the positive control on that call -- though `ke`
    prints no return value, so what they RETURN is still unmeasured.
  - **THE ONE EXPERIMENT THAT SEPARATES THE TWO SURVIVORS, and it is a Content/ write, so it is
    the editor-builder's to run.** Set `DirectiveText`'s DESIGN-TIME `Text` to a distinctive string
    and take one PIE capture. If that string PAINTS, the binding is not delivering and Slate is
    showing the UPROPERTY. If the strip stays blank, the binding IS delivering and the function is
    returning an empty `FText`. Today the design value is the stock `"Text Block"` and the screen
    shows NEITHER it nor the 76 characters, which is what makes the two indistinguishable from
    outside. Revert the design value afterwards; the asset is gated bytes.
  - **INSTRUMENT WARNING, AND IT COST A FALSE FINDING THAT WAS CAUGHT BEFORE IT REACHED THIS FILE.
    `open_asset` ON A WIDGET BLUEPRINT RECOMPILES IT, AND DOING THAT DURING PIE DESTROYS THE LIVE
    WIDGET TREE.** In the first session of this pass I opened `WBP_DirectiveStrip` to read its
    graphs while PIE was running. Afterwards `GetAll UserWidget WidgetTree` returned
    `WBP_DirectiveStrip_C_0.WidgetTree = None` with zero live children, while the scoreboard's
    `WidgetTree_0` and its fifteen children answered in the SAME command -- a control that made the
    absence look real. It was not real; it was my own recompile. What exposed it was that the box
    had already left the screen BEFORE the removal test meant to identify it, so the timeline did
    not fit. Re-run clean, with a fresh editor and nothing opened, the tree is fully live.
    **Nothing after the first `open_asset` in a PIE session is evidence about the game.** The same
    session then crashed the editor on PIE teardown at `PlayLevel.cpp:553`, the leak chain naming
    `TransBuffer` -- the editor's own undo buffer -- and no game object; a later `playtest_stop`
    with no recompile behind it ended PIE cleanly and the editor survived, which is the control on
    that half. Read the asset with PIE STOPPED, or not at all.
  - **Consequence for the lanes, stated because two of them have been waiting on it.** The
    engineer's delivery work is done as far as the screen can see: the value arrives. What remains
    is inside `WBP_DirectiveStrip` and belongs to `strat-editor-builder`. The two questions the
    2026-08-21 entry left open collapse to one, and it is NOT "does the binding take, or does its
    input read something other than the native guidance" -- the input wiring is confirmed correct
    and the shadowing branch is dead. It is: **does the delivered value reach Slate, or does the
    bound function return an empty one?** The design-time-text swap above answers it in one PIE
    capture.
  - **THE BUILDER COULD NOT BE DISPATCHED IN THIS SESSION, and the reason is the proxy, not the
    editor.** `ToolSearch` for `execute_script` returned nothing while the same keyword search
    returned `unreal_status` and `list_unreal_projects` -- the two-tool fallback surface, control
    passing. `strat-editor-builder` carries the NeoStack tools plus Read/Grep/Glob/Skill and **no
    Bash**, and a subagent only receives tools the session already has, so with `execute_script`
    absent it has no route to the editor at all. The editor itself was fine throughout: every
    measurement above was taken by the coordinator over the editor's own HTTP endpoint. Dispatch
    the builder from a session whose surface actually carries `execute_script`; do not spend a
    round having it discover the wall.

- **2026-08-22, COORDINATOR, MEASURED WHILE WRITING THIS PASS'S BANNER -- `strat_banner_sweep.py`
  CAN BE MADE TO SKIP THE BANNER'S LIVE COUNT ENTIRELY BY AN ORDINARY ENGLISH WORD, AND IT PASSES
  CLEAN WHEN IT DOES. Steward's lane; this is a guard going inert on its own subject.**
  **[STAMPED 2026-08-22, later session -- FIXED IN THE SCRIPT; true when written. Both repairs
  named below shipped, plus four the fix itself uncovered. The closing sub-bullet carries what
  landed and how it was measured; the diagnosis above is kept because the mechanism it names is
  the one that was repaired.]** The first
  draft of the banner above ended a sentence with "repeating the console reads with the fix in the
  tree" immediately before `THE SUITE **IS NOW 160/160**`. The sweep then reported
  `suite claims found: 18 (0 live, 18 stamped)` and `SWEEP CLEAN`, exit 0 -- with a banner asserting
  a suite count that no check had compared to anything.
  - **The mechanism, read out of the script rather than guessed.** `_QUOTED_FIGURE_RE` matches
    `said|says|saying|read |reads |reading|claim|shape|editing|planting|planted|wrongly|stale|
    defect|would have|reported` anywhere in the **120 characters before** a figure, and a match
    makes the loop `continue` -- the claim is not merely called stamped, it is never collected. The
    exclusion exists for a good reason (an honest account of a past miscount must be able to write
    the wrong number down) but it is far broader than that purpose: `reads`, `claim`, `shape` and
    `defect` are words this record uses constantly, and any of them within 120 characters upstream
    silences the live figure.
  - **The compounding half, and the reason this is worth a phase rather than a note: ZERO LIVE
    CLAIMS IS NOT AN ERROR TO THIS SCRIPT.** `global.md` is the only file permitted a live suite
    count, and a run in which it has none means either the banner is missing its figure or the
    figure was swallowed. Either way nothing was checked, and the script says `SWEEP CLEAN`.
  - **Rewording proved it, both directions.** With "reads" in that sentence: 18 claims, 0 live,
    exit 0. With the single word changed to "measurements" and nothing else touched: 19 claims,
    1 live, `global.md:19 LIVE 160/160`, exit 0. The banner in this commit is the second version.
  - **Two repairs, and the second is the one that matters.** (1) Do not let the quoted-figure
    exclusion outrank an explicit live tense -- `is now` immediately before the figure is a
    deliberate assertion and should beat a stray `reads` 100 characters upstream. (2) **Refuse a
    sweep of `global.md` that finds no live suite claim at all**, the same way the pre-sliced
    corpus guard was made to refuse a zero-event corpus rather than pass it. A guard that cannot
    find its subject must say so, not report clean.
  - Owner: `strat-data-steward` (`Tools/architect/` is its lane). Found by the coordinator while
    writing a banner, not by the guard's own self-tests.
  - **FIXED 2026-08-22, later session, coordinator, in `Tools/architect/strat_banner_sweep.py`
    alone -- no record file's figures were touched.** The two repairs above landed, and four
    more that only became visible once they had. **[STAMPED with this sub-bullet: everything
    here is the state of the script as of this commit.]**
    - **The exclusion is scoped to the figure's own sentence** (`quoting_window`,
      `_SENTENCE_BREAK_RE`), so a reporting verb one sentence upstream no longer silences the
      claim after it. The boundary is tuned to this record's prose: `2.11.6-B`, `global.md` and
      a `reportCreatedOn` timestamp are not read as sentence ends.
    - **An explicit live tense outranks the exclusion** -- repair (1) as filed.
    - **`check_live_count_present` refuses a sweep of this file that finds no live count** --
      repair (2) as filed, and the one that makes the whole class non-silent.
    - **`in_code_span` replaces the single-character backtick test.** Uncovered by repair (1):
      the old test caught a figure written `` `107/107` `` but not one inside
      `` `THE SUITE **IS NOW 160/160**` ``, because the character before it is an asterisk.
      This record quotes its own banner verbatim, so the sentence above that reproduces the
      banner's first-draft wording was then collected as a SECOND live claim. It agreed with
      the banner by luck, so the sweep stayed green while carrying a claim it had misread.
    - **The tense markers now reach across a bold opener.** `was **N/N**` puts `**` between the
      verb and the number and the old `\s*$` anchor could not span it, so the verb was never
      seen and the claim fell through to the default, which is LIVE. Every such figure in this
      record carries an explicit stamp, which outranks the verb anyway, so the record masked it
      and so did two fixtures.
    - **The banner region is a stack of banners, newest first** (`current_banner_end`). This
      file has no heading until `## BUILT`, so `section_of` called everything above it "BANNER"
      -- eleven successive `_Last run` blocks. The existing "last banner figure wins" rule is
      right within one block and backwards across the stack: a post-milestone figure written
      2026-08-14 was collected as this file's live claim and superseded the real one.
      `check_banner_date` already reads only the FIRST `_Last run` as current; the figures now
      follow the same rule.
    - **Measured, both directions, against this record rather than a fixture.** Planting a live
      count at each of the 766 non-blank lines of `## NEXT`: never-collected went from 132 to 38
      for the bolded wording and 134 to 38 for plain prose, and the residual 38 are the probe
      landing inside an unclosed code span on the line it was appended to. In `engine.md`, 29 to
      zero. Seven realistic wordings planted as their own bullet -- sentence-final full stop,
      closing paren, after a colon, all caps, an em-dash clause, mid-list with commas, and one
      with no live verb at all -- exit 0 on the old script for all seven, exit 1 on the new one
      for all seven. Stamping the banner's own live figure out: exit 0 before, exit 1 now with
      `LIVE COUNT MISSING`. The untouched record exits 0 on both.
    - **The bolded-versus-plain framing this entry was filed under was a symptom.** The figure
      pattern has matched a bare `N/N` since 2026-08-20. Both wordings were swallowed at the
      same 132 sites; they diverged at exactly two, where the bold form's extra two characters
      pushed the reporting verb past the 120-character cut. Read the window, not the markup.
    - Self-test now carries 29 inline fixtures, six of them for these repairs, including a pair
      that fails in one direction and passes in its near-twin so the scoping cannot quietly
      widen. CI's three steps all exit 0.

- **2026-08-21, COORDINATOR, LIVE PIE, NO SOURCE AND NO ASSET CHANGE -- THE THREE t+0 BRANCHES ARE
  SEPARATED, AND TWO OF THEM ARE REAL.** Branch 1 is eliminated by an artifact already in the log,
  branch 2 is confirmed, and branch 3 is CONFIRMED AS A MECHANISM -- an undecorated reconcile was
  driven and the strip's guidance was watched being wiped. This answers question (2) of the entry
  below. Question (1), the binding, is untouched and still open. Measured 23:40-23:53 LOCAL on
  2026-08-21 and filed after local midnight, so the commit that carries it is stamped the 22nd:
  the entry date records when the work was measured, not when the filing landed, and the log's own
  dual stamp settles it (`[2026.08.22-03.53.09]` carries `at 2026.08.21-23.53.09`). **Both defects
  have to be fixed:
  correcting the match-start ordering alone would leave any later undecorated reconcile clearing
  the strip -- and the AI turn is one, by the same call, though that specific caller is inferred
  rather than driven (see branch 3).**
  - **BRANCH 1 -- `FindScoreboardHUD` returned null: ELIMINATED, on evidence every log in this
    investigation already carried.** `UStratMatchSubsystem::HandBridgeToScoreboard` obtains the HUD
    through `FindScoreboardHUD()` and returns early on the null branch, so `AdoptBridge` is
    unreachable without one; `Bridge adopted: this HUD now draws a match it does not own` appears at
    match start in every session. Nothing new had to be run.
  - **BRANCH 2 -- `GuidanceStrip` was still null: CONFIRMED, by a chain whose every link is a log
    artifact, a straight-line reading, or a graph with zero connections.**
    (a) The HUD's terminal `BeginPlay` line reads `Scoreboard live on an adopted bridge (this HUD
    seeded nothing)`, the branch taken only when `AdoptedBridge` is already set -- so that
    `BeginPlay` ran AFTER the handover.
    (b) Nothing that `AStratScoreboardHUD::BeginPlay` reaches can log the arming line, so its
    terminal line bounds its whole execution. Checked on BOTH sides rather than asserted: in C++,
    the adopted path calls `Super::BeginPlay`, `ViewingSideInRange`, `CreateGuidanceWidget`,
    `CreateScoreboardWidget` and `RefreshScoreboard`, none of which reaches
    `AStratPlayerController::RefreshFromMachine`, the sole route to that log line; and on the
    Blueprint side, which is where re-entry would hide, `BP_StratScoreboardHUD`'s EventGraph and
    `WBP_DirectiveStrip`'s EventGraph each hold three event stubs with **zero connections** --
    `ReceiveBeginPlay`, `PreConstruct` and `Construct` execute nothing. `WBP_Scoreboard` is the
    ONLY one of the three constructed late enough to have broken the chain, and it is closed by its
    ENTRY POINTS rather than by counting its interior: its four events are `PreConstruct`,
    `Construct`, `Tick` and `OnScoreboardRefreshed`, and the first three have UNCONNECTED exec pins
    -- only `OnScoreboardRefreshed` runs anything. Nothing in that graph executes at
    `CreateWidget`/`AddToViewport` time at all, which settles it whatever the interior holds.
    Corroboration rather than the load-bearing link: 39 nodes, 44 connections, and all nine of its
    `K2Node_CallFunction` nodes are `SetText (Text)`. Only a re-entry AFTER `CreateGuidanceWidget`
    could have broken the chain, and the one graph that could have is closed at its door.
    (c) `CreateGuidanceWidget` runs inside that `BeginPlay`, above the scoreboard block and so
    before the terminal log.
    (d) In FIVE fresh PIE sessions the order was identical -- adopt, `Guided opening armed`, then
    the terminal line -- and by (b) the `BeginPlay` cannot straddle the arming line, so it STARTED
    after it.
    Therefore at the decorated `ApplyView`, which runs in the same `RefreshFromMachine` call that
    logged the arming, `GuidanceStrip` was null and `AStratScoreboardHUD::PushGuidance` -- whose
    whole body is a null check and a forward -- dropped it silently. That silence is deliberate and
    documented: "no strip is a configuration, not a refusal". The strip is then built holding the
    default, and in those five sessions the read ~2.9 s later is all-default.
  - **BRANCH 3 -- a later UNDECORATED `ApplyView` overwrites it: CONFIRMED AS A MECHANISM, driven
    and observed.** `ke * RefreshFromMachine` put the full decorated projection on the widget;
    `ke * SetViewingSide 0` then ran `UStratMatchSubsystem::SetViewingSide` ->
    `RefreshPresentation` -> `BuildViewModel` + undecorated `ApplyView` -> `PushGuidance`, and the
    next read one second later was `bActive=False, Beat=None, DirectiveText=""`. **An undecorated
    reconcile clears the strip, watched happening.** It does NOT fire spontaneously: after an
    earlier decorated push the projection survived on the widget for 46 seconds of live idle match.
    Production callers of an undecorated `ApplyView` are exactly three --
    `UStratMatchSubsystem::StartMatchInternal`'s first reconciliation, which does it deliberately
    and says why in its own comment, and `RefreshPresentation`, reached from `SetViewingSide` and
    from `RunAiTurnsNow`. The AI route is the one that matters in play and is the one NOT directly
    driven here, because taking a turn needs input; it is the same function through the same call,
    so it is a strong inference rather than an observation, and is labelled as such.
  - **THE SHAPE OF THE DEFECT, which is larger than either branch.** The guided-opening decoration
    lives on `AStratPlayerController::RefreshFromMachine` alone, while three subsystem-side paths
    reconcile the same screen without it. At match start the strip does not yet exist for the only
    decorated push, and afterwards any subsystem-side reconcile clears what a decorated push left.
    Engineer's lane. Neither a call site nor a null check is the whole answer.
  - **INSTRUMENT, and it cost a wrong claim before it paid for one.** `ke` parses arguments
    POSITIONALLY: `ke * SetViewingSide 0` works, `ke * SetViewingSide InViewingSide=0` does not, and
    the bare form does not either -- both print `'SetViewingSide': Bad or missing property
    'InViewingSide'` and then, on the SAME command, `Called 'SetViewingSide' on everything in the
    world and 2 instances succeeded`. **A `ke` success count is not evidence the function executed**;
    read the lines above it. An earlier draft of this bullet concluded from those two failures that
    `ke` could not reach the function at all, and spent that conclusion to leave branch 3
    unmeasured -- a capability claim about the instrument, drawn from one unsupported syntax, that
    the gate caught and the positional form disproved in a single run.

- **2026-08-21, COORDINATOR, LIVE PIE, NO SOURCE AND NO ASSET CHANGE -- THE PUSH PATH WORKS, AND THE
  PAINTED STRIP IS NOT A FUNCTION OF THE DATA IT HOLDS.** **[THE FIRST HALF OF THAT HEADLINE IS
  RETRACTED -- read the CORRECTION that ENDS this bullet before acting on anything in it. The push
  path works once every participant exists; it did NOT work at match start, and the two links this
  bullet clears are not cleared.]** The ordered experiment the bullet below asked for was run, and
  it answers BOTH questions -- differently from the way either was framed. One fresh PIE session,
  three states, `GetAll` for the data and `Shot showui` for the screen at each.
  - **Question (2) is ANSWERED: the third branch is the one, and `FindScoreboardHUD` and
    `PushGuidance` are both innocent.** **[RETRACTED IN FULL -- see item 1 and item 2 of the
    CORRECTION ending this bullet. This is the one sentence here that changes what the next lane
    does, and it is wrong: it retires a branch that must still be measured at t+0.]**
    **[THAT MEASUREMENT HAS SINCE BEEN MADE -- newest bullet, five sessions of ordering plus a
    driven reconcile. DO NOT REDO IT. `GuidanceStrip` was null at t+0. Kept as a sibling marker
    rather than nested inside the one above, because markdown does not nest `**` and the nested
    form rendered this sentence UNbolded inside a bold retraction -- emphasis inverted on the one
    clause meant to stop a redo.]** State A, 2.5 s
    after `Guided opening armed for side 0`: the widget's `Guidance` reads all default. Then `ke *
    RefreshFromMachine` on the live controller, and state B reads `bActive=True, Beat=Beat1a,
    DirectiveText="Select the marked Infantry. Lit hexes are its true reach. Click one to move.",
    bHasObjectiveRing=True, ObjectiveHex=(X=2,Y=7), bEndTurnGated=True, EndTurnGateHover="Move the
    marked Infantry first.", LockedUnitHover="Locked this turn."` -- the whole decorated projection,
    delivered to the widget by the ordinary route. So the route from
    `FStratGuidedOpening::DecorateViewModel` through `ApplyView` and `PushGuidance` to the widget is
    WORKING. What is missing is the CALL: nothing runs `RefreshFromMachine` at match start, so the
    model that reaches `ApplyView` then is UNDECORATED and the unconditional push delivers a
    default. Engineer's lane, and the fix is a call site rather than a mechanism.
  - **Question (1) is ANSWERED AND ITS LEAD IS DEAD -- the answer is bigger than the question.**
    **[THE "LEAD IS DEAD" HALF IS RETRACTED -- see item 4 of the CORRECTION ending this bullet. This
    run CORROBORATES that lead's open branch rather than killing it. The pixel and channel wording
    in the rest of this sub-bullet is repaired there too, in the correction's trailing paragraph.]**
    The screen is PIXEL-IDENTICAL in all three states: the same 56x38 rectangle at x 750-805, y
    157-194, n=2128, in state A (`Guidance` default), in state B (fully populated, quoted above) and
    in state C after `ke * SkipGuidance` put it back to default. **A widget holding a 76-character
    directive drew exactly the same empty box as one holding an empty string.** So the fault is not
    specific to `Visibility` and there is nothing left to explain about `ToVisibility`'s false
    branch: the strip's Slate state is not a function of `Guidance` in ANY channel. The conditional
    lead recorded below -- "if the false pin resolves to a non-painting visibility then `Active`
    must be reading something true" -- is now MOOT rather than merely unmeasured, and is superseded
    here rather than deleted. Its premise never got to matter.
  - **What the text channel shows, and it is the sharpest clue in the run.** **[RETRACTED IN FULL,
    AND IT ENDS IN AN INSTRUCTION THAT MUST NOT BE ACTED ON -- see item 3 of the CORRECTION ending
    this bullet, which withdraws the whole `GetAll` elimination below, and item 4, which says of
    this sub-bullet's closing handoff: do not dispatch an engineer on it. This is the most
    actionable-looking paragraph in the entry and it is the one most fully withdrawn.]**
    `DirectiveText` renders NEITHER the bound value (state B's 76 characters) NOR its design-time
    default, which `GetAll TextBlock Text NAME=DirectiveText` confirms is still `"Text Block"` on
    the live instance. It renders EMPTY. An empty string is what the Text binding returns when
    `Guidance` is default -- which is what `Guidance` held at construction. That reads as a binding
    evaluated ONCE and never again, rather than one never applied; a binding never applied would
    have left "Text Block" on screen. `StripBorder` is the opposite shape: it shows its DESIGN value
    (`Visible`), where a binding evaluated once at construction would have returned `Collapsed` and
    drawn nothing. Two channels, two different failures, one widget. NOT DIAGNOSED HERE, and
    deliberately not: the next step is an engineer reading `UWidget::SynchronizeProperties` and the
    generated class's `InitializeWidget` against this asset, not another black-box round.
  - **Instrument note.** `ke * RefreshFromMachine` is a way to drive a reconcile with no simulated
    input at all, and this run is the first time it has been used to CHANGE observed state rather
    than to confirm one. `AStratPlayerController::RefreshFromMachine` is `BlueprintCallable`, which
    is what makes it reachable; `GetAll` then reads the result off the live widget.
  - **CORRECTION, 2026-08-21, same session, raised by the record gate on `68d56d1` and adopted --
    THE BULLET ABOVE EXONERATED THE WRONG BRANCH ON A TEST THAT COULD NOT SEE IT, AND ITS COMMIT
    MESSAGE CARRIES THE SAME ERROR WHERE IT CANNOT BE EDITED.** Four repairs, in order of how much
    they change what the next lane does. One method note, stated once because it produced three of
    the findings that got this bullet here: a fix applied to the sentences a gate ENUMERATES, rather
    than derived over the whole passage, leaves the tail asserting what the head withdrew. The
    attempt-by-attempt history lives in the commit messages of `68d56d1`, `de517f0`, `c21e7a5`,
    `d67471b` and `a55f924`, where it cannot be edited and does not compete with the state of the
    world; this file answers what the next lane should DO.
    1. **"nothing runs `RefreshFromMachine` at match start" IS FALSE, and the tree says so twice.**
       `AStratPlayerController::BeginPlay` calls it unconditionally, and its comment there
       anticipates this exact ordering ("actor BeginPlay order is not guaranteed"). Independently,
       `Guided opening armed for side 0` is emitted only by `FStratGuidedOpening::Begin`, reachable
       only through `TryArmGuidedOpening`, which has exactly TWO call sites -- `RefreshFromMachine`
       and `HandleSelectionEvent` -- and no input event occurred at match start. So
       `RefreshFromMachine` ran, cleared its bridge and build-model guards (they precede
       `TryArmGuidedOpening` in its body), decorated, and reached `ApplyView` with a DECORATED
       model, all in frame 27 at `03.06.08:571`. **And that model was decorated with a POPULATED
       block, which is measured rather than inferred:** `FStratGuidedOpening::Begin` returns before
       the arming line on BOTH its early exits -- the `bSuppressed` branch and the `!Guided.bOk`
       branch -- and sets `bActive = true` on the same straight-line path above it, with no
       intervening return, so the log line cannot be emitted by an armed-but-inactive opening. A
       later reader asking "armed, but suppressed?" has the answer here rather than a reopened
       question.
    2. **`FindScoreboardHUD` and `PushGuidance` are therefore NOT exonerated, and the bullet above
       had no evidence about them.** It tested them at t+2.5 s and t+6 s, when the strip already
       existed. The question is the state at t+0. `AStratScoreboardHUD::BeginPlay` creates the strip
       before the scoreboard, and its terminal `Scoreboard live` line lands at `08:573`, two
       milliseconds AFTER the arming line, in the same frame; `CreateGuidanceWidget` logs nothing on
       success, so **the log cannot resolve the order and neither can this pass.** What IS measured:
       a decorated `ApplyView` ran at match start, and 2.4 s later the widget still held defaults --
       so that push did not land on the strip, or did not survive. THREE branches, and this pass
       cannot separate them: `FindScoreboardHUD` returned null; `GuidanceStrip` was still null; or
       the decorated push DID land and a later UNDECORATED `ApplyView` overwrote it, `PushGuidance`
       being documented as assigning unconditionally. The third is not excluded by "no later
       `ApplyView` comes without input" -- `ApplyView` logs nothing, so the log's silence is not a
       control. TWO of the three sit inside the branch the bullet above calls innocent; the THIRD
       does not -- there both links did their job and the fault is a later undecorated `ApplyView`,
       which is the call-site story rather than the link story. That is exactly why this pass can
       neither convict nor exonerate those links, and why the t+0 ordering still has to be
       measured. **[ANSWERED by the newest bullet at the top of `## NEXT`, dated the same day: the
       t+0 ordering WAS measured, five sessions of it. Branch 1 is eliminated; branch 2 is
       CONFIRMED; branch 3 is CONFIRMED AS A MECHANISM -- an undecorated reconcile was driven and
       the strip's guidance was watched being cleared -- though it does not fire spontaneously.
       TWO of the three are real, and both need fixing. This item's refusal to convict or exonerate
       was right for the evidence it had; it is superseded, not wrong. (This marker first said
       "branch 3 does not fire at match start", which is the framing the newest bullet retired in
       the same pass for reading as an exoneration -- carried over verbatim instead of re-derived
       against the text it summarises.)]**
       `CreateGuidanceWidget`'s own comment already names the shape: "The strip draws its own
       defaults until the first `ApplyView`" -- it assumes a LATER `ApplyView`, and whether one
       comes without input is exactly what is NOT established here. **[NOW ESTABLISHED, BOTH WAYS,
       by the newest bullet: none comes spontaneously -- a decorated projection survived 46 s of
       live idle match -- and a driven one CLEARS the strip. The question this sentence leaves open
       is closed.]**
    3. **The GetAll-based elimination is WITHDRAWN -- it used an instrument this same file rules out
       for that purpose four bullets down.** The 2026-08-21 entry says: a UMG property binding
       drives the Slate attribute and never writes the UPROPERTY back, so read `GetAll` for unbound
       state only. Then "the live instance still holds `Text Block`" is equally what a FULLY WORKING
       binding leaves behind, and it eliminates nothing. So "a binding evaluated ONCE and never
       again", "a binding never applied would have left Text Block on screen", and "two channels,
       two different failures, one widget" are all unsupported and are retracted. Also disclosed,
       because the bullet above did not: those two reads were taken at `03.07.57`, ~98 s after the
       last capture and after `SkipGuidance` -- in state C, not in state B where the 76 characters
       were loaded.
    4. **The `ToVisibility` lead is NOT moot; this run is CONSISTENT with its open branch and
       predicts the text channel exactly.** That lead's second branch is that the binding takes and
       returns `Visible` because `GetStripVisibility`'s `Active` is not fed by the native
       `Guidance`. Three captures showing the screen is not a function of the native `Guidance` is
       precisely what that branch predicts, and ONE such defect -- bindings reading a source the
       native `Guidance` never reaches -- would explain both channels at once -- PROVIDED the false
       pin does not resolve to `Collapsed` at runtime, which is still only read off the graph, since
       a shadowed always-default source would take that false pin and the border would not have
       painted at all. "Both stand until one is measured" is restored. The handoff in the bullet
       above (read `UWidget::SynchronizeProperties` and `InitializeWidget`) rests on the reading
       withdrawn in item 3; **do not dispatch an engineer on it.** The binding's input wiring is at
       least as good a place to look.
    Two wording repairs, neither load-bearing but both stated because this record has been blocked
    for smaller: "pixel-identical" overstates the artifact -- re-derived independently, 283 to 309
    of the 2128 pixels differ between any two of the three frames, all by a single LSB of dither,
    max per-channel delta 1, with 3 to 4 unique colours and no glyphs in any state; the conclusion
    that nothing painted, resized or appeared is untouched. And "not a function of `Guidance` in ANY
    channel" should read "in any channel a composited capture can observe" -- the hover strings were
    never exercised and the objective ring is drawn by the board, not the strip.
    **What survives the correction, unaffected:** the route from `DecorateViewModel` through
    `ApplyView` and `PushGuidance` to the widget WORKS once every participant exists -- state B is a
    direct observation of the full projection arriving -- and the strip's painted state did not
    follow it. Those two facts are what the next lane should start from.

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
    - **CORRECTION, same session, raised by the post-merge record gate and adopted before anyone
      relied on it: this names ONE hypothesis where the evidence carries TWO.** What is measured is
      that the border paints while `Guidance.bActive` reads false. "The bound `Visibility` does not
      take" is one explanation. The other is that the binding DOES take and correctly returns
      `Visible` because `GetStripVisibility`'s `Active` input is not fed by the native
      `Guidance` -- the variable-shadowing failure this project has already met once, which
      compiles clean and wires to nothing. The `Bindings` array proves the binding is REGISTERED,
      not what its body reads, and the `Collapsed` false-pin default was read off the graph rather
      than at runtime. `ae4795f` reports the reparent destroying the Blueprint-local `Guidance` and
      re-scoping to `StratGuidanceWidget:Guidance` at the package-byte level, which makes the
      shadowing branch less likely -- less likely is not excluded, and the two are different
      defects in different lanes. Both stand until one is measured.
  - **SECOND DEFECT, measured directly in a SECOND, FRESH PIE session rather than inferred from the
    first: the widget's guidance is still all default two seconds after the opening arms.**
    **[THE "demonstrably non-null" CLAIM BELOW IS SUPERSEDED -- see the newest bullet at the top of
    `## NEXT`. `GuidanceStrip` was measured non-null at t+2.5 s and it IS null at t+0, which is the
    moment that decides; the reading was true of the time it sampled and wrong as an exclusion.]**
    The log line `Guided opening armed for side 0: objective hex (2, 7), window turns 1-4` had
    already been
    written when `GetAll StratGuidanceWidget Guidance` read `bActive=False, Beat=None,
    DirectiveText=""` off the live instance, and the same session's `GetAll StratScoreboardHUD
    GuidanceStrip` shows the HUD holding a pointer to THAT instance -- so the widget the HUD
    created is the widget being read. `ApplyView`'s `HUD->PushGuidance(Model.Guidance)` is guarded
    only by `FindScoreboardHUD()`, and `AStratScoreboardHUD::PushGuidance` is guarded only by
    `GuidanceStrip != nullptr`, which is demonstrably non-null. That is a C++ question, answerable
    in one PIE session with the console route, and it is separate from the visibility-binding
    question above: EITHER defect alone would leave the strip blank, and both are live.
    - **CORRECTION, same session, raised by the post-merge record gate and adopted: the sentence
      that stood here -- "nothing has been pushed into it ... one of those two links does not run"
      -- was a FALSE DICHOTOMY, and the tree's own prose says so.**
      `UStratMatchSubsystem::ApplyView`'s guided-opening block records that `Model.Guidance` is
      written by `FStratGuidedOpening::DecorateViewModel` on the decorated path and is
      DEFAULT-CONSTRUCTED on every other, and that the push is unconditional with no branch on
      `bActive`; `UStratGuidanceWidget::PushGuidance` then assigns with no guard. So an all-default
      read is equally consistent with a push that SUCCEEDED and carried nothing -- an undecorated
      rebuild, or one decorated for a different side. THREE branches, not two: `FindScoreboardHUD`
      returns null, `GuidanceStrip` is null (excluded -- it is measured non-null), or the model
      handed to `ApplyView` was never decorated. The third is the one this pass overlooked and it
      is the cheapest to test. **[SUPERSEDED -- see the newest bullet at the top of `## NEXT`. The
      exclusion of the `GuidanceStrip`-null branch was drawn from a t+2.5 s sample and does not
      hold at t+0, where it is now CONFIRMED; and the third branch is confirmed too, as a mechanism
      that clears the strip on any undecorated reconcile. Two of the three are real.]**
      What is measured is only that the widget's copy is default; nothing here establishes that no
      call was made.
  - **Driven both directions with no simulated input.** The guided opening was skipped on the model
    side and PROVED skipped: a second `ke * SkipGuidance` reported 1 instance succeeded while
    printing no "skipped by the player" line, that log sitting after the `!bActive` early return.
    An explicit `ke * RefreshFromMachine` reconcile also succeeded on the live controller. The
    screen did not move for either. `playtest_key`/`playtest_click` remain unable to drive
    anything; `ke` on a `BlueprintCallable` entry point is a route that does not need them.
  - **Consequence: §2.11.6-B IS NOT CLOSED and must still not be claimed.** What remains is two
    questions in the engineer's and editor's lanes, and NEITHER IS ASKED AS A "WHY", because a
    "why X" presupposes X and X is what is in doubt.
    (1) Does `StripBorder`'s `Visibility` binding fail to take -- or does it take while its `Active`
    input reads something other than the native `Guidance.bActive`? CONDITIONAL, and the condition
    is the thing to measure first: *if* the false pin resolves at runtime to a non-painting
    visibility -- the `Collapsed` the graph shows, or `Hidden` -- then `Active` must be reading
    something TRUE, since the border paints; a DEAD input would default false and the border would
    not be there at all. On that condition "wires to nothing" is not a sufficient mechanism for
    what is on screen, and the shadowing branch above needs a live-but-wrong source rather than an
    unfed pin. The premise is NOT measured: the `Collapsed` default was read off the graph, not at
    runtime, which is exactly the caveat the note above attaches to it, and if the false branch
    resolves to something that paints then a false `Active` explains the screen and this whole
    lead dissolves. So the cheapest measurement in this bullet is `ToVisibility`'s false-branch
    return AT RUNTIME, in the same PIE session -- it may settle question (1) outright without
    touching the shadowing hypothesis at all.
    (2) **[ANSWERED -- see the newest bullet at the top of `## NEXT`. `GuidanceStrip` was null,
    because the HUD's `BeginPlay` had not run yet. Question (1), the binding, is still open.]**
    Where does the guided-opening projection stop -- `FindScoreboardHUD` returning null,
    `GuidanceStrip` null (excluded, measured non-null), or the model reaching `ApplyView`
    UNDECORATED so that an unconditional push delivers a default? Three branches, not the two an
    earlier draft of this bullet named; the third is the cheapest to test and the one this pass
    overlooked.
    Neither is answerable from the record; both are answerable in one PIE session now that the
    console route exists.
    - **CORRECTION, same session, second gate: this sub-bullet kept BOTH retracted framings after
      the notes above were written, and it is the sentence the next lane reads as its task.** It
      asked "why a bound `Visibility` does not take", presupposing the claim demoted 30 lines
      earlier, and framed the push question as "whether it ever reaches this instance", the
      two-branch shape the other note refuted. A correction that does not reach the handoff
      sentence has not been made. Worth knowing for the next sweep: the phrase straddled a line
      break, so a line-oriented `grep "does not take"` did not return it at all -- use `rg -U`.

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

  **[CORRECTED 2026-08-22, coordinator — the exclusion reason above has gone stale by
  UNDER-claiming, which is the direction that sends the next reader to re-open a settled
  question.** "Shape unstated in the GDD by explicit decision" was true when the candidate was
  scored and is FALSE now: the Director **RULED the shape on 2026-08-20** — it is a QUERY, a
  third `ui*` function beside `uiReachable` and `uiForecast`, `uiBuildOptions(w, side,
  factoryHex)` returning `std::vector<UiBuildOption>`. Recorded in `decisions.md` and filed
  upstream in crew `spec/ui_spec.md` at `4d36a16`. **The half of the reason that still holds is
  the second half**, and it is now the whole blocker: the ruling is SPEC-ONLY. Verified against
  both trees on 2026-08-22 — crew HEAD's `cpp_reference/Ui.h` still carries the refusal
  verbatim at its Queries comment, and no `uiBuildOptions` or `UiBuildOption` symbol exists
  anywhere in `cpp_reference/`. So `buildlist_query` is NOT excluded and NOT actionable here; it
  is **blocked on an upstream header change**, after which this side re-vendors, `rulesCommit`
  moves off `cb8e12b`, and `T-INT-01` hash-matches. `production_widget` above inherits that
  blocker unchanged and stays the last feature on §4.5's hard MVP line. The crew gap over the
  vendored commit is **18 commits**, re-derived with `git rev-list --count` rather than
  incremented from the seventeen and eighteen recorded in older banners; none of the 24 vendored
  files moved in that span, so there is no re-vendor pressure yet and `Source/StratRules/` here
  is current. **[SUPERSEDED WITHIN THE HOUR BY THIS SESSION'S OWN WORK, 2026-08-22
  — the gap is NINETEEN, not the eighteen recorded one commit earlier in this same file.**
  The Director's rulings on the three residual questions were committed to the crew repo at
  `6532a46`, which is a nineteenth commit over `cb8e12b`. Re-derived with
  `git rev-list --count cb8e12b..HEAD` after that commit rather than incremented, for the same
  reason the 2026-08-21 banner gives when its own count moved. **The rest of the sentence still
  holds and is the part that matters:** `git diff --name-only cb8e12b..HEAD -- cpp_reference/`
  returns only `test_balance.cpp`, `test_replay.cpp` and `test_save.cpp`, none of which is among
  the 24 vendored files, so no vendored byte has moved, there is still no re-vendor pressure, and
  `Source/StratRules/` here is still current. A gap that grows by spec commits is not a gap that
  owes a re-vendor. **What DID change is the blocker's shape:** with `6532a46` every decision
  blocking `uiBuildOptions` is ruled, so the item is no longer waiting on a Director at all —
  it waits on one header change in `cpp_reference/Ui.h` plus two comment corrections.] **Three residual decisions the ruling deliberately did not settle are already
  filed upstream** — affordable-only vs all four rows with a module-computed `affordable`
  (upstream recommends all four), what factory state does to the answer (Q31), and whether the
  per-type population cap binds the player — and they await a Director ruling, not another
  filing.]
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
