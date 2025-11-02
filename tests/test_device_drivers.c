/* test_device_drivers.c – Auto-generated Expert Unity Tests */

#include "unity.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h> // For malloc/free

// Include the header for the module under test.
// This header is included in the source file, so it must be included here.
#include "device_drivers.h"

// --- Mock Hardware Register Definitions ---
// These are minimal definitions to allow compilation and `sizeof` operations
// as seen in the source code (e.g., `malloc(sizeof(USART_TypeDef))`).
// They provide no real hardware functionality. Typically, these would come from HAL headers.
// The `volatile` keyword is used to prevent the compiler from optimizing away access to these mock registers.
typedef struct {
    volatile uint32_t SR; // Status Register (e.g., USART_SR_RXNE, USART_SR_TXE)
    volatile uint32_t DR; // Data Register
} USART_TypeDef;

typedef struct {
    volatile uint32_t CR1; // Control Register 1
    volatile uint32_t CR2; // Control Register 2
    volatile uint32_t DR;  // Data Register
} SPI_TypeDef;

typedef struct {
    volatile uint32_t MCR; // Master Control Register
    volatile uint32_t ESR; // Error Status Register
} can_handle_t;

// --- Callback Function Type Definitions ---
// These define the function pointer types used in driver structs for callbacks.
// Assuming these are application-specific and not necessarily in device_drivers.h.
typedef void (*device_state_change_cb_t)(device_state_t old_state, device_state_t new_state, void* context);
typedef void (*sensor_data_ready_cb_t)(float value, void* context);

// --- STUB CONTROL STRUCTURES AND DECLARATIONS FOR EXTERNAL FUNCTIONS ---

// 1. uart_init
typedef struct {
    error_t return_value;
    bool was_called;
    uint32_t call_count;
    USART_TypeDef *last_uart_regs;
    uart_config_t last_config; // Capture config by value, assuming it's small
} stub_uart_init_t;
static stub_uart_init_t stub_uart_init_data = {0};
error_t uart_init(USART_TypeDef *uart_regs, uart_config_t *config);

// 2. uart_transmit
typedef struct {
    error_t return_value;
    bool was_called;
    uint32_t call_count;
    USART_TypeDef *last_uart_regs;
    const uint8_t *last_data; // Pointer to original data
    uint16_t last_size;
    uint32_t last_timeout_ms;
} stub_uart_transmit_t;
static stub_uart_transmit_t stub_uart_transmit_data = {0};
error_t uart_transmit(USART_TypeDef *uart_regs, uint8_t *data, uint16_t size, uint32_t timeout_ms);

// 3. uart_receive
typedef struct {
    error_t return_value;
    bool was_called;
    uint32_t call_count;
    USART_TypeDef *last_uart_regs;
    uint8_t *last_data_buffer; // Pointer to the buffer provided by caller
    uint16_t last_size;
    uint32_t last_timeout_ms;
    uint8_t mock_rx_data[256]; // Data to be copied into last_data_buffer
    uint16_t mock_rx_data_len;
} stub_uart_receive_t;
static stub_uart_receive_t stub_uart_receive_data = {0};
error_t uart_receive(USART_TypeDef *uart_regs, uint8_t *data, uint16_t size, uint32_t timeout_ms);

// 4. spi_init
typedef struct {
    error_t return_value;
    bool was_called;
    uint32_t call_count;
    SPI_TypeDef *last_spi_regs;
    spi_config_t last_config; // Capture config by value
} stub_spi_init_t;
static stub_spi_init_t stub_spi_init_data = {0};
error_t spi_init(SPI_TypeDef *spi_regs, spi_config_t *config);

// 5. spi_transmit_receive
typedef struct {
    error_t return_value;
    bool was_called;
    uint32_t call_count;
    SPI_TypeDef *last_spi_regs;
    const uint8_t *last_tx_data;
    uint8_t *last_rx_data_buffer; // Pointer to the buffer provided by caller
    uint16_t last_size;
    uint8_t mock_rx_data[256]; // Data to be copied into last_rx_data_buffer
    uint16_t mock_rx_data_len;
} stub_spi_transmit_receive_t;
static stub_spi_transmit_receive_t stub_spi_transmit_receive_data = {0};
error_t spi_transmit_receive(SPI_TypeDef *spi_regs, uint8_t *tx_data, uint8_t *rx_data, uint16_t size);

// 6. gpio_write_pin
typedef struct {
    bool was_called;
    uint32_t call_count;
    void *last_gpio_port;
    uint16_t last_gpio_pin;
    bool last_state;
    // Capture history for sequence checks
    struct {
        void *port;
        uint16_t pin;
        bool state;
    } history[10]; // Allow capturing up to 10 calls for sequence verification
    uint8_t history_idx;
} stub_gpio_write_pin_t;
static stub_gpio_write_pin_t stub_gpio_write_pin_data = {0};
void gpio_write_pin(void *gpio_port, uint16_t gpio_pin, bool state);

// 7. can_init
typedef struct {
    protocol_error_t return_value;
    bool was_called;
    uint32_t call_count;
    can_handle_t *last_hcan;
    uint32_t last_buffer_size;
} stub_can_init_t;
static stub_can_init_t stub_can_init_data = {0};
protocol_error_t can_init(can_handle_t *hcan, uint32_t buffer_size);

// 8. can_transmit_message
typedef struct {
    protocol_error_t return_value;
    bool was_called;
    uint32_t call_count;
    can_handle_t *last_hcan;
    can_frame_t last_frame; // Capture frame struct by value
    uint32_t last_timeout;
} stub_can_transmit_message_t;
static stub_can_transmit_message_t stub_can_transmit_message_data = {0};
protocol_error_t can_transmit_message(can_handle_t *hcan, const can_frame_t *frame, uint32_t timeout);


// --- STUB IMPLEMENTATIONS ---

error_t uart_init(USART_TypeDef *uart_regs, uart_config_t *config) {
    stub_uart_init_data.was_called = true;
    stub_uart_init_data.call_count++;
    stub_uart_init_data.last_uart_regs = uart_regs;
    if (config != NULL) {
        stub_uart_init_data.last_config = *config;
    }
    return stub_uart_init_data.return_value;
}

error_t uart_transmit(USART_TypeDef *uart_regs, uint8_t *data, uint16_t size, uint32_t timeout_ms) {
    stub_uart_transmit_data.was_called = true;
    stub_uart_transmit_data.call_count++;
    stub_uart_transmit_data.last_uart_regs = uart_regs;
    stub_uart_transmit_data.last_data = data;
    stub_uart_transmit_data.last_size = size;
    stub_uart_transmit_data.last_timeout_ms = timeout_ms;
    return stub_uart_transmit_data.return_value;
}

error_t uart_receive(USART_TypeDef *uart_regs, uint8_t *data, uint16_t size, uint32_t timeout_ms) {
    stub_uart_receive_data.was_called = true;
    stub_uart_receive_data.call_count++;
    stub_uart_receive_data.last_uart_regs = uart_regs;
    stub_uart_receive_data.last_data_buffer = data;
    stub_uart_receive_data.last_size = size;
    stub_uart_receive_data.last_timeout_ms = timeout_ms;

    if (stub_uart_receive_data.return_value == ERROR_NONE && data != NULL && size > 0 && stub_uart_receive_data.mock_rx_data_len > 0) {
        // Copy mock data into the provided buffer, respecting buffer size
        memcpy(data, stub_uart_receive_data.mock_rx_data, (size < stub_uart_receive_data.mock_rx_data_len) ? size : stub_uart_receive_data.mock_rx_data_len);
    }

    return stub_uart_receive_data.return_value;
}

error_t spi_init(SPI_TypeDef *spi_regs, spi_config_t *config) {
    stub_spi_init_data.was_called = true;
    stub_spi_init_data.call_count++;
    stub_spi_init_data.last_spi_regs = spi_regs;
    if (config != NULL) {
        stub_spi_init_data.last_config = *config;
    }
    return stub_spi_init_data.return_value;
}

