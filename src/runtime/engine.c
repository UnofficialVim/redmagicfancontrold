#include <errno.h>
#include <poll.h>
#include <stddef.h>
#include <time.h>
#include <unistd.h>

#include "../config.h"
#include "../fan.h"
#include "../logging.h"
#include "../socket.h"
#include "engine.h"
#include "runtime.h"
#include <stdatomic.h>
#include "../temperature.h"


const int REFRESH_RATE = 5; // stub for now

int engine_init(Runtime *rt) {
  logger_write(rt, 2, "Initialized Engine");
  return 0;
}

void engine_run(Runtime *rt) {
  logger_write(rt, 2, "Starting Event Loop...");

  time_t next_fan_check = time(NULL) + REFRESH_RATE;

  // Main event loop
  while (rt->running && !rt->shutdown_requested) {
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
        continue; // interrupted by signal, just re-check rt->running
      logger_write(rt, 0, "poll() failed");
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
      //update temperature readings
      rt->temperature.cpu_temp = temperature_get_cpu_temp(rt);
      logger_write(rt, 3, "Current CPU Temperature: %d", rt->temperature.cpu_temp);

      //find where the current temperature falls in the fan curve and set the fan speed accordingly
      if (rt->config.active) {
        FanCurve *curve = &rt->config.active->fan_curve;
        int target_speed = 0;
        for (size_t i = 0; i < curve->step_count; i++) {
          if (rt->temperature.cpu_temp >= curve->steps[i].temp_c) {
            target_speed = curve->steps[i].fan_pct;
          } else {
            break; //temperature is below this step, so stop checking
            }
        }
        logger_write(rt, 3, "Setting fan speed to %d based on CPU temperature %d", target_speed, rt->temperature.cpu_temp);
        fan_set_speed(rt, target_speed);//either this or check it first but you gotta open it anyway
      
      }

      next_fan_check = now + REFRESH_RATE;
    }
  }
}

void engine_shutdown(Runtime *rt) {
  logger_write(rt, 2, "Shutting Down Engine");
  runtime_cleanup(rt);
}