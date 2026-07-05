#include <stdlib.h>
#include "runtime/runtime.h"

int fan_init(Runtime *rt) {
	//find the fan hardware path
	// /sys/kernel/fan/ 
	rt->fan.fan_device_path = "/sys/kernel/fan";//stub, but is correct
	if(!rt->fan.fan_device_path == NULL){
		logger_write(rt, 2, "Found fan device path");
	}else{
		logger_write(rt, 0, "No fan path found");
		return -1;
	}

	logger_write(rt, 2, "Initialized Fan Control");
	return 0;
}

int fan_set_speed(Runtime *rt, int speed) {
	rt->fan.target_speed = speed;
	return 0;
}

int fan_get_speed(Runtime *rt) {
	return rt->fan.current_speed;
}

int fan_update_speed(Runtime *rt) {
	rt->fan.current_speed = rt->fan.target_speed;
	return 0;
}
