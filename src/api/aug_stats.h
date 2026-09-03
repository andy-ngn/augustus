#ifndef API_AUG_STATS_H
#define API_AUG_STATS_H

/**
 * Pantheon read-only queries: packed city statistics, the message list, buildings and the
 * map grid. All of it is available in the headless and the viewer builds.
 */

#include "api/aug_api.h"

// --- packed stats: indices come from aug_stats.def (see tools/gen_stats.py) -----------------

AUG_EXPORT int aug_stats_count(void);
/** Recompute and return the stats array (aug_stats_count() int32 values, engine-owned). */
AUG_EXPORT const int32_t *aug_stats(void);
AUG_EXPORT const char *aug_stats_name(int index);

// --- messages ----------------------------------------------------------------------------

/**
 * Copy messages newer than since_sequence into out (6 ints each: sequence, type, year,
 * month, param1, param2), oldest first. @return the number of messages copied
 */
AUG_EXPORT int aug_messages(int since_sequence, int32_t *out, int max_messages);
/** Title / body of a message type in the game's text encoding (mostly ASCII). May be empty. */
AUG_EXPORT const char *aug_message_title(int message_type);
AUG_EXPORT const char *aug_message_content(int message_type);

// --- buildings and map --------------------------------------------------------------------

/** Id of the (main) building on the tile, 0 if none. */
AUG_EXPORT int aug_building_at(int grid_offset);
/**
 * Fill out with 10 ints: type, x, y, size, state, house population, has plague, grid offset,
 * workers, house size. @return 1 if the id is valid
 */
AUG_EXPORT int aug_building_info(int building_id, int32_t *out);
AUG_EXPORT const char *aug_building_name(int type);
/** Number of buildings slots in use (ids are 1..count-1, some slots are empty). */
AUG_EXPORT int aug_building_count(void);

AUG_EXPORT int aug_grid_offset(int x, int y);
AUG_EXPORT int aug_grid_x(int grid_offset);
AUG_EXPORT int aug_grid_y(int grid_offset);
/** Fill out with up to 8 (x, y) pairs of valid invasion points. @return how many */
AUG_EXPORT int aug_invasion_points(int32_t *out);

#endif // API_AUG_STATS_H
