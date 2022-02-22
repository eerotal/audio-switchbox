#include <stdint.h>
#include <pic16f15344.h>

#include "drivers/inc/adc.h"

void adc_setup(void) {
    // Enable ADC.
    ADCON1bits.ADCS = 0b110; // Clock = Fosc/64.
    ADCON1bits.ADFM = 1;     // Right justified result.
    ADCON1bits.ADPREF = 0;   // Vref = VDD.
    ADCON0bits.ADON = 1;     // Enable ADC.
}

void adc_set_channel(uint8_t chs) {
    ADCON0bits.CHS = chs;
}

uint16_t adc_read(void) {
    ADCON0bits.GOnDONE = 1;
    while (ADCON0bits.GOnDONE);
    return ADRES;
}