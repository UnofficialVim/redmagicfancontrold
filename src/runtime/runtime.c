#include "runtime.h"
#include "../logging.h"
#include <stdbool.h>
#include <stddef.h>
#include <signal.h>


static void handle_shutdown_signal(int sig) {
  (void)sig;
    // Set the shutdown_requested flag in the Runtime struct
    Runtime *rt = (Runtime *)sig; // Cast the signal number to Runtime pointer
    rt->shutdown_requested = 1;
}

void runtime_init(Runtime *rt) {
  rt->running = true;
  rt->shutdown_requested = 0;

  struct sigaction sa = {0};
  sa.sa_handler = handle_shutdown_signal;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0; // no SA_RESTART: let blocking epoll_wait return EINTR
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);

  config_init(rt); // load config before anything else
  logger_init(rt);
  fan_init(rt);
  socket_init(rt);
  temperature_init(rt);
}
void runtime_cleanup(Runtime *rt) {

  socket_cleanup(rt);
  logger_write(rt, 2, "Cleaning up Runtime");
  rt->running = false;
}