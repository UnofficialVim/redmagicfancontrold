#include "runtime.h"
#include "../logger.h"
#include <stdbool.h>
#include <stddef.h>
#include <signal.h>
#include <unistd.h>

static Runtime *g_rt = NULL; // Global pointer to the Runtime instance

static void handle_shutdown_signal(int sig) {
  (void)sig;
  if (g_rt)
    g_rt->shutdown_requested = 1;
}

void runtime_init(Runtime *rt) {
  rt->running = true;
  rt->shutdown_requested = 0;
  g_rt = rt;

  struct sigaction sa = {0};
  sa.sa_handler = handle_shutdown_signal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0; // no SA_RESTART: let blocking epoll_wait return EINTR
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);
  
  config_init(rt); // load config before anything else
  fan_init(rt);
  socket_init(rt);
  temperature_init(rt);
}
void runtime_cleanup(Runtime *rt) {

  socket_cleanup(rt);
  logger_info("Cleaning up Runtime");
  rt->running = false;
}

char* get_running_dir() {
  static char cwd[1024];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    logger_debug("Current working directory: %s", cwd);
    return cwd;
  } else {
    logger_errno(LOGGER_ERROR, "Failed to get current working directory");
    return NULL;
  }
}