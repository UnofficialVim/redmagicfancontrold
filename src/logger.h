#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

typedef struct {
  const char *fmt;
  const char *file;
  va_list ap;
  struct tm *time;
  int level;
  int line;
  int errno_value;
  bool show_errno;
  void *udata;
} LoggerEvent;

enum {
  LOGGER_TRACE,
  LOGGER_DEBUG,
  LOGGER_INFO,
  LOGGER_WARN,
  LOGGER_ERROR,
  LOGGER_FATAL

};

typedef void (*logger_log_fn)(LoggerEvent *ev);
typedef void (*logger_lock_fn)(bool lock, void *udata);

#define logger_trace(...)                                                      \
  logger_log(LOGGER_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define logger_debug(...)                                                      \
  logger_log(LOGGER_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define logger_info(...)                                                       \
  logger_log(LOGGER_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define logger_warn(...)                                                       \
  logger_log(LOGGER_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define logger_error(...)                                                      \
  logger_log(LOGGER_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define logger_fatal(...)                                                      \
  logger_log(LOGGER_FATAL, __FILE__, __LINE__, __VA_ARGS__)
#define logger_errno(level, ...)                                               \
  logger_log_errno(level, __FILE__, __LINE__, __VA_ARGS__)

const char *logger_level_string(int level);
void logger_set_lock(logger_lock_fn fn, void *udata);
void logger_set_level(int level);
void logger_set_quiet(bool enable);
int logger_add_callback(logger_log_fn fn, void *udata, int level);
int logger_add_fp(FILE *fp, int level);

void logger_log(int level, const char *file, int line, const char *fmt, ...);
void logger_log_errno(int level, const char *file, int line, const char *fmt,
                      ...);

#endif
