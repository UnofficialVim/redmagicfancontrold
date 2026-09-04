#include "config.h"
#include "external/cjson/cJSON.h"
#include "logger.h"
#include "runtime/runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

static char log_level_from_string(const char *s) {
  if (!s)
    return LOGGER_TRACE;
  if (strcmp(s, "LOGGER_TRACE") == 0) return LOGGER_TRACE;
  if (strcmp(s, "LOGGER_DEBUG") == 0) return LOGGER_DEBUG;
  if (strcmp(s, "LOGGER_INFO")  == 0) return LOGGER_INFO;
  if (strcmp(s, "LOGGER_WARN")  == 0) return LOGGER_WARN;
  if (strcmp(s, "LOGGER_ERROR") == 0) return LOGGER_ERROR;
  logger_warn("Unknown log_level '%s' in config, keeping default", s);
  return LOGGER_TRACE;
}
// fallback config in case the config file is missing or invalid
static Config fallback_config = {
    .version = 1,
    .calls_silence_fan = true,
    //.log_path = "/var/log/fancontrol.log",
    .log_level = LOGGER_TRACE,
    .active_profile = "default",
    .fan_device_path = "/sys/kernel/fan",
    .thermal_path = "/sys/class/thermal",
    //.socket_path = "/data/tmp/rmfc_socket",
    .profiles = {{.name = "default",
                  .steps = {{.temp_c = 40000, .fan_lvl = 1},
                            {.temp_c = 45000, .fan_lvl = 2},
                            {.temp_c = 50000, .fan_lvl = 3},
                            {.temp_c = 60000, .fan_lvl = 4},
                            {.temp_c = 70000, .fan_lvl = 5}}}},
    .active = &fallback_config.profiles[0]};

static void parse_fan_curve(cJSON *curve_json, Profile *profile) {

  if (!cJSON_IsArray(curve_json)) {
    logger_error("curve_json is not an array");
    return;
  };

  cJSON *step = NULL;
  cJSON_ArrayForEach(step, curve_json) {
    if (profile->steps_count >= MAX_FAN_STEPS) {
      break;
    }
    cJSON *temp = cJSON_GetObjectItemCaseSensitive(step, "temp_c");
    cJSON *lvl = cJSON_GetObjectItemCaseSensitive(step, "fan_lvl");
    if (!cJSON_IsNumber(temp) || !cJSON_IsNumber(lvl))
      continue;

    Step *dst = &profile->steps[profile->steps_count++];
    dst->temp_c = temp->valueint;
    dst->fan_lvl = lvl->valueint;
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
                  profile);
  return true;
}

