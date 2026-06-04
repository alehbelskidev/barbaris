#ifndef STATE_H
#define STATE_H
#define MAX_WORKSPACES 20

typedef struct {
    int id;
    char name[128];
} HyprWorkspace;

typedef struct {
    int workspaces_count;
    HyprWorkspace workspaces[MAX_WORKSPACES];
    char active_window[108];
} State;

void init_state();
void free_state();

extern State* state;

#endif  // !STATE_H
