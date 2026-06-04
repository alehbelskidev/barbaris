#include "state.h"

#include <cjson/cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hyprs.h"

/*
❯ socat -
UNIX-CONNECT:$XDG_RUNTIME_DIR/hypr/$HYPRLAND_INSTANCE_SIGNATURE/.socket2.sock
activewindow>>Alacritty,nvim . ~/c/barbaris
activewindowv2>>55bb32240070
activewindow>>Alacritty,socat - UNIX-CONNECT ~
activewindowv2>>55bb3367c3f0
activewindow>>Alacritty,nvim . ~/c/barbaris
activewindowv2>>55bb32240070
activewindow>>Alacritty,socat - UNIX-CONNECT ~
activewindowv2>>55bb3367c3f0
openlayer>>wofi
closelayer>>wofi
activewindow>>Alacritty,socat - UNIX-CONNECT ~
activewindowv2>>55bb3367c3f0
windowtitle>>55bb33691090
windowtitlev2>>55bb33691090,Thorium
urgent>>55bb33691090
openwindow>>55bb33691090,2,Thorium-browser,Thorium
activewindow>>Thorium-browser,Thorium
activewindowv2>>55bb33691090
activewindow>>Alacritty,nvim . ~/c/barbaris
activewindowv2>>55bb32240070
activewindow>>Thorium-browser,Thorium
activewindowv2>>55bb33691090
activewindow>>Alacritty,nvim . ~/c/barbaris
activewindowv2>>55bb32240070
activewindow>>Thorium-browser,Thorium
activewindowv2>>55bb33691090
activewindow>>Alacritty,socat - UNIX-CONNECT ~
activewindowv2>>55bb3367c3f0
activewindow>>Alacritty,nvim . ~/c/barbaris
activewindowv2>>55bb32240070
activewindow>>Alacritty,socat - UNIX-CONNECT ~
activewindowv2>>55bb3367c3f0
activewindow>>Thorium-browser,Thorium
activewindowv2>>55bb33691090
activewindow>>Alacritty,nvim . ~/c/barbaris
activewindowv2>>55bb32240070
activewindow>>Thorium-browser,Thorium
activewindowv2>>55bb33691090
activewindow>>Alacritty,socat - UNIX-CONNECT ~
activewindowv2>>55bb3367c3f0
activewindow>>Thorium-browser,Thorium
activewindowv2>>55bb33691090
windowtitle>>55bb33696300
windowtitlev2>>55bb33696300,thorium-browser
closewindow>>55bb33691090
activewindow>>Alacritty,socat - UNIX-CONNECT ~
activewindowv2>>55bb3367c3f0

~ via  v22.22.0
❯ echo -n "j/workspaces" | socat -
UNIX-CONNECT:$XDG_RUNTIME_DIR/hypr/$HYPRLAND_INSTANCE_SIGNATURE/.socket.sock
[{
    "id": 1,
    "name": "1",
    "monitor": "DP-3",
    "monitorID": 0,
    "windows": 1,
    "hasfullscreen": false,
    "lastwindow": "0x55bb322f2320",
    "lastwindowtitle": "Интеграция приложения с Hyprland - Claude — Zen
Browser", "ispersistent": false, "tiledLayout": "dwindle"
},{
    "id": 2,
    "name": "2",
    "monitor": "DP-3",
    "monitorID": 0,
    "windows": 2,
    "hasfullscreen": false,
    "lastwindow": "0x55bb3367c3f0",
    "lastwindowtitle": "echo -n \"j/workspace ~",
    "ispersistent": false,
    "tiledLayout": "dwindle"
},{
    "id": 3,
    "name": "3",
    "monitor": "DP-3",
    "monitorID": 0,
    "windows": 1,
    "hasfullscreen": false,
    "lastwindow": "0x55bb3238f320",
    "lastwindowtitle": "Telegram (1)",
    "ispersistent": false,
    "tiledLayout": "dwindle"
},{
    "id": 4,
    "name": "4",
    "monitor": "DP-3",
    "monitorID": 0,
    "windows": 1,
    "hasfullscreen": false,
    "lastwindow": "0x55bb3362a2d0",
    "lastwindowtitle": "Steam",
    "ispersistent": false,
    "tiledLayout": "dwindle"
},{
    "id": 5,
    "name": "5",
    "monitor": "DP-3",
    "monitorID": 0,
    "windows": 2,
    "hasfullscreen": false,
    "lastwindow": "0x55bb33726d80",
    "lastwindowtitle": "~",
    "ispersistent": false,
    "tiledLayout": "dwindle"
},{
    "id": 6,
    "name": "6",
    "monitor": "DP-3",
    "monitorID": 0,
    "windows": 2,
    "hasfullscreen": false,
    "lastwindow": "0x55bb33776740",
    "lastwindowtitle": "~",
    "ispersistent": false,
    "tiledLayout": "dwindle"
}]⏎

~ via  v22.22.0
❯ echo -n "j/activewindow" | socat -
UNIX-CONNECT:$XDG_RUNTIME_DIR/hypr/$HYPRLAND_INSTANCE_SIGNATURE/.socket.sock
{
    "address": "0x55df0602e220",
    "mapped": true,
    "hidden": false,
    "visible": true,
    "acceptsInput": true,
    "at": [1288, 14],
    "size": [1258, 1380],
    "workspace": {
        "id": 2,
        "name": "2"
    },
    "floating": false,
    "monitor": 0,
    "class": "Alacritty",
    "title": "echo -n \"j/activewin ~",
    "initialClass": "Alacritty",
    "initialTitle": "Alacritty",
    "pid": 7133,
    "xwayland": false,
    "pinned": false,
    "fullscreen": 0,
    "fullscreenClient": 0,
    "overFullscreen": false,
    "grouped": [],
    "tags": [],
    "swallowing": "0x0",
    "focusHistoryID": 0,
    "inhibitingIdle": false,
    "xdgTag": "",
    "xdgDescription": "",
    "contentType": "none",
    "stableId": "1800003a"
}⏎
*/