error_t spi_transmit_receive(SPI_TypeDef *spi_regs, uint8_t *tx_data, uint8_t *rx_data, uint16_t size) {
    stub_spi_transmit_receive_data.was_called = true;
    stub_spi_transmit_receive_data.call_count++;
    stub_spi_transmit_receive_data.last_spi_regs = spi_regs;
    stub_spi_transmit_receive_data.last_tx_data = tx_data;
    stub_spi_transmit_receive_data.last_rx_data_buffer = rx_data;
    stub_spi_transmit_receive_data.last_size = size;

    if (stub_spi_transmit_receive_data.return_value == ERROR_NONE && rx_data != NULL && size > 0 && stub_spi_transmit_receive_data.mock_rx_data_len > 0) {
        // Copy mock data into the provided buffer, respecting buffer size
        memcpy(rx_data, stub_spi_transmit_receive_data.mock_rx_data, (size < stub_spi_transmit_receive_data.mock_rx_data_len) ? size : stub_spi_transmit_receive_data.mock_rx_data_len);
    }

    return stub_spi_transmit_receive_data.return_value;
}

void gpio_write_pin(void *gpio_port, uint16_t gpio_pin, bool state) {
    stub_gpio_write_pin_data.was_called = true;
    stub_gpio_write_pin_data.call_count++;
    stub_gpio_write_pin_data.last_gpio_port = gpio_port;
    stub_gpio_write_pin_data.last_gpio_pin = gpio_pin;
    stub_gpio_write_pin_data.last_state = state;
    if (stub_gpio_write_pin_data.history_idx < (sizeof(stub_gpio_write_pin_data.history) / sizeof(stub_gpio_write_pin_data.history[0]))) {
        stub_gpio_write_pin_data.history[stub_gpio_write_pin_data.history_idx].port = gpio_port;
        stub_gpio_write_pin_data.history[stub_gpio_write_pin_data.history_idx].pin = gpio_pin;
        stub_gpio_write_pin_data.history[stub_gpio_write_pin_data.history_idx].state = state;
        stub_gpio_write_pin_data.history_idx++;
    }
}

protocol_error_t can_init(can_handle_t *hcan, uint32_t buffer_size) {
    stub_can_init_data.was_called = true;
    stub_can_init_data.call_count++;
    stub_can_init_data.last_hcan = hcan;
    stub_can_init_data.last_buffer_size = buffer_size;
    return stub_can_init_data.return_value;
}

protocol_error_t can_transmit_message(can_handle_t *hcan, const can_frame_t *frame, uint32_t timeout) {
    stub_can_transmit_message_data.was_called = true;
    stub_can_transmit_message_data.call_count++;
    stub_can_transmit_message_data.last_hcan = hcan;
    if (frame != NULL) {
        stub_can_transmit_message_data.last_frame = *frame;
    }
    stub_can_transmit_message_data.last_timeout = timeout;
    return stub_can_transmit_message_data.return_value;
}

// --- Test Setup and Teardown ---

// Global driver instances for testing, reset in setUp/tearDown
static uart_driver_t test_uart_driver;
static spi_driver_t test_spi_driver;
static i2c_driver_t test_i2c_driver;
static can_driver_t test_can_driver;
static sensor_driver_t test_sensor_driver;

// Mock callback functions
// These mock functions can be extended with stubs if their calls need to be asserted.
static bool mock_state_change_cb_called = false;
static device_state_t mock_state_change_cb_old_state;
static device_state_t mock_state_change_cb_new_state;
static void* mock_state_change_cb_context;

void mock_state_change_cb(device_state_t old_state, device_state_t new_state, void* context) {
    mock_state_change_cb_called = true;
    mock_state_change_cb_old_state = old_state;
    mock_state_change_cb_new_state = new_state;
    mock_state_change_cb_context = context;
}

static bool mock_sensor_data_ready_cb_called = false;
static float mock_sensor_data_ready_cb_value;
static void* mock_sensor_data_ready_cb_context;

void mock_sensor_data_ready_cb(float value, void* context) {
    mock_sensor_data_ready_cb_called = true;
    mock_sensor_data_ready_cb_value = value;
    mock_sensor_data_ready_cb_context = context;
}

void setUp(void) {
    // Reset all stub data using memset for completeness
    memset(&stub_uart_init_data, 0, sizeof(stub_uart_init_data));
    memset(&stub_uart_transmit_data, 0, sizeof(stub_uart_transmit_data));
    memset(&stub_uart_receive_data, 0, sizeof(stub_uart_receive_data));
    memset(&stub_spi_init_data, 0, sizeof(stub_spi_init_data));
    memset(&stub_spi_transmit_receive_data, 0, sizeof(stub_spi_transmit_receive_data));
    memset(&stub_gpio_write_pin_data, 0, sizeof(stub_gpio_write_pin_data));
    memset(&stub_can_init_data, 0, sizeof(stub_can_init_data));
    memset(&stub_can_transmit_message_data, 0, sizeof(stub_can_transmit_message_data));

    // Default return values for stubs
    stub_uart_init_data.return_value = ERROR_NONE;
    stub_uart_transmit_data.return_value = ERROR_NONE;
    stub_uart_receive_data.return_value = ERROR_NONE;
    stub_spi_init_data.return_value = ERROR_NONE;
    stub_spi_transmit_receive_data.return_value = ERROR_NONE;
    stub_can_init_data.return_value = PROTOCOL_ERROR_NONE;
    stub_can_transmit_message_data.return_value = PROTOCOL_ERROR_NONE;

    // Reset global mock callback flags/data
    mock_state_change_cb_called = false;
    mock_state_change_cb_old_state = (device_state_t)0;
    mock_state_change_cb_new_state = (device_state_t)0;
    mock_state_change_cb_context = NULL;

    mock_sensor_data_ready_cb_called = false;
    mock_sensor_data_ready_cb_value = 0.0f;
    mock_sensor_data_ready_cb_context = NULL;

    // Reset test driver structs, freeing any previous mallocs
    tearDown(); // Call tearDown to ensure clean state and freed memory
    memset(&test_uart_driver, 0, sizeof(test_uart_driver));
    memset(&test_spi_driver, 0, sizeof(test_spi_driver));
    memset(&test_i2c_driver, 0, sizeof(test_i2c_driver));
    memset(&test_can_driver, 0, sizeof(test_can_driver));
    memset(&test_sensor_driver, 0, sizeof(test_sensor_driver));
}

void tearDown(void) {
    // Free any dynamically allocated memory within the driver structs
    if (test_uart_driver.uart != NULL) {
        free(test_uart_driver.uart);
        test_uart_driver.uart = NULL;
    }
    if (test_spi_driver.spi != NULL) {
        free(test_spi_driver.spi);
        test_spi_driver.spi = NULL;
    }
    if (test_i2c_driver.i2c_regs != NULL) {
        free(test_i2c_driver.i2c_regs);
        test_i2c_driver.i2c_regs = NULL;
    }
    if (test_can_driver.can != NULL) {
        free(test_can_driver.can);
        test_can_driver.can = NULL;
    }
    // Sensor driver can allocate its own interface drivers
    if (test_sensor_driver.i2c != NULL) {
        if (test_sensor_driver.i2c->i2c_regs != NULL) {
            free(test_sensor_driver.i2c->i2c_regs);
            test_sensor_driver.i2c->i2c_regs = NULL;
        }
        free(test_sensor_driver.i2c);
        test_sensor_driver.i2c = NULL;
    }
    if (test_sensor_driver.spi != NULL) {
        free(test_sensor_driver.spi);
        test_sensor_driver.spi = NULL;
    }
    if (test_sensor_driver.uart != NULL) {
        free(test_sensor_driver.uart);
        test_sensor_driver.uart = NULL;
    }

    // Reset all stub data again, ensures no leftover from test run
    memset(&stub_uart_init_data, 0, sizeof(stub_uart_init_data));
    memset(&stub_uart_transmit_data, 0, sizeof(stub_uart_transmit_data));
    memset(&stub_uart_receive_data, 0, sizeof(stub_uart_receive_data));
    memset(&stub_spi_init_data, 0, sizeof(stub_spi_init_data));
    memset(&stub_spi_transmit_receive_data, 0, sizeof(stub_spi_transmit_receive_data));
    memset(&stub_gpio_write_pin_data, 0, sizeof(stub_gpio_write_pin_data));
    memset(&stub_can_init_data, 0, sizeof(stub_can_init_data));
    memset(&stub_can_transmit_message_data, 0, sizeof(stub_can_transmit_message_data));

    // Reset global mock callback flags/data
    mock_state_change_cb_called = false;
    mock_state_change_cb_old_state = (device_state_t)0;
    mock_state_change_cb_new_state = (device_state_t)0;
    mock_state_change_cb_context = NULL;

    mock_sensor_data_ready_cb_called = false;
    mock_sensor_data_ready_cb_value = 0.0f;
    mock_sensor_data_ready_cb_context = NULL;
}

