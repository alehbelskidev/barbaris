#include "config.h"

#include <fontconfig/fontconfig.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
                left = { "workspaces" },
                center = { "clock" },
                right = { "volume", "disk" },
        },
}
*/
void set_config_path(char* path, size_t size)
{
    const char* configdir = "/.config/barbaris/config.lua";
    const char* homedir = getenv("HOME");
    snprintf(path, size, "%s%s", homedir, configdir);
}

void deserialize_config_root(lua_State* L, Config* cfg)
{
    lua_getfield(L, -1, "height");
    cfg->height = lua_tointeger(L, -1);
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

void deserialize_theme(lua_State* L, Config* cfg)
{
    lua_getfield(L, -1, "theme");
    deserialize_color(L, &cfg->theme.fg, "fg");
    deserialize_color(L, &cfg->theme.bg, "bg");
    deserialize_color(L, &cfg->theme.accent, "accent");
    lua_pop(L, 1);
}

char* get_font_path(const char* family, const char* style)
{
    char* fontpath = NULL;
    char fontstr[1024];
    snprintf(fontstr, sizeof(fontstr), "%s:style=%s", family, style);

    FcConfig* fcfg = FcInitLoadConfigAndFonts();
    FcPattern* pat = FcNameParse((FcChar8*)family);
    FcConfigSubstitute(fcfg, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);

    FcResult res;
    FcPattern* font = FcFontMatch(fcfg, pat, &res);

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

void deserialize_font(lua_State* L, Config* cfg)
{
    const char *family, *style;

    lua_getfield(L, -1, "font");
    lua_getfield(L, -1, "family");
    family = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "size");
    cfg->fontsize = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "style");
    style = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_pop(L, 1);

    cfg->fontpath = get_font_path(family, style);
}

void deserialize_config(lua_State* L, Config* cfg)
{
    deserialize_config_root(L, cfg);
    deserialize_theme(L, cfg);
    deserialize_font(L, cfg);
}

Config* load_config()
{
    Config* cfg = calloc(1, sizeof(Config));

    if (cfg != NULL) {
        char config_path[512];
        set_config_path(config_path, sizeof(config_path));
        printf("path=%s\n", config_path);

        lua_State* L = luaL_newstate();
        luaL_openlibs(L);
        if (luaL_dofile(L, config_path) != LUA_OK) {
            fprintf(stderr, "config error: %s\n", lua_tostring(L, -1));
            return NULL;
        }
        if (!lua_istable(L, -1)) {
            fprintf(stderr, "config error: root is not a table\n");
            return NULL;
        }
        deserialize_config(L, cfg);
        lua_close(L);
    }

    return cfg;
}

void load_config_font(Config* cfg)
{
    if (cfg->fontpath != NULL) {
        cfg->font = LoadFontEx(cfg->fontpath, cfg->fontsize, NULL, 0);
        SetTextureFilter(cfg->font.texture, TEXTURE_FILTER_BILINEAR);
        free(cfg->fontpath);
    }
}

void free_config(Config* cfg)
{
    if (cfg != NULL) {
        if (cfg->font.baseSize) {
            UnloadFont(cfg->font);
        }

        free(cfg);
    }
}
