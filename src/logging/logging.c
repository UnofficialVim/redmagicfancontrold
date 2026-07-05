#include "logging.h"
#include <stdio.h>
#include "../runtime/runtime.h"


int logger_init(Runtime *rt, const char *path) {
    //rt->logger.log_file = fopen(path, "a");

    return 0;
}
void logger_set_level(Runtime *rt, int level) {
    rt->config.current_log_level = level;
}
void logger_write(Runtime *rt, int level, const char *fmt, ...) {

    va_list args;
    va_start(args, fmt);
    //vfprintf(logger->log_file, fmt, args);
    //fprintf(logger->log_file, "\n");
    va_end(args);
}
void logger_console(Runtime *rt, int level, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

void logger_close(Runtime *rt) {
    //convert logfile to a string and then close it
    char* log_file = rt->config.log_file;
    printf(log_file); 
}