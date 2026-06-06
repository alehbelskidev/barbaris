#ifndef __DEBUG_H
#define __DEBUG_H

#include "config.h"
#include "state.h"
#include "ui.h"

const char *module_name(Module m);

void DEBUG_config(Config *c);

void DEBUG_state(State *s);

void DEBUG_block(Block *b);

#endif  // !__DEBUG_H
