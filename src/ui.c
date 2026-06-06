// TODO: Block parse is similar, merge functionalities

#include "ui.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "__debug.h"

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
        block.text = ws->name;

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
    block.text = ctx->s->active_window;

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
    block.text = ctx->s->time;

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

void prep_module(Context *ctx, Module *m, int count, Block *blocks,
                 int *counter)
{
    for (int i = 0; i < count; i++) {
        if (m[i] == MOD_WORKSPACES) {
            prep_workspaces(ctx, blocks, counter);
        }
        if (m[i] == MOD_WINDOW) {
            prep_window(ctx, blocks, counter);
        }
        if (m[i] == MOD_CLOCK) {
            prep_clock(ctx, blocks, counter);
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

        DEBUG_config(ctx->c);
        DEBUG_state(ctx->s);
    }
}

void draw_module(Context *ctx, float start_offset, Block *blocks, float *alphas,
                 int count)
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
        } else if (b->workspace_id == ctx->s->active_workspace_id) {
            DrawRectangleRounded(crect, b->roundness, 3, ctx->c->theme.fg);
            textcolor = ctx->c->theme.bg;
        }

        if (b->text != NULL) {
            Font *font = b->workspace_id == ctx->s->active_workspace_id
                             ? &ctx->c->font_bold
                             : &ctx->c->font;

            DrawTextEx(*font, b->text, text_pos, ctx->c->fontsize, 0,
                       textcolor);
        }

        offset += b->container_size.x;
    }
}

void draw_left(Context *ctx)
{
    float left_offset = ctx->c->bar.padding_x;
    draw_module(ctx, left_offset, left, left_alphas, left_count);
}

void draw_center(Context *ctx)
{
    float total_width = 0;

    for (int i = 0; i < center_count; i++) {
        Block *b = &center[i];
        total_width += b->container_size.x + b->gap;
    }
    float center_offset =
        (GetScreenWidth() - ctx->c->bar.padding_x * 2) / 2 - (total_width / 2);

    draw_module(ctx, center_offset, center, center_alphas, center_count);
}

void ui_draw(Context *ctx)
{
    draw_left(ctx);
    draw_center(ctx);
}
