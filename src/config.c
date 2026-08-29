#include "config.h"
#include "external/cjson/cJSON.h"
#include "logger.h"
#include "runtime/runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

/*
TODO:
    Actually load all the configs
This works just enough for the program to continue
*/

// fallback config in case the config file is missing or invalid
static Config fallback_config = {
    .version = 1,
    .calls_silence_fan = true,
    .active_profile = "default",
    .profile_count = 1,
    .profiles = {{.name = "default",
                  .fan_curve = {.step_count = 5,
                                .steps = {{.temp_c = 40000, .fan_pct = 1},
                                          {.temp_c = 45000, .fan_pct = 2},
                                          {.temp_c = 50000, .fan_pct = 3},
                                          {.temp_c = 60000, .fan_pct = 4},
                                          {.temp_c = 70000, .fan_pct = 5}}}}},
    .active = &fallback_config
                   .profiles[0], // points to the first profile in profiles[]
    .log_file = "rmfc.log",
    .current_log_level = LOGGER_TRACE};

static int parse_log_level(const char *s) {
  if (!s)
    return LOGGER_INFO;
  if (strcasecmp(s, "error") == 0)
    return LOGGER_ERROR;
  if (strcasecmp(s, "warn") == 0)
    return LOGGER_WARN;
  if (strcasecmp(s, "info") == 0)
    return LOGGER_INFO;
  if (strcasecmp(s, "debug") == 0)
    return LOGGER_DEBUG;
  fprintf(stderr, "Unknown log_level '%s', defaulting to info\n", s);
  return LOGGER_INFO;
}

static void parse_fan_curve(cJSON *curve_json, FanCurve *curve) {
  curve->step_count = 0;
  if (!cJSON_IsArray(curve_json)) {
    logger_error("curve_json is not an array");
    return;
  };

  cJSON *step = NULL;
  cJSON_ArrayForEach(step, curve_json) {
    if (curve->step_count >= MAX_FAN_STEPS) {
      break;
    }
    cJSON *temp = cJSON_GetObjectItemCaseSensitive(step, "temp_c");
    cJSON *pct = cJSON_GetObjectItemCaseSensitive(step, "fan_pct");
    if (!cJSON_IsNumber(temp) || !cJSON_IsNumber(pct))
      continue;

    FanCurveStep *dst = &curve->steps[curve->step_count++];
    dst->temp_c = temp->valueint;
    dst->fan_pct = pct->valueint;
  }
}

static bool parse_profile(cJSON *profile_json, Profile *profile) {
  memset(profile, 0, sizeof(Profile));

  cJSON *name = cJSON_GetObjectItemCaseSensitive(profile_json, "name");
  if (!cJSON_IsString(name) || !name->valuestring)
    return false;

  strncpy(profile->name, name->valuestring, MAX_PROFILE_NAME - 1);
  profile->name[MAX_PROFILE_NAME - 1] = '\0';

  parse_fan_curve(cJSON_GetObjectItemCaseSensitive(profile_json, "fan_curve"),
                  &profile->fan_curve);
  return true;
}

void config_init(Runtime *rt) {
  memset(&rt->config, 0, sizeof(Config));
  rt->config.current_log_level = LOGGER_TRACE; // default to debug until config is loaded

  FILE *fp = fopen("build/config.json", "r");
  if (fp == NULL || ferror(fp)) {
    logger_errno(LOGGER_ERROR, "Failed to open config file, using fallback");
    rt->config = fallback_config;
    logger_set_level(rt->config.current_log_level);
    return;
  }

  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *buffer = malloc(file_size + 1);
  if (!buffer) {
    logger_errno(LOGGER_ERROR, "Memory allocation failed for config buffer, using fallback config");
    rt->config = fallback_config;
    fclose(fp);
    return;
  }

  size_t n = fread(buffer, 1, file_size, fp);
  buffer[n] = '\0';
  fclose(fp);

  cJSON *json = cJSON_Parse(buffer);
  free(buffer);
  if (!json) {
    logger_warn("json NULL or failed to parse, using fallback config");
    rt->config = fallback_config;
    return;
  }

  // assign values from JSON to config struct
  cJSON *item = NULL;
  cJSON_ArrayForEach(item, json) {
    if (!item->string)
      continue;

    if (strcmp(item->string, "version") == 0) {
      if (cJSON_IsNumber(item))
        rt->config.version = item->valueint;

    } else if (strcmp(item->string, "calls_silence_fan") == 0) {
      if (cJSON_IsBool(item))
        rt->config.calls_silence_fan = cJSON_IsTrue(item);

    } else if (strcmp(item->string, "active_profile") == 0) {
      if (cJSON_IsString(item) && item->valuestring) {
        strncpy(rt->config.active_profile, item->valuestring,
                MAX_PROFILE_NAME - 1);
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
      if (!cJSON_IsArray(item)) {
        fprintf(stderr, "'profiles' is not an array\n");
        continue;
      }
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
      logger_errno(LOGGER_WARN, "Unknown config key: %s\n", item->string);
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
    logger_errno(LOGGER_INFO,
            "active_profile '%s' not found, defaulting to profiles[0]\n",
            rt->config.active_profile);
    rt->config.active = &rt->config.profiles[0];
  }
  logger_set_level(rt->config.current_log_level);
}

void config_save(Runtime *rt) {
  // rt->config.version = 1;
}

void config_load(Runtime *rt) {
  // rt->config.version = 1;//stub
}