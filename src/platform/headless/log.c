#include "platform/log.h"

#include <stdio.h>

static void (*output_function)(const char *message, int is_error);

void platform_log_set_output_function(void (*callback)(const char *message, int is_error))
{
    output_function = callback;
}

void platform_log_message(const char *message, int is_error)
{
    if (output_function) {
        output_function(message, is_error);
        return;
    }
    fputs(message, is_error ? stderr : stdout);
    fputc('\n', is_error ? stderr : stdout);
}
