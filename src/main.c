#define GLFW_EXPOSE_NATIVE_WAYLAND
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "raylib.h"

#define BAR_HEIGHT 32
#define BAR_WIDTH 0  // 0 = растянуть на весь монитор

int main(void)
{
    // до InitWindow — window hints
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TRANSPARENT);

    // включаем layer shell
    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);

    InitWindow(BAR_WIDTH, BAR_HEIGHT, "barbaris");

    // получаем нативное GLFW окно из raylib
    GLFWwindow* glfw_win = glfwGetCurrentContext();

    // настраиваем layer shell
    glfwWindowHint(GLFW_WAYLAND_USE_ZWLR, GLFW_WAYLAND_ZWLR_LAYER_TOP);
    glfwWaylandZwlrSetLayer(glfw_win, GLFW_WAYLAND_ZWLR_LAYER_TOP);
    glfwWaylandZwlrSetAnchor(glfw_win, GLFW_WAYLAND_ZWLR_ANCHOR_TOP |
                                           GLFW_WAYLAND_ZWLR_ANCHOR_LEFT |
                                           GLFW_WAYLAND_ZWLR_ANCHOR_RIGHT);
    glfwWaylandZwlrSetExclusiveZone(glfw_win, BAR_HEIGHT);
    wl_surface_commit(glfwGetWaylandWindow(glfw_win));

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground((Color){0x1e, 0x1e, 0x2e, 0xff});
        DrawText("barbaris", 10, 8, 16, (Color){0xcd, 0xd6, 0xf4, 0xff});
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
