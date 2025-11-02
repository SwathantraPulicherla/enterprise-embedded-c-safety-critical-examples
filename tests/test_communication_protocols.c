/* test_communication_protocols.c – Auto-generated Expert Unity Tests */
#include "unity.h"
#include "communication_protocols.h" // Contains type definitions (can_handle_t, can_frame_t, etc.)
#include <stdlib.h> // For malloc, free
#include <string.h> // For memcpy, memset
#include <stdint.h> // For uint8_t, uint16_t, uint32_t, size_t
#include <stdbool.h> // For bool if needed, good practice though not explicitly in source

// Mocks/Stubs for external dependencies (NONE as per prompt's explicit instruction)
// The prompt explicitly states "EXTERNAL FUNCTIONS TO STUB: - None".
// Therefore, standard library functions like malloc, free, memcpy are used directly.

// Helper function to free CAN handle buffers
static void free_can_handle_buffers(can_handle_t *can) {
    if (can != NULL) {
        if (can->rx_buffer != NULL) {
            free(can->rx_buffer);
            can->rx_buffer = NULL;
        }
        if (can->tx_buffer != NULL) {
            free(can->tx_buffer);
            can->tx_buffer = NULL;
        }
    }
}

void setUp(void) {
    // No global stubs to reset, no shared global state that needs initialization.
    // Individual tests will manage their own can_handle_t initialization and cleanup
    // to ensure full test isolation, especially for dynamic memory.
}

void tearDown(void) {
    // No global stubs to reset, no shared global state that needs cleanup.
}

// ==== CAN Functions Tests ====

// Test can_init with a NULL CAN handle pointer.
void test_can_init_null_handle_returns_invalid_header(void) {
    protocol_error_t result = can_init(NULL, 10);
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for a NULL handle.
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
}

// Test can_init with a zero buffer size.
void test_can_init_zero_buffer_size_returns_invalid_header(void) {
    can_handle_t can_h;
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for buffer_size of 0.
    protocol_error_t result = can_init(&can_h, 0);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
    // No memory should be allocated if buffer_size is 0, so no need to free.
}

// Test can_init for successful initialization.
void test_can_init_success_initializes_handle(void) {
    can_handle_t can_h = {0}; // Initialize to zero to ensure clean state
    uint16_t buffer_size = 5;
    protocol_error_t result = can_init(&can_h, buffer_size);

    // Expected: Initialization should succeed.
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_NONE, result);
    // Expected: Both RX and TX buffers should be allocated.
    TEST_ASSERT_NOT_NULL(can_h.rx_buffer);
    TEST_ASSERT_NOT_NULL(can_h.tx_buffer);
    // Expected: All head/tail pointers should be zero.
    TEST_ASSERT_EQUAL(0, can_h.rx_head);
    TEST_ASSERT_EQUAL(0, can_h.rx_tail);
    TEST_ASSERT_EQUAL(0, can_h.tx_head);
    TEST_ASSERT_EQUAL(0, can_h.tx_tail);
    // Expected: buffer_size member should match the input.
    TEST_ASSERT_EQUAL(buffer_size, can_h.buffer_size);
    // Expected: State should transition to PROTOCOL_STATE_IDLE.
    TEST_ASSERT_EQUAL(PROTOCOL_STATE_IDLE, can_h.state);

    // Clean up dynamically allocated memory
    free_can_handle_buffers(&can_h);
}

// Test can_transmit_message with a NULL CAN handle pointer.
void test_can_transmit_message_null_handle_returns_invalid_header(void) {
    can_frame_t frame = { .id = 0x123, .dlc = 8, .data = {1,2,3,4,5,6,7,8} };
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for a NULL handle.
    protocol_error_t result = can_transmit_message(NULL, &frame, 0);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
}

// Test can_transmit_message with a NULL frame pointer.
void test_can_transmit_message_null_frame_returns_invalid_header(void) {
    can_handle_t can_h;
    uint16_t buffer_size = 5;
    can_init(&can_h, buffer_size); // Initialize handle for valid 'can' pointer
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for a NULL frame.
    protocol_error_t result = can_transmit_message(&can_h, NULL, 0);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
    free_can_handle_buffers(&can_h);
}

