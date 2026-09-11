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

static void ts_add_seconds(struct timespec *ts, double seconds) {
  long whole = (long)seconds;
  double frac = seconds - (double)whole;
  ts->tv_sec += whole;
  ts->tv_nsec += (long)(frac * 1e9);
  if (ts->tv_nsec >= 1000000000L) {
    ts->tv_nsec -= 1000000000L;
    ts->tv_sec += 1;
  }
}

void engine_run(Runtime *rt) {
  logger_info("Starting Event Loop...");

  struct timespec next_fan_check;
  clock_gettime(CLOCK_MONOTONIC, &next_fan_check);
  ts_add_seconds(&next_fan_check, rt->config.active->refresh_rate);

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

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long timeout_ms = (next_fan_check.tv_sec - now.tv_sec) * 1000 +
                      (next_fan_check.tv_nsec - now.tv_nsec) / 1000000;
    if (timeout_ms < 0)
      timeout_ms = 0;

    int ret = poll(fds, nfds, (int)timeout_ms);

    if (ret < 0) {
      if (errno == EINTR) {
        logger_errno(LOGGER_INFO, 0, "Poll() interrupted by signal");
        continue; // interrupted by signal, just re-check rt->running
      }
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

    clock_gettime(CLOCK_MONOTONIC, &now);
    if (now.tv_sec >= next_fan_check.tv_sec ||
        now.tv_sec == next_fan_check.tv_sec &&
            now.tv_nsec >= next_fan_check.tv_nsec) {
      logger_trace("Time to check fan speed based on CPU temperature");
      {
        // update temperature readings
        rt->temperature.cpu_temp = temperature_get_cpu_temp(rt);
        logger_debug("Current CPU Temperature: %d", rt->temperature.cpu_temp);
        logger_info("Current CPU Temperature: %s",
                    get_temperature_formatted_string(rt->temperature.cpu_temp));

        // find where the current temperature falls in the fan curve and set the
        // fan speed accordingly
        if (rt->config.active) {
          Profile *loaded_profile = rt->config.active;
          int target_speed = 0;
          for (size_t i = 0; i < loaded_profile->steps_count; i++) {
            if (rt->temperature.cpu_temp >= loaded_profile->steps[i].temp_c) {
              logger_debug("Temperature %d >= step %zu temp %d, setting target "
                           "speed to %d",
                           rt->temperature.cpu_temp, i,
                           loaded_profile->steps[i].temp_c,
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
            fan_set_speed(rt, target_speed); // either this or check it first
                                             // but you gotta open it anyway
          }

        } else {
          logger_fatal("No active profile loaded, cannot set fan speed");
        }
        next_fan_check = now;
        ts_add_seconds(&next_fan_check, rt->config.active->refresh_rate);
      }
    }
  }
  if (rt->shutdown_requested) {
    logger_info("Shutdown requested, exiting event loop");
  }
}

void engine_shutdown(Runtime *rt) {
  logger_info("Shutting Down Engine");
  runtime_cleanup(rt);
}