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

/**
 * Read the latest IR event data.
 * 
 * @return Pointer to an IREvent struct or NULL if there are no pending events.
 */
const IREvent* ir_get_event(void);

void ir_isr(void);

#endif	/* IR_RECEIVER_H */

