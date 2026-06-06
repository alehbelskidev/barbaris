#include "hyprs.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "ui.h"

int hypr_connect_sock()
{
    char path[256];
    snprintf(path, sizeof(path), "%s/hypr/%s/.socket2.sock",
             getenv("XDG_RUNTIME_DIR"), getenv("HYPRLAND_INSTANCE_SIGNATURE"));

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(fd);
        return -1;
    }

    fcntl(fd, F_SETFL, O_NONBLOCK);
    return fd;
}

void hypr_read_sock(int fd, State *s,
                    void (*state_update_active_window)(State *, char[108]),
                    void (*state_update_active_workspace)(State *, int),
                    void (*state_create_workspace)(State *, int),
                    void (*state_destroy_workspace)(State *, int))
{
    size_t bufsize = 4096;
    char buf[bufsize];

    int n = read(fd, buf, bufsize - 1);
    if (n > 0) {
        buf[n] = '\0';

        char *line = buf;
        char *end;

        while ((end = strchr(line, '\n')) != NULL) {
            *end = '\0';
            printf("[HYPR EVENLINE='%s']", line);

            char event_t[64], arg[108];

            int res_count = sscanf(line, "%63[^>]>>%107[^\n]", event_t, arg);

            if (res_count != 2) {
                line = end + 1;
                continue;
            }

            if (strcmp(event_t, "activewindow") == 0) {
                state_update_active_window(s, arg);
            } else if (strcmp(event_t, "workspace") == 0) {
                state_update_active_workspace(s, atoi(arg));
            } else if (strcmp(event_t, "createworkspace") == 0) {
                state_create_workspace(s, atoi(arg));
            } else if (strcmp(event_t, "destroyworkspace") == 0) {
                state_destroy_workspace(s, atoi(arg));
            }

            line = end + 1;
        }
    }
}

int hypr_request(const char *cmd, char *buf, size_t bufsize)
{
    char path[256];
    snprintf(path, sizeof(path), "%s/hypr/%s/.socket.sock",
             getenv("XDG_RUNTIME_DIR"), getenv("HYPRLAND_INSTANCE_SIGNATURE"));

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(fd);
        return -1;
    }

    write(fd, cmd, strlen(cmd));
    int n = read(fd, buf, bufsize - 1);
    buf[n] = '\0';

    close(fd);
    return n;
}

void hypr_dispatch(const char *cmd)
{
    char path[256];
    snprintf(path, sizeof(path), "%s/hypr/%s/.socket.sock",
             getenv("XDG_RUNTIME_DIR"), getenv("HYPRLAND_INSTANCE_SIGNATURE"));

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(fd);
        return;
    }

    write(fd, cmd, strlen(cmd));
    close(fd);
}

void hypr_close_sock(int fd)
{
    close(fd);
}
