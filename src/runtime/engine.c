#include <stddef.h>
#include <unistd.h>

#include "runtime.h"
#include "engine.h"
#include "../logging.h"
#include "../config.h"
#include "../fan.h"
#include "../socket.h"

const int REFRESH_RATE = 5;

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
        if (fan_get_speed(rt) == 0){
            logger_write(rt, 1, "fan_get_speed failed with a non zero value");
        };

        sleep(REFRESH_RATE);//stub for now
    }
}

void engine_shutdown(Runtime *rt) {
    logger_write(rt, 2, "Shutting Down Engine");
    runtime_cleanup(rt);
}