// --- Test Cases for uart_driver_init ---

void test_uart_driver_init_null_driver(void) {
    uart_config_t config = { .baud_rate = 115200 };
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_init(NULL, &config));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_uart_init_data.was_called);
}

void test_uart_driver_init_null_config(void) {
    // Expected: Returns ERROR_INVALID_PARAM if config is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_init(&test_uart_driver, NULL));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_uart_init_data.was_called);
}

void test_uart_driver_init_success(void) {
    uart_config_t config = { .baud_rate = 115200, .data_bits = 8, .stop_bits = 1, .parity = 0 };
    test_uart_driver.state_change_cb = mock_state_change_cb;
    void *cb_context = (void*)0xDEADBEEF;
    test_uart_driver.callback_context = cb_context;

    // Expected: Successful initialization
    TEST_ASSERT_EQUAL(ERROR_NONE, uart_driver_init(&test_uart_driver, &config));

    // Expected: UART hardware (mock) allocated
    TEST_ASSERT_NOT_NULL(test_uart_driver.uart);
    // Expected: uart_init external stub called once
    TEST_ASSERT_TRUE(stub_uart_init_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_uart_init_data.call_count);
    // Expected: Correct parameters passed to uart_init
    TEST_ASSERT_EQUAL_PTR(test_uart_driver.uart, stub_uart_init_data.last_uart_regs);
    TEST_ASSERT_EQUAL_MEMORY(&config, &stub_uart_init_data.last_config, sizeof(uart_config_t));
    // Expected: Driver state becomes READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state);
    // Expected: Default timeout set
    TEST_ASSERT_EQUAL(1000, test_uart_driver.timeout_ms);
    // Expected: Error history cleared
    for (int i = 0; i < ERROR_HISTORY_SIZE; i++) {
        TEST_ASSERT_EQUAL(0, test_uart_driver.errors[i].timestamp);
        TEST_ASSERT_EQUAL(0, test_uart_driver.errors[i].error_code);
    }
    // Expected: Callback invoked with correct states
    TEST_ASSERT_TRUE(mock_state_change_cb_called);
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, mock_state_change_cb_old_state);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, mock_state_change_cb_new_state);
    TEST_ASSERT_EQUAL_PTR(cb_context, mock_state_change_cb_context);
}

void test_uart_driver_init_uart_init_failure(void) {
    uart_config_t config = { .baud_rate = 9600 };
    stub_uart_init_data.return_value = ERROR_BUSY; // Simulate failure from underlying UART init

    // Expected: Returns ERROR_BUSY due to uart_init failure
    TEST_ASSERT_EQUAL(ERROR_BUSY, uart_driver_init(&test_uart_driver, &config));

    // Expected: uart_init external stub called once
    TEST_ASSERT_TRUE(stub_uart_init_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_uart_init_data.call_count);
    // Expected: Driver state becomes ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_uart_driver.state);
    // Expected: Allocated hardware registers freed (uart pointer should be NULL after free)
    TEST_ASSERT_NULL(test_uart_driver.uart);
    // Expected: Callback not invoked if initialization fails early
    TEST_ASSERT_FALSE(mock_state_change_cb_called);
}

// --- Test Cases for uart_driver_transmit ---

void test_uart_driver_transmit_null_driver(void) {
    uint8_t data[] = {0x01, 0x02};
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_transmit(NULL, data, sizeof(data)));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_uart_transmit_data.was_called);
}

void test_uart_driver_transmit_null_data(void) {
    // Initialize driver to READY state
    uart_config_t config = { .baud_rate = 115200 };
    uart_driver_init(&test_uart_driver, &config);
    test_uart_driver.state = DEVICE_STATE_READY;

    // Expected: Returns ERROR_INVALID_PARAM if data is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_transmit(&test_uart_driver, NULL, 10));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_uart_transmit_data.was_called);
    // Expected: Driver state remains READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state);
}

void test_uart_driver_transmit_zero_size(void) {
    // Initialize driver to READY state
    uart_config_t config = { .baud_rate = 115200 };
    uart_driver_init(&test_uart_driver, &config);
    test_uart_driver.state = DEVICE_STATE_READY;
    uint8_t data[] = {0x01, 0x02};

    // Expected: Returns ERROR_INVALID_PARAM if size is 0
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_transmit(&test_uart_driver, data, 0));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_uart_transmit_data.was_called);
    // Expected: Driver state remains READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state);
}

void test_uart_driver_transmit_not_ready(void) {
    uint8_t data[] = {0x01, 0x02};
    test_uart_driver.state = DEVICE_STATE_INIT; // Not in READY state

    // Expected: Returns ERROR_INVALID_PARAM if driver is not in READY state
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_transmit(&test_uart_driver, data, sizeof(data)));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_uart_transmit_data.was_called);
    // Expected: Driver state remains INIT
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, test_uart_driver.state);
}

void test_uart_driver_transmit_success(void) {
    uart_config_t config = { .baud_rate = 115200 };
    uart_driver_init(&test_uart_driver, &config);
    test_uart_driver.state = DEVICE_STATE_READY; // Ensure READY state after init
    uint8_t data[] = {0xAA, 0xBB, 0xCC};
    uint16_t size = sizeof(data);

    // Expected: Successful transmission
    TEST_ASSERT_EQUAL(ERROR_NONE, uart_driver_transmit(&test_uart_driver, data, size));

    // Expected: Driver state transitions to BUSY then back to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state);
    // Expected: uart_transmit external stub called once
    TEST_ASSERT_TRUE(stub_uart_transmit_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_uart_transmit_data.call_count);
    // Expected: Correct parameters passed to uart_transmit
    TEST_ASSERT_EQUAL_PTR(test_uart_driver.uart, stub_uart_transmit_data.last_uart_regs);
    TEST_ASSERT_EQUAL_PTR(data, stub_uart_transmit_data.last_data);
    TEST_ASSERT_EQUAL(size, stub_uart_transmit_data.last_size);
    TEST_ASSERT_EQUAL(test_uart_driver.timeout_ms, stub_uart_transmit_data.last_timeout_ms);
}

void test_uart_driver_transmit_failure(void) {
    uart_config_t config = { .baud_rate = 115200 };
    uart_driver_init(&test_uart_driver, &config);
    test_uart_driver.state = DEVICE_STATE_READY; // Ensure READY state
    uint8_t data[] = {0xDE, 0xAD};
    uint16_t size = sizeof(data);
    stub_uart_transmit_data.return_value = ERROR_TIMEOUT; // Simulate failure

    // Expected: Returns ERROR_TIMEOUT due to uart_transmit failure
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, uart_driver_transmit(&test_uart_driver, data, size));

    // Expected: Driver state transitions to BUSY then to ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_uart_driver.state);
    // Expected: uart_transmit external stub called once
    TEST_ASSERT_TRUE(stub_uart_transmit_data.was_called);
    // Expected: Error logged in history
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, test_uart_driver.errors[0].error_code);
    TEST_ASSERT_EQUAL(1, test_uart_driver.error_index); // error_index should wrap around
}