// Str will be cut to 256b
// TODO: do I really need to resolve that?
int parse_active_window(State* s)
{
    int status = 0;
    char win[1024];
    hypr_request("j/activewindow", win, sizeof(win));

    cJSON* win_json = cJSON_Parse(win);
    if (win_json == NULL) {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 1;
        goto end;
    }

    const cJSON* winclass = cJSON_GetObjectItem(win_json, "class");
    const cJSON* wintitle = cJSON_GetObjectItem(win_json, "title");

    if (cJSON_IsString(winclass) && (winclass->valuestring != NULL) &&
        cJSON_IsString(wintitle) && (wintitle->valuestring != NULL))
    {
        sprintf(s->active_window, "%s - %s", winclass->valuestring,
                wintitle->valuestring);
    }

end:
    cJSON_Delete(win_json);
    return status;
}

int parse_wokspaces(State* s)
{
    int status = 0;
    char workspaces[2048];
    hypr_request("j/workspaces", workspaces, sizeof(workspaces));

    cJSON* ws_json = cJSON_Parse(workspaces);
    if (ws_json == NULL) {
        const char* error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 1;
        goto end;
    }

    const cJSON* ws;
    int i = 0;
    cJSON_ArrayForEach(ws, ws_json)
    {
        cJSON* id = cJSON_GetObjectItem(ws, "id");
        cJSON* name = cJSON_GetObjectItem(ws, "name");

        if (cJSON_IsString(name) && (name->valuestring != NULL) &&
            cJSON_IsNumber(id))
        {
            s->workspaces[i].id = id->valueint;
            strncpy(s->workspaces[i].name, name->valuestring, 127);
            s->workspaces_count += 1;
        }

        i += 1;
    }

end:
    cJSON_Delete(ws_json);
    return status;
}

void state_update_active_window(State* s, char w[108])
{
    if (strcmp(s->active_window, w) != 0) {
        memcpy(s->active_window, w, sizeof(s->active_window) - 1);
        s->active_window[sizeof(s->active_window) - 1] = '\0';
        s->is_dirty = true;
    }
}

State* state_init()
{
    State* s = malloc(sizeof(State));

    if (s != NULL) {
        s->workspaces_count = 0;
        // FYI: Should be true for initial calc
        s->is_dirty = true;
        parse_active_window(s);
        parse_wokspaces(s);
    }

    return s;
}

void state_free(State* s)
{
    if (s != NULL) {
        free(s);
    }
}
