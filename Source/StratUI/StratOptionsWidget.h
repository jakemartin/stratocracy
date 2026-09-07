// GDD Sec 2.11.5 ("No settings screen beyond volume + resolution is budgeted") -- the volume
// screen's model and its widget base. The sixth `UUserWidget` subclass in this module and the
// first one that is a CONTROL rather than a READOUT.
//
// WHAT GAP THIS CLOSES. `UStratSoundDirector::CommitVolumes` writes `UStratAudioSettings` and
// `EStratShellRoute::Options` routes a menu to a screen, and until this file there was no screen
// -- three persisted gains reachable by a Blueprint author, a console and a fixture, and by no
// player. Sec 2.11.5's budget for a settings screen is exactly "volume + resolution"; this is the
// first of those two, and the other is deliberately not here (see below).
//
// IT IS A CONTROL, WHICH IS THE ONE THING THE OTHER FIVE WIDGETS IN THIS MODULE ARE NOT, AND
// EVERY DECISION BELOW FALLS OUT OF THAT. The scoreboard, the info panel, the command bar, the
// guidance strip and the result screen are all fed a model by something above them and give
// nothing back. This one is fed a model AND produces a value. So it needs a second direction,
// and the second direction is where a widget usually starts computing:
//   - THE SETTERS CLAMP AND REBUILD, AND THE MODEL IS WHAT THE SCREEN DRAWS. A slider that drew
//     its own thumb position from its own local state would be a number on screen that is not a
//     model field --
//     RETRACTED> "which is `T-UI-03`'s clause with the arrow reversed"
//     -- which is the RULE `T-UI-03` STATES with the arrow reversed. The ID itself is refused
//     for this surface; see this header's `AMENDED 2026-09-07 (SECOND PASS)` block. Every
//     setter's last act
//     is to replace `Model` and fire `OnAudioOptionsRefreshed`, so the WBP redraws from the model
//     it was just handed -- exactly as the read-only widgets do.
//   - THE PERCENT STRINGS ARE FIELDS AND NOT `Volume * 100` NODES. That multiplication is
//     arithmetic, it is the single most likely piece of arithmetic to appear in a volume WBP, and
//     `StratBuildAudioOptionsModel` does it once in C++ so the graph never has to. It is the
//     same move `FStratMatchResultModel::bViewerWon` makes about `Winner == ViewingSide`.
//   - NOTHING HERE APPLIES A VOLUME. `UGameplayStatics::SetSoundMixClassOverride` needs a
//     `USoundMix` and a `USoundClass`, both of which arrive on `UStratSoundBank` in `StratPlay`,
//     and the arrow runs `StratPlay -> StratUI` and never back. So this class cannot apply
//     anything even if it wanted to, which is a structural guarantee rather than a discipline.
//
// HOW THE VALUE GETS OUT, AND IT IS A DELEGATE BECAUSE IT CANNOT BE A CALL. `OnAudioOptionsCommitted`
// is a `BlueprintAssignable` multicast carrying the whole model; a `StratPlay`-side owner binds it
// to `UStratSoundDirector::CommitVolumes` and the join costs no `#include` in either direction.
// The two shapes that were rejected:
//   - THE WBP CALLING `Get World Subsystem -> Commit Volumes` DIRECTLY. It works, and it makes
//     the asset the thing that decides when a value is committed. Then the tested path and the
//     shipped path are different, and the difference lives in a `.uasset` no clause reads.
//   - A `BlueprintImplementableEvent` PER CHANNEL. Three events, each carrying one float, each
//     wired by hand -- three chances to wire the music slider to the SFX gain, with nothing able
//     to tell. One event carrying one model has no such degree of freedom.
//
// THE BINDER DOES NOT EXIST YET, AND THAT IS SAID HERE RATHER THAN LEFT TO BE DISCOVERED,
// BECAUSE THIS PROJECT HAS A NAMED DEFECT FOR IT: "a reflected verb with no caller reads as
// built". As of this file's first commit nothing binds `OnAudioOptionsCommitted` and no WBP
// derives from this class, so a player who reaches `EStratShellRoute::Options` today sees
// nothing. What exists is the whole path from a slider value to a written slot, with every step
// reachable by a clause. DISCHARGED BY the WBP and an owner -- `AStratShellHUD` is the obvious
// one, since it already creates the menu on `MenuWidgetClass` and already holds the shell -- that
// creates this widget on `IsOptionsPanelOpen` and binds the delegate. That owner is a later
// phase's and is not this file's to write.
//   THE OWNER LANDED 2026-09-05 AND IT IS NOT THE ONE THIS PARAGRAPH GUESSED, WHICH IS WORTH
// SAYING BECAUSE THE GUESS WAS WRONG FOR A STRUCTURAL REASON RATHER THAN A PREFERENCE.
// `AStratShellHUD` runs on the TITLE map only; the MATCH map's HUD is `AStratScoreboardHUD`,
// which is in THIS module and therefore cannot name `UStratShellSubsystem` at all -- the arrow
// runs `StratPlay -> StratUI` and never back. A HUD-shaped owner could only ever have shown this
// screen at the title. `UStratOptionsPresenter`, a `UWorldSubsystem` in `StratPlay`, is the owner
// instead, and it exists in both worlds; its file header carries the full derivation. What is
// still owed is the WBP.
//   THAT LAST SENTENCE WAS FALSE ON THE DAY IT WAS COMMITTED, WHICH IS A DIFFERENT AND MORE
// USEFUL FACT THAN ITS HAVING GONE STALE, SO IT IS STAMPED RATHER THAN QUIETLY REWRITTEN.
// `Content/UI/WBP_Options.uasset` was ADDED by `d80b28a`; the paragraph above it, headed
// "THE OWNER LANDED 2026-09-05", arrived in `374398a`. Both commits carry the same calendar
// date, so the date does not settle the order and ancestry does:
// `git merge-base --is-ancestor d80b28a 374398a` succeeds, and `git log --reverse -S "still
// owed is the WBP"` over this file returns `374398a` AND NOTHING ELSE, which is what makes it
// the sentence's ORIGIN -- a one-line result has no first-versus-last left to get wrong.
// `--reverse` IS KEPT BECAUSE IT IS THE RIGHT HABIT FOR AN ORIGIN QUESTION, NOT BECAUSE IT BIT
// HERE: run both ways this query returns that same single commit, so the recorded rule about
// the plain form answering most-recent is inert on it and no sentence here should imply
// otherwise. The asset's own arrival is a separate instrument and it is the one that carries
// the weight: `git log --diff-filter=A -- Content/UI/WBP_Options.uasset` names `d80b28a`. The
// asset was therefore already in the tree when the sentence claiming it was owed was written.
// A sentence wrong at the moment of writing is not an aging problem; it is a claim its author
// carried forward without checking, and the check was that one `--diff-filter=A` away.
//   WHAT IS TRUE NOW, each part measured rather than asserted. The asset exists and is tracked.
// It derives from `UStratOptionsWidget`. And `OptionsWidgetClass` names it on ALL THREE shipped
// GameModes -- the title map's, the match map's and the AI-vs-AI demo's. None of that rests on
// this comment being read, because it is pinned by a named clause:
// `Stratocracy.StratPlay.GATE-TITLEMENU.AllThreeShippedGameModesNameOneOptionsWidgetClass`, in
// `Source/StratPlay/Tests/StratShippedOptionsWidgetClassParity.cpp`. That clause reads each
// GameMode CDO through the REFLECTED `OptionsWidgetClass` property rather than through a cast
// to one native base (the three assets do not share one), fails rather than passing when it
// cannot tell, refuses the agreement that three UNSET GameModes would otherwise produce, and
// finishes on `IsChildOf(UStratOptionsWidget::StaticClass())` against the agreed class.
//   SO THE DEBT STATED ABOVE AS "DISCHARGED BY the WBP and an owner ... that creates this
// widget on `IsOptionsPanelOpen` and binds the delegate" IS DISCHARGED IN BOTH HALVES: the
// owner landed as `UStratOptionsPresenter`, and the WBP landed one commit earlier than the
// paragraph that said it had not. Read the whole run above as history.
//   THE `AMENDED 2026-09-05` BLOCK BELOW IS UNAFFECTED AND IS DELIBERATELY NOT BEING CORRECTED
// HERE, WHICH IS SAID SO THAT ITS SILENCE IS NOT LATER READ AS AGREEMENT WITH THE SENTENCE
// ABOVE. It argues how the asset must be AUTHORED -- a widget tree only, no event graph, the
// `BindWidget` names as the whole contract -- and it never claims the asset had not landed. It
// reads correctly against a tree that has one. It is silent on the question, not wrong about it.
// The same goes for the time-qualified sentence higher up ("As of this file's first commit ...
// no WBP derives from this class"): its qualifier is what saves it, and this stamp is what
// closes it.
//
// NOT IN THIS ROUND, with reasons:
// - RESOLUTION, WHICH IS THE OTHER HALF OF Sec 2.11.5's OWN SENTENCE. It is `UGameUserSettings`:
//   the engine owns it, persists it in `GameUserSettings.ini`, and enumerates the modes a display
//   actually supports. A `TArray<FIntPoint>` composed here would be a second, worse copy of that
//   enumeration, and `UStratAudioSettings`'s block refuses the same duplication from the save
//   side. When it lands it is rows on this screen and a second model section, not a second route.
// - AUDIO OUTPUT DEVICE, SUBTITLES, MASTER MUTE. Not budgeted, and the mute in particular is
//   `MasterVolume == 0` -- a second representation of it would be two answers to "is this
//   silent", which `EStratSoundDisposition` already refuses one layer down.
// - A "RESET TO DEFAULTS" BUTTON. It is one line (`SetAll(1, 1, 1)`) and it is left out because
//   `FStratAudioOptionsModel`'s defaults ARE unity, so the button would be the only control on
//   this screen whose meaning a reader has to look up. A WBP that wants one calls the three
//   setters.
// - LOCALIZATION. `FText::FromString` throughout, matching `StratResultLineFor` and
//   `StratGuidedOpening::DirectiveTextFor`; see `FStratGuidanceView`'s block on why localization
//   is deliberately not started.
//
// ===========================================================================================
// AMENDED 2026-09-05 -- THE CLASS NOW BINDS ITS OWN SUB-WIDGETS AND WIRES ITS OWN DELEGATES,
// AND THE REASON IS A MEASURED LIMIT OF THE ONLY AVAILABLE AUTHORING ROUTE RATHER THAN A
// PREFERENCE.
//
// The paragraph above headed "HOW THE VALUE GETS OUT" assumed the WBP's own graph would wire a
// slider's `OnValueChanged` to `SetMasterVolume`. THAT GRAPH CANNOT BE AUTHORED FROM THIS
// PROJECT'S TOOLING, and the boundary was measured rather than assumed: a widget TREE can be
// built headlessly (`unreal.new_object(unreal.Slider, outer=tree)` succeeds, properties set,
// `PanelWidget.add_child` is a Python method), but `UWidgetTree::RootWidget` is a PROTECTED
// `UPROPERTY` and Python reports *"Property 'RootWidget' ... is protected and cannot be read"*,
// so an empty widget cannot be given a root and no event graph can be authored beside the tree.
// A wiring convention that only a human in the editor can satisfy is a wiring convention this
// project cannot check, which is the same objection the header already makes to "the WBP calling
// `Get World Subsystem -> Commit Volumes` directly": the shipped path would live in a `.uasset`
// no clause reads.
//
// SO THE WIRING MOVES INTO `NativeConstruct`, WHICH MAKES IT TESTABLE FOR THE FIRST TIME. The
// asset's remaining job is to CONTAIN widgets with the declared names and nothing else -- no
// nodes, no bindings, no arithmetic. What the asset can still get wrong is a missing or
// misnamed child, and `BindWidget` turns that into a Blueprint compile error naming the member.
//
// WHICH BINDINGS ARE HARD AND WHICH ARE OPTIONAL, AND THE LINE IS PRINCIPLED RATHER THAN
// AESTHETIC: **a widget that PRODUCES a value is `BindWidget`; a widget that only DISPLAYS one
// is `BindWidgetOptional`.** The three sliders and the back button are the only things on this
// screen that can originate a player's intent, and nothing else in this class can substitute for
// them -- a WBP missing `MasterSlider` is a volume screen with no volume control, and failing its
// compile is the correct, loud outcome. The three value texts are NOT the only sanctioned route
// to the percentages: `OnAudioOptionsRefreshed` hands the whole model to the graph, so an asset
// may draw `MasterVolumeText` through a designer-authored `UTextBlock` under any name, or in a
// larger composed line, or not at all. A hard bind on a readout would forbid all three of those
// for no gain and would fail the compile of the whole screen over a label.
//
// NOTHING HERE CLOSES THE PANEL, AND THE BRIEF THAT ASKED FOR IT NAMED AN IMPOSSIBLE CALL. The
// back button was specified as reaching `UStratShellSubsystem::CloseOptionsPanel()`. That class
// is in `StratPlay` and the arrow runs `StratPlay -> StratUI` and never back, so this module
// cannot name it -- the same structural refusal the header already records about `USoundMix`.
// `OnOptionsDismissed` is the outward-facing half instead, on `OnAudioOptionsCommitted`'s exact
// precedent: a dynamic multicast a `StratPlay`-side owner binds, costing no `#include` in either
// direction.
//
// THE SLIDER RANGE IS SET FROM C++ AND NOT LEFT TO THE ASSET. `StratClampVolume` is the one
// authority on the gain domain and it is `[0, 1]`; a `USlider` authored `0..100` would submit
// `73` to a clamp that answers `1.0`, so every position above the first percent would read
// "100%" and the screen would look broken in a way no clause could see. Overriding the asset's
// `MinValue`/`MaxValue` in `NativeConstruct` costs an author the ability to choose a range this
// class could not honour anyway.
//
// ===========================================================================================
// AMENDED 2026-09-07 -- A SECOND WAY OFF THIS SCREEN, AND IT IS A DIFFERENT KIND OF EXIT FROM
// THE BACK BUTTON RATHER THAN A SECOND COPY OF IT.
//
// WHAT GAP THIS CLOSES. The user asked for "a quit button in the options screen during
// gameplay". The title menu's Quit row already exists and closes the process; this screen had
// only `BackButton`, which puts the player back exactly where they were. So a player inside a
// match had no control anywhere on screen that leaves the match. The user then RULED which
// exit this is: **return to the title, not quit to desktop** -- the title menu's own Quit row
// closes the process from there. That ruling is recorded here because this file's control is
// where a later reader would otherwise wonder why the "quit button" does not quit.
//
// IT IS A DELEGATE AND NOT A CALL, ON `OnOptionsDismissed`'S EXACT PRECEDENT AND FOR THE SAME
// STRUCTURAL REASON. The brief that produced this control named `EStratShellRoute::ReturnToTitle`
// and `UStratShellSubsystem::ExecuteRoute`. **This module cannot name either.** Both live in
// `StratPlay`, the arrow runs `StratPlay -> StratUI` and never back, and the paragraph above
// headed "NOTHING HERE CLOSES THE PANEL" already records the identical refusal about
// `CloseOptionsPanel()`. `OnReturnToTitleRequested` is the outward-facing half instead: a
// dynamic multicast a `StratPlay`-side owner binds, costing no `#include` in either direction.
// This class does not know what a "title" is beyond a word on a button.
//
// THE HARD PART IS NOT THE EXIT, IT IS THAT THE EXIT IS SOMETIMES ILLEGAL, AND THE SCREEN IS
// REACHABLE FROM BOTH SIDES OF THAT. `UStratOptionsPresenter` is a `UWorldSubsystem` and exists
// in the TITLE world as well as the match world, and `UStratShellMenuWidget::HandleOptionsClicked`
// takes `EStratShellRoute::Options` from the title menu -- so this screen is opened on a map
// where `IsRoutePermitted(ReturnToTitle, ...)` refuses with "No match in progress." A control
// that a player can click and that then silently does nothing is the defect this whole options
// route was written to avoid: `UStratShellSubsystem`'s own header calls an armed-but-dead row
// "one step worse than a greyed row".
//
// SO THE AVAILABILITY IS PUSHED IN, ON THE MODEL PRECEDENT THIS FILE ALREADY USES FOR GAINS.
// `FStratOptionsExitModel` carries a bit and a sentence; `PushExitOptions` stores it and
// `SyncExitWidgetsToModel` sets `bIsEnabled` on the button and the sentence on an optional
// label. The bit is NOT computed here and could not be -- `IsRoutePermitted` is the single
// authority, it is a `StratPlay` static, and `BuildMenuModel` already reads it the same way for
// the title menu's five rows. **A disabled `UButton` does not broadcast `OnClicked`**, so
// "cannot be clicked when it would refuse" is a property of the control rather than a check
// this class performs. THE CONSEQUENCE, SAID RATHER THAN DISCOVERED: an owner that never pushes
// leaves the control disabled forever, which is why the default is `false` and why the
// presenter pushes on the same path that seeds the gains.
//
// A SEPARATE STRUCT AND NOT THREE MORE FIELDS ON `FStratAudioOptionsModel`, AND THIS IS THE ONE
// DECISION HERE THAT A REVIEWER SHOULD CHECK RATHER THAN ACCEPT. That struct is REBUILT FROM
// THREE FLOATS by `StratBuildAudioOptionsModel` on every setter -- `SetMasterVolume` assigns
// `Model = StratBuildAudioOptionsModel(...)` -- so any field added to it that the builder does
// not take as an argument would be SILENTLY RESET TO ITS DEFAULT BY EVERY SLIDER DRAG. The exit
// row would have gone grey the first time a player touched the volume, and nothing in this file
// would have looked wrong. Two structs have no such coupling.
//
// NOT IN THIS ROUND: A CONFIRMATION PROMPT. Leaving a match discards unsaved progress and a
// "are you sure" step is the obvious guard, but it is a second screen with its own two controls
// and its own permission question, and Sec 2.11.5 budgets one settings screen. Said here so
// that its absence reads as a decision rather than an oversight.
//
// ===========================================================================================
// AMENDED 2026-09-07 (SECOND PASS) -- THE NAME `T-UI-03` IS RETRACTED FROM THIS FILE'S PROSE,
// IN ALL FIVE PLACES IT APPEARS. THE RULE IT NAMED IS UNCHANGED AND STILL BINDS EVERY LINE
// BELOW; WHAT WAS WRONG WAS THE ATTRIBUTION, AND THE FIVE SITES CARRY THEIR OWN `RETRACTED>`
// MARKERS SO THAT A READER ARRIVING AT ONE OF THEM BY CITATION MEETS THE CORRECTION THERE
// RATHER THAN HERE.
//
// WHAT THE ID ACTUALLY OWNS, quoted rather than paraphrased. `T-UI-03`'s defining sentence, as
// `Tools/architect/state/global.md`'s third acceptance-ID ruling of 2026-09-05 quotes it, is
// *"no widget-side arithmetic; every number a widget draws equals exactly one
// `strat::UiSnapshot` field."* Both halves are load-bearing and only the FIRST half is what this
// file was invoking. The second half is a quantifier over `strat::UiSnapshot`, and NOTHING ON
// THIS SCREEN HAS ONE BEHIND IT: a player's audio gain is a saved preference, and the exit
// row's sentence is written by `UStratShellSubsystem::IsRoutePermitted` one module over. A
// surface with no snapshot field gives that ID, in the ruling's words, *"no snapshot-fidelity
// fact ... to own"*.
//
// THE RULING REFUSED THE ID FOR THIS EXACT CLASS BY NAME, WHICH IS WHY THIS IS A CORRECTION AND
// NOT A PREFERENCE. It turned `T-UI-03` down for `UStratCommandBarWidget::OptionsButton` on the
// ground that a control which *"draws no value and reads no snapshot field"* has no such fact,
// and it names *"`UStratOptionsWidget`'s bound sub-widgets"* among the six subjects it covers.
// Its deciding sentence transfers directly: *"Sitting on the SAME command bar as clauses that do
// ride `T-UI-03` does not pull `OptionsButton` onto that ID; the test is the subject sentence,
// not the file."* Sitting in the same MODULE as the scoreboard is the same non-argument.
//
// SO WHAT THE FIVE SITES SHOULD HAVE SAID, and it is one substitution everywhere: the discipline
// is *the rule `T-UI-03` states*, applied here by analogy to a surface that does not ride that
// ID. This file names no acceptance ID for its own surface, and must not -- picking one is the
// steward's ruling and the test lane's clause names, never a header comment's.
//
// WHERE THE CLAUSES ACTUALLY LANDED, attributed and not vouched for. `strat-test-author`
// declined `T-UI-03` for this surface and wrote the exit row's clauses under `GATE-TITLEMENU`
// instead, on that same ruling; its reasoning is in `Source/StratUI/Tests/StratOptionsExitClauses.cpp`'s
// own header, which is the authority for it. **As of this comment those clauses are uncompiled
// and unrun** -- the editor holds the DLLs -- so this paragraph records where they were filed
// and claims nothing about their result. No count of them appears here or anywhere outside
// `global.md`.
//
// TWO OF THE FIVE SITES WERE WRITTEN BY THE EXIT-ROW PASS AND THREE PREDATE IT, WHICH MATTERS
// BECAUSE IT SAYS THE SLIP IS OLDER THAN THE DEFECT REPORTED.
//
//   RETRACTED> "Measured: `git diff -U0` over this file and its `.cpp` against `46321a6`
//   RETRACTED>  returns exactly two added lines containing the token"
//
// THAT INSTRUMENT MEASURED A THING THIS BLOCK MOVES, AND NO VALUE OF IT IS WRITTEN HERE AS A
// STANDING FACT. `strat-integration-reviewer` re-ran the quoted command -- `git diff -U0 46321a6
// -- <this file> <its .cpp> | grep '^+' | grep` for the token -- on 2026-09-07 and got 16, not
// two, because every one of the five sites was REWRITTEN by this correction (five `RETRACTED>`
// quotations plus five replacement sentences) and the block adds six more mentions on its own.
// Then the repair of THAT finding -- this paragraph and the two below -- took the same command to
// 19 the same day, and rewriting THIS VERY SENTENCE dropped one mention and left it at 18. That
// last figure is quotable at all only because the edit stating it changes digits and prose and
// adds no further occurrence of the token, so it cannot move itself; re-measured after that edit,
// the command still prints 18. The finding was the reviewer's and it is right, and the 16-19-18
// sequence is its proof: a diff-of-the-working-tree instrument counts every correction along with
// its subject, and there is no fixed point to quote.
//
// THE INSTRUMENT IS THEREFORE SCOPED TO THE BASE, WHICH NOTHING WRITTEN IN THIS TREE CAN MOVE.
// Re-run 2026-09-07: `git show 46321a6:Source/StratUI/StratOptionsWidget.h | grep -c 'T-UI-03'`
// prints `2`, and the same command on `...StratOptionsWidget.cpp` prints `1`. Three occurrences
// at the base, five sites in the tree, so two of the five are new. The base's three, by
// `git show 46321a6:<file> | grep -n 'T-UI-03'`, are the header's line 18 ("arrow reversed") and
// line 390 (`MasterValueText`), and the `.cpp`'s line 238 (`SyncBoundWidgetsToModel`) -- all three
// on the GAIN surface. The two the exit-row pass added are therefore
// `FStratOptionsExitModel::ReturnToTitleReason` here and `SyncExitWidgetsToModel` in the `.cpp`.
// CONTROL: `git show 46321a6:Source/StratUI/StratOptionsWidget.h | grep -c 'T-UI-99'` prints `0`,
// so the grep is able to return zero and a `2` is a reading rather than a fixed point.
//
// The other three (the header's "arrow reversed" line, `MasterValueText`, and
// `SyncBoundWidgetsToModel`) are on the GAIN surface and were already in the tree. They are
// corrected in the same pass rather than left, because a correction that fixed two occurrences
// of a refused name while leaving three identical ones in the same two files would rebuild the
// exact trap it was written to close -- and the same ruling refuses the ID for the gain surface
// too, citing the argument `strat-test-author` had already made about the options MODEL.
//
// THE REPORT THAT PRODUCED THIS BLOCK WAS RIGHT IN SUBSTANCE AND WRONG IN ONE DETAIL, RECORDED
// SO THAT THE NEXT READER DOES NOT RE-DERIVE IT. It said the header names the ID *"twice"* on
// the exit surface. The header names it three times in total and only ONE of those is the exit
// row; the second "on this surface" occurrence is `MasterValueText`, a volume readout the exit
// pass never touched. The finding survives the correction -- that site is false for the same
// reason -- but the count and the subject were not the same question.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "StratOptionsWidget.generated.h"

