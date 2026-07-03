#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>
#include <stdio.h>
typedef struct Runtime Runtime;

typedef struct Logger {
    FILE *log_file;
    enum {
        LOG_ERROR = 0,
        LOG_WARN,
        LOG_INFO,
        LOG_DEBUG
    } current_level;
} Logger;

int log_init(Runtime *rt, const char *path);
void log_set_level(Logger *logger, int level);
void log_write(Logger *logger, int level, const char *fmt, ...);
void log_console(Logger *logger, int level, const char *fmt, ...);
void log_close(Logger *logger);

#endif