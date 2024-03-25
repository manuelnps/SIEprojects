#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/attribs.h>
#include <xc.h>

#include "uart.h"
#include "PWM.h"
#include "config_bits.h"
#include "timer.h"

#define SYSCLK 80000000L  // System clock frequency, in Hz
#define PBCLOCK 40000000L // Peripheral Bus Clock frequency, in Hz
#define SAMP_FREQ 250
#define PULSES_ROT 420.0
#define RPM_MAX 79.0
#define WINDUP 80.0
#define SAMP 10
#define Kp 1.1
#define Ti 0.8

int rpmInput = 42;

typedef enum
{
    set_vel,
    show_Menu,
    change_Dir,
    stop_start,
    read_val
} m;

volatile char flag5hz=0;

volatile uint16_t newTMR=0;
volatile uint32_t count = 0;
volatile uint32_t countTimer = 0;
volatile char getdirection = 1;
 char setDirection=0; 
volatile char flag = 0;
volatile uint32_t countPulses = 0;
volatile int UI = 1;

float rpm = 0.0;
float rpmMedio = 0.0;
float erro = 0.0;
//uint32_t rpmInput = 15;
float Prop = 0.0;
float Int = 0.0;
float vector[SAMP];

float erroAnterior = 0.0;
float newDutyCycle = 0.0;
float u = 0;
char comute = 1;
uint8_t stop = 0;

char x;
int choiceRPM;
uint8_t togbreak = 0;
char newRPM[2];
const float h = 1.0 / SAMP_FREQ;
float filtroDigital(float newSamp);
void ControladorPI(void);
const uint32_t freq_pre =40000000/64;
int main(int argc, char **argv)
{
    T4CONbits.TCKPS=6;
    TMR4=0;
    T4CONbits.ON=0;
    IFS0bits.T4IF=0;
    IPC4bits.T4IP=3;
    IEC0bits.T4IE=0;
    T4CONbits.TON=1;
    memset(vector,0,SAMP);
    m menu = show_Menu;
    TRISEbits.TRISE7 = 0;
    // Init UART and redirect tdin/stdot/stderr to UART
    if (UartInit(PBCLOCK, 115200) != UART_SUCCESS)
    {
        PORTAbits.RA3 = 0; // If Led active error initializing UART
        while (1)
            ;
    }

    __XC_UART = 1; /* Redirect stdin/stdout/stderr to UART1*/

    INTCONSET = _INTCON_MVEC_MASK;
    __builtin_enable_interrupts();

    Timer2(20000);
    Timer3(SAMP_FREQ);
    Timer5(5);
    PWMInit();

    TRISEbits.TRISE8 = 1; // Set pin as input
    TRISDbits.TRISD2 = 1; // D2 as digital input

    INTCONbits.INT1EP = 1; // Generate interrupts on {rising edge-1 falling edge - 0}
    IFS0bits.INT1IF = 0;   // Reset int flag
    IPC1bits.INT1IP = 2;   // Set interrupt priority (1..7) * Set equal to ilpx above
    IEC0bits.INT1IE = 1;   // Enable Int1 interrupts

    PORTEbits.RE7 = 0;

    char z[2];
    char count_set=0;
    while (1)
    {
       
        
        if (flag)
        {
        //    printf("toggle %d",togbreak);
          
            ControladorPI();
            flag = 0;
           // printf("rpm: %.1f\n\r",rpmMedio);
            // printf("count: %.1f\r\n ", u);
        }
        if(flag5hz){ 
            x = GetChar();
              
            

            /*este primeiro switch serve para definir um valor numa variavel para que quando o ciclo volte a acontecer
         * os valores n�o desapare�am. Assim podemos continuar numa parte do menu o tempo que quisermos.
         */
            switch (x)
            {
            case 'm':
                menu = show_Menu;
                break;
            case 'v':
                menu = set_vel;

                break;
            case 'd':
                //menu = change_Dir;
                setDirection = !setDirection;
                
                break;
            case 'l':
                menu = read_val;
                break;
            case 's':
                togbreak = !togbreak; //para alternar entre parado e a rodar
                stop==togbreak;
                if(togbreak)
                    menu=stop_start;
                else
                    menu=read_val;
                break;
            default:
                break;
            }

            switch (menu)
            {
            case show_Menu:
                printf("\e[1;1H\e[2J");
                printf(
                    "m - Mostrar menu\n\r"
                    "v - Mudar a velocidade\n\r"
                    "d - Mudar a direcao\n\r"
                    "l - Ler Velocidade e angulo\n\r"
                    "s - Parar/Continuar o motor \n\r");
                
                break;
                 
             
            case set_vel:
                printf("\e[1;1H\e[2J");
                printf("Introduza a nova velocidade:\n\r");
               

                break;

            case change_Dir:
                
                
               // printf("%d", direction);
                     
                break;

            case read_val:
                //fazer aqui um print da velocidade e do angulo
                //printf("\e[1;1H\e[2J");
                printf("RPM input: %d,RPM count: %.1f,direcao %d\r\n", rpmInput, rpm,getdirection);
                break;
            case stop_start:

               // printf("\e[1;1H\e[2J");
                printf("Motor stoped\r\n"); 
                printf("toggle %d\n\r",togbreak);
                break;
            default:
                break;
            }
            if(menu==set_vel && x>='0' && x<='9'){
                z[count_set]=x-'0';
                count_set++;
                if(count_set==2){
                    menu=read_val;
                    rpmInput=z[0]*10+z[1];
                    count_set=0;
                    
                }
            } 
        flag5hz=0;
        }
            
        // printf("count: %d\r\n ", countPulses);
    }
    // if(get_direction)
    //   printf("frente");
    // else
    //   printf("tras");
}

