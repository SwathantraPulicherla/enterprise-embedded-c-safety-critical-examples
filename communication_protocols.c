#include "communication_protocols.h"
#include <stdlib.h>
#include <string.h>

// CAN Functions
protocol_error_t can_init(can_handle_t *can, uint16_t buffer_size) {
    if (can == NULL || buffer_size == 0) {
        return PROTOCOL_ERROR_INVALID_HEADER;
    }

    can->rx_buffer = (can_frame_t*)malloc(buffer_size * sizeof(can_frame_t));
    can->tx_buffer = (can_frame_t*)malloc(buffer_size * sizeof(can_frame_t));

    if (can->rx_buffer == NULL || can->tx_buffer == NULL) {
        free(can->rx_buffer);
        free(can->tx_buffer);
        return PROTOCOL_ERROR_BUFFER_OVERFLOW;
    }

    can->rx_head = 0;
    can->rx_tail = 0;
    can->tx_head = 0;
    can->tx_tail = 0;
    can->buffer_size = buffer_size;
    can->state = PROTOCOL_STATE_IDLE;

    return PROTOCOL_ERROR_NONE;
}

protocol_error_t can_transmit_message(can_handle_t *can, const can_frame_t *frame, uint32_t timeout) {
    if (can == NULL || frame == NULL) {
        return PROTOCOL_ERROR_INVALID_HEADER;
    }

    // Check if buffer is full (simplified)
    uint16_t next_head = (can->tx_head + 1) % can->buffer_size;
    if (next_head == can->tx_tail) {
        return PROTOCOL_ERROR_BUFFER_OVERFLOW;
    }

    // Copy frame to buffer
    memcpy(&can->tx_buffer[can->tx_head], frame, sizeof(can_frame_t));
    can->tx_head = next_head;

    // Simulate transmission (in real implementation, this would trigger hardware)
    can->state = PROTOCOL_STATE_PROCESSING;

    return PROTOCOL_ERROR_NONE;
}

protocol_error_t can_receive_message(can_handle_t *can, can_frame_t *frame, uint32_t timeout) {
    if (can == NULL || frame == NULL) {
        return PROTOCOL_ERROR_INVALID_HEADER;
    }

    if (can->rx_head == can->rx_tail) {
        return PROTOCOL_ERROR_TIMEOUT;  // No data available
    }

    // Copy frame from buffer
    memcpy(frame, &can->rx_buffer[can->rx_tail], sizeof(can_frame_t));
    can->rx_tail = (can->rx_tail + 1) % can->buffer_size;

    return PROTOCOL_ERROR_NONE;
}

uint16_t can_calculate_crc(const can_frame_t *frame) {
    if (frame == NULL) return 0;

    uint16_t crc = 0xFFFF;
    uint8_t *data = (uint8_t*)frame;

    for (size_t i = 0; i < sizeof(can_frame_t); i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

// Ethernet Functions
protocol_error_t ethernet_parse_frame(const uint8_t *data, uint16_t length, ethernet_frame_t *frame) {
    if (data == NULL || frame == NULL || length < 14) {
        return PROTOCOL_ERROR_INVALID_HEADER;
    }

    memcpy(frame->destination, data, 6);
    memcpy(frame->source, data + 6, 6);
    frame->ethertype = (data[12] << 8) | data[13];

    uint16_t payload_length = length - 14;
    if (payload_length > sizeof(frame->payload)) {
        return PROTOCOL_ERROR_BUFFER_OVERFLOW;
    }

    memcpy(frame->payload, data + 14, payload_length);
    frame->crc = 0;  // Would calculate actual CRC

    return PROTOCOL_ERROR_NONE;
}

uint32_t ethernet_calculate_crc(const ethernet_frame_t *frame) {
    if (frame == NULL) return 0;

    // Simplified CRC calculation (real implementation would use proper polynomial)
    uint32_t crc = 0xFFFFFFFF;
    uint8_t *data = (uint8_t*)frame;

    for (size_t i = 0; i < sizeof(ethernet_frame_t) - sizeof(uint32_t); i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xEDB88320;
            } else {
                crc >>= 1;
            }
        }
    }

    return ~crc;
}

// Protocol Message Functions
protocol_error_t protocol_parse_message(const uint8_t *data, uint16_t length, protocol_message_t *message) {
    if (data == NULL || message == NULL || length < 5) {
        return PROTOCOL_ERROR_INVALID_HEADER;
    }

    memcpy(message->raw_bytes, data, length < sizeof(message->raw_bytes) ? length : sizeof(message->raw_bytes));

    // Validate header
    if (message->packet.header != 0xAA) {
        return PROTOCOL_ERROR_INVALID_HEADER;
    }

    // Check data length
    if (message->packet.data_length > sizeof(message->packet.payload)) {
        return PROTOCOL_ERROR_BUFFER_OVERFLOW;
    }

    return PROTOCOL_ERROR_NONE;
}

protocol_error_t protocol_validate_message(const protocol_message_t *message) {
    if (message == NULL) {
        return PROTOCOL_ERROR_INVALID_HEADER;
    }

    // Validate CRC
    uint16_t calculated_crc = protocol_calculate_crc(message->raw_bytes,
                                                     4 + message->packet.data_length);
    if (calculated_crc != message->packet.crc) {
        return PROTOCOL_ERROR_CRC_MISMATCH;
    }

    return PROTOCOL_ERROR_NONE;
}

uint16_t protocol_calculate_crc(const uint8_t *data, uint16_t length) {
    if (data == NULL) return 0;

    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}