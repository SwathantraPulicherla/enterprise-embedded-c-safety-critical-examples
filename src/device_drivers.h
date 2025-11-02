#ifndef DEVICE_DRIVERS_H
#define DEVICE_DRIVERS_H

#include <stdint.h>
#include <stdbool.h>
#include "embedded_hardware.h"
#include "communication_protocols.h"

// Device States
typedef enum {
    DEVICE_STATE_UNINITIALIZED,
    DEVICE_STATE_INIT,
    DEVICE_STATE_READY,
    DEVICE_STATE_BUSY,
    DEVICE_STATE_ERROR,
    DEVICE_STATE_SLEEP,
    DEVICE_STATE_OFF
} device_state_t;

// Error History
#define ERROR_HISTORY_SIZE 10
typedef struct {
    uint32_t timestamp;
    error_t error_code;
    uint32_t context;
} error_history_t;

// UART Driver Structure
typedef struct {
    USART_TypeDef *uart;           // Hardware registers
    DMA_Channel_TypeDef *dma_tx;   // TX DMA channel
    DMA_Channel_TypeDef *dma_rx;   // RX DMA channel
    device_state_t state;          // Current state
    uint8_t device_addr;           // Device address
    uint32_t timeout_ms;           // Timeout in milliseconds
    void (*state_change_cb)(device_state_t old, device_state_t new, void* context);
    void *callback_context;        // Callback context
    error_history_t errors[ERROR_HISTORY_SIZE];
    uint8_t error_index;
    uint8_t tx_buffer[256];        // TX buffer
    uint8_t rx_buffer[256];        // RX buffer
    uint16_t tx_size;              // Current TX size
    uint16_t rx_size;              // Current RX size
} uart_driver_t;

// SPI Driver Structure
typedef struct {
    SPI_TypeDef *spi;              // Hardware registers
    DMA_Channel_TypeDef *dma_tx;   // TX DMA channel
    DMA_Channel_TypeDef *dma_rx;   // RX DMA channel
    device_state_t state;
    uint8_t cs_pin;                // Chip select pin
    GPIO_TypeDef *cs_gpio;         // CS GPIO port
    uint32_t frequency;            // SPI frequency
    void (*completion_cb)(error_t result, void* context);
    void *callback_context;
    error_history_t errors[ERROR_HISTORY_SIZE];
    uint8_t error_index;
} spi_driver_t;

// I2C Driver Structure
typedef struct {
    void *i2c_regs;                // I2C hardware registers (placeholder)
    device_state_t state;
    uint8_t device_addr;
    uint32_t timeout_ms;
    bool use_dma;
    DMA_Channel_TypeDef *dma_tx;
    DMA_Channel_TypeDef *dma_rx;
    void (*event_cb)(uint32_t event, void* context);
    void *callback_context;
    error_history_t errors[ERROR_HISTORY_SIZE];
    uint8_t error_index;
} i2c_driver_t;

// CAN Driver Structure
typedef struct {
    can_handle_t *can;             // CAN handle
    device_state_t state;
    uint32_t bitrate;              // CAN bitrate
    uint8_t filter_count;          // Number of filters
    void (*message_cb)(const can_frame_t* frame, void* context);
    void *callback_context;
    error_history_t errors[ERROR_HISTORY_SIZE];
    uint8_t error_index;
} can_driver_t;

// Sensor Driver Structure (complex with multiple interfaces)
typedef struct {
    i2c_driver_t *i2c;             // I2C interface
    spi_driver_t *spi;             // SPI interface (alternative)
    uart_driver_t *uart;           // UART interface (alternative)
    device_state_t state;
    uint8_t sensor_type;           // Type of sensor
    uint32_t sampling_rate;        // Sampling rate in Hz
    float calibration_offset;      // Calibration offset
    float calibration_scale;       // Calibration scale
    void (*data_ready_cb)(float data, void* context);
    void *callback_context;
    error_history_t errors[ERROR_HISTORY_SIZE];
    uint8_t error_index;
    uint32_t last_reading_time;    // Timestamp of last reading
    float last_value;              // Last sensor value
} sensor_driver_t;

// Function declarations
error_t uart_driver_init(uart_driver_t *driver, const uart_config_t *config);
error_t uart_driver_transmit(uart_driver_t *driver, const uint8_t *data, uint16_t size);
error_t uart_driver_receive(uart_driver_t *driver, uint8_t *data, uint16_t size);
void uart_driver_process_interrupt(uart_driver_t *driver);

error_t spi_driver_init(spi_driver_t *driver, const spi_config_t *config);
error_t spi_driver_transfer(spi_driver_t *driver, const uint8_t *tx_data, uint8_t *rx_data, uint16_t size);
void spi_driver_process_interrupt(spi_driver_t *driver);

error_t i2c_driver_init(i2c_driver_t *driver, uint8_t address, uint32_t frequency);
error_t i2c_driver_write(i2c_driver_t *driver, uint8_t reg, const uint8_t *data, uint16_t size);
error_t i2c_driver_read(i2c_driver_t *driver, uint8_t reg, uint8_t *data, uint16_t size);

error_t can_driver_init(can_driver_t *driver, uint32_t bitrate);
error_t can_driver_send_message(can_driver_t *driver, const can_frame_t *frame);
void can_driver_process_message(can_driver_t *driver, const can_frame_t *frame);

error_t sensor_driver_init(sensor_driver_t *driver, uint8_t interface_type, uint8_t sensor_type);
error_t sensor_driver_read(sensor_driver_t *driver, float *value);
error_t sensor_driver_calibrate(sensor_driver_t *driver, float reference_value);

#endif // DEVICE_DRIVERS_H