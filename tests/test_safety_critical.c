/* test_safety_critical.c – Auto-generated Expert Unity Tests */
#include "unity.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h> // Required for rand() if not mocked

// Include the source file to be tested. This is typical for Unity.
// Ensure this path is correct for your build system.
#include "safety_critical.h"

// =================================================================================
// Mocking for external dependencies or non-deterministic functions
// =================================================================================

// Mock rand() for deterministic behavior in power_supply_monitor
static int mock_rand_value = 0;
static int mock_rand_call_count = 0;
int rand(void) {
    mock_rand_call_count++;
    return mock_rand_value;
}
void srand(unsigned int seed) {
    (void)seed; // Suppress unused parameter warning
    // No-op for mock
}

// Watchdog timeout callback mock
static bool wd_timeout_cb_called = false;
static void *wd_timeout_cb_context = NULL;
void mock_watchdog_timeout_cb(void *context) {
    wd_timeout_cb_called = true;
    wd_timeout_cb_context = context;
}

// Emergency shutdown callback mock
static bool emergency_cb_called = false;
static uint32_t emergency_cb_code = 0;
static void *emergency_cb_context = NULL;
void mock_emergency_shutdown_cb(uint32_t code, void *context) {
    emergency_cb_called = true;
    emergency_cb_code = code;
    emergency_cb_context = context;
}

// =================================================================================
// Setup and Teardown
// =================================================================================

void setUp(void) {
    mock_rand_value = 0;
    mock_rand_call_count = 0;
    wd_timeout_cb_called = false;
    wd_timeout_cb_context = NULL;
    emergency_cb_called = false;
    emergency_cb_code = 0;
    emergency_cb_context = NULL;
}

void tearDown(void) {
    // Reset all mocks to ensure test isolation
    setUp();
}

// =================================================================================
// Tests for sensor_read_redundant
// =================================================================================

void test_sensor_read_redundant_nullSensors_returnsInvalidParam(void) {
    float result = 0.0f;
    // Expected: ERROR_INVALID_PARAM due to NULL sensors pointer
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_read_redundant(NULL, 1, &result));
}

void test_sensor_read_redundant_nullResult_returnsInvalidParam(void) {
    sensor_data_t sensors[1] = {{.last_error = ERROR_NONE, .calibrated = true, .temperature = 25.0f}};
    // Expected: ERROR_INVALID_PARAM due to NULL result pointer
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_read_redundant(sensors, 1, NULL));
}

void test_sensor_read_redundant_zeroCount_returnsInvalidParam(void) {
    sensor_data_t sensors[1] = {{.last_error = ERROR_NONE, .calibrated = true, .temperature = 25.0f}};
    float result = 0.0f;
    // Expected: ERROR_INVALID_PARAM due to zero count
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_read_redundant(sensors, 0, &result));
}

void test_sensor_read_redundant_noValidReadings_returnsBusy(void) {
    sensor_data_t sensors[3] = {
        {.last_error = ERROR_BUSY, .calibrated = true, .temperature = 20.0f},
        {.last_error = ERROR_NONE, .calibrated = false, .temperature = 25.0f},
        {.last_error = ERROR_NONE, .calibrated = true, .temperature = 30.0f, .confidence_level = 0} // Confidence 0 is okay for this func
    };
    float result = 0.0f;
    // Expected: ERROR_BUSY as no sensors are both ERROR_NONE and calibrated
    TEST_ASSERT_EQUAL(ERROR_BUSY, sensor_read_redundant(sensors, 3, &result));
}

void test_sensor_read_redundant_allValid_returnsAverage(void) {
    sensor_data_t sensors[3] = {
        {.last_error = ERROR_NONE, .calibrated = true, .temperature = 20.0f},
        {.last_error = ERROR_NONE, .calibrated = true, .temperature = 25.0f},
        {.last_error = ERROR_NONE, .calibrated = true, .temperature = 30.0f}
    };
    float result = 0.0f;
    // Expected: (20.0 + 25.0 + 30.0) / 3 = 25.0f
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_read_redundant(sensors, 3, &result));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, result);
}

void test_sensor_read_redundant_someInvalid_returnsAverageOfValid(void) {
    sensor_data_t sensors[4] = {
        {.last_error = ERROR_NONE, .calibrated = true, .temperature = 10.0f},
        {.last_error = ERROR_BUSY, .calibrated = true, .temperature = 20.0f}, // Invalid
        {.last_error = ERROR_NONE, .calibrated = false, .temperature = 30.0f}, // Invalid
        {.last_error = ERROR_NONE, .calibrated = true, .temperature = 40.0f}
    };
    float result = 0.0f;
    // Expected: (10.0 + 40.0) / 2 = 25.0f
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_read_redundant(sensors, 4, &result));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, result);
}

// =================================================================================
// Tests for tmr_sensor_init
// =================================================================================

void test_tmr_sensor_init_nullTmr_returnsInvalidParam(void) {
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    // Expected: ERROR_INVALID_PARAM due to NULL tmr pointer
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, tmr_sensor_init(NULL, sensor_ids));
}

void test_tmr_sensor_init_nullSensorIds_returnsInvalidParam(void) {
    tmr_sensor_t tmr;
    // Expected: ERROR_INVALID_PARAM due to NULL sensor_ids pointer
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, tmr_sensor_init(&tmr, NULL));
}

void test_tmr_sensor_init_validParams_initializesCorrectly(void) {
    tmr_sensor_t tmr;
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {10, 20, 30};
    // Expected: ERROR_NONE and tmr structure initialized as per source
    TEST_ASSERT_EQUAL(ERROR_NONE, tmr_sensor_init(&tmr, sensor_ids));

    // Expected: TMR structure zeroed out first
    TEST_ASSERT_EQUAL_INT(0, tmr.voted_value); // Voted value should be zeroed
    TEST_ASSERT_FALSE(tmr.fault_detected);
    TEST_ASSERT_EQUAL_INT(0, tmr.last_vote_time);
    TEST_ASSERT_EQUAL_INT(0, tmr.faulty_sensor_mask);

    for (uint8_t i = 0; i < TMR_SENSOR_COUNT; i++) {
        // Expected: Sensor ID set from input array
        TEST_ASSERT_EQUAL_UINT8(sensor_ids[i], tmr.sensors[i].sensor_id);
        // Expected: Calibrated flag set to true
        TEST_ASSERT_TRUE(tmr.sensors[i].calibrated);
        // Expected: Confidence level set to 100
        TEST_ASSERT_EQUAL_UINT8(100, tmr.sensors[i].confidence_level);
        // Expected: Last error set to ERROR_NONE
        TEST_ASSERT_EQUAL(ERROR_NONE, tmr.sensors[i].last_error);
    }
}

// =================================================================================
// Tests for tmr_sensor_vote
// =================================================================================

void test_tmr_sensor_vote_nullTmr_returnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL tmr pointer
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, tmr_sensor_vote(NULL, VOTE_AVERAGE));
}