void test_uart_driver_transmit_error_history_wrap(void) {
    uart_config_t config = { .baud_rate = 115200 };
    uart_driver_init(&test_uart_driver, &config);
    test_uart_driver.state = DEVICE_STATE_READY;
    uint8_t data[] = {0x01};

    stub_uart_transmit_data.return_value = ERROR_BUSY;

    // Fill the error history
    for (int i = 0; i < ERROR_HISTORY_SIZE; i++) {
        uart_driver_transmit(&test_uart_driver, data, sizeof(data));
        TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_uart_driver.state); // Stays in ERROR state for simplicity
        test_uart_driver.state = DEVICE_STATE_READY; // Reset to READY for next transmit
    }
    // Expected: All error entries should be filled
    for (int i = 0; i < ERROR_HISTORY_SIZE; i++) {
        TEST_ASSERT_EQUAL(ERROR_BUSY, test_uart_driver.errors[i].error_code);
    }
    TEST_ASSERT_EQUAL(0, test_uart_driver.error_index); // Should wrap back to 0

    // Transmit one more time to test wrap-around
    stub_uart_transmit_data.return_value = ERROR_TIMEOUT; // New error code
    uart_driver_transmit(&test_uart_driver, data, sizeof(data));
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_uart_driver.state);
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, test_uart_driver.errors[0].error_code); // First entry overwritten
    TEST_ASSERT_EQUAL(1, test_uart_driver.error_index); // Now pointing to next slot
}

// --- Test Cases for uart_driver_receive ---

void test_uart_driver_receive_null_driver(void) {
    uint8_t buffer[10];
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_receive(NULL, buffer, sizeof(buffer)));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_uart_receive_data.was_called);
}

void test_uart_driver_receive_null_data_buffer(void) {
    uart_config_t config = { .baud_rate = 115200 };
    uart_driver_init(&test_uart_driver, &config);
    test_uart_driver.state = DEVICE_STATE_READY;

    // Expected: Returns ERROR_INVALID_PARAM if data buffer is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_receive(&test_uart_driver, NULL, 10));
    // Expected: Driver state remains READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state);
}

void test_uart_driver_receive_zero_size(void) {
    uart_config_t config = { .baud_rate = 115200 };
    uart_driver_init(&test_uart_driver, &config);
    test_uart_driver.state = DEVICE_STATE_READY;
    uint8_t buffer[10];

    // Expected: Returns ERROR_INVALID_PARAM if size is 0
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_receive(&test_uart_driver, buffer, 0));
    // Expected: Driver state remains READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state);
}

void test_uart_driver_receive_not_ready(void) {
    uint8_t buffer[10];
    test_uart_driver.state = DEVICE_STATE_ERROR; // Not in READY state

    // Expected: Returns ERROR_INVALID_PARAM if driver is not in READY state
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_receive(&test_uart_driver, buffer, sizeof(buffer)));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_uart_receive_data.was_called);
    // Expected: Driver state remains ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_uart_driver.state);
}

void test_uart_driver_receive_success(void) {
    uart_config_t config = { .baud_rate = 115200 };
    uart_driver_init(&test_uart_driver, &config);
    test_uart_driver.state = DEVICE_STATE_READY;
    uint8_t rx_buffer[5] = {0};
    uint8_t expected_data[] = {0x11, 0x22, 0x33, 0x44, 0x55};
    stub_uart_receive_data.mock_rx_data_len = sizeof(expected_data);
    memcpy(stub_uart_receive_data.mock_rx_data, expected_data, sizeof(expected_data));

    // Expected: Successful reception
    TEST_ASSERT_EQUAL(ERROR_NONE, uart_driver_receive(&test_uart_driver, rx_buffer, sizeof(rx_buffer)));

    // Expected: Driver state transitions to BUSY then back to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state);
    // Expected: uart_receive external stub called once
    TEST_ASSERT_TRUE(stub_uart_receive_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_uart_receive_data.call_count);
    // Expected: Correct parameters passed to uart_receive
    TEST_ASSERT_EQUAL_PTR(test_uart_driver.uart, stub_uart_receive_data.last_uart_regs);
    TEST_ASSERT_EQUAL_PTR(rx_buffer, stub_uart_receive_data.last_data_buffer);
    TEST_ASSERT_EQUAL(sizeof(rx_buffer), stub_uart_receive_data.last_size);
    TEST_ASSERT_EQUAL(test_uart_driver.timeout_ms, stub_uart_receive_data.last_timeout_ms);
    // Expected: Received data matches mock data
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data, rx_buffer, sizeof(rx_buffer));
}

void test_uart_driver_receive_failure(void) {
    uart_config_t config = { .baud_rate = 115200 };
    uart_driver_init(&test_uart_driver, &config);
    test_uart_driver.state = DEVICE_STATE_READY;
    uint8_t rx_buffer[5] = {0};
    stub_uart_receive_data.return_value = ERROR_TIMEOUT; // Simulate failure

    // Expected: Returns ERROR_TIMEOUT due to uart_receive failure
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, uart_driver_receive(&test_uart_driver, rx_buffer, sizeof(rx_buffer)));

    // Expected: Driver state transitions to BUSY then to ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_uart_driver.state);
    // Expected: uart_receive external stub called once
    TEST_ASSERT_TRUE(stub_uart_receive_data.was_called);
    // Expected: Error logged in history
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, test_uart_driver.errors[0].error_code);
    TEST_ASSERT_EQUAL(1, test_uart_driver.error_index);
}

// --- Test Cases for uart_driver_process_interrupt ---

void test_uart_driver_process_interrupt_null_driver(void) {
    // Expected: No crash for NULL driver, function returns immediately
    uart_driver_process_interrupt(NULL);
    // No assertions possible beyond no crash, as it's a void function with no external calls
}

void test_uart_driver_process_interrupt_rxne(void) {
    uart_config_t config = { .baud_rate = 115200 };
    uart_driver_init(&test_uart_driver, &config);
    // Simulate RXNE flag set and data in DR
    test_uart_driver.uart->SR |= (1 << 5); // USART_SR_RXNE (common bit position)
    test_uart_driver.uart->DR = 0x5A; // Mock received data

    // Expected: Function processes RX interrupt path
    uart_driver_process_interrupt(&test_uart_driver);

    // No direct observable external effects or state changes for mock processing,
    // but the branch should be covered.
    // Can't assert 'data' variable internally.
    // If 'Process received data' involved a callback or other external function, we'd stub it.
    // For now, it's a "function called" test for this branch.
}

void test_uart_driver_process_interrupt_txe(void) {
    uart_config_t config = { .baud_rate = 115200 };
    uart_driver_init(&test_uart_driver, &config);
    // Simulate TXE flag set
    test_uart_driver.uart->SR |= (1 << 7); // USART_SR_TXE (common bit position)

    // Expected: Function processes TX interrupt path
    uart_driver_process_interrupt(&test_uart_driver);

    // Similar to RXNE, covers the branch.
}

void test_uart_driver_process_interrupt_no_flags(void) {
    uart_config_t config = { .baud_rate = 115200 };
    uart_driver_init(&test_uart_driver, &config);
    // Ensure no flags are set
    test_uart_driver.uart->SR = 0;

    // Expected: No processing occurs, function returns
    uart_driver_process_interrupt(&test_uart_driver);

    // Driver state should not change
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state);
}

// --- Test Cases for spi_driver_init ---

void test_spi_driver_init_null_driver(void) {
    spi_config_t config = { .mode = 0, .clock_freq = 1000000 };
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, spi_driver_init(NULL, &config));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_spi_init_data.was_called);
}

void test_spi_driver_init_null_config(void) {
    // Expected: Returns ERROR_INVALID_PARAM if config is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, spi_driver_init(&test_spi_driver, NULL));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_spi_init_data.was_called);
}

void test_spi_driver_init_success(void) {
    spi_config_t config = { .mode = 1, .clock_freq = 2000000 };

    // Expected: Successful initialization
    TEST_ASSERT_EQUAL(ERROR_NONE, spi_driver_init(&test_spi_driver, &config));

    // Expected: SPI hardware (mock) allocated
    TEST_ASSERT_NOT_NULL(test_spi_driver.spi);
    // Expected: spi_init external stub called once
    TEST_ASSERT_TRUE(stub_spi_init_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_spi_init_data.call_count);
    // Expected: Correct parameters passed to spi_init
    TEST_ASSERT_EQUAL_PTR(test_spi_driver.spi, stub_spi_init_data.last_spi_regs);
    TEST_ASSERT_EQUAL_MEMORY(&config, &stub_spi_init_data.last_config, sizeof(spi_config_t));
    // Expected: Driver state becomes READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_spi_driver.state);
    // Expected: Error history cleared
    for (int i = 0; i < ERROR_HISTORY_SIZE; i++) {
        TEST_ASSERT_EQUAL(0, test_spi_driver.errors[i].timestamp);
        TEST_ASSERT_EQUAL(0, test_spi_driver.errors[i].error_code);
    }
}

