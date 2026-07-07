#include <stdio.h>
#include "runtime/runtime.h"
#include "runtime/engine.h"


int main(void) {
    Runtime rt = {0};

	runtime_init(&rt);

    engine_init(&rt);

    engine_run(&rt);

    engine_shutdown(&rt);

    return 0;
}
