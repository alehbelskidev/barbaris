#ifndef STATE_H
#define STATE_H
#define MAX_WORKSPACES 20

#include <stdbool.h>

#include "config.h"

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
    char time[128];
} State;

State *state_init(char *clock_format);
void state_update_active_window(State *s, char w[108]);
void state_update_active_workspace(State *s, int id);
void state_create_workspace(State *s, int id);
void state_destroy_workspace(State *s, int id);
void state_update_time(State *s, char *clock_format);
void state_free(State *s);

#endif  // !STATE_H