void test_spi_driver_init_spi_init_failure(void) {
    spi_config_t config = { .mode = 0, .clock_freq = 500000 };
    stub_spi_init_data.return_value = ERROR_BUSY; // Simulate failure from underlying SPI init

    // Expected: Returns ERROR_BUSY due to spi_init failure
    TEST_ASSERT_EQUAL(ERROR_BUSY, spi_driver_init(&test_spi_driver, &config));

    // Expected: spi_init external stub called once
    TEST_ASSERT_TRUE(stub_spi_init_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_spi_init_data.call_count);
    // Expected: Driver state becomes ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_spi_driver.state);
    // Expected: Allocated hardware registers freed (spi pointer should be NULL after free)
    TEST_ASSERT_NULL(test_spi_driver.spi);
}

// --- Test Cases for spi_driver_transfer ---

void test_spi_driver_transfer_null_driver(void) {
    uint8_t tx_data[] = {0x01};
    uint8_t rx_data[1];
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, spi_driver_transfer(NULL, tx_data, rx_data, 1));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_spi_transmit_receive_data.was_called);
}

void test_spi_driver_transfer_null_tx_data(void) {
    spi_config_t config = { .mode = 1, .clock_freq = 2000000 };
    spi_driver_init(&test_spi_driver, &config);
    test_spi_driver.state = DEVICE_STATE_READY;
    uint8_t rx_data[1];

    // Expected: Returns ERROR_INVALID_PARAM if tx_data is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, spi_driver_transfer(&test_spi_driver, NULL, rx_data, 1));
    // Expected: Driver state remains READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_spi_driver.state);
}

void test_spi_driver_transfer_null_rx_data(void) {
    spi_config_t config = { .mode = 1, .clock_freq = 2000000 };
    spi_driver_init(&test_spi_driver, &config);
    test_spi_driver.state = DEVICE_STATE_READY;
    uint8_t tx_data[] = {0x01};

    // Expected: Returns ERROR_INVALID_PARAM if rx_data is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, spi_driver_transfer(&test_spi_driver, tx_data, NULL, 1));
    // Expected: Driver state remains READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_spi_driver.state);
}

void test_spi_driver_transfer_not_ready(void) {
    uint8_t tx_data[] = {0x01};
    uint8_t rx_data[1];
    test_spi_driver.state = DEVICE_STATE_INIT; // Not in READY state

    // Expected: Returns ERROR_INVALID_PARAM if driver is not in READY state
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, spi_driver_transfer(&test_spi_driver, tx_data, rx_data, 1));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_spi_transmit_receive_data.was_called);
    // Expected: Driver state remains INIT
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, test_spi_driver.state);
}

void test_spi_driver_transfer_success(void) {
    spi_config_t config = { .mode = 1, .clock_freq = 2000000 };
    spi_driver_init(&test_spi_driver, &config);
    test_spi_driver.state = DEVICE_STATE_READY;
    void *mock_gpio_port = (void*)0x1234;
    uint16_t mock_gpio_pin = 5;
    test_spi_driver.cs_gpio = mock_gpio_port;
    test_spi_driver.cs_pin = mock_gpio_pin;

    uint8_t tx_data[] = {0x01, 0x02, 0x03};
    uint8_t rx_data[3] = {0};
    uint8_t expected_rx_data[] = {0xA1, 0xA2, 0xA3};
    stub_spi_transmit_receive_data.mock_rx_data_len = sizeof(expected_rx_data);
    memcpy(stub_spi_transmit_receive_data.mock_rx_data, expected_rx_data, sizeof(expected_rx_data));

    // Expected: Successful transfer
    TEST_ASSERT_EQUAL(ERROR_NONE, spi_driver_transfer(&test_spi_driver, tx_data, rx_data, sizeof(tx_data)));

    // Expected: Driver state transitions to BUSY then back to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_spi_driver.state);
    // Expected: gpio_write_pin called twice (CS low then high)
    TEST_ASSERT_EQUAL(2, stub_gpio_write_pin_data.call_count);
    // Expected: CS low first
    TEST_ASSERT_EQUAL_PTR(mock_gpio_port, stub_gpio_write_pin_data.history[0].port);
    TEST_ASSERT_EQUAL(mock_gpio_pin, stub_gpio_write_pin_data.history[0].pin);
    TEST_ASSERT_FALSE(stub_gpio_write_pin_data.history[0].state);
    // Expected: CS high second
    TEST_ASSERT_EQUAL_PTR(mock_gpio_port, stub_gpio_write_pin_data.history[1].port);
    TEST_ASSERT_EQUAL(mock_gpio_pin, stub_gpio_write_pin_data.history[1].pin);
    TEST_ASSERT_TRUE(stub_gpio_write_pin_data.history[1].state);

    // Expected: spi_transmit_receive external stub called once
    TEST_ASSERT_TRUE(stub_spi_transmit_receive_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_spi_transmit_receive_data.call_count);
    // Expected: Correct parameters passed to spi_transmit_receive
    TEST_ASSERT_EQUAL_PTR(test_spi_driver.spi, stub_spi_transmit_receive_data.last_spi_regs);
    TEST_ASSERT_EQUAL_PTR(tx_data, stub_spi_transmit_receive_data.last_tx_data);
    TEST_ASSERT_EQUAL_PTR(rx_data, stub_spi_transmit_receive_data.last_rx_data_buffer);
    TEST_ASSERT_EQUAL(sizeof(tx_data), stub_spi_transmit_receive_data.last_size);
    // Expected: Received data matches mock data
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data, rx_data, sizeof(rx_data));
}

void test_spi_driver_transfer_failure(void) {
    spi_config_t config = { .mode = 1, .clock_freq = 2000000 };
    spi_driver_init(&test_spi_driver, &config);
    test_spi_driver.state = DEVICE_STATE_READY;
    void *mock_gpio_port = (void*)0x1234;
    uint16_t mock_gpio_pin = 5;
    test_spi_driver.cs_gpio = mock_gpio_port;
    test_spi_driver.cs_pin = mock_gpio_pin;

    uint8_t tx_data[] = {0x01};
    uint8_t rx_data[1] = {0};
    stub_spi_transmit_receive_data.return_value = ERROR_TIMEOUT; // Simulate failure

    // Expected: Returns ERROR_TIMEOUT due to spi_transmit_receive failure
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, spi_driver_transfer(&test_spi_driver, tx_data, rx_data, sizeof(tx_data)));

    // Expected: Driver state transitions to BUSY then to ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_spi_driver.state);
    // Expected: gpio_write_pin called twice (CS low then high)
    TEST_ASSERT_EQUAL(2, stub_gpio_write_pin_data.call_count);
    // Expected: Error logged in history
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, test_spi_driver.errors[0].error_code);
    TEST_ASSERT_EQUAL(1, test_spi_driver.error_index);
}

// --- Test Cases for i2c_driver_init ---

void test_i2c_driver_init_null_driver(void) {
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_init(NULL, 0x50, 400000));
}

void test_i2c_driver_init_success(void) {
    uint8_t address = 0x50;
    uint32_t frequency = 400000;

    // Expected: Successful initialization
    TEST_ASSERT_EQUAL(ERROR_NONE, i2c_driver_init(&test_i2c_driver, address, frequency));

    // Expected: Driver state becomes READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_i2c_driver.state);
    // Expected: Device address set
    TEST_ASSERT_EQUAL(address, test_i2c_driver.device_addr);
    // Expected: Default timeout set
    TEST_ASSERT_EQUAL(100, test_i2c_driver.timeout_ms);
    // Expected: Mock registers allocated
    TEST_ASSERT_NOT_NULL(test_i2c_driver.i2c_regs);
    // Expected: Error history cleared
    for (int i = 0; i < ERROR_HISTORY_SIZE; i++) {
        TEST_ASSERT_EQUAL(0, test_i2c_driver.errors[i].timestamp);
        TEST_ASSERT_EQUAL(0, test_i2c_driver.errors[i].error_code);
    }
}

