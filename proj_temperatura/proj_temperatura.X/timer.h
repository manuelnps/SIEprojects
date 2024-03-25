#ifndef TIMER_H
#define    TIMER_H

#ifdef    cplusplus
extern "C" {
#endif
#define PBCLK 40000000L

int8_t Timer2(uint32_t);
int8_t Timer5(uint32_t);
void Timer2Start(void);
int Timer3(float);
void Timer3Start(void);



#ifdef    cplusplus
}
#endif

#endif    /* TIMER_H */