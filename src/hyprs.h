#ifndef MOD_WINDOW_H
#define MOD_WINDOW_H

#include <stdlib.h>

#include "state.h"

int connect_hypr_sock();
int hypr_request(const char* cmd, char* buf, size_t bufsize);
void read_hypr_sock(int fd);
void close_hypr_sock(int fd);

#endif  // !MOD_WINDOW_H
