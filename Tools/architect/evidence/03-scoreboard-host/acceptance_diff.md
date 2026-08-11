# Acceptance diff — 20260811-192454

Agent output on the left, the tree as it stands on the right.

## `Source/StratUI/StratScoreboardHUD.cpp`

```diff
--- agent/Source/StratUI/StratScoreboardHUD.cpp
+++ tree/Source/StratUI/StratScoreboardHUD.cpp
@@ -264,7 +264,7 @@
 	// Constructed here rather than in the constructor: a bridge that exists before its
 	// inputs have been checked is a bridge GetBridge() could hand out unseeded, and
 	// BeginPlay is where the failure path can say why it did not survive.
-	Bridge = MakeUnique<FStratBridge>();
+	Bridge = MakePimpl<FStratBridge>();
 
 	// ROW ORDER IS LOAD-BEARING and is not this class's business -- `defIndex` indexes
 	// the vectors the bridge builds from these tables, and a §4.10 Build command
```

## `Source/StratUI/StratScoreboardHUD.h`

```diff
--- agent/Source/StratUI/StratScoreboardHUD.h
+++ tree/Source/StratUI/StratScoreboardHUD.h
@@ -43,14 +43,27 @@
 // destroy the property that file was written to hold. StratScoreboardWidget.h refuses
 // the same include for the same reason, and StratUI.Build.cs and StratUI.h both record
 // the constraint from the module side. `FStratBridge` is therefore forward declared,
-// held as `TUniquePtr<FStratBridge>`, and included ONLY from the .cpp.
-//
-// THE CONSEQUENCE OF THAT FORWARD DECLARATION, spelled out because it is easy to undo
-// by accident: `TUniquePtr` instantiates its deleter where the destructor is defined,
-// and a deleter cannot be instantiated against an incomplete type. So the constructor
-// AND destructor are declared here and defined in the .cpp, after the include. Deleting
-// either declaration, or writing `= default` on it here, breaks the build in a way
-// whose error message points at TUniquePtr rather than at this decision.
+// held as `TPimplPtr<FStratBridge>`, and included ONLY from the .cpp.
+//
+// WHY TPimplPtr AND NOT TUniquePtr, which is what this class held first and which does
+// not work here. `TUniquePtr` instantiates its deleter wherever the owning destructor
+// is instantiated -- and declaring this class's own constructor and destructor
+// out-of-line is NOT sufficient, because UHT emits a vtable-helper constructor
+// (`DEFINE_VTABLE_PTR_HELPER_CTOR`) into `Module.StratUI.gen.cpp`, a translation unit
+// that includes this header and never includes StratBridge.h. That generated file is
+// where the deleter got instantiated against the incomplete type. MEASURED: error
+// C4150, "deletion of pointer to incomplete type 'FStratBridge'", reported from
+// UniquePtr.h and compiled from the .gen.cpp, not from anything written by hand.
+//
+// `TPimplPtr` is UE's own answer to exactly this: its header says the deleter is
+// "determined at binding time and type-erased, allowing the object to be deleted
+// without access to the definition of the type". The type erasure happens at the
+// `MakePimpl` call site in the .cpp, where the definition IS available, so no
+// translation unit that merely sees this header ever needs it.
+//
+// The constructor and destructor remain declared here and defined in the .cpp. They are
+// no longer load-bearing for the deleter, and they are kept because the constructor has
+// a body worth reading and a matching pair is easier to keep honest than a lone one.
 //
 // NO CROSS-MODULE `strat::` CALLS FROM THIS FILE OR ITS .cpp. The vendored sources
 // carry no _API macro -- §4.9 forbids them engine headers -- so
@@ -96,7 +109,7 @@
 #include "CoreMinimal.h"
 #include "GameFramework/HUD.h"
 #include "Templates/SubclassOf.h"
-#include "Templates/UniquePtr.h"
+#include "Templates/PimplPtr.h"
 
 #include "StratScoreboardHUD.generated.h"
 
@@ -126,8 +139,8 @@
 
 public:
 	// Declared, defined in the .cpp. Not `= default` here, and not omitted: the
-	// TUniquePtr member below holds an incomplete type, and its deleter is
-	// instantiated wherever the destructor body lands. See the header block.
+	// definition of FStratBridge is only available in the .cpp. See the header
+	// block for why the member is a TPimplPtr and not a TUniquePtr.
 	AStratScoreboardHUD();
 	virtual ~AStratScoreboardHUD() override;
 
@@ -305,7 +318,7 @@
 	 * The authoritative `strat::GameState`, one level of indirection down.
 	 *
 	 * NOT A UPROPERTY -- `FStratBridge` is not a reflected type and must not become one.
-	 * Held by TUniquePtr rather than by value for exactly that reason: a by-value member
+	 * Held by TPimplPtr rather than by value for exactly that reason: a by-value member
 	 * would require the full definition here, and the full definition is the include
 	 * this whole file is arranged to refuse.
 	 *
@@ -313,5 +326,5 @@
 	 * call and copies engine-typed values out of the snapshot, so teardown order between
 	 * the two is not something anyone has to get right.
 	 */
-	TUniquePtr<FStratBridge> Bridge;
+	TPimplPtr<FStratBridge> Bridge;
 };
```
