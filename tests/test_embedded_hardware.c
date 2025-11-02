/* test_embedded_hardware.c – Auto-generated Expert Unity Tests */

#include "unity.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h> // For memset

// --- Mock Hardware Register Structures (MUST match embedded_hardware.h implicitly) ---
// These are definitions of the hardware register layouts for mocking purposes.
// In a real project, these would come from the MCU header file (e.g., stm32f4xx.h).
// We define them here to make the test file self-contained and compilable.
typedef struct {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t OSPEEDR;
    volatile uint32_t PUPDR;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t LCKR;
    volatile uint32_t AFR[2]; // AFR[0] for pins 00.0f, AFR[1] for pins 80.0f
} GPIO_TypeDef;

typedef struct {
    volatile uint32_t SR;  // Status Register
    volatile uint32_t DR;  // Data Register
    volatile uint32_t BRR; // Baud Rate Register
    volatile uint32_t CR1; // Control Register 1
    volatile uint32_t CR2; // Control Register 2
    volatile uint32_t CR3; // Control Register 3
    volatile uint32_t GTPR;
} USART_TypeDef;

typedef struct {
    volatile uint32_t CR1; // Control Register 1
    volatile uint32_t CR2; // Control Register 2
    volatile uint32_t SR;  // Status Register
    volatile uint32_t DR;  // Data Register
    volatile uint32_t CRCPOLY;
    volatile uint32_t RXCRCR;
    volatile uint32_t TXCRCR;
    volatile uint32_t I2SCFGR;
    volatile uint32_t I2SPR;
} SPI_TypeDef;

// --- Mock Configuration Structures (MUST match embedded_hardware.h implicitly) ---
typedef struct {
    uint8_t pin;       // Pin number, not directly used in gpio_init logic but part of config
    uint8_t mode;      // 0: Input, 1: Output, 2: Alternate function, 3: Analog
    uint8_t pull;      // 0: No pull-up/pull-down, 1: Pull-up, 2: Pull-down
    uint8_t speed;     // 0: Low, 1: Medium, 2: High, 3: Very high
    uint8_t alternate; // Alternate function number (00.0f)
} gpio_config_t;

typedef struct {
    uint32_t baud_rate;
    uint8_t data_bits; // 8 or 9
    uint8_t parity;    // 0: None, 1: Even, 2: Odd
    uint8_t stop_bits; // 0: 1 stop bit, 1: 0.5 stop bit, 2: 2 stop bits, 3: 1.5 stop bits
} uart_config_t;

typedef struct {
    uint8_t baud_rate; // Baud rate prescaler (00.0f)
    uint8_t data_size; // 8 or 16 bits
    uint8_t mode;      // CPOL/CPHA combination (00.0f)
    uint8_t direction; // 0: Full-duplex, 1: Receive-only, 2: Transmit-only
    bool nss_mode;     // true: Hardware NSS output enable, false: Software NSS
    bool dma_enable;
} spi_config_t;

// --- Mock Error Enum (MUST match embedded_hardware.h implicitly) ---
typedef enum {
    ERROR_NONE = 0,
    ERROR_INVALID_PARAM,
    ERROR_TIMEOUT
} error_t;

// --- Mock Register Bit Definitions (MUST match embedded_hardware.h implicitly) ---
// These are typical STM32-style register bits, assumed to be defined in embedded_hardware.h
#define USART_CR1_M         (0x1U << 12)
#define USART_CR1_PCE       (0x1U << 10)
#define USART_CR1_PS        (0x1U << 9)
#define USART_CR1_UE        (0x1U << 13)
#define USART_CR2_STOP      (0x3U << 12) // Mask for Stop bits, e.g., 00: 1 stop bit, 01: 0.5 stop bit, 10: 2 stop bits, 11: 1.5 stop bits

#define USART_SR_TXE        (0x1U << 7) // Transmit data register empty
#define USART_SR_TC         (0x1U << 6) // Transmission complete
#define USART_SR_RXNE       (0x1U << 5) // Read data register not empty

#define SPI_CR1_SPE         (0x1U << 6) // SPI Enable
#define SPI_CR2_SSOE        (0x1U << 2) // SS output enable
#define SPI_CR2_TXDMAEN     (0x1U << 1) // Tx buffer DMA enable
#define SPI_CR2_RXDMAEN     (0x1U << 0) // Rx buffer DMA enable

#define SPI_SR_TXE          (0x1U << 1) // Transmit buffer empty
#define SPI_SR_RXNE         (0x1U << 0) // Receive buffer not empty


// --- Declare functions from source file for direct testing ---
// These functions are internal to embedded_hardware.c, so we declare them here
// to allow direct testing without linker errors.
error_t gpio_init(GPIO_TypeDef *gpio, gpio_config_t *config);
error_t gpio_write_pin(GPIO_TypeDef *gpio, uint16_t pin, bool state);
bool gpio_read_pin(GPIO_TypeDef *gpio, uint16_t pin);

