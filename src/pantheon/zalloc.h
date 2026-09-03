#ifndef PANTHEON_ZALLOC_H
#define PANTHEON_ZALLOC_H

/**
 * Pantheon: zero every heap allocation.
 *
 * Several *_save_state() writers malloc a buffer and leave gaps (buffer_skip, unused array
 * slots, oversized dynamic pieces), so a snapshot of the same simulation contains random
 * bytes and two identical instances never hash the same. This header is forced into every
 * translation unit of the Pantheon targets with "-include": it renames malloc to
 * pantheon_malloc (declaration in <stdlib.h> included), which zalloc.c implements with calloc.
 * The cost is negligible: large blocks come from fresh, already zeroed pages.
 */
#include <stddef.h>

void *pantheon_malloc(size_t size);

#define malloc pantheon_malloc

#endif // PANTHEON_ZALLOC_H
