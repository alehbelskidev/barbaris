#ifndef MOD_WINDOW_H
#define MOD_WINDOW_H

#include <stdlib.h>

#include "state.h"

int hypr_connect_sock();
int hypr_request(const char *cmd, char *buf, size_t bufsize);
void hypr_read_sock(int fd, State *s,
                    void (*state_update_active_window)(State *, char[108]),
                    void (*state_update_active_workspace)(State *, int),
                    void (*state_create_workspace)(State *, int),
                    void (*state_destory_workspace)(State *, int));
void hypr_dispatch(const char *cmd);
void hypr_close_sock(int fd);

#endif  // !MOD_WINDOW_H
