#include <stdint.h>
#include <pic16f15344.h>
#include <string.h>

#include "ir_receiver.h"

// NEC timing constants.
#define NEC_BURST 13500UL
#define NEC_REP 11250UL
#define NEC_HI 2250UL
#define NEC_LO 1250UL
#define NEC_JIT 500UL

// TIMER0 timing constants.
#define TIMER0_FREQ 32000000UL
#define TIMER0_PRESCALE 2048UL
#define TIMER0_TICK (uint8_t) (((float) 1000000/TIMER0_FREQ)*TIMER0_PRESCALE)

// IR receiver state status constants.
enum IRStateStatus_t {
    IR_PREBURST,
    IR_BURST,
    IR_RECEIVING
};

// IR receiver status struct.
volatile static struct IRReceiverState_t {
	uint8_t status;
	uint8_t cnt;
	uint32_t buf;
} ir_state = {0, 0, 0};

// IR receiver result buffer.
volatile static struct IRResult_t {
    uint8_t status;
    uint8_t address;
    uint8_t command;
} ir_buf = {0, 0, 0};

static void (*ir_callback)(const uint8_t status, const uint8_t addr, const uint8_t cmd) = NULL;

static void ir_reset(void);
static void ir_emit(const uint8_t status, const uint8_t addr, const uint8_t cmd);
static void ir_emit_repeat(void);
static void ir_emit_error(void);
static void ir_emit_new(void);

void ir_setup(const uint8_t INT_pin) {
	// Enable external interrupts on an IO pin.
	INTPPS = INT_pin; // Select external interrupt pin.
	PIR0bits.INTF = 0; // Clear INT interrupt flag.
	INTCONbits.INTEDG = 0; // INT on falling edge. (TSOP343xxx out = active low)
	PIE0bits.INTE = 1; // External INT interrupt enable.
	INTCONbits.PEIE = 1; // Peripheral interrupt enable.
	INTCONbits.GIE = 1; // Global interrupt enable.

	// Enable TIMER0 for measuring NEC timing intervals.
	TMR0H = 0xFF; // Set period buffer.
	TMR0L = 0; // Clear counter.
	T0CON0bits.T016BIT = 0; // 8-bit timer.
	T0CON0bits.T0OUTPS = 0x0; // 1:1 postscaler.
	T0CON1bits.T0CS = 0x3; // HFINTOSC clock source.
	T0CON1bits.T0ASYNC = 1; // TIMER0 asynchronous mode.
	T0CON1bits.T0CKPS = 0xB; // 1:2048 prescaler.
	PIE0bits.TMR0IE = 1; // TIMER0 interrupt enable.
	T0CON0bits.T0EN = 1; // TIMER0 enable.
	
	ir_state.status = IR_PREBURST;
}

void ir_reg_callback(void (*cb)(const uint8_t status, const uint8_t addr, const uint8_t cmd)) {
	ir_callback = cb;
}

static void ir_reset(void) {
	ir_state.status = IR_PREBURST;
	ir_state.cnt = 0;
	ir_state.buf = 0;
}

static void ir_emit(const uint8_t status, const uint8_t addr, const uint8_t cmd) {
	ir_buf.status = status;
	ir_buf.address = addr;
	ir_buf.command = cmd;

	if (ir_callback != NULL) { (*ir_callback)(status, addr, cmd); }
}

static void ir_emit_repeat(void) {
	ir_emit(ir_buf.status, ir_buf.address, ir_buf.command);
}

static void ir_emit_error(void) {
	ir_emit(IR_ERROR, 0, 0);
}

static void ir_emit_new(void) {
	uint8_t addr = (uint8_t) (ir_state.buf & 0xFF);
	uint8_t addr_chk = (uint8_t) (((~ir_state.buf) >> 8) & 0xFF);
	uint8_t cmd = (uint8_t) ((ir_state.buf >> 16) & 0xFF);
	uint8_t cmd_chk = (uint8_t) (((~ir_state.buf) >> 24) & 0xFF);

	if (cmd == cmd_chk && addr == addr_chk) {
		// Check bytes match actual data => Emit result.
		ir_emit(IR_DONE, addr, cmd);
	} else {
		// Check byte mismatch => Emit error.
		ir_emit_error();
	}
}

void ir_isr(void) {
	if (PIE0bits.INTE) {
		if (PIR0bits.INTF) {
			// Calculate the time difference between successive edges
			// and clear the timer counter.
			uint16_t dt = TMR0L*TIMER0_TICK;
			TMR0L = 0;

			if (ir_state.status == IR_PREBURST) {
				ir_state.status = IR_BURST;
			} else if (ir_state.status == IR_BURST) {
				if (dt >= NEC_BURST - NEC_JIT && dt <= NEC_BURST + NEC_JIT) {
					// Full burst complete => Receive packet.
					ir_state.status = IR_RECEIVING;
				} else if (dt >= NEC_REP - NEC_JIT && dt <= NEC_REP + NEC_JIT) {
					// Repetition burst complete => Use last received packet.
					ir_emit_repeat();
					ir_state.status = IR_PREBURST;
				} else {
					// Invalid burst length => Cause an error because this
					// means we missed some data. This is done to make sure
					// possible future repetitions don't accidentally repeat
					// the previous packet if a new one was missed. Don't
					// ir_reset() here (which would set ir_state.status =
					// IR_PREBURST) so that we can try to catch the next edge
					// immediately.
					ir_emit_error();
				}
			} else if (ir_state.status == IR_RECEIVING) {
				// Shift next bit into the receive buffer.
				ir_state.buf >>= 1;
				if (dt >= NEC_HI - NEC_JIT && dt <= NEC_HI + NEC_JIT) {
					ir_state.buf |=  0x80000000;
				} else if (!(dt >= NEC_LO - NEC_JIT && dt <= NEC_LO + NEC_JIT)) {
					// Bit length error.
					ir_emit_error();
					ir_reset();
				}

				if (++ir_state.cnt == 32) {
					// Packet stops after the 33rd bit.
					ir_emit_new();
					ir_reset();
				}
			}
			PIR0bits.INTF = 0; // Clear the INT interrupt flag.
			PIR0bits.TMR0IF = 0; // Clear the TMR0IF interrupt flag.
		}
		
		if (PIR0bits.TMR0IF) {
			if (ir_state.status == IR_RECEIVING || ir_state.status == IR_BURST) {
				// Burst or bit length error.
				ir_emit_error();
				ir_reset();
			}
			PIR0bits.INTF = 0; // Clear the INT interrupt flag.
			PIR0bits.TMR0IF = 0; // Clear the TMR0IF interrupt flag.
		}
	}
}
