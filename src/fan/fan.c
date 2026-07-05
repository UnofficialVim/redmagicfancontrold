#include <stdlib.h>
#include "../runtime/runtime.h"

int fan_init(Runtime *rt) {
	if (!rt) return -1;
	rt->fan.current_speed = 0;
	rt->fan.target_speed = 0;
	rt->fan.device_path = NULL;

	logger_write(rt, 2, "Initialized Fan Control");
	return 0;
}

int fan_set_speed(Runtime *rt, int speed) {
	if (!rt) return -1;
	rt->fan.target_speed = speed;
	return 0;
}

int fan_get_speed(Runtime *rt) {
	if (!rt) return -1;
	return rt->fan.current_speed;
}

int fan_update_speed(Runtime *rt) {
	if (!rt) return -1;
	rt->fan.current_speed = rt->fan.target_speed;
	return 0;
}
