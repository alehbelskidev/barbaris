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
    int active_workspace_id;
    HyprWorkspace workspaces[MAX_WORKSPACES];
    char active_window[108];

    bool is_dirty;
} State;

void DEBUG_state(State* s);

State* state_init();
void state_update_active_window(State* s, char w[108]);
void state_update_active_workspace(State* s, int id);
void state_free(State* s);

#endif  // !STATE_H