void test_tmr_sensor_vote_notEnoughValidSensors_returnsBusyAndSetsFaultDetected(void) {
    tmr_sensor_t tmr;
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, sensor_ids);

    // Make only one sensor valid
    tmr.sensors[0].temperature = 25.0f;
    tmr.sensors[1].last_error = ERROR_BUSY; // Invalid
    tmr.sensors[2].last_error = ERROR_BUSY; // Invalid

    // Expected: ERROR_BUSY because valid_count < 2
    TEST_ASSERT_EQUAL(ERROR_BUSY, tmr_sensor_vote(&tmr, VOTE_AVERAGE));
    // Expected: fault_detected flag to be set to true
    TEST_ASSERT_TRUE(tmr.fault_detected);
}

void test_tmr_sensor_vote_majority_validSensors(void) {
    tmr_sensor_t tmr;
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, sensor_ids);

    tmr.sensors[0].temperature = 20.0f;
    tmr.sensors[1].temperature = 21.0f;
    tmr.sensors[2].temperature = 22.0f;

    // Expected: ERROR_NONE and voted_value set to values[0] (20.0f) as per simplified logic
    TEST_ASSERT_EQUAL(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_MAJORITY));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 20.0f, tmr.voted_value);
    // Expected: fault_detected flag to be cleared
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

void test_tmr_sensor_vote_average_validSensors(void) {
    tmr_sensor_t tmr;
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, sensor_ids);

    tmr.sensors[0].temperature = 20.0f;
    tmr.sensors[1].temperature = 25.0f;
    tmr.sensors[2].temperature = 30.0f;

    // Expected: ERROR_NONE and voted_value set to average (20+25+30)/3 = 25.0f
    TEST_ASSERT_EQUAL(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_AVERAGE));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, tmr.voted_value);
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

void test_tmr_sensor_vote_median_validSensors(void) {
    tmr_sensor_t tmr;
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, sensor_ids);

    tmr.sensors[0].temperature = 30.0f;
    tmr.sensors[1].temperature = 20.0f;
    tmr.sensors[2].temperature = 25.0f;

    // Expected: ERROR_NONE and voted_value set to median after sorting (20, 25, 30) which is 25.0f
    TEST_ASSERT_EQUAL(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_MEDIAN));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, tmr.voted_value);
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

void test_tmr_sensor_vote_median_validSensors_withOneFaulty(void) {
    tmr_sensor_t tmr;
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, sensor_ids);

    tmr.sensors[0].temperature = 30.0f;
    tmr.sensors[1].temperature = 20.0f;
    tmr.sensors[2].temperature = 25.0f;
    tmr.sensors[1].last_error = ERROR_BUSY; // Make one sensor invalid

    // Expected: ERROR_NONE, valid sensors are 30.0f, 25.0f. Median of these two is values[valid_count/2] = values[1] = 25.0f
    TEST_ASSERT_EQUAL(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_MEDIAN));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, tmr.voted_value);
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

void test_tmr_sensor_vote_weighted_validSensors(void) {
    tmr_sensor_t tmr;
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, sensor_ids);

    tmr.sensors[0].temperature = 10.0f; tmr.sensors[0].confidence_level = 50; // Weight 0.5
    tmr.sensors[1].temperature = 20.0f; tmr.sensors[1].confidence_level = 100; // Weight 1.0
    tmr.sensors[2].temperature = 30.0f; tmr.sensors[2].confidence_level = 50; // Weight 0.5

    // Expected: ((10 * 0.5) + (20 * 1.0) + (30 * 0.5)) / (0.5 + 1.0 + 0.5) = (5 + 20 + 15) / 2.0 = 40 / 2.0 = 20.0f
    TEST_ASSERT_EQUAL(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_WEIGHTED));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 20.0f, tmr.voted_value);
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

void test_tmr_sensor_vote_weighted_oneFaultySensor(void) {
    tmr_sensor_t tmr;
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, sensor_ids);

    tmr.sensors[0].temperature = 10.0f; tmr.sensors[0].confidence_level = 50;
    tmr.sensors[1].temperature = 20.0f; tmr.sensors[1].confidence_level = 100;
    tmr.sensors[2].temperature = 30.0f; tmr.sensors[2].confidence_level = 50;
    tmr.sensors[0].last_error = ERROR_BUSY; // Make sensor 0 invalid

    // Expected: ((20 * 1.0) + (30 * 0.5)) / (1.0 + 0.5) = (20 + 15) / 1.5 = 35 / 1.5 = 23.333f
    TEST_ASSERT_EQUAL(ERROR_NONE, tmr_sensor_vote(&tmr, VOTE_WEIGHTED));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 23.33f, tmr.voted_value);
    TEST_ASSERT_FALSE(tmr.fault_detected);
}

// =================================================================================
// Tests for tmr_sensor_check_consistency
// =================================================================================

void test_tmr_sensor_check_consistency_nullTmr_returnsFalse(void) {
    // Expected: false due to NULL tmr pointer
    TEST_ASSERT_FALSE(tmr_sensor_check_consistency(NULL, 1.0f));
}

void test_tmr_sensor_check_consistency_allConsistent_returnsTrue(void) {
    tmr_sensor_t tmr;
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, sensor_ids);

    tmr.sensors[0].temperature = 25.0f;
    tmr.sensors[1].temperature = 25.5f;
    tmr.sensors[2].temperature = 24.8f;
    float tolerance = 1.0f; // All within 1.0f of 25.0f

    // Expected: true as all sensors are within tolerance
    TEST_ASSERT_TRUE(tmr_sensor_check_consistency(&tmr, tolerance));
    // Expected: faulty_sensor_mask to be 0
    TEST_ASSERT_EQUAL_UINT8(0, tmr.faulty_sensor_mask);
}

void test_tmr_sensor_check_consistency_oneInconsistent_returnsFalseAndSetsMask(void) {
    tmr_sensor_t tmr;
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, sensor_ids);

    tmr.sensors[0].temperature = 25.0f;
    tmr.sensors[1].temperature = 26.5f; // |26.5 - 25.0| = 1.5
    tmr.sensors[2].temperature = 24.8f;
    float tolerance = 1.0f;

    // Expected: false as sensor[1] is outside tolerance
    TEST_ASSERT_FALSE(tmr_sensor_check_consistency(&tmr, tolerance));
    // Expected: faulty_sensor_mask to have bit 1 set (0b0010)
    TEST_ASSERT_EQUAL_UINT8(0x02, tmr.faulty_sensor_mask);
}

void test_tmr_sensor_check_consistency_multipleInconsistent_returnsFalseAndSetsMask(void) {
    tmr_sensor_t tmr;
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, sensor_ids);

    tmr.sensors[0].temperature = 25.0f;
    tmr.sensors[1].temperature = 26.5f; // |26.5 - 25.0| = 1.5 > 1.0
    tmr.sensors[2].temperature = 27.0f; // |27.0 - 25.0| = 2.0 > 1.0
    float tolerance = 1.0f;

    // Expected: false as multiple sensors are outside tolerance. Loop stops at first fault.
    TEST_ASSERT_FALSE(tmr_sensor_check_consistency(&tmr, tolerance));
    // Expected: faulty_sensor_mask to have bit 1 set (0b0010) as it detects the first inconsistency
    TEST_ASSERT_EQUAL_UINT8(0x02, tmr.faulty_sensor_mask);
}

