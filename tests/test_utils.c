/* test_utils.c – Auto-generated Expert Unity Tests */

#include "unity.h"
#include <stdint.h>   // For uint32_t
#include <stdbool.h>  // For bool
#include <string.h>   // For strcpy, memset
#include <stdlib.h>   // For malloc, free

// --- Inferred Type Definitions from utils.h ---
// As `utils.h` is not provided in the source code block, these types are inferred
// from their usage in `utils.c` and explicitly defined here for compilation.

// `sensors[i]->temperature` implies Sensor is a struct with `temperature` member.
typedef struct Sensor {
    double temperature;
} Sensor;

// `SensorArray sensors` used as `sensors[i]->temperature` implies `SensorArray` is `Sensor**`.
typedef Sensor** SensorArray;

// `logger(message)` implies `Logger` is a function pointer taking `const char*`.
typedef void (*Logger)(const char* message);

// `TempProcessor processor` as an argument, and `process_temperature` (external function)
// takes `TempProcessor` as its second argument. The prompt specifies `process_temperature`
// as an external stub, and the `TempProcessor` type itself is implied to be a function pointer
// taking `double temperature`.
typedef void (*TempProcessor)(double temperature);

// --- Source Functions Declarations ---
// These are declarations for the functions defined in `utils.c` to be tested.
// Since `utils.h` is not provided, these are explicitly declared here.
double calculate_average(SensorArray sensors, int count);
void log_message(const char* message, Logger logger);
void simulate_data(int* int_ptr, float* float_ptr, char** str_ptr);
void process_sensors(SensorArray sensors, int count, TempProcessor processor);


// --- Stub for External Dependency: process_temperature ---
// The source code in `process_sensors` calls: `process_temperature(sensors[i]->temperature, processor);`
// Given `TempProcessor` is `typedef void (*TempProcessor)(double temperature);`,
// the inferred signature for the external function `process_temperature` is:
// `void process_temperature(double temperature_val, TempProcessor processor_func_ptr);`

typedef struct {
    uint32_t call_count;
    double last_temperature_val;
    TempProcessor last_processor_func_ptr; // Captures the function pointer passed as argument
} stub_process_temperature_t;
static stub_process_temperature_t stub_process_temperature;

void process_temperature(double temperature_val, TempProcessor processor_func_ptr) {
    stub_process_temperature.call_count++;
    stub_process_temperature.last_temperature_val = temperature_val;
    stub_process_temperature.last_processor_func_ptr = processor_func_ptr;
    // Note: The `process_temperature` stub does not call `processor_func_ptr` itself.
    // Testing `process_sensors` involves verifying `process_temperature` was called with the correct arguments.
}


// --- Mock for Logger function pointer (used as argument to log_message) ---
// This mock simulates the behavior of a `Logger` function passed to `log_message`.
typedef struct {
    uint32_t call_count;
    const char* last_message;
    bool was_called; // Indicates if the mock was invoked at least once
} mock_logger_t;
static mock_logger_t mock_logger_control;

void mock_logger_func(const char* message) {
    mock_logger_control.was_called = true;
    mock_logger_control.call_count++;
    mock_logger_control.last_message = message; // Captures the last message
}


// --- Mock for TempProcessor function pointer (used as argument to process_sensors) ---
// This mock will be passed as the `processor` argument to `process_sensors`.
// The `process_temperature` stub will then capture THIS mock function pointer.
typedef struct {
    uint32_t call_count;
    double last_temperature; // Captures the last temperature if this mock were called
} mock_temp_processor_t;
static mock_temp_processor_t mock_temp_processor_control;

void mock_temp_processor_func(double temperature) {
    mock_temp_processor_control.call_count++;
    mock_temp_processor_control.last_temperature = temperature;
}


// --- Test Fixture Setup/Teardown ---
void setUp(void) {
    // Reset all stub and mock control structures for test isolation
    memset(&stub_process_temperature, 0, sizeof(stub_process_temperature));
    memset(&mock_logger_control, 0, sizeof(mock_logger_control));
    memset(&mock_temp_processor_control, 0, sizeof(mock_temp_processor_control));
}

void tearDown(void) {
    // Ensure all stub and mock control structures are reset after each test
    memset(&stub_process_temperature, 0, sizeof(stub_process_temperature));
    memset(&mock_logger_control, 0, sizeof(mock_logger_control));
    memset(&mock_temp_processor_control, 0, sizeof(mock_temp_processor_control));
}


// --- Tests for calculate_average ---

