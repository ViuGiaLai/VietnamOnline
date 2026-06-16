#!/usr/bin/env python3
"""Validate data/ JSON configs for Vietnam Online."""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / "data"

def check_json(path: Path) -> bool:
    try:
        json.loads(path.read_text(encoding="utf-8"))
        print(f"OK  {path.relative_to(ROOT)}")
        return True
    except Exception as e:
        print(f"FAIL {path}: {e}")
        return False

def main() -> int:
    ok = True
    for p in DATA.rglob("*.json"):
        ok &= check_json(p)
    loc = ROOT / "shared/include/vno/localization"
    for p in loc.glob("*.json"):
        ok &= check_json(p)
    print("validate_data: PASS" if ok else "validate_data: FAIL")
    return 0 if ok else 1

if __name__ == "__main__":
    sys.exit(main())
