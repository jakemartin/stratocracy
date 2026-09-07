# Authors the round-change cue MetaSound and fills its slot on the shipped sound bank.
#
# WHY THIS EXISTS. `EStratSoundCue` gained an eighth arm for the moment the AI's turn ends and a
# human side's begins. `Source/StratPlay/Tests/StratShippedSoundBankParity.cpp` walks the enum by
# VALUE over `[0, Count)`, so the new arm entered three of its clauses the instant the C++ landed
# and they are red until this script runs. That is the whole job: one source, one slot.
#
# ROUTE. Headless, no running editor required -- the same route make_strat_sound_bank.py records:
#   "C:/Program Files/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" \
#     "E:/MultiAgent/Stratocracy/Stratocracy.uproject" -run=pythonscript \
#     -script="E:/MultiAgent/Stratocracy/Tools/editor/make_round_change_cue.py" \
#     -unattended -nopause -nosplash -nullrhi
#
# EVERY DESIGN DECISION BELOW IS THAT FILE'S, COPIED RATHER THAN RE-DERIVED, and the three that
# cost a measurement there are carried across so they are not re-measured here:
#   - REPORTS TO A FILE AND NEVER THROUGH `unreal.log`. In a -run=pythonscript commandlet
#     `unreal.log` reaches neither stdout nor the captured log, so a script that reported only
#     through it is indistinguishable from one that did nothing.
#   - `create_float_meta_sound_literal` RETURNS A 2-TUPLE, not a literal. Passing the tuple
#     through fails at the nativizer.
#   - NODE CLASS NAMES ARE RESOLVED, NEVER GUESSED. `add_node_by_class_name` on an unresolved
#     name yields an invalid handle and `build_to_asset` then SUCCEEDS ANYWAY on an empty graph.
#     `AD Envelope` puts its display name in the NAMESPACE slot; `Multiply`/`Add` take the DATA
#     TYPE as the variant.
#
# WHAT THIS SCRIPT CANNOT PROVE: THAT ANY OF IT IS AUDIBLE. A commandlet has no audio device and
# says so -- "Audio Device Manager not initializing due to all audio being disabled". It can prove
# a graph was built, wired and saved and that a slot points at it. It cannot prove an envelope
# opens or that a voice is not silent. A human at the keyboard is the only instrument for that,
# and no line of this file's output may be read as a claim about sound.

import unreal
import io
import os
import traceback

REPORT_PATH = r"E:/MultiAgent/Stratocracy/Saved/StratAudioBuild/round_change_report.txt"
PKG = "/Game/StratAudio"
AUTHOR = "Stratocracy"
CUE_NAME = "MS_Strat_RoundChange"
BANK = "/Game/StratAudio/DA_StratSoundBank.DA_StratSoundBank"
SFX_CLASS = "/Game/StratAudio/SCL_Strat_SFX.SCL_Strat_SFX"

# The `UStratSoundBank` slot, in the snake_case reflection spells it. READ OFF THE HEADER BEFORE
# THIS RAN, never assumed: a wrong name raises, which is loud, but a name that happened to match
# a DIFFERENT slot would be silent and would overwrite a working cue.
SLOT = "player_turn_began"

# A PERFECT FIFTH, LONGER THAN `EndTurn` AND SHORTER THAN `MatchEnd`, and the spacing is the
# point rather than the taste. On the shipped no-tour configuration `TurnEnded` and this cue both
# sound within a few lines of each other, so this one has to be TELLABLE from it: `EndTurn` is
# A4+E5 decaying in 0.35 s and `MatchEnd` is C5+E5 over 1.40 s. This sits between them in length
# and above both in pitch, which is what makes "your turn" read as an opening rather than a close.
FREQS = [523.25, 783.99]
ATTACK = 0.015
DECAY = 0.600

LOG = []


def say(s):
    LOG.append(str(s))


def flush():
    os.makedirs(os.path.dirname(REPORT_PATH), exist_ok=True)
    io.open(REPORT_PATH, "w", encoding="utf-8").write("\n".join(LOG) + "\n")


OK = unreal.MetaSoundBuilderResult.SUCCEEDED
SUBS = unreal.get_engine_subsystem(unreal.MetaSoundBuilderSubsystem)
EDS = unreal.get_editor_subsystem(unreal.MetaSoundEditorSubsystem)


class Fail(Exception):
    pass


def cn(namespace, name, variant):
    c = unreal.MetasoundFrontendClassName()
    c.set_editor_property("namespace", namespace)
    c.set_editor_property("name", name)
    c.set_editor_property("variant", variant)
    return c


CLASSES = {
    "Sine": ("UE", "Sine", "Audio"),
    "Multiply": ("UE", "Multiply", "Audio"),
    "Add": ("UE", "Add", "Audio"),
    "ADEnv": ("AD Envelope", "AD Envelope", "Audio"),
}


def add_node(b, key):
    ns, nm, va = CLASSES[key]
    node, res = b.add_node_by_class_name(cn(ns, nm, va))
    if res != OK:
        raise Fail("class name did not resolve: %s -> {%r, %r, %r}" % (key, ns, nm, va))
    return node


def inp(b, node, name):
    h, res = b.find_node_input_by_name(node, name)
    if res != OK:
        raise Fail("no input pin named %r" % name)
    return h


