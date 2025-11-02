/* test_sensor.c – Auto-generated Expert Unity Tests */

#include "unity.h"
#include "sensor.h"   // Contains Sensor, SensorStatus, TempProcessor definitions
#include <stdlib.h>   // For malloc, free
#include <string.h>   // For strncpy
#include <stdio.h>    // For printf

// --- Mock for TempProcessor function pointer ---
// This acts as a test double (a mock/spy) for the function pointer argument,
// not a stub for an external function requiring `__wrap_` linkage.
static float mock_processor_last_temp_value = 0.0f;
static unsigned int mock_processor_call_count = 0;

void mock_temp_processor_func(float temp) {
    mock_processor_last_temp_value = temp;
    mock_processor_call_count++;
}

// --- setUp and tearDown functions ---
void setUp(void) {
    // Reset the mock processor state for each test
    mock_processor_last_temp_value = 0.0f;
    mock_processor_call_count = 0;
}

void tearDown(void) {
    // Ensure any dynamically allocated memory is freed by individual tests.
    // The mock processor state is reset again for full isolation, though setUp should handle it.
    mock_processor_last_temp_value = 0.0f;
    mock_processor_call_count = 0;
}

// --- Test functions for create_sensor ---

// Test 1: Successful sensor creation with valid ID and name
void test_create_sensor_success_valid_params(void) {
    int id = 101;
    const char* name = "Room_Sensor_A";
    Sensor* sensor = create_sensor(id, name);

    // Expected: Sensor pointer is not NULL, indicating successful allocation
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: Sensor ID matches the input ID
    TEST_ASSERT_EQUAL_INT(id, sensor->id);
    // Expected: Sensor name matches the input name
    TEST_ASSERT_EQUAL_STRING(name, sensor->name);
    // Expected: Initial temperature is 0.0f, with a tolerance for float comparison
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, sensor->temperature);
    // Expected: Initial status is STATUS_OK
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Initial union float_value is 0.0f, with a tolerance
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, sensor->data.float_value);

    // Clean up dynamically allocated memory
    free_sensor(sensor);
}

// Test 2: Sensor creation with a name that is longer than the buffer, check for truncation and null termination
void test_create_sensor_success_long_name_truncation(void) {
    int id = 102;
    // This name is definitely longer than typical embedded buffer sizes (e.g., char name[32])
    const char* long_name = "A_very_very_very_long_sensor_name_that_should_be_truncated_by_the_buffer_size";
    Sensor* sensor = create_sensor(id, long_name);

    // Expected: Sensor pointer is not NULL
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: Sensor ID matches input
    TEST_ASSERT_EQUAL_INT(id, sensor->id);

    // Create an expected truncated string based on the actual size of the sensor->name array
    char expected_name[sizeof(((Sensor*)0)->name)]; // Compile-time size of the name buffer
    strncpy(expected_name, long_name, sizeof(expected_name) - 1);
    expected_name[sizeof(expected_name) - 1] = '\0'; // Manually ensure null termination

    // Expected: Name is truncated and null-terminated correctly
    TEST_ASSERT_EQUAL_STRING(expected_name, sensor->name);
    // Expected: Initial status is STATUS_OK
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);

    // Clean up
    free_sensor(sensor);
}

// Test 3: Sensor creation with an empty name string
void test_create_sensor_success_empty_name(void) {
    int id = 103;
    const char* name = "";
    Sensor* sensor = create_sensor(id, name);

    // Expected: Sensor pointer is not NULL
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: Sensor ID matches input
    TEST_ASSERT_EQUAL_INT(id, sensor->id);
    // Expected: Name is an empty string, correctly null-terminated
    TEST_ASSERT_EQUAL_STRING("", sensor->name);
    // Expected: Initial status is STATUS_OK
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);

    // Clean up
    free_sensor(sensor);
}

// Note: As per the rule "EXTERNAL FUNCTIONS TO STUB (only these; infer signatures from calls if needed; use typical embedded types): - None",
// we cannot stub `malloc`. Therefore, testing the `create_sensor` path for `malloc` failure (returning NULL)
// is not feasible in a unit test environment without actual memory exhaustion or specific linker hacks.
// This test suite focuses on paths that can be deterministically controlled.

// --- Test functions for update_temperature ---