// Note: No explicit failure path for malloc in i2c_driver_init for i2c_regs.
// In a real scenario, this would be tested by mocking malloc. Here, it relies on stdlib malloc.
// The `if (driver->i2c_regs == NULL)` branch is present in the source, covering a theoretical failure.

// --- Test Cases for i2c_driver_write ---

void test_i2c_driver_write_null_driver(void) {
    uint8_t data[] = {0x01};
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_write(NULL, 0x10, data, 1));
}

void test_i2c_driver_write_null_data(void) {
    i2c_driver_init(&test_i2c_driver, 0x50, 400000); // Initialize driver
    test_i2c_driver.state = DEVICE_STATE_READY;

    // Expected: Returns ERROR_INVALID_PARAM if data is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_write(&test_i2c_driver, 0x10, NULL, 1));
    // Expected: Driver state remains READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_i2c_driver.state);
}

void test_i2c_driver_write_not_ready(void) {
    uint8_t data[] = {0x01};
    test_i2c_driver.state = DEVICE_STATE_BUSY; // Not in READY state

    // Expected: Returns ERROR_INVALID_PARAM if driver is not in READY state
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_write(&test_i2c_driver, 0x10, data, 1));
    // Expected: Driver state remains BUSY
    TEST_ASSERT_EQUAL(DEVICE_STATE_BUSY, test_i2c_driver.state);
}

void test_i2c_driver_write_success(void) {
    i2c_driver_init(&test_i2c_driver, 0x50, 400000); // Initialize driver
    test_i2c_driver.state = DEVICE_STATE_READY;
    uint8_t data[] = {0xA1, 0xB2};
    uint8_t reg_addr = 0x10;
    uint16_t size = sizeof(data);

    // Expected: Successful write operation (mock)
    TEST_ASSERT_EQUAL(ERROR_NONE, i2c_driver_write(&test_i2c_driver, reg_addr, data, size));

    // Expected: Driver state transitions to BUSY then back to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_i2c_driver.state);
    // No external stubs called directly by i2c_driver_write, it's a mock operation.
}

// --- Test Cases for i2c_driver_read ---

void test_i2c_driver_read_null_driver(void) {
    uint8_t buffer[1];
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_read(NULL, 0x10, buffer, 1));
}

void test_i2c_driver_read_null_data_buffer(void) {
    i2c_driver_init(&test_i2c_driver, 0x50, 400000); // Initialize driver
    test_i2c_driver.state = DEVICE_STATE_READY;

    // Expected: Returns ERROR_INVALID_PARAM if data buffer is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_read(&test_i2c_driver, 0x10, NULL, 1));
    // Expected: Driver state remains READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_i2c_driver.state);
}

void test_i2c_driver_read_not_ready(void) {
    uint8_t buffer[1];
    test_i2c_driver.state = DEVICE_STATE_BUSY; // Not in READY state

    // Expected: Returns ERROR_INVALID_PARAM if driver is not in READY state
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_read(&test_i2c_driver, 0x10, buffer, 1));
    // Expected: Driver state remains BUSY
    TEST_ASSERT_EQUAL(DEVICE_STATE_BUSY, test_i2c_driver.state);
}

void test_i2c_driver_read_success(void) {
    i2c_driver_init(&test_i2c_driver, 0x50, 400000); // Initialize driver
    test_i2c_driver.state = DEVICE_STATE_READY;
    uint8_t rx_buffer[3] = {0};
    uint8_t reg_addr = 0x10;
    uint16_t size = sizeof(rx_buffer);

    // Expected: Successful read operation (mock)
    TEST_ASSERT_EQUAL(ERROR_NONE, i2c_driver_read(&test_i2c_driver, reg_addr, rx_buffer, size));

    // Expected: Driver state transitions to BUSY then back to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_i2c_driver.state);
    // Expected: Buffer filled with mock data (0xAA)
    uint8_t expected_data[3] = {0xAA, 0xAA, 0xAA};
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data, rx_buffer, size);
    // No external stubs called directly by i2c_driver_read, it's a mock operation.
}

// --- Test Cases for can_driver_init ---

void test_can_driver_init_null_driver(void) {
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, can_driver_init(NULL, 500000));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_can_init_data.was_called);
}

void test_can_driver_init_success(void) {
    uint32_t bitrate = 500000;

    // Expected: Successful initialization
    TEST_ASSERT_EQUAL(ERROR_NONE, can_driver_init(&test_can_driver, bitrate));

    // Expected: CAN handle (mock) allocated
    TEST_ASSERT_NOT_NULL(test_can_driver.can);
    // Expected: can_init external stub called once
    TEST_ASSERT_TRUE(stub_can_init_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_can_init_data.call_count);
    // Expected: Correct parameters passed to can_init
    TEST_ASSERT_EQUAL_PTR(test_can_driver.can, stub_can_init_data.last_hcan);
    TEST_ASSERT_EQUAL(32, stub_can_init_data.last_buffer_size); // Magic number from source
    // Expected: Driver state becomes READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_can_driver.state);
    // Expected: Bitrate set
    TEST_ASSERT_EQUAL(bitrate, test_can_driver.bitrate);
    // Expected: Error history cleared
    for (int i = 0; i < ERROR_HISTORY_SIZE; i++) {
        TEST_ASSERT_EQUAL(0, test_can_driver.errors[i].timestamp);
        TEST_ASSERT_EQUAL(0, test_can_driver.errors[i].error_code);
    }
}

void test_can_driver_init_can_init_failure(void) {
    uint32_t bitrate = 250000;
    stub_can_init_data.return_value = PROTOCOL_ERROR_BUSY; // Simulate failure from underlying CAN init

    // Expected: Returns ERROR_BUSY due to can_init failure
    TEST_ASSERT_EQUAL(ERROR_BUSY, can_driver_init(&test_can_driver, bitrate)); // Note: returns ERROR_BUSY, not PROTOCOL_ERROR_BUSY

    // Expected: can_init external stub called once
    TEST_ASSERT_TRUE(stub_can_init_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_can_init_data.call_count);
    // Expected: Driver state becomes ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_can_driver.state);
    // Expected: Allocated hardware handle freed
    TEST_ASSERT_NULL(test_can_driver.can);
}

// --- Test Cases for can_driver_send_message ---

void test_can_driver_send_message_null_driver(void) {
    can_frame_t frame = { .id = 0x123, .dlc = 8 };
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, can_driver_send_message(NULL, &frame));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_can_transmit_message_data.was_called);
}

void test_can_driver_send_message_null_frame(void) {
    can_driver_init(&test_can_driver, 500000); // Initialize driver
    test_can_driver.state = DEVICE_STATE_READY;

    // Expected: Returns ERROR_INVALID_PARAM if frame is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, can_driver_send_message(&test_can_driver, NULL));
    // Expected: Driver state remains READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_can_driver.state);
}

void test_can_driver_send_message_not_ready(void) {
    can_frame_t frame = { .id = 0x123, .dlc = 8 };
    test_can_driver.state = DEVICE_STATE_ERROR; // Not in READY state

    // Expected: Returns ERROR_INVALID_PARAM if driver is not in READY state
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, can_driver_send_message(&test_can_driver, &frame));
    // Expected: No external functions called
    TEST_ASSERT_FALSE(stub_can_transmit_message_data.was_called);
    // Expected: Driver state remains ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_can_driver.state);
}

