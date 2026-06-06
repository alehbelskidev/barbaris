# barbaris - hyprland bar

Wayland status bar for Hyprland. Built with Raylib + C, configured in Toml.

## Dependencies

```
gcc cmake make wayland wayland-protocols xkbcommon
```

On Arch:
```bash
pacman -S gcc cmake make wayland wayland-protocols libxkbcommon
[aur] -S tomlc17
```

## Build

```bash
make prepare      # clone vendors (once)
make              # build everything
```

Binary lands in `bin/barbaris`.

## Vendor patches

Patch Raylib manually after `make prepare`, before first build.

`vendor/raylib/src/platforms/rcore_desktop_glfw.c`

Find the two `glfwCreateWindow` calls.
Insert before **each** of them:

```c
// barbaris: layer shell hint
glfwWindowHint(GLFW_WAYLAND_USE_ZWLR, GLFW_WAYLAND_ZWLR_LAYER_TOP);
```

There are exactly two `glfwCreateWindow` calls in that file __patch both__.

## Config

`config.toml` in the same directory as the binary, or `~/.config/barbaris/config.toml`.


## ps

Nothing is final here! Not sure about:
- Manual patches vs fork or smth else.
- Toml as config language.

```toml
height = 28

[bar]
padding = {x = 12, y = 4}

[theme]
bg = "#1e1e2e"
fg = "#cdd6f4"
accent = "#89b4fa"

[font]
family = "Iosevka Nerd Font"
size = 20

[modules]
left = [ "workspaces", "window"  ]
center = [ "clock", ]
right = [ "volume", "disk" ]

[workspaces]
gap = 4
padding = {x = 4, y = 4}
roundness = 0.2

[window]
gap = 8
padding = {x = 4, y = 4}
roundness = 0.2

[clock]
format = " %H:%M:%S, %d.%m.%Y"
gap = 8
padding = {x = 4, y = 4}
roundness = 0.2
```
