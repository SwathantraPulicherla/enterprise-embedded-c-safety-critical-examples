/* test_sensor.c – Auto-generated Expert Unity Tests */
#include "unity.h"
#include <stdlib.h> // For malloc, free
#include <string.h> // For strncpy, memset
#include <stdio.h>  // For printf (not stubbed, but contextual)
#include <stdbool.h> // For bool type in stubs
#include <stdint.h>  // For uint32_t in stubs

// Include the header for the module under test to get struct definitions, etc.
// This assumes sensor.h is available in the include path and defines Sensor,
// SensorStatus enum, and TempProcessor function pointer.
#include "sensor.h"

// ====================================================================
// Mock/Stub Definitions for External Dependencies
//
// As per rules: ONLY for listed external dependencies.
// These stubs allow control over external function behavior and capture calls.
// ====================================================================

// --- Malloc Stub ---
typedef struct {
    void* actual_allocated_ptr; // Stores the real pointer returned by calloc
    uint32_t call_count;
    size_t last_size;
    bool should_fail; // Configuration to simulate malloc failure
} malloc_stub_t;
static malloc_stub_t malloc_stub = {0};

// Mock function for malloc. Redirects calls via #define.
void* mock_malloc(size_t size) {
    malloc_stub.call_count++;
    malloc_stub.last_size = size;
    if (malloc_stub.should_fail) {
        malloc_stub.actual_allocated_ptr = NULL; // No allocation on failure
        return NULL;
    }
    // Allocate real memory using calloc to ensure it's zero-initialized
    // This allows subsequent tests on the allocated Sensor object to be valid.
    malloc_stub.actual_allocated_ptr = calloc(1, size);
    return malloc_stub.actual_allocated_ptr;
}
#define malloc mock_malloc // Redirect malloc to our mock

// --- Free Stub ---
typedef struct {
    uint32_t call_count;
    void* last_ptr; // Captures the pointer passed to free
} free_stub_t;
static free_stub_t free_stub = {0};

// Mock function for free. Redirects calls via #define.
void mock_free(void* ptr) {
    free_stub.call_count++;
    free_stub.last_ptr = ptr;
    if (ptr != NULL) {
        // Free the real memory that was allocated by mock_malloc
        free(ptr);
    }
}
#define free mock_free // Redirect free to our mock

// --- Strncpy Stub ---
typedef struct {
    uint32_t call_count;
    char* captured_dest;
    const char* captured_src;
    size_t captured_n;
} strncpy_stub_t;
static strncpy_stub_t strncpy_stub = {0};

// Mock function for strncpy. Redirects calls via #define.
char* mock_strncpy(char* dest, const char* src, size_t n) {
    strncpy_stub.call_count++;
    strncpy_stub.captured_dest = dest;
    strncpy_stub.captured_src = src;
    strncpy_stub.captured_n = n;
    return strncpy(dest, src, n); // Call the real strncpy for actual string manipulation
}
#define strncpy mock_strncpy // Redirect strncpy to our mock

// --- TempProcessor Stub ---
// This is for the function pointer passed to process_temperature.
typedef struct {
    uint32_t call_count;
    float last_temp; // Captures the temperature passed to the processor
} temp_processor_stub_t;
static temp_processor_stub_t temp_processor_stub = {0};

// Mock function for the TempProcessor function pointer type.
void mock_temp_processor(float temp) {
    temp_processor_stub.call_count++;
    temp_processor_stub.last_temp = temp;
}

// NOTE: printf is NOT stubbed as per absolute mandate.
// Any output from printf will go to stdout during test execution.
// Assertions on its output or call count are not possible in this framework setup.

// ====================================================================
// Test Setup and Teardown
//
// Ensure complete isolation between tests by resetting all stubs and
// test state before and after each test.
// ====================================================================

