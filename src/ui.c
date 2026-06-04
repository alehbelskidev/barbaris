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

bool is_dirty = true;

void prep_workspaces(Block* blocks, int* counter)
{
    for (int i = 0; i < state->workspaces_count; i++) {
        HyprWorkspace* ws = &state->workspaces[i];

        float left_gap = 0;
        if (*counter > 0) left_gap = config->workspaces.gap;

        Vector2 text_size =
            MeasureTextEx(config->font, ws->name, config->fontsize, 0);

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

void prep_window(Block* blocks, int* counter)
{
    Block block = {0};

    float left_gap = 0;
    printf("[window.gap=%d]\n", config->window.gap);
    if (*counter > 0) left_gap = config->window.gap;

    Vector2 text_size =
        MeasureTextEx(config->font, state->active_window, config->fontsize, 0);

    block.padding.x = 4;
    block.padding.y = 0;

    block.size.x = text_size.x + block.padding.x * 2;
    block.size.y = text_size.y + block.padding.y * 2;

    block.offset.x = left_gap;
    block.offset.y = 0;

    block.text = state->active_window;

    blocks[*counter] = block;
    (*counter)++;
}

void prep_module(Module* m, int count, Block* blocks, int* counter)
{
    for (int i = 0; i < count; i++) {
        if (m[i] == MOD_WORKSPACES) {
            prep_workspaces(blocks, counter);
        }
        if (m[i] == MOD_WINDOW) {
            prep_window(blocks, counter);
        }
    }
}

void prep_ui()
{
    if (is_dirty) {
        left_count = 0;
        center_count = 0;
        right_count = 0;
        prep_module(config->modules.left, config->modules.left_count, left,
                    &left_count);

        is_dirty = false;
    }
}

void draw_ui()
{
    float left_offset = 8;
    for (int i = 0; i < left_count; i++) {
        Block* b = &left[i];

        if (b->text != NULL) {
            left_offset += b->offset.x;

            Vector2 pos = {left_offset, config->height / 2 - b->size.y / 2};

            DrawTextEx(config->font, b->text, pos, config->fontsize, 0,
                       config->theme.fg);

            left_offset += b->size.x;
        }
    }
}