// Test 1: Update temperature to a value well within the "OK" range (temp <= 30.0f)
void test_update_temperature_normal_ok(void) {
    Sensor* sensor = create_sensor(201, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    float temp = 25.0f;
    update_temperature(sensor, temp);

    // Expected: Temperature is updated to the new value
    TEST_ASSERT_FLOAT_WITHIN(0.1f, temp, sensor->temperature);
    // Expected: Status remains STATUS_OK because 25.0f <= 30.0f
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union float_value is updated to the new temperature
    TEST_ASSERT_FLOAT_WITHIN(0.1f, temp, sensor->data.float_value);

    free_sensor(sensor);
}

// Test 2: Update temperature to the upper edge of the "OK" range (temp = 30.0f)
void test_update_temperature_edge_ok(void) {
    Sensor* sensor = create_sensor(202, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    float temp = 30.0f;
    update_temperature(sensor, temp);

    // Expected: Temperature is updated
    TEST_ASSERT_FLOAT_WITHIN(0.1f, temp, sensor->temperature);
    // Expected: Status is STATUS_OK because 30.0f is not > 30.0f
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union float_value is updated
    TEST_ASSERT_FLOAT_WITHIN(0.1f, temp, sensor->data.float_value);

    free_sensor(sensor);
}

// Test 3: Update temperature just above 30.0f, triggering WARNING status (temp > 30.0f)
void test_update_temperature_above_30_warning(void) {
    Sensor* sensor = create_sensor(203, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    float temp = 30.1f;
    update_temperature(sensor, temp);

    // Expected: Temperature is updated
    TEST_ASSERT_FLOAT_WITHIN(0.1f, temp, sensor->temperature);
    // Expected: Status is STATUS_WARNING because 30.1f > 30.0f
    TEST_ASSERT_EQUAL_INT(STATUS_WARNING, sensor->status);
    // Expected: Union float_value is updated
    TEST_ASSERT_FLOAT_WITHIN(0.1f, temp, sensor->data.float_value);

    free_sensor(sensor);
}

// Test 4: Update temperature to exactly 40.0f. Due to the source code's if/else if order,
//        any value > 30.0f will hit the first 'if' and set STATUS_WARNING, making the 'else if (temp > 40.0f)'
//        branch for STATUS_ERROR unreachable if 'temp > 30.0f' is true.
void test_update_temperature_at_40_warning_due_to_logic(void) {
    Sensor* sensor = create_sensor(204, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    float temp = 40.0f;
    update_temperature(sensor, temp);

    // Expected: Temperature is updated
    TEST_ASSERT_FLOAT_WITHIN(0.1f, temp, sensor->temperature);
    // Expected: Status is STATUS_WARNING, as 40.0f > 30.0f is true, making the first 'if' block execute.
    // The 'else if (temp > 40.0f)' is not evaluated for equality at 40.0f, and even if it was `> 40.0f` is false.
    // However, the critical part is that `40.0f > 30.0f` is true, so STATUS_WARNING is set.
    TEST_ASSERT_EQUAL_INT(STATUS_WARNING, sensor->status);
    // Expected: Union float_value is updated
    TEST_ASSERT_FLOAT_WITHIN(0.1f, temp, sensor->data.float_value);

    free_sensor(sensor);
}

// Test 5: Update temperature to a value above 40.0f. Still WARNING due to conditional logic.
void test_update_temperature_above_40_warning_due_to_logic(void) {
    Sensor* sensor = create_sensor(205, "TempSensor");
    TEST_ASSERT_NOT_NULL(sensor);

    float temp = 44.5f;
    update_temperature(sensor, temp);

    // Expected: Temperature is updated
    TEST_ASSERT_FLOAT_WITHIN(0.1f, temp, sensor->temperature);
    // Expected: Status is STATUS_WARNING, as 44.5f > 30.0f is true, making the first 'if' block execute.
    // The 'else if (temp > 40.0f)' branch is unreachable because the first 'if' was true.
    TEST_ASSERT_EQUAL_INT(STATUS_WARNING, sensor->status);
    // Expected: Union float_value is updated
    TEST_ASSERT_FLOAT_WITHIN(0.1f, temp, sensor->data.float_value);

    free_sensor(sensor);
}


// Test 6: Call update_temperature with a NULL sensor pointer (should gracefully handle it)
void test_update_temperature_null_sensor(void) {
    // Expected: Function should not crash or attempt to dereference NULL.
    // As it's a void function with no return, the test passes if no crash occurs.
    update_temperature(NULL, 25.0f);
}

// --- Test functions for print_sensor ---

// Test 1: Print sensor info with a valid sensor for different statuses
void test_print_sensor_valid_sensor(void) {
    Sensor* sensor = create_sensor(301, "DisplaySensor");
    TEST_ASSERT_NOT_NULL(sensor);

    // Set to OK status
    update_temperature(sensor, 28.5f);
    // Note: Cannot directly assert `printf` output without platform-specific redirection or stubbing `printf`.
    // This test ensures the function executes without crashing and covers the conditional branches for status string.
    print_sensor(sensor); // Prints "OK"

    // Set to WARNING status (e.g., 35.0f)
    update_temperature(sensor, 35.0f);
    print_sensor(sensor); // Prints "WARNING"

    // Due to the conditional logic in update_temperature, it's not possible to achieve STATUS_ERROR via update_temperature.
    // To ensure the "ERROR" string branch in print_sensor is covered, we would need to directly manipulate sensor->status
    // if it were allowed by the API, or if update_temperature's logic was different.
    // For this test, we are strictly following the existing source logic.
    // However, if we were to test the print path specifically, we could temporarily set the status for testing:
    sensor->status = STATUS_ERROR; // Directly set status to cover "ERROR" print branch
    print_sensor(sensor); // Prints "ERROR"

    free_sensor(sensor);
    // Manual verification of console output for different statuses is recommended.
}

// Test 2: Call print_sensor with a NULL sensor pointer (should gracefully handle it)
void test_print_sensor_null_sensor(void) {
    // Expected: Function should not crash or attempt to dereference NULL.
    // As it's a void function, the test passes if no crash occurs.
    print_sensor(NULL);
}

// --- Test functions for process_temperature ---

// Test 1: Process temperature with a valid function pointer
void test_process_temperature_valid_processor(void) {
    float temp_to_process = 22.5f;
    // Call the function under test with our mock processor function
    process_temperature(temp_to_process, mock_temp_processor_func);

    // Expected: The mock function should have been called exactly once
    TEST_ASSERT_EQUAL_UINT(1, mock_processor_call_count);
    // Expected: The mock function should have received the correct temperature value
    TEST_ASSERT_FLOAT_WITHIN(0.1f, temp_to_process, mock_processor_last_temp_value);
}

// Test 2: Process temperature with a NULL function pointer (should gracefully handle it)
void test_process_temperature_null_processor(void) {
    float temp_to_process = 10.0f;
    // Call the function under test with a NULL function pointer
    process_temperature(temp_to_process, NULL);

    // Expected: The mock function should NOT have been called
    TEST_ASSERT_EQUAL_UINT(0, mock_processor_call_count);
    // Expected: The mock's last_temp_value should remain its initial/reset value
    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, mock_processor_last_temp_value);
    // Test passes if no crash occurs.
}

// --- Test functions for free_sensor ---

// Test 1: Free a valid, previously created sensor
void test_free_sensor_valid_sensor(void) {
    Sensor* sensor = create_sensor(401, "SensorToFree");
    TEST_ASSERT_NOT_NULL(sensor); // Ensure creation was successful

    // Call free_sensor on the allocated sensor
    // Note: As per the rule "EXTERNAL FUNCTIONS TO STUB: - None", we are not stubbing `free`.
    // Therefore, we cannot directly assert that `free` was called.
    // This test primarily ensures that calling `free_sensor` with a valid pointer
    // executes without causing a crash or other observable issues.
    free_sensor(sensor);

    // After calling free_sensor, `sensor` is a dangling pointer. DO NOT dereference it.
    // The test implicitly passes if no runtime error occurs.
}

// Test 2: Call free_sensor with a NULL sensor pointer (should gracefully handle it)
void test_free_sensor_null_sensor(void) {
    // Expected: Function should not crash when given a NULL pointer.
    // As it's a void function, the test passes if no crash occurs.
    free_sensor(NULL);
}


// --- Main function to run tests ---



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_create_sensor_success_valid_params);
    RUN_TEST(test_create_sensor_success_long_name_truncation);
    RUN_TEST(test_create_sensor_success_empty_name);
    RUN_TEST(test_update_temperature_normal_ok);
    RUN_TEST(test_update_temperature_edge_ok);
    RUN_TEST(test_update_temperature_above_30_warning);
    RUN_TEST(test_update_temperature_at_40_warning_due_to_logic);
    RUN_TEST(test_update_temperature_above_40_warning_due_to_logic);
    RUN_TEST(test_update_temperature_null_sensor);
    RUN_TEST(test_print_sensor_valid_sensor);
    RUN_TEST(test_print_sensor_null_sensor);
    RUN_TEST(test_process_temperature_valid_processor);
    RUN_TEST(test_process_temperature_null_processor);
    RUN_TEST(test_free_sensor_valid_sensor);
    RUN_TEST(test_free_sensor_null_sensor);

    return UNITY_END();
}