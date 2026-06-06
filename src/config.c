// TODO: styled objects are similare merge similar deserialize functions

#include "config.h"

#include <errno.h>
#include <fontconfig/fontconfig.h>
#include <inttypes.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tomlc17.h>

static void error(const char *msg, const char *msg1)
{
    fprintf(stderr, "ERROR: %s%s\n", msg, msg1 ? msg1 : "");
    exit(1);
}

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
    const char *configdir = "/.config/barbaris/config.toml";
    const char *homedir = getenv("HOME");
    snprintf(path, size, "%s%s", homedir, configdir);
}

void deserialize_style(Styles *s, toml_result_t *r, const char *key)
{
    char buf[64];
    toml_datum_t d;

    sprintf(buf, "%s.gap", key);
    d = toml_seek(r->toptab, buf);
    if (d.type == TOML_INT64) s->gap = d.u.int64;

    sprintf(buf, "%s.padding.x", key);
    d = toml_seek(r->toptab, buf);
    if (d.type == TOML_INT64) s->padding_x = d.u.int64;

    sprintf(buf, "%s.padding.y", key);
    d = toml_seek(r->toptab, buf);
    if (d.type == TOML_INT64) s->padding_y = d.u.int64;

    sprintf(buf, "%s.roundness", key);
    d = toml_seek(r->toptab, buf);
    if (d.type == TOML_FP64) s->roundness = d.u.fp64;

    sprintf(buf, "%s.hover", key);
    d = toml_seek(r->toptab, buf);
    if (d.type == TOML_BOOLEAN) s->hover = d.u.boolean;
}

void deserialize_config_root(Config *c, toml_result_t *r)
{
    int height = 32;
    toml_datum_t d = toml_seek(r->toptab, "height");
    if (d.type == TOML_INT64) height = d.u.int64;

    deserialize_style(&c->bar, r, "bar");

    c->height = height + c->bar.padding_y;
}

void deserialize_color(toml_result_t *r, Color *color, const char *key)
{
    char buf[64];
    toml_datum_t d;
    int red = 0, green = 0, blue = 0;
    char hexstr[8];

    sprintf(buf, "theme.%s", key);
    d = toml_seek(r->toptab, buf);
    if (d.type == TOML_STRING) {
        strncpy(hexstr, d.u.s, sizeof(hexstr) - 1);
        sscanf(hexstr + 1, "%02x%02x%02x", &red, &green, &blue);
    }

    color->r = red;
    color->g = green;
    color->b = blue;
    color->a = 255;
}

void deserialize_theme(Config *c, toml_result_t *r)
{
    deserialize_color(r, &c->theme.fg, "fg");
    deserialize_color(r, &c->theme.bg, "bg");
    deserialize_color(r, &c->theme.accent, "accent");
}

char *get_font_path(FcConfig *fconfig, const char *family, const char *style)
{
    char *fontpath = NULL;
    char fontstr[1024];
    snprintf(fontstr, sizeof(fontstr), "%s:style=%s", family, style);

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

void deserialize_font(Config *c, toml_result_t *r)
{
    FcConfig *fconfig = FcInitLoadConfigAndFonts();
    char family[128];
    toml_datum_t d;

    d = toml_seek(r->toptab, "font.size");
    c->fontsize = d.type == TOML_INT64 ? d.u.int64 : 14;

    d = toml_seek(r->toptab, "font.family");
    const char *fam = d.type == TOML_STRING ? d.u.s : "monospace";
    strncpy(family, fam, sizeof(family));

    c->fontpath = get_font_path(fconfig, family, "Regular");
    c->fontpath_bold = get_font_path(fconfig, family, "ExtraBold");
}

typedef struct {
    Module *mod;
    int count;
} DeserializeModRes;

DeserializeModRes deserialize_module(toml_result_t *r, char *key)
{
    char buf[64];
    sprintf(buf, "modules.%s", key);

    toml_datum_t d = toml_seek(r->toptab, buf);
    if (d.type != TOML_ARRAY) return (DeserializeModRes){NULL, 0};

    Module *mods = calloc(d.u.arr.size, sizeof(Module));
    if (mods == NULL) return (DeserializeModRes){NULL, 0};

    for (int i = 0; i < d.u.arr.size; i++) {
        toml_datum_t elm = d.u.arr.elem[i];
        if (elm.type == TOML_STRING) {
            mods[i] = str_to_module(elm.u.s);
        }
    }

    return (DeserializeModRes){mods, d.u.arr.size};
}

void deserialize_modules(Config *c, toml_result_t *r)
{
    DeserializeModRes left = deserialize_module(r, "left");
    DeserializeModRes center = deserialize_module(r, "center");
    DeserializeModRes right = deserialize_module(r, "right");

    c->modules.left = left.mod;
    c->modules.center = center.mod;
    c->modules.right = right.mod;

    c->modules.left_count = left.count;
    c->modules.center_count = center.count;
    c->modules.right_count = right.count;
}

void deserialize_workspaces(Config *c, toml_result_t *r)
{
    deserialize_style(&c->workspaces, r, "workspaces");
}

void deserialize_window(Config *c, toml_result_t *r)
{
    deserialize_style(&c->window, r, "window");
}

void deserialize_clock(Config *c, toml_result_t *r)
{
    toml_datum_t d = toml_seek(r->toptab, "clock.format");
    const char *fmt = d.type == TOML_STRING ? d.u.s : "%H:%M:%S, %d.%m.%Y";
    strncpy(c->clock_format, fmt, sizeof(c->clock_format) - 1);
    c->clock_format[sizeof(c->clock_format) - 1] = '\0';
    deserialize_style(&c->clock, r, "clock");
}

void deserialize_config(Config *c, toml_result_t *r)
{
    deserialize_config_root(c, r);
    deserialize_theme(c, r);
    deserialize_font(c, r);
    deserialize_modules(c, r);
    deserialize_workspaces(c, r);
    deserialize_window(c, r);
    deserialize_clock(c, r);
}

Config *config_load()
{
    Config *c = calloc(1, sizeof(Config));
    if (c != NULL) {
        char config_path[512];
        set_config_path(config_path, sizeof(config_path));

        toml_result_t result = toml_parse_file_ex(config_path);
        if (!result.ok) error(result.errmsg, 0);
        deserialize_config(c, &result);
        toml_free(result);
    }

    return c;
}

void config_load_font(Config *c)
{
    if (c->fontpath != NULL) {
        int codepoints[20000];
        int count = 0;

        // ASCII
        for (int i = 32; i < 127; i++) codepoints[count++] = i;
        // Cyrillic
        for (int i = 0x400; i < 0x500; i++) codepoints[count++] = i;

        c->font = LoadFontEx(c->fontpath, c->fontsize, codepoints, count);
        SetTextureFilter(c->font.texture, TEXTURE_FILTER_BILINEAR);
        free(c->fontpath);
        c->fontpath = NULL;

        c->font_bold =
            LoadFontEx(c->fontpath_bold, c->fontsize, codepoints, count);
        SetTextureFilter(c->font_bold.texture, TEXTURE_FILTER_BILINEAR);
        free(c->fontpath_bold);
        c->fontpath_bold = NULL;
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