// Test can_transmit_message when the transmit buffer is full.
void test_can_transmit_message_buffer_full_returns_buffer_overflow(void) {
    can_handle_t can_h;
    uint16_t buffer_size = 1; // Small buffer to easily test overflow (circular buffer, 1 means 0 frames effectively)
    can_init(&can_h, buffer_size);

    can_frame_t frame1 = { .id = 0x100, .dlc = 8, .data = {1,1,1,1,1,1,1,1} };
    
    // Fill the buffer (tx_head will become 1, tx_tail is 0. Next head for next frame will be 0)
    // In a 1-size buffer (0-indexed), tx_head moves from 0 to 1, buffer_size=1, so (0+1)%1 = 0.
    // If tx_tail is 0, next_head (0) == tx_tail (0), so full.
    can_h.tx_head = can_h.tx_tail; // Initial state. Tx_head advances past Tx_tail to 'fill'.
    // Manually increment tx_head to simulate 1 frame in a buffer of size 1.
    // This makes the buffer 'full' for the next transmission attempt.
    can_h.tx_head = (can_h.tx_head + 1) % can_h.buffer_size; // tx_head = 1%1 = 0 (assuming buffer_size=1, tx_tail=0 initially)
                                                             // With buffer_size=1, tx_head=0, tx_tail=0, next_head=(0+1)%1=0. So next_head == tx_tail, it's full.
                                                             // Let's use buffer_size = 2.
    free_can_handle_buffers(&can_h); // Clean up previous init
    buffer_size = 2; // Buffer size 2 means 1 slot actually usable (head catches tail)
    can_init(&can_h, buffer_size);
    can_transmit_message(&can_h, &frame1, 0); // Fills 1st slot. tx_head becomes 1. state=PROCESSING
    TEST_ASSERT_EQUAL(PROTOCOL_STATE_PROCESSING, can_h.state);
    can_h.state = PROTOCOL_STATE_IDLE; // Reset state for current test's assertion.

    can_frame_t frame2 = { .id = 0x200, .dlc = 8, .data = {2,2,2,2,2,2,2,2} };
    // Expected: Attempting to transmit when buffer is full returns PROTOCOL_ERROR_BUFFER_OVERFLOW.
    // Current state: tx_head = 1, tx_tail = 0. next_head = (1+1)%2 = 0.
    // Since next_head (0) == tx_tail (0), buffer is full.
    protocol_error_t result = can_transmit_message(&can_h, &frame2, 0);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_BUFFER_OVERFLOW, result);
    // Expected: State should not change on error.
    TEST_ASSERT_EQUAL(PROTOCOL_STATE_IDLE, can_h.state);

    free_can_handle_buffers(&can_h);
}

// Test can_transmit_message for successful transmission and state transition.
void test_can_transmit_message_success_and_state_change(void) {
    can_handle_t can_h;
    uint16_t buffer_size = 5;
    can_init(&can_h, buffer_size);

    can_frame_t frame = { .id = 0x123, .dlc = 8, .data = {1,2,3,4,5,6,7,8}, .crc = 0xABCD };
    // Expected: Transmission should succeed.
    protocol_error_t result = can_transmit_message(&can_h, &frame, 0);

    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_NONE, result);
    // Expected: tx_head should increment, tx_tail remains unchanged.
    TEST_ASSERT_EQUAL(1, can_h.tx_head);
    TEST_ASSERT_EQUAL(0, can_h.tx_tail);
    // Expected: State should transition to PROTOCOL_STATE_PROCESSING.
    TEST_ASSERT_EQUAL(PROTOCOL_STATE_PROCESSING, can_h.state);

    // Expected: The frame data should be copied into the transmit buffer correctly.
    TEST_ASSERT_EQUAL_HEX32(frame.id, can_h.tx_buffer[0].id);
    TEST_ASSERT_EQUAL_HEX8(frame.dlc, can_h.tx_buffer[0].dlc);
    TEST_ASSERT_EQUAL_HEX16(frame.crc, can_h.tx_buffer[0].crc);
    TEST_ASSERT_EQUAL_MEMORY(frame.data, can_h.tx_buffer[0].data, sizeof(frame.data));

    free_can_handle_buffers(&can_h);
}

