#include "context.h"

#include <stdlib.h>

Context *ctx_init(Config *c, State *s)
{
    Context *ctx = malloc(sizeof(Context));

    if (ctx != NULL) {
        ctx->c = c;
        ctx->s = s;
    }

    return ctx;
}

void ctx_free(Context *ctx)
{
    if (ctx == NULL) return;

    free(ctx);
}