void setUp(void) {
    // Reset all stub control structures to their default (zero-initialized) state
    // This ensures no state from a previous test pollutes the current test.
    memset(&malloc_stub, 0, sizeof(malloc_stub));
    memset(&free_stub, 0, sizeof(free_stub));
    memset(&strncpy_stub, 0, sizeof(strncpy_stub));
    memset(&temp_processor_stub, 0, sizeof(temp_processor_stub));

    // Configure default stub behaviors for the upcoming test
    malloc_stub.should_fail = false; // Malloc succeeds by default
}

void tearDown(void) {
    // Reset all stub control structures again after each test
    // This is a robust practice to ensure cleanup even if a test fails midway.
    memset(&malloc_stub, 0, sizeof(malloc_stub));
    memset(&free_stub, 0, sizeof(free_stub));
    memset(&strncpy_stub, 0, sizeof(strncpy_stub));
    memset(&temp_processor_stub, 0, sizeof(temp_processor_stub));
}

// ====================================================================
// Test Cases for create_sensor(int id, const char* name)
// ====================================================================

// Test creating a sensor with valid, mid-range ID and a typical name.
void test_create_sensor_normal_valid(void) {
    int id = 101;
    const char* name = "Engine Temp Sensor";
    Sensor* sensor = create_sensor(id, name);

    // Expected: Sensor pointer is not NULL, indicating successful allocation.
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: Malloc was called once to allocate the Sensor struct.
    TEST_ASSERT_EQUAL_UINT32(1, malloc_stub.call_count);
    // Expected: Malloc was called with the correct size for a Sensor struct.
    TEST_ASSERT_EQUAL_UINT32(sizeof(Sensor), malloc_stub.last_size);
    // Expected: The ID member is correctly assigned.
    TEST_ASSERT_EQUAL_INT(id, sensor->id);
    // Expected: The name string is correctly copied and null-terminated.
    TEST_ASSERT_EQUAL_STRING(name, sensor->name);
    // Expected: Strncpy was called once during name copying.
    TEST_ASSERT_EQUAL_UINT32(1, strncpy_stub.call_count);
    // Expected: Default temperature is initialized to 0.0f.
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sensor->temperature);
    // Expected: Default status is STATUS_OK.
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union member float_value is initialized to 0.0f.
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, sensor->data.float_value);

    free_sensor(sensor); // Clean up allocated memory after the test
}

// Test creating a sensor with a name that is exactly the maximum allowed length.
void test_create_sensor_max_len_name(void) {
    int id = 102;
    // Determine the max name length based on the Sensor struct's name array size.
    Sensor temp_sensor_for_size; // Temporary instance to get sizeof() member
    size_t max_name_len = sizeof(temp_sensor_for_size.name) - 1; // Subtract 1 for null terminator

    char long_name[max_name_len + 1]; // +1 for the actual null terminator for the string literal
    memset(long_name, 'X', max_name_len); // Fill with characters
    long_name[max_name_len] = '\0'; // Null-terminate the input string

    Sensor* sensor = create_sensor(id, long_name);

    // Expected: Sensor is successfully created.
    TEST_ASSERT_NOT_NULL(sensor);
    // Expected: The name is copied exactly as provided and null-terminated.
    TEST_ASSERT_EQUAL_STRING(long_name, sensor->name);
    // Expected: The last character in the buffer (before the explicit null) is the last character of the input string.
    TEST_ASSERT_EQUAL_CHAR('X', sensor->name[max_name_len - 1]);
    // Expected: The explicit null-terminator is correctly placed at the end of the buffer.
    TEST_ASSERT_EQUAL_CHAR('\0', sensor->name[max_name_len]);

    free_sensor(sensor);
}

