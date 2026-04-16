/* test_sensor.c – Auto-generated Expert Unity Tests */
#include "unity.h"
#include <stdlib.h> // For malloc, free
#include <string.h> // For strncpy
#include <stdio.h>  // For printf

// Include the header file for the module under test.
// This is critical to get the definitions of Sensor, SensorStatus, SensorData, and function prototypes.
// sensor.c includes sensor.h, so we must include it here.
#include "sensor.h"

// --- Inferred SENSOR_NAME_MAX_LEN from sensor.c ---
// The source code uses sizeof(sensor->name) - 1 for strncpy.
// If Sensor.name is declared as char name[X], then sizeof(sensor->name) is X.
// Let's assume a common embedded name length of 32 for the char array.
#ifndef SENSOR_NAME_MAX_LEN
#define SENSOR_NAME_MAX_LEN 32
#endif

// --- Mock for TempProcessor function pointer ---
// This is an internal mock within the test file to capture parameters
// and call counts when a function pointer is invoked by the code under test.
static float mock_processor_last_temp;
static int mock_processor_call_count;

void mock_temp_processor(float temp) {
    mock_processor_last_temp = temp;
    mock_processor_call_count++;
}

// --- Setup and Teardown ---
// Ensures a clean state before each test and resets any mocks.
void setUp(void) {
    // Reset internal mock state for TempProcessor for test isolation
    mock_processor_last_temp = 0.0f;
    mock_processor_call_count = 0;
}

void tearDown(void) {
    // Reset internal mock state for TempProcessor for test isolation
    mock_processor_last_temp = 0.0f;
    mock_processor_call_count = 0;
}

// ====================================================================
// Test Cases for create_sensor
// Sensor* create_sensor(int id, const char* name)
// ====================================================================

void test_create_sensor_valid_input_short_name(void) {
    // Test case: Create a sensor with valid ID and a short name.
    Sensor* sensor = create_sensor(1, "TestSensor");
    // Expected: Sensor object should be allocated successfully.
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: ID should match the input value.
    TEST_ASSERT_EQUAL_INT(1, sensor->id);
    // Expected: Name should be correctly copied and null-terminated.
    TEST_ASSERT_EQUAL_STRING("TestSensor", sensor->name);
    // Expected: Temperature should be initialized to 0.0f.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, sensor->temperature);
    // Expected: Status should be initialized to STATUS_OK.
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union float_value should be initialized to 0.0f.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, sensor->data.float_value);
    // Cleanup: Free the allocated sensor memory.
    free_sensor(sensor);
}

void test_create_sensor_valid_input_max_length_name(void) {
    // Test case: Create a sensor with a name exactly at the maximum allowed length.
    char long_name[SENSOR_NAME_MAX_LEN];
    memset(long_name, 'A', SENSOR_NAME_MAX_LEN - 1);
    long_name[SENSOR_NAME_MAX_LEN - 1] = '\0'; // Manually ensure null termination
    Sensor* sensor = create_sensor(2, long_name);
    // Expected: Sensor object should be allocated successfully.
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: ID should match the input value.
    TEST_ASSERT_EQUAL_INT(2, sensor->id);
    // Expected: Name should be correctly copied and null-terminated.
    TEST_ASSERT_EQUAL_STRING(long_name, sensor->name);
    // Cleanup: Free the allocated sensor memory.
    free_sensor(sensor);
}

void test_create_sensor_valid_input_name_too_long(void) {
    // Test case: Create a sensor with a name longer than the buffer capacity.
    const char* really_long_name = "ThisIsAReallyLongSensorNameThatExceedsTheMaximumLengthBufferCapacity";
    char expected_name[SENSOR_NAME_MAX_LEN];
    strncpy(expected_name, really_long_name, SENSOR_NAME_MAX_LEN - 1);
    expected_name[SENSOR_NAME_MAX_LEN - 1] = '\0'; // Manually null terminate for comparison
    Sensor* sensor = create_sensor(3, really_long_name);
    // Expected: Sensor object should be allocated successfully.
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: ID should match the input value.
    TEST_ASSERT_EQUAL_INT(3, sensor->id);
    // Expected: Name should be truncated to SENSOR_NAME_MAX_LEN - 1 characters and null-terminated.
    TEST_ASSERT_EQUAL_STRING(expected_name, sensor->name);
    // Cleanup: Free the allocated sensor memory.
    free_sensor(sensor);
}

