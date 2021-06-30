#include <stdlib.h>
#include <stdint.h>
#include <pic16f15344.h>

#include "potentiometer.h"
#include "adc.h"

#define VOLUME_TICK 10
#define POT_DEADZONE 1

static uint8_t setpoint = 0;

void pot_setup(void) {
    // Setup pins.
    ANSELCbits.ANSC4 = 0; // RC4 = Digital
    ANSELCbits.ANSC5 = 0; // RC5 = Digital

    TRISCbits.TRISC4 = 0; // RC4 = Output
    TRISCbits.TRISC5 = 0; // RC5 = Output

    ODCONCbits.ODCC4 = 0; // RC4 = Push-pull
    ODCONCbits.ODCC5 = 0; // RC5 = Push-pull
}

void pot_volume_up(void) {
    if ((int16_t) setpoint + VOLUME_TICK <= UINT8_MAX) {
        setpoint += VOLUME_TICK;
    } else {
        setpoint = UINT8_MAX;
    }
}

void pot_volume_down(void) {
    if ((int16_t) setpoint - VOLUME_TICK >= 0) {
        setpoint -= VOLUME_TICK;
    } else {
        setpoint = 0;
    }
}

void pot_update(void) {
    adc_set_channel(ADCON0_CHSvalues.RC3);
    uint8_t result = adc_read_high();

    if (abs((int16_t) setpoint - (int16_t) result) > POT_DEADZONE) {
        if (result < setpoint) {
            LATCbits.LATC4 = 1;
            LATCbits.LATC5 = 0;
        } else {
            LATCbits.LATC4 = 0;
            LATCbits.LATC5 = 1;
        }
    } else {
        LATCbits.LATC4 = 0;
        LATCbits.LATC5 = 0;
    }
}