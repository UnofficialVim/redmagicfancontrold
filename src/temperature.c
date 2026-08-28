#include "temperature.h"
#include "runtime/runtime.h"
#include "logging.h"
#include <stddef.h>
#include <stdio.h>

int temperature_init(Runtime *rt) {
  // Initialize temperature paths for CPU using cpuss paths
  rt->temperature.cpu_temp_path[0] =
      "/sys/class/thermal/thermal_zone10/temp"; // Example path for CPU temp
  rt->temperature.cpu_temp_path[1] =
      "/sys/class/thermal/thermal_zone11/temp"; // Example path for CPU temp
  rt->temperature.cpu_temp_path[2] =
      "/sys/class/thermal/thermal_zone12/temp"; // Example path for CPU temp
  rt->temperature.cpu_temp_path[3] =
      "/sys/class/thermal/thermal_zone13/temp"; // Example path for CPU temp

  logger_write(rt, 2, "Initialized Temperature Monitoring");
  return 0;
}

int temperature_get_cpu_temp(Runtime *rt) {
  int total_temp = 0;
  int cpu_zones = 0;
  int i = 0;

  while (rt->temperature.cpu_temp_path[i] != NULL) {
    FILE *fp = fopen(rt->temperature.cpu_temp_path[i], "r");
    int temp = 0;
    if (fscanf(fp, "%d", &temp) == 1) {
      total_temp += temp;
      cpu_zones++;
    }
    fclose(fp);
    i++;
  }
  total_temp /= cpu_zones; // Average temperature
  return total_temp;
}