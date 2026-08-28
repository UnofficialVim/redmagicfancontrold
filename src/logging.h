#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
struct Runtime;

typedef struct Logger {
    FILE *file;
    bool write_to_console;
    int current_log_level;
} Logger;



int logger_init(struct Runtime *rt);
void logger_set_level(struct Runtime *rt, int level);
void logger_write(struct Runtime *rt, int level, const char *fmt, ...);
void logger_console(struct Runtime *rt, int level, const char *fmt, ...);
void logger_close(struct Runtime *rt);

#endif
