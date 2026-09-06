import unreal, json, traceback
OUT = r"E:\MultiAgent\Stratocracy\Tools\architect\scratch\bp_out.json"
res = {"reads": {}, "error": None}
try:
    eal = unreal.EditorAssetLibrary
    bp = eal.load_asset("/Game/StratPlay/BP_StratUnit")
    res["bp_class"] = type(bp).__name__
    gen = bp.generated_class()
    cdo = unreal.get_default_object(gen)
    res["cdo_class"] = type(cdo).__name__
    # CONTROL: BodyZOffset is protected+EditDefaultsOnly, C++ default 0.0, and the
    # record says the Blueprint ships 50.0. Reading 50.0 proves BOTH that a
    # protected property is reachable AND that this is the Blueprint's CDO and
    # not the C++ one. MoveTweenSeconds (C++ 0.0, BP 0.2) is a second control.
    for prop in ["BodyZOffset", "MoveTweenSeconds", "MoveTweenEaseFraction",
                 "DamageFlashSeconds", "DamageShakeAmplitude",
                 "DamageShakeFrequency", "DamageFlashMaterial"]:
        try:
            v = cdo.get_editor_property(prop)
            res["reads"][prop] = str(v)
        except Exception as e:
            res["reads"][prop] = "<UNREACHABLE: %s>" % e
except Exception:
    res["error"] = traceback.format_exc()
with open(OUT, "w") as f:
    json.dump(res, f, indent=2)
