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
 * Setup the ADC driver.
 */
void adc_setup(void);

/**
 * Select the ADC input channel.
 * 
 * @param chs uint8_t One of the values defined in ADCON0_CHSvalues.
 */
void adc_set_channel(uint8_t chs);

uint8_t adc_read_high(void);

#endif	/* ADC_H */

