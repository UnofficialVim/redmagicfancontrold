#include <stddef.h>
#include "../runtime/runtime.h"
#include "engine.h"

#include "../logging/logging.h"
#include "../config/config.h"
#include "../fan/fan.h"
#include "../ipc/socket.h"


int engine_init(Runtime *rt)
{
    //logger_init(&rt);
    //config_init(&rt);
    //fan_init(&rt);
    //socket_init(&rt);

    return 0;
}

void engine_run(Runtime *rt) {
    while (rt->running) {
        //main loop of the engine
    }
}

void engine_shutdown(Runtime *rt) {
    // Implementation for shutting down the engine
}