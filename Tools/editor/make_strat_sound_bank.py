# Authors the seven Stratocracy cue MetaSounds and reports what it actually built.
#
# ROUTE. Headless, no running editor required:
#   "C:/Program Files/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" \
#     "E:/MultiAgent/Stratocracy/Stratocracy.uproject" -run=pythonscript \
#     -script="E:/MultiAgent/Stratocracy/Tools/editor/make_strat_sound_bank.py" \
#     -unattended -nopause -nosplash -nullrhi
# PythonScriptPlugin is enabled TRANSITIVELY by PCGPythonInterop in the .uproject, so no plugin
# change and no restart -- the same reasoning make_scoreboard_wbp.py records for its own route.
#
# WHY THIS WRITES A REPORT FILE AND NEVER REPORTS THROUGH `unreal.log`. MEASURED 2026-09-05: in a
# -run=pythonscript commandlet, `unreal.log` output does NOT reach stdout or the captured log. A
# probe that reported only through it printed "Python script executed successfully", took 0.14
# seconds and emitted not one of its own lines -- which is indistinguishable from a script that
# ran and did nothing. The control that settled it was writing to a file instead: the file
# appeared, so the script had been running all along and only the reporting channel was dead.
# Every finding here goes to REPORT_PATH and the caller reads that file.
#
# WHY EVERY NODE CLASS NAME IS RESOLVED AND NONE IS GUESSED. add_node_by_class_name returns a
# non-SUCCEEDED result for a name that does not resolve, and a later build_to_asset then SUCCEEDS
# anyway and writes a SILENT asset that nothing downstream complains about. Three of the seven
# names below do not follow the regular {"UE", Name, "Audio"} shape; see CLASSES.
#
# WHAT THIS SCRIPT CANNOT PROVE: THAT ANY OF IT IS AUDIBLE. A commandlet has no audio device and
# says so in terms -- "Audio Device Manager not initializing due to all audio being disabled". It
# can prove a graph was built, wired and saved. It cannot prove an envelope opens, that a voice
# is not silent, or that a frequency is pleasant. A human at the keyboard is the only instrument
# for any of that, and no line of this file's output may be read as a claim about sound.

import unreal
import io
import os
import traceback

REPORT_PATH = r"E:/MultiAgent/Stratocracy/Saved/StratAudioBuild/report.txt"
PKG = "/Game/StratAudio"
AUTHOR = "Stratocracy"

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


