#ifndef UTILS_H
#define UTILS_H

#include "sensor.h"

// Typedef for array of sensors
typedef Sensor** SensorArray;

// Function pointer for logging
typedef void (*Logger)(const char* message);

// Function to calculate average temperature
double calculate_average(SensorArray sensors, int count);

// Function to log message using function pointer
void log_message(const char* message, Logger logger);

// Function to simulate data with pointers
void simulate_data(int* int_ptr, float* float_ptr, char** str_ptr);

// Function to process array of sensors
void process_sensors(SensorArray sensors, int count, TempProcessor processor);

#endif // UTILS_H