error_t uart_init(USART_TypeDef *uart, uart_config_t *config);
error_t uart_transmit(USART_TypeDef *uart, uint8_t *data, uint16_t size, uint32_t timeout);
error_t uart_receive(USART_TypeDef *uart, uint8_t *data, uint16_t size, uint32_t timeout);

error_t spi_init(SPI_TypeDef *spi, spi_config_t *config);
error_t spi_transmit_receive(SPI_TypeDef *spi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size);


// --- Mock Hardware Instances ---
static GPIO_TypeDef mock_gpio;
static USART_TypeDef mock_uart;
static SPI_TypeDef mock_spi;

// --- setUp and tearDown functions ---
void setUp(void) {
    // Reset all mock hardware registers to a known initial state (all zeros)
    memset(&mock_gpio, 0, sizeof(GPIO_TypeDef));
    memset(&mock_uart, 0, sizeof(USART_TypeDef));
    memset(&mock_spi, 0, sizeof(SPI_TypeDef));
}

void tearDown(void) {
    // No specific cleanup needed beyond what setUp does, but good practice to keep
    // if more complex stateful mocks were involved.
    // Reset all mock hardware registers to a known initial state (all zeros)
    memset(&mock_gpio, 0, sizeof(GPIO_TypeDef));
    memset(&mock_uart, 0, sizeof(USART_TypeDef));
    memset(&mock_spi, 0, sizeof(SPI_TypeDef));
}

// ====================================================================================
// --- GPIO Functions Tests ---
// ====================================================================================

