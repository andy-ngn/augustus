#include "api/aug_divine.h"

#include "building/building.h"
#include "building/destruction.h"
#include "building/granary.h"
#include "city/constants.h"
#include "city/finance.h"
#include "city/gods.h"
#include "city/health.h"
#include "city/sentiment.h"
#include "city/trade.h"
#include "core/direction.h"
#include "core/random.h"
#include "figure/action.h"
#include "figure/figure.h"
#include "figure/formation.h"
#include "figure/type.h"
#include "game/resource.h"
#include "graphics/weather.h"
#include "map/building.h"
#include "map/grid.h"
#include "pantheon/rules.h"
#include "scenario/earthquake.h"
#include "scenario/invasion.h"

#define DIVINE_GODS 5

static int valid_god(int god)
{
    return god >= 0 && god < DIVINE_GODS;
}

int aug_god_bless(int god)
{
    if (!valid_god(god)) {
        return 0;
    }
    city_god_blessing(god);
    return 1;
}

int aug_god_curse(int god, int major)
{
    if (!valid_god(god)) {
        return 0;
    }
    city_god_curse(god, major ? 1 : 0);
    return 1;
}

int aug_god_happiness(int god)
{
    return valid_god(god) ? city_god_happiness(god) : 0;
}

void aug_god_change_happiness(int god, int amount)
{
    if (valid_god(god)) {
        city_god_change_happiness(god, amount);
    }
}

void aug_god_set_happiness(int god, int value)
{
    if (valid_god(god)) {
        city_god_set_happiness(god, value);
    }
}

void aug_god_set_autonomous(int god, int autonomous)
{
    pantheon_set_god_autonomous(god, autonomous);
}

void aug_set_popups(int enabled)
{
    pantheon_set_popups_enabled(enabled);
}

int aug_money(int amount)
{
    city_finance_treasury_add_miscellaneous(amount);
    return city_finance_treasury();
}

int aug_food(int resource, int amount)
{
    if (resource < RESOURCE_MIN_FOOD || resource >= RESOURCE_MAX_FOOD) {
        return 0;
    }
    if (amount > 0) {
        return building_granaries_add_resource(resource, amount, 0);
    } else if (amount < 0) {
        return building_granaries_remove_resource(resource, -amount);
    }
    return 0;
}

void aug_health_change(int amount)
{
    city_health_change(amount);
}

void aug_sentiment_change(int amount)
{
    city_sentiment_change_happiness(amount);
}

void aug_legion_morale(int amount)
{
    formation_change_all_legions_morale(amount);
}

void aug_trade_problems(int land, int sea, int months)
{
    if (months < 0) {
        months = 0;
    }
    if (land) {
        city_trade_start_land_trade_problems(months);
    }
    if (sea) {
        city_trade_start_sea_trade_problems(months);
    }
}

static building *building_on_tile(int grid_offset)
{
    if (!map_grid_is_valid_offset(grid_offset)) {
        return 0;
    }
    int id = map_building_at(grid_offset);
    if (!id) {
        return 0;
    }
    building *b = building_main(building_get(id));
    if (b->state != BUILDING_STATE_IN_USE) {
        return 0;
    }
    return b;
}

int aug_fire(int grid_offset)
{
    building *b = building_on_tile(grid_offset);
    if (!b || b->type == BUILDING_BURNING_RUIN) {
        return 0;
    }
    int id = b->id;
    building_destroy_by_fire(b);
    return id;
}

int aug_collapse(int grid_offset)
{
    building *b = building_on_tile(grid_offset);
    if (!b || b->type == BUILDING_BURNING_RUIN) {
        return 0;
    }
    int id = b->id;
    building_destroy_by_collapse(b);
    return id;
}

static int plague_candidate(const building *b)
{
    if (b->state != BUILDING_STATE_IN_USE || b->has_plague) {
        return 0;
    }
    if (b->type == BUILDING_GRANARY || b->type == BUILDING_WAREHOUSE || b->type == BUILDING_DOCK) {
        return 1;
    }
    return b->house_size && b->house_population > 0;
}

int aug_plague_building(int grid_offset)
{
    building *b = building_on_tile(grid_offset);
    if (!b || !plague_candidate(b)) {
        return 0;
    }
    city_health_force_disease_in_building(b->id);
    return b->id;
}

int aug_plague(int max_buildings)
{
    if (max_buildings <= 0) {
        return 0;
    }
    int total = building_count();
    int candidates = 0;
    for (int id = 1; id < total; id++) {
        if (plague_candidate(building_get(id))) {
            candidates++;
        }
    }
    if (!candidates) {
        return 0;
    }
    int step = candidates / max_buildings;
    if (step < 1) {
        step = 1;
    }
    int infected = 0;
    int seen = 0;
    for (int id = 1; id < total && infected < max_buildings; id++) {
        if (!plague_candidate(building_get(id))) {
            continue;
        }
        if (seen++ % step == 0) {
            city_health_force_disease_in_building(id);
            infected++;
        }
    }
    return infected;
}

int aug_earthquake(int x, int y, int severity)
{
    return scenario_earthquake_start_at(x, y, severity);
}

int aug_invade(int type, int size, int invasion_point, int attack_type, int enemy_id)
{
    if (size <= 0) {
        return 0;
    }
    if (invasion_point < 0 || invasion_point >= 8) {
        invasion_point = 0;
    }
    if (attack_type < FORMATION_ATTACK_FOOD_CHAIN || attack_type > FORMATION_ATTACK_RANDOM) {
        attack_type = FORMATION_ATTACK_RANDOM;
    }
    switch (type) {
        case INVASION_TYPE_CAESAR:
            return scenario_invasion_start_from_caesar(size);
        case INVASION_TYPE_LOCAL_UPRISING:
        case INVASION_TYPE_ENEMY_ARMY:
        case INVASION_TYPE_MARS_NATIVES:
            scenario_invasion_start_from_action((invasion_type_enum) type, size, invasion_point,
                (formation_attack_enum) attack_type, (enemy_type_t) enemy_id);
            return 1;
        default:
            return 0;
    }
}

int aug_wolves(int x, int y, int count)
{
    if (!map_grid_is_inside(x, y, 1)) {
        return 0;
    }
    if (count < 1) {
        count = 1;
    } else if (count > 16) {
        count = 16;
    }
    int formation_id = formation_create_herd(FIGURE_WOLF, x, y, count);
    if (formation_id <= 0) {
        return 0;
    }
    for (int i = 0; i < count; i++) {
        random_generate_next();
        figure *f = figure_create(FIGURE_WOLF, x, y, DIR_0_TOP);
        f->action_state = FIGURE_ACTION_196_HERD_ANIMAL_AT_REST;
        f->formation_id = formation_id;
        f->wait_ticks = f->id & 0x1f;
    }
    // Stock wolf herds respawn while num_figures < max_figures (formation_herd.c can_spawn_wolf), which
    // would make a divine pack a permanent den. Zero max_figures so the pack is finite; the empty
    // formation slot stays in use afterwards, exactly like an extinct scenario herd.
    formation_get(formation_id)->max_figures = 0;
    return formation_id;
}

void aug_weather(int type, int intensity)
{
    if (type < WEATHER_NONE || type > WEATHER_SAND) {
        type = WEATHER_NONE;
    }
    set_weather(type != WEATHER_NONE, intensity, (weather_type) type);
}
