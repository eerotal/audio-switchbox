#include <stdlib.h>
#include <stdint.h>
#include <pic16f15344.h>

#include "potentiometer.h"
#include "adc.h"

#define VOLUME_TICK 8
#define POT_DEADZONE 2
#define UINT10_MAX 1024UL

static uint16_t setpoint = 0;

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
    if ((int16_t) setpoint + VOLUME_TICK <= UINT10_MAX) {
        setpoint += VOLUME_TICK;
    } else {
        setpoint = UINT10_MAX;
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
    uint16_t result = adc_read();

    if (abs((int16_t) setpoint - (int16_t) result) > POT_DEADZONE) {
        if (result < setpoint) {
            // Motor clockwise.
            LATCbits.LATC4 = 1;
            LATCbits.LATC5 = 0;
        } else {
            // Motor counterclockwise.
            LATCbits.LATC4 = 0;
            LATCbits.LATC5 = 1;
        }
    } else {
        // Motor off.
        LATCbits.LATC4 = 0;
        LATCbits.LATC5 = 0;
    }
}