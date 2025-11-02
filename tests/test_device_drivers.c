/* test_device_drivers.c – Auto-generated Expert Unity Tests */

#include "unity.h"
#include "device_drivers.h" // Contains struct and enum definitions like uart_driver_t, error_t, DEVICE_STATE_READY, etc.
#include <stdint.h>
#include <stdbool.h>
#include <string.h> // For memset
#include <stdlib.h> // Required for `malloc` and `free` definitions if not stubbed, but we will stub them

// Mock hardware register definitions to allow compilation.
// These are simplified representations of real hardware registers that would typically
// come from an MCU-specific header file (e.g., `stm32f4xx.h`).
typedef struct {
    volatile uint32_t SR; // Status Register (e.g., RXNE, TXE flags)
    volatile uint32_t DR; // Data Register (for RX/TX data)
} USART_TypeDef;

typedef struct {
    volatile uint32_t CR1; // Control Register 1
    volatile uint32_t DR;  // Data Register
} SPI_TypeDef;

typedef struct {
    // No specific members are accessed in the provided code, just its pointer is passed to gpio_write_pin.
    uint33_t placeholder; // Using a distinct type to avoid collisions and indicate mock
    uint32_t placeholder;
} GPIO_TypeDef;

// --- Mock external functions (STUBS) ---

// Malloc and Free stubs to control memory allocation behavior for tests.
// These definitions will override the standard library's malloc/free for the test executable.
// This allows controlling `malloc` failure for 100% branch coverage without linker trickery.

// We need a simple mock memory pool to return unique pointers for successful malloc calls.
// This pool does not track individual block sizes or allow general reallocation/fragmentation.
// It simply provides distinct addresses.
#define MOCK_HEAP_POOL_SIZE 10 // Max 10 mock allocations per test
static void* mock_heap_allocated_pointers[MOCK_HEAP_POOL_SIZE];
static uint8_t mock_heap_next_index;

typedef struct {
    bool was_called;
    uint32_t call_count;
    size_t captured_size;
    bool return_null; // Flag to make malloc return NULL
    bool next_call_will_fail; // Flag for single malloc failure
} stub_malloc_t;
static stub_malloc_t stub_malloc = {0};

void* malloc(size_t size) {
    stub_malloc.was_called = true;
    stub_malloc.call_count++;
    stub_malloc.captured_size = size;

    if (stub_malloc.return_null || stub_malloc.next_call_will_fail) {
        stub_malloc.next_call_will_fail = false; // Reset for next call
        return NULL;
    }

    if (mock_heap_next_index < MOCK_HEAP_POOL_SIZE) {
        // Return a distinct, non-NULL address. The actual content is not important for these tests.
        // The address range should be distinct from actual NULL (0x0) and other static addresses.
        mock_heap_allocated_pointers[mock_heap_next_index] = (void*)(0x10000000 + (mock_heap_next_index * 0x100));
        return mock_heap_allocated_pointers[mock_heap_next_index++];
    }

    // If mock pool is exhausted, subsequent mallocs will return NULL
    return NULL;
}

typedef struct {
    bool was_called;
    uint32_t call_count;
    void* captured_ptr;
} stub_free_t;
static stub_free_t stub_free = {0};

void free(void* ptr) {
    stub_free.was_called = true;
    stub_free.call_count++;
    stub_free.captured_ptr = ptr;
    // For this simple mock malloc, `free` doesn't manage the `mock_heap_allocated_pointers`.
    // We only care if `free` was called with the expected pointer.
}

// 1. uart_init(USART_TypeDef* uart, uart_config_t* config)
typedef struct {
    bool was_called;
    uint32_t call_count;
    USART_TypeDef* captured_uart;
    uart_config_t captured_config; // Store a copy
    error_t return_value;
} stub_uart_init_t;
static stub_uart_init_t stub_uart_init = {0};

error_t uart_init(USART_TypeDef* uart, uart_config_t* config) {
    stub_uart_init.was_called = true;
    stub_uart_init.call_count++;
    stub_uart_init.captured_uart = uart;
    if (config) {
        memcpy(&stub_uart_init.captured_config, config, sizeof(uart_config_t));
    }
    return stub_uart_init.return_value;
}

// 2. uart_transmit(USART_TypeDef* uart, uint8_t* data, uint16_t size, uint32_t timeout_ms)
typedef struct {
    bool was_called;
    uint32_t call_count;
    USART_TypeDef* captured_uart;
    uint8_t captured_data[256]; // Sufficient for typical embedded buffers
    uint16_t captured_size;
    uint32_t captured_timeout_ms;
    error_t return_value;
} stub_uart_transmit_t;
static stub_uart_transmit_t stub_uart_transmit = {0};

error_t uart_transmit(USART_TypeDef* uart, uint8_t* data, uint16_t size, uint32_t timeout_ms) {
    stub_uart_transmit.was_called = true;
    stub_uart_transmit.call_count++;
    stub_uart_transmit.captured_uart = uart;
    stub_uart_transmit.captured_size = size;
    stub_uart_transmit.captured_timeout_ms = timeout_ms;
    if (data && size > 0 && size <= sizeof(stub_uart_transmit.captured_data)) {
        memcpy(stub_uart_transmit.captured_data, data, size);
    }
    return stub_uart_transmit.return_value;
}

// 3. uart_receive(USART_TypeDef* uart, uint8_t* data, uint16_t size, uint32_t timeout_ms)
typedef struct {
    bool was_called;
    uint32_t call_count;
    USART_TypeDef* captured_uart;
    uint8_t* captured_data_buf; // Pointer to output buffer in test (for verifying `data` param)
    uint16_t captured_size;
    uint32_t captured_timeout_ms;
    error_t return_value;
    uint8_t data_to_return[256]; // Data to be copied into 'data' parameter by the stub
} stub_uart_receive_t;
static stub_uart_receive_t stub_uart_receive = {0};

error_t uart_receive(USART_TypeDef* uart, uint8_t* data, uint16_t size, uint32_t timeout_ms) {
    stub_uart_receive.was_called = true;
    stub_uart_receive.call_count++;
    stub_uart_receive.captured_uart = uart;
    stub_uart_receive.captured_data_buf = data;
    stub_uart_receive.captured_size = size;
    stub_uart_receive.captured_timeout_ms = timeout_ms;
    if (data && size > 0 && size <= sizeof(stub_uart_receive.data_to_return)) {
        memcpy(data, stub_uart_receive.data_to_return, size);
    }
    return stub_uart_receive.return_value;
}

// 4. spi_init(SPI_TypeDef* spi, spi_config_t* config)
typedef struct {
    bool was_called;
    uint32_t call_count;
    SPI_TypeDef* captured_spi;
    spi_config_t captured_config;
    error_t return_value;
} stub_spi_init_t;
static stub_spi_init_t stub_spi_init = {0};

error_t spi_init(SPI_TypeDef* spi, spi_config_t* config) {
    stub_spi_init.was_called = true;
    stub_spi_init.call_count++;
    stub_spi_init.captured_spi = spi;
    if (config) {
        memcpy(&stub_spi_init.captured_config, config, sizeof(spi_config_t));
    }
    return stub_spi_init.return_value;
}

// 5. spi_transmit_receive(SPI_TypeDef* spi, uint8_t* tx_data, uint8_t* rx_data, uint16_t size)
typedef struct {
    bool was_called;
    uint32_t call_count;
    SPI_TypeDef* captured_spi;
    uint8_t captured_tx_data[256];
    uint8_t* captured_rx_data_buf; // Pointer to output buffer in test
    uint16_t captured_size;
    error_t return_value;
    uint8_t data_to_return[256]; // Data to be copied into 'rx_data' parameter
} stub_spi_transmit_receive_t;
static stub_spi_transmit_receive_t stub_spi_transmit_receive = {0};

error_t spi_transmit_receive(SPI_TypeDef* spi, uint8_t* tx_data, uint8_t* rx_data, uint16_t size) {
    stub_spi_transmit_receive.was_called = true;
    stub_spi_transmit_receive.call_count++;
    stub_spi_transmit_receive.captured_spi = spi;
    stub_spi_transmit_receive.captured_size = size;
    if (tx_data && size > 0 && size <= sizeof(stub_spi_transmit_receive.captured_tx_data)) {
        memcpy(stub_spi_transmit_receive.captured_tx_data, tx_data, size);
    }
    stub_spi_transmit_receive.captured_rx_data_buf = rx_data;
    if (rx_data && size > 0 && size <= sizeof(stub_spi_transmit_receive.data_to_return)) {
        memcpy(rx_data, stub_spi_transmit_receive.data_to_return, size);
    }
    return stub_spi_transmit_receive.return_value;
}

