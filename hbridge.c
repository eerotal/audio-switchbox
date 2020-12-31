
#include <stdint.h>
#include <pic16f15344.h>
#include "hbridge.h"

volatile static uint8_t *hb_port = 0;
static uint8_t hb_lpin_mask = 0;
static uint8_t hb_rpin_mask = 0;

void hbridge_setup(volatile uint8_t* port, uint8_t lpin_mask, uint8_t rpin_mask) {
    hb_port = port;
    hb_lpin_mask = lpin_mask;
    hb_rpin_mask = rpin_mask;

    hbridge_set_direction(0);
}

void hbridge_set_direction(int8_t dir) {
    if (dir > 0) {
        *hb_port &= ~hb_rpin_mask;
        *hb_port |= hb_lpin_mask;
    } else if (dir < 0) {
        *hb_port &= ~hb_lpin_mask;
        *hb_port |= hb_rpin_mask;
    } else if (dir == 0) {
        *hb_port &= ~(hb_lpin_mask | hb_rpin_mask);
    }
}