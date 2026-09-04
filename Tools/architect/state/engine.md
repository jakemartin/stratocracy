# Stratocracy state — engine

> **Sole writer: `strat-gameplay-engineer`, except that the `coordinator` transcribes
> lane-authored entries after a merge, attributing acting and writing separately (user ruling,
> 2026-08-29; the clause and its limits are stated in `CLAUDE.md`, which is the authority).** Split out of `Tools/architect/state.md` on 2026-08-20, which is now
> FROZEN as the historical record of closed milestones. This directory is the live record.
>
> **Holds:** The `StratBridge` / `StratPlay` / `StratUI` C++ record — module arrows, deliberate absences, deferred production surface, and the branches a citation must name.
>
> **One file owns each fact class.** Other files may *link* to a fact recorded here; they may not
> restate it. Only `global.md` may carry a live suite count or a phase verdict — a live `N/N`
> in any other file is a finding, enforced by `strat_banner_sweep.py`'s RECORD OWNERSHIP check.
> Everything under `## NEXT` is swept as live; stamp an entry that has become history rather
> than deleting it, exactly as `state.md` did.

## NEXT

- **2026-09-03, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `0b17685`, working tree clean at dispatch,
  UNCOMMITTED) -- THE OPEN `AiSides` QUESTION THIS RECORD FILED IS ANSWERED BY USER RULING, AND
  THE THREE PRODUCTION SITES THAT TURNED ON IT ARE REPAIRED. THE DEBT BELOW STAYS OPEN AND IS
  NOT STAMPED.** Comment-only changes; no executable line moved in any file. No count is written
  here; the live figure is `global.md`'s alone, and this pass ran no suite (see the last bullet).
  - **THE RULING, AND IT IS THE USER'S, 2026-09-03, IN SESSION.** (1) `BP_StratGameMode`'s
    authored `MatchConfig.AiSides=(1)` is **INTENDED** -- the shipped game is human-versus-AI,
    side 0 (Directorate) the human and side 1 (Vanguard) the AI. It is not Blueprint drift from a
    hot-seat intent and the asset is correct as authored; no `Content/` byte changes.
    (2) The `Easy` starting-Fame handicap landing on the **HUMAN** side is **ALSO INTENDED**,
    because Easy is meant to help the player. `StratHandicappedSide` returning `ViewingSide` (0)
    on the shipped configuration is correct behaviour, not a defect, and **no logic changed.**
  - **THE MEASUREMENT IS NOT MINE AND IS NOT RESTATED AS MINE.** It was taken off the live editor
    on 2026-09-03 by the `coordinator`, and this lane received it as given. Its controls:
    `BP_StratGameMode_AiVsAi` reads `AiSides=(0,1)`; `struct_properties("StratMatchConfig")` gives
    the C++ default as empty; `BP_StratShellGameMode` carries no `MatchConfig` at all. `ViewingSide`
    0 and `Difficulty` `Easy` are absent from both the native and the BP export, which is what a
    zero-valued field at its C++ default looks like. Walking `StratHandicappedSide` on those values:
    ARM ONE wants `AiSides.Num()==0` and it is 1; ARM TWO wants `AiSides.Contains(ViewingSide)` and
    it holds 1, not 0; both fall through and it returns `ViewingSide` = 0. **I did not read the
    asset and this entry claims no such reading.**
  - **WHAT WAS REPAIRED, AND WHAT WAS DELIBERATELY PRESERVED.** The false claim is *"an empty
    `AiSides` is the shipped configuration"* and its consequence *"the `Easy` default is inert as
    shipped"*. The TRUE half -- that an empty `AiSides` is the **C++ default** of
    `FStratMatchConfig`, and the configuration every fixture that leaves the field alone builds --
    is preserved everywhere and **re-attributed rather than deleted**, because that distinction is
    the entire content of the repair. Cited by symbol, never by line:
      - `FStratMatchConfig`'s **AI-block preamble**, `Source/StratPlay/StratMatchSubsystem.h` --
        rewritten, and it now states the rule the rest of the file is repaired against: a comment
        describing an empty `AiSides` must say "the C++ default" and must not say "the shipped
        configuration".
      - `FStratMatchConfig::Difficulty`'s **inertness block**, same file -- the
        `THAT DEFAULT IS INERT ON THE SHIPPED CONFIGURATION` sentence is stamped `RETRACTED>` in
        place with the ruling beside it. The `Easy`-not-`Normal` argument is kept intact and its
        independence is now stated: it never depended on which configuration ships.
      - `StratHandicappedSide`'s **declaration comment**, same file -- ARM ONE's *"This is the
        shipped configuration."* stamped `RETRACTED>`; the OTHERWISE arm now says in terms that it
        is the arm the shipped game takes.
      - `StratHandicappedSide`'s **definition comment**, `Source/StratPlay/StratMatchSubsystem.cpp`
        -- *"ARM ONE -- the shipped hot seat."* stamped `RETRACTED>`, and a new ARM THREE comment
        sits on the `return Config.ViewingSide;` that the shipped game actually reaches.
  - **A FOURTH SITE THE BRIEF'S STARTING SET DID NOT NAME, FOUND BY RUNNING THE SWEEP RATHER THAN
    THE LIST.** `AStratGameMode`'s opening-AI-turn comment, `Source/StratPlay/StratGameMode.cpp`,
    read *"In a hot-seat match with no `AiSides` configured -- which is the default and is every
    match before phase C"*. The historical half is true and is kept; *"which is the default"* is
    re-attributed to the C++ default and the shipped `AiSides=(1)` is named. The brief said to
    treat its four sites as a starting set and not a complete one, and that instruction is what
    produced this one.
  - **SCOPE HELD TO `AiSides` ON THE USER'S EXPLICIT DECLINE.** The `MoveTweenSeconds` and
    `AiPlaybackStepSeconds` sites were NOT touched. They are the other half of the debt below and
    they are why it is not stamped.
  - **`Source/StratPlay/Tests/StratAiMatchClauses.cpp` CARRIES THE SAME FALSE CLAIM AND I LEFT IT
    UNTOUCHED.** Its file-header *"THE DEFAULT IS INERT"* bullet and the `Base` comment on the
    empty-`AiSides` path call those "the shipped defaults". `Tests/` is `strat-test-author`'s lane
    and is being dispatched separately; an engineer editing it would be an out-of-lane write with a
    lane standing to take it.
  - **THE THREE `RETRACTED>` MARKERS THIS PASS WROTE WERE NON-COMPLIANT WITH A CONVENTION THIS
    TREE DECLARES, AND WERE RE-ANCHORED IN THE SAME PASS ON THE `coordinator`'S CORRECTION.**
    The authority is the `THE CONVENTION, and it is per-LINE and not per-paragraph` block at the
    head of `Source/StratPlay/Tests/StratSelectionMachineParity.cpp` -- cited by block rather
    than by line, because lines move. It requires the token to sit at the head of a comment line
    after the leader (`//` or `*`), since the meaning comes from POSITION and never from mere
    presence, and it states in terms: *"A partial-line quotation is not retractable and must not
    be written."* All three of mine opened MID-LINE.
  - **MEASURED BY THE DECLARED RECIPE, BEFORE AND AFTER, NOT BY EYE.** The recipe is that block's
    own anchored filter, `grep -vE '^[[:space:]]*(//|\*)[[:space:]]*RETRACTED> '`, run directly
    over the file. **BEFORE: `StratMatchSubsystem.h` 45 markers / 43 anchored / 2 SURVIVING;
    `StratMatchSubsystem.cpp` 10 / 9 / 1 SURVIVING** -- three withdrawn quotations reading as
    LIVE PROSE to any sweep, which is the exact failure the convention exists to prevent.
    **AFTER: `.h` 47 / 47 / 0; `.cpp` 10 / 10 / 0.** Run at the phrase level too, the three
    withdrawn claims -- *the shipped hot seat*, *INERT ON THE SHIPPED*, *This is the shipped
    configuration* -- now return ZERO survivors in both files.
  - **WITH A CONTROL, BECAUSE ZERO SURVIVORS IS AN ABSENCE AND AN ABSENCE NEEDS ONE.** The same
    anchored filter over live prose on the SAME axis still returns it: `IT IS NOT THE SHIPPED`,
    `THE SHIPPED GAME IS HUMAN-VERSUS-AI`, `THIS IS THE ARM THE SHIPPED GAME TAKES` and
    `WHICH IS THE SHIPPED CONFIGURATION` all survive. So the filter subtracts withdrawn
    quotations and not live claims, and the zeroes above are evidence rather than a broken grep.
  - **NO QUOTATION WAS DROPPED; ALL THREE WERE KEPT AND BROKEN ONTO WHOLE LINES.** The convention
    offers "or do not quote them" as the second lawful option and I did not need it. The
    `Difficulty` block's withdrawn paragraph now occupies THREE whole marked lines, and
    `StratHandicappedSide`'s two withdrawn sentences one each. A greppable withdrawn claim is the
    point, so keeping them was preferred wherever the sentence could be broken without mangling
    the paragraph -- it could, in all three.
  - **BUILD, AND NO SUITE RUN.**
    `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" StratocracyEditor
    Win64 Development -project="E:\MultiAgent\Stratocracy\Stratocracy.uproject" -waitmutex` ->
    `Result: Succeeded`, 36 actions, `UnrealEditor-StratPlay.dll` relinked; REBUILT after the
    `RETRACTED>` re-anchoring above and `Result: Succeeded` again. A comment-only change still
    changes the translation units' inputs, which is why each was built rather than assumed.
    **The suite was deliberately NOT run**: a rebuild moves the report's identity and every
    citation of the previous run with it, and the `coordinator` sequences that.

- **2026-09-03, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED) -- RECORD REPAIR ONLY, NO
  `Source/` BYTE CHANGED: F4 AND ITS UNREPORTED TWIN, AND THE CLAIM-SHAPE SWEEP RUN OVER THE WHOLE
  LANE RATHER THAN OVER `Tests/`. THE SWEEP FOUND THE SAME DEFECT IN PRODUCTION PROSE AND IT IS
  DECLARED BELOW AS A DEBT, NOT REPAIRED.** No build and no suite run is claimed by this pass and
  no count is written here; the live figure is `global.md`'s alone.
  - **THE DEFECT SHAPE, STATED ONCE, BECAUSE FOUR ENTRIES NOW TURN ON IT.** Prose that calls a C++
    FIELD default "the shipped default" when a shipped Blueprint serialises an override for that
    field. It is not a wording preference: it is the belief that caused a measured regression --
    see the `[CORRECTED 2026-08-29]` block at `AStratPlayerController::HandleCancelSelectionEvent`,
    where believing AI playback shipped inert on the zero default cost the player's first click
    after every AI hand-over.
  - **THE DECIDING MEASUREMENT, AND WHAT IT DOES AND DOES NOT PROVE.** `grep -aF` over the raw
    bytes of `Content/StratPlay/BP_StratUnit.uasset`, `BP_StratGameMode.uasset` and
    `BP_StratGameMode_AiVsAi.uasset`. `AiPlaybackStepSeconds` and `AiSides` are present in both
    GameMode Blueprints and absent from `BP_StratUnit`; `MoveTweenSeconds` is the exact mirror --
    present in `BP_StratUnit`, absent from both GameModes; a fabricated property name is absent
    from all three. ~~So the instrument discriminates per asset AND per property in both
    directions. **A NAME-TABLE HIT PROVES THE PROPERTY IS SERIALISED**~~, **NOT WHAT IT HOLDS**,
    **[BOTH STRUCK CLAUSES RETRACTED 2026-09-03, `strat-gameplay-engineer` (ACTING and WRITING; IN
    LANE, main tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED), ON THE THIRD
    RE-GATE'S F8. THE CROSS-ASSET CONTROL ABOVE COULD NOT SEE WHAT REFUTES THEM: a WITHIN-CLASS
    control run on `BP_StratUnit` the same day returns 1 for `FlagMarkerMesh`, which this record
    elsewhere says is UNSET, so on the ACTOR Blueprints a hit does NOT prove an override was
    serialised and the instrument does not discriminate "in both directions" there. WHAT REPLACES
    THEM, AND IT IS NOT A NARROWING TO NOTHING: on `BP_StratGameMode` and `BP_StratGameMode_AiVsAi`
    the instrument discriminates in both directions and every sentence in this entry that rests on
    those two assets stands unaltered; on `BP_StratUnit` and `BP_StratBoard` PRESENCE PROVES
    NOTHING and ABSENCE IS DECISIVE. The counts, the negative control and the three legs of the
    absence direction are in the THE NAME-TABLE INSTRUMENT bullet below, which a reader landing
    here by citation must read before citing this one. Nothing in this file may now cite a byte
    HIT on an actor Blueprint as proof of an override.]** which is why nothing
    here states a value; `content.md` records the authored `MoveTweenSeconds` independently, and
    `strings` does NOT work for this measurement -- it fails its own positive control.
  - **TWO SITES REPAIRED IN THIS FILE, AND TWO MORE OF THE SAME CAUSE FOUND BY THE SWEEP.** F4 of
    `Tools/architect/gate_reports/2026-09-03-unit-movement-and-existence-hold-regate.md`
    (`AiPlaybackStepSeconds`, in the live "WHAT THE TEST LANE IS OWED" list) and the user's own
    second finding (`MoveTweenSeconds`, in the 2026-09-02 route-slide entry). The sweep then
    surfaced two further instances inside that same entry -- the `TweenRestOffset` bullet and the
    "AT THE SHIPPED C++ DEFAULT" heading. All four are corrected IN PLACE and stamped INLINE on
    the sentence they correct rather than only here, because a reader arriving by a citation lands
    on the sentence. In every one of the four the CONSEQUENCE was true and only the configuration
    was misnamed: no automation fixture executes those paths, because a fixture builds the C++
    object and gets the C++ default.
  - **WHY TWO GATES MISSED IT, WHICH IS THE ACTUAL LESSON.** The F1 repair's sweep was scoped to
    `Source/**/Tests/` while the defect was not scoped to anything, and it matched a PHRASE where
    the defect is a SHAPE. This pass swept 76 files -- this record plus every non-`Tests/` `.h`,
    `.cpp` and `.cs` in `StratPlay`, `StratBridge` and `StratUI` -- by joining every line into one
    string and stripping comment leaders first, then matching {shipped, ships, the real, actual, in
    practice} within 120 characters of "default" in either order. **JOINING IS LOAD-BEARING, AND THE
    COST OF NOT JOINING IS MEASURED RATHER THAN ASSERTED**: the `MoveTweenSeconds` instance the
    user found is wrapped across two lines and is invisible to any line-oriented grep, and so are
    THIRTEEN of the matches in the four production files below -- three in
    `StratMatchSubsystem.cpp`, five in `StratMatchSubsystem.h`, none in `StratUnitActor.cpp` and
    five in `StratUnitActor.h`. That count is of MATCHES, not of defects; the rulings are separate.
  - **DEBT, DECLARED AND NOT REPAIRED: THE SAME SHAPE IS IN PRODUCTION PROSE AT ROUGHLY TWENTY
    SITES IN FOUR FILES I OWN, EACH CITED BY THE SYMBOL IT SITS IN AND EACH VERIFIED TO EXIST.**
    `StratMatchSubsystem.cpp` (eight, in `ApplyView`, `RunAiTurnsNow`, `WillAiPlaybackRun`,
    `BeginAiPlayback`, `ArmNextPlaybackStep`, `AdvanceAiPlaybackOneStep` twice and
    `EndAiPlaybackTour`), `StratMatchSubsystem.h` (six, on the declaration blocks of
    `AdvanceAiPlaybackOneStep`, `BeginAiPlayback` twice, `PreParkPicturesForTour`,
    `EndAiPlaybackTour` and `LastArmedSlideSeconds`, plus `AiPlaybackStepSeconds`' own *"this
    ships inert"* sentence, which is the one that names the mechanism correctly in its second
    half and asserts the shipped state falsely in its first), `StratUnitActor.cpp` (five, in
    `ApplyUnitView` twice -- the snap and the park-clearing branch -- `PlayRouteSlide`,
    `ParkPictureAt` and `FinishTween`) and `StratUnitActor.h` (two, on the blocks of
    `ParkPictureAt` and `FinishTween`/`TweenRestOffset`). **NOT REPAIRED THIS PASS.**
    ~~THE REASON IS SPECIFIC RATHER THAN CONVENIENT: the brief for this pass is a record repair,
    and editing twenty comment blocks would make every `Source/` file newer than the exported
    report the re-gate's own suite-freshness argument rests on, inside a tree already under
    `VERDICT: BLOCK`.~~
    **[CORRECTED 2026-09-03, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master`
    in the main tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED), on the
    user's measurement and re-measured here rather than accepted. THE STRUCK REASON IS FALSE, AND
    IT IS THE SAME DEFECT CLASS THIS ENTRY EXISTS TO REPAIR -- A PLAUSIBLE MECHANISM ASSERTED
    WITHOUT BEING MEASURED. `strat_banner_sweep.py`'s `read_macro_census` updates `newest_mtime`
    ONLY inside its `if s or c:` branch -- that is, only for a `.cpp` that CONTAINS
    `IMPLEMENT_SIMPLE_AUTOMATION_TEST` or `IMPLEMENT_COMPLEX_AUTOMATION_TEST(_CLASS)`. A
    production file with no test macro never contributes to the newest-test-source mtime, so
    editing its comments cannot trip REPORT IDENTITY, and none of the four files in this debt
    defines a test macro. MEASURED BOTH WAYS RATHER THAN READ OFF THE SOURCE, each probe
    saving and restoring the file's mtime and verifying bytes and mtime identical afterwards:
    touching `Source/StratPlay/StratUnitActor.cpp` (production, no test macro) -> sweep EXIT
    0, ZERO `REPORT IDENTITY` findings, `SWEEP CLEAN`; touching
    `Source/StratPlay/Tests/StratTourExistenceHoldClauses.cpp` (defines test macros) -> sweep
    EXIT 1, ONE `REPORT IDENTITY` finding, `SWEEP FAILED`. THE SECOND PROBE IS THE CONTROL AND IS
    WHAT MAKES THE FIRST'S SILENCE EVIDENCE RATHER THAN AN ABSENT INSTRUMENT. No byte and
    no mtime under `Source/` differs after the probes.]**
    **THE DEFERRAL STANDS, AND ITS TRUE REASON IS SCOPE -- REPAIRING NOW WAS THE OTHER ROUTE
    OFFERED AND WAS DECLINED ON THESE THREE GROUNDS.** (1) The ~21 sites are RULINGS, not a
    substitution: this entry already says the thirteen wrapped hits are a count of MATCHES and
    that the rulings are separate, so each site needs its own reading against the asset evidence,
    which is a different pass from a record repair. (2) **One of them is explicitly OPEN and is
    not mine to rule** -- the `EStratDifficulty Difficulty` block in `StratMatchSubsystem.h`,
    whose consequence depends on the authored `AiSides` value only the content lane can read
    (the bullet below).
    **[CORRECTED 2026-09-03, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, main tree
    `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED), ON THE SECOND RE-GATE'S
    F5. THE CITATION THIS SENTENCE CARRIED, AND THE ONE IN THE BULLET BELOW, NAMED A SYMBOL
    THAT DOES NOT EXIST -- and it sat in the one bullet whose stated purpose is to be
    greppable, which is what makes it a finding rather than a typo.]** Both read
    RETRACTED> "`StratMatchSubsystem.h`'s `StratAiDifficulty` block".
    **`StratAiDifficulty` RETURNS ZERO HITS ACROSS `Source/` AND `Config/`, WITH
    `EStratDifficulty` RETURNING 34 IN `Source/` AS THE CONTROL** -- so the instrument speaks
    and the absence is evidence rather than a failed search. A fabricated identifier greps
    exactly like a defined one until someone looks, which is the whole reason the replacement
    was grepped BEFORE it was written here: `EStratDifficulty` (the `UENUM`),
    `FStratMatchConfig` (the struct that holds the field), and the field itself,
    `EStratDifficulty Difficulty = EStratDifficulty::Easy;`, all resolve in
    `Source/StratPlay/StratMatchSubsystem.h`. Cited by symbol and not by line, because lines
    move and this record has been bitten by that before.
    Rewriting the blocks around it would either force an unmeasured ruling on it or leave one
    file half-repaired inside a single declaration block. (3) A comment edit in a production file
    still changes that translation unit's inputs, so it is rebuild-then-run-then-record; this
    pass performed no build and no suite run and claims neither, and doing them is the discharging
    pass's work, not a record repair's.
    **DISCHARGED WHEN** the next pass that compiles this module rewrites those blocks to say "the
    C++ field default" and states, once per file, that both fields are overridden on the shipped
    Blueprints -- at which point this bullet is stamped, not deleted. **[SCOPE AMENDED
    2026-09-03 ON THE SECOND RE-GATE'S F6: this condition is NOT satisfied by rewriting only the
    symbols enumerated above. Three further sites were added to it -- see the SCOPE WIDENED
    sub-bullet below, which names them. A reader who stops at this sentence would otherwise
    stamp the debt closed with those three still standing, which is exactly what F6 found.]**
  - **ONE HIT LEFT OPEN AS A QUESTION RATHER THAN RULED, BECAUSE A NAME-TABLE HIT CANNOT ANSWER
    IT.** The `EStratDifficulty Difficulty` block in `StratMatchSubsystem.h` **[CORRECTED
    2026-09-03 BY THE SAME HAND IN THE SAME PASS AS THE F5 STAMP ABOVE, WHICH CARRIES THE
    MEASUREMENT AND THE CONTROL. This site read RETRACTED> "`StratAiDifficulty` block" and
    named nothing in the tree.]** and this record's own 2026-08-30
    copy of it say *"`AiSides` defaults empty, so on the shipped hot seat that default reaches
    `StratHandicappedSide` and comes back `INDEX_NONE`"*. `AiSides` IS serialised in
    `BP_StratGameMode.uasset`, so the sentence's antecedent is the C++ default and not the shipped
    one -- but whether the CONSEQUENCE still holds depends on the authored value, which my
    instrument cannot read. **This is the same conflation the re-gate declared open at
    `Source/StratPlay/Tests/StratAiMatchClauses.cpp:380` and it wants the same owner's
    measurement**: the content lane reading `BP_StratGameMode`'s `AiSides` off the asset. Recorded
    here so it is greppable rather than lost.
    **[ANSWERED 2026-09-03 BY USER RULING -- SEE THE TOPMOST ENTRY, WHICH CARRIES THE RULING AND
    WHOSE MEASUREMENT IT IS. THE ANSWER IS `AiSides=(1)`, INTENDED: THE SHIPPED GAME IS
    HUMAN-VERSUS-AI AND NOT A HOT SEAT, AND THE `Easy` HANDICAP LANDING ON THE HUMAN IS ALSO
    INTENDED.** So the CONSEQUENCE this bullet declared unknowable is now known and it is FALSE:
    the default does not come back `INDEX_NONE` on the shipped configuration, it comes back
    `ViewingSide` = 0. The three production sites named in the SCOPE WIDENED sub-bullet below are
    **REPAIRED** in that same entry's pass, plus a fourth in `StratGameMode.cpp` that no
    enumeration here had reached. **THIS QUESTION IS CLOSED. THE DEBT IS NOT, AND MUST NOT BE READ
    AS CLOSED -- see the DISCHARGED WHEN sentence below, which this ruling does not satisfy.]**
  - **[SCOPE WIDENED 2026-09-03, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, main
    tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED), ON THE SECOND
    RE-GATE'S F6.] THE OPEN `AiSides` QUESTION ABOVE HAS MORE THAN ONE PRODUCTION SITE AND THIS
    RECORD NAMED ONE.** The others are `StratHandicappedSide`'s own prose -- the function the
    quoted sentence sends the reader TO -- and neither half of it appears in the symbol
    enumeration the DISCHARGED WHEN condition points at. **So a pass that faithfully walked
    every cited symbol could have stamped this debt closed while both survived unstamped, in
    the very file the question is about. That is the defect: not the two comments, but a scope
    that did not reach them.** The three sites added to the condition, each verified to resolve
    before being written:
      - **`StratHandicappedSide`'s DECLARATION comment**, `Source/StratPlay/StratMatchSubsystem.h`
        -- *"`AiSides` EMPTY -> `INDEX_NONE`. ... This is the shipped configuration."*
      - **`StratHandicappedSide`'s DEFINITION comment**, `Source/StratPlay/StratMatchSubsystem.cpp`
        -- *"ARM ONE -- the shipped hot seat."*
      - **`IsAiPlaybackRunning`'s declaration comment**, `Source/StratPlay/StratMatchSubsystem.h`
        -- *"at the shipped `AiPlaybackStepSeconds` of zero"*. This one was found by the widened
        sweep below and by nothing else: it is a FOURTH `.h` site on the OTHER field this debt
        is about, and it is not among the seven `.h` symbols already enumerated.
    **STILL DEFERRED AND NOT REPAIRED, ON LAST PASS'S TRADE UNCHANGED.** Repairing them means
    ruling what `AiSides` holds on the shipped GameMode -- the question this bullet declares
    OPEN and hands to the content lane -- and the re-gate states in terms that no production
    byte need change to clear it. **DISCHARGED WHEN** the pass that rewrites the enumerated
    `AiPlaybackStepSeconds` blocks ALSO rewrites these three. The condition now NAMES them, so
    it can no longer be satisfied while they stand, which is the only thing F6 asked for.
    **[HALF-MET 2026-09-03, AND HALF-MET IS NOT MET. THIS DEBT REMAINS OPEN AND IS DELIBERATELY
    NOT STAMPED.]** The topmost entry's pass repaired **TWO OF THE THREE SITES
    LISTED IMMEDIATELY ABOVE AND NOT THE THIRD** -- both of `StratHandicappedSide`'s comments,
    which are the two `AiSides` sites; **`IsAiPlaybackRunning`'s declaration comment was NOT
    touched**, because it is an `AiPlaybackStepSeconds` site and the user's scope ruling excluded
    that field. It also repaired the `Difficulty` and AI-block prose and a fourth site in
    `StratGameMode.cpp`, on a user ruling that answered the open question. **IT DID NOT REWRITE THE ENUMERATED
    `AiPlaybackStepSeconds` BLOCKS**, because the user explicitly declined to widen that pass
    beyond `AiSides` -- the `AiPlaybackStepSeconds` prose on the field itself, on `BeginAiPlayback`,
    on `WillAiPlaybackRun` and on `IsAiPlaybackRunning`, and the `MoveTweenSeconds` sites, all
    still call a C++ field default "the shipped default" while a shipped Blueprint serialises that
    field. **The condition above is a CONJUNCTION -- the `AiPlaybackStepSeconds` blocks AND
    these three -- and NEITHER conjunct is fully satisfied: the first not at all, the second two
    sites out of three. So it is not satisfied.** A reader arriving at this bullet must not stamp it: the discharging pass is the
    one that rewrites the `AiPlaybackStepSeconds` blocks, and it has not run.
  - **THE ORIGINAL SWEEP WAS PINNED TO A TOKEN AND NOT TO A SHAPE, WHICH IS WHY IT WAS BLIND.
    RE-RUN WIDENED 2026-09-03, WITH THE PATTERN WRITTEN OUT SO THE WIDENING IS CHECKABLE RATHER
    THAN CLAIMED.** The original required the literal word `default` within 120 characters of a
    {shipped, ships, the real, actual, in practice} token. The claim it exists to catch does not
    need that word: *"this is the shipped configuration"*, *"the shipped hot seat"* and *"unset
    is the state this ships in"* all make it without ever writing `default`.
    **THE WIDENED PATTERN**, over the same 75 non-`Tests/` `.h`/`.cpp`/`.cs` files across
    `StratPlay`, `StratBridge` and `StratUI`, comment leaders stripped and all lines JOINED so a
    claim wrapped across lines is visible:
    `\b(shipped|ships|shipping|out of the box|as shipped|on the shipped|in the shipped|the
    authored|as authored)\b` -- with the `default`-nearby test retained ONLY to PARTITION the
    hits and never to filter them. **37 hits the old shape could see; 102 it could not.**
    **THE AXIS WAS GOT WRONG ON THE FIRST RUN AND THE CORRECTION IS THE LESSON, SO IT IS
    RECORDED RATHER THAN QUIETLY DROPPED.** That run also swept {live, actual, actually, the
    real, in practice} and returned **393** hits, which is not a rulable set. Those words are
    the OLD sweep's CO-TOKENS; adding them widens the VOCABULARY, and vocabulary is a different
    axis from the one F6 is about. The axis that matters is *the claim does not need the word*
    `default` *to be made*, so the second run dropped that requirement and left the
    shipping-state vocabulary alone.
    **THE 102, PARTITIONED AND RULED.** **3** are the defect class and were already inside this
    debt's enumeration -- the `AiPlaybackStepSeconds` prose on the field itself, on
    `BeginAiPlayback` and on `WillAiPlaybackRun`; the widened shape found them a second way,
    which is a check on the enumeration rather than a new finding. **3** are the defect class
    and were NOT in it: the two `StratHandicappedSide` sites and `IsAiPlaybackRunning`, added to
    the scope above. **5** are left UNRULED for a measured reason -- see the instrument bullet
    below. **The remaining 91 are correct as written**, and they fall in four groups, listed so
    the next sweep does not re-litigate them: a fact about the shipped SCENARIO or shipped DATA
    (Ferrum Crossing's seven terrains and four factories, `captureTurns = 1`,
    `Data/ferrum_crossing.json`); a fact about a shipped CODE PATH (*"no RNG ships"*, *"the
    shipping AI never refuses"*, *"no shipping caller today"*, *"bit-identical to the tour that
    shipped"*); wording quoted inside an existing RETRACTED or CORRECTED block, which must keep
    the words it quotes; and -- the pattern the deferred repairs move TOWARD -- prose that names
    the Blueprint AS the shipping authority, `StratUnitActor.h`'s *"the shipped duration goes on
    `BP_StratUnit`"* and `StratPlayerController.h`'s *"the shipping `BP_` subclass does set
    it"*. **A clean widened sweep would have been a result worth recording; this one was not
    clean, and the three sites it added are the reason the widening was worth doing.**
  - **THE NAME-TABLE INSTRUMENT DISCRIMINATES IN BOTH DIRECTIONS ON THE GAMEMODE BLUEPRINTS AND
    IN ONE DIRECTION ONLY ON THE ACTOR BLUEPRINTS: THERE, PRESENCE PROVES NOTHING AND ABSENCE IS
    DECISIVE. [HEADING CORRECTED 2026-09-03 ON THE THIRD RE-GATE'S F7; IT READ RETRACTED> "AND DOES
    NOT DISCRIMINATE ON THE ACTOR BLUEPRINTS", WHICH IS FALSE IN THE ABSENCE DIRECTION -- THE
    `BP_StratUnit` ROW BELOW CARRIES THE FOUR ZEROS THAT REFUTE IT.] MEASURED 2026-09-03 WITH A WITHIN-CLASS CONTROL,
    WHICH THE EARLIER CROSS-ASSET CONTROL DID NOT TEST, AND IT IS WHY FIVE OF THE WIDENED
    SWEEP'S HITS ARE LEFT UNRULED RATHER THAN RULED EITHER WAY.** The earlier measurement showed
    a property name appearing in the asset of the class that declares it and not in the others,
    and concluded that a hit means the property is serialised. Running the same `grep -aoF` over
    EVERY member of one class against its OWN Blueprint gives two opposite answers:
      - **`BP_StratGameMode` and `BP_StratGameMode_AiVsAi`, over all 13 `FStratMatchConfig`
        members: only 5 appear** -- `AiPlaybackStepSeconds`, `AiSides`, `AiTurnDelaySeconds`,
        `ScenarioFile`, and `bRecordCompletionOnMatchEnd`, **which appears in
        `BP_StratGameMode` and NOT in `BP_StratGameMode_AiVsAi`.** A member CAN be absent, and
        one is absent from one asset and present in the other, so on these two assets presence
        is evidence. **The `AiSides` reasoning above therefore now stands on a control it did
        not previously have** -- which is worth saying plainly, because the control could have
        come back the other way and did not.
      - **`BP_StratBoard`, over all NINE `EditDefaultsOnly` properties in `StratBoardActor.h`:
        all nine appear -- including `BuildPulseMaterial`, whose own declaration block states in
        terms that "`BP_StratBoard` carries no default here".** **[COUNT CORRECTED 2026-09-03 IN
        PLACE, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, main tree
        `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED): this row read RETRACTED>
        "over all eleven editable properties ... all eleven appear". `StratBoardActor.h` declares
        NINE `EditDefaultsOnly` members -- `HexSize`, `TerrainMeshes`, `FallbackTerrainMesh`,
        `OverlayMesh`, `ReachMaterial`, `TargetMaterial`, `ObjectiveMaterial`, `BuildPulseMaterial`
        and `OverlayZOffset` -- and all nine return 1, re-measured this pass. The five further
        editor-visible members are `VisibleAnywhere` components, not defaults. The conclusion the
        row draws is unchanged; only the count was wrong.]** The only two names that come back
        zero, `TerrainId` and `Tiles`, are bare `UPROPERTY()` runtime members and not editable
        defaults at all.
      - **`BP_StratUnit` DOES NOT BEHAVE IDENTICALLY, AND THE DIFFERENCE IS THE HALF OF THIS
        INSTRUMENT THAT WORKS.** **[CORRECTED 2026-09-03 IN PLACE, `strat-gameplay-engineer`
        (ACTING and WRITING; IN LANE, main tree `E:/MultiAgent/Stratocracy`, base commit
        `283d711`, UNCOMMITTED), ON THE THIRD RE-GATE'S F7. THIS ROW READ RETRACTED> "`BP_StratUnit`
        behaves identically: every editable property returns exactly one ... so on the actor
        Blueprints it discriminates nothing and no conclusion may be drawn from a hit in either
        direction." THE FIRST CLAUSE IS FALSE OF THIS TREE -- FOUR PROPERTIES RETURN ZERO -- AND
        THE CONCLUSION IT CARRIED DISCARDED A WORKING INSTRUMENT, WHICH IS WORSE THAN THE WRONG
        COUNT: IT TOLD THE CONTENT LANE THAT SOMETHING IT CAN USE DOES NOT WORK.]** Re-run this
        pass, `grep -aoF` over the raw bytes of `Content/StratPlay/BP_StratUnit.uasset` against all
        FIFTEEN `EditDefaultsOnly` properties on `AStratUnitActor` plus a fabricated negative
        control: **eleven return 1 and FOUR return 0.**
          - **Zero:** `GuidedMarkerZOffset`, `FlagMarkerOffset`, `UnactedPipOffset`,
            `MoveTweenEaseFraction` -- and the negative control `ZzFabricatedName`, also zero.
          - **One:** `GuidedMarkerMesh`, `GuidedMarkerMaterial`, `FlagMarkerMesh`,
            `FlagMarkerMaterial`, `UnactedPipMesh`, `UnactedPipMaterial`, `MeshByDefId`,
            `FallbackMesh`, `SideMaterials`, `BodyZOffset`, `MoveTweenSeconds`.
        **SO ON THE ACTOR BLUEPRINTS THE INSTRUMENT IS HALF GOOD RATHER THAN BLIND, AND THE TWO
        HALVES MUST BE CITED SEPARATELY.** **PRESENCE PROVES NOTHING** here, and that half of the
        original row survives intact: `GuidedMarkerMesh`, which this record says WAS assigned on
        2026-08-24, and `FlagMarkerMesh`, which the file says is unset, BOTH return 1 -- the same
        answer for a property known set and a property claimed unset. **ABSENCE IS DECISIVE** here,
        on three legs written out so a reader can re-execute them rather than trust them: the
        fabricated name returns 0, so the instrument is not answering PRESENT to everything;
        `MoveTweenSeconds` is known-overridden on evidence independent of these bytes -- the C++
        field default is `0.0f`, the tween path is inert at a non-positive duration, and the user
        watched units slide on 2026-09-03 -- and it returns 1, so a real override DOES write its
        name into this table; and delta serialisation writes a property name only where the
        Blueprint carries an override, which is this project's long-recorded reading of these
        files. **A ZERO THEREFORE SETTLES, FROM THE BYTES AND WITH NO EDITOR, THAT THE C++ FIELD
        DEFAULT IS THE SHIPPED VALUE.**
      - **THE FIRST RESULT THAT DIRECTION BUYS, RECOVERED HERE RATHER THAN LEFT SITTING IN THE
        MEASUREMENT: `MoveTweenEaseFraction` IS ABSENT, SO THE TRAPEZOID THE USER WATCHED ON
        2026-09-03 RUNS AT THE C++ `0.25f` AND NOTHING ON `BP_StratUnit` ALTERS IT.** By the same
        warrant `GuidedMarkerZOffset` ships at `300.0f`, `FlagMarkerOffset` at `(0.0, -40.0,
        300.0)` and `UnactedPipOffset` at `(0.0, 40.0, 300.0)`. Four questions that would have
        needed an editor are closed from a checkout. **`BodyZOffset` and `MoveTweenSeconds` are
        NOT closed this way** -- both read PRESENT, which is the direction that proves nothing, so
        `Source/StratPlay/Tests/StratRouteTweenUnitDouble.h`'s claim about `BodyZOffset` is not
        settled by these bytes either, and that file is the test lane's to correct if it wants it
        settled.
    So the five *"unset is the state this ships in"* sites are ruled NEITHER false NOR correct:
    my instrument cannot see which, and saying so is the honest answer rather than the absent
    one. **[RE-CHECKED 2026-09-03 AGAINST THE SHARPENED RULE ABOVE, ON THE THIRD RE-GATE'S F7, AND
    THE REFUSAL STANDS -- SAID EXPLICITLY HERE RATHER THAN LEFT IMPLIED, BECAUSE A SHARPENED RULE
    IS EXACTLY THE CONDITION UNDER WHICH A STANDING REFUSAL SHOULD BE RE-ASKED.** Every property
    the five sites turn on reads PRESENT, which is the direction that proves nothing:
    `FlagMarkerMesh` 1, `UnactedPipMesh` 1, `FlagMarkerMaterial` 1 and `UnactedPipMaterial` 1 on
    `BP_StratUnit`, and `BuildPulseMaterial` 1 on `BP_StratBoard`. **Not one of the five is an
    ABSENCE**, so the decisive direction never fires on any of them and none is settleable from
    the bytes.]** They are named here so the content lane settles them in the same reading that settles
    `AiSides`, and so the next sweep does not re-derive them from zero --
    `StratUnitActor.h`'s `FlagMarkerMesh` block, its `UnactedPipMesh` block and the
    Sec 2.11.2 markers'-art bullet in its own header block; `StratBoardActor.h`'s
    `BuildPulseMaterial` block; and the `BuildPulseMaterial` arm inside
    `AStratBoardActor::BeginPlay` in `StratBoardActor.cpp`. **`BuildPulseMaterial` is the one
    to look at first**: it does not merely describe a C++ default, it makes an affirmative
    claim ABOUT the Blueprint, and that is the kind of sentence a name-table hit was supposed
    to be able to check.
  - **WHAT THE CONTENT LANE IS OWED DID NOT NARROW, AND THAT IS THE RESULT RATHER THAN AN
    OMISSION. RE-CHECKED 2026-09-03 ON THE THIRD RE-GATE'S F7.** The re-gate asked whether the
    decisive-ABSENCE direction lets any of the six handed-off asset sites be answered from a
    checkout with no editor. Re-measured this pass with `grep -aoF` over the raw bytes: **none of
    the six. All six read PRESENT** -- `AiSides` 1 on `BP_StratGameMode` (and on THAT asset
    presence does prove serialisation, but never the value, which is what the question asks);
    `FlagMarkerMesh` 1, `UnactedPipMesh` 1, and the Sec 2.11.2 markers'-art bullet's
    `FlagMarkerMaterial` 1 and `UnactedPipMaterial` 1 on `BP_StratUnit`; `BuildPulseMaterial` 1 on
    `BP_StratBoard`, covering both its header block and its `BeginPlay` arm. **So the handoff is
    unchanged at six sites and every one of them still needs an editor.** What the sharpened rule
    settled instead is four sites that were never on that list -- `MoveTweenEaseFraction`,
    `GuidedMarkerZOffset`, `FlagMarkerOffset` and `UnactedPipOffset`, recorded above. A reader who
    expected this list to shrink should read this bullet as the answer, not as a missing one.
  - **NOTHING ELSE IN THE SWEEP IS A DEFECT, AND THE PASSES ARE LISTED SO THE NEXT SWEEP DOES NOT
    RE-LITIGATE THEM.** The blocks that name a Blueprint default AS the shipping state get it
    right and are the pattern the repairs above move toward -- `StratBoardActor.h`'s
    `ObjectiveMaterial` and `TargetMaterial`, `StratUnitActor.h`'s `GuidedMarker`,
    `StratMatchSubsystem.h`'s `bRecordCompletionOnMatchEnd` and `SaveSlotName`, and
    `NoteMatchResultIfEnded`'s *"the shipped GameMode Blueprint's default says yes"*. So does
    `AStratPlayerController::HandleCancelSelectionEvent`'s struck sentence, which is this defect's
    own retraction and must keep the false wording it quotes.

- **2026-09-02, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED) -- THE EXISTENCE HOLD:
  A ROSTER DELTA FROM THE BRIDGE, TWO CURSOR-KEYED MAPS ON THE SUBSYSTEM, AND THE RULING
  `StratCombatLog.h` DEFERRED, MADE. THIS STEP LEAVES THE TREE DELIBERATELY UNCOMPILABLE AND THE
  DEBT IS NAMED BELOW.**
  - **STAMPED 2026-09-03, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master`
    in the main tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED). RECORD
    REPAIR ONLY -- NO `Source/` BYTE WAS CHANGED THIS PASS, so no build and no suite run is
    claimed by it and no count is written here. Raised as **F2** of
    `Tools/architect/gate_reports/2026-09-03-unit-movement-and-existence-hold.md` (`VERDICT:
    BLOCK`). TWO CLAIMS IN THIS ENTRY ARE NOW FALSE OF THIS TREE. BOTH ARE CORRECTED IN PLACE
    AND STAMPED ON THEIR OWN BULLETS RATHER THAN DELETED, because a reader arriving by a
    citation lands on the bullet and not on this header.**
    - **(1) THE NAMED DEBT IS DISCHARGED: THIS TREE COMPILES AND THE FIVE CLAUSES EXIST.** The
      four `Submit` sites were repaired inside `Tests/` by `strat-test-author`, the tree links,
      and every clause this entry hands over as unwritten is written and passing. MEASURED, NOT
      ASSUMED, AND WITHOUT A BUILD OF MY OWN: `Binaries/Win64/UnrealEditor-StratPlay.dll`
      (2026-09-03 15:46) and `UnrealEditor-StratBridge.dll` (14:35) are each NEWER than every
      source file of their own module (newest respectively `Tests/StratAiPlaybackClauses.cpp` at
      15:45 and `StratBridge.cpp` at 14:34), so a link ran after the last edit; and the exported
      report at `reportCreatedOn 2026.09.03-19.47.45` carries all five clause names with state
      `Success`. **A CLAUSE NAME IS COMPILED**, so a name's PRESENCE in that report is itself
      evidence the binary that ran carries these bytes, which is what makes the timestamp
      argument more than a coincidence of clocks. The live suite figure is `global.md`'s alone
      and is deliberately not restated here.
    - **(2) THE FIRST CLAUSE'S BRIEFED ONE-LINE MUTATION WAS MEASURED INERT.** It is stamped on
      its own bullet below, with the mutation that DOES pin that clause and the seam that would
      close the gap for good.
  - **A ROSTER DELTA AND NOT A COMBAT OUTCOME OR A BUILD OUTCOME, AND THE GENERALISATION WAS
    FORCED BY MY OWN TWO MEASUREMENTS RATHER THAN CHOSEN.** A Build command spawns ZERO OR MORE
    units and a death can be the ATTACKER, so neither "the unit this Build built" nor "the unit
    this Attack killed" is a well-formed question and `TargetId` names a LIVE unit in the common
    case. **Both halves are the same question asked of one accepted command -- which ids
    appeared, which departed.** One observation, no `Kind` switch, and no pair of parallel
    mechanisms to drift.
  - **THE BRIDGE: `StratRosterObservation`, BRACKETED AROUND THE SINGLE `applyCommand` CALL, AND
    `FStratBridge::RosterDeltaOfLastCommand` AS THE RETURN CHANNEL.** Shaped exactly like
    `RepairsAtTurnOpen` -- retain the last one, hand over a copy -- because that is this tree's
    existing precedent for delivering an observation rather than only logging it. Filled on the
    accepted path and **CLEARED on the refused one**, so the pair always describes the command
    that just returned; §4.9 says a refused command changes nothing, and leaving the previous
    delta readable would attribute an appearance to a rejected command.
  - **IT READS `strat::GameState` DIRECTLY AND NOT `MakeUiSnapshot`, WHICH IS A DEPARTURE FROM
    BOTH ITS NEIGHBOURS AND IS THE DECISION IN THAT FILE.** A `UiSnapshot` is a PROJECTION, and
    "did this id survive" must be asked of the authoritative roster -- a view that one day
    filtered a unit for a presentation reason would make that unit read as a DEAD one. It is also
    far cheaper, which is what makes the third thing true: **this observation brackets EVERY
    KIND**, unlike the combat and repair observations which are gated to one each. That is a cost
    and it is stated -- Move and Build previously paid nothing and now pay two roster reads -- and
    it is affordable only because the read is ids and nothing else: no HP, no hexes, no
    projection. Gating it by kind was the alternative and is refused, because the arm deciding
    which kinds can change a roster is exactly the assumption I measured false for Build.
  - **IT DOES NOT DUPLICATE THE COMBAT OUTCOME'S DEATH TEST AND DOES NOT REPLACE IT, AND THE
    DISTINCTION IS WRITTEN DOWN RATHER THAN LEFT TO BE NOTICED.** `FStratCombatOutcome` answers
    *"did the unit I NAMED die"*, role-keyed to an attacker and a defender identified before the
    submit, and `STRAT-COMBAT resolved` needs to say WHICH of the two it was. The delta answers
    *"what left the roster"*, keyed to no role. **The delta SUBSUMES the outcome's death facts as
    information -- an id in `Departed` is exactly a unit that died -- but not as USE**, because
    the log line's question cannot be asked of it. Two answers, one question each; neither was
    rewritten to serve the other, and no death test was duplicated.
  - **THE RULING WAS RECORDED IN `StratCombatLog.h`'S OWN BLOCK RATHER THAN THE BLOCK BEING
    DELETED.** That block's every sentence is still true OF `FStratCombatOutcome`, which still has
    no accessor and is still only emitted; the amendment says the deferred ruling was made, for a
    different and smaller payload, and points anyone widening the delta toward that struct's
    fields back at the paragraph that is still the argument against.
  - **THE PORT SHAPE: A STRUCT, `FStratAiCommandEffect`, REPLACING THE ROUTE OUT-PARAM. THIS IS
    THE SECOND WIDENING OF `IStratAiTurnPort::Submit` IN ONE DAY AND THAT IS WHAT DECIDED IT.**
    Two more out-params would have made five and broken the same three doubles again on the next
    fact; one struct breaks them ONCE MORE AND THEN NEVER AGAIN BY THIS AXIS. `Reset()` clears
    all three members in one verb, so an implementation cannot clear two of three and leave a
    caller command N's roster delta against command N+1 -- which a five-parameter form would have
    made easy and silent. `FStratAiPlaybackStep` keeps three FLAT fields; the struct is a
    transport for one hop and nothing holds one.
  - **THE HOIST, AND IT IS THE ONE PLACEMENT WITH NO ALTERNATIVE.** `RefreshPresentation` is where
    a killed unit's actor is DESTROYED and a built unit's actor is SPAWNED, so after it "which
    units did this hand-over create and destroy" is unanswerable. `BuildTourExistenceHolds` runs
    above it in `RunAiTurnsNow`, reading `UnitActors` as the pre-hand-over roster -- which it
    still is at that instant, and only at that instant.
  - **`WillAiPlaybackRun()` MAKES "GATE UP, NO TOUR" UNREACHABLE BY CONSTRUCTION, WHICH IS THE
    POINT OF FOLDING THE GUARDS AND NOT A TIDY-UP.** The hoist and `BeginAiPlayback` must agree
    on whether a tour will run; a second copy of three conditions is exactly how the gate gets
    raised for a tour that then declines, stranding every retained actor forever with no verb
    left to release it. One `const` function, two callers. **All three reasons are kept as three
    blocks and three `return false`s** -- folding them into one boolean would have merged three
    arguments into one anonymous `false`. **Only the shared consequence stayed behind**: the
    `RetireReel` lambda existed because three exits needed one line and there is one exit now, so
    it is one statement, with its `SkipToEnd`-not-`Reset` argument restated rather than deleted
    with it.
  - **THE DESTROY LOOP'S CONJUNCTION, AND BOTH TERMS GUARD AGAINST THE OTHER'S FAILURE.** The
    gate alone cannot say WHICH ids and would retain every unit that left the model for any
    reason, including a reseed. The membership test alone would let a map that outlived its tour
    strand an actor FOREVER -- nothing else in that function can tell a stale entry from a live
    one. It is a `continue` and not a hide: what is deferred is DESTRUCTION, because hiding there
    would make the unit vanish at the exact moment the hold exists to prevent.
  - **`ApplyTourExistenceAtCursor` RECOMPUTES AND NEVER APPLIES A DELTA**, which is this class's
    reconcile posture applied to existence. `bVisible = (Cursor > RevealAfter) && (Cursor <=
    HideAfter)` with permissive defaults -- `-1` for "always here", `MAX_int32` for "never
    leaves" -- so a unit in both maps, one, or neither falls out of one expression with no arm.
    A delta would have to remember what it last did, and the two cases with no incremental answer
    are exactly the ones that occur: a skip moves the cursor by any amount, and a unit can be
    both revealed and hidden inside one tour.
  - **ONE CORRECTION TO THE BRIEFED CALL SITES, AND IT WOULD HAVE SHIPPED THE BUG.** The brief
    said to call it "at the hoist and after each `Advance()`". **At the hoist a built unit has no
    actor yet** -- `RefreshPresentation` has not run, so the loop over `UnitActors` finds nothing
    to hide and every newly built unit would be visible for the whole tour. It is called from
    `BeginAiPlayback` instead, which is after that reconcile and before the first step is shown.
    **No frame is rendered in between**: `RefreshPresentation` and `BeginAiPlayback` are two
    statements of one synchronous call, so a unit that must not be seen yet is never drawn once.
  - **THE RELEASE DESTROYS IN `EndAiPlaybackTour` AND DOES NOT DEFER TO THE NEXT `ApplyView`,
    WHICH IS THE CHOICE THAT LOOKS OPTIONAL AND IS NOT.** Deferring is tempting -- the destroy
    loop would do it correctly once the gate dropped. But after a tour ends the next `ApplyView`
    may be a mouse crossing a hex boundary, a command, or NOTHING AT ALL; if the cursor never
    moves again a unit the rules module killed forty steps ago stands on the board for the rest
    of the match, which is worse than the defect being repaired. Ordered UNHIDE, then DESTROY,
    then DROP THE GATE -- unhiding unconditional so no id this function failed to think about is
    left invisible, destroying against the APPLIED MODEL rather than against `HideAfterStep` so
    it asks the same authority `ApplyView` asks, and the gate last so nobody sees a half-released
    hold. It lives here and nowhere else on this function's founding argument: no verb may stop a
    tour without clearing what the tour left behind.
  - **THE SCOPE LIMIT IS STRUCTURAL AND I VERIFIED IT RATHER THAN ACCEPTING IT.** A unit BUILT AND
    KILLED inside one hand-over is absent from the final model AND from the pre-hand-over roster,
    so no actor exists for it and none is created. **`AStratUnitActor` is spawned in exactly one
    place in this tree -- `ApplyView`'s spawn branch, confirmed by sweeping `Source/` for
    `SpawnActor<AStratUnitActor>` -- so there is no other route by which such an actor could
    exist and the coordinator's reasoning holds.** It cannot be shown being built or dying without
    a spawn path the model never asked for, and this pass does not invent one. **This is the
    common case and not a corner: three of the seven deaths in the hand-over I measured were
    units built during that same hand-over.** The two held sets are therefore: units in the FINAL
    model that appeared, and units that departed but WERE in the pre-hand-over roster.
    `BuildTourExistenceHolds` drops anything in both from both, explicitly, even though the
    `UnitActors` test already makes it unreachable -- so "in both is out of scope" is a line of
    code a reader can find rather than a property derived from two other conditions.
  - **AND A SECOND LIMIT, NAMED AND NOT FIXED: A RETAINED CORPSE STANDS WHERE THE LAST RECONCILE
    PUT IT.** Right for a human-vs-AI hand-over, where the unit never moved after the player last
    saw it; wrong for a unit that MOVED and then died in the same hand-over, whose corpse waits at
    the hex it started from rather than the one it walked to. Fixing it needs the dead unit's
    final hex, which no surviving model carries. Recorded at the destroy loop as well as here.
  - **THE PROSE. THE TWO THAT MATTER WERE HANDLED DIFFERENTLY FROM EACH OTHER, DELIBERATELY.**
    `StratAiPlayback.h`'s per-step-board rejection is AMENDED and NOT overturned: that rejection
    turned on needing A MID-TURN COPY OF THE RULES STATE, and every word of it stands --
    **what now rewinds is EXISTENCE, driven from a per-step list of ids recorded at the instant
    of acceptance, which that header's own opening already declares the reel to be. POSITIONS
    STILL DO NOT REWIND**, and the amendment says so in terms. The same header's *"any click or
    Esc skips to the end state is SATISFIED TRIVIALLY, because the end state was never absent"*
    is marked **GENUINELY WEAKENED and not re-explained**: with the hold in force the end state IS
    absent while a tour runs, and a skip now ACTIVELY destroys and unhides. What survives is that
    none of it touches the RULES state, which is why `T-TURN-09`'s state-equality half is
    unaffected while its screen-equality half now rests on a verb rather than on an absence.
    `ApplyView`'s "one place every model reaches the screen" SURVIVES AS WRITTEN and gained one
    sentence rather than a rewrite: it is a claim about MODELS and is unchanged; it was never a
    claim that nothing else touches an actor.
  - **THE TREE DOES NOT COMPILE AND THAT IS THIS STEP'S NAMED DEBT.** [**STAMPED 2026-09-03,
    `strat-gameplay-engineer`: DISCHARGED, AND EVERY BUILD FACT IN THIS BULLET IS HISTORY.** It
    described the tree AT THIS ENTRY. The tree today links and the five clauses run; the
    measurement is under this entry's header. **NOTHING IN THIS BULLET MAY BE CITED AS A LIVE
    BUILD STATUS.** The one sentence still worth reading is the enumeration of the four break
    sites, which the test lane closed exactly.] Build:
    `Result: Failed (OtherCompilationError)`. **EVERY ERROR IS IN `Tests/` AND THERE ARE ZERO
    OUTSIDE IT** -- measured, not assumed: a second incremental build recompiled only
    `StratAiPlaybackClauses.cpp` and `StratAiTurnRunnerClauses.cpp`, which means every other
    translation unit in `StratBridge` and `StratPlay` had already compiled clean and was up to
    date. **NO SUITE WAS RUN AND NO REPORT WAS DELETED**, because a suite over a tree that does
    not link measures nothing. The four break sites and their exact required forms are handed to
    `strat-test-author` in this lane's report; three are `Submit` overrides and the fourth is a
    direct call on `FStratBridgeAiTurnPort` that constructs a `TArray<FIntPoint>` local.
  - **WHAT THE TEST LANE IS OWED FOR THE HOLD, EACH WITH ITS ONE-LINE MUTATION. NOT WRITTEN AND
    NOT BUILT THIS PASS.** [**STAMPED 2026-09-03, `strat-gameplay-engineer`: ALL FIVE ARE NOW
    WRITTEN, BUILT AND GREEN** -- four in `Source/StratPlay/Tests/StratTourExistenceHoldClauses.cpp`
    and the fifth in `Source/StratBridge/Tests/StratRosterDeltaClauses.cpp`. **THIS LIST IS NOW A
    BRIEF THAT WAS ANSWERED, NOT WORK OUTSTANDING.** Four of the five reddened under exactly the
    mutation written below; the FIRST did not, and carries its own stamp.]
    - `T-TURN-09.AUnitBuiltDuringTheHandoverIsHiddenUntilItsStep` -- at a positive
      `AiPlaybackStepSeconds`, a unit whose id is in some step's `AppearedUnitIds` and IS in the
      final model is `IsHidden()` at cursor 0 and visible once the cursor passes that step.
      **MUTATION: in `UStratMatchSubsystem::BeginAiPlayback`, delete the
      `ApplyTourExistenceAtCursor();` call** -- the unit is then visible from the tour's start,
      which is the defect.
      - **STAMPED 2026-09-03, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on
        `master` in the main tree `E:/MultiAgent/Stratocracy`, base commit `283d711`,
        UNCOMMITTED): THE MUTATION DIRECTLY ABOVE IS INERT AND MUST NOT BE RUN AS THIS CLAUSE'S
        PIN. IT REDDENS NOTHING. A reader who runs it gets a GREEN suite and would conclude a
        live clause is inert -- an instruction sheet ordering a false finding, which is the
        failure `f079b9f` was committed to fix. It is stamped rather than deleted because the
        same mutation is quoted in other files and a reader who meets it there is owed the
        reason it fails.**
        - **WHY IT IS INERT, VERIFIED BY ME AGAINST THIS TREE BY READING THE CALL CHAIN, NOT
          TAKEN ON REPORT.** `UStratMatchSubsystem::BeginAiPlayback`'s last two statements are
          `ApplyTourExistenceAtCursor()` and then `OnAiPlaybackTimer()`; `OnAiPlaybackTimer`
          calls `AdvanceAiPlaybackOneStep()`, which calls `ApplyTourExistenceAtCursor()` AGAIN
          at cursor 1 -- all inside one synchronous call, before `BeginAiPlayback` returns. The
          function is a **pure function of the cursor** by its own declaration, so the second
          call does not merge with the first, it SUPERSEDES it; the deleted line's only effect
          lives in the window between the two, and that window renders no frame, as
          `BeginAiPlayback`'s own comment states and `RunAiTurnsNow`'s single synchronous call
          makes true. **No headless clause can reach it, so this is a property of the CODE and
          not a weakness of any fixture.**
        - **THE LINE IS NOT DEAD, AND THAT WAS MEASURED BY THE TEST LANE RATHER THAN ASSUMED BY
          ME.** `Tools/architect/state/tests.md` records that under a supplementary mutation
          unit 11 -- which appears at step index 0 and is therefore legitimately visible at
          cursor 1 -- becomes CONCEALED, which is exactly the cursor-0 hide the unmutated
          `AdvanceAiPlaybackOneStep` immediately undoes. **That is the test lane's run and I
          neither re-ran it nor claim it as mine**; what I verified here is the call chain that
          explains it. Deleting the call is therefore a real behaviour change that nothing in
          this tree can observe -- the sharpest possible form of an unpinnable line.
        - **THE MUTATION THAT DOES PIN THIS CLAUSE: delete `ApplyTourExistenceAtCursor();` from
          `UStratMatchSubsystem::AdvanceAiPlaybackOneStep`** -- the call the reveal half
          actually rests on. It reddens property (2) on all 19 appeared ids at once. Recorded by
          the lane that ran it in `Tools/architect/state/tests.md`, and in the clause's own
          header block in `Source/StratPlay/Tests/StratTourExistenceHoldClauses.cpp`. **Cite
          THAT one, never the one above.**
        - **THE SEAM THAT WOULD CLOSE IT, AND THE CONDITION THAT DISCHARGES THIS STAMP.** The
          clause can only reach the hold THROUGH ITS CONSEQUENCES -- an actor's hidden flag --
          because `bTourExistenceHeld`, `RevealAfterStep` and `HideAfterStep` are **private on
          `UStratMatchSubsystem` with no route from `Tests/`**, and the one moment worth reading
          them at is passed over synchronously. **DISCHARGED WHEN a `const` accessor on
          `UStratMatchSubsystem` reporting whether a given unit id is currently held concealed
          exists, AND a clause reads it at a point interleaved with `BeginAiPlayback`'s own
          apply** -- at which moment the mutation above stops being inert, becomes a real pin,
          and this stamp is retired rather than merely amended. The alternative seam the test
          lane names -- hoisting the existence apply to a point a caller can interleave with --
          is REFUSED here and the refusal is the decision: that call's placement is argued line
          by line in `BeginAiPlayback` (below all three guards, after the reconcile has spawned
          the actors, before `OnAiPlaybackTimer` shows step one), so moving it to suit a test
          would trade a declared gap for a live regression risk. The accessor adds a read and
          moves nothing.
        - **AND IT IS DELIBERATELY NOT WRITTEN IN THIS PASS, WHICH IS MY CALL AND NOT AN
          OVERSIGHT** -- the same call this file already made for `LayerFor`. It is a new public
          surface on a subsystem: it needs its own clause, its own reviewer gate and its own
          mutation, and landing an unpinned surface change inside a RECORD REPAIR over a tree
          under `VERDICT: BLOCK` is how an unmeasured change gets carried in on the back of a
          prose fix. **Until it lands, the gap stands DECLARED at four sites** -- here, the
          clause's own header, `tests.md`, and `global.md` -- **and no one may write that this
          clause pins the `BeginAiPlayback` call.**
    - `T-TURN-09.AUnitKilledDuringTheHandoverSurvivesUntilItsStep` -- an id in a step's
      `DepartedUnitIds` that WAS in the pre-hand-over roster still has an actor after
      `RefreshPresentation`, and no longer has one after the tour ends. **MUTATION: in
      `ApplyView`'s destroy loop, delete the `bTourExistenceHeld && HideAfterStep.Contains(...)`
      continue** -- the actor is destroyed at the reconcile, as it is today.
    - `T-TURN-09.EndingATourDestroysEveryRetainedCorpse` -- after `SkipAiPlayback`, no actor
      exists for any id the applied model does not carry. **MUTATION: in `EndAiPlaybackTour`,
      delete the destroy loop inside the `bTourExistenceHeld` block and keep the unhide** -- a
      corpse then stands on the board with the gate down and nothing left to remove it. **This is
      the sharpest of the four**, because the deferred alternative it rules out is the one a
      future reader would reach for.
    - `T-TURN-09.TheGateIsNeverRaisedWithoutATour` -- with `AiPlaybackStepSeconds` at the C++
      FIELD default **[CORRECTED 2026-09-03, `strat-gameplay-engineer` (ACTING and WRITING; IN
      LANE), base commit `283d711`, UNCOMMITTED: THIS BULLET READ "at the shipped default" AND
      THAT WAS FALSE OF THIS TREE. `AiPlaybackStepSeconds` IS SERIALISED AS AN OVERRIDE IN BOTH
      SHIPPED GameMode BLUEPRINTS -- measured this pass with `grep -aF` over the raw bytes of
      `BP_StratGameMode.uasset` and `BP_StratGameMode_AiVsAi.uasset`, with `AiSides` as the
      positive control, `MoveTweenSeconds` as the mirror-image negative and a fabricated name as
      the absent control -- so the fixture's zero is the C++ FIELD default and a path the shipped
      game never takes, exactly as the clause's own header in
      `Source/StratPlay/Tests/StratTourExistenceHoldClauses.cpp` now says in capitals. Raised as
      **F4** of `Tools/architect/gate_reports/2026-09-03-unit-movement-and-existence-hold-regate.md`
      (`VERDICT: BLOCK`). THE MUTATION BELOW IS UNCHANGED AND STILL PINS THE CLAUSE; only the
      configuration was misnamed.]**, `RunAiTurnsNow` leaves no actor retained and no actor
      hidden. **MUTATION: in
      `RunAiTurnsNow`, replace `if (WillAiPlaybackRun())` with `if (true)`** -- the gate goes up
      with no tour to release it and the first killed unit is retained forever.
    - AND ONE ON THE BRIDGE: `T-INT-05.ARefusedCommandLeavesNoRosterDelta` --
      `RosterDeltaOfLastCommand` is empty after a refused submit that follows an accepted one
      that was not. **MUTATION: in `FStratBridge::Submit`, delete the two `clear()` calls on the
      `!R.ok` path** -- the previous command's delta is then readable as the refused command's.

- **2026-09-02, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED) -- MEASUREMENT ONLY, NO
  CODE CHANGED: WHAT AN AI-vs-AI HAND-OVER ACTUALLY CONTAINS, AND THREE RULES FINDINGS VERIFIED
  AT THE SOURCE. ONE OF THEM IS STRONGER THAN CLAIMED AND A FOURTH BREAKS A DESIGN PREMISE.**
  - **NO PROBE WAS NEEDED AND NONE WAS WRITTEN, WHICH IS THE FIRST RESULT.** The brief expected a
    temporary log line. Both facts are ALREADY EMITTED by shipped code:
    `FStratAiTurnRunner::RunTurn` emits `STRAT-AI applied kind=...` for every accepted command,
    and `StratCombatObservation::EmitResolved` emits `STRAT-COMBAT resolved ... adied=N ddied=M`
    with both ids for every applied Attack. So the measurement is a `grep` over a log the suite
    already wrote. **NO SOURCE FILE IN ANY LANE WAS EDITED THIS PASS, IN ANY MODULE** -- there is
    nothing to prove byte-identical because nothing was mutated, no probe was added and none had
    to be removed, and `git status --porcelain` is the same set that stood before it. This record
    file is the only thing this pass writes.
  - **THE INSTRUMENT, AND IT WAS RUN TWICE BY TWO ROUTES THAT AGREE EXACTLY.** First by slicing
    `Saved/Logs/Stratocracy-backup-2026.09.02-23.41.07.log` -- the full-suite run this lane
    recorded earlier the same day -- between the automation controller's own `Test Started` /
    `Test Completed` markers, so each hand-over is counted separately rather than as a suite-wide
    aggregate. **That distinction is load-bearing:** the suite-wide `STRAT-AI applied` total is
    2,682 across 28 fixtures, and quoting it as "a hand-over" would have overstated every figure
    below by more than an order of magnitude. Then again by a PURPOSE-MADE headless run of
    `Stratocracy.StratPlay.GATE-AITURN.StepFocusesAndStopsOnTheLast` alone, once the editor was
    closed and `Saved/Logs/Stratocracy.log` was no longer locked. **Every figure below is
    identical between the two**, which is what makes them a property of the hand-over rather than
    of a particular run.
  - **THE STEP-KIND HISTOGRAM OF ONE REAL HAND-OVER ON THE SHIPPED SCENARIO** (`MakeAiVsAiConfig`,
    Ferrum Crossing), from `GATE-AITURN.StepFocusesAndStopsOnTheLast` and reproduced
    STEP-FOR-STEP IDENTICALLY by nine other clauses, which is itself worth knowing because it
    means the hand-over is deterministic and a clause may rely on these counts:
    **Attack 68, Move 55, Build 22, EndTurn 11 -- 156 steps**, matching the 156 the test lane
    reported.
  - **QUESTION 1 -- DOES A BUILD STEP APPEAR? YES, DECISIVELY. 22 OF THEM PER HAND-OVER.** The
    build half of the existence hold is NOT blocked for want of a reachable step and does not
    need to be held. Builds also appear in the small fixtures (1-5 each), so a cheaper arrangement
    than a full §2.8 game exists if a clause wants one.
  - **QUESTION 2 -- DOES A HAND-OVER PRODUCE A KILL, AND IS A COUNTER-KILL REACHABLE? A KILL YES,
    A COUNTER-KILL NOT IN ANY HAND-OVER IN THIS SUITE.**
    - In the 156-step hand-over: **68 Attack steps, 7 deaths, ALL DEFENDERS, 0 counter-kills, and
      61 attacks killed nobody at all.** So **an Attack step implies a death about one time in
      ten**, measured, not assumed -- which is the empirical form of finding 1 below.
    - Suite-wide: 1,682 resolved attacks, 173 defender deaths, **2 attacker deaths**. Both
      attacker deaths are in ONE fixture and it is not an AI-vs-AI game --
      `Stratocracy.StratBridge.T-UI-01.CounterKillLeavesTheAttackerOffTheRoster`, a purpose-built
      arrangement (`attacker=5 defender=10 ahpBefore=1 ahpAfter=-1 dhpBefore=11 dhpAfter=10` --
      a 1-hp attacker dying to the counter of a defender that lost a single point).
    - **SO THE COUNTER-KILL IS REACHABLE AND ALREADY HAS A FIXTURE PRECEDENT, BUT IS NOT
      ARRANGEABLE BY PLAYING THE SHIPPED SCENARIO.** A clause over attacker-death existence must
      hand-arrange it on that clause's model, not wait for an AI game to produce one.
    - **WHICH UNIT DIED IN EACH OF THE SEVEN, SINCE THE BRIEF ASKED FOR IDENTITIES AND NOT ONLY A
      COUNT.** Read off `STRAT-COMBAT resolved`, `turn` in brackets: defender 5 (t2), 2 (t3),
      10 (t4), 15 (t5), 20 (t5), 18 (t6), 1 (t6). Every one is `adied=0 ddied=1` -- the attacker
      survived all seven.
    - **AND THE SINGLE MOST CONSEQUENTIAL THING IN THIS MEASUREMENT FOR THE DESIGN, WHICH NOTHING
      IN THE BRIEF ANTICIPATED: THREE OF THE SEVEN DEAD WERE BUILT IN THE SAME HAND-OVER.** The
      shipped scenario fields 10 units, ids 1-10, and `g.nextUnitId++` hands out ids at spawn; the
      hand-over reaches id 30. Dead ids 15, 20 and 18 are therefore units this same reel BUILT and
      then KILLED. **So the two halves of the existence hold are not disjoint sets** -- a unit can
      need hiding until its Build step AND retaining until its Attack step, in that order, within
      one tour. A design that treats "built" and "killed" as separate populations will meet a unit
      in both.
  - **FINDING 1 -- CONFIRMED AT THE SOURCE.** `Replay.good.cpp`'s Attack arm:
    `const int defHpAfter = d->hp - damage; const bool defenderDies = defHpAfter <= 0;` and on the
    else path `mutableUnit(g, defId)->hp = defHpAfter;` -- the defender stays on the roster with
    reduced hp. **An Attack step does not imply a death**, and the histogram above puts a number
    on it.
  - **FINDING 2 -- CONFIRMED AT THE SOURCE.** The counter block erases the ATTACKER when
    `atkHp - counter <= 0`. **A death can be the attacker**, so
    `FStratAiPlaybackStep::TargetId` -- documented caption-only -- names the surviving unit in
    that case and a LIVE unit in the common case. **It must not be used as a death identity.**
    Agreed without reservation.
  - **FINDING 3 -- CONFIRMED AND STRONGER THAN CLAIMED, WHICH HELPS THE DESIGN.** `applyCommand`
    contains exactly TWO `g.units.erase` calls and both are in the Attack arm -- verified by
    sweeping every `erase` / `remove_if` / `clear` / `pop_back` / `resize` over a unit container
    in all of `Source/StratRules/`. **AND THEY ARE MUTUALLY EXCLUSIVE:** the defender-death branch
    ends `return okResult(1);` before the counter block is reached, so **an Attack step erases 0
    or 1 units and NEVER 2.** The design's "0-or-1 per Attack step" premise is not merely
    unfalsified, it is structural.
    **TWO NEAR-MISSES THAT WOULD HAVE LOOKED LIKE A THIRD ERASURE, NAMED SO THE SWEEP IS
    REPRODUCIBLE.** `Driver.good.cpp` holds three `s.units.erase` calls -- they operate on
    `strat::Session`'s `DriverUnit` vector, the DEBUG DRIVER's own state, a different type on a
    different code path that `FStratBridge::Submit` never enters. This is the identical
    conflation `.agents/ue-project-context.md` already warns about for `stateHash`, hit a second
    time on a different symbol. And `Economy.good.cpp`'s erase is on `s.captures`, not units.
  - **FINDING 4, WHICH NOBODY ASKED FOR AND WHICH BREAKS A PREMISE OF THE BUILD HALF.** The Build
    arm does not spawn the unit it was given. It calls `queueBuild`, then
    `resolveBuilds(g.economy, g.bounds, occupied)` and pushes EVERY spawn that comes back:
    `for (const SpawnResult& sp : spawns) { if (!sp.spawned) continue; /* boxed in: waits, holds
    the slot */ ... g.units.push_back(u); }`. **So ONE Build command spawns ZERO OR MORE units,
    not exactly one** -- zero when the factory is boxed in, and more than one when a build queued
    earlier and blocked becomes placeable on a later Build command by that side. A hold that maps
    "this Build step" to "this one unit" is therefore wrong in both directions: it would reveal a
    unit at a step that did not create it, and hold a unit that no step will ever release.
    **THE COMPENSATING GOOD NEWS, MEASURED THE SAME WAY:** `resolveBuilds` has exactly ONE call
    site in `applyCommand` -- the Build arm -- and is NOT called from `openTurn`. So across the
    whole of `applyCommand`, **units appear only on Build commands and disappear only on Attack
    commands**, and no unit ever appears or vanishes at a turn boundary or on a Move, Capture or
    EndTurn. The design's shape survives; its 1:1 mapping does not.
  - **THE IDENTITY SOURCE EXISTS AND IS COMPUTED AS DESCRIBED -- CONFIRMED FIELD BY FIELD.**
    `FStratBridge::Submit` brackets `StratCombatObservation::CaptureBefore` and `CaptureAfter`
    around the single `strat::applyCommand` call and then calls `EmitResolved`.
    `FStratCombatOutcome` carries `AttackerId`, `DefenderId`, `bAttackerDied` and `bDefenderDied`.
    The death test is stated in the code and is the right one: *"ABSENCE FROM THE ROSTER IS THE
    DEATH TEST, and it is the only one available: the snapshot carries no 'is dead' field because
    a dead unit is not projected."*
    **AND THE DISCARD IS DELIBERATE AND ALREADY WRITTEN DOWN**, under `StratCombatLog.h`'s "WHAT
    IS DELIBERATELY NOT IN IT": *"No accessor on `FStratBridge`. The bridge assembles one of these
    per attack and emits it; it does not retain the last one. A caller that needs to READ an
    outcome needs a delivery mechanism (an event list, a return channel) that is a separate
    ruling ... This struct is the payload such a mechanism would carry, landed early because
    assembling it is the hard part."* So the design's premise -- that the identity is already
    measured and merely discarded -- is exactly right, and the header anticipated this ask.
  - **AN EDITOR IS RUNNING AND IT DID NOT BLOCK THIS PASS, WHICH IS WORTH SEPARATING.**
    `UnrealEditor` PID 44100 is live and holds `Saved/Logs/Stratocracy.log` -- a `rm` on it
    returned *"Device or resource busy"*, and a fresh headless run was diverted to
    `Stratocracy_2.log`. **No build was attempted and none was needed**: no source changed, so the
    binary from `reportCreatedOn 2026.09.02-23.41.07` is current. A build WOULD have been refused.
  - **WHAT THIS DESCOPES, STATED AS THE ANSWER TO THE QUESTION ASKED RATHER THAN AS ADVICE.** The
    build half is reachable and should not be held. The defender-death half is reachable from a
    played hand-over. The attacker-death half is reachable only from a hand-arranged fixture, and
    a precedent for one already exists. And the build half needs a different mapping than
    per-step, because a Build command is not a unit.

- **2026-09-02, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED) -- THE PRE-PARK:
  `AStratUnitActor::ParkPictureAt` AND `UStratMatchSubsystem::PreParkPicturesForTour`, FOR A
  DEFECT A HUMAN SAW IN PIE THAT NO INSTRUMENT IN THIS TREE COULD. AND A SECOND, LATENT DEFECT
  THE FIX EXPOSED: `SetRelativeLocation` DOES NOT STORE WHAT IT IS HANDED.**
  - **THE DEFECT, IN THE USER'S OWN WORDS AND NOT PARAPHRASED.** *"Before the interp movement
    activates, the AI units all snap to their target destination and then each one teleports back
    to their original position and then interps to the target location."*
  - **THE DIAGNOSIS WAS VERIFIED AT THE SOURCE AND IS CORRECT.** `RunAiTurnsNow` calls
    `RefreshPresentation` and then `BeginAiPlayback` -- verified by reading, not accepted -- so
    every unit's picture is drawn at its FINAL hex the instant the hand-over resolves, and each
    step's `PlayMoveSlideForStep` then displaces that picture BACK to its leg's start. Both
    halves of what the user described are that one ordering: a mass snap to the destinations,
    then a rubber-band per unit. **THE ORDERING WAS NOT CHANGED AND MUST NOT BE.** It is
    load-bearing, `RunAiTurnsNow`'s own block argues it, and the whole design rests on the tour
    stepping over a finished board. The model stays final.
  - **THE FIX IS A PARK WITH NO CLOCK, AND IT REUSED THE ONE RETIREMENT PATH RATHER THAN ADDING
    A SECOND.** `AStratUnitActor::ParkPictureAt` sets `TweenRestOffset` from two tile-plane
    points and calls `FinishTween` -- because "empty both arrays, put `Body` at the rest offset,
    zero the clock, tick off" IS a park with nothing running. A whole feature landed without one
    line of retirement logic being written a second time; `FinishTween` is now reached seven ways
    and its block says so.
  - **THE ANCHOR IS THE CALLER'S ON BOTH ENTRY POINTS, AND ONE SCAN NOW ANSWERS FOR BOTH.**
    `ParkPictureAt` takes a park point and an anchor from the same source, exactly as
    `PlayRouteSlide` does, so `BodyZOffset` cancels and no third copy of that expression exists.
    **AND `UStratMatchSubsystem::FindAppliedUnitHex` WAS EXTRACTED SO THE PARK AND THE SLIDE
    CANNOT DISAGREE**: `PreParkPicturesForTour` and `PlayMoveSlideForStep` both ask it rather
    than each looping over `AppliedModel.Units`. A park measured against one hex and a slide
    measured against another is a jump at the moment the step is shown -- which is the defect
    being repaired, in miniature, reintroduced by a copied loop.
  - **THE REEL GAINED A CURSOR-BLIND READER.** `FStratAiPlaybackReel::StepAt` -- `Peek`'s
    expression with the caller's index. `Peek` answers about the cursor, which is the right
    question for stepping and the wrong one for a reader that must see the whole reel before the
    tour starts. It cannot move the cursor, so there is still exactly one driver of the reel.
  - **PLACEMENT: BELOW ALL THREE OF `BeginAiPlayback`'S GUARDS AND ABOVE `OnAiPlaybackTimer()`.**
    Below, so the shipped-inert path (`AiPlaybackStepSeconds <= 0`), the empty reel and the
    no-world case park nothing -- every automation fixture is on the first of those. Above,
    because `OnAiPlaybackTimer` shows step one and a park applied after it would wipe the slide
    that step had just armed. `ParkPictureAt` refuses a second time on `MoveTweenSeconds <= 0`,
    so the inertness does not rest on the call site alone.
  - **THE FOUR THINGS I WAS ASKED TO CONFIRM. ALL FOUR HOLD, AND (2) WAS CHECKED IN THE EXACT
    FORM ASKED RATHER THAN IN THE FORM THAT WOULD HAVE PASSED EASILY.**
    (1) **A repeat mover needs no jump between legs.** Leg N+1's route `[0]` is the hex leg N
    ended on, and `PlayRouteSlide` measures waypoint 0 from that same hex against the same
    anchor, so the two values are the same expression over the same hex and agree to the bit.
    Only the first Move per unit is parked, for that reason.
    (2) **A hover cannot eat a pre-park, INCLUDING A PARK WITH NO TWEEN ARMED, which is the case
    that needed checking.** A pre-parked unit has an EMPTY polyline, so the park-clear branch's
    first term (`TweenOffsets.Num() > 0`) is false and the second (`!TweenRestOffset.IsZero()`)
    is true -- the branch therefore turns entirely on `bActorMoved`. A hover-driven `ApplyView`
    applies the same model, so `SetActorLocation` writes the value `PreviousActorLocation`
    already held and `bActorMoved` is false. **The branch is not taken and the park survives.**
    That second term is exactly what makes a pre-park eligible for a clear when the actor really
    moves, and `bActorMoved` is exactly what protects it from a mouse.
    (3) **`EndAiPlaybackTour`'s cancel already covers a pre-park**, and this was worth checking
    rather than assuming because a pre-parked picture is a state that did not exist when that
    loop was written: it has an empty polyline and a non-zero rest offset, and
    `CancelRouteSlide` zeroes the offset BEFORE retiring, so it comes home on the same call as a
    picture caught mid-slide. Recorded at the loop.
    (4) **A unit parked and never stepped is covered by (3)** -- it is the same state, and a tour
    skipped in its first second strands nothing.
  - **THE SECOND VISIBLE DEFECT I WAS ASKED ABOUT: YES, AND IT IS BROADER THAN THE QUESTION.**
    A unit BUILT during the hand-over exists in the final model, has no Move step, and is
    therefore not pre-parked -- so **it pops into existence when the hand-over resolves rather
    than at its own Build step**, and the user will see that. It is not fixed here and it is not
    a gap in the pre-park: the pre-park moves PICTURES and this is about EXISTENCE, which is a
    different mechanism. **AND THE SAME SENTENCE APPLIES IN REVERSE AND IS NOT IN THE QUESTION:**
    a unit KILLED during the hand-over VANISHES when it resolves rather than at the Attack step
    that killed it, because `ApplyView`'s destroy loop has already run. Both are the honest cost
    of a tour that steps over a finished board, both are one family, and a fix for either is a
    visibility question -- hide until built, keep a corpse until killed -- not a park.
  - **AND THEN THE FIX EXPOSED A LATENT DEFECT IN A CLAIM THIS FILE HAS MADE SINCE THE TWEEN
    LANDED. THIS IS THE MOST USEFUL THING IN THE PASS.** The first full run after the pre-park
    came back RED on `T-TURN-09.SkipLeavesNoPictureStranded` alone: *"unit 13's picture is home
    at EXACT relative zero ... it is at X=0.000 Y=-0.000 Z=0.000"*. Identical bytes were re-run
    once before diagnosing -- reproducible, not a flake -- and then MEASURED with a temporary
    probe in `ParkPictureAt` and `CancelRouteSlide` rather than reasoned about.
    **THE PROBE REFUTED THE OBVIOUS EXPLANATIONS.** `CancelRouteSlide` DID run on unit 13; the
    unit WAS in `UnitActors`; nothing wrote after it. What the probe showed is that **the same
    line, in the same call, asking for `FVector::ZeroVector`, stored an exact zero for unit 4 and
    `Y=-0.000` for unit 13** -- differing only in where their actors stood on the board.
    **THE CAUSE: `USceneComponent::SetRelativeLocation` IS A WORLD-SPACE MOVE.** It converts the
    requested relative location to world, calls `MoveComponent`, and converts the result BACK
    through the parent's inverse -- so it stores a ROUND TRIP, not the value it was handed. The
    residue is ~1e-14 uu on a 100 uu tile, invisible on a screen, **and fully visible to
    `IsZero()`, which is what two clauses assert and what this file's own "written rather than
    approached" sentence promises.**
    **THIS PREDATES THE PRE-PARK AND WAS UNOBSERVABLE UNTIL IT.** Before, only one picture per
    tour was ever displaced and returned; every other unit's `Body` still held the exact zero its
    construction gave it, so no clause could see the round trip. The pre-park displaces and
    returns EVERY moving unit, which is what made a latent inexactness reddening.
    **THE FIX MAKES THE CLAIM TRUE RATHER THAN MAKING THE CLAUSE LOOSER, AND THAT CHOICE IS THE
    DECISION.** `FinishTween` now writes through `SetRelativeLocation_Direct` plus
    `UpdateComponentToWorld`, which computes the WORLD transform FROM the relative one -- the
    correct direction, no inverse, no round trip -- so the stored value is the value asked for by
    construction. **The alternative was to ask `strat-test-author` to soften two `IsZero()`
    assertions to a tolerance, and it was refused:** this file's prose says a sub-pixel residue
    is state by any honest reading of "no state the model holds", and a clause weakened to fit my
    code is the wrong direction of repair. It is safe on this component for a reason specific to
    it and stated rather than assumed -- `Body` carries NO COLLISION, set three ways in the
    constructor and load-bearing for the cursor, so there is no sweep, overlap or physics state
    the move path was responsible for.
  - **ONE EARLIER MEASUREMENT OF MINE IS NOW FALSE AND IS RETRACTED FLAT AT THE LINE.**
    `PlayRouteSlide`'s waypoint-0 block recorded *"on a unit's FIRST slide of a tour
    `Drawn - GetActorLocation()` is ZERO"* -- true when written, and the reason the pre-park had
    to exist. **It is no longer true of any production path**: a pre-parked unit's drawn offset
    equals `RouteWorldPoints[0] - AnchorWorldPoint`, so the `bSlideInFlight` guard now selects
    between two identical values everywhere in the running game.
    **THE GUARD STAYS, AND I CHECKED WHETHER THAT MADE A CLAUSE INERT RATHER THAN ASSUMING IT
    HAD NOT.** `T-INT-05.AFirstRouteSlideArmsANonDegeneratePolyline` calls `PlayRouteSlide`
    DIRECTLY on a picture at rest that nothing pre-parks (`ArrangeAtDestination`), so its named
    mutation still collapses the polyline to {0, 0} and still reddens it. The clause is intact;
    what changed is that the collapse is no longer reachable from a tour, and the clause is now
    the only thing that keeps it reachable at all.
  - **MEASURED.** Build: `Build.bat StratocracyEditor Win64 Development` at the full engine path,
    `Result: Succeeded`, no warning on any edited translation unit; no editor was open and UBT
    did not refuse. Suite: `Saved/AutomationReport/index.json` deleted before every run with the
    destroyed stamp recorded -- the run this entry vouches for began by deleting
    `2026.09.02-23.38.57` -- and the EXPORTED report read rather than the log. **The tree left
    behind is stamped `reportCreatedOn 2026.09.02-23.41.07` with ZERO non-success entries of any
    kind.** The control held: `Stratocracy.StratPlay.T-TURN-09.PlaybackDispositionsAgreeOnState`
    reports `Success`, as do all six of the route-slide clauses. Neither a count nor a verdict is
    written here and neither may be: `global.md` is the only file that may carry either. **No
    file under any `Tests/` directory was edited by this lane**; the temporary probes were in
    `StratUnitActor.cpp`, this lane's own file, and were removed by restoring a copy taken before
    they were added.
  - **WHAT THE TEST LANE IS OWED FOR THE PRE-PARK, NAMED WITH ITS MUTATION AND NOT BUILT.**
    `Stratocracy.StratPlay.T-TURN-09.TheTourParksEveryMoverBeforeItsFirstStep` -- arrange a tour
    at a positive `AiPlaybackStepSeconds` and a positive `MoveTweenSeconds`, and assert that
    **immediately after `BeginAiPlayback` has shown step one**, a unit whose first Move comes
    LATER in the reel is already displaced, and displaced specifically to
    `WorldLocationOfHex(its first Move's RouteHexes[0]) - WorldLocationOfHex(its hex in the
    applied model)`. The later-mover is what makes it a claim about the PRE-PARK and not about
    step one's own slide. **THE MUTATION THAT MUST REDDEN IT:** in
    `UStratMatchSubsystem::BeginAiPlayback`, delete the `PreParkPicturesForTour();` call -- one
    line, and the tour then runs exactly as it did when the user reported the defect.
    **A SECOND, SHARPER MUTATION THE SAME CLAUSE SHOULD CATCH:** in `PreParkPicturesForTour`,
    delete the `Parked.Contains(Step->UnitId)` guard, so a repeat mover parks at its LAST move's
    start instead of its first -- which is the defect the user saw, reduced to one unit and
    otherwise invisible. **A THIRD, FOR A DIFFERENT CLAUSE ENTIRELY:** move
    `PreParkPicturesForTour();` to AFTER `OnAiPlaybackTimer();`, which wipes the first step's
    armed slide -- I do not know that any clause would notice, and saying so is more useful than
    guessing.

- **2026-09-02, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED) -- FALSIFIABILITY OF THE
  SIX ROUTE-SLIDE CLAUSES, MEASURED BY MUTATING MY OWN FILES. ALL SIX ARE PROVEN ABLE TO FAIL.
  AND TWO GAPS ARE RECORDED AS UNPINNABLE RATHER THAN OWED.**
  - **WHY THIS LANE RAN IT, ON THE SAME ARGUMENT AS THE 2026-09-02 ROUTE-TWEEN PASS.**
    `strat-test-author` wrote all six clauses and named each one's exact one-line mutation, and
    every one of those mutations edits `AStratUnitActor::PlayRouteSlide`,
    `AStratUnitActor::ApplyUnitView` or `UStratMatchSubsystem::EndAiPlaybackTour` -- this lane's
    files. A green suite over a clause never shown able to fail pins nothing.
  - **THE MUTATION WAS READ OFF EACH CLAUSE'S OWN BANNER AND NOT OFF THE DISPATCH.** Every one of
    the six named its file, its function and its one line, and **every one was correct and
    buildable as written** -- no banner needed reinterpreting, which is worth recording because
    two restatements from the dispatch seat caused real defects earlier the same day.
  - **METHOD, AND THE ONE NEW TRAP IT HIT.** Mutations applied IN PLACE (a copied tree reuses
    `Intermediate/Build` and reports the old binary green); rebuilt before every run;
    `Saved/AutomationReport/index.json` deleted before each run with the destroyed stamp recorded;
    the EXPORTED report read rather than the log; every patch refused on an anchor count other
    than 1; every revert proved by `git hash-object` AND `md5sum` against baselines captured
    before the first mutation -- `StratUnitActor.cpp 0185e505` is superseded, the baselines for
    this pass were `StratUnitActor.cpp c47e8a23` / md5 `fb7e30ed` and `StratMatchSubsystem.cpp
    582514c3` / md5 `9e41da26`, and both files hashed byte-identical after every one of the six
    reverts.
    **THE NEW TRAP: THE FIRST PATCH REFUSED WITH ANCHOR COUNT 0 ON A LINE THAT WAS VISIBLY
    THERE.** `Source/StratPlay/StratUnitActor.cpp` is **CRLF** while the anchor text was LF, and
    `grep` on this box cannot see a CR -- `cat -A` through `grep` showed a bare `$`, so the
    instrument that would have reported it is the one that is blind to it. Diagnosed by printing
    `repr()` of the matched line in Python. The patch tool now detects the ending from the
    TARGET's own bytes per file, because this tree's `.cpp` and `.h` files disagree with each
    other. **The refusal is the right failure mode and it is worth saying so: a line-ending
    mismatch presents as "anchor count 0", not as a corrupted file.**
  - **PER MUTANT, WITH THE FULL NON-SUCCESS LIST EACH TIME. FOUR OF THE SIX REDDEN EXACTLY ONE
    CLAUSE.**
    - **M1, the park destroyed** -- `PlayRouteSlide`'s `TweenOffsets.Add(Point -
      AnchorWorldPoint)` to `Point - RouteWorldPoints.Last()`. **ONE RED:**
      `T-INT-05.RouteSlideParksOverTheRoutesLastHex`, *"the terminal waypoint is NOT zero -- it
      is X=0.000 Y=0.000 Z=0.000, a park of 0.0 uu on a 100.0 uu tile"*. Report
      `2026.09.02-23.00.35`.
    - **M2, the waypoint-0 guard deleted** -- `PlayRouteSlide`'s `if (bSlideInFlight)` made
      unconditional, which is the shape the dispatch brief specified and this lane refused.
      **FOUR RED, AND THAT IS ENTANGLEMENT AND NOT FOUR DETECTIONS.** Only
      `T-INT-05.AFirstRouteSlideArmsANonDegeneratePolyline` failed on its own subject --
      *"a one-hex move from a picture at rest arms a slide with a positive duration (0.000 s)"*
      and *"the polyline has both of its waypoints"* to be 2, but it was 0. The other three --
      `AnActorThatMovedWithNoRouteClearsItsPark`, `AParkedPictureSurvivesAnApplyWithIdentical
      Positions` and `T-TURN-09.SkipLeavesNoPictureStranded` -- all failed on their ARRANGE
      GUARDS (*"the picture is genuinely displaced by it"*, *"some step of the tour parked a
      picture (156 of 156 steps shown)"*), never reaching their own subjects. The cause is one
      thing: the mutation collapses EVERY slide, so no park exists anywhere for any fixture to
      arrange. Report `2026.09.02-23.02.29`.
      **THIS IS ALSO THE STRONGEST AVAILABLE CONFIRMATION THAT REFUSING THE BRIEF'S SHAPE WAS
      RIGHT.** The mutant IS the briefed implementation, and under it the feature does nothing on
      a one-hex move and no park is created anywhere in a 156-step hand-over.
    - **M3, the forbidden anchor derivation** -- `Point - AnchorWorldPoint` to `Point -
      GetActorLocation()`. **ONE RED, AND THE PAIRING THE TEST LANE ASKED FOR HOLDS:**
      `T-INT-05.RouteSlideWaypointsSitAtTheDrawnUnitsHeight` **ALONE**, on all four waypoints --
      *"waypoint 1 draws at the unit's own height over hex (-1, 4): Z 0.00, expected 37.00 (plane
      0.00 + lift 37.00)"*. Clauses 6 and 7 stayed GREEN under it, which is what proves the XY
      clauses and the Z clause are not redundant. Report `2026.09.02-23.04.22`.
    - **M4, `bActorMoved &&` dropped from the park-clear branch.** **TWO RED.**
      `T-INT-05.AParkedPictureSurvivesAnApplyWithIdenticalPositions` on its own subject -- *"the
      park survives an apply with identical positions -- the waypoint count is untouched" to be
      3, but it was 0"* -- and the pre-existing
      `T-INT-05.SecondApplyWithIdenticalPositionsLeavesTheTweenAlone`, *"to be 4, but it was 0"*.
      **THAT SECOND RED IS ONE MECHANISM SEEN TWICE, NOT A SECOND DETECTION**, and it is
      informative rather than a defect: an unguarded park-clear branch does not merely pop parked
      AI pictures, it also destroys `ApplyUnitView`'s OWN tween on the second of the two applies a
      player move produces -- which the older clause was already built to catch. Report
      `2026.09.02-23.06.08`.
    - **M5, the park-clear branch's `CancelRouteSlide();` deleted.** **ONE RED:**
      `T-INT-05.AnActorThatMovedWithNoRouteClearsItsPark`, *"the park is retired outright -- no
      polyline is left behind with no clock" to be 0, but it was 3"* and *"the picture is home in
      the same call, at EXACT relative zero -- it is at X=-250.000 Y=86.603 Z=0.000"*. Report
      `2026.09.02-23.07.59`.
      **SO THE SECOND PAIRING HOLDS TOO, IN BOTH DIRECTIONS.** M4 reddens clause 9 and leaves
      clause 10 green; M5 reddens clause 10 and leaves clause 9 green. The two clauses pin the two
      halves of one `else if` -- its CONDITION and its BODY -- and each half has a mutation the
      other clause cannot see. They are testing separate things.
    - **M6, the cancel loop deleted from `EndAiPlaybackTour`.** **ONE RED:**
      `T-TURN-09.SkipLeavesNoPictureStranded`, *"unit 4 has no polyline left after the skip" to be
      0, but it was 4"*, *"unit 4's picture is home at EXACT relative zero rather than parked over
      an intermediate hex -- it is at X=-350.000 Y=86.603 Z=0.000"* and *"nothing anywhere is
      still sliding" to be 0, but it was 1"*. Report `2026.09.02-23.09.44`.
  - **GAP 1 -- THE ARM-LAST ORDERING IS PINNED BY NOTHING, AND THE REPORTED CAUSE IS RIGHT IN ITS
    CONCLUSION AND WRONG IN ITS REASON. THE CORRECTION MATTERS BECAUSE IT CHANGES WHAT WOULD FIX
    IT.** `strat-test-author` wrote `T-TURN-09.TheLastStepsSlideSurvivesTheTourEnding`, built it,
    ran it, found it RED on the unmutated tree, and deleted it rather than softening it -- which
    is the right call. The reported cause was that *"the scenario's hand-over does not end on a
    `Move` that arms"*, which reads as a property of Ferrum Crossing and therefore as something a
    different scenario or a different fixture could fix. **IT IS NOT. IT IS STRUCTURAL, READ OFF
    `FStratAiTurnRunner::RunTurn`:** that function records EVERY applied command including the
    closing EndTurn, and its own comment states *"AN APPLIED EndTurn IS THE ONLY SUCCESSFUL
    EXIT"* -- the `Record` call sits ahead of that exit test. So the last step of every
    normally-completed hand-over is an EndTurn, `Kind != Move`, and
    `UStratMatchSubsystem::PlayMoveSlideForStep` refuses on its first line. **There is never a
    slide in flight when the last step is shown, on any scenario.** A hand-over could end on an
    arming Move only if its final turn were cut short between an accepted Move and its EndTurn,
    which no shipped path produces. Nothing is swallowing the slide; the case the ordering
    protects does not arise. **THE ORDERING IS STILL CORRECT AND STAYS** -- `EndAiPlaybackTour`
    cancels every slide, so arming above it would un-show the final action of every tour the day
    a hand-over did end on a Move -- and **moving that statement above `EndAiPlaybackTour()`
    yields a fully green suite with no warning.** Recorded at the line itself in
    `AdvanceAiPlaybackOneStep` as well as here, on `PendingMoveRoutes.Reset()`'s precedent: this
    is not a gap a test can close, and the comment is the only thing standing in the way.
  - **GAP 2 -- I AGREE THAT `AdvanceOneStepStillArmsNoTimer` IS UNPINNABLE, AND THE REASON IS
    NARROWER AND MORE HONEST THAN "UNOBSERVABLE".** Checked against the code rather than accepted:
    `IsAiPlaybackRunning()` answers off the REEL and deliberately not off `FTimerManager`, and
    `AiPlaybackTimer` is private with no accessor -- so no clause can ask whether a handle is
    pending without this class growing a reader that exists only for a test. And the test lane's
    own argument holds on the fixture shapes that exist: a one-shot is already pending throughout
    a tour, `SetTimer` on a pending handle replaces it, and `ArmNextPlaybackStep`'s own
    `World == nullptr` guard covers the worldless fixture -- so relocating the re-arm into
    `AdvanceAiPlaybackOneStep` leaves identical observable state in a never-ticked world.
    **THE ONE PLACE IT WOULD BE OBSERVABLE IS NAMED RATHER THAN LEFT OUT, SO THAT "UNPINNABLE" IS
    A MEASURED CLAIM AND NOT A CATEGORY:** in a TICKED world with a short interval, a clause that
    hand-drove one step and then let the world run would, under the relocated shape, receive an
    extra automatic step it never asked for. Every playback fixture in this tree uses
    `kHarnessPlaybackInterval = 600.0f` on worlds that are never ticked, precisely to avoid
    timing flakes, so that observation is unavailable here by a deliberate choice made for a
    different reason. **RECORDED AS UNPINNABLE, NOT AS OWED.** The property it would have pinned
    is a claim about WHERE A LINE WAS PUT, and `OnAiPlaybackTimer`'s and
    `AdvanceAiPlaybackOneStep`'s own blocks are what carry it.
  - **WHAT I CHANGED IN THIS PASS, WHICH IS ONE COMMENT AND NO BEHAVIOUR.**
    `UStratMatchSubsystem::AdvanceAiPlaybackOneStep` gained the gap-1 paragraph above the
    `LastArmedSlideSeconds = PlayMoveSlideForStep(Current);` statement. Every other file this pass
    touched hashes byte-identical to its pre-mutation baseline; `StratUnitActor.cpp` is
    `c47e8a23` / `fb7e30ed` again, unchanged from before the first mutant.
  - **THE TREE WAS RESTORED AND RE-MEASURED, AND THAT FINAL RUN IS THE ONE THIS ENTRY VOUCHES
    FOR** -- the six mutant runs vouch for mutants, not for what is left behind. Rebuild
    `Result: Succeeded`; exported report **`reportCreatedOn 2026.09.02-23.12.30`, with ZERO
    non-success entries of any kind**. Neither a count nor a verdict is written here and neither
    may be: `global.md` is the only file that may carry either. **No file under any `Tests/`
    directory was edited by this lane on any pass**; `git status --porcelain` after the pass is
    the same set that stood before it.

- **2026-09-02, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED) -- AI MOVES NOW SLIDE:
  `AStratUnitActor::PlayRouteSlide`, `UStratMatchSubsystem::PlayMoveSlideForStep`, AND A TOUR
  THAT WAITS FOR EACH PICTURE. THIS REVERSES A USER DECISION AND THE REVERSAL IS THE HEADLINE.**
  - **THE REVERSAL, STATED AS A USER DECISION AND NOT AS AN IMPROVEMENT.** Until earlier the same
    day this tree carried, in six prose sites, the rule *"AI moves must not slide (user
    decision)"*. **THE USER REVERSED IT ON 2026-09-02.** AI moves now animate along the hexes
    `FStratBridge::MovePathToHex` actually walked, during §2.11.2's playback tour, ONE UNIT AT A
    TIME with the tour's clock waiting for each slide to finish. Nothing about that reversal was
    inferred from the code and nothing in the code argued for it; it is recorded here as a
    decision so that a future reader who greps the retracted sentences lands on WHO changed it
    rather than on a lane that talked itself out of a constraint.
  - **THE SIX SITES WERE COUNTED RATHER THAN ACCEPTED, AND SIX IS RIGHT.** The dispatch brief
    said six and asked for the count to be verified. Measured with `grep -rn -i "moves must not
    slide|moves do not slide"` across `Source/` and then read at each site, because a phrase
    sweep over a claim this old catches near-misses: `StratMatchSubsystem.h`'s
    `NotePendingMoveRoute` block, `StratMatchSubsystem.cpp`'s `ApplyUnitView` call site,
    `StratUnitActor.cpp`'s empty-route-is-the-first-condition block, `StratUnitActor.h`'s
    `RouteWorldPoints` param doc, `StratUnitActor.h`'s header-block `[EXTENDED 2026-09-02]`
    bullet, and `StratPlayerController.cpp`'s *"ONE CALL AND ONE UNIT"* block. **TWO FURTHER HITS
    ARE NOT THIS CLAIM AND WERE LEFT ALONE**, which is worth recording because a count that
    swept them would have been seven and would have overturned a live rule by accident:
    `AStratUnitActor::SnapToWorldLocation`'s *"a unit that just appeared must not slide in from
    wherever it was standing"* and the two `Tests/` sites quoting it are about a SPAWNED unit,
    a different subject that is untouched and still true.
  - **EACH CORRECTION SEPARATES WHAT STANDS FROM WHAT IS RETRACTED, AND THE CONTROLLER'S IS THE
    ONE THAT WOULD HAVE BEEN CHEAPEST TO BOTCH.** `StratPlayerController.cpp`'s sentence is
    *"ONE CALL AND ONE UNIT. Nothing here notes a route for anything the AI does, and that
    absence is the entire implementation of the user's decision that AI moves must not slide."*
    **THE FIRST HALF REMAINS TRUE AND ONLY THE CLAUSE AFTER THE COMMA IS RETRACTED** -- that file
    still notes exactly one route for one unit on one accepted player command and still contains
    no detection of an AI turn. A correction that struck the whole sentence would have deleted a
    live rule about the input path while claiming to record a decision about animation.
  - **FIVE MORE BLOCKS WERE FALSIFIED INCIDENTALLY AND ALL FIVE WERE CORRECTED.** `ApplyView`'s
    *"CONVERTED HERE AND NOWHERE ELSE"* (now: two named call sites, still one expression);
    `OnAiPlaybackTimer`'s *"IT HOLDS NO LOGIC OF ITS OWN ON PURPOSE"* (now: it holds the CLOCK's
    logic and none of the STEP's, which is what keeps the step hand-drivable); `SkipAiPlayback`'s
    *"NOTHING IS RECONCILED HERE"* (re-scoped, not retracted -- a park cancel recomputes no
    position, reads and writes no transform and consults no model); `TweenOffsets`' terminal-zero
    paragraph (below); and `FStratAiPlaybackStep::RouteHexes`' *"THE CONSUMER CHECKS"*, which
    named a singular and now has two readers. **THE "SIX INTS" COPY COMMENT NEEDED NOTHING** --
    the step-1 pass had already corrected it in place, verified by reading rather than assumed.
  - **THE TERMINAL-ZERO INVARIANT WAS RE-SCOPED AND NOT WEAKENED, AND THE LINE THAT MAKES IT TRUE
    IS UNCHANGED BYTE FOR BYTE.** `AStratUnitActor::ApplyUnitView` still runs
    `TweenOffsets.Add(FVector::ZeroVector)` -- APPENDED, never computed -- and that is legitimate
    on its path because `UStratMatchSubsystem::ApplyView` has already checked
    `PendingRoute->Last() == View.Hex` before converting, so zero is both the right answer AND a
    construction. `PlayRouteSlide`'s terminal is COMPUTED and is zero only by arithmetic, on a
    unit's final move of a hand-over. **The member now carries one invariant true of both paths:
    `Body` at relative `TweenOffsets.Last()` draws the unit over the route's own last hex**, of
    which terminal zero is the special case where that hex is also the actor's.
  - **THE BRIEF'S ARITHMETIC FOR WAYPOINT 0 WAS WRONG AND WOULD HAVE SHIPPED A FEATURE THAT DID
    NOTHING IN ITS COMMONEST CASE. THIS IS THE MOST CONSEQUENTIAL CALL IN THE PASS.** The brief
    required `TweenOffsets[0]` to be overwritten unconditionally with `Drawn - Destination`, on
    `ApplyUnitView`'s recorded anti-pop reason, asserting that *"at rest `Drawn - Destination`
    equals `RouteWorldPoints[0] - Anchor` exactly"*. **THAT EQUALITY HOLDS FOR A UNIT'S SECOND
    AND LATER MOVES AND IS FALSE FOR ITS FIRST.** Read off the code rather than run: before a
    tour the board already holds the FINAL state -- `RunAiTurnsNow` reconciles to it, every actor
    is at its last hex, every picture is at exact relative zero -- so on a unit's first slide
    `Drawn - Destination` is ZERO while `RouteWorldPoints[0] - Anchor` is where the move began.
    Unconditionally overwriting therefore sets waypoint 0 to zero, and **for the commonest reel
    entry of all, a one-step move with two waypoints, the polyline becomes {0, 0}, the arc length
    is zero, `FinishTween` runs and NOTHING ANIMATES.** The feature would have been absent
    exactly where it is most expected, with a green build and a green suite.
    **THE FIX KEEPS THE RECORDED REASON AND CONDITIONS IT:** waypoint 0 is overwritten with the
    drawn position ONLY when a slide is already in flight, captured before the array is rebuilt.
    At rest with a park the two expressions are equal anyway -- the park is
    `previousRoute.Last() - Anchor`, this route's `[0]` is that same hex, both through
    `AStratBoardActor::WorldLocationOfHex` on the same hex, so they agree to the bit -- so the
    condition costs nothing where the brief was right and saves the case where it was not.
    **THE GUARD IS FOR A RE-ARM NOBODY HAS WRITTEN**: `ArmNextPlaybackStep` waits out the seconds
    the actor reports before showing the next step, so mid-slide is unreachable from the tour as
    paced today. It is kept because that pacing is a property of another class.
  - **THE ANCHOR IS THE CALLER'S AND MAY NOT BE DERIVED, WHICH IS WHERE THE HEIGHT CLAUSE LIVES
    OR DIES.** `PlayRouteSlide` takes a TILE-PLANE point and differences every route point
    against it, so `BodyZOffset` cancels exactly and this function holds no second copy of the
    `+ BodyZOffset` expression `ApplyUnitView` and `SnapToWorldLocation` are paired on. The
    obvious derivation -- `GetActorLocation() - FVector(0, 0, BodyZOffset)` -- is forbidden in
    the declaration by name, because it reintroduces the exact expression whose removal is why
    `T-INT-05.TweenWaypointsSitAtTheDrawnUnitsHeight` is green, and that clause was measured able
    to fail on it earlier the same day. `UStratMatchSubsystem::PlayMoveSlideForStep` supplies the
    anchor from `AppliedModel`'s entry for the unit through `WorldLocationOfHex` -- the same
    expression the route goes through, so the two cannot disagree by construction.
  - **THE PARK IS THE NEW STATE AND IT IS BOUNDED BY THE SAME THREE TESTS THE OTHER TWEEN MEMBERS
    ARE.** `AStratUnitActor::TweenRestOffset` is where `Body` rests when the tween retires;
    `FinishTween` writes it instead of a zero literal, and it is ZERO on every path but
    `PlayRouteSlide`'s -- so `ApplyUnitView`'s tween, `SnapToWorldLocation`, `CancelRouteSlide`
    and the whole `MoveTweenSeconds <= 0` C++ FIELD default are byte-for-byte unchanged
    **[CORRECTED 2026-09-03, same pass and same cause as the two stamps in this entry: this read
    "the whole shipped `MoveTweenSeconds <= 0` default" and `MoveTweenSeconds` is serialised as an
    override in `BP_StratUnit.uasset`, so that configuration is the C++ FIELD default and not the
    shipped one. The byte-for-byte claim itself is unchanged and true at that default.]**. Its
    terminal value is zero, three writers drive it there (`ApplyUnitView`'s arming branch,
    `CancelRouteSlide`, and `SnapToWorldLocation` which zeroes it BEFORE `FinishTween` because a
    hard cut must clear a park rather than retire into one), and it is not a BIT -- no marker,
    mesh or material answers off it.
    **IT SITS OUTSIDE `T-INT-05.NoActorHoldsPresentationBits`' SUBJECT, READ OFF THE CLAUSE
    RATHER THAN ASSUMED.** Every assertion in that clause compares a field of
    `GetLastAppliedView()` against the model; a non-cached-view member is on the same footing as
    the three tween members already there.
  - **THE ONE BEHAVIOUR CHANGE IN `ApplyUnitView`, AND THE HAZARD IT WAS WRITTEN AROUND.** When
    the actor genuinely moved AND nothing armed, a park left by a previous tour is cleared.
    **IT IS CONDITIONED ON `bActorMoved` AND MUST NEVER BE UNCONDITIONAL:**
    `AStratPlayerController::Tick` calls `UpdateHoverFromCursor` every frame and a hover crossing
    into a new hex reaches `ApplyView`, so `ApplyUnitView` FIRES DURING A TOUR whenever the player
    moves the mouse across a hex boundary. An unconditional clear would pop every parked unit to
    its final hex on the next mouse move and would read as a random glitch. A hover applies
    identical positions, so `bActorMoved` is false and the park survives.
    **IT IS A HARD CUT AND NOT A ZEROED REST OFFSET, WHICH WAS THE OTHER SHAPE AND IS THE ONE
    THAT POPS.** Zeroing `TweenRestOffset` on a running slide would let it walk the polyline to
    `TweenOffsets.Last()` -- the park -- and only THEN snap to zero. `CancelRouteSlide` retires
    immediately and the picture is home in the same call. The branch also carries a second term,
    `TweenOffsets.Num() > 0 || !TweenRestOffset.IsZero()`, so at the C++ FIELD default both terms
    are false and no automation fixture executes a line of it **[CORRECTED 2026-09-03,
    `strat-gameplay-engineer` (ACTING and WRITING; IN LANE), base commit `283d711`, UNCOMMITTED:
    THIS SENTENCE READ "at the shipped default" AND THAT HALF WAS FALSE. `MoveTweenSeconds` IS
    SERIALISED AS AN OVERRIDE IN `Content/StratPlay/BP_StratUnit.uasset` -- **[WARRANT AMENDED
    2026-09-03 ON THE THIRD RE-GATE'S F7, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE,
    main tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED). CONCLUSION
    UNCHANGED, WARRANT REPLACED. This read that the override was "measured this pass with
    `grep -aF` over the raw bytes, with `AiPlaybackStepSeconds` absent from that asset and present
    in both GameMode Blueprints as the discriminating control and a fabricated name absent from all
    three". That CROSS-ASSET control does not license the inference on an ACTOR Blueprint: the
    within-class control recorded in this file's topmost entry measures PRESENCE as proving nothing
    on `BP_StratUnit`. The name IS present and the override IS real, but it now stands on the
    INDEPENDENT evidence alone -- `content.md` records the authored value, and the tween path is
    inert at the `0.0f` C++ field default while the user watched units slide on 2026-09-03. This is
    the warrant the other two stamps in this entry rest on as well; none of the three may be cited
    to a byte hit.]** So this branch IS
    reachable in the shipped game and is inert only at `MoveTweenSeconds = 0.0f`, the C++ FIELD
    default -- which is why no automation fixture executes it, that half being unchanged and true.
    The header this paragraph documents, `StratUnitActor.h`'s `PlayRouteSlide` block, already said
    "the C++ default", so the record contradicted the code it describes. NOT RAISED BY THE
    RE-GATE -- found by the user in the same pass as F4 and treated as one defect with one
    cause.]**.
  - **THE RE-ARM WENT IN `OnAiPlaybackTimer` AND NOT IN `AdvanceAiPlaybackOneStep`, WHICH IS WHAT
    KEPT TWO BLOCKS TRUE THAT THE OBVIOUS IMPLEMENTATION FALSIFIES.** The tour's interval is now
    variable -- `ActiveConfig.AiPlaybackStepSeconds + LastArmedSlideSeconds` -- so the looping
    handle became a one-shot armed by a new private `ArmNextPlaybackStep`, which refuses on two
    independent reasons (`!AiPlaybackReel.IsPlaying()`, and no world). Putting that inside the
    stepping function would have falsified its own block's *"IT DOES NOT ARM, RE-ARM OR REQUIRE A
    TIMER"* and put a timer in front of every clause in `StratAiPlaybackClauses.cpp`, which drives
    it at `kHarnessPlaybackInterval = 600.0f` on worlds that are never ticked. **BOTH SURVIVING
    SENTENCES ARE NOW STATED EXPLICITLY IN THEIR OWN BLOCKS RATHER THAN LEFT TO BE RE-DERIVED**:
    that one, and `AiPlaybackStepSeconds`' *"this subsystem never reads it"* about
    `AStratUnitActor::MoveTweenSeconds` -- kept true by `PlayRouteSlide` RETURNING the seconds it
    armed, so this class adds what the actor TOLD it instead of predicting what the actor will do.
  - **`BeginAiPlayback` KEPT EVERY GUARD AND EVERY RETIRE-ON-DECLINE EXIT BYTE FOR BYTE.** Only
    the looped `SetTimer` plus its trailing `OnAiPlaybackTimer()` were replaced, by
    `OnAiPlaybackTimer();` alone. *"The first step shows now, not one interval later"* is now
    STRUCTURAL rather than a second call, which is what preserves
    `GATE-AITURN.StepFocusesAndStopsOnTheLast`'s `cursor == 1` assertion -- and that clause is
    green in the run cited below.
  - **THE SLIDE IS ARMED LAST IN `AdvanceAiPlaybackOneStep` -- AFTER `FocusPlaybackStep`, AFTER
    `Advance()`, AND AFTER THE LAST-STEP `EndAiPlaybackTour()`.** That order is the feature:
    `EndAiPlaybackTour` cancels every unit's slide, so arming before it would show the final
    action of every tour and immediately un-show it. Armed after, the last step's slide runs with
    no timer behind it, which is correct -- nothing waits on it and `AStratUnitActor::Tick`
    retires it.
  - **THE CANCEL WENT IN `EndAiPlaybackTour` AND NOT IN `SkipAiPlayback`, BY THE ARGUMENT THAT
    CREATED THAT VERB.** There must be no way to stop a tour without clearing what the tour left
    behind, so a seventh call site cannot reopen the hazard by being written by somebody who
    wanted only to stop a clock. Without it a skip mid-slide strands a picture over an
    intermediate hex forever, with the model and the actor transform both saying otherwise. It is
    a no-op at most of the six sites and at every automation fixture, and that is the point.
    **ONE COST IS NAMED RATHER THAN LEFT TO BE FOUND: it is unconditional over every unit actor,
    so a PLAYER's slide still in flight when a tour ends is hard-cut too.** Distinguishing them
    would need this class to remember which pictures it parked -- presentation state on the
    subsystem bought to avoid presentation state on the actor, which is the wrong trade.
  - **ONE EXPRESSION FOR THE DURATION, AND THE BRIEF SAID THREE CALL SITES WHERE THERE ARE TWO.**
    `AStratUnitActor::TweenDurationSeconds()` holds `MoveTweenSeconds * (waypoints - 1)`, the
    per-hex-step user decision, with the same operands, casts and order `Tick` had inline. It is
    used by `Tick`, which divides `TweenElapsed` by it, and by `PlayRouteSlide`, which returns it.
    `ApplyUnitView` arms a tween and needs no duration, because nothing waits on its slide. The
    discrepancy is recorded in the helper's own block so the next reader does not hunt a third.
  - **AT THE C++ FIELD DEFAULT THE TOUR IS BIT-IDENTICAL TO THE ONE THAT SHIPPED BEFORE THIS.**
    **[CORRECTED 2026-09-03, same pass and same cause as the other stamps in this entry: the
    heading read "AT THE SHIPPED C++ DEFAULT", which names one configuration by two contradictory
    words. `MoveTweenSeconds` is serialised as an override in `BP_StratUnit.uasset`, so the
    bit-identical tour is the one an automation fixture runs, NOT the one the shipped game runs.
    The sentence below it already said "is the C++ default" and needed no change.]**
    `MoveTweenSeconds <= 0` is the C++ default and every automation fixture runs at it, so
    `PlayRouteSlide` returns 0 having written nothing, `LastArmedSlideSeconds` stays 0, the
    interval collapses to `AiPlaybackStepSeconds`, no park is ever created and the new
    `ApplyUnitView` branch is never entered.
  - **MEASURED.** Build: `Build.bat StratocracyEditor Win64 Development` at the full engine path
    -- 37 actions, `StratUnitActor.cpp`, `StratMatchSubsystem.cpp`, `StratPlayerController.cpp`,
    `StratAiPlayback.cpp` and both links among them, **`Result: Succeeded`**, no warning emitted
    on any of the edited translation units. Suite: `Saved/AutomationReport/index.json` DELETED
    first, recording the stamp destroyed -- `reportCreatedOn 2026.09.02-22.09.31` -- and the full
    headless run read from the EXPORTED report rather than the log, which undercounts by one.
    **The run this entry vouches for is stamped `reportCreatedOn 2026.09.02-22.28.08`, with zero
    non-success entries of any kind.** Neither the count nor a verdict is written here and
    neither may be: `global.md` is the only file that may carry either. **The control held:**
    `Stratocracy.StratPlay.T-TURN-09.PlaybackDispositionsAgreeOnState` -- whose fixture drives the
    runner and reel with no world and no subsystem, and which must therefore never reach this
    change -- reports `Success`.
  - **NO FILE UNDER ANY `Tests/` DIRECTORY WAS EDITED BY THIS LANE.** `git status --porcelain`
    after the pass is byte-for-byte the set that stood at dispatch; the three modified and two
    untracked files under `Source/StratPlay/Tests/` are `strat-test-author`'s and were already
    there.
  - **WHAT THE TEST LANE IS OWED, NAMED RATHER THAN LEFT AS "ADD CLAUSES".** In
    `Stratocracy.StratPlay.<ID>.<Clause>` form, each with the property it must pin: the park
    (`PlayRouteSlide` retires with `Body` over the route's LAST hex, not over the actor);
    **the first-slide arithmetic, which is the defect this pass caught and which no existing
    clause can see** (a two-waypoint route from a picture at rest arms a non-degenerate polyline
    -- it is the mutation `TweenOffsets[0] = Drawn - Destination` unconditionally that must redden
    it); the anchor's height cancellation on this new path, mirroring
    `TweenWaypointsSitAtTheDrawnUnitsHeight`; skip-leaves-nothing-stranded (`SkipAiPlayback`
    mid-slide returns every picture to relative zero); the `ApplyUnitView` park-clear behaviour
    change AND its hover companion (an apply with identical positions must LEAVE a park alone --
    that is the half a naive clause will omit, and it is the one that catches the glitch);
    the tour's variable interval as an interval and not as a cursor; and that
    `AdvanceAiPlaybackOneStep` still arms no timer, which is now a claim about where a line was
    put rather than about a line not existing. **A clause over the last step's slide surviving
    `EndAiPlaybackTour` is the sharpest of them**, because the ordering that makes it true is
    three statements apart and nothing else reports it.

- **2026-09-02, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED) -- THE AI ROUTE REACHES
  THE REEL: `FStratAiPlaybackStep::RouteHexes` AND A WIDENED `IStratAiTurnPort::Submit`. THIS
  STEP LEAVES THE TREE DELIBERATELY UNCOMPILABLE AND THE DEBT IS NAMED BELOW.**
  - **WHAT LANDED, IN TWO PIECES THAT ARE USELESS APART.** (1) `FStratAiPlaybackStep` gains
    `RouteHexes`, a `TArray<FIntPoint>` -- the ordered hexes an accepted Move walked, `[0]` the
    from-hex and `.Last()` equal to `Hex`, empty on every other kind and empty when the path
    query refused. `FStratAiPlaybackReel::Record` gains a fourth parameter and copies it
    VERBATIM. (2) `IStratAiTurnPort::Submit` gains a third parameter,
    `TArray<FIntPoint>& OutMoveRoute`, and `FStratBridgeAiTurnPort::Submit`'s Move arm fills it.
    `FStratAiTurnRunner::RunTurn` declares one local per command and passes it through to
    `Record`. Nothing consumes `RouteHexes` yet -- that is the wave's step 4/5 and is NOT here.
  - **A ROUTE AND NOT A FROM-HEX, ON TWO REASONS ALREADY IN THIS TREE RATHER THAN TWO NEW ONES.**
    A from-hex reconstructs a CHORD, and the arming block in `AStratUnitActor::ApplyUnitView`
    records that the degenerate two-point fallback was DELETED rather than kept because "a chord
    is the artefact this whole change exists to remove". And a from-hex would need a companion
    `bHasFromHex` for exactly `FStratAiPlaybackStep::bHasHex`'s own recorded reason -- `(0,0)` is
    a real hex and a value legal in the domain cannot signal absence -- whereas an empty
    `TArray` signals its own absence and cannot acquire that companion's drift.
  - **THE `.Last() == Hex` EQUALITY IS NOT REPEATED IN `Record`, AND THAT IS THE DECISION.**
    `UStratMatchSubsystem::ApplyView` already performs exactly that check on the player's route
    before converting it, and the AI's route arrives at that same check by that same path. One
    checking rule, one place, reused. A second copy in `Record` would be a second thing to keep
    in step; `StratAiTurnRunner.h` already records what that costs, where a second spelling of
    one rule existed for the length of one diff and had drifted inside it.
  - **THE PATH QUERY RUNS BEFORE THE SUBMIT AND IS COPIED OUT ONLY AFTER ACCEPTANCE, AND BOTH
    HALVES ARE LOAD-BEARING.** Asked AFTER the submit, `FStratBridge::MovePathToHex` would answer
    about a board on which the unit already stands on the goal -- a one-hex route at cost 0,
    STRUCTURALLY VALID AND SILENTLY WRONG, satisfying every invariant a consumer could test
    (`[0]` is the from-hex, `.Last()` is the goal, non-empty), so no downstream check could
    catch it. Copied out before acceptance, a refused move would leave a route behind and arm a
    slide for a command the rules module rejected -- which is the player path's own second
    recorded reason for noting a route only on an accepted command. A refused path query
    degrades that one step to a snap; it does not refuse the move.
  - **THE PORT'S RECORDED `IsMatchOver()` RULING WAS RESPECTED AND ONLY ONE OF ITS TWO GROUNDS
    REACHES THIS CHANGE. THE COST OF THE OTHER IS NOT PRETENDED TO BE ZERO.** That ruling
    rejected `IsMatchOver()` because (1) it would let a runner DECIDE and (2) every test double
    would have to grow an arm. GROUND (1) does not reach an out-param: it is an answer handed
    out, never a question, `RunTurn` is forbidden in the interface prose from branching on it,
    and the only thing it does with the value is copy it into a list the PACING bullet already
    documents as written and never read. GROUND (2) DOES reach it and was paid: three test
    doubles must grow the parameter. What makes it payable is what the arm costs to WRITE -- two
    of the three forward to an inner port and pass it straight through, and the third accepts
    without applying and needs one `Reset()`; an `IsMatchOver()` arm would have obliged each
    double to INVENT AN ANSWER about match state, a fact a double has no source for.
  - **`Record`'S NEW PARAMETER IS REQUIRED AND NOT DEFAULTED, DELIBERATELY.** A default of `{}`
    would compile any future caller that forgot the route, and "forgot the route" is
    indistinguishable from "this kind has no route" once it is in the reel -- both are an empty
    array and both snap. There is exactly one caller in this tree, so the default buys nothing
    today and costs a silent failure mode later.
  - **ONE PROSE CORRECTION MADE FLAT, BECAUSE THIS CHANGE FALSIFIED IT IN THE LETTER.**
    `UStratMatchSubsystem::AdvanceAiPlaybackOneStep` said of its step copy "the copy is six
    ints, so the cost of not having to re-establish that argument is zero." `RouteHexes` makes
    that a heap allocation. THE COPY IS KEPT -- it runs once per ~0.5 s playback tick -- and the
    sentence was restated rather than deleted, naming the new cost and why it is still paid.
  - **THE TREE DOES NOT COMPILE AT THIS ENTRY, ON PURPOSE, AND HERE IS WHAT DISCHARGES IT.**
    Widening `IStratAiTurnPort::Submit` breaks every implementation. Four sites, ALL under
    `Source/StratPlay/Tests/`, WHICH IS NOT THIS LANE AND WAS NOT TOUCHED: the three doubles
    `FStratRecordingAiPort` (`Tests/StratAiPlaybackClauses.cpp`), `FStratFaultInjectingAiPort`
    and `FStratNeverEndingAiPort` (`Tests/StratAiTurnRunnerClauses.cpp`), plus a fourth site the
    dispatching brief did not name -- a DIRECT two-argument `Port.Submit(C, PortReason)` on a
    production `FStratBridgeAiTurnPort` inside
    `Stratocracy.StratPlay.T-AI-01.EveryKindRoutesToARecordingSubmit`. **DISCHARGED WHEN
    `strat-test-author` lands the four signatures and the tree builds green.** Until then no
    suite figure from this tree means anything, and none is claimed here.
  - **NO BUILD DIAGNOSTIC WAS OBTAINED, AND THE REASON IS RECORDED RATHER THAN THE ATTEMPT BEING
    RESTATED AS A RESULT.** `Build.bat StratocracyEditor` refused before compiling anything:
    *"Unable to build while Live Coding is active. Exit the editor and game, or press
    Ctrl+Alt+F11"*, `Result: Failed (OtherCompilationError)`, 2.43 s. `UnrealEditor.exe` was
    running (PID 48268). **SO THE FOUR SITES ABOVE ARE A STATIC ENUMERATION AND NOT A COMPILER'S
    LIST**, and a reader is entitled to know which -- the enumeration is a grep over every
    `Submit(` and `Record(` call and override in `Source/`, and nothing in it has been confirmed
    by a compiler on this tree. Whether the set is EXACTLY these four is unproven until the
    editor is closed and the build is run.

- **2026-09-02, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED) -- FALSIFIABILITY OF THE
  FIVE ROUTE-TWEEN CLAUSES, MEASURED BY MUTATING MY OWN FILES.
  **[UPDATED 2026-09-02, SAME DAY, SECOND PASS, AFTER `strat-test-author` REWROTE THE TWO CLAUSES
  THIS PASS PROVED INERT. THE HEADLINE IS RESTATED FLAT, NOT APPENDED TO, BECAUSE A READER WHO
  GREPS THE OLD ONE MUST LAND ON THE CURRENT RESULT.]** It said:
  RETRACTED> "**THREE ARE PROVEN ABLE TO FAIL, TWO ARE INERT AND ARE NOT DISCHARGED**, and the
  RETRACTED>  more useful of the two negatives is that one of them CANNOT be fixed by rewriting
  RETRACTED>  the clause."
  **ALL FIVE CURRENT CLAUSES ARE NOW PROVEN ABLE TO FAIL, AND THE DEBT IS DISCHARGED FOR EVERY
  ONE OF THEM.** The two negatives were acted on rather than argued away: one clause was rewritten
  and now reddens under the very mutation it survived, and the other was RETIRED and replaced by a
  clause over the mechanism that actually forecloses the hazard. The per-clause detail below is
  kept in its original form and each entry carries its own second-pass result, so the first pass's
  findings remain readable as the reason the rewrites happened.**
  - **WHY THIS LANE RAN IT.** `strat-test-author` wrote the five clauses and named the mutation
    each needed, but every one of those mutations edits `AStratUnitActor::ApplyUnitView` or
    `UStratMatchSubsystem::ApplyView` -- this lane's files. A green suite over a clause never
    shown able to fail pins nothing, so the clauses were green and the debt was open.
  - **METHOD, AND THE FOUR TRAPS IT WAS BUILT AGAINST.** Mutations applied **IN PLACE** (a copied
    tree reuses `Intermediate/Build` and the mutant build is a silent no-op that reports the old
    binary green); **rebuilt before every run**; `Saved/AutomationReport/index.json` **deleted
    before each run** and the EXPORTED report read rather than the log; and every revert proved by
    `git hash-object` AND `md5sum` against baselines captured before the first mutation --
    `StratUnitActor.cpp 0185e505...` / `md5 cd14daff...` and `StratMatchSubsystem.cpp
    a238de29...` / `md5 87b29c7f...`. Every patch refused on an anchor count other than 1, which
    caught one real error: `PendingMoveRoutes.Reset();` occurs **twice** (in `ApplyView` and in
    `TearDownPresentation`) and the naive anchor would have mutated both.
  - **PER CLAUSE. THE THREE THAT ARE DISCHARGED.**
    - `TweenWaypointsFollowTheNotedRoute` -- **DISCHARGED.** Collapsing the per-hex loop to the
      deleted two-point chord reddens it: *"Expected 'the armed polyline has one waypoint per hex
      of the noted route' to be 4, but it was 2."*
    - `TweenWaypointsSitAtTheDrawnUnitsHeight` -- **DISCHARGED, AND IT IS THE SHARPEST RESULT
      HERE.** Changing `const FVector RouteEnd = RouteWorldPoints.Last();` to `= Destination;`
      reddens **it alone** -- *"waypoint 1 draws at the unit's own height over hex (-1, 4): Z
      0.00, expected 37.00 (plane 0.00 + lift 37.00)."* That is the exact `BodyZOffset` defect
      this lane refused while writing the comment, now reproduced and caught by a clause. **AND
      `TweenWaypointsFollowTheNotedRoute` STAYED GREEN UNDER IT**, which is what proves the two
      clauses are not redundant: one asserts X/Y, the other Z, and only the split sees this.
    - `NoNotedRouteArmsNoTween` -- **DISCHARGED.** Restoring the degenerate empty-route fallback
      reddens it: *"Expected 'a move with no noted route arms no tween at all' to be 0, but it was
      2."*
  - **THE TWO THAT WERE NOT DISCHARGED ON THE FIRST PASS, EACH NOW FOLLOWED BY ITS SECOND-PASS
    RESULT. THE FIRST-PASS FINDING IS LEFT STANDING BECAUSE IT IS WHY THE REWRITE EXISTS.**
    - `SecondApplyWithIdenticalPositionsLeavesTheTweenAlone` -- **FIRST PASS: INERT.**
      Dropping the `bActorMoved` half of the arming condition in `AStratUnitActor::ApplyUnitView`
      reddens **nothing**; the suite stayed fully green. The mutant was verified to have REACHED
      THE BINARY rather than being a stale-build artefact: an immediately repeated build reported
      `Target is up to date`, so the compile had happened. **THE CAUSE, READ OFF THE CLAUSE RATHER
      THAN GUESSED:** it applies the same model twice, and the FIRST `ApplyView` already emptied
      `PendingMoveRoutes`, so the second is refused by the `RouteWorldPoints.Num() > 0` term and
      never reaches `bActorMoved` at all. The clause exercises the map reset, not the guard it is
      named for. **`bActorMoved` was at that point pinned by nothing.**
      **SECOND PASS, AFTER THE REWRITE: DISCHARGED.** `strat-test-author` rewrote it so the second
      apply carries **the chord** rather than the same route -- and it caught, before I ran
      anything, that re-noting the SAME route would have been unfalsifiable in a subtler way: the
      rebuilt waypoint array is identical element by element, because `Drawn` sits at
      `TweenOffsets[0]` so `Drawn - Destination` reproduces it and every other waypoint is the same
      expression over the same route, so a re-arm would be invisible to any assertion about
      numbers. With the chord carried instead, both earlier terms of the arming condition are
      satisfied and `&& bActorMoved` is the only thing that can refuse the re-arm. **THE SAME
      MUTATION THAT REDDENED NOTHING ON THE FIRST PASS NOW REDDENS IT**, by the predicted
      mechanism and not merely in the predicted direction: *"Expected 'the second apply leaves the
      waypoint count untouched -- 4, not the chord's 2' to be 4, but it was 2."* `bActorMoved` is
      now pinned.
    - `PendingRouteIsEmptiedByEveryApply` -- **INERT. NOT DISCHARGED, AND IT IS NOT FIXABLE BY
      REWRITING THE CLAUSE.** Replacing the unconditional `PendingMoveRoutes.Reset()` with a
      removal of only the consumed ids reddens nothing. **THE CAUSE IS THE CLAUSE'S OWN MIDDLE
      STEP:** it revives the unit with `ApplyView(AtStart)` before the move, and that apply puts
      the unit back into `Seen` -- so the mutant removes the route exactly there, and the map is
      empty by the final move regardless. **AND A FURTHER DIAGNOSTIC SHOWS IT IS AN EQUIVALENT
      MUTANT, NOT MERELY AN AWKWARD CLAUSE.** Running the mutation TOGETHER WITH `bActorMoved`
      removed, and again together with the last-hex equality guard removed, left the suite green
      in both cases. The reason is structural: a unit absent from a model has its actor
      **destroyed** by `ApplyView`'s own destroy loop, so when its id next appears the actor is
      newly spawned and `SnapToWorldLocation`'d onto its hex -- `bActorMoved` is therefore false on
      the very apply a stranded route could have attached to. **The hazard the unconditional
      `Reset()` was written for is already foreclosed by a second, independent mechanism, so no
      rewriting of this clause can observe the mutation.**
      **SECOND PASS: THE CLAUSE WAS RETIRED, WHICH IS THE RIGHT ANSWER TO AN EQUIVALENT MUTANT AND
      NOT A CONCESSION.** `strat-test-author` removed it and added
      `Stratocracy.StratPlay.T-INT-05.ASpawnedUnitDoesNotSlideEvenWithARouteNoted` in its place,
      over the mechanism this pass identified as the one that actually forecloses the hazard --
      destroy, respawn, snap, actor-not-moved. **IT IS PROVEN ABLE TO FAIL TWICE OVER, BY TWO
      DIFFERENT MUTATIONS IN TWO DIFFERENT FILES.** Dropping `&& bActorMoved` in
      `AStratUnitActor::ApplyUnitView` reddens it, and so does deleting
      `Spawned->SnapToWorldLocation(Where);` from `UStratMatchSubsystem::ApplyView`'s spawn branch
      -- both with the same message, *"Expected 'a freshly spawned actor arms no tween, though a
      4-hex route ending on its own hex was noted' to be 0, but it was 4."* So the new clause pins
      BOTH halves of the spawn defence, and the retired clause's subject is covered by something
      that can actually fail. **DISCHARGED.**
      **AND THE SPAWN-SNAP MUTATION SHOWED NO ENTANGLEMENT, WHICH WAS WORTH CHECKING BECAUSE IT
      LOOKED LIKELY.** It reddens that clause ALONE -- notably
      `Stratocracy.StratPlay.T-INT-05.ApplyViewSpawnsMovesAndDestroys` stays GREEN, because
      `ApplyUnitView`'s own `SetActorLocation` still writes the destination immediately afterwards,
      so the spawn snap is unobservable in the FINAL position and observable only in whether a
      tween arms. That is exactly the split the two clauses are meant to have.
  - **WHAT I AM NOT DOING ABOUT THAT, AND WHY.** The unconditional `Reset()` STAYS. It is the
    cheaper and more direct of the two guarantees, it does not depend on reasoning about actor
    lifetimes two functions away, and "this line is currently unobservable" is not a reason to
    delete a correct one. What changes is the CLAIM: **this lane's ROUTE TWEEN entry said of the
    two protections that "neither relies on the other", and that is true of the CODE and was never
    true of the TESTS** -- no clause distinguishes them, and one of them cannot be distinguished
    even in principle. That sentence should be read as a statement about the code only.
    **AND THE RETIREMENT LEAVES THAT LINE PINNED BY NOTHING, WHICH IS STATED AS A STANDING DEBT
    RATHER THAN CLOSED BY THE GREEN RESULT ABOVE.** With
    `PendingRouteIsEmptiedByEveryApply` gone, no clause reddens if `PendingMoveRoutes.Reset()`
    becomes conditional. That is not a gap a test can close -- it is the equivalent-mutant finding
    restated -- so the honest record is: the line stays, it is correct, it is defence in depth
    behind `bActorMoved`, and **no clause will ever report its removal.** A future reader deleting
    it as dead code would get a green suite; this bullet is the only thing standing in the way.
  - **ONE ENTANGLEMENT WORTH RECORDING, SINCE IT IS NOT A DEFECT BUT WILL LOOK LIKE ONE.** The
    two-point-chord mutation reddens THREE clauses, not one -- `FollowTheNotedRoute`,
    `SitAtTheDrawnUnitsHeight` and `SecondApply...` -- and all three fail on the same message
    shape, *"to be 4, but it was 2"*. Two of them fail on their shared arrange-guard ("a tween is
    in flight to measure"), never reaching their own subject. That guard is reasonable, but it
    means a future reader must not read "three clauses reddened" as three independent detections.
  - **THE TREE WAS RESTORED AND RE-MEASURED, WHICH IS THE RUN THAT VOUCHES FOR IT.** After the
    last revert of EACH PASS both files hash byte-identical to the pre-mutation baselines above --
    `git hash-object` and `md5sum` both, on both files -- and a final rebuild
    (`Result: Succeeded`) and full suite were run over the restored bytes with **zero non-success
    entries**; the mutant runs vouch for mutants, not for what is left behind. The second pass
    added three more mutant runs on the same baseline, and the deleted report stamp it began from
    was `2026.09.02-20.41.19`, which matched `strat-test-author`'s own reported run and so
    corroborated it rather than taking it on trust.
    **NEITHER THE COUNT NOR THE VERDICT IS WRITTEN HERE AND NEITHER MAY BE: `global.md` is the
    only file that may carry a live suite count or a phase verdict.** The restored run's exported
    report is stamped `reportCreatedOn 2026.09.02-20.30.33`; cite that rather than any number,
    since a re-run replaces that file in place. No file under any `Tests/` directory was edited by
    this lane on any pass.
  - **HANDOFF TO `strat-test-author`, STATED AS TWO DIFFERENT ASKS.** (1)
    `SecondApplyWithIdenticalPositionsLeavesTheTweenAlone` IS fixable: note the route again before
    the second `ApplyView`, so that apply genuinely carries one and `bActorMoved` is the only
    thing that can refuse it. (2) `PendingRouteIsEmptiedByEveryApply` is NOT fixable as written,
    per the equivalent-mutant finding above; it should either be retired, or restated as a clause
    about the mechanism that ACTUALLY forecloses the hazard -- a respawned actor does not slide --
    which is a claim about `SnapToWorldLocation` and `bActorMoved` rather than about the map.
    **[BOTH ASKS WERE ACTED ON THE SAME DAY AND BOTH ARE NOW MEASURED -- SEE THE SECOND-PASS
    RESULTS ON EACH CLAUSE ABOVE. THIS HANDOFF IS CLOSED.]** Ask (1) was taken up but NOT as
    written: my proposed fix -- re-note the same route -- would itself have been unfalsifiable,
    and `strat-test-author` caught that and carried the chord instead. Recorded because a handoff
    that was improved by its recipient is worth more to the next reader than one that was merely
    obeyed.

- **2026-09-02, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED, ON TOP OF THE ENTRY
  BELOW WHICH IS ALSO UNCOMMITTED) — THE ROUTE TWEEN: THE PICTURE NOW
  WALKS THE HEXES THE RULES MODULE MOVED THE UNIT THROUGH, AND THE THING WORTH READING IS
  THAT THE ANIMATION AND THE COMMIT ASK THE SAME QUESTION OF THE SAME METHOD, SO THE PATH
  DRAWN CANNOT DIVERGE FROM THE PATH TAKEN. **COMPILED CLEAN AND THE SUITE RAN OVER IT** —
  the discharge, and the one thing green does NOT cover, are stated at the end.**
  - **THE GAP.** The entry below eased `Body`'s offset to zero over a SINGLE vector, so a unit
    ordered three hexes around a mountain slid along the STRAIGHT CHORD and cut the corner it
    had just been routed around. The user asked for travel along the actual route.
  - **NO PATHFINDING WAS WRITTEN AND NONE MAY BE, WHICH IS THE CONSTRAINT THAT SHAPED
    EVERYTHING ELSE.** The only neighbour math in this tree is vendored `strat::` in `Hex.h`
    and is unreachable from `StratPlay` by the `LNK2019` rule. `FStratBridge::MovePathToHex`
    already returns the ordered route and its own block records that it is THE SAME ROUTE THE
    COMMIT TAKES — §2.5's tie-break is decided solely in `Move.h` — and `FStratBridgePathQuery`
    already forwards to it for §2.11.1's preview. **The capture below constructs that existing
    adapter and calls `IStratPathQuery::PathTo`.** No bridge method, no query type, no hex
    arithmetic and no reach filter were added. A local walk downhill through the reach set
    would have been fifteen lines and would have been `Move.h`'s tie-break restated in the
    input path — the substitution `T-UI-02` exists to catch, one layer over.
  - **THE CAPTURE IS BEFORE THE SUBMIT, AND THAT IS THE ONLY CORRECT POSITION RATHER THAN A
    PREFERENCE.** `AStratPlayerController::HandleSelectionEvent` asks for the route inside the
    `Outcome.Command != None` block and above `StratSubmitSelectionCommand`. After the submit
    the unit is STANDING ON THE GOAL, so `MovePathToHex` answers `{goal}` at cost 0 — a
    one-point route, structurally valid and silently wrong, indistinguishable downstream from
    a real answer. It is held in a local and handed to
    `UStratMatchSubsystem::NotePendingMoveRoute` only after the bridge accepts, so a refused
    move leaves nothing behind.
  - **THE CARRIER IS A MAP ON THE SUBSYSTEM AND NOT A FIELD ON THE MODEL, ON A PRECEDENT
    RATHER THAN AN INVENTED ARGUMENT.** `UStratMatchSubsystem::PendingMoveRoutes` sits beside
    `AiPlaybackReel`, whose own block already refused `FStratViewModel` for the playback cursor
    in these words: *"`T-INT-05`'s subject is every member of the view model, so a playback
    cursor placed there would enter that ID's subject by its own words and owe a clause under
    it."* A route is the sharper case — it is where a unit CAME FROM, and no statement of what
    IS can carry that. **`FStratPathPreviewView` was the other candidate and is structurally
    disqualified, which is a second and independent reason:** `StratDecoratePathPreview` clears
    the preview for a unit whose `bHasMoved` is set, and that is the state one microsecond
    after a move applies — a carrier that is empty exactly when the animation needs it.
  - **THE PENDING MAP IS ITSELF THE AI SIGNAL, AND NO AI DETECTION EXISTS OR MAY BE ADDED.**
    USER DECISION: AI moves snap. A player move notes a route; nothing else calls
    `NotePendingMoveRoute`; `AStratUnitActor::ApplyUnitView` arms a tween ONLY when handed a
    non-empty route. So AI moves, loads, reseeds, spawns and a path query that refused all
    take one arm and all snap. **THIS CHANGED PHASE 1'S BEHAVIOUR AND THE CHANGE IS
    INTENDED:** "no route" used to mean a chord slide and now means a snap. The degenerate
    two-point fallback was DELETED rather than kept beside the new path — keeping it would
    have made the commonest AI move slide along the very line this change exists to remove.
  - **THE ROUTE IS CONVERTED IN EXACTLY ONE PLACE, WHICH IS WHY `NotePendingMoveRoute` TAKES
    AXIAL HEXES.** `UStratMatchSubsystem::ApplyView` converts it beside the existing `Where`
    hoist through `AStratBoardActor::WorldLocationOfHex` — the project's one axial→world
    expression. A second copy anywhere on the route's journey would be a slide half a tile off
    the day `HexSize` changes, and off by a DIFFERENT amount than the destination it ends at,
    so the unit would not arrive where it stopped.
  - **AND THE MAP IS EMPTIED WHOLE, UNCONDITIONALLY, AFTER THE DESTROY LOOP.** Not after the
    unit loop: a player's move is followed by the AI's reply before any model is built, so the
    noted unit CAN BE DEAD by then — no model entry, no actor, never seen by the unit loop. A
    "remove the ones I used" clear would strand that route for a later unit the rules module
    reissued the id to. `TearDownPresentation` clears it too, beside `AiPlaybackReel.Reset()`,
    for the match-boundary reason that member's own comment gives.
  - **THE RE-ENTRANCY FIX, WHICH IS THE ONE THING A CHORD DID NOT NEED AND A ROUTE CANNOT DO
    WITHOUT.** A player move produces TWO `ApplyView` calls with identical positions — the
    controller documents that ordering as deliberate. Re-arming a zero-length chord was
    harmless; re-arming a route would COLLAPSE IT IN THE SAME FRAME. `ApplyUnitView` now reads
    the actor's previous location alongside `Drawn`, before the snap, and arms only when the
    actor ACTUALLY MOVED. **This is neither an event inference nor a remembered bit, and both
    halves are stated at the code:** it asks nothing about what happened and labels nothing —
    only whether this class's own last transform write differs from the one it is about to
    make, the same permission `Drawn` already exercises — and it is a LOCAL that does not
    survive the call, so there is still no `LastAppliedLocation` member and
    `Stratocracy.StratPlay.T-INT-05.NoActorHoldsPresentationBits` is untouched. Two
    independent things now protect a running slide, and neither relies on the other: this
    guard, and the fact that `ApplyView` emptied the map before the second apply ran.
  - **THE PROFILE IS A TRAPEZOID AND NOT `SmoothStep`, AND THE OLD BLOCK'S ARGUMENT SURVIVES
    RATHER THAN BEING DELETED WITH THE CURVE.** The user asked for constant speed between
    eased ends; `SmoothStep` has no constant middle at all — its velocity peaks at the
    midpoint, so a unit crossing four hexes visibly races the middle two.
    `AStratUnitActor::MoveTweenEaseFraction` (0.25, clamped to [0, 0.5] where it is READ, not
    where it is declared) drives the closed-form integral of a trapezoidal velocity profile,
    with the plateau speed DERIVED as `1/(1-E)` rather than tuned. **What the `SmoothStep`
    block actually argued was never that curve — it was ZERO VELOCITY AT BOTH ENDS**, so a
    tween replaced mid-flight begins from rest and the seam is invisible. This profile has
    exactly that property and is continuous throughout, so the argument is restated at
    `MoveTweenEaseFraction` and at `AStratUnitActor::Tick` rather than dropped.
  - **A SCALAR AND DELIBERATELY NOT A `UCurveFloat`:** a curve is a `/Game/` asset this file
    may not name, so it would arrive as a second `EditDefaultsOnly` property that is unset by
    default — and an unset curve is an animation that silently does nothing.
  - **ONE REGRESSION IS DECLARED RATHER THAN LEFT TO BE FOUND.** A ONE-HEX HOP MOVED FROM
    `SmoothStep` TO THE TRAPEZOID over the same duration — same endpoints, same zero-velocity
    ends, a slightly different feel between them. Keeping `SmoothStep` for one-step routes was
    the alternative and is REFUSED: two profiles is two numbers to tune, two blocks of prose
    to keep true, and a discontinuity in behaviour at a route length nobody would think to
    test either side of.
  - **DURATION IS PER HEX STEP, A USER DECISION** — `MoveTweenSeconds * (waypoints - 1)`. A
    four-hex move takes four times a one-hex hop, which is what makes speed read as constant
    across moves of different lengths.
  - **`MoveTweenSeconds` IS NOT RENAMED, AND THE COST OF RENAMING IT IS MEASURED IN ADVANCE
    RATHER THAN GUESSED.** `MoveTweenSecondsPerHex` is the better name.
    `Content/StratPlay/BP_StratUnit.uasset` stores the authored value keyed by the property's
    `FName`, so a rename ORPHANS it: the Blueprint falls back to the C++ default of 0.0, the
    animation switches off ENTIRELY, the build is green, no log fires and no clause reddens —
    because zero is a supported configuration. Mitigating it needs a `+PropertyRedirects` line
    in `Config/DefaultEngine.ini`, which is the DATA STEWARD's lane. **So the MEANING was
    redefined in the property's own block using this file's `RETRACTED>` convention** — its
    sentence about a single offset decaying is now false in the letter — and the name is
    recorded as under-specific by one word with the cost of fixing it named.
  - **ONE ARITHMETIC DEFECT WAS CAUGHT BY WRITING THE COMMENT, AND IT WOULD HAVE SHIPPED
    VISIBLY RATHER THAN LATENTLY.** The obvious offset expression is `RouteWorldPoints[i] -
    GetActorLocation()`, and it is WRONG: the route points sit on the tile plane while the
    actor sits at that plane PLUS `BodyZOffset`, so every intermediate waypoint would be
    `BodyZOffset` below the drawn unit and the picture would dip into the board and climb back
    out.
    **[CORRECTED 2026-09-02, SAME DAY, BY THE LANE THAT WROTE IT, AFTER THE `coordinator`
    CHALLENGED THE PREMISE. WRITTEN FLAT AND NOT APPENDED TO, BECAUSE A CORRECTION THAT ONLY
    ADDS LEAVES BOTH CLAIMS LIVE.]** It said:
    RETRACTED> "**It is invisible today because the shipped `BodyZOffset` is 0.0.**"
    **THAT IS FALSE, AND IT UNDERSTATED THE FIX IN THE ONE DIRECTION THAT MATTERS.** `0.0f` is
    the C++ default in `StratUnitActor.h`; **`BP_StratUnit` OVERRIDES IT.**
    `Tools/architect/state/content.md` records `strat-editor-builder` reading the live CDO twice
    on 2026-09-02 -- once enumerating the surviving `EditDefaultsOnly` overrides
    (`BodyZOffset = 50.0`) and once, in a later pass, using `BodyZOffset = 50.000000` as the
    positive control that its reader could see a float of that kind at all, beside
    `MoveTweenSeconds = 0.200000`. **So the shipped value is 50, the defect was NOT latent, and
    with the shipped Blueprint every multi-hex move would have visibly dipped the unit 50 units
    into the board and climbed back out.** The retracted sentence would have told a future
    reader that differencing against the route's own last point was cosmetic and could be
    undone; it is load-bearing and must not be.
    **WHAT I MYSELF COULD NOT VERIFY, STATED RATHER THAN GLOSSED.** I did not read that 50 from
    the asset. `Content/StratPlay/BP_StratUnit.uasset` is LFS -- `git check-attr filter` returns
    `filter: lfs` and `git show HEAD:<path>` hands back the pointer
    (`oid sha256:e605800b...`, `size 26623`), not content -- and it is binary, and the editor
    that could answer is open and is not this lane's to drive. **The number above is
    `content.md`'s RECORDED MEASUREMENT by another lane, cited as such, and not an observation
    of mine.** What I can say from a checkout is the half that does not depend on it: `0.0f` is
    the C++ default, a Blueprint may override it, and the corrected expression is right for
    EVERY value of it including zero -- which is why the fix stands on its own and does not rest
    on this figure.
    The offsets are therefore differenced against the route's OWN last point, which cancels the
    lift exactly, needs no knowledge of it, and adds no third copy of the `+ BodyZOffset`
    expression the header pairs across `ApplyUnitView` and `SnapToWorldLocation`.
  - **THE SAME FALSE SENTENCE WAS ALSO IN THE CODE, AND IS NOW CORRECTED THERE TOO -- DEBT
    RAISED AND DISCHARGED THE SAME DAY, RECORDED AS BOTH RATHER THAN AS NEITHER.**
    `AStratUnitActor::ApplyUnitView`'s offset block carried it in prose -- *"It is invisible
    today because the shipped `BodyZOffset` is 0, which is the worst kind of latent defect"* --
    the exact claim retracted above. **The condition written here for discharging it was "the
    next pass that touches `StratUnitActor.cpp` corrects that sentence in place, in the same
    build that compiles this feature", and that is the pass that happened.** The comment now
    carries the flat retraction, the C++-default-versus-Blueprint-override distinction, the
    `content.md` citation attributed to `strat-editor-builder` as another lane's measurement,
    and the half that survives regardless of the number -- the expression is correct for EVERY
    value of `BodyZOffset` including zero. **A tree-wide grep for the claim's vocabulary
    (`BodyZOffset`, `invisible today`, `latent`) now returns the sentence ONLY inside
    `RETRACTED>` lines, in both files.** The record and the code no longer disagree.
    **AND THE COMMENT-ONLY EDIT WAS REBUILT RATHER THAN ASSUMED SAFE**, which was not
    ceremonial: `StratPlay` runs an adaptive non-unity working set and the build log names
    `[Adaptive Build] Excluded from StratPlay unity file: StratMatchSubsystem.cpp,
    StratPlayerController.cpp, StratUnitActor.cpp`, so this file has its own translation unit
    and a real compile action ran for it -- `[1/4] Compile [x64] StratUnitActor.cpp`, then a
    relink of `UnrealEditor-StratPlay.dll`. "Comments cannot break a build" would have skipped a
    genuine compile.
  - **TWO SMALL THINGS THE PLAN DID NOT NAME, LISTED BECAUSE A SILENT ADDITION IS THE ONE A
    GATE CANNOT SEE.** (1) `SnapToWorldLocation` now CALLS `FinishTween()` instead of carrying
    four hand-copied clear lines — there are two arrays and a scalar to retire now, and a
    caller that cleared two of the three would leave a spent polyline behind
    `GetTweenWaypointCount`. (2) `ApplyView` checks that the noted route's LAST HEX equals
    `View.Hex` before converting it. It is an equality and not a rule — no legality is decided
    — and a mismatch degrades to a snap; it makes "these waypoints describe THIS unit's
    arrival" checkable rather than assumed across the AI's reply.
  - **THE ACCESSOR PAIR EXISTS FOR THE CLAUSE AND SAYS SO, on `IsGuidedMarkerVisible`'s
    permission.** `AStratUnitActor::GetTweenWaypointCount` and
    `AStratUnitActor::GetTweenWaypoint` report the live arrays and not a cache. They are here
    because **"the picture follows the route's hexes rather than the chord" is a claim about a
    POLYLINE, and a polyline is unassertable from a position sampled at one instant** — a
    chord and a route agree at both ends. `Tick` stays protected and NO test-only advance
    entry point was added.
  - **[DISCHARGED 2026-09-02, SAME DAY, ONCE THE EDITOR WAS CLOSED. THE CAVEAT IS RESTATED FLAT
    RATHER THAN DELETED, because a reader who arrives by a citation to "uncompiled" must land on
    what replaced it.]** It said:
    RETRACTED> "**WHAT I COULD NOT VERIFY, AND IT IS EVERYTHING A BUILD WOULD HAVE TOLD ME.** An
    RETRACTED>  editor was open (`UnrealEditor.exe`, PID 24336) and `Build.bat` refused before
    RETRACTED>  compiling anything ... so **this entry describes UNCOMPILED CODE**: no compiler
    RETRACTED>  verdict, no UHT verdict on `MoveTweenEaseFraction`, and no suite run."
    **THE CODE IN THIS ENTRY IS NOW COMPILED AND THE SUITE HAS RUN OVER IT.** `Build.bat`
    returned `Result: Succeeded` with no warnings; `StratUnitActor.cpp` was compiled and
    `UnrealEditor-StratPlay.dll` linked, so `MoveTweenEaseFraction` has a UHT verdict and the
    widened `AStratUnitActor::ApplyUnitView` signature has a compiler one. A full headless run
    followed over that binary, exported to `Saved/AutomationReport/index.json`, and no file
    under any `Tests/` directory was edited on any pass. **NEITHER THE COUNT NOR THE VERDICT IS
    WRITTEN HERE, AND NEITHER MAY BE: `global.md` is the only file that may carry a live suite
    count or a phase verdict**, and the non-success side of the figure is still the figure.
    Read both there. A citation should name the exported report's own `reportCreatedOn` rather
    than any number, since a re-run replaces that file IN PLACE and invalidates every figure
    quoted against the old one.
    **ONE THING THE BUILD DID NOT SETTLE AND THE SUITE CANNOT:** the polyline is unpinned by any
    clause, so green here does not mean the unit walks the right hexes. See the risk bullet
    below, which is unchanged by the discharge.
    **THE ENTRY BELOW IS DISCHARGED BY THE SAME BUILD AND THE SAME RUN**, which is what "the two
    must be re-checked together" asked for -- the chord tween it describes no longer exists as
    written, so nothing could have validated one without the other, and nothing did.
  - **AND ONE RISK NO BUILD AND NO SUITE WOULD CATCH.** The polyline is a purely visual thing
    that no clause reads today, so a route that eases along the wrong hexes, or eases along
    the right ones at a visibly wrong speed, reddens nothing. The accessor pair above is what
    makes a clause POSSIBLE; until one exists the only gate is a human watching a unit move.

- **2026-09-02, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `283d711`, UNCOMMITTED) — THE MOVE TWEEN:
  THE ACTOR STILL TELEPORTS AND ONLY THE PICTURE OF IT SLIDES, WHICH IS WHAT LETS AN EASED
  MOVE EXIST WITHOUT EDITING A SINGLE TEST FILE. **THE CODE IS NOW COMPILED AND THE SUITE HAS
  RUN OVER IT** — discharged at the end, by the same build and run as the ROUTE TWEEN entry
  above, which is what "the two must be re-checked together" required.**
  - **THE GAP, AND IT IS THE PLAYER'S OWN REPORT.** Units jump between hexes.
    `AStratUnitActor::ApplyUnitView` wrote the destination straight onto the actor and the
    comment over that line said so in terms — "TELEPORTED, NOT INTERPOLATED".
  - **THE CONSTRAINT THAT MADE IT MORE THAN A LERP, AND IT IS A FACT ABOUT ROOTS RATHER THAN
    ABOUT ANIMATION.** `AStratUnitActor::Body` was the RootComponent, and **a root's relative
    location IS the actor's world location** — so there was nowhere to put a visual offset
    that `GetActorLocation()` would not also report. Interpolating the actor transform was
    the obvious shape and is the one that had to be refused: the reconcile clause
    `Stratocracy.StratPlay.T-INT-05.ApplyViewSpawnsMovesAndDestroys` reads
    `AStratUnitActor::GetActorLocation()` **synchronously** after
    `UStratMatchSubsystem::ApplyView` and compares it to
    `AStratBoardActor::WorldLocationOfHex`, so a tweened actor transform reddens it on the
    first frame and the only repair would be editing a file this lane does not own.
  - **THE DESIGN, IN ONE SENTENCE: THE TRANSFORM SNAPS AND `Body`'s RELATIVE LOCATION
    CARRIES A DECAYING OFFSET.** `AStratUnitActor::UnitRoot` is a new empty
    `USceneComponent` inserted above `Body` as the root — the shape
    `AStratBoardActor::BoardRoot` and `AStratCameraPawn::PivotRoot` already use.
    `AStratUnitActor::ApplyUnitView`'s `SetActorLocation` line is **unchanged, byte for
    byte**, and that is deliberate: it makes "this feature does not move the actor"
    checkable by reading rather than by argument. `AStratUnitActor::Tick` eases the offset
    to zero and `AStratUnitActor::FinishTween` writes exactly zero and switches the tick
    back off.
  - **THE THREE MARKERS STAY ON `Body` AND THAT STOPPED BEING FREE.** `GuidedMarker`,
    `FlagMarker` and `UnactedPip` keep `SetupAttachment(Body)`, so they follow THE VISUAL
    and arrive with the unit. On `UnitRoot` they would follow THE DESTINATION and sit at the
    far hex while their unit was still sliding. The constructor comment that used to say
    "attached to `Body` and not to the actor root separately" was true and idle; it is now
    rewritten to say that moving any marker up to the root is a visible defect with a green
    build.
  - **NO REMEMBERED LOCATION AND NO "IS THIS THE FIRST APPLY" BIT, WHICH IS THE T-INT-05
    ANSWER RATHER THAN A STYLE CHOICE.** The tween's start is measured off
    `Body->GetComponentLocation()` immediately before the snap, so a second `ApplyView`
    arriving mid-tween restarts from where the unit is ACTUALLY DRAWN and cannot pop — and
    there is therefore no `LastAppliedLocation` field and no `bHasAppliedAView` flag on the
    actor, which is what `Stratocracy.StratPlay.T-INT-05.NoActorHoldsPresentationBits` is
    about. The one fact the model cannot supply — *this actor is new, do not animate* — is
    known by the spawner and said by it: `UStratMatchSubsystem::ApplyView` calls the new
    `AStratUnitActor::SnapToWorldLocation` on the spawn branch and nowhere else.
  - **AND THAT BRANCH NEEDED A PURE MOVE, MEASURED BEFORE IT WAS MADE.** The `Where`
    computation was hoisted above the spawn branch so both the snap and the apply take ONE
    value rather than two computations. It reads `Board` and `View.Hex`; the spawn branch
    writes neither; its comment moved with it.
  - **NO CROSS-BOARD-TELEPORT GUARD, AND THE REASON WAS CHECKED RATHER THAN ASSUMED.**
    `UStratMatchSubsystem::StartMatchInternal` calls
    `UStratMatchSubsystem::TearDownPresentation` **unconditionally**, and that function
    `Destroy()`s every entry of `UnitActors` and empties the map — so after any load or
    reseed every actor is brand new and takes `SnapToWorldLocation`. There is no path on
    which a live actor is asked to slide across a different board.
  - **NO DISTANCE CLAMP EITHER, AND THE ABSENCE IS PRINCIPLED.** A "don't animate a jump
    longer than N" threshold needs a length scale and the only honest one is
    `AStratBoardActor`'s `HexSize`, which this class cannot see and must not — it holds no
    board pointer, by the same discipline that makes `ApplyUnitView` take a world location
    instead of computing one. Any constant here would be a magic number the file cannot
    derive.
  - **`AStratUnitActor::MoveTweenSeconds` SHIPS AT ZERO BY USER DECISION, ON
    `FStratMatchConfig::AiPlaybackStepSeconds`' OWN ARGUMENT RESTATED LOCALLY RATHER THAN
    CITED.** A C++ default is a second place the pace is stated and the designer-facing one
    on the Blueprint is then the one nobody can find; and a non-zero default would change
    the path every existing test runs down. At `<= 0` no offset is written, no tick is
    enabled, and `bStartWithTickEnabled` is false — so an actor whose Blueprint leaves it
    alone never ticks once, not a tick that early-returns. The shipped duration goes on
    `BP_StratUnit`, which is the content lane's. `FStratMatchConfig::AiPlaybackStepSeconds`'
    block now names it as the third pacing number and says where it lives, so a reader
    tuning pace finds all three from one place.
  - **TWO PROSE CORRECTIONS, WRITTEN FLAT, BECAUSE THE CHANGE FALSIFIED SENTENCES NOBODY
    EDITED.** (1) `StratUnitActor.h`'s "NOT IN THIS ROUND" bullet on movement interpolation
    is NARROWED and quoted — what stands is that nothing here may LABEL a position
    difference, and the ordered event list §4.9 part 2 names is still not built; what is now
    built eases this actor's own previous RENDERING to its new one, which is a fact about a
    transform this class wrote and can read back. (2) the header block's heading sentence
    "IT HOLDS NO STATE THE VIEW MODEL DOES NOT ALSO HOLD" became false in the letter and is
    retracted in place — it now reads "NO *BIT*", and the exception is named:
    `AStratUnitActor::TweenStartOffset` and `TweenElapsed` are state no model holds, their
    terminal value is zero, a clock drives them there, nothing reads either to decide
    anything, and neither is a bit. **THE DEFENSIBLE CLAIM IS NOW NARROWER AND IS WRITTEN
    NARROWLY WHEREVER IT APPEARS: the actor transform is a function of the last model alone,
    always and synchronously; the RENDERED position is a function of the last model alone AT
    REST.**
  - **ONE HAZARD WRITTEN INTO THE COMMENT RATHER THAN ASSUMED AWAY.** The start offset is a
    WORLD-SPACE difference applied as a RELATIVE location, which is exact only while the
    actor is unrotated. That holds on every path today — `SetActorLocation` in
    `ApplyUnitView` is this class's only transform write, nothing here sets a rotation, and
    `UStratMatchSubsystem::ApplyView` spawns at `FTransform::Identity` — and it is **not
    enforced anywhere**. The fix if a Blueprint ever rotates a unit is named in the comment:
    `InverseTransformVectorNoScale` on the difference before it is stored.
  - **`FMath::SmoothStep` AND NOT LINEAR, AND NOT A ONE-SIDED EASE-OUT.** C1-continuity at
    BOTH ends is what stops a pop when a tween is replaced mid-flight: the replacement
    begins at whatever velocity the curve has at its own alpha 0, and both a linear ramp and
    an ease-out start at full speed.
  - **THE BRIEF THAT DISPATCHED THIS CARRIED ONE FALSE FIGURE AND IT IS RECORDED RATHER THAN
    QUIETLY CORRECTED.** It said "the three `Stratocracy.StratPlay.T-INT-05` clauses in
    `Source/StratPlay/Tests/StratMatchReconcile.cpp` that read `GetActorLocation()`". That
    file declares **two** T-INT-05 clauses and only **one** —
    `ApplyViewSpawnsMovesAndDestroys` — reads `GetActorLocation()`, at two call sites; the
    other, `NoActorHoldsPresentationBits`, does not read it at all. The design requirement
    the figure was carrying is unaffected, which is why it is a note and not a stop.
  - **[DISCHARGED 2026-09-02, SAME DAY, BY THE ROUTE TWEEN ENTRY'S BUILD AND RUN. RESTATED FLAT
    RATHER THAN DELETED, so a reader arriving by a citation to "uncompiled" lands on what
    replaced it.]** It said:
    RETRACTED> "**WHAT I COULD NOT VERIFY, AND IT IS EVERYTHING A BUILD WOULD HAVE TOLD ME.**
    RETRACTED>  The editor was open (`UnrealEditor.exe`, PID 36876) and `Build.bat` refused
    RETRACTED>  before compiling anything ... so **this entry describes UNCOMPILED CODE**: there
    RETRACTED>  is no compiler verdict, no UHT verdict on the new `UPROPERTY`s, and no suite
    RETRACTED>  run."
    **THIS CODE IS NOW COMPILED AND THE SUITE HAS RUN OVER IT.** `Build.bat` returned
    `Result: Succeeded` with no warnings and `UnrealEditor-StratPlay.dll` linked, so the new
    `UPROPERTY`s have a UHT verdict; a full headless run followed over that binary, and no file
    under any `Tests/` directory was edited. **NEITHER THE COUNT NOR THE VERDICT IS WRITTEN HERE
    AND NEITHER MAY BE — `global.md` is the only file that may carry a live suite count or a
    phase verdict.** Cite the exported report's own `reportCreatedOn` instead, since a re-run
    replaces that file in place.
    **BUT NOTE WHAT THE DISCHARGE DOES NOT REACH: THE FEATURE THIS ENTRY DESCRIBES NO LONGER
    EXISTS AS WRITTEN.** The single-vector chord tween was replaced by the polyline in the entry
    above; `AStratUnitActor::TweenStartOffset` is gone and `FMath::SmoothStep` is no longer the
    profile. Read this entry for the ROOT-COMPONENT design -- `UnitRoot` above `Body`, the
    markers staying on `Body`, the byte-identical `SetActorLocation` -- all of which stand
    unchanged. Its tween-shape paragraphs are history, and the corrections are in the entry
    above rather than restated here.
  - **AND ONE RISK A BUILD ALONE WOULD NOT CATCH.** Changing the native RootComponent of a
    class a Blueprint derives from means `BP_StratUnit` re-parents its component hierarchy on
    load. No test in `Source/StratPlay/Tests/` spawns that Blueprint, so a green suite would
    NOT clear it; the gate is opening `BP_StratUnit` in the editor and confirming `Body`
    still sits under `UnitRoot` with a zero relative transform. That is a content-lane
    observation and is filed as one.

- **2026-09-02, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `fcf64d3`, UNCOMMITTED) — THE ON-SCREEN COMMAND
  BAR, C++ HALF: THE LATCH IS AN INPUT INTENT AND THE DRAWN BIT IS RECOMPUTED EVERY REFRESH,
  WHICH IS WHAT LETS A BUILD BUTTON EXIST WITHOUT FALSIFYING T-INT-05.**
  - **THE GAP, AND IT IS THE PLAYER'S OWN REPORT RATHER THAN AN AUDIT'S.** After playing the
    prototype the user asked for two on-screen controls: a BUILD button that appears once a
    factory they own is selected, and an END TURN button that highlights once every unit has
    used its actions. Every command in this build is a keyboard binding, nothing on screen
    says either exists, and §2.11.5's menu has no visible way out.
  - **THE ONE THING THAT MADE IT MORE THAN WIRING, MEASURED RATHER THAN ASSUMED.** Nothing in
    this project selects a HEX — `FStratSelectionMachine` tracks `SelectedUnitId` and its own
    header says so in terms — and the existing route around that, `ToggleProductionMenu`
    reading `HexUnderCursor` at the instant the key fires, **cannot serve a button, because a
    button click puts the cursor on the button.** That file's own header claimed the opposite:
    that it was `BlueprintCallable` "so a MENU BUTTON, a console command or a gate can drive
    the same path the key does". It is **RETRACTED IN PLACE, QUOTED**, at
    `AStratPlayerController::ToggleProductionMenu`. The reflection was never the problem; the
    HEX SOURCE was, and only for the open arm, so the clause is narrowed and not deleted.
  - **THE DESIGN, IN ONE SENTENCE: `FStratBuildAffordance` HOLDS A HEX AND A BOOL AND NOT
    "IS THE BUTTON VISIBLE".** `StratDecorateCommandBar` looks that hex up in
    `Model.Factories` on every decorate and writes
    `bShowBuildButton = (found && Owner == ViewingSide)`. A factory captured out from under
    the latch, a hot-seat hand-over, a concluded match: the control goes dark on the next
    refresh with **no clear-point code involved**. Staleness is structurally impossible
    rather than dependent on remembering every clear point — `FStratHoverState`'s posture,
    and the one both `UStratMatchSubsystem::IsProductionMenuOpen` and
    `AStratScoreboardHUD::IsProductionMenuWidgetOpen` take in their own headers.
  - **AND THAT IS WHAT SATISFIES T-INT-05 RATHER THAN A CONVENTION.**
    `GetProductionTargetHex` exempts itself from that clause on an EXPLICIT CONDITION — it
    "appears in no view model, and nothing on screen is drawn from it". A BUILD control drawn
    from a focus latch **does not inherit that exemption**, so what it draws is a field on
    `FStratViewModel` and the only route from the latch to the screen is `DecorateViewModel`.
  - **THE TWO FLAGGED ASSUMPTIONS WERE MEASURED BEFORE ANY CODE WAS WRITTEN, AND BOTH HELD.**
    (1) `Stratocracy.StratPlay.T-INT-05.NoActorHoldsPresentationBits`
    (`Source/StratPlay/Tests/StratMatchReconcile.cpp`) **does not census controller members**:
    it drives five decorations through `ApplyView` and compares
    `AStratUnitActor::GetLastAppliedView()` field by field, and grepping the WHOLE FILE for
    `PlayerController`, `TFieldIterator`, `FProperty` and `StaticClass()->` returns **ZERO**
    hits. So a non-`UPROPERTY` `FStratBuildAffordance` on the controller is not reachable by
    it today; the affordance's own header states what the answer is IF that clause ever grows
    a reflected walk. (2) `FStratSelectionMachine::Reset()` **has no caller anywhere in
    `Source/`, including `Tests/`** — declared at its header, defined in its `.cpp`, called by
    nothing. `FStratBuildAffordance::Reset()` was written for parity with it, so it inherits
    that debt, and **the debt is stated at the method** rather than left for a reader to
    discover the parity is with an uncalled thing.
  - **WHAT THE PLAN DID NOT ANTICIPATE, LISTED BECAUSE A SILENT ADDITION IS THE ONE A GATE
    CANNOT SEE.**
    - **THE TWO CENSUS BLOCKS IN `StratViewModel.h` / `.cpp` HAD TO BE AMENDED, AND THE PLAN
      DID NOT NAME THEM.** Those blocks enumerate what KIND of thing a field of this model may
      be — a snapshot mirror, a table read, a selection, and since 2026-09-01 a bridge-query
      copy — and claim the arithmetic count. `FStratCommandBarView` is a **fifth kind**: two
      declared copies plus three booleans FOLDED from fields of the same model and one named
      selector over it. Both blocks now say so. **The arithmetic count did not move**, and the
      distinction that keeps it still is stated: no NUMBER is stored, the count is compared
      against zero inside the decorator and only the boolean survives onto the model, which is
      exactly the shape `StratViewModelLibrary.h` already ruled permissible when it refused a
      count FIELD.
    - **THE §2.8 CONCLUDED-MATCH GATE MOVED RELATIVE TO THE CURSOR READ.** Making the gate part
      of the shared open — so the button inherits it — puts it AFTER `HexUnderCursor` on the
      key path where it used to be before. On a concluded match with the cursor off the board
      the reported reason therefore changed from the concluded-match sentence to `the cursor is
      not on the board`. Both are true and no caller branches on either string, but a clause
      asserting the first would go red on a correct change, so it is recorded at the call site.
    - **`HandleSelectionEvent` HAS NO CANCEL BRANCH TO PUT A `ClearFocus()` IN.** The plan said
      "a `ClearFocus()` in `HandleSelectionEvent`'s Cancel path"; Cancel goes straight to the
      machine. It is an `else if` beside the `HexPrimary` arm, in the same block, ahead of
      `HandleEvent`.
    - **`Observe`'S FIRST OBSERVATION RECORDS AND CLEARS NOTHING**, and the two observation
      fields are `INDEX_NONE` rather than 0 so that "not seen a model yet" is distinguishable
      from "saw turn 0, side 0". Unspecified in the plan; a first observation treated as a
      change would be a clear with no cause.
    - **TWO INPUTS ARE DOCUMENTED RATHER THAN GUARDED AGAINST, EACH WITH THE REASON.**
      `Owner == ViewingSide` latches a NEUTRAL factory on a model whose `ViewingSide` is also
      `INDEX_NONE` — unreachable through `StratBuildViewModel`, which range-checks and refuses,
      so reachable only by a hand-built model, which is exactly what a clause builds. And
      `bEndTurnSuggested` reads TRUE on a model carrying **no units at all**, vacuously; a
      units-count term was rejected because it would be the decorator forming an opinion about
      a model it was handed, and `StratBuildViewModel` "REFUSES RATHER THAN PRODUCING AN EMPTY
      MODEL" in its own words.
    - **THE HUD GAINED EIGHT MEMBERS, NOT THE PLAN'S SIX**, once the `LastPushed…` /
      `bEverPushed` pair the plan itself then names is counted. **The existing debt block was
      AMENDED and no second one opened**, on that block's own rule — and **no count of the
      moving members is restated**, because the SEVEN/ELEVEN sentence has now been overtaken
      twice and a census written inside a growing thing is invalidated by the next thing that
      grows it.
    - **`#include "StratViewModelLibrary.h"` in `StratViewModel.cpp`**, implied by the fold and
      not stated. Same module, no arrow moved, nothing vendored.
    - **THE MONOLITHIC GAME TARGET WAS BUILT TOO**, where the plan's verification named only
      the editor target.
  - **A DEBT, WITH THE CONDITION THAT DISCHARGES IT.** `FStratBuildAffordance::Reset()` has no
    caller, exactly as the method it was written for parity with has none. **Discharged when a
    load or reseed path calls both in one place** — a focus surviving a reseed is a BUILD
    control about a factory on a board that is gone, and the machine's own state is in the
    same position.
  - **AN INSTRUMENT FINDING FROM THIS PASS, RECORDED BECAUSE IT GENERALIZES PAST THIS FILE
    AND PAST THIS PASS: A PROSE CLAIM WRAPPED ACROSS TWO COMMENT LINES IS INVISIBLE TO A
    SINGLE-LINE GREP, AND THE OBVIOUS FIX IS BLIND TO IT TOO.**
    - **THE FAILURE MODE IS A FALSE "ALREADY CLEAN" ON A DEFECT SITTING IN THE FILE.** Two
      summary sentences about `ToggleProductionMenu` were false, and were caught by a human
      READING the prose. My first grep for each of them returned NOTHING on either file — not
      because they were absent but because **each wrapped across two comment lines**, and a
      per-line scan cannot see a sentence that spans one. Had I stopped there I would have
      reported the file clean. Same shape as this tree's existing findings about per-line
      scans missing wrapped signatures, arrived at from the prose side.
    - **THE OBVIOUS FIX IS ALSO DEFECTIVE, AND THAT IS THE HALF WORTH READING.** Collapsing
      whitespace — `re.sub(r"\s+", " ", source)` — does NOT work on comment prose. It
      collapses the newline and **leaves the CONTINUATION LEADER embedded mid-sentence**: the
      `*` of a block comment, the `//` of a line comment. The flattened text then reads
      `... unchanged in * behaviour ...` and still does not match. **I reported a clean scan
      from exactly that instrument earlier in this pass, and it gave the right verdict for
      the wrong reason.**
    - **WHAT ACTUALLY WORKS: STRIP THE COMMENT LEADER PER LINE, THEN FLATTEN.**
      `re.sub(r"^\s*(//+|\*/|/\*+|\*)\s?", "", line)` over each line, joined, then
      whitespace-collapsed. That is the scan any future audit of header prose should use.
    - **MEASURED WITH A CONTROL, AND THE CONTROL MUST ITSELF BE WRAPPED OR IT ONLY PROVES THE
      FILE WAS OPENED.** This is the trap inside the trap: a control phrase that happens to
      sit on ONE line is found by every instrument, including the two that are blind, so it
      certifies nothing about the property under test. The control used here is
      `It still toggles, it still reads the cursor`, which genuinely spans two comment lines
      in `AStratPlayerController::ToggleProductionMenu`'s declaration —
      **per-line 0, naive-flattened 0, leader-stripped 1.** Both weaker instruments are
      silent on a phrase that is demonstrably present.
    - **AND THE CORRECTED SCAN CHANGES AN ANSWER RATHER THAN CONFIRMING EVERY ONE.** Over the
      `AStratPlayerController` pair it FINDS `unchanged in behaviour` in the header, where
      both weaker instruments found nothing — and **that hit is CORRECT**: it is the false
      claim QUOTED INSIDE ITS OWN RETRACTION BLOCK, which is this project's house style for
      retracting in place rather than deleting. The other two subjects are absent on every
      instrument. **A scan that cannot see a quoted retraction could not tell a live claim
      from a retracted one either**, which is the reason this matters beyond the two
      sentences that produced it.
    - **NO CENSUS OF WRAPPED CLAIMS IS STATED HERE, DELIBERATELY.** A count of how many
      wrapped prose claims this tree carries would be a number written inside a growing
      thing, invalidated by the next header anybody edits. What is recorded is the
      INSTRUMENT and its control; the count is whatever the instrument says on the day it is
      run.

  - **WHAT CHANGED, BY FILE.** NEW: `Source/StratPlay/StratBuildAffordance.h` / `.cpp`,
    `Source/StratUI/StratCommandBarWidget.h` / `.cpp`. MODIFIED: `StratViewModel.h`
    (`FStratCommandBarView`, the `CommandBar` field, `StratDecorateCommandBar`'s declaration,
    the census amendment), `StratViewModel.cpp` (the decorator, the include, the census
    amendment), `StratScoreboardHUD.h` / `.cpp` (the eight members and the debt amendment),
    `StratPlayerController.h` / `.cpp` (the member, the accessor, the retraction, three verbs,
    the latch call, the two decorate calls), `StratMatchSubsystem.cpp` (one push in
    `ApplyView`, unconditional and unbranched). **Nothing else:** no `Source/StratRules/` byte,
    no `Data/` byte, no `Source/Stratocracy/` file, no `Tests/` file, no `Content/` asset, no
    `.uproject` entry, **no `.Build.cs`** — `StratUI` already carries private UMG/Slate/
    SlateCore and `StratPlay` already depends on `StratUI`, which is worth stating because a
    new widget class looks like it needs one.
  - **BUILT AND GREEN ON BOTH TARGETS, IN THE COMPILER'S OWN WORDS.** With the editor verified
    closed against a control — `tasklist` returning 92 `svchost` rows and ZERO matching
    `unreal|livecoding`, `netstat -ano` returning zero lines containing `9315` against 37
    `LISTENING` lines — `StratocracyEditor Win64 Development` and the monolithic `Stratocracy
    Win64 Development` each printed `Result: Succeeded` and exited 0, with a case-insensitive
    grep for `warning|error` over each captured build log returning **0**. **The suite was not
    run in this pass and no clause was added**; the clauses this needs are named in the handoff
    and are `strat-test-author`'s. The count and the phase verdict are `global.md`'s and are
    deliberately not restated here.
  - **THE ASSET HALF IS UNBUILT AND THAT IS THE STATE THIS LANDS IN.** C++ supplies fields and
    verbs and nothing else. `CommandBarWidgetClass` is `EditDefaultsOnly`, has no initializer
    and names no `/Game/` path; no `WBP_CommandBar` exists, `BP_StratScoreboardHUD` carries no
    default, and `WBP_ProductionMenu` has no EXIT button. **Unset is LEGITIMATE and logged
    once at Log**, exactly as an unset `GuidanceWidgetClass` is: the `B` key and the End Turn
    key both still work, because the bar is a second route to two verbs and never the only
    one. Measured this session: no `UnrealEditor` process and nothing listening on
    `127.0.0.1:9315`, so the editor lane could not act.

- **2026-09-01, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `f079b9f`, UNCOMMITTED) — W8 ITEM (4), THE
  BUILD PULSE, DRAWING HALF: "DRAWING-ONLY, NO C++" WAS FALSE A SECOND TIME, AND THE SECOND
  REASON WAS NOT THE FIRST ONE.**
  - **THE GAP, AND IT IS A GAP THE RECORD DESCRIBED AS CLOSED.**
    `FStratFactoryView::bBuildPulse` was composed by `FStratBridge::FactoryBuildPulses`,
    mirrored onto the view model by `StratBuildViewModel`, published, reflected — **and read
    by nothing.** Measured, not inferred: before this change the only tree-wide hits for
    `bBuildPulse` outside `Source/StratRules/` were the declaration in `StratViewModel.h`, the
    single assignment inside `StratBuildViewModel` in `StratViewModel.cpp`, and three PROSE
    references in header blocks citing it as a precedent. **No consumer.**
  - **WHY A CONTENT PASS COULD NOT HAVE CLOSED IT, WHICH IS THE PART WORTH READING.**
    `AStratBoardActor` carried exactly THREE `UHierarchicalInstancedStaticMeshComponent`
    overlays — `ReachOverlay`, `TargetOverlay`, `ObjectiveOverlay` — each a
    `CreateDefaultSubobject`, each with one fixed `EditDefaultsOnly` material slot, and its
    own `OverlayMesh` property said so in terms: *"One mesh, three components, three
    materials."* **There is no generic map and no extensible slot**, so `Content/` had nothing
    to bind a fourth meaning to. That is a different cause from the first time this claim
    failed, and both are the same lesson: the record described the SUBJECT of the remaining
    work correctly and its LOCATION wrongly.
  - **THE FIX: THE FOURTH COMPONENT THAT `StratBoardActor.h`'S OWN HEADER BLOCK PREDICTED.**
    That block already said *"A FOURTH MEANING MUST BE A FOURTH COMPONENT and never a third
    use of an existing one"*, so the shape was ruled before it was needed and nothing new was
    decided here. `BuildPulseOverlay` + `BuildPulseMaterial`, beside the ring's pair.
  - **A SET AND NOT ONE HEX, WHICH IS THE OPPOSITE OF `ShowObjective` AND ARGUED RATHER THAN
    COPIED.** The ring takes a single `FIntPoint` precisely so "every objective" cannot arrive
    through the back door — §4.7 authors one objective per seat. The pulse is genuinely per
    factory and several are lit at once in the ordinary case, so a single-hex signature would
    have been the defect here rather than the guard. `ShowBuildPulses(const TArray<FIntPoint>&)`.
  - **THE MATERIAL IS UNSET AND THAT IS THE STATE THIS SHIPS IN.** `BuildPulseMaterial` is
    `EditDefaultsOnly`, has NO initializer and names no `/Game/` path — `ObjectiveMaterial`'s
    block is the model and this one follows it. No pulse material instance exists in
    `Content/` yet and `BP_StratBoard` carries no default. **Unset is LEGITIMATE**: the pulse
    then draws in `OverlayMesh`'s own material, so a lit factory looks like a reach highlight
    — wrong on screen rather than absent from it, which is why it is silent and not logged
    while a missing `OverlayMesh` is logged. Stated at the property, where the next reader
    lands.
  - **NO ANIMATION IS DRIVEN FROM C++, AND THE REASON IS A MEASUREMENT RATHER THAN A
    PREFERENCE.** `MI_Overlay_Objective` — the only member of this material family in the tree
    — is a `MaterialInstanceConstant` with one vector parameter, **zero scalar parameters and
    zero static switches**, measured against the live editor's reflection on 2026-09-01. So
    there is no parameter name for a pulse to drive; code creating a dynamic instance to set
    one would name a parameter that does not exist, which is a silent no-op that reads as a
    broken pulse. **How the pulse pulses is the content lane's decision, in the material.**
    This class turns instances on and off and nothing more.
  - **THE CALL SITE: `UStratMatchSubsystem::ApplyView`, BESIDE THE RING, UNCONDITIONAL IN BOTH
    DIRECTIONS.** `ShowBuildPulses` is called on EVERY refresh, with an empty array when
    nothing pulses — there is no `if (Num() > 0)` guard, because that is the delta-shaped
    thinking that would leave last turn's pulses standing on a turn that lit none. The loop
    tests one already-composed bool and carries across one already-composed hex; it consults
    `bHasBuiltThisTurn`, `bSpawnBlocked`, `Owner` and `ViewingSide` **not at all**, each being
    folded into `bShouldPulse` upstream or deliberately excluded from it there. **This class
    asks no rules question and could not: it holds the model, not the fold.**
  - **THE BOARD-SIDE READ WAS DECIDED OUT LOUD, AND THE DECISION WAS "STATE THE CONSEQUENCE
    ONCE, POINT AT IT TWICE".** `bShouldPulse` inherits `side == activeSide` from
    `strat::canBuildAt`, so **on the opponent's hot-seat turn the pulse is dark board-wide, on
    half of all turns, and that is CORRECT.** It was derivable from the `canBuildAt` expansion
    already in `FStratFactoryBuildPulse::bShouldPulse`'s block, and derivable is not said: a
    reader meeting a dark board first will reach for the unset material, which is the wrong
    place to look on every other turn. **The paragraph was added at `bShouldPulse`, where the
    derivation lives**; `FStratFactoryView::bBuildPulse` and
    `AStratBoardActor::ClearBuildPulses` each got a one-sentence POINTER to it and not a copy,
    because a copy of an argument is a copy that can go stale.
  - **WHAT CHANGED, BY FILE.** `StratBoardActor.h`: `BuildPulseOverlay`,
    `BuildPulseMaterial`, `BuildPulseDrawnHexes`, `ShowBuildPulses` / `ClearBuildPulses` /
    `GetBuildPulseOverlayCount`, and three prose counts stamped rather than re-numbered.
    `StratBoardActor.cpp`: the subobject, its entry in the no-collision loop, the mesh and
    material assignment in `BeginPlay`, the three method bodies. `StratMatchSubsystem.cpp`:
    the collect-and-show block in `ApplyView`. `StratBridge.h` and `StratViewModel.h`: the
    board-side-read paragraph and its two pointers, prose only. **Nothing else:** no
    `Source/StratRules/` byte, no `Data/` byte, no `Source/Stratocracy/` file, no `Tests/`
    file, no `Content/` asset, no `.uproject` entry, no `.Build.cs`.
  - **THREE PROSE COUNTS WENT FALSE AND WERE STAMPED, NOT RE-NUMBERED**, on the header
    block's own stated reason that the count was never the invariant: `OverlayMesh`'s "One
    mesh, three components, three materials", the constructor's "NO COLLISION ON EITHER
    OVERLAY" (a two-era survivor), and the header block's own component enumeration. The
    `BeginPlay` log line naming which highlights will not draw was extended rather than
    stamped — it is a list of subjects, not a count.
  - **A DEBT, WITH THE CONDITION THAT DISCHARGES IT.** `GetBuildPulseOverlayCount` has no
    caller in the tree today. It exists for the clause named in the handoff below, and an
    accessor with no named reader is the shape that rots — the same debt
    `GetObjectiveOverlayCount` took on and discharged. **Discharged when
    `Stratocracy.StratPlay.T-UI-02.BuildPulseOverlayIsDrawnFromTheModelAlone` lands.**
  - **BUILT AND GREEN ON BOTH TARGETS.** `StratocracyEditor` and the monolithic `Stratocracy`
    Game target each printed `Result: Succeeded`, exit 0, with zero errors and zero warnings;
    the Game target relinked `Binaries\Win64\Stratocracy.exe`. The suite was run to a SCRATCH
    `-ReportExportPath` so the canonical `Saved/AutomationReport/` was not overwritten — no
    clause failed and none was skipped. **The count and the phase verdict are `global.md`'s
    and are deliberately not restated here.**

- **2026-09-01, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `f079b9f`, UNCOMMITTED) — W8 ITEM (3), THE
  REACTIVE REPAIR RECEIPT, C++ HALF: THE ARITHMETIC WENT DOWN A LAYER RATHER THAN SIDEWAYS,
  AND THE FILE THAT FORBADE IT IN ADVANCE WAS OBEYED RATHER THAN AMENDED.**
  - **[STAMPED 2026-09-01, LATER THE SAME DAY. THIS ENTRY'S HEADER READ `UNCOMMITTED and —
    read this before citing anything below — **UNBUILT**` AND THAT IS NO LONGER TRUE. IT IS
    BUILT.** The bullet directly below is retained as the record of the pass that failed, not
    as a live statement about this tree, and its own opening sentence is retracted:
    RETRACTED> "THE BUILD DID NOT RUN AND NOTHING HERE IS VERIFIED."
    **WHAT THE COMPILER SAID ON THE PASS THAT DID RUN: NOTHING.** With the editor genuinely
    closed — verified with a control, `tasklist` returning 93 `svchost` rows and ZERO matching
    `unreal|livecoding`, and `netstat -ano` returning no line containing `9315` —
    `Build.bat StratocracyEditor Win64 Development -project=… -waitmutex` ran 87 actions and
    printed `Result: Succeeded`, exit 0, in 46.83 s. **NOT ONE LINE OF THE CHANGE HAD TO
    MOVE: zero errors, zero warnings, no UHT diagnostic, and the one-subtraction resolution
    described below stands exactly as written.** `Module.StratUI.gen.cpp` and
    `Module.StratPlay.gen.cpp` both compiled, so the two new reflected shapes —
    `EStratReceiptKind::UnitRepair` and `FStratUnitRepairView` — went through the header tool.
    The monolithic `Stratocracy` Game target was then built as well and also printed
    `Result: Succeeded`, exit 0, relinking `E:\MultiAgent\Stratocracy\Binaries\Win64\Stratocracy.exe`
    — which matters for this change specifically, because it touches `StratBridge`, and a
    modular editor build structurally cannot observe the `LNK2005` collision class that
    directory's vendored shims exist to prevent. `-NoHotReloadFromIDE` was passed on NEITHER
    target, per `.agents/ue-project-context.md`: it is for linked worktrees, and on this tree
    it only moves the failure later.]**
  - **THE BUILD DID NOT RUN AND NOTHING HERE IS VERIFIED — HISTORY, SUPERSEDED BY THE STAMP
    ABOVE; TRUE OF THE 2.73 s PASS IT DESCRIBES AND OF NO LATER ONE.** The editor was
    reported closed in the dispatch brief and was OPEN — measured, not inferred:
    `Get-CimInstance Win32_Process` returned PID 44064,
    `"…/UnrealEditor.exe" "E:\MultiAgent\Stratocracy\Stratocracy.uproject"`, alongside
    `LiveCodingConsole` PID 9348. `Build.bat StratocracyEditor Win64 Development` therefore
    failed in 2.73 s with `Unable to build while Live Coding is active. Exit the editor and
    game, or press Ctrl+Alt+F11 if iterating on code in the editor or game` /
    `Result: Failed (OtherCompilationError)`. **That is an EARLIER failure than the
    `LNK1104`/"cannot open for writing" one `.agents/ue-project-context.md` describes, and the
    same cause** — the Live Coding mutex catches it before any compile action starts, so
    nothing in this change has been through UHT. `-NoHotReloadFromIDE` was NOT passed: it
    defeats that mutex and would have moved the failure to the link step on this tree, which
    that document records as measured on 2026-08-22. **Neither target was built; the suite was
    not run; no report stamp is cited and the dead citation elsewhere in this file was left
    alone.**
  - **THE GAP.** §2.11.6's `+[N] HP — repaired`. The game already repairs — `strat::openTurn`
    (`Replay.good.cpp`) builds the `RepairSubject` vector, derives `onOwnedObjective` and
    `enemyAdjacent` itself, calls `strat::applyStartOfTurnRepair` and adds each
    `RepairApplied::amount` to `GameUnit::hp`. **And then discards the vector: `openTurn`
    returns `void`.** So the module's own amount reaches NO caller through any channel.
    `Driver.good.cpp`'s `openActiveTurn` pushes it into a string list, but that is the debug
    driver over `Session` and the bridge never runs it. The brief's third hoped-for route —
    "a channel where the module's own `RepairApplied::amount` reaches the engine without
    anyone subtracting" — **does not exist in the vendored tree, and closing it is a rules
    change.** See the change request at the foot of this entry.
  - **THE OBSTACLE AND HOW IT WAS RESOLVED.** `StratTransientReceipts.cpp` opens with a
    standing claim that there is exactly ONE subtraction in it — the `KillFame` amount — and
    that the header's argument for it "does not extend to a second one". **An HP delta is that
    second subtraction and the claim is RIGHT to forbid it there.** The `KillFame` argument
    turns on `fameCombat` being a monotone accumulator with one cause, so a rise between two
    refreshes has exactly one meaning. **HP has none of those properties**: it falls under
    §2.6, rises under §2.7, and arrives fresh at `hpMax` on a §2.7 spawn — a rise measured
    between two PRESENTATION REFRESHES has three possible causes and that layer cannot say
    which it saw.
    - **So the subtraction was taken one layer down, in `FStratBridge`, bracketed around the
      SINGLE `strat::applyCommand` call that opens a turn.** Inside `applyCommand`'s `EndTurn`
      arm the only statement in the rules module that writes `GameUnit::hp` is `openTurn`'s
      repair loop — `strat::endTurn` and `strat::beginTurn` mutate no unit, `accrueIncome`
      moves Fame, `captureTick` moves ownership, and `resolveBuilds`, the one thing that
      creates a unit, is called from the `Build` arm and nowhere else. **A rise across that
      bracket IS a §2.7 repair by construction, not by inference** — the property the
      presentation layer cannot have, at the only layer that can. Verified by reading
      `Replay.good.cpp`'s `openTurn` and `applyCommand` and `Turn.good.cpp`'s `beginTurn` /
      `endTurn`; a re-vendor that adds a second HP writer to that arm invalidates it.
    - **The forbidding paragraph was NOT edited.** It still reads exactly as it did and is
      still TRUE of that file: the repair arm's `Amount` is copied, character for character
      with the income arm's verbatim assignment. A note was APPENDED recording that the check
      its own last sentence demands was actually performed, what it caught, and where the
      arithmetic went instead.
    - **The rejected alternative was the one the brief listed first** — argue that a second
      subtraction is justified and restate the claim to match. It was killed by the
      three-causes point above: no restatement makes an unbracketed HP delta attributable, so
      it would have been a weaker claim covering a defect rather than a stronger one.
  - **WHAT CHANGED, BY FILE.** `StratBridge.h`: plain struct `FStratRepairApplication`
    (`UnitId`, `Side`, `HpBefore`, `HpAfter`, `HpMax`, `Amount`) and
    `RepairsAtTurnOpen(TArray<...>&) const`, plus the retained member
    `RepairsAtLastTurnOpen`. `StratBridge.cpp`: the `StratRepairObservation` namespace, an
    `EndTurn` bracket in `Submit` beside the existing Attack one, the accessor, and clears in
    `LoadDefinitions`, `LoadScenarioFromFile` and `ReplayLog`. `StratTransientReceipts.h`:
    `EStratReceiptKind::UnitRepair`, `FStratReceiptView::UnitId`, the reflected mirror
    `FStratUnitRepairView`, and a four-argument `StratDecideTransientReceipts` beside the
    old three-argument form. `StratTransientReceipts.cpp`: rule 5, copy-only.
    `StratMatchSubsystem.cpp`: one bridge read and a field-for-field mirror in `ApplyView`.
    **Nothing else:** no `Source/StratRules/` byte, no `Data/` byte, no `Source/Stratocracy/`
    file, no `Tests/` file, no `.uproject` entry, no `.Build.cs`.
  - **THE THREE DESIGN QUESTIONS, RESOLVED.**
    - **VIEWER-RELATIVITY: none, deliberately.** §2.7 repairs the ACTIVE side alone, so on a
      hot-seat opponent's turn every entry is theirs; a `ViewingSide` filter here would make
      the arm silent on exactly half of all turns. `FStratReceiptView::Side` is already
      declared "an index …, never a you/enemy answer" precisely so a receipt can be about the
      other side, `EStratTurnBanner` is the one place the mapping happens, and `IncomeRate`
      already fires for whichever side is to move. **Whether a hot-seat screen DRAWS the
      opponent's floaters is a drawing decision with both facts in hand; suppressing them in
      a decider would take that decision away and hide it.**
    - **THE JOIN: unit id, via `strat::findUiUnitView`.** An INDEX join reads one unit's HP
      under another's identity the day either order moves, and nothing on screen distinguishes
      a floater on the wrong unit from a correct one. A HEX join is the shape that works for a
      FACTORY and cannot work here — a factory's hex is its identity, a unit's hex is a field
      that moves, and two snapshots either side of a turn boundary are where it is least
      trustworthy. Units absent from either snapshot are skipped in silence; neither case is
      reachable across this bracket today and both are handled, because the alternative is
      code whose correctness rests on a rules fact stated in a comment.
    - **ZERO IS NOT EVIDENCE, and the receipt covers the POSITIVE CASE ONLY.** An absent entry
      cannot be told apart from full HP, from a unit off an owned objective, or from a repair
      §2.7 BLOCKED by an adjacent enemy. §2.11.6's `enemy adjacent` one-shot needs exactly that
      third reading, **was not attempted, and remains blocked upstream** on the same missing
      module-side predicate that got the eligibility pip cut. Stated on the enum arm, on the
      `Amount` tooltip and in the header block, so no drawing layer can reach the field without
      passing the warning.
  - **THE COPY OBLIGATION IS ON A PIN, NOT ONLY IN A HEADER BLOCK.** `+[N] HP — repaired`,
    em dash, `[N]` = `FStratReceiptView::Amount` — written into the `Amount` `UPROPERTY`
    tooltip and into the `UnitRepair` enumerator's, which is what a UMG author actually meets
    in the editor. It also records that amount-voiced copy is CORRECT on this arm (unlike
    `IncomeRate`) and that nothing may be drawn off an absence.
  - **A DEBT AND A GAP, WRITTEN DOWN RATHER THAN OWNED QUIETLY.** (a) `ReplayLog` reaches
    `strat::replayLog` directly and never passes through `Submit`, so a log loaded in a fresh
    process replays its turn boundaries unobserved; the record is CLEARED there rather than
    stale-filled. **This is the same gap `Submit`'s own block already records for the
    `STRAT-COMBAT` family**, with the same fix and the same reason it is not taken.
    **Discharged when** a phase needs replayed turn boundaries observed. (b) The first turn,
    opened inside `strat::seedFromScenario`, is not measured — no earlier reading exists and
    every unit is at `hpMax` on a fresh seed.
  - **CHANGE REQUEST, UPSTREAM, NOT TAKEN HERE.** `strat::openTurn` discards
    `applyStartOfTurnRepair`'s return. Publishing it — an out-parameter, or a return type —
    would let the bridge MIRROR the module's own `RepairApplied::amount` and delete the
    subtraction described above entirely. Behaviour: `openTurn` reports what it healed.
    Acceptance: T-TURN-08 / T-TURN-09's existing subjects. Upstream file:
    `E:\MultiAgent\stratocracy-crew` → `cpp_reference/Replay.good.cpp` and `Replay.h`.
    **Not required for this receipt to be correct**, only for it to be a mirror instead of a
    bracketed measurement.

- **2026-09-01, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `cada741`, UNCOMMITTED at the time of writing --
  the user commits) -- W8 ITEM (4), THE BUILD PULSE, C++ HALF: THE CONDITION IS NOW A BRIDGE
  ANSWER, BECAUSE ITS TWO HALVES REACHED THE ENGINE THROUGH DIFFERENT DOORS AND NOTHING COULD
  HOLD BOTH AT ONCE.**
  - **THE GAP, STATED AS THE GDD STATES IT.** §2.11.5: *"When any unit is affordable and the
    factory has not built this turn, the factory tile shows a small `BUILD` pulse ... The second
    half of that condition is read rather than inferred: the factory's own `hasBuiltThisTurn`."*
    The second half was already a snapshot field and already mirrored onto
    `FStratFactoryView::bHasBuiltThisTurn`. The first half existed ONLY as per-row
    `strat::UiBuildOption::affordable`, behind `FStratBridge::BuildOptions`. **The two could not
    be joined by any existing caller**, because the only engine-side route to the buildlist is
    `UStratMatchSubsystem::RefreshProductionMenu`, which its own block declares ALL-OR-NOTHING
    against a single `ProductionMenu` slot -- so a tile polling it per factory would clobber an
    open menu. That is the specific reason this is C++ and not an editor pass.
  - **WHAT CHANGED, BY FILE.** `StratBridge.h` gains the plain struct `FStratFactoryBuildPulse`
    (`Hex`, `bAnyUnitAffordable`, `bHasBuiltThisTurn`, `bBuildAvailable`, `UnavailableReason`,
    `bShouldPulse`) and the method `FactoryBuildPulses(int32 Side, TArray<...>& OutPulses)`;
    `StratBridge.cpp` implements it; `StratViewModel.h` gains
    `FStratFactoryView::bBuildPulse` and the census amendment that field forces;
    `StratViewModel.cpp` asks the bridge once per model and copies one bool per factory.
    **Nothing else was touched:** no `Source/StratRules/` byte, no `Data/` byte, no
    `Source/Stratocracy/` file, no `Tests/` file, no `.uproject` entry, no `.Build.cs`.
  - **THE DESIGN QUESTION WAS RESOLVED AGAINST THE MODULE, AND THE ANSWER IS ONE-DIRECTIONAL.**
    The question dispatched with this task was whether `available && any(affordable)` ALREADY IS
    the GDD's condition. It is not, and the relationship is an implication rather than an
    equality:
    - **`available` IMPLIES `!hasBuiltThisTurn`.** `strat::canBuildAt` is
      `running && phase == Actions && side == activeSide && !hasBuiltThisTurn(s, factory)`;
      `strat::uiBuildOptions` consults `canBuildAt` FIRST and sets `available` false on its
      refusal; and `UiFactoryView::hasBuiltThisTurn` is filled from that same
      `hasBuiltThisTurn(t, o->hex)` -- the same predicate over the same turn state and the same
      hex, taking no side argument. So availability CONTAINS the build allowance.
    - **THE CONVERSE FAILS, on nine further reasons `uiBuildOptions` spells out** -- no match
      running, the match is over, the turn has not begun, start-of-turn repair not applied, not
      the active side, no objective at that hex, the factory is not held by this side, the
      objective has no terrain row, not a build point, a pending build already holds the slot.
      **THE FALSIFIER, so nobody has to take it on the prose:** an ENEMY-HELD factory that has
      not built this turn, with the asking side able to afford a unit, has `hasBuiltThisTurn`
      false and `available` false. **The GDD sentence read literally lights a `BUILD` pulse on
      the opponent's tile there.**
    - **SO `bShouldPulse` IS `bBuildAvailable && bAnyUnitAffordable`, A DECLARED NARROWING OF
      THE GDD SENTENCE RATHER THAN A TRANSCRIPTION OF IT**, and it is dark on every board the
      GDD says it must be dark on. `bHasBuiltThisTurn` is CARRIED UNFOLDED beside it -- which is
      how the GDD's "read rather than inferred" is satisfied by an observable value instead of
      by a comment -- and is DELIBERATELY ABSENT FROM THE EXPRESSION: by the implication above
      `bBuildAvailable && !bHasBuiltThisTurn` is EQUAL TO `bBuildAvailable` on every board, so
      writing the redundant term would assert a difference that does not exist. This repo has
      twice recorded a clause asserting a difference between two provably identical expressions;
      that is why the equality is stated here rather than left for a reader to notice.
  - **NO AFFORDABILITY IS RECOMPUTED ANYWHERE ABOVE THE MODULE, WHICH IS THE CONSTRAINT
    `BuildOptions` HANDED DOWN.** No price is compared to a purse in `StratBridge.cpp` or in
    `StratViewModel.cpp`. The single operation is an OR over `affordable` booleans the module
    already decided, taken in `StratBridge.cpp` and DECLARED DERIVED at
    `FStratFactoryBuildPulse::bAnyUnitAffordable` rather than dressed as a mirror. `available`
    is read off row zero, which is sound because `Ui.h` DECLARES it row-invariant ("IT DOES NOT
    VARY BY ROW") and `uiBuildOptions` computes it once above its own loop -- not because the
    rows happened to agree.
  - **PLURAL AND NOT PER-HEX, AND THE REASON IS A CALL COUNT RATHER THAN A PREFERENCE.**
    `FactorySpawnBlockedAt` answers ONE hex and projects a whole `UiSnapshot` to do it, and is
    called once per production-menu refresh. This question's caller is `StratBuildViewModel`,
    which runs on every presentation refresh and therefore **at mouse-move rate** --
    `AStratPlayerController`'s hover path says so at its own refusal log ("this fires at
    mouse-move rate"). A per-hex form called in that loop would project the snapshot once per
    factory: five projections per refresh on the shipped scenario where one is needed. The
    plural form projects ONCE and asks `BuildOptions` per factory. **The singular form is
    deliberately not also declared**: it would have no caller, and a bridge method with no
    caller reads as built when it is only compiled.
  - **THE VIEW-MODEL ROUTE IS THE POINT, NOT A CONVENIENCE.** The pulse lands on
    `FStratFactoryView::bBuildPulse` so that T-INT-05 stays true in the shape it is written --
    the screen is rebuildable from the view model alone -- and so that no tile composes the
    condition in the layer T-UI-03 governs. It also keeps the pulse entirely off
    `RefreshProductionMenu`'s slot: **nothing in this pass reads, writes or touches
    `ProductionMenu` or `ProductionMenuHex`.** The join between the pulse array and the factory
    loop is BY HEX and not by index: both sequences come from the same `Snapshot.factories` in
    the same canonical order and a positional read would agree today, which is exactly why it
    was refused -- it would agree silently until one order moved, and a pulse drawn on the wrong
    factory is a defect nothing on screen distinguishes from a correct one.
  - **THE CENSUS IN `StratViewModel.h` MOVED AND IS AMENDED IN PLACE RATHER THAN REWRITTEN.**
    That block says every field of the model is a snapshot mirror, a table read or a selection
    over the model's own fields. `bBuildPulse` is a FOURTH KIND -- a value copied from a bridge
    QUERY -- and the amendment names it as such rather than leaving a reader to classify it as
    the mirror it is not. **The arithmetic count did NOT move:**
    `FStratBuildOptionView::Shortfall` is still the one arithmetic exception and is still
    outside the model.
  - **BUILD.** `Build.bat StratocracyEditor Win64 Development` -> `Result: Succeeded`
    (107 actions, 58.57s). `-NoHotReloadFromIDE` was NOT passed: this is the main tree, not a
    worktree, and the editor was closed (verified: no `UnrealEditor.exe` task). **The MONOLITHIC
    branch was built too, deliberately** -- `Build.bat Stratocracy Win64 Development` ->
    `Result: Succeeded`, `Output binary: E:\MultiAgent\Stratocracy\Binaries\Win64\Stratocracy.exe`.
    The vendored-shim mechanism recorded in the 2026-08-31 entry below is the reason a
    `StratBridge` change has to be built on both link types before it is reported green, and the
    editor build cannot see the failure that mechanism prevents.
  - **ONE CLAUSE IS RED AND IT IS NOT THIS PASS'S, MEASURED WITH A CONTROL RATHER THAN
    ASSERTED.** `Stratocracy.StratPlay.T-INT-05.RefreshFromMachineDecoratesWithNoRegistration`
    fails in the exported report at `reportCreatedOn 2026.09.01-19.41.17`. It was NOT diagnosed
    from the diff: the four edited files were `git stash`ed, the editor target was REBUILT from
    `cada741`'s bytes, and the clause was run alone -- **it failed with byte-identical error
    text on both sides**, `Expected 'the controller's own refresh decorated with nobody
    registered (it returned false: 'scoreboard refresh refused: there is no scoreboard widget to
    refresh')' to be true.` and the field-for-field guidance mismatch beneath it. The stash was
    then popped and both targets rebuilt, so the binaries in this tree are the modified sources'.
    **It is a pre-existing red at `cada741`, in a lane that is not this one** -- see the HANDOFF
    line below and `global.md`, which is the only file that may carry the live figure.
  - **WHAT THIS PASS DID NOT DO, so none of it is mistaken for done.** No widget, no tile, no
    material and no `Content/` byte -- the editor half of item (4) is a separate later batch and
    was explicitly not this task. No test: `Tests/` is `strat-test-author`'s lane, and the
    clauses this needs are named in that agent's brief rather than written here. **No clause in
    this tree currently binds to `bShouldPulse`, `bBuildPulse` or `FactoryBuildPulses`**, which
    is the honest statement of this pass's coverage and is why the narrowing above is recorded
    as a claim a test must still pin rather than as one it already does.
  - **THE DEBT.** The narrowing is engineering judgement over an under-specified sentence, not a
    ruling. If §2.11.5's pulse is later RULED to be the GDD sentence literally -- pulsing on any
    factory the viewing side could afford to build at, whoever holds it -- **the change is one
    line, at `FStratBridge::FactoryBuildPulses`, and no caller moves**, because both halves are
    already on `FStratFactoryBuildPulse` and nothing downstream recomputes either. That is the
    condition that discharges this debt.

- **2026-08-31, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, on `master` in the main
  tree `E:/MultiAgent/Stratocracy`, base commit `2592276`, UNCOMMITTED at the time of writing --
  the user commits) -- THE WIN64 GAME TARGET LINKS, BECAUSE THE VENDORED SHIMS NOW EXIST ONLY
  WHERE THE DLL BOUNDARY THEY WERE WRITTEN AGAINST EXISTS.**
  - **THE FAILURE, AND WHY IT WAS INVISIBLE FOR THE WHOLE PROJECT.** `Build.bat Stratocracy
    Win64 Development` compiled every action and then died at link: **110 x `LNK2005`** followed
    by `LNK1169`, exit 6, e.g. *"`Ai.strat.cpp.obj : error LNK2005: "bool __cdecl
    strat::buildPriorityLess(struct strat::UnitDef const &, struct strat::UnitDef const &)"
    already defined in Ai.good.cpp.obj`"*. The twelve vendored translation units are compiled
    TWICE -- once as the `StratRules` UBT module, which `Stratocracy.Build.cs` depends on for
    T-DATA-05's `Combat.h`, and once as `Source/StratBridge/Vendored/*.strat.cpp`. **The editor
    target is MODULAR, so the two copies land in two DLLs and never meet a linker together; a
    Game target is MONOLITHIC and they meet in one `Stratocracy.exe`.** Nothing regressed --
    the duplication has been there since the shims were written, and only a link type this tree
    had never built could see it. **UBT labels the run `Result: Failed (OtherCompilationError)`
    and that label is wrong in the way that matters:** it is a pure link failure and reading it
    as a compile diagnostic sends the next reader to the wrong file.
  - **WHAT CHANGED, BY FILE.** `StratBridge.Build.cs` gains one C# expression --
    `bool bCompileVendoredRulesHere = Target.LinkType != TargetLinkType.Monolithic;` -- which
    emits `STRAT_VENDORED_RULES_IN_BRIDGE` as a `PrivateDefinition` **and**, on the false branch,
    adds `StratRules` to `PrivateDependencyModuleNames`. Each of the twelve
    `Vendored/*.strat.cpp` wraps its single `#include` of the vendored `.good.cpp` in
    `#if STRAT_VENDORED_RULES_IN_BRIDGE`. **Nothing else was touched:** no `Source/StratRules/`
    byte, no `Source/Stratocracy/` file, no target file, no `.uproject` entry.
  - **THE TWO HALVES COME FROM ONE EXPRESSION, AND THAT IS THE POINT OF THE MECHANISM.** UBT's
    own `IS_MONOLITHIC` would have answered the `#if` without a new definition, and is refused:
    it would leave the dependency decision in one file and the compile decision in twelve
    others, agreeing only by inspection. Because the `#if` reads a definition the SAME
    expression emits, it is not possible to switch one half and leave the other. Each shim also
    `#error`s on an **absent** definition rather than letting it read as 0 -- an absent define
    would otherwise present as `LNK2019` in the modular build, which is the exact error class
    the shims exist to prevent and the worst possible disguise for their own misconfiguration.
  - **WHAT WAS REJECTED.** Making `Stratocracy.Build.cs`'s `StratRules` dependency conditional
    instead, keeping the shims as the single copy in both link types. It would work, and it is
    refused on blast radius rather than on correctness: it edits a module this milestone does
    not own, and it would have to re-supply `Combat.h` to the parity harness as a bare include
    path -- an include edge with no module arrow behind it, in a module whose tests name
    `strat::` types and where the distance between naming and calling is one keystroke.
  - **THE DEPENDENCY IS PRIVATE, DELIBERATELY.** No public header of `StratBridge` names a
    `strat::` symbol -- `StratBridge.h` is the only header including the vendored headers at all
    and is forbidden to any header declaring a `UCLASS`/`USTRUCT`/`UENUM` -- so a public arrow
    would re-export a link-line entry no dependent could use. `PublicIncludePaths` is unchanged
    and still carries `StratRules` in both link types.
  - **THE SWITCH WAS MEASURED IN THE OBJECT FILES, NOT INFERRED FROM A GREEN LINK.** Byte-grep
    for the mangled `buildPriorityLess@strat` **with its positive controls shown**, because an
    absence proves nothing until the instrument is seen to speak (a first attempt via `dumpbin
    /SYMBOLS` returned six lines of output for every input and was discarded as mute):
    `UnrealGame/.../StratBridge/Ai.strat.cpp.obj` = **0 hits, 21,763 bytes**;
    `UnrealGame/.../StratRules/Ai.good.cpp.obj` = 1 hit, 529,306 bytes;
    `UnrealEditor/.../StratBridge/Ai.strat.cpp.obj` = 1 hit, **591,106 bytes**;
    `UnrealEditor/.../StratRules/Ai.good.cpp.obj` = 1 hit, 558,422 bytes. So the Game target has
    exactly one definition and it is the vendored module's own; the Editor target still has two,
    in two DLLs, unchanged.
  - **THE GUARD WAS PROVED LOAD-BEARING BY BREAKING IT, ON THIS TREE, IN PLACE.** A green link
    is consistent with a guard that does nothing, so one shim -- `Hex.strat.cpp` alone -- had its
    `#if STRAT_VENDORED_RULES_IN_BRIDGE` replaced by `#if 1` and the Game target rebuilt: **9 x
    `LNK2005`** naming `strat::axialToOffset`, `hexDistance`, `hexEqual`, `hexLess`, `inBounds`,
    `neighborCandidate`, `neighbors`, `offsetToAxial` and `sortCanonical`, each *"already defined
    in `Hex.good.cpp.obj`"*, then `LNK1169`. **In place and not in a copy**, because a copied
    tree's cached `Intermediate/Build` resolves the ORIGINAL sources and the mutant is a silent
    no-op. Restored by file copy and verified by md5 (`1e820aee58d633e0de63411ca6aef2a3`, equal
    to the pre-mutation backup) plus a zero-hit grep for the mutation marker; the Game target
    then rebuilt `Result: Succeeded` and the Editor target after it.
  - **BOTH TARGETS BUILT `Result: Succeeded` on this tree, Game first and Editor after**, and
    the Editor was rebuilt BEFORE the suite ran so no stale binary could report old clause names.
    **The suite figure is `global.md`'s to state and is not restated here**; the run this lane
    read was exported to `Saved/AutomationReport/index.json` (`utf-8-sig`) with
    `reportCreatedOn` **`2026.09.01-03.15.35`** -- a UTC stamp, which is why this entry is dated
    a day earlier than it reads.
  - **`Source/StratRules/` IS BYTE-UNTOUCHED, and here is how that was checked rather than
    asserted.** `git status --porcelain -- Source/StratRules Data` is empty, and the md5 over
    `git hash-object` of every file in the directory is `3e5c215537135fc4f911c4be24ed8def` both
    before and after the change -- taken through `git hash-object` rather than a worktree diff
    because `core.autocrlf=true` makes a plain byte compare cry drift on identical blobs.
  - **WHAT THIS DOES NOT CLAIM.** That the Game target has never been built here before is the
    dispatching brief's impression and this lane did not measure it, so it is recorded as
    **unverified**. That the Game target RUNS is likewise unmeasured -- this closes a link
    failure, and nothing here launched `Stratocracy.exe` or drove a packaged build. **Discharged
    by** a packaging or cook pass that actually starts the produced binary.
    - **DISCHARGED 2026-09-01, BY A PACKAGING PASS THAT STARTED THE PRODUCED BINARY. The text
      above stands as written and is not edited; this stamp says what it owed and what paid it.**
      A Win64 **Development** package was built with `RunUAT BuildCookRun`
      (`-clientconfig=Development -build -cook -stage -pak -archive`), archived to
      `E:/MultiAgent/Stratocracy-Package`, and the produced binary was launched into
      `/Game/StratMaps/Lvl_FerrumCrossing`. It stayed up 60 seconds and its own log recorded,
      verbatim: *"LogStratPlay: Match live: seeded from
      'E:/MultiAgent/Stratocracy-Package/Windows/Stratocracy/Data/ferrum_crossing.json' (first
      side 0), drawn for side 0, 99 hexes and 10 units on screen."* **The resolved path is INSIDE
      the package**, which is the load-bearing half -- it read the STAGED data, not the source
      tree, so this is a packaged run and not the editor's content reached from a different
      executable. The log contained zero errors, warnings or ensures.
    - **WHAT THE PACKAGING PASS STILL DOES NOT CLAIM.** A **Shipping** package was also built and
      launched and stayed up 60 seconds, but **its scenario load is UNVERIFIED and must not be
      read as covered by the paragraph above**: Shipping compiles logging out. The control for
      that absence -- because a silence proves nothing until the instrument is shown able to
      speak -- is that the same probe returns a real log for the Development build, and Shipping
      creates an EMPTY `Logs/` directory under `%LOCALAPPDATA%/Stratocracy/Saved/`. So the
      missing log is stripped logging rather than a failed run, and that is all it is. **Would be
      discharged by** a Shipping-configuration run whose scenario load is observed through some
      channel logging does not strip.
    - **PROVENANCE, SO THE STAMP IS NOT MISREAD AS THIS LANE'S OWN EXECUTION.** This lane did not
      run `RunUAT` or launch either binary; the measurements above were made outside it and
      delivered to it, and this entry is the record rather than the run. A reader wanting to
      re-execute has the exact switches and the archive path above.
  - **A DEBT THAT RIDES WITH THIS, AND HALF OF IT WAS ALREADY DISCHARGED BY SOMEONE ELSE WHILE
    THIS LANE WORKED.** No automation clause can cover this: the defect is a property of a link
    type the suite never builds, so the only honest net is CI building the Game target. **That
    step is in this tree already** -- an uncommitted `+54` in `.github/workflows/build-and-suite.yml`,
    NOT this lane's write and not described further here, whose owner records it in their own
    file. What remains open is the other half: **discharged by** that step being seen to go RED
    on a tree with the `#if` removed, since a CI step nobody has watched fail is a step whose
    subject is unproven.
    - **DISCHARGED 2026-09-01: THE STEP HAS NOW BEEN WATCHED TO FAIL.** GitHub Actions run
      **33469897876**, a `workflow_dispatch` on throwaway branch `probe/ci-game-target-red`
      (commit `b5e4102`, since DELETED locally and on the remote; `master` was never touched).
      That branch carried exactly one mutation, the same one this entry's in-place proof used:
      in `Source/StratBridge/Vendored/Hex.strat.cpp`, `#if STRAT_VENDORED_RULES_IN_BRIDGE`
      replaced by `#if 1`. Per-step outcomes were `success` for *build StratocracyEditor*,
      `success` for *run the automation suite*, `success` for *gate the exported suite report*,
      and **`failure` for *build the Stratocracy Game target***, whose own output was
      *"Hex.strat.cpp.obj : error LNK2005: "void __cdecl strat::axialToOffset(...)" already
      defined in Hex.good.cpp.obj"* -- 9 x `LNK2005` over `axialToOffset`, `hexDistance`,
      `hexEqual`, `hexLess`, `inBounds`, `neighborCandidate`, `neighbors`, `offsetToAxial` and
      `sortCanonical` -- then *"Stratocracy.exe : fatal error LNK1169: one or more multiply
      defined symbols found"*, `Result: Failed (OtherCompilationError)`, and a `Build.bat` exit
      code of 6 for the Game target.
    - **THE THREE GREEN ROWS ARE THE LOAD-BEARING HALF OF THAT RUN, NOT THE RED ONE.** The editor
      built, the suite ran and the report gate passed **while the Game target was unlinkable**,
      because `STRAT_VENDORED_RULES_IN_BRIDGE` is 1 in a modular build and the mutant is a
      literal no-op there. That is the blind spot this entry argued for, DEMONSTRATED instead of
      argued: a step that reddens while every other step in the same run stays green is a net
      catching something no existing net could see. **Had the suite reddened too, the step would
      have been redundant** -- and the red row alone, without the three green ones, would not
      have distinguished a new net from a duplicate of one already standing.
    - **WHAT RUN 33469897876 DID NOT PROVE, AND MUST NOT BE READ AS CLOSING WITH IT.** The
      `#error` arm of each shim is **still unfalsified**. That mutant exercised the `#if` only;
      it never removed the `PrivateDefinitions.Add` call, which is the ONLY condition that fires
      the `#error`. Two consecutive `strat-integration-reviewer` passes said so explicitly.
      **Discharged by** a build over a tree with that `PrivateDefinitions.Add` line removed, seen
      to stop on the `#error` itself rather than on the `LNK2019` the `#error` exists to
      forestall.
    - **AND THE `+54` ABOVE IS NOW STALE -- STAMPED HERE RATHER THAN CORRECTED IN PLACE.** That
      CI step is no longer uncommitted: it is **committed in `7713c6c`**, and it is now `+60`,
      not `+54`. It grew after the sentence above was written, by the `id: build_editor` line,
      the `if:` condition, and a correction to a false historical claim. It is still NOT this
      lane's write and is still recorded by its owner in their own file -- see `global.md`. A
      reader chasing a `+54` in `.github/workflows/build-and-suite.yml` will find nothing that
      matches it, which is why the figure is stamped instead of left standing.
    - **PROVENANCE, AND WHICH HALF THIS LANE RE-MEASURED RATHER THAN TOOK ON DELIVERY.** This
      lane did not push `probe/ci-game-target-red`, did not dispatch the workflow and did not
      delete the branch; that run was made outside this lane. **The step outcomes above were
      then re-read here from the GitHub API rather than transcribed from the delivery** --
      `gh run view 33469897876 --json ...` returns `headBranch` `probe/ci-game-target-red`,
      `headSha` `b5e41022aabb532e3d753cff8ce06e38b21c3736`, `event` `workflow_dispatch`,
      `conclusion` `failure`, and exactly one `failure` among fourteen steps, on *build the
      Stratocracy Game target*, with *build StratocracyEditor*, *run the automation suite* and
      *gate the exported suite report* all `success`. **What was NOT re-measured here is the
      linker text**, which is quoted from the delivery and not from the run's own log; a reader
      wanting it first-hand has the run ID and the failing step's name.
    - **DISCHARGED 2026-09-01, LATER THE SAME DAY. THE CAVEAT IMMEDIATELY ABOVE IS STAMPED AND
      NOT DELETED**, because it is an accurate record of what this stamp rested on when it was
      first written; only its linker half has since moved from delivered to measured. The
      `coordinator` handed this lane the failing step's log and said in terms not to take it on
      their word, so it was re-fetched here rather than pasted:
      `gh run view 33469897876 --log-failed` returns 27,806 bytes.
    - **THE RUNNER PATH IS THE LOAD-BEARING LINE, AND IT IS WHY THE CAVEAT WAS WORTH LEAVING
      OPEN.** The `LNK1169` line names
      `E:\actions-runner\_work\stratocracy\stratocracy\Binaries\Win64\Stratocracy.exe`.
      **This entry's own in-place mutant produced the SAME NINE SYMBOLS on this box**, so the
      nine `LNK2005` lines alone are indistinguishable from this lane re-quoting its own local
      output back to itself; the runner path is the only token in the payload that fixes the
      text to CI. A quote that cannot be distinguished from its reader's own prior measurement
      is not corroboration.
    - **HOW IT WAS COMPARED, AND THE CONTROL FIRST.** The twelve diagnostic lines were checked
      against the handed-over text by `diff` rather than by eye -- **EXACT MATCH, 12 of 12 lines
      identical** -- over the nine `LNK2005` symbols (`axialToOffset`, `hexDistance`, `hexEqual`,
      `hexLess`, `inBounds`, `neighborCandidate`, `neighbors`, `offsetToAxial` and
      `sortCanonical`), the `LNK1169`, the `Result: Failed (OtherCompilationError)` label and the
      Game-target exit code of 6. **The instrument was shown able to speak before its silence was
      read as agreement:** a control copy with `LNK1169` altered to `LNK9999` was reported as
      differing.
    - **AND THE LOG CARRIES A SECOND, INDEPENDENT READING OF THE `success` ROWS.** Across the
      whole `--log-failed` payload there is **exactly ONE step name**,
      `build the Stratocracy Game target`. That no other step failed is therefore established
      twice by two different instruments -- once from the API's step list, once from the failed
      log's own contents -- which is the claim the three green rows above depend on. (Noted for
      the next reader: `--log-failed` worked here, against a standing expectation on this box
      that it does not.)
    - **AND THE RUN'S OWN CLOCK IS WHY THESE STAMPS ARE DATED 2026-09-01 AND NOT 2026-08-31.**
      `createdAt` is `2026-09-01T04:27:16Z`, which on this box's local time is 2026-09-01
      00:27 -- so the local date rolled over before the measurement, and the usual hazard here
      (a UTC log stamp dragging an entry a day forward, which the parent entry itself calls out
      about `reportCreatedOn`) runs the OTHER way this time. The date is taken from local time
      and agrees with UTC by coincidence rather than by transcription.

- **2026-08-31, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, in worktree
  `E:/MultiAgent/Strat-wt/slot-2` on `feat/w8-transient-receipts`, base `c754342`, lane commit
  `0a697c0`, MERGED TO `master` AS `ed09973`) -- W8 ITEMS (5) AND (6): THE INCOME AND KILL
  RECEIPTS AND THE TURN BANNER, WHOSE EVERY NUMBER IS A SECOND READING OF A DURABLE HOME.**
  - **WHOSE WRITE THIS IS, SAID FIRST.** The C++ is this lane's, written by this lane, in this
    lane's own worktree, under a standing wave topology; the entry is this lane's too. Neither
    the transcription clause nor the in-session clause is invoked or needed. **The commits are
    a separate actor and are named as such:** the `coordinator` ran `0a697c0` and `ed09973` at
    the user's explicit instruction in session -- the user did not run git -- and the git
    author recorded on both is the user's identity, measured on this tree rather than assumed
    (`git show --format=%an ed09973` returns `lobohotpants`). The distinction is kept because
    *"the user committed it"* and *"the `coordinator` committed it at the user's instruction"*
    are different claims about who checked what, and a lane agent has already got this half
    wrong once. **The W8 path-preview lane's entry -- lane commit `4f5e09e`, merged as
    `1576e40` -- makes the same distinction about its own two commits; that is one wave's
    convention recorded twice and not two independent findings.**
  - **WHAT LANDED, BY SYMBOL.** `Source/StratUI/StratTransientReceipts.{h,cpp}`, new: the
    reflected `EStratReceiptKind`, `EStratTurnBanner`, `FStratReceiptView`, `FStratReceiptMark`
    and `FStratTransientReceipts`, plus three world-free deciders -- `StratMarkFromView`,
    `StratDecideTurnBanner` and `StratDecideTransientReceipts`. In `UStratMatchSubsystem`:
    `GetTransientReceipts`, `GetReceiptMark`, `CopyTransientReceipts`, the private
    `LastReceipts` and `ReceiptMark`, one two-line call site in `ApplyView`, and a reset beside
    each of the two existing `AppliedModel` teardowns. **It lives in `StratUI` and not in
    `StratPlay`**, for `StratViewModel.h`'s own placement reason: it is the lowest module both
    the widgets and the actors can see, and the widgets are the consumers.
  - **THE INVARIANT THIS LANE HAD TO NOT BREAK, AND THE SENTENCE THAT KEEPS IT TRUE.**
    `FStratViewModel` forbids an event layer in its own words -- no changed flag, no dirty set,
    no sequence number -- because that is what makes T-INT-05's rebuild-from-the-view-model-
    alone property structural. But a receipt IS an edge, and a value carrying no history cannot
    express one. **The reconciliation is that THE SCREEN MUST BE FULLY CORRECT WITH ZERO
    RECEIPTS DRAWN**: `FStratTransientReceipts` is not a field of the view model, is not an
    input to `ApplyView`, and every number in it is a second reading of a durable home that is
    on screen anyway -- `IncomePerTurn`, `FameCombat`, `SideToMove`. Dropping every receipt on
    the floor loses emphasis and loses no information. **If that ever stops being true -- if a
    receipt becomes the only place a fact appears -- this file has become an event list and
    T-INT-05 has a hole in it.** That is the property to check first if this surface grows.
  - **THE DECIDERS ARE WORLD-FREE ON `AStratShellHUD::DecideMenuTiming`'S PRECEDENT**, which
    this file already records as the shape that made a decision pinnable headlessly. No
    `UWorld`, no bridge, no viewport, no subsystem pointer: a clause plants a mark and a view
    model and asserts. What is left at the call site is one call and one re-mark. **The mark is
    a value the CALLER owns and not a cache inside the module** -- a file-static would make the
    answer depend on process history, so two clauses in one suite would contaminate each other
    and a second match in one session would inherit the first one's fame.
  - **THE KILL RECEIPT READS THE `FameCombat` RISE, AND ROUTING `FStratCombatOutcome` WAS NOT
    THIS LANE'S TO CHOOSE.** That struct already carries everything Sec 2.11.3's named-victim
    line needs -- `DefenderId`, `bDefenderDied`, `AttackerFameCombatBefore` and
    `AttackerFameCombatAfter` -- but `StratBridge.h` states in terms that the bridge *"does not
    retain the last one"* and defers the delivery mechanism BY NAME to a separate ruling,
    `bridge_event_list`. Taking that route would have been this lane opening that ruling, which
    is a decision and not a design. So the receipt reads the rise in
    `FStratSideView::FameCombat` instead, which needs no new bridge surface and no ruling.
    **`strat-integration-reviewer` weighed this independently and confirmed it does NOT
    reimplement the deferred mechanism under another name.**
  - **THE COST THAT BUYS, WRITTEN DOWN RATHER THAN OWNED QUIETLY.** (1) **No victim identity**
    -- a durable counter knows the amount and not the unit, so Sec 2.11.3's named-victim line
    is unbuildable from this tree. (2) **No per-kill granularity**: two kills between two marks
    collapse into ONE receipt carrying their summed Fame, because a counter cannot say how many
    steps it took. **The collapse is worst during AI playback**, where commands step one at a
    time -- a refresh between steps splits them and a refresh only at the end does not, so the
    receipt count there is a property of the refresh cadence rather than of the turn.
    **DISCHARGE CONDITION: the day the `bridge_event_list` ruling lands and the bridge retains
    outcomes, the kill arm grows a `VictimUnitId` and both costs go together.** Filing that
    ruling is not this lane's pass.
  - **A MEASURED FINDING AGAINST THE W8 RULING'S OWN WORDING, AND IT SURVIVES INTO THE MERGED
    TREE.** The ruling says kill toasts *"restate the scoreboard's Destroyed row"*. That row
    exists -- and it carries COMBAT FAME, not a count of destroyed units. Measured here:
    `EStratScoreCriterion::CombatFame` carries `UMETA(DisplayName = "Destroyed")` at
    `StratScoreboardWidget.h:111`, `FStratScoreboardRow`'s own comment maps that row to
    `UiSideView::fameCombat`, and Sec 2.11.4 gives it a disambiguating tooltip precisely
    because the label reads like a tally while the number is a currency. **`strat::UiSideView`
    has exactly five fields** -- `fameTotal`, `fameCombat`, `objectivesHeld`, `survivingHp`,
    `incomePerTurn` -- and none is a kill count; the reviewer checked that independently
    against `strat::UiSideView`, `FStratBridge` and `FStratCombatOutcome` and found no
    published delta. **THE CONSEQUENCE IS FOR `Content/` AND IS THE REASON THIS IS RECORDED
    RATHER THAN NOTED: copy drawn off a kill receipt MUST BE FAME-VOICED and NEVER A TALLY --
    the number is Fame that was awarded, not units that died.** No rules change follows and no
    scope question is opened: the durable home the ruling names does exist, it is simply a
    currency.
  - **THE INCOME ARM CARRIES THE STANDING RATE, NOT THE AMOUNT PAID, AND TURN 1 WAS
    DELIBERATELY NOT SPECIAL-CASED.** `Ui.h` is explicit that `incomePerTurn` is the STANDING
    rate, that it reads non-zero on turn 1 even though Q8(a) pays 0 then, and that it is
    *"deliberately NOT `accrueIncome`'s return value"*. That return -- the amount actually
    added -- is published NOWHERE: not in `UiSnapshot`, not through `FStratBridge`.
    **Suppressing the receipt on turn 1 was considered and REFUSED**, because it transcribes a
    rules fact (Sec 2.7's Q8(a)) into the presentation layer, where nothing rechecks it against
    the rules and where it would silently outlive any change to them. The arm is named
    `IncomeRate` rather than `Income` so the enumerator itself says what the number is.
    **DISCHARGE CONDITION: when a paid-this-turn figure is published -- an upstream
    `UiSideView` field carrying `accrueIncome`'s return, or an `FStratBridge` method routing
    it -- the arm becomes `IncomePaid`, `Amount` becomes that field verbatim, and the copy
    obligation below is DELETED rather than moved.**
  - **AN OPEN OBLIGATION OWED BEFORE THE TOAST WIDGET IS AUTHORED, RECORDED HERE SO IT IS NOT
    LOST.** Because the arm carries a rate and is not suppressed on turn 1, **copy drawn off an
    `IncomeRate` receipt must be RATE-VOICED and never amount-voiced** -- the amount-voiced form
    is FALSE ON TURN 1, while the rate-voiced form is true on every turn including that one. It
    is stated in two places in code: the header prose block, and the `UPROPERTY` tooltip on
    `FStratReceiptView::Amount`, added in a sanctioned comment-only pass after the reviewer
    observed that the header block is not a surface the content lane reads. **The tooltip is
    where a UMG author binding the pin actually lands, which is why it is duplicated there
    rather than cited.** **THE RECORD-SIDE HALF IS STILL OPEN AND IS MEASURED AS OPEN:**
    `content.md` carries zero occurrences of this obligation or of `GATE-TRANSIENT`, checked on
    this tree. `content.md` is not this lane's file and this lane does not write it; routing it
    is the `coordinator`'s. **The obligation is discharged when `content.md` carries it, or
    when the `IncomePaid` discharge above makes it moot -- whichever comes first.**
  - **A UHT FAILURE FIXED PROPERLY RATHER THAN AROUND, AND THE MEASUREMENT IS RECORDED AT THE
    MEMBER.** The first spelling marked `LastReceipts` `BlueprintReadOnly` in the private
    section; measured, one error: `Error: BlueprintReadOnly should not be used on private
    members`, reported at `StratMatchSubsystem.h`. **The obvious fix -- move the member to the
    public section -- was refused**, because a public mutable member is a SECOND WRITER of a
    value whose own declaration insists `ApplyView` is the only one. Closed instead with a
    `BlueprintPure` copy accessor, `CopyTransientReceipts`, which is the shape
    `GetConcludedMatchView` already uses in that class. **The copy is cheap here and that is
    why the shape is available at all**: `GetViewModel` refuses the equivalent because copying
    a 99-hex model would make a cheap read expensive, while this struct is a banner, two
    integers and a list that is empty on most refreshes.
  - **TWO BANNER-EDGE DEFECTS WERE CAUGHT IN THIS LANE'S OWN FIRST DRAFT, BEFORE ANY BUILD, AND
    THE RULE THAT FELL OUT IS REUSABLE.** `FStratReceiptMark` first carried only `SideToMove`,
    so the previous banner was recomputed with the CURRENT result flag. That breaks in two
    directions at once: a concluded match then reports a banner change on EVERY refresh forever
    -- the old side always maps to a turn arm and the new one always to `None` -- and forcing
    the flag false instead hides the one transition that matters, the refresh on which the
    match ends. `SetViewingSide` is the same shape: it flips the banner with no rules field
    moving at all. **THE RULE, now stated at the field: whatever `StratDecideTurnBanner` reads,
    the mark stores. A fourth input added to that function without a field beside it
    reintroduces the same class of bug.** It is recorded as a design rule and not as a fixed
    bug because neither defect was reachable by a clause written against the first draft's own
    shape.
  - **THE ONE SUBTRACTION IS MARKED AS SUCH IN THE SOURCE.** `StratTransientReceipts.cpp` holds
    the only arithmetic in the file, on the line its own comment labels THE ONE SUBTRACTION, and
    the file's opening block says why the argument for it does not extend to a second one: it
    subtracts one reading of `FameCombat` from an earlier reading of THAT SAME FIELD, restating
    no cost table, no half and no flag bonus -- the three clauses `StratBridge.h` says a deriver
    would have to restate. The receipt carries `DurableBefore`, `DurableAfter` and `Amount`
    together so a clause can check the subtraction against its own inputs, which is
    `FStratCombatOutcome`'s own precedent read from the other side.
  - **WHAT THIS ENTRY DOES NOT STATE.** No suite count and no phase verdict -- `global.md` owns
    both, and a live figure of that shape in this file is a finding by
    `strat_banner_sweep.py`'s RECORD OWNERSHIP check. Thirteen `GATE-TRANSIENT` clauses landed
    over this lane's code, across `Source/StratUI/Tests/StratTransientReceiptClauses.cpp` and
    `Source/StratPlay/Tests/StratTransientReceiptCallSite.cpp`; **`tests.md` is their record and
    this lane does not write it.** Eleven were PROPOSED from this lane, and the test author
    added three beyond them while widening a fourth -- `FameCombatFallIsSilent` became
    `FameCombatFallOrHoldIsSilent`, which correctly pins the strictly-greater comparison rather
    than only the fall. **Proposing a clause is not writing one.** The line-endings-vary-per-
    file lesson this wave also paid for is recorded in the path-preview lane's entry and is not
    restated here; it bound this pass too -- both files this lane wrote are LF, while
    `engine.md` is CRLF, and this entry was inserted by a script that measured both and refused
    on anything but exactly one anchor.

- **2026-08-31, `strat-gameplay-engineer` (ACTING and WRITING; IN LANE, in worktree
  `E:/MultiAgent/Strat-wt/slot-1` on `feat/w8-path-preview`, base `c754342`, lane commit
  `4f5e09e`, MERGED TO `master` AS `1576e40`) -- W8 ITEM (1): THE PATH PREVIEW IS ROUTED
  THROUGH THE MODULE, AND TWO OF ITS FIVE MUTANTS SURVIVED FOR A REASON NO CLAUSE CAN FIX.**
  - **WHOSE WRITE THIS IS, SAID FIRST, AND SAID WITHOUT LOCATING ANY OTHER ENTRY.** The C++ is
    this lane's, written by this lane, in this lane's own worktree, under a standing wave
    topology; the entry is this lane's too. **Neither the transcription clause nor the
    in-session clause is invoked or needed** -- this is an ordinary in-lane write, and those
    two are named only to say that neither is load-bearing here. **The commits are a separate
    actor and are named as such:** the `coordinator` ran `4f5e09e` and `1576e40` at the user's
    explicit instruction in session -- the user did not run git -- and the git author recorded
    on both is the user's identity. That distinction is kept because *"the user committed it"*
    and *"the `coordinator` committed it at the user's instruction"* are different claims about
    who checked what, and a lane agent has already got this half wrong once.
  - **[CORRECTED BEFORE THIS ENTRY WAS EVER COMMITTED -- THE BULLET ABOVE ONCE DESCRIBED ITS
    NEIGHBOURS BY POSITION, AND THE POSITION MOVED WHILE THE PASS WAS STILL OPEN. RECORDED
    RATHER THAN QUIETLY REWRITTEN, because the defect is one this file keeps meeting and the
    fix is a shape rather than a number.]** The first draft opened *"BECAUSE THE THREE ENTRIES
    ABOVE THIS ONE ARE OUT-OF-LANE WRITES AND THIS ONE IS NOT"*. It was written when this was
    the topmost entry under `## NEXT`. Entries PREPEND there, the sibling W8 lane's entry then
    landed above this one, and the sentence was afterwards wrong twice over: wrong on the count,
    and wrong on the character, since it labelled an IN-LANE write as out-of-lane -- inside the
    one bullet whose entire job is ownership legibility. **THE COUNT WAS NOT THE DEFECT; THE
    SHAPE WAS.** Changing "three" to "one" would have rotted again on the next prepend. **THE
    RULE THIS LANE TAKES FROM IT:** an entry may pin ITSELF by commit identity -- this one does,
    via `4f5e09e` and `1576e40` -- but it must never locate another entry by POSITION. Anything
    that needs to say something about another entry names it by DATE AND SUBJECT, or belongs in
    that entry's own file. Nothing here needs to, so nothing does. Note where the failure landed:
    this entry pins its own identity by commit and warns twice below that its own figures move,
    yet the single place it reached for a neighbour it reached for position -- because position
    was the only handle it had in hand, which is exactly when this defect gets written.
  - **WHAT GAP THIS CLOSED, AND WHY A NEW BRIDGE METHOD WAS UNAVOIDABLE RATHER THAN
    CONVENIENT.** `FStratBridge::ReachableHexes` answers WHICH hexes are in reach and WHAT each
    costs, and carries NO ROUTE. `strat::findPath` -- which has the route -- was reachable from
    nothing: measured before the pass, ZERO occurrences of that name anywhere in `Source/`
    outside `Source/StratRules/`. The vendored sources carry no `_API` macro (this file's
    standing 8 x `LNK2019`), so a route could not be asked for from `StratPlay` at all, and the
    only other way to draw one was to walk the reach set downhill in the engine. **That walk is
    `Move.h`'s tie-break rule restated by a layer that may not hold it, and it is the exact
    substitution `T-UI-02` forbids in its own words** -- the same shape as the 122 divergent
    hexes across 10 of 10 units this file already records for `distance <= move`. So the method
    had to exist before the feature could be built honestly.
  - **WHAT LANDED, BY SYMBOL.** `FStratBridge::MovePathToHex(int32 UnitId, FIntPoint GoalHex,
    TArray<FIntPoint>& OutRouteHexes, TArray<int32>& OutRouteCosts, int32& OutTotalCost)
    const`, in `StratBridge`. `FStratPathPreviewView` and the `FStratViewModel::PathPreview`
    field, in `StratUI` beside the model they belong to. `IStratPathQuery`,
    `FStratBridgePathQuery` and the free function `StratDecoratePathPreview`, in
    `Source/StratPlay/StratPathPreviewQuery.{h,cpp}`, on the decoration seam
    `StratForecastQuery` already established. One call added to
    `AStratPlayerController::DecorateForPresentation`, after the hover decorator.
  - **IT IS THE ONLY *PRODUCTION* `strat::findPath` CALL SITE IN THE TREE, AND THE QUALIFIER IS
    LOAD-BEARING RATHER THAN CAUTIOUS.** In shipping code the call occurs exactly once,
    `StratBridge.cpp:1446`. **There is a SECOND call in the merged tree**, at
    `Source/StratBridge/Tests/StratPathPreviewParity.cpp:303`, and it is deliberate: that file
    lives inside `StratBridge`, so the vendored symbol links, and it calls the module directly
    to serve as an INDEPENDENT ORACLE for the bridge's answer. The unqualified sentence *"the
    only `strat::findPath` call site"* was true on the lane branch before the test author's
    file landed and is FALSE in the merged tree; it is written here in its scoped form so that
    a later reader greps, finds two hits, and does not conclude the record is wrong.
  - **THE REFUSAL SPLIT, WHICH HAD TO BE CHOSEN RATHER THAN INHERITED, BECAUSE THE TWO
    NEIGHBOURS DISAGREE.** The refusals -- definitions not loaded, no scenario, a `defIndex`
    outside the table, an id not on the board -- are `Reachable`'s, forwarded verbatim. **An
    UNREACHABLE GOAL IS AN EMPTY ANSWER AND NOT A REFUSAL**, following `AttackTargetHexes` and
    NOT `ReachableHexes`. `ReachableHexes` may read empty as a fault because a successful call
    always yields at least the unit's own hex at cost 0; that property does not transfer. A
    hovering cursor produces an out-of-allowance, blocked or impassable goal on most of its
    frames, and those are ordinary board positions exactly as *"nothing in reach"* is for
    attack, so folding them onto the refusal channel would make an ordinary hover read as a
    broken bridge. `OutTotalCost` is 0 on that answer and **0 is not a sentinel** -- it is also
    the honest cost of a route to the unit's own hex -- so a consumer must read the array's
    emptiness and never the number.
  - **THE FIVE CODE MUTANTS, BUILT IN PLACE IN THE LANE TREE AND REVERTED FROM A BYTE BACKUP
    VERIFIED WITH `sha256sum -c`.** Never in a copy: a copied tree's cached `Intermediate/Build`
    resolves the original sources and reports `Result: Succeeded` for a build that compiled
    nothing. THREE DIED -- trimming the start hex (four clauses, two of them collateral from the
    resulting array-length desync); returning `Fail` on the no-route arm (two); gating the
    decorator on `bDone` instead of `bHasMoved` (two, the second because that clause drives
    `bHasMoved` from a table with an already-moved leg). **TWO SURVIVED, and they are the part
    of this entry a later reader most needs.**
  - **SURVIVING MUTANT ONE -- THE TICK CLAIM. `MovePathToHex`'s header asserts the ticks are
    `reachable`'s costs LOOKED UP, not derived. That is true of the source and unobservable
    from any output, and the mechanism is an IDENTITY rather than a weak fixture.**
    `strat::reachable`'s cost for a hex IS the summed `moveCost` along a cheapest path to that
    hex; `findPath`'s route is a cheapest path; and every prefix of a cheapest path is itself a
    cheapest path to its own last hex, because move costs are non-negative and a cheaper prefix
    could otherwise be spliced in to beat a minimal total. **So "look the cost up per hex" and
    "sum `moveCost` while walking the route" are THE SAME NUMBER ON EVERY BOARD** -- not merely
    on *Ferrum Crossing*, and not merely on the boards the fixtures happen to build. A clause
    asserting a difference would be asserting against an identity. The claim is ARCHITECTURAL
    and not behavioural: it says which module owns Sec 2.5's cost model, which is precisely
    `T-UI-02`'s subject, and which no black-box test can see.
  - **SURVIVING MUTANT TWO -- THE `OutTotalCost` CLAIM, AND THE CAUSE IS THE METHOD'S OWN
    CROSS-CHECK.** `MovePathToHex` REFUSES every board on which `OutRouteCosts.Last()` and
    `findPath`'s `outCost` differ. So on every board that answers at all the two are equal BY
    CONSTRUCTION, and `OutTotalCost = Cost` and `OutTotalCost = OutRouteCosts.Last()` are
    indistinguishable from outside. **The check removes the observability of the very thing the
    claim is about**, and by the same argument its own refusal arm is unreachable -- which
    upgrades the header's *"has never been observed to fire"* from a sample to a proof.
    **WHAT THAT IS BOUGHT WITH, so the trade is legible rather than merely disclosed:** the
    cross-check buys a real safety property -- the module's two movement answers can never
    silently disagree in front of a player, and a disagreement names both numbers instead of
    the bridge picking a winner between two rules answers -- and it pays in exactly this
    unfalsifiability. The payment is not a side effect to be engineered away: any check strong
    enough to guarantee the agreement is strong enough to hide which side was assigned.
    Deleting it to make the claim testable would trade a live safety property for a test of a
    dead one.
  - **BOTH CLAIMS ARE MARKED AT THE CODE SITE AND NOT ONLY HERE, IN A SEPARATE SANCTIONED
    COMMENT-ONLY PASS.** `StratBridge.h` carries a pointer paragraph at each claim and a
    closing block giving the mechanism, the trade, and the split between what a checkout can
    check (that the method performs no addition; the refusal channel, via
    `MovePathRefusesUnseeded` and `MovePathRefusesUnknownUnitId`; the numeric agreement, via
    `MovePathTicksAreReachableCosts`) and what rests on reading the body -- WHICH EXPRESSION
    PRODUCED EITHER NUMBER. It is stated at the code because a reader who finds these unpinned
    will otherwise file the gap as missing coverage and spend a pass trying to close it.
  - **THE `uiPath` DEBT, WHICH IS THE ONE THING IN `MovePathToHex` THAT IS NOT A FORWARD.**
    `Ui.h` offers `uiReachable` for the reach set and NO `uiFindPath` beside it, so unlike
    `Reachable` -- which forwards to a `Ui.h` entry point that assembles its own arguments --
    this method must look up the unit's Sec 2.4 `move` allowance itself, in the same one line
    `uiReachable`'s body uses. That is a table read and not a rule. **DISCHARGE CONDITION: the
    day a `uiPath` is vendored upstream, this body becomes a forward to it and no caller
    changes** -- the same sentence `AttackTargetHexes` carries about its own enumeration.
    Filing it upstream is the steward's pass and not this lane's; `Source/StratRules/` is
    read-only here.
  - **A SEAM WAS ASKED FOR AND DELIBERATELY NOT BUILT, AND THE MEASUREMENT IS WHY.** The request
    was to make `DecorateForPresentation`'s call site pinnable by adding a protected virtual or
    an injectable `IStratPathQuery*`, following the forecast path's shape. **Measured: the
    forecast path has no such seam to copy** -- `DecorateForPresentation` is declared at
    `StratPlayerController.h:337`, NON-VIRTUAL, under the `public:` at line 235; a grep for
    `Forecast` in that header returns nothing, so there is no member and no injectable; and
    `FStratBridgeForecastQuery` is a stack local in an unnamed scope block. **And the premise
    was false: the call site was already pinnable.** Because the method is public, existing
    clauses call it directly on a live controller -- at decision time, nine call sites across
    `StratHoverInputClauses.cpp` and `StratInfoPanelRouting.cpp`. So no production change was
    made. **THE MERGED TREE VINDICATES THAT AND ALSO MOVES THE FIGURE:** `strat-test-author`
    then wrote `Stratocracy.StratPlay.T-UI-02.DecorateForPresentationPublishesThePreview` using
    exactly that route, so the count is now eleven call sites across three files. The
    nine-across-two figure was true of the tree the decision was made against and is stamped
    here rather than silently corrected; **neither figure should be quoted forward, because
    both move whenever anyone adds a clause.**
  - **TWO INSTRUMENT DEFECTS WERE CAUGHT BEFORE THEY COULD FABRICATE A RESULT, AND BOTH ARE
    REUSABLE LESSONS FOR THIS LANE.** (1) **Line endings vary per file in this tree**:
    `Source/StratBridge/StratBridge.cpp` is CRLF while the new
    `Source/StratPlay/StratPathPreviewQuery.cpp` is LF. A mutation patcher matching on a bare
    newline found zero anchors in the CRLF file; had it not COUNTED its anchor and refused on
    anything but exactly 1, the mutant would have applied nothing, built green, and been
    reported as "survived" for entirely the wrong reason -- a false measurement
    indistinguishable from the true one that was eventually recorded. (2) Writing a
    carriage-return escape through a quoted bash heredoc into Python collapsed it to a real
    newline and produced a `SyntaxError`; rebuilt using `chr(13)`/`chr(10)` and no backslashes
    at all.
  - **WHAT THIS ENTRY DOES NOT STATE.** No suite count and no phase verdict -- `global.md` owns
    both, and a live figure of that shape in this file is a finding by
    `strat_banner_sweep.py`'s RECORD OWNERSHIP check. Thirteen `T-UI-02` clauses landed over
    this lane's code, across `Source/StratBridge/Tests/StratPathPreviewParity.cpp` and
    `Source/StratPlay/Tests/StratPathPreviewDecoratorClauses.cpp`; **`tests.md` is their record
    and this lane does not write it.** Two of the thirteen were PROPOSED from this lane and
    both landed -- `MovePathArraysAreTheSameLength` and
    `DecorateForPresentationPublishesThePreview` -- and proposing a clause is not writing one.

- **2026-08-31, the `coordinator` (ACTING -- the C++ below is this lane's and was written OUT OF
  LANE, in the integration tree, on the user's explicit instruction in session) and
  `strat-gameplay-engineer` (WRITING; this entry and nothing else in the pass) -- THE TITLE
  SCREEN WAS SWITCHING GAME INPUT OFF FOR THE REST OF THE SESSION, AND THE REPAIR IS A CLAIM THE
  CONTROLLER MAKES RATHER THAN A GIFT THE HUD GIVES BACK.**
  - **WHOSE WRITE THIS IS, SAID FIRST, BECAUSE TWO ACTORS ARE INVOLVED AND ONLY ONE OF THEM
    TOUCHED THE CODE.** The user reported the defect from a live session, was given the diagnosis
    and the proposed repair, and instructed that it be written in the integration tree there and
    then; the `coordinator` wrote `StratPlayerController.h` and `.cpp`, built them and ran the
    suite. This lane wrote none of those bytes and adopts none of those measurements as its own.
    **THIS IS NOT THE TRANSCRIPTION CLAUSE AND THE DIFFERENCE IS NOT A TECHNICALITY:** that
    clause is triggered by A MERGE and licenses only carrying across a draft a lane already
    wrote, and here there was no lane, no worktree, no branch, no merge and no draft. What this
    entry is instead is the receiving file recording an out-of-lane write, which is the shape
    this project's own note names -- an out-of-lane write is attributed in the file that RECEIVED
    it, not only in the coordinator's own.
    **IT IS THE THIRD SUCH WRITE IN THIS FILE, WHICH IS THE FACT WORTH FILING RATHER THAN THE
    APOLOGY.** The 2026-08-29 `LayerFor` discharge was the first, the shell HUD entry immediately
    below this one was the second and said so in terms, and this is the third -- three inside
    three days, on a rule whose own precedent note says that declaring "not a precedent" twice is
    how a lane rule stops describing practice. No clause is proposed here; the observation is
    filed for the user, who owns the ruling.
  - **WHAT WAS WRONG, AND WHY IT IS A LEVEL-TRAVEL DEFECT RATHER THAN AN INPUT DEFECT.**
    `AStratShellHUD::ApplyMenuInputMode` sets `FInputModeUIOnly` for the title menu, and
    `FInputModeUIOnly::ApplyInputMode` calls `SetIgnoreInput(true)` on the
    `UGameViewportClient`. That client is owned by the GameInstance and SURVIVES `OpenLevel`, so
    the flag the title screen set was still set on the match's viewport, and every key and mouse
    button was dropped there -- upstream of `UPlayerInput`, and therefore upstream of every
    binding, every mapping context and everything `StratPlayerControllerTick.cpp` protects. The
    engine-side reasoning recorded at the code site is that `bIgnoreInput` is written in exactly
    four places in the UE 5.8 source -- the two viewport-client constructors, which default it
    false, and the three `FInputMode*::ApplyInputMode` overrides -- and level travel is not one
    of them. Nothing in this module's selection path was wrong at any point.
  - **IT PRESENTED AS "HOVER WORKS, CLICKING DOES NOT", AND THAT IS THE SYMPTOM THIS LANE SHOULD
    RECOGNISE NEXT TIME.** The hover is not an input event: `Tick` polls
    `UpdateHoverFromCursor`, which reads the cursor off the viewport directly and never consults
    `bIgnoreInput`. So the one surviving part of the interface was precisely the part wave 0 had
    moved OFF Enhanced Input for unrelated reasons, and the surviving half made the dead half
    look like a selection bug. A reader who meets a live session where the board highlights under
    the cursor and refuses every click should suspect the viewport before suspecting anything in
    `HandleSelectionEvent`.
  - **WHAT WAS ADDED, BY SYMBOL.** `AStratPlayerController::RestoreProjectInputState`, public and
    `static`, taking a `UGameViewportClient&`; `AStratPlayerController::ClaimGameInput`, private,
    called once from `BeginPlay` before the mapping context is added; the unreflected member
    `LastInputClaim`, of the new plain (NOT `UENUM`) `enum class EStratInputClaim` with arms
    `NotAttempted` / `NoViewport` / `Claimed`; and the inline accessor
    `AStratPlayerController::GetLastInputClaim`. No new module, no module arrow moved, nothing
    added to `Stratocracy.uproject`, and no `UPROPERTY` anywhere in the change.
  - **`AStratShellHUD` WAS NOT TOUCHED, AND THAT IS THE LOAD-BEARING CALL.** The obvious repair
    is to make the HUD put back what it took. It was rejected on two grounds recorded at the code
    site: the HUD dies with the title map during travel, so the give-back would run inside world
    teardown; and it would cover exactly one route into a match, missing a direct launch, a
    console `open`, and any later map that raises a UI-only screen at all. A controller that
    asserts its own input state on `BeginPlay` is correct under all of them. The cost of the call
    is that the title menu's `FInputModeUIOnly` is still live and unchanged -- the defect's
    ORIGIN is untouched and only its persistence is repaired -- which is the right trade only for
    as long as every UI-only screen is followed by a controller that claims input back.
  - **IT IS NOT AN `FInputMode*`, AND THAT IS NOT THE REJECTION ALREADY RECORDED ON `Tick`.**
    The `Tick` block rejects capture-based input modes as a way to FEED THE MOUSE AXIS and stands
    untouched. `RestoreProjectInputState` restores the capture and lock modes from
    `GetDefault<UInputSettings>()` -- the same object `UGameViewportClient::Init` reads -- so
    `Config/DefaultInput.ini` remains the single source of those values and a later change to it
    moves this code with no edit here. `FInputModeGameAndUI` would have imposed
    `CaptureDuringMouseDown` and `DoNotLock`, which are not this project's defaults and would
    make a click land differently depending on whether the player came through the title screen.
    The `FApp::CanEverRender()` guard on the two mouse modes mirrors `Init`'s own headless rule
    rather than inventing one; `SetIgnoreInput(false)` sits ABOVE that guard and is
    unconditional, because that is the field the defect is actually about.
  - **THE UNREFLECTED MEMBER EXISTS FOR THE ROUTE, NOT FOR THE MECHANISM, AND WITHOUT IT THE CALL
    SITE WOULD BE DELETABLE IN SILENCE.** A headless world has no `UGameViewportClient` at all,
    so `ClaimGameInput` can only take its `NoViewport` arm there and would otherwise leave no
    mark on any object reachable from a clause. `NotAttempted` and `NoViewport` are therefore
    distinct values on purpose: the first means `BeginPlay` never asked, the second means it
    asked and there was no viewport. That is this project's recorded
    correct-mechanism-with-no-caller defect being paid for in advance rather than after. It is
    read by nothing on screen and it must not become reflected -- it is a fact about this
    object's call history, in the same family as `bGuidanceArmed`.
  - **WHAT NO INSTRUMENT IN THIS TREE CAN CONFIRM, STATED SO IT IS NOT MISTAKEN FOR MEASURED.**
    `bIgnoreInput` is not a `UPROPERTY`, so `GetAll` cannot read it and there is no `ke` getter
    for it; the end-to-end fact that clicking works again in a live session is the USER'S HUMAN
    PLAYTEST, reported after the change, and is not re-executable from a checkout. What IS
    re-executable is the clause set over the seam, and that is why the seam was cut static.
  - **NO COUNT AND NO VERDICT IS STATED HERE.** `Tools/architect/state/global.md` owns both; its
    topmost `## NEXT` entry carries the live figure for this pass and the set difference that
    moved it, and this entry links there rather than restating it. One fact that is this entry's
    to state and not that file's: the pass ADDS clauses rather than leaving the count still, so
    unlike the shell HUD pass immediately below it the count move here is not empty by
    construction.
  - **WHAT IS OWED, AND IT IS ALREADY WRITTEN RATHER THAN OUTSTANDING -- WHICH IS ITSELF A
    FINDING FOR ANOTHER LANE'S FILE.** Three clauses ride `T-UI-02` and mint no new acceptance
    ID, in the new file `Source/StratPlay/Tests/StratInputClaimClauses.cpp`:
    `TheInputClaimClearsAViewportsIgnoreInput`, `TheInputClaimRestoresTheProjectsOwnMouseModes`
    and `BeginPlayMakesTheInputClaim`. **`Tests/` IS `strat-test-author`'S LANE AND THOSE BYTES
    WERE WRITTEN BY THE `coordinator` TOO**, in the same out-of-lane pass and on the same
    instruction; `Tools/architect/state/tests.md` is not this lane's to write and carries no
    entry for them at the time this is written, so it is named here as a handoff rather than left
    to be discovered.
    **[STAMPED 2026-08-31, LATER THE SAME DAY -- THE SENTENCE ABOVE IS SUPERSEDED, AND IS KEPT
    BECAUSE IT WAS TRUE WHEN IT WAS WRITTEN AND BECAUSE DELETING IT WOULD HIDE HOW SHORT ITS LIFE
    WAS.** `Tools/architect/state/tests.md` NOW CARRIES THE ENTRY for these three clauses. It was
    written by `strat-test-author` ITSELF, in its own lane and its own file, and it attributes the
    clause bytes to the `coordinator` acting out of lane while claiming only the writing -- the
    same two-actor shape this entry uses. It landed CONCURRENTLY with this entry rather than after
    it: the two lanes were writing at the same time, which is why the bullet above could go false
    inside the hour and why its "at the time this is written" qualifier did not save it. A reader
    arriving by a grep lands on the sentence and not on the qualifier's intent. **Stamped and not
    deleted, on this record's own convention, and because under-claiming is the more expensive
    error of the two: it sends the next reader to redo work that is already finished.** No count
    and no verdict moves here -- `global.md` still owns both.]

- **2026-08-31, `strat-gameplay-engineer` (ACTING AND WRITING) -- `StratShellHUD.h`'S
  ROUND-SCOPED PROSE IS STAMPED, BECAUSE AN ASSET CHANGE FALSIFIED IT AND NO DIFF ON THIS
  MODULE SHOWED THE LINE. COMMENT-ONLY, AND PURELY ADDITIVE: 49 ADDED LINES, 0 DELETED.**
  - **WHAT WENT STALE, AND HOW IT WENT STALE WITHOUT ANYONE TOUCHING THIS FILE.** The title
    screen was wired to `AStratShellHUD` as ASSET work in the same working tree: `HUDClass` on
    `BP_StratShellGameMode` now names `BP_StratShellHUD_C`, and the graph-side menu chain was
    removed. Nothing in `Source/` changed, so four prose claims in `StratShellHUD.h` became
    false with no line in any C++ diff to point at them --
    `strat-integration-reviewer` observed it, and the user ruled it fixed in the same commit
    rather than deferred. This is the tree's recorded failure mode (*a dead path made a comment
    true*) running in the other direction: a LIVE path made a comment false.
  - **THE FOUR SITES, AND WHY IT WAS FOUR RATHER THAN THE ONE I WAS POINTED AT.** The brief
    named the `NOT IN THIS ROUND` bullet. Reading the rest of the file for the same
    falsification found three more, all of which described the graph-side creation in the
    PRESENT TENSE: the `WHAT GAP THIS CLOSES` opening paragraph (*"is created from
    `BP_StratShellGameMode`'s own graph"*), the `THE HALF THAT IS NOT COSMETIC` block (*"The
    graph works around it by hopping one tick"*), and `MaxConfigurationWaitTicks`'s doc comment
    (*"One tick is what the shipped Blueprint graph needs today"*). Each carries its own stamp
    naming what became true; the `NOT IN THIS ROUND` block is named in the other three as the
    single place to read for current state, so the stamps do not have to stay in agreement with
    each other.
  - **STAMPED, NOT DELETED -- AND ONE SENTENCE INSIDE THE STALE BULLET IS NOT STALE AT ALL.**
    *"Until it is removed and `HUDClass` is set, adopting this class would put two menus on
    screen"* is CONDITIONAL, not round-scoped: it states what happens when only one half lands,
    and it stays true of any FUTURE map that adopts this class over a GameMode graph that still
    builds its own menu. Deleting it would have thrown away the warning that made this change
    safe. What is superseded is only the framing around it. The distinction was the whole task
    and it is recorded because the same bullet will read stale again to the next reader who
    skims it.
  - **THE MEASUREMENTS IN THOSE STAMPS ARE THE `coordinator`'S, NOT THIS LANE'S, AND THE
    HEADER SAYS SO IN TERMS.** They were taken on the asset change and passed to me; I re-ran
    none of them and this entry does not adopt them as mine. `HUDClass` =
    `/Game/UI/BP_StratShellHUD.BP_StratShellHUD_C` with the `Create Widget` / `AddToViewport` /
    input-mode / cursor chain removed, both halves in one save, confirmed against the HEAD LFS
    blob by one scanner over both sides; live PIE on `/Game/StratMaps/Lvl_Title` with
    `obj list class=WBP_TitleMenu_C` returning `1 Objects` and this HUD's `MenuWidget` naming
    that instance; `bMenuDrewOnAConfiguredShell = True` with `LastFailureReason` empty.
  - **THE ZERO-TICK RESULT ANSWERS A QUESTION THE HEADER ITSELF RAISED, WHICH IS WHY IT GOT A
    STAMP RATHER THAN A FOOTNOTE.** `ConfigurationTicksWaited = 0`. The C++ route does not wait
    the ordering race out -- it REMOVES it: this HUD's `BeginPlay` lands after
    `AStratShellGameMode::BeginPlay` has RETURNED, so the shell already reports configured on
    the first pass and `DecideMenuTiming` answers `CreateNow`. `MaxConfigurationWaitTicks` is
    therefore untouched slack, held for the reason it was given, and `CreateUnconfigured` is
    still what makes a zero budget safe. No code changed and no clause moves: the four-tick
    default, the decider and every arm are byte-identical.
  - **HOW COMMENT-ONLY WAS PROVED RATHER THAN ASSERTED.** `git diff --numstat` reports
    `49 0` on `Source/StratPlay/StratShellHUD.h` and `git diff -U0 | grep -cE '^-[^-]'`
    reports `0`, so nothing was removed and every added line survives a filter that strips
    `//`, `*`, `/*` and `*/` prefixes leaving an empty set. **The filter was controlled** --
    fed a synthetic diff containing `int32 MaxConfigurationWaitTicks = 4;` it prints that line,
    so its silence on the real diff is a measurement and not an inert grep. CRLF was preserved
    and checked by count: 291 lines, 291 `\r`.
  - **THE BUILD IS NOT GREEN AND THE REASON IS THE EDITOR, MEASURED IN TWO STAGES.** Plain
    `Build.bat` refused before running any action --
    `Unable to build while Live Coding is active` -- so **it compiled nothing and proves
    nothing**. Re-run with `-NoHotReloadFromIDE`, which `.agents/ue-project-context.md` records
    as defeating the Live Coding MUTEX and explicitly NOT the editor's DLL write lock, all four
    compile actions succeeded -- `StratShellHUD.cpp`, `StratShellHudCallSiteClauses.cpp`,
    `StratShellMenuTimingClauses.cpp` and `Module.StratPlay.gen.cpp` -- and only the link
    failed, exactly the recorded editor-open signature: `UbaSessionServer - ERROR opening file
    ...UnrealEditor-StratPlay.dll for write`, then
    `LINK : fatal error LNK1104: cannot open file`. **That is the evidence that mattered here**:
    a malformed comment delimiter is a COMPILE failure, and both translation units that include
    this header plus the UHT-generated `Module.StratPlay.gen.cpp` compiled clean. The link is
    blocked by `UnrealEditor.exe` pid 33680 and by nothing in this change.
  - **THE BRIEF I WAS HANDED WAS WRONG ABOUT `-NoHotReloadFromIDE` AND THE TREE WON.** It
    called the flag "this project's recorded defeat" for the editor-open case; the architecture
    note says in capitals that it is NOT an exemption on THIS tree and only makes the failure
    later and stranger, which is precisely what was observed. Recorded because the same wrong
    premise will be handed to the next agent unless the dispatch is fixed.

- **2026-08-31, the `coordinator` (ACTING AND WRITING; OUT OF LANE, on the user's explicit
  instruction in session -- the ownership note is the first bullet below rather than a header
  above, because a reader arriving by a citation lands here) -- THE SHELL HUD EXISTS. THE THING
  IT ACTUALLY BUILDS IS AN ORDERING AND NOT A WIDGET, AND ITS POINT IS THAT IT TURNS A CLAUSE
  THIS PROJECT REFUSED TO WRITE INTO ONE IT CAN.**
  - **WHOSE WRITE THIS IS, SAID FIRST BECAUSE IT IS THE IRREGULAR PART.** This file's sole
    writer is `strat-gameplay-engineer`. The transcription clause does **not** cover this entry
    and the difference is not a technicality: that clause is triggered by A MERGE and licenses
    only carrying across a draft a lane already wrote, and here there was **no lane, no
    worktree, no branch, no merge and no draft**. The `coordinator` wrote the C++ itself, in the
    integration tree, and then wrote this. Acting and writing are the same actor and both are
    named rather than one being left to be assumed.
    **IT IS THE SECOND TIME, NOT THE FIRST, AND THAT IS THE FACT WORTH RECORDING.** The
    2026-08-29 entry below -- the `LayerFor` late-mesh discharge -- opens
    `coordinator` (ACTING AND WRITING) in this same file. A thing that has now happened twice is
    a condition forming rather than an exception, and this project's own note says that
    declaring "not a precedent" twice is exactly how a lane rule stops describing practice. No
    clause is proposed here; the observation is filed for the user, who owns the ruling.
  - **THE GAP, AND THE RECORD HAD ALREADY FILED IT IN TERMS.** W6 shipped the shell with no HUD,
    so `WBP_TitleMenu` reaches the screen from `BP_StratShellGameMode`'s own graph -- Create
    Widget, Add to Viewport, input mode, cursor -- which is the one place in this project where
    a widget is drawn without a C++ HUD owning a widget-class slot. The editor lane flagged the
    departure rather than burying it (`content.md`, W6's asset batch: *"The alternative is a
    shell HUD class with a widget slot, which would match the convention and is an
    engineer-lane item; it was not taken and the user was told."*) and `global.md` filed it as
    an engineer-lane request. This entry closes that request.
  - **WHAT WAS BUILT.** `AStratShellHUD`, an `AHUD` in `Source/StratPlay/`, plus one `UENUM`,
    `EStratShellMenuTiming`. The Blueprint-facing surface is `MenuWidgetClass` (a
    `TSubclassOf<UUserWidget>`, `EditDefaultsOnly`, never a path), `MenuZOrder`,
    `bTakeUiOnlyInputMode` and `MaxConfigurationWaitTicks`. The observable surface is
    `IsMenuLive`, `LastFailureReason`, `ConfigurationTicksWaited` and
    `bMenuDrewOnAConfiguredShell`. `CreateMenuWidget` is the one function that touches a
    viewport; `ResolveMenuTiming`, `FindShell` and `ApplyMenuInputMode` are private.
  - **THE ORDERING IS THE WHOLE POINT, AND IT IS WHY THIS CLASS IS WORTH ITS OWN FILE.**
    `AStratShellGameMode::BeginPlay` calls `Super::BeginPlay()` -- which is what fires a
    Blueprint's `Event BeginPlay` -- before `ConfigureMatchDestination`, so a menu built off
    that event asks `UStratShellSubsystem::GetMenuModel` on an unconfigured shell and draws
    four greyed rows. The graph hops one tick around it. **`global.md` records the clause that
    would have pinned that hop being REFUSED, and the refusal was right**: a clause asserting
    the CURRENT ordering pins a hazard as a requirement and goes red the day the hazard is
    repaired.
    **THIS CLASS ASSERTS NOTHING ABOUT WHEN THE CONFIGURING CALL RUNS.** It asks the shell
    WHETHER IT HAS BEEN CONFIGURED and creates the menu only once the answer is yes, waiting a
    bounded number of ticks. The property that becomes pinnable is *"the shell is configured
    before the menu is asked for its model"* -- which stays green under the repair, since moving
    `ConfigureMatchDestination` ahead of `Super::BeginPlay()` merely makes the wait zero ticks
    and moves no clause. That is this project's own note applied: pin the requirement, not the
    hazard.
  - **THE DECIDER IS WORLD-FREE, ON `UStratShellSubsystem`'s OWN STATED SPLIT.**
    `AStratShellHUD::DecideMenuTiming` is `static`, takes a bool and two ints, and returns one
    of three arms -- `CreateNow`, `WaitForConfiguration`, `CreateUnconfigured`. Three and not a
    bool, so that "wait" and "give up waiting" are distinguishable to a clause and to a log
    line; the second is the one a player can see. A headless clause reaches every arm with no
    viewport, no cursor and no travel. What is left over is one `CreateWidget`, one
    `AddToViewport` and one `SetInputMode`.
    **THE `CreateUnconfigured` ARM DRAWS RATHER THAN REFUSING, and the argument is already in
    the tree rather than invented here:** `AStratShellGameMode::BeginPlay` refuses to skip
    configuring on a null `MatchLevel` because *"refusing to configure at all would replace
    that readable menu with an empty one and a log line nobody is looking at."* The same trade
    holds one level up. An unconfigured shell still builds all four rows, greyed, each naming
    its own reason, which is §2.11.5's rule that unavailable options are greyed and named and
    never hidden. The budget expiring costs a `LastFailureReason` and a Warning, not a menu.
  - **THE SUBSYSTEM GREW A FLAG RATHER THAN THE HUD INFERRING ONE, AND THE REASON IS A DEFECT
    THIS PROJECT HAS ALREADY PAID FOR.** `UStratShellSubsystem::HasMatchDestinationBeenConfigured`
    reads a new `Transient` bool set at the end of `ConfigureMatchDestination`. **It records THE
    CALL AND NOT ITS ARGUMENTS**, because every value the call carries has a legal default that
    a configured shell can also hold: `MatchLevel` is null on a genuinely unconfigured
    Blueprint, and `SaveSlotName` arrives non-empty by construction from
    `AStratShellGameMode`'s constructor. Neither can distinguish "nobody has called" from
    "somebody called with these values" -- which is precisely the shape of the real-default-
    cannot-signal-unset defect that wrote the player's save every run. It is never cleared, and
    it says nothing about whether the configuration was any good: a configured shell with a
    null level reads true here and false on `FStratShellFacts::bMatchLevelConfigured`, and the
    two questions must not be conflated.
  - **THE MODULE ARROW MOVED BY ONE LINE AND IT HAD TO BE LOOKED FOR.** `StratPlay.Build.cs`
    gains `"UMG"` under `PrivateDependencyModuleNames`. This module already depends PUBLICLY on
    `StratUI`, which is where every other widget in the project lives -- **and that arrow
    carries nothing**, because `StratUI.Build.cs` lists UMG as PRIVATE. Private here for the
    same reason it is private there: no header in this module includes a UMG header,
    `StratShellHUD.h` holding its slot as a `TSubclassOf` over a forward declaration, which is
    all UHT needs. No new module, no reversed arrow, nothing added to the `.uproject`.
  - **THE BUILD, IN THIS TREE, EDITOR CLOSED WITH A CONTROL.** `tasklist` piped to a
    case-insensitive `UnrealEditor` filter came back empty **and `tasklist` alone was shown
    printing its header in the same pass**, because an empty filter proves nothing until the
    instrument is shown able to speak. `Build.bat StratocracyEditor Win64 Development` with the
    absolute `-project=` and `-waitmutex`, whose own last lines are `Result: Succeeded` and
    `Total execution time: 12.96 seconds` over 9 actions -- with `StratShellHUD.cpp`,
    `StratShellSubsystem.cpp`, `StratShellGameMode.cpp`, `Module.StratPlay.gen.cpp` and the
    `UnrealEditor-StratPlay.dll` link all named among them, so the green is over the new
    translation units and over UHT's regenerated reflection for the new `UCLASS` and `UENUM`
    rather than over an up-to-date tree. **ZERO warnings and ZERO errors** over the whole
    captured log, counted case-insensitively rather than eyeballed from a tail. The tool also
    reported an adaptive-unity exclusion naming both edited translation units -- the line reads
    `[Adaptive Build] Excluded from StratPlay unity file: StratShellHUD.cpp, StratShellSubsystem.cpp`
    -- so the new file compiled STANDALONE and its include list stands on its own rather than on
    a neighbour's.
  - **THE SUITE WAS RE-RUN ON THIS TREE AFTER THE RELINK AND NO FIGURE IS STATED HERE.**
    `global.md` owns the count and the verdict and this entry links to it rather than restating
    it. Two facts that are this entry's to state: the figure did not MOVE, and this pass adds
    no clause, so a set difference on the test macro over these changes is empty by
    construction. It was read from the exported report under `Saved/AutomationReport` rather
    than from the log, on the standing rule that the log undercounts by one.
  - **WHAT IS OWED, AND NEITHER HALF IS THIS LANE'S.**
    - **THE CLAUSE.** `DecideMenuTiming` was made static and three-armed *so that it could be
      pinned*, and nothing pins it yet. Its three arms and the `MaxConfigurationWaitTicks`
      boundary are `strat-test-author`'s, under `Source/StratPlay/Tests/`. **Until it lands,
      the requirement this class was built to make assertable is UNASSERTED, and this record
      says so rather than letting the design read as discharged.**
    - **THE ASSET CHANGE, AND IT CARRIES A PLAYER-VISIBLE HAZARD.** `HUDClass` on
      `BP_StratShellGameMode` must point at a Blueprint subclass of `AStratShellHUD` **AND the
      graph's own Create Widget / Add to Viewport / input mode / cursor nodes must come out in
      the same commit. Doing the first without the second puts TWO MENUS ON SCREEN.** That is
      `Content/` and therefore the editor lane's; the editor was closed for this entire pass.
      **Nothing is broken meanwhile: this class is inert until an asset names it**, because
      nothing spawns a HUD no `HUDClass` points at.
  - **PROVENANCE, STAMPED RATHER THAN WRITTEN AS A LIVE SENTENCE.** Measured 2026-08-31 in the
    integration tree at repository root `E:/MultiAgent/Stratocracy` on branch `master` over base
    `84788c0`, with no worktree, no branch, no rebase and no merge, and with the editor closed
    throughout. The absolute path is stamped to this measurement and is **not** a claim about
    any other checkout -- a live sentence naming this box's path is a thing CI can never
    satisfy, which this project has already reddened a runner to learn.
  - **WHAT THIS PASS DID NOT TOUCH, stated so the blast radius is readable:** no `Content/`, no
    `Config/`, no `Data/`, nothing under `Source/StratRules/`, no test and nothing under any
    `Tests/` directory, no `.uproject`. Outside this record the working tree carries exactly
    five paths, all under `Source/StratPlay/`.

- **2026-08-30, `strat-gameplay-engineer` -- W6, THE TITLE/MENU SHELL, LANDS AS C++ ONLY IN THE
  LANE TREE `E:/MultiAgent/Strat-wt/slot-1` ON `feat/title-menu`. THE HEADLINE IS NOT THE SCREEN;
  IT IS THE PLAYER-VISIBLE DEFECT THE SCREEN'S OWN CONFIGURATION SURFACED.** This entry is the
  engineering record for a wave whose C++ was still uncommitted when it was written. **THE
  AUTHORITY ON HOW THE WAVE WENT IS THE PERSISTED GATE ACCOUNT**,
  `Tools/architect/gate_reports/2026-08-30-w6-title-menu.md` -- five rounds, eleven findings,
  every one of them in prose or in evidence rather than in the code's own behaviour. The suite
  figure and the census delta for this wave live in `global.md`'s topmost banner, which is the
  only file that may carry them; they are not restated here.
  - **WHAT WAS BUILT.** `UStratShellSubsystem`, a `UGameInstanceSubsystem`, and
    `AStratShellGameMode`, both in `Source/StratPlay/`. **The shell is the first code in this
    project that opens a level**: before it, `GameDefaultMap` was the only thing that had ever
    chosen one, so `LoadMatchFromSlot`, a drawn section 2.8 result and a re-seeded scenario were
    each reachable exactly once per process launch and never again.
    - **FOUR ROUTES, AND THEY ARE OURS.** `EStratShellRoute` declares `NewMatch`,
      `ContinueMatch`, `ReturnToTitle`, `QuitGame`. **Restart is `NewMatch` taken from a live
      world and is deliberately not a fifth route** -- two routes would be two answers to "what
      does a fresh match seed from", which is the drift `T-TURN-09` exists to forbid. Only the
      label moves.
    - **A FACT STRUCT, A MODEL, AND SIX WORLD-FREE DECIDERS.** `FStratShellFacts` carries
      `bMatchLevelConfigured`, `bTitleLevelConfigured`, `bSaveSlotIsRestorable`, `bMatchIsLive`
      and nothing from the rules -- no side, no turn, no Fame. `FStratShellOption` and
      `FStratShellMenuModel` are the drawable shape; the model **always carries all four options
      in declaration order**, so a clause pins by index rather than by label text, which matters
      because this project has measured that `FString` comparison is case-insensitive.
      `UStratShellSubsystem::BuildMenuModel`, `IsRoutePermitted`, `ResolveDestination`,
      `RouteTravels`, `RouteLoadsSaveSlot` and `PendingSlotForRoute` are all `static` and need no
      world -- that is what makes the decision half of this class pinnable headlessly.
    - **THE CROSS-LEVEL HANDOFF IS THREE VERBS AND NOT A MEMBER WRITE.** `ArmPendingLoadSlot`,
      `PeekPendingLoadSlot`, `ConsumePendingLoadSlot`. **Travel and load cannot be one step**:
      `UStratMatchSubsystem::LoadMatchFromSlot` needs a seeded match to restore INTO, and that
      match is seeded by `AStratGameMode::BeginPlay` in a world that does not exist at the moment
      the route is chosen. `PendingLoadSlot` is what crosses the travel, and it is why the shell
      is a GameInstance subsystem rather than anything owned by a world. **Consuming rather than
      reading is a contract and not tidiness** -- a player who continues once and then starts a
      new match must not be silently handed the old save on the second travel.
    - **`ExecuteRoute` ROUTES; IT DOES NOT DECIDE.** It asks `IsRoutePermitted`, then
      `ResolveDestination`, and `RouteTravels` chooses between the two engine calls; it arms
      through `PendingSlotForRoute` and `ArmPendingLoadSlot` rather than writing the member, so
      the shipped path and the tested path are the same lines for both the deciding and the
      storing. **The extraction of `PendingSlotForRoute` was forced by a gate finding**: pulling
      out the WRITE while leaving the CHOICE of what to write inside `ExecuteRoute` left the half
      that could actually be wrong sitting on an untested line.
    - **CONFIGURATION ENTERS FROM THE TWO GAMEMODES AND NEVER FROM A PATH LITERAL.**
      `AStratShellGameMode::MatchLevel` and `AStratGameMode::TitleLevel` are
      `EditDefaultsOnly TSoftObjectPtr<UWorld>`; each GameMode holds exactly the one destination
      its own map is able to route to, so no level reference is duplicated across two Blueprint
      defaults with nothing asserting they agree. `AStratShellGameMode::BeginPlay` calls
      `ConfigureMatchDestination`; `AStratGameMode::BeginPlay` calls `ConfigureTitleDestination`
      and then `ConsumePendingLoadSlot`. **`AStratShellGameMode::SaveSlotName` is DERIVED, not
      copied** -- the constructor takes it from a default-constructed `FStratMatchConfig`, because
      `UStratMatchSubsystem::ResolveSaveSlotName` already refuses a second literal author of that
      string in its own words. `AStratShellGameMode::LastFailureReason` answers why the shell is
      unconfigured, when it is.
    - **WHERE THE CONSUME SITS IN `AStratGameMode::BeginPlay`, AND BOTH HALVES ARE LOAD-BEARING.**
      After `StartMatch`, because a restore needs a seeded match to restore into; **before the
      opening AI turn**, because running the AI first would move a match the player is about to
      have overwritten, and would move it twice on the AI-first side. A refused restore logs a
      Warning and is deliberately NOT folded into `LastFailureReason`: that property answers "why
      is there no match", and a refused restore leaves a perfectly good freshly seeded match
      standing behind it.
  - **THE DEFECT, WHICH IS THE PART A LATER READER MUST NOT MISS, AND IT WOULD HAVE SHIPPED.**
    `UStratMatchSubsystem::ApplyView` calls `RecordMatchCompletionOnSave`, which creates the slot
    when none exists and writes a payload with an **empty `SaveText`**. So after a
    completed-but-unsaved match the slot file exists, `DoesSaveGameExist` answers true,
    **`Continue` draws ENABLED and silently gives the player turn 1 instead of a restore.**
    - **NEITHER HALF IS A DEFECT ALONE.** The completion writer is correct. The derived
      `SaveSlotName` default is correct. The empty default this branch replaced had been HIDING
      the combination rather than fixing it -- so the improvement is what exposed the defect, and
      the wave that introduced the improvement is the wave that had to own it.
    - **THE VERSION GATE CANNOT CATCH IT BY CONSTRUCTION.** `StratSaveGame.h` initialises
      `SavedDataVersion = kCurrentSavedDataVersion`, so a completion-only payload's header is
      valid and current. That is also why `T-SAVE-04` was **proposed and refused by the lane it
      was offered to**: that ID's own GDD sentence scopes it to a header mismatch, and there is no
      header mismatch here. The new save-slot clauses ride `GATE-TITLEMENU` instead.
    - **THE SUITE COULD NOT SEE IT BECAUSE THE ERROR WAS IN THE FACT, NOT THE DECISION.**
      `bSaveSlotExists` said *a file is there* while every consumer of it read *a match can be
      restored*, and that fact was produced in the one function that needs a world.
    - **THE FIX.** Three payload refusals were extracted out of `LoadMatchFromSlot` into two
      world-free statics, `UStratMatchSubsystem::IsPayloadRestorable` and
      `UStratMatchSubsystem::DoesSlotHoldARestorableMatch`; the loader now CALLS the first rather
      than keeping a copy -- one statement of the conditions, two askers. The subsystem-state
      refusals stayed inside `LoadMatchFromSlot` and must: a running AI turn and an unconfigured
      subsystem are facts about that object at that moment, not facts about a payload.
      `FStratShellFacts::bSaveSlotExists` was renamed `bSaveSlotIsRestorable`, so the fact's name
      now says what its consumers were already reading it as. `DoesSaveSlotExist` was **kept, not
      narrowed** -- "does a file occupy this slot" is a real question; gating an affordance on it
      is what was wrong.
  - **AND THE TREE HAD ALREADY WARNED, CORRECTLY, IN CURRENT PROSE, AND NOBODY READ IT. THIS IS
    THE MOST TRANSFERABLE THING IN THE WAVE.** `StratSaveGame.h` -- unmodified by this branch --
    carries under the heading **`ONE CONSEQUENCE, WRITTEN DOWN RATHER THAN DISCOVERED`** the
    sentence that anything offering a "Continue" affordance must gate on a LOADABLE slot and not
    on `DoesSaveSlotExist`, "which now answers true for a slot with no match in it". **It names
    the affordance, the wrong function and the right rule, before W6 was written, under a heading
    claiming it would not have to be discovered. It was discovered.**
    - **THE LESSON IS NOT "READ MORE CAREFULLY", WHICH IS UNACTIONABLE AND WOULD BE THE THIRD
      TIME THIS PROJECT WROTE IT.** A warning placed in the header of the TYPE IT CONCERNS is
      invisible from the CONSUMING end: nothing about writing a menu sends you to a save payload's
      file comment, and grep finds it only if you already suspect what it warns about. **What
      would actually have caught it is the clause the arm never had -- a prose warning cannot fail
      a build and an executable one can.** Two of the three payload arms had no clause at all,
      measured corpus-wide: systematic, not bad luck on a single arm.
    - **ONE HYPOTHESIS WAS FORMED AND KILLED, RECORDED SO NOBODY RE-DERIVES IT.** The warning did
      NOT go unread because it named a stale symbol: `DoesSaveSlotExist` was a real method at
      `347c722` and the sentence was accurate and greppable.
    - **THE STANDING CONSEQUENCE FOR THIS LANE**: when a header states a consequence that binds a
      CONSUMER it does not name, that is the shape that needs a clause, not a better paragraph.
  - **THE PER-FRAME DEBT, DECLARED IN THE BYTES ON `UStratShellSubsystem::GetMenuModel` AND
    REPEATED HERE.** `GatherFacts` now asks `DoesSlotHoldARestorableMatch`, which performs a full
    `LoadGameFromSlot` **deserialize** where it previously read a directory entry -- and a UMG
    property binding runs every frame. `HasCompletedAMatchOnSave` sets the precedent for a
    per-call load, so this is not novel; it is worse, because a menu binds per frame and a
    guidance check does not.
    - **NO CACHE, AND THE ABSENCE IS A DECISION RATHER THAN AN OVERSIGHT.** A cache with no
      invalidation would show `Continue` greyed to a player who had just saved -- a stale-read
      defect this project has already paid for -- and the correct invalidation points are not
      knowable until the menu asset exists and its refresh cadence is a fact rather than a guess.
      **An undeclared decision is indistinguishable from an unnoticed one**, which is why it is
      declared on the node an asset author actually binds.
    - **DISCHARGED BY EITHER, AND THE FIRST IS PREFERRED**: the WBP refreshing this on demand --
      on construction, and on the events that can change the answer -- rather than binding it per
      frame; or this class growing a cache invalidated on `ConfigureMatchDestination`,
      `ArmPendingLoadSlot` and `ConsumePendingLoadSlot`, which are the writers that can move the
      answer from inside. Whichever lands, the other stops being owed.
  - **WHAT IS STILL OWED, AND ONE ITEM HAS AN ORDERING CONSTRAINT THAT BREAKS EVERY LANE IF IT IS
    IGNORED.** **No asset and no `Config/` change landed in this wave**, per the parallel-lane
    rule that a branch needing an asset lands its C++ and the asset is authored on the integration
    tree. Owed: the title level; the menu WBP; `BP_StratShellGameMode`; `TitleLevel` set on
    `BP_StratGameMode`; and the steward's `GameDefaultMap` move -- **which MUST land AFTER the
    level exists, or it breaks the suite for every lane.** Until those exist, an unset
    `TitleLevel` is a configuration and not a fault: `Return to Title` simply comes back from the
    model greyed with its own reason, and the match map is exactly as playable as it was before
    the property existed.
  - **WHAT IS UNPINNED, STATED NARROWLY RATHER THAN WIDELY.** The engine call on `ExecuteRoute`'s
    permitted arm; the CONDITIONALITY of its arming call; and whatever ordering the refusal clause
    does not reach. `ExecuteRoute` has **one non-comment call site in `Source/`**, on a fixture
    whose own assertion is that the route is REFUSED, so the function returns at the permission
    arm and never reaches the arming line -- which is WHY nothing catches a change to it, rather
    than a report that nothing did. That derivation is re-executable from a checkout and **is void
    the moment a second `ExecuteRoute` caller appears**, which is the correct trigger to rewrite
    it. Also unpinned and named in the bytes: a designer who overrides `SaveSlotName` on EITHER
    Blueprint re-opens the slot-name drift, and no C++ can see it, because a mismatch and a
    genuinely unwritten slot both land on "No saved match." -- the ordinary first-run state, so
    the failure stays quiet. **The discharge is a clause reading both Blueprint CDOs and asserting
    the two strings equal, once the editor lane has created them.**
  - **WHAT I DID NOT MEASURE IN THIS PASS, SAID SO THAT NOTHING ABOVE READS AS MY OWN RUN.** This
    dispatch wrote a record and nothing else: **I did not build, did not run the suite, and edited
    no file under `Source/`.** Every statement here about the build, the suite, the census delta
    or the clause-name set equality is **quoted from `global.md`'s banner and from the persisted
    gate report, both measured by the `coordinator` and by `strat-integration-reviewer` in this
    lane tree** -- none of it re-derived by me. The symbol names, signatures, route set, property
    list and call ordering above WERE read out of the working tree's own bytes in this pass. The
    gate report is itself explicit that **no round built the project or ran the suite**, and that
    the mutant set behind several clause justifications remains an agent self-report -- one of
    whose instruments was found dead, because a `robocopy`'d mutant tree's cached
    `Intermediate/Build` kept resolving the SOURCE tree and printed `Result: Succeeded` for a
    build that relinked nothing. **`Result: Succeeded` is not evidence that a build happened; the
    artifact is.**

> **[OUT-OF-LANE WRITE, 2026-08-29, AND IT IS NOT A TRANSCRIPTION -- SAID FIRST BECAUSE THE
> CLAUSE IN THIS FILE'S SOLE-WRITER LINE DOES NOT COVER IT. The entry directly below was ACTED
> AND WRITTEN BY THE `coordinator`, WHOSE FILE THIS IS NOT.** No lane agent, no worktree, no
> merge, no lane-authored draft: the user instructed the `coordinator` directly, in the main tree
> on `master`, to fix `LayerFor` and the clause that pins it. The transcription clause licenses
> carrying across what a lane already wrote, AFTER A MERGE; neither condition holds, so this
> block and not that clause is what makes the write readable. **Acting and writing are attributed
> separately even though they are the same agent** -- that both were the `coordinator` is the
> fact, not an omission. It licenses this write and nothing else.]**

- **2026-08-29, `coordinator` (ACTING AND WRITING; see the block above) -- THE DEBT THE PASS
  BELOW DEFERRED IS PAID. `LayerFor`'S FIND PATH NOW RE-READS THE MESH CONFIGURATION, AND THE
  CLAUSE ITS DEFERRAL WAS CONDITIONED ON LANDED WITH IT.** The bullet below states the discharge
  condition as *"a clause under `Source/StratPlay/Tests/` asserts that a board applied unmeshed,
  given a mesh, and applied again DRAWS -- red over today's `LayerFor` -- and the three lines land
  with it."* All three halves are met and the bullet is stamped where it sits.
  - **WHAT CHANGED, AND IT IS FOUR LINES RATHER THAN THREE BECAUSE THE MESH DECISION WAS
    EXTRACTED FIRST.** `AssignTerrainMesh(UHierarchicalInstancedStaticMeshComponent&, FName)` is
    the only place that decides which mesh a tile layer wears -- `TerrainMeshes` first,
    `FallbackTerrainMesh` second, neither is legitimate -- and both `LayerFor` paths call it. The
    creation path is unchanged in behaviour; the find path calls it for any layer that has a
    component. **THE EXTRACTION IS THE POINT AND NOT TIDINESS:
    the defect WAS two sites disagreeing about the mesh** -- creation assigned one and the find
    path did not -- so leaving two copies of the `TerrainMeshes`/`FallbackTerrainMesh` decision
    would have re-created the shape that produced it.
  - **THE FIND PATH RE-READS UNCONDITIONALLY, AND THE FIRST VERSION OF THIS ENTRY SAID THE
    OPPOSITE AND WAS BLOCKED FOR IT. THE RETRACTED CLAIM IS QUOTED RATHER THAN QUIETLY REPLACED,
    because a persisted gate report cites it and a reader must be able to find what was said.**
    RETRACTED>  "NARROWED TO THE NULL CASE, AND THE REASON IS A CHURN REGRESSION AND NOT
    RETRACTED>   CAUTION. A HISM DROPS ITS INSTANCES WHEN ITS MESH IS SET. An unconditional
    RETRACTED>   re-read would therefore call `SetStaticMesh` on every already-drawing layer on
    RETRACTED>   every `ApplyHexes`, turning the no-op refresh the pass below built into a full
    RETRACTED>   repopulate ... A component that has a mesh is not touched."
    **`strat-integration-reviewer` BLOCKED ON IT AND WAS RIGHT ON BOTH HALVES**, gate report
    `Tools/architect/gate_reports/2026-08-29-layerfor-late-mesh.md`, and the same false claim had
    been written into FOUR places at once -- the `.cpp`, the `.h`, this entry and `global.md`'s
    banner. **(1) "Already drawing instances" is false of that call site.** `LayerFor`'s ONLY
    caller is `ApplyHexes`, which returns at `DrawsExactlyTheseHexes` before reaching it and then
    runs `ClearInstances()` on every layer before the loop that calls it -- so a no-op refresh
    never executes the line at all and the component holds nothing when it does. **(2) "A HISM
    drops its instances when its mesh is set" is not supported by the engine.** Neither
    `UInstancedStaticMeshComponent` nor `UHierarchicalInstancedStaticMeshComponent` overrides
    `SetStaticMesh` in UE 5.8. **THE COMMAND IS STATED SO THE CONTROL REPRODUCES**, because the
    first version of this bullet quoted a control figure that did not:
    RETRACTED>  "(measured with a control: the same scan lists 30 other
    RETRACTED>   `UInstancedStaticMeshComponent::` overrides in that file, so it was able to
    RETRACTED>   speak)"
    **THE 30 WAS A `head -30` ON MY OWN PIPELINE, QUOTED AS THOUGH IT WERE A COUNT.** The same
    scan without the cap returns 75. `strat-integration-reviewer` caught it on the re-gate and
    could not reconcile the figure from the record, which is the whole objection: a control
    nobody can reproduce is a control in name only, and this one carried the entire weight of
    "measured" rather than "asserted".
    SUBJECT, over `UE_5.8/Engine/Source/Runtime/Engine/{Private/InstancedStaticMesh.cpp,
    Private/HierarchicalInstancedStaticMesh.cpp, Classes/Components/InstancedStaticMeshComponent.h,
    Classes/Components/HierarchicalInstancedStaticMeshComponent.h}`:
    `grep -rn "SetStaticMesh" <those four>` -> **0 hits**.
    CONTROL, same two `.cpp` files:
    `grep -c "UInstancedStaticMeshComponent::\|UHierarchicalInstancedStaticMeshComponent::"` ->
    **171** and **74**. The scan is amply able to speak; the hook is absent. I had asserted an
    engine behaviour I had no instrument for, in a file whose house style is to record
    measurements as measurements -- and then reached for a truncated pipeline to make the
    correction look measured.
  - **SO THE GUARD WAS REMOVED RATHER THAN RE-ARGUED, AND THAT IS A BEHAVIOUR CHANGE WITH ITS
    OWN PIN.** What actually makes the unconditional form free is measured and quoted in the code:
    `UStaticMeshComponent::SetStaticMesh` opens with `if (NewMesh == GetStaticMesh()) return
    false;`, so an unchanged configuration costs one `TMap::Find` and one pointer compare per hex.
    The widening closes half of what the narrowing would have left standing -- a terrain whose
    `TerrainMeshes` entry or fallback CHANGES after its layer exists now follows -- and
    `tests.md`'s clause gained a second half asserting exactly that, proved red against a mutant
    that restores the narrowed guard.
  - **WHAT IS STILL NOT CLOSED, AND IT IS THE OTHER HALF OF THE REVIEWER'S OBSERVATION.** A mesh
    reconfiguration is NOT a model change, so on an UNCHANGED hex list `DrawsExactlyTheseHexes`
    early-outs and `LayerFor` is never reached -- reassign `TerrainMeshes` or `FallbackTerrainMesh`
    on a live board and nothing happens until the model itself moves. **THIS IS NOT A `LayerFor`
    DEFECT AND CANNOT BE FIXED THERE**: the board has no invalidation path for a configuration
    change at all, which is a larger question than this pass. The clause's second half uses a
    CHANGED model precisely so that it pins the widening and does NOT accidentally assert this
    away. **DISCHARGED WHEN** the board grows a way to be told its mesh configuration moved, with
    a clause over it; until then a mesh reassigned on a live board is a restart.
  - **THREE PROSE SITES THAT WOULD HAVE GONE FALSE ARE CORRECTED IN THE SAME CHANGE, WHICH IS THE
    HAZARD THIS PROJECT HAS ALREADY BEEN BITTEN BY: a comment that no diff touches can be
    falsified by the change beside it.** `DrawsExactlyTheseHexes`'s block, `ApplyHexes`'s
    declaration paragraph, and `LayerFor`'s own declaration all asserted the defect as live. The
    `RETRACTED>` lines are KEPT and the surrounding sentences moved to the past tense, so the
    record of what was false on the day it was written survives; the block still refuses to claim
    the cure for itself, because it is `LayerFor`'s and not the early-out's.
  - **BUILD AND SUITE.** `Build.bat StratocracyEditor Win64 Development` with the absolute
    `.uproject`, `-waitmutex` and `-NoHotReloadFromIDE`: `Result: Succeeded`. Rebuilt before the
    suite -- and again after each mutant and again after restoring, since a clause NAME is
    compiled. The live suite figure and its report identity are `global.md`'s and are not
    restated here. The clause's own red-over-the-mutant proof is `tests.md`'s topmost entry, and
    there are TWO mutants now, one per half of the fix.
  - **STILL TRUE AND NOT CLOSED BY THIS:** the fix is not reachable on any shipping path.
    Production meshes are Blueprint defaults set before the first `ApplyHexes`, so only a fixture
    or a runtime editor assignment reaches the find path with a null mesh -- which is why the
    defect survived unnoticed since phase 3, and why nothing a human can see at the keyboard
    changes today. The clause is the whole of the evidence.

- **2026-08-29, `strat-gameplay-engineer` -- THE TILES FLICKERED UNDER THE CURSOR, AND THE CAUSE
  WAS A PER-TURN REBUILD THAT HAD SILENTLY BECOME A PER-HOVER ONE. `ApplyHexes` CLEARED AND
  RE-ADDED ALL 99 TILE INSTANCES EVERY TIME THE CURSOR CROSSED A HEX BOUNDARY.** In the
  integration tree `E:/MultiAgent/Stratocracy` on branch `master`, from `1a3520b`. TWO FILES
  MODIFIED, both under `Source/StratPlay/`: `StratBoardActor.h` and `StratBoardActor.cpp`. No
  test, nothing under any `Tests/` directory, no asset, no `Content/`, no `Config/`, no `Data/`,
  nothing under `Source/StratRules/`, no `.Build.cs`, no `.uproject`.
  - **THE USER'S REPORT, IN THEIR WORDS:** *"when mousing over the hex tiles, they flicker in
    what seems to me a switch between visible and hidden"*.
  - **THE CALL CHAIN, MEASURED IN THIS TREE RATHER THAN TAKEN FROM THE DISPATCH.**
    `AStratPlayerController::Tick` calls `UpdateHoverFromCursor` unconditionally every frame;
    that ends at `ApplyHoverChange`, which early-outs ONLY when the resolved hex is unchanged
    and otherwise calls `RefreshFromMachine`; which calls `UStratMatchSubsystem::ApplyView`;
    which calls `AStratBoardActor::ApplyHexes`; which calls `ClearInstances` on every terrain
    layer and then `AddInstance` per hex. So one hex crossing rebuilt every tile instance on the
    board. Confirmed at each hop by reading the symbols, not by grep alone.
  - **THE REGRESSION IS DATED AND IT IS NOT `ApplyHexes`'S AUTHOR'S ERROR.** That method's own
    prose licensed the full rebuild on the grounds that "the cost is paid once" -- true while a
    refresh happened per turn. Commit `1da4198` (2026-08-27) moved hover off an Enhanced Input
    handler that had been measured never to fire and onto the `Tick` poll, which is what turned
    a per-turn rebuild into a per-hover one. **NO CLAUSE CAUGHT IT AND NO CLAUSE COULD:** every
    automation test here runs headless with no viewport and no cursor, so nothing in the suite
    calls `ApplyHexes` twice with the same model and asks what it did.
  - **ONE PART OF THE DIAGNOSIS I WAS HANDED IS RECORDED AS UNMEASURED, NOT AS FACT.** That the
    HISM's async cluster-tree rebuild is specifically what makes the churn present as
    *visible/hidden* rather than merely as cost is an INFERENCE. It was handed to me as an
    inference and I did not measure it: it needs a viewport, and this box's instruments are
    headless. What IS measured is the churn itself and the chain above. `bAutoRebuildTreeOnInstanceChanges`,
    `BuildTreeIfOutdated` and cull distances are set nowhere in this tree -- re-checked, still
    true -- so nothing tunes the rebuild and nothing needed to be untuned to fix it.
  - **THE FIX IS AN IDEMPOTENT RECONCILE, AND IT IS IN THE BOARD RATHER THAN IN THE HOVER PATH.**
    New private `AStratBoardActor::DrawsExactlyTheseHexes` walks the model with one cursor per
    terrain layer and answers whether each layer's `InstanceHexes` already spells exactly that
    sequence, with the component's own `GetInstanceCount()` agreeing. `ApplyHexes` asks it first
    and returns `true` having touched nothing when the answer is yes; a `false` runs the
    identical whole-board rebuild it always ran. **NO NEW STATE AND NO CACHED MODEL** -- the
    comparison reads only the arrays picking already depends on, so a "yes" is a statement about
    what is on screen rather than about what was last requested, and there is nothing that can
    go stale against a model.
  - **WHY NOT IN `ApplyHoverChange`.** A hover-only bypass was the other available shape and it
    was refused: `AStratPlayerController` states that one refresh decision, not one per caller,
    is load-bearing, and `RefreshFromMachine`'s block gives the reason -- a second, subtly
    different sequence is what the next caller writes when there is no first one to call. The
    board-side fix also gives the no-op to every other refresh path rather than to the one
    caller that revealed the need.
  - **THE THREE INVARIANTS THE FIX HAD TO SURVIVE, AND HOW EACH DOES.** (1) *Reconciled, not
    evented*: `ApplyView` still runs in full on every refresh and what is drawn after
    `ApplyHexes` is a function of `Hexes` alone; the method became idempotent, not incremental
    -- it computes no delta and applies no patch, so `FStratTerrainLayer`'s "REBUILT WHOLE,
    NEVER PATCHED" rule is intact with a second outcome added ("not touched"). (2) *Picking*:
    `InstanceHexes` is never written on the early-out path, so `HexAtInstance`'s index map is
    the one the last rebuild produced and `StratBoardPicking.cpp` is unaffected. (3) *No rules
    answer moves into the engine* -- this class still asks nothing.
  - **THE EARLY-OUT IS CONSERVATIVE BY CONSTRUCTION, AND OF THE THREE WAYS IT COULD HAVE LIED,
    TWO ARE CLOSED IN THE COMPARISON AND THE THIRD IS CLOSED ONLY AS FAR AS THE COMPARISON
    REACHES.** [CORRECTED 2026-08-29, LATER THE SAME DAY, BY `strat-gameplay-engineer` -- MY
    OWN ENTRY, MY OWN LANE, MY OWN FILE -- on `strat-integration-reviewer`'s F1, gate report
    `Tools/architect/gate_reports/2026-08-29-boardchurn-hover-flicker.md`, VERDICT: BLOCK. The
    heading above said all three were CLOSED, and this bullet ended:
    RETRACTED>  "A terrain whose mesh was unset contributed no instances, so its first hex runs
    RETRACTED>   off an empty array and the rebuild runs -- which is what makes a mesh assigned
    RETRACTED>   later take effect on the next refresh instead of never."
    **THE FINAL CLAUSE IS FALSE OF THE BOARD AND I FOUND IT MYSELF BEFORE THE GATE DID.** I
    reported the `LayerFor` defect to the coordinator as blocked-and-not-fixed and then wrote
    the record as though it were not there -- the failure is not that the defect was missed, it
    is that a bullet listing what the early-out cannot swallow SWALLOWED IT. `tests.md`, written
    in the same change set, recorded the same fact as OPEN, so the record contradicted itself
    across two files in one pass, and `strat-test-author` confirmed the board does not draw.]
    **WHAT IS TRUE.** A terrain whose mesh was unset contributed no instances, so its first hex
    runs off an empty array and `DrawsExactlyTheseHexes` returns `false` -- the early-out never
    remembers an undrawn board as drawn, which is the part that is mine and is closed. THE
    REBUILD THEN RUNS AND STILL DRAWS NOTHING: `LayerFor` returns an existing layer by
    `TerrainId` before reaching its `SetStaticMesh` calls, so a component created during an
    unmeshed apply keeps a null mesh for the life of the actor, and `ApplyHexes` skips and
    re-reports every hex of that terrain. **THE DEFECT IS OLDER THAN THIS EARLY-OUT AND IS
    UNCHANGED BY IT** -- it behaves today exactly as it did at `1a3520b`.
    A layer still holding instances for a terrain the model dropped fails the end-of-layer
    cursor check; a component cleared behind this class's back fails the instance-count
    agreement; both of those ARE closed. Any doubt answers `false` and costs exactly one
    rebuild, which is what every call cost before today.
  - **[STAMPED 2026-08-29, LATER THE SAME DAY, BY THE `coordinator` (out-of-lane; see the block at
    the top of this file). THE BULLET DIRECTLY BELOW IS DISCHARGED, and is stamped where it sits
    rather than deleted so that a reader arriving by a citation lands on the discharge.** Its
    reasoning stands as written and its condition was met on its own terms: the clause exists, it
    is red over the old `LayerFor`, and the code change landed with it. The topmost entry in this
    file is the account.]**
  - **`LayerFor` IS DELIBERATELY NOT FIXED IN THIS PASS, AND THE DECISION IS MINE AND IS
    RECORDED RATHER THAN LEFT AS SILENCE.** The gate did not require the fix and the coordinator
    put the call to me. THE FIX IS THREE LINES: `LayerFor`, on the early-return path for an
    existing layer, re-reads `TerrainMeshes` / `FallbackTerrainMesh` and calls `SetStaticMesh`
    when the component currently has none -- narrowed to the null case precisely so it can
    never disturb a component that is already drawing instances. **WHY NOT NOW, in order of
    weight.** (1) `strat-integration-reviewer`'s Observation 4 measures that
    `GATE-BOARDCHURN.AnUnmeshedBoardIsNeverRememberedAsDrawn` is an `if/else` over the branch
    the board took and asserts on both, so **the suite contains no clause that would go red the
    day `LayerFor` is fixed** -- landing an unpinned behaviour change during a BLOCK
    remediation is the scope creep the coordinator asked me not to commit without a decision.
    (2) It is not reachable on any shipping path: production meshes are Blueprint defaults set
    before the first `ApplyHexes`, so only a fixture or a runtime editor assignment can reach
    it, which is why it has survived unnoticed since phase 3. **DISCHARGED WHEN** a clause under
    `Source/StratPlay/Tests/` asserts that a board applied unmeshed, given a mesh, and applied
    again DRAWS -- red over today's `LayerFor` -- and the three lines land with it.
  - **THE OVERLAYS GOT THE SAME GUARD, AND THAT IS A DELIBERATE SCOPE EXTENSION.** `FillOverlay`
    cleared and refilled `ReachOverlay`, `TargetOverlay` and `ObjectiveOverlay` on every refresh
    too, and the reach and target sets are a function of the SELECTION -- so a hover crossing
    repainted all three with content that had not changed, the identical defect on three more
    HISMs. It now takes its caller's drawn-hex cache (`ReachDrawnHexes`, `TargetDrawnHexes`,
    `ObjectiveDrawnHexes`) and returns early on an exact match, and it is no longer `const`.
    **THE CACHE RECORDS WHAT WAS DRAWN AND NEVER WHAT WAS REQUESTED**: an overlay with no
    `OverlayMesh` resets it, because caching the request there would make an `OverlayMesh`
    assigned afterwards -- which is what `BeginPlay` and several fixtures do -- draw nothing
    forever behind an early-out that believed a highlight was already on screen. The overlays
    carry `NoCollision`, so none of this touches picking.
  - **BUILD AND SUITE.** `Build.bat StratocracyEditor Win64 Development` with the absolute
    `.uproject` and `-waitmutex`: `Result: Succeeded`, 25 actions, no warning naming either
    file. Rebuilt before the suite. The suite was re-run and no clause changed state; **the
    count is `global.md`'s to state and this change did not move it** -- no clause was added or
    removed here, because `Tests/` is not this lane's.
  - **WHAT THIS DOES NOT DO, AND THE CLAUSE IT NEEDS.** Nothing here proves the flicker is gone
    to a human eye -- that is a viewport observation and this lane has no viewport. What IS
    headlessly provable and is NOT yet proved is the property the fix rests on: that a second
    `ApplyHexes` with the same model adds no instance and preserves the index map.
    `strat-test-author` was dispatched for it separately; until that clause exists, **the
    early-out is protected by nothing but this entry**. **AND THE CLAUSE MUST PIN THE CHURN AND
    NOT THE OUTCOME, WHICH IS THE HARD PART AND IS WHY IT IS WRITTEN DOWN HERE.** The old code
    and the new code agree on every OUTCOME a headless assertion can read -- same instance
    count, same `InstanceHexes`, same `HexAtInstance` answers -- because a full rebuild from the
    same model produces exactly what it produced before. An assertion over those passes over the
    defect, which is the shape this project has been caught by before. What distinguishes them
    is whether the renderer was touched at all: the identity of the instance transforms across
    the second call, or an `AddInstance`/`ClearInstances` count. DISCHARGED WHEN a clause under
    `Source/StratPlay/Tests/` goes red over a build with the `DrawsExactlyTheseHexes` early-out
    removed.

> **[OUT-OF-LANE WRITE, 2026-08-29. THE TWO ENTRIES DIRECTLY BELOW WERE WRITTEN INTO THIS FILE
> BY THE `coordinator`, WHOSE FILE THIS IS NOT.** This file's header names
> `strat-gameplay-engineer` its sole writer, and that rule is not weakened by what follows.
> **THE ACTING AND THE WRITING ARE ATTRIBUTED SEPARATELY, because they were done by different
> agents and naming one while leaving the other unnamed is itself a finding.** The C++, the
> builds, the design decisions and the prose of both entries are `strat-gameplay-engineer`'s,
> produced in two lane worktrees and delivered as draft record entries in its lane reports; the
> `coordinator` transcribed them here and wrote this block. The user directed it in those terms
> on 2026-08-29, after the merge, having been offered the alternative of four lane dispatches
> writing into two shared files.
> **THE REASON THE DRAFTS EXISTED AT ALL is a lane-concurrency constraint and is worth keeping:**
> two engineer lanes ran at once against one `engine.md`, so both were instructed NOT to write
> this file in their trees, because two lanes editing one record file conflict on rebase. The
> merge protocol already puts the record entry after the merge, on the integration tree, so the
> instruction cost nothing and removed a conflict.
> **WHAT THIS BLOCK DOES NOT DO:** it licenses this transcription and nothing else. It is not a
> standing fallback writer for this file, and it does not cover a `coordinator` writing an entry
> it authored itself. **AND IT IS THE THIRD OUT-OF-LANE WRITE RECORDED IN THIS DIRECTORY IN TEN
> DAYS** -- `content.md` has a standing fallback writer, `tests.md` took one on 2026-08-29
> earlier the same day, and this is the third. A third exception is a clause forming rather than
> three coincidences, and whether `Tools/architect/state/` should say plainly that the
> `coordinator` transcribes lane-authored entries after a merge was the user's to rule.
> **[RULED 2026-08-29 BY THE USER, LATER THE SAME DAY. THE QUESTION THIS BLOCK LEFT OPEN IS NOW
> CLOSED AND THE SENTENCE ABOVE IS STAMPED RATHER THAN DELETED.** This file's sole-writer line
> now carries the exception in the user's own words -- the `coordinator` transcribes
> lane-authored entries after a merge, attributing acting and writing separately -- so a future
> transcription no longer needs a block of its own, and this one stands as the account of how
> the clause came to exist. **THE COST THE USER NAMED IN RULING IT:** it removes the drift risk
> this pass measured, and costs a little of the strictness that makes ownership legible. The
> separate-attribution requirement is what buys that legibility back, and it is not optional.
> `CLAUDE.md` is the authority on the clause's limits.]**

- **2026-08-29, `strat-gameplay-engineer` (ACTING; entry TRANSCRIBED by the `coordinator` under
  the block above) -- W5: SEC 2.11.2'S AI TURN PLAYBACK. THE HEADLESS AI RESOLVED ITS TURN
  INSTANTLY AND THREW THE SEQUENCE AWAY; THE ONLY TRACE A HAND-OVER LEFT WAS A RUN OF
  `STRAT-AI applied` LOG LINES, AND A LOG LINE IS NOT SOMETHING A CAMERA CAN STEP TO.** In the
  lane tree `E:/MultiAgent/Strat-wt/slot-1`, branch `feat/ai-turn-playback`, from `b58a827`,
  landed on `master` in merge commit `69d0029`. FIVE FILES MODIFIED AND TWO ADDED, all the
  engineer's and all under `Source/StratPlay/`: added `StratAiPlayback.h` and
  `StratAiPlayback.cpp`; modified `StratAiTurnRunner.h`, `StratAiTurnRunner.cpp`,
  `StratMatchSubsystem.h`, `StratMatchSubsystem.cpp` and `StratPlayerController.cpp`. No asset,
  no `Content/`, no `Config/`, no `Data/`, nothing under `Source/StratRules/`, no test, nothing
  under any `Tests/` directory, no `.Build.cs`, no `.uproject`.
  `Source/StratPlay/Tests/StratAiPlaybackClauses.cpp` in that tree is `strat-test-author`'s and
  is recorded in `tests.md`. NO SUITE FIGURE AND NO VERDICT IS STATED HERE; `global.md` owns
  both, and this entry links to it rather than restating it.
  - **THE BUILD, IN THE LANE TREE, AFTER THE SECOND FIX BELOW.** `Build.bat StratocracyEditor
    Win64 Development -project="E:\MultiAgent\Strat-wt\slot-1\Stratocracy.uproject" -waitmutex
    -NoHotReloadFromIDE -MaxParallelActions=10`, redirected to a file with `$?` captured on the
    next line, never piped. The tool's own line is `Result: Succeeded`, `Total execution time:
    23.74 seconds`; a case-insensitive `grep -c "warning|error"` over the whole 49-line log
    returns **0**. `-NoHotReloadFromIDE` is the LANE TREE's flag -- the Live Coding mutex is
    machine-wide -- and would be WRONG on the integration tree. The merged tree was built and run
    separately by the `coordinator`; that is `global.md`'s.
  - **THE SYMBOLS ADDED**, cited by name because a `file:NNN` written during a diff is
    invalidated by that same diff. New types: `FStratAiPlaybackStep`, `FStratAiPlaybackReel`. New
    on `UStratMatchSubsystem`: `SkipAiPlayback`, `IsAiPlaybackRunning`, `GetAiPlaybackStepCount`,
    `GetAiPlaybackCursor`, `AdvanceAiPlaybackOneStep`, `BeginAiPlayback`, `OnAiPlaybackTimer`,
    `EndAiPlaybackTour`, `FocusPlaybackStep`, and the members `AiPlaybackTimer` and
    `AiPlaybackReel`. New on `FStratMatchConfig`: `AiPlaybackStepSeconds`.
    `FStratAiTurnRunner::RunTurn` gained a defaulted `FStratAiPlaybackReel*`.
  - **THE SHAPE: THE PRESENTATION IS SPLIT AND THE EXECUTION IS NOT, AND THE PARAMETER THAT LOOKS
    LIKE A REVERSAL IS WHAT MAKES THE SEPARATION POSSIBLE.** `StratAiTurnRunner.h` refuses a
    per-command delay in terms -- "resumable state is a mirror of the rules state by another
    name" -- and that ruling is untouched: the reel is WRITTEN and never read inside `RunTurn`, so
    nothing about what the AI does next depends on it and the turn still resolves in one
    synchronous call. Pass `nullptr` and the match is bit-identical, which is the falsifiable form
    of the claim. That bullet in `StratAiTurnRunner.h` now carries a dated amendment beside it
    rather than reading as contradicted.
  - **AND THE BOARD DOES NOT REWIND, WHICH IS THE LOAD-BEARING CALL.** Playback starts after
    `RefreshPresentation` has reconciled the actors to the FINAL view model, so Sec 2.11.2's
    "camera stepping to each" is a tour of the hexes the AI acted on, over a finished board. THE
    ALTERNATIVE WAS A PER-STEP BOARD and it was rejected: the only way to draw it is a mid-turn
    copy of the rules state to reconcile against, and that copy is the mirror both this runner and
    `FStratSelectionMachine` refuse. Sec 2.11.2's own "presentation pacing only, no rules change"
    licenses the reading and names the camera as the thing that steps. **The consequence is stated
    rather than hidden:** "any click or Esc skips to the end state" is satisfied TRIVIALLY here,
    because the end state was never absent -- the skip abandons a tour and fast-forwards no
    simulation.
  - **IT IS A RECORDING AND NOT AN INFERENCE.** `StratUnitActor.h` records that Sec 4.9 part 2's
    ordered event list is not built in this tree and that reconstructing an event by diffing two
    states is the conflation `StratBridge.h` warns about; that was re-checked on this tree before
    the design was fixed and was still true. Nothing here diffs anything -- a step is appended at
    the instant `Port.Submit` returned true, so every entry is a command that was submitted rather
    than one someone worked out must have been.
  - **`T-INT-05` DID NOT BECOME OWED, AND THE EVIDENCE IS A FILE THAT DOES NOT APPEAR IN THE
    DIFF.** The 2026-08-29 ruling made that ID conditional on W5 putting a playback cursor into
    the view model. It did not: the reel and cursor live on `UStratMatchSubsystem` beside the
    timer, and `Source/StratUI/StratViewModel.h` is untouched. The rejected alternative -- a
    `PlaybackCursor` on the presentation block so a widget could bind -- would have bought a
    progress readout and cost a clause under an ID whose claim is about rebuilding the screen.
  - **THE SKIP GATES THE WHOLE OF `HandleSelectionEvent` AND SITS AHEAD OF THE CONCLUDED-MATCH
    GATE, WHICH IS AN ORDERING DECISION AND NOT TIDINESS.** Gating the function rather than two
    events by name is what makes the GDD's "any" true over an input set that has grown twice.
    Sec 2.8's primary win most often lands on the AI's own command, so the most interesting
    hand-over to watch is the one after which the match is over -- and the concluded gate refuses
    every event once it is. Below that gate the skip would be unreachable in exactly the case it
    matters most.
  - **AND IT SHIPPED THE SAME DEFECT TWICE. BOTH WERE FOUND IN THIS PASS, BOTH BY SOMEONE OTHER
    THAN ITS AUTHOR, AND THE SECOND IS THE MORE INSTRUCTIVE.**
    - **INSTANCE 1, THE SHIPPED DEFAULT, FOUND BY `strat-test-author` AND CONFIRMED AT THE SOURCE
      BEFORE THE FIX.** `RunAiTurnsNow` passes the reel UNCONDITIONALLY so the count answers in
      any configuration, while only the TIMER was gated on `AiPlaybackStepSeconds`. At the
      default of zero the reel therefore ended each hand-over non-empty with the cursor at 0:
      `IsAiPlaybackRunning()` read true, `SkipAiPlayback()` succeeded, and
      `AStratPlayerController::HandleSelectionEvent` CONSUMED THE PLAYER'S FIRST CLICK OR ESC
      AFTER EVERY AI TURN. **AND THE ENGINEER'S OWN COMMENT IN THAT FILE ASSERTED THE OPPOSITE** --
      that the block was invisible "in the shipped default configuration" -- which is the worst
      way for a prose block to be wrong, because it reads as the reason not to go and check. That
      sentence is STRUCK AND QUOTED at its site, not silently reworded. Fixed by giving
      `BeginAiPlayback` the invariant that the cursor is at the end unless a tour is actually
      under way, retiring the reel on all three paths where it declines to arm a timer.
      **GATING `SkipAiPlayback` ON THE CONFIG WAS THE ALTERNATIVE AND WAS REJECTED BECAUSE OF THE
      THIRD PATH:** a positive interval with NO WORLD leaves the same stuck cursor, and that is
      how a headless caller reaches it.
    - **INSTANCE 2, THE RESEED, FOUND BY `strat-integration-reviewer`, WHICH BLOCKED ON IT -- AND
      THE PROSE IS THE DIAGNOSIS RATHER THAN AN AFTERTHOUGHT.** `BeginAiPlayback` claimed to own
      the invariant because it was "the only one in a position to know". **That is TRUE OF ARMING
      AND FALSE OF DISARMING**, and the invariant constrains both: `StopAiPlaybackTimer` cleared
      only the clock, and of its call sites `Deinitialize` and `TearDownPresentation` ended tours
      that way. `StartMatchInternal` calls the latter unconditionally and `LoadMatchFromSlot`
      reaches it through the same function. So a match at a positive `AiPlaybackStepSeconds` with
      a tour mid-reel, followed by a NEW MATCH OR A SLOT LOAD, began with a stranded cursor over
      the PREVIOUS match's step list: on a brand-new match `IsAiPlaybackRunning()` read true and
      the skip gate swallowed the first input again. The same swallow, reinstated through a door
      the prose said did not exist. The false sentence is struck and quoted in three places.
    - **THE SECOND FIX IS A RENAME AND NOT AN ADDED CALL, AND THE RENAME IS THE LOAD-BEARING
      PART.** `StopAiPlaybackTimer` is now `UStratMatchSubsystem::EndAiPlaybackTour`, which clears
      the timer AND retires the reel, always. **THERE IS NO LONGER A VERB IN THIS CLASS THAT STOPS
      THE CLOCK ALONE**, and the gate's re-audit put that structurally rather than by inspection:
      `AiPlaybackTimer` is touched by exactly TWO statements in the whole tree, `SetTimer` in
      `BeginAiPlayback` and `ClearTimer` in `EndAiPlaybackTour`, so there is no route to the
      handle that bypasses retirement and a later call site cannot reopen the gap. Retiring inside
      a function still named for the clock would have left a name promising less than its body,
      which is exactly how the next site gets added wrong.
    - **THE CALL-SITE CENSUS WAS SIX AND NOT FIVE, AND THE ENGINEER CORRECTED THE GATE ON IT.**
      `AdvanceAiPlaybackOneStep` holds two -- its null-`Peek` arm and its last-step arm -- which is
      where a count of five collapses them. The conclusion did not depend on the count, but "every
      site is safe" is a claim about EVERY site, so the set had to be right; the per-site check is
      recorded on the declaration rather than asserted about the function, and the reviewer
      accepted the correction on re-gate.
    - **`TearDownPresentation` ALSO `Reset()`s THE REEL, AND THAT HALF IS DELIBERATELY NOT IN THE
      VERB.** Retiring alone leaves the previous match's step list readable through
      `GetAiPlaybackStepCount()` on a new match. **THE SPLIT IS BY CONSEQUENCE:** failing to retire
      the cursor SWALLOWS THE PLAYER'S INPUT, so that half is inside the verb and cannot be
      forgotten by a new call site; failing to clear `Steps` can only make a readout report the
      wrong match's count, so that half is one explicit line at the one place a match boundary is
      crossed. Folding it in would break the three sites that need the count to survive a tour
      ending, which is the one discriminator between a tour cut short and a reel never filled.
    - **THE DURABLE LESSON, AND IT IS NOT ABOUT TIMERS:** an invariant stated on the function that
      establishes half of it reads as covering all of it. Grep the CALL SITES of the verb, not the
      prose.
  - **AND THE CAMERA-STEPPING HALF WAS EXECUTED BY NOTHING, SO IT GOT A SEAM.**
    `OnAiPlaybackTimer` was reachable only through `FTimerManager`, so `FocusPlaybackStep`,
    `EndAiPlaybackTour` and the arming path had no coverage at all. The body MOVED to a public
    `AdvanceAiPlaybackOneStep` and the timer callback is now one line calling it -- ONE driver with
    two entry points, not two implementations. A test-only stepping path would have been the
    substitution `T-UI-02` exists to catch, applied to Sec 2.11.2.
  - **IT SHIPS INERT AND THAT IS A DEBT.** `AiPlaybackStepSeconds` is `0.0f` and Sec 2.11.2's 0.5
    is deliberately NOT written in C++ -- a second place the pace is stated would make the
    designer-facing Blueprint default the one nobody could find. **DISCHARGED WHEN** an editor
    batch sets `0.5` on the GameMode Blueprint default and a human watches one AI hand-over. Until
    then the camera path is compiled and reasoned about and has NOT been seen. OWNED: coordinator,
    to schedule. `unreal-editor-direct` did not connect during the session that built this.
  - **THE OTHER DEBTS, EACH WITH ITS DISCHARGE CONDITION.** (a) No on-screen affordance says the
    tour can be skipped; Sec 2.11.2 states the behaviour and not a prompt, so nothing is out of
    spec, but a player who does not know to click will wait. DISCHARGED WHEN the Sec 2.11 UI-layer
    owner binds a hint to `IsAiPlaybackRunning()` / `GetAiPlaybackCursor()`, which are reflected
    for exactly that. (b) No highlight on the stepped hex: `AStratBoardActor`'s three overlay
    components are each spoken for and `ShowObjective`'s own comment rules that a new use gets a
    NEW component, which needs a material. DISCHARGED WHEN an editor batch can author the fourth
    overlay; the C++ hook is `FocusPlaybackStep`, which already has the hex. (c)
    `FocusPlaybackStep`'s camera-resolution arms are still unexecuted -- no board, no player
    controller, a possessed pawn that is not an `AStratCameraPawn`. `AdvanceAiPlaybackOneStep`
    makes the STEPPING reachable; it does not make a wrong-pawn level reachable. DISCHARGED WHEN a
    clause stands up a world with a non-camera pawn. (d) `FStratAiPlaybackReel::Peek` returns a
    pointer into a `TArray`; closed today by ordering, DISCHARGED WHEN a second writer of the reel
    appears.

- **2026-08-29, `strat-gameplay-engineer` (ACTING; entry TRANSCRIBED by the `coordinator` under
  the block above) -- W4: SEC 2.11.2'S ON-MAP MARKERS. THE FLAG `H` AND THE UNACTED PIP. THE VIEW
  MODEL HAS PUBLISHED BOTH BITS SINCE PHASE 2 AND PHASE 4 AND NO DRAWING CODE READ EITHER; A
  PLAYER COULD LEARN WHICH UNIT WAS THE FLAG, AND WHICH OF THEIR OWN UNITS WERE STILL LIVE, ONLY
  BY OPENING THE INFO PANEL ON EACH UNIT IN TURN.** In the lane worktree
  `E:/MultiAgent/Strat-wt/slot-2` on branch `feat/map-markers`, from `b58a827`, landed on `master`
  in merge commit `4084df6`. TWO FILES MODIFIED, NO FILE ADDED, both the engineer's and both under
  `Source/StratPlay/`: `StratUnitActor.h` and `StratUnitActor.cpp`. No asset, no `Content/`, no
  `Config/`, no `Data/`, nothing under `Source/StratRules/`, no test, nothing under any `Tests/`
  directory, no `.Build.cs`, no `.uproject`.
  `Source/StratPlay/Tests/StratMapMarkerClauses.cpp` is `strat-test-author`'s and is recorded in
  `tests.md`. NO SUITE FIGURE AND NO VERDICT IS STATED HERE; `global.md` owns both.
  - **THE BUILD, IN THE LANE TREE.** `Build.bat StratocracyEditor Win64 Development
    -project="E:\MultiAgent\Strat-wt\slot-2\Stratocracy.uproject" -waitmutex -NoHotReloadFromIDE
    -MaxParallelActions=10`, redirected to a file and never piped, `$?` captured on the next line.
    `REAL_EXIT=0` and the tool's own `Result: Succeeded`, `Total execution time: 67.33 seconds`.
    The green is over the edited translation unit rather than an up-to-date tree -- the log names
    `[81/84] Compile [x64] StratUnitActor.cpp`, `[43/84] Compile [x64] Module.StratPlay.gen.cpp`
    (UHT's regenerated reflection for the two new `UFUNCTION`s) and `[83/84] Link [x64]
    UnrealEditor-StratPlay.dll`. A case-insensitive `grep -c "warning|error"` over the whole
    captured log returns **0**.
  - **THE SYMBOLS ADDED**, cited by name because a `file:NNN` written during a diff is invalidated
    by that diff: `AStratUnitActor::FlagMarker`, `FlagMarkerMesh`, `FlagMarkerMaterial`,
    `FlagMarkerOffset`, `UnactedPip`, `UnactedPipMesh`, `UnactedPipMaterial`, `UnactedPipOffset`,
    `IsFlagMarkerVisible`, `IsUnactedPipVisible`, and the private `ConfigureMarker`.
    `ApplyUnitView`'s SIGNATURE IS UNCHANGED and `UStratMatchSubsystem` was not touched: both
    markers read only `FStratUnitView` fields plus the `ViewingSide` that call already passed, so
    no second driver and no new operand entered the subsystem.
  - **AND THE WAVE PLAN'S OWN BULLET NAMED A METHOD THAT DOES NOT EXIST.** `global.md`'s W4
    bullet says `AStratUnitActor::ApplyView`; the per-unit method is `ApplyUnitView`, and
    `ApplyView` is `UStratMatchSubsystem`'s, a layer up. Confirmed absent in the lane tree. The
    2026-08-29 ruling entry already carried the correction and left the bullet standing; it is
    recorded again here because a proposed symbol greps exactly like a defined one.
  - **THE WAVE WAS ALSO NARROWER THAN THAT BULLET READS, AND THE MEASUREMENT IS WHY.**
    Mesh-by-`DefId`, the side material and the guided marker were ALREADY built and applied by
    `ApplyUnitView`. What remained were the two bits projected and read by no drawing code:
    `bIsFlag`, written at `StratViewModel.cpp`'s unit projection, mirrored into the info panel and
    otherwise read only in `Tests/`; and `bDone`, produced by `FStratSelectionMachine` and consumed
    by the controller, the forecast query and the idle count. `StratUnitActor.h` named `bDone`
    three times in prose and read it nowhere.
  - **THE FLAG MARKER TAKES NO SIDE TEST AND THAT IS THE SPECIFICATION, NOT AN OVERSIGHT.**
    Sec 2.11.2's earn-your-pixels row is `Flag "H" marker (both sides, always visible)`. "Both
    sides" forecloses `View.Side == ViewingSide` and "always visible" forecloses any window bit.
    `GuidedMarker`'s side filter -- the adjacent line in the same function -- was a USER RULING
    about a marker that says "select this"; this one says "this is the flag", about a unit the
    other seat is meant to HUNT. The contrast is stated in the code because two adjacent visibility
    writes invite a reader to carry the filter across.
  - **ITS OPERAND SET IS MATCH-CONSTANT, WHICH IS THE EXACT SHAPE OF THE 2026-08-24 LATCH, AND THE
    QUESTION THAT FINDING POSES IS ANSWERED RATHER THAN DUCKED.** `bIsFlag` can no more go false
    than `bIsGuidedMarked` could. What makes it correct here is that Sec 2.11.2 ASKS for permanence
    where Sec 2.11.6 said the turn-1a marker clears -- and the route by which it stops drawing
    exists and is not a visibility write: **the flag unit's death destroys the actor.**
    `FStratViewModel::Units` is every LIVING unit and `UStratMatchSubsystem::ApplyView` destroys the
    actor for any id the model no longer carries; Sec 2.4 ends the match in the same breath. The
    gate confirmed that destruction loop against the code rather than accepting the reasoning, and
    noted honestly that it remains an UNPINNED reasoning step: it lives in a different file, and if
    that loop were ever weakened no clause in `StratMapMarkerClauses.cpp` would notice.
  - **THE PIP IS `!bDone && Side == ViewingSide`, AND `bLockedThisTurn` IS DELIBERATELY NOT AN
    OPERAND.** Sec 2.11.2 binds the pip "via the DONE bit of Sec 2.11.1's machine" and Sec 2.11.1
    enumerates it among the DONE-bound surfaces while naming the alternative's failure in the same
    sentence -- "a waited unit would keep its pip". A locked unit also cannot be ordered, which
    makes the conjunction tempting; adding it would be this class deciding a content rule the GDD
    did not state. The omission is NAMED on the component so it does not read as missed. THIS
    OPERAND SET HAS A REAL FALSE-GOER -- `bDone` moves several times a turn -- which is the check
    the 2026-08-24 correction demands of every new marker on this actor, and which the flag marker
    answers differently.
  - **`BeginPlay`'S EARLY `return` WAS A LIVE HAZARD THE MOMENT THERE WERE THREE MARKERS.** It read
    `if (GuidedMarker == nullptr) { return; }` -- correct with one marker, and with three it would
    have silently skipped the flag marker's and the pip's placement WITH A GREEN BUILD. The guard is
    now per-marker inside `ConfigureMarker` and there is no early exit. ONE HELPER RATHER THAN THREE
    COPIES, AND THE REASON IS THE LOG: the missing-mesh log is the only discriminator the project
    has between an unconfigured marker and an unmarked unit (`IsGuidedMarkerVisible` measured that
    `IsVisible` never consults the static mesh), and an omitted log is silent by construction.
  - **THE OFFSETS ARE `FVector` AND THE SEPARATION AXIS IS DERIVED, NOT EYEBALLED.** At
    `ArmPitch = -60` with no yaw, body-space screen-up is `0.866x + 0.5z` and depth is
    `0.5x - 0.866z`; NEITHER CONTAINS `y`, so Y is the one axis that moves a marker across the
    screen without changing its height or its depth sort. Guided at `(0, 0, 300)` UNCHANGED (the
    scalar is widened at the call site, so its shipped placement is bit-identical), flag at
    `(0, -40, 300)`, pip at `(0, 40, 300)`. 40 is bounded by the tile:
    `AStratBoardActor::LocalLocationOfHex` documents `HexSize` as the CENTRE-TO-CENTRE distance and
    it defaults to 100. **THE Z ASSUMES A 100 uu CENTRED MESH THAT DOES NOT EXIST YET** and the Y
    assumes an unrotated board; both are stated on the property, NO TEST PINS EITHER VECTOR, and the
    gate on the final placement is a human eye as it was for the guided marker.
  - **THE HEADER'S "NOT IN THIS ROUND" BULLET WAS HALF FALSE AND IS RETRACTED FLAT, NOT NESTED.**
    It said phase 5 decides what "done" looks like and that "Sec 2.11 has not said". Both are false
    of `bDone`: Sec 2.11.2's row named the bit AND the seam all along, and the bullet was written
    before the selection machine existed and never re-read against Sec 2.11.2 after.
    `bLockedThisTurn` still has no treatment and that half stands verbatim, named separately,
    because a bullet true of one bit and false of the other reads as wholly true to whoever greps
    for either.
  - **WHAT THIS DOES NOT DO.** Four `EditDefaultsOnly` slots ship UNSET -- `FlagMarkerMesh`,
    `FlagMarkerMaterial`, `UnactedPipMesh`, `UnactedPipMaterial` -- exactly as `GuidedMarkerMesh`
    shipped before 2026-08-24, with no `/Game/` path in this file. Both markers are constructed,
    positioned and correctly toggled on every refresh and DRAW NOTHING until the content lane
    assigns them. `IsFlagMarkerVisible` and `IsUnactedPipVisible` inherit the measured limit
    `IsGuidedMarkerVisible` carries: **they report a flag, not pixels**, and there is no headless
    gate on "a player can see the `H`". DISCHARGED WHEN an editor batch sets the four slots on
    `BP_StratUnit` and a human confirms at the keyboard that three markers 40 uu apart read as three
    things at the shipped camera pitch.
    **[DISCHARGED 2026-08-29, BOTH HALVES, STAMPED HERE RATHER THAN DELETED. WRITTEN BY
    `strat-gameplay-engineer`, WHOSE FILE THIS IS -- ATTRIBUTION ADDED LATER THE SAME DAY ON
    `strat-integration-reviewer`'s F3, gate report
    `Tools/architect/gate_reports/2026-08-29-boardchurn-hover-flicker.md`, VERDICT: BLOCK. THIS
    IS NOT AN OUT-OF-LANE WRITE AND NO CLAUSE LICENSES IT, because none is needed: the file's
    sole writer wrote it, in its own lane, in its own tree, and the `coordinator` did not edit
    this file in this pass. It went unattributed because it reads like somebody else's news --
    the discharging FACTS are content-lane and coordinator-facing (an asset batch, and a human
    at the keyboard) even though the DEBT is engine-lane and the entry carrying it is mine. That
    is exactly the case where a stamp must name its writer rather than rely on the header, since
    a reader arriving by citation lands here and not on any header above.** The editor batch
    landed in `1a3520b` -- which had to author all four assets first, because not one of them
    existed -- and it sets the four slots on `BP_StratUnit`. The human confirmation is the
    user's, at the keyboard, in these words: *"I can confirm all markers and they are good where
    they are. Camera pitch looks good."* **THE MEASURED LIMIT ABOVE IS NOT DISCHARGED WITH IT
    AND STANDS VERBATIM:** `IsFlagMarkerVisible` and `IsUnactedPipVisible` still report a flag
    rather than pixels, and there is still no headless gate on "a player can see the `H`" --
    what was discharged is the SLOTS BEING UNSET and the SEPARATION BEING UNCONFIRMED, which is
    what the condition asked for and all it asked for. A reader arriving here for the visibility
    limit has not had their question answered by this stamp.]

- **2026-08-28, `strat-gameplay-engineer` -- W7: SEC 2.9'S DIFFICULTY HANDICAP. THE GDD HAS
  SPECIFIED IT SINCE THE PROTOTYPE DOCUMENT AND `Source/` IMPLEMENTED NONE OF IT; A DESIGNER
  COULD SET NO TIER AND EVERY MATCH OPENED ON THE SCENARIO'S OWN BASELINE.** In the integration
  tree `E:/MultiAgent/Stratocracy` on branch `master`, from `e8cf7c0`. FOUR FILES MODIFIED, NO
  FILE ADDED, all mine and all under `Source/`: `Source/StratBridge/StratBridge.h`,
  `Source/StratBridge/StratBridge.cpp`, `Source/StratPlay/StratMatchSubsystem.h` and
  `Source/StratPlay/StratMatchSubsystem.cpp`. Nothing committed, nothing staged, no asset, no
  `Content/`, no `Config/`, no `Data/`, nothing under `Source/StratRules/`, no test, nothing
  under any `Tests/` directory, no `.Build.cs`, no `.uproject`. NO SUITE FIGURE AND NO VERDICT
  IS STATED HERE; `global.md` owns both, and this entry links to it rather than restating it.
  - **THE BUILD, IN THIS TREE.**
    `"C:\Program Files\Epic Games\UE_5.8\Engine\Build\BatchFiles\Build.bat" StratocracyEditor
    Win64 Development -project="E:\MultiAgent\Stratocracy\Stratocracy.uproject" -waitmutex`,
    editor closed -- `tasklist | grep -i UnrealEditor` empty before the run WITH the control
    that `tasklist` alone prints its header, because an empty grep proves nothing until the
    instrument is shown able to speak. The tool's own last lines are `Result: Succeeded` and
    `Total execution time: 47.86 seconds`, over 72 actions with `StratBridge.cpp`,
    `StratMatchSubsystem.cpp`, `Module.StratPlay.gen.cpp` and both module links named among
    them -- so the green is over the edited translation units and over UHT's regenerated
    reflection for the new `UENUM`, not over an up-to-date tree. A SECOND RUN WAS MADE WITH THE
    WHOLE LOG CAPTURED rather than tailed, because the first run's compile listing was
    truncated above the `StratBridge` lines and a warning could have hidden there:
    `touch`ed both `.cpp` files, rebuilt, `REAL_EXIT=0`, and a case-insensitive
    `grep -c "warning|error"` over the entire captured log returns **0**.
  - **THE SYMBOLS ADDED**, cited by name because a `file:NNN` written during a diff is
    invalidated by that same diff. New on `FStratBridge`:
    `FStratBridge::ApplyStartingFameHandicap`, `FStratBridge::SideFameTotal` and
    `FStratBridge::SideFameCombat`. New in `StratMatchSubsystem.h`: `EStratDifficulty`,
    `StratDifficultyFameDelta`, `StratHandicappedSide` and the `FStratMatchConfig::Difficulty`
    field. `UStratMatchSubsystem::StartMatchInternal` gained the call site, between seeding and
    the restore.
  - **THE SHAPE: A DELTA THROUGH THE BRIDGE, AND NO TIER NUMBER INSIDE `StratBridge`.**
    `Scenario.h` states in its own words that the handicap is "a match-setup parameter applied
    on top" and deliberately not a scenario field, so nothing on the seeding path can apply it
    and something after `LoadScenarioFromFile` had to. That something touches
    `strat::EconomyState`, which is a `strat::` type, so the mutation sits behind an
    `FStratBridge` method and `StratPlay` names no `strat::` type to reach it. The bridge takes
    a DELTA and never an absolute: Sec 2.7 calls the 200 "a baseline, not a constant, for the
    player" and `T-FAME-02` says a gate must assert "each side's configured value and never a
    literal 200", so **this module authors no opening-Fame value at all**: it returns three
    deltas, and THE ONLY STATEMENT IN `StratBridge`, `StratPlay` OR `StratUI` THAT WRITES A
    SIDE'S `fameTotal` IS THE ONE IN `FStratBridge::ApplyStartingFameHandicap`, which ADDS a
    delta to a number this code did not choose. `Data/ferrum_crossing.json` and
    `strat::initSide` own the absolute. Cited by SYMBOL rather than by a grep pattern, for the
    reason the correction below records.
    **[CORRECTED 2026-08-28 ON THE `coordinator`'S FINDING, IN THIS ENTRY AND AT THE CODE SITE,
    AND THEN CORRECTED A SECOND TIME BY MY OWN MEASUREMENT OF THE FIRST FIX.
    STRUCK: `350 and 100 appear nowhere in Source/`. That was subject-versus-scope -- a claim
    about TEXT IN A DIRECTORY standing in for a claim about VALUES IN CODE -- and a search for
    those numerals returns the sentence that said it, along with every other line of prose about
    the rule. STRUCK ALSO, AND THIS ONE WAS MINE ALONE: the first fix cited
    `a grep for a fameTotal assignment` as returning EXACTLY ONE LINE. It did when I ran it, and
    it stopped being true the moment the fix landed, because the fix QUOTED THE ASSIGNMENT
    VERBATIM in a comment and the quotation is matched by the same search -- re-measured, two
    hits, the second being my own sentence. The same defect one level down. Both are now stated
    as a claim about the symbol that writes the field, which no amount of prose about it can
    falsify.]** The rejected
    shape was an
    absolute `SetStartingFame(Side, Amount)`, which reads more simply and makes this module a
    second author of the scenario's `startingFame`; that is what killed it.
  - **`fameCombat` DOES NOT MOVE, AND THE MUTATION IS ONE STATEMENT SO THAT THE CLAIM HAS ONE
    STATEMENT TO CHECK.** Sec 2.8's first tiebreak key is `fameCombat` and `T-TURN-05`'s
    mutual-passivity guard fires on both sides reading zero at the cap, so a handicap that
    touched it would move the victory condition and the draw condition at once -- and would do
    so invisibly, since the scoreboard shows both numbers and nothing on screen compares either
    to the tier. `Economy.h` already says this from the other side about passive income
    (`T-FAME-01`); this is the second writer that had to be told, and the only other one the
    module has. `SideFameCombat` exists so the non-movement is observable from outside
    `StratBridge` without projecting a snapshot first.
  - **THE INERTNESS IS A `coordinator` RULING AND IS LABELLED AS ONE AT THE CODE SITE.**
    `StratHandicappedSide` returns `INDEX_NONE` when `AiSides` is empty and when `AiSides`
    contains `ViewingSide`; otherwise it returns `ViewingSide`. The whole ruling lives in that
    one function so overturning it is one edit, and `FStratMatchConfig::Difficulty`'s own block
    says in terms that the reading is a two-word reading of Sec 2.7's "Single-player
    difficulty" and NOT a GDD quotation. The alternative shape was to take the complement of
    `AiSides` over the side count; it gives the same answer on two sides and needs `StratPlay`
    to learn a side count that `AiSides`' own "A LIST AND NOT TWO BOOLS" block refuses to name.
    That is what killed it.
  - **THE DEFAULT IS `Easy` AND IS INERT, AND THOSE ARE TWO FACTS RATHER THAN ONE.** Sec 2.11.6
    says the first match runs at Easy by default, so `Easy` is the honest C++ default;
    `AiSides` defaults empty, so on the shipped hot seat that default reaches
    `StratHandicappedSide` and comes back `INDEX_NONE`. `Normal` would have made the inertness
    true by arithmetic instead of by configuration and would have hidden the ruling above,
    which is why it was not chosen. NO BLUEPRINT DEFAULT AND NO `Content/` CHANGE WAS MADE.
  - **EVERY EXISTING TEST CONFIGURATION IS INERT UNDER THAT FUNCTION, MEASURED RATHER THAN
    ASSUMED, AND NO TEST WAS EDITED.** `grep -rn "\.AiSides" --include=*.cpp Source/` returns
    twelve assignment sites across seven test files plus the two live reads in
    `StratMatchSubsystem.cpp`. Every one was read at its site. The `BothAi` cases build their
    side list from the projection's own units, so `AiSides` names every side and therefore
    contains `ViewingSide`. The three single-AI cases in `StratAiMatchClauses.cpp` and
    `StratMatchConclusion.cpp` set `AiSides` to `FirstSide`, and their shared `MakeConfig`
    writes `Out.ViewingSide = kFirstSide` beside `Out.FirstSide = kFirstSide`, so those contain
    it too. `StratProductionMenuSeam.cpp`'s is the one that would have moved a purse assertion,
    and its own comment reads "The side whose screen this is plays itself" -- `AiSides` is
    `{0}` and `ViewingSide` is 0. So the handicap is unreachable from the existing suite, which
    is a property of the ruling and not a coincidence.
  - **THE SAVE HAZARD: THE HANDICAP IS APPLIED BEFORE THE RESTORE, AND `T-SAVE-06`'S REFUSAL
    NOW NAMES IT AS A FOURTH CAUSE.** `FStratBridge::RestoreFromSaveText` replays the save's
    log onto whatever this bridge holds and compares `strat::canonicalStateHash`. The handicap
    is not a `strat::SaveCommand`, so the log cannot carry it, and not a scenario field, so
    `scenarioHash` cannot either. Applying it AFTER the restore would shift a state whose hash
    had already been certified, and a slot would load "successfully" into a match worth 150
    Fame more than the one saved; applying it BEFORE makes a tier mismatch a refusal instead.
    The refusal's message was changed from "the log, the definitions or the seeding side" to
    "the log, the definitions, the seeding side or the Sec 2.9 difficulty handicap", because it
    fails safe either way but named three causes when there were four. **CHECKED FIRST THAT NO
    CLAUSE ASSERTS THAT TEXT, since `Tests/` is not mine to fix:**
    `StratBridgeRestoreParity.cpp`'s `RestoreRefusesAMutatedStateHash` asserts
    `EqualsExact(Refused.Id, TEXT("T-SAVE-06"))` and
    `ContainsExact(Refused.Reason, *HonestHash) || ContainsExact(Refused.Reason, *MutatedHash)`
    -- the id and the two hashes, never the prose between them. Nothing else in `Source/`
    matches `seeding side`.
  - **A DEBT, WRITTEN DOWN WHERE THE CONDITION THAT DISCHARGES IT CAN BE READ.**
    `ApplyStartingFameHandicap` CLAMPS AT ZERO. A negative purse is a state `strat::initSide`
    has no way to produce and no rule reads as debt -- `queueBuild` refuses everything at 0 and
    would refuse everything at -50 -- so the clamp removes an unobservable distinction. It is
    reported through `OutFameTotalAfter` rather than refused, because Hard against a 50-Fame
    scenario is a scenario/tier mismatch and not an unplayable match. The condition that
    discharges it: if a tier is ever specified whose debt must be real, the clamp is the line
    to delete and `strat::queueBuild` is where the behaviour must then be checked -- upstream,
    in `E:\MultiAgent\stratocracy-crew`, not here.
  - **WHAT IS DELIBERATELY NOT IN THIS PASS.** No `UStratMatchSubsystem` accessor projects the
    tier into `FStratViewModel`, so no widget can show the difficulty; Sec 2.9 asks for a
    handicap and not for a readout, and a view-model field with no clause and no widget is the
    inert projection this lane already recorded once on the info panel. No menu sets the tier;
    it is a Blueprint default like every other `FStratMatchConfig` field, and authoring that
    default is `Content/` work and another lane's.

- **2026-08-28, `strat-gameplay-engineer` -- §2.11.2's INFO PANEL, WIDGET-SIDE. THE
  PROJECTION WAS BUILT, PINNED AND REACHABLE BY NOTHING; A WBP AUTHORED AGAINST IT WOULD HAVE
  BEEN INERT.** In the integration tree `E:/MultiAgent/Stratocracy`, from `63433e7`. Two files
  added and three modified, all mine and all under `Source/`: added
  `Source/StratUI/StratInfoPanelWidget.h` and `Source/StratUI/StratInfoPanelWidget.cpp`;
  modified `Source/StratUI/StratScoreboardHUD.h`, `Source/StratUI/StratScoreboardHUD.cpp` and
  `Source/StratPlay/StratMatchSubsystem.cpp`. Nothing committed, nothing staged, no asset, no
  `Content/`, no `Config/`, no test, nothing under any `Tests/` directory, no `.Build.cs`, no
  `.uproject`. No suite figure is stated here; `global.md` owns that and owns any verdict.
  - **THE BUILD, IN THIS TREE.** `Build.bat StratocracyEditor Win64 Development` against
    `E:\MultiAgent\Stratocracy\Stratocracy.uproject` with `-waitmutex`, editor closed
    (`tasklist | grep -i UnrealEditor` empty before the run, which is the control the write-lock
    failure mode needs). The tool's own last lines are `Result: Succeeded` and
    `Total execution time: 18.09 seconds`, over 21 actions with `StratInfoPanelWidget.cpp`,
    `StratScoreboardHUD.cpp`, `StratMatchSubsystem.cpp` and `Module.StratUI.gen.cpp` all named
    among the compiles -- so the green is over the edited translation units and over UHT's
    regenerated reflection, not over an up-to-date tree. Zero warnings, zero errors, first
    attempt. `-NoHotReloadFromIDE` was NOT needed and was not passed: this is the integration
    tree with the editor closed, not a linked worktree racing the engine-keyed Live Coding
    mutex. **18 s and not the ~216 s a slot costs**, because only two modules relinked.
  - **THE SYMBOLS ADDED**, cited by name because a `file:NNN` written during a diff is
    invalidated by that same diff. New in `StratInfoPanelWidget.h`: `EStratHexOwnership`,
    `FStratInfoPanelModel`, `StratComposeInfoPanelModel`, `UStratInfoPanelWidget` with
    `UStratInfoPanelWidget::PushInfoPanel`, `UStratInfoPanelWidget::Model` and
    `UStratInfoPanelWidget::OnInfoPanelRefreshed`. New on `AStratScoreboardHUD`:
    `InfoPanelWidgetClass`, `InfoPanelZOrder`, `InfoPanel`, `CreateInfoPanelWidget`,
    `PushInfoPanel`, `DeliverLatestInfoPanel`, `LastPushedInfoPanel`,
    `LastPushedInfoPanelViewingSide`, `bInfoPanelEverPushed`.
  - **THE BRIEF NAMED A TYPE THAT DOES NOT EXIST, AND THE TREE WON.** It offered
    `EStratScoreboardOwner { None, You, Enemy }` in `Source/StratUI/StratScoreboardWidget.h` as
    an existing enum to consider reusing. `grep -rn "EStratScoreboardOwner\|ScoreboardOwner"
    Source/` returns NOTHING; the control is `grep -rn "enum class EStrat" Source/`, which
    returns eleven enums, none of them that one. The real neighbour is `EStratScoreColumn` in
    that file. This is the fabricated-name shape this project has recorded before -- a plausible
    name fused from real neighbours, which reads as ordinary and is ungreppable. Every other
    figure in the brief checked out: `FStratInfoPanelView` exists with 22 `UPROPERTY` fields,
    `FStratViewModel::InfoPanel` is a `UPROPERTY`, `AStratPlayerController` does reach
    `StratDecorateInfoPanel`, and the two-grep control for the routing absence reproduces.
  - **THE DESIGN CALL: A NEW FOUR-VALUED `EStratHexOwnership`, NOT A REUSE OF
    `EStratScoreColumn`.** §2.11.2's clause is `yours` / `neutral` / `enemy` and
    `FStratInfoPanelView::HexOwner` is a SIDE, which that struct's "NOT IN THIS ROUND" block
    refuses to collapse into a boolean because "the comparison rests on the viewer". So the
    resolution moved into C++ on `FStratMatchResultModel::bViewerWon`'s precedent -- "computed
    once here so no graph computes it" -- and `FStratInfoPanelModel` carries the view WHOLE,
    `ViewingSide` beside it, and `HexOwnership` already resolved. What killed reuse of
    `EStratScoreColumn`: its `None` means "neither column", the chevron's ABSENCE, whereas
    §2.11.2's `neutral` is a real held state a word is printed for; and this panel needs a
    FOURTH state, `NotCapturable`, because a neutral Factory and a Plains hex both carry
    `HexOwner == INDEX_NONE` and only `bHexCapturable` tells them apart. A three-valued enum
    would force the WBP to COMBINE the enumerator with `bHexCapturable`, which is the
    unobservable-by-combination shape no clause covers.
  - **THE WORDS DID NOT MOVE.** `yours`, `neutral`, `enemy`, the `·` separators, the `%`
    glyph, `12/20`'s slash, `ready`/`done`, and the flag's sentence are spelled nowhere in
    either new file. Only the RESOLUTION crossed into C++.
  - **THE PUSH SITE IS `UStratMatchSubsystem::ApplyView`, ON THE STRIP'S OWN LINE.** The same
    `FindScoreboardHUD()` block that pushes `Model.Guidance` now also calls
    `HUD->PushInfoPanel(Model.InfoPanel, Model.ViewingSide)` -- unconditional, no branch on
    `bHasHex`, both operands fields of the one value being applied. `AStratScoreboardHUD` was
    chosen over the subsystem for the module arrow that block already records: `StratPlay`
    would have to name `UMG`, `Slate` and `SlateCore` to call `CreateWidget`, and `StratUI`
    already declares all three privately.
  - **THE FIRST-DELIVERY CACHE IS CARRIED, AND FOR THE MEASURED CAUSE RATHER THAN BY
    SYMMETRY.** `AStratPlayerController::BeginPlay` reaches `ApplyView` before
    `AStratScoreboardHUD::BeginPlay` creates a widget -- measured for the strip in five PIE
    sessions on 2026-08-21 -- and that ordering is a property of the two `BeginPlay`s, not of
    which widget is downstream. So `CreateInfoPanelWidget` ends in `DeliverLatestInfoPanel`.
    `bInfoPanelEverPushed` is a separate bool because a default-constructed
    `FStratInfoPanelView` is §2.11.2's ORDINARY state -- the unhovered panel -- and so "the
    cache equals the default" is the commonest TRUE reading of a live panel, which is a sharper
    version of the `FStratMatchConfig::SaveSlotName` trap.
  - **THE TWO INPUTS ARE CACHED, NOT THE COMPOSED MODEL**, so `StratComposeInfoPanelModel` has
    exactly one author and a replay cannot assert a resolution made against a viewing side that
    has since moved. And `PushInfoPanel` reads its `InViewingSide` ARGUMENT, never
    `AStratScoreboardHUD::ViewingSide` -- those are two different values, a Blueprint camera
    default and the seat the model was rendered for, and conflating them would misresolve the
    yours/enemy clause the moment a hot-seat hand-over moved one and not the other.
  - **NO `strat::` CALL, NO `StratBridge.h` IN A REFLECTED HEADER, NO `/Game/` LITERAL.**
    `StratInfoPanelWidget.h` declares a `UCLASS`, a `USTRUCT` and a `UENUM` and includes only
    `StratViewModel.h`, this module's own reflected header. `StratComposeInfoPanelModel` takes
    values and reaches no bridge, no snapshot and no table -- one step further than
    `StratBuildMatchResultModel`, which does take a bridge -- so it is total and infallible and
    returns no `bool`.
  - **THE DEBT, WRITTEN INTO `StratScoreboardHUD.h` WITH ITS DISCHARGE CONDITION.** Nine more
    members on a class whose name covers none of them, making it the FIFTH surface it hosts.
    The condition that discharges it is the one already stated there for the guidance seven and
    the result seven: when a §2.11 UI-layer owner exists, all nine move unchanged, and
    `PushInfoPanel` was given a signature that survives the move -- a reflected struct by const
    reference plus an `int32`, touching no member outside its own nine and never `GetBridge()`.
  - **WHAT IS NOT BUILT AND IS NOT MINE.** No `WBP_InfoPanel` asset exists, so nothing is on
    screen yet; the class is a parent and the reparent bakes
    `/Script/StratUI.StratInfoPanelWidget` irreversibly, which is `strat-editor-builder`'s
    call to make. `InfoPanelWidgetClass` is unset on every Blueprint default, which is a
    legitimate configuration the HUD reports at `Log` level and does not refuse over. And no
    clause pins any of this -- see the handoff to `strat-test-author` in the same report.

- **2026-08-27, `strat-gameplay-engineer` -- WAVE 2: §2.11.2's INFO PANEL, MODEL-SIDE. THE
  TERRAIN AND §2.4 ROWS THE PANEL READS WERE HELD BY THE BRIDGE AND PROJECTED NOWHERE; `Hp` /
  `HpMax` WERE PROJECTED AND READ BY NOTHING OUTSIDE `Tests/`.** In the lane worktree
  `E:/MultiAgent/Strat-wt/slot-1`, branch `feat/info-panel`, from `a45a7d9`. Three files
  modified and none added, all mine: `Source/StratUI/StratViewModel.h`,
  `Source/StratUI/StratViewModel.cpp`, `Source/StratPlay/StratPlayerController.cpp`. Nothing
  committed, nothing staged, no asset, no `Content/`, no `Config/`, no test, nothing under any
  `Tests/` directory, no `.Build.cs`, no `.uproject`, no widget. No suite figure is stated here;
  `global.md` owns that and owns any verdict.
  - **THE BUILD, IN THIS TREE.** `Build.bat StratocracyEditor Win64 Development` against
    `E:\MultiAgent\Strat-wt\slot-1\Stratocracy.uproject` with `-waitmutex -NoHotReloadFromIDE
    -MaxParallelActions=10`, redirected to a file and never piped. `REAL_EXIT=0` and the tool's
    own line is `Result: Succeeded`, over 59 actions with `StratViewModel.cpp` and
    `StratPlayerController.cpp` both named among the compiles -- so the green is over the
    edited translation units and not over an up-to-date tree. Zero warnings. The branch was
    proved by `git -C … rev-parse --abbrev-ref HEAD` -> `feat/info-panel` rather than by the
    toplevel echo, which this project has measured lying 6/6.
  - **THE SYMBOLS ADDED**, cited by name because a `file:NNN` written during a diff is
    invalidated by that same diff. On `FStratHexView`: `TerrainMoveCost`, `TerrainDefensePct`,
    `bTerrainCapturable`, `TerrainIncomeFame`. On `FStratUnitView`: `StatAtk`, `StatDef`,
    `StatMove`, `StatRangeMin`, `StatRangeMax`. New reflected struct `FStratInfoPanelView`, new
    field `FStratViewModel::InfoPanel`, new free function `StratDecorateInfoPanel`. The call
    site is the last line of `AStratPlayerController::DecorateForPresentation`.
  - **THE PLACEMENT CALL: THE TABLE ROWS LAND ON THE BOARD STRUCTS, NOT ON THE PANEL.** The
    terrain facts sit on `FStratHexView` beside `TerrainId`, and the §2.4 stats on
    `FStratUnitView` beside `DefId` -- in each case the SAME row at the SAME index in the SAME
    loop, bound to one reference and read once. The alternative considered was a self-contained
    panel struct filled from its own lookup; what killed it is that a second read of the same
    row is a second chance to read a DIFFERENT row, which is the defect
    `FStratAttackForecast`'s terrain block already refuses from the other side ("so the bonus
    shown IS the bonus `resolveDamage` was handed"). It also puts the terrain cost on every hex
    rather than only the hovered one, which is what §2.5's path-preview tick will want.
  - **AND THE PANEL ITSELF REACHES NOTHING.** `StratDecorateInfoPanel` takes
    `FStratViewModel&` and no bridge, no snapshot, no table and no query interface, so every
    number it can possibly write is already a field of the model. That is why it needs no
    clause in the header's no-arithmetic census: it cannot introduce a number the screen could
    not otherwise have shown. It is spelled as a decorator rather than a compose because it
    writes one field of a model it also reads, and a compose with an out-parameter would invite
    the caller to alias the model against its own field.
  - **NO NEW `FStratBridge` METHOD, AND NO `strat::` FUNCTION CALLED FROM ANY NEW LINE.** The
    two new table reads happen inside `StratBuildViewModel`, which already held
    `Bridge.Tables()` for the `TerrainId` / `DefId` lookups; the new fields are more fields off
    the rows it had already bound and range-checked. `strat::TerrainDef` and `strat::UnitDef`
    are NAMED in `StratViewModel.cpp`, which is legal -- naming a vendored type is free and
    calling a vendored free function from `StratUI` is the `LNK2019` the constraint is about.
  - **THE `ready` / `done` TRAP WAS AVOIDED BY READING THE PRESENTATION BLOCK.**
    `FStratInfoPanelView::bUnitDone` is `FStratUnitView::bDone` and nothing else. It is NOT
    `bHasActed`, and NOT any pair of the two turn flags: `Ui.h` states the DONE bit "is
    DERIVABLE FROM NEITHER turn flag nor from any pair of them", and §2.11.2 gives the case --
    a waited unit reads `done` while its act flag is unspent. Both flags remain on
    `FStratUnitView` and neither is consulted here.
  - **`bTerrainCapturable` IS A SEPARATE FIELD BECAUSE `Owner` CANNOT ANSWER THAT QUESTION.**
    `Ui.h` scopes `UiHexView::owner` to "capturable hexes only; OWNER_NEUTRAL elsewhere", so a
    NEUTRAL Factory and a Plains hex both project `INDEX_NONE`. Inferring capturability from
    `Owner` would print `neutral` beside every Plains hex on the board -- a defect that would
    read as a formatting choice. The flag is read off `TerrainDef::capturable` instead.
  - **TWO COMPARISONS, ONE ADMITTED INTO THE MODEL AND ONE REFUSED, on a distinction stated in
    the header rather than left implicit.** `bTerrainImpassable` is `TerrainMoveCost == 0` and
    IS in the model, because `Data.h` declares 0 to MEAN impassable (the §4.8 sentinel) and a
    widget performing that test would be a widget holding a rules constant. The
    yours/neutral/enemy reading is `HexOwner` against `FStratViewModel::ViewingSide` and is NOT
    in the model, because that compares two fields of this same model and means only what it
    says -- and baking it would carry the premise that the viewer is fixed through a hot-seat
    hand-over, which `FStratForecastView::RiskedFlagSide` already refuses for its own colour.
    Neither is arithmetic; the file's single declared arithmetic exception
    (`FStratBuildOptionView::Shortfall`) did not move, and the .cpp's census block says so in
    its own words.
  - **THE ORDERING CONSTRAINT IS REAL IN BOTH ARMS, unlike the forecast's.**
    `StratDecorateInfoPanel` must follow `FStratHoverState::DecorateViewModel` (it reads
    `Model.Hover` for which hex) AND `FStratSelectionMachine::DecorateViewModel` (it reads
    `bDone`). `StratDecorateForecast`'s selection arm is incidental because it takes the
    selection through `GetSelectedUnitId` rather than off the model; this one has no such
    escape. It is placed last in `DecorateForPresentation`, after the forecast, which the two
    do not need of each other -- last is where a decorator that only restates the model belongs.
  - **NO FAILURE CHANNEL, AND THE ABSENCE IS DESIGNED.** There is no input the decorator can be
    handed that has no answer: not hovering, hovering a hex absent from `Model.Hexes`, and
    hovering an empty hex are all `bHasHex` / `bHasUnit` false. It writes `Model.InfoPanel`
    unconditionally, so a cursor leaving the board cannot leave the previous hover's panel on
    screen, and it fills a local and assigns on the last line -- which here also removes the
    read-while-writing question rather than answering it.
  - **THE SCOPE WAS CORRECTED MID-TASK AND THE FIRST SHAPE IS RECORDED, NOT ERASED.** The
    dispatch brief described §2.11.2 as three terrain readouts and an HP line; the GDD's own
    subsection names four hex readouts (including the capturable status and its income rate)
    and a unit line carrying Atk/Def/Move/Range and the DONE bit. The narrow shape built green
    first, WITH ITS DEFERRALS DECLARED ON `FStratInfoPanelView`'s block together with the
    condition that would discharge each; the corrected spec turned out to be exactly those
    conditions, and both were taken by the route the deferral had named. That is why the second
    pass was an extension of the same branch and not a rewrite, and the block still carries the
    history.
  - **NOT DONE, DELIBERATELY, AND EACH WITH WHAT DISCHARGES IT.** (1) No widget and no
    `Content/` -- the panel has a complete model side and nothing draws it; discharged by the
    integration tree's asset tail. (2) No `UStratViewModelLibrary` accessor -- `InfoPanel` is a
    `BlueprintReadOnly` field on the model exactly as `Forecast` is, and a UMG binding reaches
    it without one; discharged only if a widget needs a query the field cannot answer. (3) No
    text, no separators and no `12/20` slash anywhere in the model; those are the widget's and
    formatting them here would put a player-facing string in a struct a clause compares field
    for field. (4) NO FILING WAS OPENED AGAINST `Source/StratRules/` and none was needed: every
    number this wave shows already existed module-side, in `TerrainDef`, in `UnitDef`, in the
    snapshot, or in the presentation block whose owner is the selection machine.
  - **THE RANGE IS CARRIED AS A BAND, AND ONE SENTENCE OF THE DISPATCH WAS WRONG ABOUT WHICH
    FIELD REACHES THE ROW.** `StatRangeMin` and `StatRangeMax` are both carried and nothing
    collapses them; `Data/units.csv` (read, not edited) has Artillery at `2..3` and the other
    three at `1..1`, so a scalar `Range` would have been right on three rows out of four and
    wrong on the only unit whose range is interesting. The dispatch also said
    `FStratUnitView::UnitId` is the UE-side counterpart of `strat::UiUnitView::unitId`, the §2.4
    row index. THE TREE SAYS OTHERWISE and the tree wins: `UnitId` is assigned from
    `UiUnitView::id`, the INSTANCE id, and the row index is `FStratUnitView::DefIndex`, assigned
    from `UiUnitView::unitId` -- a naming collision the existing field comment already flags
    ("whatever its spelling suggests"). The stat read goes through `DefIndex`. Indexing the
    definition table with `UnitId` would have compiled, would have been in range on this
    scenario's ten units, and would have shown the wrong unit's stats.
  - **UNPINNED SURFACE, NAMED SO THE HOLE IS VISIBLE RATHER THAN INFERRED.** Per the
    coordinator's ruling: `Owner` and `bIsFlag` ride `T-UI-05` alongside `Hp` / `HpMax` as
    unmarked snapshot mirrors **[AMENDED 2026-08-29 BY USER RULING, AND ON THE ID NAME ALONE:
    those four ride `T-INT-05`. `T-UI-05` STOPS AT THE SNAPSHOT -- both of its compared sides are
    module-side -- and the engine side of the bridge is `T-INT-05`'s, whose subject is "every
    member of the view-model". Everything else this bullet says is untouched: the four are still
    unmarked snapshot mirrors, they are still already pinned, and the hole this bullet exists to
    make visible is still exactly the DONE bit and the four §2.4 stat reads. The ruling is at the
    top of `global.md`'s `## NEXT`; this file was corrected by the `coordinator` on the user's
    direct instruction rather than by this lane.]**; the DONE bit rides NOTHING, because `Ui.h`
    says in terms that
    the presentation block "is NOT in T-UI-05's subject"; and the four §2.4 stat reads ride
    nothing either, being `UnitDef` table rows. The mint request is filed upstream and is the
    steward's. I wrote no test and name none as written.

- **2026-08-27, `strat-gameplay-engineer` -- WAVE 3: §2.11.2's THREE HUD NUMBERS AND ITS END TURN
  VERB, MODEL-SIDE. THE FAME POOL AND THE INCOME LINE WERE ALREADY IN THE MODEL AND HAD NO WAY
  TO REACH A WIDGET; THE IDLE COUNT DID NOT EXIST IN ANY FORM AND STRUCTURALLY CANNOT.** In the
  lane worktree `E:/MultiAgent/Strat-wt/slot-2`, branch `feat/persistent-hud`, from `ee7300c`.
  Two files added and six modified, all mine: `Source/StratUI/StratViewModelLibrary.h` and
  `Source/StratUI/StratViewModelLibrary.cpp` (new), `Source/StratUI/StratViewModel.h` and
  `.cpp`, `Source/StratPlay/StratMatchSubsystem.h` and `.cpp`,
  `Source/StratPlay/StratPlayerController.h` and `.cpp`. Nothing committed, nothing staged, no
  asset, no `Content/`, no `Config/`, no test, nothing under any `Tests/` directory, no
  `.Build.cs`, no `.uproject`. No suite figure is stated here; `global.md` owns that and owns
  any verdict.
  - **THE READER COUNTS, MEASURED IN THIS TREE AT `ee7300c` BEFORE ANYTHING WAS DESIGNED, and
    they are the basis for calling this a gap rather than a convenience.**
    `FStratSideView::FameTotal` had exactly ONE reader outside its own projection and the
    parity clauses -- the `Shortfall` subtraction inside `StratBuildProductionMenu`.
    `FStratSideView::IncomePerTurn` had NONE AT ALL: written by `StratBuildViewModel`, asserted
    by `StratViewModelParity` and `StratScoreboardHUDSeam`, and read by nothing that draws.
  - **THE IDLE COUNT HAS NO MODULE ANSWER TO BRIDGE, AND THAT IS UPSTREAM'S OWN RULING.**
    `strat::UiUnitView` carries `hasMoved` and `hasActed` and then states that neither is
    §2.11.1's DONE bit, which "is the selection machine's own ... deliberately absent here
    because where per-unit presentation state lives is unruled". `strat::UiSnapshot` therefore
    carries no idle count and could not: the count is over a bit the rules module refuses to
    hold. **NO FILING WAS OPENED AGAINST `Source/StratRules/`**, deliberately -- a rules-side
    idle count would be the module asserting presentation state it has ruled it does not own.
    What would justify one is a §2.11.2 requirement that the count mean "has a legal command
    available"; that is a rules answer, and no engine module may derive it.
  - **THE DERIVATION IS WRITTEN OUT IN THE HEADER SO IT CAN BE PINNED RATHER THAN INFERRED.**
    A `FStratUnitView` counts exactly when all four hold: its `Side` equals the side asked
    about; `!bDone`; `!bLockedThisTurn`; and `!(bHasMoved && bHasActed)`. The fourth is a
    conjunction under a negation because `hasMoved` and `hasActed` are two independent flags by
    T-TURN-01 -- a unit that has moved but not attacked still has an attack. `bIsFlag` is not
    consulted.
  - **NO NEW `FStratBridge` METHOD WAS ADDED AND NO `strat::` FUNCTION IS CALLED FROM ANY NEW
    LINE.** Every number here was already reflected: the two side numbers are `strat::UiSideView`
    fields the projection already mirrored, the purse is the local `StratBuildProductionMenu`
    already read to compute `Shortfall`, and `FStratBridge::SubmitEndTurn` already existed and
    is reached through the unchanged `HandleSelectionEvent` path. `StratViewModelLibrary.cpp`
    includes no `StratBridge.h` and names no `strat::` type at all.
  - **A SELECTOR AND NOT A FIELD, AND THAT IS THE LOAD-BEARING CALL.** A
    `UnitsAbleToActCount` on `FStratViewModel` would have been the first arithmetic INSIDE the
    model -- the second exception to `StratViewModel.h`'s own absolute, and the first one inside
    the walk `StratViewModelParity` performs, where its only possible parity assertion would be
    against a snapshot field that does not exist. So nothing was added to `FStratViewModel`; the
    numbers are functions OVER it, on `UStratViewModelLibrary`, taking `const FStratViewModel&`
    and reading nothing else. T-INT-05 holds because the answers are a function of the model
    alone; the widget performs no arithmetic because it calls a named C++ function.
  - **THE PURSE RIDES THE ROWS' CLOCK, WHICH IS WHY IT IS A NEW OUTPUT RATHER THAN A SECOND
    READ.** `StratBuildProductionMenu` gained a six-argument overload returning the very
    `FameTotal` local every `Shortfall` was computed against; the five-argument form is now a
    one-line forward and its contract and its callers are unchanged.
    `UStratMatchSubsystem::ProductionMenuFameTotal` is where it lands, in the same statement
    group as `ProductionMenu`, `ProductionMenuHex` and `bProductionMenuSpawnBlocked`, and
    cleared with them. Read off `FStratViewModel::Sides[...].FameTotal` instead, a header could
    print `Fame: 250` above a 275-cost row reading `need 50` -- each correct, at two instants.
  - **THE END TURN VERB CLOSES A ROUTE, NOT A MECHANISM.** `FStratBridge::SubmitEndTurn`,
    `EStratSelectionEvent::EndTurn` and the whole gate chain were already correct at `ee7300c`;
    the only caller was `AStratPlayerController::OnEndTurn`, a private Enhanced Input handler.
    So a turn could be ended by a key and by nothing else, and §2.11.2 specifies a button.
    `AStratPlayerController::RequestEndTurn` is a `BlueprintCallable` one-line call to the same
    `HandleSelectionEvent`, and `OnEndTurn` now goes through it, so the key and the button spell
    the event once between them. It takes no enum, so it does not reopen that header's recorded
    objection to a reflected `HandleSelectionEvent`; that paragraph was amended in place rather
    than replaced, and the amendment says which half moved.
  - **ID COVERAGE, AND ONLY ONE OF THE FOUR IS COVERED.** The GDD's UI acceptance set is
    `T-UI-01` through `T-UI-05` and nothing else -- verified by scanning every `T-UI-NN`
    occurrence in `Stratocracy_Prototype_GDD.md`, which returns exactly those five. **The purse
    header IS `T-UI-04`**, on its own wording: "the production menu binds to the buildlist
    derived from the four Stub-2 unit rows **plus current fameTotal**", and
    `ProductionMenuFameTotal` is literally that, bound to that menu. **The other three are
    uncovered.** `T-UI-03` enumerates the standings rows it governs -- destroyed, objectives
    held X/N, surviving units/HP, turn vs cap -- and neither the Fame pool nor the income line
    is among them, so §2.11.2's HUD line is not its subject. The End Turn verb has no ID. The
    idle count has no ID and structurally cannot ride `T-UI-05`, whose subject is snapshot
    fidelity, because the DONE bit is not in the snapshot. **NO ID WAS INVENTED AND NO CLAUSE
    WAS NAMED**; the ruling is the user's and was outstanding at the time of writing.
  - **BUILD AND SUITE.** `Build.bat StratocracyEditor Win64 Development` in this worktree:
    `Result: Succeeded`, process exit 0, quoted from the tool's own line rather than inferred
    from an exit code. `StratViewModelLibrary.cpp` appears in the log as `[56/114] Compile
    [x64]`, so the new translation unit was genuinely compiled and the green is not a no-op.
    The suite was run in this worktree and its report is at `Saved/AutomationReport`; the
    figures belong to `global.md` and are not restated here.
  - **ONE CORRECTION TO THE DISPATCH BRIEF, RECORDED BECAUSE IT WILL BE REPEATED.** The brief
    stated this worktree is brand-new with no `Intermediate/` and that the first build would
    take 20-40 minutes. Both `Intermediate/` and `Binaries/` were present on arrival and the
    build took 73 seconds of executor time over 114 actions. Nothing was worked around; the
    premise was simply not true of the tree.
- **2026-08-27, `strat-gameplay-engineer` -- W1, §2.11.3'S FORECAST CARD, MODEL SIDE. THE THREE
  READOUTS `strat::UiForecast` DOES NOT CARRY NOW HAVE MODULE SOURCES, AND THE ONE THAT COULD
  NOT BE ASKED FOR IS WRITTEN OUT IN THE BRIDGE WITH ITS OWN GATE NAMED.** In the lane worktree
  `E:/MultiAgent/Strat-wt/slot-1` on `feat/forecast-card`, from `ee7300c`. Five files modified
  and two added, all mine: `Source/StratBridge/StratBridge.h`, `Source/StratBridge/StratBridge.cpp`,
  `Source/StratUI/StratViewModel.h`, `Source/StratUI/StratViewModel.cpp`, and the new pair
  `Source/StratPlay/StratForecastQuery.h` / `.cpp`; plus the decorator wiring in
  `Source/StratPlay/StratPlayerController.cpp`. Nothing committed, nothing staged, no asset, no
  config, nothing under `Tests/`. No suite figure and no verdict is stated here; `global.md`
  owns both.
  - **THE BRIEF'S "BRIDGE `uiResolveForGate`" INSTRUCTION WAS REFUSED, AND THAT IS THE ONE
    LOAD-BEARING CALL OF THIS WAVE.** The dispatch was right that the HP before-to-after pair has
    a module-side source and that `Hp - damage` in a compose function is the defect `Ui.h` says
    T-UI-01 exists to catch. It was wrong that the fix is to bridge it.
    `Source/StratBridge/Tests/StratCombatOutcomeParity.cpp`'s header block states that
    `strat::uiResolveForGate` "has zero production callers in this tree BY CONSTRUCTION", that
    the phase which wrote the combat emitter deliberately declined to call it, and that "a
    `uiResolveForGate` invoked by the bridge would make every comparison below a comparison of
    the bridge with itself". `StratBridge.cpp`'s `StratCombatObservation` block says the same
    from the other side. Bridging it would have destroyed T-UI-01's independent oracle at the
    resolution end in order to serve a hover card.
  - **SO THE SUBTRACTION AND THE CLAMP-AT-ZERO ARE WRITTEN OUT ONCE, INSIDE
    `FStratBridge::AttackForecast`, AND THE DISCHARGE IS A TEST RATHER THAN A COMMENT.** A gate
    in `Source/StratBridge/Tests/` may call the oracle; the bridge may not. The bridge computing
    independently and the oracle computing independently is the arrangement that construction
    wanted -- what it forbade was the bridge ASKING the oracle. Until such a clause exists, the
    two HP-after fields are the only numbers on the card that no test compares against anything.
  - **WHAT IS NEW ON THE BRIDGE.** One plain engine-typed struct `FStratAttackForecast` and one
    method `FStratBridge::AttackForecast(int32, FIntPoint, FStratAttackForecast&)`. It calls the
    existing `Forecast` for every combat number and adds exactly four things: the defender's
    `TerrainDef::defensePct` and `id` by table read, `strat::killAward` for the lethal line's
    fame, both units' HP off one `MakeUiWorld`, and the two clamps above.
    **`strat::killAward` had no bridge caller before this** -- it appeared in `Source/StratBridge/`
    only inside `StratCombatOutcomeParity.cpp`. The §2.4 cost is never halved here and the
    literals 150 and 500 appear nowhere.
  - **`bAttackerDies` IS THE BRIDGE'S WHERE `bDefenderDies` IS THE MODULE'S**, because
    `strat::UiForecast` carries no attacker-death field. The expression used is the one
    `StratDivergenceMaskOf`'s `bExpectAttackerDie` already uses in the same file, so there is one
    spelling of the counter-kill in that module and not two.
  - **THE COUNTER LINE'S THREE-WAY REASON IS A PRESENTATION MAPPING AND IS THE ONE PLACE A LATER
    RULES CHANGE COULD MAKE THE CARD LIE WITHOUT MOVING A NUMBER.** `EStratCounterReason` is
    chosen in `StratComposeForecastView` from two module booleans, dying-defender arm first. Two
    upstream changes break it silently and no compiler and no numeric parity gate would report
    either: if `strat::defenderCanCounter` ever refuses for a reason that is not range, the card
    says `out of range` about a defender in range; if `strat::uiForecast` ever lets a dying
    defender counter, the first arm swallows a counter that fires. `Distance`, `bCounterFires`
    and `bDefenderDies` are carried on the view beside the enumerator so a clause can assert the
    choice against something other than the booleans it was chosen from.
  - **`bLegal` IS "LIT", SO NOTHING INTERSECTS AGAINST THE TARGET SET.**
    `FStratBridge::AttackTargetHexes`' own block says it keeps a hex "when the module answers
    `legal`", from the same `Forecast` call -- so the card's appearance and the highlight are the
    same predicate evaluated by the same function, and the wave added no second range test, no
    distance filter and no membership check.
  - **THE FIRST DECORATOR ON THE PRESENTATION SEAM WITH AN ORDERING CONSTRAINT THAT IS NOT
    `Observe`'S.** `StratDecorateForecast` reads `FStratViewModel::Hover`, so it runs after
    `FStratHoverState::DecorateViewModel` in `AStratPlayerController::DecorateForPresentation`.
    The hover decorator's own header amendment predicted this day and is still accurate -- the
    hover has no constraint of its own; something now depends on it. It writes
    `FStratViewModel::Forecast` unconditionally, including on all four no-card paths, so a model
    is never a difference from a previous one.
  - **ONE GATE IN THE DECORATOR IS NOT THE RULES MODULE'S, AND IT IS NAMED WHERE IT IS WRITTEN.**
    `strat::uiForecast` answers about §2.6 and not about the turn, so it calls an already-spent
    unit's attack legal. The decorator therefore reads `FStratUnitView::bHasActed` -- the same
    RULES-side bit `FStratSelectionMachine`'s own `has already acted this turn` guard reads, and
    deliberately not the engine's `bDone` -- so that no commit card appears for a command the
    selection machine will refuse.
  - **NOT IN THIS WAVE, and each is somebody else's:** the Widget Blueprint and every asset
    (`Content/` is not this lane's); all card TEXT, since the view carries an enumerator, a signed
    percentage and two ids and no formatted string; §2.11.3's range-1 dead-zone ring, which is
    `BuildOverlays`' overlay and is about the selection rather than one hovered target; and every
    clause, which is `strat-test-author`'s.

- **2026-08-27, `strat-gameplay-engineer` -- THE HOVER IS POLLED ON TICK. THE ENHANCED INPUT
  HOVER ROUTE WAS MEASURED DEAD AND HAS BEEN REMOVED, AND THE INSTRUMENT THAT MEASURED IT IS
  GONE WITH IT.** In the integration tree `E:/MultiAgent/Stratocracy` on `master`, from
  `5e0333f`, on top of the instrumentation entry below -- which this entry DISCHARGES. Two files
  modified, both mine: `Source/StratPlay/StratPlayerController.h` and
  `Source/StratPlay/StratPlayerController.cpp`. Nothing committed, nothing staged, no asset, no
  config, no test, nothing under `Tests/`. No suite figure is stated here; `global.md` owns that
  and owns any verdict.
  - **THE MEASUREMENT, WHICH IS THE ENTIRE BASIS FOR THE CHANGE AND IS PRESERVED HERE BECAUSE
    THE CODE THAT PRODUCED IT NO LONGER EXISTS.** A human playtest on the instrumented binary,
    on a seeded 99-hex board, over three and a half minutes. The complete set of `HOVER-DIAG`
    lines: the resolve path was entered four times, three of them forced from the console; the
    handler `OnHover` ran **exactly once**, at 17:48:59, seven seconds after the match seeded,
    on a focus transition, with the cursor off the board. Between two forced console reads at
    17:50:43 and 17:52:14 the human swept the cursor across the board for fifteen seconds and
    **the handler count did not move**. An independent earlier observation agrees: a planted
    hover cleared at the moment the human moved the cursor out of the window to type -- another
    focus transition, off-board, one event, one clear.
  - **THE CONTROL IS WHAT MAKES THAT AN ABSENCE RATHER THAN A SILENCE.** The forced call
    `ke StratPlayerController UpdateHoverFromCursor` printed, which proved in the same line
    that the log channel spoke AND that the editor had loaded the rebuilt DLL, while reading the
    handler's count out of the running game with no planted state. The two-counter shape was
    designed for exactly this and it did exactly this.
  - **WHAT THE MEASUREMENT PROVES, AND -- STATED FIRST, BECAUSE IT IS THE PART A LATER READER
    WILL OVER-CLAIM -- WHAT IT DOES NOT.** It proves that mouse movement produces no `Triggered`
    event for a mouse-axis action on this controller as configured. **IT DOES NOT PROVE WHY.**
    The standing hypothesis is that a visible, uncaptured cursor feeds no axis to `UPlayerInput`
    -- this class sets `bShowMouseCursor = true` and calls `SetInputMode` nowhere in
    `Source/StratPlay/` -- but that mechanism was never isolated and **this record does not
    assert it**. If the tick route works when a human next drives it, that is evidence the ROUTE
    was wrong; it is not proof of the engine mechanism behind it, and nothing downstream should
    cite it as one.
  - **THE ROUTE CHOSEN: `AStratPlayerController::Tick` calls `UpdateHoverFromCursor` and
    discards the result.** Four lines of body. Every hover entry point still ends at the single
    `ApplyHoverChange` refresh decision, unchanged.
  - **WHAT WAS REJECTED, AND ON WHAT.** `SetInputMode` with capture, which is the more obvious
    repair. Rejected on two counts. (1) `bShowMouseCursor = true` is load-bearing -- a
    mouse-driven hex strategy whose player cannot see the cursor is not the game -- and every
    mode that reliably feeds mouse axis does so by capturing, which changes cursor behaviour at
    the viewport edge and how a click lands. A route that fixes the hover and makes selection
    feel different is worse than the bug. (2) It is unfalsifiable at our cost: no clause in this
    project reaches `UPlayerInput`, so an input-mode fix could only ever be re-confirmed by
    another human playtest, whereas a tick call is reachable by a clause. **This is not a ruling
    that input modes are wrong** -- if a later pass isolates the capture mechanism and finds a
    mode that feeds axis with a free visible cursor, it is a live alternative again, and it would
    still have to beat the testability.
  - **THE THREE CLAIMS THE TASK TOLD ME TO RE-DERIVE RATHER THAN ACCEPT, ALL RE-DERIVED AT
    SOURCE AND ALL HOLDING.** (a) The de-duplication is real: `FStratHoverState::SetHoveredHex`
    and `ClearHoveredHex` each compute and return `bChanged` (`StratHoverState.cpp`), so the
    per-frame cost is one trace and NOT a model rebuild. (b) `PrimaryActorTick.bCanEverTick =
    true` was already set in the constructor with the phase-6 measurement beside it -- the
    recorded `bCanEverTick = false` incident is closed in this tree and I did not re-open or
    re-derive it. (c) `HexUnderCursor` returns before tracing when there is no board, so a
    controller in an unseeded world pays a null check per frame and nothing else.
  - **WHAT WAS REMOVED, AND WHY REMOVAL RATHER THAN LEAVING IT UNBOUND.** The `HoverAction`
    `UPROPERTY`, the `OnHover` handler, the `ETriggerEvent::Triggered` binding in
    `SetupInputComponent`, its unset-property Warning, and the whole diagnostic -- both counters,
    both log sites, the powers-of-two throttle helper and both comment blocks. **NONE OF THE
    INSTRUMENT SURVIVES**; the debt the header declared is discharged in full, on the condition
    that header named. A wired-and-dead asset reference is worse than no reference: left in
    place, `HoverAction` would read to the next reader as the hover mechanism --
    `EditDefaultsOnly`, non-null on the shipping Blueprint, bound without a warning -- and send
    anyone debugging the hover to the trigger event instead of the tick.
  - **THE DIAGNOSIS IS RECONSTRUCTIBLE FROM THIS ENTRY ALONE, WHICH IS THE CONDITION THE TASK
    SET FOR DELETING ALL OF IT.** The verbatim timings, the counts, the control and the sweep
    are above. A summary of the measurement also sits on `Tick`'s definition in the `.cpp`, so a
    reader who reaches the code without the record still gets it.
  - **BUILD, GREEN, EDITOR CLOSED.** `Build.bat StratocracyEditor Win64 Development -waitmutex`
    -> `Result: Succeeded`, 15.38 s, 11 actions, zero warnings and zero errors. I confirmed the
    DLL lock was released with my own rename round-trip on
    `Binaries/Win64/UnrealEditor-StratPlay.dll` before building, rather than taking the
    editor-closed claim on trust.
  - **THE `Stratocracy.StratPlay` SUBSET RAN CLEAN AFTER THE CHANGE** -- zero failed, zero
    notRun, zero `succeededWithWarnings`. It is a regression result and NOTHING MORE. No count is
    written in this file; the live figure is `global.md`'s.
  - **[CORRECTED 2026-08-27, SAME DAY, ON A `strat-integration-reviewer` BLOCK. The sentence
    this bullet carried before it read: "That last figure is the one that matters here beyond
    regression: it is independent confirmation that the diagnostic `Warning` is gone from the
    tree." THAT WAS AN ABSENCE WITH NO CONTROL, and it is struck rather than deleted because the
    shape is worth keeping visible.** The conclusion is true -- the instrument is gone -- but the
    zero could not have witnessed it, and this same file refutes the reasoning twice over.
    (1) The design note further down states in terms that **no automation clause can reach
    `OnHover` at all**, so that log site could never have turned a clause
    `succeededWithWarnings` in ANY run, before or after. (2) The other log site,
    `UpdateHoverFromCursor`, was put at `Log` and not `Warning` **precisely so that the clauses
    which do call it would be unaffected** -- also stated in that note. Both halves of the
    instrument were designed to be invisible to this field. A field engineered not to move cannot
    testify that something moved. There was no pre-instrumentation-versus-post reading either:
    the instrument was never committed and no suite was run while it existed, so the nearest
    earlier report (`global.md`'s `05.17.45`) is not a "before" measurement of it but a run from
    a tree that never contained it.
    **WHAT ACTUALLY CONFIRMS THE REMOVAL, AND IT IS NOT THE SUITE.** Two things, both of them
    direct rather than inferential: a tree-wide grep --
    `grep -rn "HOVER-DIAG\|HoverHandlerCalls\|HoverResolveCalls\|OnHover(" Source/` returns
    **0 lines**, so neither counter, neither log site, the throttle helper nor the handler
    survives anywhere in `Source/`; and the build, which compiled the resulting tree with zero
    warnings. The subject grep was re-derived 2026-08-27 at `5e0333f` **with a control** --
    `grep -rn "UpdateHoverFromCursor\|SetHoveredHex" Source/` returns **36 lines** over the same
    tree, so the instrument is shown able to speak and the zero is a measured absence rather than
    a grep that could not match anything. The suite's role here is to show that removing it broke
    nothing else.
    **[A THIRD ITEM STOOD HERE AND WAS WITHDRAWN 2026-08-27, ON THE SECOND GATE OF THIS PASS.**
    It read: "the diff itself, `git diff --stat Source/StratPlay/` --> 160 insertions and 72
    deletions across the two files, which is where the removed lines are enumerable one by one."
    The figure was wrong when the gate re-derived it: the tree at `5e0333f` says **176
    insertions**, 72 deletions. It moved because of prose I wrote into this same header later in
    the same pass -- the identical defect to the stale header line numbers struck one bullet
    below, and a case of the standing rule against writing a count of a growing thing inside the
    thing that grows. **It is withdrawn rather than corrected, and the reason is not the rot.**
    Re-deriving 176 would fix the instance and leave the shape live: the next edit to either
    file, including one made to satisfy the next gate, falsifies the new number too. But the
    disqualifying fault is that **a diffstat was never evidence for this claim in the first
    place.** The claim is that a specific instrument is absent. A deletion count is a measure of
    total churn across the whole pass -- it cannot distinguish an instrument line from the
    surrounding refactor, and 72 deletions is equally consistent with the instrument surviving
    intact beside 72 unrelated removals. Absence is proved by absence. The grep above names the
    four tokens, returns zero, and carries a control; it is strictly stronger AND it does not
    rot, because no volume of later editing can make a token reappear that nothing writes. A
    figure that cannot survive its own document, and that does not bear on the proposition it
    was offered for, earns no place next to it.]
  - **WHAT IS NEWLY PINNABLE, FOR `strat-test-author`.** Before this change, "a mouse move
    produces a hover" was unreachable by any clause and the wave-0 hover file says so in terms.
    It is still unreachable -- nothing here reaches `UPlayerInput`. But the ROUTE is now
    reachable for the first time: `Tick` is virtual and callable on a spawned controller, and
    with no viewport `HexUnderCursor` returns false, so a tick CLEARS a hover planted through
    `SetHoveredHex`. That is a falsifiable pin on the tick-to-hover wiring --
    emptying the `Tick` body leaves the planted hover standing. Requested clause:
    `Stratocracy.StratPlay.T-UI-01.TickDrivesTheHoverPath`. **It should assert against the live
    spawned controller and hold no `FStratHoverState` of its own**, on the precedent the
    neighbouring hover clause already sets, and it should be proved by reverting what it pins.
  - **[CORRECTED 2026-08-27, SAME DAY, SAME BLOCK -- THE HANDOFF ABOVE SAID "`Tick` is public,
    virtual and callable on a spawned controller" AND `Tick` IS NOT PUBLIC HERE.** In
    `Source/StratPlay/StratPlayerController.h` the declaration
    `virtual void Tick(float DeltaSeconds) override;` sits between the file's `protected:` and
    `private:` labels -- cited by symbol and not by line on purpose, because the line numbers
    the gate quoted for it were already stale by the time this correction was written: the
    prose amendment in the same pass moved that declaration from 650 to 666 and the `private:`
    label from 691 to 707, in the very edit that answered the gate's third finding. So
    `Controller->Tick(dt)` through an `AStratPlayerController*` is an
    access error at compile time, and `strat-test-author` would have met the handoff as a build
    failure rather than as a clause. The word "public" is struck; "virtual and callable" is not,
    and the correction is written here rather than folded silently into the sentence above
    because the sentence above is what was handed over.
    **THE CLAUSE IS STILL WRITABLE, VIA THE BASE HANDLE, AND THAT IS THE FORK I TOOK.** C++
    access is checked on the STATIC type of the expression, while dispatch is virtual, so a
    `Controller` held as `APlayerController*` or `AActor*` -- where `Tick` is public in
    `AActor` -- calls this override legitimately and with no cast, no friendship and no change
    to this file. `SpawnActor<AStratPlayerController>` already returns something assignable to
    either. **THE OTHER FORK WAS TO MAKE `Tick` PUBLIC HERE, AND I REJECTED IT.** `BeginPlay`,
    `EndPlay`, `SetupInputComponent` and the rest of the framework overrides are all in that
    same `protected:` block, which is both this file's convention and Unreal's; widening one
    would be a change to shipping code made for a test's convenience, and it would leave the
    next reader unable to tell whether the access level meant anything. A pin should reach its
    subject through a handle the engine already hands out, not by relaxing the subject.]
  - **A HANDOFF INSIDE `Tests/`, WHICH I DID NOT EDIT.**
    `Source/StratPlay/Tests/StratPlayerControllerTick.cpp` pins the CDO's `bCanEverTick == true`
    and its prose gives the Enhanced Input trigger-evaluation reason for it. That clause now
    protects TWO mechanisms and names one: with this change, a `false` there would also stop the
    hover dead. The assertion is still correct and still green; only its rationale is now
    incomplete.

- **[HISTORY, 2026-08-27 -- SUPERSEDED BY THE ENTRY ABOVE, WHICH DISCHARGED THIS INSTRUMENT'S
  DECLARED DEBT AND DELETED EVERY LINE OF IT. The counters, log sites, token and throttle
  described below NO LONGER EXIST IN THE TREE. Kept because it records the design of the
  instrument that produced the measurement the fix rests on, and because the measurement itself
  is quoted in the entry above rather than here.]**
  **2026-08-27, `strat-gameplay-engineer` -- HOVER DIAGNOSTIC: `OnHover` IS NOW OBSERVABLE, AND
  THE INSTRUMENT CARRIES ITS OWN CONTROL. INSTRUMENTATION ONLY -- NO FIX, NO INPUT-ROUTE CHANGE.**
  In the integration tree `E:/MultiAgent/Stratocracy` on `master`, branched from `5e0333f`.
  Two files modified, both mine: `Source/StratPlay/StratPlayerController.h` (+44) and
  `Source/StratPlay/StratPlayerController.cpp` (+105), zero lines removed in either. Nothing
  committed, nothing staged, no asset, no config, no test. No suite was run and no suite figure
  is stated here; `global.md` owns that and any verdict.
  - **WHAT THE GAP WAS, AND IT IS AN INSTRUMENT GAP RATHER THAN A CODE ONE.** `content.md`'s
    playtest could not distinguish "`OnHover` never ran" from "`OnHover` ran and failed to
    resolve a hex". It reached the first only by planting a hover hex and watching it survive 28
    samples -- sound, but derived from a model field two frames downstream of the handler, and it
    needed a trick to get even that far. Nothing anywhere printed when the handler itself ran.
  - **THE SHAPE. TWO COUNTERS, TWO LOG SITES, ONE SCHEDULE.** `HoverHandlerCalls` counts entries
    to `OnHover`; `HoverResolveCalls` counts entries to `UpdateHoverFromCursor`. Both are plain
    `uint32` members, not `UPROPERTY`s, for `ProductionTargetHex`'s stated reason. **They are
    members and not function-local `static`s deliberately**: a `static` is process-wide and would
    carry a previous PIE session's count into the next one, making "never ran this session"
    indistinguishable from "ran last session". The controller is respawned per PIE session, so a
    member resets exactly when the question resets.
  - **THE SECOND SITE IS THE FIRST'S CONTROL, WHICH IS THE POINT OF HAVING TWO.** `OnHover` is
    private, unreflected and reachable only through Enhanced Input, so nobody can demonstrate its
    line is CAPABLE of printing -- and this record has been bitten before by an absence measured
    on an instrument never shown able to speak. `UpdateHoverFromCursor` IS console-reachable, and
    the playtest already drove it that way 30 times out of 30. So one
    `ke StratPlayerController UpdateHoverFromCursor` prints a line that both proves the channel
    works and reports `OnHover`'s count out of the running game. A forced call answering
    `OnHover has run 0 time(s)` after a minute of the cursor circling the board settles the
    hypothesis with no planted state at all.
  - **THE BRANCH IS VISIBLE AND NO SECOND CURSOR TRACE WAS ADDED.** `OnHover` reads the branch
    back out of `Hover` after the call -- latched hex means the cursor resolved, cleared means it
    took the off-board path. A struct read, not a re-trace: this file has ONE cursor-to-hex route
    and a diagnostic is not a reason to grow a second.
  - **THE VERBOSITY SPLIT IS OPPOSITE AT THE TWO SITES, AND EACH REASON FAILS TO APPLY AT THE
    OTHER.** `OnHover` logs at `Warning`, because `LogStratPlay`'s default runtime verbosity is
    `Log` and a `Verbose` line would print nothing until somebody typed `log LogStratPlay
    Verbose` -- which makes an absent line ambiguous between "the handler never ran" and "nobody
    raised the verbosity", the exact ambiguity this exists to remove. That is safe there because
    **no automation clause can reach `OnHover` at all**, so it cannot turn a clause
    `succeededWithWarnings`. `UpdateHoverFromCursor` logs at `Log`, because the wave-0 hover
    clauses DO call it directly and a `Warning` raised inside a clause is a record-visible change
    to the suite made by a diagnostic.
  - **THE TOKEN IS `HOVER-DIAG` AND DELIBERATELY NOT `STRAT-`.** Every `FOutputDevice` capture in
    `Source/StratPlay/Tests/` filters on `STRAT-CMD` or `STRAT-AI`, and several assert exact line
    counts over their window (`StratGuidanceInputGates.cpp`'s `Lines.Num(), 0` among them). A
    line carrying either prefix would enter those windows.
  - **AN OFF-BY-ONE THAT WOULD HAVE FAKED THE FAILING READING, CAUGHT BEFORE THE SECOND BUILD.**
    `UpdateHoverFromCursor` runs INSIDE `OnHover`, so its control line reports `OnHover`'s count
    from within the very call being counted. Incrementing after the call would have made a
    handler that had run exactly once report `OnHover has run 0 time(s)` -- the precise false
    zero this instrument exists to abolish, printed by the instrument itself. `OnHover` therefore
    increments FIRST and reports AFTER; the report has to be after, because the branch it names
    is read back from `Hover`.
  - **BOUNDED BY POWERS OF TWO, `(C & (C - 1)) == 0`, EVALUATED AFTER THE INCREMENT.** `OnHover`
    is bound `Triggered` on a 2D-axis action over mouse XY, so it fires at mouse-move rate. Call
    1 always prints, then 2, 4, 8, 16 ... -- about twenty lines for a whole session, 32 lines at
    `2^32` by construction. The alternatives were considered and rejected in the code comment: a
    first-call latch cannot separate "fired once" from "streaming"; a fixed every-Nth throttle is
    silent for the first N-1 calls and so reproduces the same false zero the playtest already
    hit; a time-based throttle makes the printed count depend on how fast the human moved.
  - **THE DEBT, WITH ITS DISCHARGE CONDITION, STATED IN THE HEADER AS WELL AS HERE.** This is
    instrumentation for one open question, not a feature. Both members, both log sites and both
    comment blocks come out once the hover route is proven working or replaced. If it is still
    here after that question closes it is dead weight in a hot path, and it should be deleted
    rather than downgraded.
  - **BUILD, GREEN, EDITOR CLOSED.** `Build.bat StratocracyEditor Win64 Development -waitmutex`
    -> `Result: Succeeded`, 15.31 s, 11 actions, zero warnings and zero errors printed;
    `[Adaptive Build] Excluded from StratPlay unity file: StratPlayerController.cpp` and both
    `UnrealEditor-StratPlay.lib` and `.dll` relinked.
  - **WHAT I DID NOT TOUCH, BECAUSE THE TASK SAID SO AND BECAUSE IT WOULD HAVE DESTROYED THE
    MEASUREMENT.** `bShowMouseCursor`, `SetupInputComponent`'s binding, the `IA_Hover` asset, the
    `IMC_Selection` row and the Blueprint default are all unchanged. Fixing the route in the same
    pass that instruments it would leave nobody able to say which change did it.
  - **MY READING OF THE HYPOTHESIS, OFFERED AS A READING AND NOT AS A FINDING.** The tree is
    consistent with it and contains nothing that contradicts it: `IA_Hover` is the project's only
    non-Boolean action and `Mouse2D` its only axis key, so the measured split -- seven key rows
    working, one axis row silent -- has exactly one structural difference behind it. **And the
    source of the `True` the playtest read off the live controller is IN THIS CLASS, which I had
    assumed it was not until I grepped:** `AStratPlayerController`'s constructor sets
    `bShowMouseCursor = true` with a comment saying a turn-based game is played with a cursor and
    that it is set in the constructor rather than `BeginPlay` so a Blueprint subclass can
    override it. No `SetInputMode` call exists anywhere in `Source/StratPlay/`. So if the
    hypothesis holds, the fix is a decision about input MODE rather than about the hover code:
    `FInputModeGameAndUI` with `SetHideCursorDuringCapture(false)`, or an explicit capture, or
    reading the cursor position on tick instead of via an axis action -- and the last of those
    would be a route change, which is why none of them is in this diff. The instrument above is
    what decides which question is even being answered.

- **2026-08-26, `strat-gameplay-engineer` -- WAVE 0: THE HOVER INPUT SURFACE. C++ ONLY, IN THE
  WORKTREE `E:/MultiAgent/Strat-wt/slot-1` ON `feat/hover-input`, BRANCHED FROM `69e75bb`.
  NOTHING IS COMMITTED, NOTHING IS STAGED, NO ASSET WAS TOUCHED AND NO TEST WAS WRITTEN.** No
  suite was run in this tree and no suite figure is stated here or moves on this work -- the live
  figure is `global.md`'s, which is the only file that may carry one. No phase verdict is stated;
  nothing here has been through a gate.
  - **WHAT THE GAP WAS.** The MVP-gap audit found §2.11.3's forecast card and §2.11.2's info
    panel both specified as hover-driven with NO HOVER INPUT ANYWHERE:
    at `69e75bb` `AStratPlayerController` bound no hover action of any kind -- dated here for
    the reason it is dated in `FStratHoverView`'s own block, an inconsistency the second W0 gate
    caught in this file after I had fixed it in the header. The hover STRINGS
    are shipped and are a different thing -- `FStratGuidanceView::EndTurnGateHover` and
    `LockedUnitHover` are tooltip TEXT for a widget that already knows what it is drawing, and
    neither answers "where is the cursor". Nothing in this wave reads or writes either.
  - **THE SHAPE.** A new plain struct `FStratHoverState` (`Source/StratPlay/StratHoverState.h`
    and its `.cpp`) holds the hex and the has-a-hex flag and produces `FStratHoverView`, a new
    `USTRUCT` on `FStratViewModel::Hover`. `AStratPlayerController` gains an
    `EditDefaultsOnly TObjectPtr<UInputAction> HoverAction`, an `OnHover` handler, three public
    entry points (`UpdateHoverFromCursor`, `SetHoveredHex`, `ClearHoveredHex`), a
    `BlueprintPure GetHoveredHex`, `GetHoverState`, and one private `ApplyHoverChange` that is
    the only place a hover decides to refresh. `DecorateForPresentation` calls
    `FStratHoverState::DecorateViewModel` beside the machine's and the guidance layer's.
  - **IT REUSES THE ONE CURSOR-TO-HEX ROUTE AND ADDS NO SECOND TRACE.**
    `UpdateHoverFromCursor` calls `AStratPlayerController::HexUnderCursor`, which is the same
    instance-index lookup `OnSelect` uses. No inverse of `WorldLocationOfHex` was written and
    none exists to call. **There is no hex arithmetic in any file this wave added or changed** --
    no distance, no neighbour, no addition of two coordinates; `FIntPoint::operator==` is the
    only spatial operator and it answers "did the hovered hex change", which is a question about
    two values the code was handed rather than about the board.
  - **NOT ONE `strat::` CALL AND NOT ONE `/Game/` LITERAL WAS ADDED.** This wave asks the rules
    module nothing: a hover is a statement about the cursor, and there is no rules question whose
    answer is which hex the mouse is over. `StratHoverState.cpp` does not include
    `StratBridge.h`. The Enhanced Input asset is the `EditDefaultsOnly` property and is authored
    in a separate editor batch.
  - **THE ONE `Triggered` BINDING IN THE FILE, AND THE REASON IS RECORDED AT THE BINDING.**
    Every other action binds `ETriggerEvent::Started` because each is a discrete decision.
    `Started` on a hover fires once when the mouse begins moving and not again, which would
    freeze the hovered hex at the first frame of a drag -- a symptom that reads as a card showing
    the wrong hex and sends the next reader to the forecast rather than to the trigger event.
  - **THE OBSERVABLE SEAM, STATED SO A CLAUSE CAN BE WRITTEN AGAINST IT.** `FStratHoverState` is
    drivable with no actor, no world, no PIE and no cursor: `SetHoveredHex` then
    `DecorateViewModel` over a default-constructed `FStratViewModel`, then read
    `Model.Hover.bHasHoveredHex` and `Model.Hover.HoveredHex`. That route reaches WHICH HEX and
    not merely THAT a hex arrived. The controller-level route
    (`AStratPlayerController::SetHoveredHex` -> `GetHoveredHex`) is the same path from the hex
    onward with only the trace removed, and needs a world but no viewport. **The split exists for
    exactly this reason and the header says so** -- a hovered hex whose only observable route was
    a live mouse over live Slate could not be pinned at all.
  - **STALE COUNTS AMENDED IN PLACE RATHER THAN LEFT -- AND THE FIRST DRAFT OF THIS BULLET WAS
    ITSELF A FALSE COMPLETENESS CLAIM, WHICH THE W0 GATE CAUGHT AND WHICH IS THE MORE USEFUL
    HALF OF THIS ENTRY.** [STAMPED 2026-08-27 on `strat-integration-reviewer`'s `VERDICT: BLOCK`,
    five findings, ALL FIVE IN COMMENT PROSE AND ALL FIVE MINE; the reviewer's own words on the
    behaviour were "the code itself is clean" and all twelve constraint checks passed. What the
    draft said: *"Four further sites said 'four' of a set that had grown twice. **Every one** is
    amended to name no number, each with an `[AMENDED, wave 0: ...]` note quoting what it said."*
    Both halves were false against the tree that sentence was written about. The census was
    wrong, and `StratPlayerController.cpp`'s input-handlers banner had been changed from "Four
    one-line calls." to "One line each." **silently** -- no quote, no stamp. A bullet about
    unstamped rewrites, containing an unstamped rewrite.]
  - **WHAT THE SUBJECT ACTUALLY WAS.** `StratPlayerController.h`'s header block stated the action
    set as both "THE FOUR ACTIONS" and "all five null" within one paragraph, and BOTH were
    already wrong before this wave -- `OpenProductionMenuAction` landed 2026-08-22 and neither
    count had moved. Beyond that: `SetupInputComponent` carried five counts of the same set
    inside ONE function ("three of the four assets", "ON ALL FOUR", "these four guards",
    "consistent with four bindings", "what these four lines state directly") while that same
    function now binds a hover action; `OnToggleProductionMenu`'s note called it "the only one of
    the five that does not go through `HandleSelectionEvent`" while wave 0's own added text a few
    lines above already named `OnHover` as a second; and the class doc had its count AND its
    tense rewritten with no quote.
  - **I DO NOT STATE A TOTAL HERE, AND THAT IS THE CORRECTION RATHER THAN AN OMISSION.** The
    previous draft typed one and was wrong by at least one within its own pass. **The census is
    the marker string, and THIS IS ITS ONE SPELLING IN THE PROJECT** -- the note in
    `AStratPlayerController`'s header block points here rather than restating it, because a
    second spelling is a second thing to go stale and, worse, a spelling *inside the searched
    tree* is a hit:

        grep -ro "\[AMENDED, wave 0" Source | wc -l

    **[STAMPED 2026-08-27 -- THE SECOND W0 GATE MEASURED THE PREVIOUS SPELLING AND IT WAS BROKEN
    TWICE OVER, WHICH IS WHY THE COMMAND ABOVE LOOKS OVER-BUILT.** It read
    `grep -c "AMENDED, wave 0" Source/StratPlay/`. Run verbatim it does not run at all:
    `grep: Source/StratPlay/: Is a directory`, printing `Source/StratPlay/:0` at **exit 2** --
    so a reader who trusts the output and not the exit code sees a census reporting **ZERO**
    notes, which is this record's own "exit code is not a verdict" shape. Add `-r` to make it
    run and it returns **16 against 15 real notes**, because the sixteenth hit was the header
    line quoting its own search token -- the recorded "a census that quotes its own search
    token" defect, and the whole of the gap between the 16 reported to the user and the 15 that
    existed. **Bracketing the token does not by itself fix this**, because the documenting line
    still contains the pattern; what fixes it is keeping the only spelling OUTSIDE the tree the
    command searches, which is why it lives in this file and not in a header.]**

    Why each part: `-r` because the argument is a directory; `-o | wc -l` rather than `-c`
    because `-c` counts matching LINES and would undercount two markers landing on one line;
    `Source` rather than `Source/StratPlay/` because notes now exist in `StratUI` too; and the
    marker stays wave-qualified because the tree already carries older `[AMENDED <date>:` notes
    from previous passes that must not be swept in -- measured, there are five of them.

    **PROVED, not asserted.** It returns **17**; demoting one real note to a non-marker returns
    **16**; restoring it returns **17** again, with `sha256sum` on the file before and after
    confirming the probe was byte-lossless.
  - **THE FIRST RUN OF THAT PROOF DAMAGED A SOURCE FILE, AND ONLY THE HASH GUARD CAUGHT IT.**
    The probe's mutation pattern had a fallback that SPANNED A LINE WRAP -- it matched
    `[AMENDED, wave 0, on the second W0 gate:` plus the newline, the `//` and the opening words
    of the quoted sentence -- so the "removal" ate `"Three entry points` and the "restore" put
    back only the short marker. **The census went 17 -> 16 -> 17 exactly as predicted while the
    file was silently broken**, because the marker count was restored and the sentence was not;
    the verdict line was clean and the tree was not. `sha256sum` before and after is the only
    reason it was found, and it is why the check is stated with the hash step rather than as
    three numbers. Repaired, re-verified, rebuilt. The re-run uses a SAME-LENGTH, SINGLE-LINE
    token swap that cannot span a wrap. This is the project's recorded "prove a restore on a
    disposable copy" lesson arriving the hard way: a probe is an edit, and an edit to prove a
    claim is still an edit. Every note quotes the sentence it replaced, so the
    number and the quotations are checkable against each other without trusting this file.
  - **THE COMPLETENESS CLAIM IS NOW DERIVED FROM `git`, MEASURED, AND FALSIFIABLE.** A throwaway
    script took every comment line at `69e75bb` carrying a count word, dropped those that survive
    verbatim, and checked that each remaining one is quoted inside some `[AMENDED, wave 0:` note
    -- subject set from `git show`, never a hand-kept list. Result: **COVERED**, and the claim
    is now true at LINE level rather than only at SITE level. **That distinction was a real
    caveat and it was closed rather than narrowed**: the reviewer found two fragments
    (`three actions rather than none` -> `those actions rather than none`, and
    `because the other three are statements` -> `because the others are`) whose blocks carried a
    stamped note but whose own words were nowhere quoted, so the site-level reading passed and
    the line-level reading did not. Both are now quoted verbatim in their block's note, which
    takes the base side to **0 uncovered** on the stricter reading.
    **It was proved able to fail, and the FIRST probe was mis-shaped** -- mutating a line wave 0
    had ADDED changed nothing, because the check's subject is lines that existed at `69e75bb`, so
    the probe routed around the guard rather than testing it. The correctly shaped probe strips
    the quote out of one note; the check then names `"Binds the four actions on the Enhanced
    Input component."` as uncovered. Both mutants were reverted and the revert proved
    byte-lossless by `sha256sum` on each file before and after. **Two instrument defects were
    found and fixed BEFORE the verdict was read**, both of which had produced false alarms: the
    note blob was flattened without stripping `//` and `*` prefixes, and the comparison was
    punctuation- and non-ASCII-sensitive, so `ON ALL FOUR.` never matched a quote ending
    `ON ALL FOUR"`. Three "findings" in the first run were pure encoding artifacts on lines
    `git diff` proves this wave never touched.
  - **WHICH ORDINALS WERE REMOVED AND WHICH WERE KEPT, on the coordinator's instruction to use
    judgement rather than delete every one.** REMOVED, because each is a CENSUS of a growing set
    and goes stale on the next property: "A SIXTH ACTION ... The five above", "exactly as the
    five above are", "the other five", "the sixth handler", "THE SIXTH BINDING", "THE FIFTH
    BINDING ... THE OTHER FOUR", "A FIFTH ACTION RATHER THAN A FIFTH MEANING", "Unlike the four
    above". KEPT, because it is the CONTRAST that is the actual point and it survives without a
    total: the hover binding is `ETriggerEvent::Triggered` where **the bindings above** are
    `Started`, and the production-menu binding is `Started` for a reason **the selection bindings
    above** do not have. Rewritten as "THE HOVER BINDING" and "THE PRODUCTION-MENU BINDING" --
    a binding named for what it does cannot go stale when a seventh arrives.
  - **HISTORICAL COUNTS SCOPED RATHER THAN DELETED.** Phase 6's measurement sentences ("all four
    `BindAction` calls proven to have run", "these four guards were silent on the null side") are
    a RECORD OF WHAT WAS MEASURED and deleting them would destroy evidence. They now read "EVERY
    `BindAction` call THAT EXISTED THEN" and "THE GUARDS THAT EXISTED THEN", each with a note
    saying the measurement is unchanged and only the implication that four is still the number is
    removed. Same treatment for "the four `IA_*` actions", which was accurate about phase 5 and
    which no reader could sort from the live counts around it.
  - **COUNTS I INTRODUCED IN THIS WAVE AND THEN HAD TO REMOVE. [STAMPED 2026-08-27: THIS BULLET
    SAID "THREE" AND SAID I HAD BROKEN THE POLICY "three times in the same pass". BOTH WERE
    FALSE -- the second W0 gate's R2 found a fourth, and a sweep that closed my check's blind
    spot then found a fifth and a sixth. A completeness claim about counts, carrying a count
    that was wrong: the F1 shape, third occurrence, and the reason no total is stated in this
    bullet either.]** What they were: `StratHoverState.cpp`'s block opened "FOUR SHORT
    FUNCTIONS"; `FStratHoverView`'s block said `AStratPlayerController` "bound five actions";
    both of my own `[AMENDED]` notes restated "the fifth"/"the sixth" while removing exactly
    those ordinals a clause earlier; `FStratHoverState::DecorateViewModel` claimed it was safe
    to call "in any order relative to the other two decorators" (**the gate's R2** -- true while
    `DecorateForPresentation` calls exactly two others, false the day W1 or W2 adds one); the
    hover section header in `StratPlayerController.cpp` said "Three entry points and one refresh
    decision, rather than three copies of the same two lines"; and `FStratHoverView`'s block
    said "The two hover STRINGS on `FStratGuidanceView`". Every one is gone, each replaced by
    the invariant the sentence was actually for -- the file's own functions are the census, the
    ordering claim does not depend on how many decorators there are, there is ONE refresh
    decision however many entry points call it, and the view-model block now DATES its claim
    (`at 69e75bb ... bound no hover action of any kind`) instead of counting. **Writing the
    policy did not stop me from breaking it repeatedly in the same pass**, which is the argument
    for the derived check below rather than for care.
  - **R2's REAL LESSON IS THE BLIND SPOT, NOT THE LINE, AND THE BLIND SPOT IS NOW CLOSED.** My
    first check's subject set was "comment lines present at `69e75bb`", so it was blind BY
    CONSTRUCTION to counts introduced on ADDED lines and in NEW FILES -- which is where R2 lived
    and why the check reported COVERED over it. That is the same defect as the first
    falsifiability probe, which mutated an added line and moved nothing: a subject set that
    excludes the wave's own new text will miss the next one identically. The rebuilt check
    sweeps BOTH sides -- 114 base count-lines and 45 added ones -- and it earned its keep
    immediately: it found the two survivors above that neither the gate nor the reviewer's own
    44-line sweep named. **The added side is a TRIAGE AID AND NOT A VERDICT**, because whether a
    count names a growing set or a fixed one is a judgement, not a match: it prints 13
    candidates that are all genuinely fixed pairs named in their own sentence ("the two unit
    bits", "addition of two coordinates", "TWO FIELDS AND NOTHING ELSE"), and a human adjudicates.
  - **ONE THING THE GATE DID NOT NAME AND I FIXED ANYWAY, plus one it did.** Not named: the three
    live census sentences in the `OpenProductionMenuAction` block, which my own second pass then
    rewrote WITHOUT quotes -- committing F1's shape a second time, inside the fix for F1. Caught
    by the derived check, now quoted. Named and fixed: the wrap artifact that left an orphaned
    "A / controller that" mid-sentence.
  - **DEBT TAKEN ON, WITH ITS DISCHARGE CONDITION, AND IT IS WRITTEN IN THE HEADERS TOO.**
    `FStratHoverView` carries a hex and a flag and nothing else -- no hovered unit id, no
    terrain, no forecast. The condition that discharges it is W1 and W2 arriving with their own
    structs and their own readers; a field landed ahead of its reader is a field written without
    its caller, and this record already describes what an unreachable-but-correct surface reads
    like.
  - **THE BUILD IS GREEN IN THIS WORKTREE**, with `-NoHotReloadFromIDE` because the editor is
    open on the integration tree and the Live Coding mutex is engine-keyed and machine-wide.
    `Result: Succeeded`, zero warnings. Built twice: once for the code
    (`StratHoverState.cpp` compiled, both module DLLs linked, 41.50 s) and once after the
    comment-only amendment pass, which still recompiles because two of the amended sites are
    inside function bodies -- `StratPlayerController.cpp` recompiled and
    `UnrealEditor-StratPlay.dll` relinked, 37.33 s. No suite was run in this tree; see the
    handoff about this slot's stale local automation report.

- **2026-08-25 (second pass that day), WRITTEN BY THE `coordinator`, NOT BY
  `strat-gameplay-engineer` -- declared, same as the entry below. THE COMPOSITION IS SPLIT OUT OF
  THE BRIDGE-TAKING BUILDER SO THAT A TAG NO TEST COULD REACH BECOMES ONE A TEST DRIVES.**
  Behaviour unchanged; this is a testability seam and is labelled as one in the header.
  - **WHAT MOVED.** `STRATUI_API void StratComposeMatchResultModel(const FStratMatchResultView&,
    const FStratScoreboardModel&, int32, FStratMatchResultModel&)` now holds every copy, every
    lookup and §2.8's key-to-criterion tag. `StratBuildMatchResultModel` is reduced to two bridge
    queries and one call: it asks `StratBuildMatchResult` for §2.8's result, asks
    `StratBuildScoreboardModel` for §2.11.4's rows into a LOCAL, and hands both to the composer.
  - **WHY IT IS A FUNCTION OVER VALUES.** The builder reaches its `FStratMatchResultView` THROUGH
    the bridge, and no bridge this suite can build reports `decidedByKey` 2 or 3 — they need a
    capped match in which both sides fought to an EQUAL combat Fame. `FStratMatchResultView` is a
    plain `USTRUCT` with a public defaulted `int32 DecidedByKey`, so a clause can hand-author one
    and drive the real composition. **That is the entire reason for the split** and the header
    says so, so nobody later "simplifies" it back.
  - **TOTAL AND INFALLIBLE.** `void`, no failure reason: every input is already a value somebody
    else refused or produced. It still assigns `OutModel` wholesale on its last line.
  - **ALL-OR-NOTHING SURVIVED AND IS MORE LEGIBLY SO — *NOT* STRONGER.** [Corrected 2026-08-25 on
    `strat-integration-reviewer`'s ruling 1; this bullet first said "STRONGER" and gave a mechanism
    that does not support it.] `Built.Scoreboard` **was already a local** in the pre-image, so a
    refusing build never touched `OutModel` there either and the guarantee is unchanged. What
    improved is legibility: the composer's signature makes the wholesale assignment structural
    instead of something a reader has to notice.
  - **ONE EXTRA COPY, ACCEPTED.** `FStratScoreboardModel` is copied into the model rather than
    built in place. Three rows, once per match, on a screen that appears at the end of one.
  - **BUILD.** `Result: Succeeded`, exit 0, twice — once for the extraction and once after the
    mutation control was reverted. Editor CLOSED throughout. No `Content/`, no
    `Source/StratRules/`, no `Data/`, no `.uproject`. No suite count and no verdict here;
    `global.md` owns both.

- **2026-08-25, WRITTEN BY THE `coordinator`, NOT BY `strat-gameplay-engineer`, AND THE DEPARTURE
  IS DECLARED RATHER THAN QUIET.** No subagent ran this pass. **A SEAM WAS ADDED SO THAT A
  CORRECT-BUT-UNTESTABLE MAPPING BECAME TESTABLE. Behaviour is unchanged -- deliberately, and
  that is the whole claim.**
  - **WHAT MOVED.** `StratBuildMatchResultModel`'s inline `switch` over
    `FStratMatchResultView::DecidedByKey` is now `STRATUI_API bool StratScoreCriterionForKey(int32
    Key, EStratScoreCriterion& OutCriterion)`, declared in `Source/StratUI/StratMatchResultWidget.h`
    beside `StratResultTierTextFor` and defined in the `.cpp` above the builder. The builder's
    seven-line switch became one line: `Built.bHasDecidedBy =
    StratScoreCriterionForKey(Result.DecidedByKey, Built.DecidedByCriterion);`.
  - **WHY IT IS A FUNCTION AND NOT A `Key - 1`.** T-UI-03's reason one layer down: the shift is
    arithmetic between two vocabularies that both already exist -- §2.8 numbers its keys 1/2/3 and
    `EStratScoreCriterion` numbers the same three criteria 0/1/2 -- and a cast off an int would
    turn every key §2.8 does not have into a real row of the scoreboard. The `switch` is total and
    every enumerator of the target names a row a display could accidentally mark.
  - **THE DISPOSITION OF `false` IS LOAD-BEARING AND IS UNCHANGED FROM THE INLINE SWITCH.**
    `OutCriterion` is written ONLY on true, so the caller's own default survives key 0 -- the
    common case, a flag kill having evaluated no key at all -- and `bHasDecidedBy` remains the
    thing that says whether to read the tag. A sentinel enumerator was not added, for the reason
    the header already gave.
  - **BUILD.** `Result: Succeeded`, exit 0, 13.18 s, green on the first attempt. Adaptive unity
    excluded `StratMatchResultWidget.cpp` and `StratMatchResultModelClauses.cpp` and rebuilt
    `UnrealEditor-StratUI.dll`. Editor CLOSED for the whole pass -- no `UnrealEditor` process --
    so no DLL write lock and no Live Coding mutex was in play. No `Content/`, no
    `Source/StratRules/`, no `Data/`, no `.uproject` change. No suite count and no verdict is
    stated here; `global.md` owns both.
  - **THE MAPPING WAS NEVER WRONG.** Key 1 -> `CombatFame`, key 2 -> `Objectives`, key 3 ->
    `SurvivingHp`, before and after, and the clause that now grades it derives that from
    `StratBuildScoreboardModel`'s own rows rather than from a table. What was wrong was that no
    test could reach two of the three arms. See `tests.md`.

- **2026-08-25, `strat-gameplay-engineer`: SEC 2.11.4'S END-OF-MATCH SCREEN HAS A C++ HALF,
  AND ITS THREE ROWS ARE THE LIVE SCOREBOARD'S OWN MODEL RATHER THAN A SECOND COPY OF SEC 2.8'S
  ORDER.** Built in worktree `E:/MultiAgent/Strat-wt/slot-1` on branch `feat/match-result-screen`,
  off `194de95`. Two new files in `Source/StratUI/`, plus wiring on `AStratScoreboardHUD` and one
  call site in `UStratMatchSubsystem`. No `Tests/`, no `Content/`, no `Source/StratRules/`, no
  `Data/`. No suite count and no verdict is stated here -- `global.md` owns both, and NO SUITE
  WAS RUN THIS PASS.
  - **THE GAP, MEASURED BEFORE IT WAS CLOSED.** `FStratMatchResultView` was projected by
    `StratBuildMatchResult`, routed by `UStratMatchSubsystem::GetMatchResult`, and READ BY
    NOTHING: Sec 2.8 had a winner and no surface could say who it was. That is the
    reflected-verb-with-no-caller shape again, one layer up -- the value was reachable and the
    SCREEN was not.
  - **THE LOAD-BEARING CALL: `FStratMatchResultModel` EMBEDS `FStratScoreboardModel` WHOLE.**
    Sec 2.11.4 asks for "the same three rows in the same order", and the two available shapes
    were (a) a private row array with its own labels and its own criteria-order walk, or (b) the
    live panel's own builder called a second time on the same bridge. (a) was killed by what
    Sec 2.11.4 exists for: revision Sec 1.5-#1 forbids the tiebreak being a hidden win condition,
    and a verdict screen spelling that order a SECOND way would reopen the hiding place one layer
    up. `StratBuildMatchResultModel` therefore calls `StratBuildScoreboardModel` and copies
    nothing of the rows itself. The chevron debt `StratScoreboardWidget.h` already records --
    delegate to the rules module when it exposes the comparison -- now covers this screen too,
    for free, which is the second reason (b) won.
  - **THE FACTION BINDING IS OURS AND IS LABELLED AS OURS.** Nothing in the GDD, `kb/setting.md`,
    the scenario JSON or the vendored rules joins a `strat` side index to a faction. `SideFaction`
    in `StratMatchResultWidget.h` pins side 0 -> Directorate, side 1 -> Vanguard on the user's
    2026-08-25 ruling, matching that file's Faction A / Faction B ordering, and its block says in
    as many words that it is a HARNESS PIN and not a rule -- `AStratScoreboardHUD::FirstSide`'s
    treatment of who-moves-first, applied to the same species of undecided thing. IF THE GDD EVER
    BINDS SIDES TO FACTIONS, THAT FUNCTION MUST DELEGATE AND NOT MERELY AGREE.
  - **THE VOICE IS THE WINNER'S AND THE COLUMNS ARE THE VIEWER'S, AND THE TWO READS ARE
    SEPARATED IN CODE.** `StratResultLineFor` takes `Winner` and never sees `ViewingSide`;
    `ViewingSide` reaches only `StratBuildScoreboardModel`'s YOU/ENEMY assignment. They agree in
    single-seat play and disagree on exactly the hot-seat turn where the loser is looking at the
    screen, which is the one case a Vanguard line under a Directorate camera would be produced by.
  - **THE SIX LINES ARE QUOTED, NOT COMPOSED, AND THE QUOTE WAS CHECKED MECHANICALLY.** A script
    extracted the six backticked samples from GDD lines 677-683 and the `TEXT(...)` literals of
    at least 20 characters from `StratMatchResultWidget.cpp`, and reported 6/6 exact matches with
    an EMPTY set of cpp strings not drawn from the GDD. Longest line 15 words against
    `kb/setting.md`'s <= 30 budget; zero hits for any of its nine banned-register words. The
    header's own "longest of the six is 15 words" sentence is that measurement and not an estimate.
  - **THE TRIGGER IS AN EDGE THAT ALREADY EXISTED.** `ConcludeMatchIfEnded` latches on
    `bMatchConclusionAnnounced` and is the only place in the class a finished match is crossed
    exactly once; `ShowMatchResult` hangs inside that latch, AFTER the `STRAT-MATCH concluded`
    log line so a gate reading the transition never depends on an asset existing. TWO THINGS
    WERE NEEDED FOR A RESTART AND NEITHER IMPLIES THE OTHER: `StartMatchInternal` already cleared
    the bool, which removes nothing from the viewport, so it now also calls
    `AStratScoreboardHUD::HideMatchResult`. The widget belongs to the HUD and outlives
    `TearDownPresentation`, so without that call the last match's verdict would sit on top of the
    new match's board.
  - **ONE DIVERGENCE FROM THE PRODUCTION-MENU PRECEDENT, PAID FOR ON PURPOSE.**
    `ProductionMenuWidgetClass` is a bare `TSubclassOf<UUserWidget>` to hold the
    `/Script/Module.Class` bake open. `MatchResultWidgetClass` is typed as
    `TSubclassOf<UStratMatchResultWidget>` and pays that bake now, because this class MUST be
    called -- `ShowMatchResult` pushes a finished model in, and a WBP free to compose its own
    verdict would be a second author of Sec 2.8's result. Recorded in both headers.
  - **BUILD.** `Build.bat StratocracyEditor Win64 Development -waitmutex -NoHotReloadFromIDE
    -MaxParallelActions=10` against the worktree's own `.uproject`; `Result: Succeeded`, process
    exit 0, first attempt. The only warnings in the log are the two pre-existing C4456/C4457 in
    vendored `Source/StratRules/Driver.good.cpp`; zero from the new or edited files.
  - **WHAT IS NOT BUILT AND WHO OWNS IT.** `WBP_MatchResult` does not exist -- no `Content/` was
    touched, which is `strat-editor-builder`'s lane, and until that asset exists and is set on the
    HUD Blueprint's `MatchResultWidgetClass`, `ConcludeMatchIfEnded` logs
    `No end-of-match screen this match: no MatchResultWidgetClass is set ...` at Warning and the
    match still concludes correctly. The screen is therefore ROUTED but not yet REACHABLE by a
    player, and that is the honest state of it.
  - **NO COPY WAS INVENTED FOR `DecidedByKey`.** The model carries the key as a raw number AND as
    an `EStratScoreCriterion` tag so the WBP can mark the deciding row with no arithmetic, and no
    sentence naming the criterion, because the GDD has not written one --
    `FStratMatchResultView::DecidedByKey`'s own refusal to name the keys, honoured rather than
    worked around.

- **2026-08-25, `strat-gameplay-engineer`: §2.11.5'S BOXED-IN FOOTER HAS A BLUEPRINT ROUTE, AND
  THE ROUTE WAS CHOSEN BY MEASURING TWO CLOCKS RATHER THAN BY PICKING A SIGNATURE.** Three new
  symbols across `StratBridge` and `StratPlay`, plus two prose corrections my own insertion made
  necessary. No `Tests/`, no `Content/`, no `Source/StratRules/`, no `Data/`. No suite count and
  no verdict is stated here -- `global.md` owns both, and NO SUITE WAS RUN THIS PASS.
  - **THE GAP, MEASURED BEFORE IT WAS CLOSED.** `grep -rn "bSpawnBlocked" Source/` outside
    `Tests/` returned exactly TWO sites: the declaration in `StratViewModel.h` and the single
    assignment in `StratViewModel.cpp`. Nothing read it. The field sits on `FStratFactoryView`,
    reachable only through `FStratViewModel::Factories`, and `UStratMatchSubsystem::GetViewModel`
    is deliberately not a `UFUNCTION`. So a correct, gated, T-UI-05-pinned field **[AMENDED
    2026-08-29: read this as pinned on BOTH sides of the bridge under two IDs rather than one.
    `bSpawnBlocked`'s module-side derivation is `T-UI-05`'s and is asserted by
    `Stratocracy.StratBridge.T-UI-05.ADisplacedSpawnLandsOnANeighbourAndSpawnBlockedStaysFalse`,
    which that ruling confirms CONFORMING; the engine-side mirror on `FStratFactoryView` is
    `T-INT-05`'s. The point this sentence is making -- that a pinned field had no reader -- does
    not depend on which of the two it named. Same marker, same instruction, same author as the
    one earlier in this file: edited by the `coordinator` on the user's direct instruction rather
    than by this lane.]** had no player
    behind it -- the same shape as `SkipGuidance` having no caller, and the check that finds it
    is the ROUTE and not the mechanism.
  - **THE TWO CLOCKS ARE REAL AND THEY DIVERGE ON THE ONE SEQUENCE THIS FOOTER IS ABOUT.**
    `RefreshProductionMenu` fills `ProductionMenu` from a LIVE bridge query and writes nothing
    else; `Factories` lives on `AppliedModel`, which `ApplyView` alone writes and whose own
    comment calls it "a record, never an input". They are independent by construction. Three
    findings, re-derived in this tree rather than reasoned from the shape:
    - **REACHABLE DIVERGENCE.** In `SubmitProductionChoice` an accepted build rebuilds the MENU
      first and the SCREEN second, and a failed `RefreshPresentation` returns with the rows
      fresh and `AppliedModel` untouched -- a case that function's own block already states
      ("A `false` RETURN AFTER AN ACCEPTED COMMAND IS POSSIBLE"). `RunAiTurnsNow` has the same
      shape. **THE BUILD THAT FILLS THE FACTORY'S LAST FREE HEX IS EXACTLY THE BUILD THAT FLIPS
      THIS BIT**, so the divergence is not on an exotic path, it is on the subject's own path.
    - **`AppliedModel` IS "WHAT WAS DRAWN" AND NOT "WHAT THE RULES SAY".** `ApplyView` is public
      and takes the model as its argument on purpose; a caller may apply a hand-built one, and
      `Source/StratPlay/Tests/` does. A footer sourced there would be a function of the last
      argument somebody passed.
    - **AND IT CANNOT ANSWER AT ALL ON A STATE THE MENU REACHES.** A refresh aimed at a hex that
      is not a build point SUCCEEDS -- `IsProductionMenuOpen`'s block says so -- and
      `Snapshot.factories` holds only factory objectives, so `AppliedModel.Factories` has no
      entry for `ProductionMenuHex` in that state. This one is decisive independently of the
      clocks and would have held even if they could not diverge.
  - **THE SHAPE THAT LANDED IS NEITHER OF THE TWO THE BRIEF OFFERED, AND THAT IS THE
    CONSEQUENTIAL CALL.** Not "read the applied model" (wrong clock) and not "re-query on every
    read" (right answer, wrong cost -- a `BlueprintPure` bound to a footer's visibility runs per
    frame). It is **re-query ON THE ROWS' OWN CLOCK and publish the answer beside them**:
    `RefreshProductionMenu` asks `FStratBridge::FactorySpawnBlockedAt` in the same call, from the
    same bridge, and assigns the result in the same statement group that assigns the rows and the
    hex; `CloseProductionMenu` clears all three together. **THE FOOTER AND THE ROWS ARE THEREFORE
    FRESH TOGETHER OR STALE TOGETHER AND CANNOT DISAGREE** -- structurally, not by discipline --
    which is the property the brief asked for, and the snapshot build is paid once per menu
    refresh instead of once per frame.
  - **IT IS NOT THE `bSeeded`-SHAPED MIRROR THIS CLASS REFUSES, AND THE TEST IS THE ONE THAT RULE
    STATES.** A mirror is refused when it CAN disagree with what it mirrors. There is no other
    source for this value in the subsystem, and the three members move as one -- which is
    `ProductionMenuHex`'s existing relationship to the rows, not `bSeeded`'s to the bridge. The
    member is deliberately **not** a `UPROPERTY` and not public: `IsOpenMenuFactorySpawnBlocked`
    ANDs `IsProductionMenuOpen()` in, so publishing the raw bool beside it would be two Blueprint
    surfaces for one fact and only one of them correct.
  - **`FStratBridge::FactorySpawnBlockedAt` PROJECTS A WHOLE SNAPSHOT TO READ ONE BOOL, AND THAT
    IS FORCED RATHER THAN CHOSEN.** `strat::spawnHexesBlocked` is declared in `Ui.h` and would
    answer the occupancy half in a bounded handful of lookups -- but it answers it about ANY hex,
    so alone it would report "boxed in" for a hex that is not a build point. The predicate that
    decides build-point-ness, `isFactoryObjective`, is in an **anonymous namespace** in
    `Ui.good.cpp` and is declared in no header (measured: zero hits over `Source/StratRules/*.h`),
    so `buildUiSnapshot`'s `factories` vector is the only sanctioned way to learn which hexes are
    factories. Re-deriving `capturable && isSpawnPoint` in the bridge was the other shape and was
    killed on one argument: it puts a second author on a rules predicate.
  - **"NOT A BUILD POINT" RIDES THE REFUSAL CHANNEL HERE WHERE IT RIDES THE ANSWER CHANNEL IN
    `BuildOptions`, AND THE ASYMMETRY IS THE POINT.** A buildlist for a non-factory hex is a real
    answer -- one row per §2.4 row, each `available` false with the module's own reason, which
    is what §2.11.5 draws. "Is this factory boxed in" has NO answer for a hex that is not a
    factory: `false` reads as "there is room", `true` as "it is full", and both are sentences
    about something that does not exist. The accessor therefore answers **false** there, and
    false when no menu is open, and both are stated as decisions rather than as fallbacks.
  - **THE FOOTER SWAPS AND THE BUILD BUTTONS STAY ENABLED, WHICH IS Q31 AND NOT THIS LANE'S
    TASTE.** Ruled 2026-08-22, restated at the field in `Ui.h`: a player MAY queue into a
    boxed-in factory, `buildWaiting` is the mechanism, and `uiBuildOptions` must not fold
    `spawnBlocked` into availability. The accessor's block says outright that ANDing this into a
    button's `bIsEnabled` would re-decide widget-side a question the rules module declined to
    fold in.
  - **A REFUSAL FROM THE NEW QUERY IS NOT LOGGED, AND THAT IS A DECISION.** Two refusals are
    reachable past a successful menu build: "not a build point", which is an ordinary keypress
    over a non-factory hex and would make a `Warning` per press out of a surface already
    explaining itself in the module's words; and the corrupt-`defIndex` case `MakeUiSnapshot`
    guards, which is a real fault and is already loud in a better place, because `BuildViewModel`
    goes through the same projection and the board has stopped rebuilding. Both fall back to
    `false`, the direction that claims nothing. Written down here because a swallowed refusal
    that is never declared is indistinguishable from one nobody thought about.
  - **TWO PROSE CORRECTIONS MY OWN INSERTION MADE NECESSARY, BOTH FOUND BEFORE THE FINAL BUILD.**
    - **A CARDINAL OF MINE THAT WAS A BOUND STATED AS AN EXACT COUNT.** "would answer the
      occupancy half in seven lookups" -- `spawnHexesBlocked` early-returns on the first free
      hex, so seven is a MAXIMUM. Reworded to a bound over a set the rules module defines
      (`HEX_DIRECTIONS`), which is the distinction this record already draws between a cardinal
      over a defined set and one over an undefined one.
    - **AND I LANDED A METHOD INSIDE A SECTION THAT THREE DATED READINGS RANGE OVER.**
      `StratBridge.h`'s façade banner recites SIX / SEVEN / SEVENTEEN as the evidence that the
      SCOPE, not the number, was the defect. `FactorySpawnBlockedAt` went into
      `// ---- Rules queries`, so the SIX reading now yields seven and the SEVEN reading eight;
      the SEVENTEEN reading did NOT move, because my signature names no `strat::` type.
      **STAMPED, NOT REFRESHED** -- refreshing them would turn a dated derivation back into the
      standing census that block exists to refuse, and the argument they support is unharmed by
      the tree moving. That the three readings moved by different amounts is itself the cleanest
      demonstration available that they were never counting one set.
  - **`BlueprintPure` WAS VERIFIED OFF THE GENERATED FLAGS AND NOT OFF THE SPECIFIER I TYPED.**
    `Intermediate/.../StratMatchSubsystem.gen.cpp` emits `IsOpenMenuFactorySpawnBlocked` with
    `EFunctionFlags 0x54020401`, which carries `FUNC_BlueprintPure` (`0x10000000`),
    `FUNC_BlueprintCallable` (`0x04000000`), `FUNC_Const` (`0x00020000`) and `FUNC_Native`. The
    exec thunk and the `NativeFunctionLookup` entry are both present, so the verb exists on the
    reflected class and not merely in a header UHT might have skipped.
  - **CRLF WAS MEASURED PER FILE AND PRESERVED, WITH A NUMSTAT CONTROL.** All four edited sources
    are CRLF; edits went in through a byte-level insert with a unique-anchor assertion, never
    `sed`. CR deltas equal added-line counts on every file (`StratBridge.h` 1092 -> 1165 with
    `git diff --numstat` reading `75 2`; `StratBridge.cpp` 1577 -> 1619 / `42 0`;
    `StratMatchSubsystem.h` 1574 -> 1670 / `96 0`; `StratMatchSubsystem.cpp` 1800 -> 1842 /
    `44 2`), so no file was rewritten whole. `grep` cannot see a CR on this box; counted with
    `tr -cd '\r' | wc -c`.
  - **BUILD: GREEN, MEASURED AFTER THE LAST EDIT.** `Build.bat StratocracyEditor Win64
    Development -project="E:\MultiAgent\Stratocracy\Stratocracy.uproject" -waitmutex` from the
    Bash tool in the integration tree -> `Result: Succeeded`, `REAL_EXIT=0`, 51 actions,
    235.13 s. `Module.StratPlay.gen.cpp` and `Module.StratBridge.gen.cpp` both compiled, so UHT
    parsed the changed headers. The editor was closed and re-verified absent in the same command
    (`tasklist | grep -ci "UnrealEditor.exe"` -> 0), with `explorer.exe` as the control that the
    instrument can speak. NO `-NoHotReloadFromIDE` was needed. THIS IS THE FINAL BUILD; an
    earlier run in this pass was 58 actions / 263.01 s and is superseded rather than deleted,
    because it is the run that first proved the new `UCLASS` surface parses.
  - **DEBT: NOTHING BINDS IT YET, AND NOTHING HEADLESS FAILS IF I DELETE IT TOMORROW.** The
    accessor exists, is reflected, and has no caller -- which is precisely the shape this record
    was corrected for on `SkipGuidance`, so it is declared rather than left to be found.
    **DISCHARGED WHEN** `WBP_ProductionMenu`'s footer binds to it (`strat-editor-builder`) and a
    clause pins it (`strat-test-author`). **OWNED: coordinator, to sequence.**
  - **DEBT: `FStratFactoryView::bBuildWaiting` AND `bHasBuiltThisTurn` STILL HAVE NO BLUEPRINT
    ROUTE, AND THIS PASS DELIBERATELY DID NOT GIVE THEM ONE.** §2.11.5's boxed-in footer is one
    sentence about one field; `bBuildWaiting` is a different sentence ("a build is queued here")
    and `Ui.h` names the case that separates them. Adding all three now would have been three
    accessors written for one measured requirement. **DISCHARGED WHEN** a GDD section names the
    surface that draws them.

- **2026-08-25, `strat-gameplay-engineer`: THE WINNER NOW REACHES ENGINE CODE, THE PRODUCTION
  MENU CAN NAME A SHORTFALL, AND THREE DEFERRALS WERE RETIRED BECAUSE THEIR PREMISES WERE
  FALSE.** Six changes across `StratBridge`, `StratUI` and `StratPlay`; one of them is a defect
  fix and the rest are new surface plus stamped retractions. No suite count and no verdict is
  stated here -- `global.md` owns both.
  - **`FStratBridge::MatchResult` ROUTES THE FOURTH `ui*` QUERY.** Upstream `96d93ea` added
    `strat::UiMatchResult` / `strat::uiMatchResult` to the vendored `Ui.h` as a QUERY beside
    `uiReachable`, `uiForecast` and `uiBuildOptions` -- not as a `UiSnapshot` field. So
    `kUiSnapshotFieldCount`, `kUiMirrorFieldCount`, `kUiDerivedFieldCount`, the transcribed
    `uiFieldContract()` table and `uiEnumerateSnapshot` are all UNMOVED, and no T-UI-05 consumer
    carries anything new. `MakeUiSnapshot` is untouched. The bridge method takes an out-parameter
    in `BuildOptions`'s shape, and it has ONE channel rather than two, because `uiMatchResult`
    holds no legality the rules could decline.
  - **IT REFUSES AN UNSEEDED BRIDGE WHERE UPSTREAM ANSWERS ONE, AND THAT IS THE ONLY DIVERGENCE.**
    `Ui.h` rules that a world with no `turn` reports InProgress / SIDE_NONE, because a missing
    input and an unfinished match are the same thing to a caller that cannot tell them apart.
    `FStratBridge` HAS `IsSeeded()` and can, so it refuses -- a result screen handed "no winner
    yet" by a bridge that never loaded a match would wait forever on a match that does not exist.
  - **`FStratMatchResultView` AND `EStratResultCause` IN `StratViewModel.h`.** The cause enum is
    ordinal-pinned to `strat::ResultCause` and mapped by an exhaustive switch (`ResultCauseOf`)
    beside `ResultTierOf`, with no `default:` label, so a cause added upstream is a compile
    failure in a file anybody may fix. `Winner` is `INDEX_NONE` on a draw and while in progress,
    and a fourth `static_assert` -- `strat::SIDE_NONE == INDEX_NONE` -- makes that a copy rather
    than a re-encoding. `StratBuildMatchResult` is the projection, and
    `UStratMatchSubsystem::GetMatchResult` is the reader beside `GetConcludedMatchView`.
  - **NO PARITY CLAUSE IS OWED FOR IT, AND THE ABSENCE IS THE POINT.** `StratViewModelParity.cpp`
    walks `FStratViewModel` against a snapshot the same bridge projected; this struct is outside
    that walk because it mirrors nothing IN the snapshot. What has to be pinned instead is the
    ROUTING -- the bridge asked and this asked, on one bridge in one frame.
  - **`FStratBuildOptionView::Shortfall` IS THE FIRST DERIVED NUMBER AT THAT BOUNDARY, AND THE
    PLAN'S PREMISE FOR IT WAS WRONG.** The plan said "the same boundary already computes derived
    values". It does not: `StratViewModel.h`'s header block claimed "NO ARITHMETIC, ANYWHERE" and
    `StratBuildViewModel` contains no `+`, `-`, `/` or `.size()` over a snapshot vector. That
    claim is now NARROWED IN PLACE in three headers rather than quietly falsified -- it still
    holds for `FStratViewModel` and everything reachable from it, and there is exactly one
    exception, in a struct that is not part of the model. `{S}2.11.5` requires `need N` on an
    unaffordable row and T-UI-03 forbids the widget subtracting, so it lands here.
    `UiBuildOption::affordable` remains the sole authority on WHETHER a row can be bought;
    `Shortfall` is forced to 0 whenever it is true and clamped at 0 otherwise, so the two cannot
    disagree about the greying even if they disagreed about the number. **DISCHARGED WHEN**
    upstream adds `shortfallFame` to `UiBuildOption`, at which point the field becomes an
    ordinary mirror and the subtraction is DELETED rather than moved.
  - **`UStratMatchSubsystem::SetViewingSide` HAD A REAL DESYNC AND IT IS FIXED BY THE ORDER OF THE
    LINES, NOT BY A COMMENT.** `AStratScoreboardHUD::SetViewingSide` range-checks BEFORE
    assigning; the subsystem assigned FIRST and then forwarded, returning `false` on the HUD's
    refusal without reaching `RefreshPresentation`. So a refused out-of-range hand-over left the
    subsystem holding a side the HUD had rejected, and every later refresh failed inside
    `StratBuildViewModel` naming the builder rather than the hand-over. Every refusal now sits
    ABOVE the assignment and the rebuild sits BELOW it, which is what distinguishes the two
    failure modes structurally. The deliberate no-rollback ON A FAILED REBUILD is unchanged and
    its reason is unchanged.
  - **THE NULL-HUD PATH WAS UNCHECKED AND IS NOW DECIDED EXPLICITLY.** With no HUD, nothing
    range-checked the side at all -- a HUD-less subsystem is a legitimate configuration and it
    used to adopt any `int32`. It now runs a TRIAL BUILD at the candidate side through
    `StratBuildViewModel` -- the same authority `RefreshPresentation` reaches, asked for the
    PROPOSED side rather than the held one -- and discards the model. No new constant and no
    third range check: the two existing authorities are still the only ones, and only WHEN they
    are consulted moved. With no HUD **and** no bridge there is no authority in existence to ask,
    so the assignment stands and `RefreshPresentation` reports "there is no bridge", which is a
    rebuild failure and not a refused side; that fall-through is stated in the code.
  - **THREE PROSE CLAIMS OF MY OWN WENT FALSE IN THE SAME PASS THAT WROTE THEM, FOUND BY THE
    WAVE 3 REVIEWER GATE AND CORRECTED IN PLACE 2026-08-25.** All three are the same class as
    the "NO ARITHMETIC, ANYWHERE" claim above -- a COUNT or an unqualified QUANTIFIER that the
    change itself invalidated -- which is the pattern worth carrying forward, not the three
    instances.
    - **A CENSUS THAT COUNTED THE WRONG THING.** `StratViewModel.cpp`'s header block said
      "The static_asserts -- FOUR since 2026-08-25". The file has THREE `static_assert(`
      statements (HEAD had two; I added `strat::SIDE_NONE == INDEX_NONE`). The block then
      listed four SUBJECTS, and the fourth -- the `ResultTier` / `ResultCause` enumerators --
      is real but is pinned by a DIFFERENT MECHANISM: an exhaustive switch with no `default:`
      label. So the sentence conflated a count of asserts with a count of pinned subjects. It
      now states three asserts and two switches, names each, and says why an enum's
      MEMBERSHIP cannot be `static_assert`ed the way a constant's VALUE can.
      `StratViewModel.h`'s narrowing paragraph disagreed with the .cpp about the same objects
      and now defers to the .cpp block as the authority.
    - **AND THE CORRECTION REINTRODUCED THE DEFECT ONE LEVEL UP -- A DIFFERENT MECHANISM,
      WORTH ITS OWN LINE.** The fixed sentence read "the number is stated because this list
      is the census and a reader will count `static_assert(` to check it". It named the exact
      token it told the reader to search for, so the sentence became a member of its own
      subject: `grep -c "static_assert(" Source/StratUI/StratViewModel.cpp` returned **4**
      against a stated THREE -- three statements plus the census sentence. Found by the Wave 3
      reviewer, measured before acting on it.
      **THE GENERAL SHAPE, AND IT IS NOT THE SAME LESSON AS THE BULLET ABOVE: A CENSUS THAT
      QUOTES ITS OWN SEARCH TOKEN JOINS THE SET IT IS COUNTING.** The bullet above is a count
      invalidated by the change that wrote it; this one is a count invalidated by the sentence
      that states it, and no amount of re-counting the subjects would have caught it. The
      repo's neighbouring instance is prefix nesting -- `BP_` inside `WBP_` inflating a census
      by 19 -- which is the same failure with two tokens instead of one.
      **THE FIX IS AN ANCHOR THAT PROSE STRUCTURALLY CANNOT SATISFY**, not a reworded
      sentence. The block now prescribes `grep -c '^[[:space:]]static_assert'`, which a
      comment line cannot match because a comment line begins with `/`. Two figures in that
      block are stated and both were measured on the file as it now stands: the anchored form
      returns **3** (equal to the stated count) and an unanchored search for the bare token
      returns **8**, because the prose names the token five more times -- which is exactly why
      the unanchored form is not a census at all.
      **THE ONLY CHECK THAT WOULD HAVE CAUGHT EITHER OF THESE IS RUNNING THE COUNT THE
      SENTENCE ITSELF PRESCRIBES, ON THE FILE AS EDITED, BEFORE REPORTING.** Reading the
      sentence and re-counting the subjects passed twice. That is the practice this entry is
      really recording.
    - **AND THE ANCHOR WAS ONE CHARACTER SHORT OF IMMUNE -- A THIRD MECHANISM, FOUND BY THE
      RE-GATE AND MEASURED BEFORE ACTING.** The prescribed check was
      `grep -c '^[[:space:]]static_assert'`, which matches EXACTLY ONE whitespace character.
      It returned 3, but only because all three statements happen to sit at namespace scope
      at one tab: one added inside a function body, a nested namespace or a class would sit
      deeper and would NOT have matched, and the census would have UNDER-reported silently.
      The check is now `grep -c '^[[:space:]]*static_assert'` -- still unable to match a
      comment line, because `/` is not whitespace, and now indifferent to depth. Verified on
      the file as edited: **returns 3**, equal to the stated count, matching the same three
      statements. Build after the change: `Result: Succeeded`, exit 0.
      **THE PATTERN ACROSS ALL THREE IS THE POINT.** (a) counted the wrong subject, (b)
      quoted its own search token, (c) over-constrained the anchor. Each fix was sound and
      each introduced the next, which is why "a census is correct" is not a property a
      single reading establishes -- only running the prescribed check on the edited file
      does, and that is now the third time it was the only thing that would have.

    - **HELD, NOT FIXED: `StratBridge.h`'s FACADE BANNER, BECAUSE MY COUNT DISAGREES WITH THE
      REVIEWER'S AND A CONFIDENTLY WRONG NUMBER IS WORSE THAN AN UNCHECKABLE ONE.** The
      re-gate asked me to replace the banner's cardinals with NAMED methods -- the right
      instruction -- but supplied a figure I cannot reproduce, and the brief's own rule is to
      settle it before editing. Re-derived against the header, not taken from the relay:
      - **AGREED, and it confirms the sentence is wrong today.** The facade section declares
        **SEVEN** methods -- `Turn`, `SideToMove`, `ReachableHexes`, `AttackTargetHexes`,
        `SubmitMoveToHex`, `SubmitAttackAtHex`, `SubmitBuildAtHex` -- of which **FIVE**
        forward to a typed method beside them. `Turn()` and `SideToMove()` forward to
        nothing: both read `GameState.turn` directly in the .cpp. So the banner's inherited
        "The six methods below" and its "each one forwards to the typed method beside it"
        cannot both describe the same set. **THAT AMBIGUITY IS PRE-EXISTING AND NOT MINE**;
        I preserved it when I corrected the other half of the sentence.
      - **DISPUTED.** The relay says the methods above the banner naming a `strat::` type
        number SIX. I count **SEVEN** in the two sections immediately above it (`// ---- View
        model` and `// ---- Rules queries`): `MakeUiWorld`, `MakeUiSnapshot`,
        `CheckSnapshotFidelity`, `Reachable`, `Forecast`, `BuildOptions`, `MatchResult`.
        `MakeUiWorld` returns `strat::UiWorld`, sits above the banner, and is public -- the
        class has one `public:` and one `private:` and every declaration in that band is on
        the public side. And under the OTHER available reading of "above the banner" -- the
        whole class -- the answer is neither six nor seven but **17**, because `Submit`,
        `SubmitMove`, `SubmitAttack`, `SubmitBuild`, `ReplayLog`, `RecordedLog`, `State`,
        `UnitDefs`, `Tables` and `ScenarioData` all name a `strat::` type too.
      **SO THE REAL DEFECT IS NOT THE CARDINAL, IT IS THAT "ABOVE" HAS NO DEFINED SCOPE** --
      which is exactly why two careful readers got two different numbers, and why guessing a
      band in order to name its members would bake my guess into the header. **WHAT IS OWED:**
      a ruling on which set the sentence means, then a rewrite that NAMES those methods and
      states no cardinal at all. The substantive half stands either way and is untouched:
      `MatchResult` needs no `int32`/`FIntPoint` mirror because its consumer is `StratUI`,
      which MAY name a `strat::` type, and `StratBuildMatchResult` hands `StratPlay` a struct
      naming nothing vendored. **OWNED: coordinator, to settle the scope.**
      - **SETTLED AND LANDED 2026-08-25. The coordinator re-derived rather than adjudicating
        between two reports, confirmed SEVEN, and cited the header's own prose -- "PRIVATE,
        WHERE `MakeUiWorld` IS PUBLIC" -- which settles the public/private half from inside
        the file.** THE RULING WENT PAST THE NUMBER: the sentence stops depending on "above"
        at all, because picking a band would have left the next reader re-deriving that
        choice from a sentence that still did not state it. It now describes THIS SECTION and
        its forwarding relation -- well-defined without a scope convention and checkable by a
        reader standing in one place -- and NAMES `Turn()` and `SideToMove()` as the two that
        mirror nothing, with no cardinal for either section.
      - **THE CORRECTION HAD TO REACH THREE SENTENCES, NOT ONE, AND I FOUND THE THIRD.** The
        coordinator named two: the banner, and "THEY ADD NO POLICY. Each one forwards to the
        typed method beside it..." -- the second mattering more because it states a GUARANTEE
        rather than a count, so a reader is likelier to rely on it, and a fix reaching only
        the banner leaves the false claim standing where it does work. `Turn()` and
        `SideToMove()` refute it the same way: they forward to nothing, convert nothing, and
        cannot refuse at all, so "every refusal is the typed method's" is VACUOUS for them
        rather than true. **THE THIRD WAS "OUT OF LINE, DELIBERATELY, all six"** -- the
        section declares seven, and every one of them is out of line, established by
        inspection (no declaration in the section carries a body) rather than by counting.
        Its stated reason -- an inline body would instantiate the caller's TU over
        `strat::Hex` -- does NOT reach `Turn()`/`SideToMove()`, which name no vendored type;
        that is now said, so a reader does not take the argument as covering a case it does
        not. Three cardinals in one block, all removed.
      - **TWO CARDINALS IN THE SECTION WERE CHECKED AND DELIBERATELY LEFT ALONE**, because
        both name a set the tree defines: "The five `Submit*` methods" (the section banner is
        "Typed commands (§4.9's five, and no others)", and `strat::SaveCommandKind` is pinned
        at five) and "§2.9's AI emits Build as one of its four kinds" (`strat::AiCommandKind`
        is `{Build, Move, Attack, EndTurn}`). A cardinal over a DEFINED set is checkable; the
        defect is a cardinal over an undefined one.

    - **MECHANISM (d), AND IT IS THE ONE THAT BREAKS THE PATTERN OF THE OTHER THREE: A
      QUANTIFIER OVER A SET THE SENTENCE NEVER DEFINES.** (a) counted the wrong subject,
      (b) quoted its own search token, (c) over-constrained the anchor -- and **each of those
      three was findable by ONE reader running ONE check.** (d) was not findable that way at
      all: every reader who ran a count got a self-consistent answer, and the answers differed
      only because each had silently supplied a different scope. It surfaced solely because
      two readers compared results and a third reading (17) showed the disagreement was not a
      tie to be broken. **SO THE PRACTICE THAT CATCHES (a)-(c) -- run the check the sentence
      prescribes -- CANNOT CATCH (d).** What catches (d) is refusing to write a cardinal whose
      set the sentence does not name, and, when one is disputed, holding the edit rather than
      picking the likelier number. Holding was the right call here on the coordinator's own
      verdict: a confidently wrong list baked into the header would have been worse than the
      uncheckable number it replaced.
    - **A BANNER OFF BY ONE BECAUSE OF MY OWN INSERTION.** `StratBridge.h`'s engine-typed
      facade read "The six methods below say exactly what the five above say"; `MatchResult`
      was inserted immediately above it, so a reader counting finds seven. Corrected, AND the
      asymmetry is now stated rather than left to look like an omission: `MatchResult` needs
      no `int32`/`FIntPoint` mirror because its consumer is `StratUI`, which MAY name a
      `strat::` type, and `StratBuildMatchResult` hands `StratPlay` a struct that names
      nothing vendored. A seventh mirror would be a second translation of one value.
    - **AN UNQUALIFIED "EXACTLY ONE WRITER" THAT AUTOMATION HAD ALREADY FALSIFIED.**
      `UStratMatchSubsystem::ProductionMenu`'s block said "there is exactly one writer,
      `RefreshProductionMenu`". `Source/StratPlay/Tests/StratProductionMenuSeam.cpp` assigns
      both `ProductionMenu` and `ProductionMenuHex` directly, to re-plant a menu a deliberate
      reseed cleared. Nothing is broken -- `BlueprintReadOnly` never held C++ in this module
      out, the plant is declared at its own site, and the REASON the block gives is about
      Blueprints and still holds. Stamped: shipping writers are `RefreshProductionMenu` and
      `CloseProductionMenu`, and the invariant a widget author may rely on is the one about
      Blueprints, not a total writer count. **CHECKED AND CLEAR WHILE I WAS THERE:** that
      file's out-of-range viewing side is arranged through `StartMatch` -- which assigns
      `Config.ViewingSide` without a range check, deliberately and by its own comment -- and
      NOT through `SetViewingSide`, so the 4b reordering does not touch it.
  - **BUILD: GREEN, MEASURED AFTER THE LAST EDIT.** `Build.bat StratocracyEditor Win64
    Development -project="E:\MultiAgent\Stratocracy\Stratocracy.uproject" -waitmutex
    -NoHotReloadFromIDE` from the Bash tool in the integration tree -> `Result: Succeeded`,
    exit 0, 64 actions, 317.62 s. The editor was closed for it. THIS IS THE FINAL BUILD, run
    after the three prose corrections above; the earlier one in this pass was 59 actions /
    289.55 s and is superseded rather than deleted, because the action count moved for a
    reason that is not mine -- `strat-test-author` landed five new clause files in the same
    tree between the two runs, and a reader comparing the numbers should not read that as
    engine code growing. No suite figure is stated here; `global.md` owns it, and the clauses
    are `strat-test-author`'s.

- **2026-08-24, `strat-gameplay-engineer`: THREE COMMENTS NAMED BLUEPRINTS THAT DO NOT EXIST, IN
  THE SENTENCES THAT ASSIGN THE WORK TO THE CONTENT LANE. COMMENT-ONLY; NO DECLARATION, SIGNATURE
  OR EXECUTABLE LINE MOVED.** Four name instances across `StratBoardActor.h`, `StratUnitActor.h`
  and `StratGuidedOpening.h`: `BP_StratBoardActor` -> `BP_StratBoard`, `BP_StratUnitActor` ->
  `BP_StratUnit`. `Content/StratPlay/` holds six Blueprints and no `...Actor` variant of anything;
  the content lane's own record has said so since 2026-08-23. No suite count and no verdict is
  stated here -- `global.md` owns both, and no suite figure moves on a comment.
  - **WHY THIS IS NOT AN ORDINARY STALE COMMENT.** All three sentences are hand-off sentences:
    each one names the asset the CONTENT lane is being told to author or assign. A stale comment
    misinforms whoever reads it; a comment that names a non-existent asset in the sentence that
    ASSIGNS the work sends the receiving lane to search `/Game/StratPlay` for a name that has
    never been there, and the search comes back empty with nothing to indicate the brief was
    wrong rather than the asset missing. That asymmetry is the reason this was worth a pass.
  - **BUILD: 22 OF 25 ACTIONS COMPILED CLEAN; THE LINK WAS REFUSED BY THE OPEN EDITOR AND THAT
    WAS THE EXPECTED OUTCOME.** `Build.bat StratocracyEditor Win64 Development -waitmutex
    -NoHotReloadFromIDE`, 172.58 s. UHT re-ran (`Invalidating makefile ... working set of source
    files changed`) and `Module.StratPlay.gen.cpp` compiled, so the three edited headers parsed.
    Then `UbaSessionServer - ERROR opening file ...UnrealEditor-StratPlay.dll for write after
    retrying for 20 seconds`, and on the non-UBA retry `LINK : fatal error LNK1104: cannot open
    file 'E:\MultiAgent\Stratocracy\Binaries\Win64\UnrealEditor-StratPlay.dll'`. **For a
    comment-only change the compile IS the evidence** -- what a comment edit can break is a block
    comment or a line continuation, and that is a compile error, not a link error. The link adds
    nothing here and its refusal is not a finding.
  - **`sed` STRIPPED THE CARRIAGE RETURNS AND IT DID NOT ANNOUNCE ITSELF.** `StratBoardActor.h`
    and `StratGuidedOpening.h` are CRLF in this worktree (461 and 528 CRs); `StratUnitActor.h` is
    LF (0 CRs) -- measured per file, not assumed. An in-place `sed` rewrote both CRLF files whole
    and `git diff --numstat` would have read `461 461` instead of `1 1`. Restored before
    inspecting the diff. Measure endings per file before any scripted edit to a header.
  - **TWO NEIGHBOURING SENTENCES ARE NOW WRONG AS A CONSEQUENCE, AND ARE DELIBERATELY NOT FIXED
    IN THIS PASS.** Both say the assignment is still owed, and it is not: `BP_StratBoard`'s bytes
    carry `/Game/StratArt/Materials/MI_Overlay_Objective` against `ObjectiveMaterial`, and
    `BP_StratUnit`'s carry `/Game/StratArt/Meshes/SM_GuidedMarker` and
    `/Game/StratArt/Materials/MI_Marker_Guided` against `GuidedMarkerMesh` and
    `GuidedMarkerMaterial`. So `ObjectiveMaterial`'s "UNSET IS LEGITIMATE AND IS THE STATE THIS
    SHIPS IN", `GuidedMarkerMesh`'s identical clause, and `StratGuidedOpening.h`'s "What is still
    the content lane's is the asset half: the meshes, the material instances, and their
    assignment" all describe a tree that stopped existing when those defaults were committed.
    Kept out so this diff stays exactly what its message claims. **Discharged by** a follow-up
    pass that rewrites those three blocks to say the defaults are authored -- and note the ring
    needs no new mesh at all, it instances `OverlayMesh`, so "the meshes" is plural about one.
    **DISCHARGED 2026-08-24 by the entry immediately below.** (Its own lead says "TWO
    NEIGHBOURING SENTENCES" and then lists three; three is the count, and three were fixed.)

- **2026-08-24, WRITTEN BY THE `coordinator` INTO THE ENGINEER'S FILE, DECLARED: THE THREE
  BLOCKS THE ENTRY ABOVE DEFERRED NOW SAY THE DEFAULTS ARE AUTHORED. COMMENT-ONLY; NO
  DECLARATION, SIGNATURE OR EXECUTABLE LINE MOVED.** `strat-gameplay-engineer` is `engine.md`'s
  sole writer and was not dispatched for this pass; the user asked for the fix directly in the
  main session. Recorded here rather than nowhere, and named so the next reader knows whose hand
  it was. No suite count and no verdict is stated here -- `global.md` owns both, and no suite
  figure moves on a comment.
  - **WHAT EACH BLOCK NOW SAYS.** `AStratBoardActor::ObjectiveMaterial` and
    `AStratUnitActor::GuidedMarkerMesh` are rewritten in place: the assignment is stated as
    landed and the instance is named, "unset is legitimate" is kept but demoted to *any other
    Blueprint of this class* rather than the shipping state, and each block says outright that
    it used to claim otherwise. `StratGuidedOpening.h`'s block gets an APPENDED
    `[AMENDED 2026-08-24: ...]` instead of a rewrite -- the sentence under it is itself an
    amendment, and leaving it standing keeps the boundary it drew legible when only its tense
    is wrong.
  - **"THE MESHES" WAS PLURAL ABOUT ONE, AND THE AMENDMENT SAYS SO.** The ring instances
    `AStratBoardActor::OverlayMesh`; `SM_GuidedMarker` is the only mesh the content lane added.
  - **THE PREMISE WAS RE-MEASURED OFF THE PACKAGE BYTES, NOT CARRIED OVER FROM THE ENTRY
    ABOVE.** `grep -a` over the working-tree `.uasset`s: `BP_StratBoard.uasset` yields
    `/Game/StratArt/Materials/MI_Overlay_Objective` alongside the name `ObjectiveMaterial`;
    `BP_StratUnit.uasset` yields `/Game/StratArt/Meshes/SM_GuidedMarker` and
    `/Game/StratArt/Materials/MI_Marker_Guided` alongside `GuidedMarkerMesh` and
    `GuidedMarkerMaterial`. Working-tree bytes, not `git show` -- `.uasset` is LFS there. Both
    packages are clean in `git status`, so the defaults are committed, not pending.
  - **BUILD: 22 OF 25 ACTIONS COMPILED CLEAN; THE LINK WAS REFUSED BY THE OPEN EDITOR, THE SAME
    EXPECTED OUTCOME AS THE PASS ABOVE.** 169.34 s.
    `Module.StratPlay.gen.cpp`, `StratBoardActor.cpp`, `StratGuidedOpening.cpp` and
    `StratUnitActor.cpp` all compiled, so all three edited headers parsed. Then
    `UbaSessionServer - ERROR opening file ...UnrealEditor-StratPlay.dll for write` and on the
    non-UBA retry `LINK : fatal error LNK1104`. For a comment-only change the compile IS the
    evidence; the link adds nothing and its refusal is not a finding.
  - **THE CARRIAGE-RETURN TRAP FROM THE PASS ABOVE WAS AVOIDED BY NOT USING `sed`.** Edits made
    through the line-local editor; CRs counted before and after with `tr -cd '\r' | wc -c`
    (`grep` cannot see a CR on this box). `StratBoardActor.h` 461 -> 464, `StratGuidedOpening.h`
    528 -> 536, `StratUnitActor.h` 0 -> 0, each delta equal to that file's added-line count.
    `git diff --numstat` reads `7 4` / `9 1` / `7 3` -- line-local, not a whole-file rewrite.

- **2026-08-24, `strat-gameplay-engineer`: THE TURN-1a MARKER COULD NEVER CLEAR, BECAUSE EVERY
  OPERAND IT HAD WAS MATCH-CONSTANT. THE CODE COMPILES AND THE TREE LINKS.** Found in a human
  playtest, not by any clause. No suite count and no verdict is stated here; `global.md` owns
  both, and NO SUITE WAS RUN THIS PASS -- it is the test author's and is owed after this
  signature.
  **[HEADLINE AMENDED 2026-08-24, LATER THE SAME MORNING, BY THIS ENTRY'S OWN AUTHOR.** It read
  "THE CODE COMPILES AND THE TREE DOES NOT LINK", which was true of the tree it described: the
  editor held the DLL. The editor closed and the link completed. The two build bullets at the
  foot of this entry carry both runs and neither is deleted, because the compile-only measurement
  is what separated "the code is wrong" from "the editor is open".]**
  - **THE DIAGNOSIS IS CONFIRMED IN THIS TREE AND WAS RE-DERIVED RATHER THAN TAKEN ON REPORT.**
    `AStratUnitActor::ApplyUnitView` set the marker from
    `View.bIsGuidedMarked && View.Side == ViewingSide`. `bIsGuidedMarked` is the rules module's
    derivation off `placement` -- `StratViewModel.cpp` copies `Source.isGuidedMarked` and is its
    only producer -- and both sides are fixed for a match. So the conjunction HAD NO FALSE IN IT
    once true, and the marker latched on for the rest of the match. The player saw the objective
    ring clear with the Infantry's marker still lit.
  - **THE DURABLE FINDING IS NOT THE MISSING BIT. BOTH-DIRECTIONS IS A PROPERTY OF THE OPERAND
    SET AND NOT ONLY OF THE WRITER.** The comment directly above the defective line already named
    this exact failure mode -- "a writer that only ever SHOWS is a writer whose hide can be missed
    on one path -- and for this bit that means a permanent marker on a unit §2.11.6's window
    closed on" -- and the writer SATISFIED it: `SetVisibility` ran unconditionally, in both
    directions, on every one of the ~1700 calls a suite run makes. **A correctly-written
    unconditional writer produced a latch**, because the discipline was checked at the assignment
    and never at the inputs. That sentence is now in `GuidedMarker`'s own block.
  - **THE MISSING OPERAND IS `FStratViewModel::Guidance.bActive`, AND THE GDD PICKED IT RATHER
    THAN THIS LANE.** §2.11.6, quoted in `StratGuidedOpening.h`: the ring "and the turn-1a unit
    marker clear in the same frame as the strip". The strip IS `bActive`. **NOT "beat 1a
    retired"** -- that would clear the mark the moment the beat it belongs to succeeded, one beat
    early -- and **NOT a hex test**, which `StratGuidedOpening.h` records as the trap that unmarks
    the unit at the exact moment beat 1a needs it marked. `bIsGuidedMarked` WAS NOT TOUCHED and
    must not be; `StratViewModelParity` pins it and a second producer is the drift the view model
    header exists to refuse.
  - **THE RING'S BEHAVIOUR MAKES "SAME FRAME" STRUCTURAL AND THAT WAS MEASURED, NOT ASSUMED.**
    `FStratGuidedOpening` writes `bHasObjective = false` in exactly three places -- `SkipGuidance`,
    the turn-4 window close, the all-beats-retired branch -- and ALL THREE set `bActive = false`
    beside it; `DecorateViewModel` assigns `View.bActive = bActive` unconditionally and writes
    `bHasObjectiveRing` only inside `if (bActive)`. So a frame in which the ring is out is a frame
    in which `bActive` is out, and the marker now rides the same bool through the same
    `UStratMatchSubsystem::ApplyView` that drives the ring twenty lines below it.
  - **THE SIGNATURE GREW A FOURTH PARAMETER AND IT IS A `bool`, NOT THE `FStratGuidanceView`.**
    `AStratUnitActor::ApplyUnitView(const FStratUnitView& View, const FVector& WorldLocation,
    int32 ViewingSide, bool bGuidanceActive)`. Passing the whole struct was the other shape and
    was killed on one measurement rather than on taste: it would put `ObjectiveHex` within reach
    of the marker writer, which is the hex-keyed derivation `GuidedMarker`'s block forbids by
    name. **WHAT THIS CLASS CANNOT SEE, IT CANNOT USE**, and that is the same argument the
    `ViewingSide` parameter was shaped by on 2026-08-23. Not defaulted, for that parameter's
    recorded reason: a default lets a future caller get a latching marker silently.
  - **ONE CALL SITE, MEASURED AND NOT ASSUMED.** `grep` for `->ApplyUnitView(` over `Source/`
    returns exactly one line, in `UStratMatchSubsystem::ApplyView`, which already held the whole
    `FStratViewModel`. **NO FILE UNDER `Tests/` CALLS IT**, so no lane but this one had to move --
    corroborated at compile time, since `StratGuidedOpeningVisuals.cpp` and
    `StratMatchReconcile.cpp` both rebuilt clean against the new declaration.
  - **THE FALSE SENTENCE IN `FStratGuidedOpening::SkipGuidance` IS STAMPED, NOT DELETED.** It read
    "the marker clears because that same call publishes `bActive` false and the marker is drawn
    only while guidance runs" -- **an unimplemented mechanism asserted as fact for a day**, and it
    is exactly the shape this record has been corrected for before. It is left standing with a
    dated stamp because it is TRUE OF THIS TREE NOW and a reader should see that it described an
    intention before it described an implementation. **A SENTENCE IN ONE FILE ASSERTING WHAT
    ANOTHER FILE DOES IS A CLAIM AND NOT A SPECIFICATION.**
  - **A SECOND FALSE SENTENCE WAS FOUND BY FOLLOWING THE FIRST, AND THE BRIEF DID NOT NAME IT.**
    `FStratGuidedOpening::SkipGuidance`'s DECLARATION in `StratGuidedOpening.h` said "the marker
    reads `bIsGuidedMarked` on a model the very next `DecorateViewModel` writes with `bActive`
    false" -- which implies `bIsGuidedMarked` falls with `bActive`, and it does not and must not.
    Retracted in place with the `RETRACTED>` form. The `.cpp` and the `.h` carried the same wrong
    belief in different words, which is why fixing the site the brief named would not have
    finished the job.
  - **THE COUNT IN `GuidedMarker`'s BLOCK MOVES A SECOND TIME AND IS NOW RETIRED AS AN
    INVARIANT.** ONE until 2026-08-23, TWO until 2026-08-24, THREE today; both prior wordings are
    kept as `RETRACTED>` quotes. The "rendering of two fields" sentence further down is rewritten
    **COUNT-FREE** rather than re-numbered, on the precedent this file already set for the overlay
    count: the invariant was never the arithmetic, it is that every operand comes off ONE model
    and none is remembered. Re-numbering to three would rebuild the same trap for whichever
    ruling adds a fourth.
  - **[SUPERSEDED 2026-08-24, LATER THE SAME MORNING, BY THIS ENTRY'S OWN AUTHOR: THE EDITOR
    CLOSED AND THE TREE LINKED. The bullet below is KEPT rather than deleted -- it carries the
    twenty compiles and the zero diagnostics that the two-action link run does NOT contain, so it
    is the only record of this change ever being compiled. Do not read it as a live instruction to
    rebuild.]**
  - **THE BUILD COMPILED EVERYTHING AND LINKED NOTHING, AND THE EDITOR WAS OPEN BY ARRANGEMENT.**
    `Get-Process` returns `UnrealEditor` PID 88652, with `explorer.exe` PID 13508 in the same
    round as the control that shows the instrument can speak. `Build.bat` with the documented
    arguments PLUS `-NoHotReloadFromIDE`, twenty-three actions: **all twenty compiles succeeded**,
    `[1/23] Module.StratPlay.gen.cpp` -- so UHT parsed the changed header -- through
    `[20/23] StratUnitActor.cpp`, with all five changed/adjacent sources excluded from the unity
    file per `[Adaptive Build]`. **ZERO WARNINGS AND ZERO COMPILER DIAGNOSTICS.** Then
    `UbaSessionServer - ERROR opening file E:\MultiAgent\Stratocracy\Binaries\Win64\UnrealEditor-StratPlay.dll for write after retrying for 20 seconds (The process cannot access the file because it is being used by another process. - C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe)`,
    then on the non-UBA retry `LINK : fatal error LNK1104: cannot open file 'E:\MultiAgent\Stratocracy\Binaries\Win64\UnrealEditor-StratPlay.dll'`,
    `Result: Failed (OtherCompilationError)`, `REAL_EXIT=6`, 158.47 s.
  - **THIS IS THE WRITE LOCK AND NOT THE LIVE CODING MUTEX, AND THE FLAG BEHAVED EXACTLY AS
    `.agents/ue-project-context.md` PREDICTS.** That file records that `-NoHotReloadFromIDE`
    defeats the engine-keyed mutex but NOT this tree's DLL write lock, and that the symptom is a
    full compile followed by a UBA write error and then `LNK1104`. Observed shape matches
    line-for-line, including the retry wording. The mutex form aborts ahead of the action graph
    with zero compiles; this ran twenty. **NOT WORKED AROUND, NOT RETRIED, AND NO DLL DELETED** --
    the fix is the editor closing.
  - **THE LINK COMPLETED ON A SECOND RUN WITH THE EDITOR CLOSED, AND IT WAS TWO ACTIONS, WHICH IS
    STATED PLAINLY RATHER THAN GLOSSED.** `Get-Process UnrealEditor` returns nothing, with
    `explorer.exe` PID 13508 in the same round as the control. Same command, same flags:
    `Using Unreal Build Accelerator local executor to run 2 action(s)`,
    `[1/2] Link [x64] UnrealEditor-StratPlay.dll`, `[2/2] WriteMetadata StratocracyEditor.target`,
    `Result: Succeeded`, `REAL_EXIT=0`, 2.44 s. **THE TWENTY COMPILES ARE THE EARLIER PASS'S AND
    NOT THIS RUN'S** -- this run compiled nothing at all. That is sound here and the soundness was
    measured rather than asserted: the objects were produced by a BYTE-IDENTICAL INVOCATION over
    BYTE-IDENTICAL SOURCES. All five changed sources carry mtimes of 10:10:36 - 10:12:38, every
    one of them BEFORE the 20-compile pass, and `git hash-object` over all six changed files
    returns the same digests after this run as before it. No file was touched between the compile
    and the link.
  - **A NEW DLL EXISTS AND THE FILE MOVED, WHICH IS THE ONLY THING THAT DISTINGUISHES A LINK FROM
    A CACHED `Succeeded`.** `Binaries/Win64/UnrealEditor-StratPlay.dll` went from **1431552 B /
    2026-08-23 19:46** to **1435136 B / 2026-08-24 10:22** -- 3584 bytes larger, stamped in the
    run's own minute. **THE FAILED PASS HAD ALREADY MOVED THE `.pdb` AND NOT THE `.dll`**
    (`UnrealEditor-StratPlay.pdb` stamped 10:15 while the DLL still read 19:46), which is worth
    knowing: on an `LNK1104` the debug symbols can advance while the binary does not, so a `.pdb`
    timestamp is NOT evidence that a link happened.
  - **[STAMPED 2026-08-24, LATER THE SAME DAY. THE FIGURE ABOVE WAS CORRECT FOR THIS LANE'S LINK
    AND NO LONGER DESCRIBES THE BINARY ON DISK, WHICH IS EXPECTED RATHER THAN A DRIFT.]** A reader
    who stats `UnrealEditor-StratPlay.dll` now gets **1460224 B / 2026-08-24 10:40:11**, not the
    **1435136 B / 10:22** recorded above. Nothing was rewritten and nothing was relinked here:
    `strat-test-author` added roughly 690 lines of clauses to `Source/StratPlay/Tests/` and
    relinked the same module eighteen minutes later, in its own lane. **THE ORDERING IS COHERENT
    AND IS THE POINT** -- this lane's link at 10:22 local, the test lane's relink at 10:40:11, and
    the suite at `reportCreatedOn 2026.08.24-14.41.49` UTC, which is 10:41 local, so the suite ran
    against the LATER binary and that binary contains this change. Re-measured here with `ls -la`
    over the whole directory rather than taken on report, with the four untouched sibling DLLs
    (`StratBridge`, `StratRules`, `Stratocracy` at 2026-08-22, `StratUI` at 2026-08-23) as the
    control that the listing reports real per-file stamps and not one blanket time.
    **THE ORIGINAL FIGURE IS KEPT AND MUST NOT BE REFRESHED TO MATCH THE DISK.** It is the only
    record that this lane's link produced a binary at all -- the two-action run compiled nothing,
    so the moved byte count is the whole of that evidence. A number that tracks the current file
    would prove nothing about any particular link, which is the property being protected. **A
    BUILD ARTEFACT FIGURE IN THIS RECORD DESCRIBES A RUN, NOT A FILE**, and a later in-lane relink
    is the normal way it stops matching. `strat-integration-reviewer` raised this as an
    observation and did not gate on it; its verdict and the lane its one finding belongs to are
    stated where they are owned, and are deliberately not restated here.
  - **AN INSTRUMENT LIED SILENTLY AND ONLY A CONTROL CAUGHT IT.** `Get-Item` on the DLL through
    the PowerShell tool printed NOTHING -- no object and no error -- for a file that exists, which
    reads exactly like "the binary was never produced" and would have contradicted the
    coordinator's baseline. `ls -la` over the directory, with a total-entry count as the control,
    returned the file at precisely the coordinator's figures. **A NULL RESULT FROM A STAT IS NOT AN
    ABSENCE UNTIL THE INSTRUMENT HAS BEEN SHOWN ABLE TO SPEAK**, and this project has paid for that
    shape before.
  - **WHAT IS THEREFORE PROVED AND WHAT IS NOT.** PROVED: these bytes compile, the reflected
    header parses, every dependent translation unit in `StratPlay` including the test lane's
    agrees with the new signature, and the module links and is on disk. NOT PROVED: **no suite was
    run -- deliberately, it is the test author's and it is owed after this signature lands** -- and
    nothing has been observed at runtime. The playtest observation that produced the defect has no
    clause behind it, which is why the suite was green with it live.
  - **WHAT WAS NOT DONE.** No `Tests/` file, no `Content/` asset, no `Source/StratRules/` and no
    `Data/` was touched, and no re-vendor request is filed -- the rules module was correct
    throughout and `bIsGuidedMarked` is doing exactly what it was designed to do. The stale
    `BP_StratBoardActor` / `BP_StratUnitActor` comment names and `GuidedMarkerZOffset`'s occlusion
    were left alone on the coordinator's instruction; both are known and filed.
  - **DEBT: NOBODY HAS SEEN THE MARKER CLEAR.** The tree links now, and that moves nothing about
    this debt -- the fix is still a flag change no eye has been on.
    **DISCHARGED WHEN** a human plays §2.11.6-B past the window's close with this in the tree and
    reports the marker going out with the ring -- injected input never reaches `UPlayerInput` on
    this project, so no agent can take that observation. OWNED: coordinator, to schedule.
  - **[DISCHARGED 2026-08-24. A HUMAN PLAYED IT AND THE MARKER WENT OUT WITH THE RING. THE DEBT
    TEXT ABOVE IS KEPT DELIBERATELY]** -- it is the record that this fix shipped unobserved, and
    of how long it stayed that way. The player ran the guided opening to completion and reports
    the marker clearing WITH the ring, and additionally that the guidance STRIP disappeared in the
    same frame: all three surfaces dark together, which is §2.11.6's sentence in full rather than
    the half this lane owed.
    **WHAT THE LOG PROVES AND WHAT THE EYE PROVES ARE DIFFERENT, AND ONLY ONE OF THEM IS AN
    INSTRUMENT.** `Saved/Logs/Stratocracy.log` carries `Guided opening complete on turn 3: every
    beat retired.` at `[2026.08.24-15.57.47:040]` and `[2026.08.24-15.58.38:000]`, re-measured
    here rather than restated -- and a THIRD at `[2026.08.24-15.23.54:477]`, also after this
    lane's 10:22 link and the test lane's 10:40 relink, so every one of them ran against a binary
    containing this change. That line is emitted on the ALL-BEATS-RETIRED branch, one of the three
    places `bActive` goes false, so the log proves the sessions REACHED the instant the fourth
    operand acts on. **IT DOES NOT PROVE WHAT WAS ON SCREEN.** The three surfaces going dark
    together is the player's observation and nothing in this tree can corroborate it:
    `IsGuidedMarkerVisible` reports the visible FLAG and not pixels, exactly as its own block says,
    so **THIS DEBT IS DISCHARGED BY A HUMAN AND NOT BY A CLAUSE**, and no clause can be written
    that would discharge it.
    **THIS MAKES NO CLAUSE STRONGER THAN IT WAS.** The three clauses covering the marker remain
    pinned against a test-side instrument rather than against the production writer, because
    breaking the real subject would have needed an out-of-lane edit into this lane's files. That
    limit is `tests.md`'s and is unchanged by anything here; a playtest is not a gate, and a
    discharged debt is not a regression net. **WHAT IS STILL TRUE: nothing headless fails if this
    fourth operand is deleted tomorrow.**
  - **[CORRECTED 2026-08-24, SAME DAY, AFTER A COORDINATOR MEASUREMENT. THE TWO SENTENCES THAT
    CLOSE THE BULLET ABOVE ARE FALSE, AND THE SECOND OF THEM IS THE ONE THIS LANE'S OWN REPORT
    ASKED THE REVIEWER TO LOOK AT.]** They said:
    RETRACTED> "The three clauses covering the marker remain pinned against a test-side
    RETRACTED>  instrument rather than against the production writer, because breaking the real
    RETRACTED>  subject would have needed an out-of-lane edit into this lane's files."
    RETRACTED> "WHAT IS STILL TRUE: nothing headless fails if this fourth operand is deleted
    RETRACTED>  tomorrow."
    **THE CLAUSES READ THE PRODUCTION WRITER, AND THE MEASUREMENT WAS RE-TAKEN IN THIS TREE
    RATHER THAN ACCEPTED ON REPORT.**
    `Stratocracy.StratPlay.T-UI-02.GuidedMarkerClearsWhenGuidanceDeactivates` calls
    `Match.Subsystem->ApplyView(Dark)` -- the REAL `UStratMatchSubsystem`, which is the sole
    caller of `AStratUnitActor::ApplyUnitView` -- and reads the result through the fixture's
    `LitMarkerIds`, whose whole body is `Actor->IsGuidedMarkerVisible()` over the live actors that
    call reconciled. Before asserting anything it pins as FATAL premises that in the dark frame
    `Marked->bIsGuidedMarked && Marked->Side == Dark.ViewingSide` is STILL TRUE, that
    `Guidance.bActive` is the only field that moved, and that the seat and the unit count did not.
    Then it asserts `StillLit.Num() == 0`. **So deleting the fourth operand leaves the two
    match-constant operands the clause has just asserted true, the marker stays lit, and that
    closing `TestEqual` goes RED.** It also lights the marker in frame one as a positive control,
    so it is not passing vacuously. **SOMETHING HEADLESS DOES FAIL. THE RETRACTED SENTENCE STATES
    THE OPPOSITE OF THE TREE.**
    **THE CONFLATION, NAMED, BECAUSE IT IS THE REUSABLE PART.** `strat-test-author` reported that
    its FALSIFIABILITY MUTATION lived in a test-side helper -- it reinstated the pre-fix predicate
    in the INSTRUMENT because breaking the real subject would have meant editing this lane's
    files, which its lane forbids. That is a fact about HOW IT PROVED THE CLAUSES CAN FAIL. This
    lane read it as a fact about WHAT THE CLAUSES READ. **A PROOF OF FALSIFIABILITY THAT ROUTES
    AROUND THE SUBJECT SAYS NOTHING ABOUT WHERE THE SUBJECT IS**, and merging the two understated
    this project's own coverage in the record -- the rarer direction, and no safer for it, because
    a future reader deletes an operand the record told them nothing guards.
    **BOTH SENTENCES ARE RETRACTED AND NOT ONLY THE CONCLUSION.** The premise sat two lines above
    the conclusion and would have been left standing by a fix aimed at the headline alone; this
    project has a recorded entry for exactly that half-done shape and this is not another one.
    **WHAT SURVIVES UNCHANGED, AND IT IS THE PART THAT WAS ACTUALLY MEASURED:** no clause gates
    THE MARKER REACHED THE SCREEN. `IsGuidedMarkerVisible` reports the visible FLAG and not
    pixels, so the pixel question is human-only and this debt really was discharged by a human
    rather than by a clause. **THE OVER-CLAIM WAS EXTENDING THAT FROM PIXELS TO THE OPERAND
    ITSELF.** The flag is gated; the pixels are not; those were never the same claim.

- **2026-08-23, `strat-gameplay-engineer`: THE OVERLAY-COUNT CLAIM IS SWEPT OUT OF THE TREE BY
  CLAIM SHAPE, AND THE SWEEP FOUND ONE THE REVIEWER DID NOT.** Comment-only; no signature, no
  behaviour and no `UPROPERTY` moved. Build green, suite green. `global.md` owns the count.
  - **THE GATING FINDING WAS MINE AND IT WAS A HALF-DONE RETRACTION.** Closing the "exactly
    two overlays" claim in `AStratBoardActor`'s header block, `ShowTargets` and `OverlayMesh`
    left it standing in two more places in the same file, both verified false against the
    tree: the "NOT IN THIS ROUND" bullet's "this class exposes the two overlays it drives"
    and `BoardRoot`'s "the two overlays are constructor subobjects and attach here too".
    `ObjectiveOverlay` is a third `CreateDefaultSubobject` and calls `SetupAttachment(BoardRoot)`
    beside the other two. **A RETRACTION THAT DOES NOT REACH EVERY SITE IS A RETRACTION THAT
    LEAVES THE CLAIM LOAD-BEARING SOMEWHERE**, and this is the second time this project has
    paid for that specific shape.
  - **A THIRD SITE WAS FOUND, AND ONLY THE SHAPE SWEEP COULD HAVE FOUND IT.** `FillOverlay`'s
    declaration read "The shared tail of `ShowReach` and `ShowTargets`, so the two cannot
    drift" -- **an overlay-count claim spelled as a CALLER LIST**, containing neither the word
    "overlays" nor any number a reader would think to grep. `ShowObjective` calls it too. A
    phrase sweep for "two overlays" returns this line zero times.
  - **WHAT WAS SEARCHED FOR AND WHAT CAME BACK, ZEROES INCLUDED**, because a sweep that
    reports only its hits cannot be told from a sweep that only ran one pattern. Over
    `StratBoardActor.h`: `two overlays` 4, `both overlays` 0, `the pair` 0, `two components`
    1, `there are exactly` 1, `second component` 2, `second material` 0, `one mesh, two` 0,
    `the other overlay` 0, `duo` 0, `these two` 0, `two of them` 1, `overlay component` 2,
    `\btwo\b` 15, `\bboth\b` 8, `\bexactly\b` 6, `\bpair\b` 1, `the two` 5. Every hit
    on the three broad shapes was read rather than counted; the survivors are unrelated
    counts (two containers, two copies of a formula, two materials on ONE component, two
    accessors easy to conflate, two readers of a clause, two adjacent hexes, two of three
    callers) and are listed here so the next sweeper does not re-open them.
  - **ONE "two overlays" HIT IS KEPT DELIBERATELY AND IS NAMED SO IT IS NOT MISTAKEN FOR A
    MISS.** `ShowObjective`'s own block says "there was nothing in C++ to bind to -- this
    class declared exactly two overlays". That is PAST TENSE inside a what-gap-this-closes
    narrative and is true of the tree before this change; rewriting it would delete the
    reason the component exists. The other two surviving hits are inside `RETRACTED>` quotes,
    which is what a retraction looks like when it works.
  - **THE CORRECTIONS ARE COUNT-FREE RATHER THAN RE-NUMBERED, WHICH IS THE ACTUAL FIX.**
    "the overlays it drives", "the overlays are constructor subobjects", "`ShowReach`,
    `ShowTargets` AND `ShowObjective`". Re-numbering to three would have rebuilt the same
    trap for whichever GDD section needs a fourth component. **The invariant is stated once**,
    in the header block: ONE MEANING PER COMPONENT. A count is a fact; that is a rule.
  - **ALL THREE NON-GATING SENTENCES WERE TAKEN, AND ONE WAS TAKEN AGAINST THE ARGUMENT FOR
    DECLINING IT.** `UStratMatchSubsystem::GetBoard`'s "the selection machine drives the two
    overlays through it" was TRUE -- the machine owns exactly two highlight sets and does not
    know the ring exists. It was amended anyway, because a reader arriving at it by grepping
    the count generalises it to the board, and an understatement that reads as a total is how
    a true sentence teaches a false thing. `AStratPlayerController::RefreshFromMachine`'s
    declaration and its `.cpp` counterpart were genuinely stale: that path repaints three
    overlays by two routes, this class calling `ShowReach` / `ShowTargets` itself while the
    ring rides the `ApplyView` step. Its step list is accurate for what that class does and
    was left alone; only the count moved.
  - **`StratGuidedOpening.h`'S LANE SENTENCE IS AMENDED AND THE BOUNDARY IS RESTATED WHERE IT
    ACTUALLY FELL.** It said "the ring mesh and the turn-1a marker are `strat-editor-builder`'s
    lane". The ring's COMPONENT and its show/clear are `AStratBoardActor::ShowObjective` /
    `ClearObjective`, and the marker's COMPONENT and its visibility are
    `AStratUnitActor::GuidedMarker` / `ApplyUnitView` -- C++, this lane. What remains the
    content lane's is the ASSET half: meshes, material instances, and their assignment on
    `BP_StratBoardActor` / `BP_StratUnitActor`. **Nothing about `FStratGuidedOpening` changed**
    -- it still draws nothing and names no component; the sentence described a boundary that
    moved underneath it.
  - **BUILD GREEN, AND A COMMENT-ONLY CHANGE WAS NOT ARGUED TO BE SAFE.** Plain `Build.bat`,
    no `-NoHotReloadFromIDE`, editor re-verified absent: `Result: Succeeded`, `REAL_EXIT=0`,
    136.70 s, twenty-five actions -- twenty-two compiles including `Module.StratPlay.gen.cpp`,
    then both links. Five headers changed, so UHT and every dependent TU rebuilt; that is why
    the run is bigger than the diff.
  - **SUITE GREEN AND THE FIGURE HELD, MEASURED RATHER THAN INFERRED.** Zero non-Success,
    `notRun` zero, `reportCreatedOn 2026.08.23-23.48.14` (UTC; 19:48:14 local). **CLAUSE DELTA
    ZERO BY MACRO SET-DIFFERENCE** -- `IMPLEMENT_SIMPLE_AUTOMATION_TEST` over `Source/` reads
    identically before and after, correct because this agent writes none. **NO FIGURE IS
    WRITTEN HERE**; `global.md` owns it and it went to the coordinator. The clause that was
    red by design in the previous pass is green again, which is the test author's update
    landing and not anything this pass did.
  - **`Saved/SaveGames/` ENUMERATED ZERO BEFORE AND ZERO AFTER**, directory mtime moving
    19:09:36 -> 19:48:13, the run's own minute.
  - **WHAT WAS NOT DONE.** `global.md`, `content.md`, `decisions.md` and `tests.md` were not
    touched -- the record finding is the coordinator's and the steward's. No clause was
    written or edited. No `Content/` asset was touched, so **nothing has been seen on a
    screen** and this pass changes nothing about that.

- **2026-08-23, `strat-gameplay-engineer`: THE TURN-1a MARKER IS FILTERED TO THE VIEWING SIDE,
  AND A FALSE SENTENCE THIS LANE WROTE IS RETRACTED WHERE IT WAS BORN.** Build green; one
  clause red ON PURPOSE and named below. `global.md` owns the count and the verdict.
  - **THE PREMISE WAS RE-VERIFIED IN THIS TREE BEFORE ANY CODE MOVED.**
    `Data/ferrum_crossing.json` authors `guidedOpening` for BOTH seats -- side 0
    `infantry [1, 5]`, side 1 `infantry [9, 3]` -- so two units carry `bIsGuidedMarked` at
    once and the unfiltered pass-through marked the ENEMY seat's Infantry on the player's
    screen. **THE USER RULED IT OUT:** a marker that says "select this" pointing at a unit
    the player cannot select is confusing. That reasoning is carried in `GuidedMarker`'s own
    block, not only here.
  - **THE FILTER LANDED IN `AStratUnitActor::ApplyUnitView`, WHICH GREW A THIRD PARAMETER,
    AND THE VIEWING SIDE COMES OFF THE MODEL RATHER THAN OFF THE SUBSYSTEM.** The predicate
    is `View.bIsGuidedMarked && View.Side == ViewingSide` -- two published fields ANDed, no
    derivation, no lookup, no hex comparison, and `bIsGuidedMarked` still read off
    `placement` by the rules module and never recomputed here.
    `UStratMatchSubsystem::ApplyView` passes `Model.ViewingSide`.
  - **`UStratMatchSubsystem::GetViewingSide` EXISTS, WAS THE OTHER CANDIDATE, AND WAS
    REFUSED -- BY A SENTENCE ALREADY IN THE TREE.** `FStratViewModel::ViewingSide`'s own
    declaration block says it lives on the model "rather than in the actor that draws it so
    that 'what should be on screen' is a function of this value alone -- a viewing side held
    beside the model is a second input, and T-INT-05 would then be about two things". The
    subsystem's member is equal to the model's on every path that reaches `ApplyView` TODAY;
    reading it from the actor would make the marker a function of the model PLUS a member,
    and a stale member would put the mark on the wrong seat with a green build.
  - **THE THIRD PARAMETER IS NOT DEFAULTED, DELIBERATELY.** A default would have let a future
    caller get an unfiltered marker silently; making it a compile error is the cheaper
    failure. It cost nothing to require: `ApplyUnitView` has exactly ONE caller in the whole
    tree -- measured, `grep` for `ApplyUnitView(` outside its own declaration returns one
    line, in `UStratMatchSubsystem::ApplyView` -- and no file under `Tests/` calls it
    directly, so no lane but this one had to move.
  - **EXACTLY ONE CLAUSE WENT RED AND IT IS THE ONE THAT WAS SUPPOSED TO.**
    `Stratocracy.StratPlay.T-UI-02.GuidedMarkerFollowsTheMarkedBitAndNotTheHex`, with two
    assertions failing, both about unit 7 (side 1) and neither about unit 3 (side 0, the
    viewing seat): *"before the move: unit 7's turn-1a marker equals ITS OWN published
    `bIsGuidedMarked` -- a pass-through, not a side policy (the guided seat's marked unit is
    3; 2 units carry the bit): The two values are not equal."* and *"after the move: unit 7's
    turn-1a marker still equals ITS OWN published `bIsGuidedMarked`: The two values are not
    equal."* Its author wrote it against the unfiltered pass-through knowing a filter would
    turn it red. **NOT EDITED -- `Tests/` is not this lane** -- and NOT worked around. NO
    OTHER CLAUSE MOVED, which is the finding that matters: 197 Success, 1 Fail, 0 notRun, so
    the failure set is exactly the predicted singleton and not a superset.
  - **A FALSE SENTENCE THIS LANE WROTE IS RETRACTED AT ITS SOURCE, AND IT HAD ALREADY
    PROPAGATED TWO HOPS.** `AStratUnitActor::IsGuidedMarkerVisible`'s block claimed it
    answers "FALSE WITH NO MARKER MESH ASSIGNED" and concluded that a clause must "assign
    `GuidedMarkerMesh` on the spawned actor first". `USceneComponent::IsVisible` consults
    `bHiddenInGame`, the visible flag and the cached level collection -- NOT the static mesh
    -- so it answers TRUE for a marked unit whose marker draws nothing. Measured by
    `strat-test-author` in the suite; never measured by the author who wrote it. It reached
    a dispatch brief and from there a test author's instructions, which is why this is
    recorded as a propagation and not as a stray comment: **an unmeasured sentence in a
    declaration block is an instruction to everyone downstream.**
  - **THE CONSEQUENCE IS THE HONEST LIMIT OF THIS SEAM AND IT IS NOW IN THE HEADER.**
    `IsGuidedMarkerVisible` reports a FLAG, NOT PIXELS. It cannot say whether
    `GuidedMarkerMesh` was ever assigned and it cannot say a marker reached the screen, so
    **"the marker is actually on screen" has NO headless gate at all** and none is available
    from this class. The `BeginPlay` log line is the only place "unconfigured" is
    distinguishable from "not marked", and that path is still reached by no clause.
  - **FOUR COMMENTS THIS CHANGE FALSIFIED WERE AMENDED IN THE SAME CHANGE.**
    `GuidedMarker`'s "PASS-THROUGH OF ONE PUBLISHED FIELD" is retracted in place and now says
    two, with the ruling attached; `IsGuidedMarkerVisible`'s false sentence and the "assign
    the mesh first" instruction that rested on it are retracted together;
    `GuidedMarkerMesh`'s "`IsGuidedMarkerVisible` answers false" clause is corrected; and
    `UStratMatchSubsystem::ApplyView`'s "this function knowing nothing about either" is
    amended, because it now supplies an argument -- it still decides nothing.
  - **BUILD GREEN.** Plain `Build.bat`, no `-NoHotReloadFromIDE`, editor re-verified absent
    by `tasklist` with `explorer.exe` PID 13508 as the control: `Result: Succeeded`,
    `REAL_EXIT=0`, 94.50 s, seventeen actions including `Module.StratPlay.gen.cpp`. Zero
    warnings, zero errors.
  - **SUITE RAN IN THIS TREE, HEADLESS, EDITOR CLOSED.** `reportCreatedOn
    2026.08.23-22.51.00` (UTC; 18:51:00 local), `notRun` zero, one Fail as named above.
    **CLAUSE DELTA ZERO BY MACRO SET-DIFFERENCE** -- `IMPLEMENT_SIMPLE_AUTOMATION_TEST` over
    `Source/` reads the same before and after, correct because this agent writes none. **NO
    FIGURE IS WRITTEN HERE**; `global.md` owns it and the figure went to the coordinator.
  - **THE MARKER WRITER IS OBSERVED EXECUTING AND ITS `BeginPlay` STILL IS NOT.**
    `ApplyUnitView` ran 1757 times in this run, counted off the pre-existing unconditional
    `No mesh assigned for unit definition` line. `Saved/SaveGames/` enumerated zero before
    and zero after, directory mtime moving 18:37:24 -> 18:51:00, the run's own minute.
  - **WHAT WAS NOT DONE.** The red clause was not edited and no clause was written --
    `Tests/` is not this lane; the update is owed to `strat-test-author` and the symbol names
    it needs are `AStratUnitActor::ApplyUnitView` (third parameter `ViewingSide`),
    `FStratViewModel::ViewingSide`, and `AStratUnitActor::IsGuidedMarkerVisible`. No
    `Content/` asset was touched, so **nothing draws yet and nobody has seen a marker on a
    screen** -- the filter is proved by a flag and by one clause going red in the predicted
    place, not by pixels.

- **2026-08-23, `strat-gameplay-engineer`: THE RING AND THE MARKER NOW HAVE C++ SEAMS. THE
  CONTENT LANE WAS BLOCKED BY AN ABSENCE IN THIS LANE AND IT IS NOT ANY MORE.** Build green,
  suite green in this tree; `global.md` owns the count and the verdict and neither is stated
  here.
  - **THE BLOCK WAS REAL AND IT WAS THIS LANE'S.** §2.11.6-B beat 1a says "Select the marked
    Infantry" and beat 2 says "the ringed Factory"; nothing on screen was marked or ringed, and
    the user learned the objective hex by reading the log. `strat-editor-builder` could not fix
    it from `Content/` because there was nothing declared to bind to: `AStratBoardActor` had
    exactly two overlay components and `AStratUnitActor` had one component and one `UFUNCTION`.
    Verified in this tree before building anything.
  - **RING: A THIRD OVERLAY, AND REUSING `TargetOverlay` WAS REFUSED FOR TWO REASONS, NOT ONE.**
    `AStratBoardActor::ObjectiveOverlay`, with `ShowObjective` / `ClearObjective` /
    `GetObjectiveOverlayCount` and an `EditDefaultsOnly` `ObjectiveMaterial` left UNSET. The
    semantic reason is that `TargetOverlay` is §2.6's ATTACK targets. The sharper one is
    clause-visible and would have failed loudly: `T-UI-02.AttackIsClosedForTheMarkedInfantry`
    asserts the attack overlay is dark during beat 1a, and beat 1a and the ring are on screen
    together - a ring drawn there would have lit the very component that clause says must be
    empty, so this was never a style question.
  - **`ShowObjective` TAKES ONE HEX AND NOT A `TArray`, AND THE SIGNATURE IS THE GUARD.**
    §4.7's `guidedOpening.objective` is one authored hex per seat. A set-shaped parameter would
    invite a caller to pass "every objective", which is the "nearest objective" heuristic
    §2.11.6 forbids by name arriving through a parameter list instead of through a function.
    Internally it goes through the same private `FillOverlay` the other two use, so the ring
    cannot drift from the highlights in how it clears or how it is Z-offset.
  - **[AMENDED 2026-08-23, LATER THE SAME DAY, BY THIS ENTRY'S OWN AUTHOR: THE MARKER IS NOW
    FILTERED TO THE VIEWING SIDE, ON A USER RULING, SO "ONE BOOL" AND "FROM NOTHING ELSE"
    BELOW ARE SUPERSEDED BY THE ENTRY ABOVE.** Kept rather than rewritten: the unfiltered
    pass-through is what a clause was written against, and the next reader needs to see that
    the second operand arrived by ruling rather than that it was always there.]** MARKER: A
    COMPONENT ON `AStratUnitActor`, DRIVEN OFF THE VIEW THAT ACTOR WAS ALREADY HANDED. `GuidedMarker`, a `VisibleAnywhere`
    component; plus THREE `EditDefaultsOnly` properties this change added to
    `AStratUnitActor` -- `GuidedMarkerMesh`, `GuidedMarkerMaterial` and
    `GuidedMarkerZOffset` -- of which TWO ship unset and `GuidedMarkerZOffset` carries a real
    default of `150.0f`; and `AStratUnitActor::IsGuidedMarkerVisible` as the clause seam.
    **THE SCOPE OF THAT COUNT IS THIS ACTOR AND THIS CHANGE, AND IT IS STATED BECAUSE STATING
    IT IS THE FIX.** `AStratUnitActor` carries SEVEN `EditDefaultsOnly` properties in total --
    `MeshByDefId`, `FallbackMesh`, `SideMaterials` and `BodyZOffset` predate this change -- so
    "the `EditDefaultsOnly` properties on this actor" would be a third wrong number. The
    feature's third unset asset reference is `AStratBoardActor::ObjectiveMaterial`, which is on
    the BOARD actor and is covered by the RING bullet above; it does not belong in this count.
    **[CORRECTED 2026-08-23, TWICE, AND BOTH ARE SHOWN BECAUSE THE SEQUENCE IS THE LESSON.**
    FIRST it read "all `EditDefaultsOnly` and all UNSET" -- false, because `GuidedMarkerZOffset`
    is `= 150.0f`. THEN the correction itself read "all four `EditDefaultsOnly`, of which THREE
    are unset" -- **false in two independent ways in one sentence**, and it CONTRADICTED THE STAMP
    IT WAS PART OF, which said in the same edit that `GuidedMarker` is `VisibleAnywhere` and not
    in the set. (That stamp's wording is superseded by this one; do not look for it below, it is
    not there. The true statement was written in the stamp and the false one in the sentence the
    stamp was correcting, four lines apart, by one author in one pass.) The denominator wrongly counted `GuidedMarker`, a `CreateDefaultSubobject`
    component that can never be unset; and the numerator, THREE, only balanced by importing
    `AStratBoardActor::ObjectiveMaterial` -- a property on a DIFFERENT ACTOR -- across a scope
    boundary into a bullet whose subject is one actor.
    **THE MEASUREMENT, RE-DERIVED FROM THE HEADERS FOR THIS BULLET'S ACTOR ONLY AND REUSING
    NEITHER PREVIOUS NUMBER:** `git diff` on `Source/StratPlay/StratUnitActor.h` adds exactly
    four `UPROPERTY`s -- `UPROPERTY(VisibleAnywhere)` on
    `TObjectPtr<UStaticMeshComponent> GuidedMarker`, and `UPROPERTY(EditDefaultsOnly)` on
    `TObjectPtr<UStaticMesh> GuidedMarkerMesh`, `TObjectPtr<UMaterialInterface>
    GuidedMarkerMaterial` and `float GuidedMarkerZOffset = 150.0f`. Three `EditDefaultsOnly`;
    two with no initialiser.
    **WHAT THE THREE ROUNDS HAVE IN COMMON, AND IT IS NOT CARELESSNESS WITH ARITHMETIC.** Round
    one named a set that was not the set measured. Round two counted across a scope the sentence
    did not own. NEITHER IS A COUNTING ERROR -- both are SCOPE errors that produced a wrong
    count as a symptom, which is why re-checking the number would not have caught either. The
    procedure that does catch them is the one the coordinator imposed on round three: **state
    the scope of the sentence in the sentence, then count only inside it.** The previous
    correction closed with "a recorded lesson is not a guard; only a measurement at the moment
    of writing is" -- and then got it wrong again while writing that very sentence, which is the
    second data point for its own claim and the reason it is left standing rather than
    softened.]**
    `AStratUnitActor::ApplyUnitView` sets visibility from `FStratUnitView::bIsGuidedMarked` and
    from nothing else - no hex comparison against `guidedOpening.infantry`, which
    `StratGuidedOpening.h` records as the trap that would unmark the unit at the exact moment
    beat 1a needs it marked, because that beat's entire content is that the unit MOVES.
  - **SET IN BOTH DIRECTIONS ON EVERY CALL, WHICH IS THE SAME RULE `PublishLocks` ALREADY
    CARRIES.** A writer that only ever SHOWS is a writer whose hide can be missed on one path,
    and for this bit that means a permanent marker on a unit the §2.11.6 window closed on. Same
    for the ring: the `ApplyView` call site has an `else` that clears, so "the ring and the
    marker clear in the same frame as the strip" - `FStratGuidedOpening::SkipGuidance`'s wording
    - is structural. The clear and the strip's push are a few lines apart in one function, on
    one value, and neither surface has a second driver.
  - **ONE CALL SITE FOR THE RING AND ZERO FOR THE MARKER, AND THE ASYMMETRY IS THE POINT.**
    `UStratMatchSubsystem::ApplyView` drives the ring because the board is not per-unit. The
    marker needs no call site there at all: `ApplyUnitView` is already called once per unit per
    refresh from that same function, so the subsystem knows nothing about the marker and the two
    visuals still ride one refresh. A second call site would have been a second driver.
  - **NEITHER VISUAL PERFORMS ARITHMETIC OR A DERIVATION.** The ring reads
    `FStratGuidanceView::bActive` and `bHasObjectiveRing` as a visibility condition and copies
    `ObjectiveHex`; the marker copies one bool (TWO as of the ruling in the entry above). `bHasObjectiveRing` is read because
    `FStratGuidanceView` declares that it alone qualifies `ObjectiveHex` - `FIntPoint(0, 0)` is a
    real hex and cannot signal its own absence, the trap this project has now paid for three
    times. **This is not T-UI-03's forbidden arithmetic**: no number is drawn and no widget
    renders the conjunction.
  - **NO `/Game/` LITERAL AND NO ASSET WAS TOUCHED.** THIS BULLET'S SCOPE IS THE WHOLE
    FEATURE -- both actors -- and it is named here so that it cannot be read as disagreeing
    with the MARKER bullet above, whose scope is `AStratUnitActor` alone and which therefore
    correctly says TWO. THREE of the four new configuration properties this change added
    across `AStratBoardActor` and `AStratUnitActor` ship unset -- `ObjectiveMaterial`,
    `GuidedMarkerMesh`, `GuidedMarkerMaterial`, which is every one that decides whether
    anything DRAWS; the meshes, the
    material instances and their assignment on `BP_StratBoardActor` / `BP_StratUnitActor` are
    the CONTENT lane's and are what actually makes either visual appear.
    **[CORRECTED 2026-08-23. THIS READ "All four new configuration properties ship UNSET" AND
    IT IS THREE.** `GuidedMarkerZOffset` is `= 150.0f` and has nothing to do with whether
    anything draws -- it is how high the marker floats once it does.
    **THE SUBJECT WAS WRONG, NOT THE SCOPE, WHICH IS WHY IT SURVIVED:** narrowing "all" to
    "most" would not have caught it, because the sentence named a DIFFERENT SET than the one
    that was measured. A quantifier check passes cleanly over a mis-named subject.
    **THIS FILE ALREADY CARRIED THE WARNING AND THE WARNING DID NOT STOP IT.** An entry
    further down says in as many words that "this project has already paid once for treating a
    real default as an unset marker" -- about `FIntPoint(0, 0)` and `ProductionMenuHex` -- and
    the same author, in the same file, then wrote a real `float` default into a set defined by
    being unset. A recorded lesson is not a guard; only a measurement taken at the moment of
    writing is.
    **THE FIGURE CAME IN FROM A DISPATCH AND WAS NOT INVENTED HERE, AND THAT IS THE LESS
    USEFUL HALF OF THE FINDING** -- it was reproduced here without being checked against the
    header this same agent had written minutes earlier, which no provenance excuses.]**
    **UNTIL THAT LANDS, NOTHING DRAWS** - the seam is necessary and is not sufficient, and this
    entry does not claim the section is unblocked on screen, only that the binding surface
    exists.
  - **THE TWO UNSET CASES ARE REPORTED DIFFERENTLY ON PURPOSE.** A missing `GuidedMarkerMesh` is
    logged once per actor at `BeginPlay`, because an unconfigured marker and an unmarked unit are
    indistinguishable on screen and have entirely different fixes. A missing `ObjectiveMaterial`
    is SILENT, because a ring with no material still draws in the mesh's own material - a
    visibly-wrong ring, which needs no log line to find. Only a missing `OverlayMesh`, which
    draws nothing at all, keeps the loud line.
  - **THE BOARD ACTOR'S HEADER SAID "THERE ARE EXACTLY TWO OF THEM" AND THAT IS RETRACTED IN
    PLACE.** The count was never the invariant; ONE MEANING PER COMPONENT was, and the retracted
    wording tied a structural rule to an arithmetic fact a new GDD section was always going to
    move. `GetTargetOverlayCount`'s block is stamped too: its "the day a third overlay lands"
    hypothetical is no longer hypothetical, so reading these counts BY NAME is now load-bearing
    rather than merely tidier. `AStratUnitActor`'s "NOT IN THIS ROUND" bullet about the
    DONE/locked visual is amended to draw the contrast rather than left to look inconsistent:
    `bIsGuidedMarked` names one unit for the whole match and its directive is unreadable without
    it, where `bDone` and `bLockedThisTurn` change several times a turn and §2.11 has not said
    what they look like. That bullet still stands for those two bits.
  - **BUILD GREEN.** `Build.bat` with the documented arguments and NO `-NoHotReloadFromIDE`,
    editor confirmed absent: `Result: Succeeded`, `REAL_EXIT=0`, 114.94 s, nineteen actions,
    including `Module.StratPlay.gen.cpp` - so UHT parsed the new `UFUNCTION`s and `UPROPERTY`s -
    and all four changed sources compiled as their own translation units per
    `[Adaptive Build] Excluded from StratPlay unity file`. Zero warnings, zero errors.
  - **SUITE GREEN IN THIS TREE.** Zero non-Success, `notRun` zero.
    `reportCreatedOn 2026.08.23-22.10.25` (UTC; 18:10:25 local). Clause delta ZERO by macro
    set-difference over `Source/`, unchanged across both passes. `Saved/SaveGames/` enumerated
    zero before and zero after, directory mtime moving 18:03:09 -> 18:10:24, the run's own
    minute. **NO FIGURE IS WRITTEN HERE**; `global.md` owns it.
  - **THE MARKER'S PER-REFRESH WRITER IS OBSERVED EXECUTING; NEITHER `BeginPlay` PATH IS.**
    `ApplyUnitView` ran 1697 times in this run - counted off the pre-existing
    `No mesh assigned for unit definition` line, which is unconditional on that path - so the
    `SetVisibility(View.bIsGuidedMarked)` line executed 1697 times without incident. But
    `Saved/Logs/Stratocracy.log` carries ZERO `GuidedMarkerMesh set` lines AND zero
    `has no OverlayMesh set` lines, and the second is PRE-EXISTING board behaviour. **So no
    clause in this suite reaches EITHER actor's `BeginPlay`**, the mesh/material assignment path
    is unobserved, and it will first execute in PIE. Stated because a green suite over this
    change proves the writer and not the configuration.
  - **WHAT WAS NOT DONE AND CANNOT BE CLAIMED.** No clause was written - `Tests/` is not this
    lane, and four are named in the handoff. No `Content/` asset was touched. **Nobody has SEEN
    a ring or a marker**: injected input never reaches `UPlayerInput` on this project, so that
    needs the content defaults plus a human at the keyboard, and it is the one thing that would
    close §2.11.6-B's visual half. OWNED: coordinator, to schedule.

- **2026-08-23, `strat-gameplay-engineer`: SEC 2.11.6-B'S BEAT 2 COULD NOT RETIRE ON THE SHIPPED
  SCENARIO. THE CODE IS WRITTEN AND NOTHING HAS COMPILED IT.** No suite count and no verdict is
  stated here; `global.md` owns both.
  - **THE DISPATCH BRIEF'S MECHANISM IS CONFIRMED, AND IT WAS RE-DERIVED FROM THE VENDORED
    SOURCES RATHER THAN TAKEN ON REPORT.** Four readings, each independently checkable:
    `strat::EconomyState::captureTurns` is declared `= 1` and NOTHING assigns it — the shipped
    `Data/ferrum_crossing.json` carries no `captureTurns` key and it is named in no loader, so
    the shipped match runs at 1. `strat::captureTick` pushes a `CaptureProgress` with
    `turnsHeld = 1`, tests `turnsHeld >= captureTurns` in the same iteration, flips
    `Objective::owner` and calls `strat::clearProgress`, which erases the entry — one call.
    That `push_back` is the ONLY write into `EconomyState::captures` anywhere in the vendored
    tree, so no other path can leave one standing. The projection's `progressForUnit` therefore
    finds nothing and returns 0, so `UiUnitView::captureProgress` and
    `FStratUnitView::CaptureProgress` read 0 in every snapshot that can ever be taken, and
    `FStratGuidedOpening::HasCapturePipLanded` is unreachable at `captureTurns = 1`.
  - **[RETRACTED 2026-08-23, SAME DAY, BY THIS ENTRY'S OWN AUTHOR AFTER A COORDINATOR
    CORRECTION. THE TWO HEXES ARE THE SAME HEX AND THERE WAS NEVER A DISCREPANCY.** The bullet
    below is KEPT rather than deleted because the reading was reasonable and the next person to
    diff a scenario file against a log needs to see why it was wrong.] It said:
    RETRACTED> "ONE FIGURE IN THE BRIEF IS REFUTED BY THE TREE, AND IT DOES NOT CHANGE THE
    RETRACTED>  DIAGNOSIS. The brief names the ringed objective as `(2, 7)`.
    RETRACTED>  `Data/ferrum_crossing.json`'s `guidedOpening` block authors `objective` as
    RETRACTED>  `[5, 7]` for side 0 and `[6, 2]` for side 1; `(2, 7)` is a capturable hex in
    RETRACTED>  the `ownership` block (`owner: -1`) and is NOT either seat's ring. Either the
    RETRACTED>  session captured a different factory than the one it was directed to — in which
    RETRACTED>  case beat 2 correctly did not retire, and the 2026-08-21 ruling was doing its
    RETRACTED>  job — or the hex was mis-transcribed. Nobody re-ran the session, so this is NOT
    RETRACTED>  settled."
  - **WHY THAT WAS WRONG: A SCENARIO AUTHORS ODD-R OFFSET AND THE RUNTIME SPEAKS AXIAL.** Two
    coordinate systems, one bracket notation, and nothing in the JSON says which it is.
    `Source/StratRules/Hex.h` states the conversion in its own comment — "odd-r offset <-> axial
    (§4.7): q = col - (row - (row & 1)) / 2, r = row" — and `Source/StratRules/Save.h` says
    "PARSED STATE HOLDS AXIAL ONLY. Hexes are authored odd-r `[col, row]`, converted at" parse.
    So authored `[5, 7]` is col 5, row 7, and q = 5 - (7 - (7 & 1)) / 2 = 5 - 3 = 2, r = 7 —
    **axial (2, 7)**, exactly what the log printed in two separate sessions. Re-derived here
    against those two headers rather than taken on report.
  - **WHAT THAT PUTS BACK, AND IT ALL RUNS THE FIX'S WAY.** The user stood on the RIGHT ringed
    objective; beat 2 genuinely failed to retire on the hex it was supposed to retire on; the
    observed symptom IS evidence of the defect and not a confounder; and
    `T-SCN-07.GuidedOpeningHexesMatchesTheScenarioFile` is green because it is CORRECT, not
    because it is weak — it compares like with like across the same conversion. Nothing about
    the fix or its reasoning changes; only the confidence it rests on, which goes UP.
  - **THE DURABLE LESSON, AND IT IS NOT ABOUT THIS HEX.** A bracketed integer pair in
    `Data/ferrum_crossing.json` and a bracketed integer pair in a `LogStratPlay` line are
    DIFFERENT COORDINATE SYSTEMS that print identically, and a straight comparison between them
    manufactures a discrepancy for roughly every hex with an odd row. Convert before comparing,
    or compare through `FStratBridge::GuidedOpeningHexes`, which has already converted.
  - **THE FIX IS A SECOND OBSERVABLE OR'd ONTO BEAT 2, AND THE SUBJECT IS UNCHANGED.**
    `FStratGuidedOpening::IsRingedObjectiveHeldByGuidedSide` is new: it reads
    `FStratHexView::Owner` at the SAME hex `HasCapturePipLanded` reads —
    `guidedOpening.objective` through `FStratBridge::GuidedOpeningHexes`. The 2026-08-21 user
    ruling is untouched, no "nearest objective" heuristic is introduced, no hex is derived, and
    there is no arithmetic: one equality against a field the rules module published.
    `Stratocracy.StratBridge.T-SCN-07.GuidedOpeningHexesMatchesTheScenarioFile` is load-bearing
    for the new arm exactly as it already was for the old one.
  - **THE PIP ARM IS KEPT RATHER THAN REPLACED, AND THAT WAS A CALL.** Deleting it would drop
    §2.11.6's own named trigger, and at `captureTurns >= 2` the pip is the EARLIER of the two —
    the arrival receipt, not the deed. Keeping both also means every existing clause in
    `Source/StratPlay/Tests/StratGuidedOpeningClauses.cpp` that plants `CaptureProgress` stays
    green, so this change is additive to the suite rather than a rewrite of another lane's file.
  - **`FStratHexView` AND NOT `FStratFactoryView`.** `guidedOpening.objective` is an
    `strat::Objective` and is not obliged to sit on Factory terrain; §2.11.6 says "ringed
    Factory" because that is what Ferrum Crossing authored. `FStratHexView::Owner` mirrors
    `UiHexView::owner` for every capturable hex, so the arm survives a scenario whose ring is a
    Town. A non-capturable hex reads `INDEX_NONE` (`== strat::OWNER_NEUTRAL`), which can never
    equal a side, so a mis-authored objective fails CLOSED.
  - **THE ARM IS A STATE AND NOT AN EVENT, DELIBERATELY.** A scenario seeding the ring already
    held by the guided seat retires beat 2 on the first observation. That is correct, not
    lenient: `strat::captureTick` short-circuits on `o.owner == side`, so no pip and no flip
    could ever follow there, and rule 1 would re-issue an impossible instruction forever — the
    same defect. Measured on the shipped data: both seats' objective hexes appear in `ownership`
    with `owner: -1`, so nothing retires early on Ferrum Crossing. The alternative shape — latch
    the ring's owner at the first `Observe` and retire on a CHANGE — was rejected for adding a
    second piece of remembered state beside `SeenFriendlyUnitIds` to make the machine WORSE in
    the only case the two differ on.
  - **THE RETIREMENT LOG NOW NAMES WHICH ARM FIRED** (`pip=`, `held=`), because the two mean
    different things to a reader of a session log and a single line would let this fix look like
    it worked for the reason nobody checked. The `Guided beat 2 retired on turn %d:` prefix is
    preserved. No test captures that string — the one existing reference is a `TestFalse` on a
    bool, not a log filter.
  - **TWO COMMENTS THIS CHANGE FALSIFIED WERE AMENDED IN THE SAME CHANGE.**
    `HasCapturePipLanded`'s block said "the ownership flip a turn later is a different event
    this beat deliberately does not wait for" — retracted in place, because on the shipped
    scenario there is no "a turn later": the flip and the pip's erasure are the same call. The
    header's four-retirement-triggers list and its "every fact this machine branches on" list
    both carry the new observable and the measurement behind it.
  - **[SUPERSEDED 2026-08-23, LATER THE SAME DAY, BY THIS ENTRY'S OWN AUTHOR. THE EDITOR
    CLOSED, THE TREE BUILT, AND THE SUITE RAN IN IT.** The bullet below reported a tree with no
    compiler output at all, and that was true of the tree it described. It is KEPT rather than
    deleted because its finding is durable and is a DIFFERENT finding from the `LNK1104` one
    this record already carries: the Live Coding mutex aborts ahead of the action graph, so a
    mutex block yields ZERO compile actions and zero diagnostics, where the DLL write lock
    yields a full compile and then a link failure. A reader who has only ever seen the second
    would misread the first as a code fault. **Do not read the bullet below as a live
    instruction to rebuild.**]**
  - **THE BUILD IS GREEN UNDER THE PLAIN DOCUMENTED COMMAND, WITH NO FLAG, AND THE EDITOR'S
    ABSENCE WAS RE-VERIFIED HERE RATHER THAN TAKEN ON REPORT.** `tasklist` returns `INFO: No
    tasks are running which match the specified criteria.` for `UnrealEditor.exe`, with
    `explorer.exe` PID 13508 listed in the same round as the control that shows the instrument
    can speak. `Build.bat` with the documented arguments and NO `-NoHotReloadFromIDE`:
    `Result: Succeeded`, `REAL_EXIT=0`, 56.29 s, fifteen actions. `[7/15] Compile [x64]
    StratGuidedOpening.cpp` is the first compile these bytes have ever had, and
    `[Adaptive Build] Excluded from StratPlay unity file: StratGuidedOpening.cpp` confirms the
    file was compiled as its own translation unit rather than folded into a cached unity blob.
    Zero warnings, zero errors, both links completed.
  - **THE SUITE RAN IN THIS TREE, HEADLESS, EDITOR CLOSED, AND IT IS GREEN.** Zero non-Success
    and `notRun` zero, every entry `Success`. `reportCreatedOn 2026.08.23-22.03.10` — UTC, which
    is 18:03:10 local; do not "correct" either stamp to match the other. **NO FIGURE IS WRITTEN
    HERE AND NONE MAY BE:** `global.md` owns the count, and the figure went to the coordinator.
    **THE CLAUSE DELTA IS ZERO AND IT WAS MEASURED BY MACRO SET-DIFFERENCE, NOT BY NAME** — the
    `IMPLEMENT_SIMPLE_AUTOMATION_TEST` count over `Source/` is identical before and after, which
    is correct: this agent writes none.
  - **THE EXISTING PIP-DRIVEN CLAUSES STAYED GREEN, WHICH IS THE SPECIFIC THING THE OR ARM
    RISKED.** Four `Guided beat 2 retired` lines in `Saved/Logs/Stratocracy.log` from this run,
    every one reading `(pip=true, held=false)` — so the fixtures that plant `CaptureProgress`
    still retire beat 2 through the OLD arm and the new arm did not quietly take over. The new
    per-arm logging is what made that checkable at all; a single line would have hidden it.
  - **THE ODD-R CORRECTION IS CONFIRMED BY THE RUNTIME AND NOT ONLY BY ARITHMETIC.** Those same
    four lines print the ringed objective as `(2, 7)` — the machine's `Objective`, taken from
    `FStratBridge::GuidedOpeningHexes` off authored `[5, 7]`. The conversion is therefore
    observed, not merely computed, and the retracted discrepancy bullet above is settled.
  - **THE SAVE-SLOT CONTROL IS DISCHARGED ON THIS RUN AND NOT BORROWED.** `Saved/SaveGames/`
    enumerated ZERO entries before and ZERO after, and the directory mtime MOVED,
    17:26:41 -> 18:03:09, the run's own minute.
  - **THE BUILD WAS ATTEMPTED ONCE AND WAS BLOCKED BEFORE ANY COMPILE ACTION RAN.** `Build.bat`
    with the documented arguments and NO `-NoHotReloadFromIDE`, with `UnrealEditor.exe` open by
    prior arrangement: `Unable to build while Live Coding is active. Exit the editor and game,
    or press Ctrl+Alt+F11 if iterating on code in the editor or game`, then
    `Result: Failed (OtherCompilationError)`, 2.06 s, exit 6. **This is the Live Coding MUTEX
    and not the `LNK1104` DLL write lock** — it aborts ahead of the action graph, so ZERO
    compile actions ran and **there is no compiler output at all for these bytes**. Nothing
    below or above may be read as if the change had compiled. Discharged by the coordinator
    running the documented command with the editor closed.
  - **WHAT WAS NOT DONE.** No clause was written — `Tests/` is not this lane, and the clause
    this needs is named in the handoff. No `Content/` asset was touched; the editor-builder was
    live in that directory during this pass. `Source/StratRules/` and `Data/` were READ and not
    written, and no upstream re-vendor request is filed, because the fix needed neither — that
    is the reason option 3 (`captureTurns`) was not taken even though it would also have worked.

- **2026-08-23, `strat-gameplay-engineer`: SEC 2.8'S END-OF-MATCH TRANSITION IS BUILT. THE CODE
  COMPILES AND THE BUILD IS NOT GREEN, AND THOSE ARE TWO DIFFERENT SENTENCES.** Written against
  `global.md`'s topmost `## NEXT` entry ("A MATCH THAT ENDS DOES NOT END THE GAME"). No suite
  count and no verdict is stated here; neither could be measured this pass and `global.md` owns
  both anyway.
  - **THE UNVERIFIED LEAD IN THE BRIEF IS CONFIRMED, and it is confirmed by symbol rather than by
    line number.** `UStratMatchSubsystem::IsAiTurnDue` and `UStratMatchSubsystem::RunAiTurnsNow`
    are the two sites that read `FStratMatchView::bHasResult` in `StratPlay` outside `Tests/`, and
    both gate whether an AI turn should START. Neither gates the human, and neither survives the
    flag going down MID-`RunTurn`. So the subsystem could always SEE the result; it had nowhere to
    put the answer.
  - **ONE CORRECTION TO THE BRIEF'S ACCOUNT OF THE FIVE GREP HITS.** The brief describes the fifth
    as "a comment saying the thing does not exist", separately from the `IStratAiTurnPort` design
    note. Re-run in this tree, they are the SAME hit: the only non-`bRecordCompletionOnMatchEnd`
    match is `IStratAiTurnPort`'s own "no `IsMatchOver()`" sentence in
    `Source/StratPlay/StratAiTurnRunner.h`. Four flag hits plus that one is five. The conclusion is
    unaffected; the count was never five independent facts.
  - **WHAT THE FIX IS, IN THREE PIECES, CITED BY SYMBOL.**
    - `StratMatchIsConcluded` and `StratMatchAcceptsPlayerCommands` — new `STRATPLAY_API` free
      functions declared at file scope in `StratMatchSubsystem.h`, pure predicates over an
      `FStratViewModel`. `StratMatchConcludedRefusalText` is the single spelling of the refusal
      sentence, a function rather than a literal at three sites for the reason the `STRAT-AI`
      format string already taught this project.
    - `UStratMatchSubsystem::ConcludeMatchIfEnded` — the one-shot transition, called from
      `ApplyView` where `NoteMatchResultIfEnded` used to be called from directly. It calls
      `NoteMatchResultIfEnded` FIRST and OUTSIDE its own latch, so §2.11.6's completion writer
      keeps its own opt-in, its own latch and its own retry-on-failure; then it clears
      `AiTurnTimer` and logs `STRAT-MATCH concluded` once. `bMatchConclusionAnnounced` is the
      latch, cleared in `StartMatchInternal` beside `bMatchResultRecorded`.
      `UStratMatchSubsystem::IsMatchConcluded` and `UStratMatchSubsystem::GetConcludedMatchView`
      are the two reflected accessors.
    - `AStratPlayerController::HandleSelectionEvent` gates on the model it has ALREADY built,
      ahead of `TryArmGuidedOpening` and ahead of the machine;
      `AStratPlayerController::ToggleProductionMenu` gates only its OPEN branch; and
      `UStratMatchSubsystem::SubmitProductionChoice` gates the build path, because a gate on move
      / attack / wait / end-turn alone would have been a lockout with a build-shaped hole in it.
  - **THE RULES MODULE DOES NOT STOP THESE COMMANDS, WHICH IS WHY AN ENGINE GATE IS THE FIX AND
    NOT A BELT.** The 2026-08-23 log shows `STRAT-CMD accepted kind=Move unit=14 ... side=1` and
    `STRAT-CMD accepted kind=Attack unit=14 ... side=1` landing AFTER the flag fell; only `EndTurn`
    carries the `[T-SAVE-05] no match is running` check. "The rules will refuse it anyway" was
    available as an argument and is false.
  - **THE AI'S TURN NOW TERMINATES, AND THE PORT DID NOT CHANGE.** `RunAiTurnsNow` re-reads the
    view model after an `FStratAiTurnOutcome` with `bOk` false and, when the match has since
    concluded, ends its loop with an EMPTY stop reason rather than reporting a fault. It keys on
    the rebuilt model and never on the refusal's text, so a re-vendored wording cannot silently
    turn this arm off. **THE ALTERNATIVE WAS `IsMatchOver()` ON `IStratAiTurnPort` AND IT WAS
    KILLED TWICE:** it would make a runner able to decide, which that interface's own header
    forbids in as many words, and it would oblige every test double in `Tests/` — a lane this
    agent may not edit — to grow an arm before the tree would build at all.
  - **THREE COMMENTS THAT THIS CHANGE INVALIDATED WERE UPDATED IN THE SAME CHANGE**, which is the
    obligation `StratGuidedOpening.h` names: `IStratAiTurnPort`'s "decided before `RunTurn` is
    called" paragraph now carries the amendment and the measurement; `FStratAiTurnOutcome::bOk`
    records that a `false` is no longer always a fault at the caller; and
    `NoteMatchResultIfEnded`'s declaration records that its caller moved and its behaviour did not.
  - **[SUPERSEDED 2026-08-23, LATER THE SAME DAY, BY THIS ENTRY'S OWN AUTHOR. THE TREE NOW LINKS
    AND THE SUITE HAS RUN IN IT.** The two bullets that stood here reported a compiled-but-unlinked
    tree and a non-empty save slot, and both were true of the tree they described. Superseded
    rather than deleted, because the compile-only measurement is what separated "the code is wrong"
    from "the editor is open", and the next reader who hits `LNK1104` needs to see that the
    distinction was made rather than assumed.]** The editor was closed, and
    `Saved/SaveGames/StratocracyMatch.sav` was MOVED rather than deleted, on the user's decision,
    to `C:\Users\me\AppData\Local\Temp\claude\E--MultiAgent-Stratocracy\e142e851-499d-4869-9f61-e543568bbc02\scratchpad\StratocracyMatch.sav.2026-08-23-1114`. Both blockers were RE-VERIFIED HERE rather than taken on report: `tasklist` returns
    `INFO: No tasks are running which match the specified criteria.` for `UnrealEditor.exe`, with
    `explorer.exe` PID 13508 listed in the same round as the control that shows the instrument can
    speak, and `Saved/SaveGames/` enumerates zero entries.
  - **THE BUILD IS GREEN UNDER THE PLAIN DOCUMENTED COMMAND, WITH NO FLAG.** `Build.bat` with the
    documented arguments and NO `-NoHotReloadFromIDE`: `Result: Succeeded`, `REAL_EXIT=0`, all
    nineteen actions -- sixteen compiles including `Module.StratPlay.gen.cpp`, then
    `[17/19] Link [x64] UnrealEditor-StratPlay.dll`, `[18/19] Link [x64] UnrealEditor-StratPlay.lib`
    and `WriteMetadata StratocracyEditor.target`. Zero warnings and zero errors reported.
    **THE FIRST PLAIN RUN LINKED IN TWO ACTIONS OFF CACHED OBJECTS AND THAT WAS NOT ACCEPTED AS
    EVIDENCE**, because those objects were produced by the earlier `-NoHotReloadFromIDE` run. The
    four changed sources were `touch`ed to force a recompile under the plain invocation, with
    `git hash-object` taken before and after proving all four blobs BYTE-IDENTICAL across the
    touch, and the nineteen-action build above is that second run. A cached object is not a compile
    anybody watched.
  - **THE SUITE RAN IN THIS TREE, HEADLESS, WITH THE EDITOR CLOSED, AND IT IS GREEN.** Zero
    non-Success and `notRun` zero, every entry `Success`. `reportCreatedOn 2026.08.23-19.55.53` --
    UTC, which is 15:55:53 local on the same day; do not "correct" either stamp to match the other.
    **NO FIGURE IS WRITTEN HERE AND NONE MAY BE**: `global.md` owns the count, and the figure went
    to the coordinator instead. No clause was added or removed by this pass, which is correct --
    this agent writes none.
  - **THE REPORT IS THIS TREE'S OWN AND ITS IDENTITY WAS CHECKED, not assumed.** `index.json` is
    stamped 15:55:53 local against the four changed sources at 15:52:42 and the relinked
    `UnrealEditor-StratPlay.dll` at 15:54:21 -- so the report is newer than both the code it
    describes and the binary it ran against.
  - **THE SAVE-SLOT CONTROL IS DISCHARGED ON THIS RUN AND NOT BORROWED.** `Saved/SaveGames/`
    enumerated ZERO entries before and ZERO after, and the directory mtime MOVED, 15:51:20 ->
    15:55:53, the run's own minute. **NOTHING APPEARED DURING THE RUN**, which matters more than
    usual this pass: the change touches the completion-write path, and a file materialising there
    would have been a finding rather than something to tidy away.
  - **THE NEW TRANSITION EXECUTES AT RUNTIME, AND THE ABSENCE THAT FIRST LOOKED LIKE A DEFECT WAS
    THE INSTRUMENT.** `STRAT-MATCH concluded` greps to ZERO in `suite.log` -- and so does the bare
    string `LogStratPlay`, 0 hits, so that file carries no `LogStratPlay` output at all and its
    silence proves nothing about the line. The control moved the question to
    `Saved/Logs/Stratocracy.log`, which carries 2909 `LogStratPlay` lines from this run, and there
    `STRAT-MATCH concluded` fires SIX times, every one inside the run's own window, reading e.g.
    `STRAT-MATCH concluded turn=6 turnCap=20 sideToMove=1 tier=Decisive`. So
    `UStratMatchSubsystem::ConcludeMatchIfEnded` is reached by the EXISTING AI-vs-AI clauses with
    no clause written for it, and `StaticEnum<EStratResultTier>()` resolves the tier name at
    runtime rather than printing an integer.
  - **THE ONCE-PER-MATCH LATCH IS CORROBORATED AND IS NOT PROVED, and the difference is stated
    because this record has been corrected before for reaching further.** Six conclusion lines
    stand against 73 `STRAT-AI turn-ended` and 967 `STRAT-AI applied` in the same log, and a broken
    `bMatchConclusionAnnounced` would emit one line per `ApplyView` on a finished match -- orders
    of magnitude more than six. That is an argument from MAGNITUDE and not a count of how many
    matches actually concluded; nothing here enumerated them. The clause that would settle it is
    named in the handoff to `strat-test-author` and is not owed by this pass.
  - **THE TWO LATCHES ARE VISIBLY INDEPENDENT IN THAT SAME LOG**, which is the property the
    ordering inside `ConcludeMatchIfEnded` was written for: six `STRAT-MATCH concluded` against
    five `now records a completed match`. Sec 2.11.6's writer stayed gated on its own opt-in and
    its own slot while the transition ran regardless, so neither became the other's condition. And
    `Saved/SaveGames/` is still empty after all five, so none of them reached the player's slot.
  - **THE DEBT THIS ENTRY OPENED AS "NOTHING VERIFIES AT LINK OR AT RUNTIME" IS DISCHARGED HERE.**
    Its stated discharge condition was the editor closing and the build plus suite running; both
    happened, the tree links, the suite is green, and the new transition was observed executing.
    What is NOT discharged and is not claimed: no human has played a match to a result with this
    fix in the tree, so the player-facing half -- that a finished match stops accepting clicks on
    screen -- rests on the input gate's source and on clauses nobody has written yet, not on
    anything anybody has watched. Injected input never reaches `UPlayerInput` on this project, so
    that observation needs a human at the keyboard. OWNED: coordinator, to schedule with the user.
  - **A DEBT SHIPS WITH THIS AND IT IS THE ONE A VICTORY SCREEN NEEDS: NOBODY DOWNSTREAM CAN SAY
    WHO WON.** `strat::MatchResult` carries `winner`, `cause` and `decidedByKey` and lives on
    `TurnState`; `strat::UiMatchView` — the only thing `FStratMatchView` mirrors — carries
    `turn`, `turnCap`, `sideToMove`, `hasResult` and `resultTier` and NO winner. So
    `GetConcludedMatchView` can say *Decisive* and cannot say *for whom*. **DELIBERATELY NOT
    CLOSED HERE**, because both routes cost more than the defect did: either a new `FStratBridge`
    accessor over `TurnState::result` plus a new reflected struct in `StratViewModel.h`, which
    would put a field on the view model that mirrors nothing and would need its own parity clause;
    or an upstream `UiMatchView::winner`, which is a vendored, hash-gated change in
    `E:\MultiAgent\stratocracy-crew` and a re-vendor. **DISCHARGED WHEN** a victory surface is
    actually specified — until then a screen can say a match ended and its tier, which is the
    transition this pass owed.
  - **THAT DEBT IS DISCHARGED 2026-08-25, BY A ROUTE IT DID NOT CONSIDER, AND IS STAMPED IN PLACE
    RATHER THAN DELETED.** Both stated discharge conditions were met and neither named route was
    taken. (i) A victory surface IS specified: §2.11.4's end-of-match screen gives the tier, the
    three scoreboard rows in order, and a FACTION-VOICED result line -- and the faction is chosen
    by WHO WON, so the winner is load-bearing for the specified screen rather than decorative.
    (ii) The route taken is a THIRD one the filing did not weigh: upstream added
    `strat::uiMatchResult` as a fourth `ui*` QUERY, on the precedent `uiBuildOptions` set. It
    costs neither of the two prices the filing objected to -- no view-model field that mirrors
    nothing, and no move to `kUiSnapshotFieldCount` / `kUiMirrorFieldCount` /
    `kUiDerivedFieldCount` / `uiFieldContract()` / `uiEnumerateSnapshot`. `FStratBridge::
    MatchResult`, `FStratMatchResultView`, `StratBuildMatchResult` and
    `UStratMatchSubsystem::GetMatchResult` are the engine half; the clauses are
    `strat-test-author`'s and are named in the 2026-08-25 entry at the top of this file.
    **WHAT IS NOT DISCHARGED:** nothing engine-side ASSERTS a winner yet. Until T-TURN-02 /
    T-TURN-04 / T-TURN-05 clauses exist -- and until at least one of them arranges
    `winner != sideToMove` -- a wrong derivation would pass every clause that does exist.

- **`HexSize` on `AStratBoardActor`** is centre-to-centre spacing for a pointy-top layout, the
  only axial→world constant in the project; must be matched to whatever tile mesh phase 5 picks.
- **`EnhancedInput` is absent from `StratPlay.Build.cs` by design**; phase 4 adds it. Do not
  "fix" it early. — **Discharged in phase 4**: `EnhancedInput` is now Private in
  `StratPlay.Build.cs`; `StratBridge` moved Public → Private there in the same change.
- **A `bDone`/`bLockedThisTurn` producer does not exist yet.** Phase 4's selection machine must
  write those onto the built view model via `ApplyView(model)`, never into an actor.
  `T-INT-05.NoActorHoldsPresentationBits` pins the negative only. — **Discharged in phase 4**:
  `FStratSelectionMachine::DecorateViewModel`, called between `BuildViewModel` and `ApplyView`,
  writes both bits from the machine's own `TSet`s; no actor holds either.
- **`SubmitCapture` has no affordance and the machine never emits `Capture`.** The phase-4
  command list is literally move → attack, wait, end turn; which hex offers a capture and how
  the player is told is an unanswered UI question. The applier's `switch` will need one new arm.
  (Phase 4 deferral, carried forward.)
  - **RETIRED 2026-08-25: NOTHING IS OWED, AND THE LAST SENTENCE WAS WRONG.** The affordance
    question is not unanswered -- §2.11 answers it in the negative: "Capture and build need no
    extra verbs. Capture is by presence (§2.7 ... a progress pip appears, NO BUTTON)."
    `strat::AiCommandKind` is `{Build, Move, Attack, EndTurn}` for the same reason. And the
    applier's `switch` will NEVER need an arm: `FStratBridge::SubmitCapture` is §4.10
    SAVE-FORMAT TRANSPORT, kept because `strat::SaveCommandKind` is pinned at the five and
    `Source/StratBridge/Tests/StratBridgeRestoreParity.cpp` and `StratBridgeSaveRecording.cpp`
    dispatch it in log replay. The reason never to wire an affordance is stronger than "no phase
    owns it": `strat::captureTick` is NOT IDEMPOTENT -- `prog->turnsHeld += 1` per call, and
    `applyCommand`'s Capture arm builds occupants from the whole board and never reads
    `c.unitId`, so the command is not even unit-specific. N submissions in one turn advance a
    capture N turns' worth; on the shipped scenario (`captureTurns = 1`) that is masked. Stamped
    into `StratPlayerController.h`, `StratSelectionMachine.h` and `StratMatchSubsystem.h`.
    **WHAT MAKES THIS FALSIFIABLE RATHER THAN ASSERTED** is a clause nobody has written:
    `TwoCaptureCommandsInOneTurnAdvanceHoldingTwice`. Named for `strat-test-author`.
- **The hot-seat hand-over key is deliberately unbound.** `SetViewingSide` on a keypress would
  let either player see the other's board at any time; the confirmation screen is UI work no
  phase owns. (Phase 4 deferral, carried forward.)
  - **THE KEY STAYS UNBOUND AND THE SECOND CLAUSE IS RETRACTED 2026-08-25: THERE IS NO
    CONFIRMATION SCREEN AND NONE IS OWED.** The GDD specifies no hand-over screen. Measured
    against `E:\MultiAgent\stratocracy-content\Stratocracy_Prototype_GDD.md`: four
    case-insensitive hits for hot-seat in the whole document, all four in stretch-feature
    context (§1.5's resolved question 4 -- "2-player hotseat: in scope or cut? RESOLVED: stretch
    only, off the critical path (§2.10)" -- the §2.10 scope table's STRETCH row, and two pacing
    asides); ZERO hits for "hand-over", "handover" or "pass the device"; and §2.11.5 states the
    complete prototype screen list -- "title/menu, briefing, match, result". So "UI work no phase
    owns" described work that does not exist, and a later reader following the citation would
    have scheduled a screen nobody asked for.
    **THE HALF THAT SURVIVES IS THE WHOLE REASON THE KEY IS UNBOUND** and it needs no screen
    behind it: a key that silently flipped the viewing side would let either player see the
    other's board at any time. If hot-seat ever comes off the stretch list, whatever drives the
    swap must make it deliberate and visible; a bare keypress cannot. Stamped into
    `StratPlayerController.h`'s "NOT IN THIS ROUND" block.
    **A REAL DEFECT WAS SITTING BESIDE IT AND IS FIXED** -- the `ViewingSide` desync between this
    class and `AStratScoreboardHUD`. See the 2026-08-25 entry at the top of this file.
- **`SetLockedThisTurn` has a writer and no shipping caller** (§2.11.6 guided opening is out of
  milestone), so `bLockedThisTurn` is false in every running path. Its clause calls the setter
  itself and discloses in four places that it pins machine behaviour, not that any shipping path
  produces a lock. **When §2.11.6's producer lands it needs a clause of its own; this one will
  not cover it.** (Phase 4 deferral, carried forward.) — **DISCHARGED 2026-08-21 at `1d6f758`:**
  `FStratGuidedOpening::PublishLocks` is the first shipping caller. It writes the bit for every
  friendly unit on every observation, true and false both, rather than setting once and clearing
  once — a writer that only ever sets is a writer whose clear can be missed on one path, and for
  this bit that means a permanently undimmable unit and a player who cannot move. The lock is
  keyed on beat 1a being OUTSTANDING, not on its holding the line, so it clears mid-turn when the
  marked Infantry's move completes and not at the turn boundary. The clause this entry asked for
  exists; `tests.md` names it.
- **The attack branch's "already acted" refusal — cite the branch, not a line number, this is
  the fourth instance of line-number rot in this milestone** — is the `if (Selected->bHasActed)`
  test inside the enemy-click arm of `FStratSelectionMachine::HandleEvent`
  (`Source/StratPlay/StratSelectionMachine.cpp`), unreachable by any click sequence because
  `NotifyCommandApplied` marks an attacker DONE on the same event. Ruled an acceptable
  defensive guard, not dead code — it guards a disagreement between the model's `bHasActed`
  (rules-side) and the machine's `DoneUnits` (engine-side, per-session) that a loaded save, a
  replayed log, or phase 6's PIE can produce even though no scripted sequence in this suite can.
  **DISCHARGED in the combat-outcome milestone's phase 5:** the branch now carries a comment
  block stating exactly this — why no click sequence reaches it, what it actually guards, and
  that its cost is that no clause covers it — added by `strat-gameplay-engineer` and explicitly
  ruled correct by the reviewer ("labelling it is what stops the next reader deleting it, which
  is the failure mode the phase exists to prevent. Keep it.").
- **`ReplayRecordedLogOnto` does no save round trip** — no serialize, no parse, no
  `FStratSaveIdentity`. `SerializeRecordedSave` + `T-SAVE-06.SaveRoundTripsToEqualHash` already
  cover that path; a second entry point there would be a second policy over the same bytes.
  **Still true as written, and 2026-08-21 did not breach it:** `RestoreFromSaveText` is the READ
  side — it parses and replays §4.10 text *into* a bridge; `ReplayRecordedLogOnto` still never
  serializes. One policy over the bytes, one direction each.

---

### 2026-08-22 — the BUILD affordance for §2.11.5's production menu (input + widget host)

C++ only. No asset, no test — `WBP_ProductionMenu` and the clauses are other lanes'.

**THE BUILD IS RED AND THE CODE IS NOT THE REASON.**
`Build.bat StratocracyEditor Win64 Development -project=… -waitmutex -NoHotReloadFromIDE`
with the editor OPEN (the coordinator was driving it): **Result: Failed
(OtherCompilationError)**, 164.94 s. All 22 compile actions succeeded — including
`Module.StratUI.gen.cpp` and `Module.StratPlay.gen.cpp`, so UHT parsed every new reflected
member — and both LINK actions failed with
`LINK : fatal error LNK1104: cannot open file
'E:\MultiAgent\Stratocracy\Binaries\Win64\UnrealEditor-StratUI.dll'` and the same line for
`UnrealEditor-StratPlay.dll`, each preceded by `UbaSessionServer - ERROR opening file … for
write after retrying for 20 seconds (The process cannot access the file because it is being
used by another process. - …\UnrealEditor.exe)`. **A GREEN BUILD OF THIS CHANGE HAS NOT BEEN
MEASURED** and nothing below may be read as if one had; it needs one rerun with the editor
closed. **[SUPERSEDED 2026-08-22: THAT RERUN HAPPENED AND WAS GREEN.** The coordinator
closed the editor and rebuilt these exact bytes -- `Result: Succeeded`, exit 0, the two links
that had failed completing normally. For the suite run over these bytes see `global.md`'s
banner, which owns that fact class; this file does not restate it. The red
measurement above is KEPT rather than deleted because its finding is the durable half:
`-NoHotReloadFromIDE` does not cover the open-editor DLL write lock, which
`.agents/ue-project-context.md` now records. **Do not read the sentence before this bracket as a
live instruction to rebuild** -- doing so is the stale-status-line failure this project has paid
for, in its under-claiming direction. Stamped by the COORDINATOR and not by
`strat-gameplay-engineer`, whose file this is, under the same declared-deviation form as
`content.md`'s newest entry; `strat-integration-reviewer` raised the staleness as its one BLOCK
finding and named either owner as acceptable.]** For any suite figure or phase verdict see `global.md` — this file states neither.

- **`-NoHotReloadFromIDE` DOES NOT DEFEAT THE OPEN-EDITOR DLL LOCK, AND THE DISPATCH BRIEF
  SAID IT WOULD.** The memory it came from is about the Live Coding **mutex**, which is
  engine-keyed and global and which that switch does defeat — that is what lets a worktree
  build start at all while an editor is up. The **write lock on `Binaries\Win64\*.dll`** is a
  different mechanism: the running `UnrealEditor.exe` has the DLLs mapped, and no UBT switch
  unmaps them. Measured above; the two failures are 20 s apart in the same run, so the switch
  bought the compile phase and nothing after it. `.agents/ue-project-context.md` already
  states the rule correctly ("a link error naming `UnrealEditor-Strat*.dll` and 'cannot open
  for writing' means *close the editor*"); what was missing was that `-NoHotReloadFromIDE`
  is not an exemption from it.

- **THE RULING'S PREMISE WAS WRONG AND THE AFFORDANCE IS BUILT ON THE CURSOR INSTEAD.** The
  user's ruling — a dedicated action opens the menu "for the CURRENTLY SELECTED HEX" — names
  a thing this project does not have. `FStratSelectionMachine` holds `SelectedUnitId` and
  nothing spatial; its `HexPrimary` arm treats a click on empty ground with nothing selected
  as "an ordinary click and not a failure", so **a factory hex with no unit on it cannot be
  selected at all**, and one with a unit on it is not a hex you can build at. An accessor
  over the machine's state would have had to ADD a hex-selection concept to the machine —
  exactly what the ruling was shaped to avoid. `AStratPlayerController::HexUnderCursor` is
  used instead: the same source `OnSelect` already uses to decide which hex a click means.
  **The ruling's INTENT is intact and is what was implemented** — `HexPrimary`'s semantics
  are untouched, `FStratSelectionMachine` gained no BUILD arm, no
  `EStratSelectionCommand::Build` exists, and `StratSubmitSelectionCommand`'s `switch` gained
  no arm. Only the ruling's account of where the hex comes from moved.

- **THE HEX IS LATCHED AT THE KEYPRESS AND NEVER RE-READ, and the widget's own lifecycle is
  what forces that.** `AStratPlayerController::GetProductionTargetHex(FIntPoint& OutHex)`
  hands back what `ToggleProductionMenu` recorded, not a live cursor read. The asset's
  `Construct` runs INSIDE `AddToViewport`, so a live read would answer a different hex the
  moment the mouse moved off the factory between the keypress and the construct — a menu
  built for a factory the player never pointed at. Two channels rather than one because
  `FIntPoint(0, 0)` is a real hex and cannot signal its own absence, the same trap
  `IsProductionMenuOpen` records about `ProductionMenuHex`. **T-INT-05 does not reach this
  latch**: that clause forbids an actor holding `bDone` / `bLockedThisTurn`, which are fields
  OF THE VIEW MODEL and would drift if copied; this is an input intent, appears in no view
  model, and nothing on screen is drawn from it.

- **THE WIDGET IS CREATED BY `AStratScoreboardHUD` AND THE DEBT THAT FILE RECORDS GREW FROM
  SEVEN MEMBERS TO ELEVEN RATHER THAN A SECOND DEBT BEING OPENED.** The guidance block's
  argument transfers unchanged: `CreateWidget` + `AddToViewport` mean `UMG`, `Slate` and
  `SlateCore`, and `StratPlay.Build.cs` growing all three is a structural cost where widening
  the HUD is a prose one. The same condition discharges all eleven together — a §2.11 UI-layer
  owner existing. The **only** difference from the other two widgets is lifetime: this one is
  created ON DEMAND and destroyed on close, because §2.11.5's menu is about ONE factory chosen
  at the moment it is asked for, and a resident panel would need "no factory" as a drawable
  state nothing in the rules produces. An unset `ProductionMenuWidgetClass` is still reported
  once at `BeginPlay` beside the other two, so "not configured" and "failed to open" stay
  distinguishable.

- **`ProductionMenuWidgetClass` IS TYPED `TSubclassOf<UUserWidget>` AND THAT IS THE
  IRREVERSIBLE CALL BEING HELD OPEN RATHER THAN SPENT.** A Blueprint deriving from a C++ class
  bakes `/Script/Module.Class` into itself permanently. Typing this property as a base of ours
  would decide which module owns the menu widget before anybody needed to decide it — the same
  call the reflected seam declined a day earlier. **The cost, stated so the next reader does
  not assume it is free:** the HUD can call nothing on the widget beyond `UUserWidget`'s own
  surface, so it cannot refresh it. It does not want to; a caller that does must pay for a
  base class first. **The condition that discharges it:** somebody needing C++-side layout or
  a `BlueprintImplementableEvent` hook.

- **NOTHING IN C++ CALLS `RefreshMenu`, AND THE HUD COULD NOT IF IT WANTED TO.** `RefreshMenu`
  is a Blueprint custom event on the asset and the member is typed `UUserWidget`, so there is
  no C++ name to call — the constraint is structural, not a convention someone could forget.
  C++ creates and shows; the widget refreshes itself from `Construct` off
  `GetProductionTargetHex`. That keeps WHEN a menu's contents are decided in one place.
  `ToggleProductionMenu` likewise calls no `RefreshProductionMenu` and asks the rules module
  nothing at all.

- **THE ONE THING THE TOGGLE REFUSES IS A CURSOR OFF THE BOARD, AND IT IS NOT A RULES
  ANSWER.** It does not test whether the hex is a factory, whether the side holds it, or
  whether the side can pay. All three are rows on the menu with the module's own reasons
  attached, and `RefreshProductionMenu` is documented to SUCCEED on a hex that is not a build
  point — drawing a full menu of unavailable rows, which is what §2.11.5 draws. A pre-check
  here would replace the module's reason with this class's silence.

- **CLOSING IS TWO ACTS ON TWO OBJECTS IN A FIXED ORDER: panel down, then rows cleared.** The
  reverse leaves a live panel bound to an empty `ProductionMenu` array for however long the
  two lines are apart — a screen showing a menu the subsystem says is not open.
  `AStratScoreboardHUD::CloseProductionMenuWidget` deliberately does NOT reach into
  `UStratMatchSubsystem` to do the second act itself; that would be the first line of the HUD
  becoming a second thing that runs matches, which its header block forbids in those words.
  `AStratPlayerController::ToggleProductionMenu` is the one caller and does both.

- **`IsProductionMenuWidgetOpen()` READS THE WIDGET AND IS NOT A BOOL BESIDE IT**, on the rule
  `IsProductionMenuOpen` states about its own rows. A WBP that removes ITSELF — a Cancel button
  in the asset, a shape the asset is free to have — leaves the pointer non-null and the widget
  off the viewport; a cached bool would have answered true and the toggle would have needed two
  presses. It is **out of line rather than inline**, and that is a module arrow rather than a
  style choice: the body calls `UUserWidget::IsInViewport()`, and inlining it would push `UMG`
  onto every module including `StratScoreboardHUD.h`.

- **`CreateProductionMenuWidget` ASSIGNS THE MEMBER BEFORE `AddToViewport`, WHICH IS THE
  OPPOSITE ORDER TO `CreateGuidanceWidget`.** `AddToViewport` constructs the widget, so the
  asset's `Construct` runs inside that call — and this asset's `Construct` refreshes the menu,
  which can reach a Blueprint asking the HUD whether a menu is open. Assigning afterwards would
  have that question answered "no" from inside the act of opening one. The strip has no such
  graph and keeps the narrower rule.

- **TWO DEFERRAL BULLETS WERE RETRACTED IN PLACE, AND ONE HALF-BULLET WAS DELIBERATELY LEFT
  STANDING.** `AStratPlayerController.h`'s "CAPTURE and BUILD … neither has an answered
  affordance question" is retracted for BUILD only; `FStratSelectionMachine`'s "BUILD.
  §2.11.5's production menu is explicitly out of the hot-seat milestone" is retracted with the
  substance being that nothing in that struct changed. **`SubmitCapture` still has no
  affordance and no engine caller** — the standing `## NEXT` entry above is untouched, and the
  capture half of the controller's bullet stands unretracted.

### 2026-08-22 — the reflected seam for §2.11.5's production menu (`UStratMatchSubsystem`)

C++ only. No widget, no asset, no test — the WBP and the clauses are other lanes'. Build was
`Build.bat StratocracyEditor Win64 Development` with the editor confirmed closed
(`Get-Process UnrealEditor` empty): **Result: Succeeded**, 21 actions, 77.73 s, zero warnings.
For the suite figure and any phase verdict see `global.md` — this file states neither.

- **The seam went on `UStratMatchSubsystem` and not on `AStratScoreboardHUD`, and the WRITE
  half is what decided it.** The HUD was the closer prior art on the read half — it has a
  bridge, already hosts a second widget, and `RefreshScoreboard` is the exact shape
  `RefreshProductionMenu` needed. Two things killed it. First, **an accepted Build puts a unit
  on the board**, and the only thing in the project that can make the board agree with that is
  `ApplyView`, which lives on the subsystem; a submit routed through the HUD would refresh the
  scoreboard panel and leave the new unit unspawned until something else happened to reconcile
  — a screen carrying a change its view model did not. Second, **the HUD is shedding its
  bridge, not growing one**: `StratScoreboardHUD.h` states in advance that it must give up
  bridge ownership once a proper owner exists, the subsystem IS that owner, and hanging a
  MUTATING rules call off the borrower deepens the debt instead of discharging it. A
  `UBlueprintFunctionLibrary` was the third shape and is structurally impossible rather than
  merely worse: its statics would need an `FStratBridge&`, which is not a reflected type and
  must never become one, so no Blueprint graph could ever supply the argument.
- **No new `UCLASS`, and that is deliberate rather than incidental.** A WBP reaches the
  subsystem with `Get World Subsystem` and derives from nothing of ours, so no
  `/Script/Module.Class` parent is baked into an asset before anybody has decided who owns the
  menu widget — a choice that is irreversible the moment an asset makes it. **The condition
  that would discharge this into a `UStratProductionMenuWidget` base:** somebody deciding the
  menu needs C++-side layout or a `BlueprintImplementableEvent` hook, which no caller needs
  today.
- **The submit takes no hex, and that is the one guard the engine adds.** `DefIndex` is
  meaningful only as a row of the menu it was picked from, and `applyCommand` uses that field
  as a raw bounds-checked-only index with no name lookup — the same property that makes
  `DT_Units` row order load-bearing. A caller-supplied hex would let a widget pick row 2 of
  factory A and build it at factory B, and the rules module would accept it silently.
  `SubmitProductionChoice` reads the OPEN MENU's own hex, which removes the pairing rather than
  checking it.
- **On the SUBMIT path the engine refuses exactly one rules-shaped thing, and it is not a
  legality question.** (The other two arms there are not rules-shaped: no live match, and an
  AI turn in progress.) A `DefIndex` appearing in NO row of the open menu is refused locally
  — a value the module never OFFERED.
  `bAffordable` and `bAvailable` are **not read** on the submit path; whether the side can pay
  and whether the factory will take a build are the rules module's answers and the submit asks
  for them again from the authority that owns them. Nothing anywhere on this path ANDs the two
  bits, which is T-UI-03's forbidden arithmetic — they are separate answers because
  §2.11.5 shows them differently.
- **The READ path has TWO CHANNELS and this seam collapses neither — and the prose that
  shipped with this entry got the split backwards, so it is corrected here rather than
  quietly.** `FStratBridge::BuildOptions` states it in its own declaration block:
  `FStratResult` says whether the query could be ASKED; `bAvailable` and `Reason` on each row
  say what the rules ANSWERED.
  - **Riding the `FStratResult` refusal channel:** a bridge with no definitions loaded or no
    scenario seeded; a SIDE outside the match — the one malformed-question case, refused in
    the bridge rather than left to the module because `uiBuildOptions` spells "invalid side"
    the same way it spells "not a build point"; and an EMPTY SET, which is unreachable with a
    unit table loaded because the menu carries one row per §2.4 row.
  - **Riding each ROW's `bAvailable` / `Reason`:** a factory this side does not hold, one that
    has already taken its build this turn, one already holding a waiting build, and **a hex
    that is not a build point at all**. The factory hex is therefore NOT pre-checked — not in
    `FStratBridge::BuildOptions` and not in `UStratMatchSubsystem::RefreshProductionMenu` —
    the way `Forecast` leaves the defender to the module.
  **So a non-factory hex OPENS a menu and is not refused:** `IsProductionMenuOpen()` reads
  true, every row reads `bAvailable` false with the module's own reason, and §2.11.5 draws
  those reasons. A caller asking "is this hex a factory" must read the ROWS, never the return
  value. **The opposite claim — that `UStratMatchSubsystem::RefreshProductionMenu` REFUSES a
  hex that is not a factory — stood in this file and in that function's declaration block and
  was never measured;** it reached both from a handoff note by way of the dispatch brief.
  `strat-test-author` measured the tree independently, declined to write the clause asserting
  it, and wrote `GATE-BUILDMENU.ANonFactoryHexOpensAnUnavailableMenuAndIsNotRefused` instead.
  It is now **retracted in place** in that declaration block, and the `Bridge.Get() == nullptr`
  arm in `UStratMatchSubsystem::RefreshProductionMenu` — which drew the same contrast in other
  words, naming "that hex is not a factory" as a later refusal — is corrected beside it.
  Corrected 2026-08-22; **no behaviour changed and no signature moved** — comment text only,
  rebuilt to prove the headers still parse:
  `Build.bat StratocracyEditor Win64 Development -project=… -waitmutex` with the editor
  confirmed closed (`Get-Process UnrealEditor` empty), **Result: Succeeded**, 16 actions,
  82.52 s, zero warnings.
- **An empty array means "no menu open", and what makes that sound is one layer down.**
  `StratBuildProductionMenu` REFUSES rather than producing an empty menu, so an empty array is
  not a value any successful refresh can produce and is free to carry the marker.
  `ProductionMenuHex` could not have carried it — `FIntPoint(0, 0)` is a real hex, and this
  project has already paid once for treating a real default as an unset marker. **What would
  break it:** a rules change making an empty buildlist a legal answer; the cost if it broke is
  a refused submit, never a wrong build.
- **`false` after an accepted command is possible and is stated rather than designed away.**
  On acceptance the menu and then the screen are both rebuilt; the command is recorded and
  irreversible by then, so a rollback is not on offer. Both rebuilds are attempted before
  either is reported, and the reason is prefixed `build accepted, but …`.
  `FStratBridge::RecordedCommandCount` is the disambiguator. Same trade `SetViewingSide`
  documents about its own already-changed side.
- **The header's "NOT IN THIS ROUND — submitting commands" bullet is RETRACTED IN PLACE for
  Build and only for Build.** Its own stated condition — "a wrapper landed early is a wrapper
  written without its caller" — was met: §2.11.5's menu is the caller. Move, Attack and
  EndTurn still travel `FStratSelectionMachine` / `StratSubmitSelectionCommand` from
  `AStratPlayerController`, because that is where a CLICK ON THE BOARD belongs and a menu pick
  has no click and no selection to advance. **`SubmitCapture` still has no engine caller at
  all** — the standing `## NEXT` entry above is untouched by this change.
- **The menu is cleared in `TearDownPresentation`,** beside `AppliedModel`, because a buildlist
  describes one factory in one `strat::GameState` and a reseed and a world death both
  invalidate it.
---

### 2026-08-21 — save-slot I/O and the §2.11.6 guided opening (lane B, merged at `1d6f758`)

Built in the slot-1 worktree on `feat/save-and-guidance` off `870c611`, then rebuilt and re-run
on this tree. For the suite figure and the phase verdict see `global.md`'s banner — this file
does not restate either.

- **A restore is a replay that is verified before it is adopted.** `FStratBridge::RestoreFromSaveText`
  calls `strat::loadSave` for the header, then replays the parsed `commandLog` onto a **copy** of
  `GameState` and compares `strat::canonicalStateHash` against the save's `stateHash` before
  assigning anything. Cite the copy, not a line: the reason it is a copy is that `replayLog` is
  all-or-nothing *within the log* but the hash check sits downstream of it, and `replayLog` cannot
  roll back for a reason it never sees. Assigning `GameState`, `Recorded` and `OutCommandCount` in
  one step at the bottom is what makes the method all-or-nothing on *this* object. A mutated state
  hash is therefore REFUSED, not restored, and the refusal carries `T-SAVE-06` as its id. The
  freshness arm above it (`!Recorded.empty()`) refuses a bridge that has already applied commands,
  for `ReplayRecordedLogOnto`'s arm-4 reason.
- **`UStratSaveGame` (`Source/StratPlay/StratSaveGame.h`) carries the slot, and carries exactly
  what §4.10 cannot.** The slot overrides three config fields on load — `ScenarioFile`, `FirstSide`,
  `ViewingSide` — and nothing else. Tables and actor classes come from `ActiveConfig`, because a
  slot that pinned `DT_Units` would break the first time that asset was renamed, and the
  definitions are already checked harder by the replayed-hash comparison inside
  `RestoreFromSaveText`.
- **`StartMatchInternal` is an extraction, and the extraction is the point.** `SaveMatchToSlot`
  and `LoadMatchFromSlot` do not each own a start sequence; `LoadMatchFromSlot` runs `StartMatch`'s
  body with one optional extra step between `LoadScenarioFromFile` and the hand-over. There is
  exactly ONE `LoadDefinitions → LoadScenarioFromFile → (restore) → AdoptBridge` ordering in
  `StratMatchSubsystem.cpp`, which makes that ordering a contract rather than a habit. Two
  orderings side by side is the shape that produces two `strat::GameState`s for one map, and it
  drifts silently because both halves compile. The presentation consequence is free rather than
  arranged: the extracted body ends in `BuildViewModel → ApplyView → HandBridgeToScoreboard`, so
  `T-INT-05`'s "rebuild the screen from the view model alone" covers a loaded match without a
  second drawing path existing.
- **`FStratGuidedOpening` (`Source/StratPlay/StratGuidedOpening.h`/`.cpp`) is the guidance layer,
  held by value on the controller beside `FStratSelectionMachine` and not a `UCLASS`.** Nothing
  spawns it and nothing finds it by class; its whole visible surface reaches the screen through
  `FStratGuidanceView` on the view model. It names no `strat::` type, which is why
  `StratPlayerController.h` may include it. **It decides nothing:** every fact it branches on is a
  field read off `FStratViewModel` (`bIsGuidedMarked`, `bHasMoved`, `Turn`, `CaptureProgress`) or
  an answer `FStratBridge::GuidedOpeningHexes` gave it. In particular the marked unit comes from
  `bIsGuidedMarked`, which the rules module derives off `placement`, and is never inferred by
  comparing a hex against `guidedOpening.infantry` — that comparison would let beat 1a's own move
  unmark the unit the beat is about.
- **Beat 2 retires only on a capture pip at `guidedOpening.objective`, and that follows a ruling
  rather than a reading.** §2.11.6 is ambiguous; the 2026-08-21 user ruling narrowed
  `HasCapturePipLanded` from any pip to the ringed objective, looked up through
  `FStratBridge::GuidedOpeningHexes`. §2.11.6 forbids the alternative by name ("no 'nearest
  objective' heuristic is used"). **That makes `T-SCN-07.GuidedOpeningHexesMatchesTheScenarioFile`
  load-bearing rather than decorative:** without it a regression to a nearest-factory heuristic
  retires beat 2 on the wrong tile with a green build instead of failing visibly. `HasAnyUnitSpawned`
  is now the one remaining place that takes a broader observable than the fact it wanted, and says
  so in the open.
- **The Q27 input gates are three arms in `AStratPlayerController::HandleSelectionEvent`, and two
  of them return BEFORE the machine sees the event.** The End Turn arm and the `Wait` arm are
  pre-`HandleEvent` because `Wait` is the event that sets `bDone` without a rules command:
  intercepting it after the machine had seen it would leave the unit retired with beat 1a still
  outstanding — End Turn inert and no move left to satisfy it, which is precisely the deadlock the
  two closures exist to make unreachable. The third arm is the attack gate, and it can only sit
  after `HandleEvent` and before the submit, because a primary click on an enemy hex is `HexPrimary`
  and nothing earlier can tell it from a move; `NotifyCommandApplied` is not reached, so the DONE
  set does not move either. **All three refuse without touching `FStratBridge`** — an inert input
  is an input that was never asked about, not a command the rules module refused — and all three
  still call `RefreshFromMachine`, so the strip and the dimming show the player *why* nothing
  moved. The reason string is `FStratGuidedOpening::EndTurnGateHoverText`, so the log line, the
  hover and the strip cannot say three different things.
- **`AStratBoardActor::GetTargetOverlayCount` reads the component, not a cached number.** It
  returns `TargetOverlay->GetInstanceCount()`, so it cannot report a light the board is not
  actually showing — the failure mode a cached count has is agreeing with the code that set it
  while disagreeing with the screen. `BlueprintPure`, and deliberately not `GetDrawnHexCount`,
  which is the tile count and is the thing it is easiest to conflate it with.
- **The attack-target suppression clears the set rather than declining to compute it.**
  `RefreshFromMachine` calls `BuildOverlays` as always and then resets `TargetHexes` when
  `GuidedOpening.IsUnitInputGated` says so. Asking `FStratSelectionMachine::BuildOverlays` to know
  about beats would put a guidance rule inside a state machine that owns none. The cost is one
  `AttackTargetHexes` call whose answer the player will not see, and that is the price paid on
  purpose. The REACH overlay is left lit, because beat 1a's own directive is "Lit hexes are its
  true reach".

#### The completion writer's opt-in, 2026-08-21

- **`FStratMatchConfig::bRecordCompletionOnMatchEnd` gates the unprompted §2.11.6 write, and it
  exists because the guard it replaced protected nothing in the case it was written for.**
  `UStratMatchSubsystem::NoteMatchResultIfEnded` gated on
  `ResolveSaveSlotName(FString()).IsEmpty()` and its comment said that arm was for "an automation
  test that never configured a slot". `FStratMatchConfig::SaveSlotName` is declared
  `= TEXT("StratocracyMatch")` — the player's slot — so a caller that configured nothing resolved
  to the shipped slot, never to empty. Measured with a two-directional control: the integration
  tree ran the suite without the writer and `Saved/SaveGames/` ended with zero files; this
  worktree ran the same suite with the writer and produced `StratocracyMatch.sav`, 2096 bytes.
  The AI-vs-AI clauses in `Source/StratPlay/Tests/StratAiMatchClauses.cpp` name no slot and assert
  a result, so every suite run recorded a completed match on the player's slot and suppressed the
  guided opening permanently — the inverse of the change's purpose.
- **The root cause was a predicate, not a fixture.** Emptiness cannot distinguish `unset` from
  `chosen` when the default is non-empty, so patching one fixture would leave the trap armed for
  the next. `StratMatchLifecycle` and `StratMatchReconcile` were named latent by the gate, safe
  only because they never reach a result. The new field is a separate axis: a slot name answers
  WHERE, the flag answers WHETHER.
- **The alternative was a `TOptional<FString>` slot name and it cannot be built here.** `TOptional`
  is not a reflectable `UPROPERTY` type and `FStratMatchConfig` is `USTRUCT(BlueprintType)` reaching
  a designer through a details panel, so the unset/set distinction on the string itself would cost
  the group its editability — the one thing the struct's header block says it exists for.
- **The flag gates `NoteMatchResultIfEnded` ONLY.** `RecordMatchCompletionOnSave` and
  `SaveMatchToSlot` stay unconditional, because a caller that named a slot has already chosen.
  That partition is what keeps the two completion clauses that call the writer directly green
  while the two that reach it through the hook go red until they opt in.
- **The empty-slot early return survives, re-worded.** It is now reachable only by a deliberate
  clear — opted in with `SaveSlotName` emptied — and stays silent and unlatched for its original
  reason: `ApplyView` runs per refresh and a warning there is a warning per frame.
- **Misleading prose retracted in place** in `UStratMatchSubsystem::HasCompletedAMatchOnSave`'s
  block, which named "the slot name is empty" as the first thing to check in a "why does the strip
  keep coming back" report. That branch is unreachable without a deliberate clear. The first thing
  to check is now `bRecordCompletionOnMatchEnd` being false, which is the one a build can get wrong
  silently: false in C++, true only on the GameMode Blueprint's default.

#### Debts taken on 2026-08-21 (continued)

- **Shipping now depends on a Blueprint default that no C++ asserts.** `BP_StratGameMode`'s
  `MatchConfig.bRecordCompletionOnMatchEnd` must be true or §2.11.6's guided opening re-arms on
  every match in the shipped game — the original defect, reintroduced by an unset checkbox, with a
  green build and a green suite. This is the deliberate cost of defaulting to the safe direction:
  the failure mode moved from "corrupts the player's slot" to "forgets the player's history", and
  the second is the one §2.11.6 says to prefer. **Discharged by** a `Content/` default carrying the
  flag plus a clause that reads it off the GameMode's CDO rather than off a C++-built config — both
  outside this lane, and both filed in the handoffs for this change.

#### Debts taken on 2026-08-21

- **`UStratSaveGame::bHasCompletedAMatch` has a READER and NO WRITER, so the guided opening runs
  on every match rather than only the first.** Verified against this tree rather than taken on
  report: the only shipping read is the `return Payload != nullptr && Payload->bHasCompletedAMatch;`
  at the bottom of `UStratMatchSubsystem::HasCompletedAMatchOnSave`, and the only assignment
  anywhere in `Source/` is a test planting the value in `StratSaveSlotClauses.cpp`. `SaveMatchToSlot`
  carries the field forward by reading the existing slot before overwriting — so the debt is a
  MISSING writer and not a LOSSY one — and its declaration states in prose that it does not set the
  field and that nothing else does. **The condition that discharges it:** a caller that sets the
  field when a match reaches a result, i.e. a match-ended hook off `FStratMatchView::bHasResult`,
  which is the view model's own answer to "is this match over". It is deliberately not inferred
  inside `SaveMatchToSlot`, because a save taken on the final turn of a match the player then
  abandons is not a completed match. Until then `HasCompletedAMatchOnSave` answers false for every
  slot and guidance runs every match — the safe direction of the two, since guidance shown to a
  veteran is a strip they dismiss and guidance withheld from a first-timer is §2.11.6 silently not
  happening. The earlier note in `StratMatchSubsystem.h` that said wave B2 would write it is
  retracted in place: B2 landed the guided opening and did not, and a wave label stops being
  checkable the moment the wave closes. — **DISCHARGED 2026-08-21 on `feat/match-ended-hook`, in the
  working tree at the time of writing (the user commits, so no hash is cited here rather than a wrong
  one):** `UStratMatchSubsystem::RecordMatchCompletionOnSave` is the writer and
  `UStratMatchSubsystem::ApplyView` is the caller, through the private `NoteMatchResultIfEnded`, on
  `FStratMatchView::bHasResult`. The hook hangs off `ApplyView` and not off a command result because
  `ApplyView` is the one place every model reaches the screen through — `RefreshPresentation`,
  `AStratPlayerController::RefreshFromMachine` after decorating, and `StartMatchInternal` on a load
  all end there — and a hook on the submit path would miss §2.9's AI making the winning move, which
  is exactly how a first-session match ends. `SaveMatchToSlot` is unchanged and still carries the
  onboarding pair forward by not writing it; the new writer touches that one field and no other, so
  whichever of the two runs second keeps what the first left. The inference this debt forbade is
  still forbidden: nothing in `SaveMatchToSlot` derives the flag from match state.
- **The completion writer CREATES a slot that does not exist, so `DoesSaveSlotExist` can now answer
  true for a slot with no match in it.** Taken on deliberately: writing only into an existing slot
  would leave a first-time player who finished a match without ever pressing Save a first-time player
  forever, which is the likeliest path through a first session and therefore the path the defect
  would have survived on. The payload created that way carries an empty `SaveText`, which
  `LoadMatchFromSlot` already refuses by name ("carries no §4.10 text") rather than restoring an
  empty board — so the failure is a refusal and not a corrupt match. **Discharged by** a "Continue"
  affordance gating on a LOADABLE slot rather than on `DoesSaveSlotExist`; there is no such
  affordance in the tree today, and `DoesSaveSlotExist` has no shipping caller (its only caller is a
  test), which is why this is recorded as a hazard for the next reader rather than a live bug.
- **`LoadMatchFromSlot` tears down before it validates, so a refused load leaves NO match.** The
  sequence rebuilds; a slot that fails its header or its hash check leaves `IsMatchLive()` false
  and the caller must start a new one. Validating first would need the load checked against a
  bridge that does not exist yet — a second seeded bridge, which is the thing `UStratMatchSubsystem`
  exists to make impossible. **Discharged by** an `FStratBridge` that can be constructed, seeded
  and restored detached and then swapped in whole; `ReplayRecordedLogOnto` is already that shape on
  the write side, so the seam exists and nothing needs it yet.
- **User index 0 is hardcoded in the slot path.** §2.11 is hot-seat — two players, one machine, one
  logged-in user — and a per-side index would split one match's saves across two directories at the
  hand-over. **Discharged by** split-screen or online profiles arriving; the constant is one place.
- **The Q27 input gates are filed under `T-UI-02`, whose written invariant is the reachable-hex
  highlight.** No acceptance id was minted for them. That filing follows the precedent this tree
  already set with `ControllerTicksSoInputDispatches`, but it is a stretch and is recorded as one
  rather than papered over; the gap is filed upstream. **Discharged by** an id for input gating
  existing in the GDD. (The other reassignment in the same change is not a stretch: the
  guided-opening lookup moved from `T-SCN-02` to `T-SCN-07` because its subject is the two authored
  `guidedOpening` fields, which `Scenario.good.cpp` refuses violations of under `T-SCN-07` and under
  no other id.)
- **Two guided-opening DELIVERY defects, both closed on `feat/guidance-delivery` in the working tree
  at the time of writing (the user commits, so no hash is cited here rather than a wrong one).** The
  strip reached the screen and had never been seen to draw a directive; neither defect was in the
  beat machine or in the projection, and no clause covered either, which is how both survived a green
  suite. Cited by function and branch, never by line.
  - **Defect 1 — the session's only decorated push was dropped because the strip did not exist.**
    Measured across five fresh PIE sessions on 2026-08-21: `AStratPlayerController::BeginPlay`
    reached `UStratMatchSubsystem::ApplyView` with a decorated model before
    `AStratScoreboardHUD::BeginPlay` had run `CreateGuidanceWidget`, every time, so
    `AStratScoreboardHUD::PushGuidance`'s null check discarded it and nothing reconciles again
    without player input. **Closed by** `AStratScoreboardHUD::PushGuidance` recording every value it
    is handed in `LastPushedGuidance` / `bGuidanceEverPushed`, and `CreateGuidanceWidget` calling the
    new `AStratScoreboardHUD::DeliverLatestGuidance` after the strip is on screen. It asserts no
    `BeginPlay` order in either direction: a push before the strip is replayed, a push after it takes
    the ordinary route, and a session with no push leaves the strip on its defaults.
  - **Defect 2 — every subsystem-side reconcile cleared the strip.** Driven and watched on
    2026-08-21: a decorated refresh put a live directive on the widget, and `SetViewingSide` →
    `RefreshPresentation` → undecorated `ApplyView` → `PushGuidance` left it reading
    `bActive=False, Beat=None, DirectiveText=""` one second later. The same clearing on the first AI
    turn (`RunAiTurnsNow` → `RefreshPresentation`) is a **strong inference and not an observation** —
    taking a turn needs input, so it was never driven. **Closed at the build and not at the push**:
    `ApplyView`'s "UNCONDITIONAL, WITH NO BRANCH ON `bActive`" ruling is intact and untouched, and
    the fix is that models missing their guidance section no longer reach it.
    `UStratMatchSubsystem::BuildViewModelForPresentation` runs a registered `FStratViewDecorator`
    over every model this subsystem builds to draw from, and `AStratPlayerController::BeginPlay`
    registers `AStratPlayerController::DecorateForPresentation` as that decorator.
- **The undecorated `BuildViewModel` is still public and still has two callers that must not
  decorate.** `UStratMatchSubsystem::IsAiTurnDue` and `RunAiTurnsNow`'s loop read `sideToMove` and
  `bHasResult` off a model nobody draws; decorating there would run
  `FStratGuidedOpening::Observe` — "THE ONLY THING THAT ADVANCES A BEAT" — for a question with no
  screen behind it. The invariant to hold when editing that class is narrower than "always
  decorate": *no model this class builds reaches `ApplyView` without passing through
  `BuildViewModelForPresentation`.*
- **`AStratPlayerController::RefreshFromMachine` calls `DecorateForPresentation` DIRECTLY rather than
  going through `BuildViewModelForPresentation`, and the asymmetry is deliberate.** The registration
  is taken in `BeginPlay`, so a controller driven before or without `BeginPlay` — which is every
  fixture that spawns one into a world it built itself — would silently stop decorating, and this
  path's whole purpose would come to rest on an actor lifecycle step. It cannot double-decorate:
  `BuildViewModel` never runs the delegate and this path never calls the presentation build.
- **`FStratViewDecorator` is single, weak, and cleared BY POSITION rather than by identity.**
  `AStratPlayerController::EndPlay` calls `ClearViewDecorator()` unconditionally, so in a world where
  a SECOND controller had since registered it would drop that controller's binding. No such world
  exists this milestone — §2.11's hot seat is one controller holding two seats, and
  `FStratGuidedOpening::Begin` takes the guided side by argument for exactly that reason.
  **Discharged by** the day a second controller can exist: the seam grows an owner argument, or the
  delegate an identity check, and the `EndPlay` call passes `this`.
- **`AStratScoreboardHUD` now holds seven guidance members, not four, and one of them is a cache it
  had explicitly declined to hold.** `DeliverLatestGuidance`, `LastPushedGuidance` and
  `bGuidanceEverPushed` joined `GuidanceWidgetClass`, `GuidanceStrip`, `CreateGuidanceWidget` and
  `PushGuidance`. The header's discharge condition is unchanged and the count in it is restated
  rather than left to be recounted: when a §2.11 UI-layer owner exists, all seven move there
  unchanged. The cache does not make this class a second thing that runs matches — it never asks for
  guidance, touches no bridge and no view model, has one writer, and every later push overwrites it,
  so it changes WHEN a value is delivered and never WHICH.
- **`StratScoreboardHUD.h` now includes `StratViewModel.h`, where it forward declared
  `FStratGuidanceView`.** `LastPushedGuidance` is held by value, so the size is needed by UHT.
  Nothing about the constraint that governs that header is loosened: `StratBridge.h` remains
  forbidden there, forever, and `StratViewModel.h` is this module's own reflected header with nothing
  vendored behind it.
