

#include <xc.h>
#include "PWM.h"
#include <stdint.h>

void PWMInit(void)
{
    TRISDbits.TRISD0 = 0;
    PORTDbits.RD0 = 1;
    
    OC1CONbits.OCM=6;
    OC1CONbits.ON = 0;
    OC1CONbits.OC32 = 0;
    OC1RS = 0;
    OC1CONbits.ON = 1;
}

void setPWM(unsigned int duty)
{
    OC1RS=((PR2+1)*duty/100);
}
