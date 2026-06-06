#include "__debug.h"

#include <stdio.h>

const char *module_name(Module m)
{
    switch (m) {
        case MOD_WORKSPACES:
            return "workspaces";
        case MOD_WINDOW:
            return "window";
        case MOD_CLOCK:
            return "clock";
        case MOD_VOLUME:
            return "volume";
        case MOD_MIC:
            return "mic";
        case MOD_DISK:
            return "disk";
        case MOD_RAM:
            return "ram";
        case MOD_EMPTY:
            return "empty";
        default:
            return "unknown";
    }
}

void DEBUG_config(Config *c)
{
    printf("Config* {\n");
    printf("  height     = %d\n", c->height);
    printf("  fontsize   = %d\n", c->fontsize);
    printf("  fontpath   = %s\n", c->fontpath ? c->fontpath : "(null)");
    printf("  clock_fmt  = %s\n", c->clock_format);
    printf("  theme.bg   = #%02x%02x%02x\n", c->theme.bg.r, c->theme.bg.g,
           c->theme.bg.b);
    printf("  theme.fg   = #%02x%02x%02x\n", c->theme.fg.r, c->theme.fg.g,
           c->theme.fg.b);
    printf("  theme.acc  = #%02x%02x%02x\n", c->theme.accent.r,
           c->theme.accent.g, c->theme.accent.b);
    printf("  bar        = { gap=%d px=%d py=%d r=%.2f hover=%d }\n",
           c->bar.gap, c->bar.padding_x, c->bar.padding_y, c->bar.roundness,
           c->bar.hover);
    printf("  workspaces = { gap=%d px=%d py=%d r=%.2f hover=%d }\n",
           c->workspaces.gap, c->workspaces.padding_x, c->workspaces.padding_y,
           c->workspaces.roundness, c->workspaces.hover);
    printf("  window     = { gap=%d px=%d py=%d r=%.2f hover=%d }\n",
           c->window.gap, c->window.padding_x, c->window.padding_y,
           c->window.roundness, c->window.hover);
    printf("  clock      = { gap=%d px=%d py=%d r=%.2f hover=%d }\n",
           c->clock.gap, c->clock.padding_x, c->clock.padding_y,
           c->clock.roundness, c->clock.hover);
    printf("  modules.left[%d]   = ", c->modules.left_count);
    for (int i = 0; i < c->modules.left_count; i++)
        printf("%s ", module_name(c->modules.left[i]));
    printf("\n  modules.center[%d] = ", c->modules.center_count);
    for (int i = 0; i < c->modules.center_count; i++)
        printf("%s ", module_name(c->modules.center[i]));
    printf("\n  modules.right[%d]  = ", c->modules.right_count);
    for (int i = 0; i < c->modules.right_count; i++)
        printf("%s ", module_name(c->modules.right[i]));
    printf("\n}\n");
}

void DEBUG_state(State *s)
{
    printf("State* {\n");
    printf("  workspaces_count    = %d\n", s->workspaces_count);
    printf("  active_workspace_id = %d\n", s->active_workspace_id);
    printf("  active_window       = %s\n", s->active_window);
    printf("  time                = %s\n", s->time);
    printf("  is_dirty            = %d\n", s->is_dirty);
    printf("  workspaces          = ");
    for (int i = 0; i < s->workspaces_count; i++)
        printf("[%d] ", s->workspaces[i].id);
    printf("\n}\n");
}

void DEBUG_block(Block *b)
{
    printf("Block* {\n");
    printf("  mod            = %s\n", module_name(b->mod));
    printf("  workspace_id   = %d\n", b->workspace_id);
    printf("  container_size = (%.1f, %.1f)\n", b->container_size.x,
           b->container_size.y);
    printf("  text_size      = (%.1f, %.1f)\n", b->text_size.x, b->text_size.y);
    printf("  image_size     = (%.1f, %.1f)\n", b->image_size.x,
           b->image_size.y);
    printf("  gap            = %.2f\n", b->gap);
    printf("  roundness      = %.2f\n", b->roundness);
    printf("  hover          = %d\n", b->hover);
    printf("  text           = %s\n", b->text ? b->text : "(null)");
    printf("  image          = %s\n", b->image ? "(set)" : "(null)");
    printf("}\n");
}