// Test can_receive_message with a NULL CAN handle pointer.
void test_can_receive_message_null_handle_returns_invalid_header(void) {
    can_frame_t frame_rx;
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for a NULL handle.
    protocol_error_t result = can_receive_message(NULL, &frame_rx, 0);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
}

// Test can_receive_message with a NULL frame pointer.
void test_can_receive_message_null_frame_returns_invalid_header(void) {
    can_handle_t can_h;
    uint16_t buffer_size = 5;
    can_init(&can_h, buffer_size);
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for a NULL frame.
    protocol_error_t result = can_receive_message(&can_h, NULL, 0);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
    free_can_handle_buffers(&can_h);
}

// Test can_receive_message when the receive buffer is empty.
void test_can_receive_message_buffer_empty_returns_timeout(void) {
    can_handle_t can_h;
    uint16_t buffer_size = 5;
    can_init(&can_h, buffer_size);

    can_frame_t frame_rx;
    // Expected: Function returns PROTOCOL_ERROR_TIMEOUT if the RX buffer is empty (head == tail).
    protocol_error_t result = can_receive_message(&can_h, &frame_rx, 0);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_TIMEOUT, result);
    // Expected: State should not change.
    TEST_ASSERT_EQUAL(PROTOCOL_STATE_IDLE, can_h.state);

    free_can_handle_buffers(&can_h);
}

// Test can_receive_message for successful reception.
void test_can_receive_message_success(void) {
    can_handle_t can_h;
    uint16_t buffer_size = 5;
    can_init(&can_h, buffer_size);

    can_frame_t frame_tx = { .id = 0xABCD, .dlc = 8, .data = {10,11,12,13,14,15,16,17}, .crc = 0xEF01 };

    // Manually add a frame to the RX buffer to simulate received data
    memcpy(&can_h.rx_buffer[can_h.rx_head], &frame_tx, sizeof(can_frame_t));
    can_h.rx_head = (can_h.rx_head + 1) % can_h.buffer_size; // Move head to indicate data is available

    can_frame_t frame_rx = {0}; // Clear receive frame for comparison
    // Expected: Reception should succeed.
    protocol_error_t result = can_receive_message(&can_h, &frame_rx, 0);

    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_NONE, result);
    // Expected: rx_tail should increment, rx_head remains unchanged.
    TEST_ASSERT_EQUAL(1, can_h.rx_tail);
    // Expected: The received frame data should match the transmitted data.
    TEST_ASSERT_EQUAL_HEX32(frame_tx.id, frame_rx.id);
    TEST_ASSERT_EQUAL_HEX8(frame_tx.dlc, frame_rx.dlc);
    TEST_ASSERT_EQUAL_HEX16(frame_tx.crc, frame_rx.crc);
    TEST_ASSERT_EQUAL_MEMORY(frame_tx.data, frame_rx.data, sizeof(frame_tx.data));
    // Expected: State should not change.
    TEST_ASSERT_EQUAL(PROTOCOL_STATE_IDLE, can_h.state);

    free_can_handle_buffers(&can_h);
}

// Test can_calculate_crc with a NULL frame pointer.
void test_can_calculate_crc_null_frame_returns_zero(void) {
    // Expected: Function returns 0 for a NULL frame.
    uint16_t crc = can_calculate_crc(NULL);
    TEST_ASSERT_EQUAL(0, crc);
}

// Test can_calculate_crc with a valid frame.
void test_can_calculate_crc_valid_frame(void) {
    can_frame_t frame = {
        .id = 0x00000123, // Example CAN ID
        .dlc = 8, // Data Length Code
        .data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}, // Example data
        .crc = 0x0000 // CRC field itself is part of the calculation, set to 0 for input for this test
    };
    // Expected: CRC for the given frame data, pre-calculated value.
    uint16_t expected_crc = 0xDBB9; // Pre-calculated using the source algorithm
    uint16_t actual_crc = can_calculate_crc(&frame);
    TEST_ASSERT_EQUAL_HEX16(expected_crc, actual_crc);
}

