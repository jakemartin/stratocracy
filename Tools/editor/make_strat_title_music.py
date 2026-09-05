# Authors MS_Strat_TitleTheme, the looping music bed for the title level.
#
# ROUTE. Headless, no running editor required:
#   "C:/Program Files/Epic Games/UE_5.8/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" \
#     "E:/MultiAgent/Stratocracy/Stratocracy.uproject" -run=pythonscript \
#     -script="E:/MultiAgent/Stratocracy/Tools/editor/make_strat_title_music.py" \
#     -unattended -nopause -nosplash -nullrhi
#
# SEPARATE FROM make_strat_sound_bank.py ON PURPOSE. That script rebuilds all seven cue assets
# every time it runs; re-running it merely to add an eighth asset would rewrite seven files that
# are already verified and already unstaged, and a source-control provider on this machine
# auto-adds a package the instant it is saved. A second script touches one asset.
#
# THE TWO WAYS THIS DIFFERS FROM A CUE, AND BOTH ARE THE POINT.
#   1. `is_one_shot=False`. A one-shot ends when its envelope reports done; a music bed must not
#      end at all. The consequence is that there is NO `OnFinished` wire here -- for a cue that
#      wire is mandatory or the voice is held forever, and for this asset it would be the bug.
#   2. The envelope LOOPS. `AD Envelope` carries a `Looping` bool input, so one trigger at
#      `OnPlay` gives a swell that repeats for the life of the sound rather than a single stab.
#
# WHAT THIS SCRIPT CANNOT PROVE, exactly as its sibling records: that any of it is AUDIBLE. A
# commandlet has no audio device -- "Audio Device Manager not initializing due to all audio being
# disabled". It can prove the graph was built, wired and saved, and that the asset loads as a
# USoundBase. It cannot prove the bed is pleasant, that it loops without a seam, or that it is
# not silent. A human at the keyboard is the only instrument for any of that.
#
# Reports to a FILE and never through `unreal.log`, which does not reach a commandlet's captured
# log at all -- measured 2026-09-05 and recorded in `Tools/architect/state/content.md`.

import unreal
import io
import os
import traceback

REPORT_PATH = r"E:/MultiAgent/Stratocracy/Saved/StratAudioBuild/title_music.txt"
PKG = "/Game/StratAudio"
NAME = "MS_Strat_TitleTheme"
AUTHOR = "Stratocracy"

L = []
def say(s): L.append(str(s))

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


# Same table as the cue script, and the same reason for spelling each one out: an unresolved
# class name yields an invalid handle and the later build_to_asset SUCCEEDS anyway.
SINE = ("UE", "Sine", "Audio")
ADD = ("UE", "Add", "Audio")
MUL = ("UE", "Multiply", "Audio")
ADENV = ("AD Envelope", "AD Envelope", "Audio")   # irregular: node name in the namespace slot


def add_node(b, triple, label):
    node, res = b.add_node_by_class_name(cn(*triple))
    if res != OK:
        raise Fail("class name did not resolve: %s -> %r" % (label, (triple,)))
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


def unwrap(v):
    # The literal factories return a 2-tuple (literal, result), not a bare literal. Passing the
    # tuple straight through fails at the nativizer -- loudly, but only because this reports to
    # a file. Measured 2026-09-05.
    return v[0] if isinstance(v, tuple) else v


def setf(b, node, pin, value):
    lit = unwrap(SUBS.create_float_meta_sound_literal(float(value)))
    if b.set_node_input_default(inp(b, node, pin), lit) != OK:
        raise Fail("could not set %s = %s" % (pin, value))


def setb(b, node, pin, value):
    lit = unwrap(SUBS.create_bool_meta_sound_literal(bool(value)))
    if b.set_node_input_default(inp(b, node, pin), lit) != OK:
        raise Fail("could not set %s = %s" % (pin, value))


# An A-minor triad, low and close together so the bed sits under a menu rather than over it.
TONES = [220.00, 261.63, 329.63]
ATTACK = 2.0
DECAY = 3.0