// 6. gpio_write_pin(GPIO_TypeDef* gpio, uint16_t pin, bool state)
typedef struct {
    bool was_called;
    uint32_t call_count;
    GPIO_TypeDef* captured_gpio;
    uint16_t captured_pin;
    bool captured_state;
} stub_gpio_write_pin_t;
static stub_gpio_write_pin_t stub_gpio_write_pin = {0};

void gpio_write_pin(GPIO_TypeDef* gpio, uint16_t pin, bool state) {
    stub_gpio_write_pin.was_called = true;
    stub_gpio_write_pin.call_count++;
    stub_gpio_write_pin.captured_gpio = gpio;
    stub_gpio_write_pin.captured_pin = pin;
    stub_gpio_write_pin.captured_state = state;
}

// 7. can_init(can_handle_t* handle, uint32_t buffer_size)
typedef struct {
    bool was_called;
    uint32_t call_count;
    can_handle_t* captured_handle;
    uint32_t captured_buffer_size;
    protocol_error_t return_value;
} stub_can_init_t;
static stub_can_init_t stub_can_init = {0};

protocol_error_t can_init(can_handle_t* handle, uint32_t buffer_size) {
    stub_can_init.was_called = true;
    stub_can_init.call_count++;
    stub_can_init.captured_handle = handle;
    stub_can_init.captured_buffer_size = buffer_size;
    return stub_can_init.return_value;
}

// 8. can_transmit_message(can_handle_t* handle, const can_frame_t* frame, uint32_t timeout_ms)
typedef struct {
    bool was_called;
    uint32_t call_count;
    can_handle_t* captured_handle;
    can_frame_t captured_frame; // Store a copy
    uint32_t captured_timeout_ms;
    protocol_error_t return_value;
} stub_can_transmit_message_t;
static stub_can_transmit_message_t stub_can_transmit_message = {0};

protocol_error_t can_transmit_message(can_handle_t* handle, const can_frame_t* frame, uint32_t timeout_ms) {
    stub_can_transmit_message.was_called = true;
    stub_can_transmit_message.call_count++;
    stub_can_transmit_message.captured_handle = handle;
    stub_can_transmit_message.captured_timeout_ms = timeout_ms;
    if (frame) {
        memcpy(&stub_can_transmit_message.captured_frame, frame, sizeof(can_frame_t));
    }
    return stub_can_transmit_message.return_value;
}

// --- Test State and Setup/Teardown ---
static uart_driver_t test_uart_driver;
static uart_config_t test_uart_config;
static spi_driver_t test_spi_driver;
static spi_config_t test_spi_config;
static i2c_driver_t test_i2c_driver;
static can_driver_t test_can_driver;
static sensor_driver_t test_sensor_driver;

// Mock callback contexts and flags
static bool mock_state_change_cb_called = false;
static device_state_t mock_old_state;
static device_state_t mock_new_state;
static void* mock_cb_context;

static void mock_state_change_callback(device_state_t old_state, device_state_t new_state, void* context) {
    mock_state_change_cb_called = true;
    mock_old_state = old_state;
    mock_new_state = new_state;
    mock_cb_context = context;
}

static bool mock_data_ready_cb_called = false;
static float mock_read_value;
static void* mock_data_cb_context;

static void mock_data_ready_callback(float value, void* context) {
    mock_data_ready_cb_called = true;
    mock_read_value = value;
    mock_data_cb_context = context;
}

void setUp(void) {
    // Reset all stub control structs
    memset(&stub_malloc, 0, sizeof(stub_malloc));
    memset(&stub_free, 0, sizeof(stub_free));
    memset(&stub_uart_init, 0, sizeof(stub_uart_init));
    memset(&stub_uart_transmit, 0, sizeof(stub_uart_transmit));
    memset(&stub_uart_receive, 0, sizeof(stub_uart_receive));
    memset(&stub_spi_init, 0, sizeof(stub_spi_init));
    memset(&stub_spi_transmit_receive, 0, sizeof(stub_spi_transmit_receive));
    memset(&stub_gpio_write_pin, 0, sizeof(stub_gpio_write_pin));
    memset(&stub_can_init, 0, sizeof(stub_can_init));
    memset(&stub_can_transmit_message, 0, sizeof(stub_can_transmit_message));

    // Default return values for stubs
    stub_uart_init.return_value = ERROR_NONE;
    stub_uart_transmit.return_value = ERROR_NONE;
    stub_uart_receive.return_value = ERROR_NONE;
    stub_spi_init.return_value = ERROR_NONE;
    stub_spi_transmit_receive.return_value = ERROR_NONE;
    stub_can_init.return_value = PROTOCOL_ERROR_NONE;
    stub_can_transmit_message.return_value = PROTOCOL_ERROR_NONE;

    // Reset mock memory heap
    memset(&mock_heap_allocated_pointers, 0, sizeof(mock_heap_allocated_pointers));
    mock_heap_next_index = 0;

    // Reset mock callback flags
    mock_state_change_cb_called = false;
    mock_old_state = (device_state_t)0;
    mock_new_state = (device_state_t)0;
    mock_cb_context = NULL;
    mock_data_ready_cb_called = false;
    mock_read_value = 0.0f;
    mock_data_cb_context = NULL;

    // Initialize test driver structs to known states (memset to 0 for consistency)
    memset(&test_uart_driver, 0, sizeof(uart_driver_t));
    memset(&test_uart_config, 0, sizeof(uart_config_t));
    memset(&test_spi_driver, 0, sizeof(spi_driver_t));
    memset(&test_spi_config, 0, sizeof(spi_config_t));
    memset(&test_i2c_driver, 0, sizeof(i2c_driver_t));
    memset(&test_can_driver, 0, sizeof(can_driver_t));
    memset(&test_sensor_driver, 0, sizeof(sensor_driver_t));
}

void tearDown(void) {
    // Explicitly free any dynamically allocated memory within the driver structs
    // that might persist after a test, especially for `malloc` returning real pointers
    // or if `free` calls are being tracked. For our mock `malloc` which doesn't
    // manage a real heap, `free` doesn't do much, but the principle is important.
    if (test_uart_driver.uart) {
        free(test_uart_driver.uart);
        test_uart_driver.uart = NULL;
    }
    if (test_i2c_driver.i2c_regs) {
        free(test_i2c_driver.i2c_regs);
        test_i2c_driver.i2c_regs = NULL;
    }
    if (test_can_driver.can) {
        free(test_can_driver.can);
        test_can_driver.can = NULL;
    }
    if (test_spi_driver.spi) {
        free(test_spi_driver.spi);
        test_spi_driver.spi = NULL;
    }
    if (test_sensor_driver.i2c) {
        // Since i2c_driver_init also mallocs i2c_regs, we need to free that too.
        if (test_sensor_driver.i2c->i2c_regs) {
            free(test_sensor_driver.i2c->i2c_regs);
            test_sensor_driver.i2c->i2c_regs = NULL;
        }
        free(test_sensor_driver.i2c);
        test_sensor_driver.i2c = NULL;
    }
    if (test_sensor_driver.spi) {
        free(test_sensor_driver.spi);
        test_sensor_driver.spi = NULL;
    }
    if (test_sensor_driver.uart) {
        free(test_sensor_driver.uart);
        test_sensor_driver.uart = NULL;
    }

    // Full stub reset again, just in case a test modified state that wasn't covered in setUp
    memset(&stub_malloc, 0, sizeof(stub_malloc));
    memset(&stub_free, 0, sizeof(stub_free));
    memset(&stub_uart_init, 0, sizeof(stub_uart_init));
    memset(&stub_uart_transmit, 0, sizeof(stub_uart_transmit));
    memset(&stub_uart_receive, 0, sizeof(stub_uart_receive));
    memset(&stub_spi_init, 0, sizeof(stub_spi_init));
    memset(&stub_spi_transmit_receive, 0, sizeof(stub_spi_transmit_receive));
    memset(&stub_gpio_write_pin, 0, sizeof(stub_gpio_write_pin));
    memset(&stub_can_init, 0, sizeof(stub_can_init));
    memset(&stub_can_transmit_message, 0, sizeof(stub_can_transmit_message));

    // Reset mock memory heap
    memset(&mock_heap_allocated_pointers, 0, sizeof(mock_heap_allocated_pointers));
    mock_heap_next_index = 0;

    mock_state_change_cb_called = false;
    mock_old_state = (device_state_t)0;
    mock_new_state = (device_state_t)0;
    mock_cb_context = NULL;
    mock_data_ready_cb_called = false;
    mock_read_value = 0.0f;
    mock_data_cb_context = NULL;
}

// --- UART Driver Tests ---

void test_uart_driver_init_null_driver_param(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    error_t result = uart_driver_init(NULL, &test_uart_config);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_uart_init.was_called); // Should not call external init
    TEST_ASSERT_FALSE(stub_malloc.was_called); // Should not try to allocate
}

