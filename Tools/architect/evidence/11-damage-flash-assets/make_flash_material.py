import unreal, json, traceback

OUT  = r"E:\MultiAgent\Stratocracy\Tools\architect\scratch\make_out.json"
PKG  = "/Game/StratArt/Materials"
NAME = "M_UnitDamageFlash"
PATH = PKG + "/" + NAME

res = {"created": None, "readback": {}, "controls": {}, "error": None}
try:
    eal = unreal.EditorAssetLibrary
    mel = unreal.MaterialEditingLibrary

    res["controls"]["existed_before"] = bool(eal.does_asset_exist(PATH))

    if res["controls"]["existed_before"]:
        mat = eal.load_asset(PATH)
        res["created"] = "already existed -- reused, not recreated"
    else:
        tools = unreal.AssetToolsHelpers.get_asset_tools()
        mat = tools.create_asset(NAME, PKG, unreal.Material, unreal.MaterialFactoryNew())
        res["created"] = "created"

    # Unlit + translucent: an overlay pass that reads as a flash rather than
    # as a second lit surface. Domain stays Surface.
    mat.set_editor_property("material_domain", unreal.MaterialDomain.MD_SURFACE)
    mat.set_editor_property("blend_mode", unreal.BlendMode.BLEND_TRANSLUCENT)
    mat.set_editor_property("shading_model", unreal.MaterialShadingModel.MSM_UNLIT)
    mat.set_editor_property("two_sided", False)

    # Rebuild the graph from scratch so a re-run is idempotent rather than additive.
    # `expression_collection` is NOT a Python-exposed property in UE 5.8 -- measured,
    # it raises "Failed to find property" -- so the library's own verb does it.
    mel.delete_all_material_expressions(mat)

    col = mel.create_material_expression(mat, unreal.MaterialExpressionVectorParameter, -600, -100)
    col.set_editor_property("parameter_name", "FlashColor")
    col.set_editor_property("default_value", unreal.LinearColor(1.0, 0.045, 0.045, 1.0))

    inten = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -600, 60)
    inten.set_editor_property("parameter_name", "FlashIntensity")
    inten.set_editor_property("default_value", 3.0)

    mul = mel.create_material_expression(mat, unreal.MaterialExpressionMultiply, -320, -40)
    mel.connect_material_expressions(col, "", mul, "A")
    mel.connect_material_expressions(inten, "", mul, "B")
    mel.connect_material_property(mul, "", unreal.MaterialProperty.MP_EMISSIVE_COLOR)

    op = mel.create_material_expression(mat, unreal.MaterialExpressionScalarParameter, -320, 140)
    op.set_editor_property("parameter_name", "FlashOpacity")
    op.set_editor_property("default_value", 0.85)
    mel.connect_material_property(op, "", unreal.MaterialProperty.MP_OPACITY)

    mel.recompile_material(mat)
    saved = eal.save_asset(PATH)
    res["controls"]["save_asset_returned"] = bool(saved)

    # READ BACK FROM A FRESHLY LOADED COPY. A setter's return value is not evidence.
    eal.load_asset(PATH)
    m2 = eal.load_asset(PATH)
    r = res["readback"]
    r["exists"] = bool(eal.does_asset_exist(PATH))
    r["class"] = type(m2).__name__
    r["blend_mode"] = str(m2.get_editor_property("blend_mode"))
    r["shading_model"] = str(m2.get_editor_property("shading_model"))
    r["material_domain"] = str(m2.get_editor_property("material_domain"))
    r["num_expressions"] = mel.get_num_material_expressions(m2)
    try:
        r["scalar_params"] = [str(n) for n in mel.get_scalar_parameter_names(m2)]
    except Exception as e:
        r["scalar_params"] = "<%s>" % e
    try:
        r["vector_params"] = [str(n) for n in mel.get_vector_parameter_names(m2)]
    except Exception as e:
        r["vector_params"] = "<%s>" % e

    # CONTROL: the same readback on the project's existing master, so the
    # figures above are known to come from a helper that can speak.
    ctl = eal.load_asset("/Game/StratArt/Materials/M_OverlayPulse")
    res["controls"]["M_OverlayPulse_num_expressions"] = mel.get_num_material_expressions(ctl)
    res["controls"]["M_OverlayPulse_shading_model"] = str(ctl.get_editor_property("shading_model"))
except Exception:
    res["error"] = traceback.format_exc()

with open(OUT, "w") as f:
    json.dump(res, f, indent=2)
