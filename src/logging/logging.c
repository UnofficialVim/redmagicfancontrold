#include "logging.h"
#include <stdio.h>
#include "../runtime/runtime.h"


int log_init(Runtime *rt, const char *path) {
    //rt->logger.log_file = fopen(path, "a");
    if (!rt->logger.log_file) {
        return -1; // Failed to open log file
    }
    rt->logger.current_level = 0; // Default log level
    return 0;
}
void log_set_level(Logger *logger, int level) {
    logger->current_level = level;
}
void log_write(Logger *logger, int level, const char *fmt, ...) {
    if (level > logger->current_level) {
        return; // Skip logging if the level is higher than the current level
    }

    va_list args;
    va_start(args, fmt);
    //vfprintf(logger->log_file, fmt, args);
    //fprintf(logger->log_file, "\n");
    va_end(args);
}
void log_console(Logger *logger, int level, const char *fmt, ...) {
    if (level > logger->current_level) {
        return; // Skip logging if the level is higher than the current level
    }
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

void log_close(Logger *logger) {
    if (logger->log_file) {
        logger->log_file = NULL;
    }
}