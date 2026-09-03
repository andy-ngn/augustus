#include "api/aug_stats.h"

#include "building/building.h"
#include "building/count.h"
#include "building/type.h"
#include "city/constants.h"
#include "city/culture.h"
#include "city/festival.h"
#include "city/figures.h"
#include "city/finance.h"
#include "city/gods.h"
#include "city/health.h"
#include "city/labor.h"
#include "city/message.h"
#include "city/military.h"
#include "city/population.h"
#include "city/ratings.h"
#include "city/resource.h"
#include "city/sentiment.h"
#include "city/trade.h"
#include "core/lang.h"
#include "game/time.h"
#include "map/building.h"
#include "map/grid.h"
#include "scenario/data.h"
#include "scenario/earthquake.h"
#include "scenario/invasion.h"
#include "scenario/property.h"

enum {
#define STAT(name, expr) AUG_STAT_##name,
#include "api/aug_stats.def"
#undef STAT
    AUG_STAT_COUNT
};

static const char *const stat_names[] = {
#define STAT(name, expr) #name,
#include "api/aug_stats.def"
#undef STAT
};

static int32_t stat_values[AUG_STAT_COUNT];

int aug_stats_count(void)
{
    return AUG_STAT_COUNT;
}

const int32_t *aug_stats(void)
{
    int i = 0;
#define STAT(name, expr) stat_values[i++] = (int32_t) (expr);
#include "api/aug_stats.def"
#undef STAT
    return stat_values;
}

const char *aug_stats_name(int index)
{
    if (index < 0 || index >= AUG_STAT_COUNT) {
        return "";
    }
    return stat_names[index];
}

int aug_messages(int since_sequence, int32_t *out, int max_messages)
{
    int count = city_message_count();
    int copied = 0;
    // Slots 0..count-1 are always occupied (deleting compacts the list) but their order is not
    // guaranteed between compactions; the caller sorts by sequence.
    for (int i = 0; i < count && copied < max_messages; i++) {
        const city_message *m = city_message_get(i);
        if (!m || m->sequence <= since_sequence) {
            continue;
        }
        int32_t *dst = out + copied * 6;
        dst[0] = m->sequence;
        dst[1] = m->message_type;
        dst[2] = m->year;
        dst[3] = m->month;
        dst[4] = m->param1;
        dst[5] = m->param2;
        copied++;
    }
    return copied;
}

static const char *message_string(int message_type, int want_content)
{
    int text_id = city_message_get_text_id(message_type);
    const lang_message *msg = lang_get_message(text_id);
    if (!msg) {
        return "";
    }
    const uint8_t *text = want_content ? msg->content.text : msg->title.text;
    return text ? (const char *) text : "";
}

const char *aug_message_title(int message_type)
{
    return message_string(message_type, 0);
}

const char *aug_message_content(int message_type)
{
    return message_string(message_type, 1);
}

int aug_building_at(int grid_offset)
{
    if (!map_grid_is_valid_offset(grid_offset)) {
        return 0;
    }
    int id = map_building_at(grid_offset);
    if (!id) {
        return 0;
    }
    return building_main(building_get(id))->id;
}

int aug_building_info(int building_id, int32_t *out)
{
    if (building_id <= 0 || building_id >= building_count()) {
        return 0;
    }
    const building *b = building_get(building_id);
    out[0] = b->type;
    out[1] = b->x;
    out[2] = b->y;
    out[3] = b->size;
    out[4] = b->state;
    out[5] = b->house_population;
    out[6] = b->has_plague;
    out[7] = b->grid_offset;
    out[8] = b->num_workers;
    out[9] = b->house_size;
    return 1;
}

const char *aug_building_name(int type)
{
    const uint8_t *name = lang_get_building_type_string(type);
    return name ? (const char *) name : "";
}

int aug_building_count(void)
{
    return building_count();
}

int aug_grid_offset(int x, int y)
{
    return map_grid_offset(x, y);
}

int aug_grid_x(int grid_offset)
{
    return map_grid_offset_to_x(grid_offset);
}

int aug_grid_y(int grid_offset)
{
    return map_grid_offset_to_y(grid_offset);
}

int aug_invasion_points(int32_t *out)
{
    int n = 0;
    for (int i = 0; i < MAX_INVASION_POINTS; i++) {
        if (scenario.invasion_points[i].x != -1 && scenario.invasion_points[i].y != -1) {
            out[n * 2] = scenario.invasion_points[i].x;
            out[n * 2 + 1] = scenario.invasion_points[i].y;
            n++;
        }
    }
    return n;
}
