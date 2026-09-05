# Adds the two options buttons and reparents the title menu, headlessly.
#
# ROUTE. No editor. NOTE `-SCCProvider=None` -- it is not optional:
#   "C:/Program Files/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" \
#     "E:/MultiAgent/Stratocracy/Stratocracy.uproject" -run=pythonscript \
#     -script="E:/MultiAgent/Stratocracy/Tools/editor/add_options_buttons.py" \
#     -unattended -nopause -nosplash -nullrhi -SCCProvider=None
# MEASURED 2026-09-05: with the Git provider active, asset operations fail and the log carries
# "Operation 'CheckOut' not supported by revision control provider 'Git'". Disabling it for the
# commandlet also stops the provider auto-staging every package it saves.
#
# WHY C++ PARENTS AND NOT GRAPH WIRING. There is no Python route to a widget GRAPH, so a button
# added here can carry no OnClicked of its own. Both buttons are bound natively by their C++
# parent class, and both are `BindWidgetOptional` -- required binds would have reddened each
# asset's compile the moment the C++ landed and before this script ran.
#
# THE TITLE MENU IS REPARENTED AND THAT BAKES IRREVERSIBLY. A Blueprint stores its parent's
# script path; `/Script/StratPlay.StratShellMenuWidget` is written into the asset and changing it
# later is asset surgery. The class is in `StratPlay` and NOT `StratUI` because the arrow runs
# `StratPlay -> StratUI` and never back: a StratUI widget cannot name `UStratShellSubsystem` or
# `EStratShellRoute`. `Btn_0..Btn_3` and their existing graph bindings are untouched by this.
#
# WHAT THIS CANNOT PROVE: that either button is visible, hittable, or sensibly placed. No pixels
# in a commandlet. A human is the only instrument for that.

import unreal
import io
import os
import traceback

REPORT = r"E:/MultiAgent/Stratocracy/Saved/StratAudioBuild/options_buttons.txt"
L = []
def p(s): L.append(str(s))


def flush():
    os.makedirs(os.path.dirname(REPORT), exist_ok=True)
    io.open(REPORT, "w", encoding="utf-8").write("\n".join(L) + "\n")


def mk(tree, cls, name):
    o = unreal.new_object(cls, outer=tree, name=name)
    if o is None:
        raise Exception("new_object failed: %s %s" % (cls, name))
    return o


def add_button(asset_path, container_name, button_name, label_name, label_text):
    """Adds `button_name` (with a text child) into `container_name`. Idempotent."""
    p("=== %s ===" % asset_path)
    wb = unreal.EditorAssetLibrary.load_asset(asset_path)
    tree = unreal.load_object(wb, "WidgetTree")
    if tree is None:
        raise Exception("no WidgetTree")
    if unreal.load_object(tree, button_name) is not None:
        p("  %s already present; leaving it alone" % button_name)
        return wb
    container = unreal.load_object(tree, container_name)
    p("  container %r -> %s" % (container_name,
                                container.get_class().get_name() if container else "<MISSING>"))
    if container is None:
        raise Exception("container %r not found" % container_name)
    p("  children before: %d" % container.get_children_count())

    btn = mk(tree, unreal.Button, button_name)
    txt = mk(tree, unreal.TextBlock, label_name)
    # Design-time text only. `UStratShellMenuWidget` overwrites the title-menu label from the
    # model's own row at Construct; the command bar's is static.
    txt.set_text(unreal.Text(label_text))
    btn.add_child(txt)
    container.add_child(btn)
    p("  added %s (with %s); children after: %d"
      % (button_name, label_name, container.get_children_count()))
    return wb


def main():
    p("OPTIONS BUTTONS")
    p("")

    # 1. The in-match button, beside BuildButton and EndTurnButton in the `Bar` HorizontalBox.
    cb = add_button("/Game/UI/WBP_CommandBar", "Bar", "OptionsButton",
                    "OptionsButtonLabel", "OPTIONS")
    # COMPILE BEFORE SAVING, ALWAYS, AND THE ORDER IS THE WHOLE POINT. A widget made by
    # `new_object` has no entry in the blueprint's `WidgetVariableNameToGuidMap` -- that map is a
    # PROTECTED UPROPERTY and cannot be written from here -- so the FIRST compile is what assigns
    # the GUIDs, and it ensures while doing it: "Widget [OptionsButton] was added but did not get
    # a GUID". Saving AFTER that compile persists the populated map and every later compile is
    # clean. MEASURED 2026-09-05: saving without compiling left the asset ensuring on every load,
    # which reddened an unrelated sound-bank clause in the suite, because the automation
    # framework counts a handled ensure as a test error.
    unreal.BlueprintEditorLibrary.compile_blueprint(cb)
    unreal.EditorAssetLibrary.save_asset("/Game/UI/WBP_CommandBar", only_if_is_dirty=False)
    p("  compiled, then saved")
    p("")

    # 2. The title menu: reparent FIRST, then add. Reparent triggers a compile, and a compile
    # before the widget exists is exactly what produced the misleading "required widget binding
    # not found" errors while WBP_Options was being built -- harmless here because both binds
    # are optional, but the order is deliberate rather than accidental.
    p("=== /Game/UI/WBP_TitleMenu : REPARENT ===")
    tm = unreal.EditorAssetLibrary.load_asset("/Game/UI/WBP_TitleMenu")
    before = unreal.get_default_object(tm.generated_class())
    p("  CDO is StratShellMenuWidget before: %s"
      % isinstance(before, unreal.StratShellMenuWidget))
    unreal.BlueprintEditorLibrary.reparent_blueprint(tm, unreal.StratShellMenuWidget)
    p("  reparent_blueprint returned without exception")

    add_button("/Game/UI/WBP_TitleMenu", "Stack", "OptionsButton", "OptionsLabel", "OPTIONS")
    unreal.BlueprintEditorLibrary.compile_blueprint(tm)
    unreal.EditorAssetLibrary.save_asset("/Game/UI/WBP_TitleMenu", only_if_is_dirty=False)
    p("  saved")
    p("")

    # 3. The options widget class on every shipped GameMode. Setting only one gives a volume
    # screen in one world and not the other, silently -- the engineer recorded that as a debt.
    p("=== OptionsWidgetClass on the shipped GameModes ===")
    opts = unreal.EditorAssetLibrary.load_asset("/Game/UI/WBP_Options")
    opts_class = opts.generated_class()
    p("  class to assign: %s" % opts_class.get_name())
    for bp_path in ["/Game/StratPlay/BP_StratGameMode",
                    "/Game/StratPlay/BP_StratGameMode_AiVsAi",
                    "/Game/StratPlay/BP_StratShellGameMode"]:
        bp = unreal.EditorAssetLibrary.load_asset(bp_path)
        cdo = unreal.get_default_object(bp.generated_class())
        was = cdo.get_editor_property("options_widget_class")
        cdo.set_editor_property("options_widget_class", opts_class)
        now = cdo.get_editor_property("options_widget_class")
        unreal.EditorAssetLibrary.save_asset(bp_path, only_if_is_dirty=False)
        p("  %-38s %s -> %s" % (bp_path.split("/")[-1],
                                was.get_name() if was else "<UNSET>",
                                now.get_name() if now else "<UNSET>"))


try:
    main()
except Exception:
    p("TOP-LEVEL FAILURE\n" + traceback.format_exc())
flush()
