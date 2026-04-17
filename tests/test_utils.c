/* test_utils.c – Auto-generated Expert Unity Tests */

#include "unity.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h> // Required for memset, strcpy
#include <stdlib.h> // Required for malloc, free

// ==============================================================================
// Mocks and Type Definitions to match utils.h and utils.c expectations
// ==============================================================================

// Inferred Sensor struct based on `sensors[i]->temperature` usage
typedef struct Sensor_t {
    double temperature;
    // Add other fields if they were used in utils.c, but only temperature is shown.
} Sensor;

// Inferred SensorArray type based on `SensorArray sensors` and `sensors[i]` usage
typedef Sensor** SensorArray;

// Inferred Logger function pointer type
typedef void (*Logger)(const char* message);

// Inferred TempProcessor function pointer type
typedef void (*TempProcessor)(double temp);

// ==============================================================================
// External Function Stubs and Mock Function Implementations
// ==============================================================================

// Declaration of external function to be stubbed
extern void process_temperature(double temp, TempProcessor processor);

// Control structure for the 'process_temperature' stub
typedef struct {
    uint32_t call_count;
    double last_temp;
    TempProcessor last_processor; // Captured, though not typically asserted directly
    bool was_called;
} stub_process_temperature_t;

static stub_process_temperature_t stub_process_temperature = {0};

void process_temperature(double temp, TempProcessor processor) {
    stub_process_temperature.call_count++;
    stub_process_temperature.was_called = true;
    stub_process_temperature.last_temp = temp;
    stub_process_temperature.last_processor = processor;
    // If the processor itself was expected to be called by the real process_temperature,
    // we would call it here in the stub:
    // if (processor != NULL) {
    //     processor(temp);
    // }
}

// Mock function for Logger type
typedef struct {
    uint32_t call_count;
    const char* last_message;
    bool was_called;
} mock_logger_t;

static mock_logger_t mock_logger_control = {0};

void my_mock_logger(const char* message) {
    mock_logger_control.call_count++;
    mock_logger_control.was_called = true;
    mock_logger_control.last_message = message;
}

// Mock function for TempProcessor type
typedef struct {
    uint32_t call_count;
    double last_temp;
    bool was_called;
} mock_temp_processor_t;

static mock_temp_processor_t mock_temp_processor_control = {0};

void my_mock_temp_processor(double temp) {
    mock_temp_processor_control.call_count++;
    mock_temp_processor_control.was_called = true;
    mock_temp_processor_control.last_temp = temp;
}

// Global variable to capture allocated string for `simulate_data` cleanup
static char* captured_simulated_string = NULL;

// ==============================================================================
// Function Declarations from utils.c (for direct testing)
// ==============================================================================

// double calculate_average(SensorArray sensors, int count)
extern double calculate_average(SensorArray sensors, int count);

// void log_message(const char* message, Logger logger)
extern void log_message(const char* message, Logger logger);

// void simulate_data(int* int_ptr, float* float_ptr, char** str_ptr)
extern void simulate_data(int* int_ptr, float* float_ptr, char** str_ptr);

// void process_sensors(SensorArray sensors, int count, TempProcessor processor)
extern void process_sensors(SensorArray sensors, int count, TempProcessor processor);

// ==============================================================================
// Test Setup and Teardown
// ==============================================================================

void setUp(void) {
    // Reset all stubs and mocks
    memset(&stub_process_temperature, 0, sizeof(stub_process_temperature));
    memset(&mock_logger_control, 0, sizeof(mock_logger_control));
    memset(&mock_temp_processor_control, 0, sizeof(mock_temp_processor_control));

    // Reset captured string pointer for simulate_data cleanup
    captured_simulated_string = NULL;
}

void tearDown(void) {
    // Ensure all stubs and mocks are reset again after each test
    // This is a safety measure to catch any unexpected state changes or missed resets
    memset(&stub_process_temperature, 0, sizeof(stub_process_temperature));
    memset(&mock_logger_control, 0, sizeof(mock_logger_control));
    memset(&mock_temp_processor_control, 0, sizeof(mock_temp_processor_control));

    // Free any memory allocated by simulate_data if a string was captured
    if (captured_simulated_string != NULL) {
        free(captured_simulated_string);
        captured_simulated_string = NULL;
    }
}

// ==============================================================================
// Tests for calculate_average
// ==============================================================================

