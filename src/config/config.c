#include "config.h"
#include "../external/cjson/cJSON.h"
#include "../runtime/runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void config_init(Runtime *rt) {
    // Initialize the configuration structure
    memset(&rt->config, 0, sizeof(Config));

    // Load the configuration from a file or set default values
    config_load(rt);
}

void config_save(Runtime *rt) {
    rt->config.version = 1;//stub
}

void config_load(Runtime *rt) {
    rt->config.version = 1;//stub
}