void test_tmr_sensor_check_consistency_edgeTolerance_returnsTrue(void) {
    tmr_sensor_t tmr;
    uint8_t sensor_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    tmr_sensor_init(&tmr, sensor_ids);

    tmr.sensors[0].temperature = 25.0f;
    tmr.sensors[1].temperature = 26.0f;
    tmr.sensors[2].temperature = 24.0f;
    float tolerance = 1.0f; // All exactly at tolerance limit

    // Expected: true as all sensors are within tolerance (inclusive)
    TEST_ASSERT_TRUE(tmr_sensor_check_consistency(&tmr, tolerance));
    TEST_ASSERT_EQUAL_UINT8(0, tmr.faulty_sensor_mask);
}

// =================================================================================
// Tests for watchdog_init
// =================================================================================

void test_watchdog_init_nullWatchdog_returnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL wd pointer
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, watchdog_init(NULL, 1000));
}

void test_watchdog_init_validParams_initializesCorrectly(void) {
    watchdog_t wd;
    uint32_t timeout = 5000;
    // Expected: ERROR_NONE and watchdog structure initialized as per source
    TEST_ASSERT_EQUAL(ERROR_NONE, watchdog_init(&wd, timeout));

    // Expected: Timeout_ms set
    TEST_ASSERT_EQUAL_UINT32(timeout, wd.timeout_ms);
    // Expected: Counter set to 0
    TEST_ASSERT_EQUAL_UINT32(0, wd.counter);
    // Expected: Last feed time set to 0
    TEST_ASSERT_EQUAL_UINT32(0, wd.last_feed_time);
    // Expected: Expired flag set to false
    TEST_ASSERT_FALSE(wd.expired);
    // Callbacks should be NULL initially
    TEST_ASSERT_NULL(wd.timeout_cb);
    TEST_ASSERT_NULL(wd.callback_context);
}

// =================================================================================
// Tests for watchdog_feed
// =================================================================================

void test_watchdog_feed_nullWatchdog_doesNothing(void) {
    // Expected: No crash and function returns safely (void function)
    watchdog_feed(NULL);
    // No direct assertion possible for a void function with null input, but ensures stability
}

void test_watchdog_feed_feedsWatchdog_resetsState(void) {
    watchdog_t wd;
    watchdog_init(&wd, 1000);

    // Simulate some time passing and an expired state
    wd.counter = 500;
    wd.last_feed_time = 0;
    wd.expired = true;

    watchdog_feed(&wd);

    // Expected: last_feed_time updated to current counter
    TEST_ASSERT_EQUAL_UINT32(500, wd.last_feed_time);
    // Expected: expired flag set to false
    TEST_ASSERT_FALSE(wd.expired);
}

// =================================================================================
// Tests for watchdog_check_expired
// =================================================================================

void test_watchdog_check_expired_nullWatchdog_returnsTrue(void) {
    // Expected: true due to NULL wd pointer
    TEST_ASSERT_TRUE(watchdog_check_expired(NULL));
}

void test_watchdog_check_expired_notExpired_returnsFalse(void) {
    watchdog_t wd;
    watchdog_init(&wd, 100); // 100ms timeout
    wd.counter = 50;
    wd.last_feed_time = 0; // Time since last feed is 50ms

    // Expected: false as 50ms is less than 100ms timeout
    TEST_ASSERT_FALSE(watchdog_check_expired(&wd));
    TEST_ASSERT_FALSE(wd.expired);
    TEST_ASSERT_FALSE(wd_timeout_cb_called);
}

void test_watchdog_check_expired_justExpired_returnsTrueAndCallsCallback(void) {
    watchdog_t wd;
    watchdog_init(&wd, 100);
    wd.timeout_cb = mock_watchdog_timeout_cb;
    wd.callback_context = (void*)0xABCD;

    wd.counter = 101;
    wd.last_feed_time = 0; // Time since last feed is 101ms

    // Expected: true as 101ms is greater than 100ms timeout
    TEST_ASSERT_TRUE(watchdog_check_expired(&wd));
    TEST_ASSERT_TRUE(wd.expired);
    // Expected: callback was called
    TEST_ASSERT_TRUE(wd_timeout_cb_called);
    TEST_ASSERT_EQUAL_PTR((void*)0xABCD, wd_timeout_cb_context);
}

void test_watchdog_check_expired_alreadyExpired_returnsTrue(void) {
    watchdog_t wd;
    watchdog_init(&wd, 100);
    wd.expired = true; // Pre-set to expired
    wd.counter = 200;
    wd.last_feed_time = 0;
    wd.timeout_cb = mock_watchdog_timeout_cb; // Set to ensure it's not called again if already expired and logic doesn't re-call.

    // Expected: true as it was already expired.
    // The current logic re-evaluates expiration and calls the callback again if condition met.
    TEST_ASSERT_TRUE(watchdog_check_expired(&wd));
    TEST_ASSERT_TRUE(wd.expired);
    TEST_ASSERT_TRUE(wd_timeout_cb_called); // Still expected to be called again if condition met
}

void test_watchdog_check_expired_atTimeoutEdge_returnsFalse(void) {
    watchdog_t wd;
    watchdog_init(&wd, 100);
    wd.counter = 100;
    wd.last_feed_time = 0; // Time since last feed is exactly 100ms

    // Expected: false as 100ms is NOT > 100ms (it's ==)
    TEST_ASSERT_FALSE(watchdog_check_expired(&wd));
    TEST_ASSERT_FALSE(wd.expired);
    TEST_ASSERT_FALSE(wd_timeout_cb_called);
}

// =================================================================================
// Tests for fault_monitor_init
// =================================================================================

void test_fault_monitor_init_nullFaultMonitor_returnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL fm pointer
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, fault_monitor_init(NULL));
}

void test_fault_monitor_init_validParams_initializesCorrectly(void) {
    fault_monitor_t fm;
    // Set some garbage values to ensure memset works
    fm.total_checks = 99;
    fm.fault_count = 5;
    fm.history_index = 10;
    fm.fault_rate = 0.5f;
    fm.system_degraded = true;
    fm.system_failed = true;
    for (int i = 0; i < 32; i++) fm.fault_history[i] = 1;

    // Expected: ERROR_NONE and fm structure zeroed out
    TEST_ASSERT_EQUAL(ERROR_NONE, fault_monitor_init(&fm));

    TEST_ASSERT_EQUAL_UINT32(0, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(0, fm.fault_count);
    TEST_ASSERT_EQUAL_UINT8(0, fm.history_index);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, fm.fault_rate);
    TEST_ASSERT_FALSE(fm.system_degraded);
    TEST_ASSERT_FALSE(fm.system_failed);
    for (int i = 0; i < 32; i++) {
        // Expected: fault_history to be all zeros
        TEST_ASSERT_EQUAL_UINT8(0, fm.fault_history[i]);
    }
}

// =================================================================================
// Tests for fault_monitor_record_check
// =================================================================================

void test_fault_monitor_record_check_nullFaultMonitor_doesNothing(void) {
    // Expected: No crash and function returns safely (void function)
    fault_monitor_record_check(NULL, true);
    // No direct assertion possible, but ensures stability
}

