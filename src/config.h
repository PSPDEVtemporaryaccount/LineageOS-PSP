#ifndef LINEAGEOS_CONFIG_H
#define LINEAGEOS_CONFIG_H

#define LINEAGEOS_VERSION "0.2.0"

typedef struct {
    int format_version;
    int dark_theme;
    int quick_setup_done;
} LineageConfig;

void config_set_defaults(LineageConfig *config);
int config_load(LineageConfig *config);
int config_save(const LineageConfig *config);

#endif