void test_uart_driver_init_null_config_param(void) {
    // Test: config is NULL
    // Expected: ERROR_INVALID_PARAM
    error_t result = uart_driver_init(&test_uart_driver, NULL);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_uart_init.was_called); // Should not call external init
    TEST_ASSERT_FALSE(stub_malloc.was_called); // Should not try to allocate
}

void test_uart_driver_init_successful(void) {
    // Test: Successful initialization
    // Expected: ERROR_NONE, driver state READY, uart_init called with correct parameters
    test_uart_config.baud_rate = 115200;
    test_uart_config.data_bits = 8;
    test_uart_driver.state_change_cb = mock_state_change_callback;
    test_uart_driver.callback_context = (void*)0xDEADBEEF;

    error_t result = uart_driver_init(&test_uart_driver, &test_uart_config);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state);
    TEST_ASSERT_TRUE(stub_malloc.was_called); // Expected mock allocation
    TEST_ASSERT_NOT_NULL(test_uart_driver.uart); // Expected to be allocated
    TEST_ASSERT_EQUAL_PTR(mock_heap_allocated_pointers[0], test_uart_driver.uart); // First allocated mock ptr
    TEST_ASSERT_EQUAL(1000, test_uart_driver.timeout_ms); // Expected default timeout
    TEST_ASSERT_EQUAL(0, test_uart_driver.error_index); // Expected error index reset
    TEST_ASSERT_TRUE(stub_uart_init.was_called); // Expected external init call
    TEST_ASSERT_EQUAL(1, stub_uart_init.call_count);
    TEST_ASSERT_EQUAL_PTR(test_uart_driver.uart, stub_uart_init.captured_uart); // Expected captured UART instance
    TEST_ASSERT_EQUAL_MEMORY(&test_uart_config, &stub_uart_init.captured_config, sizeof(uart_config_t)); // Expected captured config
    TEST_ASSERT_TRUE(mock_state_change_cb_called); // Expected state change callback
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, mock_old_state);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, mock_new_state);
    TEST_ASSERT_EQUAL_PTR((void*)0xDEADBEEF, mock_cb_context);
}

void test_uart_driver_init_malloc_fails(void) {
    // Test: malloc for UART hardware registers fails
    // Expected: ERROR_BUSY, driver state INIT, no uart_init call, no memory allocated
    stub_malloc.return_null = true; // Force malloc to return NULL
    test_uart_driver.state_change_cb = mock_state_change_callback; // Setup CB to check it's not called

    error_t result = uart_driver_init(&test_uart_driver, &test_uart_config);
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, test_uart_driver.state); // State should remain INIT or set to ERROR based on specific driver. Here, it remains INIT
    TEST_ASSERT_NULL(test_uart_driver.uart); // Expected no allocation
    TEST_ASSERT_TRUE(stub_malloc.was_called);
    TEST_ASSERT_FALSE(stub_uart_init.was_called); // Should not call external init
    TEST_ASSERT_FALSE(mock_state_change_cb_called); // Callback should not be called
}

void test_uart_driver_init_uart_init_fails(void) {
    // Test: uart_init external function returns an error
    // Expected: Driver state ERROR, allocated memory freed, error propagated
    stub_uart_init.return_value = ERROR_COMMUNICATION;
    test_uart_driver.state_change_cb = mock_state_change_callback;

    error_t result = uart_driver_init(&test_uart_driver, &test_uart_config);
    TEST_ASSERT_EQUAL(ERROR_COMMUNICATION, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_uart_driver.state); // Expected state to be ERROR
    TEST_ASSERT_NULL(test_uart_driver.uart); // Expected allocated memory to be freed
    TEST_ASSERT_TRUE(stub_malloc.was_called); // Malloc should have been called
    TEST_ASSERT_TRUE(stub_free.was_called); // Free should have been called
    TEST_ASSERT_EQUAL_PTR(mock_heap_allocated_pointers[0], stub_free.captured_ptr); // Free should be for the allocated UART instance
    TEST_ASSERT_TRUE(stub_uart_init.was_called); // Expected external init call
    TEST_ASSERT_FALSE(mock_state_change_cb_called); // Callback should not be called on init error
}

void test_uart_driver_transmit_null_driver(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    uint8_t data[] = {1, 2, 3};
    error_t result = uart_driver_transmit(NULL, data, 3);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_uart_transmit.was_called);
}

void test_uart_driver_transmit_null_data(void) {
    // Test: data is NULL
    // Expected: ERROR_INVALID_PARAM
    test_uart_driver.state = DEVICE_STATE_READY;
    error_t result = uart_driver_transmit(&test_uart_driver, NULL, 3);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_uart_transmit.was_called);
}

void test_uart_driver_transmit_zero_size(void) {
    // Test: size is 0
    // Expected: ERROR_INVALID_PARAM
    uint8_t data[] = {1, 2, 3};
    test_uart_driver.state = DEVICE_STATE_READY;
    error_t result = uart_driver_transmit(&test_uart_driver, data, 0);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_uart_transmit.was_called);
}

void test_uart_driver_transmit_not_ready(void) {
    // Test: driver state is not READY
    // Expected: ERROR_INVALID_PARAM
    uint8_t data[] = {1, 2, 3};
    test_uart_driver.state = DEVICE_STATE_INIT; // Not ready
    error_t result = uart_driver_transmit(&test_uart_driver, data, 3);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_uart_transmit.was_called);
    test_uart_driver.state = DEVICE_STATE_BUSY; // Not ready
    result = uart_driver_transmit(&test_uart_driver, data, 3);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_EQUAL(0, stub_uart_transmit.call_count); // Ensure stub not called in previous test state
    test_uart_driver.state = DEVICE_STATE_ERROR; // Not ready
    result = uart_driver_transmit(&test_uart_driver, data, 3);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_uart_transmit.was_called);
}

void test_uart_driver_transmit_successful(void) {
    // Test: Successful transmission
    // Expected: ERROR_NONE, state BUSY then READY, uart_transmit called
    uart_driver_init(&test_uart_driver, &test_uart_config); // Initialize first
    test_uart_driver.state = DEVICE_STATE_READY;
    uint8_t data[] = {0xAA, 0xBB, 0xCC};
    uint16_t size = 3;

    error_t result = uart_driver_transmit(&test_uart_driver, data, size);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_TRUE(stub_uart_transmit.was_called);
    TEST_ASSERT_EQUAL(1, stub_uart_transmit.call_count);
    TEST_ASSERT_EQUAL_PTR(test_uart_driver.uart, stub_uart_transmit.captured_uart);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(data, stub_uart_transmit.captured_data, size);
    TEST_ASSERT_EQUAL(size, stub_uart_transmit.captured_size);
    TEST_ASSERT_EQUAL(test_uart_driver.timeout_ms, stub_uart_transmit.captured_timeout_ms);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state); // State should return to READY
}

void test_uart_driver_transmit_fails(void) {
    // Test: uart_transmit external function returns an error
    // Expected: Error code propagated, driver state ERROR, error logged
    uart_driver_init(&test_uart_driver, &test_uart_config); // Initialize first
    test_uart_driver.state = DEVICE_STATE_READY;
    uint8_t data[] = {0xAA, 0xBB, 0xCC};
    uint16_t size = 3;
    stub_uart_transmit.return_value = ERROR_TIMEOUT;

    error_t result = uart_driver_transmit(&test_uart_driver, data, size);
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, result);
    TEST_ASSERT_TRUE(stub_uart_transmit.was_called);
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_uart_driver.state); // Expected state to be ERROR
    TEST_ASSERT_EQUAL(1, test_uart_driver.error_index); // Expected error logged at index 0
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, test_uart_driver.errors[0].error_code);
}

