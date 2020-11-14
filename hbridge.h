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
 * Setup the H-bridge driver.
 */
void hbridge_setup(void);

/**
 * Set the H-bridge driver output speed.
 *
 * @param The driver speed as an integer.
 */
void hbridge_set_speed(int16_t speed);

#endif	/* HBRIDGE_H */

