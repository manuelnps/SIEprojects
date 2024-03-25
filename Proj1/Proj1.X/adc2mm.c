#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "adc2mm.h"

uint32_t adc2mm(unsigned int an0_val, unsigned int an1_val)
{
    
    float mm = 0;
    
    if (an0_val <= 1000 && an1_val >= 1000)
    {
        if ((an0_val > 462) && (an0_val <= 1010))
        {
            mm = -0.1086*an0_val + 206.81;
        }
        else if ((an0_val <= 462) && (an0_val > 284))
        {
            mm = -0.2184*an0_val + 259.09;
        }
        else if ((an0_val <= 284) && (an0_val > 93))
        {
            mm = -0.3232*an0_val + 288.51;
        }
        /*else if ((an0_val <= 93) && (an0_val >= 21))
        {
            mm = -0.5482*an0_val + 309.93;
        }*/
        else mm = -0.5482*an0_val + 309.93;      
    }
    else if (an0_val < 21 && an1_val >= 7)
    {
        if ((an1_val > 522) && (an1_val <= 1013))
        {
            mm = -0.197*an1_val + 495.32;
        }
        else if ((an1_val <= 522) && (an1_val > 195))
        {
            mm = -0.5319*an1_val + 603.54;
        }
        /*else if ((an1_val <= 195) && (an1_val >= 7))
        {
            mm = -0.3235*an1_val + 563.12;
        }*/
        else mm = -0.3235*an1_val + 563.12;        
    }
    else if ((an0_val >= 1011) && (an1_val >= 1015)) // 10>
    {
        mm = 100000;
    }
    
     else if ((an0_val <= 10) && (an1_val <= 10)) // 60<
    {
        mm = 100000;
    }
    else
    {
        mm = 100000;
    }
    
    return (uint32_t) roundf(mm);
}