void test_uart_driver_transmit_error_history_wrap(void) {
    // Test: Error history wraps around
    // Expected: error_index correctly wraps and oldest error is overwritten
    uart_driver_init(&test_uart_driver, &test_uart_config); // Initialize fresh driver
    test_uart_driver.state = DEVICE_STATE_READY;
    uint8_t data[] = {1};
    
    // Fill error history up to ERROR_HISTORY_SIZE, ensure stub is called and state changes
    for (int i = 0; i < ERROR_HISTORY_SIZE; i++) {
        stub_uart_transmit.return_value = (error_t)(ERROR_TIMEOUT + i); // Unique error codes for tracking
        stub_uart_transmit.call_count = 0; // Reset call count for each transmit attempt
        error_t res = uart_driver_transmit(&test_uart_driver, data, 1);
        TEST_ASSERT_EQUAL((error_t)(ERROR_TIMEOUT + i), res);
        TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_uart_driver.state);
        TEST_ASSERT_EQUAL(1, stub_uart_transmit.call_count);
        TEST_ASSERT_EQUAL((i + 1) % ERROR_HISTORY_SIZE, test_uart_driver.error_index); // Index updates
        TEST_ASSERT_EQUAL((error_t)(ERROR_TIMEOUT + i), test_uart_driver.errors[i].error_code);
        test_uart_driver.state = DEVICE_STATE_READY; // Reset state for next iteration
    }

    // Now trigger one more error to test wrap-around, overwriting the first error
    stub_uart_transmit.return_value = ERROR_OTHER;
    stub_uart_transmit.call_count = 0;
    error_t res = uart_driver_transmit(&test_uart_driver, data, 1);
    TEST_ASSERT_EQUAL(ERROR_OTHER, res);
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_uart_driver.state);
    TEST_ASSERT_EQUAL(1, stub_uart_transmit.call_count);
    TEST_ASSERT_EQUAL(1, test_uart_driver.error_index); // Should wrap to 1 (index 0 overwritten)
    TEST_ASSERT_EQUAL(ERROR_OTHER, test_uart_driver.errors[0].error_code); // New error at index 0
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT + 1, test_uart_driver.errors[1].error_code); // Original error at index 1 still present
}


void test_uart_driver_receive_null_driver(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    uint8_t data[3];
    error_t result = uart_driver_receive(NULL, data, 3);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_uart_receive.was_called);
}

void test_uart_driver_receive_null_data(void) {
    // Test: data is NULL
    // Expected: ERROR_INVALID_PARAM
    test_uart_driver.state = DEVICE_STATE_READY;
    error_t result = uart_driver_receive(&test_uart_driver, NULL, 3);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_uart_receive.was_called);
}

void test_uart_driver_receive_zero_size(void) {
    // Test: size is 0
    // Expected: ERROR_INVALID_PARAM
    uint8_t data[3];
    test_uart_driver.state = DEVICE_STATE_READY;
    error_t result = uart_driver_receive(&test_uart_driver, data, 0);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_uart_receive.was_called);
}

void test_uart_driver_receive_not_ready(void) {
    // Test: driver state is not READY
    // Expected: ERROR_INVALID_PARAM
    uint8_t data[3];
    test_uart_driver.state = DEVICE_STATE_INIT; // Not ready
    error_t result = uart_driver_receive(&test_uart_driver, data, 3);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_uart_receive.was_called);
}

void test_uart_driver_receive_successful(void) {
    // Test: Successful reception
    // Expected: ERROR_NONE, state BUSY then READY, uart_receive called, data filled
    uart_driver_init(&test_uart_driver, &test_uart_config);
    test_uart_driver.state = DEVICE_STATE_READY;
    uint8_t rx_data[3] = {0};
    uint8_t expected_data[] = {0xDE, 0xAD, 0xBE};
    uint16_t size = 3;
    memcpy(stub_uart_receive.data_to_return, expected_data, size); // Data for stub to return

    error_t result = uart_driver_receive(&test_uart_driver, rx_data, size);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_TRUE(stub_uart_receive.was_called);
    TEST_ASSERT_EQUAL(1, stub_uart_receive.call_count);
    TEST_ASSERT_EQUAL_PTR(test_uart_driver.uart, stub_uart_receive.captured_uart);
    TEST_ASSERT_EQUAL_PTR(rx_data, stub_uart_receive.captured_data_buf); // Verify buffer pointer
    TEST_ASSERT_EQUAL(size, stub_uart_receive.captured_size);
    TEST_ASSERT_EQUAL(test_uart_driver.timeout_ms, stub_uart_receive.captured_timeout_ms);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data, rx_data, size); // Verify received data
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state);
}

void test_uart_driver_receive_fails(void) {
    // Test: uart_receive external function returns an error
    // Expected: Error code propagated, driver state ERROR, error logged
    uart_driver_init(&test_uart_driver, &test_uart_config);
    test_uart_driver.state = DEVICE_STATE_READY;
    uint8_t rx_data[3] = {0};
    uint16_t size = 3;
    stub_uart_receive.return_value = ERROR_TIMEOUT;

    error_t result = uart_driver_receive(&test_uart_driver, rx_data, size);
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, result);
    TEST_ASSERT_TRUE(stub_uart_receive.was_called);
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_uart_driver.state);
    TEST_ASSERT_EQUAL(1, test_uart_driver.error_index);
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, test_uart_driver.errors[0].error_code);
}

void test_uart_driver_process_interrupt_null_driver(void) {
    // Test: driver is NULL
    // Expected: No crash, simply returns
    uart_driver_process_interrupt(NULL);
    // No assertions possible beyond no crash.
    // This implicitly tests the `if (driver == NULL) return;` branch.
}

void test_uart_driver_process_interrupt_rxne(void) {
    // Test: RXNE flag set, simulating RX interrupt
    // Expected: DR read, state remains.
    uart_driver_init(&test_uart_driver, &test_uart_config);
    test_uart_driver.uart->SR = USART_SR_RXNE; // Simulate RXNE
    test_uart_driver.uart->DR = 0x55; // Simulate received data

    // The function reads DR but doesn't store it or change state, so we just check no crash.
    uart_driver_process_interrupt(&test_uart_driver);
    TEST_ASSERT_EQUAL(USART_SR_RXNE, test_uart_driver.uart->SR); // SR should not be cleared by driver
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state); // State should not change
    // Further assertions would depend on internal RX buffer logic, which is not exposed.
}

void test_uart_driver_process_interrupt_txe(void) {
    // Test: TXE flag set, simulating TX interrupt
    // Expected: State remains.
    uart_driver_init(&test_uart_driver, &test_uart_config);
    test_uart_driver.uart->SR = USART_SR_TXE; // Simulate TXE

    uart_driver_process_interrupt(&test_uart_driver);
    TEST_ASSERT_EQUAL(USART_SR_TXE, test_uart_driver.uart->SR); // SR should not be cleared by driver
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state); // State should not change
    // Further assertions would depend on internal TX buffer logic, which is not exposed.
}

void test_uart_driver_process_interrupt_both_flags(void) {
    // Test: Both RXNE and TXE flags set
    // Expected: Both branches executed, no crash.
    uart_driver_init(&test_uart_driver, &test_uart_config);
    test_uart_driver.uart->SR = USART_SR_RXNE | USART_SR_TXE; // Simulate both
    test_uart_driver.uart->DR = 0xAA;

    uart_driver_process_interrupt(&test_uart_driver);
    TEST_ASSERT_EQUAL(USART_SR_RXNE | USART_SR_TXE, test_uart_driver.uart->SR);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state);
}

void test_uart_driver_process_interrupt_no_flags(void) {
    // Test: No flags set
    // Expected: No actions taken, no crash.
    uart_driver_init(&test_uart_driver, &test_uart_config);
    test_uart_driver.uart->SR = 0; // No flags

    uart_driver_process_interrupt(&test_uart_driver);
    TEST_ASSERT_EQUAL(0, test_uart_driver.uart->SR);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_uart_driver.state);
}

// --- SPI Driver Tests ---

void test_spi_driver_init_null_driver(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    error_t result = spi_driver_init(NULL, &test_spi_config);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_spi_init.was_called);
    TEST_ASSERT_FALSE(stub_malloc.was_called);
}

void test_spi_driver_init_null_config(void) {
    // Test: config is NULL
    // Expected: ERROR_INVALID_PARAM
    error_t result = spi_driver_init(&test_spi_driver, NULL);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_spi_init.was_called);
    TEST_ASSERT_FALSE(stub_malloc.was_called);
}

void test_spi_driver_init_successful(void) {
    // Test: Successful initialization
    // Expected: ERROR_NONE, state READY, spi_init called
    test_spi_config.speed_hz = 1000000;
    test_spi_config.mode = 0;

    error_t result = spi_driver_init(&test_spi_driver, &test_spi_config);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_spi_driver.state);
    TEST_ASSERT_TRUE(stub_malloc.was_called); // Malloc should be called
    TEST_ASSERT_NOT_NULL(test_spi_driver.spi); // Expected to be allocated
    TEST_ASSERT_EQUAL_PTR(mock_heap_allocated_pointers[0], test_spi_driver.spi); // First allocated mock ptr
    TEST_ASSERT_EQUAL(0, test_spi_driver.error_index);
    TEST_ASSERT_TRUE(stub_spi_init.was_called);
    TEST_ASSERT_EQUAL(1, stub_spi_init.call_count);
    TEST_ASSERT_EQUAL_PTR(test_spi_driver.spi, stub_spi_init.captured_spi);
    TEST_ASSERT_EQUAL_MEMORY(&test_spi_config, &stub_spi_init.captured_config, sizeof(spi_config_t));
}

