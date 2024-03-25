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
#include "adc.h"
#include "adc2temp.h"

#define SYSCLK 80000000L  // System clock frequency, in Hz
#define PBCLOCK 40000000L // Peripheral Bus Clock frequency, in Hz
#define SAMP_FREQ 0.2
#define SAMP 5

//Project Specs
#define TMAX 100.0
#define TMIN 40.0
#define STOP 0.0

//PI 
#define WINDUP 80.0
#define Kp 14.9480175795440200 //parametros obtidos a partir dos testes feitos em malha aberta,
#define Ti 14.948017579544020/0.301946286854135 //que nos deram a funçao transferencia
float posRail = 3.35; //Por vezes o rail 3.3V não estava a 3.3V, então utilizamos esta variavél para alterar mais facilmente no adc2temp
float tempInput = 0.0;//temperatura introduzida pelo utilizador
float temp = 0.0;
float tempthermistor = 0.0; //temperatura do termistor, inicializada a 0 para não obtermos falsas leituras antes do utilizador premir qualquer tecla
float tempthermocouple = 0.0;  //temperatura do termopar, inicializada a 0 para não obtermos falsas leituras antes do utilizador premir qualquer tecla
float tempPeak = 0.0; //temperatura de pico
float tempMed = 0.0; //temperatura medida, que é a soma da temperatura do termístor com o termopar
float tempAmb = 0.0; //temperatura que está a ser medida pelo termístor
float erro = 0.0; //diferença entre a temperatura introduzida, e a temperatura atual do sistema (em graus)
float erroAnterior = 0.0;
float uAnt = 0.0;
const float h = 1 / SAMP_FREQ;
//uint32_t rpmInput = 15;
float Prop = 0.0;
float Int = 0.0;
float vector[SAMP];
float previous_err = 0.0;
float newDutyCycle = 0.0;
float u = 0;
char comute = 1;
const uint32_t freq_pre = 40000000 / 256;
uint8_t togbreak = 0;
double t = 0.0;
uint8_t stop = 0;



volatile uint32_t countTimer = 0; //timer para assegurar a frequência de amostragem na interface gráfica

volatile char flag = 0;
volatile char flag5hz = 0;
//UI
char x;

typedef enum {
    show_menu,
    read_temp,
    set_temp,
    stop_start,
} m;

int i;
int *p = (int *) (&ADC1BUF0);
unsigned int adc_read = 0;
float avg_val = 0.0;

float ADC2; //adc do termistor
float ADC1; //adc do termopar

