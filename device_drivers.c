#include "device_drivers.h"
#include <stdlib.h>
#include <string.h>

// UART Driver Functions
error_t uart_driver_init(uart_driver_t *driver, const uart_config_t *config) {
    if (driver == NULL || config == NULL) {
        return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_INIT;

    // Allocate hardware registers (mock)
    driver->uart = (USART_TypeDef*)malloc(sizeof(USART_TypeDef));
    if (driver->uart == NULL) {
        return ERROR_BUSY;
    }

    // Initialize UART hardware
    error_t err = uart_init(driver->uart, (uart_config_t*)config);
    if (err != ERROR_NONE) {
        free(driver->uart);
        driver->state = DEVICE_STATE_ERROR;
        return err;
    }

    driver->timeout_ms = 1000;  // Default timeout
    driver->error_index = 0;
    memset(driver->errors, 0, sizeof(driver->errors));

    driver->state = DEVICE_STATE_READY;

    if (driver->state_change_cb) {
        driver->state_change_cb(DEVICE_STATE_INIT, DEVICE_STATE_READY, driver->callback_context);
    }

    return ERROR_NONE;
}

error_t uart_driver_transmit(uart_driver_t *driver, const uint8_t *data, uint16_t size) {
    if (driver == NULL || data == NULL || size == 0 || driver->state != DEVICE_STATE_READY) {
        return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_BUSY;

    error_t err = uart_transmit(driver->uart, (uint8_t*)data, size, driver->timeout_ms);

    driver->state = DEVICE_STATE_READY;

    if (err != ERROR_NONE) {
        // Log error
        driver->errors[driver->error_index].timestamp = 0;  // Mock timestamp
        driver->errors[driver->error_index].error_code = err;
        driver->error_index = (driver->error_index + 1) % ERROR_HISTORY_SIZE;
        driver->state = DEVICE_STATE_ERROR;
    }

    return err;
}

error_t uart_driver_receive(uart_driver_t *driver, uint8_t *data, uint16_t size) {
    if (driver == NULL || data == NULL || size == 0 || driver->state != DEVICE_STATE_READY) {
        return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_BUSY;

    error_t err = uart_receive(driver->uart, data, size, driver->timeout_ms);

    driver->state = DEVICE_STATE_READY;

    if (err != ERROR_NONE) {
        driver->errors[driver->error_index].timestamp = 0;
        driver->errors[driver->error_index].error_code = err;
        driver->error_index = (driver->error_index + 1) % ERROR_HISTORY_SIZE;
        driver->state = DEVICE_STATE_ERROR;
    }

    return err;
}

void uart_driver_process_interrupt(uart_driver_t *driver) {
    if (driver == NULL) return;

    // Simulate interrupt processing
    if (driver->uart->SR & USART_SR_RXNE) {
        // RX interrupt
        uint8_t data = (uint8_t)driver->uart->DR;
        // Process received data
    }

    if (driver->uart->SR & USART_SR_TXE) {
        // TX interrupt
        // Send next byte
    }
}

// SPI Driver Functions
error_t spi_driver_init(spi_driver_t *driver, const spi_config_t *config) {
    if (driver == NULL || config == NULL) {
        return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_INIT;

    driver->spi = (SPI_TypeDef*)malloc(sizeof(SPI_TypeDef));
    if (driver->spi == NULL) {
        return ERROR_BUSY;
    }

    error_t err = spi_init(driver->spi, (spi_config_t*)config);
    if (err != ERROR_NONE) {
        free(driver->spi);
        driver->state = DEVICE_STATE_ERROR;
        return err;
    }

    driver->error_index = 0;
    memset(driver->errors, 0, sizeof(driver->errors));

    driver->state = DEVICE_STATE_READY;

    return ERROR_NONE;
}

error_t spi_driver_transfer(spi_driver_t *driver, const uint8_t *tx_data, uint8_t *rx_data, uint16_t size) {
    if (driver == NULL || tx_data == NULL || rx_data == NULL || driver->state != DEVICE_STATE_READY) {
        return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_BUSY;

    // Set CS low (mock)
    gpio_write_pin(driver->cs_gpio, driver->cs_pin, false);

    error_t err = spi_transmit_receive(driver->spi, (uint8_t*)tx_data, rx_data, size);

    // Set CS high
    gpio_write_pin(driver->cs_gpio, driver->cs_pin, true);

    driver->state = DEVICE_STATE_READY;

    if (err != ERROR_NONE) {
        driver->errors[driver->error_index].timestamp = 0;
        driver->errors[driver->error_index].error_code = err;
        driver->error_index = (driver->error_index + 1) % ERROR_HISTORY_SIZE;
        driver->state = DEVICE_STATE_ERROR;
    }

    return err;
}

// I2C Driver Functions
error_t i2c_driver_init(i2c_driver_t *driver, uint8_t address, uint32_t frequency) {
    if (driver == NULL) {
        return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_INIT;
    driver->device_addr = address;
    driver->timeout_ms = 100;
    driver->error_index = 0;
    memset(driver->errors, 0, sizeof(driver->errors));

    // Mock I2C initialization
    driver->i2c_regs = malloc(100);  // Mock registers
    if (driver->i2c_regs == NULL) {
        return ERROR_BUSY;
    }

    driver->state = DEVICE_STATE_READY;

    return ERROR_NONE;
}

error_t i2c_driver_write(i2c_driver_t *driver, uint8_t reg, const uint8_t *data, uint16_t size) {
    if (driver == NULL || data == NULL || driver->state != DEVICE_STATE_READY) {
        return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_BUSY;

    // Mock I2C write operation
    // In real implementation, this would handle START, address, register, data, STOP

    driver->state = DEVICE_STATE_READY;

    return ERROR_NONE;
}

error_t i2c_driver_read(i2c_driver_t *driver, uint8_t reg, uint8_t *data, uint16_t size) {
    if (driver == NULL || data == NULL || driver->state != DEVICE_STATE_READY) {
        return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_BUSY;

    // Mock I2C read operation
    memset(data, 0xAA, size);  // Mock data

    driver->state = DEVICE_STATE_READY;

    return ERROR_NONE;
}

// CAN Driver Functions
error_t can_driver_init(can_driver_t *driver, uint32_t bitrate) {
    if (driver == NULL) {
        return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_INIT;
    driver->bitrate = bitrate;
    driver->error_index = 0;
    memset(driver->errors, 0, sizeof(driver->errors));

    driver->can = (can_handle_t*)malloc(sizeof(can_handle_t));
    if (driver->can == NULL) {
        return ERROR_BUSY;
    }

    protocol_error_t err = can_init(driver->can, 32);  // 32 message buffer
    if (err != PROTOCOL_ERROR_NONE) {
        free(driver->can);
        driver->state = DEVICE_STATE_ERROR;
        return ERROR_BUSY;
    }

    driver->state = DEVICE_STATE_READY;

    return ERROR_NONE;
}

error_t can_driver_send_message(can_driver_t *driver, const can_frame_t *frame) {
    if (driver == NULL || frame == NULL || driver->state != DEVICE_STATE_READY) {
        return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_BUSY;

    protocol_error_t err = can_transmit_message(driver->can, frame, 1000);

    driver->state = DEVICE_STATE_READY;

    if (err != PROTOCOL_ERROR_NONE) {
        driver->errors[driver->error_index].timestamp = 0;
        driver->errors[driver->error_index].error_code = (error_t)err;
        driver->error_index = (driver->error_index + 1) % ERROR_HISTORY_SIZE;
        driver->state = DEVICE_STATE_ERROR;
        return ERROR_BUSY;
    }

    return ERROR_NONE;
}

// Sensor Driver Functions
error_t sensor_driver_init(sensor_driver_t *driver, uint8_t interface_type, uint8_t sensor_type) {
    if (driver == NULL) {
        return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_INIT;
    driver->sensor_type = sensor_type;
    driver->sampling_rate = 100;  // 100 Hz default
    driver->calibration_offset = 0.0f;
    driver->calibration_scale = 1.0f;
    driver->error_index = 0;
    memset(driver->errors, 0, sizeof(driver->errors));

    // Initialize appropriate interface
    switch (interface_type) {
        case 0:  // I2C
            driver->i2c = (i2c_driver_t*)malloc(sizeof(i2c_driver_t));
            i2c_driver_init(driver->i2c, 0x40, 400000);  // Mock address and frequency
            break;
        case 1:  // SPI
            driver->spi = (spi_driver_t*)malloc(sizeof(spi_driver_t));
            // SPI config would be set here
            break;
        case 2:  // UART
            driver->uart = (uart_driver_t*)malloc(sizeof(uart_driver_t));
            // UART config would be set here
            break;
        default:
            return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_READY;

    return ERROR_NONE;
}

error_t sensor_driver_read(sensor_driver_t *driver, float *value) {
    if (driver == NULL || value == NULL || driver->state != DEVICE_STATE_READY) {
        return ERROR_INVALID_PARAM;
    }

    driver->state = DEVICE_STATE_BUSY;

    // Mock sensor reading based on interface
    if (driver->i2c) {
        uint8_t data[2];
        i2c_driver_read(driver->i2c, 0x00, data, 2);  // Mock register read
        *value = ((data[0] << 8) | data[1]) * 0.01f;  // Convert to float
    } else {
        *value = 25.5f;  // Mock value
    }

    // Apply calibration
    *value = (*value * driver->calibration_scale) + driver->calibration_offset;

    driver->last_value = *value;
    driver->last_reading_time = 0;  // Mock timestamp

    driver->state = DEVICE_STATE_READY;

    if (driver->data_ready_cb) {
        driver->data_ready_cb(*value, driver->callback_context);
    }

    return ERROR_NONE;
}

error_t sensor_driver_calibrate(sensor_driver_t *driver, float reference_value) {
    if (driver == NULL) {
        return ERROR_INVALID_PARAM;
    }

    // Simple calibration: adjust offset so current reading matches reference
    driver->calibration_offset = reference_value - driver->last_value;

    return ERROR_NONE;
}