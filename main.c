#include "setting_hardaware/setting.h"
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pic18f4520.h>
#include <time.h>

#pragma config OSC = INTIO67 // Oscillator Selection bits
#pragma config WDT = OFF     // Watchdog Timer Enable bit 
#pragma config PWRT = OFF    // Power-up Enable bit
#pragma config BOREN = SBORDIS   // Brown-out Reset Enable bit
#pragma config PBADEN = ON     // Watchdog Timer Enable bit 
#pragma config LVP = OFF     // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF     // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)

#define _XTAL_FREQ 4000000

int difficulty = 0; // level difficulty
int cur_play_times = -1; // current play times
int adc_time = 5000; // time to wait for user to choose play times (about 10 s)
char str[20]; // string in main while loop
int begin = 0; // flag to break from function round()
unsigned char scored = 0; // marble falls in hole with LED off 
unsigned char no_scored = 0; // marble falls in hole with LED on 

// ADC interrupt, choose number of marbles
void __interrupt(high_priority) ISR (){
    // ADC step4
    int value = ADRESH;
    int range = value/85; // 3 range
    
    int temp = 0; 
    if(range == 0){
        temp = 4;
    }else if(range ==1){
        temp = 5;
    }else{
        temp = 6;
    }
    
    char s[50];
    if(temp != cur_play_times){
        sprintf(s, "\nI would like to use %d marbles!\r", temp);
        UART_Write_Text(s);
        cur_play_times = temp;
    }
    adc_time --;
    // clear flag bit
    PIR1bits.ADIF = 0;
    // ADC step5 & go back to step3
    ADCON0bits.GO = 1;
    
    // start game after 10 seconds
    if(adc_time <= 0){
        ADCON0bits.GO = 0;
        PIR1bits.ADIF = 0;
        UART_Write_Text("\nYou can start the game now.\n\r");
        UART_Write_Text("Enjoy your game, type [start] to continue...\n\n\r");
    }
    return;
    __delay_us(4);
}

// shuffle LED lights array 
void shuffle(int *array, size_t n)
{   
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}
// starting instructions
void start()
{
    UART_Write_Text("\nWelcome to our pinball game!!\n\r");
    UART_Write_Text("Please follow our instructions to play\n\r");
    UART_Write_Text("Now you have 10 seconds to choose the playing rounds, \n\r");
    UART_Write_Text("use the Variable Resister...\n\n\r");
    ClearBuffer();
}
// record current score
void count_score(){
    scored = 0;
    no_scored = 0;
    
    while(1){
        if(PORTBbits.RB0 == 0){ // marble rolled into hole
            if(LATDbits.LATD0==1) // LED is on
                scored++;
            else{ // LED is off
                no_scored++;
            }
             char txt[30];
             sprintf(txt, "\n 0 scored = %d, not_scored = %d\r", scored, no_scored);
             UART_Write_Text(txt);
             __delay_us(500);
        }
        if(PORTBbits.RB1 == 0){
            if(LATDbits.LATD1==1)
                scored++;
            else{
                no_scored++;
            }
             char txt[30];
             sprintf(txt, "\n 1 scored = %d, not_scored = %d\r", scored, no_scored);
             UART_Write_Text(txt);
             __delay_us(500);
        }
        if(PORTBbits.RB2 == 0){
            if(LATDbits.LATD2==1)
                scored++;
            else{
                no_scored++;
            }
             char txt[30];
             sprintf(txt, "\n 2 scored = %d, not_scored = %d\r", scored, no_scored);
             UART_Write_Text(txt);
             __delay_us(500);
        }
        if(PORTBbits.RB3 == 0){
            if(LATDbits.LATD3==1)
                scored++;
            else{
                no_scored++;
            }
             char txt[30];
             sprintf(txt, "\n 3 scored = %d, not_scored = %d\r", scored, no_scored);
             UART_Write_Text(txt);
             __delay_us(500);
        }
        if(PORTBbits.RB4 == 0){
            if(LATDbits.LATD4==1)
                scored++;
            else{
                no_scored++;
            }
             char txt[30];
             sprintf(txt, "\n 4 scored = %d, not_scored = %d\r", scored, no_scored);
             UART_Write_Text(txt);
             __delay_us(500);
        }
        if(PORTBbits.RB5 == 0){
            if(LATDbits.LATD5==1)
                scored++;
            else{
                no_scored++;
            }
             char txt[30];
             sprintf(txt, "\n 5 scored = %d, not_scored = %d\r", scored, no_scored);
             UART_Write_Text(txt);
             __delay_us(500);
        }
        if(PORTEbits.RE0 == 0){
            if(LATDbits.LATD6==1)
                scored++;
            else{
                no_scored++;
            }
             char txt[30];
             sprintf(txt, "\n 6 scored = %d, not_scored = %d\r", scored, no_scored);
             UART_Write_Text(txt);
             __delay_us(500);
        }
        int seven = 0;
        if(PORTEbits.RE1 == 0){
            if(LATDbits.LATD7 == 1){
                seven = 1;
            }
            else{
                seven = -1;
            }
        }
        if(seven!=0){
            if(seven = 1)
                scored++;
            else if(seven = -1)
                no_scored++;
            char txt[30];
            sprintf(txt, "\n 7 scored = %d, not_scored = %d %d\r", scored, no_scored, seven);
            UART_Write_Text(txt);
            __delay_us(500);
        }
        // break, end of round
        if((no_scored + scored) >= cur_play_times){
            UART_Write_Text("\n\r");
            break;
        }
        __delay_us(1000);
    }
    ClearBuffer();
}

