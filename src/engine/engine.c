#include <stddef.h>
#include "runtime/runtime.h"

void engine_init(Runtime *rt) {
    //inits other modules
}

void engine_run(Runtime *rt) {
    while (rt->running) {
        //main loop of the engine
    }
}

void engine_shutdown(Runtime *rt) {
    // Implementation for shutting down the engine
}