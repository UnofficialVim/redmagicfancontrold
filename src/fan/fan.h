#ifndef FAN_H
#define FAN_H

struct Runtime;

typedef struct Fan {
    int current_speed;
    int target_speed;
    char *device_path; // Path to the fan device (e.g., /sys/class/hwmon/hwmon0/pwm1)
} Fan;

int fan_init(struct Runtime *rt);
int fan_set_speed(struct Runtime *rt, int speed);
int fan_get_speed(struct Runtime *rt);
int fan_update_speed(struct Runtime *rt);

#endif // FAN_H