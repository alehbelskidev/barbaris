#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"

typedef enum {
    MOD_WORKSPACES,
    MOD_WINDOW,
    MOD_CLOCK,
    MOD_VOLUME,
    MOD_MIC,
    MOD_DISK,
    MOD_RAM,
    MOD_EMPTY,
} Module;

typedef struct {
    Color bg;
    Color fg;
    Color accent;
} Theme;

typedef struct {
    Module* left;
    Module* center;
    Module* right;
} Modules;

typedef struct {
    int height;
    Theme theme;
    Font font;
    char* fontpath;
    int fontsize;
    Modules modules;
} Config;

Config* load_config();
void load_config_font(Config* cfg);
void free_config(Config* cfg);

#endif  // !CONFIG_H
