#!/usr/bin/env python3
"""Apply the LineageOS PSP home-screen identity and native boot animation."""

from pathlib import Path

main_path = Path("CyanogenPSP/main.c")
home_path = Path("CyanogenPSP/homeMenu.c")

if not main_path.is_file() or not home_path.is_file():
    raise SystemExit("CyanogenPSP main.c or homeMenu.c is missing")


def replace_function(source: str, signature: str, replacement: str) -> str:
    start = source.find(signature)
    if start < 0:
        raise SystemExit(f"Could not find function: {signature}")
    brace = source.find("{", start)
    if brace < 0:
        raise SystemExit(f"Could not find opening brace for {signature}")
    depth = 0
    end = None
    for index in range(brace, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                end = index + 1
                break
    if end is None:
        raise SystemExit(f"Could not find closing brace for {signature}")
    return source[:start] + replacement + source[end:]


main = main_path.read_text(encoding="utf-8")
boot = r"""int bootAnimation()
{
    int frame;
    int phase;
    int i;
    int dotX[8] = {196, 218, 240, 262, 284, 284, 240, 196};
    int dotY[8] = {151, 151, 151, 151, 151, 164, 164, 164};
    int firstBootState;

    // Keep the existing preference file compatible with CyanogenPSP settings.
    bootAnimActivator = setFileDefaultsInt(
        "system/boot/bootAnimActivator.bin", 1, bootAnimActivator);
    firstBootState = setFileDefaultsInt(
        "system/settings/boot.bin", 1, firstBoot);

    /* Native OSLib animation: no external image assets and no extra memory
       allocation. It is deliberately short for PSP-1000/2000/3000 hardware. */
    for (frame = 0; frame < 84 && !osl_quit; frame++) {
        phase = (frame / 4) % 8;

        oslStartDrawing();
        oslClearScreen(RGB(7, 12, 25));
        oslDrawGradientRect(0, 0, 480, 272,
            RGB(8, 16, 34), RGB(17, 31, 55),
            RGB(5, 10, 22), RGB(11, 20, 39));

        /* Quiet framing lines give the screen a modern system-startup look. */
        oslDrawFillRect(0, 0, 480, 2, RGB(35, 91, 150));
        oslDrawFillRect(0, 269, 480, 272, RGB(35, 91, 150));
        oslDrawLine(58, 55, 422, 55, RGB(28, 48, 73));
        oslDrawLine(58, 218, 422, 218, RGB(28, 48, 73));

        /* Geometric Lineage mark, drawn directly by the PSP GPU. */
        oslDrawLine(205, 70, 205, 132, RGB(61, 170, 255));
        oslDrawLine(205, 132, 275, 132, RGB(61, 170, 255));
        oslDrawLine(275, 132, 275, 105, RGB(61, 170, 255));
        oslDrawLine(222, 88, 258, 88, RGB(220, 239, 255));
        oslDrawLine(222, 88, 222, 114, RGB(220, 239, 255));
        oslDrawLine(222, 114, 253, 114, RGB(220, 239, 255));
        oslDrawRect(193, 59, 287, 144, RGB(40, 89, 139));

        oslIntraFontSetStyle(Roboto, 0.82f, WHITE, 0, INTRAFONT_ALIGN_CENTER);
        oslDrawStringf(240, 165, "LineageOS PSP");
        oslIntraFontSetStyle(Roboto, 0.42f, RGB(157, 190, 222), 0,
                             INTRAFONT_ALIGN_CENTER);
        oslDrawStringf(240, 187, "A new way to use your PSP");

        /* Eight-step cyan activity indicator. */
        for (i = 0; i < 8; i++) {
            int color = (i == phase)
                ? RGB(78, 190, 255) : RGB(40, 66, 96);
            oslDrawFillRect(dotX[i], dotY[i], dotX[i] + 7, dotY[i] + 7,
                            color);
        }

        oslDrawRect(151, 204, 329, 211, RGB(55, 92, 133));
        oslDrawFillRect(153, 206, 153 + ((frame + 1) * 174) / 84,
                        209, RGB(61, 170, 255));

        oslEndDrawing();
        oslEndFrame();
        oslSyncFrame();
        sceKernelDelayThread(25000);
    }

    if (osl_quit)
        return 0;

    if (firstBootState == 1)
        displayLangSelection("system/settings/language");

    firstBootInitiation();
    return 0;
}"""
main = replace_function(main, "int bootAnimation()", boot)
if "LineageOS PSP animated boot sequence" not in main:
    main = main.replace("int bootAnimation()", "/* LineageOS PSP animated boot sequence */\nint bootAnimation()", 1)
main_path.write_text(main, encoding="utf-8")

home = home_path.read_text(encoding="utf-8")
home_start = home.find("void home()")
if home_start < 0:
    raise SystemExit("Could not find home() in homeMenu.c")
image_marker = "\t\toslDrawImage(background);"
image_at = home.find(image_marker, home_start)
if image_at < 0:
    raise SystemExit("Could not find home wallpaper draw call")
home = home[:image_at] + image_marker + """
        /* LineageOS PSP system bar and project identity. */
        oslDrawFillRect(0, 0, 480, 23, RGB(10, 18, 34));
        oslDrawFillRect(0, 22, 480, 24, RGB(61, 150, 235));
        oslIntraFontSetStyle(Roboto, 0.46f, WHITE, 0,
                             INTRAFONT_ALIGN_LEFT);
        oslDrawStringf(16, 5, "LINEAGEOS PSP");""" + home[image_at + len(image_marker):]

# Replace the inherited warning-orange accent with the Lineage blue accent.
home = home.replace("RGB(245,81,30)", "RGB(61,150,235)")
home_path.write_text(home, encoding="utf-8")

# Fail early if the expected visual changes were not applied exactly once.
check_main = main_path.read_text(encoding="utf-8")
check_home = home_path.read_text(encoding="utf-8")
if check_main.count("LineageOS PSP animated boot sequence") != 1:
    raise SystemExit("Boot animation marker validation failed")
if check_home.count('"LINEAGEOS PSP"') != 1:
    raise SystemExit("Home-screen identity validation failed")
if "oslDrawGradientRect" not in check_main or "sceKernelDelayThread(25000)" not in check_main:
    raise SystemExit("Native boot animation validation failed")

print("LineageOS PSP interface and native boot animation applied.")
