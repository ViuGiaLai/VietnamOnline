"""
Giai doan A — Setup UE5 desktop playable (doc Phase A checklist)
Chay trong Unreal Editor: Tools > Execute Python Script

Buoc:
  1. Tao / cap nhat map HoiAn_MVP
  2. Anh sang (Lumen), NavMeshBoundsVolume, PlayerStart
  3. Nen tam (Plane) neu chua co Cesium
  4. Bat plugin CesiumForUnreal neu da cai
  5. Spawn CesiumGeoreference + Tileset (Hoi An)
  6. Build NavMesh
  7. Copy Mannequin mesh + tao AnimBP (neu co Third Person content)
"""
import unreal

MAP_PATH = "/Game/Maps/HoiAn_MVP"
HOI_AN_LAT = 15.8794
HOI_AN_LNG = 108.3294
NAV_EXTENT = unreal.Vector(50000, 50000, 10000)


def log(msg):
    unreal.log(f"[VNO Phase A] {msg}")


def ensure_directory(path):
    unreal.EditorAssetLibrary.make_directory(path)


def load_or_create_level():
    ensure_directory("/Game/Maps")
    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        unreal.EditorLoadingAndSavingUtils.load_map(MAP_PATH)
        log(f"Da load map: {MAP_PATH}")
        return unreal.EditorLevelLibrary.get_editor_world()
    world = unreal.EditorLevelLibrary.new_level(MAP_PATH)
    log(f"Da tao map moi: {MAP_PATH}")
    return world


def has_actor_of_class(actor_class):
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if actor.get_class() == actor_class:
            return True
    return False


def spawn_lighting():
    if not has_actor_of_class(unreal.DirectionalLight):
        unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.DirectionalLight,
            unreal.Vector(0, 0, 5000),
            unreal.Rotator(-45, 45, 0),
        )
        log("DirectionalLight")
    if not has_actor_of_class(unreal.SkyLight):
        unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.SkyLight, unreal.Vector(0, 0, 1000), unreal.Rotator(0, 0, 0)
        )
        log("SkyLight")
    if not has_actor_of_class(unreal.SkyAtmosphere):
        unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.SkyAtmosphere, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0)
        )
        log("SkyAtmosphere")
    if not has_actor_of_class(unreal.ExponentialHeightFog):
        unreal.EditorLevelLibrary.spawn_actor_from_class(
            unreal.ExponentialHeightFog, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0)
        )
        log("ExponentialHeightFog")


def spawn_floor_plane():
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if "VNO_Floor" in actor.get_actor_label():
            return
    floor_mesh = unreal.EditorAssetLibrary.load_asset("/Engine/BasicShapes/Plane.Plane")
    if not floor_mesh:
        return
    actor = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.StaticMeshActor, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0)
    )
    actor.set_actor_label("VNO_Floor")
    sm = actor.static_mesh_component
    sm.set_static_mesh(floor_mesh)
    sm.set_world_scale3d(unreal.Vector(500, 500, 1))
    sm.set_collision_enabled(unreal.CollisionEnabled.QUERY_AND_PHYSICS)
    log("Floor plane (fallback khi chua co Cesium terrain)")


def spawn_player_start():
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if isinstance(actor, unreal.PlayerStart):
            actor.set_actor_location(unreal.Vector(0, 0, 200), False, True)
            log("PlayerStart tai (0,0,200) — C++ se teleport ve Hoi An neu Cesium bat")
            return
    unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.PlayerStart, unreal.Vector(0, 0, 200), unreal.Rotator(0, 0, 0)
    )
    log("PlayerStart moi")


def spawn_nav_mesh_bounds():
    for actor in unreal.EditorLevelLibrary.get_all_level_actors():
        if isinstance(actor, unreal.NavMeshBoundsVolume):
            actor.set_actor_scale3d(unreal.Vector(100, 100, 20))
            log("NavMeshBoundsVolume da co — scale 100x")
            return
    vol = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.NavMeshBoundsVolume,
        unreal.Vector(0, 0, 0),
        unreal.Rotator(0, 0, 0),
    )
    vol.set_actor_scale3d(unreal.Vector(100, 100, 20))
    log("NavMeshBoundsVolume 100x — FR-03 khong xuyen tuong")


