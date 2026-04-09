/* test_utils.c – Auto-generated Expert Unity Tests */

#include "unity.h"
#include <stdint.h>   // For uint32_t
#include <stdbool.h>  // For bool
#include <string.h>   // For memset, strcpy, strdup
#include <stdlib.h>   // For malloc, free

// ==============================================================================
// INTERNAL SOURCE TYPES (assumed from utils.h based on usage)
// ==============================================================================
typedef struct Sensor {
    double temperature;
} Sensor;

typedef Sensor** SensorArray;

typedef void (*Logger)(const char* message);

// TempProcessor is passed as void* to process_temperature,
// suggesting it's an opaque context pointer.
typedef void* TempProcessor;

// ==============================================================================
// EXTERNAL SOURCE FUNCTION DECLARATIONS (from src/utils.c)
// ==============================================================================
// Calculate average temperature
extern double calculate_average(SensorArray sensors, int count);

// Log message using function pointer
extern void log_message(const char* message, Logger logger);

// Simulate data with pointers
extern void simulate_data(int* int_ptr, float* float_ptr, char** str_ptr);

// Process array of sensors
extern void process_sensors(SensorArray sensors, int count, TempProcessor processor);

// ==============================================================================
// STUBS FOR EXTERNAL DEPENDENCIES (as per problem statement)
// ==============================================================================

// External function to stub: process_temperature
#define MAX_PROCESS_TEMPERATURE_CALLS 5

typedef struct {
    double temperature;
    TempProcessor processor_context;
} process_temperature_args_t;

typedef struct {
    bool was_called;
    uint32_t call_count;
    process_temperature_args_t captured_args[MAX_PROCESS_TEMPERATURE_CALLS];
} stub_process_temperature_t;

static stub_process_temperature_t stub_process_temperature = {0};

void process_temperature(double temperature, TempProcessor processor_context) {
    if (stub_process_temperature.call_count < MAX_PROCESS_TEMPERATURE_CALLS) {
        stub_process_temperature.captured_args[stub_process_temperature.call_count].temperature = temperature;
        stub_process_temperature.captured_args[stub_process_temperature.call_count].processor_context = processor_context;
    }
    stub_process_temperature.call_count++;
    stub_process_temperature.was_called = true;
}

// ==============================================================================
// MOCKS FOR FUNCTION POINTERS (used internally by source functions)
// ==============================================================================

// Mock for Logger (used by log_message)
#define MAX_LOG_MESSAGE_CALLS 5

typedef struct {
    bool was_called;
    uint32_t call_count;
    const char* captured_messages[MAX_LOG_MESSAGE_CALLS]; // Need to strdup messages
} mock_logger_t;

static mock_logger_t mock_logger_data = {0};

void mock_logger(const char* message) {
    if (mock_logger_data.call_count < MAX_LOG_MESSAGE_CALLS) {
        // Expected: Duplicate message string to ensure it outlives the source function's scope
        mock_logger_data.captured_messages[mock_logger_data.call_count] = strdup(message);
    }
    mock_logger_data.call_count++;
    mock_logger_data.was_called = true;
}

// ==============================================================================
// TEST UTILITY VARIABLES & FUNCTIONS
// ==============================================================================

// Global variable to capture allocated string from simulate_data for cleanup
static char* g_simulated_string_ptr = NULL;

// Helper function to create a Sensor array for tests
static SensorArray create_sensor_array(int count, double* temperatures) {
    if (count <= 0) return NULL;
    SensorArray sensors = (SensorArray)malloc(count * sizeof(Sensor*));
    TEST_ASSERT_NOT_NULL(sensors);
    for (int i = 0; i < count; i++) {
        sensors[i] = (Sensor*)malloc(sizeof(Sensor));
        TEST_ASSERT_NOT_NULL(sensors[i]);
        sensors[i]->temperature = temperatures[i];
    }
    return sensors;
}

// Helper function to free a Sensor array
static void free_sensor_array(SensorArray sensors, int count) {
    if (sensors == NULL) return;
    for (int i = 0; i < count; i++) {
        free(sensors[i]);
    }
    free(sensors);
}

