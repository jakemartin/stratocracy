import unreal, json, traceback

OUT = r"E:\MultiAgent\Stratocracy\Tools\architect\scratch\probe_out.json"
res = {"ran": True, "assets": {}, "controls": {}, "error": None}
try:
    eal = unreal.EditorAssetLibrary
    for path in ["/Game/StratArt/Materials/M_OverlayPulse",
                 "/Game/StratArt/Materials/MI_Side_0",
                 "/Game/StratArt/Materials/MI_Overlay_Target"]:
        d = {"exists": bool(eal.does_asset_exist(path))}
        if d["exists"]:
            a = eal.load_asset(path)
            d["class"] = type(a).__name__
            if isinstance(a, unreal.MaterialInstanceConstant):
                p = a.get_editor_property("parent")
                d["parent"] = p.get_path_name() if p else None
            if isinstance(a, unreal.Material):
                for prop in ["blend_mode", "shading_model", "two_sided", "material_domain"]:
                    try:
                        d[prop] = str(a.get_editor_property(prop))
                    except Exception as e:
                        d[prop] = "<%s>" % e
                try:
                    d["num_expressions"] = unreal.MaterialEditingLibrary.get_num_material_expressions(a)
                except Exception as e:
                    d["num_expressions"] = "<%s>" % e
        res["assets"][path] = d
    # Controls: the helper must be able to answer BOTH ways, or a False above
    # is a broken lookup rather than a measurement.
    res["controls"]["absent_path"] = bool(eal.does_asset_exist("/Game/StratArt/Materials/M_NoSuchThing"))
    res["controls"]["present_path"] = bool(eal.does_asset_exist("/Game/StratPlay/BP_StratUnit"))
except Exception:
    res["error"] = traceback.format_exc()

with open(OUT, "w") as f:
    json.dump(res, f, indent=2)
