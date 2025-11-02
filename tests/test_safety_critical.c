/* test_safety_critical.c – Auto-generated Expert Unity Tests */
#include "unity.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h> // For memset
#include <math.h>   // For fabsf
#include <stdlib.h> // For rand

// Include the source file's header to get type definitions
// This header is assumed to define error_t, sensor_data_t, tmr_sensor_t, voting_method_t,
// watchdog_t, fault_monitor_t, safety_level_t, safety_system_t, power_supply_t, and TMR_SENSOR_COUNT.
#include "safety_critical.h"

// --- Mocks for Callbacks (Function Pointers) ---

// Watchdog callback mock
static bool mock_watchdog_timeout_cb_called = false;
static void *mock_watchdog_timeout_cb_context = NULL;
void mock_watchdog_timeout_callback(void *context) {
    mock_watchdog_timeout_cb_called = true;
    mock_watchdog_timeout_cb_context = context;
}

// Safety System emergency callback mock
static bool mock_emergency_cb_called = false;
static uint16_t mock_emergency_cb_reason = 0;
static void *mock_emergency_cb_context = NULL;
void mock_safety_system_emergency_callback(uint16_t reason, void *context) {
    mock_emergency_cb_called = true;
    mock_emergency_cb_reason = reason;
    mock_emergency_cb_context = context;
}

// --- Setup and Teardown ---
void setUp(void) {
    // Reset all mock callback states for test isolation
    mock_watchdog_timeout_cb_called = false;
    mock_watchdog_timeout_cb_context = NULL;

    mock_emergency_cb_called = false;
    mock_emergency_cb_reason = 0;
    mock_emergency_cb_context = NULL;

    // Note: `rand()` is used in `power_supply_monitor`. As per rules, `rand()` is not
    // listed as an external dependency to stub, and no new headers/functions can be invented.
    // Therefore, `srand()` cannot be called here for determinism of `rand()`, and
    // `power_supply_monitor` tests will assert on the *range* of possible outcomes from `rand()`.
}

void tearDown(void) {
    // Reset all mock callback states again (redundant but good practice for ensuring isolation)
    mock_watchdog_timeout_cb_called = false;
    mock_watchdog_timeout_cb_context = NULL;

    mock_emergency_cb_called = false;
    mock_emergency_cb_reason = 0;
    mock_emergency_cb_context = NULL;
}

// --- Test Cases for sensor_read_redundant ---

void test_sensor_read_redundant_NullSensors_ReturnsInvalidParam(void) {
    float result;
    // Expected: ERROR_INVALID_PARAM due to NULL sensors pointer
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, sensor_read_redundant(NULL, 1, &result));
}

void test_sensor_read_redundant_NullResult_ReturnsInvalidParam(void) {
    sensor_data_t sensors[1] = {{.last_error = ERROR_NONE, .calibrated = true, .temperature = 25.0f}};
    // Expected: ERROR_INVALID_PARAM due to NULL result pointer
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, sensor_read_redundant(sensors, 1, NULL));
}

void test_sensor_read_redundant_ZeroCount_ReturnsInvalidParam(void) {
    sensor_data_t sensors[1] = {{.last_error = ERROR_NONE, .calibrated = true, .temperature = 25.0f}};
    float result;
    // Expected: ERROR_INVALID_PARAM due to count being 0
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, sensor_read_redundant(sensors, 0, &result));
}

void test_sensor_read_redundant_AllValid_ReturnsAverage(void) {
    sensor_data_t sensors[3] = {
        {.last_error = ERROR_NONE, .calibrated = true, .temperature = 20.0f},
        {.last_error = ERROR_NONE, .calibrated = true, .temperature = 25.0f},
        {.last_error = ERROR_NONE, .calibrated = true, .temperature = 30.0f}
    };
    float result;
    // Expected: ERROR_NONE as all inputs are valid
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, sensor_read_redundant(sensors, 3, &result));
    // Expected: Result should be (20+25+30)/3 = 25.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, result);
}

void test_sensor_read_redundant_MixedValidInvalid_ReturnsAverageOfValid(void) {
    sensor_data_t sensors[5] = {
        {.last_error = ERROR_NONE,  .calibrated = true,  .temperature = 10.0f}, // Valid
        {.last_error = ERROR_BUSY,  .calibrated = true,  .temperature = 20.0f}, // Invalid: last_error is not ERROR_NONE
        {.last_error = ERROR_NONE,  .calibrated = false, .temperature = 30.0f}, // Invalid: not calibrated
        {.last_error = ERROR_NONE,  .calibrated = true,  .temperature = 40.0f}, // Valid
        {.last_error = ERROR_NONE,  .calibrated = true,  .temperature = 50.0f}  // Valid
    };
    float result;
    // Expected: ERROR_NONE as some inputs are valid
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, sensor_read_redundant(sensors, 5, &result));
    // Expected: Result should be (10+40+50)/3 = 33.333f (average of valid readings)
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 33.333f, result);
}

void test_sensor_read_redundant_NoValidReadings_ReturnsBusy(void) {
    sensor_data_t sensors[3] = {
        {.last_error = ERROR_BUSY, .calibrated = true, .temperature = 10.0f},
        {.last_error = ERROR_NONE, .calibrated = false, .temperature = 20.0f},
        {.last_error = ERROR_BUSY, .calibrated = false, .temperature = 30.0f}
    };
    float result;
    // Expected: ERROR_BUSY as no sensors meet the criteria for a valid reading
    TEST_ASSERT_EQUAL_INT(ERROR_BUSY, sensor_read_redundant(sensors, 3, &result));
}

// --- Test Cases for tmr_sensor_init ---

void test_tmr_sensor_init_NullTmr_ReturnsInvalidParam(void) {
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    // Expected: ERROR_INVALID_PARAM due to NULL tmr pointer
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, tmr_sensor_init(NULL, ids));
}

void test_tmr_sensor_init_NullSensorIds_ReturnsInvalidParam(void) {
    tmr_sensor_t tmr;
    // Expected: ERROR_INVALID_PARAM due to NULL sensor_ids pointer
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, tmr_sensor_init(&tmr, NULL));
}