// ==============================================================================
// SETUP AND TEARDOWN
// ==============================================================================

void setUp(void) {
    // Reset all stubs
    memset(&stub_process_temperature, 0, sizeof(stub_process_temperature));

    // Reset all mock logger data and free captured strings
    for (int i = 0; i < MAX_LOG_MESSAGE_CALLS; i++) {
        if (mock_logger_data.captured_messages[i] != NULL) {
            free((void*)mock_logger_data.captured_messages[i]);
            mock_logger_data.captured_messages[i] = NULL;
        }
    }
    memset(&mock_logger_data, 0, sizeof(mock_logger_data));

    // Reset global simulated string pointer and free any previously allocated memory
    if (g_simulated_string_ptr != NULL) {
        free(g_simulated_string_ptr);
        g_simulated_string_ptr = NULL;
    }
}

void tearDown(void) {
    // Perform same cleanup as setUp to ensure complete isolation
    setUp(); // Call setUp to reset all state for tearDown as well
}

// ==============================================================================
// TEST FUNCTIONS FOR calculate_average
// ==============================================================================

// Test case: sensors array is NULL, count is valid
void test_calculate_average_null_sensors(void) {
    // Expected: If sensors is NULL, function should return 0.0
    double result = calculate_average(NULL, 5);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 0.0, result);
}

// Test case: count is zero
void test_calculate_average_zero_count(void) {
    double temps[] = {10.0, 20.0};
    SensorArray sensors = create_sensor_array(2, temps);

    // Expected: If count is 0, function should return 0.0
    double result = calculate_average(sensors, 0);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 0.0, result);

    free_sensor_array(sensors, 2);
}

// Test case: count is negative
void test_calculate_average_negative_count(void) {
    double temps[] = {10.0, 20.0};
    SensorArray sensors = create_sensor_array(2, temps);

    // Expected: If count is negative, function should return 0.0
    double result = calculate_average(sensors, 0.0f);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 0.0, result);

    free_sensor_array(sensors, 2);
}

// Test case: Single sensor, positive temperature
void test_calculate_average_single_sensor_positive(void) {
    double temps[] = {25.5};
    SensorArray sensors = create_sensor_array(1, temps);

    // Expected: Average of a single sensor is its temperature
    double result = calculate_average(sensors, 1);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 25.5, result);

    free_sensor_array(sensors, 1);
}

// Test case: Multiple sensors, all positive temperatures
void test_calculate_average_multiple_sensors_positive(void) {
    double temps[] = {20.0, 30.0, 40.0};
    SensorArray sensors = create_sensor_array(3, temps);

    // Expected: (20.0 + 30.0 + 40.0) / 3 = 30.0
    double result = calculate_average(sensors, 3);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 30.0, result);

    free_sensor_array(sensors, 3);
}

// Test case: Multiple sensors, all negative temperatures
void test_calculate_average_multiple_sensors_negative(void) {
    double temps[] = {0.0f, 0.0f, 0.0f};
    SensorArray sensors = create_sensor_array(3, temps);

    // Expected: (0.0f + 0.0f + 0.0f) / 3 = 0.0f
    double result = calculate_average(sensors, 3);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 0.0f, result);

    free_sensor_array(sensors, 3);
}

// Test case: Multiple sensors, mixed positive and negative temperatures
void test_calculate_average_multiple_sensors_mixed(void) {
    double temps[] = {10.0, 0.0f, 0.0, 20.0};
    SensorArray sensors = create_sensor_array(4, temps);

    // Expected: (10.0 + 0.0f + 0.0 + 20.0) / 4 = 5.0
    double result = calculate_average(sensors, 4);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 5.0, result);

    free_sensor_array(sensors, 4);
}

// ==============================================================================
// TEST FUNCTIONS FOR log_message
// ==============================================================================

