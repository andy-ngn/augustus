#ifndef BUILDING_ANIMATION_H
#define BUILDING_ANIMATION_H

#include "building/building.h"

int building_animation_offset(building *b, int image_id, int grid_offset);

int building_animation_advance_storage_flag(building *b, int image_id);

int building_animation_advance_fumigation(building *b);

#ifdef PANTHEON
int building_animation_fumigation_frame(const building *b);
void building_animation_set_fumigation_direction(const building *b, int direction);
#endif

#endif // BUILDING_ANIMATION_H
