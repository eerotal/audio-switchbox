#include "drivers/inc/timer.h"

#include <xc.h>
#include <stdint.h>

void tmr_setup(void) {
	// Configure TIMER0 peripheral.
	TMR0H = 0;
	TMR0L = 0;
	T0CON0bits.T016BIT = 1; // 16-bit timer.
	T0CON1bits.T0CS = 0x3; // HFINTOSC clock source (32 MHz).
	T0CON1bits.T0ASYNC = 1; // TIMER0 asynchronous mode.
	T0CON1bits.T0CKPS = 0b1000; // 1:256 prescaler.
	T0CON0bits.T0OUTPS = 0x0; // 1:1 postscaler.
	T0CON0bits.T0EN = 1; // TIMER0 enable.
}

uint32_t tmr0_ticks_to_us(uint16_t tick) {
	return (uint32_t) tick*TMR_TICK;
}

uint16_t tmr0_read(void) {
	uint8_t l = TMR0L;
	uint8_t h = TMR0H;
	
	return ((uint16_t) l)|((uint16_t) h << 8);
}

void tmr0_write(uint16_t x) {
	TMR0H = (x >> 8);
	TMR0L = (x & 0xFF);
}