class UButton;
class USlider;
class UTextBlock;

/**
 * Everything the volume screen draws, in one value.
 *
 * THE THREE GAINS AND THE THREE STRINGS THAT DISPLAY THEM. The pairing is deliberate duplication,
 * on `FStratScoreboardRow`'s stated precedent: the number is what a clause compares and the
 * string is what the screen shows, and both are set from one read of one value so they cannot
 * disagree. A screen that had only the float would compute the string; a screen that had only the
 * string could not be asserted numerically.
 *
 * EVERY FIELD IS `BlueprintReadOnly`. `UStratOptionsWidget`'s three setters are the only writers,
 * so a graph cannot fabricate a displayed gain that no clamp ever saw.
 */
USTRUCT(BlueprintType)
struct FStratAudioOptionsModel
{
	GENERATED_BODY()

	/** `[0, 1]`, guaranteed by `StratClampVolume` -- see `StratBuildAudioOptionsModel`. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	float MasterVolume = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	float SfxVolume = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	float MusicVolume = 1.0f;

	/** `MasterVolume` as the screen says it -- "100%", "45%", "0%". THE MULTIPLICATION HAPPENS
	 *  HERE AND NOT IN A GRAPH; see the file header. */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	FText MasterVolumeText;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	FText SfxVolumeText;

	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	FText MusicVolumeText;
};