def build():
    b, on_play, _on_finished, audio_ins, res = SUBS.create_source_builder(
        NAME, unreal.MetaSoundOutputAudioFormat.MONO, False
    )
    if res != OK:
        raise Fail("create_source_builder did not succeed")
    if len(audio_ins) < 1:
        raise Fail("a mono source exposed no audio output input")

    tones = []
    for f in TONES:
        t = add_node(b, SINE, "Sine")
        setf(b, t, "Frequency", f)
        tones.append(t)

    # Sum the triad before the envelope so one envelope shapes one signal.
    mix1 = add_node(b, ADD, "Add")
    wire(b, outp(b, tones[0], "Audio"), inp(b, mix1, "PrimaryOperand"), "tone0 -> mix1")
    wire(b, outp(b, tones[1], "Audio"), inp(b, mix1, "AdditionalOperands"), "tone1 -> mix1")
    mix2 = add_node(b, ADD, "Add")
    wire(b, outp(b, mix1, "Out"), inp(b, mix2, "PrimaryOperand"), "mix1 -> mix2")
    wire(b, outp(b, tones[2], "Audio"), inp(b, mix2, "AdditionalOperands"), "tone2 -> mix2")

    env = add_node(b, ADENV, "AD Envelope")
    setf(b, env, "Attack Time", ATTACK)
    setf(b, env, "Decay Time", DECAY)
    setb(b, env, "Looping", True)

    vca = add_node(b, MUL, "Multiply")
    wire(b, outp(b, mix2, "Out"), inp(b, vca, "PrimaryOperand"), "mix -> vca")
    wire(b, outp(b, env, "Out Envelope"), inp(b, vca, "AdditionalOperands"), "env -> vca")
    wire(b, outp(b, vca, "Out"), audio_ins[0], "vca -> audio out")

    wire(b, on_play, inp(b, env, "Trigger"), "OnPlay -> envelope")
    # NO OnFinished WIRE. See the header: this source must never report finished.

    asset, res = EDS.build_to_asset(b, AUTHOR, NAME, PKG)
    if res != OK:
        raise Fail("build_to_asset did not succeed")
    return asset


def main():
    say("STRAT TITLE MUSIC BUILD")
    say("package: %s   asset: %s" % (PKG, NAME))
    say("triad: %s   attack=%.2f decay=%.2f looping=True one_shot=False" % (TONES, ATTACK, DECAY))
    say("")
    try:
        build()
        say("BUILT   %s" % NAME)
    except Fail as e:
        say("FAIL    %s" % e)
    except Exception:
        say("ERROR\n%s" % traceback.format_exc())

    # Route it to the music class so the volume screen's music slider reaches it, and NOT to the
    # SFX class the seven cues carry.
    say("")
    say("=== SOUND CLASS ===")
    try:
        music = unreal.EditorAssetLibrary.load_asset("%s/SCL_Strat_Music" % PKG)
        obj = unreal.EditorAssetLibrary.load_asset("%s/%s" % (PKG, NAME))
        obj.set_editor_property("sound_class_object", music)
        say("  assigned %s" % (music.get_name() if music else "<none>"))
    except Exception:
        say("  FAILED\n%s" % traceback.format_exc())

    unreal.EditorAssetLibrary.save_directory(PKG, only_if_is_dirty=False, recursive=True)

    # READBACK from disk. build_to_asset returns SUCCEEDED on an EMPTY graph -- measured with a
    # control on 2026-09-05 -- so its verdict is not evidence that anything was built.
    say("")
    say("=== READBACK ===")
    path = "%s/%s.%s" % (PKG, NAME, NAME)
    if not unreal.EditorAssetLibrary.does_asset_exist(path):
        say("MISSING %s" % path)
    else:
        o = unreal.EditorAssetLibrary.load_asset(path)
        sc = o.get_editor_property("sound_class_object")
        say("%s  class=%s  isSoundBase=%s  soundClass=%s"
            % (NAME, o.get_class().get_name(), isinstance(o, unreal.SoundBase),
               sc.get_name() if sc else "<UNSET>"))


try:
    main()
except Exception:
    say("TOP-LEVEL FAILURE\n" + traceback.format_exc())
os.makedirs(os.path.dirname(REPORT_PATH), exist_ok=True)
io.open(REPORT_PATH, "w", encoding="utf-8").write("\n".join(L) + "\n")
