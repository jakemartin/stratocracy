# Builds /Game/UI/WBP_Options -- the volume screen's widget tree -- headlessly.
#
# ROUTE. No editor required:
#   "C:/Program Files/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" \
#     "E:/MultiAgent/Stratocracy/Stratocracy.uproject" -run=pythonscript \
#     -script="E:/MultiAgent/Stratocracy/Tools/editor/make_options_wbp.py" \
#     -unattended -nopause -nosplash -nullrhi
#
# WHY THIS DUPLICATES A DONOR INSTEAD OF CREATING AN ASSET. MEASURED 2026-09-05, and it is the
# whole reason this file has the shape it does. `UWidgetTree::RootWidget` is a PROTECTED
# UPROPERTY -- Python reports "Property 'RootWidget' for attribute 'RootWidget' on 'WidgetTree'
# is protected and cannot be read" -- so a freshly created WidgetBlueprint has a tree with NO
# ROOT and there is no way from here to give it one. Everything else works: `new_object(Slider,
# outer=tree)` succeeds, properties set, and `add_child` / `add_child_to_canvas` /
# `clear_children` are Python methods on PanelWidget. So the move is to duplicate a widget that
# ALREADY HAS a root and build into it. `make_scoreboard_wbp.py` records the same wall from the
# other side and builds no tree at all.
#
# WHY THERE IS NO GRAPH WIRING HERE, AND WHY THAT IS THE BETTER DESIGN. Widget GRAPH authoring
# has no Python route either, so nothing here can connect a slider's OnValueChanged to anything.
# `UStratOptionsWidget` therefore declares `BindWidget` members and does its own binding in
# `NativeConstruct`. The names below are that class's contract; a mismatch is a COMPILE FAILURE
# of the widget, which is the loud failure and the one we want.
#
# WHAT THIS CANNOT PROVE: that the screen is usable, legible, or laid out sensibly. There are no
# pixels in a commandlet. A human at the keyboard is the only instrument for that.

import unreal
import io
import os
import traceback

REPORT = r"E:/MultiAgent/Stratocracy/Saved/StratAudioBuild/options_wbp.txt"
DONOR = "/Game/UI/WBP_PreMatchBriefing"   # has a `Root` CanvasPanel and the emptiest graph here
TARGET = "/Game/UI/WBP_Options"

L = []
def p(s): L.append(str(s))


def flush():
    os.makedirs(os.path.dirname(REPORT), exist_ok=True)
    io.open(REPORT, "w", encoding="utf-8").write("\n".join(L) + "\n")


# name -> (class, is required by BindWidget). The three labels are BindWidgetOptional; the
# sliders and the button are hard binds, so a wrong name here fails the widget's compile.
CHANNELS = [
    ("Master", "MasterSlider", "MasterValueText"),
    ("Sfx", "SfxSlider", "SfxValueText"),
    ("Music", "MusicSlider", "MusicValueText"),
]


def mk(tree, cls, name):
    o = unreal.new_object(cls, outer=tree, name=name)
    if o is None:
        raise Exception("new_object failed for %s %s" % (cls, name))
    # `bIsVariable` CANNOT BE SET FROM HERE -- it is a PROTECTED UPROPERTY, the same wall as
    # `UWidgetTree::RootWidget`, and Python reports "protected and cannot be read" even on a
    # designer-made control widget. It turns out not to be needed: BindWidget resolves by
    # searching the widget tree for a widget of the matching NAME, and the binding errors seen
    # while developing this script all came from the compiles that `reparent_blueprint` triggers
    # BEFORE the tree is built, not from the final one. The verification step is a separate
    # process that compiles the SAVED asset and reports only that process's errors.
    return o


