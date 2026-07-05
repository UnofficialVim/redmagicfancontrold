#include "config.h"
#include "external/cjson/cJSON.h"
#include "runtime/runtime.h"

#include <string.h>
#include <strings.h>   // strcasecmp
#include <stdlib.h>
#include <stdio.h>

/*
TODO: 
    Actually load all the configs
This works just enough for the program to continue
*/

static int parse_log_level(const char *s) {
    if (!s) return LOG_INFO;
    if (strcasecmp(s, "error") == 0) return LOG_ERROR;
    if (strcasecmp(s, "warn")  == 0) return LOG_WARN;
    if (strcasecmp(s, "info")  == 0) return LOG_INFO;
    if (strcasecmp(s, "debug") == 0) return LOG_DEBUG;
    fprintf(stderr, "Unknown log_level '%s', defaulting to info\n", s);
    return LOG_INFO;
}

static void parse_fan_curve(cJSON *curve_json, FanCurve *curve) {
    curve->step_count = 0;
    if (!cJSON_IsArray(curve_json)) return;

    cJSON *step = NULL;
    cJSON_ArrayForEach(step, curve_json) {
        if (curve->step_count >= MAX_FAN_STEPS) {
            fprintf(stderr, "fan_curve exceeds MAX_FAN_STEPS, truncating\n");
            break;
        }
        cJSON *temp = cJSON_GetObjectItemCaseSensitive(step, "temp_c");
        cJSON *pct  = cJSON_GetObjectItemCaseSensitive(step, "fan_pct");
        if (!cJSON_IsNumber(temp) || !cJSON_IsNumber(pct)) continue;

        FanCurveStep *dst = &curve->steps[curve->step_count++];
        dst->temp_c  = temp->valueint;
        dst->fan_pct = pct->valueint;
    }
}

static bool parse_profile(cJSON *profile_json, Profile *profile) {
    memset(profile, 0, sizeof(Profile));

    cJSON *name = cJSON_GetObjectItemCaseSensitive(profile_json, "name");
    if (!cJSON_IsString(name) || !name->valuestring) return false;

    strncpy(profile->name, name->valuestring, MAX_PROFILE_NAME - 1);
    profile->name[MAX_PROFILE_NAME - 1] = '\0';

    parse_fan_curve(cJSON_GetObjectItemCaseSensitive(profile_json, "fan_curve"),
                     &profile->fan_curve);
    return true;
}


void config_init(Runtime *rt) {
    memset(&rt->config, 0, sizeof(Config));
    rt->config.current_log_level = LOG_INFO;

    FILE *fp = fopen("../config/config.json", "r");
    if (!fp) { fprintf(stderr, "Could not open config.json for reading\n"); return; }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buffer = malloc(file_size + 1);
    if (!buffer) { fprintf(stderr, "Memory allocation failed for buffer\n"); fclose(fp); return; }

    size_t n = fread(buffer, 1, file_size, fp);
    buffer[n] = '\0';
    fclose(fp);

    cJSON *json = cJSON_Parse(buffer);
    free(buffer);
    if (!json) { fprintf(stderr, "Error parsing JSON: %s\n", cJSON_GetErrorPtr()); return; }

    cJSON *item = NULL;
    cJSON_ArrayForEach(item, json) {
        if (!item->string) continue;

        if (strcmp(item->string, "version") == 0) {
            if (cJSON_IsNumber(item)) rt->config.version = item->valueint;

        } else if (strcmp(item->string, "calls_silence_fan") == 0) {
            if (cJSON_IsBool(item)) rt->config.calls_silence_fan = cJSON_IsTrue(item);

        } else if (strcmp(item->string, "active_profile") == 0) {
            if (cJSON_IsString(item) && item->valuestring) {
                strncpy(rt->config.active_profile, item->valuestring, MAX_PROFILE_NAME - 1);
                rt->config.active_profile[MAX_PROFILE_NAME - 1] = '\0';
            }

        } else if (strcmp(item->string, "log_file") == 0) {
            if (cJSON_IsString(item) && item->valuestring) {
                free(rt->config.log_file);
                rt->config.log_file = strdup(item->valuestring);
            }

        } else if (strcmp(item->string, "log_level") == 0) {
            if (cJSON_IsString(item))
                rt->config.current_log_level = parse_log_level(item->valuestring);

        } else if (strcmp(item->string, "profiles") == 0) {
            if (!cJSON_IsArray(item)) { fprintf(stderr, "'profiles' is not an array\n"); continue; }
            cJSON *pj = NULL;
            cJSON_ArrayForEach(pj, item) {
                if (rt->config.profile_count >= MAX_PROFILES) {
                    fprintf(stderr, "profiles exceeds MAX_PROFILES, truncating\n");
                    break;
                }
                if (parse_profile(pj, &rt->config.profiles[rt->config.profile_count]))
                    rt->config.profile_count++;
            }

        } else {
            fprintf(stderr, "Unknown config key: %s\n", item->string);
        }
    }
    cJSON_Delete(json);

    // resolve active pointer now that profiles[] is fully populated
    rt->config.active = NULL;
    for (size_t i = 0; i < rt->config.profile_count; i++) {
        if (strcmp(rt->config.profiles[i].name, rt->config.active_profile) == 0) {
            rt->config.active = &rt->config.profiles[i];
            break;
        }
    }
    if (!rt->config.active && rt->config.profile_count > 0) {
        fprintf(stderr, "active_profile '%s' not found, defaulting to profiles[0]\n",
                rt->config.active_profile);
        rt->config.active = &rt->config.profiles[0];
    }
}

void config_save(Runtime *rt) {
    //rt->config.version = 1;
}

void config_load(Runtime *rt) {
    //rt->config.version = 1;//stub
}