void test_tmr_sensor_init_Valid_InitializesCorrectly(void) {
    tmr_sensor_t tmr;
    uint8_t ids[TMR_SENSOR_COUNT] = {10, 20, 30};
    // Expected: ERROR_NONE for valid initialization
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, tmr_sensor_init(&tmr, ids));

    for (uint8_t i = 0; i < TMR_SENSOR_COUNT; i++) {
        // Expected: Sensor ID matches input ID
        TEST_ASSERT_EQUAL_UINT8(ids[i], tmr.sensors[i].sensor_id);
        // Expected: Calibrated flag is set to true
        TEST_ASSERT_TRUE(tmr.sensors[i].calibrated);
        // Expected: Confidence level is initialized to 100
        TEST_ASSERT_EQUAL_UINT8(100, tmr.sensors[i].confidence_level);
        // Expected: Last error is initialized to ERROR_NONE
        TEST_ASSERT_EQUAL_INT(ERROR_NONE, tmr.sensors[i].last_error);
    }
    // Expected: Other fields (voted_value, fault_detected, last_vote_time, faulty_sensor_mask) should be zeroed by memset
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, tmr.voted_value);
    TEST_ASSERT_FALSE(tmr.fault_detected);
    TEST_ASSERT_EQUAL_UINT32(0, tmr.last_vote_time);
    TEST_ASSERT_EQUAL_UINT8(0, tmr.faulty_sensor_mask);
}

// --- Test Cases for tmr_sensor_vote ---

void test_tmr_sensor_vote_NullTmr_ReturnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL tmr pointer
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, tmr_sensor_vote(NULL, VOTE_AVERAGE));
}

void test_tmr_sensor_vote_LessThanTwoValidSensors_ReturnsBusyAndSetsFaultDetected(void) {
    tmr_sensor_t tmr = {0};
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, ids);

    tmr.sensors[0].temperature = 25.0f;
    tmr.sensors[1].last_error = ERROR_BUSY; // Make sensor 1 invalid
    tmr.sensors[2].last_error = ERROR_BUSY; // Make sensor 2 invalid

    // Expected: ERROR_BUSY as only one sensor (sensors[0]) is valid
    TEST_ASSERT_EQUAL_INT(ERROR_BUSY, tmr_sensor_vote(&tmr, VOTE_AVERAGE));
    // Expected: fault_detected should be true
    TEST_ASSERT_TRUE(tmr.fault_detected);
}

void test_tmr_sensor_vote_Majority_SimplifiedVoting(void) {
    tmr_sensor_t tmr = {0};
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, ids);

    tmr.sensors[0].temperature = 10.0f;
    tmr.sensors[1].temperature = 20.0f;
    tmr.sensors[2].temperature = 30.0f;

    // Expected: ERROR_NONE, voted_value is tmr.sensors[0].temperature (10.0f) due to simplified source logic
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_MAJORITY));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 10.0f, tmr.voted_value);
    // Expected: fault_detected should be false
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

void test_tmr_sensor_vote_Average_CalculatesMeanOfValidSensors(void) {
    tmr_sensor_t tmr = {0};
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, ids);

    tmr.sensors[0].temperature = 10.0f;
    tmr.sensors[1].temperature = 20.0f;
    tmr.sensors[2].temperature = 30.0f;

    // Expected: ERROR_NONE, voted_value is (10+20+30)/3 = 20.0f
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_AVERAGE));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 20.0f, tmr.voted_value);
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

void test_tmr_sensor_vote_Average_WithOneInvalidSensor_CalculatesMeanOfRemainingValid(void) {
    tmr_sensor_t tmr = {0};
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, ids);

    tmr.sensors[0].temperature = 10.0f;
    tmr.sensors[1].last_error = ERROR_BUSY; // Invalid sensor
    tmr.sensors[1].temperature = 100.0f; // This value should be ignored
    tmr.sensors[2].temperature = 30.0f;

    // Expected: ERROR_NONE, voted_value is (10+30)/2 = 20.0f
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_AVERAGE));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 20.0f, tmr.voted_value);
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

void test_tmr_sensor_vote_Median_SortsAndPicksMiddleValue(void) {
    tmr_sensor_t tmr = {0};
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, ids);

    tmr.sensors[0].temperature = 30.0f;
    tmr.sensors[1].temperature = 10.0f;
    tmr.sensors[2].temperature = 20.0f;

    // Expected: ERROR_NONE, voted_value is 20.0f (median of 10, 20, 30)
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_MEDIAN));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 20.0f, tmr.voted_value);
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

void test_tmr_sensor_vote_Median_WithOneInvalidSensor_PicksMedianOfRemaining(void) {
    tmr_sensor_t tmr = {0};
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, ids);

    tmr.sensors[0].temperature = 50.0f;
    tmr.sensors[1].last_error = ERROR_BUSY; // Invalid sensor
    tmr.sensors[1].temperature = 10.0f; // This value should be ignored
    tmr.sensors[2].temperature = 20.0f;

    // Expected: ERROR_NONE, valid temperatures are 20.0f, 50.0f. Sorted: 20.0f, 50.0f.
    // valid_count = 2. valid_count/2 = 1. values[1] = 50.0f.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_MEDIAN));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 50.0f, tmr.voted_value);
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

void test_tmr_sensor_vote_Weighted_CalculatesWeightedAverage(void) {
    tmr_sensor_t tmr = {0};
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, ids);

    tmr.sensors[0].temperature = 10.0f; tmr.sensors[0].confidence_level = 50;
    tmr.sensors[1].temperature = 20.0f; tmr.sensors[1].confidence_level = 100;
    tmr.sensors[2].temperature = 30.0f; tmr.sensors[2].confidence_level = 75;

    // Expected: ERROR_NONE, weighted average = ((10*0.5) + (20*1.0) + (30*0.75)) / (0.5+1.0+0.75) = (5 + 20 + 22.5) / 2.25 = 47.5 / 2.25 = 21.111f
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_WEIGHTED));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 21.111f, tmr.voted_value);
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

void test_tmr_sensor_vote_Weighted_WithOneInvalidSensor_CalculatesWeightedAverageOfRemaining(void) {
    tmr_sensor_t tmr = {0};
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, ids);

    tmr.sensors[0].temperature = 10.0f; tmr.sensors[0].confidence_level = 50;
    tmr.sensors[1].last_error = ERROR_BUSY; // Invalid sensor
    tmr.sensors[1].temperature = 100.0f; tmr.sensors[1].confidence_level = 100; // Should be ignored
    tmr.sensors[2].temperature = 30.0f; tmr.sensors[2].confidence_level = 75;

    // Expected: ERROR_NONE, weighted average = ((10*0.5) + (30*0.75)) / (0.5+0.75) = (5 + 22.5) / 1.25 = 27.5 / 1.25 = 22.0f
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_WEIGHTED));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 22.0f, tmr.voted_value);
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

