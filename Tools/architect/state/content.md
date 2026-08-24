# Stratocracy state — content

> **Writer: `strat-editor-builder`; fallback writer `coordinator`, under the one condition below.**
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
> **Holds:** Assets, Blueprint defaults, PIE evidence, and the measured limits of the NeoStack Lua API.
>
> **One file owns each fact class.** Other files may *link* to a fact recorded here; they may not
> restate it. Only `global.md` may carry a live suite count or a phase verdict — a live `N/N`
> in any other file is a finding, enforced by `strat_banner_sweep.py`'s RECORD OWNERSHIP check.
> Everything under `## NEXT` is swept as live; stamp an entry that has become history rather
> than deleting it, exactly as `state.md` did.

## NEXT

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