// calculate and print results
void show_result(){
    if(difficulty == 1 && scored >= 3)
        UART_Write_Text("\n\nYOU WIN !!!!\n\r");
    else if(difficulty == 2 && scored >= 2)
        UART_Write_Text("\n\nYOU WIN !!!!\n\r");
    else if(difficulty == 3 && scored >= 1)
        UART_Write_Text("\n\nYOU WIN !!!!\n\r");
    else
        UART_Write_Text("\n\nYOU LOSER !!!!\n\r");
    //UART_Write_Text("Please Type [end] to end the game.\n\r");
    
    // end of round
    CCPR1L = 21; // 0 deg
    CCP1CONbits.DC1B = 0b10; 
    LATD = 0; // all LED off
    scored = 0; 
    no_scored = 0;
    UART_Write_Text("\nGame ended\n\r");
    UART_Write_Text("\nType [start] to play again...\n\n\r");
    ClearBuffer();
}
// one round
void round()
{
    UART_Write_Text("\n\nSelect difficulty level:\n\r");
    UART_Write_Text("Please type [level1] [level2] [level3]\n\n\r");
    ClearBuffer();
    char level_str[10];
    strcpy(level_str, GetString()); // GetString() in uart.c
    unsigned int lenStr = strlen(str);
    int leds[8] = {1,2,4,8,16,32,64,128};
    while(begin) 
    {
        strcpy(level_str, GetString()); // GetString() in uart.c
        lenStr = strlen(level_str);
        /* choose level
         * level1 -> 6 LEDs on
         * level2 -> 4 LEDs on
         * level3 -> 2 LEDs on
        */
        if(lenStr == 6 && level_str[0]=='l' && level_str[1]=='e' && level_str[2]=='v' && level_str[3]=='e' && level_str[4]=='l' && level_str[5] == '1')
        {   
            difficulty = 1;
            shuffle(leds, 8);
            LATD = leds[0] + leds[1] + leds[2] + leds[3] + leds[4] + leds[5];
            begin = 0;
            ClearBuffer();
        }else if(lenStr == 6 && level_str[0]=='l' && level_str[1]=='e' && level_str[2]=='v' && level_str[3]=='e' && level_str[4]=='l' && level_str[5] == '2')
        {   
            difficulty = 2;
            shuffle(leds, 8);
            LATD = leds[0] + leds[1] + leds[2] + leds[3];
            begin = 0;
            ClearBuffer();
        }else if(lenStr == 6 && level_str[0]=='l' && level_str[1]=='e' && level_str[2]=='v' && level_str[3]=='e' && level_str[4]=='l' && level_str[5] == '3')
        {   
            difficulty = 2;
            shuffle(leds, 8);
            LATD = leds[0] + leds[1];
            begin = 0;
            ClearBuffer();
        }
    }
}
void main(void) 
{   
    SYSTEM_Initialize() ;
    
    // Timer2 -> On, prescaler -> 16, postscale > 16
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b11; 
    T2CONbits.T2OUTPS = 0b1111;
    
    //  PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    // set motor output (CCP1/RC2)
    TRISCbits.TRISC2 = 0;
    LATCbits.LATC2 = 0;
    // Internal Oscillator Frequency, Fosc = 1 MHz, Tosc = 1 us
    /** 
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (77 + 1) * 4 * 1us * 16
     * = 5ms
     */
    PR2 = 77;
    /**
     * Duty cycle
     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
     * = (22.656) * 1us * 16
     * = 362.5 us
     */
    CCPR1L = 21; // 0 deg
    CCP1CONbits.DC1B = 0b10; 
    
    // ADC step1
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0b1110; // AN0 is analog input, others are digital
    ADCON0bits.CHS = 0b0000;  // AN0 as analog input
    ADCON2bits.ADCS = 0b000;  // find table set 000(1Mhz / 2)
    ADCON2bits.ACQT = 0b001;  // Tad = 2 us (= 1/0.5Mhz ) acquisition time
                              // acquisition time > 2.4 us (we know)
                              // ADCON2bits.ACQT = 0b010 => 2Tad = 4 us > 2.4 us
    ADCON0bits.ADON = 1;
    ADCON2bits.ADFM = 0;    //right justified (8 bit)
    
    // ADC step2
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    // set button input
    TRISBbits.RB0 = 1;
    TRISBbits.RB1 = 1;
    TRISBbits.RB2 = 1;
    TRISBbits.RB3 = 1;
    TRISBbits.RB4 = 1;
    TRISBbits.RB5 = 1;
    TRISEbits.RE0 = 1;
    TRISEbits.RE1 = 1;
    
    // set LED output
    TRISD = 0;
    LATD = 0;
    
    // print startup instructions
    start();
    
    // ADC step3, let user choose number of marbles per round
    ADCON0bits.GO = 1;
    
    while(1) {
        strcpy(str, GetString()); // GetString() in uart.c
        unsigned int lenStr = strlen(str);
        if(lenStr == 5 && str[0]=='s' && str[1]=='t' && str[2]=='a' && str[3]=='r' && str[4]=='t') // start a new round
        {   
            begin = 1;
            round(); // new round
            CCPR1L = 0x04; // -90 deg
            CCP1CONbits.DC1B = 0b01;
            count_score();
            show_result();
            ClearBuffer();
        }

    }
    return;
}