/**
 * Whether the player may leave the match from this screen, and why not when they may not.
 *
 * A SECOND STRUCT AND NOT THREE MORE FIELDS ON `FStratAudioOptionsModel` -- see the file header.
 * The short version, because it is the one thing here that can silently go wrong:
 * `StratBuildAudioOptionsModel` REBUILDS that struct from three floats on every setter, so a bit
 * parked there would be cleared by every slider drag.
 *
 * THE BIT AND THE SENTENCE ARE ONE PUSH AND NEVER TWO, on `FStratShellOption`'s exact pairing of
 * `bEnabled` with `DisabledReason`: they are two halves of one answer from one call to one
 * decider, and pushing them separately would let a screen show "enabled" beside the reason it is
 * not.
 *
 * IT DOES NOT NAME A ROUTE, AND CANNOT. `EStratShellRoute` is a `StratPlay` type. What this
 * struct says is "the exit control is live"; what that control MEANS is the owner's to decide.
 */
USTRUCT(BlueprintType)
struct FStratOptionsExitModel
{
	GENERATED_BODY()

	/**
	 * True when the owner has said the exit is legal right now.
	 *
	 * DEFAULT `false`, WHICH IS THE RESTRICTIVE DIRECTION AND IS CHOSEN DELIBERATELY. An owner
	 * that never pushes leaves a dead control on the screen; an owner that never pushes with the
	 * opposite default leaves a LIVE control that broadcasts into nothing. This project has a
	 * named defect for the second shape ("a reflected verb with no caller reads as built") and
	 * none for the first, because a greyed control is visibly not offering anything.
	 *
	 * IT IS ALSO NOT AN "UNSET" SIGNAL AND MUST NOT BE READ AS ONE. `false` here means exactly
	 * "not available", whether that is because a title-map presenter pushed a refusal or because
	 * nobody pushed at all. If a clause ever needs to tell those apart, the instrument is the
	 * owner's own counter and not this field.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	bool bReturnToTitleEnabled = false;

	/**
	 * Why the exit is greyed, in the player's words. Empty exactly when `bReturnToTitleEnabled`.
	 *
	 * IT IS THE SENTENCE `UStratShellSubsystem::IsRoutePermitted` PRODUCED AND IS NEVER COMPOSED
	 * HERE -- the same one a greyed title-menu row shows, because it is the same call.
	 *
	 * RETRACTED> "`T-UI-03`'s clause on this surface: one field, one drawn string, no expression
	 * RETRACTED>  between them."
	 *
	 * The discipline holds and the NAME does not: one field, one drawn string, no expression
	 * between them, which is the RULE `T-UI-03` states applied by analogy. This surface does not
	 * ride that ID -- there is no `strat::UiSnapshot` behind a route permission, so the ID has no
	 * fidelity fact here to own. See this file's `AMENDED 2026-09-07 (SECOND PASS)` block.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Stratocracy|Options")
	FText ReturnToTitleReason;
};

/**
 * The model for three gains. THE ONLY PLACE a volume becomes a string in this project.
 *
 * A FREE FUNCTION AND NOT A METHOD ON THE WIDGET, on `StratBuildScoreboardModel`'s stated reason:
 * it draws nothing, holds no state, needs no Slate in existence, and can therefore be swept over
 * every interesting input in a `-nullrhi` clause with no widget constructed at all.
 *
 * IT CLAMPS THROUGH `StratClampVolume` AND DOES NOT RESTATE THE RANGE. That function is the one
 * authority and `UStratAudioSettings::Sanitize` in `StratPlay` is its other caller; the header on
 * that declaration records why the rule has to live below both modules.
 *
 * THE PERCENT IS `FMath::RoundToInt(Volume * 100)`, WHICH IS THE ARITHMETIC THIS FUNCTION EXISTS
 * TO CONTAIN. It rounds rather than truncating, so a gain of 0.999 reads "100%" instead of "99%"
 * -- a slider dragged to its stop must not report one short of full, and truncation is the way
 * that bug is usually written. The consequence, said rather than discovered: a gain of 0.996 also
 * reads "100%" while not being 1.0, so the STRING is not a faithful inverse of the float and no
 * caller should treat it as one. The float is the value; the string is the label.
 *
 * TOTAL AND INFALLIBLE. Every triple of floats has a model, including NaNs -- `StratClampVolume`
 * maps those to silence and states why.
 */
STRATUI_API FStratAudioOptionsModel StratBuildAudioOptionsModel(float InMasterVolume,
                                                                float InSfxVolume,
                                                                float InMusicVolume);

/**
 * Fired when a setter has accepted a value. Carries the whole model, never one float.
 *
 * A DYNAMIC MULTICAST SO A BLUEPRINT *AND* A `StratPlay`-SIDE OWNER CAN BOTH BIND. The shipped
 * binder is expected to be C++ (see the file header); the delegate is dynamic anyway so that an
 * asset is not locked out of a screen whose whole purpose is asset-facing.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStratAudioOptionsCommitted,
                                            const FStratAudioOptionsModel&, Model);

/**
 * Fired when the player asked to leave the screen. Carries nothing.
 *
 * NO PAYLOAD, DELIBERATELY. "Close me" is not a value and the last committed model has already
 * been broadcast by whichever setter produced it; a dismissal carrying a model would be a second
 * copy of a value an observer already has, and the observer would have to decide whether the two
 * agree. The one thing this event means is that a `StratPlay`-side owner should call
 * `UStratShellSubsystem::CloseOptionsPanel` -- see the file header on why this class cannot.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStratOptionsDismissed);

/**
 * Fired when the player asked to leave the match and go back to the title. Carries nothing.
 *
 * A SECOND DELEGATE AND NOT A PAYLOAD ON `FStratOptionsDismissed`, WHICH WAS THE OTHER SHAPE.
 * "Close this screen" and "leave the match" are different requests with different consequences
 * -- the first is free and the second discards a match -- and a bool on one event would mean
 * every existing binder had to start distinguishing them, silently, with the default doing the
 * more destructive thing on any binder that did not. Two events have no such degree of freedom,
 * which is `FStratAudioOptionsCommitted`'s stated argument against three per-channel events run
 * the other way.
 *
 * NO PAYLOAD, on `FStratOptionsDismissed`'s reason: "leave" is not a value. The one thing this
 * event means is that a `StratPlay`-side owner should take `EStratShellRoute::ReturnToTitle` --
 * see the file header on why this class cannot name that.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStratOptionsReturnToTitleRequested);

/**
 * Sec 2.11.5's volume screen.
 *
 * Holds a built model, pushes it at Blueprint, and reports every accepted change. It reads no
 * `strat` type, applies no gain, and performs no arithmetic -- see this file's header block for
 * why each of those is an acceptance criterion rather than a style preference.
 *
 * Abstract and Blueprintable because the WBP_ asset derives from it and does the layout.
 * `StratUI.Build.cs` records the irreversible part: the derived asset bakes
 * `/Script/StratUI.StratOptionsWidget` into itself, so this class's module and name are expensive
 * to change once the first asset exists.
 */
UCLASS(Abstract, Blueprintable, meta = (DisplayName = "Strat Options"))
class STRATUI_API UStratOptionsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Replaces `Model` wholesale and fires `OnAudioOptionsRefreshed`. Does NOT fire
	 * `OnAudioOptionsCommitted`.
	 *
	 * THE ASYMMETRY IS THE WHOLE CONTRACT AND IT IS THE EASIEST THING IN THIS FILE TO GET WRONG.
	 * This is how the screen is SEEDED -- an owner reads `UStratSoundDirector::GetAudioSettings`
	 * and pushes what the player chose last time. Firing the commit delegate here would make
	 * opening the screen write the slot, so a player who opened options and closed it again would
	 * have re-saved values they never touched, and any clause counting commits would count that
	 * one. Seeding is not choosing.
	 *
	 * NOT A `UFUNCTION`, on `UStratMatchResultWidget::PushMatchResult`'s reasoning: every argument
	 * is reflectable, so nothing forces this to be plain C++; it is plain C++ so that Blueprint
	 * has no way to seed a model that did not come from the settings object.
	 *
	 * IT CLAMPS, BECAUSE A CALLER CAN HAND IT ANYTHING. The model it stores is always built by
	 * `StratBuildAudioOptionsModel`, never assigned from the argument, so there is no path by
	 * which an out-of-range gain reaches `Model` and gets drawn.
	 */
	void PushAudioOptions(const FStratAudioOptionsModel& InModel);

