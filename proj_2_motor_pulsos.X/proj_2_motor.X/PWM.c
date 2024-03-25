

#include <xc.h>
#include "PWM.h"
#include <stdint.h>
#include <math.h>


void PWMInit(void)
{
    OC1CONbits.OCM=6;
    OC1CONbits.OCTSEL=0;
    OC1CONbits.ON = 1;
}

void PWMStart()
{
   OC1CONbits.ON = 1;     // Enable OC1
   
   
}

void PWMSetDutyCycle(float dutycycle)
{
   OC1RS=(uint16_t)((PR2+1)*dutycycle/100.0);
}