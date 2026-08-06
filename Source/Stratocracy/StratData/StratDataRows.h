// GDD §4.8 — the data contract, engine side.
//
// "Authored once, read twice, proven equal." The canonical CSVs live in the crew
// repo under `data/` and are vendored into this project by `sync_stratdata.py`
// (crew) into `Data/`. The headless loader (`strat::Data`, StratRules) parses
// those bytes directly; the editor imports the SAME bytes into a UDataTable whose
// row struct is declared here. T-DATA-05 iterates every imported row and asserts
// it equals the CSV field-for-field, so neither side can drift from the other
// without a gate saying so.
//
// These types are deliberately NOT in Source/StratRules/. T-INT-01 asserts every
// file in that directory is accounted for against the crew commit and none is
// unaccounted for, so adding a file there breaks a green acceptance ID. They are
// also engine types (USTRUCT/UENUM), which the vendored sources may not be:
// T-INT-04 asserts those compile standalone with zero engine dependencies.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StratDataRows.generated.h"

/**
 * Mirror of `strat::UnitType` (StratRules, Combat.h), enumerator order pinned.
 *
 * The order is load-bearing rather than cosmetic: the type-effectiveness matrix
 * (§4.8, `data/effectiveness.csv`) is indexed positionally, so an editor-side
 * reorder would silently reindex every cell. T-DATA-05 asserts the mirror is
 * exact; StratDataParity.cpp additionally static_asserts the value
 * correspondence, so a reorder of EITHER enum stops the build rather than
 * waiting for the gate to run.
 */
UENUM(BlueprintType)
enum class EUnitType : uint8
{
	Infantry = 0	UMETA(DisplayName = "Infantry"),
	Tank = 1		UMETA(DisplayName = "Tank"),
	Artillery = 2	UMETA(DisplayName = "Artillery"),
	Recon = 3		UMETA(DisplayName = "Recon")
};

/**
 * `data/units.csv` -> headless `strat::UnitDef` -> this. Exactly four rows
 * (Infantry, Tank, Artillery, Recon; §2.4).
 *
 * The flag unit is NOT a row: §2.4 defines it as "a designated Tank", so flag
 * status is a placement-level field in the scenario file (`isFlag`, §4.7 Stub 7)
 * gated by T-SCN-01, not a fifth unit type.
 *
 * `MoveClass` is reserved in §4.8 and blocked on Q2, so it is absent from both
 * the CSV and this struct. A reserved column is not a declared one; adding it
 * here before Q2 is ruled would make the parity gate assert over a field the
 * canonical file does not carry.
 */
USTRUCT(BlueprintType)
struct FUnitRow : public FTableRowBase
{
	GENERATED_BODY()

	/** §2.4 — 10 / 20 / 8 / 12. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 HP = 0;

	/** §2.4 — 3 / 5 / 3 / 7. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 Move = 0;

	/** §2.4 — 4 / 8 / 10 / 5. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 Atk = 0;

	/** §2.4 — 2 / 5 / 1 / 3. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 Def = 0;

	/** §2.4 — Artillery 2, others 1. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 RangeMin = 0;

	/** §2.4 — Artillery 3, others 1. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 RangeMax = 0;

	/** §2.4 — 100 / 300 / 200 / 150. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	int32 CostFame = 0;

	/** Pinned order, mirrored from `strat::UnitType`. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	EUnitType Type = EUnitType::Infantry;

	/** §2.7 — Infantry only; T-DATA-03 asserts exactly one row is true. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	bool CanCapture = false;
};

/**
 * `data/terrain.csv` -> headless `strat::TerrainDef` -> this. Exactly seven rows
 * (§2.3).
 */
USTRUCT(BlueprintType)
struct FTerrainRow : public FTableRowBase
{
	GENERATED_BODY()

	/** §2.3 — 0 means impassable, never "free". Water ships 0. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	int32 MoveCost = 0;

	/** §2.3 — signed; Bridge ships -10, which is why this is not unsigned. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	int32 DefensePct = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	bool PassLand = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	bool PassAir = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	bool PassSea = false;

	/** §2.3 — Town and Factory. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	bool Capturable = false;

	/** §2.7 — Factory 100, Town 25, else 0. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	int32 IncomeFame = 0;

	/** §2.7 — Factory. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	bool IsSpawnPoint = false;

	/** §2.7 Repair — Town and Factory. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Terrain")
	bool IsRepairPoint = false;
};

/**
 * `data/effectiveness.csv` -> `strat::effectiveness` -> this. A 4x4 matrix,
 * row = attacker type, columns = defender types, values in {0.5, 1.0, 1.5}.
 *
 * The SHIPPED file is all-1.0 (§2.4 — the triangle stays positional), which
 * T-DATA-06 asserts and T-COMBAT-09 independently pins at the code layer. This
 * struct is the lever's data form: a non-neutral cell is a CSV edit the Director
 * approves, not a code change.
 */
USTRUCT(BlueprintType)
struct FEffectivenessRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effectiveness")
	float Infantry = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effectiveness")
	float Tank = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effectiveness")
	float Artillery = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effectiveness")
	float Recon = 1.0f;
};
