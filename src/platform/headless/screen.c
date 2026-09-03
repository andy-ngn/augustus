#include "platform/screen.h"

#include "graphics/screen.h"
#include "platform/renderer.h"

#define HEADLESS_WIDTH 1280
#define HEADLESS_HEIGHT 800

int platform_screen_create(const char *title, int display_scale_percentage, int display_id)
{
    if (!platform_renderer_init(0)) {
        return 0;
    }
    // Camera and viewport code divides by the screen size, so a real resolution is required.
    screen_set_resolution(HEADLESS_WIDTH, HEADLESS_HEIGHT);
    return 1;
}

void platform_screen_destroy(void)
{
    platform_renderer_destroy();
}

int platform_screen_resize(int pixel_width, int pixel_height, int save)
{
    screen_set_resolution(pixel_width, pixel_height);
    return 1;
}

void platform_screen_move(int x, int y) {}
int platform_screen_get_scale(void) { return 100; }
void platform_screen_set_fullscreen(void) {}
void platform_screen_set_windowed(void) {}
void platform_screen_set_window_size(int logical_width, int logical_height) {}
void platform_screen_center_window(void) {}
void platform_screen_update_window_grab(void) {}
