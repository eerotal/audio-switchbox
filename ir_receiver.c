#include "ir_receiver.h"

#include <pic16f15344.h>
#include <stdint.h>
#include <string.h>

// NEC timing constants (us).
#define NEC_AGC_HI        9000UL
#define NEC_AGC_LO        4500UL
#define NEC_AGC_LO_REPEAT 2250UL
#define NEC_BIT_HI        2250UL
#define NEC_BIT_LO        1125UL
#define NEC_JIT           500UL

#define NEC_WITHIN(value, target) (value >= target - NEC_JIT/2 && value <= target + NEC_JIT/2)

// TIMER0 timing constants.
#define TMR_FREQ 32000000UL // Hz
#define TMR_PRESCALE 256UL
#define TMR_TICK (uint8_t) (((float) 1000000/TMR_FREQ)*TMR_PRESCALE) // us

typedef enum {
	INIT,
	AGCH,
	AGCL,
	DATA
} IRState;

typedef struct {
	IRState state;
	uint32_t data;
	uint8_t idx;
} IRData;

static IRData ir_data = {INIT, 0, 0};
static IREvent ir_event = {0};

static void ir_store(uint32_t data);
static void ir_discard(void);
static void ir_repeat(void);
static void ir_fsm(void);

static uint16_t us_to_tmr0_ticks(uint32_t us);
static uint32_t tmr0_ticks_to_us(uint16_t tick);
static uint16_t tmr0_read(void);
static void tmr0_write(uint16_t x);

void ir_setup(void) {
	// Configure Interrupt-on-Change peripheral.
	TRISCbits.TRISC1 = 1; // RC1 = Input.
	IOCCPbits.IOCCP1 = 1; // Rising IOC on RC1.
	IOCCNbits.IOCCN1 = 1; // Falling IOC on RC1.
	IOCCFbits.IOCCF1 = 0; // Clear RC1 IOC status flag.
	PIR0bits.IOCIF = 0;   // Clear IOC interrupt flag.
	PIE0bits.IOCIE = 1;   // Enable IOC.
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;

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

static uint16_t us_to_tmr0_ticks(uint32_t us) {
	return (uint16_t) us/TMR_TICK;
}

static uint32_t tmr0_ticks_to_us(uint16_t tick) {
	return (uint32_t) tick*TMR_TICK;
}

static uint16_t tmr0_read(void) {
	uint8_t l = TMR0L;
	uint8_t h = TMR0H;
	
	return ((uint16_t) l)|((uint16_t) h << 8);
}

static void tmr0_write(uint16_t x) {
	TMR0H = (x >> 8);
	TMR0L = (x & 0xFF);
}

static void ir_store(uint32_t data) {
	uint8_t addr = data & 0xFF;
	uint8_t addr_n = ~((data >> 8) & 0xFF);
	uint8_t cmd = (data >> 16) & 0xFF;
	uint8_t cmd_n = ~((data >> 24) & 0xFF);

	if (cmd == cmd_n && addr == addr_n) {
		ir_event.addr = addr;
		ir_event.cmd = cmd;
		ir_event.pending = 1;
		ir_event.valid = 1;
	}
}

static void ir_discard(void) {
	ir_event.valid = 0;
}

static void ir_repeat(void) {
	if (ir_event.valid) {
		ir_event.pending = 1;
	}
}

static void ir_fsm() {
	uint8_t in = (~PORTCbits.RC1 & 0x01);
	uint32_t us = tmr0_ticks_to_us(tmr0_read());

	// State machine.
	switch (ir_data.state) {
		case INIT:
			if (in) {
				tmr0_write(0);
				ir_data.state = AGCH;
			}
			break;
		case AGCH:
			if (NEC_WITHIN(us, NEC_AGC_HI)) {
				tmr0_write(0);
				ir_data.state = AGCL;
			} else {
				ir_discard();
				ir_data.state = INIT;
			}
			break;
		case AGCL:
			if (NEC_WITHIN(us, NEC_AGC_LO_REPEAT)) {
				ir_repeat();
				ir_data.state = INIT;
			} else if (NEC_WITHIN(us, NEC_AGC_LO)) {
				tmr0_write(0);
				ir_data.idx = 0;
				ir_data.state = DATA;
			} else {
				ir_discard();
				ir_data.state = INIT;
			}
			break;
		case DATA:
			if (ir_data.idx == 32) {
				ir_store(ir_data.data);
				ir_data.state = INIT;
			} else if (in) {
				if (NEC_WITHIN(us, NEC_BIT_HI)) {
					tmr0_write(0);
					ir_data.idx += 1;
					ir_data.data = (ir_data.data >> 1)|((uint32_t) 1 << 31);
				} else if (NEC_WITHIN(us, NEC_BIT_LO)) {
					tmr0_write(0);
					ir_data.idx += 1;
					ir_data.data = (ir_data.data >> 1);
				} else {
					ir_discard();
					ir_data.state = INIT;
				}
			}
			break;
		default:
			ir_data.state = INIT;
			break;
	}
}

const IREvent* ir_get_event(void) {
	if (ir_event.pending) {
		ir_event.pending = 0;
		return &ir_event;
	} else {
		return NULL;
	}
}

void ir_isr(void) {
	if (IOCCFbits.IOCCF1) {
		ir_fsm();
		IOCCFbits.IOCCF1 = 0;
	}
}