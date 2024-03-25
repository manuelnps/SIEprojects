#include <xc.h>
#include <sys/attribs.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "timer.h"
#define SYSCLK  80000000L // System clock frequency, in Hz
#define PBCLOCK 40000000L
const uint16_t prescaler2[] = {1, 2, 4, 8, 16, 32, 64, 256};
const uint8_t size_of_prescaler2 = 8;

int8_t Timer2(uint32_t Frequency)
{
    
    /*Variables*/
    uint8_t i = 0;
    uint32_t pr2 = 0;

    /*Timer T3 configuration */
    do {
        if (i == size_of_prescaler2)
            return -1;
        pr2 = (PBCLOCK / (Frequency * prescaler2[i])) - 1;
        T2CONbits.TCKPS = i;                                       
        i++;
    } while (pr2 > UINT16_MAX);
  
    
    //PR2=19999;
  // T2CONbits.TCKPS=0;
    PR2=pr2;
    TMR2 = 0;            
                                        
    T2CONbits.ON = 0;   // Stop timer
    IFS0bits.T2IF = 0;    // Reset interrupt flag
    IPC2bits.T2IP = 4;    //set interrupt priority (1..7) *** Make sure it matches iplx in isr declaration ***
    IEC0bits.T2IE = 0;  // Disable T3 interrupts / Enable T2 interrupt
    T2CONbits.TON = 1;
    return 0;
}
   int Timer3(float freq)
{

    /*Variables*/
    uint8_t i = 0;
    uint32_t pr3 = 0;

    /*Timer T3 configuration */
 
       do {
        if (i == size_of_prescaler2)
            return -1;
        pr3 = (PBCLOCK / (freq * prescaler2[i])) - 1;
        T3CONbits.TCKPS = i;                                       
        i++;
    } while (pr3 > UINT16_MAX);
   
    
   // PR2=19999;
   //T3CONbits.TCKPS=0;
    PR3=pr3;
    TMR3 = 0;            
                                        
    T3CONbits.ON = 0;   // Stop timer
    IFS0bits.T3IF = 0;    // Reset interrupt flag
    IPC3bits.T3IP = 5;    //set interrupt priority (1..7) *** Make sure it matches iplx in isr declaration ***
    IEC0bits.T3IE = 1;  // Disable T3 interrupts / Enable T2 interrupt
    T3CONbits.TON = 1;
}
   void Timer2Start(void)
{
    T2CONbits.TON = 1; 
    
    
}
   void Timer3Start(void)
{
    T3CONbits.ON = 1; 
 
}
   int8_t Timer5(uint32_t Frequency)
{
    
    /*Variables*/
    uint8_t i = 0;
    uint32_t pr5 = 0;

    /*Timer T3 configuration */
    do {
        if (i == size_of_prescaler2)
            return -1;
        pr5 = (PBCLOCK / (Frequency * prescaler2[i])) - 1;
        T5CONbits.TCKPS = i;                                       
        i++;
    } while (pr5 > UINT16_MAX);
  
    
    //PR2=19999;
  // T2CONbits.TCKPS=0;
    PR5=pr5;
    TMR5 = 0;            
                                        
    T5CONbits.ON = 0;   // Stop timer
    IFS0bits.T5IF = 0;    // Reset interrupt flag
    IPC5bits.T5IP = 5;    //set interrupt priority (1..7) *** Make sure it matches iplx in isr declaration ***
    IEC0bits.T5IE = 1;  // Disable T3 interrupts / Enable T2 interrupt
    T5CONbits.TON = 1;
    return 0;
}