# {namespace, name, variant}, read out of the engine's own node sources rather than assumed.
# The regular family is {"UE", Name, "Audio"} -- MetasoundOscillatorNodes.cpp:308 and its
# neighbours, MetasoundNoiseGenerator.cpp:215.
# Multiply and Add come from the math macro at MetasoundMathNodes.cpp:33, which spells the
# variant as the DATA TYPE, not the literal "Audio" variant constant -- same string here, a
# different reason for it.
# ADEnv is the irregular one: MetasoundADEnvelopeNode.cpp:268 puts the node's own display name in
# the NAMESPACE slot and carries the data type in the variant.
CLASSES = {
    "Sine": ("UE", "Sine", "Audio"),
    "Square": ("UE", "Square", "Audio"),
    "Triangle": ("UE", "Triangle", "Audio"),
    "Noise": ("UE", "Noise", "Audio"),
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
    # create_float_meta_sound_literal returns a 2-TUPLE (literal, result), not a bare literal.
    # Passing the tuple through fails at the nativizer with "Struct has 0 initialization
    # parameters, but the given sequence had 2 elements" -- a loud failure, but only because
    # this script reports to a file; through `unreal.log` it would have been invisible.
    lit = SUBS.create_float_meta_sound_literal(float(value))
    if isinstance(lit, tuple):
        lit = lit[0]
    if b.set_node_input_default(inp(b, node, pin), lit) != OK:
        raise Fail("could not set %s = %s" % (pin, value))


# name, waveform, frequencies (None = the node has no Frequency pin), attack sec, decay sec
CUES = [
    ("MS_Strat_ButtonClick", "Sine", [1200.0], 0.002, 0.040),
    ("MS_Strat_EndTurn", "Sine", [440.0, 660.0], 0.010, 0.350),
    ("MS_Strat_UnitMove", "Noise", [None], 0.005, 0.180),
    ("MS_Strat_UnitAttack", "Square", [140.0], 0.001, 0.120),
    ("MS_Strat_UnitDestroyed", "Noise", [None], 0.005, 0.900),
    ("MS_Strat_FactoryBuild", "Triangle", [330.0, 495.0], 0.020, 0.250),
    ("MS_Strat_MatchEnd", "Sine", [523.25, 659.25], 0.030, 1.400),
]


def build_one(name, wave, freqs, attack, decay):
    b, on_play, on_finished, audio_ins, res = SUBS.create_source_builder(
        name, unreal.MetaSoundOutputAudioFormat.MONO, True
    )
    if res != OK:
        raise Fail("create_source_builder did not succeed")
    if len(audio_ins) < 1:
        raise Fail("a mono source exposed no audio output input")

    tones = []
    for f in freqs:
        t = add_node(b, wave)
        if f is not None:
            setf(b, t, "Frequency", f)
        tones.append(t)

    # A chord is summed BEFORE the envelope, so one envelope shapes one signal. Enveloping each
    # voice separately would be two envelopes that can drift apart in amplitude.
    if len(tones) == 1:
        src_out = outp(b, tones[0], "Audio")
    else:
        mixer = add_node(b, "Add")
        wire(b, outp(b, tones[0], "Audio"), inp(b, mixer, "PrimaryOperand"), "tone0 -> mix")
        wire(b, outp(b, tones[1], "Audio"), inp(b, mixer, "AdditionalOperands"), "tone1 -> mix")
        src_out = outp(b, mixer, "Out")

    env = add_node(b, "ADEnv")
    setf(b, env, "Attack Time", attack)
    setf(b, env, "Decay Time", decay)

    vca = add_node(b, "Multiply")
    wire(b, src_out, inp(b, vca, "PrimaryOperand"), "source -> vca")
    wire(b, outp(b, env, "Out Envelope"), inp(b, vca, "AdditionalOperands"), "envelope -> vca")
    wire(b, outp(b, vca, "Out"), audio_ins[0], "vca -> audio out")

    # OnPlay opens the envelope. OnDone ends the one-shot -- WITHOUT THE SECOND WIRE the source
    # never reports finished and the voice is held for the life of the world.
    wire(b, on_play, inp(b, env, "Trigger"), "OnPlay -> envelope")
    wire(b, outp(b, env, "On Done"), on_finished, "envelope -> OnFinished")

    asset, res = EDS.build_to_asset(b, AUTHOR, name, PKG)
    if res != OK:
        raise Fail("build_to_asset did not succeed")
    return asset


def main():
    say("STRAT AUDIO BUILD")
    say("package: %s" % PKG)
    say("")
    built = 0
    for name, wave, freqs, a, d in CUES:
        try:
            build_one(name, wave, freqs, a, d)
            built += 1
            say("BUILT   %-26s %-9s voices=%d attack=%.3f decay=%.3f"
                % (name, wave, len(freqs), a, d))
        except Fail as e:
            say("FAIL    %-26s %s" % (name, e))
        except Exception:
            say("ERROR   %-26s\n%s" % (name, traceback.format_exc()))
    say("")
    say("built %d of %d" % (built, len(CUES)))

    # READBACK. Everything above is the builder's own word for itself. This re-opens each asset
    # from disk and checks the two things a wrong build gets wrong: that it is there at all, and
    # that it is a USoundBase -- which is exactly what makes it assignable into the bank's slots.
    say("")
    say("=== READBACK ===")
    for name, _w, _f, _a, _d in CUES:
        path = "%s/%s.%s" % (PKG, name, name)
        if not unreal.EditorAssetLibrary.does_asset_exist(path):
            say("MISSING %s" % path)
            continue
        obj = unreal.EditorAssetLibrary.load_asset(path)
        klass = obj.get_class().get_name() if obj else "<none>"
        say("%-26s class=%-18s isSoundBase=%s"
            % (name, klass, isinstance(obj, unreal.SoundBase)))

    unreal.EditorAssetLibrary.save_directory(PKG, only_if_is_dirty=False, recursive=True)
    say("")
    say("saved %s" % PKG)


try:
    main()
except Exception:
    say("TOP-LEVEL FAILURE\n" + traceback.format_exc())
flush()