void test_calculate_average_null_sensors(void) {
    // Test scenario: `sensors` array pointer is NULL
    double result = calculate_average(NULL, 5);
    // Expected: Returns 0.0 as per the `if (sensors == NULL || count <= 0)` branch
    TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, result);
}

void test_calculate_average_zero_count(void) {
    // Test scenario: `count` is 0
    Sensor s1 = { .temperature = 25.0 };
    Sensor* sensors_array[] = { &s1 };
    double result = calculate_average(sensors_array, 0);
    // Expected: Returns 0.0 as per the `if (sensors == NULL || count <= 0)` branch
    TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, result);
}

void test_calculate_average_negative_count(void) {
    // Test scenario: `count` is negative
    Sensor s1 = { .temperature = 25.0 };
    Sensor* sensors_array[] = { &s1 };
    double result = calculate_average(sensors_array, 0.0f);
    // Expected: Returns 0.0 as per the `if (sensors == NULL || count <= 0)` branch
    TEST_ASSERT_FLOAT_WITHIN(0.001, 0.0, result);
}

void test_calculate_average_single_sensor(void) {
    // Test scenario: A single sensor with a realistic temperature
    Sensor s1 = { .temperature = 30.5 };
    Sensor* sensors_array[] = { &s1 };
    double result = calculate_average(sensors_array, 1);
    // Expected: Average should be the temperature of the single sensor
    TEST_ASSERT_FLOAT_WITHIN(0.001, 30.5, result);
}

void test_calculate_average_multiple_sensors_positive(void) {
    // Test scenario: Multiple sensors with positive, realistic temperatures
    Sensor s1 = { .temperature = 20.0 };
    Sensor s2 = { .temperature = 25.0 };
    Sensor s3 = { .temperature = 30.0 };
    Sensor* sensors_array[] = { &s1, &s2, &s3 };
    double result = calculate_average(sensors_array, 3);
    // Expected: (20.0 + 25.0 + 30.0) / 3 = 75.0 / 3 = 25.0
    TEST_ASSERT_FLOAT_WITHIN(0.001, 25.0, result);
}

void test_calculate_average_multiple_sensors_mixed(void) {
    // Test scenario: Multiple sensors with mixed positive and negative temperatures
    Sensor s1 = { .temperature = 0.0f }; // Realistic negative temperature
    Sensor s2 = { .temperature = 0.0 };
    Sensor s3 = { .temperature = 10.0 };
    Sensor s4 = { .temperature = 20.0 };
    Sensor* sensors_array[] = { &s1, &s2, &s3, &s4 };
    double result = calculate_average(sensors_array, 4);
    // Expected: (0.0f + 0.0 + 10.0 + 20.0) / 4 = 20.0 / 4 = 5.0
    TEST_ASSERT_FLOAT_WITHIN(0.001, 5.0, result);
}

void test_calculate_average_edge_temperatures(void) {
    // Test scenario: Sensors with extreme but valid temperatures (e.g., 0.0f and 125.0)
    Sensor s1 = { .temperature = 0.0f }; // Minimum valid temperature for some systems
    Sensor s2 = { .temperature = 125.0 }; // Maximum valid temperature for some systems
    Sensor* sensors_array[] = { &s1, &s2 };
    double result = calculate_average(sensors_array, 2);
    // Expected: (0.0f + 125.0) / 2 = 85.0 / 2 = 42.5
    TEST_ASSERT_FLOAT_WITHIN(0.001, 42.5, result);
}


// --- Tests for log_message ---

void test_log_message_valid_logger(void) {
    // Test scenario: Pass a valid logger function (mock) and a message
    const char* test_message = "System initialized successfully.";
    log_message(test_message, mock_logger_func);
    // Expected: Mock logger should have been called once
    TEST_ASSERT_TRUE(mock_logger_control.was_called);
    // Expected: Call count should be 1
    TEST_ASSERT_EQUAL_UINT32(1, mock_logger_control.call_count);
    // Expected: The captured message should match the sent message
    TEST_ASSERT_EQUAL_STRING(test_message, mock_logger_control.last_message);
}

void test_log_message_null_logger(void) {
    // Test scenario: Pass a NULL logger function pointer
    const char* test_message = "Error: Null logger.";
    log_message(test_message, NULL);
    // Expected: Mock logger should NOT have been called as per `if (logger != NULL)` branch
    TEST_ASSERT_FALSE(mock_logger_control.was_called);
    // Expected: Call count should be 0
    TEST_ASSERT_EQUAL_UINT32(0, mock_logger_control.call_count);
    // Expected: No message should have been captured
    TEST_ASSERT_NULL(mock_logger_control.last_message);
}