// Test creating a sensor with a name longer than the maximum allowed length, ensuring truncation.
void test_create_sensor_name_truncation(void) {
    int id = 103;
    Sensor temp_sensor_for_size;
    size_t max_name_len = sizeof(temp_sensor_for_size.name) - 1;

    char very_long_name[max_name_len + 10]; // Input string is longer than the buffer
    memset(very_long_name, 'Y', sizeof(very_long_name) - 1); // Fill with characters
    very_long_name[sizeof(very_long_name) - 1] = '\0'; // Null-terminate the input string

    Sensor* sensor = create_sensor(id, very_long_name);

    // Expected: Sensor is successfully created.
    TEST_ASSERT_NOT_NULL(sensor);

    char expected_name_buffer[max_name_len + 1]; // Buffer for the expected truncated string
    strncpy(expected_name_buffer, very_long_name, max_name_len); // Manually truncate for comparison
    expected_name_buffer[max_name_len] = '\0'; // Explicitly null-terminate

    // Expected: The name in the sensor is truncated and matches the expected truncated string.
    TEST_ASSERT_EQUAL_STRING(expected_name_buffer, sensor->name);
    // Expected: The last position of the buffer (max_name_len index) contains the null terminator.
    TEST_ASSERT_EQUAL_CHAR('\0', sensor->name[max_name_len]);

    free_sensor(sensor);
}

// Test `create_sensor` behavior when `malloc` fails to allocate memory.
void test_create_sensor_malloc_failure(void) {
    malloc_stub.should_fail = true; // Configure the mock malloc to return NULL
    Sensor* sensor = create_sensor(104, "Failure Sensor");

    // Expected: `create_sensor` returns NULL when malloc fails.
    TEST_ASSERT_NULL(sensor);
    // Expected: Malloc was still attempted once.
    TEST_ASSERT_EQUAL_UINT32(1, malloc_stub.call_count);
    // Expected: Strncpy and other initializations were NOT called/attempted on a NULL sensor.
    TEST_ASSERT_EQUAL_UINT32(0, strncpy_stub.call_count);
}

// ====================================================================
// Test Cases for update_temperature(Sensor* sensor, float temp)
// ====================================================================

// Test updating temperature with a value well within the 'OK' range (25.0f).
void test_update_temperature_normal_ok(void) {
    Sensor* sensor = create_sensor(201, "Normal Temp Sensor");
    TEST_ASSERT_NOT_NULL(sensor);

    float temp = 25.0f; // Below 30.0f threshold
    update_temperature(sensor, temp);

    // Expected: Temperature member is updated to the new value.
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp, sensor->temperature);
    // Expected: Status remains STATUS_OK as temp <= 30.0f.
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    // Expected: Union float_value also updated to the new temperature.
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp, sensor->data.float_value);

    free_sensor(sensor);
}

// Test updating temperature exactly at the warning threshold (30.0f).
void test_update_temperature_warning_boundary_exact_30(void) {
    Sensor* sensor = create_sensor(202, "Boundary Temp Sensor");
    TEST_ASSERT_NOT_NULL(sensor);

    float temp = 30.0f; // Not strictly greater than 30.0f
    update_temperature(sensor, temp);

    // Expected: Temperature updated.
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp, sensor->temperature);
    // Expected: Status is STATUS_OK because `temp > 30.0f` condition is false.
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp, sensor->data.float_value);

    free_sensor(sensor);
}

// Test updating temperature just above the warning threshold (30.1f).
void test_update_temperature_warning_boundary_above_30(void) {
    Sensor* sensor = create_sensor(203, "Warning Temp Sensor");
    TEST_ASSERT_NOT_NULL(sensor);

    float temp = 30.1f; // Strictly greater than 30.0f
    update_temperature(sensor, temp);

    // Expected: Temperature updated.
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp, sensor->temperature);
    // Expected: Status is STATUS_WARNING because `temp > 30.0f` condition is true.
    TEST_ASSERT_EQUAL_INT(STATUS_WARNING, sensor->status);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp, sensor->data.float_value);

    free_sensor(sensor);
}

