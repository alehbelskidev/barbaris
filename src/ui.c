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

void DEBUG_block(Block* b)
{
    char mod[10];
    switch (b->mod) {
        case MOD_WORKSPACES:
            strcpy(mod, "Workspaces");
            break;
        case MOD_WINDOW:
            strcpy(mod, "Window");
            break;
        case MOD_CLOCK:
            strcpy(mod, "Clock");
            break;
        case MOD_VOLUME:
            strcpy(mod, "Volume");
            break;
        case MOD_MIC:
            strcpy(mod, "Mic");
            break;
        case MOD_DISK:
            strcpy(mod, "Disk");
            break;
        case MOD_RAM:
            strcpy(mod, "RAM");
            break;
        case MOD_EMPTY:
        default:
            strcpy(mod, "Empty");
            break;
    }

    printf("DEBUG: Block*\n");
    printf("    mod=%s\n", mod);
    printf("    container_size={x %f , y %f}\n", b->container_size.x,
           b->container_size.y);
    printf("    text_size={x %f , y %f}\n", b->text_size.x, b->text_size.y);
    printf("    gap=%f\n", b->gap);
    printf("    hover=%b\n", b->hover);
    printf("    roundness=%f\n", b->roundness);
    printf("    text=%s\n", b->text);
    printf("    image=%d\n", b->image != NULL);
    printf("    image_size={x %f , y %f}\n", b->image_size.x, b->image_size.y);
    printf("\\--------------------------------------------\n");
}

void prep_workspaces(Context* ctx, Block* blocks, int* counter)
{
    float max_side = 0.0f;

    for (int i = 0; i < ctx->s->workspaces_count; i++) {
        HyprWorkspace* ws = &ctx->s->workspaces[i];

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
        HyprWorkspace* ws = &ctx->s->workspaces[i];

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
    block.hover = ctx->c->window.hover;
    block.roundness = ctx->c->window.roundness;
    block.mod = MOD_WINDOW;

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

        if (hovered && b->hover) {
            left_alphas[i] += ANIM_SPEED * ctx->delta_time;
        } else {
            left_alphas[i] -= ANIM_SPEED * ctx->delta_time;
        }
        if (left_alphas[i] < 0) left_alphas[i] = 0;
        if (left_alphas[i] > 1) left_alphas[i] = 1;

        if (hovered && b->hover) {
            Color fg = ctx->c->theme.fg;
            fg.a = (unsigned char)(left_alphas[i] * 255);

            DrawRectangleRounded(crect, b->roundness, 3, fg);
            textcolor = ctx->c->theme.bg;
        }

        if (b->text != NULL) {
            DrawTextEx(ctx->c->font, b->text, text_pos, ctx->c->fontsize, 0,
                       textcolor);
        }

        left_offset += b->container_size.x;
    }
}
