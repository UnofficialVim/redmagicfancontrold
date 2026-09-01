#include "temperature.h"
#include "runtime/runtime.h"
#include "logger.h"
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

  logger_info("Initialized Temperature Monitoring");
  return 0;
}

char* get_temperature_formatted_string(int temp_milli_celsius) {
  static char buffer[16];
  snprintf(buffer, sizeof(buffer), "%.1f°C", temp_milli_celsius / 1000.0);
  return buffer;
} 

int temperature_get_cpu_temp(Runtime *rt) {
  int total_temp = 0;
  int cpu_zones = 0;
  int i = 0;

  while (rt->temperature.cpu_temp_path[i] != NULL) {
    FILE *fp = fopen(rt->temperature.cpu_temp_path[i], "r");
    if( fp == NULL ) {
      logger_errno(LOGGER_WARN,"Failed to open temperature file %s", rt->temperature.cpu_temp_path[i]);
      break;
    }
    int temp = 0;
    if (fscanf(fp, "%d", &temp) == 1) {
      total_temp += temp;
      cpu_zones++;
    }
    fclose(fp);
    i++;
  }
  if (cpu_zones == 0){
  logger_warn("cpu_zones empty");
  return -1; 
}

  total_temp /= cpu_zones; // Average temperature
  return total_temp;
}