// Test updating temperature at the error threshold (40.0f) and expose the bug.
// CRITICAL BUG EXPOSURE: The `else if (temp > 40.0f)` branch in the source code
// is unreachable due to the preceding `if (temp > 30.0f)`.
// Temperatures like 40.0f will satisfy `temp > 30.0f` first, setting status to WARNING.
void test_update_temperature_error_boundary_exact_40_bug(void) {
    Sensor* sensor = create_sensor(204, "Buggy Error Temp Sensor");
    TEST_ASSERT_NOT_NULL(sensor);

    float temp = 40.0f; // Logically should lead to ERROR if intended, but hits WARNING.
    update_temperature(sensor, temp);

    // Expected: Temperature updated.
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp, sensor->temperature);
    // Expected (due to bug): Status is STATUS_WARNING, NOT STATUS_ERROR.
    // This assertion confirms the current (buggy) behavior of the source.
    TEST_ASSERT_EQUAL_INT(STATUS_WARNING, sensor->status);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp, sensor->data.float_value);

    free_sensor(sensor);
}

// Test updating temperature just above the error threshold (40.1f) and expose the bug.
// CRITICAL BUG EXPOSURE: Same logic applies as `test_update_temperature_error_boundary_exact_40_bug`.
void test_update_temperature_error_boundary_above_40_bug(void) {
    Sensor* sensor = create_sensor(205, "Buggy Error Temp Sensor 2");
    TEST_ASSERT_NOT_NULL(sensor);

    float temp = 40.1f; // Logically should lead to ERROR, but hits WARNING.
    update_temperature(sensor, temp);

    // Expected: Temperature updated.
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp, sensor->temperature);
    // Expected (due to bug): Status is STATUS_WARNING, NOT STATUS_ERROR.
    TEST_ASSERT_EQUAL_INT(STATUS_WARNING, sensor->status);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp, sensor->data.float_value);

    free_sensor(sensor);
}

// Test updating temperature with a low, realistic negative value (0.0f).
void test_update_temperature_low_value(void) {
    Sensor* sensor = create_sensor(206, "Cold Temp Sensor");
    TEST_ASSERT_NOT_NULL(sensor);

    float temp = 0.0f; // Not greater than 30.0f
    update_temperature(sensor, temp);

    // Expected: Temperature updated.
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp, sensor->temperature);
    // Expected: Status is STATUS_OK.
    TEST_ASSERT_EQUAL_INT(STATUS_OK, sensor->status);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp, sensor->data.float_value);

    free_sensor(sensor);
}

// Test calling `update_temperature` with a NULL sensor pointer.
// Should handle gracefully and not crash.
void test_update_temperature_null_sensor(void) {
    // Record initial stub states to confirm no unexpected interactions.
    uint32_t initial_malloc_calls = malloc_stub.call_count;
    uint32_t initial_free_calls = free_stub.call_count;

    // Call with NULL pointer.
    update_temperature(NULL, 25.0f);

    // Expected: No crashes.
    // Expected: No side effects on memory allocation/deallocation (stubs not called).
    TEST_ASSERT_EQUAL_UINT32(initial_malloc_calls, malloc_stub.call_count);
    TEST_ASSERT_EQUAL_UINT32(initial_free_calls, free_stub.call_count);
    // Cannot assert on internal state of a NULL sensor object.
}

// ====================================================================
// Test Cases for print_sensor(const Sensor* sensor)
// ====================================================================

// Test printing a valid sensor object.
// As `printf` is not stubbed, this test primarily checks for crashes and ensures
// the function can be called with valid data. Output will go to stdout.
void test_print_sensor_valid(void) {
    Sensor* sensor = create_sensor(301, "Print Sensor");
    TEST_ASSERT_NOT_NULL(sensor);
    update_temperature(sensor, 28.5f); // Set some values for printing

    // Expected: Function executes without crashing.
    print_sensor(sensor);
    // No direct assertions on `printf` output possible here as per rules.
    // Manual inspection of stdout during test run would confirm output format.

    free_sensor(sensor);
}