void config_init(Runtime *rt) {
  memset(&rt->config, 0, sizeof(Config));
  rt->config.log_level = LOGGER_TRACE; // default to debug until config is loaded

  char *running_dir = get_running_dir();
  FILE *fp = fopen(strcat(running_dir, "/config.json"), "r");
  if (fp == NULL || ferror(fp)) {
    logger_errno(LOGGER_ERROR, "Failed to open config file, using fallback");
    rt->config = fallback_config;
    logger_set_level(rt->config.log_level);
    return;
  }

  fseek(fp, 0, SEEK_END);
  long file_size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  char *buffer = malloc(file_size + 1);
  if (!buffer) {
    logger_errno(
        LOGGER_ERROR,
        "Memory allocation failed for config buffer, using fallback config");
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
    logger_debug("Parsing config key: %s", item->string);

    if (strcmp(item->string, "version") == 0) {
      if (cJSON_IsNumber(item)){
        rt->config.version = item->valueint;
        logger_trace("Config version: %d", rt->config.version);
      }

    } else if (strcmp(item->string, "calls_silence_fan") == 0) {
      if (cJSON_IsBool(item)){
        rt->config.calls_silence_fan = cJSON_IsTrue(item);
        logger_trace("Config calls_silence_fan: %d", rt->config.calls_silence_fan);
      }

    } else if (strcmp(item->string, "active_profile") == 0) {
      if (cJSON_IsString(item) && item->valuestring) {
        free(rt->config.active_profile); // no op if still NULL
        rt->config.active_profile = strdup(item->valuestring);
        logger_trace("Config active_profile: %s", rt->config.active_profile);
        if (!rt->config.active_profile)
          logger_errno(LOGGER_ERROR, "strdup failed for active_profile");
      }

    } else if (strcmp(item->string, "log_path") == 0) {
      if (cJSON_IsString(item) && item->valuestring) {
        free(rt->config.log_path);
        rt->config.log_path = strdup(item->valuestring);
        logger_trace("Config log_path: %s", rt->config.log_path);
      }
 
    } else if (strcmp(item->string, "log_level") == 0) {
      if (cJSON_IsString(item) && item->valuestring){
        rt->config.log_level = log_level_from_string(item->valuestring);
        logger_trace("Config log_level: %d", rt->config.log_level);
      }
 
    } else if (strcmp(item->string, "fan_device_path") == 0) {
      if (cJSON_IsString(item) && item->valuestring) {
        free(rt->config.fan_device_path);
        rt->config.fan_device_path = strdup(item->valuestring);
        logger_trace("Config fan_device_path: %s", rt->config.fan_device_path);
      }
 
    } else if (strcmp(item->string, "thermal_path") == 0) {
      if (cJSON_IsString(item) && item->valuestring) {
        free(rt->config.thermal_path);
        rt->config.thermal_path = strdup(item->valuestring);
        logger_trace("Config thermal_path: %s", rt->config.thermal_path);
      }
 
    } else if (strcmp(item->string, "socket_path") == 0) {
      if (cJSON_IsString(item) && item->valuestring) {
        free(rt->config.socket_path);
        rt->config.socket_path = strdup(item->valuestring);
        logger_trace("Config socket_path: %s", rt->config.socket_path);
      }
 
    } else if (strcmp(item->string, "profiles") == 0) {
      if (!cJSON_IsArray(item)) {
        logger_warn("'profiles' is not an array");
        logger_trace("Dump of 'profiles' JSON: %s", cJSON_Print(item));
        continue;
      }
      cJSON *pj = NULL;
      cJSON_ArrayForEach(pj, item) {
        if (rt->config.loaded_profiles_count >= MAX_PROFILES) {
          logger_warn("profiles exceeds MAX_PROFILES, truncating");
          logger_trace("Dump of 'profiles' JSON: %s", cJSON_Print(item));
          break;
        }
        Profile *slot = &rt->config.profiles[rt->config.loaded_profiles_count];
        if (parse_profile(pj, slot)) {
          rt->config.loaded_profiles_count++; 
        } else {
          logger_warn("Failed to parse profile at index %zu, skipping",
                      rt->config.loaded_profiles_count);
        }
      }
 
    } else {
      logger_warn("Unknown config key: %s", item->string);
      logger_trace("Dump of unknown config key JSON: %s", cJSON_Print(item));
    }
  }
 
  cJSON_Delete(json);
 
  // resolve active pointer now that profiles[] is fully populated,
  // regardless of what order keys appeared in the JSON.
  rt->config.active = NULL;
  if (rt->config.active_profile) {
    for (size_t i = 0; i < rt->config.loaded_profiles_count; i++) {
      if (strcmp(rt->config.profiles[i].name, rt->config.active_profile) == 0) {
        rt->config.active = &rt->config.profiles[i];
        logger_debug("Active profile set to '%s'", rt->config.active_profile);
        break;
      }
    }
  }
  if (!rt->config.active && rt->config.loaded_profiles_count > 0) {
    logger_errno(LOGGER_INFO,
                 "active_profile '%s' not found, defaulting to profiles[0]",
                 rt->config.active_profile ? rt->config.active_profile : "(none)");
    rt->config.active = &rt->config.profiles[0];
  }
 
  logger_set_level(rt->config.log_level);
  logger_trace("logger level set to %d", rt->config.log_level);

  logger_info("Config loaded successfully with %zu profiles", rt->config.loaded_profiles_count);
}

void config_save(Runtime *rt) {
  // rt->config.version = 1;
}

void config_load(Runtime *rt) {
}