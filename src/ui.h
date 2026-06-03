#ifndef UI_H
#define UI_H

#include "config.h"
#include "raylib.h"
#include "state.h"

typedef struct {
    float width;
    float height;
} Size;

typedef struct {
    Vector2 offset;
    Size size;

    char* text;

    Texture* image;
    Size image_size;
} Block;

void prep_ui();
void free_ui();
void draw_ui();

#endif  // !UI_H
