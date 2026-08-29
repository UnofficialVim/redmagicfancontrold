#include "logger.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define MAX_CALLBACKS 32

typedef struct {
  logger_log_fn fn;
  void *udata;
  int level;
} Callback;

static struct {
  void *udata;
  logger_lock_fn lock;
  int level;
  bool quiet;
  Callback callbacks[MAX_CALLBACKS];
} L;

static const char *level_strings[] = {"TRACE", "DEBUG", "INFO",
                                      "WARN",  "ERROR", "FATAL"};
static const char *logger_strip_path(const char *path) {
  const char *slash = strrchr(path, '/');
  const char *backslash = strrchr(path, '\\');
  const char *filename = slash > backslash ? slash : backslash;
  return filename ? filename + 1 : path;
}

static void stdout_callback(LoggerEvent *ev) {
  char buf[18];
  buf[strftime(buf, sizeof(buf), "[%H:%M:%S]", ev->time)] = '\0';
  fprintf(ev->udata, "%s %-5s [%s:%d] ", buf, logger_level_string(ev->level),
          logger_strip_path(ev->file), ev->line);
  vfprintf(ev->udata, ev->fmt, ev->ap);
  if (ev->show_errno == true) {
    fprintf(ev->udata, " | Error Message: %s", strerror(ev->errno_value));
  }
  fprintf(ev->udata, "\n");
  fflush(ev->udata);
}

static void file_callback(LoggerEvent *ev) {
  char buf[64];
  buf[strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ev->time)] = '\0';
  fprintf(ev->udata, "%s %-5s %s:%d: ", buf, level_strings[ev->level], ev->file,
          ev->line);
  vfprintf(ev->udata, ev->fmt, ev->ap);
  if (ev->show_errno == true) {
    fprintf(ev->udata, " | Error Message: %s", strerror(ev->errno_value));
  }
  fprintf(ev->udata, "\n");
  fflush(ev->udata);
}

static void lock(void) {
  if (L.lock) {
    L.lock(true, L.udata);
  }
}

static void unlock(void) {
  if (L.lock) {
    L.lock(false, L.udata);
  }
}

const char *logger_level_string(int level) { return level_strings[level]; }

void logger_set_lock(logger_lock_fn fn, void *udata) {
  L.lock = fn;
  L.udata = udata;
}

void logger_set_level(int level) { L.level = level; }

void logger_set_quiet(bool enable) { L.quiet = enable; }

int logger_add_callback(logger_log_fn fn, void *udata, int level) {
  for (int i = 0; i < MAX_CALLBACKS; i++) {
    if (!L.callbacks[i].fn) {
      L.callbacks[i] = (Callback){fn, udata, level};
      return 0;
    }
  }
  return -1;
}

int logger_add_fp(FILE *fp, int level) {
  return logger_add_callback(file_callback, fp, level);
}

static void init_event(LoggerEvent *ev, void *udata) {
  if (!ev->time) {
    time_t t = time(NULL);
    ev->time = localtime(&t);
  }
  ev->udata = udata;
}

static void logger_dispatch(LoggerEvent *ev, const char *fmt, va_list ap) {
  lock();

  if (!L.quiet && ev->level >= L.level) {
    init_event(ev, stderr);
    va_copy(ev->ap, ap);

    stdout_callback(ev);

    va_end(ev->ap);
  }

  for (int i = 0; i < MAX_CALLBACKS && L.callbacks[i].fn; i++) {
    Callback *cb = &L.callbacks[i];

    if (ev->level >= cb->level) {
      init_event(ev, cb->udata);
      va_copy(ev->ap, ap);

      cb->fn(ev);

      va_end(ev->ap);
    }
  }

  unlock();
}

void logger_log(int level, const char *file, int line, const char *fmt, ...) {
  LoggerEvent ev = {
      .fmt = fmt,
      .file = file,
      .line = line,
      .level = level,
      .show_errno = false,
  };

  va_list ap;
  va_start(ap, fmt);

  logger_dispatch(&ev, fmt, ap);

  va_end(ap);
}

void logger_log_errno(int level, const char *file, int line, const char *fmt,
                      ...) {
  LoggerEvent ev = {
      .fmt = fmt,
      .file = file,
      .line = line,
      .level = level,
      .errno_value = errno,
      .show_errno = true,
  };

  va_list ap;
  va_start(ap, fmt);

  logger_dispatch(&ev, fmt, ap);

  va_end(ap);
}
