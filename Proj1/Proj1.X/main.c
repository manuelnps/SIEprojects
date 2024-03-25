#include <xc.h>
#include <sys/attribs.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "config_bits.h" // NOTE!! Must precede project file includes
#include "adc.h"
#include "adc2mm.h"
#include "uart.h"
#include "PWM.h"
#include "timer.h"


#define SYSCLK  80000000L // System clock frequency, in Hz
#define PBCLOCK 40000000L // Peripheral Bus Clock frequency, in Hz

int main(int argc, char** argv){
   
    
    UartInit(PBCLOCK,115200);
    __XC_UART = 1;
    
    Timer2();

    INTCONSET = _INTCON_MVEC_MASK;
     
    DDPCONbits.JTAGEN = 0;
    
    
    PWMInit();

    
    bool chToggle = false;
    
    unsigned int avg_val0 = 0;
    unsigned int avg_val1 = 0;
    unsigned int avg_size = 5000;
    unsigned int adc_read = 0;
    
    float curr_dist_adc;
    
    TRISBbits.TRISB12 = 0; 
    
    int i = 0;
    int *p = (int *)(&ADC1BUF0);
    unsigned int avg_counter = 0;
 
    ADCConfig();
    
    while(1)
    {
        
        if (avg_counter >= avg_size)
        {
            
            curr_dist_adc = adc2mm(2*avg_val0/avg_size, 2*avg_val1/avg_size);
            if (curr_dist_adc != 100000)
            {
                printf("Distance (mm): %.0f\n\r",curr_dist_adc); 
                LATBbits.LATB12 = 1;
                
            }
            
            
            
            else 
            {
              printf("\n\rE"); 
              LATBbits.LATB12 = 0;
            }
            //printf("ADC0: %d | ADC1 %d\n\r",2*avg_val0/avg_size,2*avg_val1/avg_size );
            
            setPWM((600-(int)curr_dist_adc)/5);
            avg_val0 = 0;
            avg_val1 = 0;
            avg_counter = 0;
        }
        
        if(chToggle)
        {
            setADChannel(1);
        }
        else setADChannel(0);
        
        adc_read = 0;
        
        ADCStart();
        
        while (IFS1bits.AD1IF == 0); // Wait fo EOC

        for(i=0; i<16; i++)
            //adc_v += (int)((p[i*4]*3300+511)/(1024-1));
            adc_read += p[i*4]; 
        
        if(chToggle)
        {
            avg_val1 += adc_read/16; 
        }
        else avg_val0 += adc_read/16; 
        
        IFS1bits.AD1IF = 0; // Reset interrupt flag
        chToggle = !chToggle;
        avg_counter++;
          
    }
    
}


