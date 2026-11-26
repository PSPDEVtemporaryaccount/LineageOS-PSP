#!/usr/bin/env python3
"""Route LineageOS PSP's Internet icon to the bundled Tilefinch launcher."""

from pathlib import Path
import sys

if len(sys.argv) != 2:
    raise SystemExit("usage: launch_tilefinch.py <CyanogenPSP>/homeMenu.c")

path = Path(sys.argv[1])
source = path.read_text(encoding="utf-8")
start_marker = "void internet() //Draws the browser"
end_marker = "\nvoid debugDisplay()"
if source.count(start_marker) != 1 or source.count(end_marker) < 1:
    raise SystemExit("CyanogenPSP internet() layout changed; refusing an unsafe patch")

start = source.index(start_marker)
end = source.index(end_marker, start)
replacement = """void internet() // Launches the integrated Tilefinch browser
{
    const char *browserPath = "ms0:/PSP/GAME/TILEFINCH/EBOOT.PBP";
    SceKernelLoadExecParam launch;
    memset(&launch, 0, sizeof(launch));
    launch.size = sizeof(launch);

    int result = sceKernelLoadExec(browserPath, &launch);
    if (result < 0)
    {
        oslStartDrawing();
        oslClearScreen(RGB(12, 18, 30));
        oslIntraFontSetStyle(Roboto, 0.65f, WHITE, 0, 0, INTRAFONT_ALIGN_CENTER);
        oslDrawStringf(240, 92, "Tilefinch is not installed");
        oslIntraFontSetStyle(Roboto, 0.48f, WHITE, 0, 0, INTRAFONT_ALIGN_CENTER);
        oslDrawStringf(240, 122, "Copy the TILEFINCH folder to PSP/GAME/");
        oslDrawStringf(240, 146, "Then reopen Internet from LineageOS PSP");
        oslEndDrawing();
        oslEndFrame();
        oslSyncFrame();
        sceKernelDelayThread(2500000);
    }
}
"""
path.write_text(source[:start] + replacement + source[end:], encoding="utf-8")
print("LineageOS PSP Internet entry now launches TILEFINCH/EBOOT.PBP")
