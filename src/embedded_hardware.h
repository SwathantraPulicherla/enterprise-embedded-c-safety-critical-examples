#ifndef EMBEDDED_HARDWARE_H
#define EMBEDDED_HARDWARE_H

#include <stdint.h>
#include <stdbool.h>

// Register bit definitions (simplified STM32-style)
#define USART_CR1_UE     (1U << 13)
#define USART_CR1_M      (1U << 12)
#define USART_CR1_PCE    (1U << 10)
#define USART_CR1_PS     (1U << 9)

#define USART_CR2_STOP_0 (1U << 12)
#define USART_CR2_STOP_1 (1U << 13)
#define USART_CR2_STOP   (USART_CR2_STOP_0 | USART_CR2_STOP_1)

#define USART_SR_TXE     (1U << 7)
#define USART_SR_TC      (1U << 6)
#define USART_SR_RXNE    (1U << 5)

#define SPI_CR1_SPE      (1U << 6)
#define SPI_CR2_SSOE     (1U << 2)
#define SPI_CR2_TXDMAEN  (1U << 1)
#define SPI_CR2_RXDMAEN  (1U << 0)

#define SPI_SR_TXE       (1U << 1)
#define SPI_SR_RXNE      (1U << 0)

// Hardware register structures based on STM32-like peripherals

// GPIO Register Structure (from STM32 datasheet)
typedef struct {
    volatile uint32_t MODER;    // Mode register
    volatile uint32_t OTYPER;   // Output type register
    volatile uint32_t OSPEEDR;  // Output speed register
    volatile uint32_t PUPDR;    // Pull-up/pull-down register
    volatile uint32_t IDR;      // Input data register
    volatile uint32_t ODR;      // Output data register
    volatile uint32_t BSRR;     // Bit set/reset register
    volatile uint32_t LCKR;     // Configuration lock register
    volatile uint32_t AFR[2];   // Alternate function registers
} GPIO_TypeDef;

// UART Register Structure
typedef struct {
    volatile uint32_t SR;       // Status register
    volatile uint32_t DR;       // Data register
    volatile uint32_t BRR;      // Baud rate register
    volatile uint32_t CR1;      // Control register 1
    volatile uint32_t CR2;      // Control register 2
    volatile uint32_t CR3;      // Control register 3
    volatile uint32_t GTPR;     // Guard time and prescaler register
} USART_TypeDef;

// SPI Register Structure
typedef struct {
    volatile uint32_t CR1;      // Control register 1
    volatile uint32_t CR2;      // Control register 2
    volatile uint32_t SR;       // Status register
    volatile uint32_t DR;       // Data register
    volatile uint32_t CRCPR;    // CRC polynomial register
    volatile uint32_t RXCRCR;   // RX CRC register
    volatile uint32_t TXCRCR;   // TX CRC register
    volatile uint32_t I2SCFGR;  // I2S configuration register
    volatile uint32_t I2SPR;    // I2S prescaler register
} SPI_TypeDef;

// DMA Channel Structure
typedef struct {
    volatile uint32_t CCR;      // Configuration register
    volatile uint32_t CNDTR;    // Number of data register
    volatile uint32_t CPAR;     // Peripheral address register
    volatile uint32_t CMAR;     // Memory address register
} DMA_Channel_TypeDef;

// Configuration structures
typedef struct {
    uint8_t mode;               // GPIO mode (input, output, etc.)
    uint8_t pull;               // Pull-up/pull-down
    uint8_t speed;              // Output speed
    uint8_t alternate;          // Alternate function
} gpio_config_t;

typedef struct {
    uint32_t baud_rate;         // Baud rate
    uint8_t data_bits;          // Data bits (7, 8, 9)
    uint8_t stop_bits;          // Stop bits
    uint8_t parity;             // Parity (none, even, odd)
    bool flow_control;          // Hardware flow control
} uart_config_t;

typedef struct {
    uint32_t baud_rate;         // SPI baud rate
    uint8_t data_size;          // Data size (4-16 bits)
    uint8_t mode;               // SPI mode (0-3)
    uint8_t direction;          // Direction (1/2 line)
    bool nss_mode;              // NSS mode
    bool dma_enable;            // DMA enable
} spi_config_t;

// Error codes
typedef enum {
    ERROR_NONE = 0,
    ERROR_INVALID_PARAM,
    ERROR_TIMEOUT,
    ERROR_BUSY,
    ERROR_OVERFLOW
} error_t;

// Function declarations
error_t gpio_init(GPIO_TypeDef *gpio, gpio_config_t *config);
error_t gpio_write_pin(GPIO_TypeDef *gpio, uint16_t pin, bool state);
bool gpio_read_pin(GPIO_TypeDef *gpio, uint16_t pin);

error_t uart_init(USART_TypeDef *uart, uart_config_t *config);
error_t uart_transmit(USART_TypeDef *uart, uint8_t *data, uint16_t size, uint32_t timeout);
error_t uart_receive(USART_TypeDef *uart, uint8_t *data, uint16_t size, uint32_t timeout);

error_t spi_init(SPI_TypeDef *spi, spi_config_t *config);
error_t spi_transmit_receive(SPI_TypeDef *spi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size);

#endif // EMBEDDED_HARDWARE_H