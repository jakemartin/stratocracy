# Acceptance diff — 20260811-165313

Agent output on the left, the tree as it stands on the right.

## `Source/StratUI/StratScoreboardWidget.cpp`

Accepted unchanged.

## `Source/StratUI/StratScoreboardWidget.h`

Accepted unchanged.

## `Source/StratUI/Tests/StratScoreboardParity.cpp`

```diff
--- agent/Source/StratUI/Tests/StratScoreboardParity.cpp
+++ tree/Source/StratUI/Tests/StratScoreboardParity.cpp
@@ -259,11 +259,16 @@
 	 *
 	 * THE ONE PLACE THIS FILE NAMES THAT FUNCTION'S RESULT SHAPE, on purpose. Every
 	 * clause below calls this helper and none of them mentions how a failure is spelt,
-	 * so if `Ui.h` reports failures differently from the shape assumed here -- a
-	 * returned list of records carrying `id` and `reason`, mirroring `SaveLoadResult`'s
-	 * `failedId` / `reason` pair -- this function is the entirety of the reconciliation
-	 * and no clause changes. The iteration deliberately uses `auto` rather than naming
-	 * the failure struct, so the type name is one fewer thing to keep in step.
+	 * so a change to `Ui.h`'s reporting is reconciled here and no clause moves.
+	 *
+	 * `uiCheckSnapshotFidelity` returns a `UiFidelityResult` -- a STRUCT carrying `ok`,
+	 * three counters, and a `failures` vector of `{clause, field, detail}`. It is not
+	 * itself iterable, and the three failure members are not `id`/`reason`.
+	 *
+	 * `Result.ok` IS CHECKED AND NOT INFERRED FROM AN EMPTY FAILURE LIST. Ui.h states
+	 * that clause (c) fails a snapshot field carrying no contract entry; treating "no
+	 * failures listed" as success would make this helper agree with a result that said
+	 * otherwise, which is the same class of defect T-UI-05 exists to catch.
 	 *
 	 * FAILURES ARE FORWARDED, NOT SUMMARISED. A fidelity failure is the rules module
 	 * saying a projected field does not match the state it projects, and the module's
@@ -276,12 +281,14 @@
 		TArray<FString>&         OutFailures)
 	{
 		OutFailures.Reset();
-		for (const auto& Failure : strat::uiCheckSnapshotFidelity(World, Snapshot))
-		{
-			OutFailures.Add(FString::Printf(TEXT("%s: %s"),
-				*FromStd(Failure.id), *FromStd(Failure.reason)));
+		const strat::UiFidelityResult Result =
+			strat::uiCheckSnapshotFidelity(World, Snapshot);
+		for (const strat::UiFidelityFailure& Failure : Result.failures)
+		{
+			OutFailures.Add(FString::Printf(TEXT("clause (%s) %s: %s"),
+				*FromStd(Failure.clause), *FromStd(Failure.field), *FromStd(Failure.detail)));
 		}
-		return OutFailures.Num() == 0;
+		return Result.ok && OutFailures.Num() == 0;
 	}
 
 	/**
```
