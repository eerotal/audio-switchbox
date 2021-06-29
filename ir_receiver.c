#include <stdint.h>
#include <pic16f15344.h>
#include <string.h>

#include "ir_receiver.h"
#include "ring_buffer.h"

// IR clock constants.
#define IR_CLOCK 8000000
#define IR_PRESCALE 8
#define IR_TICKS_PER_SEC (IR_CLOCK/IR_PRESCALE)
#define IR_TICKS_PER_USEC (IR_TICKS_PER_SEC/1000000)

// NEC pulse widths in us.
#define NEC_AGC_HIGH 9000
#define NEC_AGC_LOW  4500
#define NEC_PULSE    560
#define NEC_ONE      1690
#define NEC_ZERO     560

static struct ring_buffer interval_buffer;
static uint16_t parser_cnt = 0;

uint8_t ir_match_length(const uint16_t pulse, const uint16_t target);
uint16_t ir_ticks_to_us(const uint16_t ticks);

void ir_setup(void) {
    // Init ring buffer.
    rb_init(&interval_buffer);

    // Setup receiver pins.
    ANSELCbits.ANSC1 = 0; // RC1 = Digital
    TRISCbits.TRISC1 = 1; // RC1 = Input

    // Enable interrupts.
    INTCONbits.PEIE = 1;       // Enable peripheral interrupts.
    INTCONbits.GIE = 1;        // Global interrupt enable.

    // Configure Timer 1.
    T1CLK = 0b0001;            // Clock = Fosc/4 = 8 MHz.
    T1CONbits.CKPS = 0b11;     // Prescale by 8.
    T1CONbits.nSYNC = 0;       // Synchronize to internal phase clocks.
    T1GCONbits.GE = 1;         // Disable gating, ie. always count.

    // Configure Capture/Compare/PWM unit 1.
    CCP1PPS = 0x11;            // RC1 trigger input.
    CCP1CAPbits.CTS = 0;       // CCP1PPS trigger input.
    CCP1CONbits.MODE = 0b0011; // Capture on every edge.
    PIR6bits.CCP1IF = 0;       // Clear CCP1 interrupt flag.
    PIE6bits.CCP1IE = 1;       // Enable CCP1 interrupt.
    CCP1CONbits.EN = 1;        // Enable CCP1.

    T1CONbits.ON = 1;          // Enable Timer1.
}
/*
 * Check whether a pulse interval is roughly equal to a target interval.
 */
uint8_t ir_match_length(const uint16_t pulse, const uint16_t target) {
    return pulse > target - 200 && pulse < target + 200;
}

/*
 * Convert IR clock ticks to microseconds.
 */
uint16_t ir_ticks_to_us(const uint16_t ticks) {
    return ticks/IR_TICKS_PER_USEC;
}

/*
 * Get a pulse interval and the corresponding pin state from the IR ring buffer.
 */
uint8_t ir_get_pulse(uint8_t* pin_state, uint16_t* interval, uint8_t idx) {
    if (rb_get(&interval_buffer, interval, idx) == RB_NO_DATA) { return 0; }
    *pin_state = (*interval >> 15);
    *interval &= ~(1 << 15);
    return 1;
}

uint8_t ir_parse_next(uint8_t* data) {
    uint32_t tmp = 0;

    for (uint8_t offset = 0; ; offset++) {
        uint16_t interval = 0;
        uint8_t pin_state = 0;

        // Match AGC high period.
        if (!ir_get_pulse(&pin_state, &interval, offset)) { return 0; }
        if (!(pin_state == 1 && ir_match_length(interval, NEC_AGC_HIGH))) { continue; }

        // Match AGC low period.
        if (!ir_get_pulse(&pin_state, &interval, offset + 1)) { return 0; }
        if (!(pin_state == 0 && ir_match_length(interval, NEC_AGC_LOW))) { continue; }

        // Read received data.
        uint8_t i;
        for (i = 0; i < 2*(4*8); i += 2) {
            // Match byte high period.
            if (!ir_get_pulse(&pin_state, &interval, offset + 2 + i)) { return 0; }
            if (!(pin_state == 1 && ir_match_length(interval, NEC_PULSE))) { break; }

            // Match byte low period.
            if (!ir_get_pulse(&pin_state, &interval, offset + 3 + i)) { return 0; }
            if (pin_state == 0) {
                if (ir_match_length(interval, NEC_ONE)) {
                    // Bit is 1.
                    tmp >>= 1;
                    tmp |= (1UL << 31);
                } else if (ir_match_length(interval, NEC_ZERO)) {
                    // Bit is 0.
                    tmp >>= 1;
                } else {
                    // Bit is invalid.
                    break;
                }
            } else {
                // Bit is invalid.
                break;
            }
        }

        // Break if a whole message was received.
        if (i == 2*(4*8)) { break; }
    }

    // Pop the read values from the IR ring buffer.
    uint16_t dummy = 0;
    for (uint8_t i = 0; i < 2 + 2*(4*8); i++) { rb_pop(&interval_buffer, &dummy); }

    // Check that the inverted and non-inverted data bytes match.
    if (((~tmp & 0xff000000) >> 8) == (tmp & 0x00ff0000)) {
        *data = (tmp & 0x00ff0000) >> 16;
    }

    return 1;
}

void ir_isr(void) {
    if (PIE6bits.CCP1IE && PIR6bits.CCP1IF) {
        uint16_t interval = ir_ticks_to_us(CCPR1);
        if ((interval & (1U << 15)) == 0) {
            interval |= (uint16_t) (PORTCbits.RC1 << 15);
        } else {
            interval = 0;
        }
        rb_push(&interval_buffer, interval, RB_OVERFLOW_DISCARD_END);

        // Clear the interrupt flag and Timer 1 counter.
        PIR6bits.CCP1IF = 0;
        TMR1 = 0;
    }
}