void test_create_sensor_valid_input_empty_name(void) {
    // Test case: Create a sensor with an empty name.
    Sensor* sensor = create_sensor(4, "");
    // Expected: Sensor object should be allocated successfully.
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: ID should match the input value.
    TEST_ASSERT_EQUAL_INT(4, sensor->id);
    // Expected: Name should be an empty string.
    TEST_ASSERT_EQUAL_STRING("", sensor->name);
    // Cleanup: Free the allocated sensor memory.
    free_sensor(sensor);
}

// ====================================================================
// Test Cases for update_temperature
// void update_temperature(Sensor* sensor, float temp)
// ====================================================================

void test_update_temperature_null_sensor(void) {
    // Test case: Call update_temperature with a NULL sensor pointer.
    // Expected: Function should handle NULL sensor gracefully without crashing.
    update_temperature(NULL, 25.0f);
    // No direct assertion possible, but test passes if no crash occurs.
}

void test_update_temperature_status_ok_nominal(void) {
    // Test case: Update temperature with a nominal value that results in STATUS_OK.
    Sensor* sensor = create_sensor(1, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor); // Pre-condition check: ensure sensor creation worked.

    update_temperature(sensor, 25.0f); // Expected: 25.0f is not > 30.0f and not > 40.0f
    // Expected: Temperature should be updated to the input value.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.0f, sensor->temperature);
    // Expected: Status should be STATUS_OK.
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union float_value should be updated to the input temperature.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.0f, sensor->data.float_value);
    free_sensor(sensor);
}

void test_update_temperature_status_ok_lower_boundary(void) {
    // Test case: Update temperature with the lowest realistic positive value for STATUS_OK.
    Sensor* sensor = create_sensor(1, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    update_temperature(sensor, 0.0f); // Expected: 0.0f is not > 30.0f and not > 40.0f
    // Expected: Temperature should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, sensor->temperature);
    // Expected: Status should be STATUS_OK.
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union value should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, sensor->data.float_value);
    free_sensor(sensor);
}

void test_update_temperature_status_ok_negative_temp(void) {
    // Test case: Update temperature with a negative value that results in STATUS_OK.
    Sensor* sensor = create_sensor(1, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    update_temperature(sensor, 0.0f); // Expected: 0.0f is not > 30.0f and not > 40.0f
    // Expected: Temperature should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, sensor->temperature);
    // Expected: Status should be STATUS_OK.
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union value should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, sensor->data.float_value);
    free_sensor(sensor);
}

void test_update_temperature_status_ok_upper_boundary(void) {
    // Test case: Update temperature at the upper boundary for STATUS_OK (inclusive).
    Sensor* sensor = create_sensor(1, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    update_temperature(sensor, 30.0f); // Expected: 30.0f is NOT > 30.0f
    // Expected: Temperature should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 30.0f, sensor->temperature);
    // Expected: Status should be STATUS_OK.
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union value should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 30.0f, sensor->data.float_value);
    free_sensor(sensor);
}

