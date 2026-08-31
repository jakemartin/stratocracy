# Stratocracy state — content

> **Writer: `strat-editor-builder`; fallback writer `coordinator`, under the one condition below --
> and separately, as in the four other lane-owned files here (NOT `global.md`), the `coordinator`
> transcribes lane-authored entries after
> a merge, attributing acting and writing separately (user ruling, 2026-08-29). THE TWO ARE
> DIFFERENT AND MUST NOT BE CONFLATED: the fallback below is triggered by an ABSENT TOOL and
> licenses the `coordinator` to AUTHOR this file's content; the transcription clause is triggered
> by A MERGE and licenses only carrying across what the lane agent already wrote.**
> Split out of `Tools/architect/state.md` on 2026-08-20, which is now
> FROZEN as the historical record of closed milestones. This directory is the live record.
>
> **THE FALLBACK CONDITION, and it is the only one.** `strat-editor-builder` may write this file
> whenever it can work at all. The `coordinator` may write it *only* when `execute_script` is
> absent from the session's tool surface — the agent holds the NeoStack tools plus Read/Grep/Glob/
> Skill and no Bash, so without that one tool it has no route to the editor and cannot produce the
> evidence this file holds. Amended 2026-08-23 by the user's decision, after the condition recurred
> on three consecutive days (2026-08-21, -22, -23) with the same measured cause; a
> `strat-integration-reviewer` gate had ruled the previous "second exception, not a standing
> arrangement" wording to be drifting from practice. **Two obligations ride with the fallback:**
> (1) measure the absence with a control before invoking it — an absent tool name proves nothing
> alone, so show the same lookup returning the tools that ARE served; and (2) say so in the entry,
> naming the coordinator as its writer. No other lane may write this file, and the fallback
> licenses writing this file only — it is not a general licence to act in the content lane.
>
> **WHERE THE OTHER PERMISSION LIVES, since 2026-08-26.** The sentence above is correct and is
> not being widened: this clause has never licensed driving the editor, and three consecutive
> `strat-integration-reviewer` gates turned on that exact distinction while the asset in question
> was clean from the first read. The permission it kept being stretched to cover — who may drive
> the live editor when `strat-editor-builder` cannot reach it — is now stated in `CLAUDE.md`'s
> **The crew** section as the *editor-driver clause*, with its own two obligations, because it is
> a lane rule and lane rules belong where lanes are defined. Cite THAT for the acting and THIS
> for the file write; an entry that cites one authority for both halves is a finding. This
> paragraph was added by the `coordinator` under the fallback condition, measured with its control.
>
> **Holds:** Assets, Blueprint defaults, PIE evidence, and the measured limits of the NeoStack Lua API.
>
> **One file owns each fact class.** Other files may *link* to a fact recorded here; they may not
> restate it. Only `global.md` may carry a live suite count or a phase verdict — a live `N/N`
> in any other file is a finding, enforced by `strat_banner_sweep.py`'s RECORD OWNERSHIP check.
> Everything under `## NEXT` is swept as live; stamp an entry that has become history rather
> than deleting it, exactly as `state.md` did.

## NEXT

- **W6'S ASSET TAIL: THE TITLE SCREEN EXISTS, IS REACHABLE AND IS DRIVEN BY THE MODULE RATHER
  THAN BY ANYTHING THIS WIDGET SPELLS.** 2026-08-30, in the integration tree
  `E:/MultiAgent/Stratocracy` on branch `master`, after the W6 merge `40609e7`. **ACTING: the
  `coordinator`, under `CLAUDE.md`'s editor-driver clause. WRITING: the `coordinator`, under this
  file's own fallback condition.** The two halves have two authorities and are named separately
  because citing one for both is a finding by this file's header. `global.md` carries any count or
  verdict; this file states neither.
  - **THE ABSENCE WAS MEASURED WITH A CONTROL, WHICH BOTH CLAUSES REQUIRE.** One ToolSearch over
    the NeoStack server returned `list_unreal_projects` and `unreal_status` and **neither
    `execute_script`** -- the same lookup shown serving tools before its silence on the one that
    matters is read as evidence. `strat-editor-builder` was not dispatched: its `tools:` line
    carries no Bash, so with `execute_script` gone it holds no `curl` route either, which is the
    header's stated rationale holding exactly as written.
  - **THE ROUTE WAS THE EDITOR'S OWN HTTP ENDPOINT, AND THE EDITOR HAD TO BE LAUNCHED FIRST.**
    `unreal_status` reported no active editors and nothing was listening on `9315`, with a control
    showing the port query could see ports that WERE listening. The editor was launched, `9315`
    answered ~40s later, `initialize` returned `serverInfo.name = "unreal-editor"` version
    `1.0.0-r4254`, and `tools/list` carries **exactly one tool, `execute_script`**.
    `NeoStack_Connect` then latched as this file already records: `list_unreal_projects` reported
    `Stratocracy` active while `unreal_status` still returned its startup error, in the same pass.
  - **THREE INSTRUMENT FAULTS, EACH CAUGHT BY A CONTROL RATHER THAN BY LUCK, RECORDED BECAUSE EACH
    WOULD HAVE READ AS A TOOL FAILURE.** (1) The endpoint answers **pretty-printed JSON, not SSE**;
    a line-by-line parser sees only a bare brace, reports nothing, and reads exactly like an empty
    tool list. (2) The session header is spelled `MCP-Session-Id` and a case-sensitive dict lookup
    returned nothing, which reads as "the server issues no session". (3) `pcall` returns
    **`ok=true` for a FAILED editor op** -- every bad `list_properties` target came back
    `pcall=true` -- so the `[OK]`/`[FAIL]` trace line is the discriminator and the Lua-level success
    signal is not. That third one is this file's own recorded `set`-return hazard in a new place.
  - **WHAT WAS BUILT, AND EVERY WRITE IS VERIFIED AGAINST THE SAVED BYTES ON DISK RATHER THAN
    AGAINST THE AUTHORING CALL'S RETURN.**
    - `BP_StratShellGameMode` is NEW, parented to `StratShellGameMode`. **The parent was proved
      twice and the first proof was nearly a false one:** `StratShellGameMode` is a SUBSTRING of the
      asset's own name `BP_StratShellGameMode`, so a plain byte count returns 20 and means nothing.
      Discriminated as total-minus-prefixed = **8 bare occurrences, identical to the control sibling
      `BP_StratGameMode`**, and confirmed independently by the editor reporting
      `parent_class = StratShellGameMode`. This is the recorded prefix-nesting census trap.
    - `MatchLevel` on that CDO moved from unset to `/Game/StratMaps/Lvl_FerrumCrossing`, and the
      saved package carries `MatchLevel` and `Lvl_FerrumCrossing`.
    - **`SaveSlotName` IS ABSENT FROM THE SAVED PACKAGE, AND THAT IS THE INTENDED STATE, NOT AN
      OMISSION** -- absence means the CDO equals the C++ default, so the property keeps ONE author.
      Read back live it is `StratocracyMatch`, which is the engineer's derived-not-copied default
      arriving intact.
    - `Lvl_Title` is a NEW basic map. Its World Settings `DefaultGameMode` is
      `BP_StratShellGameMode_C`; `KillZ` was re-read unchanged as the control that the write touched
      only what it named. **The override is load-bearing rather than stylistic:**
      `GlobalDefaultGameMode` is `BP_StratGameMode` PROJECT-WIDE and the match map carries no
      override of its own (0 occurrences in its bytes), so without this override the title map would
      boot the match GameMode.
    - `TitleLevel` on `BP_StratGameMode` moved from unset to `/Game/StratMaps/Lvl_Title`. The
      control read of `MatchConfig` in the same call shows it untouched -- and incidentally shows
      `SaveSlotName="StratocracyMatch"` there too, so **the two Blueprints agree on the save slot
      today, measured.** That is the drift the engineer recorded as invisible to C++ and owed a
      clause reading both CDOs; it is satisfied at this moment and still unpinned.
    - `WBP_TitleMenu` is NEW: 16 widgets, 46 graph nodes, compiles with zero errors and zero
      warnings.
  - **THE WIDGET SPELLS NO LABEL, NO ENABLEMENT AND NO REASON.** All three are read from
    `UStratShellSubsystem::GetMenuModel` BY INDEX and written in at Construct, and each button's
    click pulls its `Route` from **the same option break the row drew from**, so the route a click
    takes and the label it drew cannot disagree. The design-time placeholder is the visible string
    `(unbound)` rather than an empty box, deliberately: an unwritten row then reads as broken
    instead of as plausible.
  - **AN ORDERING HAZARD WAS FOUND IN THE C++ AND FIXED IN THE GRAPH, AND THE FIX IS PROVED BY A
    POSITIVE CONTROL RATHER THAN BY THE REASONING THAT FOUND IT.** `AStratShellGameMode::BeginPlay`
    calls `Super::BeginPlay()` FIRST -- which is what fires the Blueprint's `Event BeginPlay` -- and
    only afterwards calls `ConfigureMatchDestination`. A widget built directly off `Event BeginPlay`
    therefore asks `GetMenuModel` before the shell has been given `MatchLevel`. The graph hops one
    tick before creating the widget. **MEASURED THREE WAYS IN SEQUENCE, not argued:** with the hop,
    `New Match` draws ENABLED and `Continue` reads *"No saved match."*; with the hop disconnected,
    `New Match` AND `Continue` both grey out reading *"No match level is configured."*; with the hop
    reconnected, the first state returns. Screenshots `ScreenShot00071`, `00072` and `00073` under
    `Saved/Screenshots/WindowsEditor/`, each captured with `playtest_console("shot showui")`, which
    is the only editor capture that composites UMG. **Delete the hop and the menu is wrong in
    exactly that one quiet way.**
  - **PIE EVIDENCE, ON `/Game/StratMaps/Lvl_Title`.** All four routes draw in declaration order;
    `New Match` and `Quit` are enabled with NO reason line, `Continue` and `Return to Title` are
    greyed each with its own reason. That is the model's *"reason empty exactly when enabled"*
    invariant holding on a live surface in both directions at once.
  - **ONE DEPARTURE FROM THIS PROJECT'S CONVENTION, FLAGGED RATHER THAN BURIED.** Every other widget
    here reaches the screen through a C++ HUD that owns a widget-class slot
    (`AStratScoreboardHUD::ProductionMenuWidgetClass`). W6 built no shell HUD, so this menu is
    created from `BP_StratShellGameMode`'s own graph instead -- Create Widget, Add to Viewport, input
    mode, cursor. It needs no C++ and stays in this lane. **The alternative is a shell HUD class with
    a widget slot, which would match the convention and is an engineer-lane item; it was not taken
    and the user was told.**
  - **WHAT THIS BATCH DID NOT DO.** `GameDefaultMap` still points at `Lvl_FerrumCrossing`. Moving it
    to `Lvl_Title` is a `Config/DefaultEngine.ini` change and therefore **`strat-data-steward`'s
    lane, not this one**, and it must land AFTER the level exists -- which it now does.
    `EditorStartupMap` sits on the line beside it and whether that moves too is the steward's call.
    No clause pins any of this batch: the menu's live behaviour is evidenced by screenshots, and a
    screenshot is not a test.

- **W4'S FOUR MARKER SLOTS AND W5'S PLAYBACK PACE ARE SET, AND THE FOUR ART ASSETS THE SLOTS
  NEEDED DID NOT EXIST -- SO THIS WAS AN AUTHORING BATCH, NOT AN ASSIGNMENT BATCH.** 2026-08-29,
  in the integration tree `E:/MultiAgent/Stratocracy` on branch `master`, with no worktree, no
  branch and no merge. **ACTING: the `coordinator`, under `CLAUDE.md`'s editor-driver clause.
  WRITING: the `coordinator`, under this file's own fallback condition.** The two halves have two
  authorities and are named separately here because citing one for both is a finding by this
  file's header. `global.md` carries any count or verdict; this file states neither.
  - **THE ABSENCE WAS MEASURED WITH A CONTROL, WHICH BOTH CLAUSES REQUIRE.** A single
    `select:mcp__NeoStack_Connect__execute_script,mcp__unreal-editor-direct__execute_script,mcp__NeoStack_Connect__unreal_status`
    lookup returned **`unreal_status` and neither `execute_script`** -- so the same lookup is shown
    serving a tool before its silence on the other two is read as evidence. `strat-editor-builder`
    was not dispatched: with no `execute_script` on the surface it holds no route to the editor.
  - **THE ROUTE WAS NEITHER MCP CLIENT, AND THIS IS THE PART OF THE ENTRY THAT NEEDS A USER
    RULING RATHER THAN A GATE.** `unreal-editor-direct` refused connection at session start and
    an MCP client does not retry, so launching the editor did not revive it. `NeoStack_Connect`
    LATCHED: after the editor was live, `list_unreal_projects` reported `Active editors:
    Stratocracy` while `unreal_status` still returned the startup "no active editors" error --
    the two disagreeing in the same pass is the measurement, and it confirms the recorded proxy
    latch rather than a discovery failure. The editor was therefore driven over its own
    Streamable-HTTP MCP endpoint at `127.0.0.1:9315/mcp` with `curl` from Bash, which
    `initialize` answered as `serverInfo.name = "unreal-editor"`, and whose `tools/list` carries
    `execute_script`. **SO THE FALLBACK CONDITION WAS MET, ITS RATIONALE INCLUDED -- AND THE GATE CORRECTED AN
    EARLIER DRAFT OF THIS BULLET, WHICH CLAIMED THE RATIONALE HAD FAILED. IT HAD NOT.** The
    header grants the fallback because `strat-editor-builder` holds "the NeoStack tools plus
    Read/Grep/Glob/Skill and no Bash, so without that one tool it has no route to the editor".
    Checked against `.claude/agents/strat-editor-builder.md`: that agent's `tools:` line carries
    no Bash, so with `execute_script` gone it had no `curl` route either. The rationale held
    exactly as written, and the `coordinator` holding Bash does not weaken it -- the clause is
    about what the LANE AGENT can reach, not about what every actor can.
    **THE REAL GAP IS NARROWER AND IS WORTH STATING PRECISELY: neither this file's fallback nor
    `CLAUDE.md`'s editor-driver clause models a tool being ABSENT FROM THE CLIENT SURFACE WHILE
    THE SERVER STILL SERVES IT.** Both condition on the tool's absence and both read that
    absence as meaning the editor is unreachable. Here the editor was reachable on
    `127.0.0.1:9315` throughout, and only the two MCP clients could not get to it. Whether an
    actor who reaches a live editor by some other transport falls inside these clauses or needs
    a sentence of its own is the user's to rule; until it is, this entry is the only place the
    question is visible.
  - **EVERY `open_asset` PRECEDED PIE, WHICH IS THIS FILE'S OWN RECORDED CRASH.** All asset reads
    and writes ran with `playtest_status()` reporting no PIE world; the playtest below was started
    only after the last write and its script calls no `open_asset`. The recorded failure -- that
    `open_asset` nulls a live PIE `WidgetTree` and fakes an absence past its own control -- was
    therefore not reachable in this pass.
  - **WHAT THE BATCH FOUND MISSING, AND IT IS WHY THIS IS NOT THE FIVE-ASSIGNMENT BATCH THE
    RECORD ANTICIPATED.** `asset_exists` answered **false** for all four of
    `/Game/StratArt/Meshes/SM_FlagMarker`, `.../SM_UnactedPip`,
    `/Game/StratArt/Materials/MI_Marker_Flag` and `.../MI_Marker_Pip`, with the instrument
    controlled in both directions in the same call -- **true** for `SM_GuidedMarker` and
    `MI_Marker_Guided`, **false** for a `SM_NoSuchThing` that must not exist. `Content/StratArt/`
    held only the guided-marker set. The art had to be authored before any slot could be filled.
  - **THE ASSETS, AND WHAT EACH ONE DERIVES FROM RATHER THAN GUESSES.**
    - `MI_Marker_Flag` and `MI_Marker_Pip` are NEW, both duplicated from `MI_Marker_Guided` so
      they inherit its parent and its blend overrides rather than being built from a fresh guess.
      Readback confirms parent `M_Translucent` on both. `Color` is gold `(1.0, 0.72, 0.05, 0.95)`
      on the flag and green `(0.15, 0.95, 0.30, 0.95)` on the pip, against the guided marker's
      magenta `(0.8, 0.1, 0.95, 0.95)` -- three hues, no pair adjacent. **The guided marker's own
      colour was re-read after both writes and is unchanged**, which is the control that keeps a
      duplicate-then-edit from being read as an edit of the source.
    - `FlagMarkerMesh` is `/Engine/BasicShapes/Cone` and `UnactedPipMesh` is
      `/Engine/BasicShapes/Sphere`. **Engine primitives are not a new precedent in this class:**
      `FallbackMesh` on this same Blueprint already reads `/Engine/BasicShapes/Cylinder`.
    - **THE 100 uu ASSUMPTION `StratUnitActor.h` STATES WAS CHECKED RATHER THAN INHERITED.** That
      header derives Z = 300 and the ±40 Y separation on the stated assumption that the markers
      "match `SM_GuidedMarker`'s 100 uu", and says in terms that no such mesh exists yet.
      `SM_GuidedMarker`'s `ExtendedBounds` measures `BoxExtent = (50, 50, 50)`, and both engine
      primitives are the same 100 uu. The assumption holds; it is now measured rather than hoped.
  - **WHAT THE AUTHORING RETURNS CLAIMED AND THE BYTES DENIED -- TWICE, AND ONCE IT WOULD HAVE
    ENTERED THIS RECORD AS A SUCCESS.**
    - `bp:set("MatchConfig.AiPlaybackStepSeconds", 0.5)` is **not supported**: dotted paths do not
      resolve, and the tool logged `property not found`. **`pcall` nonetheless returned `ok=true`**,
      so the Lua-level success signal was true for a write that never happened. Only the readback
      caught it. The value was then set by round-tripping the exported struct literal with one
      field appended -- nothing retyped by hand -- and the readback positively re-checked
      `AiTurnDelaySeconds`, `ScenarioFile` and the asset-class fields to prove the round trip cost
      no sibling field.
    - `SetMaterialInstanceVectorParameterValue` returned `false` on both material instances while
      **both colours took**, confirmed by reading `VectorParameterValues` back. The return is not
      the evidence in either direction, which this record already holds as a measured property of
      the `set` family.
    - **A COST OF THE FAILED FIRST ATTEMPT, STATED RATHER THAN LEFT IN THE MTIMES.** That attempt
      called `compile()` and `save()` on both GameMode Blueprints before the readback showed the
      write had not landed, so both were resaved once with no semantic change before the second
      attempt changed them for real.
  - **EVERY WRITE IS VERIFIED AGAINST THE SAVED BYTES ON DISK, NOT THE EDITOR'S REPORT.**
    `BP_StratUnit.uasset` carries all four of `FlagMarkerMesh`, `FlagMarkerMaterial`,
    `UnactedPipMesh` and `UnactedPipMaterial`; both GameMode packages carry
    `AiPlaybackStepSeconds`, with `AiTurnDelaySeconds` present in the same scan as the control
    that the reader can see a field of that kind at all.
  - **THE INSTRUMENT THAT FAILED SILENT, AND THE CONTROL IS THE ONLY REASON IT DID NOT BECOME A
    FINDING IN THIS FILE.** The first pass at that disk verification used `strings`, which **is
    not installed on this box**. It returned `0` for all nine probed names -- including the five
    that were known present. Read without the control, that reads exactly like "every slot is
    still unset". Re-run with `grep -a`, the five controls returned 1 each and the four W4 names
    returned 0 before the batch and 1 each after it.
  - **PIE EVIDENCE: THE MARKERS DRAW, AND THE GAP LOG IS SILENT WITH A CONTROL BESIDE IT.** PIE
    on `/Game/StratMaps/Lvl_FerrumCrossing` reached `playing=true, has_pie_world=true`, and a
    `HighResShot 1920x1080` capture shows a gold cone above the flag unit **on both sides**, green
    pips on the viewing side's still-orderable units, and the guided marker's magenta sphere
    distinct from both. `StratUnitActor.h` logs the unconfigured-slot gap once per actor, and
    **zero such lines appear after the PIE start marker** -- with the control that 86 log lines
    and live `LogStratPlay` entries do exist in that same range, so the silence is the absence of
    the gap and not the absence of logging.
    - **ONE PROBE IN THAT PASS WAS UNSOUND AND IS RETRACTED HERE RATHER THAN QUIETLY DROPPED.**
      An in-Lua `playtest_log_contains` check was read through `r.found or r.passed or r.ok`;
      `ok` reports that the CALL succeeded, so the expression is true whether or not the text was
      found. It returned true for all three probes and means nothing. The log claim above rests
      on the grep over `Saved/Logs/Stratocracy.log` and its control, not on that probe.
  - **WHAT IS PLACEHOLDER, SAID PLAINLY SO NO LATER READER TAKES IT FOR AUTHORED ART.** A cone is
    not a flag and a sphere is not a designed pip. The slots are filled, the sizes are derived and
    the colours are separated, but both meshes are engine primitives and an artist replacing them
    changes no code, no default's meaning and nothing in this entry except the two mesh paths.
  - **TWO LANE CALLS HANDED FORWARD, NEITHER OF THEM THIS LANE'S TO SETTLE.**
    - **`AiPlaybackStepSeconds = 0.5` WAS SET ON BOTH GAME MODES, WHERE THE RECORD SAID "the
      GameMode Blueprint default" IN THE SINGULAR.** `BP_StratGameMode` is where a human watches
      the AI's turn and `BP_StratGameMode_AiVsAi` is where a tour is the entire point, so both
      were set. If the singular was deliberate, `BP_StratGameMode_AiVsAi` is the one to return to
      `0.0`.
    - **THE PLACEMENT STILL HAS NO GATE BUT A HUMAN EYE, AND THE CAPTURE ALREADY RAISES ONE
      DOUBT.** `StratUnitActor.h` says no test pins either offset vector. In the capture the
      markers read large against the units and the pips visually crowd adjacent units in the
      dense bottom cluster. `FlagMarkerOffset` and `UnactedPipOffset` are `EditDefaultsOnly`
      precisely so the Y separation can be retuned without a rebuild. **The discharge condition
      `engine.md` states -- a human confirming at the keyboard that three markers 40 uu apart
      read as three things at the shipped camera pitch -- is NOT met by this entry and is not
      claimed to be.**
      **[STAMPED 2026-08-29, LATER THE SAME DAY, BY THE `coordinator` UNDER THIS FILE'S OWN
      FALLBACK CONDITION -- the licence is cited because a write under it that names no
      authority is indistinguishable from a lane violation, and obligation (1) is discharged
      at the end of this block rather than assumed: THE CONDITION IS NOW MET AND THE DOUBT
      THIS BULLET RAISED WAS NOT BORNE OUT.** The user confirmed at the
      keyboard, in their own words: "I can confirm all markers and they are good where they
      are. Camera pitch looks good." That is the human eye this bullet said was the only
      available gate, and it is the discharge `engine.md` names. **THE RESERVATION ABOVE WAS
      THIS RECORD'S OWN AND IS RETRACTED RATHER THAN LEFT STANDING:** the reading that the
      pips crowd adjacent units in the dense cluster came from a single 1920x1080 capture and
      did not survive contact with the running game. No offset was changed; `FlagMarkerOffset`
      and `UnactedPipOffset` ship at the vectors `StratUnitActor.h` derived. WHAT REMAINS
      TRUE is that no TEST pins either vector -- the discharge is a human's word, which is
      what the condition asked for and is not a headless gate.
      **OBLIGATION (1), THE CONTROL, MEASURED IN THIS SESSION AND NOT INHERITED FROM THE ENTRY
      ABOVE:** one `select:` lookup naming
      `mcp__NeoStack_Connect__execute_script`, `mcp__unreal-editor-direct__execute_script` and
      `mcp__NeoStack_Connect__unreal_status` together returned **`unreal_status` and neither
      `execute_script`** -- the same lookup shown serving a tool before its silence on the
      other two is read as evidence.]**

