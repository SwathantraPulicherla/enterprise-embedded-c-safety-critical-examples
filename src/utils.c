#include "utils.h"

// Calculate average temperature
double calculate_average(SensorArray sensors, int count) {
    if (sensors == NULL || count <= 0) return 0.0;
    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += sensors[i]->temperature;
    }
    return sum / count;
}

// Log message using function pointer
void log_message(const char* message, Logger logger) {
    if (logger != NULL) {
        logger(message);
    }
}

// Simulate data with pointers
void simulate_data(int* int_ptr, float* float_ptr, char** str_ptr) {
    if (int_ptr != NULL) *int_ptr = 42;
    if (float_ptr != NULL) *float_ptr = 3.14f;
    if (str_ptr != NULL) {
        *str_ptr = (char*)malloc(20 * sizeof(char));
        strcpy(*str_ptr, "Simulated Data");
    }
}

// Process array of sensors
void process_sensors(SensorArray sensors, int count, TempProcessor processor) {
    for (int i = 0; i < count; i++) {
        process_temperature(sensors[i]->temperature, processor);
    }
}