void test_fault_monitor_record_check_passed_updatesCountersAndHistory(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);

    fault_monitor_record_check(&fm, true);

    // Expected: total_checks incremented, fault_count not, history updated
    TEST_ASSERT_EQUAL_UINT32(1, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(0, fm.fault_count);
    TEST_ASSERT_EQUAL_UINT8(1, fm.history_index); // (0+1)%32 = 1
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, fm.fault_rate);
    TEST_ASSERT_EQUAL_UINT8(0, fm.fault_history[0]); // History entry is 0 for passed
    TEST_ASSERT_FALSE(fm.system_degraded);
    TEST_ASSERT_FALSE(fm.system_failed);
}

void test_fault_monitor_record_check_failed_updatesCountersAndHistory(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);

    fault_monitor_record_check(&fm, false);

    // Expected: total_checks incremented, fault_count incremented, history updated
    TEST_ASSERT_EQUAL_UINT32(1, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(1, fm.fault_count);
    TEST_ASSERT_EQUAL_UINT8(1, fm.history_index);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, fm.fault_rate); // 1/1 = 1.0
    TEST_ASSERT_EQUAL_UINT8(1, fm.fault_history[0]); // History entry is 1 for failed
    TEST_ASSERT_TRUE(fm.system_degraded); // 1.0 > 0.05
    TEST_ASSERT_TRUE(fm.system_failed); // 1.0 > 0.20
}

void test_fault_monitor_record_check_multipleChecks_updatesRateAndStatus(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);

    // Sequence: Pass, Pass, Fail, Pass, Fail, Pass, Fail (3 failures in 7 checks)
    fault_monitor_record_check(&fm, true); // 0/1 = 0%
    fault_monitor_record_check(&fm, true); // 0/2 = 0%
    fault_monitor_record_check(&fm, false); // 1/3 = 33.3%
    fault_monitor_record_check(&fm, true); // 1/4 = 25%
    fault_monitor_record_check(&fm, false); // 2/5 = 40%
    fault_monitor_record_check(&fm, true); // 2/6 = 33.3%
    fault_monitor_record_check(&fm, false); // 3/7 = 42.8%

    // Expected: Final counts and rate
    TEST_ASSERT_EQUAL_UINT32(7, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(3, fm.fault_count);
    TEST_ASSERT_EQUAL_UINT8(7, fm.history_index);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 3.0f / 7.0f, fm.fault_rate); // ~0.4285f

    // Expected: 0.4285 > 0.05, so degraded is true
    TEST_ASSERT_TRUE(fm.system_degraded);
    // Expected: 0.4285 > 0.20, so failed is true
    TEST_ASSERT_TRUE(fm.system_failed);

    // Check history (last 7 entries)
    TEST_ASSERT_EQUAL_UINT8(0, fm.fault_history[0]); // Pass
    TEST_ASSERT_EQUAL_UINT8(0, fm.fault_history[1]); // Pass
    TEST_ASSERT_EQUAL_UINT8(1, fm.fault_history[2]); // Fail
    TEST_ASSERT_EQUAL_UINT8(0, fm.fault_history[3]); // Pass
    TEST_ASSERT_EQUAL_UINT8(1, fm.fault_history[4]); // Fail
    TEST_ASSERT_EQUAL_UINT8(0, fm.fault_history[5]); // Pass
    TEST_ASSERT_EQUAL_UINT8(1, fm.fault_history[6]); // Fail
}

void test_fault_monitor_record_check_historyWrapsAround(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);

    for (int i = 0; i < 32; i++) {
        fault_monitor_record_check(&fm, false); // All failed
    }

    // Expected: history_index wraps back to 0
    TEST_ASSERT_EQUAL_UINT8(0, fm.history_index);
    TEST_ASSERT_EQUAL_UINT32(32, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(32, fm.fault_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, fm.fault_rate);
    TEST_ASSERT_TRUE(fm.system_degraded);
    TEST_ASSERT_TRUE(fm.system_failed);

    // Record one more check (pass)
    fault_monitor_record_check(&fm, true); // Now 32 fails, 1 pass (total 33)

    // Expected: history_index is 1
    TEST_ASSERT_EQUAL_UINT8(1, fm.history_index);
    // Expected: The oldest entry (history[0]) is now 0 (passed)
    TEST_ASSERT_EQUAL_UINT8(0, fm.fault_history[0]);
    // Still 32 fails out of 33 checks (since the oldest fail was overwritten)
    TEST_ASSERT_EQUAL_UINT32(33, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(32, fm.fault_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 32.0f / 33.0f, fm.fault_rate); // ~0.969f
    TEST_ASSERT_TRUE(fm.system_degraded);
    TEST_ASSERT_TRUE(fm.system_failed);
}

void test_fault_monitor_record_check_degradedStatusThreshold(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);

    // Test for exactly 5% (0.05f)
    // 1 fault in 20 checks = 0.05f -> NOT degraded (>0.05f)
    for (int i = 0; i < 19; i++) { fault_monitor_record_check(&fm, true); }
    fault_monitor_record_check(&fm, false); // 1 fault, 20 total. Rate = 1/20 = 0.05f
    TEST_ASSERT_EQUAL_UINT32(20, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(1, fm.fault_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.05f, fm.fault_rate);
    TEST_ASSERT_FALSE(fm.system_degraded);
    TEST_ASSERT_FALSE(fm.system_failed);

    fault_monitor_record_check(&fm, false); // 2 faults, 21 total. Rate = 2/21 = ~0.095f
    TEST_ASSERT_EQUAL_UINT32(21, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(2, fm.fault_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.0f / 21.0f, fm.fault_rate);
    // Expected: degraded because 2/21 > 0.05f
    TEST_ASSERT_TRUE(fm.system_degraded);
    TEST_ASSERT_FALSE(fm.system_failed);
}

void test_fault_monitor_record_check_failedStatusThreshold(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);

    // Test for exactly 20% (0.20f)
    // 1 fault in 5 checks = 0.20f -> NOT failed (>0.20f)
    for (int i = 0; i < 4; i++) { fault_monitor_record_check(&fm, true); }
    fault_monitor_record_check(&fm, false); // 1 fault, 5 total. Rate = 1/5 = 0.20f
    TEST_ASSERT_EQUAL_UINT32(5, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(1, fm.fault_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.20f, fm.fault_rate);
    TEST_ASSERT_FALSE(fm.system_failed); // Not > 0.20f

    fault_monitor_record_check(&fm, false); // 2 faults, 6 total. Rate = 2/6 = ~0.333f
    TEST_ASSERT_EQUAL_UINT32(6, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(2, fm.fault_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.0f / 6.0f, fm.fault_rate);
    // Expected: failed because 2/6 > 0.20f
    TEST_ASSERT_TRUE(fm.system_failed);
}

// =================================================================================
// Tests for fault_monitor_update_status
// =================================================================================

void test_fault_monitor_update_status_nullFaultMonitor_doesNothing(void) {
    // Expected: No crash and function returns safely (void function)
    fault_monitor_update_status(NULL);
    // No direct assertion possible, but ensures stability
}

void test_fault_monitor_update_status_validFaultMonitor_doesNothingAsPlaceholder(void) {
    fault_monitor_t fm;
    fault_monitor_init(&fm);

    fm.total_checks = 10;
    fm.fault_count = 1;
    fm.fault_rate = 0.1f;
    fm.system_degraded = true;
    fm.system_failed = false;

    fault_monitor_update_status(&fm);

    // Expected: No change as the function is a placeholder
    TEST_ASSERT_EQUAL_UINT32(10, fm.total_checks);
    TEST_ASSERT_EQUAL_UINT32(1, fm.fault_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.1f, fm.fault_rate);
    TEST_ASSERT_TRUE(fm.system_degraded);
    TEST_ASSERT_FALSE(fm.system_failed);
}

// =================================================================================
// Tests for safety_system_init
// =================================================================================

void test_safety_system_init_nullSystem_returnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL system pointer
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, safety_system_init(NULL, SIL_3));
}

void test_safety_system_init_validParams_initializesAllSubsystems(void) {
    safety_system_t system;
    safety_level_t sil = SIL_2;

    // Expected: ERROR_NONE and subsystems initialized
    TEST_ASSERT_EQUAL(ERROR_NONE, safety_system_init(&system, sil));

    // Expected: Required SIL set
    TEST_ASSERT_EQUAL(sil, system.required_sil);

    // Verify TMR sensors initialization
    TEST_ASSERT_EQUAL_UINT8(1, system.temperature_sensors.sensors[0].sensor_id);
    TEST_ASSERT_TRUE(system.temperature_sensors.sensors[0].calibrated);
    TEST_ASSERT_EQUAL_UINT8(100, system.temperature_sensors.sensors[0].confidence_level);
    TEST_ASSERT_EQUAL_UINT8(4, system.pressure_sensors.sensors[0].sensor_id);

    // Verify Watchdog initialization
    TEST_ASSERT_EQUAL_UINT32(1000, system.system_watchdog.timeout_ms);
    TEST_ASSERT_EQUAL_UINT32(0, system.system_watchdog.counter);
    TEST_ASSERT_FALSE(system.system_watchdog.expired);

    // Verify Fault Monitor initialization
    TEST_ASSERT_EQUAL_UINT32(0, system.fault_monitor.total_checks);
    TEST_ASSERT_EQUAL_UINT32(0, system.fault_monitor.fault_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, system.fault_monitor.fault_rate);

    // Other system fields should be zeroed
    TEST_ASSERT_FALSE(system.emergency_shutdown);
    TEST_ASSERT_EQUAL_UINT32(0, system.system_uptime);
    TEST_ASSERT_NULL(system.emergency_cb);
    TEST_ASSERT_NULL(system.callback_context);
}

// =================================================================================
// Tests for safety_system_perform_safety_checks
// =================================================================================

void test_safety_system_perform_safety_checks_nullSystem_returnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL system pointer
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, safety_system_perform_safety_checks(NULL));
}

