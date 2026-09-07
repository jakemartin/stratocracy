# Reads the round-change cue and its bank slot back IN A COLD PROCESS THAT WRITES NOTHING.
#
# WHY IT IS A SEPARATE SCRIPT AND A SEPARATE COMMANDLET. `make_round_change_cue.py` ends with a
# readback of its own, and that readback is worth very little: it runs in the process that did the
# writing, so it reads MEMORY rather than the bytes on disk, and a `set_editor_property` return is
# not evidence -- this project has measured a NeoStack `set()` reporting success in both
# directions while the saved bytes disagreed. Only a process that never touched these assets can
# say what was actually written. This one loads and prints, and does not save.
#
# ROUTE. Same as the authoring script, run afterwards:
#   "C:/Program Files/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" \
#     "E:/MultiAgent/Stratocracy/Stratocracy.uproject" -run=pythonscript \
#     -script="E:/MultiAgent/Stratocracy/Tools/editor/verify_round_change_cue.py" \
#     -unattended -nopause -nosplash -nullrhi
#
# IT WALKS ALL EIGHT SLOTS AND NOT ONLY THE NEW ONE, WHICH IS THE CONTROL. A report that only
# read `player_turn_began` and found it filled would look identical whether the other seven had
# survived the pass or been cleared by it, and the enum's values SHIFTED when the new arm was
# inserted -- `MatchEnded` moved -- so "the other seven are untouched" is exactly the thing this
# pass could plausibly have broken. Seven known-good readings beside the new one are what make
# the eighth mean something.
#
# IT STILL PROVES NOTHING ABOUT SOUND. A commandlet has no audio device. Every line below is
# about bytes and object references.

import unreal
import io
import os
import traceback

REPORT_PATH = r"E:/MultiAgent/Stratocracy/Saved/StratAudioBuild/round_change_verify.txt"
BANK = "/Game/StratAudio/DA_StratSoundBank.DA_StratSoundBank"

# In `EStratSoundCue` declaration order, which is also the order the parity clause walks.
SLOTS = [
    "button_click",
    "turn_ended",
    "player_turn_began",
    "unit_moved",
    "unit_attacked",
    "unit_destroyed",
    "factory_built_unit",
    "match_ended",
]

LOG = []


def say(s):
    LOG.append(str(s))


def flush():
    os.makedirs(os.path.dirname(REPORT_PATH), exist_ok=True)
    io.open(REPORT_PATH, "w", encoding="utf-8").write("\n".join(LOG) + "\n")


def main():
    say("STRAT ROUND-CHANGE COLD READBACK")
    say("this process has written nothing")
    say("")

    bank = unreal.EditorAssetLibrary.load_asset(BANK)
    if bank is None:
        say("FAIL    the bank did not load: %s" % BANK)
        return

    filled = 0
    for slot in SLOTS:
        try:
            obj = bank.get_editor_property(slot)
        except Exception as e:
            say("NOSLOT  %-20s %s" % (slot, e))
            continue
        if obj is None:
            say("UNSET   %-20s" % slot)
            continue
        cls = obj.get_editor_property("sound_class_object")
        say("SET     %-20s -> %-24s class=%-16s soundClass=%s"
            % (slot, obj.get_name(), obj.get_class().get_name(),
               cls.get_name() if cls else "<UNSET>"))
        filled += 1

    say("")
    say("%d of %d slots filled" % (filled, len(SLOTS)))

    # The cue's own package, read independently of the bank -- a slot can point at an object that
    # was never saved to disk, and that is the case this line separates from a real one.
    path = "/Game/StratAudio/MS_Strat_RoundChange.MS_Strat_RoundChange"
    say("")
    say("cue package exists on disk: %s"
        % unreal.EditorAssetLibrary.does_asset_exist(path))


try:
    main()
except Exception:
    say("TOP-LEVEL FAILURE\n" + traceback.format_exc())
flush()
