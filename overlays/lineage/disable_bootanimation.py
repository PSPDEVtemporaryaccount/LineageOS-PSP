from pathlib import Path

source_path = Path("CyanogenPSP/main.c")
source = source_path.read_text()

marker = "int bootAnimation()"
start = source.find(marker)
if start < 0:
    raise SystemExit("Could not find bootAnimation() in CyanogenPSP/main.c")

brace_start = source.find("{", start)
if brace_start < 0:
    raise SystemExit("Could not find bootAnimation() opening brace")

depth = 0
end = None
for index in range(brace_start, len(source)):
    char = source[index]
    if char == "{":
        depth += 1
    elif char == "}":
        depth -= 1
        if depth == 0:
            end = index + 1
            break

if end is None:
    raise SystemExit("Could not find bootAnimation() closing brace")

replacement = """int bootAnimation()
{
    // LineageOS PSP: boot animation temporarily disabled while startup is stabilized.
    // Enter the normal first-boot / lock-screen / home flow directly.
    firstBootInitiation();
    return 0;
}"""

source = source[:start] + replacement + source[end:]
source_path.write_text(source)

check = source_path.read_text()
function = check[check.find(marker):]
if "oslLoadImageFilePNG(\"system/boot/" in function[:1000]:
    raise SystemExit("Boot animation image loading is still present in the replacement")
if "firstBootInitiation();" not in function[:500]:
    raise SystemExit("Direct startup call is missing")

print("Boot animation disabled; startup now enters firstBootInitiation() directly.")
