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

// IR receiver result status constants.
enum IRResultStatus_t {
    IR_DONE,
    IR_ERROR
};

// Possible values for the INTPPS register. You can pass these to ir_setup().
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
} INTPPSValues = {
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
 * 
 * @param uint8_t INT_pin One of the pins defined in INTPPSValues.
 */
void ir_setup(const uint8_t INT_pin);

/**
 * Register a callback function for the IR receiver.
 * 
 * The registered callback is called each time a key press is received.
 * 
 * @param cb A pointer to the callback function to call.
 */
void ir_reg_callback(void (*cb)(const uint8_t status, const uint8_t addr, const uint8_t cmd));

/**
 * IR receiver Interrupt Service Routine.
 */
void ir_isr(void);
#endif	/* IR_RECEIVER_H */

