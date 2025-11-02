#include "safety_critical.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Sensor Functions
error_t sensor_read_redundant(sensor_data_t *sensors, uint8_t count, float *result) {
    if (sensors == NULL || result == NULL || count == 0) {
        return ERROR_INVALID_PARAM;
    }

    uint8_t valid_readings = 0;
    float sum = 0.0f;

    for (uint8_t i = 0; i < count; i++) {
        if (sensors[i].last_error == ERROR_NONE && sensors[i].calibrated) {
            sum += sensors[i].temperature;  // Assuming temperature for simplicity
            valid_readings++;
        }
    }

    if (valid_readings == 0) {
        return ERROR_BUSY;  // No valid readings
    }

    *result = sum / valid_readings;
    return ERROR_NONE;
}

// TMR Sensor Functions
error_t tmr_sensor_init(tmr_sensor_t *tmr, uint8_t sensor_ids[TMR_SENSOR_COUNT]) {
    if (tmr == NULL || sensor_ids == NULL) {
        return ERROR_INVALID_PARAM;
    }

    memset(tmr, 0, sizeof(tmr_sensor_t));

    for (uint8_t i = 0; i < TMR_SENSOR_COUNT; i++) {
        tmr->sensors[i].sensor_id = sensor_ids[i];
        tmr->sensors[i].calibrated = true;
        tmr->sensors[i].confidence_level = 100;
        tmr->sensors[i].last_error = ERROR_NONE;
    }

    return ERROR_NONE;
}

error_t tmr_sensor_vote(tmr_sensor_t *tmr, voting_method_t method) {
    if (tmr == NULL) {
        return ERROR_INVALID_PARAM;
    }

    float values[TMR_SENSOR_COUNT];
    uint8_t valid_count = 0;

    // Collect valid sensor values
    for (uint8_t i = 0; i < TMR_SENSOR_COUNT; i++) {
        if (tmr->sensors[i].last_error == ERROR_NONE) {
            values[valid_count++] = tmr->sensors[i].temperature;
        }
    }

    if (valid_count < 2) {
        tmr->fault_detected = true;
        return ERROR_BUSY;  // Not enough valid sensors
    }

    // Perform voting based on method
    switch (method) {
        case VOTE_MAJORITY:
            // Simple majority vote (for discrete values)
            if (valid_count >= 2) {
                tmr->voted_value = values[0];  // Simplified
            }
            break;

        case VOTE_AVERAGE:
            {
                float sum = 0.0f;
                for (uint8_t i = 0; i < valid_count; i++) {
                    sum += values[i];
                }
                tmr->voted_value = sum / valid_count;
            }
            break;

        case VOTE_MEDIAN:
            // Sort and pick median
            for (uint8_t i = 0; i < valid_count - 1; i++) {
                for (uint8_t j = i + 1; j < valid_count; j++) {
                    if (values[i] > values[j]) {
                        float temp = values[i];
                        values[i] = values[j];
                        values[j] = temp;
                    }
                }
            }
            tmr->voted_value = values[valid_count / 2];
            break;

        case VOTE_WEIGHTED:
            // Weight by confidence level
            {
                float weighted_sum = 0.0f;
                float total_weight = 0.0f;
                for (uint8_t i = 0; i < TMR_SENSOR_COUNT; i++) {
                    if (tmr->sensors[i].last_error == ERROR_NONE) {
                        float weight = tmr->sensors[i].confidence_level / 100.0f;
                        weighted_sum += values[i] * weight;
                        total_weight += weight;
                    }
                }
                tmr->voted_value = weighted_sum / total_weight;
            }
            break;
    }

    tmr->last_vote_time = 0;  // Mock timestamp
    tmr->fault_detected = false;

    return ERROR_NONE;
}

bool tmr_sensor_check_consistency(tmr_sensor_t *tmr, float tolerance) {
    if (tmr == NULL) return false;

    // Check if all sensors agree within tolerance
    float reference = tmr->sensors[0].temperature;

    for (uint8_t i = 1; i < TMR_SENSOR_COUNT; i++) {
        if (fabsf(tmr->sensors[i].temperature - reference) > tolerance) {
            tmr->faulty_sensor_mask |= (1 << i);
            return false;
        }
    }

    tmr->faulty_sensor_mask = 0;
    return true;
}

// Watchdog Functions
error_t watchdog_init(watchdog_t *wd, uint32_t timeout_ms) {
    if (wd == NULL) {
        return ERROR_INVALID_PARAM;
    }

    wd->timeout_ms = timeout_ms;
    wd->counter = 0;
    wd->last_feed_time = 0;
    wd->expired = false;

    return ERROR_NONE;
}

void watchdog_feed(watchdog_t *wd) {
    if (wd == NULL) return;

    wd->last_feed_time = wd->counter;  // Mock time
    wd->expired = false;
}

bool watchdog_check_expired(watchdog_t *wd) {
    if (wd == NULL) return true;

    // Mock expiration check
    if (wd->counter - wd->last_feed_time > wd->timeout_ms) {
        wd->expired = true;
        if (wd->timeout_cb) {
            wd->timeout_cb(wd->callback_context);
        }
    }

    return wd->expired;
}

