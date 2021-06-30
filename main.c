/* 
 * File:   main.c
 * Author: Eero Talus
 *
 * Created on 11 October 2020, 20:16
 */

// PIC16F15344 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FEXTOSC = ECH    // External Oscillator mode selection bits (EC above 8MHz; PFM set to high power)
#pragma config RSTOSC = HFINT32 // Power-up default value for COSC bits (HFINTOSC (32MHz))
#pragma config CLKOUTEN = OFF   // Clock Out Enable bit (CLKOUT function is disabled; i/o or oscillator function on OSC2)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (FSCM timer enabled)

// CONFIG2
#pragma config MCLRE = ON       // Master Clear Enable bit (MCLR pin is Master Clear function)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config LPBOREN = OFF    // Low-Power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = ON       // Brown-out reset enable bits (Brown-out Reset Enabled, SBOREN bit is ignored)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices)
#pragma config ZCD = OFF        // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR.)
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit can be cleared and set only once in software)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)

// CONFIG3
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = ON        // WDT operating mode (WDT enabled regardless of sleep; SWDTEN ignored)
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4
#pragma config BBSIZE = BB512   // Boot Block Size Selection bits (512 words boot block size)
#pragma config BBEN = OFF       // Boot Block Enable bit (Boot Block disabled)
#pragma config SAFEN = OFF      // SAF Enable bit (SAF disabled)
#pragma config WRTAPP = OFF     // Application Block Write Protection bit (Application Block not write protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block not write protected)
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration Register not write protected)
#pragma config WRTSAF = OFF     // Storage Area Flash Write Protection bit (SAF not write protected)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/Vpp pin function is MCLR.)

// CONFIG5
#pragma config CP = OFF         // UserNVM Program memory code protection bit (UserNVM code protection disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ir_receiver.h"
#include "ir_lg_akb75675311.h"
#include "adc.h"

/*
 * Main entry point.
 */
int main(int argc, char** argv) {
    ANSELCbits.ANSC0 = 0; // RC0 = Digital
	TRISCbits.TRISC0 = 0; // RC0 = Output
    PORTCbits.RC0 = 1;
	
	ir_setup();

    /*adc_setup();
    adc_set_channel(ADCON0_CHSvalues.RC6);*/

    // Setup the H-Bridge controller.
    /*ANSELCbits.ANSC4 = 0; // RC4 = Digital
    ANSELCbits.ANSC5 = 0; // RC5 = Digital
    ANSELCbits.ANSC6 = 1; // RC6 = Analog
    TRISCbits.TRISC4 = 0; // RC4 = Output
    TRISCbits.TRISC5 = 0; // RC5 = Output
    TRISCbits.TRISC6 = 1; // RC6 = Input
    pot_setup(&PORTC, _PORTC_RC4_MASK, _PORTC_RC5_MASK);
    pot_set_direction(-1);*/

	while (1) {
        for (uint16_t i = 0; i < 1000; i++);
        PORTCbits.RC0 = 0;

        uint8_t data = 0;
        if (ir_parse_next(&data) == IR_OK) {
            if (data == 0x11) {
                PORTCbits.RC0 = 1;
            }
        }
    }
    return (EXIT_SUCCESS);
}


void __interrupt() ISR(void) {
    ir_isr();
}

