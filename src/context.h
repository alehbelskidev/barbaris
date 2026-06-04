#ifndef CONTEXT_H
#define CONTEXT_H

#include "config.h"
#include "state.h"

typedef struct {
    State* s;
    Config* c;
} Context;

Context* ctx_init(Config* c, State* s);
void ctx_free(Context* ctx);

#endif  // !CONTEXT_H
