/* 
 * File:   adc.h
 * Author: eero
 *
 * Created on 31 December 2020, 17:18
 */

#ifndef ADC_H
#define	ADC_H

/**
 * Possible values for the ADCON0[7:2] CHS bits.
 * 
 * @see http://ww1.microchip.com/downloads/en/devicedoc/40001889a.pdf page 235
 */
const struct {
    uint8_t FVR_BUF2_REF;
    uint8_t RVR_BUF1_REF;
    uint8_t DAC1_OUT;
    uint8_t TEMP_OUT;
    uint8_t AVSS;
    uint8_t RC7;
    uint8_t RC6;
    uint8_t RC5;
    uint8_t RC4;
    uint8_t RC3;
    uint8_t RC2;
    uint8_t RC1;
    uint8_t RC0;
    uint8_t RB7;
    uint8_t RB6;
    uint8_t RB5;
    uint8_t RB4;
    uint8_t RA5;
    uint8_t RA4;
    uint8_t RA3;
    uint8_t RA2;
    uint8_t RA1;
    uint8_t RA0;
} ADCON0_CHSvalues = {
    0b111111,
    0b111110,
    0b111101,
    0b111100,
    0b111011,
    0b010111,
    0b010110,
    0b010101,
    0b010100,
    0b010011,
    0b010010,
    0b010001,
    0b010000,
    0b001111,
    0b001110,
    0b001101,
    0b001100,
    // 0b0011011-0b000110 = Reserved
    0b000101,
    0b000100,
    0b000011,
    0b000010,
    0b000001,
    0b000000
};

/**
 * Possible values for the ADCON1[7] ADFM bit.
 * 
 * @see http://ww1.microchip.com/downloads/en/devicedoc/40001889a.pdf page 236
 */
const struct {
    uint8_t RJUST;
    uint8_t LJUST;
} ADCON1_ADFMvalues = {
    1,
    0
};

/**
 * Possible values for the ADCON1[6:4] ADCS bits.
 * 
 * @see http://ww1.microchip.com/downloads/en/devicedoc/40001889a.pdf page 236
 */
const struct {
    uint8_t ADCRC;
    uint8_t FOSC64;
    uint8_t FOSC16;
    uint8_t FOSC4;
    uint8_t FOSC32;
    uint8_t FOSC8;
    uint8_t FOSC2;
} ADCON1_ADCSvalues = {
    0b111,
    0b110,
    0b101,
    0b100,
    // 0b011 = Also ADCRC
    0b010,
    0b001,
    0b000
};

/**
 * Possible values for the ADCON1[1:0] ADPREF bits.
 * 
 * @see http://ww1.microchip.com/downloads/en/devicedoc/40001889a.pdf page 236
 */
const struct {
    uint8_t FVR;
    uint8_t EXT;
    uint8_t VDD;
} ADCON1_ADPREFvalues = {
    0b11,
    0b10,
    // 0b01 = Reserved
    0b00
};

/**
 * Setup the ADC driver.
 */
void adc_setup(void);

/**
 * Enable the ADC peripheral and start conversion.
 */
void adc_enable(void) ;

/**
 * Disable the ADC peripheral.
 */
void adc_disable(void);

/**
 * Select the ADC input channel.
 * 
 * @param chs uint8_t One of the values defined in ADCON0_CHSvalues.
 */
void adc_set_channel(uint8_t chs);

/**
 * Select the ADC result format.
 * 
 * @param adfm uint8_t One of the values defined in ADCON1_ADFMvalues.
 */
void adc_set_format(uint8_t adfm);

/**
 * Select the ADC clock source.
 * 
 * @param adcs uint8_t One of the values defined in ADCON1_ADCSvalues.
 */
void adc_set_clock(uint8_t adcs);

/**
 * Select the positive ADC reference voltage source.
 * 
 * @param adpref uint8_t One of the values defined in ADCON1_ADPREFvalues.
 */
void adc_set_pos_ref(uint8_t adpref);

/**
 * Get the latest ADC conversion result.
 * 
 * @return The ADC conversion result.
 */
uint16_t adc_get_result(void);

#endif	/* ADC_H */