void test_spi_driver_init_malloc_fails(void) {
    // Test: malloc for SPI hardware registers fails
    // Expected: ERROR_BUSY, driver state INIT, no spi_init call, no memory allocated
    stub_malloc.return_null = true; // Force malloc to return NULL
    error_t result = spi_driver_init(&test_spi_driver, &test_spi_config);
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, test_spi_driver.state); // State should remain INIT or be set to ERROR
    TEST_ASSERT_NULL(test_spi_driver.spi); // Expected no allocation
    TEST_ASSERT_TRUE(stub_malloc.was_called);
    TEST_ASSERT_FALSE(stub_spi_init.was_called); // Should not call external init
}

void test_spi_driver_init_spi_init_fails(void) {
    // Test: spi_init external function returns an error
    // Expected: Driver state ERROR, allocated memory freed, error propagated
    stub_spi_init.return_value = ERROR_BUSY;

    error_t result = spi_driver_init(&test_spi_driver, &test_spi_config);
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_spi_driver.state);
    TEST_ASSERT_NULL(test_spi_driver.spi); // Expected allocated memory to be freed
    TEST_ASSERT_TRUE(stub_malloc.was_called); // Malloc should have been called
    TEST_ASSERT_TRUE(stub_free.was_called); // Free should have been called
    TEST_ASSERT_EQUAL_PTR(mock_heap_allocated_pointers[0], stub_free.captured_ptr); // Free should be for the allocated SPI instance
    TEST_ASSERT_TRUE(stub_spi_init.was_called);
}

void test_spi_driver_transfer_null_driver(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    uint8_t tx[3] = {0};
    uint8_t rx[3] = {0};
    error_t result = spi_driver_transfer(NULL, tx, rx, 3);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_spi_transmit_receive.was_called);
}

void test_spi_driver_transfer_null_tx_data(void) {
    // Test: tx_data is NULL
    // Expected: ERROR_INVALID_PARAM
    test_spi_driver.state = DEVICE_STATE_READY;
    uint8_t rx[3] = {0};
    error_t result = spi_driver_transfer(&test_spi_driver, NULL, rx, 3);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_spi_transmit_receive.was_called);
}

void test_spi_driver_transfer_null_rx_data(void) {
    // Test: rx_data is NULL
    // Expected: ERROR_INVALID_PARAM
    test_spi_driver.state = DEVICE_STATE_READY;
    uint8_t tx[3] = {0};
    error_t result = spi_driver_transfer(&test_spi_driver, tx, NULL, 3);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_spi_transmit_receive.was_called);
}

void test_spi_driver_transfer_not_ready(void) {
    // Test: driver state is not READY
    // Expected: ERROR_INVALID_PARAM
    test_spi_driver.state = DEVICE_STATE_INIT; // Not ready
    uint8_t tx[3] = {0};
    uint8_t rx[3] = {0};
    error_t result = spi_driver_transfer(&test_spi_driver, tx, rx, 3);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_spi_transmit_receive.was_called);
}

void test_spi_driver_transfer_successful(void) {
    // Test: Successful transfer
    // Expected: ERROR_NONE, state BUSY then READY, spi_transmit_receive called, CS toggled
    spi_driver_init(&test_spi_driver, &test_spi_config); // Initialize first
    test_spi_driver.state = DEVICE_STATE_READY;
    test_spi_driver.cs_gpio = (GPIO_TypeDef*)0x123;
    test_spi_driver.cs_pin = 5;

    uint8_t tx_data[] = {0x11, 0x22, 0x33};
    uint8_t rx_data[3] = {0};
    uint8_t expected_rx_data[] = {0xA1, 0xB2, 0xC3};
    uint16_t size = 3;
    memcpy(stub_spi_transmit_receive.data_to_return, expected_rx_data, size);

    error_t result = spi_driver_transfer(&test_spi_driver, tx_data, rx_data, size);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_TRUE(stub_gpio_write_pin.was_called);
    TEST_ASSERT_EQUAL(2, stub_gpio_write_pin.call_count); // CS low then high

    // Assertions for CS low (first call) and CS high (second call) are tricky with a simple stub.
    // The stub captures the *last* call. The prompt example `last_param` implies this.
    // We can infer the sequence from the call_count and the final captured state.
    TEST_ASSERT_TRUE(stub_spi_transmit_receive.was_called);
    TEST_ASSERT_EQUAL(1, stub_spi_transmit_receive.call_count);
    TEST_ASSERT_EQUAL_PTR(test_spi_driver.spi, stub_spi_transmit_receive.captured_spi);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(tx_data, stub_spi_transmit_receive.captured_tx_data, size);
    TEST_ASSERT_EQUAL_PTR(rx_data, stub_spi_transmit_receive.captured_rx_data_buf);
    TEST_ASSERT_EQUAL(size, stub_spi_transmit_receive.captured_size);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_rx_data, rx_data, size);

    TEST_ASSERT_EQUAL(test_spi_driver.cs_pin, stub_gpio_write_pin.captured_pin); // Pin always same
    TEST_ASSERT_TRUE(stub_gpio_write_pin.captured_state); // Last call should be CS high
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_spi_driver.state);
}

void test_spi_driver_transfer_fails(void) {
    // Test: spi_transmit_receive external function returns an error
    // Expected: Error code propagated, driver state ERROR, error logged, CS still toggled
    spi_driver_init(&test_spi_driver, &test_spi_config);
    test_spi_driver.state = DEVICE_STATE_READY;
    test_spi_driver.cs_gpio = (GPIO_TypeDef*)0x123;
    test_spi_driver.cs_pin = 5;
    uint8_t tx_data[] = {0x11};
    uint8_t rx_data[1] = {0};
    uint16_t size = 1;
    stub_spi_transmit_receive.return_value = ERROR_TIMEOUT;

    error_t result = spi_driver_transfer(&test_spi_driver, tx_data, rx_data, size);
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, result);
    TEST_ASSERT_TRUE(stub_gpio_write_pin.was_called);
    TEST_ASSERT_EQUAL(2, stub_gpio_write_pin.call_count); // CS low then high
    TEST_ASSERT_TRUE(stub_gpio_write_pin.captured_state); // Last call should be CS high
    TEST_ASSERT_TRUE(stub_spi_transmit_receive.was_called);
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_spi_driver.state);
    TEST_ASSERT_EQUAL(1, test_spi_driver.error_index);
    TEST_ASSERT_EQUAL(ERROR_TIMEOUT, test_spi_driver.errors[0].error_code);
}

// --- I2C Driver Tests ---

void test_i2c_driver_init_null_driver(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    error_t result = i2c_driver_init(NULL, 0x50, 400000);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_malloc.was_called); // No attempt to malloc
}

void test_i2c_driver_init_successful(void) {
    // Test: Successful initialization
    // Expected: ERROR_NONE, state READY, members initialized
    uint8_t address = 0x50;
    uint32_t frequency = 400000;
    error_t result = i2c_driver_init(&test_i2c_driver, address, frequency);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_i2c_driver.state);
    TEST_ASSERT_TRUE(stub_malloc.was_called); // Expected mock allocation for i2c_regs
    TEST_ASSERT_NOT_NULL(test_i2c_driver.i2c_regs); // Expected to be allocated
    TEST_ASSERT_EQUAL_PTR(mock_heap_allocated_pointers[0], test_i2c_driver.i2c_regs); // First allocated mock ptr
    TEST_ASSERT_EQUAL(address, test_i2c_driver.device_addr);
    TEST_ASSERT_EQUAL(100, test_i2c_driver.timeout_ms); // Expected default timeout
    TEST_ASSERT_EQUAL(0, test_i2c_driver.error_index);
}

void test_i2c_driver_init_malloc_fails(void) {
    // Test: malloc for I2C mock registers fails
    // Expected: ERROR_BUSY, driver state INIT, no memory allocated
    stub_malloc.return_null = true; // Force malloc to return NULL
    error_t result = i2c_driver_init(&test_i2c_driver, 0x50, 400000);
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, test_i2c_driver.state); // State should remain INIT
    TEST_ASSERT_NULL(test_i2c_driver.i2c_regs); // Expected no allocation
    TEST_ASSERT_TRUE(stub_malloc.was_called);
}

void test_i2c_driver_write_null_driver(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    uint8_t data[] = {1, 2};
    error_t result = i2c_driver_write(NULL, 0x01, data, 2);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
}

void test_i2c_driver_write_null_data(void) {
    // Test: data is NULL
    // Expected: ERROR_INVALID_PARAM
    test_i2c_driver.state = DEVICE_STATE_READY;
    error_t result = i2c_driver_write(&test_i2c_driver, 0x01, NULL, 2);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
}

