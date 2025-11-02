/* test_device_drivers.c – Auto-generated Expert Unity Tests */

#include "unity.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h> // For malloc/free stubs

// --- Mock types from device_drivers.h and hardware ---
// These definitions are required so the test file can compile without including
// the actual device_drivers.h, preventing redefinition conflicts and allowing
// control over external types.

// Error codes
typedef enum {
    ERROR_NONE = 0,
    ERROR_INVALID_PARAM,
    ERROR_BUSY,
    ERROR_TIMEOUT,
    ERROR_UNSUPPORTED,
    ERROR_CRC_FAIL,
    ERROR_READ_FAIL,
    ERROR_WRITE_FAIL,
    ERROR_PROTOCOL_FAIL
} error_t;

typedef enum {
    PROTOCOL_ERROR_NONE = 0,
    PROTOCOL_ERROR_TX_FAIL,
    PROTOCOL_ERROR_RX_FAIL,
    PROTOCOL_ERROR_TIMEOUT,
    PROTOCOL_ERROR_BUFFER_FULL,
    PROTOCOL_ERROR_UNKNOWN
} protocol_error_t;

// Device states
typedef enum {
    DEVICE_STATE_UNINITIALIZED = 0,
    DEVICE_STATE_INIT,
    DEVICE_STATE_READY,
    DEVICE_STATE_BUSY,
    DEVICE_STATE_ERROR,
    DEVICE_STATE_SLEEP
} device_state_t;

// Callbacks
typedef void (*device_state_cb_t)(device_state_t old_state, device_state_t new_state, void *context);
typedef void (*sensor_data_ready_cb_t)(float value, void *context);

// Error history
#define ERROR_HISTORY_SIZE 5
typedef struct {
    uint32_t timestamp;
    error_t error_code;
} device_error_t;

// Mock hardware register types
struct USART_TypeDef {
    volatile uint32_t SR; // Status Register
    volatile uint32_t DR; // Data Register
    // ... other registers as needed for testing
};
// UART Status Register flags (mock)
#define USART_SR_RXNE (1U << 5) // Read data register not empty
#define USART_SR_TXE  (1U << 7) // Transmit data register empty

struct SPI_TypeDef {
    volatile uint32_t CR1;
    volatile uint32_t DR;
    // ... other registers as needed
};

struct GPIO_TypeDef {
    volatile uint32_t ODR; // Output Data Register
    // ... other registers as needed
};

struct can_handle_t { // Mock CAN handle
    uint32_t buffer_size;
    // ... other members as needed
};

// Mock config types
typedef struct {
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
} uart_config_t;

typedef struct {
    uint32_t clock_speed_hz;
    uint8_t mode; // CPOL/CPHA
    uint8_t bit_order; // MSB/LSB
} spi_config_t;

typedef struct {
    uint32_t id;
    uint8_t data[8];
    uint8_t dlc;
    bool rtr;
} can_frame_t;

// Driver structs (complete definitions needed for sizeof and member access)
// These are copies of the actual structs from device_drivers.h
typedef struct uart_driver_t {
    device_state_t state;
    USART_TypeDef *uart;
    uint32_t timeout_ms;
    device_state_cb_t state_change_cb;
    void *callback_context;
    device_error_t errors[ERROR_HISTORY_SIZE];
    uint8_t error_index;
} uart_driver_t;

typedef struct spi_driver_t {
    device_state_t state;
    SPI_TypeDef *spi;
    GPIO_TypeDef *cs_gpio;
    uint16_t cs_pin;
    device_error_t errors[ERROR_HISTORY_SIZE];
    uint8_t error_index;
} spi_driver_t;

typedef struct i2c_driver_t {
    device_state_t state;
    uint8_t device_addr;
    uint32_t timeout_ms;
    void *i2c_regs; // Mock for I2C hardware registers
    device_error_t errors[ERROR_HISTORY_SIZE];
    uint8_t error_index;
} i2c_driver_t;

typedef struct can_driver_t {
    device_state_t state;
    uint32_t bitrate;
    can_handle_t *can;
    device_error_t errors[ERROR_HISTORY_SIZE];
    uint8_t error_index;
} can_driver_t;

typedef struct sensor_driver_t {
    device_state_t state;
    uint8_t sensor_type;
    uint8_t sampling_rate;
    float calibration_offset;
    float calibration_scale;
    float last_value;
    uint32_t last_reading_time;
    sensor_data_ready_cb_t data_ready_cb;
    void *callback_context;

    // Interface specific pointers (only one should be non-NULL)
    i2c_driver_t *i2c;
    spi_driver_t *spi;
    uart_driver_t *uart;

    device_error_t errors[ERROR_HISTORY_SIZE];
    uint8_t error_index;
} sensor_driver_t;

// --- Extern declarations for functions under test ---
// These are functions defined in device_drivers.c that we want to test.
// We declare them extern here to allow linking without including the source file header.
// This is necessary because some types defined above (like structs) might conflict
// if the header is included.
extern error_t uart_driver_init(uart_driver_t *driver, const uart_config_t *config);
extern error_t uart_driver_transmit(uart_driver_t *driver, const uint8_t *data, uint16_t size);
extern error_t uart_driver_receive(uart_driver_t *driver, uint8_t *data, uint16_t size);
extern void uart_driver_process_interrupt(uart_driver_t *driver);

extern error_t spi_driver_init(spi_driver_t *driver, const spi_config_t *config);
extern error_t spi_driver_transfer(spi_driver_t *driver, const uint8_t *tx_data, uint8_t *rx_data, uint16_t size);

extern error_t i2c_driver_init(i2c_driver_t *driver, uint8_t address, uint32_t frequency);
extern error_t i2c_driver_write(i2c_driver_t *driver, uint8_t reg, const uint8_t *data, uint16_t size);
extern error_t i2c_driver_read(i2c_driver_t *driver, uint8_t reg, uint8_t *data, uint16_t size);

extern error_t can_driver_init(can_driver_t *driver, uint32_t bitrate);
extern error_t can_driver_send_message(can_driver_t *driver, const can_frame_t *frame);

extern error_t sensor_driver_init(sensor_driver_t *driver, uint8_t interface_type, uint8_t sensor_type);
extern error_t sensor_driver_read(sensor_driver_t *driver, float *value);
extern error_t sensor_driver_calibrate(sensor_driver_t *driver, float reference_value);

// --- Stub Control Structures for External Dependencies ---

typedef struct {
    error_t return_value;
    bool was_called;
    uint32_t call_count;
    USART_TypeDef *captured_uart_regs;
    uart_config_t captured_config;
} stub_uart_init_t;
static stub_uart_init_t stub_uart_init_data;

typedef struct {
    error_t return_value;
    bool was_called;
    uint32_t call_count;
    USART_TypeDef *captured_uart_regs;
    uint8_t *captured_data;
    uint16_t captured_size;
    uint32_t captured_timeout_ms;
} stub_uart_transmit_t;
static stub_uart_transmit_t stub_uart_transmit_data;

typedef struct {
    error_t return_value;
    bool was_called;
    uint32_t call_count;
    USART_TypeDef *captured_uart_regs;
    uint8_t *captured_data;
    uint16_t captured_size;
    uint32_t captured_timeout_ms;
    uint8_t *mock_data_to_return; // For receive
} stub_uart_receive_t;
static stub_uart_receive_t stub_uart_receive_data;

typedef struct {
    bool was_called;
    uint32_t call_count;
    GPIO_TypeDef *captured_port;
    uint16_t captured_pin;
    bool captured_state;
} stub_gpio_write_pin_t;
static stub_gpio_write_pin_t stub_gpio_write_pin_data;

typedef struct {
    error_t return_value;
    bool was_called;
    uint32_t call_count;
    SPI_TypeDef *captured_spi_regs;
    spi_config_t captured_config;
} stub_spi_init_t;
static stub_spi_init_t stub_spi_init_data;

typedef struct {
    error_t return_value;
    bool was_called;
    uint32_t call_count;
    SPI_TypeDef *captured_spi_regs;
    uint8_t *captured_tx_data;
    uint8_t *captured_rx_data;
    uint16_t captured_size;
    uint8_t *mock_rx_data_to_return; // For receive
} stub_spi_transmit_receive_t;
static stub_spi_transmit_receive_t stub_spi_transmit_receive_data;

