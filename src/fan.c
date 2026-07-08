#include <stdlib.h>
#include "runtime/runtime.h"
#include "logging.h"
#include "stdio.h"

int fan_init(Runtime *rt)
{
	// find the fan hardware path
	//  /sys/kernel/fan/
	rt->fan.fan_device_path = "/sys/kernel/fan"; // stub, but is correct
	if (!rt->fan.fan_device_path == NULL)
	{
		logger_write(rt, 2, "Found fan device path");
	}
	else
	{
		logger_write(rt, 0, "No fan path found");
		return -1;
	}

	logger_write(rt, 2, "Initialized Fan Control");
	return 0;
}

char *append_fan_function(Runtime *rt, char *function)
{
    size_t len = strlen(rt->fan.fan_device_path) + strlen(function) + 2;
    char *new_path = malloc(len);
    if (new_path == NULL)
    {
        logger_write(rt, 1, "Failed to append char fan function to char device path");
        return NULL;
    }

    snprintf(new_path, len, "%s%s", rt->fan.fan_device_path, function);

    logger_write(rt, 3, "Appending fan function to device path: %s", new_path);
    return new_path;
}

int fan_set_speed(Runtime *rt, int speed)
{
	if (speed < 0 || speed > 5)
	{
		logger_write(rt, 1, "Invalid fan speed, use 0-5");
		return -1;
	}
	FILE *fp = fopen(append_fan_function(rt, fan_function_str[FAN_SPEED_LEVEL]), "w");

	if (fp == NULL)
	{
		logger_write(rt, 1, "fp NULL in function fan_set_speed");
		fclose(fp);
		return -1;
	}
	else
	{
		if (fprintf(fp, "%d\n", speed) < 0)
		{
			logger_write(rt, 1, "Failed to write fan speed");
			fclose(fp);
			return -1;
		}
		fclose(fp);
	}
	fclose(fp);
	return 0;
}

int fan_get_speed(Runtime *rt)
{
	FILE *fp = fopen(append_fan_function(rt, fan_function_str[FAN_SPEED_LEVEL]), "r");
	if (fp == NULL)
	{
		logger_write(rt, 1, "fp NULL in function fan_get_speed");
		return -1;
	}
	int speed;
    if (fscanf(fp, "%d", &speed) != 1)
    {
        logger_write(rt, 1, "Failed to read fan speed");
        fclose(fp);
        return -1;
    }
	fclose(fp);
	return 0;
}

int fan_update_speed(Runtime *rt)
{
	rt->fan.current_speed = rt->fan.target_speed;
	return 0;
}
