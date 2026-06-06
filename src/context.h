#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdbool.h>

#include "config.h"
#include "state.h"

typedef struct {
    State *s;
    Config *c;
    Vector2 mouse_pos;
    Vector2 mouse_delta;
    float delta_time;
    bool left_clicked;
} Context;

Context *ctx_init(Config *c, State *s);
void ctx_free(Context *ctx);

#endif  // !CONTEXT_H