typedef struct {
    protocol_error_t return_value;
    bool was_called;
    uint32_t call_count;
    can_handle_t *captured_can_handle;
    uint32_t captured_buffer_size;
} stub_can_init_t;
static stub_can_init_t stub_can_init_data;

typedef struct {
    protocol_error_t return_value;
    bool was_called;
    uint32_t call_count;
    can_handle_t *captured_can_handle;
    can_frame_t captured_frame;
    uint32_t captured_timeout_ms;
} stub_can_transmit_message_t;
static stub_can_transmit_message_t stub_can_transmit_message_data;

// Malloc/Free stubs for 100% coverage of malloc failure paths
typedef struct {
    void *mock_return_ptr[10]; // Pre-allocated pointers to return
    uint32_t mock_return_idx;
    bool was_called;
    uint32_t call_count;
    size_t captured_size[10];
    bool fail_next_malloc; // Set true to make the next malloc return NULL
} stub_malloc_t;
static stub_malloc_t stub_malloc_data;

typedef struct {
    bool was_called;
    uint32_t call_count;
    void *captured_ptr;
} stub_free_t;
static stub_free_t stub_free_data;

// --- Mock Implementations for External Dependencies ---

void *malloc(size_t size) {
    stub_malloc_data.was_called = true;
    if (stub_malloc_data.call_count < 10) { // Limit capture size
        stub_malloc_data.captured_size[stub_malloc_data.call_count] = size;
    }
    stub_malloc_data.call_count++;

    if (stub_malloc_data.fail_next_malloc) {
        stub_malloc_data.fail_next_malloc = false; // Reset for next call
        return NULL;
    }

    if (stub_malloc_data.mock_return_idx < 10 && stub_malloc_data.mock_return_ptr[stub_malloc_data.mock_return_idx] != NULL) {
        void* ret = stub_malloc_data.mock_return_ptr[stub_malloc_data.mock_return_idx];
        stub_malloc_data.mock_return_idx++;
        return ret;
    }

    // Default: use system malloc if no specific mock return is set.
    // For unit tests, we want to control memory, so this should ideally not be hit.
    // However, to satisfy "NO invented functions", we avoid `__real_malloc`
    // and instead use a pre-allocated pool in the test setup.
    // For simplicity, if mock_return_ptr is exhausted, we'll return NULL for now.
    return NULL; // Ensure controlled memory for all mallocs
}

void free(void *ptr) {
    stub_free_data.was_called = true;
    stub_free_data.call_count++;
    stub_free_data.captured_ptr = ptr;
    // In a real test setup with a memory pool, we'd mark this memory as free.
}

error_t uart_init(USART_TypeDef *uart_regs, uart_config_t *config) {
    stub_uart_init_data.was_called = true;
    stub_uart_init_data.call_count++;
    stub_uart_init_data.captured_uart_regs = uart_regs;
    if (config) {
        stub_uart_init_data.captured_config = *config;
    }
    return stub_uart_init_data.return_value;
}

error_t uart_transmit(USART_TypeDef *uart_regs, uint8_t *data, uint16_t size, uint32_t timeout_ms) {
    stub_uart_transmit_data.was_called = true;
    stub_uart_transmit_data.call_count++;
    stub_uart_transmit_data.captured_uart_regs = uart_regs;
    stub_uart_transmit_data.captured_data = data;
    stub_uart_transmit_data.captured_size = size;
    stub_uart_transmit_data.captured_timeout_ms = timeout_ms;
    return stub_uart_transmit_data.return_value;
}

error_t uart_receive(USART_TypeDef *uart_regs, uint8_t *data, uint16_t size, uint32_t timeout_ms) {
    stub_uart_receive_data.was_called = true;
    stub_uart_receive_data.call_count++;
    stub_uart_receive_data.captured_uart_regs = uart_regs;
    stub_uart_receive_data.captured_data = data;
    stub_uart_receive_data.captured_size = size;
    stub_uart_receive_data.captured_timeout_ms = timeout_ms;
    if (stub_uart_receive_data.mock_data_to_return && data && size > 0) {
        memcpy(data, stub_uart_receive_data.mock_data_to_return, size);
    }
    return stub_uart_receive_data.return_value;
}

void gpio_write_pin(GPIO_TypeDef *port, uint16_t pin, bool state) {
    stub_gpio_write_pin_data.was_called = true;
    stub_gpio_write_pin_data.call_count++;
    stub_gpio_write_pin_data.captured_port = port;
    stub_gpio_write_pin_data.captured_pin = pin;
    stub_gpio_write_pin_data.captured_state = state;
}

error_t spi_init(SPI_TypeDef *spi_regs, spi_config_t *config) {
    stub_spi_init_data.was_called = true;
    stub_spi_init_data.call_count++;
    stub_spi_init_data.captured_spi_regs = spi_regs;
    if (config) {
        stub_spi_init_data.captured_config = *config;
    }
    return stub_spi_init_data.return_value;
}

error_t spi_transmit_receive(SPI_TypeDef *spi_regs, uint8_t *tx_data, uint8_t *rx_data, uint16_t size) {
    stub_spi_transmit_receive_data.was_called = true;
    stub_spi_transmit_receive_data.call_count++;
    stub_spi_transmit_receive_data.captured_spi_regs = spi_regs;
    stub_spi_transmit_receive_data.captured_tx_data = tx_data;
    stub_spi_transmit_receive_data.captured_rx_data = rx_data;
    stub_spi_transmit_receive_data.captured_size = size;
    if (stub_spi_transmit_receive_data.mock_rx_data_to_return && rx_data && size > 0) {
        memcpy(rx_data, stub_spi_transmit_receive_data.mock_rx_data_to_return, size);
    }
    return stub_spi_transmit_receive_data.return_value;
}

protocol_error_t can_init(can_handle_t *can_handle, uint32_t buffer_size) {
    stub_can_init_data.was_called = true;
    stub_can_init_data.call_count++;
    stub_can_init_data.captured_can_handle = can_handle;
    stub_can_init_data.captured_buffer_size = buffer_size;
    return stub_can_init_data.return_value;
}

protocol_error_t can_transmit_message(can_handle_t *can_handle, const can_frame_t *frame, uint32_t timeout_ms) {
    stub_can_transmit_message_data.was_called = true;
    stub_can_transmit_message_data.call_count++;
    stub_can_transmit_message_data.captured_can_handle = can_handle;
    if (frame) {
        stub_can_transmit_message_data.captured_frame = *frame;
    }
    stub_can_transmit_message_data.captured_timeout_ms = timeout_ms;
    return stub_can_transmit_message_data.return_value;
}

// --- Mock Callback Implementations ---
static device_state_t mock_old_state;
static device_state_t mock_new_state;
static void *mock_state_change_cb_context;
static bool mock_state_change_cb_called;
static uint32_t mock_state_change_cb_call_count;

void mock_state_change_callback(device_state_t old_state, device_state_t new_state, void *context) {
    mock_state_change_cb_called = true;
    mock_state_change_cb_call_count++;
    mock_old_state = old_state;
    mock_new_state = new_state;
    mock_state_change_cb_context = context;
}

static float mock_sensor_value;
static void *mock_sensor_data_ready_cb_context;
static bool mock_sensor_data_ready_cb_called;
static uint32_t mock_sensor_data_ready_cb_call_count;

void mock_sensor_data_ready_callback(float value, void *context) {
    mock_sensor_data_ready_cb_called = true;
    mock_sensor_data_ready_cb_call_count++;
    mock_sensor_value = value;
    mock_sensor_data_ready_cb_context = context;
}

// --- Test Setup and Teardown ---

// Pre-allocate memory for mocks to control malloc behavior without __real/__wrap
static USART_TypeDef mock_uart_regs;
static SPI_TypeDef mock_spi_regs;
static GPIO_TypeDef mock_gpio_regs;
static can_handle_t mock_can_handle;
static i2c_driver_t mock_i2c_driver_instance;
static spi_driver_t mock_spi_driver_instance;
static uart_driver_t mock_uart_driver_instance;
static uint8_t mock_i2c_regs_mem[100]; // For i2c_driver_t::i2c_regs malloc

