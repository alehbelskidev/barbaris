#include "config.h"

#include <fontconfig/fontconfig.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config* config;
/*
return {
        height = 32,

        theme = {
                bg = "#1e1e2e",
                fg = "#cdd6f4",
                accent = "#89b4fa",
        },

        font = {
                path = "Iosevka Nerd Font",
                size = 14,
                style = "Regular",
        },

        modules = {
                left = { "workspaces", "window" },
                center = { "clock" },
                right = { "volume", "disk" },
        },
}
*/

Module str_to_module(const char* modstr)
{
    if (strcmp(modstr, "workspaces") == 0) return MOD_WORKSPACES;
    if (strcmp(modstr, "window") == 0) return MOD_WINDOW;
    if (strcmp(modstr, "clock") == 0) return MOD_CLOCK;
    if (strcmp(modstr, "volume") == 0) return MOD_VOLUME;
    if (strcmp(modstr, "mic") == 0) return MOD_MIC;
    if (strcmp(modstr, "disk") == 0) return MOD_DISK;
    if (strcmp(modstr, "ram") == 0) return MOD_RAM;
    if (strcmp(modstr, "empty") == 0) return MOD_EMPTY;

    return MOD_EMPTY;
}

void set_config_path(char* path, size_t size)
{
    const char* configdir = "/.config/barbaris/config.lua";
    const char* homedir = getenv("HOME");
    snprintf(path, size, "%s%s", homedir, configdir);
}

void deserialize_config_root(lua_State* L)
{
    lua_getfield(L, -1, "height");
    config->height = lua_tointeger(L, -1);
    lua_pop(L, 1);
}

void deserialize_color(lua_State* L, Color* color, char* key)
{
    lua_getfield(L, -1, key);

    const char* hexstr = lua_tostring(L, -1);
    int r, g, b;
    sscanf(hexstr + 1, "%02x%02x%02x", &r, &g, &b);
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = 255;

    lua_pop(L, 1);
}

void deserialize_theme(lua_State* L)
{
    lua_getfield(L, -1, "theme");
    deserialize_color(L, &config->theme.fg, "fg");
    deserialize_color(L, &config->theme.bg, "bg");
    deserialize_color(L, &config->theme.accent, "accent");
    lua_pop(L, 1);
}

char* get_font_path(const char* family, const char* style)
{
    char* fontpath = NULL;
    char fontstr[1024];
    snprintf(fontstr, sizeof(fontstr), "%s:style=%s", family, style);

    FcConfig* fconfig = FcInitLoadConfigAndFonts();
    FcPattern* pat = FcNameParse((FcChar8*)family);
    FcConfigSubstitute(fconfig, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);

    FcResult res;
    FcPattern* font = FcFontMatch(fconfig, pat, &res);

    if (font) {
        FcChar8* fcfilepath;
        if (FcPatternGetString(font, FC_FILE, 0, &fcfilepath) == FcResultMatch)
        {
            fontpath = malloc(strlen((char*)fcfilepath) + 1);
            strcpy(fontpath, (char*)fcfilepath);
        }
        FcPatternDestroy(font);
    }
    FcPatternDestroy(pat);

    return fontpath;
}

void deserialize_font(lua_State* L)
{
    const char *family, *style;

    lua_getfield(L, -1, "font");
    lua_getfield(L, -1, "family");
    family = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "size");
    config->fontsize = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "style");
    style = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_pop(L, 1);

    config->fontpath = get_font_path(family, style);
}

typedef struct {
    Module* mod;
    int count;
} DeserializeModRes;

DeserializeModRes deserialize_module(lua_State* L, char* key)
{
    lua_getfield(L, -1, key);
    int len = lua_rawlen(L, -1);
    Module* mods = calloc(len, sizeof(Module));

    if (mods != NULL) {
        for (int i = 1; i <= len; i++) {
            lua_rawgeti(L, -1, i);
            const char* val = lua_tostring(L, -1);
            mods[i - 1] = str_to_module(val);
            lua_pop(L, 1);
        }
    }

    lua_pop(L, 1);

    return (DeserializeModRes){mods, len};
}

void deserialize_modules(lua_State* L)
{
    lua_getfield(L, -1, "modules");

    DeserializeModRes left = deserialize_module(L, "left");
    DeserializeModRes center = deserialize_module(L, "center");
    DeserializeModRes right = deserialize_module(L, "right");

    config->modules.left = left.mod;
    config->modules.center = center.mod;
    config->modules.right = right.mod;

    config->modules.left_count = left.count;
    config->modules.center_count = center.count;
    config->modules.right_count = right.count;

    lua_pop(L, 1);
}

void deserialize_workspaces(lua_State* L)
{
    lua_getfield(L, -1, "workspaces");

    lua_getfield(L, -1, "gap");
    config->workspaces.gap = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pop(L, 1);
}

void deserialize_window(lua_State* L)
{
    lua_getfield(L, -1, "window");

    lua_getfield(L, -1, "gap");
    config->window.gap = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_pop(L, 1);
}

void deserialize_config(lua_State* L)
{
    deserialize_config_root(L);
    deserialize_theme(L);
    deserialize_font(L);
    deserialize_modules(L);
    deserialize_workspaces(L);
    deserialize_window(L);
}

void load_config()
{
    config = calloc(1, sizeof(Config));
    if (config != NULL) {
        char config_path[512];
        set_config_path(config_path, sizeof(config_path));
        printf("path=%s\n", config_path);

        lua_State* L = luaL_newstate();
        luaL_openlibs(L);
        if (luaL_dofile(L, config_path) != LUA_OK) {
            fprintf(stderr, "config error: %s\n", lua_tostring(L, -1));
        }
        if (!lua_istable(L, -1)) {
            fprintf(stderr, "config error: root is not a table\n");
        }
        deserialize_config(L);
        lua_close(L);
    }
}

void load_config_font()
{
    if (config->fontpath != NULL) {
        int codepoints[256 + 256];
        int count = 0;

        for (int i = 32; i < 127; i++) codepoints[count++] = i;
        for (int i = 0x400; i < 0x500; i++) codepoints[count++] = i;

        config->font =
            LoadFontEx(config->fontpath, config->fontsize, codepoints, count);
        SetTextureFilter(config->font.texture, TEXTURE_FILTER_BILINEAR);
        free(config->fontpath);
    }
}

void free_config()
{
    if (config != NULL) {
        if (config->font.baseSize) {
            UnloadFont(config->font);
        }

        if (config->modules.left != NULL) free(config->modules.left);
        if (config->modules.center != NULL) free(config->modules.center);
        if (config->modules.right != NULL) free(config->modules.right);

        free(config);
    }
}
