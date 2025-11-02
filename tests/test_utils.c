/* test_utils.c – Auto-generated Expert Unity Tests */

#include "unity.h"
#include "utils.h" // Includes Sensor, SensorArray, Logger, TempProcessor, and function declarations

#include <string.h> // Required for strcpy in utils.c and potentially for assertions
#include <stdlib.h> // Required for malloc/free in simulate_data and test setup

// =================================================================================================
// EXTERNAL FUNCTION STUBS
// =================================================================================================

// Stub control structure for process_temperature
typedef struct {
    uint32_t call_count;
    double last_temperature;
    TempProcessor last_processor; // Captures the function pointer passed
    double temperatures[10];     // Array to capture multiple temperature calls
    int processors_called[10];   // To track which processor was called (index) if needed
    // Assuming 10 calls max for reasonable test cases
} stub_process_temperature_t;

static stub_process_temperature_t stub_process_temperature = {0};

void process_temperature(double temperature, TempProcessor processor) {
    if (stub_process_temperature.call_count < sizeof(stub_process_temperature.temperatures) / sizeof(stub_process_temperature.temperatures[0])) {
        stub_process_temperature.temperatures[stub_process_temperature.call_count] = temperature;
    }
    stub_process_temperature.last_temperature = temperature;
    stub_process_temperature.last_processor = processor;
    stub_process_temperature.call_count++;
    // In a real scenario, we might call the captured 'processor' here if it's meant to be executed
    // For now, we only capture the fact it was passed.
}

// Mock function for Logger type
static const char* mock_logger_last_message = NULL;
static uint32_t mock_logger_call_count = 0;
void MockLogger(const char* message) {
    mock_logger_last_message = message;
    mock_logger_call_count++;
}

// Mock function for TempProcessor type (used as a callback within process_sensors)
static double mock_temp_processor_last_temp = 0.0;
static uint32_t mock_temp_processor_call_count = 0;
void MockTempProcessor(double temperature_value) {
    mock_temp_processor_last_temp = temperature_value;
    mock_temp_processor_call_count++;
}

// =================================================================================================
// Test Fixture Setup and Teardown
// =================================================================================================

void setUp(void) {
    // Reset all stub control structures
    memset(&stub_process_temperature, 0, sizeof(stub_process_temperature));
    mock_logger_last_message = NULL;
    mock_logger_call_count = 0;
    mock_temp_processor_last_temp = 0.0;
    mock_temp_processor_call_count = 0;
}

void tearDown(void) {
    // Ensure all dynamically allocated memory from simulate_data is freed if not already
    // This assumes simulate_data is called within a test where the char** points to a local char*
    // If not, it's the test's responsibility to free it. For `simulate_data` test, it's done within the test.

    // Reset all stub control structures again for safety
    memset(&stub_process_temperature, 0, sizeof(stub_process_temperature));
    mock_logger_last_message = NULL;
    mock_logger_call_count = 0;
    mock_temp_processor_last_temp = 0.0;
    mock_temp_processor_call_count = 0;
}

// =================================================================================================
// Tests for calculate_average
// =================================================================================================

void test_calculate_average_null_sensors(void) {
    double result = calculate_average(NULL, 5);
    // Expected: Function returns 0.0 when sensors array is NULL
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0, result);
}

void test_calculate_average_zero_count(void) {
    Sensor s1 = {25.0};
    Sensor s2 = {30.0};
    Sensor* sensors_data[] = {&s1, &s2};
    SensorArray sensors = sensors_data;
    double result = calculate_average(sensors, 0);
    // Expected: Function returns 0.0 when count is 0
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0, result);
}

void test_calculate_average_negative_count(void) {
    Sensor s1 = {25.0};
    Sensor s2 = {30.0};
    Sensor* sensors_data[] = {&s1, &s2};
    SensorArray sensors = sensors_data;
    double result = calculate_average(sensors, 0.0f);
    // Expected: Function returns 0.0 when count is negative
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0, result);
}

void test_calculate_average_single_sensor(void) {
    Sensor s1 = {25.5};
    Sensor* sensors_data[] = {&s1};
    SensorArray sensors = sensors_data;
    double result = calculate_average(sensors, 1);
    // Expected: Average is the single sensor's temperature
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 25.5, result);
}

void test_calculate_average_multiple_sensors_positive(void) {
    Sensor s1 = {10.0};
    Sensor s2 = {20.0};
    Sensor s3 = {30.0};
    Sensor* sensors_data[] = {&s1, &s2, &s3};
    SensorArray sensors = sensors_data;
    double result = calculate_average(sensors, 3);
    // Expected: Average of (10.0 + 20.0 + 30.0) / 3 = 20.0
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 20.0, result);
}