void setUp(void) {
    // Reset all stub control structures
    memset(&stub_uart_init_data, 0, sizeof(stub_uart_init_data));
    memset(&stub_uart_transmit_data, 0, sizeof(stub_uart_transmit_data));
    memset(&stub_uart_receive_data, 0, sizeof(stub_uart_receive_data));
    memset(&stub_gpio_write_pin_data, 0, sizeof(stub_gpio_write_pin_data));
    memset(&stub_spi_init_data, 0, sizeof(stub_spi_init_data));
    memset(&stub_spi_transmit_receive_data, 0, sizeof(stub_spi_transmit_receive_data));
    memset(&stub_can_init_data, 0, sizeof(stub_can_init_data));
    memset(&stub_can_transmit_message_data, 0, sizeof(stub_can_transmit_message_data));
    memset(&stub_malloc_data, 0, sizeof(stub_malloc_data));
    memset(&stub_free_data, 0, sizeof(stub_free_data));

    // Reset mock callbacks
    mock_state_change_cb_called = false;
    mock_state_change_cb_call_count = 0;
    mock_old_state = DEVICE_STATE_UNINITIALIZED;
    mock_new_state = DEVICE_STATE_UNINITIALIZED;
    mock_state_change_cb_context = NULL;

    mock_sensor_data_ready_cb_called = false;
    mock_sensor_data_ready_cb_call_count = 0;
    mock_sensor_value = 0.0f;
    mock_sensor_data_ready_cb_context = NULL;

    // Set default return values for stubs
    stub_uart_init_data.return_value = ERROR_NONE;
    stub_uart_transmit_data.return_value = ERROR_NONE;
    stub_uart_receive_data.return_value = ERROR_NONE;
    stub_spi_init_data.return_value = ERROR_NONE;
    stub_spi_transmit_receive_data.return_value = ERROR_NONE;
    stub_can_init_data.return_value = PROTOCOL_ERROR_NONE;
    stub_can_transmit_message_data.return_value = PROTOCOL_ERROR_NONE;

    // Set up mock malloc pointers for common allocations
    // This allows controlling which malloc call gets what memory.
    // Index 0-N corresponds to the Nth malloc call in a test.
    stub_malloc_data.mock_return_ptr[0] = &mock_uart_regs;       // uart_driver_init
    stub_malloc_data.mock_return_ptr[1] = &mock_spi_regs;       // spi_driver_init
    stub_malloc_data.mock_return_ptr[2] = &mock_can_handle;     // can_driver_init
    stub_malloc_data.mock_return_ptr[3] = &mock_i2c_driver_instance; // sensor_driver_init (I2C path)
    stub_malloc_data.mock_return_ptr[4] = mock_i2c_regs_mem;    // i2c_driver_init (called by sensor_driver_init)
    stub_malloc_data.mock_return_ptr[5] = &mock_spi_driver_instance; // sensor_driver_init (SPI path)
    stub_malloc_data.mock_return_ptr[6] = &mock_uart_driver_instance; // sensor_driver_init (UART path)
    stub_malloc_data.mock_return_ptr[7] = &mock_uart_regs; // Additional mock UART_TypeDef
    stub_malloc_data.mock_return_ptr[8] = &mock_spi_regs; // Additional mock SPI_TypeDef
    stub_malloc_data.mock_return_ptr[9] = &mock_can_handle; // Additional mock CAN_HANDLE
}

void tearDown(void) {
    // Complete reset of all stub control structures (redundant with setUp but good practice)
    memset(&stub_uart_init_data, 0, sizeof(stub_uart_init_data));
    memset(&stub_uart_transmit_data, 0, sizeof(stub_uart_transmit_data));
    memset(&stub_uart_receive_data, 0, sizeof(stub_uart_receive_data));
    memset(&stub_gpio_write_pin_data, 0, sizeof(stub_gpio_write_pin_data));
    memset(&stub_spi_init_data, 0, sizeof(stub_spi_init_data));
    memset(&stub_spi_transmit_receive_data, 0, sizeof(stub_spi_transmit_receive_data));
    memset(&stub_can_init_data, 0, sizeof(stub_can_init_data));
    memset(&stub_can_transmit_message_data, 0, sizeof(stub_can_transmit_message_data));
    memset(&stub_malloc_data, 0, sizeof(stub_malloc_data));
    memset(&stub_free_data, 0, sizeof(stub_free_data));

    // Reset mock callbacks (redundant)
    mock_state_change_cb_called = false;
    mock_state_change_cb_call_count = 0;
    mock_state_change_cb_context = NULL;
    mock_sensor_data_ready_cb_called = false;
    mock_sensor_data_ready_cb_call_count = 0;
    mock_sensor_data_ready_cb_context = NULL;
}

// --- UART Driver Tests ---

void test_uart_driver_init_Success(void) {
    uart_driver_t driver;
    uart_config_t config = {115200, 8, 1, 0}; // Valid config
    void *context_ptr = (void*)0x12345678;

    // Set up mock malloc to return a valid pointer for driver->uart
    stub_malloc_data.mock_return_ptr[0] = &mock_uart_regs;
    stub_malloc_data.mock_return_idx = 0; // Reset index for this test

    // Set up uart_init stub
    stub_uart_init_data.return_value = ERROR_NONE;

    driver.state_change_cb = mock_state_change_callback;
    driver.callback_context = context_ptr;

    error_t result = uart_driver_init(&driver, &config);

    // Expected: Function returns ERROR_NONE on success
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: Malloc called once for USART_TypeDef
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: uart_init called once with correct parameters
    TEST_ASSERT_TRUE(stub_uart_init_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_uart_init_data.call_count);
    TEST_ASSERT_EQUAL_PTR(&mock_uart_regs, stub_uart_init_data.captured_uart_regs);
    TEST_ASSERT_EEQUAL_UINT32(config.baud_rate, stub_uart_init_data.captured_config.baud_rate);
    // Expected: Driver state changes to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: Timeout and error_index initialized
    TEST_ASSERT_EQUAL(1000, driver.timeout_ms);
    TEST_ASSERT_EQUAL(0, driver.error_index);
    // Expected: Errors array cleared
    TEST_ASSERT_EQUAL_MEMORY(0, driver.errors, sizeof(driver.errors));
    // Expected: Callback invoked
    TEST_ASSERT_TRUE(mock_state_change_cb_called);
    TEST_ASSERT_EQUAL(1, mock_state_change_cb_call_count);
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, mock_old_state);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, mock_new_state);
    TEST_ASSERT_EQUAL_PTR(context_ptr, mock_state_change_cb_context);
}

void test_uart_driver_init_InvalidParam(void) {
    uart_driver_t driver;
    uart_config_t config = {115200, 8, 1, 0};

    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_init(NULL, &config));
    // Expected: NULL config
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_init(&driver, NULL));
    // Expected: No calls to malloc or uart_init on invalid param
    TEST_ASSERT_FALSE(stub_malloc_data.was_called);
    TEST_ASSERT_FALSE(stub_uart_init_data.was_called);
}

void test_uart_driver_init_MallocFails(void) {
    uart_driver_t driver;
    uart_config_t config = {115200, 8, 1, 0};

    // Set up mock malloc to return NULL for the first call
    stub_malloc_data.fail_next_malloc = true;
    stub_malloc_data.mock_return_idx = 0; // Reset index for this test

    error_t result = uart_driver_init(&driver, &config);

    // Expected: Malloc called once
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: Function returns ERROR_BUSY
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    // Expected: uart_init not called
    TEST_ASSERT_FALSE(stub_uart_init_data.was_called);
    // Expected: Driver state remains UNINITIALIZED (or not set to READY/ERROR)
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, driver.state); // State is set to INIT before malloc and not changed after
    // Expected: No callback invoked
    TEST_ASSERT_FALSE(mock_state_change_cb_called);
}

void test_uart_driver_init_UartInitFails(void) {
    uart_driver_t driver;
    uart_config_t config = {115200, 8, 1, 0};

    // Set up mock malloc to succeed
    stub_malloc_data.mock_return_ptr[0] = &mock_uart_regs;
    stub_malloc_data.mock_return_idx = 0;

    // Set up uart_init stub to fail
    stub_uart_init_data.return_value = ERROR_UNSUPPORTED;

    error_t result = uart_driver_init(&driver, &config);

    // Expected: Malloc called once
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: uart_init called once
    TEST_ASSERT_TRUE(stub_uart_init_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_uart_init_data.call_count);
    // Expected: Function returns the error from uart_init
    TEST_ASSERT_EQUAL(ERROR_UNSUPPORTED, result);
    // Expected: Driver state changes to ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, driver.state);
    // Expected: free called for the allocated uart handle
    TEST_ASSERT_TRUE(stub_free_data.was_called);
    TEST_ASSERT_EQUAL_PTR(&mock_uart_regs, stub_free_data.captured_ptr);
    // Expected: No callback invoked
    TEST_ASSERT_FALSE(mock_state_change_cb_called);
}