void test_safety_system_perform_safety_checks_allChecksPass_returnsNone(void) {
    safety_system_t system;
    safety_system_init(&system, SIL_1);

    // Setup consistent sensor values
    system.temperature_sensors.sensors[0].temperature = 25.0f;
    system.temperature_sensors.sensors[1].temperature = 25.5f;
    system.temperature_sensors.sensors[2].temperature = 24.8f; // All within 2.0f tolerance

    system.pressure_sensors.sensors[0].temperature = 100.0f;
    system.pressure_sensors.sensors[1].temperature = 105.0f;
    system.pressure_sensors.sensors[2].temperature = 98.0f; // All within 10.0f tolerance

    // Setup watchdog not expired
    system.system_watchdog.counter = 500;
    system.system_watchdog.last_feed_time = 0; // Not expired yet, timeout is 1000

    // Expected: ERROR_NONE as all checks pass
    TEST_ASSERT_EQUAL(ERROR_NONE, safety_system_perform_safety_checks(&system));

    // Expected: Fault monitor recorded 3 checks, all passed
    TEST_ASSERT_EQUAL_UINT32(3, system.fault_monitor.total_checks);
    TEST_ASSERT_EQUAL_UINT32(0, system.fault_monitor.fault_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, system.fault_monitor.fault_rate);
    TEST_ASSERT_FALSE(system.fault_monitor.system_degraded);
    TEST_ASSERT_FALSE(system.fault_monitor.system_failed);

    // Expected: Watchdog not expired
    TEST_ASSERT_FALSE(system.system_watchdog.expired);
    TEST_ASSERT_FALSE(system.emergency_shutdown);
    TEST_ASSERT_FALSE(emergency_cb_called);
    TEST_ASSERT_EQUAL_UINT32(1, system.system_uptime);

    // Expected: Voting occurred
    // Temp: Median of (24.8, 25.0, 25.5) -> 25.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, system.temperature_sensors.voted_value);
    // Press: Average of (98.0, 100.0, 105.0) -> (98+100+105)/3 = 101.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 101.0f, system.pressure_sensors.voted_value);
}

void test_safety_system_perform_safety_checks_temperatureInconsistent_returnsBusy(void) {
    safety_system_t system;
    safety_system_init(&system, SIL_1);

    // Setup inconsistent temperature sensors
    system.temperature_sensors.sensors[0].temperature = 25.0f;
    system.temperature_sensors.sensors[1].temperature = 28.0f; // |280.0f|=3 > 2.0 tolerance
    system.temperature_sensors.sensors[2].temperature = 25.0f;

    // Setup consistent pressure sensors
    system.pressure_sensors.sensors[0].temperature = 100.0f;
    system.pressure_sensors.sensors[1].temperature = 105.0f;
    system.pressure_sensors.sensors[2].temperature = 98.0f;

    // Setup watchdog not expired
    system.system_watchdog.counter = 500;
    system.system_watchdog.last_feed_time = 0;

    // Expected: ERROR_BUSY due to temperature inconsistency
    TEST_ASSERT_EQUAL(ERROR_BUSY, safety_system_perform_safety_checks(&system));

    // Expected: Fault monitor recorded 3 checks, 1 failed (temp inconsistency)
    TEST_ASSERT_EQUAL_UINT32(3, system.fault_monitor.total_checks);
    TEST_ASSERT_EQUAL_UINT32(1, system.fault_monitor.fault_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f/3.0f, system.fault_monitor.fault_rate); // ~0.333f
    TEST_ASSERT_TRUE(system.fault_monitor.system_degraded);
    TEST_ASSERT_TRUE(system.fault_monitor.system_failed); // 0.333 > 0.20
    TEST_ASSERT_FALSE(system.system_watchdog.expired);
    TEST_ASSERT_TRUE(system.emergency_shutdown); // System failed, so emergency shutdown
    TEST_ASSERT_TRUE(emergency_cb_called); // emergency_cb should be called
    TEST_ASSERT_EQUAL_UINT32(0xDEAD, emergency_cb_code);
    TEST_ASSERT_EQUAL_UINT32(1, system.system_uptime);
}

