#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>
typedef struct Runtime Runtime;

#define MAX_FAN_STEPS 16
#define MAX_PROFILE_NAME 32
#define MAX_PROFILES 8



typedef struct {
    int temp_c;
    int fan_pct;
} FanCurveStep;

typedef struct {
    FanCurveStep steps[MAX_FAN_STEPS];
    size_t step_count;
} FanCurve;

typedef struct {
    char name[MAX_PROFILE_NAME];
    FanCurve fan_curve;
} Profile;

typedef struct Config {
    //contains the config.json data
    int version;
    bool call_silence;
    char active_profile[MAX_PROFILE_NAME]; 
    size_t profile_count;
    Profile profiles[MAX_PROFILES];

    Profile *active;
}Config;

void config_init(Runtime *rt);
void config_save(Runtime *rt);
void config_load(Runtime *rt);

#endif 