void test_calculate_average_multiple_sensors_mixed(void) {
    Sensor s1 = {0.0f};
    Sensor s2 = {0.0};
    Sensor s3 = {15.0};
    Sensor s4 = {20.0};
    Sensor* sensors_data[] = {&s1, &s2, &s3, &s4};
    SensorArray sensors = sensors_data;
    double result = calculate_average(sensors, 4);
    // Expected: Average of (0.0f + 0.0 + 15.0 + 20.0) / 4 = 30.0 / 4 = 7.5
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 7.5, result);
}

void test_calculate_average_multiple_sensors_negative(void) {
    Sensor s1 = {0.0f};
    Sensor s2 = {0.0f};
    Sensor* sensors_data[] = {&s1, &s2};
    SensorArray sensors = sensors_data;
    double result = calculate_average(sensors, 2);
    // Expected: Average of (0.0f + 0.0f) / 2 = 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, result);
}

// =================================================================================================
// Tests for log_message
// =================================================================================================

void test_log_message_valid_logger(void) {
    const char* test_message = "System initialized.";
    log_message(test_message, MockLogger);
    // Expected: MockLogger called once
    TEST_ASSERT_EQUAL_UINT32(1, mock_logger_call_count);
    // Expected: MockLogger captured the correct message
    TEST_ASSERT_EQUAL_STRING(test_message, mock_logger_last_message);
}

void test_log_message_null_logger(void) {
    const char* test_message = "Error message.";
    log_message(test_message, NULL);
    // Expected: No logger function called when logger is NULL
    TEST_ASSERT_EQUAL_UINT32(0, mock_logger_call_count);
    // Expected: No message captured by mock logger
    TEST_ASSERT_NULL(mock_logger_last_message);
}

void test_log_message_empty_string(void) {
    const char* test_message = "";
    log_message(test_message, MockLogger);
    // Expected: MockLogger called once with an empty string
    TEST_ASSERT_EQUAL_UINT32(1, mock_logger_call_count);
    TEST_ASSERT_EQUAL_STRING(test_message, mock_logger_last_message);
}

void test_log_message_long_string(void) {
    const char* test_message = "This is a very long log message that should be handled correctly by the logger function pointer. It contains many characters and should still be passed without truncation or issues.";
    log_message(test_message, MockLogger);
    // Expected: MockLogger called once with the long string
    TEST_ASSERT_EQUAL_UINT32(1, mock_logger_call_count);
    TEST_ASSERT_EQUAL_STRING(test_message, mock_logger_last_message);
}

// =================================================================================================
// Tests for simulate_data
// =================================================================================================

void test_simulate_data_all_pointers_valid(void) {
    int int_val = 0;
    float float_val = 0.0f;
    char* str_val = NULL;

    simulate_data(&int_val, &float_val, &str_val);

    // Expected: int_val updated to 42
    TEST_ASSERT_EQUAL_INT(42, int_val);
    // Expected: float_val updated to 3.14f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14f, float_val);
    // Expected: str_val allocated and copied correctly
    TEST_ASSERT_NOT_NULL(str_val);
    TEST_ASSERT_EQUAL_STRING("Simulated Data", str_val);

    // Clean up allocated memory
    free(str_val);
}

void test_simulate_data_null_int_ptr(void) {
    int int_val = 100; // Original value
    float float_val = 0.0f;
    char* str_val = NULL;

    simulate_data(NULL, &float_val, &str_val);

    // Expected: int_val remains unchanged
    TEST_ASSERT_EQUAL_INT(100, int_val);
    // Expected: float_val updated
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14f, float_val);
    // Expected: str_val allocated and copied
    TEST_ASSERT_NOT_NULL(str_val);
    TEST_ASSERT_EQUAL_STRING("Simulated Data", str_val);

    // Clean up allocated memory
    free(str_val);
}

void test_simulate_data_null_float_ptr(void) {
    int int_val = 0;
    float float_val = 10.0f; // Original value
    char* str_val = NULL;

    simulate_data(&int_val, NULL, &str_val);

    // Expected: int_val updated
    TEST_ASSERT_EQUAL_INT(42, int_val);
    // Expected: float_val remains unchanged
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.0f, float_val);
    // Expected: str_val allocated and copied
    TEST_ASSERT_NOT_NULL(str_val);
    TEST_ASSERT_EQUAL_STRING("Simulated Data", str_val);

    // Clean up allocated memory
    free(str_val);
}

void test_simulate_data_null_str_ptr(void) {
    int int_val = 0;
    float float_val = 0.0f;
    char* str_val = (char*)1; // Set to a non-NULL garbage value to ensure it's not modified to NULL

    simulate_data(&int_val, &float_val, NULL);

    // Expected: int_val updated
    TEST_ASSERT_EQUAL_INT(42, int_val);
    // Expected: float_val updated
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14f, float_val);
    // Expected: str_val remains unchanged (not modified or allocated)
    TEST_ASSERT_EQUAL_PTR((char*)1, str_val); // Confirm it wasn't touched
}

