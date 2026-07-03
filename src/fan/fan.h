#ifndef FAN_H
#define FAN_H

typedef struct Runtime Runtime;

typedef struct Fan {
    int current_speed;
    int target_speed;
    char *device_path; // Path to the fan device (e.g., /sys/class/hwmon/hwmon0/pwm1)
} Fan;

int fan_init(Runtime *rt);
int fan_set_speed(Runtime *rt, int speed);
int fan_get_speed(Runtime *rt);
int fan_update_speed(Runtime *rt);

#endif // FAN_H