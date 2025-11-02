/* test_sensor.c – Auto-generated Expert Unity Tests */
#include "unity.h"
#include "sensor.h" // Required to get Sensor struct and TempProcessor definition
#include <stdio.h>  // For printf in source
#include <stdlib.h> // For malloc/free in source
#include <string.h> // For strncpy in source
#include <stdbool.h> // For boolean types

// Helper function pointer for process_temperature tests
static float s_captured_temp = 0.0f;
static bool s_processor_called = false;

static void test_processor_callback(float temp) {
    s_captured_temp = temp;
    s_processor_called = true;
}

void setUp(void) {
    // Initialize any global test state or mocks (none in this case for external stubs)
    s_captured_temp = 0.0f;
    s_processor_called = false;
}

void tearDown(void) {
    // Clean up any global test state or mocks (none in this case for external stubs)
    s_captured_temp = 0.0f;
    s_processor_called = false;
}

// ====================================================================================
// Tests for create_sensor
// Sensor* create_sensor(int id, const char* name)
// Branches: malloc success, malloc failure
// NOTE: malloc failure cannot be tested without mocking malloc, which is prohibited by the prompt.
// ====================================================================================

void test_create_sensor_success_valid_input(void) {
    Sensor* sensor = create_sensor(1, "TemperatureSensor");

    // Expected: Sensor pointer is not NULL
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: ID is 1
    TEST_ASSERT_EQUAL_INT(1, sensor->id);
    // Expected: Name is "TemperatureSensor"
    TEST_ASSERT_EQUAL_STRING("TemperatureSensor", sensor->name);
    // Expected: Temperature is 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, sensor->temperature);
    // Expected: Status is STATUS_OK
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union float_value is 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, sensor->data.float_value);

    free_sensor(sensor); // Clean up allocated memory
}

void test_create_sensor_success_long_name_truncation(void) {
    // Assuming MAX_SENSOR_NAME_LEN (from sensor.h) is 32 based on typical embedded practice.
    // If the actual size in sensor.h is different, this test might need adjustment.
    // Let's create a name longer than what a common 32-byte buffer can hold (31 chars + null).
    const char* long_name = "ThisIsAVeryLongSensorNameThatShouldBeTruncated"; // Length > 31

    Sensor* sensor = create_sensor(2, long_name);

    // Expected: Sensor pointer is not NULL
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: ID is 2
    TEST_ASSERT_EQUAL_INT(2, sensor->id);
    // Expected: Name is truncated and null-terminated.
    // We cannot directly know sizeof(sensor->name) here without inspecting sensor.h
    // Let's assume sizeof(sensor->name) is 32 from sensor.h, so max length is 31.
    // Expected: Truncated string match
    char expected_name_part[sizeof(sensor->name)];
    strncpy(expected_name_part, long_name, sizeof(sensor->name) - 1);
    expected_name_part[sizeof(sensor->name) - 1] = '\0';
    TEST_ASSERT_EQUAL_STRING(expected_name_part, sensor->name);
    // Expected: Temperature is 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, sensor->temperature);
    // Expected: Status is STATUS_OK
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union float_value is 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, sensor->data.float_value);

    free_sensor(sensor); // Clean up allocated memory
}

void test_create_sensor_success_empty_name(void) {
    Sensor* sensor = create_sensor(3, "");

    // Expected: Sensor pointer is not NULL
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: ID is 3
    TEST_ASSERT_EQUAL_INT(3, sensor->id);
    // Expected: Name is an empty string
    TEST_ASSERT_EQUAL_STRING("", sensor->name);
    // Expected: Temperature is 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, sensor->temperature);
    // Expected: Status is STATUS_OK
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union float_value is 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, sensor->data.float_value);

    free_sensor(sensor); // Clean up allocated memory
}

// ====================================================================================
// Tests for update_temperature
// void update_temperature(Sensor* sensor, float temp)
// Branches: sensor == NULL, temp > 30.0f, else if (temp > 40.0f) (UNREACHABLE), else
// ====================================================================================

void test_update_temperature_null_sensor(void) {
    // This call should not crash. No assertions possible on internal state.
    update_temperature(NULL, 25.0f);
    TEST_PASS(); // Indicate that no crash occurred
}

