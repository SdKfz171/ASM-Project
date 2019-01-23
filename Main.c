/*
 * =====================================================================
 * NAME         : Main.c
 *
 * Descriptions : Main routine for S3C2450
 *
 * IDE          : GCC-4.1.0
 *
 * Modification
 *    
 * =====================================================================
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "2450addr.h"
#include "libc.h"
#include "option.h"

#define FREQ 1000000

#define ASM 1



void __attribute__((interrupt("IRQ"))) RTC_TICK(void);

int tick = 0;
int game_flag = 1;
uint32_t random = 0;

#if !ASM

void ADC_Init(){
    uint32_t Prescaler = (PCLK / FREQ) - 1;
    rADCCON = 0x4C48;
    rADCMUX = 0x0;
    rADCCON = 0x1;
}

unsigned ADC_Read(){
    rADCCON = 0x4C49;
    while(!(rADCCON & 0x8000));
    return ((unsigned)rADCDAT0 & 0xfff);
}

#endif

void Main(){
    UART_INIT();
    ADC_INIT();
    RTC_INIT();
    RTC_INT_INIT();
    pISR_TICK = (unsigned)RTC_TICK;
    Uart_Printf("Program Started!!\r\n");
    
    // srand(rTCNTO0);
    random = (rand() % 4094) + 1; 
    Uart_Printf("DST_VAL : %d\r\n",random);
    while(1){
        
        uint32_t result = ADC_READ();
        if(abs(random - result) < 20){
            Uart_Printf("\r\nSuccess\r\n");
            game_flag = 1;
            tick = 0;
            random = (rand() % 4094) + 1; 
            Uart_Printf("DST_VAL : %d\r\n",random);
        }
        if(tick > 4){
            Uart_Printf("\r\nGame Over\r\n");
            Uart_Printf("\r\nIf You Want to Restart, Press the Button0\r\n");
            game_flag = 0;
            while(!BUTTON_IN());
            game_flag = 1;
            tick = 0;
            random = (rand() % 4094) + 1; 
            Uart_Printf("DST_VAL : %d\r\n",random);
        }
    }
}

void __attribute__((interrupt("IRQ"))) RTC_TICK(void){
    ClearPending1(BIT_TICK);
    if(game_flag){
        Uart_Printf("%3d sec\t", ++tick);
        if(random > ADC_READ()){
            Uart_Printf("SMALL\r\n");
        }
        else Uart_Printf("BIG\r\n");    
    }
}