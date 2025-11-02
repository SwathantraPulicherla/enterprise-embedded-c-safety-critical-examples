/* test_sensor.c – Auto-generated Expert Unity Tests */
#include "unity.h"
#include <stdlib.h> // For malloc, free
#include <string.h> // For strncpy, memset
#include <stdio.h>  // For printf, used by source
#include <stdbool.h> // For bool types if needed

// --- Re-declaring types from sensor.h for test compilation ---
// SENSOR_NAME_MAX_LEN is not provided in source. Let's define a realistic value.
#define SENSOR_NAME_MAX_LEN 32

typedef enum {
    STATUS_OK,
    STATUS_WARNING,
    STATUS_ERROR
} SensorStatus;

typedef union {
    float float_value;
    // Assuming 'int_value' might exist in a real sensor.h, but not used in src.
    int int_value;
} SensorData;

typedef struct {
    int id;
    char name[SENSOR_NAME_MAX_LEN];
    float temperature;
    SensorStatus status;
    SensorData data;
} Sensor;

typedef void (*TempProcessor)(float);
// --- End of re-declarations ---

// --- External function declarations from sensor.c (to be tested) ---
// Note: We're testing the functions directly, no need to declare them as extern
// if compiled with the source file or if the source file is included.
// For unit testing, typically the source file's functions are available.
// However, the prompt implies "extern" if a separate test file.
// Let's assume these functions are available to the linker.
// For the purpose of clarity within this single test file, we'll avoid redefining
// internal functions and rely on them being available.

// The prompt does not list any external functions to stub.
// Malloc, free, strncpy, printf are standard library functions and not
// subject to stubbing per the prompt's "EXTERNAL FUNCTIONS TO STUB" list.

// Dummy function for TempProcessor to capture parameters
static float captured_temp_for_processor = 0.0f;
static bool processor_was_called = false;
static void dummy_temp_processor(float temp) {
    captured_temp_for_processor = temp;
    processor_was_called = true;
}

void setUp(void) {
    // Reset captured state for dummy processor
    captured_temp_for_processor = 0.0f;
    processor_was_called = false;
}

void tearDown(void) {
    // No global resources to clean up in this specific module beyond individual tests.
    // Memory allocated in tests is freed within the test.
}

// --- Test cases for create_sensor ---
void test_create_sensor_success_valid_inputs(void) {
    int id = 101;
    const char* name = "TempSensor_Room";
    Sensor* sensor = create_sensor(id, name);

    // Expected: Sensor pointer is not NULL upon successful allocation
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: ID matches input
    TEST_ASSERT_EQUAL_INT(id, sensor->id);
    // Expected: Name matches input
    TEST_ASSERT_EQUAL_STRING(name, sensor->name);
    // Expected: Initial temperature is 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sensor->temperature);
    // Expected: Initial status is STATUS_OK
    TEST_ASSERT_EQUAL(STATUS_OK, sensor->status);
    // Expected: Initial union float_value is 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sensor->data.float_value);

    free_sensor(sensor); // Clean up allocated memory
}

void test_create_sensor_name_exact_max_len(void) {
    int id = 102;
    // Create a name exactly SENSOR_NAME_MAX_LEN - 1 characters long
    char name[SENSOR_NAME_MAX_LEN];
    memset(name, 'A', SENSOR_NAME_MAX_LEN - 1);
    name[SENSOR_NAME_MAX_LEN - 1] = '\0'; // Null-terminate

    Sensor* sensor = create_sensor(id, name);

    // Expected: Sensor pointer is not NULL
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: Name matches the exact max length string
    TEST_ASSERT_EQUAL_STRING(name, sensor->name);
    // Expected: Name is null-terminated at the end
    TEST_ASSERT_EQUAL_CHAR('\0', sensor->name[SENSOR_NAME_MAX_LEN - 1]);

    free_sensor(sensor);
}

void test_create_sensor_name_longer_than_max_len(void) {
    int id = 103;
    // Create a name longer than SENSOR_NAME_MAX_LEN - 1 characters
    char long_name[SENSOR_NAME_MAX_LEN + 5]; // +5 to make it clearly longer
    memset(long_name, 'B', SENSOR_NAME_MAX_LEN + 4);
    long_name[SENSOR_NAME_MAX_LEN + 4] = '\0';

    Sensor* sensor = create_sensor(id, long_name);

    // Expected: Sensor pointer is not NULL
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: Name is truncated to SENSOR_NAME_MAX_LEN - 1 chars and null-terminated
    char expected_name[SENSOR_NAME_MAX_LEN];
    strncpy(expected_name, long_name, SENSOR_NAME_MAX_LEN - 1);
    expected_name[SENSOR_NAME_MAX_LEN - 1] = '\0';
    TEST_ASSERT_EQUAL_STRING(expected_name, sensor->name);
    // Expected: Null terminator is at the last valid position
    TEST_ASSERT_EQUAL_CHAR('\0', sensor->name[SENSOR_NAME_MAX_LEN - 1]);

    free_sensor(sensor);
}