void test_update_temperature_status_ok_low_temp(void) {
    Sensor* sensor = create_sensor(10, "TestSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    update_temperature(sensor, 25.0f);

    // Expected: Temperature updated to 25.0f
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.0f, sensor->temperature);
    // Expected: Status is STATUS_OK (25.0f is not > 30.0f)
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union float_value updated to 25.0f
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 25.0f, sensor->data.float_value);

    free_sensor(sensor);
}

void test_update_temperature_status_ok_boundary_30_0f(void) {
    Sensor* sensor = create_sensor(11, "BoundarySensor");
    TEST_ASSERT_NOT_NULL(sensor);

    update_temperature(sensor, 30.0f);

    // Expected: Temperature updated to 30.0f
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 30.0f, sensor->temperature);
    // Expected: Status is STATUS_OK (30.0f is not > 30.0f)
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union float_value updated to 30.0f
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 30.0f, sensor->data.float_value);

    free_sensor(sensor);
}

void test_update_temperature_status_warning_boundary_30_1f(void) {
    Sensor* sensor = create_sensor(12, "WarningSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    update_temperature(sensor, 30.1f);

    // Expected: Temperature updated to 30.1f
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 30.1f, sensor->temperature);
    // Expected: Status is STATUS_WARNING (30.1f is > 30.0f)
    TEST_ASSERT_EQUAL_INT(STATUS_WARNING, sensor->status);
    // Expected: Union float_value updated to 30.1f
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 30.1f, sensor->data.float_value);

    free_sensor(sensor);
}

void test_update_temperature_status_warning_high_temp_logic_flaw(void) {
    // This test specifically targets the logic flaw in the source code:
    // if (temp > 30.0f) sets WARNING, else if (temp > 40.0f) sets ERROR.
    // For temp > 40.0f, the first 'if' (temp > 30.0f) will always be true,
    // making the 'else if (temp > 40.0f)' branch unreachable.
    Sensor* sensor = create_sensor(13, "HighTempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    update_temperature(sensor, 45.0f); // Temperature > 40.0f

    // Expected: Temperature updated to 45.0f
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 45.0f, sensor->temperature);
    // Expected: Status is STATUS_WARNING, NOT STATUS_ERROR, due to the order of if-else if conditions.
    TEST_ASSERT_EQUAL_INT(STATUS_WARNING, sensor->status);
    // Expected: Union float_value updated to 45.0f
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 45.0f, sensor->data.float_value);

    free_sensor(sensor);
}

void test_update_temperature_status_ok_negative_temp(void) {
    Sensor* sensor = create_sensor(14, "ColdSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    update_temperature(sensor, 0.0f);

    // Expected: Temperature updated to 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, sensor->temperature);
    // Expected: Status is STATUS_OK (0.0f is not > 30.0f)
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union float_value updated to 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, sensor->data.float_value);

    free_sensor(sensor);
}

// ====================================================================================
// Tests for print_sensor
// void print_sensor(const Sensor* sensor)
// Branches: sensor == NULL
// NOTE: Direct assertion of printf output is not possible with Unity without stdout redirection.
// ====================================================================================

void test_print_sensor_null_sensor(void) {
    // This call should not crash. No assertions on output.
    print_sensor(NULL);
    TEST_PASS(); // Indicate that no crash occurred
}

void test_print_sensor_valid_sensor_ok_status(void) {
    Sensor* sensor = create_sensor(20, "SensorOK");
    TEST_ASSERT_NOT_NULL(sensor);
    update_temperature(sensor, 25.0f); // Set to OK status

    // Calling print_sensor. Output is not asserted directly.
    // Expected: Prints ID, Name, Temp=25.00, Status=OK, Data=25.00
    print_sensor(sensor);
    // Assertion would require stdout redirection, not implemented here.
    TEST_PASS();

    free_sensor(sensor);
}

void test_print_sensor_valid_sensor_warning_status(void) {
    Sensor* sensor = create_sensor(21, "SensorWarning");
    TEST_ASSERT_NOT_NULL(sensor);
    update_temperature(sensor, 35.0f); // Set to WARNING status

    // Calling print_sensor. Output is not asserted directly.
    // Expected: Prints ID, Name, Temp=35.00, Status=WARNING, Data=35.00
    print_sensor(sensor);
    // Assertion would require stdout redirection, not implemented here.
    TEST_PASS();

    free_sensor(sensor);
}

