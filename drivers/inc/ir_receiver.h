/* 
 * File:   ir_receiver.h
 * Author: Eero Talus
 *
 * NEC IR receiver decoder.
 * 
 * Created on 11 October 2020, 20:19
 */

#ifndef IR_RECEIVER_H
#define	IR_RECEIVER_H

#include <stdint.h>

typedef struct {
    uint8_t addr;
    uint8_t cmd;
    uint8_t valid;
    uint8_t pending;
} IREvent;

/*
 * Setup peripherals ready to receive IR packets.
 */
void ir_setup(void);

/*
 * Get the latest received IR event.
 * 
 * @param IREvent* event Destination event buffer.
 * 
 * @return int8_t 0 on success, -1 on failure.
 */
int8_t ir_get_event(IREvent* event);

/*
 * IR driver interrupt handler.
 * 
 * This must be called on all Interrupt-On-Change interrupts.
 */
void ir_isr(void);

#endif	/* IR_RECEIVER_H */

