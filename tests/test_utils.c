/* test_utils.c – Auto-generated Expert Unity Tests */
#include "unity.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h> // For malloc, free
#include <string.h> // For memset, strcpy

// Include the header from the source file, as it is present in src/utils.c and necessary for type definitions.
#include "utils.h"

// --- External Stub Declarations ---
// ONLY 'process_temperature' is listed as an external dependency to stub.
void process_temperature(float temperature, TempProcessor processor_func);

// --- Stub Control Structures and Mocks ---

// Mock for process_temperature (external dependency)
typedef struct {
    bool was_called;
    uint32_t call_count;
    float captured_temperatures[10]; // Capture multiple calls
    TempProcessor captured_processor_funcs[10]; // Capture multiple calls
    uint32_t capacity; // Max capacity of capture arrays
} stub_process_temperature_t;
static stub_process_temperature_t stub_process_temperature = {0};

// Mock for Logger function pointer (internal to tests to verify log_message behavior)
typedef struct {
    bool was_called;
    uint32_t call_count;
    char captured_message_buffer[256]; // Buffer to copy the last message
} mock_logger_t;
static mock_logger_t mock_logger_control = {0};

// Mock for TempProcessor function pointer (internal to tests to verify process_sensors behavior)
typedef struct {
    bool was_called;
    uint32_t call_count;
    float captured_temperatures[10];
    void* captured_contexts[10];
    uint32_t capacity;
} mock_temp_processor_t;
static mock_temp_processor_t mock_temp_processor_control = {0};

// --- Mock Implementations ---

void process_temperature(float temperature, TempProcessor processor_func) {
    stub_process_temperature.was_called = true;
    if (stub_process_temperature.call_count < stub_process_temperature.capacity) {
        stub_process_temperature.captured_temperatures[stub_process_temperature.call_count] = temperature;
        stub_process_temperature.captured_processor_funcs[stub_process_temperature.call_count] = processor_func;
    }
    stub_process_temperature.call_count++;
}

void mock_logger(const char* message) {
    mock_logger_control.was_called = true;
    mock_logger_control.call_count++;
    strncpy(mock_logger_control.captured_message_buffer, message, sizeof(mock_logger_control.captured_message_buffer) - 1);
    mock_logger_control.captured_message_buffer[sizeof(mock_logger_control.captured_message_buffer) - 1] = '\0';
}

void mock_temp_processor(float temperature, void* context) {
    mock_temp_processor_control.was_called = true;
    if (mock_temp_processor_control.call_count < mock_temp_processor_control.capacity) {
        mock_temp_processor_control.captured_temperatures[mock_temp_processor_control.call_count] = temperature;
        mock_temp_processor_control.captured_contexts[mock_temp_processor_control.call_count] = context;
    }
    mock_temp_processor_control.call_count++;
}

// --- Helper Functions for Tests ---

// Allocate and initialize a sensor array
SensorArray create_sensor_array(int count, float* temperatures) {
    if (count <= 0) return NULL;
    SensorArray sensors = (SensorArray)malloc(count * sizeof(Sensor*));
    TEST_ASSERT_NOT_NULL_MESSAGE(sensors, "Failed to allocate SensorArray");
    for (int i = 0; i < count; i++) {
        sensors[i] = (Sensor*)malloc(sizeof(Sensor));
        TEST_ASSERT_NOT_NULL_MESSAGE(sensors[i], "Failed to allocate Sensor");
        if (temperatures) {
            sensors[i]->temperature = temperatures[i];
        } else {
            sensors[i]->temperature = 0.0f; // Default
        }
    }
    return sensors;
}

// Free a sensor array
void destroy_sensor_array(SensorArray sensors, int count) {
    if (sensors == NULL) return;
    for (int i = 0; i < count; i++) {
        free(sensors[i]);
    }
    free(sensors);
}

// --- Set Up and Tear Down ---

void setUp(void) {
    // Reset all stub and mock control structures
    memset(&stub_process_temperature, 0, sizeof(stub_process_temperature));
    stub_process_temperature.capacity = sizeof(stub_process_temperature.captured_temperatures) / sizeof(stub_process_temperature.captured_temperatures[0]);

    memset(&mock_logger_control, 0, sizeof(mock_logger_control));

    memset(&mock_temp_processor_control, 0, sizeof(mock_temp_processor_control));
    mock_temp_processor_control.capacity = sizeof(mock_temp_processor_control.captured_temperatures) / sizeof(mock_temp_processor_control.captured_temperatures[0]);
}