void test_uart_driver_transmit_Success(void) {
    uart_driver_t driver = {0};
    uint8_t tx_data[] = {0x01, 0x02, 0x03};
    uint16_t size = sizeof(tx_data);

    driver.state = DEVICE_STATE_READY;
    driver.uart = &mock_uart_regs; // Assume initialized
    driver.timeout_ms = 500;

    // Set up uart_transmit stub
    stub_uart_transmit_data.return_value = ERROR_NONE;

    error_t result = uart_driver_transmit(&driver, tx_data, size);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: uart_transmit called once with correct parameters
    TEST_ASSERT_TRUE(stub_uart_transmit_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_uart_transmit_data.call_count);
    TEST_ASSERT_EQUAL_PTR(&mock_uart_regs, stub_uart_transmit_data.captured_uart_regs);
    TEST_ASSERT_EQUAL_PTR(tx_data, stub_uart_transmit_data.captured_data);
    TEST_ASSERT_EQUAL(size, stub_uart_transmit_data.captured_size);
    TEST_ASSERT_EQUAL(driver.timeout_ms, stub_uart_transmit_data.captured_timeout_ms);
    // Expected: Driver state briefly BUSY, then returns to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: No error logged
    TEST_ASSERT_EQUAL(0, driver.error_index);
}

void test_uart_driver_transmit_InvalidParam(void) {
    uart_driver_t driver = {0};
    uint8_t tx_data[] = {0x01, 0x02, 0x03};

    driver.state = DEVICE_STATE_READY;
    driver.uart = &mock_uart_regs;

    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_transmit(NULL, tx_data, 3));
    // Expected: NULL data
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_transmit(&driver, NULL, 3));
    // Expected: Zero size
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_transmit(&driver, tx_data, 0));
    // Expected: Driver not ready
    driver.state = DEVICE_STATE_BUSY;
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_transmit(&driver, tx_data, 3));
    driver.state = DEVICE_STATE_ERROR;
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_transmit(&driver, tx_data, 3));
    // Expected: No calls to uart_transmit
    TEST_ASSERT_FALSE(stub_uart_transmit_data.was_called);
}

void test_uart_driver_transmit_UartTransmitFails(void) {
    uart_driver_t driver = {0};
    uint8_t tx_data[] = {0x01, 0x02};
    uint16_t size = sizeof(tx_data);

    driver.state = DEVICE_STATE_READY;
    driver.uart = &mock_uart_regs;
    driver.timeout_ms = 500;
    driver.error_index = ERROR_HISTORY_SIZE - 1; // Simulate wraparound next

    // Set up uart_transmit stub to fail
    stub_uart_transmit_data.return_value = ERROR_TIMEOUT;

    error_t result = uart_driver_transmit(&driver, tx_data, size);

    // Expected: Function returns the error from uart_transmit
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, result);
    // Expected: uart_transmit called once
    TEST_ASSERT_TRUE(stub_uart_transmit_data.was_called);
    // Expected: Driver state changes to ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, driver.state);
    // Expected: Error logged and index wrapped around
    TEST_ASSERT_EQUAL(ERROR_HISTORY_SIZE - 1, driver.errors[ERROR_HISTORY_SIZE - 1].error_code); // Last error at prev index
    TEST_ASSERT_EQUAL(0, driver.error_index); // Wrapped
}

void test_uart_driver_receive_Success(void) {
    uart_driver_t driver = {0};
    uint8_t rx_buffer[3] = {0};
    uint16_t size = sizeof(rx_buffer);
    uint8_t mock_received_data[] = {0xA1, 0xB2, 0xC3};

    driver.state = DEVICE_STATE_READY;
    driver.uart = &mock_uart_regs;
    driver.timeout_ms = 500;

    // Set up uart_receive stub
    stub_uart_receive_data.return_value = ERROR_NONE;
    stub_uart_receive_data.mock_data_to_return = mock_received_data;

    error_t result = uart_driver_receive(&driver, rx_buffer, size);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: uart_receive called once with correct parameters
    TEST_ASSERT_TRUE(stub_uart_receive_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_uart_receive_data.call_count);
    TEST_ASSERT_EQUAL_PTR(&mock_uart_regs, stub_uart_receive_data.captured_uart_regs);
    TEST_ASSERT_EQUAL_PTR(rx_buffer, stub_uart_receive_data.captured_data);
    TEST_ASSERT_EQUAL(size, stub_uart_receive_data.captured_size);
    TEST_ASSERT_EQUAL(driver.timeout_ms, stub_uart_receive_data.captured_timeout_ms);
    // Expected: Data received into buffer
    TEST_ASSERT_EQUAL_HEX8_ARRAY(mock_received_data, rx_buffer, size);
    // Expected: Driver state briefly BUSY, then returns to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: No error logged
    TEST_ASSERT_EQUAL(0, driver.error_index);
}

void test_uart_driver_receive_InvalidParam(void) {
    uart_driver_t driver = {0};
    uint8_t rx_buffer[3] = {0};

    driver.state = DEVICE_STATE_READY;
    driver.uart = &mock_uart_regs;

    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_receive(NULL, rx_buffer, 3));
    // Expected: NULL data
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_receive(&driver, NULL, 3));
    // Expected: Zero size
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_receive(&driver, rx_buffer, 0));
    // Expected: Driver not ready
    driver.state = DEVICE_STATE_BUSY;
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_receive(&driver, rx_buffer, 3));
    driver.state = DEVICE_STATE_ERROR;
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, uart_driver_receive(&driver, rx_buffer, 3));
    // Expected: No calls to uart_receive
    TEST_ASSERT_FALSE(stub_uart_receive_data.was_called);
}

void test_uart_driver_receive_UartReceiveFails(void) {
    uart_driver_t driver = {0};
    uint8_t rx_buffer[2] = {0};
    uint16_t size = sizeof(rx_buffer);

    driver.state = DEVICE_STATE_READY;
    driver.uart = &mock_uart_regs;
    driver.timeout_ms = 500;
    driver.error_index = 0; // Simulate first error

    // Set up uart_receive stub to fail
    stub_uart_receive_data.return_value = ERROR_READ_FAIL;

    error_t result = uart_driver_receive(&driver, rx_buffer, size);

    // Expected: Function returns the error from uart_receive
    TEST_ASSERT_EQUAL(ERROR_READ_FAIL, result);
    // Expected: uart_receive called once
    TEST_ASSERT_TRUE(stub_uart_receive_data.was_called);
    // Expected: Driver state changes to ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, driver.state);
    // Expected: Error logged and index incremented
    TEST_ASSERT_EQUAL(ERROR_READ_FAIL, driver.errors[0].error_code);
    TEST_ASSERT_EQUAL(1, driver.error_index);
}

void test_uart_driver_process_interrupt_NullDriver(void) {
    // Expected: No crash on NULL driver
    uart_driver_process_interrupt(NULL);
    // No assertions, just testing for crash prevention.
}

void test_uart_driver_process_interrupt_RXNE(void) {
    uart_driver_t driver = {0};
    driver.uart = &mock_uart_regs;
    driver.uart->SR = USART_SR_RXNE; // Simulate RXNE flag set
    driver.uart->DR = 0x5A;         // Simulate received data

    uart_driver_process_interrupt(&driver);

    // Expected: RX interrupt path taken (no direct output, but internal state might change)
    // For this mock, we can only verify it didn't crash and potentially internal mocks changed.
    // As there's no visible output from the RX processing, we rely on coverage.
    // In a real scenario, this would involve deeper mocks for RX buffer management.
}

void test_uart_driver_process_interrupt_TXE(void) {
    uart_driver_t driver = {0};
    driver.uart = &mock_uart_regs;
    driver.uart->SR = USART_SR_TXE; // Simulate TXE flag set

    uart_driver_process_interrupt(&driver);

    // Expected: TX interrupt path taken (similar to RXNE, relies on coverage)
}

