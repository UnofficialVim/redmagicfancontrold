#ifndef ENGINE_H
#define ENGINE_H

#include "../runtime/runtime.h"

int engine_init(Runtime *rt);
void engine_run(Runtime *rt);
void engine_shutdown(Runtime *rt);

#endif