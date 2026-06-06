// TODO: Block parse is similar, merge functionalities

#include "ui.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BLOCKS 30

static Block left[MAX_BLOCKS];
static float left_alphas[MAX_BLOCKS] = {0};
static int left_count = 0;

static Block center[MAX_BLOCKS];
static float center_alphas[MAX_BLOCKS] = {0};
static int center_count = 0;

static Block right[MAX_BLOCKS];
static float right_alphas[MAX_BLOCKS] = {0};
static int right_count = 0;

#define ANIM_SPEED 5.5f

void prep_workspaces(Context *ctx, Block *blocks, int *counter)
{
    float max_side = 0.0f;

    for (int i = 0; i < ctx->s->workspaces_count; i++) {
        HyprWorkspace *ws = &ctx->s->workspaces[i];

        Vector2 text_size =
            MeasureTextEx(ctx->c->font, ws->name, ctx->c->fontsize, 0);

        float width_with_padding = text_size.x + ctx->c->workspaces.padding_x;
        float height_with_padding = text_size.y + ctx->c->workspaces.padding_y;

        float current_max = (width_with_padding > height_with_padding)
                                ? width_with_padding
                                : height_with_padding;

        if (current_max > max_side) {
            max_side = current_max;
        }
    }

    for (int i = 0; i < ctx->s->workspaces_count; i++) {
        HyprWorkspace *ws = &ctx->s->workspaces[i];

        Vector2 text_size =
            MeasureTextEx(ctx->c->font, ws->name, ctx->c->fontsize, 0);

        Block block = {0};
        strncpy(block.text, ws->name, sizeof(block.text));
        block.text[sizeof(block.text) - 1] = '\0';

        block.container_size.x = max_side;
        block.container_size.y = max_side;

        block.text_size = text_size;
        block.gap = ctx->c->workspaces.gap;
        block.hover = ctx->c->workspaces.hover;
        block.roundness = ctx->c->workspaces.roundness;
        block.mod = MOD_WORKSPACES;
        block.workspace_id = ws->id;

        blocks[*counter] = block;
        (*counter)++;
    }
}

void prep_window(Context *ctx, Block *blocks, int *counter)
{
    Block block = {0};
    strncpy(block.text, ctx->s->active_window, sizeof(block.text));
    block.text[sizeof(block.text) - 1] = '\0';

    Vector2 text_size =
        MeasureTextEx(ctx->c->font, block.text, ctx->c->fontsize, 0);
    Vector2 container_size = {text_size.x + ctx->c->window.padding_x,
                              text_size.y + ctx->c->window.padding_y};

    block.container_size = container_size;
    block.text_size = text_size;
    block.gap = ctx->c->window.gap;
    block.hover = ctx->c->window.hover;
    block.roundness = ctx->c->window.roundness;
    block.mod = MOD_WINDOW;

    blocks[*counter] = block;
    (*counter)++;
}

void prep_clock(Context *ctx, Block *blocks, int *counter)
{
    Block block = {0};
    strncpy(block.text, ctx->s->time, sizeof(block.text));
    block.text[sizeof(block.text) - 1] = '\0';

    Vector2 text_size =
        MeasureTextEx(ctx->c->font, block.text, ctx->c->fontsize, 0);
    Vector2 container_size = {text_size.x + ctx->c->clock.padding_x,
                              text_size.y + ctx->c->clock.padding_y};

    block.container_size = container_size;
    block.text_size = text_size;
    block.gap = ctx->c->clock.gap;
    block.hover = ctx->c->clock.hover;
    block.roundness = ctx->c->clock.roundness;
    block.mod = MOD_CLOCK;

    blocks[*counter] = block;
    (*counter)++;
}

void format_wifi_state(char *dest, size_t dest_size, const char *essid,
                       int signal_dbm, char wifi_levels[6][8])
{
    const char *icon = "󰤭";

    if (!dest || dest_size == 0) {
        return;
    }

    if (!essid) {
        essid = "No Link";
    }

    if (signal_dbm <= -100 || strcmp(essid, "Disconnected") == 0 ||
        strcmp(essid, "No Link") == 0 || strlen(essid) == 0)
    {
        icon = "󰤭";
        snprintf(dest, dest_size, "%s %s", icon, "Disconnected");
        return;
    }

    int index = 0;
    if (signal_dbm >= -85) index = 1;
    if (signal_dbm >= -75) index = 2;
    if (signal_dbm >= -65) index = 3;
    if (signal_dbm >= -55) index = 4;
    if (signal_dbm >= -45) index = 5;

    icon = wifi_levels[index];

    snprintf(dest, dest_size, "%s %s", icon, essid);
}

void prep_wifi(Context *ctx, Block *blocks, int *counter)
{
    Block block = {0};

    format_wifi_state(block.text, sizeof(block.text), ctx->s->essid,
                      ctx->s->signal_dbm, ctx->c->wifi_levels);

    Vector2 text_size =
        MeasureTextEx(ctx->c->font, block.text, ctx->c->fontsize, 0);

    Vector2 container_size = {text_size.x + ctx->c->wifi.padding_x,
                              text_size.y + ctx->c->wifi.padding_y};

    block.container_size = container_size;
    block.text_size = text_size;
    block.gap = ctx->c->wifi.gap;
    block.hover = ctx->c->wifi.hover;
    block.roundness = ctx->c->wifi.roundness;
    block.mod = MOD_WIFI;

    blocks[*counter] = block;
    (*counter)++;
}

