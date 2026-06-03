#ifndef UI_H
#define UI_H

#include "config.h"
#include "raylib.h"
#include "state.h"

typedef struct {
    Vector2 offset;
    Vector2 size;
    Vector2 padding;

    char* text;

    Texture* image;
    Vector2 image_size;
} Block;

void prep_ui();
void free_ui();
void draw_ui();

#endif  // !UI_H