	/**
	 * Replaces `ExitModel` wholesale, redraws the exit control, and fires `OnExitOptionsRefreshed`.
	 *
	 * IT IS PURELY A SEED AND HAS NO COMMITTING TWIN, WHICH IS THE ASYMMETRY WITH THE GAIN SIDE.
	 * `PushAudioOptions` is one of two ways `Model` changes; this is the ONLY way `ExitModel`
	 * changes. There is no `SetReturnToTitleEnabled` and there must not be one -- a screen that
	 * could enable its own exit would be deciding a permission that `IsRoutePermitted` is the
	 * single authority for, one module over.
	 *
	 * NOT A `UFUNCTION`, on `PushAudioOptions`'s stated reasoning: every argument is reflectable,
	 * so nothing forces this to be plain C++; it is plain C++ so that Blueprint has no route to
	 * enable an exit that no decider permitted.
	 *
	 * IT COPIES RATHER THAN REBUILDING, AND THE DIFFERENCE FROM `PushAudioOptions` IS PRINCIPLED.
	 * That function rebuilds because it can: `StratBuildAudioOptionsModel` is a total function
	 * this module owns, so it can re-derive the strings from the floats and refuse to trust the
	 * caller's pairing. There is no such function here and there could not be -- the bit comes
	 * from a decider this module cannot call -- so this class has nothing to check the pair
	 * against and says so rather than pretending to validate.
	 */
	void PushExitOptions(const FStratOptionsExitModel& InModel);