void ControladorPI(void)
{
    //rpm = //(float)(countPulses * 60.0 * SAMP_FREQ / PULSES_ROT);
    rpm=60.0/(((float)newTMR/freq_pre)*420.0);
    rpmMedio = filtroDigital(rpm);
    countPulses = 0;
    erro = rpmInput - rpmMedio;

    Prop = erro * Kp;

    Int += (0.5 * (erroAnterior + erro) * h);

    erroAnterior = erro;

    if (Int > WINDUP)
    {
        Int = WINDUP;
    }
    if (Int < -WINDUP)
    {
        Int = -WINDUP;
    }
    erroAnterior = erro;

    u = round(Prop + (Int * Ti));

    if (setDirection)
    {
        newDutyCycle = (RPM_MAX - u) * (50.0 / RPM_MAX);
    }
    else
    {
        newDutyCycle = (u + RPM_MAX) * (50.0 / RPM_MAX);
    }
    if (newDutyCycle > 100)
    {
        newDutyCycle = 100.0;
    }
    if (newDutyCycle < 0)
    {
        newDutyCycle = 0.0;
    }
    if ( togbreak || rpmInput < 10 || rpmInput > 50)
    {
        PWMSetDutyCycle(50.0);
    }
    else
    {
        PWMSetDutyCycle(newDutyCycle);
    }
   
    }

float filtroDigital(float newSamp)
{
    uint32_t i;
    for (i = SAMP - 1; i > 0; i--)
    {
        vector[i] = vector[i - 1];
    }
    vector[0] = newSamp;
    float media = 0;
    for (i = 0; i < SAMP; i++)
    {
        media += vector[i];
    }
    media /= SAMP;

    return media;
}

void __ISR(24) isr_uart1(void)
{
    IFS0bits.U1RXIF = 0;
}

void __ISR(_EXTERNAL_1_VECTOR, IPL2AUTO) ExtInt1ISR(void)
{
    //  printf("teste");
    newTMR=TMR4;
    TMR4=0;
    PORTEbits.RE7 = !PORTEbits.RE7;
    countPulses++;
    if (PORTDbits.RD2)
    {
        getdirection = 1;
    }
    else
    {
        getdirection = 0;
    }
    // printf("count pulses: %d\r\n",countPulses);
    IFS0bits.INT1IF = 0; // Reset interrupt flag
}

void __ISR(_TIMER_2_VECTOR, IPL4AUTO) T2Interrupt(void)
{

    IFS0bits.T2IF = 0;
}
void __ISR(_TIMER_5_VECTOR, IPL5AUTO) T5Interrupt(void)
{
    flag5hz=1;
    IFS0bits.T5IF = 0;
}

void __ISR(_TIMER_3_VECTOR, IPL5AUTO) T3Interrupt(void)
{
    PORTEbits.RE7 = !PORTEbits.RE7;

    countTimer++;
    flag = 1;
    IFS0bits.T3IF = 0;

    // rpm = (float) countPulses*60.0*SAMP_FREQ/PULSES_ROT;
    // printf("rpm: %d\n\r",countPulses);
    /* if(rpmInput==0){
         duty=50;
    }
    else{
    ControladorPI();  
    PWMSetDutyCycle(duty);
    }
    * */
}
