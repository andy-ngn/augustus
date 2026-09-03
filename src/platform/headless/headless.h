#ifndef PLATFORM_HEADLESS_HEADLESS_H
#define PLATFORM_HEADLESS_HEADLESS_H

#include <stdint.h>

/**
 * Pantheon headless platform: no window, no renderer, no audio, no input.
 * The simulation is driven externally (native CLI or the aug_* WebAssembly API).
 */

/** Directory the engine writes preferences/config into. Must end with a path separator. */
void headless_set_pref_path(const char *path);

/** Directory that contains the "assets" folder (Augustus extra assets). */
void headless_set_assets_base_path(const char *path);

/** Virtual clock, in milliseconds. Replaces SDL_GetTicks(). */
void headless_set_ticks(uint64_t millis);
uint64_t headless_get_ticks(void);

/** Set when the engine asks to exit (system_exit / exit_with_status). */
int headless_exit_requested(void);

#endif // PLATFORM_HEADLESS_HEADLESS_H