- **GDD SEC 2.11.2'S HOVER INFO PANEL IS BUILT, COMPILES CLEAN AND IS WIRED TO THE HUD -- AND
  NOTHING HAS SEEN IT DRAW A REAL HEX, BECAUSE ONLY A HUMAN'S MOUSE CAN MAKE ONE.** 2026-08-28,
  written by `strat-editor-builder` over `mcp__unreal-editor-direct__execute_script`, which was on
  the tool surface from this session's FIRST call -- **THE FALLBACK CONDITION IN THIS FILE'S HEADER
  WAS NOT MET and the coordinator did not write this entry.** `print(project_dir())` answered
  `E:/MultiAgent/Stratocracy/` before anything was touched, so this is the integration tree and not
  a worktree. `playtest_status()` reported `has_pie_world=false, playing=false` and **no PIE was
  started at any point**, which is what makes `open_asset` on a Widget Blueprint safe here -- this
  file's own recorded crash. `global.md` carries any count or verdict; this file states neither.
  - **THE ASSETS, AND THE MODULE PRE-FLIGHT THAT GOVERNS THE IRREVERSIBLE HALF.**
    `/Game/UI/WBP_InfoPanel` is NEW, reparented to `/Script/StratUI.StratInfoPanelWidget`;
    `BP_StratScoreboardHUD` gained exactly one Blueprint default. The parent class is declared in
    `Source/StratUI/StratInfoPanelWidget.h` -- verified by locating the declaring FILE, and
    `StratUI` is the module its directory names, which is what the reparent bakes in permanently.
    `UStratInfoPanelWidget`'s own header block states the same thing and gives the reason.
  - **THE C++ SURFACE WAS VERIFIED LIVE WITH A NEGATIVE CONTROL BEFORE ANYTHING DERIVED FROM IT.**
    `class_properties("/Script/StratUI.StratInfoPanelWidget")` returns **30 entries**, while
    `/Script/StratUI.StratNoSuchWidgetXYZ` **fails to resolve** -- so the lookup is shown able to
    refuse a name before its success on the real one is read as evidence. All 22 fields of
    `FStratInfoPanelView` were then read off the `Break Strat Info Panel View` node's own output
    pins rather than trusted from the brief.
  - **THE DISPATCH BRIEF NAMED A HUD BLUEPRINT THAT DOES NOT EXIST, AND THE TREE WON.** The
    engineer's handoff called it `BP_StratHUD`. `open_asset("/Game/UI/BP_StratHUD")` answers
    **`asset not found`**; `/Game/UI/BP_StratScoreboardHUD` opens with parent
    `StratScoreboardHUD`. The default was set on the latter. **The read that proves `None` was a
    real absence rather than a broken read is the control beside it:**
    `ScoreboardWidgetClass` returned a live `WBP_Scoreboard_C` from the same call in which
    `InfoPanelWidgetClass` returned `None`, and it still returns it after the write.
  - **TWO EDGE CASES THE GDD DOES NOT SPECIFY. BOTH CALLS ARE THIS LANE'S, NOT THE GDD'S.**
    - **Plains, `DefensePct 0`: the clause is RENDERED, as `def +0%`, not suppressed.** A
      fixed-shape line is scannable at a glance, suppression would make Plains structurally
      different from every other hex, and `+0%` states "no bonus here" rather than leaving a
      reader to infer it from an absence. It also needs no conditional, so there is one less
      branch that could be wrong.
    - **Water, `bTerrainImpassable`: the move clause is REPLACED, giving `Water - impassable -
      def +0%`.** `MoveCost 0` is a sentinel and rendering it literally as `move 0` would read as
      "free to enter", which is the exact opposite of the truth. Replacing only that clause keeps
      the line's three-clause shape and never prints the misleading number.
  - **NO WIDGET-SIDE ARITHMETIC, AND THE TWO PLACES IT WAS NEARLY NEEDED WERE DESIGNED OUT.**
    - **The signed `def` is `To Text (Integer)` with `Always Sign` TRUE -- there is no comparison
      anywhere.** The obvious build is "if Pct >= 0 prefix a `+`", which is a widget deciding
      something about a number it was handed. The conversion node renders `+0`, `+20` and `-10`
      by itself, so the sign glyph costs zero branches. `Use Grouping` is set FALSE so no value
      can ever acquire a thousands separator.
    - **The range pair is the ONE comparison in the asset, and it is `Equal (Integer)` on
      `UnitStatRangeMin` against `UnitStatRangeMax` feeding a `Select`.** It derives no game
      number -- both arrive resolved and it chooses between two RENDERINGS of the same pair,
      `N` and `Min-Max`. Artillery at `2..3` is the only row that exercises it. A widget fed only
      `Min` is right on three unit rows and wrong on the one whose range matters.
  - **THE OWNERSHIP CLAUSE IS ONE TOTAL SWITCH ON `EStratHexOwnership` AND `bHexCapturable` IS
    NEVER READ.** A `Select` node driven by an enum expands into one pin per enumerator --
    `Not Capturable`, `Neutral`, `Yours`, `Enemy` -- and it is PURE, which an exec `Switch` is
    not and so could not have been used in a binding at all. `NotCapturable` carries the empty
    string, so the not-capturable case needs no second field and the combination-shaped defect
    `UStratInfoPanelWidget`'s header warns about cannot be written here.
  - **SIX MEASURED FACTS ABOUT THE NeoStack API, EVERY ONE OF WHICH COST A ROUND.**
    - **`find_nodes`'s FIRST argument is the QUERY, not the asset path.** `find_nodes(path,"Model")`
      returns `0 results` with an `[OK]` -- indistinguishable from "no such node exists".
    - **`create_asset`'s `parent_class` option is IGNORED for a WidgetBlueprint.** It reports
      `[OK] created` and the asset comes back parented to `UserWidget`. `bp:reparent(...)` is a
      separate, required call, and the parent must be re-read to confirm.
    - **A binding function's return type must be declared as the ENUM, not `byte`.**
      `add_function(..., type="byte")` compiles the function fine and then fails the BLUEPRINT
      with `the sigatnures don't match` (sic) against `VisibilityDelegate`. `type="ESlateVisibility"`
      is what binds. **The read cannot tell them apart** -- both return pins read `:byte`, and so
      does the working one on `WBP_DirectiveStrip`, so only a compile distinguishes them.
    - **A `Select`'s option pins stay `wildcard` until a TYPED SOURCE is wired into one**, and
      `set_pin` on a wildcard pin FAILS loudly (`Unsupported type Wildcard`). Wiring only the
      `Index` and the output is not enough; the output stays wildcard too and surfaces later as
      `Pin 'State' has an unexpected type: wildcard`. Order that works: connect `Index` first to
      mint the pins, then a typed source into one option, then `set_pin` the literal ones, then
      connect the output onward. `Make Literal Text` is the way to resolve it when BOTH options
      are literals.
    - **`add_node("Equal ( == )")` places a node titled `Equal (GameplayTagContainer)`.** It is a
      wildcard operator and retitles itself to `Equal (Integer)` once ints are wired to `A`/`B`;
      the alarming initial title is not a wrong node.
    - **`Format Text` mints its argument pins from the format string, and INSERTS SPACES into
      their names.** `{MoveClause}` becomes a pin named `Move Clause`. Set `Format` BEFORE
      connecting arguments, and connect by the SPACED name.
  - **THE RECORD WRITE ITSELF ALMOST CORRUPTED THIS FILE, AND THE CONTROL IS THE ONLY REASON IT
    DID NOT.** `Edit` is disabled on this session's tool surface, so the write went through the
    editor's own `write_file`. **`write_file` IN TEXT MODE WRITES UTF-16, AND `bytes_written`
    COUNTS CHARACTERS RATHER THAN DISK BYTES.** A round-trip of this file's own bytes reported
    `141969 bytes` and left **283842** on disk -- exactly 2x -- and passing `{encoding="utf8"}`
    explicitly changed NOTHING. **So this file's previously recorded control, that `bytes_written`
    matched the source length, does not detect this failure; only `file_info().size` does.**
    The route that IS byte-exact is `{encoding="binary"}` with base64, confirmed on a 95-byte
    CRLF fixture (95 == 95) before being used here, and `append=true` works in binary and is
    byte-exact too. This file is **CRLF** -- measured, not assumed: `read_file` NORMALISES CR
    AWAY (it reports zero CRLF pairs), so the endings were measured indirectly as
    `file_info().size` minus the summed normalised length, giving 1617 CR bytes for 1618 lines.
    `read_file` also silently caps at **500 lines**, so a whole-file read of this record needs
    four calls and a naive one truncates it to a third. A byte-exact `copy_file` backup was taken
    first and verified at 141969 == 141969.
    **And one search trap worth carrying:** `s:find("## NEXT")` matches the phrase
    "`## NEXT` is swept as live" in this file's own HEADER prose, 2700 bytes above the real
    heading. The token this record uses to mark its live section appears inside the sentence
    describing that section.
  - **WHAT WAS VERIFIED, AND IT IS DESIGN-TIME ONLY.** `compile()` reports `0 errors, 0 warnings`,
    and the graph was then re-read FRESH from the saved asset rather than trusted from the green
    compile: all 8 binding functions have both `execute` and `Return Value` linked, all 8 bindings
    are present, and every literal survived -- the `-` separators intact, `Always Sign` TRUE on the
    def% conversion and FALSE on the range one, and the four enumerator pins carrying empty /
    ` - neutral` / ` - enemy` / the formatted yours clause. The ONLY unlinked-and-defaultless pin
    in the whole asset is `Select.Not Capturable`, which is the intended empty status clause.
    On disk: `WBP_InfoPanel.uasset` 533449 bytes, `BP_StratScoreboardHUD.uasset` 25850 bytes.
  - **[STAMPED 2026-08-28, LATER THE SAME DAY -- TWO SENTENCES OF THE BULLET BELOW ARE NOW FALSE,
    SUPERSEDED BY THE HUMAN OBSERVATIONS RECORDED TWO BULLETS DOWN. They are named here rather
    than left for a reader to notice, and the block is restated flat rather than corrected inside
    itself. The first read: "Nothing here is a runtime observation and no hex has ever been drawn
    by this panel." The second read: "Every string this entry describes is a graph that has not
    yet run." Both were true when written and neither is true now. Everything else in that bullet
    stands -- what the DESIGNER preview shows, and that a human at the mouse is the only
    instrument that could close any of this.]**
  - **WHAT WAS NOT OBSERVED WHEN THIS ENTRY WAS FIRST WRITTEN, AS FIRST WRITTEN.** The designer
    preview confirms the PLACEMENT only -- bottom-left, compact, clear of the board's lower-centre,
    the flag line red -- and it shows the `Text Block` placeholders, because property bindings do
    not evaluate in the designer. The panel is driven by `AStratScoreboardHUD::PushInfoPanel` off
    the hover, and this file already records that no automation in this project reaches
    `UPlayerInput`, so **a human at the mouse is the only instrument that can close it.**
  - **A HUMAN DROVE THE MOUSE. THE HEX LINE AND THE UNIT LINE BOTH PAINT, AND THE RANGE PAIR IS
    WITNESSED ON ITS ONLY FALSIFYING CASE.** 2026-08-28, later the same day. Three observations,
    all HUMAN observations at the mouse, which is the only instrument that could have produced
    them -- injection never reaches `UPlayerInput`, so nothing in this project's automation can
    drive a hover. (1) Verbatim and in full: *"I can confirm hex rollover works, ui output works,
    ui updates works"*. (2) Unit rollover works. (3) **An Artillery unit was hovered and its range
    read `2-3`.**
  - **WHY THE ARTILLERY READING IS THE LOAD-BEARING ONE, AND WHY IT IS STRONGER THAN "A UNIT WAS
    HOVERED".** Artillery is the ONLY unit in `units.csv` whose range is a band. Infantry, Tank
    and Recon are all `1..1` and render `1` correctly **even if `UnitStatRangeMax` is never read at
    all** -- so a Min-only defect is invisible on three of the four rows and visible on exactly
    one. That one has now been seen reading `2-3`. So `GetUnitStatsText`'s `Equal (Integer)` on
    `UnitStatRangeMin` against `UnitStatRangeMax`, and the `Select` it drives, are attested on the
    single case that can falsify them rather than on a case that would have passed either way.
  - **WHAT THE THREE OBSERVATIONS ESTABLISH, WITH THE SUBJECT NAMED EXACTLY.** For the HEX half:
    `Panel.bHasHex` driving `GetPanelVisibility`, the terrain clause built by `GetHexLineText`, and
    the live push through `AStratScoreboardHUD::PushInfoPanel` and `UStratMatchSubsystem::ApplyView`
    work end to end, and the panel UPDATES as the cursor moves. For the UNIT half: `Panel.bHasUnit`
    driving `GetUnitVisibility`, and `GetUnitStatsText` including its range pair. **The stated
    evidence bar for this asset -- a human seeing it paint both a hex line and a unit line, with an
    Artillery so the band is exercised -- is MET.** That is a statement about which observations
    exist, not a verdict on the section; `global.md` carries any verdict and this file carries none.
  - **WHAT IS STILL NOT WITNESSED, AND ITEMS 1-3 MUST NOT BE READ AS "THE PANEL WORKS".** Four
    branches remain with no observation behind them. **The flag unit's** red edge from
    `GetPanelEdgeColor` and the appended `FLAG` line from `GetFlagVisibility` -- no flag unit was
    reported hovered. **The signed Bridge `-10`** -- which terrains were hovered was never stated,
    so `To Text (Integer)`'s `Always Sign` path is unattested in its negative direction. **Both of
    this entry's own edge-case calls**, the Plains `def +0%` and the Water `impassable`
    substitution, for the same reason. And **`ready`/`done`**: "unit rollover works" does not
    attest WHICH state was shown, so `bUnitDone` is covered structurally by the binding and is
    observed in neither state specifically.

- **THE TICK ROUTE WORKS UNDER A HUMAN'S MOUSE, AND THE HOVER ASSETS ARE GONE BECAUSE NOTHING
  REFERENCES THEM ANY MORE.** 2026-08-27, later the same day, same live editor session reopened
  on the rebuilt binary. **Attribution, separately:** the ACTING is the `coordinator` under
  `CLAUDE.md`'s **editor-driver clause**; the FILE WRITE is the `coordinator` under THIS file's
  header fallback condition. The measured absence and its control are unchanged from the entries
  below. A human drove the mouse. `global.md` carries the wave status and any verdict; this file
  states no count and no verdict.
  - **THE RELINK IS WITNESSED BY A REMOVAL THIS TIME, which is the same instrument run backwards.**
    `class_properties` on `/Script/StratPlay.StratPlayerController` returns **91 entries**, of
    which the `Action|Mapping|Hover` filter returns **seven** and no `HoverAction` -- exactly the
    figures this file recorded for the PRE-wave binary, and the exact reverse of the 92/eight that
    witnessed the default being settable. The property whose appearance proved one relink proved
    the next one by disappearing.
  - **THE BEHAVIOUR, READ-ONLY AND UNCONTAMINATED.** 40 samples over ~80 seconds of
    `UStratMatchSubsystem::AppliedModel`, no forced calls of any kind: **34 `True` across 25
    DISTINCT HEXES**, and **6 `False` in one CONTIGUOUS run**. The distinct-hex count is the
    claim that matters -- a single `True` could be a coincidence, 25 following a cursor cannot --
    and the contiguity of the `False` block is the off-board branch working rather than noise.
    Both arms of `UpdateHoverFromCursor` are now witnessed by a mouse. **The same poll against the
    Enhanced Input route gave 28/28 `False` and ZERO distinct hexes**, which is the comparison
    that makes these numbers mean something.
  - **WHAT THE ASSET CLEANUP DID, verified from saved bytes with a control at every step.** The
    C++ fix removed `HoverAction` and `OnHover` entirely, so all three assets were dead.
    - `IMC_Selection` lost its eighth row. Read the exported `DefaultKeyMappings` value, removed
      exactly the `IA_Hover`/`Mouse2D` element, set it back, read it back, saved. In the saved
      bytes `IA_Hover` went **2 -> 0** and `Mouse2D` **1 -> 0**, while the controls `IA_Select`
      and `IA_EndTurn` each still read **2**. Seven rows remain, in order.
    - `BP_StratPlayerController` was re-saved so the now-unknown property is dropped rather than
      merely ignored on load. **THE RESULT IS BYTE-IDENTICAL TO ITS PRE-DEFAULT STATE**: sha256
      `a4c3a209...` at 21848 bytes, matching the `8f888ea` blob exactly, with the `db3de97` blob
      (`4b52e88c...`, 22028 bytes) as the control proving the comparison can tell them apart.
      `git cat-file --filters` was used rather than `git show`, because these are LFS pointers.
      So the default this record spent a day setting is removed with zero collateral change.
    - `IA_Hover` was deleted. **The reference scan came FIRST and carried a control**: a single
      `grep -a -rl` over `Content/` and `Source/` found `IA_Hover` in its own asset and in two
      `.cpp`/`.h` files -- both dated amendment PROSE naming the removed symbol, not code --
      while the same scan for `IA_Select` returned `IMC_Selection` and the Blueprint, proving it
      finds real references.
  - **TWO MEASURED FACTS ABOUT THE NeoStack API, both new here.**
    - **`delete_asset` EXISTS; `remove_asset` and `asset_delete` DO NOT.** Probed by type before
      calling. Deleting from the shell is not an alternative while the editor is up: `rm` on a
      loaded `.uasset` fails `Device or resource busy`.
    - **A DATA ASSET TAKES `get`/`set` WITH ONE ARGUMENT, A BLUEPRINT TAKES TWO.**
      `a:get("self", "DefaultKeyMappings")` on an `InputMappingContext` answers
      `property "self" not found`; `a:get("DefaultKeyMappings")` answers. The Blueprint form
      recorded above -- `bp:get("self", "HoverAction")` -- is the other shape, and the two are
      not interchangeable.
  - **THE EDITOR STAGED THE DELETION ITSELF.** After `delete_asset`, `git status` showed
    `D  Content/StratInput/IA_Hover.uasset` in the INDEX, staged, without any `git add`. This is
    the mirror of the behaviour this record already carries in the other direction, where the
    editor leaves a saved asset unstaged as `AM`. Nothing here unstaged it; the user owns staging
    and was told.