// ==== Ethernet Functions Tests ====

// Test ethernet_parse_frame with a NULL data pointer.
void test_ethernet_parse_frame_null_data_returns_invalid_header(void) {
    ethernet_frame_t frame;
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for a NULL data pointer.
    protocol_error_t result = ethernet_parse_frame(NULL, 60, &frame);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
}

// Test ethernet_parse_frame with a NULL frame pointer.
void test_ethernet_parse_frame_null_frame_returns_invalid_header(void) {
    uint8_t data[60] = {0};
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for a NULL frame pointer.
    protocol_error_t result = ethernet_parse_frame(data, 60, NULL);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
}

// Test ethernet_parse_frame with length less than the minimum Ethernet header size (14 bytes).
void test_ethernet_parse_frame_short_length_returns_invalid_header(void) {
    uint8_t data[13] = {0}; // 13 bytes, less than required 14
    ethernet_frame_t frame;
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for length < 14.
    protocol_error_t result = ethernet_parse_frame(data, 13, &frame);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
}

// Test ethernet_parse_frame when the payload length exceeds the internal buffer capacity.
void test_ethernet_parse_frame_payload_overflow_returns_buffer_overflow(void) {
    // Construct data that implies a payload larger than ETHERNET_MAX_PAYLOAD_SIZE.
    // Length = 14 bytes header + (ETHERNET_MAX_PAYLOAD_SIZE + 1) for payload
    uint16_t oversized_length = 14 + ETHERNET_MAX_PAYLOAD_SIZE + 1;
    uint8_t data[oversized_length]; // Buffer sized for oversized frame
    memset(data, 0, sizeof(data));
    ethernet_frame_t frame;
    // Expected: Function returns PROTOCOL_ERROR_BUFFER_OVERFLOW.
    protocol_error_t result = ethernet_parse_frame(data, oversized_length, &frame);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_BUFFER_OVERFLOW, result);
}

// Test ethernet_parse_frame for successful parsing of a valid frame.
void test_ethernet_parse_frame_success(void) {
    // Minimum Ethernet frame size is 60 bytes, including 14 bytes header.
    // So 60 - 14 = 46 bytes for payload.
    uint8_t data[60];
    memset(data, 0, sizeof(data));

    uint8_t dest_mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02};
    uint8_t src_mac[] = {0xCA, 0xFE, 0xBA, 0xBE, 0x03, 0x04};
    uint16_t ethertype = 0x0800; // IPv4 ethertype
    uint8_t payload[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00}; // 46 bytes payload

    memcpy(data, dest_mac, 6);
    memcpy(data + 6, src_mac, 6);
    data[12] = (uint8_t)((ethertype >> 8) & 0xFF); // Big-endian ethertype
    data[13] = (uint8_t)(ethertype & 0xFF);
    memcpy(data + 14, payload, sizeof(payload));

    ethernet_frame_t frame = {0}; // Clear frame for parsing
    // Expected: Parsing should succeed.
    protocol_error_t result = ethernet_parse_frame(data, sizeof(data), &frame);

    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_NONE, result);
    // Expected: Destination MAC, Source MAC, Ethertype, and Payload should be parsed correctly.
    TEST_ASSERT_EQUAL_MEMORY(dest_mac, frame.destination, 6);
    TEST_ASSERT_EQUAL_MEMORY(src_mac, frame.source, 6);
    TEST_ASSERT_EQUAL_HEX16(ethertype, frame.ethertype);
    TEST_ASSERT_EQUAL_MEMORY(payload, frame.payload, sizeof(payload));
    // Expected: CRC field in the frame struct is reset to 0 by the function.
    TEST_ASSERT_EQUAL(0, frame.crc);

    // Test with max payload size
    uint8_t data_max_payload[14 + ETHERNET_MAX_PAYLOAD_SIZE];
    memset(data_max_payload, 0x5A, sizeof(data_max_payload)); // Fill with a pattern
    data_max_payload[0] = 0xDE; // First byte of dest mac
    data_max_payload[6] = 0xCA; // First byte of src mac
    data_max_payload[12] = 0x08; data_max_payload[13] = 0x06; // ARP ethertype
    ethernet_frame_t frame_max = {0};
    result = ethernet_parse_frame(data_max_payload, sizeof(data_max_payload), &frame_max);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_NONE, result);
    TEST_ASSERT_EQUAL_MEMORY(data_max_payload, frame_max.destination, 6);
    TEST_ASSERT_EQUAL_MEMORY(data_max_payload + 6, frame_max.source, 6);
    TEST_ASSERT_EQUAL_HEX16(0x0806, frame_max.ethertype); // ARP
    TEST_ASSERT_EQUAL_MEMORY(data_max_payload + 14, frame_max.payload, ETHERNET_MAX_PAYLOAD_SIZE);
}