void test_i2c_driver_write_not_ready(void) {
    // Test: driver state is not READY
    // Expected: ERROR_INVALID_PARAM
    test_i2c_driver.state = DEVICE_STATE_INIT; // Not ready
    uint8_t data[] = {1, 2};
    error_t result = i2c_driver_write(&test_i2c_driver, 0x01, data, 2);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
}

void test_i2c_driver_write_successful(void) {
    // Test: Successful write (mocked)
    // Expected: ERROR_NONE, state BUSY then READY
    i2c_driver_init(&test_i2c_driver, 0x50, 400000); // Initialize first
    test_i2c_driver.state = DEVICE_STATE_READY;
    uint8_t data[] = {0x01, 0x02, 0x03};
    uint8_t reg = 0x01;
    uint16_t size = 3;

    error_t result = i2c_driver_write(&test_i2c_driver, reg, data, size);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_i2c_driver.state);
    // As it's a mock internal operation, no external calls to assert.
}

void test_i2c_driver_read_null_driver(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    uint8_t data[2] = {0};
    error_t result = i2c_driver_read(NULL, 0x00, data, 2);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
}

void test_i2c_driver_read_null_data(void) {
    // Test: data is NULL
    // Expected: ERROR_INVALID_PARAM
    test_i2c_driver.state = DEVICE_STATE_READY;
    error_t result = i2c_driver_read(&test_i2c_driver, 0x00, NULL, 2);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
}

void test_i2c_driver_read_not_ready(void) {
    // Test: driver state is not READY
    // Expected: ERROR_INVALID_PARAM
    test_i2c_driver.state = DEVICE_STATE_INIT; // Not ready
    uint8_t data[2] = {0};
    error_t result = i2c_driver_read(&test_i2c_driver, 0x00, data, 2);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
}

void test_i2c_driver_read_successful(void) {
    // Test: Successful read (mocked)
    // Expected: ERROR_NONE, state BUSY then READY, data filled with 0xAA
    i2c_driver_init(&test_i2c_driver, 0x50, 400000); // Initialize first
    test_i2c_driver.state = DEVICE_STATE_READY;
    uint8_t rx_data[5] = {0};
    uint8_t reg = 0x00;
    uint16_t size = 5;
    uint8_t expected_data[5];
    memset(expected_data, 0xAA, size); // The source function mocks data with 0xAA

    error_t result = i2c_driver_read(&test_i2c_driver, reg, rx_data, size);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_i2c_driver.state);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data, rx_data, size); // Verify mock data is written
}

// --- CAN Driver Tests ---

void test_can_driver_init_null_driver(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    error_t result = can_driver_init(NULL, 500000);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_can_init.was_called);
    TEST_ASSERT_FALSE(stub_malloc.was_called);
}

void test_can_driver_init_successful(void) {
    // Test: Successful initialization
    // Expected: ERROR_NONE, state READY, can_init called
    uint32_t bitrate = 500000;
    error_t result = can_driver_init(&test_can_driver, bitrate);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_can_driver.state);
    TEST_ASSERT_TRUE(stub_malloc.was_called); // Malloc should be called
    TEST_ASSERT_NOT_NULL(test_can_driver.can);
    TEST_ASSERT_EQUAL_PTR(mock_heap_allocated_pointers[0], test_can_driver.can); // First allocated mock ptr
    TEST_ASSERT_EQUAL(bitrate, test_can_driver.bitrate);
    TEST_ASSERT_EQUAL(0, test_can_driver.error_index);
    TEST_ASSERT_TRUE(stub_can_init.was_called);
    TEST_ASSERT_EQUAL(1, stub_can_init.call_count);
    TEST_ASSERT_EQUAL_PTR(test_can_driver.can, stub_can_init.captured_handle);
    TEST_ASSERT_EQUAL(32, stub_can_init.captured_buffer_size); // Magic number from source
}

void test_can_driver_init_malloc_fails(void) {
    // Test: malloc for CAN handle fails
    // Expected: ERROR_BUSY, driver state INIT, no can_init call, no memory allocated
    stub_malloc.return_null = true; // Force malloc to return NULL
    error_t result = can_driver_init(&test_can_driver, 500000);
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, test_can_driver.state); // State should remain INIT
    TEST_ASSERT_NULL(test_can_driver.can); // Expected no allocation
    TEST_ASSERT_TRUE(stub_malloc.was_called);
    TEST_ASSERT_FALSE(stub_can_init.was_called); // Should not call external init
}

void test_can_driver_init_can_init_fails(void) {
    // Test: can_init external function returns an error
    // Expected: Driver state ERROR, allocated memory freed, ERROR_BUSY propagated
    stub_can_init.return_value = PROTOCOL_ERROR_BUFFER_FULL;

    error_t result = can_driver_init(&test_can_driver, 500000);
    TEST_ASSERT_EQUAL(ERROR_BUSY, result); // Source converts protocol error to ERROR_BUSY
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_can_driver.state);
    TEST_ASSERT_NULL(test_can_driver.can); // Expected allocated memory to be freed
    TEST_ASSERT_TRUE(stub_malloc.was_called); // Malloc should have been called
    TEST_ASSERT_TRUE(stub_free.was_called); // Free should have been called
    TEST_ASSERT_EQUAL_PTR(mock_heap_allocated_pointers[0], stub_free.captured_ptr); // Free should be for the allocated CAN handle
    TEST_ASSERT_TRUE(stub_can_init.was_called);
}

void test_can_driver_send_message_null_driver(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    can_frame_t frame = {0};
    error_t result = can_driver_send_message(NULL, &frame);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_can_transmit_message.was_called);
}

void test_can_driver_send_message_null_frame(void) {
    // Test: frame is NULL
    // Expected: ERROR_INVALID_PARAM
    test_can_driver.state = DEVICE_STATE_READY;
    error_t result = can_driver_send_message(&test_can_driver, NULL);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_can_transmit_message.was_called);
}

void test_can_driver_send_message_not_ready(void) {
    // Test: driver state is not READY
    // Expected: ERROR_INVALID_PARAM
    test_can_driver.state = DEVICE_STATE_INIT; // Not ready
    can_frame_t frame = {0};
    error_t result = can_driver_send_message(&test_can_driver, &frame);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_can_transmit_message.was_called);
}

void test_can_driver_send_message_successful(void) {
    // Test: Successful message transmission
    // Expected: ERROR_NONE, state BUSY then READY, can_transmit_message called
    can_driver_init(&test_can_driver, 500000); // Initialize first
    test_can_driver.state = DEVICE_STATE_READY;
    can_frame_t frame = { .id = 0x123, .dlc = 8, .data = {1,2,3,4,5,6,7,8}, .is_extended = false, .is_remote = false };

    error_t result = can_driver_send_message(&test_can_driver, &frame);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_TRUE(stub_can_transmit_message.was_called);
    TEST_ASSERT_EQUAL(1, stub_can_transmit_message.call_count);
    TEST_ASSERT_EQUAL_PTR(test_can_driver.can, stub_can_transmit_message.captured_handle);
    TEST_ASSERT_EQUAL_MEMORY(&frame, &stub_can_transmit_message.captured_frame, sizeof(can_frame_t));
    TEST_ASSERT_EQUAL(1000, stub_can_transmit_message.captured_timeout_ms); // Magic number from source
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_can_driver.state);
}

void test_can_driver_send_message_fails(void) {
    // Test: can_transmit_message external function returns an error
    // Expected: ERROR_BUSY, driver state ERROR, error logged
    can_driver_init(&test_can_driver, 500000);
    test_can_driver.state = DEVICE_STATE_READY;
    can_frame_t frame = { .id = 0x123, .dlc = 1, .data = {0xAA}, .is_extended = false, .is_remote = false };
    stub_can_transmit_message.return_value = PROTOCOL_ERROR_BUS_OFF;

    error_t result = can_driver_send_message(&test_can_driver, &frame);
    TEST_ASSERT_EQUAL(ERROR_BUSY, result); // Protocol error mapped to ERROR_BUSY
    TEST_ASSERT_TRUE(stub_can_transmit_message.was_called);
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_can_driver.state);
    TEST_ASSERT_EQUAL(1, test_can_driver.error_index);
    TEST_ASSERT_EQUAL((error_t)PROTOCOL_ERROR_BUS_OFF, test_can_driver.errors[0].error_code); // Stored as error_t
}

// --- Sensor Driver Tests ---

void test_sensor_driver_init_null_driver(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    error_t result = sensor_driver_init(NULL, 0, 0);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_FALSE(stub_malloc.was_called);
}

