#include <xc.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "adc2temp.h"

float adc2thermistor(float an1_val, float posRail)
{
    float thermistortemp = 0.0;
    thermistortemp = -32.6880*(posRail-an1_val)+79.7848;
    return thermistortemp;
}

float adc2thermocouple(float an0_val)
{
    float thermocoupletemp = 0.0;
    thermocoupletemp = an0_val - 0.312; //subtração da tensão de referência
    thermocoupletemp /= 600; // divisão pelo ganho antes da linearização
    thermocoupletemp = (24335.0*thermocoupletemp + 0.5014);
    return thermocoupletemp;
}