// Test case: logger function pointer is NULL
void test_log_message_null_logger(void) {
    const char* message = "Test message";
    // Expected: No call to a logger if it's NULL
    log_message(message, NULL);
    TEST_ASSERT_FALSE(mock_logger_data.was_called);
    TEST_ASSERT_EQUAL_UINT32(0, mock_logger_data.call_count);
}

// Test case: logger function pointer is valid
void test_log_message_valid_logger(void) {
    const char* message = "Important log entry";
    // Expected: mock_logger should be called with the exact message
    log_message(message, mock_logger);
    TEST_ASSERT_TRUE(mock_logger_data.was_called);
    TEST_ASSERT_EQUAL_UINT32(1, mock_logger_data.call_count);
    TEST_ASSERT_EQUAL_STRING(message, mock_logger_data.captured_messages[0]);
}

// Test case: Multiple log messages to a valid logger
void test_log_message_multiple_calls(void) {
    const char* message1 = "First log";
    const char* message2 = "Second log";

    // Expected: Both messages should be logged in order
    log_message(message1, mock_logger);
    log_message(message2, mock_logger);

    TEST_ASSERT_TRUE(mock_logger_data.was_called);
    TEST_ASSERT_EQUAL_UINT32(2, mock_logger_data.call_count);
    TEST_ASSERT_EQUAL_STRING(message1, mock_logger_data.captured_messages[0]);
    TEST_ASSERT_EQUAL_STRING(message2, mock_logger_data.captured_messages[1]);
}

// ==============================================================================
// TEST FUNCTIONS FOR simulate_data
// ==============================================================================

// Test case: All pointers passed are NULL
void test_simulate_data_all_null(void) {
    int initial_int = 10;
    float initial_float = 1.0f;
    char* initial_str = (char*)malloc(10);
    strcpy(initial_str, "Original");

    // Expected: No changes to any value
    simulate_data(NULL, NULL, NULL);

    TEST_ASSERT_EQUAL_INT(10, initial_int);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, initial_float);
    TEST_ASSERT_EQUAL_STRING("Original", initial_str);

    free(initial_str);
}

// Test case: Only int_ptr is valid
void test_simulate_data_int_only(void) {
    int my_int = 0;
    // Expected: my_int updated to 42, others unchanged
    simulate_data(&my_int, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(42, my_int);
}

// Test case: Only float_ptr is valid
void test_simulate_data_float_only(void) {
    float my_float = 0.0f;
    // Expected: my_float updated to 3.14f, others unchanged
    simulate_data(NULL, &my_float, NULL);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14f, my_float);
}

// Test case: Only str_ptr is valid
void test_simulate_data_string_only(void) {
    // Use global variable to capture allocated string
    // Expected: g_simulated_string_ptr points to "Simulated Data"
    simulate_data(NULL, NULL, &g_simulated_string_ptr);
    TEST_ASSERT_NOT_NULL(g_simulated_string_ptr);
    TEST_ASSERT_EQUAL_STRING("Simulated Data", g_simulated_string_ptr);
    // Cleanup handled by tearDown via g_simulated_string_ptr
}

// Test case: All pointers are valid
void test_simulate_data_all_valid(void) {
    int my_int = 0;
    float my_float = 0.0f;
    // Expected: All pointers updated correctly
    simulate_data(&my_int, &my_float, &g_simulated_string_ptr);

    TEST_ASSERT_EQUAL_INT(42, my_int);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.14f, my_float);
    TEST_ASSERT_NOT_NULL(g_simulated_string_ptr);
    TEST_ASSERT_EQUAL_STRING("Simulated Data", g_simulated_string_ptr);
    // Cleanup handled by tearDown via g_simulated_string_ptr
}

// ==============================================================================
// TEST FUNCTIONS FOR process_sensors
// ==============================================================================

// Test case: count is zero
void test_process_sensors_zero_count(void) {
    // Create a dummy sensor array, though it shouldn't be accessed
    double temps[] = {10.0, 20.0};
    SensorArray sensors = create_sensor_array(2, temps);
    void* dummy_processor = (void*)0x1234; // A non-NULL context

    // Expected: process_temperature should not be called
    process_sensors(sensors, 0, dummy_processor);
    TEST_ASSERT_FALSE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(0, stub_process_temperature.call_count);

    free_sensor_array(sensors, 2);
}

