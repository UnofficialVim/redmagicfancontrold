#ifndef LOGGING_H
#define LOGGING_H

#include <stdarg.h>
#include <stdio.h>
struct Runtime;


int logger_init(struct Runtime *rt, const char *path);
void logger_set_level(struct Runtime *rt, int level);
void logger_write(struct Runtime *rt, int level, const char *fmt, ...);
void logger_console(struct Runtime *rt, int level, const char *fmt, ...);
void logger_close(struct Runtime *rt);

#endif