	/**
	 * Set the master gain. Clamps, replaces `Model`, then fires BOTH events.
	 *
	 * `BlueprintCallable` BECAUSE A SLIDER IS THE CALLER, which is the one reason any writer in
	 * this module is reflected. The three setters are separate functions rather than one
	 * `SetVolumes(float, float, float)` because a WBP wires one slider to one node, and a
	 * three-argument setter would make each slider responsible for restating the other two --
	 * three chances per drag to write a stale value over a fresh one.
	 *
	 * REFRESH FIRES BEFORE COMMIT, AND THE ORDER IS NOT ARBITRARY. The screen should show the
	 * accepted value before anything acts on it, so that a commit which is slow, refuses, or
	 * (today) is bound to nothing at all cannot leave the thumb somewhere the model is not.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Options")
	void SetMasterVolume(float InVolume);

	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Options")
	void SetSfxVolume(float InVolume);

	UFUNCTION(BlueprintCallable, Category = "Stratocracy|Options")
	void SetMusicVolume(float InVolume);

	/** The last model. Everything the WBP draws comes from here.
	 *
	 *  READ-ONLY TO BLUEPRINT ON PURPOSE, on `UStratMatchResultWidget::Model`'s reasoning: the
	 *  four writers above are the only ones, and a writable property would let a graph draw a
	 *  gain that no clamp had seen and no setter had reported. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Options")
	FStratAudioOptionsModel Model;

	/** The last pushed exit availability. `PushExitOptions` is the only writer.
	 *
	 *  READ-ONLY TO BLUEPRINT FOR `Model`'S REASON, WITH MORE AT STAKE: a writable bit here would
	 *  let a graph light up an exit that `IsRoutePermitted` had refused, which is exactly the
	 *  "enabled button wired to an impermissible route" content defect `ExecuteRoute`'s own
	 *  permission re-check exists to survive. */
	UPROPERTY(BlueprintReadOnly, Transient, Category = "Stratocracy|Options")
	FStratOptionsExitModel ExitModel;

