#include "ui.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BLOCKS 30

static Block left[MAX_BLOCKS];
static int left_count = 0;

static Block center[MAX_BLOCKS];
static int center_count = 0;

static Block right[MAX_BLOCKS];
static int right_count = 0;

void prep_workspaces(Context* ctx, Block* blocks, int* counter)
{
    Vector2 average_container_size = {0};

    for (int i = 0; i < ctx->s->workspaces_count; i++) {
        HyprWorkspace* ws = &ctx->s->workspaces[i];

        Vector2 text_size =
            MeasureTextEx(ctx->c->font, ws->name, ctx->c->fontsize, 0);

        Block block = {0};
        block.text = ws->name;

        Vector2 container_size = {text_size.x + ctx->c->workspaces.padding_x,
                                  text_size.y + ctx->c->workspaces.padding_y};

        if (container_size.x > average_container_size.x)
            average_container_size.x = container_size.x;
        if (container_size.y > average_container_size.y)
            average_container_size.y = container_size.y;

        if (average_container_size.x > average_container_size.y)
            average_container_size.y = average_container_size.x;
        if (average_container_size.y > average_container_size.x)
            average_container_size.x = average_container_size.y;

        block.container_size.x = average_container_size.x;
        block.container_size.y = average_container_size.y;
        block.text_size = text_size;
        block.gap = ctx->c->workspaces.gap;

        blocks[*counter] = block;
        (*counter)++;
    }
}

void prep_window(Context* ctx, Block* blocks, int* counter)
{
    Block block = {0};
    block.text = ctx->s->active_window;

    Vector2 text_size =
        MeasureTextEx(ctx->c->font, ctx->s->active_window, ctx->c->fontsize, 0);
    Vector2 container_size = {text_size.x + ctx->c->window.padding_x,
                              text_size.y + ctx->c->window.padding_y};

    block.container_size = container_size;
    block.text_size = text_size;
    block.gap = ctx->c->window.gap;

    blocks[*counter] = block;
    (*counter)++;
}

void prep_module(Context* ctx, Module* m, int count, Block* blocks,
                 int* counter)
{
    for (int i = 0; i < count; i++) {
        if (m[i] == MOD_WORKSPACES) {
            prep_workspaces(ctx, blocks, counter);
        }
        if (m[i] == MOD_WINDOW) {
            prep_window(ctx, blocks, counter);
        }
    }
}

void ui_prep(Context* ctx)
{
    if (ctx->s->is_dirty) {
        left_count = 0;
        center_count = 0;
        right_count = 0;
        prep_module(ctx, ctx->c->modules.left, ctx->c->modules.left_count, left,
                    &left_count);

        ctx->s->is_dirty = false;
    }
}

void ui_draw(Context* ctx)
{
    float left_offset = ctx->c->padding_x;

    for (int i = 0; i < left_count; i++) {
        Block* b = &left[i];
        left_offset += b->gap;
        Vector2 pos = {left_offset,
                       ctx->c->height / 2 - b->container_size.y / 2};

        Rectangle crect = {pos.x, pos.y, b->container_size.x,
                           b->container_size.y};
        bool hovered = CheckCollisionPointRec(ctx->mouse_pos, crect);
        Vector2 text_pos = {
            pos.x + b->container_size.x / 2 - b->text_size.x / 2,
            pos.y + b->container_size.y / 2 - b->text_size.y / 2};

        Color textcolor = ctx->c->theme.fg;
        if (hovered) {
            DrawRectangleRec(crect, ctx->c->theme.fg);
            textcolor = ctx->c->theme.bg;
        }

        if (b->text != NULL) {
            DrawTextEx(ctx->c->font, b->text, text_pos, ctx->c->fontsize, 0,
                       textcolor);
        }

        left_offset += b->container_size.x;
    }
}