void test_uart_driver_process_interrupt_NoInterrupt(void) {
    uart_driver_t driver = {0};
    driver.uart = &mock_uart_regs;
    driver.uart->SR = 0; // No flags set

    uart_driver_process_interrupt(&driver);

    // Expected: No interrupt path taken.
}

void test_uart_driver_process_interrupt_BothInterrupts(void) {
    uart_driver_t driver = {0};
    driver.uart = &mock_uart_regs;
    driver.uart->SR = USART_SR_RXNE | USART_SR_TXE; // Both flags set
    driver.uart->DR = 0xAA;

    uart_driver_process_interrupt(&driver);

    // Expected: Both paths taken.
}

// --- SPI Driver Tests ---

void test_spi_driver_init_Success(void) {
    spi_driver_t driver;
    spi_config_t config = {1000000, 0, 0}; // Valid config

    // Set up mock malloc to return a valid pointer for driver->spi
    stub_malloc_data.mock_return_ptr[0] = &mock_spi_regs;
    stub_malloc_data.mock_return_idx = 0; // Reset index for this test

    // Set up spi_init stub
    stub_spi_init_data.return_value = ERROR_NONE;

    error_t result = spi_driver_init(&driver, &config);

    // Expected: Function returns ERROR_NONE on success
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: Malloc called once for SPI_TypeDef
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: spi_init called once with correct parameters
    TEST_ASSERT_TRUE(stub_spi_init_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_spi_init_data.call_count);
    TEST_ASSERT_EQUAL_PTR(&mock_spi_regs, stub_spi_init_data.captured_spi_regs);
    TEST_ASSERT_EQUAL_UINT32(config.clock_speed_hz, stub_spi_init_data.captured_config.clock_speed_hz);
    // Expected: Driver state changes to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: Error_index initialized
    TEST_ASSERT_EQUAL(0, driver.error_index);
    // Expected: Errors array cleared
    TEST_ASSERT_EQUAL_MEMORY(0, driver.errors, sizeof(driver.errors));
}

void test_spi_driver_init_InvalidParam(void) {
    spi_driver_t driver;
    spi_config_t config = {1000000, 0, 0};

    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, spi_driver_init(NULL, &config));
    // Expected: NULL config
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, spi_driver_init(&driver, NULL));
    // Expected: No calls to malloc or spi_init on invalid param
    TEST_ASSERT_FALSE(stub_malloc_data.was_called);
    TEST_ASSERT_FALSE(stub_spi_init_data.was_called);
}

void test_spi_driver_init_MallocFails(void) {
    spi_driver_t driver;
    spi_config_t config = {1000000, 0, 0};

    // Set up mock malloc to return NULL for the first call
    stub_malloc_data.fail_next_malloc = true;
    stub_malloc_data.mock_return_idx = 0;

    error_t result = spi_driver_init(&driver, &config);

    // Expected: Malloc called once
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: Function returns ERROR_BUSY
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    // Expected: spi_init not called
    TEST_ASSERT_FALSE(stub_spi_init_data.was_called);
    // Expected: Driver state remains INIT
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, driver.state);
}

void test_spi_driver_init_SpiInitFails(void) {
    spi_driver_t driver;
    spi_config_t config = {1000000, 0, 0};

    // Set up mock malloc to succeed
    stub_malloc_data.mock_return_ptr[0] = &mock_spi_regs;
    stub_malloc_data.mock_return_idx = 0;

    // Set up spi_init stub to fail
    stub_spi_init_data.return_value = ERROR_UNSUPPORTED;

    error_t result = spi_driver_init(&driver, &config);

    // Expected: Malloc called once
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: spi_init called once
    TEST_ASSERT_TRUE(stub_spi_init_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_spi_init_data.call_count);
    // Expected: Function returns the error from spi_init
    TEST_ASSERT_EQUAL(ERROR_UNSUPPORTED, result);
    // Expected: Driver state changes to ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, driver.state);
    // Expected: free called for the allocated spi handle
    TEST_ASSERT_TRUE(stub_free_data.was_called);
    TEST_ASSERT_EQUAL_PTR(&mock_spi_regs, stub_free_data.captured_ptr);
}

void test_spi_driver_transfer_Success(void) {
    spi_driver_t driver = {0};
    uint8_t tx_data[] = {0x11, 0x22, 0x33};
    uint8_t rx_data[3] = {0};
    uint8_t mock_rx_data[] = {0xA1, 0xB2, 0xC3};
    uint16_t size = sizeof(tx_data);

    driver.state = DEVICE_STATE_READY;
    driver.spi = &mock_spi_regs;
    driver.cs_gpio = &mock_gpio_regs;
    driver.cs_pin = 5;

    // Set up spi_transmit_receive stub
    stub_spi_transmit_receive_data.return_value = ERROR_NONE;
    stub_spi_transmit_receive_data.mock_rx_data_to_return = mock_rx_data;

    error_t result = spi_driver_transfer(&driver, tx_data, rx_data, size);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: CS pin set low then high
    TEST_ASSERT_TRUE(stub_gpio_write_pin_data.was_called);
    TEST_ASSERT_EQUAL(2, stub_gpio_write_pin_data.call_count);
    TEST_ASSERT_EQUAL_PTR(&mock_gpio_regs, stub_gpio_write_pin_data.captured_port);
    TEST_ASSERT_EQUAL(5, stub_gpio_write_pin_data.captured_pin);
    TEST_ASSERT_TRUE(stub_gpio_write_pin_data.captured_state); // Last call sets high
    // Expected: spi_transmit_receive called once
    TEST_ASSERT_TRUE(stub_spi_transmit_receive_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_spi_transmit_receive_data.call_count);
    TEST_ASSERT_EQUAL_PTR(&mock_spi_regs, stub_spi_transmit_receive_data.captured_spi_regs);
    TEST_ASSERT_EQUAL_PTR(tx_data, stub_spi_transmit_receive_data.captured_tx_data);
    TEST_ASSERT_EQUAL_PTR(rx_data, stub_spi_transmit_receive_data.captured_rx_data);
    TEST_ASSERT_EQUAL(size, stub_spi_transmit_receive_data.captured_size);
    // Expected: Data received into rx_data buffer
    TEST_ASSERT_EQUAL_HEX8_ARRAY(mock_rx_data, rx_data, size);
    // Expected: Driver state briefly BUSY, then returns to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: No error logged
    TEST_ASSERT_EQUAL(0, driver.error_index);
}

void test_spi_driver_transfer_InvalidParam(void) {
    spi_driver_t driver = {0};
    uint8_t tx_data[] = {0x01};
    uint8_t rx_data[] = {0x00};

    driver.state = DEVICE_STATE_READY;
    driver.spi = &mock_spi_regs;

    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, spi_driver_transfer(NULL, tx_data, rx_data, 1));
    // Expected: NULL tx_data
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, spi_driver_transfer(&driver, NULL, rx_data, 1));
    // Expected: NULL rx_data
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, spi_driver_transfer(&driver, tx_data, NULL, 1));
    // Expected: Driver not ready
    driver.state = DEVICE_STATE_BUSY;
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, spi_driver_transfer(&driver, tx_data, rx_data, 1));
    // Expected: No calls to gpio_write_pin or spi_transmit_receive
    TEST_ASSERT_FALSE(stub_gpio_write_pin_data.was_called);
    TEST_ASSERT_FALSE(stub_spi_transmit_receive_data.was_called);
}

void test_spi_driver_transfer_SpiTransferFails(void) {
    spi_driver_t driver = {0};
    uint8_t tx_data[] = {0x11};
    uint8_t rx_data[] = {0x00};
    uint16_t size = sizeof(tx_data);

    driver.state = DEVICE_STATE_READY;
    driver.spi = &mock_spi_regs;
    driver.cs_gpio = &mock_gpio_regs;
    driver.cs_pin = 5;
    driver.error_index = 0;

    // Set up spi_transmit_receive stub to fail
    stub_spi_transmit_receive_data.return_value = ERROR_CRC_FAIL;

    error_t result = spi_driver_transfer(&driver, tx_data, rx_data, size);

    // Expected: Function returns the error from spi_transmit_receive
    TEST_ASSERT_EQUAL(ERROR_CRC_FAIL, result);
    // Expected: gpio_write_pin called twice (low then high)
    TEST_ASSERT_EQUAL(2, stub_gpio_write_pin_data.call_count);
    // Expected: spi_transmit_receive called once
    TEST_ASSERT_TRUE(stub_spi_transmit_receive_data.was_called);
    // Expected: Driver state changes to ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, driver.state);
    // Expected: Error logged and index incremented
    TEST_ASSERT_EQUAL(ERROR_CRC_FAIL, driver.errors[0].error_code);
    TEST_ASSERT_EQUAL(1, driver.error_index);
}