void PI(void);
float filtroDigital(float newSamp);
int time=0;
int main(int argc, char **argv) {
    memset(vector, 0, SAMP);

    TRISEbits.TRISE7 = 0;
    TRISEbits.TRISE8 = 1; // Set pin as input
    TRISDbits.TRISD2 = 1; // D2 as digital input


    if (UartInit(PBCLOCK, 115200) != UART_SUCCESS) {
        PORTAbits.RA3 = 0; // If Led active error initializing UART
        while (1);
    }
    __XC_UART = 1; /* Redirect stdin/stdout/stderr to UART1*/

    INTCONSET = _INTCON_MVEC_MASK;
    __builtin_enable_interrupts();

    Timer2(10);
    Timer3(10);
    ADCConfig();
    PWMInit();
    PWMStart();
    char z[2];
    char count_set = 0;
    m menu = show_menu;
    int i = 0;
    int *p = (int *)(&ADC1BUF0);
    int adc_read0 = 0;
    int adc_read1 = 0;
    char flagpress = 0; // flag só entrarmos no ciclo de leituras caso seja premida uma tecla
    while (1) {
       if (!flag5hz) {
            x = GetChar();
            switch (x) {
                case 'm':
                    menu = show_menu;
                    flagpress = 1;
                    break;
                case 't':
                    menu = set_temp;
                     flagpress = 1;
                    break;
                case 'l':
                    menu = read_temp;
                     flagpress = 1;
                    break;
                case 's':
                    menu = stop_start;
                    tempInput = 0.0;
                    flagpress = 1;
                    break;
                default:
                    break;
            }
            switch (menu) {
                case show_menu:
                    printf("\e[1;1H\e[2J");
                    printf(
                            "m - Mostrar menu\n\r"
                            "t - Mudar a Temperatura\n\r"
                            "l - Ler a Temperatura\n\r"
                            "s - Parar o Sistema \n\r");
                    break;

                case set_temp:
                    printf("\e[1;1H\e[2J");
                    printf("Introduza a nova temperatura:\n\r");
                    break;

                case read_temp:
                    printf("Temperatura introduzida: %1.f,Temperatura Atual: %.1f ( Temperatura de pico %.1f, Temperatura ambiente %.2f, Temperatura termopar %.2f, Erro:%.1f )\n\r", tempInput, tempMed, tempPeak, tempAmb, tempthermocouple, erro);
                    break;

                case stop_start:
                    printf("\e[1;1H\e[2J");
                    printf("System stopped\r\n");
                    togbreak = !togbreak; //para poder parar o sistema e continuar onde estava se voltarmos a premir a mesma tecla
                    if (togbreak)
                        menu = stop_start;
                    else
                        menu = read_temp;
                        break;
                default:
                    break;
            }
            
            //para ler a temperatura do terminal
            if (menu == set_temp && x >= '0' && x <= '9') {
                z[count_set] = x - '0';
                count_set++;
                if (count_set == 2) {
                    menu = read_temp;
                    tempInput = z[0]*10 + z[1];

                    count_set = 0;
                }
                //Para assegurar que o intervalo vai dos 40º - 100ºC 
                if (tempInput > 100) {
                    tempInput = 100;
                } else if (tempInput < 40) {
                    tempInput = 40;
                }
            }
            flag5hz = 1;
        }
        
        if (flag&&flagpress) {
            //Leitura da ADC termopar 
            AD1CHSbits.CH0SA = 0; // Select AN0 as input for A/D converter
            ADCStart();
            while (IFS1bits.AD1IF == 0);
            adc_read0 = 0;
            for(i=0; i<16; i++)
            adc_read0 += p[i*4]; 
            adc_read0 = adc_read0/16;
            tempthermocouple = (adc_read0 * 3.3) / 1023;
            ADC1 = tempthermocouple;
            IFS1bits.AD1IF = 0;
            
            //Leitura da ADC termistor
            AD1CHSbits.CH0SA = 1;  //Select AN1 as input for A/D converter
            ADCStart();
            while (IFS1bits.AD1IF == 0);
            adc_read1 = 0;
            for(i=0; i<16; i++)
            adc_read1+= p[i*4]; 
            adc_read1 = adc_read1/16;
            tempAmb = (adc_read1 * 3.3) / 1023;
            ADC2 = tempAmb;
            IFS1bits.AD1IF = 0;
            
            //Cálculos de temperatura
            tempthermocouple = adc2thermocouple(tempthermocouple);
            tempAmb = adc2thermistor(tempAmb,posRail);
            tempMed = tempAmb + tempthermocouple; //temperatura total
            
            //Comparador da temperatura de pico
            if (tempMed > tempPeak) {
                tempPeak = tempMed;
            }

            PI();
            flag = 0;
        }
    }
}

void PI(void) {
    static float s0 = Kp * (1 + (h / Ti));
    static float s1 = -Kp;

    erro = tempInput - tempMed;

    u = (s0 * erro) + (s1 * erroAnterior) + uAnt;

    newDutyCycle = u;

    if (u < 0.0) {
        u = 0.0;
    }
    if (u > 100.0) {
        u = 100.0;
    }
    uAnt = u;
    erroAnterior = erro;

    PWMSetDutyCycle(100 - u);
    flag = 0;

}

float filtroDigital(float newSamp) //filtro de médias
{
    uint32_t i;
    for (i = SAMP - 1; i > 0; i--) {
        vector[i] = vector[i - 1];
    }
    vector[0] = newSamp;
    float media = 0;
    for (i = 0; i < SAMP; i++) {
        media += vector[i];
    }
    media /= SAMP;

    return media;
}

void __ISR(24) isr_uart1(void) {
    IFS0bits.U1RXIF = 0;
}

void __ISR(_TIMER_2_VECTOR, IPL4AUTO) T2Interrupt(void) // assegurar frequencia de PWM
{

    IFS0bits.T2IF = 0;
}

int counter2 = 0;

void __ISR(_TIMER_3_VECTOR, IPL5AUTO) T3Interrupt(void) //assegurar frequencia de amostragem
{
    uint32_t div = (uint32_t) (10 / SAMP_FREQ);
    uint32_t divToShow = (uint32_t) (10 / 5);

    PORTEbits.RE7 = !PORTEbits.RE7;
    countTimer++;    
    counter2++;
    if ((countTimer % div) == 0)
        flag = 1;
    if ((counter2 % divToShow) == 0)
        flag5hz = 0;
    IFS0bits.T3IF = 0;
}

