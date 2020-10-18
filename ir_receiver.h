/* 
 * File:   ir_receiver.h
 * Author: Eero Talus
 *
 * Created on 11 October 2020, 20:19
 */

#ifndef IR_RECEIVER_H
#define	IR_RECEIVER_H

#include <stdint.h>

// IR receiver status codes.
#define IR_ERROR 0
#define IR_DONE 1
#define IR_READY 2
#define IR_RECEIVING 3
#define IR_BURST 4

// IR receiver state struct.
struct IRReceiverState_t {
	uint8_t status;
	uint8_t cnt;
	uint16_t data_buf;
	uint16_t check_buf;
    uint16_t asd[33];
};

// Possible values for the INTPPS register.
const struct {
	uint8_t RA0;
	uint8_t RA1;
	uint8_t RA2;
	uint8_t RA3;
	uint8_t RA4;
	uint8_t RA5;
	uint8_t RB4;
	uint8_t RB5;
	uint8_t RB6;
	uint8_t RB7;
	uint8_t RC0;
	uint8_t RC1;
	uint8_t RC2;
	uint8_t RC3;
	uint8_t RC4;
	uint8_t RC5;
	uint8_t RC6;
	uint8_t RC7;
} INTPPSvalues = {
	0x00,
	0x01,
	0x02,
	0x03,
	0x04,
	0x05,
	0x0C,
	0x0D,
	0x0E,
	0x0F,
	0x10,
	0x11,
	0x12,
	0x13,
	0x14,
	0x15,
	0x16,
	0x17
};

/*
 * Setup peripherals ready to receive IR packets.
 */
void ir_setup(uint8_t INT_pin);

/*
 * Get a const pointer to the IR receiver state struct.
 */
volatile const struct IRReceiverState_t *get_ir_state(void);

/*
 * Mark the IR receiver ready to receive new packets.
 */
void ir_reset(void);

#endif	/* IR_RECEIVER_H */

