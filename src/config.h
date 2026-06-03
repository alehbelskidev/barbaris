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
    int left_count;
    int center_count;
    int right_count;
} Modules;

typedef struct {
    int height;
    Theme theme;
    Font font;
    char* fontpath;
    int fontsize;
    Modules modules;
} Config;

void load_config();
void load_config_font();
void free_config();

extern Config* config;

#endif  // !CONFIG_H
