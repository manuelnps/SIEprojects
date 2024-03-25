/* 
 * File:   adc2temp.h
 * Author: manue
 *
 * Created on 4 de Abril de 2022, 10:07
 */

#ifndef ADC2TEMP_H
#define	ADC2TEMP_H

#ifdef	__cplusplus
extern "C" {
#endif

float adc2thermistor(float,float);
float adc2thermocouple(float);
#ifdef	__cplusplus
}
#endif

#endif	/* ADC2TEMP_H */

