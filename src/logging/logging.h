#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>
#include <stdio.h>
struct Runtime;

typedef struct Logger {
    FILE *log_file;
    enum {
        LOG_ERROR = 0,
        LOG_WARN,
        LOG_INFO,
        LOG_DEBUG
    } current_level;
} Logger;

int logger_init(struct Runtime *rt, const char *path);
void logger_set_level(Logger *logger, int level);
void logger_write(Logger *logger, int level, const char *fmt, ...);
void logger_console(Logger *logger, int level, const char *fmt, ...);
void logger_close(Logger *logger);

#endif