/* 
 * File:   potentiometer.h
 * Author: Eero Talus
 * 
 * Motorized potentiometer control using a H-bridge driven motor.
 *
 * Created on 14 November 2020, 18:30
 */

#ifndef POTENTIOMETER_H
#define	POTENTIOMETER_H

#include <stdint.h>

/**
 * Setup the potentiometer driver.
 */
void pot_setup(void);

/**
 * Set the position of the potentiometer.
 * 
 * @param pos Position as an integer where 0 = 0% and 65536 = 100%.
 */
void pot_set_position(uint16_t pos);

#endif	/* POTENTIOMETER_H */

