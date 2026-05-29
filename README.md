# barbaris

Wayland status bar for Hyprland. Built with Raylib + C, configured in Lua.

## Dependencies

```
gcc cmake make wayland wayland-protocols lua5.4 xkbcommon
```

On Arch:
```bash
pacman -S gcc cmake make wayland wayland-protocols lua54 libxkbcommon
```

## Build

```bash
make prepare      # clone vendors (once)
make              # build everything
```

Binary lands in `bin/barbaris`.

---

## Vendor patches

After `make prepare`, before first build — patch Raylib manually.

### vendor/raylib/src/platforms/rcore_desktop_glfw.c

Find the two `glfwCreateWindow` calls (one for fullscreen, one for windowed).
Insert before **each** of them:

```c
// barbaris: layer shell hint
glfwWindowHint(GLFW_WAYLAND_USE_ZWLR, GLFW_WAYLAND_ZWLR_LAYER_TOP);
```

There are exactly two `glfwCreateWindow` calls in that file — patch both.

---

## Config

`config.lua` in the same directory as the binary, or `~/.config/barbaris/config.lua`.
