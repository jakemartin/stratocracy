// Imports the vendored §4.8 CSVs into UDataTable assets. Editor-only, headless.
//
// This exists as a C++ commandlet rather than an editor Python script because
// PythonScriptPlugin does not execute on this machine: the plugin mounts and its
// DLL loads, but neither `-run=pythonscript` nor `-ExecutePythonScript` nor
// `-ExecCmds="py ..."` produces any LogPython output at all, proven with a
// two-line probe script that only calls unreal.log(). A commandlet compiles with
// the module the gate already needs, so the import path has one fewer moving
// part than the gate it feeds.
//
// Run with:
//   UnrealEditor-Cmd.exe <uproject> -run=ImportStratData -unattended -nosplash

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "ImportStratDataCommandlet.generated.h"

UCLASS()
class UImportStratDataCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UImportStratDataCommandlet();

	virtual int32 Main(const FString& Params) override;
};
