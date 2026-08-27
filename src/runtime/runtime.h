#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>
#include <stddef.h>
#include "../logging.h"
#include "../config.h"
#include "../fan.h"
#include "../socket.h"
#include <signal.h>
 #include "../temperature.h"

typedef struct Runtime
{
    bool running;

    volatile sig_atomic_t shutdown_requested;

    Logger logger;

    Config config;

    Fan fan;

    Socket socket;

    Temperature temperature;

} Runtime;

void runtime_init(Runtime *rt);
void runtime_cleanup(Runtime *rt);

#endif // RUNTIME_H