#include "hyprs.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "ui.h"

int connect_hypr_sock()
{
    char path[256];
    snprintf(path, sizeof(path), "%s/hypr/%s/.socket2.sock",
             getenv("XDG_RUNTIME_DIR"), getenv("HYPRLAND_INSTANCE_SIGNATURE"));

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(fd);
        return -1;
    }

    fcntl(fd, F_SETFL, O_NONBLOCK);
    return fd;
}

void read_hypr_sock(int fd)
{
    size_t bufsize = 1024;
    char buf[bufsize];
    int n = read(fd, buf, bufsize - 1);
    if (n > 0) {
        buf[n] = '\0';
        printf("HYPR buf = %s\n", buf);

        // example
        // activewindow>>Alacritty,nvim . ~/c/barbaris
        char event_t[64], arg[108];
        int res_count = sscanf(buf, "%[^>]>>%[^\n]", event_t, arg);
        if (res_count != 2) {
            printf("Hypr event parse error! Got args (required 2): %d\n",
                   res_count);
        }

        if (strcmp(event_t, "activewindow") == 0) {
            if (strcmp(state->active_window, arg) != 0) {
                memcpy(state->active_window, arg,
                       sizeof(state->active_window) - 1);
                state->active_window[sizeof(state->active_window) - 1] = '\0';
                is_dirty = true;
            }
        }
    }
}

int hypr_request(const char* cmd, char* buf, size_t bufsize)
{
    char path[256];
    snprintf(path, sizeof(path), "%s/hypr/%s/.socket.sock",
             getenv("XDG_RUNTIME_DIR"), getenv("HYPRLAND_INSTANCE_SIGNATURE"));

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    memcpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    addr.sun_path[sizeof(addr.sun_path) - 1] = '\0';

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(fd);
        return -1;
    }

    write(fd, cmd, strlen(cmd));
    int n = read(fd, buf, bufsize - 1);
    buf[n] = '\0';

    close(fd);
    return n;
}

void close_hypr_sock(int fd)
{
    close(fd);
}