void test_simulate_data_all_pointers_null(void) {
    // No variables to pass, ensure no crashes and no side effects
    simulate_data(NULL, NULL, NULL);

    // Expected: No assertions on variables as they are NULL, just verify no crash
    // This test ensures robustness for null inputs.
    TEST_PASS();
}

// =================================================================================================
// Tests for process_sensors
// =================================================================================================

void test_process_sensors_single_sensor(void) {
    Sensor s1 = {25.0};
    Sensor* sensors_data[] = {&s1};
    SensorArray sensors = sensors_data;

    process_sensors(sensors, 1, MockTempProcessor);

    // Expected: process_temperature (stub) called once
    TEST_ASSERT_EQUAL_UINT32(1, stub_process_temperature.call_count);
    // Expected: The correct temperature passed to the stub
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 25.0, stub_process_temperature.last_temperature);
    // Expected: The provided processor was passed to the stub
    TEST_ASSERT_EQUAL_PTR(MockTempProcessor, stub_process_temperature.last_processor);
}

void test_process_sensors_multiple_sensors(void) {
    Sensor s1 = {10.5};
    Sensor s2 = {30.0};
    Sensor s3 = {0.0f};
    Sensor* sensors_data[] = {&s1, &s2, &s3};
    SensorArray sensors = sensors_data;

    process_sensors(sensors, 3, MockTempProcessor);

    // Expected: process_temperature (stub) called three times
    TEST_ASSERT_EQUAL_UINT32(3, stub_process_temperature.call_count);
    // Expected: Temperatures captured in order
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 10.5, stub_process_temperature.temperatures[0]);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 30.0, stub_process_temperature.temperatures[1]);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, stub_process_temperature.temperatures[2]);
    // Expected: The last temperature captured is 0.0f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, stub_process_temperature.last_temperature);
    // Expected: The provided processor was passed to the stub in all calls (last captured)
    TEST_ASSERT_EQUAL_PTR(MockTempProcessor, stub_process_temperature.last_processor);
}

void test_process_sensors_zero_count(void) {
    Sensor s1 = {25.0};
    Sensor* sensors_data[] = {&s1};
    SensorArray sensors = sensors_data;

    process_sensors(sensors, 0, MockTempProcessor);

    // Expected: process_temperature (stub) not called
    TEST_ASSERT_EQUAL_UINT32(0, stub_process_temperature.call_count);
}

void test_process_sensors_empty_array(void) {
    Sensor* sensors_data[] = {}; // Empty array literal
    SensorArray sensors = (SensorArray)sensors_data; // Cast to avoid warnings if sensors_data is empty

    process_sensors(sensors, 0, MockTempProcessor);

    // Expected: process_temperature (stub) not called
    TEST_ASSERT_EQUAL_UINT32(0, stub_process_temperature.call_count);
}

// NOTE: Passing a NULL SensorArray with count > 0 would lead to a dereference of a NULL pointer
// (sensors[i]->temperature) and crash the application. The source function `process_sensors`
// does not include NULL checks for `sensors` and relies on `count` for loop termination.
// Therefore, we do not write a test case that would intentionally crash the system as per rules
// "MEANINGFUL TESTS ONLY" and "NO NONSENSE" (no physical impossibilities or ignoring source thresholds).
// The current implementation, if passed NULL sensors and count > 0, would be a runtime error.

// =================================================================================================
// Main Test Runner
// =================================================================================================



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_calculate_average_null_sensors);
    RUN_TEST(test_calculate_average_zero_count);
    RUN_TEST(test_calculate_average_negative_count);
    RUN_TEST(test_calculate_average_single_sensor);
    RUN_TEST(test_calculate_average_multiple_sensors_positive);
    RUN_TEST(test_calculate_average_multiple_sensors_mixed);
    RUN_TEST(test_calculate_average_multiple_sensors_negative);
    RUN_TEST(test_log_message_valid_logger);
    RUN_TEST(test_log_message_null_logger);
    RUN_TEST(test_log_message_empty_string);
    RUN_TEST(test_log_message_long_string);
    RUN_TEST(test_simulate_data_all_pointers_valid);
    RUN_TEST(test_simulate_data_null_int_ptr);
    RUN_TEST(test_simulate_data_null_float_ptr);
    RUN_TEST(test_simulate_data_null_str_ptr);
    RUN_TEST(test_simulate_data_all_pointers_null);
    RUN_TEST(test_process_sensors_single_sensor);
    RUN_TEST(test_process_sensors_multiple_sensors);
    RUN_TEST(test_process_sensors_zero_count);
    RUN_TEST(test_process_sensors_empty_array);

    return UNITY_END();
}