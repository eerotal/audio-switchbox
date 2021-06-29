/* 
 * File:   ir_receiver.h
 * Author: Eero Talus
 *
 * NEC IR receiver decoder.
 * 
 * Created on 11 October 2020, 20:19
 */

#ifndef IR_RECEIVER_H
#define	IR_RECEIVER_H

#include <stdint.h>

void ir_setup(void);
uint8_t ir_parse_next(uint8_t* data);
void ir_isr(void);

#endif	/* IR_RECEIVER_H */

