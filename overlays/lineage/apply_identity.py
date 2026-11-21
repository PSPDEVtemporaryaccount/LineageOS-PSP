from pathlib import Path
import re

ROOT = Path("CyanogenPSP")
MAKEFILE = ROOT / "Makefile"
MAIN = ROOT / "main.c"

if not ROOT.is_dir():
    raise SystemExit("Upstream source directory is missing: CyanogenPSP")
if not MAKEFILE.is_file() or not MAIN.is_file():
    raise SystemExit("Required upstream Makefile or main.c is missing")

makefile = MAKEFILE.read_text(encoding="utf-8")
match = re.search(r"(?m)^VERSION\s*=\s*([^\s#]+)", makefile)
if not match:
    raise SystemExit("Could not read VERSION from the upstream Makefile")
version = match.group(1)
identity = f"LineageOS PSP ver.{version}"

# Replace the inherited product name in source/build text only. Keep the
# upstream checkout directory and its separate license/attribution files intact.
source_suffixes = {".c", ".h", ".cpp", ".hpp", ".cc", ".hh", ".s", ".S", ".mk"}
changed = []
for path in ROOT.rglob("*"):
    if not path.is_file():
        continue
    if path.name != "Makefile" and path.suffix not in source_suffixes:
        continue
    try:
        content = path.read_text(encoding="utf-8")
    except UnicodeDecodeError:
        continue
    updated = content.replace("CyanogenPSP", identity)
    if path == MAIN:
        updated, count = re.subn(
            r'PSP_MODULE_INFO\("LineageOS PSP(?: ver\.[^"]*)?",',
            f'PSP_MODULE_INFO("{identity}",',
            updated,
            count=1,
        )
        if count != 1:
            raise SystemExit("Could not set the versioned PSP module identity in main.c")
    if path == MAKEFILE:
        updated = re.sub(
            r"(?m)^PSP_EBOOT_TITLE\s*=.*$",
            f"PSP_EBOOT_TITLE = {identity}",
            updated,
        )
    if updated != content:
        path.write_text(updated, encoding="utf-8")
        changed.append(path.relative_to(ROOT).as_posix())

# Validate the exact identity embedded in both PSP-visible metadata fields.
main_text = MAIN.read_text(encoding="utf-8")
makefile = MAKEFILE.read_text(encoding="utf-8")
if f'PSP_MODULE_INFO("{identity}",' not in main_text:
    raise SystemExit("Versioned PSP_MODULE_INFO identity validation failed")
if f"PSP_EBOOT_TITLE = {identity}" not in makefile:
    raise SystemExit("Versioned EBOOT title validation failed")

remaining = []
for path in ROOT.rglob("*"):
    if not path.is_file():
        continue
    if path.name != "Makefile" and path.suffix not in source_suffixes:
        continue
    try:
        if "CyanogenPSP" in path.read_text(encoding="utf-8"):
            remaining.append(path.relative_to(ROOT).as_posix())
    except UnicodeDecodeError:
        pass
if remaining:
    raise SystemExit("Unreplaced source identity remains in: " + ", ".join(remaining))

print(f"Product identity applied: {identity}")
print("Changed source/build files:")
for name in changed:
    print(f" - {name}")
