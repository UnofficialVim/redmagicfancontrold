#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>
#include <stddef.h>

struct Runtime;

#define MAX_FAN_STEPS 16
#define MAX_PROFILE_NAME 32
#define MAX_PROFILES 8

typedef struct {
    int temp_c;
    int fan_lvl;
} Step;

typedef struct {
    Step steps[MAX_FAN_STEPS];
    char name[MAX_PROFILE_NAME];
    int steps_count;
} Profile;


typedef struct Config
{
    int version;
    bool calls_silence_fan;
    char *log_path;
    char *active_profile;
    char *fan_device_path;
    char *thermal_path;
    char *socket_path;
    int log_level;
    Profile profiles[MAX_PROFILES];//are we holding all 8 profiles in memory at once?
    Profile *active; //maybe change to active_profile for clarity
    int loaded_profiles_count;
} Config;

void config_init(struct Runtime *rt);
void config_save(struct Runtime *rt);
void config_load(struct Runtime *rt);

#endif