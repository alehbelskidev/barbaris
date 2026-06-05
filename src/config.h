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
    Module *left;
    Module *center;
    Module *right;
    int left_count;
    int center_count;
    int right_count;
} Modules;

typedef struct {
    int gap;

    int padding_x;
    int padding_y;

    float roundness;

    bool hover;
} Styles;

typedef struct {
    int height;
    int padding_x;
    int padding_y;

    Theme theme;
    Font font;
    Font font_bold;
    char *fontpath;
    char *fontpath_bold;
    int fontsize;

    Modules modules;

    Styles workspaces;
    Styles window;
} Config;

Config *config_load();
void config_load_font(Config *c);
void config_free(Config *c);

#endif  // !CONFIG_H