def outp(b, node, name):
    h, res = b.find_node_output_by_name(node, name)
    if res != OK:
        raise Fail("no output pin named %r" % name)
    return h


def wire(b, o, i, what):
    if b.connect_nodes(o, i) != OK:
        raise Fail("connect failed: %s" % what)


def setf(b, node, pin, value):
    lit = SUBS.create_float_meta_sound_literal(float(value))
    if isinstance(lit, tuple):
        lit = lit[0]
    if b.set_node_input_default(inp(b, node, pin), lit) != OK:
        raise Fail("could not set %s = %s" % (pin, value))


def build_cue():
    b, on_play, on_finished, audio_ins, res = SUBS.create_source_builder(
        CUE_NAME, unreal.MetaSoundOutputAudioFormat.MONO, True
    )
    if res != OK:
        raise Fail("create_source_builder did not succeed")
    if len(audio_ins) < 1:
        raise Fail("a mono source exposed no audio output input")

    tones = []
    for f in FREQS:
        t = add_node(b, "Sine")
        setf(b, t, "Frequency", f)
        tones.append(t)

    # Summed BEFORE the envelope, so one envelope shapes one signal -- enveloping each voice
    # separately would be two envelopes that can drift apart in amplitude.
    mixer = add_node(b, "Add")
    wire(b, outp(b, tones[0], "Audio"), inp(b, mixer, "PrimaryOperand"), "tone0 -> mix")
    wire(b, outp(b, tones[1], "Audio"), inp(b, mixer, "AdditionalOperands"), "tone1 -> mix")
    src_out = outp(b, mixer, "Out")

    env = add_node(b, "ADEnv")
    setf(b, env, "Attack Time", ATTACK)
    setf(b, env, "Decay Time", DECAY)

    vca = add_node(b, "Multiply")
    wire(b, src_out, inp(b, vca, "PrimaryOperand"), "source -> vca")
    wire(b, outp(b, env, "Out Envelope"), inp(b, vca, "AdditionalOperands"), "envelope -> vca")
    wire(b, outp(b, vca, "Out"), audio_ins[0], "vca -> audio out")

    # OnPlay opens the envelope. OnDone ends the one-shot -- WITHOUT THE SECOND WIRE the source
    # never reports finished and the voice is held for the life of the world.
    wire(b, on_play, inp(b, env, "Trigger"), "OnPlay -> envelope")
    wire(b, outp(b, env, "On Done"), on_finished, "envelope -> OnFinished")

    asset, res = EDS.build_to_asset(b, AUTHOR, CUE_NAME, PKG)
    if res != OK:
        raise Fail("build_to_asset did not succeed")
    return asset


def main():
    say("STRAT ROUND-CHANGE CUE BUILD")
    say("package: %s   slot: UStratSoundBank.%s" % (PKG, SLOT))
    say("")

    # ---- THE BEFORE READING, which is what makes the AFTER mean anything ---------
    bank = unreal.EditorAssetLibrary.load_asset(BANK)
    if bank is None:
        raise Fail("the shipped sound bank did not load: %s" % BANK)
    try:
        before = bank.get_editor_property(SLOT)
    except Exception as e:
        raise Fail("no slot named %r on the bank -- the C++ property name is wrong in this "
                   "script: %s" % (SLOT, e))
    say("BEFORE  bank.%s = %s" % (SLOT, before.get_name() if before else "<UNSET>"))

    build_cue()
    say("BUILT   %s  voices=%d attack=%.3f decay=%.3f"
        % (CUE_NAME, len(FREQS), ATTACK, DECAY))

    path = "%s/%s.%s" % (PKG, CUE_NAME, CUE_NAME)
    obj = unreal.EditorAssetLibrary.load_asset(path)
    if obj is None:
        raise Fail("the cue did not load back from %s" % path)
    if not isinstance(obj, unreal.SoundBase):
        raise Fail("built asset is not a USoundBase, so it is not assignable: %s"
                   % obj.get_class().get_name())

    # ---- THE MIX CLASS, which has its own parity clause and is not optional -----
    sfx = unreal.EditorAssetLibrary.load_asset(SFX_CLASS)
    if sfx is None:
        raise Fail("the SFX sound class did not load: %s" % SFX_CLASS)
    obj.set_editor_property("sound_class_object", sfx)

    bank.set_editor_property(SLOT, obj)
    unreal.EditorAssetLibrary.save_loaded_asset(obj, only_if_is_dirty=False)
    unreal.EditorAssetLibrary.save_loaded_asset(bank, only_if_is_dirty=False)

    # A set() RETURN IS NOT EVIDENCE, and a same-process read is memory rather than bytes. This
    # readback is the WEAKER half on purpose; the cold one is a separate commandlet and is the
    # one that counts. Both are reported so a reader can tell which is which.
    after = bank.get_editor_property(SLOT)
    cls = obj.get_editor_property("sound_class_object")
    say("AFTER   bank.%s = %s   (same process -- memory, not bytes)"
        % (SLOT, after.get_name() if after else "<UNSET>"))
    say("        cue class=%s  isSoundBase=%s  soundClass=%s"
        % (obj.get_class().get_name(), isinstance(obj, unreal.SoundBase),
           cls.get_name() if cls else "<UNSET>"))
    say("")
    say("saved the cue and the bank")


try:
    main()
except Exception:
    say("TOP-LEVEL FAILURE\n" + traceback.format_exc())
flush()
