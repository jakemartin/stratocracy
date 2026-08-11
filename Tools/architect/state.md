# Architect state

_Last run 2026-08-11 17:07 UTC._

## BUILT

- `Source/StratBridge/StratBridge.h` — applied (iteration 1, step `bridge_view_model`, replay)
- `Source/StratBridge/StratBridge.cpp` — applied (iteration 1, step `bridge_view_model`, replay)
- `Source/StratUI/StratScoreboardWidget.h` — applied (iteration 2, step `scoreboard_h`, replay)
- `Source/StratUI/StratScoreboardWidget.cpp` — applied (iteration 2, step `scoreboard_cpp`, replay)
- `Source/StratUI/Tests/StratScoreboardParity.cpp` — applied (iteration 2, step `scoreboard_test`, replay)

## DECISIONS

- iteration 1: selected `bridge_view_model` at score 95; runner-up scoreboard_widget was blocked on gamestate_to_uiworld
- iteration 2: selected `scoreboard_widget` at score 65; runner-up production_widget was blocked on buildlist_query

## NEXT

- `presentation_statelessness` — Presentation statelessness pass (rebuild widgets from the view model) (actionable)
- `production_widget` — Production menu widget (§2.11.5) (blocked on buildlist_query)
- `bridge_event_list` — Bridge ordered event list (§4.9 'command in / events out') (actionable)
- `buildlist_query` — Buildlist query on the Ui.h contract (actionable, excluded: shape unstated in the GDD by explicit decision, and the file is vendored certified bytes in another repo -- T-INT-01 hash-matches it)
