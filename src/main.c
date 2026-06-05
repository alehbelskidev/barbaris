#define GLFW_EXPOSE_NATIVE_WAYLAND
#define BAR_WIDTH 0

#include <stdio.h>

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "config.h"
#include "context.h"
#include "hyprs.h"
#include "raylib.h"
#include "state.h"
#include "ui.h"

int main(void)
{
    int fd = hypr_connect_sock();
    State *s = state_init();
    Config *c = config_load();
    Context *ctx = ctx_init(c, s);

    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TRANSPARENT);

    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);

    InitWindow(BAR_WIDTH, c->height, "barbaris");
    config_load_font(c);

    GLFWwindow *glfw_win = glfwGetCurrentContext();

    glfwWindowHint(GLFW_WAYLAND_USE_ZWLR, GLFW_WAYLAND_ZWLR_LAYER_TOP);
    glfwWaylandZwlrSetLayer(glfw_win, GLFW_WAYLAND_ZWLR_LAYER_TOP);
    glfwWaylandZwlrSetAnchor(glfw_win, GLFW_WAYLAND_ZWLR_ANCHOR_TOP |
                                           GLFW_WAYLAND_ZWLR_ANCHOR_LEFT |
                                           GLFW_WAYLAND_ZWLR_ANCHOR_RIGHT);
    glfwWaylandZwlrSetExclusiveZone(glfw_win, c->height);
    wl_surface_commit(glfwGetWaylandWindow(glfw_win));

    SetTargetFPS(60);
    ui_prep(ctx);

    while (!WindowShouldClose()) {
        ui_prep(ctx);
        ctx->mouse_pos = GetMousePosition();
        ctx->mouse_delta = GetMouseDelta();
        ctx->delta_time = GetFrameTime();
        hypr_read_sock(fd, ctx->s, &state_update_active_window,
                       &state_update_active_workspace);

        BeginDrawing();
        ClearBackground(c->theme.bg);
        ui_draw(ctx);
        EndDrawing();
    }

    state_free(s);
    config_free(c);
    ctx_free(ctx);
    hypr_close_sock(fd);
    CloseWindow();
    return 0;
}
