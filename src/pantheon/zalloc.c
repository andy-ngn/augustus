#include "pantheon/zalloc.h"

#include <stdlib.h>

void *pantheon_malloc(size_t size)
{
    return calloc(1, size ? size : 1);
}