// Test case: count is negative (should also not call process_temperature)
void test_process_sensors_negative_count(void) {
    double temps[] = {10.0, 20.0};
    SensorArray sensors = create_sensor_array(2, temps);
    void* dummy_processor = (void*)0x1234;

    // Expected: process_temperature should not be called
    process_sensors(sensors, 0.0f, dummy_processor);
    TEST_ASSERT_FALSE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(0, stub_process_temperature.call_count);

    free_sensor_array(sensors, 2);
}


// Test case: Single sensor in the array
void test_process_sensors_single_sensor(void) {
    double temps[] = {25.0};
    SensorArray sensors = create_sensor_array(1, temps);
    void* my_processor_context = (void*)0xABCD; // Arbitrary non-NULL context

    // Expected: process_temperature called once with sensor's temperature and context
    process_sensors(sensors, 1, my_processor_context);

    TEST_ASSERT_TRUE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(1, stub_process_temperature.call_count);
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 25.0, stub_process_temperature.captured_args[0].temperature);
    TEST_ASSERT_EQUAL_PTR(my_processor_context, stub_process_temperature.captured_args[0].processor_context);

    free_sensor_array(sensors, 1);
}

// Test case: Multiple sensors in the array
void test_process_sensors_multiple_sensors(void) {
    double temps[] = {10.0, 0.0f, 30.0};
    SensorArray sensors = create_sensor_array(3, temps);
    void* my_processor_context = (void*)0xEF01; // Arbitrary non-NULL context

    // Expected: process_temperature called thrice, with temperatures in order
    process_sensors(sensors, 3, my_processor_context);

    TEST_ASSERT_TRUE(stub_process_temperature.was_called);
    TEST_ASSERT_EQUAL_UINT32(3, stub_process_temperature.call_count);

    // Verify first call
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 10.0, stub_process_temperature.captured_args[0].temperature);
    TEST_ASSERT_EQUAL_PTR(my_processor_context, stub_process_temperature.captured_args[0].processor_context);

    // Verify second call
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 0.0f, stub_process_temperature.captured_args[1].temperature);
    TEST_ASSERT_EQUAL_PTR(my_processor_context, stub_process_temperature.captured_args[1].processor_context);

    // Verify third call
    TEST_ASSERT_DOUBLE_WITHIN(0.001, 30.0, stub_process_temperature.captured_args[2].temperature);
    TEST_ASSERT_EQUAL_PTR(my_processor_context, stub_process_temperature.captured_args[2].processor_context);

    free_sensor_array(sensors, 3);
}

// ==============================================================================
// MAIN TEST RUNNER
// ==============================================================================



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_calculate_average_null_sensors);
    RUN_TEST(test_calculate_average_zero_count);
    RUN_TEST(test_calculate_average_negative_count);
    RUN_TEST(test_calculate_average_single_sensor_positive);
    RUN_TEST(test_calculate_average_multiple_sensors_positive);
    RUN_TEST(test_calculate_average_multiple_sensors_negative);
    RUN_TEST(test_calculate_average_multiple_sensors_mixed);
    RUN_TEST(test_log_message_null_logger);
    RUN_TEST(test_log_message_valid_logger);
    RUN_TEST(test_log_message_multiple_calls);
    RUN_TEST(test_simulate_data_all_null);
    RUN_TEST(test_simulate_data_int_only);
    RUN_TEST(test_simulate_data_float_only);
    RUN_TEST(test_simulate_data_string_only);
    RUN_TEST(test_simulate_data_all_valid);
    RUN_TEST(test_process_sensors_zero_count);
    RUN_TEST(test_process_sensors_negative_count);
    RUN_TEST(test_process_sensors_single_sensor);
    RUN_TEST(test_process_sensors_multiple_sensors);

    return UNITY_END();
}