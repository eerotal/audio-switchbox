/* 
 * File:   hbridge.h
 * Author: Eero Talus
 *
 * H-bridge driver implementation.
 * 
 * Created on 14 November 2020, 18:28
 */

#ifndef HBRIDGE_H
#define	HBRIDGE_H

#include <stdint.h>

/**
 * Initialize the H-Bridge.
 *
 * @param port      A pointer to the PORTx register.
 * @param lpin_mask A bitmask for the left H-Bridge pin.
 * @param rpin_mask A bitmask for the right H-Bridge pin.
 */
void hbridge_setup(volatile uint8_t* port, uint8_t lpin_mask, uint8_t rpin_mask);

/**
 * Set the H-bridge driver output direction.
 *
 * @param int8_t The direction as an integer.
 */
void hbridge_set_direction(int8_t dir);

#endif	/* HBRIDGE_H */