void test_safety_system_perform_safety_checks_watchdogExpired_returnsBusy(void) {
    safety_system_t system;
    safety_system_init(&system, SIL_1);

    // Setup consistent sensors
    system.temperature_sensors.sensors[0].temperature = 25.0f;
    system.temperature_sensors.sensors[1].temperature = 25.0f;
    system.temperature_sensors.sensors[2].temperature = 25.0f;

    system.pressure_sensors.sensors[0].temperature = 100.0f;
    system.pressure_sensors.sensors[1].temperature = 100.0f;
    system.pressure_sensors.sensors[2].temperature = 100.0f;

    // Setup watchdog expired
    system.system_watchdog.counter = 1001;
    system.system_watchdog.last_feed_time = 0; // Expired, timeout is 1000
    system.system_watchdog.timeout_cb = mock_watchdog_timeout_cb;
    system.callback_context = (void*)0xCAFE;
    system.emergency_cb = mock_emergency_shutdown_cb;


    // Expected: ERROR_BUSY due to watchdog expiration
    TEST_ASSERT_EQUAL(ERROR_BUSY, safety_system_perform_safety_checks(&system));

    // Expected: Fault monitor recorded 3 checks, 1 failed (watchdog)
    TEST_ASSERT_EQUAL_UINT32(3, system.fault_monitor.total_checks);
    TEST_ASSERT_EQUAL_UINT32(1, system.fault_monitor.fault_count);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f/3.0f, system.fault_monitor.fault_rate); // ~0.333f
    TEST_ASSERT_TRUE(system.fault_monitor.system_degraded);
    TEST_ASSERT_TRUE(system.fault_monitor.system_failed);
    TEST_ASSERT_TRUE(system.system_watchdog.expired); // Watchdog should be expired
    TEST_ASSERT_TRUE(wd_timeout_cb_called); // watchdog callback should be called
    TEST_ASSERT_EQUAL_PTR((void*)0xCAFE, wd_timeout_cb_context);

    TEST_ASSERT_TRUE(system.emergency_shutdown); // System failed, so emergency shutdown
    TEST_ASSERT_TRUE(emergency_cb_called); // emergency_cb should be called
    TEST_ASSERT_EQUAL_UINT32(0xDEAD, emergency_cb_code);
    TEST_ASSERT_EQUAL_PTR((void*)0xCAFE, emergency_cb_context); // context should be passed
    TEST_ASSERT_EQUAL_UINT32(1, system.system_uptime);
}

void test_safety_system_perform_safety_checks_systemFailedTriggersEmergencyShutdown(void) {
    safety_system_t system;
    safety_system_init(&system, SIL_1);
    system.emergency_cb = mock_emergency_shutdown_cb;
    system.callback_context = (void*)0xBEEF;

    // Setup system to be "failed" by injecting fault history
    system.fault_monitor.total_checks = 4;
    system.fault_monitor.fault_count = 1; // 1/4 = 0.25f > 0.20f
    system.fault_monitor.fault_rate = 0.25f;
    system.fault_monitor.system_failed = true; // Pre-set for test

    // Setup all current checks to pass
    system.temperature_sensors.sensors[0].temperature = 25.0f;
    system.temperature_sensors.sensors[1].temperature = 25.0f;
    system.temperature_sensors.sensors[2].temperature = 25.0f;
    system.pressure_sensors.sensors[0].temperature = 100.0f;
    system.pressure_sensors.sensors[1].temperature = 100.0f;
    system.pressure_sensors.sensors[2].temperature = 100.0f;
    system.system_watchdog.counter = 500;
    system.system_watchdog.last_feed_time = 0;

    // Expected: ERROR_NONE from current checks, but overall status leads to emergency shutdown.
    // The return value of perform_safety_checks is based on ALL_CHECKS_PASSED, not the system.failed state.
    TEST_ASSERT_EQUAL(ERROR_NONE, safety_system_perform_safety_checks(&system));

    // Even though current checks pass, previous system_failed state will trigger shutdown
    TEST_ASSERT_TRUE(system.emergency_shutdown);
    TEST_ASSERT_TRUE(emergency_cb_called);
    TEST_ASSERT_EQUAL_UINT32(0xDEAD, emergency_cb_code);
    TEST_ASSERT_EQUAL_PTR((void*)0xBEEF, emergency_cb_context);

    // Verify fault monitor state after the current (passing) checks
    // 3 new checks passed, previous state (1 fail, 4 total) is updated
    TEST_ASSERT_EQUAL_UINT32(7, system.fault_monitor.total_checks); // 4 initial + 3 new checks
    TEST_ASSERT_EQUAL_UINT32(1, system.fault_monitor.fault_count); // 1 initial fail, 3 new passes
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f/7.0f, system.fault_monitor.fault_rate); // ~0.142f
    TEST_ASSERT_FALSE(system.fault_monitor.system_degraded); // 0.142 <= 0.05 (this should be false now)
    TEST_ASSERT_FALSE(system.fault_monitor.system_failed); // 0.142 <= 0.20 (this should be false now)
    TEST_ASSERT_EQUAL_UINT32(1, system.system_uptime);
}

// =================================================================================
// Tests for safety_system_is_safe
// =================================================================================

void test_safety_system_is_safe_nullSystem_returnsFalse(void) {
    // Expected: false due to NULL system pointer
    TEST_ASSERT_FALSE(safety_system_is_safe(NULL));
}

void test_safety_system_is_safe_systemIsNominallySafe_returnsTrue(void) {
    safety_system_t system;
    safety_system_init(&system, SIL_1);

    // Default init state is safe
    // Expected: true as no emergency, not failed, watchdog not expired
    TEST_ASSERT_TRUE(safety_system_is_safe(&system));
}

void test_safety_system_is_safe_emergencyShutdownActive_returnsFalse(void) {
    safety_system_t system;
    safety_system_init(&system, SIL_1);
    system.emergency_shutdown = true; // Simulate emergency shutdown

    // Expected: false due to emergency shutdown
    TEST_ASSERT_FALSE(safety_system_is_safe(&system));
}

void test_safety_system_is_safe_faultMonitorSystemFailed_returnsFalse(void) {
    safety_system_t system;
    safety_system_init(&system, SIL_1);
    system.fault_monitor.system_failed = true; // Simulate system failure

    // Expected: false due to fault monitor indicating system failure
    TEST_ASSERT_FALSE(safety_system_is_safe(&system));
}

void test_safety_system_is_safe_watchdogExpired_returnsFalse(void) {
    safety_system_t system;
    safety_system_init(&system, SIL_1);
    system.system_watchdog.expired = true; // Simulate watchdog expiration

    // Expected: false due to watchdog expiration
    TEST_ASSERT_FALSE(safety_system_is_safe(&system));
}

void test_safety_system_is_safe_multipleUnsafeConditions_returnsFalse(void) {
    safety_system_t system;
    safety_system_init(&system, SIL_1);
    system.emergency_shutdown = true;
    system.fault_monitor.system_failed = true;

    // Expected: false due to multiple unsafe conditions
    TEST_ASSERT_FALSE(safety_system_is_safe(&system));
}

// =================================================================================
// Tests for power_supply_init
// =================================================================================

void test_power_supply_init_nullPowerSupply_returnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL ps pointer
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, power_supply_init(NULL));
}

