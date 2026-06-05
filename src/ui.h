#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include "config.h"
#include "context.h"
#include "raylib.h"

typedef struct {
    int workspace_id;
    Module mod;

    Vector2 container_size;
    Vector2 text_size;
    float gap;

    bool hover;

    float roundness;

    char *text;

    Texture *image;
    Vector2 image_size;
} Block;

void ui_prep(Context *ctx);
void ui_draw(Context *ctx);

#endif  // !UI_H