- **THE HUMAN PLAYTEST RAN, AND THE HOVER DOES NOT REACH THE GAME: `Mouse2D` DELIVERS NO
  `Triggered` EVENT, WHILE EVERY OTHER LINK IN THE CHAIN IS PROVEN WORKING.** **THE HEADLINE IS
  ONE INFERENTIAL STEP PAST THE MEASUREMENT, and the step is marked here rather than left for a
  reader to notice.** What was MEASURED is an unchanged model field across 28 samples. That it
  means `OnHover` never ran follows from `AStratPlayerController::OnHover` doing nothing but call
  `UpdateHoverFromCursor`, which unconditionally sets or clears -- so no run is the only state
  that leaves a planted hex untouched. Sound, but derived. 2026-08-27, the
  same session and the same live editor as the entry below. **Attribution, separately as both
  clauses require:** the ACTING -- driving the live editor and its PIE session -- is the
  `coordinator` under `CLAUDE.md`'s **editor-driver clause**; the FILE WRITE is the `coordinator`
  under THIS file's header fallback condition. The shared measured absence and its control are
  recorded in the entry below and are unchanged. A human drove the mouse; nothing here was
  injected, because nothing in this project's automation reaches `UPlayerInput`. `global.md`
  carries what this means for the wave and any verdict; this file states no count and no verdict.
  - **THE DIFFERENTIAL, WHICH IS THE WHOLE FINDING.** Three polls carry it, each ~28-30 samples
    over ~56 seconds. A FOURTH exists on disk and is named here so its absence below is not read
    as concealment: an earlier 28-reading all-`False` read-only poll, discarded because the cursor
    was not witnessed to be over the board during it -- the user was reading a message at the
    time. It is not evidence and is not counted. The three that are, reading `UStratMatchSubsystem::AppliedModel`'s `Hover` through
    `playtest_console("GetAll ...")` and the log:
    - **Forced call, cursor parked on the board: 30/30 `bHasHoveredHex=True`.**
      `ke StratPlayerController UpdateHoverFromCursor` resolved a hex every time AND TRACKED THE
      CURSOR MOVING -- **five samples at `(1, 4)`, twenty-five at `(-1, 3)`**, the latter the hex
      `AppliedModel` gives Artillery `UnitId 4`. **[The split first written here was `ten` and
      `twenty-six`, and a gate blocked on it. The cause is worth more than the correction: the
      figures came from a `uniq -c` over the poller's OUTPUT FILE, which prints each reading once
      in its per-sample list and AGAIN in the six sample lines its own `readings 30   TRUE 30` foot
      repeats beneath itself. The instrument
      double-counted its own summary. `10 + 26 = 36` contradicted the `30/30` in the same
      sentence, so the entry refuted itself without leaving the bullet -- and the arithmetic was
      the only reader that could have caught it, since both figures looked plausible. Never
      derive a count from a file that summarises itself.]** So `HexUnderCursor`, `ApplyHoverChange`,
      `RefreshFromMachine`, `FStratHoverState::DecorateViewModel` and the model field are all
      sound.
    - **Read-only, cursor circling the board: 28/28 `bHasHoveredHex=False`**, after a confirmed
      clear. No forced calls in this poll at all.
    - **Plant-survival, cursor circling the board: 28/28 still `True (2, 7)`.** A planted hex was
      neither replaced by the hex under the cursor nor cleared. `distinct states` over the whole
      poll was a one-element set.
    The third is the decisive one and it is a THREE-WAY discriminator by construction: replaced
    means the path works, cleared means it runs and fails to resolve, unchanged means it never
    runs. It came back unchanged.
  - **WHAT IS PROVEN WORKING, and it is most of the wave.** The `HoverAction` default loads and
    `SetupInputComponent` binds it -- no `HoverAction is unset; no hover binding exists.` line
    exists in the PIE session's log, and that message shape is shared verbatim by all six actions.
    **THAT CONTROL IS THE WEAKER HALF OF THIS BULLET AND IS LABELLED RATHER THAN LEANED ON.** No
    `is unset` line of ANY of the six appears in this log, so nothing here demonstrates the
    channel can speak -- the shape is shared, which is not the same as the instrument being shown
    able to print it. What actually carries the conclusion is independent: the live mapping row
    below, and seven key rows that work through the same `SetupInputComponent`. **WHERE THAT
    SECOND SUPPORT WAS MEASURED IS NOT THIS SESSION'S LOG, and saying so is the point of naming
    it.** This session's log carries only two player-driven `LogStratPlay` lines, both
    `Guided opening skipped by the player`; the key rows working is carried by earlier passes,
    the closed SS2.11.6-B playtest among them, and by this session's human report of selecting a
    unit and seeing its movement space. Load-bearing and true, but not re-measured here.
    The mapping row is live in the RUNNING game, not merely in the asset:
    `GetAll InputMappingContext DefaultKeyMappings` returned eight rows ending
    `(Action="...IA_Hover.IA_Hover'",Key=Mouse2D)`, in the same array as the seven rows that
    demonstrably work -- which kills the hypothesis this file's own notes made most likely, that
    the new row had gone into the LEGACY `Mappings` array. That array printed EMPTY in the same
    dump, which is the control.
  - **THE HYPOTHESIS, STATED AS ONE AND NOT AS A FINDING.** `GetAll PlayerController
    bShowMouseCursor` returns **True** on the live `BP_StratPlayerController_C_0`. With a visible,
    uncaptured cursor, UE feeds no mouse AXIS input -- `MouseX`, `MouseY` and `Mouse2D` are
    look-input axes generated on capture -- while button and key events are unaffected. That is
    exactly the split measured: all seven key rows work and the one `Axis2D`-on-`Mouse2D` row
    does not, and `IA_Hover` being the project's only non-Boolean action is the one deliberate
    difference wave 0 introduced. **THIS IS NOT PROVEN AND IS NOT RECORDED AS PROVEN.** What
    would prove it is a log line inside `AStratPlayerController::OnHover`, which is C++, the
    engineer's lane, and a rebuild with the editor closed.
  - **AN INFERENCE MADE DURING THIS PLAYTEST AND RETRACTED BY THE NEXT MEASUREMENT, recorded
    because the reasoning was wrong in a way that reads as sound.** A planted hex was observed to
    go `False` after a mouse sweep, and the `coordinator` inferred from a `grep` of every
    `ClearHoveredHex` caller -- there are two, and only `UpdateHoverFromCursor`'s off-board branch
    is reachable without the console -- that `OnHover` must therefore be firing. The grep was
    right and the inference was wrong: THE SWEEP ENDED WITH THE CURSOR LEAVING THE VIEWPORT to
    reach the terminal, and clearing on an off-board cursor is the CORRECT behaviour that branch
    exists for. A right answer was read as evidence of a wrong one. The plant-survival poll, where
    the cursor never left the window, is what caught it.
  - **AND THE FIRST INSTRUMENT WROTE THE VALUE IT WAS BUILT TO OBSERVE.** The forced-call poll
    calls `UpdateHoverFromCursor` and then reads the field that call writes, so its 30/30 `True`
    cannot distinguish "Enhanced Input drove this" from "the poller drove this". It is retained
    above because it is sound for the ONE thing it does establish -- that the resolution path
    works -- and useless for the thing it was first reached for. The read-only and plant-survival
    polls exist because of it.
  - **WHAT THIS COSTS THE RECORD, and it is narrower than it looks.** Nothing in the commit that
    set the default is falsified: the default IS set, the row IS live, and both were verified from
    the saved bytes and now again from the running game. What is false is any reading of wave 0 in
    which the hover input surface reaches the game. PIE was left RUNNING at the time of writing.

- **THE `HoverAction` BLUEPRINT DEFAULT IS SET AND VERIFIED FROM THE SAVED PACKAGE BYTES, AND THE
  RELINK THAT MADE IT POSSIBLE WAS WITNESSED BY THE PROPERTY ITSELF.** 2026-08-27. **THE TWO
  PERMISSIONS ARE CITED SEPARATELY, because citing one authority for both halves is a finding.**
  The ACTING -- launching the editor and driving it -- is the `coordinator` under `CLAUDE.md`'s
  **editor-driver clause**. The FILE WRITE is the `coordinator` under THIS file's header fallback
  condition. **Both share one measured absence, shown with the control the clauses require:** a
  single `ToolSearch` `select:` lookup naming four tools returned exactly two schemas --
  `mcp__NeoStack_Connect__unreal_status` and `mcp__NeoStack_Connect__list_unreal_projects` -- and
  neither `mcp__NeoStack_Connect__execute_script` nor `mcp__unreal-editor-direct__execute_script`.
  The same lookup returning the tools that ARE served is what makes the absence a measurement
  rather than a name that failed to appear. `strat-editor-builder` therefore had no route to the
  editor at all, which is the condition both clauses name. The editor was driven over its own MCP
  endpoint with `Tools/neostack/ns.py`. `global.md` carries the suite figure and any verdict; this
  file states neither.
  - **THE EDITOR WAS NOT RUNNING AND WAS LAUNCHED ON THE INTEGRATION TREE.** No editor process
    existed (`Get-Process UnrealEditor` errored, which is the absence). Launched on
    `E:/MultiAgent/Stratocracy/Stratocracy.uproject`, pid 37412, and readiness taken from the
    window title REVERSING to `Stratocracy - Unreal Editor` with a 4.09 GB working set -- not from
    a non-empty title, which is the splash. `ns.py` then advertised `http://127.0.0.1:9315/mcp`
    with `editorPid 37412` and `projectPath E:/MultiAgent/Stratocracy/`, and
    `print(project_dir())` answered `E:/MultiAgent/Stratocracy/` from INSIDE the editor, which is
    the control that matters -- `runtimes.json` accumulates stale entries from other checkouts.
  - **THE RELINK IS WITNESSED BY THE SUBJECT OF THE WORK, WHICH IS THE STRONGEST FORM AVAILABLE
    HERE.** The entry below measured the pre-wave DLL as `class_properties` on
    `/Script/StratPlay.StratPlayerController` returning **91 entries**, of which the
    `Action|Mapping|Hover` filter returned exactly **seven**. The same two calls against the
    relinked editor return **92** and **eight**, the eighth being `HoverAction`. The other seven
    are unchanged and in the same order. No tooltip probe was needed: the property whose absence
    proved the stale binary is the property whose presence proves the fresh one.
  - **THE DEFAULT WAS SET ON THE CDO THROUGH `bp:set("self", ...)`, AND THE TARGET NAME WAS FOUND
    BY PROBE RATHER THAN GUESSED.** `cdo`, `default`, `CDO` and `defaults` all answered
    `[FAIL] -> target not found`; `self` answered, and the control that proves the READ works is
    that `bp:get("self", "OpenProductionMenuAction")` returned `IA_OpenProductionMenu` before
    anything was written. `HoverAction` read `None` before the set and the full `InputAction`
    object path after it. `compile()` -> success, `save()` -> saved.
  - **VERIFIED FROM THE SAVED BYTES WITH A POSITIVE CONTROL, NOT FROM THE `set` RETURN VALUE** --
    this file already records that an authoring call's return lies in both directions. On disk,
    `Content/StratPlay/BP_StratPlayerController.uasset` went 21848 -> 22028 bytes and
    `sha256 a4c3a209...` -> `4b52e88c...`. Occurrence counts in the package bytes, before -> after:
    `HoverAction` **0 -> 1** and `IA_Hover` **0 -> 2**. The control that makes those numbers mean
    something is an action already known to be wired: `SelectAction` reads **1** and `IA_Select`
    reads **2**, both before AND after -- so the new default has exactly the byte shape of a wired
    action, and nothing else in the package moved. `git status --short Content/` was EMPTY before
    the save and names this one file after it.
  - **WHAT THIS STILL DOES NOT PIN, and it is the same four properties.** That `HoverAction` is
    BOUND and to `Triggered`, that a mouse move produces a hover, that `UpdateHoverFromCursor`
    resolves a hex, and that `ApplyHoverChange` refreshes. The asset half of the first is now
    discharged -- the default exists and points at the right asset -- but the BINDING itself runs
    in `SetupInputComponent`, and nothing in this project's automation reaches `UPlayerInput`.
    A human at the mouse is the only instrument left, and the editor is open for it.

- **`IA_Hover` EXISTS AND IS MAPPED TO `Mouse2D` IN `IMC_Selection`, AND IT IS THE FIRST
  NON-BOOLEAN INPUT ACTION THIS PROJECT HAS.** 2026-08-27, written by `strat-editor-builder` over
  `mcp__unreal-editor-direct__execute_script`, which was on the tool surface from this session's
  FIRST call -- **THE FALLBACK CONDITION IN THIS FILE'S HEADER WAS NOT MET and the coordinator did
  not write this entry.** `print(project_dir())` answered `E:/MultiAgent/Stratocracy/` before
  anything was touched, so this is the integration tree and not a worktree. `playtest_status()`
  reports `has_pie_world=false, playing=false`; **no PIE was started, no `open_asset` was called on
  any Widget Blueprint, and no Blueprint was compiled** -- this batch is two data assets and
  nothing else. `global.md` carries what any of this does for the section; this file states no
  count and no verdict.
  - **TWO ASSETS MOVED, BOTH UNDER `/Game/StratInput`.** `IA_Hover` is NEW; `IMC_Selection` gained
    exactly one row. Neither derives from a Stratocracy C++ class -- `InputAction` and
    `InputMappingContext` are both `/Script/EnhancedInput`, an engine module, so the "which module
    declares the C++ parent" pre-flight has nothing here to get wrong. Read off
    `class_properties("/Script/EnhancedInput.InputAction")` returning its 11 properties, not
    assumed.
  - **`HoverAction` DOES NOT EXIST ON THE LOADED `AStratPlayerController`, MEASURED WITH THE
    CONTROL THAT MAKES IT A MEASUREMENT.** `class_properties` on
    `/Script/StratPlay.StratPlayerController` returns **91 entries**, and filtering them for
    `Action|Mapping|Hover` yields exactly seven: `SelectionMappingContext`, `MappingPriority`,
    `SelectAction`, `CancelAction`, `WaitAction`, `EndTurnAction`, `OpenProductionMenuAction`.
    **The same filter that would have caught `HoverAction` twice over returns the five action
    properties that DO exist and no sixth.** So the running editor's CDO has no such property and
    the `BP_StratPlayerController.HoverAction` default cannot be set in this session; that half of
    the wave's asset tail is BLOCKED until `feat/hover-input` merges and the editor relinks.
    **[STAMPED 2026-08-27, LATER THE SAME DAY -- DISCHARGED, NOT RETRACTED. Every measurement in
    this bullet was correct for the editor it was taken in; `feat/hover-input` has since merged
    (`1e32b18`), the editor was relaunched on the rebuilt tree, and the same `class_properties`
    calls now return 92 and eight. The default IS set and verified from the saved package bytes
    -- see the entry above this one. The BLOCKED half of the asset tail is closed; the BINDING
    half still needs a human at the mouse.]**
    **The trap this file already records was live again:** `class_properties` on the BLUEPRINT
    class path (`/Game/.../BP_StratPlayerController_C`) answered **`[OK] -> 0 entries`**, which is
    indistinguishable from "the property is absent" and proves nothing. Only the NATIVE class path
    answers.
  - **WHERE `IA_Hover` DIFFERS FROM `IA_Select`, AND IT IS ONE FIELD.** All five existing actions
    were read, not just one: every one of them is `ValueType=Boolean`,
    `AccumulationBehavior=TakeHighestAbsoluteValue`, `bConsumeInput=True`,
    `bTriggerWhenPaused=False`, `bConsumesActionAndAxisMappings=False`, `bReserveAllMappings=False`,
    with **zero `Triggers` and zero `Modifiers`** -- this project adds no trigger or modifier to
    any action and `IA_Hover` adds none either. `IA_Hover` matches all of that except
    **`ValueType`, which is `Axis2D`**, because a hover reports a continuous cursor position rather
    than a press. The second, smaller difference is convention-following rather than a departure:
    only the NEWEST of the five, `IA_OpenProductionMenu`, carries an `ActionDescription`; the four
    older ones are empty. `IA_Hover` follows the newer practice with
    `Report where the cursor is, continuously`.
  - **THE MAPPING ROWS, BEFORE AND AFTER, READ OFF THE ASSET.** Before, 7 rows:
    `IA_Select`/`LeftMouseButton`, `IA_Cancel`/`RightMouseButton`, `IA_Cancel`/`Escape`,
    `IA_Wait`/`W`, `IA_EndTurn`/`Enter`, `IA_EndTurn`/`SpaceBar`, `IA_OpenProductionMenu`/`B`.
    After, 8 rows: the same seven **in the same order and unchanged**, plus
    `[7] IA_Hover / Mouse2D` appended. `Mouse2D` is the engine's mouse XY 2D-axis key and imports
    from the bare name, which is this file's own recorded `FKey` rule (`Key=(KeyName="X")` writes
    an empty key silently) used rather than rediscovered.
  - **IN UE 5.8 THE ROWS ARE NOT IN `Mappings`, THEY ARE IN `DefaultKeyMappings.Mappings`, AND
    `Mappings` STILL EXISTS AND READS AS EMPTY.** `array_count("Mappings")` returns **0** on a
    context that plainly has seven rows, and `property_meta("Mappings")` reports it
    `editable=false`. Anyone reading the legacy array will conclude the context is empty. The live
    rows are the `FInputMappingContextMappingData` struct property `DefaultKeyMappings`, and this
    file's standing whole-struct workaround is still the WRITE route:
    `array_count("DefaultKeyMappings.Mappings")` fails `property not found` exactly as recorded.
    **What IS newly available is the dot-path READ** -- `get("DefaultKeyMappings.Mappings[0].Key")`
    returns `LeftMouseButton`, and the per-element `Action`/`Key` reads drove the before/after
    lists above. So: read per element with dot-path, write the whole struct.
  - **`set` LIED IN BOTH DIRECTIONS IN THIS PASS, AND THE READBACK IS THE ONLY THING THAT WAS
    RIGHT.** Two separate failures, and both matter to anyone writing a struct property here.
    (1) A malformed value returned **`true`** with a `[WARN] ... (no-op: value already equal, or
    engine reverted the write)` -- so a write that did nothing reported success. (2) The
    **correct** value returned **`[FAIL] Failed to set value ... Use list_properties() to see valid
    format`** and yet **APPLIED**: the readback grew 704 -> 797 bytes with the new row present, and
    it survived `save()` into the package. Note why that FAIL is spurious -- the error message
    echoes the offending string with its final `)` missing, i.e. **the diagnostic truncates its own
    echo**, so the text it complains about is not the text that was passed. **Never take `set`'s
    return or its log line as evidence on a struct property; diff the value you read back.**
  - **THE SELF-INFLICTED HALF, RECORDED BECAUSE IT WILL RECUR.** The first two attempts failed on
    string surgery, not on the API. The exported tail is `...Key=B)))` -- three closing parens that
    are the last ROW, the `Mappings` array, and the struct. Stripping three to append a row eats
    the last row's own paren and produces `Key=B,(Action=...`, which is silently a nested field
    rather than a new element. **Strip TWO.** The `Mouse2D` key name was never the problem, and a
    lane that had not run the control would have blamed it.
  - **VERIFIED AGAINST THE SAVED BYTES, NOT THE AUTHORING CALL, WITH A NEGATIVE CONTROL.**
    `IMC_Selection.uasset` is 5273 bytes at `2026-08-27T04:17:42Z` and its name table carries
    `IA_Hover` x2 and `Mouse2D` x1 beside all five pre-existing actions and their keys.
    `IA_Hover.uasset` is 1537 bytes at `04:16:56Z` and carries `Axis2D` x2 and
    `EInputActionValueType` x2 with **`Boolean` at ZERO** -- the value type is serialised because
    it differs from the C++ default, which is the same "a default is absent from the package"
    reasoning this project already relies on, used here in its positive direction. A
    `ZZZ_NOT_PRESENT` token scored 0 in both files, which is what makes the non-zero counts a
    measurement rather than a hopeful substring hit.
  - **TWO INSTRUMENTS ARE BLIND TO `.uasset` AND ONE OF THEM FAILS QUIETLY ENOUGH TO MISLEAD.**
    `read_file` on a `.uasset` refuses outright with `binary asset rejected` and returns zero
    bytes -- loud, fine. **`Grep`/ripgrep over `Content/StratInput` returns "No files found" for
    `LeftMouseButton`**, a string that is certainly in that package: rg skips binary files silently
    and the Grep tool exposes no `-a`. That control is the only reason those zeros were not read
    as "the row is not there". **The route that works:** `copy_file` the package to a
    non-`.uasset` extension, `read_file(..., {encoding="binary"})` -- which returns base64 -- and
    decode in Lua. A `utf8` read of the same copy returns **9 bytes** for a 5273-byte file, so that
    encoding is useless here. And the obvious decoder is too slow: a `gsub`-per-character
    bit-string base64 decoder hit `Script exceeded instruction limit (possible infinite loop)` on
    7 KB, while an arithmetic decoder stepping 4 chars at a time handled the whole 112 KB record
    file. **`read_file`'s `offset`/`limit` are IGNORED under `encoding="binary"`** -- it returns
    the entire file every time, which is convenient here but silently defeats any attempt to page
    a large binary.
  - **`list_properties` IS A METHOD ON THE ASSET TABLE, NOT A GLOBAL, AND ITS ENTRIES KEY ON
    `.name`.** The global call aborts the whole script with `attempt to call a nil value`, which is
    this file's recorded "the script died on line 2" shape. And where this file records entries
    keying on `.property`, `asset:list_properties()` entries key on **`.name`**, with `.value`,
    `.type` and `.category` -- a loop over `.property` prints a column of `nil` while cheerfully
    reporting the right property count.
  - **THIS ENTRY WAS SPLICED IN AT BYTE OFFSET 2900 AND THE FILE IS CRLF.** `Edit` was disabled on
    this session's tool surface, so the record write went through the editor's own `write_file` on
    the base64 route this file already documents. Two things the next lane needs: `content.md` is
    **CRLF**, so an entry written with bare `\n` ships as a mixed-ending file; and a base64 splice
    only concatenates cleanly on a 3-byte boundary, so the inserted text was padded with trailing
    spaces on its final blank line to make its length a multiple of 3. A `write_file` round-trip
    of a known string through a scratch file was run FIRST as the control, and `bytes_written`
    matched the source length exactly.
  - **WHAT THIS DOES NOT REACH.** Nothing here is a runtime observation. No controller property
    points at `IA_Hover` yet, so nothing consumes the mapping and no `STRAT-` line can witness it;
    the asset and the row are design-time facts verified in the package bytes and are claimed as
    nothing more. The `HoverAction` default is OWNED by whoever runs the editor after
    `feat/hover-input` merges and `UnrealEditor-StratPlay.dll` relinks -- and this file's recorded
    way to WITNESS that relink rather than assume it is to read the new property off the NATIVE
    class path, exactly as its absence was measured above.
    **[STAMPED 2026-08-27, LATER THE SAME DAY -- THIS IS THIS ENTRY'S HANDOFF SENTENCE AND IT WAS
    THE ONE THE FIRST CORRECTION MISSED. A `strat-integration-reviewer` gate blocked on it: the
    stamp 91 lines above was attached to the evidence and never reached the bullet that ASSIGNS
    the work, which is the one place a reader looks to find out what is owed. Both halves are now
    false. `feat/hover-input` merged at `1e32b18`; a controller property DOES point at `IA_Hover`
    -- `BP_StratPlayerController.HoverAction`, set and verified in this file's topmost entry -- so
    the mapping is consumed and the ownership sentence names work that is done. The prescription
    in it was FOLLOWED and is the only part that stands: the relink was witnessed by reading the
    new property off the NATIVE class path, exactly as this bullet said to. WHAT IS STILL TRUE is
    the first sentence -- nothing here is a runtime observation, and no `STRAT-` line has yet
    witnessed a hover.]**

