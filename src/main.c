#define GLFW_EXPOSE_NATIVE_WAYLAND
#define BAR_WIDTH 0

#include <stdio.h>

#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "config.h"
#include "hyprs.h"
#include "raylib.h"
#include "state.h"
#include "ui.h"

int main(void)
{
    int fd = connect_hypr_sock();
    init_state();
    load_config();
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TRANSPARENT);

    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);

    InitWindow(BAR_WIDTH, config->height, "barbaris");

    load_config_font();

    GLFWwindow* glfw_win = glfwGetCurrentContext();

    glfwWindowHint(GLFW_WAYLAND_USE_ZWLR, GLFW_WAYLAND_ZWLR_LAYER_TOP);
    glfwWaylandZwlrSetLayer(glfw_win, GLFW_WAYLAND_ZWLR_LAYER_TOP);
    glfwWaylandZwlrSetAnchor(glfw_win, GLFW_WAYLAND_ZWLR_ANCHOR_TOP |
                                           GLFW_WAYLAND_ZWLR_ANCHOR_LEFT |
                                           GLFW_WAYLAND_ZWLR_ANCHOR_RIGHT);
    glfwWaylandZwlrSetExclusiveZone(glfw_win, config->height);
    wl_surface_commit(glfwGetWaylandWindow(glfw_win));

    SetTargetFPS(60);

    for (int i = 0; i < config->modules.left_count; i += 1) {
        printf("[[modi=%d, mod=%d]]\n", i, config->modules.left[i]);
    }

    while (!WindowShouldClose()) {
        read_hypr_sock(fd);
        prep_ui();

        BeginDrawing();
        ClearBackground(config->theme.bg);
        draw_ui();
        EndDrawing();
    }

    free_state();
    free_config();
    close_hypr_sock(fd);
    CloseWindow();
    return 0;
}