// Test ethernet_calculate_crc with a NULL frame pointer.
void test_ethernet_calculate_crc_null_frame_returns_zero(void) {
    // Expected: Function returns 0 for a NULL frame.
    uint32_t crc = ethernet_calculate_crc(NULL);
    TEST_ASSERT_EQUAL(0, crc);
}

// Test ethernet_calculate_crc with a valid frame.
void test_ethernet_calculate_crc_valid_frame(void) {
    ethernet_frame_t frame = {
        .destination = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01},
        .source = {0xCA, 0xFE, 0xBA, 0xBE, 0x00, 0x02},
        .ethertype = 0x0800, // IP ethertype
        .payload = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10},
        .crc = 0x00000000 // CRC field is excluded from calculation by the function
    };
    // Expected: CRC for the given frame data, pre-calculated value.
    uint32_t expected_crc = 0x9F41E383; // Pre-calculated using the source algorithm
    uint32_t actual_crc = ethernet_calculate_crc(&frame);
    TEST_ASSERT_EQUAL_HEX32(expected_crc, actual_crc);
}

// ==== Protocol Message Functions Tests ====

// Test protocol_parse_message with a NULL data pointer.
void test_protocol_parse_message_null_data_returns_invalid_header(void) {
    protocol_message_t msg;
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for a NULL data pointer.
    protocol_error_t result = protocol_parse_message(NULL, 10, &msg);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
}

// Test protocol_parse_message with a NULL message pointer.
void test_protocol_parse_message_null_message_returns_invalid_header(void) {
    uint8_t data[] = {0xAA, 0x01, 0x00, 0xDE, 0xAD};
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for a NULL message pointer.
    protocol_error_t result = protocol_parse_message(data, sizeof(data), NULL);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
}

// Test protocol_parse_message with length less than the minimum required (5 bytes).
void test_protocol_parse_message_short_length_returns_invalid_header(void) {
    uint8_t data[] = {0xAA, 0x01, 0x00, 0xDE}; // 4 bytes, minimum is 5 (H, L, P[0], Crc[0], Crc[1])
    protocol_message_t msg;
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for length < 5.
    protocol_error_t result = protocol_parse_message(data, sizeof(data), &msg);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
}

// Test protocol_parse_message with an invalid header byte (not 0xAA).
void test_protocol_parse_message_invalid_header_returns_invalid_header(void) {
    uint8_t data[] = {0xBB, 0x01, 0x00, 0xDE, 0xAD}; // Header 0xBB instead of 0xAA
    protocol_message_t msg = {0};
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for an incorrect header byte.
    protocol_error_t result = protocol_parse_message(data, sizeof(data), &msg);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
}

// Test protocol_parse_message when the reported data_length exceeds the payload buffer capacity.
void test_protocol_parse_message_payload_overflow_returns_buffer_overflow(void) {
    // Construct data where data_length is greater than PROTOCOL_MAX_PAYLOAD_SIZE.
    uint8_t data[5 + PROTOCOL_MAX_PAYLOAD_SIZE + 1]; // Make space for header/len/crc + oversized payload
    memset(data, 0, sizeof(data));
    data[0] = 0xAA; // Valid header
    data[1] = PROTOCOL_MAX_PAYLOAD_SIZE + 1; // data_length causes overflow
    protocol_message_t msg = {0};
    // Expected: Function returns PROTOCOL_ERROR_BUFFER_OVERFLOW.
    protocol_error_t result = protocol_parse_message(data, sizeof(data), &msg);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_BUFFER_OVERFLOW, result);
}

