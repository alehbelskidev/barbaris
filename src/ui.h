#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include "context.h"
#include "raylib.h"

typedef struct {
    Vector2 offset;
    Vector2 size;
    Vector2 padding;

    char* text;

    Texture* image;
    Vector2 image_size;
} Block;

void ui_prep(Context* ctx);
void ui_draw(Context* ctx);

#endif  // !UI_H
