#include <errno.h>
#include <poll.h>
#include <stddef.h>
#include <time.h>
#include <unistd.h>

#include "../config.h"
#include "../fan.h"
#include "../logger.h"
#include "../socket.h"
#include "../temperature.h"
#include "engine.h"
#include "runtime.h"
#include <stdatomic.h>

const int REFRESH_RATE = 5; // stub for now

void engine_run(Runtime *rt) {
  logger_info("Starting Event Loop...");

  time_t next_fan_check = time(NULL) + REFRESH_RATE;

  // Main event loop
  while (rt->running && !rt->shutdown_requested) {
    logger_trace("Engine : Event Loop Iteration started");
    struct pollfd fds[2];
    int nfds = 0;

    int server_idx = nfds;
    fds[nfds].fd = rt->socket.server_fd;
    fds[nfds].events = POLLIN;
    nfds++;

    int client_idx = -1;
    if (rt->socket.client_fd >= 0) {
      client_idx = nfds;
      fds[nfds].fd = rt->socket.client_fd;
      fds[nfds].events = POLLIN;
      nfds++;
    }

    time_t now = time(NULL);
    int timeout_ms = (int)(next_fan_check - now) * 1000;
    if (timeout_ms < 0)
      timeout_ms = 0;

    int ret = poll(fds, nfds, timeout_ms);

    if (ret < 0) {
      if (errno == EINTR)
        logger_errno(LOGGER_INFO, 0, "Poll() interrupted by signal");
      continue; // interrupted by signal, just re-check rt->running

      logger_fatal("Poll() failed");
      break;
    }

    if (ret > 0) {
      if (fds[server_idx].revents & POLLIN)
        socket_accept(rt);

      if (client_idx >= 0 &&
          (fds[client_idx].revents & (POLLIN | POLLHUP | POLLERR)))
        socket_receive(rt);
    }

    now = time(NULL);
    if (now >= next_fan_check) {
      // update temperature readings
      rt->temperature.cpu_temp = temperature_get_cpu_temp(rt);
      logger_debug("Current CPU Temperature: %d", rt->temperature.cpu_temp);
      logger_info("Current CPU Temperature: %s", get_temperature_formatted_string(rt->temperature.cpu_temp));

      // find where the current temperature falls in the fan curve and set the
      // fan speed accordingly
      if (rt->config.active) {
        Profile *loaded_profile = rt->config.active;
        int target_speed = 0;
        for (size_t i = 0; i < loaded_profile->steps_count; i++) {
          if (rt->temperature.cpu_temp >= loaded_profile->steps[i].temp_c) {
            logger_debug("Temperature %d >= step %zu temp %d, setting target "
                         "speed to %d",
                         rt->temperature.cpu_temp, i, loaded_profile->steps[i].temp_c,
                         loaded_profile->steps[i].fan_lvl);
            target_speed = loaded_profile->steps[i].fan_lvl;
          } else {
            break; // temperature is below this step, so stop checking
          }
        }
        if (rt->temperature.cpu_temp == -1) {
          logger_debug("Temperature reading failed, not changing fan speed");
        } else {
          logger_debug("Setting fan speed to %d based on CPU temperature %d",
                       target_speed, rt->temperature.cpu_temp);
          fan_set_speed(rt, target_speed); // either this or check it first but
                                           // you gotta open it anyway
        }
      }
      next_fan_check = now + REFRESH_RATE;
    }
  }
}

void engine_shutdown(Runtime *rt) {
  logger_info("Shutting Down Engine");
  runtime_cleanup(rt);
}