void tearDown(void) {
    // Reset all stub and mock control structures for complete test isolation
    memset(&stub_process_temperature, 0, sizeof(stub_process_temperature));
    stub_process_temperature.capacity = sizeof(stub_process_temperature.captured_temperatures) / sizeof(stub_process_temperature.captured_temperatures[0]);

    memset(&mock_logger_control, 0, sizeof(mock_logger_control));

    memset(&mock_temp_processor_control, 0, sizeof(mock_temp_processor_control));
    mock_temp_processor_control.capacity = sizeof(mock_temp_processor_control.captured_temperatures) / sizeof(mock_temp_processor_control.captured_temperatures[0]);
}

// --- Include the source file directly for testing internal functions ---
// This standard Unity pattern exposes all functions, including static ones, for testing.
// It assumes utils.c itself includes utils.h (with include guards) and links correctly with process_temperature stub.

// --- Test Cases for calculate_average ---

void test_calculate_average_null_sensors(void) {
    // Expected: Returns 0.0 when sensors array is NULL, as per the function's contract.
    double result = calculate_average(NULL, 5);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0, result);
}

void test_calculate_average_zero_count(void) {
    float temps[] = {25.0f, 30.0f};
    SensorArray sensors = create_sensor_array(2, temps);
    // Expected: Returns 0.0 when count is 0, as per the function's contract.
    double result = calculate_average(sensors, 0);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0, result);
    destroy_sensor_array(sensors, 2);
}

void test_calculate_average_negative_count(void) {
    float temps[] = {25.0f, 30.0f};
    SensorArray sensors = create_sensor_array(2, temps);
    // Expected: Returns 0.0 when count is negative, as per the function's contract.
    double result = calculate_average(sensors, 0.0f);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0, result);
    destroy_sensor_array(sensors, 2);
}

void test_calculate_average_single_sensor(void) {
    float temps[] = {25.5f};
    SensorArray sensors = create_sensor_array(1, temps);
    // Expected: Average of a single sensor is its temperature.
    double result = calculate_average(sensors, 1);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 25.5, result);
    destroy_sensor_array(sensors, 1);
}

void test_calculate_average_multiple_sensors_positive(void) {
    float temps[] = {20.0f, 30.0f, 40.0f};
    SensorArray sensors = create_sensor_array(3, temps);
    // Expected: Correct average for positive temperatures, (20+30+40)/3 = 30.0.
    double result = calculate_average(sensors, 3);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 30.0, result);
    destroy_sensor_array(sensors, 3);
}

void test_calculate_average_multiple_sensors_mixed(void) {
    float temps[] = {0.0f, 0.0f, 50.0f}; // Realistic mixed temperatures
    SensorArray sensors = create_sensor_array(3, temps);
    // Expected: Correct average for mixed (negative, zero, positive) temperatures, (0.0f+0+50)/3 = 13.333...
    double result = calculate_average(sensors, 3);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 13.333f, result);
    destroy_sensor_array(sensors, 3);
}

void test_calculate_average_multiple_sensors_max_min_edge(void) {
    float temps[] = {0.0f, 125.0f}; // Realistic temperature range edges
    SensorArray sensors = create_sensor_array(2, temps);
    // Expected: Correct average for edge case temperatures, (0.0f+125)/2 = 42.5.
    double result = calculate_average(sensors, 2);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 42.5, result);
    destroy_sensor_array(sensors, 2);
}


// --- Test Cases for log_message ---

void test_log_message_valid_logger(void) {
    const char* test_message = "System initialized.";
    // Expected: mock_logger is called once with the correct message.
    log_message(test_message, mock_logger);
    TEST_ASSERT_TRUE(mock_logger_control.was_called);
    TEST_ASSERT_EQUAL_UINT32(1, mock_logger_control.call_count);
    TEST_ASSERT_EQUAL_STRING(test_message, mock_logger_control.captured_message_buffer);
}

void test_log_message_null_logger(void) {
    const char* test_message = "Error occurred.";
    // Expected: mock_logger is NOT called when logger is NULL, as per the function's internal check.
    log_message(test_message, NULL);
    TEST_ASSERT_FALSE(mock_logger_control.was_called);
    TEST_ASSERT_EQUAL_UINT32(0, mock_logger_control.call_count);
}

