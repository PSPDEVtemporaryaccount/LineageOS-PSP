#include <pspkernel.h>
#include <pspctrl.h>
#include <pspdebug.h>

#include <stdio.h>
#include <string.h>

#include "config.h"

PSP_MODULE_INFO("LineageOS PSP", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

typedef enum { SCREEN_HOME, SCREEN_SETTINGS, SCREEN_RECOVERY } Screen;

static LineageConfig g_config;
static Screen g_screen = SCREEN_HOME;
static int g_selection = 0;
static int g_running = 1;
static char g_status[72] = "System ready";

static void status_set(const char *message)
{
    strncpy(g_status, message, sizeof(g_status) - 1);
    g_status[sizeof(g_status) - 1] = '\0';
}

static void header(const char *screen)
{
    pspDebugScreenSetTextColor(0xFFB58CFF);
    pspDebugScreenPrintf("  LINEAGEOS PSP");
    pspDebugScreenSetTextColor(0xFFAAAAAA);
    pspDebugScreenPrintf("                              0.1.0\n");
    pspDebugScreenPrintf("  Android-inspired home environment | %s\n", screen);
    pspDebugScreenPrintf("  ------------------------------------------------------------\n\n");
    pspDebugScreenSetTextColor(0xFFFFFFFF);
}

static void draw_home(void)
{
    const char *items[] = { "Applications", "Settings", "Recovery", "Power off" };
    int i;

    pspDebugScreenClear();
    header("Home");
    pspDebugScreenSetTextColor(0xFFB58CFF);
    pspDebugScreenPrintf("  Welcome to LineageOS PSP\n\n");
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPrintf("  Your PSP. A new home.\n\n");

    for (i = 0; i < 4; ++i) {
        if (i == g_selection) {
            pspDebugScreenSetTextColor(0xFFB58CFF);
            pspDebugScreenPrintf("   >  %s\n", items[i]);
        } else {
            pspDebugScreenSetTextColor(0xFFFFFFFF);
            pspDebugScreenPrintf("      %s\n", items[i]);
        }
    }

    pspDebugScreenSetTextColor(0xFF888888);
    pspDebugScreenPrintf("\n\n  D-pad: navigate   CROSS: select   CIRCLE: exit\n");
    pspDebugScreenPrintf("  %s", g_status);
    pspDebugScreenSetTextColor(0xFFFFFFFF);
}

static void draw_settings(void)
{
    pspDebugScreenClear();
    header("Settings");
    pspDebugScreenPrintf("  SYSTEM SETTINGS\n\n");

    if (g_selection == 0) pspDebugScreenSetTextColor(0xFFB58CFF);
    pspDebugScreenPrintf("   %s Appearance: %s\n",
        g_selection == 0 ? ">" : " ",
        g_config.dark_theme ? "Dark" : "Light");
    pspDebugScreenSetTextColor(0xFFFFFFFF);

    if (g_selection == 1) pspDebugScreenSetTextColor(0xFFB58CFF);
    pspDebugScreenPrintf("   %s Run Quick Setup again\n", g_selection == 1 ? ">" : " ");
    pspDebugScreenSetTextColor(0xFFFFFFFF);

    if (g_selection == 2) pspDebugScreenSetTextColor(0xFFB58CFF);
    pspDebugScreenPrintf("   %s Back\n", g_selection == 2 ? ">" : " ");
    pspDebugScreenSetTextColor(0xFFFFFFFF);

    pspDebugScreenSetTextColor(0xFF888888);
    pspDebugScreenPrintf("\n\n  UP/DOWN: navigate  CROSS: select  CIRCLE: back\n");
    pspDebugScreenPrintf("  Preferences are stored on the Memory Stick.\n");
    pspDebugScreenSetTextColor(0xFFFFFFFF);
}

static void draw_quick_setup(void)
{
    pspDebugScreenClear();
    header("Quick Setup");
    pspDebugScreenSetTextColor(0xFFB58CFF);
    pspDebugScreenPrintf("  FIRST START\n\n");
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPrintf("  Welcome! Your LineageOS PSP environment is ready.\n\n");
    pspDebugScreenPrintf("  Language: English (initial release)\n");
    pspDebugScreenPrintf("  Appearance: %s\n\n", g_config.dark_theme ? "Dark" : "Light");
    pspDebugScreenPrintf("  Press CROSS to finish setup.\n");
    pspDebugScreenPrintf("  Quick Setup will not appear on every startup.\n");
}

static void draw_recovery(void)
{
    pspDebugScreenClear();
    header("Recovery");
    pspDebugScreenSetTextColor(0xFFFFCC66);
    pspDebugScreenPrintf("  RECOVERY MODE\n\n");
    pspDebugScreenSetTextColor(0xFFFFFFFF);
    pspDebugScreenPrintf("  The normal interface is bypassed.\n\n");
    pspDebugScreenPrintf("  CROSS  - Restart the LineageOS PSP interface\n");
    pspDebugScreenPrintf("  CIRCLE - Return to the PSP XMB\n\n");
    pspDebugScreenSetTextColor(0xFF888888);
    pspDebugScreenPrintf("  Official PSP firmware and XMB remain untouched.\n");
    pspDebugScreenSetTextColor(0xFFFFFFFF);
}

static void render(void)
{
    if (!g_config.quick_setup_done) {
        draw_quick_setup();
    } else if (g_screen == SCREEN_SETTINGS) {
        draw_settings();
    } else if (g_screen == SCREEN_RECOVERY) {
        draw_recovery();
    } else {
        draw_home();
    }
}

static void handle_input(unsigned int pressed)
{
    if (!g_config.quick_setup_done) {
        if (pressed & PSP_CTRL_CROSS) {
            g_config.quick_setup_done = 1;
            if (config_save(&g_config)) status_set("Quick Setup complete. Welcome!");
            else status_set("Setup complete; Memory Stick save failed.");
            g_screen = SCREEN_HOME;
            g_selection = 0;
        }
        return;
    }

    if (g_screen == SCREEN_HOME) {
        if (pressed & PSP_CTRL_UP) g_selection = (g_selection + 3) % 4;
        else if (pressed & PSP_CTRL_DOWN) g_selection = (g_selection + 1) % 4;
        else if (pressed & PSP_CTRL_CROSS) {
            if (g_selection == 0) status_set("App library is planned for a future release.");
            else if (g_selection == 1) { g_screen = SCREEN_SETTINGS; g_selection = 0; }
            else if (g_selection == 2) g_screen = SCREEN_RECOVERY;
            else g_running = 0;
        } else if (pressed & PSP_CTRL_CIRCLE) g_running = 0;
    } else if (g_screen == SCREEN_SETTINGS) {
        if (pressed & PSP_CTRL_UP) g_selection = (g_selection + 2) % 3;
        else if (pressed & PSP_CTRL_DOWN) g_selection = (g_selection + 1) % 3;
        else if (pressed & PSP_CTRL_CROSS) {
            if (g_selection == 0) {
                g_config.dark_theme = !g_config.dark_theme;
                status_set(config_save(&g_config) ? "Appearance preference saved." :
                           "Save failed; check Memory Stick.");
            } else if (g_selection == 1) g_config.quick_setup_done = 0;
            else { g_screen = SCREEN_HOME; g_selection = 1; }
        } else if (pressed & PSP_CTRL_CIRCLE) {
            g_screen = SCREEN_HOME;
            g_selection = 1;
        }
    } else if (g_screen == SCREEN_RECOVERY) {
        if (pressed & PSP_CTRL_CROSS) { g_screen = SCREEN_HOME; g_selection = 0; }
        else if (pressed & PSP_CTRL_CIRCLE) g_running = 0;
    }
}

int main(void)
{
    SceCtrlData pad;
    unsigned int previous = 0;
    unsigned int pressed;

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL);
    pspDebugScreenInit();

    config_set_defaults(&g_config);
    config_load(&g_config);
    while (g_running) {
        sceCtrlReadBufferPositive(&pad, 1);
        pressed = pad.Buttons & ~previous;
        previous = pad.Buttons;
        handle_input(pressed);
        render();
        sceDisplayWaitVblankStart();
    }

    sceKernelExitGame();
    return 0;
}