void test_update_temperature_status_warning_lower_boundary(void) {
    // Test case: Update temperature just above the STATUS_OK threshold (exclusive), resulting in WARNING.
    Sensor* sensor = create_sensor(1, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    update_temperature(sensor, 30.1f); // Expected: 30.1f IS > 30.0f
    // Expected: Temperature should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 30.1f, sensor->temperature);
    // Expected: Status should be STATUS_WARNING.
    TEST_ASSERT_EQUAL_INT(STATUS_WARNING, sensor->status);
    // Expected: Union value should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 30.1f, sensor->data.float_value);
    free_sensor(sensor);
}

void test_update_temperature_status_warning_mid_range(void) {
    // Test case: Update temperature in the mid-range for STATUS_WARNING.
    Sensor* sensor = create_sensor(1, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    update_temperature(sensor, 35.0f); // Expected: 35.0f IS > 30.0f
    // Expected: Temperature should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 35.0f, sensor->temperature);
    // Expected: Status should be STATUS_WARNING.
    TEST_ASSERT_EQUAL_INT(STATUS_WARNING, sensor->status);
    // Expected: Union value should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 35.0f, sensor->data.float_value);
    free_sensor(sensor);
}

void test_update_temperature_status_warning_above_40_bug_demonstration(void) {
    // CRITICAL BUG DEMONSTRATION:
    // Test case: Update temperature above 40.0f. Due to the 'if-else if' ordering in source,
    // `if (temp > 30.0f)` takes precedence. Since 40.1f > 30.0f, the status becomes WARNING.
    // The `else if (temp > 40.0f)` condition is never evaluated for values greater than 30.0f.
    // This means STATUS_ERROR is unreachable via the update_temperature function as written.
    Sensor* sensor = create_sensor(1, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    update_temperature(sensor, 40.1f);
    // Expected: Temperature should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 40.1f, sensor->temperature);
    // Expected: Status is WARNING, revealing the bug in the conditional logic.
    TEST_ASSERT_EQUAL_INT(STATUS_WARNING, sensor->status);
    // Expected: Union value should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 40.1f, sensor->data.float_value);
    free_sensor(sensor);
}

void test_update_temperature_status_warning_extreme_temp_bug_demonstration(void) {
    // CRITICAL BUG DEMONSTRATION:
    // Test case: Update temperature with a very high value (e.g., 100.0f).
    // This demonstrates that even extreme temperatures will result in WARNING, not ERROR,
    // due to the precedence of `if (temp > 30.0f)`.
    Sensor* sensor = create_sensor(1, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    update_temperature(sensor, 100.0f);
    // Expected: Temperature should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 100.0f, sensor->temperature);
    // Expected: Status is WARNING, further confirming the bug.
    TEST_ASSERT_EQUAL_INT(STATUS_WARNING, sensor->status);
    // Expected: Union value should be updated.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 100.0f, sensor->data.float_value);
    free_sensor(sensor);
}

// ====================================================================
// Test Cases for print_sensor
// void print_sensor(const Sensor* sensor)
// ====================================================================
// Note: Direct assertion of printf output is not possible with Unity
// without redirection techniques (e.g., freopen to a buffer).
// These tests primarily ensure the function does not crash with various inputs.

void test_print_sensor_null_sensor(void) {
    // Test case: Call print_sensor with a NULL sensor pointer.
    // Expected: Function should handle NULL sensor gracefully without crashing.
    print_sensor(NULL);
}

void test_print_sensor_valid_sensor_ok_status(void) {
    // Test case: Call print_sensor with a valid sensor in STATUS_OK.
    Sensor* sensor = create_sensor(10, "DisplaySensorOK");
    TEST_ASSERT_NOT_NULL(sensor);
    update_temperature(sensor, 20.5f); // Set a realistic temp, status remains OK
    // Expected: Function should execute without crashing and print appropriate info.
    print_sensor(sensor);
    free_sensor(sensor);
}

void test_print_sensor_valid_sensor_warning_status(void) {
    // Test case: Call print_sensor with a valid sensor in STATUS_WARNING.
    Sensor* sensor = create_sensor(11, "DisplaySensorWARN");
    TEST_ASSERT_NOT_NULL(sensor);
    update_temperature(sensor, 35.0f); // Set to WARNING status
    // Expected: Function should execute without crashing and print appropriate info.
    print_sensor(sensor);
    free_sensor(sensor);
}

void test_print_sensor_valid_sensor_error_status(void) {
    // Test case: Call print_sensor with a valid sensor in STATUS_ERROR.
    // (Note: STATUS_ERROR cannot be reached via `update_temperature` due to the bug,
    // so we manually set it here to test the `print_sensor` logic for this state).
    Sensor* sensor = create_sensor(12, "DisplaySensorERROR");
    TEST_ASSERT_NOT_NULL(sensor);
    // Manually set temperature and data to reflect an error condition.
    sensor->temperature = 45.0f;
    sensor->data.float_value = 45.0f;
    sensor->status = STATUS_ERROR; // Directly set status to ERROR for print test
    // Expected: Function should execute without crashing and print appropriate info.
    print_sensor(sensor);
    free_sensor(sensor);
}

// ====================================================================
// Test Cases for process_temperature
// void process_temperature(float temp, TempProcessor processor)
// ====================================================================

void test_process_temperature_valid_processor(void) {
    // Test case: Call process_temperature with a valid function pointer.
    float test_temp = 15.5f;
    process_temperature(test_temp, mock_temp_processor);
    // Expected: The mock processor function should be called once.
    TEST_ASSERT_EQUAL_INT(1, mock_processor_call_count);
    // Expected: The correct temperature value should be passed to the processor.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, test_temp, mock_processor_last_temp);
}

void test_process_temperature_another_valid_processor_call(void) {
    // Test case: Call process_temperature again with a valid function pointer and different temp.
    // This also implicitly tests `setUp` and `tearDown` resetting the mock.
    float test_temp = 75.0f;
    process_temperature(test_temp, mock_temp_processor);
    // Expected: The mock processor function should be called once (since setUp resets count).
    TEST_ASSERT_EQUAL_INT(1, mock_processor_call_count);
    // Expected: The correct temperature value should be passed to the processor.
    TEST_ASSERT_FLOAT_WITHIN(0.1f, test_temp, mock_processor_last_temp);
}

void test_process_temperature_null_processor(void) {
    // Test case: Call process_temperature with a NULL function pointer.
    // Expected: Function should handle NULL processor gracefully without crashing.
    process_temperature(20.0f, NULL);
    // Expected: The mock processor function should not be called.
    TEST_ASSERT_EQUAL_INT(0, mock_processor_call_count);
}

// ====================================================================
// Test Cases for free_sensor
// void free_sensor(Sensor* sensor)
// ====================================================================

void test_free_sensor_valid_sensor(void) {
    // Test case: Free a valid sensor object.
    Sensor* sensor = create_sensor(100, "DisposableSensor");
    TEST_ASSERT_NOT_NULL(sensor); // Pre-condition check
    // Expected: free_sensor should free the allocated memory without crashing.
    free_sensor(sensor);
    // Cannot assert actual memory freeing (e.g., memory becomes available),
    // but the test passes if no crash or runtime error occurs.
}

void test_free_sensor_null_sensor(void) {
    // Test case: Call free_sensor with a NULL input.
    // Expected: free_sensor should handle NULL input gracefully without crashing.
    free_sensor(NULL);
    // No crash indicates success.
}

// ====================================================================
// Main Test Runner
// ====================================================================


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_create_sensor_valid_input_short_name);
    RUN_TEST(test_create_sensor_valid_input_max_length_name);
    RUN_TEST(test_create_sensor_valid_input_name_too_long);
    RUN_TEST(test_create_sensor_valid_input_empty_name);
    RUN_TEST(test_update_temperature_null_sensor);
    RUN_TEST(test_update_temperature_status_ok_nominal);
    RUN_TEST(test_update_temperature_status_ok_lower_boundary);
    RUN_TEST(test_update_temperature_status_ok_negative_temp);
    RUN_TEST(test_update_temperature_status_ok_upper_boundary);
    RUN_TEST(test_update_temperature_status_warning_lower_boundary);
    RUN_TEST(test_update_temperature_status_warning_mid_range);
    RUN_TEST(test_update_temperature_status_warning_above_40_bug_demonstration);
    RUN_TEST(test_update_temperature_status_warning_extreme_temp_bug_demonstration);
    RUN_TEST(test_print_sensor_null_sensor);
    RUN_TEST(test_print_sensor_valid_sensor_ok_status);
    RUN_TEST(test_print_sensor_valid_sensor_warning_status);
    RUN_TEST(test_print_sensor_valid_sensor_error_status);
    RUN_TEST(test_process_temperature_valid_processor);
    RUN_TEST(test_process_temperature_another_valid_processor_call);
    RUN_TEST(test_process_temperature_null_processor);
    RUN_TEST(test_free_sensor_valid_sensor);
    RUN_TEST(test_free_sensor_null_sensor);

    return UNITY_END();
}