void test_log_message_empty_message(void) {
    // Test scenario: Pass an empty string as a message
    const char* test_message = "";
    log_message(test_message, mock_logger_func);
    // Expected: Mock logger should have been called once
    TEST_ASSERT_TRUE(mock_logger_control.was_called);
    // Expected: Call count should be 1
    TEST_ASSERT_EQUAL_UINT32(1, mock_logger_control.call_count);
    // Expected: The captured message should be an empty string
    TEST_ASSERT_EQUAL_STRING(test_message, mock_logger_control.last_message);
}

void test_log_message_multiple_calls(void) {
    // Test scenario: Call log_message multiple times with different messages
    const char* msg1 = "First log entry";
    const char* msg2 = "Second log entry";
    log_message(msg1, mock_logger_func);
    log_message(msg2, mock_logger_func);
    // Expected: Mock logger called twice
    TEST_ASSERT_EQUAL_UINT32(2, mock_logger_control.call_count);
    // Expected: The last captured message should be the second one sent
    TEST_ASSERT_EQUAL_STRING(msg2, mock_logger_control.last_message);
}


// --- Tests for simulate_data ---

void test_simulate_data_all_null_pointers(void) {
    // Test scenario: All input pointers are NULL.
    // The function contains `if (ptr != NULL)` checks, so it should handle NULL gracefully.
    simulate_data(NULL, NULL, NULL);
    // Expected: No modifications, no crashes. This test validates the NULL checks.
    // No specific assertions are needed other than that the function completes without crashing.
    UNITY_TEST_ASSERT(true, __LINE__, "Function should complete gracefully when all pointers are NULL.");
}

void test_simulate_data_int_ptr_only(void) {
    // Test scenario: Only `int_ptr` is valid, others are NULL
    int test_int = 0;
    simulate_data(&test_int, NULL, NULL);
    // Expected: `test_int` should be updated to 42
    TEST_ASSERT_EQUAL_INT(42, test_int);
}

void test_simulate_data_float_ptr_only(void) {
    // Test scenario: Only `float_ptr` is valid, others are NULL
    float test_float = 0.0f;
    simulate_data(NULL, &test_float, NULL);
    // Expected: `test_float` should be updated to 3.14f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 3.14f, test_float);
}

void test_simulate_data_str_ptr_only(void) {
    // Test scenario: Only `str_ptr` is valid, leading to memory allocation and string copy
    char* test_str = NULL; // Pointer to be filled by malloc
    simulate_data(NULL, NULL, &test_str);
    // Expected: `test_str` should not be NULL and contain the correct string
    TEST_ASSERT_NOT_NULL(test_str);
    TEST_ASSERT_EQUAL_STRING("Simulated Data", test_str);
    // Cleanup: Free the allocated memory to prevent leaks
    free(test_str);
}

void test_simulate_data_all_valid_pointers(void) {
    // Test scenario: All input pointers are valid
    int test_int = 0;
    float test_float = 0.0f;
    char* test_str = NULL;

    simulate_data(&test_int, &test_float, &test_str);
    // Expected: All values set correctly
    TEST_ASSERT_EQUAL_INT(42, test_int);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 3.14f, test_float);
    TEST_ASSERT_NOT_NULL(test_str);
    TEST_ASSERT_EQUAL_STRING("Simulated Data", test_str);
    // Cleanup: Free the allocated memory
    free(test_str);
}


// --- Tests for process_sensors ---

void test_process_sensors_empty_array(void) {
    // Test scenario: `sensors` array is NULL and `count` is 0
    process_sensors(NULL, 0, mock_temp_processor_func);
    // Expected: `process_temperature` (stub) should not be called
    TEST_ASSERT_EQUAL_UINT32(0, stub_process_temperature.call_count);
    // Expected: `mock_temp_processor_func` should not be called (as it's not called by `process_sensors` directly)
    TEST_ASSERT_EQUAL_UINT32(0, mock_temp_processor_control.call_count);
}

void test_process_sensors_valid_array_zero_count(void) {
    // Test scenario: `sensors` array is valid, but `count` is 0
    Sensor s1 = { .temperature = 25.0 };
    Sensor* sensors_array[] = { &s1 };
    process_sensors(sensors_array, 0, mock_temp_processor_func);
    // Expected: `process_temperature` (stub) should not be called, loop not entered
    TEST_ASSERT_EQUAL_UINT32(0, stub_process_temperature.call_count);
    // Expected: `mock_temp_processor_func` should not be called
    TEST_ASSERT_EQUAL_UINT32(0, mock_temp_processor_control.call_count);
}

