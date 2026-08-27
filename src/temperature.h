#ifndef TEMPERATURE_H
#define TEMPERATURE_H

struct Runtime;

typedef struct Temperature {
    char *cpu_temp_path[10]; // Assuming a maximum of 10 temperature sensors
    char *gpu_temp_path[10]; 
    int cpu_temp;
    int gpu_temp;
} Temperature;

 int temperature_init(struct Runtime *rt);
 int temperature_get_cpu_temp(struct Runtime *rt);
 int temperature_get_gpu_temp(struct Runtime *rt);
 #endif // TEMPERATURE_H