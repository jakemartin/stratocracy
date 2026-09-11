---
name: stratplay
description: Work in Source/StratPlay — the gameplay module (board and unit actors, camera pawn, player controller, selection machine, match and shell subsystems, AI playback). Covers why each dependency arrow has the strength it has, the link failures that arrive as missing functions, and the C++-default-versus-Blueprint-default trap. Use when editing StratPlay, changing StratPlay.Build.cs, or diagnosing an LNK2019 on UnrealEditor-StratPlay.dll.
---

# Source/StratPlay — the gameplay module

## Purpose

The actors, input and match flow that turn the rules into a playable board. The arrow is
`Stratocracy → StratBridge → StratUI → StratPlay`; nothing depends on `StratPlay`, and it is
reached from asset data (`BP_StratGameMode`, the levels), which is why it must be listed in
`Stratocracy.uproject`'s `Modules` array.

**`StratPlay` never names a `strat::` type** — `FIntPoint` and `int32` only. Every rules answer
arrives through an `FStratBridge` method. The project-wide rules this rests on (no `strat::` call
outside `StratBridge`, no `StratBridge.h` in a reflected header, `TPimplPtr` not `TUniquePtr`) are
in `CLAUDE.md` and `.agents/ue-project-context.md`, and are not restated here.

## Dependency strength is load-bearing here

`.agents/ue-project-context.md`'s module table lists what `StratPlay` depends on. It flattens
**strength**, and on this module the strength is the point. Read `StratPlay.Build.cs`'s header
block before changing any of it.

- **`StratBridge` is `Private`, deliberately.** It was moved Public → Private closing a phase-3
  gate finding: nothing in this module's headers names `FStratBridge` beyond a forward
  declaration, so `Public` re-exported the arrow to every dependent for no caller's benefit.
- **It is named at all**, rather than arriving transitively through `StratUI` (which declares it
  `Public`), because of a measurement recorded in `StratPlay.Build.cs`: **4 × `LNK2019`** on
  `UnrealEditor-StratPlay.dll`, naming `__imp_?LoadDefinitions@FStratBridge@@`,
  `__imp_?LoadScenarioFromFile@FStratBridge@@`, and the imported constructor and destructor. The
  transitive arrow carried the include paths; it did not carry `UnrealEditor-StratBridge.lib` onto
  the link line. **A module that CALLS a symbol declares the module that exports it.** `IsSeeded()`
  and `GetBridge()` are inline in the header and linked fine — which is exactly how the omission
  would survive a smaller caller.
- **`EnhancedInput` is `Private`.** `AStratPlayerController` is the only file including an Enhanced
  Input header; its asset properties are forward-declared `TObjectPtr`s.
- **`InputCore` is absent deliberately, and one `FKey` call brings it back.** `FKey::ToString` and
  `FKey::IsGamepadKey` are `INPUTCORE_API` rather than inline; omitting the module was measured as
  2 × `LNK2019` then `LNK1120`. The declarations resolve fine — `InputCoreTypes.h` arrives
  transitively through `Engine` — so the failure reads as a missing function, not a missing module.

## Gotchas

- **A transitive include path is not a link dependency.** See the 4 × `LNK2019` above.
- **A C++ field default is not the shipped value.** The shipped Blueprints (`BP_StratUnit`,
  `BP_StratGameMode`, …) override many `EditDefaultsOnly` fields — `MoveTweenSeconds`,
  `AiPlaybackStepSeconds` and `DamageFlashSeconds` among them. A comment that calls the C++ `<= 0`
  "the shipped default" is false while the Blueprint sets a value. Say "C++ field default", and read
  the Blueprint default before claiming what ships. **No phrase grep finds every instance of this
  claim.** The sites are spelt in capitals, wrapped across lines, or worded as "ships with", and a
  case-sensitive grep for "shipped default" plus "ships inert" missed all four of the sites the
  record held open on 2026-09-11. `grep -rniE "\bship(ped|s)\b"` does reach all four, but it
  returns hundreds of lines across `StratPlay` and `StratUI`. Treat it as a reading list, not a
  check, and never read an empty result from a narrower pattern as a clean module.
- **`StratPlay → StratUI` never reverses.** `StratUI` cannot name a `StratPlay` type, so anything a
  widget needs to reach in this module is broadcast from `StratUI` and reconciled here, never called
  from the widget.
- **Presentation is reconciled, not evented.** `ApplyView` spawns, moves and destroys to match the
  view model on every refresh.