// --- Test Cases for tmr_sensor_check_consistency ---

void test_tmr_sensor_check_consistency_NullTmr_ReturnsFalse(void) {
    // Expected: false due to NULL tmr pointer
    TEST_ASSERT_FALSE(tmr_sensor_check_consistency(NULL, 1.0f));
}

void test_tmr_sensor_check_consistency_AllConsistent_ReturnsTrue(void) {
    tmr_sensor_t tmr = {0};
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, ids);

    tmr.sensors[0].temperature = 25.0f;
    tmr.sensors[1].temperature = 25.5f;
    tmr.sensors[2].temperature = 24.8f;

    // Expected: true as all sensors are within 1.0f tolerance of each other
    TEST_ASSERT_TRUE(tmr_sensor_check_consistency(&tmr, 1.0f));
    // Expected: faulty_sensor_mask should be 0
    TEST_ASSERT_EQUAL_UINT8(0, tmr.faulty_sensor_mask);
}

void test_tmr_sensor_check_consistency_InconsistentSensor1_ReturnsFalseAndSetsMask(void) {
    tmr_sensor_t tmr = {0};
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, ids);

    tmr.sensors[0].temperature = 25.0f;
    tmr.sensors[1].temperature = 28.0f; // Difference from sensors[0] is 3.0f, which is > 2.0f tolerance
    tmr.sensors[2].temperature = 25.1f;

    // Expected: false as sensor 1 is outside 2.0f tolerance
    TEST_ASSERT_FALSE(tmr_sensor_check_consistency(&tmr, 2.0f));
    // Expected: faulty_sensor_mask should indicate sensor 1 is faulty (1 << 1 = 0b00000010)
    TEST_ASSERT_EQUAL_UINT8(0b00000010, tmr.faulty_sensor_mask);
}

void test_tmr_sensor_check_consistency_MultipleInconsistent_ReturnsFalseAndMasksFirst(void) {
    tmr_sensor_t tmr = {0};
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, ids);

    tmr.sensors[0].temperature = 25.0f;
    tmr.sensors[1].temperature = 28.0f; // Inconsistent (3.0f diff)
    tmr.sensors[2].temperature = 22.0f; // Inconsistent (3.0f diff)

    // Expected: false, as first detected inconsistency returns immediately.
    TEST_ASSERT_FALSE(tmr_sensor_check_consistency(&tmr, 2.0f));
    // Expected: faulty_sensor_mask indicates sensor 1 is faulty (1 << 1 = 0b00000010).
    // The current implementation returns on the first inconsistency, so only the first will be masked.
    TEST_ASSERT_EQUAL_UINT8(0b00000010, tmr.faulty_sensor_mask);
}

void test_tmr_sensor_check_consistency_EdgeOfTolerance_ReturnsTrue(void) {
    tmr_sensor_t tmr = {0};
    uint8_t ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, ids);

    tmr.sensors[0].temperature = 25.0f;
    tmr.sensors[1].temperature = 27.0f; // Exactly at positive tolerance limit (+2.0f)
    tmr.sensors[2].temperature = 23.0f; // Exactly at negative tolerance limit (0.0f)

    // Expected: true, as |diff| <= tolerance for all sensors
    TEST_ASSERT_TRUE(tmr_sensor_check_consistency(&tmr, 2.0f));
    TEST_ASSERT_EQUAL_UINT8(0, tmr.faulty_sensor_mask);
}

// --- Test Cases for watchdog_init ---

void test_watchdog_init_NullWd_ReturnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL wd pointer
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, watchdog_init(NULL, 1000));
}

void test_watchdog_init_Valid_InitializesCorrectly(void) {
    watchdog_t wd;
    // Expected: ERROR_NONE for valid initialization
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, watchdog_init(&wd, 500));
    // Expected: timeout_ms matches input
    TEST_ASSERT_EQUAL_UINT32(500, wd.timeout_ms);
    // Expected: counter, last_feed_time are zero, expired is false
    TEST_ASSERT_EQUAL_UINT32(0, wd.counter);
    TEST_ASSERT_EQUAL_UINT32(0, wd.last_feed_time);
    TEST_ASSERT_FALSE(wd.expired);
    // Expected: callback pointers are NULL by default (due to memset in safety_system_init or explicit init, not directly here)
    TEST_ASSERT_NULL(wd.timeout_cb);
    TEST_ASSERT_NULL(wd.callback_context);
}

// --- Test Cases for watchdog_feed ---

void test_watchdog_feed_NullWd_DoesNothing(void) {
    // Calling with NULL should not crash. No state change to assert outside the function.
    watchdog_feed(NULL);
    // This test ensures NULL safety.
}

void test_watchdog_feed_Valid_ResetsTimerAndExpiredFlag(void) {
    watchdog_t wd;
    watchdog_init(&wd, 1000);
    wd.counter = 500; // Simulate time passing
    wd.last_feed_time = 0; // Prior feed time
    wd.expired = true;    // Prior expired state

    watchdog_feed(&wd);
    // Expected: last_feed_time is updated to current counter value (500)
    TEST_ASSERT_EQUAL_UINT32(500, wd.last_feed_time);
    // Expected: expired flag is reset to false
    TEST_ASSERT_FALSE(wd.expired);
}

// --- Test Cases for watchdog_check_expired ---

void test_watchdog_check_expired_NullWd_ReturnsTrue(void) {
    // Expected: true for NULL watchdog as it implies a critical system failure.
    TEST_ASSERT_TRUE(watchdog_check_expired(NULL));
}

void test_watchdog_check_expired_NotExpired_ReturnsFalse(void) {
    watchdog_t wd;
    watchdog_init(&wd, 1000);
    wd.counter = 500; // Current time
    wd.last_feed_time = 0; // Last feed time. 500 < 1000, so not expired.

    // Expected: false as counter has not exceeded timeout
    TEST_ASSERT_FALSE(watchdog_check_expired(&wd));
    // Expected: expired flag remains false
    TEST_ASSERT_FALSE(wd.expired);
    // Expected: callback was not called
    TEST_ASSERT_FALSE(mock_watchdog_timeout_cb_called);
}