	/**
	 * Fired after `Model` is replaced, by a seed OR by a setter. Redraw here.
	 *
	 * The model is passed as an argument as well as being readable off the widget, for the reason
	 * `OnScoreboardRefreshed`, `OnGuidanceRefreshed` and `OnMatchResultRefreshed` all give: so a
	 * graph has no motive to cache its own copy, a cached copy being a value that can go stale.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Stratocracy|Options")
	void OnAudioOptionsRefreshed(const FStratAudioOptionsModel& InModel);

	/**
	 * Fired after `ExitModel` is replaced. Redraw the exit row here.
	 *
	 * A SECOND EVENT AND NOT AN EXTRA ARGUMENT ON `OnAudioOptionsRefreshed`, because the two fire
	 * at different rates for different reasons: the gain model is replaced on every drag and the
	 * exit model is replaced once, when the panel is shown. Folding them would make a graph
	 * redraw a permission it has been handed unchanged sixty times a second, and -- worse -- would
	 * change an existing event's signature, silently un-wiring whatever `WBP_Options` already
	 * binds to it.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Stratocracy|Options")
	void OnExitOptionsRefreshed(const FStratOptionsExitModel& InModel);

	/**
	 * Fired after a SETTER, and never after `PushAudioOptions`. Bind this to persist and apply.
	 *
	 * THE COUNT OF THESE IS THE MEASURABLE THING A CLAUSE SHOULD PIN. "The screen shows 0.4" is a
	 * statement about `Model`; "the screen ASKED for 0.4 exactly once" is a statement about this
	 * delegate, and only the second can tell a seeded screen from a used one -- which is the
	 * distinction `PushAudioOptions` exists to preserve.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Stratocracy|Options")
	FStratAudioOptionsCommitted OnAudioOptionsCommitted;

	/**
	 * Fired when `BackButton` was clicked. The owner closes the panel; this class does not.
	 *
	 * IT DOES NOT REMOVE ITSELF FROM THE VIEWPORT, AND THAT IS THE SAME SPLIT `AStratShellHUD`
	 * MAKES ABOUT ITS MENU. Whoever added a widget to the screen is the one thing that knows
	 * what else is on it and what should be there instead; a widget that tears itself down
	 * leaves its owner holding a pointer to something no longer drawn, and this project already
	 * records a defect of exactly that shape ("a blank widget may still be the widget").
	 */
	UPROPERTY(BlueprintAssignable, Category = "Stratocracy|Options")
	FStratOptionsDismissed OnOptionsDismissed;

