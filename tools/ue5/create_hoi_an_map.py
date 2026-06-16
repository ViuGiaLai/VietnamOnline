"""
Tao map HoiAn_MVP — wrapper goi setup_phase_a.py (Giai doan A day du).
"""
import unreal
import importlib.util
import os

def create_hoi_an_level():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    setup_path = os.path.join(script_dir, "setup_phase_a.py")
    spec = importlib.util.spec_from_file_location("vno_setup_phase_a", setup_path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    mod.run_phase_a_setup()

if __name__ == "__main__":
    create_hoi_an_level()