- **SEC 2.11.4'S END-OF-MATCH SCREEN EXISTS, AND IT WAS SEEN DRAWING A REAL VERDICT OVER A REAL
  MATCH.** 2026-08-25. `WBP_MatchResult` is NEW and `BP_StratScoreboardHUD` gained one
  Blueprint default; nothing else under `Content/` moved.
  - **TWO AUTHORITIES, AND THEY ARE NOT THE SAME ONE. CORRECTED AFTER A `VERDICT: BLOCK`.** This
    entry first cited this file's header for the whole pass, and the header does not reach that
    far -- it says in terms that the fallback "licenses writing this file only -- it is not a
    general licence to act in the content lane". Separated, then. **The ASSET WORK was done by
    the `coordinator` on the USER'S EXPLICIT INSTRUCTION**, given after they were shown that
    testing before the merge required exactly this and that it departs from the topology's
    `Content/` rule. **THIS ENTRY WAS WRITTEN BY THE `coordinator` AS THIS FILE'S FALLBACK
    WRITER**, under the header's fallback condition, whose absence was measured with the control
    the header demands -- named here because the header's second obligation is to say so in the
    entry, and because the correction above deleted the sentence that first discharged it. The
    file write and the asset work now each name their actor. Neither authority covers the other
    half, and the first draft of this entry read as though one did.
    **[THE RULING CARRIED HERE WAS MADE ON 2026-08-26 AND THIS ITEM IS DISCHARGED. Stamped rather
    than deleted; the paragraph below is what was carried, and is kept because it is what named
    the gap.]** It read: *"**CARRIED FOR A RULING:** this is at least the fourth recurrence of the
    fallback condition, and the header was already amended once on 2026-08-23 for exactly that
    reason. A clause that has to be stretched every pass has stopped describing practice -- and
    the thing practice now needs a word for is not the file write, it is who may drive the editor
    when the lane's own agent cannot reach it."* **The user ruled that practice now has that
    word, and that it does not belong in this header.** `CLAUDE.md`'s **The crew** section carries
    an *editor-driver clause* naming the `coordinator` as the driver when `execute_script` is
    absent, under two obligations mirroring this header's own -- measure the absence with a
    control, and attribute the acting and the writing separately. This header keeps its file-write
    clause unchanged and points at that one; the two permissions are stated apart, which is what
    three gates in a row were asking for. This stamp was written by the `coordinator` as this
    file's fallback writer, the condition measured with its control on 2026-08-26.
  - **THE FALLBACK CONDITION, MEASURED WITH ITS CONTROL.** A `ToolSearch` naming all three of
    `mcp__unreal-editor-direct__execute_script`, `mcp__NeoStack_Connect__execute_script` and
    `mcp__NeoStack_Connect__unreal_status` returned **only `unreal_status`** -- so the lookup
    demonstrably serves the tools that exist and neither `execute_script` is on this session's
    tool surface. `strat-editor-builder` holds the NeoStack tools and no Bash, so it had no
    route to the editor at all.
  - **THE PROXY WAS LATCHED SHUT WHILE THE EDITOR WAS FINE, AND THE TWO PROXY TOOLS DISAGREED
    WITH EACH OTHER.** `mcp__NeoStack_Connect__list_unreal_projects` answered `Stratocracy:
    E:/MultiAgent/Strat-wt/slot-1/` while `unreal_status`, reading the SAME
    `C:/Users/me/AppData/Local/NeoStackAI/runtimes.json`, answered "No active NeoStackAI editors
    were found". `runtimes.json` itself carried a fresh heartbeat, `editorPid 99748`, and the
    endpoint URL. Another instance of the latch living in the PROXY alone.
  - **THE EDITOR'S OWN MCP ENDPOINT ANSWERS OVER PLAIN HTTP, AND THE PORT IS PER-INSTANCE.**
    JSON-RPC `initialize` / `notifications/initialized` / `tools/call` against the URL
    `runtimes.json` publishes returned `serverInfo unreal-editor 1.0.0-r4254` and a `tools/list`
    of exactly one tool, `execute_script`. **The port was 9316, NOT the 9315 this record has
    seen before** -- a second editor instance binds the next free port, so a probe that pins
    9315 will report a healthy editor as absent. Take the port from `runtimes.json`, never from
    memory.
  - **THE EDITOR WAS OPENED ON THE `slot-1` WORKTREE AND NOT ON THE INTEGRATION TREE**, on the
    user's explicit instruction, because `UStratMatchResultWidget` exists only on
    `feat/match-result-screen` and an asset cannot derive from a class the tree has never
    compiled. `print(project_dir())` answered `E:/MultiAgent/Strat-wt/slot-1/` from INSIDE the
    editor, which is the control that matters. This is a deliberate departure from the
    "`Content/` is touched on the integration tree only" rule and is safe only because no other
    lane was running: `BP_StratScoreboardHUD.uasset` is a binary edit on a branch, and a
    concurrent edit to it elsewhere would be lost silently rather than conflict.
  - **WHAT THE ASSET IS.** `/Game/UI/WBP_MatchResult`, reparented to
    `/Script/StratUI.StratMatchResultWidget` -- 18 widgets, one `EventGraph`, `0 errors, 0
    warnings`. `OnMatchResultRefreshed` breaks the pushed model and drives **eleven** `SetText`
    calls on one exec chain: the tier word, the faction line, and three rows x (label, YOU,
    ENEMY). **NO ARITHMETIC AND NO FORMATTING NODE ANYWHERE IN IT** -- `FStratScoreboardRow`
    already carries `Label`, `YouText` and `EnemyText` as `FText`, so the graph reads fields and
    assigns them. The live panel's own `WBP_Scoreboard` graph uses `Format Text` nodes for the
    same rows; this one does not need to, which is worth knowing before anyone "fixes" the
    asymmetry.
  - **PINS WERE LOCATED BY TYPE, NOT BY TYPED-IN NAME**, and that is the reusable part. The
    Blueprint display names carry inserted spaces -- `Tier Text`, `In Text`, `Strat Match Result
    Model`, and a `Get Row0Label` node whose output pin is named `Row 0Label` -- and a guessed
    literal that misses does not error, it leaves a pin unconnected and compiles green. Walking
    `pins_in`/`pins_out` for the first pin of the wanted TYPE wired 11 of 11 on the first run.
  - **NO IN-EDITOR CAPTURE PATH COMPOSITES UMG, AND THE LIVE SCOREBOARD IS THE CONTROL THAT
    PROVES IT.** `playtest_observe`, `screenshot({mode="asset"})` and `screenshot({mode=
    "active"})` all returned the 3D scene with NO widget in it -- and `WBP_Scoreboard`, which a
    human has watched draw during play, was equally absent from every one of them. So the
    instrument is blind, not the widget. **`playtest_console("shot showui")` DOES capture it**,
    writing `Saved/Screenshots/WindowsEditor/ScreenShot00000.png` -- the whole editor window,
    UMG included. `HighResShot`, with or without a `ShowUI` argument, does NOT.
  - **WHAT WAS ON SCREEN**, from that capture, over a live AI-vs-AI match on Ferrum Crossing
    that ended `STRAT-MATCH concluded turn=6 turnCap=20 sideToMove=1 tier=Decisive`: the word
    `Decisive`; beneath it `Their flag is down. We hold the ground. That's the whole report.`;
    then `Destroyed 75 / 775`, `Objectives 2/8 / 6/8`, `Unit HP 49 / 115` under YOU and ENEMY --
    with the live scoreboard still visible top-left showing `TURN 6 / 20` and the same three
    numbers. Sec 2.11.4's "the verdict is always a restatement of what was on screen all match"
    is therefore visible in one frame, both surfaces at once.
  - **THE VOICE WAS THE WINNER'S AND THE VIEWER LOST, WHICH IS THE WHOLE POINT OF THE CLAUSE.**
    `GetAll StratMatchResultWidget Model` on the live instance:
    `Winner=1, ViewingSide=0, bViewerWon=False, bViewerLost=True, WinnerFaction=Vanguard`,
    `Cause=FlagDestroyed`. The Directorate seat was shown the VANGUARD's victory line. Had the
    line been chosen from `ViewingSide`, this exact match would have printed the Directorate's
    line to the side that just lost.
  - **ALL ELEVEN TEXT BLOCKS WERE READ ON THE LIVE INSTANCE, NOT INFERRED FROM THE MODEL.**
    `GetAll TextBlock Text Name=<n>` for each: `TierText=Decisive`, `ResultLineText=Their flag is
    down...`, `Row0Label=Destroyed / 75 / 775`, `Row1Label=Objectives / 2-of-8 / 6-of-8`,
    `Row2Label=Unit HP / 49 / 115`. The same sweep also returned the on-disk asset defaults
    (`Row 0`, `0`) and the editor-preview instances under `/Engine/Transient.World_51`, both
    correctly untouched -- which is how one tells a graph that RAN from a widget that merely
    exists.
  - **THE HUD'S REFUSAL PATH IS SILENT WHEN THERE IS NO HUD**, and that is worth carrying.
    `ConcludeMatchIfEnded` logs `No end-of-match screen this match: ...` only INSIDE
    `if (FindScoreboardHUD())`; a null HUD logs nothing at all. So the absence of that warning
    does NOT prove the screen was shown, and this pass did not rest on it -- `GetAll
    StratScoreboardHUD MatchResultScreen` returned a live `WBP_MatchResult_C_0`, which does.
  - **FOUR SMALLER API FACTS MEASURED THIS PASS**, each of which cost a round. `bp:read_graph`
    does NOT exist -- graph nodes are `bp.graphs[name].nodes`. `list_properties` entries key on
    `.property` and `.value`, NOT `.name`, and a loop over `.name` prints nothing while
    reporting 149 properties found. `playtest_log_contains` returns `.ok` for "the call worked"
    and `.found` for "the text was there", so a loop breaking on `.ok` reads a clean miss as a
    hit -- it did, and it ended a 60-poll wait after 4 seconds. And
    `Saved/AutomationReport/index.json` is **UTF-8 with a BOM**: a plain `json.load` throws
    `Expecting value: line 1 column 1`.
  - **PIE WAS STOPPED CLEANLY AND NOTHING WAS LOST.** `playtest_stop` -> `playing=false,
    in_progress=false`, no `TransBuffer` assert, and `git status Content/` shows exactly the two
    intended paths. No `open_asset` was called at any point while PIE was live.

- **SEC 2.11.5'S PRODUCTION MENU HAS NOW BEEN SEEN WORKING BY A PERSON -- AND THE PLAYTEST
  CORRECTED THE INSTRUCTIONS IT WAS RUN FROM, NOT THE GAME.** 2026-08-25, written by
  `strat-editor-builder` over `mcp__unreal-editor-direct__execute_script`, which was on the tool
  surface from this session's FIRST call -- **THE FALLBACK CONDITION IN THIS FILE'S HEADER WAS NOT
  MET and the coordinator did not write this entry.** `print(project_dir())` answered
  `E:/MultiAgent/Stratocracy/` before anything else, so this is the integration tree and not a
  worktree. **THIS IS A RECORD PASS AND NO ASSET WAS TOUCHED:** no `open_asset`, no PIE, no
  compile, no save. The editor was live only to serve `write_file`, and this entry was spliced in
  with this file's own recorded `encoding="binary"` base64 route at byte offset 2021.
  - **THE BOXED-IN FOOTER IS CONFIRMED ON A SCREEN, BY THE USER, AND THE STATE TOOK TWO ATTEMPTS
    TO REACH.** Reported verbatim: "I ran the order and everything worked properly. There was a
    difference in how I got it to work. In step 4, I moved all units off of the factory hex so
    that it was empty. Selecting the production menu on a factory hex with all 6 adjacent hexes
    occupied did not produce the boxed in footer. For the next step I ended the round and during
    my turn I built another infantry unit so that the factory hex was occupied along with the 6
    adjacent hexes. That produced the boxed in message."
  - **THE GAME WAS RIGHT AND THE USER-FACING CHECKLIST WAS WRONG, AND THE RULE SAYS SO.**
    `spawnHexesBlocked`, in the vendored `Source/StratRules/Ui.good.cpp`, OPENS with a
    return-false when the factory hex ITSELF is unoccupied, before it reads a single neighbour.
    The spawner places on the factory hex first, so a free centre means there is somewhere to
    build no matter how full the ring is. Cited BY SYMBOL deliberately -- the doc-citation gate
    refuses live vendored line citations, and a line number in a vendored file is invalidated by
    the next re-vendor anyway. The coordinator's step 4 told the user to move the newly-built
    Infantry OFF the factory hex onto its free neighbour, which empties the one hex that must be
    occupied and so makes the target state unreachable BY CONSTRUCTION. Every dispatch said
    "its own hex AND all six neighbours"; only the user-facing checklist contradicted it, and the
    checklist is what the human actually followed.
    **SO, PLAINLY, AND IT EARNS ITS OWN SENTENCE: SIX NEIGHBOURS OCCUPIED WITH A FREE CENTRE IS
    NOT BOXED IN. Anyone constructing this state needs SEVEN hexes filled, the centre included.**
    That is the trap that cost a round here, and it will cost the next lane the same round if it
    is not read before the checklist is rewritten.
  - **WHAT THE HUMAN PASS CLOSES -- AND IT IS A PERSON AT A SCREEN, NOT AN INSTRUMENT IN THIS
    TREE.** The user reports everything worked properly: the greyed row, the `need N` string, the
    BUILD buttons enabled on available rows and disabled on unavailable ones, the footer sentence
    appearing, AND the Q31 half -- that the BUILD buttons stay CLICKABLE while the footer shows.
    The previous entry named that last half as the one a structural argument cannot close; it is
    now observed. **None of this is an instrument reading and none of it is claimed as one.** No
    tool in this tree can fire a Slate click, so a human observation is the only evidence this
    class of claim will ever have -- the same standing this file gave the objective ring, the
    turn-1a marker and the `Skip guidance` button, and for the same measured reason.
  - **THE DATA PATH BEHIND ALL OF THAT WAS MEASURED LIVE IN THE PREVIOUS PASS, AND THAT PASS'S
    WRITE-UP DIED WITH THE EDITOR.** The measurements below are that session's, reproduced here
    because the crash landed before they could be recorded; nothing in this bullet group was
    re-run today and nothing in it is a fresh reading.
    - **`RefreshProductionMenu` ON SIDE 0'S FACTORY -- authored `[1,4]`, which reads as axial
      `(-1,4)` live, the same hex printed in two coordinate systems -- RETURNED FOUR ROWS, one
      per Sec 2.4 row.**
    - **`Shortfall` IS PRICE MINUS LIVE FAME, CONFIRMED AT TWO FAME VALUES ACROSS FOUR PRICES.**
      At `FameTotal=200`, Tank at `300` gave `Shortfall=100`. After the build dropped side 0 to
      `100`, Tank / Artillery / Recon gave `200` / `100` / `50`. TWO Fame values is what makes
      this a subtraction observed rather than a constant that happened to match once.
    - **THE LIVE PIE WIDGET TREE CARRIED THE STRINGS:** `Row1Afford="need 200"`,
      `Row2Afford="need 100"`, `Row3Afford="need 50"`, and `Row0Afford` EMPTY. The affordable row
      draws no shortfall at all, which is the Select's `True` branch reaching the screen.
    - **OPACITY AND ENABLED TRACK DIFFERENT FIELDS, READ SEPARATELY, WHICH IS THE WHOLE POINT OF
      THE SPLIT.** `RenderOpacity` 1.0 on affordable rows and 0.4 on unaffordable ones;
      `bIsEnabled` tracking `bAvailable` ALONE.
    - **`bAffordable` AND `bAvailable` WERE OBSERVED DISAGREEING IN BOTH DIRECTIONS**, which is
      what turns the previous entry's derived sink lists from a structural argument into a live
      measurement: Tank unaffordable-but-available, and then Infantry affordable-but-unavailable
      carrying `Reason="that factory has already taken its build this turn"`.
  - **[RESOLVED 2026-08-25 -- MEASURED FALSE BY A PIXEL SAMPLE OF THE IMAGE ITSELF; THIS FILE'S OWN
    SINK LIST WAS RIGHT AND THE EYES WERE WRONG. Nothing in this bullet is deleted or reworded --
    the sub-bullets at its end carry the measurement and the correction. STAMPED 2026-08-25.]**
    **`ScreenShot00065.png` SHOWS THE MENU PAINTED AND LEGIBLE, AND A SECOND READER LOOKED AT
    IT.** The coordinator viewed the image independently and confirms the Tank row is visibly
    dimmer across its name, its cost and its shortfall. **AND IT ALSO CONFIRMS SOMETHING THAT
    DOES NOT SIT FLUSH WITH THIS FILE'S OWN RECORD, SO IT IS RECORDED RATHER THAN GLOSSED:** that
    row's BUILD button is drawn at the SAME brightness as the other three, while the entry below
    records `SetRenderOpacity` being applied to all FIVE of `Row{N}Name/Cost/Afford/Avail/Build`
    off the same `Affordable` Select. A filled `Button` may simply not read as dimmed the way
    text does at 0.4, or its style may not carry render opacity to the eye -- **neither
    explanation was measured, and this entry claims neither.** It is cosmetic and it cuts in the
    safe direction for Q31 (an affordance that stays legible is not a disabled-looking one), so
    nothing is being changed for it. It is logged here so the next reader meets it as an open
    observation instead of rediscovering it as a contradiction.
    - **WHAT SETTLED IT: A DIRECT PIXEL SAMPLE OF THE SAME PNG, AND IT REFUTES BOTH VISUAL
      READINGS.** The coordinator measured `ScreenShot00065.png` with Pillow -- greyscale, mean
      and max over a 45x20 box on each row's BUILD button and a 180x20 box on each row name:

      | Row | BUILD mean | BUILD max | Row-name max |
      | --- | --- | --- | --- |
      | Infantry | 173.1 | 255 | 245 |
      | Tank | 115.9 | 173 | 141 |
      | Artillery | 173.1 | 255 | 245 |
      | Recon | 178.7 | 255 | 245 |

      **THE TANK BUILD BUTTON IS DIMMED** -- max 173 against 255 on all three of the others, with
      its mean down by a third -- so the `SetRenderOpacity` sink list recorded above reaches
      `Row{N}Build` on the screen exactly as this file said it did.
    - **THE FALSE PREMISE CAME FROM THE COORDINATOR, NOT FROM THIS LANE, AND THAT IS THE MORE
      USEFUL HALF OF THE LESSON.** The "same brightness" reading was the coordinator's own
      eyeball reading of a DOWNSCALED view of the image, and it was then repeated to
      `strat-editor-builder` in the dispatch that produced the bullet above. The entry INHERITED
      a false premise from its dispatch rather than generating one. What this lane did right was
      to log the conflict with its own measured sink list as OPEN and explain neither side away
      -- had it reasoned the contradiction shut, there would have been nothing left to settle.
    - **A MEAN-AND-MAX PIXEL SAMPLE BEAT TWO INDEPENDENT VISUAL READINGS OF THE SAME IMAGE.**
      Both readings were of the same PNG, both were by a reader looking for exactly this thing,
      and both were wrong in the same direction. When a claim is about what an image SHOWS,
      sample the image -- a screenshot viewed is not a screenshot measured.
    - **WHAT THE MEASUREMENT RAISES THAT THE OPEN OBSERVATION DID NOT -- FILED FOR A DIRECTOR
      RULING, NOT AS A DEFECT AND NOT AS A FIX FOR THIS ASSET.** An unaffordable row's BUILD
      button is dimmed to the same `0.4` as the rest of its row while remaining FULLY ENABLED AND
      CLICKABLE, because render opacity does not affect hit testing. **So the button LOOKS
      disabled and is not.** That is a Sec 2.11.5 legibility question and it belongs to the
      Director; this lane changes nothing in `/Game/UI/WBP_ProductionMenu` for it and proposes no
      fix. **Q31 IS UNTOUCHED BY IT:** Q31 is about `bAvailable` driving `IsEnabled`, a different
      field read separately, and the player confirmed the click works.
  - **THE CRASH WAS A TEARDOWN ASSERT AND NO PACKAGE WAS EVER AT RISK.** `Assertion failed:
    false [PlayLevel.cpp:553]`, with `TransBuffer` holding the PIE `GameInstance`, at teardown
    after an injected `Escape` stopped PIE. The whole log carries ZERO `Saving Package` lines, so
    nothing was mid-write when it went down. It cost this lane a write-up, not an asset -- which
    is why the pass above is a record pass and touches nothing.
  - **THE INJECTION FINDING IS SHARPENED BY A CONTROL, AND IT IS NOT "INJECTION DOES NOT WORK".**
    In that same session fifteen injected keys produced ZERO game-side effect -- and `Escape` WAS
    delivered, and it STOPPED PIE. So **injection REACHES THE EDITOR and never reaches this
    project's Enhanced Input.** The `Escape` is the control that makes the fifteen a measurement
    about the game's input path rather than about a dead injector; without it the zero was
    equally consistent with an injector that does nothing at all. Everywhere this file says
    "injection never reaches this project's `UPlayerInput`", read it with that control attached.
    Those entries were right and none of them is withdrawn -- what changes is that the boundary
    now has a measured location instead of being a blanket.
  `global.md` carries what any of this does for the section; this file states no count and no
  verdict. 

