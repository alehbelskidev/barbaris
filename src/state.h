#ifndef STATE_H
#define STATE_H
#define MAX_WORKSPACES 20

#include <stdbool.h>

typedef struct {
    int id;
    char name[128];
} HyprWorkspace;

typedef struct {
    int workspaces_count;
    HyprWorkspace workspaces[MAX_WORKSPACES];
    char active_window[108];

    bool is_dirty;
} State;

State* state_init();
void state_update_active_window(State* s, char w[108]);
void state_free(State* s);

#endif  // !STATE_H
