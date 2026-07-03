#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>

typedef enum {
    LOG_ERROR = 0,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
} log_level_t;

int log_init(const char *path);
void log_set_level(log_level_t level);
void log_write(log_level_t level, const char *fmt, ...);
void log_close(void);

#endif