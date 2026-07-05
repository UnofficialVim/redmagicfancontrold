#include "logging.h"
#include <stdio.h>
#include "../runtime/runtime.h"
#include <time.h>


char *time_stamp(void)
{
    static char buffer[100];

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", t);
    return buffer;
}

int logger_init(Runtime *rt)
{
    // Open the log file in append mode
    rt->logger.file = fopen(rt->config.log_file, "a+");
    if (rt->logger.file == NULL)
    {
        printf("Failed to open log file '%s'\n", rt->config.log_file ? rt->config.log_file : "(null)");
        return -1; // Indicate initialization failure
    }
    // Write init message
    logger_write(rt, 2, "Logger initialized");
    return 0;
}
void logger_set_level(Runtime *rt, int level)
{
}
void logger_write(Runtime *rt, int level, const char *fmt, ...)
{
    if (rt->config.current_log_level)//add logic
    {
        va_list args;
        va_start(args, fmt);
        rt->logger.file = fopen(rt->config.log_file, "a+");
        fprintf(rt->logger.file, "[%s] ", time_stamp());
        vfprintf(rt->logger.file, fmt, args);
        fprintf(rt->logger.file, "\n");
        fclose(rt->logger.file);
        va_end(args);
    }
}
void logger_console(Runtime *rt, int level, const char *fmt, ...)
{
}
void logger_close(Runtime *rt)
{
}