- **SEC 2.11.5'S BOXED-IN FOOTER IS BUILT ON `/Game/UI/WBP_ProductionMenu`, AND THE T-UI-03
  SINK LISTS THE GATE COULD NOT VERIFY ARE NOW RE-DERIVED FROM A SECOND INDEPENDENT READ.**
  2026-08-25, written by `strat-editor-builder` over
  `mcp__unreal-editor-direct__execute_script`, which was on the tool surface from this
  session's FIRST call -- **THE FALLBACK CONDITION IN THIS FILE'S HEADER WAS NOT MET and the
  coordinator did not write this entry.** `print(project_dir())` answered
  `E:/MultiAgent/Stratocracy/` before anything was touched, so this is the integration tree and
  not a worktree. `playtest_status()` reported `has_pie_world=false, playing=false` BEFORE any
  `open_asset`, which is this file's own recorded precondition for reading a Widget Blueprint.
  - **THE RELINK WAS WITNESSED RATHER THAN ASSUMED, and the witness is the new verb's own
    tooltip.** The editor's binaries are newer than the previous session's.
    `class_methods("/Script/StratPlay.StratMatchSubsystem")` returns 21 entries and one of them
    is `IsOpenMenuFactorySpawnBlocked`, `returns=Bool`, `category=Stratocracy|Production`,
    carrying the multi-paragraph tooltip that matches `StratMatchSubsystem.h` on disk verbatim,
    its `TRUE IS NOT A REASON TO GREY A BUILD BUTTON, AND Q31 IS WHY` paragraph included. A
    function absent from the old binary cannot be reflected off it, so its presence IS the
    relink. The NATIVE class path was used; this file already records that the BP class path
    answers 0 entries with an `[OK]` and proves nothing.
  - **ONE ASSET MOVED: `/Game/UI/WBP_ProductionMenu`**, parent `UserWidget` from `/Script/UMG`
    -- an engine class, read off `bp.parent_class` rather than assumed, so the "which module
    declares the C++ parent" pre-flight has no Stratocracy module to get wrong here. The verb it
    now calls is declared in `Source/StratPlay/StratMatchSubsystem.h`, module **`StratPlay`**,
    verified by that file's path in the tree AND by `class_methods` on
    `/Script/StratPlay.StratMatchSubsystem` answering. `PaintRows` went 122 -> 126 nodes and
    164 -> 169 connections; the widget count went 35 -> 36. `EventGraph`'s 70 nodes are
    untouched, and so is every other widget.
  - **WHAT WAS ADDED: ONE WIDGET AND FOUR NODES.** `FooterText`, a `TextBlock`, appended as the
    FOURTH child of `Stack` (after `TitleText`, `StatusText`, `Grid`), `is_variable=true`,
    Roboto Regular 12, amber `(0.98, 0.75, 0.18)`, slot padding `Top=8`, `HAlign_Left`. The name
    was checked disjoint BEFORE creation -- `find_widgets("FooterText")` returned 0 and the
    Blueprint reports 0 member variables -- which is this file's own recorded collision trap,
    used rather than rediscovered. In `PaintRows`, appended to the exec tail:
    `IsOpenMenuFactorySpawnBlocked` -> `Select.Index`; `Select.Return Value` ->
    `SetText.In Text`; `Get FooterText` -> `SetText.Target`; and the previous terminal
    `SetRenderOpacity.then` -> `SetText.execute`. It rides the rows' clock because it is inside
    the rows' own function, which is the property the C++ block says it designed for.
  - **THE SWAP IS THE TEXT, NOT THE VISIBILITY, AND AN API MEASUREMENT DECIDED IT.** Sec 2.11.5
    specifies the sentence and not the mechanism, so this was a real choice. Visibility-swapping
    was the first design, because `Collapsed` costs zero layout, and it was ABANDONED ON A
    MEASUREMENT: **`configure_widget` CANNOT SET `Visibility` AT ALL.** `Collapsed`, `Hidden`,
    `SelfHitTestInvisible` and `ESlateVisibility::Collapsed` each returned `no changes applied`
    and each read back `Visible`. **The control is `MinDesiredWidth` on the SAME widget in the
    SAME call style, which applied and read back `120.000000`** -- so the refusal is a property
    of that property and not of the call. `bp:set_property("FooterText", {Visibility=...})`
    returned `false` and there is no global `set`. A visibility swap would therefore have
    shipped a footer whose DESIGN-TIME default is `Visible` WITH THE SENTENCE IN IT: a wrong
    positive claim on screen for any frame before the first `PaintRows`, and permanently in the
    asset preview. The text swap has no such window -- the design-time `Text` is EMPTY and the
    sentence exists only as the `True` literal on the Select, so nothing can paint it until the
    boolean is actually true.
    **WHAT IT COSTS, stated rather than glossed:** an always-`Visible` empty `TextBlock` still
    reserves its line box, so an unblocked menu carries roughly one blank text line plus 8px of
    padding under the grid. That is the price of the API limit above and it is cosmetic. If
    `Visibility` ever becomes settable, the visibility swap is the better shape.
  - **THE SENTENCE IS BYTE-EXACT, CHECKED AS BYTES.** The `True` literal reads
    `Boxed in — build waits for a free hex.` at 40 bytes, and bytes 10-12 are
    `226,128,148` -- U+2014 EM DASH in UTF-8, not a hyphen and not an en dash. The `False`
    literal is the empty string, 0 bytes. Round-tripped through `get_pin` and compared to the
    source string for EQUALITY, not eyeballed.
  - **Q31 IS SATISFIED STRUCTURALLY, AND IT WAS DERIVED RATHER THAN ASSERTED.**
    `IsOpenMenuFactorySpawnBlocked` reaches **EXACTLY ONE** sink in the whole graph and it is
    `Select.Index` on the footer's Select; that Select's only output goes to the footer's
    `SetText.In Text`. Walked independently, every `SetIsEnabled.In Is Enabled` in the graph is
    fed by `Break Strat Build Option View.Available` and by nothing else -- four of them, with
    the four `Row{N}Build` getters on the `Target` pins. So the boolean is not ANDed into
    anything, touches no `IsEnabled`, and the Build buttons stay enabled while the footer shows.
    That is what the 2026-08-22 Q31 ruling requires, and it is a property of the derived edge
    list rather than a hope.
  - **THE GATE'S UNGATED SELF-REPORT IS NOW REPRODUCED, AND IT AGREES.** The reviewer proved
    from the package bytes that no arithmetic, comparison or boolean-conjunction node exists in
    this asset, but the name table gives names and not edges, so the earlier sink derivation was
    recorded as a lane self-report nobody had re-run. Re-derived here from a FRESH `read_graph`,
    walking every connection leaving a `Break` node: `Affordable` -> 8 sinks, every one
    `Select.Index`; `Available` -> 4, every one `SetIsEnabled.In Is Enabled`; `Shortfall` -> 4,
    every one `To String (Integer).In Int`; `Reason` -> 4, every one `SetText (Text).In Text`;
    `Cost Fame` -> 4 `To Text (Integer).Value`; `Id` -> 4 `To Text (Name).In Name`. **EVERY
    COUNT MATCHES THE EARLIER MEASUREMENT EXACTLY; NOTHING DISAGREES**, so that claim no longer
    rests on a single unreproduced read. A full node census sums to 126 with no `Add`,
    `Subtract`, `Greater`, `Less`, `Equal`, `Boolean AND`/`OR` or `To Text (Boolean)` node of
    any kind.
  - **A `To Text (Boolean)` NODE WAS SILENTLY CREATED DURING THIS PASS AND WAS CAUGHT BY THE
    CENSUS RATHER THAN BY EYE -- and it is the exact defect class this asset was cleaned of.**
    An attempt to spawn the `Select` by dragging off the boolean auto-connected it to
    **`Option 0`, an OPTION pin, rather than to `Index`**. That resolved the Select's value type
    to `bool`, and the follow-up `connect(Return Value -> In Text)` reported `-> with
    conversion` and inserted a `To Text (Boolean)`. Shipped, the footer would have read `true` /
    `false`. **`connect` returning `true` is therefore NOT evidence of a clean wire: read its
    `direct` vs `with conversion` suffix, and re-census the graph after any auto-connect.** Both
    discarded Selects and the conversion node were deleted and the final census reports
    `To Text (Boolean)` at ZERO. Note also that `add_node`'s auto-connect picks the first
    COMPATIBLE pin, and on a wildcard Select a bool matches an option before it matches `Index`.
  - **COMPILED, VERIFIED, SAVED, IN THAT ORDER.** `read_log("compile", {asset=...})` reads back
    `UpToDate, 0 errors, 0 warnings`. The exec chain walks **41** steps from the entry --
    17 `SetText`, 4 `SetIsEnabled`, 20 `SetRenderOpacity` -- terminating on the footer's
    `SetText`, and NO node in the graph is unlinked. `save()` returned `saved`. A
    `screenshot({mode="asset"})` of the design-time preview shows the four headers and the
    placeholder rows with the footer PRESENT AND EMPTY, which is exactly the unblocked state;
    the preview CANNOT show the sentence, because `PaintRows` runs at runtime and the preview
    never calls it. That image is evidence about the empty state and about nothing else.
  - **WHAT THIS DOES NOT REACH, and it is the same last link every surface in this file has
    needed.** Nobody has seen the sentence on a screen. No instrument here can fire a Slate
    click and injection still never reaches this project's `UPlayerInput`, so the run that would
    show it is a HUMAN one: open the production menu on a factory whose own hex and all six
    neighbours are occupied, confirm the sentence appears, and confirm IN THE SAME FRAME that
    the BUILD buttons on that menu are still clickable -- the second half is the Q31 half and it
    is the one a structural argument cannot close. Confirm too that it clears when the menu
    moves to an unblocked factory. Nothing above is claimed as observed at runtime.
  - **FOUR MEASURED API FACTS, all 2026-08-25. TWO OF THEM CORRECT ENTRIES ALREADY IN THIS
    FILE, and the corrections are stated here because this is where the next lane will look.**
    (1) **CORRECTION TO THIS FILE'S `UK2Node_Select` NOTE.** The recorded rule is that the
    option pins rename to `True`/`False` "the moment a bool reaches `Index`". That is half
    right, and the missing half decides the outcome: **the rename happens only if the option
    pins are still WILDCARD when `Index` is connected.** Wire the output first -- which this
    file elsewhere recommends, for the separate and still-correct reason that a Select whose
    options are only literals stays wildcard and will not compile -- and the pins resolve to
    `text` but keep the generic names `Option 0` / `Option 1` FOREVER, surviving `bp:refresh()`
    and a clean compile. Measured both ways in this pass, on two nodes. **The order that
    satisfies both constraints is `Index` FIRST, then `Return Value` into a typed pin.** Done
    that way the pins are born `False` / `True` and the output wire reports `direct`.
    (2) **CORRECTION TO THIS FILE'S "THERE IS NO PIN INSPECTION CALL AT ALL".** `get_pin`,
    `set_pin` and `reset_pin` all exist, under `help("SetPin")`. `get_pin` returns a pin's
    DEFAULT VALUE, so the old note is right that nothing LISTS pins by return value -- but
    **`get_pin` with a deliberately bogus pin name prints the full listing in its FAILURE
    message**: names, types and directions, e.g. `Available: False (in, text), True (in, text),
    Index (in, bool), Return Value (out, text)`. That is the pin inspector this file says does
    not exist, reached through an error rather than a return, and it was used throughout this
    pass. `read_graph` still returns no pins, so that half of the old note stands.
    (3) **`Option 0` IS `False` AND `Option 1` IS `True` ON A BOOL SELECT, measured off an
    EXISTING node rather than reasoned about.** The rows' opacity Select reads `True = 1`,
    `False = 0.4`, `Option 0 = 0.4`, `Option 1 = 1` -- so `get_pin` accepts both the display
    name and the positional alias for the same pin, and index 0 is the FALSE slot. This is what
    lets a Select that failed to rename still be wired correctly instead of guessed at.
    (4) **`configure_widget` TAKES SLOT PROPERTIES ONLY THROUGH A NESTED `slot = {}` TABLE.**
    `{slot_Padding=...}` and a bare `{Padding=...}` both warn `property not found or not
    editable on widget` and apply nothing; `{slot={Padding=..., HorizontalAlignment=...}}`
    reported 3 changes and read back. The flat forms fail LOUDLY, which is the good case, but
    the warning names the property rather than the shape and reads as "this widget has no
    padding".
  `global.md` carries what any of this does for the section; this file states no count and no
  verdict.  


- **SEC 2.11.5'S ROWS NOW SAY `need N` AND GREY THEMSELVES, AND `IsEnabled` IS WIRED TO
  `bAvailable` ALONE.** 2026-08-25, written by `strat-editor-builder` over
  `mcp__unreal-editor-direct__execute_script`, which was on the tool surface from this session's
  FIRST call -- **THE FALLBACK CONDITION IN THIS FILE'S HEADER WAS NOT MET and the coordinator
  did not write this entry.** `print(project_dir())` answered `E:/MultiAgent/Stratocracy/`
  before anything was touched, so this is the integration tree and not a worktree.
  `playtest_status()` reported `has_pie_world=false, playing=false` BEFORE any `open_asset`,
  which is this file's own recorded precondition for reading a Widget Blueprint at all.
  - **ONE ASSET MOVED: `/Game/UI/WBP_ProductionMenu`**, parent `UserWidget` from `/Script/UMG`
    -- an engine class, so the "which module declares the C++ parent" pre-flight has no
    Stratocracy module to get wrong here; read off `widget_info().parent_class` rather than
    assumed. `PaintRows` went 62 -> 122 nodes and 76 -> 164 connections. The widget tree, all
    35 widgets and the `EventGraph` are untouched but for two column headers.
  - **THE MENU HAS NO PROPERTY BINDINGS AND NO MEMBER VARIABLES -- IT PAINTS IMPERATIVELY, and
    that decided the shape of this work.** `list_bindings()` returns 0 and the Blueprint reports
    0 variables; every cell is a `SetText` inside the `PaintRows` function graph, driven off
    `Get StratMatchSubsystem -> Get ProductionMenu -> Get (a copy) -> Break Strat Build Option
    View`, four times over with a literal row index. So `IsEnabled` was set with `SetIsEnabled`
    in that same exec chain rather than with `add_binding`: there is no variable for a binding
    function to read, and minting one would have put row state in the widget. **This is why the
    `pure=true`-plus-exec-wire rule this file spent four sessions on did not apply** -- nothing
    added here is a binding function.
  - **WHAT EACH ROW NOW READS, one field per drawn thing and no arithmetic anywhere:**
    `Row{N}Build.SetIsEnabled` <- `Break{N}.Available` AND NOTHING ELSE. `Row{N}Afford` <- a
    `Select` on `Break{N}.Affordable`: True -> the empty Text default, False ->
    `Append("need ", To String (Integer)(Break{N}.Shortfall))` through `To Text (String)`.
    `Row{N}Avail` <- `Break{N}.Reason` directly -- the module's own words, empty when
    `bAvailable` is true, which is what that field's block says it is for. Greying is
    `SetRenderOpacity` on all five of `Row{N}Name/Cost/Afford/Avail/Build`, fed by a SECOND
    `Select` on `Break{N}.Affordable` (True 1.0, False 0.4). Render opacity does not affect hit
    testing, so a greyed row stays clickable -- which is what Q31 needs and is a property of
    the node chosen, not a hope.
  - **THE TWO SUBSTITUTIONS T-UI-03 EXISTS TO CATCH WERE CHECKED BY DERIVING THE SINK LIST, NOT
    BY EYE.** A fresh `read_graph` was walked for every connection leaving a `Break` node.
    `Affordable` reaches EXACTLY eight sinks and every one is a `Select.Index`; `Available`
    reaches EXACTLY four and every one is `SetIsEnabled.In Is Enabled`; `Shortfall` reaches only
    `To String (Integer).In Int`; `Reason` reaches only four `SetText.In Text`. So nothing greys
    off `Shortfall > 0`, nothing ANDs the two booleans, and no `IsEnabled` reads `Affordable`.
    Per-row pairing was checked the same way and by HANDLE rather than by canvas position: row
    N's break node feeds row N's widgets, on all four rows.
  - **THE `true`/`false` TEXT IS GONE BY COUNT, NOT BY INSPECTION.** All eight
    `To Text (Boolean)` nodes were deleted and a fresh read reports ZERO remaining. The exec
    chain walks 40 steps from the entry -- 16 `SetText`, 4 `SetIsEnabled`, 20
    `SetRenderOpacity` -- and no node in the graph is unlinked. Two headers followed the data
    they label: `AFFORD` -> `SHORTFALL`, `FACTORY` -> `STATUS`.
  - **COMPILED AND SAVED.** `read_log("compile", ...)` read back `UpToDate, 0 errors,
    0 warnings` after the change and `save()` returned `saved`. A `screenshot({mode="asset"})`
    of the design-time preview shows the new headers over the four placeholder rows; the
    preview CANNOT show `need N`, because `PaintRows` runs at runtime and the preview never
    calls it. That screenshot is evidence about the headers and about nothing else.
  - **WHAT THIS DOES NOT REACH, and it is the same last link every surface in this file has
    needed.** Nobody has seen a greyed row, a `need N` string, or a BUILD button refusing a
    click. No instrument here can fire a Slate click and injection still never reaches this
    project's `UPlayerInput`, so the run that would show it is a HUMAN one: open the production
    menu on a factory whose buildlist holds at least one row this side cannot afford, and at
    least once on a factory that has already built this turn. Nothing above is claimed as
    observed on a screen.
  - **THE FOOTER HALF OF 2a-iii WAS NOT BUILT, AND THE MEASUREMENT SAYS IT CANNOT BE BUILT HERE.**
    Sec 2.11.5's boxed-in footer wants `FStratFactoryView::bSpawnBlocked`. It sits on
    `FStratFactoryView`, reachable only through `FStratViewModel::Factories`, and
    `UStratMatchSubsystem::GetViewModel` is deliberately NOT a `UFUNCTION` -- its own
    declaration states that UHT will not take a reference return. No other header puts an
    `FStratFactoryView` on a Blueprint surface, so there is no Lua or Blueprint route to that
    boolean. It needs a C++ accessor first. OWNED: `strat-gameplay-engineer`; the content lane
    cannot edit `Source/`. This lane's own probing AGREES with the brief that assigned it.
  - **THREE MEASURED API FACTS, all 2026-08-25 and none of them recorded here before.**
    (1) `UK2Node_Select` spawns with option pins named `Option 0` / `Option 1` and RENAMES them
    to `True` / `False` the moment a bool reaches `Index`, so `connect(..., "False")` works only
    after that connection exists; and the node's VALUE type resolves from whatever is wired to
    an option or to the return, so a Select whose options are only literals stays wildcard and
    will not compile. Wire the output first, set the literals second.
    (2) The delete function is `delete_node`; `remove_node` does not exist. And there is NO pin
    inspection call at all -- `node_info`, `get_node`, `list_pins`, `node_pins`, `describe_node`
    and `get_pins` are every one of them `nil`, checked in a single call as its own control, so
    `add_node`'s returned `pins_in` / `pins_out` is the only pin listing available and it is a
    snapshot from spawn time. `read_graph` returns nodes and connections and no pins.
    (3) Lua's `io` AND `os` are both absent from this sandbox. So the UTF-16 `write_file` trap
    recorded above has no standard-library escape hatch; the base64 route is the only one, and
    this entry was spliced into this file with it at byte offset 2021.
  `global.md` carries what any of this does for the section; this file states no count and no
  verdict.
 
- **THE `Skip guidance` BUTTON EXISTS AND IS PAINTED ON THE STRIP -- THE ONE SURFACE
  Sec 2.11.6-B's closure was recorded OVER.** 2026-08-24. `global.md` carries what this does for
  that `OWNED:` line; this file states no count and no verdict.
  **WRITTEN BY THE COORDINATOR UNDER THIS FILE'S FALLBACK CONDITION, WITH THE CONTROL THE HEADER
  REQUIRES.** `execute_script` was absent from this session's tool surface: a `select:` lookup for
  `mcp__NeoStack_Connect__execute_script`, `mcp__unreal-editor-direct__execute_script` and
  `mcp__NeoStack_Connect__unreal_status` returned **exactly one** schema, `unreal_status` -- so the
  same lookup that found nothing for the two `execute_script` names DID return the tool that is
  served, which is what makes the absence a measurement rather than a missing name. Driven over
  the editor's own MCP endpoint with `Tools/neostack/ns.py`, probed with
  `lua("print(project_dir())")` -> `E:/MultiAgent/Stratocracy/`, so the evidence below is this
  tree's.
  - **THE EDITOR WAS CLOSED WHEN THIS PASS STARTED, AND THE STANDING NOTE SAID IT WAS OPEN.**
    `runtimes.json` held `"runtimes":[]`, no `UnrealEditor.exe` was in `tasklist` (control: the
    same command listed 915 processes), and `ns.py` raised `no live MCP port among 0 advertised
    url(s)`. The editor was launched from
    `C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe` on the `.uproject`
    and advertised `http://127.0.0.1:9315/mcp` within 50 s. A UMG button cannot be authored
    without a live editor, so this is the whole of why the pass began with a launch.
  - **WHAT WAS ADDED, to `/Game/UI/WBP_DirectiveStrip` and to nothing else.** `SkipButton`, a
    `Button` appended as the third child of `StripRow` (after `DirectiveText` and `WindowEndTag`),
    holding one `TextBlock` `SkipLabel` reading `Skip guidance`. Names chosen disjoint from every
    member variable, which is this file's own recorded collision trap. `bind_event` auto-promoted
    `SkipButton` to a variable, which is the widget's own variable and not a second one.
  - **THE GRAPH, AND THE PALETTE LIMIT THAT SHAPES IT.** `EventGraph`:
    `On Clicked (SkipButton)` -> `Cast To StratPlayerController` -> `Skip Guidance`, with
    `Get Owning Player` feeding the cast's `Object` and the cast's `As Strat Player Controller`
    feeding the call's `Target`. **`find_nodes` IS FILTERED TO `self`'s CLASS AND CANNOT SEE A
    FOREIGN CLASS'S VERBS AT ALL.** `find_nodes("Skip Guidance", …)` returned **0** from this
    widget -- and so did `Set Show Mouse Cursor` and `Client Restart`, two engine
    `BlueprintCallable`s on `APlayerController` that certainly exist. Those two are the control
    that makes the zero a property of the tool rather than of the function. The route that works
    is the **pin-context form** documented only in `help("FindNodes")`:
    `find_nodes({query=…, asset_path=…, graph_name=…, from_handle=<cast>, from_pin="As Strat
    Player Controller"})`, which is the API's "drag off a pin" and returned `Skip Guidance` at
    score 100 immediately. Anyone wiring a call to another class's UFUNCTION needs this; the
    plain form will read as "the function does not exist".
  - **VERIFIED FROM A FRESH `read_graph`, NOT FROM THE GREEN COMPILE.** All five links report
    `connected=true, is_orphaned=false` in both directions. The four pre-existing binding function
    graphs were re-read in the same pass and are unchanged -- each still exec-wired with
    `Return Value` connected, which is the defect this file spent four sessions on and which a
    structural edit could plausibly have disturbed.
  - **IT IS ON THE SCREEN, AND IT GOES OUT WITH THE STRIP.** PIE from the game's own startup path
    (`Game class is 'BP_StratGameMode_C'`, `Guided opening armed for side 0: objective hex (2, 7),
    window turns 1-4`; no `.sav` existed, so the suppression trap this file records did not
    apply). The live instance
    `UnrealEdEngine_0:GameInstance_0.WBP_DirectiveStrip_C_0` carried `bActive=True, Beat=Beat1a`
    and its `WidgetTree_0` held `SkipButton` and `SkipLabel.Text = "Skip guidance"`.
    `Shot showui` at 2544x1320 shows the chip painted inside the strip immediately right of the
    directive line. **THE POSITIVE CONTROL:** `ke * SkipGuidance` -- `1 instances succeeded` --
    dropped the projection to `bActive=False, Beat=None`, and the next capture shows the strip and
    the button gone TOGETHER. The button carries no visibility binding of its own; it inherits
    `StripBorder`'s, which is what that control demonstrates rather than asserts.
  - **WHAT THIS DOES NOT REACH, and it is the same last link the ring and the marker needed.**
    No mouse event was injected -- injection still never reaches this project's input -- so
    "a Slate click on that rectangle fires `OnClicked`" is NOT measured here. What is measured is
    everything up to it: the button is in the live widget tree, painted, hit-testable by
    construction (`StripRow` is `SelfHitTestInvisible`, so its children hit-test), and its
    `OnClicked` is bound to a compiled graph whose every pin reads connected. A human click is
    what closes it, exactly as one closed the ring and the marker.
    **[CLOSED 2026-08-24, later the same evening: THE USER CLICKED IT AND REPORTS THAT IT WORKS.**
    That is the whole of the evidence and it is stated as what it is -- a human observation at the
    keyboard, not an instrument reading, which is the same standing this project gave the ring and
    the turn-1a marker and for the same reason: no instrument here can fire a Slate click. The
    paragraph above was true when written and nothing in it is withdrawn; what changes is that the
    link it names as inferred is now observed.]**