// Fault Monitor Functions
error_t fault_monitor_init(fault_monitor_t *fm) {
    if (fm == NULL) {
        return ERROR_INVALID_PARAM;
    }

    memset(fm, 0, sizeof(fault_monitor_t));
    return ERROR_NONE;
}

void fault_monitor_record_check(fault_monitor_t *fm, bool passed) {
    if (fm == NULL) return;

    fm->total_checks++;
    if (!passed) {
        fm->fault_count++;
        fm->fault_history[fm->history_index] = 1;
    } else {
        fm->fault_history[fm->history_index] = 0;
    }

    fm->history_index = (fm->history_index + 1) % 32;

    // Update fault rate
    fm->fault_rate = (float)fm->fault_count / fm->total_checks;

    // Update system status
    fm->system_degraded = (fm->fault_rate > 0.05f);  // 5% fault rate
    fm->system_failed = (fm->fault_rate > 0.20f);    // 20% fault rate
}

void fault_monitor_update_status(fault_monitor_t *fm) {
    // Additional status updates can be added here
}

// Safety System Functions
error_t safety_system_init(safety_system_t *system, safety_level_t sil) {
    if (system == NULL) {
        return ERROR_INVALID_PARAM;
    }

    memset(system, 0, sizeof(safety_system_t));
    system->required_sil = sil;

    // Initialize TMR sensors
    uint8_t temp_ids[TMR_SENSOR_COUNT] = {1, 2, 3};
    uint8_t press_ids[TMR_SENSOR_COUNT] = {4, 5, 6};

    tmr_sensor_init(&system->temperature_sensors, temp_ids);
    tmr_sensor_init(&system->pressure_sensors, press_ids);

    // Initialize watchdog
    watchdog_init(&system->system_watchdog, 1000);  // 1 second timeout

    // Initialize fault monitor
    fault_monitor_init(&system->fault_monitor);

    return ERROR_NONE;
}

error_t safety_system_perform_safety_checks(safety_system_t *system) {
    if (system == NULL) {
        return ERROR_INVALID_PARAM;
    }

    bool all_checks_passed = true;

    // Check TMR sensor consistency
    bool temp_consistent = tmr_sensor_check_consistency(&system->temperature_sensors, 2.0f);
    bool press_consistent = tmr_sensor_check_consistency(&system->pressure_sensors, 10.0f);

    fault_monitor_record_check(&system->fault_monitor, temp_consistent);
    fault_monitor_record_check(&system->fault_monitor, press_consistent);

    if (!temp_consistent || !press_consistent) {
        all_checks_passed = false;
    }

    // Check watchdog
    bool wd_expired = watchdog_check_expired(&system->system_watchdog);
    fault_monitor_record_check(&system->fault_monitor, !wd_expired);

    if (wd_expired) {
        all_checks_passed = false;
    }

    // Perform voting
    tmr_sensor_vote(&system->temperature_sensors, VOTE_MEDIAN);
    tmr_sensor_vote(&system->pressure_sensors, VOTE_AVERAGE);

    // Emergency shutdown if system failed
    if (system->fault_monitor.system_failed) {
        system->emergency_shutdown = true;
        if (system->emergency_cb) {
            system->emergency_cb(0xDEAD, system->callback_context);
        }
    }

    system->system_uptime++;

    return all_checks_passed ? ERROR_NONE : ERROR_BUSY;
}

bool safety_system_is_safe(safety_system_t *system) {
    if (system == NULL) return false;

    return !system->emergency_shutdown &&
           !system->fault_monitor.system_failed &&
           !system->system_watchdog.expired;
}

// Power Supply Functions
error_t power_supply_init(power_supply_t *ps) {
    if (ps == NULL) {
        return ERROR_INVALID_PARAM;
    }

    memset(ps, 0, sizeof(power_supply_t));
    ps->supply_a_active = true;
    ps->supply_b_active = true;
    ps->voltage_a = 24.0f;
    ps->voltage_b = 24.0f;

    return ERROR_NONE;
}

error_t power_supply_monitor(power_supply_t *ps) {
    if (ps == NULL) {
        return ERROR_INVALID_PARAM;
    }

    // Mock voltage monitoring
    ps->voltage_a = 23.8f + (rand() % 100) * 0.01f;  // 23.8V to 24.7V
    ps->voltage_b = 24.1f + (rand() % 100) * 0.01f;  // 24.1V to 25.0V

    // Check if supplies are within acceptable range
    bool a_ok = (ps->voltage_a >= 20.0f && ps->voltage_a <= 28.0f);
    bool b_ok = (ps->voltage_b >= 20.0f && ps->voltage_b <= 28.0f);

    ps->supply_a_active = a_ok;
    ps->supply_b_active = b_ok;

    ps->redundancy_lost = !(a_ok && b_ok);

    return ERROR_NONE;
}

bool power_supply_has_redundancy(power_supply_t *ps) {
    if (ps == NULL) return false;

    return ps->supply_a_active && ps->supply_b_active && !ps->redundancy_lost;
}