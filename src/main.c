#include "sensor.h"
#include "utils.h"

// Logger function
void console_logger(const char* message) {
    printf("[LOG] %s\n", message);
}

// Temperature processor function
void temperature_alert(float temp) {
    if (temp > 35.0f) {
        printf("ALERT: High temperature detected: %.2f°C\n", temp);
    } else {
        printf("Temperature normal: %.2f°C\n", temp);
    }
}

int main() {
    // Create sensors
    Sensor* sensor1 = create_sensor(1, "Room Sensor");
    Sensor* sensor2 = create_sensor(2, "Outdoor Sensor");

    if (sensor1 == NULL || sensor2 == NULL) {
        printf("Failed to create sensors\n");
        return 1;
    }

    // Array of sensors
    SensorArray sensors = (SensorArray)malloc(2 * sizeof(Sensor*));
    sensors[0] = sensor1;
    sensors[1] = sensor2;

    // Update temperatures
    update_temperature(sensor1, 25.5f);
    update_temperature(sensor2, 38.0f);

    // Print sensors
    print_sensor(sensor1);
    print_sensor(sensor2);

    // Calculate average
    double avg = calculate_average(sensors, 2);
    printf("Average Temperature: %.2f°C\n", avg);

    // Use function pointers
    log_message("Monitoring started", console_logger);
    process_sensors(sensors, 2, temperature_alert);

    // Simulate data with pointers
    int int_val;
    float float_val;
    char* str_val;
    simulate_data(&int_val, &float_val, &str_val);
    printf("Simulated Int: %d, Float: %.2f, String: %s\n", int_val, float_val, str_val);

    // Free resources
    free(str_val);
    free(sensors);
    free_sensor(sensor1);
    free_sensor(sensor2);

    return 0;
}