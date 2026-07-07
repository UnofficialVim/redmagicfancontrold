#include <stddef.h>
#include "runtime.h"
#include "engine.h"

#include "../logging.h"
#include "../config.h"
#include "../fan.h"
#include "../socket.h"


int engine_init(Runtime *rt)
{

    logger_write(rt, 2, "Initialized Engine");
    return 0;
}

void engine_run(Runtime *rt) {
    logger_write(rt, 2, "Starting Event Loop...");
    while (rt->running) {
        //main loop of the engine

        //check with runtime profile and reference current temp
    }
}

void engine_shutdown(Runtime *rt) {
    logger_write(rt, 2, "Shutting Down Engine");
    runtime_cleanup(rt);
}