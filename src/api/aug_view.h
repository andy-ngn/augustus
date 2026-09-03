#ifndef API_AUG_VIEW_H
#define API_AUG_VIEW_H

/**
 * Pantheon viewer API: functions that only exist in the SDL (viewer) build. They let the
 * world layer take over the clock and the window after Augustus' own main() has started.
 */

#include "api/aug_api.h"

/** Internal: 1 while the world layer drives the clock and game_run() must not tick. */
int aug_view_external_tick_enabled(void);

/**
 * Take control after callMain(): the simulation only advances through aug_tick(), autosaves
 * and mission saves are disabled.
 */
AUG_EXPORT void aug_view_attach(void);

/** Hand the clock back to Augustus' speed controls (possess mode) or take it again. */
AUG_EXPORT void aug_view_set_external_tick(int enabled);

/** Switch to the city window (after loading a scenario or a snapshot). */
AUG_EXPORT void aug_view_show_city(void);

/** Camera position in tiles. */
AUG_EXPORT void aug_view_set_camera(int x, int y);
AUG_EXPORT int aug_view_camera_x(void);
AUG_EXPORT int aug_view_camera_y(void);

/** Centre the camera on a map grid offset. */
AUG_EXPORT void aug_view_go_to_grid_offset(int grid_offset);

/** Pause or resume Augustus' own clock (only matters when external tick is off). */
AUG_EXPORT void aug_view_set_paused(int paused);

/**
 * Observe lock: while on, windows that change the city (build menu, advisors, empire map,
 * legion control, file and quit dialogs) do not open and no construction type can be set.
 * Building info, overlays and the message list still work.
 */
AUG_EXPORT void aug_view_set_observe_lock(int locked);
int aug_view_observe_locked(void);
int aug_view_window_allowed(int window_id);

/** Collapse (1) or expand (0) the city sidebar. */
AUG_EXPORT void aug_view_set_sidebar_collapsed(int collapsed);
AUG_EXPORT int aug_view_sidebar_collapsed(void);

/** Resize the SDL window / canvas (logical pixels). */
AUG_EXPORT void aug_view_resize(int width, int height);

/** Map grid offset under a canvas pixel, 0 if outside the city viewport. */
AUG_EXPORT int aug_view_screen_to_grid_offset(int x, int y);

/** Zoom in percent (100 = native), clamped by the engine. */
AUG_EXPORT void aug_view_set_scale(int percent);
AUG_EXPORT int aug_view_scale(void);

#endif // API_AUG_VIEW_H
