#include "api/aug_view.h"

#include "city/view.h"
#include "game/file.h"
#include "game/state.h"
#include "window/city.h"

static int external_tick_enabled;

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