- **THE OBJECTIVE RING AND THE TURN-1a MARKER ARE ON THE SCREEN, AND EACH WAS IDENTIFIED BY BEING
  REMOVED RATHER THAN BY MATCHING A SHAPE.** 2026-08-23, written by `strat-editor-builder` -- the
  first entry this lane has written here since the tool-surface condition began -- over
  `mcp__unreal-editor-direct__execute_script`, which was present from the session's first call.
  Log stamps below read `2026.08.24-02.5x` / `-03.0x` because logs are UTC and this straddles local
  midnight; both dates are right, do not reconcile them. `global.md` carries what this does for the
  section; this file states no count and no verdict.
  - **WHAT WAS CREATED, three assets, all verified by read-back after save:**
    `/Game/StratArt/Materials/MI_Overlay_Objective` (parent `M_Translucent`, the same parent
    `MI_Overlay_Reach` and `MI_Overlay_Target` carry -- read off those two rather than guessed),
    `/Game/StratArt/Materials/MI_Marker_Guided` (same parent, so ring and marker are ONE guidance
    channel and cannot drift apart in hue), and `/Game/StratArt/Meshes/SM_GuidedMarker`, a
    duplicate of `/Engine/BasicShapes/Cone` -- 100 uu tall, 100 uu across, apex up, centred on its
    origin. Duplicating an engine basic shape is this project's existing practice, not a new one:
    `BP_StratUnit.FallbackMesh` has been `/Engine/BasicShapes/Cylinder` since phase 5.
  - **WHAT WAS SET, ON WHICH BLUEPRINT, AND THE NAMES ARE `BP_StratBoard` AND `BP_StratUnit`.**
    `BP_StratBoard.ObjectiveMaterial` -> `MI_Overlay_Objective`. `BP_StratUnit.GuidedMarkerMesh` ->
    `SM_GuidedMarker` and `BP_StratUnit.GuidedMarkerMaterial` -> `MI_Marker_Guided`. Both compiled
    clean and saved. Read back afterwards, `ReachMaterial`, `TargetMaterial`, `OverlayMesh`,
    `OverlayZOffset` (3.0), `HexSize` (200.0), `GuidedMarkerZOffset` (150.0, a real C++ default and
    never part of this work), `FallbackMesh` and `BodyZOffset` (50.0) are all unchanged.
  - **THE RING NEEDED NO MESH, AND THIS RECORD SAID IT DID -- CORRECTED WHERE IT WAS WRITTEN.**
    `ObjectiveOverlay` is a `UHierarchicalInstancedStaticMeshComponent` instancing the SHARED
    `OverlayMesh`; `StratBoardActor.h` states it as "One mesh, three components, three materials",
    and `OverlayMesh` was already `SM_HexOverlay`. So the ring is a MATERIAL ONLY and the marker is
    the only new mesh. The three older bullets that said "the ring and marker meshes" and that
    named `BP_StratBoardActor` / `BP_StratUnitActor` are corrected in place, including the one that
    ASSIGNS the work rather than only the ones that record it.
  - **THE POSITIVE CONTROL FOR THE RING: IT WAS REMOVED AND THE FRAME CHANGED IN EXACTLY ONE
    PLACE.** The objective hex is a Factory, so the ring sits on an orange tile and "a coloured hex
    among coloured hexes" would have been an inference. `playtest_console("ke BP_StratBoard_C_0
    ClearObjective")` was issued mid-PIE and the next `playtest_observe` differed from the previous
    one at that hex ALONE, which reverted to the plain Factory tile every other Factory shows.
    Nothing else in the frame moved. `ke` reaching a live PIE actor is this record's own prior
    finding, used rather than rediscovered.
  - **THE POSITIVE CONTROL FOR THE MARKER: A REAL A/B THROUGH TWO PIE SESSIONS, BECAUSE NO
    `UFUNCTION` CAN HIDE IT.** `AStratUnitActor` exposes no callable that clears the marker, so the
    remove-and-recapture trick used on the ring is unavailable. Instead: PIE stopped,
    `GuidedMarkerMesh` set to `None` and COMPILED BUT NOT SAVED, PIE restarted. The control frame
    shows the marked unit as a plain body identical to its four neighbours, and `LogStratPlay`
    carries `has no GuidedMarkerMesh set` -- absent from every run either side of it. The mesh was
    then restored, compiled, SAVED and read back, and the marker returned to the same unit in the
    same place. Present -> absent -> present, on the same instrument.
  - **AND THE MARKER IS ON THE RIGHT UNIT, MEASURED RATHER THAN EYEBALLED.**
    `GetAll StaticMeshComponent bVisible Name=GuidedMarker` returns ten lines: exactly ONE is
    `True`, `BP_StratUnit_C_2`, and `GetAll StratUnitActor UnitId` gives that actor `UnitId = 3` --
    which is the side-0 marked Infantry `strat-test-author` independently measured, with side 1's
    unit 7 marked in the same frame and correctly NOT shown. The `View.Side == ViewingSide` filter
    is therefore observed working on a screen, not only in a suite. `GetAll ... GuidedMarkerMesh`
    shows `SM_GuidedMarker` on all ten live actors, so the Blueprint default reaches the spawned
    actor and not merely the CDO.
  - **THE COLOUR WAS CHOSEN TWICE, AND THE FIRST CHOICE WAS WRONG ON EVIDENCE FROM THE SCREEN.**
    Both assets shipped first at amber `(1.00, 0.78, 0.08)`, picked to sit clear of reach-blue
    `(0.02, 0.30, 0.95)` and target-red `(0.95, 0.05, 0.03)`. The capture showed why that is not
    enough: the ring is always on a FACTORY, and `MI_Terrain_Factory` is orange, so amber-on-orange
    read as a slightly-brighter factory. Both were moved to violet `(0.80, 0.10, 0.95)` -- ring
    alpha 0.85, marker alpha 0.95 -- and the re-capture shows both unmistakably, distinct from
    reach, target, factory orange, terrain green and both side colours. **A palette that clears the
    other OVERLAYS can still collide with the TERRAIN under the one hex it will always be on.**
  - **THE SAVE SLOT WAS MEASURED CLEAN, WITH THE INSTRUMENT CONTROLLED FIRST**, because this
    record's own trap says an unarmed guided opening is indistinguishable from the defect.
    `Saved/SaveGames/` exists and `StratocracyMatch.sav` does not; positive controls in the same
    call reported `Config/DefaultEngine.ini` at 4331 bytes and `Saved/Logs/Stratocracy.log` at
    640364 bytes, so the instrument can see a file that is there. All four PIE runs logged
    `Guided opening armed for side 0: objective hex (2, 7), window turns 1-4.` and none logged
    `Guided opening suppressed`. Nothing was moved and nothing needs restoring.
  - **WHAT THIS DOES NOT REACH, and the accessor cannot help.** `IsGuidedMarkerVisible` reports a
    FLAG, not pixels, and its own block says there is no headless gate on "the marker is actually
    visible to a player" -- so nothing above is claimed off it. What was observed is BEAT 1a and
    the beat-2 ring as they stand AT MATCH START, which needs no input. Beat 2 actually retiring
    when the Infantry reaches the ringed Factory, the ring clearing in the same frame as the strip,
    and the marker following the unit as it moves are all unobserved: injected input still never
    reaches `UPlayerInput` here, so they need a human at the keyboard. No section is closed by this
    entry.
  - **[STAMPED 2026-08-24: ALL THREE OF THOSE ITEMS ARE NOW WITNESSED -- BUT ONE HALF-CLAUSE
    INSIDE THEM IS NOT, AND A NEW GAP OPENED WHERE THE PLAYTEST CLOSED THE OLD ONE.** A human
    played the shipped scenario at the keyboard after the bullet above was written, so the input
    the injector could not deliver was delivered by a player. Twice, in two separate runs, the
    engine log carries `Guided beat 2 retired on turn 3: the ringed objective (2, 7) was taken
    by side 0 (pip=false, held=true).` then `Guided beat 3 retired on turn 3: a unit spawned.`
    and `Guided opening complete on turn 3: every beat retired.` -- so BEAT 2 ACTUALLY RETIRING
    WHEN THE INFANTRY REACHES THE RINGED FACTORY IS OBSERVED, on its second arm. The player
    confirmed on the screen that THE RING CLEARED, and confirmed separately that THE MARKER
    FOLLOWED THE INFANTRY AS IT MOVED. Those three are retracted from the unobserved list.
    **WHAT IS STILL NOT OBSERVED, and it must not ride along on the item that contains it:** the
    second item above says the ring clears *in the same frame as the strip*, and nobody has
    reported watching the guidance strip disappear in that frame. Only the ring half was
    witnessed. That half-clause is not pinned headlessly either -- `strat-test-author` reports
    `FStratGuidanceView` reaches a UMG widget `StratPlay` cannot construct in a test -- so it
    has no gate on either instrument.
    **AND NOBODY HAS SEEN THE MARKER CLEAR.** This playtest is what found it: the marker latched
    on for the whole match. `AStratUnitActor::ApplyUnitView` now ANDs `Model.Guidance.bActive`
    ahead of the two match-constant operands, which is green in the suite and unobserved on a
    screen. So this stamp does NOT close the playtest gap -- it MOVES it. Reading it as "the
    guided opening is now witnessed end to end" would be wrong.
    `global.md` carries what this evidence does for the section; this file states no count and
    no verdict. THE TRAP, worth carrying forward: this same sentence was stamped in `global.md`,
    where the EVIDENCE sits, a full pass before it was stamped here, where the WORK IS ASSIGNED
    -- and left standing here it would have sent the next reader to schedule a playtest for
    observations already taken. **A retraction that reaches the file holding the evidence has
    not reached the file holding the handoff.**]**
  - **[STAMPED 2026-08-24, LATER THE SAME DAY: THE LAST TWO THINGS THE STAMP ABOVE HELD OPEN ARE
    NOW WITNESSED -- BY EYE, AND THE SUITE STILL CANNOT GATE THEM.** A human ran the guided
    opening to completion twice more and watched the moment it ends. THE MARKER WENT OUT WITH
    THE RING, and THE GUIDANCE STRIP DISAPPEARED IN THE SAME FRAME -- all three surfaces going
    dark together, not the ring alone. So `AND NOBODY HAS SEEN THE MARKER CLEAR` above is
    retired: the `Model.Guidance.bActive` AND in `AStratUnitActor::ApplyUnitView` is now
    observed working on a screen and not only green in the suite. And the `in the same frame as
    the strip` half-clause of the original bullet, which the stamp above correctly broke out as
    its own limit, is witnessed too.
    **WHICH INSTRUMENT CARRIES WHICH CLAIM, because they are not the same instrument.** The LOG
    carries only that both runs REACHED the instant `bActive` falls:
    `[2026.08.24-15.57.47:040] Guided opening complete on turn 3: every beat retired.` and
    `[2026.08.24-15.58.38:000] Guided opening complete on turn 3: every beat retired.` -- two
    separate runs, and that branch is one of the three where `bActive` falls, which is the exact
    frame the fix acts on. THE THREE SURFACES GOING DARK TOGETHER IS THE HUMAN AT THE KEYBOARD,
    and that is the only instrument that can see it -- nothing in this tree corroborates it.
    **AND NO HEADLESS GATE BECAME POSSIBLE.** `IsGuidedMarkerVisible` still reports the visible
    FLAG and not pixels, exactly as the bullet at the top of this entry says, so no clause
    covers any of this and none was added. What changed is that the only instrument that could
    ever see it was used -- not that the suite grew a way to see it. Reading this as "now
    covered by a clause" would be wrong.
    `global.md` carries what this evidence does for the section; this file states no count and
    no verdict. The original bullet and the stamp above it are both left exactly as written --
    each was correct when written, an hour ago, and neither is reworded here.]**
  - **NEW MEASURED LIMITS OF THE NeoStack Lua API, all 2026-08-23, each with a control:**
    - **`write_file` WRITES UTF-16LE WITH A BOM IN EVERY TEXT MODE, INCLUDING `encoding="utf8"`.**
      A 30-byte ASCII-plus-em-dash string landed as 58 bytes on disk; `encoding="utf8"` and
      `encoding="ansi"` gave the identical 58. It reports `bytes_written` as the SOURCE length, so
      the return value cannot see it. Round-tripping this file through `write_file` would have
      converted a 43914-byte tracked Markdown file to UTF-16 while every read still looked right.
      **The only byte-exact route is `encoding="binary"`, which takes BASE64** -- verified by
      writing the same 30-byte string and reading back 30 bytes.
    - **`read_file` SILENTLY STRIPS CR AND SILENTLY TRUNCATES AT 500 LINES.** This file is CRLF on
      disk; the returned content had zero CR, and the difference showed up only as 43403 returned
      against 43914 on disk. The default `limit` is 500 even when `total_lines` reports 512, and
      the truncated read reports the FULL total. An unguarded read-modify-write would have deleted
      the last twelve lines of this file and rewritten every line ending.
    - **BASE64-ENCODING A WHOLE FILE IN ONE SCRIPT TRIPS `Script exceeded instruction limit`.**
      44445 bytes encoded fine; 58070 aborted. Stash the text in a scratch file between calls and
      write the target in chunks with `{append=true}`, on 3-byte boundaries so the base64 pieces
      concatenate without interior padding.
    - **`read_file` RETURNS NOTHING FOR `Saved/Logs/Stratocracy.log`** -- `total_lines = 0`, empty
      content -- while the same file reads 640364 bytes through `file_info` and
      `playtest_log_contains` reads its lines fine. The editor holds it open. Read the log through
      `playtest_log_contains`, never `read_file`.
    - **`playtest_log_contains` RETURNS THE FIRST MATCH IN THE WHOLE FILE, so an asset name matches
      its own `Saving Package:` line from an hour earlier.** Searching `SM_GuidedMarker` "found" a
      save log line, not the `GetAll` output. Anchor on a string only the answer can produce --
      `BP_StratUnit_C_2.GuidedMarkerMesh` -- rather than on the value being looked for.
    - **EACH `execute_script` CALL IS A FRESH LUA STATE: `_G` DOES NOT PERSIST BETWEEN CALLS.** A
      `playtest_log_marker()` stashed in `_G` came back `nil` in the next call, and
      `playtest_log_contains(t, {since=nil})` does NOT error -- it silently searches the whole log.
      A scoped assertion split across two calls is an unscoped one that looks scoped. Take the
      marker and the assertions in ONE script.
    - **`list_files` CANNOT SEE `Saved/`.** `Saved/Logs` and `Saved/SaveGames` both return 0 items
      with `pattern="*"`, while `Config` returns 6 and `Source/StratPlay` returns 23 in the same
      call, and a genuinely absent directory errors with `directory not found`. So 0 items from
      `list_files` does NOT mean empty. **And `list_files` resolves a relative path against
      `project_dir()` while `file_info` resolves it against the ENGINE's `Binaries/Win64`** -- the
      same relative string names two different files. Pass `file_info` an absolute path.
    - **`geometry_create()` IS ABSENT** despite `help('LevelDesign')` naming it, so there is no
      procedural static-mesh route; `duplicate_asset` from `/Engine/BasicShapes` is the way.
      `screenshot({focus_actor=...})` cannot see a PIE actor either -- `Actor not found` for a name
      `GetAll` had just printed -- because it looks in the editor world.
  - **MORE MEASURED LIMITS OF THE SAME API, and these are 2026-08-24, NOT the 2026-08-23 batch
    above -- a separate day, recorded here because this is where the next lane will look.**
    - **`read_file` AT `encoding="binary"` SILENTLY IGNORES `offset` AND `length`, and returns
      the WHOLE file.** Measured by asking for `{encoding="binary", offset=0, length=12}` on
      this file and getting the entire 79172-character base64 back, which blew the tool-result
      token cap instead of returning 12 bytes. **This does NOT contradict the 500-line
      truncation recorded above: that is a TEXT-MODE behaviour, and binary is all-or-nothing.**
      The two are easy to confuse, and together they decide how a lane slices a large file --
      text mode hands you a truncated read that LOOKS complete, binary hands you everything or
      nothing. The working route is to take the whole base64 once and slice it inside Lua with
      `string.sub` on 4-character boundaries, which are 3-byte boundaries.
    - **`help("file")` CLOSES THE SOCKET**, returning `The socket connection was closed
      unexpectedly`, presumably on output size. So `help()` is NOT a safe probe for that
      domain -- which matters because it is the obvious first move for a lane trying to
      discover this API, and it costs a dead connection rather than an error message. The file
      functions were found by trial instead: `file_info`, `read_file` and `write_file`
      exercised directly against a scratch path, and `_G` scanned for a base64 helper, which
      found NONE -- the encoder and decoder used in this pass are hand-rolled in Lua.
  - **FOR OTHER LANES, neither blocking.** (1) `StratBoardActor.h` and `StratUnitActor.h` each name
    `BP_StratBoardActor` / `BP_StratUnitActor` in the comment on the very property the content lane
    fills; those assets do not exist and the names are `BP_StratBoard` / `BP_StratUnit`. It is a
    comment, so nothing is broken, but it is the sentence a future builder would follow. OWNED:
    `strat-gameplay-engineer`; the content lane cannot edit `Source/`. (2) The marker sits directly
    above the body and at this camera pitch it OCCLUDES the unit it names rather than pointing at
    it. `GuidedMarkerZOffset` is a C++ default of 150.0 and was left alone; moving the marker
    beside the unit is a C++ default change or a new offset property, not a content edit.

