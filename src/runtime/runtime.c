#include <stdbool.h>
#include <stddef.h>
#include "runtime.h"
#include "../logging.h"

void runtime_init(Runtime *rt) {
    rt->running = true;
    config_init(rt);//load config before anything else
    logger_init(rt);
    fan_init(rt);
    socket_init(rt);
}
void runtime_cleanup(Runtime *rt) {

    socket_cleanup(rt);
    logger_write(rt, 2, "Cleaning up Runtime");
    rt->running = false;
}