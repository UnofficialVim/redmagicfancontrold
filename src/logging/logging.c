#include "logging.h"
#include <stdio.h>

static FILE *log_file = NULL;
static log_level_t current_level = LOG_INFO;

int log_init(const char *path)
{
    log_file = fopen(path, "a");

    if (!log_file)
        return -1;

    return 0;
}

void log_close(void)
{
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}

void log_write(log_level_t level, const char *fmt, ...)
{
    if (level > current_level)
        return;

    if (!log_file)
        return;

    va_list args;
    va_start(args, fmt);

    vfprintf(log_file, fmt, args);
    fprintf(log_file, "\n");
    fflush(log_file);

    va_end(args);
}

void log_set_level(log_level_t level)
{
    current_level = level;
}