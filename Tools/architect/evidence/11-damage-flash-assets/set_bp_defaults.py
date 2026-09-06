import unreal, json, traceback
OUT = r"E:\MultiAgent\Stratocracy\Tools\architect\scratch\setbp_out.json"
BP  = "/Game/StratPlay/BP_StratUnit"
MAT = "/Game/StratArt/Materials/M_UnitDamageFlash"
res = {"before": {}, "after_write_in_process": {}, "saved": None, "error": None}
try:
    eal = unreal.EditorAssetLibrary
    bp  = eal.load_asset(BP)
    cdo = unreal.get_default_object(bp.generated_class())
    mat = eal.load_asset(MAT)
    res["material_loaded"] = mat.get_path_name() if mat else None

    props = ["DamageFlashSeconds", "DamageShakeAmplitude",
             "DamageShakeFrequency", "DamageFlashMaterial"]
    for p in props:
        res["before"][p] = str(cdo.get_editor_property(p))

    cdo.set_editor_property("DamageFlashSeconds",   0.2)
    cdo.set_editor_property("DamageShakeAmplitude", 8.0)
    cdo.set_editor_property("DamageShakeFrequency", 20.0)
    cdo.set_editor_property("DamageFlashMaterial",  mat)

    for p in props:
        res["after_write_in_process"][p] = str(cdo.get_editor_property(p))

    # An in-process read is NOT evidence the bytes landed. Save, and let a
    # SEPARATE process be the instrument.
    res["saved"] = bool(eal.save_asset(BP, only_if_is_dirty=False))
except Exception:
    res["error"] = traceback.format_exc()
with open(OUT, "w") as f:
    json.dump(res, f, indent=2)