- **WRITTEN BY THE COORDINATOR UNDER THE HEADER'S FALLBACK CONDITION. Fifth occurrence, and the
  SECOND one today (2026-08-23) -- the entry immediately below is the fourth.** `execute_script` was
  again absent from the session's tool surface: a `select:` lookup for
  `mcp__unreal-editor-direct__execute_script` and a bare keyword lookup for `execute_script` both
  answered `No matching deferred tools found`. **Control, per obligation (1):** the same lookup in
  the same round served `mcp__NeoStack_Connect__unreal_status` and
  `mcp__NeoStack_Connect__list_unreal_projects`, both of which loaded and answered. The instrument
  speaks; the tool is not there.
  **[SUPERSEDED 2026-08-23, LATER THE SAME DAY, BY THE ENTRY AT THE TOP OF THIS SECTION -- the
  first one `strat-editor-builder` has written here since the condition began. WHAT IS SUPERSEDED
  IS THE IMPLIED STATE OF THE LANE, "there is no route to the editor", AND NOTHING ELSE: every
  measurement below was correct when made and stands. AND THE DIAGNOSIS BELOW IS NOW CONFIRMED
  RATHER THAN MERELY RETIRED, WHICH IS WORTH MORE THAN THE ENTRY IT RETIRES. It predicts that
  starting the session with the editor already running is the whole fix. That is exactly what was
  done and exactly what happened: the editor was up before this session began, PID 81704 on port
  9315, `mcp__unreal-editor-direct__execute_script` was on the tool surface, `project_dir()`
  round-tripped `E:/MultiAgent/Stratocracy/` on the first call, and the lane created assets,
  compiled, saved and ran four PIE sessions without touching `NeoStack_Connect` once. THE
  FALLBACK CONDITION WAS NOT MET THIS SESSION and the coordinator did not write this file.]**
  - **THE CAUSE IS SESSION-STARTUP ORDERING, NOT THE PROXY LATCH, AND IT NEEDS NO DECISION FROM
    ANYONE.** `unreal-editor-direct` is registered correctly, and has been all along -- globally in
    `~/.claude.json`, type `http`, url `http://127.0.0.1:9315/mcp`, which is the exact endpoint the
    editor serves. It was missing from this session because **the editor was closed when the session
    started**: at the session's first probe the editor PID then recorded in `runtimes.json` was gone,
    port 9315 refused TCP, and no `UnrealEditor` process existed at all. The MCP client dialled a
    dead port at launch, that server failed, and a failed server is not retried mid-session. Opening
    the editor afterwards repairs the endpoint but cannot repopulate a tool surface that was fixed at
    launch. **The fix is to start the session with the editor already running**, and it is the whole
    fix.
  - **THE EDITOR ITSELF SERVES THE TOOL -- measured directly over HTTP, bypassing both proxies.**
    With the editor reopened (PID 81704; port 9315 listening and owned by that PID; process confirmed
    present), a JSON-RPC `initialize` against `http://127.0.0.1:9315/mcp` returned 200 with
    `serverInfo.name` = `unreal-editor`, version `1.0.0-r4254`, and `tools/list` returned exactly one
    tool -- `execute_script` -- carrying the full Lua surface, `CreateAsset`, `WidgetBlueprint`,
    `Playtest` and `Screenshot` among its domains. The lane's route exists and is healthy. Only this
    session cannot see it.
  - **THE LATCH IS REAL AND WAS REPRODUCED IN THE SAME ROUND, BUT IT IS NOT THIS.**
    `list_unreal_projects` answered `Active editors: - Stratocracy: E:/MultiAgent/Stratocracy/` while
    `unreal_status`, reading the same file in the same round, reported that no active editors were
    found. That is the recorded disagreement inside `NeoStack_Connect`, and it reproduces. But
    `execute_script` is served by a DIFFERENT server, `unreal-editor-direct`, which was never latched
    -- it was simply not up. The two faults were conflated because they present identically to the
    lane: no route to the editor. Only today's condition was measured; the four earlier occurrences
    are not re-litigated here.
  - **NOTHING WAS BUILT, PROBED IN-EDITOR, OR CLAIMED.** No Lua executed, no asset created, no
    compile, no save, no PIE, no screenshot. `strat-editor-builder` was not dispatched at all: without
    `execute_script`, and holding no Bash, it has no route and could only have stopped and reported.
    The HTTP probes above were the coordinator's own and read the server's tool list -- they touched
    no asset.
  - **THE SAVE SLOT IS CLEAN.** `Saved/SaveGames/` is empty, so there is no `StratocracyMatch.sav` to
    clear ahead of the next playtest. The only copy on the box is a backup inside an unrelated
    session's scratchpad, which the game never reads.
  - **WHAT IS STILL OWED IS UNCHANGED BY THIS PASS:** the ring and marker meshes and material
    instances, and the Blueprint defaults filling `ObjectiveMaterial`, `GuidedMarkerMesh` and
    `GuidedMarkerMaterial`, exactly as the entry below records them.
    **[CORRECTED AND DISCHARGED 2026-08-23 by `strat-editor-builder`, which built them. TWO
    ERRORS, BOTH INHERITED FROM THE ENTRY THIS ONE POINTS AT, AND BOTH CORRECTED THERE TOO.
    (1) THE RING NEEDS NO MESH. `StratBoardActor.h` measures it: `ObjectiveOverlay` is a
    `UHierarchicalInstancedStaticMeshComponent` instancing the SHARED `OverlayMesh` -- "One mesh,
    three components, three materials" -- and `OverlayMesh` was already `SM_HexOverlay`. The ring
    is a MATERIAL ONLY. Only the marker needed a new mesh, so "the ring and marker meshes" names
    one artifact that never existed. (2) THE BLUEPRINTS ARE `BP_StratBoard` AND `BP_StratUnit`,
    not `BP_StratBoardActor` / `BP_StratUnitActor`; `find_assets("/Game/StratPlay")` returns six
    assets and neither of those names is among them. All three defaults are now set, and the
    ring and the marker have both been seen in PIE -- see the entry at the top of this section.]**


- **WRITTEN BY THE COORDINATOR UNDER THE HEADER'S FALLBACK CONDITION, WHICH IS MET FOR THE FOURTH
  CONSECUTIVE DAY (2026-08-21, -22, -23, and this pass).** `execute_script` was again absent from
  `strat-editor-builder`'s tool surface; its NeoStack surface was exactly `unreal_status` and
  `list_unreal_projects`. **Measured with a control, per obligation (1), and the control is
  unusually good this time because the two tools CONTRADICT EACH OTHER:**
  `list_unreal_projects` answered `Active editors: - Stratocracy: E:/MultiAgent/Stratocracy/` --
  so the instrument speaks, and it confirms the live editor (PID 77172) is on the real tree and
  not a worktree -- while `unreal_status` in the same round reported that no active editors were
  found in `runtimes.json`. That is the recorded proxy latch, not a closed editor, and the
  disagreement is INSIDE the proxy between two of its own tools.
  - **[SUPERSEDED 2026-08-23, LATER THE SAME DAY, BY THE ENTRY AT THE TOP OF THIS SECTION. The
    dilemma below -- "either the proxy gets fixed or the lane's route gets redefined" -- rests on
    the premise that the latch is why `execute_script` is absent. That premise is FALSE. The tool
    comes from `unreal-editor-direct`, a different server, which is registered correctly and was
    merely not up at session start. Neither branch of the dilemma is needed; starting the session
    with the editor already running is the whole fix. Stamped rather than deleted because the
    measurement was correct when made.]**
  - **THE STANDING-CONDITION QUESTION IS NOW OVERDUE AND IS THE USER'S TO SETTLE.** The header was
    amended on 2026-08-23 after three consecutive days; this is the fourth, and a fallback invoked
    every single time a lane runs is not a fallback. Either the proxy gets fixed or the lane's
    route to the editor gets redefined. Recorded here rather than silently invoked a fourth time.

- **THE MARKER AND THE RING CANNOT BE BUILT IN THE CONTENT LANE AT ALL, AND RESTORING EDITOR ACCESS
  WOULD NOT CHANGE THAT. Measured 2026-08-23 by `strat-editor-builder`, re-verified independently
  by the coordinator before recording.**
  This is the more important half of this pass and it is independent of the latch above.
  **[SUPERSEDED 2026-08-23, LATER THE SAME DAY, BY THE COMMIT THIS ENTRY IS COMMITTED IN. The two
  C++ seams this bullet says do not exist WERE BUILT by `strat-gameplay-engineer` in the same
  change: `AStratBoardActor::ObjectiveOverlay` is a third constructor subobject with
  `ShowObjective` / `ClearObjective` / `GetObjectiveOverlayCount`, and `AStratUnitActor::GuidedMarker`
  with `IsGuidedMarkerVisible`. So "there is no surface to draw on" and "no marked-state seam of
  any kind" are both FALSE as of this commit. Stamped rather than deleted because the measurement
  was correct when made and is the reason the seams exist. WHAT SURVIVES UNCHANGED: the content
  lane still cannot draw anything, because the THREE `EditDefaultsOnly` ASSET REFERENCES ship UNSET
  — `AStratBoardActor::ObjectiveMaterial`, `AStratUnitActor::GuidedMarkerMesh` and
  `GuidedMarkerMaterial`, all null `TObjectPtr`s. (The fourth new `EditDefaultsOnly` property,
  `GuidedMarkerZOffset`, is a numeric carrying a real default and is NOT part of this claim; an
  earlier draft of this bullet said "all four ship UNSET", which named a different set than was
  measured.) The
  marker mesh, the material instances and the Blueprint defaults on `BP_StratBoard` /
  `BP_StratUnit` are still owed, and nobody has seen a ring or a marker on a screen. The
  blocker moved from "no seam" to "no assets"; it did not lift.]**
  **[TWO CORRECTIONS AND ONE DISCHARGE, 2026-08-23, by `strat-editor-builder`. The sentence above
  used to read `meshes, the material instances and the Blueprint defaults on
  BP_StratBoardActor / BP_StratUnitActor`. BOTH HALVES WERE WRONG. There is only ONE new mesh,
  the marker's: the ring instances the shared `OverlayMesh`, per `StratBoardActor.h`'s "One mesh,
  three components, three materials", so it is a material only. And the Blueprints on disk are
  `BP_StratBoard` and `BP_StratUnit` -- the `...Actor` names appear nowhere in `/Game/StratPlay`.
  AND THE BLOCKER HAS NOW LIFTED: the three defaults are set, compiled and saved, and both the
  ring and the marker were observed in PIE with a positive control each.]**
  **[NESTING REPAIRED 2026-08-24, by `strat-editor-builder`, on the user's report. Nothing above
  was deleted and no measurement was withdrawn. The stamp before this one was written INSIDE the
  SUPERSEDED stamp's brackets, which left that stamp's `]**` stranded alone on its own line and
  split the bullet's own closing sentence -- "This is the more important half of this pass and it
  is independent of the latch above." -- off from the headline it belongs to. Both stamps now
  close in place and read as siblings, and that sentence has been moved back to the top of this
  bullet, where its "this" has a subject again. THE TRAP, worth carrying forward: stamping a
  correction INSIDE an already-bracketed block leaves one dangling closer, `strat_banner_sweep.py`
  does not catch it, and the four `**` toggles render the whole block wrong. Stamp a correction as
  a SIBLING block after the one it corrects and let it say "the stamp above"; never nest one.]**
  - **THE STRUCTS ARE REFLECTED; NOTHING HANDS A BLUEPRINT AN INSTANCE.** `FStratUnitView::
    bIsGuidedMarked`, `FStratGuidanceView::bHasObjectiveRing`, `ObjectiveHex` and `bActive` are all
    `UPROPERTY(BlueprintReadOnly)` in `StratViewModel.h`. But `UStratMatchSubsystem` reflects 21
    `UFUNCTION`s and NOT ONE returns the view model, the units array or the guidance view. The two
    carrying `Category = "Stratocracy|Guidance"` are `HasCompletedAMatchOnSave` and
    `RecordMatchCompletionOnSave` -- the save-slot completion reader and writer, nothing to do with
    the guidance view. The accessor is deliberately unreflected and the header says why: UHT does
    not accept a reference return. **Verified by reading both functions, not by counting hits.**
  - **AND THERE IS NO SURFACE TO DRAW ON.** `AStratBoardActor` declares exactly TWO overlay
    components, `ReachOverlay` and `TargetOverlay`, with `ShowReach`/`ClearReach`/`ShowTargets`/
    `ClearTargets` and one shared `OverlayMesh`. There is no third overlay for a guidance ring, and
    reusing `TargetOverlay` would hijack §2.6's attack-target overlay -- which is not merely untidy
    but clause-visible, since `T-UI-02` asserts `ShowTargets` filled exactly one component and that
    it is not the reach one. `AStratUnitActor` exposes exactly ONE `UFUNCTION` (`GetUnitId`), has
    no marker component beside `Body`, and holds `LastAppliedView` as a bare
    `UPROPERTY(Transient)` with no Blueprint visibility. There is no marked-state seam of any kind.
  - **SO `StratGuidedOpening.h`'S DIVISION OF LABOUR IS HALF-SPECIFIED, and that is the finding.**
    It assigns "the ring mesh and the turn-1a marker" to this lane, which is right for the MESH and
    the MATERIAL -- authorable here -- but the plumbing that decides WHERE they appear was never
    built. The directive strip works because `UStratGuidanceWidget::Guidance` is PUSHED from C++;
    the ring and the marker have no equivalent push. **A lane cannot draw a field it cannot reach.**
  - **NOTHING WAS BUILT, PROBED OR CLAIMED.** No `create_asset`, no `set`, no `compile`, no `save`,
    no Lua executed, no PIE, no screenshot. The builder stopped rather than improvising, which is
    the correct call and is recorded as such.
  - **WHAT THE CONTENT LANE WILL OWN once the seams exist:** the ring and marker meshes and
    material instances under `Content/`, and the Blueprint defaults assigning them on
    `BP_StratBoardActor` and `BP_StratUnitActor`. Blocked until then.
    **[CORRECTED AND DONE 2026-08-23 by `strat-editor-builder`. This is the sentence that ASSIGNS
    the work, so the correction has to reach it and not only the evidence above. There is ONE new
    mesh, not two -- the ring instances the shared `OverlayMesh` and is a material only -- and the
    Blueprints are `BP_StratBoard` and `BP_StratUnit`. Built and saved:
    `MI_Overlay_Objective`, `MI_Marker_Guided`, `SM_GuidedMarker`, and the three defaults.]** The two seams proposed to
    `strat-gameplay-engineer` are recorded in `global.md`'s `## NEXT`, not restated here.
  Measured with a control -- the same keyword lookup returned `unreal_status` and
  `list_unreal_projects`, so the instrument could speak; independently the editor's own endpoint
  served `serverInfo: unreal-editor 1.0.0-r4254` and round-tripped Lua against the live pid, and
  `project_dir()` answered `E:/MultiAgent/Stratocracy/` rather than a worktree. Driven over that
  endpoint with `Tools/neostack/ns.py`.

- **T-UI-04'S PRODUCTION PATH WORKS END TO END, HUMAN-DRIVEN AND OBSERVED. THE DEFECT THAT WAS
  REPORTED AGAINST IT IS NOT IN THE PRODUCTION PATH.** 2026-08-23, four PIE sessions, the user at
  the keyboard. The reported symptom was "a unit never seems to actually build or spawn after
  clicking build". It is explained, and the explanation is not a bug in the menu.
  - **WHAT WAS OBSERVED, AND BY THREE INDEPENDENT INSTRUMENTS.** With the factory hex FREE, the
    user watched the unit spawn ON the factory hex. The rules logged
    `STRAT-CMD accepted: Build definition index 3 at factory (-1, 4)` at 15:09:54, and in the same
    second the guidance layer independently logged `Guided beat 3 retired on turn 1: a unit
    spawned.` -- a corroboration from a system that is not the production code and was not asked.
  - **WHY IT LOOKED BROKEN IN THE EARLIER SESSIONS, AND IT WAS THE MAP RATHER THAN THE MENU.**
    Building is TWO steps and only the first is synchronous with the click: `Economy.h::queueBuild`
    charges the ~2.4 cost immediately and queues, and `resolveBuilds` places -- "the factory hex if
    free, else the canonically smallest free neighbour, else it keeps waiting and keeps the slot".
    In both earlier sessions the user first moved a unit ONTO the factory hex and then built there,
    which forces the spawn to a neighbour. Side 0's factory is unusually hemmed in: computed with
    the rules' own odd-r conversion and direction vectors, **five of its six neighbours hold side
    0's own starting units** and only E `(0, 4)` is free. So the unit either landed inside a
    five-unit friendly cluster or was boxed in and waited.
  - **THE REAL GAP, AND IT IS NARROWER THAN THE FIRST READING.** The refusal paths DO report:
    `Production menu: the cursor is not on the board` and
    `STRAT-CMD refused: factory is not held by this side` both reached `LogStratPlay`. The one path
    that is genuinely silent is an ACCEPTED build whose unit is displaced to a neighbour or boxed
    in -- the rules `continue` without a word, so Fame is spent and the slot held with nothing said
    and nothing shown. `FStratFactoryView::bSpawnBlocked` already carries the rules'
    declared-derived answer into the view model and no widget reads it.
    **WHAT IS NOT MEASURED:** whether any of the reporting lines above reach the SCREEN or only the
    log. Nothing here looked, and the log is not the player.
  - **THE COVERAGE GAP THAT LET IT THROUGH.** `GATE-BUILDMENU`'s reconcile clause asserts exact
    actor/model set equality, which is the right shape, but it selects "a row this side can pay for
    AND place" -- so it steps around the displaced and boxed-in cases by construction. No clause
    covers either. OWNED: `strat-test-author`, and the widget half by `strat-editor-builder`.