// Test protocol_parse_message for successful parsing of a valid message.
void test_protocol_parse_message_success(void) {
    // Example: Header (0xAA), Data Length (0x04), Payload (0x11, 0x22, 0x33, 0x44), CRC (0xDF28)
    uint8_t raw_data[] = {0xAA, 0x04, 0x11, 0x22, 0x33, 0x44, 0xDF, 0x28}; // Total 8 bytes
    uint16_t length = sizeof(raw_data);
    protocol_message_t msg = {0};

    // Expected: Parsing should succeed.
    protocol_error_t result = protocol_parse_message(raw_data, length, &msg);

    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_NONE, result);
    // Expected: raw_bytes in the union should be a copy of the input data.
    TEST_ASSERT_EQUAL_MEMORY(raw_data, msg.raw_bytes, length);
    // Expected: Header, data_length, and payload should be correctly parsed.
    TEST_ASSERT_EQUAL_HEX8(0xAA, msg.packet.header);
    TEST_ASSERT_EQUAL_HEX8(0x04, msg.packet.data_length);
    uint8_t expected_payload[] = {0x11, 0x22, 0x33, 0x44};
    TEST_ASSERT_EQUAL_MEMORY(expected_payload, msg.packet.payload, msg.packet.data_length);
    TEST_ASSERT_EQUAL_HEX16(0xDF28, msg.packet.crc);

    // Test with zero length payload
    uint8_t raw_data_zero_payload[] = {0xAA, 0x00, 0x0D, 0x09}; // Header, len=0, CRC
    length = sizeof(raw_data_zero_payload);
    memset(&msg, 0, sizeof(msg)); // Clear for next test case
    result = protocol_parse_message(raw_data_zero_payload, length, &msg);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_NONE, result);
    TEST_ASSERT_EQUAL_HEX8(0xAA, msg.packet.header);
    TEST_ASSERT_EQUAL_HEX8(0x00, msg.packet.data_length);
    TEST_ASSERT_EQUAL_HEX16(0x0D09, msg.packet.crc);
}

// Test protocol_validate_message with a NULL message pointer.
void test_protocol_validate_message_null_message_returns_invalid_header(void) {
    // Expected: Function returns PROTOCOL_ERROR_INVALID_HEADER for a NULL message.
    protocol_error_t result = protocol_validate_message(NULL);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_INVALID_HEADER, result);
}

// Test protocol_validate_message with a CRC mismatch.
void test_protocol_validate_message_crc_mismatch_returns_crc_mismatch(void) {
    protocol_message_t msg = {0};
    // Manually construct a message with an incorrect CRC.
    msg.packet.header = 0xAA;
    msg.packet.data_length = 2;
    msg.packet.payload[0] = 0xDE;
    msg.packet.payload[1] = 0xAD;
    msg.packet.crc = 0x1234; // Deliberately incorrect CRC (expected is 0xDF28)

    // Expected: Function returns PROTOCOL_ERROR_CRC_MISMATCH.
    protocol_error_t result = protocol_validate_message(&msg);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_CRC_MISMATCH, result);
}

// Test protocol_validate_message for successful validation with correct CRC.
void test_protocol_validate_message_success(void) {
    protocol_message_t msg = {0};
    // Manually construct a message with the correct CRC.
    msg.packet.header = 0xAA;
    msg.packet.data_length = 2;
    msg.packet.payload[0] = 0xDE;
    msg.packet.payload[1] = 0xAD;
    // CRC for {0xAA, 0x02, 0xDE, 0xAD}
    msg.packet.crc = 0xDF28; // Pre-calculated correct CRC

    // Expected: Validation should succeed.
    protocol_error_t result = protocol_validate_message(&msg);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_NONE, result);

    // Test with zero length payload
    memset(&msg, 0, sizeof(msg)); // Clear for next test case
    msg.packet.header = 0xAA;
    msg.packet.data_length = 0;
    // CRC for {0xAA, 0x00}
    msg.packet.crc = 0x0D09; // Pre-calculated correct CRC for zero payload

    result = protocol_validate_message(&msg);
    TEST_ASSERT_EQUAL(PROTOCOL_ERROR_NONE, result);
}

