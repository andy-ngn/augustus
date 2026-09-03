#ifndef API_AUG_DIVINE_H
#define API_AUG_DIVINE_H

/**
 * Pantheon divine API: miracles, disasters and the few direct adjustments the god layer
 * needs. Gods are the engine's: 0 Ceres/Demeter, 1 Neptune/Poseidon, 2 Mercury/Hermes,
 * 3 Mars/Ares, 4 Venus/Aphrodite. Every function returns immediately; effects play out in
 * the following ticks like the engine's own events.
 */

#include "api/aug_api.h"

// --- the engine's own blessings and curses -----------------------------------------------

/** Run the god's blessing (e.g. Ceres fills farms, Neptune doubles trade). @return 1 if valid */
AUG_EXPORT int aug_god_bless(int god);
/** Run the god's minor (0) or major (1) curse. @return 1 if valid */
AUG_EXPORT int aug_god_curse(int god, int major);
AUG_EXPORT int aug_god_happiness(int god);
AUG_EXPORT void aug_god_change_happiness(int god, int amount);
AUG_EXPORT void aug_god_set_happiness(int god, int value);
/**
 * 0 stops the engine's monthly mood logic from blessing/cursing for this god (the player's
 * deity acts only through this API; bolts still accumulate and are reported by the stats).
 */
AUG_EXPORT void aug_god_set_autonomous(int god, int autonomous);
/** 0 turns popup messages into plain list entries. Default 1. */
AUG_EXPORT void aug_set_popups(int enabled);

// --- adjustments --------------------------------------------------------------------------

/** Add (or remove, negative) denarii. @return the treasury afterwards */
AUG_EXPORT int aug_money(int amount);
/**
 * Add food to the city's granaries (amount > 0, ignores storage orders) or remove it.
 * @param resource A food resource id (1 wheat, 2 vegetables, 3 fruit, 4 olives... see resource.h)
 * @return the amount that could NOT be added or removed (0 = the whole request was honoured)
 */
AUG_EXPORT int aug_food(int resource, int amount);
AUG_EXPORT void aug_health_change(int amount);
AUG_EXPORT void aug_sentiment_change(int amount);
/** Change the morale of every legion (+/-). */
AUG_EXPORT void aug_legion_morale(int amount);
/** Start (months > 0) or clear (0) trade problems on land and/or sea routes. */
AUG_EXPORT void aug_trade_problems(int land, int sea, int months);

// --- disasters ---------------------------------------------------------------------------

/** Set the building on the tile ablaze. @return the building id, 0 if nothing burnable there */
AUG_EXPORT int aug_fire(int grid_offset);
/** Collapse the building on the tile. @return the building id, 0 if none */
AUG_EXPORT int aug_collapse(int grid_offset);
/** Infect the building on the tile (house, granary, warehouse or dock). @return building id or 0 */
AUG_EXPORT int aug_plague_building(int grid_offset);
/** Infect up to max_buildings storage buildings and inhabited houses spread over the city. @return count */
AUG_EXPORT int aug_plague(int max_buildings);
/** Start an earthquake at (x, y): severity 1 small, 2 medium, 3 large. @return 1 if started */
AUG_EXPORT int aug_earthquake(int x, int y, int severity);
/**
 * Start an invasion now.
 * @param type 1 local uprising, 2 enemy army, 3 Caesar's legions, 5 natives
 * @param size Number of soldiers
 * @param invasion_point 0-7, one of the scenario's entry points (see aug_invasion_points)
 * @param attack_type 0 food chain, 1 gold stores, 2 best buildings, 3 troops, 4 random
 * @param enemy_id -1 for the scenario's enemy, else 0 barbarian, 1 numidian, 2 gaul, 3 celt,
 *        4 goth, 5 pergamum, 6 seleucid, 7 etruscan, 8 greek, 9 egyptian, 10 carthaginian
 */
AUG_EXPORT int aug_invade(int type, int size, int invasion_point, int attack_type, int enemy_id);
/** Release a pack of wolves (1-16) at (x, y). @return the formation id, 0 on failure */
AUG_EXPORT int aug_wolves(int x, int y, int count);
/** Weather: 0 none, 1 rain, 2 snow, 3 sandstorm; intensity as the scenario events use it. */
AUG_EXPORT void aug_weather(int type, int intensity);

#endif // API_AUG_DIVINE_H
