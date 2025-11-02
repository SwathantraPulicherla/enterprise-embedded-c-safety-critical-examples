#include "embedded_hardware.h"

// GPIO Functions
error_t gpio_init(GPIO_TypeDef *gpio, gpio_config_t *config) {
    if (gpio == NULL || config == NULL) {
        return ERROR_INVALID_PARAM;
    }

    // Configure mode
    uint32_t moder = gpio->MODER;
    moder &= ~(0x3U << (config->mode * 2));  // Clear bits
    moder |= (config->mode << (config->mode * 2));  // Set mode
    gpio->MODER = moder;

    // Configure pull-up/pull-down
    uint32_t pupdr = gpio->PUPDR;
    pupdr &= ~(0x3U << (config->pull * 2));
    pupdr |= (config->pull << (config->pull * 2));
    gpio->PUPDR = pupdr;

    // Configure speed
    uint32_t ospeedr = gpio->OSPEEDR;
    ospeedr &= ~(0x3U << (config->speed * 2));
    ospeedr |= (config->speed << (config->speed * 2));
    gpio->OSPEEDR = ospeedr;

    // Configure alternate function if needed
    if (config->mode == 2 || config->mode == 3) {  // AF mode
        uint32_t afr_index = config->alternate / 8;
        uint32_t afr_shift = (config->alternate % 8) * 4;
        gpio->AFR[afr_index] &= ~(0xFU << afr_shift);
        gpio->AFR[afr_index] |= (config->alternate << afr_shift);
    }

    return ERROR_NONE;
}

error_t gpio_write_pin(GPIO_TypeDef *gpio, uint16_t pin, bool state) {
    if (gpio == NULL || pin > 15) {
        return ERROR_INVALID_PARAM;
    }

    if (state) {
        gpio->BSRR = (1U << pin);  // Set pin
    } else {
        gpio->BSRR = (1U << (pin + 16));  // Reset pin
    }

    return ERROR_NONE;
}

bool gpio_read_pin(GPIO_TypeDef *gpio, uint16_t pin) {
    if (gpio == NULL || pin > 15) {
        return false;
    }
    return (gpio->IDR & (1U << pin)) != 0;
}

// UART Functions
error_t uart_init(USART_TypeDef *uart, uart_config_t *config) {
    if (uart == NULL || config == NULL) {
        return ERROR_INVALID_PARAM;
    }

    // Configure baud rate
    uart->BRR = config->baud_rate;

    // Configure data bits, parity, stop bits
    uint32_t cr1 = uart->CR1;
    cr1 &= ~(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS);
    if (config->data_bits == 9) {
        cr1 |= USART_CR1_M;
    }
    if (config->parity != 0) {
        cr1 |= USART_CR1_PCE;
        if (config->parity == 2) {  // Odd parity
            cr1 |= USART_CR1_PS;
        }
    }
    uart->CR1 = cr1;

    // Configure stop bits
    uint32_t cr2 = uart->CR2;
    cr2 &= ~USART_CR2_STOP;
    cr2 |= (config->stop_bits << 12);
    uart->CR2 = cr2;

    // Enable UART
    uart->CR1 |= USART_CR1_UE;

    return ERROR_NONE;
}

error_t uart_transmit(USART_TypeDef *uart, uint8_t *data, uint16_t size, uint32_t timeout) {
    if (uart == NULL || data == NULL || size == 0) {
        return ERROR_INVALID_PARAM;
    }

    for (uint16_t i = 0; i < size; i++) {
        // Wait for TXE flag
        uint32_t ticks = 0;
        while (!(uart->SR & USART_SR_TXE)) {
            if (ticks++ > timeout) {
                return ERROR_TIMEOUT;
            }
        }
        uart->DR = data[i];
    }

    // Wait for transmission complete
    uint32_t ticks = 0;
    while (!(uart->SR & USART_SR_TC)) {
        if (ticks++ > timeout) {
            return ERROR_TIMEOUT;
        }
    }

    return ERROR_NONE;
}

error_t uart_receive(USART_TypeDef *uart, uint8_t *data, uint16_t size, uint32_t timeout) {
    if (uart == NULL || data == NULL || size == 0) {
        return ERROR_INVALID_PARAM;
    }

    for (uint16_t i = 0; i < size; i++) {
        // Wait for RXNE flag
        uint32_t ticks = 0;
        while (!(uart->SR & USART_SR_RXNE)) {
            if (ticks++ > timeout) {
                return ERROR_TIMEOUT;
            }
        }
        data[i] = (uint8_t)uart->DR;
    }

    return ERROR_NONE;
}

// SPI Functions
error_t spi_init(SPI_TypeDef *spi, spi_config_t *config) {
    if (spi == NULL || config == NULL) {
        return ERROR_INVALID_PARAM;
    }

    // Configure CR1
    uint32_t cr1 = 0;
    cr1 |= (config->baud_rate << 3);  // Baud rate
    cr1 |= (config->data_size - 1) << 8;  // Data size
    cr1 |= config->mode << 0;  // CPOL/CPHA
    cr1 |= config->direction << 14;  // Direction
    spi->CR1 = cr1;

    // Configure CR2
    uint32_t cr2 = 0;
    if (config->nss_mode) {
        cr2 |= SPI_CR2_SSOE;
    }
    if (config->dma_enable) {
        cr2 |= SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN;
    }
    spi->CR2 = cr2;

    // Enable SPI
    spi->CR1 |= SPI_CR1_SPE;

    return ERROR_NONE;
}

error_t spi_transmit_receive(SPI_TypeDef *spi, uint8_t *tx_data, uint8_t *rx_data, uint16_t size) {
    if (spi == NULL || tx_data == NULL || rx_data == NULL || size == 0) {
        return ERROR_INVALID_PARAM;
    }

    for (uint16_t i = 0; i < size; i++) {
        // Wait for TXE
        while (!(spi->SR & SPI_SR_TXE));

        // Send data
        spi->DR = tx_data[i];

        // Wait for RXNE
        while (!(spi->SR & SPI_SR_RXNE));

        // Receive data
        rx_data[i] = (uint8_t)spi->DR;
    }

    return ERROR_NONE;
}