#include <stdint.h>
#include <pic16f15344.h>
#include <string.h>

#include "ir_receiver.h"

// NEC timing constants.
#define NEC_BURST 13500UL
#define NEC_BIT_HI 2250UL
#define NEC_BIT_LO 1250UL
#define NEC_JITTER 1000UL

#define TIMER0_FREQ 3200000UL
#define TIMER0_PRESCALE 512UL
#define TIMER0_TICK (uint8_t) (((float) 1000000/TIMER0_FREQ)*TIMER0_PRESCALE) // In us.

volatile static struct IRReceiverState_t IRReceiverState = {0, 0, 0, 0};

void ir_setup(uint8_t INT_pin) {
	// Enable external interrupts on an IO pin.
	INTPPS = INT_pin; // Select external interrupt pin.
	PIR0bits.INTF = 0; // Clear INT interrupt flag.
	INTCONbits.INTEDG = 1; // Interrupt on rising edge of INT pin.
	PIE0bits.INTE = 1; // External INT interrupt enable.
	INTCONbits.PEIE = 1; // Peripheral interrupt enable.
	INTCONbits.GIE = 1; // Global interrupt enable.

	// Enable TIMER0 for measuring time intervals.
	TMR0H = 0xFF; // Set period buffer to 0xFF.
	TMR0L = 0; // Clear timer counter.
	T0CON0bits.T016BIT = 0; // 8-bit timer.
	T0CON0bits.T0OUTPS = 0x0; // 1:1 postscaler.
	T0CON1bits.T0CS = 0x3; // HFINTOSC clock source.
	T0CON1bits.T0ASYNC = 1;
	T0CON1bits.T0CKPS = 0x9; // 1:512 prescaler.
	T0CON0bits.T0EN = 1; // TIMER0 enable.
}

volatile const struct IRReceiverState_t *get_ir_state(void) {
	return &IRReceiverState;
}

void ir_reset(void) {
	IRReceiverState.status = IR_READY;
}

void __interrupt() ISR(void) {
	if (PIE0bits.INTE && PIR0bits.INTF) {
		uint16_t dt = TMR0L*TIMER0_TICK;
		
		if (IRReceiverState.status == IR_READY) {
			// Received first rising edge => Begin receiving NEC.
			IRReceiverState.status = IR_BURST;
			INTCONbits.INTEDG = 0;
		} else if (IRReceiverState.status == IR_BURST) {
			IRReceiverState.status = IR_READY;
			INTCONbits.INTEDG = 1;
		} /*else if (IRReceiverState.status == IR_RECEIVING) {
			uint16_t *buf = 0;

			if (
				IRReceiverState.cnt <= 7 || // 1st byte
				(IRReceiverState.cnt >= 16 && IRReceiverState.cnt <= 24) // 3rd byte
			) {
				// 1st and 3rd bytes are actual data.
				buf = &IRReceiverState.data_buf;
			} else if (
				(IRReceiverState.cnt >= 8 && IRReceiverState.cnt <= 15) || // 2nd byte
				(IRReceiverState.cnt >= 24 && IRReceiverState.cnt <= 31)   // 4th byte
			) {
				// 2nd and 4th bytes are inverted check bytes.
				buf = &IRReceiverState.check_buf;
			} else if (IRReceiverState.cnt == 32) {
				// Packet stops at the 32nd bit.
				buf = &IRReceiverState.check_buf;
				IRReceiverState.status = IR_DONE;
			}

			// Add the new bit to the correct buffer.
			*buf >>= 1;
			if (dt > NEC_BIT_HI - NEC_JITTER && dt < NEC_BIT_HI + NEC_JITTER) {
				*buf |= 0x8000;
			} else if (!(dt > NEC_BIT_LO - NEC_JITTER && dt < NEC_BIT_LO + NEC_JITTER)) {
				IRReceiverState.status = IR_ERROR;
			}
			IRReceiverState.asd[IRReceiverState.cnt+1] = dt;
			IRReceiverState.cnt++;
		}*/
		
		TMR0L = 0; // Clear the timer counter.
		PIR0bits.INTF = 0; // Clear the INT interrupt flag.
	}
}
