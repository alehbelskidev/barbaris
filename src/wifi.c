#include "wifi.h"

#include <linux/wireless.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

struct WifiContext {
    char if_name[IFNAMSIZ];
};

WifiContext *wifi_connect(const char *ifname)
{
    WifiContext *ctx = malloc(sizeof(WifiContext));
    if (!ctx) return NULL;

    strncpy(ctx->if_name, ifname, sizeof(ctx->if_name) - 1);
    ctx->if_name[sizeof(ctx->if_name) - 1] = '\0';

    return ctx;
}

void wifi_read_sock(WifiContext *ctx, State *s,
                    void (*state_update_wifi)(State *s, const char *essid,
                                              int signal_dbm))
{
    if (!ctx) return;

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        state_update_wifi(s, "No Link", -100);
        return;
    }

    struct iwreq iw_req = {0};
    strncpy(iw_req.ifr_name, ctx->if_name, IFNAMSIZ);

    char essid[IW_ESSID_MAX_SIZE + 1] = {0};
    int signal = -100;

    iw_req.u.essid.pointer = essid;
    iw_req.u.essid.length = IW_ESSID_MAX_SIZE;
    iw_req.u.essid.flags = 0;

    if (ioctl(sock, SIOCGIWESSID, &iw_req) >= 0) {
        if (strlen(essid) == 0) {
            strcpy(essid, "Disconnected");
        }
    } else {
        strcpy(essid, "No Link");
    }

    if (strcmp(essid, "Disconnected") != 0 && strcmp(essid, "No Link") != 0) {
        struct iw_statistics stats = {0};
        iw_req.u.data.pointer = &stats;
        iw_req.u.data.length = sizeof(struct iw_statistics);
        iw_req.u.data.flags = 1;

        if (ioctl(sock, SIOCGIWSTATS, &iw_req) >= 0) {
            signal = (signed char)stats.qual.level;
        }
    }

    close(sock);

    state_update_wifi(s, essid, signal);
}

void wifi_close(WifiContext *ctx)
{
    if (ctx) {
        free(ctx);
    }
}