void test_create_sensor_with_empty_name(void) {
    int id = 104;
    const char* name = "";
    Sensor* sensor = create_sensor(id, name);

    // Expected: Sensor pointer is not NULL
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: Name is an empty string
    TEST_ASSERT_EQUAL_STRING("", sensor->name);

    free_sensor(sensor);
}

// NOTE: Cannot test malloc failure directly as malloc is not an external stub per instructions.

// --- Test cases for update_temperature ---
void test_update_temperature_null_sensor_no_crash(void) {
    // Expected: Calling with NULL sensor should not crash
    update_temperature(NULL, 25.0f);
    // No assertions possible beyond "no crash"
}

void test_update_temperature_ok_status(void) {
    Sensor* sensor = create_sensor(201, "Sensor_OK");
    TEST_ASSERT_NOT_NULL(sensor);

    // Test a temperature well within OK range
    update_temperature(sensor, 25.0f);
    // Expected: Temperature is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, sensor->temperature);
    // Expected: Status is OK (25.0f <= 30.0f)
    TEST_ASSERT_EQUAL(STATUS_OK, sensor->status);
    // Expected: Data union float_value is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, sensor->data.float_value);

    // Test negative temperature
    update_temperature(sensor, 0.0f);
    // Expected: Temperature is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, sensor->temperature);
    // Expected: Status is OK (0.0f <= 30.0f)
    TEST_ASSERT_EQUAL(STATUS_OK, sensor->status);
    // Expected: Data union float_value is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, sensor->data.float_value);

    // Test OK boundary (exactly 30.0f)
    update_temperature(sensor, 30.0f);
    // Expected: Temperature is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 30.0f, sensor->temperature);
    // Expected: Status is OK (30.0f is NOT > 30.0f)
    TEST_ASSERT_EQUAL(STATUS_OK, sensor->status);
    // Expected: Data union float_value is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 30.0f, sensor->data.float_value);

    free_sensor(sensor);
}

void test_update_temperature_warning_status(void) {
    Sensor* sensor = create_sensor(202, "Sensor_Warning");
    TEST_ASSERT_NOT_NULL(sensor);

    // Test WARNING boundary (just above 30.0f)
    update_temperature(sensor, 30.1f);
    // Expected: Temperature is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 30.1f, sensor->temperature);
    // Expected: Status is WARNING (30.1f > 30.0f)
    TEST_ASSERT_EQUAL(STATUS_WARNING, sensor->status);
    // Expected: Data union float_value is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 30.1f, sensor->data.float_value);

    // Test a temperature well within WARNING range
    update_temperature(sensor, 35.0f);
    // Expected: Temperature is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 35.0f, sensor->temperature);
    // Expected: Status is WARNING (35.0f > 30.0f)
    TEST_ASSERT_EQUAL(STATUS_WARNING, sensor->status);
    // Expected: Data union float_value is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 35.0f, sensor->data.float_value);

    free_sensor(sensor);
}

void test_update_temperature_error_logic_flaw_actual_behavior(void) {
    Sensor* sensor = create_sensor(203, "Sensor_Error_Logic");
    TEST_ASSERT_NOT_NULL(sensor);

    // Test a temperature that should logically be ERROR (e.g., 40.1f)
    // However, due to the if/else if structure:
    // if (temp > 30.0f) { STATUS_WARNING; }
    // else if (temp > 40.0f) { STATUS_ERROR; }
    // A temp like 40.1f will satisfy 'temp > 30.0f' first, setting STATUS_WARNING.
    // The 'else if (temp > 40.0f)' branch is never reached for temps > 30.0f.
    // This test verifies the current, flawed behavior of the source code.
    update_temperature(sensor, 40.1f);
    // Expected: Temperature is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 40.1f, sensor->temperature);
    // Expected: Status is WARNING due to current source logic, NOT ERROR.
    // This highlights a potential bug in the original source code where STATUS_ERROR
    // branch is effectively unreachable if temp > 30.0f.
    TEST_MESSAGE("NOTE: For temp > 40.0f, status is WARNING due to current source logic (if/else if order).");
    TEST_ASSERT_EQUAL(STATUS_WARNING, sensor->status);
    // Expected: Data union float_value is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 40.1f, sensor->data.float_value);

    // Test an even higher temperature, still resulting in WARNING
    update_temperature(sensor, 100.0f);
    // Expected: Status is WARNING (still > 30.0f)
    TEST_ASSERT_EQUAL(STATUS_WARNING, sensor->status);
    // Expected: Data union float_value is updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 100.0f, sensor->data.float_value);

    free_sensor(sensor);
}

// --- Test cases for print_sensor ---
void test_print_sensor_null_sensor_no_crash(void) {
    // Expected: Calling with NULL sensor should not crash
    print_sensor(NULL);
    // No assertions possible beyond "no crash" and no direct printf output capture.
    TEST_MESSAGE("NOTE: print_sensor output not captured/asserted in unit test.");
}

