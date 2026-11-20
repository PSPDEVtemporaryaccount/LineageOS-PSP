#include "config.h"

#include <stdio.h>
#include <pspiofilemgr.h>

#define CONFIG_DIR  "ms0:/PSP/SYSTEM/LineageOS-PSP"
#define CONFIG_FILE CONFIG_DIR "/config.ini"
#define CONFIG_TEMP CONFIG_DIR "/config.tmp"

void config_set_defaults(LineageConfig *config)
{
    if (config == NULL) return;
    config->format_version = 1;
    config->dark_theme = 1;
    config->quick_setup_done = 0;
}

int config_load(LineageConfig *config)
{
    FILE *file;
    int version = 0, dark = 1, setup = 0;

    if (config == NULL) return 0;
    config_set_defaults(config);

    file = fopen(CONFIG_FILE, "r");
    if (file == NULL) return 0;

    if (fscanf(file, "format_version=%d\ndark_theme=%d\nquick_setup_done=%d",
               &version, &dark, &setup) == 3 && version == 1) {
        config->format_version = version;
        config->dark_theme = dark ? 1 : 0;
        config->quick_setup_done = setup ? 1 : 0;
    }
    fclose(file);
    return 1;
}

int config_save(const LineageConfig *config)
{
    FILE *file;
    int written;

    if (config == NULL) return 0;

    sceIoMkdir("ms0:/PSP", 0777);
    sceIoMkdir("ms0:/PSP/SYSTEM", 0777);
    sceIoMkdir(CONFIG_DIR, 0777);

    file = fopen(CONFIG_TEMP, "w");
    if (file == NULL) return 0;

    written = fprintf(file,
        "format_version=1\ndark_theme=%d\nquick_setup_done=%d\n",
        config->dark_theme ? 1 : 0,
        config->quick_setup_done ? 1 : 0);

    if (fclose(file) != 0 || written < 0) {
        sceIoRemove(CONFIG_TEMP);
        return 0;
    }

    if (sceIoRename(CONFIG_TEMP, CONFIG_FILE) < 0) {
        sceIoRemove(CONFIG_FILE);
        if (sceIoRename(CONFIG_TEMP, CONFIG_FILE) < 0) {
            sceIoRemove(CONFIG_TEMP);
            return 0;
        }
    }
    return 1;
}
