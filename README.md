# barbaris - hyprland bar

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

`config.lua` in the same directory as the binary, or `~/.config/barbaris/config.lua`.


## ps

Nothing is final here! Not sure about:
- Manual patches vs fork or smth else.
- Lua as config language.

```lua
return {
	height = 40,

	theme = {
		bg = "#1e1e2e",
		fg = "#cdd6f4",
		accent = "#89b4fa",
	},

	font = {
		family = "Iosevka Nerd Font",
		size = 20,
		style = "Regular",
	},

	modules = {
		left = { "workspaces", "window" },
		center = { "clock" },
		right = { "volume", "disk" },
	},

	workspaces = {
		gap = 8,
		padding_x = 8,
		padding_y = 8;
	},

	window = {
		gap = 16,
		padding_x = 4,
		padding_y = 4;
	},
}
```