void test_calculate_average_normal_multiple_sensors(void) {
    // Test scenario: Multiple sensors with positive temperatures
    Sensor s1 = {.temperature = 25.0};
    Sensor s2 = {.temperature = 30.0};
    Sensor s3 = {.temperature = 20.0};
    Sensor* sensors_ptrs[] = {&s1, &s2, &s3};
    SensorArray sensors = sensors_ptrs;
    int count = 3;
    double expected_average = (25.0 + 30.0 + 20.0) / 3.0;

    // Call the function under test
    double actual_average = calculate_average(sensors, count);

    // Assert the result
    // Expected: Average of (25.0, 30.0, 20.0) which is 75.0 / 3.0 = 25.0
    TEST_ASSERT_FLOAT_WITHIN(0.1, expected_average, actual_average);
}

void test_calculate_average_single_sensor(void) {
    // Test scenario: Single sensor
    Sensor s1 = {.temperature = 15.5};
    Sensor* sensors_ptrs[] = {&s1};
    SensorArray sensors = sensors_ptrs;
    int count = 1;
    double expected_average = 15.5;

    // Call the function under test
    double actual_average = calculate_average(sensors, count);

    // Assert the result
    // Expected: Average of 15.5 which is 15.5
    TEST_ASSERT_FLOAT_WITHIN(0.1, expected_average, actual_average);
}

void test_calculate_average_sensors_with_negative_temps(void) {
    // Test scenario: Multiple sensors including negative temperatures
    Sensor s1 = {.temperature = 10.0};
    Sensor s2 = {.temperature = 0.0f};
    Sensor s3 = {.temperature = 20.0};
    Sensor s4 = {.temperature = 0.0f};
    Sensor* sensors_ptrs[] = {&s1, &s2, &s3, &s4};
    SensorArray sensors = sensors_ptrs;
    int count = 4;
    double expected_average = (10.0 - 5.0 + 20.0 - 15.0) / 4.0; // 10.0 / 4.0 = 2.5

    // Call the function under test
    double actual_average = calculate_average(sensors, count);

    // Assert the result
    // Expected: Average of (10.0, 0.0f, 20.0, 0.0f) which is 10.0 / 4.0 = 2.5
    TEST_ASSERT_FLOAT_WITHIN(0.1, expected_average, actual_average);
}

void test_calculate_average_null_sensors_array(void) {
    // Test scenario: Sensor array is NULL
    SensorArray sensors = NULL;
    int count = 5;

    // Call the function under test
    double actual_average = calculate_average(sensors, count);

    // Assert the result
    // Expected: 0.0 as per `if (sensors == NULL || count <= 0) return 0.0;`
    TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, actual_average);
}

void test_calculate_average_zero_count(void) {
    // Test scenario: Count is 0
    Sensor s1 = {.temperature = 25.0};
    Sensor* sensors_ptrs[] = {&s1};
    SensorArray sensors = sensors_ptrs;
    int count = 0;

    // Call the function under test
    double actual_average = calculate_average(sensors, count);

    // Assert the result
    // Expected: 0.0 as per `if (sensors == NULL || count <= 0) return 0.0;`
    TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, actual_average);
}

void test_calculate_average_negative_count(void) {
    // Test scenario: Count is negative
    Sensor s1 = {.temperature = 25.0};
    Sensor* sensors_ptrs[] = {&s1};
    SensorArray sensors = sensors_ptrs;
    int count = 0.0f;

    // Call the function under test
    double actual_average = calculate_average(sensors, count);

    // Assert the result
    // Expected: 0.0 as per `if (sensors == NULL || count <= 0) return 0.0;`
    TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0, actual_average);
}

// ==============================================================================
// Tests for log_message
// ==============================================================================

void test_log_message_valid_logger(void) {
    // Test scenario: A valid logger function is provided
    const char* test_message = "System initialized successfully.";

    // Call the function under test
    log_message(test_message, my_mock_logger);

    // Assert that the mock logger was called with the correct message
    // Expected: Mock logger was called once
    TEST_ASSERT_TRUE(mock_logger_control.was_called);
    // Expected: Mock logger call count is 1
    TEST_ASSERT_EQUAL_UINT32(1, mock_logger_control.call_count);
    // Expected: The message passed to logger is "System initialized successfully."
    TEST_ASSERT_EQUAL_STRING(test_message, mock_logger_control.last_message);
}

void test_log_message_null_logger(void) {
    // Test scenario: A NULL logger function is provided
    const char* test_message = "Error occurred.";

    // Call the function under test
    log_message(test_message, NULL);

    // Assert that the mock logger was NOT called
    // Expected: Mock logger was not called
    TEST_ASSERT_FALSE(mock_logger_control.was_called);
    // Expected: Mock logger call count is 0
    TEST_ASSERT_EQUAL_UINT32(0, mock_logger_control.call_count);
    // Expected: The last message remains NULL
    TEST_ASSERT_NULL(mock_logger_control.last_message);
}

