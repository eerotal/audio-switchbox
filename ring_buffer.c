#include <stdint.h>
#include <stdlib.h>

#include "ring_buffer.h"

void rb_init(struct ring_buffer* rb) {
	rb->read_idx = 0;
	rb->write_idx = 0;
	rb->num_items = 0;
	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) { rb->buffer[i] = 0; }
}

uint8_t rb_push(struct ring_buffer* rb, const uint16_t item, const uint8_t overflow) {
	if (rb->num_items >= RING_BUFFER_SIZE) {
		if (rb->write_idx == RING_BUFFER_SIZE) {
			// We are at the end of the buffer.
			if (overflow == RB_OVERFLOW_DISCARD_START) {
				// Write the new item into the beginning of the buffer.
				rb->write_idx = 0;
				rb->buffer[rb->write_idx++] = item;
				
				// Advance the read index by one so that data is still read
				// in chronological order.
				if (++rb->read_idx == RING_BUFFER_SIZE) {
					rb->read_idx = 0;
				}
			}
		} else {
			// If we have already wrapped, write the item into the buffer
			// as usual but don't increment the number of items since the
			// buffer is still overflowing.
			rb->buffer[rb->write_idx++] = item;
			
			// Advance the read index by one so that data is still read
			// in chronological order.
			if (++rb->read_idx == RING_BUFFER_SIZE) {
				rb->read_idx = 0;
			}
		}
	
		return RB_OVERFLOW;
	} else if (rb->write_idx == RING_BUFFER_SIZE) {
		// Wrap to the start of the buffer.
		rb->write_idx = 0;
	}

	// Add the newest value to the end of the buffer.
	rb->buffer[rb->write_idx++] = item;
	rb->num_items++;
	
	return RB_OK;
}

uint8_t rb_pop(struct ring_buffer* rb, uint16_t* result) {
	if (rb->num_items == 0) { return RB_NO_DATA; }
	
	// Read the value and clear it.
	if (result != NULL) { *result = rb->buffer[rb->read_idx]; }
	rb->buffer[rb->read_idx] = 0;
	
	// Advance indices.
	if (++rb->read_idx == RING_BUFFER_SIZE) {
		// Wrap the read index at the end of the buffer.
		rb->read_idx = 0;
	}
	rb->num_items--;

	return RB_OK;
}

uint8_t rb_get(struct ring_buffer* rb, uint16_t *result, uint8_t idx) {
	if (rb->num_items == 0 || idx >= rb->num_items) {
		return RB_NO_DATA;
	}

	// Return the byte at idx but wrap if idx >= RING_BUFFER_SIZE.
	*result = rb->buffer[(rb->read_idx + idx) % RING_BUFFER_SIZE];

	return RB_OK;
}