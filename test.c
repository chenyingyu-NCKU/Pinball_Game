#include "setting_hardaware/setting.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include <pic18f4520.h>
// using namespace std;

char str[20];
int mode;
int s1 = 0;
int s2 = 0;

void Mode1(){   // Todo : Mode1 
    
    return ;
}
void Mode2(){   // Todo : Mode2 
    return ;
}
void delay(){
    for(int i = 0; i<20000; i++){
        
    }
}
void main(void) 
{   
    //light
    //set RA0~4 = output

    SYSTEM_Initialize() ;
   // TRISA = 0;
    //LATA = 0;
    
    // Timer2 -> On, prescaler -> 16, postscale > 2
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b11; 
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    /** 
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0x9b + 1) * 4 * 1 µs * 16 
     * = 0.019968s ~= 20ms
     * 
     * = 5ms
     */
    PR2 = 77;
    /**
     * Duty cycle
     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
     * = (0x0b*4 + 0b01) * 0.5µs * 16
     * = 0.00144s ~= 1450µs
     * 
     * = 362.5
     */
    CCPR1L = 20; // 0 deg
    CCP1CONbits.DC1B = 0b10;
    // motor output (CCP1/RC2)
    TRISC = 0;
    LATC = 0;
    
    TRISD = 0;
    LATD = 0;
    int m1Led[2] = {5, 10};
    int m2Led[4] = {8, 4, 2, 1};
    
    while(1) {
        strcpy(str, GetString()); // TODO : GetString() in uart.c
        unsigned int lenStr = strlen(str);
        if(str[0]=='m' && str[1]=='1'){ // Mode1
            Mode1();
            mode = 1;
            ClearBuffer();
        }
        else if(str[0]=='m' && str[1]=='2'){ // Mode2
            Mode2();
            mode = 2;
            ClearBuffer();  
        }else if(str[0]=='e'){
            mode = 0;
            ClearBuffer();
            //LATA = 0;
            LATD = 0;
        }else
        {
            if(lenStr == 1 && str[0] < 58 && str[0] > 47)
               // LATA = str[0] - '0';
                LATD = str[0] - '0';
            else if(lenStr == 2 && str[0] < 58 && str[0] > 47 && str[1] < 58 && str[1] > 47)
                //LATA = (str[0] - '0')*10  + str[1] - '0';
                LATD = (str[0] - '0')*10  + str[1] - '0';

        }

        if (mode == 1) {
            //LATA = m1Led[s1++];
            LATD = m1Led[s1++];
            s1 = s1%2;
            delay();
        } else if (mode == 2) {
            //LATA = m2Led[s2++];
            LATD = m2Led[s2++];
            s2 = s2%4;
            delay();
        }

    }
    return;
}

void __interrupt(high_priority) Hi_ISR(void)
{

}