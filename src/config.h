#ifndef CONFIG_H
#define CONFIG_H

#include "raylib.h"

typedef enum {
    MOD_WORKSPACES,
    MOD_WINDOW,
    MOD_CLOCK,
    MOD_WIFI,
    MOD_VOLUME,
    MOD_DISKS,
    MOD_PROC,
    MOD_RAM,
    MOD_SYSTEM,
    MOD_EMPTY,
} Module;

typedef enum {
    WSIGNAL_ERR,
    WSIGNAL_0,
    WSIGNAL_25,
    WSIGNAL_50,
    WSIGNAL_75,
    WSIGNAL_100
} WSignal;

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

    Theme theme;
    Font font;
    Font font_bold;
    char *fontpath;
    char *fontpath_bold;
    int fontsize;

    Modules modules;

    Styles bar;
    Styles workspaces;
    Styles window;
    Styles clock;
    Styles wifi;

    char clock_format[64];
    char wifi_levels[6][8];
} Config;

Config *config_load();
void config_load_font(Config *c);
void config_free(Config *c);

#endif  // !CONFIG_H