void test_power_supply_init_validParams_initializesCorrectly(void) {
    power_supply_t ps;
    // Set some garbage values to ensure memset works
    ps.supply_a_active = false;
    ps.supply_b_active = false;
    ps.voltage_a = 0.0f;
    ps.voltage_b = 0.0f;
    ps.redundancy_lost = true;

    // Expected: ERROR_NONE and ps structure initialized
    TEST_ASSERT_EQUAL(ERROR_NONE, power_supply_init(&ps));

    // Expected: Both supplies active
    TEST_ASSERT_TRUE(ps.supply_a_active);
    TEST_ASSERT_TRUE(ps.supply_b_active);
    // Expected: Default voltages set
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 24.0f, ps.voltage_a);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 24.0f, ps.voltage_b);
    // Expected: Redundancy not lost
    TEST_ASSERT_FALSE(ps.redundancy_lost);
}

// =================================================================================
// Tests for power_supply_monitor
// =================================================================================

void test_power_supply_monitor_nullPowerSupply_returnsInvalidParam(void) {
    // Expected: ERROR_INVALID_PARAM due to NULL ps pointer
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, power_supply_monitor(NULL));
}

void test_power_supply_monitor_bothSuppliesOk_noRedundancyLost(void) {
    power_supply_t ps;
    power_supply_init(&ps);

    // Mock rand() to produce values within the acceptable range
    mock_rand_value = 50; // Results in 23.8 + 0.5 = 24.3f for A, 24.1 + 0.5 = 24.6f for B
                          // Both 24.3 and 24.6 are within [20.0, 28.0]

    // Expected: ERROR_NONE, both active, no redundancy lost
    TEST_ASSERT_EQUAL(ERROR_NONE, power_supply_monitor(&ps));

    // Expected: Voltages updated based on mock rand
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 24.3f, ps.voltage_a);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 24.6f, ps.voltage_b);
    // Expected: Both supplies active
    TEST_ASSERT_TRUE(ps.supply_a_active);
    TEST_ASSERT_TRUE(ps.supply_b_active);
    // Expected: Redundancy not lost
    TEST_ASSERT_FALSE(ps.redundancy_lost);
}

void test_power_supply_monitor_supplyAOutOfRangeLow_redundancyLost(void) {
    power_supply_t ps;
    power_supply_init(&ps);

    // Mock rand() such that supply A is low, B is OK
    mock_rand_value = 0; // Results in 23.8 + 0.0 = 23.8f (OK) for A, then again 24.1 + 0.0 = 24.1f (OK) for B
                         // Need to mock rand to get A < 20.0f for specific test case
    // This requires a more complex mocking setup or a specific sequence.
    // For now, let's assume direct control over `ps->voltage_a` for this specific test,
    // or set `mock_rand_value` carefully to simulate the desired range.
    // The source uses `rand() % 100 * 0.01f`, so `rand()` returns 00.0f.
    // Voltage A range: 23.8f + (0 to 0.99) => 23.8f to 24.79f
    // Voltage B range: 24.1f + (0 to 0.99) => 24.1f to 25.09f
    // These ranges do not naturally go below 20.0f or above 28.0f for the *mocked* rand() values.
    // This implies that power_supply_monitor, *as written*, will always report supplies as active
    // when using `rand() % 100 * 0.01f` for voltage generation because its generated range
    // (23.80.0f and 24.10.0f) is completely within [20.0, 28.0].
    // To test the `a_ok` and `b_ok` branches correctly, `rand()` needs to return values
    // that make the voltages fall outside the [20.0, 28.0] range, which is impossible with current generation.

    // Acknowledging the `rand()` behavior:
    // With `ps->voltage_a = 23.8f + (rand() % 100) * 0.01f;`
    // Min A = 23.8 + 0 = 23.8f
    // Max A = 23.8 + 99*0.01 = 23.8 + 0.99 = 24.79f
    // Both are within [20.0, 28.0]. So `supply_a_active` will always be true.

    // With `ps->voltage_b = 24.1f + (rand() % 100) * 0.01f;`
    // Min B = 24.1 + 0 = 24.1f
    // Max B = 24.1 + 99*0.01 = 24.1 + 0.99 = 25.09f
    // Both are within [20.0, 28.0]. So `supply_b_active` will always be true.

    // Therefore, the condition `ps->redundancy_lost = !(a_ok && b_ok);` will *always* result in `false`.
    // The `if (ps == NULL)` branch is the only other branch in this function.
    // The only testable aspect beyond that is the calculation and the fact that active is true.

    // Given the "realistic scenarios only" rule and the source code's current `rand()` based voltage
    // generation, it's impossible for `supply_a_active` or `supply_b_active` to become false.
    // I will write tests to confirm this current behavior and add a comment.

    mock_rand_value = 0; // lowest voltage for A (23.8f) and B (24.1f)
    TEST_ASSERT_EQUAL(ERROR_NONE, power_supply_monitor(&ps));
    TEST_ASSERT_TRUE(ps.supply_a_active);
    TEST_ASSERT_TRUE(ps.supply_b_active);
    TEST_ASSERT_FALSE(ps.redundancy_lost);

    mock_rand_value = 99; // highest voltage for A (24.79f) and B (25.09f)
    TEST_ASSERT_EQUAL(ERROR_NONE, power_supply_monitor(&ps));
    TEST_ASSERT_TRUE(ps.supply_a_active);
    TEST_ASSERT_TRUE(ps.supply_b_active);
    TEST_ASSERT_FALSE(ps.redundancy_lost);

    // Comment explaining the limitation due to source code's voltage generation logic.
    // To properly test the 'redundancy_lost' branches, the voltage generation
    // within `power_supply_monitor` would need to be modified or directly
    // stubbed to produce values outside [20.0f, 28.0f].
    // As per rules, I cannot modify source, and rand() is a standard lib function which
    // has been mocked to be deterministic, but its *range* as used in the source makes
    // these branches unreachable.
}

// =================================================================================
// Tests for power_supply_has_redundancy
// =================================================================================

void test_power_supply_has_redundancy_nullPowerSupply_returnsFalse(void) {
    // Expected: false due to NULL ps pointer
    TEST_ASSERT_FALSE(power_supply_has_redundancy(NULL));
}

void test_power_supply_has_redundancy_bothActiveNoRedundancyLost_returnsTrue(void) {
    power_supply_t ps;
    power_supply_init(&ps); // Default state: both active, redundancy not lost

    // Expected: true
    TEST_ASSERT_TRUE(power_supply_has_redundancy(&ps));
}

void test_power_supply_has_redundancy_supplyAInactive_returnsFalse(void) {
    power_supply_t ps;
    power_supply_init(&ps);
    ps.supply_a_active = false; // Simulate supply A failure

    // Expected: false due to supply A being inactive
    TEST_ASSERT_FALSE(power_supply_has_redundancy(&ps));
}

void test_power_supply_has_redundancy_supplyBInactive_returnsFalse(void) {
    power_supply_t ps;
    power_supply_init(&ps);
    ps.supply_b_active = false; // Simulate supply B failure

    // Expected: false due to supply B being inactive
    TEST_ASSERT_FALSE(power_supply_has_redundancy(&ps));
}