void test_print_sensor_valid_sensor_ok_status(void) {
    Sensor* sensor = create_sensor(301, "PrintSensor_OK");
    TEST_ASSERT_NOT_NULL(sensor);
    update_temperature(sensor, 22.5f); // Set to OK status

    // Expected: Function call completes without crashing
    print_sensor(sensor);
    TEST_MESSAGE("NOTE: print_sensor output not captured/asserted in unit test. Visual inspection only for this function.");

    free_sensor(sensor);
}

void test_print_sensor_valid_sensor_warning_status(void) {
    Sensor* sensor = create_sensor(302, "PrintSensor_Warning");
    TEST_ASSERT_NOT_NULL(sensor);
    update_temperature(sensor, 33.3f); // Set to WARNING status

    // Expected: Function call completes without crashing
    print_sensor(sensor);
    TEST_MESSAGE("NOTE: print_sensor output not captured/asserted in unit test. Visual inspection only for this function.");

    free_sensor(sensor);
}

void test_print_sensor_valid_sensor_error_status(void) {
    Sensor* sensor = create_sensor(303, "PrintSensor_Error");
    TEST_ASSERT_NOT_NULL(sensor);
    // Based on the identified logic flaw, to get an ERROR status, we'd need to manually set it.
    // The update_temperature function won't set it.
    sensor->temperature = 45.0f;
    sensor->status = STATUS_ERROR; // Manually set to simulate ERROR for print test
    sensor->data.float_value = 45.0f;

    // Expected: Function call completes without crashing
    print_sensor(sensor);
    TEST_MESSAGE("NOTE: print_sensor output not captured/asserted in unit test. Visual inspection only for this function.");

    free_sensor(sensor);
}

// --- Test cases for process_temperature ---
void test_process_temperature_valid_processor_and_temp(void) {
    float test_temp = 28.5f;

    // Expected: Processor is called with the correct temperature
    process_temperature(test_temp, dummy_temp_processor);
    // Expected: The dummy processor was called
    TEST_ASSERT_TRUE(processor_was_called);
    // Expected: The captured temperature matches the input
    TEST_ASSERT_FLOAT_WITHIN(0.001f, test_temp, captured_temp_for_processor);
}

void test_process_temperature_null_processor_no_crash(void) {
    float test_temp = 15.0f;

    // Expected: Calling with NULL processor should not crash and not call a processor
    process_temperature(test_temp, NULL);
    // Expected: The dummy processor was NOT called
    TEST_ASSERT_FALSE(processor_was_called);
    // Expected: Captured temperature remains at its default (0.0f from setUp)
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, captured_temp_for_processor);
}

void test_process_temperature_different_temp_values(void) {
    float temp1 = 10.0f;
    process_temperature(temp1, dummy_temp_processor);
    TEST_ASSERT_TRUE(processor_was_called);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp1, captured_temp_for_processor);
    setUp(); // Reset for next call

    float temp2 = 0.0f;
    process_temperature(temp2, dummy_temp_processor);
    TEST_ASSERT_TRUE(processor_was_called);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp2, captured_temp_for_processor);
}


// --- Test cases for free_sensor ---
void test_free_sensor_valid_sensor_no_crash(void) {
    Sensor* sensor = create_sensor(401, "Sensor_to_free");
    TEST_ASSERT_NOT_NULL(sensor);

    // Expected: Freeing a valid sensor should not crash
    free_sensor(sensor);
    // No direct assertions possible on `free` itself.
    // The main goal is to ensure memory is released and no crash occurs.
}

void test_free_sensor_null_sensor_no_crash(void) {
    // Expected: Freeing a NULL sensor should not crash
    free_sensor(NULL);
    // No assertions possible beyond "no crash"
}

// --- Main function to run all tests ---


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_create_sensor_success_valid_inputs);
    RUN_TEST(test_create_sensor_name_exact_max_len);
    RUN_TEST(test_create_sensor_name_longer_than_max_len);
    RUN_TEST(test_create_sensor_with_empty_name);
    RUN_TEST(test_update_temperature_null_sensor_no_crash);
    RUN_TEST(test_update_temperature_ok_status);
    RUN_TEST(test_update_temperature_warning_status);
    RUN_TEST(test_update_temperature_error_logic_flaw_actual_behavior);
    RUN_TEST(test_print_sensor_null_sensor_no_crash);
    RUN_TEST(test_print_sensor_valid_sensor_ok_status);
    RUN_TEST(test_print_sensor_valid_sensor_warning_status);
    RUN_TEST(test_print_sensor_valid_sensor_error_status);
    RUN_TEST(test_process_temperature_valid_processor_and_temp);
    RUN_TEST(test_process_temperature_null_processor_no_crash);
    RUN_TEST(test_process_temperature_different_temp_values);
    RUN_TEST(test_free_sensor_valid_sensor_no_crash);
    RUN_TEST(test_free_sensor_null_sensor_no_crash);

    return UNITY_END();
}