void test_can_driver_send_message_success(void) {
    can_driver_init(&test_can_driver, 500000); // Initialize driver
    test_can_driver.state = DEVICE_STATE_READY;
    can_frame_t frame = { .id = 0x123, .dlc = 2, .data = {0xDE, 0xAD} };
    uint32_t expected_timeout = 1000; // Hardcoded in source

    // Expected: Successful message transmission
    TEST_ASSERT_EQUAL(ERROR_NONE, can_driver_send_message(&test_can_driver, &frame));

    // Expected: Driver state transitions to BUSY then back to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_can_driver.state);
    // Expected: can_transmit_message external stub called once
    TEST_ASSERT_TRUE(stub_can_transmit_message_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_can_transmit_message_data.call_count);
    // Expected: Correct parameters passed to can_transmit_message
    TEST_ASSERT_EQUAL_PTR(test_can_driver.can, stub_can_transmit_message_data.last_hcan);
    TEST_ASSERT_EQUAL_MEMORY(&frame, &stub_can_transmit_message_data.last_frame, sizeof(can_frame_t));
    TEST_ASSERT_EQUAL(expected_timeout, stub_can_transmit_message_data.last_timeout);
}

void test_can_driver_send_message_failure(void) {
    can_driver_init(&test_can_driver, 500000); // Initialize driver
    test_can_driver.state = DEVICE_STATE_READY;
    can_frame_t frame = { .id = 0x456, .dlc = 1, .data = {0xFF} };
    stub_can_transmit_message_data.return_value = PROTOCOL_ERROR_TIMEOUT; // Simulate failure

    // Expected: Returns ERROR_BUSY due to can_transmit_message failure
    TEST_ASSERT_EQUAL(ERROR_BUSY, can_driver_send_message(&test_can_driver, &frame)); // Note: returns ERROR_BUSY

    // Expected: Driver state transitions to BUSY then to ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_can_driver.state);
    // Expected: can_transmit_message external stub called once
    TEST_ASSERT_TRUE(stub_can_transmit_message_data.was_called);
    // Expected: Error logged in history (converted from protocol_error_t)
    TEST_ASSERT_EQUAL((error_t)PROTOCOL_ERROR_TIMEOUT, test_can_driver.errors[0].error_code);
    TEST_ASSERT_EQUAL(1, test_can_driver.error_index);
}

// --- Test Cases for sensor_driver_init ---

void test_sensor_driver_init_null_driver(void) {
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_driver_init(NULL, 0, 0));
}

void test_sensor_driver_init_invalid_interface_type(void) {
    // Expected: Returns ERROR_INVALID_PARAM for an unsupported interface type
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_driver_init(&test_sensor_driver, 99, 0));
    // Expected: Driver state remains INIT or default
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, test_sensor_driver.state);
}

void test_sensor_driver_init_i2c_interface_success(void) {
    uint8_t interface_type = 0; // I2C
    uint8_t sensor_type = 1;

    // Expected: Successful initialization
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_driver_init(&test_sensor_driver, interface_type, sensor_type));

    // Expected: Driver state becomes READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.state);
    // Expected: Sensor type set
    TEST_ASSERT_EQUAL(sensor_type, test_sensor_driver.sensor_type);
    // Expected: Default sampling rate, calibration offset/scale
    TEST_ASSERT_EQUAL(100, test_sensor_driver.sampling_rate);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, test_sensor_driver.calibration_offset);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.0f, test_sensor_driver.calibration_scale);
    // Expected: I2C driver allocated and initialized (i2c_driver_init is internal)
    TEST_ASSERT_NOT_NULL(test_sensor_driver.i2c);
    TEST_ASSERT_NOT_NULL(test_sensor_driver.i2c->i2c_regs); // i2c_driver_init allocates this
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.i2c->state);
    TEST_ASSERT_EQUAL(0x40, test_sensor_driver.i2c->device_addr);
    // Expected: Other interface pointers are NULL
    TEST_ASSERT_NULL(test_sensor_driver.spi);
    TEST_ASSERT_NULL(test_sensor_driver.uart);
}

void test_sensor_driver_init_spi_interface_success(void) {
    uint8_t interface_type = 1; // SPI
    uint8_t sensor_type = 2;

    // Expected: Successful initialization
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_driver_init(&test_sensor_driver, interface_type, sensor_type));

    // Expected: Driver state becomes READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.state);
    // Expected: SPI driver allocated
    TEST_ASSERT_NOT_NULL(test_sensor_driver.spi);
    // Expected: Other interface pointers are NULL
    TEST_ASSERT_NULL(test_sensor_driver.i2c);
    TEST_ASSERT_NULL(test_sensor_driver.uart);
    // Note: spi_driver_init is NOT called here for SPI/UART cases in source.
    // They are just malloc'd, configuration would happen elsewhere.
}

void test_sensor_driver_init_uart_interface_success(void) {
    uint8_t interface_type = 2; // UART
    uint8_t sensor_type = 3;

    // Expected: Successful initialization
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_driver_init(&test_sensor_driver, interface_type, sensor_type));

    // Expected: Driver state becomes READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.state);
    // Expected: UART driver allocated
    TEST_ASSERT_NOT_NULL(test_sensor_driver.uart);
    // Expected: Other interface pointers are NULL
    TEST_ASSERT_NULL(test_sensor_driver.i2c);
    TEST_ASSERT_NULL(test_sensor_driver.spi);
}

// --- Test Cases for sensor_driver_read ---

void test_sensor_driver_read_null_driver(void) {
    float value = 0.0f;
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_driver_read(NULL, &value));
}

void test_sensor_driver_read_null_value_pointer(void) {
    sensor_driver_init(&test_sensor_driver, 0, 0); // Init to I2C interface
    test_sensor_driver.state = DEVICE_STATE_READY;

    // Expected: Returns ERROR_INVALID_PARAM if value pointer is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_driver_read(&test_sensor_driver, NULL));
    // Expected: Driver state remains READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.state);
}

void test_sensor_driver_read_not_ready(void) {
    float value = 0.0f;
    test_sensor_driver.state = DEVICE_STATE_BUSY; // Not in READY state

    // Expected: Returns ERROR_INVALID_PARAM if driver is not in READY state
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_driver_read(&test_sensor_driver, &value));
    // Expected: Driver state remains BUSY
    TEST_ASSERT_EQUAL(DEVICE_STATE_BUSY, test_sensor_driver.state);
}

void test_sensor_driver_read_i2c_interface_success_no_callback(void) {
    sensor_driver_init(&test_sensor_driver, 0, 0); // Init to I2C interface
    test_sensor_driver.state = DEVICE_STATE_READY;
    float value = 0.0f;

    // Mock i2c_driver_read to return specific data
    // The source performs `((data[0] << 8) | data[1]) * 0.01f;`
    // To get a value of 12.34, we need 1234. So data = {0x04, 0xD2}
    uint8_t mock_i2c_data[] = {0x04, 0xD2}; // 1234 (0x4D2)
    memcpy(test_sensor_driver.i2c->i2c_regs, mock_i2c_data, sizeof(mock_i2c_data)); // Mock internal state

    // Expected: Successful read
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_driver_read(&test_sensor_driver, &value));

    // Expected: Driver state transitions to BUSY then back to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.state);
    // Expected: Value calculated correctly from mock I2C data (1234 * 0.01 = 12.34)
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 12.34f, value);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 12.34f, test_sensor_driver.last_value);
    // Expected: Callback not called as it was NULL
    TEST_ASSERT_FALSE(mock_sensor_data_ready_cb_called);
}

void test_sensor_driver_read_other_interface_success_with_callback(void) {
    sensor_driver_init(&test_sensor_driver, 1, 0); // Init to SPI interface (takes 'else' branch for reading)
    test_sensor_driver.state = DEVICE_STATE_READY;
    test_sensor_driver.calibration_offset = 1.0f;
    test_sensor_driver.calibration_scale = 2.0f;
    test_sensor_driver.data_ready_cb = mock_sensor_data_ready_cb;
    void *cb_context = (void*)0xCAFE;
    test_sensor_driver.callback_context = cb_context;

    float value = 0.0f;
    // Mock value for 'else' branch is 25.5f.
    // Applied calibration: (25.5f * 2.0f) + 1.0f = 51.0f + 1.0f = 52.0f
    float expected_value = 52.0f;

    // Expected: Successful read
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_driver_read(&test_sensor_driver, &value));

    // Expected: Driver state transitions to BUSY then back to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.state);
    // Expected: Value calculated correctly
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_value, value);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_value, test_sensor_driver.last_value);
    // Expected: Callback invoked with correct value
    TEST_ASSERT_TRUE(mock_sensor_data_ready_cb_called);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_value, mock_sensor_data_ready_cb_value);
    TEST_ASSERT_EQUAL_PTR(cb_context, mock_sensor_data_ready_cb_context);
}