// Test gpio_init with NULL gpio pointer
void test_gpio_init_null_gpio_param(void) {
    gpio_config_t config = {0};
    error_t result = gpio_init(NULL, &config);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL gpio pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test gpio_init with NULL config pointer
void test_gpio_init_null_config_param(void) {
    error_t result = gpio_init(&mock_gpio, NULL);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL config pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test gpio_init for input mode, no pull, low speed, no alternate function
void test_gpio_init_input_mode_no_pull_low_speed(void) {
    gpio_config_t config = {.pin = 5, .mode = 0, .pull = 0, .speed = 0, .alternate = 0}; // Input, No pull, Low speed
    error_t result = gpio_init(&mock_gpio, &config);
    // Expected: Function should return ERROR_NONE for valid parameters.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: MODER for pin 5 (bits 100.0f) should be 00 (Input mode).
    TEST_ASSERT_EQUAL_HEX(0x0U, mock_gpio.MODER & (0x3U << (config.pin * 2)));
    // Expected: PUPDR for pin 5 (bits 100.0f) should be 00 (No pull).
    TEST_ASSERT_EQUAL_HEX(0x0U, mock_gpio.PUPDR & (0x3U << (config.pin * 2)));
    // Expected: OSPEEDR for pin 5 (bits 100.0f) should be 00 (Low speed).
    TEST_ASSERT_EQUAL_HEX(0x0U, mock_gpio.OSPEEDR & (0x3U << (config.pin * 2)));
    // Expected: AFR registers should not be modified for input mode.
    TEST_ASSERT_EQUAL_HEX(0x0U, mock_gpio.AFR[0]);
    TEST_ASSERT_EQUAL_HEX(0x0U, mock_gpio.AFR[1]);
}

// Test gpio_init for output mode, pull-up, medium speed
void test_gpio_init_output_mode_pull_up_medium_speed(void) {
    gpio_config_t config = {.pin = 7, .mode = 1, .pull = 1, .speed = 1, .alternate = 0}; // Output, Pull-up, Medium speed
    error_t result = gpio_init(&mock_gpio, &config);
    // Expected: Function should return ERROR_NONE for valid parameters.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: MODER for pin 7 (bits 140.0f) should be 01 (Output mode).
    TEST_ASSERT_EQUAL_HEX(0x1U << (config.pin * 2), mock_gpio.MODER & (0x3U << (config.pin * 2)));
    // Expected: PUPDR for pin 7 (bits 140.0f) should be 01 (Pull-up).
    TEST_ASSERT_EQUAL_HEX(0x1U << (config.pin * 2), mock_gpio.PUPDR & (0x3U << (config.pin * 2)));
    // Expected: OSPEEDR for pin 7 (bits 140.0f) should be 01 (Medium speed).
    TEST_ASSERT_EQUAL_HEX(0x1U << (config.pin * 2), mock_gpio.OSPEEDR & (0x3U << (config.pin * 2)));
}

// Test gpio_init for alternate function mode (mode 2), pull-down, very high speed, AF4 on pin 0
void test_gpio_init_af_mode_pull_down_very_high_speed_af4_pin0(void) {
    gpio_config_t config = {.pin = 0, .mode = 2, .pull = 2, .speed = 3, .alternate = 4}; // AF, Pull-down, Very high speed, AF4
    error_t result = gpio_init(&mock_gpio, &config);
    // Expected: Function should return ERROR_NONE for valid parameters.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: MODER for pin 0 (bits 00.0f) should be 10 (Alternate function mode).
    TEST_ASSERT_EQUAL_HEX(0x2U << (config.pin * 2), mock_gpio.MODER & (0x3U << (config.pin * 2)));
    // Expected: PUPDR for pin 0 (bits 00.0f) should be 10 (Pull-down).
    TEST_ASSERT_EQUAL_HEX(0x2U << (config.pin * 2), mock_gpio.PUPDR & (0x3U << (config.pin * 2)));
    // Expected: OSPEEDR for pin 0 (bits 00.0f) should be 11 (Very high speed).
    TEST_ASSERT_EQUAL_HEX(0x3U << (config.pin * 2), mock_gpio.OSPEEDR & (0x3U << (config.pin * 2)));
    // Expected: AFR[0] for pin 0 (bits 00.0f) should be 4 (AF4).
    TEST_ASSERT_EQUAL_HEX(0x4U << ((config.alternate % 8) * 4), mock_gpio.AFR[0] & (0xFU << ((config.alternate % 8) * 4)));
}

// Test gpio_init for alternate function mode (mode 3 - Analog), AF15 on pin 15 (AFR[1])
void test_gpio_init_analog_mode_af15_pin15(void) {
    gpio_config_t config = {.pin = 15, .mode = 3, .pull = 0, .speed = 0, .alternate = 15}; // Analog (AF mode), AF15
    error_t result = gpio_init(&mock_gpio, &config);
    // Expected: Function should return ERROR_NONE for valid parameters.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: MODER for pin 15 (bits 300.0f) should be 11 (Analog mode).
    TEST_ASSERT_EQUAL_HEX(0x3U << (config.pin * 2), mock_gpio.MODER & (0x3U << (config.pin * 2)));
    // Expected: AFR[1] for pin 15 (bits (15%8)*4 = 280.0f) should be 15 (AF15).
    TEST_ASSERT_EQUAL_HEX(0xFU << ((config.pin % 8) * 4), mock_gpio.AFR[1] & (0xFU << ((config.pin % 8) * 4)));
}

// Test gpio_write_pin with NULL gpio pointer
void test_gpio_write_pin_null_gpio_param(void) {
    error_t result = gpio_write_pin(NULL, 0, true);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL gpio pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test gpio_write_pin with invalid pin number (too high)
void test_gpio_write_pin_invalid_pin_high(void) {
    error_t result = gpio_write_pin(&mock_gpio, 16, true); // Pin 16 is invalid (max 15)
    // Expected: Function should return ERROR_INVALID_PARAM for an invalid pin.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test gpio_write_pin to set a specific pin (pin 5)
void test_gpio_write_pin_set_pin_5(void) {
    uint16_t pin_to_set = 5;
    error_t result = gpio_write_pin(&mock_gpio, pin_to_set, true);
    // Expected: Function should return ERROR_NONE for valid parameters.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: BSRR should have the bit for pin 5 set (1U << 5).
    TEST_ASSERT_EQUAL_HEX(1U << pin_to_set, mock_gpio.BSRR);
}

// Test gpio_write_pin to reset a specific pin (pin 12)
void test_gpio_write_pin_reset_pin_12(void) {
    uint16_t pin_to_reset = 12;
    error_t result = gpio_write_pin(&mock_gpio, pin_to_reset, false);
    // Expected: Function should return ERROR_NONE for valid parameters.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: BSRR should have the reset bit for pin 12 set (1U << (12 + 16)).
    TEST_ASSERT_EQUAL_HEX(1U << (pin_to_reset + 16), mock_gpio.BSRR);
}

// Test gpio_read_pin with NULL gpio pointer
void test_gpio_read_pin_null_gpio_param(void) {
    bool result = gpio_read_pin(NULL, 0);
    // Expected: Function should return false for a NULL gpio pointer.
    TEST_ASSERT_FALSE(result);
}

// Test gpio_read_pin with invalid pin number (too high)
void test_gpio_read_pin_invalid_pin_high(void) {
    bool result = gpio_read_pin(&mock_gpio, 16); // Pin 16 is invalid
    // Expected: Function should return false for an invalid pin.
    TEST_ASSERT_FALSE(result);
}

// Test gpio_read_pin when pin is high
void test_gpio_read_pin_is_high(void) {
    uint16_t pin_to_read = 3;
    mock_gpio.IDR = (1U << pin_to_read); // Simulate pin 3 being high
    bool result = gpio_read_pin(&mock_gpio, pin_to_read);
    // Expected: Function should return true as the pin is set in IDR.
    TEST_ASSERT_TRUE(result);
}

// Test gpio_read_pin when pin is low
void test_gpio_read_pin_is_low(void) {
    uint16_t pin_to_read = 8;
    mock_gpio.IDR = 0x0U; // Simulate all pins being low
    bool result = gpio_read_pin(&mock_gpio, pin_to_read);
    // Expected: Function should return false as the pin is not set in IDR.
    TEST_ASSERT_FALSE(result);
}

// ====================================================================================
// --- UART Functions Tests ---
// ====================================================================================

// Test uart_init with NULL uart pointer
void test_uart_init_null_uart_param(void) {
    uart_config_t config = {0};
    error_t result = uart_init(NULL, &config);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL uart pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test uart_init with NULL config pointer
void test_uart_init_null_config_param(void) {
    error_t result = uart_init(&mock_uart, NULL);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL config pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test uart_init with 8 data bits, no parity, 1 stop bit
void test_uart_init_8n1(void) {
    uart_config_t config = {
        .baud_rate = 115200, .data_bits = 8, .parity = 0, .stop_bits = 0
    };
    error_t result = uart_init(&mock_uart, &config);
    // Expected: Function should return ERROR_NONE for valid parameters.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: BRR should be set to the configured baud rate.
    TEST_ASSERT_EQUAL_HEX(config.baud_rate, mock_uart.BRR);
    // Expected: CR1 should have M, PCE, PS bits cleared (8-bit, no parity).
    TEST_ASSERT_EQUAL_HEX(0x0U, mock_uart.CR1 & (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS));
    // Expected: CR2 should have STOP bits cleared (1 stop bit).
    TEST_ASSERT_EQUAL_HEX(0x0U, mock_uart.CR2 & USART_CR2_STOP);
    // Expected: CR1 should have UE bit set (UART Enabled).
    TEST_ASSERT_EQUAL_HEX(USART_CR1_UE, mock_uart.CR1 & USART_CR1_UE);
}

// Test uart_init with 9 data bits, even parity, 2 stop bits
void test_uart_init_9e2(void) {
    uart_config_t config = {
        .baud_rate = 9600, .data_bits = 9, .parity = 1, .stop_bits = 2
    };
    error_t result = uart_init(&mock_uart, &config);
    // Expected: Function should return ERROR_NONE for valid parameters.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: BRR should be set.
    TEST_ASSERT_EQUAL_HEX(config.baud_rate, mock_uart.BRR);
    // Expected: CR1 should have M (9-bit) and PCE (parity enable) set, PS (parity select) cleared (even).
    TEST_ASSERT_EQUAL_HEX(USART_CR1_M | USART_CR1_PCE, mock_uart.CR1 & (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS));
    // Expected: CR2 should have 2 stop bits (config.stop_bits = 2).
    TEST_ASSERT_EQUAL_HEX(config.stop_bits << 12, mock_uart.CR2 & USART_CR2_STOP);
    // Expected: CR1 should have UE bit set.
    TEST_ASSERT_EQUAL_HEX(USART_CR1_UE, mock_uart.CR1 & USART_CR1_UE);
}

// Test uart_init with 8 data bits, odd parity, 1.5 stop bits
void test_uart_init_8o1_5(void) {
    uart_config_t config = {
        .baud_rate = 57600, .data_bits = 8, .parity = 2, .stop_bits = 3
    };
    error_t result = uart_init(&mock_uart, &config);
    // Expected: Function should return ERROR_NONE for valid parameters.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: BRR should be set.
    TEST_ASSERT_EQUAL_HEX(config.baud_rate, mock_uart.BRR);
    // Expected: CR1 should have PCE (parity enable) and PS (odd parity) set, M cleared (8-bit).
    TEST_ASSERT_EQUAL_HEX(USART_CR1_PCE | USART_CR1_PS, mock_uart.CR1 & (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS));
    // Expected: CR2 should have 1.5 stop bits (config.stop_bits = 3).
    TEST_ASSERT_EQUAL_HEX(config.stop_bits << 12, mock_uart.CR2 & USART_CR2_STOP);
    // Expected: CR1 should have UE bit set.
    TEST_ASSERT_EQUAL_HEX(USART_CR1_UE, mock_uart.CR1 & USART_CR1_UE);
}

// Test uart_transmit with NULL uart pointer
void test_uart_transmit_null_uart_param(void) {
    uint8_t data[] = {0xAA};
    error_t result = uart_transmit(NULL, data, 1, 100);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL uart pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test uart_transmit with NULL data pointer
void test_uart_transmit_null_data_param(void) {
    error_t result = uart_transmit(&mock_uart, NULL, 1, 100);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL data pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test uart_transmit with zero size
void test_uart_transmit_zero_size_param(void) {
    uint8_t data[] = {0xAA};
    error_t result = uart_transmit(&mock_uart, data, 0, 100);
    // Expected: Function should return ERROR_INVALID_PARAM for zero size.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test uart_transmit for single byte successfully
void test_uart_transmit_single_byte_success(void) {
    uint8_t data[] = {0x55};
    // Simulate TXE and TC flags are set immediately (or after 1 tick as timeout allows)
    mock_uart.SR = USART_SR_TXE | USART_SR_TC;
    error_t result = uart_transmit(&mock_uart, data, 1, 1); // Timeout of 1 allows for 1 check
    // Expected: Function should return ERROR_NONE for successful transmission.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: The data register should contain the transmitted byte.
    TEST_ASSERT_EQUAL_HEX(data[0], mock_uart.DR);
}

// Test uart_transmit for multiple bytes successfully
void test_uart_transmit_multiple_bytes_success(void) {
    uint8_t data[] = {0x01, 0x02, 0x03};
    uint16_t size = sizeof(data);

    // To simulate successful transmission, we need the SR flags to be set for each iteration.
    // For simplicity, we assume the flags are set 'just in time' for the loop to proceed.
    // In a more complex mock, we might increment a counter and set the flag after a certain call count.
    // Here, we ensure SR is ready to proceed through the loop quickly.
    mock_uart.SR = USART_SR_TXE | USART_SR_TC; // Set both flags, assume they 'clear' and 'set' for next byte

    error_t result = uart_transmit(&mock_uart, data, size, 1); // Small timeout, but flags are pre-set
    // Expected: Function should return ERROR_NONE for successful transmission.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: The last byte (0x03) should be in the DR (DR is typically overwritten).
    TEST_ASSERT_EQUAL_HEX(data[size - 1], mock_uart.DR);
}

// Test uart_transmit with timeout waiting for TXE
void test_uart_transmit_timeout_txe(void) {
    uint8_t data[] = {0xAA};
    // Simulate TXE flag never being set (it's 0)
    mock_uart.SR = 0x0U;
    error_t result = uart_transmit(&mock_uart, data, 1, 0); // Timeout 0, meaning it fails on first check if TXE is not set
    // Expected: Function should return ERROR_TIMEOUT if TXE is not set within timeout.
    TEST_ASSERT_EQUAL_INT(ERROR_TIMEOUT, result);
    // Expected: Data register should remain unchanged as no data was written.
    TEST_ASSERT_EQUAL_HEX(0x0U, mock_uart.DR);
}

// Test uart_transmit with timeout waiting for TC
void test_uart_transmit_timeout_tc(void) {
    uint8_t data[] = {0xBB};
    // Simulate TXE being set, but TC never being set
    mock_uart.SR = USART_SR_TXE; // TXE is ready
    error_t result = uart_transmit(&mock_uart, data, 1, 0); // Timeout 0, it transmits, but waits for TC
    // Expected: Function should return ERROR_TIMEOUT if TC is not set within timeout.
    TEST_ASSERT_EQUAL_INT(ERROR_TIMEOUT, result);
    // Expected: Data register should have received the byte, but TC timeout happens afterwards.
    TEST_ASSERT_EQUAL_HEX(data[0], mock_uart.DR);
}

// Test uart_receive with NULL uart pointer
void test_uart_receive_null_uart_param(void) {
    uint8_t data[1];
    error_t result = uart_receive(NULL, data, 1, 100);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL uart pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test uart_receive with NULL data pointer
void test_uart_receive_null_data_param(void) {
    error_t result = uart_receive(&mock_uart, NULL, 1, 100);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL data pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test uart_receive with zero size
void test_uart_receive_zero_size_param(void) {
    uint8_t data[1];
    error_t result = uart_receive(&mock_uart, data, 0, 100);
    // Expected: Function should return ERROR_INVALID_PARAM for zero size.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test uart_receive for single byte successfully
void test_uart_receive_single_byte_success(void) {
    uint8_t rx_buffer[1];
    uint8_t expected_data = 0xCC;
    mock_uart.DR = expected_data;
    // Simulate RXNE flag is set immediately
    mock_uart.SR = USART_SR_RXNE;

    error_t result = uart_receive(&mock_uart, rx_buffer, 1, 1);
    // Expected: Function should return ERROR_NONE for successful reception.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: The receive buffer should contain the expected byte.
    TEST_ASSERT_EQUAL_HEX(expected_data, rx_buffer[0]);
}

// Test uart_receive for multiple bytes successfully
void test_uart_receive_multiple_bytes_success(void) {
    uint8_t rx_buffer[3];
    uint8_t expected_data[] = {0x11, 0x22, 0x33};
    uint16_t size = sizeof(expected_data);

    // Simulate RXNE flag being set, and DR providing sequential data.
    // This requires a more complex mock if we strictly follow loop mechanics,
    // but for unit testing, we can simplify by setting DR and SR such that
    // the loop runs and collects data.
    mock_uart.SR = USART_SR_RXNE; // RXNE is set
    // For each read, DR would provide the next byte. We can simulate this by direct assignment.
    // The actual loop reads DR and copies it. So we simulate a "stream" by setting DR then calling
    // which implies that each subsequent read of DR would yield the next byte.
    // This is an abstraction for unit testing a blocking receive.

    // Manual simulation of loop for clarity, though not directly how the function would run in real-time
    // mock_uart.DR = expected_data[0]; // First byte
    // call uart_receive
    // mock_uart.DR = expected_data[1]; // Second byte etc.
    //
    // For a simple mock of blocking receive, we ensure RXNE is set and then manually
    // fill DR for each expected read. The function will read mock_uart.DR `size` times.
    // To ensure the correct data is "received", we'll directly assign to DR in a way
    // that allows the for loop to consume them.
    // The most realistic mock would involve a call_count for DR reads, and a return queue.
    // For this level of mock, we'll assume RXNE is always high, and DR is read.
    // To test `data[i] = (uint8_t)uart->DR;`, we can pre-load DR with the last expected byte.
    // Or we can assume RXNE is true and that the `data` buffer will be populated correctly.
    // The prompt says "Match assertions to source behavior".

    // More realistic mock for multiple bytes:
    // We can't directly mock dynamic DR changes in a simple setUp/tearDown structure.
    // The best approach for this specific test case (multiple bytes success) is to
    // assume that if `uart->SR & USART_SR_RXNE` is true, then `uart->DR` *will* contain
    // the next byte. Since we cannot dynamically change `mock_uart.DR` within the `uart_receive`
    // function's execution from the test, we simplify by focusing on whether the buffer
    // is populated for the expected size.
    // For a blocking read, the function polls RXNE. If RXNE is high, it reads DR.
    // To pass this, we'll set the RXNE flag and simply check the `size` of data received.
    // The content will be tricky without an advanced mock.
    // A simplified test for multiple bytes assumes the underlying hardware provides the bytes
    // and checks if the loop correctly processes `size` bytes.

    // Let's create a queue for DR reads if we need to verify byte-by-byte for multiple reads.
    // This goes beyond simple `memset` setup. For this problem, I'll simplify `uart_receive`
    // multiple bytes to ensure it *attempts* to read `size` times if RXNE is available.
    // A simple mock of mock_uart.DR for a "single read" of the last byte is more feasible
    // than a multi-byte queue for DR without changing `uart_receive` itself to use a mock
    // layer or creating a more elaborate register mock which requires function pointers.

    // A better approach for multiple bytes is to use a "DR read callback" in the mock,
    // but that's not allowed by the prompt's `memset` based stub setup.
    // So for multiple bytes, if RXNE is true, the loop will run `size` times.
    // We cannot verify `data[i]` for each `i` without a dynamic DR.
    // The crucial check here is that `ERROR_NONE` is returned and `size` bytes are
    // "processed" from `DR` and placed into `data`.
    // Let's make `uart_receive` write known values to DR for testing the loop.
    mock_uart.SR = USART_SR_RXNE; // RXNE is always set.
    mock_uart.DR = 0xAA; // Default value read from DR, assuming it's available.
    error_t result = uart_receive(&mock_uart, rx_buffer, size, 1);
    // Expected: Function should return ERROR_NONE.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: All bytes in the buffer should be filled with the last value of DR.
    // This is a limitation of this mock approach, but it proves the loop runs.
    TEST_ASSERT_EQUAL_HEX(0xAA, rx_buffer[0]);
    TEST_ASSERT_EQUAL_HEX(0xAA, rx_buffer[1]);
    TEST_ASSERT_EQUAL_HEX(0xAA, rx_buffer[2]);
}


// Test uart_receive with timeout waiting for RXNE
void test_uart_receive_timeout_rxne(void) {
    uint8_t rx_buffer[1];
    // Simulate RXNE flag never being set (it's 0)
    mock_uart.SR = 0x0U;
    error_t result = uart_receive(&mock_uart, rx_buffer, 1, 0); // Timeout 0, fails on first check
    // Expected: Function should return ERROR_TIMEOUT if RXNE is not set within timeout.
    TEST_ASSERT_EQUAL_INT(ERROR_TIMEOUT, result);
    // Expected: The receive buffer should remain unchanged.
    TEST_ASSERT_EQUAL_HEX(0x0U, rx_buffer[0]);
}

// ====================================================================================
// --- SPI Functions Tests ---
// ====================================================================================

// Test spi_init with NULL spi pointer
void test_spi_init_null_spi_param(void) {
    spi_config_t config = {0};
    error_t result = spi_init(NULL, &config);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL spi pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test spi_init with NULL config pointer
void test_spi_init_null_config_param(void) {
    error_t result = spi_init(&mock_spi, NULL);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL config pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test spi_init with basic configuration (8-bit, mode 0, full-duplex, software NSS, no DMA)
void test_spi_init_basic_8bit_mode0_full_duplex(void) {
    spi_config_t config = {
        .baud_rate = 0, // PCLK/2
        .data_size = 8,
        .mode = 0,      // CPOL=0, CPHA=0
        .direction = 0, // Full-duplex
        .nss_mode = false,
        .dma_enable = false
    };
    error_t result = spi_init(&mock_spi, &config);
    // Expected: Function should return ERROR_NONE for valid parameters.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: CR1 bits: baud_rate=0 (bits 30.0f), data_size=7 (8-bit, bits 80.0f), mode=0 (bits 00.0f), direction=0 (bits 140.0f).
    // CR1 = (0<<3) | ((80.0f)<<8) | (0<<0) | (0<<14) = 0x700
    TEST_ASSERT_EQUAL_HEX(0x700U, mock_spi.CR1 & ~SPI_CR1_SPE); // Exclude SPE for now
    // Expected: CR2 should have SSOE, TXDMAEN, RXDMAEN cleared.
    TEST_ASSERT_EQUAL_HEX(0x0U, mock_spi.CR2);
    // Expected: CR1 should have SPE bit set (SPI Enabled).
    TEST_ASSERT_TRUE(mock_spi.CR1 & SPI_CR1_SPE);
}

// Test spi_init with advanced configuration (16-bit, mode 3, RX-only, hardware NSS, DMA enabled)
void test_spi_init_advanced_16bit_mode3_rx_only_hw_nss_dma(void) {
    spi_config_t config = {
        .baud_rate = 7, // PCLK/256
        .data_size = 16,
        .mode = 3,      // CPOL=1, CPHA=1
        .direction = 1, // Receive-only
        .nss_mode = true,
        .dma_enable = true
    };
    error_t result = spi_init(&mock_spi, &config);
    // Expected: Function should return ERROR_NONE for valid parameters.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: CR1 bits: baud_rate=7 (111b at bits 30.0f), data_size=15 (16-bit, 11b at bits 80.0f), mode=3 (11b at bits 00.0f), direction=1 (01b at bits 140.0f).
    // CR1 = (7<<3) | ((160.0f)<<8) | (3<<0) | (1<<14)
    // CR1 = (0x38) | (0xF00) | (0x3) | (0x4000) = 0x4F3B
    TEST_ASSERT_EQUAL_HEX(0x4F3BU, mock_spi.CR1 & ~SPI_CR1_SPE);
    // Expected: CR2 should have SSOE, TXDMAEN, RXDMAEN set.
    // CR2 = SPI_CR2_SSOE | SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN = 0x7
    TEST_ASSERT_EQUAL_HEX(SPI_CR2_SSOE | SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN, mock_spi.CR2);
    // Expected: CR1 should have SPE bit set.
    TEST_ASSERT_TRUE(mock_spi.CR1 & SPI_CR1_SPE);
}

// Test spi_transmit_receive with NULL spi pointer
void test_spi_transmit_receive_null_spi_param(void) {
    uint8_t tx_data[] = {0x00};
    uint8_t rx_data[1];
    error_t result = spi_transmit_receive(NULL, tx_data, rx_data, 1);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL spi pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test spi_transmit_receive with NULL tx_data pointer
void test_spi_transmit_receive_null_tx_data_param(void) {
    uint8_t rx_data[1];
    error_t result = spi_transmit_receive(&mock_spi, NULL, rx_data, 1);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL tx_data pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test spi_transmit_receive with NULL rx_data pointer
void test_spi_transmit_receive_null_rx_data_param(void) {
    uint8_t tx_data[] = {0x00};
    error_t result = spi_transmit_receive(&mock_spi, tx_data, NULL, 1);
    // Expected: Function should return ERROR_INVALID_PARAM for a NULL rx_data pointer.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test spi_transmit_receive with zero size
void test_spi_transmit_receive_zero_size_param(void) {
    uint8_t tx_data[] = {0x00};
    uint8_t rx_data[1];
    error_t result = spi_transmit_receive(&mock_spi, tx_data, rx_data, 0);
    // Expected: Function should return ERROR_INVALID_PARAM for zero size.
    TEST_ASSERT_EQUAL_INT(ERROR_INVALID_PARAM, result);
}

// Test spi_transmit_receive for a single byte successfully
void test_spi_transmit_receive_single_byte_success(void) {
    uint8_t tx_data[] = {0xA5};
    uint8_t rx_data[1] = {0x00}; // Initialize to know it changes
    uint8_t expected_rx_byte = 0x5A;

    // Simulate TXE and RXNE are always set for success, and DR contains the expected rx_byte
    mock_spi.SR = SPI_SR_TXE | SPI_SR_RXNE;
    mock_spi.DR = expected_rx_byte; // This will be read by rx_data[0]

    error_t result = spi_transmit_receive(&mock_spi, tx_data, rx_data, 1);
    // Expected: Function should return ERROR_NONE for successful transfer.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: Data Register should have received the transmit byte.
    TEST_ASSERT_EQUAL_HEX(tx_data[0], mock_spi.DR);
    // Expected: Receive buffer should contain the expected received byte.
    TEST_ASSERT_EQUAL_HEX(expected_rx_byte, rx_data[0]);
}

// Test spi_transmit_receive for multiple bytes successfully
void test_spi_transmit_receive_multiple_bytes_success(void) {
    uint8_t tx_data[] = {0x01, 0x02, 0x03};
    uint8_t rx_data[3] = {0x00, 0x00, 0x00};
    uint8_t expected_rx_data[] = {0x10, 0x20, 0x30}; // Simulate received bytes

    // Simulate TXE and RXNE flags are set for all iterations.
    mock_spi.SR = SPI_SR_TXE | SPI_SR_RXNE;

    // For simplicity in this mock, we'll make the mock_spi.DR return the expected_rx_data
    // This is a simplification; a more advanced mock would use a function pointer or queue.
    // The loop iterates `size` times, writes `tx_data[i]` to `spi->DR`, and reads from `spi->DR`.
    // Since we can only set `mock_spi.DR` once before the function call, we cannot perfectly simulate
    // individual byte reads in a simple struct mock.
    // To pass this test meaningfully, we ensure the loop runs and fills the rx_data buffer.
    // We will assume `mock_spi.DR` provides the correct `expected_rx_data[i]` in a real scenario
    // for each iteration. Here, we'll set `mock_spi.DR` to the LAST expected value and check `rx_data`.
    // This highlights a limitation of simple struct mocks for streaming data.

    // A better way to approach this specific test would be to have mock_spi.DR
    // store the last value written, and then a separate mechanism to control
    // the value returned when `spi->DR` is *read*. Without this, we simplify:
    // The `spi_transmit_receive` loop writes `tx_data[i]` to `spi->DR` and reads from `spi->DR`.
    // If we want to assert specific `rx_data` values, we need a way for `mock_spi.DR`
    // to "return" different values on successive reads. Since `mock_spi.DR` is a single `uint32_t`,
    // this is not possible without an advanced mocking mechanism (e.g., custom getter/setter).

    // Given the constraints of the prompt, we assume success if the call returns ERROR_NONE,
    // and for `rx_data` we must acknowledge `mock_spi.DR` is a single value, so `rx_data`
    // would be filled with the last value set to `mock_spi.DR` during the final iteration of the loop.
    // Let's modify `mock_spi.DR` to provide a consistent value for testing that the loop reads.
    mock_spi.DR = 0xFE; // Simulate fixed received data

    error_t result = spi_transmit_receive(&mock_spi, tx_data, rx_data, 3);
    // Expected: Function should return ERROR_NONE.
    TEST_ASSERT_EQUAL_INT(ERROR_NONE, result);
    // Expected: Last transmitted byte should be in DR.
    TEST_ASSERT_EQUAL_HEX(tx_data[2], mock_spi.DR);
    // Expected: All received bytes should be filled with the mocked DR value (0xFE).
    TEST_ASSERT_EQUAL_HEX(0xFE, rx_data[0]);
    TEST_ASSERT_EQUAL_HEX(0xFE, rx_data[1]);
    TEST_ASSERT_EQUAL_HEX(0xFE, rx_data[2]);
}

// ====================================================================================
// --- Main function to run all tests ---
// ====================================================================================



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_gpio_init_null_gpio_param);
    RUN_TEST(test_gpio_init_null_config_param);
    RUN_TEST(test_gpio_init_input_mode_no_pull_low_speed);
    RUN_TEST(test_gpio_init_output_mode_pull_up_medium_speed);
    RUN_TEST(test_gpio_init_af_mode_pull_down_very_high_speed_af4_pin0);
    RUN_TEST(test_gpio_init_analog_mode_af15_pin15);
    RUN_TEST(test_gpio_write_pin_null_gpio_param);
    RUN_TEST(test_gpio_write_pin_invalid_pin_high);
    RUN_TEST(test_gpio_write_pin_set_pin_5);
    RUN_TEST(test_gpio_write_pin_reset_pin_12);
    RUN_TEST(test_gpio_read_pin_null_gpio_param);
    RUN_TEST(test_gpio_read_pin_invalid_pin_high);
    RUN_TEST(test_gpio_read_pin_is_high);
    RUN_TEST(test_gpio_read_pin_is_low);
    RUN_TEST(test_uart_init_null_uart_param);
    RUN_TEST(test_uart_init_null_config_param);
    RUN_TEST(test_uart_init_8n1);
    RUN_TEST(test_uart_init_9e2);
    RUN_TEST(test_uart_init_8o1_5);
    RUN_TEST(test_uart_transmit_null_uart_param);
    RUN_TEST(test_uart_transmit_null_data_param);
    RUN_TEST(test_uart_transmit_zero_size_param);
    RUN_TEST(test_uart_transmit_single_byte_success);
    RUN_TEST(test_uart_transmit_multiple_bytes_success);
    RUN_TEST(test_uart_transmit_timeout_txe);
    RUN_TEST(test_uart_transmit_timeout_tc);
    RUN_TEST(test_uart_receive_null_uart_param);
    RUN_TEST(test_uart_receive_null_data_param);
    RUN_TEST(test_uart_receive_zero_size_param);
    RUN_TEST(test_uart_receive_single_byte_success);
    RUN_TEST(test_uart_receive_multiple_bytes_success);
    RUN_TEST(test_uart_receive_timeout_rxne);
    RUN_TEST(test_spi_init_null_spi_param);
    RUN_TEST(test_spi_init_null_config_param);
    RUN_TEST(test_spi_init_basic_8bit_mode0_full_duplex);
    RUN_TEST(test_spi_init_advanced_16bit_mode3_rx_only_hw_nss_dma);
    RUN_TEST(test_spi_transmit_receive_null_spi_param);
    RUN_TEST(test_spi_transmit_receive_null_tx_data_param);
    RUN_TEST(test_spi_transmit_receive_null_rx_data_param);
    RUN_TEST(test_spi_transmit_receive_zero_size_param);
    RUN_TEST(test_spi_transmit_receive_single_byte_success);
    RUN_TEST(test_spi_transmit_receive_multiple_bytes_success);

    return UNITY_END();
}