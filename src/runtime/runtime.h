#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include "../logging/logging.h"
#include "../config/config.h"
#include "../fan/fan.h"
#include "../ipc/socket.h"

typedef struct Runtime
{
    bool running;

    Config config;

    Fan fan;

    Socket socket;

} Runtime;

void runtime_init(Runtime *rt);
void runtime_cleanup(Runtime *rt);

#endif // RUNTIME_H