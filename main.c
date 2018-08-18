/*
 * File:   main.c
 * Author: Phil
 *
 * Created on 05 August 2018, 17:41
 */


#include <xc.h>
#include <stdio.h>
#include "main.h"
#include "config.h"
#include "init_ports.h"
#include "init_TMR0.h"
#include "init_TMR1.h"
#include "init_TMR2.h"
#include "config_osc.h"
#include "set_usart.h"
#include "config_oled.h"
#include "clear_oled.h"
#include "splash_screen.h"
#include "setup_vol_scrn.h"
#include "putch.h"
#include "resetRc5.h"
#include "initIOCir.h"
#include "stateMachine.h"
#include "fsm.h"
void main(void) 
{
    init_ports();
    config_osc();
    clear_oled();
    init_TMR0();
    init_TMR1();
   // init_TMR2();
    initIOCir();
    set_usart();
    config_oled();      // oled now talking to PIC, but with garbage pixels illuminated on oled
    splash_screen();
    __delay_ms(2000);
    clear_oled();
    setup_vol_scrn();
    printf("preampV2 14/08/2018\n");

    while(1)                                    // IR sensor active low code
    {   
            while(IR == HI && has_run !=1);    // wait for first button press on RC5 handset
            while(IR == LO && has_run !=1);
            if(IR == HI && has_run !=1)
            {
                bits--;
                has_run = 1;                // this block only runs once until system is reset
                INTCONbits.IOCIE = 0;       // master switch disable for interrupt on change   
                RC5_code = RC5_code | 1<<bits; // generates initial RC5 code S1 of 0x2000
                bits--;
                RC5_code = RC5_code | 1<<bits; // generates initial RC5 code S2 of 0x1000
                bits--;                        // keep track of how many of the 14 RC5 bits have been consumed
                state = MID1;
                TMR1H = 0x00;
                TMR1L = 0x00;
                INTCONbits.IOCIE = 1;       // master switch enable for interrupt on change     
                IOCBPbits.IOCBP5 = 0;       // disable interrupt on rising edge
                IOCBNbits.IOCBN5 = 1;       // enable interrupt on falling edge
                
            }
            else if(IOCIF !=1 && (bits + 1) == 0)  // if no interrupt and all 14 RC5 bits have been consumed by state machine
            {
                printf("RC5 code = %x\n", RC5_code);
                if(RC5_code == 0x3d01 || RC5_code == 0x3501)
                {
                    count--;
                }
                else if (RC5_code == 0x3d02 || RC5_code == 0x3502)
                {
                    count++;
                }
                resetRc5();
                INTCONbits.IOCIE = 0; 
                T1CONbits.TMR1ON = 0;
            }        
    }
}
    
    
        
    

