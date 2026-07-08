#ifndef FAN_H
#define FAN_H

struct Runtime;

typedef enum {
    FAN_ENABLE,
    FAN_SPEED_LEVEL,
    LED_BRIGHTNESS,
    LED_FADE_MS,
    LED_OFF_MS,
    FAN_SPEED_COUNT,
    FAN_SPEED_PWM,
    LED_ENABLE,
    LED_ID,
    LED_ON_MS
} fan_functions;

static const char *fan_function_str[] = {

    "fan_enable",       //Owned by system
    "fan_speed_level",  //Owned by system
    "led_brightness",   //Owned by root
    "led_fade_ms",      //Owned by root
    "led_off_ms",       //Owned by root
    "fan_speed_count",  //Owned by system
    "fan_speed_pwm",    //Owned by root
    "led_enable",       //Owned by root
    "led_id",           //Owned by root
    "led_on_ms"         //Owned by root
};

typedef struct Fan {
    int current_speed;
    int target_speed;
    char *fan_device_path; // Path to the fan device (e.g., /sys/kernel/fan/)
} Fan;

int fan_init(struct Runtime *rt);
int fan_set_speed(struct Runtime *rt, int speed);
int fan_get_speed(struct Runtime *rt);
int fan_update_speed(struct Runtime *rt);
char *append_fan_function(struct Runtime *rt, char *function);

#endif // FAN_H