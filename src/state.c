#include "state.h"

#include <cjson/cJSON.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "__debug.h"
#include "hyprs.h"

// Str will be cut to 256b
// TODO: do I really need to resolve that?
int parse_active_window(State *s)
{
    int status = 0;
    char win[1024];
    hypr_request("j/activewindow", win, sizeof(win));

    cJSON *win_json = cJSON_Parse(win);
    if (win_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "ERROR: parse_active_window: %s\n", error_ptr);
        }
        status = 1;
        goto end;
    }

    const cJSON *winclass = cJSON_GetObjectItem(win_json, "class");
    const cJSON *wintitle = cJSON_GetObjectItem(win_json, "title");

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

int parse_wokspaces(State *s)
{
    int status = 0;
    char workspaces[2048];
    hypr_request("j/workspaces", workspaces, sizeof(workspaces));

    cJSON *ws_json = cJSON_Parse(workspaces);
    if (ws_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "ERROR: parse_workspace: %s\n", error_ptr);
        }
        status = 1;
        goto end;
    }

    const cJSON *ws;
    int i = 0;
    cJSON_ArrayForEach(ws, ws_json)
    {
        cJSON *id = cJSON_GetObjectItem(ws, "id");
        cJSON *name = cJSON_GetObjectItem(ws, "name");

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

int parse_active_workspace(State *s)
{
    int status = 0;
    char ws[2048];
    hypr_request("j/activeworkspace", ws, sizeof(ws));

    cJSON *ws_json = cJSON_Parse(ws);
    if (ws_json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "ERROR: parse_active_workspace: %s\n", error_ptr);
        }
        status = 1;
        goto end;
    }

    cJSON *id = cJSON_GetObjectItem(ws_json, "id");
    if (cJSON_IsNumber(id)) {
        s->active_workspace_id = id->valueint;
    }

end:
    cJSON_Delete(ws_json);
    return status;
}

void state_update_active_window(State *s, char w[108])
{
    if (strcmp(s->active_window, w) != 0) {
        memcpy(s->active_window, w, sizeof(s->active_window) - 1);
        s->active_window[sizeof(s->active_window) - 1] = '\0';
        s->is_dirty = true;
    }
}

void state_update_active_workspace(State *s, int id)
{
    if (s->active_workspace_id != id) {
        s->active_workspace_id = id;
        s->is_dirty = true;
    }
}

void state_create_workspace(State *s, int id)
{
    char name[128];
    snprintf(name, sizeof(name), "%d", id);
    HyprWorkspace new_ws = {id, ""};
    strncpy(new_ws.name, name, sizeof(new_ws.name) - 1);

    int i = s->workspaces_count - 1;

    while (i >= 0 && s->workspaces[i].id > id) {
        s->workspaces[i + 1] = s->workspaces[i];
        i--;
    }

    s->workspaces[i + 1] = new_ws;

    s->workspaces_count++;
    s->is_dirty = true;
}

void state_destroy_workspace(State *s, int id)
{
    int write_idx = 0;

    for (int i = 0; i < s->workspaces_count; i++) {
        if (s->workspaces[i].id != id) {
            s->workspaces[write_idx] = s->workspaces[i];
            write_idx++;
        }
    }

    s->workspaces_count = write_idx;
    s->is_dirty = true;
}

void state_update_time(State *s, char *clock_format)
{
    time_t t = time(NULL);
    struct tm *lt = localtime(&t);

    strftime(s->time, sizeof(s->time), clock_format, lt);
}

State *state_init(char *clock_format)
{
    State *s = malloc(sizeof(State));

    if (s != NULL) {
        s->workspaces_count = 0;
        // FYI: Should be true for initial calc
        s->is_dirty = true;

        int status = 0;
        status = parse_active_window(s);
        status = parse_wokspaces(s);
        status = parse_active_workspace(s);

        if (status > 0) {
            printf("ERROR: initializing State\n");
            DEBUG_state(s);
        }

        state_update_time(s, clock_format);
    }

    return s;
}

void state_free(State *s)
{
    if (s != NULL) {
        free(s);
    }
}
