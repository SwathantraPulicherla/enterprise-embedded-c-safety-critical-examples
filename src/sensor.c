#include "sensor.h"

// Function to create a sensor
Sensor* create_sensor(int id, const char* name) {
    Sensor* sensor = (Sensor*)malloc(sizeof(Sensor));
    if (sensor == NULL) {
        return NULL;
    }
    sensor->id = id;
    strncpy(sensor->name, name, sizeof(sensor->name) - 1);
    sensor->name[sizeof(sensor->name) - 1] = '\0';
    sensor->temperature = 0.0f;
    sensor->status = STATUS_OK;
    sensor->data.float_value = 0.0f;  // Initialize union
    return sensor;
}

// Function to update temperature
void update_temperature(Sensor* sensor, float temp) {
    if (sensor == NULL) return;
    sensor->temperature = temp;
    if (temp > 30.0f) {
        sensor->status = STATUS_WARNING;
    } else if (temp > 40.0f) {
        sensor->status = STATUS_ERROR;
    } else {
        sensor->status = STATUS_OK;
    }
    sensor->data.float_value = temp;  // Store in union
}

// Function to print sensor info
void print_sensor(const Sensor* sensor) {
    if (sensor == NULL) return;
    printf("Sensor ID: %d\n", sensor->id);
    printf("Name: %s\n", sensor->name);
    printf("Temperature: %.2f°C\n", sensor->temperature);
    printf("Status: %s\n", sensor->status == STATUS_OK ? "OK" : sensor->status == STATUS_WARNING ? "WARNING" : "ERROR");
    printf("Data (float): %.2f\n", sensor->data.float_value);
}

// Function using function pointer
void process_temperature(float temp, TempProcessor processor) {
    if (processor != NULL) {
        processor(temp);
    }
}

// Function to free sensor
void free_sensor(Sensor* sensor) {
    if (sensor != NULL) {
        free(sensor);
    }
}