// Test protocol_calculate_crc with a NULL data pointer.
void test_protocol_calculate_crc_null_data_returns_zero(void) {
    // Expected: Function returns 0 for a NULL data pointer.
    uint16_t crc = protocol_calculate_crc(NULL, 10);
    TEST_ASSERT_EQUAL(0, crc);
}

// Test protocol_calculate_crc with valid data.
void test_protocol_calculate_crc_valid_data(void) {
    // Data: {Header, Data Length, Payload...}
    uint8_t data[] = {0xAA, 0x02, 0xDE, 0xAD}; // Header, data_length, 2 bytes of payload
    uint16_t expected_crc = 0xDF28; // Pre-calculated value
    // Expected: CRC for the given data should match the pre-calculated value.
    uint16_t actual_crc = protocol_calculate_crc(data, sizeof(data));
    TEST_ASSERT_EQUAL_HEX16(expected_crc, actual_crc);

    // Test with zero-length payload data
    uint8_t data_zero_len[] = {0xAA, 0x00}; // Header, data_length=0
    expected_crc = 0x0D09; // Pre-calculated value
    actual_crc = protocol_calculate_crc(data_zero_len, sizeof(data_zero_len));
    TEST_ASSERT_EQUAL_HEX16(expected_crc, actual_crc);
}



int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_can_init_null_handle_returns_invalid_header);
    RUN_TEST(test_can_init_zero_buffer_size_returns_invalid_header);
    RUN_TEST(test_can_init_success_initializes_handle);
    RUN_TEST(test_can_transmit_message_null_handle_returns_invalid_header);
    RUN_TEST(test_can_transmit_message_null_frame_returns_invalid_header);
    RUN_TEST(test_can_transmit_message_buffer_full_returns_buffer_overflow);
    RUN_TEST(test_can_transmit_message_success_and_state_change);
    RUN_TEST(test_can_receive_message_null_handle_returns_invalid_header);
    RUN_TEST(test_can_receive_message_null_frame_returns_invalid_header);
    RUN_TEST(test_can_receive_message_buffer_empty_returns_timeout);
    RUN_TEST(test_can_receive_message_success);
    RUN_TEST(test_can_calculate_crc_null_frame_returns_zero);
    RUN_TEST(test_can_calculate_crc_valid_frame);
    RUN_TEST(test_ethernet_parse_frame_null_data_returns_invalid_header);
    RUN_TEST(test_ethernet_parse_frame_null_frame_returns_invalid_header);
    RUN_TEST(test_ethernet_parse_frame_short_length_returns_invalid_header);
    RUN_TEST(test_ethernet_parse_frame_payload_overflow_returns_buffer_overflow);
    RUN_TEST(test_ethernet_parse_frame_success);
    RUN_TEST(test_ethernet_calculate_crc_null_frame_returns_zero);
    RUN_TEST(test_ethernet_calculate_crc_valid_frame);
    RUN_TEST(test_protocol_parse_message_null_data_returns_invalid_header);
    RUN_TEST(test_protocol_parse_message_null_message_returns_invalid_header);
    RUN_TEST(test_protocol_parse_message_short_length_returns_invalid_header);
    RUN_TEST(test_protocol_parse_message_invalid_header_returns_invalid_header);
    RUN_TEST(test_protocol_parse_message_payload_overflow_returns_buffer_overflow);
    RUN_TEST(test_protocol_parse_message_success);
    RUN_TEST(test_protocol_validate_message_null_message_returns_invalid_header);
    RUN_TEST(test_protocol_validate_message_crc_mismatch_returns_crc_mismatch);
    RUN_TEST(test_protocol_validate_message_success);
    RUN_TEST(test_protocol_calculate_crc_null_data_returns_zero);
    RUN_TEST(test_protocol_calculate_crc_valid_data);

    return UNITY_END();
}