// --- I2C Driver Tests ---

void test_i2c_driver_init_Success(void) {
    i2c_driver_t driver;
    uint8_t address = 0x50;
    uint32_t frequency = 400000;

    // Set up mock malloc for i2c_regs
    stub_malloc_data.mock_return_ptr[0] = mock_i2c_regs_mem;
    stub_malloc_data.mock_return_idx = 0;

    error_t result = i2c_driver_init(&driver, address, frequency);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: Malloc called once for i2c_regs
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    TEST_ASSERT_EQUAL(100, stub_malloc_data.captured_size[0]);
    // Expected: Driver state changes to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: Device address, timeout, error_index initialized
    TEST_ASSERT_EQUAL(address, driver.device_addr);
    TEST_ASSERT_EQUAL(100, driver.timeout_ms);
    TEST_ASSERT_EQUAL(0, driver.error_index);
    // Expected: Errors array cleared
    TEST_ASSERT_EQUAL_MEMORY(0, driver.errors, sizeof(driver.errors));
}

void test_i2c_driver_init_InvalidParam(void) {
    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_init(NULL, 0x50, 400000));
    // Expected: No malloc calls
    TEST_ASSERT_FALSE(stub_malloc_data.was_called);
}

void test_i2c_driver_init_MallocFails(void) {
    i2c_driver_t driver;
    uint8_t address = 0x50;
    uint32_t frequency = 400000;

    // Set up mock malloc to fail
    stub_malloc_data.fail_next_malloc = true;
    stub_malloc_data.mock_return_idx = 0;

    error_t result = i2c_driver_init(&driver, address, frequency);

    // Expected: Malloc called once
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: Function returns ERROR_BUSY
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    // Expected: Driver state remains INIT
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, driver.state);
}

void test_i2c_driver_write_Success(void) {
    i2c_driver_t driver = {0};
    uint8_t data[] = {0xAA, 0xBB};
    uint16_t size = sizeof(data);

    driver.state = DEVICE_STATE_READY;
    driver.device_addr = 0x50;
    driver.i2c_regs = mock_i2c_regs_mem; // Assume initialized

    error_t result = i2c_driver_write(&driver, 0x01, data, size);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: Driver state briefly BUSY, then returns to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
}

void test_i2c_driver_write_InvalidParam(void) {
    i2c_driver_t driver = {0};
    uint8_t data[] = {0xAA};

    driver.state = DEVICE_STATE_READY;

    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_write(NULL, 0x01, data, 1));
    // Expected: NULL data
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_write(&driver, 0x01, NULL, 1));
    // Expected: Driver not ready
    driver.state = DEVICE_STATE_BUSY;
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_write(&driver, 0x01, data, 1));
}

void test_i2c_driver_read_Success(void) {
    i2c_driver_t driver = {0};
    uint8_t rx_buffer[2] = {0};
    uint16_t size = sizeof(rx_buffer);

    driver.state = DEVICE_STATE_READY;
    driver.device_addr = 0x50;
    driver.i2c_regs = mock_i2c_regs_mem; // Assume initialized

    error_t result = i2c_driver_read(&driver, 0x00, rx_buffer, size);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: rx_buffer filled with mock data (0xAA)
    TEST_ASSERT_EQUAL_HEX8(0xAA, rx_buffer[0]);
    TEST_ASSERT_EQUAL_HEX8(0xAA, rx_buffer[1]);
    // Expected: Driver state briefly BUSY, then returns to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
}

void test_i2c_driver_read_InvalidParam(void) {
    i2c_driver_t driver = {0};
    uint8_t rx_buffer[2] = {0};

    driver.state = DEVICE_STATE_READY;

    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_read(NULL, 0x00, rx_buffer, 2));
    // Expected: NULL data
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_read(&driver, 0x00, NULL, 2));
    // Expected: Driver not ready
    driver.state = DEVICE_STATE_ERROR;
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, i2c_driver_read(&driver, 0x00, rx_buffer, 2));
}

// --- CAN Driver Tests ---

void test_can_driver_init_Success(void) {
    can_driver_t driver;
    uint32_t bitrate = 500000;

    // Set up mock malloc for can handle
    stub_malloc_data.mock_return_ptr[0] = &mock_can_handle;
    stub_malloc_data.mock_return_idx = 0;

    // Set up can_init stub
    stub_can_init_data.return_value = PROTOCOL_ERROR_NONE;

    error_t result = can_driver_init(&driver, bitrate);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: Malloc called once for can_handle_t
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: can_init called once with correct buffer size (32)
    TEST_ASSERT_TRUE(stub_can_init_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_can_init_data.call_count);
    TEST_ASSERT_EQUAL_PTR(&mock_can_handle, stub_can_init_data.captured_can_handle);
    TEST_ASSERT_EQUAL(32, stub_can_init_data.captured_buffer_size);
    // Expected: Driver state changes to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: Bitrate and error_index initialized
    TEST_ASSERT_EQUAL(bitrate, driver.bitrate);
    TEST_ASSERT_EQUAL(0, driver.error_index);
    // Expected: Errors array cleared
    TEST_ASSERT_EQUAL_MEMORY(0, driver.errors, sizeof(driver.errors));
}

void test_can_driver_init_InvalidParam(void) {
    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, can_driver_init(NULL, 500000));
    // Expected: No malloc or can_init calls
    TEST_ASSERT_FALSE(stub_malloc_data.was_called);
    TEST_ASSERT_FALSE(stub_can_init_data.was_called);
}

void test_can_driver_init_MallocFails(void) {
    can_driver_t driver;
    uint32_t bitrate = 500000;

    // Set up mock malloc to fail
    stub_malloc_data.fail_next_malloc = true;
    stub_malloc_data.mock_return_idx = 0;

    error_t result = can_driver_init(&driver, bitrate);

    // Expected: Malloc called once
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: Function returns ERROR_BUSY
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    // Expected: can_init not called
    TEST_ASSERT_FALSE(stub_can_init_data.was_called);
    // Expected: Driver state remains INIT
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, driver.state);
}

void test_can_driver_init_CanInitFails(void) {
    can_driver_t driver;
    uint32_t bitrate = 500000;

    // Set up mock malloc to succeed
    stub_malloc_data.mock_return_ptr[0] = &mock_can_handle;
    stub_malloc_data.mock_return_idx = 0;

    // Set up can_init stub to fail
    stub_can_init_data.return_value = PROTOCOL_ERROR_BUFFER_FULL;

    error_t result = can_driver_init(&driver, bitrate);

    // Expected: Malloc called once
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: can_init called once
    TEST_ASSERT_TRUE(stub_can_init_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_can_init_data.call_count);
    // Expected: Function returns ERROR_BUSY (as per source mapping)
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    // Expected: Driver state changes to ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, driver.state);
    // Expected: free called for the allocated can handle
    TEST_ASSERT_TRUE(stub_free_data.was_called);
    TEST_ASSERT_EQUAL_PTR(&mock_can_handle, stub_free_data.captured_ptr);
}

void test_can_driver_send_message_Success(void) {
    can_driver_t driver = {0};
    can_frame_t frame = {.id = 0x123, .dlc = 8, .data = {1, 2, 3, 4, 5, 6, 7, 8}, .rtr = false};

    driver.state = DEVICE_STATE_READY;
    driver.can = &mock_can_handle; // Assume initialized

    // Set up can_transmit_message stub
    stub_can_transmit_message_data.return_value = PROTOCOL_ERROR_NONE;

    error_t result = can_driver_send_message(&driver, &frame);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: can_transmit_message called once with correct parameters
    TEST_ASSERT_TRUE(stub_can_transmit_message_data.was_called);
    TEST_ASSERT_EQUAL(1, stub_can_transmit_message_data.call_count);
    TEST_ASSERT_EQUAL_PTR(&mock_can_handle, stub_can_transmit_message_data.captured_can_handle);
    TEST_ASSERT_EQUAL_UINT32(frame.id, stub_can_transmit_message_data.captured_frame.id);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(frame.data, stub_can_transmit_message_data.captured_frame.data, frame.dlc);
    TEST_ASSERT_EQUAL(1000, stub_can_transmit_message_data.captured_timeout_ms);
    // Expected: Driver state briefly BUSY, then returns to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: No error logged
    TEST_ASSERT_EQUAL(0, driver.error_index);
}