void test_watchdog_check_expired_Expired_ReturnsTrueAndCallsCallback(void) {
    watchdog_t wd;
    watchdog_init(&wd, 1000);
    wd.timeout_cb = mock_watchdog_timeout_callback; // Assign mock callback
    wd.callback_context = (void*)0xABCD;

    wd.counter = 1500; // Current time, timeout is 1000ms. 1500 - 0 > 1000.
    wd.last_feed_time = 0;

    // Expected: true as watchdog is expired
    TEST_ASSERT_TRUE(watchdog_check_expired(&wd));
    // Expected: expired flag is true
    TEST_ASSERT_TRUE(wd.expired);
    // Expected: callback was called
    TEST_ASSERT_TRUE(mock_watchdog_timeout_cb_called);
    // Expected: callback context matches
    TEST_ASSERT_EQUAL_PTR((void*)0xABCD, mock_watchdog_timeout_cb_context);
}

void test_watchdog_check_expired_ExpiredMultipleTimesWithoutFeed_CallsCallbackMultipleTimes(void) {
    watchdog_t wd;
    watchdog_init(&wd, 1000);
    wd.timeout_cb = mock_watchdog_timeout_callback;
    wd.callback_context = (void*)0x1234;

    wd.counter = 1500; // First expiration event
    wd.last_feed_time = 0;
    // Expected: First call detects expiration and calls callback
    TEST_ASSERT_TRUE(watchdog_check_expired(&wd));
    TEST_ASSERT_TRUE(wd.expired);
    TEST_ASSERT_TRUE(mock_watchdog_timeout_cb_called);
    mock_watchdog_timeout_cb_called = false; // Reset mock flag for next check

    wd.counter = 2000; // Still expired, no feed in between
    // Expected: Second call also detects expiration and calls callback again as per source logic
    TEST_ASSERT_TRUE(watchdog_check_expired(&wd));
    TEST_ASSERT_TRUE(wd.expired);
    TEST_ASSERT_TRUE(mock_watchdog_timeout_cb_called); // Callback called again
    TEST_ASSERT_EQUAL_PTR((void*)0x1234, mock_watchdog_timeout_cb_context);
}


// --- Test Cases for fault_monitor_init ---

void test_fault_monitor_init_NullFm_ReturnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL fm pointer
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, fault_monitor_init(NULL));
}

void test_fault_monitor_init_Valid_InitializesCorrectly(void) {
    fault_monitor_t fm;
    // Expected: ERROR_NONE for valid initialization
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, fault_monitor_init(&fm));
    // Expected: All fields zeroed by memset
    TEST_ASSERT_EQUAL_UINT32(0, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(0, fm.fault_count);
    TEST_ASSERT_EQUAL_UINT8(0, fm.history_index);
    for (int i = 0; i < 32; i++) {
        TEST_ASSERT_EQUAL_UINT8(0, fm.fault_history[i]);
    }
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, fm.fault_rate);
    TEST_ASSERT_FALSE(fm.system_degraded);
    TEST_ASSERT_FALSE(fm.system_failed);
}

// --- Test Cases for fault_monitor_record_check ---

void test_fault_monitor_record_check_NullFm_DoesNothing(void) {
    // Calling with NULL should not crash. No state change to assert outside the function.
    fault_monitor_record_check(NULL, true);
}

void test_fault_monitor_record_check_Pass_UpdatesStatsCorrectly(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);

    fault_monitor_record_check(&fm, true);
    // Expected: total_checks incremented
    TEST_ASSERT_EQUAL_UINT32(1, fm.total_checks);
    // Expected: fault_count remains 0
    TEST_ASSERT_EQUAL_UINT32(0, fm.fault_count);
    // Expected: fault_rate is 0.0f (0/1)
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, fm.fault_rate);
    // Expected: history[0] is 0
    TEST_ASSERT_EQUAL_UINT8(0, fm.fault_history[0]);
    // Expected: history_index increments
    TEST_ASSERT_EQUAL_UINT8(1, fm.history_index);
    // Expected: system status flags are false (rate 0.0f is not > 0.05f or > 0.20f)
    TEST_ASSERT_FALSE(fm.system_degraded);
    TEST_ASSERT_FALSE(fm.system_failed);
}

void test_fault_monitor_record_check_Fail_UpdatesStatsAndStatus(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);

    fault_monitor_record_check(&fm, false);
    // Expected: total_checks incremented
    TEST_ASSERT_EQUAL_UINT32(1, fm.total_checks);
    // Expected: fault_count incremented
    TEST_ASSERT_EQUAL_UINT32(1, fm.fault_count);
    // Expected: fault_rate is 1.0f (1/1)
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, fm.fault_rate);
    // Expected: history[0] is 1
    TEST_ASSERT_EQUAL_UINT8(1, fm.fault_history[0]);
    // Expected: history_index increments
    TEST_ASSERT_EQUAL_UINT8(1, fm.history_index);
    // Expected: system status flags are true (rate 1.0f is > 0.05f and > 0.20f)
    TEST_ASSERT_TRUE(fm.system_degraded);
    TEST_ASSERT_TRUE(fm.system_failed);
}

void test_fault_monitor_record_check_MixToDegradedState_UpdatesStatus(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);

    // 10 checks, 1 failure => rate 0.1f (degraded but not failed)
    for (int i = 0; i < 9; i++) {
        fault_monitor_record_check(&fm, true); // 9 passes
    }
    fault_monitor_record_check(&fm, false); // 1 failure (total 10 checks, 1 fault)
    // Expected: total_checks = 10, fault_count = 1
    TEST_ASSERT_EQUAL_UINT32(10, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(1, fm.fault_count);
    // Expected: fault_rate = 1/10 = 0.1f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.1f, fm.fault_rate);
    // Expected: system_degraded is true (0.1f > 0.05f)
    TEST_ASSERT_TRUE(fm.system_degraded);
    // Expected: system_failed is false (0.1f is not > 0.20f)
    TEST_ASSERT_FALSE(fm.system_failed);
}

void test_fault_monitor_record_check_MixToFailedState_UpdatesStatus(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);

    // 10 checks, 3 failures => rate 0.3f (failed)
    for (int i = 0; i < 7; i++) {
        fault_monitor_record_check(&fm, true); // 7 passes
    }
    for (int i = 0; i < 3; i++) {
        fault_monitor_record_check(&fm, false); // 3 failures (total 10 checks, 3 faults)
    }
    // Expected: total_checks = 10, fault_count = 3
    TEST_ASSERT_EQUAL_UINT32(10, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(3, fm.fault_count);
    // Expected: fault_rate = 3/10 = 0.3f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.3f, fm.fault_rate);
    // Expected: system_degraded is true (0.3f > 0.05f)
    TEST_ASSERT_TRUE(fm.system_degraded);
    // Expected: system_failed is true (0.3f > 0.20f)
    TEST_ASSERT_TRUE(fm.system_failed);
}

