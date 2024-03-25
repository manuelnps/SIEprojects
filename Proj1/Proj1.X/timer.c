#include <xc.h>
#include <stdint.h>
#include "timer.h"


void Timer2(void)
{
    T2CONbits.ON = 0;
    T2CONbits.SIDL = 0;
    
    T2CONbits.TCKPS = 0;
    T2CONbits.T32 = 0;
    T2CONbits.TCS = 0;
    TMR2 = 0;
    
    PR2 = (PBCLK/(20000*3))-1;
    IEC0bits.T2IE = 0;
    IFS0bits.T2IF = 0;
    
    T2CONbits.ON = 1; //Turn on timer
}