def build():
    p("=== DUPLICATE ===")
    # RUN WITH `-SCCProvider=None`. MEASURED 2026-09-05: with the Git provider active,
    # `duplicate_asset` returns None and the log carries "Operation 'CheckOut' not supported by
    # revision control provider 'Git'". Disabling the provider for the commandlet also stops it
    # auto-staging every package it saves, which has had to be undone by hand four times.
    if unreal.EditorAssetLibrary.does_asset_exist(TARGET):
        wb = unreal.EditorAssetLibrary.load_asset(TARGET)
        p("  target already exists; reusing %r" % wb)
    else:
        wb = unreal.EditorAssetLibrary.duplicate_asset(DONOR, TARGET)
        p("  %s -> %s : %r" % (DONOR, TARGET, wb))
        if wb is None:
            raise Exception("duplicate_asset returned None (is the SCC provider disabled?)")

    p("")
    p("=== REPARENT to UStratOptionsWidget ===")
    # `parent_class` is NOT an exposed property name on WidgetBlueprint -- measured. The
    # documented route is BlueprintEditorLibrary; the property spellings are kept as fallbacks
    # and each is reported, so a silent no-op is impossible.
    done = False
    try:
        unreal.BlueprintEditorLibrary.reparent_blueprint(wb, unreal.StratOptionsWidget)
        p("  reparent_blueprint: returned without exception")
        done = True
    except Exception as e:
        p("  reparent_blueprint FAILED: %s" % str(e).splitlines()[0])
    if not done:
        for spell in ("ParentClass", "parent_class"):
            try:
                wb.set_editor_property(spell, unreal.StratOptionsWidget)
                p("  set_editor_property(%r): OK" % spell)
                done = True
                break
            except Exception as e:
                p("  set_editor_property(%r) FAILED: %s" % (spell, str(e).splitlines()[0]))
    if not done:
        raise Exception("no route reparented the blueprint")

    p("")
    p("=== TREE ===")
    tree = unreal.load_object(wb, "WidgetTree")
    p("  tree -> %r" % tree)
    root = unreal.load_object(tree, "Root")
    p("  Root -> %r" % root)
    if root is None:
        raise Exception("donor had no `Root` canvas; pick a different donor")

    # The donor's own contents are not ours. Clearing is what makes this a blank canvas with a
    # ROOT rather than a copy of a briefing screen.
    root.clear_children()
    p("  cleared donor children; root now has %d" % root.get_children_count())

    stack = mk(tree, unreal.VerticalBox, "Stack")
    slot = root.add_child_to_canvas(stack)
    try:
        slot.set_editor_property("anchors", unreal.Anchors(unreal.Vector2D(0.5, 0.5),
                                                           unreal.Vector2D(0.5, 0.5)))
        slot.set_editor_property("alignment", unreal.Vector2D(0.5, 0.5))
        slot.set_editor_property("auto_size", True)
    except Exception as e:
        p("  (slot layout partial: %s)" % e)
    p("  Stack added to Root")

    made = []
    for label, slider_name, value_name in CHANNELS:
        cap = mk(tree, unreal.TextBlock, "Label%s" % label)
        cap.set_text(unreal.Text(label.upper()))
        stack.add_child(cap)

        sl = mk(tree, unreal.Slider, slider_name)
        # MinValue/MaxValue are deliberately NOT set: NativeConstruct overwrites them with 0..1
        # and the C++ header records why. Setting them here would be a second author.
        stack.add_child(sl)

        val = mk(tree, unreal.TextBlock, value_name)
        val.set_text(unreal.Text("100%"))
        stack.add_child(val)
        made += [cap.get_name(), sl.get_name(), val.get_name()]

    back = mk(tree, unreal.Button, "BackButton")
    back_text = mk(tree, unreal.TextBlock, "BackLabel")
    back_text.set_text(unreal.Text("BACK"))
    back.add_child(back_text)
    stack.add_child(back)
    made += [back.get_name(), back_text.get_name()]

    p("  built: %s" % ", ".join(made))
    p("  Stack children: %d" % stack.get_children_count())
    return wb


def main():
    p("STRAT OPTIONS WIDGET BUILD")
    p("")
    wb = build()

    p("")
    p("=== COMPILE ===")
    try:
        unreal.BlueprintEditorLibrary.compile_blueprint(wb)
        p("  compile_blueprint returned (no exception)")
    except Exception as e:
        p("  compile FAILED/absent: %s" % e)

    unreal.EditorAssetLibrary.save_asset(TARGET, only_if_is_dirty=False)
    p("  saved")

    # READBACK. A duplicate that failed to reparent, or a tree whose names do not match the C++
    # contract, both LOOK like success up to here.
    p("")
    p("=== READBACK ===")
    o = unreal.EditorAssetLibrary.load_asset(TARGET)
    gc = o.generated_class()
    cdo = unreal.get_default_object(gc)
    p("  is StratOptionsWidget : %s" % isinstance(cdo, unreal.StratOptionsWidget))
    tree = unreal.load_object(o, "WidgetTree")
    for nm in ["Stack", "MasterSlider", "SfxSlider", "MusicSlider", "BackButton",
               "MasterValueText", "SfxValueText", "MusicValueText"]:
        child = unreal.load_object(tree, nm)
        p("  %-18s %s" % (nm, child.get_class().get_name() if child else "<MISSING>"))


try:
    main()
except Exception:
    p("TOP-LEVEL FAILURE\n" + traceback.format_exc())
flush()