void test_log_message_empty_message(void) {
    const char* test_message = "";
    // Expected: mock_logger is called with an empty string.
    log_message(test_message, mock_logger);
    TEST_ASSERT_TRUE(mock_logger_control.was_called);
    TEST_ASSERT_EQUAL_UINT32(1, mock_logger_control.call_count);
    TEST_ASSERT_EQUAL_STRING(test_message, mock_logger_control.captured_message_buffer);
}

void test_log_message_long_message(void) {
    char long_message[300];
    memset(long_message, 'A', sizeof(long_message) - 1);
    long_message[sizeof(long_message) - 1] = '\0'; // Ensure null termination
    // Expected: mock_logger captures the message, potentially truncated if longer than its buffer.
    log_message(long_message, mock_logger);
    TEST_ASSERT_TRUE(mock_logger_control.was_called);
    TEST_ASSERT_EQUAL_UINT32(1, mock_logger_control.call_count);
    // Check that the captured message matches the beginning of the long message up to buffer size.
    TEST_ASSERT_EQUAL_STRING_LEN(long_message, mock_logger_control.captured_message_buffer, sizeof(mock_logger_control.captured_message_buffer) - 1);
}


// --- Test Cases for simulate_data ---

void test_simulate_data_all_valid_pointers(void) {
    int int_val = 0;
    float float_val = 0.0f;
    char* str_val = NULL; // Will be allocated by UUT
    // Expected: All pointers are updated with specific values (42, 3.14f, "Simulated Data"), memory is allocated for string.
    simulate_data(&int_val, &float_val, &str_val);
    TEST_ASSERT_EQUAL_INT(42, int_val);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14f, float_val);
    TEST_ASSERT_NOT_NULL(str_val);
    TEST_ASSERT_EQUAL_STRING("Simulated Data", str_val);
    free(str_val); // Clean up allocated memory
}

void test_simulate_data_null_int_ptr(void) {
    int int_val = 100; // Should remain unchanged
    float float_val = 0.0f;
    char* str_val = NULL;
    // Expected: float and string pointers are updated, int_ptr remains unchanged due to NULL input.
    simulate_data(NULL, &float_val, &str_val);
    TEST_ASSERT_EQUAL_INT(100, int_val); // Should be untouched
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14f, float_val);
    TEST_ASSERT_NOT_NULL(str_val);
    TEST_ASSERT_EQUAL_STRING("Simulated Data", str_val);
    free(str_val);
}

void test_simulate_data_null_float_ptr(void) {
    int int_val = 0;
    float float_val = 100.0f; // Should remain unchanged
    char* str_val = NULL;
    // Expected: int and string pointers are updated, float_ptr remains unchanged due to NULL input.
    simulate_data(&int_val, NULL, &str_val);
    TEST_ASSERT_EQUAL_INT(42, int_val);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 100.0f, float_val); // Should be untouched
    TEST_ASSERT_NOT_NULL(str_val);
    TEST_ASSERT_EQUAL_STRING("Simulated Data", str_val);
    free(str_val);
}

void test_simulate_data_null_str_ptr(void) {
    int int_val = 0;
    float float_val = 0.0f;
    char* str_val = (char*)1; // Set to a non-NULL value to ensure it's not written/freed if passed as NULL
    // Expected: int and float pointers are updated, str_ptr remains unchanged (no malloc/strcpy for NULL str_ptr).
    simulate_data(&int_val, &float_val, NULL);
    TEST_ASSERT_EQUAL_INT(42, int_val);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14f, float_val);
    TEST_ASSERT_EQUAL_PTR((char*)1, str_val); // Should be untouched
    // No free needed for str_val as it was not allocated by UUT
}

void test_simulate_data_all_null_pointers(void) {
    int int_val = 0;
    float float_val = 0.0f;
    char* str_val = NULL;
    // Expected: No crashes, no changes to values (as all pointers passed were NULL).
    simulate_data(NULL, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(0, int_val); // Should remain untouched
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, float_val); // Should remain untouched
    TEST_ASSERT_NULL(str_val); // Should remain NULL
}


// --- Test Cases for process_sensors ---