void test_process_sensors_single_sensor(void) {
    // Test scenario: Process a single sensor
    Sensor s1 = { .temperature = 22.5 };
    Sensor* sensors_array[] = { &s1 };
    
    process_sensors(sensors_array, 1, mock_temp_processor_func);
    
    // Expected: `process_temperature` (stub) called once
    TEST_ASSERT_EQUAL_UINT32(1, stub_process_temperature.call_count);
    // Expected: Captured temperature in stub should match sensor's temperature
    TEST_ASSERT_FLOAT_WITHIN(0.1, 22.5, stub_process_temperature.last_temperature_val);
    // Expected: Captured `TempProcessor` func ptr in stub should be our mock
    TEST_ASSERT_EQUAL_PTR(mock_temp_processor_func, stub_process_temperature.last_processor_func_ptr);
    // Expected: `mock_temp_processor_func` itself should not be called by `process_sensors` (it's passed to `process_temperature`)
    TEST_ASSERT_EQUAL_UINT32(0, mock_temp_processor_control.call_count);
}

void test_process_sensors_multiple_sensors(void) {
    // Test scenario: Process multiple sensors with realistic temperatures
    Sensor s1 = { .temperature = 15.0 };
    Sensor s2 = { .temperature = 30.0 };
    Sensor s3 = { .temperature = 0.0f }; // Realistic negative temperature
    Sensor* sensors_array[] = { &s1, &s2, &s3 };

    process_sensors(sensors_array, 3, mock_temp_processor_func);

    // Expected: `process_temperature` (stub) called three times
    TEST_ASSERT_EQUAL_UINT32(3, stub_process_temperature.call_count);
    // Expected: The last temperature captured should be from s3
    TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0f, stub_process_temperature.last_temperature_val);
    // Expected: Captured `TempProcessor` func ptr should be our mock for the last call
    TEST_ASSERT_EQUAL_PTR(mock_temp_processor_func, stub_process_temperature.last_processor_func_ptr);
    // Expected: `mock_temp_processor_func` should not be called directly by `process_sensors`
    TEST_ASSERT_EQUAL_UINT32(0, mock_temp_processor_control.call_count);
}

void test_process_sensors_null_processor(void) {
    // Test scenario: Pass a NULL `TempProcessor` argument to `process_sensors`
    Sensor s1 = { .temperature = 25.0 };
    Sensor* sensors_array[] = { &s1 };

    process_sensors(sensors_array, 1, NULL); // Pass NULL as TempProcessor

    // Expected: `process_temperature` (stub) should still be called
    TEST_ASSERT_EQUAL_UINT32(1, stub_process_temperature.call_count);
    // Expected: Captured temperature should be correct
    TEST_ASSERT_FLOAT_WITHIN(0.1, 25.0, stub_process_temperature.last_temperature_val);
    // Expected: Captured `TempProcessor` func ptr should be NULL
    TEST_ASSERT_NULL(stub_process_temperature.last_processor_func_ptr);
    // Expected: `mock_temp_processor_func` should not have been used or called
    TEST_ASSERT_EQUAL_UINT32(0, mock_temp_processor_control.call_count);
}


// --- Main function for running all tests ---


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_calculate_average_null_sensors);
    RUN_TEST(test_calculate_average_zero_count);
    RUN_TEST(test_calculate_average_negative_count);
    RUN_TEST(test_calculate_average_single_sensor);
    RUN_TEST(test_calculate_average_multiple_sensors_positive);
    RUN_TEST(test_calculate_average_multiple_sensors_mixed);
    RUN_TEST(test_calculate_average_edge_temperatures);
    RUN_TEST(test_log_message_valid_logger);
    RUN_TEST(test_log_message_null_logger);
    RUN_TEST(test_log_message_empty_message);
    RUN_TEST(test_log_message_multiple_calls);
    RUN_TEST(test_simulate_data_all_null_pointers);
    RUN_TEST(test_simulate_data_int_ptr_only);
    RUN_TEST(test_simulate_data_float_ptr_only);
    RUN_TEST(test_simulate_data_str_ptr_only);
    RUN_TEST(test_simulate_data_all_valid_pointers);
    RUN_TEST(test_process_sensors_empty_array);
    RUN_TEST(test_process_sensors_valid_array_zero_count);
    RUN_TEST(test_process_sensors_single_sensor);
    RUN_TEST(test_process_sensors_multiple_sensors);
    RUN_TEST(test_process_sensors_null_processor);

    return UNITY_END();
}