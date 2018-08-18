/*
 * File:   interrupt isr.c
 * Author: philip
 *
 * Created on 30 March 2018, 20:08
 */
#include <xc.h>
#include <stdio.h>
#include <stdbool.h>
#include "config.h"
#include "stateMachine.h"
#include "fsm.h"
#include "interrupt isr.h"   

void interrupt isr(void) 
{
   
    if(PIR1bits.TMR1IF == 1)
    {  
        //RC0 = ~RC0;
        TMR1L = 0x00;
        TMR1H = 0x00;  
        PIR1bits.TMR1IF = 0;
    }
    if(INTCONbits.TMR0IF == 1)
    { 
        fsm(&kurrent_state); // FSM for rotary encoder detecting de-bounced CW and ACW rotations to provide volume control functionality
        
        if(CW_Flag)       // clockwise movement of the knob
        { 
            count++;
            
            CW_Flag == LO;   
        }
        else if (ACW_Flag) // anti-clockwise movement of the knob
        {
            count--;
           
            ACW_Flag == LO;     
        }
        if(prev_count != count) 
        {
            printf("volume position = %d\n", count);  // display latest value of count via serial monitor
        }
        
        prev_count = count;
        TMR0 = 0xE0;
        INTCONbits.TMR0IF = 0;
    } 
    
  /*  if(IOCAFbits.IOCAF1 == 1) 
    {
        unsigned char clear_port_a = 0;
        fsm(&kurrent_state);
       
        
            if(CW_Flag == 1 || RC5_code == 0x3d01 || RC5_code == 0x3501)
            {
               RC0 = 1;
               RC4 = 0;
               CW_Flag = 0;
               count++; 
               
            }
            else if(ACW_Flag == 1 || RC5_code == 0x3d02 || RC5_code == 0x3502)
            {
                RC0 = 0;
                RC4 = 1;
                ACW_Flag = 0;
                count--;
            }
        
          
        
            if(prev_count != count) 
        {
            printf("volume position = %d\n", count);  // display latest value of count via serial monitor
        }
        
        prev_count = count;
        clear_port_a = PORTA;
        INTCONbits.IOCIF == 0;
        IOCAFbits.IOCAF1 = 0;      // clear IOC interrupt flag to allow any future interrupts
       
    }*/
    
  
    
    if(PIR1bits.TMR2IF == 1)
    {
        
        TMR2 = 0x00;
        PIR1bits.TMR2IF = 0; 
    }
     
     if(IOCBFbits.IOCBF5 == 1)
     {
         if(magic_num%2 != 0)   // first rising edge of pulse detected
         {
             if(countRC5 == 0)                 // path A
             {
                T1CONbits.TMR1ON = 1;       // start TMR1
                magic_num++;               // make an even number so %evennum = 0
                countRC5 = 1;
                IOCBPbits.IOCBP5 = 1;       // enable interrupt on rising edge
                IOCBFbits.IOCBF5 = 0;      // clear relevant interrupt flags
                INTCONbits.IOCIF = 0;  
             }
             else if(countRC5 !=0)             // path C
             {
                 
                 temp3 = TMR1H<<8 | TMR1L;    // store TMR1 at start of next pulse 
                 space_width = temp3 - pulse_width;
                 
                 if(space_width >= 1776 && space_width <= 5332)
                 {
                     pulse_type = SS;
                 }
                 else if(space_width >= 5333 && space_width <= 8888)
                 {
                     pulse_type = LS;
                 }
                 stateMachine();
                 TMR1H = 0x00;
                 TMR1L = 0x00;
                 temp1 = 0;
                 magic_num++;
                 IOCBFbits.IOCBF5 = 0;      // clear relevant interrupt flags
                 INTCONbits.IOCIF = 0;    
             }
         }
         else           // path B
         {
            
             temp2 = TMR1H<<8 | TMR1L;    // store TMR1 at start of pulse
             pulse_width = temp2; //- temp1;
             magic_num = 1;
             
             if(pulse_width >= 1776 && pulse_width <= 5332)
                 {
                     pulse_type = SP;
                 }
             else if(pulse_width >= 5333 && pulse_width <= 8888)
                 {
                     pulse_type = LP;
                 }
             stateMachine(); 
             IOCBFbits.IOCBF5 = 0;      // clear relevant interrupt flags
             INTCONbits.IOCIF = 0;
         }   
     }
}

     