	/**
	 * Fired when `ReturnToTitleButton` was clicked. The owner leaves the match; this class does
	 * not, and structurally cannot.
	 *
	 * IT DOES NOT CLOSE THE PANEL EITHER, AND THE OWNER MUST. Leaving the panel's flag open
	 * across a level travel is the state `UStratShellSubsystem` records as surviving the map --
	 * so an owner that travels without closing it puts an options screen over the title menu the
	 * player just arrived at. That is the owner's job for `OnOptionsDismissed`'s stated reason
	 * (whoever put the widget up is the only thing that knows what else is on screen) and the
	 * obligation is repeated here because the failure mode is different and worse.
	 *
	 * BROADCAST UNCONDITIONALLY BY `HandleReturnToTitleClicked`, WITH NO RE-CHECK OF
	 * `ExitModel.bReturnToTitleEnabled`. A disabled `UButton` does not broadcast `OnClicked` at
	 * all, so the guard would be dead code on the shipped path; and if it were ever reachable --
	 * a graph calling the handler directly -- refusing here would hide the content defect from
	 * `ExecuteRoute`, which re-asks the permission itself and logs the refusal by name. A second
	 * silent refusal one module upstream would make that log never happen.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Stratocracy|Options")
	FStratOptionsReturnToTitleRequested OnReturnToTitleRequested;

protected:
	// ---- THE BOUND SUB-WIDGETS. Names are the asset's contract; see the file header. ----

	/**
	 * The master gain control. `BindWidget` -- a screen without it is not this screen.
	 *
	 * `protected` AND NOT `private`, so `UStratOptionsWidgetDouble` and any later test double can
	 * plant one. `BindWidget` itself is indifferent to access, since the binding is done by
	 * reflection at Blueprint compile time; the access level is chosen for the clause author.
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> MasterSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> SfxSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> MusicSlider;

	/**
	 * Leaves the screen. `BindWidget`, because a screen a player cannot leave is worse than a
	 * screen that fails to compile -- and this one takes a UI-only input mode.
	 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	/**
	 * The percentage label beside `MasterSlider`. `BindWidgetOptional` -- see the file header
	 * for why the readouts are optional and the controls are not.
	 *
	 * ITS TEXT IS ASSIGNED FROM `Model.MasterVolumeText` AND IS NEVER COMPOSED HERE.
	 *
	 * RETRACTED> "That is `T-UI-03`'s clause on this surface: one field, one drawn number, no
	 * RETRACTED>  expression between them."
	 *
	 * That is the RULE `T-UI-03` states -- one field, one drawn number, no expression between
	 * them -- applied by analogy. The ID is refused for this surface, and this member is one of
	 * the *"`UStratOptionsWidget`'s bound sub-widgets"* the 2026-09-05 ruling names among the
	 * subjects it covers. See this file's `AMENDED 2026-09-07 (SECOND PASS)` block.
	 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MasterValueText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SfxValueText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MusicValueText;

	/**
	 * Leaves the match and goes back to the title. §2.11.5's exit, added 2026-09-07.
	 *
	 * `BindWidgetOptional` AND NOT `BindWidget`, WHICH IS AN EXCEPTION TO THE RULE THIS VERY FILE
	 * STATES, AND IT IS CALLED OUT RATHER THAN LEFT TO BE INFERRED. The file header's line is
	 * "a widget that PRODUCES a value is `BindWidget`; a widget that only DISPLAYS one is
	 * `BindWidgetOptional`", and this member produces one -- it is the second of only two controls
	 * on this screen that can originate a player's intent. It is optional anyway, for
	 * `UStratCommandBarWidget::OptionsButton`'s and `UStratShellMenuWidget::OptionsButton`'s
	 * measured reason: `BindWidget` is enforced by the WIDGET BLUEPRINT COMPILER, `WBP_Options`
	 * already derives from this class and has no such button in its tree [FALSE SINCE THE
	 * 2026-09-07 ASSET PASS; SEE THE STAMP BELOW. The REASONING it supports survives unchanged --
	 * the two changes were still in two lanes and still could not be atomic, which is why this
	 * shipped optional], so a hard bind would
	 * turn the ENTIRE volume screen -- three sliders, back button and all -- into a compile error
	 * the moment this header lands, and would stay one until the editor lane caught up. **The two
	 * changes are in two different lanes and cannot be atomic.** That is a fact about this
	 * project's authoring route and not a preference.
	 *
	 * A READER APPLYING THE RULE TO THIS MEMBER WOULD READ THE OPTIONALITY AS A CLAIM THAT THE
	 * CONTROL IS DECORATIVE. It is not; it is the only way out of a match this screen has.
	 * DISCHARGED BY a clause that reads the shipped `WBP_Options` CDO's `ReturnToTitleButton`
	 * [THAT WORDING IS VACUOUS AND WAS RE-WORDED ON 2026-09-07; THE STAMP BELOW GIVES THE MEASURED
	 * REASON AND THE CONDITION THAT REPLACED IT],
	 * after which this may become `BindWidget` in a pass that touches nothing else. The clause is
	 * the test lane's; note that both of the two prior members that reached this same condition
	 * DECLINED the hard bind once the asset landed, on the stated ground that a `BindWidget` which
	 * reds a whole screen should rest on a clause rather than on a record sentence.
	 *
	 * THAT CONDITION IS MET, AND ITS SECOND HALF WAS WORDED SO THAT NO CLAUSE COULD EVER SATISFY IT
	 * LITERALLY. Both facts are recorded, because the wording is why the first one took two gate
	 * rounds to be seen. The condition names two things: an asset carrying the button, and a clause
	 * reading the shipped `WBP_Options` CDO.
	 *
	 * [FALSE SINCE THE 2026-09-07 TEST-LANE PASS; CORRECTED BY THE STAMP BELOW] "The CLAUSE half is
	 * NOT: no clause in this tree reads that CDO, so the fact authorising a hard bind is still
	 * testimony plus a byte scan." AND, IN THE SAME BREATH AND EQUALLY FALSE: "the discharge
	 * condition above stands unchanged and unmet."
	 *
	 * THAT IS THE SECOND CORRECTION ON THIS MEMBER IN TWO GATE ROUNDS AND BOTH HAD ONE SHAPE, SO
	 * THE SHAPE IS WRITTEN DOWN HERE RATHER THAN CORRECTED A THIRD TIME. Each was a present-tense
	 * claim about what ANOTHER LANE had not yet built, made in a header this lane owns, in a tree
	 * where that lane was working. The first said the ASSET had no such button; the editor lane
	 * landed it in the same tree. The second -- written as the fix for the first -- said no CLAUSE
	 * read it; the test lane landed that in the same tree. THE RULE FOR WHOEVER EDITS THIS MEMBER
	 * NEXT: do not state the status of another lane's artifact in the present tense. Say what was
	 * true at a named commit or in a named run, and stop there.
	 *
	 * WHAT IS TRUE NOW, STAMPED 2026-09-07 BY `strat-gameplay-engineer` OVER BASE `46321a6`, EACH
	 * HALF ANCHORED TO AN IDENTITY RATHER THAN TO THE PRESENT TENSE.
	 *   THE ASSET HALF. `Content/UI/WBP_Options.uasset` acquired the control in the editor lane's
	 * 2026-09-07 pass. `Tools/architect/state/content.md` records that pass in the editor lane's
	 * own words, and the record was CHECKED against the bytes rather than taken on testimony:
	 * `grep -a -c ReturnToTitleButton Content/UI/WBP_Options.uasset` returns a hit on the
	 * working-tree file, whose `sha256` is `7e32ef67836b2d9241ecd70e760905d32e16def2ec7df9e2accdc4a346098adb`
	 * -- the same oid `content.md` cites, so the record and the bytes are known to be about one
	 * file. Positive control `MasterSlider` hits; negative control `NoSuchWidgetXYZ` returns `0`,
	 * so the scan can say no.
	 *   WHAT THE SCAN DOES NOT ESTABLISH, said here so it is not read as more than it is: a name
	 * present in a `.uasset`'s bytes is a NAME, not a `UButton` at a known place in a compiled
	 * widget tree, and it cannot distinguish a live control from a stale entry.
	 *   THE CLAUSE HALF, AND WHY ITS WORDING WAS VACUOUS ALL ALONG.
	 * `Stratocracy.StratUI.GATE-TITLEMENU.ShippedOptionsWidgetCarriesReturnToTitleButton` reported
	 * green in the run whose `reportCreatedOn` is `2026.09.07-21.25.09`. It does read the shipped
	 * class's default object -- and its assertion ON THAT READ is `TestNull`, deliberately, as a
	 * CONTROL: a `BindWidget*` member is assigned in `UUserWidget::Initialize` on a CONSTRUCTED
	 * INSTANCE and is null on a class default object BY CONSTRUCTION. So "a clause that reads the
	 * shipped CDO's `ReturnToTitleButton`" could never have authorised anything -- the read it asks
	 * for returns null on a correct asset and on a broken one alike, which is the definition of an
	 * instrument that cannot say no. THE CONDITION IS THEREFORE RE-WORDED TO WHAT IT MEANT: a
	 * clause that finds a child of this member's name in the SHIPPED WIDGET TREE and asserts its
	 * CLASS. That is exactly what the clause named above asserts, and it closes precisely the gap
	 * the byte scan leaves open -- grep cannot tell a name from a `UButton`, and a re-typed control
	 * binds to nothing while the screen still compiles.
	 *   SO THE HARD BIND IS AUTHORISED AND IS STILL NOT TAKEN IN THIS PASS. Promoting this to
	 * `BindWidget` is an EXECUTABLE change -- it moves enforcement into the Widget Blueprint
	 * compiler, where a regression in the asset reds the entire volume screen rather than one
	 * clause. This pass is comment-only and was not built from this seat, so it is not the pass to
	 * make that move; it is a separate decision on its own evidence, and both prior members that
	 * reached this condition -- `UStratCommandBarWidget::OptionsButton` and
	 * `UStratShellMenuWidget::OptionsButton` -- are `BindWidgetOptional` in this tree still.
	 *
	 * `protected` FOR `MasterSlider`'S REASON: so a test double can plant one. `BindWidget*` is
	 * resolved by reflection at Blueprint compile time and is indifferent to access.
	 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ReturnToTitleButton;

	/**
	 * Why the exit is greyed, when it is. `BindWidgetOptional` on the ORDINARY reading this time:
	 * it only displays a value.
	 *
	 * ITS TEXT IS `ExitModel.ReturnToTitleReason` AND IS NEVER COMPOSED HERE, and it is NOT the
	 * only sanctioned route to that sentence -- `OnExitOptionsRefreshed` hands the whole model to
	 * the graph, so an asset may draw the reason under any name, inside the button, or not at all.
	 * A screen without it is still correct: the control is still DISABLED, which is the property
	 * the requirement actually names. This label only explains it.
	 */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ReturnToTitleReasonText;