void test_fault_monitor_record_check_HistoryWrapAround_UpdatesCorrectly(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);

    // Fill history buffer with 32 passes
    for (int i = 0; i < 32; i++) {
        fault_monitor_record_check(&fm, true);
    }
    // Expected: history_index should wrap around to 0
    TEST_ASSERT_EQUAL_UINT8(0, fm.history_index);
    TEST_ASSERT_EQUAL_UINT32(32, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(0, fm.fault_count);

    // Record one fault, this should overwrite fault_history[0]
    fault_monitor_record_check(&fm, false);
    // Expected: total_checks = 33, fault_count = 1
    TEST_ASSERT_EQUAL_UINT32(33, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(1, fm.fault_count);
    // Expected: fault_rate = 1/33.0f
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f / 33.0f, fm.fault_rate);
    // Expected: history[0] is 1
    TEST_ASSERT_EQUAL_UINT8(1, fm.fault_history[0]);
    // Expected: history_index increments to 1
    TEST_ASSERT_EQUAL_UINT8(1, fm.history_index);
}


// --- Test Cases for fault_monitor_update_status ---

void test_fault_monitor_update_status_NullFm_DoesNothing(void) {
    // Calling with NULL should not crash. No state change to assert outside the function as it's empty.
    fault_monitor_update_status(NULL);
}

void test_fault_monitor_update_status_ValidFm_NoFunctionalChangeYet(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);
    fm.total_checks = 10;
    fm.fault_count = 1;
    fm.fault_rate = 0.1f;
    fm.system_degraded = true;
    fm.system_failed = false;

    // Call the function
    fault_monitor_update_status(&fm);

    // Expected: No change in state as the function body is empty in the source code
    TEST_ASSERT_EQUAL_UINT32(10, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(1, fm.fault_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.1f, fm.fault_rate);
    TEST_ASSERT_TRUE(fm.system_degraded);
    TEST_ASSERT_FALSE(fm.system_failed);
}


// --- Test Cases for safety_system_init ---

void test_safety_system_init_NullSystem_ReturnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL system pointer
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, safety_system_init(NULL, SIL_1));
}

void test_safety_system_init_Valid_InitializesAllComponents(void) {
    safety_system_t system;
    // Expected: ERROR_NONE for valid initialization
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, safety_system_init(&system, SIL_2));

    // Expected: required_sil matches input
    TEST_ASSERT_EQUAL_INT(SIL_2, system.required_sil);

    // Check temperature_sensors initialization (tmr_sensor_init called internally)
    TEST_ASSERT_EQUAL_UINT8(1, system.temperature_sensors.sensors[0].sensor_id); // First sensor ID
    TEST_ASSERT_TRUE(system.temperature_sensors.sensors[0].calibrated);
    TEST_ASSERT_EQUAL_UINT8(100, system.temperature_sensors.sensors[0].confidence_level);
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, system.temperature_sensors.sensors[0].last_error);

    // Check pressure_sensors initialization (tmr_sensor_init called internally)
    TEST_ASSERT_EQUAL_UINT8(4, system.pressure_sensors.sensors[0].sensor_id); // First sensor ID
    TEST_ASSERT_TRUE(system.pressure_sensors.sensors[0].calibrated);
    TEST_ASSERT_EQUAL_UINT8(100, system.pressure_sensors.sensors[0].confidence_level);
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, system.pressure_sensors.sensors[0].last_error);

    // Check system_watchdog initialization (watchdog_init called internally)
    TEST_ASSERT_EQUAL_UINT32(1000, system.system_watchdog.timeout_ms); // Initialized with 1000ms
    TEST_ASSERT_EQUAL_UINT32(0, system.system_watchdog.counter);
    TEST_ASSERT_FALSE(system.system_watchdog.expired);

    // Check fault_monitor initialization (fault_monitor_init called internally)
    TEST_ASSERT_EQUAL_UINT32(0, system.fault_monitor.total_checks);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, system.fault_monitor.fault_rate);

    // Check safety_system_t's own fields are zeroed initially
    TEST_ASSERT_FALSE(system.emergency_shutdown);
    TEST_ASSERT_EQUAL_UINT32(0, system.system_uptime);
    TEST_ASSERT_NULL(system.emergency_cb);
    TEST_ASSERT_NULL(system.callback_context);
}

// --- Test Cases for safety_system_perform_safety_checks ---

void test_safety_system_perform_safety_checks_NullSystem_ReturnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL system pointer
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, safety_system_perform_safety_checks(NULL));
}

void test_safety_system_perform_safety_checks_AllChecksPass_ReturnsNone(void) {
    safety_system_t system;
    safety_system_init(&system, SIL_3);

    // Set consistent temperatures (within 2.0f tolerance)
    system.temperature_sensors.sensors[0].temperature = 25.0f;
    system.temperature_sensors.sensors[1].temperature = 26.0f;
    system.temperature_sensors.sensors[2].temperature = 27.0f;
    // Set consistent pressures (within 10.0f tolerance)
    system.pressure_sensors.sensors[0].temperature = 100.0f;
    system.pressure_sensors.sensors[1].temperature = 105.0f;
    system.pressure_sensors.sensors[2].temperature = 110.0f;

    // Set watchdog counter to not expire (current time 500ms, timeout 1000ms)
    system.system_watchdog.counter = 500;
    system.system_watchdog.last_feed_time = 0;

    // Expected: ERROR_NONE as all checks pass
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, safety_system_perform_safety_checks(&system));
    // Expected: Fault monitor recorded 3 passes (temp, press, watchdog)
    TEST_ASSERT_EQUAL_UINT32(3, system.fault_monitor.total_checks);
    TEST_ASSERT_EQUAL_UINT32(0, system.fault_monitor.fault_count);
    TEST_ASSERT_FALSE(system.fault_monitor.system_degraded);
    TEST_ASSERT_FALSE(system.fault_monitor.system_failed);
    // Expected: Emergency shutdown not triggered
    TEST_ASSERT_FALSE(system.emergency_shutdown);
    TEST_ASSERT_FALSE(mock_emergency_cb_called);
    // Expected: system_uptime incremented
    TEST_ASSERT_EQUAL_UINT32(1, system.system_uptime);
    // Expected: TMR votes should have been performed
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 26.0f, system.temperature_sensors.voted_value); // Median of 25,26,27 is 26
    TEST_ASSERT_FLOAT_WITHIN(0.01f, (100.0f+105.0f+110.0f)/3.0f, system.pressure_sensors.voted_value); // Average is 105
}

