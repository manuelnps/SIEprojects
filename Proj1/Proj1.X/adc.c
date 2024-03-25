#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include "adc.h"
/* Define prototypes (public interface) */

int8_t ADCConfig(void){
    
    AD1CON1bits.SSRC = 7; // Internal counter ends sampling and starts conversion
    AD1CON1bits.CLRASAM = 1; //Stop conversion when 1st A/D converter interrupt is generated and clears ASAM bit automatically
    AD1CON1bits.FORM = 0; // Integer 16 bit output format
    AD1CON2bits.VCFG = 0; // VR+=AVdd; VR-=AVss
    AD1CON2bits.SMPI = 15; // Number (+1) of consecutive conversions, stored in ADC1BUF0...ADCBUF{SMPI}
    AD1CON3bits.ADRC = 1; // ADC uses internal RC clock
    AD1CON3bits.SAMC = 16; // Sample time is 16TAD ( TAD = 100ns)
    // Set AN0 as input
    AD1CHSbits.CH0SA = 0; // Select AN0 as input for A/D converter
    TRISBbits.TRISB0 = 1; // Set AN0 to input mode
    AD1PCFGbits.PCFG0 = 0; // Set AN0 to analog mode
    TRISBbits.TRISB1 = 1; // Set AN1 to input mode
    AD1PCFGbits.PCFG1 = 0; // Set AN1 to analog mode
    
    
    
    IFS1bits.AD1IF = 0;
    
    AD1CON1bits.ON = 1; // Enable A/D module (This must be the ***last instruction of configuration phase***)
    
    return 0;
}

int8_t setADChannel(unsigned int ch){
    
    if (ch == 1)
    {
        AD1CHSbits.CH0SA = 1;
    }
    else AD1CHSbits.CH0SA = 0;
    
    return 0;
    
}

int8_t ADCStart(void){
    
    AD1CON1bits.ASAM = 1;         // Start conversion
    return 0;
    
}

int8_t ADCStop(void)
{
    AD1CON1bits.ASAM = 0;         // Stop conversion
    return 0; 
}
