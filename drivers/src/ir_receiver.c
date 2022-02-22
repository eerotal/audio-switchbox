#include "drivers/inc/ir_receiver.h"
#include "drivers/inc/timer.h"

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

// Check whether an NEC time interval is within an acceptable range.
#define NEC_WITHIN(value, target) (value >= target - NEC_JIT/2 && value <= target + NEC_JIT/2)

// IR state machine states.
typedef enum {
	INIT,
	AGCH,
	AGCL,
	DATA
} IRState;

// IR state machine data struct.
typedef struct {
	IRState state; // Current state.
	uint32_t data; // Current frame buffer.
	uint8_t idx;   // Current frame bit index.
} IRData;

volatile static IRData ir_data = {INIT, 0, 0};
volatile static IREvent ir_event = {0};

static void ir_store(uint32_t data);
static void ir_discard(void);
static void ir_repeat(void);
static void ir_fsm(void);

/*
 * Check the validity of a received IR frame and store it if it's valid.
 * 
 * @param uint32_t data The received raw IR frame.
 */
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

/*
 * Mark the currently buffered IR frame as not suitable for repeating.
 */
static void ir_discard(void) {
	ir_event.valid = 0;
}

/*
 * Repeat the currently buffered IR frame.
 */
static void ir_repeat(void) {
	if (ir_event.valid) {
		ir_event.pending = 1;
	}
}

/*
 * Main IR driver state machine.
 * 
 * This should be called on all rising and falling edges of the IR sensor pin.
 */
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
}

int8_t ir_get_event(IREvent* event) {
	PIE0bits.IOCIE = 0; // Disable IOC while copying data.

	if (ir_event.pending) {
		memcpy(event, (IREvent*) &ir_event, sizeof(IREvent));
		ir_event.pending = 0;
		
		PIE0bits.IOCIE = 1; // Re-enable IOC.
		return 0;
	}
	
	PIE0bits.IOCIE = 1; // Re-enable IOC.
	return -1;
}

void ir_isr(void) {
	if (IOCCFbits.IOCCF1) {
		ir_fsm();
		IOCCFbits.IOCCF1 = 0;
	}
}