void test_print_sensor_valid_sensor_error_status_manual(void) {
    // Manually set status to ERROR to test the printf logic branch,
    // as update_temperature cannot set STATUS_ERROR due to the logic flaw.
    Sensor* sensor = create_sensor(22, "SensorErrorManual");
    TEST_ASSERT_NOT_NULL(sensor);
    sensor->temperature = 50.0f;
    sensor->status = STATUS_ERROR; // Manually set to cover print branch
    sensor->data.float_value = 50.0f;

    // Calling print_sensor. Output is not asserted directly.
    // Expected: Prints ID, Name, Temp=50.00, Status=ERROR, Data=50.00
    print_sensor(sensor);
    // Assertion would require stdout redirection, not implemented here.
    TEST_PASS();

    free_sensor(sensor);
}

// ====================================================================================
// Tests for process_temperature
// void process_temperature(float temp, TempProcessor processor)
// Branches: processor != NULL
// ====================================================================================

void test_process_temperature_null_processor(void) {
    // This call should not crash.
    process_temperature(22.5f, NULL);
    // Expected: Callback should not have been called
    TEST_ASSERT_FALSE(s_processor_called);
}

void test_process_temperature_valid_processor(void) {
    float test_temp = 22.5f;

    process_temperature(test_temp, test_processor_callback);

    // Expected: Callback should have been called
    TEST_ASSERT_TRUE(s_processor_called);
    // Expected: Captured temperature should match the input
    TEST_ASSERT_FLOAT_WITHIN(0.1f, test_temp, s_captured_temp);
}

void test_process_temperature_another_temp(void) {
    float test_temp = 0.0f;

    process_temperature(test_temp, test_processor_callback);

    // Expected: Callback should have been called
    TEST_ASSERT_TRUE(s_processor_called);
    // Expected: Captured temperature should match the input
    TEST_ASSERT_FLOAT_WITHIN(0.1f, test_temp, s_captured_temp);
}


// ====================================================================================
// Tests for free_sensor
// void free_sensor(Sensor* sensor)
// Branches: sensor != NULL
// ====================================================================================

void test_free_sensor_null_sensor(void) {
    // This call should not crash. No assertions possible on internal state.
    free_sensor(NULL);
    TEST_PASS(); // Indicate that no crash occurred
}

void test_free_sensor_valid_sensor(void) {
    Sensor* sensor = create_sensor(30, "SensorToFree");
    TEST_ASSERT_NOT_NULL(sensor);

    // Call free_sensor. No direct way to assert memory is freed without
    // a custom memory allocator mock, which is not permitted.
    free_sensor(sensor);
    // Expected: The memory pointed to by 'sensor' is now freed.
    // Further access to 'sensor' would be undefined behavior.
    TEST_PASS(); // Indicate no crash
}

// ====================================================================================
// Main test runner
// ====================================================================================



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_processor_callback);
    RUN_TEST(test_create_sensor_success_valid_input);
    RUN_TEST(test_create_sensor_success_long_name_truncation);
    RUN_TEST(test_create_sensor_success_empty_name);
    RUN_TEST(test_update_temperature_null_sensor);
    RUN_TEST(test_update_temperature_status_ok_low_temp);
    RUN_TEST(test_update_temperature_status_ok_boundary_30_0f);
    RUN_TEST(test_update_temperature_status_warning_boundary_30_1f);
    RUN_TEST(test_update_temperature_status_warning_high_temp_logic_flaw);
    RUN_TEST(test_update_temperature_status_ok_negative_temp);
    RUN_TEST(test_print_sensor_null_sensor);
    RUN_TEST(test_print_sensor_valid_sensor_ok_status);
    RUN_TEST(test_print_sensor_valid_sensor_warning_status);
    RUN_TEST(test_print_sensor_valid_sensor_error_status_manual);
    RUN_TEST(test_process_temperature_null_processor);
    RUN_TEST(test_process_temperature_valid_processor);
    RUN_TEST(test_process_temperature_another_temp);
    RUN_TEST(test_free_sensor_null_sensor);
    RUN_TEST(test_free_sensor_valid_sensor);

    return UNITY_END();
}