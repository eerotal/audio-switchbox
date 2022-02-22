/* 
 * File:   timer.h
 * Author: eero
 *
 * Created on 22 February 2022, 23:50
 */

#ifndef TIMER_H
#define	TIMER_H

#include <stdint.h>

// TIMER0 constants.
#define TMR_FREQ 32000000UL // Hz
#define TMR_PRESCALE 256UL
#define TMR_TICK (uint8_t) (((float) 1000000/TMR_FREQ)*TMR_PRESCALE) // us

/*
 * Setup timers.
 */
void tmr_setup(void);

/*
 * Convert Timer 0 ticks to us.
 * 
 * @param uint16_t tick Time in ticks.
 * 
 * @return uint32_t Time in us.
 */
uint32_t tmr0_ticks_to_us(uint16_t tick);

/*
 * Read the 16-bit Timer 0 counter.
 * 
 * @return uint16_t The counter value.
 */
uint16_t tmr0_read(void);

/*
 * Write the 16-bit Timer 0 counter.
 * 
 * @param uint16_t The value to write.
 */
void tmr0_write(uint16_t x);

#endif	/* TIMER_H */