void test_sensor_driver_init_invalid_interface_type(void) {
    // Test: interface_type is invalid
    // Expected: ERROR_INVALID_PARAM, no interface allocated, state remains INIT
    error_t result = sensor_driver_init(&test_sensor_driver, 99, 0); // Invalid interface_type
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, test_sensor_driver.state); // State should not change from INIT
    TEST_ASSERT_NULL(test_sensor_driver.i2c);
    TEST_ASSERT_NULL(test_sensor_driver.spi);
    TEST_ASSERT_NULL(test_sensor_driver.uart);
    TEST_ASSERT_FALSE(stub_malloc.was_called); // No attempt to malloc
}

void test_sensor_driver_init_i2c_interface_successful(void) {
    // Test: I2C interface initialization
    // Expected: ERROR_NONE, state READY, i2c driver allocated and initialized (internal call)
    error_t result = sensor_driver_init(&test_sensor_driver, 0, 1); // Interface 0 = I2C
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.state);
    TEST_ASSERT_TRUE(stub_malloc.call_count >= 2); // One for sensor->i2c, one for sensor->i2c->i2c_regs
    TEST_ASSERT_NOT_NULL(test_sensor_driver.i2c);
    TEST_ASSERT_NULL(test_sensor_driver.spi);
    TEST_ASSERT_NULL(test_sensor_driver.uart);
    TEST_ASSERT_EQUAL(1, test_sensor_driver.sensor_type);
    TEST_ASSERT_EQUAL(100, test_sensor_driver.sampling_rate);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, test_sensor_driver.calibration_offset);
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.0f, test_sensor_driver.calibration_scale);
    // Internally calls i2c_driver_init. Since it's internal, we don't stub it, but check its side effects.
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.i2c->state);
    TEST_ASSERT_NOT_NULL(test_sensor_driver.i2c->i2c_regs);
    TEST_ASSERT_EQUAL(0x40, test_sensor_driver.i2c->device_addr);
    TEST_ASSERT_EQUAL(100, test_sensor_driver.i2c->timeout_ms);
}

void test_sensor_driver_init_i2c_interface_malloc_fail_driver(void) {
    // Test: Malloc for i2c_driver_t within sensor_driver_init fails
    // Expected: ERROR_BUSY, no i2c driver allocated
    stub_malloc.next_call_will_fail = true; // Make the first malloc (for driver->i2c) fail
    error_t result = sensor_driver_init(&test_sensor_driver, 0, 1);
    TEST_ASSERT_EQUAL(ERROR_BUSY, result); // This function returns ERROR_BUSY on malloc failure
    TEST_ASSERT_NULL(test_sensor_driver.i2c);
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, test_sensor_driver.state); // State remains INIT
}

void test_sensor_driver_init_i2c_interface_malloc_fail_regs(void) {
    // Test: Malloc for i2c_regs within i2c_driver_init (called by sensor_driver_init) fails
    // Expected: ERROR_BUSY, i2c driver allocated but i2c_regs is NULL
    stub_malloc.next_call_will_fail = false; // First malloc (for driver->i2c) succeeds
    mock_heap_next_index = 0; // Ensure first malloc gets index 0
    malloc(1); // Consume mock_heap_allocated_pointers[0] for sensor->i2c
    stub_malloc.next_call_will_fail = true; // Make the next malloc (for i2c_regs) fail
    
    error_t result = sensor_driver_init(&test_sensor_driver, 0, 1);
    TEST_ASSERT_EQUAL(ERROR_BUSY, result); // i2c_driver_init returns ERROR_BUSY, which sensor_driver_init returns
    TEST_ASSERT_NOT_NULL(test_sensor_driver.i2c); // i2c driver struct itself is allocated
    TEST_ASSERT_NULL(test_sensor_driver.i2c->i2c_regs); // But its internal regs allocation failed
    TEST_ASSERT_EQUAL(DEVICE_STATE_ERROR, test_sensor_driver.state); // Sensor driver state becomes ERROR (as i2c init failed)
    TEST_ASSERT_TRUE(stub_free.was_called); // Should free the malloc'd i2c_driver_t struct
    TEST_ASSERT_EQUAL_PTR(mock_heap_allocated_pointers[0], stub_free.captured_ptr);
}


void test_sensor_driver_init_spi_interface_successful(void) {
    // Test: SPI interface initialization
    // Expected: ERROR_NONE, state READY, spi driver allocated
    error_t result = sensor_driver_init(&test_sensor_driver, 1, 2); // Interface 1 = SPI
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.state);
    TEST_ASSERT_NULL(test_sensor_driver.i2c);
    TEST_ASSERT_TRUE(stub_malloc.was_called); // Malloc should be called for driver->spi
    TEST_ASSERT_NOT_NULL(test_sensor_driver.spi);
    TEST_ASSERT_NULL(test_sensor_driver.uart);
    TEST_ASSERT_EQUAL(2, test_sensor_driver.sensor_type);
}

void test_sensor_driver_init_spi_interface_malloc_fail(void) {
    // Test: Malloc for spi_driver_t within sensor_driver_init fails
    // Expected: ERROR_BUSY, no spi driver allocated
    stub_malloc.next_call_will_fail = true;
    error_t result = sensor_driver_init(&test_sensor_driver, 1, 2);
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    TEST_ASSERT_NULL(test_sensor_driver.spi);
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, test_sensor_driver.state);
}

void test_sensor_driver_init_uart_interface_successful(void) {
    // Test: UART interface initialization
    // Expected: ERROR_NONE, state READY, uart driver allocated
    error_t result = sensor_driver_init(&test_sensor_driver, 2, 3); // Interface 2 = UART
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.state);
    TEST_ASSERT_NULL(test_sensor_driver.i2c);
    TEST_ASSERT_NULL(test_sensor_driver.spi);
    TEST_ASSERT_TRUE(stub_malloc.was_called); // Malloc should be called for driver->uart
    TEST_ASSERT_NOT_NULL(test_sensor_driver.uart);
    TEST_ASSERT_EQUAL(3, test_sensor_driver.sensor_type);
}

void test_sensor_driver_init_uart_interface_malloc_fail(void) {
    // Test: Malloc for uart_driver_t within sensor_driver_init fails
    // Expected: ERROR_BUSY, no uart driver allocated
    stub_malloc.next_call_will_fail = true;
    error_t result = sensor_driver_init(&test_sensor_driver, 2, 3);
    TEST_ASSERT_EQUAL(ERROR_BUSY, result);
    TEST_ASSERT_NULL(test_sensor_driver.uart);
    TEST_ASSERT_EQUAL(DEVICE_STATE_INIT, test_sensor_driver.state);
}


void test_sensor_driver_read_null_driver(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    float value = 0.0f;
    error_t result = sensor_driver_read(NULL, &value);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
}

void test_sensor_driver_read_null_value(void) {
    // Test: value is NULL
    // Expected: ERROR_INVALID_PARAM
    test_sensor_driver.state = DEVICE_STATE_READY;
    error_t result = sensor_driver_read(&test_sensor_driver, NULL);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
}

void test_sensor_driver_read_not_ready(void) {
    // Test: driver state is not READY
    // Expected: ERROR_INVALID_PARAM
    test_sensor_driver.state = DEVICE_STATE_INIT; // Not ready
    float value = 0.0f;
    error_t result = sensor_driver_read(&test_sensor_driver, &value);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
}

void test_sensor_driver_read_i2c_interface_successful(void) {
    // Test: Read using I2C interface
    // Expected: ERROR_NONE, value updated, data_ready_cb called
    sensor_driver_init(&test_sensor_driver, 0, 1); // Init with I2C
    test_sensor_driver.state = DEVICE_STATE_READY;
    test_sensor_driver.data_ready_cb = mock_data_ready_callback;
    test_sensor_driver.callback_context = (void*)0xCAFE;
    
    // The i2c_driver_read mock memset to 0xAA.
    // Conversion: ((data[0] << 8) | data[1]) * 0.01f
    // With data = {0xAA, 0xAA}, this is (0xAAAA) * 0.01f = 43690 * 0.01f = 436.9f
    float value = 0.0f;
    float expected_value = 436.9f; 

    error_t result = sensor_driver_read(&test_sensor_driver, &value);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.state);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_value, value); // Value from I2C read conversion
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_value, test_sensor_driver.last_value);
    TEST_ASSERT_TRUE(mock_data_ready_cb_called);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_value, mock_read_value);
    TEST_ASSERT_EQUAL_PTR((void*)0xCAFE, mock_data_cb_context);
}

