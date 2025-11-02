#ifndef SENSOR_H
#define SENSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enum for sensor status
typedef enum {
    STATUS_OK,
    STATUS_WARNING,
    STATUS_ERROR
} SensorStatus;

// Union for different data representations
typedef union {
    int int_value;
    float float_value;
    double double_value;
    char char_value;
    void* ptr_value;
} DataUnion;

// Struct for sensor data
typedef struct {
    char name[50];
    float temperature;
    int id;
    SensorStatus status;
    DataUnion data;
} Sensor;

// Struct with nested structs and pointers
typedef struct {
    Sensor* sensors;  // Pointer to array of sensors
    int sensor_count;
    double average_temp;
    char* log_message;  // Pointer to string
} MonitoringSystem;

// Function pointer type for temperature processing
typedef void (*TempProcessor)(float temp);

// Function declarations
Sensor* create_sensor(int id, const char* name);
void update_temperature(Sensor* sensor, float temp);
void print_sensor(const Sensor* sensor);
void process_temperature(float temp, TempProcessor processor);
void free_sensor(Sensor* sensor);

#endif // SENSOR_H