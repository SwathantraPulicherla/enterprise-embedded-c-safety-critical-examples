#ifndef SAFETY_CRITICAL_H
#define SAFETY_CRITICAL_H

#include <stdint.h>
#include <stdbool.h>
#include "embedded_hardware.h"  // For error_t definition

// Redundant Sensor Data Structure
typedef struct {
    float temperature;
    float pressure;
    uint8_t sensor_id;
    bool calibrated;
    error_t last_error;
    uint32_t timestamp;
    uint8_t confidence_level;  // 0-100
} sensor_data_t;

// Triple Modular Redundancy (TMR) Sensor Array
#define TMR_SENSOR_COUNT 3
typedef struct {
    sensor_data_t sensors[TMR_SENSOR_COUNT];
    float voted_value;
    bool fault_detected;
    uint8_t faulty_sensor_mask;  // Bit mask of faulty sensors
    uint32_t last_vote_time;
} tmr_sensor_t;

// Watchdog Timer Structure
typedef struct {
    volatile uint32_t counter;
    uint32_t timeout_ms;
    uint32_t last_feed_time;
    bool expired;
    void (*timeout_cb)(void* context);
    void *callback_context;
} watchdog_t;

// Fault Detection Structure
typedef struct {
    uint32_t fault_count;
    uint32_t total_checks;
    float fault_rate;
    uint8_t fault_history[32];  // Circular buffer of fault states
    uint8_t history_index;
    bool system_degraded;
    bool system_failed;
} fault_monitor_t;

// Voting Logic Types
typedef enum {
    VOTE_MAJORITY,
    VOTE_AVERAGE,
    VOTE_MEDIAN,
    VOTE_WEIGHTED
} voting_method_t;

// Safety Levels (SIL - Safety Integrity Level)
typedef enum {
    SIL_0,  // No safety requirements
    SIL_1,  // Low safety
    SIL_2,  // Medium safety
    SIL_3,  // High safety
    SIL_4   // Very high safety
} safety_level_t;

// Safety-Critical System Structure
typedef struct {
    tmr_sensor_t temperature_sensors;
    tmr_sensor_t pressure_sensors;
    watchdog_t system_watchdog;
    fault_monitor_t fault_monitor;
    safety_level_t required_sil;
    bool emergency_shutdown;
    uint32_t system_uptime;
    void (*emergency_cb)(uint32_t fault_code, void* context);
    void *callback_context;
} safety_system_t;

// Redundant Power Supply Structure
typedef struct {
    bool supply_a_active;
    bool supply_b_active;
    float voltage_a;
    float voltage_b;
    float current_a;
    float current_b;
    bool redundancy_lost;
    uint32_t last_switch_time;
} power_supply_t;

// Function declarations
error_t sensor_read_redundant(sensor_data_t *sensors, uint8_t count, float *result);
error_t tmr_sensor_init(tmr_sensor_t *tmr, uint8_t sensor_ids[TMR_SENSOR_COUNT]);
error_t tmr_sensor_vote(tmr_sensor_t *tmr, voting_method_t method);
bool tmr_sensor_check_consistency(tmr_sensor_t *tmr, float tolerance);

error_t watchdog_init(watchdog_t *wd, uint32_t timeout_ms);
void watchdog_feed(watchdog_t *wd);
bool watchdog_check_expired(watchdog_t *wd);

error_t fault_monitor_init(fault_monitor_t *fm);
void fault_monitor_record_check(fault_monitor_t *fm, bool passed);
void fault_monitor_update_status(fault_monitor_t *fm);

error_t safety_system_init(safety_system_t *system, safety_level_t sil);
error_t safety_system_perform_safety_checks(safety_system_t *system);
bool safety_system_is_safe(safety_system_t *system);

error_t power_supply_init(power_supply_t *ps);
error_t power_supply_monitor(power_supply_t *ps);
bool power_supply_has_redundancy(power_supply_t *ps);

#endif // SAFETY_CRITICAL_H