- **THE GUIDED OPENING ARMS ONCE THE SAVE IS OUT OF THE WAY, AND THIS RECORD'S OWN WARNING WAS
  WHAT FOUND IT.** The user's first run logged `Guided opening suppressed for side 0: this save has
  a completed match`, exactly as the entry below predicts. `Saved/SaveGames/StratocracyMatch.sav`
  was MOVED, not deleted, and the next run logged
  `Guided opening armed for side 0: objective hex (2, 7), window turns 1-4.` at 15:09:49, in the
  same frame as `Match live` and `Match started by BP_StratGameMode_C_0`.
  **THE CONTROL:** the suppression line is present in all three earlier sessions and absent from
  that one. `global.md` carries what this does and does not do for the section; this file does not
  state it.

- **CORRECTION, WRITTEN BY THE COORDINATOR ON THE USER'S INSTRUCTION: THE SIX BINDING FUNCTIONS
  ARE PURE, NOT IMPURE, AND THE FIX IS UNAFFECTED.** 2026-08-23 afternoon, measured on a live
  editor. The entry below states that all six were "authored IMPURE" and cites visible exec pins
  on entry and result (`is_hidden=false` under `read_graph(..., {include_hidden=true})`). **That
  inference is wrong, and `global.md` had already withdrawn it on 2026-08-22** -- its
  "A LEAD WAS RAISED HERE AND IS WITHDRAWN IN THE SAME PASS" bullet reasoned to the correct answer
  a day before the entry below re-asserted the error. The measurement now closes it.
  - **THE CONTROL THAT SETTLES IT.** On a throwaway `/Game/UI/ZZ_PureProbe`, created and deleted
    inside the experiment, two functions identical but for the `pure` flag were bound to the same
    widget property by the same `add_binding` call. `pure=false` FAILS to compile, verbatim:
    *"Binding: property ' /Script/UMG.TextBlock:TextDelegate ' on widget ' ProbeLine ' needs to be
    bound to a pure function, ' Get Probe Impure ' is not pure."* `pure=true` compiles clean. Since
    both shipped widgets compile clean today with all six bindings registered, the six are pure.
    The same differential is what proves `add_function(..., pure=true)` actually sets the flag.
  - **WHY THE EVIDENCE NEVER DISCRIMINATED.** A `pure=true` function shows the SAME visible exec
    pins on entry and result, before and after compile -- read side by side on the probe. Neither
    `read_graph` nor `class_methods` exposes purity at all. Package bytes do not either:
    `bIsPureFunc` belongs to `UK2Node_CallFunction`, not the entry node, and the entry's
    `ExtraFlags` is present in known-impure graphs too (`WBP_ProductionMenu`'s `PaintRows`), so it
    discriminates nothing. The only instrument that answered was the compiler's own refusal.
  - **AND PURITY DOES NOT EXEMPT A BODY FROM NEEDING AN EXEC WIRE, which is why the fix was right.**
    On the same probe, a PURE function compiling clean with `Make Literal Text("PURE7741")`
    data-wired to `Return Value` and no exec wire returned `[]`; wiring
    `entry.then -> result.execute` and recompiling returned `[PURE7741]`. Instrument control:
    `Conv_StringToText("MARSHAL3312")` marshalled fine, so the empty was real. `FUNC_BlueprintPure`
    governs how CALL SITES render; the body stays a linear script driven by the entry's exec chain.
    **Every measurement in the entry below other than the word IMPURE stands, and so do the six
    wires.**
  - **NO SHIPPED BYTE MOVED FOR THIS.** `WBP_DirectiveStrip.uasset` and `WBP_OneShotTip.uasset`
    re-read `5bd3eb81c264ae49d8677bcca4f5b36a031b078e` and
    `2d1faa3a2555d590f23fd34f2b38ab8d2d410446`, identical either side of the probe; PIE was
    confirmed stopped before any asset was opened; the scratch asset was deleted and `/Game/UI` is
    back to its six assets.
  - **A TOOL QUIRK THAT CAN FAKE AN ABSENCE, found while looking for a purity accessor.**
    `class_methods(cls, {include_inherited=false})` returns **0** on `WBP_DirectiveStrip_C`, which
    owns four methods; `include_inherited=true` returns 167 including those four. A reader taking
    the first answer would conclude the class has no methods of its own.

- **WRITTEN BY THE COORDINATOR, NOT BY `strat-editor-builder` -- THE SECOND RECORDED DEVIATION OF
  EXACTLY THE KIND THE ENTRY BELOW RECORDS, FOR THE SAME MEASURED REASON.** 2026-08-23.
  `execute_script` was again absent from the session's tool surface -- `NeoStack_Connect` served
  `unreal_status` and `list_unreal_projects` only -- so `strat-editor-builder`, which has the
  NeoStack tools plus Read/Grep/Glob/Skill and no Bash, again had no route to the editor. Driven
  over the editor's own MCP endpoint with `Tools/neostack/ns.py`. The header above still names
  `strat-editor-builder` as sole writer and that is unchanged as policy; this is the second
  exception, not a standing arrangement. **[AMENDED 2026-08-23 AFTER THE RE-GATE, WHICH
  JUDGED THIS SENTENCE STRAINED BY ITS OWN EVIDENCE AND WAS RIGHT.** Two occurrences with the SAME
  measured cause on consecutive days is a STANDING ENVIRONMENTAL CONDITION, not a run of exceptions,
  and calling it the latter is how a rule quietly stops describing practice. The gate did not gate
  on it -- nothing here is silent, and the writer is the agent that did the work rather than a peer
  lane claiming another's. What it ruled is that the header line above is DRIFTING FROM PRACTICE and
  that the drift wants resolving deliberately: either the tool surface is fixed so
  `strat-editor-builder` can reach the editor at all, or this header is amended to name the fallback
  writer and the condition that licenses it. OWNED: coordinator, since no crew lane owns another
  lane's header. NOT DONE HERE, because changing a lane's sole-writer rule is a crew policy decision
  and not a coordinator's to take alone under a deadline.]**
  **[RESOLVED 2026-08-23 afternoon: THE USER TOOK THE DECISION AND CHOSE TO AMEND THE HEADER.** It
  now names `coordinator` as fallback writer and states the single condition that licenses it, plus
  the two obligations that ride with it. `execute_script` was absent a THIRD consecutive day when
  the decision was taken, which is what settled it. The header above is the authority; this bullet
  is history. What was NOT chosen is the other branch -- nobody has fixed the tool surface, the
  latch's trigger is still unidentified, and restarting Claude Code remains measured NOT to clear
  it, so `strat-editor-builder` still cannot reach a live editor in a latched session.]**
  **A gate should evaluate the deviation rather than
  rediscover it.**
  - **DATES, BECAUSE THIS SESSION STRADDLES LOCAL MIDNIGHT.** The measurements below were taken
    2026-08-22 23:26 through 2026-08-23 00:11 LOCAL, which is 03:26 through 04:11 UTC on
    2026-08-23. Every log stamp quoted below is UTC and therefore reads `2026.08.23-03.xx` for
    work done on the evening of the 22nd. Both are right; do not "correct" either to match the
    other.

- **ALL SIX OF THIS PROJECT'S UMG PROPERTY-BINDING FUNCTIONS WERE RETURNING THEIR TYPE'S DEFAULT,
  AND THE CAUSE IS ONE MISSING EXEC WIRE EACH. FIXED IN BOTH ASSETS THAT HAVE ANY.** This is the
  answer to the question the strip investigation had narrowed to -- what the binding function
  RETURNS, as opposed to whether it runs -- and it is the whole of it. `global.md` carries what it
  means for the milestone; this entry carries the assets and the measurements.
  - **THE SUBJECT SET IS DERIVED, NOT TYPED.** A census over all five `/Game/UI` Widget Blueprints
    found SEVEN function graphs. Six are property bindings and every one had zero exec connection
    from its `K2Node_FunctionEntry` to its `K2Node_FunctionResult`:
    `WBP_DirectiveStrip`'s `GetDirectiveText`, `GetWindowEndTagText`, `GetStripVisibility`,
    `GetTagVisibility`, and `WBP_OneShotTip`'s `GetTipText`, `GetTipVisibility`. The seventh,
    `WBP_ProductionMenu`'s `PaintRows`, is exec-chained normally and does NOT carry the defect.
    `WBP_Scoreboard` and `WBP_PreMatchBriefing` have no function graphs at all -- the scoreboard
    drives its TextBlocks imperatively from `EventGraph`, which is why its live text was always
    real while the strip's was not.
  - **WHAT THE DEFECT IS.** Every one of the six had NO EXEC WIRE from its
    `K2Node_FunctionEntry` to its `K2Node_FunctionResult`.
    **[CORRECTED 2026-08-23 afternoon, coordinator, by measurement -- see the CORRECTION bullet at
    the top of this file. This sentence formerly read "Every one of the six was authored IMPURE --
    entry and result carry visible exec pins, which `read_graph(..., {include_hidden=true})`
    reports with `is_hidden=false`." THE SIX ARE PURE, not impure, and the pin dump was never
    evidence either way. The missing exec wire -- the part that matters, and the part the fix
    addressed -- is unchanged.]** The data wire from the value source to `Return Value` was
    correct in all
    six and had been verified as "connected" by three prior sessions. It connects to a return node
    that is never reached, so the function returns its return-node default: an empty `FText`, or
    `ESlateVisibility`'s zero value, which is `Visible`. **That is why the strip painted a visible
    empty box rather than collapsing** -- both halves of what was seen come from the same default.
  - **HOW THE RETURN VALUE WAS READ AT ALL, which is the instrument this investigation lacked.**
    `invoke("/Game/UI/WBP_DirectiveStrip.WBP_DirectiveStrip_C", "GetDirectiveText")` answers from
    the CDO. Elsewhere in this record answering-from-the-CDO is recorded as a TRAP; here it is the
    asset, because `UStratGuidanceWidget::Guidance` can be planted on the CDO first and the
    function then read directly. `ke` had only ever confirmed execution and printed no return
    value, and that gap is what kept the section open.
  - **THE TWO CONTROLS THAT MAKE "EVERYTHING CAME BACK DEFAULT" MEAN SOMETHING.** (1) THE
    DISCRIMINATOR: planted `bActive=False`, for which a working body returns `Collapsed` from
    `To Visibility (Boolean)` while a dead body returns the zero default `Visible`. Both
    visibility functions returned `Visible`. (2) THE INSTRUMENT:
    `invoke("/Script/Engine.KismetTextLibrary", "Conv_StringToText", {"HELLO9999"})` returned
    `HELLO9999` and `Conv_BoolToText(false)` returned `false`, so the reader marshals non-empty
    `FText` and the empties are real rather than a marshalling failure.
  - **CAUSATION IS MEASURED, NOT INFERRED, AND IT WAS STAGED TO KEEP A NEGATIVE CONTROL LIVE.**
    In `WBP_DirectiveStrip` the first wire went onto `GetStripVisibility` ALONE: it flipped
    `Visible` -> `Collapsed` in the same call in which `GetDirectiveText`, still unwired, returned
    empty, with `Guidance` read back identical either side. The remaining three were then wired and
    all four moved: texts returned the planted strings, and the visibility pair moved in BOTH
    directions on the model value. A vary-text control -- a second plant returning
    `SECOND5150DIFFERENT` / `TAG2ND9024` -- shows the text functions track their input rather than
    returning one cached answer. `WBP_OneShotTip` was done the same way, visibility wire first,
    with `GetTipText` as the live negative control.
  - **THE PIXEL MEASUREMENT, AGAINST THIS RECORD'S OWN BASELINE.** PIE from the game's own startup
    path on `/Game/StratMaps/Lvl_FerrumCrossing`, log `Game class is 'BP_StratGameMode_C'` then
    `Guided opening armed for side 0: objective hex (2, 7), window turns 1-4`, captured with
    `Shot showui`. The live strip instance carried the populated projection at capture time --
    `bActive=True, Beat=Beat1a`, the same 76-character directive, `bHasObjectiveRing=True,
    ObjectiveHex=(X=2,Y=7), bEndTurnGated=True`. In a 2544x1320 frame the strip rectangle is now
    **x 434-1121, y 157-194, w=688, h=38, 22298 border pixels, and 3017 bright glyph pixels
    spanning x 456-1099**. The recorded baseline was x 750-805, w=56, h=38, 2128 pixels and ZERO
    glyph pixels. Same height, same centre 777.5; the box grew symmetrically around its centre and
    688 is the old 56 px of border padding plus 632 px of content that was never there.
  - **THE MEASURING SCRIPT WAS CONTROLLED AGAINST THE BASELINE IMAGE BEFORE IT WAS TRUSTED ON THE
    NEW ONE.** Run against the retained `ScreenShot00059.png` it returns
    `x 750-805, y 157-194, w=56, h=38, 2128 px, centre 777.5` -- digit for digit what prior
    sessions recorded by their own method. Captures live under `Saved/Screenshots/WindowsEditor/`,
    which is not tracked, so quote the geometry rather than relying on those files.
  - **WHAT THIS DOES NOT REACH, stated because the entries below it were each corrected for
    reaching too far.** Only BEAT 1a was observed -- the directive present at match start, which
    needs no input. Beats 1b and 2, the window-end tag and the gated end-turn hovers all require
    actual play, and injected input still never reaches `UPlayerInput` here. The 3017 bright pixels
    at the right extent are a SHAPE claim; nothing read the painted glyphs back as text. And no
    section is closed by this entry -- see `global.md`, which is where any such call belongs.

- **`WBP_OneShotTip` IS FIXED BLIND, AND THE FEATURE IT SERVES DOES NOT EXIST.** Its two bindings
  carried the identical defect and the identical differential was run on them -- `GetTipText`
  empty -> `TIPPROBE6620`, `GetTipVisibility` `Visible` -> `Collapsed` on `TipIsShowing=False` and
  back to `Visible` on True. But `StratGuidanceWidget.h`'s NO ONE-SHOT TIP LAYER block records that
  `CurrentTip` / `TipIsShowing` / `TipId` are Blueprint-only names with no counterpart in `Source/`,
  and `StratGuidedOpening.h`'s NO ONE-SHOT TIPS block records that they are deliberately unbuilt.
  So nothing writes those variables at runtime, nothing instantiates the widget from C++, and it
  was never taken into PIE. **This is a latent defect removed from an asset that is not yet wired
  to anything**, which would otherwise have cost teacher 3 what it cost the strip.
  - **THE CDO RESTORE ON THIS ONE IS A REPRESENTATIONAL MATCH, NOT A PROVED BYTE MATCH, AND THAT
    IS A REAL LIMIT RATHER THAN A CAVEAT.** `WBP_DirectiveStrip` had no such exposure --
    `UStratGuidanceWidget::Guidance` is `Transient`, so nothing planted there can reach disk. But
    `WBP_OneShotTip`'s `CurrentTip` / `TipIsShowing` are ordinary Blueprint variables and planting
    a probe wrote the CDO. They were restored and read back as they originally read -- `CurrentTip`
    empty, `TipIsShowing` `False`, `TipId` `None` untouched -- but an explicitly-set empty `FText`
    and a default-constructed one could serialise differently and no available instrument tells
    them apart. **The saved bytes may carry more than the two wires. A re-gate should look.**
    **[SETTLED 2026-08-23, AFTER a re-gate that could not settle it either and said so plainly.**
    Measured on a THROWAWAY widget so that no shipped asset was perturbed to answer a question about
    a shipped asset: `/Game/UI/ZZ_FTextProbe`, one Text and one bool variable, created and deleted
    inside the experiment. (a) DETERMINISM CONTROL -- saving an unchanged package twice is
    byte-identical, so any later difference is signal rather than noise. (b) An untouched,
    default-constructed Text variable and one explicitly set to empty by the SAME
    `set("self", <name>, "")` route used on `CurrentTip` produce BYTE-IDENTICAL packages.
    (c) FALSIFIABILITY -- planting `TIPPROBE6620` and `true` DOES move the hash, so the instrument
    can see a change when there is one. (d) ROUND TRIP -- planting and then restoring returns the
    package to the untouched hash exactly. The plant-and-restore this bullet worried about is
    therefore BYTE-LOSSLESS and the exposure is closed. **WHAT IT DOES NOT SHOW:** it was measured
    on a scratch asset, not on `WBP_OneShotTip` itself, which admits no A/B because its wires moved
    in the same save. It establishes the general serialisation property the gate named; it is not a
    diff of that file. The gate's own half stands alongside it -- no probe VALUE reached disk in
    either package, shown twice by independent instruments, with the only added name being the
    generated `CallFunc_Conv_BoolToSlateVisibility_ReturnValue`, itself corroboration that the node
    only now sits on a reachable exec path.]**

- **NEW MEASURED LIMITS AND USES OF THE NeoStack Lua API**, all 2026-08-23, added to the list below
  rather than replacing it:
  - **`set("self", <struct property>, "(...)")` MERGES; it does not replace.** Fields omitted from
    the ImportText string KEEP their previous values. Measured while restoring `Guidance`: writing
    back only the fields that had been changed left both `FText` members still holding the probe
    strings, and the restore only took when every one of the ten fields was named explicitly. A
    restore that reads "clean" after a partial write is not one.
  - **A WRONG MAP PATH LATCHES THE EDITOR'S PLAY STATE AND ONLY A RESTART CLEARS IT.**
    `playtest_start` with `/Game/Maps/Lvl_FerrumCrossing` -- the level is under `/Game/StratMaps/`
    -- logged `Failed to find object`, then `Failed to create editor preview world` after 99.4 s,
    and left `playing=true, in_progress=true, has_pie_world=false`. `playtest_stop()` returned
    `requested end play` three times over nine seconds with nothing moving, a retry was refused
    with `Play session is already active or queued.`, and `playtest_console("stat fps")` answered
    `No PIE viewport found for input.` -- so the flag was latched with no session behind it. The
    user restarted the editor; nothing else worked.
  - **`playtest_console("GetAll ...")` RETURNS NOTHING THROUGH LUA.** The command writes to the
    output log and the Lua return is empty, so read the answer out of
    `Saved/Logs/Stratocracy.log`. A caller that trusts the return value concludes the widget holds
    nothing.
  - **COUNTING `then` -> `execute` CONNECTIONS OUT OF `read_graph` COUNTS EVERY EXEC LINK, NOT ONLY
    THE ENTRY-TO-RESULT ONE.** `PaintRows` reports 16 by that measure because its intermediate
    nodes carry pins of the same names. It equals the entry-to-result wire only in graphs whose
    other nodes are pure, which is true of all six binding functions and of nothing else here.
    Wiring was therefore done by node handle off `K2Node_FunctionEntry` / `K2Node_FunctionResult`,
    never by that count.

- **A PLAYTEST TRAP THAT COSTS A CYCLE AND LOOKS EXACTLY LIKE THE BUG BEING INVESTIGATED.** The
  first correct-path PIE run armed nothing: `LogStratPlay: Guided opening suppressed for side 0:
  this save has a completed match.` `Saved/SaveGames/StratocracyMatch.sav` existed, so the live
  `Guidance` read all-default and a capture taken then would have shown a blank strip with no error
  anywhere -- a false negative indistinguishable from the defect. The save was moved aside, the run
  repeated, and the file restored afterwards at its original SHA-256
  `e166cab5451a8a223dcd1128d4ea87926a74949b4b71f5f24fa61d00e78b7003`. **Check the arming line in
  the log before trusting any guided-opening capture.**

- **WRITTEN BY THE COORDINATOR, NOT BY `strat-editor-builder`, AND THAT IS A LANE DEVIATION
  RECORDED RATHER THAN HIDDEN.** 2026-08-22. `execute_script` was absent from the session's tool
  surface for the whole of this phase, which leaves `strat-editor-builder` with no way to reach the
  editor at all -- it has the NeoStack tools plus Read/Grep/Glob/Skill and no Bash. The coordinator
  drove the live editor through `Tools/neostack/ns.py` instead and therefore wrote this entry. The
  header above still names `strat-editor-builder` as sole writer and that is unchanged as policy;
  this entry is the exception, not a precedent. **A gate should evaluate the deviation rather than
  rediscover it.**
- **§2.11.5's production menu exists as an asset: `Content/UI/WBP_ProductionMenu`.** A plain
  `UUserWidget` with NO C++ parent, deliberately -- `engine.md` records why (a derived Blueprint
  bakes a `/Script/Module.Class` path irreversibly, and no caller needed a base class). It reaches
  the rules through `UStratMatchSubsystem` via `Get World Subsystem`. Structure: a `SizeBox` root
  at 680 px, a `Border`, a title, a status line, and a `UniformGridPanel` of four unit rows by four
  columns plus a BUILD button per row. `RefreshMenu(FactoryHex)` calls `RefreshProductionMenu` and,
  on success, a shared `PaintRows` function fills the sixteen cells; on refusal it paints the
  bridge's own reason into the status line and leaves the rows alone. Each BUILD button calls
  `SubmitProductionChoice` with that row's own `DefIndex`, read positionally.
- **T-UI-03 is held by CONSTRUCTION, not by intent.** `bAffordable` and `bAvailable` are two
  separate columns (`AFFORD`, `FACTORY`) and no node combines them; the only computation anywhere
  in either graph is `To Text (Name|Integer|Boolean)` formatting. Rows are read at index 0..3 and
  never sorted, so the bridge's order survives.
- **THE MENU'S FOUR ROWS ARE A HARD-CODED COUNT MATCHING `Data/units.csv`** (Infantry, Tank,
  Artillery, Recon). Correct today and `Data/` is vendored read-only, but a fifth unit row would
  silently not appear -- the widget would show four of five with no error. The fix if the table
  ever grows is a runtime loop over `ProductionMenu` building rows dynamically. Recorded as a known
  limit, not a defect.
- **The BUILD affordance is `IA_OpenProductionMenu` on key `B`**, mapped in `IMC_Selection`
  (7 entries now, was 6). `B` was chosen because LMB, RMB, Escape, W, Enter and SpaceBar were all
  taken. Blueprint defaults set and verified by read-back:
  `BP_StratScoreboardHUD.ProductionMenuWidgetClass` -> `WBP_ProductionMenu_C` (`ZOrder` 20, above
  guidance's 10), and `BP_StratPlayerController.OpenProductionMenuAction` -> the new action.
  `SelectAction` and `SelectionMappingContext` were confirmed unchanged by the same read.
- **T-UI-04 IS NOT CLOSED BY ANY OF THIS.** Everything above is design-time evidence: a clean
  compile and a verified graph topology. T-UI-04 asserts the menu BINDS, which is a claim about a
  running widget, and injected input never reaches `UPlayerInput` on this project -- so it needs a
  human-driven playtest. For the suite figure and any phase verdict see `global.md`; this file
  states neither.
- **NEW MEASURED LIMITS OF THE NeoStack Lua API**, all 2026-08-22, all with a control:
  - **`find_nodes` cannot see a class's member functions without a pin context.** `Is Match Live`
    and `Set Viewing Side` -- both long-standing `UFUNCTION`s -- return zero hits exactly like a
    function that does not exist, which reads as a stale DLL. The pin-context form is the answer:
    `find_nodes({query=..., from_handle=<node>, from_pin="Return Value"})` off a
    `Get StratMatchSubsystem` node returned all five production members at score 85-100. **Control
    first**: query a member of the same class that certainly exists before concluding anything.
  - **`add_custom_event` takes `params=`, NOT `inputs=`.** The `neostack-widget` skill says
    `inputs=`; that is silently ignored and the event is created with zero parameters. `bp:help()`
    is right where the skill is wrong.
  - **`bind_event`'s custom-name argument does not retitle the node.** The node is
    `On Clicked (Row0Build)` regardless, so a graph search for the name you passed finds nothing.
  - **`UniformGridSlot` has no `Padding`** -- only `HorizontalAlignment`, `VerticalAlignment`,
    `Row`, `Column`. Cell padding is the PANEL's `SlotPadding`.
  - **`class_properties` returns only DIRECTLY-DECLARED properties and does not unwrap a
    `UScriptStruct`.** It answered 0 for `FStratBuildOptionView` and for a subsystem whose
    inherited members are many; `/Script/UMG.CanvasPanelSlot` returning its 3 own properties is the
    control that shows the instrument works.
  - **`list_assets` does not exist, and `list_properties()` requires an argument.** Both abort the
    WHOLE script at that line, so everything after them silently does not run -- a script that
    "did nothing" may simply have died on line 2.
  - **The WidgetBlueprint preview capture renders at fill and ignores a `SizeBox` root.** Verified
    the root IS a `SizeBox` with `bOverride_WidthOverride` true at 680; the preview looked
    identical before and after. **A preview screenshot is not a runtime width test.**
- **The editor's source-control integration STAGED a new `.uasset` in git without being asked**
  (`A  Content/StratInput/IA_OpenProductionMenu.uasset` appeared in `git status` after
  `create_asset` + `save`). Unstaged with `git restore --staged`. Worth knowing before a commit:
  the index can carry an asset nobody staged.

- **`AWorldSettings` is unreachable through the NeoStack Lua API**, so no level's GameMode
  override can be authored that way. Exact failures: `configure("actor","WorldSettings")`,
  `select_actor`, `get_actor_properties`, `open_asset` on the sub-object path, and
  `invoke({actor_label="WorldSettings"})` all failed; `configure` accepts only
  `actor|landscape`. (Worked around this phase for the `Config` question by reading the two
  levels' binary `.umap` bytes directly instead — see Phase 5 below.)
- **`array_count("DefaultKeyMappings.Mappings")` → `property not found`** — dot-path into a
  struct's array is unsupported; write the whole struct instead. **Generalises to struct members,
  not just arrays** — phase C (AI-opponent milestone) hit the same failure on
  `get`/`set("self", "MatchConfig.AiSides")`, a plain (non-array) struct field, and had to read
  and rewrite the whole `MatchConfig` struct. Whole-struct read/write is the standing workaround
  for both cases.
- **`FKey` `ImportText` takes the bare name.** `Key=(KeyName="X")` silently produced `Key=()`;
  `Key=LeftMouseButton` succeeded. A silent empty-key write is exactly the kind of failure that
  reads as an input bug later.
- Three findings for `strat-gameplay-engineer`, none blocking: (1) `AStratCameraPawn` sets
  `Arm->bInheritYaw = false` at `StratCameraPawn.cpp:59`, pinning the camera to world yaw 0 so
  spawn rotation cannot turn the view — measured, `PlayerStart` yaw `-90` and `0` give
  pixel-identical framing; Ferrum Crossing's long axis (2800×1385, long axis on world X) is
  stuck on the screen's short axis. An `ArmYaw` property or `bInheritYaw = true` fixes it; the
  builder compensated with `DefaultArmLength = 3600`. (2) `AStratBoardActor` has
  `TerrainMeshes` but no `TerrainMaterials` map, which is the sole reason there are seven
  near-identical `SM_HexTile_*` assets instead of one mesh and seven materials. (3)
  `MI_Terrain_Water.Color` reads back `(0.02, 0.09, 0.30)` yet renders pale blue, and darkening
  every terrain colour ~55% produced no visible change — the harvested `M_Mono` does not
  respond linearly to `Color`; a purpose-built opaque `M_StratTile` would remove the guesswork.
- **Scope note:** `BP_StratCamera` is a sixth Blueprint beyond the four the brief named. The
  builder judged it inside "the Blueprints" because `AStratCameraPawn`'s C++ default
  `DefaultArmLength = 1200` shows about a third of the board. It holds no logic — four float
  defaults only. Recorded so a gate can evaluate the judgement rather than rediscover it.
- **The map→GameMode binding is a single unguarded line.**
  `Config/DefaultEngine.ini:4` (`GlobalDefaultGameMode`) is now the *only*
  thing binding `Lvl_FerrumCrossing` to `BP_StratGameMode_C`, because
  `AWorldSettings` is unreachable through the NeoStack Lua API and the level
  carries no World Settings override — unlike `Lvl_TopDown`, which does (see
  Phase 5's `Config` flip evidence above). This fact is not recorded in
  `.agents/ue-project-context.md`, which is where a future config edit would
  most plausibly be checked against. Owed to a future doc pass. **No crew
  agent owns `.agents/ue-project-context.md`** — this is a flag, not a task
  the steward takes.
