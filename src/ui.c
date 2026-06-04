#include "ui.h"

#include <errno.h>
#include <stdbool.h>
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
    for (int i = 0; i < ctx->s->workspaces_count; i++) {
        HyprWorkspace* ws = &ctx->s->workspaces[i];

        float left_gap = 0;
        if (*counter > 0) left_gap = ctx->c->workspaces.gap;

        Vector2 text_size =
            MeasureTextEx(ctx->c->font, ws->name, ctx->c->fontsize, 0);

        Block block = {0};

        block.padding.x = 4;
        block.padding.y = 0;

        block.size.x = text_size.x + block.padding.x * 2;
        block.size.y = text_size.y + block.padding.y * 2;

        block.offset.x = left_gap;
        block.offset.y = 0;

        block.text = ws->name;

        blocks[*counter] = block;
        (*counter)++;
    }
}

void prep_window(Context* ctx, Block* blocks, int* counter)
{
    Block block = {0};

    float left_gap = 0;
    if (*counter > 0) left_gap = ctx->c->window.gap;

    Vector2 text_size =
        MeasureTextEx(ctx->c->font, ctx->s->active_window, ctx->c->fontsize, 0);

    block.padding.x = 4;
    block.padding.y = 0;

    block.size.x = text_size.x + block.padding.x * 2;
    block.size.y = text_size.y + block.padding.y * 2;

    block.offset.x = left_gap;
    block.offset.y = 0;

    block.text = ctx->s->active_window;

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
    // TODO: global panel gap
    float left_offset = 8;
    for (int i = 0; i < left_count; i++) {
        Block* b = &left[i];

        if (b->text != NULL) {
            left_offset += b->offset.x;

            Vector2 pos = {left_offset, ctx->c->height / 2 - b->size.y / 2};

            DrawTextEx(ctx->c->font, b->text, pos, ctx->c->fontsize, 0,
                       ctx->c->theme.fg);

            left_offset += b->size.x;
        }
    }
}
