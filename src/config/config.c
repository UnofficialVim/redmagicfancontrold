#include "config.h"
#include "../external/cjson/cJSON.h"
#include "../runtime/runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void config_init(Runtime *rt) {
    // Initialize the configuration structure
    memset(&rt->config, 0, sizeof(Config));

    FILE* fp = fopen("../config/config.json", "r");
    if (fp != NULL) {
        // Read the file contents into a buffer
        fseek(fp, 0, SEEK_END);
        long file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char* buffer = (char*)malloc(file_size + 1);
        if (buffer != NULL) {
            fread(buffer, 1, file_size, fp);
            buffer[file_size] = '\0'; // Null-terminate the string

            // Parse the JSON data
            cJSON* json = cJSON_Parse(buffer);
            if (json != NULL) {
                //load the config data from the JSON object into the Config structure
                //loop thro
                cJSON_Delete(json);
            } else {
                fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr());
            }

            free(buffer);
        } else {
            fprintf(stderr, "Memory allocation failed for buffer\n");
        }

        fclose(fp);
    } else {
        fprintf(stderr, "Could not open config.json for reading\n");
    }
}

void config_save(Runtime *rt) {
    //rt->config.version = 1;
}

void config_load(Runtime *rt) {
    //rt->config.version = 1;//stub
}