void prep_module(Context *ctx, Module *m, int count, Block *blocks,
                 int *counter)
{
    for (int i = 0; i < count; i++) {
        if (m[i] == MOD_WORKSPACES) {
            prep_workspaces(ctx, blocks, counter);
        } else if (m[i] == MOD_WINDOW) {
            prep_window(ctx, blocks, counter);
        } else if (m[i] == MOD_CLOCK) {
            prep_clock(ctx, blocks, counter);
        } else if (m[i] == MOD_WIFI) {
            prep_wifi(ctx, blocks, counter);
        }
    }
}

void ui_prep(Context *ctx)
{
    if (ctx->s->is_dirty) {
        left_count = 0;
        center_count = 0;
        right_count = 0;

        prep_module(ctx, ctx->c->modules.left, ctx->c->modules.left_count, left,
                    &left_count);
        prep_module(ctx, ctx->c->modules.center, ctx->c->modules.center_count,
                    center, &center_count);
        prep_module(ctx, ctx->c->modules.right, ctx->c->modules.right_count,
                    right, &right_count);

        ctx->s->is_dirty = false;
    }
}

void draw_module(Context *ctx, float start_offset, Block *blocks, float *alphas,
                 int count, void (*hypr_dispatch)(const char *))
{
    if (count <= 0) return;

    float offset = start_offset;

    for (int i = 0; i < count; i++) {
        Block *b = &blocks[i];

        offset += b->gap;
        Vector2 pos = {offset, ctx->c->height / 2 - b->container_size.y / 2};

        Rectangle crect = {pos.x, pos.y, b->container_size.x,
                           b->container_size.y};
        bool hovered = CheckCollisionPointRec(ctx->mouse_pos, crect);
        Vector2 text_pos = {
            pos.x + b->container_size.x / 2 - b->text_size.x / 2,
            pos.y + b->container_size.y / 2 - b->text_size.y / 2};

        Color textcolor = ctx->c->theme.fg;

        if (hovered && b->hover) {
            alphas[i] += ANIM_SPEED * ctx->delta_time;
        } else {
            alphas[i] -= ANIM_SPEED * ctx->delta_time;
        }
        if (alphas[i] < 0) alphas[i] = 0;
        if (alphas[i] > 1) alphas[i] = 1;

        if (hovered && b->hover) {
            Color fg = ctx->c->theme.fg;
            fg.a = (unsigned char)(alphas[i] * 255);

            DrawRectangleRounded(crect, b->roundness, 3, fg);
            textcolor = ctx->c->theme.bg;

            if (ctx->left_clicked && b->mod == MOD_WORKSPACES &&
                b->workspace_id != ctx->s->active_workspace_id)
            {
                char cmd[64];
                snprintf(cmd, 64, "/dispatch workspace %d", b->workspace_id);
                hypr_dispatch(cmd);
            }
        } else if (b->workspace_id == ctx->s->active_workspace_id) {
            DrawRectangleRounded(crect, b->roundness, 3, ctx->c->theme.fg);
            textcolor = ctx->c->theme.bg;
        }

        if (b->text[0] != '\0') {
            Font *font = b->workspace_id == ctx->s->active_workspace_id
                             ? &ctx->c->font_bold
                             : &ctx->c->font;

            DrawTextEx(*font, b->text, text_pos, ctx->c->fontsize, 0,
                       textcolor);
        }

        offset += b->container_size.x;
    }
}

void draw_left(Context *ctx, void (*hypr_dispatch)(const char *))
{
    float offset = ctx->c->bar.padding_x;
    draw_module(ctx, offset, left, left_alphas, left_count, hypr_dispatch);
}

void draw_center(Context *ctx, void (*hypr_dispatch)(const char *))
{
    float total_width = 0;

    for (int i = 0; i < center_count; i++) {
        Block *b = &center[i];
        total_width += b->container_size.x + b->gap;
    }
    float offset =
        (GetScreenWidth() - ctx->c->bar.padding_x * 2) / 2 - (total_width / 2);

    draw_module(ctx, offset, center, center_alphas, center_count,
                hypr_dispatch);
}

void draw_right(Context *ctx, void (*hypr_dispatch)(const char *))
{
    float total_width = 0;

    for (int i = 0; i < right_count; i++) {
        Block *b = &right[i];
        total_width += b->container_size.x + b->gap;
    }
    float offset = GetScreenWidth() - ctx->c->bar.padding_x - total_width;

    draw_module(ctx, offset, right, right_alphas, right_count, hypr_dispatch);
}

void ui_draw(Context *ctx, void (*hypr_dispatch)(const char *))
{
    draw_left(ctx, hypr_dispatch);
    draw_center(ctx, hypr_dispatch);
    draw_right(ctx, hypr_dispatch);
}