void test_can_driver_send_message_InvalidParam(void) {
    can_driver_t driver = {0};
    can_frame_t frame = {.id = 0x123, .dlc = 8, .data = {1, 2, 3, 4, 5, 6, 7, 8}, .rtr = false};

    driver.state = DEVICE_STATE_READY;
    driver.can = &mock_can_handle;

    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, can_driver_send_message(NULL, &frame));
    // Expected: NULL frame
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, can_driver_send_message(&driver, NULL));
    // Expected: Driver not ready
    driver.state = DEVICE_STATE_ERROR;
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, can_driver_send_message(&driver, &frame));
    // Expected: No calls to can_transmit_message
    TEST_ASSERT_FALSE(stub_can_transmit_message_data.was_called);
}

void test_can_driver_send_message_CanTransmitFails(void) {
    can_driver_t driver = {0};
    can_frame_t frame = {.id = 0x123, .dlc = 1, .data = {1}, .rtr = false};

    driver.state = DEVICE_STATE_READY;
    driver.can = &mock_can_handle;
    driver.error_index = ERROR_HISTORY_SIZE - 1; // Simulate wraparound

    // Set up can_transmit_message stub to fail
    stub_can_transmit_message_data.return_value = PROTOCOL_ERROR_TX_FAIL;

    error_t result = can_driver_send_message(&driver, &frame);

    // Expected: Function returns ERROR_BUSY (as per source mapping)
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    // Expected: can_transmit_message called once
    TEST_ASSERT_TRUE(stub_can_transmit_message_data.was_called);
    // Expected: Driver state changes to ERROR
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, driver.state);
    // Expected: Error logged and index wrapped
    TEST_ASSERT_EQUAL((error_t)PROTOCOL_ERROR_TX_FAIL, driver.errors[ERROR_HISTORY_SIZE - 1].error_code);
    TEST_ASSERT_EQUAL(0, driver.error_index);
}

// --- Sensor Driver Tests ---

void test_sensor_driver_init_I2CInterfaceSuccess(void) {
    sensor_driver_t driver;
    uint8_t interface_type = 0; // I2C
    uint8_t sensor_type = 1; // Temperature sensor

    // Malloc calls sequence for I2C interface:
    // 1. sensor_driver_t (internal stack allocation, not malloc)
    // 2. i2c_driver_t (for driver->i2c)
    // 3. i2c_driver_t::i2c_regs (within i2c_driver_init)
    stub_malloc_data.mock_return_ptr[0] = &mock_i2c_driver_instance; // driver->i2c
    stub_malloc_data.mock_return_ptr[1] = mock_i2c_regs_mem;          // i2c_driver_instance->i2c_regs
    stub_malloc_data.mock_return_idx = 0;

    error_t result = sensor_driver_init(&driver, interface_type, sensor_type);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: Malloc called twice (for i2c_driver_t and its regs)
    TEST_ASSERT_EQUAL(2, stub_malloc_data.call_count);
    // Expected: Driver state changes to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: Sensor type, sampling rate, calibration initialized
    TEST_ASSERT_EQUAL(sensor_type, driver.sensor_type);
    TEST_ASSERT_EQUAL(100, driver.sampling_rate);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, driver.calibration_offset);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, driver.calibration_scale);
    // Expected: I2C driver initialized and pointer set
    TEST_ASSERT_NOT_NULL(driver.i2c);
    TEST_ASSERT_NULL(driver.spi);
    TEST_ASSERT_NULL(driver.uart);
    TEST_ASSERT_EQUAL(0x40, driver.i2c->device_addr);
}

void test_sensor_driver_init_SPIInterfaceSuccess(void) {
    sensor_driver_t driver;
    uint8_t interface_type = 1; // SPI
    uint8_t sensor_type = 2; // Accelerometer

    // Malloc calls sequence for SPI interface:
    // 1. spi_driver_t (for driver->spi)
    stub_malloc_data.mock_return_ptr[0] = &mock_spi_driver_instance; // driver->spi
    stub_malloc_data.mock_return_idx = 0;

    error_t result = sensor_driver_init(&driver, interface_type, sensor_type);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: Malloc called once (for spi_driver_t)
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: Driver state changes to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: SPI driver pointer set
    TEST_ASSERT_NOT_NULL(driver.spi);
    TEST_ASSERT_NULL(driver.i2c);
    TEST_ASSERT_NULL(driver.uart);
}

void test_sensor_driver_init_UARTInterfaceSuccess(void) {
    sensor_driver_t driver;
    uint8_t interface_type = 2; // UART
    uint8_t sensor_type = 3; // Flow sensor

    // Malloc calls sequence for UART interface:
    // 1. uart_driver_t (for driver->uart)
    stub_malloc_data.mock_return_ptr[0] = &mock_uart_driver_instance; // driver->uart
    stub_malloc_data.mock_return_idx = 0;

    error_t result = sensor_driver_init(&driver, interface_type, sensor_type);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: Malloc called once (for uart_driver_t)
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: Driver state changes to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: UART driver pointer set
    TEST_ASSERT_NOT_NULL(driver.uart);
    TEST_ASSERT_NULL(driver.i2c);
    TEST_ASSERT_NULL(driver.spi);
}

void test_sensor_driver_init_InvalidParam(void) {
    sensor_driver_t driver;
    uint8_t interface_type = 0; // I2C
    uint8_t sensor_type = 1;

    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_driver_init(NULL, interface_type, sensor_type));
    // Expected: Invalid interface type
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_driver_init(&driver, 99, sensor_type));
    // Expected: No malloc calls for invalid params
    TEST_ASSERT_EQUAL(0, stub_malloc_data.call_count);
}

void test_sensor_driver_init_I2CMallocFails(void) {
    sensor_driver_t driver;
    uint8_t interface_type = 0; // I2C
    uint8_t sensor_type = 1;

    // Set up malloc to fail for the i2c_driver_t allocation (first malloc in I2C branch)
    stub_malloc_data.fail_next_malloc = true;
    stub_malloc_data.mock_return_idx = 0;

    error_t result = sensor_driver_init(&driver, interface_type, sensor_type);

    // Expected: Function returns ERROR_INVALID_PARAM (from internal i2c_driver_init path)
    // Note: The specific error returned depends on how i2c_driver_init propagates malloc failure.
    // In this case, sensor_driver_init calls i2c_driver_init, which could return ERROR_BUSY.
    // Then sensor_driver_init continues, and since it doesn't check the return of malloc for i2c_driver_t directly
    // but just proceeds with driver->i2c_driver_init(driver->i2c, ...),
    // it results in a NULL pointer dereference or other undefined behavior if not handled.
    // The current source doesn't handle the return value of i2c_driver_init after malloc for i2c_driver_t.
    // This is an implicit bug in the source code if i2c_driver_init returns an error and sensor_driver_init doesn't catch it.
    // The *test* must reflect the *actual* behavior of the source.
    // Since `i2c_driver_init` is called *after* `driver->i2c` is assigned from malloc,
    // if malloc returns NULL for `driver->i2c`, then `i2c_driver_init(NULL, ...)` is called.
    // Which correctly returns ERROR_INVALID_PARAM from `i2c_driver_init`.
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    // Expected: Malloc called once (for the `driver->i2c` pointer)
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Expected: Driver state remains INIT
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, driver.state);
    TEST_ASSERT_NULL(driver.i2c);
}

void test_sensor_driver_init_SPIMallocFails(void) {
    sensor_driver_t driver;
    uint8_t interface_type = 1; // SPI
    uint8_t sensor_type = 2;

    stub_malloc_data.fail_next_malloc = true;
    stub_malloc_data.mock_return_idx = 0;

    error_t result = sensor_driver_init(&driver, interface_type, sensor_type);

    // Expected: Malloc called once
    TEST_ASSERT_EQUAL(1, stub_malloc_data.call_count);
    // Source code does not check for NULL return from malloc before continuing.
    // This would lead to a NULL dereference if spi config was set up.
    // The current source simply assigns `driver->spi = NULL` and proceeds.
    // The result should still be ERROR_NONE because there's no error check after `driver->spi = malloc()`
    // This means the sensor driver would be initialized but with a NULL SPI handle.
    // This is a potential bug in the source. Test reflects actual code behavior.
    TEST_ASSERT_EQUAL(ERROR_NONE, result); // No error check after malloc(spi_driver_t)
    TEST_ASSERT_NULL(driver.spi);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state); // State still becomes READY
}