void test_safety_system_perform_safety_checks_TempInconsistent_ReturnsBusyAndTriggersEmergency(void) {
    safety_system_t system;
    safety_system_init(&system, SIL_3);
    system.emergency_cb = mock_safety_system_emergency_callback; // Assign mock callback
    system.callback_context = (void*)0xABCD;

    // Make temperature sensors inconsistent (e.g., sensor 1 is 3.0f different from sensor 0, tolerance is 2.0f)
    system.temperature_sensors.sensors[0].temperature = 25.0f;
    system.temperature_sensors.sensors[1].temperature = 28.0f; // Inconsistent
    system.temperature_sensors.sensors[2].temperature = 26.0f;
    // Set pressure consistent
    system.pressure_sensors.sensors[0].temperature = 100.0f;
    system.pressure_sensors.sensors[1].temperature = 101.0f;
    system.pressure_sensors.sensors[2].temperature = 102.0f;
    // Set watchdog counter to not expire
    system.system_watchdog.counter = 500;
    system.system_watchdog.last_feed_time = 0;

    // Expected: ERROR_BUSY due to temperature inconsistency
    TEST_ASSERT_EQUAL_INT(ERROR_BUSY, safety_system_perform_safety_checks(&system));
    // Expected: Fault monitor recorded 1 fail (temp), 1 pass (press), 1 pass (watchdog)
    TEST_ASSERT_EQUAL_UINT32(3, system.fault_monitor.total_checks);
    TEST_ASSERT_EQUAL_UINT32(1, system.fault_monitor.fault_count);
    // Expected: fault_rate 1/3 ~ 0.333f, which is > 0.05f and > 0.20f
    TEST_ASSERT_TRUE(system.fault_monitor.system_degraded);
    TEST_ASSERT_TRUE(system.fault_monitor.system_failed); // Becomes failed with 1/3 fault rate
    // Expected: Emergency shutdown triggered because system_failed is true
    TEST_ASSERT_TRUE(system.emergency_shutdown);
    TEST_ASSERT_TRUE(mock_emergency_cb_called);
    TEST_ASSERT_EQUAL_HEX16(0xDEAD, mock_emergency_cb_reason);
    TEST_ASSERT_EQUAL_PTR((void*)0xABCD, mock_emergency_cb_context);
    TEST_ASSERT_EQUAL_UINT32(1, system.system_uptime);
}

void test_safety_system_perform_safety_checks_WatchdogExpired_ReturnsBusyAndTriggersEmergency(void) {
    safety_system_t system;
    safety_system_init(&system, SIL_3);
    system.emergency_cb = mock_safety_system_emergency_callback; // Assign mock callback
    system.callback_context = (void*)0xBBBB;

    // Set all sensors consistent
    system.temperature_sensors.sensors[0].temperature = 25.0f;
    system.temperature_sensors.sensors[1].temperature = 26.0f;
    system.temperature_sensors.sensors[2].temperature = 27.0f;
    system.pressure_sensors.sensors[0].temperature = 100.0f;
    system.pressure_sensors.sensors[1].temperature = 101.0f;
    system.pressure_sensors.sensors[2].temperature = 102.0f;

    // Set watchdog to expire
    system.system_watchdog.counter = 1500; // Current time is past timeout (1000ms)
    system.system_watchdog.last_feed_time = 0;
    system.system_watchdog.timeout_cb = mock_watchdog_timeout_callback; // Assign WD mock callback
    system.system_watchdog.callback_context = (void*)0xCCCC;

    // Expected: ERROR_BUSY due to watchdog expiration
    TEST_ASSERT_EQUAL_INT(ERROR_BUSY, safety_system_perform_safety_checks(&system));
    // Expected: Watchdog expired flag is true
    TEST_ASSERT_TRUE(system.system_watchdog.expired);
    TEST_ASSERT_TRUE(mock_watchdog_timeout_cb_called); // WD callback called
    TEST_ASSERT_EQUAL_PTR((void*)0xCCCC, mock_watchdog_timeout_cb_context);
    // Expected: Fault monitor recorded 2 passes (sensors), 1 fail (watchdog)
    TEST_ASSERT_EQUAL_UINT32(3, system.fault_monitor.total_checks);
    TEST_ASSERT_EQUAL_UINT32(1, system.fault_monitor.fault_count);
    // Expected: fault_rate 1/3 ~ 0.333f, which is > 0.05f and > 0.20f
    TEST_ASSERT_TRUE(system.fault_monitor.system_degraded);
    TEST_ASSERT_TRUE(system.fault_monitor.system_failed);
    // Expected: Emergency shutdown triggered because system_failed is true
    TEST_ASSERT_TRUE(system.emergency_shutdown);
    TEST_ASSERT_TRUE(mock_emergency_cb_called);
    TEST_ASSERT_EQUAL_HEX16(0xDEAD, mock_emergency_cb_reason);
    TEST_ASSERT_EQUAL_PTR((void*)0xBBBB, mock_emergency_cb_context);
    TEST_ASSERT_EQUAL_UINT32(1, system.system_uptime);
}


// --- Test Cases for safety_system_is_safe ---

void test_safety_system_is_safe_NullSystem_ReturnsFalse(void) {
    // Expected: false due to NULL system pointer
    TEST_ASSERT_FALSE(safety_system_is_safe(NULL));
}

void test_safety_system_is_safe_IsSafe_ReturnsTrue(void) {
    safety_system_t system = {0}; // Zero-initialize to ensure all flags are false
    // Explicitly set safe conditions (redundant but clear)
    system.emergency_shutdown = false;
    system.fault_monitor.system_failed = false;
    system.system_watchdog.expired = false;

    // Expected: true as all conditions for safety are met
    TEST_ASSERT_TRUE(safety_system_is_safe(&system));
}

void test_safety_system_is_safe_EmergencyShutdownActive_ReturnsFalse(void) {
    safety_system_t system = {0};
    system.emergency_shutdown = true; // Unsafe condition
    system.fault_monitor.system_failed = false;
    system.system_watchdog.expired = false;

    // Expected: false due to emergency shutdown flag
    TEST_ASSERT_FALSE(safety_system_is_safe(&system));
}