def try_enable_cesium_plugin():
    try:
        if unreal.PluginBlueprintLibrary.is_plugin_enabled("CesiumForUnreal"):
            log("CesiumForUnreal da bat")
            return True
        unreal.PluginBlueprintLibrary.set_plugin_enabled("CesiumForUnreal", True)
        log("Da bat CesiumForUnreal — RESTART Editor de ap dung")
        return True
    except Exception as e:
        log(f"Cesium plugin chua cai: {e}")
        return False


def try_spawn_cesium_actors():
    try:
        geo_class = unreal.load_class(None, "/Script/CesiumRuntime.CesiumGeoreference")
        tile_class = unreal.load_class(None, "/Script/CesiumRuntime.Cesium3DTileset")
        if not geo_class or not tile_class:
            log("Cesium classes khong tim thay — cai plugin tu Marketplace")
            return False
        if has_actor_of_class(geo_class):
            log("Cesium actors da co trong level")
            return True
        geo = unreal.EditorLevelLibrary.spawn_actor_from_class(
            geo_class, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0)
        )
        geo.set_editor_property("origin_latitude", HOI_AN_LAT)
        geo.set_editor_property("origin_longitude", HOI_AN_LNG)
        geo.set_actor_label("VNO_CesiumGeoreference")
        tile = unreal.EditorLevelLibrary.spawn_actor_from_class(
            tile_class, unreal.Vector(0, 0, 0), unreal.Rotator(0, 0, 0)
        )
        tile.set_editor_property("georeference", geo)
        tile.set_editor_property("ion_tileset_id", 2275207)
        tile.set_actor_label("VNO_Cesium3DTileset")
        log(f"Cesium Hoi An ({HOI_AN_LAT}, {HOI_AN_LNG}) — nhap CESIUM_ION_TOKEN trong .env")
        return True
    except Exception as e:
        log(f"Khong spawn Cesium: {e}")
        return False


def build_navigation():
    try:
        unreal.SystemLibrary.execute_console_command(
            None, "RebuildNavigation"
        )
        log("NavMesh rebuild — cho vai giay trong Output Log")
    except Exception as e:
        log(f"NavMesh rebuild: {e}")


def setup_mannequin_assets():
    """Copy Mannequin mesh vao /Game/VNO neu Third Person content co san."""
    src_mesh = "/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"
    dst_dir = "/Game/VNO/Characters/Mesh"
    dst_mesh = f"{dst_dir}/SKM_VNOPlayer"
    ensure_directory("/Game/VNO")
    ensure_directory(dst_dir)
    ensure_directory("/Game/VNO/Characters/Anim")
    if unreal.EditorAssetLibrary.does_asset_exist(dst_mesh):
        log("SKM_VNOPlayer da co")
        return
    if not unreal.EditorAssetLibrary.does_asset_exist(src_mesh):
        log("Mannequin chua co — Add Feature: Third Person (Content Only) hoac dung cube tam")
        return
    unreal.EditorAssetLibrary.duplicate_asset(src_mesh, dst_mesh)
    log(f"Da copy mesh -> {dst_mesh}")


def setup_quinn_asset():
    src_mesh = "/Game/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"
    dst_dir = "/Game/VNO/Characters/Mesh"
    dst_mesh = f"{dst_dir}/SKM_VNOPlayer_F"
    if unreal.EditorAssetLibrary.does_asset_exist(dst_mesh):
        return
    if not unreal.EditorAssetLibrary.does_asset_exist(src_mesh):
        return
    unreal.EditorAssetLibrary.duplicate_asset(src_mesh, dst_mesh)
    log(f"Da copy mesh nu -> {dst_mesh}")


def save_level():
    unreal.EditorLevelLibrary.save_current_level()
    log(f"Da luu {MAP_PATH}")


def run_phase_a_setup():
    log("=== BAT DAU GIAI DOAN A ===")
    load_or_create_level()
    spawn_lighting()
    spawn_floor_plane()
    spawn_player_start()
    spawn_nav_mesh_bounds()
    try_enable_cesium_plugin()
    try_spawn_cesium_actors()
    setup_mannequin_assets()
    setup_quinn_asset()
    build_navigation()
    save_level()
    log("=== HOAN TAT — Play (Alt+P) | Console: vno.Login email pass ===")
    log("Chay schema_supabase.sql trong Supabase SQL Editor cho FR-10")


if __name__ == "__main__":
    run_phase_a_setup()