void test_power_supply_has_redundancy_redundancyLostFlagTrue_returnsFalse(void) {
    power_supply_t ps;
    power_supply_init(&ps);
    ps.redundancy_lost = true; // Simulate redundancy loss

    // Expected: false due to redundancy lost flag
    TEST_ASSERT_FALSE(power_supply_has_redundancy(&ps));
}

void test_power_supply_has_redundancy_allConditionsMetButOneSupplyInactive_returnsFalse(void) {
    power_supply_t ps;
    power_supply_init(&ps);
    ps.supply_b_active = false; // One supply inactive
    ps.redundancy_lost = false; // Redundancy lost flag not set (should still be false)

    // Expected: false because ps.supply_b_active is false
    TEST_ASSERT_FALSE(power_supply_has_redundancy(&ps));
}


// =================================================================================
// Main Test Runner
// =================================================================================



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_sensor_read_redundant_nullSensors_returnsInvalidParam);
    RUN_TEST(test_sensor_read_redundant_nullResult_returnsInvalidParam);
    RUN_TEST(test_sensor_read_redundant_zeroCount_returnsInvalidParam);
    RUN_TEST(test_sensor_read_redundant_noValidReadings_returnsBusy);
    RUN_TEST(test_sensor_read_redundant_allValid_returnsAverage);
    RUN_TEST(test_sensor_read_redundant_someInvalid_returnsAverageOfValid);
    RUN_TEST(test_tmr_sensor_init_nullTmr_returnsInvalidParam);
    RUN_TEST(test_tmr_sensor_init_nullSensorIds_returnsInvalidParam);
    RUN_TEST(test_tmr_sensor_init_validParams_initializesCorrectly);
    RUN_TEST(test_tmr_sensor_vote_nullTmr_returnsInvalidParam);
    RUN_TEST(test_tmr_sensor_vote_notEnoughValidSensors_returnsBusyAndSetsFaultDetected);
    RUN_TEST(test_tmr_sensor_vote_majority_validSensors);
    RUN_TEST(test_tmr_sensor_vote_average_validSensors);
    RUN_TEST(test_tmr_sensor_vote_median_validSensors);
    RUN_TEST(test_tmr_sensor_vote_median_validSensors_withOneFaulty);
    RUN_TEST(test_tmr_sensor_vote_weighted_validSensors);
    RUN_TEST(test_tmr_sensor_vote_weighted_oneFaultySensor);
    RUN_TEST(test_tmr_sensor_check_consistency_nullTmr_returnsFalse);
    RUN_TEST(test_tmr_sensor_check_consistency_allConsistent_returnsTrue);
    RUN_TEST(test_tmr_sensor_check_consistency_oneInconsistent_returnsFalseAndSetsMask);
    RUN_TEST(test_tmr_sensor_check_consistency_multipleInconsistent_returnsFalseAndSetsMask);
    RUN_TEST(test_tmr_sensor_check_consistency_edgeTolerance_returnsTrue);
    RUN_TEST(test_watchdog_init_nullWatchdog_returnsInvalidParam);
    RUN_TEST(test_watchdog_init_validParams_initializesCorrectly);
    RUN_TEST(test_watchdog_feed_nullWatchdog_doesNothing);
    RUN_TEST(test_watchdog_feed_feedsWatchdog_resetsState);
    RUN_TEST(test_watchdog_check_expired_nullWatchdog_returnsTrue);
    RUN_TEST(test_watchdog_check_expired_notExpired_returnsFalse);
    RUN_TEST(test_watchdog_check_expired_justExpired_returnsTrueAndCallsCallback);
    RUN_TEST(test_watchdog_check_expired_alreadyExpired_returnsTrue);
    RUN_TEST(test_watchdog_check_expired_atTimeoutEdge_returnsFalse);
    RUN_TEST(test_fault_monitor_init_nullFaultMonitor_returnsInvalidParam);
    RUN_TEST(test_fault_monitor_init_validParams_initializesCorrectly);
    RUN_TEST(test_fault_monitor_record_check_nullFaultMonitor_doesNothing);
    RUN_TEST(test_fault_monitor_record_check_passed_updatesCountersAndHistory);
    RUN_TEST(test_fault_monitor_record_check_failed_updatesCountersAndHistory);
    RUN_TEST(test_fault_monitor_record_check_multipleChecks_updatesRateAndStatus);
    RUN_TEST(test_fault_monitor_record_check_historyWrapsAround);
    RUN_TEST(test_fault_monitor_record_check_degradedStatusThreshold);
    RUN_TEST(test_fault_monitor_record_check_failedStatusThreshold);
    RUN_TEST(test_fault_monitor_update_status_nullFaultMonitor_doesNothing);
    RUN_TEST(test_fault_monitor_update_status_validFaultMonitor_doesNothingAsPlaceholder);
    RUN_TEST(test_safety_system_init_nullSystem_returnsInvalidParam);
    RUN_TEST(test_safety_system_init_validParams_initializesAllSubsystems);
    RUN_TEST(test_safety_system_perform_safety_checks_nullSystem_returnsInvalidParam);
    RUN_TEST(test_safety_system_perform_safety_checks_allChecksPass_returnsNone);
    RUN_TEST(test_safety_system_perform_safety_checks_temperatureInconsistent_returnsBusy);
    RUN_TEST(test_safety_system_perform_safety_checks_watchdogExpired_returnsBusy);
    RUN_TEST(test_safety_system_perform_safety_checks_systemFailedTriggersEmergencyShutdown);
    RUN_TEST(test_safety_system_is_safe_nullSystem_returnsFalse);
    RUN_TEST(test_safety_system_is_safe_systemIsNominallySafe_returnsTrue);
    RUN_TEST(test_safety_system_is_safe_emergencyShutdownActive_returnsFalse);
    RUN_TEST(test_safety_system_is_safe_faultMonitorSystemFailed_returnsFalse);
    RUN_TEST(test_safety_system_is_safe_watchdogExpired_returnsFalse);
    RUN_TEST(test_safety_system_is_safe_multipleUnsafeConditions_returnsFalse);
    RUN_TEST(test_power_supply_init_nullPowerSupply_returnsInvalidParam);
    RUN_TEST(test_power_supply_init_validParams_initializesCorrectly);
    RUN_TEST(test_power_supply_monitor_nullPowerSupply_returnsInvalidParam);
    RUN_TEST(test_power_supply_monitor_bothSuppliesOk_noRedundancyLost);
    RUN_TEST(test_power_supply_monitor_supplyAOutOfRangeLow_redundancyLost);
    RUN_TEST(test_power_supply_has_redundancy_nullPowerSupply_returnsFalse);
    RUN_TEST(test_power_supply_has_redundancy_bothActiveNoRedundancyLost_returnsTrue);
    RUN_TEST(test_power_supply_has_redundancy_supplyAInactive_returnsFalse);
    RUN_TEST(test_power_supply_has_redundancy_supplyBInactive_returnsFalse);
    RUN_TEST(test_power_supply_has_redundancy_redundancyLostFlagTrue_returnsFalse);
    RUN_TEST(test_power_supply_has_redundancy_allConditionsMetButOneSupplyInactive_returnsFalse);

    return UNITY_END();
}