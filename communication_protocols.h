#ifndef COMMUNICATION_PROTOCOLS_H
#define COMMUNICATION_PROTOCOLS_H

#include <stdint.h>
#include <stdbool.h>

// CAN Bus Message Structure with bit fields
typedef struct {
    uint32_t id : 29;        // Standard identifier (11 bits) or Extended (29 bits)
    uint32_t rtr : 1;        // Remote transmission request
    uint32_t ide : 1;        // Identifier extension bit
    uint32_t rsvd : 1;       // Reserved bit
    uint8_t data[8];         // Data field (up to 8 bytes)
    uint8_t dlc : 4;         // Data length code (0-8)
    uint32_t timestamp;      // Reception timestamp
} can_frame_t;

// CAN Identifier Union for flexible access
typedef union {
    struct {
        uint32_t identifier : 29;
        uint32_t rtr : 1;
        uint32_t ide : 1;
        uint32_t rsvd : 1;
    } bits;
    uint32_t raw;            // Raw 32-bit access
} can_identifier_t;

// Ethernet Frame Structure
typedef struct {
    uint8_t destination[6];  // Destination MAC address
    uint8_t source[6];       // Source MAC address
    uint16_t ethertype;      // EtherType or length
    uint8_t payload[1500];   // Payload data
    uint32_t crc;            // CRC checksum
} ethernet_frame_t;

// UART Protocol Message Union
typedef union {
    struct {
        uint8_t header;      // Message header
        uint8_t command;     // Command byte
        uint16_t data_length; // Data length
        uint8_t payload[256]; // Payload data
        uint16_t crc;        // CRC checksum
    } packet;
    uint8_t raw_bytes[262];  // Raw byte access
} protocol_message_t;

// Protocol States
typedef enum {
    PROTOCOL_STATE_IDLE,
    PROTOCOL_STATE_RECEIVING,
    PROTOCOL_STATE_PROCESSING,
    PROTOCOL_STATE_ERROR,
    PROTOCOL_STATE_COMPLETE
} protocol_state_t;

// Error codes for protocols
typedef enum {
    PROTOCOL_ERROR_NONE = 0,
    PROTOCOL_ERROR_INVALID_HEADER,
    PROTOCOL_ERROR_CRC_MISMATCH,
    PROTOCOL_ERROR_BUFFER_OVERFLOW,
    PROTOCOL_ERROR_TIMEOUT
} protocol_error_t;

// CAN Handle Structure
typedef struct {
    can_frame_t *rx_buffer;
    can_frame_t *tx_buffer;
    uint16_t rx_head;
    uint16_t rx_tail;
    uint16_t tx_head;
    uint16_t tx_tail;
    uint16_t buffer_size;
    protocol_state_t state;
} can_handle_t;

// Function declarations
protocol_error_t can_init(can_handle_t *can, uint16_t buffer_size);
protocol_error_t can_transmit_message(can_handle_t *can, const can_frame_t *frame, uint32_t timeout);
protocol_error_t can_receive_message(can_handle_t *can, can_frame_t *frame, uint32_t timeout);
uint16_t can_calculate_crc(const can_frame_t *frame);

protocol_error_t ethernet_parse_frame(const uint8_t *data, uint16_t length, ethernet_frame_t *frame);
uint32_t ethernet_calculate_crc(const ethernet_frame_t *frame);

protocol_error_t protocol_parse_message(const uint8_t *data, uint16_t length, protocol_message_t *message);
protocol_error_t protocol_validate_message(const protocol_message_t *message);
uint16_t protocol_calculate_crc(const uint8_t *data, uint16_t length);

#endif // COMMUNICATION_PROTOCOLS_H