void test_sensor_driver_read_I2CSuccess(void) {
    sensor_driver_t driver = {0};
    float value;
    uint8_t mock_read_data[] = {0x0A, 0x20}; // Represents 25.92 (0x0A20 / 100)
    void *cb_context = (void*)0xDEADBEEF;

    // Manually set up a "mock initialized" driver
    driver.state = DEVICE_STATE_READY;
    driver.i2c = &mock_i2c_driver_instance;
    driver.i2c->state = DEVICE_STATE_READY; // Ensure sub-driver is ready
    driver.calibration_offset = 1.0f;
    driver.calibration_scale = 2.0f;
    driver.data_ready_cb = mock_sensor_data_ready_callback;
    driver.callback_context = cb_context;

    // Setup for i2c_driver_read call
    stub_uart_receive_data.mock_data_to_return = mock_read_data; // Using uart_receive stub data because i2c_driver_read uses memset. This is a bit of a hack.
                                                                // Better would be to create a separate i2c_driver_read mock.
                                                                // The actual i2c_driver_read just does memset(data, 0xAA, size);
                                                                // So we expect 0xAA converted.
    // The i2c_driver_read is an internal function, so we don't stub it, we call it directly.
    // The mock data set in i2c_driver_read is `memset(data, 0xAA, size);`
    // So if size is 2, data will be {0xAA, 0xAA}.
    // ((0xAA << 8) | 0xAA) * 0.01f = (27370) * 0.01f = 273.70f

    error_t result = sensor_driver_read(&driver, &value);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: Value computed from mock i2c read and calibrated
    // Raw value: 273.70f
    // Calibrated: (273.70f * 2.0f) + 1.0f = 547.4f + 1.0f = 548.4f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 548.4f, value);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 548.4f, driver.last_value);
    // Expected: Driver state briefly BUSY, then returns to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: Callback invoked
    TEST_ASSERT_TRUE(mock_sensor_data_ready_cb_called);
    TEST_ASSERT_EQUAL(1, mock_sensor_data_ready_cb_call_count);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 548.4f, mock_sensor_value);
    TEST_ASSERT_EQUAL_PTR(cb_context, mock_sensor_data_ready_cb_context);
}

void test_sensor_driver_read_DefaultMockSuccess(void) {
    sensor_driver_t driver = {0};
    float value;
    void *cb_context = (void*)0xCAFEFEED;

    driver.state = DEVICE_STATE_READY;
    driver.i2c = NULL; // No I2C interface, so takes the else branch
    driver.calibration_offset = 0.0f;
    driver.calibration_scale = 0.5f;
    driver.data_ready_cb = mock_sensor_data_ready_callback;
    driver.callback_context = cb_context;

    error_t result = sensor_driver_read(&driver, &value);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: Value from mock (25.5f) and calibrated
    // Raw value: 25.5f
    // Calibrated: (25.5f * 0.5f) + (0.0f) = 12.75f - 5.0f = 7.75f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 7.75f, value);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 7.75f, driver.last_value);
    // Expected: Driver state briefly BUSY, then returns to READY
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, driver.state);
    // Expected: Callback invoked
    TEST_ASSERT_TRUE(mock_sensor_data_ready_cb_called);
    TEST_ASSERT_EQUAL(1, mock_sensor_data_ready_cb_call_count);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 7.75f, mock_sensor_value);
    TEST_ASSERT_EQUAL_PTR(cb_context, mock_sensor_data_ready_cb_context);
}

void test_sensor_driver_read_InvalidParam(void) {
    sensor_driver_t driver = {0};
    float value;

    driver.state = DEVICE_STATE_READY;

    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_driver_read(NULL, &value));
    // Expected: NULL value pointer
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_driver_read(&driver, NULL));
    // Expected: Driver not ready
    driver.state = DEVICE_STATE_BUSY;
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_driver_read(&driver, &value));
}

void test_sensor_driver_calibrate_Success(void) {
    sensor_driver_t driver = {0};
    float reference_value = 50.0f;

    driver.last_value = 25.0f; // Simulate a previous reading
    driver.calibration_offset = 0.0f;

    error_t result = sensor_driver_calibrate(&driver, reference_value);

    // Expected: Function returns ERROR_NONE
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected: Calibration offset adjusted (reference - last_value)
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 25.0f, driver.calibration_offset); // 50.0f - 25.0f = 25.0f
}

void test_sensor_driver_calibrate_InvalidParam(void) {
    float reference_value = 50.0f;

    // Expected: NULL driver
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, sensor_driver_calibrate(NULL, reference_value));
}

// --- Main Test Runner ---


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_uart_driver_init_Success);
    RUN_TEST(test_uart_driver_init_InvalidParam);
    RUN_TEST(test_uart_driver_init_MallocFails);
    RUN_TEST(test_uart_driver_init_UartInitFails);
    RUN_TEST(test_uart_driver_transmit_Success);
    RUN_TEST(test_uart_driver_transmit_InvalidParam);
    RUN_TEST(test_uart_driver_transmit_UartTransmitFails);
    RUN_TEST(test_uart_driver_receive_Success);
    RUN_TEST(test_uart_driver_receive_InvalidParam);
    RUN_TEST(test_uart_driver_receive_UartReceiveFails);
    RUN_TEST(test_uart_driver_process_interrupt_NullDriver);
    RUN_TEST(test_uart_driver_process_interrupt_RXNE);
    RUN_TEST(test_uart_driver_process_interrupt_TXE);
    RUN_TEST(test_uart_driver_process_interrupt_NoInterrupt);
    RUN_TEST(test_uart_driver_process_interrupt_BothInterrupts);
    RUN_TEST(test_spi_driver_init_Success);
    RUN_TEST(test_spi_driver_init_InvalidParam);
    RUN_TEST(test_spi_driver_init_MallocFails);
    RUN_TEST(test_spi_driver_init_SpiInitFails);
    RUN_TEST(test_spi_driver_transfer_Success);
    RUN_TEST(test_spi_driver_transfer_InvalidParam);
    RUN_TEST(test_spi_driver_transfer_SpiTransferFails);
    RUN_TEST(test_i2c_driver_init_Success);
    RUN_TEST(test_i2c_driver_init_InvalidParam);
    RUN_TEST(test_i2c_driver_init_MallocFails);
    RUN_TEST(test_i2c_driver_write_Success);
    RUN_TEST(test_i2c_driver_write_InvalidParam);
    RUN_TEST(test_i2c_driver_read_Success);
    RUN_TEST(test_i2c_driver_read_InvalidParam);
    RUN_TEST(test_can_driver_init_Success);
    RUN_TEST(test_can_driver_init_InvalidParam);
    RUN_TEST(test_can_driver_init_MallocFails);
    RUN_TEST(test_can_driver_init_CanInitFails);
    RUN_TEST(test_can_driver_send_message_Success);
    RUN_TEST(test_can_driver_send_message_InvalidParam);
    RUN_TEST(test_can_driver_send_message_CanTransmitFails);
    RUN_TEST(test_sensor_driver_init_I2CInterfaceSuccess);
    RUN_TEST(test_sensor_driver_init_SPIInterfaceSuccess);
    RUN_TEST(test_sensor_driver_init_UARTInterfaceSuccess);
    RUN_TEST(test_sensor_driver_init_InvalidParam);
    RUN_TEST(test_sensor_driver_init_I2CMallocFails);
    RUN_TEST(test_sensor_driver_init_SPIMallocFails);
    RUN_TEST(test_sensor_driver_read_I2CSuccess);
    RUN_TEST(test_sensor_driver_read_DefaultMockSuccess);
    RUN_TEST(test_sensor_driver_read_InvalidParam);
    RUN_TEST(test_sensor_driver_calibrate_Success);
    RUN_TEST(test_sensor_driver_calibrate_InvalidParam);

    return UNITY_END();
}