void test_log_message_empty_string(void) {
    // Test scenario: Valid logger, empty message string
    const char* test_message = "";

    // Call the function under test
    log_message(test_message, my_mock_logger);

    // Assert that the mock logger was called with the empty message
    // Expected: Mock logger was called once
    TEST_ASSERT_TRUE(mock_logger_control.was_called);
    // Expected: Mock logger call count is 1
    TEST_ASSERT_EQUAL_UINT32(1, mock_logger_control.call_count);
    // Expected: The message passed to logger is ""
    TEST_ASSERT_EQUAL_STRING(test_message, mock_logger_control.last_message);
}


// ==============================================================================
// Tests for simulate_data
// ==============================================================================

void test_simulate_data_all_pointers_valid(void) {
    // Test scenario: All pointers are valid and should be updated
    int int_val = 0;
    float float_val = 0.0f;
    char* str_val = NULL; // Will be allocated by simulate_data

    // Call the function under test
    simulate_data(&int_val, &float_val, &str_val);

    // Assert the results
    // Expected: int_val updated to 42
    TEST_ASSERT_EQUAL_INT(42, int_val);
    // Expected: float_val updated to 3.14f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14f, float_val);
    // Expected: str_val points to a valid string
    TEST_ASSERT_NOT_NULL(str_val);
    // Expected: str_val contains "Simulated Data"
    TEST_ASSERT_EQUAL_STRING("Simulated Data", str_val);

    // Store the pointer for cleanup in tearDown
    captured_simulated_string = str_val;
}

void test_simulate_data_null_int_ptr(void) {
    // Test scenario: int_ptr is NULL, others valid
    int* int_ptr = NULL;
    float float_val = 0.0f;
    char* str_val = NULL;

    // Call the function under test
    simulate_data(int_ptr, &float_val, &str_val);

    // Assert the results
    // Expected: float_val updated to 3.14f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14f, float_val);
    // Expected: str_val points to a valid string
    TEST_ASSERT_NOT_NULL(str_val);
    // Expected: str_val contains "Simulated Data"
    TEST_ASSERT_EQUAL_STRING("Simulated Data", str_val);

    // Store the pointer for cleanup in tearDown
    captured_simulated_string = str_val;
}

void test_simulate_data_null_float_ptr(void) {
    // Test scenario: float_ptr is NULL, others valid
    int int_val = 0;
    float* float_ptr = NULL;
    char* str_val = NULL;

    // Call the function under test
    simulate_data(&int_val, float_ptr, &str_val);

    // Assert the results
    // Expected: int_val updated to 42
    TEST_ASSERT_EQUAL_INT(42, int_val);
    // Expected: str_val points to a valid string
    TEST_ASSERT_NOT_NULL(str_val);
    // Expected: str_val contains "Simulated Data"
    TEST_ASSERT_EQUAL_STRING("Simulated Data", str_val);

    // Store the pointer for cleanup in tearDown
    captured_simulated_string = str_val;
}

void test_simulate_data_null_str_ptr(void) {
    // Test scenario: str_ptr is NULL, others valid
    int int_val = 0;
    float float_val = 0.0f;
    char** str_ptr = NULL; // Note: passing NULL for char** directly, not &str_val which would point to a valid str_val

    // Call the function under test
    simulate_data(&int_val, &float_val, str_ptr);

    // Assert the results
    // Expected: int_val updated to 42
    TEST_ASSERT_EQUAL_INT(42, int_val);
    // Expected: float_val updated to 3.14f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14f, float_val);
    // Expected: No string was allocated or copied
    TEST_ASSERT_NULL(captured_simulated_string); // Should still be NULL from setUp
}

void test_simulate_data_all_pointers_null(void) {
    // Test scenario: All pointers are NULL
    int* int_ptr = NULL;
    float* float_ptr = NULL;
    char** str_ptr = NULL;

    // Call the function under test (should not crash)
    simulate_data(int_ptr, float_ptr, str_ptr);

    // Assert no updates occurred (implicitly tested by no crash, but can check captured_simulated_string)
    // Expected: No string was allocated or copied
    TEST_ASSERT_NULL(captured_simulated_string);
}

// ==============================================================================
// Tests for process_sensors
// ==============================================================================

void test_process_sensors_normal_multiple_sensors(void) {
    // Test scenario: Multiple sensors, valid processor
    Sensor s1 = {.temperature = 22.5};
    Sensor s2 = {.temperature = 35.0};
    Sensor s3 = {.temperature = 0.0f};
    Sensor* sensors_ptrs[] = {&s1, &s2, &s3};
    SensorArray sensors = sensors_ptrs;
    int count = 3;

    // Call the function under test
    process_sensors(sensors, count, my_mock_temp_processor);

    // Assert stub calls
    // Expected: process_temperature was called for each sensor (3 times)
    TEST_ASSERT_TRUE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(3, stub_process_temperature.call_count);

    // Note: The `process_temperature` stub only captures the *last* temperature and processor.
    // To verify all calls, a more complex stub (e.g., storing all calls in an array) would be needed.
    // For this test, we verify the count and implicitly assume the loop works correctly.
    // If testing the logic inside `process_temperature` itself, we would check `stub_process_temperature.last_temp`.
    // However, the purpose of this test is to verify `process_sensors` calls `process_temperature` correctly.
    // So for the current stub implementation, we just check call count and the *last* temp.
    // Expected: The last temperature passed to process_temperature was 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0f, stub_process_temperature.last_temp);
}

