#include <stdint.h>
#include <pic16f15344.h>
#include "adc.h"

volatile static uint16_t adc_last_result = 0;

void adc_setup(void) {
    INTCONbits.PEIE = 1; // Peripheral interrupt enable.
    INTCONbits.GIE = 1; // Global interrupt enable.
    PIR1bits.ADIF = 0; // Clear ADC conversion interrupt flag.
    PIE1bits.ADIE = 1; // ADC conversion interrupt enable.
    
    // Initialize ADC with some default settings.
    adc_disable();
    adc_set_channel(ADCON0_CHSvalues.AVSS);
    adc_set_format(ADCON1_ADFMvalues.RJUST);
    adc_set_clock(ADCON1_ADCSvalues.FOSC64);
    adc_set_pos_ref(ADCON1_ADPREFvalues.FVR);
}

void adc_enable(void) {
    ADCON0bits.ADON = 1;
    ADCON0bits.GOnDONE = 1;
}

void adc_disable(void) {
    ADCON0bits.ADON = 0;
}

void adc_set_channel(uint8_t chs) {
    ADCON0bits.CHS = chs;
}

void adc_set_format(uint8_t adfm) {
    ADCON1bits.ADPREF = adfm;
}

void adc_set_clock(uint8_t adcs) {
    ADCON1bits.ADCS = adcs;
}

void adc_set_pos_ref(uint8_t adpref) {
    ADCON1bits.ADPREF = adpref;
}

uint16_t adc_get_result(void) {
    return adc_last_result;
}

void adc_isr(void) {
    if (PIE1bits.ADIE && PIR1bits.ADIF) {
        // Store conversion result.
        adc_last_result = ADRES;

        // Start next conversion.
        PIR1bits.ADIF = 0;
        ADCON0bits.GOnDONE = 1;
    }
}