#include "platform/headless/headless.h"

#include "core/file.h"
#include "core/log.h"
#include "game/system.h"
#include "platform/platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct {
    char pref_path[FILE_NAME_MAX];
    char assets_base_path[FILE_NAME_MAX];
    uint64_t ticks;
    int exit_requested;
} data = { "./", "./", 0, 0 };

void headless_set_pref_path(const char *path)
{
    snprintf(data.pref_path, FILE_NAME_MAX, "%s", path);
}

void headless_set_assets_base_path(const char *path)
{
    snprintf(data.assets_base_path, FILE_NAME_MAX, "%s", path);
}

void headless_set_ticks(uint64_t millis)
{
    data.ticks = millis;
}

uint64_t headless_get_ticks(void)
{
    return data.ticks;
}

int headless_exit_requested(void)
{
    return data.exit_requested;
}

// --- game/system.h ---

const char *system_architecture(void)
{
    return "headless";
}

const char *system_OS(void)
{
#ifdef __EMSCRIPTEN__
    return "Emscripten (headless)";
#else
    return "Native (headless)";
#endif
}

uint64_t system_get_ticks(void)
{
    return data.ticks;
}

void system_resize(int width, int height)
{}

void system_get_max_resolution(int *width, int *height)
{
    *width = 1280;
    *height = 800;
}

void system_center(void)
{}

int system_is_fullscreen_only(void)
{
    return 0;
}

void system_set_fullscreen(int fullscreen)
{}

void system_change_window_title(const char *title)
{}

int system_scale_display(int scale_percentage)
{
    return 100;
}

int system_can_scale_display(int *min_scale, int *max_scale)
{
    *min_scale = 100;
    *max_scale = 100;
    return 0;
}

void system_update_window_grab(void)
{}

void system_show_error_message_box(const char *title, const char *message)
{
    log_error(title, message, 0);
}

int system_supports_select_folder_dialog(void)
{
    return 0;
}

const char *system_show_select_folder_dialog(const char *title, const char *default_path)
{
    return 0;
}

void system_exit(void)
{
    data.exit_requested = 1;
}

// --- platform/platform.h ---

int platform_sdl_version_at_least(int major, int minor, int patch)
{
    return 1;
}

const char *platform_get_logging_path(void)
{
    return data.pref_path;
}

const char *platform_get_pref_path(void)
{
    return data.pref_path;
}

const char *platform_get_base_path(void)
{
    return data.assets_base_path;
}

void exit_with_status(int status)
{
    data.exit_requested = 1;
#ifndef __EMSCRIPTEN__
    exit(status);
#endif
}
