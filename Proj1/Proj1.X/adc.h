

#ifndef _ADC_H
#define	_ADC_H

#ifdef __cplusplus
extern "C" {
#endif


int8_t ADCConfig(void);
int8_t ADCStart(void);
int8_t ADCStop(void);
int8_t setADChannel(unsigned int ch);

#ifdef __cplusplus
}
#endif

#endif	

