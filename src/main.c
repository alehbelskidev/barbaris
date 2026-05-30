#define GLFW_EXPOSE_NATIVE_WAYLAND
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "config.h"
#include "raylib.h"

#define BAR_WIDTH 0

int main(void)
{
    Config* cfg = load_config();
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TRANSPARENT);

    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);

    InitWindow(BAR_WIDTH, cfg->height, "barbaris");

    load_config_font(cfg);

    GLFWwindow* glfw_win = glfwGetCurrentContext();

    glfwWindowHint(GLFW_WAYLAND_USE_ZWLR, GLFW_WAYLAND_ZWLR_LAYER_TOP);
    glfwWaylandZwlrSetLayer(glfw_win, GLFW_WAYLAND_ZWLR_LAYER_TOP);
    glfwWaylandZwlrSetAnchor(glfw_win, GLFW_WAYLAND_ZWLR_ANCHOR_TOP |
                                           GLFW_WAYLAND_ZWLR_ANCHOR_LEFT |
                                           GLFW_WAYLAND_ZWLR_ANCHOR_RIGHT);
    glfwWaylandZwlrSetExclusiveZone(glfw_win, cfg->height);
    wl_surface_commit(glfwGetWaylandWindow(glfw_win));

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(cfg->theme.bg);
        DrawTextEx(cfg->font, "Barbaris", (Vector2){0, 0}, cfg->fontsize, 0,
                   cfg->theme.fg);
        EndDrawing();
    }

    free_config(cfg);
    CloseWindow();
    return 0;
}