void test_safety_system_is_safe_SystemFailed_ReturnsFalse(void) {
    safety_system_t system = {0};
    system.emergency_shutdown = false;
    system.fault_monitor.system_failed = true; // Unsafe condition
    system.system_watchdog.expired = false;

    // Expected: false due to system_failed flag
    TEST_ASSERT_FALSE(safety_system_is_safe(&system));
}

void test_safety_system_is_safe_WatchdogExpired_ReturnsFalse(void) {
    safety_system_t system = {0};
    system.emergency_shutdown = false;
    system.fault_monitor.system_failed = false;
    system.system_watchdog.expired = true; // Unsafe condition

    // Expected: false due to watchdog_expired flag
    TEST_ASSERT_FALSE(safety_system_is_safe(&system));
}

void test_safety_system_is_safe_AllUnsafeConditions_ReturnsFalse(void) {
    safety_system_t system = {0};
    system.emergency_shutdown = true;
    system.fault_monitor.system_failed = true;
    system.system_watchdog.expired = true;

    // Expected: false as multiple unsafe conditions are met
    TEST_ASSERT_FALSE(safety_system_is_safe(&system));
}


// --- Test Cases for power_supply_init ---

void test_power_supply_init_NullPs_ReturnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL ps pointer
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, power_supply_init(NULL));
}

void test_power_supply_init_Valid_InitializesCorrectly(void) {
    power_supply_t ps;
    // Expected: ERROR_NONE for valid initialization
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, power_supply_init(&ps));
    // Expected: Both supplies active
    TEST_ASSERT_TRUE(ps.supply_a_active);
    TEST_ASSERT_TRUE(ps.supply_b_active);
    // Expected: Voltages initialized to 24.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 24.0f, ps.voltage_a);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 24.0f, ps.voltage_b);
    // Expected: Redundancy not lost
    TEST_ASSERT_FALSE(ps.redundancy_lost);
}

// --- Test Cases for power_supply_monitor ---

void test_power_supply_monitor_NullPs_ReturnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL ps pointer
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, power_supply_monitor(NULL));
}

void test_power_supply_monitor_VoltagesWithinExpectedRanges_SuppliesActiveAndRedundancyPresent(void) {
    power_supply_t ps;
    power_supply_init(&ps);

    // Run multiple times to cover different `rand()` outputs.
    // `rand() % 100` gives a value from 0 to 99.
    // `voltage_a` will be `23.8f + (0 to 99) * 0.01f` => `23.80f` to `24.79f`.
    // `voltage_b` will be `24.1f + (0 to 99) * 0.01f` => `24.10f` to `25.09f`.
    // The acceptable range is `20.0f` to `28.0f`. Both simulated ranges are always within this.
    for (int i = 0; i < 5; i++) {
        // Expected: ERROR_NONE for valid monitoring call
        TEST_ASSERT_EQUAL_INT(ERROR_NONE, power_supply_monitor(&ps));
        // Expected: Voltage A within its simulated range
        TEST_ASSERT_TRUE(ps.voltage_a >= 23.80f);
        TEST_ASSERT_TRUE(ps.voltage_a <= 24.79f);
        // Expected: Voltage B within its simulated range
        TEST_ASSERT_TRUE(ps.voltage_b >= 24.10f);
        TEST_ASSERT_TRUE(ps.voltage_b <= 25.09f);

        // Expected: Both supplies remain active because simulated voltages are always within [20.0f, 28.0f]
        TEST_ASSERT_TRUE(ps.supply_a_active);
        TEST_ASSERT_TRUE(ps.supply_b_active);
        // Expected: Redundancy not lost because both are active and within range
        TEST_ASSERT_FALSE(ps.redundancy_lost);
    }
}

// NOTE: Specific "out of range" scenarios for `power_supply_monitor` cannot be tested
// without stubbing `rand()` or directly manipulating the `ps->voltage_a/b` fields
// before the internal checks. Per prompt rules, `rand()` is not an external dependency
// to stub. Thus, testing is confined to the observable behavior given the `rand()` outputs.

// --- Test Cases for power_supply_has_redundancy ---

void test_power_supply_has_redundancy_NullPs_ReturnsFalse(void) {
    // Expected: false due to NULL ps pointer
    TEST_ASSERT_FALSE(power_supply_has_redundancy(NULL));
}

void test_power_supply_has_redundancy_BothActiveNoRedundancyLost_ReturnsTrue(void) {
    power_supply_t ps = {0}; // Zero-initialize
    ps.supply_a_active = true;
    ps.supply_b_active = true;
    ps.redundancy_lost = false;

    // Expected: true as both supplies are active and redundancy is not lost
    TEST_ASSERT_TRUE(power_supply_has_redundancy(&ps));
}

void test_power_supply_has_redundancy_SupplyAInactive_ReturnsFalse(void) {
    power_supply_t ps = {0};
    ps.supply_a_active = false; // Unsafe condition
    ps.supply_b_active = true;
    ps.redundancy_lost = false;

    // Expected: false due to supply A being inactive
    TEST_ASSERT_FALSE(power_supply_has_redundancy(&ps));
}

void test_power_supply_has_redundancy_SupplyBInactive_ReturnsFalse(void) {
    power_supply_t ps = {0};
    ps.supply_a_active = true;
    ps.supply_b_active = false; // Unsafe condition
    ps.redundancy_lost = false;

    // Expected: false due to supply B being inactive
    TEST_ASSERT_FALSE(power_supply_has_redundancy(&ps));
}

void test_power_supply_has_redundancy_RedundancyLostFlag_ReturnsFalse(void) {
    power_supply_t ps = {0};
    ps.supply_a_active = true;
    ps.supply_b_active = true;
    ps.redundancy_lost = true; // Unsafe condition

    // Expected: false due to redundancy lost flag being set
    TEST_ASSERT_FALSE(power_supply_has_redundancy(&ps));
}

void test_power_supply_has_redundancy_AllConditionsUnsafe_ReturnsFalse(void) {
    power_supply_t ps = {0};
    ps.supply_a_active = false;
    ps.supply_b_active = false;
    ps.redundancy_lost = true;

    // Expected: false as multiple unsafe conditions are met
    TEST_ASSERT_FALSE(power_supply_has_redundancy(&ps));
}