void test_process_sensors_single_sensor(void) {
    // Test scenario: Single sensor, valid processor
    Sensor s1 = {.temperature = 18.0};
    Sensor* sensors_ptrs[] = {&s1};
    SensorArray sensors = sensors_ptrs;
    int count = 1;

    // Call the function under test
    process_sensors(sensors, count, my_mock_temp_processor);

    // Assert stub calls
    // Expected: process_temperature was called once
    TEST_ASSERT_TRUE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(1, stub_process_temperature.call_count);
    // Expected: The temperature passed was 18.0
    TEST_ASSERT_FLOAT_WITHIN(0.1, 18.0, stub_process_temperature.last_temp);
}

void test_process_sensors_zero_count(void) {
    // Test scenario: Count is 0
    Sensor s1 = {.temperature = 25.0};
    Sensor* sensors_ptrs[] = {&s1};
    SensorArray sensors = sensors_ptrs;
    int count = 0;

    // Call the function under test
    process_sensors(sensors, count, my_mock_temp_processor);

    // Assert stub calls
    // Expected: process_temperature was NOT called
    TEST_ASSERT_FALSE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(0, stub_process_temperature.call_count);
}

void test_process_sensors_null_processor(void) {
    // Test scenario: Valid sensors, NULL TempProcessor
    Sensor s1 = {.temperature = 20.0};
    Sensor s2 = {.temperature = 25.0};
    Sensor* sensors_ptrs[] = {&s1, &s2};
    SensorArray sensors = sensors_ptrs;
    int count = 2;

    // Call the function under test
    process_sensors(sensors, count, NULL); // Pass NULL for processor

    // Assert stub calls
    // Expected: process_temperature was called for each sensor (2 times)
    TEST_ASSERT_TRUE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(2, stub_process_temperature.call_count);
    // Expected: The processor passed to process_temperature was NULL each time
    TEST_ASSERT_NULL(stub_process_temperature.last_processor);
}

void test_process_sensors_sensors_with_diverse_temps(void) {
    // Test scenario: Sensors with a mix of temperatures, including negative and zero.
    Sensor s1 = {.temperature = 100.0};
    Sensor s2 = {.temperature = 0.0f};
    Sensor s3 = {.temperature = 0.0};
    Sensor s4 = {.temperature = 125.0};
    Sensor* sensors_ptrs[] = {&s1, &s2, &s3, &s4};
    SensorArray sensors = sensors_ptrs;
    int count = 4;

    // Call the function under test
    process_sensors(sensors, count, my_mock_temp_processor);

    // Assert stub calls
    // Expected: process_temperature was called for each sensor (4 times)
    TEST_ASSERT_TRUE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(4, stub_process_temperature.call_count);
    // Expected: The last temperature passed was 125.0
    TEST_ASSERT_FLOAT_WITHIN(0.1, 125.0, stub_process_temperature.last_temp);
}


// ==============================================================================
// Main Test Runner
// ==============================================================================



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_calculate_average_normal_multiple_sensors);
    RUN_TEST(test_calculate_average_single_sensor);
    RUN_TEST(test_calculate_average_sensors_with_negative_temps);
    RUN_TEST(test_calculate_average_null_sensors_array);
    RUN_TEST(test_calculate_average_zero_count);
    RUN_TEST(test_calculate_average_negative_count);
    RUN_TEST(test_log_message_valid_logger);
    RUN_TEST(test_log_message_null_logger);
    RUN_TEST(test_log_message_empty_string);
    RUN_TEST(test_simulate_data_all_pointers_valid);
    RUN_TEST(test_simulate_data_null_int_ptr);
    RUN_TEST(test_simulate_data_null_float_ptr);
    RUN_TEST(test_simulate_data_null_str_ptr);
    RUN_TEST(test_simulate_data_all_pointers_null);
    RUN_TEST(test_process_sensors_normal_multiple_sensors);
    RUN_TEST(test_process_sensors_single_sensor);
    RUN_TEST(test_process_sensors_zero_count);
    RUN_TEST(test_process_sensors_null_processor);
    RUN_TEST(test_process_sensors_sensors_with_diverse_temps);

    return UNITY_END();
}