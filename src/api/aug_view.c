#include "api/aug_view.h"

#include "building/construction.h"
#include "city/view.h"
#include "game/file.h"
#include "game/state.h"
#include "graphics/window.h"
#include "platform/screen.h"
#include "window/city.h"

static int external_tick_enabled;
static int observe_locked;

int aug_view_external_tick_enabled(void)
{
    return external_tick_enabled;
}

void aug_view_attach(void)
{
    external_tick_enabled = 1;
    game_file_set_disk_saves_enabled(0);
}

void aug_view_set_external_tick(int enabled)
{
    external_tick_enabled = enabled;
}

void aug_view_show_city(void)
{
    window_city_show();
}

void aug_view_set_camera(int x, int y)
{
    city_view_set_camera(x, y);
}

int aug_view_camera_x(void)
{
    int x, y;
    city_view_get_camera(&x, &y);
    return x;
}

int aug_view_camera_y(void)
{
    int x, y;
    city_view_get_camera(&x, &y);
    return y;
}

void aug_view_go_to_grid_offset(int grid_offset)
{
    city_view_go_to_grid_offset(grid_offset);
}

void aug_view_set_paused(int paused)
{
    if (paused) {
        game_state_pause();
    } else {
        game_state_unpause();
    }
}

void aug_view_set_observe_lock(int locked)
{
    observe_locked = locked ? 1 : 0;
    if (observe_locked) {
        building_construction_cancel();
        building_construction_clear_type();
    }
}

int aug_view_observe_locked(void)
{
    return observe_locked;
}

int aug_view_window_allowed(int window_id)
{
    switch (window_id) {
        case WINDOW_CITY:
        case WINDOW_OVERLAY_MENU:
        case WINDOW_SLIDING_SIDEBAR:
        case WINDOW_BUILDING_INFO:
        case WINDOW_MESSAGE_DIALOG:
        case WINDOW_MESSAGE_LIST:
        case WINDOW_PLAIN_MESSAGE_DIALOG:
            return 1;
        default:
            return 0;
    }
}

void aug_view_set_sidebar_collapsed(int collapsed)
{
    if (city_view_is_sidebar_collapsed() != (collapsed ? 1 : 0)) {
        city_view_toggle_sidebar();
        window_invalidate();
    }
}

int aug_view_sidebar_collapsed(void)
{
    return city_view_is_sidebar_collapsed();
}

void aug_view_resize(int width, int height)
{
    if (width < 640) {
        width = 640;
    }
    if (height < 480) {
        height = 480;
    }
    platform_screen_set_window_size(width, height);
}

int aug_view_screen_to_grid_offset(int x, int y)
{
    view_tile tile;
    if (!city_view_pixels_to_view_tile(x, y, &tile)) {
        return 0;
    }
    return city_view_tile_to_grid_offset(&tile);
}

void aug_view_set_scale(int percent)
{
    city_view_set_scale(percent);
    window_invalidate();
}

int aug_view_scale(void)
{
    return city_view_get_scale();
}
