#include <stdbool.h>
#include <stddef.h>
#include "runtime.h"
#include "../logging/logging.h"

void runtime_init(Runtime *rt) {
    rt->running = true;

    //log_init("redmagicfancontrold.log");
    //log_write(LOG_INFO, "RedMagic Fan Control Daemon started");

    config_init(rt);
    fan_init(rt);
    socket_init(rt);
}
void runtime_cleanup(Runtime *rt) {
    rt->running = false;

    //log_write(LOG_INFO, "RedMagic Fan Control Daemon shutting down");
    //log_cleanup(&rt->logger);
}