void test_sensor_driver_read_other_interface_successful(void) {
    // Test: Read using other (non-I2C) interface
    // Expected: ERROR_NONE, default mock value, data_ready_cb called
    sensor_driver_init(&test_sensor_driver, 1, 1); // Init with SPI (not I2C)
    test_sensor_driver.state = DEVICE_STATE_READY;
    test_sensor_driver.data_ready_cb = mock_data_ready_callback;
    
    float value = 0.0f;
    float expected_value = 25.5f; // Mock value for non-I2C path from source
    
    error_t result = sensor_driver_read(&test_sensor_driver, &value);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_EQUAL(DEVICE_STATE_READY, test_sensor_driver.state);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_value, value); // Default mock value
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_value, test_sensor_driver.last_value);
    TEST_ASSERT_TRUE(mock_data_ready_cb_called);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_value, mock_read_value);
}

void test_sensor_driver_read_with_calibration(void) {
    // Test: Read with calibration offset and scale
    // Expected: Value adjusted by calibration factors
    sensor_driver_init(&test_sensor_driver, 1, 1); // Init with SPI (non-I2C)
    test_sensor_driver.state = DEVICE_STATE_READY;
    test_sensor_driver.calibration_offset = 5.0f;
    test_sensor_driver.calibration_scale = 2.0f;

    float value = 0.0f;
    float base_mock_value = 25.5f; // From source
    float expected_value = (base_mock_value * 2.0f) + 5.0f; // (25.5 * 2.0) + 5.0 = 51.0 + 5.0 = 56.0f

    error_t result = sensor_driver_read(&test_sensor_driver, &value);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_value, value);
    TEST_ASSERT_FLOAT_WITHIN(0.01f, expected_value, test_sensor_driver.last_value);
}

void test_sensor_driver_calibrate_null_driver(void) {
    // Test: driver is NULL
    // Expected: ERROR_INVALID_PARAM
    error_t result = sensor_driver_calibrate(NULL, 10.0f);
    TEST_ASSERT_EQUAL(ERROR_INVALID_PARAM, result);
}

void test_sensor_driver_calibrate_successful(void) {
    // Test: Successful calibration
    // Expected: calibration_offset adjusted to make last_value match reference
    sensor_driver_init(&test_sensor_driver, 1, 1); // Init with SPI
    test_sensor_driver.state = DEVICE_STATE_READY;
    test_sensor_driver.calibration_offset = 0.0f; // Start with no offset
    test_sensor_driver.calibration_scale = 1.0f; // Start with no scale
    
    float initial_read_value;
    sensor_driver_read(&test_sensor_driver, &initial_read_value); // Perform a read to set last_value
    // Initial read will set last_value to 25.5f (mocked non-I2C value)

    float reference_value = 30.0f;
    error_t result = sensor_driver_calibrate(&test_sensor_driver, reference_value);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected offset = reference_value - last_value = 30.0f - 25.5f = 4.5f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 4.5f, test_sensor_driver.calibration_offset);

    // Verify a subsequent read with the new calibration
    float calibrated_read_value;
    sensor_driver_read(&test_sensor_driver, &calibrated_read_value);
    // Expected: (25.5f * 1.0f) + 4.5f = 30.0f
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 30.0f, calibrated_read_value);
}

void test_sensor_driver_calibrate_with_existing_offset(void) {
    // Test: Calibration adjusts an already existing offset
    // Expected: New offset correctly calculated based on current last_value
    sensor_driver_init(&test_sensor_driver, 1, 1); // Init with SPI
    test_sensor_driver.state = DEVICE_STATE_READY;
    test_sensor_driver.calibration_offset = 10.0f; // Existing offset
    test_sensor_driver.calibration_scale = 1.0f;
    
    float initial_read_value;
    sensor_driver_read(&test_sensor_driver, &initial_read_value);
    // last_value will be (25.5f * 1.0f) + 10.0f = 35.5f

    float reference_value = 40.0f;
    error_t result = sensor_driver_calibrate(&test_sensor_driver, reference_value);
    TEST_ASSERT_EQUAL(ERROR_NONE, result);
    // Expected offset = reference_value - last_value = 40.0f - 35.5f = 4.5f
    // The previous offset is overwritten/adjusted.
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 4.5f, test_sensor_driver.calibration_offset);
}

// --- Main Unity Test Runner ---


int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_uart_driver_init_null_driver_param);
    RUN_TEST(test_uart_driver_init_null_config_param);
    RUN_TEST(test_uart_driver_init_successful);
    RUN_TEST(test_uart_driver_init_malloc_fails);
    RUN_TEST(test_uart_driver_init_uart_init_fails);
    RUN_TEST(test_uart_driver_transmit_null_driver);
    RUN_TEST(test_uart_driver_transmit_null_data);
    RUN_TEST(test_uart_driver_transmit_zero_size);
    RUN_TEST(test_uart_driver_transmit_not_ready);
    RUN_TEST(test_uart_driver_transmit_successful);
    RUN_TEST(test_uart_driver_transmit_fails);
    RUN_TEST(test_uart_driver_transmit_error_history_wrap);
    RUN_TEST(test_uart_driver_receive_null_driver);
    RUN_TEST(test_uart_driver_receive_null_data);
    RUN_TEST(test_uart_driver_receive_zero_size);
    RUN_TEST(test_uart_driver_receive_not_ready);
    RUN_TEST(test_uart_driver_receive_successful);
    RUN_TEST(test_uart_driver_receive_fails);
    RUN_TEST(test_uart_driver_process_interrupt_null_driver);
    RUN_TEST(test_uart_driver_process_interrupt_rxne);
    RUN_TEST(test_uart_driver_process_interrupt_txe);
    RUN_TEST(test_uart_driver_process_interrupt_both_flags);
    RUN_TEST(test_uart_driver_process_interrupt_no_flags);
    RUN_TEST(test_spi_driver_init_null_driver);
    RUN_TEST(test_spi_driver_init_null_config);
    RUN_TEST(test_spi_driver_init_successful);
    RUN_TEST(test_spi_driver_init_malloc_fails);
    RUN_TEST(test_spi_driver_init_spi_init_fails);
    RUN_TEST(test_spi_driver_transfer_null_driver);
    RUN_TEST(test_spi_driver_transfer_null_tx_data);
    RUN_TEST(test_spi_driver_transfer_null_rx_data);
    RUN_TEST(test_spi_driver_transfer_not_ready);
    RUN_TEST(test_spi_driver_transfer_successful);
    RUN_TEST(test_spi_driver_transfer_fails);
    RUN_TEST(test_i2c_driver_init_null_driver);
    RUN_TEST(test_i2c_driver_init_successful);
    RUN_TEST(test_i2c_driver_init_malloc_fails);
    RUN_TEST(test_i2c_driver_write_null_driver);
    RUN_TEST(test_i2c_driver_write_null_data);
    RUN_TEST(test_i2c_driver_write_not_ready);
    RUN_TEST(test_i2c_driver_write_successful);
    RUN_TEST(test_i2c_driver_read_null_driver);
    RUN_TEST(test_i2c_driver_read_null_data);
    RUN_TEST(test_i2c_driver_read_not_ready);
    RUN_TEST(test_i2c_driver_read_successful);
    RUN_TEST(test_can_driver_init_null_driver);
    RUN_TEST(test_can_driver_init_successful);
    RUN_TEST(test_can_driver_init_malloc_fails);
    RUN_TEST(test_can_driver_init_can_init_fails);
    RUN_TEST(test_can_driver_send_message_null_driver);
    RUN_TEST(test_can_driver_send_message_null_frame);
    RUN_TEST(test_can_driver_send_message_not_ready);
    RUN_TEST(test_can_driver_send_message_successful);
    RUN_TEST(test_can_driver_send_message_fails);
    RUN_TEST(test_sensor_driver_init_null_driver);
    RUN_TEST(test_sensor_driver_init_invalid_interface_type);
    RUN_TEST(test_sensor_driver_init_i2c_interface_successful);
    RUN_TEST(test_sensor_driver_init_i2c_interface_malloc_fail_driver);
    RUN_TEST(test_sensor_driver_init_i2c_interface_malloc_fail_regs);
    RUN_TEST(test_sensor_driver_init_spi_interface_successful);
    RUN_TEST(test_sensor_driver_init_spi_interface_malloc_fail);
    RUN_TEST(test_sensor_driver_init_uart_interface_successful);
    RUN_TEST(test_sensor_driver_init_uart_interface_malloc_fail);
    RUN_TEST(test_sensor_driver_read_null_driver);
    RUN_TEST(test_sensor_driver_read_null_value);
    RUN_TEST(test_sensor_driver_read_not_ready);
    RUN_TEST(test_sensor_driver_read_i2c_interface_successful);
    RUN_TEST(test_sensor_driver_read_other_interface_successful);
    RUN_TEST(test_sensor_driver_read_with_calibration);
    RUN_TEST(test_sensor_driver_calibrate_null_driver);
    RUN_TEST(test_sensor_driver_calibrate_successful);
    RUN_TEST(test_sensor_driver_calibrate_with_existing_offset);

    return UNITY_END();
}