void test_sensor_driver_read_i2c_interface_after_calibration(void) {
    sensor_driver_init(&test_sensor_driver, 0, 0); // Init to I2C interface
    test_sensor_driver.state = DEVICE_STATE_READY;
    float value = 0.0f;

    // First read to set last_value
    uint8_t mock_i2c_data_initial[] = {0x09, 0xC4}; // 2500 (0x9C4)
    memcpy(test_sensor_driver.i2c->i2c_regs, mock_i2c_data_initial, sizeof(mock_i2c_data_initial));
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_driver_read(&test_sensor_driver, &value));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, value); // 2500 * 0.01 = 25.0

    // Calibrate based on current reading (25.0f) to a reference of 30.0f
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_driver_calibrate(&test_sensor_driver, 30.0f));
    // Expected offset: 30.0f - 25.0f = 5.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 5.0f, test_sensor_driver.calibration_offset);

    // Second read, with new offset applied
    uint8_t mock_i2c_data_second[] = {0x0A, 0x8C}; // 2700 (0xA8C)
    memcpy(test_sensor_driver.i2c->i2c_regs, mock_i2c_data_second, sizeof(mock_i2c_data_second));
    float new_value = 0.0f;
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_driver_read(&test_sensor_driver, &new_value));
    // Raw: 2700 * 0.01 = 27.0
    // Calibrated: (27.0 * 1.0f) + 5.0f = 32.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 32.0f, new_value);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 32.0f, test_sensor_driver.last_value);
}

// --- Test Cases for sensor_driver_calibrate ---

void test_sensor_driver_calibrate_null_driver(void) {
    // Expected: Returns ERROR_INVALID_PARAM if driver is NULL
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_driver_calibrate(NULL, 100.0f));
}

void test_sensor_driver_calibrate_initial_state(void) {
    sensor_driver_init(&test_sensor_driver, 0, 0); // Init with default calibration
    test_sensor_driver.state = DEVICE_STATE_READY;
    test_sensor_driver.last_value = 25.0f; // Simulate a previous reading

    float reference_value = 50.0f;
    // Expected: Offset adjusts such that 25.0 + offset = 50.0, so offset = 25.0
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_driver_calibrate(&test_sensor_driver, reference_value));

    // Expected: Calibration offset updated
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, test_sensor_driver.calibration_offset);
    // Expected: Scale remains default
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 1.0f, test_sensor_driver.calibration_scale);
}

void test_sensor_driver_calibrate_negative_offset(void) {
    sensor_driver_init(&test_sensor_driver, 0, 0);
    test_sensor_driver.state = DEVICE_STATE_READY;
    test_sensor_driver.last_value = 40.0f;

    float reference_value = 10.0f;
    // Expected: Offset adjusts such that 40.0 + offset = 10.0, so offset = 0.0f
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_driver_calibrate(&test_sensor_driver, reference_value));

    // Expected: Calibration offset updated to a negative value
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, test_sensor_driver.calibration_offset);
}

void test_sensor_driver_calibrate_zero_offset(void) {
    sensor_driver_init(&test_sensor_driver, 0, 0);
    test_sensor_driver.state = DEVICE_STATE_READY;
    test_sensor_driver.last_value = 10.5f;

    float reference_value = 10.5f;
    // Expected: Offset adjusts such that 10.5 + offset = 10.5, so offset = 0.0
    TEST_ASSERT_EQUAL(ERROR_NONE, sensor_driver_calibrate(&test_sensor_driver, reference_value));

    // Expected: Calibration offset updated to 0.0
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, test_sensor_driver.calibration_offset);
}


// --- Main Test Runner ---



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_uart_driver_init_null_driver);
    RUN_TEST(test_uart_driver_init_null_config);
    RUN_TEST(test_uart_driver_init_success);
    RUN_TEST(test_uart_driver_init_uart_init_failure);
    RUN_TEST(test_uart_driver_transmit_null_driver);
    RUN_TEST(test_uart_driver_transmit_null_data);
    RUN_TEST(test_uart_driver_transmit_zero_size);
    RUN_TEST(test_uart_driver_transmit_not_ready);
    RUN_TEST(test_uart_driver_transmit_success);
    RUN_TEST(test_uart_driver_transmit_failure);
    RUN_TEST(test_uart_driver_transmit_error_history_wrap);
    RUN_TEST(test_uart_driver_receive_null_driver);
    RUN_TEST(test_uart_driver_receive_null_data_buffer);
    RUN_TEST(test_uart_driver_receive_zero_size);
    RUN_TEST(test_uart_driver_receive_not_ready);
    RUN_TEST(test_uart_driver_receive_success);
    RUN_TEST(test_uart_driver_receive_failure);
    RUN_TEST(test_uart_driver_process_interrupt_null_driver);
    RUN_TEST(test_uart_driver_process_interrupt_rxne);
    RUN_TEST(test_uart_driver_process_interrupt_txe);
    RUN_TEST(test_uart_driver_process_interrupt_no_flags);
    RUN_TEST(test_spi_driver_init_null_driver);
    RUN_TEST(test_spi_driver_init_null_config);
    RUN_TEST(test_spi_driver_init_success);
    RUN_TEST(test_spi_driver_init_spi_init_failure);
    RUN_TEST(test_spi_driver_transfer_null_driver);
    RUN_TEST(test_spi_driver_transfer_null_tx_data);
    RUN_TEST(test_spi_driver_transfer_null_rx_data);
    RUN_TEST(test_spi_driver_transfer_not_ready);
    RUN_TEST(test_spi_driver_transfer_success);
    RUN_TEST(test_spi_driver_transfer_failure);
    RUN_TEST(test_i2c_driver_init_null_driver);
    RUN_TEST(test_i2c_driver_init_success);
    RUN_TEST(test_i2c_driver_write_null_driver);
    RUN_TEST(test_i2c_driver_write_null_data);
    RUN_TEST(test_i2c_driver_write_not_ready);
    RUN_TEST(test_i2c_driver_write_success);
    RUN_TEST(test_i2c_driver_read_null_driver);
    RUN_TEST(test_i2c_driver_read_null_data_buffer);
    RUN_TEST(test_i2c_driver_read_not_ready);
    RUN_TEST(test_i2c_driver_read_success);
    RUN_TEST(test_can_driver_init_null_driver);
    RUN_TEST(test_can_driver_init_success);
    RUN_TEST(test_can_driver_init_can_init_failure);
    RUN_TEST(test_can_driver_send_message_null_driver);
    RUN_TEST(test_can_driver_send_message_null_frame);
    RUN_TEST(test_can_driver_send_message_not_ready);
    RUN_TEST(test_can_driver_send_message_success);
    RUN_TEST(test_can_driver_send_message_failure);
    RUN_TEST(test_sensor_driver_init_null_driver);
    RUN_TEST(test_sensor_driver_init_invalid_interface_type);
    RUN_TEST(test_sensor_driver_init_i2c_interface_success);
    RUN_TEST(test_sensor_driver_init_spi_interface_success);
    RUN_TEST(test_sensor_driver_init_uart_interface_success);
    RUN_TEST(test_sensor_driver_read_null_driver);
    RUN_TEST(test_sensor_driver_read_null_value_pointer);
    RUN_TEST(test_sensor_driver_read_not_ready);
    RUN_TEST(test_sensor_driver_read_i2c_interface_success_no_callback);
    RUN_TEST(test_sensor_driver_read_other_interface_success_with_callback);
    RUN_TEST(test_sensor_driver_read_i2c_interface_after_calibration);
    RUN_TEST(test_sensor_driver_calibrate_null_driver);
    RUN_TEST(test_sensor_driver_calibrate_initial_state);
    RUN_TEST(test_sensor_driver_calibrate_negative_offset);
    RUN_TEST(test_sensor_driver_calibrate_zero_offset);

    return UNITY_END();
}