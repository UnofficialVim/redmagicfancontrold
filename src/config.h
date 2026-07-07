#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>

struct Runtime;

#define MAX_FAN_STEPS 16
#define MAX_PROFILE_NAME 32
#define MAX_PROFILES 8

typedef struct
{
    int temp_c;
    int fan_pct;
} FanCurveStep;

typedef struct
{
    FanCurveStep steps[MAX_FAN_STEPS];
    size_t step_count;
} FanCurve;

typedef struct
{
    char name[MAX_PROFILE_NAME];
    FanCurve fan_curve;
} Profile;


typedef enum {
        LOG_ERROR = 0,
        LOG_WARN,
        LOG_INFO,
        LOG_DEBUG,
    LOG_MAX   
} LogLevel;

static const char *log_level_str[] = {
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG"
};

typedef struct Config
{
    int version;
    bool calls_silence_fan;
    char active_profile[MAX_PROFILE_NAME];
    size_t profile_count;
    Profile profiles[MAX_PROFILES];
    Profile *active;
    char *log_file;
    LogLevel current_log_level; 
} Config;

void config_init(struct Runtime *rt);
void config_save(struct Runtime *rt);
void config_load(struct Runtime *rt);

#endif