// Test printing a NULL sensor pointer.
// Should handle gracefully by returning early and not crashing.
void test_print_sensor_null(void) {
    // Expected: Function executes without crashing.
    print_sensor(NULL);
    // Expected: No `printf` calls would occur (due to the `if (sensor == NULL)` check).
    // No direct assertions on `printf` output possible here.
}

// ====================================================================
// Test Cases for process_temperature(float temp, TempProcessor processor)
// ====================================================================

// Test processing temperature with a valid function pointer callback.
void test_process_temperature_valid_processor(void) {
    float temp_to_process = 35.5f;

    // Call process_temperature, passing our mock function as the processor.
    process_temperature(temp_to_process, mock_temp_processor);

    // Expected: The mock processor function was called exactly once.
    TEST_ASSERT_EQUAL_UINT32(1, temp_processor_stub.call_count);
    // Expected: The mock processor received the correct temperature value.
    TEST_ASSERT_FLOAT_WITHIN(0.001f, temp_to_process, temp_processor_stub.last_temp);
}

// Test processing temperature with a NULL function pointer callback.
// Should handle gracefully by not attempting to dereference NULL and not crashing.
void test_process_temperature_null_processor(void) {
    float temp_to_process = 20.0f;

    // Call process_temperature with a NULL function pointer.
    process_temperature(temp_to_process, NULL);

    // Expected: The mock processor was NOT called (because the input was NULL).
    TEST_ASSERT_EQUAL_UINT32(0, temp_processor_stub.call_count);
    // Expected: No crash, function returns immediately due to `if (processor != NULL)` check.
}

// ====================================================================
// Test Cases for free_sensor(Sensor* sensor)
// ====================================================================

// Test freeing a valid sensor object.
void test_free_sensor_valid(void) {
    Sensor* sensor = create_sensor(401, "Sensor to Free");
    TEST_ASSERT_NOT_NULL(sensor);
    TEST_ASSERT_EQUAL_UINT32(1, malloc_stub.call_count); // Ensure allocation happened

    // Capture the pointer that was originally returned by mock_malloc for comparison.
    void* allocated_ptr = malloc_stub.actual_allocated_ptr;

    free_sensor(sensor);

    // Expected: The `free` mock was called exactly once.
    TEST_ASSERT_EQUAL_UINT32(1, free_stub.call_count);
    // Expected: The `free` mock was called with the exact pointer that was originally allocated.
    TEST_ASSERT_EQUAL_PTR(allocated_ptr, free_stub.last_ptr);
}

// Test freeing a NULL sensor pointer.
// Should handle gracefully by returning early and not attempting to free NULL.
void test_free_sensor_null(void) {
    // Expected: The `free` mock was NOT called (due to the `if (sensor != NULL)` check).
    free_sensor(NULL);
    TEST_ASSERT_EQUAL_UINT32(0, free_stub.call_count);
}

// ====================================================================
// Main Test Runner
//
// This function aggregates all test cases and runs them using Unity.
// ====================================================================



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_create_sensor_normal_valid);
    RUN_TEST(test_create_sensor_max_len_name);
    RUN_TEST(test_create_sensor_name_truncation);
    RUN_TEST(test_create_sensor_malloc_failure);
    RUN_TEST(test_update_temperature_normal_ok);
    RUN_TEST(test_update_temperature_warning_boundary_exact_30);
    RUN_TEST(test_update_temperature_warning_boundary_above_30);
    RUN_TEST(test_update_temperature_error_boundary_exact_40_bug);
    RUN_TEST(test_update_temperature_error_boundary_above_40_bug);
    RUN_TEST(test_update_temperature_low_value);
    RUN_TEST(test_update_temperature_null_sensor);
    RUN_TEST(test_print_sensor_valid);
    RUN_TEST(test_print_sensor_null);
    RUN_TEST(test_process_temperature_valid_processor);
    RUN_TEST(test_process_temperature_null_processor);
    RUN_TEST(test_free_sensor_valid);
    RUN_TEST(test_free_sensor_null);

    return UNITY_END();
}