	/** Binds the five controls. Safe on a native subclass with no widget tree: every bound
	 *  pointer is null there and every bind is guarded. */
	virtual void NativeConstruct() override;

	/** Unbinds exactly what `NativeConstruct` bound. */
	virtual void NativeDestruct() override;

private:
	/**
	 * Pushes `Model` at the bound sub-widgets. The ONLY writer of a slider value or a label.
	 *
	 * IT IS CALLED FROM `PushAudioOptions` AND FROM ALL THREE SETTERS, INCLUDING THE SETTER A
	 * SLIDER JUST CALLED, and that is deliberate rather than wasteful: `StratClampVolume` maps
	 * NaN to silence and clamps out of range, so the value the model holds is not always the
	 * value the control submitted, and a thumb left where the control put it would then be a
	 * position the model does not have.
	 *
	 * THE RE-ENTRANCY GUARD IS STRUCTURAL AND NOT DEFENSIVE, AND THE ENGINE QUESTION IT WAS
	 * WRITTEN AROUND IS NOW ANSWERED.
	 *
	 *   RESOLVED> 2026-09-05: "`USlider::SetValue` is not documented here to re-broadcast
	 *   `OnValueChanged` and this file does not assert that it does not -- an unmeasured claim
	 *   about an engine internal is exactly what this project refuses to write down."
	 *
	 * IT RE-BROADCASTS. `USlider::SetValue` (`Slider.cpp:152-163`, UE 5.8) calls
	 * `HandleOnValueChanged` whenever `Value != InValue`, and that broadcasts `OnValueChanged`
	 * unconditionally -- OUTSIDE the `MySlider.IsValid()` block, so it needs no Slate widget, no
	 * `TakeWidget`, and no user drag. The feedback loop is REAL AND REACHABLE HEADLESSLY. The
	 * discipline the retracted sentence was practising was right; the conclusion a reader drew
	 * from it -- that no clause could reach this path -- was wrong, and cost a proposed seam.
	 *
	 * WHICH MAKES THE TWO GUARD SITES DIFFERENT THINGS, and only one of them is load-bearing.
	 * The early returns in the three `Handle*SliderChanged` handlers are what stop a seed from
	 * being recorded as a commit. Measured 2026-09-05 by deleting all three: it reddens
	 * `EachOptionsSliderReachesOnlyItsOwnChannel` ("dragging the master slider produces exactly
	 * one commit ... it was 2"), `EveryOptionsValueTextEqualsItsOwnModelField` ("seeding the
	 * planted screen still commits nothing ... it was 6") and
	 * `TheOptionsBackButtonDismissesWithoutCommitting`. They are already pinned by three
	 * clauses and need no seam.
	 *
	 * The early return in `SyncBoundWidgetsToModel` itself is NOT reachable while those three
	 * hold -- deleting it alone was measured green across the whole suite the same day, because
	 * a handler guard catches the re-entry before it can reach a second sync. It is kept anyway,
	 * for a reason the same pair of measurements supplies: with the handler guards gone, it is
	 * the line that turns an unbounded sync/broadcast recursion into a BOUNDED double commit,
	 * which is why deleting them produced three legible assertion failures instead of a stack
	 * overflow. It buys a diagnosable failure for a future regression, and that is its whole
	 * job. IT IS DELIBERATELY NOT GIVEN A TEST SEAM: a clause that could reach it would first
	 * have to disable the guards that make it unreachable, and would then be asserting against
	 * its own fixture rather than against this class.
	 */
	void SyncBoundWidgetsToModel();

	/** True for the duration of `SyncBoundWidgetsToModel`. See its block. */
	bool bSyncingBoundWidgets = false;

	/**
	 * The three slider handlers, one per channel.
	 *
	 * `UFUNCTION` BECAUSE `FOnFloatValueChangedEvent` IS A DYNAMIC MULTICAST and can bind
	 * nothing else -- the same constraint `UStratOptionsWidgetDouble::HandleCommitted` records.
	 * THREE FUNCTIONS AND NOT ONE TAKING A CHANNEL, on the `.cpp`'s stated reasoning about the
	 * three setters: a channel in a runtime value is how the music slider ends up writing the
	 * SFX gain, silently.
	 */
	UFUNCTION()
	void HandleMasterSliderChanged(float InValue);

	UFUNCTION()
	void HandleSfxSliderChanged(float InValue);

	UFUNCTION()
	void HandleMusicSliderChanged(float InValue);

	/** Broadcasts `OnOptionsDismissed`. Closes nothing itself -- see that delegate. */
	UFUNCTION()
	void HandleBackClicked();

	/** Broadcasts `OnReturnToTitleRequested`. Leaves nothing itself -- see that delegate. */
	UFUNCTION()
	void HandleReturnToTitleClicked();

	/**
	 * Pushes `ExitModel` at the exit control. The ONLY writer of that button's enabled state.
	 *
	 * A SECOND SYNC AND NOT AN EXTENSION OF `SyncBoundWidgetsToModel`, AND THE REASON IS THAT
	 * FUNCTION'S RE-ENTRANCY GUARD RATHER THAN TIDINESS. `bSyncingBoundWidgets` exists because
	 * `USlider::SetValue` re-broadcasts `OnValueChanged` -- measured, see that declaration -- so
	 * the gain sync is a function that can be RE-ENTERED and returns early when it is. Folding the
	 * exit row into it would make the exit control's enabled state a casualty of a guard that has
	 * nothing to do with it: a sync that returned early to break a slider loop would silently skip
	 * the permission redraw. `UButton::SetIsEnabled` broadcasts nothing, so this function needs no
	 * guard and must not inherit one.
	 */
	void SyncExitWidgetsToModel();
};
