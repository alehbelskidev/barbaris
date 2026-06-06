#ifndef MOD_WIFI_H
#define MOD_WIFI_H

#include <stdlib.h>

#include "state.h"

typedef struct WifiContext WifiContext;

WifiContext *wifi_connect(const char *ifname);
void wifi_read_sock(WifiContext *ctx, State *s,
                    void (*state_update_wifi)(State *, const char *, int));
void wifi_close(WifiContext *ctx);

#endif  // !MOD_WIFI_H
