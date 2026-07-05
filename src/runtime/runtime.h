#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include "../logging.h"
#include "../config.h"
#include "../fan.h"
#include "../socket.h"

typedef struct Runtime
{
    bool running;

    Logger logger;

    Config config;

    Fan fan;

    Socket socket;

} Runtime;

void runtime_init(Runtime *rt);
void runtime_cleanup(Runtime *rt);

#endif // RUNTIME_H