// --- Main Test Runner ---


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_sensor_read_redundant_NullSensors_ReturnsInvalidParam);
    RUN_TEST(test_sensor_read_redundant_NullResult_ReturnsInvalidParam);
    RUN_TEST(test_sensor_read_redundant_ZeroCount_ReturnsInvalidParam);
    RUN_TEST(test_sensor_read_redundant_AllValid_ReturnsAverage);
    RUN_TEST(test_sensor_read_redundant_MixedValidInvalid_ReturnsAverageOfValid);
    RUN_TEST(test_sensor_read_redundant_NoValidReadings_ReturnsBusy);
    RUN_TEST(test_tmr_sensor_init_NullTmr_ReturnsInvalidParam);
    RUN_TEST(test_tmr_sensor_init_NullSensorIds_ReturnsInvalidParam);
    RUN_TEST(test_tmr_sensor_init_Valid_InitializesCorrectly);
    RUN_TEST(test_tmr_sensor_vote_NullTmr_ReturnsInvalidParam);
    RUN_TEST(test_tmr_sensor_vote_LessThanTwoValidSensors_ReturnsBusyAndSetsFaultDetected);
    RUN_TEST(test_tmr_sensor_vote_Majority_SimplifiedVoting);
    RUN_TEST(test_tmr_sensor_vote_Average_CalculatesMeanOfValidSensors);
    RUN_TEST(test_tmr_sensor_vote_Average_WithOneInvalidSensor_CalculatesMeanOfRemainingValid);
    RUN_TEST(test_tmr_sensor_vote_Median_SortsAndPicksMiddleValue);
    RUN_TEST(test_tmr_sensor_vote_Median_WithOneInvalidSensor_PicksMedianOfRemaining);
    RUN_TEST(test_tmr_sensor_vote_Weighted_CalculatesWeightedAverage);
    RUN_TEST(test_tmr_sensor_vote_Weighted_WithOneInvalidSensor_CalculatesWeightedAverageOfRemaining);
    RUN_TEST(test_tmr_sensor_check_consistency_NullTmr_ReturnsFalse);
    RUN_TEST(test_tmr_sensor_check_consistency_AllConsistent_ReturnsTrue);
    RUN_TEST(test_tmr_sensor_check_consistency_InconsistentSensor1_ReturnsFalseAndSetsMask);
    RUN_TEST(test_tmr_sensor_check_consistency_MultipleInconsistent_ReturnsFalseAndMasksFirst);
    RUN_TEST(test_tmr_sensor_check_consistency_EdgeOfTolerance_ReturnsTrue);
    RUN_TEST(test_watchdog_init_NullWd_ReturnsInvalidParam);
    RUN_TEST(test_watchdog_init_Valid_InitializesCorrectly);
    RUN_TEST(test_watchdog_feed_NullWd_DoesNothing);
    RUN_TEST(test_watchdog_feed_Valid_ResetsTimerAndExpiredFlag);
    RUN_TEST(test_watchdog_check_expired_NullWd_ReturnsTrue);
    RUN_TEST(test_watchdog_check_expired_NotExpired_ReturnsFalse);
    RUN_TEST(test_watchdog_check_expired_Expired_ReturnsTrueAndCallsCallback);
    RUN_TEST(test_watchdog_check_expired_ExpiredMultipleTimesWithoutFeed_CallsCallbackMultipleTimes);
    RUN_TEST(test_fault_monitor_init_NullFm_ReturnsInvalidParam);
    RUN_TEST(test_fault_monitor_init_Valid_InitializesCorrectly);
    RUN_TEST(test_fault_monitor_record_check_NullFm_DoesNothing);
    RUN_TEST(test_fault_monitor_record_check_Pass_UpdatesStatsCorrectly);
    RUN_TEST(test_fault_monitor_record_check_Fail_UpdatesStatsAndStatus);
    RUN_TEST(test_fault_monitor_record_check_MixToDegradedState_UpdatesStatus);
    RUN_TEST(test_fault_monitor_record_check_MixToFailedState_UpdatesStatus);
    RUN_TEST(test_fault_monitor_record_check_HistoryWrapAround_UpdatesCorrectly);
    RUN_TEST(test_fault_monitor_update_status_NullFm_DoesNothing);
    RUN_TEST(test_fault_monitor_update_status_ValidFm_NoFunctionalChangeYet);
    RUN_TEST(test_safety_system_init_NullSystem_ReturnsInvalidParam);
    RUN_TEST(test_safety_system_init_Valid_InitializesAllComponents);
    RUN_TEST(test_safety_system_perform_safety_checks_NullSystem_ReturnsInvalidParam);
    RUN_TEST(test_safety_system_perform_safety_checks_AllChecksPass_ReturnsNone);
    RUN_TEST(test_safety_system_perform_safety_checks_TempInconsistent_ReturnsBusyAndTriggersEmergency);
    RUN_TEST(test_safety_system_perform_safety_checks_WatchdogExpired_ReturnsBusyAndTriggersEmergency);
    RUN_TEST(test_safety_system_is_safe_NullSystem_ReturnsFalse);
    RUN_TEST(test_safety_system_is_safe_IsSafe_ReturnsTrue);
    RUN_TEST(test_safety_system_is_safe_EmergencyShutdownActive_ReturnsFalse);
    RUN_TEST(test_safety_system_is_safe_SystemFailed_ReturnsFalse);
    RUN_TEST(test_safety_system_is_safe_WatchdogExpired_ReturnsFalse);
    RUN_TEST(test_safety_system_is_safe_AllUnsafeConditions_ReturnsFalse);
    RUN_TEST(test_power_supply_init_NullPs_ReturnsInvalidParam);
    RUN_TEST(test_power_supply_init_Valid_InitializesCorrectly);
    RUN_TEST(test_power_supply_monitor_NullPs_ReturnsInvalidParam);
    RUN_TEST(test_power_supply_monitor_VoltagesWithinExpectedRanges_SuppliesActiveAndRedundancyPresent);
    RUN_TEST(test_power_supply_has_redundancy_NullPs_ReturnsFalse);
    RUN_TEST(test_power_supply_has_redundancy_BothActiveNoRedundancyLost_ReturnsTrue);
    RUN_TEST(test_power_supply_has_redundancy_SupplyAInactive_ReturnsFalse);
    RUN_TEST(test_power_supply_has_redundancy_SupplyBInactive_ReturnsFalse);
    RUN_TEST(test_power_supply_has_redundancy_RedundancyLostFlag_ReturnsFalse);
    RUN_TEST(test_power_supply_has_redundancy_AllConditionsUnsafe_ReturnsFalse);

    return UNITY_END();
}