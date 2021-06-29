/* 
 * File:   ring_buffer.h
 * Author: Eero Talus
 *
 * Created on June 29, 2021, 4:59 PM
 */

#ifndef RINGBUFFER_H
#define	RINGBUFFER_H

#include <stdint.h>

// Ring buffer error codes.
#define RB_OK 0
#define RB_NO_DATA 1
#define RB_OVERFLOW 2

// Ring buffer constants.
#define RING_BUFFER_SIZE 100
#define RB_OVERFLOW_DISCARD_START 0
#define RB_OVERFLOW_DISCARD_END 1

// Ring buffer type definitions.
struct ring_buffer {
	uint8_t read_idx;
	uint8_t write_idx;
	uint8_t num_items;
	uint16_t buffer[RING_BUFFER_SIZE];
};

/*
 * Initialize a ring buffer.
 *
 * @param struct ring_buffer* rb The ring buffer to initialize.
 */
void rb_init(struct ring_buffer* rb);

/*
 * Push an item into a ring buffer.
 *
 * @param ring_buffer_t* rb The ring buffer to push to.
 * @param uint16_t item The item to push to the buffer.
 * @param uint8_t overflow The way of handling overflows. One of RB_OVERFLOW_DISCARD_*.
 *
 * @return uint8_t One of the RB_* error flags.
 */
uint8_t rb_push(struct ring_buffer* rb, const uint16_t item, const uint8_t overflow);

/*
 * Pop an item from the beginning of a ring buffer.
 *
 * If result is NULL, the popped value is discarded.
 *
 * @param struct ring_buffer* rb The ring buffer to pop from.
 * @param uint16_t* result A pointer to a result variable.
 *
 * @return uint8_t One of the RB_* error flags.
 */
uint8_t rb_pop(struct ring_buffer* rb, uint16_t* result);

/*
 * Get an item at a specific position in the ring buffer.
 *
 * @param ring_buffer* rb The ring buffer to pop from.
 * @param uint16_t* result A pointer to a result variable.
 * @param uint8_t idx The index of the item.
 *
 * @return uint8_t One of the RB_* error flags.
 */
uint8_t rb_get(struct ring_buffer* rb, uint16_t *result, uint8_t idx);

#endif	/* RINGBUFFER_H */

