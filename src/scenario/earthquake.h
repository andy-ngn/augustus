#ifndef SCENARIO_EARTHQUAKE_H
#define SCENARIO_EARTHQUAKE_H

#include "core/buffer.h"

void scenario_earthquake_init(void);

void scenario_earthquake_process(void);

int scenario_earthquake_is_in_progress(void);

void scenario_earthquake_save_state(buffer *buf);

void scenario_earthquake_load_state(buffer *buf);

#ifdef PANTHEON
/** Pantheon: start an earthquake now at (x, y) with severity 1-3. @return 1 if started */
int scenario_earthquake_start_at(int x, int y, int severity);
#endif

#endif // SCENARIO_EARTHQUAKE_H
