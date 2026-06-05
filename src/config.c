#include "config.h"

#include <fontconfig/fontconfig.h>
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Module str_to_module(const char *modstr)
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

void set_config_path(char *path, size_t size)
{
    const char *configdir = "/.config/barbaris/config.lua";
    const char *homedir = getenv("HOME");
    snprintf(path, size, "%s%s", homedir, configdir);
}

void deserialize_config_root(Config *c, lua_State *L)
{
    int height, padding_x, padding_y;

    lua_getfield(L, -1, "height");
    height = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "padding_x");
    padding_x = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "padding_y");
    padding_y = lua_tointeger(L, -1);
    lua_pop(L, 1);

    c->padding_x = padding_x;
    c->padding_y = padding_y;
    c->height = height + padding_y;
}

void deserialize_color(lua_State *L, Color *color, char *key)
{
    lua_getfield(L, -1, key);

    const char *hexstr = lua_tostring(L, -1);
    int r, g, b;
    sscanf(hexstr + 1, "%02x%02x%02x", &r, &g, &b);
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = 255;

    lua_pop(L, 1);
}

void deserialize_theme(Config *c, lua_State *L)
{
    lua_getfield(L, -1, "theme");
    deserialize_color(L, &c->theme.fg, "fg");
    deserialize_color(L, &c->theme.bg, "bg");
    deserialize_color(L, &c->theme.accent, "accent");
    lua_pop(L, 1);
}

char *get_font_path(const char *family, const char *style)
{
    char *fontpath = NULL;
    char fontstr[1024];
    snprintf(fontstr, sizeof(fontstr), "%s:style=%s", family, style);

    FcConfig *fconfig = FcInitLoadConfigAndFonts();
    FcPattern *pat = FcNameParse((FcChar8 *)fontstr);
    FcConfigSubstitute(fconfig, pat, FcMatchPattern);
    FcDefaultSubstitute(pat);

    FcResult res;
    FcPattern *font = FcFontMatch(fconfig, pat, &res);

    if (font) {
        FcChar8 *fcfilepath;
        if (FcPatternGetString(font, FC_FILE, 0, &fcfilepath) == FcResultMatch)
        {
            fontpath = malloc(strlen((char *)fcfilepath) + 1);
            strcpy(fontpath, (char *)fcfilepath);
        }
        FcPatternDestroy(font);
    }
    FcPatternDestroy(pat);

    return fontpath;
}

void deserialize_font(Config *c, lua_State *L)
{
    const char *family, *style;

    lua_getfield(L, -1, "font");
    lua_getfield(L, -1, "family");
    family = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "size");
    c->fontsize = lua_tointeger(L, -1);
    lua_pop(L, 1);
    lua_getfield(L, -1, "style");
    style = lua_tostring(L, -1);
    lua_pop(L, 1);
    lua_pop(L, 1);

    // TODO: add to config instead of hardcode,
    // not all fonts have that style
    // very few have.
    c->fontpath = get_font_path(family, style);
    c->fontpath_bold = get_font_path(family, "ExtraBold");
}

typedef struct {
    Module *mod;
    int count;
} DeserializeModRes;

DeserializeModRes deserialize_module(lua_State *L, char *key)
{
    lua_getfield(L, -1, key);
    int len = lua_rawlen(L, -1);
    Module *mods = calloc(len, sizeof(Module));

    if (mods != NULL) {
        for (int i = 1; i <= len; i++) {
            lua_rawgeti(L, -1, i);
            const char *val = lua_tostring(L, -1);
            mods[i - 1] = str_to_module(val);
            lua_pop(L, 1);
        }
    }

    lua_pop(L, 1);

    return (DeserializeModRes){mods, len};
}

void deserialize_modules(Config *c, lua_State *L)
{
    lua_getfield(L, -1, "modules");

    DeserializeModRes left = deserialize_module(L, "left");
    DeserializeModRes center = deserialize_module(L, "center");
    DeserializeModRes right = deserialize_module(L, "right");

    c->modules.left = left.mod;
    c->modules.center = center.mod;
    c->modules.right = right.mod;

    c->modules.left_count = left.count;
    c->modules.center_count = center.count;
    c->modules.right_count = right.count;

    lua_pop(L, 1);
}

// TODO: Merge OR extrac common logic from wss and window

void deserialize_workspaces(Config *c, lua_State *L)
{
    lua_getfield(L, -1, "workspaces");

    lua_getfield(L, -1, "gap");
    c->workspaces.gap = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "padding_x");
    c->workspaces.padding_x = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "padding_y");
    c->workspaces.padding_y = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "roundness");
    c->workspaces.roundness = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "hover");
    c->workspaces.hover = lua_isnil(L, -1) ? true : lua_toboolean(L, -1);
    lua_pop(L, 1);

    lua_pop(L, 1);
}

void deserialize_window(Config *c, lua_State *L)
{
    lua_getfield(L, -1, "window");

    lua_getfield(L, -1, "gap");
    c->window.gap = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "padding_x");
    c->window.padding_x = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "padding_y");
    c->window.padding_y = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "roundness");
    c->window.roundness = (float)lua_tonumber(L, -1);
    lua_pop(L, 1);

    lua_getfield(L, -1, "hover");
    c->window.hover = lua_isnil(L, -1) ? true : lua_toboolean(L, -1);
    lua_pop(L, 1);

    lua_pop(L, 1);
}

void deserialize_config(Config *c, lua_State *L)
{
    deserialize_config_root(c, L);
    deserialize_theme(c, L);
    deserialize_font(c, L);
    deserialize_modules(c, L);
    deserialize_workspaces(c, L);
    deserialize_window(c, L);
}

Config *config_load()
{
    Config *c = calloc(1, sizeof(Config));
    if (c != NULL) {
        char config_path[512];
        set_config_path(config_path, sizeof(config_path));
        printf("path=%s\n", config_path);

        lua_State *L = luaL_newstate();
        luaL_openlibs(L);
        if (luaL_dofile(L, config_path) != LUA_OK) {
            fprintf(stderr, "config error: %s\n", lua_tostring(L, -1));
        }
        if (!lua_istable(L, -1)) {
            fprintf(stderr, "config error: root is not a table\n");
        }
        deserialize_config(c, L);
        lua_close(L);
    }

    return c;
}

void config_load_font(Config *c)
{
    if (c->fontpath != NULL) {
        int codepoints[256 + 256];
        int count = 0;

        for (int i = 32; i < 127; i++) codepoints[count++] = i;
        for (int i = 0x400; i < 0x500; i++) codepoints[count++] = i;

        c->font = LoadFontEx(c->fontpath, c->fontsize, codepoints, count);
        SetTextureFilter(c->font.texture, TEXTURE_FILTER_BILINEAR);
        free(c->fontpath);

        c->font_bold =
            LoadFontEx(c->fontpath_bold, c->fontsize, codepoints, count);
        SetTextureFilter(c->font_bold.texture, TEXTURE_FILTER_BILINEAR);
        free(c->fontpath_bold);
    }
}

void config_free(Config *c)
{
    if (c != NULL) {
        if (c->font.baseSize) {
            UnloadFont(c->font);
        }

        if (c->modules.left != NULL) free(c->modules.left);
        if (c->modules.center != NULL) free(c->modules.center);
        if (c->modules.right != NULL) free(c->modules.right);

        free(c);
    }
}