void test_process_sensors_single_sensor_valid(void) {
    float temps[] = {25.0f};
    SensorArray sensors = create_sensor_array(1, temps);
    // Expected: process_temperature stub is called once with sensor's temperature (25.0f) and mock_temp_processor.
    process_sensors(sensors, 1, mock_temp_processor);
    TEST_ASSERT_TRUE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(1, stub_process_temperature.call_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 25.0f, stub_process_temperature.captured_temperatures[0]);
    TEST_ASSERT_EQUAL_PTR(mock_temp_processor, stub_process_temperature.captured_processor_funcs[0]);
    destroy_sensor_array(sensors, 1);
}

void test_process_sensors_multiple_sensors_valid(void) {
    float temps[] = {10.0f, 20.0f, 30.0f};
    SensorArray sensors = create_sensor_array(3, temps);
    // Expected: process_temperature stub is called for each sensor (3 times) with correct temperatures and processor.
    process_sensors(sensors, 3, mock_temp_processor);
    TEST_ASSERT_TRUE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(3, stub_process_temperature.call_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, stub_process_temperature.captured_temperatures[0]);
    TEST_ASSERT_EQUAL_PTR(mock_temp_processor, stub_process_temperature.captured_processor_funcs[0]);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.0f, stub_process_temperature.captured_temperatures[1]);
    TEST_ASSERT_EQUAL_PTR(mock_temp_processor, stub_process_temperature.captured_processor_funcs[1]);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 30.0f, stub_process_temperature.captured_temperatures[2]);
    TEST_ASSERT_EQUAL_PTR(mock_temp_processor, stub_process_temperature.captured_processor_funcs[2]);
    destroy_sensor_array(sensors, 3);
}

void test_process_sensors_zero_count(void) {
    float temps[] = {10.0f};
    SensorArray sensors = create_sensor_array(1, temps);
    // Expected: process_temperature stub is NOT called when count is 0, as the loop won't execute.
    process_sensors(sensors, 0, mock_temp_processor);
    TEST_ASSERT_FALSE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(0, stub_process_temperature.call_count);
    destroy_sensor_array(sensors, 1);
}

void test_process_sensors_null_sensors_zero_count(void) {
    // Expected: process_temperature stub is NOT called when sensors is NULL and count is 0 (loop won't run).
    process_sensors(NULL, 0, mock_temp_processor);
    TEST_ASSERT_FALSE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(0, stub_process_temperature.call_count);
}

void test_process_sensors_null_processor(void) {
    float temps[] = {25.0f};
    SensorArray sensors = create_sensor_array(1, temps);
    // Expected: process_temperature stub is called with sensor's temperature and a NULL processor.
    process_sensors(sensors, 1, NULL);
    TEST_ASSERT_TRUE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(1, stub_process_temperature.call_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 25.0f, stub_process_temperature.captured_temperatures[0]);
    TEST_ASSERT_NULL(stub_process_temperature.captured_processor_funcs[0]); // Processor was NULL
    destroy_sensor_array(sensors, 1);
}

// --- Main test runner ---



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_calculate_average_null_sensors);
    RUN_TEST(test_calculate_average_zero_count);
    RUN_TEST(test_calculate_average_negative_count);
    RUN_TEST(test_calculate_average_single_sensor);
    RUN_TEST(test_calculate_average_multiple_sensors_positive);
    RUN_TEST(test_calculate_average_multiple_sensors_mixed);
    RUN_TEST(test_calculate_average_multiple_sensors_max_min_edge);
    RUN_TEST(test_log_message_valid_logger);
    RUN_TEST(test_log_message_null_logger);
    RUN_TEST(test_log_message_empty_message);
    RUN_TEST(test_log_message_long_message);
    RUN_TEST(test_simulate_data_all_valid_pointers);
    RUN_TEST(test_simulate_data_null_int_ptr);
    RUN_TEST(test_simulate_data_null_float_ptr);
    RUN_TEST(test_simulate_data_null_str_ptr);
    RUN_TEST(test_simulate_data_all_null_pointers);
    RUN_TEST(test_process_sensors_single_sensor_valid);
    RUN_TEST(test_process_sensors_multiple_sensors_valid);
    RUN_TEST(test_process_sensors_zero_count);
    RUN_TEST(test_process_sensors_null_sensors_zero_count);
    RUN_TEST(test_process_sensors_null_processor);

    return UNITY_END();
}